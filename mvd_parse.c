#include "demo.h"


void MVD_ParsePacketPlayers(void) {
	uint8_t num;
	uint16_t bits;
	
	while (1) {
		num = MSG_ReadByte();
		if (num == CLIENTNUM_NONE) {
			break;
		}
		
		printf(" Playerstate [%d]\n", num);
		ParsePlayerstate(&frame.ps);
	}
}

void MVD_ParsePacketEntities(void) {
	uint16_t num;
	uint32_t bits;
	static entity_state_t nullstate;
	
	while (1) {
		bits = ParseEntityBitmask();
		num = ParseEntityNumber(bits);
		
		if (!num) {
			break;
		}
		
		MSG_ParseDeltaEntity(NULL, &nullstate, num, bits, 0);
		printf(" Entity [%d]\n", num);
	}
}


void MVD_ParseFrame(void) {
	uint8_t len;
	byte *data;
	
	printf(" Frame\n");
	
	len = MSG_ReadByte();
	msg.index += len;
	
	MVD_ParsePacketPlayers();
	MVD_ParsePacketEntities();
}

void MVD_ParseServerData(uint32_t extrabits) {
	uint32_t protocol, count, flags;
	uint16_t clientnum, index;
	char *gamedir, *cfgstr;
	
	protocol =   MSG_ReadLong();
	protocol =   MSG_ReadShort();
	count =      MSG_ReadLong();
	gamedir =    MSG_ReadString();
	clientnum =  MSG_ReadShort();
	
	flags = extrabits;
	
	printf(" ServerData\n");
	
	while (1) {
		index = MSG_ReadShort();
		
		if (index == MAX_CONFIGSTRINGS) {
			break;
		}
		
		cfgstr = MSG_ReadString();
		printf(" ConfigString - [%d] %s\n", index, cfgstr);
	}
	
	MVD_ParseFrame();
	exit(0);
}

void MVD_ParseMulticast(mvd_ops_t op, uint32_t extrabits) {
	uint32_t length, leafnum;
	
	length = MSG_ReadByte();
	length |= extrabits << 8;
	
	switch (op) {
	case mvd_multicast_pvs_r:
	case mvd_multicast_pvs:
	case mvd_multicast_phs_r:
	case mvd_multicast_phs:
		leafnum = MSG_ReadWord();
		break;
	}
	
	msg.index += length;
}

void MVD_ParseUnicast(mvd_ops_t op, uint32_t extrabits) {
	uint32_t clientNum, cmd;
	size_t length, last;
	bool reliable;
	
	length = MSG_ReadByte();
	length |= extrabits << 8;
	clientNum = MSG_ReadByte();
	
	last = msg.index + length;
	reliable = op == mvd_unicast_r;
	
	while (msg.index < last) {
		cmd = MSG_ReadByte();
		
		switch (cmd) {
			
		}
	}
}