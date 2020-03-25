/*
 * name:	g_client.c
 *
 * desc:
 *
 * NQQS:
 *
 */
#include "g_local.h"
#include "../../etmain/ui/menudef.h"
#include "etpro_mdx.h"
#include "g_strparse.h"

// *LUA*
#ifdef ET_LUA
#include "g_lua.h"
#endif

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

// g_client.c -- client functions that don't happen every frame

qboolean CompareIPNoPort(char const* ip1, char const* ip2);

// Ridah, new bounding box
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

	G_SpawnInt( "nobots", "0", &i);
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}

	ent->enemy = G_PickTarget( ent->target );
	if(ent->enemy) {
		vec3_t	dir;
		VectorSubtract( ent->enemy->s.origin, ent->s.origin, dir );
		vectoangles( dir, ent->s.angles );
	}
}

/*QUAKED info_player_checkpoint (1 0 0) (-16 -16 -24) (16 16 32) a b c d
these are start points /after/ the level start
the letter (a b c d) designates the checkpoint that needs to be complete in order to use this start position
*/
void SP_info_player_checkpoint(gentity_t *ent) {
	ent->classname = "info_player_checkpoint";
	SP_info_player_deathmatch( ent );
}

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

	for (i=0 ; i<num ; ++i) {
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
	gentity_t	*spot = NULL;
	vec3_t		delta;
	float		dist;
	float 		nearestDist = 999999;
	gentity_t	*nearestSpot = NULL;
	int i;

	for ( i=0; i<g_spawns.count; ++i ) {
		spot = g_spawns.ent[i];
		if ( !spot->inuse ) continue;
		if ( spot->classnamehash != INFO_PLAYER_DEATHMATCH_HASH ) continue;
		if ( Q_stricmp(spot->classname, "info_player_deathmatch") ) continue;

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
gentity_t *SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t	*spot = NULL;
	int			count = 0;
	int			selection;
	gentity_t	*spots[MAX_SPAWN_POINTS];
	int			i;
	gentity_t	*firstspot = NULL;

	for ( i=0; i<g_spawns.count; ++i ) {
		spot = g_spawns.ent[i];
		if ( !spot->inuse ) continue;
		if ( spot->classnamehash != INFO_PLAYER_DEATHMATCH_HASH ) continue;
		if ( Q_stricmp(spot->classname, "info_player_deathmatch") ) continue;
		if ( !firstspot ) firstspot = spot;
		if ( SpotWouldTelefrag(spot) ) continue;
		spots[ count ] = spot;
		count++;
	}

	if ( !count ) {	// no spots that won't telefrag
		return firstspot;
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
	gentity_t *nearestSpot 	= SelectNearestDeathmatchSpawnPoint( avoidPoint );
	gentity_t *spot 		= SelectRandomDeathmatchSpawnPoint ( );

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
	for (i=0; i<BODY_QUEUE_SIZE ; ++i) {
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
	gentity_t	*tent = G_TempEntity( ent->r.currentOrigin, EV_BODY_DP ); // so clients will memset them off :P

	tent->s.otherEntityNum2 = ent->s.number;
	tent->r.svFlags = SVF_BROADCAST;	// send to everyone

	trap_UnlinkEntity( ent );
	ent->physicsObject = qfalse;
#ifdef USE_BODY_QUE
	// WIP: ent->inuse = qfalse; // core: stop executing in G_RunEntity() for nothing
#endif

}

void G_BodyDP ( gentity_t *ent ){
	gentity_t	*tent = G_TempEntity( ent->r.currentOrigin, EV_BODY_DP ); // so clients will memset them off :P
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
		}
		else {
			ent->activator = NULL;
		}
	}

	BodySink2( ent );
}

static qboolean G_TestPosition( gentity_t *ent, vec3_t newOrigin ) {
	trace_t	trace;

	trap_TraceCapsule( &trace, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, newOrigin, ent->s.number, MASK_PLAYERSOLID );

	if ( trace.fraction == 1 ) {
		VectorCopy ( trace.endpos, ent->s.pos.trBase );
		return qtrue;
	}
	else {
		return qfalse;
	}
}

// note that this is only with first stage, corpse can just use slidemove
static void G_StepSlideCorpse( gentity_t *ent, vec3_t newOrigin )
{
	vec3_t		start, down, up;
	trace_t		trace;

	VectorCopy ( ent->s.pos.trBase, start);

	if ( G_TestPosition( ent, newOrigin ) ) {

		// so check if we can fall even more down
		VectorCopy ( ent->s.pos.trBase, down);
		down[2] -= 16;
		// item code is using these
		trap_Trace( &trace, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_PLAYERSOLID );
		if ( trace.fraction == 1 ){
			// begin with falling again
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
		}
		else {
			VectorCopy ( trace.endpos, ent->s.pos.trBase );
		}

		return;		// we got exactly where we wanted to go first try
	}

	VectorCopy( ent->s.pos.trBase, down );

	down[2] -= 18;

	trap_TraceCapsule( &trace, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_PLAYERSOLID );

	VectorSet(up, 0, 0, 1);
	// never step up when you still have up velocity
	if ( ent->s.pos.trDelta[2] > 0 && (trace.fraction == 1.0 || DotProduct(trace.plane.normal, up) < 0.7)) {
		return;
	}

	VectorCopy (ent->s.pos.trBase, down);

	VectorCopy (start, up);
	up[2] += 18;

	// test the player position if they were a stepheight higher
	trap_TraceCapsule( &trace, start, ent->r.mins, ent->r.maxs, up, ent->s.number, MASK_PLAYERSOLID );

	if ( trace.allsolid ) {
		return;		// can't step up
	}

	// try slidemove from this position
	VectorCopy (trace.endpos, ent->s.pos.trBase);

	G_TestPosition( ent, newOrigin );

	// push down the final amoun
	VectorCopy (ent->s.pos.trBase, down);
	down[2] -= 18;

	memset(&trace, 0, sizeof(trace));

	trap_TraceCapsule( &trace, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_PLAYERSOLID );

	if ( !trace.allsolid ) {
		VectorCopy (trace.endpos, ent->s.pos.trBase);
	}

	// so check if we can fall even more down
	if ( trace.fraction == 1.f ){
		down[2] -= 16;
		// item code is using these
		trap_Trace( &trace, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_PLAYERSOLID );
		if ( trace.fraction == 1 ){
			// begin with falling again
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
		}
		else {
			VectorCopy ( trace.endpos, ent->s.pos.trBase );
		}
	}
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/

void G_FlushItem( gentity_t *ent, trace_t *trace);

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
	// WIP: body->inuse = qtrue; // IRATA: process entity again
#else
	body = G_Spawn();
#endif

	body->s = ent->s;
	body->s.eFlags = EF_DEAD;		// clear EF_TALK, etc

	if( ent->client->ps.eFlags & EF_HEADSHOT ) {
		body->s.eFlags |= EF_HEADSHOT;			// make sure the dead body draws no head (if killed that way)
	}

	body->s.eType = ET_CORPSE;
	body->classname = "corpse";

	body->classnamehash = CORPSE_HASH;
	body->s.powerups = 0;	// clear powerups
	body->s.loopSound = 0;	// clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	body->physicsSlide = qfalse;
	body->physicsFlush = qfalse;

	VectorCopy( ent->client->ps.viewangles, body->s.angles );
	VectorCopy( ent->client->ps.viewangles, body->r.currentAngles);

	if ( ent->client->ps.powerups[PW_HELMETSHIELD] )
		body->s.powerups |= (1 << PW_HELMETSHIELD);

	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) {
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	}
	else {
		body->s.pos.trType = TR_STATIONARY;
	}

	body->s.event = 0;

	// DHM - Clear out event system
	for( i=0; i<MAX_EVENTS; ++i ) {
		body->s.events[i] = 0;
	}

	body->s.eventSequence = 0;

	// time needed to complete animation
	body->s.torsoAnim = body->s.legsAnim = ent->client->legsDeathAnim;

	body->r.svFlags = ent->r.svFlags & ~SVF_BOT;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);

	// ydnar: bodies have lower bounding box
	body->r.maxs[ 2 ] = 0;

	body->s.effect1Time = ent->client->deathAnimTime;

	// this'll look better
	if ( body->s.onFireEnd > level.time ) {
		body->s.onFireEnd = ent->client->deathAnimTime+1500;
	}

	VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );

	if ( ent->client->deathAnim ) {
		vec3_t			origin, offset;
		grefEntity_t	refent;

		mdx_PlayerAnimation( body );
		mdx_gentity_to_grefEntity( body, &refent, level.time );
		mdx_tag_position( body, &refent, origin, "tag_ubelt", 0, 0 );

		// convert it to vector around null pos
		offset[0] = origin[0] - body->r.currentOrigin[0];
		offset[1] = origin[1] - body->r.currentOrigin[1];
		offset[2] = origin[2] = body->r.currentOrigin[2];

		G_StepSlideCorpse( body, origin );
		// this is the max vector we can reach
		VectorCopy ( body->s.pos.trBase, origin );

		// change bounding box to be off the origin of the corpse
		// that will make correct box agains model
		body->r.maxs[0] = offset[0] + 18;
		body->r.maxs[1] = offset[1] + 18;
		body->r.mins[0] = offset[0] - 18;
		body->r.mins[1] = offset[1] - 18;

		body->r.currentOrigin[0] = origin[0] - offset[0];
		body->r.currentOrigin[1] = origin[1] - offset[1];
		body->r.currentOrigin[2] = origin[2];

		// ok set it und Fertig!
		VectorCopy ( body->r.currentOrigin, body->s.pos.trBase );
	}

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	// DHM - Nerve :: allow bullets to pass through bbox
	// Gordon: need something to allow the hint for covert ops
	body->r.contents = CONTENTS_CORPSE;
	body->r.ownerNum = ent->r.ownerNum;

	BODY_TEAM(body) = ent->client->sess.sessionTeam;
	BODY_CLASS(body) = ent->client->sess.playerType;
	BODY_CHARACTER(body) = ent->client->pers.characterIndex;
	BODY_VALUE(body) = 0;

	if ( ent->client->ps.eFlags & EF_PANTSED ){
		body->s.time2 =	1;
	}
	else {
		body->s.time2 =	0;
	}

	body->activator = NULL;
	body->nextthink = level.time + BODY_TIME;
	body->think = BodySink;
	body->die = body_die;

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH ) {
		body->takedamage = qfalse;
	}
	else {
		body->takedamage = qtrue;
	}

	trap_LinkEntity (body);

}

//======================================================================

/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) {

	// set the delta angle
	ent->client->ps.delta_angles[0] = ANGLE2SHORT(angle[0]) - ent->client->pers.cmd.angles[0];
	ent->client->ps.delta_angles[1] = ANGLE2SHORT(angle[1]) - ent->client->pers.cmd.angles[1];
	ent->client->ps.delta_angles[2] = ANGLE2SHORT(angle[2]) - ent->client->pers.cmd.angles[2];

	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

void SetClientViewAnglePitch( gentity_t *ent, vec_t angle ) {

	ent->client->ps.delta_angles[PITCH] = ANGLE2SHORT(angle) - ent->client->pers.cmd.angles[PITCH];

	ent->s.angles[ PITCH ] = 0;
	VectorCopy( ent->s.angles, ent->client->ps.viewangles);
}

void G_DropLimboHealth(gentity_t *ent)
{

	if(g_dropHealth.integer == 0 || !ent->client || ent->client->sess.playerType != PC_MEDIC || g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	{
		vec3_t launchvel, launchspot;
		int i, packs = g_dropHealth.integer;
		int cwt		= ent->client->ps.classWeaponTime;

		if(g_dropHealth.integer == -1) {
			packs = 5; // max packs
			ent->client->ps.classWeaponTime += (level.time - ent->client->deathTime + 10000);
		}

		for(i=0; i<packs; ++i) {
			if(g_dropHealth.integer == -1 && ent->client->ps.classWeaponTime >= level.time) {
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
}

void G_DropLimboAmmo(gentity_t *ent)
{

	if(g_dropAmmo.integer == 0 || !ent->client || ent->client->sess.playerType != PC_FIELDOPS ) {
		return;
	}

	{
		vec3_t launchvel, launchspot;
		int i, packs = g_dropAmmo.integer;
		int cwt = ent->client->ps.classWeaponTime;

		if(g_dropAmmo.integer == -1) {
			packs = 5; // max packs
			// adjust the charge timer so that the fops only drops as much ammo as he could have before he died.
			// don't ask me where the 10000 comes from, it just makes this more accurate.
			ent->client->ps.classWeaponTime += (level.time - ent->client->deathTime	+ 10000);
		}

		for(i=0; i<packs; ++i) {
			if(g_dropAmmo.integer == -1 && ent->client->ps.classWeaponTime >= level.time) {
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
}

/* JPW NERVE
================
limbo
================
*/
void limbo( gentity_t *ent, qboolean makeCorpse )
{
	if (!(ent->client->ps.pm_flags & PMF_LIMBO)) {
		int i,contents;
		int startclient = ent->client->ps.clientNum;

		if( ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
			if( g_maxlivesRespawnPenalty.integer ) {
				ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] = g_maxlivesRespawnPenalty.integer;
			}
			else {
				ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] = -1;
			}
		}

		// DHM - Nerve :: First save off persistant info we'll need for respawn
		for( i = 0; i < MAX_PERSISTANT; ++i) {
			ent->client->saved_persistant[i] = ent->client->ps.persistant[i];
		}

		ent->client->ps.pm_flags |= PMF_LIMBO;
		ent->client->ps.pm_flags |= PMF_FOLLOW;


		if( makeCorpse ) {
			G_DropLimboHealth(ent);
			G_DropLimboAmmo(ent);
			// drop dog tags if enabled
			if ( jp_insanity.integer & JP_INSANITY_DOGTAGS ) {
				G_DropDogTag(ent);
			}
			CopyToBodyQue (ent); // make a nice looking corpse
		}
		else {
			trap_UnlinkEntity (ent);
		}

		// DHM - Nerve :: reset these values
		ent->client->ps.viewlocked = VIEWLOCK_NONE;
		ent->client->ps.viewlocked_entNum = 0;

		ent->r.maxs[2] = 0;
		ent->r.currentOrigin[2] += 8;
		contents = trap_PointContents( ent->r.currentOrigin, -1 ); // drop stuff
		ent->s.weapon = ent->client->limboDropWeapon; // stored in player_die()
		if ( makeCorpse && !( contents & CONTENTS_NODROP ) ) {
			TossClientItems( ent );
		}

		ent->client->sess.spectatorClient = startclient;
		Cmd_FollowCycle_f(ent,1,qfalse); // get fresh spectatorClient

		if (ent->client->sess.spectatorClient == startclient) {
			// No one to follow, so just stay put
			ent->client->sess.spectatorState = SPECTATOR_FREE;
		}
		else {
			ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		}

		if (ent->client->sess.sessionTeam == TEAM_AXIS)	{
			level.redNumWaiting++;
		}
		else if (ent->client->sess.sessionTeam == TEAM_ALLIES) {
			level.blueNumWaiting++;
		}

		for(i=0; i<level.numConnectedClients; ++i) {
			gclient_t *cl = &level.clients[level.sortedClients[i]];
			if( cl->ps.pm_flags & PMF_LIMBO && cl->sess.spectatorClient == ent - g_entities ) {
				Cmd_FollowCycle_f( &g_entities[level.sortedClients[i]], 1, qfalse );
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
	int			p;
	vec3_t		spawn_origin, spawn_angles;
	gentity_t	*spawnPoint = NULL;

	if (!(ent->client->ps.pm_flags & PMF_LIMBO)) {
		G_Printf("player already deployed, skipping\n");
		return;
	}

	if ( ent->client->sess.sessionTeam != TEAM_AXIS && ent->client->sess.sessionTeam != TEAM_ALLIES ) {
		return;
	}

#ifdef MV_SUPPORT
	if(ent->client->pers.mvCount > 0) {
		G_smvRemoveInvalidClients(ent, TEAM_AXIS);
		G_smvRemoveInvalidClients(ent, TEAM_ALLIES);
	}
#endif

	// core: stay in limbo, if there is no free spawnpoint..
	spawnPoint = SelectCTFSpawnPoint( ent->client->sess.sessionTeam, ent->client->pers.teamState.state, spawn_origin, spawn_angles, ent->client->sess.spawnObjectiveIndex );
	if ( !spawnPoint ) {
		ent->client->ps.pm_flags |= PMF_LIMBO;
		trap_SendServerCommand( ent-g_entities, "reqforcespawn" );
		return;
	}

	// DHM - Nerve :: restore persistant data now that we're out of Limbo
	for (p=0; p<MAX_PERSISTANT; ++p) {
		ent->client->ps.persistant[p] = ent->client->saved_persistant[p];
	}
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
			}
			else {
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

	for(i=0; i<level.numConnectedClients; ++i) {
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

	switch (weaponNum) {
		// weapons that only use clip (not ammo)
		// and are class/skill specific..
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_POISON_SYRINGE:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
			client->ps.ammoclip[WeaponTable[weaponNum].clipindex] = BG_ExtraAmmoForWeapon( weaponNum, client->sess.skillBits, client->sess.playerType );
			break;

		// weapons that only use clip (not ammo)
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		case WP_FLAMETHROWER:
			{
				int maxamount = BG_MaxAmmoForWeapon( weaponNum, client->sess.skillBits, client->sess.playerType );
				client->ps.ammoclip[WeaponTable[weaponNum].clipindex] += BG_ExtraAmmoForWeapon( weaponNum, client->sess.skillBits, client->sess.playerType );
				if ( client->ps.ammoclip[WeaponTable[weaponNum].clipindex] > maxamount )
					client->ps.ammoclip[WeaponTable[weaponNum].clipindex] = maxamount;
			}
			break;

		// weapons that use ammo+clip fillup the ammo here..
		default:
			{
				int maxamount = BG_MaxAmmoForWeapon( weaponNum, client->sess.skillBits, client->sess.playerType );
				client->ps.ammo[WeaponTable[weaponNum].ammoindex] += BG_ExtraAmmoForWeapon( weaponNum, client->sess.skillBits, client->sess.playerType );
				if ( client->ps.ammo[WeaponTable[weaponNum].ammoindex] > maxamount )
					client->ps.ammo[WeaponTable[weaponNum].ammoindex] = maxamount;
			}
			break;
	}
}

qboolean AddWeaponToPlayer( gclient_t *client, weapon_t weapon, int ammo, int ammoclip, qboolean setcurrent )
{
	COM_BitSet( client->ps.weapons, weapon );

	client->ps.ammoclip[WeaponTable[weapon].clipindex]	= ammoclip;
	client->ps.ammo[WeaponTable[weapon].ammoindex]		= ammo;

	if( setcurrent ) {
		client->ps.weapon = weapon;
	}

	// skill handling
	AddExtraSpawnAmmo( client, weapon );

#ifdef OMNIBOTS
	Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(weapon));
#endif

	return qtrue;
}

/*
 G_AddClassSpecificTools  //tjw
 This stuff was stripped out of SetWolfSpawnWeapons()  so it can
 be used for class stealing.  It needed cleaning up badly anyway.
 It still does.
 */
void G_AddClassSpecificTools(gclient_t *client)
{
	qboolean add_binocs = qfalse;

	if( client->sess.skillBits[SK_BATTLE_SENSE] & (1<<1) ) {
		add_binocs = qtrue;
	}

	switch(client->sess.playerType) {
		case PC_ENGINEER:
			AddWeaponToPlayer( client, WP_DYNAMITE, 0, 1, qfalse );
			AddWeaponToPlayer( client, WP_PLIERS, 0, 1, qfalse );
			AddWeaponToPlayer( client,
				WP_LANDMINE,
				GetWeaponTableData(WP_LANDMINE)->defaultStartingAmmo,
				GetWeaponTableData(WP_LANDMINE)->defaultStartingClip,
				qfalse );
			if(  client->sess.skillBits[SK_ENGINEER] & (1<<4) )
				client->ps.powerups[PW_FLAKJACKET] = INT_MAX;
			break;
		case PC_COVERTOPS:
			add_binocs = qtrue;
			AddWeaponToPlayer(client,
				WP_SMOKE_BOMB,
				GetWeaponTableData(WP_SMOKE_BOMB)->defaultStartingAmmo,
				GetWeaponTableData(WP_SMOKE_BOMB)->defaultStartingClip,
				qfalse);
			// See if we already have a satchel charge placed
			// this needs to be here for class stealing
			if( G_FindSatchel( &g_entities[client->ps.clientNum] ) ) {
				AddWeaponToPlayer( client, WP_SATCHEL, 0, 0, qfalse );
				AddWeaponToPlayer( client, WP_SATCHEL_DET, 0, 1, qfalse );
			}
			else {
				AddWeaponToPlayer( client, WP_SATCHEL, 0, 1, qfalse );
				AddWeaponToPlayer( client, WP_SATCHEL_DET, 0, 0, qfalse );
			}
			break;
		case PC_FIELDOPS:
			if ( !(g_weapons.integer & WPF_NO_L0_FOPS_BINOCS) || client->sess.skillBits[SK_SIGNALS] & (1<<1) )
			add_binocs = qtrue;
			AddWeaponToPlayer(client, WP_AMMO,			0, 1, qfalse);
			AddWeaponToPlayer(client, WP_SMOKE_MARKER,
				GetWeaponTableData(WP_SMOKE_MARKER)->defaultStartingAmmo,
				GetWeaponTableData(WP_SMOKE_MARKER)->defaultStartingClip,
				qfalse);
			break;
		case PC_MEDIC:
			AddWeaponToPlayer(client,
				WP_MEDIC_SYRINGE,
				GetWeaponTableData(WP_MEDIC_SYRINGE)->defaultStartingAmmo,
				GetWeaponTableData(WP_MEDIC_SYRINGE)->defaultStartingClip,
				qfalse);
			AddWeaponToPlayer(client,
				WP_MEDKIT,
				GetWeaponTableData(WP_MEDKIT)->defaultStartingAmmo,
				GetWeaponTableData(WP_MEDKIT)->defaultStartingClip,
				qfalse);
			break;
		case PC_SOLDIER:
			if ( client->sess.skillBits[SK_SOLDIER] & (1<<9))
				client->ps.powerups[PW_HELMETSHIELD] = INT_MAX;
			break;
	}

	if(add_binocs) {
		if(AddWeaponToPlayer(client, WP_BINOCULARS, 1, 0, qfalse)) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	}

	if( (client->sess.playerType != PC_ENGINEER && (jp_keepAwards.integer & JP_KEEP_FLAKJACKET))
		  && client->sess.skillBits[SK_ENGINEER] & (1<<4) )
		  client->ps.powerups[PW_FLAKJACKET] = INT_MAX;

	if( client->sess.skillBits[SK_LIGHT_WEAPONS]& (1<<7)) {
		AddWeaponToPlayer(client, WP_POISON_SYRINGE, GetWeaponTableData(WP_POISON_SYRINGE)->defaultStartingAmmo, GetWeaponTableData(WP_POISON_SYRINGE)->defaultStartingClip, qfalse);
	}
}
/*
===========================================
G_CorrectSecondaryTeam


makes sure that client recieve correct secondary
============================================
*/
weapon_t G_CorrectSecondaryWeapon( weapon_t original, team_t team, int LWSkill, int HWSkill, int pc )
{
	switch ( original ) {
		case WP_LUGER:
		case WP_COLT:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
			return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_LUGER:
			if ( LWSkill >= 4 && ( pc != PC_MEDIC || !(g_medics.integer & MEDIC_NOAKIMBO)) )
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
			else if ( LWSkill >= 4 )
				return team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
			else
				return team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
		default:
			return WP_NONE; // invalid weapon for secondary
	}
}

void G_AddClassWeapons(gclient_t *client)
{
	int					numClips;
	weapon_t			primary=WP_NONE, secondary=WP_NONE, secondary2=WP_NONE, grenade=WP_NONE;
	bg_playerclass_t	*classInfo;
	team_t				team = client->sess.sessionTeam;
	int					pc = client->sess.playerType;

	classInfo = team == TEAM_ALLIES ? &bg_allies_playerclasses[pc] : &bg_axis_playerclasses[pc];
	grenade = team == TEAM_ALLIES ? WP_GRENADE_PINEAPPLE : WP_GRENADE_LAUNCHER;

	switch(pc) {
		case PC_SOLDIER:
			numClips = 1;
			break;
		case PC_MEDIC:
			numClips = 0;
			break;
		case PC_ENGINEER:
			numClips = 1;
			break;
		case PC_FIELDOPS:
			numClips = 1;
			break;
		case PC_COVERTOPS:
			numClips = 1;
			break;
		default:
			return;
	}

	primary = client->sess.latchPlayerWeapon;
	secondary = client->sess.latchPlayerWeapon2;

	// correct player weapon
	secondary = G_CorrectSecondaryWeapon( secondary, team, client->sess.skill[SK_LIGHT_WEAPONS], client->sess.skill[SK_HEAVY_WEAPONS], pc );

	// set secondary slot if not available
	if ( secondary == WP_NONE ) {
		if ( pc == PC_SOLDIER && client->sess.skillBits[SK_SOLDIER] & (1<<4) ) {
			secondary = team == TEAM_ALLIES ? WP_THOMPSON : WP_MP40;
		}
		else if ( pc == PC_COVERTOPS && client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) ) {
			secondary = team == TEAM_ALLIES ? WP_AKIMBO_SILENCEDCOLT : WP_AKIMBO_SILENCEDLUGER;
		}
		else if ( client->sess.skillBits[SK_LIGHT_WEAPONS]& (1<<4) ) {
			secondary = team == TEAM_ALLIES ? WP_AKIMBO_COLT : WP_AKIMBO_LUGER;
		}
		else {
			if ( pc == PC_COVERTOPS ) {
				secondary2 = team == TEAM_ALLIES ? WP_SILENCED_COLT : WP_SILENCER;
			}
			secondary = team == TEAM_ALLIES ? WP_COLT : WP_LUGER;
		}
	}
	else if ( pc == PC_COVERTOPS ) {
		secondary2 = weapAlts[secondary];
	}

	// add ammo to akimbo sidearm
	if( IS_AKIMBO_WEAPON(secondary) ) {
		client->ps.ammoclip[WeaponTable[BG_AkimboSidearm(secondary)].clipindex] = GetWeaponTableData(secondary)->defaultStartingClip;
	}

	// add secondary
	AddWeaponToPlayer( client, secondary, GetWeaponTableData(secondary)->defaultStartingAmmo, GetWeaponTableData(secondary)->defaultStartingClip, qtrue );
	if ( secondary2 ) {
		AddWeaponToPlayer( client, secondary2, GetWeaponTableData(secondary2)->defaultStartingAmmo, GetWeaponTableData(secondary2)->defaultStartingClip, qtrue );
	}

	// add primary
	if ( primary <= WP_NONE || primary >= WP_NUM_WEAPONS || !BG_ClassHasWeapon(classInfo, primary ) || G_IsWeaponDisabled( &g_entities[client->ps.clientNum], primary, qtrue )) {
		primary = classInfo->classWeapons[0];
	}

	if ( pc != PC_MEDIC || !( g_medics.integer & MEDIC_PISTOLONLY )) {
		AddWeaponToPlayer( client, primary, GetWeaponTableData(primary)->defaultStartingAmmo*numClips, GetWeaponTableData(primary)->defaultStartingClip, qtrue );
	}

	// add alt weapon if available
	if ( weapAlts[primary] != WP_NONE && !G_IsWeaponDisabled(&g_entities[client->ps.clientNum], weapAlts[primary], qtrue) ){
		AddWeaponToPlayer( client, weapAlts[primary], GetWeaponTableData(weapAlts[primary])->defaultStartingAmmo*numClips, GetWeaponTableData(weapAlts[primary])->defaultStartingClip, qfalse );
		if ( weapAlts[primary] == WP_M7 || weapAlts[primary] == WP_GPG40 ){
			if ( client->sess.rifleNade != 1 ){
				client->sess.rifleNade = 1;
				ClientUserinfoChanged( client->ps.clientNum );
			}
		}
		else {
			if ( client->sess.rifleNade != 0 ){
				client->sess.rifleNade = 0;
				ClientUserinfoChanged( client->ps.clientNum );
			}
		}
	}
	else {
		if ( client->sess.rifleNade != 0 ){
			client->sess.rifleNade = 0;
			ClientUserinfoChanged( client->ps.clientNum );
		}
	}

	// add nades
	// core: AddExtraSpawnAmmo() will add the correct number of nades for this class/skill
	AddWeaponToPlayer( client, grenade, 0, 0, qfalse );

}

/*
===========
SetWolfSpawnWeapons
===========
*/
void SetWolfSpawnWeapons( gclient_t *client )
{
	int			pc;
	qboolean	doWar = (nq_War.integer & WARMODE_ENABLE)? qtrue : qfalse;

	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

#ifdef OMNIBOTS
	Bot_Event_ResetWeapons(client->ps.clientNum);
	// core: add the WP_FOOTKICK for bots..
	Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_FOOTKICK));
#endif

    pc = client->sess.playerType;

	// Reset special weapon time
	if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_HALFCHARGE)) {
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
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_ZEROCHARGE)) {
		client->ps.classWeaponTime = level.time;
	}
	else if(client->pers.slashKill && (g_slashKill.integer & SLASHKILL_SAMECHARGE)) {
		// tjw: classWeaponTime preserved in ClientSpawn()
		//      adjust it to compensate for the time of /kill
		client->ps.classWeaponTime += level.time - client->deathTime;
	}
	else {
		// tjw: give full charge bar
		client->ps.classWeaponTime = -999999;
	}

	client->pers.slashKill = qfalse;

	// Communicate it to cgame
	client->ps.stats[STAT_PLAYER_CLASS] = pc;

	// Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
	client->ps.teamNum = pc;

	// core: in warmode, weapons/ammo/clips are handled differently..
	if ( doWar ) {
		return;
	}

	// JPW NERVE -- zero out all ammo counts
	memset(client->ps.ammo, 0, MAX_WEAPONS * sizeof(int));

	// All players start with a knife (not OR-ing so that it clears previous weapons)
	client->ps.weapons[0] = 0;
	client->ps.weapons[1] = 0;

	// core: AddExtraSpawnAmmo() will add the correct number of knives for this class/skill
	AddWeaponToPlayer( client, (client->sess.sessionTeam == TEAM_AXIS ? WP_KNIFE : WP_KNIFE_KABAR) , 0, 0, qtrue );


	client->ps.weaponstate = WEAPON_READY;

	// jet Pilot - Give Adrenaline if the player has earned it
	//if (client->sess.skillBits[SK_MEDIC] & (1<<4) && ((pc == PC_MEDIC && !(g_medics.integer & MEDIC_NOSELFADREN) ) || jp_keepAwards.integer & JP_KEEP_ADRENALINE))

	// StM adrenaline limit to classes
	if (client->sess.skillBits[SK_MEDIC] & (1<<4) &&
			((pc == PC_SOLDIER && g_adrenaline.integer & ADF_SOLDIER ) ||
			(pc == PC_MEDIC && g_adrenaline.integer & ADF_MEDIC ) ||
			(pc == PC_ENGINEER && g_adrenaline.integer & ADF_ENGINEER ) ||
			(pc == PC_FIELDOPS && g_adrenaline.integer & ADF_FIELDOPS ) ||
			(pc == PC_COVERTOPS && g_adrenaline.integer & ADF_COVERTOPS )))
	{
		// core: AddExtraSpawnAmmo() will add the correct number of adrenalines for this class/skill
		AddWeaponToPlayer(client, WP_MEDIC_ADRENALINE, 0, 0, qfalse);
		if ( g_adrenaline.integer & ADF_NOSPAWN ) {
			// start with 0, but able to pickup more at cabinet etc..
			client->ps.ammoclip[WP_MEDIC_ADRENALINE] = 0;
		}
	}

	G_AddClassWeapons(client);
	G_AddClassSpecificTools(client);
}

int G_CountTeamMedics( team_t team, qboolean alivecheck ) {
	int numMedics = 0;
	int i, j;

	for( i = 0; i < level.numConnectedClients; ++i ) {
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

	client->pers.maxHealth = 100;

	// core: do we use the same maxHP for every playerclass?..
	if ( g_medics.integer & MEDIC_ALLMAXHP ) {
		client->pers.maxHealth = 125;
	}
	// max HP depends on the # medics in a team..
	else if ( !(g_medics.integer & MEDIC_NOEXTRATEAMHP) ) {
		// compute health mod
		client->pers.maxHealth += 10 * G_CountTeamMedics( client->sess.sessionTeam, qfalse );

		if( client->pers.maxHealth > 125 ) {
			client->pers.maxHealth = 125;
		}
	}

	if( client->sess.skillBits[SK_BATTLE_SENSE] & (1<<3) ) {
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
	int		len = 0;
	int  	colorlessLen = 0;
	char	ch;
	char	*p = out;
	int		spaces = 0;

	//save room for trailing null byte
	outSize--;

	*p = 0;

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

// redeye - copied from ETPub, courtesy of Dens
const char *GetParsedIP(const char *ipadd)
{
	// code by Dan Pop, http://bytes.com/forum/thread212174.html
	unsigned b1, b2, b3, b4, port = 0;
	unsigned char c;
	int rc;
	static char ipge[20];

	if(!Q_strncmp(ipadd,"localhost",strlen("localhost"))) {
		return "localhost";
	}

	rc = sscanf(ipadd, "%3u.%3u.%3u.%3u:%u%c", &b1, &b2, &b3, &b4, &port, &c);
	if (rc < 4 || rc > 5) {
		return NULL;
	}
	if ( (b1 | b2 | b3 | b4) > 255 || port > 65535) {
		return NULL;
	}
	if (strspn(ipadd, "0123456789.:") < strlen(ipadd)) {
		return NULL;
	}
	sprintf(ipge, "%u.%u.%u.%u", b1, b2, b3, b4);
	return ipge;
}

// redeye - copied from ETPub, courtesy of Dens
// Dens: based on reyalp's userinfocheck.lua and combinedfixes.lua
// IRATA TODO
// Add more checks if possible ... (uci in case of enabled ...)
char *CheckUserinfo( int clientNum, char *userinfo )
{
	char*	value;
	int		length = strlen(userinfo);
	int 	i, slashCount = 0, count = 0;

	if(length < 1) {
		return "Userinfo too short";
	}

	// Dens: 44 is a bit random now: MAX_INFO_STRING - 44 = 980. The LUA script
	// uses 980, but I don't know if there is a specific reason for that
	// number. Userinfo should never get this big anyway, unless someone is
	// trying to force the engine to truncate it, so that the real IP is lost
	if(length > MAX_INFO_STRING - 44) {
		return "Userinfo too long.";
	}

	// Dens: userinfo always has to have a leading slash
	if(userinfo[0] != '\\') {
		return "Missing leading slash in userinfo.";
	}
	// Dens: the engine always adds ip\ip:port at the end, so there will never
	// be a trailing slash
	if(userinfo[length-1] == '\\') {
		return "Trailing slash in userinfo.";
	}

	for(i=0; userinfo[i]; ++i) {
		if(userinfo[i] == '\\'){
			slashCount++;
		}
	}
	if(slashCount % 2 != 0){
		return "Bad number of slashes in userinfo.";
	}

	// Dens: make sure there is only one ip, cl_guid, name and cl_punkbuster
	// field
	if(length > 4){
		for(i=0; userinfo[i+3]; ++i) {
			if(userinfo[i] == '\\' && userinfo[i+1] == 'i' &&
				userinfo[i+2] == 'p' && userinfo[i+3] == '\\'){
				count++;
			}
		}
	}
	if(count == 0) {
		return "Missing IP in userinfo.";
	}
	else if(count > 1) {
		return "Too many IP fields in userinfo.";
	}
	else {
		if (GetParsedIP(Info_ValueForKey(userinfo, "ip")) == NULL) {
			return "Malformed IP in userinfo.";
		}
	}
	count = 0;

	if(length > 9) {
		for(i=0;userinfo[i+8]; ++i) {
			if(userinfo[i] == '\\' && userinfo[i+1] == 'c' &&
				userinfo[i+2] == 'l' && userinfo[i+3] == '_' &&
				userinfo[i+4] == 'g' && userinfo[i+5] == 'u' &&
				userinfo[i+6] == 'i' && userinfo[i+7] == 'd' &&
				userinfo[i+8] == '\\'){
				count++;
			}
		}
	}
	if(count > 1) {
		return "Too many cl_guid fields in userinfo.";
	}
	count = 0;

	if(length > 6) {
		for(i=0;userinfo[i+5]; ++i) {
			if(userinfo[i] == '\\' && userinfo[i+1] == 'n' &&
				userinfo[i+2] == 'a' && userinfo[i+3] == 'm' &&
				userinfo[i+4] == 'e' && userinfo[i+5] == '\\'){
				count++;
			}
		}
	}
	if(count == 0) {
		return "Missing name field in userinfo.";
	}
	else if(count > 1) {
		return "Too many name fields in userinfo.";
	}
	count = 0;

	if(length > 15) {
		for(i=0;userinfo[i+14];++i) {
			if(userinfo[i] == '\\' && userinfo[i+1] == 'c' &&
				userinfo[i+2] == 'l' && userinfo[i+3] == '_' &&
				userinfo[i+4] == 'p' && userinfo[i+5] == 'u' &&
				userinfo[i+6] == 'n' && userinfo[i+7] == 'k' &&
				userinfo[i+8] == 'b' && userinfo[i+9] == 'u' &&
				userinfo[i+10] == 's' && userinfo[i+11] == 't' &&
				userinfo[i+12] == 'e' && userinfo[i+13] == 'r' &&
				userinfo[i+14] == '\\'){
				count++;
			}
		}
	}
	if(count > 1) {
		return "Too many cl_punkbuster fields in userinfo.";
	}

	value = Info_ValueForKey(userinfo, "rate");
	if (value == NULL || value[0] == '\0') {
		return "Wrong rate field in userinfo.";
	}

	return 0;
}

/*
============
core:
	FileExists()
============
*/
qboolean FileExists( char* filename ) {
	struct stat statbuf;

	if ( stat(filename,&statbuf) < 0 ) {
		return qfalse;
	}
	return qtrue;
}

/*
============
core:
	CGAMEFileExists()

	Some function for checking if client-files exist on the server.
	Used for checking "localhost" hackers..
============
*/
qboolean CGAMEFileExists( void ) {
	char homepath[512];

	trap_Cvar_VariableStringBuffer("fs_homepath", homepath, sizeof(homepath));
#ifdef __MACOS__
	strcat(homepath, "/nq/cgame_mac");
	if ( !FileExists(homepath) ) return qfalse;
#endif
#ifdef __linux__
	strcat(homepath, "/nq/cgame.mp.i386.so");
	if ( !FileExists(homepath) ) return qfalse;
#endif
#ifdef WIN32
	strcat(homepath, "/nq/cgame_mp_x86.dll");
	if ( !FileExists(homepath) ) return qfalse;
#endif
	return qtrue;
}

/*
============
core:
	CheckLocalhost()

	Checking for "localhost" hackers..
============
*/
char *CheckLocalhost( gentity_t *ent, char *userinfo ) {
	qboolean valid = qtrue;
	char *reason = NULL;

	if ( !(ent->r.svFlags & SVF_BOT) ) {
		char *strIP = Info_ValueForKey(userinfo,"ip");

		if ( !Q_strncmp(strIP, "localhost", MAX_IP_LENGTH) ) {
			// localhost,.. hmm, are we really playing at this server?
			// Dedicated servers cannot have people playing at the server itself..
			if ( g_dedicated.integer ) {
				valid = qfalse;
			}

			// Client game-file must exist on the server, in order to make someone join as localhost..
			// This check is for non-dedicated servers only.. (listenserver)
			if ( valid && !level.clientFilesFound ) {
				valid = qfalse;
			}

			if ( !valid ) {
				reason = va("ClientUserinfoChanged: client hacking IP! clientnum: %d, name: %s", ent->client->ps.clientNum, ent->client->pers.netname);
				G_Printf(reason);
				G_LogPrintf( "%s %s %s %s\n", reason, ent->client->pers.client_ip, strIP, ent->client->pers.cl_guid );
				G_DPrintf( "%s %s %s %s\n", reason, ent->client->pers.client_ip, strIP, ent->client->pers.cl_guid );
				// i think just disconnecting them is way too friendly..
				// ... CG_DestroyClientHardware();
				trap_DropClient(ent->client->ps.clientNum, "", 0);

			}
		}
	}
	return reason;
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
extern g_shrubbot_ban_t		*g_shrubbot_mutes[MAX_SHRUBBOT_BANS]; // jaquboss
extern void _shrubbot_writeconfig();

void ClientUserinfoChanged( int clientNum ) {
	gentity_t	*ent = g_entities + clientNum;
	char		*s;
	char		oldname[MAX_STRING_CHARS];
	char		userinfo[MAX_INFO_STRING];
	gclient_t	*client  = ent->client;
	int			i;
	char		skillStr[16] 					= "";
	char		medalStr[16] 					= "";
	int			characterIndex					= -1;
	const char*	userinfo_ptr					= NULL;
	char		cs_key[MAX_STRING_CHARS]		= "";
	char		cs_value[MAX_STRING_CHARS]		= "";
	char		cs_cg_uinfo[MAX_STRING_CHARS]	= "";
	char		cs_name[MAX_NETNAME]			= "";
	char		cs_skill[MAX_STRING_CHARS]		= "";
	char		skillBits[128] = "";
	char		*reason;

	client->ps.clientNum = clientNum;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ));

	// core: some IP checking against localhost hackers..
	reason = CheckLocalhost(ent,userinfo);
	if ( reason ) {
		return;
	}

	// redeye - check for bad userinfo
	if ( !(ent->r.svFlags & SVF_BOT) ) {
		reason = CheckUserinfo(clientNum, userinfo);
		if(reason) {
			// wtf we are telling the client what's wrong? If this check fails something is going on ...
			// ... and the scum knows what to do
			trap_DropClient(clientNum,va("^1%s", "Bad userinfo."), 0);
			G_Printf("ClientUserinfoChanged: CheckUserinfo: client %d: %s\n", clientNum, reason);
		}
	}

	// Lucel: Cache config string values used in ClientUserinfoChanged
	//
	// Every time we call Info_ValueForKey we search through the string from the start....
	// let's grab the values we need in just one pass... key matching is fast because
	// we will use our minimal perfect hashing function.
	userinfo_ptr = userinfo;

	while(1) {
		g_StringToken_t token;
		// Get next key/value pair
		if ( qfalse == Info_NextPair(&userinfo_ptr, cs_key, cs_value ) ) {
			// This would only happen if the end user is trying to manually modify the user info string
			G_Printf("ClientUserinfoChanged: client %d hacking clientinfo, empty key found!\n", clientNum);

			if ( !(ent->r.svFlags & SVF_BOT) )
				trap_DropClient(clientNum, "Bad userinfo.", 0);
			break;
		}
		// Empty string for key and we exit...
		if ( cs_key[0] == 0 ) {
			break;
		}

		token = G_GetTokenForString(cs_key);
		switch(token) {
			case TOK_ip:
				{
					if ( !(ent->r.svFlags & SVF_BOT) && CompareIPNoPort(client->pers.client_ip, cs_value) == qfalse ) {
						// They're trying to hack their ip address....
						G_Printf("ClientUserinfoChanged: client %d hacking ip, old=%s, new=%s\n", clientNum, client->pers.client_ip, cs_value);
						trap_DropClient(clientNum, "Bad userinfo.", 0);
						break;
					}
					Q_strncpyz(client->pers.client_ip, cs_value, MAX_IP_LENGTH);
					break;
				}
			case TOK_cg_uinfo:
				Q_strncpyz(cs_cg_uinfo, cs_value, MAX_STRING_CHARS);
				break;
			case TOK_pmove_fixed:
				if ( cs_value[0] )
					client->pers.pmoveFixed = atoi(cs_value);
				else
					client->pers.pmoveFixed = 0;
				break;
			case TOK_pmove_msec:
				if ( cs_value[0] )
					client->pers.pmoveMsec = atoi(cs_value);
				else
					client->pers.pmoveMsec = 8;
				// paranoia
				if (  client->pers.pmoveMsec > 33 )
					client->pers.pmoveMsec = 33;
				if ( client->pers.pmoveMsec < 3 )
					client->pers.pmoveMsec = 3;
				break;
			case TOK_name:
				Q_strncpyz(cs_name, cs_value, MAX_NETNAME);
				break;
			case TOK_cl_guid:
				if ( !(ent->r.svFlags & SVF_BOT) &&
					 strcmp(client->pers.cl_guid, "unknown") &&
					 strcmp(client->pers.cl_guid, cs_value) )
				{
					// They're trying to hack their guid...
					G_Printf("ClientUserinfoChanged: client %d hacking cl_guid, old=%s, new=%s\n", clientNum, client->pers.cl_guid, cs_value);
					trap_DropClient(clientNum, "Bad userinfo.", 0);
					break;
				}
				Q_strncpyz(client->pers.cl_guid, cs_value, PB_GUID_LENGTH+1);
				break;
			case TOK_ch:
				// check for custom character
				if ( cs_value[0] )
					characterIndex = atoi(cs_value);
				break;
			case TOK_skill:
				Q_strncpyz(cs_skill, cs_value, MAX_STRING_CHARS);
				break;
			default:
				continue;
		}
	}

	// censor name
	if (g_censorNames.string[0] && !(ent->r.svFlags & SVF_BOT)) {
		char censoredName[MAX_NETNAME];

		SanitizeString(cs_name, censoredName, qtrue);
		if (G_CensorText((char *)&censoredName,&censorNamesDictionary)) {
			G_LogPrintf( va("ClientUserInfoChanged: Censored name \"%s\" (clientNum=%i, IP=%s)\n", cs_name, clientNum, client->pers.client_ip) );
			Q_strncpyz( cs_name, censoredName, sizeof( cs_name ) );
			Info_SetValueForKey( userinfo, "name", censoredName);
			trap_SetUserinfo( clientNum, userinfo );
			if (g_censorPenalty.integer & CNSRPNLTY_KICK) {
				trap_DropClient(clientNum, "Name censor: Please change your name.", 0);
			}
		}
	}

	if ( ent->client->sess.muted /*|| G_shrubbot_mute_check(client->pers.client_ip, client->pers.cl_guid)*/ ) {
		// core: get the current/old playername..
		// If this player is muted, his shrubbot_mutes record must also be adjusted.
		trap_GetConfigstring( CS_PLAYERS + clientNum, oldname, sizeof(oldname) );
		s = Info_ValueForKey( oldname, "n" );

		for(i=0; g_shrubbot_mutes[i]; ++i) {
			if ( !Q_stricmp(g_shrubbot_mutes[i]->name, s) ) {
				Q_strncpyz( g_shrubbot_mutes[i]->name, cs_name, sizeof(g_shrubbot_mutes[i]->name) );
				_shrubbot_writeconfig();
				break;
			}
		}
	}

	client->medals = 0;
	for( i = 0; i < SK_NUM_SKILLS; ++i ) {
		client->medals += client->sess.medals[ i ];
	}

	// check for malformed or illegal info strings
	if ( !Info_Validate(userinfo) ) {
		Q_strncpyz( userinfo, "\\name\\badinfo", sizeof(userinfo) );
	}

#ifndef DEBUG_STATS
	if( g_developer.integer || g_dedicated.integer )
#endif
	{
		G_Printf("Userinfo: %s\n", userinfo);
	}

	// check for local client
	if ( !strcmp( client->pers.client_ip, "localhost" ) ) {
		client->pers.localClient = qtrue;
		level.fLocalHost = qtrue;
		// core: no bots are referee..
		if ( !(ent->r.svFlags & SVF_BOT) ) {
			client->sess.referee = RL_REFEREE;
		}
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
	}
	else {
		if(cs_cg_uinfo[0]) {
			sscanf(cs_cg_uinfo, "%i %i %i",
								&client->pers.clientFlags,
								&client->pers.clientTimeNudge,
								&client->pers.clientMaxPackets);
		}
		else {
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

		if(client->pers.clientFlags & CGF_WEAPALTRELOAD) {
			client->pmext.weapAltReload = qtrue;
			client->pers.weapAltReloadAux = qtrue;
		}
		else {
			client->pmext.weapAltReload = qfalse;
			client->pers.weapAltReloadAux = qfalse;
		}

		if(client->pers.clientFlags & CGF_AUTORELOAD) {
			client->pers.bAutoReloadAux = qtrue;
			client->pmext.bAutoReload = qtrue;
		}
		else {
			client->pers.bAutoReloadAux = qfalse;
			client->pmext.bAutoReload = qfalse;
		}
	}

	ClientCleanName( cs_name, client->pers.netname, sizeof(client->pers.netname) );

	for( i = 0; i < SK_NUM_SKILLS; ++i ) {
		Q_strcat( skillBits, sizeof(skillBits), va("%i ", client->sess.skillBits[i] ) );
		Q_strcat( skillStr, sizeof(skillStr), va("%i",client->sess.skill[i]) );
		Q_strcat( medalStr, sizeof(medalStr), va("%i",client->sess.medals[i]) );
		// FIXME: Gordon: wont this break if medals > 9 arnout? JK: Medal count is tied to skill count :() Gordon: er, it's based on >> skill per map, so for a huuuuuuge campaign it could break...
	}

	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	// To communicate it to cgame
	client->ps.stats[ STAT_PLAYER_CLASS ] = client->sess.playerType;
	// Gordon: Not needed any more as it's in clientinfo?

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
	s = va( "n\\%s\\t\\%i\\c\\%i\\r\\%i\\m\\%s\\s\\%s\\dn\\%s\\dr\\%i\\w\\%i\\lw\\%i\\sw\\%i\\mu\\%i\\ref\\%i\\rn\\%i\\lc\\%i\\xp\\%s\\u\\%u",
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
		client->sess.muted? 1 : 0,
		client->sess.referee,
		client->sess.rifleNade,
		client->sess.latchPlayerType,
		skillBits,
		client->sess.uci //mcwf GeoIP
	);

	trap_GetConfigstring( CS_PLAYERS + clientNum, oldname, sizeof( oldname ) );

	if( !Q_stricmp( oldname, s ) ) {
		return;
	}

	trap_SetConfigstring( CS_PLAYERS + clientNum, s );

#ifdef ET_LUA
	// *LUA* API callbacks
	// This only gets called when the ClientUserinfo is changed, replicating
	// ETPro's behaviour.
	G_LuaHook_ClientUserinfoChanged(clientNum);
#endif

	// Log the GUIDs?
	if (g_logOptions.integer & LOGOPTS_GUID) {

		if (g_logOptions.integer & LOGOPTS_IP) {
			G_LogPrintf( "ClientUserinfoChangedGUID: %i %s %s %s\n", clientNum, client->pers.client_ip, client->pers.cl_guid, s );
			G_DPrintf( "ClientUserinfoChangedGUID: %i :: %s %s %s\n", clientNum, client->pers.client_ip, client->pers.cl_guid, s );
		}
		else {
			G_LogPrintf( "ClientUserinfoChangedGUID: %i %s %s\n", clientNum, client->pers.cl_guid, s );
			G_DPrintf( "ClientUserinfoChangedGUID: %i :: %s %s\n", clientNum, client->pers.cl_guid, s );
		}
	}
	else {
		G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
		G_DPrintf( "ClientUserinfoChanged: %i :: %s\n", clientNum, s );
	}
}

/*
 * Lucel:
 *
 * Ported from the etpro lua module coded by reyalP.
	-- lua module to limit fakeplayers DOS
	-- http://aluigi.altervista.org/fakep.htm
	-- used if cvar is not set
	-- author: reyalp@gmail.com
	-- confugration:
	-- set ip_max_clients cvar as desired. If not set, defaults to the value below.
*/

// This defines the default value and also the minimum value we will allow the client to set...
#define DEF_IP_MAX_CLIENTS	3

// Return the length of the IP address if it has a port, or INT_MAX otherwise
int GetIPLength(char const* ip)
{
	char* 	start	= strchr(ip, ':');
	return (start == NULL ? INT_MAX : start - ip);
}

qboolean CompareIPNoPort(char const* ip1, char const* ip2)
{
	int			ip1_len		= GetIPLength(ip1);
	int			ip2_len		= GetIPLength(ip2);
	int			checkLength	= min(ip1_len, ip2_len);
	// Don't compare the port - just the IP
	if ( checkLength < INT_MAX && !Q_strncmp(ip1, ip2, checkLength) )
		return qtrue;
	else if ( checkLength == INT_MAX && !strcmp(ip1, ip2) )
		return qtrue;
	else
		return qfalse;
}

char* IsFakepConnection(int clientNum, char const* ip, char const* rate)
{
	int 		count		= 1; 		// we count as the first one
	int			max			= trap_Cvar_VariableIntegerValue("ip_max_clients");
	int			maxClients	= trap_Cvar_VariableIntegerValue("sv_maxclients");
	int			myIPLength	= GetIPLength(ip);
	int			i;

	// Default it to DEF_IP_MAX_CLIENTS as the minimum
	if ( !max || max <= 0 ) {
		max = DEF_IP_MAX_CLIENTS;
	}

	// validate userinfo to filter out the people blindly using luigi's code
	if ( rate[0] == 0 ) {
		return "Invalid connection!";
	}

	// jaquboss let localhost ( mostly bots and host player ) be
	if ( !strcmp(ip, "localhost") ) {
		return NULL;
	}

	// Iterate over each connected client and check the IP, keeping a count of connections
	// from the same IP as this connecting client
	for ( i = 0; i < maxClients; ++i ) {
		//char		cluserinfo[MAX_INFO_STRING];
		char*		theirIP;
		int			theirIPLength;
		int			checkLength;
		gentity_t	*ent;

		// Skip our own slot
		if ( i == clientNum ) {
			continue;
		}

		ent = &g_entities[i];
		if ( ent->client == NULL ) {
			continue;
		}

		theirIP 		= ent->client->pers.client_ip;
		theirIPLength	= GetIPLength(theirIP);
		checkLength		= min(theirIPLength, myIPLength);

		// pers.connected is set correctly for fake players can't rely on userinfo being empty
		if ( ent->client->pers.connected > 0 ) {
			qboolean	ipMatch		= qfalse;

			// Don't compare the port - just the IP
			if ( checkLength < INT_MAX && !Q_strncmp(ip, theirIP, checkLength) ) {
				ipMatch = qtrue;
			}
			else if ( checkLength == INT_MAX && !strcmp(ip, theirIP) ) {
				ipMatch = qtrue;
			}

			if ( ipMatch == qtrue ) {
				++count;
				if ( count > max ) {
					G_Printf("IsFakepConnection: too many connections from %s\n", ip);
					// TODO should we drop / ban all connections from this IP ?
					return va("Only %d connection%s per IP %s allowed on this server!",
							max,
							max == 1 ? "" : "s",
							max == 1 ? "is" : "are");
				}
			}
		}
	}

	// Ok let them connect
	return NULL;
}

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

// core: used for shortcuts..
int lastconnected_client = -1;

char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot ) {
	char*			value;
	gclient_t*		client;
	char			userinfo[MAX_INFO_STRING];
	char			reason[MAX_STRING_CHARS] 		= "";
	gentity_t*		ent = &g_entities[ clientNum ];
	int				i;
	const char*		userinfo_ptr					= NULL;
	char			cs_key[MAX_STRING_CHARS]		= "";
	char			cs_value[MAX_STRING_CHARS]		= "";
	char			cs_ip[MAX_STRING_CHARS] 		= "";
	char			cs_password[MAX_STRING_CHARS] 	= "";
	char 			cs_name[MAX_NETNAME]			= "";
	char			cs_guid[PB_GUID_LENGTH+1]		= "";
	char			cs_scriptname[MAX_STRING_CHARS] = "";
	char			cs_respawn[MAX_STRING_CHARS] 	= "";
	char			cs_rate[MAX_STRING_CHARS]		= "";
	char*			userinfoReason;
	char 			playername[MAX_NETNAME]			= "";

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	G_Printf("ClientConnect: %d: %s\n", clientNum, userinfo);

	// IRATA: we have special case on connect, ent->r.svFlags is not set
	// see if( isBot )  at end of this function
	if (!isBot) {
		// core: some IP checking against localhost hackers..
		value = CheckLocalhost(ent,userinfo);
		if (value) {
			return value; //reason
		}

		// redeye - check for bad userinfo
		userinfoReason = CheckUserinfo(clientNum, userinfo);
		if(userinfoReason) {
			G_Printf("ClientConnect: CheckUserinfo: client %d: %s\n", clientNum, userinfoReason);
			return "Bad userinfo.";
		}
	}

	// Lucel: Cache config string values used in ClientConnect
	//
	// Every time we call Info_ValueForKey we search through the string from the start....
	// let's grab the values we need in just one pass... key matching is fast because
	// we will use our minimal perfect hashing function.
	userinfo_ptr = userinfo;
	while(1) {
		g_StringToken_t token;
		// Get next key/value pair
		Info_NextPair(&userinfo_ptr, cs_key, cs_value );
		// Empty string for key and we exit...
		if ( cs_key[0] == 0 ) {
			break;
		}
		token = G_GetTokenForString(cs_key);
		switch(token) {
			case TOK_ip:
				Q_strncpyz(cs_ip, cs_value, MAX_STRING_CHARS);
				break;
			case TOK_name:
				Q_strncpyz(cs_name, cs_value, MAX_NETNAME);
				break;
			case TOK_cl_guid:
				Q_strncpyz(cs_guid, cs_value, PB_GUID_LENGTH+1);
				break;
			case TOK_password:
				Q_strncpyz(cs_password, cs_value, MAX_STRING_CHARS);
				break;
			case TOK_scriptName:
				Q_strncpyz(cs_scriptname, cs_value, MAX_STRING_CHARS);
				break;
			case TOK_respawn:
				Q_strncpyz(cs_respawn, cs_value, MAX_STRING_CHARS);
				break;
			case TOK_rate:
				Q_strncpyz(cs_rate, cs_value, MAX_STRING_CHARS);
				break;
			default:
				continue;
		}
	}

	if (!isBot) {
		// IP filtering
		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
		// recommanding PB based IP / GUID banning, the builtin system is pretty limited
		// check to see if they are on the banned IP list
		if ( G_FilterIPBanPacket(cs_ip) ) {
			return "You are banned from this server.";
		}

		// Check for fakep connections
		value = IsFakepConnection(clientNum, cs_ip, cs_rate);
		if ( value != NULL ) {
			// Too many connections from the same IP - don't permit the connection
			return value;
		}

		// josh: censor names
		// IRATA: added some static names in InitCensorNamesStructure
		if (g_censorNames.string[0]) {
			char censoredName[MAX_NETNAME];

			SanitizeString(cs_name, censoredName, qtrue);
			if (G_CensorText((char *)&censoredName,&censorNamesDictionary)) {
				G_LogPrintf( va("ClientConnect: Censored name \"%s\" (clientNum=%i, IP=%s)\n", cs_name, clientNum, cs_ip) );
				Info_SetValueForKey( userinfo, "name", censoredName);
				trap_SetUserinfo( clientNum, userinfo );
				if (g_censorPenalty.integer & CNSRPNLTY_KICK) {
					return "Name censor: Please change your name.";
				}
			}
		}

		// redeye/IRATA ext. ASCII chars check to avoid active players footkick on bad names connect
		for (i = 0; i < strlen(cs_name); ++i) {
			if (cs_name[i] < 0) {// extended ASCII chars have values between -128 and 0 (signed char)
				return "Bad name: Extended ASCII characters. Please change your name.";
			}
		}

		// check for shrubbot ban
		if(G_shrubbot_ban_check(cs_ip, cs_guid, reason)) {
			return va("You are banned from this server.\n%s", reason);
		}

		// Xian - check for max lives enforcement ban
		if( g_gametype.integer != GT_WOLF_LMS ) {
			if( g_enforcemaxlives.integer && (g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0) ) {
				if( trap_Cvar_VariableIntegerValue( "sv_punkbuster" ) ) {
					if ( G_FilterMaxLivesPacket ( cs_guid ) ) {
						return "Max Lives Enforcement Temp Ban. You will be able to reconnect when the next round starts. This ban is enforced to ensure you don't reconnect to get additional lives.";
					}
				}
				else {
					if ( G_FilterMaxLivesIPPacket ( cs_ip ) ) {
						return "Max Lives Enforcement Temp Ban. You will be able to reconnect when the next round starts. This ban is enforced to ensure you don't reconnect to get additional lives.";
					}
				}
			}
		}
		// End Xian

		// we don't check password for bots and local client
		// NOTE: local client <-> "ip" "localhost"
		//   this means this client is not running in our current process
		if ( strcmp(cs_ip, "localhost") != 0) {
			// check for a password
			if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) && strcmp( g_password.string, cs_password) != 0) {
				if( !sv_privatepassword.string[ 0 ] || strcmp( sv_privatepassword.string, cs_password ) ) {
					return "Invalid password.";
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
		if((g_XPSave.integer & XPSF_ENABLE) && firstTime &&
			(!trap_Cvar_VariableIntegerValue("sv_wwwDlDisconnected") ||
			 (g_XPSave.integer & XPSF_WIPE_DUP_GUID))) {

			for (i = 0; i < level.numConnectedClients; ++i) {
		   		int	 		clientNum2	= level.sortedClients[i];
				gclient_t*	cl 			= &level.clients[clientNum2];

				if(clientNum == clientNum2) continue;
				if(!Q_stricmp(cs_guid, cl->pers.cl_guid)) {
					G_LogPrintf( "Forcing disconnect of duplicate guid %s for client %i\n", cs_guid, clientNum2);
				// tjw: we need to disconnect the old
				trap_DropClient(clientNum2, va("disconnected duplicate guid for client %i", clientNum2), 0);
				}
			}
		}
	} // if (!isBot && !(ent->r.svFlags & SVF_BOT))

	// they can connect
	ent->client = level.clients + clientNum;
	client = ent->client;

	// Initialise the client object
	memset( client, 0, sizeof(*client) );

	client->pers.connected = CON_CONNECTING;
	client->pers.connectTime = level.time;			// DHM - Nerve

	// Set the client ip and guid
	Q_strncpyz(client->pers.client_ip, cs_ip, MAX_IP_LENGTH);
	Q_strncpyz(client->pers.cl_guid, cs_guid, PB_GUID_LENGTH+1);

	// tjw: -999 indicates that no XPSave lives were found this value
	//      will be updated in G_xpsave_load() otherwise
	ent->client->XPSave_lives = -999;

	// read or initialize the session data
	if ( firstTime ) {
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
	if(!client->XPSave_loaded) {
			client->XPSave_loaded = G_xpsave_load(ent);
	}

//mcwf GeoIP

//10.0.0.0/8			[RFC1918]
//172.16.0.0/12			[RFC1918]
//192.168.0.0/16		[RFC1918]
//169.254.0.0/16		[RFC3330] we need this ?


//query performance about ~2.6 Sec for 1 million requests p4 2.0 Ghz

	// redeye - comment out the following check to hide the country flag for bots
	if (gidb != NULL /*&& ! isBot*/) {

		value = Info_ValueForKey (userinfo, "ip");
		if (!strcmp( value, "localhost")) {
			client->sess.uci = 0;
		}
		else {
			unsigned long ip = GeoIP_addr_to_num(value);

			if (((ip & 0xFF000000) == 0x0A000000) ||
				((ip & 0xFFF00000) == 0xAC100000) ||
				((ip & 0xFFFF0000) == 0xC0A80000) ||
				(ip  == 0x7F000001)) {					// redeye - recognise also 127.0.0.1
				client->sess.uci = 0;
			}
			else {
				unsigned int ret = GeoIP_seek_record(gidb,ip);

				if (ret > 0) {
					client->sess.uci = ret;
				}
				else {
					client->sess.uci = 246;
					G_LogPrintf("GeoIP: This IP:%s cannot be located\n",value);
				}
			}
		}
	}
	else {
		client->sess.uci = 255; //Don't draw anything if DB error
	}
//mcwf GeoIP

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
	}
	else {
		client->pers.enterTime = level.time;
	}

	if( isBot ) {
		ent->s.number = clientNum;
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;

	}
	else if( firstTime ) {
		// force into spectator
		client->sess.sessionTeam = TEAM_SPECTATOR;
		client->sess.spectatorState = SPECTATOR_FREE;
		client->sess.spectatorClient = 0;

		// unlink the entity - just in case they were already connected
		trap_UnlinkEntity( ent );
	}

#ifdef ET_LUA
	// *LUA* API callbacks (check with Lua scripts)
	if ( G_LuaHook_ClientConnect(clientNum, firstTime, isBot, reason) ) {
		if ( !isBot && !(ent->r.svFlags & SVF_BOT) )
			return va("You are excluded from this server. %s\n", reason);
	}
#endif

	// get and distribute relevent paramters
	SanitizeString(cs_name, playername, qfalse);

	if (g_logOptions.integer & LOGOPTS_IP) {
		G_LogPrintf( va("ClientConnect: %i, IP: %s, name: \"%s\"\n", clientNum, client->pers.client_ip, playername) );
	}
	else {
		// also old way - no name
		G_LogPrintf( va("ClientConnect: %i\n", clientNum) );
	}

	G_UpdateCharacter( client );

#ifdef OMNIBOTS
	Bot_Event_ClientConnected(clientNum, isBot);
#endif

	ClientUserinfoChanged( clientNum );

	// don't do the "xxx connected" messages if they were caried over from previous level
	//		TAT 12/10/2002 - Don't display connected messages in single player
	if ( firstTime ) {
		trap_SendServerCommand( -1, va("cpm \"%s" S_COLOR_WHITE " connected\n\"", client->pers.netname) );
		if (!isBot) {
			lastconnected_client = clientNum;
		}
	}

	// count current clients and rank for scoreboard
	CalculateRanks( qtrue );
	client->ps.persistant[PERS_KILLS] = 0;

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
	if((float)(level.time - level.startTime) >= (g_timelimit.value * 60000.0f)) {
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
	gentity_t	*ent = g_entities + clientNum;
	gclient_t	*client = level.clients + clientNum;
	int			flags;
	int			spawn_count, lives_left;		// DHM - Nerve
	int			health = ent->health;
	qboolean	maxlives = (g_maxlives.integer || g_alliedmaxlives.integer || g_axismaxlives.integer);
	char		*cs_ip		= NULL;
	char 		*cs_name	= NULL;
	char		*cs_guid	= NULL;
	vec3_t		spawn_origin;
	gentity_t	*spawnPoint = NULL;

	//IlDuca - call LUA clientBegin only once when player connects
#ifdef ET_LUA
	qboolean firsttime = qfalse;
	if ( client->pers.connected == CON_CONNECTING ) {
		firsttime = qtrue;
	}
#endif

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

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
	}
	else {
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
	client->pers.lastteambleed_client = -1;
	client->pers.lastteambleed_dmg = -1;

	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawn_count;
	if(!maxlives) {
		client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
	}
	else {
		client->ps.persistant[PERS_RESPAWNS_LEFT] = lives_left;
	}


	client->pers.complaintClient = -1;
	client->pers.complaintEndTime = -1;

#ifdef OMNIBOTS
	//Omni-bot
	client->sess.botSuicide = qfalse; // make sure this is not set

	if ( ent->r.svFlags & SVF_BOT )
		client->sess.botPush = qtrue; // make sure this is set for bots
	else
		client->sess.botPush = qfalse;
#endif

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
				}
				else {
					client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
				}

				if( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
					if(client->sess.sessionTeam == TEAM_AXIS) {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = G_ComputeMaxLives(client, g_axismaxlives.integer);
					}
					else if(client->sess.sessionTeam == TEAM_ALLIES) {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = G_ComputeMaxLives(client, g_alliedmaxlives.integer);
					}
					else {
						client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
					}
 				}

				client->maxlivescalced = qtrue;
			}
			else {
				if( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
					if( client->sess.sessionTeam == TEAM_AXIS ) {
						if( client->ps.persistant[ PERS_RESPAWNS_LEFT ] > g_axismaxlives.integer ) {
							client->ps.persistant[ PERS_RESPAWNS_LEFT ] = g_axismaxlives.integer;
						}
					}
					else if( client->sess.sessionTeam == TEAM_ALLIES ) {
						if( client->ps.persistant[ PERS_RESPAWNS_LEFT ] > g_alliedmaxlives.integer ) {
							client->ps.persistant[ PERS_RESPAWNS_LEFT ] = g_alliedmaxlives.integer;
						}
					}
 				}
			}
		}
	}

	// core: set the mute status in the ConfigString CS_PLAYERS, so icons will show up..
	// Do this check for bots too.
	// This has to be done after the client's connection is set to CON_CONNECTED..
	cs_ip = va("%s",client->pers.client_ip);
	cs_guid = va("%s",client->pers.cl_guid);
	cs_name = va("%s",client->pers.netname);
	if ( G_shrubbot_mute_check(cs_ip, cs_guid) ) {
		int					pids[MAX_CLIENTS];
		gentity_t			*pidEnt = NULL;
		int					n;

		n = ClientNumbersFromString(cs_name, pids);
		pidEnt = &g_entities[pids[0]];
		if ( n == 1 ) {
			time_t			expiretime;

			if ( time(&expiretime) ) {
				int				seconds, i;

				for (i=0; g_shrubbot_mutes[i]; ++i) {
					if ( !Q_stricmp(g_shrubbot_mutes[i]->guid, cs_guid) && !Q_stricmp(g_shrubbot_mutes[i]->name, cs_name) ) {
						break;
					}
				}
				seconds = g_shrubbot_mutes[i]->expires - (expiretime - SHRUBBOT_BAN_EXPIRE_OFFSET);
				// core: fix: let the mute status be put in CS_PLAYERS configstring
				pidEnt->client->sess.muted = qtrue;
				pidEnt->client->sess.auto_mute_time = level.time + seconds*1000;
				pidEnt->client->sess.muted_by = MUTED_BY_SHRUB;
				ClientUserinfoChanged( pids[0] );
			}
		}

	}

	// core: check if there is any valid spawnpoint..
	if ( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) {
		vec3_t		spawn_angles;
		spawnPoint = SelectCTFSpawnPoint( client->sess.sessionTeam, client->pers.teamState.state, spawn_origin, spawn_angles, client->sess.spawnObjectiveIndex );
	}

	// locate ent at a spawn point
	if(!g_teamChangeKills.integer && health > 0) {
		ClientSpawn(ent, qfalse, qtrue, qfalse);
	}
	else {
		ClientSpawn(ent, qfalse, qtrue, qtrue);
	}

	// core: Put the player in limbo, if there was no valid spawnpoint..
	if ( (client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES) && !spawnPoint ) {
		ent->health = 0;
		ent->r.contents = CONTENTS_CORPSE;
		client->ps.pm_type = PM_DEAD;
		client->ps.stats[STAT_HEALTH] = 0;
		limbo( ent, qfalse );
	}

	if ( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) {
		client->inactivityTime = level.time + ( g_inactivity.integer ? g_inactivity.integer : 60 ) * 1000;
		client->inactivitySecondsLeft = (g_inactivity.integer)? g_inactivity.integer : 60;
	}
	else {
		client->inactivityTime = level.time + ( g_spectatorInactivity.integer ? g_spectatorInactivity.integer : 60 ) * 1000;
		client->inactivitySecondsLeft = (g_spectatorInactivity.integer)? g_spectatorInactivity.integer : 60;
	}

	// DHM - Nerve :: Start players in limbo mode if they change teams
	// during the match
	// tjw: don't kill the player unless they're dead already
	if(client->sess.sessionTeam != TEAM_SPECTATOR &&
		level.time - level.startTime > FRAMETIME * GAME_INIT_FRAMES) {

			// tjw: otherwise players lose 2 lives.
			// tjw: wtf does gametype have to do with it?
			if(g_gametype.integer != GT_WOLF_LMS && maxlives) {
				client->ps.persistant[PERS_RESPAWNS_LEFT]++;
			}

			if(health <= 0 || g_teamChangeKills.integer) {
				ent->health = 0;
				ent->r.contents = CONTENTS_CORPSE;
				client->ps.pm_type = PM_DEAD;
				client->ps.stats[STAT_HEALTH] = 0;

				limbo(ent, qfalse);
			}
	}

	if(client->sess.sessionTeam != TEAM_SPECTATOR) {
		trap_SendServerCommand( -1, va("print %i \"%s\"", GAMESTRING_ENTERED_THE_GAME, client->pers.netname) );
	}

	G_LogPrintf( "ClientBegin: %i\n", clientNum );

	// Xian - Check for maxlives enforcement
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if ( g_enforcemaxlives.integer == 1 && (g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0)) {
			char*		value = level.clients[clientNum].pers.cl_guid;

			G_LogPrintf( "EnforceMaxLives-GUID: %s\n", value );
			AddMaxLivesGUID( value );

			value = level.clients[clientNum].pers.client_ip;
			G_LogPrintf( "EnforceMaxLives-IP: %s\n", value );
			AddMaxLivesBan( value );
		}
	}
	// End Xian

	// count current clients and rank for scoreboard
	CalculateRanks( qtrue );

	// No surface determined yet.
	ent->surfaceFlags = 0;

#ifdef MV_SUPPORT
	// OSP
	G_smvUpdateClientCSList(ent);
	// OSP
#endif

#ifdef ET_LUA
	//IlDuca - call LUA clientBegin only once
	if ( firsttime == qtrue ) {
		// *LUA* API callbacks
		G_LuaHook_ClientBegin(clientNum);
	}
#endif
}


/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn( gentity_t *ent, qboolean revived, qboolean teamChange, qboolean restoreHealth)
{
	int					index = ent - g_entities;
	vec3_t				spawn_origin, spawn_angles;
	gclient_t			*client = ent->client;
	int					i;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	int					persistant[MAX_PERSISTANT];
	gentity_t			*spawnPoint;
	int					flags;
	int					savedPing;
	int					savedTeam;
	int					savedClassWeaponTime;
	int					savedDeathTime;
	qboolean			doWar = (nq_War.integer & WARMODE_ENABLE)? qtrue : qfalse;

	G_UpdateSpawnCounts();

	client->pers.lastSpawnTime = level.time;
	client->pers.lastBattleSenseBonusTime = level.timeCurrent;
	client->pers.lastHQMineReportTime = level.timeCurrent;
	client->pers.lastHQArtyReportTime = level.timeCurrent;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if( revived ) {
		spawnPoint = ent;
		VectorCopy( ent->r.currentOrigin, spawn_origin );
		spawn_origin[2] += 9;	// spawns seem to be sunk into ground?
		VectorCopy( ent->s.angles, spawn_angles );
	}
	else {
		// Arnout: let's just be sure it does the right thing at all times. (well maybe not the right thing, but at least not the bad thing!)
		//if( client->sess.sessionTeam == TEAM_SPECTATOR || client->sess.sessionTeam == TEAM_FREE ) {
		if( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES ) {
			spawnPoint = SelectSpectatorSpawnPoint( spawn_origin, spawn_angles );
		}
		else {
			spawnPoint = SelectCTFSpawnPoint( client->sess.sessionTeam, client->pers.teamState.state, spawn_origin, spawn_angles, client->sess.spawnObjectiveIndex );
			// core: if there was no valid spawnpoint, make them spectator..
			// This should never happen. (safety)..
			if ( !spawnPoint ) {
				SetTeam( ent, "s", qtrue, -1, -1, qfalse );
			}
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
	savedClassWeaponTime = client->ps.classWeaponTime;
	savedDeathTime = client->deathTime;
	// END		xkan, 8/27/2002

	for( i = 0 ; i < MAX_PERSISTANT ; ++i ) {
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

	for( i = 0 ; i < MAX_PERSISTANT ; ++i ) {
		client->ps.persistant[i] = persistant[i];
	}

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	if( revived ) {
		client->ps.persistant[PERS_REVIVE_COUNT]++;
	}
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;
	client->ps.persistant[PERS_HWEAPON_USE] = 0;

	if ( client->sess.skillBits[SK_BATTLE_SENSE] & (1<<6) ) {
		client->airOutTime = level.time + HOLDBREATHTIME_LONG;
		// core: air left while underwater..
		ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
		ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME_LONG;
	}
	else {
		client->airOutTime = level.time + HOLDBREATHTIME;
		// core: air left while underwater..
		ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
		ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME;
	}

	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	client->ps.eFlags = flags;

	// tjw: these are saved in case of SLASHKILL_SAMECHARGE
	client->ps.classWeaponTime = savedClassWeaponTime;
	client->deathTime = savedDeathTime;
	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	ent->classnamehash = PLAYER_HASH;
	ent->r.contents = CONTENTS_BODY;

	ent->clipmask = MASK_PLAYERSOLID;

	// DHM - Nerve :: Init to -1 on first spawn;
	if ( !revived ) {
		ent->props_frame_state = -1;
	}
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;

	// so resuscitation will work more properly
	ent->reviveVal = 0;

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

	// core: reset killercam, so it will work again..
	client->pmext.viewlocked_killer = 1;

	// TTimo
	// retrieve from the persistant storage (we use this in pmoveExt_t beause we need it in bg_*)
	client->pmext.bAutoReload = client->pers.bAutoReloadAux;
	// done

	client->pmext.weapAltReload = client->pers.weapAltReloadAux;

	client->ps.clientNum = index;

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );	// NERVE - SMF - moved this up here

	// DHM - Nerve :: Add appropriate weapons
	if ( !revived ) {
		qboolean update = qfalse;

		if( client->sess.playerType != client->sess.latchPlayerType ) {
			client->sess.playerType = client->sess.latchPlayerType;
			update = qtrue;
		}

		if ( !doWar ) {
			if( G_IsWeaponDisabled( ent, client->sess.latchPlayerWeapon, qtrue ) ) {
				bg_playerclass_t* classInfo = BG_PlayerClassForPlayerState( &ent->client->ps );
				client->sess.latchPlayerWeapon = classInfo->classWeapons[0];
				update = qtrue;
			}
		}

		if( client->sess.playerWeapon != client->sess.latchPlayerWeapon ) {
			client->sess.playerWeapon = client->sess.latchPlayerWeapon;
			update = qtrue;
		}

		if( G_IsClassDisabled( ent, client->sess.playerType, qtrue ) ) {
			qboolean found=qfalse;

			for ( i=0; i<NUM_PLAYER_CLASSES; ++i) {
				if (!G_IsClassDisabled( ent, i, qtrue )) {
					client->sess.latchPlayerType = client->sess.playerType = i;
					found = qtrue;
					break;
				}
			}
			// no other option, have to kick him to spectator
			if ( !found )
				SetTeam( ent, "s", qtrue, -1, -1, qfalse );

			update = qtrue;
		}

		if ( !doWar ) {
			if( G_IsWeaponDisabled( ent, client->sess.playerWeapon, qtrue ) ) {
				bg_playerclass_t* classInfo = BG_PlayerClassForPlayerState( &ent->client->ps );
				client->sess.playerWeapon = classInfo->classWeapons[0];
				update = qtrue;
			}
		}


		if( client->sess.playerWeapon2 != client->sess.latchPlayerWeapon2 ) {
			client->sess.playerWeapon2 = client->sess.latchPlayerWeapon2;
			update = qtrue;
		}

		// core: riflenade detached..
		client->pmext.silencedSideArm &= ~2;
		if ( client->sess.rifleNade ) {
			client->sess.rifleNade = 0;
			update = qtrue;
		}

		if( update || teamChange ) {
			ClientUserinfoChanged( index );
		}
	}

	// TTimo keep it isolated from spectator to be safe still
	if( client->sess.sessionTeam != TEAM_SPECTATOR ) {
		// Xian - Moved the invul. stuff out of SetWolfSpawnWeapons and put it here for clarity
		if ( g_medics.integer & MEDIC_FASTRESUSCITATIONING && revived )
			client->ps.powerups[PW_INVULNERABLE] = level.time + 1000;
		else if(revived)
			client->ps.powerups[PW_INVULNERABLE] = level.time + 3000;
		else {
			if(client->sess.sessionTeam == TEAM_AXIS) {
				if (g_axisSpawnInvul.integer) {
					client->ps.powerups[PW_INVULNERABLE] = level.time + (g_axisSpawnInvul.integer * 1000);
				}
				else {
					client->ps.powerups[PW_INVULNERABLE] = level.time + (g_spawnInvul.integer * 1000);
				}
			}
			else if (client->sess.sessionTeam == TEAM_ALLIES) {
				if (g_alliedSpawnInvul.integer) {
					client->ps.powerups[PW_INVULNERABLE] = level.time + (g_alliedSpawnInvul.integer * 1000);
				}
				else {
					client->ps.powerups[PW_INVULNERABLE] = level.time + (g_spawnInvul.integer * 1000);
				}
			}
			else {
				client->ps.powerups[PW_INVULNERABLE] = level.time + (g_spawnInvul.integer * 1000);
			}
		}
	}
	// End Xian

	G_UpdateCharacter( client );

	SetWolfSpawnWeapons( client );

	// START	Mad Doctor I changes, 8/17/2002

	// JPW NERVE ***NOTE*** the following line is order-dependent and must *FOLLOW* SetWolfSpawnWeapons() in multiplayer
	// JPW NERVE -- increases stats[STAT_MAX_HEALTH] based on # of medics in game
	AddMedicTeamBonus( client );

	// END		Mad Doctor I changes, 8/17/2002

	if( !revived ) {
		client->pers.cmd.weapon = ent->client->ps.weapon;
	}
// dhm - end

	if(restoreHealth) {
		if( client->sess.skillBits[SK_BATTLE_SENSE] & (1<<3) )
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
	}
	else {
		//bani - #245 - we try to orient them in the freelook direction when revived
		vec3_t	newangle;

		newangle[YAW] = SHORT2ANGLE( ent->client->pers.cmd.angles[YAW] + ent->client->ps.delta_angles[YAW] );
		newangle[PITCH] = 0;
		newangle[ROLL] = 0;

		SetClientViewAngle( ent, newangle );
	}

	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		//G_KillBox( ent );
		trap_LinkEntity (ent);
	}

	client->respawnTime = level.timeCurrent;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->inactivityWarning = qfalse;
	client->inactivitySecondsLeft = (g_inactivity.integer)? g_inactivity.integer : 60;
	client->latched_buttons = 0;
	client->latched_wbuttons = 0;	//----(SA)	added

	// xkan, 1/13/2003 - reset death time
	client->deathTime = 0;

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	}
	else {
		// fire the targets of the spawn point
		if ( !revived ) {
			G_UseTargets( spawnPoint, ent );
		}
	}

#ifdef ET_LUA
	// *LUA* API callbacks
	G_LuaHook_ClientSpawn( ent-g_entities, revived, teamChange, restoreHealth );
#endif

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;

	// add spawn event
	G_AddEvent( ent, EV_SPAWN, 0 );

	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, level.time, /*g_fixedphysics.integer ? qfalse :*/ qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// set idle animation on weapon
	ent->client->ps.weapAnim = ( ( ent->client->ps.weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | PM_IdleAnimForWeapon( ent->client->ps.weapon );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, level.time, /*g_fixedphysics.integer ? qfalse :*/ qtrue );

	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
	G_ResetMarkers( ent );

	// RF, start the scripting system
	if (!revived && client->sess.sessionTeam != TEAM_SPECTATOR) {
		G_Script_ScriptEvent( ent, SE_PLAYERSTART, "" );
	}

	// core: when switching teams, reset the commandmap/radar icons,
	// so your own (meanwhile invalid) landmine-markers don't show up..
	if ( teamChange ) {
		G_ResetTeamMapData();
	}
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
	gentity_t	*ent = g_entities + clientNum;
	gentity_t	*flag=NULL;
	gitem_t		*item=NULL;
	vec3_t		launchvel;
	int			i;
	char 		playername[MAX_NETNAME] = "";

	if ( !ent->client ) {
		return;
	}

#ifdef ET_LUA
	// *LUA* API callbacks
	G_LuaHook_ClientDisconnect(clientNum);
#endif

#ifdef OMNIBOTS
	Bot_Event_ClientDisConnected(clientNum);
#endif

	G_xpsave_add(ent);

	G_RemoveClientFromFireteams( clientNum, qtrue, qfalse );
	G_RemoveFromAllIgnoreLists( clientNum );
	G_LeaveTank( ent, qfalse );

	// stop any following clients
	for ( i = 0 ; i < level.numConnectedClients ; ++i ) {
		flag = g_entities + level.sortedClients[i];
		if ( flag->client->sess.sessionTeam == TEAM_SPECTATOR
			&& flag->client->sess.spectatorState == SPECTATOR_FOLLOW
			&& flag->client->sess.spectatorClient == clientNum ) {
			StopFollowing( flag );
		}
		if ( flag->client->ps.pm_flags & PMF_LIMBO
			&& flag->client->sess.spectatorClient == clientNum ) {
			Cmd_FollowCycle_f( flag, 1, qfalse );
		}
		// NERVE - SMF - remove complaint client
		if ( flag->client->pers.complaintEndTime > level.time && flag->client->pers.complaintClient == clientNum ) {
			flag->client->pers.complaintClient = -1;
			flag->client->pers.complaintEndTime = -1;

			CPx( level.sortedClients[i], "complaint -2" );
		}
	}

	if( g_landminetimeout.integer ) {
		G_FadeLandmines( ent );
	}
	G_FadeSatchels( ent );
	G_FadeAirstrikes( ent );
	G_FadeArties( ent );

	// jaquboss
	if ( jp_insanity.integer & JP_INSANITY_MORTARCAM ) {
		G_FadeMortars( ent );
	}
	if (jp_insanity.integer & JP_INSANITY_MISSILECAM ) {
		G_FadePanzerfausts( ent );
		G_FadeBazookas( ent );
	}

	// remove ourself from teamlists
	{
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList;

		for( i = 0; i < 2; ++i ) {
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
				item = &bg_itemlist[IL_TEAM_TEAM_CTF_REDFLAG];
				ent->client->ps.powerups[PW_REDFLAG] = 0;
			}
			if (ent->client->ps.powerups[PW_BLUEFLAG]) {
				item = &bg_itemlist[IL_TEAM_TEAM_CTF_BLUEFLAG];
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

	SanitizeString( ent->client->pers.netname, playername, qfalse );

	if (g_logOptions.integer & LOGOPTS_IP) {
		G_LogPrintf( va("ClientDisconnect: %i, IP: %s, name: \"%s\"\n", clientNum, ent->client->pers.client_ip, playername) );
	}
	else {
		// Old way - no name
		G_LogPrintf( va("ClientDisconnect: %i\n", clientNum) );
	}

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
	CalculateRanks( qtrue );

	// OSP
	G_verifyMatchState(i);
#ifdef MV_SUPPORT
	G_smvAllRemoveSingleClient(ent - g_entities);
#endif
	// OSP

}

// In just the GAME DLL, we want to store the groundtrace surface stuff,
// so we don't have to keep tracing.
void ClientStoreSurfaceFlags(int clientNum, int surfaceFlags)
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

	if (hitEnt->client->ps.eFlags & EF_DEAD)	return 4;
	if (hitEnt->client->ps.eFlags & EF_PLAYDEAD) return 4;
	if (hitEnt->client->ps.eFlags & EF_PRONE) return 4;
	if (hitEnt->client->ps.eFlags & EF_CROUCHING)	return 24;

	return 36;
}
