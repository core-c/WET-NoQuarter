/*
 * name:	g_local.h
 *
 * desc:	local definitions for game module
 *
 * NQQS:	healthy
 *
 */
#ifndef __G_LOCAL_H__
#define __G_LOCAL_H__

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"

//==================================================================

// the "gameversion" client command will print this plus compile date
#ifndef PRE_RELEASE_DEMO
#define GAMEVERSION			"nq"
#else
//#define GAMEVERSION			"You look like you need a monkey!"
#define GAMEVERSION			"ettest"
#endif // PRE_RELEASE_DEMO

#define BODY_QUEUE_SIZE		8

#define	EVENT_VALID_MSEC	300
#define	CARNAGE_REWARD_TIME	3000

#define	INTERMISSION_DELAY_TIME	1000

#define MG42_MULTIPLAYER_HEALTH 350				// JPW NERVE

#define USE_BODY_QUE

// How long do bodies last?
// SP : Axis: 20 seconds
//		Allies: 30 seconds
// MP : Both 10 seconds

#ifdef USE_BODY_QUE
#define BODY_TIME 20000
#else
#define BODY_TIME 10000	// jaquboss, old ET time, since there is 'unlimited' number of bodies now
#endif

// jaquboss

#define	REVIVE_DECAYRATE		8
#define	REVIVE_HEALRATE			3

#define MAX_MG42_HEAT			1500.f

// How often do we make Aura checks? Lucel
#define NQ_AURA_FRAME_FREQ		20
#define NQ_AURA_RADIUS			512

// gentity->flags
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_DROPPED_ITEM			0x00001000
#define FL_NO_BOTS				0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS			0x00004000	// spawn point just for bots
#define	FL_AI_GRENADE_KICK		0x00008000	// an AI has already decided to kick this grenade
// Rafael
#define FL_NOFATIGUE			0x00010000	// cheat flag no fatigue

#define FL_TOGGLE				0x00020000	//----(SA)	ent is toggling (doors use this for ex.)
#define FL_KICKACTIVATE			0x00040000	//----(SA)	ent has been activated by a kick (doors use this too for ex.)
#define	FL_SOFTACTIVATE			0x00000040	//----(SA)	ent has been activated while 'walking' (doors use this too for ex.)
#define	FL_DEFENSE_GUARD		0x00080000	// warzombie defense pose

#define	FL_BLANK				0x00100000
#define	FL_BLANK2				0x00200000
#define	FL_NO_MONSTERSLICK		0x00400000
#define	FL_NO_HEADCHECK			0x00800000

#define	FL_NODRAW				0x01000000
#define FL_SILENCER				0x02000000	// jaquboss - pistol with silencer

#define TKFL_MINES				0x00000001
#define TKFL_AIRSTRIKE			0x00000002
#define TKFL_MORTAR				0x00000004

// movers are things like doors, plats, buttons, etc
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_POS3,
	MOVER_1TO2,
	MOVER_2TO1,
	MOVER_2TO3,
	MOVER_3TO2,
	MOVER_POS1ROTATE,
	MOVER_POS2ROTATE,
	MOVER_1TO2ROTATE,
	MOVER_2TO1ROTATE
} moverState_t;

// door AI sound ranges
#define HEAR_RANGE_DOOR_LOCKED		128	// really close since this is a cruel check
#define HEAR_RANGE_DOOR_KICKLOCKED	512
#define HEAR_RANGE_DOOR_OPEN		256
#define HEAR_RANGE_DOOR_KICKOPEN	768

// DHM - Nerve :: Worldspawn spawnflags to indicate if a gametype is not supported
#define NO_GT_WOLF		1
#define NO_STOPWATCH	2
#define NO_CHECKPOINT	4
#define NO_LMS			8

#define MAX_CONSTRUCT_STAGES 3

#define ALLOW_AXIS_TEAM			1
#define ALLOW_ALLIED_TEAM		2
#define ALLOW_DISGUISED_CVOPS	4

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

//====================================================================



// core: structures for parsing script-command params (at mapstart)..
typedef struct
{
	// command arguments
	int			data_int[8];	// if we need more space for more storage,
	float		data_float[8];	// then increase the array-size..
	char		*data_char[8];	// ..but mind the POOLSIZE (G_Alloc()).
} g_script_preparsed_params_t;

// Scripting, these structure are not saved into savegames (parsed each start)
typedef struct
{
	char	*actionString;
	qboolean (*actionFunc)(gentity_t *ent, char *params);
	int		hash;
} g_script_stack_action_t;

typedef struct
{
	// set during script parsing
	g_script_stack_action_t		*action;			// points to an action to perform
	char						*params;
	g_script_preparsed_params_t	*parsed_params;		// core: params pre-parsed at mapstart..
} g_script_stack_item_t;

// Gordon: need to up this, forest has a HUGE script for the tank.....
//#define	G_MAX_SCRIPT_STACK_ITEMS	128
//#define	G_MAX_SCRIPT_STACK_ITEMS	176
// RF, upped this again for the tank
// Gordon: and again...
#define	G_MAX_SCRIPT_STACK_ITEMS	196

typedef struct
{
	g_script_stack_item_t	items[G_MAX_SCRIPT_STACK_ITEMS];
	int						numItems;
} g_script_stack_t;

typedef struct
{
	int					eventNum;			// index in scriptEvents[]
	char				*params;			// trigger targetname, etc
	int					paramshash;			// core: params hash value
	g_script_stack_t	stack;
} g_script_event_t;

typedef struct
{
	char		*eventStr;
	qboolean	(*eventMatch)( g_script_event_t *event, char *eventParm );
	int			hash;
} g_script_event_define_t;
//
// Script Flags
#define	SCFL_GOING_TO_MARKER	0x1
#define	SCFL_ANIMATING			0x2
#define SCFL_FIRST_CALL			0x4

// Scripting Status (NOTE: this MUST NOT contain any pointer vars)
typedef struct
{
	int		scriptStackHead, scriptStackChangeTime;
	int		scriptEventIndex;	// current event containing stack of actions to perform
	// scripting system variables
	int		scriptId;				// incremented each time the script changes
	int		scriptFlags;
	int		actionEndTime;			// time to end the current action
	char	*animatingParams;		// Gordon: read 8 lines up for why i love this code ;)
} g_script_status_t;

#define	G_MAX_SCRIPT_ACCUM_BUFFERS 10

// core: Do not change the order of this enum.
// If You do, then alsoadjust G_Script_GetEventIndex() code accordingly..
typedef enum {
	SE_SPAWN,
	SE_TRIGGER,
	SE_PAIN,
	SE_DEATH,
	SE_ACTIVATE,
	SE_STOPCAM,
	SE_PLAYERSTART,
	SE_BUILT,
	SE_BUILDSTART,
	SE_DECAYED,
	SE_DESTROYED,
	SE_REBIRTH,
	SE_FAILED,
	SE_DYNAMITED,
	SE_DEFUSED,
	SE_MG42,
	SE_MESSAGE,
	SE_EXPLODED,
	SE_NUM_SCRIPTEVENTS
} gScriptEvent_t;
//====================================================================

typedef struct g_constructible_stats_s {
	float	chargebarreq;
	float	constructxpbonus;
	float	destructxpbonus;
	int		health;
	int		weaponclass;
	int		duration;
} g_constructible_stats_t;

#define NUM_CONSTRUCTIBLE_CLASSES	3

extern g_constructible_stats_t g_constructible_classes[NUM_CONSTRUCTIBLE_CLASSES];

qboolean G_WeaponIsExplosive(  meansOfDeath_t mod );
int G_GetWeaponClassForMOD( meansOfDeath_t mod );

//====================================================================

#define MAX_NETNAME			36

#define	CFOFS(x) ((int)&(((gclient_t *)0)->x))

#define MAX_COMMANDER_TEAM_SOUNDS 16

//unlagged - true ping
#define NUM_PING_SAMPLES 32
//unlagged - true ping

typedef struct commanderTeamChat_s {
	int index;
} commanderTeamChat_t;

// MAPVOTE
typedef struct {
	char bspName[128];
	int numVotes;
	int timesPlayed;
	int lastPlayed;
	int totalVotes;
	int voteEligible;
	int zOrder;
} mapVoteInfo_t;

// forty - realistic hitboxes
//         based on lerpFrame_t
typedef struct {

	qhandle_t	oldFrameModel;
	qhandle_t	frameModel;

	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on
	int			oldFrameSnapshotTime;

	vec3_t		oldFramePos;

	int			frame;
	int			frameTime;		// time when ->frame will be exactly on

	float		yawAngle;
	int			yawing;
	float		pitchAngle;
	int			pitching;

	int			moveSpeed;

	int			animationNumber;	// may include ANIM_TOGGLEBIT
	int			oldAnimationNumber;	// may include ANIM_TOGGLEBIT
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact
	float		animSpeedScale;

} glerpFrame_t;

struct gentity_s {
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s	*client;			// NULL if not a client

	qboolean	inuse;

	vec3_t		instantVelocity;	// ydnar: per entity instantaneous velocity, set per frame

	char		*classname;			// set in QuakeEd
	int			classnamehash;		// core: for faster classname comparison..
	int			spawnflags;			// set in QuakeEd

	qboolean	neverFree;			// if true, FreeEntity will only unlink
									// bodyque uses this

	int			flags;				// FL_* variables

	char		*model;
	char		*model2;
	int			freetime;			// level.time when the object was freed

	int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
	qboolean	freeAfterEvent;
	qboolean	unlinkAfterEvent;

	qboolean	physicsObject;		// if true, it can be pushed by movers and fall off edges
									// all game items are physicsObjects,
	float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce

	qboolean	physicsSlide;
	qboolean	physicsFlush;			// if true object will never be flushed ( realigned to fit the terrain slope )

	int			clipmask;			// brushes with this content value will be collided against
									// when moving.  items and corpses do not collide against
									// players, for instance

	int			realClipmask;		// Arnout: use these to backup the contents value when we go to state under construction
	int			realContents;
	qboolean	realNonSolidBModel;	// For script_movers with spawnflags 2 set

	// movers
	moverState_t moverState;
	int			soundPos1;
	int			sound1to2;
	int			sound2to1;
	int			soundPos2;
	int			soundLoop;
	int			sound2to3;
	int			sound3to2;
	int			soundPos3;

	int			soundSoftopen;
	int			soundSoftendo;
	int			soundSoftclose;
	int			soundSoftendc;

	int			soundKickopen;
	int			soundKickendo;

	gentity_t	*parent;
	gentity_t	*nextTrain;
	gentity_t	*prevTrain;
	vec3_t		pos1, pos2, pos3;

	char		*message;

	int			timestamp;		// body queue sinking, etc

	float		angle;			// set in editor, -1 = up, -2 = down

	char		*target;
	int			targethash;		// core: adding a hash for this for faster lookups

	char		*targetname;
	int			targetnamehash; // Gordon: adding a hash for this for faster lookups

	char		*team;
	gentity_t	*target_ent;

	float		speed;
	float		closespeed;		// for movers that close at a different speed than they open
	vec3_t		movedir;

	int			gDuration;
	int			gDurationBack;
	vec3_t		gDelta;
	vec3_t		gDeltaBack;

	int			nextthink;
	void		(*free)(gentity_t *self);
	void		(*think)(gentity_t *self);
	void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint
	void		(*blocked)(gentity_t *self, gentity_t *other);
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage, vec3_t point);
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel

	int			health;

	qboolean	takedamage;

	int			damage;
	int			splashDamage;	// quad will increase this without increasing radius
	int			splashRadius;
	int			methodOfDeath;
	int			splashMethodOfDeath;

	int			count;
	int			count3;

	gentity_t	*chain;
	gentity_t	*enemy;
	gentity_t	*activator;
	gentity_t	*teamchain;		// next entity in team
	gentity_t	*teammaster;	// master of the team

	meansOfDeath_t	deathType;

	int			watertype;
	int			waterlevel;

	int			noise_index;

	// timing variables
	float		wait;
	float		random;

	// Rafael - sniper variable
	// sniper uses delay, random, radius
	int			radius;
	float		delay;

	// JOSEPH 10-11-99
	int			TargetFlag;
	float		duration;
	vec3_t		rotate;
	vec3_t		TargetAngles;
	// END JOSEPH

	gitem_t		*item;			// for bonus items

	// Ridah, AI fields
	char		*aiName;
	void		(*AIScript_AlertEntity)( gentity_t *ent );
	// done.

	char		*aiSkin;

	vec3_t		dl_color;
	char		*dl_stylestring;
	char		*dl_shader;
	int			dl_atten;

	int			key;			// used by:  target_speaker->nopvs,

	qboolean	active;

	// Rafael - mg42
	float		harc;
	float		varc;

	int			props_frame_state;

	// Ridah
	int			missionLevel;		// mission we are currently trying to complete
									// gets reset each new level
	int			start_size;
	int			end_size;

	// Rafael props

	qboolean	isProp; // team killed

	int			mg42BaseEnt;

	gentity_t	*melee;

	char		*spawnitem;

	int			flameQuota, flameQuotaTime, flameBurnEnt;

	int			count2;

	int			grenadeExplodeTime;	// we've caught a grenade, which was due to explode at this time
	int			grenadeFired;		// the grenade entity we last fired

	char		*track;

	// entity scripting system
	char		*scriptName;
	int			scriptnamehash;		// core: for faster scriptname comparison..

	int					numScriptEvents;
	g_script_event_t	*scriptEvents;	// contains a list of actions to perform for each event type
	g_script_status_t	scriptStatus;	// current status of scripting
	// the accumulation buffer
	int scriptAccumBuffer[G_MAX_SCRIPT_ACCUM_BUFFERS];

	float		accuracy;

	char		tagName[MAX_QPATH];		// name of the tag we are attached to
	gentity_t	*tagParent;
	gentity_t	*tankLink;

	int			lastHintCheckTime;			// DHM - Nerve
	int			voiceChatSquelch;			// DHM - Nerve
	int			voiceChatPreviousTime;		// DHM - Nerve
	int			lastBurnedFrameNumber;		// JPW - Nerve   : to fix FT instant-kill exploit

	entState_t	entstate;
	char		*constages;
	char		*desstages;
	char		*damageparent;
	int			conbmodels[MAX_CONSTRUCT_STAGES+1];
	int			desbmodels[MAX_CONSTRUCT_STAGES];
	int			partofstage;

	int			allowteams;

	int			spawnTime;

	gentity_t	*dmgparent;

	int			spawnCount;					// incremented each time this entity is spawned

	int			tagNumber;		// Gordon: "handle" to a tag header

	int				linkTagTime;

	splinePath_t*	backspline;
	vec3_t			backorigin;
	float			backdelta;
	qboolean		back;
	qboolean		moving;

	// What sort of surface are we standing on?
	int		surfaceFlags;

	char	tagBuffer[MAX_QPATH];

	// bleh - ugly
	int		backupWeaponTime;
	int		mg42weapHeat;

	vec3_t	oldOrigin;

	qboolean runthisframe;

	g_constructible_stats_t	constructibleStats;

	//bani
	int	etpro_misc_1;		// bit 0 = it's a planted/ticking dynamite AT AN OBJECTIVE. Bit 1 = missionair point already given.
	int	etpro_misc_2;		// the entityNumber of the (last) planted dyna. bit strange it's only 1 dyna..

	// jet Pilot - new entity vars
	int numPlayers;

	// forty - realistic hitboxes
	glerpFrame_t	legsFrame;
	glerpFrame_t	torsoFrame;
	int				timeShiftTime;

	// jaquboss
	int				reviveTime;
	int				reviveVal;
	int				auraCheckTime;

	int				shufflePause;

#ifdef OMNIBOTS
	//Omni-bot increment dyno count
	int numPlanted;
#endif

	//core: a chain of free entities, for faster searches..
	gentity_t *prev_free_entity;
	gentity_t *next_free_entity;
};

//core: The first & last free entity in the chain of free entities, for faster searches..
gentity_t *first_free_entity;
gentity_t *last_free_entity;


//--- core: to store an array of pointers to entities..
typedef struct {
	gentity_t	*ent[MAX_GENTITIES];
	int			count;
} entityarray_t;

// the functions to handle the above struct..
void InitEntityArray( entityarray_t *ptr_array );
qboolean AddToEntityArray( entityarray_t *ptr_array, gentity_t *ent, qboolean unique );
qboolean RemoveFromEntityArray( entityarray_t *ptr_array, gentity_t *ent );

entityarray_t g_landmines;		// all landmine entities..
entityarray_t g_satchels;		// all satchel entities..
entityarray_t g_mortars;		// all mortar entities..
entityarray_t g_airstrikes;		// all airstrike entities..
entityarray_t g_arties;			// all arty entities..
entityarray_t g_panzerfausts;	// all panzerfaust entities..
entityarray_t g_bazookas;		// all bazooka entities..
entityarray_t g_miscMG42s;		// all misc_mg42 entities..
entityarray_t g_indicators;		// all indicator entities..
// indicators included in the array are:
// ET_CONSTRUCTIBLE_INDICATOR, ET_EXPLOSIVE_INDICATOR,
// ET_TANK_INDICATOR, ET_TANK_INDICATOR_DEAD & ET_COMMANDMAP_MARKER
entityarray_t g_scripttriggers;	// all scripttrigger entities used in G_ScriptAction_Trigger()..
entityarray_t g_triggers;		// all trigger entities (TOI etc)..
entityarray_t g_spawns;			// all spawn-stuff entities..
// spawn entities used in the array are:
// info_player_deathmatch, info_player_intermission, team_CTF_redspawn, team_CTF_bluespawn,
// team_WOLF_objective, team_WOLF_checkpoint.
//---

typedef enum {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_WALK,
	/*SPECTATOR_SCOREBOARD*/
} spectatorState_t;

typedef enum {
	COMBATSTATE_COLD,
	COMBATSTATE_DAMAGEDEALT,
	COMBATSTATE_DAMAGERECEIVED,
	COMBATSTATE_KILLEDPLAYER
} combatstate_t;

typedef enum {
	TEAM_BEGIN,		// Beginning a team game, spawn at base
	TEAM_ACTIVE		// Now actively playing
} playerTeamStateState_t;

typedef struct playerTeamState_s
{
	playerTeamStateState_t	state;

	int						location[3];
	int						captures;
	int						basedefense;
	int						carrierdefense;
	int						flagrecovery;
	int						fragcarrier;
	int						assists;

	float					lasthurtcarrier;
	float					lastreturnedflag;
	float					flagsince;
	float					lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

// OSP - weapon stat counters
typedef struct weapon_stat_s
{
	unsigned int atts;
	unsigned int deaths;
	unsigned int headshots;
	unsigned int hits;
	unsigned int kills;
} weapon_stat_t;


// core: bitflags "mute initiated by"..
#define UNMUTED			0		// currently not muted
#define MUTED_BY_NONE	UNMUTED	// same as ^^
#define MUTED_BY_REF	1		// by referee
#define MUTED_BY_CENSOR	2		// by censor (language/name)
#define MUTED_BY_SHRUB	4		// by shrubbot !mute command
#define MUTED_BY_LUA	8		// by Lua


// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct {
	team_t				sessionTeam;			// Player's team
	int					spectatorTime;			// Time in spectator mode
	spectatorState_t	spectatorState;
	int					spectatorClient;		// for chasecam and follow mode
	int					playerType;				// Player Class
	int					playerWeapon;			// Primary Weapon
	int					playerWeapon2;			// Secondary Weapon
	int					spawnObjectiveIndex;	// JPW NERVE index of objective to spawn nearest to (returned from UI)
	int					latchPlayerType;		// Player Class we will switch to
	int					latchPlayerWeapon;		// Primary weapon we switched to
	int					latchPlayerWeapon2;		// Secondary weapon we switched to
	int					rifleNade;				// player has rifleNade avaialable
	int					ignoreClients[2];		// MAX_CLIENTS / 32
	qboolean			muted;
	int 				auto_mute_time;			// josh
	int					muted_by;				// core: to differentiate mute types
	float				skillpoints[SK_NUM_SKILLS];		// Arnout: skillpoints
	float				startskillpoints[SK_NUM_SKILLS];// Gordon: initial skillpoints at map beginning
	float				startxptotal;
	int					skill[SK_NUM_SKILLS];			// Arnout: skill
	int					rank;							// Arnout: rank
	int					medals[SK_NUM_SKILLS];			// Arnout: medals

	// OSP
	int					coach_team;
	int					damage_given;
	int					damage_received;
	int					deaths;
	int					game_points;
	int					kills;
	int					referee;
	int					rounds;
	int					spec_invite;
	int					spec_team;
	int					suicides;
	int					team_damage;
	int					team_kills;

	// matt
	float 				team_hits;
	float 				hits;

	// MAPVOTE
	int mapVotedFor[3];

	// Perro - Killing Streaks, death streaks
	int					kstreak;
	int					dstreak;

	// jet Pilot
	int					killingSpree;			// last kill spree

	// Weapon stats.  +1 to avoid invalid weapon check
	weapon_stat_t		aWeaponStats[WS_MAX+1];

	int					newton;				// newton award
	int					darwin;
	int					goomba;
	int					executions;
	int					poisoneer;
	int					kickass;
	int					panzernoob;
	int					missionair;
	int					resuscetations;
	float				mapstartSkillpoints[SK_NUM_SKILLS]; // core: initial skillpoints at mapstart

	int					skillBits[7];		// jaquboss available skills in bits
	unsigned int 		uci; 				//mcwf GeoIP

#ifdef OMNIBOTS
	//Omni-bot
	qboolean	botSuicide;			// /kill before next spawn
	qboolean	botPush;			// allow for disabling of bot pushing via script
#endif

} clientSession_t;


#define	MAX_VOTE_COUNT		3

#define PICKUP_ACTIVATE	0	// pickup items only when using "+activate"
#define PICKUP_TOUCH	1	// pickup items when touched
#define PICKUP_FORCE	2	// pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)

#ifdef MV_SUPPORT
// OSP -- multiview handling
#define MULTIVIEW_MAXVIEWS	16
typedef struct {
	qboolean	fActive;
	int			entID;
	gentity_t	*camera;
} mview_t;
#endif

typedef struct ipFilter_s {
	unsigned	mask;
	unsigned	compare;
} ipFilter_t;

#define MAX_COMPLAINTIPS 5

// josh: constants for zinx' antiwarp
#define LAG_MAX_COMMANDS 512
#define LAG_MAX_DELTA 75
#define LAG_MAX_DROP_THRESHOLD 800
#define LAG_MIN_DROP_THRESHOLD (LAG_MAX_DROP_THRESHOLD - 200)
#define LAG_DECAY 1.02f


// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
	clientConnected_t	connected;
	usercmd_t			cmd;					// we would lose angles if not persistant
	usercmd_t			oldcmd;					// previous command processed by pmove()
	qboolean			localClient;			// true if "ip" info key is "localhost"
	qboolean			initialSpawn;			// the first spawn should be at a cool location
	qboolean			predictItemPickup;		// based on cg_predictItems userinfo
	qboolean			pmoveFixed;				//
	int					pmoveMsec;
	char				netname[MAX_NETNAME];
	char				cl_guid[PB_GUID_LENGTH+1];	// Lucel: this seems a better place to store this
	char				client_ip[MAX_IP_LENGTH];	// Cache their ip too - it won't change
	int					autoActivate;			// based on cg_autoactivate userinfo		(uses the PICKUP_ values above)

	int					maxHealth;				// for handicapping
	int					enterTime;				// level.time the client entered the game
	int					connectTime;			// DHM - Nerve :: level.time the client first connected to the server
	playerTeamState_t	teamState;				// status in teamplay games
	int					voteCount;				// to prevent people from constantly calling votes
	int					teamVoteCount;			// to prevent people from constantly calling votes

	int					complaints;				// DHM - Nerve :: number of complaints lodged against this client
	int					complaintClient;		// DHM - Nerve :: able to lodge complaint against this client
	int					complaintEndTime;		// DHM - Nerve :: until this time has expired

	int					lastReinforceTime;		// DHM - Nerve :: last reinforcement

	qboolean			teamInfo;				// send team overlay updates?

	qboolean			bAutoReloadAux;			// TTimo - auxiliary storage for pmoveExt_t::bAutoReload, to achieve persistance

	int					applicationClient;		// Gordon: this client has requested to join your fireteam
	int					applicationEndTime;		// Gordon: you have X seconds to reply or this message will self destruct!

	int					invitationClient;		// Gordon: you have been invited to join this client's fireteam
	int					invitationEndTime;		// Gordon: quickly now, chop chop!.....

	int					propositionClient;		// Gordon: propositionClient2 has requested you invite this client to join the fireteam
	int					propositionClient2;		// Gordon:
	int					propositionEndTime;		// Gordon: tick, tick, tick....

	int					autofireteamEndTime;
	int					autofireteamCreateEndTime;
	int					autofireteamJoinEndTime;

	playerStats_t		playerStats;

	int					lastBattleSenseBonusTime;
	int					lastHQMineReportTime;
	int					lastHQArtyReportTime;	// IRATA arty detection
	int					lastCCPulseTime;

	int					lastSpawnTime;
	int					lastTeamChangeTime;

	// OSP
	unsigned int		autoaction;				// End-of-match auto-requests
	unsigned int		clientFlags;			// Client settings that need server involvement
	unsigned int		clientMaxPackets;		// Client com_maxpacket settings
	unsigned int		clientTimeNudge;		// Client cl_timenudge settings
	int					cmd_debounce;			// Dampening of command spam
	unsigned int		invite;					// Invitation to a team to join
#ifdef MV_SUPPORT
	mview_t				mv[MULTIVIEW_MAXVIEWS];	// Multiview portals
	int					mvCount;				// Number of active portals
	int					mvReferenceList;		// Reference list used to generate views after a map_restart
	int					mvScoreUpdate;			// Period to send score info to MV clients
#endif
	qboolean			ready;					// Ready state to begin play
	// OSP

	bg_character_t		*character;
    int					characterIndex;

	ipFilter_t			complaintips[MAX_COMPLAINTIPS];

	// tjw
	int					hitsounds;
	qboolean			slashKill;

	int					lastkilled_client;
	int					lastrevive_client;
	int					lastkiller_client;
	int					lastammo_client;
	int					lasthealth_client;
	int					lastteambleed_client;
	int					lastteambleed_dmg;

//unlagged - true ping
	int					realPing;
	int					pingsamples[NUM_PING_SAMPLES];
	int					samplehead;
//unlagged - true ping
	qboolean			weapAltReloadAux;

} clientPersistant_t;

typedef struct {
	vec3_t 		mins;
	vec3_t 		maxs;

	vec3_t 		origin;

	//josh: Need these for BuildHead/Legs
	int 		eFlags;		// s.eFlags to ps.eFlags
	int 		viewheight; // ps for both
	int 		pm_flags;	// ps for both
	vec3_t		viewangles; // s.apos.trBase to ps.viewangles

	int			time;

	// forty - realistic headboxes - torso markers
 	qhandle_t 	torsoOldFrameModel;
 	qhandle_t 	torsoFrameModel;
	int 		torsoOldFrame;
	int 		torsoFrame;
	int 		torsoOldFrameTime;
	int 		torsoFrameTime;
	float 		torsoYawAngle;
	float 		torsoPitchAngle;
	int		 	torsoYawing;
	int		 	torsoPitching;

	// forty - realistic headboxes - leg markers
 	qhandle_t 	legsOldFrameModel;
 	qhandle_t 	legsFrameModel;
	int 		legsOldFrame;
	int 		legsFrame;
	int 		legsOldFrameTime;
	int 		legsFrameTime;
	float 		legsYawAngle;
	float 		legsPitchAngle;
	int		 	legsYawing;
	qboolean 	legsPitching;
} clientMarker_t;

#define MAX_CLIENT_MARKERS 10

#define LT_SPECIAL_PICKUP_MOD	3		// JPW NERVE # of times (minus one for modulo) LT must drop ammo before scoring a point
#define MEDIC_SPECIAL_PICKUP_MOD	4	// JPW NERVE same thing for medic

// Gordon: debris test
typedef struct debrisChunk_s {
	vec3_t	origin;
	int		model;
	vec3_t	velocity;
	char	target[32];
	char	targetname[32];
} debrisChunk_t;

#define MAX_DEBRISCHUNKS		256
// ===================

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t		ps;				// communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;

	qboolean	noclip;

	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	int			wbuttons;
	int			oldwbuttons;
	int			latched_wbuttons;
	vec3_t		oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation
	qboolean	damage_fromWorld;	// if true, don't use the damage_from vector

	int			lastkilled_client;	// last client that this client killed
	int			lasthurt_client;	// last client that damaged this client
	int			lasthurt_mod;		// type of damage the client did
	int			lasthurt_time;		// level.time of last damage
	int			lasthurt_location;	// jet Pilot - location (head, torso, leg, etc.) of last damage
	int			lastrevive_client;
	int			lastkiller_client;
	int			lastammo_client;
	int			lasthealth_client;

	// timers
	int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
	int			inactivityTime;		// kick players when time > this
	qboolean	inactivityWarning;	// qtrue if the five seoond warning has been given
	int			inactivitySecondsLeft;	// core: for displaying a counting-down time on clients (milliseconds before activity kicks in..)

	// int			rewardTime;			// clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int			airOutTime;

	int			lastKillTime;		// for multiple kill rewards
	int			multikill;

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int			timeResidual;

	float		currentAimSpreadScale;

	int			dropWeaponTime; // JPW NERVE last time a weapon was dropped
	int			limboDropWeapon; // JPW NERVE weapon to drop in limbo
	int			lastBurnTime; // JPW NERVE last time index for flamethrower burn
	int			PCSpecialPickedUpCount; // JPW NERVE used to count # of times somebody's picked up this LTs ammo (or medic health) (for scoring)
	int			saved_persistant[MAX_PERSISTANT];	// DHM - Nerve :: Save ps->persistant here during Limbo

	gentity_t	*touchingTOI;	// Arnout: the trigger_objective_info a player is touching this frame

	int			lastConstructibleBlockingWarnTime;
	int			lastConstructibleBlockingWarnEnt;

	int			landmineSpottedTime;
	gentity_t*	landmineSpotted;

	int			speedScale;

	combatstate_t	combatState;

	// unlagged history
	int				topMarker;
	clientMarker_t	clientMarkers[MAX_CLIENT_MARKERS];
	clientMarker_t	backupMarker;

	// antiwarp history
	int             lastCmdRealTime;
	int             cmdhead;        // antiwarp command queue head
	int             cmdcount;       // antiwarp command queue # valid commands
	float           cmddelta;       // antiwarp command queue # valid commands

	usercmd_t       cmds[LAG_MAX_COMMANDS]; // antiwarp command queue

	gentity_t		*tempHead;	// Gordon: storing a temporary head for bullet head shot detection
	gentity_t		*tempLeg;	// Arnout: storing a temporary leg for bullet head shot detection

	int				flagParent;

	// the next 2 are used to play the proper animation on the body
	int				torsoDeathAnim;
	int				legsDeathAnim;

	int				lastSpammyCentrePrintTime;
	pmoveExt_t		pmext;
	int				deathTime;		// if we are dead, when did we die

	char			disguiseNetname[MAX_NETNAME];
	int				disguiseRank;

	int				medals;
	float			acc;
	int				headshots;		// jP - track total headshots
	int				hitSound;		// jP - hitsound to play, 0 for none

	qboolean		hasaward;
	qboolean		wantsscore;
	qboolean		maxlivescalced;
	int				XPSave_lives;
	qboolean		XPSave_loaded;
	int				flametime;

//unlagged - smooth clients #1
	// the last frame number we got an update from this client
	int			lastUpdateFrame;
//unlagged - smooth clients #1

//unlagged - backward reconciliation #1
// an approximation of the actual server time we received this
	// command (not in 50ms increments)
	int			frameOffset;
//unlagged - backward reconciliation #1
	qboolean warping;
	qboolean warped;

// jaquboss
	qboolean	weaponsRemoved;

	qboolean	deathAnim;				// if true body has effect1Time set so death animation may start
	int			deathAnimTime;		// time when anim ends
	qboolean	executed;
	qboolean	freezed;

	int			poisonTime;

	// tjw: used for antilag
	int			attackTime;
};

typedef struct {
	char	modelname[32];
	int		model;
} brushmodelInfo_t;

typedef struct limbo_cam_s {
	qboolean	hasEnt;
	int			targetEnt;
	vec3_t		angles;
	vec3_t		origin;
	qboolean	spawn;
	int			info;
} limbo_cam_t;

#define MAX_LIMBO_CAMS 32


// this structure is cleared as each map is entered
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	2048
#define VOTE_MAXSTRING			256		// Same value as MAX_STRING_TOKENS

// tjw: increased from 8 to 10 for compatability with maps that relied on
//      it before Project: Bug Fix #055
#define	MAX_SCRIPT_ACCUM_BUFFERS	10

#define MAX_BUFFERED_CONFIGSTRINGS 128

typedef struct voteInfo_s {
	char		voteString[MAX_STRING_CHARS];
	int			voteTime;				// level.time vote was called
	int			voteYes;
	int			voteNo;
	int			numVotingClients;		// set by CalculateRanks
	int			numVotingTeamClients[2];
	int			(*vote_fn)(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
	char		vote_value[VOTE_MAXSTRING];	// Desired vote item setting.
	int         voteCaller; // id of the vote caller
	int         voteTeam;   // id of the vote caller's team
} voteInfo_t;

typedef struct cfgCvar_s {
	char		name[256];
	char		value[256];
} cfgCvar_t;

typedef struct config_s {
	char		name[256];
	char		version[256];
	cfgCvar_t	setl[256];
	int			numSetl;
	qboolean	loaded;
} config_t;

//jet Pilot - moved level_locals_t declaration to bg_local.h
typedef struct level_locals_s {
	struct gclient_s	*clients;					// [maxclients]
	struct gentity_s	*gentities;
	int					gentitySize;
	int					num_entities;				// current number, <= MAX_GENTITIES
	int					warmupTime;					// restart match at this time
	fileHandle_t		chatLog;					// jet Pilot - separate logfile specific to chats
	fileHandle_t		tkLog;
	fileHandle_t		logFile;
	fileHandle_t		crashLog;
	char				rawmapname[MAX_QPATH];		// name of the currently loaded map

	// store latched cvars here that we want to get at often
	int					maxclients;
	int					framenum;
	int					time;						// in msec
	int					overTime;
	int					previousTime;				// so movers can back up when blocked
	int					frameTime;					// Gordon: time the frame started, for antilag stuff
	int					startTime;					// level.time the map was started
	int					teamScores[TEAM_NUM_TEAMS];
	int					lastTeamLocationTime;		// last time of client team location update
	qboolean			newSession;					// don't use any old session data, because
													// we changed gametype

	qboolean			restarted;					// waiting for a map_restart to fire
	int					numConnectedClients;
	int					numNonSpectatorClients;		// includes connecting clients
	int					numPlayingClients;			// connected, non-spectators
	int					sortedClients[MAX_CLIENTS];	// sorted by score
	int					follow1, follow2;			// clientNums for auto-follow spectators
	int					warmupModificationCount;	// for detecting if g_warmup is changed

	// voting
	voteInfo_t			voteInfo;
	int					numTeamClients[2];
	int					numVotingTeamClients[2];
	// ~+~+~+~+~+~+~+~+~+~+~+~+~+~

	// spawn variables
	qboolean			spawning;					// the G_Spawn*() functions are valid
	int					numSpawnVars;
	char				*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int					numSpawnVarChars;
	char				spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int					intermissionQueued;		// intermission was qualified, but
												// wait INTERMISSION_DELAY_TIME before
												// actually going there so the last
												// frag can be watched.  Disable future
												// kills during this delay
	int					intermissiontime;		// time the intermission was started
	char				*changemap;
	int					exitTime;
	vec3_t				intermission_origin;	// also used for spectator spawns
	vec3_t				intermission_angle;
	qboolean			lmsDoNextMap;			// should LMS do a map_restart or a vstr nextmap

	int					bodyQueIndex;			// dead bodies
	gentity_t			*bodyQue[BODY_QUEUE_SIZE];

	gentity_t			*missileCams[MAX_CLIENTS];


	int					portalSequence;
	int					reloadPauseTime;		// don't think AI/client's until this time has elapsed
	int					reloadDelayTime;		// don't start loading the savegame until this has expired

	int					capturetimes[4];						// red, blue, none, spectator for WOLF_MP_CPH
	int					redReinforceTime, blueReinforceTime;	// last time reinforcements arrived in ms
	int					redNumWaiting, blueNumWaiting;			// number of reinforcements in queue
	vec3_t				spawntargets[MAX_MULTI_SPAWNTARGETS];	// coordinates of spawn targets
	int					numspawntargets;						// # spawntargets in this map

	// RF, entity scripting
	char				*scriptEntity;

	// player/AI model scripting (server repository)
	animScriptData_t	animScriptData;

	// int					totalHeadshots;
	// int					missedHeadshots;
	qboolean			lastRestartTime;

	int					numFinalDead[2];		// DHM - Nerve :: unable to respawn and in limbo (per team)
	int					numOidTriggers;			// DHM - Nerve

	qboolean			latchGametype;			// DHM - Nerve

	int					globalAccumBuffer[MAX_SCRIPT_ACCUM_BUFFERS];

	// charge times for each class
	int					soldierChargeTime[2];
	int					medicChargeTime[2];
	int					engineerChargeTime[2];
	int					lieutenantChargeTime[2];
	int					covertopsChargeTime[2];
	// ~+~+~+~+~+~+~+~+~+~+~+~+~+~

	int					lastMapEntityUpdate;
	int					objectiveStatsAllies[MAX_OBJECTIVES];
	int					objectiveStatsAxis[MAX_OBJECTIVES];

	int					lastSystemMsgTime[2];

	// charge time modifiers
	float				soldierChargeTimeModifier[2];
	float				medicChargeTimeModifier[2];
	float				engineerChargeTimeModifier[2];
	float				lieutenantChargeTimeModifier[2];
	float				covertopsChargeTimeModifier[2];
	// ~+~+~+~+~+~+~+~+~+~+~+~+~+~

	int					firstbloodTeam;
	int					teamEliminateTime;
	int					lmsWinningTeam;

	int					campaignCount;
	int					currentCampaign;
	qboolean			newCampaign;

	brushmodelInfo_t	brushModelInfo[128];
	int					numBrushModels;
	gentity_t			*gameManager;

    // record last time we loaded, so we can hack around sighting issues on reload
    int					lastLoadTime;

	qboolean			doorAllowTeams;	// used by bots to decide whether or not to use team travel flags

	// Gordon: for multiplayer fireteams
	fireteamData_t		fireTeams[MAX_FIRETEAMS];

	qboolean			ccLayers;

	// OSP
	int					dwBlueReinfOffset;
	int					dwRedReinfOffset;
	qboolean			fLocalHost;
	qboolean			fResetStats;
	int					match_pause;				// Paused state of the match
	qboolean			ref_allready;				// Referee forced match start
	int					server_settings;
	int					sortedStats[MAX_CLIENTS];	// sorted by weapon stat
	int					timeCurrent;				// Real game clock
	int					timeDelta;					// Offset from internal clock - used to calculate real match time
	// OSP

	qboolean			mapcoordsValid, tracemapLoaded;
	vec2_t				mapcoordsMins, mapcoordsMaxs;

	char				tinfoAxis[1400];
	char				tinfoAllies[1400];

// Gordon: debris test
	int					numDebrisChunks;
	debrisChunk_t		debrisChunks[MAX_DEBRISCHUNKS];
// ===================

	qboolean			disableTankExit;
	qboolean			disableTankEnter;

	int					axisBombCounter, alliedBombCounter;
	// forty - arty/airstrike rate limiting
	int					axisArtyCounter, alliedArtyCounter;
	int					axisAutoSpawn, alliesAutoSpawn;
	int					axisMG42Counter, alliesMG42Counter;

	limbo_cam_t			limboCams[MAX_LIMBO_CAMS];
	int					numLimboCams;

	int					numActiveAirstrikes[2];

	float				teamXP[SK_NUM_SKILLS][2];

	commanderTeamChat_t commanderSounds[2][MAX_COMMANDER_TEAM_SOUNDS];
	int					commanderSoundInterval[2];
	int					commanderLastSoundTime[2];

	qboolean			tempTraceIgnoreEnts[ MAX_GENTITIES ];

	int 				nextBanner;

	// much more complicated sv_cvars
	svCvar_t			svCvars[MAX_SVCVARS];
	int					svCvarsCount;

	// forcecvars
	forceCvar_t			forceCvars[MAX_FORCECVARS];
	int					forceCvarCount;

	//perro - longest spree (count and player name) for this level
	int					maxspree_count;
	char				maxspree_player[MAX_STRING_CHARS];

	// tjw: track CS usage
	int					csLen[MAX_CONFIGSTRINGS];
	int					csLenTotal;

	// jaquboss
	char	weaponScriptsDir[MAX_CVAR_VALUE_STRING];

	config_t	config; // etpro style config - name and locked cvars

	// core: shortcuts for last client calling for medic/ammo
	int					last_axisCallingMedic;
	int					last_alliesCallingMedic;
	int					last_axisCallingAmmo;
	int					last_alliesCallingAmmo;

	// core: for checking if client-files are present..
	qboolean			clientFilesFound;

#ifdef OMNIBOTS
	//Omni-bot time triggers
	qboolean	twoMinute;
	qboolean	thirtySecond;
#endif

	// matt: MAPVOTE information
	int sortedMaps[MAX_VOTE_MAPS];
	mapVoteInfo_t mapvoteinfo[MAX_VOTE_MAPS];
	int mapVoteNumMaps;
	int mapsSinceLastXPReset;

} level_locals_t;

typedef struct {
	char		mapnames[MAX_MAPS_PER_CAMPAIGN][MAX_QPATH];
	//arenaInfo_t	arenas[MAX_MAPS_PER_CAMPAIGN];
	int			mapCount;
	int			current;

	char		shortname[256];
	char		next[256];
	int			typeBits;

#ifdef OMNIBOTS
	//Omni-bot time triggers
	qboolean	twoMinute;
	qboolean	thirtySecond;
#endif
} g_campaignInfo_t;

//
// g_spawn.c
//
#define		G_SpawnString(		key, def, out ) G_SpawnStringExt	( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnFloat(		key, def, out ) G_SpawnFloatExt		( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnInt(			key, def, out ) G_SpawnIntExt		( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnVector(		key, def, out ) G_SpawnVectorExt	( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnVector2D(	key, def, out ) G_SpawnVector2DExt	( key, def, out, __FILE__, __LINE__ )

qboolean	G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line );	// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean	G_SpawnFloatExt	( const char *key, const char *defaultString, float *out, const char* file, int line );
qboolean	G_SpawnIntExt	( const char *key, const char *defaultString, int *out, const char* file, int line );
qboolean	G_SpawnVectorExt( const char *key, const char *defaultString, float *out, const char* file, int line );
qboolean	G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line );
gentity_t *G_SpawnGEntityFromSpawnVars( void );
void		G_SpawnEntitiesFromString( void );
char *G_NewString( const char *string );
// Ridah
qboolean G_CallSpawn( gentity_t *ent );
// done.
char *G_AddSpawnVarToken( const char *string );
void G_ParseField( const char *key, const char *value, gentity_t *ent );

//
// g_cmds.c
// Josh: max words and word lengths for censor dictionary
// IRATA: increased to keep compatibility - see InitCensorNamesStructure
// default 50 + DcensoredNames count = 62 atm
#define DICT_MAX_WORDS  62
#define DICT_MAX_WORD_LENGTH 32
// Josh: word dictionary for censor
typedef struct {
	char	words[DICT_MAX_WORDS][DICT_MAX_WORD_LENGTH];
	int num_words;
} wordDictionary;
qboolean G_CensorText(char *text, wordDictionary *dictionary);


void G_PrivateMessage(gentity_t *ent);
void G_PlayDead(gentity_t *ent);
void G_TeamDamageStats(gentity_t *ent);
void Cmd_Score_f (gentity_t *ent);
void StopFollowing( gentity_t *ent );
//void BroadcastTeamChange( gclient_t *client, int oldTeam );
void G_TeamDataForString( const char* teamstr, int clientNum, team_t* team, spectatorState_t* sState, int* specClient );
qboolean SetTeam( gentity_t *ent, const char *s, qboolean force, weapon_t w1, weapon_t w2, qboolean setweapons );
void G_SetClientWeapons( gentity_t* ent, weapon_t w1, weapon_t w2, qboolean updateclient );
void Cmd_FollowCycle_f( gentity_t *ent, int dir, qboolean skipBots );
void Cmd_Kill_f( gentity_t *ent );

#ifdef OMNIBOTS
void Cmd_SwapPlacesWithBot_f( gentity_t *ent, int botNum );
#endif

void G_EntitySound( gentity_t *ent, const char *soundId, int volume );
void G_EntitySoundNoCut( gentity_t *ent, const char *soundId, int volume );
qboolean G_MatchOnePlayer(int *plist, char *err, int len);
int ClientNumbersFromString( char *s, int *plist );
int ClientNumberFromString( gentity_t *to, char *s );
void DecolorString( char *in, char *out );
void SanitizeString( char *in, char *out, qboolean fToLower );
void G_DropItems(gentity_t *self);
char *ConcatArgs( int start );
char *Q_SayConcatArgs(int start);
char *Q_AddCR(char *s);
int Q_SayArgc();
qboolean Q_SayArgv(int n, char *buffer, int bufferLength);

//
// g_items.c
//
void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );
void PrecacheItem (gitem_t *it);
gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle, qboolean novelocity );
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum, int flags );
void SetRespawn (gentity_t *ent, float delay);
void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon (gentity_t *ent);
int ArmorIndex (gentity_t *ent);
void Fill_Clip (playerState_t *ps, int weapon);
int Add_Ammo (gentity_t *ent, int weapon, int count, qboolean fillClip);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);
qboolean AddMagicAmmo(gentity_t *receiver, int numOfClips);
weapon_t G_GetWeaponsForClient( gclient_t *client, weapon_t secondary );
weapon_t G_GetPrimaryWeaponForClient( gclient_t *client );
weapon_t G_GetSecondaryWeaponForClient( gclient_t *client, weapon_t primary ); // jaquboss

void G_DropWeapon( gentity_t *ent, weapon_t weapon );
void G_DropClip( gentity_t *ent, weapon_t weapon ); // jaquboss
void G_DropDogTag( gentity_t *ent ); // use drop weapon ?

// Touch_Item_Auto is bound by the rules of autoactivation (if cg_autoactivate is 0, only touch on "activate")
void Touch_Item_Auto (gentity_t *ent, gentity_t *other, trace_t *trace);

void Prop_Break_Sound (gentity_t *ent);
void Spawn_Shard (gentity_t *ent, gentity_t *inflictor, int quantity, int type);

//
// g_utils.c
//
// Ridah
int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create );
extern void Svcmd_EntityList_f(void);
// done.

// CHRUKER: b087 - Player always mounting the last gun used, on multiple tank maps
void G_RemoveConfigstringIndex( const char *name, int start, int max);
 // b087

int		G_ModelIndex( char *name );
int		G_SoundIndex( const char *name );
int		G_SkinIndex( const char *name );
int		G_ShaderIndex( const char *name );
int		G_CharacterIndex( const char *name );
int		G_StringIndex( const char* string );

qboolean G_AllowTeamsAllowed( gentity_t *ent, gentity_t *activator );
void	G_UseEntity( gentity_t *ent, gentity_t *other, gentity_t *activator );
qboolean G_IsWeaponDisabled( gentity_t* ent, weapon_t weapon, qboolean quiet);
qboolean G_IsClassDisabled( gentity_t* ent, int clsn, qboolean quiet);
void	G_TeamCommand( team_t team, char *cmd );
void	G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
gentity_t *G_FindInt(gentity_t *from, int fieldofs, int match);
gentity_t *G_FindFloat(gentity_t *from, int fieldofs, float match);
gentity_t *G_FindVector(gentity_t *from, int fieldofs, const vec3_t match);
gentity_t* G_FindByTargetname(gentity_t *from, const char* match);
gentity_t* G_FindByTargetnameFast(gentity_t *from, const char* match, int hash);
gentity_t* G_FindByTargetFast(gentity_t *from, const char* match, int hash);
gentity_t* G_FindByScriptnameFast(gentity_t *from, const char* match, int hash);
gentity_t* G_FindByClassnameFast(gentity_t *from, const char* match, int hash);
gentity_t *G_PickTarget(char *targetname);
gentity_t *G_PickSpawnTarget(char *targetname);
void	G_UseTargets (gentity_t *ent, gentity_t *activator);
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);

void	G_InitGentity( gentity_t *e );
// IlDuca - ported from ETPub : returns the number of free entities
int	G_GentitiesAvailable();

// core: initialize a chain of free entities for faster searches..
void G_InitFreeEntitiesChain( void );

gentity_t	*G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
gentity_t* G_PopupMessage( popupMessageType_t type );
void	G_Sound( gentity_t *ent, int soundIndex );
void	G_AnimScriptSound( int soundIndex, vec3_t org, int client );
void	G_FreeEntity( gentity_t *e );
int		G_EntitiesFree( void );

// *LUA* pheno
void G_ClientSound( gentity_t *ent, int soundIndex );

void	G_TouchTriggers (gentity_t *ent);
void	G_TouchSolids (gentity_t *ent);

float	*tv (float x, float y, float z);
char	*vtos( const vec3_t v );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig();
void G_SetAngle( gentity_t *ent, vec3_t angle );

qboolean infront (gentity_t *self, gentity_t *other);

void G_ProcessTagConnect(gentity_t *ent, qboolean clearAngles);

void G_SetEntState( gentity_t *ent, entState_t state );
void G_ParseCampaigns( void );
qboolean G_MapIsValidCampaignStartMap( void );

team_t G_GetTeamFromEntity( gentity_t *ent );


/* cut down refEntity_t w/ only stuff needed for player bone calculation */
/* Used only by game code - not engine */
/* core: This struct was moved here from etpro_mdx.h */
typedef struct {
	qhandle_t	hModel;				// opaque type outside refresh

	vec3_t		headAxis[3];

	// most recent data
	vec3_t		axis[3];		// rotation vectors
	vec3_t		torsoAxis[3];		// rotation vectors for torso section of skeletal animation
//	qboolean	nonNormalizedAxes;	// axis are not normalized, i.e. they have scale
	float		origin[3];
	int			frame;
	qhandle_t	frameModel;
	int			torsoFrame;			// skeletal torso can have frame independant of legs frame
	qhandle_t	torsoFrameModel;

	// previous data for frame interpolation
	float		oldorigin[3];
	int			oldframe;
	qhandle_t	oldframeModel;
	int			oldTorsoFrame;
	qhandle_t	oldTorsoFrameModel;
	float		backlerp;			// 0.0 = current, 1.0 = old
	float		torsoBacklerp;
} grefEntity_t;

//
// g_combat.c
//
void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t vec );
qboolean CanDamage (gentity_t *targ, vec3_t origin);
void G_Damage (gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);
qboolean G_RadiusDamage (vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod);
qboolean etpro_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod, qboolean clientsonly );
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
void TossClientItems( gentity_t *self );
gentity_t *G_BuildHead(gentity_t *ent, grefEntity_t *refent, qboolean newRefent);
gentity_t *G_BuildLeg(gentity_t *ent, grefEntity_t *refent, qboolean newRefent);
void G_UpdateKillingSpreeForMapEnd(gentity_t *ent);
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_HALF_KNOCKBACK		0x00000002	// Gordon: do less knockback
#define DAMAGE_NO_KNOCKBACK			0x00000008	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000020  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION	0x00000010  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_DISTANCEFALLOFF		0x00000040	// distance falloff
qboolean G_WeaponCanGib( meansOfDeath_t mod );
//
// g_missile.c
//
void G_RunMissile( gentity_t *ent );
int G_PredictMissile( gentity_t *ent, int duration, vec3_t endPos, qboolean allowBounce );
qboolean G_HasDroppedItem(gentity_t* ent, int modType);
void Destabilize_Missile( gentity_t *missile );

// Rafael zombiespit
void G_RunDebris( gentity_t *ent );

//DHM - Nerve :: server side flamethrower collision
void G_RunFlamechunk( gentity_t *ent );

//----(SA) removed unused q3a weapon firing
gentity_t *fire_flamechunk (gentity_t *self, vec3_t start, vec3_t dir);

gentity_t *fire_grenade (gentity_t *self, vec3_t start, vec3_t aimdir, int grenadeWPID);
gentity_t *fire_rocket (gentity_t *self, vec3_t start, vec3_t dir, int rocketType);
gentity_t *fire_speargun (gentity_t *self, vec3_t start, vec3_t dir);

#define Fire_Lead( ent, activator, spread, damage, muzzle, forward, right, up ) Fire_Lead_Ext( ent, activator, spread, damage, muzzle, forward, right, up, MOD_MACHINEGUN )
void Fire_Lead_Ext( gentity_t *ent, gentity_t *activator, float spread, int damage, vec3_t muzzle, vec3_t forward, vec3_t right, vec3_t up, int mod );
qboolean visible (gentity_t *self, gentity_t *other);

gentity_t *fire_mortar (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_flamebarrel (gentity_t *self, vec3_t start, vec3_t dir);
// done

//
// g_mover.c
//
gentity_t *G_TestEntityPosition( gentity_t *ent );
void G_RunMover( gentity_t *ent );
qboolean G_MoverPush( gentity_t *pusher, vec3_t move, vec3_t amove, gentity_t **obstacle );
void Use_BinaryMover( gentity_t *ent, gentity_t *other, gentity_t *activator );
void G_Activate( gentity_t *ent, gentity_t *activator );

void G_TryDoor(gentity_t *ent, gentity_t *other, gentity_t *activator);	//----(SA)	added

void InitMoverRotate ( gentity_t *ent );

void InitMover( gentity_t *ent );
void SetMoverState( gentity_t *ent, moverState_t moverState, int time );

void func_constructible_underconstructionthink( gentity_t *ent );

//
// g_tramcar.c
//
void Reached_Tramcar (gentity_t *ent);

//
// g_trigger.c
//
void Think_SetupObjectiveInfo( gentity_t *ent);

//
// g_misc.c
//
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles );
void mg42_fire( gentity_t *other );
void mg42_stopusing( gentity_t *self );

//
// g_weapon.c
//
qboolean jP_CheckDisguise( gentity_t *ent );
qboolean AccuracyHit( gentity_t *target, gentity_t *attacker );
void CalcMuzzlePoint ( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void SnapVectorTowards( vec3_t v, vec3_t to );
gentity_t *weapon_grenadelauncher_fire (gentity_t *ent, int grenadeWPID);

// void G_FadeItems(gentity_t* ent, int modType);
void G_FadeLandmines(gentity_t* ent);		// core: same as G_FadeItems( ent, MOD_LANDMINE), but faster..
void G_FadeSatchels(gentity_t* ent);		//   "    "   "  G_FadeItems( ent, MOD_SATCHEL)
void G_FadeMortars(gentity_t* ent);			//   "    "   "  G_FadeItems( ent, MOD_MORTAR)
void G_FadeAirstrikes(gentity_t* ent);		//   "    "   "  G_FadeItems( ent, MOD_AIRSTRIKE)
void G_FadeArties(gentity_t* ent);			//   "    "   "  G_FadeItems( ent, MOD_ARTY)
void G_FadePanzerfausts(gentity_t* ent);	//   "    "   "  G_FadeItems( ent, MOD_PANZERFAUST)
void G_FadeBazookas(gentity_t* ent);		//   "    "   "  G_FadeItems( ent, MOD_BAZOOKA)
gentity_t *G_FindSatchel(gentity_t* ent);
qboolean G_ExplodeSatchels(gentity_t* ent);

void CalcMuzzlePoints(gentity_t *ent, int weapon);
void CalcMuzzlePointForActivate ( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint, qboolean attack );
void Weapon_MagicAmmo_Ext(gentity_t *ent,
						  vec3_t viewpos,
						  vec3_t tosspos,
						  vec3_t velocity);
void Weapon_Medic_Ext(gentity_t *ent,
						  vec3_t viewpos,
						  vec3_t tosspos,
						  vec3_t velocity);
void Weapon_Shotgun( gentity_t *ent );	// Jaybird

//
// g_client.c
//
team_t TeamCount( int ignoreClientNum, int team );			// NERVE - SMF - merge from team arena
team_t PickTeam( int ignoreClientNum );
void SetClientViewAngle( gentity_t *ent, vec3_t angle );
gentity_t *SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
void respawn (gentity_t *ent);
void BeginIntermission (void);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
#ifdef USE_BODY_QUE
void InitBodyQue (void);
#endif
// core: missile/mortar-cams..
gentity_t *InitCamera( int clientNum );
void FreeCamera( int clientNum );
//
void ClientSpawn( gentity_t *ent, qboolean revived, qboolean teamChange, qboolean restoreHealth);
void player_die (gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
void AddScore( gentity_t *ent, int score );
void AddKillScore( gentity_t *ent, int score );
void CalculateRanks( qboolean doETproPlayerInfo );

qboolean SpotWouldTelefrag( gentity_t *spot );
qboolean G_CheckForExistingModelInfo( bg_playerclass_t* classInfo, const char *modelName, animModelInfo_t **modelInfo );
void G_AddClassSpecificTools(gclient_t *client);
void SetWolfSpawnWeapons( gclient_t *client );
void limbo( gentity_t *ent, qboolean makeCorpse ); // JPW NERVE
void reinforce(gentity_t *ent); // JPW NERVE
qboolean AddWeaponToPlayer( gclient_t *client, weapon_t weapon, int ammo, int ammoclip, qboolean setcurrent );

#ifdef ET_LUA
// *LUA* pheno: g_sha1.c
char *G_SHA1( char *string );
#endif

//
// g_character.c
//

qboolean G_RegisterCharacter( const char *characterFile, bg_character_t *character );
void G_RegisterPlayerClasses( void );
//void G_SetCharacter( gclient_t *client, bg_character_t *character, qboolean custom );
void G_UpdateCharacter( gclient_t *client );

//
// g_svcmds.c
//
qboolean ConsoleCommand( void );
void G_ProcessIPBans(void);
qboolean G_FilterIPBanPacket( char *from );
qboolean G_FilterMaxLivesPacket (char *from);
qboolean G_FilterMaxLivesIPPacket( char *from );
void AddMaxLivesGUID( char *str );
void AddMaxLivesBan( const char *str );
void ClearMaxLivesBans();
void AddIPBan( const char *str );

void Svcmd_ShuffleTeams_f( qboolean restart );

void G_UpdateForceCvars( void );
void G_UpdateSvCvars( void );

//
// g_weapon.c
//
void Kick_Fire( gentity_t *ent );
void FireWeapon( gentity_t *ent );
void ThrowKnife( gentity_t	*ent );
void G_BurnMeGood( gentity_t *self, gentity_t *body, gentity_t *chunk );

//
// p_hud.c
//
void MoveClientToIntermission (gentity_t *client);
void G_SetStats (gentity_t *ent);
void G_SendScore( gentity_t *client );

//
// g_cmds.c
//
void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize ); // JPW NERVE removed static declaration so it would link
// *LUA* pheno: added for Lua's et.G_Say() support
void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText );
qboolean Cmd_CallVote_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_Follow_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 );
void Cmd_Team_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void G_PlaySound_Cmd(void);
qboolean CheatsOkGeneral(void);
qboolean ExecGive( gentity_t *ent, char *name, char *amt, char *amt2); // jaquboss
void Cmd_GiveDisguise_f(gentity_t *ent, int startArg);
void Cmd_Insanity_Add(gentity_t *ent);

// MAPVOTE
void G_IntermissionMapVote( gentity_t *ent );
void G_IntermissionMapList( gentity_t *ent );
void G_IntermissionVoteTally( gentity_t *ent );

/*
//====================================
core:
	arays to store data previously
	contained in the configstring..
//====================================
*/
char gs_models[MAX_MODELS][MAX_QPATH];
char gs_shaders[MAX_CS_SHADERS][MAX_QPATH];
char gs_skins[MAX_CS_SKINS][MAX_QPATH];
char gs_characters[MAX_CHARACTERS][MAX_QPATH];
char gs_sounds[MAX_SOUNDS][MAX_QPATH];
char gs_shaderstate[MAX_QPATH];

void Init_gs_models( void );
void Init_gs_shaders( void );
void Init_gs_skins( void );
void Init_gs_characters( void );
void Init_gs_sounds( void );
void Init_gs_shaderstate( void );

void C_SendNewConfigString( gentity_t *ent, int num, const char *string );
void C_SendAllNewConfigStrings( gentity_t *ent, qboolean wantModels, qboolean wantShaders, qboolean wantSkins, qboolean wantCharacters, qboolean wantSounds );
char *C_CSMETHODINFO( void );

// the buffering mechanism
typedef struct {
	int			time;		// the time to send the string to the client
	int			index;		// the CS index of the string
} csmEntry_t;

typedef struct {
	int			push;		// index into the array for writing new entries
	int			pop;		// index into the array for reading entries
	int			count;		// the number of entries in this buffer
	csmEntry_t	entry[625];	// the entries
} csmBufferEntry_t;

csmBufferEntry_t csmBuffer[MAX_CLIENTS];

void C_csmBufferInit( void );
void C_csmBufferPush( gentity_t *ent, int index, int timetosend );
void C_csmBuffersCheck( void );
//====================================

//
// g_pweapon.c
//
//
// g_main.c
//
void FindIntermissionPoint( void );
void G_RunThink (gentity_t *ent);
void QDECL G_LogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
void QDECL G_CrashLogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char *fmt, ... )_attribute((format(printf,1,2)));
void QDECL G_DPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
void QDECL G_Error( const char *fmt, ... )_attribute((format(printf,1,2)));
void LogExit( const char *string );
qboolean G_LoadConfig( char forceFilename[MAX_QPATH], qboolean	init);
char *G_GetRealTime(void);

//
// g_client.c
//
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );
void ClientUserinfoChanged( int clientNum );
void ClientDisconnect( int clientNum );
void ClientBegin( int clientNum );
void ClientCommand( int clientNum );
float ClientHitboxMaxZ(gentity_t *hitEnt);

//
// g_active.c
//
void ClientThink( int clientNum );
void ClientThink_cmd( gentity_t* ent, usercmd_t* cmd );
void ClientEndFrame( gentity_t *ent );
void G_RunClient( gentity_t *ent );
qboolean ClientNeedsAmmo( int client );
qboolean ClientOutOfAmmo( int client );

//
// et-antiwarp.c
//
void etpro_AddUsercmd( int clientNum, usercmd_t *cmd );
void DoClientThinks( gentity_t *ent );
qboolean G_DoAntiwarp( gentity_t *ent );


// Does ent have enough "energy" to call artillery?
qboolean ReadyToCallArtillery(gentity_t* ent);
// to call airstrike?
qboolean ReadyToCallAirstrike(gentity_t* ent);
// to use smoke grenade?
qboolean ReadyToThrowSmoke(gentity_t *ent);
// Are we ready to construct?  Optionally, will also update the time while we are constructing
qboolean ReadyToConstruct(gentity_t *ent, gentity_t *constructible, qboolean updateState);

//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
//int QDECL G_SortPlayersByKillRating( const void *a, const void *b );
//void G_ActiveTeamBalance();

//
// g_mem.c
//
void *G_Alloc( int size );
void G_InitMemory( void );
void Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( qboolean restart );

void G_CalcRank( gclient_t* client );

//returns the number of the client with the given name
int ClientFromName(char *name);

// g_cmd.c
void Cmd_Activate_f (gentity_t *ent);
void Cmd_Activate2_f (gentity_t *ent);
qboolean Do_Activate_f(gentity_t *ent, gentity_t *traceEnt);
void G_LeaveTank( gentity_t* ent, qboolean position );

// g_script.c
void G_Script_ScriptParse( gentity_t *ent );
qboolean G_Script_ScriptRun( gentity_t *ent );
void G_Script_ScriptEvent( gentity_t *ent, gScriptEvent_t eventStr, char *params );
void G_Script_ScriptLoad( void );

void mountedmg42_fire( gentity_t *other );
void script_mover_use(gentity_t *ent, gentity_t *other, gentity_t *activator);
void script_mover_blocked( gentity_t *ent, gentity_t *other );

float AngleDifference(float ang1, float ang2);

// g_props.c
void Props_Chair_Skyboxtouch (gentity_t *ent);


#include "g_team.h" // teamplay specific stuff

extern	level_locals_t	level;
extern	gentity_t		g_entities[];	//DAJ was explicit set to MAX_ENTITIES
extern g_campaignInfo_t g_campaigns[];
extern int				saveGamePending;

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))

#ifdef OMNIBOTS
extern	vmCvar_t	g_OmniBotPath;
extern	vmCvar_t	g_OmniBotEnable;
extern	vmCvar_t	g_OmniBotFlags;
extern	vmCvar_t	g_OmniBotPlaying;
#ifdef DEBUG
extern	vmCvar_t	g_allowBotSwap;
#endif
#endif

extern	vmCvar_t	g_gametype;

extern	vmCvar_t	g_log;
extern	vmCvar_t	g_logChat;
extern	vmCvar_t	g_logCrash;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_minGameClients;		// NERVE - SMF - we need at least this many before match actually starts
extern	vmCvar_t	g_restarted;
extern	vmCvar_t	g_reset;
extern	vmCvar_t	g_timelimit;
extern	vmCvar_t	g_friendlyFire;
extern	vmCvar_t	g_password;
extern	vmCvar_t	sv_privatepassword;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_debugBullets;	//----(SA)	added
#ifdef ALLOW_GSYNC
extern	vmCvar_t	g_synchronousClients;
#endif // ALLOW_GSYNC
extern	vmCvar_t	g_warmup;
extern	vmCvar_t	voteFlags;

// DHM - Nerve :: The number of complaints allowed before kick/ban
extern	vmCvar_t	g_complaintlimit;
extern	vmCvar_t	g_ipcomplaintlimit;
extern	vmCvar_t	g_filtercams;
extern	vmCvar_t	g_maxlives;				// DHM - Nerve :: number of respawns allowed (0==infinite)
extern	vmCvar_t	g_maxlivesRespawnPenalty;
extern	vmCvar_t	g_voiceChatsAllowed;	// DHM - Nerve :: number before spam control
extern	vmCvar_t	g_alliedmaxlives;		// Xian
extern	vmCvar_t	g_axismaxlives;			// Xian
extern	vmCvar_t	g_enforcemaxlives;		// Xian - Temp ban with maxlives between rounds

extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_balancedteams;
extern	vmCvar_t	g_doWarmup;
extern	vmCvar_t	g_teamAutoJoin;
extern	vmCvar_t	g_teamForceBalance;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_filterBan;
extern	vmCvar_t	pmove_fixed;
extern	vmCvar_t	pmove_msec;

//Rafael
extern	vmCvar_t	g_scriptName;		// name of script file to run (instead of default for that map)

extern	vmCvar_t	g_scriptDebug;

extern	vmCvar_t	g_userAim;
extern	vmCvar_t	g_developer;

// JPW NERVE multiplayer
extern vmCvar_t		g_redlimbotime;
extern vmCvar_t		g_bluelimbotime;
extern vmCvar_t		g_medicChargeTime;
extern vmCvar_t		g_engineerChargeTime;
extern vmCvar_t		g_LTChargeTime;
extern vmCvar_t		g_soldierChargeTime;
// jpw

extern vmCvar_t		g_covertopsChargeTime;
extern vmCvar_t		g_debugConstruct;
extern vmCvar_t		g_landminetimeout;

// What level of detail do we want script printing to go to.
extern vmCvar_t		g_scriptDebugLevel;

// How fast do SP player and allied bots move?
extern vmCvar_t		g_movespeed;

extern vmCvar_t g_axismapxp;
extern vmCvar_t g_alliedmapxp;

extern vmCvar_t g_oldCampaign;
extern vmCvar_t g_currentCampaign;
extern vmCvar_t g_currentCampaignMap;

// Arnout: for LMS
extern vmCvar_t g_axiswins;
extern vmCvar_t g_alliedwins;
extern vmCvar_t g_lms_teamForceBalance;
extern vmCvar_t g_lms_roundlimit;
extern vmCvar_t g_lms_matchlimit;
extern vmCvar_t g_lms_currentMatch;
extern vmCvar_t g_lms_lockTeams;
extern vmCvar_t g_lms_followTeamOnly;

// NERVE - SMF
extern vmCvar_t		g_warmupLatch;
extern vmCvar_t		g_nextTimeLimit;
extern vmCvar_t		g_userTimeLimit;
extern vmCvar_t		g_userAlliedRespawnTime;
extern vmCvar_t		g_userAxisRespawnTime;
extern vmCvar_t		g_spawntimeMinPlayers;
extern vmCvar_t		g_currentRound;
extern vmCvar_t		g_noTeamSwitching;
extern vmCvar_t		g_altStopwatchMode;
extern vmCvar_t		g_gamestate;
extern vmCvar_t		g_swapteams;
// -NERVE - SMF

//Gordon
extern vmCvar_t		g_antilag;

// OSP
extern vmCvar_t		refereePassword;
extern vmCvar_t		g_spectatorInactivity;
extern vmCvar_t		match_latejoin;
extern vmCvar_t		match_minplayers;
extern vmCvar_t		match_mutespecs;
extern vmCvar_t		match_readypercent;
extern vmCvar_t		match_timeoutcount;
extern vmCvar_t		match_timeoutlength;
extern vmCvar_t		match_warmupDamage;
extern vmCvar_t		server_autoconfig;
extern vmCvar_t		server_motd0;
extern vmCvar_t		server_motd1;
extern vmCvar_t		server_motd2;
extern vmCvar_t		server_motd3;
extern vmCvar_t		server_motd4;
extern vmCvar_t		server_motd5;
extern vmCvar_t		team_panzerRestriction; // IRATA
extern vmCvar_t		team_maxPanzers;
extern vmCvar_t		team_maxMortars;
extern vmCvar_t		team_maxMG42s;
extern vmCvar_t		team_maxFlamers;
extern vmCvar_t		team_maxRiflegrenades;
extern vmCvar_t		team_maxVenoms;			// jet Pilot
extern vmCvar_t		team_maxShotguns;		// jet Pilot - per request
extern vmCvar_t		team_maxLMGs;
extern vmCvar_t		team_maxScopes;
extern vmCvar_t		team_maxIsPercentage;

// jaquboss - cos it is pro
extern vmCvar_t		team_maxSoldiers;
extern vmCvar_t		team_maxMedics;
extern vmCvar_t		team_maxEngineers;
extern vmCvar_t		team_maxFieldops;
extern vmCvar_t		team_maxCovertops;

extern vmCvar_t		team_maxplayers;
extern vmCvar_t		team_nocontrols;
//
// NOTE!!! If any vote flags are added, MAKE SURE to update the voteFlags struct in bg_misc.c w/appropriate info,
//         menudef.h for the mask and g_main.c for vote_allow_* flag updates
//
extern vmCvar_t		vote_allow_comp;
extern vmCvar_t		vote_allow_gametype;
extern vmCvar_t		vote_allow_kick;
extern vmCvar_t		vote_allow_map;
extern vmCvar_t		vote_allow_matchreset;
extern vmCvar_t		vote_allow_mutespecs;
extern vmCvar_t		vote_allow_nextmap;
extern vmCvar_t		vote_allow_pub;
extern vmCvar_t		vote_allow_referee;
extern vmCvar_t		vote_allow_shuffleteamsxp;
extern vmCvar_t		vote_allow_shuffleteamsxp_norestart;
extern vmCvar_t		vote_allow_swapteams;
extern vmCvar_t		vote_allow_friendlyfire;
extern vmCvar_t		vote_allow_timelimit;
extern vmCvar_t		vote_allow_warmupdamage;
extern vmCvar_t		vote_allow_antilag;
extern vmCvar_t		vote_allow_balancedteams;
extern vmCvar_t		vote_allow_muting;
extern vmCvar_t		vote_allow_surrender;
extern vmCvar_t		vote_allow_restartcampaign;
extern vmCvar_t		vote_allow_nextcampaign;
extern vmCvar_t		vote_allow_poll;
extern vmCvar_t		vote_allow_maprestart;
extern vmCvar_t		vote_limit;
extern vmCvar_t		vote_percent;
extern vmCvar_t		z_serverflags;
extern vmCvar_t		g_letterbox;

extern vmCvar_t		g_debugSkills;
extern vmCvar_t		g_heavyWeaponRestriction;
extern vmCvar_t		g_autoFireteams;

extern vmCvar_t		g_nextmap;
extern vmCvar_t		g_nextcampaign;

extern vmCvar_t		g_disableComplaints;

extern vmCvar_t		g_fixedphysics;
extern vmCvar_t		g_fixedphysicsfps;

#define				WIF_DROPMAIN			1  // -- drop main gun on death, default on
#define				WIF_DROPPISTOLS			2  // -- drop secondary on death, default off
#define				WIF_PICKUPAMMO			4  // -- pickup ammo reserve dropped with weapons, default off
#define				WIF_KEEPWEAPONITEM		8  // -- don't remove weapon items when they are empty, default off
#define				WIF_KEEPWEAPONITEM2		16 // -- don't remove weapons if we didn't pickup anything from them
#define				WIF_NOGARANDAMMOCLIP	32 // -- don't pickup clip from garands, default off

extern vmCvar_t		g_weaponItems; //jaquboss
// tjw
extern vmCvar_t		g_shrubbot;
extern vmCvar_t		g_playDead;
extern vmCvar_t		g_shove;
extern vmCvar_t 	g_dragCorpse;
extern vmCvar_t 	g_classChange;
extern vmCvar_t 	g_forceLimboHealth;
extern vmCvar_t		g_privateMessages;
extern vmCvar_t		g_XPSave;
extern vmCvar_t		g_XPSaveDirectory;

extern vmCvar_t		g_XPSaveMaxAge;
extern vmCvar_t 	g_weapons; // see WPF_* defines
extern vmCvar_t 	g_goomba;
extern vmCvar_t 	g_spawnInvul;
extern vmCvar_t 	g_teamChangeKills;

extern vmCvar_t 	g_logAdmin;
extern vmCvar_t 	team_maxLandmines;
extern vmCvar_t 	g_mapConfigs;
extern vmCvar_t		g_customConfig;
extern vmCvar_t 	g_dropHealth;
extern vmCvar_t 	g_dropAmmo;
extern vmCvar_t 	g_intermissionTime;
extern vmCvar_t 	g_intermissionReadyPercent;
//extern vmCvar_t 	g_hitboxes; // see HBF_* defines
extern vmCvar_t 	g_debugHitboxes;
extern vmCvar_t		g_debugPlayerHitboxes;
extern vmCvar_t 	g_voting; // see VF_* defines
extern vmCvar_t 	g_moverScale;
extern vmCvar_t 	g_ammoCabinetTime;
extern vmCvar_t 	g_healthCabinetTime;
extern vmCvar_t 	g_maxWarp;
extern vmCvar_t		g_antiwarp; // uses zinx etpro antiwarp. overrides g_maxWarp
extern vmCvar_t 	g_dropObj;
extern vmCvar_t 	g_serverInfo; // see SIF_* defines
extern vmCvar_t 	g_goombaFlags; // see GBF_* defines
extern vmCvar_t 	g_shoveNoZ;
extern vmCvar_t 	g_tyranny;
extern vmCvar_t 	g_mapScriptDirectory;
extern vmCvar_t 	g_campaignFile;
extern vmCvar_t 	g_fear;
extern vmCvar_t 	g_obituary;
extern vmCvar_t		g_shortcuts;
extern vmCvar_t		g_XPDecay;
extern vmCvar_t		g_XPDecayRate;
extern vmCvar_t		g_XPDecayFloor;
extern vmCvar_t		g_maxXP;

// Josh
extern vmCvar_t 	g_logOptions;
extern vmCvar_t 	g_censor;
extern vmCvar_t 	g_censorNames;
extern vmCvar_t 	g_censorPenalty;
extern vmCvar_t 	g_censorMuteTime;

// unlagged server options
extern vmCvar_t		sv_fps;
extern vmCvar_t		g_skipCorrection;
extern vmCvar_t		g_truePing;
// unlagged server options

// forty - canister kicking
extern vmCvar_t		g_canisterKick;
extern vmCvar_t		g_canisterKickOwner;
// dvl
extern vmCvar_t		g_slashKill;

#ifdef ET_LUA
// *LUA*
extern vmCvar_t lua_modules;
extern vmCvar_t lua_allowedModules;
#endif

// jaquboss
extern vmCvar_t		g_fastBackStab;
extern vmCvar_t		g_hitsounds;
extern vmCvar_t		g_missileGravity;


// Michael

// for changing amount of xp needed to level up each skill
extern vmCvar_t 	g_skillSoldier;
extern vmCvar_t 	g_skillMedic;
extern vmCvar_t 	g_skillEngineer;
extern vmCvar_t 	g_skillFieldOps;
extern vmCvar_t 	g_skillCovertOps;
extern vmCvar_t 	g_skillBattleSense;
extern vmCvar_t 	g_skillLightWeapons;

// matt
extern vmCvar_t 	g_teamDamageRestriction;
extern vmCvar_t 	g_minHits;
extern vmCvar_t 	g_autoTempBan;
extern vmCvar_t 	g_autoTempBanTime;
//extern vmCvar_t 	g_poison;
extern vmCvar_t 	g_medics;
extern vmCvar_t 	g_alliedSpawnInvul;
extern vmCvar_t 	g_axisSpawnInvul;
extern vmCvar_t 	g_msgs;
extern vmCvar_t 	g_msgpos;

// jet Pilot - new cvar external declarations
extern vmCvar_t		g_doubleJump;
extern vmCvar_t		g_doubleJumpHeight;

// MAPVOTE
extern vmCvar_t g_mapVoteFlags;
extern vmCvar_t g_maxMapsVotedFor;
extern vmCvar_t g_minMapAge;
extern vmCvar_t g_excludedMaps;
extern vmCvar_t g_resetXPMapCount;
// MAPVOTE END

// forty - arty/airstrike rate limiting
extern vmCvar_t		team_airstrikeTime;
extern vmCvar_t		team_artyTime;

// forty - constructible xp sharing
extern vmCvar_t		g_constructiblexpsharing;

// forty - airstrike block
extern vmCvar_t		g_asblock;

extern vmCvar_t		jp_falloff;
extern vmCvar_t		jp_keepAwards;
extern vmCvar_t		jp_drawBB;
extern vmCvar_t		jp_insanity;
extern vmCvar_t		nq_War;
extern vmCvar_t		g_weaponBounce;
extern vmCvar_t		g_throwDistance;
extern vmCvar_t		g_realHead;

extern vmCvar_t		g_missileSpeed;
extern vmCvar_t		g_missileHealth;
extern vmCvar_t		g_mortarBBox;
extern vmCvar_t		g_flushItems;


extern vmCvar_t		g_announcer;

extern vmCvar_t		g_headshot;
extern vmCvar_t		g_bulletReflect;

#ifdef MV_SUPPORT
extern vmCvar_t		g_multiview;
#endif

extern vmCvar_t		g_customChat;

extern vmCvar_t		g_spectator;
extern vmCvar_t		g_realism;
extern vmCvar_t		g_adrenaline;
extern vmCvar_t		g_noAttacklInvul;

//mcwf GeoIP
extern vmCvar_t		g_countryflags; //mcwf GeoIP

extern vmCvar_t		g_simpleBullets;

//-- core: configstring handling method
extern vmCvar_t		g_csMethod;		// latched, to be set by admin..
extern vmCvar_t		csMethod;		// readonly, used by code..
extern vmCvar_t		g_csStrings;	// # strings in a packet..
//---

// core: unlock weapons per class..
extern vmCvar_t		g_unlockWeapons;

// core: The duration (in seconds) that a killer-cam will be active..
extern vmCvar_t		g_killerLockTime;

// core: a mapsscript-mode cvar..
extern vmCvar_t		nq_scriptMode;

// core: player spawning behavior cvar..
extern vmCvar_t		nq_playerspawning;


typedef struct GeoIPTag {
	fileHandle_t GeoIPDatabase;
	unsigned char * cache;
	unsigned int memsize;
} GeoIP;

unsigned long GeoIP_addr_to_num(const char *addr);
unsigned int GeoIP_seek_record(GeoIP *gi, unsigned long ipnum);
void GeoIP_open(void);
void GeoIP_close(void);

extern GeoIP * gidb;
//mcwf GeoIP

void	trap_Printf( const char *fmt );
void	trap_Error( const char *fmt );
int		trap_Milliseconds( void );
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );
int		trap_FS_Write( const void *buffer, int len, fileHandle_t f );
int		trap_FS_Rename( const char *from, const char *to );
void	trap_FS_FCloseFile( fileHandle_t f );
int		trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
void	trap_SendConsoleCommand( int exec_when, const char *text );
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void	trap_DropClient( int clientNum, const char *reason, int length );
void	trap_SendServerCommand( int clientNum, const char *text );
void	trap_SetConfigstring( int num, const char *string );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetUserinfo( int num, char *buffer, int bufferSize );
void	trap_SetUserinfo( int num, const char *buffer );
void	trap_GetServerinfo( char *buffer, int bufferSize );
void	trap_SetBrushModel( gentity_t *ent, const char *name );
void	trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceCapsuleNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int		trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void	 trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void	 trap_LinkEntity( gentity_t *ent );
void	 trap_UnlinkEntity( gentity_t *ent );
int		 trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
qboolean trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );

#ifdef OMNIBOTS
int		trap_BotAllocateClient( int clientNum );
int		trap_BotGetServerCommand(int clientNum, char *message, int size);
void	trap_BotUserCommand(int client, usercmd_t *ucmd);
void	trap_EA_Command(int client, char *command);
#endif

void	 trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean trap_GetEntityToken( char *buffer, int bufferSize );
qboolean trap_GetTag( int clientNum, int tagFileNumber, char *tagName, orientation_t *orientation );
qboolean trap_LoadTag( const char* filename );

int		trap_RealTime( qtime_t *qtime );

int		trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points);
void	trap_DebugPolygonDelete(int id);

int		trap_GeneticParentsAndChildSelection(int numranks, float *ranks, int *parent1, int *parent2, int *child);

void	trap_SnapVector( float *v );

void	trap_SendMessage( int clientNum, char *buf, int buflen );
messageStatus_t	trap_MessageStatus( int clientNum );

void G_ExplodeMissile( gentity_t *ent );

void Svcmd_StartMatch_f(void);
void Svcmd_ResetMatch_f(qboolean fDoReset, qboolean fDoRestart);
void Svcmd_SwapTeams_f(void);

void trap_PbStat ( int clientNum , char *category , char *values ) ;

// g_antilag.c
void G_StoreClientPosition( gentity_t* ent );
void G_AdjustClientPositions( gentity_t* ent, int time, qboolean forward);
void G_ResetMarkers( gentity_t* ent );
void G_HistoricalTrace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void G_HistoricalTraceBegin( gentity_t *ent );
void G_HistoricalTraceEnd( gentity_t *ent );
void G_Trace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, qboolean ignoreCorpses );
void G_AdjustSingleClientPosition( gentity_t* ent, int time, gentity_t* debugger);
void G_ReAdjustSingleClientPosition( gentity_t* ent );
void G_PredictPmove(gentity_t *ent, float frametime);

#define BODY_VALUE(ENT) ENT->watertype
#define BODY_TEAM(ENT) ENT->s.modelindex
#define BODY_CLASS(ENT) ENT->s.modelindex2
#define BODY_CHARACTER(ENT) ENT->s.onFireStart

//g_buddy_list.c

#define MAX_FIRE_TEAMS 8

typedef struct {
	char name[32];
	char clientbits[8];
	char requests[8];
	int leader;
	qboolean open;
	qboolean valid;
} fireteam_t;

void Cmd_FireTeam_MP_f(gentity_t* ent);
int G_IsOnAFireTeam(int clientNum);
qboolean G_IsOnFireteam(int entityNum, fireteamData_t** teamNum);

void G_RemoveFromAllIgnoreLists( int clientNum );


//g_teammapdata.c
typedef struct mapEntityData_s {
	vec3_t			org;
	int				yaw;
	int				data;
	char			type;
	int				startTime;
	int				singleClient;

	int				status;
	int				entNum;
	struct mapEntityData_s *next, *prev;
} mapEntityData_t;

typedef struct mapEntityData_Team_s {
	mapEntityData_t mapEntityData_Team[MAX_GENTITIES];
	mapEntityData_t *freeMapEntityData;					// single linked list
	mapEntityData_t activeMapEntityData;				// double linked list
} mapEntityData_Team_t;

extern mapEntityData_Team_t mapEntityData[2];

void G_InitMapEntityData( mapEntityData_Team_t *teamList );
mapEntityData_t *G_FreeMapEntityData( mapEntityData_Team_t *teamList, mapEntityData_t *mEnt );
mapEntityData_t *G_AllocMapEntityData( mapEntityData_Team_t *teamList );
mapEntityData_t *G_FindMapEntityData( mapEntityData_Team_t *teamList, int entNum );
mapEntityData_t *G_FindMapEntityDataSingleClient( mapEntityData_Team_t *teamList, mapEntityData_t *start, int entNum, int clientNum );

void G_ResetTeamMapData();
void G_UpdateTeamMapData();

void G_SetupFrustum( gentity_t* ent );
void G_SetupFrustum_ForBinoculars( gentity_t* ent );
qboolean G_VisibleFromBinoculars ( gentity_t* viewer, gentity_t* ent, vec3_t origin );

void G_LogTeamKill(		gentity_t* ent,	weapon_t weap );
void G_LogDeath(		gentity_t* ent,	weapon_t weap );
void G_LogKill(			gentity_t* ent,	weapon_t weap );
void G_LogRegionHit(	gentity_t* ent, hitRegion_t hr, qboolean wasAlive );

// Skills
void G_ResetXP(gentity_t *ent);
void G_SetPlayerScore( gclient_t *client );
void G_SetPlayerSkill( gclient_t *client, skillType_t skill );
void G_AddSkillPoints( gentity_t *ent, skillType_t skill, float points );
void G_LoseSkillPoints( gentity_t *ent, skillType_t skill, float points );
void G_XPDecay(gentity_t *ent, int seconds, qboolean force);
// jet Pilot - modified G_AddKillSkillPoints() to award bounty xp for ending killing sprees
void G_AddKillSkillPoints( gentity_t *attacker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash, float bounty );
void G_AddKillSkillPointsForDestruction( gentity_t *attacker, meansOfDeath_t mod, g_constructible_stats_t *constructibleStats );
void G_LoseKillSkillPoints( gentity_t *tker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash );

void G_DebugOpenSkillLog( void );
void G_DebugCloseSkillLog( void );
void G_DebugAddSkillLevel( gentity_t *ent, skillType_t skill );
void G_DebugAddSkillPoints( gentity_t *ent, skillType_t skill, float points, const char *reason );
void G_ReassignSkillLevel( skillType_t );

#define G_DEBUG_ADD_SKILL_POINTS(ent, skill, points, reason) \
	if (g_debugSkills.integer) G_DebugAddSkillPoints(ent, skill, points, reason)


void G_CheckForNeededClasses( team_t team );
void G_CheckMenDown( team_t team );
void G_SendMapEntityInfo( gentity_t* e );
qboolean G_SendSystemMessage( sysMsg_t message, int team );
int G_GetSysMessageNumber( const char* sysMsg );
int G_CountTeamLandmines ( team_t team );
qboolean G_SweepForLandmines( vec3_t origin, float radius, int team );
qboolean G_SweepForArty( vec3_t origin, float radius);

void G_AddClientToFireteam( int entityNum, int leaderNum );
void G_InviteToFireTeam( int entityNum, int otherEntityNum );
void G_UpdateFireteamConfigString(fireteamData_t* ft);
void G_RemoveClientFromFireteams( int entityNum, qboolean update, qboolean print );

void G_PrintClientSpammyCenterPrint(int entityNum, char* text);

// Match settings
#define PAUSE_NONE		0x00	// Match is NOT paused.
#define PAUSE_UNPAUSING	0x01	// Pause is about to expire

// HRESULTS
#define G_OK		 0
#define G_INVALID	-1
#define G_NOTFOUND	-2

#define AP(x) trap_SendServerCommand(-1, x)					// Print to all
#define CP(x) if (!(g_entities[ent-g_entities].r.svFlags & SVF_BOT)) trap_SendServerCommand(ent-g_entities, x)		// Print to an ent, but not to bots..
#define CPx(x, y) if (!(g_entities[x].r.svFlags & SVF_BOT)) trap_SendServerCommand(x, y)							// Print to id = x, but not to bots..
#define SP(x) G_shrubbot_print(ent, x)
#define SPC(x) G_shrubbot_print_chat(ent, x)

// tjw: like SP() only uses buffering to reduce client commands
#define SBP(x) G_shrubbot_buffer_print(ent, x)
#define SBP_begin() G_shrubbot_buffer_begin()
#define SBP_end() G_shrubbot_buffer_end(ent)

#define PAUSE_NONE		0x00	// Match is NOT paused.
#define PAUSE_UNPAUSING	0x01	// Pause is about to expire

#define ZSF_COMP		0x01	// Have comp settings loaded for current gametype?

#define HELP_COLUMNS	4

#define CMD_DEBOUNCE	5000	// 5s between cmds

#define EOM_WEAPONSTATS	0x01	// Dump of player weapon stats at end of match.
#define EOM_MATCHINFO	0x02	// Dump of match stats at end of match.

#define AA_STATSALL		0x01	// Client AutoAction: Dump ALL player stats
#define AA_STATSTEAM	0x02	// Client AutoAction: Dump TEAM player stats


// "Delayed Print" ent enumerations
typedef enum {
	DP_PAUSEINFO,		// Print current pause info
	DP_UNPAUSING,		// Print unpause countdown + unpause
	DP_CONNECTINFO,		// Display OSP info on connect
	DP_MVSPAWN			// Set up MV views for clients who need them
} enum_t_dp;


// Remember: Axis = RED, Allies = BLUE ... right?!

// Team extras
typedef struct {
	qboolean	spec_lock;
	qboolean	team_lock;
	char		team_name[24];
	int			team_score;
	int			timeouts;
} team_info;

///////////////////////
// g_main.c
//
void G_UpdateChargeTimes(void);
void G_UpdateCvars(void);
void G_wipeCvars(void);
void CheckVote(void);

///////////////////////
// g_cmds_ext.c
//
qboolean G_commandCheck(gentity_t *ent, char *cmd, qboolean fDoAnytime);
qboolean G_commandHelp(gentity_t *ent, char *pszCommand, unsigned int dwCommand);
qboolean G_cmdDebounce(gentity_t *ent, const char *pszCommand);
void G_commands_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_lock_cmd(gentity_t *ent, unsigned int dwCommand, qboolean state);
void G_pause_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_players_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_ready_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_say_teamnl_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_scores_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_specinvite_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fLock);
void G_speclock_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fLock);
void G_statsall_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_teamready_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_weaponRankings_cmd(gentity_t *ent, unsigned int dwCommand, qboolean state);
void G_weaponStats_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_weaponStatsLeaders_cmd(gentity_t* ent, qboolean doTop, qboolean doWindow);
void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceOnly, float randomNum );

///////////////////////
// g_config.c
//
void G_configSet(int mode, qboolean doComp);

///////////////////////
// g_match.c
//
void G_addStats(gentity_t *targ, gentity_t *attacker, int dmg_ref, int mod);
void G_addStatsHeadShot(gentity_t *attacker, int mod);
qboolean G_allowPanzer(gentity_t *ent);
int G_checkServerToggle(vmCvar_t *cv);
char *G_createStats(gentity_t *refEnt);
void G_deleteStats(int nClient);
qboolean G_desiredFollow(gentity_t *ent, int nTeam);
void G_globalSound(char *sound);
void G_globalSoundEnum(int sound);
void G_initMatch(void);
void G_loadMatchGame(void);
void G_matchInfoDump(unsigned int dwDumpType);
void G_printMatchInfo(gentity_t *ent);
void G_parseStats(char *pszStatsInfo);
void G_printFull(char *str, gentity_t *ent);
void G_resetModeState(void);
void G_resetRoundState(void);
void G_spawnPrintf(int print_type, int print_time, gentity_t *owner);
void G_statsPrint(gentity_t *ent, int nType);
unsigned int G_weapStatIndex_MOD(unsigned int iWeaponMOD);

///////////////////////
// g_multiview.c
//
#ifdef MV_SUPPORT
qboolean G_smvCommands(gentity_t *ent, char *cmd);
void G_smvAdd_cmd(gentity_t *ent);
void G_smvAddTeam_cmd(gentity_t *ent, int nTeam);
void G_smvDel_cmd(gentity_t *ent);
//
void G_smvAddView(gentity_t *ent, int pID);
void G_smvAllRemoveSingleClient(int pID);
unsigned int G_smvGenerateClientList(gentity_t *ent);
qboolean G_smvLocateEntityInMVList(gentity_t *ent, int pID, qboolean fRemove);
void G_smvRegenerateClients(gentity_t *ent, int clientList);
void G_smvRemoveEntityInMVList(gentity_t *ent, mview_t *ref);
void G_smvRemoveInvalidClients(gentity_t *ent, int nTeam);
qboolean G_smvRunCamera(gentity_t *ent);
void G_smvUpdateClientCSList(gentity_t *ent);
#endif

///////////////////////
// g_referee.c
//
void Cmd_AuthRcon_f(gentity_t *ent);
void G_refAllReady_cmd(gentity_t *ent);
void G_ref_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
qboolean G_refCommandCheck(gentity_t *ent, char *cmd);
void G_refHelp_cmd(gentity_t *ent);
void G_refLockTeams_cmd(gentity_t *ent, qboolean fLock);
void G_refPause_cmd(gentity_t *ent, qboolean fPause);
void G_refPlayerPut_cmd(gentity_t *ent, int team_id);
void G_refRemove_cmd(gentity_t *ent);
void G_refSpeclockTeams_cmd(gentity_t *ent, qboolean fLock);
void G_refWarmup_cmd(gentity_t* ent);
void G_refWarning_cmd(gentity_t* ent);
void G_refMute_cmd(gentity_t *ent, qboolean mute);
int  G_refClientnumForName(gentity_t *ent, const char *name);
void G_refPrintf(gentity_t* ent, const char *fmt, ...)_attribute((format(printf,2,3)));
void G_PlayerBan(void);
void G_MakeReferee(void);
void G_RemoveReferee(void);
void G_MuteClient(void);
void G_UnMuteClient(void);

///////////////////////
// g_team.c
//
extern char *aTeams[TEAM_NUM_TEAMS];
extern team_info teamInfo[TEAM_NUM_TEAMS];

qboolean G_allowFollow(gentity_t *ent, int nTeam);
int G_blockoutTeam(gentity_t *ent, int nTeam);
qboolean G_checkReady(void);
qboolean G_readyMatchState(void);
void G_removeSpecInvite(int team);
void G_shuffleTeams(void);
void G_swapTeamLocks(void);
void G_swapTeams(void);
qboolean G_teamJoinCheck(int team_num, gentity_t *ent);
int  G_teamID(gentity_t *ent);
void G_teamReset(int team_num, qboolean fClearSpecLock);
void G_verifyMatchState(int team_id);
void G_updateSpecLock(int nTeam, qboolean fLock);

///////////////////////
// g_vote.c
//
int  G_voteCmdCheck(gentity_t *ent, char *arg, char *arg2, qboolean fRefereeCmd);
void G_voteFlags(void);
void G_voteHelp(gentity_t *ent, qboolean fShowVote);
void G_playersMessage(gentity_t *ent);
// Actual voting commands
int G_Comp_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Gametype_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Kick_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Mute_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_UnMute_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Map_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Campaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_MapRestart_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_MatchReset_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Mutespecs_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Nextmap_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Pub_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Referee_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_ShuffleTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_ShuffleTeams_NoRestart_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_StartMatch_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_SwapTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_FriendlyFire_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Timelimit_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Warmupfire_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Unreferee_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_AntiLag_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_BalancedTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Surrender_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_RestartCampaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_NextCampaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Poll_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);

void G_LinkDebris( void );
void G_LinkDamageParents( void );
int EntsThatRadiusCanDamage( vec3_t origin, float radius, int *damagedList );

qboolean G_LandmineTriggered( gentity_t* ent );
qboolean G_LandmineArmed( gentity_t* ent );
qboolean G_LandmineUnarmed( gentity_t* ent );
team_t G_LandmineTeam( gentity_t* ent );
qboolean G_LandmineSpotted( gentity_t* ent );
void G_SetTargetName( gentity_t* ent, char* targetname );
void G_KillEnts( const char* target, gentity_t* ignore, gentity_t* killer, meansOfDeath_t mod );
void trap_EngineerTrace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );

qboolean G_ConstructionIsPartlyBuilt( gentity_t* ent );

int G_CountTeamMedics( team_t team, qboolean alivecheck );
qboolean G_TankIsOccupied( gentity_t* ent );
qboolean G_TankIsMountable( gentity_t* ent, gentity_t* other );

qboolean G_ConstructionIsDestroyable( gentity_t* ent );
qboolean G_ConstructionBegun( gentity_t* ent );
qboolean G_ConstructionIsFullyBuilt( gentity_t* ent );
qboolean G_ConstructionIsPartlyBuilt( gentity_t* ent );
gentity_t* G_ConstructionForTeam( gentity_t* toi, team_t team );
gentity_t* G_IsConstructible( team_t team, gentity_t* toi );
qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other );
qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other );
void G_CheckForCursorHints(gentity_t *ent);
void G_CalcClientAccuracies( void );
void G_BuildEndgameStats( void );
void jP_BuildEndgameStats(void);
int G_TeamCount( gentity_t* ent, weapon_t weap );

qboolean G_IsFireteamLeader( int entityNum, fireteamData_t** teamNum );
fireteamData_t* G_FindFreePublicFireteam( team_t team );
void G_RegisterFireteam(/*const char* name,*/ int entityNum);

void weapon_callAirStrike(gentity_t *ent);
void weapon_checkAirStrikeThink2( gentity_t *ent );
void weapon_checkAirStrikeThink1( gentity_t *ent );
void weapon_callSecondPlane( gentity_t *ent );
qboolean weapon_checkAirStrike( gentity_t *ent );


void G_MakeReady( gentity_t* ent );
void G_MakeUnready( gentity_t* ent );

void SetPlayerSpawn( gentity_t* ent, int spawn, qboolean update );
void G_UpdateSpawnCounts( void );

void G_SetConfigStringValue( int num, const char* key, const char* value );
void G_GlobalClientEvent( int event, int param, int client );

void G_InitTempTraceIgnoreEnts( void );
void G_ResetTempTraceIgnoreEnts( void );
void G_ResetTempTraceIgnorePlayersAndBodies( void );
void G_TempTraceIgnoreEntity( gentity_t* ent );
void G_TempTraceIgnorePlayersAndBodies( void );

qboolean G_CanPickupWeapon( weapon_t weapon, gentity_t* ent );

qboolean G_LandmineSnapshotCallback( int entityNum, int clientNum );

#include "g_shrubbot.h"
#include "g_xpsave.h"
extern g_shrubbot_level_t	*g_shrubbot_levels[MAX_SHRUBBOT_LEVELS];
extern g_shrubbot_admin_t	*g_shrubbot_admins[MAX_SHRUBBOT_ADMINS];
extern g_shrubbot_ban_t		*g_shrubbot_bans[MAX_SHRUBBOT_BANS];

#define CH_KNIFE_DIST           64      // from g_weapon.c
#define CH_LADDER_DIST          100
#define CH_WATER_DIST           100
#define CH_BREAKABLE_DIST       64
#define CH_DOOR_DIST            96
#define CH_ACTIVATE_DIST        96
#define CH_EXIT_DIST            256
#define CH_FRIENDLY_DIST        1024
#define CH_REVIVE_DIST          64
#define CH_MAX_DIST				1024    // use the largest value from above
#define CH_MAX_DIST_ZOOM        8192    // max dist for zooming hints

// tjw: g_XPSave flags
#define XPSF_ENABLE					1	// enable XP Save on disconnect
#define XPSF_NR_MAPRESET			2	// no reset on map restarts
#define XPSF_NR_EVER				4	// no reset ever
#define XPSF_WIPE_DUP_GUID			8	// call ClientDisconnect() on clients with the same GUID as
										// the connecting client even if sv_wwwDlDisconnected is enabled.
#define XPSF_DISABLE_STOPWATCH		16	// do not use xp-save when playing stopwatch
#define XPSF_CONVERT				32  // if enabled the server tries to convert the very old xp file format (in one file) to single file based system - see g_XPSaveDirectory

// tjw: g_voting flags
#define VOTEF_USE_TOTAL_VOTERS		1	// use total voters instead of total players to decide if a vote passes
#define VOTEF_NO_POPULIST_PENALTY	2	// successful votes do not count against vote_limit
#define VOTEF_DISP_CALLER			4	// append "(called by name)" in vote string

// tjw: g_spectator flags
#define SPECF_FL_CLICK_FOLLOW		1	// freelook specs can click another player to follow
//#define SPECF_FL_MISS_FOLLOW_NEXT	2	// when 'missing' another player in freelook mode, go to next available client
//#define SPECF_PERSIST_FOLLOW		4	// specs continue following the same player when he goes into limbo
//#define SPECF_FL_ON_LIMBO			8	// specs go into freelook instead of following next available player

// tjw: g_serverInfo flags
#define SIF_P						1	// display team information with P
#define SIF_PLAYERS					2	// display team information with Players_Allies and Players_Axis
#define SIF_CAMPAIGN_MAPS			4	// display semi-colon delimited list of campaign maps
#define SIF_C						8	// display current campaign status in form xx,yy
#define SIF_CHARGE					16	// display charge times

// tjw: g_goombaFlags
#define GBF_ENEMY_ONLY				1	// goomba can only damage enemies
#define GBF_NO_HOP_DAMAGE			2	// EV_FALL_SHORT (hopping) does not do damage
#define GBF_NO_HOP_TEAMDAMAGE		4	// EV_FALL_SHORT does not to damage to teammates
#define GBF_NO_SELF_DAMAGE			8	// goomba doesn't do any damage to faller
#define GBF_INSTAGIB				16	// instagib goomba damage

// Josh: g_logOptions flags, only for GUIDs now (no WeaponStatsDMG or TStats)
#define LOGOPTS_OBIT_CHAT		1		// Server puts obituaries in chat.
#define LOGOPTS_GUID			128		// Log the GUIDs, had other functionality in shrubet
#define LOGOPTS_PRIVMSG			256		// log all private messages
#define LOGOPTS_REALTIME		512		// real time stamps in log (dvl)
#define LOGOPTS_IP				1024	// Log the IP in case of LOGOPTS_GUID is set for
										// ClientUserinfoChangedGUID, ClientConnect & ClientDisconnect
										// Adds also player name in some cases

// #define LOGOPTS_TK_WEAPON		2048	// print out how you TK'ed

// Josh: g_censorPenalty flags
#define CNSRPNLTY_KILL				1	// Gibs unless CNSRPNLTY_NOGIB
#define CNSRPNLTY_KICK				2	// Kick if the word is in their name
#define CNSRPNLTY_NOGIB				4	// Won't GIB, only kill
#define CNSRPNLTY_TMPMUTE			8	// Muted the next g_censorMuteTime.integer seconds
#define CNSRPNLTY_XP				16  // forty - #127 g_censorXP.integer
#define CNSRPNLTY_BURN				32  // forty - #127 Burn a potty mouth
#define CNSRPNLTY_POISON			64  // Meyer - poison the bastard

// dvl (used in the g_slashKill cvar)
#define SLASHKILL_HALFCHARGE		1	// Half a charge bar on /kill
#define SLASHKILL_ZEROCHARGE		2	// No charge on a /kill
#define SLASHKILL_SAMECHARGE		4	// Same charge as a client had upon the /kill
#define SLASHKILL_NOKILL			8	// cannot use /kill
#define SLASHKILL_NOPOISON			16	// cannot /kill while poisoned
#define SLASHKILL_NORMALCHARGE		32  // while in limbo after a /kill, charge bar fills normally
#define SLASHKILL_NOINFIGHT			64  // cannot use /kill while in a fight
#define SLASHKILL_NOFREEZE			128  // cannot use /kill while freezed

// tjw: g_XPDecay
#define XPDF_ENABLE					1	// don't decay xp when the client is disconnected (XPSave)
#define XPDF_NO_DISCONNECT_DECAY	2	// don't decay xp specific for the clients class
#define XPDF_NO_CLASS_DECAY			4	// don't decay xp if the client is on specators
#define XPDF_NO_SPEC_DECAY			8	// don't decay xp unless the gamestate is GS_PLAYING
#define XPDF_NO_GAMESTATE_DECAY		16	// don't decay xp while a player is actively playing
#define XPDF_NO_PLAYING_DECAY		32	// don't decay battle sense
#define XPDF_NO_BS_DECAY			64  // don't decay light weapons
#define XPDF_NO_LW_DECAY			128

// matt MAPVOTE
// used when mapvoting is enabled
#define MAPVOTE_TIE_LEASTPLAYED 1
#define MAPVOTE_ALT_INTERMISSION 2
#define MAPVOTE_MULTI_VOTE 4
#define MAPVOTE_NO_RANDOMIZE 8
#define MAPVOTE_NEXTMAP_VOTEMAP 16

// Position of messages (?jP)
#define MSGPOS_CHAT					0
#define MSGPOS_CENTER				1
#define MSGPOS_LEFT_BANNER			2


qboolean G_FlingClient( gentity_t *vic, int flingType );

void	 G_createClusterNade( gentity_t *ent, int numNades );
void	 G_MakePip( gentity_t *vic );
void	 G_PrintBanners();

// forty - airstrike block
#define ASBLOCK_ANNOUNCE		1
#define ASBLOCK_EASY			2
// Not implemented but reserving since it's in the shrub docs
#define ASBLOCK_PLAY_SOUND 4
#define ASBLOCK_LVL3_FDOPS_NOBLOCK 8


extern wordDictionary censorDictionary;
extern wordDictionary censorNamesDictionary;

// forty - canister kicking
void G_CanisterKick();


void jP_GetDurationString(int secs, char *dur, int len);

int G_GetEnemyPosition(gentity_t *ent, gentity_t *targ);

// forty - stack traces - g_crash.h
void EnableCoreDumps();
void DisableCoreDumps();
void EnableStackTrace();
void DisableStackTrace();

void G_ShutdownGame( int restart );

// MAPVOTE
void G_mapvoteinfo_write( void );
void G_mapvoteinfo_read( void );

// jaquboss
void G_RailTrail( vec_t* start, vec_t* end, vec_t* color );
void G_RailBox( vec_t* origin, vec_t* mins, vec_t* maxs, vec_t* color, int index );

typedef enum {
	F_INT,
	F_FLOAT,
	F_LSTRING,			// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,			// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_ENTITY,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;

typedef struct
{
	char		*name;
	int			ofs;
	fieldtype_t	type;
	int			flags;
} field_t;

#define G_FRIENDLYFIRE (g_friendlyFire.integer & 1 )

// IlDuca - add core's functions definitions defined in g_spawn.c
int GetFieldIndex( char *fieldname );
qboolean FieldExists( char *fieldname );
fieldtype_t GetFieldType( char *fieldname );
char *GetFieldValueAsString( int entnum, char *fieldname );
qboolean SetFieldValueFromString( int entnum, char *fieldname, char *value );

// core: defined in g_utils.c
void G_ChatMessage( char *message );
void G_ChatMessageForConstruction( gentity_t *player, gentity_t *TOI, int msgType );
void G_ChatMessageForDynamite( gentity_t *player, gentity_t *TOI, int msgType );
void G_ChatMessageForDestruction( gentity_t *player, gentity_t *TOI, int msgType, int weapon );
void G_ChatMessageForObjective( gentity_t *player, gentity_t *obj_ent, int msgType );

// messages in the popup-area.. (left screen, above HUD-head)
void G_PopupMessageForConstruction( gentity_t *player, gentity_t *TOI, int msgType, int sound );
void G_PopupMessageForDynamite( gentity_t *player, gentity_t *TOI, int msgType, int sound );
void G_PopupMessageForDestruction( gentity_t *player, gentity_t *TOI, int msgType, int weapon, int sound );
void G_PopupMessageForObjective( gentity_t *player, gentity_t *obj_ent, int msgType, int sound );
void G_PopupMessageForMines( gentity_t *player, int sound );

// messages in the mission-area.. (top screen, below fireteam-overlay)
void G_MissionMessageForConstruction( gentity_t *player, gentity_t *TOI, int msgType, int sound );
void G_MissionMessageForDynamite( gentity_t *player, gentity_t *TOI, int msgType, int sound );
void G_MissionMessageForDestruction( gentity_t *player, gentity_t *TOI, int msgType, int weapon, int sound );
void G_MissionMessageForObjective( gentity_t *player, gentity_t *obj_ent, int msgType, int sound );
void G_MissionMessageForMines( gentity_t *player, int sound );

// messages in the area indicated by the g_announcer cvar..
void G_AnnounceMessageForConstruction( gentity_t *player, gentity_t *TOI, int msgType );
void G_AnnounceMessageForDynamite( gentity_t *player, gentity_t *TOI, int msgType );
void G_AnnounceMessageForDestruction( gentity_t *player, gentity_t *TOI, int msgType, int weapon );
void G_AnnounceMessageForObjective( gentity_t *player, gentity_t *obj_ent, int msgType );
void G_AnnounceMessageForMines( gentity_t *player );

// g_weapon.c
float G_DistanceDamageFalloff( float distance );

// g_script_actions.c
void CheckRespawnTimes( qboolean disabled );


// core: script_mover spawnflags
#define MOVER_TRIGGERSPAWN				1
#define MOVER_SOLID						2
#define MOVER_EXPLOSIVEDAMAGEONLY		4
#define MOVER_RESURECTABLE				8
#define MOVER_COMPASS					16
#define MOVER_ALLIED					32
#define MOVER_AXIS						64
#define MOVER_MOUNTEDGUN				128

// g_client.c
qboolean CGAMEFileExists( void );

// core: bitflags for the g_classchange cvar
#define CLASSCHANGE_ENABLE				1
#define CLASSCHANGE_KEEPCLASS			2

// core: bitflags for the nq_playerspawning cvar
#define PLAYERSPAWN_RANDOM					1	// players spawn at closest- or random-spawn location


#endif
