/*
 * name:		nq_client_help.h
 *
 * desc:		Client help /nqhelp
 *
 * NQQS: TODO: check CLIENT_HELP_CAT_VIDEO and CLIENT_HELP_CAT_DRAW
 *
 */
#ifndef __NQ_CLIENT_HELP_H
#define __NQ_CLIENT_HELP_H

#include "nq_help.h"

// categories
#define CLIENT_HELP_CAT_NONE		0
#define CLIENT_HELP_CAT_VIDEO		0x00000001 // any 3D
#define CLIENT_HELP_CAT_AUDIO	    0x00000002
#define CLIENT_HELP_CAT_FILTER		0x00000004
#define CLIENT_HELP_CAT_TWEAK		0x00000008
#define CLIENT_HELP_CAT_WEAPON		0x00000010
#define CLIENT_HELP_CAT_HUD		    0x00000020
#define CLIENT_HELP_CAT_MOVEMENT	0x00000040
#define CLIENT_HELP_CAT_CHEAT		0x00000080
#define CLIENT_HELP_CAT_DRAW		0x00000100 // any 2D, see also HUD
#define CLIENT_HELP_CAT_CLIENT		0x00000200
#define CLIENT_HELP_CAT_FIRETEAM	0x00000400
#define CLIENT_HELP_CAT_NETWORK		0x00000800
#define CLIENT_HELP_CAT_DEMO		0x00001000 // demo recording, screenshots
#define CLIENT_HELP_CAT_CHAT 		0x00002000 // chats, pms & co
#define CLIENT_HELP_CAT_DEBUG 		0x00004000 // debug outputs
// TODO: most of crosshair stuff is also CLIENT_HELP_CAT_DRAW
#define CLIENT_HELP_CAT_CROSSHAIR 	0x00008000 // crosshair related cvars ...



#define CLIENT_HELP_CAT_DOCU		0x80000000	// do not change or remove this bitflag !..


helpCategory_t clientHelpCategories[] =
{
	{ "video",		CLIENT_HELP_CAT_VIDEO },
	{ "audio",		CLIENT_HELP_CAT_AUDIO },
	{ "filter",		CLIENT_HELP_CAT_FILTER },
	{ "tweak",		CLIENT_HELP_CAT_TWEAK },
	{ "weapon",		CLIENT_HELP_CAT_WEAPON },
	{ "hud",		CLIENT_HELP_CAT_HUD },
	{ "movement",	CLIENT_HELP_CAT_MOVEMENT },
	{ "cheat",		CLIENT_HELP_CAT_CHEAT },
	{ "draw",		CLIENT_HELP_CAT_DRAW },
	{ "client",		CLIENT_HELP_CAT_CLIENT },
	{ "fireteam",	CLIENT_HELP_CAT_FIRETEAM },
	{ "network",	CLIENT_HELP_CAT_NETWORK },
	{ "demo",		CLIENT_HELP_CAT_DEMO },
	{ "chat",		CLIENT_HELP_CAT_CHAT },
	{ "debug",		CLIENT_HELP_CAT_DEBUG },
	{ "crosshair",	CLIENT_HELP_CAT_CROSSHAIR },
	// must be last
	{ NULL, 0 }
};


// TODO:
// Keep in mind we have also engine cvars ... -> list is incomplete (mod side only)
// Add the renderer r_CVARS  (r_window etc)
// Check all integer types - some are bitmasks
// Cheat vars are only shown in debug mode

// Next one is cg_skillViewOptions (bottom-up) http://shitstorm.org/noquarter/wiki/index.php?title=Client_CVARs


// This page might help us too:
// http://tremulous.net/wiki/Category:Client_Side_Settings


helpText_t clientHelpTexts[] =
{
// engine cvars

{"com_hunkmegs",						"56",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: Integer",
										"This option determines how much memory is used for the client.",
										"Set this to a value of 128 (MB).",
										"",
										"Warning: The ET default value of 56 (MB) may reach the memory limit f.e. when fueldump is loaded.",
										"",
										"","","","","","","","","","","","","",""}},


{"com_soundmegs",						"24",
										CLIENT_HELP_CAT_CLIENT + CLIENT_HELP_CAT_AUDIO,
										{"Type: Integer",
										"This option determines how much memory is used for the client sound cache.",
										"Set this to a value of 160 (MB) to cache all sounds. See client cmd 's_list'.",
										"","","","","","","","","","","","","","","","",""}},

{"snaps",								"30",
										CLIENT_HELP_CAT_CLIENT + CLIENT_HELP_CAT_TWEAK,
										{"Type: Integer",
										"Sets the clients requests for snapshot every snapshotMsec unless rate choked. 1000/x",
										"The ET server engine restricts this value to 30.",
										"A value of 30 requests every 33.33msec",
										"A value of 20 requests every    50msec",
										"","","","","","","","","","","","","","",""}},

{"rate",								"3000",
										CLIENT_HELP_CAT_CLIENT + CLIENT_HELP_CAT_TWEAK,
										{"Type: Integer",
										"Rate controls how much data (in bytes) should be sent to/from each client every second.",
										"Higher values result in smother play, but setting it higher than your connection can handle will cause packet loss and ruin your game.",
										"The rate used by a specific client is the minimum of that clients rate, and the servers rate.",
										"The default of 3000 is designed for dial-up modems, with modern connections (f.e. DSL connections) the max of 25000 is recommended.",
										"",
										"LAN admins should have a look at sv_lanForceRate.",
										"","","","","","","","","","","","",""}},

// end engine cvars

{"cg_runpitch",							"0.002",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"Up-Down movement while running.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_runroll",							"0.005",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"Forward/Backward movement while running.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_bobup",							"0.005",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"Vertical motion of the body while moving.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_bobpitch",							"0.002",
										CLIENT_HELP_CAT_MOVEMENT + CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Float",
										"Crosshair vertical movement.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_bobroll",							"0.002",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"Rolling motion of the body while moving.",
										"","","","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_swingSpeed",						"0.1",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_shadows",							"1",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Toggles player shadows.",
										"0",
										"1",
										"2 Unused",
										"3","","","","","","","","","","","","","",""}},

{"cg_gibs",								"1",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Boolean",
										"Toggles the display of gib models.",
										"","","","","","","","","","","","","","","","","",""}},
// TODO
{"cg_draw2D",							"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toggles interface (HUD, chat, scoreboard, popups etc).",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawFPS",							"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draws frames per second (fps).",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawSnapshot",						"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draws snapshots.",
										"","","","","","","","","","","","","","","","","",""}},

// TODO: options ...
// check default value of cg_main (use colors?)
{"cg_drawCrosshair",					"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Integer",
										"Customizes crosshair type.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawCrosshairNames",				"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Bitmask",
										"Displays player info when the crosshair is over a teammate.",
										"As in ETMain, if greater than 0 it will draw the health bar and name of the player you're looking at. In No Quarter additional flags are available.",
										"Value 0: Disabled",
										"Bitflag 1: Enable Name Drawing (class icons instead of letter is hardcoded)",
										"Bitflag 2: Colored player names (flag 1 needs to be set)",
										"Bitflag 4: Rank icons instead of rank name",
										"","","","","","","","","","","","",""}},

{"cg_drawCrosshairPickups",				"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Integer",
										"Highlights items the player looks at.",
										"A value of 2 is force highlights.",
										"","","","","","","","","","","","","","","","",""}},

{"cg_cycleAllWeaps",					"1",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Cycling all weapons or just using next weapon bank.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_useWeapsForZoom",					"1",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Weapon switch will zoom in and out while scoped, rather than switch weapons",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairSize",					"48",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Integer",
										"Sets the size of the displayed crosshair.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairX",						"0",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Integer",
										"Horizontal position of the crosshair.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairY",						"0",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Integer",
										"Vertical position of the crosshair.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairHealth",					"0",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Boolean",
										"Colors the crosshair based on current health (overrides crosshair color settings).",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_teamChatsOnly",					"0",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Boolean",
										"Print team chats only.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_noVoiceChats",						"0",
										CLIENT_HELP_CAT_CHAT + CLIENT_HELP_CAT_AUDIO,
										{"Type: Boolean",
										"If set to 1, voice chat sounds will not be played.",
										"See also cg_noVoiceText",
										"","","","","","","","","","","","","","","","",""}},

{"cg_noVoiceText",						"0",
										CLIENT_HELP_CAT_CHAT + CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"If set to 1, voice chat text will not be printed.",
										"See also cg_noVoiceChats",
										"","","","","","","","","","","","","","","","",""}},

{"cg_drawStatus",						"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toogles status and timed menus.",
										"","","","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_animSpeed",						"1",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_drawSpreadScale",					"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_WEAPON,
										{"Type: Integer",
										"Draws a bar showing current weapon stability level (0-255).",
										"A value of 1 means only draw for scoped weapons",
										"A value of 2 means draw all the time (for debugging)",
										"","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_debugAnim",						"0",
										CLIENT_HELP_CAT_CHEAT + CLIENT_HELP_CAT_DEBUG,
										{"Type: Integer",
										"Debugs animations.",
										"1",
										"2",
										"3",
										"","","","","","","","","","","","","","",""}},

{"cg_debugPosition",					"0",
										CLIENT_HELP_CAT_CHEAT + CLIENT_HELP_CAT_DEBUG,
										{"Type: Boolean",
										"Shows player torso yaw in ResetPlayerEntity()",
										"","","","","","","","","","","","","","","","","",""}},


{"cg_debugEvents",						"0",
										CLIENT_HELP_CAT_CHEAT + CLIENT_HELP_CAT_DEBUG,
										{"Type: Boolean",
										"Debugs events",
										"","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_errorDecay",						"100",
										CLIENT_HELP_CAT_NONE,
										{"Type: Float",
										"Controls how fast a prediction error is decayed off to smooth out any jerkiness.",
										"A prediction error is, very basically, what occurs when the client and server disagree on the location you're in.",
										"Max error decay is 500. We do recommend not to change this cvar.",
										"","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_nopredict",						"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_noPlayerAnims",					"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif


#ifdef _DEBUG
// prediction info
{"cg_showmiss",							"0",
										CLIENT_HELP_CAT_DEBUG,
										{"Type: Bitflag",
										"Shows prediction infos.",
										"Value 0: Disabled",
										"Value 1: Enabled",
										"Value 2: Additional info of client time, numPredicted and numPlayedBack",
										"Value 4: Additional time info of server and client",
										"","","","","","","","","","","","","",""}},

{"cg_footsteps",						"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_markTime",							"20000",
										CLIENT_HELP_CAT_NONE,
										{"Type: Integer",
										"Wall Mark Lifetime.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_brassTime",						"2500",
										CLIENT_HELP_CAT_NONE,
										{"Type: Integer",
										"Ejecting brass time.",
										"","","","","","","","","","","","","","","","","",""}},

/* IRATA: removed
{"cg_letterbox",						"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: Boolean",
										"","","","","","","","","","","","","","","","","","",""}},
*/

{"cg_drawGun",							"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Toggles first person items (gun, knife, grenade etc).",
										"Value 0: No gun",
										"Value 1: Draw gun",
										"","","","","","","","","","","","","","","",""}},

// TODO: check this !
{"cg_cursorHints",						"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Integer",
										"Controls cursor hints.",
										"Value 0: No hints",
										"Value 1: sin size pulse",
										"Value 2: one way size pulse",
										"Value 3: alpha pulse",
										"Value 4+: static image",
										"","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_gunx",								"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
{"cg_guny",								"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
{"cg_gunz",								"0",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_gun_foreshorten",					"0",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Float",
										"Multiplies view matrix for weapon model.",
										"","","","","","","","","","","","","","","","","",""}},

//mcwf GeoIP
{"cg_countryflags",						"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toggles the country flags from the GeoIP Mod on the scoreboard (TAB).",
										"Note: This CVAR only affects the client, other players are still able to see your country flag when set to 0.",
										"","","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_tracerChance",						"0.4",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_tracerWidth",						"0.8",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_tracerLength",						"160",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_tracerSpeed",						"4500",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_autoswitch",						"2",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Integer",
										"Toggles automatic switching to a newly auto-picked up weapon.",
										"0 - off",
										"1 - always switch",
										"2 - if new",
										"3 - I better",
										"4 - new or better",
										"","","","","","","","","","","","",""}},

{"cg_fov",								"90",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Float",
										"Field of view in degrees.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_zoomStepSniper",					"2",
										CLIENT_HELP_CAT_WEAPON, // TODO: only weapon cat ?
										{"Type: Integer",
										"Customizes the number of zoom levels for sniper scopes.",
										"Value 2: 8 Levels of Zoom",
										"Value 4: 4 Levels of Zoom",
										"","","","","","","","","","","","","","","",""}},

{"cg_zoomDefaultSniper",				"20",
										CLIENT_HELP_CAT_WEAPON, // TODO: only weapon cat ?
										{"Type: Integer",
										"Customizes the default zoom level for sniper scopes.",
										"Value 4: All the way in",
										"Value 20: All the way out",
										"Note: You can use custom integers between these two values",
										"","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_thirdPerson",						"0",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Boolean",
										"Toggles Thirdperson-View",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_thirdPersonRange",					"80",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Integer",
										"Distance to the 3rdperson player model (in game units).","","","","","","","","","","","","","","","","","",""}},

{"cg_thirdPersonAngle",					"0",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Integer",
										"Angle at which to look at 3rdperson player model (in degrees).",
										"","","","","","","","","","","","","","","","","",""}},
#endif
// 3d
{"cg_stereoSeparation",					"0.4",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_lagometer",						"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toggles Lag-o-Meter view.",
										"","","","","","","","","","","","","","","","","",""}},

/*
#ifdef ALLOW_GSYNC
{"cg_synchronousClients;
*/

{"cg_teamChatTime",						"8000",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Integer",
										"Defines the time (in milliseconds) that chat texts are visible for.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_teamChatHeight",					"8",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Integer",
										"Defines the number of lines in chat.",
										"Note: You can only set a value less than the default value.",
										"","","","","","","","","","","","","","","","",""}},
/*
#ifdef _DEBUG
{"cg_stats; // displayes client frame number
*/

{"cg_buildScript",						"0",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: Boolean",
										"Force loading of all possible data and error on failures.",
										"See also com_buildScript.",
										"","","","","","","","","","","","","","","","",""}},


{"cg_coronafardist",					"1536",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Performance variable will keep down super long traces - see cg_coronas.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_coronas",							"1",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Controls coronas behaviour.",
										"Value 0: No coronas",
										"Value 1: Cornonas enabled",
										"Value 2: Trace everything",
										"","","","","","","","","","","","","","",""}},
/*

{"cg_paused",							"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},
*/

{"cg_predictItems",						"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_autoactivate",						"1",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: Boolean",
										"Toggles auto-pickup of healthpacks/ammopacks/weapons and ammo.",
										"","","","","","","","","","","","","","","","","",""}},

{"pmove_fixed",							"",
										CLIENT_HELP_CAT_MOVEMENT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"pmove_msec",							"",
										CLIENT_HELP_CAT_MOVEMENT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_wolfparticles",					"1",
										CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_TWEAK,
										{"Type: Boolean",
										"Toggles explosions effects.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_smokeparticles",					"1",
										CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_TWEAK,
										{"Type: Boolean",
										"Toggles Smoke. The familiar cg_wolfParticles has been broken up into several separate CVARs for more control.",
										"This CVAR disables smoke particles from map elements and gun barrels.",
										"This does not disable the Covert Ops smoke screen, landmine smoke or coming smoke from artillery/airstrike markers.",
										"","","","","","","","","","","","","","","",""}},

{"cg_impactparticles",					"1",
										CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_TWEAK,
										{"Type: Boolean",
										"This CVAR toogles impact particles, from shooting the ground and walls, like sparks, dirt, snow etc.",
										"The familiar cg_wolfParticles has been broken up into several separate CVARs for more control."
										,"","","","","","","","","","","","","","","","",""}},

{"cg_trailparticles",					"1",
										CLIENT_HELP_CAT_WEAPON + CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_TWEAK,
										{"Type: Boolean",
										"Toggles grenade, panzer and mortar trails.",
										"","","","","","","","","","","","","","","","","",""}},

// cg_gameType - communicated by systeminfo

{"cg_bloodTime",						"120",
										CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_TWEAK,
										{"Type: Integer",
										"Time to show blood on surfaces.",
										"","","","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_skybox",							"",
										CLIENT_HELP_CAT_CHEAT,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_message",							"1",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_messageType",						"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_messageTarget",					"",
										CLIENT_HELP_CAT_CHAT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_voiceSpriteTime",					"6000",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Integer",
										"The time (in milliseconds) that voice chat icons (above players' heads) stay visible for.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawCompass",						"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Integer",
										"Manages icon behaviour of compass-icon clipping.",
										"Value 0: Drawn no compass at all. ",
										"Value 1: Icons are drawn if they appear inside the compass. If their position is outside (the circle of) the compass then they will not be drawn (no matter what shader is used). ",
										"Value 2: Icons are drawn in the old way, a correct shader must filter the icons outside the compass. ",
										"","","","","","","","","","","","","","",""}},

{"cg_drawNotifyText",					"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toogles printing of [skipnotify].",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_quickMessageAlt",					"0",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Boolean",
										"Customizes Quick Chat Mode:",
										"Value 1: Use numeric keys (1-0) to send voice chat messages.",
										"Value 0: Use alpha keys (A-Z) to send voice chat messages.",
										"","","","","","","","","","","","","","","",""}},

{"cg_descriptiveText",					"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toggles descriptive text/shows spectator info.",
										"","","","","","","","","","","","","","","","","",""}},
/*

{"cg_redlimbotime",						"",
										CLIENT_HELP_CAT_NONE,
										{"Communicated by systeminfo",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_bluelimbotime",					"",
										CLIENT_HELP_CAT_NONE,
										{"Communicated by systeminfo",
										"","","","","","","","","","","","","","","","","","",""}},


{"cg_antilag",							"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},
*/

#ifdef _DEBUG
{"developer",							"0",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Boolean",
										"Toggles developer functions.",
										"","","","","","","","","","","","","","","","","",""}},
#endif

{"authLevel",							"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: Read-Only Boolean",
										"Value 0: Normal player status.",
										"Value 1: Referee status.",
										"","","","","","","","","","","","","","","","",""}},

{"cf_wstats",							"1.2",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Float",
										"Sets font scale of stats window (+wstats).",
										"","","","","","","","","","","","","","","","","",""}},


{"cf_wtopshots",						"1.0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Float",
										"Sets font scale of topshots window (+wtopshots).",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_skillViewOptions",					"31",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Bitflag",
										"Specify filter for several skills (only visible skills).",
										"Value 0:    Disable all",
										"Bitflag 1:  Enable view medic skill 9",
										"Bitflag 2:  Enable view signal 6",
										"Bitflag 4:  Enable view battle sense 7",
										"Bitflag 8:  Enable view battle sense 8",
										"Bitflag 16: Enable view battle sense 9",
										"","","","","","","","","","","",""}},

{"cg_announcer",						"1",
										CLIENT_HELP_CAT_NONE,
										{"Type: Bitflag",
										"Controls the various announcements available on the server.",
										"Value 0:    Disable all announcements",
										"Bitflag 1:  Enable announcements",
										"Bitflag 2:  Disables Killingspree/Multikill/Firstblood/Firstheadshot announcements",
										"Bitflag 4:  Only announce kills made by yourself (or the one you're observing)",
										"Bitflag 8:  Disable textual centerprint messages",
										"Bitflag 16: Disable voice messages (silent mode)",
										"","","","","","","","","","","",""}},

{"cg_autoAction",						"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: Bitflag",
										"Enables auto actions.",
										"Value 0:	 Do nothing",
										"Bitflag 1:  Auto record a demo",
										"Bitflag 2:  Auto record a screenshot at end of game",
										"Bitflag 4:  Does a stats dump at end of round",
										"","","","","","","","","","","","","",""}},

{"cg_autoReload",						"1",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Toggles auto-reload if clip empty.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_weapAltReloads",					"1",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Whether +attack2 will reload weapons that dont have alternate fire.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_bloodDamageBlend",					"1.0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Float",
										"Blood \"splatter\" transparency when taking damage from other players.",
										"Value 0: Invisible.",
										"Value 1: Max visible.",
										"","","","","","","","","","","","","","","",""}},

{"cg_bloodFlash",						"1.0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Float",
										"Red blood \"flash\" scale when receiving damage.",
										"Value 0: Invisible.",
										"Value 1: Max visible.",
										"","","","","","","","","","","","","","","",""}},

{"cg_complaintPopUp",					"1",
										CLIENT_HELP_CAT_NONE,
										{"Type: Boolean",
										"Display the option to file complaints about teammates who killed you.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairAlpha",					"1.0",
										CLIENT_HELP_CAT_CROSSHAIR + CLIENT_HELP_CAT_DRAW,
										{"Type: Float",
										"Customizes the transparency level of the primary crosshair.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairAlphaAlt",				"1.0",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Float",
										"Customizes the transparency level of the secondary crosshair.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_crosshairColor",					"White",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: String",
										"Customizes the color of the primary crosshair.",
										"The following colors are also the corresponding value names:",
										"White, Yellow, Red, Green, Blue, Magenta, Cyan, Orange and Black.",
										"More colors:",
										"0xa0c0ff: Light Blue.",
										"mdblue:   Medium Blue.",
										"0xffc0a0: Light Red.",
										"mdred:    Medium Red.",
										"0xa0ffc0: Light Green.",
										"mdgreen:  Medium Green.",
										"dkgreen:  Dark Green.",
										"mdcyan:   Medium Cyan.",
										"mdyellow: Medium Yellow.",
										"mdorange: Medium Orange.",
										"ltgrey:   Light Grey.",
										"mdgrey:   Medium Grey.",
										"dkgrey:   Dark Grey.",
										"",""}},

{"cg_crosshairColorAlt",				"White",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: String",
										"Customizes the color of the secondary crosshair.",
										"The following colors are also the corresponding value names:",
										"White, Yellow, Red, Green, Blue, Magenta, Cyan, Orange and Black.",
										"More colors:",
										"0xa0c0ff: Light Blue.",
										"mdblue:   Medium Blue.",
										"0xffc0a0: Light Red.",
										"mdred:    Medium Red.",
										"0xa0ffc0: Light Green.",
										"mdgreen:  Medium Green.",
										"dkgreen:  Dark Green.",
										"mdcyan:   Medium Cyan.",
										"mdyellow: Medium Yellow.",
										"mdorange: Medium Orange.",
										"ltgrey:   Light Grey.",
										"mdgrey:   Medium Grey.",
										"dkgrey:   Dark Grey.",
										"",""}},

{"cg_crosshairPulse",					"1",
										CLIENT_HELP_CAT_CROSSHAIR,
										{"Type: Boolean",
										"Toggles crosshair spread sizing while moving/firing.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawReinforcementTime",			"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Displays your team's reinforcement timer.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawWeaponIconFlash",				"0",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Flash weapon icon when firing, reloading or switching.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_noAmmoAutoSwitch",					"1",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Integer",
										"Customizes auto-switch to a new weapon when out of ammunition.",
										"Value 0: No Switching.",
										"Value 1: Switch when out of Ammo.",
										"Value 2: No autoswitch to pistols after shooting panzerfaust/bazooka.",
										"","","","","","","","","","","","","","",""}},

{"cg_printObjectiveInfo",				"1",
										CLIENT_HELP_CAT_NONE,
										{"Type: Boolean",
										"Prints important game messages to the console.",
										"","","","","","","","","","","","","","","","","",""}},

#ifdef MV_SUPPORT
{"cg_specHelp",							"1",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},
#endif

{"cg_uinfo",							"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: String",
										"Example value: \"108 0 100\".",
										"[cg_autoreload / cg_autoactivate / cg_predictitems setting] [cl_timenudge setting] [cl_maxpackets setting]",
										"","","","","","","","","","","","","","","","",""}},

{"cg_useScreenshotJPEG",				"1",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Boolean",
										"Switches between .tga and .jpg picture types for the \"autoscreenshot\" command.",
										"Value 0: Use TGA",
										"Value 1: Use JPG",
										"","","","","","","","","","","","","","","",""}},

{"demo_avifpsF1",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"demo_avifpsF2",						"10",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"demo_avifpsF3",						"15",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"demo_avifpsF4",						"20",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"demo_avifpsF5",						"24",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"demo_drawTimeScale",					"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Toggles Demo Timescale.",
										"","","","","","","","","","","","","","","","","",""}},

{"demo_infoWindow",						"1",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Boolean",
										"Toggles Demo Controls.",
										"","","","","","","","","","","","","","","","","",""}},

{"timescale",							"1",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"ui_blackout",							"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"rconPassword",						"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"Set this to the server's rcon password to be able to send commands",
										"to the server via /rcon <cmd> instead of /rcon <pw> <cmd>.",
										"See also rconAddress.",
										"","","","","","","","","","","","","","","",""}},

{"refereePassword",						"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"Set this to the server's referee password to be able to send referee commands.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_atmosphericEffects",				"1",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Boolean",
										"Toggles weather effects like rain and snow.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawRoundTimer",					"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Shows mission time left.",
										"","","","","","","","","","","","","","","","","",""}},

// TODO: delete? we do a instant tapout anyway if not LMS ... but if this cvar is set and LMS a tapout is done !!!
{"cg_instanttapout",					"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: Boolean",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_debugSkills",						"0",
										CLIENT_HELP_CAT_DEBUG,
										{"Type: Boolean",
										"Draws stats debug infos.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawFireteamOverlay",				"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_FIRETEAM,
										{"Type: Boolean",
										"Toggles fireteam window.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawSmallPopupIcons",				"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draw small icons on announcements in echo area instead of large ones.",
										"","","","","","","","","","","","","","","","","",""}},

// TODO: value 0 ?
{"cg_smallFont",						"8",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Bitmask",
										"Customizes the font size of announcements.",
										"Bitflag 1:  Use small font for chats.",
										"Bitflag 2:  Use small font for popups.",
										"Bitflag 4:  Use small font for mission announcements.",
										"Bitflag 16: Use small font for limbo.",
										"","","","","","","","","","","","","",""}},

//bani - demo recording cvars
{"cl_demorecording",					"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only Boolean",
										"This cvar is set by the engine whenever a demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_demofilename",						"",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only String",
										"This cvar is set by the engine whenever a demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_demooffset",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only Integer",
										"This cvar is set by the engine whenever a demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

//bani - wav recording cvars
{"cl_waverecording",					"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only Boolean",
										"This cvar is set by the engine whenever a wave demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_wavefilename",						"",
										CLIENT_HELP_CAT_AUDIO + CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only Boolean",
										"This cvar is set by the engine whenever a wave demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_waveoffset",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Read-Only Boolean",
										"This cvar is set by the engine whenever a wave demo is recorded.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_recording_statusline",				"9",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Integer",
										"Changes position of the \"RECORDING demo xy\" message (in pixels from the top).",
										"Set to 0 to hide.",
										"","","","","","","","","","","","","","","","",""}},

{"cg_HUDBackgroundColor",				".16 .2 .17",
										CLIENT_HELP_CAT_HUD,
										{"Type: String",
										"Changes the HUD background color.",
										"Three values must be set: [red] [green] [blue]",
										"See also cg_HUDBorderColor.",
										"","","","","","","","","","","","","","","",""}},

{"cg_HUDBorderColor",					".5 .5 .5",
										CLIENT_HELP_CAT_HUD,
										{"Type: String",
										"Changes the HUD border color.",
										"Three values must be set: [red] [green] [blue]",
										"See also cg_HUDBackgroundColor.",
										"","","","","","","","","","","","","","","",""}},

{"cg_HUDAlpha",							"0.8",
										CLIENT_HELP_CAT_HUD,
										{"Type: Float",
										"Changes the alpha of the HUD Border and Background.",
										"Valid values range from 0-1.",
										"","","","","","","","","","","","","","","","",""}},

{"cg_goatSound",						"3",
										CLIENT_HELP_CAT_AUDIO,
										{"Type: Bitmask",
										"Customizes when knife goat sound is played:",
										"Bitflag 1: Play For Frontal Stabs.",
										"Bitflag 2: Play For Backstabs.",
										"Bitflag 4: Play For Throwing.",
										"","","","","","","","","","","","","","",""}},

{"cg_hitSounds",						"",
										CLIENT_HELP_CAT_AUDIO,
										{"Type: Bitmask",
										"Customizes hitsounds:",
										"Value 0:   Disable hitsounds",
										"Bitflag 1: Enable Hitsounds.",
										"Bitflag 2: Do Not Play Team Hitsounds.",
										"Bitflag 4: Do Not Play Head Hitsounds.",
										"Bitflag 8: Do Not Play Body Hitsounds.",
										"","","","","","","","","","","","",""}},

{"cg_drawHUDHead",						"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_HUD,
										{"Type: Boolean",
										"Draw player head in HUD.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawObjIcons",						"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draw objective icons in HUD.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawPing",							"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draws ping in HUD.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawKillSpree",					"2",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Bitmask",
										"This will change draw a counter to track total kills or deaths, current spree, and your current bounty below the game timer.",
										"Value 0: Disabled",
										"Bitflag 1: Show current spree",
										"Bitflag 2: Show total kills",
										"Bitflag 3: Show current bounty",
										"Bitflag 4: Show total deaths",
										"","","","","","","","","","","","",""}},

{"cg_drawTime",							"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Integer",
										"Draws a clock.",
										"Value 0: Disabled",
										"Value 1: Draws 24h mode",
										"Value 2: Draws 12h mode",
										"","","","","","","","","","","","","","",""}},

{"cg_drawTimeSeconds",					"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"If set and cg_drawTime is enabled seconds are drawn.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_graphicObituaries",				"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draws graphical obituaries.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawCarryWeapons",					"0",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Draws a list with all carried weapons.",
										"","","","","","","","","","","","","","","","","",""}},

// iconSize in G_DrawActivePowerups()
{"hud_powerupSize",						"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_skillBarX",						"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_skillBarY",						"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_skillBarAlpha",					"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_rankX",							"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_rankY",							"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_rankAlpha",						"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_drawPowerups",					"",
										CLIENT_HELP_CAT_HUD,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"hud_drawAltHUD",						"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_HUD,
										{"Type: Integer",
										"Sets the HUD style.",
										"Value 0: Vanilla style",
										"Value 1: NQ style",
										"","","","","","","","","","","","","","","",""}},

{"cg_drawMuzzleFlash",					"1",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Bitmask",
										"Muzzle flesh draw options.",
										"Value 0: Disabled",
										"Bitmask 1: Draw muzzle flesh & dlight",
										"Bitmask 2: Draw smoke",
										"","","","","","","","","","","","","","",""}},

{"cg_drawTracers",						"1",
										CLIENT_HELP_CAT_VIDEO + CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Toogles bullet tracers.",
										"","","","","","","","","","","","","","","","","",""}},

#ifdef _DEBUG
{"cg_drawHitbox",						"0",
										CLIENT_HELP_CAT_CHEAT,
										{"Type: Bitmask",
										"",
										"Value 0: Disabled",
										"1",
										"2",
										"4",
										"","","","","","","","","","","","","",""}},
#endif

// used - but only once !!! TODO:  check deletion
{"cg_markDistance",						"384",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Integer",
										"Impacts beyond this range won't leave bullet marks.",
										"","","","","","","","","","","","","","","","","",""}},

// forty - speedometer
{"cg_drawspeed",						"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Draws speed info of player.",
										"","","","","","","","","","","","","","","","","",""}},

// forty - speedometer
{"cg_speedunit",						"0",
										CLIENT_HELP_CAT_DRAW,
										{"Type: Integer",
										"Changes the way cg_drawSpeed is displayed.",
										"Value 0: Display in Units Per Second (UPS)",
										"Value 1: Display in Kilometers Per Hour (KPH)",
										"Value 2: Display in Miles Per Hour (MPH)",
										"","","","","","","","","","","","","","",""}},

// TODO: cat & test
{"cg_optimizePrediction",				"1",
										CLIENT_HELP_CAT_NONE,
										{"Type: Boolean",
										"Unlagged - optimized prediction",
										"With this method, we get incremental predicts on every client frame",
										"except a frame following a new snapshot in which there was a prediction error.",
										"This yeilds anywhere from a 15% to 40% performance increase,",
										"depending on how much of a bottleneck the CPU is.",
										"","","","","","","","","","","","","",""}},

{"cg_locations",						"3",
										CLIENT_HELP_CAT_NONE + CLIENT_HELP_CAT_FIRETEAM,
										{"Type: Integer",
										"Controls behaviour of location views.",
										"Value 0: Coordinates only",
										"Value 1: Names Fire-Team only",
										"Value 2: Names Team-Chat only",
										"Value 3: Team-Chat & Fire-Team",
										"","","","","","","","","","","","","",""}},


{"cg_fixedFTeamSize",					"1",
										CLIENT_HELP_CAT_DRAW + CLIENT_HELP_CAT_FIRETEAM,
										{"Type: Boolean",
										"Fixed or dynamic fire team rectangle.",
										"","","","","","","","","","","","","","","","","",""}},
/* here */
{"cg_popupMessageFilter",				"0",
										CLIENT_HELP_CAT_NONE,
										{"Type: Bitmask",
										"Specify filter for the pop-up messages in the left notification area (CPM).",
										"Value 0: 	  Filter disabled",
										"Bitflag 1:   Filter out objective messages (planted at ..., capture flags, etc.)",
										"Bitflag 2:	  Filter out pick-up messages (Picked up a health/ammo packs, weapons)",
										"Bitflag 4:	  Filter out team joining messages",
										"Bitflag 8:	  Filter out player connect/disconnect messages",
										"Bitflag 16:  Filter out all map script messages (command post constructed, tank damaged, etc.)",
										"Bitflag 32:  Filter out all obituaries caused by the enemy team",
										"Bitflag 64:  Filter out all obituaries caused by your own team",
										"Bitflag 128: Filter out all obituaries for self kills (of all players)",
										"Bitflag 256: Do NOT filter out death/kill messages",
										"","","","","","","",""}},

// Play Weapon Bounce Sound
{"cg_weaponBounceSound",				"1",
										CLIENT_HELP_CAT_AUDIO + CLIENT_HELP_CAT_WEAPON,
										{"Type: Boolean",
										"Toggles the sound for weapons and clips as they hit the ground. ",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_altHQVoice",						"0",
										CLIENT_HELP_CAT_AUDIO,
										{"Type: Boolean",
										"Play alternate commander voice.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_rtcwShake",						"0",
										CLIENT_HELP_CAT_TWEAK,
										{"Type: Boolean",
										"Changes your perception of nearby explosions and hits ",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_limbo_secondary",					"2",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Integer",
										"Preselection for the secondary limbo weapon.",
										"Value 0: Single",
										"Value 1: Dual",
										"Value 2: SMG (always best secondary)",
										"","","","","","","","","","","","","","",""}},

// TODO: check removal ...
{"cg_modelHilights",					"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cg_FTAutoSelect",						"1",
										CLIENT_HELP_CAT_FIRETEAM,
										{"Type: Boolean",
										"Automatically Select Buddies In A Fireteam",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_drawAuraIcons",					"1",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Boolean",
										"Toggles the swirling icons of the aura abilities on the ground, beneath the feet of each player. ",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_automapZoom",						"5.159",
										CLIENT_HELP_CAT_HUD,
										{"Type: Float",
										"Adjusts the zoom level of the HUD compass. Lowering this value means zooming out.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_shoveSounds",						"1",
										CLIENT_HELP_CAT_AUDIO,
										{"Type: Boolean",
										"Toogles shove sound.",
										"","","","","","","","","","","","","","","","","",""}},

{"cg_logFile",							"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"Sets the name of the chat log file saved in the client's 'nq' folder.",
										"When nothing is set (default), chat log is disabled.",
										"","","","","","","","","","","","","","","","",""}},

{"cg_pmWaitTime",						"2",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_pmFadeTime",						"2.5",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"cg_pmPopupTime",						"1",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Float",
										"Sets the amount of seconds between pop-up messages (obituaries and events) in the left notification area (CPM).",
										"Set to 0 for instant messages.",
										"","","","","","","","","","","","","","","","",""}},

{"cg_pmBigPopupTime",					"3.5",
										CLIENT_HELP_CAT_CHAT,
										{"Type: Float",
										"Sets the amount of seconds between big pop-up messages.",
										"Set to 0 for instant messages.",
										"","","","","","","","","","","","","","","","",""}},
// TODO: options
{"cg_spectator",						"3",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Bitmask",
										"Sets flags for various spectator options. If the server has it enabled, you can choose to use it",
										"Bitflag 1: Players will have a team coloured glow around them (red=Axis, blue=Allies).",
										"Bitflag 2: Players will have their names shown above them. ",
										"","","","","","","","","","","","","","","",""}},

// bullets flying by (integer part = distance, fraction part = trace.fraction)
{"cg_whizzSounds",						"1",
										CLIENT_HELP_CAT_AUDIO,
										{"Type: Integer",
										"Enable or disable bullet whizz sounds, produced by passing bullets whenever a player fires at you.",
										"Value 0: Disable all bullet whizz sounds",
										"Value 1: Disable bullet whizz sounds for MG's (+ Venom)",
										"Value 2: Enable bullet whizz sounds for all weapons that hold bullets",
										"","","","","","","","","","","","","","",""}},


{"cg_favWeaponBank",					"0",
										CLIENT_HELP_CAT_WEAPON,
										{"Type: Integer",
										"Sets the favourite weapon bank after using panzer/airstrike/satchel/smoke bomb",
										"Value 0: Previously used weapon",
										"Value 1: Knife",
										"Value 2: Pistol",
										"Value 3: SMG",
										"Value 4: Grenade",
										"","","","","","","","","","","","",""}},

// spawntimer from etpub
{"cg_spawnTimer_set",					"-1",
										CLIENT_HELP_CAT_TWEAK,
										{"","","","","","","","","","","","","","","","","","","",""}},
// spawntimer from etpub
{"cg_spawnTimer_period",				"0",
										CLIENT_HELP_CAT_TWEAK,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_wavefilerecord",					"0",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_nodelta",							"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_debugMove",						"0",
										CLIENT_HELP_CAT_DEBUG,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_noprint",							"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_motd",								"1",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

// never used ...
{"cl_autoupdate",						"1",
										CLIENT_HELP_CAT_CLIENT,
										{"",
										"This CVAR is never used.",
										"","","","","","","","","","","","","","","","","",""}},

{"rconAddress",							"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_timeout",							"200",
										CLIENT_HELP_CAT_NETWORK,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_maxpackets",						"30",
										CLIENT_HELP_CAT_NETWORK,
										{"","","","","","","","","","","","","","","","","","","",""}},
// TODO
{"cl_packetdup",						"1",
										CLIENT_HELP_CAT_NETWORK,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_timeNudge",						"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_showTimeDelta",					"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_freezeDemo",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

// TODO
{"cl_shownet",							"0",
										CLIENT_HELP_CAT_NETWORK  + CLIENT_HELP_CAT_DEBUG,
										{"Type: Integer",
										"",
										"0 Disabled",
										"1 Current message size",
										"2",
										"3",
										"4",
										"","","","","","","","","","","","",""}},

{"cl_shownuments",						"0",
										CLIENT_HELP_CAT_NETWORK + CLIENT_HELP_CAT_DEBUG,
										{"Type: Boolean",
										"Shows number of entities in packets.",
										"","","","","","","","","","","","","","","","","",""}},

// IRATA: Actually unused in the engine code
/*
{"cl_visibleClients",					"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},
*/

{"cl_showSend",							"0",
										CLIENT_HELP_CAT_NETWORK + CLIENT_HELP_CAT_DEBUG,
										{"Type: Boolean",
										"Shows sent packet/fragment infos",
										"","","","","","","","","","","","","","","","","",""}},

// IRATA: I don't see any output if set
{"cl_showServerCommands",				"0",
										CLIENT_HELP_CAT_NETWORK,
										{"Type: Boolean",
										"","","","","","","","","","","","","","","","","","",""}},

{"cl_avidemo",							"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Boolean",
										"","","","","","","","","","","","","","","","","","",""}},

{"cl_forceavidemo",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_freelook",							"1",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"sensitivity",							"5",
										CLIENT_HELP_CAT_NONE,
										{"Type: Float",
										"Sets the mouse snsitivity.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_mouseAccel",						"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_showMouseRate",					"0",
										CLIENT_HELP_CAT_DEBUG + CLIENT_HELP_CAT_DRAW,
										{"Type: Boolean",
										"Shows debug output rate of mouse.",
										"","","","","","","","","","","","","","","","","",""}},

{"m_pitch",								"0.022",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"m_yaw",								"0.022",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"m_forward",							"0.25",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"m_side",								"0.25",
										CLIENT_HELP_CAT_MOVEMENT,
										{"Type: Float",
										"","","","","","","","","","","","","","","","","","",""}},

{"m_filter",							"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_autorecord",						"0",
										CLIENT_HELP_CAT_DEMO,
										{"Type: Boolean",
										"Enabling it will start a demo recording when a client joins a server (or starts a local one),",
										"will stop record on map change, and create a new one.",
										"","","","","","","","","","","","","","","","",""}},

{"cl_motdString",						"",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_allowDownload",					"1",
										CLIENT_HELP_CAT_CLIENT + CLIENT_HELP_CAT_NETWORK,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_wwwDownload",						"1",
										CLIENT_HELP_CAT_CLIENT + CLIENT_HELP_CAT_NETWORK,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_conXOffset",						"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_serverStatusResendTime",			"750",
										CLIENT_HELP_CAT_CLIENT,
										{"","","","","","","","","","","","","","","","","","","",""}},

{"cl_waitForFire",						"0",
										CLIENT_HELP_CAT_NONE,
										{"",
										"This var is never used.",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_language",							"0",
										CLIENT_HELP_CAT_NONE,
										{"obsolete?",
										"","","","","","","","","","","","","","","","","","",""}},
// TODO
{"cl_debugTranslation",					"0",
										CLIENT_HELP_CAT_DEBUG,
										{"Type: Integer",
										"Sets debug options for translations",
										"Value 0: Disabled",
										"Value 1: ",
										"Value 2: ",
										"Value 3: ",
										"","","","","","","","","","","","","",""}},

/**
// CVAR_ROM - can not be set by user directly
{"cl_updateavailable",					"0",
										CLIENT_HELP_CAT_NONE,
										{"","","","","","","","","","","","","","","","","","","",""}},


{"cl_updatefiles",						"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"","","","","","","","","","","","","","","","","","",""}},


// CVAR_ROM - can not be set by user directly
{"cl_profile",							"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"Sets the current profile.",
										"","","","","","","","","","","","","","","","","",""}},


// CVAR_ROM - can not be set by user directly
{"cl_defaultProfile",					"",
										CLIENT_HELP_CAT_CLIENT,
										{"Type: String",
										"Sets the default profile.","","","","","","","","","","","","","","","","","",""}},
*/

#ifdef _DEBUG
{"cl_packetloss",						"0",
										CLIENT_HELP_CAT_NETWORK + CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Integer",
										"Mimic packet loss",
										"","","","","","","","","","","","","","","","","",""}},

{"cl_packetdelay",						"0",
										CLIENT_HELP_CAT_NETWORK + CLIENT_HELP_CAT_CHEAT,
										{"Type: Cheat-Protected Integer",
										"Artificially sets the client's latency.","","","","","","","","","","","","","","","","","",""}},
#endif

// renderer cvars
{"r_mode",								"4",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Sets video mode",
										"Show available modes with /modelist",
										"","","","","","","","","","","","","","","","",""}},
										
{"r_customheight",						"1024",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Custom height in pixels",
										"NOTE: r_mode must be set to -1 for this to work",
										"","","","","","","","","","","","","","","","",""}},
										
{"r_customwidth",						"1600",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Custom width in pixels",
										"NOTE: r_mode must be set to -1 for this to work",
										"","","","","","","","","","","","","","","","",""}},
										
{"r_depthbits",							"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Depth buffer:",
										"Value 16: 16-bit",
										"Value 24: 24-bit",
										"","","","","","","","","","","","","","","",""}},
										
{"r_stencilbits",						"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"This is a hardware dependent graphical setting which creates more realistic textures.",
										"Be sure to set this to 8 if your graphics card has a hardware stencil buffer.",
										"Be aware that the screen may darken with this enabled, so make sure to alter gamma accordingly.",
										"","","","","","","","","","","","","","","",""}},
										
{"r_smp",								"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"If you have two CPUs, Windows NT or 2000, and a graphics card AND driver that support SMP,",
										"you can benefit from a more sustained framerate which might also be noticeably higher.",
										"Set to 1 if your system fulfils the above requirements.",
										"","","","","","","","","","","","","","","",""}},
										
{"r_colorbits",							"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Color depth:",
										"Value  0: Desktop default",
										"Value 16: 16-bit",
										"Value 32: 32-bit",
										"","","","","","","","","","","","","","",""}},
										
{"r_texturebits",						"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Integer",
										"Texture quality:",
										"Value  0: Default",
										"Value 16: 16-bit",
										"Value 32: 32-bit",
										"","","","","","","","","","","","","","",""}},
										
/*{"r_clampToEdge",						"0",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: Boolean",
										"Setting it to 0 removes the fog using ATI video cards,",
										"improving long range visibility.",
										"","","","","","","","","","","","","","","","",""}},
										
{"",						"",
										CLIENT_HELP_CAT_VIDEO,
										{"Type: ",
										"","","","","","","","","","","","","","","","","","",""}},*/
};



/*

From void R_Register( void )

	// latched and archived variables
	//
	r_glDriver = ri.Cvar_Get( "r_glDriver", OPENGL_DRIVER_NAME, CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_allowExtensions = ri.Cvar_Get( "r_allowExtensions", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_ext_compressed_textures = ri.Cvar_Get( "r_ext_compressed_textures", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE ); // (SA) ew, a spelling change I missed from the missionpack
	r_ext_gamma_control = ri.Cvar_Get( "r_ext_gamma_control", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_ext_multitexture = ri.Cvar_Get( "r_ext_multitexture", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_ext_compiled_vertex_array = ri.Cvar_Get( "r_ext_compiled_vertex_array", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_glIgnoreWicked3D = ri.Cvar_Get( "r_glIgnoreWicked3D", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );

//----(SA)	added
	r_ext_ATI_pntriangles           = ri.Cvar_Get( "r_ext_ATI_pntriangles", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE ); //----(SA)	default to '0'
	r_ati_truform_tess              = ri.Cvar_Get( "r_ati_truform_tess", "1", CVAR_ARCHIVE | CVAR_UNSAFE );
	r_ati_truform_normalmode        = ri.Cvar_Get( "r_ati_truform_normalmode", "GL_PN_TRIANGLES_NORMAL_MODE_LINEAR", CVAR_ARCHIVE | CVAR_UNSAFE );
	r_ati_truform_pointmode         = ri.Cvar_Get( "r_ati_truform_pointmode", "GL_PN_TRIANGLES_POINT_MODE_LINEAR", CVAR_ARCHIVE | CVAR_UNSAFE );

	r_ati_fsaa_samples              = ri.Cvar_Get( "r_ati_fsaa_samples", "1", CVAR_ARCHIVE | CVAR_UNSAFE );        //DAJ valids are 1, 2, 4

	r_ext_texture_filter_anisotropic    = ri.Cvar_Get( "r_ext_texture_filter_anisotropic", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );

	r_ext_NV_fog_dist                   = ri.Cvar_Get( "r_ext_NV_fog_dist", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_nv_fogdist_mode                   = ri.Cvar_Get( "r_nv_fogdist_mode", "GL_EYE_RADIAL_NV", CVAR_ARCHIVE | CVAR_UNSAFE );  // default to 'looking good'
//----(SA)	end

#ifdef __linux__ // broken on linux
	r_ext_texture_env_add = ri.Cvar_Get( "r_ext_texture_env_add", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#else
	r_ext_texture_env_add = ri.Cvar_Get( "r_ext_texture_env_add", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#endif

	r_clampToEdge = ri.Cvar_Get( "r_clampToEdge", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE ); // ydnar: opengl 1.2 GL_CLAMP_TO_EDGE support

	r_picmip = ri.Cvar_Get( "r_picmip", "1", CVAR_ARCHIVE | CVAR_LATCH ); //----(SA)	mod for DM and DK for id build.  was "1" // JPW NERVE pushed back to 1
	r_roundImagesDown = ri.Cvar_Get( "r_roundImagesDown", "1", CVAR_ARCHIVE | CVAR_LATCH );
	r_rmse = ri.Cvar_Get( "r_rmse", "0.0", CVAR_ARCHIVE | CVAR_LATCH );
	r_colorMipLevels = ri.Cvar_Get( "r_colorMipLevels", "0", CVAR_LATCH );
	AssertCvarRange( r_picmip, 0, 3, qtrue );
	r_detailTextures = ri.Cvar_Get( "r_detailtextures", "1", CVAR_ARCHIVE | CVAR_LATCH );
	r_texturebits = ri.Cvar_Get( "r_texturebits", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_colorbits = ri.Cvar_Get( "r_colorbits", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_stereo = ri.Cvar_Get( "r_stereo", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#ifdef __linux__
	r_stencilbits = ri.Cvar_Get( "r_stencilbits", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#else
	r_stencilbits = ri.Cvar_Get( "r_stencilbits", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#endif
	r_depthbits = ri.Cvar_Get( "r_depthbits", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_overBrightBits = ri.Cvar_Get( "r_overBrightBits", "0", CVAR_ARCHIVE | CVAR_LATCH ); // Arnout: disable overbrightbits by default
	AssertCvarRange( r_overBrightBits, 0, 1, qtrue );                                   // ydnar: limit to overbrightbits 1 (sorry 1337 players)
	r_ignorehwgamma = ri.Cvar_Get( "r_ignorehwgamma", "0", CVAR_ARCHIVE | CVAR_LATCH );        // ydnar: use hw gamma by default
	r_mode = ri.Cvar_Get( "r_mode", "4", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
	r_oldMode = ri.Cvar_Get( "r_oldMode", "", CVAR_ARCHIVE );                             // ydnar: previous "good" video mode
	r_fullscreen = ri.Cvar_Get( "r_fullscreen", "1", CVAR_ARCHIVE | CVAR_LATCH );
	r_customwidth = ri.Cvar_Get( "r_customwidth", "1600", CVAR_ARCHIVE | CVAR_LATCH );
	r_customheight = ri.Cvar_Get( "r_customheight", "1024", CVAR_ARCHIVE | CVAR_LATCH );
	r_customaspect = ri.Cvar_Get( "r_customaspect", "1", CVAR_ARCHIVE | CVAR_LATCH );
	r_simpleMipMaps = ri.Cvar_Get( "r_simpleMipMaps", "1", CVAR_ARCHIVE | CVAR_LATCH );
	r_uiFullScreen = ri.Cvar_Get( "r_uifullscreen", "0", 0 );
	r_subdivisions = ri.Cvar_Get( "r_subdivisions", "4", CVAR_ARCHIVE | CVAR_LATCH );
#ifdef MACOS_X
	// Default to using SMP on Mac OS X if we have multiple processors
	r_smp = ri.Cvar_Get( "r_smp", Sys_ProcessorCount() > 1 ? "1" : "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#elif defined WIN32
	// ydnar: r_smp is nonfunctional on windows
	r_smp = ri.Cvar_Get( "r_smp", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE | CVAR_ROM );
	Cvar_Set( "r_smp", "0" );
#else
	r_smp = ri.Cvar_Get( "r_smp", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_UNSAFE );
#endif
	r_ignoreFastPath = ri.Cvar_Get( "r_ignoreFastPath", "0", CVAR_ARCHIVE | CVAR_LATCH ); // ydnar: use fast path by default
#if MAC_STVEF_HM || MAC_WOLF2_MP
	r_ati_fsaa_samples              = ri.Cvar_Get( "r_ati_fsaa_samples", "1", CVAR_ARCHIVE );       //DAJ valids are 1, 2, 4
#endif
	//
	// temporary latched variables that can only change over a restart
	//
	r_displayRefresh = ri.Cvar_Get( "r_displayRefresh", "0", CVAR_LATCH | CVAR_UNSAFE );
	AssertCvarRange( r_displayRefresh, 0, 200, qtrue );
	r_mapOverBrightBits = ri.Cvar_Get( "r_mapOverBrightBits", "2", CVAR_LATCH );
	AssertCvarRange( r_mapOverBrightBits, 0, 3, qtrue );
	r_intensity = ri.Cvar_Get( "r_intensity", "1", CVAR_LATCH );
	AssertCvarRange( r_intensity, 0, 1.5, qfalse );
	r_singleShader = ri.Cvar_Get( "r_singleShader", "0", CVAR_CHEAT | CVAR_LATCH );

	//
	// archived variables that can change at any time
	//
	r_lodCurveError = ri.Cvar_Get( "r_lodCurveError", "250", CVAR_ARCHIVE );
	r_lodbias = ri.Cvar_Get( "r_lodbias", "0", CVAR_ARCHIVE );
	r_flares = ri.Cvar_Get( "r_flares", "1", CVAR_ARCHIVE );
	r_znear = ri.Cvar_Get( "r_znear", "3", CVAR_CHEAT );  // ydnar: changed it to 3 (from 4) because of lean/fov cheats
	AssertCvarRange( r_znear, 0.001f, 200, qtrue );
//----(SA)	added
	r_zfar = ri.Cvar_Get( "r_zfar", "0", CVAR_CHEAT );
//----(SA)	end
	r_ignoreGLErrors = ri.Cvar_Get( "r_ignoreGLErrors", "1", CVAR_ARCHIVE );
	r_fastsky = ri.Cvar_Get( "r_fastsky", "0", CVAR_ARCHIVE );
	r_inGameVideo = ri.Cvar_Get( "r_inGameVideo", "1", CVAR_ARCHIVE );
	r_drawSun = ri.Cvar_Get( "r_drawSun", "1", CVAR_ARCHIVE );
	r_dynamiclight = ri.Cvar_Get( "r_dynamiclight", "1", CVAR_ARCHIVE );
	r_dlightBacks = ri.Cvar_Get( "r_dlightBacks", "1", CVAR_ARCHIVE );
	r_finish = ri.Cvar_Get( "r_finish", "0", CVAR_ARCHIVE );
	r_textureMode = ri.Cvar_Get( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST", CVAR_ARCHIVE );
	r_textureAnisotropy = ri.Cvar_Get( "r_textureAnisotropy", "1.0", CVAR_ARCHIVE );
	r_swapInterval = ri.Cvar_Get( "r_swapInterval", "0", CVAR_ARCHIVE );
#ifdef __MACOS__
	r_gamma = ri.Cvar_Get( "r_gamma", "1.2", CVAR_ARCHIVE );
#else
	r_gamma = ri.Cvar_Get( "r_gamma", "1.3", CVAR_ARCHIVE );
#endif
	r_facePlaneCull = ri.Cvar_Get( "r_facePlaneCull", "1", CVAR_ARCHIVE );

	r_railWidth = ri.Cvar_Get( "r_railWidth", "16", CVAR_ARCHIVE );
	r_railCoreWidth = ri.Cvar_Get( "r_railCoreWidth", "1", CVAR_ARCHIVE );
	r_railSegmentLength = ri.Cvar_Get( "r_railSegmentLength", "32", CVAR_ARCHIVE );

	r_primitives = ri.Cvar_Get( "r_primitives", "0", CVAR_ARCHIVE );

	r_ambientScale = ri.Cvar_Get( "r_ambientScale", "0.5", CVAR_CHEAT );
	r_directedScale = ri.Cvar_Get( "r_directedScale", "1", CVAR_CHEAT );

	//
	// temporary variables that can change at any time
	//
	r_showImages = ri.Cvar_Get( "r_showImages", "0", CVAR_TEMP );

	r_debugLight = ri.Cvar_Get( "r_debuglight", "0", CVAR_TEMP );
	r_debugSort = ri.Cvar_Get( "r_debugSort", "0", CVAR_CHEAT );
	r_printShaders = ri.Cvar_Get( "r_printShaders", "0", 0 );
	r_saveFontData = ri.Cvar_Get( "r_saveFontData", "0", 0 ); \

	// Ridah
	// TTimo show_bug.cgi?id=440
	//   with r_cache enabled, non-win32 OSes were leaking 24Mb per R_Init..
	r_cache = ri.Cvar_Get( "r_cache", "1", CVAR_LATCH );  // leaving it as this for backwards compability. but it caches models and shaders also
// (SA) disabling cacheshaders
//	ri.Cvar_Set( "r_cacheShaders", "0");
	// Gordon: enabling again..
	r_cacheShaders = ri.Cvar_Get( "r_cacheShaders", "1", CVAR_LATCH );
//----(SA)	end

	r_cacheModels = ri.Cvar_Get( "r_cacheModels", "1", CVAR_LATCH );
	r_cacheGathering = ri.Cvar_Get( "cl_cacheGathering", "0", 0 );
	r_buildScript = ri.Cvar_Get( "com_buildscript", "0", 0 );
	r_bonesDebug = ri.Cvar_Get( "r_bonesDebug", "0", CVAR_CHEAT );
	// done.

	// Rafael - wolf fog
	r_wolffog = ri.Cvar_Get( "r_wolffog", "1", CVAR_CHEAT ); // JPW NERVE cheat protected per id request
	// done

	r_nocurves = ri.Cvar_Get( "r_nocurves", "0", CVAR_CHEAT );
	r_drawworld = ri.Cvar_Get( "r_drawworld", "1", CVAR_CHEAT );
	r_drawfoliage = ri.Cvar_Get( "r_drawfoliage", "1", CVAR_CHEAT );  // ydnar
	r_lightmap = ri.Cvar_Get( "r_lightmap", "0", CVAR_CHEAT ); // DHM - NERVE :: cheat protect
	r_portalOnly = ri.Cvar_Get( "r_portalOnly", "0", CVAR_CHEAT );

	r_flareSize = ri.Cvar_Get( "r_flareSize", "40", CVAR_CHEAT );
	ri.Cvar_Set( "r_flareFade", "5" ); // to force this when people already have "7" in their config
	r_flareFade = ri.Cvar_Get( "r_flareFade", "5", CVAR_CHEAT );

	r_showSmp = ri.Cvar_Get( "r_showSmp", "0", CVAR_CHEAT );
	r_skipBackEnd = ri.Cvar_Get( "r_skipBackEnd", "0", CVAR_CHEAT );

	r_measureOverdraw = ri.Cvar_Get( "r_measureOverdraw", "0", CVAR_CHEAT );
	r_lodscale = ri.Cvar_Get( "r_lodscale", "5", CVAR_CHEAT );
	r_norefresh = ri.Cvar_Get( "r_norefresh", "0", CVAR_CHEAT );
	r_drawentities = ri.Cvar_Get( "r_drawentities", "1", CVAR_CHEAT );
	r_ignore = ri.Cvar_Get( "r_ignore", "1", CVAR_CHEAT );
	r_nocull = ri.Cvar_Get( "r_nocull", "0", CVAR_CHEAT );
	r_novis = ri.Cvar_Get( "r_novis", "0", CVAR_CHEAT );
	r_showcluster = ri.Cvar_Get( "r_showcluster", "0", CVAR_CHEAT );
	r_speeds = ri.Cvar_Get( "r_speeds", "0", CVAR_CHEAT );
	r_verbose = ri.Cvar_Get( "r_verbose", "0", CVAR_CHEAT );
	r_logFile = ri.Cvar_Get( "r_logFile", "0", CVAR_CHEAT );
	r_debugSurface = ri.Cvar_Get( "r_debugSurface", "0", CVAR_CHEAT );
	r_nobind = ri.Cvar_Get( "r_nobind", "0", CVAR_CHEAT );
	r_showtris = ri.Cvar_Get( "r_showtris", "0", CVAR_CHEAT );
	r_trisColor = ri.Cvar_Get( "r_trisColor", "1.0 1.0 1.0 1.0", CVAR_ARCHIVE );
	r_showsky = ri.Cvar_Get( "r_showsky", "0", CVAR_CHEAT );
	r_shownormals = ri.Cvar_Get( "r_shownormals", "0", CVAR_CHEAT );
	r_normallength = ri.Cvar_Get( "r_normallength", "0.5", CVAR_ARCHIVE );
	r_showmodelbounds = ri.Cvar_Get( "r_showmodelbounds", "0", CVAR_CHEAT );
	r_clear = ri.Cvar_Get( "r_clear", "0", CVAR_CHEAT );
	r_offsetFactor = ri.Cvar_Get( "r_offsetfactor", "-1", CVAR_CHEAT );
	r_offsetUnits = ri.Cvar_Get( "r_offsetunits", "-2", CVAR_CHEAT );
	r_drawBuffer = ri.Cvar_Get( "r_drawBuffer", "GL_BACK", CVAR_CHEAT );
	r_lockpvs = ri.Cvar_Get( "r_lockpvs", "0", CVAR_CHEAT );
	r_noportals = ri.Cvar_Get( "r_noportals", "0", CVAR_CHEAT );
	r_shadows = ri.Cvar_Get( "cg_shadows", "1", 0 );

	r_portalsky = ri.Cvar_Get( "cg_skybox", "1", 0 );
	r_maxpolys = ri.Cvar_Get( "r_maxpolys", va( "%d", MAX_POLYS ), 0 );
	r_maxpolyverts = ri.Cvar_Get( "r_maxpolyverts", va( "%d", MAX_POLYVERTS ), 0 );

	r_highQualityVideo = ri.Cvar_Get( "r_highQualityVideo", "1", CVAR_ARCHIVE );



/////////////////////////////////////////////////////////////////////////////////////////////
// commands
/////////////////////////////////////////////////////////////////////////////////////////////

	// renderer

	// removed in R_Shutdown
	ri.Cmd_AddCommand( "imagelist", R_ImageList_f );
	ri.Cmd_AddCommand( "shaderlist", R_ShaderList_f );
	ri.Cmd_AddCommand( "skinlist", R_SkinList_f );
	ri.Cmd_AddCommand( "modellist", R_Modellist_f );
	ri.Cmd_AddCommand( "modelist", R_ModeList_f );
	ri.Cmd_AddCommand( "screenshot", R_ScreenShot_f );
	ri.Cmd_AddCommand( "screenshotJPEG", R_ScreenShotJPEG_f );
	ri.Cmd_AddCommand( "gfxinfo", GfxInfo_f );
	ri.Cmd_AddCommand( "taginfo", R_TagInfo_f );


    // client
    Cmd_AddCommand( "toggleConsole", Con_ToggleConsole_f );
	Cmd_AddCommand( "clear", Con_Clear_f );
	Cmd_AddCommand( "condump", Con_Dump_f );

	// ydnar: these are deprecated in favor of cgame/ui based version
	Cmd_AddCommand( "clMessageMode", Con_MessageMode_f );
	Cmd_AddCommand( "clMessageMode2", Con_MessageMode2_f );
	Cmd_AddCommand( "clMessageMode3", Con_MessageMode3_f );


	// Rafael Kick
	// Arnout: now prone
	Cmd_AddCommand( "+prone", IN_ProneDown );
	Cmd_AddCommand( "-prone", IN_ProneUp );
	// done

	Cmd_AddCommand( "+sprint", IN_SprintDown );
	Cmd_AddCommand( "-sprint", IN_SprintUp );


	// wolf buttons
	Cmd_AddCommand( "+attack2",      IN_Wbutton0Down );   //----(SA) secondary firing
	Cmd_AddCommand( "-attack2",      IN_Wbutton0Up );
	Cmd_AddCommand( "+zoom",     IN_ZoomDown );       //
	Cmd_AddCommand( "-zoom",     IN_ZoomUp );
	Cmd_AddCommand( "+reload",       IN_ReloadDown );     //
	Cmd_AddCommand( "-reload",       IN_ReloadUp );
	Cmd_AddCommand( "+leanleft", IN_LeanLeftDown );
	Cmd_AddCommand( "-leanleft", IN_LeanLeftUp );
	Cmd_AddCommand( "+leanright",    IN_LeanRightDown );
	Cmd_AddCommand( "-leanright",    IN_LeanRightUp );


	Cmd_AddCommand( "+mlook", IN_MLookDown );
	Cmd_AddCommand( "-mlook", IN_MLookUp );

	Cmd_AddCommand( "help",IN_Help );

	Cmd_AddCommand( "bind",Key_Bind_f );
	Cmd_AddCommand( "unbind",Key_Unbind_f );
	Cmd_AddCommand( "unbindall",Key_Unbindall_f );
	Cmd_AddCommand( "bindlist",Key_Bindlist_f );

		Cmd_AddCommand( "cmd", CL_ForwardToServer_f );
	Cmd_AddCommand( "configstrings", CL_Configstrings_f );
	Cmd_AddCommand( "clientinfo", CL_Clientinfo_f );
	Cmd_AddCommand( "snd_reload", CL_Snd_Reload_f );
	Cmd_AddCommand( "snd_restart", CL_Snd_Restart_f );
	Cmd_AddCommand( "vid_restart", CL_Vid_Restart_f );
	Cmd_AddCommand( "ui_restart", CL_UI_Restart_f );          // NERVE - SMF
	Cmd_AddCommand( "disconnect", CL_Disconnect_f );
	Cmd_AddCommand( "record", CL_Record_f );
	Cmd_AddCommand( "demo", CL_PlayDemo_f );
	Cmd_AddCommand( "cinematic", CL_PlayCinematic_f );
	Cmd_AddCommand( "stoprecord", CL_StopRecord_f );
	Cmd_AddCommand( "connect", CL_Connect_f );
	Cmd_AddCommand( "reconnect", CL_Reconnect_f );
	Cmd_AddCommand( "localservers", CL_LocalServers_f );
	Cmd_AddCommand( "globalservers", CL_GlobalServers_f );
	Cmd_AddCommand( "rcon", CL_Rcon_f );
	Cmd_AddCommand( "setenv", CL_Setenv_f );
	Cmd_AddCommand( "ping", CL_Ping_f );
	Cmd_AddCommand( "serverstatus", CL_ServerStatus_f );
	Cmd_AddCommand( "showip", CL_ShowIP_f );
	Cmd_AddCommand( "fs_openedList", CL_OpenedPK3List_f );
	Cmd_AddCommand( "fs_referencedList", CL_ReferencedPK3List_f );

	// Ridah, startup-caching system
	Cmd_AddCommand( "cache_startgather", CL_Cache_StartGather_f );
	Cmd_AddCommand( "cache_usedfile", CL_Cache_UsedFile_f );
	Cmd_AddCommand( "cache_setindex", CL_Cache_SetIndex_f );
	Cmd_AddCommand( "cache_mapchange", CL_Cache_MapChange_f );
	Cmd_AddCommand( "cache_endgather", CL_Cache_EndGather_f );

	Cmd_AddCommand( "updatehunkusage", CL_UpdateLevelHunkUsage );
	Cmd_AddCommand( "updatescreen", SCR_UpdateScreen );
	// done.
#ifndef __MACOS__  //DAJ USA
	Cmd_AddCommand( "SaveTranslations", CL_SaveTranslations_f );     // NERVE - SMF - localization
	Cmd_AddCommand( "SaveNewTranslations", CL_SaveNewTranslations_f );   // NERVE - SMF - localization
	Cmd_AddCommand( "LoadTranslations", CL_LoadTranslations_f );     // NERVE - SMF - localization
#endif


	Cmd_AddCommand( "setRecommended", CL_SetRecommended_f );

	//bani - we eat these commands to prevent exploits
	Cmd_AddCommand( "userinfo", CL_EatMe_f );

	Cmd_AddCommand( "wav_record", CL_WavRecord_f );
	Cmd_AddCommand( "wav_stoprecord", CL_WavStopRecord_f );

	Cmd_AddCommand( "play", S_Play_f );
	Cmd_AddCommand( "music", S_Music_f );
	Cmd_AddCommand( "music_queue", S_QueueMusic_f );
	Cmd_AddCommand( "streamingsound", S_StreamingSound_f );
	Cmd_AddCommand( "s_list", S_SoundList_f );
	Cmd_AddCommand( "s_info", S_SoundInfo_f );
	Cmd_AddCommand( "s_stop", S_StopAllSounds );


	Cmd_AddCommand( "in_restart", Sys_In_Restart_f );

Win only:	Cmd_AddCommand( "net_restart", Sys_Net_Restart_f );
Win only:	Cmd_AddCommand( "clearviewlog", Sys_ClearViewlog_f );

	// client end


	// qcommon

 	Cmd_AddCommand( "cmdlist",Cmd_List_f );
	Cmd_AddCommand( "exec",Cmd_Exec_f );
	Cmd_AddCommand( "vstr",Cmd_Vstr_f );
	Cmd_AddCommand( "echo",Cmd_Echo_f );
	Cmd_AddCommand( "wait", Cmd_Wait_f );

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
#ifdef ZONE_DEBUG
	Cmd_AddCommand( "zonelog", Z_LogHeap );
#endif
#ifdef HUNK_DEBUG
	Cmd_AddCommand( "hunklog", Hunk_Log );
	Cmd_AddCommand( "hunksmalllog", Hunk_SmallLog );
#endif



	if ( com_developer && com_developer->integer ) {
		Cmd_AddCommand( "error", Com_Error_f );
		Cmd_AddCommand( "crash", Com_Crash_f );
		Cmd_AddCommand( "freeze", Com_Freeze_f );
		Cmd_AddCommand( "cpuspeed", Com_CPUSpeed_f );
	}
	Cmd_AddCommand( "quit", Com_Quit_f );
	Cmd_AddCommand( "changeVectors", MSG_ReportChangeVectors_f );
	Cmd_AddCommand( "writeconfig", Com_WriteConfig_f );


	Cmd_AddCommand( "toggle", Cvar_Toggle_f );
	Cmd_AddCommand( "cycle", Cvar_Cycle_f );  // ydnar
	Cmd_AddCommand( "set", Cvar_Set_f );
	Cmd_AddCommand( "sets", Cvar_SetS_f );
	Cmd_AddCommand( "setu", Cvar_SetU_f );
	Cmd_AddCommand( "seta", Cvar_SetA_f );
	Cmd_AddCommand( "reset", Cvar_Reset_f );
	Cmd_AddCommand( "cvarlist", Cvar_List_f );
	Cmd_AddCommand( "cvar_restart", Cvar_Restart_f );

		Cmd_AddCommand( "path", FS_Path_f );
	Cmd_AddCommand( "dir", FS_Dir_f );
	Cmd_AddCommand( "fdir", FS_NewDir_f );
	Cmd_AddCommand( "touchFile", FS_TouchFile_f );

	Cmd_AddCommand( "vmprofile", VM_VmProfile_f );
	Cmd_AddCommand( "vminfo", VM_VmInfo_f );



	// renderer
	ri.Cmd_AddCommand( "imagelist", R_ImageList_f );
	ri.Cmd_AddCommand( "shaderlist", R_ShaderList_f );
	ri.Cmd_AddCommand( "skinlist", R_SkinList_f );
	ri.Cmd_AddCommand( "modellist", R_Modellist_f );
	ri.Cmd_AddCommand( "modelist", R_ModeList_f );
	ri.Cmd_AddCommand( "screenshot", R_ScreenShot_f );
	ri.Cmd_AddCommand( "screenshotJPEG", R_ScreenShotJPEG_f );
	ri.Cmd_AddCommand( "gfxinfo", GfxInfo_f );
	ri.Cmd_AddCommand( "taginfo", R_TagInfo_f );
*/

#endif
