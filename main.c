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

/**
 * Loop through the whole demo file
 */
void ParseDemo(const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "rb");
	if (!fp) {
		return;
	}

	memset(&demo, 0, sizeof(struct demo_s));

	// save for later
	demo.filename = (void *) filename;
	//demo.delta_frame_number = -1;

	// loop through each chunk of server messages (typically one per server frame)
	while (1) {
		memset(&msg, 0, sizeof(msg_buffer_t));
		memset(&buffer, 0, sizeof(buffer));

		// Read how long the next chunk is
		fread(msg.data, MSGLEN, 1, fp);
		msg.index = 0;
		msg.length = MSG_ReadLong();

		// EOF
		if (msg.length == -1) {
			break;
		}

		// read in all the msgs in that chunk
		fread(msg.data, msg.length, 1, fp);
		msg.index = 0;

		// loop through each individual message
		while (msg.index < msg.length) {
			ProcessServerMessage();
		}

		// if we added to the buffer, output it
		if (buffer[0] != 0) {
			printf(buffer);
		}
	}

	if ((options & OPT_CROP) && demo.recording) {
		EndRecording();
	}

	fclose(fp);
}

uint32_t main(uint32_t argc, char **argv)
{
	int i = ParseArgs(argc, argv);

	for (; i < argc; i++) {
		ParseDemo(argv[i]);
	}

	return EXIT_SUCCESS;
}
