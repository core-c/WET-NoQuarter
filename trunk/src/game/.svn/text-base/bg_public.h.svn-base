/*
 * name:		bg_public.h
 *
 * desc:		definitions shared by both the server game and client game modules
 *
*/

// because games can change separately from the main system version, we need a
// second version that must match between game and cgame

#ifndef __BG_PUBLIC_H__
#define __BG_PUBLIC_H__

#define	GAME_VERSION	"Enemy Territory"

#if defined(_DEBUG)
	#define	GAME_VERSION_DATED	GAME_VERSION
#else
	#define	GAME_VERSION_DATED	(GAME_VERSION ", " Q3_VERSION)
#endif

//bani
#ifdef __GNUC__
#define _attribute(x) __attribute__(x)
#else
#define _attribute(x)
#endif

#define USE_MDXFILE

#define NQ_VERSION "1.3.0b" // compatibility reasons with Omni-Bots...
#define NOQUARTER

#define SPRINTTIME					20000.0f
#define ADRENALINE_TIME				10000.0f	// duration for which injected adrenaline is active..
#define	DEFAULT_GRAVITY				800
#define FORCE_LIMBO_HEALTH			-75 // JPW NERVE
#define	GIB_HEALTH					-175 // JPW NERVE
#define MAX_HEALTH  				125
#define HEALTH_BONUS 		 		15
#define HOLDBREATHTIME				12000
#define HOLDBREATHTIME_LONG			15000
#define	MAX_ITEMS					256
#define MAX_FIRETEAM_MEMBERS		6
#define MAX_SHOTGUN_PELLETS			10
#define	KNIFECHARGETIME				500
#define KNIFESPEED					1300

// IRATA: whenever you change this make sure
// Bullet_Endpos for scope weapons is in sync!
#define MAX_TRACE 8192.0f

#define	ITEM_RADIUS				10 // Rafael changed the radius so that the items would fit in the 3 new containers
#define FLAMETHROWER_RANGE		2500		// DHM - Nerve :: multiplayer range, was 850 in SP
#define	VOTE_TIME				30000	// 30 seconds before vote times out

// Rafael
#define	DEFAULT_VIEWHEIGHT		40
#define CROUCH_VIEWHEIGHT		16
#define DEAD_VIEWHEIGHT			-16

#define PRONE_VIEWHEIGHT		-8

extern vec3_t	playerlegsProneMins;
extern vec3_t	playerlegsProneMaxs;

#define MAX_COMMANDMAP_LAYERS	16

// RF, on fire effects
#define FIRE_FLASH_TIME			2000
#define	FIRE_FLASH_FADEIN_TIME	1000

#define MG42_SPREAD_MP			100

#define MG42_DAMAGE_MP			20
#define MG42_RATE_OF_FIRE_MP	66
#define	MG42_YAWSPEED			300.f		// degrees per second

#define SAY_ALL					0
#define SAY_TEAM				1
#define	SAY_BUDDY				2
#define SAY_TEAMNL				3

#define MAX_FORCECVARS 128
#define MAX_SVCVARS 128

#define SVC_EQUAL			0
#define SVC_GREATER			1
#define SVC_GREATEREQUAL	2
#define SVC_LOWER			3
#define SVC_LOWEREQUAL		4
#define SVC_INSIDE			5
#define SVC_OUTSIDE			6
#define SVC_INCLUDE			7
#define SVC_EXCLUDE			8

typedef struct svCvar_s {
	char	cvarName[MAX_CVAR_VALUE_STRING];
	int		mode;
	char	Val1[MAX_CVAR_VALUE_STRING];
	char	Val2[MAX_CVAR_VALUE_STRING];
} svCvar_t;

typedef struct forceCvar_s {
	char	cvarName[MAX_CVAR_VALUE_STRING];
	char	cvarValue[MAX_CVAR_VALUE_STRING];
} forceCvar_t;

// RF, client damage identifiers

// Arnout: different entity states
typedef enum {
	STATE_DEFAULT,			// ent is linked, can be used and is solid
	STATE_INVISIBLE,		// ent is unlinked, can't be used, doesn't think and is not solid
	STATE_UNDERCONSTRUCTION	// ent is being constructed
} entState_t;

typedef enum {
  SELECT_BUDDY_ALL = 0,
  SELECT_BUDDY_1,
  SELECT_BUDDY_2,
  SELECT_BUDDY_3,
  SELECT_BUDDY_4,
  SELECT_BUDDY_5,
  SELECT_BUDDY_6,

  SELECT_BUDDY_LAST	// must be the last one in the enum

} SelectBuddyFlag;

// RF
#define	MAX_TAGCONNECTS				64

// (SA) zoom sway values
#define	ZOOM_PITCH_AMPLITUDE		0.13f
#define	ZOOM_PITCH_FREQUENCY		0.24f
#define ZOOM_PITCH_MIN_AMPLITUDE	0.1f		// minimum amount of sway even if completely settled on target

#define	ZOOM_YAW_AMPLITUDE			0.7f
#define	ZOOM_YAW_FREQUENCY			0.12f
#define ZOOM_YAW_MIN_AMPLITUDE		0.2f

// DHM - Nerve
#define MAX_OBJECTIVES				8
#define MAX_OID_TRIGGERS			18
// dhm

typedef struct {
	const char *mapName;
	const char *mapLoadName;
	const char *imageName;

	int typeBits;
	int cinematic;

	qhandle_t levelShot;
	qboolean active;

	// NERVE - SMF
	int Timelimit;
	int AxisRespawnTime;
	int AlliedRespawnTime;
	// -NERVE - SMF

	vec2_t mappos;

	const char *briefing;
	const char *lmsbriefing;
	const char *objectives;
} mapInfo;

// rain - 128 -> 512, campaigns are commonplace
#define MAX_CAMPAIGNS			256 // IRATA: decreased from 512 - I've never seen more than 200 campaigns in path ...

// START Mad Doc - TDF changed this from 6 to 10
#define MAX_MAPS_PER_CAMPAIGN	10

#define CPS_IDENT	(('S'<<24)+('P'<<16)+('C'<<8)+'I')
#define CPS_VERSION	1

typedef struct {
	int				mapnameHash;
} cpsMap_t;

typedef struct {
	int				shortnameHash;
	int				progress;
	cpsMap_t		maps[MAX_MAPS_PER_CAMPAIGN];
} cpsCampaign_t;

typedef struct {
	int				ident;
	int				version;
	int				numCampaigns;
	int				profileHash;
} cpsHeader_t;

typedef struct {
	cpsHeader_t		header;
	cpsCampaign_t	campaigns[MAX_CAMPAIGNS];
} cpsFile_t;

qboolean BG_LoadCampaignSave( const char *filename, cpsFile_t *file, const char *profile );
// qboolean BG_StoreCampaignSave( const char *filename, cpsFile_t *file, const char *profile );

typedef struct {
	const char		*campaignShortName;
	const char		*campaignName;
	const char		*campaignDescription;
	const char		*nextCampaignShortName;
	const char		*maps;
	int				mapCount;
	mapInfo			*mapInfos[MAX_MAPS_PER_CAMPAIGN];
	vec2_t			mapTC[2];
	cpsCampaign_t	*cpsCampaign; // if this campaign was found in the campaignsave, more detailed info can be found here

	const char		*campaignShotName;
	int				campaignCinematic;
	qhandle_t		campaignShot;

	qboolean		unlocked;
	int				progress;

	qboolean		initial;
	int				order;

	int				typeBits;
} campaignInfo_t;

// Random reinforcement seed settings
#define MAX_REINFSEEDS					8
#define REINF_RANGE						16		// (0 to n-1 second offset)
#define REINF_BLUEDELT					3		// Allies shift offset
#define REINF_REDDELT					2		// Axis shift offset
extern const unsigned int aReinfSeeds[MAX_REINFSEEDS];

// Client flags for server processing
#define CGF_AUTORELOAD					0x01
#define CGF_STATSDUMP					0x02
#define CGF_AUTOACTIVATE				0x04
#define CGF_PREDICTITEMS				0x08
#define CGF_WEAPALTRELOAD				0x10

#define MAX_MOTDLINES					6

// Multiview settings
#ifdef MV_SUPPORT
  #define MAX_MVCLIENTS					32
  #define MV_SCOREUPDATE_INTERVAL		5000	// in msec
#endif

#define MAX_CHARACTERS					16

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//
// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC						2
#define	CS_WARMUP						5		// server time when the match will be restarted
#define CS_VOTE_TIME					6
#define CS_VOTE_STRING					7
#define	CS_VOTE_YES						8
#define	CS_VOTE_NO						9
#define	CS_GAME_VERSION					10
#define	CS_LEVEL_START_TIME				11		// so the timer only shows the current level
#define	CS_INTERMISSION					12		// when 1, intermission will start in a second or two
#define CS_MULTI_INFO					13
#define CS_MULTI_MAPWINNER				14
#define CS_MULTI_OBJECTIVE				15
//
#define	CS_SCREENFADE					17		// Ridah, used to tell clients to fade their screen to black/normal
#define	CS_FOGVARS						18		//----(SA) used for saving the current state/settings of the fog
#define	CS_SKYBOXORG					19		// this is where we should view the skybox from
#define CS_TARGETEFFECT					20		//----(SA)
#define CS_WOLFINFO						21		// NERVE - SMF
#define CS_FIRSTBLOOD					22		// Team that has first blood
#define CS_ROUNDSCORES1					23		// Axis round wins
#define CS_ROUNDSCORES2					24		// Allied round wins
#define CS_MUSIC_QUEUE					25
#define CS_SCRIPT_MOVER_NAMES			26
#define CS_CONSTRUCTION_NAMES			27
#define CS_REINFSEEDS					28		// Reinforcement seeds
#define CS_SERVERTOGGLES				29		// Shows current enable/disabled settings (for voting UI)
#define CS_GLOBALFOGVARS				30
#define CS_AXIS_MAPS_XP					31
#define CS_ALLIED_MAPS_XP				32
#define	CS_INTERMISSION_START_TIME		33
#define CS_ENDGAME_STATS				34
#define CS_CHARGETIMES					35
#define CS_FILTERCAMS					36
#define CS_NOQUARTERINFO				37
#define CS_SKILLLEVELS					38
#define CS_FORCECVAR					39
#define CS_SVCVAR						40
#define CS_CONFIGNAME					41

#define CS_CSMETHODINFO					42		// core: new configstring handling..

#define	CS_MODELS						64
#define	CS_SOUNDS						( CS_MODELS +				MAX_MODELS					)
#define CS_SHADERS						( CS_SOUNDS +				MAX_SOUNDS					)
#define CS_SHADERSTATE					( CS_SHADERS +				MAX_CS_SHADERS				)
#define CS_SKINS						( CS_SHADERSTATE +			1							)
#define CS_CHARACTERS					( CS_SKINS +				MAX_CS_SKINS				)
#define	CS_PLAYERS						( CS_CHARACTERS +			MAX_CHARACTERS				)
#define CS_MULTI_SPAWNTARGETS			( CS_PLAYERS +				MAX_CLIENTS					)
#define CS_OID_TRIGGERS					( CS_MULTI_SPAWNTARGETS +	MAX_MULTI_SPAWNTARGETS		)
#define CS_OID_DATA						( CS_OID_TRIGGERS +			MAX_OID_TRIGGERS			)
#define CS_DLIGHTS						( CS_OID_DATA +				MAX_OID_TRIGGERS			)
#define CS_SPLINES						( CS_DLIGHTS +				MAX_DLIGHT_CONFIGSTRINGS	)
#define	CS_TAGCONNECTS					( CS_SPLINES +				MAX_SPLINE_CONFIGSTRINGS	)
#define CS_FIRETEAMS					( CS_TAGCONNECTS +			MAX_TAGCONNECTS				)
#define CS_CUSTMOTD						( CS_FIRETEAMS +			MAX_FIRETEAMS				)
#define CS_STRINGS						( CS_CUSTMOTD +				MAX_MOTDLINES				)
#define CS_MAX							( CS_STRINGS +				MAX_CSSTRINGS				)

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

typedef enum {
	GT_SINGLE_PLAYER,
	GT_COOP,
	GT_WOLF,
	GT_WOLF_STOPWATCH,
	GT_WOLF_CAMPAIGN,	// Exactly the same as GT_WOLF, but uses campaign roulation (multiple maps form one virtual map)
	GT_WOLF_LMS,
	GT_WOLF_MAPVOTE,    // ETPub gametype map voting - Credits go to their team. TU!
	GT_MAX_GAME_TYPE
} gametype_t;

typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/

typedef enum {
	PM_NORMAL,		// can accelerate and turn
	PM_NOCLIP,		// noclip movement
	PM_SPECTATOR,	// still run into walls
	PM_DEAD,		// no acceleration or turning, but free falling
	PM_FREEZE,		// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
	PM_PLAYDEAD,		// player requested playdead change
} pmtype_t;

typedef enum {
	WEAPON_READY,
	WEAPON_RAISING,
	WEAPON_RAISING_TORELOAD,
	WEAPON_DROPPING,
	WEAPON_DROPPING_TORELOAD,
	WEAPON_FIRING,
	WEAPON_FIRINGALT,
	WEAPON_RELOADING,	//----(SA)	added
	WEAPON_HOLSTER_IN,
	WEAPON_HOLSTER_OUT,
	// end
} weaponstate_t;

typedef enum {
	WSTATE_IDLE,
	WSTATE_SWITCH,
	WSTATE_FIRE,
	WSTATE_RELOAD
} weaponstateCompact_t;

// pmove->pm_flags	(sent as max 16 bits in msg.c)
#define	PMF_DUCKED			1
#define	PMF_JUMP_HELD		2
#define PMF_LADDER			4		// player is on a ladder
#define	PMF_BACKWARDS_JUMP	8		// go into backwards land
#define	PMF_BACKWARDS_RUN	16		// coast down to backwards run
#define	PMF_TIME_LAND		32		// pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		// pm_time is an air-accelerate only time
#define	PMF_TIME_WATERJUMP	256		// pm_time is waterjump
#define PMF_DOUBLEJUMPING   128     // josh: For double jumping
#define	PMF_RESPAWNED		512		// clear after attack and jump buttons come up
#define	PMF_MEDICUSE		1024
#define PMF_FLAILING		2048	// not really used
#define PMF_FOLLOW			4096	// spectate following another player
#define	PMF_LIMBO			16384	// JPW NERVE limbo state, pm_time is time until reinforce
#define PMF_TIME_LOCKPLAYER	32768	// DHM - Nerve :: Lock all movement and view changes

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK|PMF_TIME_LOCKPLAYER|PMF_MEDICUSE)

// MAPVOTE
#define MAX_VOTE_MAPS 32

typedef enum {
	SK_BATTLE_SENSE,
	SK_EXPLOSIVES_AND_CONSTRUCTION,
	SK_FIRST_AID,
	SK_SIGNALS,
	SK_LIGHT_WEAPONS,
	SK_HEAVY_WEAPONS,
	SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS,
	SK_NUM_SKILLS
} skillType_t;

typedef struct pmoveExt_s
{
	qboolean	bAutoReload;		// do we predict autoreload of weapons

	int			jumpTime;			// used in MP to prevent jump accel

	int			weapAnimTimer;		// don't change low priority animations until this runs out		//----(SA)	added
	int			silencedSideArm;	// Gordon: Keep track of whether the luger/colt is silenced "in holster", prolly want to do this for the kar98 etc too
	int			sprintTime;

	int			airleft;

	// Arnout: MG42 aiming
	float		varc, harc;
	vec3_t		centerangles;

	// int			dtmove;	// doubletap move // IRATA: unused

	int			proneTime;			// time a go-prone or stop-prone move starts, to sync the animation to

	int			proneRaiseTime;		// jaquboss - so 1st person effect can match animation
	// int			playDeadRaiseTime; // IRATA: unused

	//int			proneGroundTime;	// time a prone player last had ground under him // IRATA: never used, just assigned
	float		proneLegsOffset;	// offset legs bounding box

	vec3_t		mountedWeaponAngles;	// mortar, mg42 (prone), etc

	int			weapRecoilTime;		// Arnout: time at which a weapon that has a recoil kickback has been fired last
	int			weapRecoilDuration;
	float		weapRecoilYaw;
	float		weapRecoilPitch;
	int			lastRecoilDeltaTime;

	qboolean	releasedFire;

	// tjw: moved these here from playerState because we can't add
    //      anything to playerState_t without breaking the client

	int			objDrops;		// tjw: the number of times the player has dropped the objective this life.
	int			objDropTime;	// core: added to prevent spamming the message "You must switch to knife to drop objective"

	int			poisonerEnt;
	// int			poisonHurt; // IRATA: never used
	int			poisonAmnt;

	int			shoveTime;

	qboolean	disoriented;
	qboolean	wasDisoriented;

	// jet Pilot - new stats to track

	//int			sfxFlags;			// (ie, view is warped as if underwater) // IRATA: unused

#ifdef GAMEDLL
	qboolean	shoved;
	int			pusher;
#endif

	int			lastFireTime;

	qboolean	artyFire;

	int			kickDelay;
	int			kickTime;

	// end of jP additions

	int			playDeadTime;

	qboolean	weapAltReload;

	// core: killerLock, locked on Your last killer..
	int			viewlocked_time;
	int			viewlocked_killer;

} pmoveExt_t;	// data used both in client and server - store it here
				// instead of playerstate to prevent different engine versions of playerstate between XP and MP

#define	MAXTOUCH	32

typedef struct {
	// state (in / out)
	playerState_t			*ps;
	pmoveExt_t				*pmext;
	struct bg_character_s	*character;

	// command (in)
	usercmd_t	cmd, oldcmd;
	int			tracemask;			// collide against these types of surfaces
	int			debugLevel;			// if set, diagnostic output will be printed

	// NERVE - SMF (in)
	int			gametype;
	int			ltChargeTime;
	int			soldierChargeTime;
	int			engineerChargeTime;
	int			medicChargeTime;
	// -NERVE - SMF
	int			covertopsChargeTime;

	// results (out)
	int			numtouch;
	int			touchents[MAXTOUCH];

	vec3_t		mins, maxs;			// bounding box size

	int			watertype;
	int			waterlevel;

	float		xyspeed;

	int			*skill;				// player skills

//#endif // GAMEDLL

	// for fixed msec Pmove
	int			pmove_fixed;
	int			pmove_msec;

	// callbacks to test the world
	// these will be different functions during game and cgame
	void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );
	// tjw: used to determine if the player move is for prediction
	//      if it is, the movement should trigger no events
	qboolean predict;

	int			skillBits[SK_NUM_SKILLS]; // used for chargebars
#ifdef GAMEDLL
	void		(*voice)( int clientNum, const char *chatId );
#endif

} pmove_t;

// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, pmoveExt_t *pmext, usercmd_t *cmd, void (trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int tracemask );
int Pmove (pmove_t *pmove);
void PmovePredict(pmove_t *pmove, float frametime);

//===================================================================================

#define PC_SOLDIER				0	//	shoot stuff
#define PC_MEDIC				1	//	heal stuff
#define PC_ENGINEER				2	//	build stuff
#define PC_FIELDOPS				3	//	bomb stuff
#define PC_COVERTOPS			4	//	sneak about ;o

#define NUM_PLAYER_CLASSES		5

// JPW NERVE
#define MAX_WEAP_BANKS_MP		10

// core: leaning flags..
#define STAT_LEAN_LEFT			0x00000001
#define STAT_LEAN_RIGHT			0x00000002

// player_state->stats[] indexes
typedef enum {
	STAT_HEALTH,
	STAT_KEYS,						// 16 bit fields
	STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
	STAT_TEAMAURA,
	STAT_MAX_HEALTH,				// health / armor limit, changable by handicap
	STAT_PLAYER_CLASS,				// DHM - Nerve :: player class in multiplayer
	STAT_XP,						// Gordon: "realtime" version of xp that doesnt need to go thru the scoreboard
	STAT_IDENTIFYAMMO,				// percentage of ammo
	STAT_IDENTIFYCLIP,				// percentage of clip
	STAT_XP_OVERFLOW,				// redeye - count XP overflow(every 2^15)
    STAT_PS_FLAGS,					// leaning bitflags.
	STAT_SPRINTTIME,				// core: sprinttime for CG_DrawStaminaBar()
	STAT_AIRLEFT,					// core: airtime for CG_DrawBreathBar()
	STAT_DOGTAGS					// core: collected dogtags counter
} statIndex_t;

// jaquboss - do auras over network, no need for other classes, also auras are not leveled up anyway
#define AURABIT_MEDIC			1
#define AURABIT_ENGINEER		2
#define AURABIT_FIELDOPS		4

// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
typedef enum {
	PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,						// total points damage inflicted so damage beeps can sound on change
	PERS_RANK,
	PERS_TEAM,
	PERS_SPAWN_COUNT,				// incremented every respawn
	PERS_ATTACKER,					// clientnum of last damage inflicter
	PERS_HEADSHOTS,					// was PERS_KILLED - count of the number of times you died
	// these were added for single player awards tracking

// kept as reminder
	// PERS_RESPAWNS_LEFT,				// DHM - Nerve :: number of remaining respawns
	// PERS_RESPAWNS_PENALTY,			// how many respawns you have to sit through before respawning again
// end reminder

	PERS_REVIVE_COUNT = 9,
	// jet Pilot - track these stats

	PERS_AWARDS,					// (0-255 each) Deaths
	PERS_AWARDS2,					// (0-255 each) Best Spree, Bounty

	// Rafael - mg42				// (SA) I don't understand these here.  can someone explain?
	PERS_HWEAPON_USE,
	PERS_KILLSPREE,					// killing spree as a nubmer
	PERS_KILLS,						// total kills as a number
} persEnum_t;

// jet Pilot - hijack these, convert to flags
#define AWARDMASK_BEST_SPREE	0x000000FF	// 0-255		PERS_AWARDS2
#define AWARDMASK_BOUNTY		0x0000FF00  // 0-255 << 8	PERS_AWARDS2
#define AWARDMASK_DEATHS		0x000000FF  // 0-255		PERS_AWARDS
#define AWARDSHIFT_BEST_SPREE	0
#define AWARDSHIFT_BOUNTY		8
#define AWARDSHIFT_DEATHS		0

int BG_GetStatBounty(playerState_t *ps);
int BG_GetKillSpree(playerState_t *ps);
int	BG_GetStatBestSpree(playerState_t *ps);
int BG_GetStatDeaths(playerState_t *ps);

int BG_SetStatBounty(playerState_t *ps, int value);
int	BG_SetKillSpree(playerState_t *ps, int value);
int	BG_SetStatBestSpree(playerState_t *ps, int value);
int BG_SetStatDeaths(playerState_t *ps, int value);

int BG_AdjustBounty(playerState_t *ps, int value);
int BG_AdjustBestSpree(playerState_t *ps, int value);
int BG_AdjustDeaths(playerState_t *ps, int value);

// entityState_t->eFlags
#define	EF_DEAD				0x00000001		// don't draw a foe marker over players with EF_DEAD
#define EF_NONSOLID_BMODEL	0x00000002		// bmodel is visible, but not solid
#define	EF_POISONED			EF_NONSOLID_BMODEL
#define	EF_TELEPORT_BIT		0x00000004		// toggled every time the origin abruptly changes
#define	EF_READY			0x00000008		// player is ready
#define	EF_CROUCHING		0x00000010		// player is crouching
#define	EF_MG42_ACTIVE		0x00000020		// currently using an MG42
#define	EF_NODRAW			0x00000040		// may have an event, but no model (unspawned items)
#define	EF_GLOW				EF_NODRAW		// jaquboss
#define	EF_FIRING			0x00000080		// for lightning gun
#define	EF_INHERITSHADER	EF_FIRING		// some ents will never use EF_FIRING, hijack it for "USESHADER"
#define EF_SPINNING			0x00000100		// (SA) added for level editor control of spinning pickup items
#define EF_BREATH			EF_SPINNING		// Characters will not have EF_SPINNING set, hijack for drawing character breath
#define	EF_TALK				0x00000200		// draw a talk balloon
#define	EF_CONNECTION		0x00000400		// draw a connection trouble sprite
#define	EF_SMOKINGBLACK		0x00000800		// JPW NERVE -- like EF_SMOKING only darker & bigger
#define EF_HEADSHOT			0x00001000		// last hit to player was head shot (Gordon: NOTE: not last hit, but has BEEN shot in the head since respawn)
#define EF_SMOKING			0x00002000		// DHM - Nerve :: ET_GENERAL ents will emit smoke if set // JPW switched to this after my code change
#define	EF_OVERHEATING		(EF_SMOKING | EF_SMOKINGBLACK)	// ydnar: light smoke/steam effect
#define	EF_VOTED			0x00004000		// already cast a vote
#define	EF_TAGCONNECT		0x00008000		// connected to another entity via tag
#define EF_MOUNTEDTANK		EF_TAGCONNECT	// Gordon: duplicated for clarity
#define EF_FAKEBMODEL		0x00010000		// tjw: from etpro
#define	EF_PANTSED			EF_FAKEBMODEL	// jaquboss use this for pants down flag :)
#define	EF_PATH_LINK		0x00020000		// Gordon: linking trains together
#define EF_ZOOMING			0x00040000		// client is zooming
#define	EF_PRONE			0x00080000		// player is prone
#define EF_PRONE_MOVING		0x00100000		// player is prone and moving
//#define EF_VIEWING_CAMERA	0x00200000		// player is viewing a camera
#define EF_BUYING			0x00400000		// player is buying (shrubbot !buy command)
#define EF_PLAYDEAD			0x00800000		// tjw: player is playing dead
// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000
// #define	EF_SPARE2			0x02000000		// Gordon: freed // IRATA: unused
#define	EF_BOUNCE			0x04000000		// for missiles
#define	EF_BOUNCE_HALF		0x08000000		// for missiles
#define	EF_MOVER_STOP		0x10000000		// will push otherwise	// (SA) moved down to make space for one more client flag
#define EF_MOVER_BLOCKED	0x20000000		// mover was blocked dont lerp on the client // xkan, moved down to make space for client flag

#define BG_PlayerMounted( eFlags ) (( eFlags & EF_MG42_ACTIVE ) || ( eFlags & EF_MOUNTEDTANK ))

// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000
typedef enum {
	PW_NONE,

	// (SA) for Wolf
	PW_INVULNERABLE,
	PW_FLAKJACKET,
	PW_HELMET,
	PW_NOFATIGUE,			//----(SA)

	PW_REDFLAG,
	PW_BLUEFLAG,

	PW_OPS_DISGUISED,
	PW_OPS_CLASS_1,
	PW_OPS_CLASS_2,
	PW_OPS_CLASS_3,

	PW_ADRENALINE,
	PW_HELMETSHIELD,

	PW_HEAVY_BURNS,			// Burnt by improved flamethrower, recieving less heal

	PW_BLACKOUT		= 14,	// OSP - spec blackouts. FIXME: we don't need 32bits here...relocate
#ifdef MV_SUPPORT
	PW_MVCLIENTLIST = 15,	// OSP - MV client info.. need a full 32 bits
#endif

	PW_NUM_POWERUPS
} powerup_t;

typedef enum {
	//----(SA)	These will probably all change to INV_n to get the word 'key' out of the game.
	//			id and DM don't want references to 'keys' in the game.
	//			I'll change to 'INV' as the item becomes 'permanent' and not a test item.
	KEY_NONE,
	KEY_1,		// skull
	KEY_2,		// chalice
	KEY_3,		// eye
	KEY_4,		// field radio
	KEY_5,		// satchel charge
	INV_BINOCS,	// binoculars
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_10,
	KEY_11,
	KEY_12,
	KEY_13,
	KEY_14,
	KEY_15,
	KEY_16,
	KEY_LOCKED_PICKABLE, // Mad Doc - TDF: ent can be unlocked with the WP_LOCKPICK.
	KEY_NUM_KEYS
} wkey_t;			// conflicts with types.h

// NOTE: we can only use up to 15 in the client-server stream
// SA NOTE: should be 31 now (I added 1 bit in msg.c)
// RF NOTE: if this changes, please update etmain\botfiles\inv.h
#define	NO_AIRSTRIKE	1
#define	NO_ARTY	2

typedef enum weapon_s
{
	WP_NONE,				//  0 - nothing, sometimes kick reference as this
	WP_KNIFE,				//  1 - axis knife
	WP_LUGER,				//  2 - P08 9mm Luger pistol
	WP_MP40,				//  3 - MP40 smg
	WP_GRENADE_LAUNCHER,	//  4 - Axis stick grenade
	WP_PANZERFAUST,			//  5 - Axis Panzerfaust weapon
	WP_FLAMETHROWER,		//  6 - American flamethrower
	WP_COLT,				//  7 - American Colt .45 cal pistol
	WP_THOMPSON,			//  8 - M1 smg
	WP_GRENADE_PINEAPPLE,	//  9 - American frag grenade
	WP_STEN,				// 10 - British Sten
	WP_MEDIC_SYRINGE,		// 11 - Morphin syringe
	WP_AMMO,				// 12 - Ammo pack
	WP_ARTY,				// 13 - Arty shell, also it's ammo sents airstrike/arty availability to client
	WP_SILENCER,			// 14 - P08 with silencer on
	WP_DYNAMITE,			// 15 - Dynamite pack
	WP_SMOKETRAIL,			// 16 - Smoke on arty hit as well as it's explosion
	VERYBIGEXPLOSION,		// 17 - explosion effect for airplanes
	WP_MEDKIT,				// 18 - Medkit 'launcher'
	WP_BINOCULARS,			// 19 - Binoculars
	WP_PLIERS,				// 20 - Enineer's pliers to build/repair/plant/disarm stuff
	WP_SMOKE_MARKER,		// 21 - Airstrike marker smoke grenade
	WP_KAR98,				// 22 - Engineer's K43
	WP_CARBINE,				// 23 - Engineer's M1 Garand
	WP_GARAND,				// 24 - Covert's M1 Garand
	WP_LANDMINE,			// 25 - landmine
	WP_SATCHEL,				// 26 - satchel 'launcher'
	WP_SATCHEL_DET,			// 27 - satchel detonator
	WP_SMOKE_BOMB,			// 28 - Covert's smoke grenade
	WP_MOBILE_MG42,			// 29 - Mobile MG42
	WP_K43,					// 30 - Covert's K43
	WP_FG42,				// 31 - FG42
	WP_DUMMY_MG42,          // 32 - storing heat on mounted mg42s
	WP_MORTAR,				// 33 - Allied Mortar
	WP_AKIMBO_COLT,			// 34 - akimbo colts
	WP_AKIMBO_LUGER,		// 35 - akimbo lugers
	WP_GPG40,				// 36 - Grenade launcher for K43
	WP_M7,					// 37 - grenade launcher for M1 Garand
	WP_SILENCED_COLT,		// 38 - Colt with silencer on
	WP_GARAND_SCOPE,		// 39 - Active scoped M1 Garand
	WP_K43_SCOPE,			// 40 - Active scoped K43
	WP_FG42SCOPE,			// 41 - Active scoped FG42
	WP_MORTAR_SET,			// 42 - deployed mortar
	WP_MEDIC_ADRENALINE,	// 43 - adrenaline syringe
	WP_AKIMBO_SILENCEDCOLT,	// 44 - akimbo silenced colts
	WP_AKIMBO_SILENCEDLUGER,// 45 - akimbo silenced lugers
	WP_MOBILE_MG42_SET,		// 46 - deployed MG42
	WP_SHOTGUN,				// 47 - Winchester Model 1897 12-gauge
	WP_KNIFE_KABAR,			// 48 - US KABAR Combat Knife
	WP_MOBILE_BROWNING,		// 49 - Browning .30 Light Machinegun
	WP_MOBILE_BROWNING_SET,	// 50 - (as above with bipod deployed)
	WP_BAR,					// 51 - Browning Automatic Rifle
	WP_BAR_SET,				// 52 - (as above with bipod deployed)
	WP_STG44,				// 53 - Sturmgewehr 44 Assault Rifle
	WP_JOHNSON,				// 54 - Johnson
	WP_BAZOOKA,				// 55 - M1 Bazooka Anti-Tank
	WP_MP34,				// 56 - Solothurn S1-100 SMG
	WP_MORTAR2,				// 57 - Axis mortar
	WP_MORTAR2_SET,			// 58 - deployed Axis mortar
	WP_VENOM,				// 59 - Venom machinegun
	WP_POISON_SYRINGE,		// 60 - poisoned syringe
	WP_FOOTKICK,			// 61 - foot kicking
	WP_JOHNSON_SCOPE,		// 62

	WP_NUM_WEAPONS			// 63 - NOTE: this cannot be larger than 64 for AI/player weapons!
} weapon_t;


#define WPF_NO_L0_FOPS_BINOCS			1
#define WPF_UNDERWATER_SYRINGE			2
#define WPF_UNDERWATER_PLIERS			4
#define WPF_TM_AIRSTRIKE_RESTORE_FULL	8
#define WPF_TM_AIRSTRIKE_RESTORE_HALF	16
#define WPF_LANDMINES_TIMEOUT			32 // IRATA; was g_landminetimeout before

typedef struct {
	int kills, teamkills, killedby;
} weaponStats_t;

typedef enum {
	HR_HEAD,
	HR_ARMS,
	HR_BODY,
	HR_LEGS,
	HR_NUM_HITREGIONS,
} hitRegion_t;

// jet Pilot - Skill Name Shortcuts
#define SK_SOLDIER		SK_HEAVY_WEAPONS
#define SK_MEDIC		SK_FIRST_AID
#define SK_ENGINEER		SK_EXPLOSIVES_AND_CONSTRUCTION
#define SK_FIELDOPS		SK_SIGNALS
#define SK_COVERTOPS	SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS
// end of jP additions

extern const char* skillNames[SK_NUM_SKILLS];
extern const char* medalNames[SK_NUM_SKILLS];

#define NUM_SKILL_LEVELS 10
extern int skillLevels[SK_NUM_SKILLS][NUM_SKILL_LEVELS];


typedef struct {
	weaponStats_t	weaponStats[WP_NUM_WEAPONS];
	int				suicides;
	int				hitRegions[HR_NUM_HITREGIONS];
	int				objectiveStats[MAX_OBJECTIVES];
} playerStats_t;

typedef struct ammotable_s {
	int			maxammo;				//
	int			uses;					//
	int			maxclip;				//
	int			defaultStartingAmmo;	// Mad Doc - TDF
	int			defaultStartingClip;	// Mad Doc - TDF
	int			skillclip;				// cs: extra ammo from skill promotion			
	int			reloadTime;				//
	int			fireDelayTime;			//
	int			nextShotTime;			//
//----(SA)	added
	int			maxHeat;				// max active firing time before weapon 'overheats' (at which point the weapon will fail)
	int			coolRate;				// how fast the weapon cools down. (per second)
//----(SA)	end
	int			mod;					// means of death
//----core
	int			clipindex;				// for faster BG_FindClipForWeapon() lookups..
	int			ammoindex;				// for faster BG_FindAmmoForWeapon() lookups..
	qboolean	isAutoReload;
	qboolean	isFastSwitch;			// IRATA: is this fast reload? see BG_isLightWeaponSupportingFastReload
	qboolean	isHeavy;				// IRATA: there is a define to do that too!? ... is this in sync?
//----
} weapontable_t;

// Lookup table to find ammo table entry
extern weapontable_t *GetWeaponTableData(int ammoIndex);

extern int weapAlts[];	// defined in bg_misc.c
extern int weapTeamConversion[];

//--- core: weapon restriction checking ...
//#define WP_NUM_CAN_DISABLE 35 // IRATA: not used
//extern int weaponCanDisable[WP_NUM_CAN_DISABLE];

#define WP_NUM_CAN_RESTRICT 16 // used to read/write WeaponRestrictions
extern int weaponCanRestrict[WP_NUM_CAN_RESTRICT];
//---

void BG_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce );

// TTimo
// NOTE: what about WP_VENOM and other XP weapons?
// rain - #81 - added added akimbo weapons and deployed MG42
/* IRATA unused
#define IS_AUTORELOAD_WEAPON(w) \
	(	w == WP_LUGER				|| w == WP_COLT					|| \
		w == WP_SILENCER			|| w == WP_SILENCED_COLT		|| \
		w == WP_AKIMBO_LUGER		|| w == WP_AKIMBO_COLT			|| \
		w == WP_AKIMBO_SILENCEDLUGER|| w == WP_AKIMBO_SILENCEDCOLT	|| \
		w == WP_STEN				|| w == WP_JOHNOSN				|| \
		w == WP_MP40				|| w == WP_THOMPSON				|| \
		w == WP_STG44				|| w == WP_VENOM				|| \
		w == WP_KAR98				|| w == WP_K43_SCOPE			|| \
		w == WP_K43					|| w == WP_CARBINE				|| \
		w == WP_GARAND_SCOPE		|| w == WP_GARAND				|| \
		w == WP_FG42				|| w == WP_FG42SCOPE			|| \
		w == WP_BAR					|| w == WP_BAR_SET				|| \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_MG42_SET		|| \
		w == WP_MOBILE_BROWNING		|| w == WP_MOBILE_BROWNING_SET	|| \
		w == WP_SHOTGUN				|| w == WP_MP34 				|| \
		w == WP_JOHNSON_SCOPE)
*/

// jet Pilot - thought this may be a good idea, as there are gobs of checks for this shit
/* IRATA unused
#define IS_FASTRELOAD_WEAPON(w) \
	(	w == WP_LUGER				|| w == WP_COLT				|| \
		w == WP_MP40				|| w == WP_THOMPSON			|| \
		w == WP_STEN				|| w == WP_FG42				|| \
		w == WP_SILENCER			|| w == WP_SILENCED_COLT	|| \
		w == WP_SHOTGUN				|| w == WP_STG44			|| \
		w == WP_BAR					|| w == WP_JOHNSON			|| \
		w == WP_MP34 )
*/

#define IS_SCOPED_WEAPON(w) \
	(	w == WP_K43_SCOPE			|| w == WP_GARAND_SCOPE		|| \
		w == WP_FG42SCOPE 			|| w == WP_JOHNSON_SCOPE )

#define IS_HANDGRENADE(w) \
	(	w == WP_GRENADE_PINEAPPLE	|| w == WP_GRENADE_LAUNCHER	)

#define IS_AKIMBO_WEAPON(w) \
	(	w == WP_AKIMBO_COLT			|| w == WP_AKIMBO_LUGER			|| \
		w == WP_AKIMBO_SILENCEDCOLT	|| w == WP_AKIMBO_SILENCEDLUGER )

#define IS_EXPLOSIVE_WEAPON(w) \
	(	w == WP_GRENADE_LAUNCHER	|| weapon==WP_GRENADE_PINEAPPLE	|| \
		w == WP_GPG40				|| weapon==WP_M7				|| \
		w == WP_PANZERFAUST			|| weapon==WP_BAZOOKA			|| \
		w == WP_MORTAR				|| weapon==WP_MORTAR_SET		|| \
		w == WP_MORTAR2				|| weapon==WP_MORTAR2_SET		|| \
		w == WP_DYNAMITE			|| weapon==WP_LANDMINE			|| \
		w == WP_SATCHEL				|| weapon==WP_SATCHEL_DET	)

// This is used in CG_ClassSkillForPosition and in some #defines here
#define IS_HEAVY_WEAPON(w)	\
	(	w == WP_MORTAR				|| w == WP_MORTAR2				|| \
		w == WP_MORTAR_SET			|| w == WP_MORTAR2_SET			|| \
		w == WP_PANZERFAUST			|| w == WP_FLAMETHROWER			|| \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_MG42_SET   	|| \
		w == WP_MOBILE_BROWNING  	|| w == WP_MOBILE_BROWNING_SET 	|| \
		w == WP_BAR  				|| w == WP_BAR_SET 				|| \
        w == WP_BAZOOKA             || w == WP_VENOM  || w == WP_STG44 )

#define IS_PANZER_WEAPON(w)	\
	(	w == WP_PANZERFAUST			|| w == WP_BAZOOKA )

#define IS_RIFLENADE_WEAPON(w)	\
	(	w == WP_GPG40				|| w == WP_M7 )

#define IS_MORTAR_WEAPON(w)	\
	(	w == WP_MORTAR				|| w == WP_MORTAR2				|| \
		w == WP_MORTAR_SET			|| w == WP_MORTAR2_SET )

#define IS_MORTAR_WEAPON_SET(w)	\
	(	w == WP_MORTAR_SET			|| w == WP_MORTAR2_SET )

#define IS_SET_WEAPON(w)	\
	(	w == WP_MORTAR_SET			|| w == WP_MORTAR2_SET 			|| \
		w == WP_MOBILE_MG42_SET   	|| w == WP_MOBILE_BROWNING_SET 	|| \
	 	w == WP_BAR_SET )		


#define IS_SECONDARY(w) \
	(	w == WP_AKIMBO_COLT			|| w == WP_AKIMBO_LUGER		|| \
		w == WP_COLT				|| w == WP_LUGER			|| \
		w == WP_SILENCED_COLT		|| w == WP_SILENCER			|| \
		w == WP_AKIMBO_SILENCEDCOLT	|| w == WP_AKIMBO_SILENCEDLUGER )

#define IS_PISTOL_ALL(w) \
	(	IS_PISTOL(w)				|| IS_SILENCED_PISTOL(w) || \
		IS_AKIMBO_PISTOL(w)			|| IS_AKIMBO_SILENCED_PISTOL(w)	)

#define IS_PISTOL(w) \
	(	w == WP_COLT				|| w == WP_LUGER	)

#define IS_SILENCED_PISTOL(w) \
	(	w == WP_SILENCED_COLT		|| w == WP_SILENCER	)

#define IS_AKIMBO_PISTOL(w) \
	(	w == WP_AKIMBO_COLT			|| w == WP_AKIMBO_LUGER	)

#define IS_AKIMBO_SILENCED_PISTOL(w) \
	(	w == WP_AKIMBO_SILENCEDCOLT	|| w == WP_AKIMBO_SILENCEDLUGER	)

#define IS_SUBMACHINEGUN(w) \
	(	w == WP_THOMPSON			|| w == WP_MP40	)

#define IS_SOLDIER_WEAPON(w) \
	(	IS_HEAVY_WEAPON(w)			|| IS_SUBMACHINEGUN(w) )

/* IRATA: unused
#define IS_SOLDIER4_PRIMARYWEAPON(w) \
	(	IS_HEAVY_WEAPON(w)			|| \
		w == WP_BAR					|| w == WP_STG44		|| \
		w == WP_STEN_MKII			|| w == WP_VENOM	)
*/
#define IS_COVERT_WEAPON(w) \
	(	w == WP_STEN				|| w == WP_JOHNSON	|| \
		w == WP_K43					|| w == WP_FG42			|| \
		w == WP_GARAND				|| w == WP_BAR			|| \
		w == WP_MP34	  )

#define IS_FIELDOPS_WEAPON(w) \
	(	IS_SUBMACHINEGUN(w)			|| w == WP_SHOTGUN 	)

#define IS_ENGINEER_WEAPON(w) \
	(	IS_SUBMACHINEGUN(w)			|| w == WP_SHOTGUN		|| \
		w == WP_KAR98				|| w == WP_CARBINE 	)

#define IS_BIPOD_WEAPON(w) \
	(	w == WP_MOBILE_MG42			|| w == WP_MOBILE_BROWNING	|| \
		w == WP_BAR )

#define IS_NONFLASH_WEAPON(w) \
	(	w == WP_GRENADE_LAUNCHER	|| w == WP_GRENADE_PINEAPPLE	|| \
		w == WP_GPG40				|| w == WP_M7					|| \
		w == WP_KNIFE				|| w == WP_KNIFE_KABAR			|| \
		w == WP_DYNAMITE			|| w == WP_LANDMINE				|| \
		w == WP_SATCHEL				|| w == WP_SATCHEL_DET			|| \
		w == WP_SMOKE_BOMB			|| w == WP_FOOTKICK				|| \
		w == WP_MEDIC_SYRINGE		|| w == WP_MEDIC_ADRENALINE		|| \
		w == WP_PLIERS				|| w == WP_MEDKIT				|| \
		w == WP_AMMO				|| w == WP_BINOCULARS			|| \
		w == WP_SMOKE_MARKER		|| w == WP_POISON_SYRINGE		 )

#define IS_SILENCED_WEAPON(w) \
	(	w == WP_STEN				|| \
		w == WP_GARAND				|| w == WP_K43					|| \
		w == WP_GARAND_SCOPE		|| w == WP_K43_SCOPE			|| \
		w == WP_SILENCED_COLT		|| w == WP_SILENCER				|| \
		w == WP_AKIMBO_SILENCEDCOLT || w == WP_AKIMBO_SILENCEDLUGER )

#define IS_VALID_WEAPON(w) ( w > WP_NONE && w < WP_NUM_WEAPONS )

#define IS_WEAPON_WITH_ALT(w) (	IS_VALID_WEAPON(w) && weapAlts[w] != WP_NONE )

#define GET_ALT_WEAPON(w) (	IS_WEAPON_WITH_ALT(w) ? weapAlts[w] : w )

/*
#define IS_FAST_SWITCH_WEAPON(w) \
	(   IS_SUBMACHINEGUN(w)			|| IS_PISTOL_ALL(w)				|| \
		IS_COVERT_WEAPON(w)			|| w == WP_GRENADE_LAUNCHER		|| \
		w == WP_KAR98				|| w == WP_CARBINE				|| \
		w == WP_SHOTGUN				|| w == WP_GRENADE_PINEAPPLE	|| \
		w == WP_STG44				|| w == WP_PLIERS				|| \
		w == WP_DYNAMITE			|| w == WP_SMOKE_MARKER			|| \
		w == WP_SMOKE_BOMB			|| w == WP_MEDIC_SYRINGE		|| \
		w == WP_MEDIC_ADRENALINE	|| w == WP_MEDKIT				|| \
		w == WP_BINOCULARS			|| w == WP_AMMO					|| \
		w == WP_LANDMINE			|| w == WP_POISON_SYRINGE		|| \
		w == WP_KNIFE				|| w == WP_KNIFE_KABAR	)
*/

// entityState_t->event values
// entity events are for effects that take place relative
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

typedef enum {
	EV_NONE,
	EV_FOOTSTEP,
	// EV_FOOTSTEP_METAL,
	// EV_FOOTSTEP_WOOD,
	// EV_FOOTSTEP_GRASS,
	// EV_FOOTSTEP_GRAVEL,
	// EV_FOOTSTEP_ROOF,
	// EV_FOOTSTEP_SNOW,
	// EV_FOOTSTEP_CARPET,
	EV_FOOTSPLASH,
	// EV_FOOTWADE,
	EV_SWIM,
	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,
	EV_FALL_SHORT,
	EV_FALL_MEDIUM,
	EV_FALL_FAR,
	EV_FALL_NDIE,
	EV_FALL_DMG_10,
	EV_FALL_DMG_15,
	EV_FALL_DMG_25,
	EV_FALL_DMG_50,
	EV_WATER_TOUCH,				// foot touches
	EV_WATER_LEAVE,				// foot leaves
	EV_WATER_UNDER,				// head touches
	EV_WATER_CLEAR,				// head leaves
	EV_ITEM_PICKUP,				// normal item pickups are predictable
	EV_ITEM_PICKUP_QUIET,		// (SA) same, but don't play the default pickup sound as it was specified in the ent
	EV_GLOBAL_ITEM_PICKUP,		// powerup / team sounds are broadcast to everyone
	EV_NOAMMO,
	EV_WEAPONSWITCHED,
	EV_FILL_CLIP,
	EV_WEAP_OVERHEAT,
	EV_CHANGE_WEAPON,
	EV_CHANGE_WEAPON_2,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,
	EV_NOFIRE_UNDERWATER,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_WEAPON_MOUNTEDMG42,
	EV_ITEM_RESPAWN,
	EV_ITEM_POP,
	EV_GRENADE_BOUNCE,			// eventParm will be the soundindex
	EV_GENERAL_SOUND,
	EV_GENERAL_SOUND_VOLUME,
	EV_GLOBAL_SOUND,			// no attenuation
	EV_GLOBAL_CLIENT_SOUND,		// DHM - Nerve :: no attenuation, only plays for specified client
	EV_GLOBAL_TEAM_SOUND,		// no attenuation, team only
	EV_FX_SOUND,
	EV_BULLET_HIT_FLESH,
	EV_BULLET_HIT_WALL,
	EV_MISSILE_HIT,
	EV_MISSILE_MISS,
	EV_RAILTRAIL,
	EV_BULLET,					// otherEntity is the shooter
	EV_LOSE_HAT,				//----(SA)
	EV_PAIN,
	// EV_CROUCH_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_DEATH4,
	EV_OBITUARY,
	EV_STOPSTREAMINGSOUND,		// JPW NERVE swiped from sherman
	// EV_POWERUP_QUAD,
	// EV_POWERUP_BATTLESUIT,
	// EV_POWERUP_REGEN,
	EV_GIB_PLAYER,				// gib a previously living player
	// EV_DEBUG_LINE,
	EV_STOPLOOPINGSOUND,
	EV_SMOKE,
	EV_SPARKS,
	EV_SPARKS_ELECTRIC,
	EV_EXPLODE,					// func_explosive
	EV_RUBBLE,
	EV_EFFECT,					// target_effect
	EV_MORTAREFX,				// mortar firing
	EV_SPINUP,					// JPW NERVE panzerfaust preamble
	// EV_SNOW_ON,
	// EV_SNOW_OFF,
	EV_MISSILE_MISS_SMALL,
	EV_MISSILE_MISS_LARGE,
	EV_MORTAR_IMPACT,
	EV_MORTAR_MISS,
	EV_SHARD,
	EV_JUNK,
	EV_EMITTER,					//----(SA)	added // generic particle emitter that uses client-side particle scripts
	EV_OILPARTICLES,
	EV_OILSLICK,
	EV_OILSLICKREMOVE,
	// EV_MG42EFX,
	EV_SNOWFLURRY,
	EV_DUST,
	EV_RUMBLE_EFX,
	EV_GUNSPARKS,
	EV_FLAMETHROWER_EFFECT,
	EV_MG42BULLET_HIT_FLESH,	// Arnout: these two send the seed as well
	EV_MG42BULLET_HIT_WALL,
	EV_SHAKE,
	EV_DISGUISE_SOUND,
	EV_BUILDDECAYED_SOUND,
	EV_DEBRIS,
	EV_ALERT_SPEAKER,
	EV_POPUPMESSAGE,
	EV_ARTYMESSAGE,
	EV_AIRSTRIKEMESSAGE,
	EV_MEDIC_CALL,
	EV_SHOTGUN_PUMP,			// jaquboss
	// EV_SPAWN,				// jaquboss
	EV_BODY_DP,
	EV_BOUNCE_SOUND,
	EV_KICK,
	EV_WOLFKICK_MISS,
	EV_WOLFKICK_HIT_FLESH,
	EV_WOLFKICK_HIT_WALL,
	EV_THROWKNIFE,
	EV_SHOVE_SOUND,
	EV_WEAPALT,
	EV_PRIVATE_MESSAGE,
	EV_SHOTGUN_FIRED,
	EV_ARTY_DETECTED,
	EV_MISSIONMESSAGE,			// core: mission related messages..
	EV_MAX_EVENTS				// just added as an 'endcap'
} entity_event_t;

typedef enum {
	WEAP_IDLE1,
	WEAP_IDLE2,
	WEAP_ATTACK1,
	WEAP_ATTACK2,
	WEAP_ATTACK_LASTSHOT,	// used when firing the last round before having an empty clip.
	WEAP_DROP,
	WEAP_RAISE,
	WEAP_RELOAD1,
	WEAP_RELOAD2,
	WEAP_RELOAD3,
	WEAP_ALTSWITCHFROM,	// switch from alt fire mode weap (scoped/silencer/etc)
	WEAP_ALTSWITCHTO,	// switch to alt fire mode weap
	WEAP_DROP2,
	WEAP_FASTDROP,
	WEAP_FASTRAISE,
	MAX_WP_ANIMATIONS
} weapAnimNumber_t;

typedef enum hudHeadAnimNumber_s {
	HD_IDLE1,
	HD_IDLE2,
	HD_IDLE3,
	HD_IDLE4,
	HD_IDLE5,
	HD_IDLE6,
	HD_IDLE7,
	HD_IDLE8,
	HD_DAMAGED_IDLE1,
	HD_DAMAGED_IDLE2,
	HD_DAMAGED_IDLE3,
	HD_LEFT,
	HD_RIGHT,
	HD_ATTACK,
	HD_ATTACK_END,
	HD_PAIN,
	MAX_HD_ANIMATIONS
} hudHeadAnimNumber_t;

#define	ANIMFL_LADDERANIM	0x1
#define	ANIMFL_FIRINGANIM	0x2
#define	ANIMFL_REVERSED		0x4

typedef struct animation_s {
#ifdef USE_MDXFILE
	qhandle_t	mdxFile;
#else
	char		mdxFileName[MAX_QPATH];
#endif // USE_MDXFILE
	char		name[MAX_QPATH];
	int			firstFrame;
	int			numFrames;
	int			loopFrames;			// 0 to numFrames
	int			frameLerp;			// msec between frames
	int			initialLerp;		// msec to get to first frame
	int			moveSpeed;
	int			animBlend;			// take this long to blend to next anim

	// derived
	int			duration;
	int			nameHash;
	int			flags;
	int			movetype;
} animation_t;

// done.

// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT		(1<<(ANIM_BITS-1))

// Gordon: renamed these to team_axis/allies, it really was awful....
typedef enum {
	TEAM_FREE,
	TEAM_AXIS,
	TEAM_ALLIES,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;


// OSP - weapon stat info: mapping between MOD_ and WP_ types (FIXME for new ET weapons)
typedef enum extWeaponStats_s
{
	WS_KNIFE,				// 0	-- axis + allied knives
	WS_LUGER,				// 1	-- axis luger + akimbo luger
	WS_COLT,				// 2	-- allied colt + akimbo colt
	WS_MP40,				// 3
	WS_THOMPSON,			// 4
	WS_STEN,				// 5	-- allied sten
	WS_FG42,				// 6	-- axis FG42 (core: ?? -> Also includes WS_BAR (allies version of fg42))
	WS_PANZERFAUST,			// 7
	WS_FLAMETHROWER,		// 8
	WS_GRENADE,				// 9	-- Includes axis and allies grenade types
	WS_MORTAR,				// 10	-- axis + allied mortar_set
	WS_DYNAMITE,			// 11
	WS_AIRSTRIKE,			// 12	-- Lt. smoke grenade attack
	WS_ARTILLERY,			// 13	-- Lt. binocular attack
	WS_SATCHEL,				// 14
	WS_GRENADELAUNCHER,		// 15	-- rifles (axis + allied)
	WS_LANDMINE,			// 16
	WS_MG42,				// 17
	WS_GARAND,				// 18	-- allied carbine + garand (+ scoped)
	WS_K43,					// 19	-- axis k43 + kar98 (+ scoped)
	WS_SHOTGUN,				// 20 (jet Pilot - Shotgun)
	WS_BROWNING,			// 21 (jet Pilot - Mobile Browning)
	WS_STG44,				// 22 (jet Pilot - German StG44 Assault Rifle)
	WS_BAR,					// 23	-- allied soldier BAR + allied covertops BAR
	WS_BAZOOKA,				// 24 (jet Pilot - Bazooka )
	WS_MP34,				// 25 (jet Pilot - Solothurn S1-100 )
	WS_VENOM,				// 26
	WS_POISON,				// 27
	WS_FOOTKICK,			// 28
	WS_JOHNSON,				// 29
	WS_MAX

} extWeaponStats_t;

typedef struct {
	weapon_t WP_Index;
	const char *pszCode;
	const char *pszName;
} weap_ws_t;

extern const weap_ws_t aWeaponInfo[WS_MAX];
// OSP

// means of death
typedef enum {
	MOD_UNKNOWN,				// 	0
	MOD_MACHINEGUN,				// 	1
	MOD_BROWNING,				// 	2
	MOD_MG42,					// 	3
	MOD_GRENADE,				// 	4
	MOD_KNIFE,					// 	5
	MOD_LUGER,					// 	6
	MOD_COLT,					// 	7
	MOD_MP40,					// 	8
	MOD_THOMPSON,				//  9
	MOD_STEN,					// 10
	MOD_GARAND,					// 11
	MOD_SILENCER,				// 12
	MOD_FG42,					// 13
	MOD_FG42_SCOPE,				// 14
	MOD_PANZERFAUST,			// 15
	MOD_GRENADE_LAUNCHER,		// 16
	MOD_FLAMETHROWER,			// 17
	MOD_GRENADE_PINEAPPLE,		// 18
	MOD_MAPMORTAR,				// 19
	MOD_MAPMORTAR_SPLASH,		// 20
	MOD_KICKED,					// 21
	MOD_DYNAMITE,				// 22
	MOD_AIRSTRIKE,				// 23
	MOD_SYRINGE,				// 24
	MOD_AMMO,					// 25
	MOD_ARTY,					// 26
	MOD_WATER,					// 27
	MOD_SLIME,					// 28
	MOD_LAVA,					// 29
	MOD_CRUSH,					// 30
	MOD_TELEFRAG,				// 31
	MOD_FALLING,				// 32
	MOD_SUICIDE,				// 33
	MOD_TARGET_LASER,			// 34
	MOD_TRIGGER_HURT,			// 35
	MOD_EXPLOSIVE,				// 36
	MOD_CARBINE,				// 37
	MOD_KAR98,					// 38
	MOD_GPG40,					// 39
	MOD_M7,						// 40
	MOD_LANDMINE,				// 41
	MOD_SATCHEL,				// 42
	MOD_SMOKEBOMB,				// 43
	MOD_MOBILE_MG42,			// 44
	MOD_SILENCED_COLT,			// 45
	MOD_GARAND_SCOPE,			// 46
	MOD_CRUSH_CONSTRUCTION,		// 47
	MOD_CRUSH_CONSTRUCTIONDEATH,// 48
	MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER,	// 49
	MOD_K43,					// 50
	MOD_K43_SCOPE,				// 51
	MOD_MORTAR,					// 52
	MOD_AKIMBO_COLT,			// 53
	MOD_AKIMBO_LUGER,			// 54
	MOD_AKIMBO_SILENCEDCOLT,	// 55
	MOD_AKIMBO_SILENCEDLUGER,	// 56
	MOD_SMOKEGRENADE,			// 57
	MOD_SWAP_PLACES,			// 58
	MOD_SWITCHTEAM,				// 59
	MOD_GOOMBA,					// 60
	MOD_POISON,					// 61
	MOD_FEAR,					// 62
	MOD_CENSORED,				// 63
	MOD_SHOTGUN,				// 64
	MOD_BACKSTAB,				// 65
	MOD_MOBILE_BROWNING,		// 66
	MOD_BAR,					// 67
	MOD_STG44,					// 68
	MOD_BAZOOKA,				// 69
	MOD_JOHNSON,				// 70
	MOD_MP34,					// 71
	MOD_VENOM,					// 72
	MOD_SHOVE,					// 73
	MOD_THROWKNIFE,				// 74
	MOD_JOHNSON_SCOPE,			// 75

	MOD_NUM_MODS

} meansOfDeath_t;

// obituary flags
#define		OBIT_EXECUTED	0x00000001

//---------------------------------------------------------

// gitem_t->type
typedef enum {
	IT_BAD,
	IT_WEAPON,				// EFX: rotate + upscale + minlight
	IT_AMMO,				// EFX: rotate
	IT_ARMOR,				// EFX: rotate + minlight
	IT_HEALTH,				// EFX: static external sphere + rotating internal
	IT_KEY,
	IT_TEAM,
	IT_POWERUP,				// jaquboss - breather, uniform...
} itemType_t;

#define MAX_ITEM_MODELS 3
#define MAX_ITEM_ICONS 4

// JOSEPH 4-17-00
typedef struct gitem_s
{
	char		*classname;		// spawning name
	int			classnamehash;	// classname hash value
	char		*pickup_sound;  // pick up sound name 	 IRATA: not used anymore/ just kept as reminder
	char		*world_model[MAX_ITEM_MODELS];

	char		*icon;
	char		*ammoicon;
	char		*pickup_name;	// for printing on pickup

	int			quantity;		// for ammo how much, or duration of powerup (value not necessary for ammo/health.  that value set in gameskillnumber[] below)
	itemType_t  giType;			// IT_* flags

	int			giTag;

	int			giAmmoIndex;	// type of weapon ammo this uses.  (ex. WP_MP40 and WP_LUGER share 9mm ammo, so they both have WP_LUGER for giAmmoIndex)
	int			giClipIndex;	// which clip this weapon uses.  this allows the sniper rifle to use the same clip as the garand, etc.
} gitem_t;
// END JOSEPH

// included in both the game dll and the client
extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

typedef enum {
	IL_NULL = 0,
	// it_powerup
	IL_POWERUP_ADRENALINE,
	IL_POWERUP_FLAKJACKET,
	IL_POWERUP_HELMETSHIELD,
	IL_POWERUP_HELMET,
	IL_POWERUP_AXISSOLDIERUNIFORM,
	IL_POWERUP_AXISMEDICUNIFORM,
	IL_POWERUP_AXISENGINEEROPSUNIFORM,
	IL_POWERUP_AXISFIELDOPSUNIFORM,
	IL_POWERUP_AXISCOVERTOPSUNIFORM,
	IL_POWERUP_ALLIEDSOLDIERUNIFORM, // 10
	IL_POWERUP_ALLIEDMEDICUNIFORM,
	IL_POWERUP_ALLIEDENGINEEROPSUNIFORM,
	IL_POWERUP_ALLIEDFIELDOPSUNIFORM,
	IL_POWERUP_ALLIEDCOVERTOPSUNIFORM,
	// it_health
	IL_HEALTH_HEALTH_CABINET,
	IL_HEALTH_HEALTH,
	IL_HEALTH_HEALTH_LARGE,
	IL_HEALTH_HEALTH_SMALL,
	IL_HEALTH_HEALTH_TURKEY,
	IL_HEALTH_HEALTH_BREADANDMEAT,	// 20
	IL_HEALTH_HEALTH_WALL,
	// it_weapon
	IL_WEAPON_MP40,
	IL_WEAPON_THOMPSON,
	IL_WEAPON_AKIMBOLUGER,
	IL_WEAPON_AKIMBOSILENCEDLUGER,
	IL_WEAPON_LUGER,
	IL_WEAPON_SILENCEDCOLT,
	IL_WEAPON_STEN,
	IL_WEAPON_COLT,
	IL_WEAPON_AKIMBOCOLT,			// 30
	IL_WEAPON_AKIMBOSILENCEDCOLT,
	IL_WEAPON_KNIFE,
	IL_WEAPON_PANZERFAUST,
	IL_WEAPON_GRENADELAUNCHER,
	IL_WEAPON_GRENADEPINEAPPLE,
	IL_WEAPON_GRENADESMOKE,
	IL_WEAPON_SMOKETRAIL,
	IL_WEAPON_MEDIC_HEAL,
	IL_WEAPON_DYNAMITE,
	IL_WEAPON_FLAMETHROWER,			// 40
	IL_WEAPON_CLASS_SPECIAL,
	IL_WEAPON_ARTY,
	IL_WEAPON_MEDIC_SYRINGE,
	IL_WEAPON_POISON_SYRINGE,
	IL_WEAPON_MEDIC_ADRENALINE,
	IL_WEAPON_MAGICAMMO,
	IL_WEAPON_MAGICAMMO2,
	IL_WEAPON_MAGICAMMO3,
	IL_WEAPON_BINOCULARS,
	IL_WEAPON_KAR43,				// 50
	IL_WEAPON_KAR43_SCOPE,
	IL_WEAPON_KAR98RIFLE,
	IL_WEAPON_GPG40,
	IL_WEAPON_GPG40_ALLIED,
	IL_WEAPON_M1CARBINERIFLE,
	IL_WEAPON_GARANDRIFLE,
	IL_WEAPON_GARANDRIFLESCOPE,
	IL_WEAPON_FG42,
	IL_WEAPON_FG42SCOPE,
	IL_WEAPON_MORTAR,				// 60
	IL_WEAPON_MORTAR_SET,
	IL_WEAPON_MORTAR2,
	IL_WEAPON_MORTAR2_SET,
	IL_WEAPON_LANDMINE,
	IL_WEAPON_SATCHEL,
	IL_WEAPON_SATCHELDETONATOR,
	IL_WEAPON_SMOKEBOMB,
	IL_WEAPON_MOBILE_MG42,
	IL_WEAPON_MOBILE_MG42_SET,
	IL_WEAPON_SILENCER,				// 70
	IL_WEAPON_DUMMY,
	IL_WEAPON_SHOTGUN,
	IL_WEAPON_KNIFE_KABAR,
	IL_WEAPON_MOBILE_BROWNING_SET,
	IL_WEAPON_MOBILE_BROWNING,
	IL_WEAPON_STG44,
	IL_WEAPON_BAR_SET,
	IL_WEAPON_BAR,
	IL_WEAPON_JOHNSON,
	IL_WEAPON_JOHNSON_SCOPE,		// 80
	IL_WEAPON_BAZOOKA,
	IL_WEAPON_MP34,
	IL_WEAPON_VENOM,
	IL_WEAPON_DOGTAGS,
	// it_ammo
	IL_AMMO_THOMPSON,
	IL_AMMO_MP40,
	IL_AMMO_DYNAMITE,
	IL_AMMO_DISGUISE,
	IL_AMMO_AIRSTRIKE,
	IL_AMMO_LANDMINE,				// 90
	IL_AMMO_SATCHEL_CHARGE,
	IL_AMMO_LUGER,
	IL_AMMO_SMOKE_GRENADE,
	IL_AMMO_COLT,
	IL_AMMO_SYRINGE,
	IL_AMMO_GARAND,
	IL_AMMO_CARBINE,
	IL_AMMO_MG42,
	IL_AMMO_BROWNING,
	IL_AMMO_SHOTGUN,				// 100
	IL_AMMO_STG44,
	IL_AMMO_BAR,
	IL_AMMO_FG42,
	IL_AMMO_MP34,
	IL_AMMO_STEN,
	IL_AMMO_K43,
	IL_AMMO_KAR98,
	IL_AMMO_VENOM,
	IL_AMMO_MORTAR,
	IL_AMMO_BAZOOKA,				// 110
	IL_AMMO_PANZERFAUST,
	IL_AMMO_JOHNSON,
	// it_team
	IL_TEAM_TEAM_CTF_REDFLAG,
	IL_TEAM_TEAM_CTF_BLUEFLAG,
	IL_NUM_ITEMS
} itemIndex_t;

//core: indexes into the bg_itemlist array (for faster lookups)
#define INDEX_IT_POWERUP	IL_POWERUP_ADRENALINE
#define INDEX_IT_HEALTH		IL_HEALTH_HEALTH_CABINET
#define INDEX_IT_WEAPON		IL_WEAPON_MP40
#define INDEX_IT_AMMO		IL_AMMO_THOMPSON
#define INDEX_IT_TEAM		IL_TEAM_TEAM_CTF_REDFLAG


gitem_t	*BG_FindItem( const char *pickupName );
gitem_t	*BG_FindItemForClassName( const char *className, long hash );
gitem_t	*BG_FindItemForWeapon	( weapon_t weapon );
gitem_t *BG_FindItemForAmmo		( int weapon );
weapon_t BG_FindAmmoForWeapon	( weapon_t weapon );
weapon_t BG_FindClipForWeapon	( weapon_t weapon );
//--- core: faster lookups, copy data into a more compact array, for direct indexed access..
void Init_FindClipForWeapon( void );
void Init_FindAmmoForWeapon( void );
//---

qboolean BG_AkimboFireSequence( int weapon, int akimboClip, int mainClip );
qboolean BG_IsAkimboSideArm( int weaponNum, playerState_t *ps );
int BG_AkimboSidearm( int weaponNum );

#define	ITEM_INDEX(x) ((x)-bg_itemlist)

qboolean	BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, int *skill, int teamNum );

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
//#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_LAVA)		//----(SA)	modified since slime is no longer deadly
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)
#define	MASK_MISSILESHOT		(MASK_SHOT | CONTENTS_MISSILECLIP)

//
// entityState_t->eType
//

// cursorhints (stored in ent->s.dmgFlags since that's only used for players at the moment)
typedef enum {
	HINT_NONE,		// reserved
	HINT_FORCENONE,	// reserved
	HINT_PLAYER,
	HINT_ACTIVATE,
	HINT_DOOR,
	HINT_DOOR_ROTATING,
	HINT_DOOR_LOCKED,
	HINT_DOOR_ROTATING_LOCKED,
	HINT_MG42,
	HINT_BREAKABLE,
	HINT_BREAKABLE_DYNAMITE,
	HINT_CHAIR,
	HINT_ALARM,
	HINT_HEALTH,
	HINT_KNIFE,
	HINT_LADDER,
	HINT_BUTTON,
	HINT_WATER,
	HINT_WEAPON,
	HINT_AMMO,
	HINT_POWERUP,
	HINT_INVENTORY,
	HINT_SCENARIC,
	HINT_EXIT,
	HINT_NOEXIT,
	HINT_PLYR_FRIEND,
	HINT_PLYR_NEUTRAL,
	HINT_PLYR_ENEMY,
	HINT_PLYR_UNKNOWN,
	HINT_BUILD,				// DHM - Nerve
	HINT_DISARM,			// DHM - Nerve
	HINT_REVIVE,			// DHM - Nerve
	HINT_DYNAMITE,			// DHM - Nerve
	HINT_CONSTRUCTIBLE,
	HINT_UNIFORM,
	HINT_LANDMINE,
	HINT_TANK,
	HINT_SATCHELCHARGE,
	HINT_PISTOL,			// jet Pilot - Pistol Assassination icon
	HINT_LUGER,
	HINT_REVIVE_ACTIVATE,
	HINT_BAD_USER,			// invisible user with no target
	HINT_NUM_HINTS
} hintType_t;

void	BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splinePath );
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splineData );
void	BG_GetMarkDir( const vec3_t dir, const vec3_t normal, vec3_t out );

void	BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, int time, qboolean snap );
//void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, qboolean snap );
weapon_t BG_DuplicateWeapon( weapon_t weap );
gitem_t* BG_ValidStatWeapon( weapon_t weap );
// weapon_t BG_WeaponForMOD( int MOD ); // IRATA: unused

qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );
qboolean	BG_PlayerSeesItem	( playerState_t *ps, entityState_t *item, int atTime );
int			BG_GrenadesForClass( int cls, int* skillBits );
qboolean	BG_AddMagicAmmo ( playerState_t *ps, int *skillBits, int teamNum, int numOfClips );

#define	OVERCLIP		1.001

//----(SA)	removed PM_ammoNeeded 11/27/00
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce );

#define MAX_CAMPAIGNS_TEXT	8192

typedef enum {
	FOOTSTEP_NORMAL,
	FOOTSTEP_METAL,
	FOOTSTEP_WOOD,
	FOOTSTEP_GRASS,
	FOOTSTEP_GRAVEL,
	FOOTSTEP_SPLASH,
	FOOTSTEP_ROOF,
	FOOTSTEP_SNOW,
	FOOTSTEP_CARPET,

	FOOTSTEP_TOTAL
} footstep_t;

typedef enum {
	BRASSSOUND_METAL = 0,
	BRASSSOUND_SOFT,
	BRASSSOUND_STONE,
	BRASSSOUND_WOOD,
	BRASSSOUND_MAX,
} brassSound_t;

typedef enum {
	FXTYPE_WOOD = 0,
	FXTYPE_GLASS,
	FXTYPE_METAL,
	FXTYPE_GIBS,
	FXTYPE_BRICK,
	FXTYPE_STONE,
	FXTYPE_FABRIC,
	FXTYPE_MAX
} fxType_t;

//==================================================================
// New Animation Scripting Defines

#define	MAX_ANIMSCRIPT_MODELS				32
#define	MAX_ANIMSCRIPT_ITEMS_PER_MODEL		2048
#define	MAX_MODEL_ANIMATIONS				512		// animations per model
#define	MAX_ANIMSCRIPT_ANIMCOMMANDS			8
#define	MAX_ANIMSCRIPT_ITEMS				128
// NOTE: these must all be in sync with string tables in bg_animation.c

typedef enum
{
	ANIM_MT_UNUSED,
	ANIM_MT_IDLE,
	ANIM_MT_IDLECR,
	ANIM_MT_WALK,
	ANIM_MT_WALKBK,
	ANIM_MT_WALKCR,
	ANIM_MT_WALKCRBK,
	ANIM_MT_RUN,
	ANIM_MT_RUNBK,
	ANIM_MT_SWIM,
	ANIM_MT_SWIMBK,
	ANIM_MT_STRAFERIGHT,
	ANIM_MT_STRAFELEFT,
	ANIM_MT_TURNRIGHT,
	ANIM_MT_TURNLEFT,
	ANIM_MT_CLIMBUP,
	ANIM_MT_CLIMBDOWN,
	ANIM_MT_FALLEN,					// DHM - Nerve :: dead, before limbo
	ANIM_MT_PRONE,
	ANIM_MT_PRONEBK,
	ANIM_MT_IDLEPRONE,
	ANIM_MT_FLAILING,
	ANIM_MT_REVIVE,
	ANIM_MT_RADIO,
	ANIM_MT_RADIOCR,
	ANIM_MT_RADIOPRONE,
	ANIM_MT_DEAD,
	NUM_ANIM_MOVETYPES
} scriptAnimMoveTypes_t;

typedef enum
{
	ANIM_ET_PAIN,
	ANIM_ET_DEATH,
	ANIM_ET_FIREWEAPON,
	ANIM_ET_FIREWEAPON2,
	ANIM_ET_JUMP,
	ANIM_ET_JUMPBK,
	ANIM_ET_LAND,
	ANIM_ET_DROPWEAPON,
	ANIM_ET_RAISEWEAPON,
	ANIM_ET_CLIMB_MOUNT,
	ANIM_ET_CLIMB_DISMOUNT,
	ANIM_ET_RELOAD,
	ANIM_ET_KICK,
	ANIM_ET_REVIVE,
	ANIM_ET_DIVE,
	ANIM_ET_PRONE_TO_CROUCH,
	ANIM_ET_DO_ALT_WEAPON_MODE,
	ANIM_ET_UNDO_ALT_WEAPON_MODE,
	ANIM_ET_DO_ALT_WEAPON_MODE_PRONE,
	ANIM_ET_UNDO_ALT_WEAPON_MODE_PRONE,
	ANIM_ET_FIREWEAPONPRONE,
	ANIM_ET_FIREWEAPON2PRONE,
	ANIM_ET_RAISEWEAPONPRONE,
	ANIM_ET_RELOADPRONE,
	ANIM_ET_NOPOWER,
	ANIM_ET_SALUTE,
	ANIM_ET_DEATH_FROM_BEHIND,
	ANIM_ET_RELOAD_SG1,
	ANIM_ET_RELOAD_SG2,
	ANIM_ET_RELOAD_SG3,
	ANIM_ET_RELOADPRONE_SG1,
	ANIM_ET_RELOADPRONE_SG2,
	ANIM_ET_RELOADPRONE_SG3,
	ANIM_ET_RAISE,
	ANIM_ET_FIREWEAPON3,
	ANIM_ET_FIREWEAPON3PRONE,

	NUM_ANIM_EVENTTYPES
} scriptAnimEventTypes_t;

typedef enum
{
	ANIM_BP_UNUSED,
	ANIM_BP_LEGS,
	ANIM_BP_TORSO,
	ANIM_BP_BOTH,

	NUM_ANIM_BODYPARTS
} animBodyPart_t;

typedef enum
{
	ANIM_COND_WEAPON,
	ANIM_COND_ENEMY_POSITION,
	ANIM_COND_ENEMY_WEAPON,
	ANIM_COND_UNDERWATER,
	ANIM_COND_MOUNTED,
	ANIM_COND_MOVETYPE,
	ANIM_COND_UNDERHAND,
	ANIM_COND_LEANING,
	ANIM_COND_IMPACT_POINT,
	ANIM_COND_CROUCHING,
	ANIM_COND_STUNNED,
	ANIM_COND_FIRING,
	ANIM_COND_SHORT_REACTION,
	ANIM_COND_ENEMY_TEAM,
	ANIM_COND_PARACHUTE,
	ANIM_COND_CHARGING,
	ANIM_COND_PLAYERCLASS,//ANIM_COND_SECONDLIFE,
	ANIM_COND_HEALTH_LEVEL,
	ANIM_COND_FLAILING_TYPE,
	ANIM_COND_GEN_BITFLAG,		// xkan 1/15/2003 - general bit flags (to save some space)
	ANIM_COND_AISTATE,			// xkan 1/17/2003 - our current ai state (sometimes more convenient than creating a separate section)
	ANIM_COND_HOLDING,

	NUM_ANIM_CONDITIONS
} scriptAnimConditions_t;

typedef enum
{
	ANIM_DIR_UNUSED,
	ANIM_DIR_BEHIND,
	ANIM_DIR_INFRONT,
	ANIM_DIR_RIGHT,
	ANIM_DIR_LEFT,
} scriptAnimDirections_t;

//-------------------------------------------------------------------

typedef struct
{
	char	*string;
	int		hash;
} animStringItem_t;

typedef struct
{
	int	index;		// reference into the table of possible conditionals
	int	value[2];		// can store anything from weapon bits, to position enums, etc
	qboolean negative; // (,)NOT <condition>
} animScriptCondition_t;

typedef struct
{
	short int	bodyPart[2];	// play this animation on legs/torso/both
	short int	animIndex[2];	// animation index in our list of animations
	short int	animDuration[2];
	short int	soundIndex;
} animScriptCommand_t;

typedef struct
{
	int						numConditions;
	animScriptCondition_t	conditions[NUM_ANIM_CONDITIONS];
	int						numCommands;
	animScriptCommand_t		commands[MAX_ANIMSCRIPT_ANIMCOMMANDS];
} animScriptItem_t;

typedef struct
{
	int		numItems;
	animScriptItem_t	*items[MAX_ANIMSCRIPT_ITEMS];	// pointers into a global list of items
} animScript_t;

typedef struct
{
	char				animationGroup[MAX_QPATH];
	char				animationScript[MAX_QPATH];

	// parsed from the start of the cfg file (this is basically obsolete now - need to get rid of it)
	gender_t			gender;
	footstep_t			footsteps;
	vec3_t				headOffset;
	int					version;
	qboolean			isSkeletal;

	// parsed from animgroup file
	animation_t			*animations[MAX_MODEL_ANIMATIONS];		// anim names, frame ranges, etc
	int					numAnimations, numHeadAnims;

	// parsed from script file
	animScript_t		scriptAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];			// locomotive anims, etc
	animScript_t		scriptCannedAnims[NUM_ANIM_MOVETYPES];					// played randomly
	animScript_t		scriptEvents[NUM_ANIM_EVENTTYPES];						// events that trigger special anims

	// global list of script items for this model
	animScriptItem_t	scriptItems[MAX_ANIMSCRIPT_ITEMS_PER_MODEL];
	int					numScriptItems;

} animModelInfo_t;

// this is the main structure that is duplicated on the client and server
typedef struct
{
	animModelInfo_t		modelInfo[MAX_ANIMSCRIPT_MODELS];
	int					clientConditions[MAX_CLIENTS][NUM_ANIM_CONDITIONS][2];

	// pointers to functions from the owning module
	// TTimo: constify the arg
	int		(*soundIndex)( const char *name );
	void	(*playSound)( int soundIndex, vec3_t org, int clientNum );
} animScriptData_t;

//------------------------------------------------------------------
// Conditional Constants

typedef enum
{
	POSITION_UNUSED,
	POSITION_BEHIND,
	POSITION_INFRONT,
	POSITION_RIGHT,
	POSITION_LEFT,

	NUM_ANIM_COND_POSITIONS
} animScriptPosition_t;

typedef enum
{
	MOUNTED_UNUSED,
	MOUNTED_MG42,

	NUM_ANIM_COND_MOUNTED
} animScriptMounted_t;

typedef enum
{
	LEANING_UNUSED,
	LEANING_RIGHT,
	LEANING_LEFT,

	NUM_ANIM_COND_LEANING
} animScriptLeaning_t;

typedef enum
{
	IMPACTPOINT_UNUSED,
	IMPACTPOINT_HEAD,
	IMPACTPOINT_CHEST,
	IMPACTPOINT_GUT,
	IMPACTPOINT_GROIN,
	IMPACTPOINT_SHOULDER_RIGHT,
	IMPACTPOINT_SHOULDER_LEFT,
	IMPACTPOINT_KNEE_RIGHT,
	IMPACTPOINT_KNEE_LEFT,

	IMPACTPOINT_FIRE,
	IMPACTPOINT_STUN,

	NUM_ANIM_COND_IMPACTPOINT
} animScriptImpactPoint_t;

typedef enum
{
	FLAILING_UNUSED,
	FLAILING_INAIR,
	FLAILING_VCRASH,
	FLAILING_HCRASH,

	NUM_ANIM_COND_FLAILING
} animScriptFlailingType_t;

typedef enum
{
	ANIM_BITFLAG_ZOOMING,

	NUM_ANIM_COND_BITFLAG
} animScriptGenBitFlag_t;

typedef enum
{
	ANIM_BITFLAG_HOLDING,

	NUM_ANIM_COND_HOLDING
} animScriptHoldFlag_t;

typedef enum{
	ACC_BELT_LEFT,	// belt left (lower)
	ACC_BELT_RIGHT,	// belt right (lower)
	ACC_BELT,		// belt (upper)
	ACC_BACK,		// back (upper)
	ACC_WEAPON,		// weapon (upper)
	ACC_WEAPON2,	// weapon2 (upper)
	ACC_HAT,		// hat (head)
	ACC_MOUTH2,		//
	ACC_MOUTH3,		//
	ACC_SHIELD,
	ACC_RANK,		//
	ACC_MAX			// this is bound by network limits, must change network stream to increase this
} accType_t;

//#define ACC_NUM_MOUTH 3	// matches the above count // IRATA: unused

#define MAX_GIB_MODELS		10 // ET uses 10 gib models 0...9

#define MAX_WEAPS_PER_CLASS	15

typedef struct {
	int			classNum;
	const char	*characterFile;
	const char* iconName;
	const char* iconArrow;

	weapon_t	classWeapons[MAX_WEAPS_PER_CLASS];

	qhandle_t	icon;
	qhandle_t	arrow;

} bg_playerclass_t;

typedef struct bg_character_s
{
	char				characterFile[MAX_QPATH];
#ifdef USE_MDXFILE
	qhandle_t			mesh;
	qhandle_t			skin;
	qhandle_t			accModels[ACC_MAX];
	qhandle_t			accSkins[ACC_MAX];
	qhandle_t			hudhead;
	qhandle_t			hudheadskin;
	animation_t			hudheadanimations[MAX_HD_ANIMATIONS];
	qhandle_t			gibModels[MAX_GIB_MODELS];
	qhandle_t			undressedCorpseModel;
	qhandle_t			undressedCorpseSkin;
#endif // USE_MDXFILE
	animModelInfo_t*	animModelInfo;
} bg_character_t;


//------------------------------------------------------------------
// Global Function Decs

void BG_InitWeaponStrings(void);
void BG_AnimParseAnimScript( animModelInfo_t *modelInfo, animScriptData_t *scriptData, const char *filename, char *input );
int BG_AnimScriptAnimation( playerState_t *ps, animModelInfo_t *modelInfo, scriptAnimMoveTypes_t movetype, qboolean isContinue );
int	BG_AnimScriptCannedAnimation( playerState_t *ps, animModelInfo_t *modelInfo );
int	BG_AnimScriptEvent( playerState_t *ps, animModelInfo_t *modelInfo, scriptAnimEventTypes_t event, qboolean isContinue, qboolean force );
int BG_IndexForString( char *token, animStringItem_t *strings, qboolean allowFail );
int BG_PlayAnimName( playerState_t *ps, animModelInfo_t *animModelInfo, char *animName, animBodyPart_t bodyPart, qboolean setTimer, qboolean isContinue, qboolean force );
void BG_ClearAnimTimer( playerState_t *ps, animBodyPart_t bodyPart );
// qboolean BG_ValidAnimScript( int clientNum ); // IRATA: not implemented
char *BG_GetAnimString( animModelInfo_t* animModelInfo, int anim );
void BG_UpdateConditionValue( int client, int condition, int value, qboolean checkConversion );
int BG_GetConditionValue( int client, int condition, qboolean checkConversion );
qboolean BG_GetConditionBitFlag(int client, int condition, int bitNumber);
void BG_SetConditionBitFlag(int client, int condition, int bitNumber);
void BG_ClearConditionBitFlag(int client, int condition, int bitNumber);
int BG_GetAnimScriptAnimation( int client, animModelInfo_t* animModelInfo, aistateEnum_t aistate, scriptAnimMoveTypes_t movetype );
void BG_AnimUpdatePlayerStateConditions( pmove_t *pmove );
animation_t *BG_AnimationForString( char *string, animModelInfo_t *animModelInfo );
animation_t *BG_GetAnimationForIndex( animModelInfo_t* animModelInfo, int index );
int	BG_GetAnimScriptEvent( playerState_t *ps, scriptAnimEventTypes_t event );
int PM_IdleAnimForWeapon ( int weapon );
int PM_RaiseAnimForWeapon ( int weapon );
void PM_ContinueWeaponAnim( int anim );

extern animStringItem_t animStateStr[];
extern animStringItem_t animBodyPartsStr[];

bg_playerclass_t* BG_GetPlayerClassInfo(int team, int cls);
bg_playerclass_t* BG_PlayerClassForPlayerState(playerState_t* ps);
qboolean BG_ClassHasWeapon(bg_playerclass_t* classInfo, weapon_t weap);
qboolean BG_WeaponIsPrimaryForClassAndTeam( int classnum, team_t team, weapon_t weapon );
// int BG_ClassWeaponCount( bg_playerclass_t* classInfo, team_t team ); // IRATA: not implemented
const char* BG_ShortClassnameForNumber( int classNum );
const char* BG_ClassnameForNumber( int classNum );
const char* BG_ClassnameForNumber_Filename( int classNum );
const char* BG_ClassLetterForNumber( int classNum );

// void BG_DisableClassWeapon( bg_playerclass_t* classinfo, int weapon ); // IRATA: not implemented
// void BG_DisableWeaponForAllClasses( int weapon ); // IRATA: not implemented

extern bg_playerclass_t bg_allies_playerclasses[NUM_PLAYER_CLASSES];
extern bg_playerclass_t bg_axis_playerclasses[NUM_PLAYER_CLASSES];

#define MAX_PATH_CORNERS		512

typedef struct {
	char	name[64];
	vec3_t	origin;
} pathCorner_t;

extern	int					numPathCorners;
extern	pathCorner_t		pathCorners[MAX_PATH_CORNERS];

#define NUM_EXPERIENCE_LEVELS 19

typedef enum {
	ME_PLAYER,
	ME_PLAYER_REVIVE,
	ME_PLAYER_DISGUISED,
	ME_CONSTRUCT,
	ME_DESTRUCT,
	ME_DESTRUCT_2,
	ME_LANDMINE,
	ME_TANK,
	ME_TANK_DEAD,
	ME_COMMANDMAP_MARKER,
} mapEntityType_t;

extern const char* rankNames_Axis[NUM_EXPERIENCE_LEVELS];
extern const char* rankNames_Allies[NUM_EXPERIENCE_LEVELS];
extern const char* miniRankNames_Axis[NUM_EXPERIENCE_LEVELS];
extern const char* miniRankNames_Allies[NUM_EXPERIENCE_LEVELS];
extern const char* rankSoundNames_Axis[NUM_EXPERIENCE_LEVELS];
extern const char* rankSoundNames_Allies[NUM_EXPERIENCE_LEVELS];

#define MAX_SPLINE_PATHS		512
#define MAX_SPLINE_CONTROLS		4
#define MAX_SPLINE_SEGMENTS		16

typedef struct splinePath_s splinePath_t;

typedef struct {
	vec3_t	start;
	vec3_t	v_norm;
	float length;
} splineSegment_t;

struct splinePath_s {
	pathCorner_t	point;

	char			strTarget[64];

	splinePath_t*	next;
	splinePath_t*	prev;

	pathCorner_t	controls[MAX_SPLINE_CONTROLS];
	int				numControls;
	splineSegment_t	segments[MAX_SPLINE_SEGMENTS];

	float			length;

	qboolean		isStart;
	qboolean		isEnd;
};

extern	int					numSplinePaths;
extern	splinePath_t		splinePaths[MAX_SPLINE_PATHS];

pathCorner_t    *BG_Find_PathCorner (const char *match);
splinePath_t    *BG_GetSplineData( int number, qboolean* backwards );
void            BG_AddPathCorner(const char* name, vec3_t origin);
splinePath_t    *BG_AddSplinePath(const char* name, const char* target, vec3_t origin);
void            BG_BuildSplinePaths();
splinePath_t    *BG_Find_Spline (const char *match);
float           BG_SplineLength(splinePath_t* pSpline);
void            BG_AddSplineControl(splinePath_t* spline, const char* name);
void            BG_LinearPathOrigin2(float radius, splinePath_t** pSpline, float *deltaTime, vec3_t result, qboolean backwards);
int				BG_ExtraAmmoForWeapon( weapon_t weaponNum, int *skillBits, int playerType );
int             BG_MaxAmmoForWeapon( weapon_t weaponNum, int *skillBits, int playerType );
void            BG_InitLocations( vec2_t world_mins, vec2_t world_maxs );
char            *BG_GetLocationString( vec_t* pos );

#define MAX_FIRETEAMS		12
extern const char* bg_fireteamNames[MAX_FIRETEAMS / 2];

typedef struct {
	int			ident;
	char		joinOrder[MAX_CLIENTS]; // order in which clients joined the fire team (server), client uses to store if a client is on this fireteam
	int			leader; // leader = joinOrder[0] on server, stored here on client
	qboolean	inuse;
	qboolean	priv;
} fireteamData_t;

long BG_StringHashValue( const char *fname );
long BG_StringHashValue_Lwr( const char *fname );

void BG_RotatePoint(vec3_t point, const vec3_t matrix[3]);
void BG_CreateRotationMatrix(const vec3_t angles, vec3_t matrix[3]);

int trap_PC_AddGlobalDefine( char *define );
int trap_PC_LoadSource( const char *filename );
int trap_PC_FreeSource( int handle );
int trap_PC_ReadToken( int handle, pc_token_t *pc_token );
int trap_PC_SourceFileAndLine( int handle, char *filename, int *line );
int	trap_PC_UnReadToken( int handle );

void PC_SourceError(int handle, char *format, ...);
void PC_SourceWarning(int handle, char *format, ...);

#ifdef GAMEDLL
const char* PC_String_Parse( int handle);
const char* PC_Line_Parse( int handle );
#else
const char *String_Alloc(const char *p);
qboolean PC_String_Parse(int handle, const char **out);
#endif
qboolean PC_String_ParseNoAlloc(int handle, char *out, size_t size);
qboolean PC_Int_Parse(int handle, int *i);
qboolean PC_Color_Parse(int handle, vec4_t *c);
qboolean PC_Vec_Parse(int handle, vec3_t *c);
qboolean PC_Float_Parse(int handle, float *f);

typedef enum {
	UIMENU_NONE,
	UIMENU_MAIN,
	UIMENU_INGAME,
	UIMENU_NEED_CD,
	UIMENU_BAD_CD_KEY,
	UIMENU_TEAM,
	UIMENU_POSTGAME,
	UIMENU_HELP,

	UIMENU_WM_QUICKMESSAGE,
	UIMENU_WM_QUICKMESSAGEALT,

	UIMENU_WM_FTQUICKMESSAGE,
	UIMENU_WM_FTQUICKMESSAGEALT,

	UIMENU_WM_TAPOUT,
	UIMENU_WM_TAPOUT_LMS,

	UIMENU_WM_AUTOUPDATE,

	UIMENU_WM_CLASS,
	UIMENU_WM_CLASSALT,

	// ydnar: say, team say, etc
	UIMENU_INGAME_MESSAGEMODE,
} uiMenuCommand_t;

skillType_t BG_ClassSkillForClass( int classnum );

qboolean BG_isLightWeaponSupportingFastReload( int weapon );

int BG_FootstepForSurface( int surfaceFlags );

#define MATCH_MINPLAYERS "4"	// Minimum # of players needed to start a match

#ifdef MV_SUPPORT
// Multiview support
int BG_simpleHintsCollapse(int hint, int val);
int BG_simpleHintsExpand(int hint, int val);
#endif

char *WeaponStatsName( int WS_Index );

int BG_simpleWeaponState(int ws);

// Color escape handling
// int BG_colorstrncpyz(char *in, char *out, int str_max, int out_max); // unused
int BG_drawStrlen(const char *str);
// int BG_strRelPos(char *in, int index); // unused
int BG_cleanName(const char *pszIn, char *pszOut, int dwMaxLength, qboolean fCRLF);

// Crosshair support
void BG_setCrosshair(char *colString, float *col, float alpha, char *cvarName);

// Voting
#define VOTING_DISABLED		((1 << numVotesAvailable) - 1)

typedef struct {
	const char	*pszCvar;
	int			flag;
} voteType_t;

extern const voteType_t voteToggles[];
extern int numVotesAvailable;

// Tracemap
#ifdef CGAMEDLL
void CG_GenerateTracemap( void );
#endif // CGAMEDLL
qboolean BG_LoadTraceMap( char *rawmapname, vec2_t world_mins, vec2_t world_maxs );
float BG_GetSkyHeightAtPoint( vec3_t pos );
float BG_GetSkyGroundHeightAtPoint( vec3_t pos );
float BG_GetGroundHeightAtPoint( vec3_t pos );
int BG_GetTracemapGroundFloor( void );
// int BG_GetTracemapGroundCeil( void );

//
// bg_animgroup.c
//
void BG_ClearAnimationPool( void );
qboolean BG_R_RegisterAnimationGroup( const char *filename, animModelInfo_t *animModelInfo );

//
// bg_character.c
//
typedef struct bg_characterDef_s {
	// common
	char		undressedCorpseModel[MAX_QPATH];
	char		undressedCorpseSkin[MAX_QPATH];
	char		animationGroup[MAX_QPATH];
	char		animationScript[MAX_QPATH];
	char		mesh[MAX_QPATH];
	char		skin[MAX_QPATH];
	char		hudhead[MAX_QPATH];
	char		hudheadskin[MAX_QPATH];
	char		hudheadanims[MAX_QPATH];
} bg_characterDef_t;

qboolean BG_ParseCharacterFile( const char *filename, bg_characterDef_t* characterDef );
bg_character_t *BG_GetCharacter( int team, int cls );
bg_character_t *BG_GetCharacterForPlayerstate( playerState_t *ps );
void BG_ClearCharacterPool( void );
bg_character_t *BG_FindFreeCharacter( const char *characterFile );
bg_character_t *BG_FindCharacter( const char *characterFile );

//
// bg_sscript.c
//
typedef enum {
	S_LT_NOT_LOOPED,
	S_LT_LOOPED_ON,
	S_LT_LOOPED_OFF
} speakerLoopType_t;

typedef enum {
	S_BT_LOCAL,
	S_BT_GLOBAL,
	S_BT_NOPVS
} speakerBroadcastType_t;

typedef struct bg_speaker_s {
	char					filename[MAX_QPATH];
	qhandle_t				noise;
	vec3_t					origin;
	char					targetname[32];
	long					targetnamehash;

	speakerLoopType_t		loop;
    speakerBroadcastType_t	broadcast;
	int						wait;
	int						random;
	int						volume;
	int						range;

	qboolean				activated;
	int						nextActivateTime;
	int						soundTime;
} bg_speaker_t;

void BG_ClearScriptSpeakerPool( void );
int BG_NumScriptSpeakers( void );
int BG_GetIndexForSpeaker( bg_speaker_t *speaker );
bg_speaker_t *BG_GetScriptSpeaker( int index );
qboolean BG_SS_DeleteSpeaker( int index );
qboolean BG_SS_StoreSpeaker( bg_speaker_t *speaker );
qboolean BG_LoadSpeakerScript( const char *filename );

// Lookup table to find ammo table entry
extern weapontable_t WeaponTable[WP_NUM_WEAPONS];
#define GetWeaponTableData(ammoIndex) ((weapontable_t*)(&WeaponTable[ammoIndex]))
// core: weapons that overheat
extern int weapOverheat[8];
// core: get weapons from MOD
extern int modWeaponTableMP[MOD_NUM_MODS][4];
extern qboolean modFalloffDamage[MOD_NUM_MODS];
extern qboolean modHeadshotModeWeapon[MOD_NUM_MODS];
// core: WP_ names to strings
extern char *WP_names[WP_NUM_WEAPONS];


#define MAX_MAP_SIZE 65536

qboolean BG_BBoxCollision( vec3_t min1, vec3_t max1, vec3_t min2, vec3_t max2 );

//
// bg_stats.c
//

typedef struct weap_ws_convert_s {
	weapon_t			iWeapon;
	extWeaponStats_t	iWS;
} weap_ws_convert_t;

extWeaponStats_t BG_WeapStatForWeapon( weapon_t iWeaponID );

typedef enum popupMessageType_e {
	PM_DYNAMITE,
	PM_CONSTRUCTION,
	PM_MINES,
	PM_DEATH,
	PM_MESSAGE,
	PM_OBJECTIVE,
	PM_DESTRUCTION,
	PM_TEAM,
	PM_GRAPHICOBITUARY,
	PM_AMMOPICKUP,
	PM_HEALTHPICKUP,	// Jaybird
	PM_WEAPON,
	PM_DOGTAGS,
	PM_NUM_TYPES
} popupMessageType_t;

typedef enum popupMessageBigType_e {
	PM_SKILL,
	PM_RANK,
	PM_DISGUISE,
	PM_BIG_NUM_TYPES
} popupMessageBigType_t;

int PM_AltSwitchFromForWeapon ( int weapon );
int PM_AltSwitchToForWeapon ( int weapon );

void PM_TraceLegs( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end, trace_t *bodytrace, vec3_t viewangles, void (tracefunc)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int ignoreent, int tracemask );
void PM_TraceHead( trace_t *trace, vec3_t start, vec3_t end, trace_t *bodytrace, vec3_t viewangles, void (tracefunc)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int ignoreent, int tracemask );
void PM_TraceAllParts( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end );
void PM_TraceAll( trace_t *trace, vec3_t start, vec3_t end );

// jet Pilot - game rule flags
#define JP_KEEP_FLAKJACKET		0x0001
#define JP_KEEP_RECOGNITION		0x0004
#define JP_KEEP_ASSASSIN		0x0008
#define JP_KEEP_MINES			0x0010	// keep covert ops landmine spotting ability

#define JP_INSANITY_SPAWN			0x0001	// 1		- players spawn instantly upon death
#define JP_INSANITY_AMMUNITION		0x0002	// 2		- players have unlimited ammo clips
#define JP_INSANITY_MORTARCAM		0x0004	// 4		- mortar cam
#define JP_INSANITY_MISSILECAM		0x0008	// 8		- missile cam
#define JP_INSANITY_CHARGEBAR		0x0010	// 16 		- players have unlimited charge bars
#define JP_INSANITY_GUIDED			0x0020	// 32		- rockets can be steered by the firing player
#define JP_INSANITY_HOMING			0x0040	// 64		- rockets are homing
#define JP_INSANITY_DOGTAGS  		0x0080	// 128		- players can pick up dog tags
#define JP_INSANITY_LAUNCH_TK		0x0100	// 256		- launches teamkillers - fun mode for baserace
#define JP_INSANITY_ALLOWBETTING	0x0200	// 512		- enable betting
#define JP_INSANITY_ALLOWBUYING		0x0400	// 1024		- enable buying
#define JP_INSANITY_DYNAKICK		0x0800	// 2048		- dynamites can be kicked
#define JP_INSANITY_KILLERCAM		0x1000	// 4096		- killer cam (in seperate window like mortarcam)
#define JP_INSANITY_RIFLENADECAM	0x2000	// 8192		- riflenade cam
#define JP_INSANITY_SYRINGEGIBS		0x4000	// 16384	- syringe gibs instantly
#define JP_INSANITY_NUM_MODES		15


#define DYNA_KICKDISTANCE 80			// kick distance for dynamites (jp_insanity & JP_INSANITY_DYNAKICK)

// core: bitmask for nq_War
#define WARMODE_ENABLE			0x0001	// 1		- war mode: spawn with a few possible weapons, but plenty ammo..
#define WARMODE_RESERVED		0x0002	// 2		- reserved, not in use..
#define WARMODE_KNIFE			0x0004	// 4		- enable knives
#define WARMODE_PANZER			0x0008	// 8		- enable panzers
#define WARMODE_SNIPER			0x0010	// 16		- enable sniperguns
#define WARMODE_NADE			0x0020	// 32		- enable grenades
#define WARMODE_RIFLE			0x0040	// 64		- enable rifles
#define WARMODE_FLAMER			0x0080	// 128		- enable flamethrowers
#define WARMODE_SHOTGUN			0x0100	// 256		- enable shotguns
#define WARMODE_POISON			0x0200	// 512		- enable poison
#define WARMODE_SMOKE			0x0400	// 1024		- enable smoke grenades
#define WARMODE_BINOCS			0x0800	// 2048		- enable binoculars
#define WARMODE_PLIERS			0x1000	// 4096		- enable pliers + dynamite
#define WARMODE_NUM_MODES		13

// core: bitflags for g_unlockWeapons (per class)..
#define UNLOCK_WEAPONS_NONE			0
#define UNLOCK_WEAPONS_SOLDIER		0x0001
#define UNLOCK_WEAPONS_MEDIC		0x0002
#define UNLOCK_WEAPONS_ENGINEER		0x0004
#define UNLOCK_WEAPONS_FIELDOPS		0x0008
#define UNLOCK_WEAPONS_COVERTOPS	0x0010
#define UNLOCK_WEAPONS_ALL			(UNLOCK_WEAPONS_SOLDIER | UNLOCK_WEAPONS_MEDIC | UNLOCK_WEAPONS_ENGINEER | UNLOCK_WEAPONS_FIELDOPS | UNLOCK_WEAPONS_COVERTOPS)


// bitmask for friendly fire modes
#define FRIENDLY_FIRE_NORMAL	0x0001	// 1  	- FF is enabled
#define FRIENDLY_FIRE_HALF		0x0002	// 2  	- FF does half damage to teammates
#define FRIENDLY_FIRE_MIRROR	0x0004	// 4  	- FF does an equal amount of damage to the inflictor
#define FRIENDLY_FIRE_MOVERS	0x0008	// 8  	- FF affects other ents as well (tanks, etc)
#define FRIENDLY_FIRE_BOOSTING  0x0010	// 16 	- FF allows for nade boosts
#define FRIENDLY_FIRE_NOLANDMINETRIGGER   0x0020  // 32   - Landmines cannot be tripped by teammates
#define FRIENDLY_FIRE_LANDMINESHURTTEAMMATES   0x0040  // 64   - Landmines hurt teammates

#define ANNOUNCE_KILLINGSPREES		0x0001 // 1
#define ANNOUNCE_DEATHSPREES		0x0002 // 2
#define ANNOUNCE_MULTIKILL			0x0004 // 4
#define ANNOUNCE_FIRSTBLOOD			0x0008 // 8
#define ANNOUNCE_FIRSTHEADSHOT		0x0010 // 16	Let NOQ do that!
#define ANNOUNCE_KICKSOUND			0x0020 // 32	Let NOQ do that!
#define ANNOUNCE_BANSOUND			0x0040 // 64	Let NOQ do that!
#define ANNOUNCE_PERS_OBJ_MSG_POPUP	0x0080 // 128	core: mission msgs. with playernames in popup-area (instead of top-area)..
#define ANNOUNCE_PERS_OBJ_DISABLED	0x0100 // 256	core: disable all mission messages
#define ANNOUNCE_HPLEFT				0x0200 // 512   cs: moved from jp_insanity
// #define ANNOUNCE_CONNECT			0x0400 // 1024   Let NOQ do that! sound 'new player' on connect
// #define ANNOUNCE_JOIN_TEAM		0x0800 // 2048  Let NOQ do that! sound for joining a team

// core:
// Create a text+voice announce for killingsprees/multikill/firstblood/firstheadshot
// (used in: g_combat C_TextVoiceAnnounce() & cg_event.c CG_EntityEvent()
#define FLAGS_KILLINGSPREE	0x80
#define FLAGS_MULTIKILL		0x00
#define FLAGS_FIRSTBLOOD	0x40
#define FLAGS_FIRSTHEADSHOT	0xC0


extern float chargeCost_Repair[NUM_SKILL_LEVELS];
extern float chargeCost_Construct[NUM_SKILL_LEVELS];
extern float chargeCost_Landmine[NUM_SKILL_LEVELS];
extern float chargeCost_Dynamite[NUM_SKILL_LEVELS];
extern float chargeCost_Satchel[NUM_SKILL_LEVELS];
extern float chargeCost_Smoke[NUM_SKILL_LEVELS];
extern float chargeCost_Health[NUM_SKILL_LEVELS];
extern float chargeCost_Ammo[NUM_SKILL_LEVELS];
extern float chargeCost_Adrenaline[NUM_SKILL_LEVELS];
extern float chargeCost_Artillery[NUM_SKILL_LEVELS];
extern float chargeCost_Airstrike[NUM_SKILL_LEVELS];
extern float chargeCost_Airstrike_Half[NUM_SKILL_LEVELS];
extern float chargeCost_Panzer[NUM_SKILL_LEVELS];
extern float chargeCost_Bazooka[NUM_SKILL_LEVELS];
extern float chargeCost_Mortar[NUM_SKILL_LEVELS];
extern float chargeCost_Riflenade[NUM_SKILL_LEVELS];
extern float lightWeapon_Spread[NUM_SKILL_LEVELS];
extern float disguiseTestCost[NUM_SKILL_LEVELS];

extern float chargeCost_Defuse[NUM_SKILL_LEVELS];
extern float armSpeed_Dynamite[NUM_SKILL_LEVELS];
extern float disarmSpeed_Dynamite[NUM_SKILL_LEVELS];
extern float armSpeed_LandMine[NUM_SKILL_LEVELS];
extern float disarmSpeed_LandMine[NUM_SKILL_LEVELS];
extern float disarmSpeed_Satchel[NUM_SKILL_LEVELS];
extern float artilleryBombs[NUM_SKILL_LEVELS];

// void BG_IndexedSubstring(char *dest, int maxLen, const char *source, int index);

// end of jP additions

// jaquboss
void BG_RegisterWeapon( int weaponNum, qboolean force );

typedef struct BGweaponInfo_s {
	qboolean	registered;
	int		    animationTime[MAX_WP_ANIMATIONS];

	char		name[22];		// IRATA: we need to limit 22 chars because of Limbo and pickup message in game
	char		statname[12];	// core: string is max. 11 chars + the trailing 0
	int			damage;
	int			spread;

	qboolean	recoil;
	int			recoilDuration;
	float		recoilYaw;
	float		recoilPitch;

	float		CrouchSpreadRatio;
	float		ProneSpreadRatio;

	int			aimSpreadScaleAdd;
	int			aimSpreadScaleAddRand;
	int			weaponIndex;

	qboolean	limboKill;
	qboolean	bulletReflection;
	qboolean	noMCReload;

	float		movementSpeedScale;

	char		selfKillMessage[64];
	char		KillMessage[64];
	char		KillMessage2[64];
	qboolean	headShot;
	qboolean	gib;
	qboolean	falloff;

	int			splashdamage;
	int			splashdamage_radius;

	int			minHSDamage;
	float		HSRatio;

	float		wpnScale;
	qboolean	velocity2spread;
	qboolean	viewchange2spread;

} weaponStatsInfo_t;

weaponStatsInfo_t BG_Weapons[MAX_WEAPONS];
// end

// for charge bar and pmove
qboolean BG_CheckCharge( pmove_t *pm );
float	BG_ChargeUsage( weapon_t weapon, int skill[SK_NUM_SKILLS]);

#define	SPEC_PLAYERGLOW				1
#define	SPEC_SHOWNAMES				2

// Jaybird - seperate shotgun reload states
typedef enum {
	SHOTGUN_NOT_RELOADING,
	SHOTGUN_RELOADING_BEGIN, // jaquboss - Shotgun reloading
	SHOTGUN_RELOADING_BEGIN_1ST,
	SHOTGUN_RELOADING_AFTER_1ST, // loaded first shell and pumped
	SHOTGUN_RELOADING_LOOP,
	SHOTGUN_RELOADING_STOP,
	SHOTGUN_RELOADING_INTERRUPT,
} sg_state_t;

// realism flags
#define REALISM_EXTENDED_PRONE						1		// play transition animations, locked when standing, slow standing
#define	REALISM_NOLADDERGUN							2		// cannot use weapon on ladders
#define	REALISM_COVERT_DISGUISED_HEAD				4		// core: Besides clothes, a covertops also steals the players' head
#define	REALISM_AURA_WITH_PVS						8		// IRATA: activate PVS in aura
#define REALISM_NOLEAN								16		// leaning disabled
#define REALISM_NO_AUTO_WEAPON_CHANGE_IF_POISONED	32		// IRATA: no auto switching for poisoned players
#define REALISM_MOVER_GIBS_ITEMS					64		// core: a script_mover removes items (when driving over them)
#define REALISM_MOVER_ROTATES_PLAYERS				128		// core: players rotate along on a rotating mover
#define REALISM_NO_VIEWCHANGE2SPREAD				256		// core: changing view (look left/right) does not increase spread
#define REALISM_EASY_DOORS							512		// core: opening/closing doors that are already opening/closing..

typedef enum {
	SM_NEED_MEDIC,
	SM_NEED_ENGINEER,
	SM_NEED_LT,
	SM_NEED_COVERTOPS,
	SM_LOST_MEN,
	SM_OBJ_CAPTURED,
	SM_OBJ_LOST,
	SM_OBJ_DESTROYED,
	SM_CON_COMPLETED,
	SM_CON_FAILED,
	SM_CON_DESTROYED,
	SM_NUM_SYS_MSGS,
} sysMsg_t;

typedef struct sysMessage_s {
	char *codeString;
	char *voiceScript;
	char *chatString;
}sysMessage_t;

extern sysMessage_t HQMessages[SM_NUM_SYS_MSGS];

// Store all sounds used in server engine and send them to client in events only as Enums
typedef enum {
    GAMESOUND_PLAYER_GURP1,         // "sound/player/gurp1.wav"                         Player takes damage from drowning
    GAMESOUND_PLAYER_GURP2,         // "sound/player/gurp2.wav"
    GAMESOUND_WPN_AIRSTRIKE_PLANE,  // "sound/weapons/airstrike/airstrike_plane.wav"    Used by Airstrike marker after it triggers
	GAMESOUND_WPN_ARTILLERY_FLY_1,  // "sound/weapons/artillery/artillery_fly_1.wav"    Used by Artillery before impact
	GAMESOUND_WPN_ARTILLERY_FLY_2,  // "sound/weapons/artillery/artillery_fly_2.wav"
    GAMESOUND_WPN_ARTILLERY_FLY_3,  // "sound/weapons/artillery/artillery_fly_3.wav"
    GAMESOUND_WORLD_JUMPPAD,        // "sound/world/jumppad.wav"                        Legacy code
	GAMESOUND_WORLD_BUILD,          // "sound/world/build.wav"                          Used for Pliers when constructing objectives
	GAMESOUND_WORLD_CHAIRCREAK,     // "sound/world/chaircreak.wav"                     Legacy Code
    GAMESOUND_MOOVERS_LOCK,         // "sound/movers/doors/default_door_locked.wav"     Used by doors with playerside restriction
    GAMESOUND_MISC_RESUSCITATE,     // "sound/misc/vo_resuscitate.wav"                  Used by level 9 First Aid skill
	GAMESOUND_MISC_REVIVE,          // "sound/misc/vo_revive.wav"                       Used by revival Needle
    GAMESOUND_MISC_WHACK,           // "sound/jetpilot/whack.wav"                       Used for custom Command
    GAMESOUND_MISC_WINDFLY,         // "sound/misc/windfly.wav"                         Used by 'target_push' trigger
    GAMESOUND_MISC_REFEREE,         // "sound/misc/referee.wav"                         Game Referee performs action
    GAMESOUND_MISC_VOTE,            // "sound/misc/vote.wav"                            Vote is issued
    GAMESOUND_MISC_BANNED,          // "sound/osp/banned.wav"                           Player is banned
    GAMESOUND_MISC_KICKED,          // "sound/osp/kicked.wav"                           Player is kicked
    // Announcers
    GAMESOUND_KILL_MULTI,           // "sound/spree/multikill.wav"
    GAMESOUND_KILL_ULTRA,           // "sound/spree/ultrakill.wav"
    GAMESOUND_KILL_MONSTER,         // "sound/spree/monsterkill.wav"
    GAMESOUND_KILL_MEGA,            // "sound/spree/megakill.wav"
    GAMESOUND_KILL_LUDICROUS,       // "sound/spree/ludicrouskill.wav"
    GAMESOUND_SPREE_KILLINGSPREE,   // "sound/spree/killingspree.wav"
    GAMESOUND_SPREE_RAMPAGE,        // "sound/spree/rampage.wav"
    GAMESOUND_SPREE_DOMINATING,     // "sound/spree/dominating.wav"
    GAMESOUND_SPREE_UNSTOPPABLE,    // "sound/spree/unstoppable.wav"
    GAMESOUND_SPREE_GODLIKE,        // "sound/spree/godlike.wav"
    GAMESOUND_SPREE_WICKEDSICK,     // "sound/spree/wickedsick.wav"
    GAMESOUND_SPREE_POTTER,         // "sound/spree/potter.wav"
    GAMESOUND_SPREE_HOLYCOW,        // "sound/spree/holyshit.wav"
    GAMESOUND_SPREE_HUMILIATION,    // "sound/spree/humiliation.wav"
    // Announcers
    GAMESOUND_ANN_FIRSTBLOOD,       // "sound/spree/firstblood.wav"
    GAMESOUND_ANN_FIRTSHEADSHOT,    // "sound/spree/firstheadshot.wav"
    GAMESOUND_MAX
} gameSounds;

typedef enum {
    GAMEMODEL_WORLD_FLAGPOLE,       // "models/multiplayer/flagpole/flagpole.md3"
    GAMEMODEL_WORLD_DOGTAGS,        // "models/multiplayer/dogtags/dogtags.md3"
    // GAMEMODEL_WORLD_SEARCHLIGHT,    // "models/mapobjects/light/searchlight_pivot.md3"
    // GAMEMODEL_WORLD_ALARMBOX,       // "models/mapobjects/electronics/alarmbox.md3"
    // GAMEMODEL_WORLD_BOX_32,         // "models/mapobjects/boxes/box32.md3"
    // GAMEMODEL_WORLD_BOX_48,         // "models/mapobjects/boxes/box48.md3"
    // GAMEMODEL_WORLD_BOX_64,         // "models/mapobjects/boxes/box64.md3"
    // GAMEMODEL_WORLD_BENCH,          // "models/furniture/bench/bench_sm.md3"
    // GAMEMODEL_WORLD_RADIO,          // "models/mapobjects/electronics/radio1.md3"
    // GAMEMODEL_WORLD_RADIOS,		    // "models/mapobjects/electronics/radios.md3"
    // GAMEMODEL_WORLD_CASTLEBED,	    // "models/furniture/bed/castlebed.md3"
    // GAMEMODEL_WORLD_TABLE_56X112,	// "models/furniture/table/56x112tablew.md3"
    // GAMEMODEL_WORLD_CRATE_32X64,	// "models/furniture/crate/crate32x64.md3"
    // GAMEMODEL_WORLD_WOODFLIP,		// "models/furniture/table/woodflip.md3";
    // GAMEMODEL_WORLD_LOCKER,		    // "models/furniture/storage/lockertall.md3"
    GAMEMODEL_WORLD_CHAIR_OFFICE,	// "models/furniture/chair/chair_office3.md3"
    GAMEMODEL_WORLD_CHAIR_HIBACK,	// "models/furniture/chair/hiback5.md3"
    GAMEMODEL_WORLD_CHAIR_SIDECHAIR,// "models/furniture/chair/sidechair3.md3"
    //GAMEMODEL_WORLD_CHAIR_CHAT,		// "models/furniture/chair/chair_chat.md3"
    //GAMEMODEL_WORLD_CHAIR_CHATARM,	// "models/furniture/chair/chair_chatarm.md3"
    // GAMEMODEL_WORLD_DESKLAMP,	    // "models/furniture/lights/desklamp.md3"
    // GAMEMODEL_WORLD_BARREL_C,	    // "models/furniture/barrel/barrel_c.md3"
    // GAMEMODEL_WORLD_BARREL_D,	    // "models/furniture/barrel/barrel_d.md3"
    // GAMEMODEL_WORLD_BARREL_B,	    // "models/furniture/barrel/barrel_b.md3"
    // GAMEMODEL_WORLD_CRATE_64,	    // "models/furniture/crate/crate64.md3"
    // GAMEMODEL_WORLD_CRATE_32,	    // "models/furniture/crate/crate32.md3"
    // GAMEMODEL_WORLD_FOOTLOCKER,     // "models/mapobjects/furniture/footlocker.md3"
    GAMEMODEL_MISC_ROCKET,		    // "models/ammo/rocket/rocket.md3"
    GAMEMODEL_WPN_MG42,             // "models/multiplayer/mg42/mg42.md3"
    GAMEMODEL_WPN_MG42_B,           // "models/mapobjects/weapons/mg42b.md3"

    GAMEMODEL_MAX
} gameModels;

#define ADF_NOSPAWN			1		// Spawn with no adrenaline needles, can only get them from ammo packs
#define ADF_NOPACK			2		// Ammo packs do not contain adrenaline needles
#define ADF_JUSTONE			4		// Can only carry one needle at a time
#define ADF_NOHEALTH		8		// Adrenaline does not boost health
#define ADF_NOSTAMINA		16		// Adrenaline does not boost stamina
#define ADF_EYEGLOW			32		// Adrenaline causes player's eyes to glow red when used
#define ADF_NOHPICK			64		// Disable adrenaline pickup (cabinet etc)
#define ADF_SOLDIER			128		// Enables adrenaline for soldiers
#define ADF_MEDIC			256		// Enables adrenaline for medics
#define ADF_ENGINEER		512		// Enables adrenaline for engineers
#define ADF_FIELDOPS		1024	// Enables adrenaline for field ops
#define ADF_COVERTOPS		2048	// Enables adrenaline for covert ops
#define ADF_HALFDAMAGE		4096	// While using adrenaline you receive only half the damage !!!


#define HITBOXBIT_HEAD 1024
#define HITBOXBIT_LEGS 2048
#define HITBOXBIT_CLIENT 4096

// can we use this skill?
float BG_GetFromTable( float* table, int *skillBits, int skillType );

// matt: g_medics flags
#define MEDIC_NOSELFPACKPOISON		1		// Medics can't pick up their own med packs to heal poisoning
#define MEDIC_NOSELFPACK			2		// Medics can't pick up their own med packs at all
#define MEDIC_REGENRATE21			4		// Medics regenerate at 2/1 instead of 3/2
#define MEDIC_REGENRATE10			8		// Medics regenerate at 1/0 instead of 1/0
#define MEDIC_NOAKIMBO				16		// Medics can't get akimbos
#define MEDIC_PISTOLONLY			32		// Medics can't carry SMG, nor pick one up
#define MEDIC_SYRINGEHEAL			64		// Alternative to tk/revive for medics
#define MEDIC_EXTRAHP				128		// Medics get 12% extra HP
#define MEDIC_NOEXTRATEAMHP			256		// More Medics in a team do NOT make the team get higher maxHP
#define MEDIC_NOREGENERATION		512		// Medics do not at all regenerate
#define MEDIC_MIRRORHEAL			1024	// Medics get HP if (non-medic)teammates pickup their healthpack(s)
#define MEDIC_ALLMAXHP				2048	// The maxHP of all players is independent of # medics in a team,
											//   all players have the 25 extra HP.
#define MEDIC_FASTRESUSCITATIONING	4096    // Fast medic resuscitationing - was g_fastres before/no need for an extra medic behaviour var

// core:
#define NUM_ENDGAME_AWARDS			26		// total number of endgame Awards
#define NUMSHOW_ENDGAME_AWARDS		14		// number of awards to display (that will fit on screen)


// core: bitflags for cvar team_maxIsPercentage
// Set team_maxX restrictions as a percentage or as an absolute value (players per team)
#define TEAM_MAXWEAPONS_PERCENT		1
#define TEAM_MAXCLASSES_PERCENT		2


// core: defines for viewlocking (mg/medics/killercam etc)..
#define VIEWLOCK_NONE				0	// disabled, let them look around
#define VIEWLOCK_JITTER				2	// this enable screen jitter when firing
#define VIEWLOCK_MG42				3	// tell the client to lock the view in the direction of the gun
#define VIEWLOCK_MEDIC				7	// look at the nearest medic
#define VIEWLOCK_KILLER				8	// look at the latest killer


// core: The dog tags are items of giType IT_WEAPON, and of giTag -1
// (see bg_itemlist[] in bg_misc.c)
// The following define is for code readability..
#define ITEM_DOGTAGS				-1

#define PACKET_OFFSET 5 // to store message packet info etc

// core: packet types when using trap_SendMessage() on client or server
// IRATA: unique packet idents for client AND server so we always know the type - _<sender>_
#define PACKET_S_TEAMPLAYINFOMESSAGE	0x01 // server sends teamplay info
// WIP: new packet types
#define PACKET_C_SEND_NQKEY				0x02 // client sends the NQ key
#define PACKET_S_RECEIVED_NQKEY			0x04 // server accepts client, NQKEY is available on server
#define PACKET_S_REQUEST_CLIENT_INFO	0x08 // NQ AUTH SYSTEM: server requests additional client infos see PACKET_C_SENDS_CLIENT_INFO
#define PACKET_C_SENDS_CLIENT_INFO		0x10 // NQ AUTH SYSTEM: there are several self compiled clients outa there ... let's check build date, engine related vars etc
											 // case1: client doesn't send info back - 2.55 client? (drop)
											 // case2: we don't want this client - let the admins finally decide which clients are valid (drop)
											 // case3: stay connected


// core: hash values for various commands..
#define LUA_STATUS_HASH		135068
#define GETGS_HASH			65124
#define BINDSEARCH_R_HASH	155890
#define SAY_HASH			39966
#define M_HASH				12971
#define PM_HASH				26408
#define DAMAGE_HASH			73756
#define SAY_TEAM_HASH		104207
#define VSAY_HASH			54341
#define VSAY_TEAM_HASH		119100
#define SAY_BUDDY_HASH		118585
#define VSAY_BUDDY_HASH		133591
#define SCORE_HASH			65327
#define VOTE_HASH			53720
#define FIRETEAM_HASH		103505
#define SHOWSTATS_HASH		123989
#define RCONAUTH_HASH		106340
#define IGNORE_HASH			78253
#define UNIGNORE_HASH		106664
#define OBJ_HASH			37795
#define IMPKD_HASH			64481
#define IMWA_HASH			51808
#define IMWS_HASH			54004
#define IMREADY_HASH		91147
#define WS_HASH				27961
#define MAPVOTE_HASH		93221
#define IMMAPLIST_HASH		120113
#define FORCETAPOUT_HASH	148411
#define WSTATS_HASH			82359
#define SGSTATS_HASH		94802
#define STSHOTS_HASH		96620
#define RS_HASH				27366
#define FOLLOWNEXT_HASH		136675
#define FOLLOWPREV_HASH		136406
#define SETSPAWNPT_HASH		137482
#define KILL_HASH			51577
#define PLAYDEAD_HASH		102336
#define DROPWEAPON_HASH		134250
#define DROPCLIP_HASH		105481
#define GIVE_HASH			51457
#define GOD_HASH			37677
#define NOFATIGUE_HASH		118318
#define NOTARGET_HASH		106315
#define NOCLIP_HASH			78368
#define WHERE_HASH			65193
#define SETVIEWPOS_HASH		137480
#define SHRUG_HASH			66902
#define MYCREDITS_HASH		120547
#define BET_HASH			37818
#define ACCEPT_HASH			75884
#define BUY_HASH			40343
//
#define TINFO_HASH			65811
#define SC0_HASH			31373
#define SC1_HASH			31494
#define WEAPONSTATS_HASH	149968
#define CPM_HASH			38410
#define CPM_MAP_HASH		89435
#define ANNOUNCE_HASH		104727
#define ANNOUNCER_HASH		119205
#define CP_HASH				25221
#define REQFORCESPAWN_HASH	176027
#define SDBG_HASH			50109
#define CS_HASH				25581
#define PRINT_HASH			67401
#define ENTNFO_HASH			78985
#define ENTNFO2_HASH		85235
#define CHAT_HASH			50150
#define VSCHAT_HASH			78824
#define TCHAT_HASH			64370
#define GAMECHAT_HASH		101222
#define VCHAT_HASH			64608
#define VTCHAT_HASH			78944
#define VBCHAT_HASH			76784
#define COMPLAINT_HASH		118983
#define MAP_RESTART_HASH	147165
#define SC_HASH				25565
#define WWS_HASH			42356
#define GSTATS_HASH			80455
#define ASTATS_HASH			79741
#define ASTATSB_HASH		91991
#define BSTATS_HASH			79860
#define BSTATSB_HASH		92110
#define WBSTATS_HASH		94678
#define RWS_HASH			41761
#define PORTALCAMPOS_HASH	161962
#define ROCKANDROLL_HASH	146207
#define APPLICATION_HASH	145376
#define INVITATION_HASH		134986
#define PROPOSITION_HASH	151490
#define AFT_HASH			37819
#define AFTC_HASH			49897
#define AFTJ_HASH			50751
#define REMAPSHADER_HASH	144301
#define MU_START_HASH		107698
#define MU_PLAY_HASH		92607
#define MU_STOP_HASH		94568
#define MU_FADE_HASH		87906
#define SND_FADE_HASH		100375
#define ADDTOBUILD_HASH		129971
#define SPAWNSERVER_HASH	150779
#define GS_HASH				26057
#define BINDSEARCH_C_HASH	153940
#define IMVOTETALLY_HASH	150058
#define RESTRICT_HASH		107786
//
#define TEAM_CTF_REDFLAG_HASH			209279	//BG_StringHashValue( "team_CTF_redflag" );
#define TEAM_CTF_BLUEFLAG_HASH			223985	//BG_StringHashValue( "team_CTF_blueflag" );
#define PLAYER_HASH						79346	//BG_StringHashValue( "player" );
#define DYNAMITE_HASH					105217	//BG_StringHashValue( "dynamite" );
#define LANDMINE_HASH					102905	//BG_StringHashValue( "landmine" );
#define SATCHEL_CHARGE_HASH				182268	//BG_StringHashValue( "satchel_charge" );
#define SMOKE_BOMB_HASH					130076	//BG_StringHashValue( "smoke_bomb" );
#define AIR_STRIKE_HASH					124367	//BG_StringHashValue( "air strike" );
#define CORPSE_HASH						79228	//BG_StringHashValue( "corpse" );
#define GRENADE_HASH					88534	//BG_StringHashValue( "grenade" );
#define ROCKET_HASH						78726	//BG_StringHashValue( "rocket" );
#define MORTAR_GRENADE_HASH				185795	//BG_StringHashValue( "mortar_grenade" );
#define FLAMECHUNK_HASH					130240	//BG_StringHashValue( "flamechunk" );
#define GPG40_GRENADE_HASH				155078	//BG_StringHashValue( "gpg40_grenade" );
#define M7_GRENADE_HASH					121778	//BG_StringHashValue( "m7_grenade" );
#define DPRINTF_HASH					104578	//BG_StringHashValue( "DPRINTF_" );
// entities
#define TRIGGER_OBJECTIVE_INFO_HASH		301509
#define SCRIPT_MOVER_HASH				162988	//BG_StringHashValue( "script_mover" );
#define FUNC_CONSTRUCTIBLE_HASH			246325
#define TEAM_CTF_REDPLAYER_HASH			242140
#define TEAM_CTF_BLUEPLAYER_HASH		257089
#define TEAM_CTF_REDSPAWN_HASH			228504	//BG_StringHashValue( "team_CTF_redspawn" );
#define TEAM_CTF_BLUESPAWN_HASH			243353	//BG_StringHashValue( "team_CTF_bluespawn" );
#define TEAM_WOLF_OBJECTIVE_HASH		257045	//BG_StringHashValue( "team_WOLF_objective" );
#define TEAM_WOLF_CHECKPOINT_HASH		272169	//BG_StringHashValue( "team_WOLF_checkpoint" );
#define TRIGGER_FLAGONLY_HASH			216488
#define TRIGGER_FLAGONLY_MULTIPLE_HASH	351465
#define MISC_COMMANDMAP_MARKER_HASH		299582
#define MISC_CONSTRUCTIBLEMARKER_HASH	335897
#define MISC_GAMEMODEL_HASH				183465
#define INFO_TRAIN_SPLINE_MAIN_HASH		301242
#define INFO_TRAIN_SPLINE_CONTROL_HASH	350619
#define PATH_CORNER_HASH				145496
#define PATH_CORNER_2_HASH				164396
#define INFO_LIMBO_CAMERA_HASH			224193
#define TARGET_SPEAKER_HASH				186830
#define TRIGGER_HEAL_HASH				156857
#define TRIGGER_AMMO_HASH				158930
#define MISC_CABINET_HEALTH_HASH		252730	//BG_StringHashValue( "misc_cabinet_health" );
#define MISC_CABINET_SUPPLY_HASH		260101	//BG_StringHashValue( "misc_cabinet_supply" );
#define FUNC_DOOR_ROTATING_HASH			245642	//BG_StringHashValue( "func_door_rotating" );
#define FUNC_TIMER_HASH					131913
#define FUNC_INVISIBLE_USER_HASH		259901	//BG_StringHashValue( "func_invisible_user" );
#define FUNC_EXPLOSIVE_HASH				190086
#define FUNC_DOOR_HASH					117990	//BG_StringHashValue( "func_door" );
#define FUNC_STATIC_HASH				145176
#define FUNC_BOBBING_HASH				155092
#define TRIGGER_ALWAYS_HASH				189303
#define TRIGGER_MULTIPLE_HASH			218436
#define TRIGGER_PUSH_HASH				161723
#define TRIGGER_TELEPORT_HASH			218920
#define TRIGGER_HURT_HASH				162138
#define TRIGGER_CONCUSSIVE_DUST_HASH	323081
#define TRIGGER_ONCE_HASH				158246
#define MISC_MG42_HASH					102542	//BG_StringHashValue( "misc_mg42" );
#define TARGET_SMOKE_HASH				159960
#define CORONA_HASH						77995
#define DLIGHT_HASH						77307
#define SHOOTER_ROCKET_HASH				190060
#define SHOOTER_GRENADE_HASH			200492
#define SHOOTER_MORTAR_HASH				191742
#define INFO_PLAYER_START_HASH			232439
#define INFO_PLAYER_CHECKPOINT_HASH		302525
#define INFO_PLAYER_DEATHMATCH_HASH		299584	//BG_StringHashValue( "info_player_deathmatch" );
#define INFO_PLAYER_INTERMISSION_HASH	337887	//BG_StringHashValue( "info_player_intermission" );
#define INFO_NULL_HASH					118853
#define INFO_NOTNULL_HASH				162313
#define INFO_NOTNULL_BIG_HASH			215461
#define TARGET_EXPLOSION_HASH			219578
#define SCRIPT_MULTIPLAYER_HASH			250417
#define FUNC_DEBRIS_HASH				143377
#define FUNC_PLAT_HASH					117593
#define FUNC_BUTTON_HASH				147774	//BG_StringHashValue( "func_button" );
#define FUNC_ROTATING_HASH				174383
#define FUNC_PENDULUM_HASH				174721
#define FUNC_TRAIN_HASH					131522
#define FUNC_TRAIN_ROTATING_HASH		260141
#define MISC_LANDMINE_HASH				170354
#define TARGET_DELAY_HASH				157980
#define TARGET_EFFECT_HASH				170317
#define TARGET_SCRIPT_TRIGGER_HASH		290764
#define TARGET_PUSH_HASH				147593
#define TARGET_GIVE_HASH				144932
#define TARGET_REMOVE_POWERUPS_HASH		310062
#define TARGET_PRINT_HASH				161786
#define TARGET_LASER_HASH				158982
#define TARGET_SCORE_HASH				159593
#define TARGET_TELEPORTER_HASH			233283
#define TARGET_RELAY_HASH				159744
#define TARGET_KILL_HASH				145059
#define TARGET_POSITION_HASH			205077
#define TARGET_LOCATION_HASH			201493
#define TARGET_ALARM_HASH				157716
#define TARGET_COUNTER_HASH				189582
#define TARGET_LOCK_HASH				144666
#define TARGET_FOG_HASH					130619
#define TARGET_AUTOSAVE_HASH			203400
#define SCRIPT_MOVER_MED_HASH			213570
#define FUNC_BRUSHMODEL_HASH			201599
#define MISC_BEAM_HASH					114091
#define FUNC_FAKEBRUSH_HASH				185586
#define TRIGGER_REMOVEPROTECTION_HASH	339382
#define INFO_CAMP_HASH					115623
#define TARGET_RUMBLE_HASH				173574
#define LIGHT_HASH						64871
#define LIGHTJUNIOR_HASH				148749
#define MISC_TELEPORTER_DEST_HASH		275587
#define MISC_MODEL_HASH					129891
#define MISC_PORTAL_SURFACE_HASH		258541
#define MISC_PORTAL_CAMERA_HASH			241192
#define MISC_VIS_DUMMY_HASH				189875
#define MIC_VIS_DUMMY_MULTIPLE_HASH		322910
#define MISC_LIGHT_SURFACE_HASH			242045
#define MISC_FIRETRAILS_HASH			201683
#define MISC_SPAWNER_HASH				160773
#define PROPS_SMOKEDUST_HASH			207894
#define PROPS_DUST_HASH					136723
#define PROPS_SPARKS_HASH				164179
#define PROPS_GUNSPARKS_HASH			207746
#define PROPS_BENCH_HASH				145062
#define PROPS_RADIO_HASH				146959
#define PROPS_CHAIR_HASH				145972
#define PROPS_CHAIR_HIBACK_HASH			239755
#define PROPS_CHAIR_SIDE_HASH			214081
#define PROPS_CHAIR_CHAT_HASH			213464
#define PROPS_CHAIR_CHATARM_HASH		256996
#define PROPS_DAMAGEINFLICTOR_HASH		288434
#define PROPS_LOCKER_TALL_HASH			231342
#define PROPS_DESKLAMP_HASH				189160
#define PROPS_FLAMEBARREL_HASH			229457	//BG_StringHashValue( "props_flamebarrel" );
#define PROPS_CRATE_64_HASH				173251
#define PROPS_FLIPPY_TABLE_HASH			246341
#define PROPS_CRATE_32_HASH				172594
#define PROPS_CRATE_32X64_HASH			202810
#define PROPS_58X112TABLEW_HASH			213367
#define PROPS_RADIOSEVEN_HASH			218889
#define PROPS_SNOWGENERATOR_HASH		266285
#define PROPS_DECORATION_HASH			217910
#define PROPS_DECORBRUSH_HASH			219090
#define PROPS_STATUE_HASH				164409
#define PROPS_STATUEBRUSH_HASH			237306
#define PROPS_SKYPORTAL_HASH			209091
#define PROPS_FOOTLOCKER_HASH			219873	//BG_StringHashValue( "props_footlocker" );
#define PROPS_FLAMETHROWER_HASH			249286
#define PROPS_DECORATION_SCALE_HASH		302476
#define PROPS_BOX_32_HASH				146743
#define PROPS_BOX_48_HASH				147652
#define PROPS_BOX_64_HASH				147390
#define ALARM_BOX_HASH					116811
#define TEST_GAS_HASH					105063
#define FUNC_LEAKY_HASH					130567	//BG_StringHashValue( "func_leaky" );
#define FUNC_GROUP_HASH					133454
// script actions
#define GOTOMARKER_HASH						133741
#define PLAYSOUND_HASH						121881
#define PLAYANIM_HASH						105217
#define WAIT_HASH							52658
#define TRIGGER_HASH						92198
#define ALERTENTITY_HASH					149582
#define TOGGLESPEAKER_HASH					173558
#define DISABLESPEAKER_HASH					184664
#define ENABLESPEAKER_HASH					170317
#define ACCUM_HASH							63083
#define GLOBALACCUM_HASH					142136
#define PRINT_HASH							67401
#define FACEANGLES_HASH						127680
#define RESETSCRIPT_HASH					149825
#define ATTACHTOTAG_HASH					145327
#define HALT_HASH							51236
#define STOPSOUND_HASH						123794
#define ENTITYSCRIPTNAME_HASH				220877
#define WM_AXIS_RESPAWNTIME_HASH			262921
#define WM_ALLIED_RESPAWNTIME_HASH			288581
#define WM_NUMBER_OF_OBJECTIVES_HASH		317829
#define WM_SETWINNER_HASH					163577
#define WM_SET_DEFENDING_TEAM_HASH			283627
#define WM_ANNOUNCE_HASH					146028
#define WM_TEAMVOICEANNOUNCE_HASH			274257
#define WM_ADDTEAMVOICEANNOUNCE_HASH		316227
#define WM_REMOVETEAMVOICEANNOUNCE_HASH		366546
#define WM_ANNOUNCE_ICON_HASH				214704
#define WM_ENDROUND_HASH					147063
#define WM_SET_ROUND_TIMELIMIT_HASH			306988
#define WM_VOICEANNOUNCE_HASH				216473
#define WM_OBJECTIVE_STATUS_HASH			262477
#define WM_SET_MAIN_OBJECTIVE_HASH			286439
#define	REMOVE_HASH							79455
#define SETSTATE_HASH						107393
#define FOLLOWSPLINE_HASH					163074
#define FOLLOWPATH_HASH						134377
#define ABORTMOVE_HASH						119996
#define SETSPEED_HASH						105396
#define SETROTATION_HASH					150260
#define STOPROTATION_HASH					165999
#define STARTANIMATION_HASH					190460
#define ATTATCHTOTRAIN_HASH					190029
#define FREEZEANIMATION_HASH				201793
#define UNFREEZEANIMATION_HASH				232118
#define REMAPSHADER_HASH					144301
#define REMAPSHADERFLUSH_HASH				216384
#define CHANGEMODEL_HASH					141782
#define SETCHARGETIMEFACTOR_HASH			258593
#define SETDAMAGABLE_HASH					154516
#define REPAIRMG42_HASH						117730
#define SETHQSTATUS_HASH					151953
#define PRINTACCUM_HASH						133089
#define PRINTGLOBALACCUM_HASH				215267
#define CVAR_HASH							51586
#define ABORTIFWARMUP_HASH					176528
#define ABORTIFNOTSINGLEPLAYER_HASH			307668
#define SETDEBUGLEVEL_HASH					173363
#define SETPOSITION_HASH					150892
#define SETAUTOSPAWN_HASH					165106
#define SETMODELFROMBRUSHMODEL_HASH			307347
#define FADEALLSOUNDS_HASH					172841
#define MU_START_HASH						107698
#define MU_PLAY_HASH						92607
#define MU_STOP_HASH						94568
#define MU_QUEUE_HASH						106558
#define MU_FADE_HASH						87906
#define SETAASSTATE_HASH					147053
#define CONSTRUCT_HASH						122676
#define SPAWNRUBBLE_HASH					147318
#define SETGLOBALFOG_HASH					158408
#define ALLOWTANKEXIT_HASH					176962
#define ALLOWTANKENTER_HASH					190185
#define SETTANKAMMO_HASH					147275
#define ADDTANKAMMO_HASH					143077
#define KILL_HASH							51577
#define DISABLEMESSAGE_HASH					183871
#define SET_HASH							39841
#define CONSTRUCTIBLE_CLASS_HASH			260698
#define CONSTRUCTIBLE_CHARGEBARREQ_HASH		362499
#define CONSTRUCTIBLE_CONSTRUCTXPBONUS_HASH	438745
#define CONSTRUCTIBLE_DESTRUCTXPBONUS_HASH	421228
#define CONSTRUCTIBLE_HEALTH_HASH			273962
#define CONSTRUCTIBLE_WEAPONCLASS_HASH		351977
#define CONSTRUCTIBLE_DURATION_HASH			307340
#define CREATE_HASH							76308
#define DELETE_HASH							76202
// script events
#define SPAWN_HASH							66910
#define PAIN_HASH							51093
#define DEATH_HASH							62701
#define ACTIVATE_HASH						104037
#define STOPCAM_HASH						92530
#define PLAYERSTART_HASH					150212
#define BUILT_HASH							65851
#define BUILDSTART_HASH						134191
#define DECAYED_HASH						87740
#define DESTROYED_HASH						120424
#define REBIRTH_HASH						91760
#define FAILED_HASH							74482
#define DYNAMITED_HASH						117917
#define DEFUSED_HASH						89805
#define MG42_HASH							37723
#define MESSAGE_HASH						90364
#define EXPLODED_HASH						104425
// scriptaction accum
#define INC_HASH								37674
#define ABORT_IF_LESS_THAN_HASH					241559
#define ABORT_IF_GREATER_THAN_HASH				284005
#define ABORT_IF_NOT_EQUAL_HASH					242478
#define ABORT_IF_NOT_EQUALS_HASH				258233
#define ABORT_IF_EQUAL_HASH						184410
#define ABORT_IF_EQUALS_HASH					199705
#define BITSET_HASH								79135
#define BITRESET_HASH							106130
#define ABORT_IF_BITSET_HASH					199726
#define ABORT_IF_NOT_BITSET_HASH				258254
#define RANDOM_HASH								77885
#define TRIGGER_IF_EQUAL_HASH					213564
#define WAIT_WHILE_EQUAL_HASH					214978
#define SET_TO_DYNAMITECOUNT_HASH				277809
#define DEC_HASH								35999
#define MUL_HASH								40079
#define DIV_HASH								38778
#define INC_ACCUM_HASH							114431
#define ABORT_IF_LESS_THAN_ACCUM_HASH			327556
#define ABORT_IF_GREATER_THAN_ACCUM_HASH		371850
#define ABORT_IF_NOT_EQUAL_ACCUM_HASH			328475
#define ABORT_IF_NOT_EQUALS_ACCUM_HASH			344846
#define ABORT_IF_EQUAL_ACCUM_HASH				267943
#define SET_ACCUM_HASH							116598
#define RANDOM_ACCUM_HASH						156490
#define DEC_ACCUM_HASH							112756
#define MUL_ACCUM_HASH							116836
#define DIV_ACCUM_HASH							115535
#define SET_TO_PLAYINGCLIENTS_HASH				291460
#define INC_GLOBALACCUM_HASH					195984
#define ABORT_IF_LESS_THAN_GLOBALACCUM_HASH		418484
#define ABORT_IF_GREATER_THAN_GLOBALACCUM_HASH	464653
#define ABORT_IF_NOT_EQUAL_GLOBALACCUM_HASH		419403
#define ABORT_IF_NOT_EQUALS_GLOBALACCUM_HASH	436399
#define ABORT_IF_EQUAL_GLOBALACCUM_HASH			356371
#define SET_GLOBALACCUM_HASH					198151
#define RANDOM_GLOBALACCUM_HASH					239918
#define DEC_GLOBALACCUM_HASH					194309
#define MUL_GLOBALACCUM_HASH					198389
#define DIV_GLOBALACCUM_HASH					197088
// script trigger
#define SELF_HASH								51317
#define GLOBAL_HASH								75927
#define PLAYER_HASH								79346
#define ACTIVATOR_HASH							119775


// core: weapon restrictions records..
// Both client and server use these.
// These records are read from file at initgame time.
typedef struct {
	qboolean	restricted;		// A Weapon is definately not avialable == true if there is some kind of restriction for the weapon.
	qboolean	disabled;		// ==true if the weapon is so much restricted that in effect it's disabled.
	int			minplayers;		// The weapon is only available if there are at least 'minplayers' players.
	int			maxweap;		// A maximum of 'maxweap' weapons is available.
	int			percentage;		// The weapon is available for 'percentage' percent of the players.
	qboolean	r_minplayers;	// ==true if the weapon is restricted on minplayers
	qboolean	r_maxweap;		// ==true if the weapon is restricted on maxweap
	qboolean	r_percentage;	// ==true if the weapon is restricted on percentage
} weapon_restriction_t;

weapon_restriction_t weapon_restrictions[WP_NUM_WEAPONS];

//cs: weapon defaults
#define WPN_DEFAULT_MAX_LANDMINES	20


#endif
