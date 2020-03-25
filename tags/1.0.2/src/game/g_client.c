#include "g_local.h"
#include "../../etmain/ui/menudef.h"
#include "etpro_mdx.h"

// g_client.c -- client functions that don't happen every frame

// Ridah, new bounding box
//static vec3_t	playerMins = {-15, -15, -24};
//static vec3_t	playerMaxs = {15, 15, 32};
vec3_t	playerMins = {-18, -18, -24};
vec3_t	playerMaxs = {18, 18, 48};
// done.

/*QUAKED info_player_deathmatch (1 0 1) (-18 -18 -24) (18 18 48)
potential spawning position for deathmatch games.
Targets will be fired when someone spawns in on them.
"nobots" will prevent bots from using this spot.
"nohumans" will prevent non-bots from using this spot.
If the start position is targeting an entity, the players camera will start out facing that ent (like an info_notnull)
*/
void SP_info_player_deathmatch( gentity_t *ent ) {
	int		i;
	vec3_t	dir;

	G_SpawnInt( "nobots", "0", &i);
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}

	ent->enemy = G_PickTarget( ent->target );
	if(ent->enemy)
	{
		VectorSubtract( ent->enemy->s.origin, ent->s.origin, dir );
		vectoangles( dir, ent->s.angles );
	}

}

//----(SA) added
/*QUAKED info_player_checkpoint (1 0 0) (-16 -16 -24) (16 16 32) a b c d
these are start points /after/ the level start
the letter (a b c d) designates the checkpoint that needs to be complete in order to use this start position
*/
void SP_info_player_checkpoint(gentity_t *ent) {
	ent->classname = "info_player_checkpoint";
	SP_info_player_deathmatch( ent );
}

//----(SA) end


/*QUAKED info_player_start (1 0 0) (-18 -18 -24) (18 18 48)
equivelant to info_player_deathmatch
*/
void SP_info_player_start(gentity_t *ent) {
	ent->classname = "info_player_deathmatch";
	SP_info_player_deathmatch( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32) AXIS ALLIED
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent ) {

}

extern void BotSpeedBonus(int clientNum);


/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
SpotWouldTelefrag

================
*/
qboolean SpotWouldTelefrag( gentity_t *spot ) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( spot->r.currentOrigin, playerMins, mins );
	VectorAdd( spot->r.currentOrigin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) {
		hit = &g_entities[touch[i]];
		if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
			return qtrue;
		}

	}

	return qfalse;
}

/*
================
SelectNearestDeathmatchSpawnPoint

Find the spot that we DON'T want to use
================
*/
#define	MAX_SPAWN_POINTS	128
gentity_t *SelectNearestDeathmatchSpawnPoint( vec3_t from ) {
	gentity_t	*spot;
	vec3_t		delta;
	float		dist, nearestDist;
	gentity_t	*nearestSpot;

	nearestDist = 999999;
	nearestSpot = NULL;
	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {

		VectorSubtract( spot->r.currentOrigin, from, delta );
		dist = VectorLength( delta );
		if ( dist < nearestDist ) {
			nearestDist = dist;
			nearestSpot = spot;
		}
	}

	return nearestSpot;
}


/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_SPAWN_POINTS	128
gentity_t *SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t	*spot;
	int			count;
	int			selection;
	gentity_t	*spots[MAX_SPAWN_POINTS];

	count = 0;
	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		count++;
	}

	if ( !count ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), "info_player_deathmatch");
	}

	selection = rand() % count;
	return spots[ selection ];
}


/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	gentity_t	*spot;
	gentity_t	*nearestSpot;

	nearestSpot = SelectNearestDeathmatchSpawnPoint( avoidPoint );

	spot = SelectRandomDeathmatchSpawnPoint ( );
	if ( spot == nearestSpot ) {
		// roll again if it would be real close to point of death
		spot = SelectRandomDeathmatchSpawnPoint ( );
		if ( spot == nearestSpot ) { 
			// last try
			spot = SelectRandomDeathmatchSpawnPoint ( );
		}		
	}

	// find a single player start spot
	if (!spot) {
		G_Error( "Couldn't find a spawn point" );
	}

	VectorCopy (spot->r.currentOrigin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectInitialSpawnPoint

Try to find a spawn point marked 'initial', otherwise
use normal spawn selection.
============
*/
/*gentity_t *SelectInitialSpawnPoint( vec3_t origin, vec3_t angles ) {
	gentity_t	*spot;

	spot = NULL;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( spot->spawnflags & 1 ) {
			break;
		}
	}

	if ( !spot || SpotWouldTelefrag( spot ) ) {
		return SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy (spot->r.currentOrigin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}*/

/*
===========
SelectSpectatorSpawnPoint

============
*/
gentity_t *SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles ) {
	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, origin );
	VectorCopy( level.intermission_angle, angles );

	return NULL;
}

/*
=======================================================================

BODYQUE

=======================================================================
*/

/*
===============
InitBodyQue
===============
*/
#ifdef USE_BODY_QUE
void InitBodyQue (void) {
	int		i;
	gentity_t	*ent;

	level.bodyQueIndex = 0;
	for (i=0; i<BODY_QUEUE_SIZE ; i++) {
		ent = G_Spawn();
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[i] = ent;
	}
}
#endif
/*
=============
BodyUnlink
  
Called by BodySink
=============
*/
void BodyUnlink( gentity_t *ent ) {
	trap_UnlinkEntity( ent );
	ent->physicsObject = qfalse;

}

void G_BodyDP ( gentity_t *ent ){
	gentity_t	*tent;
	// so clients will memset them off :P
	tent = G_TempEntity( ent->r.currentOrigin, EV_BODY_DP );
	tent->s.otherEntityNum2 = ent->s.number;
	tent->r.svFlags = SVF_BROADCAST;	// send to everyone

	G_FreeEntity( ent );
}
                
/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear 
=============
*/ 
void BodySink2( gentity_t *ent ) {
	ent->physicsObject = qfalse;
	// tjw: BODY_TIME is how long they last all together, not just sink anim
	//ent->nextthink = level.time + BODY_TIME(BODY_TEAM(ent))+1500;
	ent->nextthink = level.time + 1800;//4000; // jaquboss - less time since it is too slow, body is already sunk but still not removed
#ifdef USE_BODY_QUE
	ent->think = BodyUnlink;
#else
	// jaquboss - let's free the dead guy
	ent->think = G_BodyDP;
#endif
	ent->s.pos.trType = TR_LINEAR;
	ent->s.pos.trTime = level.time;
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	VectorSet( ent->s.pos.trDelta, 0, 0, -8 );
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent ) {
	if( ent->activator ) {
		// see if parent is still disguised
		if( ent->activator->client->ps.powerups[PW_OPS_DISGUISED] ) {
			ent->nextthink = level.time + 100;
			return;
		} else {
			ent->activator = NULL;
		}
	}

	BodySink2( ent );
}


/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent ) {
	gentity_t		*body;
	int			contents, i;

	trap_UnlinkEntity (ent);

	// if client is in a nodrop area, don't leave the body
  	contents = trap_PointContents( ent->client->ps.origin, -1 );
	if ( contents & CONTENTS_NODROP ) {
		return;
	}

	// grab a body que and cycle to the next one
#ifdef USE_BODY_QUE
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = (level.bodyQueIndex + 1) % BODY_QUEUE_SIZE;
#else
	body = G_Spawn();
#endif
	// Gordon: um, what on earth was this here for?
//	trap_UnlinkEntity (body);

	body->s = ent->s;
	body->s.eFlags = EF_DEAD;		// clear EF_TALK, etc

	if( ent->client->ps.eFlags & EF_HEADSHOT ) 
	{
		body->s.eFlags |= EF_HEADSHOT;			// make sure the dead body draws no head (if killed that way)
	}

	if( ent->client->ps.eFlags & EF_GLOW ) 
	{
		body->s.eFlags |= EF_GLOW;			
	}

	body->s.eType = ET_CORPSE;
	body->classname = "corpse";
	body->s.powerups = 0;	// clear powerups
	body->s.loopSound = 0;	// clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) {
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	} else {
		body->s.pos.trType = TR_STATIONARY;
	}
	body->s.event = 0;

	// DHM - Clear out event system
	for( i=0; i<MAX_EVENTS; i++ )
		body->s.events[i] = 0;
	body->s.eventSequence = 0;

	// DHM - Nerve
	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
/*	switch ( body->s.legsAnim & ~ANIM_TOGGLEBIT ) 
	{
	case BOTH_DEATH1:	
	case BOTH_DEAD1:
	default:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
		break;
	case BOTH_DEATH2:
	case BOTH_DEAD2:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
		break;
	case BOTH_DEATH3:
	case BOTH_DEAD3:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
		break;
	}*/
	// time needed to complete animation
	body->s.torsoAnim = body->s.legsAnim = ent->client->legsDeathAnim;

	body->r.svFlags = ent->r.svFlags & ~SVF_BOT;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);


	if ( ent->client->deathAnim ) {
		orientation_t or;			// DHM - Nerve
		grefEntity_t refent;

		body->s.effect1Time = level.time + ent->client->ps.pm_time;

		// jaquboss - still animate them
		// zinx - realistic hitboxes
		VectorCopy( ent->client->ps.viewangles, body->s.angles);
		mdx_PlayerAnimation( body );
		mdx_gentity_to_grefEntity( body, &refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		
		mdx_tag_position( body, &refent, or.origin, "tag_chest", 0, 0 );

		body->r.mins[0] = or.origin[0] - 20; 
		body->r.mins[1] = or.origin[1] - 20; 

		body->r.maxs[0] = or.origin[0] + 20; 
		body->r.maxs[1] = or.origin[1] + 20; 

	} else {
		body->s.effect1Time = 0; // was primary weapon before we died
	}
	
	// ydnar: bodies have lower bounding box
	body->r.maxs[ 2 ] = 0;

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	// DHM - Nerve :: allow bullets to pass through bbox
	// Gordon: need something to allow the hint for covert ops
	body->r.contents = CONTENTS_CORPSE;
	body->r.ownerNum = ent->r.ownerNum;

	BODY_TEAM(body) = ent->client->sess.sessionTeam;
	BODY_CLASS(body) = ent->client->sess.playerType;
	BODY_CHARACTER(body) = ent->client->pers.characterIndex;
	BODY_VALUE(body) = 0;
	BODY_WEAPON(body) = ent->client->sess.playerWeapon;

	if ( ent->client->ps.eFlags & EF_PANTSED ){
	body->s.time2 =			1;
	} else {
	body->s.time2 =			0;
	}

	body->activator = NULL;

	body->nextthink = level.time + BODY_TIME(ent->client->sess.sessionTeam);

	body->think = BodySink;

	body->die = body_die;

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH ) {
		body->takedamage = qfalse;
	} else {
		body->takedamage = qtrue;
	}


	VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );
	trap_LinkEntity (body);
}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) {
	int			i;

	// set the delta angle
	for (i=0 ; i<3 ; i++) {
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

void SetClientViewAnglePitch( gentity_t *ent, vec_t angle ) {
	int	cmdAngle;

	cmdAngle = ANGLE2SHORT(angle);
	ent->client->ps.delta_angles[PITCH] = cmdAngle - ent->client->pers.cmd.angles[PITCH];

	ent->s.angles[ PITCH ] = 0;
	VectorCopy( ent->s.angles, ent->client->ps.viewangles);
}


void G_DropLimboHealth(gentity_t *ent)
{
	vec3_t launchvel, launchspot;
	int packs = 0;
	int i;
	int cwt;

	if(g_dropHealth.integer == 0)
		return;
	if(!ent->client)
		return;
	if(ent->client->sess.playerType != PC_MEDIC) 
		return;
	if(g_gamestate.integer != GS_PLAYING)
		return;

	cwt		= ent->client->ps.classWeaponTime;
	packs	= g_dropHealth.integer;	
	if(g_dropHealth.integer == -1) {
		packs = 5; // max packs
		ent->client->ps.classWeaponTime += (level.time - ent->client->deathTime + 10000);
	}
	for(i=0; i<packs; i++) 
	{
		if(g_dropHealth.integer == -1 && ent->client->ps.classWeaponTime >= level.time) 
		{
			break;
		}
		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = 0;
		VectorScale(launchvel, 100, launchvel);
		launchvel[2] = (25 + g_throwDistance.integer);
		VectorCopy(ent->r.currentOrigin, launchspot);
		
		Weapon_Medic_Ext(ent, launchspot, launchspot, launchvel);
	}
	ent->client->ps.classWeaponTime = cwt;
}

void G_DropLimboAmmo(gentity_t *ent)
{
	vec3_t launchvel, launchspot;
	int packs = 0;
	int i;
	int cwt;

	if(g_dropAmmo.integer == 0)
		return;
	if(!ent->client)
		return;
	if(ent->client->sess.playerType != PC_FIELDOPS) 
		return;

	cwt		= ent->client->ps.classWeaponTime;
	packs	= g_dropAmmo.integer;

	if(g_dropAmmo.integer == -1) {
		packs = 5; // max packs
		// adjust the charge timer so that the fops only drops as much ammo as he could have before he died. 
		// don't ask me where the 10000 comes from, it just makes this more accurate.
		ent->client->ps.classWeaponTime += (level.time - ent->client->deathTime	+ 10000);
	}
	for(i=0; i<packs; i++) 
	{
		if(g_dropAmmo.integer == -1 && ent->client->ps.classWeaponTime >= level.time) 
		{
			break;
		}

		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = 0;

		VectorScale(launchvel, 100, launchvel);

		launchvel[2] = (25 + g_throwDistance.integer);
		VectorCopy(ent->r.currentOrigin, launchspot);
		
		Weapon_MagicAmmo_Ext(ent, launchspot, launchspot, launchvel);
	}

	ent->client->ps.classWeaponTime = cwt;
}


/* JPW NERVE
================
limbo
================
*/
void limbo( gentity_t *ent, qboolean makeCorpse ) 
{
	int i,contents;
	//int startclient = ent->client->sess.spectatorClient;
	int startclient = ent->client->ps.clientNum;

	if(ent->r.svFlags & SVF_POW) {
		return;
	}

	if (!(ent->client->ps.pm_flags & PMF_LIMBO)) {

		if( ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
			if( g_maxlivesRespawnPenalty.integer ) {
				ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] = g_maxlivesRespawnPenalty.integer;
			} else {
				ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] = -1;
			}
		}

		// DHM - Nerve :: First save off persistant info we'll need for respawn
		for( i = 0; i < MAX_PERSISTANT; i++) {
			ent->client->saved_persistant[i] = ent->client->ps.persistant[i];
		}

		ent->client->ps.pm_flags |= PMF_LIMBO;
		ent->client->ps.pm_flags |= PMF_FOLLOW;

		
		if( makeCorpse ) {
			G_DropLimboHealth(ent);
			G_DropLimboAmmo(ent);
			CopyToBodyQue (ent); // make a nice looking corpse
		} else {
			trap_UnlinkEntity (ent);
		}

		// DHM - Nerve :: reset these values
		ent->client->ps.viewlocked = 0;
		ent->client->ps.viewlocked_entNum = 0;

		ent->r.maxs[2] = 0;
		ent->r.currentOrigin[2] += 8;
		contents = trap_PointContents( ent->r.currentOrigin, -1 ); // drop stuff
		ent->s.weapon = ent->client->limboDropWeapon; // stored in player_die()
		if ( makeCorpse && !( contents & CONTENTS_NODROP ) ) {
			TossClientItems( ent );
		}

		ent->client->sess.spectatorClient = startclient;
		Cmd_FollowCycle_f(ent,1); // get fresh spectatorClient

		if (ent->client->sess.spectatorClient == startclient) {	
			// No one to follow, so just stay put
			ent->client->sess.spectatorState = SPECTATOR_FREE;
		}
		else
			ent->client->sess.spectatorState = SPECTATOR_FOLLOW;

//		ClientUserinfoChanged( ent->client - level.clients );		// NERVE - SMF - don't do this
		if (ent->client->sess.sessionTeam == TEAM_AXIS)	{
			ent->client->deployQueueNumber = level.redNumWaiting;
			level.redNumWaiting++;
		}
		else if (ent->client->sess.sessionTeam == TEAM_ALLIES) {
			ent->client->deployQueueNumber = level.blueNumWaiting;
			level.blueNumWaiting++;
		}

		for(i=0; i<level.numConnectedClients; i++) {
			gclient_t *cl = &level.clients[level.sortedClients[i]];

			if(cl->sess.spectatorClient != (ent - g_entities) ||
				cl->sess.spectatorState != SPECTATOR_FOLLOW   ||
				(g_spectator.integer & SPECF_PERSIST_FOLLOW)) {

				continue;
			}

			if((cl->ps.pm_flags & PMF_LIMBO)) {				
				Cmd_FollowCycle_f(&g_entities[level.sortedClients[i]], 1);
			}
			else if(cl->sess.sessionTeam == TEAM_SPECTATOR) {				
				if((g_spectator.integer & SPECF_FL_ON_LIMBO)) {
					cl->sess.spectatorState = SPECTATOR_FREE;
					ClientBegin(cl - level.clients);
				}
				else {
					Cmd_FollowCycle_f(&g_entities[level.sortedClients[i]], 1);
				}
			}
		}
	}
}

/* JPW NERVE
================
reinforce 
================
// -- called when time expires for a team deployment cycle and there is at least one guy ready to go
*/
void reinforce(gentity_t *ent) {
	int p, team;// numDeployable=0, finished=0; // TTimo unused
	char *classname;
	gclient_t *rclient;
#ifndef NO_BOT_SUPPORT
	char	userinfo[MAX_INFO_STRING], *respawnStr;

	if (ent->r.svFlags & SVF_BOT) {
		trap_GetUserinfo( ent->s.number, userinfo, sizeof(userinfo) );
		respawnStr = Info_ValueForKey( userinfo, "respawn" );
		if (!Q_stricmp( respawnStr, "no" ) || !Q_stricmp( respawnStr, "off" )) {
			return;	// no respawns
		}
	}
#endif

	if (!(ent->client->ps.pm_flags & PMF_LIMBO)) {
		G_Printf("player already deployed, skipping\n");
		return;
	}
#ifdef MV_SUPPORT
	if(ent->client->pers.mvCount > 0) {
		G_smvRemoveInvalidClients(ent, TEAM_AXIS);
		G_smvRemoveInvalidClients(ent, TEAM_ALLIES);
	}
#endif
	// get team to deploy from passed entity
	team = ent->client->sess.sessionTeam;

	// find number active team spawnpoints
	if (team == TEAM_AXIS)
		classname = "team_CTF_redspawn";
	else if (team == TEAM_ALLIES)
		classname = "team_CTF_bluespawn";
	else
		assert(0);

	// DHM - Nerve :: restore persistant data now that we're out of Limbo
	rclient = ent->client;
	for (p=0; p<MAX_PERSISTANT; p++)
		rclient->ps.persistant[p] = rclient->saved_persistant[p];
	// dhm

	respawn(ent);
}
// jpw


/*
================
respawn
================
*/
void respawn( gentity_t *ent ) 
{
	ent->client->ps.pm_flags &= ~PMF_LIMBO; // JPW NERVE turns off limbo

	// DHM - Nerve :: Decrease the number of respawns left
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( ent->client->ps.persistant[PERS_RESPAWNS_LEFT] > 0 && g_gamestate.integer == GS_PLAYING ) {
			if( g_maxlives.integer > 0 ) {
				ent->client->ps.persistant[PERS_RESPAWNS_LEFT]--;
			} else {
				if( g_alliedmaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_ALLIES ) {
					ent->client->ps.persistant[PERS_RESPAWNS_LEFT]--;
				}
				if( g_axismaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_AXIS ) {
					ent->client->ps.persistant[PERS_RESPAWNS_LEFT]--;
				}
			}
		}
	}

	G_DPrintf( "Respawning %s, %i lives left\n", ent->client->pers.netname, ent->client->ps.persistant[PERS_RESPAWNS_LEFT]);

	ClientSpawn(ent, qfalse, qfalse, qtrue);


	// DHM - Nerve :: Add back if we decide to have a spawn effect
	// add a teleportation effect
	//tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
	//tent->s.clientNum = ent->s.clientNum;
}

// NERVE - SMF - merge from team arena
/*
================
TeamCount

Returns number of players on a team
================
*/
team_t TeamCount(int ignoreClientNum, int team)
{
	int i, ref, count = 0;

	for(i=0; i<level.numConnectedClients; i++) {
		if((ref = level.sortedClients[i]) == ignoreClientNum) continue;
		if(level.clients[ref].sess.sessionTeam == team) count++;
	}

	return(count);
}
// -NERVE - SMF

/*
================
PickTeam

================
*/
team_t PickTeam(int ignoreClientNum)
{
	int counts[TEAM_NUM_TEAMS] = { 0, 0, 0 };

	counts[TEAM_ALLIES] = TeamCount(ignoreClientNum, TEAM_ALLIES);
	counts[TEAM_AXIS] = TeamCount(ignoreClientNum, TEAM_AXIS);

	if(counts[TEAM_ALLIES] > counts[TEAM_AXIS]) return(TEAM_AXIS);
	if(counts[TEAM_AXIS] > counts[TEAM_ALLIES]) return(TEAM_ALLIES);

	// equal team count, so join the team with the lowest score
	return(((level.teamScores[TEAM_ALLIES] > level.teamScores[TEAM_AXIS]) ? TEAM_AXIS : TEAM_ALLIES));
}

/*
===========
AddExtraSpawnAmmo
===========
*/
static void AddExtraSpawnAmmo( gclient_t *client, weapon_t weaponNum)
{
	switch( weaponNum ) 
	{
		//case WP_KNIFE:
		case WP_LUGER:
		case WP_COLT:
		case WP_STEN:
		case WP_STEN_MKII:
		case WP_MP34:
		case WP_STG44:
		case WP_SILENCER:
		case WP_CARBINE:
		case WP_KAR98:
		case WP_SILENCED_COLT:
			if( client->sess.skill[SK_LIGHT_WEAPONS] >= 1 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += GetAmmoTableData(weaponNum)->maxclip;
			break;
		case WP_SHOTGUN:
			if( client->sess.skill[SK_LIGHT_WEAPONS] >= 1 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += 8; // jP - 8 instead of 1 for extra shotgun shells
			break;
		case WP_MP40:
		case WP_THOMPSON:
			if( (client->sess.skill[SK_FIRST_AID] >= 1 && client->sess.playerType == PC_MEDIC) || client->sess.skill[SK_LIGHT_WEAPONS] >= 1 ) {
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += GetAmmoTableData(weaponNum)->maxclip;
			}
			break;
		case WP_M7:
		case WP_GPG40:
			if( client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 1 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += 4;
			break;
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
			if( client->sess.playerType == PC_ENGINEER ) {
				if( client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 1 ) {
					client->ps.ammoclip[BG_FindAmmoForWeapon(weaponNum)] += 4;
				}
			}
			if( client->sess.playerType == PC_MEDIC ) {
				if( client->sess.skill[SK_FIRST_AID] >= 1 ) {
					client->ps.ammoclip[BG_FindAmmoForWeapon(weaponNum)] += 1;
				}
			}
			break;
		case WP_VENOM:
		case WP_MOBILE_MG42:
		case WP_MOBILE_BROWNING:
		case WP_BAR:
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		//case WP_FLAMETHROWER:
			if( client->sess.skill[SK_HEAVY_WEAPONS] >= 7 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += GetAmmoTableData(weaponNum)->maxclip;
			break;
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_MORTAR:
		case WP_MORTAR_SET:
			if( client->sess.skill[SK_HEAVY_WEAPONS] >= 7 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += 2;
			break;
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
			if( client->sess.skill[SK_FIRST_AID] >= 2 )
 				client->ps.ammoclip[BG_FindAmmoForWeapon(weaponNum)] += 2;
			break;
		case WP_GARAND:
		case WP_K43:
		case WP_FG42:
			if( client->sess.skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 1 || client->sess.skill[SK_LIGHT_WEAPONS] >= 1 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += GetAmmoTableData(weaponNum)->maxclip;
			break;
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_FG42_SCOPE:
			if( client->sess.skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 1 )
				client->ps.ammo[BG_FindAmmoForWeapon(weaponNum)] += GetAmmoTableData(weaponNum)->maxclip;
			break;
		default:
			break;
	}
}

qboolean AddWeaponToPlayer( gclient_t *client, weapon_t weapon, int ammo, int ammoclip, qboolean setcurrent ) 
{
#ifdef OMNIBOTS
	qboolean isBot = (g_entities[client->ps.clientNum].r.svFlags & SVF_BOT) ? qtrue : qfalse;
#endif
	
	COM_BitSet( client->ps.weapons, weapon );

	client->ps.ammoclip[BG_FindClipForWeapon(weapon)]	= ammoclip;
	client->ps.ammo[BG_FindAmmoForWeapon(weapon)]		= ammo;

	if( setcurrent )
	{
		client->ps.weapon = weapon;
	}

	// skill handling
	AddExtraSpawnAmmo( client, weapon );

#ifdef OMNIBOTS
	if (isBot) {
		botUserData.DataType = dtInt;
		botUserData.udata.m_Int = Bot_WeaponGameToBot(weapon);
		Bot_Interface_SendEvent(MESSAGE_ADDWEAPON, client->ps.clientNum, 0, 0, &botUserData);
	}
#endif

	return qtrue;
}

void BotSetPOW(int entityNum, qboolean isPOW);



/*
 G_AddClassSpecificTools  //tjw
 This stuff was stripped out of SetWolfSpawnWeapons()  so it can
 be used for class stealing.  It needed cleaning up badly anyway.
 It still does.
 */
void G_AddClassSpecificTools(gclient_t *client) 
{
//	int	pc = client->sess.playerType;
	qboolean add_binocs = qfalse;

	if(client->sess.skill[SK_BATTLE_SENSE] >= 1)
		add_binocs = qtrue;

	switch(client->sess.playerType) {
		case PC_ENGINEER:
			AddWeaponToPlayer( client, WP_DYNAMITE, 0, 1, qfalse );
			AddWeaponToPlayer( client, WP_PLIERS, 0, 1, qfalse );
			AddWeaponToPlayer( client, 
				WP_LANDMINE,
				GetAmmoTableData(WP_LANDMINE)->defaultStartingAmmo,
				GetAmmoTableData(WP_LANDMINE)->defaultStartingClip,
				qfalse );
			if( client->sess.skill[SK_ENGINEER] >= 4 )
				client->ps.powerups[PW_FLAKJACKET] = INT_MAX;
			break;
		case PC_COVERTOPS:
			add_binocs = qtrue;
			AddWeaponToPlayer(client,
				WP_SMOKE_BOMB,
				GetAmmoTableData(WP_SMOKE_BOMB)->defaultStartingAmmo,
				GetAmmoTableData(WP_SMOKE_BOMB)->defaultStartingClip,
				qfalse);
			// See if we already have a satchel charge placed 
			// this needs to be here for class stealing
			if( G_FindSatchel( &g_entities[client->ps.clientNum] ) ) {
				AddWeaponToPlayer( client, WP_SATCHEL, 0, 0, qfalse );
				AddWeaponToPlayer( client, WP_SATCHEL_DET, 0, 1, qfalse );
			} else {
				AddWeaponToPlayer( client, WP_SATCHEL, 0, 1, qfalse );
				AddWeaponToPlayer( client, WP_SATCHEL_DET, 0, 0, qfalse );
			}
			break;
		case PC_FIELDOPS:
			if ( !(g_weapons.integer & WPF_NO_L0_FOPS_BINOCS) || client->sess.skill[SK_SIGNALS] >= 1 )
			add_binocs = qtrue;
			AddWeaponToPlayer(client, WP_AMMO,			0, 1, qfalse);
			AddWeaponToPlayer(client, WP_SMOKE_MARKER,	
				GetAmmoTableData(WP_SMOKE_MARKER)->defaultStartingAmmo,
				GetAmmoTableData(WP_SMOKE_MARKER)->defaultStartingClip,
				qfalse);
			break;
		case PC_MEDIC:
			AddWeaponToPlayer(client,
				WP_MEDIC_SYRINGE,
				GetAmmoTableData(WP_MEDIC_SYRINGE)->defaultStartingAmmo,
				GetAmmoTableData(WP_MEDIC_SYRINGE)->defaultStartingClip,
				qfalse);
			if( client->sess.skill[SK_FIRST_AID] >= 4 && !(g_medics.integer & MEDIC_NOSELFADREN))
				AddWeaponToPlayer(client,
					WP_MEDIC_ADRENALINE,
					GetAmmoTableData(WP_MEDIC_ADRENALINE)->defaultStartingAmmo,
					GetAmmoTableData(WP_MEDIC_ADRENALINE)->defaultStartingClip,
					qfalse);

			AddWeaponToPlayer(client,
				WP_MEDKIT,
				GetAmmoTableData(WP_MEDKIT)->defaultStartingAmmo,
				GetAmmoTableData(WP_MEDKIT)->defaultStartingClip,
				qfalse);
			break;
		case PC_SOLDIER:
			break;
	}
	if(add_binocs) {
		if(AddWeaponToPlayer(client, WP_BINOCULARS, 1, 0, qfalse)) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	}

	//// Josh: adren in different classes when level 4
	// jet Pilot - keep adrenaline between classes
	if( (client->sess.playerType != PC_MEDIC && (jp_keepAwards.integer & JP_KEEP_ADRENALINE))
		  && client->sess.skill[SK_FIRST_AID] >= 4 )
		AddWeaponToPlayer(client,
			WP_MEDIC_ADRENALINE,
			GetAmmoTableData(WP_MEDIC_ADRENALINE)->defaultStartingAmmo,
			GetAmmoTableData(WP_MEDIC_ADRENALINE)->defaultStartingClip,
			qfalse);

	if( (client->sess.playerType != PC_ENGINEER && (jp_keepAwards.integer & JP_KEEP_FLAKJACKET))
		  && client->sess.skill[SK_ENGINEER] >= 4 )
		  client->ps.powerups[PW_FLAKJACKET] = INT_MAX;

	// jet Pilot - offhand grenades
	if( client->sess.skill[SK_LIGHT_WEAPONS]>= 7)
		client->ps.powerups[PW_OFFHAND_GRENADES] = INT_MAX;


}
/*
===========================================
G_CorrectSecondaryTeam
	

makes sure that client recieve correct secondary
============================================
*/
weapon_t G_CorrectSecondaryWeapon( weapon_t original, team_t team, int LWSkill, int HWSkill, int pc )
{
	switch ( original )
	{
	case WP_SILENCER:
	case WP_SILENCED_COLT:
	case WP_LUGER:
	case WP_COLT:
	case WP_SILENCER:
	case WP_SILENCED_COLT:
		return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
	case WP_AKIMBO_COLT:
	case WP_AKIMBO_LUGER:
		if ( LWSkill >= 4 )
			return team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
		else
			return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
	case WP_AKIMBO_SILENCEDCOLT:
	case WP_AKIMBO_SILENCEDLUGER:
		if ( LWSkill >= 4 )
			if ( pc == PC_COVERTOPS )
				return team == TEAM_ALLIES ? WP_AKIMBO_SILENCEDCOLT : WP_AKIMBO_SILENCEDLUGER;
			else
				return team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
		else
			return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
	case WP_THOMPSON:
	case WP_MP40:
		if ( HWSkill >= 4 && pc == PC_SOLDIER )
			return team == TEAM_ALLIES ? WP_THOMPSON : WP_MP40;
		else 	if ( LWSkill >= 4 )
			return team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
		else
			return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
	default:
		return WP_NONE; // invalid weapon for secondary
	}
}
void G_AddClassWeapons(gclient_t *client)
{
	int					numClips, numNades;
	weapon_t			primary=WP_NONE, secondary=WP_NONE, secondary2=WP_NONE, grenade=WP_NONE;
	bg_playerclass_t	*classInfo;
	team_t				team;
	int					pc;

	pc = client->sess.playerType;	
	team = client->sess.sessionTeam;
	classInfo = team == TEAM_ALLIES ? &bg_allies_playerclasses[pc] : &bg_axis_playerclasses[pc];
	grenade = team == TEAM_ALLIES ? WP_GRENADE_PINEAPPLE : WP_GRENADE_LAUNCHER;

	switch(pc) 
	{
		case PC_SOLDIER:
			numClips = 1;
			numNades = 4;
			break;
		case PC_MEDIC:
			numClips = 0;
			numNades = 1;
			break;
		case PC_ENGINEER:
			numClips = 1;
			numNades = 4;
			break;
		case PC_FIELDOPS:
			numClips = 1;
			numNades = 1;
			break;
		case PC_COVERTOPS:
			numClips = 1;
			numNades = 2;
			break;
		default:
			return; 
	}

	primary = client->sess.latchPlayerWeapon;
	secondary = client->sess.latchPlayerWeapon2;

	// correct player weapon
	secondary = G_CorrectSecondaryWeapon( secondary, team, client->sess.skill[SK_LIGHT_WEAPONS], client->sess.skill[SK_HEAVY_WEAPONS], pc );

	// set secondary slot if not available
	if ( secondary <= WP_NONE || secondary >= WP_NUM_WEAPONS ) {
		if (  client->sess.skill[SK_SOLDIER] >= 4 && pc == PC_SOLDIER ) {
			secondary = team == TEAM_ALLIES ? WP_THOMPSON : WP_MP40;
		} else if (  client->sess.skill[SK_LIGHT_WEAPONS] >= 4 && pc == PC_COVERTOPS ) {
			secondary = team == TEAM_ALLIES ? WP_AKIMBO_SILENCEDCOLT : WP_AKIMBO_SILENCEDLUGER;
		} else if (  client->sess.skill[SK_LIGHT_WEAPONS] >= 4 ) {
			secondary = team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
		} else {
			if (  pc == PC_COVERTOPS ) 
			secondary2 = team == TEAM_ALLIES ? WP_SILENCED_COLT : WP_SILENCER;

			secondary = team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
		} 
	} else if ( pc == PC_COVERTOPS ){
		secondary2 = weapAlts[secondary];
	}

	// add ammo to akimbo sidearm
	if( BG_IsAkimboWeapon(secondary)) 
		client->ps.ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(secondary))] = GetAmmoTableData(secondary)->defaultStartingClip;

	// add primary
	if ( primary <= WP_NONE || primary >= WP_NUM_WEAPONS || !BG_ClassHasWeapon(classInfo, primary ) || G_IsWeaponDisabled( &g_entities[client->ps.clientNum], primary, qtrue )) {
		primary = classInfo->classWeapons[0];
	} 

	AddWeaponToPlayer( client, primary, GetAmmoTableData(primary)->defaultStartingAmmo*numClips, GetAmmoTableData(primary)->defaultStartingClip, qtrue );
	// add alt weapon if available
	if ( weapAlts[primary] && !G_IsWeaponDisabled(&g_entities[client->ps.clientNum], weapAlts[primary], qtrue) ){
		AddWeaponToPlayer( client, weapAlts[primary], GetAmmoTableData(weapAlts[primary])->defaultStartingAmmo*numClips, GetAmmoTableData(weapAlts[primary])->defaultStartingClip, qfalse );
		if ( weapAlts[primary] == WP_M7 || weapAlts[primary] == WP_GPG40 ){
			if ( client->sess.rifleNade != 1 ){
				client->sess.rifleNade = 1; 
				ClientUserinfoChanged( client->ps.clientNum ); 
			}
		} else {
			if ( client->sess.rifleNade != 0 ){
				client->sess.rifleNade = 0;
				ClientUserinfoChanged( client->ps.clientNum );
			}
		}
	} else {
		if ( client->sess.rifleNade != 0 ){
			client->sess.rifleNade = 0;
			ClientUserinfoChanged( client->ps.clientNum ); 
		}
	}

	// add secondary
	AddWeaponToPlayer( client, secondary, GetAmmoTableData(secondary)->defaultStartingAmmo, GetAmmoTableData(secondary)->defaultStartingClip, qfalse );
		if ( secondary2 )
	AddWeaponToPlayer( client, secondary2, GetAmmoTableData(secondary2)->defaultStartingAmmo, GetAmmoTableData(secondary2)->defaultStartingClip, qfalse );

	// add nades
	AddWeaponToPlayer( client, grenade, 0, numNades, qfalse );

}

/*
===========
SetWolfSpawnWeapons
===========
*/
void SetWolfSpawnWeapons( gclient_t *client ) 
{
	int pc;
#ifdef OMNIBOTS
	qboolean	isBot = (g_entities[client->ps.clientNum].r.svFlags & SVF_BOT) ? qtrue : qfalse;
#endif

	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) 
		return;

#ifdef OMNIBOTS
	if (isBot)
	{
		Bot_Interface_SendEvent(MESSAGE_RESETWEAPONS, client->ps.clientNum, 0, 0, 0);
	}
#endif

    pc = client->sess.playerType;

	// Reset special weapon time
	if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_HALFCHARGE)) 
	{
		int cteam = (client->sess.sessionTeam - 1);
		int ctime = 0;

		switch(client->sess.playerType) {
		case PC_MEDIC:
			ctime = level.medicChargeTime[cteam];
			break;
		case PC_ENGINEER:
			ctime = level.engineerChargeTime[cteam];
			break;
		case PC_FIELDOPS:
			ctime = level.lieutenantChargeTime[cteam];
			break;
		case PC_COVERTOPS:
			ctime = level.covertopsChargeTime[cteam];
			break;
		default:
			ctime = level.soldierChargeTime[cteam];	
		}
		client->ps.classWeaponTime = level.time - (.5f * ctime);
	}
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_ZEROCHARGE)) 
	{
		client->ps.classWeaponTime = level.time;
	}
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_SAMECHARGE)) 
	{
		// tjw: classWeaponTime preserved in ClientSpawn()
		//      adjust it to compensate for the time of /kill
		client->ps.classWeaponTime += level.time - client->deathTime; 
	}
	else
	{
		// tjw: give full charge bar
		client->ps.classWeaponTime = -999999;
	}

	client->pers.slashKill = qfalse;

	// Communicate it to cgame
	client->ps.stats[STAT_PLAYER_CLASS] = pc;

	// Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
	client->ps.teamNum = pc;

	// JPW NERVE -- zero out all ammo counts
	memset(client->ps.ammo, 0, MAX_WEAPONS * sizeof(int));

	// All players start with a knife (not OR-ing so that it clears previous weapons)
	client->ps.weapons[0] = 0;
	client->ps.weapons[1] = 0;


	// jet Pilot - Add Knife to Inventory (KABAR for allies, dagger for axis)
	AddWeaponToPlayer( client, client->sess.sessionTeam == TEAM_AXIS ? WP_KNIFE : WP_KNIFE_KABAR, 1, 0, qtrue );
	
	client->ps.weaponstate = WEAPON_READY;

	// jet Pilot - Give Adrenaline if the player has earned it
	if (client->sess.skill[SK_MEDIC] >= 4 && (pc == PC_MEDIC || jp_keepAwards.integer & JP_KEEP_ADRENALINE))
	{
		AddWeaponToPlayer(client, WP_MEDIC_ADRENALINE, 0, 10, qfalse);
	}


	if(g_knifeonly.integer || jp_insanity.integer & JP_INSANITY_FIREBOLT)
	{
		switch(pc) 
		{
			case PC_MEDIC:
				AddWeaponToPlayer(client, WP_MEDIC_SYRINGE, 0, 20, qfalse);
				break;
			case PC_ENGINEER:
				AddWeaponToPlayer(client, WP_DYNAMITE, 0, 1, qfalse);
				AddWeaponToPlayer(client, WP_PLIERS, 0, 1, qfalse);
				break;
		}

		if ( jp_insanity.integer & JP_INSANITY_FIREBOLT )
		AddWeaponToPlayer( client, WP_FIREBOLT, 1, 0, qtrue );

		return;
	}
	
	G_AddClassWeapons(client);
	G_AddClassSpecificTools(client);
}
#if 0
/*
===========
SetWolfSpawnWeapons
===========
*/
void SetWolfSpawnWeapons( gclient_t *client ) 
{
	int pc;
	//qboolean isBot = qfalse;
	//qboolean isPOW = qfalse;

    pc = client->sess.playerType;

#ifdef NQ_BOT_SUPPORT
	if(g_entities[client->ps.clientNum].r.svFlags & SVF_BOT)
		isBot = qtrue;
	if(g_entities[client->ps.clientNum].r.svFlags & SVF_POW) 
		isPOW = qtrue;
#endif//NQ_BOT_SUPPORT

	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) 
		return;

	// Reset special weapon time
	if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_HALFCHARGE)) 
	{
		int cteam = (client->sess.sessionTeam - 1);
		int ctime = 0;

		switch(client->sess.playerType) {
		case PC_MEDIC:
			ctime = level.medicChargeTime[cteam];
			break;
		case PC_ENGINEER:
			ctime = level.engineerChargeTime[cteam];
			break;
		case PC_FIELDOPS:
			ctime = level.lieutenantChargeTime[cteam];
			break;
		case PC_COVERTOPS:
			ctime = level.covertopsChargeTime[cteam];
			break;
		default:
			ctime = level.soldierChargeTime[cteam];	
		}
		client->ps.classWeaponTime = level.time - (.5f * ctime);
	}
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_ZEROCHARGE)) 
	{
		client->ps.classWeaponTime = level.time;
	}
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_SAMECHARGE)) 
	{
		// tjw: classWeaponTime preserved in ClientSpawn()
		//      adjust it to compensate for the time of /kill
		client->ps.classWeaponTime += level.time - client->deathTime; 
	}
	else
	{
		// tjw: give full charge bar
		client->ps.classWeaponTime = -999999;
	}

#ifdef OMNIBOTS
	if ( isBot )
	{
		Bot_Interface_SendEvent(MESSAGE_RESETWEAPONS, client->ps.clientNum, 0, 0, 0);
	}
#endif

	client->pers.slashKill = qfalse;

	// Communicate it to cgame
	client->ps.stats[STAT_PLAYER_CLASS] = pc;

	// Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
	client->ps.teamNum = pc;

	// JPW NERVE -- zero out all ammo counts
	memset(client->ps.ammo, 0, MAX_WEAPONS * sizeof(int));

	// All players start with a knife (not OR-ing so that it clears previous weapons)
	client->ps.weapons[0] = 0;
	client->ps.weapons[1] = 0;

	// Gordon: set up pow status
#ifdef NO_BOT_SUPPORT
	if( isBot )
	{
		if( isPOW ) 
		{
			BotSetPOW( client->ps.clientNum, qtrue );
			return;
		}
		else 
		{
			BotSetPOW( client->ps.clientNum, qfalse );
		}
	}
#endif//NQ_BOT_SUPPORT

	// jet Pilot - Add Knife to Inventory (KABAR for allies, dagger for axis)
	AddWeaponToPlayer( client, client->sess.sessionTeam == TEAM_AXIS ? WP_KNIFE : WP_KNIFE_KABAR, 1, 0, qtrue );
	
	client->ps.weaponstate = WEAPON_READY;

	// jet Pilot - Give Adrenaline if the player has earned it
	if (client->sess.skill[SK_MEDIC] >= 4 && (pc == PC_MEDIC || jp_keepAwards.integer & JP_KEEP_ADRENALINE))
	{
		AddWeaponToPlayer(client, WP_MEDIC_ADRENALINE, 0, 10, qfalse);
	}


	if(g_knifeonly.integer || jp_insanity.integer & JP_INSANITY_FIREBOLT)
	{
		switch(pc) 
		{
			case PC_MEDIC:
				AddWeaponToPlayer(client, WP_MEDIC_SYRINGE, 0, 20, qfalse);
				break;
			case PC_ENGINEER:
				AddWeaponToPlayer(client, WP_DYNAMITE, 0, 1, qfalse);
				AddWeaponToPlayer(client, WP_PLIERS, 0, 1, qfalse);
				break;
		}

		if ( jp_insanity.integer & JP_INSANITY_FIREBOLT )
		AddWeaponToPlayer( client, WP_FIREBOLT, 1, 0, qtrue );

		return;
	}

	switch(pc) 
	{
		case PC_SOLDIER:
			_SetSoldierSpawnWeapons(client);
			break;
		case PC_MEDIC:
			_SetMedicSpawnWeapons(client);
			break;
		case PC_ENGINEER:
			_SetEngineerSpawnWeapons(client);
			break;
		case PC_FIELDOPS:
			_SetFieldOpSpawnWeapons(client);
			break;
		case PC_COVERTOPS:
			_SetCovertSpawnWeapons(client);
			break;
	}

	G_AddClassSpecificTools(client);
}
#endif
int G_CountTeamMedics( team_t team, qboolean alivecheck ) {
	int numMedics = 0;
	int i, j;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if( level.clients[j].sess.playerType != PC_MEDIC ) {
			continue;
		}

		if( alivecheck ) {
			if( g_entities[j].health <= 0 ) {
				continue;
			}

			if( level.clients[j].ps.pm_type == PM_DEAD || level.clients[j].ps.pm_flags & PMF_LIMBO ) {
				continue;
			}
		}

		numMedics++;
	}

	return numMedics;
}

//
// AddMedicTeamBonus
//
void AddMedicTeamBonus( gclient_t *client ) {
	int numMedics = G_CountTeamMedics( client->sess.sessionTeam, qfalse );

	// compute health mod
	client->pers.maxHealth = 100 + 10 * numMedics;

	if( client->pers.maxHealth > 125 ) {
		client->pers.maxHealth = 125;
	}

	if( client->sess.skill[SK_BATTLE_SENSE] >= 3 ) {
		client->pers.maxHealth += 15;
	}

	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
}

/*
===========
ClientCheckName
============
*/
/*static*/ void ClientCleanName( const char *in, char *out, int outSize )
{
	int		len, colorlessLen;
	char	ch;
	char	*p;
	int		spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while( 1 ) {
		ch = *in++;
		if( !ch ) {
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' ) {
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE ) {
			// solo trailing carat is not a color prefix
			if( !*in ) {
				break;
			}

			// don't allow black in a name, period
/*			if( ColorIndex(*in) == 0 ) {
				in++;
				continue;
			}
*/
			// make sure room in dest for both chars
			if( len > outSize - 2 ) {
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' ) {
			spaces++;
			if( spaces > 3 ) {
				continue;
			}
		}
		else {
			spaces = 0;
		}

		if( len > outSize - 1 ) {
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}
	*out = 0;

	// don't allow empty names
	if( *p == 0 || colorlessLen == 0 ) {
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

void G_StartPlayerAppropriateSound(gentity_t *ent, char *soundType) {
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum ) {
	gentity_t	*ent;
	char		*s;
	char		oldname[MAX_STRING_CHARS];
	char		userinfo[MAX_INFO_STRING];
	gclient_t	*client;
	int			i;
	char		skillStr[16] = "";
	char		medalStr[16] = "";
	int			characterIndex;
	qboolean	muted=qfalse;

	ent = g_entities + clientNum;
	client = ent->client;
	client->ps.clientNum = clientNum;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ));

	if ( G_shrubbot_mute_check( userinfo ) || ent->client->sess.muted )
		muted = qtrue;

	client->medals = 0;
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		client->medals += client->sess.medals[ i ];
	}


	// check for malformed or illegal info strings
	if ( !Info_Validate(userinfo) ) {
		Q_strncpyz( userinfo, "\\name\\badinfo", sizeof(userinfo) );
	}

#ifndef DEBUG_STATS
	if( g_developer.integer /*|| *g_log.string*/ || g_dedicated.integer ) 
#endif
	{
		G_Printf("Userinfo: %s\n", userinfo);
	}

	// check for local client
	s = Info_ValueForKey( userinfo, "ip" );
	if ( s && !strcmp( s, "localhost" ) ) {
		client->pers.localClient = qtrue;
		level.fLocalHost = qtrue;
		client->sess.referee = RL_REFEREE;
	}

	// OSP - extra client info settings
	//		 FIXME: move other userinfo flag settings in here
	if(ent->r.svFlags & SVF_BOT) {
		client->pers.autoActivate = PICKUP_TOUCH;
		client->pers.bAutoReloadAux = qtrue;
		client->pmext.bAutoReload = qtrue;
		client->pers.predictItemPickup = qfalse;
		client->pers.pmoveFixed = qfalse;
		client->pers.pmoveMsec = 8;
	} else {
		s = Info_ValueForKey(userinfo, "cg_uinfo");		
		if(s != NULL && Q_stricmp(s, "")) {
			sscanf(s, "%i %i %i",
								&client->pers.clientFlags,
								&client->pers.clientTimeNudge,
								&client->pers.clientMaxPackets);
		} else {
			// cg_uinfo was empty and useless.
			// Let us fill these values in with something useful,
			// if there not present to prevent auto reload 
			// from failing. - forty
			if(!client->pers.clientFlags) client->pers.clientFlags = 13;
			if(!client->pers.clientTimeNudge) client->pers.clientTimeNudge = 0;
			if(!client->pers.clientMaxPackets) client->pers.clientMaxPackets = 30;
		}

		client->pers.autoActivate = (client->pers.clientFlags & CGF_AUTOACTIVATE) ? PICKUP_TOUCH : PICKUP_ACTIVATE;
		client->pers.predictItemPickup = ((client->pers.clientFlags & CGF_PREDICTITEMS) != 0);

		if(client->pers.clientFlags & CGF_AUTORELOAD) {
			client->pers.bAutoReloadAux = qtrue;
			client->pmext.bAutoReload = qtrue;
		} else {
			client->pers.bAutoReloadAux = qfalse;
			client->pmext.bAutoReload = qfalse;
		}
	}


	// jaquboss - pmove_fixed stuff
	s = Info_ValueForKey(userinfo, "pmove_fixed");
	if(*s) client->pers.pmoveFixed = atoi(s);
	else client->pers.pmoveFixed = 0;

	s = Info_ValueForKey(userinfo, "pmove_msec");
	if(*s) client->pers.pmoveMsec = atoi(s);
	else client->pers.pmoveMsec = 8;
	// paranoia
	if (  client->pers.pmoveMsec > 33 )
		client->pers.pmoveMsec = 33;
	if ( client->pers.pmoveMsec < 3 )
		client->pers.pmoveMsec = 3;

	// set name
	Q_strncpyz( oldname, client->pers.netname, sizeof( oldname ) );
	if ( !muted || !oldname || strlen(client->pers.netname) < 1  ){
		s = Info_ValueForKey (userinfo, "name");
		ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname) );
	} else if ( muted ) { // reset the name if muted
		Info_SetValueForKey( userinfo, "name", oldname);
		ClientCleanName( oldname, client->pers.netname, sizeof(client->pers.netname) );
	}

	if (  G_shrubbot_mute_check( userinfo ) && Q_stricmp( G_shrubbot_mute_name( userinfo ), client->pers.netname ) != 0 )
	{
		Info_SetValueForKey( userinfo, "name",  G_shrubbot_mute_name( userinfo ));
		ClientCleanName(  G_shrubbot_mute_name( userinfo ), client->pers.netname, sizeof(client->pers.netname) );
	}

	if ( client->pers.connected == CON_CONNECTED )
	{
		// jet Pilot - admins with '6' can rename without it being broadcast to everyone
		if ( strcmp( oldname, client->pers.netname ) && !G_shrubbot_permission(ent, SBF_UNACCOUNTABLE) ) 
		{
			trap_SendServerCommand( -1, va("print \"[lof]%s" S_COLOR_WHITE " [lon]renamed to[lof] %s\n\"", oldname, 
				client->pers.netname) );
		}
	}

	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		Q_strcat( skillStr, sizeof(skillStr), va("%i",client->sess.skill[i]) );
		Q_strcat( medalStr, sizeof(medalStr), va("%i",client->sess.medals[i]) );
		// FIXME: Gordon: wont this break if medals > 9 arnout? JK: Medal count is tied to skill count :() Gordon: er, it's based on >> skill per map, so for a huuuuuuge campaign it could break...
	}

	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	// check for custom character
	s = Info_ValueForKey( userinfo, "ch" );
	if( *s ) {
		characterIndex = atoi(s);
	} else {
		characterIndex = -1;
	}

	// To communicate it to cgame
	client->ps.stats[ STAT_PLAYER_CLASS ] = client->sess.playerType;
	// Gordon: Not needed any more as it's in clientinfo?

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
#ifndef NO_BOT_SUPPORT
	if ( ent->r.svFlags & SVF_BOT ) {
		// n: netname
		// t: sessionTeam
		// c1: color
		// hc: maxHealth
		// skill: skill
		// c: playerType (class?)
		// r: rank
		// f: fireteam
		// bot: botSlotNumber
		// nwp: noWeapon
		// m: medals
		// ch: character

		s = va( "n\\%s\\t\\%i\\skill\\%s\\c\\%i\\r\\%i\\m\\%s\\s\\%s%s\\dn\\%s\\dr\\%i\\w\\%i\\lw\\%i\\sw\\%i\\mu\\%i\\rn\\%i",
			client->pers.netname,
			client->sess.sessionTeam, 
			Info_ValueForKey( userinfo, "skill" ), 
			client->sess.playerType,
			client->sess.rank,
			medalStr,
            skillStr,
			characterIndex >= 0 ? va( "\\ch\\%i", characterIndex ) : "",
			client->disguiseNetname,
			client->disguiseRank,
			client->sess.playerWeapon,
			client->sess.latchPlayerWeapon,
			client->sess.latchPlayerWeapon2,
			client->sess.muted ? 1 : 0,
			client->sess.rifleNade
		);
	} else {
#endif
		s = va( "n\\%s\\t\\%i\\c\\%i\\r\\%i\\m\\%s\\s\\%s\\dn\\%s\\dr\\%i\\w\\%i\\lw\\%i\\sw\\%i\\mu\\%i\\ref\\%i\\rn\\%i",
			client->pers.netname, 
			client->sess.sessionTeam, 
			client->sess.playerType, 
			client->sess.rank, 
			medalStr,
			skillStr,
			client->disguiseNetname,
			client->disguiseRank,
			client->sess.playerWeapon,
			client->sess.latchPlayerWeapon,
			client->sess.latchPlayerWeapon2,
			client->sess.muted ? 1 : 0,
			client->sess.referee,
			client->sess.rifleNade
		);
#ifndef NO_BOT_SUPPORT
	}
#endif

	trap_GetConfigstring( CS_PLAYERS + clientNum, oldname, sizeof( oldname ) );

	trap_SetConfigstring( CS_PLAYERS + clientNum, s );

	if( !Q_stricmp( oldname, s ) ) {
		return;
	}

	if (g_logOptions.integer & LOGOPTS_GUID) // Log the GUIDs?
	{
		char *guid = Info_ValueForKey ( userinfo, "cl_guid" );
		G_LogPrintf( "ClientUserinfoChangedGUID: %i %s %s\n", clientNum, guid, s );
		G_DPrintf( "ClientUserinfoChangedGUID: %i :: %s %s\n", clientNum, guid, s );
	} else {
		G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
		G_DPrintf( "ClientUserinfoChanged: %i :: %s\n", clientNum, s );
	}
}
#ifdef __SSYSTEM__
void ClientPCinfoChanged( int clientNum ){
	gentity_t	*ent;
	char		*s;
	gclient_t	*client;

	G_Printf( "PCINFO for player with index %i sent \n", clientNum );  

	ent = g_entities + clientNum;
	client = ent->client;
//			 0		 1		 2		3	   4	  5		   6	   7        8       9       10       11      12     13      14   
	s = va( "bd\\%i\\hd\\%i\\f\\%i\\t\\%i\\l\\%i\\bc\\%i\\bcs\\%i\\bl\\%i\\bls\\%i\\br\\%i\\brs\\%i\\he\\%i\\hs\\%i\\g\\%i\\gs\\%i\\",
		client->pers.pc.bodyIndex,			//	0
		client->pers.pc.headIndex,			//	1
		client->pers.pc.faceIndex,			//	2
		client->pers.pc.torsoIndex,			//	3
		client->pers.pc.legIndex,			//	4
		client->pers.pc.backPackIndex,		//	5
		client->pers.pc.backPackSkinIndex,	//	6
		client->pers.pc.bLeftIndex,			//	7
		client->pers.pc.bLeftSkinIndex,		//	8
		client->pers.pc.bRightIndex,		//	9
		client->pers.pc.bRightSkinIndex,	//	10
		client->pers.pc.helmetIndex,		//	11
		client->pers.pc.helmetSkinIndex,	//	12
		client->pers.pc.glassIndex,			//	13
		client->pers.pc.glassSkinIndex		//	14
	);

	trap_SetConfigstring( CS_PCINFO + clientNum, s );
}
#endif


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot ) {
	char		*value;
	gclient_t	*client;
	char		userinfo[MAX_INFO_STRING];
	char		userinfo2[MAX_INFO_STRING];
	char		reason[MAX_STRING_CHARS] = "";
	gentity_t	*ent;
	int		i;
      	int	 	clientNum2;
	char		guid[33];


	ent = &g_entities[ clientNum ];

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// IP filtering
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
	// recommanding PB based IP / GUID banning, the builtin system is pretty limited
	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	if ( G_FilterIPBanPacket( value ) ) {
		return "You are banned from this server.";
	}

	// josh: censor names
	if (g_censorNames.string[0]) { 
		char censoredName[MAX_NETNAME];
		char name[MAX_NETNAME];
		value = Info_ValueForKey (userinfo, "name");
		Q_strncpyz( name, value, sizeof(name) );
		SanitizeString(name, censoredName, qtrue);
		if (G_CensorText(censoredName,&censorNamesDictionary)) {
			Info_SetValueForKey( userinfo, "name", censoredName);
			trap_SetUserinfo( clientNum, userinfo );
			value = Info_ValueForKey (userinfo, "name");
			if (g_censorPenalty.integer & CNSRPNLTY_KICK) { 
				return "Name censor: Please change your name.";
			}
		}
	}



	// check for shrubbot ban
	if(G_shrubbot_ban_check(userinfo, reason)) 
		return "You are banned from this server.";
	

	// Xian - check for max lives enforcement ban
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_enforcemaxlives.integer && (g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0) ) {
			if( trap_Cvar_VariableIntegerValue( "sv_punkbuster" ) ) {
				value = Info_ValueForKey ( userinfo, "cl_guid" );
				if ( G_FilterMaxLivesPacket ( value ) ) {
					return "Max Lives Enforcement Temp Ban. You will be able to reconnect when the next round starts. This ban is enforced to ensure you don't reconnect to get additional lives.";
				}
			} else {
				value = Info_ValueForKey ( userinfo, "ip" );	// this isn't really needed, oh well.
				if ( G_FilterMaxLivesIPPacket ( value ) ) {
					return "Max Lives Enforcement Temp Ban. You will be able to reconnect when the next round starts. This ban is enforced to ensure you don't reconnect to get additional lives.";
				}
			}
		}
	}
	// End Xian
	
	// we don't check password for bots and local client
	// NOTE: local client <-> "ip" "localhost"
	//   this means this client is not running in our current process
	if ( !isBot && !( ent->r.svFlags & SVF_BOT ) && (strcmp(Info_ValueForKey ( userinfo, "ip" ), "localhost") != 0)) {
		// check for a password
		value = Info_ValueForKey (userinfo, "password");
		if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) && strcmp( g_password.string, value) != 0) {
			if( !sv_privatepassword.string[ 0 ] || strcmp( sv_privatepassword.string, value ) ) {
				return "Invalid password";
			}
		}
	}

	// Gordon: porting q3f flag bug fix
	//			If a player reconnects quickly after a disconnect, the client disconnect may never be called, thus flag can get lost in the ether
	if( ent->inuse ) {
		G_LogPrintf( "Forcing disconnect on active client: %i\n", ent-g_entities );
		// so lets just fix up anything that should happen on a disconnect
		ClientDisconnect( ent-g_entities );
	}

	// tjw: if the client has crashed, force disconnect of previous
	//      session so that the XP can be reclaimed
	// tjw: this interferes with sv_wwwDlDisconnected
	if((g_XPSave.integer & XPSF_ENABLE) &&
		firstTime &&
		(!trap_Cvar_VariableIntegerValue("sv_wwwDlDisconnected") ||
		 (g_XPSave.integer & XPSF_WIPE_DUP_GUID))) {

		value = Info_ValueForKey (userinfo, "cl_guid");
		Q_strncpyz(guid, value, sizeof(guid));
		for (i = 0; i < level.numConnectedClients; i++) {
			clientNum2 = level.sortedClients[i];
			if(clientNum == clientNum2) continue;
			trap_GetUserinfo(clientNum2,
				userinfo2,
				sizeof(userinfo2));
			value = Info_ValueForKey (userinfo2, "cl_guid");
			if(!Q_stricmp(guid, value)) { 
				G_LogPrintf( "Forcing disconnect of "
					"duplicate guid %s for client %i\n",
					guid,
					clientNum2);
			// tjw: we need to disconnect the old 
			trap_DropClient(clientNum2, va( 
					"disconnected duplicate " 
					"guid for client %i",
					clientNum2), 
					0); 
			}
		}
	}

	// they can connect
	ent->client = level.clients + clientNum;
	client = ent->client;



	memset( client, 0, sizeof(*client) );

	client->pers.connected = CON_CONNECTING;
	client->pers.connectTime = level.time;			// DHM - Nerve

	
	// tjw: -999 indicates that no XPSave lives were found this value   
	//      will be updated in G_xpsave_load() otherwise  
	ent->client->XPSave_lives = -999;  

	// read or initialize the session data
	if( firstTime ) {
		client->pers.lastTeamChangeTime = 0;
		client->pers.initialSpawn = qtrue;				// DHM - Nerve
		G_InitSessionData( client, userinfo );
		client->pers.enterTime = level.time;
		client->ps.persistant[PERS_SCORE] = 0;
		}  
		else {  
			G_ReadSessionData( client );
	}

	// tjw: keep trying to load stored XP on every connect in case  
	//      cl_guid is not yet set for the firstTime connect  
	if(!client->XPSave_loaded)  
			client->XPSave_loaded = G_xpsave_load(ent);  


	// tjw: this should not be necessary, but there seems to be
	//      certain cases that allow new players to assume the
	//      stats of the last player that had this slot number.
	//      we're not storing wstats at all between levels now
	//      so this will need to be fixed the right way if we start.
	G_deleteStats(clientNum);
	
	if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {
		if( g_campaigns[level.currentCampaign].current == 0 || level.newCampaign ) {
			client->pers.enterTime = level.time;
		}
	} else {
		client->pers.enterTime = level.time;
	}

	if( isBot ) {
#ifndef NO_BOT_SUPPORT
		// Set up the name for the bot client before initing the bot
		value = Info_ValueForKey ( userinfo, "scriptName" );
		if (value && value[0]) {
			Q_strncpyz( client->pers.botScriptName, value, sizeof( client->pers.botScriptName ) );
			ent->scriptName = client->pers.botScriptName;
		}
		ent->aiName = ent->scriptName;
#endif
		ent->s.number = clientNum;

		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
#ifndef NO_BOT_SUPPORT
		// if this bot is reconnecting, and they aren't supposed to respawn, then dont let it in
		if (!firstTime) {
			value = Info_ValueForKey (userinfo, "respawn");
			if (value && value[0] && (!Q_stricmp(value, "NO") || !Q_stricmp(value, "DISCONNECT"))) {
				return "BotConnectFailed (no respawn)";
			}
		}

		if( !G_BotConnect( clientNum, !firstTime ) ) {
			return "BotConnectfailed";
		}
#endif
	}
	else if( g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER ) {
		// RF, in single player, enforce team = ALLIES
		// Arnout: disabled this for savegames as the double ClientBegin it causes wipes out all loaded data
		if( saveGamePending != 2 )
			client->sess.sessionTeam = TEAM_ALLIES;
			client->sess.spectatorState = SPECTATOR_NOT;
			client->sess.spectatorClient = 0;
	} else if( firstTime ) {
		// force into spectator
		client->sess.sessionTeam = TEAM_SPECTATOR;
		client->sess.spectatorState = SPECTATOR_FREE;
		client->sess.spectatorClient = 0;

		// unlink the entity - just in case they were already connected
		trap_UnlinkEntity( ent );
	}

#ifdef OMNIBOTS
	// We send these with a 1 ms delay because this is called during the addbot, and
	// if there's no delay the bot will create a bot as a result of this message
	// which will not honor the class and team requested by the user.
	if (isBot)
	{
		Bot_Interface_SendGlobalEvent(GAME_ID_BOTCONNECTED, clientNum, 0, NULL);
	}
	else
	{
		Bot_Interface_SendGlobalEvent(GAME_ID_CLIENTCONNECTED, clientNum, 0, NULL);
	}
#endif

	// get and distribute relevent paramters
	G_LogPrintf( "ClientConnect: %i\n", clientNum );
	G_UpdateCharacter( client );
	ClientUserinfoChanged( clientNum );

	if (g_gametype.integer == GT_SINGLE_PLAYER) {
#ifndef NO_BOT_SUPPORT
		if (!isBot) {
#endif
			ent->scriptName = "player";

// START	Mad Doctor I changes, 8/14/2002
			// We must store this here, so that BotFindEntityForName can find the
			// player.
			ent->aiName = "player";
// END		Mad Doctor I changes, 8/12/2002

			G_Script_ScriptParse( ent );
			G_Script_ScriptEvent( ent, "spawn", "" );
#ifndef NO_BOT_SUPPORT
		}
#endif

	}


	// don't do the "xxx connected" messages if they were caried over from previous level
	//		TAT 12/10/2002 - Don't display connected messages in single player
	if ( firstTime && !G_IsSinglePlayerGame())
	{
		trap_SendServerCommand( -1, va("cpm \"%s" S_COLOR_WHITE " connected\n\"", client->pers.netname) );
	}

	// count current clients and rank for scoreboard
	CalculateRanks();
	client->ps.persistant[PERS_KILLS]=0;

	return NULL;
}

//
// Scaling for late-joiners of maxlives based on current game time
//
int G_ComputeMaxLives(gclient_t *cl, int maxRespawns)
{
	float scaled;
	int val;

	// rain - #102 - don't scale of the timelimit is 0
	if (g_timelimit.value == 0.0) {
		return maxRespawns - 1;
	}

	// tjw: do not give out lives in dual obj maps once time runs out.
	if((float)(level.time - level.startTime) >=
		(g_timelimit.value * 60000.0f)) {

		return -1;
	}

	scaled = (float)(maxRespawns - 1) * (1.0f - ((float)(level.time - level.startTime) / (g_timelimit.value * 60000.0f)));
	val = (int)scaled;

	val += ((scaled - (float)val) < 0.5f) ? 0 : 1;

	if((g_XPSave.integer & XPSF_ENABLE) && 
		cl->XPSave_lives != -999 &&
		cl->XPSave_lives < val) {

		val = cl->XPSave_lives;
	}

	return(val);
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum )
{
	gentity_t	*ent;
	gclient_t	*client;
	int			flags;
	int			spawn_count, lives_left;		// DHM - Nerve
	int			health;
	qboolean	maxlives;

	ent = g_entities + clientNum;

	client = level.clients + clientNum;

	health = ent->health;

	maxlives = (g_maxlives.integer || 
		g_alliedmaxlives.integer || 
		g_axismaxlives.integer);

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	// tjw: first ClientBegin() call for the client
	if(client->pers.connected == CON_CONNECTING) {
		int i;

		// tjw: send forcecvar commands if there are any
		for(i = 0; i < level.forceCvarCount; i++) {
			CP(va("forcecvar \"%s\" \"%s\"",
				level.forceCvars[i][0],
				level.forceCvars[i][1]));
		}
	}

	client->pers.connected = CON_CONNECTED;
	client->pers.teamState.state = TEAM_BEGIN;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	// DHM - Nerve :: Also save PERS_SPAWN_COUNT, so that CG_Respawn happens
	spawn_count = client->ps.persistant[PERS_SPAWN_COUNT];
	//bani - proper fix for #328
	if( client->ps.persistant[PERS_RESPAWNS_LEFT] > 0 ) {
		lives_left = client->ps.persistant[PERS_RESPAWNS_LEFT] - 1;
	} else {
		lives_left = client->ps.persistant[PERS_RESPAWNS_LEFT];
	}
	flags = client->ps.eFlags;
	memset( &client->ps, 0, sizeof( client->ps ) );
	
	// tjw: if playerState is cleared this should be too right?
	memset( &client->pmext, 0, sizeof( client->pmext ) );
	
 
	// tjw: reset all the markers used by g_shortcuts  
	client->pers.lastkilled_client = -1;  
	client->pers.lastammo_client = -1;  
	client->pers.lasthealth_client = -1;  
	client->pers.lastrevive_client = -1;  
	client->pers.lastkiller_client = -1;  


	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawn_count;
	if(!maxlives)
		client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
	else
		client->ps.persistant[PERS_RESPAWNS_LEFT] = lives_left;
	

	client->pers.complaintClient = -1;
	client->pers.complaintEndTime = -1;

	// tjw: even if g_teamChangeKills is 0, kill them if they try to
	//      change teams twice in one round to prevent team change
	//      spamming.
	if(client->sess.sessionTeam == TEAM_AXIS) {
		if(level.time - client->pers.lastTeamChangeTime < g_redlimbotime.integer)
			health = 0;
	}
	if(client->sess.sessionTeam == TEAM_ALLIES) {
		if(level.time - client->pers.lastTeamChangeTime < g_bluelimbotime.integer)
			health = 0;
	}

	// Xian -- Changed below for team independant maxlives
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( ( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) ) {
		
			if( !client->maxlivescalced ) {
				if(g_maxlives.integer > 0) {
					client->ps.persistant[PERS_RESPAWNS_LEFT] = G_ComputeMaxLives(client, g_maxlives.integer);
				} else {
					client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
				}

				if( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
					if(client->sess.sessionTeam == TEAM_AXIS) {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = G_ComputeMaxLives(client, g_axismaxlives.integer);	
					} else if(client->sess.sessionTeam == TEAM_ALLIES) {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = G_ComputeMaxLives(client, g_alliedmaxlives.integer);
					} else {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
					}
 				}

				client->maxlivescalced = qtrue;
			} else {
				if( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
					if( client->sess.sessionTeam == TEAM_AXIS ) {
						if( client->ps.persistant[ PERS_RESPAWNS_LEFT ] > g_axismaxlives.integer ) {
							client->ps.persistant[ PERS_RESPAWNS_LEFT ] = g_axismaxlives.integer;
						}
					} else if( client->sess.sessionTeam == TEAM_ALLIES ) {
						if( client->ps.persistant[ PERS_RESPAWNS_LEFT ] > g_alliedmaxlives.integer ) {
							client->ps.persistant[ PERS_RESPAWNS_LEFT ] = g_alliedmaxlives.integer;
						}
					}
 				}
			}
		}
	}

	// locate ent at a spawn point
	if(!g_teamChangeKills.integer && health > 0) { 

		ClientSpawn(ent, qfalse, qtrue, qfalse);
	}
	else {
		ClientSpawn(ent, qfalse, qtrue, qtrue);
	}


	// DHM - Nerve :: Start players in limbo mode if they change teams 
	// during the match
	// tjw: don't kill the player unless they're dead already
	if(client->sess.sessionTeam != TEAM_SPECTATOR && 
		level.time - level.startTime > FRAMETIME * GAME_INIT_FRAMES) {
			
			// tjw: otherwise players lose 2 lives. 
			// tjw: wtf does gametype have to do with it?
			if(g_gametype.integer != GT_WOLF_LMS && maxlives)
				client->ps.persistant[PERS_RESPAWNS_LEFT]++;
			
			if(health <= 0 || g_teamChangeKills.integer) {
				ent->health = 0;
				ent->r.contents = CONTENTS_CORPSE;
				client->ps.pm_type = PM_DEAD;
				client->ps.stats[STAT_HEALTH] = 0;

				limbo(ent, qfalse);
			}
	}

	if(client->sess.sessionTeam != TEAM_SPECTATOR) {
		trap_SendServerCommand( -1, va("print \"[lof]%s" S_COLOR_WHITE " [lon]entered the game\n\"", client->pers.netname) );
	}

	G_LogPrintf( "ClientBegin: %i\n", clientNum );

	// Xian - Check for maxlives enforcement
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if ( g_enforcemaxlives.integer == 1 && (g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0)) {
			char *value;
			char userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
			value = Info_ValueForKey ( userinfo, "cl_guid" );
			G_LogPrintf( "EnforceMaxLives-GUID: %s\n", value );
			AddMaxLivesGUID( value );

			value = Info_ValueForKey (userinfo, "ip");
			G_LogPrintf( "EnforceMaxLives-IP: %s\n", value );
			AddMaxLivesBan( value );
		}
	}
	// End Xian

	// count current clients and rank for scoreboard
	CalculateRanks();

	// No surface determined yet.
	ent->surfaceFlags = 0;
#ifdef MV_SUPPORT
	// OSP
	G_smvUpdateClientCSList(ent);
	// OSP
#endif
}

gentity_t *SelectSpawnPointFromList( char *list, vec3_t spawn_origin, vec3_t spawn_angles )
{
	char *pStr, *token;
	gentity_t	*spawnPoint=NULL, *trav;
	#define	MAX_SPAWNPOINTFROMLIST_POINTS	16
	int	valid[MAX_SPAWNPOINTFROMLIST_POINTS];
	int numValid;

	memset( valid, 0, sizeof(valid) );
	numValid = 0;

	pStr = list;
	while((token = COM_Parse( &pStr )) != NULL && token[0]) {
		trav = g_entities + level.maxclients;
		while((trav = G_FindByTargetname(trav, token)) != NULL) {
			if (!spawnPoint) spawnPoint = trav;
			if (!SpotWouldTelefrag( trav )) {
				valid[numValid++] = trav->s.number;
				if (numValid >= MAX_SPAWNPOINTFROMLIST_POINTS) {
					break;
				}
			}
		}
	}

	if (numValid)
	{
		spawnPoint = &g_entities[valid[rand()%numValid]];

		// Set the origin of where the bot will spawn
		VectorCopy (spawnPoint->r.currentOrigin, spawn_origin);
		spawn_origin[2] += 9;

		// Set the angle we'll spawn in to
		VectorCopy (spawnPoint->s.angles, spawn_angles);
	}

	return spawnPoint;
}


// TAT 1/14/2003 - init the bot's movement autonomy pos to it's current position
void BotInitMovementAutonomyPos(gentity_t *bot);

#if 0 // rain - not used
static char *G_CheckVersion( gentity_t *ent )
{
	// Prevent nasty version mismatches (or people sticking in Q3Aimbot cgames)

	char userinfo[MAX_INFO_STRING];
	char *s;

	trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
	s = Info_ValueForKey( userinfo, "cg_etVersion" );
	if( !s || strcmp( s, GAME_VERSION_DATED ) )
		return( s );
	return( NULL );
}
#endif

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn(
		gentity_t *ent,
		qboolean revived,
		qboolean teamChange,
		qboolean restoreHealth)
{
	int			index;
	vec3_t		spawn_origin, spawn_angles;
	gclient_t	*client;
	int			i;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	int			persistant[MAX_PERSISTANT];
	gentity_t	*spawnPoint;
	int			flags;
	int			savedPing;
	int			savedTeam;
	int			savedSlotNumber;
	int savedClassWeaponTime;
	int savedDeathTime;

	index = ent - g_entities;
	client = ent->client;

	G_UpdateSpawnCounts();

	client->pers.lastSpawnTime = level.time;
	client->pers.lastBattleSenseBonusTime = level.timeCurrent;
	client->pers.lastHQMineReportTime = level.timeCurrent;



/*#ifndef _DEBUG
	if( !client->sess.versionOK ) {
		char *clientMismatchedVersion = G_CheckVersion( ent );	// returns NULL if version is identical

		if( clientMismatchedVersion ) {
			trap_DropClient( ent - g_entities, va( "Client/Server game mismatch: '%s/%s'", clientMismatchedVersion, GAME_VERSION_DATED ) );
		} else {
			client->sess.versionOK = qtrue;
		}
	}
#endif*/

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if( revived ) {
		spawnPoint = ent;
		VectorCopy( ent->r.currentOrigin, spawn_origin );
		spawn_origin[2] += 9;	// spawns seem to be sunk into ground?
		VectorCopy( ent->s.angles, spawn_angles );
	} else {
		// Arnout: let's just be sure it does the right thing at all times. (well maybe not the right thing, but at least not the bad thing!)
		//if( client->sess.sessionTeam == TEAM_SPECTATOR || client->sess.sessionTeam == TEAM_FREE ) {
		if( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES ) {
			spawnPoint = SelectSpectatorSpawnPoint( spawn_origin, spawn_angles );
		} else {
			// RF, if we have requested a specific spawn point, use it (fixme: what if this will place us inside another character?)
/*			spawnPoint = NULL;
			trap_GetUserinfo( ent->s.number, userinfo, sizeof(userinfo) );
			if( (str = Info_ValueForKey( userinfo, "spawnPoint" )) != NULL && str[0] ) {
				spawnPoint = SelectSpawnPointFromList( str, spawn_origin, spawn_angles );
				if (!spawnPoint) {
					G_Printf( "WARNING: unable to find spawn point \"%s\" for bot \"%s\"\n", str, ent->aiName );
				}
			}
			//
			if( !spawnPoint ) {*/
				spawnPoint = SelectCTFSpawnPoint( client->sess.sessionTeam, client->pers.teamState.state, spawn_origin, spawn_angles, client->sess.spawnObjectiveIndex );
//			}
		}
	}

	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	flags = ent->client->ps.eFlags & EF_TELEPORT_BIT;
	flags ^= EF_TELEPORT_BIT;
//unlagged reset history markers
	G_ResetMarkers(ent); 
//unlagged
	flags |= (client->ps.eFlags & EF_VOTED);
	// clear everything but the persistant data

	ent->s.eFlags &= ~EF_MOUNTEDTANK;

	saved			= client->pers;
	savedSess		= client->sess;
	savedPing		= client->ps.ping;
	savedTeam		= client->ps.teamNum;
	// START	xkan, 8/27/2002
	savedSlotNumber	= client->botSlotNumber;
	savedClassWeaponTime = client->ps.classWeaponTime;
	savedDeathTime = client->deathTime;
	// END		xkan, 8/27/2002

	for( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		persistant[i] = client->ps.persistant[i];
	}

	{
		qboolean set = client->maxlivescalced;
		int XPSave_lives = client->XPSave_lives;

		memset( client, 0, sizeof(*client) );

		client->maxlivescalced = set;
		client->XPSave_lives = XPSave_lives;
	}

	client->pers			= saved;
	client->sess			= savedSess;
	client->ps.ping			= savedPing;
	client->ps.teamNum		= savedTeam;
	// START	xkan, 8/27/2002
	client->botSlotNumber	= savedSlotNumber;
	// END		xkan, 8/27/2002

	for( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		client->ps.persistant[i] = persistant[i];
	}

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	if( revived ) {
		client->ps.persistant[PERS_REVIVE_COUNT]++;
	}
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;
	client->ps.persistant[PERS_HWEAPON_USE] = 0;

	if ( client->sess.skill[SK_BATTLE_SENSE] >= 7 )
	client->airOutTime = level.time + HOLDBREATHTIME_LONG;
	else
	client->airOutTime = level.time + HOLDBREATHTIME;

	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	client->ps.eFlags = flags;

	// tjw: these are saved in case of SLASHKILL_SAMECHARGE
	client->ps.classWeaponTime = savedClassWeaponTime;
	client->deathTime = savedDeathTime;

	// MrE: use capsules for AI and player
	//client->ps.eFlags |= EF_CAPSULE;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
#ifndef NO_BOT_SUPPORT
	if( ent->r.svFlags & SVF_BOT )
		ent->classname = "bot";
	else
#endif
		ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;

	ent->clipmask = MASK_PLAYERSOLID;

	// DHM - Nerve :: Init to -1 on first spawn;
	if ( !revived )
		ent->props_frame_state = -1;

	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;
	
	VectorCopy( playerMins, ent->r.mins );
	VectorCopy( playerMaxs, ent->r.maxs );

	// Ridah, setup the bounding boxes and viewheights for prediction
	VectorCopy( ent->r.mins, client->ps.mins );
	VectorCopy( ent->r.maxs, client->ps.maxs );
	
	client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
	client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
	client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
	
	//client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
	client->ps.crouchMaxZ = CROUCH_VIEWHEIGHT; 

	client->ps.runSpeedScale = 0.8;
	client->ps.sprintSpeedScale = 1.1;
	client->ps.crouchSpeedScale = 0.25;
	client->ps.weaponstate = WEAPON_READY;

	// Rafael
	client->pmext.sprintTime = SPRINTTIME;
	client->ps.sprintExertTime = 0;

	client->ps.friction = 1.0;
	// done.

	// TTimo
	// retrieve from the persistant storage (we use this in pmoveExt_t beause we need it in bg_*)
	client->pmext.bAutoReload = client->pers.bAutoReloadAux;
	// done

	client->ps.clientNum = index;

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );	// NERVE - SMF - moved this up here

	// DHM - Nerve :: Add appropriate weapons
	if ( !revived ) {
		qboolean update = qfalse;

		if( client->sess.playerType != client->sess.latchPlayerType )
			update = qtrue;

		//if ( update || client->sess.playerWeapon != client->sess.latchPlayerWeapon) {
		//	G_ExplodeMines(ent);
		//}

		client->sess.playerType = client->sess.latchPlayerType;


		if( G_IsWeaponDisabled( ent, client->sess.latchPlayerWeapon, qtrue ) ) {
			bg_playerclass_t* classInfo = BG_PlayerClassForPlayerState( &ent->client->ps );
			client->sess.latchPlayerWeapon = classInfo->classWeapons[0];
			update = qtrue;
		}

		if( client->sess.playerWeapon != client->sess.latchPlayerWeapon ) {
			client->sess.playerWeapon = client->sess.latchPlayerWeapon;
			update = qtrue;
		}

		if( G_IsWeaponDisabled( ent, client->sess.playerWeapon, qtrue ) ) {
			bg_playerclass_t* classInfo = BG_PlayerClassForPlayerState( &ent->client->ps );
			client->sess.playerWeapon = classInfo->classWeapons[0];
			update = qtrue;
		}

		client->sess.playerWeapon2 = client->sess.latchPlayerWeapon2;

		if( update || teamChange ) {
			ClientUserinfoChanged( index );
		}

#ifdef __SSYSTEM__
		ent->client->pers.pc = ent->client->pers.latchedpc;
		if ( ent->client->pers.pcLatched ){
			ent->client->pers.pcLatched = qfalse;
			ClientPCinfoChanged( index );
		}
#endif

	}

	// TTimo keep it isolated from spectator to be safe still
	if( client->sess.sessionTeam != TEAM_SPECTATOR ) {
		// Xian - Moved the invul. stuff out of SetWolfSpawnWeapons and put it here for clarity
		if ( g_fastres.integer == 1 && revived )
			client->ps.powerups[PW_INVULNERABLE] = level.time + 1000; 
		else if(revived) 
			client->ps.powerups[PW_INVULNERABLE] = level.time + 3000; 
		else {
			if(client->sess.sessionTeam == TEAM_AXIS) {
				if(g_axisSpawnInvul.integer) {
					client->ps.powerups[PW_INVULNERABLE] = 
						level.time + 
						(g_axisSpawnInvul.integer *
						 	1000);
				} else {
					client->ps.powerups[PW_INVULNERABLE] = 
						level.time + 
						(g_spawnInvul.integer *
						 	1000); 
				}
			} else if (client->sess.sessionTeam == TEAM_ALLIES) {
				if(g_alliedSpawnInvul.integer) {
					client->ps.powerups[PW_INVULNERABLE] = 
						level.time + 
						(g_alliedSpawnInvul.integer *
						 	1000); 
				} else {
					client->ps.powerups[PW_INVULNERABLE] = 
						level.time + 
						(g_spawnInvul.integer *
						 	1000); 
				}
			} else {			
				client->ps.powerups[PW_INVULNERABLE] = 
					level.time + 
					(g_spawnInvul.integer * 1000); 
			}
		}
	}
	// End Xian

	G_UpdateCharacter( client );

#ifdef OMNIBOTS
	// important to do this before weapons are added to the bot
	if (ent->r.svFlags & SVF_BOT)
	{
		// Send the respawn event.
		if(revived)
		{
			Bot_Interface_SendEvent(ET_EVENT_REVIVED, client->ps.clientNum, 0, 0, 0);
		}
		else
		{
			Bot_Interface_SendEvent(MESSAGE_SPAWN, client->ps.clientNum, 0, 0, 0);
		}

		botUserData.DataType = dtInt;
		botUserData.udata.m_Int = Bot_PlayerClassGameToBot(ent->client->sess.playerType);
		Bot_Interface_SendEvent(MESSAGE_CHANGECLASS, client->ps.clientNum, 0,0, &botUserData);
		botUserData.DataType = dtInt;
		botUserData.udata.m_Int = Bot_TeamGameToBot(ent->client->sess.sessionTeam);
		Bot_Interface_SendEvent(MESSAGE_CHANGETEAM, client->ps.clientNum, 0,0, &botUserData);
	}
#endif

	SetWolfSpawnWeapons( client ); 
	
	// START	Mad Doctor I changes, 8/17/2002

	// JPW NERVE -- increases stats[STAT_MAX_HEALTH] based on # of medics in game
	AddMedicTeamBonus( client );

	// END		Mad Doctor I changes, 8/17/2002

	if( !revived ) {
		client->pers.cmd.weapon = ent->client->ps.weapon;
	}
// dhm - end

	// JPW NERVE ***NOTE*** the following line is order-dependent and must *FOLLOW* SetWolfSpawnWeapons() in multiplayer
	// AddMedicTeamBonus() now adds medic team bonus and stores in ps.stats[STAT_MAX_HEALTH].

	if(restoreHealth) {
		if( client->sess.skill[SK_BATTLE_SENSE] >= 3 )
			// We get some extra max health, but don't spawn with that much
			ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH] - 15;
		else
			ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH];
	}
	else {
		client->ps.stats[STAT_HEALTH] = ent->health;
	}

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;

	if( !revived ) {
		SetClientViewAngle( ent, spawn_angles );
	} else {
		//bani - #245 - we try to orient them in the freelook direction when revived
		vec3_t	newangle;

		newangle[YAW] = SHORT2ANGLE( ent->client->pers.cmd.angles[YAW] + ent->client->ps.delta_angles[YAW] );
		newangle[PITCH] = 0;
		newangle[ROLL] = 0;

		SetClientViewAngle( ent, newangle );
	}
#ifndef NO_BOT_SUPPORT
	if( ent->r.svFlags & SVF_BOT ) {
		// xkan, 10/11/2002 - the ideal view angle is defaulted to 0,0,0, but the 
		// spawn_angles is the desired angle for the bots to face.
		BotSetIdealViewAngles( index, spawn_angles );

		// TAT 1/14/2003 - now that we have our position in the world, init our autonomy positions
		BotInitMovementAutonomyPos(ent);
	}
#endif

	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		//G_KillBox( ent );
		trap_LinkEntity (ent);
	}

	client->respawnTime = level.timeCurrent;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;
	client->latched_wbuttons = 0;	//----(SA)	added

	// xkan, 1/13/2003 - reset death time
	client->deathTime = 0;

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		// fire the targets of the spawn point
		if ( !revived )
			G_UseTargets( spawnPoint, ent );
	}

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, level.time, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// set idle animation on weapon
	ent->client->ps.weapAnim = ( ( ent->client->ps.weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | PM_IdleAnimForWeapon( ent->client->ps.weapon );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, level.time, qtrue );

	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
	G_ResetMarkers( ent );

	// Set up bot speed bonusses
	BotSpeedBonus( ent->s.number );

	// RF, start the scripting system
	if (!revived && client->sess.sessionTeam != TEAM_SPECTATOR) {
		Bot_ScriptInitBot( ent->s.number );
		//
		if (spawnPoint && spawnPoint->targetname) {
			Bot_ScriptEvent( ent->s.number, "spawn", spawnPoint->targetname );
		} else {
			Bot_ScriptEvent( ent->s.number, "spawn", "" );
		}
		// RF, call entity scripting event
		G_Script_ScriptEvent( ent, "playerstart", "" );
	} 
#ifndef NO_BOT_SUPPORT
	else if( revived && ent->r.svFlags & SVF_BOT) {
		Bot_ScriptEvent( ent->s.number, "revived", "" );
	}
#endif

}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum ) {
	gentity_t	*ent;
	gentity_t	*flag=NULL;
	gitem_t		*item=NULL;
	vec3_t		launchvel;
	int			i;

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;
	}
	
	G_xpsave_add(ent);
		
	G_RemoveClientFromFireteams( clientNum, qtrue, qfalse );
	G_RemoveFromAllIgnoreLists( clientNum );
	G_LeaveTank( ent, qfalse );

	// stop any following clients
	for ( i = 0 ; i < level.numConnectedClients ; i++ ) {
		flag = g_entities + level.sortedClients[i];
		if ( flag->client->sess.sessionTeam == TEAM_SPECTATOR
			&& flag->client->sess.spectatorState == SPECTATOR_FOLLOW
			&& flag->client->sess.spectatorClient == clientNum ) {
			StopFollowing( flag );
		}
		if ( flag->client->ps.pm_flags & PMF_LIMBO
			&& flag->client->sess.spectatorClient == clientNum ) {
			Cmd_FollowCycle_f( flag, 1 );
		}
		// NERVE - SMF - remove complaint client
		if ( flag->client->pers.complaintEndTime > level.time && flag->client->pers.complaintClient == clientNum ) {
			flag->client->pers.complaintClient = -1;
			flag->client->pers.complaintEndTime = -1;

			CPx( level.sortedClients[i], "complaint -2" );
		}
	}

	if( g_landminetimeout.integer ) {
		G_ExplodeMines(ent);
	}
	G_FadeItems(ent, MOD_SATCHEL);
	G_FadeItems(ent, MOD_AIRSTRIKE);  
	G_FadeItems(ent, MOD_ARTY);  

	// jaquboss
	if ( jp_insanity.integer & JP_INSANITY_MORTARCAM )
		G_FadeItems(ent, MOD_MORTAR);

	if (jp_insanity.integer & JP_INSANITY_MISSILECAM ) {
		G_FadeItems(ent, MOD_PANZERFAUST);
		G_FadeItems(ent, MOD_BAZOOKA);
		G_FadeItems(ent, MOD_FIREBOLT);
	}

	// remove ourself from teamlists
	{
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

	// send effect if they were completely connected
	if ( ent->client->pers.connected == CON_CONNECTED 
		&& ent->client->sess.sessionTeam != TEAM_SPECTATOR
		&& !(ent->client->ps.pm_flags & PMF_LIMBO) ) {

		// They don't get to take powerups with them!
		// Especially important for stuff like CTF flags
		TossClientItems( ent );

		// New code for tossing flags
			if (ent->client->ps.powerups[PW_REDFLAG]) {
				item = BG_FindItem("Red Flag");
				if (!item)
					item = BG_FindItem("Objective");

				ent->client->ps.powerups[PW_REDFLAG] = 0;
			}
			if (ent->client->ps.powerups[PW_BLUEFLAG]) {
				item = BG_FindItem("Blue Flag");
				if (!item)
					item = BG_FindItem("Objective");

				ent->client->ps.powerups[PW_BLUEFLAG] = 0;
			}

			if( item ) {
				// OSP - fix for suicide drop exploit through walls/gates
				launchvel[0] = 0;//crandom()*20;
				launchvel[1] = 0;//crandom()*20;
				launchvel[2] = 0;//10+random()*10;

				flag = LaunchItem(item,ent->r.currentOrigin,launchvel,ent-g_entities, FL_DROPPED_ITEM);
				flag->s.modelindex2 = ent->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
				flag->message = ent->message;	// DHM - Nerve :: also restore item name
				// Clear out player's temp copies
				ent->s.otherEntityNum2 = 0;
				ent->message = NULL;
			}

		// OSP - Log stats too
		G_LogPrintf("WeaponStats: %s\n", G_createStats(ent));
	}

	G_LogPrintf( "ClientDisconnect: %i\n", clientNum );

	trap_UnlinkEntity (ent);
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	i = ent->client->sess.sessionTeam;
	ent->client->sess.sessionTeam = TEAM_FREE;
	ent->active = 0;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "");
#ifdef __SSYSTEM__
	trap_SetConfigstring( CS_PCINFO + clientNum, "");
#endif
	CalculateRanks();

	if ( ent->r.svFlags & SVF_BOT )
	{
#ifndef NO_BOT_SUPPORT
		BotAIShutdownClient( clientNum );
#endif
#ifdef OMNIBOTS
		Bot_Interface_SendGlobalEvent(GAME_ID_BOTDISCONNECTED, clientNum, 0, NULL);
#endif
	}
	else
	{
#ifdef OMNIBOTS
		Bot_Interface_SendGlobalEvent(GAME_ID_CLIENTDISCONNECTED, clientNum, 0, NULL);
#endif
	}

	// OSP
	G_verifyMatchState(i);
#ifdef MV_SUPPORT
	G_smvAllRemoveSingleClient(ent - g_entities);
#endif
	// OSP

}

// In just the GAME DLL, we want to store the groundtrace surface stuff,
// so we don't have to keep tracing.
void ClientStoreSurfaceFlags
( 
	int clientNum, 
	int surfaceFlags
)
{
	// Store the surface flags
	g_entities[clientNum].surfaceFlags = surfaceFlags;

}


// ClientHitboxMaxZ returns the proper value to use for 
// the entity's r.maxs[2] when running a trace.
float ClientHitboxMaxZ(gentity_t *hitEnt) 
{
		if(!hitEnt) return 0;
		if(!hitEnt->client) return hitEnt->r.maxs[2];

		if((g_hitboxes.integer & HBF_CORPSE) &&
			(hitEnt->client->ps.eFlags & EF_DEAD)) {
			return 4;
		}
		else if((g_hitboxes.integer & HBF_PLAYDEAD) &&
			(hitEnt->client->ps.eFlags & EF_PLAYDEAD)) {
			return 4;
		}
		else if((g_hitboxes.integer & HBF_PRONE) &&
			(hitEnt->client->ps.eFlags & EF_PRONE)) {
			return 4;
		}
		else if((g_hitboxes.integer & HBF_CROUCHING) &&
			(hitEnt->client->ps.eFlags & EF_CROUCHING)) {
			// tjw: changed this because the crouched moving
			//      animation is higher than the idle one
			//      and that should be the most commonly used
			//return 18;
			return 24; 
		}
		else if((g_hitboxes.integer & HBF_STANDING)) {
			return 36;
		}
		return hitEnt->r.maxs[2];
}

