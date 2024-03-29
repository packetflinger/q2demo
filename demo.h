#ifndef DEMO_H
#define DEMO_H

#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _M_AMD64
#define Q_stricmp _strcmpi	//odd, amd64 libc is missing this...
#define Q_strncasecmp _strnicmp
#endif

#define MVD_MAGIC 			(((unsigned)('2')<<24)|(('D')<<16)|(('V')<<8)|('M'))

#define FRAME_COUNT         4
#define FRAME_MASK          (FRAME_COUNT - 1)
#define CS_NAME             0
#define CS_CDTRACK          1
#define CS_SKY              2
#define CS_SKYAXIS          3       // %f %f %f format
#define CS_SKYROTATE        4
#define CS_STATUSBAR        5       // display program string

#define CS_AIRACCEL         29      // air acceleration control
#define CS_MAXCLIENTS       30
#define CS_MAPCHECKSUM      31      // for catching cheater maps

#define CS_MODELS           32
#define CS_SOUNDS           (CS_MODELS+MAX_MODELS)
#define CS_IMAGES           (CS_SOUNDS+MAX_SOUNDS)
#define CS_LIGHTS           (CS_IMAGES+MAX_IMAGES)
#define CS_ITEMS            (CS_LIGHTS+MAX_LIGHTSTYLES)
#define CS_PLAYERSKINS      (CS_ITEMS+MAX_ITEMS)
#define CS_GENERAL          (CS_PLAYERSKINS+MAX_CLIENTS)
#define MAX_CONFIGSTRINGS   (CS_GENERAL+MAX_GENERAL)

#define MAX_CLIENTS         256     // absolute limit
#define MAX_EDICTS          1024    // must change protocol to increase more
#define MAX_LIGHTSTYLES     256
#define MAX_MODELS          256     // these are sent over the net as bytes
#define MAX_SOUNDS          256     // so they cannot be blindly increased
#define MAX_IMAGES          256
#define MAX_ITEMS           256
#define MAX_GENERAL         (MAX_CLIENTS * 2) // general config strings

#define MAX_CLIENT_NAME     16

//#define MAX_CLIENTS         256
//#define MAX_CONFIGSTRINGS   2080
#define MAX_CFGSTR_CHARS    1024
#define MAX_STRING_CHARS    2048
#define MSGLEN              4
//#define MAX_EDICTS          1024
#define MAX_STATS           32
#define MAX_MAP_AREAS       256
#define MAX_MAP_AREA_BYTES  (MAX_MAP_AREAS / 8)
#define SVCMD_BITS          5
#define SVCMD_MASK          ((1 << SVCMD_BITS) - 1)
#define CLIENTNUM_NONE      (MAX_CLIENTS - 1)
#define MAX_DEMO_CHUNK_SIZE 4096

#define OPENTDM_TIME		1572

#define COORD2SHORT(x)  ((int)((x)*8.0f))
#define SHORT2COORD(x)  ((x)*(1.0f/8))
#define ANGLE2BYTE(x)   ((int)((x)*256.0f/360)&255)
#define BYTE2ANGLE(x)   ((x)*(360.0f/256))
#define ANGLE2SHORT(x)  ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)  ((x)*(360.0f/65536))

#define NUMVERTEXNORMALS    162
#define DotProduct(x,y)         ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorCopy(a,b)     ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])


// render fx options
#define RF_FRAMELERP        64
#define RF_BEAM             128

#define VectorCompare(v1,v2)    ((v1)[0]==(v2)[0]&&(v1)[1]==(v2)[1]&&(v1)[2]==(v2)[2])
#define CROPFRAME(f)    ((f >= crop_args.start) && (f <= crop_args.end))
#define clamp(a,b,c)    ((a)<(b)?(a)=(b):(a)>(c)?(a)=(c):(a))
#define Vector4Compare(v1,v2)    ((v1)[0]==(v2)[0]&&(v1)[1]==(v2)[1]&&(v1)[2]==(v2)[2]&&(v1)[3]==(v2)[3])

// print types
#define PRINT_LOW           0       // pickup messages
#define PRINT_MEDIUM        1       // death messages
#define PRINT_HIGH          2       // critical messages
#define PRINT_CHAT          3       // chat messages

#define CREDIT_FRAME        20      // the frame to spam credit msg
typedef unsigned char       byte;

typedef float               vec_t;
typedef vec_t               vec3_t[3];


typedef struct {
	size_t         length;
	uint32_t       index;
	byte           data[0xffff];
} msg_buffer_t;

extern msg_buffer_t msg;    // for reading existing demo file
extern msg_buffer_t msg2;   // for writing new demo files

extern char buffer[0xffff];

typedef enum {
    MSG_PS_IGNORE_GUNINDEX      = (1 << 0),
    MSG_PS_IGNORE_GUNFRAMES     = (1 << 1),
    MSG_PS_IGNORE_BLEND         = (1 << 2),
    MSG_PS_IGNORE_VIEWANGLES    = (1 << 3),
    MSG_PS_IGNORE_DELTAANGLES   = (1 << 4),
    MSG_PS_IGNORE_PREDICTION    = (1 << 5),      // mutually exclusive with IGNORE_VIEWANGLES
    MSG_PS_FORCE                = (1 << 7),
    MSG_PS_REMOVE               = (1 << 8)
} msgPsFlags_t;

typedef enum {
    MSG_ES_FORCE        = (1 << 0),
    MSG_ES_NEWENTITY    = (1 << 1),
    MSG_ES_FIRSTPERSON  = (1 << 2),
    MSG_ES_LONGSOLID    = (1 << 3),
    MSG_ES_UMASK        = (1 << 4),
    MSG_ES_BEAMORIGIN   = (1 << 5),
    MSG_ES_SHORTANGLES  = (1 << 6),
    MSG_ES_REMOVE       = (1 << 7)
} msgEsFlags_t;

// try to pack the common update flags into the first byte
#define U_ORIGIN1   (1<<0)
#define U_ORIGIN2   (1<<1)
#define U_ANGLE2    (1<<2)
#define U_ANGLE3    (1<<3)
#define U_FRAME8    (1<<4)        // frame is a byte
#define U_EVENT     (1<<5)
#define U_REMOVE    (1<<6)        // REMOVE this entity, don't add it
#define U_MOREBITS1 (1<<7)        // read one additional byte

// second byte
#define U_NUMBER16  (1<<8)        // NUMBER8 is implicit if not set
#define U_ORIGIN3   (1<<9)
#define U_ANGLE1    (1<<10)
#define U_MODEL     (1<<11)
#define U_RENDERFX8 (1<<12)        // fullbright, etc
#define U_ANGLE16   (1<<13)
#define U_EFFECTS8  (1<<14)        // autorotate, trails, etc
#define U_MOREBITS2 (1<<15)        // read one additional byte

// third byte
#define U_SKIN8         (1<<16)
#define U_FRAME16       (1<<17)     // frame is a short
#define U_RENDERFX16    (1<<18)     // 8 + 16 = 32
#define U_EFFECTS16     (1<<19)     // 8 + 16 = 32
#define U_MODEL2        (1<<20)     // weapons, flags, etc
#define U_MODEL3        (1<<21)
#define U_MODEL4        (1<<22)
#define U_MOREBITS3     (1<<23)     // read one additional byte

// fourth byte
#define U_OLDORIGIN     (1<<24)     // FIXME: get rid of this
#define U_SKIN16        (1<<25)
#define U_SOUND         (1<<26)
#define U_SOLID         (1<<27)

#define PS_M_TYPE           (1<<0)
#define PS_M_ORIGIN         (1<<1)
#define PS_M_VELOCITY       (1<<2)
#define PS_M_TIME           (1<<3)
#define PS_M_FLAGS          (1<<4)
#define PS_M_GRAVITY        (1<<5)
#define PS_M_DELTA_ANGLES   (1<<6)
#define PS_VIEWOFFSET       (1<<7)

#define PS_VIEWANGLES       (1<<8)
#define PS_KICKANGLES       (1<<9)
#define PS_BLEND            (1<<10)
#define PS_FOV              (1<<11)
#define PS_WEAPONINDEX      (1<<12)
#define PS_WEAPONFRAME      (1<<13)
#define PS_RDFLAGS          (1<<14)
#define PS_RESERVED         (1<<15)

#define PS_BITS             16
#define PS_MASK             ((1<<PS_BITS)-1)

// a sound without an ent or pos will be a local only sound
#define SND_VOLUME          (1<<0)  // a byte
#define SND_ATTENUATION     (1<<1)  // a byte
#define SND_POS             (1<<2)  // three coordinates
#define SND_ENT             (1<<3)  // a short 0-2: channel, 3-12: entity
#define SND_OFFSET          (1<<4)  // a byte, msec offset from frame start

#define DEFAULT_SOUND_PACKET_VOLUME         1.0f
#define DEFAULT_SOUND_PACKET_ATTENUATION    1.0f

typedef enum {
    TE_GUNSHOT,
    TE_BLOOD,
    TE_BLASTER,
    TE_RAILTRAIL,
    TE_SHOTGUN,
    TE_EXPLOSION1,
    TE_EXPLOSION2,
    TE_ROCKET_EXPLOSION,
    TE_GRENADE_EXPLOSION,
    TE_SPARKS,
    TE_SPLASH,
    TE_BUBBLETRAIL,
    TE_SCREEN_SPARKS,
    TE_SHIELD_SPARKS,
    TE_BULLET_SPARKS,
    TE_LASER_SPARKS,
    TE_PARASITE_ATTACK,
    TE_ROCKET_EXPLOSION_WATER,
    TE_GRENADE_EXPLOSION_WATER,
    TE_MEDIC_CABLE_ATTACK,
    TE_BFG_EXPLOSION,
    TE_BFG_BIGEXPLOSION,
    TE_BOSSTPORT,           // used as '22' in a map, so DON'T RENUMBER!!!
    TE_BFG_LASER,
    TE_GRAPPLE_CABLE,
    TE_WELDING_SPARKS,
    TE_GREENBLOOD,
    TE_BLUEHYPERBLASTER,
    TE_PLASMA_EXPLOSION,
    TE_TUNNEL_SPARKS,
	//ROGUE
	TE_BLASTER2,
	TE_RAILTRAIL2,
	TE_FLAME,
	TE_LIGHTNING,
	TE_DEBUGTRAIL,
	TE_PLAIN_EXPLOSION,
	TE_FLASHLIGHT,
	TE_FORCEWALL,
	TE_HEATBEAM,
	TE_MONSTER_HEATBEAM,
	TE_STEAM,
	TE_BUBBLETRAIL2,
	TE_MOREBLOOD,
	TE_HEATBEAM_SPARKS,
	TE_HEATBEAM_STEAM,
	TE_CHAINFIST_SMOKE,
	TE_ELECTRIC_SPARKS,
	TE_TRACKER_EXPLOSION,
	TE_TELEPORT_EFFECT,
	TE_DBALL_GOAL,
	TE_WIDOWBEAMOUT,
	TE_NUKEBLAST,
	TE_WIDOWSPLASH,
	TE_EXPLOSION1_BIG,
	TE_EXPLOSION1_NP,
	TE_FLECHETTE,
//ROGUE

	TE_NUM_ENTITIES
} temp_event_t;

typedef struct {
    int     flags;
    int     index;
    int     entity;
    int     channel;
    vec3_t  pos;
    float   volume;
    float   attenuation;
    float   timeofs;
} snd_params_t;

typedef struct {
    int type;
    vec3_t pos1;
    vec3_t pos2;
    vec3_t offset;
    vec3_t dir;
    int count;
    int color;
    int entity1;
    int entity2;
    int time;
} tent_params_t;

// pmove_state_t is the information necessary for client side movement
// prediction
typedef enum {
    // can accelerate and turn
    PM_NORMAL,
    PM_SPECTATOR,
    // no acceleration or turning
    PM_DEAD,
    PM_GIB,     // different bounding box
    PM_FREEZE
} pmtype_t;


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

typedef enum {
    mvd_bad,
    mvd_nop,
    mvd_disconnect,     // reserved
    mvd_reconnect,      // reserved
    mvd_serverdata,
    mvd_configstring,
    mvd_frame,
    mvd_frame_nodelta,  // reserved
    mvd_unicast,
    mvd_unicast_r,

    // must match multicast_t order!!!
    mvd_multicast_all,
    mvd_multicast_phs,
    mvd_multicast_pvs,
    mvd_multicast_all_r,
    mvd_multicast_phs_r,
    mvd_multicast_pvs_r,

    mvd_sound,
    mvd_print,
    mvd_stufftext,      // reserved

    mvd_num_types
} mvd_ops_t;

typedef enum {
    MVF_NOMSGS      = 1,
    MVF_SINGLEPOV   = 2,
    MVF_RESERVED2   = 4
} mvd_flags_t;

typedef struct {
	uint32_t    version;
	uint32_t    count;
	byte        demo;
	char        gamedir[256];
	uint16_t    client_edict;
	char        map[256];
} serverdata_t;

struct configstring_s {
	uint16_t    index;
	char        string[MAX_CFGSTR_CHARS];
	struct configstring_s *next;
};

typedef struct configstring_s configstring_t;

// entity_state_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
typedef struct entity_state_s {
    int     number;         // edict index

    vec3_t  origin;
    vec3_t  angles;
    vec3_t  old_origin;     // for lerping
    int     modelindex;
    int     modelindex2, modelindex3, modelindex4;  // weapons, CTF flags, etc
    int     frame;
    int     skinnum;
    unsigned int        effects;        // PGM - we're filling it, so it needs to be unsigned
    int     renderfx;
    int     solid;          // for client side prediction, 8*(bits 0-4) is x/y radius
                            // 8*(bits 5-9) is z down distance, 8(bits10-15) is z up
                            // gi.linkentity sets this properly
    int     sound;          // for looping sounds, to guarantee shutoff
    int     event;          // impulse events -- muzzle flashes, footsteps, etc
                            // events only go out for a single frame, they
                            // are automatically cleared each frame
} entity_state_t;

extern entity_state_t baselines[MAX_EDICTS];

// entity and player states are pre-quantized before sending to make delta
// comparsion easier
typedef struct {
    uint16_t    number;
    int16_t     origin[3];
    int16_t     angles[3];
    int16_t     old_origin[3];
    uint8_t     modelindex;
    uint8_t     modelindex2;
    uint8_t     modelindex3;
    uint8_t     modelindex4;
    uint32_t    skinnum;
    uint32_t    effects;
    uint32_t    renderfx;
    uint32_t    solid;
    uint16_t    frame;
    uint8_t     sound;
    uint8_t     event;
} entity_packed_t;

// pmove->pm_flags
#define PMF_DUCKED          1
#define PMF_JUMP_HELD       2
#define PMF_ON_GROUND       4
#define PMF_TIME_WATERJUMP  8   // pm_time is waterjump
#define PMF_TIME_LAND       16  // pm_time is time before rejump
#define PMF_TIME_TELEPORT   32  // pm_time is non-moving time
#define PMF_NO_PREDICTION   64  // temporarily disables prediction (used for grappling hook)
#define PMF_TELEPORT_BIT    128 // used by q2pro

// this structure needs to be communicated bit-accurate
// from the server to the client to guarantee that
// prediction stays in sync, so no floats are used.
// if any part of the game code modifies this struct, it
// will result in a prediction error of some degree.
typedef struct {
    pmtype_t    pm_type;

    short       origin[3];      // 12.3
    short       velocity[3];    // 12.3
    byte        pm_flags;       // ducked, jump_held, etc
    byte        pm_time;        // each unit = 8 ms
    short       gravity;
    short       delta_angles[3];    // add to command angles to get view direction
                                    // changed by spawns, rotating objects, and teleporters
} pmove_state_t;

typedef struct {
    pmove_state_t   pmove;
    int16_t         viewangles[3];
    int8_t          viewoffset[3];
    int8_t          kick_angles[3];
    int8_t          gunangles[3];
    int8_t          gunoffset[3];
    uint8_t         gunindex;
    uint8_t         gunframe;
    uint8_t         blend[4];
    uint8_t         fov;
    uint8_t         rdflags;
    int16_t         stats[MAX_STATS];
} player_packed_t;

// player_state_t is the information needed in addition to pmove_state_t
// to rendered a view.  There will only be 10 player_state_t sent each second,
// but the number of pmove_state_t changes will be reletive to client
// frame rates
typedef struct {
    pmove_state_t   pmove;      // for prediction

    // these fields do not need to be communicated bit-precise

    vec3_t      viewangles;     // for fixed views
    vec3_t      viewoffset;     // add to pmovestate->origin
    vec3_t      kick_angles;    // add to view direction to get render angles
                                // set by weapon kicks, pain effects, etc

    vec3_t      gunangles;
    vec3_t      gunoffset;
    int         gunindex;
    int         gunframe;

    float       blend[4];       // rgba full screen effect

    float       fov;            // horizontal field of view

    int         rdflags;        // refdef flags

    short       stats[MAX_STATS];       // fast status bar updates
} player_state_t;


/**
 * Represents a SVC_FRAME server message
 *
 * Frames are sent over as "delta-compressed", meaning
 * the current frame was compared to a previous frame (almost
 * always the one immediately preceding this one) and
 * only the differences (or delta) is sent.
 *
 * This saves bandwidth and drastically reduces load by
 * not requiring the crunching of edicts that didn't change.
 *
 * A frame consists of at least 3 messages in this order:
 *  1. SVC_FRAME           (number, what we delta'd against)
 *  2. SVC_PLAYERINFO      (move, gun, hud stats, etc)
 *  3. SVC_PACKETENTITIES  (edict updates)
 *
 * Additional messages can be sent as well (prints, temp_entities,
 * sounds, flashes, etc), but are not required for each frame.
 */
typedef struct {
	// is this frame valid or not?
    bool            valid;

    // the current frame number
    uint32_t        number;

    // the frame this one is compressed against. Almost always previous frame
    uint32_t        delta;

    // I have no idea wtf areabits are for
    byte            areabits[MAX_MAP_AREA_BYTES];

    // the number of area bits
    uint32_t        areabytes;

    //
    uint32_t        suppressed;

    // not sure i need this
    uint8_t         clientNum;

    // the number of entities sent this frame (SVC_PACKETENTITIES)
    //uint16_t        numEntities;

    // maybe remove
    //uint16_t        firstEntity;

    // the SVC_PLAYERINFO sent over with this frame
    //player_state_t  ps;

    // the entire entity blob at this point in time
    // edicts sent over using SVC_PACKETENTITIES this frame
    // are decompressed into this
    //entity_state_t  *entities;

    struct configstring_s *cs;
} serverframe_t;

typedef struct {
	uint8_t level;
	char string[1024];
} serverprint_t;


typedef struct {
	char string[1024];
} stufftext_t;

typedef struct {
	char string[1024];
} layout_t;

typedef struct {
	uint16_t ent;
	uint8_t flash;
} muzzleflash_t;

typedef struct {
	char string[1024];
} centerprint_t;

/**
 * a merged (uncompressed) server frame
 */
typedef struct {
	//serverframe_t      frameinfo;

	uint32_t           framenum;
	//uint32_t           deltanum;
	int32_t            areabytes;
	uint32_t           areabits;

	player_state_t     ps;
	//player_packed_t    ps_packed;
	entity_state_t     edicts[MAX_EDICTS];
	size_t             edict_count;
	//entity_packed_t    edicts_packed[MAX_EDICTS];
	/*
	configstring_t     strings[20];
	serverprint_t      prints[20];
	centerprint_t      centerprints[5];
	snd_params_t       sounds[20];
	stufftext_t        stuffs[20];
	tent_params_t      tempents[20];
	muzzleflash_t      flashes[20];
	layout_t           layouts[20];
	*/
} frame_t;

/**
 * The entire demo is parsed (decompressed) into this structure. From there
 * it can be modified as required and re-commpressed back into a .dm2 file
 */
struct demo_s {
	// first message
	serverdata_t     serverdata;

	// initial strings sent when connecting
	configstring_t   configstrings[MAX_CONFIGSTRINGS]; // initial

	// cumulatively merged strings at a certain point in time
	//configstring_t   configstrings_merged[MAX_CONFIGSTRINGS];

	// initial entity states (positions, orientations, etc)
	entity_state_t   baselines[MAX_EDICTS];

	// cumulatively merged edicts at a certain point in time
	entity_state_t   gamestate[MAX_EDICTS];

	// one of these for each 0.1 second
	//serverframe_t    frames[0xffff];

	// the number of frames we have
	uint32_t         frame_count;

	// the current frame number;
	uint32_t         frame_current;

	// whether we're currently writing a new demo file or not
	bool             recording;

	// original demo name
	char             *filename;

	char             layout[1024];

	//player_state_t   ps;
	//player_packed_t  last_ps;
	//entity_packed_t  last_ent[MAX_EDICTS];
	//uint32_t         delta_frame_number;
	uint32_t         frame_number;
	//frame_t          current_frame; // will always contain current merged gamestate
	//frame_t          last_frame;
	frame_t          frames[4];
};


typedef struct {
	uint16_t start;
	uint16_t end;
} crop_args_t;

extern crop_args_t crop_args;

extern const vec3_t bytedirs[NUMVERTEXNORMALS];
/**
 * Command line arguments options
 */
#define OPT_VERBOSE    1
#define OPT_PRINTS     2
#define OPT_USAGE      4
#define OPT_CSTRINGS   8
#define OPT_FRAMES     16
#define OPT_LAYOUTS    32
#define OPT_JSON       64
#define OPT_CROP       128


//
// muzzle flashes / player effects
//
#define MZ_BLASTER          0
#define MZ_MACHINEGUN       1
#define MZ_SHOTGUN          2
#define MZ_CHAINGUN1        3
#define MZ_CHAINGUN2        4
#define MZ_CHAINGUN3        5
#define MZ_RAILGUN          6
#define MZ_ROCKET           7
#define MZ_GRENADE          8
#define MZ_LOGIN            9
#define MZ_LOGOUT           10
#define MZ_RESPAWN          11
#define MZ_BFG              12
#define MZ_SSHOTGUN         13
#define MZ_HYPERBLASTER     14
#define MZ_ITEMRESPAWN      15
// RAFAEL
#define MZ_IONRIPPER        16
#define MZ_BLUEHYPERBLASTER 17
#define MZ_PHALANX          18
#define MZ_SILENCED         128     // bit flag ORed with one of the above numbers

//ROGUE
#define MZ_ETF_RIFLE        30
#define MZ_UNUSED           31
#define MZ_SHOTGUN2         32
#define MZ_HEATBEAM         33
#define MZ_BLASTER2         34
#define MZ_TRACKER          35
#define MZ_NUKE1            36
#define MZ_NUKE2            37
#define MZ_NUKE4            38
#define MZ_NUKE8            39
//ROGUE

// message stuff
void       MSG_ReadData(void *out, size_t len);
uint8_t    MSG_ReadByte(void);
uint16_t   MSG_ReadShort(void);
int8_t     MSG_ReadChar(void);
int32_t    MSG_ReadLong(void);
char       *MSG_ReadString(void);
uint16_t   MSG_ReadCoord(void);
int16_t    MSG_ReadWord(void);
uint8_t    MSG_ReadAngle(void);
uint16_t   MSG_ReadAngle16(void);
void       MSG_ReadPos(vec3_t pos);
void       MSG_ReadDir(vec3_t dir);
void       MSG_ParseDeltaEntity(const entity_state_t *from,
                          entity_state_t *to,
                          int            number,
                          int            bits,
                          msgEsFlags_t   flags);

void       MSG_ChunkLength(uint32_t len, msg_buffer_t *buf);
void       MSG_WriteByte(byte b, msg_buffer_t *buf);
void       MSG_WriteShort(uint16_t s, msg_buffer_t *buf);
void       MSG_WriteLong(uint32_t l, msg_buffer_t *buf);
void       MSG_WriteString(const char *str, msg_buffer_t *buf);
void       MSG_WriteData(const void *data, size_t length, msg_buffer_t *buf);
void       MSG_PackEntity(entity_packed_t *out, const entity_state_t *in, bool short_angles);
void       MSG_WriteDeltaEntity(const entity_packed_t *from, const entity_packed_t *to, msgEsFlags_t flags, msg_buffer_t *buf);
void       MSG_WriteDeltaPlayerstate_Default(const player_packed_t *from, const player_packed_t *to, msg_buffer_t *buf);
void       MSG_PackPlayer(player_packed_t *out, const player_state_t *in);
void       MSG_WriteDir(const vec3_t dir, msg_buffer_t *buf);
void       MSG_WritePos(vec3_t pos, msg_buffer_t *buf);


// parsing stuff
void       ParseServerData(void);
void       ParseConfigString(void);
uint16_t   ParseEntityNumber(uint32_t bitmask);
uint32_t   ParseEntityBitmask(void);
void       ParseBaseline(int index, int bits);
void       ParseFrame(uint32_t extrabits);
void       ParsePlayerstate(player_state_t *to, player_state_t *from);
void       ParsePacketEntities(frame_t *to_frame, frame_t *from_frame);
void       ParseSound(void);
void       ParsePrint(void);
void       ParseCenterprint(void);
void       ParseMuzzleFlash(void);
void       ParseTempEntity(void);
void       ParseStuffText(void);
void       ParseLayout(void);
int        ParseArgs(uint32_t argc, char **argv);
void       ParseCropArgs(char *str);

// utils
void       WriteDemoFile(const char *filename);
char       *va(const char *format, ...);
const char *MZ_Name(uint32_t idx);
const char *Flash_Name(temp_event_t idx);
void       InsertCredits(void);
// writing
void       StartRecording(char *newdemoname);
void       EndRecording(void);
size_t     WriteBuffer(msg_buffer_t *in);

// multi-view demo stuff
void       MVD_ParseServerData(uint32_t extrabits);

// direction
int        DirToByte(const vec3_t dir);


extern uint32_t opt;
extern uint32_t options;
extern struct demo_s demo;

extern FILE *outfile;

// cumulatively merged configstrings.
extern configstring_t cs_merged[MAX_CONFIGSTRINGS];

// the starting strings sent on connect between serverdata and baselines
extern configstring_t cs_initial[MAX_CONFIGSTRINGS];

// cumulatively merged edicts
extern entity_state_t ents_merged[MAX_EDICTS];

// Initial state for each edict in the game.
extern entity_state_t baselines[MAX_EDICTS];

#endif
