#include "demo.h"

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
		msg.length = MSG_ReadLong();

		printf("Reading: %d bytes\n", msg.length);
		if (msg.length == -1) {
			break;
		}

		fread(msg.data, msg.length, 1, fp);

		// process each msg here

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
