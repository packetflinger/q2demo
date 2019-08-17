#include "demo.h"

void ProcessMVDMessage(void)
{
	uint32_t cmd, bitmask, extrabits;
	uint16_t number;

	cmd = MSG_ReadByte();
	extrabits = cmd >> SVCMD_BITS;
	cmd &= SVCMD_MASK;

	switch (cmd) {
	case mvd_serverdata:
		MVD_ParseServerData(extrabits);
		break;
	case mvd_multicast_all:
	case mvd_multicast_pvs:
	case mvd_multicast_phs:
	case mvd_multicast_all_r:
	case mvd_multicast_pvs_r:
	case mvd_multicast_phs_r:
		MVD_ParseMulticast(cmd, extrabits);
		break;
	case mvd_unicast:
	case mvd_unicast_r:
		MVD_ParseUnicast(cmd, extrabits);
		break;
	case mvd_configstring:
		break;
	case mvd_frame:
		break;
	case mvd_sound:
		break;
	case mvd_print:
		break;
	case mvd_nop:
		break;
	default:
		printf("Invalid MVD message\n");
	}
}

/**
 * Loop through the whole demo file
 */
void ParseDemo(const char *filename)
{
	FILE *fp;
	unsigned magic;

	fp = fopen(filename, "rb");
	if (!fp) {
		return;
	}

	memset(&msg, 0, sizeof(msg_buffer_t));
	fread(msg.data, MSGLEN, 1, fp);
	magic = MSG_ReadLong();

	if (magic != MVD_MAGIC) {
		printf("Invalid format, not a multi-view demo\n");
		return;
	}

	// loop through each chunk of server messages (typically one per server frame)
	while (1) {
		memset(&msg, 0, sizeof(msg_buffer_t));
		memset(&buffer, 0, sizeof(buffer));

		// Read how long the next chunk is
		fread(msg.data, 2, 1, fp);
		msg.index = 0;
		msg.length = MSG_ReadShort();

		printf("length: %ld\n", msg.length);
		// EOF
		if (msg.length == 0) {
			break;
		}

		// read in all the msgs in that chunk
		fread(msg.data, msg.length, 1, fp);
		//fread(msg.data, 8646, 1, fp);
		msg.index = 0;

		printf("msg size: %d\n", msg.length);
		// loop through each individual message
		while (msg.index < msg.length) {
			ProcessMVDMessage();
		}

		/*
		// if we added to the buffer, output it
		if (buffer[0] != 0) {
			printf(buffer);
		}*/
	}

	fclose(fp);
}

uint32_t main(uint32_t argc, char **argv)
{
	int i;

	i = ParseArgs(argc, argv);

	for (; i < argc; i++) {
		ParseDemo(argv[i]);
	}

	return EXIT_SUCCESS;
}
