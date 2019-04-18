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

	case svc_temp_entity:
		ParseTempEntity();
		break;

	case svc_stufftext:
		ParseStuffText();
		break;

	case svc_layout:
		ParseLayout();
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

int parseArgs(uint32_t argc, char **argv) {

	uint16_t i;
	options = 0;

	while ((opt = getopt(argc, argv, "vphcfl")) != -1) {
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

		case 'h':
			options = OPT_USAGE;
			break;

		case '?':
			printf("unknown option: %c\n", optopt);
			break;
		}
	}

	if (options & OPT_USAGE) {
		printf("Usage: %s [args] <demofilename>\n", argv[0]);
		printf("Args -\n");
		printf("  -h (this help message)\n");
		printf("  -p (output only server print message (chat, obituaries, etc)\n");
		printf("  -v (output verbose parsing information - each message parsed)\n\n");
		exit(EXIT_SUCCESS);
	}

	return optind;
}

uint32_t main(uint32_t argc, char **argv)
{
	int i;

	i = parseArgs(argc, argv);

	for (; i < argc; i++) {
		ParseDemo(argv[i]);
	}

	return EXIT_SUCCESS;
}
