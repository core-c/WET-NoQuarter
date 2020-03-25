/*
 * name:	cg_servercmds.c
 *
 * desc:	reliably sequenced text commands sent by the server
 * 			these are processed at snapshot transition time, so there will definately
 * 			be a valid snapshot this frame
 *
 * NQQS:
 *
 */
#include "cg_local.h"
#include "../game/g_strparse.h"

void CG_LimboMenu_f();

/*
=================
CG_ParseScores

=================
*/
// Gordon: NOTE: team doesnt actually signify team, think i was on drugs that day.....
static void CG_ParseScore( team_t team )
{
	int		i, j, powerups;
	int		numScores;
	int		offset;

	if( team == TEAM_AXIS ) {
		cg.numScores = 0;

		cg.teamScores[0] = atoi( CG_Argv( 1 ) );
		cg.teamScores[1] = atoi( CG_Argv( 2 ) );

		offset = 4;
	}
	else {
		offset = 2;
	}

	numScores = atoi( CG_Argv( offset - 1 ) );

	for(j = 0; j < numScores; j++) {
		i = cg.numScores;

		cg.scores[i].client = atoi(			CG_Argv( offset + 0 + (j*7)));
		cg.scores[i].score = atoi(			CG_Argv( offset + 1 + (j*7)));
		cg.scores[i].ping = atoi(			CG_Argv( offset + 2 + (j*7)));
		cg.scores[i].time = atoi(			CG_Argv( offset + 3 + (j*7)));
		powerups = atoi(					CG_Argv( offset + 4 + (j*7)));
		cg.scores[i].scoreflags	 = atoi(	CG_Argv( offset + 5 + (j*7)));
		cg.scores[i].respawnsLeft = atoi(	CG_Argv( offset + 6 + (j*7)));


		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
			cg.scores[i].client = 0;
		}

		cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
		cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;

		cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;

		cg.numScores++;
	}
}

#define	NUMARGS 6

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo( void )
{
	int		i;
	int		client;
	int		numSortedTeamPlayers = atoi( CG_Argv( 1 ) );

	// core: The old check was limited to max. 32 players per team..
	if( numSortedTeamPlayers < 0 || numSortedTeamPlayers >= MAX_CLIENTS ) {
		CG_Printf( "CG_ParseTeamInfo: numSortedTeamPlayers out of range (%d)", numSortedTeamPlayers );
		return;
	}

	for ( i = 0 ; i < numSortedTeamPlayers ; i++ ) {
		client = atoi( CG_Argv( i * NUMARGS + 2 ) );

		if( client < 0 || client >= MAX_CLIENTS ) {
		  CG_Printf( "CG_ParseTeamInfo: bad client number: %d", client );
		  return;
		}

		cgs.clientinfo[	client ].location[0] =			atoi( CG_Argv( i * NUMARGS + 3 ) );
		cgs.clientinfo[	client ].location[1] =			atoi( CG_Argv( i * NUMARGS + 4 ) );
		cgs.clientinfo[	client ].location[2] =			atoi( CG_Argv( i * NUMARGS + 5 ) );
		cgs.clientinfo[	client ].health =				atoi( CG_Argv( i * NUMARGS + 6 ) );
		cgs.clientinfo[	client ].powerups =				atoi( CG_Argv( i * NUMARGS + 7 ) );
	}
}

/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo( void )
{
	const char	*info = CG_ConfigString( CS_SERVERINFO );
	char	*mapname;

	cg_gameType.integer = cgs.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	cg_antilag.integer = cgs.antilag = atoi( Info_ValueForKey( info, "g_antilag" ) );
	if ( !cgs.localServer ) {
		trap_Cvar_Set("g_gametype", va("%i", cgs.gametype));
		trap_Cvar_Set("g_antilag", va("%i", cgs.antilag));
		trap_Cvar_Update( &cg_antilag );
		trap_Cvar_Update( &cg_gameType );
	}
	cgs.timelimit = atof( Info_ValueForKey( info, "timelimit" ) );
	cgs.maxclients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	mapname = Info_ValueForKey( info, "mapname" );
	Q_strncpyz( cgs.rawmapname, mapname, sizeof(cgs.rawmapname) );
	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );

	// prolly should parse all CS_SERVERINFO keys automagically, but I don't want to break anything that might be improperly set for wolf SP, so I'm just parsing MP relevant stuff here
	trap_Cvar_Set("g_redlimbotime",Info_ValueForKey(info,"g_redlimbotime"));
	cg_redlimbotime.integer = atoi( Info_ValueForKey(info,"g_redlimbotime") );
	trap_Cvar_Set("g_bluelimbotime",Info_ValueForKey(info,"g_bluelimbotime"));
	cg_bluelimbotime.integer = atoi( Info_ValueForKey(info,"g_bluelimbotime") );
	cgs.weaponRestrictions = atoi( Info_ValueForKey( info, "g_heavyWeaponRestriction" ) ) * 0.01f;

	cgs.fixedphysics = atoi(Info_ValueForKey(info, "g_fixedphysics"));
	cgs.fixedphysicsfps = atoi(Info_ValueForKey(info, "g_fixedphysicsfps"));


	// jaquboss moved here instead
	cg_insanity.integer = atoi( Info_ValueForKey( info, "jp_insanity" ));

	cgs.minclients = atoi( Info_ValueForKey( info, "g_minGameClients" ) );		// NERVE - SMF -- OSP: overloaded for ready counts

	// TTimo - make this available for ingame_callvote
	trap_Cvar_Set( "cg_ui_voteFlags", ((authLevel.integer == RL_NONE) ? Info_ValueForKey(info, "voteFlags") : "0"));
}


// server code originaly by Michael
void InitSkillLevelStructure( skillType_t skillType )
{
	char newLevels[MAX_CVAR_VALUE_STRING];
	char * nextLevel;
	int levels[9];
	int count;

	cg.xpOrder = qfalse;

	switch( skillType ) {
		case SK_HEAVY_WEAPONS:
			Q_strncpyz( newLevels, cgs.skillSoldier,
						sizeof(cgs.skillSoldier) );
			break;
		case SK_FIRST_AID:
			Q_strncpyz( newLevels, cgs.skillMedic,
						sizeof(cgs.skillMedic) );
			break;
		case SK_EXPLOSIVES_AND_CONSTRUCTION:
			Q_strncpyz( newLevels, cgs.skillEngineer,
						sizeof(cgs.skillEngineer) );
			break;
		case SK_SIGNALS:
			Q_strncpyz( newLevels, cgs.skillFieldOps,
						sizeof(cgs.skillFieldOps) );
			break;
		case SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS:
			Q_strncpyz( newLevels, cgs.skillCovertOps,
						sizeof(cgs.skillCovertOps) );
			break;
		case SK_BATTLE_SENSE:
			Q_strncpyz( newLevels, cgs.skillBattleSense,
						sizeof(cgs.skillBattleSense) );
			break;
		case SK_LIGHT_WEAPONS:
			Q_strncpyz( newLevels, cgs.skillLightWeapons,
						sizeof(cgs.skillLightWeapons) );
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
		cg.xpOrder = qtrue;
	}

	for (count = 1; count < NUM_SKILL_LEVELS; count++) {
		skillLevels[skillType][count]=levels[count-1];
	}
}

void CG_UpdateSkills( void )
{
	const char *info = CG_ConfigString( CS_SKILLLEVELS );

	Q_strncpyz(cgs.skillSoldier,		Info_ValueForKey(info, "SS"), sizeof(cgs.skillSoldier)		);
	Q_strncpyz(cgs.skillMedic,			Info_ValueForKey(info, "SM"), sizeof(cgs.skillMedic) 		);
	Q_strncpyz(cgs.skillEngineer,		Info_ValueForKey(info, "SE"), sizeof(cgs.skillEngineer)		);
	Q_strncpyz(cgs.skillFieldOps,		Info_ValueForKey(info, "SF"), sizeof(cgs.skillFieldOps)		);
	Q_strncpyz(cgs.skillCovertOps,		Info_ValueForKey(info, "SC"), sizeof(cgs.skillCovertOps)	);
	Q_strncpyz(cgs.skillBattleSense,	Info_ValueForKey(info, "SB"), sizeof(cgs.skillBattleSense)	);
	Q_strncpyz(cgs.skillLightWeapons,	Info_ValueForKey(info, "SL"), sizeof(cgs.skillLightWeapons)	);

	InitSkillLevelStructure(SK_SOLDIER);
	InitSkillLevelStructure(SK_MEDIC);
	InitSkillLevelStructure(SK_ENGINEER);
	InitSkillLevelStructure(SK_FIELDOPS);
	InitSkillLevelStructure(SK_COVERTOPS);
	InitSkillLevelStructure(SK_BATTLE_SENSE);
	InitSkillLevelStructure(SK_LIGHT_WEAPONS);
}

void CG_UpdateForceCvars( void )
{
	const char *info = CG_ConfigString( CS_FORCECVAR );
	int		i;
	char	*token;
	char	*buffer;

	cg.forceCvarCount =  atoi(Info_ValueForKey(info, "NUM"));

	for ( i=0; i<cg.forceCvarCount; i++){
		// get what is it
		buffer = Info_ValueForKey(info, va("FCV%i", i));
		token = strtok(buffer, " ");
		Q_strncpyz(cg.forceCvars[i].cvarName, token, sizeof(cg.forceCvars[0].cvarName));
		token	= strtok(NULL, " ");
		Q_strncpyz(cg.forceCvars[i].cvarValue, token, sizeof(cg.forceCvars[0].cvarValue));
		// do a backup
		Q_strncpyz(cg.cvarBackups[cg.cvarBackupsCount].cvarName, cg.forceCvars[i].cvarName, sizeof(cg.cvarBackups[0].cvarName));
		trap_Cvar_VariableStringBuffer(cg.forceCvars[i].cvarName, cg.cvarBackups[cg.cvarBackupsCount].cvarValue, sizeof(cg.cvarBackups[0].cvarValue));
		cg.cvarBackupsCount++;
	}

}

void CG_UpdateSvCvars( void )
{
	const char *info = CG_ConfigString( CS_SVCVAR );
	int		i;
	char	*token;
	char	*buffer;

	cg.svCvarCount =  atoi(Info_ValueForKey(info, "NUM"));

	for ( i=0; i<cg.svCvarCount; i++){
		// get what is it
		buffer = Info_ValueForKey(info, va("SVCV%i", i));
		// get a mode pf ot
		token = strtok(buffer, " ");
		cg.svCvars[i].mode = atoi(token);

		token	= strtok(NULL, " ");
		Q_strncpyz(cg.svCvars[i].cvarName, token, sizeof(cg.svCvars[0].cvarName));

		token	= strtok(NULL, " ");
		Q_strncpyz(cg.svCvars[i].Val1, token, sizeof(cg.svCvars[0].Val1));

		token	= strtok(NULL, " ");
		if ( token )
		Q_strncpyz(cg.svCvars[i].Val2, token, sizeof(cg.svCvars[0].Val2));


		// do a backup
		Q_strncpyz(cg.cvarBackups[cg.cvarBackupsCount].cvarName, cg.svCvars[i].cvarName, sizeof(cg.cvarBackups[0].cvarName));
		trap_Cvar_VariableStringBuffer(cg.svCvars[i].cvarName, cg.cvarBackups[cg.cvarBackupsCount].cvarValue, sizeof(cg.cvarBackups[0].cvarValue));
		cg.cvarBackupsCount++;
	}

}

void CG_ParseNQinfo( void )
{
	const char *info = CG_ConfigString( CS_NOQUARTERINFO );

	// Jaybird
	cgs.jp_keepAwards = atoi(Info_ValueForKey(CG_ConfigString( CS_SERVERINFO ), "g_skills"));

	cgs.doubleJump = atoi(Info_ValueForKey(info, "D")); // use shortcuts instead
	cgs.doubleJumpHeight = atof(Info_ValueForKey(info, "DH"));

	// weapon team limits
	cgs.panzerRestrictions = atoi( Info_ValueForKey( info, "T1" ) ) * 0.01f;
	cgs.maxPanzers = atoi(Info_ValueForKey(info, "T2"));
	cgs.maxMG42s = atoi(Info_ValueForKey(info, "T3"));
	cgs.maxFlamers = atoi(Info_ValueForKey(info, "T4"));
	cgs.maxMortars = atoi(Info_ValueForKey(info, "T5"));
	cgs.maxGrenLaunchers = atoi(Info_ValueForKey(info, "T6"));
	cgs.maxVenoms = atoi(Info_ValueForKey(info, "T7"));
	cgs.maxShotguns = atoi(Info_ValueForKey(info, "T8"));
	cgs.maxLMG = atoi(Info_ValueForKey(info, "T9"));
	cgs.maxScopeguns = atoi(Info_ValueForKey(info, "T0"));
	cgs.maxIsPercentage = atoi(Info_ValueForKey(info, "P"));

	cgs.maxSoldiers  = atoi(Info_ValueForKey(info, "MS"));
	cgs.maxMedics	 = atoi(Info_ValueForKey(info, "MM"));
	cgs.maxEngineers = atoi(Info_ValueForKey(info, "ME"));
	cgs.maxFieldops  = atoi(Info_ValueForKey(info, "MF"));
	cgs.maxCovertops = atoi(Info_ValueForKey(info, "MC"));


#ifdef MV_SUPPORT
	cgs.mvAllowed = atoi(Info_ValueForKey(info, "MV"));
#endif

	cgs.spectator = atoi(Info_ValueForKey(info, "S"));
    // Clear names if disabled
    if((cgs.spectator & SPEC_SHOWNAMES) == 0) {
         memset(&cg.specOnScreenNames, 0, sizeof(cg.specOnScreenNames));
    }

	cgs.realism = atoi(Info_ValueForKey(info, "R" ));
	cgs.adrenaline = atoi(Info_ValueForKey(info, "A" ));
	cgs.medics = atoi(Info_ValueForKey(info, "M" ));

	// MAPVOTE
	if (cgs.gametype == GT_WOLF_MAPVOTE ){
		cgs.mapVoteMapX = atoi(Info_ValueForKey(info, "X"));
		cgs.mapVoteMapY = atoi(Info_ValueForKey(info, "Y"));
	}

	// Reload client data for weapons if changed
	{
		qboolean modifed=qfalse;
		char	oldDir[MAX_CVAR_VALUE_STRING];

		Q_strncpyz(oldDir, cgs.weaponScriptsDir, MAX_CVAR_VALUE_STRING );

		Q_strncpyz(cgs.weaponScriptsDir, Info_ValueForKey(info, "W" ), MAX_CVAR_VALUE_STRING );

		if ( Q_stricmp(cgs.weaponScriptsDir, oldDir) != 0 ){
			modifed = qtrue;
		}

		if ( modifed ) {
			int i;

			for( i = WP_NONE; i < WP_NUM_WEAPONS; i++ ) {
				BG_RegisterWeapon( i, qtrue );
			}
		}
	}

}

/*
==================
core:
CG_ParseCSMethodInfo

returns the total of new configstrings available..
==================
*/
int C_ParseCSMethodInfo( void )
{
	const char *info = CG_ConfigString( CS_CSMETHODINFO );
	int total = 0;

	// core: new configstring handling..
	sscanf( info, "%i %i %i %i %i",
		&cgs.csMethod_Models, &cgs.csMethod_Shaders, &cgs.csMethod_Skins,
		&cgs.csMethod_Characters, &cgs.csMethod_Sounds );

	total = cgs.csMethod_Models + cgs.csMethod_Shaders + cgs.csMethod_Skins +
			cgs.csMethod_Characters + cgs.csMethod_Sounds;

	cgs.csMethod = (total == 0)? 0 : 1;

	return total;
}


/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup( void )
{
	const char	*info = CG_ConfigString(CS_WARMUP);
	int			warmup = atoi(info);

	if(warmup == 0 && cg.warmup) {

	}
	else if(warmup > 0 && cg.warmup <= 0 && cgs.gamestate != GS_WARMUP) {

		//CG_AddAnnouncer("PREPARE TO FIGHT!", cgs.media.countPrepare, 0.85f, 1500, 1.0f, 1.0f, 0.0f, ANNOUNCER_NORMAL);

		if(!cg.demoPlayback && cg_autoAction.integer & AA_DEMORECORD) {
			CG_autoRecord_f();
		}

		if(cg.warmupCount >= 0) {
			Pri("^3All players ready!^7\nMatch starting...\n");
			CPri("^3All players ready!^7\nMatch starting...");
		}
	}

	if(cgs.gamestate != GS_WARMUP || cg.warmup > 0) {
		cg.warmup = warmup;
	}

	cg.warmupCount++;
}

/*
==================
CG_ParseOIDInfo
==================
*/

oidInfo_t* CG_OIDInfoForEntityNum( int num )
{
	int i;

	for( i = 0; i < MAX_OID_TRIGGERS; i++ ) {
		if( cgs.oidInfo[ i ].entityNum == num ) {
			return &cgs.oidInfo[ i ];
		}
	}
	return NULL;
}

void CG_ParseOIDInfo( int num )
{
	const char* info = CG_ConfigString( num );
	const char* cs;
	int index = num - CS_OID_DATA;

	memset( &cgs.oidInfo[ index ], 0, sizeof( cgs.oidInfo[ 0 ] ) );

	if( !info || !*info ) {
		return;
	}

	cs = Info_ValueForKey( info, "s" );
	if( cs && *cs ) {
		cgs.oidInfo[ index ].spawnflags = atoi( cs );
	}

	cs = Info_ValueForKey( info, "a" ); // "cia"
	if( cs && *cs ) {
		cgs.oidInfo[ index ].customimageallies = cgs.gameShaders[atoi( cs )];
	}

	cs = Info_ValueForKey( info, "b" ); // "cix"
	if( cs && *cs ) {
		cgs.oidInfo[ index ].customimageaxis = cgs.gameShaders[atoi( cs )];
	}

	cs = Info_ValueForKey( info, "e" );
	if( cs && *cs ) {
		cgs.oidInfo[ index ].entityNum = atoi( cs );
	}

	cs = Info_ValueForKey( info, "n" );
	if( cs && *cs ) {
		Q_strncpyz( cgs.oidInfo[ index ].name, cs, sizeof( cgs.oidInfo[ 0 ].name ) );
	}

	cs = Info_ValueForKey( info, "x" );
	if( cs && *cs ) {
		cgs.oidInfo[ index ].origin[0] = atoi( cs );
	}

	cs = Info_ValueForKey( info, "y" );
	if( cs && *cs ) {
		cgs.oidInfo[ index ].origin[1] = atoi( cs );
	}

	cs = Info_ValueForKey( info, "z" );
	if( cs && *cs ) {
		cgs.oidInfo[ index ].origin[2] = atoi( cs );
	}
}

void CG_ParseOIDInfos( void )
{
	int i;

	for( i = 0; i < MAX_OID_TRIGGERS; i++ ) {
		CG_ParseOIDInfo( CS_OID_DATA + i );
	}
}

/*
==================
CG_ParseWolfinfo

NERVE - SMF
==================
*/
void CG_ParseWolfinfo( void )
{

	int 			old_gs 						= cgs.gamestate;
	const char*		info 						= CG_ConfigString( CS_WOLFINFO );
	char			key[MAX_STRING_CHARS]		= "";
	char			value[MAX_STRING_CHARS]		= "";

	//CG_Printf("CG_ParseWolfinfo(1)\n");

	// Retrieve all the key/value pairs from the string
	while(1)
	{
		g_StringToken_t token;
		// Get next key/value pair
		Info_NextPair(&info, key, value );
		// Empty string for key and we exit...
		if ( key[0] == 0 )
			break;
		token = G_GetTokenForString(key);
		switch(token)
		{
		case TOK_g_currentRound:
			cgs.currentRound		= atoi(value);
			break;
		case TOK_g_nextTimeLimit:
			cgs.nextTimeLimit 		= atof(value);
			break;
		case TOK_gamestate:
			cgs.gamestate  			= atoi(value);
			break;
		case TOK_g_currentCampaign:
			Q_strncpyz(cgs.currentCampaign, value, sizeof(cgs.currentCampaign));
			break;
		case TOK_g_currentCampaignMap:
			cgs.currentCampaignMap 	= atoi(value);
			break;
		default:
			continue;
		}
	}

	// OSP - Announce game in progress if we are really playing
	if(old_gs != GS_PLAYING && cgs.gamestate == GS_PLAYING) {
		CG_AddAnnouncer("FIGHT!", "", cgs.media.countFight, 1.1f, 1500, 1.0f, 0.0f, 0.0f, ANNOUNCER_NORMAL );
	}

	if(!cgs.localServer) {
		trap_Cvar_Set( "gamestate", va( "%i", cgs.gamestate ) );
	}

	if(old_gs != GS_WARMUP_COUNTDOWN && cgs.gamestate == GS_WARMUP_COUNTDOWN) {
		CG_ParseWarmup();
	}
}

/*
==================
CG_ParseSpawns
==================
*/
void CG_ParseSpawns( void )
{
	const char *info = CG_ConfigString( CS_MULTI_INFO );
	const char *s = Info_ValueForKey( info, "n" );		//numspawntargets
	int i;
	int newteam;

	if ( !s || !strlen( s ) )
		return;

	// first index is for autopicking
	Q_strncpyz( cg.spawnPoints[0], CG_TranslateString( "Auto Pick" ), MAX_SPAWNDESC );

	cg.spawnCount = atoi( s ) + 1;

	for ( i = 1; i < cg.spawnCount; i++ ) {
		info = CG_ConfigString( CS_MULTI_SPAWNTARGETS + i - 1 );

		s = Info_ValueForKey( info, "s" ); // "spawn_targ"

		if ( !s || !strlen( s ) )
			return;

		Q_strncpyz( cg.spawnPoints[i], CG_TranslateString( s ), MAX_SPAWNDESC );

		s = Info_ValueForKey( info, "x" );
		if ( !s || !strlen( s ) )
			return;
		cg.spawnCoordsUntransformed[i][0] = cg.spawnCoords[i][0] = atof( s );

		s = Info_ValueForKey( info, "y" );
		if ( !s || !strlen( s ) )
			return;
		cg.spawnCoordsUntransformed[i][1] =cg.spawnCoords[i][1] = atof( s );

		if( cgs.ccLayers ) {
			s = Info_ValueForKey( info, "z" );
			if ( !s || !strlen( s ) )
			return;
			cg.spawnCoordsUntransformed[i][2] = cg.spawnCoords[i][2] = atof( s );
		}

		CG_TransformToCommandMapCoord( &cg.spawnCoords[i][0], &cg.spawnCoords[i][1] );

		s = Info_ValueForKey( info, "t" );

		newteam = atoi(s);
		if(cg.spawnTeams[i] != newteam) {
			cg.spawnTeams_old[i] = cg.spawnTeams[i];
			cg.spawnTeams_changeTime[i] = cg.time;
			cg.spawnTeams[i] = newteam;
		}

		s = Info_ValueForKey( info, "c" );
		cg.spawnPlayerCounts[i] = atoi(s);
	}
}

/*
=====================
CG_ParseScreenFade
=====================
*/
static void CG_ParseScreenFade( void )
{
	const char	*info = CG_ConfigString( CS_SCREENFADE );
	char *token = COM_Parse( (char **)&info );

	cgs.fadeAlpha = atof( token );

	token = COM_Parse( (char **)&info );
	cgs.fadeStartTime = atoi( token );
	token = COM_Parse( (char **)&info );
	cgs.fadeDuration = atoi( token );

	if (cgs.fadeStartTime + cgs.fadeDuration < cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	}
}


/*
==============
CG_ParseFog
	float near dist
	float far dist
	float density
	float[3] r,g,b
	int		time
==============
*/
static void CG_ParseFog( void )
{
	const char	*info = CG_ConfigString( CS_FOGVARS );
	char *token;
	float	ne, fa, r, g, b, density;
	int		time;

	token = COM_Parse( (char **)&info );	ne = atof(token);
	token = COM_Parse( (char **)&info );	fa = atof(token);
	token = COM_Parse( (char **)&info );	density = atof(token);
	token = COM_Parse( (char **)&info );	r = atof(token);
	token = COM_Parse( (char **)&info );	g = atof(token);
	token = COM_Parse( (char **)&info );	b = atof(token);
	token = COM_Parse( (char **)&info );	time = atoi(token);

	if(fa) {	// far of '0' from a target_fog means "return to map fog"
		trap_R_SetFog(FOG_SERVER, (int)ne, (int)fa, r, g, b, density+.1);
		trap_R_SetFog(FOG_CMD_SWITCHFOG, FOG_SERVER, time, 0, 0, 0, 0);
	}
	else {
		trap_R_SetFog(FOG_CMD_SWITCHFOG, FOG_MAP, time, 0, 0, 0, 0);
	}
}

static void CG_ParseGlobalFog( void )
{
	const char	*info = CG_ConfigString( CS_GLOBALFOGVARS );
	char		*token;
	qboolean	restore;
	int			duration;

	token = COM_Parse( (char **)&info );	restore = atoi(token);
	token = COM_Parse( (char **)&info );	duration = atoi(token);

	if( restore ) {
		trap_R_SetGlobalFog( qtrue, duration, 0.f, 0.f, 0.f, 0 );
	}
	else {
		float		r, g, b, depthForOpaque;

    	token = COM_Parse( (char **)&info );	r = atof(token);
		token = COM_Parse( (char **)&info );	g = atof(token);
		token = COM_Parse( (char **)&info );	b = atof(token);
		token = COM_Parse( (char **)&info );	depthForOpaque = atof(token);

		trap_R_SetGlobalFog( qfalse, duration, r, g, b, depthForOpaque );
	}
}

// Parse reinforcement offsets
void CG_ParseReinforcementTimes(const char *pszReinfSeedString)
{
	const char *tmp = pszReinfSeedString, *tmp2;
	unsigned int i, j, dwDummy, dwOffset[TEAM_NUM_TEAMS];

#define GETVAL(x,y) if((tmp = strchr(tmp, ' ')) == NULL) return; x = atoi(++tmp)/y;

	dwOffset[TEAM_ALLIES] = atoi(pszReinfSeedString) >> REINF_BLUEDELT;
	GETVAL(dwOffset[TEAM_AXIS], (1 << REINF_REDDELT));
	tmp2 = tmp;

	for(i=TEAM_AXIS; i<=TEAM_ALLIES; i++) {
		tmp = tmp2;
		for(j=0; j<MAX_REINFSEEDS; j++) {
			if(j == dwOffset[i]) {
				GETVAL(cgs.aReinfOffset[i], aReinfSeeds[j]);
				cgs.aReinfOffset[i] *= 1000;
				break;
			}
			GETVAL(dwDummy, 1);
		}
	}
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues( void )
{
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );
	cgs.intermissionStartTime = atoi( CG_ConfigString( CS_INTERMISSION_START_TIME ) );
	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );

	// rain - set all of this crap in cgs - it won't be set if it doesn't
	// change, otherwise.  consider:
	// vote was called 5 minutes ago for 'Match Reset'.  you connect.
	// you're sent that value for CS_VOTE_STRING, but ignore it, so
	// you have nothing to use if another 'Match Reset' vote is called
	// (no update will be sent because the string will be the same.)

	cgs.voteTime = atoi(CG_ConfigString(CS_VOTE_TIME));
	cgs.voteYes = atoi(CG_ConfigString(CS_VOTE_YES));
	cgs.voteNo = atoi(CG_ConfigString(CS_VOTE_NO));
	Q_strncpyz(cgs.voteString, CG_ConfigString(CS_VOTE_STRING), sizeof(cgs.voteString));

	cg.teamFirstBlood = atoi(CG_ConfigString(CS_FIRSTBLOOD));
	// rain - yes, the order is this way on purpose. not my fault!
	cg.teamWonRounds[1] = atoi(CG_ConfigString(CS_ROUNDSCORES1));
	cg.teamWonRounds[0] = atoi(CG_ConfigString(CS_ROUNDSCORES2));

	// OSP
	CG_ParseReinforcementTimes(CG_ConfigString(CS_REINFSEEDS));
	// OSP
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged(void)
{
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o = CG_ConfigString( CS_SHADERSTATE );
	char *n,*t;


	while (o && *o) {
		n = strstr(o, "=");
		if (n && *n) {
			strncpy(originalShader, o, n-o);
			originalShader[n-o] = 0;
			n++;
			t = strstr(n, ":");
			if (t && *t) {
				strncpy(newShader, n, t-n);
				newShader[t-n] = 0;
			}
			else {
				break;
			}
			t++;
			o = strstr(t, "@");
			if (o) {
				strncpy(timeOffset, t, o-t);
				timeOffset[o-t] = 0;
				o++;
				trap_R_RemapShader( cgs.gameShaderNames[atoi(originalShader)],
									cgs.gameShaderNames[atoi(newShader)],
									timeOffset );
			}
		}
		else {
			break;
		}
	}
}

/*
===============
CG_ChargeTimesChanged
===============
*/
void CG_ChargeTimesChanged( void ) {
	const char *info = CG_ConfigString( CS_CHARGETIMES );

	cg.soldierChargeTime[0] = atoi(Info_ValueForKey( info, "s" ));
	cg.soldierChargeTime[1] = atoi(Info_ValueForKey( info, "o" ));
	cg.medicChargeTime[0] = atoi(Info_ValueForKey( info, "m" ));
	cg.medicChargeTime[1] = atoi(Info_ValueForKey( info, "i" ));
	cg.engineerChargeTime[0] = atoi(Info_ValueForKey( info, "e" ));
	cg.engineerChargeTime[1] = atoi(Info_ValueForKey( info, "d" ));
	cg.ltChargeTime[0] = atoi(Info_ValueForKey( info, "f" ));
	cg.ltChargeTime[1] = atoi(Info_ValueForKey( info, "l" ));
	cg.covertopsChargeTime[0] = atoi(Info_ValueForKey( info, "c" ));
	cg.covertopsChargeTime[1] = atoi(Info_ValueForKey( info, "v" ));
}

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified( void )
{
	const char	*str;
	int		num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap_GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	switch(num) {
		case CS_FORCECVAR:
			CG_UpdateForceCvars();
			break;
		case CS_SVCVAR:
			CG_UpdateSvCvars();
			break;
		case CS_SERVERINFO:
			CG_ParseServerinfo();
			break;
		case CS_NOQUARTERINFO:
			CG_ParseNQinfo();
			break;
		case CS_WOLFINFO:
			CG_ParseWolfinfo();
			break;
		case CS_LEVEL_START_TIME:
			cgs.levelStartTime = atoi( str );
			break;
		case CS_INTERMISSION_START_TIME:
			cgs.intermissionStartTime = atoi( str );
			break;
		case CS_WARMUP:
			CG_ParseWarmup();
			break;
		case CS_INTERMISSION:
			cg.intermissionStarted = atoi( str );
			break;
		case CS_REINFSEEDS:
			CG_ParseReinforcementTimes(str);		// OSP - set reinforcement times for each team
			break;
		case CS_FIRSTBLOOD:
			cg.teamFirstBlood = atoi( str );
			break;
		case CS_ROUNDSCORES1:
			cg.teamWonRounds[1] = atoi( str );
			break;
		case CS_ROUNDSCORES2:
			cg.teamWonRounds[0] = atoi( str );
			break;
		case CS_VOTE_TIME:
			cgs.voteTime = atoi( str );
			cgs.voteModified = qtrue;
			break;
		case CS_VOTE_YES:
			cgs.voteYes = atoi( str );
			cgs.voteModified = qtrue;
			break;
		case CS_VOTE_NO:
			cgs.voteNo = atoi( str );
			cgs.voteModified = qtrue;
			break;
		case CS_VOTE_STRING:
			Q_strncpyz( cgs.voteString, str, sizeof( cgs.voteString ) );
			break;
		case CS_SCREENFADE:
			CG_ParseScreenFade();
			break;
		case CS_FOGVARS:
			CG_ParseFog();
			break;
		case CS_GLOBALFOGVARS:
			CG_ParseGlobalFog();
			break;
		case CS_SKYBOXORG:
			CG_ParseSkyBox();
			break;
		case CS_ALLIED_MAPS_XP:
		case CS_AXIS_MAPS_XP:
			CG_ParseTeamXPs( num - CS_AXIS_MAPS_XP );
			break;
		case CS_FILTERCAMS:
			cg.filtercams = atoi( str ) ? qtrue : qfalse;
			break;
		case CS_SKILLLEVELS:
			CG_UpdateSkills();
			break;
		case CS_CHARGETIMES:
			CG_ChargeTimesChanged();
			break;
		case CS_MUSIC:
			CG_StartMusic();
			break;
		case CS_MUSIC_QUEUE:
			CG_QueueMusic();
			break;
		case CS_CSMETHODINFO:
			C_ParseCSMethodInfo();
			break;
		case CS_SHADERSTATE:
			CG_ShaderStateChanged();
			break;
		default:
			if( num >= CS_PLAYERS && num < CS_PLAYERS+MAX_CLIENTS ) {
				CG_NewClientInfo( num - CS_PLAYERS );
			}
			else
			if ( num >= CS_MULTI_SPAWNTARGETS && num < CS_MULTI_SPAWNTARGETS + MAX_MULTI_SPAWNTARGETS ) {
				CG_ParseSpawns();
			//} else if( num >= CS_DLIGHTS && num < CS_DLIGHTS+MAX_DLIGHT_CONFIGSTRINGS) {
			//		FIXME - dlight changes ignored!
			}
			else
			if( num >= CS_FIRETEAMS && num < CS_FIRETEAMS+MAX_FIRETEAMS ) {
				CG_ParseFireteams();
			}
			else
			if( num >= CS_TAGCONNECTS && num < CS_TAGCONNECTS + MAX_TAGCONNECTS ) {
				CG_ParseTagConnect( num );
			}
			else
			if( num >= CS_OID_DATA && num < CS_OID_DATA + MAX_OID_TRIGGERS ) {
				CG_ParseOIDInfo( num );
			}
			else
			// core: csMethod 0 values.. the original way.
			if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) {
				cgs.gameModels[ num-CS_MODELS ] = trap_R_RegisterModel( str );
			}
			else
			if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) {
				if ( str[0] != '*' ) {  // player specific sounds don't register here
					// Ridah, register sound scripts seperately
					if (!strstr(str, ".wav")) {
						CG_SoundScriptPrecache( str );
					}
					else {
						cgs.gameSounds[ num-CS_SOUNDS] = trap_S_RegisterSound( str, qfalse );   //FIXME: add a compress flag?
					}
				}
			}
			else
			if( num >= CS_SHADERS && num < CS_SHADERS + MAX_CS_SHADERS ) {
				cgs.gameShaders[ num - CS_SHADERS ] = str[0] == '*' ? trap_R_RegisterShader( str + 1 ) : trap_R_RegisterShaderNoMip( str );
				Q_strncpyz( cgs.gameShaderNames[num - CS_SHADERS], str[0] == '*' ? str + 1 : str, MAX_QPATH );
			}
			else
			if( num >= CS_SKINS && num < CS_SKINS+MAX_CS_SKINS ) {
				cgs.gameModelSkins[ num-CS_SKINS ] = trap_R_RegisterSkin( str );
			}
			else
			if( num >= CS_CHARACTERS && num < CS_CHARACTERS+MAX_CHARACTERS ) {
				if( !BG_FindCharacter( str ) ) {
					cgs.gameCharacters[ num - CS_CHARACTERS ] = BG_FindFreeCharacter( str );
					Q_strncpyz( cgs.gameCharacters[ num - CS_CHARACTERS ]->characterFile, str, sizeof(cgs.gameCharacters[ num - CS_CHARACTERS ]->characterFile) );
					if( !CG_RegisterCharacter( str, cgs.gameCharacters[ num - CS_CHARACTERS ] ) ) {
						CG_Error( "ERROR: CG_ConfigStringModified: failed to load character file '%s'\n", str );
					}
				}
			}
			break;
	}
}

/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat( const char *str, int clientnum, qboolean disguise )
{
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT) {
		chatHeight = cg_teamChatHeight.integer;
	}
	else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if (chatHeight <= 0 || cg_teamChatTime.integer <= 0) {
		// team chat disabled, dump into normal chat
		cgs.teamChatPos = cgs.teamLastChatPos = 0;
		return;
	}

	len = 0;

	p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while (*str) {
		if (len > TEAMCHAT_WIDTH - 1) {
			if (ls) {
				str -= (p - ls);
				str++;
				p -= (p - ls);
			}
			*p = 0;

			cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
			cgs.teamChatMsgTeams[cgs.teamChatPos % chatHeight] = cgs.clientinfo[ clientnum ].team;

			if (disguise)
				cgs.teamChatMsgTeams[cgs.teamChatPos % chatHeight] = cgs.clientinfo[ clientnum ].team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;


			cgs.teamChatPos++;
			p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if (*str == ' ') {
			ls = p;
		}
		*p++ = *str++;
		len++;
	}
	*p = 0;

	cgs.teamChatMsgTeams[cgs.teamChatPos % chatHeight] = cgs.clientinfo[ clientnum ].team;

	if (disguise)
		cgs.teamChatMsgTeams[cgs.teamChatPos % chatHeight] = cgs.clientinfo[ clientnum ].team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

	cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
	cgs.teamChatPos++;

	if (cgs.teamChatPos - cgs.teamLastChatPos > chatHeight)
		cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
}

/*
=======================
CG_AddToNotify

=======================
*/
void CG_AddToNotify( const char *str )
{
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight = NOTIFY_HEIGHT;
	float	notifytime;
	char var[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) );
	notifytime = atof( var ) * 1000;

	if (chatHeight <= 0 || notifytime <= 0) {
		// team chat disabled, dump into normal chat
		cgs.notifyPos = cgs.notifyLastPos = 0;
		return;
	}

	len = 0;

	p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while (*str) {
		if (len > NOTIFY_WIDTH - 1 || (*str == '\n' && (*(str + 1) != 0)) ) {
			if (ls) {
				str -= (p - ls);
				str++;
				p -= (p - ls);
			}
			*p = 0;

			cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;

			cgs.notifyPos++;
			p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if (*str == ' ') {
			ls = p;
		}
		while (*str == '\n') {
      // TTimo gcc warning: value computed is not used
      // was *str++;
			str++;
		}

		if ( *str ) {
			*p++ = *str++;
			len++;
		}
	}
	*p = 0;

	cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;
	cgs.notifyPos++;

	if (cgs.notifyPos - cgs.notifyLastPos > chatHeight)
		cgs.notifyLastPos = cgs.notifyPos - chatHeight;
}

/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart( void )
{
	if ( cg_showmiss.integer ) {
		CG_Printf( "CG_MapRestart\n" );
	}

	memset(&cg.lastWeapSelInBank[0], 0, MAX_WEAP_BANKS_MP * sizeof(int));	// clear weapon bank selections

	cg.numbufferedSoundScripts = 0;

	cg.centerPrintTime = 0;	// reset centerprint counter so previous messages don't re-appear
	cg.cursorHintFade = 0;	// reset cursor hint timer

	// DHM - Nerve :: Reset complaint system
	cgs.complaintClient = -1;
	cgs.complaintEndTime = 0;

	CG_LimboPanel_RequestObjective();

	// (SA) clear zoom (so no warpies)
	cg.zoomedBinoc	= qfalse;
	cg.zoomedScope	= qfalse;
	cg.zoomTime		= 0;
	cg.zoomval		= 0;

	cg.leanTime		= 0;

	cg.idleAnim		= 0;
	cg.nextIdleTime = 0;
	cg.painTime		= 0;

	cgs.complaintEndTime = 0;
	cgs.invitationEndTime = 0;
	cgs.applicationEndTime = 0;
	cgs.propositionEndTime = 0;
	cgs.autoFireteamEndTime = 0;
	cgs.autoFireteamCreateEndTime = 0;

	// reset fog to world fog (if present)
	trap_R_SetFog(FOG_CMD_SWITCHFOG, FOG_MAP,20,0,0,0,0);

	// clear pmext
	memset( &cg.pmext, 0, sizeof(cg.pmext) );

	cg.pmext.bAutoReload = (cg_autoReload.integer > 0);
	cg.pmext.weapAltReload = (cg_weapAltReloads.integer > 0);

	numSplinePaths = 0;
	numPathCorners = 0;
	cg.numOIDtriggers2 = 0;

	cgs.fadeStartTime = 0;
	cgs.fadeAlpha = 0;
	trap_Cvar_Set( "cg_letterbox", "0" );

	// kw: reset gamestate
	cgs.gamestate = GS_INITIALIZE;

	CG_ParseWolfinfo();

	CG_ParseEntitiesFromString();

	CG_LoadObjectiveData();

	CG_InitLocalEntities();
	CG_InitMarkPolys();

	cg.editingSpeakers = qfalse;

	BG_BuildSplinePaths();

	InitSmokeSprites();

	//Rafael particles
	CG_ClearParticles ();

	// Ridah
	CG_ClearFlameChunks ();
	CG_SoundInit ();
	// done.

	cg.intermissionStarted = qfalse;
	cg.lightstylesInited = qfalse;
	cg.mapRestart = qtrue;
	cg.timelimitWarnings = 0;
	cgs.voteTime = 0;
	cgs.dumpStatsTime = 0;

	CG_StartMusic();

	trap_S_ClearLoopingSounds();
	trap_S_ClearSounds( qfalse );

	// ydnar
	trap_R_ClearDecals();

	cg.latchAutoActions = qfalse;
	cg.latchVictorySound = qfalse;			// NERVE - SMF

	// we really should clear more parts of cg here and stop sounds
	cg.v_dmg_time = 0;
	cg.v_noFireTime = 0;
	cg.v_fireTime = 0;

	cg.filtercams = atoi( CG_ConfigString( CS_FILTERCAMS ) ) ? qtrue : qfalse;

	CG_ChargeTimesChanged();
	CG_ParseFireteams();
	CG_ParseOIDInfos();
	CG_InitPM();
	CG_InitMM();
	CG_ParseSpawns();
	CG_ParseTagConnects();

	trap_Cvar_Set("cg_thirdPerson", "0");
}
// NERVE - SMF


#define MAX_VOICEFILES		8

// TAT - 10/28/2002 we've got some really big VO files now
#define MAX_VOICEFILESIZE	32768
#define MAX_VOICECHATS		272
// TAT - NOTE: If we're worried about space - do we really need 96 possible sounds for any one chat?
//			I think this is used to allow multiple sound clips for one command, so do we need 96 available selection sounds?
#define MAX_VOICESOUNDS		32

#define MAX_CHATSIZE		64
// #define MAX_HEADMODELS		64 // never used

typedef struct voiceChat_s
{
	char id[64];
	int numSounds;
	sfxHandle_t sounds[MAX_VOICESOUNDS];
	char chats[MAX_VOICESOUNDS][MAX_CHATSIZE];
	qhandle_t	sprite[MAX_VOICESOUNDS];		// DHM - Nerve
} voiceChat_t;

typedef struct voiceChatList_s
{
	char name[64];
	int gender;
	int numVoiceChats;
	voiceChat_t voiceChats[MAX_VOICECHATS];
} voiceChatList_t;

/* never used
typedef struct headModelVoiceChat_s
{
	char headmodel[64];
	int voiceChatNum;
} headModelVoiceChat_t;
*/

voiceChatList_t voiceChatLists[MAX_VOICEFILES];
// headModelVoiceChat_t headModelVoiceChat[MAX_HEADMODELS]; // never used

/*
=================
CG_ParseVoiceChats
=================
*/
int CG_ParseVoiceChats( const char *filename, voiceChatList_t *voiceChatList, int maxVoiceChats )
{
	int	len, i;
	int	current = 0;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;
	voiceChat_t *voiceChats;
	qboolean compress = qtrue;

	if( cg_buildScript.integer ) {
		compress = qfalse;
	}

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "voice chat file not found: %s\n", filename ) );
		return qfalse;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return qfalse;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	Com_sprintf(voiceChatList->name, sizeof(voiceChatList->name), "%s", filename);
	voiceChats = voiceChatList->voiceChats;
	for ( i = 0; i < maxVoiceChats; i++ ) {
		voiceChats[i].id[0] = 0;
	}
	token = COM_ParseExt(p, qtrue);
	if (!token || token[0] == 0) {
		return qtrue;
	}
	if (!Q_stricmp(token, "female")) {
		voiceChatList->gender = GENDER_FEMALE;
	}
	else if (!Q_stricmp(token, "male")) {
		voiceChatList->gender = GENDER_MALE;
	}
	else if (!Q_stricmp(token, "neuter")) {
		voiceChatList->gender = GENDER_NEUTER;
	}
	else {
		trap_Print( va( S_COLOR_RED "expected gender not found in voice chat file: %s\n", filename ) );
		return qfalse;
	}

	// Gordon: setting before call so we can load multiple files into one list
	// TAT - 10/28/2002 - if you really want to be able to load multiple files, you should take out the loop
	//		above that clears out all the commands "voiceChats[i].id[0] = 0;"
	//		We don't even want the MP voice chats in SP, so no need anyway
	voiceChatList->numVoiceChats = 0;
	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);
		if (!token || token[0] == 0) {
			return qtrue;
		}

		Com_sprintf(voiceChats[voiceChatList->numVoiceChats].id, sizeof( voiceChats[voiceChatList->numVoiceChats].id ), "%s", token);
		token = COM_ParseExt(p, qtrue);
		if (Q_stricmp(token, "{")) {
			trap_Print( va( S_COLOR_RED "Expected { found %s in voice chat file: %s\n", token, filename ) );
			return qfalse;
		}
		voiceChats[voiceChatList->numVoiceChats].numSounds = 0;
		current = voiceChats[voiceChatList->numVoiceChats].numSounds;

		while(1) {
			token = COM_ParseExt(p, qtrue);
			if (!token || token[0] == 0) {
				return qtrue;
			}
			if (!Q_stricmp(token, "}"))
				break;
			voiceChats[voiceChatList->numVoiceChats].sounds[current] = trap_S_RegisterSound( token, compress );
			token = COM_ParseExt(p, qtrue);
			if (!token || token[0] == 0) {
				return qtrue;
			}
			Com_sprintf(voiceChats[voiceChatList->numVoiceChats].chats[current], MAX_CHATSIZE, "%s", token);

			// DHM - Nerve :: Specify sprite shader to show above player's head
			token = COM_ParseExt(p, qfalse);
			if (!Q_stricmp(token, "}") || !token || token[0] == 0) {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
				COM_RestoreParseSession( p );
			}
			else {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( token );
				if ( voiceChats[voiceChatList->numVoiceChats].sprite[current] == 0 )
					voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
			}
			// dhm - end

			voiceChats[voiceChatList->numVoiceChats].numSounds++;
			current = voiceChats[voiceChatList->numVoiceChats].numSounds;

			if (voiceChats[voiceChatList->numVoiceChats].numSounds >= MAX_VOICESOUNDS)
				break;
		}

		voiceChatList->numVoiceChats++;
		if (voiceChatList->numVoiceChats >= maxVoiceChats)
			return qtrue;
	}
	return qtrue;
}

/*
=================
CG_LoadVoiceChats
=================
*/
void CG_LoadVoiceChats( void )
{

	voiceChatLists[0].numVoiceChats = 0;
	voiceChatLists[1].numVoiceChats = 0;

	CG_ParseVoiceChats( "scripts/wm_axis_chat.voice", &voiceChatLists[0], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/wm_allies_chat.voice", &voiceChatLists[1], MAX_VOICECHATS );

	// IRATA add some usefull output
	CG_Printf("Axis voice chat count = %i\n", voiceChatLists[0].numVoiceChats );
	CG_Printf("Allies voice chat count = %i\n", voiceChatLists[1].numVoiceChats );

	CG_Printf("Voice chat memory remaining = %d kb\n", trap_MemoryRemaining()/8/1024);
}

/*
=================
CG_HeadModelVoiceChats
=================
*/
int CG_HeadModelVoiceChats( char *filename )
{
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;
	int	len = trap_FS_FOpenFile( filename, &f, FS_READ ), i;

	if ( !f ) {
		trap_Print( va( "voice chat file not found: %s\n", filename ) );
		return -1;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return -1;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	token = COM_ParseExt(p, qtrue);
	if (!token || token[0] == 0) {
		return -1;
	}

	for ( i = 0; i < MAX_VOICEFILES; i++ ) {
		if ( !Q_stricmp(token, voiceChatLists[i].name) ) {
			return i;
		}
	}

	//FIXME: maybe try to load the .voice file which name is stored in token?
	return -1;
}

/*
=================
CG_GetVoiceChat
=================
*/
int CG_GetVoiceChat( voiceChatList_t *voiceChatList, const char *id, sfxHandle_t *snd, qhandle_t *sprite, char **chat, float randomNum)
{
	int i, rnd;

	for ( i = 0; i < voiceChatList->numVoiceChats; i++ ) {
		if ( !Q_stricmp( id, voiceChatList->voiceChats[i].id ) ) {
			rnd = /*random()*/randomNum * voiceChatList->voiceChats[i].numSounds;	// jaquboss , sync with server
			*snd = voiceChatList->voiceChats[i].sounds[rnd];
			*sprite = voiceChatList->voiceChats[i].sprite[rnd];
			*chat = voiceChatList->voiceChats[i].chats[rnd];
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
CG_VoiceChatListForClient
=================
*/
voiceChatList_t *CG_VoiceChatListForClient( int clientNum, qboolean disguise )
{
	if ( cgs.clientinfo[ clientNum ].team == TEAM_AXIS  ) {
		return &voiceChatLists[disguise ? 1 : 0];
	}
	else  {
		return &voiceChatLists[disguise ? 0 : 1];
	}
}

#define MAX_VOICECHATBUFFER		32

typedef struct bufferedVoiceChat_s
{
	int			clientNum;				// client who called the vsay
	sfxHandle_t snd;					// sound effect to play
	qhandle_t	sprite;					// icon to display over the speaking player's head
	int			voiceOnly;				// No text printed in the chat area
	char		cmd[MAX_SAY_TEXT];
	char		message[MAX_SAY_TEXT];	// Message to print in chat area
	vec3_t		origin;					// NERVE - SMF
	qboolean	disguise;
} bufferedVoiceChat_t;

bufferedVoiceChat_t voiceChatBuffer[MAX_VOICECHATBUFFER];

/*
=================
CG_PlayVoiceChat
=================
*/
void CG_PlayVoiceChat( bufferedVoiceChat_t *vchat )
{
	if ( !cg_noVoiceChats.integer ) {
		trap_S_StartLocalSound( vchat->snd, CHAN_VOICE);

		// Arnout: don't show icons for the HQ (clientnum -1)
		if( vchat->clientNum != -1 ) {
			// DHM - Nerve :: Show icon above head
			if ( vchat->clientNum == cg.snap->ps.clientNum ) {
				cg.predictedPlayerEntity.voiceChatSprite = vchat->sprite;
				// display chat icon for duration specified by ch_voiceSpriteTime
				if ( vchat->sprite == cgs.media.voiceChatShader ) {
					cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
				}
				// display non-chat icons for double the duration
				else {
					cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
				}
			}
			else {
				cg_entities[ vchat->clientNum ].voiceChatSprite = vchat->sprite;
				VectorCopy( vchat->origin, cg_entities[ vchat->clientNum ].lerpOrigin );			// NERVE - SMF

				// display chat icon for duration specified by ch_voiceSpriteTime
				if ( vchat->sprite == cgs.media.voiceChatShader ) {
					cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
				}
				// display non-chat icons for double the duration
				else {
					cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
				}
			} // dhm - end
		}
	}

	if (!vchat->voiceOnly && !cg_noVoiceText.integer) {
		CG_AddToTeamChat( vchat->message, vchat->clientNum, vchat->disguise );
		CG_Printf( va( "[skipnotify]: %s\n", vchat->message ) ); // JPW NERVE
		CG_WriteToLog( "%s\n", vchat->message  );
	}

	voiceChatBuffer[cg.voiceChatBufferOut].snd = 0;
}

/*
=====================
CG_PlayBufferedVoieChats
=====================
*/
void CG_PlayBufferedVoiceChats( void )
{
	if ( cg.voiceChatTime < cg.time ) {
		if (cg.voiceChatBufferOut != cg.voiceChatBufferIn && voiceChatBuffer[cg.voiceChatBufferOut].snd) {
			//
			CG_PlayVoiceChat(&voiceChatBuffer[cg.voiceChatBufferOut]);
			//
			cg.voiceChatBufferOut = (cg.voiceChatBufferOut + 1) % MAX_VOICECHATBUFFER;
			cg.voiceChatTime = cg.time + 1000;
		}
	}
}

/*
=====================
CG_AddBufferedVoiceChat
=====================
*/
void CG_AddBufferedVoiceChat( bufferedVoiceChat_t *vchat )
{
// JPW NERVE new system doesn't buffer but overwrites vchats FIXME put this on a cvar to choose which to use
	memcpy(&voiceChatBuffer[0],vchat,sizeof(bufferedVoiceChat_t));
	cg.voiceChatBufferIn = 0;
	CG_PlayVoiceChat(&voiceChatBuffer[0]);
}

/*
=================
CG_VoiceChatLocal
=================
*/
void CG_VoiceChatLocal( int mode, qboolean voiceOnly, int clientNum, int color, const char *cmd, vec3_t origin, float randomNum, int disguise )
{
	char *chat;
	voiceChatList_t		*voiceChatList;
	clientInfo_t		*ci;
	sfxHandle_t			snd;
	qhandle_t			sprite;
	bufferedVoiceChat_t vchat;
	char		*locStr = NULL;			// NERVE - SMF

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}

	ci = &cgs.clientinfo[ clientNum ];

	cgs.currentVoiceClient	= clientNum;
	voiceChatList			= CG_VoiceChatListForClient( clientNum, disguise ? qtrue : qfalse );

	if ( CG_GetVoiceChat( voiceChatList, cmd, &snd, &sprite, &chat, randomNum ) ) {
		//
		if ( mode == SAY_TEAM || !cg_teamChatsOnly.integer ) {
			vchat.clientNum		= clientNum;	// who called the vsay
			vchat.snd			= snd;			// sound to play
			vchat.sprite		= sprite;		// sprite to put over the player's head
			vchat.voiceOnly		= voiceOnly;	// voice only (No text in chat area)
			vchat.disguise		= qfalse;

			VectorCopy( origin, vchat.origin );		// NERVE - SMF
			Q_strncpyz(vchat.cmd, cmd, sizeof(vchat.cmd));

			if( mode != SAY_ALL ) {

				locStr = CG_BuildLocationString( clientNum, origin, LOC_VCHAT );

				if( !locStr || !*locStr ) {
					locStr = "";
				}
			}

			if( mode == SAY_TEAM ) {
				if ( disguise && cgs.clientinfo[cg.clientNum].team != cgs.clientinfo[clientNum].team ) {
					Com_sprintf(vchat.message, sizeof(vchat.message), "(%s^7)^3(%s)^7:^%c%s",
						cgs.clientinfo[ clientNum ].disguiseName, locStr, color, CG_TranslateString( chat ));
					vchat.disguise = qtrue;
				}
				else {
					Com_sprintf(vchat.message, sizeof(vchat.message), "(%s^7)^3(%s)^7:^%c%s",
						ci->name, locStr, color, CG_TranslateString( chat ));
				}
			}
			else if( mode == SAY_BUDDY ) {
				Com_sprintf(vchat.message, sizeof(vchat.message), "<%s^7>%c%c<%s>: %c%c%s",
					ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, locStr, Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			}
			else {
				Com_sprintf(vchat.message, sizeof(vchat.message), "%s%c%c: %c%c%s",
					ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			}
			CG_AddBufferedVoiceChat(&vchat);
		}
	}
}

/*
=================
CG_VoiceChat
=================
*/
void CG_VoiceChat( int mode )
{
	const char *cmd;
	qboolean voiceOnly = atoi(CG_Argv(1));
	int clientNum = atoi(CG_Argv(2));
	int color = atoi(CG_Argv(3));
	vec3_t origin;
	float randomNum;
	int disguise=0;

	if( mode != SAY_ALL ) {
		origin[0] = atoi(CG_Argv(5));
		origin[1] = atoi(CG_Argv(6));
		origin[2] = atoi(CG_Argv(7));
		randomNum = atof(CG_Argv(8));
		disguise = atoi(CG_Argv(9));
	}
	else {
		randomNum = atof(CG_Argv(5));
	}

	cmd = CG_Argv(4);

	CG_VoiceChatLocal( mode, voiceOnly, clientNum, color, cmd, origin, randomNum, disguise );
}
// -NERVE - SMF

/*
=================
CG_RemoveChatEscapeChar
=================
*/
static void CG_RemoveChatEscapeChar( char *text )
{
	int i, l = 0;

	for ( i = 0; text[i]; i++ ) {
		if (text[i] == '\x19')
			continue;
		text[l++] = text[i];
	}
	text[l] = '\0';
}

/*
=================
CG_LocalizeServerCommand

NERVE - SMF - localize string sent from server

- localization is ON by default.
- use [lof] in string to turn OFF
- use [lon] in string to turn back ON
=================
*/
const char* CG_LocalizeServerCommand( const char *buf )
{
	static char token[MAX_TOKEN_CHARS];
	char temp[MAX_TOKEN_CHARS];
	qboolean togloc = qtrue;
	const char *s =buf;
	int i, prev = 0;

	memset( token, 0, sizeof( token ) );

	for ( i = 0; *s; i++, s++ ) {
		// TTimo:
		// line was: if ( *s == '[' && !Q_strncmp( s, "[lon]", 5 ) || !Q_strncmp( s, "[lof]", 5 ) ) {
		// || prevails on &&, gcc warning was 'suggest parentheses around && within ||'
		// modified to the correct behaviour
		if ( *s == '[' && ( !Q_strncmp( s, "[lon]", 5 ) || !Q_strncmp( s, "[lof]", 5 ) ) ) {

			if ( togloc ) {
				memset( temp, 0, sizeof( temp ) );
				strncpy( temp, buf + prev, i - prev );
				strcat( token, CG_TranslateString( temp ) );
			}
			else {
				strncat( token, buf + prev, i - prev );
			}

			if ( s[3] == 'n' )
				togloc = qtrue;
			else
				togloc = qfalse;

			i += 5;
			s += 5;
			prev = i;
		}
	}

	if ( togloc ) {
		memset( temp, 0, sizeof( temp ) );
		strncpy( temp, buf + prev, i - prev );
		strcat( token, CG_TranslateString( temp ) );
	}
	else {
		strncat( token, buf + prev, i - prev );
	}

	return token;
}
// -NERVE - SMF


// OSP
void CG_wstatsParse_cmd( void )
{
	if( cg.showStats ) {
		if(cg.statsWindow == NULL
		  || cg.statsWindow->id != WID_STATS
		  || cg.statsWindow->inuse == qfalse) {
			CG_createStatsWindow();
		}
		else if(cg.statsWindow->state == WSTATE_SHUTDOWN) {
			cg.statsWindow->state = WSTATE_START;
			cg.statsWindow->time = trap_Milliseconds();
		}

		if(cg.statsWindow == NULL) {
			cg.showStats = qfalse;
		}
		else {
			cg.statsWindow->effects |= WFX_TEXTSIZING;
			cg.statsWindow->lineCount = 0;
			cg.windowCurrent = cg.statsWindow;
			CG_parseWeaponStats_cmd(CG_printWindow);
		}
	}
}


void CG_topshotsParse_cmd(qboolean doBest)
{
	int iArg = 1;
	int iWeap = atoi(CG_Argv(iArg++));
	topshotStats_t *ts = &cgs.topshots;

	ts->cWeapons = 0;

	while(iWeap) {
		int cnum = atoi(CG_Argv(iArg++));
		int hits = atoi(CG_Argv(iArg++));
		int atts = atoi(CG_Argv(iArg++));
		int kills = atoi(CG_Argv(iArg++));
		char name[32];

		// rain - bump up iArg since we didn't push it into deaths, above
		iArg++;

		if(ts->cWeapons < WS_MAX * 2) {
			float acc = (atts > 0) ? (float)(hits * 100) / (float)atts : 0.0f;

			BG_cleanName(cgs.clientinfo[cnum].name, name, 17, qfalse);
			Q_strncpyz(ts->strWS[ts->cWeapons++],
						va("%-12s %5.1f %4d/%-4d %5d  %s",
															WeaponStatsName(iWeap-1),
															acc, hits, atts,
															kills,
															name),
						sizeof(ts->strWS[0]));
		}

		iWeap = atoi(CG_Argv(iArg++));
	}
}

void CG_ParseWeaponStats( void )
{
	cgs.ccWeaponShots =	atoi( CG_Argv( 1 ) );
	cgs.ccWeaponHits =	atoi( CG_Argv( 2 ) );
}

void CG_ParsePortalPos( void )
{
	int i;

	cgs.ccCurrentCamObjective = atoi( CG_Argv( 1 ) );
	cgs.ccPortalEnt =			atoi( CG_Argv( 8 ) );

	for( i = 0; i < 3; i++ ) {
		cgs.ccPortalPos[i] = atoi( CG_Argv( i+2 ) );
	}

	for( i = 0; i < 3; i++ ) {
		cgs.ccPortalAngles[i] = atoi( CG_Argv( i+5 ) );
	}
}

extern pmove_t *pm;

// Cached stats
void CG_parseWeaponStatsGS_cmd(void)
{
	gameStats_t		*gs			= &cgs.gamestats;
	int				i;
	int				iArg		= 1;
	int 			nClientID	= atoi(CG_Argv(iArg++));
	clientInfo_t	*ci			= &cgs.clientinfo[nClientID];
	int 			nRounds		= atoi(CG_Argv(iArg++));
	int 			weaponMask	= atoi(CG_Argv(iArg++));
	int 			skillMask;
	int				xp			= 0;

	gs->cWeapons	= 0;
	gs->cSkills		= 0;
	gs->fHasStats	= qfalse;
	gs->nClientID	= nClientID;
	gs->nRounds		= nRounds;

//	Q_strncpyz(strName, ci->name, sizeof(strName));
//	BG_cleanName(cgs.clientinfo[gs->nClientID].name, strName, sizeof(strName), qfalse);

	if(weaponMask != 0) {
		int				tHits		= 0;
		int				tHeadshots	= 0;

		// core: hit regions (this is an overall score, counting all weapons/all hits)
		int	hrHead = atoi(CG_Argv(iArg++));
		int	hrArms = atoi(CG_Argv(iArg++));
		int	hrBody = atoi(CG_Argv(iArg++));
		int	hrLegs = atoi(CG_Argv(iArg++));

		char strName[MAX_STRING_CHARS];

		int nHits;
		int nShots;
		int nKills;
		int nDeaths;
		int nHeadshots;

		for(i=WS_KNIFE; i<WS_MAX; i++) {
			if(weaponMask & (1 << i)) {
				nHits = atoi(CG_Argv(iArg++));
				nShots = atoi(CG_Argv(iArg++));
				nKills = atoi(CG_Argv(iArg++));
				nDeaths = atoi(CG_Argv(iArg++));
				nHeadshots = atoi(CG_Argv(iArg++));

				if ( BG_Weapons[aWeaponInfo[i].WP_Index].headShot ) {
					tHits += nHits;
					tHeadshots += nHeadshots;
				}

				Q_strncpyz(strName, va("%-12s  ", WeaponStatsName(i)), sizeof(strName));
				if(nShots > 0 || nHits > 0) {
					Q_strcat(strName, sizeof(strName), va("%5.1f %4d/%-4d ",
														((nShots == 0) ? 0.0 : (float)(nHits*100.0/(float)nShots)),
														nHits, nShots));
				}
				else {
					Q_strcat(strName, sizeof(strName), va("                "));
				}

				Q_strncpyz(gs->strWS[gs->cWeapons++],
						   va("%s%5d %6d%s", strName, nKills, nDeaths, ((BG_Weapons[aWeaponInfo[i].WP_Index].headShot) ? va(" %9d", nHeadshots) : "")),
						   sizeof(gs->strWS[0]));

				if(nShots > 0 || nHits > 0 || nKills > 0 || nDeaths) {
					gs->fHasStats = qtrue;
				}
			}
		}

		if(gs->fHasStats) {
			int dmg_given	= atoi(CG_Argv(iArg++));
			int dmg_rcvd	= atoi(CG_Argv(iArg++));
			int team_dmg	= atoi(CG_Argv(iArg++));
			int spree		= BG_GetKillSpree(&cg.snap->ps);
			int bestspree	= BG_GetStatBestSpree(&cg.snap->ps);
			float hsRatio	= (tHeadshots == 0)? 0.0 : (float)(tHeadshots*100.0/(float)tHits);

			Q_strncpyz(gs->strExtra[0], va("Damage Given: %-6d  Team Damage: %-6d HS%%: %-3.1f", dmg_given, team_dmg, hsRatio), sizeof(gs->strExtra[0]));
			Q_strncpyz(gs->strExtra[1], va("Damage Recvd: %d", dmg_rcvd), sizeof(gs->strExtra[1]));
			Q_strncpyz(gs->strExtra[2], va("KillingSpree: %-6d   Best spree: %d", spree, bestspree), sizeof(gs->strExtra[2]));
			Q_strncpyz(gs->strExtra[3], va("Hit Regions : Head:%-3d Arms:%-3d Body:%-3d Legs:%-3d", hrHead, hrArms, hrBody, hrLegs), sizeof(gs->strExtra[3]));
		}
	}

	// Derive XP from individual skill XP
	skillMask = atoi(CG_Argv(iArg++));

	for(i=SK_BATTLE_SENSE; i<SK_NUM_SKILLS; i++) {
		if(skillMask & (1 << i)) {
			ci->skillpoints[i] = atoi(CG_Argv(iArg++));
			xp += ci->skillpoints[i];
		}
	}

	Q_strncpyz(gs->strRank, va("%-13s %d", ((ci->team == TEAM_AXIS) ? rankNames_Axis : rankNames_Allies)[ci->rank], xp), sizeof(gs->strRank));

	if(skillMask != 0) {
		char *str;

		for(i=SK_BATTLE_SENSE; i<SK_NUM_SKILLS; i++) {
			if((skillMask & (1 << i)) == 0) {
				continue;
			}

			if(ci->skill[i] < NUM_SKILL_LEVELS - 1) {
				str = va("%4d/%-4d", ci->skillpoints[i], skillLevels[i][ci->skill[i]+1]);
			}
			else {
				str = va("%d", ci->skillpoints[i]);
			}

			if(cgs.gametype == GT_WOLF_CAMPAIGN) {
				Q_strncpyz(gs->strSkillz[gs->cSkills++], va("%-15s %3d %s %12d", skillNames[i], ci->skill[i], str, ci->medals[i]), sizeof(gs->strSkillz[0]));
			}
			else {
				Q_strncpyz(gs->strSkillz[gs->cSkills++], va("%-15s %3d %s", skillNames[i], ci->skill[i], str),	sizeof(gs->strSkillz[0]));
			}
		}
	}
}


// Client-side stat presentation
void CG_parseWeaponStats_cmd(void (txt_dump)(char *))
{
	qboolean fFull = (txt_dump != CG_printWindow);
	qboolean fHasStats = qfalse;
	char strName[MAX_STRING_CHARS];
	unsigned int i, iArg = 1;
	unsigned int nClient = atoi(CG_Argv(iArg++));
	clientInfo_t *ci = &cgs.clientinfo[nClient];
	unsigned int nRounds = atoi(CG_Argv(iArg++));
	unsigned int dwWeaponMask = atoi(CG_Argv(iArg++));
	unsigned int dwSkillPointMask;
	int			 xp = 0;

	Q_strncpyz(strName, ci->name, sizeof(strName));
	BG_cleanName(cgs.clientinfo[nClient].name, strName, sizeof(strName), qfalse);
	txt_dump(va("^7Overall stats for: ^3%s ^7(^2%d^7 Round%s)\n\n", strName, nRounds, ((nRounds != 1) ? "s" : "")));

	if(fFull) {
		txt_dump(     "Weapon     Acrcy Hits/Atts Kills Deaths Headshots\n");
		txt_dump(     "-------------------------------------------------\n");
	}
	else {
		txt_dump(     "Weapon     Acrcy Hits/Atts Kll Dth HS\n");
		//txt_dump(     "-------------------------------------\n");
		txt_dump(     "\n");
	}

	if(!dwWeaponMask) {
		txt_dump("^3No weapon info available.\n");
	}
	else {
		int atts, deaths, dmg_given, dmg_rcvd, hits, kills, team_dmg, headshots;

		for(i=WS_KNIFE; i<WS_MAX; i++) {
			if(dwWeaponMask & (1 << i)) {
				hits = atoi(CG_Argv(iArg++));
				atts = atoi(CG_Argv(iArg++));
				kills = atoi(CG_Argv(iArg++));
				deaths = atoi(CG_Argv(iArg++));
				headshots = atoi(CG_Argv(iArg++));

				Q_strncpyz(strName, va("^3%-9s: ", WeaponStatsName(i)), sizeof(strName));
				if(atts > 0 || hits > 0) {
					fHasStats = qtrue;
					Q_strcat(strName, sizeof(strName), va("^7%5.1f ^5%4d/%-4d ",
														((atts == 0) ? 0.0 : (float)(hits*100.0/(float)atts)),
														hits, atts));
				}
				else {
					Q_strcat(strName, sizeof(strName), va("                "));
					if(kills > 0 || deaths > 0) fHasStats = qtrue;
				}

				if(fFull)
					txt_dump(va("%s^2%5d ^1%6d%s\n", strName, kills, deaths, ((BG_Weapons[aWeaponInfo[i].WP_Index].headShot) ? va(" ^3%9d", headshots) : "")));
				else
					txt_dump(va("%s^2%3d ^1%3d%s\n", strName, kills, deaths, ((BG_Weapons[aWeaponInfo[i].WP_Index].headShot) ? va(" ^3%2d", headshots) : "")));
			}
		}

		if(fHasStats) {
			dmg_given = atoi(CG_Argv(iArg++));
			dmg_rcvd  = atoi(CG_Argv(iArg++));
			team_dmg  = atoi(CG_Argv(iArg++));

			if(!fFull) {
				txt_dump("\n\n");
			}

			txt_dump(va("\n^3Damage Given: ^7%-6d  ^3Team Damage: ^7%d\n", dmg_given, team_dmg));
			txt_dump(va(  "^3Damage Recvd: ^7%d\n", dmg_rcvd));
		}
	}

	if(!fFull) {
		txt_dump("\n\n\n");
	}

	// Derive XP from individual skill XP
	dwSkillPointMask = atoi(CG_Argv(iArg++));
	for(i=SK_BATTLE_SENSE; i<SK_NUM_SKILLS; i++) {
		if(dwSkillPointMask & (1 << i)) {
			ci->skillpoints[i] = atoi(CG_Argv(iArg++));
			xp += ci->skillpoints[i];
		}
	}

	txt_dump(va("\n^2Rank: ^7%s (%d XP)\n", ((ci->team == TEAM_AXIS) ? rankNames_Axis : rankNames_Allies)[ci->rank], xp));

	if(!fFull) {
		txt_dump("\n\n\n");
	}

	// Medals only in campaign mode
	txt_dump(    va("Skills         Level/Points%s\n", ((cgs.gametype == GT_WOLF_CAMPAIGN) ? "  Medals" : "")));
	if(fFull) {
		txt_dump(va("---------------------------%s\n", ((cgs.gametype == GT_WOLF_CAMPAIGN) ? "--------" : "")));
	} else {
		txt_dump("\n");
	}

	if(dwSkillPointMask == 0) {
		txt_dump("^3No skills acquired!\n");
	}
	else {
		char *str;

		for(i=SK_BATTLE_SENSE; i<SK_NUM_SKILLS; i++) {

			if((dwSkillPointMask & (1 << i)) == 0) {
				continue;
			}

			if(ci->skill[i] < NUM_SKILL_LEVELS - 1)
			{
				str = va("%d (%d/%d)", ci->skill[i], ci->skillpoints[i], skillLevels[i][ci->skill[i]+1]);
			}
			else {
				str = va("%d (%d)", ci->skill[i], ci->skillpoints[i]);
			}

			if(cgs.gametype == GT_WOLF_CAMPAIGN) {
				txt_dump(va("%-14s ^3%-12s  ^2%6d\n", skillNames[i], str, ci->medals[i]));
			}
			else {
				txt_dump(va("%-14s ^3%-12s\n", skillNames[i], str));
			}
		}
	}
}

void CG_parseBestShotsStats_cmd(qboolean doTop, void (txt_dump)(char *))
{
	qboolean fFull = (txt_dump != CG_printWindow);
	int iArg = 1, iWeap = atoi(CG_Argv(iArg++));

	if (!iWeap) {
		txt_dump(va("^3No qualifying %sshot info available.\n", ((doTop) ? "top" : "bottom")));
		return;
	}

	txt_dump(va("^2%s Match Accuracies:\n", (doTop) ? "BEST" : "WORST"));
	if(fFull) {
		txt_dump(  "\n^3WP   Acrcy Hits/Atts Kills Deaths\n");
		txt_dump(    "-------------------------------------------------------------\n");
	}
	else {
		txt_dump(    "^3WP   Acrcy Hits/Atts Kll Dth\n");
	//	txt_dump(    "-------------------------------------------\n");
		txt_dump(    "\n");
	}

	while(iWeap) {
		int cnum = atoi(CG_Argv(iArg++));
		int hits = atoi(CG_Argv(iArg++));
		int atts = atoi(CG_Argv(iArg++));
		int kills = atoi(CG_Argv(iArg++));
		int deaths = atoi(CG_Argv(iArg++));
		float acc = (atts > 0) ? (float)(hits*100)/(float)atts : 0.0;
		char name[32];

		if(fFull) {
			BG_cleanName(cgs.clientinfo[cnum].name, name, 30, qfalse);
			txt_dump(va("^3%s ^7%5.1f ^5%4d/%-4d ^2%5d ^1%6d ^7%s\n",
							aWeaponInfo[iWeap-1].pszCode, acc, hits, atts, kills, deaths, name));
		}
		else {
			BG_cleanName(cgs.clientinfo[cnum].name, name, 12, qfalse);
			txt_dump(va("^3%s ^7%5.1f ^5%4d/%-4d ^2%3d ^1%3d ^7%s\n",
							aWeaponInfo[iWeap-1].pszCode, acc, hits, atts, kills, deaths, name));
		}

		iWeap = atoi(CG_Argv(iArg++));
	}
}

void CG_parseTopShotsStats_cmd(qboolean doTop, void (txt_dump)(char *))
{
	int i, iArg = 1;
	int cClients = atoi(CG_Argv(iArg++));
	int iWeap = atoi(CG_Argv(iArg++));
	int wBestAcc = atoi(CG_Argv(iArg++));

	char name[32];
	int cnum, hits,atts,kills,deaths;
	float acc;
	const char* color;

	txt_dump(va("Weapon accuracies for: ^3%s\n",
							(iWeap >= WS_KNIFE && iWeap < WS_MAX) ? WeaponStatsName(iWeap) : "UNKNOWN"));

	txt_dump("\n^3  Acc Hits/Atts Kills Deaths\n");
	txt_dump(    "----------------------------------------------------------\n");

	if(!cClients) {
		txt_dump("NO QUALIFYING WEAPON INFO AVAILABLE.\n");
		return;
	}

	for (i = 0; i < cClients; i++) {
		cnum = atoi(CG_Argv(iArg++));
		hits = atoi(CG_Argv(iArg++));
		atts = atoi(CG_Argv(iArg++));
		kills = atoi(CG_Argv(iArg++));
		deaths = atoi(CG_Argv(iArg++));
		acc = (atts > 0) ? (float)(hits*100)/(float)atts : 0.0;
		color = (((doTop)?acc:((float)wBestAcc) + 0.999) >= ((doTop)?wBestAcc:acc)) ? "^3" : "^7";


		BG_cleanName(cgs.clientinfo[cnum].name, name, 30, qfalse);
		txt_dump( va("%s%5.1f ^5%4d/%-4d ^2%5d ^1%6d %s%s\n", color, acc, hits, atts, kills, deaths, color, name) );
	}
}

void CG_scores_cmd(void)
{
	const char *str = CG_Argv(1);

	CG_Printf("[skipnotify]%s", str);
	if(cgs.dumpStatsFile > 0) {
		char s[MAX_STRING_CHARS];

		BG_cleanName(str, s, sizeof(s), qtrue);
		trap_FS_Write(s, strlen(s), cgs.dumpStatsFile);
	}

	if(trap_Argc() > 2) {
		if(cgs.dumpStatsFile > 0) {
			qtime_t ct;

			trap_RealTime(&ct);
			str = va("\nStats recorded: %02d:%02d:%02d (%02d %s %d)\n\n\n",
									ct.tm_hour, ct.tm_min, ct.tm_sec,
									ct.tm_mday, aMonths[ct.tm_mon], 1900+ct.tm_year);

			trap_FS_Write(str, strlen(str), cgs.dumpStatsFile);

			CG_Printf("[cgnotify]\n^3>>> Stats recorded to: ^7%s\n\n", cgs.dumpStatsFileName);
			trap_FS_FCloseFile(cgs.dumpStatsFile);
			cgs.dumpStatsFile = 0;
		}
		cgs.dumpStatsTime = 0;
	}
}

void CG_printFile(char *str)
{
	CG_Printf(str);
	if(cgs.dumpStatsFile > 0) {
		char s[MAX_STRING_CHARS];

		BG_cleanName(str, s, sizeof(s), qtrue);
		trap_FS_Write(s, strlen(s), cgs.dumpStatsFile);
	}
}

void CG_dumpStats(void)
{
	qtime_t ct;
	qboolean fDoScores = qfalse;
	const char *info = CG_ConfigString(CS_SERVERINFO);
	char *s = va("^3>>> %s: ^2%s\n\n", CG_TranslateString("Map"), Info_ValueForKey(info, "mapname"));

	trap_RealTime(&ct);
	// /me holds breath (using circular va() buffer)
	if(cgs.dumpStatsFile == 0) {
		fDoScores = qtrue;
		cgs.dumpStatsFileName = va("stats/%d.%02d.%02d/%02d%02d%02d.txt",
										1900+ct.tm_year, ct.tm_mon+1, ct.tm_mday,
										ct.tm_hour, ct.tm_min, ct.tm_sec);
	}

	if(cgs.dumpStatsFile != 0) trap_FS_FCloseFile(cgs.dumpStatsFile);
	trap_FS_FOpenFile(cgs.dumpStatsFileName, &cgs.dumpStatsFile, FS_APPEND);

	CG_printFile(s);
	CG_parseWeaponStats_cmd(CG_printFile);
	if(cgs.dumpStatsFile == 0) {
		CG_Printf("[cgnotify]\n^3>>> %s: %s\n\n", CG_TranslateString("Could not create logfile"), cgs.dumpStatsFileName);
	}

	// Daisy-chain to scores info
	//	-- we play a game here for a statsall dump:
	//		1. we still have more ws entries in the queue to parse
	//		2. on the 1st ws entry, go ahead and send out the scores request
	//		3. we'll just continue to parse the remaining ws entries and dump them to the log
	//		   before the scores result would ever hit us.. thus, we still keep proper ordering :)
	if(fDoScores) trap_SendClientCommand("scores");
}
// -OSP


/*
====================
core:
	CG_ServerCommand_HandleChat()

	Handles a chat command for function CG_ServerCommand().
	(There is probably a nicer way to process this function..)
====================
*/
void CG_ServerCommand_HandleChat( void ) {
	int			argc = trap_Argc();
	char		text[MAX_SAY_TEXT];
	const		char *s;
	char		s1[MAX_STRING_CHARS];
	char		s2[MAX_STRING_CHARS];
	char		s3[MAX_STRING_CHARS];
	char		s4[MAX_STRING_CHARS];
	char		s5[MAX_STRING_CHARS];
	char		s6[MAX_STRING_CHARS];
	int			clientNum;
	qboolean	doChat = qfalse;
	int strIndex = atoi(CG_Argv(1));

	if ( cg_teamChatsOnly.integer ) {
		return;
	}

	Q_strncpyz( s1, CG_Argv(1), MAX_SAY_TEXT );
	// core: check if this is a cachedString..
	if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX && argc >= 2 ) {
		s = cgs.cachedStrings[strIndex];

		if ( argc >= 3 ) Q_strncpyz( s2, CG_Argv(2), MAX_SAY_TEXT );
		if ( argc >= 4 ) Q_strncpyz( s3, CG_Argv(3), MAX_SAY_TEXT );
		if ( argc >= 5 ) Q_strncpyz( s4, CG_Argv(4), MAX_SAY_TEXT );
		if ( argc >= 6 ) Q_strncpyz( s5, CG_Argv(5), MAX_SAY_TEXT );
		if ( argc >= 7 ) Q_strncpyz( s6, CG_Argv(6), MAX_SAY_TEXT );

		// chat 0 params
		if ( strIndex >= GAMESTRING_PLAYER_NOT_FOUND && strIndex < GAMESTRING_1_AUTO_UNMUTE_LANGUAGE ) {
			clientNum = atoi(CG_Argv(2));
			doChat = qtrue;
		}
		// chat 1 param
		else if ( /*strIndex >= GAMESTRING_1_AUTO_UNMUTE_LANGUAGE &&*/ strIndex < GAMESTRING_2_MUTED_SECONDS_LANGUAGE ) {
			if ( argc >= 3 ) {
				s = va(s,s2);
				clientNum = atoi(CG_Argv(3));
				doChat = qtrue;
			}
		}
		// chat 2 params
		else if ( /*strIndex >= GAMESTRING_2_MUTED_SECONDS_LANGUAGE &&*/ strIndex < GAMESTRING_3_DEATHSPREE ) {
			if ( argc >= 4 ) {
				s = va(s,s2,s3);
				clientNum = atoi(CG_Argv(4));
				doChat = qtrue;
			}
		}
		// chat 3 params
		else if ( /*strIndex >= GAMESTRING_3_DEATHSPREE &&*/ strIndex < GAMESTRING_4_PM_X_RECIPIENTS ) {
			if ( argc >= 5 ) {
				s = va(s,s2,s3,s4);
				clientNum = atoi(CG_Argv(5));
				doChat = qtrue;
			}
		}
		// chat 4 params
		else if ( /*strIndex >= GAMESTRING_4_PM_X_RECIPIENTS &&*/ strIndex < GAMESTRING_5_ADMINTEST_LEVEL_X_USER ) {
			if ( argc >= 6 ) {
				s = va(s,s2,s3,s4,s5);
				clientNum = atoi(CG_Argv(6));
				doChat = qtrue;
			}
		}
		// chat 5 params
		else if ( /*strIndex >= GAMESTRING_5_ADMINTEST_LEVEL_X_USER &&*/ strIndex < GAMESTRING_SC_HEADER1 ) {
			if ( argc >= 7 ) {
				s = va(s,s2,s3,s4,s5,s6);
				clientNum = atoi(CG_Argv(7));
				doChat = qtrue;
			}
		}
	}
	else {
		// Use the default way to display the print-text,..
		// this chat is not using cached strings.
		if ( atoi( CG_Argv( 3 ) ) ) {
			s = CG_LocalizeServerCommand( s1 );
		} else {
			s = s1;
		}
		clientNum = atoi(CG_Argv(2));
		doChat = qtrue;
	}

	// output the text..
	if ( doChat ) {
		Q_strncpyz( text, s, MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		CG_AddToTeamChat( text, clientNum, qfalse );
		CG_Printf( "%s\n", text );
		CG_WriteToLog( "%s\n", text );
	}
	return;
}

/*
====================
core:
	CG_ServerCommand_HandlePrint()

	Handles a print command for function CG_ServerCommand().
	(There is probably a nicer way to process this function..)
====================
*/
void CG_ServerCommand_HandlePrint( void ) {
	int argc = trap_Argc();
	if ( argc >= 2 ) {
		char *s = (char *)CG_Argv(1);
		// core: check if we received an index into the cachedStrings array..
		int strIndex = atoi(s);
		if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX ) {
			if ( argc >= 3 ) {
				s = cgs.cachedStrings[strIndex];
				switch( argc - 2 ) {
					case 1:
						s = va( s, CG_Argv(2) );
						break;
					case 2:
						s = va( s, CG_Argv(2),CG_Argv(3) );
						break;
					case 3:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4) );
						break;
					case 4:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5) );
						break;
					case 5:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6) );
						break;
					case 6:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7) );
						break;
					case 7:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8) );
						break;
					case 8:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9) );
						break;
					case 9:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10) );
						break;
					case 10:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11) );
						break;
					case 11:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12) );
						break;
					case 12:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13) );
						break;
					case 13:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13),CG_Argv(14) );
						break;
					case 14:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13),CG_Argv(14),CG_Argv(15) );
						break;
					case 15:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13),CG_Argv(14),CG_Argv(15),CG_Argv(16) );
						break;
					case 16:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13),CG_Argv(14),CG_Argv(15),CG_Argv(16),CG_Argv(17) );
						break;
					case 17:
						s = va( s, CG_Argv(2),CG_Argv(3),CG_Argv(4),CG_Argv(5),CG_Argv(6),CG_Argv(7),CG_Argv(8),CG_Argv(9),CG_Argv(10),CG_Argv(11),CG_Argv(12),CG_Argv(13),CG_Argv(14),CG_Argv(15),CG_Argv(16),CG_Argv(17),CG_Argv(18) );
						break;
					default:
						s = NULL;	//"^3!!!!!we need even more params for the print-command!!!!!";
						break;
				}
				if (s) {
					CG_Printf( "[cgnotify]%s", CG_LocalizeServerCommand(s) );
				}
			}
			else { // if ( argc >= 3 )
				CG_Printf( "[cgnotify]%s", CG_LocalizeServerCommand(cgs.cachedStrings[strIndex]) );
			}
		} 
		else { // if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX )
			CG_Printf( "[cgnotify]%s", CG_LocalizeServerCommand(s) );
		}
	}
}

/*
====================
core:
	CG_ServerCommand_HandleCP()

	Handles a CP command for function CG_ServerCommand().
====================
*/
void CG_ServerCommand_HandleCP( void ) {
	// NERVE - SMF
	int argc = trap_Argc();
	char *s;
	int strIndex;

	if ( argc >= 3 ) {
		s = CG_TranslateString( CG_Argv(1) );

		// core: check if we received an index into the cachedStrings array..
		strIndex = atoi(s);
		if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX ) {
			s = cgs.cachedStrings[strIndex];
		}

		if ( argc == 4 ) {
			s = va( "%s%s", CG_Argv( 3 ), s );
		}

		// OSP - for client logging
		if(cg_printObjectiveInfo.integer > 0 && (argc == 4 || atoi(CG_Argv(2)) > 1)) {
			CG_Printf("[cgnotify]*** ^3INFO: ^5%s\n", CG_LocalizeServerCommand(CG_Argv(1)));
		}
		CG_PriorityCenterPrint( s, SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH, atoi( CG_Argv( 2 ) ) );
	}
	else {
		s = (char *)CG_LocalizeServerCommand( CG_Argv(1) );

		// core: check if we received an index into the cachedStrings array..
		strIndex = atoi(s);
		if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX ) {
			s = cgs.cachedStrings[strIndex];
		}

		CG_CenterPrint( s, SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH );	//----(SA)	modified
	}
}

/*
====================
core:
	CG_ServerCommand_HandleGS

	handle a gs command..
====================
*/
void CG_ServerCommand_HandleGS( void ) {
	int argc = trap_Argc();
	int arg;
#ifdef _DEBUG
	int countModels=0, countShaders=0, countSkins=0, countCharacters=0, countSounds=0, countShaderstate=0;
#endif

	int index;
	qboolean isModels, isSkins, isCharacters, isShaders, isSounds, isShaderstate;

	for ( arg=1; arg<argc; arg+=2 ) {
		index = atoi( CG_Argv(arg) );
		isModels = ( index >= CS_MODELS && index < CS_MODELS+MAX_MODELS )? qtrue : qfalse;
		isSkins = ( index >= CS_SKINS && index < CS_SKINS+MAX_CS_SKINS )? qtrue : qfalse;
		isCharacters = ( index >= CS_CHARACTERS && index < CS_CHARACTERS+MAX_CHARACTERS )? qtrue : qfalse;
		isShaders = ( index >= CS_SHADERS && index < CS_SHADERS+MAX_CS_SHADERS )? qtrue : qfalse;
		isSounds = ( index >= CS_SOUNDS && index < CS_SOUNDS+MAX_SOUNDS )? qtrue : qfalse;
		isShaderstate = ( index == CS_SHADERSTATE )? qtrue : qfalse;

		if ( isModels ) {
			const char *modelName = CG_Argv(arg+1);
			index -= CS_MODELS;
			if ( modelName && modelName[0] != 0 ) {
				cgs.gameModels[ index ] = trap_R_RegisterModel( modelName );
#ifdef _DEBUG
				countModels++;
#endif
			}
			cgs.csMethod_received++;

		}
		else if ( isShaders ) {
			const char *shaderName = CG_Argv(arg+1);
			index -= CS_SHADERS;
			if ( shaderName && shaderName[0] != 0 ) {
				cgs.gameShaders[ index ] = (shaderName[0] == '*')? trap_R_RegisterShader( shaderName+1 ) : trap_R_RegisterShaderNoMip( shaderName );
				Q_strncpyz( cgs.gameShaderNames[ index ], (shaderName[0] == '*')? shaderName+1 : shaderName, MAX_QPATH );
				// initialize the map-icons for triggers, now that the shader-index is known..
				CG_ParseOIDInfos();
#ifdef _DEBUG
				countShaders++;
#endif
			}
			cgs.csMethod_received++;

		}
		else if ( isSkins ) {
			const char *skinName = CG_Argv(arg+1);
			index -= CS_SKINS;
			if ( skinName && skinName[0] != 0 ) {
				cgs.gameModelSkins[ index ] = trap_R_RegisterSkin( skinName );
#ifdef _DEBUG
				countSkins++;
#endif
			}
			cgs.csMethod_received++;

		}
		else if ( isCharacters ) {
			const char *characterName = CG_Argv(arg+1);
			index -= CS_CHARACTERS;
			if ( characterName && characterName[0] != 0 ) {
				if( !BG_FindCharacter( characterName ) ) {
					cgs.gameCharacters[ index ] = BG_FindFreeCharacter( characterName );
					Q_strncpyz( cgs.gameCharacters[ index ]->characterFile, characterName, sizeof(cgs.gameCharacters[ index ]->characterFile) );
					if( !CG_RegisterCharacter( characterName, cgs.gameCharacters[ index ] ) ) {
						CG_Error( "ERROR: CG_RegisterGraphics: failed to load character file '%s'\n", characterName );
					}
#ifdef _DEBUG
					countCharacters++;
#endif
				}
			}
			cgs.csMethod_received++;

		}
		else if ( isSounds ) {
			const char *soundName = CG_Argv(arg+1);
			index -= CS_SOUNDS;
			if ( soundName && soundName[0] != 0 ) {
				// no custom sound?..
				if ( soundName[0] != '*' ) {
					Q_strncpyz( cgs.gameSoundNames[ index ], soundName, sizeof(cgs.gameSoundNames[0]) );
					// Ridah, register sound scripts seperately
					if (!strstr(soundName, ".wav")) {
						CG_SoundScriptPrecache( soundName );
					} else {
						cgs.gameSounds[ index ] = trap_S_RegisterSound( soundName, qfalse );
					}
#ifdef _DEBUG
					countSounds++;
#endif
				}
			}
			cgs.csMethod_received++;

		}
		else if ( isShaderstate ) {
			// This is a helper-entry..
			// It is only send by the server to indicate the client
			// to do a remapshaderflush..
			// Note: see the comment in C_SendAllNewConfigStrings()
			CG_ShaderStateChanged();
#ifdef _DEBUG
			countShaderstate++;
#endif

			if ( cgs.csMethod == 1 && cgs.csMethod_requesting != 0 ) {
				// core: new ConfigString data must all been received now
				if ( cgs.csMethod_received >= cgs.csMethod_total ) {
					// all the data has been received..
					C_CSMethodDone();
				} else {
					// not all is received, check & request again..
					C_CSMethodRequest();
				}
			}
		}
	}

#ifdef _DEBUG
	CG_Printf( va("Gamestate received:  models:%i shaders:%i skins:%i characters:%i sounds:%i remapshader:%i\n",
				countModels, countShaders, countSkins,
				countCharacters, countSounds, countShaderstate) );
#endif
}



/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
void CG_ForceTapOut_f( void );
void C_BindSearch( const char *command );

static void CG_ServerCommand( void )
{
	const char	*cmd;
	char		text[MAX_SAY_TEXT];

 	cmd = CG_Argv(0);

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	if ( !strcmp( cmd, "tinfo" ) ) {
		CG_ParseTeamInfo();
		return;
	}

	if ( !Q_stricmp( cmd, "entnfo" ) ) {
		char buffer[16];
		int allied_number, axis_number;

		trap_Argv(1, buffer, 16);
		axis_number = atoi(buffer);

		trap_Argv(2, buffer, 16);
		allied_number = atoi(buffer);

		CG_ParseMapEntityInfo( axis_number, allied_number );

		return;
	}

	if ( !strcmp( cmd, "sc0" ) ) {
		CG_ParseScore(TEAM_AXIS);
		return;
	}
	else if ( !strcmp( cmd, "sc1" ) ) {
		CG_ParseScore(TEAM_ALLIES);
		return;
	}

	if ( !Q_stricmp( cmd, "print" ) ) {
		CG_ServerCommand_HandlePrint();
		return;
	}

	if ( !Q_stricmp( cmd, "chat" ) ) {
		CG_ServerCommand_HandleChat();
		return;
	}

	if ( !Q_stricmp( cmd, "cp" ) ) {
		CG_ServerCommand_HandleCP();
		return;
	}

	if ( !Q_stricmp( cmd, "cpm" ) ) {
		// redeye - filter connect/disconnect messages
		// is there any better/faster/more reliable way to detect this?
		const char *msg = CG_LocalizeServerCommand( CG_Argv(1) );

		// core: check if we received an index into the cachedStrings array..
		int strIndex = atoi(msg);
		if ( strIndex > GAMESTRING_ERROR && strIndex < GAMESTRING_MAX ) {
			msg = cgs.cachedStrings[strIndex];
		}

		if (((cg_popupMessageFilter.integer & POPUP_FILTER_CONNECT) && (strstr(msg, " connected") || strstr(msg, " disconnected")))) {
			trap_Print(va("connect_filter: %s", msg)); // log to console if filtered
		}
		else {
			CG_AddPMItem( PM_MESSAGE, msg, cgs.media.voiceChatShader, NULL );
		}
		return;
	}

	if ( !Q_stricmp( cmd, "cpm_map" ) ) {
		// redeye - filter map event information messages
		const char *msg = CG_LocalizeServerCommand( CG_Argv(1) );
		if (cg_popupMessageFilter.integer & POPUP_FILTER_MAPINFO) {
			trap_Print(va("%s\n", msg)); // log to console if filtered
		}
		else {
			CG_AddPMItem( PM_MESSAGE, msg, cgs.media.voiceChatShader, NULL );
		}
		return;
	}

	if ( !Q_stricmp( cmd, "announce" ) ) {
		// NERVE - SMF
		int args = trap_Argc();
		char *s;
		if ( args >= 3 ) {
			s = CG_TranslateString( CG_Argv(1) );

			if ( args == 4 ) {
				s = va( "%s%s", CG_Argv( 3 ), s );
			}

			// OSP - for client logging
			if(cg_printObjectiveInfo.integer > 0 && (args == 4 || atoi(CG_Argv(2)) > 1)) {
				CG_Printf("[cgnotify]*** ^3ANNOUNCEMENT: ^5%s\n", CG_LocalizeServerCommand(CG_Argv(1)));
			}
			CG_PrintAnnouncement( s, 200, SMALLCHAR_WIDTH, atoi( CG_Argv( 2 ) ) );
		}
		else {
			CG_CenterPrint( CG_LocalizeServerCommand( CG_Argv(1) ), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH );	//----(SA)	modified
		}
		return;
	}

	// note a bit similar name to above but much siplier
	if ( !Q_stricmp( cmd, "announcer" ) ) {
		switch (atoi(CG_Argv(1))) {
			case 0: // fight!
				CG_AddAnnouncer("FIGHT!", "", cgs.media.countFight, 1.1f, 1500, 1.0f, 0.0f, 0.0f, ANNOUNCER_NORMAL);
				break;
			case 1: // prepare to fight!
				CG_AddAnnouncer("PREPARE TO FIGHT!", "", cgs.media.countPrepare, 0.85f, 1500,  1.0f, 1.0f, 0.0f, ANNOUNCER_NORMAL);
				break;
			default:
				break;
		}
		return;
	}

	if ( !Q_stricmp( cmd, "cs" ) ) {
		CG_ConfigStringModified();
		return;
	}

	// core: new configstring handling..
	if( !Q_stricmp( cmd, "gs" ) ) {
		CG_ServerCommand_HandleGS();
		return;
	}

	if( CG_Debriefing_ServerCommand( cmd ) ) {
		return;
	}

	// IRATA: remapShader fix for 2.60 clients - solved with 2.60b
	// see http://www.lucasforums.com/showthread.php?t=140104
	if ( Q_stricmp (cmd, "remapShader") == 0 ) {
		if (trap_Argc() == 4) {
			char shader1[MAX_QPATH];
			char shader2[MAX_QPATH];
			char shader3[MAX_QPATH];

			Q_strncpyz(shader1, CG_Argv(1), sizeof(shader1));
			Q_strncpyz(shader2, CG_Argv(2), sizeof(shader2));
			Q_strncpyz(shader3, CG_Argv(3), sizeof(shader3));

			trap_R_RemapShader(shader1, shader2, shader3);
		}
		return;
	}

	if ( !Q_stricmp( cmd, "vschat" ) ){
		int clNum = atoi(CG_Argv( 1 ));
		int	msgNum = atoi(CG_Argv( 2 ));
		const char* wav = va("%s%s", cgs.clientinfo[clNum].team == TEAM_AXIS ? "axis" : "allies" , HQMessages[msgNum].voiceScript );

		CG_SoundPlaySoundScript( wav, NULL, -1,  qtrue );
		CG_AddToTeamChat( HQMessages[msgNum].chatString, clNum, qfalse );
		CG_Printf( "%s\n", HQMessages[msgNum].chatString );
		return;
	}

	if ( !Q_stricmp( cmd, "tchat" ) ) {
		const	char *s;
		vec3_t	origin;
		char	*locStr = NULL;
		int		clientNum = atoi( CG_Argv( 2 ) );

		if ( atoi( CG_Argv( 3 ) ) ) {
			s = CG_LocalizeServerCommand( CG_Argv( 1 ) );
		}
		else {
			s = CG_Argv( 1 );
		}

		origin[0] = atoi( CG_Argv( 4 ) );
		origin[1] = atoi( CG_Argv( 5 ) );
		origin[2] = atoi( CG_Argv( 6 ) );

		locStr = CG_BuildLocationString( clientNum, origin, LOC_TCHAT );

		if( !locStr || !*locStr ) {
			locStr = "";
		}

		// process locations and name here
		Com_sprintf(text, sizeof(text), "(%s^7)^3(%s):%s",	cgs.clientinfo[clientNum].name, locStr, s);

		CG_RemoveChatEscapeChar( text );
		CG_AddToTeamChat( text, clientNum, qfalse );
		CG_Printf( "%s\n", text ); // JPW NERVE
		CG_WriteToLog( "%s\n", text );
		return;
	}

	if ( !Q_stricmp( cmd, "gamechat" ) ) { // eg. pilot, hq, fire mission
		const	char *s = CG_LocalizeServerCommand( CG_Argv( 1 ) );

		Q_strncpyz( text, s, MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		CG_AddToTeamChat( text, cg.snap->ps.clientNum, qfalse );
		CG_Printf( "%s\n", text ); // JPW NERVE

		return;
	}

	if ( !Q_stricmp( cmd, "vchat" ) ) {
		CG_VoiceChat( SAY_ALL );			// NERVE - SMF - enabled support
		return;
	}

	if ( !Q_stricmp( cmd, "vtchat" ) ) {
		CG_VoiceChat( SAY_TEAM );			// NERVE - SMF - enabled support
		return;
	}

	if ( !Q_stricmp( cmd, "vbchat" ) ) {
		CG_VoiceChat( SAY_BUDDY );
		return;
	}

	// DHM - Nerve :: Allow client to lodge a complaing
	if ( !Q_stricmp( cmd, "complaint" ) && cgs.gamestate == GS_PLAYING) {
		// CHRUKER: b096 - Complaint votes sticks around, so vote no if we have complaints turned off
		if (cg_complaintPopUp.integer == 0) {
			trap_SendClientCommand("vote no");
		} // b096

		cgs.complaintEndTime = cg.time + 20000;
		cgs.complaintClient = atoi( CG_Argv(1) );

		if ( cgs.complaintClient < 0 )
			cgs.complaintEndTime = cg.time + 10000;

		return;
	}
	// dhm

	if ( !Q_stricmp( cmd, "map_restart" ) ) {
		CG_MapRestart();
		return;
	}

	// Gordon: single weapon stat (requested weapon stats)
	if( !Q_stricmp( cmd, "rws" ) ) {
		CG_ParseWeaponStats();
		return;
	}

	if ( !strcmp( cmd, "WeaponStats" ) ) {
		int i, start = 1;

		for( i = WP_KNIFE; i < WP_NUM_WEAPONS; i++ ) {

			if(!BG_ValidStatWeapon( i )) {
				continue;
			}

			cgs.playerStats.weaponStats[i].kills =			atoi(CG_Argv(start++));
			cgs.playerStats.weaponStats[i].killedby =		atoi(CG_Argv(start++));
			cgs.playerStats.weaponStats[i].teamkills =		atoi(CG_Argv(start++));
		}

		cgs.playerStats.suicides = atoi(CG_Argv(start++));

		for( i = 0; i < HR_NUM_HITREGIONS; i++ ) {
			cgs.playerStats.hitRegions[i] = atoi(CG_Argv(start++));
		}

		cgs.numOIDtriggers = atoi(CG_Argv(start++));

		for( i = 0; i < cgs.numOIDtriggers; i++ ) {
			cgs.playerStats.objectiveStats[i] = atoi(CG_Argv(start++));
			cgs.teamobjectiveStats[i] = atoi(CG_Argv(start++));
		}

		return;
	}

	// OSP - match stats
	if(!Q_stricmp(cmd, "sc")) {
		CG_scores_cmd();
		return;
	}

	// OSP - weapon stats parsing
	if(!Q_stricmp(cmd, "ws")) {
		if(cgs.dumpStatsTime > cg.time) {
			CG_dumpStats();
		} else {
			CG_parseWeaponStats_cmd(CG_printConsoleString);
			cgs.dumpStatsTime = 0;
		}

		return;
	}
	if(!Q_stricmp(cmd, "wws")) {
		CG_wstatsParse_cmd();
		return;
	}
	if(!Q_stricmp(cmd, "gstats")) {
		CG_parseWeaponStatsGS_cmd();
		return;
	}

	// OSP - "topshots"-related commands
	if(!Q_stricmp(cmd, "astats")) {
		CG_parseTopShotsStats_cmd(qtrue, CG_printConsoleString);
		return;
	}
	if(!Q_stricmp(cmd, "astatsb")) {
		CG_parseTopShotsStats_cmd(qfalse, CG_printConsoleString);
		return;
	}
	if(!Q_stricmp(cmd, "bstats")) {
		CG_parseBestShotsStats_cmd(qtrue, CG_printConsoleString);
		return;
	}
	if(!Q_stricmp(cmd, "bstatsb")) {
		CG_parseBestShotsStats_cmd(qfalse, CG_printConsoleString);
		return;
	}
	if(!Q_stricmp(cmd, "wbstats")) {
		CG_topshotsParse_cmd(qtrue);
		return;
	}

	if( !Q_stricmp( cmd, "portalcampos" ) ) {
		CG_ParsePortalPos();
		return;
	}

	if ( !Q_stricmp( cmd, "setspawnpt" ) ) {
		cg.selectedSpawnPoint = atoi(CG_Argv(1)) + 1;
		return;
	}

	if( !Q_stricmp( cmd, "reqforcespawn" ) ) {
		if( cg_instanttapout.integer ) {
			CG_ForceTapOut_f();
		}
		else {
			if( cgs.gametype == GT_WOLF_LMS ) {
				trap_UI_Popup( UIMENU_WM_TAPOUT_LMS );
			}
			else {
				trap_UI_Popup( UIMENU_WM_TAPOUT );
			}
		}
		return;
	}

	if ( !Q_stricmp( cmd, "application" ) ) {
		cgs.applicationEndTime = cg.time + 20000;
		cgs.applicationClient = atoi( CG_Argv(1) );

		if ( cgs.applicationClient < 0 )
			cgs.applicationEndTime = cg.time + 10000;

		return;
	}

	if ( !Q_stricmp( cmd, "invitation" ) ) {
		cgs.invitationEndTime = cg.time + 20000;
		cgs.invitationClient = atoi( CG_Argv(1) );

		if ( cgs.invitationClient < 0 )
			cgs.invitationEndTime = cg.time + 10000;

		return;
	}

	if ( !Q_stricmp( cmd, "proposition" ) ) {
		cgs.propositionEndTime = cg.time + 20000;
		cgs.propositionClient = atoi( CG_Argv(1) );
		cgs.propositionClient2 = atoi( CG_Argv(2) );

		if ( cgs.propositionClient < 0 )
			cgs.propositionEndTime = cg.time + 10000;

		return;
	}

	if( !Q_stricmp( cmd, "aft" ) ) {
		cgs.autoFireteamEndTime = cg.time + 20000;
		cgs.autoFireteamNum = atoi( CG_Argv(1) );

		if( cgs.autoFireteamNum < -1 ) {
			cgs.autoFireteamEndTime = cg.time + 10000;
		}
		return;
	}

	if( !Q_stricmp( cmd, "aftc" ) ) {
		cgs.autoFireteamCreateEndTime = cg.time + 20000;
		cgs.autoFireteamCreateNum = atoi( CG_Argv(1) );

		if( cgs.autoFireteamCreateNum < -1 ) {
			cgs.autoFireteamCreateEndTime = cg.time + 10000;
		}
		return;
	}

	if( !Q_stricmp( cmd, "aftj" ) ) {
		cgs.autoFireteamJoinEndTime = cg.time + 20000;
		cgs.autoFireteamJoinNum = atoi( CG_Argv(1) );

		if( cgs.autoFireteamJoinNum < -1 ) {
			cgs.autoFireteamJoinEndTime = cg.time + 10000;
		}
		return;
	}

	//---
	//--- music
	//---

	// loops \/
	if ( !Q_stricmp( cmd, "mu_start" ) ) {	// has optional parameter for fade-up time
		int fadeTime = 0;	// default to instant start
		Q_strncpyz( text, CG_Argv(2), MAX_SAY_TEXT );
		if(*text){
			fadeTime = atoi(text);
		}
		trap_S_StartBackgroundTrack( CG_Argv(1), CG_Argv(1), fadeTime );
		return;
	}
	// plays once then back to whatever the loop was \/
	if ( !Q_stricmp( cmd, "mu_play" ) ) {	// has optional parameter for fade-up time
		int fadeTime = 0;	// default to instant start
		Q_strncpyz( text, CG_Argv(2), MAX_SAY_TEXT );
		if(*text){
			fadeTime = atoi(text);
		}
		trap_S_StartBackgroundTrack( CG_Argv(1), "onetimeonly", fadeTime );
		return;
	}

	if ( !Q_stricmp( cmd, "mu_stop" ) ) {	// has optional parameter for fade-down time
		int fadeTime = 0;	// default to instant stop
		Q_strncpyz( text, CG_Argv(1), MAX_SAY_TEXT );
		if(*text){
			fadeTime = atoi(text);
		}
		trap_S_FadeBackgroundTrack(0.0f, fadeTime, 0);
		trap_S_StartBackgroundTrack( "", "", -2);	// '-2' for 'queue looping track' (QUEUED_PLAY_LOOPED)
		return;
	}

	if ( !Q_stricmp( cmd, "mu_fade" ) ) {
		trap_S_FadeBackgroundTrack(atof(CG_Argv(1)), atoi(CG_Argv(2)), 0 );
		return;
	}

	if ( !Q_stricmp( cmd, "snd_fade" ) ) {
		trap_S_FadeAllSound(atof(CG_Argv(1)), atoi(CG_Argv(2)), atoi(CG_Argv(3)));
		return;
	}
	//---

	// ydnar: bug 267: server sends this command when it's about to kill the current server, before the client can reconnect
	if( !Q_stricmp( cmd, "spawnserver" ) ) {
		// print message informing player the server is restarting with a new map
		CG_PriorityCenterPrint( va( "%s", CG_TranslateString( "^3Server Restarting" ) ), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.25), SMALLCHAR_WIDTH, 999999 );

		// hack here
		cg.serverRespawning = qtrue;
		return;
	}

	if( !Q_stricmpn( cmd, "bindsearch_c", 12 ) ) {
		const char *command = CG_Argv(1);
		C_BindSearch( command );
		return;
	}

	if ( !Q_stricmp( cmd, "rockandroll" ) ) {	// map loaded, game is ready to begin.
		trap_S_FadeAllSound(1.0f, 1000, qfalse);	// fade sound up
		return;
	}

	// ensure a file gets into a build (mainly for scripted music calls)
	if(!Q_stricmp(cmd, "addToBuild")) {
		fileHandle_t f;

		if( !cg_buildScript.integer )
			return;

		// just open the file so it gets copied to the build dir
		trap_FS_FOpenFile( CG_Argv(1), &f, FS_READ );
		trap_FS_FCloseFile( f );
		return;
	}

	if( !Q_stricmp( cmd, "sdbg" ) ) {
		CG_StatsDebugAddText( CG_Argv(1) );
		return;
	}

	CG_Printf( "Unknown client game command: %s\n", cmd );
}


/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this snapshot.
====================
*/
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap_GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}


/*
====================
core:
	C_CSMethodCheck

	In case g_csMethod 1 is enabled:
	This function checks if the client has received all the
	new configstrings from the server..
	(in case he missed some after a vid_restart,
	the client will request the server to send the missing strings again).
====================
*/
#ifdef _DEBUG
#define STR_GS_REQUEST	". Requesting from server..\n"
#define STR_GS_OK		". OK\n"
#endif

void C_CSMethodCheck( void ) {
	int i;
	int wantModels		= 0;
	int wantShaders		= 0;
	int wantSkins		= 0;
	int wantCharacters	= 0;
	int wantSounds		= 0;
	int total			= 0;

	if ( cgs.csMethod == 0 ) return;

#ifdef _DEBUG
	CG_Printf( va("Gamestate for:  models:%i shaders:%i skins:%i characters:%i sounds:%i",
				cgs.csMethod_Models, cgs.csMethod_Shaders, cgs.csMethod_Skins,
				cgs.csMethod_Characters, cgs.csMethod_Sounds) );
#endif

	// models..
	if ( cgs.csMethod_Models > 0 ) {
		for ( i=1; i<=cgs.csMethod_Models; i++ ) {
			if ( !cgs.gameModels[i] ) {
				wantModels = 1;
#ifdef _DEBUG
				CG_Printf( STR_GS_REQUEST );
#endif
				break;
			}
		}
	}
	if ( wantModels ) total += cgs.csMethod_Models;

	// shaders..
	if ( cgs.csMethod_Shaders > 0 ) {
		for ( i=1; i<=cgs.csMethod_Shaders; i++ ) {
			if ( !cgs.gameShaders[i] ) {
				wantShaders = 1;
#ifdef _DEBUG
				CG_Printf( STR_GS_REQUEST );
#endif
				break;
			}
		}
	}
	if ( wantShaders ) total += cgs.csMethod_Shaders;

	// skins..
	if ( cgs.csMethod_Skins > 0 ) {
		for ( i=1; i<=cgs.csMethod_Skins; i++ ) {
			if ( !cgs.gameModelSkins[i] ) {
				wantSkins = 1;
#ifdef _DEBUG
				CG_Printf( STR_GS_REQUEST );
#endif
				break;
			}
		}
	}
	if ( wantSkins ) total += cgs.csMethod_Skins;

	// characters..
	if ( cgs.csMethod_Characters > 0 ) {
		for ( i=1; i<=cgs.csMethod_Characters; i++ ) {
			if ( cgs.gameCharacters[i]->characterFile[0] == 0 ) {
				wantCharacters = 1;
#ifdef _DEBUG
				CG_Printf( STR_GS_REQUEST );
#endif
				break;
			}
		}
	}
	if ( wantCharacters ) total += cgs.csMethod_Characters;

	// sounds..
	if ( cgs.csMethod_Sounds > 0 ) {
		for ( i=1; i<=cgs.csMethod_Sounds; i++ ) {
			if ( !cgs.gameSounds[i] ) {
				wantSounds = 1;
#ifdef _DEBUG
				CG_Printf( STR_GS_REQUEST );
#endif
				break;
			}
		}
	}
	if ( wantSounds ) total += cgs.csMethod_Sounds;

	// core: send a request to the server to (re)send the new configstrings..
	if ( total > 0 ) {
		cgs.csMethod_total = total;
		cgs.csMethod_received = 0;
		cgs.csMethod_requesting = cg.time;
		trap_SendConsoleCommand( va("getgs %i %i %i %i %i\n", wantModels, wantShaders, wantSkins, wantCharacters, wantSounds) );
	} else {
		C_CSMethodDone();
	}

#ifdef _DEBUG
	CG_Printf( STR_GS_OK );
#endif
}


/*
====================
core:
	C_CSMethodDone

	Cancel the g_csMethod 1 mechanism; It's done..
====================
*/
void C_CSMethodDone( void ) {
	cgs.csMethod_refresh = 0;
	cgs.csMethod_request = 0;
	cgs.csMethod_requesting = 0;
	cgs.csMethod_received = 0;
	cgs.csMethod_total = 0;
}

/*
====================
core:
	C_CSMethodRequest

	Start a (new) request for g_csMethod 1 data..
====================
*/
void C_CSMethodRequest( void ) {
	cgs.csMethod_refresh = 0;
	cgs.csMethod_request = 1;
	cgs.csMethod_requesting = 0;
	cgs.csMethod_received = 0;
	cgs.csMethod_total = 0;
}


/*
====================
core:
	C_BindSearch

	search a bind containing a specific command..
====================
*/
void C_BindSearch( const char *command ) {
	char		buf[MAX_STRING_TOKENS];
	int			j;
	qboolean	found = qfalse;
	char		*result = va("%s","");

	if ( !command[0] ) return;
	for ( j=0; j<256; ++j ) {
		trap_Key_GetBindingBuf(j, buf, MAX_STRING_TOKENS);
		if(*buf == 0) continue;
		if ( strstr(buf, command) ) {
			char buf2[MAX_STRING_TOKENS];
			char *keyStr;
			memcpy( buf2, buf, MAX_STRING_TOKENS );
			keyStr = CG_getBindKeyName( buf, buf2, MAX_STRING_TOKENS );
			result = strcat( result, va("key %s => %s\n", keyStr, buf) );
			found = qtrue;
		}
	}
	if ( found ) {
		trap_SendConsoleCommand( va("bindsearch_r %s\n", result) );
	}
}
