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
