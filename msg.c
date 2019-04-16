#include "demo.h"

void MSG_ReadData(void *out, size_t len)
{
	memcpy(out, &(msg.data[msg.index]), len);
	msg.index += len;
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

uint16_t MSG_ReadShort(void)
{
	return 	(msg.data[msg.index++] +
			(msg.data[msg.index++] << 8)) & 0xffff;
}

int16_t MSG_ReadWord(void)
{
	return 	(msg.data[msg.index++] +
			(msg.data[msg.index++] << 8));
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

uint8_t MSG_ReadAngle(void)
{
	return MSG_ReadByte();
}

uint16_t MSG_ReadAngle16(void)
{
	return MSG_ReadShort();
}

uint16_t MSG_ReadCoord(void)
{
	return MSG_ReadShort();
}

void MSG_ReadPos(vec3_t pos)
{
	pos[0] = MSG_ReadCoord();
	pos[1] = MSG_ReadCoord();
	pos[2] = MSG_ReadCoord();
}

void MSG_ParseDeltaEntity(const entity_state_t *from,
                          entity_state_t *to,
                          int            number,
                          int            bits,
                          msgEsFlags_t   flags)
{

    // set everything to the state we are delta'ing from
    if (!from) {
        memset(to, 0, sizeof(*to));
    } else if (to != from) {
        memcpy(to, from, sizeof(*to));
    }

    to->number = number;
    to->event = 0;

    if (!bits) {
        return;
    }

    if (bits & U_MODEL) {
        to->modelindex = MSG_ReadByte();
    }
    if (bits & U_MODEL2) {
        to->modelindex2 = MSG_ReadByte();
    }
    if (bits & U_MODEL3) {
        to->modelindex3 = MSG_ReadByte();
    }
    if (bits & U_MODEL4) {
        to->modelindex4 = MSG_ReadByte();
    }

    if (bits & U_FRAME8)
        to->frame = MSG_ReadByte();
    if (bits & U_FRAME16)
        to->frame = MSG_ReadShort();

    if ((bits & (U_SKIN8 | U_SKIN16)) == (U_SKIN8 | U_SKIN16))  //used for laser colors
        to->skinnum = MSG_ReadLong();
    else if (bits & U_SKIN8)
        to->skinnum = MSG_ReadByte();
    else if (bits & U_SKIN16)
        to->skinnum = MSG_ReadWord();

    if ((bits & (U_EFFECTS8 | U_EFFECTS16)) == (U_EFFECTS8 | U_EFFECTS16))
        to->effects = MSG_ReadLong();
    else if (bits & U_EFFECTS8)
        to->effects = MSG_ReadByte();
    else if (bits & U_EFFECTS16)
        to->effects = MSG_ReadWord();

    if ((bits & (U_RENDERFX8 | U_RENDERFX16)) == (U_RENDERFX8 | U_RENDERFX16))
        to->renderfx = MSG_ReadLong();
    else if (bits & U_RENDERFX8)
        to->renderfx = MSG_ReadByte();
    else if (bits & U_RENDERFX16)
        to->renderfx = MSG_ReadWord();

    if (bits & U_ORIGIN1) {
        to->origin[0] = MSG_ReadCoord();
    }
    if (bits & U_ORIGIN2) {
        to->origin[1] = MSG_ReadCoord();
    }
    if (bits & U_ORIGIN3) {
        to->origin[2] = MSG_ReadCoord();
    }

    if ((flags & MSG_ES_SHORTANGLES) && (bits & U_ANGLE16)) {
        if (bits & U_ANGLE1)
            to->angles[0] = MSG_ReadAngle16();
        if (bits & U_ANGLE2)
            to->angles[1] = MSG_ReadAngle16();
        if (bits & U_ANGLE3)
            to->angles[2] = MSG_ReadAngle16();
    } else {
        if (bits & U_ANGLE1)
            to->angles[0] = MSG_ReadAngle();
        if (bits & U_ANGLE2)
            to->angles[1] = MSG_ReadAngle();
        if (bits & U_ANGLE3)
            to->angles[2] = MSG_ReadAngle();
    }

    if (bits & U_OLDORIGIN) {
        MSG_ReadPos(to->old_origin);
    }

    if (bits & U_SOUND) {
        to->sound = MSG_ReadByte();
    }

    if (bits & U_EVENT) {
        to->event = MSG_ReadByte();
    }

    if (bits & U_SOLID) {
        if (flags & MSG_ES_LONGSOLID) {
            to->solid = MSG_ReadLong();
        } else {
            to->solid = MSG_ReadWord();
        }
    }
}

void MSG_ReadDir(vec3_t dir)
{
    int     b;

    b = MSG_ReadByte();
    //dir[0] = (vec_t *) bytedirs[0];
    //dir[1] = (vec_t *) bytedirs[1];
    //dir[2] = (vec_t *) bytedirs[2];
}


