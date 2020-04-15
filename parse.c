#include "demo.h"

int ParseArgs(uint32_t argc, char **argv)
{

	uint16_t i;

	options = 0;

	while ((opt = getopt(argc, argv, "vphcfljx:")) != -1) {
		switch(opt) {
		case 'p':
			options |= OPT_PRINTS;
			break;

		case 'c':
			options |= OPT_CSTRINGS;
			break;

		case 'f':
			options |= OPT_FRAMES;
			break;

		case 'l':
			options |= OPT_LAYOUTS;
			break;

		case 'v':
			options = OPT_VERBOSE;
			break;

		case 'j':
			options |= OPT_JSON | OPT_VERBOSE;
			break;

		case 'h':
			options = OPT_USAGE;
			break;

		case 'x':
			options |= OPT_CROP;
			ParseCropArgs(optarg);
			break;
		case '?':
			printf("unknown option: %c\n", optopt);
			break;
		}
	}

	if ((options & OPT_USAGE) || argc == 1) {
		printf("Usage: %s [args] <demofilename> [demofilename...]\n", argv[0]);
		printf("Args:\n");
		printf("  -c (output configstrings)\n");
		printf("  -f (output frame number)\n");
		printf("  -l (output layouts)\n");
		printf("  -h (this help message)\n");
		printf("  -j (output in JSON format\n");
		printf("  -p (output only server print message (chat, obituaries, etc)\n");
		printf("  -v (output verbose parsing information - each message parsed)\n");
		printf("  -x <framestart:frameend> (crop a demo to frames xx:yy)\n\n");
		exit(EXIT_SUCCESS);
	}

	return optind;
}

/**
 * args to crop flag are given as "##:##" on the command line. Break these
 * up into separate start and stop integers
 */
void ParseCropArgs(char *str) {

	char *delimiter = ":";
	char *token;

	// no arg givn
	if (!str[0]) {
		options &= ~OPT_CROP;
		return;
	}

	// no ":" found in the arg, not valid
	if (!strstr(str, delimiter)) {
		options &= ~OPT_CROP;
		return;
	}

	token = strtok(str, delimiter);
	crop_args.start = atoi(token);

	token = strtok(NULL, delimiter);
	crop_args.end = atoi(token);

	// sanity checks
	if (crop_args.start == crop_args.end) {
		options &= ~OPT_CROP;
		return;
	}

	if (crop_args.start > crop_args.end) {
		options &= ~OPT_CROP;
		return;
	}
}

/**
 * First message, only seen once per demo
 */
void ParseServerData(void)
{
	serverdata_t *srv = &demo.serverdata;

	memset(srv, 0, sizeof(serverdata_t));

	srv->version = MSG_ReadLong();
	srv->count = MSG_ReadLong();
	srv->demo = MSG_ReadByte();
	//srv->gamedir = MSG_ReadString();
	strncpy(srv->gamedir, MSG_ReadString(), sizeof(srv->gamedir));
	srv->client_edict = MSG_ReadShort();
	//srv->map = MSG_ReadString();
	strncpy(srv->map, MSG_ReadString(), sizeof(srv->map));

	if (options & OPT_JSON) {
		//strcat(buffer, va("\"serverdata\": { \"protocol_version\": %d, \"game\": \"%s\", \"client_edict\": %d, \"map\": \"%s\" } ", data.version, data.gamedir, data.client_edict, data.map));
	} else if (options & OPT_VERBOSE) {
		strcat(buffer, "ServerData\n");
	}
}

/**
 * Strings (typically) sent to all clients
 */
void ParseConfigString(void)
{
	uint16_t index;
	char *str;

	index = MSG_ReadShort();
	str = MSG_ReadString();

	strncpy(demo.configstrings[index].string, str, MAX_CFGSTR_CHARS);

	if ((options & OPT_JSON)) {
		strcat(buffer, va("\"config\""));
	} else if ((options & OPT_VERBOSE) || (options & OPT_CSTRINGS)) {
		strcat(buffer, va("ConfigString [%d] - %s\n", index, str));
	}

	// send it to the new demo
	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_configstring, &msg2);
		MSG_WriteShort(index, &msg2);
		MSG_WriteString(str, &msg2);
	}
}


uint32_t ParseEntityBitmask(void)
{
	static uint32_t bits;

	bits = MSG_ReadByte();

	if (bits & U_MOREBITS1) {
		bits |= MSG_ReadByte() << 8;
	}

	if (bits & U_MOREBITS2) {
		bits |= MSG_ReadByte() << 16;
	}

	if (bits & U_MOREBITS3) {
		bits |= MSG_ReadByte() << 24;
	}

	return bits;
}

uint16_t ParseEntityNumber(uint32_t bitmask)
{
	static uint16_t number;

	number = (bitmask & U_NUMBER16) ? MSG_ReadShort() : MSG_ReadByte();

	return number;
}

void ParseBaseline(int index, int bits)
{
	entity_packed_t pack;

    if (index < 1 || index >= MAX_EDICTS) {
        printf("Err: Baseline index out of range\n");
    }

    MSG_ParseDeltaEntity(NULL, &demo.baselines[index], index, bits, 0);

    if (options & OPT_VERBOSE) {
    	strcat(buffer, va("Baseline [%d]\n", index));
    }
}

void ParseFrame(uint32_t extrabits)
{
	frame_t *to, *from;
	int32_t fnum, dnum, suppressed;

	demo.frame_number++;

	// pending msgs in the buffer, write them before processing the new frame
	if ((options & OPT_CROP) && demo.recording && msg2.length) {
		WriteBuffer(&msg2);
	}

	fnum = MSG_ReadLong();  // frame
	dnum = MSG_ReadLong();  // delta

	to = &demo.frames[fnum & FRAME_MASK];
	from = &demo.frames[dnum & FRAME_MASK];

	// copy previous frame to current to build on
	memcpy(to, from, sizeof(frame_t));

	to->framenum = demo.frame_number;
	suppressed = MSG_ReadByte(); // we don't care about suppressed frames
	to->areabytes = MSG_ReadByte();
	MSG_ReadData(&to->areabits, to->areabytes);

	demo.frame_current = to->framenum;

	if ((options & OPT_VERBOSE) || (options & OPT_FRAMES)) {
		strcat(buffer, va("Frame [%d]\n", to->framenum));
	}

	// start new demo file
	if ((options & OPT_CROP) && CROPFRAME(to->framenum) && !demo.recording) {
		StartRecording(va("%s-1", demo.filename));
	}

	// being 0 would mean this is the first frame,
	// if so, this frame is uncompressed, so set previous to -1
	if (from->framenum == 0) {
		from->framenum = -1;
	}

	if ((options & OPT_CROP) && demo.recording) {
		// emit frame to demo file
		MSG_WriteByte(svc_frame, &msg2);
		MSG_WriteLong(to->framenum, &msg2);
		MSG_WriteLong(from->framenum, &msg2);
		MSG_WriteByte(0, &msg2);
		MSG_WriteByte(to->areabytes, &msg2);
		MSG_WriteData(&to->areabits, to->areabytes, &msg2);
	}

	// playerstate next
	if (MSG_ReadByte() != svc_playerinfo) {
		printf("Playerstate not immediately following frame (%d), malformed demo file.", to->framenum);
		exit(EXIT_FAILURE);
	}

	if (from->framenum > 0) {
		ParsePlayerstate(&to->ps, &from->ps);
	} else {
		ParsePlayerstate(&to->ps, NULL);
	}


	// packet entities next
	if (MSG_ReadByte() != svc_packetentities) {
		printf("Packetentities not immediately following playerstate (frame %d), malformed demo.", to->framenum);
		exit(EXIT_FAILURE);
	}

	ParsePacketEntities(to, from);

	// we hit the end of the crop, stop capturing the demo
	if ((options & OPT_CROP) && demo.recording && demo.frame_current >= crop_args.end) {
		EndRecording();
	}
}

/**
 * Read and unpack the current playerstate
 */
void ParsePlayerstate(player_state_t *to, player_state_t *from)
{
	uint32_t bits;
	int i, statbits;
	player_packed_t to_p, from_p;
	//player_packed_t from = demo.last_frame.ps_packed;
	//player_packed_t *to = &demo.current_frame.ps_packed;

	bits = MSG_ReadWord();

	if (bits & PS_M_TYPE)
		to->pmove.pm_type = MSG_ReadByte();

    if (bits & PS_M_ORIGIN) {
        to->pmove.origin[0] = MSG_ReadShort();
        to->pmove.origin[1] = MSG_ReadShort();
        to->pmove.origin[2] = MSG_ReadShort();
    }

    if (bits & PS_M_VELOCITY) {
        to->pmove.velocity[0] = MSG_ReadShort();
        to->pmove.velocity[1] = MSG_ReadShort();
        to->pmove.velocity[2] = MSG_ReadShort();
    }

    if (bits & PS_M_TIME)
        to->pmove.pm_time = MSG_ReadByte();

    if (bits & PS_M_FLAGS)
        to->pmove.pm_flags = MSG_ReadByte();

    if (bits & PS_M_GRAVITY)
        to->pmove.gravity = MSG_ReadShort();

    if (bits & PS_M_DELTA_ANGLES) {
        to->pmove.delta_angles[0] = MSG_ReadShort();
        to->pmove.delta_angles[1] = MSG_ReadShort();
        to->pmove.delta_angles[2] = MSG_ReadShort();
    }

    if (bits & PS_VIEWOFFSET) {
		to->viewoffset[0] = MSG_ReadChar() * 0.25f;
		to->viewoffset[1] = MSG_ReadChar() * 0.25f;
		to->viewoffset[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_VIEWANGLES) {
		to->viewangles[0] = MSG_ReadAngle16();
		to->viewangles[1] = MSG_ReadAngle16();
		to->viewangles[2] = MSG_ReadAngle16();
	}

	if (bits & PS_KICKANGLES) {
		to->kick_angles[0] = MSG_ReadChar() * 0.25f;
		to->kick_angles[1] = MSG_ReadChar() * 0.25f;
		to->kick_angles[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_WEAPONINDEX) {
		to->gunindex = MSG_ReadByte();
	}

	if (bits & PS_WEAPONFRAME) {
		to->gunframe = MSG_ReadByte();
		to->gunoffset[0] = MSG_ReadChar() * 0.25f;
		to->gunoffset[1] = MSG_ReadChar() * 0.25f;
		to->gunoffset[2] = MSG_ReadChar() * 0.25f;
		to->gunangles[0] = MSG_ReadChar() * 0.25f;
		to->gunangles[1] = MSG_ReadChar() * 0.25f;
		to->gunangles[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_BLEND) {
		to->blend[0] = MSG_ReadByte() / 255.0f;
		to->blend[1] = MSG_ReadByte() / 255.0f;
		to->blend[2] = MSG_ReadByte() / 255.0f;
		to->blend[3] = MSG_ReadByte() / 255.0f;
	}

    if (bits & PS_FOV)
        to->fov = MSG_ReadByte();

    if (bits & PS_RDFLAGS)
        to->rdflags = MSG_ReadByte();

    statbits = MSG_ReadLong();
    for (i = 0; i < MAX_STATS; i++) {
        if (statbits & (1U << i)) {
            to->stats[i] = MSG_ReadShort();
        }
    }

    // pack the current playerstate for later comparison
    MSG_PackPlayer(&to_p, to);

    if (options & OPT_VERBOSE) {
    	strcat(buffer, "PlayerState\n");
    }

    if ((options & OPT_CROP) && demo.recording) {
    	MSG_PackPlayer(&from_p, from);
    	MSG_WriteByte(svc_playerinfo, &msg2);
    	MSG_WriteDeltaPlayerstate_Default(&from_p, &to_p, &msg2);
    }
}

void ParsePacketEntities(frame_t *to_frame, frame_t *from_frame)
{
	static uint32_t bits;
	static uint16_t num;
	uint32_t i, j, new_found, old_found, newnum, oldnum, newindex, oldindex, from_num_entities;
	entity_state_t *to, *from, *oldent, *newent;
	entity_packed_t *to_p, *from_p, oldpack, newpack;

	if (options & OPT_VERBOSE) {
		strcat(buffer, "PacketEntities - ");
	}

	while (true) {
		bits = ParseEntityBitmask();
		num = ParseEntityNumber(bits);

		from = &from_frame->edicts[num];
		to = &to_frame->edicts[num];

		if (num <= 0) {
			break;
		}

		if (from_frame->framenum == -1) {
			MSG_ParseDeltaEntity(NULL, to, num, bits, 0);
		} else {
			MSG_ParseDeltaEntity(from, to, num, bits, 0);
		}

		to_frame->edict_count++;

		// copy the newly merged entity back to the gamestate
		memcpy(&demo.gamestate[num], to, sizeof(entity_state_t));

		if (options & OPT_VERBOSE) {
			strcat(buffer, va("%d ", num));
		}
	}

	if (options & OPT_VERBOSE) {
		strcat(buffer, "\n");
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_packetentities, &msg2);

		// entities in from frame but not in to frame (remove them)
		from_num_entities = (!from) ? 0 : from_frame->edict_count;

		for (i=1, old_found=0; old_found<=from_num_entities; i++) {
			if (i >= MAX_EDICTS) {
				break;
			}
			oldent = &from_frame->edicts[i];
			newent = &to_frame->edicts[i];

			if (oldent && oldent->number) {
				old_found++;
				if (newent && newent->number == 0) {
					MSG_PackEntity(&oldpack, oldent, false);
					MSG_WriteDeltaEntity(&oldpack, NULL, MSG_ES_FORCE, &msg2);
					printf("remove - frame: %d, oldent: %d, newent: %d\n", to_frame->framenum, oldent->number, newent->number);
				}
			}
		}

		// new entities (ones in to frame but not in from frame) delta from baseline
		for (i=1, new_found=0; new_found<=to_frame->edict_count; i++) {
			if (i >= MAX_EDICTS) {
				break;
			}

			oldent = &from_frame->edicts[i];
			newent = &to_frame->edicts[i];

			// found one
			if (newent && newent->number == i) {
				new_found++;
				if (oldent && oldent->number == 0) {
					MSG_PackEntity(&oldpack, &demo.baselines[i], false);
					MSG_PackEntity(&newpack, newent, false);
					MSG_WriteDeltaEntity(&newpack, &oldpack, MSG_ES_FORCE | MSG_ES_NEWENTITY, &msg2);
					printf("add from baseline - frame: %d, oldent: %d, newent: %d\n", to_frame->framenum, oldent->number, newent->number);
				}
			}

		}

		// both found, compress against each other
		for (i=1, new_found=0; new_found<=to_frame->edict_count; i++) {
			if (i >= MAX_EDICTS) {
				break;
			}
			oldent = &from_frame->edicts[i];
			newent = &to_frame->edicts[i];

			if (newent && newent->number == i) {
				new_found++;
				if (oldent && oldent->number == i) {
					MSG_PackEntity(&oldpack, oldent, false);
					MSG_PackEntity(&newpack, newent, false);
					MSG_WriteDeltaEntity(&oldpack, &newpack,
										 newent->number <= atoi(demo.configstrings[CS_MAXCLIENTS].string) ? MSG_ES_NEWENTITY : 0, &msg2);
					printf("from previous - frame: %d, oldent: %d, newent: %d\n", to_frame->framenum, oldent->number, newent->number);
				}
			}
		}
/*
		if (!from)
			from_num_entities = 0;
		else
			from_num_entities = from_frame->edict_count;

		newindex = 0;
		oldindex = 0;
		oldent = newent = NULL;
		while (newindex < to_frame->edict_count || oldindex < from_num_entities) {
			if (newindex >= to_frame->edict_count) {
				newnum = 9999;
			} else {
				//i = (to->firstEntity + newindex) & PARSE_ENTITIES_MASK;
				// get the next edict in the frame
				i = newindex;
				while (to_frame->edicts[i].number != i && i < to_frame->edict_count) {
					i++;
				}

				//newindex = i; // remove later
				newent = &demo.gamestate[i];
				newnum = newent->number;
			}

			if (oldindex >= from_num_entities) {
				oldnum = 9999;
			} else {
				//i = (from->firstEntity + oldindex) & PARSE_ENTITIES_MASK;
				i = oldindex;
				while (!from_frame->edicts[i].number != i && i < from_frame->edict_count) {
					i++;
				}
				oldent = &demo.gamestate[i];
				oldnum = oldent->number;
			}

			printf("new index: %d, old index: %d\n", newnum, oldnum);
*/

		/*

			if (newnum == oldnum) {
				// Delta update from old position. Because the force parm is false,
				// this will not result in any bytes being emitted if the entity has
				// not changed at all. Note that players are always 'newentities',
				// this updates their old_origin always and prevents warping in case
				// of packet loss.
				MSG_PackEntity(&oldpack, oldent, false);
				MSG_PackEntity(&newpack, newent, false);
				MSG_WriteDeltaEntity(&oldpack, &newpack,
									 newent->number <= atoi(demo.configstrings[CS_MAXCLIENTS].string) ? MSG_ES_NEWENTITY : 0, &msg2);
				oldindex++;
				newindex++;
				continue;
			}

			if (newnum < oldnum) {
				// this is a new entity, send it from the baseline
				MSG_PackEntity(&oldpack, &demo.baselines[newnum], false);
				MSG_PackEntity(&newpack, newent, false);
				MSG_WriteDeltaEntity(&oldpack, &newpack, MSG_ES_FORCE | MSG_ES_NEWENTITY, &msg2);
				newindex++;
				continue;
			}

			if (newnum > oldnum) {
				// the old entity isn't present in the new message
				MSG_PackEntity(&oldpack, oldent, false);
				MSG_WriteDeltaEntity(&oldpack, NULL, MSG_ES_FORCE, &msg2);
				oldindex++;
				continue;
			}

			*/
		//}

		MSG_WriteShort(0, &msg2);
	}
}

void ParseSound(void)
{
	snd_params_t    snd;
    //int flags, channel, entity;

    snd.flags = MSG_ReadByte();
    snd.index = MSG_ReadByte();

    if (snd.flags & SND_VOLUME)
        snd.volume = MSG_ReadByte() / 255.0f;
    else
        snd.volume = DEFAULT_SOUND_PACKET_VOLUME;

    if (snd.flags & SND_ATTENUATION)
        snd.attenuation = MSG_ReadByte() / 64.0f;
    else
        snd.attenuation = DEFAULT_SOUND_PACKET_ATTENUATION;

    if (snd.flags & SND_OFFSET)
        snd.timeofs = MSG_ReadByte() / 1000.0f;
    else
        snd.timeofs = 0;

    if (snd.flags & SND_ENT) {
        // entity relative
        snd.channel = MSG_ReadShort();
        snd.entity = snd.channel >> 3;
        snd.channel = snd.channel & 7;
    } else {
        snd.entity = 0;
        snd.channel = 0;
    }

    // positioned in space
    if (snd.flags & SND_POS)
        MSG_ReadPos(snd.pos);

    //snd.flags = flags;

    if (options & OPT_VERBOSE) {
    	strcat(buffer, "Sound\n");
    }

    if ((options & OPT_CROP) && demo.recording) {
    	MSG_WriteByte(svc_sound, &msg2);
    	MSG_WriteByte(snd.flags, &msg2);
    	MSG_WriteByte(snd.index, &msg2);

    	if (snd.flags & SND_VOLUME) {
    		MSG_WriteByte(snd.volume * 255, &msg2);
    	}

    	if (snd.flags & SND_ATTENUATION) {
    		MSG_WriteByte(snd.attenuation * 64, &msg2);
    	}

    	if (snd.flags & SND_OFFSET) {
    		MSG_WriteByte(snd.timeofs * 1000, &msg2);
    	}

    	if (snd.flags & SND_ENT) {
    		MSG_WriteShort(snd.channel + (snd.entity << 3), &msg2);
    	}

    	if (snd.flags & SND_POS) {
    		MSG_WriteShort(COORD2SHORT(snd.pos[0]), &msg2);
    		MSG_WriteShort(COORD2SHORT(snd.pos[1]), &msg2);
    		MSG_WriteShort(COORD2SHORT(snd.pos[2]), &msg2);
    	}
    }
}

void ParsePrint(void)
{
	static int level;
	static char *print;
	level = MSG_ReadByte();
	print = MSG_ReadString();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("Print - %s", print)); // includes \n
	}

	if (options & OPT_PRINTS) {
		strcat(buffer, va("%s", print)); // includes\n
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_print, &msg2);
		MSG_WriteByte(level, &msg2);
		MSG_WriteString(print, &msg2);
	}
}

void ParseCenterprint(void)
{
	static char *text;
	text = MSG_ReadString();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("Centerprint - %s", text)); // include \n
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_centerprint, &msg2);
		MSG_WriteString(text, &msg2);
	}
}

void ParseMuzzleFlash(void)
{
	static uint16_t ent;
	static byte effect;

	ent = MSG_ReadShort();
	effect = MSG_ReadByte();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("Muzzleflash - (%d) %s\n", effect, MZ_Name(effect)));
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_muzzleflash, &msg2);
		MSG_WriteShort(ent, &msg2);
		MSG_WriteByte(effect, &msg2);
	}
}

void ParseTempEntity(void)
{
	tent_params_t   te;

    te.type = MSG_ReadByte();

    switch (te.type) {
    case TE_BLOOD:
    case TE_GUNSHOT:
    case TE_SPARKS:
    case TE_BULLET_SPARKS:
    case TE_SCREEN_SPARKS:
    case TE_SHIELD_SPARKS:
    case TE_SHOTGUN:
    case TE_BLASTER:
    case TE_GREENBLOOD:
    case TE_BLASTER2:
    case TE_FLECHETTE:
    case TE_HEATBEAM_SPARKS:
    case TE_HEATBEAM_STEAM:
    case TE_MOREBLOOD:
    case TE_ELECTRIC_SPARKS:
        MSG_ReadPos(te.pos1);
        MSG_ReadDir(te.dir);
        break;

    case TE_SPLASH:
    case TE_LASER_SPARKS:
    case TE_WELDING_SPARKS:
    case TE_TUNNEL_SPARKS:
        te.count = MSG_ReadByte();
        MSG_ReadPos(te.pos1);
        MSG_ReadDir(te.dir);
        te.color = MSG_ReadByte();
        break;

    case TE_BLUEHYPERBLASTER:
    case TE_RAILTRAIL:
    case TE_BUBBLETRAIL:
    case TE_DEBUGTRAIL:
    case TE_BUBBLETRAIL2:
    case TE_BFG_LASER:
        MSG_ReadPos(te.pos1);
        MSG_ReadPos(te.pos2);
        break;
    case TE_GRENADE_EXPLOSION:
	case TE_GRENADE_EXPLOSION_WATER:
	case TE_EXPLOSION2:
	case TE_PLASMA_EXPLOSION:
	case TE_ROCKET_EXPLOSION:
	case TE_ROCKET_EXPLOSION_WATER:
	case TE_EXPLOSION1:
	case TE_EXPLOSION1_NP:
	case TE_EXPLOSION1_BIG:
	case TE_BFG_EXPLOSION:
	case TE_BFG_BIGEXPLOSION:
	case TE_BOSSTPORT:
	case TE_PLAIN_EXPLOSION:
	case TE_CHAINFIST_SMOKE:
	case TE_TRACKER_EXPLOSION:
	case TE_TELEPORT_EFFECT:
	case TE_DBALL_GOAL:
	case TE_WIDOWSPLASH:
	case TE_NUKEBLAST:
		MSG_ReadPos(te.pos1);
		break;

	case TE_PARASITE_ATTACK:
	case TE_MEDIC_CABLE_ATTACK:
	case TE_HEATBEAM:
	case TE_MONSTER_HEATBEAM:
		te.entity1 = MSG_ReadShort();
		MSG_ReadPos(te.pos1);
		MSG_ReadPos(te.pos2);
		break;

	case TE_GRAPPLE_CABLE:
		te.entity1 = MSG_ReadShort();
		MSG_ReadPos(te.pos1);
		MSG_ReadPos(te.pos2);
		MSG_ReadPos(te.offset);
		break;

	case TE_LIGHTNING:
		te.entity1 = MSG_ReadShort();
		te.entity2 = MSG_ReadShort();
		MSG_ReadPos(te.pos1);
		MSG_ReadPos(te.pos2);
		break;

	case TE_FLASHLIGHT:
		MSG_ReadPos(te.pos1);
		te.entity1 = MSG_ReadShort();
		break;
	case TE_FORCEWALL:
			MSG_ReadPos(te.pos1);
			MSG_ReadPos(te.pos2);
			te.color = MSG_ReadByte();
			break;

	case TE_STEAM:
		te.entity1 = MSG_ReadShort();
		te.count = MSG_ReadByte();
		MSG_ReadPos(te.pos1);
		MSG_ReadDir(te.dir);
		te.color = MSG_ReadByte();
		te.entity2 = MSG_ReadShort();
		if (te.entity1 != -1) {
			te.time = MSG_ReadLong();
		}
		break;

	case TE_WIDOWBEAMOUT:
		te.entity1 = MSG_ReadShort();
		MSG_ReadPos(te.pos1);
		break;

	default:
		printf("TempEnt - unknown (%d)\n", te.type);
	}

    if (options & OPT_VERBOSE) {
    	strcat(buffer, va("Temporary Entity - (%d) %s\n", te.type, Flash_Name(te.type)));
    }

    if ((options & OPT_CROP) && demo.recording) {
    	MSG_WriteByte(svc_temp_entity, &msg2);
    	MSG_WriteByte(te.type, &msg2);

		switch (te.type) {
		case TE_BLOOD:
		case TE_GUNSHOT:
		case TE_SPARKS:
		case TE_BULLET_SPARKS:
		case TE_SCREEN_SPARKS:
		case TE_SHIELD_SPARKS:
		case TE_SHOTGUN:
		case TE_BLASTER:
		case TE_GREENBLOOD:
		case TE_BLASTER2:
		case TE_FLECHETTE:
		case TE_HEATBEAM_SPARKS:
		case TE_HEATBEAM_STEAM:
		case TE_MOREBLOOD:
		case TE_ELECTRIC_SPARKS:
			MSG_WritePos(te.pos1, &msg2);
			MSG_WriteDir(te.dir, &msg2);
			break;

		case TE_SPLASH:
		case TE_LASER_SPARKS:
		case TE_WELDING_SPARKS:
		case TE_TUNNEL_SPARKS:
			MSG_WriteByte(te.count, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			MSG_WriteDir(te.dir, &msg2);
			MSG_WriteByte(te.color, &msg2);
			break;

		case TE_BLUEHYPERBLASTER:
		case TE_RAILTRAIL:
		case TE_BUBBLETRAIL:
		case TE_DEBUGTRAIL:
		case TE_BUBBLETRAIL2:
		case TE_BFG_LASER:
			MSG_WritePos(te.pos1, &msg2);
			MSG_WritePos(te.pos2, &msg2);
			break;
		case TE_GRENADE_EXPLOSION:
		case TE_GRENADE_EXPLOSION_WATER:
		case TE_EXPLOSION2:
		case TE_PLASMA_EXPLOSION:
		case TE_ROCKET_EXPLOSION:
		case TE_ROCKET_EXPLOSION_WATER:
		case TE_EXPLOSION1:
		case TE_EXPLOSION1_NP:
		case TE_EXPLOSION1_BIG:
		case TE_BFG_EXPLOSION:
		case TE_BFG_BIGEXPLOSION:
		case TE_BOSSTPORT:
		case TE_PLAIN_EXPLOSION:
		case TE_CHAINFIST_SMOKE:
		case TE_TRACKER_EXPLOSION:
		case TE_TELEPORT_EFFECT:
		case TE_DBALL_GOAL:
		case TE_WIDOWSPLASH:
		case TE_NUKEBLAST:
			MSG_WritePos(te.pos1, &msg2);
			break;

		case TE_PARASITE_ATTACK:
		case TE_MEDIC_CABLE_ATTACK:
		case TE_HEATBEAM:
		case TE_MONSTER_HEATBEAM:
			MSG_WriteShort(te.entity1, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			MSG_WritePos(te.pos2, &msg2);
			break;

		case TE_GRAPPLE_CABLE:
			MSG_WriteShort(te.entity1, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			MSG_WritePos(te.pos2, &msg2);
			MSG_WritePos(te.offset, &msg2);
			break;

		case TE_LIGHTNING:
			MSG_WriteShort(te.entity1, &msg2);
			MSG_WriteShort(te.entity2, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			MSG_WritePos(te.pos2, &msg2);
			break;

		case TE_FLASHLIGHT:
			MSG_WritePos(te.pos1, &msg2);
			MSG_WriteShort(te.entity1, &msg2);
			break;
		case TE_FORCEWALL:
			MSG_WritePos(te.pos1, &msg2);
			MSG_WritePos(te.pos2, &msg2);
			MSG_WriteByte(te.color, &msg2);
			break;

		case TE_STEAM:
			MSG_WriteShort(te.entity1, &msg2);
			MSG_WriteByte(te.count, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			MSG_WriteDir(te.dir, &msg2);
			MSG_WriteByte(te.color, &msg2);
			MSG_WriteShort(te.entity2, &msg2);
			if (te.entity1 != -1) {
				MSG_WriteLong(te.time, &msg2);
			}
			break;

		case TE_WIDOWBEAMOUT:
			MSG_WriteShort(te.entity1, &msg2);
			MSG_WritePos(te.pos1, &msg2);
			break;
		}
    }
}

void ParseStuffText(void)
{
	static char *text;
	text = MSG_ReadString();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("StuffText - %s", text)); // included \n
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_stufftext, &msg2);
		MSG_WriteString(text, &msg2);
	}
}

void ParseLayout(void)
{
	strncpy(demo.layout, MSG_ReadString(), 1024);

	if (options & OPT_VERBOSE) {
		strcat(buffer, "Layout\n");
	}

	if (options & OPT_LAYOUTS) {
		strcat(buffer, ("Layout - %s\n", demo.layout));
	}

	if ((options & OPT_CROP) && demo.recording) {
		MSG_WriteByte(svc_layout, &msg2);
		MSG_WriteString(demo.layout, &msg2);
	}
}
