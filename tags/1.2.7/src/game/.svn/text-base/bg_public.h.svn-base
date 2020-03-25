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

#define NQ_VERSION "1.2.7" // compatibility reasons with Omni-Bots...
#define NOQUARTER

#define SPRINTTIME					20000.0f
#define ADRENALINE_TIME				10000.0f	// duration for which injected adrenaline is active..
#define	DEFAULT_GRAVITY				800
#define FORCE_LIMBO_HEALTH			-75 // JPW NERVE
#define	GIB_HEALTH					-175 // JPW NERVE
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

// NOTE: use this value, and THEN the cl_input.c scales to tweak the feel
#define	MG42_IDLEYAWSPEED		80.0	// degrees per second (while returning to base)
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

#define MAX_GAMETYPES				16

typedef struct {
	const char *mapName;
	const char *mapLoadName;
	const char *imageName;

	int typeBits;
	int cinematic;

	int teamMembers;
	int timeToBeat[MAX_GAMETYPES];

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
#define MAX_CAMPAIGNS			512

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
qboolean BG_StoreCampaignSave( const char *filename, cpsFile_t *file, const char *profile );

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
	PM_SPECTATOR,		// still run into walls
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

	int			dtmove;	// doubletap move

	int			dodgeTime;
	int			proneTime;			// time a go-prone or stop-prone move starts, to sync the animation to

	int			proneRaiseTime;		// jaquboss - so 1st person effect can match animation
	int			playDeadRaiseTime;

	int			proneGroundTime;	// time a prone player last had ground under him
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

	int			poisonerEnt;
	int			poisonHurt;
	int			poisonAmnt;

	int			shoveTime;

	qboolean	disoriented;
	qboolean	wasDisoriented;

	// jet Pilot - new stats to track

	int			sfxFlags;			// (ie, view is warped as if underwater)

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

	int			skillBits[7];
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
#define MAX_WEAPS_IN_BANK_MP	21	// jP - raised max weapons in bank
#define MAX_WEAP_BANKS_MP		10
// jpw

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
    STAT_PS_FLAGS,
	STAT_SPRINTTIME					// core: sprinttime for CG_DrawStaminaBar()
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
	PERS_RESPAWNS_LEFT,				// DHM - Nerve :: number of remaining respawns
	PERS_RESPAWNS_PENALTY,			// how many respawns you have to sit through before respawning again

	PERS_REVIVE_COUNT,
	// jet Pilot - track these stats
	PERS_AWARDS,					// unused.. (but do not delete!)
	PERS_AWARDS2,					// (0-255 each) Best Spree, Bounty

	// Rafael - mg42				// (SA) I don't understand these here.  can someone explain?
	PERS_HWEAPON_USE,
	PERS_KILLSPREE,					// killing spree as a nubmer
	PERS_KILLS,						// total kills as a number
} persEnum_t;

// jet Pilot - hijack these, convert to flags
#define AWARDMASK_BEST_SPREE	0x000000FF	// 0-255
#define AWARDMASK_BOUNTY		0x0000FF00  // 0-255 << 8
#define AWARDSHIFT_BEST_SPREE	0
#define AWARDSHIFT_BOUNTY		8

int BG_GetStatBounty(playerState_t *ps);
int BG_GetKillSpree(playerState_t *ps);
int	BG_GetStatBestSpree(playerState_t *ps);

int BG_SetStatBounty(playerState_t *ps, int value);
int	BG_SetKillSpree(playerState_t *ps, int value);
int	BG_SetStatBestSpree(playerState_t *ps, int value);

int BG_AdjustBounty(playerState_t *ps, int value);
int	BG_AdjustKillSpree(playerState_t *ps, int value);
int BG_AdjustBestSpree(playerState_t *ps, int value);

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
#define EF_PLAYDEAD			0x00800000		// tjw: player is playing dead
// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000
#define	EF_SPARE2			0x02000000		// Gordon: freed
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
	WP_STEN,				// 10 - British Sten MKII
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
	WP_STEN_MKII,			// 54 - Sten MkII SMG (unsilenced)
	WP_BAZOOKA,				// 55 - M1 Bazooka Anti-Tank
	WP_MP34,				// 56 - Solothurn S1-100 SMG
	WP_MORTAR2,				// 57 - Axis mortar
	WP_MORTAR2_SET,			// 58 - deployed Axis mortar
	WP_VENOM,				// 59 - Venom machinegun
	WP_POISON_SYRINGE,		// 60 - poisoned syringe
	WP_NUM_WEAPONS			// 61 - NOTE: this cannot be larger than 64 for AI/player weapons!

} weapon_t;

#define WPF_NO_L0_FOPS_BINOCS			1
#define WPF_UNDERWATER_SYRINGE			2
#define WPF_UNDERWATER_PLIERS			4
#define WPF_TM_AIRSTRIKE_RESTORE_FULL	8
#define WPF_TM_AIRSTRIKE_RESTORE_HALF	16
#define WPF_AMMO_RESTORES_HELMET		32


// JPW NERVE moved from cg_weapons (now used in g_active) for drop command, actual array in bg_misc.c
extern int weapBanksMultiPlayer[MAX_WEAP_BANKS_MP][MAX_WEAPS_IN_BANK_MP];
// jpw

// TAT 10/4/2002
//		Using one unified list for which weapons can received ammo
//		This is used both by the ammo pack code and by the bot code to determine if reloads are needed
extern int reloadableWeapons[];

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

// jet Pilot - Skill Name Shortcuts

#define SK_SOLDIER		SK_HEAVY_WEAPONS
#define SK_MEDIC		SK_FIRST_AID
#define SK_ENGINEER		SK_EXPLOSIVES_AND_CONSTRUCTION
#define SK_FIELDOPS		SK_SIGNALS
#define SK_COVERTOPS	SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS

// end of jP additions

extern const char* skillNames[SK_NUM_SKILLS];
extern const char* skillNamesLine1[SK_NUM_SKILLS];
extern const char* skillNamesLine2[SK_NUM_SKILLS];
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
	qboolean	isFastSwitch;
	qboolean	isHeavy;
//----
} weapontable_t;

// Lookup table to find ammo table entry
extern weapontable_t *GetWeaponTableData(int ammoIndex);

extern int weapAlts[];	// defined in bg_misc.c
extern int weapTeamConversion[];
#define WP_NUM_CAN_DISABLE_WEAPONS 33
extern int weaponCanDisable[WP_NUM_CAN_DISABLE_WEAPONS];
extern qboolean weaponsCanDisable[WP_NUM_WEAPONS];


void BG_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce );

// TTimo
// NOTE: what about WP_VENOM and other XP weapons?
// rain - #81 - added added akimbo weapons and deployed MG42
#define IS_AUTORELOAD_WEAPON(w) \
	(	w == WP_LUGER				|| w == WP_COLT					|| \
		w == WP_SILENCER			|| w == WP_SILENCED_COLT		|| \
		w == WP_AKIMBO_LUGER		|| w == WP_AKIMBO_COLT			|| \
		w == WP_AKIMBO_SILENCEDLUGER|| w == WP_AKIMBO_SILENCEDCOLT	|| \
		w == WP_STEN				|| w == WP_STEN_MKII			|| \
		w == WP_MP40				|| w == WP_THOMPSON				|| \
		w == WP_STG44				|| w == WP_VENOM				|| \
		w == WP_KAR98				|| w == WP_K43_SCOPE			|| \
		w == WP_K43					|| w == WP_CARBINE				|| \
		w == WP_GARAND_SCOPE		|| w == WP_GARAND				|| \
		w == WP_FG42				|| w == WP_FG42SCOPE			|| \
		w == WP_BAR					|| w == WP_BAR_SET				|| \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_MG42_SET		|| \
		w == WP_MOBILE_BROWNING		|| w == WP_MOBILE_BROWNING_SET	|| \
		w == WP_SHOTGUN				|| w == WP_MP34 )

// jet Pilot - thought this may be a good idea, as there are gobs of checks for this shit

#define IS_FASTRELOAD_WEAPON(w) \
	(	w == WP_LUGER				|| w == WP_COLT				|| \
		w == WP_MP40				|| w == WP_THOMPSON			|| \
		w == WP_STEN				|| w == WP_FG42				|| \
		w == WP_SILENCER			|| w == WP_SILENCED_COLT	|| \
		w == WP_SHOTGUN				|| w == WP_STG44			|| \
		w == WP_BAR					|| w == WP_STEN_MKII		|| \
		w == WP_MP34 )

#define IS_SCOPED_WEAPON(w) \
	(	w == WP_K43_SCOPE			|| w == WP_GARAND_SCOPE		|| \
		w == WP_FG42SCOPE )

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

#define IS_HEAVY_WEAPON(w)	\
	(	w == WP_MORTAR				|| w == WP_MORTAR2			|| \
		w == WP_PANZERFAUST			|| w == WP_FLAMETHROWER		|| \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_BROWNING  || \
		w == WP_BAZOOKA		)

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
	(	IS_HEAVY_WEAPON(w)			|| IS_SUBMACHINEGUN(w)	|| \
		w == WP_BAR					|| w == WP_STG44		|| \
		w == WP_STEN_MKII			|| w == WP_VENOM	)

#define IS_SOLDIER4_PRIMARYWEAPON(w) \
	(	IS_HEAVY_WEAPON(w)			|| \
		w == WP_BAR					|| w == WP_STG44		|| \
		w == WP_STEN_MKII			|| w == WP_VENOM	)

#define IS_COVERT_WEAPON(w) \
	(	w == WP_STEN				|| w == WP_STEN_MKII	|| \
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
		w == WP_SMOKE_BOMB			|| 								   \
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
	EV_FOOTSTEP_METAL,
	EV_FOOTSTEP_WOOD,
	EV_FOOTSTEP_GRASS,
	EV_FOOTSTEP_GRAVEL,
	EV_FOOTSTEP_ROOF,
	EV_FOOTSTEP_SNOW,
	EV_FOOTSTEP_CARPET,
	EV_FOOTSPLASH,
	EV_FOOTWADE,
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
	EV_CROUCH_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_DEATH4,
	EV_OBITUARY,
	EV_STOPSTREAMINGSOUND,		// JPW NERVE swiped from sherman
	EV_POWERUP_QUAD,
	EV_POWERUP_BATTLESUIT,
	EV_POWERUP_REGEN,
	EV_GIB_PLAYER,				// gib a previously living player
	EV_DEBUG_LINE,
	EV_STOPLOOPINGSOUND,
	EV_SMOKE,
	EV_SPARKS,
	EV_SPARKS_ELECTRIC,
	EV_EXPLODE,					// func_explosive
	EV_RUBBLE,
	EV_EFFECT,					// target_effect
	EV_MORTAREFX,				// mortar firing
	EV_SPINUP,					// JPW NERVE panzerfaust preamble
	EV_SNOW_ON,
	EV_SNOW_OFF,
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
	EV_MG42EFX,
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
	EV_SPAWN,					// jaquboss
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

// Time between location updates
#define TEAM_LOCATION_UPDATE_TIME		1000

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
	WS_SYRINGE,				// 14	-- Medic syringe uses/successes
	WS_SMOKE,				// 15	-- axis + allied smoke grenade
	WS_SATCHEL,				// 16
	WS_GRENADELAUNCHER,		// 17	-- rifles (axis + allied)
	WS_LANDMINE,			// 18
	WS_MG42,				// 19
	WS_GARAND,				// 20	-- allied carbine + garand (+ scoped)
	WS_K43,					// 21	-- axis k43 + kar98 (+ scoped)
	WS_SHOTGUN,				// 22 (jet Pilot - Shotgun)
	WS_BROWNING,			// 23 (jet Pilot - Mobile Browning)
	WS_STG44,				// 24 (jet Pilot - German StG44 Assault Rifle)
	WS_STEN_MKII,			// 25 (jet Pilot - Sten Mk II - Unsilenced) //core: is this thing used in NQ?..
	WS_BAR,					// 26	-- allied soldier BAR + allied covertops BAR
	WS_BAZOOKA,				// 27 (jet Pilot - Bazooka )
	WS_MP34,				// 28 (jet Pilot - Solothurn S1-100 )
	WS_VENOM,				// 29
	WS_POISON,				// 30

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
	MOD_UNKNOWN,
	MOD_MACHINEGUN,
	MOD_BROWNING,
	MOD_MG42,
	MOD_GRENADE,
	MOD_KNIFE,
	MOD_LUGER,
	MOD_COLT,
	MOD_MP40,
	MOD_THOMPSON,
	MOD_STEN,
	MOD_GARAND,
	MOD_SILENCER,
	MOD_FG42,
	MOD_FG42_SCOPE,
	MOD_PANZERFAUST,
	MOD_GRENADE_LAUNCHER,
	MOD_FLAMETHROWER,
	MOD_GRENADE_PINEAPPLE,
	MOD_MAPMORTAR,
	MOD_MAPMORTAR_SPLASH,
	MOD_KICKED,
	MOD_DYNAMITE,
	MOD_AIRSTRIKE,
	MOD_SYRINGE,
	MOD_AMMO,
	MOD_ARTY,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
	MOD_EXPLOSIVE,
	MOD_CARBINE,
	MOD_KAR98,
	MOD_GPG40,
	MOD_M7,
	MOD_LANDMINE,
	MOD_SATCHEL,
	MOD_SMOKEBOMB,
	MOD_MOBILE_MG42,
	MOD_SILENCED_COLT,
	MOD_GARAND_SCOPE,
	MOD_CRUSH_CONSTRUCTION,
	MOD_CRUSH_CONSTRUCTIONDEATH,
	MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER,
	MOD_K43,
	MOD_K43_SCOPE,
	MOD_MORTAR,
	MOD_AKIMBO_COLT,
	MOD_AKIMBO_LUGER,
	MOD_AKIMBO_SILENCEDCOLT,
	MOD_AKIMBO_SILENCEDLUGER,
	MOD_SMOKEGRENADE,
	MOD_SWAP_PLACES,
	MOD_SWITCHTEAM,
	MOD_GOOMBA,
	MOD_POISON,
	MOD_FEAR,
	MOD_CENSORED,
	MOD_SHOTGUN,
	MOD_BACKSTAB,
	MOD_MOBILE_BROWNING,
	MOD_BAR,
	MOD_STG44,
	MOD_BAZOOKA,
	MOD_STEN_MKII,
	MOD_MP34,
	MOD_VENOM,
	MOD_SHOVE,
	MOD_THROWKNIFE,

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
	char		*classname;	// spawning name
	char		*pickup_sound;
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
	IL_POWERUP_ALLIEDSOLDIERUNIFORM,
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
	IL_HEALTH_HEALTH_BREADANDMEAT,
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
	IL_WEAPON_AKIMBOCOLT,
	IL_WEAPON_AKIMBOSILENCEDCOLT,
	IL_WEAPON_KNIFE,
	IL_WEAPON_PANZERFAUST,
	IL_WEAPON_GRENADELAUNCHER,
	IL_WEAPON_GRENADEPINEAPPLE,
	IL_WEAPON_GRENADESMOKE,
	IL_WEAPON_SMOKETRAIL,
	IL_WEAPON_MEDIC_HEAL,
	IL_WEAPON_DYNAMITE,
	IL_WEAPON_FLAMETHROWER,
	IL_WEAPON_CLASS_SPECIAL,
	IL_WEAPON_ARTY,
	IL_WEAPON_MEDIC_SYRINGE,
	IL_WEAPON_POISON_SYRINGE,
	IL_WEAPON_MEDIC_ADRENALINE,
	IL_WEAPON_MAGICAMMO,
	IL_WEAPON_MAGICAMMO2,
	IL_WEAPON_MAGICAMMO3,
	IL_WEAPON_BINOCULARS,
	IL_WEAPON_KAR43,
	IL_WEAPON_KAR43_SCOPE,
	IL_WEAPON_KAR98RIFLE,
	IL_WEAPON_GPG40,
	IL_WEAPON_GPG40_ALLIED,
	IL_WEAPON_M1CARBINERIFLE,
	IL_WEAPON_GARANDRIFLE,
	IL_WEAPON_GARANDRIFLESCOPE,
	IL_WEAPON_FG42,
	IL_WEAPON_FG42SCOPE,
	IL_WEAPON_MORTAR,
	IL_WEAPON_MORTAR_SET,
	IL_WEAPON_MORTAR2,
	IL_WEAPON_MORTAR2_SET,
	IL_WEAPON_LANDMINE,
	IL_WEAPON_SATCHEL,
	IL_WEAPON_SATCHELDETONATOR,
	IL_WEAPON_SMOKEBOMB,
	IL_WEAPON_MOBILE_MG42,
	IL_WEAPON_MOBILE_MG42_SET,
	IL_WEAPON_SILENCER,
	IL_WEAPON_DUMMY,
	IL_WEAPON_SHOTGUN,
	IL_WEAPON_KNIFE_KABAR,
	IL_WEAPON_MOBILE_BROWNING_SET,
	IL_WEAPON_MOBILE_BROWNING,
	IL_WEAPON_STG44,
	IL_WEAPON_BAR_SET,
	IL_WEAPON_BAR,
	IL_WEAPON_STENMKII,
	IL_WEAPON_BAZOOKA,
	IL_WEAPON_MP34,
	IL_WEAPON_VENOM,
	// it_ammo
	IL_AMMO_THOMPSON,
	IL_AMMO_MP40,
	IL_AMMO_DYNAMITE,
	IL_AMMO_DISGUISE,
	IL_AMMO_AIRSTRIKE,
	IL_AMMO_LANDMINE,
	IL_AMMO_SATCHEL_CHARGE,
	IL_AMMO_LUGER,
	IL_AMMO_SMOKE_GRENADE,
	IL_AMMO_COLT,
	IL_AMMO_SYRINGE,
	IL_AMMO_GARAND,
	IL_AMMO_CARBINE,
	IL_AMMO_MG42,
	IL_AMMO_BROWNING,
	IL_AMMO_SHOTGUN,
	IL_AMMO_STG44,
	IL_AMMO_BAR,
	IL_AMMO_FG42,
	IL_AMMO_MP34,
	IL_AMMO_STEN,
	IL_AMMO_K43,
	IL_AMMO_KAR98,
	IL_AMMO_VENOM,
	IL_AMMO_MORTAR,
	IL_AMMO_BAZOOKA,
	IL_AMMO_PANZERFAUST,
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
gitem_t	*BG_FindItemForClassName( const char *className );
gitem_t	*BG_FindItemForWeapon	( weapon_t weapon );
gitem_t *BG_FindItemForAmmo		( int weapon );
weapon_t BG_FindAmmoForWeapon	( weapon_t weapon );
weapon_t BG_FindClipForWeapon	( weapon_t weapon );
//--- core: faster lookups, copy data into a more compact array, for direct indexed access..
void Init_FindClipForWeapon( void );
void Init_FindAmmoForWeapon( void );
//---

qboolean BG_AkimboFireSequence( int weapon, int akimboClip, int mainClip );
qboolean BG_IsAkimboWeapon( int weaponNum );
qboolean BG_IsAkimboSideArm( int weaponNum, playerState_t *ps );
int BG_AkimboSidearm( int weaponNum );

#define	ITEM_INDEX(x) ((x)-bg_itemlist)

// IRATA - unused
// qboolean BG_CanUseWeapon(int classNum, int teamNum, weapon_t weapon);

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
void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, qboolean snap );
weapon_t BG_DuplicateWeapon( weapon_t weap );
gitem_t* BG_ValidStatWeapon( weapon_t weap );
weapon_t BG_WeaponForMOD( int MOD );

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

#define ACC_NUM_MOUTH 3	// matches the above count

#define MAX_GIB_MODELS		16

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
qboolean BG_ValidAnimScript( int clientNum );
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
int BG_ClassWeaponCount( bg_playerclass_t* classInfo, team_t team );
const char* BG_ShortClassnameForNumber( int classNum );
const char* BG_ClassnameForNumber( int classNum );
const char* BG_ClassnameForNumber_Filename( int classNum );
const char* BG_ClassLetterForNumber( int classNum );

void BG_DisableClassWeapon( bg_playerclass_t* classinfo, int weapon );
void BG_DisableWeaponForAllClasses( int weapon );

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

	char	strTarget[64];

	splinePath_t*	next;
	splinePath_t*	prev;

	pathCorner_t	controls[MAX_SPLINE_CONTROLS];
	int				numControls;
	splineSegment_t	segments[MAX_SPLINE_SEGMENTS];

	float	length;

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

int BG_ClassTextToClass(char *token);
skillType_t BG_ClassSkillForClass( int classnum );

qboolean BG_isLightWeaponSupportingFastReload( int weapon );
qboolean BG_IsScopedWeapon( int weapon );

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
int BG_colorstrncpyz(char *in, char *out, int str_max, int out_max);
int BG_drawStrlen(const char *str);
int BG_strRelPos(char *in, int index);
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
int BG_GetTracemapGroundCeil( void );

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
	PM_NUM_TYPES
} popupMessageType_t;

typedef enum popupMessageBigType_e {
	PM_SKILL,
	PM_RANK,
	PM_DISGUISE,
	PM_BIG_NUM_TYPES
} popupMessageBigType_t;

#define NUM_HEAVY_WEAPONS 12
extern weapon_t bg_heavyWeapons[NUM_HEAVY_WEAPONS];

#define NUM_PRIMARY_HEAVY_WEAPONS 8
extern weapon_t bg_primaryHeavyWeapons[NUM_PRIMARY_HEAVY_WEAPONS];

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

#define JP_INSANITY_SPAWN       0x0001	// 1		- players spawn instantly upon death
#define JP_INSANITY_AMMUNITION  0x0002	// 2		- players have unlimited ammo clips
#define JP_INSANITY_MORTARCAM	0x0004	// 4		- mortar cam
#define JP_INSANITY_MISSILECAM	0x0008	// 8		- missile cam
#define JP_INSANITY_CHARGEBAR   0x0010	// 16 		- players have unlimited charge bars
#define JP_INSANITY_GUIDED		0x0020	// 32		- rockets can be steered by the firing player
#define JP_INSANITY_HOMING		0x0040	// 64		- rockets are homing
#define JP_INSANITY_ALLWEAPONS  0x0080	// 128		- players can pick up any weapon on the ground
#define JP_INSANITY_VENOM		0x0100	// 256		- lets soldiers choose Venom
#define JP_INSANITY_HPLEFT		0x0200	// 512		- shows attacker's HP left to victim
#define JP_INSANITY_HPLEFT_EXT	0x0400	// 1024		- shows attacker's HP left to victim with distance
#define JP_INSANITY_DYNAKICK	0x0800	// 2048		- dynamites can be kicked
#define JP_INSANITY_NUM_MODES	12

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
#define WARMODE_PLIERS			0x1000	// 4096		- enable pliers
#define WARMODE_NUM_MODES		13

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
#define ANNOUNCE_FIRSTHEADSHOT		0x0010 // 16
#define ANNOUNCE_KICKSOUND			0x0020 // 32
#define ANNOUNCE_BANSOUND			0x0040 // 64
#define ANNOUNCE_PERS_OBJ_MSG_POPUP	0x0080 // 128	core: mission msgs. with playernames in popup-area (instead of top-area)..
#define ANNOUNCE_PERS_OBJ_DISABLED	0x0100 // 256	core: disable all mission messages

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

extern float disguiseTestCost[NUM_SKILL_LEVELS];

void BG_IndexedSubstring(char *dest, int maxLen, const char *source, int index);

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
#define REALISM_EXTENDED_PRONE		1	// play transition animations, locked when standing, slow standing
#define	REALISM_NOLADDERGUN			2	// cannot use weapon on ladders
#define	REALISM_AURA_WITH_PVS		8	// IRATA: activate PVS in aura
#define REALISM_NOLEAN				16	// leaning disabled

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
    GAMEMODEL_WORLD_SEARCHLIGHT,    // "models/mapobjects/light/searchlight_pivot.md3"
    GAMEMODEL_WORLD_ALARMBOX,       // "models/mapobjects/electronics/alarmbox.md3"
    GAMEMODEL_WORLD_BOX_32,         // "models/mapobjects/boxes/box32.md3"
    GAMEMODEL_WORLD_BOX_48,         // "models/mapobjects/boxes/box48.md3"
    GAMEMODEL_WORLD_BOX_64,         // "models/mapobjects/boxes/box64.md3"
    GAMEMODEL_WORLD_BENCH,          // "models/furniture/bench/bench_sm.md3"
    GAMEMODEL_WORLD_RADIO,          // "models/mapobjects/electronics/radio1.md3"
    GAMEMODEL_WORLD_RADIOS,		    // "models/mapobjects/electronics/radios.md3"
    GAMEMODEL_WORLD_CASTLEBED,	    // "models/furniture/bed/castlebed.md3"
    GAMEMODEL_WORLD_TABLE_56X112,	// "models/furniture/table/56x112tablew.md3"
    GAMEMODEL_WORLD_CRATE_32X64,	// "models/furniture/crate/crate32x64.md3"
    GAMEMODEL_WORLD_WOODFLIP,		// "models/furniture/table/woodflip.md3";
    GAMEMODEL_WORLD_LOCKER,		    // "models/furniture/storage/lockertall.md3"
    GAMEMODEL_WORLD_CHAIR_OFFICE,	// "models/furniture/chair/chair_office3.md3"
    GAMEMODEL_WORLD_CHAIR_HIBACK,	// "models/furniture/chair/hiback5.md3"
    GAMEMODEL_WORLD_CHAIR_SIDECHAIR,// "models/furniture/chair/sidechair3.md3"
    GAMEMODEL_WORLD_CHAIR_CHAT,		// "models/furniture/chair/chair_chat.md3"
    GAMEMODEL_WORLD_CHAIR_CHATARM,	// "models/furniture/chair/chair_chatarm.md3"
    GAMEMODEL_WORLD_DESKLAMP,	    // "models/furniture/lights/desklamp.md3"
    GAMEMODEL_WORLD_BARREL_C,	    // "models/furniture/barrel/barrel_c.md3"
    GAMEMODEL_WORLD_BARREL_D,	    // "models/furniture/barrel/barrel_d.md3"
    GAMEMODEL_WORLD_BARREL_B,	    // "models/furniture/barrel/barrel_b.md3"
    GAMEMODEL_WORLD_CRATE_64,	    // "models/furniture/crate/crate64.md3"
    GAMEMODEL_WORLD_CRATE_32,	    // "models/furniture/crate/crate32.md3"
    GAMEMODEL_WORLD_FOOTLOCKER,     // "models/mapobjects/furniture/footlocker.md3"
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

// core:
#define NUM_ENDGAME_AWARDS			25		// total number of endgame Awards
#define NUMSHOW_ENDGAME_AWARDS		14		// number of awards to display (that will fit on screen)


// core: bitflags for cvar team_maxIsPercentage
// Set team_maxX restrictions as a percentage or as an absolute value (players per team)
#define TEAM_MAXWEAPONS_PERCENT		1
#define TEAM_MAXCLASSES_PERCENT		2


#endif
