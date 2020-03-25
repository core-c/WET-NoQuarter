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

//#define SAVEGAME_SUPPORT	// uncomment to enable savegames
							// enabling this requires you to run extractfuncs.bat as well before compiling qagame
//#define NO_BOT_SUPPORT	// jaquboss - we still need dummies to shoot at :)
#define USE_MDXFILE

// ifdefs for stuff which won't be in (first) release version
//#define __CLIENTCMD__ // server side clientcmd code, SS servers only
//#define	__SSYSTEM__	// skin system code, needs selection code, and corpse code, next version after first release version
//#define __JETPILOT__ // not yet done
//#define __SELFDEFENSE__ // needs anims and polishing
/*
			:TODO:

0) change cg_player() and cg_player_limbo() code to work with new system -- DONE!!
1) add cvars and network command to change playerCharacter indexes -- DONE!!
	2) make a submenu in limbo menu to do it automaticly for player -- started
3) load and use ACC models compatible with new system -- DONE!!
4) rewrite character files -- DONE!! ( still missing ACC models and need to add some more options :P )
	5) do a code for corpses and looted corpses - this will be thoughtest part
	6) code grenade and main weapon visualisation -- being done
*/

#define SPRINTTIME					20000.0f
#define DEBUG_BOT_RETREATBEHAVIOR	1
#define	DEFAULT_GRAVITY				800
#define FORCE_LIMBO_HEALTH			-75 // JPW NERVE
#define	GIB_HEALTH					-175 // JPW NERVE
#define	ARMOR_PROTECTION			0.66
#define HOLDBREATHTIME				12000
#define HOLDBREATHTIME_LONG			15000
#define	MAX_ITEMS					256
#define	RANK_TIED_FLAG				0x4000
#define MAX_FIRETEAM_MEMBERS		8	// jP raised max fireteam members to 8 from 6


//#define DEFAULT_SHOTGUN_SPREAD	700
//#define DEFAULT_SHOTGUN_COUNT	11

//#define	ITEM_RADIUS			15		// item sizes are needed for client side pickup detection
#define	ITEM_RADIUS				10 // Rafael changed the radius so that the items would fit in the 3 new containers

// RF, zombie getup
//#define	TIMER_RESPAWN	(38*(1000/15)+100)

//#define	LIGHTNING_RANGE		600
//#define	TESLA_RANGE			800

#define FLAMETHROWER_RANGE		2500		// DHM - Nerve :: multiplayer range, was 850 in SP

//#define ZOMBIE_FLAME_RADIUS 300

// RF, AI effects
//#define	PORTAL_ZOMBIE_SPAWNTIME		3000
//#define	PORTAL_FEMZOMBIE_SPAWNTIME	3000

#define	SCORE_NOT_PRESENT		-9999	// for the CS_SCORES[12] when only one player is present

#define	VOTE_TIME				30000	// 30 seconds before vote times out

// Ridah, disabled these
//#define	MINS_Z				-24
//#define	DEFAULT_VIEWHEIGHT	26
//#define CROUCH_VIEWHEIGHT	12
// done.

// Rafael
// note to self: Corky test
//#define	DEFAULT_VIEWHEIGHT	26
//#define CROUCH_VIEWHEIGHT	12
#define	DEFAULT_VIEWHEIGHT		40
#define CROUCH_VIEWHEIGHT		16
#define DEAD_VIEWHEIGHT			-16

#define PRONE_VIEWHEIGHT		-8

extern vec3_t	playerlegsProneMins;
extern vec3_t	playerlegsProneMaxs;

#define MAX_COMMANDMAP_LAYERS	16

#define	DEFAULT_MODEL		"multi"
#define DEFAULT_HEAD		"default"	// technically the default head skin.  this means "head_default.skin" for the head

// RF, on fire effects
#define FIRE_FLASH_TIME			2000
#define	FIRE_FLASH_FADEIN_TIME	1000

#define LIGHTNING_FLASH_TIME	150

#define AAGUN_DAMAGE			25
#define AAGUN_SPREAD			10

// NOTE: use this value, and THEN the cl_input.c scales to tweak the feel
#define	MG42_IDLEYAWSPEED		80.0	// degrees per second (while returning to base)
#define MG42_SPREAD_MP			100

#define MG42_DAMAGE_MP			20
#define MG42_RATE_OF_FIRE_MP	66

#define MG42_DAMAGE_SP			40
#define MG42_RATE_OF_FIRE_SP	100

#define AAGUN_RATE_OF_FIRE		100
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

// START - TAT 10/21/2002
// New icon based bot action command system
typedef enum
{
	BOT_ACTION_ATTACK = 0,
	BOT_ACTION_COVER,			// 1
	BOT_ACTION_MOUNTGUN,		// 2
	BOT_ACTION_OPENDOOR,		// 3
	BOT_ACTION_USEDYNAMITE,		// 4
	BOT_ACTION_DISARM,			// 5
	BOT_ACTION_CONSTRUCT,		// 6
	BOT_ACTION_REPAIR,			// 7
	BOT_ACTION_REVIVE,			// 8
	BOT_ACTION_GETDISGUISE,		// 9
	BOT_ACTION_HEAL,			// 10
	BOT_ACTION_AMMO,			// 11
	BOT_ACTION_GRENADELAUNCH,	// 12
	BOT_ACTION_PICKUPITEM,		// 13
	BOT_ACTION_PANZERFAUST,		// 14
	BOT_ACTION_FLAMETHROW,		// 15
	BOT_ACTION_MG42,			// 16
	BOT_ACTION_MOUNTEDATTACK,	// 17		-- attack when mounted on mg42
	BOT_ACTION_KNIFEATTACK,		// 18
	BOT_ACTION_LOCKPICK,		// 19

	BOT_ACTION_MAXENTITY,

	// None of these need an entity...
	BOT_ACTION_RECON = BOT_ACTION_MAXENTITY,	// 20
	BOT_ACTION_SMOKEBOMB,		// 21
	BOT_ACTION_FINDMINES,		// 22
	BOT_ACTION_PLANTMINE,		// 23
	BOT_ACTION_ARTILLERY,		// 24
	BOT_ACTION_AIRSTRIKE,		// 25
	BOT_ACTION_MOVETOLOC,		// 26

	// NOTE: if this gets bigger than 32 items, need to make botMenuIcons bigger
	BOT_ACTION_MAX
} botAction_t;
// END - TAT 10/21/2002

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

	// Gordon: FIXME: remove
	const char *opponentName;
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

#define MAX_MOTDLINES					6

// Multiview settings
#define MAX_MVCLIENTS					32
#define MV_SCOREUPDATE_INTERVAL			5000	// in msec

#define MAX_CHARACTERS					16

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC						2
#define	CS_MESSAGE						3	//- unused 	// from the map worldspawn's message field
#define	CS_MOTD							4	//- unused	// g_motd string for server message of the day
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
//#define CS_MAIN_AXIS_OBJECTIVE		25		// Most important current objective -- unused !
//#define CS_MAIN_ALLIES_OBJECTIVE		26		// Most important current objective -- unused !
#define CS_MUSIC_QUEUE					25
#define CS_SCRIPT_MOVER_NAMES			26
#define CS_CONSTRUCTION_NAMES			27

//#define CS_VERSIONINFO				28		// Versioning info for demo playback compatibility -- TODO!!
#define CS_REINFSEEDS					28		// Reinforcement seeds
#define CS_SERVERTOGGLES				29		// Shows current enable/disabled settings (for voting UI)
#define CS_GLOBALFOGVARS				30
#define CS_AXIS_MAPS_XP					31
#define CS_ALLIED_MAPS_XP				32
#define	CS_INTERMISSION_START_TIME		33		//
#define CS_ENDGAME_STATS				34
#define CS_CHARGETIMES					35
#define CS_FILTERCAMS					36
#define CS_NOQUARTERINFO				37
#define CS_SKILLLEVELS					38
#define CS_FORCECVAR					39
#define CS_SVCVAR						40
#define CS_CONFIGNAME					41
		
#ifdef USELOCATIONS
#define MAX_LOCATIONS					93		// jet Pilot - Maximum target_location entities oasis has 105! -- you could stick it above jet..
#endif

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

#ifdef __SSYSTEM__

#define CS_PCINFO						( CS_STRINGS +				MAX_CSSTRINGS				)
#define CS_MAX							( CS_PCINFO +				MAX_CLIENTS					)

#else 

#define CS_MAX							( CS_SKILLS +				MAX_CSSTRINGS				)

#endif

// jet Pilot - to undo, rename CS_LOCATIONS to CS_MAX, and comment out CS_MAX
#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

//#ifndef GAMETYPES
//#define GAMETYPES
typedef enum {
	GT_SINGLE_PLAYER,
	GT_COOP,
	GT_WOLF,
	GT_WOLF_STOPWATCH,
	GT_WOLF_CAMPAIGN,	// Exactly the same as GT_WOLF, but uses campaign roulation (multiple maps form one virtual map)
	GT_WOLF_LMS,
	GT_MAX_GAME_TYPE
} gametype_t;
//#define GAMETYPES

// Rafael gameskill
/*typedef enum {
	GSKILL_EASY = 1,
	GSKILL_MEDIUM,
	GSKILL_MEDIUMHARD, // normal default level
	GSKILL_HARD,
	GSKILL_VERYHARD,
	GSKILL_MAX		// must always be last
} gameskill_t;*/

//#endif // ifndef GAMETYPES

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
	PM_PLAYDEAD,		// no movement or status bar
	PM_JETPILOT,		// jet Pilot Movement!
} pmtype_t;

typedef enum {
	WEAPON_READY, 
	WEAPON_RAISING,
	WEAPON_RAISING_TORELOAD,
	WEAPON_DROPPING,
	WEAPON_DROPPING_TORELOAD,
//	WEAPON_READYING,	// getting from 'ready' to 'firing'
//	WEAPON_RELAXING,	// weapon is ready, but since not firing, it's on it's way to a "relaxed" stance
	WEAPON_FIRING,
//	WEAPON_FIRINGALT,
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
//#define PMF_PRONE_BIPOD		1024	// prone with a bipod set
#define	PMF_MEDICUSE		1024
#define PMF_FLAILING		2048
#define PMF_FOLLOW			4096	// spectate following another player
#define	PMF_TIME_LOAD		8192	// hold for this time after a load game, and prevent large thinks
#define	PMF_LIMBO			16384	// JPW NERVE limbo state, pm_time is time until reinforce
#define PMF_TIME_LOCKPLAYER	32768	// DHM - Nerve :: Lock all movement and view changes

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK|PMF_TIME_LOCKPLAYER|PMF_MEDICUSE/*|PMF_TIME_LOAD*/)
/*
#define VIEW_WARPED			0x00000001
#define VIEW_SWAY			0x00000002
#define SOUND_UNDERWATER	0x00000004
#define VIEW_HALLUCINATING	0x00000002
#define VIEW_THERMAL		0x00000004
#define VIEW_MONOCHROME		0x00000008
#define	VIEW_TUNNELVISION	0x00000010
#define VIEW_INVERTED		0x00000020
#define SOUND_UNDERWATER	0x00000040
#define SOUND_RINGING		0x00000080
#define SOUND_DEAFENED		0x00000100*/

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
	qboolean	noFootsteps;		// if the game is setup for no footsteps by the server
	qboolean	noWeapClips;		// if the game is setup for no weapon clips by the server
	qboolean	gauntletHit;		// true if a gauntlet attack would actually hit something

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

//#ifdef GAMEDLL	// the whole stamina thing is only in qagame

	// jet Pilot - "auras" players can give off. Any teammate within 512 units will enjoy the benefits granted.
	
	int			auraEffects[5];		// NUM_PLAYER_CLASSES

	qboolean	covering_fire;
	qboolean	first_aid;			//  Stationary medic 6+ : all health pickups give 150% health
	qboolean	technician;			//          Engineer 6+ : Weapons cool at double the normal rate
	qboolean	leadership;			//         Field Ops 5+ : Recharge Stamina at double the normal rate
	qboolean	awareness;			//        Covert Ops 6+ : Sense Landmines at double range

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
	STAT_CLIENTS_READY,				// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_MAX_HEALTH,				// health / armor limit, changable by handicap
	STAT_PLAYER_CLASS,				// DHM - Nerve :: player class in multiplayer
	//STAT_CAPTUREHOLD_RED,			// JPW NERVE - red team score
	//STAT_CAPTUREHOLD_BLUE,			// JPW NERVE - blue team score
	STAT_XP,						// Gordon: "realtime" version of xp that doesnt need to go thru the scoreboard
	STAT_LEANING,
	STAT_SFXFLAGS,
} statIndex_t;

// jet Pilot - 
//#define STAT_LEANING	STAT_CLIENTS_READY
#define STAT_TEAMAURA	STAT_CLIENTS_READY

#define AURAMASK_SOLDIER		0x0000000F	// 0-16
#define AURAMASK_MEDIC			0x000000F0	// 0-16 << 4
#define AURAMASK_ENGINEER		0x00000F00	// 0-16 << 8
#define AURAMASK_FIELDOPS		0x0000F000	// 0-16 << 12
#define AURAMASK_COVERTOPS		0x000F0000	// 0-16 << 16

#define AURASHIFT_SOLDIER		0
#define AURASHIFT_MEDIC			4
#define AURASHIFT_ENGINEER		8
#define AURASHIFT_FIELDOPS		12
#define AURASHIFT_COVERTOPS		16

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
	PERS_AWARDS,					// (0-255 each) Goombas, Darwin Awards, Best Spree, Bounty
	PERS_STATS,						// (0-255 each) Damage Type, Impact Point, Enemy Weapon, Enemy Direction

	// Rafael - mg42				// (SA) I don't understand these here.  can someone explain?
	PERS_HWEAPON_USE,	
	PERS_KILLSPREE,					// was PERS_WOLFKICK
	
	PERS_KILLS,					// jaquboss , number of kills in rows, needed to be here because we may die after our multikill
	PERS_DARWINDEATHS,				// anything after this doesn't seem to work
} persEnum_t;

// jet Pilot - hijack these, convert to flags

#define AWARDMASK_GOOMBA		0x000000FF	// 0-255		
#define AWARDMASK_DARWIN		0x0000FF00  // 0-255 << 8
#define AWARDMASK_BEST_SPREE	0x00FF0000  // 0-255 << 16
#define AWARDMASK_BOUNTY		0xFF000000	// 0-255 << 24

#define AWARDSHIFT_GOOMBA		0
#define AWARDSHIFT_DARWIN		8
#define AWARDSHIFT_BEST_SPREE	16
#define AWARDSHIFT_BOUNTY		24

#define STATSMASK_ENEMY_WEAPON	0x000000FF	// 0-255
#define STATSMASK_IMPACT_POINT	0x00000F00	// 0-16  << 8
#define STATSMASK_PAIN_TYPE		0x0000F000	// 0-16  << 12
#define STATSMASK_KILLSPREE		0x00FF0000	// 0-255 << 16
#define STATSMASK_DIRECTION		0xFF000000	// 0-255 << 24

#define STATSHIFT_ENEMY_WEAPON	0
#define STATSHIFT_IMPACT_POINT	8
#define STATSHIFT_PAIN_TYPE		12
#define STATSHIFT_KILLSPREE		16
#define STATSHIFT_DIRECTION		24

int	BG_SetStatGoombas(playerState_t *ps, int value);
int	BG_SetStatDarwin(playerState_t *ps, int value);
int	BG_SetStatBestSpree(playerState_t *ps, int value);
int	BG_SetStatBounty(playerState_t *ps, int value);

int BG_SetLastEnemyWeapon(playerState_t *ps, int value);
int BG_SetImpactPoint(playerState_t *ps, int value);
int	BG_SetPainType(playerState_t *ps, int value);
int	BG_SetKillSpree(playerState_t *ps, int value);
int BG_SetDirection(playerState_t *ps, int value);

int	BG_GetStatGoombas(playerState_t *ps);
int BG_GetStatBounty(playerState_t *ps);
int	BG_GetStatDarwin(playerState_t *ps);
int	BG_GetStatBestSpree(playerState_t *ps);

int BG_GetLastEnemyWeapon(playerState_t *ps);
int BG_GetImpactPoint(playerState_t *ps);
int	BG_GetPainType(playerState_t *ps);
int	BG_GetKillSpree(playerState_t *ps);
int BG_GetDirection(playerState_t *ps);

int	BG_SetStatGoombas(playerState_t *ps, int value);
int	BG_SetStatDarwin(playerState_t *ps, int value);
int	BG_SetStatBestSpree(playerState_t *ps, int value);
int BG_SetStatBounty(playerState_t *ps, int value);

int BG_AdjustGoombas(playerState_t *ps, int value);
int BG_AdjustLastEnemyWeapon(playerState_t *ps, int value);
int	BG_AdjustKillSpree(playerState_t *ps, int value);
int BG_AdjustBounty(playerState_t *ps, int value);
int BG_AdjustBestSpree(playerState_t *ps, int value);

//#define PERS_SPAWNMASK  0x


// entityState_t->eFlags
#define	EF_DEAD				0x00000001		// don't draw a foe marker over players with EF_DEAD
#define EF_NONSOLID_BMODEL	0x00000002		// bmodel is visible, but not solid
//#define	EF_FORCE_END_FRAME	EF_NONSOLID_BMODEL	// force client to end of current animation (after loading a savegame)
#define	EF_POISONED		EF_NONSOLID_BMODEL	
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
#define EF_VIEWING_CAMERA	0x00200000		// player is viewing a camera
#define EF_AAGUN_ACTIVE		0x00400000		// Gordon: player is manning an AA gun
#define EF_PLAYDEAD			0x00800000		// tjw: player is playing dead

#ifdef __SELFDEFENSE__
#define EF_DEFENDING		EF_SMOKINGBLACK	// jaquboss - player is defending himselfs while wounded
#endif

// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000
#define	EF_MOTION			0x01000000		// tjw: player animation is in motion
#define	EF_SPARE2			0x02000000		// Gordon: freed
#define	EF_BOUNCE			0x04000000		// for missiles
#define	EF_BOUNCE_HALF		0x08000000		// for missiles
#define	EF_MOVER_STOP		0x10000000		// will push otherwise	// (SA) moved down to make space for one more client flag
#define EF_MOVER_BLOCKED	0x20000000		// mover was blocked dont lerp on the client // xkan, moved down to make space for client flag

#define BG_PlayerMounted( eFlags ) (( eFlags & EF_MG42_ACTIVE ) || ( eFlags & EF_MOUNTEDTANK ) || ( eFlags & EF_AAGUN_ACTIVE ))

// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000
typedef enum {
	PW_NONE,

	// (SA) for Wolf
	PW_INVULNERABLE,
//	PW_FIRE,				//----(SA)	
//	PW_ELECTRIC,			//----(SA)
	PW_FLAKJACKET,	
	PW_HELMET,	
	PW_BREATHER,			//----(SA)	
	PW_NOFATIGUE,			//----(SA)	

	PW_REDFLAG,
	PW_BLUEFLAG,

	PW_OPS_DISGUISED,
	PW_OPS_CLASS_1,
	PW_OPS_CLASS_2,
	PW_OPS_CLASS_3,

	PW_ADRENALINE,
	PW_OFFHAND_GRENADES,	// jet Pilot - lvl 7 LW gives this

	PW_BLACKOUT		= 14,	// OSP - spec blackouts. FIXME: we don't need 32bits here...relocate
	PW_MVCLIENTLIST = 15,	// OSP - MV client info.. need a full 32 bits

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

typedef enum {
	HI_NONE,

//	HI_TELEPORTER,
	HI_MEDKIT,

	// new for Wolf
	HI_WINE,
	HI_SKULL,
	HI_WATER,
	HI_ELECTRIC,
	HI_FIRE,
	HI_STAMINA,
	HI_BOOK1,	//----(SA)	added
	HI_BOOK2,	//----(SA)	added
	HI_BOOK3,	//----(SA)	added
	HI_11,
	HI_12,
	HI_13,
	HI_14,
//	HI_15,	// ?

	HI_NUM_HOLDABLE
} holdable_t;

#ifdef KITS
// START Mad Doc - TDF
// for kits dropped by allied bots in SP
typeef enum {
	KIT_SOLDIER,
	KIT_MEDIC,
	KIT_ENGINEER,
	KIT_LT,
	KIT_COVERTOPS
} kit_t;
// END Mad Doc - TDF
#endif

// NOTE: we can only use up to 15 in the client-server stream
// SA NOTE: should be 31 now (I added 1 bit in msg.c)
// RF NOTE: if this changes, please update etmain\botfiles\inv.h
#define	NO_AIRSTRIKE	1
#define	NO_ARTY	2

typedef enum weapon_s
{
	WP_NONE,				// 0
	WP_KNIFE,				// 1
	WP_LUGER,				// 2
	WP_MP40,				// 3
	WP_GRENADE_LAUNCHER,	// 4
	WP_PANZERFAUST,			// 5
	WP_FLAMETHROWER,		// 6

	WP_COLT,				// 7	// equivalent american weapon to german luger
	WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	WP_GRENADE_PINEAPPLE,	// 9
	WP_STEN,				// 10	// silenced sten sub-machinegun
	WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	WP_AMMO,				// 12	// JPW NERVE likewise
	WP_ARTY,				// 13

	WP_SILENCER,			// 14	// used to be sp5
	WP_DYNAMITE,			// 15
	WP_SMOKETRAIL,			// 16
//	WP_MAPMORTAR,			// 17	
	VERYBIGEXPLOSION,		// 18	// explosion effect for airplanes
	WP_MEDKIT,				// 19
	WP_BINOCULARS,			// 20

	WP_PLIERS,				// 21
	WP_SMOKE_MARKER,		// 22	// Arnout: changed name to cause less confusion
	WP_KAR98,				// 23	// WolfXP weapons
	WP_CARBINE,				// 24
	WP_GARAND,				// 25
	WP_LANDMINE,			// 26
	WP_SATCHEL,				// 27
	WP_SATCHEL_DET,			// 28
	WP_TRIPMINE,			// 29
	WP_SMOKE_BOMB,			// 30

	WP_MOBILE_MG42,			// 31
	WP_K43,					// 32
	WP_FG42,				// 33
	WP_DUMMY_MG42,          // 34 // Gordon: for storing heat on mounted mg42s...
	WP_MORTAR,				// 35
//	WP_LOCKPICK,			// 36	// Mad Doc - TDF lockpick
	WP_AKIMBO_COLT,			// 37
	WP_AKIMBO_LUGER,		// 38

// ONLY secondaries below this mark, as they are checked >= WP_GPG40 && < WP_NUM_WEAPONS
	WP_GPG40,				// 39
	WP_M7,					// 40
	WP_SILENCED_COLT,		// 41 	
// end secondary weapons 

	WP_GARAND_SCOPE,		// 42
	WP_K43_SCOPE,			// 43
	WP_FG42_SCOPE,			// 44
	WP_MORTAR_SET,			// 45
	WP_MEDIC_ADRENALINE,	// 46
	WP_AKIMBO_SILENCEDCOLT,	// 47
	WP_AKIMBO_SILENCEDLUGER,// 48
	WP_MOBILE_MG42_SET,		// 49

// jet Pilot - new WP_ definitions

	WP_SHOTGUN,				// 50 - Winchester Model 1897 12-gauge 
	WP_KNIFE_KABAR,			// 51 - US KABAR Combat Knife
	WP_MOBILE_BROWNING,		// 52 - Browning .30 Light Machinegun
	WP_MOBILE_BROWNING_SET,	// 53 - (as above with bipod deployed)
	WP_BAR,					// 54 - Browning Automatic Rifle
	WP_BAR_SET,				// 55 - (as above with bipod deployed)
	WP_STG44,				// 56 - Sturmgewehr 44 Assault Rifle
	WP_STEN_MKII,			// 57 - Sten MkII SMG (unsilenced)
	WP_BAZOOKA,				// 58 - M1 Bazooka Anti-Tank
	WP_MP34,				// 59 - Solothurn S1-100 SMG
	WP_FIREBOLT,			// 60 - insanity weapon
	WP_MORTAR2,				// 61
	WP_MORTAR2_SET,			// 62
	WP_VENOM,				// 63
	WP_POISON_SYRINGE,		// not 64 note the numbering is not complete

// end of jP additions

	WP_NUM_WEAPONS			// WolfMP: 32 WolfXP: 50
							// NOTE: this cannot be larger than 64 for AI/player weapons!
} weapon_t;

//#define WP_KNIFE_KABAR WP_KNIFE

#define WPF_NO_L0_FOPS_BINOCS			1 
#define WPF_UNDERWATER_SYRINGE			2 
#define WPF_UNDERWATER_PLIERS			4 
#define WPF_TM_AIRSTRIKE_RESTORE_FULL	8 
#define WPF_TM_AIRSTRIKE_RESTORE_HALF	16 
#define WPF_AMMO_RESTORES_HELMET		32 
//#define WPF_DROP_BINOCS					64
//#define WPF_L4HW_KEEPS_PISTOL			128 
//#define WPF_GARAND_RELOADS				256

#define MISC_DOUBLE_JUMP				1

// matt: g_medics flags 
// Medics can't pick up their own med packs to heal poisoning 
#define MEDIC_NOSELFPACKPOISON 1 
// Medics can't pick up their own med packs at all 
#define MEDIC_NOSELFPACK 2 
// Flag 4 unused 
// Flag 8 unused 
// Medics can't get akimbos 
#define MEDIC_NOAKIMBO 16 
// Medics can't carry SMG, nor pick one up 
#define MEDIC_PISTOLONLY 32 
// Alternative to tk/revive for medics 
#define MEDIC_SYRINGEHEAL 64 
// Flag 128 unused
// No self-adren 
#define MEDIC_NOSELFADREN 256 


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
/*
typedef enum {
	SK_BATTLE_SENSE,
	SK_HEAVY_WEAPONS,
	SK_FIRST_AID,
	SK_EXPLOSIVES_AND_CONSTRUCTION,	
	SK_SIGNALS,
	SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS,
	SK_LIGHT_WEAPONS,	
	SK_NUM_SKILLS
} skillType_t;
*/

// jet Pilot - Skill Name Shortcuts

#define SK_SOLDIER		SK_HEAVY_WEAPONS
#define SK_MEDIC		SK_FIRST_AID
#define SK_ENGINEER		SK_EXPLOSIVES_AND_CONSTRUCTION
#define SK_FIELDOP		SK_SIGNALS
#define SK_FIELDOPS		SK_SIGNALS
#define SK_COVERTOP		SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS
#define SK_COVERTOPS	SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS

// end of jP additions

extern const char* skillNames[SK_NUM_SKILLS];
extern const char* skillNamesLine1[SK_NUM_SKILLS];
extern const char* skillNamesLine2[SK_NUM_SKILLS];
extern const char* medalNames[SK_NUM_SKILLS];

//#define NUM_SKILL_LEVELS 5
#define NUM_SKILL_LEVELS 10

//#ifdef GAMEDLL
extern int skillLevels[SK_NUM_SKILLS][NUM_SKILL_LEVELS];
/*#else 
extern const int skillLevels[NUM_SKILL_LEVELS];
#endif*/

typedef struct {
	weaponStats_t	weaponStats[WP_NUM_WEAPONS];
	int				suicides;
	int				hitRegions[HR_NUM_HITREGIONS];
	int				objectiveStats[MAX_OBJECTIVES];	
} playerStats_t;

typedef struct ammotable_s {
	int		maxammo;				// 
	int		uses;					// 
	int		maxclip;				// 
	int		defaultStartingAmmo;	// Mad Doc - TDF
	int		defaultStartingClip;	// Mad Doc - TDF
	int		reloadTime;				// 
	int		fireDelayTime;			// 
	int		nextShotTime;			// 
//----(SA)	added
	int		maxHeat;				// max active firing time before weapon 'overheats' (at which point the weapon will fail)
	int		coolRate;				// how fast the weapon cools down. (per second)
//----(SA)	end
	int		mod;					// means of death
} ammotable_t;


// Lookup table to find ammo table entry
extern ammotable_t *GetAmmoTableData(int ammoIndex);

extern int weapAlts[];	// defined in bg_misc.c


//----(SA)	
// for routines that need to check if a WP_ is </=/> a given set of weapons
#define WP_BEGINSECONDARY	WP_GPG40
#define WP_LASTSECONDARY	WP_SILENCED_COLT
#define WEAPS_ONE_HANDED	((1<<WP_KNIFE)|(1<<WP_LUGER)|(1<<WP_COLT)|(1<<WP_SILENCER)|(1<<WP_SILENCED_COLT)|(1<<WP_GRENADE_LAUNCHER)|(1<<WP_GRENADE_PINEAPPLE))

// TTimo
// NOTE: what about WP_VENOM and other XP weapons?
// rain - #81 - added added akimbo weapons and deployed MG42
#define IS_AUTORELOAD_WEAPON(w) \
	(	\
		w == WP_LUGER				|| w == WP_COLT					|| \
		w == WP_SILENCER			|| w == WP_SILENCED_COLT		|| \
		w == WP_AKIMBO_LUGER		|| w == WP_AKIMBO_COLT			|| \
		w == WP_AKIMBO_SILENCEDLUGER|| w == WP_AKIMBO_SILENCEDCOLT	|| \
		w == WP_STEN				|| w == WP_STEN_MKII			|| \
		w == WP_MP40				|| w == WP_THOMPSON				|| \
		w == WP_STG44				|| w == WP_VENOM				|| \
		w == WP_KAR98				|| w == WP_K43_SCOPE			|| w == WP_K43			|| \
		w == WP_CARBINE				|| w == WP_GARAND_SCOPE			|| w == WP_GARAND		|| \
		w == WP_FG42				|| w == WP_FG42_SCOPE			|| \
		w == WP_BAR					|| w == WP_BAR_SET				|| \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_MG42_SET		|| \
		w == WP_MOBILE_BROWNING		|| w == WP_MOBILE_BROWNING_SET	|| \
		w == WP_SHOTGUN				|| w == WP_MP34 \
	)

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
	(	w == WP_K43_SCOPE			|| w == WP_GARAND_SCOPE || w == WP_FG42_SCOPE )

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
	(	w == WP_MORTAR				|| w == WP_MORTAR2				|| w == WP_PANZERFAUST			|| w == WP_FLAMETHROWER || \
		w == WP_MOBILE_MG42			|| w == WP_MOBILE_BROWNING		|| w == WP_BAZOOKA		)
// jaquboss added pistols, needed for picking up

#define IS_SECONDARY(w) \
	(	w == WP_AKIMBO_COLT			|| w == WP_AKIMBO_LUGER			|| \
		w == WP_COLT				|| w == WP_LUGER			|| \
		w == WP_SILENCED_COLT			|| w == WP_SILENCER			|| \
		w == WP_AKIMBO_SILENCEDCOLT	|| w == WP_AKIMBO_SILENCEDLUGER )

#define IS_PISTOL_ALL(w) \
	(	IS_PISTOL(w) || IS_SILENCED_PISTOL(w) || IS_AKIMBO_PISTOL(w) || IS_AKIMBO_SILENCED_PISTOL(w)	)


#define IS_PISTOL(w) \
	(	w == WP_COLT			|| w == WP_LUGER			)

#define IS_SILENCED_PISTOL(w) \
	(	w == WP_SILENCED_COLT			|| w == WP_SILENCER			)

#define IS_AKIMBO_PISTOL(w) \
	(	w == WP_AKIMBO_COLT			|| w == WP_AKIMBO_LUGER			)

#define IS_AKIMBO_SILENCED_PISTOL(w) \
	(	w == WP_AKIMBO_SILENCEDCOLT			|| w == WP_AKIMBO_SILENCEDLUGER			)

#define IS_SUBMACHINEGUN(w) \
	(	w == WP_THOMPSON			|| w == WP_MP40			)

#define IS_SOLDIER_WEAPON(w) \
	(	IS_HEAVY_WEAPON(w)			|| IS_SUBMACHINEGUN(w)			|| w == WP_BAR			|| \
		w == WP_STG44				|| w == WP_STEN_MKII			|| w == WP_VENOM	)

#define IS_COVERT_WEAPON(w) \
	(	w == WP_STEN				|| w == WP_STEN_MKII			|| w == WP_K43			|| \
		w == WP_GARAND				|| w == WP_FG42					|| w == WP_BAR			|| \
		w == WP_MP34	  )

#define IS_FIELDOPS_WEAPON(w) \
	(	IS_SUBMACHINEGUN(w)			|| w == WP_SHOTGUN 	)

#define IS_ENGINEER_WEAPON(w) \
	(	IS_SUBMACHINEGUN(w)			|| w == WP_SHOTGUN				|| \
		w == WP_KAR98				|| w == WP_CARBINE 	)

#define IS_BIPOD_WEAPON(w) \
	(	w == WP_MOBILE_MG42			|| w == WP_MOBILE_BROWNING		|| w == WP_BAR )

#define IS_NONFLASH_WEAPON(w) \
	(	w == WP_GRENADE_LAUNCHER	|| w == WP_GRENADE_PINEAPPLE	|| \
		w == WP_GPG40				|| w == WP_M7					|| \
		w == WP_KNIFE				|| w == WP_KNIFE_KABAR			|| \
		w == WP_DYNAMITE			|| w == WP_LANDMINE				|| \
		w == WP_SATCHEL				|| w == WP_SATCHEL_DET			|| \
		w == WP_TRIPMINE			|| w == WP_SMOKE_BOMB			|| \
		w == WP_MEDIC_SYRINGE		|| w == WP_MEDIC_ADRENALINE		|| \
		w == WP_PLIERS				|| w == WP_MEDKIT				|| \
		w == WP_AMMO				|| w == WP_BINOCULARS			|| \
		w == WP_SMOKE_MARKER		|| \
		w == WP_POISON_SYRINGE		|| \
		w == WP_FIREBOLT	 )

#define IS_SILENCED_WEAPON(w) \
	(	w == WP_STEN				|| \
		w == WP_GARAND				|| w == WP_K43					|| \
		w == WP_GARAND_SCOPE		|| w == WP_K43_SCOPE			|| \
		w == WP_SILENCED_COLT		|| w == WP_SILENCER				|| \
		w == WP_AKIMBO_SILENCEDCOLT || w == WP_AKIMBO_SILENCEDLUGER )

#define IS_VALID_WEAPON(w) \
	(	w > WP_NONE && w < WP_NUM_WEAPONS )

#define IS_WEAPON_WITH_ALT(w) \
	(	IS_VALID_WEAPON(w) && weapAlts[w] != WP_NONE )

#define GET_ALT_WEAPON(w) \
	(	IS_WEAPON_WITH_ALT(w) ? weapAlts[w] : w )

#define IS_OFFHAND_NADE_WEAPON(w) \
	(   IS_SUBMACHINEGUN(w)			|| w == WP_STEN					|| \
		w == WP_STEN_MKII			|| w == WP_SHOTGUN				|| \
		w == WP_MP34				|| w == WP_STG44				|| \
		w == WP_KNIFE				|| w == WP_KNIFE_KABAR	)

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
		w == WP_LANDMINE			|| \
		w == WP_POISON_SYRINGE		|| \
		w == WP_KNIFE				|| w == WP_KNIFE_KABAR	)


/*
#define IS_EXPLOSIVE_MOD(mod) \
			{	\
				mod == MOD_GRENADE				|| mod == MOD_GRENADE_LAUNCHER	|| \
				mod == MOD_ROCKET				|| mod == MOD_GRENADE_PINEAPPLE	|| \
				mod == MOD_MAPMORTAR			|| mod == MOD_MAPMORTAR_SPLASH 	|| \ 
				mod == MOD_EXPLOSIVE			|| mod == MOD_LANDMINE			|| \ 
				mod == MOD_GPG40				|| mod == MOD_M7				|| \
				mod == MOD_SATCHEL				|| mod == MOD_ARTY				|| \
				mod == MOD_AIRSTRIKE			|| mod == MOD_DYNAMITE			|| \
				mod == MOD_MORTAR				|| mod == MOD_PANZERFAUST		|| \
				mod == MOD_MAPMORTAR			   \
			}
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
	EV_JUMP,
	EV_WATER_TOUCH,				// foot touches
	EV_WATER_LEAVE,				// foot leaves
	EV_WATER_UNDER,				// head touches
	EV_WATER_CLEAR,				// head leaves
	EV_ITEM_PICKUP,				// normal item pickups are predictable
	EV_ITEM_PICKUP_QUIET,		// (SA) same, but don't play the default pickup sound as it was specified in the ent
	EV_GLOBAL_ITEM_PICKUP,		// powerup / team sounds are broadcast to everyone
	EV_NOAMMO,
	EV_WEAPONSWITCHED,
	EV_EMPTYCLIP,
	EV_FILL_CLIP,
	EV_MG42_FIXED,				// JPW NERVE
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
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,
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
	EV_VENOM,
	EV_BULLET,					// otherEntity is the shooter
	EV_LOSE_HAT,				//----(SA)	
	EV_PAIN,
	EV_CROUCH_PAIN,
//	EV_DEATH,
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
	EV_TAUNT,
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
	EV_SPIT_HIT,
	EV_SPIT_MISS,
	EV_SHARD,
	EV_JUNK,
	EV_EMITTER,					//----(SA)	added // generic particle emitter that uses client-side particle scripts
	EV_OILPARTICLES,
	EV_OILSLICK,
	EV_OILSLICKREMOVE,
	EV_MG42EFX,
//	EV_FLAKGUN1,
//	EV_FLAKGUN2,
//	EV_FLAKGUN3,
//	EV_FLAKGUN4,
//	EV_EXERT1,
//	EV_EXERT2,
//	EV_EXERT3,
	EV_SNOWFLURRY,
	EV_CONCUSSIVE,
	EV_DUST,
	EV_RUMBLE_EFX,
	EV_GUNSPARKS,
	EV_FLAMETHROWER_EFFECT,
//	EV_POPUP,
//	EV_POPUPBOOK,
//	EV_GIVEPAGE,
	EV_MG42BULLET_HIT_FLESH,	// Arnout: these two send the seed as well
	EV_MG42BULLET_HIT_WALL,
	EV_SHAKE,
	EV_DISGUISE_SOUND,
	EV_BUILDDECAYED_SOUND,
	EV_FIRE_WEAPON_AAGUN,
	EV_DEBRIS,
	EV_ALERT_SPEAKER,
	EV_POPUPMESSAGE,
	EV_ARTYMESSAGE,
	EV_AIRSTRIKEMESSAGE,
	EV_MEDIC_CALL,
	EV_FIREWORKS,				// jet Pilot - per Lagging Tom's request
	EV_SHOTGUN_PUMP,			// jaquboss
	EV_SPAWN,					// jaquboss
	EV_BODY_DP,
	EV_FIRE_JET,
	EV_BOUNCE_SOUND,
	EV_KICK,
	//EV_KICKED,
	EV_WOLFKICK_MISS,
	EV_WOLFKICK_HIT_FLESH,
	EV_WOLFKICK_HIT_WALL,
	EV_BOT_DEBUG_LINE,
	EV_BOT_DEBUG_RADIUS,
	EV_MAX_EVENTS				// just added as an 'endcap'
} entity_event_t;


// new (10/18/00)
typedef enum {
	BOTH_DEATH1,
	BOTH_DEAD1,
	BOTH_DEAD1_WATER,
	BOTH_DEATH2,
	BOTH_DEAD2,
	BOTH_DEAD2_WATER,
	BOTH_DEATH3,
	BOTH_DEAD3,
	BOTH_DEAD3_WATER,

	BOTH_CLIMB,
/*10*/	BOTH_CLIMB_DOWN,
	BOTH_CLIMB_DISMOUNT,

	BOTH_SALUTE,
	
	BOTH_PAIN1,			// head
	BOTH_PAIN2,			// chest
	BOTH_PAIN3,			// groin
	BOTH_PAIN4,			// right shoulder
	BOTH_PAIN5,			// left shoulder
	BOTH_PAIN6,			// right knee
	BOTH_PAIN7,			// left knee
/*20*/	BOTH_PAIN8,		// dazed

	BOTH_GRAB_GRENADE,

	BOTH_ATTACK1,
	BOTH_ATTACK2,
	BOTH_ATTACK3,
	BOTH_ATTACK4,
	BOTH_ATTACK5,

	BOTH_EXTRA1,
	BOTH_EXTRA2,
	BOTH_EXTRA3,
/*30*/	BOTH_EXTRA4,
	BOTH_EXTRA5,
	BOTH_EXTRA6,
	BOTH_EXTRA7,
	BOTH_EXTRA8,
	BOTH_EXTRA9,
	BOTH_EXTRA10,
	BOTH_EXTRA11,
	BOTH_EXTRA12,
	BOTH_EXTRA13,
/*40*/	BOTH_EXTRA14,
	BOTH_EXTRA15,
	BOTH_EXTRA16,
	BOTH_EXTRA17,
	BOTH_EXTRA18,
	BOTH_EXTRA19,
	BOTH_EXTRA20,

	TORSO_GESTURE,
	TORSO_GESTURE2,
	TORSO_GESTURE3,
/*50*/	TORSO_GESTURE4,

	TORSO_DROP,

	TORSO_RAISE,	// (low)
	TORSO_ATTACK,
	TORSO_STAND,
	TORSO_STAND_ALT1,
	TORSO_STAND_ALT2,
	TORSO_READY,
	TORSO_RELAX,

	TORSO_RAISE2,	// (high)
/*60*/	TORSO_ATTACK2,
	TORSO_STAND2,
	TORSO_STAND2_ALT1,
	TORSO_STAND2_ALT2,
	TORSO_READY2,
	TORSO_RELAX2,

	TORSO_RAISE3,	// (pistol)
	TORSO_ATTACK3,
	TORSO_STAND3,
	TORSO_STAND3_ALT1,
/*70*/	TORSO_STAND3_ALT2,
	TORSO_READY3,
	TORSO_RELAX3,

	TORSO_RAISE4,	// (shoulder)
	TORSO_ATTACK4,
	TORSO_STAND4,
	TORSO_STAND4_ALT1,
	TORSO_STAND4_ALT2,
	TORSO_READY4,
	TORSO_RELAX4,

/*80*/	TORSO_RAISE5,	// (throw)
	TORSO_ATTACK5,
	TORSO_ATTACK5B,
	TORSO_STAND5,
	TORSO_STAND5_ALT1,
	TORSO_STAND5_ALT2,
	TORSO_READY5,
	TORSO_RELAX5,

	TORSO_RELOAD1,	// (low)
	TORSO_RELOAD2,	// (high)
/*90*/	TORSO_RELOAD3,	// (pistol)
	TORSO_RELOAD4,	// (shoulder)

	TORSO_MG42,		// firing tripod mounted weapon animation

	TORSO_MOVE,		// torso anim to play while moving and not firing (swinging arms type thing)
	TORSO_MOVE_ALT,

	TORSO_EXTRA,
	TORSO_EXTRA2,
	TORSO_EXTRA3,
	TORSO_EXTRA4,
	TORSO_EXTRA5,
/*100*/	TORSO_EXTRA6,
	TORSO_EXTRA7,
	TORSO_EXTRA8,
	TORSO_EXTRA9,
	TORSO_EXTRA10,

	LEGS_WALKCR,
	LEGS_WALKCR_BACK,
	LEGS_WALK,
	LEGS_RUN,
	LEGS_BACK,
/*110*/	LEGS_SWIM,
	LEGS_SWIM_IDLE,

	LEGS_JUMP,
	LEGS_JUMPB,
	LEGS_LAND,

	LEGS_IDLE,
	LEGS_IDLE_ALT,// LEGS_IDLE2
	LEGS_IDLECR,

	LEGS_TURN,

	LEGS_BOOT,		// kicking animation

/*120*/	LEGS_EXTRA1,
	LEGS_EXTRA2,
	LEGS_EXTRA3,
	LEGS_EXTRA4,
	LEGS_EXTRA5,
	LEGS_EXTRA6,
	LEGS_EXTRA7,
	LEGS_EXTRA8,
	LEGS_EXTRA9,
	LEGS_EXTRA10,

/*130*/	MAX_ANIMATIONS
} animNumber_t;

// text represenation for scripting
extern char *animStrings[];		// defined in bg_misc.c
extern char *animStringsOld[];		// defined in bg_misc.c


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

	//
	// derived
	//
	int			duration;
	int			nameHash;
	int			flags;
	int			movetype;
	int			time; // jaquboss, it is easier to have it computed while loading
} animation_t;

// Ridah, head animations
typedef enum {
	HEAD_NEUTRAL_CLOSED,
	HEAD_NEUTRAL_A,
	HEAD_NEUTRAL_O,
	HEAD_NEUTRAL_I,
	HEAD_NEUTRAL_E,
	HEAD_HAPPY_CLOSED,
	HEAD_HAPPY_O,
	HEAD_HAPPY_I,
	HEAD_HAPPY_E,
	HEAD_HAPPY_A,
	HEAD_ANGRY_CLOSED,
	HEAD_ANGRY_O,
	HEAD_ANGRY_I,
	HEAD_ANGRY_E,
	HEAD_ANGRY_A,

	MAX_HEAD_ANIMS
} animHeadNumber_t;

typedef struct headAnimation_s {
	int		firstFrame;
	int		numFrames;
} headAnimation_t;
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
	WS_KNIFE,				// 0
	WS_LUGER,				// 1
	WS_COLT,				// 2
	WS_MP40,				// 3
	WS_THOMPSON,			// 4
	WS_STEN,				// 5
	WS_FG42,				// 6	-- Also includes WS_BAR (allies version of fg42)
	WS_PANZERFAUST,			// 7
	WS_FLAMETHROWER,		// 8
	WS_GRENADE,				// 9	-- Includes axis and allies grenade types
	WS_MORTAR,				// 10
	WS_DYNAMITE,			// 11
	WS_AIRSTRIKE,			// 12	-- Lt. smoke grenade attack
	WS_ARTILLERY,			// 13	-- Lt. binocular attack
	WS_SYRINGE,				// 14	-- Medic syringe uses/successes

	WS_SMOKE,				// 15
	WS_SATCHEL,				// 16
	WS_GRENADELAUNCHER,		// 17
	WS_LANDMINE,			// 18
	WS_MG42,				// 19
	WS_GARAND,				// 20 // Gordon: (carbine and garand)
	WS_K43,					// 21 // Gordon: (kar98 and k43)

	WS_SHOTGUN,				// 22 (jet Pilot - Shotgun)
	WS_BROWNING,			// 23 (jet Pilot - Mobile Browning)
//	WS_BAYONETTE,			

	WS_STG44,				// 24 (jet Pilot - German StG44 Assault Rifle)
//  WS_DOUBLE_SHOUTGUN,		//    (jet Pilot - Double Barreled Shotgun)
	WS_STEN_MKII,			// 25 (jet Pilot - Sten Mk II - Unsilenced) // jaquboss - man do we need TWO for almost same ?!
	WS_BAR,					// 26 (jet Pilot - BAR )
	WS_BAZOOKA,				// 27 (jet Pilot - Bazoka )
	WS_MP34,				// 28 (jet Pilot - Solothurn S1-100 )
	WS_VENOM,
	WS_FIREBOLT,
	WS_POISON,

	WS_MAX
} extWeaponStats_t;

typedef struct {
	qboolean fHasHeadShots;
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
	MOD_ROCKET,

	// (SA) modified wolf weap mods
	MOD_KNIFE,
	MOD_LUGER,
	MOD_COLT,
	MOD_MP40,
	MOD_THOMPSON,
	MOD_STEN,
	MOD_GARAND,
	MOD_SNOOPERSCOPE,
	MOD_SILENCER,	//----(SA)	
	MOD_FG42,
	MOD_FG42SCOPE,
	MOD_PANZERFAUST,
	MOD_GRENADE_LAUNCHER,
	MOD_FLAMETHROWER,
	MOD_GRENADE_PINEAPPLE,
	MOD_CROSS,
	// end

	MOD_MAPMORTAR,
	MOD_MAPMORTAR_SPLASH,

	MOD_KICKED,
	MOD_GRABBER,

	MOD_DYNAMITE,
	MOD_AIRSTRIKE, // JPW NERVE
	MOD_SYRINGE,	// JPW NERVE
	MOD_AMMO,	// JPW NERVE
	MOD_ARTY,	// JPW NERVE

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
	MOD_TRIPMINE,
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

	// RF
	MOD_SWAP_PLACES,

	// OSP -- keep these 2 entries last
	MOD_SWITCHTEAM,

	MOD_GOOMBA,
	MOD_POISON,
	MOD_FEAR,

	// jet Pilot - new means of death (MOD)
	MOD_CENSORED,
	MOD_BLEEDING,

	MOD_SHOTGUN,	
//	MOD_SHOTGUN_DOUBLE,
//	MOD_BAYONETTE,
	MOD_BACKSTAB,
	MOD_MOBILE_BROWNING,
	MOD_BAR,
	MOD_STG44,
	MOD_BAZOOKA,
	MOD_STEN_MKII,
	MOD_MP34,
	MOD_FIREBOLT,
	MOD_VENOM,
	MOD_JETGUN,
	MOD_COLISION,
	MOD_SHOVE,
	
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
	IT_HOLDABLE,			// single use, holdable item
							// EFX: rotate + bob
	IT_KEY,
	IT_TREASURE,			// gold bars, etc.  things that can be picked up and counted for a tally at end-level
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

	char		*precaches;		// string of all models and images this item will use
	char		*sounds;		// string of all sounds this item will use

//	int			gameskillnumber[5];
} gitem_t;
// END JOSEPH

// included in both the game dll and the client
extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

gitem_t	*BG_FindItem( const char *pickupName );
gitem_t	*BG_FindItemForClassName( const char *className );
gitem_t	*BG_FindItemForWeapon	( weapon_t weapon );
gitem_t	*BG_FindItemForPowerup	( powerup_t pw );
gitem_t	*BG_FindItemForHoldable	( holdable_t pw );
gitem_t *BG_FindItemForAmmo		( int weapon );
//gitem_t *BG_FindItemForKey		( wkey_t k, int *index );
weapon_t BG_FindAmmoForWeapon	( weapon_t weapon );
weapon_t BG_FindClipForWeapon	( weapon_t weapon );

qboolean BG_AkimboFireSequence( int weapon, int akimboClip, int mainClip );
qboolean BG_IsAkimboWeapon( int weaponNum );
qboolean BG_IsAkimboSideArm( int weaponNum, playerState_t *ps );
int BG_AkimboSidearm( int weaponNum );

#define	ITEM_INDEX(x) ((x)-bg_itemlist)

qboolean BG_CanUseWeapon(int classNum, int teamNum, weapon_t weapon);

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
	HINT_TREASURE,
	HINT_KNIFE,
	HINT_LADDER,
	HINT_BUTTON,
	HINT_WATER,
//	HINT_CAUTION,
//	HINT_DANGER,
//	HINT_SECRET,
//	HINT_QUESTION,
//	HINT_EXCLAMATION,
//	HINT_CLIPBOARD,
	HINT_WEAPON,
	HINT_AMMO,
	HINT_ARMOR,
	HINT_POWERUP,
	HINT_HOLDABLE,
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
//	HINT_LOCKPICK,
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

//void	BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, int time, qboolean snap );
void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, qboolean snap );
weapon_t BG_DuplicateWeapon( weapon_t weap );
gitem_t* BG_ValidStatWeapon( weapon_t weap );
weapon_t BG_WeaponForMOD( int MOD );

qboolean	BG_WeaponInWolfMP( int weapon );
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );
qboolean	BG_PlayerSeesItem	( playerState_t *ps, entityState_t *item, int atTime );
qboolean	BG_AddMagicAmmo ( playerState_t *ps, int *skill, int teamNum, int numOfClips );

#define	OVERCLIP		1.001

//----(SA)	removed PM_ammoNeeded 11/27/00
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce );

//#define ARENAS_PER_TIER		4
#define MAX_ARENAS			64
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			64
#define MAX_BOTS_TEXT		8192

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
	FXTYPE_STONE
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
//	ANIM_MT_TALK,
//	ANIM_MT_SNEAK,
//	ANIM_MT_AFTERBATTLE,			// xkan, 1/8/2003, just finished battle
	ANIM_MT_REVIVE,
	ANIM_MT_RADIO,
	ANIM_MT_RADIOCR,
	ANIM_MT_RADIOPRONE,
//#ifdef __SELFDEFENSE__
	ANIM_MT_DEFENDING,
//#endif
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
	ANIM_ET_PICKUPGRENADE,
	ANIM_ET_KICKGRENADE,
	ANIM_ET_QUERY,
	ANIM_ET_INFORM_FRIENDLY_OF_ENEMY,
	ANIM_ET_KICK,
	ANIM_ET_REVIVE,
	ANIM_ET_FIRSTSIGHT,
	ANIM_ET_ROLL,
	ANIM_ET_FLIP,
	ANIM_ET_DIVE,
	ANIM_ET_PRONE_TO_CROUCH,
	ANIM_ET_BULLETIMPACT,
	ANIM_ET_INSPECTSOUND,
	ANIM_ET_SECONDLIFE,
	ANIM_ET_DO_ALT_WEAPON_MODE,
	ANIM_ET_UNDO_ALT_WEAPON_MODE,
	ANIM_ET_DO_ALT_WEAPON_MODE_PRONE,
	ANIM_ET_UNDO_ALT_WEAPON_MODE_PRONE,
	ANIM_ET_FIREWEAPONPRONE,
	ANIM_ET_FIREWEAPON2PRONE,
	ANIM_ET_RAISEWEAPONPRONE,
	ANIM_ET_RELOADPRONE,
	ANIM_ET_TALK,
	ANIM_ET_NOPOWER,

	ANIM_ET_LEANR,
	ANIM_ET_LEANL,
	ANIM_ET_SALUTE,

	ANIM_ET_PAIN_HEAD,
	ANIM_ET_PAIN_CHEST,
	ANIM_ET_PAIN_GUT, 
	ANIM_ET_PAIN_GROIN, 
	ANIM_ET_PAIN_SHOULDER_RIGHT,
	ANIM_ET_PAIN_SHOULDER_LEFT,
	ANIM_ET_PAIN_KNEE_RIGHT,
	ANIM_ET_PAIN_KNEE_LEFT,
	ANIM_ET_PAIN_FIRE,
	ANIM_ET_PAIN_STUN,

	ANIM_ET_DEATH_FROM_BEHIND,
	ANIM_ET_DEATH_FROM_ABOVE,

	ANIM_ET_PLANT_LANDMINE,
	ANIM_ET_FIRING_BAYONETTE,
	ANIM_ET_STAND_RADIO,
	ANIM_ET_IMPLORE_MEDIC,

	ANIM_ET_OVERHAND_THROW,
	ANIM_ET_UNDERHAND_THROW,
	ANIM_ET_CHEER,

	ANIM_ET_RELOAD_SG1,
	ANIM_ET_RELOAD_SG2,
	ANIM_ET_RELOAD_SG3,
	ANIM_ET_RELOADPRONE_SG1,
	ANIM_ET_RELOADPRONE_SG2,
	ANIM_ET_RELOADPRONE_SG3,

	ANIM_ET_DEFEND_RAISE,
	ANIM_ET_DEFEND_LOWER,

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
	headAnimation_t		headAnims[MAX_HEAD_ANIMS];
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
//	int					clientModels[MAX_CLIENTS];		// so we know which model each client is using
	animModelInfo_t		modelInfo[MAX_ANIMSCRIPT_MODELS];
	int					clientConditions[MAX_CLIENTS][NUM_ANIM_CONDITIONS][2];
	//
	// pointers to functions from the owning module
	//
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
	MOUNTED_AAGUN,

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
/*	ANIM_BITFLAG_SNEAKING,
	ANIM_BITFLAG_AFTERBATTLE,*/
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

#ifdef __SSYSTEM__
#define MAX_CHARACTER_INDEXES 8 // jaquboss - for now, I don't really want small limits so this will be changed ;)

// model and skins allowed for it
typedef struct	bodyModel_s {
	qhandle_t		mesh;
	qhandle_t		torsoSkin[MAX_CHARACTER_INDEXES];
	int				numTorsoSkins;
	qhandle_t		legsSkin[MAX_CHARACTER_INDEXES];
	int				numLegsSkins;
} bodyModel_t;

// for head
typedef struct	headModel_s {
	qhandle_t		model;
	qhandle_t		skin[MAX_CHARACTER_INDEXES];
	animation_t		animations[MAX_HD_ANIMATIONS];
	int				numSkins;
} headModel_t;

// accesory
typedef struct	accModel_s {
	qhandle_t		model;
//	accType_t		tag;
	qhandle_t		skin[MAX_CHARACTER_INDEXES];
	int				numSkins;
} accModel_t;

// from cgame
typedef struct {
	char		*type;
	accType_t	index;
} acc_t;

extern acc_t bg_accessories[];

#define NUM_ACC  8

// every player should have this one, on server there is practly needed only bodyIndex..
typedef struct playerCharacter_s{
	int		headIndex;			// index of head MODEL
	int		faceIndex;			// index of head SKIN
	int		bodyIndex;			// index of player MODEL
	int		torsoIndex;			// index of torso SKIN
	int		legIndex;			// index of legs SKIN
	int		helmetIndex;		// index of helmet MODEL
	int		helmetSkinIndex;	// index of helmet SKIN
	int		backPackIndex;		// index of tag_back accesory MODEL
	int		backPackSkinIndex;	// index of tag_back accesory SKIN
	int		bLeftIndex;			// index of tag_bleft accesory MODEL
	int		bLeftSkinIndex;		// index of tag_bleft accesory SKIN
	int		bRightIndex;		// index of tag_bright accesory MODEL
	int		bRightSkinIndex;	// index of tag_bright accesory SKIN
	int		uBeltIndex;			// index of tag_ubelt accesory MODEL
	int		uBeltSkinIndex;		// index of tag_ubelt accesory SKIN
//	int		lBeltIndex;			// index of tag_lbelt accesory MODEL
//	int		helmetShieldIndex;	// md3_hat2 / ACC_MOUTH2 MODEL -- jaquboss ATM force the same
	int		glassIndex;			// md3_hat3 / ACC_MOUTH3 MODEL
	int		glassSkinIndex;		// md3_hat3 / ACC_MOUTH3 SKIN
} playerCharacter_t;
#endif

typedef struct bg_character_s
{
	char				characterFile[MAX_QPATH];
#ifdef USE_MDXFILE
#ifndef __SSYSTEM__
	qhandle_t			mesh;
	qhandle_t			skin;
	qhandle_t			accModels[ACC_MAX];
	qhandle_t			accSkins[ACC_MAX];
	qhandle_t			hudhead;
	qhandle_t			hudheadskin;
	animation_t			hudheadanimations[MAX_HD_ANIMATIONS];
#else
	bodyModel_t			body[MAX_CHARACTER_INDEXES];
	headModel_t			head[MAX_CHARACTER_INDEXES];
	accModel_t			acc[ACC_MAX][MAX_CHARACTER_INDEXES];
	int					numHeads;
	int					numBodies;
	int					numAcc[ACC_MAX];
#endif
	qhandle_t			gibModels[MAX_GIB_MODELS];
	qhandle_t			undressedCorpseModel;
	qhandle_t			undressedCorpseSkin;
#endif // USE_MDXFILE
	animModelInfo_t*	animModelInfo;
} bg_character_t;

/*
==============================================================

SAVE

	12 -
	13 - (SA) added 'episode' tracking to savegame
	14 - RF added 'skill'
	15 - (SA) moved time info above the main game reading
	16 - (SA) added fog
	17 - (SA) rats, changed fog.
	18 - TTimo targetdeath fix
       https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=434
	30 - Arnout: initial Enemy Territory implementation
	31 - Arnout: added new global fog

==============================================================
*/

#define	SAVE_VERSION			31
#define	SAVE_INFOSTRING_LENGTH	256

//------------------------------------------------------------------
// Global Function Decs

//animModelInfo_t *BG_ModelInfoForModelname( char *modelname );
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
	//ME_LANDMINE_ARMED,
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

pathCorner_t *BG_Find_PathCorner (const char *match);
splinePath_t* BG_GetSplineData( int number, qboolean* backwards );
void BG_AddPathCorner(const char* name, vec3_t origin);
splinePath_t* BG_AddSplinePath(const char* name, const char* target, vec3_t origin);
void BG_BuildSplinePaths();
splinePath_t *BG_Find_Spline (const char *match);
float BG_SplineLength(splinePath_t* pSpline);
void BG_AddSplineControl(splinePath_t* spline, const char* name);
void BG_LinearPathOrigin2(float radius, splinePath_t** pSpline, float *deltaTime, vec3_t result, qboolean backwards);

int BG_MaxAmmoForWeapon( weapon_t weaponNum, int *skill );

void BG_InitLocations( vec2_t world_mins, vec2_t world_maxs );
char *BG_GetLocationString( vec_t* pos );

// START Mad Doc - TDF
typedef struct botpool_x
{
	int num;
	int playerclass;
	int rank;
	struct botpool_x *next;
} botpool_t;

// END Mad Doc - TDF

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
void BG_TransposeMatrix(const vec3_t matrix[3], vec3_t transpose[3]);
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

void BG_AdjustAAGunMuzzleForBarrel( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up, int barrel );

int BG_ClassTextToClass(char *token);
skillType_t BG_ClassSkillForClass( int classnum );

qboolean BG_isLightWeaponSupportingFastReload( int weapon );
qboolean BG_IsScopedWeapon( int weapon );

int BG_FootstepForSurface( int surfaceFlags );

#define MATCH_MINPLAYERS "4"//"1"	// Minimum # of players needed to start a match

// Multiview support
int BG_simpleHintsCollapse(int hint, int val);
int BG_simpleHintsExpand(int hint, int val);
int BG_simpleWeaponState(int ws);

// Color escape handling
int BG_colorstrncpyz(char *in, char *out, int str_max, int out_max);
int BG_drawStrlen(const char *str);
int BG_strRelPos(char *in, int index);
// CHRUKER: b069 - Cleaned up a few compiler warnings
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
#ifdef __SSYSTEM__
typedef struct bodyDef_s {
	char		mesh[MAX_QPATH];
	char		torsoSkin[MAX_CHARACTER_INDEXES][MAX_QPATH];
	char		legsSkin[MAX_CHARACTER_INDEXES][MAX_QPATH];
	int			numLegsSkins;
	int			numTorsoSkins;
} bodyDef_t;

typedef struct headDef_s {
	char		model[MAX_QPATH];
	char		skin[MAX_CHARACTER_INDEXES][MAX_QPATH];
	int			numSkins;
	char		animations[MAX_QPATH];
} headDef_t;

typedef struct accDef_s {
	char		model[MAX_QPATH];
	//accType_t	tag;
	char		skin[MAX_CHARACTER_INDEXES][MAX_QPATH];
	int			numSkins;
	qboolean	registred;
} accDef_t;
#endif

typedef struct bg_characterDef_s {
	// common
	char		undressedCorpseModel[MAX_QPATH];
	char		undressedCorpseSkin[MAX_QPATH];
	char		animationGroup[MAX_QPATH];
	char		animationScript[MAX_QPATH];
#ifdef __SSYSTEM__
	bodyDef_t	body[MAX_CHARACTER_INDEXES];
	headDef_t	head[MAX_CHARACTER_INDEXES];
	accDef_t	acc[ACC_MAX][MAX_CHARACTER_INDEXES];
	int			numHeads;
	int			numBodies;
	int			numAcc[ACC_MAX];
#else 
	char		mesh[MAX_QPATH];
	char		skin[MAX_QPATH];
	char		hudhead[MAX_QPATH];
	char		hudheadskin[MAX_QPATH];
	char		hudheadanims[MAX_QPATH];
#endif 
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
extern ammotable_t ammoTableMP[WP_NUM_WEAPONS];
#define GetAmmoTableData(ammoIndex) ((ammotable_t*)(&ammoTableMP[ammoIndex]))

#define MAX_MAP_SIZE 65536

qboolean BG_BBoxCollision( vec3_t min1, vec3_t max1, vec3_t min2, vec3_t max2 );

//#define VISIBLE_TRIGGERS

//
// bg_stats.c
//

int BG_GetAuraLevel(int cls, playerState_t *ps);


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
#define JP_KEEP_ADRENALINE		0x0002
#define JP_KEEP_RECOGNITION		0x0004
#define JP_KEEP_ASSASSIN		0x0008
//#define JP_KEEP_SMG_SIDEARM	0x0010	// Jaybird - not used
#define JP_KEEP_MINES			0x0010	// keep covert ops landmine spotting ability 



#define JP_DRAWBB_OBJECTIVES	0x0001
#define JP_DRAWBB_PLAYERS		0x0002
#define JP_DRAWBB_PLAYERHEADS	0x0004
#define JP_DRAWBB_MISSILES		0x0008
#define JP_DRAWBB_BREAKABLES	0x0010

#define JP_WEAPON_SHOTGUN		0x0001	
#define JP_WEAPON_BAYONETTE		0x0002


#define JP_INSANITY_SPAWN       0x0001	// 1	- players spawn instantly upon death
#define JP_INSANITY_AMMUNITION  0x0002	// 2	- players have unlimited ammo clips
#define JP_INSANITY_MORTARCAM	0x0004	// 4	- mortar cam
#define JP_INSANITY_MISSILECAM	0x0008	// 8	- missile cam
#define JP_INSANITY_CHARGEBAR   0x0010	// 16 	- players have unlimited charge bars
#define JP_INSANITY_GUIDED		0x0020	// 32	- rockets can be steered by the firing player
#define JP_INSANITY_HOMING		0x0040	// 64	- rockets are homing
#define JP_INSANITY_ALLWEAPONS  0x0080	// 128  - players can pick up any weapon on the ground
#define JP_INSANITY_VENOM		0x0100	// 256	- lets soldiers choose Venom
#define JP_INSANITY_FIREBOLT	0x0200	// 512	- gives players firebolt

#ifdef __JETPILOT__
#define JP_INSANITY_FLYING		0x0400	// 1024	- players can fly like spectators
#define JP_INSANITY_NUM_MODES	11
#else
#define JP_INSANITY_NUM_MODES	10
#endif



// bitmask for friendly fire modes
#define FRIENDLY_FIRE_NORMAL	0x0001	// 1  	- FF is enabled
#define FRIENDLY_FIRE_HALF		0x0002	// 2  	- FF does half damage to teammates
#define FRIENDLY_FIRE_MIRROR	0x0004	// 4  	- FF does an equal amount of damage to the inflictor
#define FRIENDLY_FIRE_MOVERS	0x0008	// 8  	- FF affects other ents as well (tanks, etc)
#define FRIENDLY_FIRE_BOOSTING  0x0010	// 16 	- FF allows for nade boosts
#define FRIENDLY_FIRE_DISABLED  0x0020	// 32  	- FF is disabled
#define FRIENDLY_FIRE_NOSPAWN   0x0040  // 64   - FF is disabled near your teams spawn


#define ANNOUNCE_KILLINGSPREES	0x0001
#define ANNOUNCE_DEATHSPREES	0x0002
#define ANNOUNCE_MULTIKILL		0x0004

#include "jp_weapons.h"

extern float chargeCost_Repair[NUM_SKILL_LEVELS];
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
extern int   armSpeed_Dynamite[NUM_SKILL_LEVELS];
extern int   disarmSpeed_Dynamite[NUM_SKILL_LEVELS];
extern int   armSpeed_LandMine[NUM_SKILL_LEVELS];
extern int   disarmSpeed_LandMine[NUM_SKILL_LEVELS];
extern int   disarmSpeed_Satchel[NUM_SKILL_LEVELS];
extern int	 artilleryBombs[NUM_SKILL_LEVELS];
extern float pistolRecoil[NUM_SKILL_LEVELS];

extern float disguiseTestCost[NUM_SKILL_LEVELS];

// hijack ET_BOTGOAL_INDICATOR for tagetting pointers
#define ET_TARGETING_POINTER ET_BOTGOAL_INDICATOR
#define ET_ANIMATED_HEALTH   ET_WAYPOINT
#define ET_LOCATION_MARKER	 ET_WAYPOINT

void BG_IndexedSubstring(char *dest, int maxLen, const char *source, int index);

// end of jP additions


// jaquboss
void BG_RegisterWeapon( int weaponNum, qboolean force );

typedef struct BGweaponInfo_s {
	qboolean		registered;
	animation_t		weapAnimations[MAX_WP_ANIMATIONS];

	char		name[32];
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
	int			headShot;
	int			gib;
	int			falloff;

	int			splashdamage;
	int			splashdamage_radius;

} BGweaponInfo_t;

BGweaponInfo_t BG_Weapons[MAX_WEAPONS];
// end

// for charge bar and pmove
qboolean BG_CheckCharge( pmove_t *pm );
float	BG_ChargeUsage( weapon_t weapon, int skill[SK_NUM_SKILLS]);

// tjw: g_hitboxes flags
// jaquboss predict on clients
#define HBF_STANDING				1	// lower hitboxes when standing
#define HBF_CROUCHING				2	// lower hitboxes when crouched
#define HBF_CORPSE					4	// lower hitboxes for corpses
#define HBF_PRONE					8	// lower hitboxes for prone
#define HBF_PLAYDEAD				16	// lower hitboxes for playdead

#define LEAN_VISIBLE				1 // visible leaning
#define LEAN_SHOOT					2 // can shoot while leaning (1 flag must be set)
#define LEAN_RUN					4 // can run while leaning

#define ADR_GLOWEYES				1 // adrenaline glowing eyes
#define ADR_NOSTAMINA				2 // adren doesn't double stamina
#define ADR_NOREDUCT				4 // adren doesn't half inflicted

#define	SPEC_PLAYERGLOW 16	// players will have colored glow arround them
#define	SPEC_DLIGHT 32		// draw dlight at view origin
#define	SPEC_DLIGHT_GLOBAL 64	// draw dlight over whole map
#define	SPEC_SHOWNAMES	128	// show player names above their heads

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

typedef	enum WeaponBool_e {
	WEAPON_DEFAULT,
	WEAPON_YES,
	WEAPON_NO
} WeaponBool_t;

// jaquboss

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

sysMessage_t systemMessages[SM_NUM_SYS_MSGS];

#endif
