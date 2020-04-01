#include "demo.h"

const entity_packed_t   nullEntityState;

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

	for (i=0; i<=len; i++) {
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


void MSG_WriteByte(byte b, msg_buffer_t *buf)
{
	buf->data[buf->index] = b;
	buf->index++;
	buf->length++;
}

void MSG_WriteShort(uint16_t s, msg_buffer_t *buf)
{
	buf->data[buf->index++] = s & 0xff;
	buf->data[buf->index++] = s >> 8;
	buf->length += 2;
}

void MSG_WriteLong(uint32_t l, msg_buffer_t *buf)
{
	buf->data[buf->index++] = l & 0xff;
	buf->data[buf->index++] = (l >> 8) & 0xff;
	buf->data[buf->index++] = (l >> 16) & 0xff;
	buf->data[buf->index++] = l >> 24;
	buf->length += 4;
}

void MSG_WriteString(const char *str, msg_buffer_t *buf)
{
	size_t len;

	if (!str) {
		MSG_WriteByte(0, buf);
		return;
	}

	len = strlen(str);

	MSG_WriteData(str, len + 1, buf);
}

void MSG_WriteData(const void *data, size_t length, msg_buffer_t *buf)
{
	uint32_t i;
	for (i=0; i<length; i++) {
		MSG_WriteByte(((byte *) data)[i], buf);
	}
}

// write the length of the current chunk into a msg buffer
void MSG_ChunkLength(uint32_t len, msg_buffer_t *buf)
{
	buf->data[buf->index++] = len & 0xff;
	buf->data[buf->index++] = (len >> 8) & 0xff;
	buf->data[buf->index++] = (len >> 16) & 0xff;
	buf->data[buf->index++] = len >> 24;
	buf->length += 4;
}

void MSG_PackEntity(entity_packed_t *out, const entity_state_t *in, bool short_angles)
{
    // allow 0 to accomodate empty baselines
    if (in->number < 0 || in->number >= MAX_EDICTS) {
        //Com_Error(ERR_DROP, "%s: bad number: %d", __func__, in->number);
    }

    out->number = in->number;
    out->origin[0] = COORD2SHORT(in->origin[0]);
    out->origin[1] = COORD2SHORT(in->origin[1]);
    out->origin[2] = COORD2SHORT(in->origin[2]);
    if (short_angles) {
        out->angles[0] = ANGLE2SHORT(in->angles[0]);
        out->angles[1] = ANGLE2SHORT(in->angles[1]);
        out->angles[2] = ANGLE2SHORT(in->angles[2]);
    } else {
        // pack angles8 akin to angles16 to make delta compression happy when
        // precision suddenly changes between entity updates
        out->angles[0] = ANGLE2BYTE(in->angles[0]) << 8;
        out->angles[1] = ANGLE2BYTE(in->angles[1]) << 8;
        out->angles[2] = ANGLE2BYTE(in->angles[2]) << 8;
    }
    out->old_origin[0] = COORD2SHORT(in->old_origin[0]);
    out->old_origin[1] = COORD2SHORT(in->old_origin[1]);
    out->old_origin[2] = COORD2SHORT(in->old_origin[2]);
    out->modelindex = in->modelindex;
    out->modelindex2 = in->modelindex2;
    out->modelindex3 = in->modelindex3;
    out->modelindex4 = in->modelindex4;
    out->skinnum = in->skinnum;
    out->effects = in->effects;
    out->renderfx = in->renderfx;
    out->solid = in->solid;
    out->frame = in->frame;
    out->sound = in->sound;
    out->event = in->event;
}

void MSG_WriteDeltaEntity(const entity_packed_t *from,
                          const entity_packed_t *to,
                          msgEsFlags_t          flags,
						  msg_buffer_t          *buf)
{
    uint32_t    bits, mask;

    if (!to) {
        if (!from) {}
            //Com_Error(ERR_DROP, "%s: NULL", __func__);

        if (from->number < 1 || from->number >= MAX_EDICTS) {}
            //Com_Error(ERR_DROP, "%s: bad number: %d", __func__, from->number);

        bits = U_REMOVE;
        if (from->number & 0xff00)
            bits |= U_NUMBER16 | U_MOREBITS1;

        MSG_WriteByte(bits & 255, buf);
        if (bits & 0x0000ff00)
            MSG_WriteByte((bits >> 8) & 255, buf);

        if (bits & U_NUMBER16)
            MSG_WriteShort(from->number, buf);
        else
            MSG_WriteByte(from->number, buf);

        return; // remove entity
    }

    if (to->number < 1 || to->number >= MAX_EDICTS) {}
        //Com_Error(ERR_DROP, "%s: bad number: %d", __func__, to->number);

    if (!from)
        from = &nullEntityState;

// send an update
    bits = 0;

    if (!(flags & MSG_ES_FIRSTPERSON)) {
        if (to->origin[0] != from->origin[0])
            bits |= U_ORIGIN1;
        if (to->origin[1] != from->origin[1])
            bits |= U_ORIGIN2;
        if (to->origin[2] != from->origin[2])
            bits |= U_ORIGIN3;

        if (flags & MSG_ES_SHORTANGLES) {
            if (to->angles[0] != from->angles[0])
                bits |= U_ANGLE1 | U_ANGLE16;
            if (to->angles[1] != from->angles[1])
                bits |= U_ANGLE2 | U_ANGLE16;
            if (to->angles[2] != from->angles[2])
                bits |= U_ANGLE3 | U_ANGLE16;
        } else {
            if (to->angles[0] != from->angles[0])
                bits |= U_ANGLE1;
            if (to->angles[1] != from->angles[1])
                bits |= U_ANGLE2;
            if (to->angles[2] != from->angles[2])
                bits |= U_ANGLE3;
        }

        if ((flags & MSG_ES_NEWENTITY) && !VectorCompare(to->old_origin, from->origin))
            bits |= U_OLDORIGIN;
    }

    if (flags & MSG_ES_UMASK)
        mask = 0xffff0000;
    else
        mask = 0xffff8000;  // don't confuse old clients

    if (to->skinnum != from->skinnum) {
        if (to->skinnum & mask)
            bits |= U_SKIN8 | U_SKIN16;
        else if (to->skinnum & 0x0000ff00)
            bits |= U_SKIN16;
        else
            bits |= U_SKIN8;
    }

    if (to->frame != from->frame) {
        if (to->frame & 0xff00)
            bits |= U_FRAME16;
        else
            bits |= U_FRAME8;
    }

    if (to->effects != from->effects) {
        if (to->effects & mask)
            bits |= U_EFFECTS8 | U_EFFECTS16;
        else if (to->effects & 0x0000ff00)
            bits |= U_EFFECTS16;
        else
            bits |= U_EFFECTS8;
    }

    if (to->renderfx != from->renderfx) {
        if (to->renderfx & mask)
            bits |= U_RENDERFX8 | U_RENDERFX16;
        else if (to->renderfx & 0x0000ff00)
            bits |= U_RENDERFX16;
        else
            bits |= U_RENDERFX8;
    }

    if (to->solid != from->solid)
        bits |= U_SOLID;

    // event is not delta compressed, just 0 compressed
    if (to->event)
        bits |= U_EVENT;

    if (to->modelindex != from->modelindex)
        bits |= U_MODEL;
    if (to->modelindex2 != from->modelindex2)
        bits |= U_MODEL2;
    if (to->modelindex3 != from->modelindex3)
        bits |= U_MODEL3;
    if (to->modelindex4 != from->modelindex4)
        bits |= U_MODEL4;

    if (to->sound != from->sound)
        bits |= U_SOUND;

    if (to->renderfx & RF_FRAMELERP) {
        bits |= U_OLDORIGIN;
    } else if (to->renderfx & RF_BEAM) {
        if (flags & MSG_ES_BEAMORIGIN) {
            if (!VectorCompare(to->old_origin, from->old_origin))
                bits |= U_OLDORIGIN;
        } else {
            bits |= U_OLDORIGIN;
        }
    }

    //
    // write the message
    //
    if (!bits && !(flags & MSG_ES_FORCE))
        return;     // nothing to send!

    if (flags & MSG_ES_REMOVE)
        bits |= U_REMOVE; // used for MVD stream only

    //----------

    if (to->number & 0xff00)
        bits |= U_NUMBER16;     // number8 is implicit otherwise

    if (bits & 0xff000000)
        bits |= U_MOREBITS3 | U_MOREBITS2 | U_MOREBITS1;
    else if (bits & 0x00ff0000)
        bits |= U_MOREBITS2 | U_MOREBITS1;
    else if (bits & 0x0000ff00)
        bits |= U_MOREBITS1;

    MSG_WriteByte(bits & 255, buf);

    if (bits & 0xff000000) {
        MSG_WriteByte((bits >> 8) & 255, buf);
        MSG_WriteByte((bits >> 16) & 255, buf);
        MSG_WriteByte((bits >> 24) & 255, buf);
    } else if (bits & 0x00ff0000) {
        MSG_WriteByte((bits >> 8) & 255, buf);
        MSG_WriteByte((bits >> 16) & 255, buf);
    } else if (bits & 0x0000ff00) {
        MSG_WriteByte((bits >> 8) & 255, buf);
    }

    //----------

    if (bits & U_NUMBER16)
        MSG_WriteShort(to->number, buf);
    else
        MSG_WriteByte(to->number, buf);

    if (bits & U_MODEL)
        MSG_WriteByte(to->modelindex, buf);
    if (bits & U_MODEL2)
        MSG_WriteByte(to->modelindex2, buf);
    if (bits & U_MODEL3)
        MSG_WriteByte(to->modelindex3, buf);
    if (bits & U_MODEL4)
        MSG_WriteByte(to->modelindex4, buf);

    if (bits & U_FRAME8)
        MSG_WriteByte(to->frame, buf);
    else if (bits & U_FRAME16)
        MSG_WriteShort(to->frame, buf);

    if ((bits & (U_SKIN8 | U_SKIN16)) == (U_SKIN8 | U_SKIN16))  //used for laser colors
        MSG_WriteLong(to->skinnum, buf);
    else if (bits & U_SKIN8)
        MSG_WriteByte(to->skinnum, buf);
    else if (bits & U_SKIN16)
        MSG_WriteShort(to->skinnum, buf);

    if ((bits & (U_EFFECTS8 | U_EFFECTS16)) == (U_EFFECTS8 | U_EFFECTS16))
        MSG_WriteLong(to->effects, buf);
    else if (bits & U_EFFECTS8)
        MSG_WriteByte(to->effects, buf);
    else if (bits & U_EFFECTS16)
        MSG_WriteShort(to->effects, buf);

    if ((bits & (U_RENDERFX8 | U_RENDERFX16)) == (U_RENDERFX8 | U_RENDERFX16))
        MSG_WriteLong(to->renderfx, buf);
    else if (bits & U_RENDERFX8)
        MSG_WriteByte(to->renderfx, buf);
    else if (bits & U_RENDERFX16)
        MSG_WriteShort(to->renderfx, buf);

    if (bits & U_ORIGIN1)
        MSG_WriteShort(to->origin[0], buf);
    if (bits & U_ORIGIN2)
        MSG_WriteShort(to->origin[1], buf);
    if (bits & U_ORIGIN3)
        MSG_WriteShort(to->origin[2], buf);

    if ((flags & MSG_ES_SHORTANGLES) && (bits & U_ANGLE16)) {
        if (bits & U_ANGLE1)
            MSG_WriteShort(to->angles[0], buf);
        if (bits & U_ANGLE2)
            MSG_WriteShort(to->angles[1], buf);
        if (bits & U_ANGLE3)
            MSG_WriteShort(to->angles[2], buf);
    } else {
        if (bits & U_ANGLE1)
            MSG_WriteByte(to->angles[0] >> 8, buf);
        if (bits & U_ANGLE2)
            MSG_WriteByte(to->angles[1] >> 8, buf);
        if (bits & U_ANGLE3)
            MSG_WriteByte(to->angles[2] >> 8, buf);
    }

    if (bits & U_OLDORIGIN) {
        MSG_WriteShort(to->old_origin[0], buf);
        MSG_WriteShort(to->old_origin[1], buf);
        MSG_WriteShort(to->old_origin[2], buf);
    }

    if (bits & U_SOUND)
        MSG_WriteByte(to->sound, buf);
    if (bits & U_EVENT)
        MSG_WriteByte(to->event, buf);
    if (bits & U_SOLID) {
        if (flags & MSG_ES_LONGSOLID)
            MSG_WriteLong(to->solid, buf);
        else
            MSG_WriteShort(to->solid, buf);
    }
}
