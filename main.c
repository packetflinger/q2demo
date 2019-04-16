#include "demo.h"

void ProcessServerMessage(void)
{
	uint32_t cmd, bitmask, extrabits;
	uint16_t number;

	cmd = MSG_ReadByte();
	extrabits = cmd >> SVCMD_BITS;
	cmd &= SVCMD_MASK;

	switch (cmd) {
	case svc_serverdata:
		ParseServerData();
		break;

	case svc_configstring:
		ParseConfigString();
		break;

	case svc_spawnbaseline:
		bitmask = ParseEntityBitmask();
		number = ParseEntityNumber(bitmask);
		ParseBaseline(number, bitmask);
		break;

	case svc_frame:
		ParseFrame(extrabits);
		break;

	case svc_playerinfo:
		ParsePlayerstate(&frame.ps);
		break;

	case svc_packetentities:
		ParsePacketEntities();
		break;

	case svc_sound:
		ParseSound();
		break;

	case svc_print:
		ParsePrint();
		break;

	case svc_centerprint:
		ParseCenterprint();
		break;

	case svc_muzzleflash2:
	case svc_muzzleflash:
		ParseMuzzleFlash();
		break;
	}
}

void ParseDemo(const char *filename)
{
	FILE *fp;
	size_t bytesread;

	fp = fopen(filename, "rb");
	if (!fp) {
		return;
	}

	memset(&msg, 0, sizeof(msg_buffer_t));

	while (1) {
		bytesread = fread(msg.data, MSGLEN, 1, fp);
		msg.index = 0;
		msg.length = MSG_ReadLong();

		printf("Reading: %d bytes\n", msg.length);
		if (msg.length == -1) {
			break;
		}

		fread(msg.data, msg.length, 1, fp);
		msg.index = 0;

		while (msg.index < msg.length) {
			ProcessServerMessage();
		}

		memset(&msg, 0, sizeof(msg_buffer_t));
	}

	fclose(fp);
}

uint32_t main(uint32_t argc, char **argv)
{
	size_t i;
	for (i=1; i<argc; i++) {
		ParseDemo(argv[i]);
	}

	return EXIT_SUCCESS;
}
