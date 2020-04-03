#include "demo.h"

/**
 * Write a new demo file from the demo_s structure
 */
void WriteDemoFile(const char *filename)
{
	FILE *fp;
	uint32_t i;
	msg_buffer_t chunklen;

	fp = fopen(filename, "wb");
	if (!fp) {
		return;
	}

	memset(&msg2, 0, sizeof(msg_buffer_t));

	/*
	// serverdata first
	MSG_WriteByte(svc_serverdata);
	MSG_WriteLong(demo.serverdata.version);
	MSG_WriteLong(demo.serverdata.count);
	MSG_WriteByte(demo.serverdata.demo);
	MSG_WriteString(demo.serverdata.gamedir);
	MSG_WriteShort(demo.serverdata.client_edict);
	MSG_WriteString(demo.serverdata.map);

	// then configstrings
	for (i=0; i<MAX_CONFIGSTRINGS; i++) {
		if (demo.configstrings[i].index != i) {
			continue;
		}

		MSG_WriteByte(svc_configstring);
		MSG_WriteShort(demo.configstrings[i].index);
		MSG_WriteString(demo.configstrings[i].string);
	}

	*/
	/*
	// then baselines
	for (i=0; i<MAX_EDICTS; i++) {
		if (demo.baselines[i].number != i) {
			continue;
		}

		MSG_WriteByte(svc_spawnbaseline);
		MSG_WriteShort(demo.configstrings[i].index);
		MSG_WriteString(demo.configstrings[i].string);
	}
	*/

	/*
	MSG_ChunkLength(msg2.length, &chunklen);
	fwrite(&chunklen, 1, sizeof(uint32_t), fp);
	printf("len: %d\n", msg2.length);
	fwrite(&msg2, 1, msg2.length, fp);
*/

	fclose(fp);
}

/**
 * Open the new demo file and write all the headers (serverdata, cstrings, baselines)
 */
void StartRecording(char *newdemoname) {
	uint16_t i;
	entity_packed_t pack;

	outfile = fopen(newdemoname, "wb");
	if (!outfile) {
		return;
	}

	memset(&msg2, 0, sizeof(msg_buffer_t));

	demo.recording = true;

	// serverdata first
	MSG_WriteByte(svc_serverdata, &msg2);
	MSG_WriteLong(demo.serverdata.version, &msg2);
	MSG_WriteLong(demo.serverdata.count, &msg2);
	MSG_WriteByte(demo.serverdata.demo, &msg2);
	MSG_WriteString(demo.serverdata.gamedir, &msg2);
	MSG_WriteShort(demo.serverdata.client_edict, &msg2);
	MSG_WriteString(demo.serverdata.map, &msg2);

	// then configstrings
	for (i=0; i<MAX_CONFIGSTRINGS; i++) {
		if (!demo.configstrings[i].string[0]) {
			continue;
		}
		MSG_WriteByte(svc_configstring, &msg2);
		MSG_WriteShort(i, &msg2);
		MSG_WriteString(demo.configstrings[i].string, &msg2);
	}

	// write the buffer now, it should be rather large now
	WriteBuffer(&msg2);

	// then add baslines
	for (i=0; i<MAX_EDICTS; i++) {
		if (!demo.baselines[i].number) {
			continue;
		}
		MSG_WriteByte(svc_spawnbaseline, &msg2);
		MSG_PackEntity(&pack, &demo.baselines[i], false);
		MSG_WriteDeltaEntity(NULL, &pack, 0, &msg2);
	}

	MSG_WriteByte(svc_stufftext, &msg2);
	MSG_WriteString("precache\n", &msg2);

	WriteBuffer(&msg2);

	memset(&demo.last_frame, 0, sizeof(frame_t));
}

/**
 * Tidy stuff up and close the file pointer
 */
void EndRecording(void) {
	uint32_t eof = 0xffffffff;

	if (!demo.recording) {
		return;
	}

	fwrite(&eof, sizeof(uint32_t), 1, outfile);
	fclose(outfile);

	demo.recording = false;
}

/**
 * Write a chunk of (if not the entire) output buffer to file.
 * Returns the number of bytes written
 */
size_t WriteBuffer(msg_buffer_t *in) {

	size_t ret;
	msg_buffer_t tmpmsg;

	memset(&tmpmsg, 0, sizeof(msg_buffer_t));

	MSG_WriteLong(in->length, &tmpmsg);
	MSG_WriteData(in->data, in->length, &tmpmsg);

	ret = fwrite(&tmpmsg.data, tmpmsg.length, 1, outfile);

	memset(in, 0, sizeof(msg_buffer_t));

	return ret;
}
