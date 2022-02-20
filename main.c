#include "demo.h"

msg_buffer_t msg;    // for reading existing demo file
msg_buffer_t msg2;   // for writing new demo files
char buffer[0xffff];
entity_state_t baselines[MAX_EDICTS];
crop_args_t crop_args;
uint32_t opt;
uint32_t options;
struct demo_s demo;

FILE *outfile;
configstring_t cs_merged[MAX_CONFIGSTRINGS];
configstring_t cs_initial[MAX_CONFIGSTRINGS];
entity_state_t ents_merged[MAX_EDICTS];
entity_state_t baselines[MAX_EDICTS];

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

	if ((options & OPT_CROP) && demo.recording) {
		if (demo.frame_number - crop_args.start == CREDIT_FRAME) {
			InsertCredits();
		}
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

	// loop through each chunk of server messages (typically one per server frame)
	while (true) {
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
