#include "g_local.h"

void BotDebug(int clientNum);
void GetBotAutonomies(int clientNum, int *weapAutonomy, int *moveAutonomy);	
qboolean G_IsOnFireteam(int entityNum, fireteamData_t** teamNum);

// jaquboss - code cleanup
void Cmd_GiveDisguise_f(gentity_t *ent, int startArg);


//#define MULTILOC 1

void Censor_Force_Suicide( gentity_t *ent) { 
		if(ent->client->sess.sessionTeam == TEAM_SPECTATOR || 
			(ent->client->ps.pm_flags & PMF_LIMBO) || 
			ent->health <= 0 || level.match_pause != PAUSE_NONE) { 
			return; 
		} 
			ent->flags &= ~FL_GODMODE; 
			ent->client->ps.stats[STAT_HEALTH] = ent->health = -500; 
		
			// TTimo - if using /kill while at MG42 
		ent->client->ps.persistant[PERS_HWEAPON_USE] = 0; 
		player_die(ent, ent, ent, 
		(g_gamestate.integer == GS_PLAYING) ? 100000 : 135, 
		MOD_CENSORED); } 

// Josh: like shrub, pretty naive
qboolean G_CensorText(char *text, wordDictionary *dictionary)
{
	qboolean foundWord = qfalse, lastWordFound = qfalse;
	int word = 0;
	char *textPos = text;
	while (*textPos) {
		lastWordFound = qfalse;
		word = 0;
		while (word < dictionary->num_words && *textPos) {
			if(!Q_stricmpn(textPos, dictionary->words[word], strlen(dictionary->words[word]))) {
				foundWord = qtrue;
				if (word == dictionary->num_words - 1) {
					lastWordFound = qtrue;
				}
				{
					int i;
					for (i = 0; 
					    i < strlen(
					      dictionary->words[word]);
					    i++) {
						*textPos++ =  '*';
					}
				}
			}
			word++;
		}
		// If it's the last word, it will already be incremented
		if (!lastWordFound)
			textPos++;
	}
	return foundWord;
}

// Josh: Won't gib if already in limbo, same as shrub
void G_CensorPenalize(gentity_t *ent)
{
	if (!ent->client) return;
	// forty - #127 - Burn a potty mouth  
	if (g_censorPenalty.integer & CNSRPNLTY_POISON) {  
			if(ent->client->sess.sessionTeam != TEAM_SPECTATOR &&  
					!(ent->client->ps.pm_flags & PMF_LIMBO) &&  
					ent->health >= 0 &&   
					level.match_pause == PAUSE_NONE) 
				{  
					ent->client->pmext.poisonerEnt = ent->client->ps.clientNum;
					ent->client->ps.eFlags |= EF_POISONED; 
					ent->client->pmext.poisonAmnt = 1;
					AP(va("chat \"^7%s ^9poisoned his dirty mouth.^7\"", ent->client->pers.netname));  
				}  
	} 
	if (g_censorPenalty.integer & CNSRPNLTY_BURN) {  
			if(ent->client->sess.sessionTeam != TEAM_SPECTATOR &&  
					!(ent->client->ps.pm_flags & PMF_LIMBO) &&  
					ent->health >= 0 &&   
					level.match_pause == PAUSE_NONE) 
				{  
					G_BurnMeGood(ent, ent, NULL);  
					AP(va("chat \"^7%s ^9burned his tongue.^7\"", ent->client->pers.netname));  
				}  
	}  
	         // forty - #127 - censor xp penalty  
	if (g_censorPenalty.integer & CNSRPNLTY_XP) {  
			if(ent->client->sess.sessionTeam != TEAM_SPECTATOR) 
					{  
						CPx(ent - g_entities, va("chat \"^1Warning^9: Watch you mouth!^* You have Lost %d XP.\"",10 ));  
						G_LoseSkillPoints(ent, BG_ClassSkillForClass(ent->client->sess.playerType), 10);  
	                }  
	         }  

	if (g_censorPenalty.integer & CNSRPNLTY_TMPMUTE)
	{
		ent->client->sess.muted = qtrue;
		ent->client->sess.auto_mute_time = level.time;
    CPx(ent - g_entities, va("print \"^9You've been auto-muted for %d seconds for language.\"",g_censorMuteTime.integer ));
    AP(va("chat \"%s^9 has been auto-muted for %d seconds for language.",  
		ent->client->pers.netname, g_censorMuteTime.integer ));
	}
	if (g_censorPenalty.integer & CNSRPNLTY_KILL) { 
		Censor_Force_Suicide(ent); 
		return; 
	}
	if (g_censorPenalty.integer & CNSRPNLTY_NOGIB)
	{
		// KILL, don't GIB
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		ent->client->ps.persistant[PERS_HWEAPON_USE]	 = 0;	// TTimo - if at MG42
        G_Damage(ent, ent, ent, NULL, NULL, ent->health, 0, MOD_CENSORED);
		//player_die(ent, ent, ent , 135, MOD_CENSORED);
	}
}

/*
 * G_PlayDead
 */
void G_PlayDead(gentity_t *ent) 
{

	if(!ent->client)
		return;
	if(!g_playDead.integer)
		return;
	if(ent->health < 0)
		return;
	if(ent->client->ps.eFlags & EF_POISONED)
		return;
	// tjw: can't playdead with a ticking grenade or a panzer
	//      that is spinning up.
	if(ent->client->ps.weaponDelay) 
		return;

	if(ent->client->ps.eFlags & EF_PLAYDEAD) {

		ent->client->ps.stats[STAT_HEALTH] = ent->health;
		ent->client->ps.pm_type = PM_PLAYDEAD;
		CP("cp \"SURPRISE!\" 1");
	}
	else {
		ent->client->ps.pm_type = PM_PLAYDEAD;
	}
}

void G_PrivateMessage(gentity_t *ent) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH];
	char netname[MAX_NAME_LENGTH];
	char cmd[12];
	char *msg;
	int pcount;
	int i;
	int skipargs = 0;
	gentity_t *tmpent;
	qboolean sent = qfalse;
	qboolean self = qfalse;
	char userinfo[MAX_INFO_STRING];

	if ( ent )
		trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ));

	if( ent && ( ent->client->sess.muted || G_shrubbot_mute_check( userinfo )) ) {
		SP(va("^9You are muted\n"));
		return;
	}

	if(!g_privateMessages.integer && ent) return;
	Q_SayArgv(0, cmd, sizeof(cmd));
	if(!Q_stricmp(cmd, "say")) {
		skipargs = 1;
		Q_SayArgv(1, cmd, sizeof(cmd));
	}
	if(Q_SayArgc() < 3+skipargs) {
		SP(va("^9usage: ^7%s [^2name^7|^2slot#^7] [^2message^7]\n", cmd));
		return;
	}
	Q_SayArgv(1+skipargs, name, sizeof(name));
	msg = Q_SayConcatArgs(2+skipargs);
	pcount = ClientNumbersFromString(name, pids);

	if(ent)
		Q_strncpyz(netname, 
			ent->client->pers.netname,
			sizeof(name));
	else
		Q_strncpyz(netname, "console", sizeof(name));

	for(i=0; i<pcount; i++) {
		// tjw: allow private message to self for etadmin_mod compat
		if(pids[i] == ent-g_entities) self=qtrue;
		tmpent = &g_entities[pids[i]];
		sent = qtrue;
		if(COM_BitCheck(
			tmpent->client->sess.ignoreClients,
			(ent-g_entities))) {

			SP(va("%s^1 is ignoring you\n",	tmpent->client->pers.netname));
			continue;
		}
		CPx(pids[i], va(
			"chat \"%s^7 -> %s^7: (%d recipients^7): ^3%s^7\"",
			netname,
			name,
			pcount,
			msg));
		CPx(pids[i], va("cp \"^3private message from ^7%s^7\"",
			netname));
		/*CPx(ent-g_entities, va("chat \"private message sent to %s: ^3%s^7\"",
			tmpent->client->pers.netname,
			msg
			));*/
	}
	// make a chat like feel
	if ( sent && ent && !self )
		CPx(ent-g_entities, va("chat \"%s^7 -> %s^7: (%d recipients^7): ^3%s^7\"", netname, name, pcount, msg));

	if(!sent) {
		CPx(ent-g_entities, "chat \"player not found\""); 
	}
	else if(g_tyranny.integer && g_logOptions.integer & LOGOPTS_PRIVMSG) 
	{
		// jet Pilot - changed from privmsg to make an easier check in G_LogPrintf
		G_LogPrintf( "saypriv: %s: %s: %s\n", netname, name, msg );
	}
}

void G_PlaySound_Cmd(void) {
	char sound[MAX_QPATH], name[MAX_NAME_LENGTH];
	int pids[MAX_CLIENTS];
	char err[MAX_STRING_CHARS];
	int index;
	gentity_t *victim;
	gentity_t *tent;

	if(trap_Argc() < 2) {
		G_Printf("usage: playsound [name|slot#] sound\n");
		return;
	}

	if(trap_Argc() > 2) {
		trap_Argv(1, name, sizeof(name));
		trap_Argv(2, sound, sizeof(sound));
	}
	else {
		if(!g_tyranny.integer) {
			G_Printf("playsound: g_tyranny must be enabled to play "
				"sounds to specific clients\n");
			return;
		}
		trap_Argv(1, sound, sizeof(sound));
		name[0] = '\0';
	}

	index = G_SoundIndex(sound);

	if(name[0]) {
		if(ClientNumbersFromString(name, pids) != 1) {
			G_MatchOnePlayer(pids, err, sizeof(err));
			G_Printf("playsound: %s\n", err);
			return;
		}
		victim = &level.gentities[pids[0]];
		tent = G_TempEntity(victim->r.currentOrigin, 
		       EV_GLOBAL_CLIENT_SOUND);
		tent->s.teamNum = pids[0];
		tent->s.eventParm = index;
	}
	else {
		G_globalSound(sound);
	}

}

void G_TeamDamageStats(gentity_t *ent)
{
	float teamHitPct;
	if (!ent->client) return;

	teamHitPct =
		ent->client->sess.hits ?
		(ent->client->sess.team_hits / ent->client->sess.hits)*(100):
		0;

	CPx(ent-g_entities, 
		va("print \"Team Hits: %.2f Total Hits: %.2f "
			"Pct: %.2f Limit: %d\n\"",
		ent->client->sess.team_hits,
		ent->client->sess.hits,
		teamHitPct,
		g_teamDamageRestriction.integer
		));
	return;
}


// G_SendScore_Add
// 
// Add score with clientNum at index i of level.sortedClients[]
// to the string buf.
// 
// returns qtrue if the score was appended to buf, qfalse otherwise.
qboolean G_SendScore_Add(gentity_t *ent, int i, char *buf, int bufsize) 
{
	gclient_t *cl;
	int ping, /*playerClass,*/ respawnsLeft;
	char entry[128];
	int totalXP = 0;
	int j;
	int	ready=-1;

	entry[0] = '\0';

	cl = &level.clients[level.sortedClients[i]];

//	playerClass = 0;
	// tjw: spectators should be able to see the player's class,
	//      but apperantly the client won't draw them even if we
	//      send it.  *clientmod*
/*	if(
#ifdef MV_SUPPORT
	G_smvLocateEntityInMVList(ent, level.sortedClients[i], qfalse) ||
#endif
		cl->sess.sessionTeam == ent->client->sess.sessionTeam ||
		ent->client->sess.sessionTeam == TEAM_SPECTATOR) { 

		playerClass = cl->sess.playerType;
	}*/

	// NERVE - SMF - number of respawns left
	respawnsLeft = cl->ps.persistant[PERS_RESPAWNS_LEFT];
	if(g_gametype.integer == GT_WOLF_LMS) {
		if(g_entities[level.sortedClients[i]].health <= 0) {
			respawnsLeft = -2;
		}
	}
	else if(respawnsLeft == 0 && 
		((cl->ps.pm_flags & PMF_LIMBO) || 
		  (level.intermissiontime && 
		  g_entities[level.sortedClients[i]].health <= 0))) {

		respawnsLeft = -2;
	}

	if ( cl->pers.connected == CON_CONNECTING ) {
		ping = -1;
	}
	else {
		//unlagged - true ping
		//ping = cl->ps.ping < 999 
		//	? cl->ps.ping : 999;
		ping = cl->pers.realPing < 999 ? cl->pers.realPing : 999;
		//unlagged - true ping
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		totalXP = cl->ps.persistant[PERS_SCORE];
	}
	else {
		for(j = 0; j < SK_NUM_SKILLS; j++) {
			totalXP += cl->sess.skillpoints[j];
		}
	}

	if ( cl->ps.eFlags & EF_READY ) {
		ready = 1;
	}

	Com_sprintf(entry,
		sizeof(entry),
		" %i %i %i %i %i %i %i",
		level.sortedClients[i],
		totalXP,
		ping, 
		(level.time - cl->pers.enterTime) / 60000,
		g_entities[level.sortedClients[i]].s.powerups,
		ready,
		//playerClass,
		respawnsLeft
		);

	if((strlen(buf) + strlen(entry) + 1) > bufsize) {
		return qfalse;
	}
	Q_strcat(buf, bufsize, entry);
	return qtrue;
}


/*
==================
G_SendScore

Sends current scoreboard information
==================
*/
void G_SendScore( gentity_t *ent ) {
	int i;
	int numSorted;
	int count;
	// tjw: commands over 1022 will crash the client so they're
	//      pruned in trap_SendServerCommand()
	//      1022 -32 for the startbuffer -3 for the clientNum 
	char		buffer[987];
	char		startbuffer[32];

	// send the latest information on all clients
	numSorted = level.numConnectedClients;

	i = 0;
	count = 0;
	*buffer = '\0';
	*startbuffer = '\0';
	Q_strncpyz(startbuffer, va(
		"sc0 %i %i",
		level.teamScores[TEAM_AXIS],
		level.teamScores[TEAM_ALLIES]),
		sizeof(startbuffer));

	// tjw: keep adding scores to the sc0 command until we fill 
	//      up the buffer.  Any scores that are left will be 
	//      added on to the sc1 command.
	for(; i < numSorted; i++) {
		if(g_entities[level.sortedClients[i]].r.svFlags & SVF_POW) {
			continue;
		}
		// tjw: the old version of SendScore() did this.  I removed it
		//      originally because it seemed like an unneccessary hack.
		//      perhaps it is necessary for compat with CG_Argv()?
		if(count == 33) {
			break;
		}
		if(!G_SendScore_Add(ent, i, buffer, sizeof(buffer))) {
			break;
		}
		count++;
	}
	trap_SendServerCommand(ent-g_entities, va(
		"%s %i%s", startbuffer, count, buffer));

	if(i == numSorted)
		return;
	
	count = 0;
	*buffer = '\0';
	*startbuffer = '\0';
	Q_strncpyz(startbuffer, "sc1", sizeof(startbuffer));
	for(; i < numSorted; i++) {
		if(g_entities[level.sortedClients[i]].r.svFlags & SVF_POW) {
			continue;
		}
		if(!G_SendScore_Add(ent, i, buffer, sizeof(buffer))) {
			G_Printf("ERROR: G_SendScore() buffer overflow\n");
			break;
		}
		count++;
	}
	if(!count) {
		return;
	}
	trap_SendServerCommand(ent-g_entities, va(
		"%s %i%s", startbuffer, count, buffer));

}

/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) {
	ent->client->wantsscore = qtrue;
//	G_SendScore( ent );
}

/*
==================
CheatsOkGeneral
==================
*/
qboolean	CheatsOkGeneral( void ) {
#ifdef DEBUG
	return qtrue; // jaquboss, allow cheats in debug
#endif

	if ( !g_cheats.integer ) {
		return qfalse;
	}

	return qtrue;
}

/*
==================
CheatsOk
==================
*/
qboolean	CheatsOk( gentity_t *ent ) {
#ifdef DEBUG
	return qtrue; // jaquboss, allow cheats in debug
#endif

	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return qfalse;
	}
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You must be alive to use this command.\n\""));
		return qfalse;
	}
	return qtrue;
}

// josh: tjw moved the declaration to q_shared.h, q_shared.c needs this
// so I moved it to q_shared.c
///*
//==================
//ConcatArgs
//==================
//*/
//char	*ConcatArgs( int start ) {
//	int		i, c, tlen;
//	static char	line[MAX_STRING_CHARS];
//	int		len;
//	char	arg[MAX_STRING_CHARS];
//
//	len = 0;
//	c = trap_Argc();
//	for ( i = start ; i < c ; i++ ) {
//		trap_Argv( i, arg, sizeof( arg ) );
//		tlen = strlen( arg );
//		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
//			break;
//		}
//		memcpy( line + len, arg, tlen );
//		len += tlen;
//		if ( i != c - 1 ) {
//			line[len] = ' ';
//			len++;
//		}
//	}
//
//	line[len] = 0;
//
//	return line;
//}

/*
==================
DecolorString

Remove color characters
==================
*/
void DecolorString( char *in, char *out)
{
	while(*in) {
		if(*in == 27 || *in == '^') {
			in++;		// skip color code
			if(*in) in++;
			continue;
		}
		*out++ = *in++;
	}
	*out = 0;
}

/*
==================

SanitizeString

Remove case and control characters
==================
*/
void SanitizeString( char *in, char *out, qboolean fToLower )
{
	while(*in) {
		if(*in == 27 || *in == '^') {
			in++;		// skip color code
			if(*in) in++;
			continue;
		}

		if(*in < 32) {
			in++;
			continue;
		}

		*out++ = (fToLower) ? tolower(*in++) : *in++;
	}

	*out = 0;
}

qboolean G_MatchOnePlayer(int *plist, char *err, int len) 
{
	gclient_t *cl;
	int *p;
	char line[MAX_NAME_LENGTH+10];

	err[0] = '\0';
	line[0] = '\0';
	if(plist[0] == -1) {
		Q_strcat(err, len, 
			"no connected player by that name or slot #");
		return qfalse;
	}
	if(plist[1] != -1) {
		Q_strcat(err, len, "more than one player name matches. "
			"be more specific or use the slot #:\n");
		for(p = plist;*p != -1; p++) {
			cl = &level.clients[*p];
			if(cl->pers.connected == CON_CONNECTED) {
				sprintf(line, "%2i - %s^7\n",
					*p,	
					cl->pers.netname);
				if(strlen(err)+strlen(line) > len)
					break;
				Q_strcat(err, len, line);
			}
		}
		return qfalse;
	}
	return qtrue;
}

/*
==================
ClientNumbersFromString

Sets plist to an array of integers that represent client numbers that have 
names that are a partial match for s. List is terminated by a -1.

Returns number of matching clientids.
==================
*/
int ClientNumbersFromString( char *s, int *plist) {
	gclient_t *p;
	int i, found = 0;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];
	char *m;
	qboolean is_slot = qtrue;

	*plist = -1;

	// if a number is provided, it might be a slot # 
	for(i=0; i<(int)strlen(s); i++) { 
		if(s[i] < '0' || s[i] > '9') {
			is_slot = qfalse;
			break;
		}
	}
	if(is_slot) {
		i = atoi(s);
		if(i >= 0 && i < level.maxclients) {
			p = &level.clients[i];
			if(p->pers.connected == CON_CONNECTED ||
				p->pers.connected == CON_CONNECTING) {

				*plist++ = i;
				*plist = -1;
				return 1;
			}
		}
	}

	// now look for name matches
	SanitizeString(s, s2, qtrue);
	if(strlen(s2) < 1) return 0;
	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED &&
			p->pers.connected != CON_CONNECTING) {
			
			continue;
		}
		SanitizeString(p->pers.netname, n2, qtrue);
		m = strstr(n2, s2);
		if(m != NULL) {
			*plist++ = i;
			found++;
		}
	}
	*plist = -1;
	return found;
}
	
/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t	*cl;
	int			idnum;
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];
	qboolean	fIsNumber = qtrue;

	// See if its a number or string
	// CHRUKER: b068 - Added the (int) type casting
	for(idnum=0; idnum<(int)strlen(s) && s[idnum] != 0; idnum++) {
		if(s[idnum] < '0' || s[idnum] > '9') {
			fIsNumber = qfalse;
			break;
		}
	}

	// check for a name match
	SanitizeString(s, s2, qtrue);
	for(idnum=0, cl=level.clients; idnum<level.maxclients; idnum++, cl++) {
		if(cl->pers.connected != CON_CONNECTED) continue;

		SanitizeString(cl->pers.netname, n2, qtrue);
		if(!strcmp( n2, s2)) return(idnum);
	}

	// numeric values are just slot numbers
	if(fIsNumber) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			CPx( to-g_entities, va("print \"Bad client slot: [lof]%i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			CPx( to-g_entities, va("print \"Client[lof] %i [lon]is not active\n\"", idnum));
			return -1;
		}
		return(idnum);
	}

	CPx(to-g_entities, va("print \"User [lof]%s [lon]is not on the server\n\"", s));
	return(-1);
}

/*
=================
Cmd_ListBotGoals_f

=================
*/

void Cmd_ListBotGoals_f( gentity_t* ent ) {
	int i;
	team_t t;

	if( !CheatsOk( ent ) ) {
		return;
	}
	
	for( t = TEAM_AXIS; t <= TEAM_ALLIES; t++ ) {
		gentity_t* list = g_entities, *targ;

		G_Printf( "\n%s bot goals\n=====================\n", (t == TEAM_AXIS ? "Axis" : "Allies"));
		

		for( i = 0; i < level.num_entities; i++, list++ ) {
			if(!list->inuse) {
				continue;
			}

			if(!(list->aiInactive & (1 << t))) {
				G_Printf( "%s (%s)", (list->scriptName ? list->scriptName :( list->targetname ? list->targetname : "NONE" )), list->classname );
				if (list->target_ent) {
					targ = list->target_ent;
					G_Printf( " -> " );
					G_Printf( "%s (%s)", (targ->scriptName ? targ->scriptName :( targ->targetname ? targ->targetname : "NONE" )), targ->classname );
				}
				G_Printf( "\n" );
			}
		}
	}
}

/*
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~
+~+~+~+~ Cmd_Gesture_f ~+~+~+~+
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~
jet Pilot Cmd_Gesture_f
Plays any animation on command

*/
extern char *jP_animationNames[];
//extern animStringItem_t animEventTypesStr[47];

void Cmd_Gesture_f(gentity_t *ent)
{	
	int		animIndex = -1;
	char	*args;

	if( !CheatsOk( ent ) ) {
		return;
	}

	args = ConcatArgs(1);
	if (! *args)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"^9usage : ^2gesture ^7<^20-%d^7>\n\"", NUM_ANIM_EVENTTYPES - 1));
		return;
	}
	animIndex = atoi(args);

	if (animIndex < 0 || animIndex >= NUM_ANIM_EVENTTYPES)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"^9gesture : ^1ERROR:^7 Must be within the range of ^20 ^7and ^2%d\n\"", NUM_ANIM_EVENTTYPES));
		return;
	}

	trap_SendServerCommand( ent-g_entities, va("print \"^9gesture: ^7 Playing animation ^2%s\n\"", jP_animationNames[animIndex]));

	BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, animIndex, qtrue, qfalse );
}

void Cmd_Test_GetDurationString(gentity_t *ent)
{
	int  secs;
	char dur[128];
	char *tmp;

	tmp = ConcatArgs(1);

	if (*tmp) secs = atoi(tmp);
	else
	{
		trap_SendServerCommand( ent-g_entities, "print \"^dusage : ^9getduration ^7<^2seconds^7>\n\"");
		return;
	}

	jP_GetDurationString(secs, dur, 128);

	trap_SendServerCommand( ent-g_entities, va("print \"^dgetduration : %s\n\"", dur));
}

/*
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~
+~+~+~+~ Cmd_Shrug_f ~+~+~+~+
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~
jet Pilot Cmd_Shrug_f
Plays the shrug animation on command

TODO - prevent from using it during movement, as it screws with the hitboxes
*/
void Cmd_Shrug_f(gentity_t *ent)
{
/*	if( !CheatsOk( ent ) ) {
		return;
	}*/
	if ( !ent || ent->client->ps.pm_type == PM_INTERMISSION || ent->client->ps.pm_flags & PMF_TIME_LOCKPLAYER || ent->health <= 0 )
		return;

	ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
	ent->client->ps.pm_time =  BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );

}

void Cmd_Salute_f(gentity_t *ent)
{
/*	if( !CheatsOk( ent ) ) {
		return;
	}*/
	if ( !ent || ent->client->ps.pm_type == PM_INTERMISSION || ent->client->ps.pm_flags & PMF_TIME_LOCKPLAYER || ent->health <= 0 )
		return;

	ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
	ent->client->ps.pm_time =  BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_SALUTE, qtrue, qfalse );
}

// jet Pilot - Cmd_DropWeapon_f
// 
// drops a client's current weapon
//

// jaquboss 
extern char *nq_wpNames[];

void Cmd_DropWeapon_f(gentity_t *ent)
{
	weapon_t secondary=WP_NONE; // init this stuff
	weapon_t primary=WP_NONE; // init this stuff


//	TossClientItems(ent);
//	return;

	if ( !ent || !ent->client )
		return;

	if ( ent->health <= 0 ) // jaquboss - using ent->health instead
		return;

	if ( level.time - ent->client->dropWeaponTime < 500 ) // dont allow as often
		return;

	primary = G_GetPrimaryWeaponForClient(ent->client);
	secondary = G_GetSecondaryWeaponForClient( ent->client, primary ); // jaquboss modifed both because these are working, note that primary is used only as reference in secondary detection

	if ( ent->client->ps.weapon == secondary )
	{	// secondary weapon dropping
		if ( secondary != WP_NONE && secondary < WP_NUM_WEAPONS)
		{ 
			G_DropWeapon( ent, secondary ); 
			BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_DROPWEAPON, qfalse, qtrue ); 
		} 
	} 

	if ( ent->client->ps.weapon == primary )
	{	// primary weapon dropping 
		if ( primary != WP_NONE && primary < WP_NUM_WEAPONS)
		{ 
			G_DropWeapon( ent, primary );
			BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qtrue ); 
		}  
	}

	// jaquboss
	ent->client->dropWeaponTime = level.time;

}
// jaquboss 
void Cmd_DropClip_f(gentity_t *ent)
{
	weapon_t secondary=WP_NONE; // init this stuff
	weapon_t primary=WP_NONE; // init this stuff

	if ( !ent || !ent->client )
		return;

	if ( ent->health <= 0 ) // jaquboss - using ent->health instead
		return;

	if ( level.time - ent->client->dropWeaponTime < 500 ) // dont allow as often
		return;

	primary = G_GetPrimaryWeaponForClient(ent->client);
	secondary = G_GetSecondaryWeaponForClient( ent->client, primary ); // jaquboss modifed both because these are working, note that primary is used only as reference in secondary detection

	if ( ent->client->ps.weapon == secondary )
	{	// secondary weapon's clip dropping
		if ( secondary > WP_NONE && secondary < WP_NUM_WEAPONS)
		{ 

			if ( ent->client->ps.ammo[BG_FindAmmoForWeapon(secondary)] ){
				G_DropClip( ent, secondary );
				BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qtrue ); 
			}
		} 
	} 

	if ( ent->client->ps.weapon == primary )
	{	// primary weapon's clip dropping 
		if ( primary > WP_NONE && primary < WP_NUM_WEAPONS && primary != WP_FLAMETHROWER) // don't drop flamethrower ammo
		{ 

			if ( ent->client->ps.ammo[BG_FindAmmoForWeapon(primary)] ){
				G_DropClip( ent, primary );
				BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qtrue ); 
			} else if ( primary == WP_BAZOOKA || primary == WP_PANZERFAUST) { // bazooka/panzerfaust use only clip
				if ( ent->client->ps.ammoclip[BG_FindClipForWeapon(primary)] > 1){
					G_DropClip( ent, primary );
					BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qtrue ); 
				}
			}
		}  
	}

	// jaquboss
	ent->client->dropWeaponTime = level.time;

}

void Cmd_CurWeapons_f(gentity_t *ent)
{
	weapon_t primary, secondary=WP_NONE;

	primary = G_GetWeaponsForClient(ent->client, secondary);
}



void Cmd_SimGoomba_f(gentity_t *ent)
{

// jet Pilot - temp allow cheats during testing
	if ( !CheatsOk( ent ) ) {
		return;
	}

	BG_AdjustGoombas(&ent->client->ps, 1);
    G_AddSkillPoints(ent, SK_BATTLE_SENSE, 5.0f);	

//	ent->client->ps.persistant[PERS_KILLSPREE] += 1;
	jP_UpdateSpree(ent, NULL, qfalse, qfalse);
}

void Cmd_SimKill_f(gentity_t *ent)
{

// jet Pilot - temp allow cheats during testing
	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->client->sess.kills += 1;
	if (ent->client->ps.persistant[PERS_HWEAPON_USE]	|| 
		(ent->client->sess.playerType == PC_SOLDIER		&& 
		 ent->client->ps.weapon != WP_MP40				&& 
		 ent->client->ps.weapon != WP_THOMPSON			))
	{
		G_AddSkillPoints(ent, SK_HEAVY_WEAPONS, 3.0f);
	}
	else
	{
    	G_AddSkillPoints(ent, SK_LIGHT_WEAPONS, 3.0f);
	}

//	ent->client->ps.persistant[PERS_KILLSPREE] += 1;
	jP_UpdateSpree(ent, NULL, qfalse, qfalse);
}

void Cmd_SimulateEvent_f(gentity_t *ent)
{
	char* args;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	args = ConcatArgs(1);
	if ( !args || !*args)
	{
		SP("print \"Usage : SimulateEvent <event name>\n\"");
		return;
	}

	if (!Q_stricmp(args, "kill"))
	{
		Cmd_SimKill_f(ent);
	}
	else if (!Q_stricmp(args, "goomba"))
	{
		Cmd_SimGoomba_f(ent);
	}
}

void Targeting_Pointer_Think( gentity_t *self );
void Cmd_TargetPointer_f(gentity_t *ent, qboolean active)
{
	gentity_t	*las;
	int			oldtype;
	int			type = 0;
	
	//Get rid of you?
	if (ent->client->targetingPointer) 
	{
		  oldtype = ent->client->targetingPointer->s.eventParm;

		  G_FreeEntity( ent->client->targetingPointer );
		  ent->client->targetingPointer = NULL;

		  if (oldtype == type)
		  {
			  return;
		  }
	}

	if (!active) 
	{
		return;
	}

	las = G_Spawn();

	las->nextthink	= level.time + 10;
	las->think		= Targeting_Pointer_Think;
	las->r.ownerNum = ent->s.number;
	las->parent		= ent;
	las->s.eType	= ET_TARGETING_POINTER;

	//Lets tell it if flashlight or laser
	if (type == 2)	{
		las->s.eventParm = 2; //tells CG that it is a flashlight
		las->classname = "targeting_pointer";
	}
	else {
		las->s.eventParm = 1; //tells CG that it is a laser sight
		las->classname = "targeting_pointer";
	}

	ent->client->targetingPointer = las;
}

/*
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
+~+~+~+~ Cmd_CreateLoc_f +~+~+~+~
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
added by - jet Pilot 
Creates a target_location entity at the player's current location
*/
#ifdef USELOCATIONS
void Cmd_LoadLocations_f( void /*gentity_t *ent*/)// jaquboss - changed to void
{
//	char			*filename;			// filename to load
	fileHandle_t	f;					// handle of file on disk
	int				fLen;				// length of the file
	char			fBuffer[32768];		// buffer to read the file into
	char			message[64] = "\0";	// location description
	char			temp[64]	= "\0"; // temporary buffer	
	int				x			= 0;	// x-coord of the location
	int				y			= 0;	// y-coord of the location
	int				z			= 0;	// z-coord of the location
	int				count		= 1;	// current number of locations
	int				p			= 0;	// current location in the file buffer
	int				t			= 0;	// current location in the temp buffer	
	static int 		curTot		= 0;
//	int				j, k;
	char			cString[BIG_INFO_STRING];

	// open the location .dat file that matches the map's name
	fLen = trap_FS_FOpenFile(va("maps/%s_loc.dat", level.rawmapname), &f, FS_READ);

	if (fLen < 0)
	{
		Com_Printf("^dLoadLocations: ^3Warning: ^9No location data found for map ^2%s^9.\n", level.rawmapname);
		level.noloc = qtrue; // jaquboss
		trap_SetConfigstring( CS_LOCATIONS, "Unknown" );
		return;
	}

	level.noloc = qfalse; // jaquboss

	trap_FS_Read(&fBuffer, fLen, f);					// read the file into the buffer
	fBuffer[fLen] = '\0';								// make sure it's null-terminated
	trap_FS_FCloseFile( f );							// close the file, we're done with it
	
	Com_Printf("^dLoadLocations: ^9location data for map ^2%s ^9loaded\n", level.rawmapname);

	// initialize the first location 
	trap_SetConfigstring( CS_LOCATIONS, "Unknown" );
	//strcpy(cString, "Unknown");
	level.locationHead = NULL;

	// start parsing!
	while (p < fLen)
	{
		// check for the beginning of a comment
		if (fBuffer[p++] == '/')						
		{
			//check for single line comment
			if (fBuffer[p] == '/')						
			{
				while (p < fLen && (fBuffer[p] != '\n' && fBuffer[p] != '\r'))
				{
					p++;
				}
			}
			// check for multiline comment
			else if (fBuffer[p] =='*')	
			{
				while (p < fLen && (fBuffer[p] != '*' && fBuffer[p+1] != '/'))
				{
					p++;
				}
			}
		}

		// parse the next line
        while (p < fLen && (fBuffer[p] != '\n' || fBuffer[p] != '\r') )
		{
			// grab the x-coord
			while (p < fLen && fBuffer[p] != ' ')
			{
				temp[t++] = fBuffer[p++];
			}
			temp[t] = '\0';
			x = atoi(temp);
			t = 0;
            memset(&temp, 0, sizeof(temp));				
			if (p > fLen) 
			{
			//	Com_Printf("^1Buffer Overflow prevented(x), aborting LoadLoc2\n");
				return;
			}
			p++;

			// grab the y-coord
			while (p < fLen && fBuffer[p] != ' ')
			{
				temp[t++] = fBuffer[p++];
			}
			temp[t] = '\0';
			y = atoi(temp);
			t = 0;
			memset(&temp, 0, sizeof(temp));
			if (p > fLen) 
			{
			//	Com_Printf("^1Buffer Overflow prevented(y), aborting LoadLoc2\n");
				return;
			}
			p++;

			// grab the z-coord
			while (p < fLen && fBuffer[p] != ' ')
			{
				temp[t++] = fBuffer[p++];
			}
			temp[t] = '\0';
			z = atoi(temp);
			t = 0;
			memset(&temp, 0, sizeof(temp));
			if (p > fLen) 
			{
			//	Com_Printf("^1Buffer Overflow prevented(z), aborting LoadLoc2\n");
				return;
			}
			p++;

			// grab the description
			while (p < fLen && fBuffer[p] != '\n' && fBuffer[p] != '\r')
			{
				// ignore quotation marks
				if (fBuffer[p] != '\"')
					temp[t++] = fBuffer[p++];
				else
					p++;
			}
			temp[t] = '\0';
			//Com_Printf(va("%s ^7at ^1%d ^2%d ^3%d\n", temp, x, y, z));
			t = 0;

			// if @, then keep the previous location name, otherwise, update message
			if (Q_stricmp(temp, "@"))
			{
				strcpy(message, temp);
			}
			
			//memset(&temp, 0, sizeof(temp));
			if (p > fLen) 
			{
				//Com_Printf("^1Buffer Overflow prevented(desc), aborting LoadLoc2\n");
				return;
			}

			// made it here, load the locations into memory
		//	if (curTot >= MAX_LOCATIONS) 
		//	{
		//		Com_Printf("MAX_LOCATIONS reached, ignoring remaining locations\n");
		//		return;
		//	}
		//	else
			{
				gentity_t		*loc;
				vec3_t			origin;

				loc				= G_Spawn();	
				loc->classname	= "target_location";
				loc->message	= message;
				loc->s.eType	= ET_LOCATION_MARKER;
				origin[0]		= x;
				origin[1]		= y;
				origin[2]		= z;
				
				G_SetOrigin(loc, origin);									// set the location's origin
				trap_LinkEntity( loc );										// link the location entity
#if 1				
				trap_GetConfigstring( CS_LOCATIONS, cString, sizeof(cString));
				strcat(cString, va("\\%s", loc->message));
				trap_SetConfigstring( CS_LOCATIONS, cString);
#else
				if (count & 1)
				{
					trap_GetConfigstring( CS_LOCATIONS + curTot, cString, sizeof(cString));
					strcat(cString, va("\\%s", loc->message));
				}
				else
				{
					curTot++;
					strcpy(cString, loc->message);
				}

				trap_SetConfigstring( CS_LOCATIONS + curTot, cString );		// add to the locations config string
				//strcpy(locationName[count], loc->message);
#endif
   				loc->health			= count++;								// set the location's id
				loc->count2			= curTot;								// set the config string index
   				loc->nextTrain		= level.locationHead;					// set this ent's next target as the old head
   				level.locationHead	= loc;									// set this ent as the new list head
			}
		}		
	}
	// send the configstring!
	trap_SetConfigstring( CS_LOCATIONS, cString);
	Com_Printf("%s\n", cString);

	// let the game know we have locations!
	level.locationLinked = qtrue;	
}


void Cmd_SaveLocations_f(void/*gentity_t *ent*/) // jaquboss - changed to void
{
	fileHandle_t	f;
	char			*filename;
	int				fLen = 0;
	char			temp[512], *temp2 = NULL;
	int				i;
	gentity_t		*ent=NULL;// jaquboss - changed to void

	filename = ConcatArgs(1);
	if (!*filename)
	{
		fLen = trap_FS_FOpenFile(va("maps/%s_loc.dat", level.rawmapname), &f, FS_WRITE);		
	}
	else
	{
		// open the file for writing
		fLen = trap_FS_FOpenFile(va("maps/%s_loc.dat", filename), &f, FS_WRITE);
	}
    
	strcpy(temp, va("// Location data file : %s  - (jet Pilot)\n", va("maps/%s_loc.dat",level.rawmapname)));
	trap_FS_Write(temp, strlen(temp), f);

	for (ent = g_entities, i = 0; i < level.num_entities; ent++, i++)
   	{
   		if (ent->classname && !Q_stricmp(ent->classname, "target_location"))
		{
			strcpy(temp, va("%d %d %d \"%s\"\n",
				(int)ent->r.currentOrigin[0],
				(int)ent->r.currentOrigin[1],
				(int)ent->r.currentOrigin[2],
				!Q_stricmp(ent->message, temp2) ? "@" : ent->message));

			temp2 = ent->message;

			trap_FS_Write(temp, strlen(temp), f);			
		}
	}

	trap_FS_FCloseFile( f );
}

/*
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
+~+~+~+~ Cmd_CreateLoc_f +~+~+~+~
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
added by - jet Pilot 
Creates a target_location entity at the player's current location
*/
void target_location_linkup(gentity_t *ent);

// TODO - save generated locations to a file
void Cmd_CreateLoc_f(gentity_t *ent)
{
	fileHandle_t	f;
	char			*desc;			// location description
	char			*msg;			// console message to print 
//	int				color;			// color of entity
	static int		index = 1;		// location we are at
	static int		id2		= 0;
	gentity_t		*loc;			// entity to add
	static int		first = 1;	
	char			temp[128];
	static char		temp2[64] = "";

	desc = ConcatArgs(1);
	if (! *desc)
	{
		msg = "^9usage : ^7createloc ^2<description>\n";
		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
		return;
	}

	if (level.locationLinked)
	{
		msg = "^3WARNING: ^7target_location^9 list already finalized. Ignoring\n";
		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
		return;
	}

	if (first)
	{
		trap_SetConfigstring( CS_LOCATIONS, "Unknown" );
//		strcpy(locationName[0], "Unknown");
		level.locationHead = NULL;
		trap_FS_FOpenFile(va("maps/%s_loc.dat", level.rawmapname), &f, FS_WRITE);

		strcpy(temp, va("// Location data file : %s  - (jet Pilot)\n", va("maps/%s_loc.dat",level.rawmapname)));
		trap_FS_Write(temp, strlen(temp), f);			
		first = 0;	
	}
	else
	{
		trap_FS_FOpenFile(va("maps/%s_loc.dat", level.rawmapname), &f, FS_APPEND);	
	}

	if (index >= MAX_LOCATIONS)
	{
		msg = "^3WARNING: ^7target_location^9 limit reached. Ignoring\n";
		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
		trap_FS_FCloseFile(f);
		return;
	}
		
	trap_SendServerCommand( ent-g_entities, va("print \"^9adding ^7target_location^9 ^2(%s^2).\n\"", desc));	

	// spawn the location entity
	loc				= G_Spawn();

	loc->parent		= ent;
	loc->r.ownerNum	= ent->s.number;
	loc->classname	= "target_location";
	loc->message	= desc;

	VectorCopy(ent->r.currentOrigin, loc->r.currentOrigin);
	VectorCopy(ent->s.pos.trBase,	 loc->s.pos.trBase);

	trap_LinkEntity( loc );

	trap_SetConfigstring( CS_LOCATIONS + index, loc->message );
//	strcpy(locationName[index], loc->message);
   	loc->health			= index++;
	loc->count2			= 0;
   	loc->nextTrain		= level.locationHead;
   	level.locationHead	= loc;

	strcpy(temp, va("%d %d %d \"%s\"\n",
		(int)ent->r.currentOrigin[0],
		(int)ent->r.currentOrigin[1],
		(int)ent->r.currentOrigin[2],
		loc->message));

//	strcpy(temp2, ent->message);

	trap_FS_Write(temp, strlen(temp), f);
	trap_FS_FCloseFile(f);
}
#endif
/*
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
+~+~+~+~ Cmd_GiveDisguise_f +~+~+~+~
~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+~+
added by - jet Pilot 
CHEAT
Gives a player a disguise, which class the disguise belongs to is specified by a letter or number
*/

void Cmd_GiveDisguise_f(gentity_t *ent, int startArg)
{
	char		*cls;
	char		*msg = NULL;
	int			classNum = 0;	

	cls  = ConcatArgs(startArg+1);
	if (! *cls)
	{
		msg = "^9usage : ^7givedisguise ^2<class>\n";

		trap_SendServerCommand( ent-g_entities, va("print \"^9usage : ^7givedisguise ^2<class>\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \"^9        available choices are ^2s, m, e, f, ^9and ^2c^9, or the numbers ^20 ^9through ^24\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \"^2        -1 ^9removes any current disguise.\n\""));
		return;
	}

	classNum = atoi(cls);
	
	if ( classNum < -1 )
		classNum = -1;
	if ( classNum > 4 )
		classNum = 4;

	if (!Q_stricmpn(cls, "m", 1)  || classNum == 1)
	{
		msg = "^9givedisguise : ^7Medic\n";
		classNum = 1;
	}
	if (!Q_stricmpn(cls, "e", 1) || classNum == 2)
	{
		msg = "^9givedisguise : ^7Engineer\n";
		classNum = 2;
	}
	if (!Q_stricmpn(cls, "f", 1) || classNum == 3)
	{
		msg = "^9givedisguise : ^7Field Ops\n";
		classNum = 3;
	}
	if (!Q_stricmpn(cls, "c", 1) || classNum == 4)
	{
		msg = "^9givedisguise : ^7Covert Ops\n";
		classNum = 4;
	}
	if (!Q_stricmpn(cls, "s", 1) || classNum == 0)
	{
		msg = "^9givedisguise : ^7Soldier\n";
		classNum = 0;
	}	
	if (!Q_stricmpn(cls, "-1", 1) || classNum == -1) // this just mean that there must be '-' so '-x' does the shit
	{		
		trap_SendServerCommand( ent-g_entities, va("print \"^9givedisguise : ^7Removing disguise\n\""));
		ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
		return;
	}
	

	ent->client->ps.powerups[PW_OPS_DISGUISED] = 1;
	ent->client->ps.powerups[PW_OPS_CLASS_1] = classNum & 1;
	ent->client->ps.powerups[PW_OPS_CLASS_2] = classNum & 2;
	ent->client->ps.powerups[PW_OPS_CLASS_3] = classNum & 4;
	
// jaquboss give him some name and rank
	Q_strncpyz(ent->client->disguiseNetname,
		ent->client->pers.netname,
		sizeof(ent->client->disguiseNetname));

	ent->client->disguiseClient = ent->s.clientNum;
	ent->client->disguiseRank = ent->client->sess.rank;


	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}



/*
void Cmd__On_f(gentity_t *ent)
{
	trap_Cvar_Set( "jp_insanity", va("%d", jp_insanity.integer | JP_INSANITY_ ));

	trap_SendServerCommand( ent-g_entities, va("print \"^9insanity : ^7 ^2Unlocked\n\""));	
}

void Cmd__Off_f(gentity_t *ent)
{
	if (jp_insanity.integer | JP_INSANITY_) 
		trap_Cvar_Set( "jp_insanity", va("%d", jp_insanity.integer - JP_INSANITY_ ));
	trap_SendServerCommand( ent-g_entities, va("print \"^9insanity : ^7 ^1Locked\n\""));
}
*/


/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void ExecGive( gentity_t *ent, char *name, char *amt, char *amt2)
{
	gitem_t		*it;
	int			i;
	qboolean	give_all;
	int			skill;
	int			amount, amount2;
	qboolean	hasAmount = qfalse;
	qboolean	hasAmount2= qfalse;


	if( *amt )
		hasAmount = qtrue;
		amount = atoi(amt);

	if (*amt2)
		hasAmount2 = qtrue;
		amount2 = atoi(amt2);

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if( Q_stricmpn( name, "skill", 5 ) == 0 ) {
		if( hasAmount ) 
		{

			// CHRUKER: b064 - Changed amount to skill, so that we can use amount properly 
			skill = amount; 
			if( skill >= 0 && skill < SK_NUM_SKILLS )
			{ 
				// CHRUKER: b064 - Detecting the correct amount to move to the next skill level 
				amount = 20; 
				if ( ent->client->sess.skill[skill] < NUM_SKILL_LEVELS-1 ) 
				{
				//	amount = skillLevels[ent->client->sess.skill[skill] + 1] - ent->client->sess.skillpoints[skill]; 
				}
				
				G_AddSkillPoints( ent, skill, hasAmount2 ? amount2 : amount ); 
				G_DEBUG_ADD_SKILL_POINTS( ent, skill, amount, "give skill" ); // b064 
			}			
		} 
		else 
		{
			// bumps all skills with 1 level
			for( i = 0; i < SK_NUM_SKILLS; i++ ) 
			{
				G_AddSkillPoints( ent, i, 20 );
				G_DEBUG_ADD_SKILL_POINTS( ent, i, 20, "give skill" ); 
			}
		}
		return;
	}

	if ( Q_stricmpn( name, "medal", 5) == 0) {
        for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			if( !ent->client->sess.medals[i] )
				ent->client->sess.medals[i] = 1;
		}
		ClientUserinfoChanged( ent-g_entities );
		return;
	}

	if (give_all || Q_stricmpn( name, "health", 6) == 0)
	{
		//----(SA)	modified
		if(amount)
			ent->health += amount;
		else
			ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		if (!give_all)
			return;
	}


	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		for(i=0;i<WP_NUM_WEAPONS;i++) {
			if ( BG_WeaponInWolfMP(i) )
				COM_BitSet( ent->client->ps.weapons, i );
		}

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmpn(name, "ammo", 4) == 0)
	{
		if(amount) {
			if(ent->client->ps.weapon
				&& ent->client->ps.weapon != WP_SATCHEL && ent->client->ps.weapon != WP_SATCHEL_DET
				)
				Add_Ammo(ent, ent->client->ps.weapon, amount, qtrue);
		} else {
			for ( i = 1 ; i < WP_NUM_WEAPONS ; i++ ) {
				if( COM_BitCheck( ent->client->ps.weapons, i ) && i != WP_SATCHEL && i != WP_SATCHEL_DET)
					Add_Ammo(ent, i, 9999, qtrue);
			}
		}

		if (!give_all)
			return;
	}

	//	"give allammo <n>" allows you to give a specific amount of ammo to /all/ weapons while
	//	allowing "give ammo <n>" to only give to the selected weap.
	if (Q_stricmpn(name, "allammo", 7) == 0 && amount)
	{
		for ( i = 1 ; i < WP_NUM_WEAPONS; i++ )
			Add_Ammo(ent, i, amount, qtrue);

		if (!give_all)
			return;
	}

	//---- (SA) Wolf keys
	if (give_all || Q_stricmp(name, "keys") == 0)
	{
		ent->client->ps.stats[STAT_KEYS] = (1 << KEY_NUM_KEYS) - 2;
		if (!give_all)
			return;
	}
	//---- (SA) end


	// spawn a specific item right on the player
	if ( !give_all ) {
		gentity_t	*drop = NULL;

		it = BG_FindItem (name);

		if (!it) {

			it = BG_FindItemForClassName (name);

			if (!it)
			return;
		}

		drop = Drop_Item (ent, it, 0, qfalse);
	}

}
// jaquboss moved code above so it can be used for shrubbot as well
void Cmd_Give_f (gentity_t *ent)
{

// jet Pilot - temp allow cheats during testing
	if ( !CheatsOk( ent ) ) {
		return;
	}

	ExecGive( ent, ConcatArgs( 1 ), ConcatArgs(2), ConcatArgs(3));
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (gentity_t *ent)
{
	char	*msg;
	char	*name;
	qboolean godAll = qfalse;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	name = ConcatArgs( 1 );

	// are we supposed to make all our teammates gods too?
	if (Q_stricmp(name, "all") == 0)
		godAll = qtrue;

	// can only use this cheat in single player
	if (godAll && g_gametype.integer == GT_SINGLE_PLAYER)
	{
		int j;
		qboolean settingFlag = qtrue;
		gentity_t *other;

		// are we turning it on or off?
		if (ent->flags & FL_GODMODE)
			settingFlag = qfalse;

		// loop through all players
		for (j = 0; j < level.maxclients; j++)
		{
			other = &g_entities[j];
			// if they're on the same team
			if (OnSameTeam(other, ent))
			{
				// set or clear the flag
				if (settingFlag)
					other->flags |= FL_GODMODE;
				else
					other->flags &= ~FL_GODMODE;
			}
		}
		if (settingFlag)
			msg = "^9godmode all ^2ON\n";
		else
			msg = "^9godmode all ^2OFF\n";
	}
	else
	{
		if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
			ent->flags |= FL_GODMODE;
		} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
			ent->flags &= ~FL_GODMODE;
		} else {
			ent->flags ^= FL_GODMODE;
		}
		if (!(ent->flags & FL_GODMODE) )
			msg = "^9godmode ^2OFF\n";
		else
			msg = "^9godmode ^2ON\n";
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
==================
Cmd_Nofatigue_f

Sets client to nofatigue

argv(0) nofatigue
==================
*/

void Cmd_Nofatigue_f (gentity_t *ent)
{
	char	*msg;

	char	*name = ConcatArgs( 1 );

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
		ent->flags |= FL_NOFATIGUE;
	} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
		ent->flags &= ~FL_NOFATIGUE;
	} else {
		ent->flags ^= FL_NOFATIGUE;
	}

	if (!(ent->flags & FL_NOFATIGUE) )
		msg = "^9nofatigue ^2OFF\n";
	else
		msg = "^9nofatigue ^2ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent ) {
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
	char	*msg;

	char	*name = ConcatArgs( 1 );

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
		ent->client->noclip = qtrue;
	} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
		ent->client->noclip = qfalse;
	} else {
		ent->client->noclip = !ent->client->noclip;
	}

	if ( ent->client->noclip ) {
		msg = "^9noclip ^2ON\n";
	} else {
		msg = "^9noclip ^2OFF\n";
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
	gentity_t *attacker;

	if(ent->health <= 0) {
		SP("^9You must be alive to use ^3/kill.\n");
		return;
	}

	if( g_slashKill.integer & SLASHKILL_NOKILL ) {
		SP("^3/kill ^9is disabled on this server.\n");
		return;
	}
	if((g_slashKill.integer & SLASHKILL_NOPOISON) && 
		ent->client->ps.eFlags & EF_POISONED) {
		SP("^3/kill ^9is disabled while you are poisoned.\n");
		return;
	}
	if(ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
	  (ent->client->ps.pm_flags & PMF_LIMBO) ||
	  ent->health <= 0 || level.match_pause != PAUSE_NONE) {
		return;
	}
	ent->client->pers.slashKill = qtrue;

	// jet Pilot - /kill gives you a darwin award
	//ent->client->ps.persistant[PERS_DARWINDEATHS]++;

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
	// TTimo - if using /kill while at MG42
	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;

	attacker = &level.gentities[ent->client->lasthurt_client];

	// tjw: put an end to /kill binding stat whores.
	if(g_fear.integer && attacker && attacker->client &&
		(level.time - ent->client->lasthurt_time) < g_fear.integer &&
		attacker->client->sess.sessionTeam != ent->client->sess.sessionTeam &&
		attacker->health > 0) 
	{
		// tjw: 150 added to make sure the player is gibbed since
		//      that was the intent.
		player_die(ent, attacker, attacker, (ent->health + 150), MOD_FEAR);
	}
	else 
	{
		player_die(ent, ent, ent, (g_gamestate.integer == GS_PLAYING) ? 100000 : 135, MOD_SUICIDE);
	}
}



void BotRecordTeamChange( int client );

void G_TeamDataForString( const char* teamstr, int clientNum, team_t* team, spectatorState_t* sState, int* specClient ) {
	*sState = SPECTATOR_NOT;
	if( !Q_stricmp( teamstr, "follow1" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FOLLOW;
		if( specClient ) {
			*specClient = -1;
		}
	} else if( !Q_stricmp( teamstr, "follow2" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FOLLOW;
		if( specClient ) {
			*specClient = -2;
		}
	} else if( !Q_stricmp( teamstr, "spectator" ) || !Q_stricmp( teamstr, "s" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FREE;
	} else if ( !Q_stricmp( teamstr, "red" ) || !Q_stricmp( teamstr, "r" ) || !Q_stricmp( teamstr, "axis" ) ) {
		*team =		TEAM_AXIS;
	} else if ( !Q_stricmp( teamstr, "blue" ) || !Q_stricmp( teamstr, "b" ) || !Q_stricmp( teamstr, "allies" ) ) {
		*team =		TEAM_ALLIES;
	} else {
		*team = PickTeam( clientNum );
		if(!G_teamJoinCheck( *team, &g_entities[clientNum] )) {
			*team = ((TEAM_AXIS | TEAM_ALLIES) & ~*team);
		}
	}
}


void G_DropItems(gentity_t *self) 
{
	gitem_t *item= NULL;

	// drop flag regardless
	if (self->client->ps.powerups[PW_REDFLAG]) {
		item = BG_FindItem("Red Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	if (self->client->ps.powerups[PW_BLUEFLAG]) {
		item = BG_FindItem("Blue Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if (item) {
		vec3_t launchvel = { 0, 0, 0 };
		vec3_t forward;
		vec3_t origin;
		vec3_t angles;
		gentity_t *flag;

		VectorCopy(self->client->ps.origin, origin);
		// tjw: if the player hasn't died, then assume he's 
		//      throwing objective per g_dropObj
		if(self->health > 0) {
			VectorCopy(self->client->ps.viewangles, angles);
			if(angles[PITCH] > 0)
				angles[PITCH] = 0;
			AngleVectors(angles, forward, NULL, NULL);
			VectorMA(self->client->ps.velocity, 
				96, forward, launchvel);
			VectorMA(origin, 36, forward, origin);
			origin[2] += self->client->ps.viewheight;
		}

		flag = LaunchItem(item, origin, launchvel, self->s.number, FL_DROPPED_ITEM);

		flag->s.modelindex2 = self->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;	// DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}
}
/*
=================
SetTeam
=================
*/
qboolean SetTeam( gentity_t *ent, char *s, qboolean force, weapon_t w1, weapon_t w2, qboolean setweapons ) {
	team_t				team, oldTeam;
	gclient_t			*client;
	int					clientNum;
	spectatorState_t	specState;
	int					specClient;
	int					respawnsLeft;
	int i;
	gclient_t *cl;

	// if the team changing player is a shrubbot admin with the
	// '5' flag, they can switch teams regardless of balance
	if(G_shrubbot_permission(ent, SBF_FORCETEAMCHANGE)) 
		force = qtrue;

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;
	
	// ydnar: preserve respawn count
	respawnsLeft = client->ps.persistant[ PERS_RESPAWNS_LEFT ];
	
	G_TeamDataForString( s, client - level.clients, &team, &specState, &specClient );

	if( team != TEAM_SPECTATOR )
	{
		// Ensure the player can join
		if(!G_teamJoinCheck(team, ent)) 
		{
			// Leave them where they were before the command was issued
			return(qfalse);
		}

		if(g_noTeamSwitching.integer && (team != ent->client->sess.sessionTeam && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) && g_gamestate.integer == GS_PLAYING && !force ) {
			trap_SendServerCommand( clientNum, "cp \"You cannot switch during a match, please wait until the round ends.\n\"" );
			return qfalse;	// ignore the request
		}

		if ( ( (g_gametype.integer == GT_WOLF_LMS && g_lms_teamForceBalance.integer) || g_teamForceBalance.integer ) && !force ) {
			int		counts[TEAM_NUM_TEAMS];

			counts[TEAM_ALLIES] = TeamCount( ent-g_entities, TEAM_ALLIES );
			counts[TEAM_AXIS] = TeamCount( ent-g_entities, TEAM_AXIS );

			// We allow a spread of one
			if ( team == TEAM_AXIS && counts[TEAM_AXIS] - counts[TEAM_ALLIES] >= 1 ) {
				CP("cp \"The Axis has too many players.\n\"");
				return qfalse; // ignore the request
			}
			if ( team == TEAM_ALLIES && counts[TEAM_ALLIES] - counts[TEAM_AXIS] >= 1 ) {
				CP("cp \"The Allies have too many players.\n\"");
				return qfalse; // ignore the request
			}

			// It's ok, the team we are switching to has less or same number of players
		}
	}

	if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) {
		team = TEAM_SPECTATOR;
	}

	//
	// decide if we will allow the change
	//
	oldTeam = client->sess.sessionTeam;
	if ( team == oldTeam && team != TEAM_SPECTATOR ) 
	{
		return qfalse;
	}

	// NERVE - SMF - prevent players from switching to regain deployments
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( ( g_maxlives.integer > 0 || 
			( g_alliedmaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_ALLIES ) || 
			( g_axismaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_AXIS ) ) 
			
			&& ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 && oldTeam != TEAM_SPECTATOR ) {
			CP("cp \"You can't switch teams because you are out of lives.\n\" 3");
			return qfalse;	// ignore the request
		}
	}

	// DHM - Nerve :: Force players to wait 30 seconds before they can join a new team.
	// OSP - changed to 5 seconds
	// Gordon: disabling if in dev mode: cuz it sucks
	// Gordon: bleh, half of these variables don't mean what they used to, so this doesn't work
/*	if ( !g_cheats.integer ) {
		 if ( team != oldTeam && level.warmupTime == 0 && !client->pers.initialSpawn && ( (level.time - client->pers.connectTime) > 10000 ) && ( (level.time - client->pers.enterTime) < 5000 ) && !force ) {
			CP(va("cp \"^3You must wait %i seconds before joining ^3a new team.\n\" 3", (int)(5 - ((level.time - client->pers.enterTime)/1000))));
			return qfalse;
		}
	}*/
	// dhm
  
	//
	// execute the team change
	//


	// DHM - Nerve
	// OSP
	if(team != TEAM_SPECTATOR) {
		client->pers.initialSpawn = qfalse;
		// no MV in-game
#ifdef MV_SUPPORT
		if(client->pers.mvCount > 0) 
		{
			G_smvRemoveInvalidClients(ent, TEAM_AXIS);
			G_smvRemoveInvalidClients(ent, TEAM_ALLIES);
		}
#endif
	}

	// he starts at 'base'
	// RF, in single player, bots always use regular spawn points
	if (!((g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP) && (ent->r.svFlags & SVF_BOT))) {
		client->pers.teamState.state = TEAM_BEGIN;
	}
	

	if(oldTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_LIMBO) ) 
	{		
		ent->flags &= ~FL_GODMODE;
		// tjw: if they're not dead, don't kill
		if(g_teamChangeKills.integer || ent->health <= 0) {
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			player_die (ent, ent, ent, 100000, MOD_SWITCHTEAM);
		}
		else {
			ent->client->ps.stats[STAT_HEALTH] = ent->health;
			G_DropItems(ent);
		}
	}

	// they go to the end of the line for tournements
	if ( team == TEAM_SPECTATOR ) {
		client->sess.spectatorTime = level.time;
		if(!client->sess.referee) client->pers.invite = 0;
#ifdef MV_SUPPORT
		if(team != oldTeam) G_smvAllRemoveSingleClient(ent - g_entities);
#endif
	}

	G_LeaveTank( ent, qfalse );
	G_RemoveClientFromFireteams( clientNum, qtrue, qfalse );
	if( g_landminetimeout.integer ) {
		G_ExplodeMines( ent );
	}
	G_FadeItems(ent, MOD_SATCHEL);
	G_FadeItems(ent, MOD_AIRSTRIKE);  
	G_FadeItems(ent, MOD_ARTY);  


	// remove ourself from teamlists
	{
		int i;
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList;

		for( i = 0; i < 2; i++ ) {
			teamList = &mapEntityData[i];

			if((mEnt = G_FindMapEntityData(&mapEntityData[0], ent-g_entities)) != NULL) {
				G_FreeMapEntityData( teamList, mEnt );
			}

			mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, ent->s.number, -1 );
			
			while( mEnt ) {
				mapEntityData_t	*mEntFree = mEnt;

				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, ent->s.number, -1 );

				G_FreeMapEntityData( teamList, mEntFree );
			}
		}
	}

	// tjw: if someone was following, make them follow the next 
	//      player on the team.
	for(i=0; i<level.numConnectedClients; i++) 
	{
		cl = &level.clients[level.sortedClients[i]];
		if((cl->ps.pm_flags & PMF_LIMBO) &&	
			cl->sess.spectatorState == SPECTATOR_FOLLOW &&
			cl->sess.spectatorClient == (ent - g_entities)) 
		{			
			Cmd_FollowCycle_f(&g_entities[level.sortedClients[i]], 1);
		}
	}

	client->sess.spec_team		 = 0;
	client->sess.sessionTeam	 = team;
	client->sess.spectatorState  = specState;
	client->sess.spectatorClient = specClient;
	client->pers.ready			 = qfalse;

	// CHRUKER: b081 - During team switching you can sometime spawn immediately
	client->pers.lastReinforceTime = 0;

	// (l)users will spam spec messages... honest!
	if(team != oldTeam) 
	{
		gentity_t* tent = G_PopupMessage( PM_TEAM );
		tent->s.effect2Time = team;
		tent->s.effect3Time = clientNum;
		tent->s.density = 0;
	}

	if( setweapons ) 	
	{
		G_SetClientWeapons( ent, w1, w2, qfalse );
	}

	// get and distribute relevent paramters
	G_UpdateCharacter( client );			// FIXME : doesn't ClientBegin take care of this already?
	ClientUserinfoChanged( clientNum );

	ClientBegin( clientNum );
	
	// ydnar: restore old respawn count (players cannot jump from team to team to regain lives)
	if(respawnsLeft >= 0 && oldTeam != TEAM_SPECTATOR)
	{
		client->ps.persistant[ PERS_RESPAWNS_LEFT ] = respawnsLeft;
	}

	G_verifyMatchState(oldTeam);
	BotRecordTeamChange( clientNum );

	/*
	// Reset stats when changing teams
	if(team != oldTeam) {
		G_deleteStats(clientNum);
	}
	*/

	G_UpdateSpawnCounts();

	if( g_gamestate.integer == GS_PLAYING && ( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) ) {
		if(g_gametype.integer == GT_WOLF_LMS && level.numTeamClients[0] > 0 && level.numTeamClients[1] > 0) {
			trap_SendServerCommand( clientNum, "cp \"Will spawn next round, please wait.\n\"" );
			limbo(ent, qfalse);
			return(qfalse);
		} else {
			int i;
			int x = client->sess.sessionTeam - TEAM_AXIS;

			for( i = 0; i < MAX_COMMANDER_TEAM_SOUNDS; i++ ) {
				if( level.commanderSounds[ x ][ i ].index ) {
					gentity_t* tent = G_TempEntity( client->ps.origin, EV_GLOBAL_CLIENT_SOUND ); 
					tent->s.eventParm = level.commanderSounds[ x ][ i ].index - 1;
					tent->s.teamNum = clientNum;
				}
			}
		}
	}

	ent->client->pers.autofireteamCreateEndTime = 0;
	ent->client->pers.autofireteamJoinEndTime = 0;

	if( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) {
		if( g_autoFireteams.integer ) {
			fireteamData_t* ft = G_FindFreePublicFireteam( client->sess.sessionTeam );

			if( ft ) {
				trap_SendServerCommand( ent-g_entities, "aftj -1" );
				ent->client->pers.autofireteamJoinEndTime = level.time + 20500;

//				G_AddClientToFireteam( ent-g_entities, ft->joinOrder[0] );
			} else {
				trap_SendServerCommand( ent-g_entities, "aftc -1" );
				ent->client->pers.autofireteamCreateEndTime = level.time + 20500;
			}
		}
	}
	ent->client->pers.lastTeamChangeTime = level.time;
	return qtrue;
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent ) {
	// ATVI Wolfenstein Misc #474
	// divert behaviour if TEAM_SPECTATOR, moved the code from SpectatorThink to put back into free fly correctly
	// (I am not sure this can be called in non-TEAM_SPECTATOR situation, better be safe)
	if( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		// drop to free floating, somewhere above the current position (that's the client you were following)
		vec3_t pos, angle;
		gclient_t	*client = ent->client;
		VectorCopy(client->ps.origin, pos); 
//		pos[2] += 16; // Gordon: removing for now
		VectorCopy(client->ps.viewangles, angle);
		// Need this as it gets spec mode reset properly
		SetTeam( ent, "s", qtrue, -1, -1, qfalse );
		VectorCopy(pos, client->ps.origin);
		SetClientViewAngle(ent, angle);
		ent->client->sess.spectatorState = SPECTATOR_FREE;
	} else {
		// legacy code, FIXME: useless?
		// Gordon: no this is for limbo i'd guess
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		ent->client->ps.clientNum = ent - g_entities;
	}
}

int G_NumPlayersWithWeapon( weapon_t weap, team_t team ) {
	int i, j, cnt = 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( level.clients[j].sess.playerType != PC_SOLDIER ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if( level.clients[j].sess.latchPlayerWeapon != weap && level.clients[j].sess.playerWeapon != weap ) {
			continue;
		}

		cnt++;
	}

	return cnt;
}

int G_NumPlayersOnTeam( team_t team ) {
	int i, j, cnt = 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		cnt++;
	}

	return cnt;
}


int G_ClassCount( gentity_t* ent, int cls ) {
	int i, j, cnt;

	if( cls == -1 ) { // we aint checking for a class, so always include ourselves
		cnt = 1;
	} else { // we ARE checking for a class, so ignore ourselves
		cnt = 0;
	}

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];
		
		if( j == ent-g_entities ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != ent->client->sess.sessionTeam ) {
			continue;
		}

		if( cls != -1 ) {
			if( level.clients[j].sess.playerType != cls && level.clients[j].sess.latchPlayerType != cls ) {
				continue;
			}
		}

		cnt++;
	}

	return cnt;
}

qboolean G_IsClassDisabled( gentity_t* ent, int cls, qboolean quiet ) 
{
	int count, wcount;	

	if( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return qfalse;
	}

	count =		G_ClassCount( ent, -1 );
	wcount =	G_ClassCount( ent, cls );

	switch(cls) 
	{
		case PC_SOLDIER:
			if(team_maxSoldiers.integer == -1) return qfalse;
			if(wcount >= team_maxSoldiers.integer) {
				if (!quiet)
					CP("cp \"^1SOLDIER not available^7\" 1");
				return qtrue;
			}
		case PC_MEDIC:
			if(team_maxMedics.integer == -1) return qfalse;
			if(wcount >= team_maxMedics.integer) {
				if (!quiet)
					CP("cp \"^1MEDIC not available^7\" 1");
				return qtrue;
			}
		case PC_ENGINEER:
			if(team_maxEngineers.integer == -1) return qfalse;
			if(wcount >= team_maxEngineers.integer) {
				if (!quiet)
					CP("cp \"^1ENGINEER not available^7\" 1");
				return qtrue;
			}
		case PC_FIELDOPS:
			if(team_maxFieldops.integer == -1) return qfalse;
			if(wcount >= team_maxFieldops.integer) {
				if (!quiet)
					CP("cp \"^1FIELDOPS not available^7\" 1");
				return qtrue;
			}
		case PC_COVERTOPS:
			if(team_maxCovertops.integer == -1) return qfalse;
			if(wcount >= team_maxCovertops.integer) {
				if (!quiet)
					CP("cp \"^1COVERTOPS not available^7\" 1");
				return qtrue;
			}
		default:	
			break;
	}
	return qfalse;
}

qboolean G_IsHeavyWeapon( weapon_t weap ) {
	int i;

	for( i = 0; i < NUM_HEAVY_WEAPONS; i++ ) {
		if( bg_heavyWeapons[i] == weap ) {
			return qtrue;
		}
	}

	return qfalse;
}

int G_TeamCount( gentity_t* ent, weapon_t weap ) {
	int i, j, cnt;

	if( weap == -1 ) { // we aint checking for a weapon, so always include ourselves
		cnt = 1;
	} else { // we ARE checking for a weapon, so ignore ourselves
		cnt = 0;
	}

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];
		
		if( j == ent-g_entities ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != ent->client->sess.sessionTeam ) {
			continue;
		}

		if( weap != -1 ) {
			if ( weap == WP_M7 || weap == WP_GPG40 ){
				if ( !level.clients[j].sess.rifleNade || ( level.clients[j].sess.playerWeapon != weapAlts[weap] && level.clients[j].sess.latchPlayerWeapon != weapAlts[weap] ) ) continue; // jaquboss added case for non rifle owners
			} else 	if( level.clients[j].sess.playerWeapon != weap && level.clients[j].sess.latchPlayerWeapon != weap ) {
				continue;
			}
		}

		cnt++;
	}

	return cnt;
}

qboolean G_IsWeaponDisabled( gentity_t* ent, weapon_t weapon, qboolean quiet ) 
{
	int count, wcount;	

	if( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return qtrue;
	}

	count =		G_TeamCount( ent, -1 );
	wcount =	G_TeamCount( ent, weapon );

	if(G_IsHeavyWeapon(weapon)) {
		if( wcount >= ceil( count * g_heavyWeaponRestriction.integer * 0.01f ) ) {
			return qtrue;
		}
	}

	switch(weapon) 
	{
		case WP_VENOM:
			if(!(jp_insanity.integer & JP_INSANITY_VENOM) ){ 
				if (!quiet)
					CP("cp \"^1VENOM GUN is not available^7\" 1");
				return qtrue;
			}
			if(team_maxVenoms.integer == -1) return qfalse;
			if(wcount >= team_maxVenoms.integer) {
				if (!quiet)
					CP("cp \"^1VENOM not available^7\" 1");
				return qtrue;
			}
		case WP_SHOTGUN:
			if(team_maxShotguns.integer == -1) return qfalse;
			if(wcount >= team_maxShotguns.integer) {
				if (!quiet)
					CP("cp \"^1SHOTGUN not available^7\" 1");
				return qtrue;
			}
		case WP_PANZERFAUST:
			if(team_maxPanzers.integer == -1) return qfalse;
			if(wcount >= team_maxPanzers.integer) {
				if(!quiet)
					CP("cp \"^1PANZERFAUST not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_BAZOOKA:
			if(team_maxPanzers.integer == -1) return qfalse;
			if(wcount >= team_maxPanzers.integer) {
				if(!quiet)
					CP("cp \"^1BAZOOKA not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_MOBILE_BROWNING:
			if(team_maxMG42s.integer == -1) return qfalse;
			if(wcount >= team_maxMG42s.integer) {
				CP("cp \"^1Browning not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_MOBILE_MG42:
			if(team_maxMG42s.integer == -1) return qfalse;
			if(wcount >= team_maxMG42s.integer) {
				if(!quiet) 
					CP("cp \"^1MG42 not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_FLAMETHROWER:
			if(team_maxFlamers.integer == -1) return qfalse;
			if(wcount >= team_maxFlamers.integer) {
				if(!quiet) 
					CP("cp \"^1FLAMETHROWER not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_MORTAR:
		case WP_MORTAR2:
			if(team_maxMortars.integer == -1) return qfalse;
			if(wcount >= team_maxMortars.integer) {
				if(!quiet) 
					CP("cp \"^1MORTAR not available^7\" 1");
				return qtrue;
			}
			break;
		case WP_GPG40:
		case WP_M7: // jaquboss - not the carbine!!
			if(team_maxriflegrenades.integer == -1) return qfalse;
			if(wcount >= team_maxriflegrenades.integer) {
				if(!quiet)
					CP("cp \"^1GRENADE LAUNCHER not available^7\" 1");
				return qtrue;
			}
			break;
		default:	// Jaybird
			break;
	}
	return qfalse;
}

void G_SetClientWeapons( gentity_t* ent, weapon_t w1, weapon_t w2, qboolean updateclient ) {
	qboolean changed = qfalse;

	if( ent->client->sess.latchPlayerWeapon2 != w2 ) {
		ent->client->sess.latchPlayerWeapon2 = w2;
		changed = qtrue;
	}

	if( !G_IsWeaponDisabled( ent, w1, qfalse ) ) {
		if( ent->client->sess.latchPlayerWeapon != w1 ) {
			ent->client->sess.latchPlayerWeapon = w1;
			changed = qtrue;
		}		
	} else {
		if( ent->client->sess.latchPlayerWeapon != 0 ) {
			ent->client->sess.latchPlayerWeapon = 0;
			changed = qtrue;
		}
	}

	if( updateclient && changed ) {
		ClientUserinfoChanged( ent-g_entities );
	}
}

/*
=================
Cmd_PcInfo_f
=================
*/
#ifdef __SSYSTEM__
int G_ClampPCIndex( int val, int max, qboolean undeselectable, gentity_t *ent )
{
	if ( undeselectable ) {
		if (val < 0 || max < 0){
		//	CP(va("print \"invalid value %i, changing to %i\n\"", val, 0));
			return 0;
		}
	} else {
		if (val < 0){
		//	CP(va("print \"invalid value %i, changing to %i\n\"", val, -1));
			return -1;	
		}
	}

	if ( val > max ){
	//	CP(va("print \"invalid value %i, changing to %i\n\"", val, max));
		return max;
	}

	return val;
}

void Cmd_PcInfo_f( gentity_t *ent ) {
	char		tempString[8];
	int			tempInt;
	playerCharacter_t *pc;
	bg_character_t *character;
	int			i;

	if ( trap_Argc() < 2 ) {
		CP(va("print \"usage: <headmodel> <headskin> <bodymodel> <torsoskin> <legsskin> <helmetmodel> <helmetskin> <backpackmodel> <backpackskin> <lbeltmodel> <lbeltskin> <rbeltmodel> <rbeltskin> <glassmodel> <glassskin> \n\""));
		return;
	}

	pc = &ent->client->pers.latchedpc;
	ent->client->pers.pcLatched = qtrue;

	character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.latchPlayerType );

	for ( i = 1 ; i < 16 ; i++ ){
		trap_Argv( i, tempString,	sizeof( tempString ));
		tempInt = atoi( tempString );
	
		switch ( i ){
			default:
				continue;
			// head
			case 1:
				pc->headIndex = G_ClampPCIndex(tempInt, character->numHeads-1, qtrue, ent);
				break;
			case 2:
				pc->faceIndex = G_ClampPCIndex(tempInt, character->head[pc->headIndex].numSkins-1, qtrue, ent);
				break;
			// body
			case 3:
				pc->bodyIndex = G_ClampPCIndex(tempInt, character->numBodies-1, qtrue, ent);
				break;
			case 4:
				pc->torsoIndex = G_ClampPCIndex(tempInt, character->body[pc->bodyIndex].numTorsoSkins-1, qtrue, ent);
				break;
			case 5:
				pc->legIndex = G_ClampPCIndex(tempInt, character->body[pc->bodyIndex].numLegsSkins-1, qtrue, ent);
				break;
			// helmet
			case 6:
				pc->helmetIndex = G_ClampPCIndex(tempInt, character->numAcc[ACC_HAT]-1, qtrue, ent);
				break;
			case 7:
				pc->helmetSkinIndex = G_ClampPCIndex(tempInt, character->acc[ACC_HAT][pc->helmetIndex].numSkins-1, qtrue, ent);
				break;
			// backpack
			case 8:
				pc->backPackIndex = G_ClampPCIndex(tempInt, character->numAcc[ACC_BACK]-1, qfalse, ent);
				break;
			case 9:
				if ( pc->backPackIndex >= 0 ) 
				pc->backPackSkinIndex = G_ClampPCIndex(tempInt, character->acc[ACC_BACK][pc->backPackSkinIndex].numSkins-1, qtrue, ent);
				break;
			// left belt
			case 10:
				pc->bLeftIndex = G_ClampPCIndex(tempInt, character->numAcc[ACC_BELT_LEFT]-1, qfalse, ent);
				break;
			case 11:
				if ( pc->bLeftIndex >= 0 ) 
				pc->bLeftSkinIndex = G_ClampPCIndex(tempInt, character->acc[ACC_BELT_LEFT][pc->bLeftIndex].numSkins-1, qtrue, ent);
				break;
			// right belt
			case 12:
				pc->bRightIndex = G_ClampPCIndex(tempInt, character->numAcc[ACC_BELT_RIGHT]-1, qfalse, ent);
				break;
			case 13:
				if ( pc->bRightIndex >= 0 ) 
				pc->bRightSkinIndex = G_ClampPCIndex(tempInt, character->acc[ACC_BELT_RIGHT][pc->bRightIndex].numSkins-1, qtrue, ent);
				break;
			// glasses
			case 14:
				pc->glassIndex = G_ClampPCIndex(tempInt, character->numAcc[ACC_MOUTH3]-1, qfalse, ent);
				break;
			case 15:
				if ( pc->glassIndex >= 0 ) 
				pc->glassSkinIndex = G_ClampPCIndex(tempInt, character->acc[ACC_MOUTH3][pc->glassIndex].numSkins-1, qtrue, ent);
				break;
		}
	}
}
#endif

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue ) {
	char		s[MAX_TOKEN_CHARS];
	char		ptype[4];
	char		weap[4], weap2[4];
	weapon_t	w, w2;


	if ( trap_Argc() < 2 ) {
		char *pszTeamName;

		switch ( ent->client->sess.sessionTeam ) {
			case TEAM_ALLIES:
				pszTeamName = "Allies";
				break;
			case TEAM_AXIS:
				pszTeamName = "Axis";
				break;
			case TEAM_SPECTATOR:
				pszTeamName = "Spectator";
				break;
			case TEAM_FREE:
			default:
				pszTeamName = "Free";
				break;
		}

		CP(va("print \"%s team\n\"", pszTeamName));
		return;
	}

	trap_Argv( 1, s,		sizeof( s		));
	trap_Argv( 2, ptype,	sizeof( ptype	));
	trap_Argv( 3, weap,		sizeof( weap	));
	trap_Argv( 4, weap2,	sizeof( weap2	));

	w =		atoi( weap );
	w2 =	atoi( weap2 );

	// tjw: allow old-school q2 players to use the '/team r' or '/team b' commands without losing their class
	if(*ptype) {
		ent->client->sess.latchPlayerType =	atoi( ptype );

		if( G_IsClassDisabled( ent, ent->client->sess.latchPlayerType, qtrue ) ) {
			qboolean found=qfalse;
			int		i;

			for ( i=0; i<NUM_PLAYER_CLASSES; i++)
			{
				if (!G_IsClassDisabled( ent, i, qtrue ))
				{
					ent->client->sess.latchPlayerType = ent->client->sess.playerType = i;
					found = qtrue;
					break;
				}
			}
			// no other option, have to kick him to spectator
			if ( !found ){
				SetTeam( ent, "s", qtrue, -1, -1, qfalse );
				return;
			}
		}
	}
	
	if( ent->client->sess.latchPlayerType < PC_SOLDIER || ent->client->sess.latchPlayerType > PC_COVERTOPS ) {

			ent->client->sess.latchPlayerType = PC_SOLDIER;
	}

	if( !SetTeam( ent, s, qfalse, w, w2, qtrue ) ) {
		G_SetClientWeapons( ent, w, w2, qtrue );
	}
}

void Cmd_ResetSetup_f( gentity_t* ent ) {
	qboolean changed = qfalse;

	if( !ent || !ent->client ) {
		return;
	}

	ent->client->sess.latchPlayerType =		ent->client->sess.playerType;
	
	if( ent->client->sess.latchPlayerWeapon != ent->client->sess.playerWeapon ) {
		ent->client->sess.latchPlayerWeapon = ent->client->sess.playerWeapon;
		changed = qtrue;
	}

	if( ent->client->sess.latchPlayerWeapon2 != ent->client->sess.playerWeapon2 ) {
		ent->client->sess.latchPlayerWeapon2 =	ent->client->sess.playerWeapon2;
		changed = qtrue;
	}

	if( changed ) {
		ClientUserinfoChanged( ent-g_entities );
	}
}

void Cmd_SetClass_f( gentity_t* ent, unsigned int dwCommand, qboolean fValue ) {
	char		s[MAX_TOKEN_CHARS];
	char		ptype[4];
	char		weap[4], weap2[4];
	weapon_t	w, w2;


	if ( trap_Argc() < 2 ) {
		char *pszClassName;

		switch ( ent->client->sess.latchPlayerType )
		{
			case PC_SOLDIER:
				pszClassName = "Soldier";
				break;
			case PC_MEDIC:
				pszClassName = "Axis";
				break;
			case PC_ENGINEER:
				pszClassName = "Engineer";
				break;
			case PC_FIELDOPS:
				pszClassName = "Field Ops";
				break;
			case PC_COVERTOPS:
				pszClassName = "Covert Ops";
				break;
			default:
				pszClassName = "^1Unknown!";
				break;
		}

		CP(va("print \"%s class\n\"", pszClassName));
		return;
	}

	if (ent->client->sess.sessionTeam == TEAM_ALLIES)
	{
		strcpy(s, "Allies");
	}
	else if(ent->client->sess.sessionTeam == TEAM_AXIS)
	{
		strcpy(s, "Axis");
	}
	else
	{
		CP(va("print \"^3Cannot pick a class unless you're on a team!\n\""));
	}

	//trap_Argv( 1, s,		sizeof( s		));
	trap_Argv( 1, ptype,	sizeof( ptype	));
	trap_Argv( 2, weap,		sizeof( weap	));
	trap_Argv( 3, weap2,	sizeof( weap2	));

	w  =	atoi( weap );
	w2 =	atoi( weap2 );
		
	// dont change playertype if it's invalid
	if( *ptype && atoi( ptype ) >= PC_SOLDIER && atoi( ptype) <= PC_COVERTOPS )
	{	
		ent->client->sess.latchPlayerType = atoi( ptype );
	}

	if( !SetTeam( ent, s, qfalse, w, w2, qtrue ) )
	{
		G_SetClientWeapons( ent, w, w2, qtrue );
	}
}

void Cmd_SetWeapons_f( gentity_t* ent, unsigned int dwCommand, qboolean fValue ) {
}



// START Mad Doc - TDF
/*
=================
Cmd_TeamBot_f
=================
*/
void Cmd_TeamBot_f(gentity_t *foo)
{
	char		ptype[4], weap[4], fireteam[4];
	char entNumStr[4];
	int entNum;
	char *weapon;
	char weaponBuf[MAX_INFO_STRING];
	char userinfo[MAX_INFO_STRING];

	gentity_t *ent;

	trap_Argv( 1, entNumStr, sizeof( entNumStr ) );
	entNum = atoi(entNumStr);

	ent = g_entities + entNum;

	trap_Argv( 3, ptype, sizeof( ptype ) );
	trap_Argv( 4, weap, sizeof( weap ) );
	trap_Argv( 5, fireteam, sizeof( fireteam) );



	ent->client->sess.latchPlayerType = atoi( ptype );
	ent->client->sess.latchPlayerWeapon = atoi( weap );
	ent->client->sess.latchPlayerWeapon2 = 0;
	ent->client->sess.playerType = atoi( ptype );
	ent->client->sess.playerWeapon = atoi( weap );

	// remove any weapon info from the userinfo, so SetWolfSpawnWeapons() doesn't reset the weapon as that
	trap_GetUserinfo(entNum, userinfo, sizeof(userinfo) );
		
	weapon = Info_ValueForKey( userinfo, "pWeapon");
	if (weapon[0])
	{
		Q_strncpyz(weaponBuf, weapon, sizeof(weaponBuf));
		Info_RemoveKey(userinfo, "pWeapon");
		trap_SetUserinfo(entNum, userinfo);
	}

 	SetWolfSpawnWeapons( ent->client ); 
}

// END Mad Doc - TDF
/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue ) {
	int		i;
	char	arg[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];

	if ( trap_Argc() != 2 ) {
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			StopFollowing( ent );
		}
		return;
	}

	if ((ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES) && !(ent->client->ps.pm_flags & PMF_LIMBO)) 
	{
		CP("print \"Can't follow while not in limbo if on a team!\n\"");
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	// josh: Let /follow match partial names
	// Use > instead of != since could be a team name
	if(ClientNumbersFromString(arg, pids) > 1) 
	{
		CP("print \"Partial Name Matches more than 1 Player.\n\"");
		return;
	}
	i = pids[0];

	if ( i == -1 ) {
		if(!Q_stricmp(arg, "allies")) i = TEAM_ALLIES;
		else if(!Q_stricmp(arg, "axis")) i = TEAM_AXIS;
		else return;
		if ((ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES) &&
			ent->client->sess.sessionTeam != i) {
			CP("print \"Can't follow a player on an enemy team!\n\"");
			return;
		}

		if(!TeamCount(ent - g_entities, i)) {
			CP(va("print \"The %s team %s empty!  Follow command ignored.\n\"", aTeams[i],
																((ent->client->sess.sessionTeam != i) ? "is" : "would be")));
			return;
		}

		// Allow for simple toggle
		if(ent->client->sess.spec_team != i) {
			if(teamInfo[i].spec_lock && !(ent->client->sess.spec_invite & i)) CP(va("print \"Sorry, the %s team is locked from spectators.\n\"", aTeams[i]));
			else {
				ent->client->sess.spec_team = i;
				CP(va("print \"Spectator follow is now locked on the %s team.\n\"", aTeams[i]));
				Cmd_FollowCycle_f(ent, 1);
			}
		} else {
			ent->client->sess.spec_team = 0;
			CP(va("print \"%s team spectating is now disabled.\n\"", aTeams[i]));
		}

		return;
	}

	// Josh: Can't follow enemy players if not a spectator
	if ((ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES) &&
			ent->client->sess.sessionTeam != level.clients[ i ].sess.sessionTeam) {
		CP("print \"Can't follow a player on an enemy team!\n\"");
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client ) return;

	// can't follow another spectator
	if ( level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR ) return;
	if ( level.clients[ i ].ps.pm_flags & PMF_LIMBO ) return;

	// OSP - can't follow a player on a speclocked team, unless allowed
	if(!G_allowFollow(ent, level.clients[i].sess.sessionTeam)) {
		CP(va("print \"Sorry, the %s team is locked from spectators.\n\"", aTeams[level.clients[i].sess.sessionTeam]));
		return;
	}

	// first set them to spectator
	// josh: don't set them to spectator
	//if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
	//	SetTeam( ent, "spectator", qfalse, -1, -1, qfalse );
	//}

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) {
	int		clientnum;
	int		original;

	// first set them to spectator
	if (( ent->client->sess.spectatorState == SPECTATOR_NOT ) && (!( ent->client->ps.pm_flags & PMF_LIMBO)) ) { // JPW NERVE for limbo state
		SetTeam( ent, "spectator", qfalse, -1, -1, qfalse );
	}

	if ( dir != 1 && dir != -1 ) {
		G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	clientnum = ent->client->sess.spectatorClient;
	original = clientnum;
	do {
		clientnum += dir;
		if ( clientnum >= level.maxclients ) {
			clientnum = 0;
		}
		if ( clientnum < 0 ) {
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED ) {
			continue;
		}

		// can't follow another spectator
		if ( level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		// JPW NERVE -- couple extra checks for limbo mode
		if (ent->client->ps.pm_flags & PMF_LIMBO) {
			if (level.clients[clientnum].ps.pm_flags & PMF_LIMBO)
				continue;
			if (level.clients[clientnum].sess.sessionTeam != ent->client->sess.sessionTeam)
				continue;
		}

		if (level.clients[clientnum].ps.pm_flags & PMF_LIMBO)
			continue;

		// OSP
		if(!G_desiredFollow(ent, level.clients[clientnum].sess.sessionTeam))
			continue;

		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	} while ( clientnum != original );

	// tjw: if we're still following the same guy and he's not
	//      on the same team, then follow ourself
	if(ent->client->sess.spectatorClient == original &&
		ent->client->sess.sessionTeam != 
		level.clients[original].sess.sessionTeam) {

		ent->client->ps.viewlocked = 0;
		ent->client->ps.viewlocked_entNum = 0;
		ent->client->sess.spectatorClient =
			(ent->client - level.clients);
		ent->client->sess.spectatorState = SPECTATOR_FREE;
	}

	// leave it where it was
}


/*======================
G_EntitySound
	Mad Doc xkan, 11/06/2002 -

	Plays a sound (wav file or sound script) on this entity

	Note that calling G_AddEvent(..., EV_GENERAL_SOUND, ...) has the danger of
	the event never getting through to the client because the entity might not
	be visible (unless it has the SVF_BROADCAST flag), so if you want to make sure
	the sound is heard, call this function instead.
======================*/
void G_EntitySound( 
	gentity_t *ent,			// entity to play the sound on
	const char *soundId,	// sound file name or sound script ID
	int volume)				// sound volume, only applies to sound file name call
							//   for sound script, volume is currently always 127.
{
	trap_SendServerCommand( -1, va("entitySound %d %s %d %i %i %i normal", ent->s.number, soundId, volume,
		(int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ));
}

/*======================
G_EntitySoundNoCut
	Mad Doc xkan, 1/16/2003 -

	Similar to G_EntitySound, but do not cut this sound off

======================*/
void G_EntitySoundNoCut( 
	gentity_t *ent,			// entity to play the sound on
	const char *soundId,	// sound file name or sound script ID
	int volume)				// sound volume, only applies to sound file name call
							//   for sound script, volume is currently always 127.
{
	trap_SendServerCommand( -1, va("entitySound %d %s %d %i %i %i noCut", ent->s.number, soundId, volume,
		(int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ));
}

char *G_ShortcutSanitize(char *text)
{
	static char n[MAX_SAY_TEXT] = {""};

	if(!text || !*text)  
		return n; 
	
	Q_strncpyz(n, text, sizeof(n));  

	Q_strncpyz(n, Q_StrReplace(n, "[a]", "(a)"), sizeof(n));     
	Q_strncpyz(n, Q_StrReplace(n, "[d]", "(d)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[h]", "(h)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[k]", "(k)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[l]", "(l)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[n]", "(n)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[r]", "(r)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[p]", "(p)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[s]", "(s)"), sizeof(n));  
	Q_strncpyz(n, Q_StrReplace(n, "[w]", "(w)"), sizeof(n));
	return n;
}

char *G_Shortcuts(gentity_t *ent, char *text)
{
	static char out[MAX_SAY_TEXT];
	char a[MAX_NAME_LENGTH] = {"*unknown*"};
	char d[MAX_NAME_LENGTH] = {"*unknown*"};
	char h[MAX_NAME_LENGTH] = {"*unknown*"};
	char k[MAX_NAME_LENGTH] = {"*unknown*"};
	char l[32] = {"*unknown*"};
	char n[MAX_NAME_LENGTH] = {"*unknown*"};
	char r[MAX_NAME_LENGTH] = {"*unknown*"};
	char p[MAX_NAME_LENGTH] = {"*unknown*"};
	char s[32] = {"*unknown*"};
//	char w[32] = {"*unknown*"};
	char w[32] = {"Nothing"}; // jaquboss most likely happen if we have no weapon
	char t[32] = {"*unknown*"};
	gclient_t *client = NULL;
	gentity_t *crosshairEnt;
	char *rep;
	gitem_t *weapon;
	int clip;
	int ammo;

	out[0] = '\0';

	if (ent ) {
		if(ent->client->pers.lastammo_client != -1 && 
			( ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
			client = &level.clients[ent->client->pers.lastammo_client];
			if(client) {
				Q_strncpyz(a,
					G_ShortcutSanitize(client->pers.netname),
					sizeof(a));
			}
		}

		if(ent->client->pers.lastkiller_client != -1 && 
			( ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
			client = &level.clients[ent->client->pers.lastkiller_client];
			if(client) {
				Q_strncpyz(d,
					G_ShortcutSanitize(client->pers.netname),
					sizeof(d));
			}
		}

		if(ent->client->pers.lasthealth_client != -1 && 
			( ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
			client = &level.clients[ent->client->pers.lasthealth_client];
			if(client) {
				Q_strncpyz(h,
					G_ShortcutSanitize(client->pers.netname),
					sizeof(h));
			}
		}

		if(ent->client->pers.lastkilled_client != -1 &&
			( ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
			client = &level.clients[ent->client->pers.lastkilled_client];
			if(client) {
				Q_strncpyz(k,
					G_ShortcutSanitize(client->pers.netname),
					sizeof(k));
			}
		}

		Q_strncpyz(l,
			BG_GetLocationString(ent->r.currentOrigin),
			sizeof(l));

		Q_strncpyz(n,
			G_ShortcutSanitize(ent->client->pers.netname),
			sizeof(n));

		if(ent->client->pers.lastrevive_client != -1  && 
			( ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
			client = &level.clients[ent->client->pers.lastrevive_client];
			if(client) {
				Q_strncpyz(r,
					G_ShortcutSanitize(client->pers.netname),
					sizeof(r));
			}
		}

		crosshairEnt = &g_entities[ent->client->ps.identifyClient];
		// Dens: only give the name of the other client, if the player should be able to see it
		if(crosshairEnt && crosshairEnt->client && crosshairEnt->inuse && 
			(ent->client->sess.sessionTeam == crosshairEnt->client->sess.sessionTeam ||
			crosshairEnt->client->ps.powerups[PW_OPS_DISGUISED] ||
			ent->client->sess.sessionTeam == TEAM_SPECTATOR)) {
			client = crosshairEnt->client;
			if(client) {
				// Dens: show the name of the owner of the suit
				if(client->ps.powerups[PW_OPS_DISGUISED] &&
					(ent->client->sess.sessionTeam != client->sess.sessionTeam)){
					Q_strncpyz(p,
						G_ShortcutSanitize(client->disguiseNetname),
						sizeof(p));
				} else {
					Q_strncpyz(p,
						G_ShortcutSanitize(client->pers.netname),
						sizeof(p));
				}
			}
		}

		if (  ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES ){
			Com_sprintf(s, sizeof(s), "%i", ent->health);

			weapon = BG_FindItemForWeapon(ent->client->ps.weapon);
			if ( weapon ){
				Q_strncpyz(w, weapon->pickup_name, sizeof(w));

				clip = BG_FindClipForWeapon(ent->client->ps.weapon);
				ammo = BG_FindAmmoForWeapon(ent->client->ps.weapon);
				Com_sprintf(t, sizeof(t), "%i",
					(ent->client->ps.ammoclip[clip] +
					((ent->client->ps.weapon == WP_KNIFE) ? 0 : ent->client->ps.ammo[ammo])));
			}
		}
	}


	rep = Q_StrReplace(text, "[a]", a);
	rep = Q_StrReplace(rep, "[d]", d);
	rep = Q_StrReplace(rep, "[h]", h);
	rep = Q_StrReplace(rep, "[k]", k);
	rep = Q_StrReplace(rep, "[l]", l);
	rep = Q_StrReplace(rep, "[n]", n);
	rep = Q_StrReplace(rep, "[r]", r);
	rep = Q_StrReplace(rep, "[p]", p);
	rep = Q_StrReplace(rep, "[s]", s);
	rep = Q_StrReplace(rep, "[w]", w);
	rep = Q_StrReplace(rep, "[t]", t);

	Q_strncpyz(out, rep, sizeof(out));
	return out;
}

/*
==================
G_Say
==================
*/

void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize )
{
	char cmd[6];

	if( !other || !other->inuse || !other->client )
		return;

	if((mode == SAY_TEAM || mode == SAY_TEAMNL) && !OnSameTeam(ent, other) && !G_shrubbot_permission(other, SBF_SPEC_ALLCHAT)) 
		return;

	if((mode == SAY_TEAM || mode == SAY_TEAMNL) && G_shrubbot_permission(other, SBF_SPEC_ALLCHAT) && other->client->sess.sessionTeam != TEAM_SPECTATOR && !OnSameTeam(ent, other))
		return;

	// NERVE - SMF - if spectator, no chatting to players in WolfMP
	if ( match_mutespecs.integer > 0 && ent->client->sess.referee == 0 &&	// OSP
		(( ent->client->sess.sessionTeam == TEAM_FREE && other->client->sess.sessionTeam != TEAM_FREE ) ||
		( ent->client->sess.sessionTeam == TEAM_SPECTATOR && other->client->sess.sessionTeam != TEAM_SPECTATOR ))) {
		return;
	} else {
		if ( mode == SAY_BUDDY ) { 	// send only to people who have the sender on their buddy list
			if(ent->s.clientNum != other->s.clientNum) {
				fireteamData_t *ft1, *ft2;
				if(!G_IsOnFireteam( other-g_entities, &ft1 )) 
					return;
				if(!G_IsOnFireteam( ent-g_entities, &ft2 ))
					return;
				if(ft1 != ft2)
					return;
			}
		}
		
		if(COM_BitCheck(other->client->sess.ignoreClients, (ent - g_entities))) {
			Q_strncpyz(cmd, "print", sizeof(cmd));

#ifdef OMNIBOTS
		// Omni-bot: Tell the bot about the chat message
		if(other->r.svFlags & SVF_BOT)
		{
			int iMsg = PERCEPT_HEAR_GLOBALCHATMSG;
			BotUserData bud;
			bud.DataType = dtString;
			bud.udata.m_String = message;
			switch(mode)
			{
			case SAY_ALL:
				iMsg = PERCEPT_HEAR_GLOBALCHATMSG;
				break;
			case SAY_TEAM:
			case SAY_TEAMNL:
				iMsg = PERCEPT_HEAR_TEAMCHATMSG;
				break;
			case SAY_BUDDY:
				iMsg = PERCEPT_HEAR_PRIVCHATMSG;
				break;
			}

			Bot_Interface_SendEvent(iMsg, other-g_entities, ent-g_entities, 0, &bud);
		}
#endif
		} else if(mode == SAY_TEAM || mode == SAY_BUDDY) {
			Q_strncpyz(cmd, "tchat", sizeof(cmd));

			trap_SendServerCommand( other-g_entities,
				va("%s \"%c%c%s%s\" %i %i %i %i %i",
				cmd,
				 Q_COLOR_ESCAPE, color, message,
				(!Q_stricmp(cmd, "print")) ? "\n" : "",
				ent-g_entities, localize,
				(int)ent->s.pos.trBase[0],
				(int)ent->s.pos.trBase[1],
				(int)ent->s.pos.trBase[2] ));

		} else {
			Q_strncpyz(cmd, "chat", sizeof(cmd));

			trap_SendServerCommand(other-g_entities,
				va("%s \"%s%c%c%s%s\" %i %i",
				cmd, name, Q_COLOR_ESCAPE, color,
				message,
				(!Q_stricmp(cmd, "print")) ? "\n" : "",
				ent-g_entities, localize));
		}

	}
}

void G_HQSay(  gentity_t *other, int color, const char *name, const char *message )
{

	if( !other || !other->inuse || !other->client )
	{
		return;
	}

	trap_SendServerCommand(other-g_entities,
			va("gamechat \"%s%c%c%s\" 1", 
			name, Q_COLOR_ESCAPE, color, message));

}

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) {
	int			j;
	gentity_t	*other;
	int			color;
	char		name[64];
	// don't let text be too long for malicious reasons
	char		text[MAX_SAY_TEXT];
	char		censoredText[MAX_SAY_TEXT];
	qboolean	localize = qfalse;
	char		*loc;
	char		*shortcuts; 

	Q_strncpyz( text, chatText, sizeof(text) );

	if (g_censor.string[0] && !(G_shrubbot_permission(ent, SBF_NOCENSORFLOOD))) 
	{
		SanitizeString(text, censoredText, qtrue);
		if (G_CensorText(censoredText,&censorDictionary))
		{
			// like shrub, if the text is censored, it becomes colorless
			Q_strncpyz( text, censoredText, sizeof(text) );
			G_CensorPenalize(ent);
		}
	}

	if(g_shortcuts.integer) {  
		shortcuts = G_Shortcuts(ent, text);  
		Q_strncpyz(text, shortcuts, sizeof(text));
	}

	switch ( mode ) {
		default:
		case SAY_ALL:
			G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, text );
			Com_sprintf (name, sizeof(name), "%s%c%c: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
			color = COLOR_GREEN;
			break;
		case SAY_BUDDY:
			localize = qtrue;
			G_LogPrintf( "saybuddy: %s: %s\n", ent->client->pers.netname, text );
			loc = BG_GetLocationString( ent->r.currentOrigin );
			Com_sprintf (name, sizeof(name), "[lof](%s%c%c) (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, loc);
			color = COLOR_YELLOW;
			break;
		case SAY_TEAM:
			localize = qtrue;			
			G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, text );
			loc = BG_GetLocationString( ent->r.currentOrigin );
			Com_sprintf (name, sizeof(name), "[lof](%s%c%c) (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, loc);
			color = COLOR_CYAN;
			break;
		case SAY_TEAMNL:
			G_LogPrintf( "sayteamnl: %s: %s\n", ent->client->pers.netname, text );
			Com_sprintf (name, sizeof(name), "(%s^7): ", ent->client->pers.netname);
			color = COLOR_CYAN;
			break;
	}

	if ( target ) {
		if( !COM_BitCheck( target->client->sess.ignoreClients, ent - g_entities ) ) {
			G_SayTo( ent, target, mode, color, name, text, localize );
		}
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for(j=0; j<level.numConnectedClients; j++) {
		other = &g_entities[level.sortedClients[j]];
		G_SayTo( ent, other, mode, color, name, text, localize );
	}

	G_shrubbot_cmd_check(ent);
}


/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 )
{
	char *args;

	if(trap_Argc() < 2 && !arg0) return;
	args = Q_SayConcatArgs(0);
	if(g_privateMessages.integer &&
		(!Q_stricmpn(args, "say /m ", 7) ||
		 !Q_stricmpn(args, "say_team /m ", 12) ||
		 !Q_stricmpn(args, "say_buddy /m ", 13) ||
		 !Q_stricmpn(args, "say /pm ", 8) ||
		 !Q_stricmpn(args, "say_team /pm ", 13) ||
		 !Q_stricmpn(args, "say_buddy /pm ", 14))) {

		G_PrivateMessage(ent);
	}
	else
		G_Say(ent, NULL, mode, ConcatArgs(((arg0) ? 0 : 1)));
}

extern void BotRecordVoiceChat( int client, int destclient, const char *id, int mode, qboolean noResponse );

// NERVE - SMF
void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceonly, float randomNum ) {
	int color;
	char *cmd;
	qboolean disguise=0;


	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}
	if ( mode == SAY_TEAM && !OnSameTeam(ent, other) ) {
		if ( ent->client->sess.playerType == PC_COVERTOPS &&
			ent->client->sess.skill[SK_COVERTOPS] >= 8 &&
			ent->client->ps.powerups[PW_OPS_DISGUISED] &&
			(!Q_stricmp( id, "Medic") || !Q_stricmp( id, "NeedAmmo") || !Q_stricmp( id, "FTHealMe") || !Q_stricmp( id, "FTResupplyMe") )
			){
				disguise = 1;
		} else {
		return;
		}
	}

	// OSP - spec vchat rules follow the same as normal chatting rules
	if(match_mutespecs.integer > 0 && ent->client->sess.referee == 0 &&
	  ent->client->sess.sessionTeam == TEAM_SPECTATOR && other->client->sess.sessionTeam != TEAM_SPECTATOR) {
		return;
	}

	// send only to people who have the sender on their buddy list
	if ( mode == SAY_BUDDY ) {
		if(ent->s.clientNum != other->s.clientNum) {
			fireteamData_t *ft1, *ft2;
			if(!G_IsOnFireteam( other-g_entities, &ft1 )) {
				return;
			}
			if(!G_IsOnFireteam( ent-g_entities, &ft2 )) {
				return;
			}
			if(ft1 != ft2) {
				return;
			}
		}
	}

	if( mode == SAY_TEAM ) {
		color = COLOR_CYAN;
		cmd = "vtchat";
	} else if( mode == SAY_BUDDY ) {
		color = COLOR_YELLOW;
		cmd = "vbchat";
	} else {
		color = COLOR_GREEN;
		cmd = "vchat";
	}

	// RF, record this chat so bots can parse them
	// bots respond with voiceonly, so we check for this so they dont keep responding to responses
#ifndef NO_BOT_SUPPORT
	BotRecordVoiceChat( ent->s.number, other->s.number, id, mode, voiceonly == 2 );
#endif

#ifdef OMNIBOTS
	// Omni-bot Send this voice macro to the bot as an event.
	if(other->r.svFlags & SVF_BOT)
	{
		int iMessageId = PERCEPT_HEAR_GLOBALVOICEMACRO;		
		botUserData.DataType = dtString;
		botUserData.udata.m_String = id;

		if(mode == SAY_TEAM)
			iMessageId = PERCEPT_HEAR_TEAMVOICEMACRO;
		else if(mode == SAY_BUDDY )
			iMessageId = PERCEPT_HEAR_PRIVATEVOICEMACRO;
		Bot_Interface_SendEvent( iMessageId, other-g_entities, ent-g_entities, 0.0f, &botUserData );
	}
#endif

	if (voiceonly == 2) {
		voiceonly = qfalse;
	}

	if( mode == SAY_TEAM || mode == SAY_BUDDY ) {
		CPx( other-g_entities, va("%s %d %d %d %s %i %i %i %f %i", cmd, voiceonly, ent - g_entities, color, id, (int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2], randomNum, disguise ));
	} else {
		CPx( other-g_entities, va("%s %d %d %d %s %f", cmd, voiceonly, ent - g_entities, color, id, randomNum ));
	}
}

void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, const char *customChat, qboolean voiceonly ) {
	int			j;
	gentity_t *victim;
	float randomNum=random();

	// DHM - Nerve :: Don't allow excessive spamming of voice chats
	ent->voiceChatSquelch -= (level.time - ent->voiceChatPreviousTime);
	ent->voiceChatPreviousTime = level.time;

	if ( ent->voiceChatSquelch < 0 )
		ent->voiceChatSquelch = 0;

	// Only do the spam check for MP
	if ( ent->voiceChatSquelch >= 30000 && 
			!G_shrubbot_permission(ent, SBF_NOCENSORFLOOD) ) {
		// CHRUKER: b066 - Was using the cpm command, but this needs
		//          to be displayed immediately.
		trap_SendServerCommand( ent-g_entities,
			"cp \"^1Spam Protection^7: VoiceChat ignored\"" );
		return;
	}

	if ( g_voiceChatsAllowed.integer )
		ent->voiceChatSquelch += (34000 / g_voiceChatsAllowed.integer);
	else
		return;
	// dhm

	// OSP - Charge for the lame spam!
	/*if(mode == SAY_ALL && (!Q_stricmp(id, "DynamiteDefused") || !Q_stricmp(id, "DynamitePlanted"))) {
		return;
	}*/

	if ( target ) {
		if ( g_customChat.integer && strlen( customChat ) > 1 ){
			G_Say( ent, target, mode, customChat );
			G_VoiceTo(ent, target, mode, id, qtrue, randomNum);
		} else {
			G_VoiceTo( ent, target, mode, id, voiceonly, randomNum );
		}
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "voice: %s %s\n", ent->client->pers.netname, id);
	}

	if( mode == SAY_BUDDY ) {
		char buffer[32];
		int	cls = -1, i, cnt, num;
		qboolean allowclients[MAX_CLIENTS];

		memset( allowclients, 0, sizeof( allowclients ) );

		trap_Argv( 1, buffer, 32 );

		cls = atoi( buffer );

		trap_Argv( 2, buffer, 32 );
		cnt = atoi( buffer );
		if( cnt > MAX_CLIENTS ) {
			cnt = MAX_CLIENTS;
		}

		for( i = 0; i < cnt; i++ ) {
			trap_Argv( 3 + i, buffer, 32 );

			num = atoi( buffer );
			if( num < 0 ) {
				continue;
			}
			if( num >= MAX_CLIENTS ) {
				continue;
			}

			allowclients[ num ] = qtrue;
		}

		for( j = 0; j < level.numConnectedClients; j++ ) {
			victim = &g_entities[level.sortedClients[j]];
			if( level.sortedClients[j] != ent->s.clientNum ) {
				if( cls != -1 && cls != level.clients[level.sortedClients[j]].sess.playerType ) {
					continue;
				}
			}

			if( cnt ) {
				if( !allowclients[ level.sortedClients[j] ] ) {
					continue;
				}
			}

			if(COM_BitCheck(victim->client->sess.ignoreClients,
				(ent - g_entities))) {
			
				continue;
			}
			if ( g_customChat.integer && strlen( customChat ) > 1 ){
				G_Say( ent, &g_entities[level.sortedClients[j]], mode, customChat );
				G_VoiceTo(ent, &g_entities[level.sortedClients[j]], mode, id, qtrue, randomNum);
			} else {
				G_VoiceTo(ent, &g_entities[level.sortedClients[j]], mode, id, voiceonly, randomNum);
			}
		}
	} else {

		// send it to all the apropriate clients
		for( j = 0; j < level.numConnectedClients; j++ ) {
			victim = &g_entities[level.sortedClients[j]];
			if(COM_BitCheck(victim->client->sess.ignoreClients,
				(ent - g_entities))) {
			
				continue;
			}
				if ( g_customChat.integer && strlen( customChat ) > 1 ){
					G_Say( ent, victim, mode, customChat );
					G_VoiceTo(ent, victim, mode, id, qtrue, randomNum);
				} else {
					G_VoiceTo(ent, victim, mode, id, voiceonly, randomNum);
				}
		}
	}
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f( gentity_t *ent, int mode, qboolean arg0, qboolean voiceonly ) {
	char buf[32];

	if( mode != SAY_BUDDY ) {
		if(trap_Argc() < 2 && !arg0) {
			return;
		}

		trap_Argv( (arg0) ? 0 : 1, buf, sizeof(buf) );

		G_Voice(ent, NULL, mode, buf, ConcatArgs(((arg0) ? 1 : 2)), voiceonly);
	} else {
		char buffer[16];
		int index;
	
		trap_Argv( 2, buffer, sizeof( buffer ) );
		index = atoi( buffer );
		if( index < 0 ) {
			index = 0;
		}

		if( trap_Argc() < 3 + index && !arg0 ) {
			return;
		}

		trap_Argv( (arg0) ? 2 + index : 3 + index, buf, sizeof(buf) );

		G_Voice(ent, NULL, mode, buf, ConcatArgs(((arg0) ? 3 + index : 4 + index)), voiceonly);
	}
}

// TTimo gcc: defined but not used
#if 0
/*
==================
Cmd_VoiceTell_f
==================
*/
static void Cmd_VoiceTell_f( gentity_t *ent, qboolean voiceonly ) {
	int			targetNum;
	gentity_t	*target;
	char		*id;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	id = ConcatArgs( 2 );

	G_LogPrintf( "vtell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, id );
	G_Voice( ent, target, SAY_TELL, id,  voiceonly );
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
		G_Voice( ent, ent, SAY_TELL, id, voiceonly );
	}
}
#endif

// TTimo gcc: defined but not used
#if 0
/*
==================
Cmd_VoiceTaunt_f
==================
*/
static void Cmd_VoiceTaunt_f( gentity_t *ent ) {
	gentity_t *who;
	int i;

	if (!ent->client) {
		return;
	}

	// insult someone who just killed you
	if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number) {
		// i am a dead corpse
		if (!(ent->enemy->r.svFlags & SVF_BOT)) {
//			G_Voice( ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		if (!(ent->r.svFlags & SVF_BOT)) {
//			G_Voice( ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		ent->enemy = NULL;
		return;
	}
	// insult someone you just killed
	if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number) {
		who = g_entities + ent->client->lastkilled_client;
		if (who->client) {
			// who is the person I just killed
			if (who->client->lasthurt_mod == MOD_GAUNTLET) {
				if (!(who->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );	// and I killed them with a gauntlet
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );
				}
			} else {
				if (!(who->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );	// and I killed them with something else
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );
				}
			}
			ent->client->lastkilled_client = -1;
			return;
		}
	}

	if (g_gametype.integer >= GT_TEAM) {
		// praise a team mate who just got a reward
		for(i = 0; i < MAX_CLIENTS; i++) {
			who = g_entities + i;
			if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
				if (who->client->rewardTime > level.time) {
					if (!(who->r.svFlags & SVF_BOT)) {
//						G_Voice( ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					if (!(ent->r.svFlags & SVF_BOT)) {
//						G_Voice( ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					return;
				}
			}
		}
	}

	// just say something
//	G_Voice( ent, NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse );
}
// -NERVE - SMF
#endif

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) {
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}

/*
==================
Cmd_CallVote_f
==================
*/
qboolean Cmd_CallVote_f( gentity_t *ent, unsigned int dwCommand, qboolean fRefCommand ) {
	int		i;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];
	char voteDesc[VOTE_MAXSTRING];
	qboolean sbfNoVoteLimit;
	char		userinfo[MAX_INFO_STRING];
	qboolean	muted=qfalse;

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ));

	if ( G_shrubbot_mute_check( userinfo ) || ent->client->sess.muted )
		muted = qtrue;

	// Normal checks, if its not being issued as a referee command
	// CHRUKER: b067 - Was using the cpm command, but these needs to
	//          be displayed immediately.
	if( !fRefCommand ) {
		// Jaybird - added mute check
		if (muted) {
			CP("cp \"You cannot call a vote while muted.\"");
			return qfalse;
		} else if( level.voteInfo.voteTime ) {
			CP("cp \"A vote is already in progress.\"");
			return qfalse;
		} else if( level.intermissiontime ) {
			CP("cp \"Cannot callvote during intermission.\"");
			return qfalse;
		} else if( !ent->client->sess.referee ) {
			sbfNoVoteLimit = G_shrubbot_permission(ent,
					SBF_NO_VOTE_LIMIT);
			if( voteFlags.integer == VOTING_DISABLED && !sbfNoVoteLimit) {
				CP("cp \"Voting not enabled on this server.\"");
				return qfalse;
			} else if( vote_limit.integer > 0 && ent->client->pers.voteCount >= vote_limit.integer && 
					!sbfNoVoteLimit ) {
				CP(va("cp \"You have already called the maximum number of votes (%d).\"", vote_limit.integer));
				return qfalse;
			} else if( ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
				CP("cp \"Not allowed to call a vote as a spectator.\"");
				return qfalse;
			}
		}
	}

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if(strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
		char *strCmdBase = (!fRefCommand) ? "vote" : "ref command";

		G_refPrintf(ent, "Invalid %s string.", strCmdBase);
		return(qfalse);
	}


	if(trap_Argc() > 1 && (i = G_voteCmdCheck(ent, arg1, arg2, fRefCommand)) != G_NOTFOUND) {	//  --OSP
		if(i != G_OK)	 {
			if(i == G_NOTFOUND) return(qfalse);	// Command error
			else return(qtrue);
		}
	} else {
		if(!fRefCommand) {
			CP(va("print \"\n^3>>> Unknown vote command: ^7%s %s\n\"", arg1, arg2));
			G_voteHelp(ent, qtrue);
		}
		return(qfalse);
	}

	Com_sprintf(level.voteInfo.voteString, sizeof(level.voteInfo.voteString), "%s %s", arg1, arg2);

	// start the voting, the caller automatically votes yes
	// If a referee, vote automatically passes.	// OSP
	if( fRefCommand ) {
//		level.voteInfo.voteYes = level.voteInfo.numVotingClients + 10;	// JIC :)
		// Don't announce some votes, as in comp mode, it is generally a ref
		// who is policing people who shouldn't be joining and players don't want
		// this sort of spam in the console
		if(level.voteInfo.vote_fn != G_Kick_v && level.voteInfo.vote_fn != G_Mute_v) {
			AP("cp \"^1** Referee Server Setting Change **\n\"");
		}

		// Gordon: just call the stupid thing.... don't bother with the voting faff
		level.voteInfo.vote_fn(NULL, 0, NULL, NULL, qfalse);

		G_globalSound("sound/misc/referee.wav");
	} else {
		// jaquboss do not automaticly vote yes in polls
		if ( level.voteInfo.vote_fn != G_Poll_v )
			level.voteInfo.voteYes = 1;
		else 
			level.voteInfo.voteYes = 0;

		AP(va("print \"[lof]%s^7 [lon]called a vote.[lof]  Voting for: %s\n\"", ent->client->pers.netname, level.voteInfo.voteString));
		level.voteInfo.voteCaller = ent->s.number;
		level.voteInfo.voteTeam = ent->client->sess.sessionTeam;
		AP(va("print \"[lof]%s^7 [lon]called a vote.\n\"", ent->client->pers.netname));
		AP(va("cp \"[lof]%s\n^7[lon]called a vote.\n\"", ent->client->pers.netname));
		G_globalSound("sound/misc/vote.wav");
	}

	level.voteInfo.voteTime = level.time;
	level.voteInfo.voteNo = 0;

	// Don't send the vote info if a ref initiates (as it will automatically pass)
	if(!fRefCommand) {
		for(i=0; i<level.numConnectedClients; i++) {
			level.clients[level.sortedClients[i]].ps.eFlags &= ~EF_VOTED;
		}

		ent->client->pers.voteCount++;

		// jaquboss allow to vote...
		if ( level.voteInfo.vote_fn != G_Poll_v )
		ent->client->ps.eFlags |= EF_VOTED;

		trap_SetConfigstring(CS_VOTE_YES,	 va("%i", level.voteInfo.voteYes));
		trap_SetConfigstring(CS_VOTE_NO,	 va("%i", level.voteInfo.voteNo));
		Q_strncpyz(voteDesc,
			level.voteInfo.voteString,
			sizeof(voteDesc));
		if((g_voting.integer & VOTEF_DISP_CALLER)) {
			Q_strcat(voteDesc, sizeof(voteDesc)," (called by ");
			Q_strcat(voteDesc,
				sizeof(voteDesc),
				ent->client->pers.netname);
			Q_strcat(voteDesc, sizeof(voteDesc),")");
		}
		trap_SetConfigstring(CS_VOTE_STRING, voteDesc);
		trap_SetConfigstring(CS_VOTE_TIME,	 va("%i", level.voteInfo.voteTime));
	}

	return(qtrue);
}

qboolean StringToFilter( const char *s, ipFilter_t *f );

qboolean G_FindFreeComplainIP( gclient_t* cl, ipFilter_t* ip ) {
	int i = 0;

	if( !g_ipcomplaintlimit.integer ) {
		return qtrue;
	}

	for( i = 0; i < MAX_COMPLAINTIPS && i < g_ipcomplaintlimit.integer; i++ ) {
		if( !cl->pers.complaintips[i].compare && !cl->pers.complaintips[i].mask ) {
			cl->pers.complaintips[i].compare = ip->compare;
			cl->pers.complaintips[i].mask = ip->mask;
			return qtrue;
		}
		if( ( cl->pers.complaintips[i].compare & cl->pers.complaintips[i].mask ) == ( ip->compare & ip->mask ) ) {
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent ) {
	char		msg[64];
	int			num;
	char		userinfo[MAX_INFO_STRING];
	qboolean	muted=qfalse;

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ));

	if ( G_shrubbot_mute_check( userinfo ) || ent->client->sess.muted )
		muted = qtrue;
	// DHM - Nerve :: Complaints supercede voting (and share command)
	if ( ent->client->pers.complaintEndTime > level.time && g_gamestate.integer == GS_PLAYING && g_complaintlimit.integer ) {

		gentity_t* other = &g_entities[ ent->client->pers.complaintClient ];
		gclient_t *cl = other->client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;
		if ( cl->pers.localClient ) {
			trap_SendServerCommand( ent-g_entities, "complaint -3" );
			return;
		}

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			// Increase their complaint counter
			cl->pers.complaints++;

			num = g_complaintlimit.integer - cl->pers.complaints;

			if( !cl->pers.localClient ) {
				
				const char* value;
				char userinfo[MAX_INFO_STRING];
				ipFilter_t ip;

				trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ) );
				value = Info_ValueForKey( userinfo, "ip" );

				StringToFilter( value, &ip );

				if( num <= 0 || !G_FindFreeComplainIP( cl, &ip ) ) {
					trap_DropClient( cl - level.clients, "kicked after too many complaints.", cl->sess.referee ? 0 : 300 );
					trap_SendServerCommand( ent-g_entities, "complaint -1" );
					return;
				}
			}

			trap_SendServerCommand( ent->client->pers.complaintClient, va("cpm \"^1Warning^7: Complaint filed against you by %s^7 You have Lost XP.\n\"", ent->client->pers.netname ) ); // ^*
			trap_SendServerCommand( ent-g_entities, "complaint -1" );
			
			AddScore( other, WOLF_FRIENDLY_PENALTY );

			G_LoseKillSkillPoints( other, ent->sound2to1, ent->sound1to2, ent->sound2to3 ? qtrue : qfalse );
		} else {
			trap_SendServerCommand( ent->client->pers.complaintClient, "cpm \"No complaint filed against you.\n\"" );
			trap_SendServerCommand( ent-g_entities, "complaint -2" );
		}

		// Reset this ent's complainEndTime so they can't send multiple complaints
		ent->client->pers.complaintEndTime = -1;
		ent->client->pers.complaintClient = -1;

		return;
	}
	// dhm

	if ( ent->client->pers.applicationEndTime > level.time ) {

		gclient_t *cl = g_entities[ ent->client->pers.applicationClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "application -4" );
			trap_SendServerCommand( ent->client->pers.applicationClient, "application -3" );

			G_AddClientToFireteam( ent->client->pers.applicationClient, ent-g_entities );
		}
		else {
			trap_SendServerCommand( ent-g_entities, "application -4" );
			trap_SendServerCommand( ent->client->pers.applicationClient, "application -2" );
		}

		ent->client->pers.applicationEndTime = 0;
		ent->client->pers.applicationClient = -1;

		return;
	}

	ent->client->pers.applicationEndTime = 0;
	ent->client->pers.applicationClient = -1;

	if ( ent->client->pers.invitationEndTime > level.time ) {

		gclient_t *cl = g_entities[ ent->client->pers.invitationClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "invitation -4" );
			trap_SendServerCommand( ent->client->pers.invitationClient, "invitation -3" );

			G_AddClientToFireteam( ent-g_entities, ent->client->pers.invitationClient );
		} else {
			trap_SendServerCommand( ent-g_entities, "invitation -4" );
			trap_SendServerCommand( ent->client->pers.invitationClient, "invitation -2" );
		}

		ent->client->pers.invitationEndTime = 0;
		ent->client->pers.invitationClient = -1;

		return;
	}

	ent->client->pers.invitationEndTime = 0;
	ent->client->pers.invitationClient = -1;

	if( ent->client->pers.propositionEndTime > level.time ) {
		gclient_t *cl = g_entities[ ent->client->pers.propositionClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "proposition -4" );
			trap_SendServerCommand( ent->client->pers.propositionClient2, "proposition -3" );

			G_InviteToFireTeam( ent-g_entities, ent->client->pers.propositionClient );
		} else {
			trap_SendServerCommand( ent-g_entities, "proposition -4" );
			trap_SendServerCommand( ent->client->pers.propositionClient2, "proposition -2" );
		}

		ent->client->pers.propositionEndTime = 0;
		ent->client->pers.propositionClient = -1;
		ent->client->pers.propositionClient2 = -1;

		return;
	}

	if( ent->client->pers.autofireteamEndTime > level.time ) {
		fireteamData_t* ft;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "aft -2" );

			if( G_IsFireteamLeader( ent-g_entities, &ft ) ) {
				ft->priv = qtrue;
			}
		} else {
			trap_SendServerCommand( ent-g_entities, "aft -2" );
		}

		ent->client->pers.autofireteamEndTime = 0;

		return;
	}

	if( ent->client->pers.autofireteamCreateEndTime > level.time ) {
		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "aftc -2" );

			G_RegisterFireteam( ent-g_entities );
		} else {
			trap_SendServerCommand( ent-g_entities, "aftc -2" );
		}

		ent->client->pers.autofireteamCreateEndTime = 0;

		return;
	}

	if( ent->client->pers.autofireteamJoinEndTime > level.time ) {
		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			fireteamData_t* ft;

			trap_SendServerCommand( ent-g_entities, "aftj -2" );


			ft = G_FindFreePublicFireteam( ent->client->sess.sessionTeam );
			if( ft ) {
				G_AddClientToFireteam( ent-g_entities, ft->joinOrder[0] );
			}
		} else {
			trap_SendServerCommand( ent-g_entities, "aftj -2" );
		}

		ent->client->pers.autofireteamCreateEndTime = 0;

		return;
	}

	ent->client->pers.propositionEndTime = 0;
	ent->client->pers.propositionClient = -1;
	ent->client->pers.propositionClient2 = -1;

	// dhm
	// Reset this ent's complainEndTime so they can't send multiple complaints
	ent->client->pers.complaintEndTime = -1;
	ent->client->pers.complaintClient = -1;

	if ( !level.voteInfo.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
		return;
	}
	if ( ent->client->ps.eFlags & EF_VOTED ) {
		trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
		return;
	}

	if ( muted ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote when muted.\n\"" );
		return;
	}

	if( level.voteInfo.vote_fn == G_Kick_v ) {
		int pid = atoi( level.voteInfo.vote_value );
		if( !g_entities[ pid ].client ) {
			return;
		}

		if( g_entities[ pid ].client->sess.sessionTeam != TEAM_SPECTATOR && ent->client->sess.sessionTeam != g_entities[ pid ].client->sess.sessionTeam ) {
			trap_SendServerCommand( ent - g_entities, "print \"Cannot vote to kick player on opposing team.\n\"" );
			return;
		}
	}
	else if(level.voteInfo.vote_fn == G_Surrender_v) {
		if(ent->client->sess.sessionTeam != level.voteInfo.voteTeam) {
			CP("cp \"You cannot vote on the other team's surrender\"");
			return;
		}
	}

	trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
		level.voteInfo.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteInfo.voteYes ) );
	} else {
		level.voteInfo.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteInfo.voteNo ) );	
	}

	// a majority will be determined in G_CheckVote, which will also account
	// for players entering or leaving
}


qboolean G_canPickupMelee(gentity_t *ent) {
// JPW NERVE -- no "melee" weapons in net play
		return qfalse;
}
// jpw




/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) {
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return;
	}
	if ( trap_Argc() != 5 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles );
}

/*
=================
Cmd_StartCamera_f
=================
*/
void Cmd_StartCamera_f( gentity_t *ent ) {

	if( ent->client->cameraPortal  )
	{
		G_FreeEntity( ent->client->cameraPortal );
	}

	ent->client->cameraPortal						= G_Spawn();
	ent->client->cameraPortal->s.eType				= ET_CAMERA;
	ent->client->cameraPortal->s.apos.trType 		= TR_STATIONARY;
	ent->client->cameraPortal->s.apos.trTime 		= 0;
	ent->client->cameraPortal->s.apos.trDuration	= 0;

	VectorClear( ent->client->cameraPortal->s.angles );
	VectorClear( ent->client->cameraPortal->s.apos.trDelta );
	G_SetOrigin( ent->client->cameraPortal, ent->r.currentOrigin );
	VectorCopy( ent->r.currentOrigin, ent->client->cameraPortal->s.origin2 );

	ent->client->cameraPortal->s.frame				= 0;

	ent->client->cameraPortal->r.svFlags		   |= (SVF_PORTAL|SVF_SINGLECLIENT);
	ent->client->cameraPortal->r.singleClient		= ent->client->ps.clientNum;
	
	ent->client->ps.eFlags						   |= EF_VIEWING_CAMERA;
	ent->s.eFlags								   |= EF_VIEWING_CAMERA;

	VectorCopy( ent->r.currentOrigin, ent->client->cameraOrigin );	// backup our origin

// (SA) trying this in client to avoid 1 frame of player drawing
//	ent->client->ps.eFlags |= EF_NODRAW;
//	ent->s.eFlags |= EF_NODRAW;
}

/*
=================
Cmd_StopCamera_f
=================
*/
void Cmd_StopCamera_f( gentity_t *ent ) {
//	gentity_t *sp;

	if( ent->client->cameraPortal && (ent->client->ps.eFlags & EF_VIEWING_CAMERA) ) {
		// send a script event
//		G_Script_ScriptEvent( ent->client->cameraPortal, "stopcam", "" );

		// go back into noclient mode
		G_FreeEntity( ent->client->cameraPortal );
		ent->client->cameraPortal = NULL;

		ent->s.eFlags &= ~EF_VIEWING_CAMERA;
		ent->client->ps.eFlags &= ~EF_VIEWING_CAMERA;

		//G_SetOrigin( ent, ent->client->cameraOrigin );	// restore our origin
		//VectorCopy( ent->client->cameraOrigin, ent->client->ps.origin );

// (SA) trying this in client to avoid 1 frame of player drawing
//		ent->s.eFlags &= ~EF_NODRAW;
//		ent->client->ps.eFlags &= ~EF_NODRAW;

		// RF, if we are near the spawn point, save the "current" game, for reloading after death
//		sp = NULL;
    // gcc: suggests () around assignment used as truth value
//		while ((sp = G_Find( sp, FOFS(classname), "info_player_deathmatch" ))) {	// info_player_start becomes info_player_deathmatch in it's spawn functon
//			if (Distance( ent->s.pos.trBase, sp->s.origin ) < 256 && trap_InPVS( ent->s.pos.trBase, sp->s.origin )) {
//				G_SaveGame( NULL );
//				break;
//			}
//		}
	}
}

/*
=================
Cmd_SetCameraOrigin_f
=================
*/
void Cmd_SetCameraOrigin_f( gentity_t *ent ) {
	char		buffer[MAX_TOKEN_CHARS];
	int i;
	vec3_t		origin;

	if ( trap_Argc() != 4 ) {
		return;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	if( ent->client->cameraPortal ) {
		//G_SetOrigin( ent->client->cameraPortal, origin );	// set our origin
		VectorCopy( origin, ent->client->cameraPortal->s.origin2 );
		trap_LinkEntity( ent->client->cameraPortal );
	//	G_SetOrigin( ent, origin );	// set our origin
	//	VectorCopy( origin, ent->client->ps.origin );
	}
}

extern gentity_t *BotFindEntityForName( char *name );

/*
==============
Cmd_InterruptCamera_f
==============
*/
void Cmd_InterruptCamera_f( gentity_t *ent ) {
	gentity_t *player;

	if( g_gametype.integer != GT_SINGLE_PLAYER && g_gametype.integer != GT_COOP )
		return;

	player = BotFindEntityForName( "player" );

	if( !player )
		return;

	G_Script_ScriptEvent( player, "trigger", "cameraInterrupt" );
}

extern vec3_t playerMins;
extern vec3_t playerMaxs;

qboolean G_TankIsOccupied( gentity_t* ent ) {
	if( !ent->tankLink ) {
		return qfalse;
	}

	return qtrue;
}

qboolean G_TankIsMountable( gentity_t* ent, gentity_t* other ) {
	if( !(ent->spawnflags & 128) ) {
		return qfalse;
	}

	if( level.disableTankEnter ) {
		return qfalse;
	}

	//KW: bugfix: you can't get in a tank while scoped
	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( G_TankIsOccupied( ent ) ) {
		return qfalse;
	}

	if( ent->health <= 0 ) {
		return qfalse;
	}

	if( other->client->ps.weaponDelay ) {
		return qfalse;
	}

	return qtrue;
}

// Rafael
/*
==================
G_UniformSteal formerly known as Cmd_Activate2_f
==================
*/
qboolean G_UniformSteal(gentity_t *ent, gentity_t *traceEnt) 
{
//	qboolean found = qfalse;
	// already used corpse
	if(traceEnt->activator)
		return qfalse;
	if(ent->client->sess.playerType != PC_COVERTOPS)
		return qfalse;
	// CHRUKER: b006 - Only steal if we are alive
	if(ent->health <= 0) 
		return qfalse;
	if(ent->client->ps.powerups[PW_OPS_DISGUISED])
		return qfalse;
	if(ent->client->ps.powerups[PW_BLUEFLAG] || 
		ent->client->ps.powerups[PW_REDFLAG])
		return qfalse;
	if(BODY_TEAM(traceEnt) == ent->client->sess.sessionTeam)
		return qfalse;
	if( BODY_VALUE(traceEnt) < 250 ) {
		BODY_VALUE(traceEnt) += 5; // @SV_FPS
		return qfalse;
	}

	traceEnt->nextthink = traceEnt->timestamp + BODY_TIME(BODY_TEAM(traceEnt));

	//BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_PICKUPGRENADE, qfalse, qtrue );
	//ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
	//ent->client->ps.pm_time = 2100;

	ent->client->ps.powerups[PW_OPS_DISGUISED] = 1;
	ent->client->ps.powerups[PW_OPS_CLASS_1] = BODY_CLASS(traceEnt) & 1;
	ent->client->ps.powerups[PW_OPS_CLASS_2] = BODY_CLASS(traceEnt) & 2;
	ent->client->ps.powerups[PW_OPS_CLASS_3] = BODY_CLASS(traceEnt) & 4;

	BODY_TEAM(traceEnt) += 4;
	traceEnt->activator = ent;

	traceEnt->s.time2 =	1;

	// sound effect
	G_AddEvent( ent, EV_DISGUISE_SOUND, 0 );

	G_AddSkillPoints( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 5.f );
	G_DEBUG_ADD_SKILL_POINTS( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 5.f, "stealing uniform" ); 

	Q_strncpyz(ent->client->disguiseNetname,
		g_entities[traceEnt->s.clientNum].client->pers.netname,
		sizeof(ent->client->disguiseNetname));

	ent->client->disguiseClient = traceEnt->s.clientNum;

	ent->client->disguiseRank = g_entities[traceEnt->s.clientNum].client ? 
		g_entities[traceEnt->s.clientNum].client->sess.rank : 0;

	ClientUserinfoChanged( ent->s.clientNum );


	return qtrue;
}

/* 
  G_ClassStealFixWeapons() //tjw
  a helper for handling weapons for G_ClassSteal
  note by jaquboss - this function is mess and should be removed
  */
void G_ClassStealFixWeapons(gclient_t *client) 
{
	// tools
	COM_BitClear(client->ps.weapons, WP_DYNAMITE);
	COM_BitClear(client->ps.weapons, WP_PLIERS);
	COM_BitClear(client->ps.weapons, WP_LANDMINE);
	COM_BitClear(client->ps.weapons, WP_SMOKE_BOMB);
	COM_BitClear(client->ps.weapons, WP_SATCHEL);
	COM_BitClear(client->ps.weapons, WP_SATCHEL_DET);
	COM_BitClear(client->ps.weapons, WP_SMOKE_MARKER);
	COM_BitClear(client->ps.weapons, WP_AMMO);
	COM_BitClear(client->ps.weapons, WP_MEDKIT);
	COM_BitClear(client->ps.weapons, WP_MEDIC_SYRINGE);
	COM_BitClear(client->ps.weapons, WP_POISON_SYRINGE);
	COM_BitClear(client->ps.weapons, WP_MEDIC_ADRENALINE);

	// primary weapons
	COM_BitClear(client->ps.weapons, WP_PANZERFAUST);
	COM_BitClear(client->ps.weapons, WP_FLAMETHROWER);
	
	COM_BitClear(client->ps.weapons, WP_MOBILE_MG42);
	COM_BitClear(client->ps.weapons, WP_MOBILE_MG42_SET);
	COM_BitClear(client->ps.weapons, WP_MORTAR);
	COM_BitClear(client->ps.weapons, WP_MORTAR_SET);
	COM_BitClear(client->ps.weapons, WP_MORTAR2);
	COM_BitClear(client->ps.weapons, WP_MORTAR2_SET);
	COM_BitClear(client->ps.weapons, WP_FG42);
	COM_BitClear(client->ps.weapons, WP_FG42_SCOPE);
	COM_BitClear(client->ps.weapons, WP_K43);
	COM_BitClear(client->ps.weapons, WP_GARAND);
	COM_BitClear(client->ps.weapons, WP_K43_SCOPE);
	COM_BitClear(client->ps.weapons, WP_GARAND_SCOPE);
	COM_BitClear(client->ps.weapons, WP_STEN);
	COM_BitClear(client->ps.weapons, WP_MP40);
	COM_BitClear(client->ps.weapons, WP_THOMPSON);
	COM_BitClear(client->ps.weapons, WP_KAR98);
	COM_BitClear(client->ps.weapons, WP_GPG40);
	COM_BitClear(client->ps.weapons, WP_CARBINE);
	COM_BitClear(client->ps.weapons, WP_M7);

	COM_BitClear(client->ps.weapons, WP_MOBILE_BROWNING);
	COM_BitClear(client->ps.weapons, WP_MOBILE_BROWNING_SET);
	COM_BitClear(client->ps.weapons, WP_SHOTGUN);
	COM_BitClear(client->ps.weapons, WP_STEN_MKII);
	COM_BitClear(client->ps.weapons, WP_STG44);
	COM_BitClear(client->ps.weapons, WP_BAR);
	COM_BitClear(client->ps.weapons, WP_BAR_SET);
	COM_BitClear(client->ps.weapons, WP_BAZOOKA);
	COM_BitClear(client->ps.weapons, WP_MP34);

	COM_BitClear(client->ps.weapons, WP_VENOM);


	
	// silence pistols are different than regular pistols so swap them
	if(client->sess.playerType == PC_COVERTOPS) 
	{
		client->pmext.silencedSideArm = 1;
		COM_BitSet(client->ps.weapons, WP_SILENCER);
		COM_BitSet(client->ps.weapons, WP_SILENCED_COLT);
		
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_LUGER)) 
		{
			COM_BitClear(client->ps.weapons, WP_AKIMBO_LUGER);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER);
		}
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_COLT)) 
		{
			COM_BitClear(client->ps.weapons, WP_AKIMBO_COLT);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT);
		}
	}
	else 
	{
		client->pmext.silencedSideArm = 0;

		COM_BitClear(client->ps.weapons, WP_SILENCER);
		COM_BitClear(client->ps.weapons, WP_SILENCED_COLT);

		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER)) 
		{
			COM_BitClear(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_LUGER);
		}
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT)) 
		{
			COM_BitClear(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_COLT);
		}
	}

	// give them all the tool-like weapons for the class
	G_AddClassSpecificTools(client);
}


/*
 G_ClassSteal //tjw
 */
// this all sucks
qboolean G_ClassSteal(gentity_t *stealer, gentity_t *deadguy) 
{
	if(!g_classChange.integer) return qfalse;
	if(deadguy->activator) return qfalse; // already used corpse
	if( BODY_VALUE(deadguy) < 250 ) {
		if(BODY_VALUE(deadguy) == 0) {
			CPx(stealer->s.number, "cp \"Switching Classes\" 1");
		}
		BODY_VALUE(deadguy) += 5; // @SV_FPS
		return qtrue;
	}

	deadguy->nextthink = deadguy->timestamp + BODY_TIME(BODY_TEAM(deadguy));

	deadguy->activator = stealer;
	deadguy->s.time2 =	1;

	// sound effect
	G_AddEvent(stealer, EV_DISGUISE_SOUND, 0);

	// I guess this disables future use of the corpse 
	BODY_TEAM(deadguy) += 4;

	stealer->client->sess.playerType = BODY_CLASS(deadguy);


	G_ClassStealFixWeapons(stealer->client);

	// team_max[weapon] and g_heavyWeaponRestriction checks
	if(!G_IsWeaponDisabled(stealer, BODY_WEAPON(deadguy), qtrue)) 
	{
		COM_BitSet(stealer->client->ps.weapons, BODY_WEAPON(deadguy));
		stealer->client->ps.weapon = BODY_WEAPON(deadguy);
	}
	else if(BODY_CLASS(deadguy) == PC_COVERTOPS) 
	{		
		if (BODY_TEAM(deadguy) == TEAM_AXIS)
		{
			COM_BitSet(stealer->client->ps.weapons, WP_STEN_MKII);
			stealer->client->ps.weapon = WP_STEN_MKII;
		}
		else
		{
			COM_BitSet(stealer->client->ps.weapons, WP_STEN);
			stealer->client->ps.weapon = WP_STEN;
		}
	}
	else if(BODY_TEAM(deadguy) == TEAM_AXIS) 
	{
		COM_BitSet(stealer->client->ps.weapons, WP_MP40);
		stealer->client->ps.weapon = WP_MP40;
	}
	else 
	{
		COM_BitSet(stealer->client->ps.weapons, WP_THOMPSON);
		stealer->client->ps.weapon = WP_THOMPSON;
	}

	// if they took a gren launching weapon, they can have the
	// grenade launcher too I guess

	// jet Pilot - redo this, looks like wrong weapons chosen
	if(COM_BitCheck(stealer->client->ps.weapons, WP_GARAND))		// WP_GARAND
	{	
		COM_BitSet(stealer->client->ps.weapons, WP_GARAND_SCOPE);	// WP_CARBINE
	}
	else if(COM_BitCheck(stealer->client->ps.weapons, WP_K43))		// WP_K43
	{
		COM_BitSet(stealer->client->ps.weapons, WP_K43_SCOPE);		// WP_GPG40
	}
	else if(COM_BitCheck(stealer->client->ps.weapons, WP_FG42))		
	{
		COM_BitSet(stealer->client->ps.weapons, WP_FG42_SCOPE);		
	}
	else if(COM_BitCheck(stealer->client->ps.weapons, WP_CARBINE))	// WP_GARAND
	{	
		COM_BitSet(stealer->client->ps.weapons, WP_M7);				// WP_CARBINE
	}
	else if(COM_BitCheck(stealer->client->ps.weapons, WP_KAR98))	// WP_K43
	{
		COM_BitSet(stealer->client->ps.weapons, WP_GPG40);			// WP_GPG40
	}

	// make sure they lose disguise when changing classes
	stealer->client->ps.powerups[PW_OPS_DISGUISED] = 0;

	ClientUserinfoChanged(stealer->s.clientNum);

	return qtrue;
}

qboolean G_PushPlayer(gentity_t *ent, gentity_t *victim) 
{
	vec3_t	dir, push;

	if(!g_shove.integer)
		return qfalse;

	if(ent->health <= 0)
		return qfalse;

	if((level.time - ent->client->pmext.shoveTime) < 500) {
		return qfalse;
	}

	ent->client->pmext.shoveTime = level.time;

	VectorSubtract(victim->r.currentOrigin, ent->r.currentOrigin, dir);

	VectorNormalizeFast(dir);

	VectorScale(dir, (g_shove.value * 5.0f), push);

	if((fabs(push[2]) > fabs(push[0])) &&
		(fabs(push[2]) > fabs(push[1]))) {

		// player is being boosted
		if(g_shoveNoZ.integer)
			return qfalse;
		
		push[2] = g_shove.value * 3;
        if (push[2] < 350)
            push[2] = 350;
		push[0] = push[1] = 0;
	}
	else {
		// give them a little hop
		push[2] = 24;
	}
			
	VectorAdd(victim->s.pos.trDelta, push, victim->s.pos.trDelta);
	VectorAdd(victim->client->ps.velocity, push,
		victim->client->ps.velocity);

	victim->client->pmext.shoved = qtrue; // jaquboss - are we pushed?, guess i don't need that in pmove..
	victim->client->pmext.pusher = ent - g_entities;

	if(g_shoveSound.string != '\0') {
		G_AddEvent(victim, EV_GENERAL_SOUND,
			G_SoundIndex(g_shoveSound.string));
	}
	return qtrue;
}


/* 
 G_DragCorpse //tjw
 */
qboolean G_DragCorpse(gentity_t *dragger, gentity_t *corpse) 
{
	vec3_t	dir, pull, res;
	float dist;

	if(!g_dragCorpse.integer) return qfalse;
	VectorSubtract(dragger->r.currentOrigin, corpse->r.currentOrigin, dir);
	dir[2] = 0;
	dist = VectorNormalize(dir);

	// don't pull corpses past the dragger's head and don't start dragging
	// until both players look like they're in contact
	if(dist > 85 || dist < 40) return qfalse;

	VectorScale(dir, 110, pull);

	// prevent some zipping around when the corpse doesn't have 
	// much friction
	VectorSubtract(pull, corpse->client->ps.velocity, res);

	// no dragging into the ground
	res[2] = 0;

	VectorAdd(corpse->s.pos.trDelta, res, corpse->s.pos.trDelta );
	VectorAdd(corpse->client->ps.velocity, res, corpse->client->ps.velocity);

	return qtrue; // no checks yet
}
/*

Revive Guy

*/

extern void ReviveEntity(gentity_t *ent, gentity_t *traceEnt, qboolean resuscitation);

static qboolean G_MedicActivate(gentity_t *ent, gentity_t *traceEnt) {


	if( ent->client->ps.groundEntityNum == ENTITYNUM_NONE || ent->client->ps.eFlags & EF_PRONE || ent->waterlevel == 3 )
	{
		return qtrue; // dont try anything else
	}

	// do not care about nonclients
	if ( !traceEnt->client )
		return qfalse;

	// don't try this
	if (traceEnt->client->ps.eFlags & EF_PLAYDEAD)
		return qfalse;

	if ( ent->health <= 0) // uch
		return qfalse;

	if ( traceEnt->health <= 0 ){ 

		traceEnt->reviveTime = level.time + 500; // be long first time

		if ( traceEnt->reviveVal < 250 ){ // need to revive
			traceEnt->reviveVal += REVIVE_HEALRATE; // @SV_FPS
		} else { // done!

			ReviveEntity( ent, traceEnt, qtrue );

			// from syringe code
			if ( !traceEnt->isProp )
			{
				if ( traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam )
				{
					AddScore( ent, WOLF_MEDIC_BONUS );	// JPW NERVE props to the medic for the swift and dexterous bit o healitude
					G_AddSkillPoints( ent, SK_FIRST_AID, 4.f );
					G_DEBUG_ADD_SKILL_POINTS( ent, SK_FIRST_AID, 4.f, "reviving a player" );
				}
			}

			// Arnout: calculate ranks to update numFinalDead arrays. Have to do it manually as addscore has an early out
			if ( g_gametype.integer == GT_WOLF_LMS )
			{
				CalculateRanks();
			}

		}

	}

	// flag me!
	ent->client->ps.pm_flags |= PMF_MEDICUSE;
	ent->client->ps.pm_time = 500;


	return qtrue;
}


// TAT 1/14/2003 - extracted out the functionality of Cmd_Activate_f from finding the object to use
//		so we can force bots to use items, without worrying that they are looking EXACTLY at the target
qboolean Do_Activate_f(gentity_t *ent, gentity_t *traceEnt) 
{
	qboolean found		= qfalse;
	qboolean walking	= qfalse;
	vec3_t	 forward;	//, offset, end;
	//trace_t		tr;

	// Arnout: invisible entities can't be used
	if( traceEnt->entstate == STATE_INVISIBLE || traceEnt->entstate == STATE_UNDERCONSTRUCTION ) 
	{
		return qfalse;
	}

	if((ent->client->pers.cmd.buttons & BUTTON_WALKING) || (ent->client->ps.pm_flags & PMF_DUCKED)) 
	{
		walking = qtrue;
	}

	if (traceEnt->classname)
	{
		traceEnt->flags &= ~FL_SOFTACTIVATE;	// FL_SOFTACTIVATE will be set if the user is holding 'walk' key

		if (traceEnt->s.eType == ET_ALARMBOX)
		{
			trace_t		trace;

			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
			{
				return qfalse;
			}
			
			memset( &trace, 0, sizeof(trace) );

			if(traceEnt->use)
			{
				G_UseEntity( traceEnt, ent, 0 );
			}
			found = qtrue;
		}
		else if (traceEnt->s.eType == ET_ITEM)
		{
			trace_t		trace;

			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
			{
				return qfalse;
			}
			
			memset( &trace, 0, sizeof(trace) );

			if( traceEnt->touch ) 
			{
				if( ent->client->pers.autoActivate == PICKUP_ACTIVATE )
				{
					ent->client->pers.autoActivate = PICKUP_FORCE;		//----(SA) force pickup
				}
				traceEnt->active = qtrue;
				traceEnt->touch( traceEnt, ent, &trace );
			}

			found = qtrue;
		} 
		else if ( traceEnt->s.eType == ET_MOVER && G_TankIsMountable( traceEnt, ent ) ) 
		{
			G_Script_ScriptEvent( traceEnt, "mg42", "mount" );
			ent->tagParent = traceEnt->nextTrain;
			Q_strncpyz( ent->tagName, "tag_player", MAX_QPATH );
			ent->backupWeaponTime = ent->client->ps.weaponTime;
			ent->client->ps.weaponTime = traceEnt->backupWeaponTime;
			ent->client->ps.weapHeat[WP_DUMMY_MG42] = traceEnt->mg42weapHeat;
			
			ent->tankLink = traceEnt;
			traceEnt->tankLink = ent;

			G_ProcessTagConnect( ent, qtrue );
			found = qtrue;
		} 
		else if( G_EmplacedGunIsMountable( traceEnt, ent ) ) 
		{
			gclient_t* cl = &level.clients[ ent->s.clientNum ];
			vec3_t	point;

			AngleVectors (traceEnt->s.apos.trBase, forward, NULL, NULL);
			VectorMA (traceEnt->r.currentOrigin, -36, forward, point);
			point[2] = ent->r.currentOrigin[2];

			// Save initial position
			VectorCopy( point, ent->TargetAngles );

			// Zero out velocity
			VectorCopy( vec3_origin, ent->client->ps.velocity );
			VectorCopy( vec3_origin, ent->s.pos.trDelta );

			traceEnt->active = qtrue;
			ent->active = qtrue;
			traceEnt->r.ownerNum = ent->s.number;
			VectorCopy (traceEnt->s.angles, traceEnt->TargetAngles);
			traceEnt->s.otherEntityNum = ent->s.number;

			cl->pmext.harc = traceEnt->harc;
			cl->pmext.varc = traceEnt->varc;
			VectorCopy( traceEnt->s.angles, cl->pmext.centerangles );
			cl->pmext.centerangles[PITCH] = AngleNormalize180( cl->pmext.centerangles[PITCH] );
			cl->pmext.centerangles[YAW] = AngleNormalize180( cl->pmext.centerangles[YAW] );
			cl->pmext.centerangles[ROLL] = AngleNormalize180( cl->pmext.centerangles[ROLL] );

			ent->backupWeaponTime = ent->client->ps.weaponTime;
			ent->client->ps.weaponTime = traceEnt->backupWeaponTime;
			ent->client->ps.weapHeat[WP_DUMMY_MG42] = traceEnt->mg42weapHeat;

			G_UseTargets( traceEnt, ent);	//----(SA)	added for Mike so mounting an MG42 can be a trigger event (let me know if there's any issues with this)
			found = qtrue;
		}
		else if ( ( (Q_stricmp (traceEnt->classname, "func_door") == 0) || (Q_stricmp (traceEnt->classname, "func_door_rotating") == 0) ) ) 
		{
			// jet Pilot - do crouching softactivate here
			if( walking ) 
			{
				traceEnt->flags |= FL_SOFTACTIVATE;		// no noise
			}
			G_TryDoor(traceEnt, ent, ent);		// (door,other,activator)
			found = qtrue;
		}
		else if ( (Q_stricmp (traceEnt->classname, "team_WOLF_checkpoint") == 0) ) 
		{
			if( traceEnt->count != ent->client->sess.sessionTeam ) {
				traceEnt->health++;
			}
			found = qtrue;
		} else if ( (Q_stricmp (traceEnt->classname, "func_button") == 0) && ( traceEnt->s.apos.trType == TR_STATIONARY && traceEnt->s.pos.trType == TR_STATIONARY) && traceEnt->active == qfalse ) {
			Use_BinaryMover (traceEnt, ent, ent);
			traceEnt->active = qtrue;
			found = qtrue;
		} else if ( !Q_stricmp (traceEnt->classname, "func_invisible_user") ) {
			if( walking ) {
				traceEnt->flags |= FL_SOFTACTIVATE;		// no noise
			}
			G_UseEntity( traceEnt, ent, ent );
			found = qtrue;
		} else if ( !Q_stricmp (traceEnt->classname, "props_footlocker") ) {
			G_UseEntity( traceEnt, ent, ent );
			found = qtrue;
		}
	}

	return found;
}

void G_LeaveTank( gentity_t* ent, qboolean position ) {
	gentity_t* tank;

	// found our tank (or whatever)
	vec3_t axis[3];
	vec3_t pos;
	trace_t tr;

	tank = ent->tankLink;
	if( !tank ) {
		return;
	}

	if( position ) {

		AnglesToAxis( tank->s.angles, axis );

		VectorMA( ent->client->ps.origin, 128, axis[1], pos );
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
			// try right
			VectorMA( ent->client->ps.origin, -128, axis[1], pos );
			trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				// try back
				VectorMA( ent->client->ps.origin, -224, axis[0], pos );
				trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

				if( tr.startsolid ) {
					// try front
					VectorMA( ent->client->ps.origin, 224, axis[0], pos );
					trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

					if( tr.startsolid ) {
						// give up
						return;
					}
				}
			}
		}

		VectorClear( ent->client->ps.velocity ); // Gordon: dont want them to fly away ;D
		TeleportPlayer( ent, pos, ent->client->ps.viewangles );
	}


	tank->mg42weapHeat = ent->client->ps.weapHeat[WP_DUMMY_MG42];
	tank->backupWeaponTime = ent->client->ps.weaponTime;
	ent->client->ps.weaponTime = ent->backupWeaponTime;

	// CHRUKER: b087 - Player always mounting the last gun used, on multiple tank maps
	G_RemoveConfigstringIndex( va("%i %i %s", ent->s.number, ent->tagParent->s.number, ent->tagName), CS_TAGCONNECTS, MAX_TAGCONNECTS );

	G_Script_ScriptEvent( tank, "mg42", "unmount" );
	ent->tagParent = NULL;
	*ent->tagName = '\0';
	ent->s.eFlags &= ~EF_MOUNTEDTANK;
	ent->client->ps.eFlags &= ~EF_MOUNTEDTANK;
	tank->s.powerups = -1;

	tank->tankLink = NULL;
	ent->tankLink = NULL;
}

void Cmd_Activate_f( gentity_t *ent )
{
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	vec3_t		forward, right, up, offset;
//	int			activatetime = level.time;
	qboolean	found = qfalse;
	qboolean	pass2 = qfalse;
	int			i;

	if( ent->health <= 0 )
	{
		return;
	}

	if( ent->s.weapon == WP_MORTAR2_SET || ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET || ent->s.weapon == WP_MOBILE_BROWNING_SET || ent->s.weapon == WP_BAR_SET)
	{
		return;
	}

// dont activate while in jet
#ifdef __JETPILOT__
	if ( jp_insanity.integer & JP_INSANITY_FLYING )
		return;
#endif

	if (ent->active)
	{
		if (ent->client->ps.persistant[PERS_HWEAPON_USE])
		{
			// DHM - Nerve :: Restore original position if current position is bad
			trap_Trace (&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->s.number, MASK_PLAYERSOLID);
			if ( tr.startsolid ) 
			{
				VectorCopy( ent->TargetAngles, ent->client->ps.origin );
				VectorCopy( ent->TargetAngles, ent->r.currentOrigin );
				ent->r.contents = CONTENTS_CORPSE;		// DHM - this will correct itself in ClientEndFrame
			}

			ent->client->ps.eFlags &= ~EF_MG42_ACTIVE;			// DHM - Nerve :: unset flag
			ent->client->ps.eFlags &= ~EF_AAGUN_ACTIVE;

			ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;
			ent->active = qfalse;
			
			for( i = 0; i < level.num_entities; i++ )
			{
				if( g_entities[i].s.eType == ET_MG42_BARREL && g_entities[i].r.ownerNum == ent->s.number )
				{
					g_entities[i].mg42weapHeat = ent->client->ps.weapHeat[WP_DUMMY_MG42];
					g_entities[i].backupWeaponTime = ent->client->ps.weaponTime;
					break;
				}
			}
			ent->client->ps.weaponTime = ent->backupWeaponTime;
		} 
		else 
		{
			ent->active = qfalse;
		}
		return;
	}
	else if( ent->client->ps.eFlags & EF_MOUNTEDTANK && ent->s.eFlags & EF_MOUNTEDTANK && !level.disableTankExit )
	{
		G_LeaveTank( ent, qtrue );
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, right, up);

	VectorCopy( ent->client->ps.origin, offset );
	offset[2] += ent->client->ps.viewheight;

	// lean
	if( ent->client->ps.leanf )
	{
		VectorMA( offset, ent->client->ps.leanf, right, offset );
	}

	//VectorMA( offset, 256, forward, end );
	VectorMA( offset, 96, forward, end );

	trap_Trace( &tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE)  );

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.entityNum == ENTITYNUM_WORLD)
	{
		trap_Trace (&tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_MISSILECLIP|CONTENTS_TRIGGER));
		pass2 = qtrue;
	}

tryagain:

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.entityNum == ENTITYNUM_WORLD) 
	{
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	found = Do_Activate_f(ent, traceEnt);

	if(!found && !pass2) 
	{
		pass2 = qtrue;
		trap_Trace (&tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_TRIGGER));
		goto tryagain;
	}
}

/*
Cmd_Activate2_f() is only for:
    uniform stealing
	shove 
	dragging
	class switch
 */
void Cmd_Activate2_f( gentity_t *ent )
{
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	vec3_t		forward, right, up, offset;
//	qboolean	found = qfalse;
//	qboolean	pass2 = qfalse;


	//if( ent->client->sess.playerType != PC_COVERTOPS ) {
	//	return;
	//}

	if ( ent->health <= 0) // uch
		return;

	if( ent->s.weapon == WP_MORTAR2_SET || ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET || ent->s.weapon == WP_MOBILE_BROWNING_SET || ent->s.weapon == WP_BAR_SET)
		return;

 // dont activate while in jet
#ifdef __JETPILOT__
	if ( jp_insanity.integer & JP_INSANITY_FLYING )
		return;
#endif
	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePointForActivate (ent, forward, right, up, offset, qfalse);
	VectorMA (offset, 96, forward, end);



	// look for a corpse to drag
	trap_Trace(&tr, 
		offset, 
		NULL, 
		NULL, 
		end, 
		ent->s.number, 
		CONTENTS_CORPSE);
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->client) {
			if ( ent->client->sess.sessionTeam == traceEnt->client->sess.sessionTeam && ent->client->sess.skill[SK_MEDIC] >= 9 )
			G_MedicActivate(ent, traceEnt);
				else
			G_DragCorpse(ent, traceEnt);
			return;
		}
	}

	// look for a guy to push
	trap_Trace(&tr, 
		offset, 
		NULL, 
		NULL, 
		end, 
		ent->s.number, 
		CONTENTS_BODY);
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->client) {
			if(traceEnt->client->ps.eFlags & EF_PLAYDEAD) 
				G_DragCorpse(ent, traceEnt);
			else
				G_PushPlayer(ent, traceEnt);
			return;
		}
	}

	// look for a gibbed corpse
	trap_Trace(&tr,
		offset,
		NULL,
		NULL,
		end,
		ent->s.number,
		(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE));
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->s.eType == ET_CORPSE && BODY_TEAM(traceEnt) && /*BODY_TEAM(traceEnt) < 4*/ traceEnt->s.time2 == 0 ) {
				if(BODY_TEAM(traceEnt) == ent->client->sess.sessionTeam && ent->client->sess.playerType != BODY_CLASS(traceEnt)) {
					G_ClassSteal(ent, traceEnt);
					return;
				}
				else if(ent->client->sess.playerType == PC_COVERTOPS)  {
					G_UniformSteal(ent, traceEnt);
					return;
				}
		}
	}

	if(g_dropObj.integer &&
		ent->client->pmext.objDrops <= g_dropObj.integer &&
		(ent->client->ps.powerups[PW_REDFLAG] ||
		 ent->client->ps.powerups[PW_BLUEFLAG])) {

		if(ent->client->ps.weapon != WP_KNIFE && ent->client->ps.weapon != WP_KNIFE_KABAR) {
			CP("cp \"You must switch to knife to drop objective\"");
		}
		else {
			G_DropItems(ent);
			ent->client->pmext.objDrops++;
		}
	}
	
	// forty - canister kicking 
	/*if(g_canisterKick.integer) 
			G_CanisterKick(ent);  */
	
} 

/*
============================
Cmd_ClientMonsterSlickAngle 
============================
*/
/*
void Cmd_ClientMonsterSlickAngle (gentity_t *clent) {

	char s[MAX_STRING_CHARS];
	int	entnum;
	int angle;
	gentity_t *ent;
	vec3_t	dir, kvel;
	vec3_t	forward;

	if (trap_Argc() != 3) {
		G_Printf( "ClientDamage command issued with incorrect number of args\n" );
	}

	trap_Argv( 1, s, sizeof( s ) );
	entnum = atoi(s);
	ent = &g_entities[entnum];

	trap_Argv( 2, s, sizeof( s ) );
	angle = atoi(s);

	// sanity check (also protect from cheaters)
	if (g_gametype.integer != GT_SINGLE_PLAYER && entnum != clent->s.number) {
		trap_DropClient( clent->s.number, "Dropped due to illegal ClientMonsterSlick command\n" );
		return;
	}

	VectorClear (dir);
	dir[YAW] = angle;
	AngleVectors (dir, forward, NULL, NULL);
	
	VectorScale (forward, 32, kvel);
	VectorAdd (ent->client->ps.velocity, kvel, ent->client->ps.velocity);
}
*/

void G_UpdateSpawnCounts( void ) {
	int		i, j;
	char	cs[MAX_STRING_CHARS];
	int		current, count, team;

	for( i = 0; i < level.numspawntargets; i++ ) {
		trap_GetConfigstring( CS_MULTI_SPAWNTARGETS + i, cs, sizeof(cs) );

		current = atoi(Info_ValueForKey( cs, "c" ));
		team = atoi(Info_ValueForKey( cs, "t" )) & ~256;

		count = 0;
		for( j = 0; j < level.numConnectedClients; j++ ) {
			gclient_t* client = &level.clients[ level.sortedClients[ j ] ];

			if( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES ) {
				continue;
			}

			if( client->sess.sessionTeam == team && client->sess.spawnObjectiveIndex == i + 1) {
				count++;
				continue;
			}

			if( client->sess.spawnObjectiveIndex == 0 ) {
				if( client->sess.sessionTeam == TEAM_AXIS ) {
					if( level.axisAutoSpawn == i ) {
						count++;
						continue;
					}
				} else {
					if( level.alliesAutoSpawn == i ) {
						count++;
						continue;
					}
				}
			}
		}

		if(count == current) {
			continue;
		}

		Info_SetValueForKey( cs, "c", va("%i", count));
		trap_SetConfigstring( CS_MULTI_SPAWNTARGETS + i, cs );
	}
}

//extern pmove_t *pm;


/*
============
Cmd_SetSpawnPoint_f
============
*/
void SetPlayerSpawn( gentity_t* ent, int spawn, qboolean update ) {
// jet Pilot - more logical than before
	if( spawn < MAX_MULTI_SPAWNTARGETS && spawn >= 0 )
	{
        ent->client->sess.spawnObjectiveIndex  = spawn;
	}
	else if ( spawn != -999 )
	{
		Com_Printf("^9Argument Exceeds Range : ^3setspawnpt ^9must be in the range of 0 and %d\n", MAX_MULTI_SPAWNTARGETS);

		ent->client->sess.spawnObjectiveIndex = 0;
	}			
	
	// Give feedback to the player
	if (ent->client->sess.spawnObjectiveIndex  == 0)
	{
		Com_Printf("^9Current spawn point : ^wDefault\n");
	}
	else
	{
		Com_Printf("^9Current spawn point : ^w%d\n", ent->client->sess.spawnObjectiveIndex );
	}			
	
	/*
	ent->client->sess.spawnObjectiveIndex = spawn;

	if( ent->client->sess.spawnObjectiveIndex >= MAX_MULTI_SPAWNTARGETS || ent->client->sess.spawnObjectiveIndex < 0 ) 
	{
		ent->client->sess.spawnObjectiveIndex = 0;
	}
	*/


	if( update ) 
	{
		G_UpdateSpawnCounts();
	}
}

void Cmd_SetSpawnPoint_f( gentity_t* ent ) {
	char arg[MAX_TOKEN_CHARS];
	int val, i;

	if ( trap_Argc() != 2 ) 
	{
		// jet Pilot - print current spawn point

		SetPlayerSpawn( ent, -999, qfalse );

		// end of changes

		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	val = atoi( arg );

	if( ent->client ) {
		
		SetPlayerSpawn( ent, val, qtrue);
	}

	


//	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
//		return;
//	}

	for( i = 0; i < level.numLimboCams; i++ ) {
		int x = (g_entities[level.limboCams[i].targetEnt].count - CS_MULTI_SPAWNTARGETS) + 1;
		if( level.limboCams[i].spawn && x == val ) {
			VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
			ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
			trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );
			break;
		}
	}
}

/*
============
Cmd_SetSniperSpot_f
============
*/
void Cmd_SetSniperSpot_f( gentity_t *clent ) {
	gentity_t *spot;

	vmCvar_t	cvar_mapname;
	char		filename[MAX_QPATH];
	fileHandle_t f;
	char		buf[1024];

	if (!g_cheats.integer) return;
	if (!trap_Cvar_VariableIntegerValue("cl_running")) return;	// only allow locally playing client
	if (clent->s.number != 0) return;	// only allow locally playing client

	// drop a sniper spot here
	spot = G_Spawn();
	spot->classname = "bot_sniper_spot";
	VectorCopy( clent->r.currentOrigin, spot->s.origin );
	VectorCopy( clent->client->ps.viewangles, spot->s.angles );
	spot->aiTeam = clent->client->sess.sessionTeam;

	// output to text file
	trap_Cvar_Register( &cvar_mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	Com_sprintf( filename, sizeof(filename), "maps/%s.botents", cvar_mapname.string );
	if (trap_FS_FOpenFile( filename, &f, FS_APPEND ) < 0) {
		G_Error("Cmd_SetSniperSpot_f: cannot open %s for writing", filename );
	}

	Com_sprintf( buf, sizeof(buf), "{\n\"classname\" \"%s\"\n\"origin\" \"%.3f %.3f %.3f\"\n\"angles\" \"%.2f %.2f %.2f\"\n\"aiTeam\" \"%i\"\n}\n\n", spot->classname, spot->s.origin[0], spot->s.origin[1], spot->s.origin[2], spot->s.angles[0], spot->s.angles[1], spot->s.angles[2], spot->aiTeam );
	trap_FS_Write( buf, strlen(buf), f );

	trap_FS_FCloseFile( f );

	G_Printf( "dropped sniper spot\n" );

	return;
}

void G_PrintAccuracyLog( gentity_t *ent );

/*
============
Cmd_SetWayPoint_f
============
*/
/*void Cmd_SetWayPoint_f( gentity_t *ent ) {
	char	arg[MAX_TOKEN_CHARS];
	vec3_t	forward, muzzlePoint, end, loc;
	trace_t	trace;

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to set waypoints as spectator.\n\"" );
		return;
	}

	if ( trap_Argc() != 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->r.currentOrigin, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	VectorMA( muzzlePoint, 8192, forward, end );
	trap_Trace( &trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if( trace.surfaceFlags & SURF_NOIMPACT )
		return;

	VectorCopy( trace.endpos, loc );

	G_SetWayPoint( ent, atoi(arg), loc );
}*/

/*
============
Cmd_ClearWayPoint_f
============
*/
/*void Cmd_ClearWayPoint_f( gentity_t *ent ) {

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to clear waypoints as spectator.\n\"" );
		return;
	}

	G_RemoveWayPoint( ent->client );
}*/

void Cmd_WeaponStat_f ( gentity_t* ent ) {
	char buffer[16];
	extWeaponStats_t stat;

	if(!ent || !ent->client) {
		return;
	}

	if( trap_Argc() != 2 ) {
		return;
	}
	trap_Argv( 1, buffer, 16 );
	stat = atoi(buffer);

	trap_SendServerCommand( ent-g_entities, va( "rws %i %i", ent->client->sess.aWeaponStats[stat].atts, ent->client->sess.aWeaponStats[stat].hits ) );
}

void Cmd_IntermissionWeaponStats_f ( gentity_t* ent ) {
	char buffer[1024];
	int i, clientNum;

	if( !ent || !ent->client ) {
		return;
	}

	trap_Argv( 1, buffer, sizeof( buffer ) );

	clientNum = atoi( buffer );
	if( clientNum < 0 || clientNum > MAX_CLIENTS ) {
		return;
	}

	Q_strncpyz( buffer, "imws ", sizeof( buffer ) );
	for( i = 0; i < WS_MAX; i++ ) {
		Q_strcat( buffer, sizeof( buffer ), va( "%i %i %i ", level.clients[clientNum].sess.aWeaponStats[i].atts, level.clients[clientNum].sess.aWeaponStats[i].hits, level.clients[clientNum].sess.aWeaponStats[i].kills ) );
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void G_MakeReady( gentity_t* ent ) {
	ent->client->ps.eFlags |= EF_READY;
	ent->s.eFlags |= EF_READY;
	// rain - #105 - moved this set here
	ent->client->pers.ready = qtrue;
}

void G_MakeUnready( gentity_t* ent ) {
	ent->client->ps.eFlags &= ~EF_READY;
	ent->s.eFlags &= ~EF_READY;
	// rain - #105 - moved this set here
	ent->client->pers.ready = qfalse;
}

void Cmd_IntermissionReady_f ( gentity_t* ent ) {
	if( !ent || !ent->client ) {
		return;
	}

	G_MakeReady( ent );
}

void Cmd_IntermissionPlayerKillsDeaths_f ( gentity_t* ent ) {
	char buffer[1024];
	int i;

	if( !ent || !ent->client ) {
		return;
	}

	Q_strncpyz( buffer, "impkd ", sizeof( buffer ) );
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		if( g_entities[i].inuse ) {
			Q_strcat( buffer, sizeof( buffer ), va( "%i %i ", level.clients[i].sess.kills, level.clients[i].sess.deaths ) );
		} else {
			Q_strcat( buffer, sizeof( buffer ), "0 0 " );
		}
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

// jet Pilot - updated to track headshots
void G_CalcClientAccuracies( void ) {
	int i, j;
	int shots, hits;
	int	headshots;																	// jP

	for( i = 0; i < MAX_CLIENTS; i++ ) {
		shots = 0;
		hits = 0;
		headshots = 0;

		if( g_entities[i].inuse ) {
			for( j = 0; j < WS_MAX; j++ ) {
				shots	  += level.clients[i].sess.aWeaponStats[j].atts;
				hits	  += level.clients[i].sess.aWeaponStats[j].hits;
				headshots += level.clients[i].sess.aWeaponStats[j].headshots;		// jP
			}

			level.clients[ i ].acc		 = shots ? (100 * hits) / (float)shots : 0;
			level.clients[ i ].headshots = headshots;								// jP
		} else {
			level.clients[ i ].acc		 = 0;
			level.clients[ i ].headshots = 0;										// jP
		}
	}	
}


void Cmd_IntermissionWeaponAccuracies_f ( gentity_t* ent ) {
	char buffer[1024];
	int i;

	if( !ent || !ent->client ) {
		return;
	}

	G_CalcClientAccuracies();

	Q_strncpyz( buffer, "imwa ", sizeof( buffer ) );
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		Q_strcat( buffer, sizeof( buffer ), va( "%i ", (int)level.clients[ i ].acc ) );
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void Cmd_SelectedObjective_f ( gentity_t* ent ) {
	int i, val;
	char buffer[16];
	vec_t dist, neardist = 0;
	int nearest = -1;


	if(!ent || !ent->client) {
		return;
	}

//	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
//		return;
//	}

	if( trap_Argc() != 2 ) {
		return;
	}
	trap_Argv( 1, buffer, 16 );
	val = atoi(buffer) + 1;


	for( i = 0; i < level.numLimboCams; i++ ) {
		if( !level.limboCams[i].spawn && level.limboCams[i].info == val ) {			
			if( !level.limboCams[i].hasEnt ) {
				VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
				ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
				trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );

				break;
			} else {
				dist = VectorDistanceSquared( level.limboCams[i].origin, g_entities[level.limboCams[i].targetEnt].r.currentOrigin );
				if( nearest == -1 || dist < neardist ) {
					nearest = i;
					neardist = dist;
				}
			}
		}
	}

	if( nearest != -1 ) {
		i = nearest;

		VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
		ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
		trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );
	}
}

void Cmd_Ignore_f (gentity_t* ent ) {
	char	cmd[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];
	char err[MAX_STRING_CHARS];
	gentity_t *victim;

	trap_Argv( 1, cmd, sizeof( cmd ) );		

	if(!*cmd) {
		CP("print \"usage: ignore [name|slot#]\n\"");
		return;
	}

	if(ClientNumbersFromString(cmd, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		CP(va("print \"ignore: %s\"", err));
		return;
	}

	if(pids[0] == (ent-g_entities)) {
		CP("print \"ignore: you can't ignore yourself\n\"");
		return;
	}

	victim = &g_entities[pids[0]];

	if(COM_BitCheck(ent->client->sess.ignoreClients, pids[0])) {
		CP(va("print \"ignore: you are already ignoring %s\n\"",
			victim->client->pers.netname));
		return;
	}

	COM_BitSet(ent->client->sess.ignoreClients, pids[0]);
	CP(va("print \"ignore: %s^7 added to your ignore list\n\"",
		victim->client->pers.netname));
	CPx(pids[0], va("chat \"%s^1 is now ignoring you\"",
		ent->client->pers.netname));

}

void Cmd_TicketTape_f ( void ) {
/*	char	cmd[MAX_TOKEN_CHARS];

	trap_Argv( 1, cmd, sizeof( cmd ) );

	trap_SendServerCommand( -1, va( "tt \"LANDMINES SPOTTED <STOP> CHECK COMMAND MAP FOR DETAILS <STOP>\"\n", cmd ));*/
}

void Cmd_UnIgnore_f (gentity_t* ent ) {
	char	cmd[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];
	int i;
	gentity_t *victim;
	int clientNum = 0;
	int matches;

	trap_Argv( 1, cmd, sizeof( cmd ) );		

	if(!*cmd) {
		CP("print \"usage: unignore [name|slot#]\n\"");
		if(!ent->client->sess.ignoreClients[0] &&
			!ent->client->sess.ignoreClients[1]) {

			return;
		}	
		CP("print \"currently ignoring:\n\"");
		for(i=0; i<MAX_CLIENTS; i++) {
			if(COM_BitCheck(ent->client->sess.ignoreClients, i)) {
				victim = &g_entities[i];
				if(!victim || !victim->client)
					continue;
				CP(va("print \" %2d: %s\n\"",
					i,
					victim->client->pers.netname));
			}
		}
		return;
	}

	matches = ClientNumbersFromString(cmd, pids);
	for(i=0; i<matches; i++) {
		if(!COM_BitCheck(ent->client->sess.ignoreClients, pids[i])) {
			matches--;
		}
		else {
			clientNum = pids[i];
		}
	}
	if(matches != 1) {
		CP("print \"unignore: no match found.  "
			"run /unignore with no parameter for a list\n\"");
		return;
	}
	victim = &g_entities[clientNum];

	COM_BitClear(ent->client->sess.ignoreClients, clientNum);
	CP(va("print \"unignore: %s^7 removed from your ignore list\n\"",
		victim->client->pers.netname));
	CPx(clientNum, va("chat \"%s^1 has stopped ignoring you\"",
		ent->client->pers.netname));
}

/*
=================
Cmd_SwapPlacesWithBot_f
=================
*/
void Cmd_SwapPlacesWithBot_f( gentity_t *ent, int botNum ) {
	gentity_t *botent;
	gclient_t cl, *client;
	clientPersistant_t	saved;
	clientSession_t sess;
	int		persistant[MAX_PERSISTANT];
	//
	client = ent->client;
	//
	botent = &g_entities[botNum];
	if (!botent->client) return;
	// if this bot is dead
	if (botent->health <= 0 && (botent->client->ps.pm_flags & PMF_LIMBO)) {
		trap_SendServerCommand( ent-g_entities, "print \"Bot is in limbo mode, cannot swap places.\n\"" );
		return;
	}
	//
	if (client->sess.sessionTeam != botent->client->sess.sessionTeam) {
		trap_SendServerCommand( ent-g_entities, "print \"Bot is on different team, cannot swap places.\n\"" );
		return;
	}
	//
	// copy the client information
	cl = *botent->client;
	//
	G_DPrintf( "Swapping places: %s in for %s\n", ent->client->pers.netname, botent->client->pers.netname);
	// kill the bot
	botent->flags &= ~FL_GODMODE;
	botent->client->ps.stats[STAT_HEALTH] = botent->health = 0;
	player_die (botent, ent, ent, 100000, MOD_SWAP_PLACES);
	// make sure they go into limbo mode right away, and dont show a corpse
	limbo( botent, qfalse );
	// respawn the player
	ent->client->ps.pm_flags &= ~PMF_LIMBO; // JPW NERVE turns off limbo
	// copy the location
	VectorCopy( cl.ps.origin, ent->s.origin );
	VectorCopy( cl.ps.viewangles, ent->s.angles );
	// copy session data, so we spawn in as the same class
	// save items
	saved = client->pers;
	sess = client->sess;
	memcpy( persistant, ent->client->ps.persistant, sizeof(persistant) );
	// give them the right weapons/etc
	*client = cl;
	client->sess = sess;
	client->sess.playerType = ent->client->sess.latchPlayerType = cl.sess.playerType;
	client->sess.playerWeapon = ent->client->sess.latchPlayerWeapon = cl.sess.playerWeapon;
	client->sess.playerWeapon2 = ent->client->sess.latchPlayerWeapon2 = cl.sess.playerWeapon2;
	// spawn them in
	ClientSpawn(ent, qtrue, qfalse, qtrue);
	// restore items
	client->pers = saved;
	memcpy( ent->client->ps.persistant, persistant, sizeof(persistant) );
	client->ps = cl.ps;
	client->ps.clientNum = ent->s.number;
	ent->health = client->ps.stats[STAT_HEALTH];
	SetClientViewAngle( ent, cl.ps.viewangles );
	// make sure they dont respawn immediately after they die
	client->pers.lastReinforceTime = 0;
}


/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum ) {
	gentity_t *ent;
	char	cmd[MAX_TOKEN_CHARS];
	char		userinfo[MAX_INFO_STRING];
	qboolean	muted = qfalse;

	ent = g_entities + clientNum;
	if ( !ent->client )
		return;		// not fully in game yet

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ));

	if ( G_shrubbot_mute_check( userinfo ) || ent->client->sess.muted )
		muted = qtrue;

	trap_Argv( 0, cmd, sizeof( cmd ) );

	if (Q_stricmp (cmd, "say") == 0) {
		if( !muted ) {
			Cmd_Say_f (ent, SAY_ALL, qfalse);
		}
		return;
	} 
	
	if (!Q_stricmp (cmd, "m") || !Q_stricmp(cmd, "pm")) {
		G_PrivateMessage(ent);
		return;
	}

	if (!Q_stricmp (cmd, "damage")) {
		G_TeamDamageStats(ent);
		return;
	}

	if( Q_stricmp (cmd, "say_team") == 0 ) {
		if( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
			trap_SendServerCommand( ent-g_entities, "print \"Can't team chat as spectator\n\"\n" );
			return;
		}

		if( !muted ) {
			Cmd_Say_f (ent, SAY_TEAM, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "vsay") == 0) {
		if( !muted ) {
			Cmd_Voice_f (ent, SAY_ALL, qfalse, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "vsay_team") == 0) {
		if( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
			trap_SendServerCommand( ent-g_entities, "print \"Can't team chat as spectator\n\"\n" );
			return;
		}

		if( !muted ) {
			Cmd_Voice_f (ent, SAY_TEAM, qfalse, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "say_buddy") == 0) {
		if( !muted ) {
			Cmd_Say_f( ent, SAY_BUDDY, qfalse );
		}
		return;
	} else if (Q_stricmp (cmd, "vsay_buddy") == 0) {
		if( !muted ) {
			Cmd_Voice_f( ent, SAY_BUDDY, qfalse, qfalse );
		}
		return;
	} else if (Q_stricmp (cmd, "score") == 0) {
		Cmd_Score_f (ent);
		return;
	} else if( Q_stricmp (cmd, "vote") == 0 ) {
		Cmd_Vote_f (ent);
		return;
	} else if (Q_stricmp (cmd, "fireteam") == 0) {
		Cmd_FireTeam_MP_f (ent);
		return;
	} else if (Q_stricmp (cmd, "showstats") == 0) {
		G_PrintAccuracyLog( ent );
		return;
	} else if (Q_stricmp (cmd, "rconAuth") == 0) {
		Cmd_AuthRcon_f( ent );
		return;
	} else if (Q_stricmp (cmd, "ignore") == 0) {
		Cmd_Ignore_f( ent );
		return;
	} else if (Q_stricmp (cmd, "unignore") == 0) {
		Cmd_UnIgnore_f( ent );
		return;
	} else if (Q_stricmp (cmd, "obj") == 0) {
		Cmd_SelectedObjective_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "impkd" ) ) {
		Cmd_IntermissionPlayerKillsDeaths_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imwa" ) ) {
		Cmd_IntermissionWeaponAccuracies_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imws" ) ) {		
		Cmd_IntermissionWeaponStats_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imready" ) ) {		
		Cmd_IntermissionReady_f( ent );
		return;
	} else if (Q_stricmp (cmd, "ws") == 0) {
		Cmd_WeaponStat_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "forcetapout" ) ) {
		if( !ent || !ent->client ) {
			return;
		}

		if( ent->health <= 0 && ( ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES ) ) {
			limbo( ent, qtrue );
		}

		return;
	}

	// OSP
	// Do these outside as we don't want to advertise it in the help screen
	if(!Q_stricmp(cmd, "wstats")) {
		G_statsPrint(ent, 1);
		return;
	}
	if(!Q_stricmp(cmd, "sgstats")) {	// Player game stats
		G_statsPrint(ent, 2);
		return;
	}
	if(!Q_stricmp(cmd, "stshots")) {	// "Topshots" accuracy rankings
		G_weaponStatsLeaders_cmd(ent, qtrue, qtrue);
		return;
	}
	if( !Q_stricmp( cmd, "rs" ) ) {
		Cmd_ResetSetup_f( ent );
		return;
	}

	if(G_commandCheck(ent, cmd, qtrue)) return;
	// OSP

	if(G_shrubbot_permission(ent, SBF_STEALTH)) 
		if(G_shrubbot_cmd_check(ent)) 
			return;

	// ignore all other commands when at intermission
	if (level.intermissiontime) {
//		Cmd_Say_f (ent, qfalse, qtrue);			// NERVE - SMF - we don't want to spam the clients with this.
		CPx(clientNum, va("print \"^3%s^7 not allowed during intermission.\n\"", cmd));
		return;
	}

	if (Q_stricmp (cmd, "give") == 0) {
		Cmd_Give_f (ent);
	} else if (Q_stricmp (cmd, "listbotgoals") == 0) {
		Cmd_ListBotGoals_f(ent);
	} else if (Q_stricmp (cmd, "god") == 0) {
		Cmd_God_f (ent);
	} else if (Q_stricmp (cmd, "nofatigue") == 0) {
		Cmd_Nofatigue_f (ent);
	} else if (Q_stricmp (cmd, "notarget") == 0) {
		Cmd_Notarget_f (ent);
	} else if (Q_stricmp (cmd, "noclip") == 0) {
		Cmd_Noclip_f (ent);
	} else if (Q_stricmp (cmd, "kill") == 0) {
		Cmd_Kill_f (ent);
	} else if (Q_stricmp (cmd, "follownext") == 0) {
		Cmd_FollowCycle_f (ent, 1);
	} else if (Q_stricmp (cmd, "followprev") == 0) {
		Cmd_FollowCycle_f (ent, -1);
	} else if (Q_stricmp (cmd, "where") == 0) {
		Cmd_Where_f (ent);
// jet Pilot - enable camera?
	} else if (Q_stricmp (cmd, "startCamera") == 0) {
		Cmd_StartCamera_f( ent );
	} else if (Q_stricmp (cmd, "stopCamera") == 0) {
		Cmd_StopCamera_f( ent );
	} else if (Q_stricmp (cmd, "setCameraOrigin") == 0) {
		Cmd_SetCameraOrigin_f( ent );
	} else if (Q_stricmp (cmd, "cameraInterrupt") == 0) {
		Cmd_InterruptCamera_f( ent );
	} else if (Q_stricmp (cmd, "setviewpos") == 0) {
		Cmd_SetViewpos_f( ent );
	} else if (Q_stricmp (cmd, "setspawnpt") == 0) {
		Cmd_SetSpawnPoint_f( ent );
	} else if (Q_stricmp (cmd, "setsniperspot") == 0) {
		Cmd_SetSniperSpot_f( ent );
//	} else if (Q_stricmp (cmd, "waypoint") == 0) {
//		Cmd_SetWayPoint_f( ent );
//	} else if (Q_stricmp (cmd, "clearwaypoint") == 0) {
//		Cmd_ClearWayPoint_f( ent );
	// OSP
	} else if(G_commandCheck(ent, cmd, qfalse)) {
		return;
	} else if (!Q_stricmp (cmd, "playdead")) {
		G_PlayDead(ent);
	} else if (!Q_stricmp (cmd, "givedisguise")) {
		Cmd_GiveDisguise_f(ent, 0);
	} else if (!Q_stricmp (cmd, "shrug")) {
		Cmd_Shrug_f(ent);	
	} else if (!Q_stricmp (cmd, "salute")) {
		Cmd_Salute_f(ent);
	} else if (!Q_stricmp (cmd, "gesture")) {
		Cmd_Gesture_f(ent);
#ifdef USELOCATIONS
	} else if (!Q_stricmp (cmd, "createloc")) {
		Cmd_CreateLoc_f(ent);
	} else if (!Q_stricmp (cmd, "loadlocations") || !Q_stricmp (cmd, "loadloc")) {
		Cmd_LoadLocations_f(/*ent*/);// jaquboss - changed to void
	} else if (!Q_stricmp (cmd, "saveloc")) { // jaquboss - please can someone explain me this ?! i wonder why this should be client command, and  wonder even more why you supply it with ent
		Cmd_SaveLocations_f(/*ent*/); // jaquboss - changed to void
#endif
#ifdef __SSYSTEM__
	} else if (!Q_stricmp (cmd , "pcinfo")) {
		Cmd_PcInfo_f(ent);
#endif
	} else if (!Q_stricmp (cmd, "dropweapon")) {
		Cmd_DropWeapon_f(ent);
	} else if (!Q_stricmp (cmd, "dropclip")) {
		Cmd_DropClip_f(ent);
	}  else if (!Q_stricmp (cmd, "currentweapons")) {
		Cmd_CurWeapons_f(ent);	
	} else if (!Q_stricmp (cmd, "tp_on")) {
		Cmd_TargetPointer_f(ent, qtrue);
	} else if (!Q_stricmp (cmd, "tp_off")) {
		Cmd_TargetPointer_f(ent, qfalse);
	} else if (!Q_stricmp (cmd, "simkill")) {
		Cmd_SimKill_f(ent);
	} else if (!Q_stricmp (cmd, "simgoomba")) {
		Cmd_SimGoomba_f(ent);		
	} else if (!Q_stricmp (cmd, "simulateevent")) {
		Cmd_SimulateEvent_f(ent);
	} else if (!Q_stricmp (cmd, "getduration")) {
		Cmd_Test_GetDurationString(ent);
	/*} else if(!Q_stricmp(cmd, "disconnect")) {  
	// tjw: workaround for the fact that if a client runs /quit  
	//      server is not notified of the disconnect.  This  
	//      requires client modification too.  
		ClientDisconnect(ent-g_entities);  // jaquboss - fixme? send dropclient trap instead?*/

	} else {
		trap_SendServerCommand( clientNum, va("print \"unknown cmd[lof] %s\n\"", cmd ) );
	}
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}


char *Q_SayConcatArgs(int start) {
	char *s;
	int c = 0;

	s = ConcatArgs(0);
	while(*s) {
		if(c == start) return s;
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return s;
}


// replaces all occurances of "\n" with '\n'
char *Q_AddCR(char *s)
{
	char *copy, *place, *start;

	if(!*s) return s;
	start = s;
	while(*s) {
		if(*s == '\\') {
			copy = s;
			place = s;
			*s++;
			if(*s && *s == 'n') {
				*copy = '\n';
				while(*++s) {
					*++copy = *s;
				}
				*++copy = '\0';
				s = place;
				continue;
			}
		}
		*s++;
	}
	return start;
}

// A replacement for trap_Argc() that can correctly handle
//   say "!cmd arg0 arg1"
// as well as
//   say !cmd arg0 arg1
// The client uses the latter for messages typed in the console
// and the former when the message is typed in the chat popup
int Q_SayArgc() 
{
	int c = 1;
	char *s;

	s = ConcatArgs(0);
	if(!*s) return 0;
	while(*s) {
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return c;
}

// A replacement for trap_Argv() that can correctly handle
//   say "!cmd arg0 arg1"
// as well as
//   say !cmd arg0 arg1
// The client uses the latter for messages typed in the console
// and the former when the message is typed in the chat popup
qboolean Q_SayArgv(int n, char *buffer, int bufferLength)
{
	int bc = 1;
	int c = 0;
	char *s;

	if(bufferLength < 1) return qfalse;
	if(n < 0) return qfalse;
	*buffer = '\0';
	s = ConcatArgs(0);
	while(*s) {
		if(c == n) {
			while(*s && (bc < bufferLength)) {
				if(*s == ' ') {
					*buffer = '\0';
					return qtrue;
				}
				*buffer = *s;
				*buffer++;
				*s++;
				bc++;
			}
			*buffer = '\0';
			return qtrue;
		}
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return qfalse;
}

