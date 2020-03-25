/*
 * name:		cg_consolecmds.c
 *
 * desc:		text commands typed in at the local console, or executed by a key binding
 *
 * NQQS:
 *
 */
#include "cg_local.h"
#include "nq_help.h"	// core: NQ-help, documentation on settings, howto use, explaining bitflag values, etc..

/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f (void)
{
	CG_Printf("(%i %i %i) : %i\n", (int)cg.refdef.vieworg[0], (int)cg.refdef.vieworg[1], (int)cg.refdef.vieworg[2], (int)cg.refdefViewAngles[YAW]);
}


void CG_ClassMenu_f( void ) {

	if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR ) {
		return;
	}

	CG_EventHandling( CGAME_EVENT_NONE, qfalse );

	if( cg_quickMessageAlt.integer ) {
		trap_UI_Popup( UIMENU_WM_CLASSALT );
	} else {
		trap_UI_Popup( UIMENU_WM_CLASS );
	}
}

void CG_Class_f( void ) {
	int selectClass, weapon = 0, weaponNum;
	bg_playerclass_t* classInfo;
	char *str;

	if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR ) {
		return;
	}

	if ( trap_Argc() < 2 ) {
		return;
	}

	if (!Q_stricmp(CG_Argv(1), "s") ){
		selectClass = PC_SOLDIER;
	}
	else if (!Q_stricmp(CG_Argv(1), "m") ){
		selectClass = PC_MEDIC;
	}
	else if (!Q_stricmp(CG_Argv(1), "e") ){
		selectClass = PC_ENGINEER;
	}
	else if (!Q_stricmp(CG_Argv(1), "f") ){
		selectClass = PC_FIELDOPS;
	}
	else if (!Q_stricmp(CG_Argv(1), "c") ){
		selectClass = PC_COVERTOPS;
	}
	else {
		return; // unknown class, wtf :)
	}

	if (  cgs.clientinfo[ cg.clientNum ].team == TEAM_AXIS ){
		classInfo =	&bg_axis_playerclasses[selectClass];
	}
	else if ( cgs.clientinfo[ cg.clientNum ].team == TEAM_ALLIES){
		classInfo =	&bg_allies_playerclasses[selectClass];
	}
	else {
		// unknown team
		return;
	}

	if ( trap_Argc() > 2 ) {
		weapon = atoi(CG_Argv(2));
	}

	if ( weapon && classInfo->classWeapons[weapon-1] ){
		weaponNum = classInfo->classWeapons[weapon-1];
		CG_LimboPanel_SetSelectedWeaponNumForSlot(weapon-1, 0);
	}
	else {
		weaponNum = classInfo->classWeapons[0];
		CG_LimboPanel_SetSelectedWeaponNumForSlot(0, 0);
	}

	if ( CG_LimboPanel_WeaponIsDisabled( cgs.ccSelectedWeapon )) {
		weaponNum = classInfo->classWeapons[0];
		CG_LimboPanel_SetSelectedWeaponNumForSlot(0, 0);
		CG_Printf( "%s is disabled\n", BG_Weapons[weaponNum].name );
	}

	trap_SendClientCommand( va( "team %s %i %i \n",  cgs.clientinfo[ cg.clientNum ].team == TEAM_AXIS ? "r" : "b", selectClass, weaponNum ) );

	// print center message
	switch( cgs.clientinfo[ cg.clientNum ].team ) {
		case TEAM_AXIS:
			str = "Axis";
			break;
		case TEAM_ALLIES:
			str = "Allied";
			break;
		default: // rain - added default
			str = "unknown";
			break;
	}

	{
		CG_PriorityCenterPrint(
			va( "You will spawn as an %s %s with a %s.",
				str, BG_ClassnameForNumber( selectClass ),
				BG_Weapons[weaponNum].name),
				SCREEN_HEIGHT - 88,
				SMALLCHAR_WIDTH,
				-1 );
	}
	cgs.limboLoadoutSelected = qtrue;
}

void CG_LimboMenu_f( void ) {
	if( cg.showGameView ) {
		CG_EventHandling( CGAME_EVENT_NONE, qfalse );
	}
	else {
		CG_EventHandling( CGAME_EVENT_GAMEVIEW, qfalse );
	}
}

static void CG_StatsDown_f(void)
{
	if(!cg.demoPlayback) {

		if(
#ifdef MV_SUPPORT
			cg.mvTotalClients < 1 &&
#endif
			cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
			Pri("You must be a player or following a player to use +stats\n");
			return;
		}

		if(cgs.gamestats.show == SHOW_SHUTDOWN && cg.time < cgs.gamestats.fadeTime) {
			cgs.gamestats.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.gamestats.fadeTime;
		}
		else if(cgs.gamestats.show != SHOW_ON) {
			cgs.gamestats.fadeTime = cg.time + STATS_FADE_TIME;
		}

		cgs.gamestats.show = SHOW_ON;

		if(cgs.gamestats.requestTime < cg.time) {
			int i =
#ifdef MV_SUPPORT
			(cg.mvTotalClients > 0) ? (cg.mvCurrentActive->mvInfo & MV_PID) :
#endif
			cg.snap->ps.clientNum;

			cgs.gamestats.requestTime = cg.time + 2000;
			trap_SendClientCommand(va("sgstats %d", i));
		}
	}
}

static void CG_StatsUp_f(void)
{
	if(cgs.gamestats.show == SHOW_ON) {
		cgs.gamestats.show = SHOW_SHUTDOWN;
		if(cg.time < cgs.gamestats.fadeTime) {
			cgs.gamestats.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.gamestats.fadeTime;
		}
		else {
			cgs.gamestats.fadeTime = cg.time + STATS_FADE_TIME;
		}
	}
}

void CG_topshotsDown_f(void)
{
	if(!cg.demoPlayback) {
		if(cgs.topshots.show == SHOW_SHUTDOWN && cg.time < cgs.topshots.fadeTime) {
			cgs.topshots.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.topshots.fadeTime;
		}
		else if(cgs.topshots.show != SHOW_ON) {
			cgs.topshots.fadeTime = cg.time + STATS_FADE_TIME;
		}

		cgs.topshots.show = SHOW_ON;

		if(cgs.topshots.requestTime < cg.time) {
			cgs.topshots.requestTime = cg.time + 2000;
			trap_SendClientCommand("stshots");
		}
	}
}

void CG_topshotsUp_f(void)
{
	if(cgs.topshots.show == SHOW_ON) {
		cgs.topshots.show = SHOW_SHUTDOWN;
		if(cg.time < cgs.topshots.fadeTime) {
			cgs.topshots.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.topshots.fadeTime;
		}
		else {
			cgs.topshots.fadeTime = cg.time + STATS_FADE_TIME;
		}
	}
}

void CG_ScoresDown_f( void ) {
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		// the scores are more than two seconds out of data,
		// so request new ones
		cg.scoresRequestTime = cg.time;

		// OSP - we get periodic score updates if we are merging clients
		if(!cg.demoPlayback
#ifdef MV_SUPPORT
			&& cg.mvTotalClients < 1
#endif
			) trap_SendClientCommand( "score" );

		// leave the current scores up if they were already
		// displayed, but if this is the first hit, clear them out
		if ( !cg.showScores ) {
			cg.showScores = qtrue;
			if(!cg.demoPlayback
#ifdef MV_SUPPORT
				&& cg.mvTotalClients < 1
#endif
				) cg.numScores = 0;
		}
	}
	else {
		// show the cached contents even if they just pressed if it is within two seconds
		cg.showScores = qtrue;
	}
}

void CG_ScoresUp_f( void ) {
	if ( cg.showScores ) {
		cg.showScores = qfalse;
		cg.scoreFadeTime = cg.time;
	}
}

static void CG_LoadWeapons_f( void ) {
	int i;
	// start with 0 !
	for( i = WP_NONE; i < WP_NUM_WEAPONS; i++ ) {
		BG_RegisterWeapon( i, qtrue ); // jaquboss
	}
}


static void CG_TellTarget_f( void ) {
	int		clientNum = CG_CrosshairPlayer();
	char	command[128];
	char	message[128];

	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "pm %s %s", cgs.clientinfo[clientNum].name, message );
	trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( void ) {
	int		clientNum = CG_LastAttacker();
	char	command[128];
	char	message[128];

	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "pm %s %s", cgs.clientinfo[clientNum].name, message );
	trap_SendClientCommand( command );
}

static void CG_Fade_f( void ) {
	int r, g, b, a;
	float duration;

	if ( trap_Argc() < 6 ) {
		return;
	}

	r = atof(CG_Argv(1));
	g = atof(CG_Argv(2));
	b = atof(CG_Argv(3));
	a = atof(CG_Argv(4));

	duration = atof(CG_Argv(5)) * 1000;

	CG_Fade(r, g, b, a, cg.time, duration);
}

void CG_QuickMessage_f( void ) {

	CG_EventHandling( CGAME_EVENT_NONE, qfalse );

	if( cg_quickMessageAlt.integer ) {
		trap_UI_Popup( UIMENU_WM_QUICKMESSAGEALT );
	}
	else {
		trap_UI_Popup( UIMENU_WM_QUICKMESSAGE );
	}
}

void CG_QuickFireteamMessage_f( void ) {
	if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR ) {
		return;
	}

	CG_EventHandling( CGAME_EVENT_NONE, qfalse );

	if( cg_quickMessageAlt.integer ) {
		trap_UI_Popup( UIMENU_WM_FTQUICKMESSAGEALT );
	}
	else {
		trap_UI_Popup( UIMENU_WM_FTQUICKMESSAGE );
	}
}

void CG_QuickFireteamAdmin_f( void ) {
	trap_UI_Popup( UIMENU_NONE );

	if( cg.showFireteamMenu ) {
		if( cgs.ftMenuMode == 1 ) {
			CG_EventHandling( CGAME_EVENT_NONE, qfalse );
		}
		else {
			cgs.ftMenuMode = 1;
		}
	}
	else if( cgs.clientinfo[ cg.clientNum ].team != TEAM_SPECTATOR ) {
		CG_EventHandling( CGAME_EVENT_FIRETEAMMSG, qfalse );
		cgs.ftMenuMode = 1;
	}
}

static void CG_QuickFireteams_f( void ) {
	if( cg.showFireteamMenu ) {
		if( cgs.ftMenuMode == 0 ) {
			CG_EventHandling( CGAME_EVENT_NONE, qfalse );
		}
		else {
			cgs.ftMenuMode = 0;
		}
	}
	else if( CG_IsOnFireteam( cg.clientNum ) ) {
		CG_EventHandling( CGAME_EVENT_FIRETEAMMSG, qfalse );
		cgs.ftMenuMode = 0;
	}
}

static void CG_FTSayPlayerClass_f( void ) {
	const char *s;

	switch (cgs.clientinfo[ cg.clientNum ].cls) {
		case PC_MEDIC:
			s = "IamMedic";
			break;
		case PC_ENGINEER:
			s = "IamEngineer";
			break;
		case PC_FIELDOPS:
			s = "IamFieldOps";
			break;
		case PC_COVERTOPS:
			s = "IamCovertOps";
			break;
		default:
			s = "IamSoldier";
			break;
	}

	if ( cg.snap && ( cg.snap->ps.pm_type != PM_INTERMISSION ) ) {
		if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE ) {
			CG_Printf ( CG_TranslateString( "Can't say class when spectator.\n" ) );
			return;
		}
	}

	trap_SendConsoleCommand( va( "cmd vsay_buddy -1 %s %s\n", CG_BuildSelectedFirteamString(), s ) );
}

static void CG_SayPlayerClass_f( void )
{
	const char *s;

	switch (cgs.clientinfo[ cg.clientNum ].cls) {
		case PC_MEDIC:
			s = "IamMedic";
			break;
		case PC_ENGINEER:
			s = "IamEngineer";
			break;
		case PC_FIELDOPS:
			s = "IamFieldOps";
			break;
		case PC_COVERTOPS:
			s = "IamCovertOps";
			break;
		default:
			s = "IamSoldier";
			break;
	}

	if ( cg.snap && ( cg.snap->ps.pm_type != PM_INTERMISSION ) ) {
		if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE ) {
			CG_Printf ( CG_TranslateString( "Can't say class when spectator.\n" ) );
			return;
		}
	}

	trap_SendConsoleCommand( va( "cmd vsay_team %s\n", s ) );
}

static void CG_VoiceChat_f( void ) {
	char chatCmd[64];

	if ( trap_Argc() != 2 )
		return;

	trap_Argv( 1, chatCmd, 64 );
	trap_SendConsoleCommand( va( "cmd vsay %s\n", chatCmd ) );
}

static void CG_TeamVoiceChat_f( void ) {
	char chatCmd[64];

	if( trap_Argc() != 2 ) {
		return;
	}

	// NERVE - SMF - don't let spectators voice chat
	// NOTE - This cg.snap will be the person you are following, but its just for intermission test
	if ( cg.snap && ( cg.snap->ps.pm_type != PM_INTERMISSION ) ) {
		if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE ) {
			CG_Printf ( CG_TranslateString( "Can't team voice chat as a spectator.\n" ) );
			return;
		}
	}

	trap_Argv( 1, chatCmd, 64 );
	trap_SendConsoleCommand( va( "cmd vsay_team %s\n", chatCmd ) );
}

static void CG_BuddyVoiceChat_f( void ) {
	char chatCmd[64];

	if( trap_Argc() != 2 ) {
		return;
	}

	// NERVE - SMF - don't let spectators voice chat
	// NOTE - This cg.snap will be the person you are following, but its just for intermission test
	if ( cg.snap && ( cg.snap->ps.pm_type != PM_INTERMISSION ) ) {
		if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE ) {
			CG_Printf ( CG_TranslateString( "Can't buddy voice chat as a spectator.\n" ) );
			return;
		}
	}

	trap_Argv( 1, chatCmd, 64 );

	trap_SendConsoleCommand( va( "cmd vsay_buddy -1 %s %s\n", CG_BuildSelectedFirteamString(), chatCmd ) );
}


// core: to backup/restore the PM message target..
// (so we can PM, global chat, and PM again without losing the messageTarget).
// used in: CG_MessageMode_f() and CG_MessageSend_f()
static char	lastMessageTarget[MAX_NAME_LENGTH] = "\0";

// ydnar: say, team say, etc
static void CG_MessageMode_f( void ) {
	char		cmd[ 64 ];

	if( cgs.eventHandling != CGAME_EVENT_NONE ) {
		return;
	}

	// get the actual command
	trap_Argv( 0, cmd, 64 );

	// team say
	if( !Q_stricmp( cmd, "messagemode2" ) ) {
		trap_Cvar_Set( "cg_messageType", "2" );
		trap_Cvar_Set( "cg_messageTarget", "" );
	}
	// fireteam say
	else if( !Q_stricmp( cmd, "messagemode3" ) ) {
		trap_Cvar_Set( "cg_messageType", "3" );
		trap_Cvar_Set( "cg_messageTarget", "" );
	}
	// private say
	else if( !Q_stricmp( cmd, "messagemode4" ) ) {
		trap_Cvar_Set( "cg_messageType", "4" );
		// restore the previously used PM message target..
		trap_Cvar_Set( "cg_messageTarget", lastMessageTarget );
	}
	// (normal) say
	else {
		trap_Cvar_Set( "cg_messageType", "1" );
		trap_Cvar_Set( "cg_messageTarget", "" );
	}

	// clear the chat text
	trap_Cvar_Set( "cg_messageText", "" );

	// open the menu
	trap_UI_Popup( UIMENU_INGAME_MESSAGEMODE );
}

static void CG_MessageSend_f( void )
{
	char	messageText[ 256 ];
	int		messageType;
	char	target[MAX_NAME_LENGTH];

	// get values
	trap_Cvar_VariableStringBuffer( "cg_messageType", messageText, 256 );
	messageType = atoi( messageText );
	trap_Cvar_VariableStringBuffer( "cg_messageText", messageText, 256 );
	trap_Cvar_VariableStringBuffer( "cg_messageTarget", target, MAX_NAME_LENGTH );

	// reset values
	trap_Cvar_Set( "cg_messageText", "" );
	trap_Cvar_Set( "cg_messageType", "" );

	// don't send empty messages
	if( messageText[0] == '\0' ) {
		return;
	}

	switch (messageType) {
		case 2: // team say
			trap_SendConsoleCommand( va( "say_team \"%s\"\n", messageText ) );
			break;
		case 3: // fireteam say
			trap_SendConsoleCommand( va( "say_buddy \"%s\"\n", messageText ) );
			break;
		case 4: // personal message
			// don't send empty pm targets
			if( target[ 0 ] == '\0' ) {
				return;
			}
			trap_SendConsoleCommand( va( "pm \"%s\" \"%s\"\n", target , messageText ) );
			// backup current PM message target, so we can fill in the same target in later calls..
			Q_strncpyz( lastMessageTarget, target, sizeof(lastMessageTarget) );
			break;
		default:
			trap_SendConsoleCommand( va( "say \"%s\"\n", messageText ) );
			break;
	}
}

static void CG_SetWeaponCrosshair_f( void ) {
	char crosshair[64];

	trap_Argv( 1, crosshair, 64 );
	cg.newCrosshairIndex = atoi( crosshair ) + 1;
}
// -NERVE - SMF

static void CG_SelectBuddy_f( void ) {
	int pos = atoi( CG_Argv( 1 ) );
	int i;
	clientInfo_t* ci;

	// Gordon:
	// 0 - 5 = select that person
	// -1 = none
	// -2 = all

	switch( pos ) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if( !CG_IsOnFireteam( cg.clientNum ) ) {
				break; // Gordon: we aren't a leader, so dont allow selection
			}

			ci = CG_SortedFireTeamPlayerForPosition( pos );//, 6 );
			if( !ci ) {
				break; // there was no-one in this position
			}

			ci->selected ^= qtrue;

			break;

		case -1:
			if( !CG_IsOnFireteam( cg.clientNum ) ) {
				break; // Gordon: we aren't a leader, so dont allow selection
			}

			for(i = 0; i < MAX_FIRETEAM_MEMBERS; i++) {
				ci = CG_SortedFireTeamPlayerForPosition( i );//, 6 );
				if( !ci ) {
					break; // there was no-one in this position
				}

				ci->selected = qfalse;
			}
			break;

		case -2:
			if( !CG_IsOnFireteam( cg.clientNum ) ) {
				break; // Gordon: we aren't a leader, so dont allow selection
			}

			for(i = 0; i < MAX_FIRETEAM_MEMBERS; i++) {
				ci = CG_SortedFireTeamPlayerForPosition( i );//, 6 );
				if(!ci) {
					break; // there was no-one in this position
				}

				ci->selected = qtrue;
			}
			break;

	}
}

extern void CG_AdjustAutomapZoom(int zoomIn);

static void CG_AutomapZoomIn_f(void) {
	if( !cgs.autoMapOff ) {
		CG_AdjustAutomapZoom(qtrue);
	}
}

static void CG_AutomapZoomOut_f(void) {
	if( !cgs.autoMapOff ) {
		CG_AdjustAutomapZoom(qfalse);
	}
}
#if 0
static void CG_MouseModeDown_f( void )
{
	if ( !cgs.mouseModeOn )
	{
		cgs.mouseModeOn = qtrue;
		Com_Printf("^9Entering Mouse Pointer Mode\n");
	}
}

static void CG_MouseModeUp_f( void )
{
	if ( cgs.mouseModeOn )
	{
		cgs.mouseModeOn = qfalse;
		Com_Printf("^9Exiting Mouse Pointer Mode\n");
	}
}
#endif


static void CG_KickDown_f( void ) {
	trap_SendConsoleCommand("+salute");
}

static void CG_KickUp_f( void ) {
	trap_SendConsoleCommand("-salute");
}


// core: Some help for the user/admin..
static void CG_Help_f( void ) {
	const char		*Str = NULL;
	int				count = sizeof(helpTexts) / sizeof(helpTexts[0]) - 1;
	int				i = 0, lineNr = 0;
	char			*Txt = NULL;
	char			*colorLine = "^9--------------------------------------------\n";
	char			*colorCVar = "^A";
	char			*colorDesc = "^M";
	char			*colorDef = "^A";
	char			*colorSpecial = "^O";
	qboolean		explain = qtrue;
	unsigned int	category = 0;
	int				argc = trap_Argc();

	if ( argc == 1 ) {
		explain = qtrue;
	} else if ( argc >= 2 ) {
		Str = CG_Argv(1);
		explain = (!Q_stricmpn("?",Str,1))? qtrue : qfalse;
		explain = (!Q_stricmpn("help",Str,4))? qtrue : explain;
	}

	CG_Printf(colorLine);
	// check for special arguments..
	if ( explain ) {
		CG_Printf(va("%sUsage:  nqhelp <cvar || special>\n\n",colorDesc));
		CG_Printf(va("%sYou can enter a CVar-name you want a description of.\n\n",colorDesc));
		CG_Printf(va("%sYou can instead enter a special search-argument\n",colorDesc));
		CG_Printf(va("%sto get a list of related CVars.\n",colorDesc));
		CG_Printf(va("%sThe special search-arguments are:\n",colorDesc));
		CG_Printf(va("%sweapons      spawning     warmup       logging      \n",colorSpecial));
		CG_Printf(va("%ssecurity     xp           configs      messages     \n",colorSpecial));
		CG_Printf(va("%ssoldier      medic        engineer     fieldops     \n",colorSpecial));
		CG_Printf(va("%scovertops    voting       damage       penalty      \n",colorSpecial));
		CG_Printf(va("%sskill        bots         time         duration     \n",colorSpecial));
		CG_Printf(va("%sdistance     shortcuts    lua          debug        \n",colorSpecial));
		CG_Printf(va("%sperformance  restrict                               \n",colorSpecial));

	} else if ( !Q_stricmp("weapons",Str) ) {
		category = HELP_CAT_WEAPONS;
	} else if ( !Q_stricmp("spawning",Str) ) {
		category = HELP_CAT_SPAWNING;
	} else if ( !Q_stricmp("warmup",Str) ) {
		category = HELP_CAT_WARMUP;
	} else if ( !Q_stricmp("logging",Str) ) {
		category = HELP_CAT_LOGGING;
	} else if ( !Q_stricmp("security",Str) ) {
		category = HELP_CAT_SEQURITY;
	} else if ( !Q_stricmp("xp",Str) ) {
		category = HELP_CAT_XP;
	} else if ( !Q_stricmp("configs",Str) ) {
		category = HELP_CAT_CONFIGS;
	} else if ( !Q_stricmp("messages",Str) ) {
		category = HELP_CAT_MESSAGES;
	} else if ( !Q_stricmp("soldier",Str) ) {
		category = HELP_CAT_SOLDIERS;
	} else if ( !Q_stricmp("medic",Str) ) {
		category = HELP_CAT_MEDICS;
	} else if ( !Q_stricmp("engineer",Str) ) {
		category = HELP_CAT_ENGINEERS;
	} else if ( !Q_stricmp("fieldops",Str) ) {
		category = HELP_CAT_FIELDOPS;
	} else if ( !Q_stricmp("covertops",Str) ) {
		category = HELP_CAT_COVERTOPS;
	} else if ( !Q_stricmp("voting",Str) ) {
		category = HELP_CAT_VOTING;
	} else if ( !Q_stricmp("damage",Str) ) {
		category = HELP_CAT_DAMAGE;
	} else if ( !Q_stricmp("penalty",Str) ) {
		category = HELP_CAT_PENALTY;
	} else if ( !Q_stricmp("skill",Str) ) {
		category = HELP_CAT_SKILLS;
	} else if ( !Q_stricmp("bots",Str) ) {
		category = HELP_CAT_BOTS;
	} else if ( !Q_stricmp("time",Str) || !Q_stricmp("duration",Str) ) {
		category = HELP_CAT_TIME;
	} else if ( !Q_stricmp("distance",Str) ) {
		category = HELP_CAT_DISTANCE;
	} else if ( !Q_stricmp("shortcuts",Str) ) {
		category = HELP_CAT_DOCU + HELP_CAT_SHORTCUTS;	// not cvar related..
	} else if ( !Q_stricmp("lua",Str) ) {
		category = HELP_CAT_LUA;
	} else if ( !Q_stricmp("debug",Str) ) {
		category = HELP_CAT_DEBUG;
	} else if ( !Q_stricmp("performance",Str) ) {
		category = HELP_CAT_PERFORMANCE;
	} else if ( !Q_stricmp("restrict",Str) || !Q_stricmp("restriction",Str) ) {
		category = HELP_CAT_RESTRICT;
	} else {
		// if found, print the CVar help-content from the nq_help array..
		for ( i=0; i<count; ++i ) {
			if ( Q_stricmpn(helpTexts[i].cvar, Str, strlen(Str)) ) continue;
			// the CVar..
			CG_Printf( va("%s%s\n",colorCVar,helpTexts[i].cvar) );
			// the description..
			for ( lineNr=0; lineNr<HELP_NUM_LINES; ++lineNr ) {
				Txt = helpTexts[i].line[lineNr];
				if ( strlen(Txt)==0 ) break;
				CG_Printf(va("%s%s\n",colorDesc,Txt));
			}
			// the default value..
			CG_Printf(va("%sDefault value: \"%s\"\n",colorDef,helpTexts[i].defVal));
			CG_Printf(colorLine);
		}
		return;
	}

	// process special search arguments..
	if ( argc >= 2 ) {
		if ( category & HELP_CAT_DOCU ) {
			// common topics..
			for ( i=0; i<count; ++i ) {
				if ( (helpTexts[i].categories & category) == category ) {
					// print the description..
					for ( lineNr=0; lineNr<HELP_NUM_LINES; ++lineNr ) {
						Txt = helpTexts[i].line[lineNr];
						if ( strlen(Txt)==0 ) break;
						CG_Printf(va("%s%s\n",colorDesc,Txt));
					}
				}
			}
		} else {
			// Related CVars..
			Txt = Q_strCapitalize((char *)Str);
			CG_Printf( va("%s%s%s related CVars are:\n",colorSpecial,Txt,colorDesc) );
			for ( i=0; i<count; ++i ) {
				if ( helpTexts[i].categories & category ) {
					// print the CVar..
					CG_Printf( va("%s%s\n",colorCVar,helpTexts[i].cvar) );
				}
			}
		}
	}
	CG_Printf(colorLine);
}


static void CG_AutomapExpandDown_f( void ) {
	if( !cgs.autoMapExpanded ) {
		cgs.autoMapExpanded = qtrue;
		if( cg.time - cgs.autoMapExpandTime < 250.f ) {
			cgs.autoMapExpandTime = cg.time - ( 250.f - ( cg.time - cgs.autoMapExpandTime ) );
		} else {
			cgs.autoMapExpandTime = cg.time;
		}
	}
}

static void CG_AutomapExpandUp_f( void ) {
	if( cgs.autoMapExpanded ) {
		cgs.autoMapExpanded = qfalse;
		if( cg.time - cgs.autoMapExpandTime < 250.f ) {
			cgs.autoMapExpandTime = cg.time - ( 250.f - ( cg.time - cgs.autoMapExpandTime ) );
		} else {
			cgs.autoMapExpandTime = cg.time;
		}
	}
}

static void CG_ToggleAutomap_f( void ) {
	cgs.autoMapOff = !cgs.autoMapOff;
}

// OSP
const char *aMonths[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void CG_currentTime_f( void ) {
	qtime_t ct;

	trap_RealTime(&ct);
	CG_Printf( "[cgnotify]Current time: ^3%02d:%02d:%02d (%02d %s %d)\n", ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday, aMonths[ct.tm_mon], 1900+ct.tm_year );
}

// Dynamically names a demo and sets up the recording
void CG_autoRecord_f( void ) {
	trap_SendConsoleCommand(va("record %s\n", CG_generateFilename()));
}

// Dynamically names a screenshot[JPEG]
void CG_autoScreenShot_f(void)
{
	trap_SendConsoleCommand(va("screenshot%s %s\n", ((cg_useScreenshotJPEG.integer)?"JPEG":""), CG_generateFilename()));
}

void CG_vstrDown_f(void)
{
	// The engine also passes back the key code and time of the key press
	if(trap_Argc() == 5) trap_SendConsoleCommand(va("vstr %s;", CG_Argv(1)));
	else CG_Printf("[cgnotify]Usage: +vstr [down_vstr] [up_vstr]\n");
}

void CG_vstrUp_f(void)
{
	// The engine also passes back the key code and time of the key press
	if(trap_Argc() == 5) trap_SendConsoleCommand(va("vstr %s;", CG_Argv(2)));
	else CG_Printf("[cgnotify]Usage: +vstr [down_vstr] [up_vstr]\n");
}

void CG_keyOn_f(void)
{
	if(!cg.demoPlayback) {
		CG_Printf("[cgnotify]^3*** NOT PLAYING A DEMO!!\n");
		return;
	}

	if(demo_infoWindow.integer > 0) {
		CG_ShowHelp_On(&cg.demohelpWindow);
	}

	CG_EventHandling(CGAME_EVENT_DEMO, qtrue);
}

void CG_keyOff_f(void)
{
	if(!cg.demoPlayback) return;
	CG_EventHandling(CGAME_EVENT_NONE, qfalse);
}

void CG_dumpStats_f(void)
{
	if(cgs.dumpStatsTime < cg.time) {
		cgs.dumpStatsTime = cg.time + 2000;
		trap_SendClientCommand(
#ifdef MV_SUPPORT
			(cg.mvTotalClients < 1) ?
#endif
			"weaponstats"
#ifdef MV_SUPPORT
			: "statsall"
#endif
			);
	}
}
void CG_wStatsDown_f(void)
{
	if(
#ifdef MV_SUPPORT
		cg.mvTotalClients < 1 &&
#endif
		cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		Pri("You must be a player or following a player to use +wstats\n");
		return;
	}

	if(cg.statsRequestTime < cg.time) {
		int i =
#ifdef MV_SUPPORT
		(cg.mvTotalClients > 0) ? (cg.mvCurrentActive->mvInfo & MV_PID) :
#endif
		cg.snap->ps.clientNum;

		cg.statsRequestTime = cg.time + 500;
		trap_SendClientCommand(va("wstats %d", i));
	}

	cg.showStats = qtrue;
}

void CG_wStatsUp_f(void)
{
	cg.showStats = qfalse;
	CG_windowFree(cg.statsWindow);
	cg.statsWindow = NULL;
}

void CG_toggleSpecHelp_f(void)
{
#ifdef MV_SUPPORT
	if(cg.mvTotalClients > 0 && !cg.demoPlayback) {
		if(cg.spechelpWindow != SHOW_ON && cg_specHelp.integer > 0) {
			CG_ShowHelp_On(&cg.spechelpWindow);
		}
		else if(cg.spechelpWindow == SHOW_ON) {
			CG_ShowHelp_Off(&cg.spechelpWindow);
		}
	}
#endif
}
// -OSP

void CG_Obj_f( void ) {
	// Gordon: short circuit this
}

static void CG_EditSpeakers_f( void )
{
	if( cg.editingSpeakers ) {
		CG_DeActivateEditSoundMode();
	}
	else {
		const char *s = Info_ValueForKey( CG_ConfigString( CS_SYSTEMINFO ), "sv_cheats" );
		if ( s[0] != '1' ) {
			CG_Printf( "editSpeakers is cheat protected.\n" );
			return;
		}
		CG_ActivateEditSoundMode();
	}
}

static void CG_DumpSpeaker_f( void )
{
	bg_speaker_t	speaker;
	trace_t			tr;
	vec3_t			end;

	if( !cg.editingSpeakers ) {
		CG_Printf( "Speaker Edit mode needs to be activated to dump speakers\n" );
		return;
	}

	memset( &speaker, 0, sizeof(speaker) );

	speaker.volume = 127;
	speaker.range = 1250;

	VectorMA( cg.refdef_current->vieworg, 32.0f, cg.refdef_current->viewaxis[0], end );
	CG_Trace( &tr, cg.refdef_current->vieworg, NULL, NULL, end, -1, MASK_SOLID );

	if( tr.fraction < 1.f ) {
		VectorCopy( tr.endpos, speaker.origin );
		VectorMA( speaker.origin, -4.0f, cg.refdef_current->viewaxis[0], speaker.origin );
	}
	else {
		VectorCopy( tr.endpos, speaker.origin );
	}

	if( !BG_SS_StoreSpeaker( &speaker ) ) {
		CG_Printf( S_COLOR_RED "ERROR: Failed to store speaker\n" );
	}
}

static void CG_ModifySpeaker_f( void )
{
	if( cg.editingSpeakers ) {
		CG_ModifyEditSpeaker();
	}
}

static void CG_UndoSpeaker_f( void )
{
	if( cg.editingSpeakers ) {
		CG_UndoEditSpeaker();
	}
}

void CG_ForceTapOut_f( void ) {
	trap_SendClientCommand( "forcetapout" );
}

static void CG_CPM_f( void ) {
	CG_AddPMItem( PM_MESSAGE, CG_Argv(1), cgs.media.voiceChatShader, NULL );
}

qboolean resetmaxspeed = qfalse;

static void CG_ResetMaxSpeed_f(void) {
	resetmaxspeed = qtrue;
}

// quad from etpub: etpro style enemy spawntimer
void CG_TimerSet_f( void )
{
	char buff[32] = {""};

	if (cgs.gamestate != GS_PLAYING) {
		CG_Printf("You may only use this command during the match.\n");
		return;
	}

	if (trap_Argc() == 1) {
		trap_Cvar_Set("cg_spawnTimer_set", "-1");
	}
	else if (trap_Argc() == 2) {
		int spawnPeriod;

		trap_Argv( 1, buff, sizeof(buff) );
		spawnPeriod = atoi(buff);
		if (spawnPeriod < 1 || spawnPeriod > 60) {
			CG_Printf("Argument must be a number between 1 and 60.\n");
		}
		else {
			int msec = (cgs.timelimit * 60.f * 1000.f) - (cg.time - cgs.levelStartTime);

			trap_Cvar_Set("cg_spawnTimer_period", buff);
			trap_Cvar_Set("cg_spawnTimer_set", va("%d", msec/1000));
		}
	}
	else {
			CG_Printf("Usage: timerSet [seconds]\n");
	}
}

// flms from etpub: etpro style timer resetting
void CG_ResetTimer_f(void)
{
	int msec;

	if (cgs.gamestate != GS_PLAYING) {
		CG_Printf("You may only use this command during the match.\n");
		return;
	}

	msec = (cgs.timelimit * 60.f * 1000.f) - (cg.time - cgs.levelStartTime);
	trap_Cvar_Set("cg_spawnTimer_set", va("%d", msec/1000));
}

typedef struct {
	char	*cmd;
	int		hash;	// core: the string hash value for cmd..
	void	(*function)(void);
} consoleCommand_t;


static consoleCommand_t	commands[] =
 {
#ifdef _DEBUG
	{ "testgun",				94918,			CG_TestGun_f },
	{ "testmodel", 				120104,			CG_TestModel_f },
	{ "nextframe", 				119250,			CG_TestModelNextFrame_f },
	{ "prevframe", 				118993,			CG_TestModelPrevFrame_f },
	{ "nextskin",				108280,			CG_TestModelNextSkin_f },
	{ "prevskin",				108023,			CG_TestModelPrevSkin_f },
#endif
	{ "viewpos", 				95296,			CG_Viewpos_f },
	{ "+scores", 				85359,			CG_ScoresDown_f },
	{ "-scores", 				85597,			CG_ScoresUp_f },
	{ "zoomin",					81122,			CG_ZoomIn_f },
	{ "zoomout",				97228,			CG_ZoomOut_f },
	{ "weaplastused",			161593,			CG_LastWeaponUsed_f },
	{ "weapnextinbank", 		188553,			CG_NextWeaponInBank_f },
	{ "weapprevinbank", 		188288,			CG_PrevWeaponInBank_f },
	{ "weapnext",				107352,			CG_NextWeapon_f },
	{ "weapprev",				107087,			CG_PrevWeapon_f },
	{ "weapalt",				91505,			CG_AltWeapon_f },
	{ "weapon",					78975,			CG_Weapon_f },
	{ "weaponbank",				131113,			CG_WeaponBank_f },
	{ "tell_target",			145699,			CG_TellTarget_f },
	{ "tell_attacker",			171855,			CG_TellAttacker_f },
	{ "fade", 					48200,			CG_Fade_f },	// duffy
	{ "loadweapons",			146519,			CG_LoadWeapons_f },

	{ "mp_QuickMessage",		201252,			CG_QuickMessage_f },
	{ "mp_fireteammsg",			186777,			CG_QuickFireteams_f },
	{ "mp_fireteamadmin",		212749,			CG_QuickFireteamAdmin_f },
	{ "wm_sayPlayerClass",		234026,			CG_SayPlayerClass_f },
	{ "wm_ftsayPlayerClass",	263778,			CG_FTSayPlayerClass_f },

	{ "VoiceChat",				116798,			CG_VoiceChat_f },
	{ "VoiceTeamChat",			171536,			CG_TeamVoiceChat_f },
	{ "VoiceFireTeamChat",		227856,			CG_BuddyVoiceChat_f },

	// ydnar: say, teamsay, etc
	{ "messageMode",			144024,			CG_MessageMode_f },
	{ "messageMode2",			150524,			CG_MessageMode_f },
	{ "messageMode3",			150654,			CG_MessageMode_f },
	{ "messageMode4",			150784,			CG_MessageMode_f },
	{ "messageSend",			144661,			CG_MessageSend_f },

	{ "SetWeaponCrosshair",		249328,			CG_SetWeaponCrosshair_f },

	{ "openlimbomenu",			175272,			CG_LimboMenu_f },

	{ "+stats",					73315,			CG_StatsDown_f },
	{ "-stats",					73553,			CG_StatsUp_f },
	{ "+topshots",				116269,			CG_topshotsDown_f },
	{ "-topshots",				116507,			CG_topshotsUp_f },

	// OSP
	{ "autoRecord",				133346,			CG_autoRecord_f },
	{ "autoScreenshot",			191673,			CG_autoScreenShot_f },
	{ "currentTime",			149018,			CG_currentTime_f },
	{ "keyoff",					78230,			CG_keyOff_f },
	{ "keyon",					66566,			CG_keyOn_f },
#ifdef MV_SUPPORT
	{ "mvactivate",				132866,			CG_mvToggleAll_f },
	{ "mvdel",					64837,			CG_mvDelete_f },
	{ "mvhide",					77349,			CG_mvHideView_f },
	{ "mvnew",					67400,			CG_mvNew_f },
	{ "mvshow",					82143,			CG_mvShowView_f },
	{ "mvswap",					81383,			CG_mvSwapViews_f },
	{ "mvtoggle",				106376,			CG_mvToggleView_f },
	{ "spechelp",				104352,			CG_toggleSpecHelp_f },
#endif
	{ "statsdump",				122622,			CG_dumpStats_f },
	{ "+vstr",					61366,			CG_vstrDown_f },
	{ "-vstr",					61604,			CG_vstrUp_f },
	// OSP

	{ "selectbuddy",			145857,			CG_SelectBuddy_f },

	{ "MapZoomIn",				121289,			CG_AutomapZoomIn_f },
	{ "MapZoomOut", 			137782,			CG_AutomapZoomOut_f },
	{ "+mapexpand", 			123893,			CG_AutomapExpandDown_f },
	{ "-mapexpand", 			124131,			CG_AutomapExpandUp_f },

	{ "generateTracemap",		213520,			CG_GenerateTracemap },		// core needed in NQ?
	// xkan, 11/27/2002, toggle automap on/off
	{ "ToggleAutoMap",			175111,			CG_ToggleAutomap_f },

	{ "editSpeakers",			159929,			CG_EditSpeakers_f },		// core needed in NQ?
	{ "dumpSpeaker",			146896,			CG_DumpSpeaker_f },			// IRATA needed ?
	{ "modifySpeaker",			174348,			CG_ModifySpeaker_f },		// IRATA needed ?
	{ "undoSpeaker",			146868,			CG_UndoSpeaker_f },			// core needed in NQ?
	{ "cpm",					38410,			CG_CPM_f },
	{ "forcetapout",			148411,			CG_ForceTapOut_f },
	{ "class",					64653,			CG_Class_f },
	{ "classmenu",				119513,			CG_ClassMenu_f },
	{ "resetmaxspeed",			175152,			CG_ResetMaxSpeed_f },

	{ "+kick", 					55901,			CG_KickDown_f },
	{ "-kick", 					56139,			CG_KickUp_f },

	{ "nqhelp",					78728,			CG_Help_f },

	//quad from etpub: spawntimer
	{ "timerSet",				107438,			CG_TimerSet_f },
	{ "resetTimer",				134853,			CG_ResetTimer_f },
};

unsigned int numConsoleCommands = sizeof(commands) / sizeof(commands[0]);


/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void ) {
	const char	*cmd;
	int			cmdHash;
	int			i;

	// Arnout - don't allow console commands until a snapshot is present
	if ( !cg.snap ) {
		return qfalse;
	}

	cmd = CG_Argv(0);

	// core: create a hash-value for faster "string"-comparing..
	cmdHash = BG_StringHashValue(cmd);

	for ( i=0; i<numConsoleCommands; i++ ) {
		if ( cmdHash == commands[i].hash ) {	// !Q_stricmp(cmd, commands[i].cmd)
			commands[i].function();
			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void ) {
	int	i;
	const char *s;

	for ( i=0; i<numConsoleCommands; ++i ) {
		trap_AddCommand( commands[i].cmd );
	}

	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	trap_AddCommand ("kill");
	trap_AddCommand ("say");
	trap_AddCommand ("give");
	trap_AddCommand ("god");
	trap_AddCommand ("notarget");
	trap_AddCommand ("noclip");
	trap_AddCommand ("setviewpos");
	trap_AddCommand ("nofatigue");
	trap_AddCommand ("listcampaigns");
	trap_AddCommand ("team");
	trap_AddCommand ("follow");
	trap_AddCommand ("callvote");
	trap_AddCommand ("vote");
	trap_AddCommand ("follownext");
	trap_AddCommand ("followprev");
	trap_AddCommand ("start_match");
	trap_AddCommand ("reset_match");
	trap_AddCommand ("swap_teams");
	trap_AddCommand ("?");
	trap_AddCommand ("bottomshots");
	trap_AddCommand ("commands");
	trap_AddCommand ("lock");

#ifdef MV_SUPPORT
	trap_AddCommand ("mvadd");
	trap_AddCommand ("mvaxis");
	trap_AddCommand ("mvallies");
	trap_AddCommand ("mvall");
	trap_AddCommand ("mvnone");
#endif

	trap_AddCommand ("notready");
	trap_AddCommand ("pause");
	trap_AddCommand ("players");
	trap_AddCommand ("readyteam");
	trap_AddCommand ("ready");
	trap_AddCommand ("ref");
	trap_AddCommand ("say_teamnl");
	trap_AddCommand ("say_team");
	trap_AddCommand ("scores");
	trap_AddCommand ("specinvite");
	trap_AddCommand ("speclock");
	trap_AddCommand ("specunlock");
	trap_AddCommand ("statsall");
	trap_AddCommand ("statsdump");
	trap_AddCommand ("timein");
	trap_AddCommand ("timeout");
	trap_AddCommand ("topshots");
	trap_AddCommand ("unlock");
	trap_AddCommand ("unpause");
	trap_AddCommand ("unready");
	trap_AddCommand ("weaponstats");

	trap_AddCommand ("fireteam");
	// implemented ?
	trap_AddCommand ("showstats");

	trap_AddCommand ("ignore");
	trap_AddCommand ("unignore");

	trap_AddCommand ("campaign");

	trap_AddCommand ("setweapons");
	trap_AddCommand ("setclass");

	trap_AddCommand ("imready");
	trap_AddCommand ("say_buddy");
	trap_AddCommand ("setspawnpt");
	trap_AddCommand ("vsay");
	trap_AddCommand ("vsay_buddy");
	trap_AddCommand ("vsay_team");
	trap_AddCommand ("where");
	trap_AddCommand ("dropweapon");
	trap_AddCommand ("dropclip");
	trap_AddCommand ("pm");
	trap_AddCommand ("lua_status");

	// core: silly commands used in anti-recoil scripts..
	trap_RemoveCommand("+lookup");
	trap_RemoveCommand("+lookdown");
	trap_RemoveCommand("-lookup");
	trap_RemoveCommand("-lookdown");

	// IRATA added, THX kw : only allow configstrings command when cheats enabled
	s = Info_ValueForKey( CG_ConfigString(CS_SYSTEMINFO), "sv_cheats" );
	if ( s[0] != '1' ) {
		trap_RemoveCommand("configstrings");
	}
}
