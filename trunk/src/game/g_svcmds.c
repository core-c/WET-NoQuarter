/*
 * name:		g_svcmds.c
 *
 * desc:		this file holds commands that can be executed by the server console, but not remote clients
 *
 * NQQS:
 *
 */
#include "g_local.h"

#ifdef ET_LUA
// *LUA*
#include "g_lua.h"
#endif

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

/*
==============================================================================

PACKET FILTERING


You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and you can use '*' to match any value
so you can specify an entire class C network with "addip 192.246.40.*"

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.

TTimo NOTE: GUID functions are copied over from the model of IP banning,
used to enforce max lives independently from server reconnect and team changes (Xian)

TTimo NOTE: for persistence, bans are stored in g_banIPs cvar MAX_CVAR_VALUE_STRING
The size of the cvar string buffer is limiting the banning to around 20 masks
this could be improved by putting some g_banIPs2 g_banIps3 etc. maybe
still, you should rely on PB for banning instead

==============================================================================
*/

#define	MAX_IPFILTERS	1024

typedef struct ipFilterList_s {
	ipFilter_t	ipFilters[MAX_IPFILTERS];
	int			numIPFilters;
	char		cvarIPList[32];
} ipFilterList_t;

static ipFilterList_t		ipFilters;

/*
=================
StringToFilter
=================
*/
qboolean StringToFilter( const char *s, ipFilter_t *f )
{
	char	num[128];
	int		i, j;
	byte	b[4];
	byte	m[4];

	b[0] = 0;
	m[0] = 0;
	b[1] = 0;
	m[1] = 0;
	b[2] = 0;
	m[2] = 0;
	b[3] = 0;
	m[3] = 0;

	for (i=0 ; i<4 ; i++) {
		if (*s < '0' || *s > '9') {
			if (*s == '*') { // 'match any'
				// b[i] and m[i] to 0
				s++;
				if (!*s) {
					break;
				}
				s++;
				continue;
			}
			G_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}

		j = 0;
		while (*s >= '0' && *s <= '9') {
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		m[i] = 255;

		if (!*s) {
			break;
		}
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans( ipFilterList_t *ipFilterList )
{
	byte	b[4];
	byte	m[4];
	int		i,j;
	char	iplist_final[MAX_CVAR_VALUE_STRING];
	char	ip[64];

	*iplist_final = 0;
	for (i = 0 ; i < ipFilterList->numIPFilters ; i++) {
		if( ipFilterList->ipFilters[i].compare == 0xffffffff ) {
			continue;
		}

		*(unsigned *)b = ipFilterList->ipFilters[i].compare;
		*(unsigned *)m = ipFilterList->ipFilters[i].mask;
		*ip = 0;
		for( j = 0; j < 4 ; j++ ) {
			if( m[j] != 255 )
				Q_strcat(ip, sizeof(ip), "*");
			else
				Q_strcat(ip, sizeof(ip), va("%i", b[j]));
			Q_strcat(ip, sizeof(ip), (j<3) ? "." : " ");
		}
		if( strlen(iplist_final)+strlen(ip) < MAX_CVAR_VALUE_STRING ) {
			Q_strcat( iplist_final, sizeof(iplist_final), ip);
		}
		else {
			Com_Printf( "%s overflowed at MAX_CVAR_VALUE_STRING\n", ipFilterList->cvarIPList );
			break;
		}
	}

	trap_Cvar_Set( ipFilterList->cvarIPList, iplist_final );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket( ipFilterList_t *ipFilterList, char *from )
{
	int		i = 0;
	unsigned	in;
	byte m[4];
	char *p = from;

	while (*p && i < 4) {
		m[i] = 0;
		while (*p >= '0' && *p <= '9') {
			m[i] = m[i]*10 + (*p - '0');
			p++;
		}
		if (!*p || *p == ':')
			break;
		i++, p++;
	}

	in = *(unsigned *)m;

	for( i = 0; i < ipFilterList->numIPFilters; i++ ) {
		if( (in & ipFilterList->ipFilters[i].mask) == ipFilterList->ipFilters[i].compare) {
 			return g_filterBan.integer != 0;
		}
	}
	return g_filterBan.integer == 0;
}

qboolean G_FilterIPBanPacket( char *from )
{
	return( G_FilterPacket( &ipFilters, from ) );
}

/*
=================
AddIP
=================
*/
void AddIP( ipFilterList_t *ipFilterList, const char *str )
{
	int		i;

	for( i = 0; i < ipFilterList->numIPFilters; i++ ) {
		if(  ipFilterList->ipFilters[i].compare == 0xffffffff )
			break;		// free spot
	}

	if( i == ipFilterList->numIPFilters ) {
		if( ipFilterList->numIPFilters == MAX_IPFILTERS ) {
			G_Printf( "IP filter list is full\n" );
			return;
		}
		ipFilterList->numIPFilters++;
	}

	if( !StringToFilter( str, &ipFilterList->ipFilters[i] ) )
		ipFilterList->ipFilters[i].compare = 0xffffffffu;

	UpdateIPBans( ipFilterList );
}

void AddIPBan( const char *str )
{
	AddIP( &ipFilters, str );
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans(void)
{
	char *s, *t;
	char str[MAX_CVAR_VALUE_STRING];

	ipFilters.numIPFilters = 0;
	Q_strncpyz( ipFilters.cvarIPList, "g_banIPs", sizeof(ipFilters.cvarIPList) );

	Q_strncpyz( str, g_banIPs.string, sizeof(str) );

	for( t = s = g_banIPs.string; *t; /* */ ) {
		s = strchr(s, ' ');
		if( !s )
			break;
		while( *s == ' ' )
			*s++ = 0;
		if( *t )
			AddIP( &ipFilters, t );
		t = s;
	}
}

/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f (void)
{
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf( "Usage:  addip <ip-mask>\n" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	AddIP( &ipFilters, str );
}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f (void)
{
	ipFilter_t	f;
	int			i;
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage:  removeip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	if( !StringToFilter( str, &f ) ) {
		return;
	}

	for (i=0 ; i< ipFilters.numIPFilters ; i++) {
		if( ipFilters.ipFilters[i].mask == f.mask	&&
			ipFilters.ipFilters[i].compare == f.compare ) {
			ipFilters.ipFilters[i].compare = 0xffffffffu;
			G_Printf( "Removed.\n" );

			UpdateIPBans( &ipFilters );
			return;
		}
	}

	G_Printf( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void	Svcmd_EntityList_f (void) {
	int			e, entsFree = 0;
	gentity_t	*check = g_entities+1;

	for (e = 1; e < MAX_GENTITIES ; e++, check++) {
		if ( !check->inuse ) {
			G_Printf("%4i: %s\n", e, check->classname);
			entsFree++;
			continue;
		}
		G_Printf("%4i:", e);
		switch ( check->s.eType ) {
		case ET_GENERAL:
			G_Printf("ET_GENERAL           ");
			break;
		case ET_PLAYER:
			G_Printf("ET_PLAYER            ");
			break;
		case ET_ITEM:
			G_Printf("ET_ITEM              ");
			break;
		case ET_MISSILE:
			G_Printf("ET_MISSILE           ");
			break;
		case ET_MOVER:
			G_Printf("ET_MOVER             ");
			break;
		case ET_BEAM:
			G_Printf("ET_BEAM              ");
			break;
		case ET_PORTAL:
			G_Printf("ET_PORTAL            ");
			break;
		case ET_SPEAKER:
			G_Printf("ET_SPEAKER           ");
			break;
		case ET_PUSH_TRIGGER:
			G_Printf("ET_PUSH_TRIGGER      ");
			break;
		case ET_CONCUSSIVE_TRIGGER:
			G_Printf("ET_CONCUSSIVE_TRIGGR ");
			break;
		case ET_TELEPORT_TRIGGER:
			G_Printf("ET_TELEPORT_TRIGGER  ");
			break;
		case ET_INVISIBLE:
			G_Printf("ET_INVISIBLE         ");
			break;
		case ET_EXPLOSIVE:
			G_Printf("ET_EXPLOSIVE         ");
			break;
		case ET_ALARMBOX:
			G_Printf("ET_ALARMBOX          ");
			break;
		case ET_MISSILECAM:
			G_Printf("ET_MISSILECAM        ");
			break;
		default:
			G_Printf("%4i                  ", check->s.eType);
			break;
		}

		if ( check->classname ) {
			G_Printf("%s", check->classname);
		}
		G_Printf("\n");
	}
#if defined(__x86_64__)
	G_Printf("%4i: first_free_entity\n", (int) (first_free_entity - g_entities) );
	G_Printf("%4i: last_free_entity\n", (int)(last_free_entity - g_entities));
#else
	G_Printf("%4i: first_free_entity\n", first_free_entity - g_entities );
	G_Printf("%4i: last_free_entity\n", last_free_entity - g_entities);
#endif
	G_Printf("%4i: entities not in use\n", entsFree);
	G_Printf("%4i: num_entities\n", level.num_entities);
}


// fretn, note: if a player is called '3' and there are only 2 players
// on the server (clientnum 0 and 1)
// this function will say 'client 3 is not connected'
// solution: first check for usernames, if none is found, check for slotnumbers
gclient_t	*ClientForString( const char *s ) {
	gclient_t	*cl;
	int			i;

	// check for a name match
	for ( i=0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		int idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			G_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	G_Printf( "User %s is not on the server\n", s );

	return NULL;
}

// fretn

static qboolean G_Is_SV_Running( void ) {
	char		cvar[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "sv_running", cvar, sizeof( cvar ) );
	return (qboolean)atoi( cvar );
}

/*
==================
G_GetPlayerByNum
==================
*/
gclient_t	*G_GetPlayerByNum( int clientNum ) {
	gclient_t	*cl;

	// make sure server is running
	if ( !G_Is_SV_Running() ) {
		return NULL;
	}

	if ( trap_Argc() < 2 ) {
		G_Printf( "No player specified.\n" );
		return NULL;
	}

	if ( clientNum < 0 || clientNum >= level.maxclients ) {
		Com_Printf( "Bad client slot: %i\n", clientNum );
		return NULL;
	}

	cl = &level.clients[clientNum];
	if ( cl->pers.connected == CON_DISCONNECTED ) {
		G_Printf( "Client %i is not connected\n", clientNum );
		return NULL;
	}

	if (cl)
		return cl;


	G_Printf( "User %d is not on the server\n", clientNum );

	return NULL;
}

/*
==================
G_GetPlayerByName
==================
*/
gclient_t *G_GetPlayerByName( char *name ) {
	int			i;
	gclient_t	*cl;
	char		cleanName[64];

	// make sure server is running
	if ( !G_Is_SV_Running() ) {
		return NULL;
	}

	if ( trap_Argc() < 2 ) {
		G_Printf( "No player specified.\n" );
		return NULL;
	}

	for (i = 0; i < level.numConnectedClients; i++) {

		 // CHRUKER: b097 - Kick command is not able to kick some players by name
		cl = &level.clients[level.sortedClients[i]];

		if (!Q_stricmp(cl->pers.netname, name)) {
			return cl;
		}

		Q_strncpyz( cleanName, cl->pers.netname, sizeof(cleanName) );
		Q_CleanStr( cleanName );
		if ( !Q_stricmp( cleanName, name ) ) {
			return cl;
		}
	}

	G_Printf( "Player %s is not on the server\n", name );

	return NULL;
}

// -fretn

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void Svcmd_ForceTeam_f( void ) {
	gclient_t	*cl;
	char		str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) {
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str, qfalse, cl->sess.playerWeapon, cl->sess.playerWeapon2, qtrue );
}

/*
============
Svcmd_StartMatch_f

NERVE - SMF - starts match if in tournament mode
============
*/
void Svcmd_StartMatch_f(void)
{
	G_refAllReady_cmd(NULL);
}

/*
==================
Svcmd_ResetMatch_f

OSP - multiuse now for both map restarts and total match resets
==================
*/
void Svcmd_ResetMatch_f(qboolean fDoReset, qboolean fDoRestart)
{
	int i;

	for(i=0; i<level.numConnectedClients; i++) {
		g_entities[level.sortedClients[i]].client->pers.ready = 0;
	}

	if(fDoReset) {
		G_resetRoundState();
		G_resetModeState();
	}

	if(fDoRestart) {
		int mode = GS_RESET;

		switch(g_gamestate.integer) {
		case GS_PLAYING:
		case GS_INTERMISSION:
			mode = GS_WARMUP;
		}
		trap_SendConsoleCommand(EXEC_APPEND,
			va("map_restart 0 %i\n", mode));
	}
	// tjw: added so that a test can be done when on the first map
	//      of a campaign to determine if this is a new campaign
	//      or just a re-run of it's first map
	trap_Cvar_Set("g_reset", "1");
}

/*
============
Svcmd_SwapTeams_f

NERVE - SMF - swaps all clients to opposite team
============
*/
void Svcmd_SwapTeams_f(void)
{
	G_resetRoundState();

	if ( (g_gamestate.integer == GS_INITIALIZE) ||
		 (g_gamestate.integer == GS_WARMUP) ||
		 (g_gamestate.integer == GS_RESET) ) {
		G_swapTeams();
		return;
	}

	G_resetModeState();

	trap_Cvar_Set( "g_swapteams", "1" );
	Svcmd_ResetMatch_f(qfalse, qtrue);
}


/*
====================
Svcmd_ShuffleTeams_f

OSP - randomly places players on teams
====================
*/
void Svcmd_ShuffleTeams_f( qboolean restart)
{
	if ( restart )
	G_resetRoundState();

	G_shuffleTeams();

	if((g_gamestate.integer == GS_INITIALIZE) ||
	  (g_gamestate.integer == GS_WARMUP) ||
	  (g_gamestate.integer == GS_RESET)) {
		return;
	}
	if ( restart ){
		G_resetModeState();
		Svcmd_ResetMatch_f(qfalse, qtrue);
	}
}

void Svcmd_Campaign_f(void) {
	char	str[MAX_TOKEN_CHARS];
	int		i;
	g_campaignInfo_t *campaign = NULL;

	// find the campaign
	trap_Argv( 1, str, sizeof( str ) );

	for( i = 0; i < level.campaignCount; i++ ) {
		campaign = &g_campaigns[i];

		if( !Q_stricmp( campaign->shortname, str ) ) {
			break;
		}
	}

	if( i == level.campaignCount || !(campaign->typeBits & (1 << GT_WOLF) ) ) {
		G_Printf( "Can't find campaign '%s'\n", str );
		return;
	}

	trap_Cvar_Set( "g_oldCampaign", g_currentCampaign.string );
	trap_Cvar_Set( "g_currentCampaign", campaign->shortname );
	trap_Cvar_Set( "g_currentCampaignMap", "0" );

	level.newCampaign = qtrue;

	// we got a campaign, start it
	trap_Cvar_Set( "g_gametype", va( "%i", GT_WOLF_CAMPAIGN ) );
#if 0
	if( g_developer.integer )
		trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s\n", campaign->mapnames[0] ) );
	else
#endif
		trap_SendConsoleCommand( EXEC_APPEND, va( "map %s\n", campaign->mapnames[0] ) );
}

void Svcmd_ListCampaigns_f(void) {
	int i, mpCampaigns = 0;

	for( i = 0; i < level.campaignCount; i++ ) {
		if( g_campaigns[i].typeBits & (1 << GT_WOLF) )
			mpCampaigns++;
	}

	if( mpCampaigns ) {
		G_Printf( "%i campaigns found:\n", mpCampaigns );
	}
	else {
		G_Printf( "No campaigns found.\n" );
		return;
	}

	for( i = 0; i < level.campaignCount; i++ ) {
		if( g_campaigns[i].typeBits & (1 << GT_WOLF) )
			G_Printf( " %s\n", g_campaigns[i].shortname );
	}
}



// ydnar: modified from maddoc sp func
extern void ReviveEntity(gentity_t *ent, gentity_t *traceEnt, qboolean resuscitation);
extern int FindClientByName( char *name );

void Svcmd_RevivePlayer( char *name )
{
	int			clientNum;
	gentity_t	*player;

	if( !g_cheats.integer ) {
		trap_SendServerCommand( -1, va( "print \"Cheats are not enabled on this server.\n\"" ) );
		return;
	}

	clientNum = FindClientByName( name );
	if( clientNum < 0 ) {
		return;
	}
	player = &g_entities[ clientNum ];

	ReviveEntity( player, player, qfalse );
}

/*
==================
Svcmd_Kick_f

Kick a user off of the server
==================
*/

// change into qfalse if you want to use the qagame banning system
// which makes it possible to unban IP addresses
#define USE_ENGINE_BANLIST qtrue

static void Svcmd_Kick_f( void )
{
	gclient_t	*cl;
	int			timeout = -1;
	char		name[MAX_TOKEN_CHARS];

	// make sure server is running
	if ( !G_Is_SV_Running() ) {
		G_Printf( "Server is not running.\n" );
		return;
	}

	if ( trap_Argc() < 2 || trap_Argc() > 3 ) {
		G_Printf ("Usage: kick <player name> [timeout]\n");
		return;
	}

	if( trap_Argc() == 3 ) {
		char		sTimeout[MAX_TOKEN_CHARS];
		trap_Argv( 2, sTimeout, sizeof( sTimeout ) );
		timeout = atoi( sTimeout );
	}
	else {
		timeout = 300;
	}

	trap_Argv(1, name, sizeof(name));
	cl = G_GetPlayerByName( name );//ClientForString( name );

	if ( !cl ) {
		if ( !Q_stricmp(name, "all") ) {
			int i;

			for (i = 0, cl = level.clients; i < level.numConnectedClients; i++, cl++) {
				// dont kick localclients ...
				if ( cl->pers.localClient ) {
					continue;
				}

				if ( timeout != -1 ) {
					char* 	ip = level.clients[cl->ps.clientNum].pers.client_ip;

					// use engine banning system, mods may choose to use their own banlist
					if (USE_ENGINE_BANLIST) {

						// kick but dont ban bots, they arent that lame
						if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) ) {
							timeout = 0;
						}

						trap_DropClient(cl->ps.clientNum, "player kicked", timeout);
					}
					else {
						trap_DropClient(cl->ps.clientNum, "player kicked", 0);

						// kick but dont ban bots, they arent that lame
						if ( !(g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) )
							AddIPBan( ip );
					}

				}
				else {
					trap_DropClient(cl->ps.clientNum, "player kicked", 0);
				}
			}
		}
		return;
	}
	else {
		// dont kick localclients ...
		if ( cl->pers.localClient ) {
			G_Printf("Cannot kick host player\n");
			return;
		}

		if ( timeout != -1 ) {
			char* 	ip = level.clients[cl->ps.clientNum].pers.client_ip;

			// use engine banning system, mods may choose to use their own banlist
			if (USE_ENGINE_BANLIST) {

				// kick but dont ban bots, they arent that lame
				if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) ) {
					timeout = 0;
				}
				trap_DropClient(cl->ps.clientNum, "player kicked", timeout);
			}
			else {
				trap_DropClient(cl->ps.clientNum, "player kicked", 0);

				// kick but dont ban bots, they arent that lame
				if ( !(g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) ) {
					AddIPBan( ip );
				}
			}

		}
		else {
			trap_DropClient(cl->ps.clientNum, "player kicked", 0);
		}
	}
}

/*
==================
Svcmd_KickNum_f

Kick a user off of the server
==================
*/
static void Svcmd_KickNum_f( void )
{
	gclient_t	*cl;
	int 	timeout = -1;
	char	*ip;
	char	name[MAX_TOKEN_CHARS];
	int		clientNum;

	// make sure server is running
	if ( !G_Is_SV_Running() ) {
		G_Printf( "Server is not running.\n" );
		return;
	}

	if ( trap_Argc() < 2 || trap_Argc() > 3 ) {
		G_Printf ("Usage: kick <client number> [timeout]\n");
		return;
	}

	if( trap_Argc() == 3 ) {
		char	sTimeout[MAX_TOKEN_CHARS];

		trap_Argv( 2, sTimeout, sizeof( sTimeout ) );
		timeout = atoi( sTimeout );
	}
	else {
		timeout = 300;
	}

	trap_Argv(1, name, sizeof(name));
	clientNum = atoi(name);

	cl = G_GetPlayerByNum( clientNum );
	if ( !cl ) {
		return;
	}
	if ( cl->pers.localClient ) {
		G_Printf("Cannot kick host player\n");
		return;
	}

	ip = level.clients[cl->ps.clientNum].pers.client_ip;
	// use engine banning system, mods may choose to use their own banlist
	if (USE_ENGINE_BANLIST) {

		// kick but dont ban bots, they arent that lame
		if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) ) {
			timeout = 0;
		}
		trap_DropClient(cl->ps.clientNum, "player kicked", timeout);
	}
	else {
		trap_DropClient(cl->ps.clientNum, "player kicked", 0);

		// kick but dont ban bots, they arent that lame
		if ( !(g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) )
			AddIPBan( ip );
	}
}

qboolean ConsoleChat(qboolean chatClient)
{
	int 	clientNum = -1;
	char 	line[MAX_SAY_TEXT];
	int 	i;
	char 	*s;
	int 	pids[MAX_CLIENTS];
	char 	name[MAX_NAME_LENGTH];

	if(chatClient) {
		int 	pcount = 0;

		if(trap_Argc() < 3) {
			G_Printf("usage: chatclient [slot#|name] message\n");
			return qfalse;
		}
		trap_Argv(1, name, sizeof(name));
		pcount = ClientNumbersFromString(name, pids);
		if(pcount > 1) {
			G_Printf("chatclient: more than one player matches."
				"nothing sent.\n");
			return qfalse;
		}
		if(pcount < 1) {
			G_Printf("chatclient: no player found\n");
			return qfalse;
		}
		clientNum = pids[0];
		s = Q_AddCR(ConcatArgs(2));
	}
	else {
		s = Q_AddCR(ConcatArgs(1));
	}

	i = 0;
	line[0] = '\0';
	while(*s) {
		if(*s == '\n' || i == (MAX_SAY_TEXT - 1)) {
			line[i] = '\0';
			CPx(clientNum, va(
				"chat \"%s\" -1 0",
				line));
			line[0] = '\0';
			i = 0;
			*s++;
			continue;
		}
		line[i] = *s;
		*s++;
		i++;
	}
	if(line[0]) {
		line[i] = '\0';
		CPx(clientNum, va("chat \"%s\" -1 0",line));
	}
	if (g_logOptions.integer & LOGOPTS_CHATS) {
		if(chatClient) {
			G_LogPrintf(va("chat(client): %d: %s\n",
				clientNum,
				ConcatArgs(1)));
		}
		else {
			G_LogPrintf(va("chat(client): %s\n",
				ConcatArgs(1)));
		}
	}
	return qtrue;
}

void G_ClearXP()
{
	int i, j;
	gentity_t *ent = &g_entities[0];

	G_xpsave_resetxp();

	for(i = 0; i < g_maxclients.integer; i++) {
		trap_Cvar_Set(va("sessionstats%i", i), "");
		// tjw: in case we add this back in
		trap_Cvar_Set(va("wstats%i", i), "");

		ent = &g_entities[i];
		if(ent && ent->inuse && ent->client) {
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				ent->client->sess.skillpoints[j] = 0;
				ent->client->sess.medals[j] = 0;
			}
			G_deleteStats(i);
		}
	}
}

#ifdef HW_BAN
void Cmd_ShowHWGuid_f() {
	Com_Printf( "HWGuid: %s\n", getHWGuid());
}

void Cmd_ShowHWInfo_f() {
	Com_Printf("%s\n", getHWInfo());
}
#endif // HW_BAN

void G_UpdateForceCvars(void)
{
	char cs[MAX_INFO_STRING];
	int	i;

	cs[0] = '\0';

	for ( i=0; i < level.forceCvarCount; i++ ){
		Info_SetValueForKey(cs, va("FCV%i", i), va("%s %s", level.forceCvars[i].cvarName, level.forceCvars[i].cvarValue) );
	}

	Info_SetValueForKey(cs, "NUM", va("%i", level.forceCvarCount) );
	trap_SetConfigstring(CS_FORCECVAR, cs);
}

void G_UpdateSvCvars(void)
{
	char cs[MAX_INFO_STRING];
	int	i;

	cs[0] = '\0';

	for ( i=0; i < level.svCvarsCount; i++ ){
		Info_SetValueForKey(cs, va("SVCV%i", i) ,
		va("%i %s %s %s", level.svCvars[i].mode, level.svCvars[i].cvarName, level.svCvars[i].Val1, level.svCvars[i].Val2 ) );
	}

	Info_SetValueForKey(cs, "NUM", va("%i", level.svCvarsCount) );

	trap_SetConfigstring(CS_SVCVAR, cs);
}

void CC_csinfo(void)
{
	int			i = 0, j;
	char		cs[BIG_INFO_STRING];
	char		cspart[MAX_TOKEN_CHARS];
	char		valuestr[MAX_TOKEN_CHARS];
	int			value = -1;
	int			size = 0;
	int			total = 0;
	char		*str = NULL;
	qboolean	arg1 = ( trap_Argc() > 1 )? qtrue : qfalse;
	qboolean	arg1numeric = qtrue;

	valuestr[0] = 0;
	if ( arg1 ) {
		trap_Argv( 1, valuestr, sizeof(valuestr) );
		for ( i=0; i<strlen(valuestr); i++ ) {
			if ( valuestr[i] < '0' || valuestr[i] > '9' ) {
				arg1numeric = qfalse;
				break;
			}
		}
		if ( arg1numeric ) {
			value = atoi(valuestr);
			if ( value >= MAX_CONFIGSTRINGS ) {
				value = -1;
			}
		}
	}

	G_Printf("CS   Length   Type\n--------------------------------------------\n");
	for(i=0; i<MAX_CONFIGSTRINGS; i++) {
		trap_GetConfigstring(i, cs, sizeof(cs));
		size = strlen(cs);
		total += size;
		if ( size == 0 ) continue;

		switch (i) {
			case CS_SERVERINFO:
				str = "CS_SERVERINFO";
				break;
			case CS_SYSTEMINFO:
				str = "CS_SYSTEMINFO";
				break;
			case CS_MUSIC:
				str = "CS_MUSIC";
				break;
			case CS_MUSIC_QUEUE:
				str = "CS_MUSIC_QUEUE";
				break;
			case CS_WARMUP:
				str = "CS_WARMUP";
				break;
			case CS_VOTE_STRING:
				str = "CS_VOTE_STRING";
				break;
			case CS_VOTE_YES:
				str = "CS_VOTE_YES";
				break;
			case CS_VOTE_NO:
				str = "CS_VOTE_NO";
				break;
			case CS_GAME_VERSION:
				str = "CS_GAME_VERSION";
				break;
			case CS_LEVEL_START_TIME:
				str = "CS_LEVEL_START_TIME";
				break;
			case CS_INTERMISSION:
				str = "CS_INTERMISSION";
				break;
			case CS_MULTI_INFO:
				str = "CS_MULTI_INFO";
				break;
			case CS_MULTI_MAPWINNER:
				str = "CS_MULTI_MAPWINNER";
				break;
			case CS_MULTI_OBJECTIVE:
				str = "CS_MULTI_OBJECTIVE";
				break;
			case CS_SCREENFADE:
				str = "CS_SCREENFADE";
				break;
			case CS_FOGVARS:
				str = "CS_FOGVARS";
				break;
			case CS_SKYBOXORG:
				str = "CS_SKYBOXORG";
				break;
			case CS_TARGETEFFECT:
				str = "CS_TARGETEFFECT";
				break;
			case CS_WOLFINFO:
				str = "CS_WOLFINFO";
				break;
			case CS_FIRSTBLOOD:
				str = "CS_FIRSTBLOOD";
				break;
			case CS_ROUNDSCORES1:
				str = "CS_ROUNDSCORES1";
				break;
			case CS_ROUNDSCORES2:
				str = "CS_ROUNDSCORES2";
				break;
			case CS_SCRIPT_MOVER_NAMES:
				str = "CS_SCRIPT_MOVER_NAMES";
				break;
			case CS_CONSTRUCTION_NAMES:
				str = "CS_CONSTRUCTION_NAMES";
				break;
			case CS_REINFSEEDS:
				str = "CS_REINFSEEDS";
				break;
			case CS_SERVERTOGGLES:
				str = "CS_SERVERTOGGLES";
				break;
			case CS_GLOBALFOGVARS:
				str = "CS_GLOBALFOGVARS";
				break;
			case CS_AXIS_MAPS_XP:
				str = "CS_AXIS_MAPS_XP";
				break;
			case CS_ALLIED_MAPS_XP:
				str = "CS_ALLIED_MAPS_XP";
				break;
			case CS_INTERMISSION_START_TIME:
				str = "CS_INTERMISSION_START_TIME";
				break;
			case CS_ENDGAME_STATS:
				str = "CS_ENDGAME_STATS";
				break;
			case CS_CHARGETIMES:
				str = "CS_CHARGETIMES";
				break;
			case CS_FILTERCAMS:
				str = "CS_FILTERCAMS";
				break;
			case CS_NOQUARTERINFO:
				str = "CS_NOQUARTERINFO";
				break;
			case CS_SKILLLEVELS:
				str = "CS_SKILLLEVELS";
				break;
			case CS_FORCECVAR:
				str = "CS_FORCECVAR";
				break;
			case CS_SVCVAR:
				str = "CS_SVCVAR";
				break;
			case CS_CONFIGNAME:
				str = "CS_CONFIGNAME";
				break;
			case CS_CSMETHODINFO:
				str = "CS_CSMETHODINFO";
				break;
			case CS_SHADERSTATE:
				str = "CS_SHADERSTATE";
				break;
			default:
				if (i>=CS_MODELS && i<CS_MODELS+MAX_MODELS) {
					str = "CS_MODELS";
					break;
				}
				else if (i>=CS_SOUNDS && i<CS_SOUNDS+MAX_SOUNDS) {
					str = "CS_SOUNDS";
					break;
				}
				else if (i>=CS_SHADERS && i<CS_SHADERS+MAX_CS_SHADERS) {
					str = "CS_SHADERS";
					break;
				}
				else if (i>=CS_SKINS && i<CS_SKINS+MAX_CS_SKINS) {
					str = "CS_SKINS";
					break;
				}
				else if (i>=CS_CHARACTERS && i<CS_CHARACTERS+MAX_CHARACTERS) {
					str = "CS_CHARACTERS";
					break;
				}
				else if (i>=CS_PLAYERS && i<CS_PLAYERS+MAX_CLIENTS) {
					str = "CS_PLAYERS";
					break;
				}
				else if (i>=CS_MULTI_SPAWNTARGETS && i<CS_MULTI_SPAWNTARGETS+MAX_MULTI_SPAWNTARGETS) {
					str = "CS_MULTI_SPAWNTARGETS";
					break;
				}
				else if (i>=CS_OID_TRIGGERS && i<CS_OID_TRIGGERS+MAX_OID_TRIGGERS) {
					str = "CS_OID_TRIGGERS";
					break;
				}
				else if (i>=CS_OID_DATA && i<CS_OID_DATA+MAX_OID_TRIGGERS) {
					str = "CS_OID_DATA";
					break;
				}
				else if (i>=CS_DLIGHTS && i<CS_DLIGHTS+MAX_DLIGHT_CONFIGSTRINGS) {
					str = "CS_DLIGHTS";
					break;
				}
				else if (i>=CS_SPLINES && i<CS_SPLINES+MAX_SPLINE_CONFIGSTRINGS) {
					str = "CS_SERVERINFO";
					break;
				}
				else if (i>=CS_TAGCONNECTS && i<CS_TAGCONNECTS+MAX_TAGCONNECTS) {
					str = "CS_TAGCONNECTS";
					break;
				}
				else if (i>=CS_FIRETEAMS && i<CS_FIRETEAMS+MAX_FIRETEAMS) {
					str = "CS_FIRETEAMS";
					break;
				}
				else if (i>=CS_CUSTMOTD && i<CS_CUSTMOTD+MAX_MOTDLINES) {
					str = "CS_CUSTMOTD";
					break;
				}
				else if (i>=CS_STRINGS && i<CS_STRINGS+MAX_CSSTRINGS) {
					str = "CS_STRINGS";
					break;
				}
				else {
					str = "";
				}
				break;
		}

		if ( arg1 ) {
			if ( (arg1numeric && value == i) || (!arg1numeric && !Q_stricmp(valuestr,str)) ) {
				G_Printf(va("%-4i %-8i %s\n", i, size, str));
				// value 239 is taken from SBP()
				for ( j=0; j<=(int)(size/(239-1)); j++ ) {
					Q_strncpyz( cspart, (char *)&cs[j*(239-1)], 239 );
					G_Printf(va("%s", cspart));
				}
				G_Printf("\n");
			}
		} else {
			G_Printf(va("%-4i %-8i %s\n", i, size, str));
		}
	}
	G_Printf(va("--------------------------------------------\nTotal CONFIGSTRING Length: %i\n--------------------------------------------\n", total));
}

void CC_csdump(void)
{
	char	csnum_str[4];
	int		csnum = 0;
	char	cs[BIG_INFO_STRING];
	char	cspart[MAX_TOKEN_CHARS];
	int		size = 0;
	int		j;

	if(trap_Argc() != 2) {
		G_Printf("usage: csdump [csnum]\n");
		return;
	}
	trap_Argv(1, csnum_str, sizeof(csnum_str));
	csnum = atoi(csnum_str);
	if(csnum >= MAX_CONFIGSTRINGS || csnum < 0) {
		G_Printf("csdump: illegal csnum\n");
		return;
	}
	trap_GetConfigstring(csnum, cs, sizeof(cs));
	size = strlen(cs);
	for ( j=0; j<=(int)(size/(MAX_TOKEN_CHARS-1)); j++ ) {
		Q_strncpyz( cspart, (char *)&cs[j*(MAX_TOKEN_CHARS-1)], MAX_TOKEN_CHARS );
		G_Printf( "%s", cspart );
	}
	G_Printf( "\n" );
	return;
}

void CC_forcecvar(void)
{
	// tjw: what's the max for a cvar name?
	char	cvarName[MAX_CVAR_VALUE_STRING];
	char	cvarValue[MAX_CVAR_VALUE_STRING];
	int		i;
	int		index = level.forceCvarCount;

	if(trap_Argc() != 3) {
		G_Printf("usage: forcecvar <cvar name> <cvar value>\n");
		return;
	}
	trap_Argv(1, cvarName, sizeof(cvarName));
	trap_Argv(2, cvarValue, sizeof(cvarValue));

	// core: is this cvar already in the array?.. (maybe they have a double entry)
	for ( i=0; i<level.forceCvarCount; i++ ) {
		if ( !Q_stricmp(cvarName, level.forceCvars[i].cvarName) ) {
			index = i;
		}
	}

	// tjw: add the cvar to the list that is fed to
	//      connecting clients
	if ( index >= MAX_FORCECVARS ) {
		G_Printf("forcecvar: MAX_FORCECVARS hit\n");
		return;
	}

	Q_strncpyz(level.forceCvars[index].cvarName, cvarName, sizeof(level.forceCvars[0].cvarName));
	Q_strncpyz(level.forceCvars[index].cvarValue, cvarValue, sizeof(level.forceCvars[0].cvarValue));

	// cvar wasn't yet in the array?
	if ( index >= level.forceCvarCount ) {
		level.forceCvarCount++;
	}

	// tjw: prevent spamming the client with this twice
	//      if the command is issued this early nobody
	//      should be connected yet.
	/*if((level.time - level.startTime) >
		(SERVER_FRAMETIME_DELTA * GAME_INIT_FRAMES))  {

		trap_SendServerCommand(-1,
			va("forcecvar \"%s\" \"%s\"",
			cvarName, cvarValue));
	}*/

	G_UpdateForceCvars();
}

void CC_svcvar(void)
{
	char	cvarName[MAX_CVAR_VALUE_STRING];
	char	mode[16];
	char	cvarValue1[MAX_CVAR_VALUE_STRING];
	char	cvarValue2[MAX_CVAR_VALUE_STRING];
	int		i;
	int		index = level.svCvarsCount;

	if(trap_Argc() <= 3) {
		G_Printf("usage: sv_cvar <cvar name> <mode> <value1> <value2>\n");
		return;
	}
	trap_Argv(1, cvarName, sizeof(cvarName));
	trap_Argv(2, mode, sizeof(mode));
	trap_Argv(3, cvarValue1, sizeof(cvarValue1));

	if(trap_Argc() == 5)
		trap_Argv(4, cvarValue2, sizeof(cvarValue2));

	// core: is this cvar already in the array?.. (maybe they have a double entry)
	for ( i=0; i<level.svCvarsCount; i++ ) {
		if ( !Q_stricmp(cvarName, level.svCvars[i].cvarName) ) {
			index = i;
		}
	}

	if ( index >= MAX_SVCVARS ) {
		G_Printf("sv_cvar: MAX_SVCVARS hit\n");
		return;
	}

	if ( !Q_stricmp( mode, "EQ") || !Q_stricmp( mode, "EQUAL") )
		level.svCvars[index].mode = SVC_EQUAL;
	else if ( !Q_stricmp( mode, "G") || !Q_stricmp( mode, "GREATER"))
		level.svCvars[index].mode = SVC_GREATER;
	else if ( !Q_stricmp( mode, "GE") || !Q_stricmp( mode, "GREATEREQUAL"))
		level.svCvars[index].mode = SVC_GREATEREQUAL;
	else if ( !Q_stricmp( mode, "L") || !Q_stricmp( mode, "LOWER"))
		level.svCvars[index].mode = SVC_LOWER;
	else if ( !Q_stricmp( mode, "LE") || !Q_stricmp( mode, "LOWEREQUAL"))
		level.svCvars[index].mode = SVC_LOWEREQUAL;
	else if (  !Q_stricmp( mode, "IN") || !Q_stricmp( mode, "INSIDE"))
		level.svCvars[index].mode = SVC_INSIDE;
	else if (  !Q_stricmp( mode, "OUT") || !Q_stricmp( mode, "OUTSIDE"))
		level.svCvars[index].mode = SVC_OUTSIDE;
	else if ( !Q_stricmp( mode, "INC") || !Q_stricmp( mode, "INCLUDE") )
		level.svCvars[index].mode = SVC_INCLUDE;
	else if ( !Q_stricmp( mode, "EXC") || !Q_stricmp( mode, "EXCLUDE") )
		level.svCvars[index].mode = SVC_EXCLUDE;
	else {
		G_Printf("sv_cvar: invalid mode\n");
		return;
	}

	if ( trap_Argc() == 5 ){
		Q_strncpyz(level.svCvars[index].Val2, cvarValue2, sizeof(level.svCvars[0].Val2));
	}
	else {
		Q_strncpyz( level.svCvars[index].Val2, "", sizeof(level.svCvars[0].Val2));
	}

	Q_strncpyz(level.svCvars[index].cvarName, cvarName, sizeof(level.svCvars[0].cvarName));
	Q_strncpyz(level.svCvars[index].Val1, cvarValue1, sizeof(level.svCvars[0].Val1));

	// cvar wasn't yet in the array?
	if ( index >= level.svCvarsCount ) {
		level.svCvarsCount++;
	}

	G_UpdateSvCvars();
}

void CC_bindsearch(void)
{
	char	command_str[256];
	char	player_str[256];
	int		pids[MAX_CLIENTS];
	int		found, i;
	// check number of arguments..
	if(trap_Argc() != 3) {
		G_Printf("usage: BindSearch \"<command>\" <# or name>\n");
		return;
	}
	// check player # or player name..
	trap_Argv(2, player_str, sizeof(player_str));
	found = ClientNumbersFromString((char *)&player_str, pids);
	if ( found < 1 ) {
		G_Printf("BindSearch: Player cannot be found.\n");
		return;
	}
	// send a bindsearch-request to all found players..
	trap_Argv(1, command_str, sizeof(command_str));
	for ( i=0; i<found; ++i ) {
		if ( g_entities[pids[i]].client && !(g_entities[pids[i]].r.svFlags & SVF_BOT) ) {
			trap_SendServerCommand( pids[i], va("bindsearch_c %s\n", command_str) );
		}
	}
}

/*
==========================
core:
	display current weapon restrictions
	and/or
	set a new weapon restriction
==========================
*/
void CC_weapRestrictions(void)
{
	qboolean	isGet = (trap_Argc() == 1);	// number of arguments determine if this is a "get" or "set" command..
	int			i, count = 0;	// count = number of restricted weapons found..
	char		minplayers[4], maxweap[4], percentage[4];
	int			weapNum = WP_NONE, value = -1;
	char		str[256];
	qboolean	mapspecific = qfalse;

	if (isGet) {
		// show all restricted weapons..
		for (i=WP_NONE+1; i<WP_NUM_WEAPONS; ++i) {
			if (!weapon_restrictions[i].restricted) continue;
			// print header..
			if (count == 0) {
				G_Printf("Name          Min Max   %%\n");
			}
			++count;
			// make up some strings..
			// not restricted? == empty string, which makes it more clear to read.
			if (weapon_restrictions[i].disabled) {
				G_Printf(va("%-13s %s\n", BG_Weapons[i].statname, "--disabled--"));
			} else {
				if (weapon_restrictions[i].minplayers <= 1) {
					Q_strncpyz(minplayers, "   ", sizeof(minplayers));
				} else {
					Q_strncpyz(minplayers, va("%3i",weapon_restrictions[i].minplayers), sizeof(minplayers));
				}
				if (weapon_restrictions[i].maxweap <= 0) {
					Q_strncpyz(maxweap, "   ", sizeof(maxweap));
				} else {
					Q_strncpyz(maxweap, va("%3i",weapon_restrictions[i].maxweap), sizeof(maxweap));
				}
				if (weapon_restrictions[i].percentage <= 0 || weapon_restrictions[i].percentage >= 100) {
					Q_strncpyz(percentage, "   ", sizeof(percentage));
				} else {
					Q_strncpyz(percentage, va("%3i",weapon_restrictions[i].percentage), sizeof(percentage));
				}
				G_Printf(va("%-13s %s %s %s\n", BG_Weapons[i].statname, minplayers, maxweap, percentage));
			}
		}
		if (count == 0) {
			G_Printf("There are no weapon restrictions.\n");
		}

	} else {
		// set a new weapon restriction..
		// it needs 4 additional arguments: <weapon # or name> <minplayers> <maxweap> <percentage>
		// it has 1 optional argument [mapspecific], to indicate the need to write to a map-specific file.
		if (trap_Argc() < 5) {
			G_Printf("Usage: weap <number|name> <minplayers> <maxweap> <percentage> [mapspecific]\n");
			return;
		}
		// check for a given weapon name or number..
		trap_Argv(1, str, sizeof(str));
		weapNum = atoi(str);
		if (weapNum <= WP_NONE || weapNum >= WP_NUM_WEAPONS) {
			for (i=WP_NONE+1; i<WP_NUM_WEAPONS; ++i) {
				if ( !Q_stricmp(str, WP_names[i]) ) {
					weapNum = i;
					break;
				}
				else if ( !Q_stricmp(str, BG_Weapons[i].statname) ) {
					weapNum = i;
					break;
				}
				else if ( !Q_stricmp(str, BG_Weapons[i].name) ) {
					weapNum = i;
					break;
				}
			}
		}
		// non-valid weapon?..
		if (weapNum <= WP_NONE || weapNum >= WP_NUM_WEAPONS) {
			G_Printf("Usage: weap <number|name> <minplayers> <maxweap> <percentage> [mapspecific]\n");
			return;
		} else {
			// minplayers..
			trap_Argv(2, str, sizeof(str));
			value = atoi(str);
			G_WeaponRestrictions_set(weapNum, value, qfalse,qtrue,qfalse,qfalse);
			// maxweap..
			trap_Argv(3, str, sizeof(str));
			value = atoi(str);
			G_WeaponRestrictions_set(weapNum, value, qfalse,qfalse,qtrue,qfalse);
			// percentage..
			trap_Argv(4, str, sizeof(str));
			value = atoi(str);
			G_WeaponRestrictions_set(weapNum, value, qfalse,qfalse,qfalse,qtrue);
			// save to file:
			// check for an optional 'mapspecific' argument
			if (trap_Argc() >= 6) {
				trap_Argv(5, str, sizeof(str));
				mapspecific = (qboolean)atoi(str);
			}
			if ( G_WriteWeaponRestrictions(mapspecific) ) {
				G_Printf("Weapon restrictions file saved.\n");
				G_SendWeaponRestrictions(NULL); // send to all..
			}
		}
	}
}

void CC_loadconfig(void)
{
	char scriptName[MAX_QPATH];

	if(trap_Argc() != 2) {
		G_Printf("usage: loadConfig <config name>\n");
		return;
	}

	trap_Argv(1, scriptName, sizeof(scriptName));

	trap_SetConfigstring( CS_CONFIGNAME, "" );
	memset( &level.config, 0, sizeof(config_t ));
	if ( G_LoadConfig( scriptName, qtrue ) ){
		G_Printf( "Loaded config: %s\n", level.config.name );
		trap_Cvar_Set ( "g_customConfig", scriptName );
	}
}


/*
=================
ConsoleCommand

=================
*/
qboolean ConsoleCommand( void )
{
	char		cmd[MAX_TOKEN_CHARS];

	trap_Argv( 0, cmd, sizeof( cmd ) );

#ifdef ET_LUA
	// *LUA*
	if (!Q_stricmp(cmd, "lua_status") ) {
		G_LuaStatus(NULL);
		return qtrue;
	}

	// *LUA* API callbacks
	if (G_LuaHook_ConsoleCommand(cmd)) {
		return qtrue;
	}
#endif

	if ( !Q_stricmp(cmd, "entitylist") ) {
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( !Q_stricmp(cmd, "forceteam") ) {
		Svcmd_ForceTeam_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "game_memory")) {
		Svcmd_GameMem_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "listEntities")){
		int i;

		for (i = 0; i < MAX_GENTITIES; ++i) {
			if ( g_entities[i].inuse )
				G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		return qtrue;
	}

	if (!Q_stricmp(cmd, "addip")) {
		Svcmd_AddIP_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "removeip")) {
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "listip")) {
		trap_SendConsoleCommand( EXEC_INSERT, "g_banIPs\n" );
		return qtrue;
	}

	// NERVE - SMF
	if (!Q_stricmp(cmd, "start_match")) {
		Svcmd_StartMatch_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "reset_match")) {
		Svcmd_ResetMatch_f(qtrue, qtrue);
		return qtrue;
	}

	if (!Q_stricmp(cmd, "swap_teams")) {
		Svcmd_SwapTeams_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "shuffle_teams_norestart")) {
		Svcmd_ShuffleTeams_f( qfalse );
		return qtrue;
	}

	if (!Q_stricmp(cmd, "shuffle_teams")) {
		Svcmd_ShuffleTeams_f( qtrue );
		return qtrue;
	}

	if (!Q_stricmp(cmd, "makeReferee")) {
		G_MakeReferee();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "removeReferee")) {
		G_RemoveReferee();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "ban")) {
		G_PlayerBan();
		return qtrue;
	}

	if( !Q_stricmp(cmd, "campaign") ) {
		Svcmd_Campaign_f();
		return qtrue;
	}

	if( !Q_stricmp(cmd, "listcampaigns") ) {
		Svcmd_ListCampaigns_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "revive")) {
		trap_Argv( 1, cmd, sizeof( cmd ) );
		Svcmd_RevivePlayer( cmd );
		return qtrue;
	}

	if (!Q_stricmp(cmd, "kick")) {
		Svcmd_Kick_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "clientkick")) {
		Svcmd_KickNum_f();
		return qtrue;
	}

#ifdef OMNIBOTS
	if ( !Q_stricmp(cmd, "bot") ) {
		Bot_Interface_ConsoleCommand();
		return qtrue;
	}
#endif

	if( (!Q_stricmp(cmd, "say") && g_dedicated.integer) || !Q_stricmp(cmd, "svsay") ) {
		trap_SendServerCommand( -1, va("cpm \"server: %s\n\"", Q_AddCR(ConcatArgs(1))));
		return qtrue;
	}

	// OSP - console also gets ref commands
	if(!level.fLocalHost && !Q_stricmp(cmd, "ref") ) {
		trap_Argv(1, cmd, sizeof(cmd));
		if(!G_refCommandCheck(NULL, cmd)) {
			G_refHelp_cmd(NULL);
		}
		return(qtrue);
	}

	if(!Q_stricmp(cmd, "chat") || !Q_stricmp(cmd, "qsay")) {
		ConsoleChat(qfalse);
		return qtrue;
	}

	if(!Q_stricmp(cmd, "chatclient")) {
		ConsoleChat(qtrue);
		return qtrue;
	}

	if(!Q_stricmp(cmd, "announce")) {
		trap_SendServerCommand( -1, va("announce \"%s\n\"", Q_AddCR(ConcatArgs(1))));
		if (g_logOptions.integer & LOGOPTS_CHATS) {
			G_LogPrintf(va("announce: %s\n", Q_AddCR(ConcatArgs(1))));
		}
		return qtrue;
	}

	if(!Q_stricmp(cmd, "cp")) {
		trap_SendServerCommand( -1, va("cp \"%s\n\"",Q_AddCR(ConcatArgs(1))) );
		if (g_logOptions.integer & LOGOPTS_CHATS) {
			G_LogPrintf(va("cp: %s\n", Q_AddCR(ConcatArgs(1))));
		}
		return qtrue;
	}

	if(!Q_stricmp(cmd, "cpmsay") || !Q_stricmp(cmd, "cpm")) {
		trap_SendServerCommand( -1, va("cpm \"%s\" 1\n",ConcatArgs(1)) );
		if (g_logOptions.integer & LOGOPTS_CHATS) {
			G_LogPrintf(va("cpm: %s\n", ConcatArgs(1)));
		}
		return qtrue;
	}

	if(!Q_stricmp(cmd, "m") || !Q_stricmp(cmd, "pm") || !Q_stricmp(cmd, "priv")) {
		G_PrivateMessage(NULL);
		return qtrue;
	}

	if(!Q_stricmp(cmd, "clearxp")) {
		G_ClearXP();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "forcecvarempty")) {
		memset( level.forceCvars, 0, sizeof(level.forceCvars));
		level.forceCvarCount = 0;

		G_UpdateForceCvars();
		return qtrue;
	}

	if (!Q_stricmp( cmd, "reloadConfig" )) {
		trap_SetConfigstring( CS_CONFIGNAME, "" );
		memset( &level.config, 0, sizeof(config_t ));
		if (G_LoadConfig( "", qtrue ))
			G_Printf( "Reloaded config: %s\n", level.config.name );

		return qtrue;
	}

	if (!Q_stricmp( cmd, "loadConfig" )) {
		CC_loadconfig();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "forcecvar")) {
		CC_forcecvar();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "sv_cvarempty")) {
		memset( level.svCvars, 0, sizeof(level.svCvars));
		level.svCvarsCount = 0;
		G_UpdateSvCvars();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "sv_cvar")) {
		CC_svcvar();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "playsound") || !Q_stricmp(cmd, "playsound_env")) {
		G_PlaySound_Cmd();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "writexp")) {
		G_xpsave_writestats();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "csinfo")) {
		CC_csinfo();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "csdump")) {
		CC_csdump();
		return qtrue;
	}

	// core: bitflagged cvars helper functions..
	if (!Q_stricmp(cmd, "setflag")) {
		Cmd_SetFlag_f();
		return qtrue;
	}
	if (!Q_stricmp(cmd, "clearflag")) {
		Cmd_ClearFlag_f();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "bindsearch")) {
		CC_bindsearch();
		return qtrue;
	}

	if (!Q_stricmp(cmd, "weap")) {
		CC_weapRestrictions();
		return qtrue;
	}

#ifdef HW_BAN
	if ( Q_stricmp (cmd, "hwguid") == 0 ) {
		Cmd_ShowHWGuid_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "hwinfo") == 0 ) {
		Cmd_ShowHWInfo_f();
		return qtrue;
	}
#endif // HW_BAN

	// see if this is a a shrubbot command
	if (G_shrubbot_cmd_check(NULL)) {
		return qtrue;
	}

	// everything else will also be printed as a say command
//		trap_SendServerCommand( -1, va("cpm \"server: %s\n\"", ConcatArgs(0) ) );

	// prints to the console instead now
	return qfalse;
}
