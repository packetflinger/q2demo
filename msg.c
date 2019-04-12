#include "demo.h"

byte *MSG_ReadData(size_t len)
{
    byte *buf = msg.data;
/*
    msg_read.readcount += len;
    msg_read.bitpos = msg_read.readcount << 3;

    if (msg_read.readcount > msg_read.cursize) {
        return NULL;
    }
*/
    return buf;
}

// unsigned
uint8_t MSG_ReadByte(void)
{
	unsigned char b = msg.data[msg.index];
	msg.index++;
	return b & 0xff;
}

// signed
int8_t MSG_ReadChar(void)
{
    signed char c = msg.data[msg.index];
    msg.index++;
    return c;
}

int16_t MSG_ReadShort(void)
{

	return 	(msg.data[msg.index++] +
			(msg.data[msg.index++] << 8)) & 0xff;
}


int32_t MSG_ReadLong(void)
{
	return 	msg.data[msg.index++] +
			(msg.data[msg.index++] << 8) +
			(msg.data[msg.index++] << 16) +
			(msg.data[msg.index++] << 24);
}

char *MSG_ReadString(void)
{
	static char str[MAX_STRING_CHARS];
	static char character;
	size_t i, len = 0;

	do {
		len++;
	} while (msg.data[(msg.index + len)] != 0);

	memset(&str, 0, MAX_STRING_CHARS);

	for (i=0; i<len; i++) {
		character = MSG_ReadByte() & 0x7f;
		strcat(str,  &character);
	}

	return str;
}
