/*
 * name:		g_teammapdata.c
 *
 * desc:
 *
 */

#include "g_local.h"

/*
===================
G_PushMapEntityToBuffer
===================
*/
void G_PushMapEntityToBuffer( char* buffer, int size, mapEntityData_t *mEnt ) {
	char buf[32];

	// core: we only send "entnfo" for entities that change position continuesly..
	switch( mEnt->type ) {
		case ME_PLAYER:
		case ME_PLAYER_REVIVE:
		case ME_PLAYER_DISGUISED:
		case ME_TANK:
			break;
		default:
			return;
	}

	// CHRUKER: b043 - These also needs to be send so that icons can be displayed correct command map layer
	if( level.ccLayers ) {
		Com_sprintf( buf, sizeof( buf ), "%i %i %i", ((int)mEnt->org[0]) >> 7, ((int)mEnt->org[1]) >> 7, ((int)mEnt->org[2]) >> 7 );	// for integers: >>7 is same as /128
	}
	else {
		Com_sprintf( buf, sizeof( buf ), "%i %i", ((int)mEnt->org[0]) >> 7, ((int)mEnt->org[1]) >> 7 );
	}

	if ( mEnt->type == ME_TANK ) {
		Q_strcat( buffer, size, va(" %i %s %i", mEnt->type, buf, mEnt->data ) );
	} else {
		Q_strcat( buffer, size, va(" %i %s %i %i", mEnt->type, buf, mEnt->yaw, mEnt->data ) );
	}
}

/*
===================
core:
	G_PushMapEntityToBuffer2

This version handles all the entities that do not change positions continuesly.
Since the pushed data for those entities only changes when that entity is switching state (ME_CONSTRUCT is constructed, ME_TANK is damaged, ME_LANDMINE defused etc etc.),
most of the time that data is the same as the last time it was sent to clients.
We only want to send the data, if it has changed..
===================
*/
void G_PushMapEntityToBuffer2( char* buffer, int size, mapEntityData_t *mEnt ) {
	char buf[32];

	// core: we only send "entnfo2" for entities that do not change position continuesly..
	switch( mEnt->type ) {
		case ME_PLAYER:
		case ME_PLAYER_REVIVE:
		case ME_PLAYER_DISGUISED:
		case ME_TANK:
			return;
	}

	// CHRUKER: b043 - These also needs to be send so that icons can be displayed correct command map layer
	if( level.ccLayers ) {
		Com_sprintf( buf, sizeof( buf ), "%i %i %i", ((int)mEnt->org[0]) >> 7, ((int)mEnt->org[1]) >> 7, ((int)mEnt->org[2]) >> 7 );	// for integers: >>7 is same as /128
	}
	else {
		Com_sprintf( buf, sizeof( buf ), "%i %i", ((int)mEnt->org[0]) >> 7, ((int)mEnt->org[1]) >> 7 );
	}

	Q_strcat( buffer, size, va(" %i %s %i", mEnt->type, buf, mEnt->data ) );
}

/*
===================
G_InitMapEntityData
===================
*/
void G_InitMapEntityData( mapEntityData_Team_t *teamList ) {
	int		i;
	mapEntityData_t *trav, *lasttrav;

	memset( teamList, 0, sizeof(mapEntityData_Team_t) );

	teamList->activeMapEntityData.next = &teamList->activeMapEntityData;
	teamList->activeMapEntityData.prev = &teamList->activeMapEntityData;
	teamList->freeMapEntityData = teamList->mapEntityData_Team;

	for ( i = 0, trav = teamList->mapEntityData_Team+1, lasttrav = teamList->mapEntityData_Team ; i < MAX_GENTITIES - 1 ; i++, trav++ ) {
		lasttrav->next = trav;
		lasttrav = trav;
	}
}

/*
==================
G_FreeMapEntityData

returns next entity in the array
==================
*/
mapEntityData_t *G_FreeMapEntityData( mapEntityData_Team_t *teamList, mapEntityData_t *mEnt ) {
	mapEntityData_t *ret = mEnt->next;

	if ( !mEnt->prev ) {
		G_Error( "G_FreeMapEntityData: not active" );
	}

	// remove from the doubly linked active list
	mEnt->prev->next = mEnt->next;
	mEnt->next->prev = mEnt->prev;

	// the free list is only singly linked
	mEnt->next = teamList->freeMapEntityData;
	teamList->freeMapEntityData = mEnt;

	return( ret );
}

/*
===================
G_AllocMapEntityData
===================
*/
mapEntityData_t *G_AllocMapEntityData( mapEntityData_Team_t *teamList ) {
	mapEntityData_t	*mEnt;

	if ( !teamList->freeMapEntityData ) {
		// no free entities - bomb out
		G_Error( "G_AllocMapEntityData: out of entities" );
	}

	mEnt = teamList->freeMapEntityData;
	teamList->freeMapEntityData = teamList->freeMapEntityData->next;

	memset( mEnt, 0, sizeof( *mEnt ) );

	mEnt->singleClient = -1;

	// link into the active list
	mEnt->next = teamList->activeMapEntityData.next;
	mEnt->prev = &teamList->activeMapEntityData;
	teamList->activeMapEntityData.next->prev = mEnt;
	teamList->activeMapEntityData.next = mEnt;
	return mEnt;
}

/*
===================
G_FindMapEntityData
===================
*/
mapEntityData_t *G_FindMapEntityData( mapEntityData_Team_t *teamList, int entNum ) {
	mapEntityData_t *mEnt;

	for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if( mEnt->singleClient >= 0 )
			continue;
		if( entNum == mEnt->entNum )
			return( mEnt );
	}

	// not found
	return( NULL );
}

/*
===============================
G_FindMapEntityDataSingleClient
===============================
*/
mapEntityData_t *G_FindMapEntityDataSingleClient( mapEntityData_Team_t *teamList, mapEntityData_t *start, int entNum, int clientNum ) {
	mapEntityData_t *mEnt;

	if( start )
		mEnt = start->next;
	else
		mEnt = teamList->activeMapEntityData.next;

	for( ; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if( clientNum == -1 ) {
			if( mEnt->singleClient < 0 )
				continue;
		}
		else if( mEnt->singleClient >= 0 && clientNum != mEnt->singleClient )
			continue;
		if( entNum == mEnt->entNum )
			return( mEnt );
	}

	// not found
	return( NULL );
}


void G_RemoveTeamMapData( gentity_t *ent, qboolean forceDisguised, qboolean forceAxis, qboolean forceAllied )
{
	mapEntityData_t			*mEnt;
	mapEntityData_Team_t	*teamList;
	mapEntityData_t			*mEntFree;
	int						i;

	for( i = 0; i < 2; ++i ) {
		teamList = &mapEntityData[i];

		// anything but disguised players..
		if ( (forceAxis && i == 0) || (forceAllied && i == 1) ) {
			if((mEnt = G_FindMapEntityData(teamList, ent-g_entities)) != NULL) {
				G_FreeMapEntityData( teamList, mEnt );
			}
		}

		// disguised players..
		if ( forceDisguised ) {
			mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, ent->s.number, -1 );
			while( mEnt ) {
				mEntFree = mEnt;
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, ent->s.number, -1 );
				G_FreeMapEntityData( teamList, mEntFree );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////

// some culling bits
typedef struct plane_s {
	vec3_t	normal;
	float	dist;
} plane_t;

static plane_t frustum[4];

/*
========================
G_SetupFrustum
========================
*/
void G_SetupFrustum( gentity_t* ent ) {
	float	xs, xc;
	float	ang;
	vec3_t	axis[3];
	vec3_t	vieworg;

	ang = DEG2RAD(90) * 0.5f;
	SinCos( ang, xs,xc );

	AnglesToAxis( ent->client->ps.viewangles, axis );

	VectorScale( axis[0], xs, frustum[0].normal );
	VectorMA( frustum[0].normal, xc, axis[1], frustum[0].normal );

	VectorScale( axis[0], xs, frustum[1].normal );
	VectorMA( frustum[1].normal, -xc, axis[1], frustum[1].normal );

	// core: ?! This is already calculated..
	//	ang = (90 / 180.f)* M_PI * 0.5f;
	//	xs = sin( ang );
	//	xc = cos( ang );

	VectorScale( axis[0], xs, frustum[2].normal );
	VectorMA( frustum[2].normal, xc, axis[2], frustum[2].normal );

	VectorScale( axis[0], xs, frustum[3].normal );
	VectorMA( frustum[3].normal, -xc, axis[2], frustum[3].normal );

	VectorCopy(ent->client->ps.origin, vieworg);
	vieworg[2] += ent->client->ps.viewheight;

	frustum[0].dist = DotProduct( vieworg, frustum[0].normal );
	frustum[1].dist = DotProduct( vieworg, frustum[1].normal );
	frustum[2].dist = DotProduct( vieworg, frustum[2].normal );
	frustum[3].dist = DotProduct( vieworg, frustum[3].normal );
}

void G_SetupFrustum_ForBinoculars( gentity_t* ent )
{
// TAT 12/26/2002 - Give bots a larger view angle through binoculars than players get - this should help the
//		landmine detection...
#define BINOCULAR_ANGLE 10.0f
#define BOT_BINOCULAR_ANGLE 60.0f
	float	xs, xc;
	float	ang;
	vec3_t	axis[3];
	vec3_t	vieworg;
	float	baseAngle = (ent->r.svFlags & SVF_BOT)? BOT_BINOCULAR_ANGLE : BINOCULAR_ANGLE;

	ang = DEG2RAD(baseAngle) * 0.5;
	SinCos( ang, xs,xc );

	AnglesToAxis( ent->client->ps.viewangles, axis );

	VectorScale( axis[0], xs, frustum[0].normal );
	VectorMA( frustum[0].normal, xc, axis[1], frustum[0].normal );

	VectorScale( axis[0], xs, frustum[1].normal );
	VectorMA( frustum[1].normal, -xc, axis[1], frustum[1].normal );

	// core: ?! This is already calculated..
	//	ang = (baseAngle / 180.f)* M_PI * 0.5f;
	//	xs = sin( ang );
	//	xc = cos( ang );

	VectorScale( axis[0], xs, frustum[2].normal );
	VectorMA( frustum[2].normal, xc, axis[2], frustum[2].normal );

	VectorScale( axis[0], xs, frustum[3].normal );
	VectorMA( frustum[3].normal, -xc, axis[2], frustum[3].normal );

	VectorCopy(ent->client->ps.origin, vieworg);
	vieworg[2] += ent->client->ps.viewheight;

	frustum[0].dist = DotProduct( vieworg, frustum[0].normal );
	frustum[1].dist = DotProduct( vieworg, frustum[1].normal );
	frustum[2].dist = DotProduct( vieworg, frustum[2].normal );
	frustum[3].dist = DotProduct( vieworg, frustum[3].normal );
}

/*
========================
G_CullPointAndRadius - returns true if not culled
========================
*/
static qboolean G_CullPointAndRadius( vec3_t pt, float radius ) {
	float	dist = DotProduct( pt, frustum[0].normal) - frustum[0].dist;

	if ( dist < -radius || dist <= radius ) return( qfalse );

	dist = DotProduct( pt, frustum[1].normal) - frustum[1].dist;
	if ( dist < -radius || dist <= radius ) return( qfalse );

	dist = DotProduct( pt, frustum[2].normal) - frustum[2].dist;
	if ( dist < -radius || dist <= radius ) return( qfalse );

	dist = DotProduct( pt, frustum[3].normal) - frustum[3].dist;
	if ( dist < -radius || dist <= radius ) return( qfalse );

	return( qtrue );
}


qboolean G_VisibleFromBinoculars( gentity_t* viewer, gentity_t* ent, vec3_t origin ) {
	vec3_t vieworg;
	trace_t trace;

	VectorCopy(viewer->client->ps.origin, vieworg);
	vieworg[2] += viewer->client->ps.viewheight;

	if(!G_CullPointAndRadius( origin, 0 )) {
		return qfalse;
	}

	if(!trap_InPVS( vieworg, origin )) {
		return qfalse;
	}

	trap_Trace( &trace, vieworg, NULL, NULL, origin, viewer->s.number, MASK_SHOT );

	if( trace.fraction != 1.f ) {
		if( ent ) {
			if( trace.entityNum != ent->s.number ) {
				return qfalse;
			}
			else {
				return qtrue;
			}
		}
		else {
			return qfalse;
		}
	}

	return qtrue;
}

qboolean G_VisibleFromBinoculars_Box( gentity_t* viewer, gentity_t* ent, vec3_t origin, vec3_t mins, vec3_t maxs )
{
	vec3_t vieworg;
	trace_t trace;

	VectorCopy(viewer->client->ps.origin, vieworg);
	vieworg[2] += viewer->client->ps.viewheight;

	if(!G_CullPointAndRadius( origin, 0 )) {
		return qfalse;
	}

	if(!trap_InPVS( vieworg, origin )) {
		return qfalse;
	}

	trap_Trace( &trace, vieworg, mins, maxs, origin, viewer->s.number, MASK_SHOT );

	if( trace.fraction != 1.f ) {
		if( ent ) {
			if( trace.entityNum != ent->s.number ) {
				return qfalse;
			}
			else {
				return qtrue;
			}
		}
		else {
			return qfalse;
		}
	}

	return qtrue;
}

void G_ResetTeamMapData() {
	G_InitMapEntityData( &mapEntityData[0] );
	G_InitMapEntityData( &mapEntityData[1] );
}

void G_UpdateTeamMapData_Construct(gentity_t* ent) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList;
	mapEntityData_t*		mEnt;

	if( ent->s.teamNum == 3 ) {
		teamList = &mapEntityData[0];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->type = ME_CONSTRUCT;
		mEnt->startTime = level.time;
		mEnt->yaw = 0;

		teamList = &mapEntityData[1];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->type = ME_CONSTRUCT;
		mEnt->startTime = level.time;
		mEnt->yaw = 0;

		return;
	}

	if( ent->s.teamNum == TEAM_AXIS ) {
		teamList = &mapEntityData[0];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->type = ME_CONSTRUCT;
		mEnt->startTime = level.time;
		mEnt->yaw = 0;
	}
	else if( ent->s.teamNum == TEAM_ALLIES ) {
		teamList = &mapEntityData[1];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->type = ME_CONSTRUCT;
		mEnt->startTime = level.time;
		mEnt->yaw = 0;
	}
}

void G_UpdateTeamMapData_Tank(gentity_t* ent) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList = &mapEntityData[0];
	mapEntityData_t*		mEnt = G_FindMapEntityData( teamList, num );

	if( !mEnt ) {
		mEnt = G_AllocMapEntityData( teamList );
		mEnt->entNum = num;
	}
	VectorCopy( ent->s.pos.trBase, mEnt->org );
	mEnt->data = ent->s.modelindex2;
	mEnt->startTime = level.time;
	if(ent->s.eType == ET_TANK_INDICATOR_DEAD) {
		mEnt->type = ME_TANK_DEAD;
	}
	else {
		mEnt->type = ME_TANK;
	}
	mEnt->yaw = 0;

	teamList = &mapEntityData[1];
	mEnt = G_FindMapEntityData( teamList, num );
	if( !mEnt ) {
		mEnt = G_AllocMapEntityData( teamList );
		mEnt->entNum = num;
	}
	VectorCopy( ent->s.pos.trBase, mEnt->org );
	mEnt->data = ent->s.modelindex2;
	mEnt->startTime = level.time;
	if(ent->s.eType == ET_TANK_INDICATOR_DEAD) {
		mEnt->type = ME_TANK_DEAD;
	}
	else {
		mEnt->type = ME_TANK;
	}
	mEnt->yaw = 0;
}

void G_UpdateTeamMapData_Destruct(gentity_t* ent) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList;
	mapEntityData_t*		mEnt;

	if( ent->s.teamNum == TEAM_AXIS ) {
		teamList = &mapEntityData[1];	// inverted
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->startTime = level.time;
		mEnt->type = ME_DESTRUCT;
		mEnt->yaw = 0;
	}
	else {
		if( ent->parent->target_ent && ( ent->parent->target_ent->s.eType == ET_CONSTRUCTIBLE || ent->parent->target_ent->s.eType == ET_EXPLOSIVE ) ) {
			if( ent->parent->spawnflags & ((1 << 6) | (1 << 4))) {
				teamList = &mapEntityData[1];	// inverted
				mEnt = G_FindMapEntityData( teamList, num );
				if( !mEnt ) {
					mEnt = G_AllocMapEntityData( teamList );
					mEnt->entNum = num;
				}
				VectorCopy( ent->s.pos.trBase, mEnt->org );
				mEnt->data = mEnt->entNum; //ent->s.modelindex2;
				mEnt->startTime = level.time;
				mEnt->type = ME_DESTRUCT_2;
				mEnt->yaw = 0;
			}
		}
	}

	if(ent->s.teamNum == TEAM_ALLIES  ) {
		teamList = &mapEntityData[0];	// inverted
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->s.pos.trBase, mEnt->org );
		mEnt->data = mEnt->entNum; //ent->s.modelindex2;
		mEnt->startTime = level.time;
		mEnt->type = ME_DESTRUCT;
		mEnt->yaw = 0;
	}
	else {
		if( ent->parent->target_ent && ( ent->parent->target_ent->s.eType == ET_CONSTRUCTIBLE || ent->parent->target_ent->s.eType == ET_EXPLOSIVE ) ) {
			if( ent->parent->spawnflags & ((1 << 6) | (1 << 4)) ) {
				teamList = &mapEntityData[0];	// inverted
				mEnt = G_FindMapEntityData( teamList, num );
				if( !mEnt ) {
					mEnt = G_AllocMapEntityData( teamList );
					mEnt->entNum = num;
				}
				VectorCopy( ent->s.pos.trBase, mEnt->org );
				mEnt->data = mEnt->entNum; //ent->s.modelindex2;
				mEnt->startTime = level.time;
				mEnt->type = ME_DESTRUCT_2;
				mEnt->yaw = 0;
			}
		}
	}
}

void G_UpdateTeamMapData_Player(gentity_t* ent, qboolean forceAllied, qboolean forceAxis) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList;
	mapEntityData_t*		mEnt;

	if ( !ent->client ) {
		return;
	}

	if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
		return;
	}

	switch(ent->client->sess.sessionTeam) {
		case TEAM_AXIS:
			if ( ent->client->ps.powerups[PW_OPS_DISGUISED] && ent->client->sess.skillBits[SK_COVERTOPS]& (1<<8) ) {
				forceAllied = qtrue;
			}
			forceAxis = qtrue;
			break;

		case TEAM_ALLIES:
			if ( ent->client->ps.powerups[PW_OPS_DISGUISED] && ent->client->sess.skillBits[SK_COVERTOPS] & (1<<8) ) {
				forceAxis = qtrue;
			}
			forceAllied = qtrue;
			break;

		default:
			break;
	}

	if (forceAxis) {
		teamList = &mapEntityData[0];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->client->ps.origin, mEnt->org );
		mEnt->yaw = ent->client->ps.viewangles[YAW];
		mEnt->data = num;
		mEnt->startTime = level.time;
		mEnt->type = (ent->health <= 0)? ME_PLAYER_REVIVE : ME_PLAYER;
	}

	if (forceAllied) {
		teamList = &mapEntityData[1];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->client->ps.origin, mEnt->org );
		mEnt->yaw = ent->client->ps.viewangles[YAW];
		mEnt->data = num;
		mEnt->startTime = level.time;
		mEnt->type = (ent->health <= 0)? ME_PLAYER_REVIVE : ME_PLAYER;
	}
}

static void G_UpdateTeamMapData_DisguisedPlayer( gentity_t* spotter, gentity_t* ent, qboolean forceAllied, qboolean forceAxis ) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList;
	mapEntityData_t*		mEnt;

	if ( !ent->client ) {
		return;
	}

	if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
		return;
	}

	switch( ent->client->sess.sessionTeam ) {
		case TEAM_AXIS:
			forceAxis = qtrue;
			break;

		case TEAM_ALLIES:
			forceAllied = qtrue;
			break;

		default:
			break;
	}

	if ( forceAxis ) {
		teamList = &mapEntityData[0];
		mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, num, spotter->s.clientNum );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
			mEnt->singleClient = spotter->s.clientNum;
		}
		VectorCopy( ent->client->ps.origin, mEnt->org );
		mEnt->yaw = ent->client->ps.viewangles[YAW];
		mEnt->data = num;
		mEnt->startTime = level.time;
		mEnt->type = ME_PLAYER_DISGUISED;
	}

	if ( forceAllied ) {
		teamList = &mapEntityData[1];
		mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, num, spotter->s.clientNum );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
			mEnt->singleClient = spotter->s.clientNum;
		}
		VectorCopy( ent->client->ps.origin, mEnt->org );
		mEnt->yaw = ent->client->ps.viewangles[YAW];
		mEnt->data = num;
		mEnt->startTime = level.time;
		mEnt->type = ME_PLAYER_DISGUISED;
	}
}

void G_UpdateTeamMapData_LandMine(gentity_t* ent) {
	int						num = ent-g_entities;
	mapEntityData_Team_t*	teamList;
	mapEntityData_t			*mEnt;
	int						team = ent->s.teamNum & 3; //ent->s.teamNum % 4

	if ( !(ent->s.teamNum < 4 || ent->s.teamNum >= 8) ) return;	// must be armed..

	// inversed teamlists, we want to see the enemy mines
	if ( ent->s.modelindex2 ) { // must be spotted..
		teamList = &mapEntityData[ (team==TEAM_AXIS)?1:0 ];
		mEnt = G_FindMapEntityData( teamList, num );
		if( !mEnt ) {
			mEnt = G_AllocMapEntityData( teamList );
			mEnt->entNum = num;
		}
		VectorCopy( ent->r.currentOrigin, mEnt->org );
		mEnt->data = team;
		mEnt->startTime = level.time;
		mEnt->type = ME_LANDMINE;
	}

	// team mines..
	teamList = &mapEntityData[ (team==TEAM_AXIS)?0:1 ];
	mEnt = G_FindMapEntityData( teamList, num );
	if( !mEnt ) {
		mEnt = G_AllocMapEntityData( teamList );
		mEnt->entNum = num;
	}
	VectorCopy( ent->r.currentOrigin, mEnt->org );
	mEnt->data = team;
	mEnt->startTime = level.time;
	mEnt->type = ME_LANDMINE;
}

void G_UpdateTeamMapData_CommandmapMarker( gentity_t* ent )
{
	if( !ent->parent ) {
		return;
	}

	if( ent->entstate != STATE_DEFAULT ) {
		return;
	}

	{
		int num = ent-g_entities;
		mapEntityData_Team_t* teamList;
		mapEntityData_t *mEnt;

		if( ent->parent->spawnflags & ALLIED_OBJECTIVE ) {
			teamList = &mapEntityData[0];
			mEnt = G_FindMapEntityData( teamList, num );
			if( !mEnt ) {
				mEnt = G_AllocMapEntityData( teamList );
				mEnt->entNum = num;
			}
			VectorCopy( ent->s.origin, mEnt->org );
			mEnt->data = ent->parent ? ent->parent->s.teamNum : -1;
			mEnt->startTime = level.time;
			mEnt->type = ME_COMMANDMAP_MARKER;
			mEnt->yaw = 0;
		}

		if( ent->parent->spawnflags & AXIS_OBJECTIVE ) {
			teamList = &mapEntityData[1];
			mEnt = G_FindMapEntityData( teamList, num );
			if( !mEnt ) {
				mEnt = G_AllocMapEntityData( teamList );
				mEnt->entNum = num;
			}
			VectorCopy( ent->s.origin, mEnt->org );
			mEnt->data = ent->parent ? ent->parent->s.teamNum : -1;
			mEnt->startTime = level.time;
			mEnt->type = ME_COMMANDMAP_MARKER;
			mEnt->yaw = 0;
		}
	}
}


// core: this buffer holds the last sent entnfo2 data.
// it is used to check if the last data is different from the current data.
// both G_SendSpectatorMapEntityInfo() & G_SendMapEntityInfo() share it.
static char lastBuffer2[2048];

void G_SendSpectatorMapEntityInfo( gentity_t* e ) {
	// special version, sends different set of ents - only the objectives, but also team info (string is split in two basically)
	mapEntityData_t*		mEnt;
	mapEntityData_Team_t*	teamList = &mapEntityData[0]; // Axis data init
	char					buffer[2048];
	static char				lastBuffer[2048];
	int						al_cnt = 0, ax_cnt = 0, len;

	//--- entnfo

	for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if ( mEnt->type != ME_TANK ) continue;
		if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
		ax_cnt++;
	}

	// Allied data init
	teamList = &mapEntityData[1];
	for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if ( mEnt->type != ME_TANK ) continue;
		if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
		al_cnt++;
	}

	// Data setup
	Com_sprintf( buffer, sizeof( buffer ), "entnfo %i %i", ax_cnt, al_cnt );

	// Axis data
	if ( ax_cnt > 0 ) {
		teamList = &mapEntityData[0];
		for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
			if ( mEnt->type != ME_TANK ) continue;
			if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
			G_PushMapEntityToBuffer( buffer, sizeof( buffer ), mEnt );
		}
	}

	// Allied data
	if ( al_cnt > 0 ) {
		teamList = &mapEntityData[1];
		for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
			if ( mEnt->type != ME_TANK ) continue;
			if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
			G_PushMapEntityToBuffer( buffer, sizeof( buffer ), mEnt );
		}
	}

	// core: if nothing has changed, don't send the packet..
	len = strlen(buffer);
	if ( memcmp(buffer, lastBuffer, len) ) {
		memcpy(lastBuffer, buffer, len);
		trap_SendServerCommand( e-g_entities, buffer );
	}

	//--- entnfo2

	al_cnt = ax_cnt = 0;

	// Axis data init
	teamList = &mapEntityData[0];
	for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if ( mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE || mEnt->type == ME_PLAYER_DISGUISED || mEnt->type == ME_TANK || mEnt->type == ME_LANDMINE ) continue;
		if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
		ax_cnt++;
	}

	// Allied data init
	teamList = &mapEntityData[1];
	for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
		if ( mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE || mEnt->type == ME_PLAYER_DISGUISED || mEnt->type == ME_TANK || mEnt->type == ME_LANDMINE ) continue;
		if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
		al_cnt++;
	}

	// Data setup
	Com_sprintf( buffer, sizeof( buffer ), "entnfo2 %i %i", ax_cnt, al_cnt );

	// Axis data
	if ( ax_cnt > 0 ) {
		teamList = &mapEntityData[0];
		for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
			if ( mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE || mEnt->type == ME_PLAYER_DISGUISED || mEnt->type == ME_TANK || mEnt->type == ME_LANDMINE ) continue;
			if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
			G_PushMapEntityToBuffer2( buffer, sizeof( buffer ), mEnt );
		}
	}

	// Allied data
	if ( al_cnt > 0 ) {
		teamList = &mapEntityData[1];
		for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
			if ( mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE || mEnt->type == ME_PLAYER_DISGUISED || mEnt->type == ME_TANK || mEnt->type == ME_LANDMINE ) continue;
			if ( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
			G_PushMapEntityToBuffer2( buffer, sizeof( buffer ), mEnt );
		}
	}

	// core: if nothing has changed, don't send the packet..
	len = strlen(buffer);
	if ( memcmp(buffer, lastBuffer2, len) ) {
		memcpy(lastBuffer2, buffer, len);
		trap_SendServerCommand( e-g_entities, buffer );
	}
}

void G_SendMapEntityInfo( gentity_t* e ) {
	if( e->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		G_SendSpectatorMapEntityInfo( e );
		return;
	}

	// something really went wrong if this evaluates to true
	if(e->client->sess.sessionTeam != TEAM_AXIS && e->client->sess.sessionTeam != TEAM_ALLIES) {
		return;
	}

	{
		char buffer[2048];
		int cnt = 0;
		mapEntityData_Team_t *teamList = e->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[0] : &mapEntityData[1];
		mapEntityData_t *mEnt = teamList->activeMapEntityData.next;

		//--- entnfo

		while( mEnt && mEnt != &teamList->activeMapEntityData ) {
			if(level.time - mEnt->startTime > SECONDS_5) {
				mEnt->status = 1;
				// we can free this player from the list now
				if( mEnt->type == ME_PLAYER ) {
					mEnt = G_FreeMapEntityData( teamList, mEnt );
					continue;
				}
				else if( mEnt->type == ME_PLAYER_DISGUISED ) {
					if( mEnt->singleClient == e->s.clientNum ) {
						mEnt = G_FreeMapEntityData( teamList, mEnt );
						continue;
					}
				}
			}
			else {
				mEnt->status = 2;
			}

			if ( mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE || mEnt->type == ME_PLAYER_DISGUISED || mEnt->type == ME_TANK ) {
				cnt++;
			}

			mEnt = mEnt->next;
		}

		if( e->client->sess.sessionTeam == TEAM_AXIS ) {
			Com_sprintf(buffer, sizeof( buffer ), "entnfo %i 0", cnt);
		}
		else {
			Com_sprintf(buffer, sizeof( buffer ), "entnfo 0 %i", cnt);
		}

		if ( cnt > 0 ) {
			for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
				if( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
				G_PushMapEntityToBuffer( buffer, sizeof( buffer ), mEnt );
			}
		}

		trap_SendServerCommand( e-g_entities, buffer );

		//--- entnfo2

		cnt = 0;
		mEnt = teamList->activeMapEntityData.next;
		while( mEnt && mEnt != &teamList->activeMapEntityData ) {
			if ( mEnt->type != ME_PLAYER && mEnt->type != ME_PLAYER_REVIVE && mEnt->type != ME_PLAYER_DISGUISED && mEnt->type != ME_TANK ) {
				cnt++;
			}
			mEnt = mEnt->next;
		}

		if( e->client->sess.sessionTeam == TEAM_AXIS ) {
			Com_sprintf(buffer, sizeof( buffer ), "entnfo2 %i 0", cnt);
		}
		else {
			Com_sprintf(buffer, sizeof( buffer ), "entnfo2 0 %i", cnt);
		}

		if ( cnt > 0 ) {
			for( mEnt = teamList->activeMapEntityData.next; mEnt && mEnt != &teamList->activeMapEntityData; mEnt = mEnt->next ) {
				if( mEnt->singleClient >= 0 && e->s.clientNum != mEnt->singleClient ) continue;
				G_PushMapEntityToBuffer2( buffer, sizeof( buffer ), mEnt );
			}
		}

		{
			int len = strlen(buffer);
			// only send it when the data has changed..
			if ( Q_stricmp(lastBuffer2, buffer) ) {
				memcpy(lastBuffer2, buffer, len+1);
				trap_SendServerCommand( e-g_entities, buffer );
			}
		}

	}
}

void C_MineSpotting( void )
{
	if (level.time < level.lastMapEntityUpdate_mines ) {
		return;
	}
	level.lastMapEntityUpdate_mines = level.time + SECOND_HALF;

	{
		int i, j;
		gentity_t *ent, *ent2;
		// core: we want to search through clients here, not all entities..
		for ( i = 0; i < level.numConnectedClients; i++ ) {
			ent = &g_entities[ level.sortedClients[i] ];

			if( !ent->inuse || !ent->client ) {
				continue;
			}

			if ( ent->health <= 0 ) {
				continue;
			}

			// must be in a valid team
			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
				continue;
			}
			if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
				continue;
			}

			// spotted landmines on map
			// forty - FORUM - specs/team specs get xp for spot. core: this issue is already solved by now..
			if ( ent->client->sess.playerType == PC_COVERTOPS ||
				((jp_keepAwards.integer & JP_KEEP_MINES) &&  ent->client->sess.skillBits[SK_BATTLE_SENSE]& (1<<4)) )
			{
				if (ent->client->ps.eFlags & EF_ZOOMING) {
					G_SetupFrustum_ForBinoculars( ent );

					for ( j=0; j<g_landmines.count; j++ ) {
						ent2 = g_landmines.ent[j];

						if(!ent2->inuse || ent2 == ent) {
							continue;
						}

						if ( ent2->s.eType != ET_MISSILE ) {
							continue;
						}

						if( ent2->methodOfDeath != MOD_LANDMINE) {
							continue;
						}

						if( (ent2->s.teamNum < 4 || ent2->s.teamNum >= 8) && (ent2->s.teamNum%4 != ent->client->sess.sessionTeam) ) {
							// TAT - as before, we can only detect a mine if we can see it from our binoculars
							if (G_VisibleFromBinoculars( ent, ent2, ent2->r.currentOrigin )) {
								G_UpdateTeamMapData_LandMine(ent2);

								switch(ent2->s.teamNum%4) {
								case TEAM_AXIS:
								case TEAM_ALLIES:
									if( !ent2->s.modelindex2 ) {
										ent->client->landmineSpottedTime = level.time;
										ent->client->landmineSpotted = ent2;
										ent2->s.density = ent-g_entities+1;
										ent2->missionLevel = level.time;

										ent->client->landmineSpotted->count2 += 50;
										if(ent->client->landmineSpotted->count2 >= 250) {
											ent->client->landmineSpotted->count2 = 250;
											ent->client->landmineSpotted->s.modelindex2 = 1;

											// for marker
											// CHRUKER: b036 - Landmine flags shouldn't block our view
											// forty - #423 don't do this if the mine has been triggered.
											if(!G_LandmineTriggered(ent->client->landmineSpotted)) {
													ent->client->landmineSpotted->s.frame = rand() % 20;
													ent->client->landmineSpotted->r.contents = CONTENTS_TRANSLUCENT;
													trap_LinkEntity( ent->client->landmineSpotted );
											}

											// core: display a message on who did it..
											G_AnnounceMessageForMines( ent );

											trap_SendServerCommand( ent-g_entities, "cp \"Landmine Revealed\n\"" );

											AddScore( ent, 1 );

											G_AddSkillPoints( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 3.f );
											G_DEBUG_ADD_SKILL_POINTS( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 3.f, "spotting a landmine" );
										}
									}
									break;
								} // end switch
							}
							else {
								// TAT - if we can't see the mine from our binoculars, make sure we clear out the landmineSpotted ptr,
								//		because bots looking for mines are getting confused
								ent->client->landmineSpotted = NULL;
							}
						}
					}
				}
			}
		}
	}
}

void G_UpdateTeamMapData( void )
{
	// landmine spotting needs to be done at shorter intervals than the commandmap-icons-update..
	// otherwise spotting mines is too slow (they need to look at the mines for too long time).
	C_MineSpotting();

	// commandmap/radar icons update..
	if (level.time < level.lastMapEntityUpdate) {
		return;
	}
	level.lastMapEntityUpdate = level.time + 1500;

	{
		int i, j;
		gentity_t *ent, *ent2;
		mapEntityData_t	*mEnt;

		// core: check all relevant entities.
		// They are all stored in seperate arrays, so no need to loop through all entities..

		// ET_PLAYER
		for ( i=0; i<level.numConnectedClients; i++ ) {
			ent = &g_entities[ level.sortedClients[i] ];
			if ( !ent->inuse ) continue;
			if ( ent->s.eType != ET_PLAYER ) continue;

			// update enemy
			mEnt = G_FindMapEntityData( (ent->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0]), ent->client->ps.clientNum );
			if ( mEnt ) {
				G_UpdateTeamMapData_Player( ent, ent->client->sess.sessionTeam == TEAM_AXIS, ent->client->sess.sessionTeam == TEAM_ALLIES );	//opposite teams..
			}

			G_UpdateTeamMapData_Player( ent, qfalse, qfalse );
			for( j = 0; j < 2; j++ ) {
				mapEntityData_Team_t *teamList = &mapEntityData[j];
				mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, ent->s.number, -1 );
				while( mEnt ) {
					VectorCopy( ent->client->ps.origin, mEnt->org );
					mEnt->yaw = ent->client->ps.viewangles[YAW];
					mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, ent->s.number, -1 );
				}
			}
		}

		// ET_MISSILE MOD_LANDMINE
		for ( i=0; i<g_landmines.count; i++ ) {
			ent = g_landmines.ent[i];
			if ( !ent->inuse ) continue;
			if ( ent->s.eType != ET_MISSILE ) continue;
			if ( ent->methodOfDeath != MOD_LANDMINE) continue;
			G_UpdateTeamMapData_LandMine(ent);
		}

		// ET_CONSTRUCTIBLE_INDICATOR
		// ET_EXPLOSIVE_INDICATOR
		// ET_TANK_INDICATOR
		// ET_TANK_INDICATOR_DEAD
		// ET_COMMANDMAP_MARKER
		for ( i=0; i<g_indicators.count; i++ ) {
			ent = g_indicators.ent[i];
			if ( !ent->inuse ) continue;
			switch (ent->s.eType) {
				case ET_CONSTRUCTIBLE_INDICATOR:
					if( ent->parent && ent->parent->entstate == STATE_DEFAULT ) {
						G_UpdateTeamMapData_Construct(ent);
					}
					break;
				case ET_EXPLOSIVE_INDICATOR:
					if( ent->parent && ent->parent->entstate == STATE_DEFAULT ) {
						G_UpdateTeamMapData_Destruct(ent);
					}
					break;
				case ET_TANK_INDICATOR:
				case ET_TANK_INDICATOR_DEAD:
					G_UpdateTeamMapData_Tank(ent);
					break;
				case ET_COMMANDMAP_MARKER:
					G_UpdateTeamMapData_CommandmapMarker( ent );
					break;
				default:
					break;
			}
		}


		// core: we want to search through clients here, not all entities..
		for ( i = 0; i < level.numConnectedClients; i++ ) {
			qboolean	f1 = qfalse, f2 = qfalse;	// Jaybird - clean up warnings
			vec3_t		pos[3];

			ent = &g_entities[ level.sortedClients[i] ];

			if( !ent->inuse || !ent->client ) {
				continue;
			}

			if ( ent->health <= 0 ) {
				continue;
			}

			// must be in a valid team
			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
				continue;
			}
			if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
				continue;
			}

			f1 = ent->client->sess.sessionTeam == TEAM_ALLIES ? qtrue : qfalse;
			f2 = ent->client->sess.sessionTeam == TEAM_AXIS ?	qtrue : qfalse;

			// disguised players on map
			if( ent->client->sess.playerType == PC_FIELDOPS &&
				(ent->client->ps.eFlags & EF_ZOOMING) &&					// need to use binocs for detecting a disguised covertops
				(ent->client->sess.skillBits[SK_SIGNALS] & (1<<4)) ) {

				G_SetupFrustum_ForBinoculars( ent );	// G_SetupFrustum( ent );

				for ( j=0; j<level.numConnectedClients; j++ ) {
					ent2 = &g_entities[ level.sortedClients[j] ];

					if( !ent2->inuse || ent2 == ent ) {
						continue;
					}

					// players are sometimes of type ET_GENERAL.. (thanks crapshoot for mentioning)
					if ( ent2->s.eType != ET_PLAYER ) {
						continue;
					}

					if( ent2->client->sess.sessionTeam == TEAM_SPECTATOR || ent2->client->sess.sessionTeam == TEAM_FREE ) {
						continue;
					}

					if( ent2->health <= 0 ||
						ent2->client->sess.sessionTeam == ent->client->sess.sessionTeam ||
						!ent2->client->ps.powerups[PW_OPS_DISGUISED] )
					{
						continue;
					}

					if ( ent2->client->sess.skillBits[SK_COVERTOPS] & (1<<7) ) {
						continue;
					}

					// core: performing a boundingbox-trace, instead of 3 seperate point-traces..
					VectorCopy( ent2->client->ps.origin, pos[0] );
					VectorCopy( ent2->client->ps.mins, pos[1] );
					VectorCopy( ent2->client->ps.maxs, pos[2] );
					// core: G_VisibleFromBinoculars is used, but there is no check if the fieldops is currently using his binocs.. correct?
					if ( G_VisibleFromBinoculars_Box(ent, ent2, pos[0], pos[1], pos[2]) ) {
						G_UpdateTeamMapData_DisguisedPlayer( ent, ent2, f1, f2 );
					}

				}
			}
			// core: it appears the covertops can detect normal enemy players only??!
			// but a fieldops can detect disguised coverts.. hmm is this correct?
			// To me it doesn't make much sense to display normal enemies on the commandmap.
			// It makes it impossible to differentiate from enemy disguised coverts.
			// It just clutters the commandmap (in my opinion)
			else if( ent->client->sess.playerType == PC_COVERTOPS ) {
				G_SetupFrustum( ent );

				for ( j = 0; j < level.numConnectedClients; j++ ) {
					ent2 = &g_entities[ level.sortedClients[j] ];

					if( !ent2->inuse || ent2 == ent ) {
						continue;
					}

					if ( ent2->s.eType != ET_PLAYER ) {
						continue;
					}

					// we only see non-teammates, and we never see spectators..
					if ( ent2->client->sess.sessionTeam == ent->client->sess.sessionTeam ||
						ent2->client->sess.sessionTeam == TEAM_SPECTATOR || ent2->client->sess.sessionTeam == TEAM_FREE )
					{
						continue;
					}

					if ( ent2->health <= 0 ) {
						continue;
					}

					// core: performing a boundingbox-trace, instead of 3 seperate point-traces..
					VectorCopy( ent2->client->ps.origin, pos[0] );
					VectorCopy( ent2->client->ps.mins, pos[1] );
					VectorCopy( ent2->client->ps.maxs, pos[2] );
//					if ( G_VisibleFromBinoculars_Box(ent, ent2, pos[0], pos[1], pos[2]) ) {
						G_UpdateTeamMapData_Player(ent2, f1, f2);
//					}

				}
			}
		}
	}
}
