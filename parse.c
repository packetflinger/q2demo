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

	printf("crop start: %d, crop end: %d\n", crop_args.start, crop_args.end);
}

/**
 * First message, only seen once per demo
 */
void ParseServerData(void)
{
	serverdata_t *srv = &demo.serverdata;

	srv->version = MSG_ReadLong();
	srv->count = MSG_ReadLong();
	srv->demo = MSG_ReadByte();
	srv->gamedir = MSG_ReadString();
	srv->client_edict = MSG_ReadShort();
	srv->map = MSG_ReadString();

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
	struct configstring_s *new_cs, *cs;
	serverframe_t *fr;

	uint16_t index;
	char *str;

	index = MSG_ReadShort();
	str = MSG_ReadString();


	strncpy(demo.configstrings[index].string, str, MAX_CFGSTR_CHARS);

/*
	// it's an initial cs
	if (!demo.frame_count) {
		new_cs = &demo.configstrings[index];
		new_cs->index = index;
		strncpy(new_cs->string, str, sizeof(new_cs->string));
		return;
	}

	fr = &demo.frames[demo.frame_current];
	new_cs = malloc(sizeof(struct configstring_s));

	cs = fr->cs;

	// find the last cs
	while (cs) {
		cs = cs->next;
	}

	cs = new_cs;
	cs->index = index;
	strncpy(cs->string, str, sizeof(cs->string));

*/
	if ((options & OPT_CROP) && demo.frame_current >= crop_args.start && demo.frame_current <= crop_args.end) {
		MSG_WriteShort(index);
		MSG_WriteString(str);
	}

	if ((options & OPT_JSON)) {
		strcat(buffer, va("\"config\""));
	} else if ((options & OPT_VERBOSE) || (options & OPT_CSTRINGS)) {
		strcat(buffer, va("ConfigString [%d] - %s\n", index, str));
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
	uint32_t framenum, deltanum, areabytes;
	serverframe_t *fr;

	int suppressed, length;

	framenum = MSG_ReadLong();
	deltanum = MSG_ReadLong();
	suppressed = MSG_ReadByte();
	areabytes = MSG_ReadByte();

	fr = &demo.frames[framenum];

	memset(fr, 0, sizeof(serverframe_t));
	fr->number = framenum;
	fr->delta = deltanum;
	fr->suppressed = suppressed;
	MSG_ReadData(&fr->areabits, areabytes);

	// previous frame is still in the writing buffer, write it to disk
	if (msg2.length) {

	}

	if ((options & OPT_VERBOSE) || (options & OPT_FRAMES)) {
		strcat(buffer, va("Frame [%d]\n", framenum));
	}
}

void ParsePlayerstate(player_state_t *ps)
{
	uint32_t bits;
	int i, statbits;

	bits = MSG_ReadWord();

	if (bits & PS_M_TYPE)
		ps->pmove.pm_type = MSG_ReadByte();

    if (bits & PS_M_ORIGIN) {
        ps->pmove.origin[0] = MSG_ReadShort();
        ps->pmove.origin[1] = MSG_ReadShort();
        ps->pmove.origin[2] = MSG_ReadShort();
    }

    if (bits & PS_M_VELOCITY) {
        ps->pmove.velocity[0] = MSG_ReadShort();
        ps->pmove.velocity[1] = MSG_ReadShort();
        ps->pmove.velocity[2] = MSG_ReadShort();
    }

    if (bits & PS_M_TIME)
        ps->pmove.pm_time = MSG_ReadByte();

    if (bits & PS_M_FLAGS)
        ps->pmove.pm_flags = MSG_ReadByte();

    if (bits & PS_M_GRAVITY)
        ps->pmove.gravity = MSG_ReadShort();

    if (bits & PS_M_DELTA_ANGLES) {
        ps->pmove.delta_angles[0] = MSG_ReadShort();
        ps->pmove.delta_angles[1] = MSG_ReadShort();
        ps->pmove.delta_angles[2] = MSG_ReadShort();
    }

    if (bits & PS_VIEWOFFSET) {
		ps->viewoffset[0] = MSG_ReadChar() * 0.25f;
		ps->viewoffset[1] = MSG_ReadChar() * 0.25f;
		ps->viewoffset[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_VIEWANGLES) {
		ps->viewangles[0] = MSG_ReadAngle16();
		ps->viewangles[1] = MSG_ReadAngle16();
		ps->viewangles[2] = MSG_ReadAngle16();
	}

	if (bits & PS_KICKANGLES) {
		ps->kick_angles[0] = MSG_ReadChar() * 0.25f;
		ps->kick_angles[1] = MSG_ReadChar() * 0.25f;
		ps->kick_angles[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_WEAPONINDEX) {
		ps->gunindex = MSG_ReadByte();
	}

	if (bits & PS_WEAPONFRAME) {
		ps->gunframe = MSG_ReadByte();
		ps->gunoffset[0] = MSG_ReadChar() * 0.25f;
		ps->gunoffset[1] = MSG_ReadChar() * 0.25f;
		ps->gunoffset[2] = MSG_ReadChar() * 0.25f;
		ps->gunangles[0] = MSG_ReadChar() * 0.25f;
		ps->gunangles[1] = MSG_ReadChar() * 0.25f;
		ps->gunangles[2] = MSG_ReadChar() * 0.25f;
	}

	if (bits & PS_BLEND) {
		ps->blend[0] = MSG_ReadByte() / 255.0f;
		ps->blend[1] = MSG_ReadByte() / 255.0f;
		ps->blend[2] = MSG_ReadByte() / 255.0f;
		ps->blend[3] = MSG_ReadByte() / 255.0f;
	}

    if (bits & PS_FOV)
        ps->fov = MSG_ReadByte();

    if (bits & PS_RDFLAGS)
        ps->rdflags = MSG_ReadByte();

    statbits = MSG_ReadLong();
    for (i = 0; i < MAX_STATS; i++)
        if (statbits & (1U << i))
            ps->stats[i] = MSG_ReadShort();

    if (options & OPT_VERBOSE) {
    	strcat(buffer, "PlayerState\n");
    }
}

void ParsePacketEntities(void)
{
	static uint32_t bits;
	static uint16_t num;
	static entity_state_t nullstate;

	if (options & OPT_VERBOSE) {
		strcat(buffer, "PacketEntities - ");
	}

	while (true) {
		bits = ParseEntityBitmask();
		num = ParseEntityNumber(bits);

		if (num <= 0) {
			break;
		}

		MSG_ParseDeltaEntity(NULL, &nullstate, num, bits, 0);

		if (options & OPT_VERBOSE) {
			strcat(buffer, va("%d ", num));
		}
	}

	if (options & OPT_VERBOSE) {
		strcat(buffer, "\n");
	}
}

void ParseSound(void)
{
	snd_params_t    snd;
    int flags, channel, entity;

    flags = MSG_ReadByte();
    snd.index = MSG_ReadByte();

    if (flags & SND_VOLUME)
        snd.volume = MSG_ReadByte() / 255.0f;
    else
        snd.volume = DEFAULT_SOUND_PACKET_VOLUME;

    if (flags & SND_ATTENUATION)
        snd.attenuation = MSG_ReadByte() / 64.0f;
    else
        snd.attenuation = DEFAULT_SOUND_PACKET_ATTENUATION;

    if (flags & SND_OFFSET)
        snd.timeofs = MSG_ReadByte() / 1000.0f;
    else
        snd.timeofs = 0;

    if (flags & SND_ENT) {
        // entity relative
        channel = MSG_ReadShort();
        entity = channel >> 3;
        snd.entity = entity;
        snd.channel = channel & 7;
    } else {
        snd.entity = 0;
        snd.channel = 0;
    }

    // positioned in space
    if (flags & SND_POS)
        MSG_ReadPos(snd.pos);

    snd.flags = flags;

    if (options & OPT_VERBOSE) {
    	strcat(buffer, "Sound\n");
    }
}

void ParsePrint(void)
{
	static int level;
	static char *print;
	level = MSG_ReadByte();
	print = MSG_ReadString();

	if ((options & OPT_PRINTS) || (options & OPT_VERBOSE)) {
		strcat(buffer, va("Print - %s", print)); // include \n
	}
}

void ParseCenterprint(void)
{
	static char *text;
	text = MSG_ReadString();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("Centerprint - %s", text)); // include \n
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
}

void ParseStuffText(void)
{
	static char *text;
	text = MSG_ReadString();

	if (options & OPT_VERBOSE) {
		strcat(buffer, va("StuffText - %s", text)); // included \n
	}
}

void ParseLayout(void)
{
	static char *layout;
	layout = MSG_ReadString();
	if (options & OPT_VERBOSE) {
		strcat(buffer, "Layout\n");
	}

	if (options & OPT_LAYOUTS) {
		strcat(buffer, ("Layout - %s\n", layout));
	}
}
