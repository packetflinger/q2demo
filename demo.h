#ifndef DEMO_H
#define DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MSGLEN	4

typedef unsigned char 	byte;

typedef struct {
	size_t         length;
	uint32_t       index;
	byte           data[0x0fff];
} msg_buffer_t;

msg_buffer_t msg;

int32_t MSG_ReadLong(void);
#endif
