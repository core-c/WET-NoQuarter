#include "g_local.h"

/*
qboolean G_NeedEngineers( int team ) {
	int i;
	gentity_t* e;

	for ( i=0; i<g_indicators.count; i++ ) {
		e = g_indicators.ent[i];

		if(!e->inuse) {
			continue;
		}

		if( e->s.eType == ET_CONSTRUCTIBLE_INDICATOR || e->s.eType == ET_EXPLOSIVE_INDICATOR || e->s.eType == ET_TANK_INDICATOR) {
			if( e->s.teamNum == 3 ) {
				return qtrue;
			}
			else if( team == TEAM_AXIS) {
				if( e->s.teamNum == 2 ) {
					return qtrue;
				}
			}
			else {
				if( e->s.teamNum == 1 ) {
					return qtrue;
				}
			}
		}
	}

	return qfalse;
}
*/

int G_GetSysMessageNumber( const char* sysMsg ) {
	int i;

	for( i = 0; i < SM_NUM_SYS_MSGS; i++ ) {
		if(!Q_stricmp(HQMessages[i].codeString, sysMsg)) {
			return i;
		}
	}

	return -1;
}

qboolean G_SendSystemMessage( sysMsg_t message, int team ) {
	gentity_t* other;
	int *time;
	int j;

	time = team == TEAM_AXIS ? &level.lastSystemMsgTime[0] : &level.lastSystemMsgTime[1];

	if(*time && (level.time - *time) < SECONDS_5)
		return qfalse;

	*time = level.time;

	for (j = 0; j < level.numConnectedClients; j++) {
		other = &g_entities[level.sortedClients[j]];

		if(!other->client || !other->inuse)
			continue;

		if(other->client->sess.sessionTeam != team)
			continue;
#if defined(__x86_64__)
		trap_SendServerCommand( (int)(other-g_entities), va("vschat %i %i", (int)(other-g_entities), message));
#else
		trap_SendServerCommand( other-g_entities, va("vschat %i %i", other-g_entities, message));
#endif
		//trap_SendServerCommand( other-g_entities, va("vschat 0 %d 3 %s 0 0 0", other-g_entities, systemMessages[message]));
	}
	return qtrue;
}

/*
void G_CheckForNeededClasses( team_t team ) {
	qboolean playerClasses[NUM_PLAYER_CLASSES-1];
	int i, cnt;
	int teamCounts=0;
	gentity_t* ent;
	static int lastcheck[2];

	memset(playerClasses, 0, sizeof(playerClasses));

	if(lastcheck[team-1] && (level.time - lastcheck[team-1]) < SECONDS_30)
		return;

	lastcheck[team-1] = level.time;


	for( i = 0; i < level.maxclients; i++, ent++) {
		ent = &g_entities[i];

		if (!ent)
			continue;

		if(!ent->client)
			continue;

		if(ent->client->pers.connected != CON_CONNECTED )
			continue;

		// don't want spectators
		if( ent->client->sess.sessionTeam != team )
			continue;

		if(ent->client->sess.playerType != PC_SOLDIER)
			playerClasses[ent->client->sess.playerType-1] = qtrue;

		teamCounts++;
	}

	if( teamCounts < 6 )
		return;

	if(!playerClasses[PC_ENGINEER - 1])
		playerClasses[PC_ENGINEER - 1] = G_NeedEngineers( team ) ? 0 : 1;

	cnt = 0;

	for( i = 0; i < (NUM_PLAYER_CLASSES - 1); i++ ) {
		if(!playerClasses[i])
			cnt++;
	}

	if(cnt == 0)
		return;

	cnt = rand() % cnt;

	for( i = 0; i < (NUM_PLAYER_CLASSES - 1); i++ ) {
		if(!playerClasses[i]) {
			if(cnt-- == 0) {
				if (!G_SendSystemMessage( SM_NEED_MEDIC + i, team ))
					lastcheck[team-1] = level.time - SECONDS_25;
				}
			}
	}

}


void G_CheckMenDown( team_t team ) {
	int			alive;
	int			dead;
	gentity_t*	ent;
	int			i;
	qboolean	newWave=qfalse;
	int			testtime;
	static int	lastMDcheck[2];
	static qboolean checkedWave[2];

	if( team == TEAM_AXIS)
		testtime = (level.dwRedReinfOffset + level.timeCurrent - level.startTime) % g_redlimbotime.integer;
	else
		testtime = (level.dwBlueReinfOffset + level.timeCurrent - level.startTime) % g_bluelimbotime.integer;

		newWave = (testtime < lastMDcheck[team-1]);
		lastMDcheck[team-1] = testtime;

	if ( !newWave ) {
		if ( checkedWave[team-1] )
			return;
	}
	else {
		checkedWave[team-1] = qfalse;
		return;	// qait a while if new wave
	}

	alive = dead = 0;

	for( i = 0; i < level.maxclients; i++, ent++) {
		ent = &g_entities[i];

		if (!ent)
			continue;

		if(!ent->client)
			continue;

		if(ent->client->pers.connected != CON_CONNECTED )
			continue;

		if( ent->client->sess.sessionTeam != team )
			continue;

		if(ent->health <= 0)
			dead++;
		else
			alive++;
	}

	if(dead + alive >= 6 && (dead >= ((int)((dead + alive) * 0.75f)))){
		if ( G_SendSystemMessage( SM_LOST_MEN, team ))
			checkedWave[team-1] = qtrue;
	}

}
*/
