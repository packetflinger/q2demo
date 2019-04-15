#include "demo.h"

void ParseServerData(void)
{
	srv_data_t data;

	data.version = MSG_ReadLong();
	data.count = MSG_ReadLong();
	data.demo = MSG_ReadByte();
	data.gamedir = MSG_ReadString();
	data.client_edict = MSG_ReadShort();
	data.map = MSG_ReadString();

	printf("ServerData (%s, %s)\n",data.gamedir, data.map);
}

void ParseConfigString(void)
{
	srv_configstring_t cs;

	cs.index = MSG_ReadShort();
	strcpy(cs.string, MSG_ReadString());

	printf("ConfigString [%d] - %s\n", cs.index, cs.string);
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

    MSG_ParseDeltaEntity(NULL, &baselines[index], index, bits, 0);

    printf("Baseline [%d]\n", index);
}

void ParseFrame(uint32_t extrabits)
{
	int suppressed, length;

	memset(&frame, 0, sizeof(server_frame_t));

	frame.number = MSG_ReadLong();
	frame.delta = MSG_ReadLong();
	suppressed = MSG_ReadByte();
	frame.areabytes = MSG_ReadByte();
	MSG_ReadData(&frame.areabits, frame.areabytes);

	printf("Frame [%d]\n", frame.number);
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

    printf("Playerstate\n");
}

void ParsePacketEntities(void)
{

}