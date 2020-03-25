/*
 * name:		bg_misc.c
 *
 * desc:		both games misc functions, all completely stateless
 *
*/

#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#else
#include "q_shared.h"
#include "bg_public.h"
#endif // CGAMEDLL

#include "../../etmain/ui/menudef.h"

#ifdef CGAMEDLL // CGAME

extern	vmCvar_t		cg_gameType;
#define gametypeCvar	cg_gameType
#define BG_ADRENALINE	cgs.adrenaline
#define BG_MEDICS		cgs.medics

#elif GAMEDLL // QAGAME

extern	vmCvar_t		g_developer;
extern	vmCvar_t		g_gametype;
#define gametypeCvar	g_gametype
extern vmCvar_t			g_adrenaline;
#define BG_ADRENALINE	g_adrenaline.integer
extern vmCvar_t			g_medics;
#define BG_MEDICS		g_medics.integer

#else // UI

extern	vmCvar_t		ui_gameType;
#define gametypeCvar	ui_gameType
#define BG_ADRENALINE	0
#define BG_MEDICS		0

#endif

//------------------------------------------------------------

/*
================
return a hash value for the given string
================
*/
long BG_StringHashValue( const char *fname ) {
	int		i;
	long	hash;

	if( !fname ) {
		return -1;
	}

	hash = 0;
	i = 0;
	while (fname[i] != '\0') {
		if( Q_isupper( fname[i] ) ) {
			hash += (long)(fname[i] + ('a' - 'A'))*(i+119);
		}
		else {
			hash += (long)(fname[i])*(i+119);
		}

		i++;
	}
	if (hash == -1) {
		hash = 0;	// never return -1
	}
	return hash;
}

/*
================
return a hash value for the given string (make sure the strings and lowered first)
================
*/
long BG_StringHashValue_Lwr( const char *fname ) {
	int		i = 0;
	long	hash = 0;

	while (fname[i] != '\0') {
		hash+=(long)(fname[i])*(i+119);
		i++;
	}
	if (hash == -1) {
		hash = 0;	// never return -1
	}
	return hash;
}

float BG_GetFromTable( float* table, int *skillBits, int skillType )
{
	int i;
	float best=0.f;

	for ( i = 0; i < NUM_SKILL_LEVELS ; i ++ ) {
		if ( i == NUM_SKILL_LEVELS)
			break;

		// zero level is the default
		if ( i == 0 )
			best = table[0];
		else if ( table[i] == table[i-1] ) // not a change
			continue;

		// ok check if this is achieved
		if ( skillBits[skillType] & (1<<i) )
			best = table[i];
	}

	return best;
}

sysMessage_t HQMessages[SM_NUM_SYS_MSGS] = {
	{"SYS_NeedMedic",		 "_hq_need_medic",			"^7(HQ): ^3We need a Medic!" },
	{"SYS_NeedEngineer",	 "_hq_need_engineer",		"^7(HQ): ^3We need an Engineer!" },
	{"SYS_NeedFieldops",	 "_hq_need_fieldops",		"^7(HQ): ^3We need an Field Ops!" },
	{"SYS_NeedCovertOps",	 "_hq_need_covertops",		"^7(HQ): ^3We need an Covert Ops!" },
	{"SYS_MenDown",			 "_hq_men_down",			"^7(HQ): ^3We've lost most of our men!"},
	{"SYS_ObjCaptured",		 "_hq_objective_captured",	"^7(HQ): ^3Objective captured!"},
	{"SYS_ObjLost",			 "_hq_objective_lost",		"^7(HQ): ^3Objective lost!"},
	{"SYS_ObjDestroyed",	 "_hq_objective_destroyed",	"^7(HQ): ^3Objective destroyed!"},
	{"SYS_ConstructComplete","_hq_const_completed",		"^7(HQ): ^3Construction complete!"},
	{"SYS_ConstructFailed",	 "_hq_const_failed",		"^7(HQ): ^3Construction failed!"},
	{"SYS_Destroyed",		 "_hq_const_destroyed",		"^7(HQ): ^3Construction destroyed!"},
};

//   0     1     2     3     4     5     6     7     8     9
float	  syringeLoadout[NUM_SKILL_LEVELS] =
{	10,	  10,   12,	  12,	12,	  12,   15,	  15,	15,   15};

const char* skillNames[SK_NUM_SKILLS] = {
	"Battle Sense",
	"Engineering",
	"First Aid",
	"Signals",
	"Light Weapons",
	"Heavy Weapons",
	"Covert Ops"
};

const char* skillNamesLine1[SK_NUM_SKILLS] = {
	"Battle",
	"Engineering",
	"First",
	"Signals",
	"Light",
	"Heavy",
	"Covert"
};

const char* skillNamesLine2[SK_NUM_SKILLS] = {
	"Sense",
	"",
	"Aid",
	"",
	"Weapons",
	"Weapons",
	"Ops"
};

const char* medalNames[SK_NUM_SKILLS] = {
	"Distinguished Service Medal",
	"Steel Star",
	"Silver Cross",
	"Signals Medal",
	"Infantry Medal",
	"Bombardment Medal",
	"Silver Snake"
};


int skillLevels[SK_NUM_SKILLS][NUM_SKILL_LEVELS] = {
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
	{0, 20, 50, 90, 140, 200, 270, 350, 440, 540},
};


vec3_t	playerlegsProneMins = { -13.5f, -13.5f, -24.f };
vec3_t	playerlegsProneMaxs = { 13.5f, 13.5f, -14.4f };

int					numSplinePaths;
splinePath_t		splinePaths[MAX_SPLINE_PATHS];

int					numPathCorners;
pathCorner_t		pathCorners[MAX_PATH_CORNERS];

// these defines are matched with the character torso animations
#define DELAY_NO		0
#define DELAY_VERY_LOW	50	// rl
#define DELAY_LOW		100	// machineguns, tesla, spear, flame
#define DELAY_THROW		250	// grenades, dynamite
#define DELAY_HW		750

// Arnout: the new loadout for WolfXP
int weapBanksMultiPlayer[MAX_WEAP_BANKS_MP][MAX_WEAPS_IN_BANK_MP] = {
	{0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},	// empty bank '0'
	{WP_KNIFE,				WP_KNIFE_KABAR,			WP_POISON_SYRINGE,		0,		                0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0}, // jP added KABAR knife for allies
	{WP_LUGER,				WP_COLT,				WP_AKIMBO_COLT,			WP_AKIMBO_LUGER,		WP_AKIMBO_SILENCEDCOLT,
 	 WP_AKIMBO_SILENCEDLUGER,	0,					0,						0,						0,0,0,0},
	{WP_MP40,				WP_THOMPSON,			WP_STEN,				WP_GARAND,				WP_PANZERFAUST,
	 WP_FLAMETHROWER,		WP_KAR98,				WP_CARBINE,				WP_FG42,				WP_K43,
	 WP_MOBILE_MG42,		WP_MORTAR,				WP_SHOTGUN,				WP_MOBILE_BROWNING,		WP_STEN_MKII,
	 WP_BAR,				WP_STG44,				WP_BAZOOKA,				WP_MP34,				WP_MORTAR2,				 WP_VENOM },
	{WP_GRENADE_LAUNCHER,	WP_GRENADE_PINEAPPLE,	0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
	{WP_MEDIC_SYRINGE,		WP_PLIERS,				WP_SMOKE_MARKER,		WP_SMOKE_BOMB,			0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
	{WP_DYNAMITE,			WP_MEDKIT,				WP_AMMO,				WP_SATCHEL,				WP_SATCHEL_DET,			0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
	{WP_LANDMINE,			WP_MEDIC_ADRENALINE,	0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
	{WP_BINOCULARS,			0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
	{0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,						0,		0,	0},
};

// TAT 10/4/2002
//		Using one unified list for which weapons can received ammo
//		This is used both by the ammo pack code and by the bot code to determine if reloads are needed
int reloadableWeapons[] =
{// Axis or Neutral				Allied
	WP_MP40,					WP_THOMPSON,
	WP_STG44,					WP_STEN,
    WP_STEN_MKII,
	WP_LUGER,					WP_COLT,
	WP_AKIMBO_LUGER,			WP_AKIMBO_COLT,
	WP_AKIMBO_SILENCEDLUGER,	WP_AKIMBO_SILENCEDCOLT,
	WP_PANZERFAUST,				WP_BAZOOKA,
	WP_KAR98,					WP_CARBINE,
	WP_GPG40,					WP_M7,
	WP_FG42,					WP_BAR,
	WP_K43,						WP_GARAND,
	WP_MOBILE_MG42,				WP_MOBILE_BROWNING,
	WP_MORTAR,
	WP_MORTAR2,
	WP_FLAMETHROWER,
	WP_SHOTGUN,
	WP_MP34,
	WP_VENOM,
	-1
};


// core:	weapons marked with ** are listed with the total # clips players get,
//			so not just the extra ammo/clips they get (when they have reached the specific skilllevel)
// extraclips	-	the extra ammo/clips players can earn when reaching the proper skilllevel
//					Note: several weapons get updgrades more than once on SK_LIGHT_WEAPONS.
//					Note: akimbo weapons get the extra clips for each weapon.
//					Note: See also functions BG_ExtraAmmoForWeapon() & BG_MaxAmmoForWeapon().
//					Note: the following weapons are treated specially:
//					WP_KNIFE				dummy entry, handled differently.. clip (not ammo)
//					WP_GRENADE_LAUNCHER		dummy entry, handled differently.. clip (not ammo)
//					WP_GRENADE_PINEAPPLE	dummy entry, handled differently.. clip (not ammo)
//					WP_MEDIC_SYRINGE		dummy entry, handled differently.. clip (not ammo)
//					WP_MEDIC_ADRENALINE		dummy entry, handled differently.. clip (not ammo)
//					WP_KNIFE_KABAR			dummy entry, handled differently.. clip (not ammo)
//					WP_POISON_SYRINGE		dummy entry, handled differently.. clip (not ammo)
//					WP_PANZERFAUST			uses only clip (not ammo)
//					WP_FLAMETHROWER			uses only clip (not ammo)
//
int skillExtraClips[WP_NUM_WEAPONS] = {
	0,		//WP_NONE
	0,		//WP_KNIFE					!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	8,		//WP_LUGER
	30,		//WP_MP40
	0,		//WP_GRENADE_LAUNCHER		!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	1,		//WP_PANZERFAUST			!! uses only clip (not ammo)
	75,		//WP_FLAMETHROWER			!! uses only clip (not ammo)
	8,		//WP_COLT
	30,		//WP_THOMPSON
	0,		//WP_GRENADE_PINEAPPLE		!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	32,		//WP_STEN
	0,		//WP_MEDIC_SYRINGE			!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	0,		//WP_AMMO
	0,		//WP_ARTY
	8,		//WP_SILENCER
	0,		//WP_DYNAMITE
	0,		//WP_SMOKETRAIL
	0,		//VERYBIGEXPLOSION
	0,		//WP_MEDKIT
	0,		//WP_BINOCULARS
	0,		//WP_PLIERS
	0,		//WP_SMOKE_MARKER
	10,		//WP_KAR98
	10,		//WP_CARBINE
	10,		//WP_GARAND
	0,		//WP_LANDMINE
	0,		//WP_SATCHEL
	0,		//WP_SATCHEL_DET
	0,		//WP_SMOKE_BOMB
	150,	//WP_MOBILE_MG42
	10,		//WP_K43
	20,		//WP_FG42
	0,		//WP_DUMMY_MG42
	2,		//WP_MORTAR
	8,		//WP_AKIMBO_COLT
	8,		//WP_AKIMBO_LUGER
	4,		//WP_GPG40
	4,		//WP_M7
	8,		//WP_SILENCED_COLT
	10,		//WP_GARAND_SCOPE
	10,		//WP_K43_SCOPE
	10,		//WP_FG42SCOPE
	2,		//WP_MORTAR_SET
	0,		//WP_MEDIC_ADRENALINE		!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	8,		//WP_AKIMBO_SILENCEDCOLT
	8,		//WP_AKIMBO_SILENCEDLUGER
	150,	//WP_MOBILE_MG42_SET
	8,		//WP_SHOTGUN
	0,		//WP_KNIFE_KABAR			!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	150,	//WP_MOBILE_BROWNING
	150,	//WP_MOBILE_BROWNING_SET
	20,		//WP_BAR
	20,		//WP_BAR_SET
	30,		//WP_STG44
	32,		//WP_STEN_MKII
	1,		//WP_BAZOOKA
	32,		//WP_MP34
	2,		//WP_MORTAR2
	2,		//WP_MORTAR2_SET
	250,	//WP_VENOM
	0,		//WP_POISON_SYRINGE			!! ** dummy entry, handled differently (class/skill).. clip (not ammo)
	0		//WP_FOOTKICK
};


// [0] = maxammo		-	max player ammo carrying capacity.
// [1] = uses			-	how many 'rounds' it takes/costs to fire one cycle.
// [2] = maxclip		-	max 'rounds' in a clip.
// [3] = startammo		-	player ammo when spawning
// [4] = startclip		-	player clips when spawning
// [5] = reloadTime		-	time from start of reload until ready to fire.
// [6] = fireDelayTime	-	time from pressing 'fire' until first shot is fired. (used for delaying fire while weapon is 'readied' in animation)
// [7] = nextShotTime	-	when firing continuously, this is the time between shots
// [8] = maxHeat		-	max active firing time before weapon 'overheats' (at which point the weapon will fail for a moment)
// [9] = coolRate		-	how fast the weapon cools down.
// [10] = mod			-	means of death
// [11] = clipindex		-	weapon-number of the clip the weapon is using (for clip-sharing weapons).  Note: these entries are filled in at runtime
// [12] = ammoindex		-	weapon-number of the ammo the weapon is using (for ammo-sharing weapons).  Note: these entries are filled in at runtime
// [13] = isAutoReload	-	is an auto reload weapon
// [14] = isFastSwitch	-	is a fast-switch weapon
// [15] = isHeavy		-	is a heavy weapon
//
weapontable_t WeaponTable[WP_NUM_WEAPONS] =
{
	//	MAX		USES	MAX		START	START	RELOAD	FIRE			NEXT	HEAT,	COOL,	MOD,						CLIP	AMMO	AUTO	FAST	HEAVY	...
	//	AMMO	AMT.	CLIP	AMMO	CLIP	TIME	DELAY			SHOT												INDEX	INDEX	RELOAD	SWITCH

	{	0,		0,		0,		0,		0,		0,		50,				0,		0,		0,		MOD_UNKNOWN,				0,		0,		qfalse,	qfalse,	qfalse	},	// WP_NONE					// 0
	{	8,		0,		8,		0,		4,		0,		DELAY_VERY_LOW,	200,	0,		0,		MOD_KNIFE,					0,		0,		qfalse,	qtrue,	qfalse	},	// WP_KNIFE					// 1
	{	24,		1,		8,		24,		8,		1500,	DELAY_LOW,		400,	0,		0,		MOD_LUGER,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_LUGER					// 2	// NOTE: also 32 round 'snail' magazine
	{	90,		1,		30,		30,		30,		2400,	DELAY_LOW,		150,	0,		0,		MOD_MP40,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_MP40					// 3
	{	45,		1,		15,		0,		4,		1000,	DELAY_THROW,	1600,	0,		0,		MOD_GRENADE_LAUNCHER,		0,		0,		qfalse,	qtrue,	qfalse	},	// WP_GRENADE_LAUNCHER		// 4
	{	4,		1,		1,		0,		4,		1000,	DELAY_HW	,	2000,	0,		0,		MOD_PANZERFAUST,			0,		0,		qfalse,	qfalse,	qtrue	},	// WP_PANZERFAUST			// 5	// DHM - Nerve :: updated delay so prediction is correct
	{	200,	1,		200,	0,		200,	1000,	DELAY_LOW,		50,		0,		0,		MOD_FLAMETHROWER,			0,		0,		qfalse,	qfalse,	qtrue	},	// WP_FLAMETHROWER			// 6
	{	24,		1,		8,		24,		8,		1500,	DELAY_LOW,		400,	0,		0,		MOD_COLT,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_COLT					// 7
	{	90,		1,		30,		30,		30,		2400,	DELAY_LOW,		150,	0,		0,		MOD_THOMPSON,				0,		0,		qtrue,	qtrue,	qfalse	},	// WP_THOMPSON				// 8
	{	45,		1,		15,		0,		4,		1000,	DELAY_THROW,	1600,	0,		0,		MOD_GRENADE_PINEAPPLE,		0,		0,		qfalse,	qtrue,	qfalse	},	// WP_GRENADE_PINEAPPLE		// 9

	{	96,		1,		32,		64,		32,		3100,	DELAY_LOW,		150,	1200,	450,	MOD_STEN,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_STEN					// 10
	{	10,		1,		1,		0,		10,		1500,	DELAY_VERY_LOW,	1000,	0,		0,		MOD_SYRINGE,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_MEDIC_SYRINGE			// 11
	{	1,		0,		1,		0,		0,		3000,	DELAY_VERY_LOW,	1000,	0,		0,		MOD_AMMO,					0,		0,		qfalse,	qtrue,	qfalse	},	// WP_AMMO					// 12
	{	1,		0,		1,		0,		1,		3000,	DELAY_VERY_LOW,	1000,	0,		0,		MOD_ARTY,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_ARTY					// 13
	{	24,		1,		8,		24,		8,		1500,	DELAY_LOW,		400,	0,		0,		MOD_SILENCER,				0,		0,		qtrue,	qtrue,	qfalse	},	// WP_SILENCER				// 14
	{	1,		0,		10,		0,		0,		1000,	DELAY_THROW,	1600,	0,		0,		MOD_DYNAMITE,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_DYNAMITE				// 15
	{	999,	0,		999,	0,		0,		0,		DELAY_VERY_LOW,	0,		0,		0,		0,							0,		0,		qfalse,	qfalse,	qfalse	},	// WP_SMOKETRAIL			// 16

	{	0,		0,		0,		0,		0,		0,		DELAY_VERY_LOW,	0,		0,		0,		0,							0,		0,		qfalse,	qfalse,	qfalse	},	// VERYBIGEXPLOSION			// 17

	{	999,	0,		999,	1,		1,		0,		DELAY_VERY_LOW,	1000,	0,		0,		0,							0,		0,		qfalse,	qtrue,	qfalse	},	// WP_MEDKIT				// 18
	{	999,	0,		999,	0,		0,		0,		DELAY_VERY_LOW,	0,		0,		0,		0,							0,		0,		qfalse,	qtrue,	qfalse	},	// WP_BINOCULARS			// 19

	{	999,	0,		999,	0,		0,		0,		DELAY_VERY_LOW,	50,		0,		0,		0,							0,		0,		qfalse,	qtrue,	qfalse	},	// WP_PLIERS				// 20
	{	999,	0,		999,	0,		1,		0,		DELAY_VERY_LOW,	1000,	0,		0,		MOD_AIRSTRIKE,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_SMOKE_MARKER			// 21
	{	30,		1,		10,		20,		10,		2500,	DELAY_LOW,		400,	0,		0,		MOD_KAR98,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_KAR98					// 22		K43
	{	30,		1,		10,		20,		10,		1500,	DELAY_LOW,		400,	0,		0,		MOD_CARBINE,				0,		0,		qtrue,	qtrue,	qfalse	},	// WP_CARBINE				// 23		GARAND (Engineer)
	{	30,		1,		10,		20,		10,		1500,	DELAY_LOW,		400,	0,		0,		MOD_GARAND,					0,		0,		qtrue,	qfalse,	qfalse	},	// WP_GARAND				// 24		GARAND (Covert)
	{	1,		0,		1,		0,		1,		100,	DELAY_LOW,		100,	0,		0,		MOD_LANDMINE,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_LANDMINE				// 25
	{	1,		0,		1,		0,		0,		3000,	DELAY_LOW,		2000,	0,		0,		MOD_SATCHEL,				0,		0,		qfalse,	qfalse,	qfalse	},	// WP_SATCHEL				// 26
	{	1,		0,		1,		0,		0,		3000,	722,			2000,	0,		0,		0,							0,		0,		qfalse,	qfalse,	qfalse	},	// WP_SATCHEL_DET			// 27
	{	1,		0,		10,		0,		1,		1000,	DELAY_THROW,	1600,	0,		0,		MOD_SMOKEBOMB,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_SMOKE_BOMB			// 28
	{	450,	1,		150,	0,		150,	3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_MG42,			0,		0,		qtrue,	qfalse,	qtrue	},	// WP_MOBILE_MG42			// 29

	{	30,		1,		10,		20,		10,		2500,	DELAY_LOW,		400,	0,		0,		MOD_K43,					0,		0,		qtrue,	qfalse,	qfalse	},	// WP_K43					// 30		K43
	{	60,		1,		20,		40,		20,		2000,	DELAY_LOW,		100,	0,		0,		MOD_FG42,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_FG42					// 31
	{	0,		0,		0,		0,		0,		0,	    DELAY_NO,		0,	    1500,	300,	0,						    0,		0,		qfalse,	qfalse,	qfalse	},	// WP_DUMMY_MG42			// 32
	{	15,		1,		1,		0,		0,		0,		DELAY_HW,		1600,	0,		0,		MOD_MORTAR,					0,		0,		qfalse,	qfalse,	qtrue	},	// WP_MORTAR				// 33
	{	48,		1,		8,		48,		8,		2700,	DELAY_LOW,		200,	0,		0,		MOD_AKIMBO_COLT,			0,		0,		qtrue,	qtrue,	qfalse	},	// WP_AKIMBO_COLT			// 34
	{	48,		1,		8,		48,		8,		2700,	DELAY_LOW,		200,	0,		0,		MOD_AKIMBO_LUGER,			0,		0,		qtrue,	qtrue,	qfalse	},	// WP_AKIMBO_LUGER			// 35
	{	4,		1,		1,		4,		1,		3000,	DELAY_LOW,		400,	0,		0,		MOD_GPG40,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_GPG40					// 36
	{	4,		1,		1,		4,		1,		3000,	DELAY_LOW,		400,	0,		0,		MOD_M7,						0,		0,		qfalse,	qfalse,	qfalse	},	// WP_M7					// 37
	{	24,		1,		8,		24,		8,		1500,	DELAY_LOW,		400,	0,		0,		MOD_SILENCED_COLT,			0,		0,		qtrue,	qtrue,	qfalse	},	// WP_SILENCED_COLT			// 38
	{	30,		1,		10,		20,		10,		1500,	DELAY_NO,		400,	0,		0,		MOD_GARAND_SCOPE,			0,		0,		qtrue,	qfalse,	qfalse	},	// WP_GARAND_SCOPE			// 39		GARAND

	{	30,		1,		10,		20,		10,		2500,	DELAY_NO,		400,	0,		0,		MOD_K43_SCOPE,				0,		0,		qtrue,	qfalse,	qfalse	},	// WP_K43_SCOPE				// 40		K43
	{	60,		1,		20,		40,		20,		2000,	DELAY_LOW,		400,	0,		0,		MOD_FG42_SCOPE,				0,		0,		qtrue,	qfalse,	qfalse	},	// WP_FG42_SCOPE			// 41
	{	16,		1,		1,		12,		0,		0,		DELAY_HW,		1400,	0,		0,		MOD_MORTAR,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_MORTAR_SET			// 42
	{	10,		1,		1,		0,		10,		1500,	DELAY_VERY_LOW,	1000,	0,		0,		MOD_SYRINGE,				0,		0,		qfalse,	qtrue,	qfalse	},	// WP_MEDIC_ADRENALINE		// 43
	{	48,		1,		8,		48,		8,		2700,	DELAY_LOW,		200,	0,		0,		MOD_AKIMBO_SILENCEDCOLT,	0,		0,		qtrue,	qtrue,	qfalse	},	// WP_AKIMBO_SILENCEDCOLT	// 44
	{	48,		1,		8,		48,		8,		2700,	DELAY_LOW,		200,	0,		0,		MOD_AKIMBO_SILENCEDLUGER,	0,		0,		qtrue,	qtrue,	qfalse	},	// WP_AKIMBO_SILENCEDLUGER	// 45
	{	450,	1,		150,	0,		150,	3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_MG42,			0,		0,		qtrue,	qfalse,	qfalse	},	// WP_MOBILE_MG42_SET		// 46
	{	24,		1,		6,		24,		6,		1500,	DELAY_LOW,		850,	0,		0,		MOD_SHOTGUN,				0,		0,		qtrue,	qtrue,	qfalse	},	// WP_SHOTGUN				// 47		Winchester Model 97
	{	8,		0,		8,		0,		4,		0,		DELAY_VERY_LOW,	200,	0,		0,		MOD_KNIFE,					0,		0,		qfalse,	qtrue,	qfalse	},	// WP_KNIFE_KABAR			// 48		Allied Knife
	{	450,	1,		150,	0,		150,	3000,	DELAY_LOW,		66,		1400,	400,	MOD_MOBILE_BROWNING,		0,		0,		qtrue,	qfalse,	qtrue	},	// WP_MOBILE_BROWNING		// 49		Browning .30 cal

	{	450,	1,		150,	0,		150,	3000,	DELAY_LOW,		66,		1400,	400,	MOD_MOBILE_BROWNING,		0,		0,		qtrue,	qfalse,	qfalse	},	// WP_MOBILE_BROWNING_SET	// 50		Browning .30 cal set
	{	80,		1,		20,		60,		20,		2000,	DELAY_LOW,		150,	0,		0,		MOD_BAR, 					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_BAR					// 51		Browning Automatic Rifle
	{	80,		1,		20,		60,		20,		2000,	DELAY_LOW,		165,	0,		0,		MOD_BAR, 					0,		0,		qtrue,	qfalse,	qfalse	},	// WP_BAR_SET				// 52		Browning Automatic Rifle (Set)
	{	90,		1,		30,		30,		30,		2400,	DELAY_LOW,		180,	0,		0,		MOD_STG44,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_STG44					// 53		StG44 Assault Rifle
	{	96,		1,		32,		64,		32,		3100,	DELAY_LOW,		150,	0,		0,		MOD_STEN_MKII,				0,		0,		qtrue,	qtrue,	qfalse	},	// WP_STEN_MKII				// 54		Sten MkII Unsilenced SMG
	{	5,		1,		1,		0,		5,		900,	DELAY_HW,		2000,	0,		0,		MOD_BAZOOKA,				0,		0,		qfalse,	qfalse,	qtrue	},	// WP_BAZOOKA				// 55		M1 Bazooka Anti-Tank Weapon
	{	96,		1,		32,		64,		32,		3100,	DELAY_LOW,		150,	1100,	550,	MOD_MP34,					0,		0,		qtrue,	qtrue,	qfalse	},	// WP_MP34					// 56		Solothurn S1-100 SMG
	{	15,		1,		1,		0,		0,		0,		DELAY_HW,		1600,	0,		0,		MOD_MORTAR,					0,		0,		qfalse,	qfalse,	qtrue	},	// WP_MORTAR2				// 57
	{	16,		1,		1,		12,		0,		0,		DELAY_HW,		1400,	0,		0,		MOD_MORTAR,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_MORTAR2_SET			// 58
	{	500,	1,		250,	0,		250,	3000,	DELAY_HW,		45,		2500,	200,	MOD_VENOM,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_VENOM					// 59

	{	10,		1,		1,		0,		10,		1500,	DELAY_VERY_LOW,	1000,	0,		0,		MOD_POISON,					0,		0,		qfalse,	qtrue,	qfalse	},	// WP_POISON_SYRINGE		// 60

	{	0,		0,		0,		0,		0,		0,		50,				0,		0,		0,		MOD_KICKED,					0,		0,		qfalse,	qfalse,	qfalse	},	// WP_FOOTKICK					// 61

};

// core: weapons that overheat
int weapOverheat[8] =
{
	WP_STEN,
	WP_MOBILE_MG42,
	WP_DUMMY_MG42,
	WP_MOBILE_MG42_SET,
	WP_MOBILE_BROWNING,
	WP_MOBILE_BROWNING_SET,
	WP_MP34,
	WP_VENOM
};

//----(SA)	moved in here so both games can get to it
int weapAlts[] =
{
	WP_NONE,				// 0  WP_NONE
	WP_NONE,				// 1  WP_KNIFE
	WP_SILENCER,			// 2  WP_LUGER
	WP_NONE,				// 3  WP_MP40
	WP_NONE,				// 4  WP_GRENADE_LAUNCHER
	WP_NONE,				// 5  WP_PANZERFAUST
	WP_NONE,				// 6  WP_FLAMETHROWER
	WP_SILENCED_COLT,		// 7  WP_COLT
	WP_NONE,				// 8  WP_THOMPSON
	WP_NONE,				// 9  WP_GRENADE_PINEAPPLE

	WP_NONE,				// 10 WP_STEN
	WP_NONE,				// 11 WP_MEDIC_SYRINGE
	WP_NONE,				// 12 WP_AMMO
	WP_NONE,				// 13 WP_ARTY
	WP_LUGER,				// 14 WP_SILENCER
	WP_NONE,				// 15 WP_DYNAMITE
	WP_NONE,				// 16 WP_SMOKETRAIL
	WP_NONE,				// 17 VERYBIGEXPLOSION
	WP_NONE,				// 18 WP_MEDKIT
	WP_NONE,				// 19 WP_BINOCULARS

	WP_NONE,				// 20 WP_PLIERS
	WP_NONE,				// 21 WP_SMOKE_MARKER
	WP_GPG40,				// 22 WP_KAR98
	WP_M7,					// 23 WP_CARBINE (GARAND really)
	WP_GARAND_SCOPE,		// 24 WP_GARAND
	WP_NONE,				// 25 WP_LANDMINE
	WP_NONE,				// 26 WP_SATCHEL
	WP_NONE,				// 27 WP_SATCHEL_DET
	WP_NONE,				// 28 WP_SMOKE_BOMB
	WP_MOBILE_MG42_SET,		// 29 WP_MOBILE_MG42

	WP_K43_SCOPE,			// 30 WP_K43
	WP_FG42SCOPE,			// 31 WP_FG42
    WP_NONE,            	// 32 WP_DUMMY_MG42
	WP_MORTAR_SET,			// 33 WP_MORTAR
	WP_NONE,				// 34 WP_AKIMBO_COLT
	WP_NONE,				// 35 WP_AKIMBO_LUGER
	WP_KAR98,				// 36 WP_GPG40
	WP_CARBINE,				// 37 WP_M7
	WP_COLT,				// 38 WP_SILENCED_COLT
	WP_GARAND,				// 39 WP_GARAND_SCOPE

	WP_K43,					// 40 WP_K43_SCOPE
	WP_FG42,				// 41 WP_FG42_SCOPE
	WP_MORTAR,				// 42 WP_MORTAR_SET
	WP_NONE,				// 43 WP_MEDIC_ADRENALINE
	WP_NONE,				// 44 WP_AKIMBO_SILENCEDCOLT
	WP_NONE,				// 45 WP_AKIMBO_SILENCEDLUGER
	WP_MOBILE_MG42,			// 46 WP_MOBILE_MG42_SET
	WP_NONE,				// 47 WP_SHOTGUN
	WP_NONE,				// 48 WP_KNIFE_KABAR
	WP_MOBILE_BROWNING_SET, // 49 WP_MOBILE_BROWNING

	WP_MOBILE_BROWNING,		// 50 WP_MOBILE_BROWNING_SET
	WP_BAR_SET,				// 51 WP_BAR
	WP_BAR,					// 52 WP_BAR_SET
	WP_NONE,				// 53 WP_STG44
	WP_NONE,				// 54 WP_STEN_MKII
	WP_NONE,				// 55 WP_BAZOOKA
	WP_NONE,				// 56 WP_MP34
	WP_MORTAR2_SET,			// 57 WP_MORTAR2
	WP_MORTAR2,				// 58 WP_MORTAR2_SET
	WP_NONE,				// 59 WP_VENOM

	WP_NONE,				// 60 WP_POISON_SYRINGE
	WP_NONE,				// 61 WP_FOOTKICK

};


//----(SA)	moved in here so both games can get to it
int weapTeamConversion[] =
{
	WP_NONE,				// 0  WP_NONE
	WP_KNIFE_KABAR,			// 1  WP_KNIFE
	WP_COLT,				// 2  WP_LUGER
	WP_THOMPSON,			// 3  WP_MP40
	WP_GRENADE_PINEAPPLE,	// 4  WP_GRENADE_LAUNCHER
	WP_BAZOOKA,				// 5  WP_PANZERFAUST
	WP_FLAMETHROWER,		// 6  WP_FLAMETHROWER
	WP_LUGER,				// 7  WP_COLT
	WP_MP40,				// 8  WP_THOMPSON
	WP_GRENADE_LAUNCHER,	// 9  WP_GRENADE_PINEAPPLE

	WP_MP34,				// 10 WP_STEN
	WP_MEDIC_SYRINGE,		// 11 WP_MEDIC_SYRINGE
	WP_AMMO,				// 12 WP_AMMO
	WP_ARTY,				// 13 WP_ARTY
	WP_SILENCED_COLT,		// 14 WP_SILENCER
	WP_DYNAMITE,			// 15 WP_DYNAMITE
	WP_SMOKETRAIL,			// 16 WP_SMOKETRAIL
	VERYBIGEXPLOSION,		// 17 VERYBIGEXPLOSION
	WP_MEDKIT,				// 18 WP_MEDKIT
	WP_BINOCULARS,			// 19 WP_BINOCULARS

	WP_PLIERS,				// 20 WP_PLIERS
	WP_SMOKE_MARKER,		// 21 WP_SMOKE_MARKER
	WP_CARBINE,				// 22 WP_KAR98
	WP_KAR98,				// 23 WP_CARBINE (GARAND really)
	WP_K43,					// 24 WP_GARAND
	WP_LANDMINE,			// 25 WP_LANDMINE
	WP_SATCHEL,				// 26 WP_SATCHEL
	WP_SATCHEL_DET,			// 27 WP_SATCHEL_DET
	WP_SMOKE_BOMB,			// 28 WP_SMOKE_BOMB
	WP_MOBILE_BROWNING,		// 29 WP_MOBILE_MG42

	WP_GARAND,				// 30 WP_K43
	WP_FG42,				// 31 WP_FG42					no conversion for this weapon (was WP_BAR)..
    WP_DUMMY_MG42,         	// 32 WP_DUMMY_MG42
	WP_MORTAR2,				// 33 WP_MORTAR
	WP_AKIMBO_LUGER,		// 34 WP_AKIMBO_COLT
	WP_AKIMBO_COLT,			// 35 WP_AKIMBO_LUGER
	WP_M7,					// 36 WP_GPG40
	WP_GPG40,				// 37 WP_M7
	WP_SILENCER,			// 38 WP_SILENCED_COLT
	WP_K43_SCOPE,			// 39 WP_GARAND_SCOPE

	WP_GARAND_SCOPE,		// 40 WP_K43_SCOPE
	WP_FG42SCOPE,			// 41 WP_FG42SCOPE				no conversion for this weapon (was WP_BAR_SET)..
	WP_MORTAR2_SET,			// 42 WP_MORTAR_SET
	WP_MEDIC_ADRENALINE,	// 43 WP_MEDIC_ADRENALINE
	WP_AKIMBO_SILENCEDLUGER,// 44 WP_AKIMBO_SILENCEDCOLT
	WP_AKIMBO_SILENCEDCOLT,	// 45 WP_AKIMBO_SILENCEDLUGER
	WP_MOBILE_BROWNING_SET,	// 46 WP_MOBILE_MG42_SET
	WP_SHOTGUN,				// 47 WP_SHOTGUN
	WP_KNIFE,				// 48 WP_KNIFE_KABAR
	WP_MOBILE_MG42,			// 49 WP_MOBILE_BROWNING

	WP_MOBILE_MG42_SET,		// 50 WP_MOBILE_BROWNING_SET
	WP_STG44,				// 51 WP_BAR					other conversion for this weapon (was WP_FG42)..
	WP_STG44,				// 52 WP_BAR_SET				other conversion for this weapon (was WP_FG42SCOPE)..
	WP_BAR,					// 53 WP_STG44
	WP_MP34,				// 54 WP_STEN_MKII
	WP_PANZERFAUST,			// 55 WP_BAZOOKA
	WP_STEN,				// 56 WP_MP34
	WP_MORTAR,				// 57 WP_MORTAR2
	WP_MORTAR_SET,			// 58 WP_MORTAR2_SET
	WP_VENOM,				// 59 WP_VENOM

	WP_POISON_SYRINGE,		// 60 WP_POISON_SYRINGE
	WP_FOOTKICK,			// 61 WP_FOOTKICK
							//    WP_NUM_WEAPONS
};

// core: weapons that can be disabled/restricted..
int weaponCanDisable[WP_NUM_CAN_DISABLE_WEAPONS] =
{
	WP_GRENADE_LAUNCHER,
	WP_PANZERFAUST,
	WP_FLAMETHROWER,
	WP_GRENADE_PINEAPPLE,
	WP_BINOCULARS,			// war-mode weapon
	WP_PLIERS,				// war-mode weapon
	WP_KAR98,
	WP_CARBINE,
	WP_GARAND,
	WP_SMOKE_BOMB,			// war-mode weapon
	WP_MOBILE_MG42,
	WP_K43,
	WP_FG42,
	WP_MORTAR,
	WP_GPG40,
	WP_M7,
	WP_GARAND_SCOPE,
	WP_K43_SCOPE,
	WP_FG42SCOPE,
	WP_MORTAR_SET,
	WP_MEDIC_ADRENALINE,	// special case
	WP_MOBILE_MG42_SET,
	WP_SHOTGUN,
	WP_MOBILE_BROWNING,
	WP_MOBILE_BROWNING_SET,
	WP_BAR,
	WP_BAR_SET,
	WP_STG44,
	WP_BAZOOKA,
	WP_MORTAR2,
	WP_MORTAR2_SET,
	WP_VENOM,
	WP_POISON_SYRINGE		// special case
};

// core: weapons that can be disabled/restricted..
// (an array with all weapons, for quick indexed lookups).
qboolean weaponsCanDisable[WP_NUM_WEAPONS] =
{
	qfalse,					// 0  WP_NONE
	qfalse,					// 1  WP_KNIFE
	qfalse,					// 2  WP_LUGER
	qfalse,					// 3  WP_MP40
	qtrue,					// 4  WP_GRENADE_LAUNCHER
	qtrue,					// 5  WP_PANZERFAUST
	qtrue,					// 6  WP_FLAMETHROWER
	qfalse,					// 7  WP_COLT
	qfalse,					// 8  WP_THOMPSON
	qtrue,					// 9  WP_GRENADE_PINEAPPLE

	qfalse,					// 10 WP_STEN
	qfalse,					// 11 WP_MEDIC_SYRINGE
	qfalse,					// 12 WP_AMMO
	qfalse,					// 13 WP_ARTY
	qfalse,					// 14 WP_SILENCER
	qfalse,					// 15 WP_DYNAMITE
	qfalse,					// 16 WP_SMOKETRAIL
	qfalse,					// 17 VERYBIGEXPLOSION
	qfalse,					// 18 WP_MEDKIT
	qtrue,					// 19 WP_BINOCULARS

	qtrue,					// 20 WP_PLIERS
	qfalse,					// 21 WP_SMOKE_MARKER
	qtrue,					// 22 WP_KAR98
	qtrue,					// 23 WP_CARBINE (GARAND really)
	qtrue,					// 24 WP_GARAND
	qfalse,					// 25 WP_LANDMINE
	qfalse,					// 26 WP_SATCHEL
	qfalse,					// 27 WP_SATCHEL_DET
	qtrue,					// 28 WP_SMOKE_BOMB
	qtrue,					// 29 WP_MOBILE_MG42

	qtrue,					// 30 WP_K43
	qtrue,					// 31 WP_FG42
    qfalse,		           	// 32 WP_DUMMY_MG42
	qtrue,					// 33 WP_MORTAR
	qfalse,					// 34 WP_AKIMBO_COLT
	qfalse,					// 35 WP_AKIMBO_LUGER
	qtrue,					// 36 WP_GPG40
	qtrue,					// 37 WP_M7
	qfalse,					// 38 WP_SILENCED_COLT
	qtrue,					// 39 WP_GARAND_SCOPE

	qtrue,					// 40 WP_K43_SCOPE
	qtrue,					// 41 WP_FG42_SCOPE
	qtrue,					// 42 WP_MORTAR_SET
	qtrue,					// 43 WP_MEDIC_ADRENALINE
	qfalse,					// 44 WP_AKIMBO_SILENCEDCOLT
	qfalse,					// 45 WP_AKIMBO_SILENCEDLUGER
	qtrue,					// 46 WP_MOBILE_MG42_SET
	qtrue,					// 47 WP_SHOTGUN
	qfalse,					// 48 WP_KNIFE_KABAR
	qtrue,					// 49 WP_MOBILE_BROWNING

	qtrue,					// 50 WP_MOBILE_BROWNING_SET
	qtrue,					// 51 WP_BAR
	qtrue,					// 52 WP_BAR_SET
	qtrue,					// 53 WP_STG44
	qfalse,					// 54 WP_STEN_MKII
	qtrue,					// 55 WP_BAZOOKA
	qfalse,					// 56 WP_MP34
	qtrue,					// 57 WP_MORTAR2
	qtrue,					// 58 WP_MORTAR2_SET
	qtrue,					// 59 WP_VENOM

	qtrue,					// 60 WP_POISON_SYRINGE

	qfalse,					// 61 WP_FOOTKICK

};


// core: a table for direct lookups..
//       Some MOD have more than 1 possible weapon,
//       but the first element always returns the same value as BG_WeaponForMOD() would do..
//       I just put them all in this array (maybe used at sometime).
int modWeaponTableMP[MOD_NUM_MODS][4] =
{
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_UNKNOWN,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MACHINEGUN,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_BROWNING,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MG42,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GRENADE,
	{ WP_KNIFE,					WP_KNIFE_KABAR,			WP_NONE,	WP_NONE },			// MOD_KNIFE,
	{ WP_LUGER,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_LUGER,
	{ WP_COLT,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_COLT,
	{ WP_MP40,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MP40,
	{ WP_THOMPSON,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_THOMPSON,
	{ WP_STEN,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_STEN,
	{ WP_GARAND,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GARAND,
	{ WP_SILENCER,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SILENCER,
	{ WP_FG42,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_FG42,
	{ WP_FG42SCOPE,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_FG42_SCOPE,
	{ WP_PANZERFAUST,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_PANZERFAUST,
	{ WP_GRENADE_LAUNCHER,		WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GRENADE_LAUNCHER,
	{ WP_FLAMETHROWER,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_FLAMETHROWER,
	{ WP_GRENADE_PINEAPPLE,		WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GRENADE_PINEAPPLE,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MAPMORTAR,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MAPMORTAR_SPLASH,
	{ WP_FOOTKICK,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_KICKED,
	{ WP_DYNAMITE,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_DYNAMITE,
	{ WP_SMOKE_MARKER,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AIRSTRIKE,
	{ WP_MEDIC_SYRINGE,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SYRINGE,
	{ WP_AMMO,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AMMO,
	{ WP_ARTY,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_ARTY,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_WATER,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SLIME,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_LAVA,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CRUSH,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_TELEFRAG,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_FALLING,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SUICIDE,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_TARGET_LASER,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_TRIGGER_HURT,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_EXPLOSIVE,
	{ WP_CARBINE,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CARBINE,
	{ WP_KAR98,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_KAR98,
	{ WP_GPG40,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GPG40,
	{ WP_M7,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_M7,
	{ WP_LANDMINE,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_LANDMINE,
	{ WP_SATCHEL,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SATCHEL,
	{ WP_SMOKE_BOMB,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SMOKEBOMB,
	{ WP_MOBILE_MG42,			WP_MOBILE_MG42_SET,		WP_NONE,	WP_NONE },			// MOD_MOBILE_MG42,
	{ WP_SILENCED_COLT,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SILENCED_COLT,
	{ WP_GARAND_SCOPE,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GARAND_SCOPE,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CRUSH_CONSTRUCTION,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CRUSH_CONSTRUCTIONDEATH,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER,
	{ WP_K43,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_K43,
	{ WP_K43_SCOPE,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_K43_SCOPE,
	{ WP_MORTAR,				WP_MORTAR_SET,			WP_MORTAR2,	WP_MORTAR2_SET },	// MOD_MORTAR,
	{ WP_AKIMBO_COLT,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AKIMBO_COLT,
	{ WP_AKIMBO_LUGER,			WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AKIMBO_LUGER,
	{ WP_AKIMBO_SILENCEDCOLT,	WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AKIMBO_SILENCEDCOLT,
	{ WP_AKIMBO_SILENCEDLUGER,	WP_NONE,				WP_NONE,	WP_NONE },			// MOD_AKIMBO_SILENCEDLUGER,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SMOKEGRENADE,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SWAP_PLACES,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SWITCHTEAM,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_GOOMBA,
	{ WP_POISON_SYRINGE,		WP_NONE,				WP_NONE,	WP_NONE },			// MOD_POISON,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_FEAR,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_CENSORED,
	{ WP_SHOTGUN,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SHOTGUN,
	{ WP_KNIFE,					WP_KNIFE_KABAR,			WP_NONE,	WP_NONE },			// MOD_BACKSTAB,
	{ WP_MOBILE_BROWNING,		WP_MOBILE_BROWNING_SET,	WP_NONE,	WP_NONE },			// MOD_MOBILE_BROWNING,
	{ WP_BAR,					WP_BAR_SET,				WP_NONE,	WP_NONE },			// MOD_BAR,
	{ WP_STG44,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_STG44,
	{ WP_BAZOOKA,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_BAZOOKA,
	{ WP_STEN_MKII,				WP_NONE,				WP_NONE,	WP_NONE },			// MOD_STEN_MKII,
	{ WP_MP34,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_MP34,
	{ WP_VENOM,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_VENOM,
	{ WP_NONE,					WP_NONE,				WP_NONE,	WP_NONE },			// MOD_SHOVE,
	{ WP_KNIFE,					WP_KNIFE_KABAR,			WP_NONE,	WP_NONE }			// MOD_THROWKNIFE
};

qboolean modFalloffDamage[MOD_NUM_MODS] =
{
	qfalse,					//MOD_UNKNOWN,
	qtrue,					//MOD_MACHINEGUN,
	qtrue,					//MOD_BROWNING,
	qtrue,					//MOD_MG42,
	qfalse,					//MOD_GRENADE,
	qfalse,					//MOD_KNIFE,
	qtrue,					//MOD_LUGER,
	qtrue,					//MOD_COLT,
	qtrue,					//MOD_MP40,
	qtrue,					//MOD_THOMPSON,
	qtrue,					//MOD_STEN,
	qtrue,					//MOD_GARAND,
	qtrue,					//MOD_SILENCER,
	qtrue,					//MOD_FG42,
	qfalse,					//MOD_FG42_SCOPE,
	qfalse,					//MOD_PANZERFAUST,
	qfalse,					//MOD_GRENADE_LAUNCHER,
	qfalse,					//MOD_FLAMETHROWER,
	qfalse,					//MOD_GRENADE_PINEAPPLE,
	qfalse,					//MOD_MAPMORTAR,
	qfalse,					//MOD_MAPMORTAR_SPLASH,
	qfalse,					//MOD_KICKED,
	qfalse,					//MOD_DYNAMITE,
	qfalse,					//MOD_AIRSTRIKE,
	qfalse,					//MOD_SYRINGE,
	qfalse,					//MOD_AMMO,
	qfalse,					//MOD_ARTY,
	qfalse,					//MOD_WATER,
	qfalse,					//MOD_SLIME,
	qfalse,					//MOD_LAVA,
	qfalse,					//MOD_CRUSH,
	qfalse,					//MOD_TELEFRAG,
	qfalse,					//MOD_FALLING,
	qfalse,					//MOD_SUICIDE,
	qfalse,					//MOD_TARGET_LASER,
	qfalse,					//MOD_TRIGGER_HURT,
	qfalse,					//MOD_EXPLOSIVE,
	qtrue,					//MOD_CARBINE,
	qtrue,					//MOD_KAR98,
	qfalse,					//MOD_GPG40,
	qfalse,					//MOD_M7,
	qfalse,					//MOD_LANDMINE,
	qfalse,					//MOD_SATCHEL,
	qfalse,					//MOD_SMOKEBOMB,
	qtrue,					//MOD_MOBILE_MG42,
	qtrue,					//MOD_SILENCED_COLT,
	qfalse,					//MOD_GARAND_SCOPE,
	qfalse,					//MOD_CRUSH_CONSTRUCTION,
	qfalse,					//MOD_CRUSH_CONSTRUCTIONDEATH,
	qfalse,					//MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER,
	qtrue,					//MOD_K43,
	qfalse,					//MOD_K43_SCOPE,
	qfalse,					//MOD_MORTAR,
	qtrue,					//MOD_AKIMBO_COLT,
	qtrue,					//MOD_AKIMBO_LUGER,
	qtrue,					//MOD_AKIMBO_SILENCEDCOLT,
	qtrue,					//MOD_AKIMBO_SILENCEDLUGER,
	qfalse,					//MOD_SMOKEGRENADE,
	qfalse,					//MOD_SWAP_PLACES,
	qfalse,					//MOD_SWITCHTEAM,
	qfalse,					//MOD_GOOMBA,
	qfalse,					//MOD_POISON,
	qfalse,					//MOD_FEAR,
	qfalse,					//MOD_CENSORED,
	qtrue,					//MOD_SHOTGUN,
	qfalse,					//MOD_BACKSTAB,
	qtrue,					//MOD_MOBILE_BROWNING,
	qtrue,					//MOD_BAR,
	qtrue,					//MOD_STG44,
	qfalse,					//MOD_BAZOOKA,
	qtrue,					//MOD_STEN_MKII,
	qtrue,					//MOD_MP34,
	qtrue,					//MOD_VENOM,
	qfalse,					//MOD_SHOVE,
	qfalse,					//MOD_THROWKNIFE,
};


// core: an array for fast lookups during headshot-modes (g_headshot)..
// (To quickly determine if You missed a headshot, and G_Damage() can bail out early)
qboolean modHeadshotModeWeapon[MOD_NUM_MODS] =
{
	qfalse,					//MOD_UNKNOWN,
	qtrue,					//MOD_MACHINEGUN,
	qtrue,					//MOD_BROWNING,
	qtrue,					//MOD_MG42,
	qfalse,					//MOD_GRENADE,
	qfalse,					//MOD_KNIFE,
	qtrue,					//MOD_LUGER,
	qtrue,					//MOD_COLT,
	qtrue,					//MOD_MP40,
	qtrue,					//MOD_THOMPSON,
	qtrue,					//MOD_STEN,
	qtrue,					//MOD_GARAND,
	qtrue,					//MOD_SILENCER,
	qtrue,					//MOD_FG42,
	qtrue,					//MOD_FG42_SCOPE,
	qfalse,					//MOD_PANZERFAUST,
	qfalse,					//MOD_GRENADE_LAUNCHER,
	qfalse,					//MOD_FLAMETHROWER,
	qfalse,					//MOD_GRENADE_PINEAPPLE,
	qfalse,					//MOD_MAPMORTAR,
	qfalse,					//MOD_MAPMORTAR_SPLASH,
	qfalse,					//MOD_KICKED,
	qfalse,					//MOD_DYNAMITE,
	qfalse,					//MOD_AIRSTRIKE,
	qfalse,					//MOD_SYRINGE,
	qfalse,					//MOD_AMMO,
	qfalse,					//MOD_ARTY,
	qfalse,					//MOD_WATER,
	qfalse,					//MOD_SLIME,
	qfalse,					//MOD_LAVA,
	qfalse,					//MOD_CRUSH,
	qfalse,					//MOD_TELEFRAG,
	qfalse,					//MOD_FALLING,
	qfalse,					//MOD_SUICIDE,
	qfalse,					//MOD_TARGET_LASER,
	qfalse,					//MOD_TRIGGER_HURT,
	qfalse,					//MOD_EXPLOSIVE,
	qtrue,					//MOD_CARBINE,
	qtrue,					//MOD_KAR98,
	qfalse,					//MOD_GPG40,
	qfalse,					//MOD_M7,
	qfalse,					//MOD_LANDMINE,
	qfalse,					//MOD_SATCHEL,
	qfalse,					//MOD_SMOKEBOMB,
	qtrue,					//MOD_MOBILE_MG42,
	qtrue,					//MOD_SILENCED_COLT,
	qtrue,					//MOD_GARAND_SCOPE,
	qfalse,					//MOD_CRUSH_CONSTRUCTION,
	qfalse,					//MOD_CRUSH_CONSTRUCTIONDEATH,
	qfalse,					//MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER,
	qtrue,					//MOD_K43,
	qtrue,					//MOD_K43_SCOPE,
	qfalse,					//MOD_MORTAR,
	qtrue,					//MOD_AKIMBO_COLT,
	qtrue,					//MOD_AKIMBO_LUGER,
	qtrue,					//MOD_AKIMBO_SILENCEDCOLT,
	qtrue,					//MOD_AKIMBO_SILENCEDLUGER,
	qfalse,					//MOD_SMOKEGRENADE,
	qfalse,					//MOD_SWAP_PLACES,
	qfalse,					//MOD_SWITCHTEAM,
	qfalse,					//MOD_GOOMBA,
	qfalse,					//MOD_POISON,
	qfalse,					//MOD_FEAR,
	qfalse,					//MOD_CENSORED,
	qtrue,					//MOD_SHOTGUN,
	qfalse,					//MOD_BACKSTAB,
	qtrue,					//MOD_MOBILE_BROWNING,
	qtrue,					//MOD_BAR,
	qtrue,					//MOD_STG44,
	qfalse,					//MOD_BAZOOKA,
	qtrue,					//MOD_STEN_MKII,
	qtrue,					//MOD_MP34,
	qtrue,					//MOD_VENOM,
	qfalse,					//MOD_SHOVE,
	qfalse,					//MOD_THROWKNIFE,
};


/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) SUSPENDED SPIN PERSISTANT
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
SUSPENDED - will allow items to hang in the air, otherwise they are dropped to the next surface.
SPIN - will allow items to spin in place.
PERSISTANT - some items (ex. clipboards) can be picked up, but don't disappear

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
"stand" if the item has a stand (ex: mp40_stand.md3) this specifies which stand tag to attach the weapon to ("stand":"4" would mean "tag_stand4" for example)  only weapons support stands currently
*/

// JOSEPH 5-2-00
//----(SA) the addition of the 'ammotype' field was added by me, not removed by id (SA)
gitem_t	bg_itemlist[] =
{
	//---------------------------------
	// index 0
	{
		NULL,					// classname
		-1,						// hash
		NULL,					// pickup_sound
		{
			0,					// world_model[0]
			0,					// world_model[1]
			0					// world_model[2]
		},
		NULL,					// icon
		NULL,					// ammoicon
		NULL,					// pickup_name
		0,						// quantity
		0,						// giType
		0,						// giTag
		0,						// giAmmoIndex
		0						// giClipIndex
	},	// leave index 0 alone

	//----------------------------------
	// index 1: powerups, item & addons
	{
		"item_adrenaline",
		197679,
		"sound/player/default/blank.wav",
		{ "models/powerups/adrenaline.md3", 0, 0 },
		NULL, NULL,
		"Adrenaline",
		10000,
		IT_POWERUP, PW_ADRENALINE, 0, 0
	},

	// index 2
	{
		"item_flakJacket",
		197306,
		"sound/pickup/powerups/flakjacket.wav",
		{ "models/powerups/flakjacket.md3", 0, 0 },
		NULL, NULL,
		"Flak Jacket",
		INT_MAX,					// undepletable
		IT_POWERUP, PW_FLAKJACKET, 0, 0
	},

	// index 3
	{
		"item_helmetshield",
		228322,
		"sound/pickup/powerups/helmet.wav",
		{ "models/players/temperate/axis/helmetArmor.md3", 0, 0 },
		NULL, NULL,
		"Helmet Armor",
		INT_MAX,					// undepletable
		IT_POWERUP, PW_HELMETSHIELD, 0, 0
	},

	// index 4
	{
		"item_helmet",
		144483,
		"sound/pickup/powerups/helmet.wav",
		{ "models/powerups/helmet.md3", 0, 0 },
		NULL, NULL,
		"Helmet",
		INT_MAX,					// undepletable
		IT_POWERUP, PW_HELMET, 0, 0
	},

	// index 5
	{
		"item_axisSoldierUniform",
		323204,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/axisSoldier.md3", 0, 0 },
		NULL, NULL,
		"Axis Soldier Uniform",
		0,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_AXIS, 0
	},

	// index 6
	{
		"item_axisMedicUniform",
		289724,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/axisMedic.md3", 0, 0 },
		NULL, NULL,
		"Axis Medic Uniform",
		1,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_AXIS, 0
	},

	// index 7
	{
		"item_axisEngineerOpsUniform",
		384978,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/axisEngineer.md3", 0, 0 },
		NULL, NULL,
		"Axis Engineer Uniform",
		2,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_AXIS, 0
	},

	// index 8
	{
		"item_axisFieldOpsUniform",
		337599,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/axisFieldOps.md3", 0, 0 },
		NULL, NULL,
		"Axis Field Ops Uniform",
		3,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_AXIS, 0
	},

	// index 9
	{
		"item_axisCovertOpsUniform",
		357664,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/axisCovertOps.md3", 0, 0 },
		NULL, NULL,
		"Axis Covert Ops Uniform",
		4,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_AXIS, 0
	},

	// index 10
	{
		"item_alliedSoldierUniform",
		349684,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/alliedSoldier.md3", 0, 0 },
		NULL, NULL,
		"Allied Soldier Uniform",
		0,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_ALLIES, 0
	},

	// index 11
	{
		"item_alliedMedicUniform",
		315724,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/alliedMedic.md3", 0, 0 },
		NULL, NULL,
		"Allied Medic Uniform",
		1,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_ALLIES, 0
	},

	// index 12
	{
		"item_alliedEngineerOpsUniform",
		412316,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/alliedEngineer.md3", 0, 0 },
		NULL, NULL,
		"Allied Engineer Uniform",
		2,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_ALLIES, 0
	},

	// index 13
	{
		"item_alliedFieldOpsUniform",
		364279,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/alliedFieldOps.md3", 0, 0 },
		NULL, NULL,
		"Allied Field Ops Uniform",
		3,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_ALLIES, 0
	},

	// index 14
	{
		"item_alliedCovertOpsUniform",
		384630,
		"sound/pickup/powerups/uniform.wav",
		{ "models/powerups/uniforms/alliedCovertOps.md3", 0, 0 },
		NULL, NULL,
		"Allied Covert Ops Uniform",
		4,							// disguise type
		IT_POWERUP, PW_OPS_DISGUISED, TEAM_ALLIES, 0
	},

	//----------------------------------
	// index 15: health
	{
		"item_health_cabinet",
		253047,
		"sound/misc/health_pickup.wav",
        { 0, 0, 0 },
		NULL, NULL,
		"Health",
		0,
		IT_HEALTH, 0, 0, 0
	},

	// index 16
	{
		"item_health",
		143342,
		"sound/misc/health_pickup.wav",
        { "models/multiplayer/medpack/medpack_pickup.md3", 0, 0 },
		NULL, NULL,
		"Med Health",
		20,
		IT_HEALTH, WP_MEDKIT, WP_MEDKIT, WP_MEDKIT
	},

	// index 17
	{
		"item_health_large",
		225243,
		"sound/misc/health_pickup.wav",
        { "models/multiplayer/medpack/medpack_pickup.md3", 0, 0 },
		NULL, NULL,
		"Med Health",
		50,
		IT_HEALTH, 0, 0, 0
	},

	// index 18
	{
		"item_health_small",
		227098,
		"sound/items/n_health.wav",
		{ "models/powerups/health/health_s.md3", 0, 0 },
		NULL, NULL,
		"Small Health",
		5,
		IT_HEALTH, 0, 0, 0
	},

	// index 19
	/*
	multi-stage health item.
	gives 40 on first use, then gives 20 on "finishing up"
	player will only eat what he needs.  health at 90, turkey fills up and leaves remains (leaving 15).
	health at 5 you eat the whole thing.
	*/
	{
		"item_health_turkey",
		245923,
		"sound/items/hot_pickup.wav",
		{
			"models/powerups/health/health_t3.md3",	// just plate (should now be destructable)
			"models/powerups/health/health_t2.md3",	// half eaten
        	"models/powerups/health/health_t1.md3"	// whole turkey
		},
		NULL, NULL,
		"Hot Meal",
		20,							// amount given in last stage
		IT_HEALTH, 0, 0, 0
	},

	// index 20
	/*
	multi-stage health item.
	gives 30 on first use, then gives 15 on "finishing up"
	*/
	{
		"item_health_breadandmeat",
		325011,
		"sound/items/cold_pickup.wav",
		{
			"models/powerups/health/health_b3.md3",	// just plate (should now be destructable)
			"models/powerups/health/health_b2.md3",	// half eaten
        	"models/powerups/health/health_b1.md3"	// whole turkey
		},
		NULL, NULL,
		"Cold Meal",
		15,							// amount given in last stage
		IT_HEALTH, 0, 0, 0
	},

	// index 21
	{
		"item_health_wall",
		212921,
		"sound/items/n_health.wav",
        { "models/powerups/health/health_w.md3", 0, 0 },
		NULL, NULL,
		"Health",
		25,
		IT_HEALTH, 0, 0, 0
	},

	//---------------------------------
	// index 22: weapons
	{
		"weapon_mp40",
		131656,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/mp40/mp40.md3", "models/weapons2/mp40/v_mp40.md3", 0 },
		"icons/iconw_mp40_1", "icons/ammo2",
		"MP40",
		30,
		IT_WEAPON, WP_MP40, WP_MP40, WP_MP40
	},

	// index 23
	{
		"weapon_thompson",
		205850,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/thompson/thompson.md3", "models/weapons2/thompson/v_thompson.md3", 0 },
		"icons/iconw_thompson_1", "icons/ammo2",
		"Thompson",
		30,
		IT_WEAPON, WP_THOMPSON, WP_THOMPSON, WP_THOMPSON
	},

	// index 24
	{
		"weapon_akimboluger",
		244201,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/luger/luger.md3", "models/weapons2/akimbo_luger/v_akimbo_luger.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Akimbo Luger",
		50,
		IT_WEAPON, WP_AKIMBO_LUGER, WP_LUGER, WP_AKIMBO_LUGER
	},

	// index 25
	{
		"weapon_akimbosilencedluger",
		362158,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/luger/luger.md3", "models/weapons2/akimbo_luger/v_akimbo_luger.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Silenced Akimbo Luger",
		50,
		IT_WEAPON, WP_AKIMBO_SILENCEDLUGER, WP_LUGER, WP_AKIMBO_LUGER
	},

	// index 26
	{
		"weapon_luger",
		160350,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/luger/luger.md3", "models/weapons2/luger/v_luger.md3", 0 },
		"", "icons/ammo2",
		"Luger",
		50,
		IT_WEAPON, WP_LUGER, WP_LUGER, WP_LUGER
	},

	// index 27
	{
		"weapon_silencedcolt",
		258260,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/colt/colt.md3", "models/multiplayer/silencedcolt/v_silencedcolt.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Silenced Colt",
		50,
		IT_WEAPON, WP_SILENCED_COLT, WP_COLT, WP_COLT
	},

	// index 28
	{
		"weapon_sten",
		147190,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/sten/sten.md3", "models/weapons2/sten/v_sten.md3", 0 },
		"icons/iconw_sten_1", "icons/ammo2",
		"Sten",
		30,
		IT_WEAPON, WP_STEN, WP_STEN, WP_STEN
	},

	// index 29
	{
		"weapon_colt",
		146209,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/colt/colt.md3", "models/weapons2/colt/v_colt.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Colt",
		50,
		IT_WEAPON, WP_COLT, WP_COLT, WP_COLT
	},

	// index 30
	{
		"weapon_akimbocolt",
		229406,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/colt/colt.md3", "models/weapons2/akimbo_colt/v_akimbo_colt.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Akimbo Colt",
		50,
		IT_WEAPON, WP_AKIMBO_COLT, WP_COLT, WP_AKIMBO_COLT
	},

	// index 31
	{
		"weapon_akimbosilencedcolt",
		346491,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/colt/colt.md3", "models/weapons2/akimbo_colt/v_akimbo_colt.md3", 0 },
		"icons/iconw_colt_1", "icons/ammo2",
		"Silenced Akimbo Colt",
		50,
		IT_WEAPON, WP_AKIMBO_SILENCEDCOLT, WP_COLT, WP_AKIMBO_COLT
	},

	// index 32
	{
		"weapon_knife",
		158030,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/knife/knife.md3", "models/multiplayer/knife/v_knife.md3", 0 },
		"icons/iconw_knife_1", "icons/ammo2",
		"Knife",
		50,
		IT_WEAPON, WP_KNIFE, WP_KNIFE, WP_KNIFE
	},

	// index 33
	{
		"weapon_panzerfaust",
		248507,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/panzerfaust/pf.md3", "models/weapons2/panzerfaust/v_pf.md3", 0 },
		"icons/iconw_panzerfaust_1", "icons/ammo6",
		"Panzerfaust",
		1,
		IT_WEAPON, WP_PANZERFAUST, WP_PANZERFAUST, WP_PANZERFAUST
	},

	// index 34
	{
		"weapon_grenadelauncher",
		300497,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/grenade/grenade.md3", "models/weapons2/grenade/v_grenade.md3", 0 },
		"icons/iconw_grenade_1", "icons/icona_grenade",
		"Grenade",
		6,
		IT_WEAPON, WP_GRENADE_LAUNCHER, WP_GRENADE_LAUNCHER, WP_GRENADE_LAUNCHER
	},

	// index 35
	{
		"weapon_grenadepineapple",
		315692,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/grenade/pineapple.md3", "models/weapons2/grenade/v_pineapple.md3", 0 },
		"icons/iconw_pineapple_1", "icons/icona_pineapple",
		"Pineapple",
		6,
		IT_WEAPON, WP_GRENADE_PINEAPPLE, WP_GRENADE_PINEAPPLE, WP_GRENADE_PINEAPPLE
	},

	// index 36
	{
		"weapon_grenadesmoke",
		257741,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/smokegrenade/smokegrenade.md3", "models/multiplayer/smokegrenade/v_smokegrenade.md3", 0 },
		"icons/iconw_smokegrenade_1", "icons/ammo2",
		"smokeGrenade",
		50,
		IT_WEAPON, WP_SMOKE_MARKER, WP_SMOKE_MARKER, WP_SMOKE_MARKER
	},

	// index 37
	/* JPW NERVE
	weapon_smoketrail -- only used as a special effects emitter for smoke trails (artillery spotter etc)
	*/
	{
		"weapon_smoketrail",
		232119,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/smokegrenade/smokegrenade.md3", "models/multiplayer/smokegrenade/v_smokegrenade.md3", 0 },
		"icons/iconw_smokegrenade_1", "icons/ammo2",
		"smokeTrail",
		50,
		IT_WEAPON, WP_SMOKETRAIL, WP_SMOKETRAIL, WP_SMOKETRAIL
	},

	// index 38
	{
		"weapon_medic_heal",
		223810,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/medpack/medpack.md3", "models/multiplayer/medpack/v_medpack.md3", 0 },
		"icons/iconw_medheal_1", "icons/ammo2",
		"medicheal",
		50,
		IT_WEAPON, WP_MEDKIT, WP_MEDKIT, WP_MEDKIT
	},

	// index 39
	{
		"weapon_dynamite",
		202080,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/dynamite/dynamite_3rd.md3", "models/weapons2/dynamite/v_dynamite.md3", 0 },
		"icons/iconw_dynamite_1", "icons/ammo9",
		"Dynamite Weapon",
		7,
		IT_WEAPON, WP_DYNAMITE, WP_DYNAMITE, WP_DYNAMITE
	},

	// index 40
	{
		"weapon_flamethrower",
		261404,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/flamethrower/flamethrower.md3", "models/weapons2/flamethrower/v_flamethrower.md3", "models/weapons2/flamethrower/pu_flamethrower.md3" },
		"icons/iconw_flamethrower_1", "icons/ammo10",
		"Flamethrower",
		200,
		IT_WEAPON, WP_FLAMETHROWER, WP_FLAMETHROWER, WP_FLAMETHROWER
	},

	// index 41
	/*
	JPW NERVE -- class-specific multiplayer weapon, can't be picked up, dropped, or placed in map
	*/
	{
		"weapon_class_special",
		271134,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/pliers/pliers.md3", "models/multiplayer/pliers/v_pliers.md3", 0 },
		"icons/iconw_pliers_1", "icons/ammo2",
		"Special",
		50,							// this should never be picked up
		IT_WEAPON, WP_PLIERS, WP_PLIERS, WP_PLIERS
	},

	// index 42
	{
		"weapon_arty",
		148007,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/syringe/syringe.md3", "models/multiplayer/syringe/v_syringe.md3", 0 },
		"icons/iconw_syringe_1", "icons/ammo2",
		"Artillery",
		50,							// this should never be picked up
		IT_WEAPON, WP_ARTY, WP_ARTY, WP_ARTY
	},

	// index 43
	{
		"weapon_medic_syringe",
		272804,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/syringe/syringe.md3", "models/multiplayer/syringe/v_syringe.md3", 0 },
		"icons/iconw_syringe_1", "icons/ammo2",
		"Syringe",
		50,							// this should never be picked up
		IT_WEAPON, WP_MEDIC_SYRINGE, WP_MEDIC_SYRINGE, WP_MEDIC_SYRINGE
	},

	// index 44
	{

		"weapon_poison_syringe",
		293216,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/syringe/syringe.md3", "models/multiplayer/syringe/v_syringe.md3", 0 },
		"icons/iconw_poison_1", "icons/ammo2",
		"Poison Syringe",
		50,							// this should never be picked up
		IT_WEAPON, WP_POISON_SYRINGE, WP_POISON_SYRINGE, WP_POISON_SYRINGE
	},

	// index 45
	{
		"weapon_medic_adrenaline",
		311475,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/syringe/syringe.md3", "models/multiplayer/syringe/v_syringe.md3", 0 },
		"icons/iconw_syringe_1", "icons/ammo2",
		"Adrenaline Syringe",
		50, // this should never be picked up
		IT_WEAPON, WP_MEDIC_ADRENALINE, WP_MEDIC_ADRENALINE, WP_MEDIC_ADRENALINE
	},

	// index 46
	{
		"weapon_magicammo",
		212968,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/ammopack/ammopack.md3", "models/multiplayer/ammopack/v_ammopack.md3", "models/multiplayer/ammopack/ammopack_pickup.md3" },
		"icons/iconw_ammopack_1", "icons/ammo2",
		"Ammo Pack",
		50,							// this should never be picked up
		IT_WEAPON, WP_AMMO, WP_AMMO, WP_AMMO
	},

	// index 47
	{
		"weapon_magicammo2",
		219718,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/binocs/v_binocs.md3", "models/multiplayer/binocs/v_binocs.md3", "models/multiplayer/binocs/v_binocs.md3" },
		"icons/iconw_ammopack_1", "icons/ammo2",
		"Mega Ammo Pack",
		50,							// this should never be picked up
		IT_WEAPON, WP_AMMO, WP_AMMO, WP_AMMO
	},

	// index 48
	{
		"weapon_magicammo3",
		219853,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/binocs/v_binocs.md3", "models/multiplayer/binocs/v_binocs.md3", "models/multiplayer/binocs/v_binocs.md3" },
		"icons/iconw_ammopack_1", "icons/ammo2",
		"Huge Ammo Pack",
		50,							// this should never be picked up
		IT_WEAPON, WP_AMMO, WP_AMMO, WP_AMMO
	},

	// index 49
	{
		"weapon_binoculars",
		231087,
		"sound/misc/w_pkup.wav",
        { "", "models/multiplayer/binocs/v_binocs.md3", 0 },
		"", "",
		"Binoculars",
		50,							// this should never be picked up
		IT_WEAPON, WP_BINOCULARS, WP_BINOCULARS, WP_BINOCULARS
	},

	// index 50
	{
		"weapon_kar43",
		144581,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/kar98/kar98_3rd.md3", "models/multiplayer/kar98/v_kar98.mdc", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_mauser_1", "icons/ammo3",
		"K43 Rifle",
		50,
		IT_WEAPON, WP_K43, WP_K43, WP_K43
	},

	// index 51
	{
		"weapon_kar43_scope",
		229103,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/kar98/kar98_3rd.md3", "models/multiplayer/kar98/v_kar98.mdc", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_mauser_1", "icons/ammo3",
		"K43 Rifle Scope",
		50,
		IT_WEAPON, WP_K43_SCOPE, WP_K43, WP_K43
	},

	// index 52
	{
		"weapon_kar98Rifle",
		216343,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/kar98/kar98_3rd.md3", "models/multiplayer/kar98/v_kar98.mdc", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_kar98_1", "icons/ammo3",
		"K43",
		50,
		IT_WEAPON, WP_KAR98, WP_KAR98, WP_KAR98
	},

	// index 53
	{
		"weapon_gpg40",
		144184,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/kar98/kar98_3rd.md3", "models/multiplayer/kar98/v_kar98.mdc", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_kar98_1", "icons/ammo10",
		"GPG40",
		200,
		IT_WEAPON, WP_GPG40, WP_GPG40, WP_GPG40
	},

	// index 54
	{
		"weapon_gpg40_allied",
		239880,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/m1_garand/m1_garand_3rd.md3", "models/multiplayer/m1_garand/v_m1_garand.md3", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_m1_garand_1", "icons/ammo10",
		"GPG40A",
		200,
		IT_WEAPON, WP_M7, WP_M7, WP_M7
	},

	// index 55
	{
		"weapon_M1CarbineRifle",
		278261,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/m1_garand/m1_garand_3rd.md3", "models/multiplayer/m1_garand/v_m1_garand.md3", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_m1_garand_1", "icons/ammo3",
		"M1 Garand",
		50,
		IT_WEAPON, WP_CARBINE, WP_CARBINE, WP_CARBINE
	},

	// index 56
	{
		"weapon_garandRifle",
		241649,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/m1_garand/m1_garand_3rd.md3", "models/multiplayer/m1_garand/v_m1_garand.md3", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_mauser_1", "icons/ammo3",
		"Garand",
		50,
		IT_WEAPON, WP_GARAND, WP_GARAND, WP_GARAND
	},

	// index 57
	{
		"weapon_garandRifleScope",
		316416,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/m1_garand/m1_garand_3rd.md3", "models/multiplayer/m1_garand/v_m1_garand.md3", "models/multiplayer/mauser/mauser_pickup.md3" },
		"icons/iconw_mauser_1", "icons/ammo3",
		"M1 Garand Scope",
		50,
		IT_WEAPON, WP_GARAND_SCOPE, WP_GARAND, WP_GARAND
	},

	// index 58
	{
		"weapon_fg42",
		129889,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/fg42/fg42.md3", "models/weapons2/fg42/v_fg42.md3", "models/weapons2/fg42/pu_fg42.md3" },
		"icons/iconw_fg42_1", "icons/ammo5",
		"FG42 Paratroop Rifle",
		10,
		IT_WEAPON, WP_FG42, WP_FG42, WP_FG42
	},

	// index 59
	{
		"weapon_fg42scope",
		200890,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/fg42/fg42.md3", "models/weapons2/fg42/v_fg42.md3", "models/weapons2/fg42/pu_fg42.md3" },
		"icons/iconw_fg42_1", "icons/ammo5",
		"FG42 Scope",
		0,
		IT_WEAPON, WP_FG42SCOPE, WP_FG42, WP_FG42
	},

	// index 60
	{
		"weapon_mortar",
		175781,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/mortar/mortar_3rd.md3", "models/multiplayer/mortar/v_mortar.md3", 0 },
		"icons/iconw_mortar_1", "icons/ammo5",
		"Mortar",
		0,
		IT_WEAPON, WP_MORTAR, WP_MORTAR, WP_MORTAR
	},

	// index 61
	{
		"weapon_mortar_set",
		232810,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/mortar/mortar_3rd.md3", "models/multiplayer/mortar/v_mortar.md3", 0 },
		"icons/iconw_mortar_1", "icons/ammo5",
		"Mounted Mortar",
		0,
		IT_WEAPON, WP_MORTAR_SET, WP_MORTAR, WP_MORTAR
	},

	// index 62
	{
		"weapon_mortar2",
		182381,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/mortar/mortar_3rd.md3", "models/multiplayer/mortar/v_mortar.md3", 0 },
		"icons/iconw_mortar_1", "icons/ammo5",
		"Granatwerfer",
		0,
		IT_WEAPON, WP_MORTAR2, WP_MORTAR, WP_MORTAR
	},

	// index 63
	{
		"weapon_mortar2_set",
		239837,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/mortar/mortar_3rd.md3", "models/multiplayer/mortar/v_mortar.md3", 0 },
		"icons/iconw_mortar_1", "icons/ammo5",
		"Mounted Granatwerfer",
		0,
		IT_WEAPON, WP_MORTAR2_SET, WP_MORTAR, WP_MORTAR
	},

	// index 64
	{
		"weapon_landmine",
		199635,
		"",
		{ "models/multiplayer/landmine/landmine.md3", "models/multiplayer/landmine/v_landmine.md3", 0 },
		"icons/iconw_landmine_1", "icons/ammo9",
		"Landmine",
		7,
		IT_WEAPON, WP_LANDMINE, WP_LANDMINE, WP_LANDMINE
	},

	// index 65
	{
		"weapon_satchel",
		186285,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/satchel/satchel.md3", "models/multiplayer/satchel/v_satchel.md3", 0 },
		"icons/iconw_satchel_1", "icons/ammo2",
		"Satchel Charge",
		0,
		IT_WEAPON, WP_SATCHEL, WP_SATCHEL, WP_SATCHEL
	},

	// index 66
	{
		"weapon_satchelDetonator",
		320069,
		"",
        { "models/multiplayer/satchel/radio.md3", "models/multiplayer/satchel/v_satchel.md3", 0 },
		"icons/iconw_radio_1", "icons/ammo2",
		"Satchel Charge Detonator",
		0,
		IT_WEAPON, WP_SATCHEL_DET, WP_SATCHEL_DET, WP_SATCHEL_DET
	},

	// index 67
	{
		"weapon_smokebomb",
		215443,
		"",
        { "models/multiplayer/smokebomb/smokebomb.md3", "models/multiplayer/smokebomb/v_smokebomb.md3", 0 },
		"icons/iconw_dynamite_1", "icons/ammo9",
		"Smoke Bomb",
		0,
		IT_WEAPON, WP_SMOKE_BOMB, WP_SMOKE_BOMB, WP_SMOKE_BOMB
	},

	// index 68
	{
		"weapon_mobile_mg42",
		226700,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/mg42/mg42_3rd.md3", "models/multiplayer/mg42/v_mg42.md3", 0 },
		"icons/iconw_mg42_1", "icons/ammo2",
		"Mobile MG42",
		30,
		IT_WEAPON, WP_MOBILE_MG42, WP_MOBILE_MG42, WP_MOBILE_MG42
	},

	// index 69
	{
		"weapon_mobile_mg42_set",
		285864,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/mg42/mg42_3rd.md3", "models/multiplayer/mg42/v_mg42.md3", 0 },
		"icons/iconw_mg42_1", "icons/ammo2",
		"Mobile MG42 Bipod",
		30,
		IT_WEAPON, WP_MOBILE_MG42_SET, WP_MOBILE_MG42, WP_MOBILE_MG42
	},

	// index 70
	{
		"weapon_silencer",
		201291,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/silencer/silencer.md3", "models/weapons2/silencer/v_silencer.md3", "models/weapons2/silencer/pu_silencer.md3" },
		"icons/iconw_silencer_1", "icons/ammo5",
		"sp5 pistol",
		10,
		IT_WEAPON, WP_SILENCER, WP_LUGER, WP_LUGER
	},

	// index 71
	{
		"weapon_dummy",
		162052,
		"",
        { 0, 0, 0 },
		"", "",
		"BLANK",
		0,
		IT_WEAPON, WP_DUMMY_MG42, WP_DUMMY_MG42, WP_DUMMY_MG42
	},

	// index 72
	{
		"weapon_shotgun",
		190957,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/winchester/winchester_3rd.md3", "models/weapons2/winchester/v_winchester.md3", 0 },
		"icons/iconw_shotgun_1_wide", "icons/ammo2",
		"Shotgun",
		8,
		IT_WEAPON, WP_SHOTGUN, WP_SHOTGUN, WP_SHOTGUN
	},

	// index 73
	{
		"weapon_knife_kabar",
		239231,
		"sound/misc/w_pkup.wav",
        { "models/multiplayer/knife_kbar/knife.md3", "models/multiplayer/knife_kbar/v_knife.md3", 0 },
		"icons/iconw_knife_1", "icons/ammo2",
		"Ka-Bar",
		50,
		IT_WEAPON, WP_KNIFE_KABAR, WP_KNIFE_KABAR, WP_KNIFE_KABAR
	},

	// index 74
	{
		"weapon_mobile_browning_set",
		364202,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/browning/brown30cal_3rd.md3", "models/weapons2/browning/v_brown30cal.md3", 0 },
		"icons/iconw_browning_1", "icons/ammo2",
		"Mobile Browning Bipod",
		30,
		IT_WEAPON, WP_MOBILE_BROWNING_SET, WP_MOBILE_BROWNING, WP_MOBILE_BROWNING
	},

	// index 75
	{
		"weapon_mobile_browning",
		303330,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/browning/brown30cal_3rd.md3", "models/weapons2/browning/v_brown30cal.md3", 0 },
		"icons/iconw_browning_1", "icons/ammo2",
		"Mobile Browning",
		30,
		IT_WEAPON, WP_MOBILE_BROWNING, WP_MOBILE_BROWNING, WP_MOBILE_BROWNING
	},

	// index 76
	{
		"weapon_StG44",
		146724,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/StG44/mp44.md3", "models/weapons2/StG44/v_mp44.md3", 0 },
		"icons/iconw_stg44_1", "icons/ammo2",
		"StG44",
		30,
		IT_WEAPON, WP_STG44, WP_STG44, WP_STG44
	},

	// index 77
	{
		"weapon_BAR_set",
		185857,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/bar/bar3rd_bipod.md3", "models/weapons2/bar/v_bar.md3", 0 },
		"icons/iconw_bar_1", "icons/ammo2",
		"BAR Bipod",
		30,
		IT_WEAPON, WP_BAR_SET, WP_BAR, WP_BAR
	},

	// index 78
	{
		"weapon_BAR",
		130109,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/bar/bar3rd.md3", "models/weapons2/bar/v_bar.md3", 0 },
		"icons/iconw_bar_1", "icons/ammo2",
		"BAR",
		10,
		IT_WEAPON, WP_BAR, WP_BAR, WP_BAR
	},

	// index 79
	{
		"weapon_StenMkII",
		203202,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/StenMkII/Sss_sten.md3", "models/weapons2/StenMkII/Sv_sten.md3", 0 },
		"icons/iconw_stenMkII_1", "icons/ammo2",
		"StenMkII",
		30,
		IT_WEAPON, WP_STEN_MKII, WP_STEN, WP_STEN
	},

	// index 80
	{
		"weapon_bazooka",
		186703,
		"sound/misc/w_pkup.wav",
        { "models/weapons2/panzerfaust/bazooka_3rd.md3", "models/weapons2/panzerfaust/v_bazooka.md3", 0 },
		"icons/iconw_panzerfaust_1", "icons/ammo6",
		"Bazooka",
		1,
		IT_WEAPON, WP_BAZOOKA, WP_BAZOOKA, WP_BAZOOKA
	},

	// index 81
	{
		"weapon_mp34",
		132044,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/mp34/mp34_3rd.md3", "models/weapons2/mp34/v_mp34.md3", 0 },
		"icons/iconw_sten_1", "icons/ammo2",
		"MP34",
		30,
		IT_WEAPON, WP_MP34, WP_MP34, WP_MP34
	},

	// index 82
	{
		"weapon_venom",
		161114,
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/venom/pu_venom.md3", "models/weapons2/venom/v_venom.md3", 0 },
		"icons/iconw_venom", "icons/ammo2",
		"Venom",
		30,
		IT_WEAPON, WP_VENOM, WP_VENOM, WP_VENOM
	},

	// index 83
	{
		"weapon_dogtags",
		186978,
		"sound/misc/w_pkup.wav",
		{ "models/multiplayer/binocs/pickup.md3", 0, 0 },
		"models/multiplayer/dogtags/dogtag.jpg", NULL,
		"Dog Tag",
		INT_MAX,					// undepletable
		IT_WEAPON, ITEM_DOGTAGS, 0, 0
	},

	//---------------------------------
	// index 84
	// jaquboss here should be all needed clip items, we don't need for example silenced ones, becuase they use same ammo as normals
	{
		"ammo_thompson",
		176263,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/thompson.md3", 0, 0 },
		"", NULL,
		"Thompson Clip",
		30,
		IT_AMMO, WP_THOMPSON, WP_THOMPSON, WP_THOMPSON
	},

	// index 85
	{
		"ammo_mp40",
		103203,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/mp40.md3", 0, 0 },
		"", NULL,
		"MP40 Clip",
		30,
		IT_AMMO, WP_MP40, WP_MP40, WP_MP40
	},

	// index 86
	{
		"ammo_dynamite",
		172551,
		"sound/misc/am_pkup.wav",
        { "models/ammo/dynamite/dynamite.md3", 0, 0 },
		"", NULL,
		"dynamite",
		1,
		IT_AMMO, WP_DYNAMITE, WP_DYNAMITE, WP_DYNAMITE
	},

	// index 87
	{
		"ammo_disguise",
		172837,
		"sound/misc/am_pkup.wav",
        { "models/ammo/disguise/disguise.md3", 0, 0 },
		"", NULL,
		"disguise",
		1,
		IT_AMMO, -1, -1, -1
	},

	// index 88
	{
		"ammo_airstrike",
		187714,
		"sound/misc/am_pkup.wav",
        { "models/ammo/smoke/smoke.md3", 0, 0 },
		"", NULL,
		"airstrike canister",
		1,
		IT_AMMO, WP_SMOKE_MARKER, WP_SMOKE_MARKER, WP_SMOKE_MARKER
	},

	// index 89
	{
		"ammo_landmine",
		170144,
		"sound/misc/am_pkup.wav",
        { "models/ammo/landmine/landmine.md3", 0, 0 },
		"", NULL,
		"landmine",
		1,
		IT_AMMO, WP_LANDMINE, WP_LANDMINE, WP_LANDMINE
	},

	// index 90
	{
		"ammo_satchel_charge",
		252572,
		"sound/misc/am_pkup.wav",
        { "models/ammo/satchel/satchel.md3", 0, 0 },
		"", NULL,
		"satchel charge",
		1,
		IT_AMMO, WP_SATCHEL, WP_SATCHEL, WP_SATCHEL
	},

	// index 91
	{
		"ammo_luger",
		131453,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/luger.md3", 0, 0 },
		"", NULL,
		"Luger Clip",
		8,
		IT_AMMO, WP_LUGER, WP_LUGER, WP_LUGER
	},

	// index 92
	{
		"ammo_smoke_grenade",
		240202,
		"sound/misc/am_pkup.wav",
        { "models/ammo/smoke_grenade/smoke_grenade.md3", 0, 0 },
		"", NULL,
		"smoke grenade",
		1,
		IT_AMMO, WP_SMOKE_BOMB, WP_SMOKE_BOMB, WP_SMOKE_BOMB
	},

	// index 93
	{
		"ammo_colt",
		117530,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/colt.md3", 0, 0 },
		"", NULL,
		"Colt Clip",
		8,
		IT_AMMO, WP_COLT, WP_COLT, WP_COLT
	},

	// index 94
	{
		"ammo_syringe",
		160620,
		"sound/misc/am_pkup.wav",
        { "models/ammo/syringe/syringe.md3", 0, 0 },
		"", NULL,
		"syringe",
		1,
		IT_AMMO, WP_MEDIC_SYRINGE, WP_MEDIC_SYRINGE, WP_MEDIC_SYRINGE
	},

	// index 95
	{
		"ammo_garand",
		141599,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/garand.md3", 0, 0 },
		"", NULL,
		"Garand Sniper Clip",
		8,
		IT_AMMO, WP_GARAND, WP_GARAND, WP_GARAND
	},

	// index 96
	{
		"ammo_carbine",
		155010,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/garand.md3", 0, 0 },
		"", NULL,
		"Garand Clip",
		8,
		IT_AMMO, WP_CARBINE, WP_CARBINE, WP_CARBINE
	},

	// index 97
	{
		"ammo_mg42",
		102332,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/ammoBelt.md3", 0, 0 },
		"", NULL,
		"MG42 Ammo Belt",
		150,
		IT_AMMO, WP_MOBILE_MG42, WP_MOBILE_MG42, WP_MOBILE_MG42
	},

	// index 98
	{
		"ammo_browning",
		173958,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/ammoBelt.md3", 0, 0 },
		"", NULL,
		"Browning Ammobox",
		150,
		IT_AMMO, WP_MOBILE_BROWNING, WP_MOBILE_BROWNING, WP_MOBILE_BROWNING
	},

	// index 99
	{
		"ammo_shotgun",
		161594,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/winchester.md3", 0, 0 },
		"", NULL,
		"Pack of Shells",
		8,
		IT_AMMO, WP_SHOTGUN, WP_SHOTGUN, WP_SHOTGUN
	},

	// index 100
	{
		"ammo_stg44",
		118037,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/stg44.md3", 0, 0 },
		"", NULL,
		"Stg44 Clip",
		30,
		IT_AMMO, WP_STG44, WP_STG44, WP_STG44
	},

	// index 101
	{
		"ammo_bar",
		101680,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/bar.md3", 0, 0 },
		"", NULL,
		"BAR Clip",
		20,
		IT_AMMO, WP_BAR, WP_BAR, WP_BAR
	},

	// index 102
	{
		"ammo_fg42",
		101464,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/fg42.md3", 0, 0 },
		"", NULL,
		"FG42 Clip",
		20,
		IT_AMMO, WP_FG42, WP_FG42, WP_FG42
	},

	// index 103
	{
		"ammo_mp34",
		103585,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/mp34.md3", 0, 0 },
		"", NULL,
		"MP34 Clip",
		32,
		IT_AMMO, WP_MP34, WP_MP34, WP_MP34
	},

	// index 104
	{
		"ammo_sten",
		118495,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/sten.md3", 0, 0 },
		"", NULL,
		"Sten Clip",
		32,
		IT_AMMO, WP_STEN, WP_STEN, WP_STEN
	},

	// index 105
	{
		"ammo_k43",
		89233,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/k43.md3", 0, 0 },
		"", NULL,
		"K43 Clip",
		10,
		IT_AMMO, WP_K43, WP_K43, WP_K43
	},

	// index 106
	{
		"ammo_kar98",
		117203,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/k43.md3", 0, 0 },
		"", NULL,
		"Kar43 Clip",
		10,
		IT_AMMO, WP_KAR98, WP_KAR98, WP_KAR98
	},

	// index 107
	{
		"ammo_venom",
		132205,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/ammoBelt.md3", 0, 0 },
		"", NULL,
		"Venom Ammo Belt",
		250,
		IT_AMMO, WP_VENOM, WP_VENOM, WP_VENOM
	},

	// index 108
	{
		"ammo_mortar",
		146648,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/mortar.md3", 0, 0 },
		"", NULL,
		"Mortar Shell",
		1,
		IT_AMMO, WP_MORTAR, WP_MORTAR, WP_MORTAR
	},

	// index 109
	{
		"ammo_bazooka",
		157406,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/bazooka.md3", 0, 0 },
		"", NULL,
		"Bazooka Rocket",
		1,
		IT_AMMO, WP_BAZOOKA, WP_BAZOOKA, WP_BAZOOKA
	},

	// index 110
	{
		"ammo_panzerfaust",
		218290,
		"sound/misc/am_pkup.wav",
		{ "models/ammo/panzerfaust.md3", 0, 0 },
		"", NULL,
		"Panzerfaust",
		1,
		IT_AMMO, WP_PANZERFAUST, WP_PANZERFAUST, WP_PANZERFAUST
	},

	//---------------------------------
	// index 111: team
	{
		"team_CTF_redflag",
		209279,
		"",
        { 0, 0, 0 },
		"", NULL,
		"Objective",
		0,
		IT_TEAM, PW_REDFLAG, 0, 0
	},

	// index 112: team
	{
		"team_CTF_blueflag",
		223985,
		"",
        { 0, 0, 0 },
		"", NULL,
		"Blue Flag",
		0,
		IT_TEAM, PW_BLUEFLAG, 0, 0
	},

	// end of list marker
	{NULL}
};
// END JOSEPH

int		bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;


/*
===============
BG_FindItemForWeapon

===============
*/
gitem_t	*BG_FindItemForWeapon( weapon_t weapon )
{
	gitem_t	*it;

	if ( weapon == WP_NONE ) {
		return NULL;
	}

	for ( it = bg_itemlist + INDEX_IT_WEAPON ; it->classname ; it++)
	{
		if ( it->giType == IT_WEAPON ) {
			if ( it->giTag == weapon )
			{
				return it;
			}
		}
		else {
			goto err;
		}
	}

err:
	// jet Pilot - don't disconnect! just return NULL
	Com_Printf("^1ERROR : ^7BG_FindItemForWeapon() ^9no item for weapon %i (%s^9)",
		weapon,  (weapon >= WP_NONE && weapon < WP_NUM_WEAPONS)  ? BG_Weapons[weapon].name  : "^1invalid");

	return NULL;
}

/*
==============
BG_FindClipForWeapon
==============
*/
weapon_t BG_FindClipForWeapon( weapon_t weapon) {
	gitem_t	*it;

	for ( it = bg_itemlist + INDEX_IT_WEAPON ; it->classname ; it++) {
		if ( it->giType == IT_WEAPON ) {
			if ( it->giTag == weapon ) {
				return it->giClipIndex;
			}
		}
		else {
			goto err;
		}
	}
err:
	return 0;
}

void Init_FindClipForWeapon( void ) {
	int	wp;
	for ( wp=WP_NONE; wp<WP_NUM_WEAPONS; wp++ ) {
		WeaponTable[wp].clipindex = BG_FindClipForWeapon(wp);
	}
}

/*
==============
BG_FindAmmoForWeapon
==============
*/
weapon_t BG_FindAmmoForWeapon( weapon_t weapon ) {
	gitem_t	*it;

	for ( it = bg_itemlist + INDEX_IT_WEAPON ; it->classname ; it++) {
		if ( it->giType == IT_WEAPON ) {
			if ( it->giTag == weapon ) {
				return it->giAmmoIndex;
			}
		}
		else {
			goto err;
		}
	}
err:
	return 0;
}

void Init_FindAmmoForWeapon( void ) {
	int	wp;
	for ( wp=WP_NONE; wp<WP_NUM_WEAPONS; wp++ ) {
		WeaponTable[wp].ammoindex = BG_FindAmmoForWeapon(wp);
	}
}

/*
==============
BG_AkimboFireSequence
	returns 'true' if it's the left hand's turn to fire, 'false' if it's the right hand's turn
==============
*/
qboolean BG_AkimboFireSequence( int weapon, int akimboClip, int mainClip ) {
	if( !IS_AKIMBO_WEAPON( weapon ) )
		return qfalse;

	if( !akimboClip )
		return qfalse;

	// no ammo in main weapon, must be akimbo turn
	if( !mainClip )
		return qtrue;

	// at this point, both have ammo

	// now check 'cycle'   // (removed old method 11/5/2001)
	if( (akimboClip + mainClip) & 1) {
		return qfalse;
	}

	return qtrue;
}

/*
==============
BG_IsAkimboSideArm
==============
*/
qboolean BG_IsAkimboSideArm( int weaponNum, playerState_t *ps ) {
	switch( weaponNum ) {
		case WP_COLT:	if( ps->weapon == WP_AKIMBO_COLT || ps->weapon == WP_AKIMBO_SILENCEDCOLT  )		return qtrue;	break;
		case WP_LUGER:	if( ps->weapon == WP_AKIMBO_LUGER || ps->weapon == WP_AKIMBO_SILENCEDLUGER )	return qtrue;	break;
		default: break;
	}
	return qfalse;
}

/*
==============
BG_AkimboSidearm
==============
*/
int BG_AkimboSidearm( int weaponNum ) {
	switch( weaponNum ) {
		case WP_AKIMBO_COLT:			return WP_COLT;				break;
		case WP_AKIMBO_SILENCEDCOLT:	return WP_COLT;				break;
		case WP_AKIMBO_LUGER:			return WP_LUGER;			break;
		case WP_AKIMBO_SILENCEDLUGER:	return WP_LUGER;			break;
		default:						return WP_NONE;				break;
	}
}

/*
==============
BG_FindItemForAmmo
==============
*/
gitem_t *BG_FindItemForAmmo(int ammo)
{
	int		i = INDEX_IT_AMMO;

	for (;i < bg_numItems; i++)
	{
		if ( bg_itemlist[i].giType == IT_AMMO ) {
			if ( bg_itemlist[i].giAmmoIndex == ammo )
				return &bg_itemlist[i];
		}
		else {
			goto err;
		}
	}
err:
	Com_Error( ERR_DROP, "Item not found for ammo: %d", ammo );
	return NULL;
}

/*
===============
BG_FindItem
===============
*/
gitem_t	*BG_FindItem( const char *pickupName ) {
	gitem_t	*it;

	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->pickup_name, pickupName ) )
			return it;
	}

	return NULL;
}

gitem_t	*BG_FindItemForClassName( const char *className ) {
	gitem_t	*it;
	int		startIndex = INDEX_IT_POWERUP;
	int		hash = BG_StringHashValue(className);

	// core: do a quick test on the classname prefix
	//       to get the startindex into the bg_itemlist array.
	if ( !Q_strncmp(className, "weapon_", 7) ) {
		startIndex = INDEX_IT_WEAPON;
	}
	else if ( !Q_strncmp(className, "ammo_", 5) ) {
		startIndex = INDEX_IT_AMMO;
	}
	else if ( !Q_strncmp(className, "item_health", 11) ) {
		startIndex = INDEX_IT_HEALTH;
	}
	else if ( !Q_strncmp(className, "team_", 5) ) {
		startIndex = INDEX_IT_TEAM;
	}

	for ( it = bg_itemlist + startIndex ; it->classname ; it++ ) {
		if ( it->classnamehash == hash )	// !Q_stricmp(it->classname, className)
			return it;
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
	vec3_t		origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin, qfalse, item->effect2Time );

	// we are ignoring ducked differences here
	if ( ps->origin[0] - origin[0] > 36
		|| ps->origin[0] - origin[0] < -36
		|| ps->origin[1] - origin[1] > 36
		|| ps->origin[1] - origin[1] < -36
		|| ps->origin[2] - origin[2] > 36
		|| ps->origin[2] - origin[2] < -36 ) {
		return qfalse;
	}

	return qtrue;
}

// core: no player can carry more than MAX_GRENADES grenades,
// not even an lvl 9 engineer with lvl 9 light weapons (8+1)..
#define MAX_GRENADES 8

float jP_GrenadesPerLevel[SK_NUM_SKILLS][NUM_SKILL_LEVELS] =
{
	//  0   1   2	3	4	5	6	7	8	9
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0	},		// Battle Sense (Bonus)
	{	4,	8,	8,	8,	8,	8,	8,	8,	8,	8	},		// Engineer
	{	1,	2,	2,	2,	2,	2,	4,	4,	4,	4	},		// Medic
	{	1,	2,	2,	2,	2,	2,	2,	2,	2,	2	},		// Signals
	{	0,	0,	0,	0,	0,	0,	1,	1,	1,	1	},		// Light Weapons (Bonus)
	{	4,	4,	4,	4,	4,	4,	5,	5,	5,	5	},		// Heavy Weapons
	{	2,	2,	2,	2,	2,	2,	2,	2,	2,	2	},		// Covert Ops
};

int SkillForClass(int cls)
{
	switch(cls) {
		case PC_MEDIC:
			return SK_MEDIC;
		case PC_ENGINEER:
			return SK_ENGINEER;
		case PC_FIELDOPS:
			return SK_SIGNALS;
		case PC_COVERTOPS:
			return SK_COVERTOPS;
		default:
			return SK_SOLDIER;
	}
}


/*
=================================
BG_AddMagicAmmo:
	if numOfClips is 0, no ammo is added, it just return whether any ammo CAN be added;
	otherwise return whether any ammo was ACTUALLY added.

WARNING: when numOfClips is 0, DO NOT CHANGE ANYTHING under ps.
=================================
*/

int BG_GrenadesForClass( int cls, int* skillBits )
{
	int skill = SkillForClass(cls);
	int nades = (int)BG_GetFromTable( jP_GrenadesPerLevel[skill], skillBits, skill );
	int bonus = (int)BG_GetFromTable( jP_GrenadesPerLevel[SK_LIGHT_WEAPONS], skillBits, SK_LIGHT_WEAPONS );

	nades += bonus;
	return ( nades <= MAX_GRENADES )? nades : MAX_GRENADES;
}

weapon_t BG_GrenadeTypeForTeam( team_t team ) {
	switch( team ) {
		case TEAM_AXIS:
			return WP_GRENADE_LAUNCHER;
		case TEAM_ALLIES:
			return WP_GRENADE_PINEAPPLE;
		default:
			return WP_NONE;
	}
}


// Gordon: setting numOfClips = 0 allows you to check if the client needs ammo, but doesnt give any
qboolean BG_AddMagicAmmo( playerState_t *ps, int *skillBits, int teamNum, int numOfClips )
{
	int			weapon;
	int			canAdd;
	int			maxammo;
	int			clip;
	int			i;
	qboolean	result = qfalse;

	// Michael:  add the helmet first, then do weapons
	if(ps->eFlags & EF_HEADSHOT) {
		ps->eFlags &= ~EF_HEADSHOT;
		result = qtrue;
	}

	// weapons that only use clip (not ammo)
	// and need COM_BitSet

	// grenades..
	weapon = ( teamNum == TEAM_AXIS )? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE;
	clip = WeaponTable[weapon].clipindex;
	maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
	canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
	if ( canAdd ) {
		if ( !numOfClips ) return qtrue;
		COM_BitSet(ps->weapons, weapon);
		ps->ammoclip[clip] += numOfClips;
		if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
		result = qtrue;
	}

	// knives..
	weapon = ( teamNum == TEAM_AXIS )? WP_KNIFE : WP_KNIFE_KABAR;
	clip = WeaponTable[weapon].clipindex;
	maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
	canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
	if ( canAdd ) {
		if ( !numOfClips ) return qtrue;
		COM_BitSet(ps->weapons, weapon);
		ps->ammoclip[clip] += numOfClips;
		if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
		result = qtrue;
	}

	// weapons that only use clip (not ammo)

	// syringes..
	weapon = WP_MEDIC_SYRINGE;
	if ( COM_BitCheck( ps->weapons, weapon ) ) {
		clip = WeaponTable[weapon].clipindex;
		maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
		canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
		if ( canAdd ) {
			if ( !numOfClips ) return qtrue;
			ps->ammoclip[clip] += numOfClips;
			if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
			result = qtrue;
		}
	}

	weapon = WP_POISON_SYRINGE;
	if ( COM_BitCheck( ps->weapons, weapon ) ) {
		clip = WeaponTable[weapon].clipindex;
		maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
		canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
		if ( canAdd ) {
			if ( !numOfClips ) return qtrue;
			ps->ammoclip[clip] += numOfClips;
			if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
			result = qtrue;
		}
	}

	weapon = WP_MEDIC_ADRENALINE;
	if ( !(BG_ADRENALINE & ADF_NOPACK) && COM_BitCheck( ps->weapons, weapon ) ) {
		clip = WeaponTable[weapon].clipindex;
		maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
		if ( BG_ADRENALINE & ADF_JUSTONE ) maxammo = 1;
		canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
		if ( canAdd ) {
			if ( !numOfClips ) return qtrue;
			ps->ammoclip[clip] += numOfClips;
			if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
			result = qtrue;
		}
	}

	// reloadable weapons..

	for ( i=0; reloadableWeapons[i] >= 0; i++) {
		weapon = reloadableWeapons[i];
		if ( !COM_BitCheck( ps->weapons, weapon ) ) continue;

		switch (weapon) {
			// filling clips..
			case WP_FLAMETHROWER:
				clip = WeaponTable[weapon].clipindex;
				maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
				canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
				if ( canAdd ) {
					if ( !numOfClips ) return qtrue;
					ps->ammoclip[clip] = maxammo;	// fills up in 1 pass..
					result = qtrue;
				}
				break;

			case WP_PANZERFAUST:
			case WP_BAZOOKA:
				clip = WeaponTable[weapon].clipindex;
				maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
				canAdd = ( ps->ammoclip[clip] < maxammo )? qtrue : qfalse;
				if ( canAdd ) {
					if ( !numOfClips ) return qtrue;
					ps->ammoclip[clip] += numOfClips;
					if ( ps->ammoclip[clip] > maxammo ) ps->ammoclip[clip] = maxammo;
					result = qtrue;
				}
				break;

			// filling ammo..
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_SILENCEDLUGER:
			case WP_AKIMBO_SILENCEDCOLT:
				clip = WeaponTable[weapon].ammoindex;
				maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
				canAdd = ( ps->ammo[clip] < maxammo )? qtrue : qfalse;
				if ( canAdd ) {
					if ( !numOfClips ) return qtrue;
					ps->ammo[clip] += numOfClips * 2 * WeaponTable[clip].maxclip;
					if ( ps->ammo[clip] > maxammo ) ps->ammo[clip] = maxammo;
					result = qtrue;
				}
				break;

			default:
				clip = WeaponTable[weapon].ammoindex;
				maxammo = BG_MaxAmmoForWeapon( weapon, skillBits, ps->stats[STAT_PLAYER_CLASS] );
				canAdd = ( ps->ammo[clip] < maxammo )? qtrue : qfalse;
				if ( canAdd ) {
					if ( !numOfClips ) return qtrue;
					ps->ammo[clip] += numOfClips * WeaponTable[clip].maxclip;
					if ( ps->ammo[clip] > maxammo ) ps->ammo[clip] = maxammo;
					result = qtrue;
				}
				break;
		}
	}

	return result;
}

/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, int *skillBits, int teamNum )
{
	gitem_t	*item;

	if ( ent->modelindex < 1 || ent->modelindex >= bg_numItems ) {
		Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
		return qfalse;
	}

	item = &bg_itemlist[ent->modelindex];

	switch( item->giType ) {
		case IT_WEAPON:
			if( item->giTag == WP_AMMO ) {
				// magic ammo for any two-handed weapon
				// xkan, 11/21/2002 - only pick up if ammo is not full, numClips is 0, so ps will
				// NOT be changed (I know, it places the burden on the programmer, rather than the
				// compiler, to ensure that).
				return BG_AddMagicAmmo( (playerState_t *)ps, skillBits, teamNum, 0);	// Arnout: had to cast const away
			}
			return qtrue;

		case IT_AMMO:
			if( item->giTag == WP_AMMO ) {
				return BG_AddMagicAmmo( (playerState_t *)ps, skillBits, teamNum, 0);	// Arnout: had to cast const away
			}
			// jaquboss don't grab ammo if we have already enough
			if ( ps->ammo[item->giTag] < BG_MaxAmmoForWeapon( item->giTag, skillBits, ps->stats[STAT_PLAYER_CLASS] ) ){
				int			i;

				// jaquboss only eat ammo we can use
				for ( i=WP_KNIFE; i < WP_NUM_WEAPONS; i++ ) {
					if (!COM_BitCheck( ps->weapons, i )) // we don't have this weapon
						continue;

					if ( WeaponTable[i].ammoindex == item->giTag ){ // this weapon uses that ammo
					    return qtrue;
					    break;
					}
				}
			}
			return qfalse;

		case IT_ARMOR:		// FIXME no armor in game
			return qfalse;

		case IT_HEALTH:
			// Gordon: ps->teamNum is really class.... thx whoever decided on that...
			if ( (ps->powerups[PW_ADRENALINE]>0) && (BG_ADRENALINE & ADF_NOHPICK) )
				return qfalse;

			if( ps->teamNum == PC_MEDIC ) {
				// Gordon: medics can go up to 12% extra on max health as they have perm. regen
				float extraHP = (BG_MEDICS & MEDIC_EXTRAHP)? 1.12 : 1.00;
				if( ps->stats[STAT_HEALTH] >= (int)(ps->stats[STAT_MAX_HEALTH] * extraHP) ) {
					return qfalse;
				}
			}
			else {
				if( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] ) {
					return qfalse;
				}
			}
			return qtrue;

		case IT_POWERUP:
			if ( item->giTag == PW_OPS_DISGUISED ) {
				if ( item->quantity == ps->teamNum && ps->eFlags & EF_PANTSED && item->giAmmoIndex == ps->persistant[PERS_TEAM] )
						return qtrue;

				if ( item->giAmmoIndex == ps->persistant[PERS_TEAM] ||  ps->teamNum != PC_COVERTOPS || ps->powerups[PW_OPS_DISGUISED] )
						return qfalse;

				return qtrue;
			}

			if ( item->giTag == PW_HELMET && !(ps->eFlags & EF_HEADSHOT)) {
				return qfalse;
			}

			if ( item->giTag == PW_FLAKJACKET && ps->powerups[PW_FLAKJACKET]) {
				return qfalse;
			}

			if ( item->giTag == PW_HELMETSHIELD && ps->powerups[PW_HELMETSHIELD] ) {
				return qfalse;
			}

			if ( ps->powerups[item->giTag] == item->quantity ){
				return qfalse;
			}

			return qtrue;

		case IT_TEAM: // team items, such as flags

			switch (ps->persistant[PERS_TEAM]) {
				case TEAM_AXIS:
					// If the item has no uses left (density) or if the player is already carrying a flag
					// then he can't pick up another
					if (ent->density < 1 || ps->powerups[PW_BLUEFLAG]) {
						return qfalse;
					}
					// can always grab the enemy flag
					if (item->giTag == PW_BLUEFLAG) {
						return qtrue;
					}
					// otherEntityNum2 will be set if the flag is dropped (and returnable)
					if (item->giTag == PW_REDFLAG && ent->otherEntityNum2) {
						return qtrue;
					}

					break;

				case TEAM_ALLIES:
					// If the item has no uses left (density) or if the player is already carrying a flag
					// then he can't pick up another
					if (ent->density < 1 || ps->powerups[PW_REDFLAG]) {
						return qfalse;
					}
					// can always grab the enemy flag
					if (item->giTag == PW_REDFLAG) {
						return qtrue;
					}
					// otherEntityNum2 will be set if the flag is dropped (and returnable)
					if (item->giTag == PW_BLUEFLAG && ent->otherEntityNum2) {
						return qtrue;
					}

				default:
					// Specs cannot pick up anything
					return qfalse;
			}
			return qfalse;
		case IT_KEY:		// keys are always picked up
			return qtrue;

		case IT_BAD:
			Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );

	}
	return qfalse;
}

//======================================================================

void BG_CalculateSpline_r(splinePath_t* spline, vec3_t out1, vec3_t out2, float tension) {
	vec3_t points[18];
	int i;
	int count = spline->numControls + 2;
	vec3_t dist;

	VectorCopy( spline->point.origin, points[0] );
	for( i = 0; i < spline->numControls; i++ ) {
		VectorCopy( spline->controls[i].origin, points[i+1] );
	}
	if(!spline->next) {
		return;
	}
	VectorCopy( spline->next->point.origin, points[i+1] );


	while(count > 2) {
		for( i = 0; i < count-1; i++ ) {
			VectorSubtract( points[i+1], points[i], dist );
			VectorMA(points[i], tension, dist, points[i]);
		}
		count--;
	}

	VectorCopy( points[0], out1 );
	VectorCopy( points[1], out2 );
}

qboolean BG_TraverseSpline( float* deltaTime, splinePath_t** pSpline) {
	float dist;

	while( (*deltaTime) > 1 ) {
		(*deltaTime) -= 1;
		dist = (*pSpline)->length * (*deltaTime);

		if(!(*pSpline)->next || !(*pSpline)->next->length) {
			return qfalse;
		}

		(*pSpline) = (*pSpline)->next;
		*deltaTime = dist / (*pSpline)->length;
	}

	while( (*deltaTime) < 0 ) {
		dist = -((*pSpline)->length * (*deltaTime));

		if(!(*pSpline)->prev || !(*pSpline)->prev->length) {
			return qfalse;
		}

		(*pSpline) = (*pSpline)->prev;
		(*deltaTime) = 1 - (dist / (*pSpline)->length);
	}

	return qtrue;
}

/*
================
BG_RaySphereIntersection

================
*/
qboolean BG_RaySphereIntersection( float radius, vec3_t origin, splineSegment_t* path, float *t0, float *t1 ) {
	vec3_t v;
	float b, c, d;

	VectorSubtract( path->start, origin, v );

	b = 2 * DotProduct( v, path->v_norm );
	c = DotProduct( v, v ) - (radius * radius);

	d = (b * b) - (4 * c);
	if( d < 0 ) {
		return qfalse;
	}
	d = sqrt( d );

	*t0 = (-b + d) * 0.5f;
	*t1 = (-b - d) * 0.5f;

	return qtrue;
}

void BG_LinearPathOrigin2(float radius, splinePath_t** pSpline, float *deltaTime, vec3_t result, qboolean backwards) {
	qboolean	first = qtrue;
	float		t = 0.f;
	int			i = floor((*deltaTime) * (MAX_SPLINE_SEGMENTS));
	float		frac;
	float		s;
	// Hackfix against endless-loop execution.
	// It could happen if a script_mover drives (backwards) over a spline-path
	// where the distance between splines is (too) short.
	int			loopCount = 0;

	if(i >= MAX_SPLINE_SEGMENTS) {
		i = MAX_SPLINE_SEGMENTS - 1;
		frac = 1.f;
	}
	else {
		frac = (((*deltaTime) * (MAX_SPLINE_SEGMENTS)) - i);
	}

	while(qtrue) {
		float t0, t1;

		while(qtrue) {
			if(BG_RaySphereIntersection( radius, result, &(*pSpline)->segments[i], &t0, &t1 )) {
				qboolean found = qfalse;

				t0 /= (*pSpline)->segments[i].length;
				t1 /= (*pSpline)->segments[i].length;

				if(first) {
					if(radius < 0) {
						if(t0 < frac && (t0 >= 0.f && t0 <= 1.f)) {
							t = t0;
							found = qtrue;
						}
						else if (t1 < frac) {
							t = t1;
							found = qtrue;
						}
					}
					else {
						if(t0 > frac && (t0 >= 0.f && t0 <= 1.f)) {
							t = t0;
							found = qtrue;
						}
						else if (t1 > frac) {
							t = t1;
							found = qtrue;
						}
					}
				}
				else {
					if(radius < 0) {
						if(t0 < t1 && (t0 >= 0.f && t0 <= 1.f)) {
							t = t0;
							found = qtrue;
						}
						else {
							t = t1;
							found = qtrue;
						}
					}
					else {
						if(t0 > t1 && (t0 >= 0.f && t0 <= 1.f)) {
							t = t0;
							found = qtrue;
						}
						else {
							t = t1;
							found = qtrue;
						}
					}
				}

				if( found ) {
					if(t >= 0.f && t <= 1.f) {
						*deltaTime = (i / (float)(MAX_SPLINE_SEGMENTS)) + (t / (float)(MAX_SPLINE_SEGMENTS));
						s = t * (*pSpline)->segments[i].length;
						VectorMA( (*pSpline)->segments[i].start, s, (*pSpline)->segments[i].v_norm, result );
						return;
					}
				}
				found = qfalse;

				// check number of executed loops.
				// Note: if MAX_SPLINE_SEGMENTS == 1 (or maybe any other value <16)
				// this error will not occur.. (any side-effects are not investigated)
				if ( ++loopCount > 256 ) {
#ifdef CGAMEDLL
					Com_Error( ERR_DISCONNECT, "BG_LinearPathOrigin2: Splines too close apart.  Near: \"%s\" (%i,%i,%i)", (*pSpline)->point.name, (int)(*pSpline)->point.origin[0], (int)(*pSpline)->point.origin[1], (int)(*pSpline)->point.origin[2] );
#endif
#ifdef GAMEDLL
					Com_Error( ERR_FATAL, "BG_LinearPathOrigin2: Splines too close apart.  Near: \"%s\" (%i,%i,%i)", (*pSpline)->point.name, (int)(*pSpline)->point.origin[0], (int)(*pSpline)->point.origin[1], (int)(*pSpline)->point.origin[2] );
#endif
				}
			}

			first = qfalse;
			if(radius < 0) {
				i--;
				if(i < 0) {
					i = MAX_SPLINE_SEGMENTS - 1;
					break;
				}
			} else {
				i++;
				if(i >= MAX_SPLINE_SEGMENTS) {
					i = 0;
					break;
				}
			}
		}

		if( radius < 0 ) {
			if(!(*pSpline)->prev) {
				return;
			}
			*pSpline = (*pSpline)->prev;
		}
		else {
			if(!(*pSpline)->next) {
				return;
			}
			*pSpline = (*pSpline)->next;
		}
	}
}

void BG_ComputeSegments(splinePath_t* pSpline) {
	int i;
	float granularity = 1 / ((float)(MAX_SPLINE_SEGMENTS));
	vec3_t	vec[4];
	float s;

	for( i = 0; i < MAX_SPLINE_SEGMENTS; i++ ) {
		BG_CalculateSpline_r( pSpline, vec[0], vec[1], i * granularity );
		VectorSubtract( vec[1], vec[0], pSpline->segments[i].start );
		s = i * granularity;
		VectorMA(vec[0], s, pSpline->segments[i].start, pSpline->segments[i].start );

		BG_CalculateSpline_r( pSpline, vec[2], vec[3], (i + 1) * granularity );
		VectorSubtract( vec[3], vec[2], vec[0] );
		s = (i + 1) * granularity;
		VectorMA(vec[2], s, vec[0], vec[0] );

		VectorSubtract( vec[0], pSpline->segments[i].start, pSpline->segments[i].v_norm );
		pSpline->segments[i].length = VectorLength( pSpline->segments[i].v_norm );
		VectorNormalize( pSpline->segments[i].v_norm );
	}
}

/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splinePath ) {
	float		deltaTime;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
	case TR_GRAVITY_PAUSED:	//----(SA)
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		{
			float phase;
			deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
			phase = sin( deltaTime * M_PI * 2 );
			VectorMA( tr->trBase, phase, tr->trDelta, result );
		}
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	case TR_GRAVITY_LOW:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * (DEFAULT_GRAVITY * 0.3) * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	case TR_GRAVITY_FLOAT:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * (DEFAULT_GRAVITY * 0.2) * deltaTime;
		break;
	// RF, acceleration
	case TR_ACCELERATE:		// trDelta is the ultimate speed
		{
			float		phase;
			float		s;
			if ( atTime > tr->trTime + tr->trDuration ) {
				atTime = tr->trTime + tr->trDuration;
			}
			deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
			// phase is the acceleration constant
			phase = VectorLength( tr->trDelta ) / (tr->trDuration*0.001);
			// trDelta at least gives us the acceleration direction
			VectorNormalize2( tr->trDelta, result );
			// get distance travelled at current time
			s = phase * 0.5 * deltaTime * deltaTime;
			VectorMA( tr->trBase, s, result, result );
		}
		break;
	case TR_DECCELERATE:	// trDelta is the starting speed
		{
			vec3_t		v;
			float		phase;
			float		s;
			if ( atTime > tr->trTime + tr->trDuration ) {
				atTime = tr->trTime + tr->trDuration;
			}
			deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
			// phase is the breaking constant
			phase = VectorLength( tr->trDelta ) / (tr->trDuration*0.001);
			// trDelta at least gives us the acceleration direction
			VectorNormalize2( tr->trDelta, result );
			// get distance travelled at current time (without breaking)
			VectorMA( tr->trBase, deltaTime, tr->trDelta, v );
			// subtract breaking force
			s = -phase * 0.5 * deltaTime * deltaTime;
			VectorMA( v, s, result, result );
		}
		break;
	case TR_SPLINE:
		{
			qboolean backwards = qfalse;
			splinePath_t* pSpline;
			vec3_t vec[2];
			float deltaTime2;

			if(!(pSpline = BG_GetSplineData( splinePath, &backwards ))) {
				return;
			}

			deltaTime = tr->trDuration ? (atTime - tr->trTime) / ((float)tr->trDuration) : 0;

			if(deltaTime < 0.f) {
				deltaTime = 0.f;
			}
			else if(deltaTime > 1.f) {
				deltaTime = 1.f;
			}

			if(backwards) {
				deltaTime = 1 - deltaTime;
			}

			deltaTime2 = deltaTime;

			BG_CalculateSpline_r( pSpline, vec[0], vec[1], deltaTime );

			if(isAngle) {
				qboolean dampin = qfalse;
				qboolean dampout = qfalse;
				float base1;

				if(tr->trBase[0]) {
					splinePath_t* pSp2 = pSpline;

					deltaTime2 += tr->trBase[0] / pSpline->length;

					if(BG_TraverseSpline( &deltaTime2, &pSp2 )) {
						vec3_t result2;

						VectorSubtract( vec[1], vec[0], result );
						VectorMA(vec[0], deltaTime, result, result);

						BG_CalculateSpline_r( pSp2, vec[0], vec[1], deltaTime2 );

						VectorSubtract( vec[1], vec[0], result2 );
						VectorMA(vec[0], deltaTime2, result2, result2);

						if(	tr->trBase[0] < 0 ) {
							VectorSubtract( result, result2, result );
						}
						else {
							VectorSubtract( result2, result, result );
						}
					}
					else {
						VectorSubtract( vec[1], vec[0], result );
					}
				}
				else {
					VectorSubtract( vec[1], vec[0], result );
				}

				vectoangles( result, result );

				base1 = tr->trBase[1];
				if(base1 >= 10000 || base1 < -10000) {
					dampin = qtrue;
					if(base1 < 0) {
						base1 += 10000;
					}
					else {
						base1 -= 10000;
					}
				}

				if(base1 >= 1000 || base1 < -1000) {
					dampout = qtrue;
					if(base1 < 0) {
						base1 += 1000;
					}
					else {
						base1 -= 1000;
					}
				}

				if(dampin && dampout) {
					result[ROLL] = base1 + ((sin(((deltaTime * 2) - 1) * M_PI * 0.5f) + 1) * 0.5f * tr->trBase[2]);
				}
				else if(dampin) {
					result[ROLL] = base1 + (sin(deltaTime * M_PI * 0.5f) * tr->trBase[2]);
				}
				else if(dampout) {
					result[ROLL] = base1 + ((1 - sin((1 - deltaTime) * M_PI * 0.5f)) * tr->trBase[2]);
				}
				else {
					result[ROLL] = base1 + (deltaTime * tr->trBase[2]);
				}
			}
			else {
				VectorSubtract( vec[1], vec[0], result );
				VectorMA(vec[0], deltaTime, result, result);
			}
		}
		break;
	case TR_LINEAR_PATH:
		{
			qboolean backwards = qfalse;
			splinePath_t* pSpline;

			if(!(pSpline = BG_GetSplineData( splinePath, &backwards ))) {
				return;
			}

			deltaTime = tr->trDuration ? (atTime - tr->trTime) / ((float)tr->trDuration) : 0;

			if(deltaTime < 0.f) {
				deltaTime = 0.f;
			}
			else if(deltaTime > 1.f) {
				deltaTime = 1.f;
			}

			if(backwards) {
				deltaTime = 1 - deltaTime;
			}

			if(isAngle) {
				int pos = floor(deltaTime * (MAX_SPLINE_SEGMENTS));
				float frac;

				if(pos >= MAX_SPLINE_SEGMENTS) {
					pos = MAX_SPLINE_SEGMENTS - 1;
					frac = pSpline->segments[pos].length;
				}
				else {
					frac = ((deltaTime * (MAX_SPLINE_SEGMENTS)) - pos) * pSpline->segments[pos].length;
				}

				if(tr->trBase[0]) {
					vec3_t	v;

					VectorMA( pSpline->segments[pos].start, frac, pSpline->segments[pos].v_norm, result );
					VectorCopy( result, v );

					BG_LinearPathOrigin2( tr->trBase[0], &pSpline, &deltaTime, v, backwards );
					if(	tr->trBase[0] < 0 ) {
						VectorSubtract( v, result, result );
					}
					else {
						VectorSubtract( result, v, result );
					}

					vectoangles( result, result );
				}
				else {
					vectoangles( pSpline->segments[pos].v_norm, result );
				}

			}
			else {
				int pos = floor(deltaTime * (MAX_SPLINE_SEGMENTS));
				float frac;

				if(pos >= MAX_SPLINE_SEGMENTS) {
					pos = MAX_SPLINE_SEGMENTS - 1;
					frac = pSpline->segments[pos].length;
				}
				else {
					frac = ((deltaTime * (MAX_SPLINE_SEGMENTS)) - pos) * pSpline->segments[pos].length;
				}


				VectorMA( pSpline->segments[pos].start, frac, pSpline->segments[pos].v_norm, result );
			}
		}
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
		break;
	}
}


/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splineData ) {
	float	deltaTime;
	float	phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	case TR_GRAVITY_LOW:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= (DEFAULT_GRAVITY * 0.3) * deltaTime;		// FIXME: local gravity...
		break;
	case TR_GRAVITY_FLOAT:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= (DEFAULT_GRAVITY * 0.2) * deltaTime;
		break;
	// RF, acceleration
	case TR_ACCELERATE:	// trDelta is eventual speed
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		phase = deltaTime / (float)tr->trDuration;
		VectorScale( tr->trDelta, deltaTime * deltaTime, result );
		break;
	case TR_DECCELERATE:	// trDelta is breaking force
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorScale( tr->trDelta, deltaTime, result );
		break;
	case TR_SPLINE:
	case TR_LINEAR_PATH:
		VectorClear( result );
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
		break;
	}
}

/*
============
BG_GetMarkDir

  used to find a good directional vector for a mark projection, which will be more likely
  to wrap around adjacent surfaces

  dir is the direction of the projectile or trace that has resulted in a surface being hit
============
*/
void BG_GetMarkDir( const vec3_t dir, const vec3_t normal, vec3_t out ) {
	vec3_t	ndir, lnormal;
	float	minDot = 0.3;
	int x = 0;

	if( dir[0] < 0.001 && dir[1] < 0.001 ) {
		VectorCopy( dir, out );
		return;
	}

	if( VectorLengthSquared( normal ) < SQR(1.f) ) {	// this is needed to get rid of (0,0,0) normals (happens with entities?)
		VectorSet( lnormal, 0.f, 0.f, 1.f );
	}
	else {
		VectorNormalize2( normal, lnormal );
	}

	VectorNegate( dir, ndir );
	VectorNormalize( ndir );
	if( normal[2] > .8f ) {
		minDot = .7f;
	}

	// make sure it makrs the impact surface
	while( DotProduct( ndir, lnormal ) < minDot && x < 10 ) {
		VectorMA( ndir, 0.5f, lnormal, ndir );
		VectorNormalize( ndir );

		x++;
	}

#ifdef GAMEDLL
	if( x >= 10 ) {
		if( g_developer.integer ) {
			Com_Printf( "BG_GetMarkDir loops: %i\n", x );
		}
	}
#endif // GAMEDLL

	VectorCopy( ndir, out );
}

#ifdef _DEBUG
char *eventnames[] = {
	"EV_NONE",
	"EV_FOOTSTEP",
	"EV_FOOTSTEP_METAL",
	"EV_FOOTSTEP_WOOD",
	"EV_FOOTSTEP_GRASS",
	"EV_FOOTSTEP_GRAVEL",
	"EV_FOOTSTEP_ROOF",
	"EV_FOOTSTEP_SNOW",
	"EV_FOOTSTEP_CARPET",
	"EV_FOOTSPLASH",
	"EV_FOOTWADE",
	"EV_SWIM",
	"EV_STEP_4",
	"EV_STEP_8",
	"EV_STEP_12",
	"EV_STEP_16",
	"EV_FALL_SHORT",
	"EV_FALL_MEDIUM",
	"EV_FALL_FAR",
	"EV_FALL_NDIE",
	"EV_FALL_DMG_10",
	"EV_FALL_DMG_15",
	"EV_FALL_DMG_25",
	"EV_FALL_DMG_50",
	"EV_WATER_TOUCH",
	"EV_WATER_LEAVE",
	"EV_WATER_UNDER",
	"EV_WATER_CLEAR",
	"EV_ITEM_PICKUP",
	"EV_ITEM_PICKUP_QUIET",
	"EV_GLOBAL_ITEM_PICKUP",
	"EV_NOAMMO",
	"EV_WEAPONSWITCHED",
	"EV_FILL_CLIP",
	"EV_WEAP_OVERHEAT",
	"EV_CHANGE_WEAPON",
	"EV_CHANGE_WEAPON_2",
	"EV_FIRE_WEAPON",
	"EV_FIRE_WEAPONB",
	"EV_FIRE_WEAPON_LASTSHOT",
	"EV_NOFIRE_UNDERWATER",
	"EV_FIRE_WEAPON_MG42",
	"EV_FIRE_WEAPON_MOUNTEDMG42",
	"EV_ITEM_RESPAWN",
	"EV_ITEM_POP",
	"EV_GRENADE_BOUNCE",
	"EV_GENERAL_SOUND",
	"EV_GENERAL_SOUND_VOLUME",
	"EV_GLOBAL_SOUND",
	"EV_GLOBAL_CLIENT_SOUND",
	"EV_GLOBAL_TEAM_SOUND",
	"EV_FX_SOUND",
	"EV_BULLET_HIT_FLESH",
	"EV_BULLET_HIT_WALL",
	"EV_MISSILE_HIT",
	"EV_MISSILE_MISS",
	"EV_RAILTRAIL",
	"EV_BULLET",
	"EV_LOSE_HAT",
	"EV_PAIN",
	"EV_CROUCH_PAIN",
	"EV_DEATH1",
	"EV_DEATH2",
	"EV_DEATH3",
	"EV_DEATH4",
	"EV_OBITUARY",
	"EV_STOPSTREAMINGSOUND",
	"EV_POWERUP_QUAD",
	"EV_POWERUP_BATTLESUIT",
	"EV_POWERUP_REGEN",
	"EV_GIB_PLAYER",
	"EV_DEBUG_LINE",
	"EV_STOPLOOPINGSOUND",
	"EV_SMOKE",
	"EV_SPARKS",
	"EV_SPARKS_ELECTRIC",
	"EV_EXPLODE",
	"EV_RUBBLE",
	"EV_EFFECT",
	"EV_MORTAREFX",
	"EV_SPINUP",
	"EV_SNOW_ON",
	"EV_SNOW_OFF",
	"EV_MISSILE_MISS_SMALL",
	"EV_MISSILE_MISS_LARGE",
	"EV_MORTAR_IMPACT",
	"EV_MORTAR_MISS",
	"EV_SHARD",
	"EV_JUNK",
	"EV_EMITTER",
	"EV_OILPARTICLES",
	"EV_OILSLICK",
	"EV_OILSLICKREMOVE",
	"EV_MG42EFX",
	"EV_SNOWFLURRY",
	"EV_DUST",
	"EV_RUMBLE_EFX",
	"EV_GUNSPARKS",
	"EV_FLAMETHROWER_EFFECT",
	"EV_MG42BULLET_HIT_FLESH",
	"EV_MG42BULLET_HIT_WALL",
	"EV_SHAKE",
	"EV_DISGUISE_SOUND",
	"EV_BUILDDECAYED_SOUND", // 110
	"EV_DEBRIS",
	"EV_ALERT_SPEAKER",
	"EV_POPUPMESSAGE",
	"EV_ARTYMESSAGE",
	"EV_AIRSTRIKEMESSAGE",
	"EV_MEDIC_CALL",
	"EV_SHOTGUN_PUMP",				// jaquboss
	"EV_SPAWN",
	"EV_BODY_DP",
	"EV_BOUNCE_SOUND",
	"EV_KICK",
	"EV_WOLFKICK_MISS",
	"EV_WOLFKICK_HIT_FLESH",
	"EV_WOLFKICK_HIT_WALL",
	"EV_THROWKNIFE",
	"EV_SHOVE_SOUND",
	"EV_WEAPALT",
	"EV_PRIVATE_MESSAGE",
	"EV_SHOTGUN_FIRED",
	"EV_ARTY_DETECTED",
	"EV_MISSIONMESSAGE",			// core: mission related messages..
	"EV_MAX_EVENTS",
};
#endif

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {

#ifdef _DEBUG
	{
		char buf[256];
		trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));
		if ( atof(buf) != 0 ) {
#ifdef QAGAME
			Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
			Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
		}
	}
#endif
	ps->events[ps->eventSequence & (MAX_EVENTS-1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_EVENTS-1)] = eventParm;
	ps->eventSequence++;
}

// Gordon: would like to just inline this but would likely break qvm support
#define SETUP_MOUNTEDGUN_STATUS( ps )							\
	switch( ps->persistant[PERS_HWEAPON_USE] ) {				\
		case 1:													\
			ps->eFlags |= EF_MG42_ACTIVE;						\
			ps->powerups[PW_OPS_DISGUISED] = 0;					\
			break;												\
		default:												\
			ps->eFlags &= ~EF_MG42_ACTIVE;						\
			break;												\
	}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/


weapon_t BG_GetPrimaryWeaponForPlayerState( playerState_t *ps )
{
	weapon_t			primary		= WP_NONE;

	if ( COM_BitCheck( ps->weapons, WP_BAZOOKA ) )	{
		primary = WP_BAZOOKA;
	} else if ( COM_BitCheck( ps->weapons, WP_PANZERFAUST ) )	{
		primary = WP_PANZERFAUST;
	} else if ( COM_BitCheck( ps->weapons, WP_MOBILE_BROWNING ) )	{
		primary = WP_MOBILE_BROWNING;
	} else if ( COM_BitCheck( ps->weapons, WP_MOBILE_MG42 ) )	{
		primary = WP_MOBILE_MG42;
	} else if ( COM_BitCheck( ps->weapons, WP_MORTAR ) )	{
		primary = WP_MORTAR;
	} else if ( COM_BitCheck( ps->weapons, WP_MORTAR2 ) )	{
		primary = WP_MORTAR2;
	} else if ( COM_BitCheck( ps->weapons, WP_FLAMETHROWER ) )	{
		primary = WP_FLAMETHROWER;
	} else if ( COM_BitCheck( ps->weapons, WP_SHOTGUN ) )	{
		primary = WP_SHOTGUN;
	} else if ( COM_BitCheck( ps->weapons, WP_K43 ) )	{
		primary = WP_K43;
	} else if ( COM_BitCheck( ps->weapons, WP_GARAND ) )	{
		primary = WP_GARAND;
	} else if ( COM_BitCheck( ps->weapons, WP_CARBINE ) )	{
		primary = WP_CARBINE;
	} else if ( COM_BitCheck( ps->weapons, WP_KAR98 ) )	{
		primary = WP_KAR98;
	} else if ( COM_BitCheck( ps->weapons, WP_STG44 ) )	{
		primary = WP_STG44;
	} else if ( COM_BitCheck( ps->weapons, WP_BAR ) )	{
		primary = WP_BAR;
	} else if ( COM_BitCheck( ps->weapons, WP_FG42 ) )	{
		primary = WP_FG42;
	} else if ( COM_BitCheck( ps->weapons, WP_MP34 ) )	{
		primary = WP_MP34;
	} else if ( COM_BitCheck( ps->weapons, WP_STEN ) )	{
		primary = WP_STEN;
	} else if ( COM_BitCheck( ps->weapons, WP_STEN_MKII ) )	{
		primary = WP_STEN_MKII;
	} else if ( COM_BitCheck( ps->weapons, WP_MP40 ) )	{
		primary = WP_MP40;
	} else if ( COM_BitCheck( ps->weapons, WP_THOMPSON ) )	{
		primary = WP_THOMPSON;
	} else if ( COM_BitCheck( ps->weapons, WP_VENOM ) )	{
		primary = WP_VENOM;
	}
	return primary;
}

void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
	int		i;

	if(ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR  ) { // JPW NERVE limbo

		s->eType = ET_INVISIBLE;
	}
	else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
		s->eType = ET_INVISIBLE;
	}
	else {
		s->eType = ET_PLAYER;
	}

	s->number		= ps->clientNum;
	s->pos.trType	= TR_INTERPOLATE;
	s->pos.trTime	= time;				// zinx - help out new synced animations.

	VectorCopy( ps->origin, s->pos.trBase );

	if ( snap ) {
		SnapVector( s->pos.trBase );
	}

	// josh: This wasn't in here?! Why!? It's in Quake 3
	VectorCopy( ps->velocity, s->pos.trDelta );

	if ( snap ) {
		SnapVector( s->pos.trDelta );
	}

	s->apos.trType	= TR_INTERPOLATE;

	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	if (ps->movementDir > 128)
		s->angles2[YAW] = (float)ps->movementDir - 256;
	else
		s->angles2[YAW] = ps->movementDir;

	// head rotation - playdead
	if ( ps->eFlags & EF_PLAYDEAD )
		s->angles2[PITCH] = SHORT2ANGLE(ps->stats[STAT_DEAD_YAW]);
	else
		s->angles2[PITCH] = 0;

	s->legsAnim		= ps->legsAnim;
	s->torsoAnim	= ps->torsoAnim;
	s->clientNum	= ps->clientNum;	// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	// Ridah, let clients know if this person is using a mounted weapon
	// so they don't show any client muzzle flashes

	if( ps->eFlags & EF_MOUNTEDTANK ) {
		ps->eFlags &= ~EF_MG42_ACTIVE;
	}
	else {
		SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;

	if(ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	}
	else {
		s->eFlags &= ~EF_DEAD;
	}

// from MP
	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	}
	else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}
// end
	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for (i = ps->oldEventSequence; i != ps->eventSequence; i++) {
		s->events[s->eventSequence & (MAX_EVENTS-1)] = ps->events[i & (MAX_EVENTS-1)];
		s->eventParms[s->eventSequence & (MAX_EVENTS-1)] = ps->eventParms[i & (MAX_EVENTS-1)];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;

	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->nextWeapon = ps->nextWeapon;	// Ridah
	s->teamNum = ps->teamNum;
	s->aiState = ps->aiState;		// xkan, 1/10/2003

	if ( ps->pm_type != PM_SPECTATOR ){
        s->constantLight = ps->stats[STAT_PS_FLAGS];
		s->dl_intensity = ps->stats[STAT_HEALTH]; // so we can draw pain shaders on client
	}
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, qboolean snap )
{
	int		i;

	if(ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR ) {
		s->eType = ET_INVISIBLE;
	}
	else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
		s->eType = ET_INVISIBLE;
	}
	else {
		s->eType = ET_PLAYER;
	}

	s->number		= ps->clientNum;
	s->pos.trType	= TR_LINEAR_STOP;

	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}

	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = (int)time;
	// set maximum extrapolation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType	= TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim		= ps->legsAnim;
	s->torsoAnim	= ps->torsoAnim;
	s->clientNum	= ps->clientNum;		// ET_PLAYER looks here instead of at number
											// so corpses can also reference the proper config

	if( ps->eFlags & EF_MOUNTEDTANK ) {
		ps->eFlags &= ~EF_MG42_ACTIVE;
	}
	else {
		SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;

	if(ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	}
	else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	}
	else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for (i = ps->oldEventSequence; i != ps->eventSequence; i++) {
		s->events[s->eventSequence & (MAX_EVENTS-1)] = ps->events[i & (MAX_EVENTS-1)];
		s->eventParms[s->eventSequence & (MAX_EVENTS-1)] = ps->eventParms[i & (MAX_EVENTS-1)];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->nextWeapon = ps->nextWeapon;	// Ridah
	s->teamNum = ps->teamNum;
	s->aiState = ps->aiState;		// xkan, 1/10/2003
}

// Gordon: some weapons are duplicated for code puposes.... just want to treat them as a single
weapon_t BG_DuplicateWeapon( weapon_t weap ) {
	switch( weap ) {
		case WP_M7:					return WP_GPG40;
		case WP_GARAND_SCOPE:		return WP_GARAND;
		case WP_K43_SCOPE:			return WP_K43;
		case WP_GRENADE_PINEAPPLE:	return WP_GRENADE_LAUNCHER;
		case WP_MORTAR2_SET:		return WP_MORTAR_SET; // jaquboss
		default:					return weap;
	}
}

gitem_t* BG_ValidStatWeapon( weapon_t weap ) {
	weapon_t weap2;

	if (!IS_VALID_WEAPON(weap)) return NULL;

	switch(weap) {
		case WP_MEDKIT:
		case WP_PLIERS:
		case WP_SMOKETRAIL:
		case WP_MEDIC_SYRINGE:
		case WP_SMOKE_BOMB:
		case WP_AMMO:
 		case VERYBIGEXPLOSION: // IRATA added since there is no BG_WeaponInWolfMP() anymore
		case WP_FOOTKICK:
			return NULL;
		default:
			break;
	}

	weap2 = BG_DuplicateWeapon( weap );
	if( weap != weap2 ) {
		return NULL;
	}

	return BG_FindItemForWeapon( weap );
}

weapon_t BG_WeaponForMOD( int MOD ) {
	weapon_t i;

	// jaquboss - we don't return KABAR, in fact we don't even need KABAR ( can be team model )
	if ( MOD == MOD_BACKSTAB || MOD == MOD_THROWKNIFE )
		return WP_KNIFE;

	// IRATA start with 1st real weapon not WP_NONE
	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++) {
		if(GetWeaponTableData(i)->mod == MOD) {
			return i;
		}
	}

	return WP_NONE;
}

const char* rankSoundNames_Allies[NUM_EXPERIENCE_LEVELS] = {
	"",
	"allies_hq_promo_private",
	"allies_hq_promo_corporal",
	"allies_hq_promo_sergeant",
	"allies_hq_promo_sergeant_staff",
	"allies_hq_promo_sergeant_fc",
	"allies_hq_promo_sergeant_master",
	"allies_hq_promo_sergeant_major",
	"allies_hq_promo_lieutenant_second",
	"allies_hq_promo_lieutenant",
	"allies_hq_promo_captain",
	"allies_hq_promo_major",
	"allies_hq_promo_colonel_lieutenant",
	"allies_hq_promo_colonel",
	"allies_hq_promo_general_major",
	"allies_hq_promo_general_brigadier",
	"allies_hq_promo_general_lieutenant",
	"allies_hq_promo_general",
	"allies_hq_promo_general_ofarmy",
};

const char* rankSoundNames_Axis[NUM_EXPERIENCE_LEVELS] = {
	"",
	"axis_hq_promo_private",
	"axis_hq_promo_corporal",
	"axis_hq_promo_sergeant",
	"axis_hq_promo_sergeant_ober",
	"axis_hq_promo_sergeant_haupt",
	"axis_hq_promo_sergeant_stabs",
	"axis_hq_promo_sergeant_oberstabs",
	"axis_hq_promo_lieutenant",
	"axis_hq_promo_lieutenant_ober",
	"axis_hq_promo_captain",
	"axis_hq_promo_major",
	"axis_hq_promo_colonel_leutnant",
	"axis_hq_promo_colonel",
	"axis_hq_promo_general_brigade",
	"axis_hq_promo_general_major",
	"axis_hq_promo_general_lieutenant",
	"axis_hq_promo_general",
	"axis_hq_promo_feldmarschall",
};

const char* rankNames_Axis[NUM_EXPERIENCE_LEVELS] = {
	"Schutze",
	"Oberschutze",
	"Gefreiter",
	"Feldwebel",
	"Oberfeldwebel",
	"Hauptfeldwebel",
	"Stabsfeldwebel",
	"Oberstabsfeldwebel",
	"Leutnant",
	"Oberleutnant",
	"Hauptmann",
	"Major",
	"Oberstleutnant",
	"Oberst",
	"Brigadegeneral",
	"Generalmajor",
	"Generalleutnant",
	"General",
	"Feldmarschall",
};

const char* rankNames_Allies[NUM_EXPERIENCE_LEVELS] = {
	"Private",
	"Private 1st Class",
	"Corporal",
	"Sergeant",
	"Staff Sergeant",
	"Sergeant First Class",
	"Master Sergeant",
	"Sergeant Major",
	"Second Lieutenant",
	"First Lieutenant",
	"Captain",
	"Major",
	"Lieutenant Colonel",
	"Colonel",
	"Brigadier General",
	"Major General",
	"Lieutenant General",
	"General",
	"General of the Army",
};

const char* miniRankNames_Axis[NUM_EXPERIENCE_LEVELS] = {
	"Stz",
	"Otz",
	"Gfr",
	"Fwb",
	"Ofw",
	"Hfw",
	"Sfw",
	"Osf",
	"Ltn",
	"Olt",
	"Hpt",
	"Mjr",
	"Obl",
	"Obs",
	"Bgn",
	"Gmj",
	"LtG",
	"Gen",
	"Fdm",
};

const char* miniRankNames_Allies[NUM_EXPERIENCE_LEVELS] = {
	"Pvt",
	"PFC",
	"Cpl",
	"Sgt",
	"SSg",
	"SFC",
	"MSg",
	"SMj",
	"SLt",
	"FLt",
	"Cpt",
	"Maj",
	"LtC",
	"Cnl",
	"BGn",
	"GMj",
	"GLt",
	"Gen",
	"GoA",
};

/*
=============
BG_Find_PathCorner
=============
*/
pathCorner_t *BG_Find_PathCorner (const char *match)
{
	int i;

	for (i = 0 ; i < numPathCorners; i++) {
		if ( !Q_stricmp(pathCorners[i].name, match) ) {
			return &pathCorners[i];
		}
	}

	return NULL;
}

/*
=============
BG_AddPathCorner
=============
*/
void BG_AddPathCorner(const char* name, vec3_t origin) {
	if(numPathCorners >= MAX_PATH_CORNERS ) {
		Com_Error( ERR_DROP, "MAX PATH CORNERS (%i) hit", MAX_PATH_CORNERS );
	}

	VectorCopy( origin, pathCorners[numPathCorners].origin );
	Q_strncpyz( pathCorners[numPathCorners].name, name, 64 );

	numPathCorners++;
}

/*
=============
BG_Find_Spline
=============
*/
splinePath_t *BG_Find_Spline (const char *match) {
	int i;

	for (i = 0 ; i < numSplinePaths; i++) {
		if ( !Q_stricmp(splinePaths[i].point.name, match) ) {
			return &splinePaths[i];
		}
	}

	return NULL;
}

splinePath_t* BG_AddSplinePath(const char* name, const char* target, vec3_t origin) {
	splinePath_t* spline;
	if(numSplinePaths >= MAX_SPLINE_PATHS) {
		Com_Error( ERR_DROP, "MAX SPLINES (%i) hit", MAX_SPLINE_PATHS );
	}

	spline = &splinePaths[numSplinePaths];

	memset( spline, 0, sizeof( splinePath_t ) );

	VectorCopy( origin, spline->point.origin );

	Q_strncpyz( spline->point.name,	name,						64 );
	Q_strncpyz( spline->strTarget,	target ? target : "",		64 );

	spline->numControls = 0;

	numSplinePaths++;

	return spline;
}

void BG_AddSplineControl(splinePath_t* spline, const char* name) {
	if(spline->numControls >= MAX_SPLINE_CONTROLS) {
		Com_Error( ERR_DROP, "MAX SPLINE CONTROLS (%i) hit", MAX_SPLINE_CONTROLS );
	}

	Q_strncpyz( spline->controls[spline->numControls].name, name, 64 );

	spline->numControls++;
}

float BG_SplineLength(splinePath_t* pSpline) {
	float i;
	float granularity = 0.01f;
	float dist = 0;
	vec3_t	vec[2];
	vec3_t lastPoint;
	vec3_t result;

	memset(&lastPoint, 0, sizeof(lastPoint));

	for(i = 0; i <= 1.f; i += granularity ) {

		BG_CalculateSpline_r( pSpline, vec[0], vec[1], i );
		VectorSubtract( vec[1], vec[0], result );
		VectorMA(vec[0], i, result, result);

		if( i != 0 ) {
			VectorSubtract( result, lastPoint, vec[0] );
			dist += VectorLength( vec[0] );
		}

		VectorCopy( result, lastPoint );
	}

	return dist;
}

void BG_BuildSplinePaths() {
	int i, j;
	pathCorner_t* pnt;
	splinePath_t *spline, *st;

	for( i = 0; i < numSplinePaths; i++ ) {
		spline = &splinePaths[i];

		if(*spline->strTarget) {
			for( j = 0; j < spline->numControls; j++) {
				pnt = BG_Find_PathCorner( spline->controls[j].name );

				if( !pnt ) {
					Com_Printf( "^1Cant find control point (%s) for spline (%s)\n", spline->controls[j].name, spline->point.name );
					// Gordon: Just changing to a warning for now, easier for region compiles...
					continue;

				}
				else {
					VectorCopy( pnt->origin, spline->controls[j].origin );
				}
			}

			st = BG_Find_Spline( spline->strTarget );
			if( !st ) {
				Com_Printf( "^1Cant find target point (%s) for spline (%s)\n", spline->strTarget, spline->point.name );
				// Gordon: Just changing to a warning for now, easier for region compiles...
				continue;
			}

			spline->next = st;

			spline->length = BG_SplineLength(spline);
			BG_ComputeSegments( spline );
		}
	}

	for( i = 0; i < numSplinePaths; i++ ) {
		spline = &splinePaths[i];

		if(spline->next) {
			spline->next->prev = spline;
		}
	}
}

splinePath_t* BG_GetSplineData( int number, qboolean* backwards ) {
	if( number < 0 ) {
		*backwards = qtrue;
		number = -number;
	}
	else {
		*backwards = qfalse;
	}
	number--;

	if( number < 0 || number >= numSplinePaths ) {
		return NULL;
	}

	return &splinePaths[number];
}


#define MAX_KNIVES 8	// the max. a player can carry
#define SKILL_KNIVES 4	// the # a player gets when spawning

// core: get the extra ammo for a weapon/class/skill
#define MAX_EXTRA_CLIPS 2
int BG_ExtraAmmoForWeapon( weapon_t weaponNum, int *skillBits, int playerType ) {
	int extraAmmo = 0;
	int extraClips = 0;

	if ( weaponNum <= WP_NONE || weaponNum >= WP_NUM_WEAPONS ) {
		return extraAmmo;
	}

	// jet Pilot - for lvl 1 & 6 light weapons, an extra EXTRA clip
	if ( skillBits[SK_LIGHT_WEAPONS] & (1<<1) ) extraClips++;
	if ( skillBits[SK_LIGHT_WEAPONS] & (1<<6) ) extraClips++;

	switch( weaponNum ) {
		// the grenades are special, because they are dependent on class/skill..
		// grenades do not return the extra amount, but instead the total amount.
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
			extraAmmo = BG_GrenadesForClass( playerType, skillBits );
			break;

		// the syringes are special, because they are dependent on skill..
		// syringes do not return the extra amount, but instead the total amount.
		case WP_POISON_SYRINGE:
		case WP_MEDIC_SYRINGE:
			extraAmmo = (int)BG_GetFromTable( syringeLoadout, skillBits, SK_MEDIC );
			break;
		case WP_MEDIC_ADRENALINE:
			if ( BG_ADRENALINE & ADF_JUSTONE ) {
				extraAmmo = 1;
			}
			else {
				extraAmmo = (int)BG_GetFromTable( syringeLoadout, skillBits, SK_MEDIC );
			}
			break;

		// knives do not return the extra amount, but instead the total amount.
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			if ( skillBits[SK_LIGHT_WEAPONS] & (1<<8) ) {
				extraAmmo = SKILL_KNIVES;
			}
			else {
				extraAmmo = 1;
			}
			break;

		case WP_LUGER:
		case WP_COLT:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_STEN:
		case WP_STEN_MKII:
		case WP_MP34:
		case WP_STG44:
		case WP_CARBINE:
		case WP_KAR98:
		case WP_BAR:
		case WP_BAR_SET:
		case WP_SHOTGUN:
			extraAmmo = skillExtraClips[weaponNum] * extraClips;
			break;

		case WP_MP40:
		case WP_THOMPSON:
			if ( skillBits[SK_FIRST_AID] & (1<<1) && extraClips < MAX_EXTRA_CLIPS ) {
				extraClips++;
			}
			extraAmmo = skillExtraClips[weaponNum] * extraClips;
			break;

		case WP_M7:
		case WP_GPG40:
			if ( skillBits[SK_EXPLOSIVES_AND_CONSTRUCTION] & (1<<1) ) {
				extraAmmo = skillExtraClips[weaponNum];
			}
			break;

		case WP_VENOM:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		case WP_FLAMETHROWER:			// uses only clip (no ammo)
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_MORTAR:
		case WP_MORTAR_SET:
			if ( skillBits[SK_HEAVY_WEAPONS] & (1<<6) ) {
				extraAmmo = skillExtraClips[weaponNum];
			}
			break;

		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_FG42SCOPE:
		case WP_GARAND:
		case WP_K43:
		case WP_FG42:
			if ( skillBits[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] & (1<<1) && extraClips < MAX_EXTRA_CLIPS ) {
				extraClips++;
			}
			extraAmmo = skillExtraClips[weaponNum] * extraClips;
			break;

		default:
			break;
	}

	return extraAmmo;
}

int BG_MaxAmmoForWeapon( weapon_t weaponNum, int *skillBits, int playerType )
{
	// core: note that some weapons use only clip (not ammo),
	// but still, the maxammo is checked here, like it has always been done for all weapons..

	switch (weaponNum) {
		// special weapons that are dependent on class and/or skill..
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_POISON_SYRINGE:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
			return BG_ExtraAmmoForWeapon( weaponNum, skillBits, playerType );

		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			return (BG_ExtraAmmoForWeapon(weaponNum, skillBits, playerType) == 1)? 1 : MAX_KNIVES;

		default:
			return GetWeaponTableData(weaponNum)->maxammo + BG_ExtraAmmoForWeapon( weaponNum, skillBits, playerType );
	}
}

/*
================
BG_CreateRotationMatrix
================
*/
void BG_CreateRotationMatrix(const vec3_t angles, vec3_t matrix[3])
{
	AngleVectors(angles, matrix[0], matrix[1], matrix[2]);
	VectorInverse(matrix[1]);
}

/*
================
BG_RotatePoint
================
*/
void BG_RotatePoint(vec3_t point, const vec3_t matrix[3]) {
	vec3_t tvec;

	VectorCopy(point, tvec);
	point[0] = DotProduct(matrix[0], tvec);
	point[1] = DotProduct(matrix[1], tvec);
	point[2] = DotProduct(matrix[2], tvec);
}

/*
=================
PC_SourceWarning
=================
*/
void PC_SourceWarning(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_YELLOW "WARNING: %s, line %d: %s\n", filename, line, string);
}

/*
=================
PC_SourceError
=================
*/
void PC_SourceError(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

#ifdef GAMEDLL
	Com_Error( ERR_DROP, S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
#else
	Com_Printf(S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
#endif
}

/*
=================
PC_Float_Parse
=================
*/
qboolean PC_Float_Parse(int handle, float *f) {
	pc_token_t token;
	int negative = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] == '-') {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;
		negative = qtrue;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected float but found %s\n", token.string);
		return qfalse;
	}
	if (negative)
		*f = -token.floatvalue;
	else
		*f = token.floatvalue;
	return qtrue;
}

/*
=================
PC_Color_Parse
=================
*/
qboolean PC_Color_Parse(int handle, vec4_t *c) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		(*c)[i] = f;
	}
	return qtrue;
}

/*
=================
PC_Vec_Parse
=================
*/
qboolean PC_Vec_Parse(int handle, vec3_t *c) {
	int i;
	float f;

	for (i = 0; i < 3; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		(*c)[i] = f;
	}
	return qtrue;
}

/*
=================
PC_Int_Parse
=================
*/
qboolean PC_Int_Parse(int handle, int *i) {
	pc_token_t token;
	int negative = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] == '-') {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;
		negative = qtrue;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected integer but found %s\n", token.string);
		return qfalse;
	}
	*i = token.intvalue;
	if (negative)
		*i = - *i;
	return qtrue;
}

#ifdef GAMEDLL
/*
=================
PC_String_Parse
=================
*/
const char* PC_String_Parse(int handle) {
	static char buf[MAX_TOKEN_CHARS];
	pc_token_t token;
	if (!trap_PC_ReadToken(handle, &token))
		return NULL;

	Q_strncpyz( buf, token.string, MAX_TOKEN_CHARS );
    return buf;
}
#else
/*
=================
PC_String_Parse
=================
*/
qboolean PC_String_Parse(int handle, const char **out) {
	pc_token_t token;
	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;

	*(out) = String_Alloc(token.string);
    return qtrue;
}
#endif

/*
=================
PC_String_ParseNoAlloc

Same as one above, but uses a static buff and not the string memory pool
=================
*/
qboolean PC_String_ParseNoAlloc(int handle, char *out, size_t size)
{
	pc_token_t token;

	if( !trap_PC_ReadToken(handle, &token) ) {
		return qfalse;
	}

	Q_strncpyz( out, token.string, size );
    return qtrue;
}

const char* bg_fireteamNames[MAX_FIRETEAMS / 2] =
{
	"Alpha",
	"Bravo",
	"Charlie",
	"Delta",
	"Echo",
	"Foxtrot",
};

const voteType_t voteToggles[] =
{
	{ "vote_allow_comp",			CV_SVF_COMP },
	{ "vote_allow_gametype",		CV_SVF_GAMETYPE },
	{ "vote_allow_kick",			CV_SVF_KICK },
	{ "vote_allow_map",				CV_SVF_MAP },
	{ "vote_allow_matchreset",		CV_SVF_MATCHRESET },
	{ "vote_allow_mutespecs",		CV_SVF_MUTESPECS },
	{ "vote_allow_nextmap",			CV_SVF_NEXTMAP },
	{ "vote_allow_pub",				CV_SVF_PUB },
	{ "vote_allow_referee",			CV_SVF_REFEREE },
	{ "vote_allow_shuffleteamsxp",	CV_SVF_SHUFFLETEAMS },
	{ "vote_allow_shuffleteamsxp_norestart",	CV_SVF_SHUFFLETEAMS_NORESTART },
	{ "vote_allow_swapteams",		CV_SVF_SWAPTEAMS },
	{ "vote_allow_friendlyfire",	CV_SVF_FRIENDLYFIRE },
	{ "vote_allow_timelimit",		CV_SVF_TIMELIMIT },
	{ "vote_allow_warmupdamage",	CV_SVF_WARMUPDAMAGE },
	{ "vote_allow_antilag",			CV_SVF_ANTILAG },
	{ "vote_allow_balancedteams",	CV_SVF_BALANCEDTEAMS },
	{ "vote_allow_muting",			CV_SVF_MUTING },
	{ "vote_allow_surrender",		CV_SVF_SURRENDER },
	{ "vote_allow_restartcampaign",	CV_SVF_RESTARTCAMPAIGN },
	{ "vote_allow_nextcampaign",	CV_SVF_NEXTCAMPAIGN },
	{ "vote_allow_poll",			CV_SVF_POLL },
	{ "vote_allow_maprestart",		CV_SVF_MAPRESTART }
};

int numVotesAvailable = sizeof(voteToggles) / sizeof(voteType_t);

// consts to offset random reinforcement seeds
const unsigned int aReinfSeeds[MAX_REINFSEEDS] = { 11, 3, 13, 7, 2, 5, 1, 17 };

// Weapon full names + headshot capability
const weap_ws_t aWeaponInfo[WS_MAX] =
{
	{ WP_KNIFE,					"KNIF",	"Knife"			},	// 0 WS_KNIFE
	{ WP_LUGER,					"LUGR",	"Luger"			},	// 1 WS_LUGER
	{ WP_COLT,					"COLT",	"Colt"			},	// 2 WS_COLT
	{ WP_MP40,					"MP40",	"MP-40"			},	// 3 WS_MP40
	{ WP_THOMPSON,				"TMPS",	"Thompson"		},	// 4 WS_THOMPSON
	{ WP_STEN,					"STEN",	"Sten"			},	// 5 WS_STEN
	{ WP_FG42,					"FG42",	"FG-42"			},	// 6 WS_FG42
	{ WP_PANZERFAUST,			"PNZR",	"Panzer"		},	// 7 WS_PANZERFAUST
	{ WP_FLAMETHROWER,			"FLAM",	"F.Thrower"		},	// 8 WS_FLAMETHROWER
	{ WP_GRENADE_LAUNCHER,		"GRND",	"Grenade"		},	// 9 WS_GRENADE
	{ WP_MORTAR_SET,			"MRTR",	"Mortar"		},	// 10 WS_MORTAR
	{ WP_DYNAMITE,				"DYNA",	"Dynamite"		},	// 11 WS_DYNAMITE
	{ WP_SMOKE_MARKER,			"ARST",	"Airstrike"		},	// 12 WS_AIRSTRIKE
	{ WP_ARTY,					"ARTY",	"Artillery"		},	// 13 WS_ARTILLERY
	{ WP_SATCHEL,				"STCH",	"Satchel"		},	// 16 WS_SATCHEL
	{ WP_GPG40,					"GRLN", "G.Launchr"		},	// 17 WS_GRENADELAUNCHER
	{ WP_LANDMINE,				"LNMN", "Landmine"		},	// 18 WS_LANDMINE
	{ WP_MOBILE_MG42_SET,		"MG42",	"MG-42 Gun"		},	// 19 WS_MG42
	{ WP_GARAND,				"GARN",	"Garand"		},	// 20 WS_GARAND
	{ WP_K43,					"K-43",	"K43 Rifle"		},	// 21 WS_K43
	{ WP_SHOTGUN,				"SHOT", "Shotgun"		},  // 22 WS_SHOTGUN
	{ WP_MOBILE_BROWNING_SET,	"BRNG", "Browning MG"	},	// 23 WS_BROWNING
	{ WP_STG44,					"MP44", "StG 44"		},	// 24 WS_STG44
	{ WP_BAR_SET,				"BAR",  "BAR"			},	// 26 WS_BAR
	{ WP_BAZOOKA,				"BAZ",  "Bazooka"		},  // 27 WS_BAZOOKA
	{ WP_MP34,					"MP34",	"MP34"			},  // 28 WS_MP34
	{ WP_VENOM,					"VENM",	"Venom"			},	// 29 WS_VENOM
	{ WP_POISON_SYRINGE,		"POIS", "Poison"		},	// 30 WS_POISON
	{ WP_FOOTKICK,				"FOOT", "Foot Kick"		},	// 31 WS_FOOTKICK
};

char *WeaponStatsName( int WS_Index ) {
	int weapon = aWeaponInfo[WS_Index].WP_Index;
	if ( BG_Weapons[weapon].statname[0] != 0 ) {
		return (char *)&BG_Weapons[weapon].statname[0];
	} else {
		return (char *)aWeaponInfo[WS_Index].pszName;
	}
}


// Multiview: Convert weaponstate to simpler format
int BG_simpleWeaponState(int ws)
{
	switch(ws) {
		case WEAPON_READY:
//		case WEAPON_READYING:
//		case WEAPON_RELAXING:
			return(WSTATE_IDLE);
		case WEAPON_RAISING:
		case WEAPON_DROPPING:
		case WEAPON_DROPPING_TORELOAD:
			return(WSTATE_SWITCH);
		case WEAPON_FIRING:
		case WEAPON_FIRINGALT:
			return(WSTATE_FIRE);
		case WEAPON_RELOADING:
			return(WSTATE_RELOAD);
	}

	return(WSTATE_IDLE);
}

#ifdef MV_SUPPORT
// Multiview: Reduce hint info to 2 bits.  However, we can really
// have up to 8 values, as some hints will have a 0 value for
// cursorHintVal
int BG_simpleHintsCollapse(int hint, int val)
{
	switch(hint) {
		case HINT_DISARM:
			if(val > 0)	return(0);
		case HINT_BUILD:
			if(val > 0) return(1);
		case HINT_BREAKABLE:
			if(val == 0) return(1);
		case HINT_DOOR_ROTATING:
		case HINT_BUTTON:
		case HINT_MG42:
			if(val == 0) return(2);
		case HINT_BREAKABLE_DYNAMITE:
			if(val == 0) return(3);
	}

	return(0);
}

// Multiview: Expand the hints.  Because we map a couple hints
// into a single value, we can't replicate the proper hint back
// in all cases.
int BG_simpleHintsExpand(int hint, int val)
{
	switch(hint) {
		case 0: return((val >= 0) ? HINT_DISARM : 0);
		case 1: return((val >= 0) ? HINT_BUILD : HINT_BREAKABLE);
		case 2: return((val >= 0) ? HINT_BUILD : HINT_MG42);
		case 3: return((val >= 0) ? HINT_BUILD : HINT_BREAKABLE_DYNAMITE);
	}

	return(0);
}
#endif

// Real printable charater count
int BG_drawStrlen(const char *str)
{
	int cnt = 0;

	while(*str) {
		if(Q_IsColorString(str)) str += 2;
		else {
			cnt++;
			str++;
		}
	}
	return(cnt);
}

// Copies a color string, with limit of real chars to print
//		in = reference buffer w/color
//		out = target buffer
//		str_max = max size of printable string
//		out_max = max size of target buffer
//
// Returns size of printable string
int BG_colorstrncpyz(char *in, char *out, int str_max, int out_max)
{
	int str_len = 0;	// current printable string size
	int out_len = 0;	// current true string size
	const int in_len = strlen(in);

	out_max--;
	while(*in && out_len < out_max && str_len < str_max) {
		if(*in == '^') {
			if(out_len + 2 >= in_len && out_len + 2 >= out_max)
				break;

			*out++ = *in++;
			*out++ = *in++;
			out_len += 2;
			continue;
		}

		*out++ = *in++;
		str_len++;
		out_len++;
	}

	*out = 0;

	return(str_len);
}

int BG_strRelPos(char *in, int index)
{
	int cPrintable = 0;
	const char *ref = in;

	while(*ref && cPrintable < index) {
		if(Q_IsColorString(ref)) ref += 2;
		else {
			ref++;
			cPrintable++;
		}
	}

	return(ref - in);
}

// strip colors and control codes, copying up to dwMaxLength-1 "good" chars and nul-terminating
// returns the length of the cleaned string
// CHRUKER: b069 - Cleaned up a few compiler warnings
int BG_cleanName( const char *pszIn, char *pszOut, int dwMaxLength, qboolean fCRLF )
{
	const char *pInCopy = pszIn;
	const char *pszOutStart = pszOut;

	while( *pInCopy && ( pszOut - pszOutStart < dwMaxLength - 1 ) ) {
		if( *pInCopy == '^' )
			pInCopy += ((pInCopy[1] == 0) ? 1 : 2);
		else if( (*pInCopy < 32 && (!fCRLF || *pInCopy != '\n')) || (*pInCopy > 126))
			pInCopy++;
		else
			*pszOut++ = *pInCopy++;
	}

	*pszOut = 0;
	return( pszOut - pszOutStart );
}

// Only used locally
typedef struct {
	char *colorname;
	vec4_t *color;
} colorTable_t;

// Colors for crosshairs
colorTable_t OSP_Colortable[] =
{
	{ "white",		&colorWhite },
	{ "red",		&colorRed },
	{ "green",		&colorGreen },
	{ "blue",		&colorBlue },
	{ "yellow",		&colorYellow },
	{ "magenta",	&colorMagenta },
	{ "cyan",		&colorCyan },
	{ "orange",		&colorOrange },
	{ "mdred",		&colorMdRed },
	{ "mdgreen",	&colorMdGreen },
	{ "dkgreen",	&colorDkGreen },
	{ "mdcyan",		&colorMdCyan },
	{ "mdyellow",	&colorMdYellow },
	{ "mdorange",	&colorMdOrange },
	{ "mdblue",		&colorMdBlue },
	{ "ltgrey",		&colorLtGrey },
	{ "mdgrey",		&colorMdGrey },
	{ "dkgrey",		&colorDkGrey },
	{ "black",		&colorBlack },
	{ "brown",		&clrBrown },
	{ NULL,			NULL }
};

extern void trap_Cvar_Set( const char *var_name, const char *value );
void BG_setCrosshair(char *colString, float *col, float alpha, char *cvarName)
{
	char *s = colString;

	col[0] = 1.0f;
	col[1] = 1.0f;
	col[2] = 1.0f;
	col[3] = (alpha > 1.0f) ? 1.0f : (alpha < 0.0f) ? 0.0f : alpha;

	if(*s == '0' && (*(s+1) == 'x' || *(s+1) == 'X')) {
		s +=2;
		//parse rrggbb
		if(Q_IsHexColorString(s)) {
			col[0] = ((float)(gethex(*(s))   * 16 + gethex(*(s+1)))) / 255.00;
			col[1] = ((float)(gethex(*(s+2)) * 16 + gethex(*(s+3)))) / 255.00;
			col[2] = ((float)(gethex(*(s+4)) * 16 + gethex(*(s+5)))) / 255.00;
			return;
		}
	}
	else {
		int i = 0;
		while(OSP_Colortable[i].colorname != NULL) {
			if(Q_stricmp(s, OSP_Colortable[i].colorname) == 0) {
				col[0] = (*OSP_Colortable[i].color)[0];
				col[1] = (*OSP_Colortable[i].color)[1];
				col[2] = (*OSP_Colortable[i].color)[2];
				return;
			}
			i++;
		}
	}

	trap_Cvar_Set(cvarName, "White");
}

qboolean BG_isLightWeaponSupportingFastReload( int weapon ) {
	switch (weapon) {
		case WP_LUGER:
		case WP_COLT:
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_SILENCER:
		case WP_FG42:
		case WP_SILENCED_COLT:
		case WP_STEN_MKII:
		case WP_STG44:
		case WP_BAR:
		case WP_MP34:
			return qtrue;
		default:
			return qfalse;
	}
}

///////////////////////////////////////////////////////////////////////////////
typedef struct locInfo_s {
	vec2_t gridStartCoord;
	vec2_t gridStep;
} locInfo_t;

static locInfo_t locInfo;

void BG_InitLocations( vec2_t world_mins, vec2_t world_maxs )
{
	// keep this in sync with CG_DrawGrid
	locInfo.gridStep[0] = 1200.f;
	locInfo.gridStep[1] = 1200.f;

	// ensure minimal grid density
	while( ( world_maxs[0] - world_mins[0] ) / locInfo.gridStep[0] < 7 )
		locInfo.gridStep[0] -= 50.f;
	while( ( world_mins[1] - world_maxs[1] ) / locInfo.gridStep[1] < 7 )
		locInfo.gridStep[1] -= 50.f;

	locInfo.gridStartCoord[0] = world_mins[0] + .5f * ( ( ( ( world_maxs[0] - world_mins[0] ) / locInfo.gridStep[0] ) - ( (int)(( world_maxs[0] - world_mins[0] ) / locInfo.gridStep[0] ) ) ) * locInfo.gridStep[0] );
	locInfo.gridStartCoord[1] = world_mins[1] - .5f * ( ( ( ( world_mins[1] - world_maxs[1] ) / locInfo.gridStep[1] ) - ( (int)(( world_mins[1] - world_maxs[1] ) / locInfo.gridStep[1] ) ) ) * locInfo.gridStep[1] );
}

char *BG_GetLocationString( vec_t* pos )
{
    static char coord[6];
	int x, y;

	coord[0] = '\0';

	x = (pos[0] - locInfo.gridStartCoord[0]) / locInfo.gridStep[0];
	y = (locInfo.gridStartCoord[1] - pos[1]) / locInfo.gridStep[1];

	if( x < 0 ) x = 0;
	if( y < 0 ) y = 0;

	Com_sprintf( coord, sizeof(coord), "%c,%i", 'A' + x, y );

	return coord;
}

void BG_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce ) {
	float	backoff;
	float	change;
	int		i;

	backoff = DotProduct (in, normal);

	if ( backoff < 0 ) {
		backoff *= overbounce;
	}
	else {
		backoff /= overbounce;
	}

	for ( i=0 ; i<3 ; i++ ) {
		change = normal[i]*backoff;
		out[i] = in[i] - change;
	}
}


qboolean BG_BBoxCollision( vec3_t min1, vec3_t max1, vec3_t min2, vec3_t max2 ) {

	if( min1[0] > max2[0] ) {
		return qfalse;
	}
	if( min2[0] > max1[0] ) {
		return qfalse;
	}

	if( min1[1] > max2[1] ) {
		return qfalse;
	}
	if( min2[1] > max1[1] ) {
		return qfalse;
	}

	if( min1[2] > max2[2] ) {
		return qfalse;
	}
	if( min2[2] > max1[2] ) {
		return qfalse;
	}

	return qtrue;
}

/* IRATA unused
weapon_t bg_heavyWeapons[NUM_HEAVY_WEAPONS] = {
	WP_FLAMETHROWER,
	WP_MOBILE_MG42,
	WP_MOBILE_MG42_SET,
	WP_MOBILE_BROWNING,
	WP_MOBILE_BROWNING_SET,
	WP_PANZERFAUST,
	WP_BAZOOKA,
	WP_MORTAR,
	WP_MORTAR_SET,
//	WP_BAR,
//	WP_BAR_SET // jaquboss - default weapons shouldnt be restricted! IRATA: we restrict again if BAR removed from CovOps
	WP_MORTAR2,
	WP_MORTAR2_SET,
	WP_VENOM,
};
 */


/* IRATA unused
weapon_t bg_primaryHeavyWeapons[8] =
{
	WP_FLAMETHROWER,
	WP_MOBILE_MG42,
	WP_MOBILE_BROWNING,
	WP_PANZERFAUST,
	WP_BAZOOKA,
	WP_MORTAR,
	WP_MORTAR2,
	WP_BAR
};
*/

/////////////////////////

int BG_FootstepForSurface( int surfaceFlags ) {
	if ( surfaceFlags & SURF_NOSTEPS ) {
		return FOOTSTEP_TOTAL;
	}

	if ( surfaceFlags & SURF_METAL ) {
		return FOOTSTEP_METAL;
	}

	if ( surfaceFlags & SURF_WOOD ) {
		return FOOTSTEP_WOOD;
	}

	if ( surfaceFlags & SURF_GRASS ) {
		return FOOTSTEP_GRASS;
	}

	if ( surfaceFlags & SURF_GRAVEL ) {
		return FOOTSTEP_GRAVEL;
	}

	if ( surfaceFlags & SURF_ROOF ) {
		return FOOTSTEP_ROOF;
	}

	if ( surfaceFlags & SURF_SNOW ) {
		return FOOTSTEP_SNOW;
	}

	if ( surfaceFlags & SURF_CARPET ) {
		return FOOTSTEP_CARPET;
	}

	if ( surfaceFlags & SURF_SPLASH ) {
		return FOOTSTEP_SPLASH;
	}

	return FOOTSTEP_NORMAL;
}


// jet Pilot - Charge costs for various weapons/abilities
//   0     1     2     3     4     5     6     7     8     9

// jaquboss - assuming float now

float   armSpeed_LandMine[NUM_SKILL_LEVELS] =
{   12,   12,   24,   24,   24,   24,   32,   32,   32,   32};
float   armSpeed_Dynamite[NUM_SKILL_LEVELS] =
{    7,    7,   14,   14,   14,   14,   14,   14,   14,   14};
float   disarmSpeed_Dynamite[NUM_SKILL_LEVELS] =
{    3,    3,    6,    6,    6,    6,    6,    6,    6,    6};
float   disarmSpeed_LandMine[NUM_SKILL_LEVELS] =
{    3,    3,    6,    6,    6,    6,    8,    8,    8,    8};
float	  artilleryBombs[NUM_SKILL_LEVELS] =
{	 5,	   5,    5,    9,    9,    9,    9,    9,    9,    9};
float   disarmSpeed_Satchel[NUM_SKILL_LEVELS] =
{    3,    3,    3,    3,    3,    3,    6,    6,    6,    6};
float chargeCost_Repair[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 1.0f, .66f, .66f, .66f, .66f, .50f, .50f, .50f};

float chargeCost_Construct[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 0.66f, .66f, .66f, .66f, .66f, .50f, .50f, .50f};

float chargeCost_Landmine[NUM_SKILL_LEVELS] =
{ 0.5f, 0.5f, 0.5f, .33f, .33f, .33f, .33f, .25f, .25f, .25f};
float chargeCost_Dynamite[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 1.0f, .66f, .66f, .66f, .66f, .66f, .66f, .66f}; // jaquboss no more double-dynamite
float chargeCost_Riflenade[NUM_SKILL_LEVELS] =
{ .50f, .50f, .50f, .50f, .50f, .50f, .50f, .50f, .35f, .35f};
float chargeCost_Defuse[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 1.0f, .66f, .66f, .66f, .66f, .50f, .50f, .50f};
float chargeCost_Satchel[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, .66f, .66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Smoke[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, .66f, .66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Health[NUM_SKILL_LEVELS] =
{ .25f, .15f, .15f, .15f, .15f, .15f, .15f, .15f, .15f, .15f};
float chargeCost_Ammo[NUM_SKILL_LEVELS] =
{ .25f, .15f, .15f, .15f, .15f, .15f, .15f, .15f, .15f, .15f};
float chargeCost_Adrenaline[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
float chargeCost_Artillery[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, .66f, .66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Airstrike[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, .66f, .66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Panzer[NUM_SKILL_LEVELS] =
{ 1.f, 0.66f, 0.66f,.66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Bazooka[NUM_SKILL_LEVELS] =
{ 1.f, 0.66f, 0.66f,.66f, .66f, .66f, .66f, .66f, .66f, .66f};
float chargeCost_Mortar[NUM_SKILL_LEVELS] =
{ 0.5f, 0.33f, 0.33f,.33f, .33f,.33f, .33f, .33f, .33f, .33f};
float lightWeapon_Spread[NUM_SKILL_LEVELS] =
{ 1.0f, 1.0f, 1.0f, .65f, .65f, .65f, .65f, .65f, .65f, .65f};
float disguiseTestCost[NUM_SKILL_LEVELS] =
{ 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, .75f, .65f, .55f, .45f, .35f};

//   0     1     2     3     4     5     6     7     8     9
// end of jP additions


float BG_ChargeUsage( weapon_t weapon, int skillBits[SK_NUM_SKILLS] )
{
	switch ( weapon ) {
		case WP_BAZOOKA:
			return BG_GetFromTable(chargeCost_Bazooka, skillBits, SK_HEAVY_WEAPONS);
		case WP_PANZERFAUST:
			return BG_GetFromTable(chargeCost_Panzer, skillBits, SK_HEAVY_WEAPONS);
		case WP_M7:
		case WP_GPG40:
			return BG_GetFromTable(chargeCost_Riflenade, skillBits, SK_ENGINEER);
		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			return BG_GetFromTable(chargeCost_Mortar, skillBits, SK_HEAVY_WEAPONS);
		case WP_SMOKE_BOMB:
		case WP_SATCHEL:
			return BG_GetFromTable(chargeCost_Satchel, skillBits, SK_COVERTOPS);
		case WP_LANDMINE:
			return BG_GetFromTable(chargeCost_Landmine, skillBits, SK_ENGINEER);
		case WP_DYNAMITE:
			return BG_GetFromTable(chargeCost_Dynamite, skillBits, SK_ENGINEER);
		case WP_AMMO:
			return BG_GetFromTable(chargeCost_Ammo, skillBits, SK_SIGNALS);
		case WP_SMOKE_MARKER:
			return BG_GetFromTable(chargeCost_Airstrike, skillBits, SK_SIGNALS);
		case WP_MEDKIT:
			return BG_GetFromTable(chargeCost_Health, skillBits, SK_MEDIC);
		case WP_MEDIC_ADRENALINE:
			return BG_GetFromTable(chargeCost_Adrenaline, skillBits, SK_MEDIC);
        case WP_BINOCULARS:
            return BG_GetFromTable(chargeCost_Artillery, skillBits, SK_SIGNALS);
		default:
			return -1;
	}

	return -1;
}

qboolean BG_CheckCharge( pmove_t *pm )
{
	if(pm->ps->eFlags & EF_ZOOMING) {
		if(pm->ps->stats[STAT_PLAYER_CLASS] == PC_FIELDOPS) {
            if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime * BG_GetFromTable(chargeCost_Artillery, pm->skillBits, SK_SIGNALS)) )
			    return qfalse;
            else
                return qtrue;
        }
    }

	// JPW NERVE -- in multiplayer, don't allow firing if charge bar isn't full
	switch (pm->ps->weapon) {
		case WP_BAZOOKA:
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->soldierChargeTime * BG_GetFromTable(chargeCost_Bazooka, pm->skillBits, SK_HEAVY_WEAPONS)) )
				return qfalse;
			break;

		case WP_PANZERFAUST:
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->soldierChargeTime * BG_GetFromTable(chargeCost_Panzer, pm->skillBits, SK_HEAVY_WEAPONS) ) )
				return qfalse;
			break;

		case WP_GPG40:
		case WP_M7:
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime * BG_GetFromTable(chargeCost_Riflenade, pm->skillBits, SK_ENGINEER) ) )
				return qfalse;
			break;

		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->soldierChargeTime * BG_GetFromTable(chargeCost_Mortar, pm->skillBits, SK_HEAVY_WEAPONS) ) )
				return qfalse;
			break;

		case WP_SMOKE_BOMB:
		case WP_SATCHEL:
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->covertopsChargeTime * BG_GetFromTable(chargeCost_Satchel, pm->skillBits, SK_COVERTOPS) ))
				return qfalse;
			break;

		case WP_LANDMINE:
			if(pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime * BG_GetFromTable(chargeCost_Landmine, pm->skillBits, SK_ENGINEER) ))
				return qfalse;
			break;

		case WP_DYNAMITE:
			if(pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime * BG_GetFromTable(chargeCost_Dynamite, pm->skillBits, SK_ENGINEER) ))
				return qfalse;
			break;

		case WP_AMMO:
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime *  BG_GetFromTable(chargeCost_Ammo, pm->skillBits, SK_FIELDOPS) ) )
				return qfalse;
			break;

		case WP_MEDKIT:
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->medicChargeTime * BG_GetFromTable(chargeCost_Health, pm->skillBits, SK_MEDIC) ) )
				return qfalse;
			break;

		case WP_SMOKE_MARKER:
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime *  BG_GetFromTable(chargeCost_Airstrike, pm->skillBits, SK_FIELDOPS) ) )
				return qfalse;
			break;

		case WP_MEDIC_ADRENALINE:
			if ( pm->ps->powerups[PW_ADRENALINE] )
				return qfalse;
			break;

		default:
			break;
	}

	return qtrue;
}

/*
============
Q_vsnprintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final string if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count,
then count characters are written and -1 is returned and no trailing '\0' is added.

Q_vsnPrintf: always append a trailing '\0', returns number of characters written or
returns -1 on failure or if the buffer would be overflowed.

copied over from common.c implementation
============
*/
int Q_vsnprintf( char *dest, int size, const char *fmt, va_list argptr ) {
	int ret;

#ifdef _WIN32
#undef _vsnprintf
	ret = _vsnprintf( dest, size-1, fmt, argptr );
#define _vsnprintf	use_idStr_vsnPrintf
#else
#undef vsnprintf
	ret = vsnprintf( dest, size, fmt, argptr );
#define vsnprintf	use_idStr_vsnPrintf
#endif
	dest[size-1] = '\0';
	if ( ret < 0 || ret >= size ) {
		return -1;
	}
	return ret;
}


// jet Pilot - Grab the substring at specified index
// source string must be separated by and begin with the '\\' character
void BG_IndexedSubstring(char *dest, int maxLen, const char *source, int index)
{
	int		i		= 0;	// start at 1 because 0 should always be a '\\'
	int		j		= 0;
	int		count	= 0;
	int		srcLen = strlen(source);

	while( i < srcLen && source[i] != '\0') {
		if (index == count) {
			while (source[i] != '\\' && source[i] != '\0') {
				dest[j++] = source[i++];
			}
			dest[j] = '\0';
			return;
		}
		else {
			while (source[i] != '\\' && source[i++] != '\0') {
				// just wait
			}

			count++;
		}
	}

	strcpy(dest, "Unknown");
}
