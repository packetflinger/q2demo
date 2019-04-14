#include "demo.h"

void ProcessServerMessage(void)
{
	uint32_t bitmask;
	uint16_t number;

	switch (MSG_ReadByte()) {
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
