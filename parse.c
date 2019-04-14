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



