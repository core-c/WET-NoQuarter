#include "g_local.h"
#include "etpub.h"
#include "etpro_mdx.h"

// Include the "External"/"Public" components of AI_Team
#include "../botai/ai_teamX.h"

wordDictionary censorDictionary;
wordDictionary censorNamesDictionary;

level_locals_t	level;

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;	// for tracking changes
	qboolean	trackChange;		// track this variable, and announce if changed
	qboolean	fConfigReset;		// OSP: set this var to the default on a config reset
	qboolean	teamShader;			// track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];

g_campaignInfo_t g_campaigns[MAX_CAMPAIGNS];
int				saveGamePending;	// 0 = no, 1 = check, 2 = loading

mapEntityData_Team_t mapEntityData[2];

#ifdef OMNIBOTS
vmCvar_t	g_OmniBotPath;
vmCvar_t	g_OmniBotEnable;
#endif

vmCvar_t	g_gametype;
vmCvar_t	g_fraglimit;
vmCvar_t	g_timelimit;
vmCvar_t	g_friendlyFire;			// bitmaak for friendly fire modes
									// 0  - FF is disabled
									// 1  - FF is enabled
									// 2  - FF does half damage to teammates
									// 4  - FF does an equal amount of damage to the inflictor
									// 8  - FF affects other ents as well (tanks, etc)
									// 16 - reserved
vmCvar_t	g_password;
vmCvar_t	sv_privatepassword;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_minGameClients;		// NERVE - SMF
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_debugBullets;	//----(SA)	added
vmCvar_t	g_motd;
#ifdef ALLOW_GSYNC
vmCvar_t	g_synchronousClients;
#endif // ALLOW_GSYNC
vmCvar_t	g_warmup;

// NERVE - SMF
vmCvar_t	g_warmupLatch;
vmCvar_t	g_nextTimeLimit;
vmCvar_t	g_userTimeLimit;
vmCvar_t	g_userAlliedRespawnTime;
vmCvar_t	g_userAxisRespawnTime;
vmCvar_t	g_currentRound;
vmCvar_t	g_noTeamSwitching;
vmCvar_t	g_altStopwatchMode;
vmCvar_t	g_gamestate;
vmCvar_t	g_swapteams;
// -NERVE - SMF

vmCvar_t	g_restarted;
vmCvar_t	g_log;
vmCvar_t	g_logChat;
vmCvar_t	g_logCrash;
vmCvar_t	g_logSync;
vmCvar_t	g_podiumDist;
vmCvar_t	g_podiumDrop;
vmCvar_t	voteFlags;
vmCvar_t	g_complaintlimit;		// DHM - Nerve
vmCvar_t	g_ipcomplaintlimit;
vmCvar_t	g_filtercams;
vmCvar_t	g_maxlives;				// DHM - Nerve
vmCvar_t	g_maxlivesRespawnPenalty;
vmCvar_t	g_voiceChatsAllowed;	// DHM - Nerve
vmCvar_t	g_alliedmaxlives;		// Xian
vmCvar_t	g_axismaxlives;			// Xian
vmCvar_t	g_fastres;				// Xian
vmCvar_t	g_knifeonly;			// Xian
vmCvar_t	g_enforcemaxlives;		// Xian

vmCvar_t	g_needpass;
vmCvar_t	g_balancedteams;
vmCvar_t	g_doWarmup;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_rankings;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;

// Rafael
vmCvar_t	g_scriptName;			// name of script file to run (instead of default for that map)

vmCvar_t	g_developer;

vmCvar_t	g_userAim;

// JPW NERVE multiplayer reinforcement times
vmCvar_t		g_redlimbotime;
vmCvar_t		g_bluelimbotime;
// charge times for character class special weapons
vmCvar_t		g_medicChargeTime;
vmCvar_t		g_engineerChargeTime;
vmCvar_t		g_LTChargeTime;
vmCvar_t		g_soldierChargeTime;
// screen shakey magnitude multiplier

// Gordon
vmCvar_t		g_antilag;

// OSP
vmCvar_t		g_spectatorInactivity;
vmCvar_t		match_latejoin;
vmCvar_t		match_minplayers;
vmCvar_t		match_mutespecs;
vmCvar_t		match_readypercent;
vmCvar_t		match_timeoutcount;
vmCvar_t		match_timeoutlength;
vmCvar_t		match_warmupDamage;
vmCvar_t		server_autoconfig;
vmCvar_t		team_maxPanzers;
vmCvar_t		team_maxMG42s;
vmCvar_t		team_maxFlamers;
vmCvar_t		team_maxMortars;
vmCvar_t		team_maxriflegrenades;
vmCvar_t		team_maxVenoms;			// jet Pilot
vmCvar_t		team_maxShotguns;		// jet Pilot - per request

vmCvar_t		team_maxSoldiers;
vmCvar_t		team_maxMedics;
vmCvar_t		team_maxEngineers;
vmCvar_t		team_maxFieldops;
vmCvar_t		team_maxCovertops;

vmCvar_t		team_maxplayers;
vmCvar_t		team_nocontrols;
vmCvar_t		server_motd0;
vmCvar_t		server_motd1;
vmCvar_t		server_motd2;
vmCvar_t		server_motd3;
vmCvar_t		server_motd4;
vmCvar_t		server_motd5;
vmCvar_t		vote_allow_comp;
vmCvar_t		vote_allow_gametype;
vmCvar_t		vote_allow_kick;
vmCvar_t		vote_allow_map;
vmCvar_t		vote_allow_matchreset;
vmCvar_t		vote_allow_mutespecs;
vmCvar_t		vote_allow_nextmap;
vmCvar_t		vote_allow_pub;
vmCvar_t		vote_allow_referee;
vmCvar_t		vote_allow_shuffleteamsxp;
vmCvar_t		vote_allow_shuffleteamsxp_norestart;
vmCvar_t		vote_allow_swapteams;
vmCvar_t		vote_allow_friendlyfire;
vmCvar_t		vote_allow_timelimit;
vmCvar_t		vote_allow_warmupdamage;
vmCvar_t		vote_allow_antilag;
vmCvar_t		vote_allow_balancedteams;
vmCvar_t		vote_allow_muting;
vmCvar_t		vote_allow_surrender;
vmCvar_t		vote_allow_restartcampaign;
vmCvar_t		vote_allow_nextcampaign;
vmCvar_t		vote_allow_poll;
vmCvar_t		vote_allow_maprestart;
vmCvar_t		vote_limit;
vmCvar_t		vote_percent;
vmCvar_t		z_serverflags;

// forty - airstrike block  
vmCvar_t		g_asblock;  

vmCvar_t		g_covertopsChargeTime;
vmCvar_t		refereePassword;
vmCvar_t		g_debugConstruct;
vmCvar_t		g_landminetimeout;

// Variable for setting the current level of debug printing/logging
// enabled in bot scripts and regular scripts.
// Added by Mad Doctor I, 8/23/2002
vmCvar_t		g_scriptDebugLevel;
vmCvar_t		g_movespeed;

vmCvar_t 		g_axismapxp;
vmCvar_t 		g_alliedmapxp;
vmCvar_t 		g_oldCampaign;
vmCvar_t 		g_currentCampaign;
vmCvar_t 		g_currentCampaignMap;

// Arnout: for LMS
vmCvar_t 		g_axiswins;
vmCvar_t 		g_alliedwins;
vmCvar_t 		g_lms_teamForceBalance;
vmCvar_t 		g_lms_roundlimit;
vmCvar_t 		g_lms_matchlimit;
vmCvar_t 		g_lms_currentMatch;
vmCvar_t 		g_lms_lockTeams;
vmCvar_t 		g_lms_followTeamOnly;

vmCvar_t		url;

vmCvar_t		g_letterbox;
vmCvar_t		bot_enable;

vmCvar_t		g_debugSkills;
vmCvar_t		g_heavyWeaponRestriction;
vmCvar_t		g_autoFireteams;

vmCvar_t		g_nextmap;
vmCvar_t		g_nextcampaign;

vmCvar_t		g_disableComplaints;
// tjw: non config
vmCvar_t g_reset;

// tjw
vmCvar_t 		g_shrubbot;
vmCvar_t 		g_playDead;
vmCvar_t 		g_shove;
vmCvar_t 		g_dragCorpse;
vmCvar_t 		g_classChange;
vmCvar_t 		g_forceLimboHealth;
vmCvar_t 		g_privateMessages;
vmCvar_t 		g_XPSave;
vmCvar_t		g_XPSaveFile;
vmCvar_t		g_XPSaveDirectory;
//vmCvar_t		g_XPSaveMaxAge_xp;
vmCvar_t		g_XPSaveMaxAge;;
vmCvar_t 		g_weapons; // see WPF_ defines
vmCvar_t 		g_goomba;
vmCvar_t 		g_goombaXp;
vmCvar_t 		g_spawnInvul;
vmCvar_t 		g_teamChangeKills;

vmCvar_t 		g_logAdmin;
vmCvar_t 		team_maxLandmines;

vmCvar_t		g_weaponItems;

vmCvar_t 		g_mapConfigs;
vmCvar_t		g_customConfig;
vmCvar_t 		g_dropHealth;
vmCvar_t 		g_dropAmmo;
vmCvar_t 		g_intermissionTime;
vmCvar_t 		g_intermissionReadyPercent;
//vmCvar_t 		g_hitboxes; // see HBF_ defines
vmCvar_t 		g_debugHitboxes;
vmCvar_t 		g_debugPlayerHitboxes;
vmCvar_t 		g_voting; // see VF_ defines
vmCvar_t 		g_moverScale;
vmCvar_t 		g_ammoCabinetTime;
vmCvar_t 		g_healthCabinetTime;
vmCvar_t 		g_maxWarp;
vmCvar_t 		g_dropObj;
vmCvar_t 		g_serverInfo; // see SIF_ defines
vmCvar_t 		g_goombaFlags; // see GBF_ defines
//vmCvar_t 		g_shoveSound;
vmCvar_t 		g_shoveNoZ;
//vmCvar_t 		g_poisonSound;
vmCvar_t 		g_tyranny;
vmCvar_t		g_SBCheats;
vmCvar_t 		g_mapScriptDirectory;
vmCvar_t 		g_campaignFile;
vmCvar_t 		g_fear;
vmCvar_t		g_shortcuts; 
vmCvar_t		g_XPDecay; // see XPDF_ defines  
vmCvar_t		g_XPDecayRate;
vmCvar_t		g_XPDecayFloor;   
vmCvar_t		g_maxXP;

// Josh
vmCvar_t 		g_logOptions;
vmCvar_t 		g_censor;
vmCvar_t 		g_censorNames;
vmCvar_t 		g_censorPenalty;
vmCvar_t 		g_censorMuteTime;

//unlagged server options
vmCvar_t		sv_fps;
vmCvar_t		g_skipCorrection;
vmCvar_t		g_truePing;
//unlagged server options

// dvl
vmCvar_t		g_slashKill;

// Michael
vmCvar_t 		g_skillSoldier;
vmCvar_t 		g_skillMedic;
vmCvar_t 		g_skillEngineer;
vmCvar_t 		g_skillFieldOps;
vmCvar_t 		g_skillCovertOps;
vmCvar_t 		g_skillBattleSense;
vmCvar_t 		g_skillLightWeapons;

// matt
vmCvar_t 		g_teamDamageRestriction;
vmCvar_t 		g_minHits;
vmCvar_t 		g_autoTempBan;
vmCvar_t 		g_autoTempBanTime;
//vmCvar_t 		g_poison;
vmCvar_t 		g_medics;
vmCvar_t 		g_alliedSpawnInvul;
vmCvar_t 		g_axisSpawnInvul;
vmCvar_t		g_noAttacklInvul;
vmCvar_t 		g_msgs;
vmCvar_t 		g_msgpos;

// forty - arty/airstrike rate limiting  
vmCvar_t		team_airstrikeTime;
vmCvar_t		team_artyTime;

// forty - constructible xp sharing
vmCvar_t g_constructiblexpsharing;


// jet Pilot - new cvars

vmCvar_t	g_doubleJump;			// allow double jump
vmCvar_t	g_doubleJumpHeight;

vmCvar_t	g_shotgunPellets;

//vmCvar_t	g_maxLandminesRed;		// max landmines for the axis
//vmCvar_t	g_maxLandminesBlue;		// max landmines for the allies
//vmCvar_t	g_allowPersonalMines;	// can engineers have a personal stock of mines?

vmCvar_t	jp_maxLevelBattleSense;
vmCvar_t	jp_maxLevelLightWeapons;
vmCvar_t	jp_maxLevelSoldier;
vmCvar_t	jp_maxLevelMedic;		
vmCvar_t	jp_maxLevelEngineer;
vmCvar_t	jp_maxLevelFieldOp;
vmCvar_t	jp_maxLevelCovertOp;

vmCvar_t	jp_falloff;
vmCvar_t	jp_insanity;			// bitmask for Insanity Mode
			
		
vmCvar_t	jp_keepAwards;			// bitmask for keeping award powerups between classes
									// 0  - default ET powerups
									// 1  - keep flak jacket
									// 2  - keep adrenaline
									// 4  - keep enemy recognition
									// 8  - keep 
									// 16 - keep 

vmCvar_t	jp_drawBB;				// bitmask for drawing bounding boxes around entities
									// 0  - disabled
									// 1  - objectives
									// 2  - players
									// 4  - player heads
									// 8  - missiles
									// 16 - func_explosive (breakables)	

vmCvar_t	g_weaponBounce;			// Bounciness of dropped weapons
vmCvar_t	g_throwDistance;		// distance items are thrown (health packs, weapons, etc)
vmCvar_t	g_realHead;				// b_realHead functionality from ETPro
//vmCvar_t	g_missileFlags;			// bitmask for controlling how missiles are handled
									//  0  - normal missile behavior
									//  1  - guided missiles
									//  2  - homing missiles
									//  4  - missiles are affected by gravity
									//  8  - missiles are 'drunk' - erratic flight

vmCvar_t	g_missileSpeed;			// speed of rockets (0 = default speed, or 2500)
vmCvar_t	g_missileHealth;		// health of missiles, grenades, etc. if 0 then non damageable (default : 5)
vmCvar_t	g_flushItems;			// items land depending on the slope thy're on
vmCvar_t	g_mortarBBox;
vmCvar_t	g_grenadeBonus;			// bitmask
									// 0 - normal grenade loadout, ignore levels above 4
									// 1 - additional grenades for levels above 4
									// 2 - additional bonus grenades for high light-weapon skill


vmCvar_t	g_multikillSound[5];
vmCvar_t	g_spreeSound[10];
vmCvar_t	g_humiliationSound[6];
vmCvar_t	g_spreeMessage[10];
vmCvar_t	g_humiliationMessage[6];
vmCvar_t	g_announcer;
vmCvar_t	g_headshot;
vmCvar_t	g_multikillTime;
vmCvar_t	g_bulletReflect;
vmCvar_t    g_painAnims;
vmCvar_t    g_painChance;
// end of jP additions
vmCvar_t	g_canisterKick;
vmCvar_t	g_canisterKickOwner; 

vmCvar_t	g_smokeColor_Axis;
vmCvar_t	g_smokeColor_Allies;
//vmCvar_t	g_smokeColor_Neutral;
vmCvar_t	g_smokeMod_Axis;
vmCvar_t	g_smokeMod_Allies;
//vmCvar_t	g_smokeMod_Neutral;

//vmCvar_t	g_lean;
vmCvar_t	g_spectator;
vmCvar_t	g_realism;
vmCvar_t	g_weaponScriptsDir;
vmCvar_t	g_fastBackStab;
vmCvar_t	g_missileGravity;
vmCvar_t	g_hitsounds;
#ifdef MV_SUPPORT
vmCvar_t	g_multiview;
#endif

vmCvar_t	g_fixedphysics;
vmCvar_t	g_fixedphysicsfps;
vmCvar_t	g_customChat;
vmCvar_t	g_HQMessages;
vmCvar_t	g_defaultMute;
vmCvar_t	g_adrenaline;

// forty - in mod flood protection 
vmCvar_t	g_floodprotect; 
vmCvar_t	g_floodthreshold; 

cvarTable_t		gameCvarTable[] = 
{
	// don't override the cheat state set by the system
	{ &g_cheats,				"sv_cheats",				"1",				0,	qfalse },

	// noset vars
	{ NULL,						"gamename",					GAMEVERSION,		CVAR_SERVERINFO | CVAR_ROM, 	0, qfalse  },
	{ NULL,						"gamedate",					__DATE__,			CVAR_ROM, 						0, qfalse  },
	{ &g_restarted,				"g_restarted",				"0", 				CVAR_ROM, 						0, qfalse  },
	{ &g_reset,					"g_reset",					"0", 				CVAR_ROM, 						0, qfalse  },
	{ NULL,						"sv_mapname",				"",					CVAR_SERVERINFO | CVAR_ROM, 	0, qfalse  },

	// latched vars
	{ &g_gametype,				"g_gametype",				"4",				CVAR_SERVERINFO | CVAR_LATCH,	0, qfalse  },		// Arnout: default to GT_WOLF_CAMPAIGN

	// Class Specific Charge Times
	{ &g_redlimbotime,			"g_redlimbotime",			"30000", 			CVAR_SERVERINFO | CVAR_LATCH, 	0, qfalse },
	{ &g_bluelimbotime,			"g_bluelimbotime",			"30000", 			CVAR_SERVERINFO | CVAR_LATCH, 	0, qfalse },
	{ &g_medicChargeTime,		"g_medicChargeTime",		"45000", 			CVAR_LATCH, 					0, qfalse, qtrue },
	{ &g_engineerChargeTime,	"g_engineerChargeTime",		"30000", 			CVAR_LATCH, 					0, qfalse, qtrue },
	{ &g_LTChargeTime,			"g_LTChargeTime",			"40000", 			CVAR_LATCH, 					0, qfalse, qtrue },
	{ &g_soldierChargeTime,		"g_soldierChargeTime",		"20000", 			CVAR_LATCH, 					0, qfalse, qtrue },
	{ &g_covertopsChargeTime,	"g_covertopsChargeTime",	"30000", 			CVAR_LATCH, 					0, qfalse, qtrue },


	{ &g_landminetimeout,		"g_landminetimeout",		"1",				CVAR_ARCHIVE,					0, qfalse, qtrue },

	{ &g_maxclients,			"sv_maxclients",			"20",				CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },			// NERVE - SMF - made 20 from 8
	{ &g_maxGameClients,		"g_maxGameClients", 		"0",				CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_minGameClients,		"g_minGameClients", 		"8",				CVAR_SERVERINFO,				0, qfalse  },								// NERVE - SMF

	// change anytime vars
	{ &g_fraglimit, 			"fraglimit", 				"0", /*CVAR_SERVERINFO |*/ CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, 			"timelimit", 				"0",				CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

#ifdef ALLOW_GSYNC
	{ &g_synchronousClients,	"g_synchronousClients",		"0", 				CVAR_SYSTEMINFO | CVAR_CHEAT, 0, qfalse  },
#endif // ALLOW_GSYNC

	{ &g_friendlyFire,			"g_friendlyFire",			"1", 				CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue, qtrue },

	{ &g_teamForceBalance,		"g_teamForceBalance",		"0", 				CVAR_ARCHIVE  },							// NERVE - SMF - merge from team arena

	{ &g_warmup,				"g_warmup",					"60",				CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup,				"g_doWarmup",				"0",				CVAR_ARCHIVE, 0, qtrue  },

	// NERVE - SMF
	{ &g_warmupLatch,			"g_warmupLatch",			"1",				0, 0, qfalse },

	{ &g_nextTimeLimit,			"g_nextTimeLimit",			"0", 				CVAR_WOLFINFO, 0, qfalse  },
	{ &g_currentRound,			"g_currentRound",			"0", 				CVAR_WOLFINFO, 0, qfalse, qtrue },
	{ &g_altStopwatchMode,		"g_altStopwatchMode",		"0", 				CVAR_ARCHIVE, 0, qtrue, qtrue },
	{ &g_gamestate,				"gamestate",				"-1",				CVAR_WOLFINFO | CVAR_ROM, 0, qfalse  },

	{ &g_noTeamSwitching,		"g_noTeamSwitching",		"0",				CVAR_ARCHIVE, 0, qtrue  },


	{ &g_userTimeLimit,			"g_userTimeLimit",			"0", 				0, 0, qfalse, qtrue },
	{ &g_userAlliedRespawnTime, "g_userAlliedRespawnTime",	"0", 				0, 0, qfalse, qtrue },
	{ &g_userAxisRespawnTime,	"g_userAxisRespawnTime",	"0", 				0, 0, qfalse, qtrue },
	
	{ &g_swapteams,				"g_swapteams",				"0",				CVAR_ROM,		0,	qfalse, qtrue },
	// -NERVE - SMF

	{ &g_log,					"g_log",					"",					CVAR_ARCHIVE, 	0, 	qfalse },
	{ &g_logChat,				"g_logChat",				"",					CVAR_ARCHIVE, 	0, 	qfalse },
	{ &g_logSync,				"g_logSync",				"0",				CVAR_ARCHIVE, 	0, 	qfalse },
	{ &g_logCrash,				"g_logCrash",				"crash.log",		CVAR_ARCHIVE, 	0, 	qfalse }, // jaquboss turn it on

	{ &g_password,				"g_password",				"none", 			CVAR_USERINFO,	0, 	qfalse },
	{ &sv_privatepassword,		"sv_privatepassword",		"", 				CVAR_TEMP,		0, 	qfalse },
	{ &g_banIPs,				"g_banIPs",					"", 				CVAR_ARCHIVE,	0, 	qfalse },
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
	{ &g_filterBan,				"g_filterBan",				"1",				CVAR_ARCHIVE, 0, qfalse },

	{ &g_dedicated,				"dedicated",				"0",				0, 0, qfalse },

	{ &g_speed,					"g_speed",					"320",				0, 0, qtrue, qtrue },
	{ &g_gravity,				"g_gravity",				"800",				0, 0, qtrue, qtrue },
	{ &g_knockback, 			"g_knockback",				"1000",				0, 0, qtrue, qtrue },
	
	{ &g_needpass,				"g_needpass",				"0", 				CVAR_SERVERINFO | CVAR_ROM, 0, qtrue },
	{ &g_balancedteams,			"g_balancedteams",			"0", 				CVAR_SERVERINFO | CVAR_ROM, 0, qtrue },
	{ &g_forcerespawn,			"g_forcerespawn",			"0", 				0, 0, qtrue },
	{ &g_inactivity,			"g_inactivity",				"0", 				0, 0, qtrue },
	{ &g_debugMove,				"g_debugMove",				"0", 				0, 0, qfalse },
	{ &g_debugDamage,			"g_debugDamage",			"0", 				CVAR_CHEAT, 0, qfalse },
	{ &g_debugAlloc,			"g_debugAlloc",				"0",				0, 0, qfalse },
	{ &g_debugBullets,			"g_debugBullets",			"0",				0, 0, qfalse},
	{ &g_motd,					"g_motd",					"",					CVAR_ARCHIVE, 0, qfalse },

	{ &voteFlags,				"voteFlags",				"0", 				CVAR_TEMP|CVAR_ROM|CVAR_SERVERINFO, 0, qfalse },

	{ &g_complaintlimit,		"g_complaintlimit",			"6", 				CVAR_ARCHIVE, 0, qtrue },						// DHM - Nerve
	{ &g_ipcomplaintlimit,		"g_ipcomplaintlimit",		"3", 				CVAR_ARCHIVE, 0, qtrue },
	{ &g_filtercams,			"g_filtercams",				"0", 				CVAR_ARCHIVE, 0, qfalse },
	{ &g_maxlives,				"g_maxlives",				"0", 				CVAR_ARCHIVE|CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },		// DHM - Nerve
	{ &g_maxlivesRespawnPenalty,"g_maxlivesRespawnPenalty", "0", 				CVAR_ARCHIVE|CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },
	{ &g_voiceChatsAllowed,		"g_voiceChatsAllowed",		"4", 				CVAR_ARCHIVE, 0, qfalse },				// DHM - Nerve

	{ &g_alliedmaxlives,		"g_alliedmaxlives",			"0", 				CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },		// Xian
	{ &g_axismaxlives,			"g_axismaxlives",			"0", 				CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },			// Xian
	{ &g_fastres,				"g_fastres",				"0", 				CVAR_ARCHIVE, 0, qtrue, qtrue },							// Xian - Fast Medic Resing
	{ &g_knifeonly,				"g_knifeonly",				"0", 				0, 0, qtrue },											// Xian - Fast Medic Resing
	{ &g_enforcemaxlives,		"g_enforcemaxlives",		"1", 				CVAR_ARCHIVE, 0, qtrue },					// Xian - Gestapo enforce maxlives stuff by temp banning

	{ &g_developer,				"developer",				"0", 				CVAR_TEMP, 0, qfalse },
	{ &g_rankings,				"g_rankings",				"0", 				0, 0, qfalse },
	{ &g_userAim,				"g_userAim",				"1", 				CVAR_CHEAT, 0, qfalse },

	{ &pmove_fixed,				"pmove_fixed",				"0", 				CVAR_SYSTEMINFO, 0, qfalse },
	{ &pmove_msec,				"pmove_msec",				"8", 				CVAR_SYSTEMINFO, 0, qfalse },

	{ &g_scriptName,			"g_scriptName",				"",					CVAR_CHEAT, 0, qfalse },

	{ &g_antilag,				"g_antilag",				"1",				CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ NULL, 					"Players_Axis",				"", 				CVAR_ROM, 0, qfalse, qfalse },
	{ NULL, 					"Players_Allies",			"", 				CVAR_ROM, 0, qfalse, qfalse },
	//bani - #184
	{ NULL, 					"P",						"", 				CVAR_ROM, 0, qfalse, qfalse },

	{ NULL, 					"campaign_maps",			"", 				CVAR_ROM, 0, qfalse, qfalse },
	{ NULL, 					"C",						"", 				CVAR_ROM, 0, qfalse, qfalse },

	{ &refereePassword,			"refereePassword",			"none",				0, 0, qfalse},
	{ &g_spectatorInactivity,	"g_spectatorInactivity",	"0",				0, 0, qfalse, qfalse },
	{ &match_latejoin,			"match_latejoin",			"1",				0, 0, qfalse, qfalse },
	{ &match_minplayers,		"match_minplayers",			MATCH_MINPLAYERS,	0, 0, qfalse, qfalse },
	{ &match_mutespecs,			"match_mutespecs",			"0",				0, 0, qfalse, qtrue },
	{ &match_readypercent,		"match_readypercent", 		"100",				0, 0, qfalse, qtrue },
	{ &match_timeoutcount,		"match_timeoutcount", 		"3",				0, 0, qfalse, qtrue },
	{ &match_timeoutlength,		"match_timeoutlength",		"180",				0, 0, qfalse, qtrue },
	{ &match_warmupDamage,		"match_warmupDamage",		"1",				0, 0, qfalse },
	{ &server_autoconfig,		"server_autoconfig",		"0",				0, 0, qfalse, qfalse },
	{ &server_motd0,			"server_motd0", 			"^dNo Quarter",		0, 0, qfalse, qfalse },
	{ &server_motd1,			"server_motd1", 			"",					0, 0, qfalse, qfalse },
	{ &server_motd2,			"server_motd2", 			"",					0, 0, qfalse, qfalse },
	{ &server_motd3,			"server_motd3", 			"", 				0, 0, qfalse, qfalse },
	{ &server_motd4,			"server_motd4", 			"", 				0, 0, qfalse, qfalse },
	{ &server_motd5,			"server_motd5", 			"", 				0, 0, qfalse, qfalse },
	{ &team_maxPanzers, 		"team_maxPanzers",			"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxMG42s,			"team_maxMG42s",			"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxFlamers, 		"team_maxFlamers", 			"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxMortars, 		"team_maxMortars", 			"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxriflegrenades,	"team_maxriflegrenades",	"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxVenoms, 			"team_maxVenoms", 			"-1", 				0, 0, qfalse, qfalse }, // jet Pilot
	{ &team_maxShotguns, 		"team_maxShotguns", 		"-1", 				0, 0, qfalse, qfalse }, // jet Pilot
	

	{ &team_maxSoldiers, 		"team_maxSoldiers", 		"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxMedics,			"team_maxMedics",			"-1", 				0, 0, qfalse, qfalse },
	{ &team_maxEngineers, 		"team_maxEngineers", 		"-1", 				0, 0, qfalse, qfalse }, // jet Pilot
	{ &team_maxFieldops, 		"team_maxFieldops", 		"-1", 				0, 0, qfalse, qfalse }, // jet Pilot
	{ &team_maxCovertops, 		"team_maxCovertops", 		"-1", 				0, 0, qfalse, qfalse }, // jet Pilot

	{ &team_maxplayers,			"team_maxplayers",			"0", 				0, 0, qfalse, qfalse },
	{ &team_nocontrols,			"team_nocontrols",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_comp,			"vote_allow_comp",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_gametype,		"vote_allow_gametype",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_kick,			"vote_allow_kick",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_map,			"vote_allow_map",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_matchreset,	"vote_allow_matchreset",	"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_mutespecs,	"vote_allow_mutespecs",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_nextmap,		"vote_allow_nextmap",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_pub,			"vote_allow_pub",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_referee,		"vote_allow_referee",		"0", 				0, 0, qfalse, qfalse },
	{ &vote_allow_shuffleteamsxp,"vote_allow_shuffleteamsxp","1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_shuffleteamsxp_norestart,"vote_allow_shuffleteamsxp_norestart","1",0, 0, qfalse, qfalse },
	{ &vote_allow_swapteams,	"vote_allow_swapteams",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_friendlyfire,	"vote_allow_friendlyfire",	"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_timelimit,	"vote_allow_timelimit",		"0", 				0, 0, qfalse, qfalse },
	{ &vote_allow_warmupdamage,	"vote_allow_warmupdamage",	"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_antilag,		"vote_allow_antilag",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_balancedteams,"vote_allow_balancedteams", "1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_muting,		"vote_allow_muting",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_surrender,	"vote_allow_surrender",		"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_restartcampaign,	
								"vote_allow_restartcampaign",
															"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_nextcampaign,	"vote_allow_nextcampaign",	"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_poll,			"vote_allow_poll",			"1", 				0, 0, qfalse, qfalse },
	{ &vote_allow_maprestart,	"vote_allow_maprestart",	"1", 				0, 0, qfalse, qfalse },
	{ &vote_limit,				"vote_limit",				"5", 				0, 0, qfalse, qfalse },
	{ &vote_percent,			"vote_percent",				"50",				0, 0, qfalse, qfalse },

	// state vars
	{ &z_serverflags,			"z_serverflags",			"0", 				0, 0, qfalse, qfalse },

	{ &g_debugConstruct,		"g_debugConstruct",			"0", 				CVAR_CHEAT, 0, qfalse },

	{ &g_scriptDebug,			"g_scriptDebug",			"0", 				CVAR_CHEAT, 0, qfalse },

	// What level of detail do we want script printing to go to.
	{ &g_scriptDebugLevel,		"g_scriptDebugLevel",		"0", 				CVAR_CHEAT, 0, qfalse },

	// How fast do we want Allied single player movement?
	{ &g_movespeed,				"g_movespeed",				"76",				CVAR_CHEAT, 0, qfalse },

	// Arnout: LMS	
	{ &g_lms_teamForceBalance,	"g_lms_teamForceBalance",	"1",				CVAR_ARCHIVE },
	{ &g_lms_roundlimit,		"g_lms_roundlimit",			"3",				CVAR_ARCHIVE },
	{ &g_lms_matchlimit,		"g_lms_matchlimit",			"2",				CVAR_ARCHIVE },
	{ &g_lms_currentMatch,		"g_lms_currentMatch",		"0",				CVAR_ROM, 0, qfalse, qtrue },
	{ &g_lms_lockTeams,			"g_lms_lockTeams",			"0",				CVAR_ARCHIVE },
	{ &g_lms_followTeamOnly,	"g_lms_followTeamOnly",		"1",				CVAR_ARCHIVE },
	{ &g_axiswins,				"g_axiswins",				"0",				CVAR_ROM, 0, qfalse, qtrue },
	{ &g_alliedwins,			"g_alliedwins",				"0",				CVAR_ROM, 0, qfalse, qtrue },
	{ &g_axismapxp,				"g_axismapxp",				"0",				CVAR_ROM, 0, qfalse, qtrue },
	{ &g_alliedmapxp,			"g_alliedmapxp",			"0",				CVAR_ROM, 0, qfalse, qtrue },

	{ &g_oldCampaign,			"g_oldCampaign",			"",					CVAR_ROM, 0, },
	{ &g_currentCampaign,		"g_currentCampaign",		"",					CVAR_WOLFINFO | CVAR_ROM, 0, },
	{ &g_currentCampaignMap,	"g_currentCampaignMap",		"0",				CVAR_WOLFINFO | CVAR_ROM, 0, },
	
	// configured by the server admin, points to the web pages for the server
	{ &url,						"URL",						"",					CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },

	{ &g_letterbox,				"cg_letterbox", 			"0", 				CVAR_TEMP	},
	{ &bot_enable,				"bot_enable",				"0", 				0			},

	{ &g_debugSkills,			"g_debugSkills",			"0", 				0			},

	{ &g_heavyWeaponRestriction,"g_heavyWeaponRestriction", "100",				CVAR_ARCHIVE|CVAR_SERVERINFO },
	{ &g_autoFireteams,			"g_autoFireteams",			"1",				CVAR_ARCHIVE },

	{ &g_nextmap,				"nextmap",					"", 				CVAR_TEMP	},
	{ &g_nextcampaign,			"nextcampaign",				"", 				CVAR_TEMP	},

	{ &g_disableComplaints,		"g_disableComplaints",		"0",				CVAR_ARCHIVE },
	// tjw
	{ &g_shrubbot,				"g_shrubbot",				"shrubbot.cfg",						0,	0 },
	{ &g_playDead,				"g_playDead",				"0",				0 },
	{ &g_shove,					"g_shove",					"80",				0 },
	{ &g_dragCorpse,			"g_dragCorpse",				"1", 				0 },
	{ &g_classChange,			"g_classChange",			"0", 				0 },
	{ &g_forceLimboHealth,		"g_forceLimboHealth",		"75", 				0 }, // jaquboss - changed behaviour of this
	{ &g_privateMessages,		"g_privateMessages",		"1", 				0 },
	{ &g_XPSave,				"g_XPSave",					"0", 				0 },
    { &g_XPSaveFile,			"g_XPSaveFile",				"xpsave.cfg",		0 },
	{ &g_XPSaveDirectory,		"g_XPSaveDirectory",		"",					0 },
	{ &g_XPSaveMaxAge,			"g_XPSaveMaxAge",			"86400",			0 }, 
	{ &g_weapons,				"g_weapons",				"0", 				0 },
	{ &g_goomba,				"g_goomba",					"10", 				0 },
	{ &g_goombaXp,				"g_goombaXP",				"5", 				0 },
	{ &g_spawnInvul, 			"g_spawnInvul",				"3", 				0 },
	{ &g_teamChangeKills,		"g_teamChangeKills",		"1", 				0 },

	{ &g_logAdmin,				"g_logAdmin",				"",					0 },
	{ &team_maxLandmines,		"team_maxLandmines",		"10",				0 },
	{ &g_mapConfigs,			"g_mapConfigs",				"",					0 },
	{ &g_customConfig,			"g_customConfig",			"",					0 },
	{ &g_dropHealth,			"g_dropHealth", 			"0", 				0 },
	{ &g_dropAmmo,				"g_dropAmmo",				"0",				0 },
	{ &g_intermissionTime,		"g_intermissionTime",		"60",				0 },
	{ &g_intermissionReadyPercent,	"g_intermissionReadyPercent",	"100",		0 },

	{ &g_defaultMute,			"g_defaultMute",			"3600",				0 },									
//	{ &g_hitboxes,				"g_hitboxes",				"31",				0 },

	// cheat protected this because it will overflow gentities if enabled on a full server
	{ &g_debugHitboxes,			"g_debugHitboxes",			"0",				CVAR_CHEAT },
	{ &g_debugPlayerHitboxes,	"g_debugPlayerHitboxes",	"0",				0 },
	{ &g_voting,				"g_voting",					"0",				0 },
	{ &g_moverScale,			"g_moverScale",				"1.0",				0 },
	{ &g_ammoCabinetTime,		"g_ammoCabinetTime",		"60000", 			0 },
	{ &g_healthCabinetTime,		"g_healthCabinetTime",		"10000", 			0 },
	{ &g_maxWarp, 				"g_maxWarp",				"4",				0 },
	{ &g_dropObj, 				"g_dropObj",				"0",				0 },
	//{ &g_serverInfo, "g_serverInfo", "1", 0 },
	{ &g_goombaFlags,			"g_goombaFlags",			"25",				0 },
	//{ &g_shoveSound,			"g_shoveSound",				"sound/weapons/grenade/gren_throw.wav", 0 },
	{ &g_shoveNoZ,				"g_shoveNoZ",				"0",				0 },
	{ &g_tyranny,				"g_tyranny",				"1",				CVAR_SERVERINFO }, // jaquboss show it again to players
	{ &g_SBCheats,				"g_SBCheats",				"0",				CVAR_SERVERINFO }, // to disallow cheating for admins
	{ &g_mapScriptDirectory,	"g_mapScriptDirectory", 	"", 				0 },
	{ &g_campaignFile,			"g_campaignFile",			"", 				0 },
	{ &g_fear,					"g_fear",					"2000",				0 },
	{ &g_shortcuts,				"g_shortcuts",				"0",				0 },
	{ &g_XPDecay,				"g_XPDecay",				"0",				0 },  
	{ &g_XPDecayRate,			"g_XPDecayRate",			"0.0",				0 },
	{ &g_XPDecayFloor,			"g_XPDecayFloor",			"0",				0 }, 

	// Josh
	{ &g_logOptions,			"g_logOptions",				"0",				0 },
	{ &g_censor,				"g_censor",					"extreme",			0 },
	{ &g_censorNames,			"g_censorNames",			"",					0 },
	{ &g_censorPenalty,			"g_censorPenalty",			"1",				0 },
	{ &g_censorMuteTime,		"g_censorMuteTime", 		"0",				0 },
	{ &g_maxXP,					"g_maxXP",					"-1",				0 }, 

	// dvl
	{ &g_slashKill,				"g_slashKill",				"0",				0 },
	// Michael
	{ &g_skillSoldier,			"skill_soldier",			"20 50 90 140 200 270 350 440 540",		0 },	
	{ &g_skillMedic,			"skill_medic",				"20 50 90 140 200 270 350 440 540",		0 },
	{ &g_skillEngineer,			"skill_engineer",			"20 50 90 140 200 270 350 440 540",		0 },
	{ &g_skillFieldOps,			"skill_fieldops",			"20 50 90 140 200 270 350 440 540",		0 },
	{ &g_skillCovertOps,		"skill_covertops",			"20 50 90 140 200 270 350 440 540",		0 },
	{ &g_skillBattleSense,		"skill_battlesense",		"20 50 90 140 200 270 350 440 540",		0 },
	{ &g_skillLightWeapons, 	"skill_lightweapons",		"20 50 90 140 200 270 350 440 540",		0 },

// unlagged server options
	{ &sv_fps,					"sv_fps",					"20",				CVAR_SYSTEMINFO, 0, qfalse },
	{ &g_truePing,				"g_truePing",				"0",  				0},
	{ &g_skipCorrection,		"g_skipCorrection",			"1",  				0},
// unlagged server options
	
	// matt
	{ &g_teamDamageRestriction, "g_teamDamageRestriction",	"0",						0 },
	{ &g_minHits,				"g_minHits",				"6",						0 },
	{ &g_autoTempBan,			"g_autoTempBan",			"0",						0 },
	{ &g_autoTempBanTime,		"g_autoTempBanTime",		"1800",						0 },

// poisoned needle settings
//	{ &g_poison, 				"g_poison",					"10", 						0 },
//	{ &g_poisonSound,			"g_poisonSound",			"sound/player/gurp2.wav",	0 },
// end of poisoned needle sttings

	{ &g_medics, 				"g_medics",					"0", 						0 },
	{ &g_alliedSpawnInvul,		"g_alliedSpawnInvul",		"0", 						0 },
	{ &g_axisSpawnInvul,		"g_axisSpawnInvul",			"0", 						0 },
	{ &g_msgs,					"g_msgs",					"0", 						0 },
	{ &g_msgpos,				"g_msgpos",					"0", 						0 },

// forty - arty/airstrike rate limiting  
	{ &team_airstrikeTime,		"team_airstrikeTime",		"10",						0 },
	{ &team_artyTime,			"team_artyTime",			"10",						0 },

// forty - constructible xp sharing
	{ &g_constructiblexpsharing,"g_constructiblexpsharing", "0",						0 },

// forty - airstrike block  
	{ &g_asblock,				"g_asblock",				"0",						0 },  

// forty - canister kicking  
	{ &g_canisterKick,			"g_canisterKick",			"0",						0 },  
	{ &g_canisterKickOwner,		"g_canisterKickOwner",		"0",						0 }, 

	{ NULL,						"mod_version",				"1.1.0",					CVAR_SERVERINFO | CVAR_ROM },
	{ NULL,						"mod_url",					"http://shitstorm.org",		CVAR_SERVERINFO | CVAR_ROM },

	{ &g_doubleJump,			"g_doubleJump",				"0",						0 },	// jaquboss - defaulted to 0 because of realism and objective reasons
	{ &g_doubleJumpHeight,		"g_doubleJumpHeight",		"1.4",						0 },
	
	{ &g_shotgunPellets,		"g_shotgunPellets",			"12",						0 },

	{ &jp_maxLevelLightWeapons,	"g_maxLevelLightWeapons",	"10",						0 },
	{ &jp_maxLevelBattleSense,	"g_maxLevelBattleSense",	"10",						0 },
	{ &jp_maxLevelSoldier,		"g_maxLevelSoldier",		"10",						0 },
	{ &jp_maxLevelMedic,		"g_maxLevelMedic",			"10",						0 },
	{ &jp_maxLevelEngineer,		"g_maxLevelEngineer",		"10",						0 },
	{ &jp_maxLevelFieldOp,		"g_maxLevelFieldOp",		"10",						0 },
	{ &jp_maxLevelCovertOp,		"g_maxLevelCovertOp",		"10",						0 },

	//{ &g_maxLandminesRed,		"g_maxMinesAxis",			"10",						0 },
	//{ &g_maxLandminesBlue,		"g_maxMinesAllies",			"10",						0 },
	//{ &g_allowPersonalMines,	"g_allowPersonalMines",		"0",						0 },

	{ &jp_falloff,				"g_falloff",				"1",						0 },

// jaquboss

	{ &g_weaponItems,			"g_weaponItems",			"1",						0 },

	{ &jp_keepAwards,			"g_skills",					"0",						CVAR_SERVERINFO },
	{ &jp_insanity,				"jp_insanity",				"0",						CVAR_SERVERINFO },
	
	{ &g_weaponBounce,			"g_weaponBounce",			"0.25",						0 },
	{ &g_throwDistance,			"g_throwDistance",			"75",						0 },
	{ &g_realHead,				"g_realHead",				"1",						0 },
	{ &g_missileSpeed,			"g_missileSpeed",			"0",						0 },
	{ &g_missileHealth,			"g_missileHealth",			"5",						0 },
	{ &g_mortarBBox,			"g_mortarBBox",				"0",						0 },
	
	{ &g_grenadeBonus,			"g_grenadeBonus",			"3",						0 },
	{ &g_flushItems,			"g_flushItems",				"1",						0 },

	{ &g_bulletReflect,			"g_bulletReflect",			"1",						0 },
	{ &g_painAnims,				"g_painAnims",				"0",    					0 },
	{ &g_painChance, 			"g_painChance",				"25",						0 },
	
	{ &g_spreeSound[0],			"g_spreeSound0",			"sound/spree/killingspree.wav",			0 },
	{ &g_spreeSound[1],			"g_spreeSound1",			"sound/spree/rampage.wav",		0 },
	{ &g_spreeSound[2],			"g_spreeSound2",			"sound/spree/dominating.wav",		0 },
	{ &g_spreeSound[3],			"g_spreeSound3",			"sound/spree/unstoppable.wav",	0 },
	{ &g_spreeSound[4],			"g_spreeSound4",			"sound/spree/godlike.wav",		0 },
	{ &g_spreeSound[5],			"g_spreeSound5",			"sound/spree/wickedsick.wav", 		0 },
	{ &g_spreeSound[6],			"g_spreeSound6",			"sound/spree/potter.wav", 		0 },
	{ &g_spreeSound[7],			"g_spreeSound7",			"sound/spree/holyshit.wav", 		0 },
	{ &g_spreeSound[8],			"g_spreeSound8",			"sound/spree/holyshit.wav", 		0 },
	{ &g_spreeSound[9],			"g_spreeSound9",			"sound/spree/holyshit.wav", 		0 },

	{ &g_humiliationSound[0],	"g_humiliationSound0",		"sound/spree/humiliation.wav", 		0 },
	{ &g_humiliationSound[1],	"g_humiliationSound1",		"sound/spree/humiliation.wav", 		0 },
	{ &g_humiliationSound[2],	"g_humiliationSound2",		"sound/spree/humiliation.wav", 		0 },
	{ &g_humiliationSound[3],	"g_humiliationSound3",		"sound/spree/humiliation.wav", 		0 },
	{ &g_humiliationSound[4],	"g_humiliationSound4",		"sound/spree/humiliation.wav", 		0 },
	{ &g_humiliationSound[5],	"g_humiliationSound5",		"sound/spree/humiliation.wav", 		0 },

	{ &g_multikillSound[0],		"g_multikillSound0",		"sound/spree/multikill.wav",		0 },
	{ &g_multikillSound[1],		"g_multikillSound1",		"sound/spree/ultrakill.wav",		0 },
	{ &g_multikillSound[2],		"g_multikillSound2",		"sound/spree/monsterkill.wav",		0 },
	{ &g_multikillSound[3],		"g_multikillSound3",		"sound/spree/megakill.wav",			0 },
	{ &g_multikillSound[4],		"g_multikillSound4",		"sound/spree/ludicrouskill.wav",	0 },

	{ &g_announcer,				"g_announcer",				"7",			0 },

	{ &g_multikillTime,			"g_multikillTime",			"1000",			0 },


	
	{ &g_spreeMessage[0],		"g_spreeMessage0",			"^dis on a ^2Killing Spree^d!",			0 },
	{ &g_spreeMessage[1],		"g_spreeMessage1",			"^dis on a ^2Rampage^d!",				0 },
	{ &g_spreeMessage[2],		"g_spreeMessage2",			"^dis ^2Dominating^d!",					0 },
	{ &g_spreeMessage[3],		"g_spreeMessage3",			"^drevels in his ^2Bloodbath^d!",		0 },
	{ &g_spreeMessage[4],		"g_spreeMessage4",			"^dis a walking ^2Slaughterhouse^d!",	0 },
	{ &g_spreeMessage[5],		"g_spreeMessage5",			"^dwreaks ^2Havoc ^dupon his foes!", 	0 },
	{ &g_spreeMessage[6],		"g_spreeMessage6",			"^dcuts through enemies like a ^2God ^2of ^2War^d!", 		0 },
	{ &g_spreeMessage[7],		"g_spreeMessage7",			"^dis the ^2Prophet of Doom^d!", 		0 },
	{ &g_spreeMessage[8],		"g_spreeMessage8",			"^dunleashes the ^>Shitstorm^d!", 		0 },
	{ &g_spreeMessage[9],		"g_spreeMessage9",			"^dis still unleashing the ^>Shitstorm^d!", 		0 },

	{ &g_humiliationMessage[0],	"g_humiliationMessage0",	"^dmust be having a bad day!", 		0 },
	{ &g_humiliationMessage[1],	"g_humiliationMessage1",	"^dis getting his ass kicked!", 		0 },
	{ &g_humiliationMessage[2],	"g_humiliationMessage2",	"^dis a death magnet!", 				0 },
	{ &g_humiliationMessage[3],	"g_humiliationMessage3",	"^dneeds remedial combat training!", 	0 },
	{ &g_humiliationMessage[4],	"g_humiliationMessage4",	"^dseems well-aquainted with Death!", 	0 },
	{ &g_humiliationMessage[5],	"g_humiliationMessage5",	"^dstill can't kill shit!", 	0 },
	{ &g_headshot,				"g_headshot",				"0",								0 },
#ifdef MV_SUPPORT
	{ &g_multiview,				"g_multiview",				"0",								0 },
#endif
	//Meyer - changed default colors to NQ style
	{ &g_smokeColor_Axis,		"g_smokeColor_Axis",		"0.6 0.1 0.1 0.0",					0 },
	{ &g_smokeColor_Allies,		"g_smokeColor_Allies",		"0.1 0.1 0.6 0.0",					0 },
//	{ &g_smokeColor_Neutral,	"g_smokeColor_Neutral",		"0.5 0.5 0.5 0.0",					0 },
	{ &g_smokeMod_Axis,			"g_smokeMod_Axis",			"0.1 0.0 0.0 0.5",					0 },
	{ &g_smokeMod_Allies,		"g_smokeMod_Allies",		"0.0 0.0 0.1 0.5",					0 },
//	{ &g_smokeMod_Neutral,		"g_smokeMod_Neutral",		"0.25 0.25 0.25 0.5",				0 },
//	{ &g_lean,					"g_lean",					"1",								0 },
	{ &g_spectator,				"g_spectator",				"0",								0 },
	{ &g_realism,				"g_realism",				"0",								0 }, // jaquboss, better prone, no ladder weapon..
	{ &g_weaponScriptsDir,		"g_weaponScriptsDir",		"",									0 }, // jaquboss, better prone, no ladder weapon..

	{ &g_fastBackStab,			"g_fastBackStab",			"0",								0 }, // jaquboss - insta backstabs ( if they kill )
	{ &g_missileGravity,		"g_missileGravity",			"0",								0 }, // jaquboss - gravited rockets
	
	{ &g_hitsounds,				"g_hitsounds",				"1",								0 }, // jaquboss - allow hitsounds
	
	{ &g_fixedphysics,			"g_fixedphysics",			"0",								CVAR_ARCHIVE|CVAR_SERVERINFO }, 
	{ &g_fixedphysicsfps,		"g_fixedphysicsfps",		"125",								CVAR_ARCHIVE|CVAR_SERVERINFO }, 

	{ &g_customChat,			"g_customChat",				"1",								0 }, // jaquboss
	{ &g_HQMessages,			"g_HQMessages",				"3",								0 }, // jaquboss - 1 - we need a <class>, 2 - we've lost most of our men 	
	{ &g_adrenaline,			"g_adrenaline",				"0",								0 },
#ifdef OMNIBOTS
	// Omni-bot user defined path to load bot library from.
	{ &g_OmniBotPath, "omnibot_path", "", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_OmniBotEnable, "omnibot_enable", "0", CVAR_SERVERINFO_NOUPDATE | CVAR_NORESTART, 0, qfalse },
#endif
	{ &g_noAttacklInvul,		"g_noAttackInvul",			"0",								0 },
	{ NULL,						"ip_max_clients",			"3",								0 }, // jaquboss - register this cvar
	{ &g_floodprotect,			"g_floodprotect",			"1",								0 }, 
	{ &g_floodthreshold,		"g_floodthreshold",			"6",								0 }, 

};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );

void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );
// Josh:
void InitCensorStructure( void );
void InitCensorNamesStructure( void );
// Michael
void InitSkillLevelStructure( skillType_t );

qboolean G_SnapshotCallback( int entityNum, int clientNum ) 
{
	gentity_t* ent = &g_entities[ entityNum ];

	if( ent->s.eType == ET_MISSILE ) 
	{
		if( ent->s.weapon == WP_LANDMINE ) 
		{
			return G_LandmineSnapshotCallback( entityNum, clientNum );
		}
	}

	return qtrue;
}

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/


#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 ) {
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif
	switch ( command ) 
	{
		case GAME_INIT:
#ifndef DEBUG
			EnableStackTrace(); // Jaquboss - from ETPub to help debugging
#endif
			G_InitGame( arg0, arg1, arg2 );
#ifdef OMNIBOTS
			if (!Bot_Interface_Init())
				G_Printf(S_COLOR_RED "Unable to Initialize Omni-Bot.\n");
#endif
			return 0;
		case GAME_SHUTDOWN:
			G_ShutdownGame( arg0 );
#ifndef DEBUG // so we can use debuggers
			DisableStackTrace();// Jaquboss - from ETPub to help debugging
#endif 
#ifdef OMNIBOTS
			if (!Bot_Interface_Shutdown())
				G_Printf(S_COLOR_RED "Error shutting down Omni-Bot.\n");
#endif
			return 0;
		case GAME_CLIENT_CONNECT:
			return (int)ClientConnect( arg0, arg1, arg2 );
		case GAME_CLIENT_THINK:
			ClientThink( arg0 );
			return 0;
		case GAME_CLIENT_USERINFO_CHANGED:
			ClientUserinfoChanged( arg0 );
			return 0;
		case GAME_CLIENT_DISCONNECT:
			ClientDisconnect( arg0 );
			return 0;
		case GAME_CLIENT_BEGIN:
			ClientBegin( arg0 );
			return 0;
		case GAME_CLIENT_COMMAND:
			ClientCommand( arg0 );
			return 0;
		case GAME_RUN_FRAME:
			G_RunFrame( arg0 );
#ifdef OMNIBOTS
			Bot_Interface_Update();
#endif
			return 0;
		case GAME_CONSOLE_COMMAND:
 			return ConsoleCommand();
		case BOTAI_START_FRAME:
#ifdef NO_BOT_SUPPORT
			return 0;
#else
			return BotAIStartFrame( arg0 );
#endif // NO_BOT_SUPPORT
		case BOT_VISIBLEFROMPOS:
#ifdef NO_BOT_SUPPORT
			return qfalse;
#else
			return BotVisibleFromPos( (float *)arg0, arg1, (float *)arg2, arg3, arg4 );
#endif // NO_BOT_SUPPORT
		case BOT_CHECKATTACKATPOS:
#ifdef NO_BOT_SUPPORT
			return qfalse;
#else
			return BotCheckAttackAtPos( arg0, arg1, (float *)arg2, arg3, arg4 );
#endif // NO_BOT_SUPPORT
		case GAME_SNAPSHOT_CALLBACK:
			return G_SnapshotCallback( arg0, arg1 );
		case GAME_MESSAGERECEIVED:
			return -1;
	}

	return -1;
}

void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}
//bani
void QDECL G_Printf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_DPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	if (!g_developer.integer)
		return;

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}
//bani
void QDECL G_DPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}
//bani
void QDECL G_Error( const char *fmt, ... )_attribute((format(printf,1,2)));

/*
==============
G_CursorHintIgnoreEnt: returns whether the ent should be ignored 
for cursor hint purpose (because the ent may have the designed content type
but nevertheless should not display any cursor hint)
==============
*/
static qboolean G_CursorHintIgnoreEnt(gentity_t *traceEnt, gentity_t *clientEnt) {
	return (traceEnt->s.eType == ET_OID_TRIGGER || traceEnt->s.eType == ET_TRIGGER_MULTIPLE) ? qtrue : qfalse;
}

/*
==============
G_CheckForCursorHints
	non-AI's check for cursor hint contacts

	server-side because there's info we want to show that the client
	just doesn't know about.  (health or other info of an explosive,invisible_users,items,etc.)

	traceEnt is the ent hit by the trace, checkEnt is the ent that is being
	checked against (in case the traceent was an invisible_user or something)
	
==============
*/

qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other ) {
	if( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if( !other->client ) {
		return qfalse;
	}

	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( other->client->ps.pm_flags & PMF_DUCKED ) {
		return qfalse;
	}

	if( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if( ent->r.currentOrigin[2] - other->r.currentOrigin[2] >= 40 ) {
		return qfalse;
	}
	
	if( ent->r.currentOrigin[2] - other->r.currentOrigin[2] < 0 ) {
		return qfalse;
	}

	if( ent->s.frame != 0 ) {
		return qfalse;
	}

	if( ent->active ) {
		return qfalse;
	}

	if( other->client->ps.grenadeTimeLeft ) {
		return qfalse;
	}

	if( infront( ent, other ) ) {
		return qfalse;
	}

	return qtrue;
}

qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other ) {
	if( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if( !other->client ) {
		return qfalse;
	}

	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if( ent->s.frame == 0 ) {
		return qfalse;
	}

	return qtrue;
}

void G_CheckForCursorHints( gentity_t *ent ) 
{
	vec3_t			forward, right, up;
	vec3_t			offset, end;
	trace_t			*tr;
	float			dist;
	float			chMaxDist			 = CH_MAX_DIST;
	gentity_t		*checkEnt, *traceEnt = 0;
  	playerState_t	*ps;
	int				hintType, hintDist;
	int				hintVal;
	static int hintValMax = 255; // CHRUKER: b080 - Breakable damage indicator can wrap when the entity has a lot of health
	qboolean		zooming, indirectHit;	// indirectHit means the checkent was not the ent hit by the trace (checkEnt!=traceEnt)
	int				trace_contents;			// DHM - Nerve
	int				numOfIgnoredEnts	 = 0;

	if( !ent->client )
	{
		return;
	}

	ps = &ent->client->ps;

	indirectHit = qfalse;

	zooming = (qboolean)(ps->eFlags & EF_ZOOMING);

	AngleVectors (ps->viewangles, forward, right, up);

	VectorCopy( ps->origin, offset );
	if(ps->viewangles[PITCH] > 30 && (ps->eFlags & EF_CROUCHING))
		offset[2] += 30;
	else
		offset[2] += ps->viewheight;

	// lean
	if( ps->leanf ) {
		VectorMA( offset, ps->leanf, right, offset );
	}

	if( zooming ) {
		VectorMA( offset, CH_MAX_DIST_ZOOM, forward, end );
	} else {
		VectorMA( offset, chMaxDist, forward, end );
	}

	tr = &ps->serverCursorHintTrace;

	trace_contents = (CONTENTS_TRIGGER|CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE);
	trap_Trace( tr, offset, NULL, NULL, end, ps->clientNum, trace_contents );

	// reset all
	hintType	= ps->serverCursorHint		= HINT_NONE;
	hintVal		= ps->serverCursorHintVal	= 0;
	
	dist = VectorDistanceSquared( offset, tr->endpos );

	if( zooming ) {
		hintDist	= CH_MAX_DIST_ZOOM;
	} else {
		hintDist	= chMaxDist;
	}

	// Arnout: building something - add this here because we don't have anything solid to trace to - quite ugly-ish
	if( ent->client->touchingTOI && ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		gentity_t* constructible;
		if ((constructible = G_IsConstructible( ent->client->sess.sessionTeam, ent->client->touchingTOI ))) {
			ps->serverCursorHint = HINT_CONSTRUCTIBLE;
			ps->serverCursorHintVal = (int)constructible->s.angles2[0];
			return;
		}
	}

	if( ps->stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		if(ent->client->landmineSpottedTime && level.time - ent->client->landmineSpottedTime < 500) {
			ps->serverCursorHint = HINT_LANDMINE;
			ps->serverCursorHintVal	= ent->client->landmineSpotted ? ent->client->landmineSpotted->count2 : 0;
			return;
		}
	}

	if( tr->fraction == 1 ) {
		return;
	}

	traceEnt = &g_entities[tr->entityNum];
	while( G_CursorHintIgnoreEnt(traceEnt, ent) && numOfIgnoredEnts < 10 ) {
		// xkan, 1/9/2003 - we may hit multiple invalid ents at the same point
		// count them to prevent too many loops
		numOfIgnoredEnts++;

		// xkan, 1/8/2003 - advance offset (start point) past the entity to ignore
		VectorMA( tr->endpos, 0.1, forward, offset );

		trap_Trace( tr, offset, NULL, NULL, end, traceEnt->s.number, trace_contents );

		// xkan, 1/8/2003 - (hintDist - dist) is the actual distance in the above
		// trap_Trace call. update dist accordingly.
		dist += VectorDistanceSquared( offset, tr->endpos );
		if(tr->fraction == 1) {
			return;
		}
		traceEnt = &g_entities[tr->entityNum];
	}

	if( tr->entityNum == ENTITYNUM_WORLD ) {
		if ((tr->contents & CONTENTS_WATER) && !(ps->powerups[PW_BREATHER])) {
			hintDist = CH_WATER_DIST;
			hintType = HINT_WATER;
		} else if( (tr->surfaceFlags & SURF_LADDER) && !(ps->pm_flags & PMF_LADDER) ) { // ladder
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
		}
	} else if( tr->entityNum < MAX_CLIENTS ) {
		// Show medics a syringe if they can revive someone

		if ( traceEnt->client && traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
			if(	traceEnt->client->ps.pm_type == PM_DEAD && 
				!(traceEnt->client->ps.pm_flags & PMF_LIMBO) && 
				!(traceEnt->client->ps.eFlags & EF_PLAYDEAD)) {
				if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_MEDIC && traceEnt->reviveVal == 0 ){
					hintDist	= CH_REVIVE_DIST; 
					hintType	= HINT_REVIVE;
				} else if ( ent->client->sess.skill[SK_FIRST_AID] >= 9 ) {
					hintType	= HINT_REVIVE_ACTIVATE;
					hintVal		= traceEnt->reviveVal;
					hintDist	= CH_ACTIVATE_DIST; 

					if( hintVal > 255 ) { 
						hintVal = 255;
					} 
				}
			}
		} else if(traceEnt->client && traceEnt->client->isCivilian) {
			// xkan, 1/6/2003 - check for civilian, show neutral cursor (no matter which team)		
			hintType = HINT_PLYR_NEUTRAL;
			hintDist = CH_FRIENDLY_DIST;	// far, since this will be used to determine whether to shoot bullet weaps or not
		}
	} else {
		checkEnt = traceEnt;

		// Arnout: invisible entities don't show hints
		if( traceEnt->entstate == STATE_INVISIBLE || traceEnt->entstate == STATE_UNDERCONSTRUCTION ) {
			return;
		}

		// check invisible_users first since you don't want to draw a hint based
		// on that ent, but rather on what they are targeting.
		// so find the target and set checkEnt to that to show the proper hint.
		if( traceEnt->s.eType == ET_GENERAL ) {

			// ignore trigger_aidoor.  can't just not trace for triggers, since I need invisible_users...
			// damn, I would like to ignore some of these triggers though.

			if( !Q_stricmp(traceEnt->classname, "trigger_aidoor") ) {
				return;
			}

			if(!Q_stricmp(traceEnt->classname, "func_invisible_user")) {
				indirectHit = qtrue;

				// DHM - Nerve :: Put this back in only in multiplayer
				if(traceEnt->s.dmgFlags) {	// hint icon specified in entity
					hintType = traceEnt->s.dmgFlags;
					hintDist = CH_ACTIVATE_DIST;
					checkEnt = 0;
				} else { // use target for hint icon
					checkEnt = G_FindByTargetname( NULL, traceEnt->target);
					if(!checkEnt) {		// no target found
						hintType = HINT_BAD_USER;
						hintDist = CH_MAX_DIST_ZOOM;	// show this one from super far for debugging
					}
				}
			}
		}


		if(checkEnt) {

			// TDF This entire function could be the poster boy for converting to OO programming!!!
			// I'm making this into a switch in a vain attempt to make this readable so I can find which
			// brackets don't match!!!
			//
			// tjw: OO programming won't help those who refuse to use 
			// procedures and insist on stretching lines and tab depth
			// into oblivion.  It amazes me how people think that using 
			// objects are going to magically enable them write readable 
			// code.

			switch (checkEnt->s.eType)
			{
				case ET_CORPSE:
					if( !ent->client->ps.powerups[PW_BLUEFLAG] && !ent->client->ps.powerups[PW_REDFLAG] && !ent->client->ps.powerups[PW_OPS_DISGUISED]) 
					{
						if( /*BODY_TEAM(traceEnt) < 4*/ traceEnt->s.time2 == 0 && BODY_TEAM(traceEnt) != ent->client->sess.sessionTeam && traceEnt->nextthink == traceEnt->timestamp + BODY_TIME(BODY_TEAM(traceEnt))) 
						{
							if( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) 
							{
								hintDist	= 48;
								hintType	= HINT_UNIFORM;
								hintVal		= BODY_VALUE(traceEnt);
								if( hintVal > 255 )
								{
									hintVal = 255;
								}
								break;
							}
						}
					}
					// class stealing
					if(!g_classChange.integer) break;
					if( traceEnt->s.time2 == 0  && BODY_TEAM(traceEnt) == ent->client->sess.sessionTeam  && ent->client->sess.playerType != BODY_CLASS(traceEnt)) { // jaquboss, no more for same class
						hintDist	= 48;
						hintType	= HINT_UNIFORM;
						hintVal		= BODY_VALUE(traceEnt);
						if( hintVal > 255 ) {
							hintVal = 255;
						}
					}
					break;
				case ET_GENERAL:
				case ET_MG42_BARREL:
				case ET_AAGUN:
					hintType = HINT_FORCENONE;

					if( G_EmplacedGunIsMountable( traceEnt, ent) ) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_MG42;	
						hintVal = 0;
					} else {
						if( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER && G_EmplacedGunIsRepairable( traceEnt, ent)) {
							hintType = HINT_BUILD;
							hintDist = CH_BREAKABLE_DIST;
							hintVal = traceEnt->health;
							if( hintVal > 255 ) {
								hintVal = 255;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;
						}
					}
					break;
				case ET_EXPLOSIVE:
				{
					if( checkEnt->spawnflags & EXPLOSIVE_TANK ) {
						hintDist = CH_BREAKABLE_DIST * 2;
						hintType = HINT_TANK;
						hintVal	 = ps->serverCursorHintVal	= 0;	// no health for tank destructibles
					} else {
						switch( checkEnt->constructibleStats.weaponclass ) {
						case 0:
							hintDist = CH_BREAKABLE_DIST;
							hintType = HINT_BREAKABLE;
							hintVal  = checkEnt->health;		// also send health to client for visualization
							
							// CHRUKER: b080 - Breakable damage indicator can wrap when the entity has a lot of health
							if ( hintVal > hintValMax ) hintValMax = hintVal;
							hintVal = (hintVal * 255) / hintValMax;

							break;
						case 1:
							hintDist = CH_BREAKABLE_DIST * 2;
							hintType = HINT_SATCHELCHARGE;
							hintVal	 = ps->serverCursorHintVal = 0;	// no health for satchel charges
							break;
						case 2:
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;

							if( checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER ) {
								if( ( (ent->client->sess.sessionTeam == TEAM_AXIS) && (checkEnt->parent->spawnflags & ALLIED_OBJECTIVE) ) ||
									( (ent->client->sess.sessionTeam == TEAM_ALLIES) && (checkEnt->parent->spawnflags & AXIS_OBJECTIVE) ) ) {
									hintDist = CH_BREAKABLE_DIST * 2;
									hintType = HINT_BREAKABLE_DYNAMITE;
									hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite
								}
							}
							break;
						default:
							if( checkEnt->health > 0 ) {
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization

								// CHRUKER: b080 - Breakable damage indicator can wrap when the entity has a lot of health
								if ( hintVal > hintValMax ) hintValMax = hintVal;
								hintVal = (hintVal * 255) / hintValMax;

							} else {
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
							}
							break;
						}
					}

					break;
				}
				case ET_CONSTRUCTIBLE:					
					if( G_ConstructionIsPartlyBuilt( checkEnt ) && !(checkEnt->spawnflags & CONSTRUCTIBLE_INVULNERABLE) ) {
						// only show hint for players who can blow it up
						if( checkEnt->s.teamNum != ent->client->sess.sessionTeam ) {
							switch( checkEnt->constructibleStats.weaponclass ) {
							case 0:
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization

								// CHRUKER: b080 - Breakable damage indicator can wrap when the entity has a lot of health
								if ( hintVal > hintValMax ) hintValMax = hintVal;
								hintVal = (hintVal * 255) / hintValMax;

								break;
							case 1:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_SATCHELCHARGE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for satchel charges
								break;
							case 2:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite
								break;
							default:
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
								break;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint		= HINT_FORCENONE;
							hintVal	 = ps->serverCursorHintVal	= 0;
							return;
						}
					}

					break;
				case ET_ALARMBOX:
					if(checkEnt->health > 0) {
						hintType = HINT_ACTIVATE;
					}
					break;

				case ET_ITEM: 
				{
					gitem_t* it = &bg_itemlist[checkEnt->item - bg_itemlist];

					hintDist = CH_ACTIVATE_DIST;

					switch(it->giType) {
						case IT_HEALTH:
							//if ( !(jp_insanity.integer & JP_INSANITY_VAMPIRE )) // jaquboss
							hintType = HINT_HEALTH;
							break;
						case IT_TREASURE:
							hintType = HINT_TREASURE;
							break;
						case IT_POWERUP:
							if ( it->giTag == PW_OPS_DISGUISED) {
								if ( it->giAmmoIndex != ent->client->sess.sessionTeam && ent->client->sess.playerType == PC_COVERTOPS && !ent->client->ps.powerups[PW_BLUEFLAG] && !ent->client->ps.powerups[PW_REDFLAG] && !ent->client->ps.powerups[PW_OPS_DISGUISED] )
									hintType = HINT_UNIFORM;
							} else { 
								hintType = HINT_INVENTORY;		// jaquboss - fixme
							}
							break;
						case IT_WEAPON: {
								qboolean canPickup = COM_BitCheck( ent->client->ps.weapons, it->giTag );

								if( it->giTag == WP_AMMO ) {
									hintType = HINT_AMMO;
									break;
								}

								if( it->giTag == WP_MEDKIT ) { // be sure
									hintType = HINT_HEALTH;
									break;
								}

								if( !canPickup ) {
									canPickup = G_CanPickupWeapon( it->giTag, ent );
								}

								if( canPickup ) {
									hintType = HINT_WEAPON;
									hintVal	 = ps->serverCursorHintVal	= it->giTag; // jaquboss	
									}
								break;
							}
						case IT_AMMO:	
							hintType = HINT_AMMO;
							hintVal	 = ps->serverCursorHintVal	= it->giTag; // jaquboss	
							break;
						case IT_ARMOR:	
						//	hintType = HINT_ARMOR;
							break;
						case IT_HOLDABLE:
							hintType = HINT_HOLDABLE;
							break;
						case IT_KEY:
							hintType = HINT_INVENTORY;
							break;
						case IT_TEAM:
							if ( !Q_stricmp( traceEnt->classname, "team_CTF_redflag" ) && ent->client->sess.sessionTeam == TEAM_ALLIES )
								hintType = HINT_POWERUP;
							else if ( !Q_stricmp( traceEnt->classname, "team_CTF_blueflag" ) && ent->client->sess.sessionTeam == TEAM_AXIS )
								hintType = HINT_POWERUP;
							break;
						case IT_BAD:
						default:
							break;
					}

					break;
				}
				case ET_MOVER:
					if(!Q_stricmp( checkEnt->classname, "script_mover" ) ) {
						if( G_TankIsMountable( checkEnt, ent ) ) {
							hintDist = CH_ACTIVATE_DIST;
							hintType = HINT_ACTIVATE;
						}
					} else if( !Q_stricmp( checkEnt->classname, "func_door_rotating" ) ) {
						if( checkEnt->moverState == MOVER_POS1ROTATE ) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR_ROTATING;
							if( checkEnt->key == -1 || !G_AllowTeamsAllowed( checkEnt, ent ) ) { // locked
								hintType = HINT_DOOR_ROTATING_LOCKED;
							}
						}
					} else if(!Q_stricmp(checkEnt->classname, "func_door")) {
						if(checkEnt->moverState == MOVER_POS1) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR;

							if( checkEnt->key == -1 || !G_AllowTeamsAllowed( checkEnt, ent ) ) { // locked
								hintType = HINT_DOOR_LOCKED;
							}
						}
					} else if(!Q_stricmp(checkEnt->classname, "func_button")) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_BUTTON;
					} else if(!Q_stricmp(checkEnt->classname, "props_flamebarrel")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;
					} else if(!Q_stricmp(checkEnt->classname, "props_statue")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;
					}
					
					break;
				case ET_MISSILE:
				case ET_BOMB:
					if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) 
					{
						hintDist	= CH_BREAKABLE_DIST;
						hintType	= HINT_DISARM;
						hintVal		= checkEnt->health;		// also send health to client for visualization
						if ( hintVal > 255 )
							hintVal = 255;
					}
					

					// hint icon specified in entity (and proper contact was made, so hintType was set)
					// first try the checkent...
					if( checkEnt->s.dmgFlags && hintType ) {
						hintType = checkEnt->s.dmgFlags;
					}

					// then the traceent
					if( traceEnt->s.dmgFlags && hintType ) {
						hintType = traceEnt->s.dmgFlags;
					}

					break;
				default:
					break;
			}

			if(zooming) {
				hintDist = CH_MAX_DIST_ZOOM;

				// zooming can eat a lot of potential hints
				switch(hintType) {

					// allow while zooming
					case HINT_PLAYER:
					case HINT_TREASURE:
					case HINT_LADDER:
					case HINT_EXIT:
					case HINT_NOEXIT:
					case HINT_PLYR_FRIEND:
					case HINT_PLYR_NEUTRAL:
					case HINT_PLYR_ENEMY:
					case HINT_PLYR_UNKNOWN:
						break;

					default:
						return;
				}
			}
		}
	} 

	if( dist <= Square( hintDist )) {
		ps->serverCursorHint = hintType;
		ps->serverCursorHintVal = hintVal;
	}

}

void G_SetTargetName( gentity_t* ent, char* targetname ) {
	if( targetname && *targetname ) {
		ent->targetname = targetname;
		ent->targetnamehash = BG_StringHashValue(targetname);
	} else {
		ent->targetnamehash = -1;
	}
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ){
		if (!e->inuse)
			continue;
		
		if (!e->team)
			continue;

		if (e->flags & FL_TEAMSLAVE)
			continue;
		
		if (!Q_stricmp (e->classname, "func_tramcar"))
		{
			if (e->spawnflags & 8) // leader
			{
				e->teammaster = e;
			}
			else 
			{
				continue;
			}
		}
		
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
//				e2->key = e->key;	// (SA) I can't set the key here since the master door hasn't finished spawning yet and therefore has a key of -1
				e2->flags |= FL_TEAMSLAVE;

				if (!Q_stricmp (e2->classname, "func_tramcar"))
				{
					trap_UnlinkEntity (e2);
				}
		
				// make sure that targets only point at the master
				if ( e2->targetname ) {
					G_SetTargetName( e, e2->targetname );
					
					// Rafael
					// note to self: added this because of problems
					// pertaining to keys and double doors
					if (Q_stricmp (e2->classname, "func_door_rotating"))
						e2->targetname = NULL;
				}
			}
		}
	}

		G_Printf ("%i teams with %i entities\n", c, c2);
}


/*
==============
G_RemapTeamShaders
==============
*/
void G_RemapTeamShaders() {
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
	int i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	level.server_settings = 0;

	trap_Cvar_Register(&g_serverInfo, "g_serverInfo", "1", CVAR_ROM);

	for (i=0, cv=gameCvarTable; i<gameCvarTableSize; i++, cv++) {

		if((g_serverInfo.integer & SIF_CHARGE) &&
			(!Q_stricmp(cv->cvarName, "g_medicChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_LTChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_engineerChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_soldierChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_covertopsChargeTime"))) {

			cv->cvarFlags |= CVAR_SERVERINFO;

		}

#if DEBUG
			G_Printf("Registring cvar '%s' default value '%s' \n", cv->cvarName, cv->defaultString ? cv->defaultString : "^1NULL^7");
#endif

		trap_Cvar_Register(cv->vmCvar,
			cv->cvarName,
			cv->defaultString,
			cv->cvarFlags);
		if(cv->vmCvar) {
			cv->modificationCount = cv->vmCvar->modificationCount;
			// OSP - Update vote info for clients, if necessary
			if(!G_IsSinglePlayerGame()) {
				G_checkServerToggle(cv->vmCvar);
			}
		}

		remapped = (remapped || cv->teamShader);
	}

	if(g_serverInfo.integer & SIF_PLAYERS) {
		trap_Cvar_Register(NULL, "Players_Axis", "",
			CVAR_SERVERINFO_NOUPDATE);
		// tjw: if we're going to overflow the server info string
		//      it's best to do it now instead of when players 
		//      connect to fill up this cvar
		//      MAX_CLIENTS is 64 so this could be up to 181
		trap_Cvar_Set("Players_Axis",
			"..............................."
			"..............................."
			"..............................."
			"..............................."
			"..............................."
			"....................."
			);
		trap_Cvar_Register(NULL, "Players_Allies", "",
			CVAR_SERVERINFO_NOUPDATE);
		trap_Cvar_Set("Players_Allies", "(none)");
	}

	if(g_serverInfo.integer & SIF_P) {
		trap_Cvar_Register(NULL, "P", "", CVAR_SERVERINFO_NOUPDATE);
		trap_Cvar_Set("P",
			"..............................."
			"...............................");
	}

	if(g_gametype.integer == GT_WOLF_CAMPAIGN &&
		(g_serverInfo.integer & SIF_CAMPAIGN_MAPS)) {

		trap_Cvar_Register(NULL, "campaign_maps", "",
			CVAR_SERVERINFO_NOUPDATE);
		// tjw: campaign_maps is populated with '.' to it's 
		//      longest possible value in g_utils.c as the
		//      campaign files are parsed.
	}
	
	if(g_gametype.integer == GT_WOLF_CAMPAIGN &&
			(g_serverInfo.integer & SIF_C) ) {
		trap_Cvar_Register(NULL, "C", "", CVAR_SERVERINFO_NOUPDATE);
		trap_Cvar_Set("C", ".....");
	}

	if(remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	// DHM - Gametype is currently restricted to supported types only
	if((g_gametype.integer < GT_WOLF || g_gametype.integer >= GT_MAX_GAME_TYPE)) {
		G_Printf( "g_gametype %i is out of range, defaulting to GT_WOLF(%i)\n", g_gametype.integer, GT_WOLF );
		trap_Cvar_Set( "g_gametype", va("%i",GT_WOLF) );
		trap_Cvar_Update( &g_gametype );
	}

	// OSP
	if(!G_IsSinglePlayerGame()) {
		trap_SetConfigstring(CS_SERVERTOGGLES, va("%d", level.server_settings));
		if(match_readypercent.integer < 1) trap_Cvar_Set("match_readypercent", "1");
	}

	if(pmove_msec.integer < 8) {
		trap_Cvar_Set("pmove_msec", "8");
	} else if(pmove_msec.integer > 33) {
		trap_Cvar_Set("pmove_msec", "33");
	}

}

static qboolean G_IsVoteFlagCvar(cvarTable_t *cv)
{
	if(cv->vmCvar == &vote_allow_comp ||
		cv->vmCvar == &vote_allow_gametype ||
		cv->vmCvar == &vote_allow_kick ||
		cv->vmCvar == &vote_allow_map ||
		cv->vmCvar == &vote_allow_matchreset ||
		cv->vmCvar == &vote_allow_mutespecs ||
		cv->vmCvar == &vote_allow_nextmap ||
		cv->vmCvar == &vote_allow_pub ||
		cv->vmCvar == &vote_allow_referee ||
		cv->vmCvar == &vote_allow_shuffleteamsxp ||
		cv->vmCvar == &vote_allow_shuffleteamsxp_norestart ||
		cv->vmCvar == &vote_allow_swapteams ||
		cv->vmCvar == &vote_allow_friendlyfire ||
		cv->vmCvar == &vote_allow_timelimit ||
		cv->vmCvar == &vote_allow_warmupdamage ||
		cv->vmCvar == &vote_allow_antilag ||
		cv->vmCvar == &vote_allow_balancedteams ||
		cv->vmCvar == &vote_allow_muting ||
		cv->vmCvar == &vote_allow_surrender ||
		cv->vmCvar == &vote_allow_restartcampaign ||
		cv->vmCvar == &vote_allow_nextcampaign ||
		cv->vmCvar == &vote_allow_poll ||
		cv->vmCvar == &vote_allow_maprestart) {

		return qtrue;
	}
	return qfalse;
}

const char szModeNames[JP_INSANITY_NUM_MODES][3][128] =
{
	{"Instant Spawn",		"^2On",			"^1Off"		},
	{"Unlimited Ammo",		"^2On",			"^1Off"		},	
	{"Mortar Camera",		"^2On",			"^1Off"		},
	{"Missile Camera",		"^2On",			"^1Off"		},
	{"Unlimited Charge",	"^2On",			"^1Off"		},
	{"Guided Rockets",		"^2On",			"^1Off"		},
	{"Homing Rockets",		"^2On",			"^1Off"		},
	{"Weapon Pickups",		"^2Unlocked",	"^1Locked"	},
	{"Venom Gun",			"^2Unlocked",	"^1Locked"	},
	{"Fireballs",			"^2Unlocked",	"^1Locked"	},
//	{"Regeneration",		"^2On",			"^1Off"		},
//	{"Health Pickups",		"^2Disabled",	"^1Normal"	},
//	{"Vampire Mode",		"^2On",			"^1Off"		},
};

void jP_InsanityCheck(void)
{
	static int		lastValue		= 0;

	int				numchanges		= 0;
	int				i				= 0;

	qboolean		help			= qfalse;
	qboolean		changed[JP_INSANITY_NUM_MODES];
	char			message[128];
	int msgpos						= g_msgpos.integer;

	if (!Q_stricmp(jp_insanity.string, "help") || !Q_stricmp(jp_insanity.string, "list") )
	{
		trap_Cvar_Set( "jp_insanity", va("%d", lastValue));
		help = qtrue;
	}

	// check for no change, and early out if the same
	if (jp_insanity.integer == lastValue && !help)
		return;

	for (i = 0; i < JP_INSANITY_NUM_MODES; i++ )
	{
		changed[i] = qfalse;

		if ((jp_insanity.integer & (1 << i)) != (lastValue & (1 << i)))
		{			
			numchanges++;
			changed[i] = qtrue;
		}
	}

	lastValue = jp_insanity.integer;

    for (i = 0; i < JP_INSANITY_NUM_MODES; i++)
	{
		if (changed[i])
		{						
			sprintf(message, "^dRules Change: ^7%s: %s", szModeNames[i][0], szModeNames[i][lastValue & (1 << i) ? 1 : 2]);

			// if there's more than one change queued for the center banner, put on left instead
			if (numchanges > 1 && msgpos == MSGPOS_CENTER) 
				msgpos = MSGPOS_LEFT_BANNER;

			switch(msgpos) 
			{
				case MSGPOS_CENTER:
					AP(va("cp \"%s\"",		message));
					break;
				case MSGPOS_LEFT_BANNER:
					AP(va("cpm \"%s\"",		message));
					break;
				case MSGPOS_CHAT:
				default:
					AP(va("chat \"%s\"",	message));		
			}
		}
	}
}


qboolean G_IsNQinfoCvar(vmCvar_t *c)  
	{  
	return (  
				c == &g_doubleJump ||  
				c == &g_doubleJumpHeight 
#ifdef MV_SUPPORT
				|| c == &g_multiview
#endif
				// jet Pilot - server controlled smoke color for meyer
				|| c == &g_smokeColor_Axis
				|| c == &g_smokeColor_Allies
//				|| c == &g_smokeColor_Neutral
				|| c == &g_smokeMod_Axis
				|| c == &g_smokeMod_Allies
//				|| c == &g_smokeMod_Neutral
				// jaquboss
				//|| c == &g_hitboxes
				|| c == &team_maxPanzers
				|| c == &team_maxMG42s
				|| c == &team_maxFlamers
				|| c == &team_maxMortars
				|| c == &team_maxriflegrenades
				|| c == &team_maxVenoms
				|| c == &team_maxShotguns
//				|| c == &g_lean
				|| c == &g_spectator
				|| c == &g_realism
				|| c == &g_adrenaline
				|| c == &team_maxSoldiers
				|| c == &team_maxMedics
				|| c == &team_maxEngineers
				|| c == &team_maxFieldops
				|| c == &team_maxCovertops
//				|| c == &g_weaponScriptsDir
     		);	                  
}  
	  
void G_UpdateNQinfo(void)  
	{  
	char cs[MAX_INFO_STRING];  
	int	i;

	cs[0] = '\0';  
 
	Info_SetValueForKey(cs, /*"g_doubleJump"*/"DJ",   // jaquboss - save some place..
		va("%i", g_doubleJump.integer));  
	Info_SetValueForKey(cs, /*"g_doubleJumpHeight"*/"DJH",  
		va("%f", g_doubleJumpHeight.value));   // note floats needs too much place maybe code it to use only few numbers , %f looks like 1.0000000 should be 1.00 or 1.0
#ifdef MV_SUPPORT
	Info_SetValueForKey(cs, /*"g_multiview"*/ "MV",  
		va("%i", g_multiview.integer));  
#endif
	Info_SetValueForKey(cs, "RS",
		va("%s", g_smokeColor_Axis.string));
	Info_SetValueForKey(cs, "BS",
		va("%s", g_smokeColor_Allies.string));
//	Info_SetValueForKey(cs, "NS",
//		va("%s", g_smokeColor_Neutral.string));
	Info_SetValueForKey(cs, "RM",
		va("%s", g_smokeMod_Axis.string));
	Info_SetValueForKey(cs, "BM",
		va("%s", g_smokeMod_Allies.string));
//	Info_SetValueForKey(cs, "NM",
//		va("%s", g_smokeMod_Neutral.string));
// jaquboss predict hitboxes clientside
/*	Info_SetValueForKey(cs, "HB",  
		va("%i", g_hitboxes.integer));*/
// weapon limits	
	Info_SetValueForKey(cs, "TMP",  
		va("%i", team_maxPanzers.integer));  
	Info_SetValueForKey(cs, "TMMG",  
		va("%i", team_maxMG42s.integer));  
	Info_SetValueForKey(cs, "TMF",  
		va("%i", team_maxFlamers.integer));  
	Info_SetValueForKey(cs, "TMM",  
		va("%i", team_maxMortars.integer)); 
	Info_SetValueForKey(cs, "TMGL",  
		va("%i", team_maxriflegrenades.integer)); 
	Info_SetValueForKey(cs, "TMV",  
		va("%i", team_maxVenoms.integer)); 
	Info_SetValueForKey(cs, "TMSG",  
		va("%i", team_maxShotguns.integer)); 
/*	Info_SetValueForKey(cs, "L",  
		va("%i", g_lean.integer)); */
	Info_SetValueForKey(cs, "S",  
		va("%i", g_spectator.integer));
	Info_SetValueForKey(cs, "R",  
		va("%i", g_realism.integer));
	Info_SetValueForKey(cs, "A",  
		va("%i", g_adrenaline.integer));
	// Jaybird
	Info_SetValueForKey(cs, "JPKA",
		va("%i", jp_keepAwards.integer));

// jaquboss - cos it is pro
	Info_SetValueForKey(cs, "MS",
		va("%i", team_maxSoldiers.integer));
	Info_SetValueForKey(cs, "MM",
		va("%i", team_maxMedics.integer));
	Info_SetValueForKey(cs, "ME",
		va("%i", team_maxEngineers.integer));
	Info_SetValueForKey(cs, "MF",
		va("%i", team_maxFieldops.integer));
	Info_SetValueForKey(cs, "MC",
		va("%i", team_maxCovertops.integer));

	Info_SetValueForKey(cs, "NW",  
		va("%i", level.numWeaponScriptsDir));
	for ( i = 0; i < level.numWeaponScriptsDir; i++ ){
		Info_SetValueForKey(cs, va("W%i", i),  level.weaponScriptsDir[i]);
	}


	trap_SetConfigstring(CS_NOQUARTERINFO, cs);  
}  


/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
	int i;
	cvarTable_t	*cv;
	qboolean fToggles = qfalse;
	qboolean fVoteFlags = qfalse;
	qboolean remapped = qfalse;
	qboolean chargetimechanged = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) 
	{
		if ( cv->vmCvar )
		{
			trap_Cvar_Update( cv->vmCvar );

			if(cv->modificationCount != cv->vmCvar->modificationCount)
			{
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange && !(cv->cvarFlags & CVAR_LATCH) ) {
					trap_SendServerCommand( -1, va("print \"Server:[lof] %s [lon]changed to[lof] %s\n\"", cv->cvarName, cv->vmCvar->string ) );
				}

				if (cv->teamShader) {
					remapped = qtrue;
				}

				if (cv->vmCvar == &jp_insanity ) 
				{
					jP_InsanityCheck();
				}

				if( cv->vmCvar == &g_filtercams ) {
					trap_SetConfigstring( CS_FILTERCAMS, va( "%i", g_filtercams.integer ) );
				}

				if( cv->vmCvar == &g_soldierChargeTime )
				{
					level.soldierChargeTime[0] = g_soldierChargeTime.integer * level.soldierChargeTimeModifier[0];
					level.soldierChargeTime[1] = g_soldierChargeTime.integer * level.soldierChargeTimeModifier[1];
					chargetimechanged = qtrue;
				} 
				else if( cv->vmCvar == &g_medicChargeTime )
				{
					level.medicChargeTime[0] = g_medicChargeTime.integer * level.medicChargeTimeModifier[0];
					level.medicChargeTime[1] = g_medicChargeTime.integer * level.medicChargeTimeModifier[1];
					chargetimechanged = qtrue;
				}
				else if( cv->vmCvar == &g_engineerChargeTime ) 
				{
					level.engineerChargeTime[0] = g_engineerChargeTime.integer * level.engineerChargeTimeModifier[0];
					level.engineerChargeTime[1] = g_engineerChargeTime.integer * level.engineerChargeTimeModifier[1];
					chargetimechanged = qtrue;
				} 
				else if( cv->vmCvar == &g_LTChargeTime )
				{
					level.lieutenantChargeTime[0] = g_LTChargeTime.integer * level.lieutenantChargeTimeModifier[0];
					level.lieutenantChargeTime[1] = g_LTChargeTime.integer * level.lieutenantChargeTimeModifier[1];
					chargetimechanged = qtrue;
				}
				else if( cv->vmCvar == &g_covertopsChargeTime )
				{
					level.covertopsChargeTime[0] = g_covertopsChargeTime.integer * level.covertopsChargeTimeModifier[0];
					level.covertopsChargeTime[1] = g_covertopsChargeTime.integer * level.covertopsChargeTimeModifier[1];
					chargetimechanged = qtrue;
				}
				else if(cv->vmCvar == &match_readypercent)
				{
					if(match_readypercent.integer < 1) trap_Cvar_Set(cv->cvarName, "1");
					else if(match_readypercent.integer > 100) trap_Cvar_Set(cv->cvarName, "100");
				}
				else if(cv->vmCvar == &g_warmup && (g_gamestate.integer != GS_PLAYING) && !G_IsSinglePlayerGame() )
				{
						level.warmupTime = level.time + (((g_warmup.integer < 10) ? 11 : g_warmup.integer + 1) * 1000);
						trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));				
				}
				// Moved this check out of the main world think loop
				else if(cv->vmCvar == &g_gametype) 
				{
					int worldspawnflags = g_entities[ENTITYNUM_WORLD].spawnflags;
					int gt, gametype;
					char buffer[32];

					trap_Cvar_LatchedVariableStringBuffer( "g_gametype", buffer, sizeof( buffer ) );
					gametype = atoi( buffer );


					if( gametype == GT_WOLF_CAMPAIGN && gametype != g_gametype.integer )
					{
						if( !G_MapIsValidCampaignStartMap() )
						{
							gt = g_gametype.integer;
							if( gt != GT_WOLF_LMS )
							{
								if( !(worldspawnflags & NO_GT_WOLF) )
								{
									gt = GT_WOLF;	// Default wolf
								}
								else 
								{
									gt = GT_WOLF_LMS;	// Last man standing
								}
							}

							G_Printf( "Map '%s' isn't a valid campaign start map, resetting game type to '%i'\n", level.rawmapname, gt );
							trap_Cvar_Set( "g_gametype", va( "%i", gt ) );
						}
						continue;
					}

					if(!level.latchGametype && g_gamestate.integer == GS_PLAYING && 
					  ( ( ( g_gametype.integer == GT_WOLF || g_gametype.integer == GT_WOLF_CAMPAIGN ) && (worldspawnflags & NO_GT_WOLF)) ||	
					  (g_gametype.integer == GT_WOLF_STOPWATCH && (worldspawnflags & NO_STOPWATCH)) ||
					  (g_gametype.integer == GT_WOLF_LMS && (worldspawnflags & NO_LMS)) )
					  ) 
					{					
						if( !(worldspawnflags & NO_GT_WOLF) )
						{
							gt = GT_WOLF;	// Default wolf
						}
						else 
						{
							gt = GT_WOLF_LMS;	// Last man standing
						}

						level.latchGametype = qtrue;
						AP("print \"Invalid gametype was specified, Restarting\n\"");
						trap_SendConsoleCommand( EXEC_APPEND, va("wait 2 ; g_gametype %i ; map_restart 10 0\n", gt ) );
					}
				} 
				else if(cv->vmCvar == &pmove_msec) 
				{
					if(pmove_msec.integer < 8)
					{
						trap_Cvar_Set(cv->cvarName, "8");
					} 
					else if(pmove_msec.integer > 33)
					{
						trap_Cvar_Set(cv->cvarName, "33");
					}
				} else if(G_IsNQinfoCvar(cv->vmCvar)) {  
					G_UpdateNQinfo();  
				}  

				// OSP - Update vote info for clients, if necessary
				else if(!G_IsSinglePlayerGame())
				{
					if(G_IsVoteFlagCvar(cv))
					{
						fVoteFlags = qtrue;
					}
					else 
					{
						fToggles = (G_checkServerToggle(cv->vmCvar) || fToggles);
					}
				}
			}
		}
	}

	if(fVoteFlags)
	{
		G_voteFlags();
	}

	if(fToggles)
	{
		trap_SetConfigstring(CS_SERVERTOGGLES, va("%d", level.server_settings));
	}

	if (remapped)
	{
		G_RemapTeamShaders();
	}

	if( chargetimechanged ) 
	{
		char cs[MAX_INFO_STRING];
		cs[0] = '\0';
		Info_SetValueForKey( cs, "axs_sld", va("%i", level.soldierChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_sld", va("%i", level.soldierChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_mdc", va("%i", level.medicChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_mdc", va("%i", level.medicChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_eng", va("%i", level.engineerChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_eng", va("%i", level.engineerChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_lnt", va("%i", level.lieutenantChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_lnt", va("%i", level.lieutenantChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_cvo", va("%i", level.covertopsChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_cvo", va("%i", level.covertopsChargeTime[1]) );
		trap_SetConfigstring( CS_CHARGETIMES, cs );
	}
}

// Reset particular server variables back to defaults if a config is voted in.
void G_wipeCvars(void)
{
	int			i;
	cvarTable_t	*pCvars;

	for(i=0, pCvars=gameCvarTable; i<gameCvarTableSize; i++, pCvars++) {
		if(pCvars->vmCvar && pCvars->fConfigReset) {
			G_Printf("set %s %s\n", pCvars->cvarName, pCvars->defaultString);
			trap_Cvar_Set(pCvars->cvarName, pCvars->defaultString);
		}
	}

	G_UpdateCvars();
}

//bani - #113
#define SNIPSIZE 250

//copies max num chars from beginning of dest into src and returns pointer to new src
char *strcut( char *dest, char *src, int num ) {
	int i;

	if( !dest || !src || !num )
		return NULL;
	for( i = 0 ; i < num ; i++ ) {
		if( (char)*src ) {
			*dest = *src;
			dest++;
			src++;
		} else { 
			break;
		}
	}
	*dest = (char)0;
	return src;
}

//g_{axies,allies}mapxp overflows and crashes the server
void bani_clearmapxp( void ) {
	trap_SetConfigstring( CS_AXIS_MAPS_XP, "" );
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, "" );

	trap_Cvar_Set( va( "%s_axismapxp0", GAMEVERSION ), "" );
	trap_Cvar_Set( va( "%s_alliedmapxp0", GAMEVERSION ), "" );
}

void bani_storemapxp( void ) {
	char cs[MAX_STRING_CHARS];
	char u[MAX_STRING_CHARS];
	char *k;
	int i, j;

	//axis
	trap_GetConfigstring( CS_AXIS_MAPS_XP, cs, sizeof(cs) );
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		Q_strcat( cs, sizeof( cs ), va( " %i", (int)level.teamXP[ i ][ 0 ] ) );
	}
	trap_SetConfigstring( CS_AXIS_MAPS_XP, cs );

	j = 0;
	k = strcut( u, cs, SNIPSIZE );
	while( strlen( u ) ) {
		//"to be continued..."
		if( strlen( u ) == SNIPSIZE ) {
			strcat( u, "+" );
		}
		trap_Cvar_Set( va( "%s_axismapxp%i", GAMEVERSION, j ), u );
		j++;
		k = strcut( u, k, SNIPSIZE );
	}

	//allies
	trap_GetConfigstring( CS_ALLIED_MAPS_XP, cs, sizeof(cs) );
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		Q_strcat( cs, sizeof( cs ), va( " %i", (int)level.teamXP[ i ][ 1 ] ) );
	}
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, cs );

	j = 0;
	k = strcut( u, cs, SNIPSIZE );
	while( strlen( u ) ) {
		//"to be continued..."
		if( strlen( u ) == SNIPSIZE ) {
			strcat( u, "+" );
		}
		trap_Cvar_Set( va( "%s_alliedmapxp%i", GAMEVERSION, j ), u );
		j++;
		k = strcut( u, k, SNIPSIZE );
	}
}

void bani_getmapxp( void ) {
	int j;
	char s[MAX_STRING_CHARS];
	char t[MAX_STRING_CHARS];

	j = 0;
	trap_Cvar_VariableStringBuffer( va( "%s_axismapxp%i", GAMEVERSION, j ), s, sizeof(s) );
	//reassemble string...
	while( strrchr( s, '+' ) ) {
		j++;
		*strrchr( s, '+' ) = (char)0;
		trap_Cvar_VariableStringBuffer( va( "%s_axismapxp%i", GAMEVERSION, j ), t, sizeof(t) );
		strcat( s, t );
	}
	trap_SetConfigstring( CS_AXIS_MAPS_XP, s );

	j = 0;
	trap_Cvar_VariableStringBuffer( va( "%s_alliedmapxp%i", GAMEVERSION, j ), s, sizeof(s) );
	//reassemble string...
	while( strrchr( s, '+' ) ) {
		j++;
		*strrchr( s, '+' ) = (char)0;
		trap_Cvar_VariableStringBuffer( va( "%s_alliedmapxp%i", GAMEVERSION, j ), t, sizeof(t) );
		strcat( s, t );
	}
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, s );
}

/*
=============
G_ConfigParse

=============
*/

static qboolean BG_PCF_ParseError( int handle, char *format, ... )
{
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );

	Com_Printf( S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string );

	trap_PC_FreeSource( handle );

	return qfalse;
}

static qboolean G_ConfigParse( int handle, config_t	*config )
{
	pc_token_t	token;
	char		text[256];	
	char		value[256];	

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) )
		return BG_PCF_ParseError( handle, "expected '{'" );

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) )
			break;

		if( token.string[0] == '}' )
			break;

		// simple set that does not need to save
		if( !Q_stricmp( token.string, "set" ) ) {
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text) ) ) {
				return BG_PCF_ParseError( handle, "expected cvar to set" );
			} else {
				if( !PC_String_ParseNoAlloc( handle, value, sizeof(value) ) )
					return BG_PCF_ParseError( handle, "expected cvar value" );

				if ( value[0] == '-' ){
					if( !PC_String_ParseNoAlloc( handle, text , sizeof(text)))
						return BG_PCF_ParseError( handle, "expected value after '-'" );
		
					Q_strncpyz(value, va("-%s", text ), sizeof(value));
				}

				trap_Cvar_Set( text, value );
			}
		// commands do not need to save as well
		} else if( !Q_stricmp( token.string, "command" ) ) {
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text)))
				return BG_PCF_ParseError( handle, "expected command to execute" );
			else
				trap_SendConsoleCommand(EXEC_APPEND, va("%s\n",text)); 
		// setlock - need to save us to whine
		} else if( !Q_stricmp( token.string, "setl" ) ) {
			if( !PC_String_ParseNoAlloc( handle, config->setl[config->numSetl].name , sizeof(config->setl[0].name)))
				return BG_PCF_ParseError( handle, "expected name of cvar to set and lock" );

			if( !PC_String_ParseNoAlloc( handle, config->setl[config->numSetl].value , sizeof(config->setl[0].value)))
				return BG_PCF_ParseError( handle, "expected value of cvar to set and lock" );

				if ( config->setl[config->numSetl].value[0] == '-' ){
					if( !PC_String_ParseNoAlloc( handle, text , sizeof(text)))
						return BG_PCF_ParseError( handle, "expected value after '-'" );
		
					Q_strncpyz(config->setl[config->numSetl].value, va("-%s", text ), sizeof(config->setl[0].value));
				}

				trap_Cvar_Set( config->setl[config->numSetl].name, config->setl[config->numSetl].value );
				config->numSetl++;
		} else {
			return BG_PCF_ParseError( handle, "unknown token '%s'", token.string );
		}
	}

	return qtrue;
}

qboolean G_LoadConfig( char forceFilename[MAX_QPATH], qboolean	init )
{
	char			filename[MAX_QPATH];
	int				handle = 0;
	config_t		*config;
	pc_token_t		token;
	
	if ( forceFilename[0] )
	{
		Q_strncpyz(filename, forceFilename, sizeof(filename));
	}
	else if(g_customConfig.string[0])
	{
		Q_strncpyz(filename, g_customConfig.string, sizeof(filename));
		// need to reload config?
		if ( !level.config.loaded )
			init = qtrue;
	} 
	else if(g_mapConfigs.string[0])
	{
		Q_strncpyz(filename, g_mapConfigs.string, sizeof(filename));
		Q_strcat( filename, sizeof(filename), "/");
		Q_strcat( filename, sizeof(filename), level.rawmapname );
		Q_strcat( filename, sizeof(filename), ".config" );
		init = qfalse;
	} else {
		return qfalse;
	}

	if ( init )
		memset( &level.config, 0, sizeof( config_t )); 

	handle = trap_PC_LoadSource( filename );

	if( !handle ){
		G_Printf( "^3Warning: No config with filename '%s' found\n", filename );
		return qfalse;
	}

	config = &level.config;

	while( 1 ) 
	{
		if( !trap_PC_ReadToken( handle, &token ) ) 
			break;

		if( !Q_stricmp( token.string, "configname" ) ) {
			if( !PC_String_ParseNoAlloc( handle, config->name, sizeof(config->name) ) ) 
				return BG_PCF_ParseError( handle, "expected config name" );

			trap_SetConfigstring( CS_CONFIGNAME, config->name );
		} else if( !Q_stricmp( token.string, "version" ) ) {
			if( !PC_String_ParseNoAlloc( handle, config->version, sizeof(config->name) ) ) 
				return BG_PCF_ParseError( handle, "expected config version" );
		} else if ( !Q_stricmp( token.string, "init" ) ) {
			if( !G_ConfigParse( handle, config ) ) {
				return BG_PCF_ParseError( handle, "failed to load init struct" );
			} else {
				if ( level.config.version[0] && level.config.name[0] )
					trap_SendServerCommand( -1, va("cp \"Script '%s^7' version '%s'^7 loaded\n\"", level.config.name, level.config.version) );
				else if ( level.config.name[0] ) 
					trap_SendServerCommand( -1, va("cp \"Script '%s^7' loaded\n\"", level.config.name ) );
			}
		} else if ( !Q_stricmp( token.string, "default" )){
			if( !G_ConfigParse( handle, config ) ) {
				return BG_PCF_ParseError( handle, "failed to load default struct");
			} 
		} else if ( !Q_stricmp( token.string, level.rawmapname )){
			if( !G_ConfigParse( handle, config ) ) {
				return BG_PCF_ParseError( handle, "failed to load %s struct", level.rawmapname );
			} 
		}
	}
	level.config.loaded = qtrue;

	trap_PC_FreeSource( handle );
	return qtrue;
}

/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int					i;
	char				cs[MAX_INFO_STRING];
	char mapConfig[MAX_STRING_CHARS];

	G_Printf ("------- Game Initialization -------\n");
	G_Printf ("gamename: %s\n", GAMEVERSION);
	G_Printf ("gamedate: %s\n", __DATE__);

	srand( randomSeed );

	//bani - make sure pak2.pk3 gets referenced on server so pure checks pass
	trap_FS_FOpenFile( "pak2.dat", &i, FS_READ );
	trap_FS_FCloseFile( i );

	G_RegisterCvars();

	// Xian enforcemaxlives stuff	
	/*
	we need to clear the list even if enforce maxlives is not active
	in case the g_maxlives was changed, and a map_restart happened
	*/
	ClearMaxLivesBans();
	
	// just for verbosity
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_enforcemaxlives.integer &&
			( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) { 
			G_Printf ( "EnforceMaxLives-Cleared GUID List\n" );
		}
	}

	G_ProcessIPBans();

	G_InitMemory();

	// NERVE - SMF - intialize gamestate
	if ( g_gamestate.integer == GS_INITIALIZE ) {
		// OSP
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP ) );
	}

	// set some level globals
	i = level.server_settings;
	{
		qboolean oldspawning = level.spawning;
		voteInfo_t votedata;
		forceCvar_t	forceCvars[MAX_FORCECVARS];
		int			forceCvarCount;
		svCvar_t	svCvars[MAX_FORCECVARS];
		int			svCvarCount;
		config_t	configBackup;

		// backup forcecvars
		for ( i=0;i<level.forceCvarCount; i++ ){
			memcpy( &forceCvars[i], &level.forceCvars[i], sizeof( forceCvar_t ) );
		}
		forceCvarCount = level.forceCvarCount; 

		// backup config
		memcpy ( &configBackup, &level.config, sizeof( config_t ) );

		// backup sv_cvars
		for ( i=0;i<level.svCvarsCount; i++ ){
			memcpy( &svCvars[i], &level.svCvars[i], sizeof( svCvar_t ) );
		}
		svCvarCount = level.forceCvarCount; 

		memcpy( &votedata, &level.voteInfo, sizeof( voteInfo_t ) );

		memset( &level, 0, sizeof( level ) );

		memcpy( &level.voteInfo, &votedata, sizeof( voteInfo_t ) );


		// restore forcecvars
		for ( i=0;i<forceCvarCount; i++ ){
			memcpy( &level.forceCvars[i], &forceCvars[i], sizeof( forceCvar_t ) );
		}
		level.forceCvarCount = forceCvarCount; 

		// restore sv_cvars
		for ( i=0;i<svCvarCount; i++ ){
			memcpy( &level.svCvars[i], &svCvars[i], sizeof( svCvar_t ) );
		}
		level.forceCvarCount = svCvarCount; 

		// restore config
		memcpy ( &level.config, &configBackup, sizeof( config_t ) );


		level.spawning = oldspawning;
	}

	// get all weapon script dirs up to 16
	for ( i = 0; i < 16 ; i++ ) {
		if ( i == 0 ){
			Q_strncpyz( level.weaponScriptsDir[i], g_weaponScriptsDir.string, sizeof(g_weaponScriptsDir.string) );
		} else {
			trap_Cvar_VariableStringBuffer(va("g_weaponScriptsDir%i",level.numWeaponScriptsDir), 
				level.weaponScriptsDir[i], sizeof(level.weaponScriptsDir[i]));
		}
		if(!Q_stricmp(level.weaponScriptsDir[i],""))
			break;
		else
			level.numWeaponScriptsDir++;
	}

	G_UpdateNQinfo();


	// jaquboss
	for( i = /*WP_KNIFE*/ WP_NONE; i < WP_NUM_WEAPONS; i++ ) {
		// DHM - Nerve :: Only register weapons we use in WolfMP
		if ( BG_WeaponInWolfMP(i) || i == WP_NONE /*|| i == WP_MAPMORTAR */)
			BG_RegisterWeapon( i, qtrue );
	}	


	level.time = levelTime;
	level.startTime = levelTime;
	level.server_settings = i;

	G_UpdateForceCvars();
	G_UpdateSvCvars();

	for( i =0; i < level.numConnectedClients; i++ ) {
		level.clients[ level.sortedClients[ i ] ].sess.spawnObjectiveIndex = 0;
	}

	// RF, init the anim scripting
	level.animScriptData.soundIndex = G_SoundIndex;
	level.animScriptData.playSound = G_AnimScriptSound;

	level.warmupModificationCount = g_warmup.modificationCount;

	level.soldierChargeTime[0] = level.soldierChargeTime[1] = g_soldierChargeTime.integer;
	level.medicChargeTime[0] = level.medicChargeTime[1] = g_medicChargeTime.integer;
	level.engineerChargeTime[0] = level.engineerChargeTime[1] = g_engineerChargeTime.integer;
	level.lieutenantChargeTime[0] = level.lieutenantChargeTime[1] = g_LTChargeTime.integer;

	level.covertopsChargeTime[0] = level.covertopsChargeTime[1] = g_covertopsChargeTime.integer;

	level.soldierChargeTimeModifier[0] = level.soldierChargeTimeModifier[1] = 1.f;
	level.medicChargeTimeModifier[0] = level.medicChargeTimeModifier[1] = 1.f;
	level.engineerChargeTimeModifier[0] = level.engineerChargeTimeModifier[1] = 1.f;
	level.lieutenantChargeTimeModifier[0] = level.lieutenantChargeTimeModifier[1] = 1.f;
	level.covertopsChargeTimeModifier[0] = level.covertopsChargeTimeModifier[1] = 1.f;

	cs[0] = '\0';
	Info_SetValueForKey( cs, "axs_sld", va("%i", level.soldierChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_sld", va("%i", level.soldierChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_mdc", va("%i", level.medicChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_mdc", va("%i", level.medicChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_eng", va("%i", level.engineerChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_eng", va("%i", level.engineerChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_lnt", va("%i", level.lieutenantChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_lnt", va("%i", level.lieutenantChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_cvo", va("%i", level.covertopsChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_cvo", va("%i", level.covertopsChargeTime[1]) );
	trap_SetConfigstring( CS_CHARGETIMES, cs );
	trap_SetConfigstring( CS_FILTERCAMS, va( "%i", g_filtercams.integer ) );

	G_SoundIndex( "sound/misc/referee.wav"	);
	G_SoundIndex( "sound/misc/vote.wav"		);
	G_SoundIndex( "sound/player/gurp1.wav"	);
	G_SoundIndex( "sound/player/gurp2.wav"	);

	if( g_gametype.integer == GT_WOLF_LMS ) {
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
		Info_SetValueForKey( cs, "winner", "-1" );
		trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );

		level.firstbloodTeam = -1;

		if( g_currentRound.integer == 0 ) {
			trap_Cvar_Set( "g_axiswins", "0" );
			trap_Cvar_Set( "g_alliedwins", "0" );

			trap_Cvar_Update( &g_axiswins );
			trap_Cvar_Update( &g_alliedwins );
		}

		trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
		trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );
	}

	if( g_gametype.integer == GT_WOLF ) {
		//bani - #113
		bani_clearmapxp();
	}

	if( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		//bani - #113
		bani_clearmapxp();
	}


	trap_GetServerinfo( cs, sizeof( cs ) );
	Q_strncpyz( level.rawmapname, Info_ValueForKey( cs, "mapname" ), sizeof(level.rawmapname) );

	G_ParseCampaigns();
	if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {
		char maps[(64*MAX_MAPS_PER_CAMPAIGN)+MAX_MAPS_PER_CAMPAIGN];
		char *map;
		int current = g_campaigns[level.currentCampaign].current;

		if(current == 0 || level.newCampaign) {

			trap_Cvar_Set( "g_axiswins", "0" );
			trap_Cvar_Set( "g_alliedwins", "0" );

			//bani - #113
			bani_clearmapxp();

			trap_Cvar_Update( &g_axiswins );
			trap_Cvar_Update( &g_alliedwins );

		} else {
			//bani - #113
			bani_getmapxp();
		}

		maps[0] = '\0';
		for(i=0; i<MAX_MAPS_PER_CAMPAIGN; i++) {
			map = g_campaigns[level.currentCampaign].mapnames[i];
			if(*map) {
				Q_strcat(maps, sizeof(maps), map);
				Q_strcat(maps, sizeof(maps), ",");
			}
		}
		if(maps[0]) 
			maps[strlen(maps)-1] = '\0';
		trap_Cvar_Set("campaign_maps", maps);

		trap_Cvar_Set("C", va("%d,%d",
				g_campaigns[level.currentCampaign].current+1,
				g_campaigns[level.currentCampaign].mapCount));
	}

	trap_SetConfigstring( CS_SCRIPT_MOVER_NAMES, "" );	// clear out

	G_DebugOpenSkillLog();

	// jet Pilot - separate log file for logging chats
	if ( g_logChat.string[0] )
	{
		if ( g_logSync.integer ) 
		{
			trap_FS_FOpenFile( g_logChat.string, &level.chatLog, FS_APPEND_SYNC );
		}
		else 
		{
			trap_FS_FOpenFile( g_logChat.string, &level.chatLog, FS_APPEND );
		}

		if (!level.chatLog)
		{
			G_Printf( "^3WARNING: Couldn't open chatlog: %s\n", g_logChat.string);
		}		
	}
	else
	{
		G_Printf( "Not logging chats to disk.\n" );
	}

	// jaquboss - separate log file for crashes
	if ( g_logCrash.string[0] )
	{
		if ( g_logSync.integer ) 
		{
			trap_FS_FOpenFile( g_logCrash.string, &level.crashLog, FS_APPEND_SYNC );
		}
		else 
		{
			trap_FS_FOpenFile( g_logCrash.string, &level.crashLog, FS_APPEND );
		}

		if (!level.crashLog)
		{
			G_Printf( "^3WARNING: Couldn't open crashlog: %s\n", g_logCrash.string);
		}		
	}
	else
	{
		G_Printf( "Not logging crashes to disk.\n" );
	}

	// set up log file
	if ( g_log.string[0] )
	{
		if ( g_logSync.integer ) 
		{
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
		}
		else 
		{
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) 
		{
			G_Printf( "^3WARNING: Couldn't open logfile: %s\n", g_log.string );
		}
		else 
		{
			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", cs );
		}
	} 
	else 
	{
			G_Printf( "Not logging to disk.\n" );
	}

	if ( g_shrubbot.string[0] ) 
	{
		G_shrubbot_readconfig(NULL, 0);
	}
	
	// Perform any xpsave code tasks
	G_xpsave_init_game();

	if(g_mapConfigs.string[0]) 
	{		
		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/default.cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);

		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/");
		Q_strcat(mapConfig, sizeof(mapConfig), level.rawmapname);
		Q_strcat(mapConfig, sizeof(mapConfig), ".cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);
	}

	G_InitWorldSession();

	// DHM - Nerve :: Clear out spawn target config strings
	trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof(cs) );
	Info_SetValueForKey( cs, "numspawntargets", "0" );
	trap_SetConfigstring( CS_MULTI_INFO, cs );

	for ( i=CS_MULTI_SPAWNTARGETS; i<CS_MULTI_SPAWNTARGETS + MAX_MULTI_SPAWNTARGETS; i++ ) {
		trap_SetConfigstring( i, "" );
	}

	G_ResetTeamMapData();

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	// jaquboss load etpro configs
	trap_SetConfigstring( CS_CONFIGNAME, "" );

	G_LoadConfig("", qfalse);

	// load level script
	G_Script_ScriptLoad();

	// reserve some spots for dead player bodies
#ifdef USE_BODY_QUE
	InitBodyQue();
#endif
	numSplinePaths = 0 ;
	numPathCorners = 0;

	InitCameras();		// jaquboss missile/mortar camera portals

	if(!((g_XPSave.integer & XPSF_NR_EVER) ||

		(g_gametype.integer == GT_WOLF_CAMPAIGN && 
		!(g_campaigns[level.currentCampaign].current == 0 ||
		  level.newCampaign)) ||
		
		((g_gametype.integer == GT_WOLF_STOPWATCH ||
		g_gametype.integer == GT_WOLF_LMS) &&
		g_currentRound.integer))) {
		
		G_xpsave_resetxp();
	}

	// START	Mad Doctor I changes, 8/21/2002
	// This needs to be called before G_SpawnEntitiesFromString, or the 
	// bot entities get trashed.
	//initialize the bot game entities
//	BotInitBotGameEntities();
	// END		Mad Doctor I changes, 8/21/2002

	// TAT 11/13/2002
	//		similarly set up the Server entities
	InitServerEntities();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// TAT 11/13/2002 - entities are spawned, so now we can do setup
	InitialServerEntitySetup();

	// Gordon: debris test
	G_LinkDebris();

	// Gordon: link up damage parents
	G_LinkDamageParents();

	BG_ClearScriptSpeakerPool();

	BG_LoadSpeakerScript( va( "sound/maps/%s.sps", level.rawmapname ) );

	// ===================

	if( !level.gameManager ) {
		G_Printf( "^1ERROR No 'script_multiplayer' found in map\n" );
	}

	level.tracemapLoaded = qfalse;
	if( !BG_LoadTraceMap( level.rawmapname, level.mapcoordsMins, level.mapcoordsMaxs ) ) {
		G_Printf( "^1ERROR No tracemap found for map\n" );
	} else {
		level.tracemapLoaded = qtrue;
	}

	// Link all the splines up
	BG_BuildSplinePaths();

	// create the camera entity that will communicate with the scripts
//	G_SpawnScriptCamera();

	// general initialization
	G_FindTeams();

	G_Printf ("-----------------------------------\n");

	trap_PbStat ( -1 , "INIT" , "GAME" ) ;

#ifndef NO_BOT_SUPPORT
	if ( bot_enable.integer ) {
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}
#endif // NO_BOT_SUPPORT

	G_RemapTeamShaders();

	BG_ClearAnimationPool();

	BG_ClearCharacterPool();

	BG_InitWeaponStrings();

	G_RegisterPlayerClasses();

	// Match init work
		G_loadMatchGame();

	// jaquboss init those
	{
		char cs[MAX_INFO_STRING];  
		cs[0] = '\0';  

		for ( i=0; i < SK_NUM_SKILLS;i++){
			InitSkillLevelStructure( i );
			G_ReassignSkillLevel( i );
		}
 
		Info_SetValueForKey(cs, "SS",  
			va("%s", g_skillSoldier.string)); 
		Info_SetValueForKey(cs, "SM",  
			va("%s", g_skillMedic.string)); 
		Info_SetValueForKey(cs, "SE",  
			va("%s", g_skillEngineer.string)); 
		Info_SetValueForKey(cs, "SF",  
			va("%s", g_skillFieldOps.string)); 
		Info_SetValueForKey(cs, "SC",  
			va("%s", g_skillCovertOps.string)); 
		Info_SetValueForKey(cs, "SB",  
			va("%s", g_skillBattleSense.string)); 
		Info_SetValueForKey(cs, "SL",  
			va("%s", g_skillLightWeapons.string)); 	

		trap_SetConfigstring(CS_SKILLLEVELS, cs);  
	}  
	// Reinstate any MV views for clients -- need to do this after all init is complete
	// --- maybe not the best place to do this... seems to be some race conditions on map_restart
#ifdef MV_SUPPORT
	G_spawnPrintf(DP_MVSPAWN, level.time + 2000, NULL);
#endif
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {

	// Arnout: gametype latching
	if	( 
		( ( g_gametype.integer == GT_WOLF || g_gametype.integer == GT_WOLF_CAMPAIGN ) && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_GT_WOLF)) ||
		(g_gametype.integer == GT_WOLF_STOPWATCH && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_STOPWATCH)) ||
		(g_gametype.integer == GT_WOLF_LMS && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_LMS))		
		) {

		if ( !(g_entities[ENTITYNUM_WORLD].r.worldflags & NO_GT_WOLF) )
			trap_Cvar_Set( "g_gametype", va("%i", GT_WOLF) );
		else
			trap_Cvar_Set( "g_gametype", va("%i", GT_WOLF_LMS) );

		trap_Cvar_Update( &g_gametype );
	}

	G_Printf ("==== ShutdownGame ====\n");

	G_DebugCloseSkillLog();

	if ( level.chatLog)
	{
		const char *msg = "~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+\n";

		trap_FS_Write(msg, strlen(msg), level.chatLog);
		trap_FS_FCloseFile( level.chatLog );
		level.chatLog = 0;
	}

	if ( level.crashLog ){
		trap_FS_FCloseFile( level.crashLog );
		level.crashLog = 0;
	}

	if ( level.logFile ) 
	{
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

	// write all the client session data so we can get it back
	G_WriteSessionData( restart );

#ifndef NO_BOT_SUPPORT
	if ( bot_enable.integer ) {
		BotAIShutdown( restart );
	}
#endif // NO_BOT_SUPPORT

	// zinx - realistic hitboxes
	mdx_cleanup();

	G_shrubbot_cleanup();  
}

//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error ( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	G_Error( "%s", text);
}
//bani
void QDECL Com_Error( int level, const char *error, ... )_attribute((format(printf,2,3)));

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	G_Printf ("%s", text);
}
//bani  
void QDECL Com_Printf( const char *msg, ... )_attribute((format(printf,1,2)));

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( /*ca->sess.spectatorState == SPECTATOR_SCOREBOARD ||*/ ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( /*cb->sess.spectatorState == SPECTATOR_SCOREBOARD ||*/ cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		// then sort by score
		if ( ca->ps.persistant[PERS_SCORE]
			> cb->ps.persistant[PERS_SCORE] ) {
			return -1;
		}
		if ( ca->ps.persistant[PERS_SCORE]
			< cb->ps.persistant[PERS_SCORE] ) {
			return 1;
		}
	} else {
		int i, totalXP[2];

		for( totalXP[0] = totalXP[1] = 0, i = 0; i < SK_NUM_SKILLS; i++ ) {
			totalXP[0] += ca->sess.skillpoints[i];
			totalXP[1] += cb->sess.skillpoints[i];
		}

		// then sort by xp
		if ( totalXP[0] > totalXP[1] ) {
			return -1;
		}
		if ( totalXP[0] < totalXP[1] ) {
			return 1;
		}
	}
	return 0;
}

//bani - #184
//(relatively) sane replacement for OSP's Players_Axis/Players_Allies
void etpro_PlayerInfo( void ) {
	//128 bits
	char playerinfo[MAX_CLIENTS + 1];
	gentity_t *e;
	team_t playerteam;
	int i;
	int lastclient;

	memset( playerinfo, 0, sizeof( playerinfo ) );

	lastclient = -1;
	e = &g_entities[0];
	for ( i = 0; i < MAX_CLIENTS; i++, e++ ) {
		if( e->client == NULL || e->client->pers.connected == CON_DISCONNECTED ) {
			playerinfo[i] = '-';
			continue;
		}

		//keep track of highest connected/connecting client
		lastclient = i;

		if( e->inuse == qfalse ) {
			playerteam = 0;
		} else {
			playerteam = e->client->sess.sessionTeam;
		}
		playerinfo[i] = (char)'0' + playerteam;
	}
	//terminate the string, if we have any non-0 clients
	if( lastclient != -1 ) {
		playerinfo[lastclient+1] = (char)0;
	} else {
		playerinfo[0] = (char)0;
	}

	trap_Cvar_Set( "P", playerinfo );
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int		i;
//	int		rank;
//	int		score;
//	int		newScore;
	char	teaminfo[TEAM_NUM_TEAMS][256];	// OSP
//	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.voteInfo.numVotingClients = 0;		// don't count bots

	level.numFinalDead[0] = 0;		// NERVE - SMF
	level.numFinalDead[1] = 0;		// NERVE - SMF

	level.voteInfo.numVotingTeamClients[ 0 ] = 0;
	level.voteInfo.numVotingTeamClients[ 1 ] = 0;

	for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
		if( i < 2 ) {
			level.numTeamClients[i] = 0;
		}
		teaminfo[i][0] = 0;			// OSP
	}

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			int team = level.clients[i].sess.sessionTeam;

			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( team != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;
			
				// OSP
				Q_strcat(teaminfo[team], sizeof(teaminfo[team])-1, va("%d ", level.numConnectedClients));
			
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					int teamIndex = level.clients[i].sess.sessionTeam == TEAM_AXIS ? 0 : 1;
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						level.voteInfo.numVotingClients++;
					}

					if( level.clients[i].sess.sessionTeam == TEAM_AXIS ||
						level.clients[i].sess.sessionTeam == TEAM_ALLIES ) {
						if( g_gametype.integer == GT_WOLF_LMS ) {
							if( g_entities[i].health <= 0 || level.clients[i].ps.pm_flags & PMF_LIMBO ) {
								level.numFinalDead[teamIndex]++;
							}
						} else {
							if( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0 && g_entities[i].health <= 0 ) {
								level.numFinalDead[teamIndex]++;
							}
						}

						level.numTeamClients[teamIndex]++;
						if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
							level.voteInfo.numVotingTeamClients[ teamIndex ]++;
						}
					}

					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	// OSP
	for(i=0; i<TEAM_NUM_TEAMS; i++) {
		if(0 == teaminfo[i][0]) Q_strncpyz(teaminfo[i], "(None)", sizeof(teaminfo[i]));
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
/*		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
		if ( level.teamScores[TEAM_AXIS] == level.teamScores[TEAM_ALLIES] ) {
				cl->ps.persistant[PERS_RANK] = 2;
		} else if ( level.teamScores[TEAM_AXIS] > level.teamScores[TEAM_ALLIES] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
*/
	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
//	trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_AXIS] ) );
//	trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_ALLIES] ) );

	trap_SetConfigstring( CS_FIRSTBLOOD, va("%i", level.firstbloodTeam ) );
	trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
	trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );

	trap_Cvar_Set("Players_Axis", teaminfo[TEAM_AXIS]);
	trap_Cvar_Set("Players_Allies", teaminfo[TEAM_ALLIES]);

	//bani - #184
	etpro_PlayerInfo();

	// if we are at the intermission, send the new info to everyone
	if( g_gamestate.integer == GS_INTERMISSION ) {
		SendScoreboardMessageToAllClients();
	} else {
		// see if it is time to end the level
		CheckExitRules();
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for(i=0; i<level.numConnectedClients; i++) {
		if(level.clients[level.sortedClients[i]].pers.connected == CON_CONNECTED) {
			level.clients[level.sortedClients[i]].wantsscore = qtrue;
//			G_SendScore(g_entities + level.sortedClients[i]);
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
//	float			timeLived;

	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}

	/*if ( ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES ) {
		timeLived = (level.time - ent->client->pers.lastSpawnTime) * 0.001f;
	
		G_AddExperience( ent, min((timeLived * timeLived) * 0.00005f, 5) );
	}*/

	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	// memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->s.events[0] = ent->s.events[1] = ent->s.events[2] = ent->s.events[3] = 0;		// DHM - Nerve
	ent->r.contents = 0;

	// todo: call bot routine so they can process the transition to intermission (send voice chats, etc)
	BotMoveToIntermission( ent->s.number );
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t	*ent = NULL, *target;
	vec3_t		dir;
	char		cs[MAX_STRING_CHARS];		// DHM - Nerve
	char		*buf;						// DHM - Nerve
	int			winner;						// DHM - Nerve

	// NERVE - SMF - if the match hasn't ended yet, and we're just a spectator
	if( !level.intermissiontime ) {
		// try to find the intermission spawnpoint with no team flags set
		ent = G_Find( NULL, FOFS(classname), "info_player_intermission" );

		for( ; ent; ent = G_Find (ent, FOFS(classname), "info_player_intermission") ) {
			if( !ent->spawnflags )
				break;
		}
	}

	trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
	buf = Info_ValueForKey( cs, "winner" );
	winner = atoi( buf );

	// Change from scripting value for winner (0==AXIS, 1==ALLIES) to spawnflag value
	if( winner == 0 ) {
		winner = TEAM_AXIS;
	} else {
		winner = TEAM_ALLIES;
	}


	if( !ent ) {
		ent = G_Find( NULL, FOFS(classname), "info_player_intermission" );
		while( ent ) {
			if( ent->spawnflags & winner ) {
				break;
			}

			ent = G_Find( ent, FOFS(classname), "info_player_intermission" );
		}
	}

	if( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle );
	} else {
		VectorCopy( ent->s.origin, level.intermission_origin );
		VectorCopy( ent->s.angles, level.intermission_angle );
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int			i;
	gentity_t	*client;
	int itime;

	if( g_gamestate.integer == GS_INTERMISSION ) {
		return;		// already active
	}

	level.intermissiontime = level.time;

	// tjw: jaybird's fix for tricking the client into drawing
	//      the correct countdown timer.
	itime = level.intermissiontime;
	if(g_intermissionTime.integer > 0) {
		itime -= (60000 - (g_intermissionTime.integer * 1000));
	}
	trap_SetConfigstring(CS_INTERMISSION_START_TIME,
		va("%i", itime));
	trap_Cvar_Set("gamestate", va( "%i", GS_INTERMISSION));
	trap_Cvar_Update( &g_gamestate );

	FindIntermissionPoint();

	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar 

=============
*/
void ExitLevel (void) {
	int		i;
	gclient_t *cl;

	if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {
		g_campaignInfo_t *campaign = &g_campaigns[level.currentCampaign];

		if( campaign->current + 1 < campaign->mapCount ) {
			trap_Cvar_Set( "g_currentCampaignMap", va( "%i", campaign->current + 1 ) );
#if 0
			if( g_developer.integer )
				trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s\n", campaign->mapnames[campaign->current + 1] ) );
			else
#endif
				trap_SendConsoleCommand( EXEC_APPEND, va( "map %s\n", campaign->mapnames[campaign->current + 1] ) );
		} else {
			char s[MAX_STRING_CHARS];
			trap_Cvar_VariableStringBuffer( "nextcampaign", s, sizeof(s) );

			if( *s ) {
				trap_SendConsoleCommand( EXEC_APPEND, "vstr nextcampaign\n" );
			} else {
				// restart the campaign
				trap_Cvar_Set( "g_currentCampaignMap", "0" );
#if 0
				if( g_developer.integer )
					trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s\n", campaign->mapnames[0] ) );
				else
#endif
					trap_SendConsoleCommand( EXEC_APPEND, va( "map %s\n", campaign->mapnames[0] ) );
			}

			// FIXME: do we want to do something else here?
			//trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
		}
	} else if( g_gametype.integer == GT_WOLF_LMS ) {
		if( level.lmsDoNextMap ) {
			trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
		} else {
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		}
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	}
	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_AXIS] = 0;
	level.teamScores[TEAM_ALLIES] = 0;
	if( g_gametype.integer != GT_WOLF_CAMPAIGN ) {
		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			cl->ps.persistant[PERS_SCORE] = 0;
		}
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData( qfalse );

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {

		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
			trap_UnlinkEntity(&g_entities[i]);
		}
	}

	G_LogPrintf( "ExitLevel: executed\n" );
}

// dvl - real time stamp
char *G_GetRealTime(void) 
{
	qtime_t tm;

	trap_RealTime(&tm);
	return va("%2i:%s%i:%s%i",
				tm.tm_hour,
				(tm.tm_min > 9 ? "" : "0"),// minute padding
				tm.tm_min,
				(tm.tm_sec > 9 ? "" : "0"),// second padding
				tm.tm_sec );
}



/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
jet Pilot
modified to allow separate logfile for chat messages 
*/

void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list			argptr;
	char			string[1024];
	int				min, tens, sec, l;
	qboolean		appendLF;		// append a \n at the end of each line (for chatlog)

	fileHandle_t	curFile;

	if (*fmt && fmt[0] == 's' && fmt[1] == 'a' && fmt[2] == 'y' && level.chatLog)
	{	
		curFile  = level.chatLog;
		appendLF = qtrue;
	}
	else
	{
		curFile = level.logFile;
		appendLF = qfalse;
	}	

	// dvl - real time stamps
	if(g_logOptions.integer & LOGOPTS_REALTIME)
	{
		Com_sprintf( string, sizeof(string), "%s ", G_GetRealTime() );
	}
	else 
	{
		sec = level.time / 1000;

		min  = sec / 60;
		sec -= min * 60;
		tens = sec / 10;
		sec -= tens * 10;
		Com_sprintf( string, sizeof(string), "%i:%i%i ", min, tens, sec );
	}

	l = strlen( string );

	va_start( argptr, fmt );
	Q_vsnprintf( string + l, sizeof( string ) - l, fmt, argptr );
	va_end( argptr );
	
	if (appendLF)
	{
		string[strlen(string)] = '\n';
	}

	if ( g_dedicated.integer )
	{
		G_Printf( "%s", string + l );
	}

	if ( !curFile ) //!level.logFile )
	{
		return;
	}

	trap_FS_Write( string, strlen( string ), curFile );//level.logFile );
}
//bani
void QDECL G_LogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_CrashLogPrintf( const char *fmt, ... ) {
	va_list			argptr;
	char			string[1024];
	int				min, tens, sec, l;

	// dvl - real time stamps
	if(g_logOptions.integer & LOGOPTS_REALTIME)
	{
		Com_sprintf( string, sizeof(string), "%s ", G_GetRealTime() );
	}
	else 
	{
		sec = level.time / 1000;

		min  = sec / 60;
		sec -= min * 60;
		tens = sec / 10;
		sec -= tens * 10;
		Com_sprintf( string, sizeof(string), "%i:%i%i ", min, tens, sec );
	}

	l = strlen( string );

	va_start( argptr, fmt );
	Q_vsnprintf( string + l, sizeof( string ) - l, fmt, argptr );
	va_end( argptr );
	
	if ( g_dedicated.integer )
		G_Printf( "%s", string + l );

	if ( !level.crashLog )
		return;

	trap_FS_Write( string, strlen( string ), level.crashLog );
}

void QDECL G_CrashLogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int				i;
	gclient_t		*cl;
	char			cs[MAX_STRING_CHARS];
	int winner = -1;

	// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
	if ( g_gamestate.integer != GS_PLAYING )
		return;

	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	for( i = 0; i < level.numConnectedClients; i++ ) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		// rain - #105 - use G_MakeUnready instead
		G_MakeUnready(&g_entities[level.sortedClients[i]]);

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		// CHRUKER: b016 - Make sure all the stats are recalculated
		//          and accurate
		G_CalcRank(cl);

		jP_UpdateSpree( &g_entities[level.sortedClients[i]], NULL, qfalse, qtrue );

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i], cl->pers.netname );
	}

	// CHRUKER: b016 - Moved here because we needed the stats to be 
	//			up-to-date before sending	
	// NERVE - SMF - send gameCompleteStatus message to master servers
	trap_SendConsoleCommand( EXEC_APPEND, "gameCompleteStatus\n" );

	G_LogPrintf("red:%i  blue:%i\n",
		level.teamScores[TEAM_AXIS],
		level.teamScores[TEAM_ALLIES]);

	trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
	winner = atoi( Info_ValueForKey( cs, "winner" ) );

	// NERVE - SMF
	if( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		char	cs[MAX_STRING_CHARS];
		int		defender;

		trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof(cs) );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		// NERVE - SMF
		if ( !g_currentRound.integer ) {
			if ( winner == defender ) {
				// if the defenders won, use default timelimit
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", g_timelimit.value ) );
			}
			else {
				// use remaining time as next timer
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", (level.timeCurrent - level.startTime) / 60000.f ) );
			}
		}
		else {
			// reset timer
			trap_Cvar_Set( "g_nextTimeLimit", "0" );
		}

		trap_Cvar_Set( "g_currentRound", va( "%i", !g_currentRound.integer ) );

		//bani - #113
		bani_storemapxp();
	}
	// -NERVE - SMF

	else if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {
		int		highestskillpoints, highestskillpointsclient, j, teamNum;
		// CHRUKER: b017 - Preventing medals from being handed out left and right
		int		highestskillpointsincrease = 0;

		if( winner == 0 ) {
			g_axiswins.integer |= (1 << g_campaigns[level.currentCampaign].current);
			trap_Cvar_Set( "g_axiswins", va( "%i", g_axiswins.integer ) );
			trap_Cvar_Update( &g_axiswins );
		} else if( winner == 1 ) {
			g_alliedwins.integer |= (1 << g_campaigns[level.currentCampaign].current);
			trap_Cvar_Set( "g_alliedwins", va( "%i", g_alliedwins.integer ) );
			trap_Cvar_Update( &g_alliedwins );
		}

		trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
		trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );

		//bani - #113
		bani_storemapxp();

		// award medals
		for( teamNum = TEAM_AXIS; teamNum <= TEAM_ALLIES; teamNum++ ) {
			for( i = 0; i < SK_NUM_SKILLS; i++ ) {
				highestskillpoints = 0;
				highestskillpointsclient = -1;
				for( j = 0; j < level.numConnectedClients; j++ ) {
					cl = &level.clients[level.sortedClients[j]];

					if( cl->sess.sessionTeam != teamNum )
						continue;
					
					// CHRUKER: b017 
					if( cl->sess.skill[i] < 1 )
						continue;
					// CHRUKER: b017
					if ( i == SK_BATTLE_SENSE || i == SK_LIGHT_WEAPONS ) {
						if( cl->sess.skillpoints[i] > highestskillpoints ) {
							highestskillpoints = cl->sess.skillpoints[i];
							highestskillpointsclient = j;
						}
					}
					else {
						if( (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]) > highestskillpointsincrease ) {
							highestskillpointsincrease = (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]);
							highestskillpointsclient = j; 
						}
					} // b017
				}

				if( highestskillpointsclient >= 0 ) {
					// highestskillpointsclient is the first client that has this highest
					// score. See if there are more clients with this same score. If so,
					// give them medals too
					for( j = highestskillpointsclient; j < level.numConnectedClients; j++ ) {
						cl = &level.clients[level.sortedClients[j]];

						if( cl->sess.sessionTeam != teamNum )
							continue;

						 // CHRUKER: b017 
						if( cl->sess.skill[i] < 1 )
							continue;

						 // CHRUKER: b017 
						if ( i == SK_BATTLE_SENSE || i == SK_LIGHT_WEAPONS ) {
							if( cl->sess.skillpoints[i] == highestskillpoints ) {
								cl->sess.medals[i]++;
	
								ClientUserinfoChanged( level.sortedClients[j] );
							}
						}
						else {
							if( (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]) == highestskillpointsincrease ) {
								cl->sess.medals[i]++;
								ClientUserinfoChanged( level.sortedClients[j] );
							}
						} // b017
					}
				}
			}
		}
	} else if( g_gametype.integer == GT_WOLF_LMS ) {
		int		roundLimit = g_lms_roundlimit.integer < 3 ? 3 : g_lms_roundlimit.integer;
		int		numWinningRounds = (roundLimit / 2) + 1;

		roundLimit -= 1;	// -1 as it starts at 0

		if( winner == -1 ) {
			// who drew first blood?
			if( level.firstbloodTeam == TEAM_AXIS )
				winner = 0;
			else
				winner = 1;
		}

		if( winner == 0 ) {
			trap_Cvar_Set( "g_axiswins", va( "%i", g_axiswins.integer + 1 ) );
			trap_Cvar_Update( &g_axiswins );
		} else {
			trap_Cvar_Set( "g_alliedwins", va( "%i", g_alliedwins.integer + 1 ) );
			trap_Cvar_Update( &g_alliedwins );
		}

		if( g_currentRound.integer >= roundLimit || g_axiswins.integer == numWinningRounds || g_alliedwins.integer == numWinningRounds ) {
			trap_Cvar_Set( "g_currentRound", "0" );
			if( g_lms_currentMatch.integer + 1 >= g_lms_matchlimit.integer ) {
				trap_Cvar_Set( "g_lms_currentMatch", "0" );
				level.lmsDoNextMap = qtrue;
			} else {
				trap_Cvar_Set( "g_lms_currentMatch", va( "%i", g_lms_currentMatch.integer + 1 ) );
				level.lmsDoNextMap = qfalse;
			}
		} else {
			trap_Cvar_Set( "g_currentRound", va( "%i", g_currentRound.integer + 1 ) );
			trap_Cvar_Update( &g_currentRound );
		}
	} else if( g_gametype.integer == GT_WOLF ) {
		//bani - #113
		bani_storemapxp();
	}

//	G_BuildEndgameStats();
	jP_BuildEndgameStats();

	G_xpsave_writestats();

	// tjw: when the map finishes normally this must be cleared 	
	trap_Cvar_Set( "g_reset", "0");
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	static int fActions = 0;
	qboolean exit = qfalse;
	int i;
	// rain - for #105
	gclient_t *cl;
	int ready = 0, notReady = 0;

	// OSP - end-of-level auto-actions
	//		  maybe make the weapon stats dump available to single player?
	if(!(fActions & EOM_WEAPONSTATS) && level.time - level.intermissiontime > 300) {
		G_matchInfoDump(EOM_WEAPONSTATS);
		fActions |= EOM_WEAPONSTATS;
	}
	if(!(fActions & EOM_MATCHINFO) && level.time - level.intermissiontime > 800) {
		G_matchInfoDump(EOM_MATCHINFO);
		fActions |= EOM_MATCHINFO;
	}

	for( i = 0; i < level.numConnectedClients; i++ ) {
		// rain - #105 - spectators and people who are still loading
		// don't have to be ready at the end of the round.
		// additionally, make readypercent apply here.

		cl = level.clients + level.sortedClients[i];

		if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		} else if ( cl->pers.ready || ( g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) ) {
			ready++;
		} else {
			notReady++;
		}
	}
	if(!level.numConnectedClients) return;
	if((100.0f * (ready / (level.numConnectedClients * 1.0f))) >= 
	       (g_intermissionReadyPercent.value * 1.0f)) {

		exit = qtrue;
	}

	// Gordon: changing this to a minute for now
	// tjw: making this a cvar
	if(!exit && (level.time < (level.intermissiontime + 
		(1000 * g_intermissionTime.integer)))) {

		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int		a/*, b*/;
	char	cs[MAX_STRING_CHARS];
	char	*buf;

	// DHM - Nerve :: GT_WOLF checks the current value of
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );

		buf = Info_ValueForKey( cs, "winner" );
		a = atoi( buf );

		return a == -1;
}

qboolean G_ScriptAction_SetWinner( gentity_t *ent, char *params );

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
	char	cs[MAX_STRING_CHARS];

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if( g_gamestate.integer == GS_INTERMISSION ) {
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued ) {
		level.intermissionQueued = 0;
		BeginIntermission();
		return;
	}

	// tjw: is this really necessary?	
	//if ( level.numPlayingClients < 2 ) {
	//	return;
	//}

	// tjw: moved this check before timelimit for the case of multi-objective
	//      maps
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
			if ( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Axis team eliminated." );
			}
			else if ( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Allied team eliminated." );
			}
		}
	}

	if ( g_timelimit.value && !level.warmupTime ) {
		// OSP
		if((level.timeCurrent - level.startTime) >= (g_timelimit.value * 60000)) {
		// OSP

			// Check who has the most players alive
			if( g_gametype.integer == GT_WOLF_LMS ) {
				int axisSurvivors, alliedSurvivors;

				axisSurvivors = level.numTeamClients[0] - level.numFinalDead[0];
				alliedSurvivors = level.numTeamClients[1] - level.numFinalDead[1];

				//bani - if team was eliminated < 3 sec before round end, we _properly_ end it here
				if( level.teamEliminateTime ) {
					LogExit( va( "%s team eliminated.", level.lmsWinningTeam == TEAM_ALLIES ? "Axis" : "Allied" ) );
				}

				if( axisSurvivors == alliedSurvivors ) {
					// First blood wins
					if( level.firstbloodTeam == TEAM_AXIS ) {
						trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
						Info_SetValueForKey( cs, "winner", "0" );
						trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
						LogExit( "Axis team wins by drawing First Blood." );
						trap_SendServerCommand( -1, "print \"Axis team wins by drawing First Blood.\n\"");
					} else if( level.firstbloodTeam == TEAM_ALLIES ) {
						trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
						Info_SetValueForKey( cs, "winner", "1" );
						trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
						LogExit( "Allied team wins by drawing First Blood." );
						trap_SendServerCommand( -1, "print \"Allied team wins by drawing First Blood.\n\"");
					} else {
						// no winner yet - sudden death!
						return;
					}
				} else if( axisSurvivors > alliedSurvivors ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "0" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Axis team has the most survivors." );
					trap_SendServerCommand( -1, "print \"Axis team has the most survivors.\n\"");
					return;
				} else {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "1" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Allied team has the most survivors." );
					trap_SendServerCommand( -1, "print \"Allied team has the most survivors.\n\"");
					return;
				}
			} else {
				// check for sudden death 
				if ( ScoreIsTied() ) {
					// score is tied, so don't end the game
					return;
				}
			}

			if ( level.gameManager ) {
				G_Script_ScriptEvent( level.gameManager, "trigger", "timelimit_hit" );
			}

			// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
			// - This already happens in LogExit, but we need it for the print command
			if ( g_gamestate.integer != GS_PLAYING )
				return;

			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
			LogExit( "Timelimit hit." );

			return;
		}
	}

	//bani - #444
	//i dont really get the point of the delay anyway, why not end it immediately like maxlives games?
	if( g_gametype.integer == GT_WOLF_LMS ) {
		if( !level.teamEliminateTime ) {
			if( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				level.teamEliminateTime = level.time;
				level.lmsWinningTeam = TEAM_ALLIES;
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			} else if( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				level.teamEliminateTime = level.time;
				level.lmsWinningTeam = TEAM_AXIS;
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			}
		} else if( level.teamEliminateTime + 3000 < level.time ) {
			LogExit( va( "%s team eliminated.", level.lmsWinningTeam == TEAM_ALLIES ? "Axis" : "Allied" ) );
		}
		return;
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
			if ( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Axis team eliminated." );
			}
			else if ( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Allied team eliminated." );
			}
		}
	}
}



/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
=============
CheckWolfMP

NERVE - SMF
=============
*/
/*
void CheckGameState() {
	gamestate_t current_gs;

	current_gs = trap_Cvar_VariableIntegerValue( "gamestate" );

	if ( level.intermissiontime && current_gs != GS_INTERMISSION ) {
		trap_Cvar_Set( "gamestate", va( "%i", GS_INTERMISSION ) );
		return;
	}

	if ( g_noTeamSwitching.integer && !trap_Cvar_VariableIntegerValue( "sv_serverRestarting" ) ) {
		if ( current_gs != GS_WAITING_FOR_PLAYERS && level.numPlayingClients <= 1 && level.lastRestartTime + 1000 < level.time ) {
			level.lastRestartTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WAITING_FOR_PLAYERS ) );
		}
	}

	if ( current_gs == GS_WAITING_FOR_PLAYERS && g_minGameClients.integer > 1 && 
		level.numPlayingClients >= g_minGameClients.integer && level.lastRestartTime + 1000 < level.time ) {

		level.lastRestartTime = level.time;
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	if( g_gametype.integer == GT_WOLF_LMS && current_gs == GS_WAITING_FOR_PLAYERS && level.numPlayingClients > 1
		&& level.lastRestartTime + 1000 < level.time ) {
		level.lastRestartTime = level.time;
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	// if the warmup is changed at the console, restart it
	if ( current_gs == GS_WARMUP_COUNTDOWN && g_warmup.modificationCount != level.warmupModificationCount ) {
		level.warmupModificationCount = g_warmup.modificationCount;
		current_gs = GS_WARMUP;
	}

	// check warmup latch
	if ( current_gs == GS_WARMUP ) {
		int delay = g_warmup.integer;

		if( g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_WOLF_LMS )
			delay *= 2;

		delay++;

		if ( delay < 6 ) {
			trap_Cvar_Set( "g_warmup", "5" );
			delay = 7;
		}

		level.warmupTime = level.time + ( delay * 1000 );
		trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
	}
}
*/

/*
=============
CheckWolfMP

NERVE - SMF - Once a frame, check for changes in wolf MP player state
=============
*/
/*
void CheckWolfMP() {
  // TTimo unused
//	static qboolean latch = qfalse;

	// NERVE - SMF - check game state
	CheckGameState();

	if ( level.warmupTime == 0 ) {
		return;
	}


	// Only do the restart for MP
	if(g_gametype.integer != GT_SINGLE_PLAYER && g_gametype.integer != GT_COOP)

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime ) {
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		return;
	}
}
// -NERVE - SMF
*/
void CheckWolfMP() {
	// check because we run 6 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if(g_gametype.integer >= GT_WOLF) {
		if(g_gamestate.integer == GS_PLAYING || g_gamestate.integer == GS_INTERMISSION) {
			if(level.intermissiontime && g_gamestate.integer != GS_INTERMISSION) trap_Cvar_Set("gamestate", va( "%i", GS_INTERMISSION));
			return;
		}

		// check warmup latch
		if(g_gamestate.integer == GS_WARMUP) {
			if(!g_doWarmup.integer ||
			  (level.numPlayingClients >= match_minplayers.integer &&
			   level.lastRestartTime + 1000 < level.time && G_readyMatchState()))
			{
				int delay = (g_warmup.integer < 10) ? 11 : g_warmup.integer + 1;

				// Why scale these at all?  Minimum would mean 22s on Campaign and 44 on LMS....
				// Once people are ready, they want to get the show rolling :)
/*				if( g_gametype.integer == GT_WOLF_CAMPAIGN )
					delay *= 2;
				else if( g_gametype.integer == GT_WOLF_LMS && !g_doWarmup.integer )
					delay *= 4;
*/

				level.warmupTime = level.time + (delay * 1000);
				trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
				trap_Cvar_Update(&g_gamestate);
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			}
		}

		// if the warmup time has counted down, restart
		if(g_gamestate.integer == GS_WARMUP_COUNTDOWN) {
			if(level.time > level.warmupTime) {
				level.warmupTime += 10000;
				trap_Cvar_Set( "g_restarted", "1" );
				trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
				level.restarted = qtrue;
/*				if(g_fightSound.string[0]) {
					G_globalSound(g_fightSound.string);
				}*/
				return;
			}
		}
	}
}

/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	int pcnt;
	int total;

	if(!level.voteInfo.voteTime ||
		level.voteInfo.vote_fn == NULL ||
		level.time - level.voteInfo.voteTime < 1000) {

		return;
	}

	pcnt = vote_percent.integer;

	if( pcnt > 99 ) {
		pcnt = 99;
	}
	if( pcnt < 1 ) {
		pcnt = 1;
	}

	if((g_voting.integer & VOTEF_USE_TOTAL_VOTERS) &&
		level.time - level.voteInfo.voteTime >= VOTE_TIME) {

		total = (level.voteInfo.voteYes 
			+ level.voteInfo.voteNo);
	}
	else if(level.voteInfo.vote_fn == G_Kick_v ||
		level.voteInfo.vote_fn == G_Surrender_v) {

		gentity_t *other = &g_entities[level.voteInfo.voteCaller];
		if(!other->client ||
			other->client->sess.sessionTeam == TEAM_SPECTATOR ) {

			total = level.voteInfo.numVotingClients;
		}
		else {
			total = level.voteInfo.numVotingTeamClients[other->client->sess.sessionTeam == TEAM_AXIS ? 0 : 1 ];
		}
	}
	else {
		total = level.voteInfo.numVotingClients;
	}

	if( level.voteInfo.voteYes > pcnt*total/100 ) {
		// execute the command, then remove the vote
		if(level.voteInfo.voteYes > total + 1) {
			// Don't announce some votes, as in comp mode, it is generally a ref
			// who is policing people who shouldn't be joining and players don't want
			// this sort of spam in the console
			if(level.voteInfo.vote_fn != G_Kick_v) {
				AP(va("cpm \"^5Referee changed setting! ^7(%s)\n\"", level.voteInfo.voteString));
			}
			G_LogPrintf("Referee Setting: %s\n", level.voteInfo.voteString);
		} else {
			AP("cpm \"^5Vote passed!\n\"");
			G_LogPrintf("Vote Passed: %s\n", level.voteInfo.voteString);
		}

		// Perform the passed vote
		level.voteInfo.vote_fn(NULL, 0, NULL, NULL, qfalse);

		// don't penalize player if the vote passes.
		if((g_voting.integer & VOTEF_NO_POPULIST_PENALTY)) {
			gentity_t *ent;

			ent = &g_entities[level.voteInfo.voteCaller];
			if(ent->client) 
				ent->client->pers.voteCount--;
		}

	}
	else if((level.voteInfo.voteNo &&
		level.voteInfo.voteNo >= (100-pcnt)*total/100) ||
		level.time - level.voteInfo.voteTime >= VOTE_TIME) {

		// same behavior as a no response vote
		AP(va("cpm \"^2Vote FAILED! ^3(%s)\n\"", level.voteInfo.voteString));
		G_LogPrintf("Vote Failed: %s\n", level.voteInfo.voteString);
	}
	else {
		// still waiting for a majority
		return;
	}

	level.voteInfo.voteTime = 0;
	trap_SetConfigstring(CS_VOTE_TIME, "");
}

#ifdef SAVEGAME_SUPPORT
/*
=============
CheckReloadStatus
=============
*/
void G_CheckReloadStatus(void) {
	// if we are waiting for a reload, check the delay time
	if(g_reloading.integer) {
		if (level.reloadDelayTime) {
			if (level.reloadDelayTime < level.time) {

				/*if (g_reloading.integer == RELOAD_NEXTMAP_WAITING) {
					trap_Cvar_Set( "g_reloading", va("%d", RELOAD_NEXTMAP) );	// set so sv_map_f will know it's okay to start a map
					if (g_cheats.integer)
						trap_SendConsoleCommand( EXEC_APPEND, va("spdevmap %s\n", level.nextMap) );
					else
						trap_SendConsoleCommand( EXEC_APPEND, va("spmap %s\n", level.nextMap ) );

				}*//* else if(g_reloading.integer == RELOAD_ENDGAME) {
					G_EndGame();	// kick out to the menu and start the "endgame" menu (credits, etc)

				}*/// else {
					// set the loadgame flag, and restart the server
					trap_Cvar_Set( "savegame_loading", "2" );	// 2 means it's a restart, so stop rendering until we are loaded
					trap_SendConsoleCommand( EXEC_INSERT, "map_restart\n" );
				//}

				level.reloadDelayTime = 0;
			}
		} else if (level.reloadPauseTime) {
			if (level.reloadPauseTime < level.time) {
				trap_Cvar_Set( "g_reloading", "0" );
				level.reloadPauseTime = 0;
			}
		}
	}
}

static void G_EnableRenderingThink(gentity_t *ent)
{
	trap_Cvar_Set( "cg_norender", "0" );
//		trap_S_FadeAllSound(1.0f, 1000);	// fade sound up
	G_FreeEntity( ent );
}

extern gentity_t *BotFindEntityForName( char *name );
extern void Bot_ScriptThink( void );

static void G_CheckLoadGame(void) {
	char loading[4];
	gentity_t *ent = NULL; // TTimo: VC6 'may be used without having been init'
	qboolean ready;

	// have we already done the save or load?
	if (!saveGamePending)
		return;

	// tell the cgame NOT to render the scene while we are waiting for things to settle
	//trap_Cvar_Set( "cg_norender", "1" );

	trap_Cvar_VariableStringBuffer( "savegame_loading", loading, sizeof(loading) );

	//trap_Cvar_Set( "g_reloading", "1" );	// moved down

	if (strlen( loading ) > 0 && atoi(loading) != 0)
	{
		trap_Cvar_Set( "g_reloading", "1" );

		// screen should be black if we are at this stage
		trap_SetConfigstring( CS_SCREENFADE, va("1 %i 1", level.time - 10 ) );

//		if (!reloading && atoi(loading) == 2) {
		if (!(g_reloading.integer) && atoi(loading) == 2) {
			// (SA) hmm, this seems redundant when it sets it above...
//			reloading = qtrue;	// this gets reset at the Map_Restart() since the server unloads the game dll
			trap_Cvar_Set( "g_reloading", "1" );
		}

		ready = qtrue;
		if ((ent = BotFindEntityForName("player")) == NULL)
			ready = qfalse;
		else if (!ent->client || ent->client->pers.connected != CON_CONNECTED)
			ready = qfalse;

		if (ready) {
			trap_Cvar_Set( "savegame_loading", "0" );	// in-case it aborts
			saveGamePending = 0;
			G_LoadGame();

			// RF, spawn a thinker that will enable rendering after the client has had time to process the entities and setup the display
			ent = G_Spawn();
			ent->nextthink = level.time + 200;
			ent->think = G_EnableRenderingThink;

			// wait for the clients to return from faded screen
			//trap_SetConfigstring( CS_SCREENFADE, va("0 %i 1500", level.time + 500) );
			trap_SetConfigstring( CS_SCREENFADE, va("0 %i 750", level.time + 500) );
			level.reloadPauseTime = level.time + 1100;

			// make sure sound fades up
			trap_SendServerCommand(-1, va("snd_fade 1 %d 0", 2000) );	//----(SA)	added

			Bot_ScriptThink();
		}
	} else {

		ready = qtrue;
		if ((ent = BotFindEntityForName("player")) == NULL)
			ready = qfalse;
		else if (!ent->client || ent->client->pers.connected != CON_CONNECTED)
			ready = qfalse;

		// not loading a game, we must be in a new level, so look for some persistant data to read in, then save the game
		if (ready) {
			G_LoadPersistant();		// make sure we save the game after we have brought across the items

			saveGamePending = 0;

// briefing menu will handle transition, just set a cvar for it to check for drawing the 'continue' button
			trap_SendServerCommand(-1, "rockandroll\n");

			level.reloadPauseTime = level.time + 1100;

			Bot_ScriptThink();
		}
	}
}
#endif // SAVEGAME_SUPPORT

/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int g_password_lastMod = -1;
	static int g_teamForceBalance_lastMod = -1;
	static int g_lms_teamForceBalance_lastMod = -1;
	static int g_censor_lastMod = -1;
	static int g_censorNames_lastMod = -1;
	static int g_skillSoldier_lastMod = -1;
	static int g_skillMedic_lastMod = -1;
	static int g_skillEngineer_lastMod = -1;
	static int g_skillFieldOps_lastMod = -1;
	static int g_skillCovertOps_lastMod = -1;
	static int g_skillBattleSense_lastMod = -1;
	static int g_skillLightWeapons_lastMod = -1;
	qboolean	changedLevels = qfalse;

	if ( g_censor.modificationCount != g_censor_lastMod ) {
		g_censor_lastMod = g_censor.modificationCount;
		InitCensorStructure();
	}

	if ( g_censorNames.modificationCount != g_censorNames_lastMod ) {
		g_censorNames_lastMod = g_censorNames.modificationCount;
		InitCensorNamesStructure();
	}

	if ( g_password.modificationCount != g_password_lastMod ) {
		g_password_lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		if( g_lms_teamForceBalance.modificationCount != g_lms_teamForceBalance_lastMod ) {
			g_lms_teamForceBalance_lastMod = g_lms_teamForceBalance.modificationCount;
			if ( g_lms_teamForceBalance.integer ) {
				trap_Cvar_Set( "g_balancedteams", "1" );
			} else {
				trap_Cvar_Set( "g_balancedteams", "0" );
			}
		}
	} else {
		if( g_teamForceBalance.modificationCount != g_teamForceBalance_lastMod ) {
			g_teamForceBalance_lastMod = g_teamForceBalance.modificationCount;
			if ( g_teamForceBalance.integer ) {
				trap_Cvar_Set( "g_balancedteams", "1" );
			} else {
				trap_Cvar_Set( "g_balancedteams", "0" );
			}
		}
	}

	if( g_skillSoldier.modificationCount != g_skillSoldier_lastMod ) {
		g_skillSoldier_lastMod = g_skillSoldier.modificationCount;
		InitSkillLevelStructure( SK_HEAVY_WEAPONS );
		G_ReassignSkillLevel( SK_HEAVY_WEAPONS );
		changedLevels = qtrue;
	}

	if( g_skillMedic.modificationCount != g_skillMedic_lastMod ) {
		g_skillMedic_lastMod = g_skillMedic.modificationCount;
		InitSkillLevelStructure( SK_FIRST_AID );
		G_ReassignSkillLevel( SK_FIRST_AID );
		changedLevels = qtrue;
	}
	
	if( g_skillEngineer.modificationCount != g_skillEngineer_lastMod ) {
		g_skillEngineer_lastMod = g_skillEngineer.modificationCount;
		InitSkillLevelStructure( SK_EXPLOSIVES_AND_CONSTRUCTION );
		G_ReassignSkillLevel(SK_EXPLOSIVES_AND_CONSTRUCTION);
		changedLevels = qtrue;
	}

	if( g_skillFieldOps.modificationCount != g_skillFieldOps_lastMod ) {
		g_skillFieldOps_lastMod = g_skillFieldOps.modificationCount;
		InitSkillLevelStructure( SK_SIGNALS );
		G_ReassignSkillLevel(SK_SIGNALS);
		changedLevels = qtrue;
	}

	if( g_skillCovertOps.modificationCount != g_skillCovertOps_lastMod ) {
		g_skillCovertOps_lastMod = g_skillCovertOps.modificationCount;
		InitSkillLevelStructure( SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS );
		G_ReassignSkillLevel(SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS);
		changedLevels = qtrue;
	}

	if( g_skillBattleSense.modificationCount != g_skillBattleSense_lastMod ) {
		g_skillBattleSense_lastMod = g_skillBattleSense.modificationCount;
		InitSkillLevelStructure( SK_BATTLE_SENSE );
		G_ReassignSkillLevel(SK_BATTLE_SENSE);
		changedLevels = qtrue;
	}

	if( g_skillLightWeapons.modificationCount != g_skillLightWeapons_lastMod ) {
		g_skillLightWeapons_lastMod = g_skillLightWeapons.modificationCount;
		InitSkillLevelStructure( SK_LIGHT_WEAPONS );
		G_ReassignSkillLevel(SK_LIGHT_WEAPONS);
		changedLevels = qtrue;
	}

	if ( changedLevels )
	{  
		char cs[MAX_INFO_STRING];  
		cs[0] = '\0';  
 
		Info_SetValueForKey(cs, "SS",  
			va("%s", g_skillSoldier.string)); 
		Info_SetValueForKey(cs, "SM",  
			va("%s", g_skillMedic.string)); 
		Info_SetValueForKey(cs, "SE",  
			va("%s", g_skillEngineer.string)); 
		Info_SetValueForKey(cs, "SF",  
			va("%s", g_skillFieldOps.string)); 
		Info_SetValueForKey(cs, "SC",  
			va("%s", g_skillCovertOps.string)); 
		Info_SetValueForKey(cs, "SB",  
			va("%s", g_skillBattleSense.string)); 
		Info_SetValueForKey(cs, "SL",  
			va("%s", g_skillLightWeapons.string)); 	

		trap_SetConfigstring(CS_SKILLLEVELS, cs);  
	}  

}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent) {
	int	thinktime;

	// OSP - If paused, push nextthink
	if(level.match_pause != PAUSE_NONE && (ent - g_entities) >= g_maxclients.integer &&
	  ent->nextthink > level.time && strstr(ent->classname, "DPRINTF_") == NULL) {
		ent->nextthink += level.time - level.previousTime;
	}

	// RF, run scripting
	if (ent->s.number >= MAX_CLIENTS) {
		G_Script_ScriptRun( ent );
	}

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}
	
	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

void G_RunEntity( gentity_t* ent, int msec );

/*
======================
G_PositionEntityOnTag
======================
*/
qboolean G_PositionEntityOnTag( gentity_t *entity, gentity_t* parent, char *tagName ) {
	int				i;
	orientation_t	tag;
	vec3_t			axis[3];
	AnglesToAxis( parent->r.currentAngles, axis );

	VectorCopy( parent->r.currentOrigin, entity->r.currentOrigin );
	
	if(!trap_GetTag( -1, parent->tagNumber, tagName, &tag )) {
		return qfalse;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->r.currentOrigin, tag.origin[i], axis[i], entity->r.currentOrigin );
	}

	if( entity->client && entity->s.eFlags & EF_MOUNTEDTANK ) {
		// zinx - moved tank hack to here
		// bani - fix tank bb
		// zinx - figured out real values, only tag_player is applied,
		// so there are two left:
		// mg42upper attaches to tag_mg42nest[mg42base] at:
		// 0.03125, -1.171875, 27.984375
		// player attaches to tag_playerpo[mg42upper] at:
		// 3.265625, -1.359375, 2.96875
		// this is a hack, by the way.
		entity->r.currentOrigin[0] += 0.03125 + 3.265625;
		entity->r.currentOrigin[1] += -1.171875 + -1.359375;
		entity->r.currentOrigin[2] += 27.984375 + 2.96875;
	}

	G_SetOrigin( entity, entity->r.currentOrigin );

	if( entity->r.linked && !entity->client ) {
		if( !VectorCompare( entity->oldOrigin, entity->r.currentOrigin ) ) {
			trap_LinkEntity( entity );
		}
	}

	return qtrue;
}

void G_TagLinkEntity( gentity_t* ent, int msec ) {
	gentity_t* parent = &g_entities[ent->s.torsoAnim];
	vec3_t move, amove;
	gentity_t* obstacle;
	vec3_t origin, angles;
	vec3_t v;

	// Jaybird - clean warnings
	memset(&origin, 0, sizeof(origin));

	if( ent->linkTagTime >= level.time ) {
		return;
	}

	G_RunEntity( parent, msec );

	if (!(parent->s.eFlags & EF_PATH_LINK)) {
		if( parent->s.pos.trType == TR_LINEAR_PATH ) {
			int pos;
			float frac;

			if((ent->backspline = BG_GetSplineData( parent->s.effect2Time, &ent->back )) == NULL) {
				return;
			}

			ent->backdelta = parent->s.pos.trDuration ? (level.time - parent->s.pos.trTime) / ((float)parent->s.pos.trDuration) : 0;

			if(ent->backdelta < 0.f) {
				ent->backdelta = 0.f;
			} else if(ent->backdelta > 1.f) {
				ent->backdelta = 1.f;
			}

			if(ent->back) {
				ent->backdelta = 1 - ent->backdelta;
			}

			pos = floor(ent->backdelta * (MAX_SPLINE_SEGMENTS));
			if(pos >= MAX_SPLINE_SEGMENTS) {
				pos = MAX_SPLINE_SEGMENTS - 1;
				frac = ent->backspline->segments[pos].length;
			} else {
				frac = ((ent->backdelta * (MAX_SPLINE_SEGMENTS)) - pos) * ent->backspline->segments[pos].length;
			}
			

			VectorMA( ent->backspline->segments[pos].start, frac, ent->backspline->segments[pos].v_norm, v );
			if(parent->s.apos.trBase[0]) {
				BG_LinearPathOrigin2( parent->s.apos.trBase[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, origin );
			
			if(ent->s.angles2[0]) {
				BG_LinearPathOrigin2( ent->s.angles2[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, ent->backorigin );

			if(	ent->s.angles2[0] < 0 ) {
				VectorSubtract( v, origin, v );
				vectoangles( v, angles );
			} else if( ent->s.angles2[0] > 0 ){
				VectorSubtract( origin, v, v );
				vectoangles( v, angles );
			} else {
				VectorCopy( vec3_origin, origin );
			}

			ent->moving = qtrue;
		} else {
			ent->moving = qfalse;
		}
	} else {
		if(parent->moving) {
			VectorCopy( parent->backorigin, v );

			ent->back =			parent->back;
			ent->backdelta =	parent->backdelta;
			ent->backspline =	parent->backspline;

			VectorCopy( v, origin );
			
			if(ent->s.angles2[0]) {
				BG_LinearPathOrigin2( ent->s.angles2[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, ent->backorigin );

			if(	ent->s.angles2[0] < 0 ) {
				VectorSubtract( v, origin, v );
				vectoangles( v, angles );				
			} else if( ent->s.angles2[0] > 0 ){
				VectorSubtract( origin, v, v );
				vectoangles( v, angles );
			} else {
				VectorCopy( vec3_origin, origin );
			}

			ent->moving = qtrue;
		} else {
			ent->moving = qfalse;
		}
	}

	if( ent->moving ) {
		VectorSubtract( origin, ent->r.currentOrigin, move );
		VectorSubtract( angles, ent->r.currentAngles, amove );

		if(!G_MoverPush( ent, move, amove, &obstacle )) {
			script_mover_blocked( ent, obstacle );
		}

		VectorCopy( origin, ent->s.pos.trBase );
		VectorCopy( angles, ent->s.apos.trBase );
	} else {
		memset( &ent->s.pos, 0, sizeof( ent->s.pos ) );
		memset( &ent->s.apos, 0, sizeof( ent->s.apos ) );

		VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
		VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );
	}

	ent->linkTagTime = level.time;
}


// move missile camera if needed
qboolean G_missileCamera(gentity_t *ent)
{
	gentity_t	*master=NULL;
	gentity_t	*player=NULL;
	int	i;

	for ( i=0;i< level.num_entities; i++){
		if (g_entities[i].s.eType != ET_MISSILE )
			continue;
		
		if ( ( g_entities[i].s.weapon != WP_MORTAR_SET && g_entities[i].s.weapon != WP_MORTAR2_SET ) || !(jp_insanity.integer & JP_INSANITY_MORTARCAM)) 
		{
			if ( (  g_entities[i].s.weapon != WP_BAZOOKA &&
					g_entities[i].s.weapon != WP_PANZERFAUST &&
					g_entities[i].s.weapon != WP_FIREBOLT ) || !(jp_insanity.integer & JP_INSANITY_MISSILECAM)) 
				continue; 
		}

		if (g_entities[i].r.ownerNum == ent->r.ownerNum ){
			master = &g_entities[i];
			break;
		}
	}

	player = &g_entities[ent->r.ownerNum];

	if ( !master )
		return qfalse;

	if ( !player )
		return qfalse;

	if ( !player->client )
		return qfalse;

	if ( player->client->pers.connected != CON_CONNECTED )
		return qfalse;

	VectorCopy(player->r.currentOrigin, ent->s.origin);
	VectorCopy(player->r.currentOrigin, ent->r.currentOrigin);
	G_SetOrigin(ent, player->r.currentOrigin);
	VectorCopy(master->r.currentOrigin, ent->s.origin2);
	VectorCopy(player->r.currentOrigin, ent->s.pos.trBase);

	ent->s.effect1Time = master->s.number;

	trap_LinkEntity(ent);

	return qtrue;
}
// this sucks, it'll be nicer to do it only on client -> a loooooot of saved entities, and less brandwith
void G_DrawEntBBox ( gentity_t* ent )
{
	vec3_t maxs,mins;

	if ( G_EntitiesFree() < 64 )
		return;

	if ( g_debugHitboxes.string[0] && Q_isalpha(g_debugHitboxes.string[0])){
		if ( ent->classname && !Q_stricmp( ent->classname, g_debugHitboxes.string )) {
			G_RailBox( ent->r.currentOrigin, ent->r.mins, ent->r.maxs, tv(0.5f,0.f,0.5f), ent->s.number );
		}
		return;
	}


	switch ( ent->s.eType )
	{
		case ET_CORPSE:
		case ET_PLAYER:
			if ( g_debugHitboxes.integer != 3 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			maxs[2] = ClientHitboxMaxZ(ent);
			break;
		case ET_BOMB:
		case ET_MISSILE:
			if ( g_debugHitboxes.integer != 4 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_EXPLOSIVE:
			if ( g_debugHitboxes.integer != 5 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_ITEM:
			if ( g_debugHitboxes.integer != 6 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_MOVERSCALED:
		case ET_MOVER:
			if ( g_debugHitboxes.integer != 7 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_AAGUN:
		case ET_MG42_BARREL:
			if ( g_debugHitboxes.integer != 8 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_CONSTRUCTIBLE_INDICATOR:
		case ET_CONSTRUCTIBLE:
		case ET_CONSTRUCTIBLE_MARKER:
			if ( g_debugHitboxes.integer != 9 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_PUSH_TRIGGER:
		case ET_TELEPORT_TRIGGER:
		case ET_CONCUSSIVE_TRIGGER:
		case ET_OID_TRIGGER:
		case ET_TRIGGER_MULTIPLE:
		case ET_TRIGGER_FLAGONLY:
		case ET_TRIGGER_FLAGONLY_MULTIPLE:
			if ( g_debugHitboxes.integer != 10 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_CABINET_H:
		case ET_CABINET_A:
		case ET_HEALER:
		case ET_SUPPLIER:
			if ( g_debugHitboxes.integer != 11 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_EF_SPOTLIGHT:
		case ET_ALARMBOX:
		case ET_FOOTLOCKER:
		case ET_PROP:
		case ET_TRAP:
			if ( g_debugHitboxes.integer != 12 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;


		case ET_GAMEMODEL:
			if ( g_debugHitboxes.integer != 13 )
				return;
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		case ET_GENERAL:
			if ( g_debugHitboxes.integer != 14 )
				return;
			// this is a bit hacky, but well..
			VectorCopy(ent->r.maxs, maxs);
			VectorCopy(ent->r.mins, mins);
			if ( !maxs || !mins )
				return;
			break;

		default:
			return;
	}

	G_RailBox( ent->r.currentOrigin, mins, maxs, tv(0.f,1.f,0.f), ent->s.number);

}


void G_RunEntity( gentity_t* ent, int msec ) {
	if( ent->runthisframe ) {
		return;
	}

	ent->runthisframe = qtrue;

	if( !ent->inuse ) {
		return;
	}

	if ( g_debugHitboxes.integer || g_debugHitboxes.string[0] )
		G_DrawEntBBox(ent);

	if( ent->tagParent ) {

		G_RunEntity( ent->tagParent, msec );

		if( ent->tagParent ) {
			if( G_PositionEntityOnTag( ent, ent->tagParent, ent->tagName ) ) {
				if( !ent->client ) {
					if( !ent->s.density) {
						BG_EvaluateTrajectory( &ent->s.apos, level.time, ent->r.currentAngles, qtrue, ent->s.effect2Time  );
						VectorAdd( ent->tagParent->r.currentAngles, ent->r.currentAngles, ent->r.currentAngles );
					} else {
						BG_EvaluateTrajectory( &ent->s.apos, level.time, ent->r.currentAngles, qtrue, ent->s.effect2Time  );
					}
				}
			}
		}
	} else if (ent->s.eFlags & EF_PATH_LINK ) {

		G_TagLinkEntity( ent, msec );
	}

	// ydnar: hack for instantaneous velocity
	VectorCopy( ent->r.currentOrigin, ent->oldOrigin );

	// check EF_NODRAW status for non-clients
	if( ent-g_entities > level.maxclients ) {
		if (ent->flags & FL_NODRAW) {
			ent->s.eFlags |= EF_NODRAW;
		} else {
			ent->s.eFlags &= ~EF_NODRAW;
		}
	}

	if( ent->s.eType == ET_MISSILECAM ) {
		if ( !G_missileCamera(ent) )
			trap_UnlinkEntity(ent);
			
		return;
	}

	// clear events that are too old
	if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
		if ( ent->s.event ) {
			ent->s.event = 0;
		}
		if ( ent->freeAfterEvent ) {
			// tempEntities or dropped items completely go away after their event
			G_FreeEntity( ent );
			return;
		} else if ( ent->unlinkAfterEvent ) {
			// items that will respawn will hide themselves after their pickup event
			ent->unlinkAfterEvent = qfalse;
			trap_UnlinkEntity( ent );
		}
	}

	// temporary entities don't think
	if( ent->freeAfterEvent ) {
		return;
	}

	// Arnout: invisible entities don't think
	// NOTE: hack - constructible one does
	if( ent->s.eType != ET_CONSTRUCTIBLE && ( ent->entstate == STATE_INVISIBLE || ent->entstate == STATE_UNDERCONSTRUCTION ) ) {
		// Gordon: we want them still to run scripts tho :p
		if (ent->s.number >= MAX_CLIENTS) {
			G_Script_ScriptRun( ent );
		}
		return;
	}


	if ( !ent->r.linked && ent->neverFree ) {
		return;
	}

	if ( ent->s.eType == ET_MISSILE 
		|| ent->s.eType == ET_FLAMEBARREL 
		|| ent->s.eType == ET_FP_PARTS
		|| ent->s.eType == ET_FIRE_COLUMN
		|| ent->s.eType == ET_FIRE_COLUMN_SMOKE
		|| ent->s.eType == ET_EXPLO_PART
		|| ent->s.eType == ET_RAMJET) {

		// OSP - pausing
		if( level.match_pause == PAUSE_NONE ) {
			G_RunMissile( ent );
		} else {
			// During a pause, gotta keep track of stuff in the air
			ent->s.pos.trTime += level.time - level.previousTime;
			// Keep pulsing right for dynmamite
			if( ent->methodOfDeath == MOD_DYNAMITE ) {
				ent->s.effect1Time += level.time - level.previousTime;
			}
			G_RunThink(ent);
		}
		// OSP

		return;
	}

	// DHM - Nerve :: Server-side collision for flamethrower
	if( ent->s.eType == ET_FLAMETHROWER_CHUNK ) {
		G_RunFlamechunk( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	if( ent->s.eType == ET_ITEM || ent->physicsObject ) {
		G_RunItem( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	if ( ent->s.eType == ET_MOVER || ent->s.eType == ET_PROP)
	{
		G_RunMover( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}
	
	if( ent-g_entities < MAX_CLIENTS ) {
		G_RunClient( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	// OSP - multiview
#ifdef MV_SUPPORT
	if( ent->s.eType == ET_PORTAL && G_smvRunCamera(ent) ) {
		return;
	}
#endif

	if((ent->s.eType == ET_HEALER || ent->s.eType == ET_SUPPLIER) && ent->target_ent) {
		ent->target_ent->s.onFireStart =	ent->health;
		ent->target_ent->s.onFireEnd =		ent->count;
	}

	G_RunThink( ent );
		
	// ydnar: hack for instantaneous velocity
	VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
	VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
}

/*
=====================
G_CheckMultiKill

checks if multikill sound should be played
=====================
*/

char multiKillName[5][15] = {
	"Multikill",
	"Ultra Kill",
	"Monster Kill",
	"Mega Kill",
	"Ludicrous Kill",
};

void G_CheckMultiKill ( int clientNum ) {
	gclient_t *client= &level.clients[level.sortedClients[clientNum]];

	if ( !client || !(g_announcer.integer & ANNOUNCE_MULTIKILL) )
		return;
		
	if ( level.time - client->lastKillTime > g_multikillTime.integer ) {
		int	multikillLevel=0;
	
		if ( client->multikill >= 7 ) 
			multikillLevel = 5;//ludicrous kill
		else if (client->multikill >= 6)
			multikillLevel = 4;//mega kill
		else if (client->multikill >= 5)
			multikillLevel = 3;//monster kill
		else if (client->multikill >= 4)
			multikillLevel = 2;//ultra kill
		else if (client->multikill >= 3)
			multikillLevel = 1;//multi kill

		if ( multikillLevel ){

			gentity_t *te;
			te = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex(g_multikillSound[multikillLevel-1].string);
			te->r.svFlags |= SVF_BROADCAST;
			AP(va("chat \"^w!!!! ^1%s ^w> ^*%s ^w< ^1%s ^w!!!!\"", 
				multiKillName[multikillLevel-1], 
				client->pers.netname,
				multiKillName[multikillLevel-1]));
		}
		client->multikill = 0;
	}
}

void ServerOutput ( void )
{
	char pakNames[4096];
	char versionStr[512];
	int i;
	static int whine;
	int	oldWhine;

	trap_Cvar_VariableStringBuffer( "version", versionStr, sizeof(versionStr) );

	oldWhine = whine;
	whine  = (whine+1) % 5;

	// do not whine as often if old version
	if ( !strstr( versionStr, "2.60b") && ( whine < oldWhine ) )
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Wrong ET server application version, not recommended for NQ^3|\n\"") );

	trap_Cvar_VariableStringBuffer( "fs_game", versionStr, sizeof(versionStr) );

	if ( !Q_stricmp(versionStr, "etmain" ) )
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Server installed in etmain directory, features may break!! Contact Server Admin^3|\n\"") );
	else if ( Q_stricmp(versionStr, "noquarter" ) != 0 )
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Wrong mod directory, should be '^2noquarter^d'^3|\n\"") );

	trap_Cvar_VariableStringBuffer( "sv_pure", versionStr, sizeof(versionStr) );
	if ( atoi(versionStr) < 0 )
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Server is not pure!! No Quarter will not work properly! Contact Server Admin^3|\n\"") );


	for ( i=0; i<level.config.numSetl; i++ )
	{
		trap_Cvar_VariableStringBuffer( level.config.setl[i].name, versionStr, sizeof(versionStr) );

		if ( Q_stricmp( versionStr, level.config.setl[i].value ) != 0)
			trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Server Admin changed locked cvar, reload config! ('%s' should be %s not %s)^3|\n\"", level.config.setl[i].name, level.config.setl[i].value, versionStr) );
	}

	trap_Cvar_VariableStringBuffer( "sv_pakNames", pakNames, sizeof(pakNames) );

	if (!strstr(pakNames, "noquarter_b1.1.0"))
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: No ^2noquarter_b1.1.0.pk3^d found on server! Missing essential files! Contact Server Admin^3|\n\"") );

	if (!strstr(pakNames, "nq_bin_b1.1.0"))
		trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: No ^2nq_bin_b1.1.0.pk3^d found on server! Missing essential files! Contact Server Admin^3|\n\"") );

//	2,3,4 - binaries
	for ( i=2;i<5;i++ ) {
		if (strstr(pakNames, va("nq_bin_b1.0.%i", i)) ){
			trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Old binary pk3 found on server. Contact Server Admin^3|\n\"") );
			break;
		}
	}
// 2,3,4 - assets
	for ( i=2;i<5;i++ ) {
		if (strstr(pakNames, va("noquarter_b1.0.%i", i)) ){
			trap_SendServerCommand( -1, va("cpm \"^3|^1Warning^d: Old assets pk3 found on server, contact server Admin^3|\n\"") );
			break;
		}
	}

	if ( trap_Cvar_VariableIntegerValue("sv_floodprotect") ){
		trap_Cvar_Set("g_floodprotect", "1" );
		trap_Cvar_Set("sv_floodprotect", "0" );
	}


	level.whineTime = level.time + (1000*60);

}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int			i, msec;
	static	int	lastCheck;
//	int			pass = 0;

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

	// tjw: workaround for q3 bug 
	//      levelTime will start over when the timelimit 
	//      expires on dual objective maps.
	if(level.previousTime > level.time)
	{
		level.overTime = level.previousTime;
	}

	levelTime = levelTime + level.overTime;

	// Handling of pause offsets
	if( level.match_pause == PAUSE_NONE )
	{
		level.timeCurrent = levelTime - level.timeDelta;
	} 
	else 
	{
		level.timeDelta = levelTime - level.timeCurrent;
		if((level.time % 500) == 0) 
		{
			// FIXME: set a PAUSE cs and let the client adjust their local starttimes
			//        instead of this spam
			trap_SetConfigstring(CS_LEVEL_START_TIME, va("%i", level.startTime + level.timeDelta));
		}
	}

	level.frameTime	= trap_Milliseconds();

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;

	msec = level.time - level.previousTime;

	level.axisBombCounter -= msec;
	level.alliedBombCounter -= msec;
// forty - arty/airstrike rate limiting.  
	level.axisArtyCounter -= msec;  
	level.alliedArtyCounter -= msec;  


	if( level.axisBombCounter < 0 )	{
		level.axisBombCounter = 0;
	}
	if( level.alliedBombCounter < 0 ) {
		level.alliedBombCounter = 0;
    }  
	// forty - arty/airstrike rate limiting.  
	if( level.axisArtyCounter < 0 ) {  
			level.axisArtyCounter = 0;  
	}  
	if( level.alliedArtyCounter < 0 ) {  
			level.alliedArtyCounter = 0;  
	}
	
	// get any cvar changes
	G_UpdateCvars();

	for( i = 0; i < level.num_entities; i++ ) 
	{
		g_entities[i].runthisframe = qfalse;
	}

	// go through all allocated objects
	for( i = 0; i < level.num_entities; i++ ) 
	{
		G_RunEntity( &g_entities[ i ], msec );
	}

//	G_ActiveTeamBalance();

	if(g_msgs.integer && (level.time % 1000 == 0) && (((level.time/1000) % g_msgs.integer) == 0) )
	{
		G_PrintBanners();
	}	

	for( i = 0; i < level.numConnectedClients; i++ )
	{
		ClientEndFrame(&g_entities[level.sortedClients[i]]);
		G_CheckMultiKill(i);
	}

	// NERVE - SMF
	CheckWolfMP();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// for tracking changes
	CheckCvars();

	// jaquboss - naggy messages for wrong installed servers
	if ( level.time > level.whineTime )
		ServerOutput();

	G_UpdateTeamMapData();


	if(level.gameManager)
	{
		level.gameManager->s.otherEntityNum = team_maxLandmines.integer - G_CountTeamLandmines(TEAM_AXIS);
		level.gameManager->s.otherEntityNum2 = team_maxLandmines.integer - G_CountTeamLandmines(TEAM_ALLIES);
	}

	// jaquboss
	if ( g_HQMessages.integer & 1 && (g_gamestate.integer == GS_PLAYING) ){
		G_CheckForNeededClasses( TEAM_ALLIES );
		G_CheckForNeededClasses( TEAM_AXIS );
	} 
	if ( g_HQMessages.integer & 2 && (level.time > lastCheck) && (g_gamestate.integer == GS_PLAYING) ){
		G_CheckMenDown( TEAM_ALLIES );
		G_CheckMenDown( TEAM_AXIS );
		lastCheck = level.time + 1500;
	}

}

// Is this a single player type game - sp or coop?
qboolean G_IsSinglePlayerGame()
{
	if (g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP)
		return qtrue;

	return qfalse;
}

// Josh
void InitCensorStructure( void )
{
	char wordList[MAX_CVAR_VALUE_STRING];
	char *nextWord;
	Q_strncpyz( wordList, g_censor.string, sizeof(g_censor.string) );
	censorDictionary.num_words = 0;
	nextWord = strtok(wordList," ,");
	while (nextWord && censorDictionary.num_words != DICT_MAX_WORDS) {
		Q_strncpyz( censorDictionary.words[censorDictionary.num_words], nextWord,
									sizeof(censorDictionary.words[censorDictionary.num_words]));
		nextWord = strtok(NULL," ,");
		if (!nextWord) {
			break;
		}
		censorDictionary.num_words++;
	}
	censorDictionary.num_words++;
}

// Josh
void InitCensorNamesStructure( void )
{
	char wordList[MAX_CVAR_VALUE_STRING];
	char *nextWord;
	Q_strncpyz( wordList, g_censorNames.string, sizeof(g_censorNames.string) );
	censorNamesDictionary.num_words = 0;
	nextWord = strtok(wordList," ,");
	while (nextWord && censorNamesDictionary.num_words != DICT_MAX_WORDS) {
		Q_strncpyz( censorNamesDictionary.words[censorNamesDictionary.num_words], nextWord,
									sizeof(censorNamesDictionary.words[censorNamesDictionary.num_words]));
		nextWord = strtok(NULL," ,");
		if (!nextWord) {
			break;
		}
		censorNamesDictionary.num_words++;
	}
	censorNamesDictionary.num_words++;
}

// Michael
void InitSkillLevelStructure( skillType_t skillType )
{
	char newLevels[MAX_CVAR_VALUE_STRING];
	char * nextLevel;
	int levels[9];
	int count;

	switch( skillType )
	{
		case SK_HEAVY_WEAPONS:
			Q_strncpyz( newLevels, g_skillSoldier.string, 
						sizeof(g_skillSoldier.string) );
			break;
		case SK_FIRST_AID:
			Q_strncpyz( newLevels, g_skillMedic.string, 
						sizeof(g_skillMedic.string) );
			break;
		case SK_EXPLOSIVES_AND_CONSTRUCTION:
			Q_strncpyz( newLevels, g_skillEngineer.string, 
						sizeof(g_skillEngineer.string) );
			break;
		case SK_SIGNALS:
			Q_strncpyz( newLevels, g_skillFieldOps.string, 
						sizeof(g_skillFieldOps.string) );
			break;
		case SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS:
			Q_strncpyz( newLevels, g_skillCovertOps.string, 
						sizeof(g_skillCovertOps.string) );
			break;
		case SK_BATTLE_SENSE:
			Q_strncpyz( newLevels, g_skillBattleSense.string, 
						sizeof(g_skillBattleSense.string) );
			break;
		case SK_LIGHT_WEAPONS:
			Q_strncpyz( newLevels, g_skillLightWeapons.string, 
						sizeof(g_skillLightWeapons.string) );
			break;
		default: // Jaybird - clean warnings
			break;
	}


	nextLevel = strtok(newLevels," ");

	for (count = 0; count < 9 && nextLevel; count++) {
		levels[count]=atoi(nextLevel);
		nextLevel = strtok(NULL," ,");
	}

	// make sure we have 4 positive integers in ascending order, or its not valid
	if( !(count == 9 && levels[0] >= 0 &&
			levels[1] >= levels[0] &&
			levels[2] >= levels[1] &&
			levels[3] >= levels[2] &&
			levels[4] >= levels[3] &&
			levels[5] >= levels[4] &&
			levels[6] >= levels[5] &&
			levels[7] >= levels[6] &&
			levels[8] >= levels[7] ) )
	{
		return;
	}

	for (count = 1; count < NUM_SKILL_LEVELS; count++) {
		skillLevels[skillType][count]=levels[count-1];
	}
}



