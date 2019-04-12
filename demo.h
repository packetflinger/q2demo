#ifndef DEMO_H
#define DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CFGSTR_CHARS    1024
#define MAX_STRING_CHARS    2048
#define MSGLEN                 4

typedef unsigned char 	byte;

typedef struct {
	size_t         length;
	uint32_t       index;
	byte           data[0x0fff];
} msg_buffer_t;

msg_buffer_t msg;

typedef enum {
    svc_bad,

    // these ops are known to the game dll
    svc_muzzleflash,
    svc_muzzleflash2,
    svc_temp_entity,
    svc_layout,
    svc_inventory,

    // the rest are private to the client and server
    svc_nop,
    svc_disconnect,
    svc_reconnect,
    svc_sound,                  // <see code>
    svc_print,                  // [byte] id [string] null terminated string
    svc_stufftext,              // [string] stuffed into client's console buffer
                                // should be \n terminated
    svc_serverdata,             // [long] protocol ...
    svc_configstring,           // [short] [string]
    svc_spawnbaseline,
    svc_centerprint,            // [string] to put in center of the screen
    svc_download,               // [short] size [size bytes]
    svc_playerinfo,             // variable
    svc_packetentities,         // [...]
    svc_deltapacketentities,    // [...]
    svc_frame,

    // r1q2 specific operations
    svc_zpacket,
    svc_zdownload,
    svc_gamestate, // q2pro specific, means svc_playerupdate in r1q2
    svc_setting,

    svc_num_types
} svc_ops_t;

typedef struct {
	uint32_t    version;
	uint32_t    count;
	byte        demo;
	char        *gamedir;
	byte        client_edict;
	char        *map;
} srv_data_t;

typedef struct {
	uint16_t    index;
	char        string[MAX_CFGSTR_CHARS];
} srv_configstring_t;

uint8_t MSG_ReadByte(void);
int16_t MSG_ReadShort(void);
int32_t MSG_ReadLong(void);
char *MSG_ReadString(void);

void ParseServerData(void);
void ParseConfigString(void);

#endif
