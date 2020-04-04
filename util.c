#include "demo.h"

const char *MZ_Name(uint32_t idx) {
	switch (idx) {
	case MZ_BLASTER:
		return "blaster";
	case MZ_MACHINEGUN:
		return "machinegun";
	case MZ_SHOTGUN:
		return "shotgun";
	case MZ_CHAINGUN1:
		return "chaingun[1]";
	case MZ_CHAINGUN2:
		return "chaingun[2]";
	case MZ_CHAINGUN3:
		return "chaingun[3]";
	case MZ_RAILGUN:
		return "railgun";
	case MZ_ROCKET:
		return "rocket";
	case MZ_GRENADE:
		return "grenade";
	case MZ_LOGIN:
		return "spawn";
	case MZ_LOGOUT:
		return "despawn";
	case MZ_RESPAWN:
		return "player respawn";
	case MZ_BFG:
		return "bfg";
	case MZ_SSHOTGUN:
		return "super shotgun";
	case MZ_HYPERBLASTER:
		return "hyperblaster";
	case MZ_ITEMRESPAWN:
		return "item respawn";
	case MZ_IONRIPPER:
		return "ion ripper";
	case MZ_BLUEHYPERBLASTER:
		return "blue hyperblaster";
	case MZ_PHALANX:
		return "phalanx";
	case MZ_ETF_RIFLE:
		return "etf rifle";
	case MZ_SHOTGUN2:
		return "shotgun2";
	case MZ_HEATBEAM:
		return "heatbeam";
	case MZ_BLASTER2:
		return "blaster2";
	case MZ_TRACKER:
		return "tracker";
	case MZ_NUKE1:
		return "nuke[1]";
	case MZ_NUKE2:
		return "nuke[2]";
	case MZ_NUKE4:
		return "nuke[4]";
	case MZ_NUKE8:
		return "nuke[8]";
	default:
		return "unknown";
	}
}

const char *Flash_Name(temp_event_t idx) {
	switch (idx) {
	case TE_GUNSHOT:
		return "gunshot";
	case TE_BLOOD:
		return "blood";
	case TE_BLASTER:
		return "blaster";
	case TE_RAILTRAIL:
		return "rail trail";
	case TE_SHOTGUN:
		return "shotgun";
	case TE_EXPLOSION1:
		return "explosion[1]";
	case TE_EXPLOSION2:
		return "explosion[2]";
	case TE_ROCKET_EXPLOSION:
		return "rocket explosion";
	case TE_GRENADE_EXPLOSION:
		return "grenade explosion";
	case TE_SPARKS:
		return "sparks";
	case TE_SPLASH:
		return "splash";
	case TE_BUBBLETRAIL:
		return "bubble trail";
	case TE_SCREEN_SPARKS:
		return "screen sparks";
	case TE_SHIELD_SPARKS:
		return "shield sparks";
	case TE_BULLET_SPARKS:
		return "bullet sparks";
	case TE_LASER_SPARKS:
		return "laser sparks";
	case TE_PARASITE_ATTACK:
		return "parasite attack";
	case TE_ROCKET_EXPLOSION_WATER:
		return "underwater rocket explosion";
	case TE_GRENADE_EXPLOSION_WATER:
		return "underwater grenade explosion";
	case TE_MEDIC_CABLE_ATTACK:
		return "medic cable attack";
	case TE_BFG_EXPLOSION:
		return "bfg explosion";
	case TE_BFG_BIGEXPLOSION:
		return "bfg big explosion";
	case TE_BOSSTPORT:
		return "boss teleport";
	case TE_BFG_LASER:
		return "bfg laser";
	case TE_GRAPPLE_CABLE:
		return "grapple cable";
	case TE_WELDING_SPARKS:
		return "welding sparks";
	case TE_GREENBLOOD:
		return "green blood";
	case TE_BLUEHYPERBLASTER:
		return "blue hyperblaster";
	case TE_PLASMA_EXPLOSION:
		return "plasma explosion";
	case TE_TUNNEL_SPARKS:
		return "tunnel sparks";
	case TE_BLASTER2:
		return "blaster 2";
	case TE_RAILTRAIL2:
		return "rail trail 2";
	case TE_FLAME:
		return "flame";
	case TE_LIGHTNING:
		return "lightning";
	case TE_DEBUGTRAIL:
		return "debug trail";
	case TE_PLAIN_EXPLOSION:
		return "plain explosion";
	case TE_FLASHLIGHT:
		return "flashlight";
	case TE_FORCEWALL:
		return "force wall";
	case TE_HEATBEAM:
		return "heat beam";
	case TE_MONSTER_HEATBEAM:
		return "monster heat beam";
	case TE_STEAM:
		return "steam";
	case TE_BUBBLETRAIL2:
		return "bubble trail 2";
	case TE_MOREBLOOD:
		return "more blood";
	case TE_HEATBEAM_SPARKS:
		return "heat beam sparks";
	case TE_HEATBEAM_STEAM:
		return "heat beam steam";
	case TE_CHAINFIST_SMOKE:
		return "chainfist smoke";
	case TE_ELECTRIC_SPARKS:
		return "electric sparks";
	case TE_TRACKER_EXPLOSION:
		return "tracker explosion";
	case TE_TELEPORT_EFFECT:
		return "teleport effect";
	case TE_DBALL_GOAL:
		return "dodgeball goal";
	case TE_WIDOWBEAMOUT:
		return "widowbeam";
	case TE_NUKEBLAST:
		return "nuke blast";
	case TE_WIDOWSPLASH:
		return "widow splash";
	case TE_EXPLOSION1_BIG:
		return "big explosion 1";
	case TE_EXPLOSION1_NP:
		return "np explosion 1";
	case TE_FLECHETTE:
		return "flechette";
	default:
		return "unknown";
	}
}


const vec3_t bytedirs[NUMVERTEXNORMALS] = {
    {-0.525731, 0.000000, 0.850651},
    {-0.442863, 0.238856, 0.864188},
    {-0.295242, 0.000000, 0.955423},
    {-0.309017, 0.500000, 0.809017},
    {-0.162460, 0.262866, 0.951056},
    {0.000000, 0.000000, 1.000000},
    {0.000000, 0.850651, 0.525731},
    {-0.147621, 0.716567, 0.681718},
    {0.147621, 0.716567, 0.681718},
    {0.000000, 0.525731, 0.850651},
    {0.309017, 0.500000, 0.809017},
    {0.525731, 0.000000, 0.850651},
    {0.295242, 0.000000, 0.955423},
    {0.442863, 0.238856, 0.864188},
    {0.162460, 0.262866, 0.951056},
    {-0.681718, 0.147621, 0.716567},
    {-0.809017, 0.309017, 0.500000},
    {-0.587785, 0.425325, 0.688191},
    {-0.850651, 0.525731, 0.000000},
    {-0.864188, 0.442863, 0.238856},
    {-0.716567, 0.681718, 0.147621},
    {-0.688191, 0.587785, 0.425325},
    {-0.500000, 0.809017, 0.309017},
    {-0.238856, 0.864188, 0.442863},
    {-0.425325, 0.688191, 0.587785},
    {-0.716567, 0.681718, -0.147621},
    {-0.500000, 0.809017, -0.309017},
    {-0.525731, 0.850651, 0.000000},
    {0.000000, 0.850651, -0.525731},
    {-0.238856, 0.864188, -0.442863},
    {0.000000, 0.955423, -0.295242},
    {-0.262866, 0.951056, -0.162460},
    {0.000000, 1.000000, 0.000000},
    {0.000000, 0.955423, 0.295242},
    {-0.262866, 0.951056, 0.162460},
    {0.238856, 0.864188, 0.442863},
    {0.262866, 0.951056, 0.162460},
    {0.500000, 0.809017, 0.309017},
    {0.238856, 0.864188, -0.442863},
    {0.262866, 0.951056, -0.162460},
    {0.500000, 0.809017, -0.309017},
    {0.850651, 0.525731, 0.000000},
    {0.716567, 0.681718, 0.147621},
    {0.716567, 0.681718, -0.147621},
    {0.525731, 0.850651, 0.000000},
	{0.425325, 0.688191, 0.587785},
	{0.864188, 0.442863, 0.238856},
	{0.688191, 0.587785, 0.425325},
	{0.809017, 0.309017, 0.500000},
	{0.681718, 0.147621, 0.716567},
	{0.587785, 0.425325, 0.688191},
	{0.955423, 0.295242, 0.000000},
	{1.000000, 0.000000, 0.000000},
	{0.951056, 0.162460, 0.262866},
	{0.850651, -0.525731, 0.000000},
	{0.955423, -0.295242, 0.000000},
	{0.864188, -0.442863, 0.238856},
	{0.951056, -0.162460, 0.262866},
	{0.809017, -0.309017, 0.500000},
	{0.681718, -0.147621, 0.716567},
	{0.850651, 0.000000, 0.525731},
	{0.864188, 0.442863, -0.238856},
	{0.809017, 0.309017, -0.500000},
	{0.951056, 0.162460, -0.262866},
	{0.525731, 0.000000, -0.850651},
	{0.681718, 0.147621, -0.716567},
	{0.681718, -0.147621, -0.716567},
	{0.850651, 0.000000, -0.525731},
	{0.809017, -0.309017, -0.500000},
	{0.864188, -0.442863, -0.238856},
	{0.951056, -0.162460, -0.262866},
	{0.147621, 0.716567, -0.681718},
	{0.309017, 0.500000, -0.809017},
	{0.425325, 0.688191, -0.587785},
	{0.442863, 0.238856, -0.864188},
	{0.587785, 0.425325, -0.688191},
	{0.688191, 0.587785, -0.425325},
	{-0.147621, 0.716567, -0.681718},
	{-0.309017, 0.500000, -0.809017},
	{0.000000, 0.525731, -0.850651},
	{-0.525731, 0.000000, -0.850651},
	{-0.442863, 0.238856, -0.864188},
	{-0.295242, 0.000000, -0.955423},
	{-0.162460, 0.262866, -0.951056},
	{0.000000, 0.000000, -1.000000},
	{0.295242, 0.000000, -0.955423},
	{0.162460, 0.262866, -0.951056},
	{-0.442863, -0.238856, -0.864188},
	{-0.309017, -0.500000, -0.809017},
	{-0.162460, -0.262866, -0.951056},
	{0.000000, -0.850651, -0.525731},
	{-0.147621, -0.716567, -0.681718},
	{0.147621, -0.716567, -0.681718},
	{0.000000, -0.525731, -0.850651},
	{0.309017, -0.500000, -0.809017},
	{0.442863, -0.238856, -0.864188},
	{0.162460, -0.262866, -0.951056},
	{0.238856, -0.864188, -0.442863},
	{0.500000, -0.809017, -0.309017},
	{0.425325, -0.688191, -0.587785},
	{0.716567, -0.681718, -0.147621},
	{0.688191, -0.587785, -0.425325},
	{0.587785, -0.425325, -0.688191},
	{0.000000, -0.955423, -0.295242},
	{0.000000, -1.000000, 0.000000},
	{0.262866, -0.951056, -0.162460},
	{0.000000, -0.850651, 0.525731},
	{0.000000, -0.955423, 0.295242},
	{0.238856, -0.864188, 0.442863},
	{0.262866, -0.951056, 0.162460},
	{0.500000, -0.809017, 0.309017},
	{0.716567, -0.681718, 0.147621},
	{0.525731, -0.850651, 0.000000},
	{-0.238856, -0.864188, -0.442863},
	{-0.500000, -0.809017, -0.309017},
	{-0.262866, -0.951056, -0.162460},
	{-0.850651, -0.525731, 0.000000},
	{-0.716567, -0.681718, -0.147621},
	{-0.716567, -0.681718, 0.147621},
	{-0.525731, -0.850651, 0.000000},
	{-0.500000, -0.809017, 0.309017},
	{-0.238856, -0.864188, 0.442863},
	{-0.262866, -0.951056, 0.162460},
	{-0.864188, -0.442863, 0.238856},
	{-0.809017, -0.309017, 0.500000},
	{-0.688191, -0.587785, 0.425325},
	{-0.681718, -0.147621, 0.716567},
	{-0.442863, -0.238856, 0.864188},
	{-0.587785, -0.425325, 0.688191},
	{-0.309017, -0.500000, 0.809017},
	{-0.147621, -0.716567, 0.681718},
	{-0.425325, -0.688191, 0.587785},
	{-0.162460, -0.262866, 0.951056},
	{0.442863, -0.238856, 0.864188},
	{0.162460, -0.262866, 0.951056},
	{0.309017, -0.500000, 0.809017},
	{0.147621, -0.716567, 0.681718},
	{0.000000, -0.525731, 0.850651},
	{0.425325, -0.688191, 0.587785},
	{0.587785, -0.425325, 0.688191},
	{0.688191, -0.587785, 0.425325},
	{-0.955423, 0.295242, 0.000000},
	{-0.951056, 0.162460, 0.262866},
	{-1.000000, 0.000000, 0.000000},
	{-0.850651, 0.000000, 0.525731},
	{-0.955423, -0.295242, 0.000000},
	{-0.951056, -0.162460, 0.262866},
	{-0.864188, 0.442863, -0.238856},
	{-0.951056, 0.162460, -0.262866},
	{-0.809017, 0.309017, -0.500000},
	{-0.864188, -0.442863, -0.238856},
	{-0.951056, -0.162460, -0.262866},
	{-0.809017, -0.309017, -0.500000},
	{-0.681718, 0.147621, -0.716567},
	{-0.681718, -0.147621, -0.716567},
	{-0.850651, 0.000000, -0.525731},
	{-0.688191, 0.587785, -0.425325},
	{-0.587785, 0.425325, -0.688191},
	{-0.425325, 0.688191, -0.587785},
	{-0.425325, -0.688191, -0.587785},
	{-0.587785, -0.425325, -0.688191},
	{-0.688191, -0.587785, -0.425325},
};

int DirToByte(const vec3_t dir)
{
    int     i, best;
    float   d, bestd;

    if (!dir) {
        return 0;
    }

    bestd = 0;
    best = 0;
    for (i = 0; i < NUMVERTEXNORMALS; i++) {
        d = DotProduct(dir, bytedirs[i]);
        if (d > bestd) {
            bestd = d;
            best = i;
        }
    }

    return best;
}

/**
 * @brief A shorthand g_snprintf into a statically allocated buffer. Several
 * buffers are maintained internally so that nested va()'s are safe within
 * reasonable limits. This function is not thread safe.
 *
 * This version ganked from Quetoo
 */
char *va(const char *format, ...) {
        static char strings[8][MAX_STRING_CHARS];
        static uint16_t index;

        char *string = strings[index++ % 8];

        va_list args;

        va_start(args, format);
        vsnprintf(string, MAX_STRING_CHARS, format, args);
        va_end(args);

        return string;
}

#if defined _WIN32 && defined _M_AMD64
int Q_strncasecmp (const char *s1, const char *s2, size_t n)
{
	int		c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
			return 0;		// strings are equal until end point

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
			if (c1 != c2)
				return -1;		// strings not equal
		}
	} while (c1);

	return 0;		// strings are equal
}

int Q_stricmp (const char *s1, const char *s2)
{
	return Q_strncasecmp (s1, s2, 99999);
}
#endif

void InsertCredits(void) {
	MSG_WriteByte(svc_print, &msg2);
	MSG_WriteByte(PRINT_LOW, &msg2);
	MSG_WriteString("https://packetflinger.com/q2demo\n", &msg2);
}

