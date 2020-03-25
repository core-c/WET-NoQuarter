/*
 * name:		g_weapon.c
 *
 * desc:		perform the server side effects of a weapon firing
 *
*/


#include "g_local.h"
#include "etpro_mdx.h"

vec3_t	forward, right, up;
vec3_t	muzzleEffect;
vec3_t	muzzleTrace;

// forward dec
void Bullet_Fire (gentity_t *ent, float spread, int damage, qboolean distance_falloff);
qboolean Bullet_Fire_Extended(gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage, qboolean distance_falloff, int reflectNum);

int G_GetWeaponDamage( int weapon ); // JPW

void InitCameras (void) {
	int		i;

	for (i=0; i<level.maxclients ; i++) {
		gentity_t *ent = G_Spawn();
		ent->classname = "missilecam";
		ent->neverFree = qtrue;
		ent->s.eType = ET_MISSILECAM;
		ent->r.ownerNum = i;
		ent->r.svFlags = SVF_PORTAL | SVF_SINGLECLIENT;
		ent->r.singleClient = i;
		trap_LinkEntity( ent );
	}
}

qboolean G_WeaponIsExplosive( meansOfDeath_t mod )
{
	switch( mod )
	{
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
		case MOD_PANZERFAUST:
		case MOD_LANDMINE:
		case MOD_GPG40:
		case MOD_M7:
		case MOD_ARTY:
		case MOD_AIRSTRIKE:
		case MOD_MORTAR:
		case MOD_SATCHEL:
		case MOD_DYNAMITE:
		// map entity based explosions
		case MOD_GRENADE:
		case MOD_MAPMORTAR:
		case MOD_MAPMORTAR_SPLASH:
		case MOD_EXPLOSIVE:
		case MOD_TELEFRAG: // Gordon: yes this _SHOULD_ be here, kthxbye
		case MOD_CRUSH:
		case MOD_BAZOOKA:
			return qtrue;
		default:
			return qfalse;
	}
}

int G_GetWeaponClassForMOD( meansOfDeath_t mod )
{
	switch( mod ) 
	{
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
		case MOD_PANZERFAUST:
		case MOD_LANDMINE:
		case MOD_GPG40:
		case MOD_M7:
		case MOD_ARTY:
		case MOD_AIRSTRIKE:
		case MOD_MORTAR:
		case MOD_BAZOOKA:
		// map entity based explosions
		case MOD_GRENADE:
		case MOD_MAPMORTAR:
		case MOD_MAPMORTAR_SPLASH:
		case MOD_EXPLOSIVE:
			return 0;
		case MOD_SATCHEL:
			return 1;
		case MOD_DYNAMITE:
			return 2;
		default:
			return -1;
	}
}

#define NUM_NAILSHOTS 10


/*
==============
AddLean
	add leaning offset
==============
*/
void AddLean(gentity_t *ent, vec3_t point)
{
	if(ent->client)
	{
		if(ent->client->ps.leanf)
		{
			vec3_t	up, right;
			AngleVectors(ent->client->ps.viewangles, up, right, NULL);
			VectorMA(point, ent->client->ps.leanf, right, point);
			//point[2] -= fabs(ent->client->ps.leanf/2); // jaquboss to match client's view
			point[2] -= fabs(ent->client->ps.leanf/3.5);
		}
	}
}

/*
================
CG_ReflectVelocity
================
*/
void jP_ReflectVector( vec3_t v, vec3_t origin, trace_t *trace )
{
/*	vec3_t	velocity;
	float	dot;
	int		hitTime;
	trace_t	*tr;
*/
	//trap_Trace( &tr, origin, NULL, NULL, end, NULL, SONTENTS_SOLID);
/*


	// reflect the velocity on the trace plane
//	hitTime = cg.time - cg.frametime + cg.frametime * trace->fraction;

//	BG_EvaluateTrajectoryDelta( &le->pos, hitTime, velocity, qfalse, -1 );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2*dot, trace->plane.normal, le->pos.trDelta );

	VectorScale( le->pos.trDelta, le->bounceFactor, le->pos.trDelta );

	VectorCopy( trace->endpos, le->pos.trBase );
	le->pos.trTime = cg.time;


	// check for stop, making sure that even on low FPS systems it doesn't bobble

	if (le->leMarkType == LEMT_BLOOD && trace->startsolid) {
			//centity_t *cent;
			//cent = &cg_entities[trace->entityNum];
			//if (cent && cent->currentState.apos.trType != TR_STATIONARY)
			//	le->pos.trType = TR_STATIONARY;
	} else if ( trace->allsolid || ( trace->plane.normal[2] > 0 && ( le->pos.trDelta[2] < 40 || le->pos.trDelta[2] < -cg.frametime * le->pos.trDelta[2] ) ) ) {
//----(SA)	if it's a fragment and it's not resting on the world...
//			if(le->leType == LE_DEBRIS && trace->entityNum < (MAX_ENTITIES - 1))
			if(le->leType == LE_FRAGMENT && trace->entityNum < (MAX_ENTITIES - 1)) {
				le->pos.trType = TR_GRAVITY_PAUSED;
			} else {
				le->pos.trType = TR_STATIONARY;
			}
	}
	*/
}

qboolean jP_UnDoChargeCost(gentity_t *ent, int skill, float charge )
{
	int chargeTime	= 0;

	// early out for non-players
	if (!ent || !ent->client )
	{
		return qfalse;
	}

	if (jp_insanity.integer & JP_INSANITY_CHARGEBAR)
	{
		return qtrue;
	}

	switch (skill)
	{
		case SK_SOLDIER:
			chargeTime = level.soldierChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_ENGINEER:
			chargeTime = level.engineerChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_MEDIC:
			chargeTime = level.medicChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_FIELDOPS:
			chargeTime = level.lieutenantChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_COVERTOPS:
			chargeTime = level.covertopsChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_LIGHT_WEAPONS:
		case SK_BATTLE_SENSE:
		default:
			chargeTime = level.soldierChargeTime[ent->client->sess.sessionTeam-1];
			break;
	}	

	ent->client->ps.classWeaponTime -= (charge * chargeTime);

	return qfalse;
}

qboolean jP_DoChargeCost(gentity_t *ent, int skill, float charge )
{
	int chargeTime	= 0;

	// early out for non-players
	if (!ent || !ent->client )
	{
		return qfalse;
	}

	// don't use any charge if Insanity mode is specified
	if (jp_insanity.integer & JP_INSANITY_CHARGEBAR)
	{
		return qtrue;
	}


	switch (skill)
	{
		case SK_SOLDIER:
			chargeTime = level.soldierChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_ENGINEER:
			chargeTime = level.engineerChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_MEDIC:
			chargeTime = level.medicChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_FIELDOPS:
			chargeTime = level.lieutenantChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_COVERTOPS:
			chargeTime = level.covertopsChargeTime[ent->client->sess.sessionTeam-1];
			break;
		case SK_LIGHT_WEAPONS:
		case SK_BATTLE_SENSE:
		default:
			chargeTime = level.soldierChargeTime[ent->client->sess.sessionTeam-1];
			break;
	}	

	// make sure the charge bar isn't charged beyond full
	if( level.time - ent->client->ps.classWeaponTime > chargeTime )
	{
		ent->client->ps.classWeaponTime = level.time - chargeTime;
	}

	if ((level.time - ent->client->ps.classWeaponTime) >= (charge * chargeTime))
	{
		ent->client->ps.classWeaponTime += (charge * chargeTime);
		return qtrue;
	}
	
	return qfalse;
}


// Let's use the same angle between function we've used before
extern float sAngleBetweenVectors(vec3_t a, vec3_t b);

/*
==============
Weapon_Knife
==============
*/
void Weapon_Knife( gentity_t *ent )
{
	trace_t		tr;
	gentity_t	*traceEnt;
	gentity_t	*tent;
	int			damage;
	int			mod;
	vec3_t		pforward;
	vec3_t		eforward;
	vec3_t		end;

	mod = MOD_KNIFE;

	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePoint ( ent, ent->s.weapon, forward, right, up, muzzleTrace );
	VectorMA (muzzleTrace, CH_KNIFE_DIST, forward, end);

	// rain - antilag knife
	//trap_Trace (&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);
	G_HistoricalTrace(ent, &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);

	// ignore hits on NOIMPACT surfaces
	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	// no contact
	if(tr.fraction == 1.0f)
	{
		return;
	}

	if(tr.entityNum >= MAX_CLIENTS)		
	{	
		// world brush or non-player entity (no blood)
		tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
	} 
	else								
	{	
		// other player						
		tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
	}

	tent->s.otherEntityNum	= tr.entityNum;
	tent->s.eventParm		= DirToByte( tr.plane.normal );
	tent->s.weapon			= ent->s.weapon;
	tent->s.clientNum		= ent->r.ownerNum;

	// don't worry about doing any damage to the world
	if(tr.entityNum == ENTITYNUM_WORLD)	
	{
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	// ignore hits on entities that dont take damage
	if(!(traceEnt->takedamage))
	{
		return;
	}

	damage = G_GetWeaponDamage(ent->s.weapon);		// JPW		// default knife damage for frontal attacks
	
	// Covert ops deal double damage with a knife
	if( ent->client->sess.playerType == PC_COVERTOPS )
	{
		//damage += damage;	
		damage <<= 1;
	}

	// jet Pilot - REMOVED - not allowing insta-gib backstabs on wounded players
	if(traceEnt->client) // && traceEnt->health > 0) // CHRUKER: b002 "fix", removed by jP
	{
		AngleVectors (ent->client->ps.viewangles,		pforward, NULL, NULL);
		AngleVectors (traceEnt->client->ps.viewangles,	eforward, NULL, NULL);

		if (G_GetEnemyPosition(ent, traceEnt) == POSITION_BEHIND)
//		if( DotProduct( eforward, pforward ) > 0.6f )		// from behind(-ish)
		{
			damage	= 100;			// enough to drop a 'normal' (100 health) human with one jab
			mod		= MOD_BACKSTAB; // Rear-attacks with a knife count as a backstab

			// rain - only do this if they have a positive health
			if ( traceEnt->health > 0 && ent->client->sess.skillBits[SK_COVERTOPS] & (1<<4) 
				&& (ent->client->sess.playerType == PC_COVERTOPS || (jp_keepAwards.integer & JP_KEEP_ASSASSIN))) {
				damage = traceEnt->health; 
			}
			else if (traceEnt->health <= 0) // jet Pilot - bugfix - always allow 'instagib' on wounded players
			{
				damage = -GIB_HEALTH + traceEnt->health ;
			}
		}
	}

	// jet pilot - remove the random damage addition, replace with light weapons level + covert ops level
	//damage += ent->client->sess.skill[SK_LIGHT_WEAPONS];
	damage += ent->client->sess.skill[SK_LIGHT_WEAPONS];
	damage += ent->client->sess.skill[SK_COVERTOPS];
	G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, mod);	
	//G_Damage( traceEnt, ent, ent, vec3_origin, tr.endpos, (damage + rand()%5), 0, mod);
}

void MagicSink( gentity_t *self )
{
    self->clipmask = 0;
    self->r.contents = 0;

    self->nextthink = level.time + 4000;
    self->think = G_FreeEntity;

    self->s.pos.trType = TR_LINEAR;
    self->s.pos.trTime = level.time;
    VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    VectorSet( self->s.pos.trDelta, 0, 0, -5 );
}

/*
=================
fire_missile
=================
*/
extern void Homing_Missile_Think( gentity_t *ent );
extern void Guided_Missile_Think (gentity_t *missile);

gentity_t *fire_missile (gentity_t *self, vec3_t start, vec3_t dir, int damage, int radius, int speed, int mod, int size) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();

	bolt->classname = "missile";
	// jaquboss added support for this..
	if ((jp_insanity.integer & JP_INSANITY_GUIDED) && self->client)
	{
		bolt->nextthink			= level.time + FRAMETIME;
		bolt->think				= Guided_Missile_Think;		
	}
	else if (jp_insanity.integer & JP_INSANITY_HOMING)
	{
		bolt->nextthink			= level.time + FRAMETIME;
		bolt->think				= Homing_Missile_Think;		
	}
	else
	{	
		bolt->nextthink 		= level.time + 20000;					// push it out a little
		bolt->think				= G_ExplodeMissile;
	}

	bolt->accuracy = 4;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_BROADCAST;	
	
	//DHM - Nerve :: Use the correct weapon in multiplayer
	bolt->s.weapon = self->s.weapon;

	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = damage; // JPW NERVE
	bolt->splashDamage = damage; // JPW NERVE
	bolt->splashRadius = radius; 
	bolt->methodOfDeath = mod;
	bolt->splashMethodOfDeath = mod;
	bolt->clipmask = MASK_MISSILESHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - 50;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
// JPW NERVE
	VectorScale(dir,speed,bolt->s.pos.trDelta);
// jpw
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);
// jaquboss - hitbox
	VectorSet (bolt->r.mins, -size, -size, -size);
	VectorSet (bolt->r.maxs, size, size, size);

	if(self->client) {
		bolt->s.teamNum = self->client->sess.sessionTeam;
		bolt->s.clientNum = self->s.clientNum; // jaquboss, missile cam bug
	}
/*
	if(self->client && jp_insanity.integer & JP_INSANITY_MISSILECAM) 
	{
		gentity_t *camera;
	
		camera = G_Spawn();
		camera->s.eType = ET_MISSILECAM;
		camera->r.singleClient = self->client->ps.clientNum;
		camera->r.svFlags = SVF_PORTAL | SVF_SINGLECLIENT;
		VectorCopy( start, bolt->s.pos.trBase );
		G_SetOrigin (camera, start); 
		VectorClear(camera->r.mins);
		VectorClear(camera->r.maxs);
		trap_LinkEntity(camera);
		camera->r.ownerNum = self->client->ps.clientNum;
		camera->nextthink = 0;
		camera->s.pos.trType = TR_STATIONARY;
		vectoangles(dir, camera->s.angles);
		vectoangles(dir, camera->s.angles2);
		camera->s.clientNum = self->client->ps.clientNum;
	}	
*/
	return bolt;
}

/*
=============
Weapon_Firebolt

=============
*//*
void Weapon_Firebolt ( gentity_t *ent) {
	gentity_t	*m;

	AngleVectors (ent->client->ps.viewangles, forward, right, up);


	m = fire_missile (ent, muzzleEffect, forward, G_GetWeaponDamage(WP_FIREBOLT),
		BG_Weapons[WP_FIREBOLT].splashdamage_radius ?  BG_Weapons[WP_FIREBOLT].splashdamage_radius  : 75,
		BG_Weapons[WP_FIREBOLT].splashdamage ? BG_Weapons[WP_FIREBOLT].splashdamage : 750,
		MOD_FIREBOLT,
		5 );	//Meyer - increase firebolt speed

}*/

/*
======================
  Weapon_Class_Special
	class-specific in multiplayer
======================
*/
// JPW NERVE
void Weapon_Medic( gentity_t *ent ) {
	vec3_t	velocity, offset;
	vec3_t	angles;
	vec3_t	tosspos, viewpos;

	VectorCopy( ent->client->ps.viewangles, angles );

	// clamp pitch
	if ( angles[PITCH] < -30 ) {
		angles[PITCH] = -30;
	} else if ( angles[PITCH] > 30 ) {
		angles[PITCH] = 30;
	}

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset);
	offset[2] += ent->client->ps.viewheight/2;

	VectorScale( velocity, g_throwDistance.integer, velocity);
	velocity[2] += 50 + crandom() * 25;

	VectorCopy( muzzleEffect, tosspos );
	VectorMA( tosspos, 48, forward, tosspos );
	VectorCopy( ent->client->ps.origin, viewpos );
	Weapon_Medic_Ext(ent, viewpos, tosspos, velocity);
}

void Weapon_Medic_Ext(gentity_t *ent, vec3_t viewpos, vec3_t tosspos, vec3_t velocity)
{
	gitem_t		*item;
	gentity_t	*ent2;
	vec3_t		mins,maxs;
	trace_t		tr;

	jP_DoChargeCost(ent, SK_MEDIC, BG_GetFromTable(chargeCost_Health, ent->client->sess.skillBits, SK_MEDIC ) );

	item = BG_FindItemForClassName("item_health");

	VectorSet( mins, -(ITEM_RADIUS + 8), -(ITEM_RADIUS+8), 0 );
	VectorSet( maxs, (ITEM_RADIUS + 8), (ITEM_RADIUS+8), 2*(ITEM_RADIUS+8) );

	trap_EngineerTrace( &tr, viewpos, mins, maxs, tosspos, ent->s.number, MASK_MISSILESHOT );
	if( tr.startsolid ) {
		// Arnout: this code is a bit more solid than the previous code
		VectorCopy( forward, viewpos );
		VectorNormalizeFast( viewpos );
		VectorMA( ent->r.currentOrigin, -24.f, viewpos, viewpos ); 

		trap_EngineerTrace(&tr, viewpos, mins, maxs, tosspos, ent->s.number, MASK_MISSILESHOT);

		VectorCopy( tr.endpos, tosspos );
	} else if( tr.fraction < 1 ) {	// oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

    ent2 = LaunchItem( item, tosspos, velocity, ent->s.number, FL_DROPPED_ITEM );
    ent2->think = MagicSink;
    ent2->nextthink = level.time + 30000;

	ent2->s.teamNum = ent->client->sess.sessionTeam; // jaquboss

//	ent2->timestamp = level.time + 31200;

	ent2->parent = ent; // JPW NERVE so we can score properly later
	//ent2->count = 20;
}

/*
==========
G_PlaceTripmine
==========
*/
void G_PlaceTripmine(gentity_t* ent)
{
	vec3_t start, end;
	trace_t trace;
	gentity_t* bomb;
	vec3_t forward;

	VectorCopy( ent->client->ps.origin, start );
	start[2] += ent->client->ps.viewheight;

	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);

	VectorMA(start, 64, forward, end);

	trap_Trace(&trace, start, NULL, NULL, end, ent->s.number, MASK_SHOT);

	bomb = G_Spawn();
	bomb->r.svFlags	= SVF_BROADCAST;
	bomb->s.eType = ET_BOMB;
	bomb->s.eFlags = 0;
	bomb->s.weapon = WP_TRIPMINE;
	bomb->parent = ent;
	bomb->think = G_TripMinePrime;
	bomb->nextthink = level.time + 2000;
	bomb->splashDamage = 300;
	bomb->splashRadius = 300;
	bomb->methodOfDeath = MOD_TRIPMINE;
	bomb->splashMethodOfDeath = MOD_TRIPMINE;
	bomb->r.contents = CONTENTS_CORPSE;	// (player can walk through)

	VectorSet(bomb->r.mins, -12, -12, 0);
	VectorCopy(bomb->r.mins, bomb->r.absmin);
	VectorSet(bomb->r.maxs, 12, 12, 20);
	VectorCopy(bomb->r.maxs, bomb->r.absmax);

	VectorMA(trace.endpos, 1, trace.plane.normal, start);
	G_SetOrigin(bomb, start);
	G_SetAngle(bomb, vec3_origin);

	VectorCopy(trace.plane.normal, bomb->s.origin2);

	trap_LinkEntity(bomb);
}

/*void Weapon_SatchelCharge(gentity_t *ent) {
	gitem_t *item;
	gentity_t *ent2;
	vec3_t	velocity, org, offset;
	vec3_t	angles,mins,maxs;
	trace_t	tr;

	item = BG_FindItem("Satchel Charge");
	VectorCopy( ent->client->ps.viewangles, angles );

	// clamp pitch
	if ( angles[PITCH] < -30 )
		angles[PITCH] = -30;
	else if ( angles[PITCH] > 30 )
		angles[PITCH] = 30;

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset);
	offset[2] += ent->client->ps.viewheight/2;
	VectorScale( velocity, 75, velocity );
	velocity[2] += 50 + crandom() * 25;

	VectorAdd(ent->client->ps.origin,offset,org);

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2*ITEM_RADIUS );

	trap_Trace (&tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID);
	VectorCopy( tr.endpos, org );

	ent2 = LaunchItem( item, org, velocity, ent->s.number );
	ent2->think = MagicSink;
	ent2->timestamp = level.time + 4000;
	ent2->parent = ent;
	ent2->s.eType = ET_MISSILE;
	ent2->methodOfDeath = MOD_SATCHEL;
	ent2->splashMethodOfDeath = MOD_SATCHEL;
	ent2->s.weapon = WP_SATCHEL;
	ent2->touch = 0;
}
*/

// JPW NERVE
/*
==================
Weapon_MagicAmmo
==================
*/
void Weapon_MagicAmmo( gentity_t *ent )  {
	vec3_t	velocity, offset;
	vec3_t	tosspos, viewpos;
	vec3_t	angles;

	VectorCopy( ent->client->ps.viewangles, angles );

	// clamp pitch
	if ( angles[PITCH] < -30 )
		angles[PITCH] = -30;
	else if ( angles[PITCH] > 30 )
		angles[PITCH] = 30;

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset);
	offset[2] += ent->client->ps.viewheight/2;

	VectorScale( velocity, g_throwDistance.integer, velocity);
	velocity[2] += 50 + crandom() * 25;

	VectorCopy( muzzleEffect, tosspos );
	VectorMA( tosspos, 48, forward, tosspos );
	VectorCopy( ent->client->ps.origin, viewpos );
	Weapon_MagicAmmo_Ext(ent, viewpos, tosspos, velocity);
}

void Weapon_MagicAmmo_Ext(gentity_t *ent, vec3_t viewpos, vec3_t tosspos, vec3_t velocity)
{
	vec3_t mins, maxs;
	trace_t	tr;
	gitem_t *item;
	gentity_t *ent2;

	jP_DoChargeCost(ent, SK_SIGNALS, BG_GetFromTable(chargeCost_Ammo, ent->client->sess.skillBits, SK_SIGNALS ) );

	if ( ent->client->sess.skillBits[SK_SIGNALS] & (1<<8) )
		item = BG_FindItem("Huge Ammo Pack");
	else 
		item = BG_FindItem( (ent->client->sess.skillBits[SK_SIGNALS] & (1<<1)) ? "Mega Ammo Pack" : "Ammo Pack" );
	
	VectorSet( mins, -(ITEM_RADIUS + 8), -(ITEM_RADIUS+8), 0 );
	VectorSet( maxs, (ITEM_RADIUS + 8), (ITEM_RADIUS+8), 2*(ITEM_RADIUS+8) );

	trap_EngineerTrace( &tr, viewpos, mins, maxs, tosspos, ent->s.number, MASK_MISSILESHOT );
	if( tr.startsolid ) 
	{
		// Arnout: this code is a bit more solid than the previous code
		VectorCopy( forward, viewpos );
		VectorNormalizeFast( viewpos );
		VectorMA( ent->r.currentOrigin, -24.f, viewpos, viewpos ); 

		trap_EngineerTrace (&tr, viewpos, mins, maxs, tosspos, ent->s.number, MASK_MISSILESHOT);

		VectorCopy( tr.endpos, tosspos );
	}
	else if( tr.fraction < 1 )	// oops, bad launch spot
	{	
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

    ent2 = LaunchItem( item, tosspos, velocity, ent->s.number, FL_DROPPED_ITEM );

	ent2->s.teamNum = ent->client->sess.sessionTeam;

    ent2->think = MagicSink;
    ent2->nextthink = level.time + 30000;

	ent2->parent = ent;

	if( ent->client->sess.skillBits[SK_SIGNALS] & (1<<8) ) {
		ent2->count = 3;
		ent2->s.density = 3;
	} else if( ent->client->sess.skillBits[SK_SIGNALS] & (1<<1) ) {
		ent2->count = 2;
		ent2->s.density = 2;
	} else {
		ent2->count = 1;
		ent2->s.density = 1;
	}
}
// jpw



// START - Mad Doc - TDF
// took this out of Weapon_Syringe so we can use it from other places
qboolean ReviveEntity(gentity_t *ent, gentity_t *traceEnt, qboolean resuscitation)
{
	vec3_t		org;
	trace_t		tr;
	int			healamt, headshot, pantsed, glowing, poisoned, poisonAmnt, oldweapon,oldweaponstate,oldclasstime=0;
	qboolean	usedSyringe = qfalse;		// DHM - Nerve
	int			ammo[MAX_WEAPONS];		// JPW NERVE total amount of ammo
	int			ammoclip[MAX_WEAPONS];	// JPW NERVE ammo in clip
	int			weapons[MAX_WEAPONS/(sizeof(int)*8)];	// JPW NERVE 64 bits for weapons held
	gentity_t	*te;

	// heal the dude
	// copy some stuff out that we'll wanna restore
	VectorCopy(traceEnt->client->ps.origin, org);

	headshot = traceEnt->client->ps.eFlags & EF_HEADSHOT;
	pantsed = traceEnt->client->ps.eFlags & EF_PANTSED;
	glowing	= traceEnt->client->ps.eFlags & EF_GLOW;
	poisoned = traceEnt->client->ps.eFlags & EF_POISONED;
	poisonAmnt = traceEnt->client->pmext.poisonAmnt;

	healamt = traceEnt->client->ps.stats[STAT_MAX_HEALTH];

	if ( ent )
		healamt *= (ent->client->sess.skillBits[SK_MEDIC] & (1<<3)) ? 1 : 0.5f;

	oldweapon = traceEnt->client->ps.weapon;
	oldweaponstate = traceEnt->client->ps.weaponstate;

	// keep class special weapon time to keep them from exploiting revives
	oldclasstime = traceEnt->client->ps.classWeaponTime;

	memcpy(ammo,traceEnt->client->ps.ammo,sizeof(int)*MAX_WEAPONS);
	memcpy(ammoclip,traceEnt->client->ps.ammoclip,sizeof(int)*MAX_WEAPONS);
	memcpy(weapons,traceEnt->client->ps.weapons,sizeof(int)*(MAX_WEAPONS/(sizeof(int)*8)));

	ClientSpawn(traceEnt, qtrue, qfalse, qtrue);

	// jaquboss
	traceEnt->reviveVal = 0;

	traceEnt->client->ps.stats[STAT_PLAYER_CLASS] = traceEnt->client->sess.playerType;
	memcpy(traceEnt->client->ps.ammo,ammo,sizeof(int)*MAX_WEAPONS);
	memcpy(traceEnt->client->ps.ammoclip,ammoclip,sizeof(int)*MAX_WEAPONS);
	memcpy(traceEnt->client->ps.weapons,weapons,sizeof(int)*(MAX_WEAPONS/(sizeof(int)*8)));

	if( headshot ) 
		traceEnt->client->ps.eFlags |= EF_HEADSHOT;

	if( pantsed ) 
		traceEnt->client->ps.eFlags |= EF_PANTSED;

	if( glowing ) 
		traceEnt->client->ps.eFlags |= EF_GLOW;

	if ( poisoned && poisonAmnt){
		traceEnt->client->ps.eFlags |= EF_POISONED;
		traceEnt->client->pmext.poisonAmnt = poisonAmnt;
		if ( resuscitation )
			healamt *= 0.33f;
	}

	traceEnt->client->ps.weapon = oldweapon;
	traceEnt->client->ps.weaponstate = oldweaponstate;

	// set idle animation on weapon
	traceEnt->client->ps.weapAnim = ( ( traceEnt->client->ps.weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | PM_IdleAnimForWeapon( traceEnt->client->ps.weapon );

	traceEnt->client->ps.classWeaponTime = oldclasstime;

	traceEnt->health = healamt;
	VectorCopy(org,traceEnt->s.origin);
	VectorCopy(org,traceEnt->r.currentOrigin);
	VectorCopy(org,traceEnt->client->ps.origin);

	trap_Trace(&tr, traceEnt->client->ps.origin, traceEnt->client->ps.mins, traceEnt->client->ps.maxs, traceEnt->client->ps.origin, traceEnt->s.number, MASK_PLAYERSOLID);
	if ( tr.allsolid ) {
		traceEnt->client->ps.pm_flags |= PMF_DUCKED;
	}

	traceEnt->r.contents = CONTENTS_CORPSE;

	if ( ent ) {
		trap_LinkEntity( ent );
		trap_SendServerCommand( traceEnt-g_entities, va("cp \"You have been revived by [lof]%s[lon] [lof]%s!\n\"", ent->client->sess.sessionTeam == TEAM_ALLIES ? rankNames_Allies[ ent->client->sess.rank ] : rankNames_Axis[ ent->client->sess.rank ], ent->client->pers.netname) );
		traceEnt->props_frame_state = ent->s.number;
		traceEnt->client->pers.lastrevive_client = ent->s.clientNum;  
	}
	// DHM - Nerve :: Mark that the medicine was indeed dispensed
	usedSyringe = qtrue;

	// sound
	te = G_TempEntity( traceEnt->r.currentOrigin, EV_GENERAL_SOUND );
	if ( resuscitation )
	te->s.eventParm = G_SoundIndex( "sound/misc/vo_resuscitate.wav" );
		else
	te->s.eventParm = G_SoundIndex( "sound/misc/vo_revive.wav" );

	// Xian -- This was gay and I always hated it.
	if ( g_fastres.integer > 0 )
		BG_AnimScriptEvent( &traceEnt->client->ps, traceEnt->client->pers.character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
	else {
		// DHM - Nerve :: Play revive animation
		BG_AnimScriptEvent( &traceEnt->client->ps, traceEnt->client->pers.character->animModelInfo, ANIM_ET_REVIVE, qfalse, qtrue );
		traceEnt->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
		traceEnt->client->ps.pm_time = 2100;
	}


	// Tell the caller if we actually used a syringe
	return usedSyringe;
}
// END - Mad Doc


// JPW NERVE Weapon_Syringe:
/*
======================
  Weapon_Syringe
	shoot the syringe, do the old lazarus bit
======================
*/
qboolean Weapon_Syringe(gentity_t *ent) {
	vec3_t		end;
	trace_t		tr;
	gentity_t	*traceEnt;
	int			healamt;  
	gentity_t	*te;  

	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePointForActivate( ent, forward, right, up, muzzleTrace, qfalse );
	VectorMA (muzzleTrace, CH_REVIVE_DIST, forward, end);
	trap_Trace (&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);


	if (tr.startsolid) {
		VectorMA (muzzleTrace, 8, forward, end);
		trap_Trace(&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);
	}


	if (tr.fraction == 1.0)
		return qfalse;

	traceEnt = &g_entities[ tr.entityNum ];

	if (!traceEnt->client)
		return qfalse;

	if(traceEnt->client->ps.pm_type == PM_DEAD )
	{
		qboolean	usedSyringe = qfalse;

		if ( traceEnt->client->sess.sessionTeam != ent->client->sess.sessionTeam )
			return qfalse;

		if ( traceEnt->client->ps.eFlags & EF_PLAYDEAD )
			return qfalse;

		usedSyringe = ReviveEntity( ent, traceEnt, qfalse );

		//if(g_gamestate.integer == GS_PLAYING)
		//	ent->client->sess.aWeaponStats[WS_SYRINGE].hits++;

		if(ent && ent->client)
			G_LogPrintf("Medic_Revive: %d %d\n", ent - g_entities, traceEnt - g_entities);

		if( !traceEnt->isProp ) { // Gordon: flag for if they were teamkilled or not
			AddScore(ent, WOLF_MEDIC_BONUS); // JPW NERVE props to the medic for the swift and dexterous bit o healitude
			G_AddSkillPoints( ent, SK_FIRST_AID, 4.f );
			G_DEBUG_ADD_SKILL_POINTS( ent, SK_FIRST_AID, 4.f, "reviving a player" );
		}

		if(usedSyringe)
			ent->client->sess.team_hits -= 2.f;

		// Arnout: calculate ranks to update numFinalDead arrays. Have to do it manually as addscore has an early out
		if( g_gametype.integer == GT_WOLF_LMS )
			CalculateRanks();
		// ok unpoison the guy
		traceEnt->client->ps.eFlags &= ~EF_POISONED;
		traceEnt->client->pmext.poisonAmnt = 0;

		return usedSyringe;
	} 
	else if ( traceEnt->client->ps.eFlags & EF_POISONED )
	{
		if ( traceEnt->client->sess.sessionTeam != ent->client->sess.sessionTeam )
				return qfalse;

		traceEnt->client->ps.eFlags &= ~EF_POISONED;
		traceEnt->client->pmext.poisonAmnt = 0;

		if ( g_medics.integer & MEDIC_SYRINGEHEAL ) 
			goto heal;
		else
			return qtrue;

	} 
	else if (g_medics.integer & MEDIC_SYRINGEHEAL)
	{
heal:
		if ( traceEnt->client->sess.sessionTeam != ent->client->sess.sessionTeam )
			return qfalse;

		if ( traceEnt->health > (traceEnt->client->ps.stats[STAT_MAX_HEALTH] * 0.25))
			return qfalse;

		// check the skill of the needle-bearer  
		if( ent->client->sess.skillBits[SK_MEDIC] & (1<<3) )  
			healamt = traceEnt->client->ps.stats[STAT_MAX_HEALTH];  
		else  
			healamt = traceEnt->client->ps.stats[STAT_MAX_HEALTH] * 0.5;

		traceEnt->health = healamt;  
		te = G_TempEntity( traceEnt->r.currentOrigin, EV_GENERAL_SOUND );  
		te->s.eventParm = G_SoundIndex( "sound/misc/vo_revive.wav" );
		ent->client->sess.team_hits -= 2.f;  

		//if(g_gamestate.integer == GS_PLAYING)
		//	ent->client->sess.aWeaponStats[WS_SYRINGE].hits++;  

		if( !traceEnt->isProp ) {  
			// perro:  may need to tune this based on feedback  
			// currently set to add XP like a medpack  
			// but not as much as a revive.  Builds skill faster than   
			// medpacks but not as much as revive...    
			AddScore(ent, WOLF_HEALTH_UP);   
			G_AddSkillPoints( ent, SK_FIRST_AID, 2.f );  
			G_DEBUG_ADD_SKILL_POINTS( ent, SK_FIRST_AID, 2.f, "syringe heal a player" );  
		}  

		return qtrue;
	}

	return qfalse;
}


qboolean Weapon_Poison(gentity_t *ent) {
	vec3_t		end;
	trace_t		tr;
	gentity_t	*traceEnt;
	gentity_t	*tent;

	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePointForActivate( ent, forward, right, up, muzzleTrace, qfalse );
	VectorMA (muzzleTrace, CH_REVIVE_DIST, forward, end);
	//trap_Trace (&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);

	// jaquboss - unlag me!
	G_HistoricalTrace(ent, &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);

/*	if (tr.startsolid) {
		VectorMA (muzzleTrace, 8, forward, end);			// CH_ACTIVATE_DIST
		trap_Trace(&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);
	}*/

	// todo add stab sound to make it more action?
	if (tr.fraction == 1.0)
		return qfalse;

	traceEnt = &g_entities[ tr.entityNum ];

	if(tr.entityNum >= MAX_CLIENTS)		
	{	
		// world brush or non-player entity (no blood)
		tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
	} 
	else								
	{	
		// other player						
		tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
	}

	tent->s.otherEntityNum	= tr.entityNum;
	tent->s.eventParm		= DirToByte( tr.plane.normal );
	tent->s.weapon			= ent->s.weapon;
	tent->s.clientNum		= ent->r.ownerNum;

	if(tr.entityNum >= MAX_CLIENTS)
		return qfalse;


	if( traceEnt->client->ps.powerups[PW_INVULNERABLE] >= level.time)
		return qfalse;

	if ( !G_FRIENDLYFIRE && OnSameTeam(ent, traceEnt))
		return qfalse;


	if( traceEnt->client->ps.eFlags & EF_POISONED ){
		// whoever did it before, now we are the poisoners
		traceEnt->client->pmext.poisonerEnt = ent->s.number;
		traceEnt->client->pmext.poisonAmnt++;
		return qtrue;
	}

	traceEnt->client->ps.eFlags |= EF_POISONED;
	traceEnt->client->pmext.poisonerEnt = ent->s.number;
	traceEnt->client->poisonTime = level.time;
	traceEnt->client->pmext.poisonAmnt = 1;
	return qtrue;
}

/*
======================
  Weapon_AdrenalineSyringe
	Hmmmm. Needles. With stuff in it. Woooo.
======================
*/
void Weapon_AdrenalineSyringe(gentity_t *ent) {
	ent->client->ps.powerups[PW_ADRENALINE] = level.time + 10000;
}

void G_ExplodeMissile( gentity_t *ent );
void DynaSink(gentity_t* self );

// Arnout: crude version of G_RadiusDamage to see if the dynamite can damage a func_constructible
int EntsThatRadiusCanDamage( vec3_t origin, float radius, int *damagedList ) {
	float		dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	int			i, e;
	float		boxradius;
	vec3_t		dest; 
	trace_t		tr;
	vec3_t		midpoint;
	int			numDamaged = 0;

	if ( radius < 1 ) {
		radius = 1;
	}

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement -- 
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if (!ent->r.bmodel)
			VectorSubtract(ent->r.currentOrigin,origin,v);
		else {
			for ( i = 0 ; i < 3 ; i++ ) {
				if ( origin[i] < ent->r.absmin[i] ) {
					v[i] = ent->r.absmin[i] - origin[i];
				} else if ( origin[i] > ent->r.absmax[i] ) {
					v[i] = origin[i] - ent->r.absmax[i];
				} else {
					v[i] = 0;
				}
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		if( CanDamage (ent, origin) ) {
			damagedList[numDamaged++] = entityList[e];
		} else {
			VectorAdd (ent->r.absmin, ent->r.absmax, midpoint);
			VectorScale (midpoint, 0.5, midpoint);
			VectorCopy (midpoint, dest);
			
			trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
			if (tr.fraction < 1.0) {
				VectorSubtract(dest,origin,dest);
				dist = VectorLength(dest);
				if (dist < radius*0.2f) { // closer than 1/4 dist
					damagedList[numDamaged++] = entityList[e];
				}
			}
		}
	}

	return( numDamaged );
}

void G_LandminePrime( gentity_t *self );
extern void explosive_indicator_think( gentity_t *ent );

#define MIN_BLOCKINGWARNING_INTERVAL 5000

static void MakeTemporarySolid( gentity_t *ent ) {
	if( ent->entstate == STATE_UNDERCONSTRUCTION ) {
		ent->clipmask = ent->realClipmask;
		ent->r.contents = ent->realContents;
		if( !ent->realNonSolidBModel )
			ent->s.eFlags &= ~EF_NONSOLID_BMODEL;
	}

	trap_LinkEntity( ent );
}

static void UndoTemporarySolid( gentity_t *ent ) {
	ent->entstate = STATE_UNDERCONSTRUCTION;
	ent->s.powerups = STATE_UNDERCONSTRUCTION;
	ent->realClipmask = ent->clipmask;
	ent->clipmask = 0;
	ent->realContents = ent->r.contents;
	ent->r.contents = 0;
	if( ent->s.eFlags & EF_NONSOLID_BMODEL )
		ent->realNonSolidBModel = qtrue;
	else
		ent->s.eFlags |= EF_NONSOLID_BMODEL;

	trap_LinkEntity( ent );
}

// handleBlockingEnts = kill players, return flags, remove entities
// warnBlockingPlayers = warn any players that are in the constructible area
static void HandleEntsThatBlockConstructible( gentity_t *constructor, gentity_t *constructible, qboolean handleBlockingEnts, qboolean warnBlockingPlayers ) {
	// check if something blocks us
	int constructibleList[MAX_GENTITIES];
	int entityList[MAX_GENTITIES];
	int blockingList[MAX_GENTITIES];
	int constructibleEntities = 0;
	int listedEntities, e;
	int blockingEntities = 0;
	gentity_t *check, *block;

	// backup...
	int constructibleModelindex = constructible->s.modelindex;
	int constructibleClipmask = constructible->clipmask;
	int constructibleContents = constructible->r.contents;
	int constructibleNonSolidBModel = (constructible->s.eFlags & EF_NONSOLID_BMODEL);

	trap_SetBrushModel( constructible, va( "*%i", constructible->s.modelindex2 ) );

	// ...and restore
	constructible->clipmask = constructibleClipmask;
	constructible->r.contents = constructibleContents;
	if( !constructibleNonSolidBModel )
		constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;
	trap_LinkEntity( constructible );

	// store our origin
	VectorCopy( constructible->r.absmin, constructible->s.origin2 );
	VectorAdd( constructible->r.absmax, constructible->s.origin2, constructible->s.origin2 );
	VectorScale( constructible->s.origin2, 0.5, constructible->s.origin2 );

	// get all the entities that make up the constructible
	if( constructible->track && constructible->track[0] ) {
		vec3_t mins, maxs;

		VectorCopy( constructible->r.absmin, mins );
		VectorCopy( constructible->r.absmax, maxs );

		check = NULL;

		while(1) {
			check = G_Find( check, FOFS(track), constructible->track );

			if( check == constructible )
				continue;

			if (!check ) {
				break;
			}

			if( constructible->count2 ) {
				if( check->partofstage != constructible->grenadeFired )
					continue;
			}

			// get the bounding box of all entities in the constructible together
			AddPointToBounds( check->r.absmin, mins, maxs );
			AddPointToBounds( check->r.absmax, mins, maxs );

			constructibleList[constructibleEntities++] = check->s.number;
		}

		listedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

		// make our constructible entities solid so we can check against them
		//trap_LinkEntity( constructible );
		MakeTemporarySolid( constructible );
		for( e = 0; e < constructibleEntities; e++ ) {
			check = &g_entities[constructibleList[e]];

			//trap_LinkEntity( check );
			MakeTemporarySolid( check );
		}

	} else {
		// Gordon: changed * to abs*
		listedEntities = trap_EntitiesInBox( constructible->r.absmin, constructible->r.absmax, entityList, MAX_GENTITIES );

		// make our constructible solid so we can check against it
		//trap_LinkEntity( constructible );
		MakeTemporarySolid( constructible );
	}

	for( e = 0; e < listedEntities; e++ ) {
		check = &g_entities[entityList[e]];

		// ignore everything but items, players and missiles (grenades too)
		if( check->s.eType != ET_MISSILE && check->s.eType != ET_ITEM && check->s.eType != ET_PLAYER && !check->physicsObject ) {
			continue;
		}

		// remove any corpses, this includes dynamite
		if( check->r.contents == CONTENTS_CORPSE ) {
			blockingList[blockingEntities++] = entityList[e];
			continue;
		}

		// FIXME : dynamite seems to test out of position?
		// see if the entity is in a solid now
		if((block = G_TestEntityPosition( check )) == NULL)
			continue;

		// the entity is blocked and it is a player, then warn the player
		if( warnBlockingPlayers && check->s.eType == ET_PLAYER ) {
			if( (level.time - check->client->lastConstructibleBlockingWarnTime) >= MIN_BLOCKINGWARNING_INTERVAL ) {
				trap_SendServerCommand( check->s.number, "cp \"Warning, leave the construction area...\" 1" );
				// Gordon: store the entity num to warn the bot
				check->client->lastConstructibleBlockingWarnEnt = constructible - g_entities;
				check->client->lastConstructibleBlockingWarnTime = level.time;
			}

			// unlink our entities again
			/*trap_UnlinkEntity( constructible );

			if( constructible->track && constructible->track[0] ) {
				for( e = 0; e < constructibleEntities; e++ ) {
					check = &g_entities[constructibleList[e]];

					trap_UnlinkEntity( check );
				}
			}
			return;*/
		}

		blockingList[blockingEntities++] = entityList[e];
	}

	// undo the temporary solid for our entities
	UndoTemporarySolid( constructible );
	if( constructible->track && constructible->track[0] ) {
		for( e = 0; e < constructibleEntities; e++ ) {
			check = &g_entities[constructibleList[e]];

			//trap_UnlinkEntity( check );
			UndoTemporarySolid( check );
		}
	}

	if( handleBlockingEnts ) {
		for( e = 0; e < blockingEntities; e++ ) {
			block = &g_entities[blockingList[e]];

			if( block->client || block->s.eType == ET_CORPSE ) 
			{
				G_Damage( block, constructible, constructor, NULL, NULL, 9999, DAMAGE_NO_PROTECTION, MOD_CRUSH_CONSTRUCTION );
				if (block->client)
				{
					// jet Pilot - darwin award for getting crushed in construction
					BG_AdjustDarwin( &block->client->ps, 1 );
				}
			} 
			else if( block->s.eType == ET_ITEM && block->item->giType == IT_TEAM ) {
				// see if it's a critical entity, one that we can't just simply kill (basically flags)
				Team_DroppedFlagThink( block );
			} else {
				// remove the landmine from both teamlists
				if ( block->s.eType == ET_MISSILE && block->methodOfDeath == MOD_LANDMINE ) {
					mapEntityData_t	*mEnt;

					if((mEnt = G_FindMapEntityData(&mapEntityData[0], block-g_entities)) != NULL) {
						G_FreeMapEntityData( &mapEntityData[0], mEnt );
					}

					if((mEnt = G_FindMapEntityData(&mapEntityData[1], block-g_entities)) != NULL) {
						G_FreeMapEntityData( &mapEntityData[1], mEnt );
					}
				}

				// just get rid of it
				G_TempEntity( block->s.origin, EV_ITEM_POP );
				G_FreeEntity( block );
			}
		}
	}

	if( constructibleModelindex ) {
		trap_SetBrushModel( constructible, va( "*%i", constructibleModelindex ) );
		// ...and restore
		constructible->clipmask = constructibleClipmask;
		constructible->r.contents = constructibleContents;
		if( !constructibleNonSolidBModel )
			constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;
		trap_LinkEntity( constructible );
	} else {
		constructible->s.modelindex = 0;
		//constructible->clipmask = constructibleClipmask;
		//constructible->r.contents = constructibleContents;
		//if( !constructibleNonSolidBModel )
		//	constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;
		trap_LinkEntity( constructible );
	}
}

#define CONSTRUCT_POSTDECAY_TIME 500

// !! NOTE !!: if the conditions here of a buildable constructible change, then BotIsConstructible() must reflect those changes

// returns qfalse when it couldn't build
static qboolean TryConstructing( gentity_t *ent )
{
	gentity_t	*check;
	gentity_t	*constructible = ent->client->touchingTOI->target_ent;
	int			i;

	// no construction during prematch
	if( level.warmupTime )
	{
		return( qfalse );
	}

	// see if we are in a trigger_objective_info targetting multiple func_constructibles
	if( constructible->s.eType == ET_CONSTRUCTIBLE && ent->client->touchingTOI->chain ) 
	{
		gentity_t *otherconstructible = NULL;

		// use the target that has the same team as the player
		if( constructible->s.teamNum != ent->client->sess.sessionTeam )
		{
			constructible = ent->client->touchingTOI->chain;
		}

		otherconstructible = constructible->chain;

		// make sure the other constructible isn't built/underconstruction/something
		if( otherconstructible->s.angles2[0] ||	otherconstructible->s.angles2[1] ||
		  ( otherconstructible->count2       && otherconstructible->grenadeFired ))
		{
			return( qfalse );
		}
	}

	// see if we are in a trigger_objective_info targetting a func_constructible
	if( constructible->s.eType == ET_CONSTRUCTIBLE &&
		constructible->s.teamNum == ent->client->sess.sessionTeam )	{

		// forty - constructible xp sharing  
		float addhealth;  
		float xpperround;  

		if( constructible->s.angles2[0] >= 250 ) // have to do this so we don't score multiple times
			return( qfalse );

		if( constructible->s.angles2[1] != 0 )
			return( qfalse );

		// Check if we can construct - updates the classWeaponTime as well
		if (!ReadyToConstruct(ent, constructible, qtrue))
			return qtrue;

		// try to start building
		if( constructible->s.angles2[0] <= 0 ) {
			// wait a bit, this prevents network spam
			if( level.time - constructible->lastHintCheckTime < CONSTRUCT_POSTDECAY_TIME )
				return( qtrue );	// likely will come back soon - so override other plier bits anyway

			// Gordon: are we scripted only?
			if( !(ent->spawnflags & CONSTRUCTIBLE_AAS_SCRIPTED) ) {
				if ( !(ent->spawnflags & CONSTRUCTIBLE_NO_AAS_BLOCKING) ) {
					// RF, if we are blocking AAS areas when built, then clear AAS blocking so we can set it again after the stage has been increased
					if( constructible->spawnflags & CONSTRUCTIBLE_BLOCK_PATHS_WHEN_BUILD ) {
						G_SetAASBlockingEntity( ent, AAS_AREA_ENABLED );
					}
				}
			}

			// swap brushmodels if staged
			if( constructible->count2 ) {
				constructible->grenadeFired++;
				constructible->s.modelindex2 = constructible->conbmodels[constructible->grenadeFired-1];
				//trap_SetBrushModel( constructible, va( "*%i", constructible->conbmodels[constructible->grenadeFired-1] ) );
			}

			G_SetEntState( constructible, STATE_UNDERCONSTRUCTION );

			if( !constructible->count2 ) {
				// call script
				G_Script_ScriptEvent( constructible, "buildstart", "final" );
				constructible->s.frame = 1;
			} else {
				if( constructible->grenadeFired == constructible->count2 ) {
					G_Script_ScriptEvent( constructible, "buildstart", "final" );
					constructible->s.frame = constructible->grenadeFired;
				} else {
					switch( constructible->grenadeFired ) {
					case 1: G_Script_ScriptEvent( constructible, "buildstart", "stage1" ); constructible->s.frame = 1; break;
					case 2: G_Script_ScriptEvent( constructible, "buildstart", "stage2" ); constructible->s.frame = 2; break;
					case 3: G_Script_ScriptEvent( constructible, "buildstart", "stage3" ); constructible->s.frame = 3; break;
					}
				}
			}

			{
				vec3_t mid;
				gentity_t* te;

				VectorAdd( constructible->parent->r.absmin, constructible->parent->r.absmax, mid );
				VectorScale( mid, 0.5f, mid );

				te = G_TempEntity( mid, EV_GENERAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/world/build.wav" );
			}
			

			if( ent->client->touchingTOI->chain && ent->client->touchingTOI->count2 ) {
				// find the constructible indicator and change team
				mapEntityData_t	*mEnt;
				mapEntityData_Team_t *teamList;
				gentity_t *indicator = &g_entities[ent->client->touchingTOI->count2];

				indicator->s.teamNum = constructible->s.teamNum;

				// update the map for the other team
				teamList = indicator->s.teamNum == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0]; // inversed
				if((mEnt = G_FindMapEntityData( teamList, indicator-g_entities)) != NULL) {
					G_FreeMapEntityData( teamList, mEnt );
				}
			}

			if( !constructible->count2 || constructible->grenadeFired == 1 ) {
				// link in if we just started building
				G_UseEntity( constructible, ent->client->touchingTOI, ent );
			}

			// setup our think function for decaying
			constructible->think = func_constructible_underconstructionthink;
			constructible->nextthink = level.time + FRAMETIME;

			G_PrintClientSpammyCenterPrint( ent-g_entities, "Constructing..." );
		}

		// forty - constructible xp sharing  
		if(g_constructiblexpsharing.integer) {  
					addhealth = (255.f/(constructible->constructibleStats.duration/(float)FRAMETIME));  
					xpperround = constructible->constructibleStats.constructxpbonus/(255.f/addhealth) + 0.01f;  

						G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, xpperround );  
						G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, xpperround, "construction sharing." );  
                  
		// Give health until it is full, don't continue  
		constructible->s.angles2[0] += addhealth;  
		} else {  
		// Give health until it is full, don't continue  
		constructible->s.angles2[0] += (255.f/(constructible->constructibleStats.duration/(float)FRAMETIME));  
		}  

		if ( constructible->s.angles2[0] >= 250 )
		{
			constructible->s.angles2[0] = 0;
			HandleEntsThatBlockConstructible( ent, constructible, qtrue, qfalse );
		} 
		else
		{
			constructible->lastHintCheckTime = level.time;
			HandleEntsThatBlockConstructible( ent, constructible, qfalse, qtrue );
            return( qtrue );	// properly constructed
		}

		//trap_SendServerCommand( ent-g_entities, "cp \"Job's done!\" 1");

		// eeeh no point in doing this twice
		//HandleEntsThatBlockConstructible( ent, constructible, qtrue, qfalse );
		if( constructible->count2 )
		{
			// backup...
			//int constructibleModelindex = constructible->s.modelindex;
			int constructibleClipmask = constructible->clipmask;
			int constructibleContents = constructible->r.contents;
			int constructibleNonSolidBModel = (constructible->s.eFlags & EF_NONSOLID_BMODEL);

			constructible->s.modelindex2 = 0;
			trap_SetBrushModel( constructible, va( "*%i", constructible->conbmodels[constructible->grenadeFired-1] ) );

			// ...and restore
			constructible->clipmask = constructibleClipmask;
			constructible->r.contents = constructibleContents;
			if( !constructibleNonSolidBModel )
				constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;

			if( constructible->grenadeFired == constructible->count2 ) {
				constructible->s.angles2[1] = 1;
			}
		} 
		else
		{
			// backup...
			//int constructibleModelindex = constructible->s.modelindex;
			int constructibleClipmask = constructible->clipmask;
			int constructibleContents = constructible->r.contents;
			int constructibleNonSolidBModel = (constructible->s.eFlags & EF_NONSOLID_BMODEL);

			constructible->s.modelindex2 = 0;
			trap_SetBrushModel( constructible, constructible->model );

			// ...and restore
			constructible->clipmask = constructibleClipmask;
			constructible->r.contents = constructibleContents;
			if( !constructibleNonSolidBModel )
				constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;

			constructible->s.angles2[1] = 1;
		}

		// Gordon: removing messages for now
/*		if( ent->client->touchingTOI->spawnflags & 4 ) { // MESSAGE_OVERRIDE
			gentity_t* pm = G_PopupMessage( PM_CONSTRUCTION );
			pm->s.effect3Time = G_StringIndex( ent->client->touchingTOI->spawnitem );
			pm->s.effect2Time = TEAM_AXIS;
			pm->s.density = -1; // -1 = built (custom msg)
		} else {
			gentity_t* pm = G_PopupMessage( PM_CONSTRUCTION );
			pm->s.density = ent->client->sess.sessionTeam;
			pm->s.effect2Time = 0; // 0 = built
			pm->s.effect3Time = ent->client->touchingTOI->s.teamNum;
		}*/

		// jet Pilot - TODO/FIXME - dont award xp to the guy if he's the one that broke it

		// hijack 
	//	if (ent->s.number != constructible->botLastAttackedEnt)
	//	{
			AddScore( ent, constructible->accuracy ); // give drop score to guy who built it
			// forty - constructible xp sharing  
			if(!g_constructiblexpsharing.integer) {  
					G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, constructible->constructibleStats.constructxpbonus );  
					G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, constructible->constructibleStats.constructxpbonus, "finishing a construction" );  
			}  

			
//		}
		// unlink the objective info to get rid of the indicator for now
		// Arnout: don't unlink, we still want the location popup. Instead, constructible_indicator_think got changed to free
		// the indicator when the constructible is constructed
//			if( constructible->parent )
//				trap_UnlinkEntity( constructible->parent );

		G_SetEntState( constructible, STATE_DEFAULT );

		// make destructable
		if( !(constructible->spawnflags & 2) ) {
			constructible->takedamage = qtrue;
			constructible->health = constructible->sound1to2;
		}

		// Stop thinking
		constructible->think = NULL;
		constructible->nextthink = 0;

		if( !constructible->count2 ) {
			// call script
			G_Script_ScriptEvent( constructible, "built", "final" );
		} else {
			if( constructible->grenadeFired == constructible->count2 ) {
				G_Script_ScriptEvent( constructible, "built", "final" );
			} else {
				switch( constructible->grenadeFired ) {
				case 1: G_Script_ScriptEvent( constructible, "built", "stage1" ); break;
				case 2: G_Script_ScriptEvent( constructible, "built", "stage2" ); break;
				case 3: G_Script_ScriptEvent( constructible, "built", "stage3" ); break;
				}
			}
		}

		// Stop sound
		if( constructible->parent->spawnflags & 8 ) {
			constructible->parent->s.loopSound = 0;
		} else {
			constructible->s.loopSound = 0;
		}

		//ent->client->ps.classWeaponTime = level.time; // Out of "ammo"

		// if not invulnerable and dynamite-able, create a 'destructable' marker for the other team
		if( !(constructible->spawnflags & CONSTRUCTIBLE_INVULNERABLE) && (constructible->constructibleStats.weaponclass >= 1) ) {
			if( !constructible->count2 || constructible->grenadeFired == 1 ) {
				gentity_t* tent = NULL;
				gentity_t *e;
				e = G_Spawn();

				e->r.svFlags = SVF_BROADCAST;
				e->classname = "explosive_indicator";
				e->s.pos.trType = TR_STATIONARY;
				e->s.eType = ET_EXPLOSIVE_INDICATOR;

				while((tent = G_Find (tent, FOFS(target), constructible->targetname)) != NULL) {
					if(tent->s.eType == ET_OID_TRIGGER) {
						if(tent->spawnflags & 8) {
							e->s.eType = ET_TANK_INDICATOR;
						}
					}
				}

				// Find the trigger_objective_info that targets us (if not set before)
				{
					gentity_t* tent = NULL;
					while((tent = G_Find (tent, FOFS(target), constructible->targetname)) != NULL) {
						if((tent->s.eType == ET_OID_TRIGGER)) {
							e->parent = tent;
						}
					}
				}

				if ( constructible->spawnflags & AXIS_CONSTRUCTIBLE )
					e->s.teamNum = TEAM_AXIS;
				else if ( constructible->spawnflags & ALLIED_CONSTRUCTIBLE )
					e->s.teamNum = TEAM_ALLIES;

				e->s.modelindex2 = ent->client->touchingTOI->s.teamNum;
				e->r.ownerNum = constructible->s.number;
				e->think = explosive_indicator_think;
				e->nextthink = level.time + FRAMETIME;

				e->s.effect1Time = constructible->constructibleStats.weaponclass;

				if(constructible->parent->tagParent) {
					e->tagParent = constructible->parent->tagParent;
					Q_strncpyz( e->tagName, constructible->parent->tagName, MAX_QPATH );
				} else {
					VectorCopy( constructible->r.absmin, e->s.pos.trBase );
					VectorAdd( constructible->r.absmax, e->s.pos.trBase, e->s.pos.trBase );
					VectorScale( e->s.pos.trBase, 0.5, e->s.pos.trBase );
				}

				SnapVector( e->s.pos.trBase );

				trap_LinkEntity( e );
			} else {
				// find our marker and update it's coordinates
				for( i = 0, check = g_entities; i < level.num_entities; i++, check++) {
					if( check->s.eType != ET_EXPLOSIVE_INDICATOR && check->s.eType != ET_TANK_INDICATOR && check->s.eType != ET_TANK_INDICATOR_DEAD )
						continue;

					if( check->r.ownerNum == constructible->s.number ) {
						// found it!
						if(constructible->parent->tagParent) {
							check->tagParent = constructible->parent->tagParent;
							Q_strncpyz( check->tagName, constructible->parent->tagName, MAX_QPATH );
						} else {
							VectorCopy( constructible->r.absmin, check->s.pos.trBase );
							VectorAdd( constructible->r.absmax, check->s.pos.trBase, check->s.pos.trBase );
							VectorScale( check->s.pos.trBase, 0.5, check->s.pos.trBase );

							SnapVector( check->s.pos.trBase );
						}

						trap_LinkEntity( check );
						break;
					}
				}
			}
		}

		// Gordon: are we scripted only?
		if( !(ent->spawnflags & CONSTRUCTIBLE_AAS_SCRIPTED) ) {
			if ( !(ent->spawnflags & CONSTRUCTIBLE_NO_AAS_BLOCKING) ) {
				// RF, a stage has been completed, either enable or disable AAS areas appropriately
				if( !(constructible->spawnflags & CONSTRUCTIBLE_BLOCK_PATHS_WHEN_BUILD) ) {
					// builing creates AAS paths
					// Gordon: HACK from ryan
	//				if( !constructible->count2 || ( constructible->grenadeFired == constructible->count2 ) )
					{
						// completely built, enable paths
						G_SetAASBlockingEntity( constructible, AAS_AREA_ENABLED );
					}
				} else {
					// builing blocks AAS paths
					G_SetAASBlockingEntity( constructible, AAS_AREA_DISABLED );
				}
			}
		}

		return( qtrue );	// building
	}

	return( qfalse );
}

void AutoBuildConstruction( gentity_t* constructible ) {
	int i;
	gentity_t* check;

	HandleEntsThatBlockConstructible( NULL, constructible, qtrue, qfalse );
	if( constructible->count2 ) {
		// backup...
		//int constructibleModelindex = constructible->s.modelindex;
		int constructibleClipmask = constructible->clipmask;
		int constructibleContents = constructible->r.contents;
		int constructibleNonSolidBModel = (constructible->s.eFlags & EF_NONSOLID_BMODEL);

		constructible->s.modelindex2 = 0;
		trap_SetBrushModel( constructible, va( "*%i", constructible->conbmodels[constructible->grenadeFired-1] ) );

		// ...and restore
		constructible->clipmask = constructibleClipmask;
		constructible->r.contents = constructibleContents;
		if( !constructibleNonSolidBModel )
			constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;

		if( constructible->grenadeFired == constructible->count2 ) {
			constructible->s.angles2[1] = 1;
		}
	} else {
		// backup...
		//int constructibleModelindex = constructible->s.modelindex;
		int constructibleClipmask = constructible->clipmask;
		int constructibleContents = constructible->r.contents;
		int constructibleNonSolidBModel = (constructible->s.eFlags & EF_NONSOLID_BMODEL);

		constructible->s.modelindex2 = 0;
		trap_SetBrushModel( constructible, constructible->model );

		// ...and restore
		constructible->clipmask = constructibleClipmask;
		constructible->r.contents = constructibleContents;
		if( !constructibleNonSolidBModel )
			constructible->s.eFlags &= ~EF_NONSOLID_BMODEL;

		constructible->s.angles2[1] = 1;
	}

	// unlink the objective info to get rid of the indicator for now
	// Arnout: don't unlink, we still want the location popup. Instead, constructible_indicator_think got changed to free
	// the indicator when the constructible is constructed
//			if( constructible->parent )
//				trap_UnlinkEntity( constructible->parent );

	G_SetEntState( constructible, STATE_DEFAULT );

	// make destructable
	if( !(constructible->spawnflags & CONSTRUCTIBLE_INVULNERABLE) ) {
		constructible->takedamage = qtrue;
		constructible->health = constructible->constructibleStats.health;
	}

	// Stop thinking
	constructible->think = NULL;
	constructible->nextthink = 0;

	if( !constructible->count2 ) {
		// call script
		G_Script_ScriptEvent( constructible, "built", "final" );
	} else {
		if( constructible->grenadeFired == constructible->count2 ) {
			G_Script_ScriptEvent( constructible, "built", "final" );
		} else {
			switch( constructible->grenadeFired ) {
			case 1: G_Script_ScriptEvent( constructible, "built", "stage1" ); break;
			case 2: G_Script_ScriptEvent( constructible, "built", "stage2" ); break;
			case 3: G_Script_ScriptEvent( constructible, "built", "stage3" ); break;
			}
		}
	}

	// Stop sound
	if( constructible->parent->spawnflags & 8 ) {
		constructible->parent->s.loopSound = 0;
	} else {
		constructible->s.loopSound = 0;
	}

	//ent->client->ps.classWeaponTime = level.time; // Out of "ammo"

	// if not invulnerable and dynamite-able, create a 'destructable' marker for the other team
	if( !(constructible->spawnflags & CONSTRUCTIBLE_INVULNERABLE) && (constructible->constructibleStats.weaponclass >= 1) ) {
		if( !constructible->count2 || constructible->grenadeFired == 1 ) {
			gentity_t* tent = NULL;
			gentity_t *e;
			e = G_Spawn();

			e->r.svFlags = SVF_BROADCAST;
			e->classname = "explosive_indicator";
			e->s.pos.trType = TR_STATIONARY;
			e->s.eType = ET_EXPLOSIVE_INDICATOR;

			while((tent = G_Find(tent, FOFS(target), constructible->targetname)) != NULL) {
				if((tent->s.eType == ET_OID_TRIGGER)) {
					if(tent->spawnflags & 8) {
						e->s.eType = ET_TANK_INDICATOR;
					}
				}
			}

			// Find the trigger_objective_info that targets us (if not set before)
			{
				gentity_t* tent = NULL;
				while((tent = G_Find (tent, FOFS(target), constructible->targetname)) != NULL) {
					if((tent->s.eType == ET_OID_TRIGGER)) {
						e->parent = tent;
					}
				}
			}

			if ( constructible->spawnflags & AXIS_CONSTRUCTIBLE )
				e->s.teamNum = TEAM_AXIS;
			else if ( constructible->spawnflags & ALLIED_CONSTRUCTIBLE )
				e->s.teamNum = TEAM_ALLIES;

			e->s.modelindex2 = constructible->parent->s.teamNum == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
			e->r.ownerNum = constructible->s.number;
			e->think = explosive_indicator_think;
			e->nextthink = level.time + FRAMETIME;

			e->s.effect1Time = constructible->constructibleStats.weaponclass;

			if(constructible->parent->tagParent) {
				e->tagParent = constructible->parent->tagParent;
				Q_strncpyz( e->tagName, constructible->parent->tagName, MAX_QPATH );
			} else {
				VectorCopy( constructible->r.absmin, e->s.pos.trBase );
				VectorAdd( constructible->r.absmax, e->s.pos.trBase, e->s.pos.trBase );
				VectorScale( e->s.pos.trBase, 0.5, e->s.pos.trBase );
			}

			SnapVector( e->s.pos.trBase );

			trap_LinkEntity( e );
		} else {
			// find our marker and update it's coordinates
			for( i = 0, check = g_entities; i < level.num_entities; i++, check++) {
				if( check->s.eType != ET_EXPLOSIVE_INDICATOR && check->s.eType != ET_TANK_INDICATOR && check->s.eType != ET_TANK_INDICATOR_DEAD )
					continue;

				if( check->r.ownerNum == constructible->s.number ) {
					// found it!
					if(constructible->parent->tagParent) {
						check->tagParent = constructible->parent->tagParent;
						Q_strncpyz( check->tagName, constructible->parent->tagName, MAX_QPATH );
					} else {
						VectorCopy( constructible->r.absmin, check->s.pos.trBase );
						VectorAdd( constructible->r.absmax, check->s.pos.trBase, check->s.pos.trBase );
						VectorScale( check->s.pos.trBase, 0.5, check->s.pos.trBase );

						SnapVector( check->s.pos.trBase );
					}

					trap_LinkEntity( check );
					break;
				}
			}
		}
	}

	// Gordon: are we scripted only?
	if( !(constructible->spawnflags & CONSTRUCTIBLE_AAS_SCRIPTED) ) {
		if ( !(constructible->spawnflags & CONSTRUCTIBLE_NO_AAS_BLOCKING) ) {
			// RF, a stage has been completed, either enable or disable AAS areas appropriately
			if( !(constructible->spawnflags & CONSTRUCTIBLE_BLOCK_PATHS_WHEN_BUILD) ) {
				// builing creates AAS paths
				if( !constructible->count2 || ( constructible->grenadeFired == constructible->count2 ) ) {
					// completely built, enable paths
					G_SetAASBlockingEntity( constructible, AAS_AREA_ENABLED );
				}
			} else {
				// builing blocks AAS paths
				G_SetAASBlockingEntity( constructible, AAS_AREA_DISABLED );
			}
		}
	}
}

qboolean G_LandmineTriggered( gentity_t* ent ) {
	switch( ent->s.teamNum ) {
		case TEAM_AXIS + 8:
		case TEAM_ALLIES + 8:
			return qtrue;
	}

	return qfalse;
}

qboolean G_LandmineArmed( gentity_t* ent ) {
	switch( ent->s.teamNum ) {
		case TEAM_AXIS:
		case TEAM_ALLIES:
			return qtrue;
	}
	return qfalse;
}

qboolean G_LandmineUnarmed( gentity_t* ent ) {
	return (!G_LandmineArmed( ent ) && !G_LandmineTriggered( ent ));
}

team_t G_LandmineTeam( gentity_t* ent ) {
	return (ent->s.teamNum % 4);
}

qboolean G_LandmineSpotted( gentity_t* ent ) {
	return ent->s.modelindex2 ? qtrue : qfalse;
}

void trap_EngineerTrace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	G_TempTraceIgnorePlayersAndBodies();
	trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );
	G_ResetTempTraceIgnoreEnts();
}

// jet Pilot - Shortcut to engineer level
#define	ENGINEER_LEVEL ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION]

// jet Pilot - Engineer_Landmine
// Attempt to arm / disarm a landmine
void Engineer_Landmine( gentity_t *ent, gentity_t *traceEnt )
{
	if (!ent || !traceEnt)
	{
		return;
	}
}

// jet Pilot - Engineer_Dynamite
void Engineer_Dynamite( gentity_t *ent, gentity_t *traceEnt )
{
	if (!ent || !traceEnt)
	{
		return;
	}
}

// jet Pilot - Engineer_MG42
qboolean Engineer_MG42( gentity_t *ent, gentity_t *traceEnt )
{
	if ( ent->client->ps.classWeaponTime + level.engineerChargeTime[ent->client->sess.sessionTeam-1] < level.time ) 
	{
		ent->client->ps.classWeaponTime = level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1];
	}
	
	// is not this wrong?
	ent->client->ps.classWeaponTime += (int)BG_GetFromTable( chargeCost_Repair, ent->client->sess.skillBits, SK_ENGINEER )*150;


	// No charge bar
	if ( ent->client->ps.classWeaponTime > level.time )
	{
		ent->client->ps.classWeaponTime = level.time;
		return qfalse;		
	}
	
	// finished repairing it
	if( traceEnt->health >= 255 )			
	{
		traceEnt->s.frame = 0;

		if ( traceEnt->mg42BaseEnt > 0 ) 
		{
			g_entities[ traceEnt->mg42BaseEnt ].health		= MG42_MULTIPLAYER_HEALTH;
			g_entities[ traceEnt->mg42BaseEnt ].takedamage  = qtrue;
			traceEnt->health = 0;
		} 
		else
		{
			traceEnt->health = MG42_MULTIPLAYER_HEALTH;
		}

		G_LogPrintf("Repair: %d\n", ent - g_entities);	// OSP

		// make sure that our team didn't damage it before awarding xp for repairing it
		if( traceEnt->sound3to2 != ent->client->sess.sessionTeam )
		{
			AddScore( ent, WOLF_REPAIR_BONUS ); 
			G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 3.f );
			G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 3.f, "repairing a MG42" );
		}

		traceEnt->takedamage = qtrue;
		traceEnt->s.eFlags  &= ~EF_SMOKING;

		trap_SendServerCommand(ent-g_entities, "cp \"You have repaired the MG!\n\"");
		G_AddEvent( ent, EV_MG42_FIXED, 0 );
	} 
	else // not finished, so continue repairing it
	{
		traceEnt->health += 3;
	}

	return qtrue;
}

// jet Pilot - Engineer_Other
void Engineer_Other( gentity_t *ent, gentity_t *traceEnt )
{
	if (!ent || !traceEnt)
	{
		return;
	}
}

// DHM - Nerve
void Weapon_Engineer( gentity_t *ent ) 
{
	trace_t		tr;
	gentity_t	*traceEnt, *hit;
	vec3_t		mins, maxs;									// JPW NERVE
	int			i, num, touch[MAX_GENTITIES], scored = 0;	// JPW NERVE
	int			dynamiteDropTeam;
//	int			ENGINEER_LEVEL;
	vec3_t		end;
	vec3_t		origin;

	// DHM - Nerve :: Can't heal an MG42 if you're using one!
	if( ent->client->ps.persistant[PERS_HWEAPON_USE] ) 
	{
		return;
	}

	// if touching a constructable objective, try to build it
	if( ent->client->touchingTOI )
	{
		if( TryConstructing( ent ) ) 
		{
			return;
		}
	}

	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	VectorCopy( ent->client->ps.origin, muzzleTrace );
	muzzleTrace[2] += ent->client->ps.viewheight;
	
	VectorMA (muzzleTrace, 64, forward, end);			// CH_BREAKABLE_DIST
	trap_EngineerTrace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT | CONTENTS_TRIGGER );
	
	if( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	// jet Pilot - TODO : Add helping mg soldiers here

	// no contact
	if( tr.fraction == 1.0f ) 
	{
		return;
	}

	// dont mess with the world, cause it's unmessable-with
	if( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) 
	{
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if( G_EmplacedGunIsRepairable( traceEnt, ent ) )
	{

		Engineer_MG42( ent, traceEnt);


	}
	else	// not an MG needin' fixin'
	{
		trap_EngineerTrace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );

		if ( tr.surfaceFlags & SURF_NOIMPACT )
		{
			return;
		}
		
		if(tr.fraction == 1.0f)
		{
			return;
		}
		
		if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD )
		{
			return;
		}

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->methodOfDeath == MOD_LANDMINE ) 
		{
			trace_t tr2;
			vec3_t base;
			vec3_t tr_down = {0, 0, 16};

			VectorSubtract(traceEnt->s.pos.trBase, tr_down, base);

			trap_EngineerTrace( &tr2, traceEnt->s.pos.trBase, NULL, NULL, base, traceEnt->s.number, MASK_SHOT );
			
			// ydnar: added "surfaceparm landmine" (SURF_LANDMINE) support
			if( !(tr2.surfaceFlags & SURF_LANDMINE) || (tr2.entityNum != ENTITYNUM_WORLD && (!g_entities[tr2.entityNum].inuse || g_entities[tr2.entityNum].s.eType != ET_CONSTRUCTIBLE))) 
			{
				trap_SendServerCommand(ent-g_entities, "cp \"Landmine cannot be armed here...\" 1");

				G_FreeEntity( traceEnt );

				Add_Ammo(ent, WP_LANDMINE, 1, qfalse);				

				// jet Pilot - use charge cost array (Landmine)
				jP_UnDoChargeCost(ent, SK_ENGINEER, BG_GetFromTable(chargeCost_Landmine, ent->client->sess.skillBits, SK_ENGINEER));

				ent->client->sess.aWeaponStats[WS_LANDMINE].atts--;
				return;
//bani
// rain - #384 - check landmine team so that enemy mines can be disarmed
// even if you're using all of yours :x
			} 
			else if ( G_CountTeamLandmines(ent->client->sess.sessionTeam) >= team_maxLandmines.integer && G_LandmineTeam(traceEnt) == ent->client->sess.sessionTeam) 
			{
				if(G_LandmineUnarmed(traceEnt)) 
				{
					trap_SendServerCommand(ent-g_entities, "cp \"Your team has too many landmines placed...\" 1");

					G_FreeEntity( traceEnt );

					Add_Ammo(ent, WP_LANDMINE, 1, qfalse);
					// rain - #202 - give back the correct charge amount
					// jet Pilot - use charge cost array (Landmine)
					jP_UnDoChargeCost(ent, SK_ENGINEER, BG_GetFromTable(chargeCost_Landmine, ent->client->sess.skillBits, SK_ENGINEER));


					ent->client->sess.aWeaponStats[WS_LANDMINE].atts--;
					return;
				}
//bani - #471
				else 
				{
					goto evilbanigoto;
				}
			}
			else // dont have too many mine placed already, and it is a suitable spot
			{
				if(G_LandmineUnarmed(traceEnt)) {
					// Opposing team cannot accidentally arm it
					if( G_LandmineTeam(traceEnt) != ent->client->sess.sessionTeam )
						return;

					G_PrintClientSpammyCenterPrint(ent-g_entities, "Arming landmine...");

					// Give health until it is full, don't continue

					// jet Pilot - Use Arming Speed Array (Landmine)
					//traceEnt->health += armSpeed_LandMine[ENGINEER_LEVEL]; 
					
					traceEnt->health += (int)BG_GetFromTable( armSpeed_LandMine, ent->client->sess.skillBits, SK_ENGINEER );
					
					if ( traceEnt->health >= 250 ) {
						//traceEnt->health = 255;
						trap_SendServerCommand(ent-g_entities, "cp \"Landmine armed...\" 1");
					} else {
						return;
					}

					traceEnt->r.contents = 0;	// (player can walk through)
					trap_LinkEntity( traceEnt );

					// jet Pilot - Forty mod crosshair mines
					traceEnt->s.otherEntityNum = ent->s.number; 

					// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
					traceEnt->timestamp 	= level.time + 1000;
					traceEnt->health		= 0;
					traceEnt->s.teamNum		= ent->client->sess.sessionTeam;
					traceEnt->s.modelindex2 = 0;
					traceEnt->nextthink		= level.time + 2000;
					traceEnt->think			= G_LandminePrime;
				} 
				else // landmine is armed
				{
//bani - #471
evilbanigoto:
					if (traceEnt->timestamp > level.time)
						return;
					if (traceEnt->health >= 250) // have to do this so we don't score multiple times
						return;

					// jet Pilot - Use Arming Speed Array (Landmine) (defuse at 1/4 speed)
					//traceEnt->health += disarmSpeed_LandMine[ENGINEER_LEVEL]; 
					
					traceEnt->health += (int)BG_GetFromTable( disarmSpeed_LandMine, ent->client->sess.skillBits, SK_ENGINEER );
					

					G_PrintClientSpammyCenterPrint(ent-g_entities, "Defusing landmine");

					if ( traceEnt->health >= 250 ) {
						mapEntityData_t	*mEnt;

						trap_SendServerCommand(ent-g_entities, "cp \"Landmine defused...\" 1");

						Add_Ammo(ent, WP_LANDMINE, 1, qfalse);

						if( G_LandmineTeam( traceEnt ) != ent->client->sess.sessionTeam ) {
							G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 4.f );
							G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 4.f, "defusing an enemy landmine" );
						}

						if((mEnt = G_FindMapEntityData(&mapEntityData[0], traceEnt-g_entities)) != NULL) {
							G_FreeMapEntityData( &mapEntityData[0], mEnt );
						}

						if((mEnt = G_FindMapEntityData(&mapEntityData[1], traceEnt-g_entities)) != NULL) {
							G_FreeMapEntityData( &mapEntityData[1], mEnt );
						}

						G_FreeEntity( traceEnt );

					} else {
						return;
					}
				}	
			}
		} 
		else if ( traceEnt->methodOfDeath == MOD_SATCHEL ) 
		{
			if( traceEnt->health >= 250 ) // have to do this so we don't score multiple times
				return;
			
			// jet Pilot - Use disarm speed array (Satchel)
			//traceEnt->health += disarmSpeed_Satchel[ENGINEER_LEVEL]; // 3;

			traceEnt->health += (int)BG_GetFromTable( disarmSpeed_Satchel, ent->client->sess.skillBits, SK_ENGINEER );


			G_PrintClientSpammyCenterPrint(ent-g_entities, "Disarming satchel charge...");

			if ( traceEnt->health >= 250 ) {

				traceEnt->health = 255;
				traceEnt->think = G_FreeEntity;
				traceEnt->nextthink = level.time + FRAMETIME;

				//bani - consistency with dynamite defusing
				G_PrintClientSpammyCenterPrint(ent-g_entities, "Satchel charge disarmed...");

				// forty - don't give points for disarming your teammates satchel  
				if( traceEnt->s.teamNum  != ent->client->sess.sessionTeam ) { 
						G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
						G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "disarming satchel charge" );
				}
			} else {
				return;
			}
//bani - no tripmine...
#if 0
		} else if ( traceEnt->methodOfDeath == MOD_TRIPMINE ) {
			// Give health until it is full, don't continue
			traceEnt->health += 3;

			G_PrintClientSpammyCenterPrint(ent-g_entities, "Disarming tripmine...");

			if ( traceEnt->health >= 250 ) {
				traceEnt->health = 255;
				traceEnt->think = G_FreeEntity;
				traceEnt->nextthink = level.time + FRAMETIME;

				Add_Ammo(ent, WP_TRIPMINE, 1, qfalse);
			} else {
				return;
			}
#endif
		} else
		if ( traceEnt->methodOfDeath == MOD_DYNAMITE ) {

			// Not armed
			if ( traceEnt->s.teamNum >= 4 ) {
				//bani
				qboolean friendlyObj	= qfalse;
				qboolean enemyObj		= qfalse;

				// Opposing team cannot accidentally arm it
				if ( (traceEnt->s.teamNum - 4) != ent->client->sess.sessionTeam )
					return;

				G_PrintClientSpammyCenterPrint(ent-g_entities, "Arming dynamite...");

				// Give health until it is full, don't continue

				// jet Pilot - use arming speed array (Dynamite)
				//traceEnt->health += armSpeed_Dynamite[ENGINEER_LEVEL];

				traceEnt->health += (int)BG_GetFromTable( armSpeed_Dynamite, ent->client->sess.skillBits, SK_ENGINEER );

				{
					int		entityList[MAX_GENTITIES];
					int		numListedEntities;
					int		e;
					vec3_t  org;

					VectorCopy( traceEnt->r.currentOrigin, org );
					org[2] += 4;	// move out of ground

					G_TempTraceIgnorePlayersAndBodies();
					numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );
					G_ResetTempTraceIgnoreEnts();

					for( e = 0; e < numListedEntities; e++ ) {
						hit = &g_entities[entityList[ e ]];

						if( hit->s.eType != ET_CONSTRUCTIBLE ) {
							continue;
						}

						// invulnerable
						if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (hit->parent && hit->parent->spawnflags & 8) ) {
							continue;
						}

						if( !G_ConstructionIsPartlyBuilt( hit ) ) {
							continue;
						}

						// is it a friendly constructible
						if( hit->s.teamNum == traceEnt->s.teamNum - 4 ) {
							friendlyObj = qtrue;
						}
					}
				}

				VectorCopy( traceEnt->r.currentOrigin, origin );
				SnapVector( origin );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );

				for ( i=0 ; i<num ; i++ ) {
					hit = &g_entities[touch[i]];

					if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
						continue;
					}

					if ((hit->s.eType == ET_OID_TRIGGER)) {
						if ( !(hit->spawnflags & (AXIS_OBJECTIVE|ALLIED_OBJECTIVE)) )
							continue;

						// Arnout - only if it targets a func_explosive
						if( hit->target_ent && Q_stricmp( hit->target_ent->classname, "func_explosive" ) )
							continue;

						if ( ((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS)) || 
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ) {
							friendlyObj = qtrue;
						}

						//bani
						if ( ((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ||
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS)) ) {
							enemyObj = qtrue;
						}
					}
				}

				//bani
				if( friendlyObj && !enemyObj ) {
					G_FreeEntity( traceEnt );
					trap_SendServerCommand( ent-g_entities, "cp \"You cannot arm dynamite near a friendly objective!\" 1");
					return;
				}

				if ( traceEnt->health >= 250 ) {
					traceEnt->health = 255;
				} else {
					return;
				}

				// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
				traceEnt->timestamp = level.time + 1000;
				traceEnt->health = 5;

				// set teamnum so we can check it for drop/defuse exploit
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
				// For dynamic light pulsing
				traceEnt->s.effect1Time = level.time;

				// ARM IT!
				traceEnt->nextthink = level.time + 30000;
				traceEnt->think = G_ExplodeMissile;

				// Gordon: moved down here to prevent two prints when dynamite IS near objective

				trap_SendServerCommand( ent-g_entities, "cp \"Dynamite is now armed with a 30 second timer!\" 1");

				// check if player is in trigger objective field
				// NERVE - SMF - made this the actual bounding box of dynamite instead of range, also must snap origin to line up properly
				VectorCopy( traceEnt->r.currentOrigin, origin );
				SnapVector( origin );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

				for ( i=0 ; i<num ; i++ ) {
					hit = &g_entities[touch[i]];

					if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
						continue;
					}
					if ((hit->s.eType == ET_OID_TRIGGER)) {

						if ( !(hit->spawnflags & (AXIS_OBJECTIVE|ALLIED_OBJECTIVE)) )
							continue;

						// Arnout - only if it targets a func_explosive
						if( hit->target_ent && Q_stricmp( hit->target_ent->classname, "func_explosive" ) )
							continue;

						if ( hit->spawnflags & AXIS_OBJECTIVE ) {
							if (ent->client->sess.sessionTeam == TEAM_ALLIES) { // transfer score info if this is a bomb scoring objective
								traceEnt->accuracy = hit->accuracy;
							}
						} else if (hit->spawnflags & ALLIED_OBJECTIVE) {
							if (ent->client->sess.sessionTeam == TEAM_AXIS) { // ditto other team
								traceEnt->accuracy = hit->accuracy;
							}
						}

						// rain - spawnflags 128 = disabled (#309)
						if (!(hit->spawnflags & 128) && (((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ||
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS))) ) {

							gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
							pm->s.effect2Time = 0;
							pm->s.effect3Time = hit->s.teamNum;
							pm->s.teamNum = ent->client->sess.sessionTeam;

							G_Script_ScriptEvent( hit, "dynamited", "" );

#ifdef OMNIBOTS
							// notify omni-bot framework of planted dynamite
							if (traceEnt->s.teamNum == TEAM_AXIS)
								Bot_Util_AddGoal((GameEntity)traceEnt, ET_GOAL_DYNAMITE, (1 << ET_TEAM_ALLIES), NULL, NULL);
							else
								Bot_Util_AddGoal((GameEntity)traceEnt, ET_GOAL_DYNAMITE, (1 << ET_TEAM_AXIS), NULL, NULL);
#endif

							if ( !(hit->spawnflags & OBJECTIVE_DESTROYED) ) {
								AddScore(traceEnt->parent, WOLF_DYNAMITE_PLANT); // give drop score to guy who dropped it
								if(traceEnt->parent && traceEnt->parent->client) {
									G_LogPrintf("Dynamite_Plant: %d\n", traceEnt->parent - g_entities);	// OSP
								}
								traceEnt->parent = ent; // give explode score to guy who armed it
							}
							//bani - fix #238
							traceEnt->etpro_misc_1 |= 1;
						}
//bani
						return;	//bani - bail out here because primary obj's take precendence over constructibles
					}		
				}

//bani - reordered this check so its AFTER the primary obj check
				// Arnout - first see if the dynamite is planted near a constructable object that can be destroyed
				{
					int		entityList[MAX_GENTITIES];
					int		numListedEntities;
					int		e;
					vec3_t  org;

					VectorCopy( traceEnt->r.currentOrigin, org );
					org[2] += 4;	// move out of ground

					G_TempTraceIgnorePlayersAndBodies();
					numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );
					G_ResetTempTraceIgnoreEnts();

					for( e = 0; e < numListedEntities; e++ ) {
						hit = &g_entities[entityList[ e ]];

						if( hit->s.eType != ET_CONSTRUCTIBLE )
							continue;

						// invulnerable
						if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE )
							continue;

						if( !G_ConstructionIsPartlyBuilt( hit ) ) {
							continue;
						}

						// is it a friendly constructible
						if( hit->s.teamNum == traceEnt->s.teamNum ) {
//bani - er, didnt we just pass this check earlier?
							continue;
						}

						// not dynamite-able
						if( hit->constructibleStats.weaponclass < 1 ) {
							continue;
						}

						if( hit->parent ) {
							gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
							pm->s.effect2Time = 0; // 0 = planted
							pm->s.effect3Time = hit->parent->s.teamNum;
							pm->s.teamNum = ent->client->sess.sessionTeam;

							G_Script_ScriptEvent( hit, "dynamited", "" );

#ifdef OMNIBOTS
							// notify omni-bot framework of planted dynamite
							if (traceEnt->s.teamNum == TEAM_AXIS)
								Bot_Util_AddGoal((GameEntity)traceEnt, ET_GOAL_DYNAMITE, (1 << ET_TEAM_ALLIES), NULL, NULL);
							else
								Bot_Util_AddGoal((GameEntity)traceEnt, ET_GOAL_DYNAMITE, (1 << ET_TEAM_AXIS), NULL, NULL);
#endif	

							if( (!(hit->parent->spawnflags & OBJECTIVE_DESTROYED)) && 
								hit->s.teamNum && (hit->s.teamNum == ent->client->sess.sessionTeam) ) {	// ==, as it's inverse
								AddScore(traceEnt->parent, WOLF_DYNAMITE_PLANT); // give drop score to guy who dropped it
								if( traceEnt->parent && traceEnt->parent->client ) {
									G_LogPrintf("Dynamite_Plant: %d\n", traceEnt->parent - g_entities);	// OSP
								}
								traceEnt->parent = ent; // give explode score to guy who armed it
							}
							//bani - fix #238
							traceEnt->etpro_misc_1 |= 1;
						}
						return;
					}
				}
			} 
			else 
			{
				if (traceEnt->timestamp > level.time)
					return;
				if (traceEnt->health >= 248) // have to do this so we don't score multiple times
					return;
				dynamiteDropTeam = traceEnt->s.teamNum; // set this here since we wack traceent later but want teamnum for scoring
				
				//traceEnt->health += disarmSpeed_Dynamite[ENGINEER_LEVEL];

				traceEnt->health += (int)BG_GetFromTable( disarmSpeed_Dynamite, ent->client->sess.skillBits, SK_ENGINEER );
				

				G_PrintClientSpammyCenterPrint(ent-g_entities, "Defusing dynamite...");

				if ( traceEnt->health >= 248 ) {
//bani
					qboolean defusedObj = qfalse;

					traceEnt->health = 255;
					// Need some kind of event/announcement here

					traceEnt->think = G_FreeEntity;
					traceEnt->nextthink = level.time + FRAMETIME;

					VectorCopy( traceEnt->r.currentOrigin, origin );
					SnapVector( origin );
					VectorAdd( origin, traceEnt->r.mins, mins );
					VectorAdd( origin, traceEnt->r.maxs, maxs );
					num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

					//bani - eh, why was this commented out? it makes sense, and prevents a sploit.
					if (dynamiteDropTeam == ent->client->sess.sessionTeam)
						return;

					for ( i=0 ; i<num ; i++ ) {
						hit = &g_entities[touch[i]];

						if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
							continue;
						}
						if ((hit->s.eType == ET_OID_TRIGGER)) {

							if ( !(hit->spawnflags & (AXIS_OBJECTIVE | ALLIED_OBJECTIVE)) )
								continue;

							// rain - spawnflags 128 = disabled (#309)
							if (hit->spawnflags & 128)
								continue;

							//bani - prevent plant/defuse exploit near a/h cabinets or non-destroyable locations (bank doors on goldrush)
							if( !hit->target_ent || hit->target_ent->s.eType != ET_EXPLOSIVE ) {
								continue;
							}

							if (ent->client->sess.sessionTeam == TEAM_AXIS) {
								if ((hit->spawnflags & AXIS_OBJECTIVE) && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++;
								}
								if(hit->target_ent) {
									G_Script_ScriptEvent( hit->target_ent, "defused", "" );
								}

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

								//bani
								defusedObj = qtrue;
							} else { // TEAM_ALLIES
								if ((hit->spawnflags & ALLIED_OBJECTIVE) && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++; 
									hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed
								}
								if(hit->target_ent) {
									G_Script_ScriptEvent( hit->target_ent, "defused", "" );
								}

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

								//bani
								defusedObj = qtrue;
							}
						}
					}
//bani - prevent multiple messages here
					if( defusedObj )
						return;

//bani - reordered this check so its AFTER the primary obj check
					// Gordon - first see if the dynamite was planted near a constructable object that would have been destroyed
					{
						int		entityList[MAX_GENTITIES];
						int		numListedEntities;
						int		e;
						vec3_t  org;

						VectorCopy( traceEnt->r.currentOrigin, org );
						org[2] += 4;	// move out of ground

						numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );

						for( e = 0; e < numListedEntities; e++ ) {
							hit = &g_entities[entityList[ e ]];

							if( hit->s.eType != ET_CONSTRUCTIBLE )
								continue;

							// invulnerable
							if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE )
								continue;

							// not dynamite-able
							if( hit->constructibleStats.weaponclass < 1 ) {
								continue;
							}

							// we got somthing to destroy
							if (ent->client->sess.sessionTeam == TEAM_AXIS) {
								if ( hit->s.teamNum == TEAM_AXIS && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									if(ent && ent->client) G_LogPrintf("Dynamite_Diffuse: %d\n", ent - g_entities);	// OSP
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++;
								}
								G_Script_ScriptEvent( hit, "defused", "" );

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->parent->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

							} else { // TEAM_ALLIES
								if ( hit->s.teamNum == TEAM_ALLIES && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									if(ent && ent->client) G_LogPrintf("Dynamite_Diffuse: %d\n", ent - g_entities);	// OSP
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DEBUG_ADD_SKILL_POINTS( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++; 
								}
								G_Script_ScriptEvent( hit, "defused", "" );

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->parent->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

							}

							return;
						}
					}
				}
	// jpw
			}
		}
	}
}


// JPW NERVE -- launch airstrike as line of bombs mostly-perpendicular to line of grenade travel
// (close air support should *always* drop parallel to friendly lines, tho accidents do happen)
extern void G_ExplodeMissile( gentity_t *ent );

void G_AirStrikeExplode( gentity_t *self ) {

	self->r.svFlags &= ~SVF_NOCLIENT;
	self->r.svFlags |= SVF_BROADCAST;

	self->think = G_ExplodeMissile;
	self->nextthink = level.time + 50;
}

qboolean G_AvailableAirstrikes( gentity_t* ent ) {
	if( ent->client->sess.sessionTeam == TEAM_AXIS ) {
		// forty - arty/airstrike rate limiting.
		if( level.axisBombCounter > 0 ) {
			return qfalse;
		}
	} else {
		// forty - arty/airstrike rate limiting.
		if( level.alliedBombCounter > 0 ) {
			return qfalse;
		}
	}

	return qtrue;
}

// forty - arty/airstrike rate limiting.
qboolean G_AvailableArty( gentity_t* ent ) {
	if( ent->client->sess.sessionTeam == TEAM_AXIS ) {
		if( level.axisArtyCounter > 0 ) {
			return qfalse;
		}
	} else {
		if( level.alliedArtyCounter > 0 ) {
			return qfalse;
		}
	}

	return qtrue;
}

void G_AddAirstrikeToCounters( gentity_t* ent ) {
	if( ent->client->sess.sessionTeam == TEAM_AXIS ) {
		level.axisBombCounter += team_airstrikeTime.integer * 1000;
	} else {
		level.alliedBombCounter += team_airstrikeTime.integer * 1000;
	}
}

// forty - arty/airstrike rate limiting.
void G_AddArtyToCounters( gentity_t* ent ) {

	if( ent->client->sess.sessionTeam == TEAM_AXIS ) {
		level.axisArtyCounter += team_artyTime.integer * 1000;
	} else {
		level.alliedArtyCounter +=  team_artyTime.integer * 1000;
	}
}

#define NUMBOMBS 10
#define BOMBSPREAD 150
extern void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize );
extern void G_HQSay( gentity_t *other, int color, const char *name, const char *message );

void weapon_checkAirStrikeThink1( gentity_t *ent ) {
	if( !weapon_checkAirStrike( ent ) ) {
		ent->think = G_ExplodeMissile;
		ent->nextthink = level.time + 1000;
		return;
	}

	ent->think = weapon_callAirStrike;
	ent->nextthink = level.time + 1500;
}

void weapon_checkAirStrikeThink2( gentity_t *ent ) {
	if( !weapon_checkAirStrike( ent ) ) {
		ent->think = G_ExplodeMissile;
		ent->nextthink = level.time + 1000;
		return;
	}

	ent->think = weapon_callSecondPlane;
	ent->nextthink = level.time + 500;
}

void weapon_callSecondPlane( gentity_t *ent ) {
	gentity_t* te;
	
	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
	te->s.eventParm = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane.wav" );
	te->r.svFlags |= SVF_BROADCAST;

	ent->nextthink = level.time + 1000;
	ent->think = weapon_callAirStrike;
}

qboolean weapon_checkAirStrike( gentity_t *ent ) 
{
	if( ent->s.teamNum == TEAM_AXIS ) {
		level.numActiveAirstrikes[0]++;
	} else {
		level.numActiveAirstrikes[1]++;
	}

	// cancel the airstrike if FF off and player joined spec
	// FIXME: this is a stupid workaround. Just store the parent team in the enitity itself and use that - no need to look up the parent
	if ( !G_FRIENDLYFIRE && ent->parent->client && ent->parent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		ent->splashDamage	= 0;	// no damage
		ent->think			= G_ExplodeMissile;
		ent->nextthink		= level.time + crandom()*50;		
		ent->active			= qfalse;

		if( ent->s.teamNum == TEAM_AXIS )
		{
			level.numActiveAirstrikes[0]--;
		} 
		else 
		{
			level.numActiveAirstrikes[1]--; 
		}
		return qfalse; // do nothing, don't hurt anyone 
	}

	if( ent->s.teamNum == TEAM_AXIS ) {
		if( level.numActiveAirstrikes[0] > 6 || !G_AvailableAirstrikes( ent->parent ) ) {
			G_HQSay( ent->parent, COLOR_YELLOW, "HQ: ", "All available planes are already en-route." );

			G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 0, ent->parent-g_entities );

			ent->active = qfalse;
			if( ent->s.teamNum == TEAM_AXIS ) {
				level.numActiveAirstrikes[0]--;
			} else {
				level.numActiveAirstrikes[1]--;
			}
			
			//restore charge if g_weapons set with bitmask
			//code contributed by Azyu of CSCWMod
			if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
				jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
			} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
				jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
			}
			
			return qfalse;
		}
	} else {
		if( level.numActiveAirstrikes[1] > 6 || !G_AvailableAirstrikes( ent->parent ) ) {
			G_HQSay( ent->parent, COLOR_YELLOW, "HQ: ", "All available planes are already en-route." );

			G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 0, ent->parent-g_entities );

			ent->active = qfalse;
			if( ent->s.teamNum == TEAM_AXIS ) {
				level.numActiveAirstrikes[0]--;
			} else {
				level.numActiveAirstrikes[1]--;
			}

			//restore charge if g_weapons set with bitmask
			//code contributed by Azyu of CSCWMod
			if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
				jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
			} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
				jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
			}

			return qfalse;
		}
	}

	return qtrue;
}

void weapon_callAirStrike( gentity_t *ent ) {
	int i, j, num, touch[MAX_GENTITIES];  
	vec3_t bombaxis, lookaxis, pos, bomboffset, fallaxis, temp, dir, skypoint, origin, mins, maxs;  
	gentity_t *bomb, *traceEnt;  
	trace_t	tr;
	float traceheight, bottomtraceheight;

	VectorCopy( ent->s.pos.trBase,bomboffset );
	bomboffset[2] += 4096.f;

	// turn off smoke grenade
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 950 + NUMBOMBS*100 + crandom()*50; // 950 offset is for aircraft flyby

	ent->active = qtrue;

	G_AddAirstrikeToCounters( ent->parent );

	{
		gentity_t* te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane.wav");
		te->r.svFlags |= SVF_BROADCAST;
	}

	// forty - easy airstrike blocking
        // forty - #208 - can always obscure field ops canisters  
	if( ent->parent->client->sess.skillBits[SK_SIGNALS] & (1<<3) &&   
		(g_asblock.integer & ASBLOCK_LVL3_FDOPS_NOBLOCK) ) {  
  	} else if( g_asblock.integer & ASBLOCK_EASY ) {  

		VectorCopy( ent->r.currentOrigin, origin );
		SnapVector( origin );
		VectorAdd( origin, tv(-8.f,-8.f,-8.f), mins );
		VectorAdd( origin, tv(8.f,8.f,8.f), maxs );

		if(g_debugBullets.integer >= 2) {
			gentity_t *tent;
			vec3_t b1, b2;

			G_Printf("mins: %f, %f, %f\n", mins[0], mins[1], mins[2] );
            G_Printf("maxs: %f, %f, %f\n", maxs[0], maxs[1], maxs[2] );

            VectorCopy(ent->r.currentOrigin, b1);
            VectorCopy(ent->r.currentOrigin, b2);

            VectorAdd(b1, tv(-8.f,-8.f,-8.f), b1);
            VectorAdd(b2, tv(8.f,8.f,8.f), b2);

            tent = G_TempEntity( b1, EV_RAILTRAIL );
            VectorCopy(b2, tent->s.origin2);
            tent->s.dmgFlags = 1;
		}

		num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );;
		for(i=0; i<num; i++) {

			traceEnt = &g_entities[touch[i]];

			if(traceEnt->client) {
		
				G_HQSay(  ent->parent, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target." );

				if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
					jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
				} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
					jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
				}

				// forty - airstrike blocking annoucement
				if(g_asblock.integer & ASBLOCK_ANNOUNCE) {
					AP(
						va(
							"cpm \"%s ^7blocked %s's ^7airstrike!\"", 
							traceEnt->client->pers.netname,
							ent->parent->client->pers.netname
						)
					);
				}

				G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 1, ent->parent-g_entities );

				if( ent->s.teamNum == TEAM_AXIS ) {
					level.numActiveAirstrikes[0]--;
				} else {
					level.numActiveAirstrikes[1]--;
				}
				ent->active = qfalse;
				return;
			}

		}

	}	
	
	// forty - #208 - can always obscure field ops canisters  
	if( ent->parent->client->sess.skillBits[SK_SIGNALS] & (1<<3) &&   
		(g_asblock.integer & ASBLOCK_LVL3_FDOPS_NOBLOCK)  
		) {  
		// forty - use trap engineer trace as it will ignore players and corpses.  
		trap_EngineerTrace( &tr, ent->s.pos.trBase, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );  
	} else {  
		// forty - allow stock air strike blocking.  
		trap_Trace( &tr, ent->s.pos.trBase, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );  
	}  

	if ((tr.fraction < 1.0) && (!(tr.surfaceFlags & SURF_NOIMPACT)) ) { //SURF_SKY)) ) { // JPW NERVE changed for trenchtoast foggie prollem
		G_HQSay( ent->parent, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target." );
		
		if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
			jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
		} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
			jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
		}

		// forty - airstrike blocking annoucement
		traceEnt = &g_entities[tr.entityNum];

		if(traceEnt->client && (g_asblock.integer & ASBLOCK_ANNOUNCE) ) {
			AP(
				va(
					"cpm \"%s ^7blocked %s's ^7airstrike!\"", 
					traceEnt->client->pers.netname,
					ent->parent->client->pers.netname
				)
			);
		}

		G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 1, ent->parent-g_entities );

		if( ent->s.teamNum == TEAM_AXIS ) {
			level.numActiveAirstrikes[0]--;
		} else {
			level.numActiveAirstrikes[1]--;
		}
		ent->active = qfalse;
		return;
	}



	trap_Trace( &tr, ent->s.pos.trBase, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );
	if ((tr.fraction < 1.0) && (!(tr.surfaceFlags & SURF_NOIMPACT)) ) { //SURF_SKY)) ) { // JPW NERVE changed for trenchtoast foggie prollem
		G_HQSay( ent->parent, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target." );

		if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
			jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
		} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
			jP_UnDoChargeCost(ent->parent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Airstrike, ent->parent->client->sess.skillBits, SK_FIELDOPS));
		}

		G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 1, ent->parent-g_entities );

		if( ent->s.teamNum == TEAM_AXIS ) {
			level.numActiveAirstrikes[0]--;
		} else {
			level.numActiveAirstrikes[1]--;
		}
		ent->active = qfalse;
		return;
	}

	G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 2, ent->parent-g_entities );


	VectorCopy( tr.endpos, bomboffset );
	VectorCopy( tr.endpos, skypoint );	
	traceheight = bomboffset[2];
	bottomtraceheight = traceheight - 8192.f;

	VectorSubtract( ent->s.pos.trBase, ent->parent->client->ps.origin, lookaxis );
	lookaxis[2] = 0;
	VectorNormalize( lookaxis );

	dir[0] = 0;
	dir[1] = 0;
	dir[2] = crandom(); // generate either up or down vector
	VectorNormalize( dir ); // which adds randomness to pass direction below

	for( j = 0; j < ent->count; j++ ) {		
		RotatePointAroundVector( bombaxis, dir, lookaxis, 90 + crandom() * 30 ); // munge the axis line a bit so it's not totally perpendicular
		VectorNormalize( bombaxis );

		VectorCopy( bombaxis, pos );
		VectorScale( pos,(float)(-.5f * BOMBSPREAD * NUMBOMBS ), pos );
		VectorAdd( ent->s.pos.trBase, pos, pos ); // first bomb position
		VectorScale( bombaxis, BOMBSPREAD, bombaxis ); // bomb drop direction offset

		for( i = 0; i < NUMBOMBS; i++ ) {
			bomb = G_Spawn();
			bomb->nextthink		= level.time + i * 100 + crandom() * 50 + 1000 + ( j * 2000 ); // 1000 for aircraft flyby, other term for tumble stagger
			bomb->think			= G_AirStrikeExplode;
			bomb->s.eType		= ET_MISSILE;
			bomb->r.svFlags		= SVF_NOCLIENT;
			bomb->s.weapon		= WP_SMOKE_MARKER; // might wanna change this
			bomb->r.ownerNum	= ent->s.number;
			bomb->parent		= ent->parent;
			bomb->s.teamNum		= ent->s.teamNum;
			bomb->damage		= 400; // maybe should un-hard-code these?
			bomb->splashDamage  = 400;

			// Gordon: for explosion type
			bomb->accuracy				= 2;
			bomb->classname				= "air strike";
			bomb->splashRadius			= 400;
			bomb->methodOfDeath			= MOD_AIRSTRIKE;
			bomb->splashMethodOfDeath	= MOD_AIRSTRIKE;
			bomb->clipmask		= MASK_MISSILESHOT;
			bomb->s.pos.trType	= TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			//bomb->s.pos.trTime = level.time;		// move a bit on the very first frame
			bomboffset[0]		= crandom() * .5f * BOMBSPREAD;
			bomboffset[1]		= crandom() * .5f * BOMBSPREAD;
			bomboffset[2]		= 0.f;
			VectorAdd( pos, bomboffset, bomb->s.pos.trBase );

			VectorCopy( bomb->s.pos.trBase, bomboffset ); // make sure bombs fall "on top of" nonuniform scenery
			bomboffset[2]		= traceheight;

			VectorCopy( bomboffset, fallaxis );
			fallaxis[2]			= bottomtraceheight;


			trap_Trace( &tr, bomboffset, NULL, NULL, fallaxis, ent-g_entities, bomb->clipmask );
			if( tr.fraction != 1.0 ) {
				VectorCopy(tr.endpos,bomb->s.pos.trBase);

				// Snap origin!
				VectorMA( bomb->s.pos.trBase, 2.f, tr.plane.normal, temp );
				SnapVectorTowards( bomb->s.pos.trBase, temp );			// save net bandwidth

//				G_RailTrail( skypoint, bomb->s.pos.trBase );
				trap_TraceNoEnts( &tr, skypoint, NULL, NULL, bomb->s.pos.trBase, 0, CONTENTS_SOLID );
				if( tr.fraction < 1.f ) {
					G_FreeEntity( bomb );

					// move pos for next bomb
					VectorAdd( pos, bombaxis, pos );

					continue;
				}
			}

			VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

			// move pos for next bomb
			VectorAdd( pos, bombaxis, pos );
		}
	}
}

// JPW NERVE -- sound effect for spotter round, had to do this as half-second bomb warning

void artilleryThink_real( gentity_t *ent )
{
	ent->freeAfterEvent = qtrue;

	trap_LinkEntity(ent);

	{
		int sfx = rand()%3;

		switch( sfx )
		{
			case 0:	
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/weapons/artillery/artillery_fly_1.wav" )); 
				break;
			case 1:
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/weapons/artillery/artillery_fly_2.wav" )); 
				break;
			case 2:
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/weapons/artillery/artillery_fly_3.wav" )); 
				break;
		}
	}
}

void artilleryThink( gentity_t *ent )
{
	ent->think		= artilleryThink_real;
	ent->nextthink	= level.time + 100;

	ent->r.svFlags	= SVF_BROADCAST;
}

// JPW NERVE -- makes smoke disappear after a bit (just unregisters stuff)
void artilleryGoAway(gentity_t *ent)
{
	ent->freeAfterEvent = qtrue;
	trap_LinkEntity(ent);
}

// JPW NERVE -- generates some smoke debris
void artillerySpotterThink( gentity_t *ent )
{
	gentity_t *bomb;
	vec3_t tmpdir;
	int i;
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 1;
	SnapVector( ent->s.pos.trBase );

	for( i = 0; i < 7; i++ ) 
	{
		bomb = G_Spawn();
		bomb->s.eType			= ET_MISSILE;
		bomb->r.svFlags			= 0;
		bomb->r.ownerNum		= ent->s.number;
		bomb->parent			= ent;
		bomb->s.teamNum			= ent->s.teamNum;
		bomb->nextthink			= level.time + 1000 + random() * 300;
		bomb->classname			= "WP";				// WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
		bomb->damage			= 000;				// maybe should un-hard-code these?
		bomb->splashDamage  	= 000;
		bomb->splashRadius		= 000;
		bomb->s.weapon			= WP_SMOKETRAIL;
		bomb->think				= artilleryGoAway;
		bomb->s.eFlags			|= EF_BOUNCE;
		bomb->clipmask			= MASK_MISSILESHOT;
		bomb->s.pos.trType		= TR_GRAVITY;		// was TR_GRAVITY,  might wanna go back to this and drop from height
		bomb->s.pos.trTime		= level.time;		// move a bit on the very first frame

		bomb->s.otherEntityNum2	= ent->s.otherEntityNum2;
		VectorCopy( ent->s.pos.trBase, bomb->s.pos.trBase );

		tmpdir[0]				= crandom();
		tmpdir[1]				= crandom();
		tmpdir[2]				= 1;

		VectorNormalize( tmpdir );

		tmpdir[2]				= 1;				// extra up

		VectorScale( tmpdir, 500 + random() * 500, tmpdir );
		VectorCopy( tmpdir,bomb->s.pos.trDelta );
		SnapVector( bomb->s.pos.trDelta );			// save net bandwidth
		VectorCopy( ent->s.pos.trBase, bomb->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase, bomb->r.currentOrigin );
	}
}

void G_GlobalClientEvent( int event, int param, int client ) 
{
	gentity_t* tent 		= G_TempEntity( vec3_origin, event );

	tent->s.density 		= param;
	tent->r.singleClient	= client;
	tent->r.svFlags			= SVF_SINGLECLIENT | SVF_BROADCAST;
	// CHRUKER: b099 - Calling for a lot of artillery or airstrikes can result voice over spam 
	tent->s.effect1Time = 1; // don't buffer
}

/*
==================
Weapon_Artillery
==================
*/
void Weapon_Artillery(gentity_t *ent) 
{
	trace_t		trace;
	int			i, count;	
	vec3_t		muzzlePoint, end, bomboffset, pos, fallaxis;
	float		traceheight, bottomtraceheight;
	gentity_t	*bomb, *bomb2;

	if( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_FIELDOPS ) 
	{
		G_Printf("not a fieldops, you can't shoot this!\n");
		return;
	}

	


	if (!jP_DoChargeCost(ent, SK_SIGNALS, BG_GetFromTable(chargeCost_Artillery, ent->client->sess.skillBits, SK_SIGNALS)))
	{
		return;
	}

	if( !G_AvailableArty( ent ) ) 
	{
		G_HQSay( ent, COLOR_YELLOW, "Fire Mission: ", "Insufficient fire support." );
		ent->active = qfalse;

		G_GlobalClientEvent( EV_ARTYMESSAGE, 0, ent-g_entities );

		if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
			jP_UnDoChargeCost(ent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Artillery, ent->client->sess.skillBits, SK_FIELDOPS));
		} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
			jP_UnDoChargeCost(ent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Artillery, ent->client->sess.skillBits, SK_FIELDOPS));
		}

		return;
	}

	AngleVectors (ent->client->ps.viewangles, forward, right, up);

	CalcMuzzlePointForActivate ( ent, forward, right, up, muzzlePoint, qtrue );

	VectorMA (muzzlePoint, 8192, forward, end);
	trap_Trace (&trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT);

	if (trace.surfaceFlags & SURF_NOIMPACT)
	{
		return;
	}

	VectorCopy(trace.endpos,pos);
	VectorCopy(pos,bomboffset);
	bomboffset[2] += 4096;

	trap_Trace(&trace, pos, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT);
	if ((trace.fraction < 1.0) && (!(trace.surfaceFlags & SURF_NOIMPACT)) )	{	
		G_HQSay( ent, COLOR_YELLOW, "Fire Mission: ", "Aborting, can't see target." );

		G_GlobalClientEvent( EV_ARTYMESSAGE, 1, ent-g_entities );

		if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_FULL ) {
			jP_UnDoChargeCost(ent, SK_FIELDOPS, BG_GetFromTable(chargeCost_Artillery, ent->client->sess.skillBits, SK_FIELDOPS));
		} else if( g_weapons.integer & WPF_TM_AIRSTRIKE_RESTORE_HALF ) {
			jP_UnDoChargeCost(ent, SK_FIELDOPS, 0.5*BG_GetFromTable(chargeCost_Artillery, ent->client->sess.skillBits, SK_FIELDOPS));
		}


		return;
	}

	// forty - arty/airstrike rate limiting.
	G_AddArtyToCounters( ent );

	G_HQSay( ent, COLOR_YELLOW, "Fire Mission: ", "Firing for effect!" );

	G_GlobalClientEvent( EV_ARTYMESSAGE, 2, ent-g_entities );

	VectorCopy( trace.endpos, bomboffset );
	traceheight			= bomboffset[2];
	bottomtraceheight	= traceheight - 8192;


// "spotter" round (i == 0)
// i == 1->4 is regular explosives
	count = (int)BG_GetFromTable( artilleryBombs, ent->client->sess.skillBits, SK_SIGNALS );


	// jet Pilot - use array 
	for( i = 0; i < count; i++ ) 
	{
		bomb						= G_Spawn();
		bomb->think					= G_AirStrikeExplode;
		bomb->s.eType				= ET_MISSILE;
		bomb->r.svFlags				= SVF_NOCLIENT;
		bomb->s.weapon				= WP_ARTY; // might wanna change this
		bomb->r.ownerNum			= ent->s.number;
		bomb->s.clientNum			= ent->s.number;
		bomb->parent				= ent;
		bomb->s.teamNum				= ent->client->sess.sessionTeam;

		if (i == 0)
		{
			bomb->nextthink			= level.time + 5000;
			bomb->r.svFlags			= SVF_BROADCAST;
			bomb->classname			= "props_explosion"; // was "air strike"
			bomb->damage			= 0; // maybe should un-hard-code these?
			bomb->splashDamage  	= 90;
			bomb->splashRadius		= 50;
			bomb->count				= 7;
			bomb->count2			= 1000;
			bomb->delay				= 300;
			bomb->s.otherEntityNum2 = 1;	// first bomb
			bomb->think				= artillerySpotterThink;
		} 
		else
		{
			bomb->nextthink			= level.time + 8950 + 2000 * i + crandom() * 800;
			// Gordon: for explosion type
			bomb->accuracy			= 2;
			bomb->classname			= "air strike";
			bomb->damage			= 0;
			bomb->splashDamage  	= 400;
			bomb->splashRadius		= 400;
		}
		bomb->methodOfDeath			= MOD_ARTY;
		bomb->splashMethodOfDeath	= MOD_ARTY;
		bomb->clipmask				= MASK_MISSILESHOT;
		bomb->s.pos.trType			= TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
		bomb->s.pos.trTime			= level.time;		// move a bit on the very first frame

		if( i ) 
		{	// spotter round is always dead on (OK, unrealistic but more fun)
			bomboffset[0] = crandom()*250;
			bomboffset[1] = crandom()*250;
		} 
		else
		{
			bomboffset[0] = crandom()*50; // was 0; changed per id request to prevent spotter round assassinations
			bomboffset[1] = crandom()*50; // was 0;
		}
		bomboffset[2] = 0;
		VectorAdd(pos,bomboffset, bomb->s.pos.trBase);

		VectorCopy(bomb->s.pos.trBase, bomboffset); // make sure bombs fall "on top of" nonuniform scenery
		bomboffset[2] = traceheight;

		VectorCopy(bomboffset, fallaxis);
		fallaxis[2] = bottomtraceheight;

		trap_Trace(&trace, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT);

		if (trace.fraction != 1.0)
			VectorCopy(trace.endpos,bomb->s.pos.trBase);	

		bomb->s.pos.trDelta[0] = 0; // might need to change this
		bomb->s.pos.trDelta[1] = 0;
		bomb->s.pos.trDelta[2] = 0;

		SnapVector( bomb->s.pos.trDelta );			// save net bandwidth
		VectorCopy (bomb->s.pos.trBase, bomb->r.currentOrigin);

// build arty falling sound effect in front of bomb drop
		bomb2				= G_Spawn();
		bomb2->think		= artilleryThink;
		bomb2->s.eType		= ET_MISSILE;
		bomb2->r.svFlags	= SVF_NOCLIENT;
		bomb2->r.ownerNum	= ent->s.number;
		bomb2->parent		= ent;
		bomb2->s.teamNum	= ent->s.teamNum;
		bomb2->damage		= 0;
		bomb2->nextthink	= bomb->nextthink - 600;
		bomb2->classname	= "air strike";
		bomb2->clipmask		= MASK_MISSILESHOT;
		bomb2->s.pos.trType = TR_STATIONARY;	// was TR_GRAVITY,  might wanna go back to this and drop from height
		bomb2->s.pos.trTime = level.time;		// move a bit on the very first frame

		VectorCopy(bomb->s.pos.trBase,bomb2->s.pos.trBase);
		VectorCopy(bomb->s.pos.trDelta,bomb2->s.pos.trDelta);
		VectorCopy(bomb->s.pos.trBase,bomb2->r.currentOrigin);
	}

	// OSP -- weapon stats
#ifndef DEBUG_STATS
	if(g_gamestate.integer == GS_PLAYING)
#endif
		ent->client->sess.aWeaponStats[WS_ARTILLERY].atts++;

}


#define SMOKEBOMB_GROWTIME 1000
#define SMOKEBOMB_SMOKETIME 15000
#define SMOKEBOMB_POSTSMOKETIME 2000	
// xkan, 11/25/2002 - increases postsmoke time from 2000->32000, this way, the entity 
// is still around while the smoke is around, so we can check if it blocks bot's vision 
// Arnout: eeeeeh this is wrong. 32 seconds is way too long. Also - we shouldn't be
// rendering the grenade anymore after the smoke stops and definately not send it to the client
// xkan, 12/06/2002 - back to the old value 2000, now that it looks like smoke disappears more
// quickly

void weapon_smokeBombExplode( gentity_t *ent ) {
	int lived = 0;

	if( !ent->grenadeExplodeTime )
		ent->grenadeExplodeTime = level.time;

	lived = level.time - ent->grenadeExplodeTime;
	ent->nextthink = level.time + FRAMETIME;

	if( lived < SMOKEBOMB_GROWTIME ) {
		// Just been thrown, increase radius
		ent->s.effect1Time = 16 + lived * ((640.f-16.f)/(float)SMOKEBOMB_GROWTIME);
	} else if( lived < SMOKEBOMB_SMOKETIME + SMOKEBOMB_GROWTIME ) {
		// Smoking
		ent->s.effect1Time = 640;
	} else if( lived < SMOKEBOMB_SMOKETIME + SMOKEBOMB_GROWTIME + SMOKEBOMB_POSTSMOKETIME ) {
		// Dying out
		ent->s.effect1Time = -1;
	} else {
		// Poof and it's gone
		G_FreeEntity( ent );
	}
}

gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum, int flags );
// jpw

/*
======================================================================

MACHINEGUN

======================================================================
*/

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating 
into a wall.
======================
*/

// (SA) modified so it doesn't have trouble with negative locations (quadrant problems)
//			(this was causing some problems with bullet marks appearing since snapping
//			too far off the target surface causes the the distance between the transmitted impact
//			point and the actual hit surface larger than the mark radius.  (so nothing shows) )

void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int		i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
//			v[i] = (int)v[i];
			v[i] = floor(v[i]);
		} else {
//			v[i] = (int)v[i] + 1;
			v[i] = ceil(v[i]);
		}
	}
}

// JPW
// mechanism allows different weapon damage for single/multiplayer; we want "balanced" weapons
// in multiplayer but don't want to alter the existing single-player damage items that have already
// been changed
//
// KLUDGE/FIXME: also modded #defines below to become macros that call this fn for minimal impact elsewhere
//
int G_GetWeaponDamage( int weapon ) 
{
// jaquboss, specifed in weapon file
	if ( BG_Weapons[weapon].damage )
		return BG_Weapons[weapon].damage;

	switch (weapon) 
	{
		default:
			return 1;
		// knives
		case WP_KNIFE: 
		case WP_KNIFE_KABAR:
			return 10;
		// pistols
		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT: 
			return 18;
		// Submachineguns
		case WP_STEN: 
		case WP_MP34:		//Meyer - wtf who put it at 16... all this time O_o
			return 14;
		case WP_STEN_MKII:
		case WP_THOMPSON: 
		case WP_MP40: 
			return 18;
		// Rifles
		case WP_CARBINE:
		case WP_GARAND:
		case WP_KAR98:
		case WP_K43:
			return 34;
		case WP_FG42: 
			return 15;
		case WP_BAR:
		case WP_BAR_SET:
		case WP_STG44:
			return 19;			
        // Machineguns		
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42: 
		case WP_MOBILE_MG42_SET:
			return 18;
		// Scoped Weapons
		case WP_FG42_SCOPE: 
			return 30;
		case WP_GARAND_SCOPE: 
		case WP_K43_SCOPE: 
			return 50;
		// Explosives
		case WP_SMOKE_MARKER: 
			return 140; 
//		case WP_MAPMORTAR: 
		case WP_GRENADE_LAUNCHER: 
		case WP_GRENADE_PINEAPPLE: 
		case WP_GPG40:
		case WP_M7: 
		case WP_LANDMINE: 
		case WP_SATCHEL:
			return 250;
		case WP_TRIPMINE: 
			return 300;
		case WP_PANZERFAUST: 
		case WP_MORTAR2_SET: 
		case WP_MORTAR_SET: 
		case WP_DYNAMITE: 
			return 400;
		case WP_BAZOOKA:
			return 300;
		case WP_SHOTGUN:			
			return 10;
		/*case WP_FIREBOLT:			
			return 40;*/
		case WP_FLAMETHROWER:	// jaquboss - added
			return 5;
		case WP_VENOM:
			return 20;
	}
}

extern char *nq_wpNames[];

float G_GetWeaponSpread( int weapon ) 
{
// do we have this in weapon file?
	if ( BG_Weapons[weapon].spread )
		return BG_Weapons[weapon].spread;

	switch (weapon) 
	{
		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
			return 600;
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
			return 600;
		case WP_MP40:
		case WP_THOMPSON:		
		case WP_BAR_SET:
			return 400;
		case WP_STG44:
		case WP_FG42:
			return 500;
		case WP_STEN:
		case WP_STEN_MKII:
			return 200;
		case WP_MP34:
			return 300;
		case WP_FG42_SCOPE:
			return 200;
		case WP_BAR:
			return 600;
		case WP_GARAND:
		case WP_CARBINE:
		case WP_KAR98:
		case WP_K43:
			return 250;
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			return 700;
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
			return 2500;
		case WP_SHOTGUN:
			return 1000;
		case WP_VENOM: 
			return 1000;
	// jaquboss added spread support to rocket launchers
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_BAZOOKA:
		case WP_PANZERFAUST:
			return 0;
	}

	G_Printf( "^1ERROR : ^7G_GetWeaponSpread ^9no case for weapon %d (%s)\n", 
		weapon, 
		( weapon < WP_NUM_WEAPONS && weapon > WP_NONE ) ? nq_wpNames[weapon] : "^1invalid" 
		);
	// jpw
	return 0;	// shouldn't get here
}

#define FLAMETHROWER_DAMAGE			G_GetWeaponDamage(WP_FLAMETHROWER) // JPW
#define LUGER_SPREAD				G_GetWeaponSpread(WP_LUGER)
#define LUGER_DAMAGE				G_GetWeaponDamage(WP_LUGER) // JPW
#define SILENCER_DAMAGE				G_GetWeaponDamage(WP_SILENCER)
#define SILENCER_SPREAD				G_GetWeaponSpread(WP_SILENCER)
#define AKIMBO_LUGER_DAMAGE			G_GetWeaponDamage(WP_AKIMBO_LUGER)
#define AKIMBO_LUGER_SPREAD			G_GetWeaponSpread(WP_AKIMBO_LUGER)
#define AKIMBO_SILENCEDLUGER_DAMAGE	G_GetWeaponDamage(WP_AKIMBO_SILENCEDLUGER)
#define AKIMBO_SILENCEDLUGER_SPREAD	G_GetWeaponSpread(WP_AKIMBO_SILENCEDLUGER)
#define COLT_SPREAD					G_GetWeaponSpread(WP_COLT)
#define	COLT_DAMAGE					G_GetWeaponDamage(WP_COLT) // JPW
#define SILENCED_COLT_DAMAGE		G_GetWeaponDamage(WP_SILENCED_COLT)
#define SILENCED_COLT_SPREAD		G_GetWeaponSpread(WP_SILENCED_COLT)
#define AKIMBO_COLT_DAMAGE			G_GetWeaponDamage(WP_AKIMBO_COLT)
#define AKIMBO_COLT_SPREAD			G_GetWeaponSpread(WP_AKIMBO_COLT)
#define AKIMBO_SILENCEDCOLT_DAMAGE	G_GetWeaponDamage(WP_AKIMBO_SILENCEDCOLT)
#define AKIMBO_SILENCEDCOLT_SPREAD	G_GetWeaponSpread(WP_AKIMBO_SILENCEDCOLT)
#define MP40_SPREAD					G_GetWeaponSpread(WP_MP40)
#define	MP40_DAMAGE					G_GetWeaponDamage(WP_MP40) // JPW
#define THOMPSON_SPREAD				G_GetWeaponSpread(WP_THOMPSON)
#define	THOMPSON_DAMAGE				G_GetWeaponDamage(WP_THOMPSON) // JPW
#define STEN_SPREAD					G_GetWeaponSpread(WP_STEN)
#define	STEN_DAMAGE					G_GetWeaponDamage(WP_STEN) // JPW
#define GARAND_SPREAD				G_GetWeaponSpread(WP_GARAND)
#define	GARAND_DAMAGE				G_GetWeaponDamage(WP_GARAND) // JPW
#define KAR98_SPREAD				G_GetWeaponSpread(WP_KAR98)
#define	KAR98_DAMAGE				G_GetWeaponDamage(WP_KAR98)
#define CARBINE_SPREAD				G_GetWeaponSpread(WP_CARBINE)
#define	CARBINE_DAMAGE				G_GetWeaponDamage(WP_CARBINE)
#define	KAR98_GREN_DAMAGE			G_GetWeaponDamage(WP_GREN_KAR98)
#define MOBILE_MG42_SPREAD			G_GetWeaponSpread(WP_MOBILE_MG42)
#define	MOBILE_MG42_DAMAGE			G_GetWeaponDamage(WP_MOBILE_MG42)
#define FG42_SPREAD					G_GetWeaponSpread(WP_FG42)
#define	FG42_DAMAGE					G_GetWeaponDamage(WP_FG42) // JPW
#define FG42SCOPE_SPREAD			G_GetWeaponSpread(WP_FG42_SCOPE)
#define	FG42SCOPE_DAMAGE			G_GetWeaponDamage(WP_FG42_SCOPE) // JPW
#define K43_SPREAD					G_GetWeaponSpread(WP_K43)
#define	K43_DAMAGE					G_GetWeaponDamage(WP_K43)
#define GARANDSCOPE_SPREAD			G_GetWeaponSpread(WP_GARAND_SCOPE)
#define GARANDSCOPE_DAMAGE			G_GetWeaponDamage(WP_GARAND_SCOPE)
#define K43SCOPE_SPREAD				G_GetWeaponSpread(WP_K43_SCOPE)
#define K43SCOPE_DAMAGE 			G_GetWeaponDamage(WP_K43_SCOPE)
// jet Pilot - define new weapon spread/etc
#define SHOTGUN_SPREAD  			G_GetWeaponSpread(WP_SHOTGUN)
#define SHOTGUN_DAMAGE  			G_GetWeaponDamage(WP_SHOTGUN)
#define MOBILE_BROWNING_SPREAD		G_GetWeaponSpread(WP_MOBILE_BROWNING)
#define MOBILE_BROWNING_DAMAGE		G_GetWeaponDamage(WP_MOBILE_BROWNING)
#define BAR_SPREAD					G_GetWeaponSpread(WP_BAR)
#define BAR_DAMAGE					G_GetWeaponDamage(WP_BAR)
#define STG44_SPREAD				G_GetWeaponSpread(WP_STG44)
#define STG44_DAMAGE				G_GetWeaponDamage(WP_STG44)
#define STENMKII_SPREAD				G_GetWeaponSpread(WP_STEN_MKII)
#define	STENMKII_DAMAGE				G_GetWeaponDamage(WP_STEN_MKII) // JPW
#define MP34_SPREAD					G_GetWeaponSpread(WP_MP34)
#define	MP34_DAMAGE					G_GetWeaponDamage(WP_MP34) 

#define VENOM_SPREAD				G_GetWeaponSpread(WP_VENOM)
#define	VENOM_DAMAGE				G_GetWeaponDamage(WP_VENOM) 

void RubbleFlagCheck (gentity_t *ent, trace_t tr)
{
	qboolean	is_valid = qfalse;
	int			type = 0;

	// (SA) moving client-side
	return;

	if (tr.surfaceFlags & SURF_RUBBLE || tr.surfaceFlags & SURF_GRAVEL)
	{
		is_valid = qtrue;
		type = 4;
	}
	else if (tr.surfaceFlags & SURF_METAL)
	{
//----(SA)	removed
//		is_valid = qtrue;
//		type = 2;
	}
	else if (tr.surfaceFlags & SURF_WOOD)
	{
		is_valid = qtrue;
		type = 1;
	}
	
	if (is_valid && ent->client && ( ent->client->ps.persistant[PERS_HWEAPON_USE] ) )
	{
		if (rand()%100 > 75)
		{
			gentity_t	*sfx;
			vec3_t		start;
			vec3_t		dir;

			sfx = G_Spawn ();

			sfx->s.density = type; 

			VectorCopy (tr.endpos, start);

			VectorCopy (muzzleTrace, dir);
			VectorNegate (dir, dir);

			G_SetOrigin (sfx, start); 
			G_SetAngle (sfx, dir);

			G_AddEvent( sfx, EV_SHARD, DirToByte( dir ));

			sfx->think = G_FreeEntity;
			sfx->nextthink = level.time + 1000;

			sfx->s.frame = 3 + (rand()%3) ;
								
			trap_LinkEntity (sfx);

		}
	}
}

/*
==============
EmitterCheck
	see if a new particle emitter should be created at the bullet impact point
==============
*/
void EmitterCheck(gentity_t *ent, gentity_t *attacker, trace_t *tr) {
	gentity_t *tent;
	vec3_t	origin;

	VectorCopy(tr->endpos, origin);
	SnapVectorTowards( tr->endpos, attacker->s.origin);

	if(Q_stricmp(ent->classname, "func_explosive") == 0) {
	} else if(Q_stricmp(ent->classname, "func_leaky") == 0) {


		tent = G_TempEntity (origin, EV_EMITTER);
		VectorCopy (origin, tent->s.origin);
		tent->s.time = 1234;
		tent->s.density = 9876;
		VectorCopy (tr->plane.normal, tent->s.origin2);

	}
}


/*
==============
Bullet_Endpos
	find target end position for bullet trace based on entities weapon and accuracy
==============
*/
void Bullet_Endpos(gentity_t *ent, float spread, vec3_t *end) {
	float		r, u;
	qboolean	randSpread = qtrue;
	int			dist = 8192;

	r = crandom()*spread;
	u = crandom()*spread;

	if(BG_IsScopedWeapon(ent->s.weapon)) {
		// aim dir already accounted for sway of scoped weapons in CalcMuzzlePoints()
		//dist*= 2;
		dist <<= 1;
		randSpread = qfalse;
	}

	VectorMA (muzzleTrace, dist, forward, *end);

	if(randSpread) {
		VectorMA (*end, r, right, *end);
		VectorMA (*end, u, up, *end);
	}
}

/*
==============
Bullet_Fire
==============
*/
void Bullet_Fire (gentity_t *ent, float spread, int damage, qboolean distance_falloff)
{
	vec3_t		end;

	// Gordon: skill thing should be here Arnout! 
	switch( ent->s.weapon ) 
	{
		// light weapons
		case WP_LUGER:
		case WP_COLT:
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
		// CHRUKER: b045 - Akimbo weapons also need spread reduction
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_AKIMBO_SILENCEDCOLT:
		// jet Pilot - New Weapons that recieve a spread reduction
		case WP_BAR:
		case WP_STG44:
		case WP_STEN_MKII:
		case WP_SHOTGUN:			
			if( ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<3) )
				spread *= .65f;
			break;
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42_SET:
		case WP_BAR_SET:
			VectorMA(muzzleTrace, 48, forward, muzzleTrace);
			break;
	}

	if ( BG_Weapons[ent->s.weapon].falloff == WEAPON_YES )
		distance_falloff = qtrue;

	if ( BG_Weapons[ent->s.weapon].falloff == WEAPON_NO )
		distance_falloff = qfalse;

	if ( BG_Weapons[ent->s.weapon].CrouchSpreadRatio && ent->client->ps.eFlags & EF_CROUCHING )
		spread *= BG_Weapons[ent->s.weapon].CrouchSpreadRatio;

	if ( BG_Weapons[ent->s.weapon].ProneSpreadRatio && ent->client->ps.eFlags & EF_PRONE )
		spread *= BG_Weapons[ent->s.weapon].ProneSpreadRatio;

	Bullet_Endpos(ent, spread, &end);

	G_HistoricalTraceBegin( ent );

	Bullet_Fire_Extended(ent, ent, muzzleTrace, end, spread, damage, distance_falloff, 0);

	G_HistoricalTraceEnd( ent );
}

qboolean jP_BulletReflects(weapon_t weapon)
{

	if ( g_bulletReflect.integer == 0 )
		return qfalse;

	if ( g_bulletReflect.integer == 2 )
		return qtrue;

	if ( BG_Weapons[weapon].bulletReflection )
		return qtrue;

	return qfalse;
}


/*
==============
Bullet_Fire_Extended
	A modified Bullet_Fire with more parameters.
	The original Bullet_Fire still passes through here and functions as it always has.

	uses for this include shooting through entities (windows, doors, other players, etc.) and reflecting bullets
==============
*/
qboolean Bullet_Fire_Extended(gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage, qboolean distance_falloff, int reflectNum) 
{
	trace_t		tr;
	gentity_t	*tent;
	gentity_t	*traceEnt;
	qboolean hitClient = qfalse;
	qboolean reducedDamage = qfalse;
	qboolean waslinked = qfalse;

	if ( reflectNum > 16 )
		return qfalse;

	//bani - prevent shooting ourselves in the head when prone, firing through a breakable
	if( g_entities[ attacker->s.number ].client && g_entities[ attacker->s.number ].r.linked == qtrue && reflectNum < 1 ) {
		g_entities[ attacker->s.number ].r.linked = qfalse;
		waslinked = qtrue;
	}

	// jet Pilot - allow us to shoot our own stuff if vulnerable rockets/nades is enabled
	if (g_missileHealth.integer > 0 || reflectNum > 0)
		G_Trace(source, &tr, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_SHOT, !G_WeaponCanGib( GetAmmoTableData(attacker->s.weapon)->mod ));
	else
        G_Trace(source, &tr, start, NULL, NULL, end, source->s.number, MASK_SHOT, !G_WeaponCanGib( GetAmmoTableData(attacker->s.weapon)->mod ));

	//bani - prevent shooting ourselves in the head when prone, firing through a breakable
	if( waslinked == qtrue && reflectNum < 1 )
		g_entities[ attacker->s.number ].r.linked = qtrue;

	// bullet debugging using Q3A's railtrail
	if(g_debugBullets.integer & 1) 
	{
	/*	tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy(tr.endpos, tent->s.origin2);
		tent->s.otherEntityNum2 = attacker->s.number;
		*/
		G_RailTrail( start, tr.endpos, tv(1.f,0.f,0.f));
	}

	traceEnt = &g_entities[ tr.entityNum ];
	EmitterCheck(traceEnt, attacker, &tr);
	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, start );

	// jet pilot - optionally disable falloff damage
	if( jp_falloff.integer && distance_falloff )
	{
		//vec_t dist;
		float dist;
		//vec3_t shotvec;
		float scale;

	/*	VectorSubtract( tr.endpos, muzzleTrace, shotvec );		
		dist = VectorLengthSquared( shotvec );*/

		dist = Distance(tr.endpos, start );

		// ~~~---______
		// zinx - start at 100% at 1500 units (and before),
		// and go to 50% at 2500 units (and after)

		// CHRUKER b022
		// 1500 to 2500 -> 0.0 to 1.0 
		scale = (dist - 1500.f) / (2500.f - 1500.f);
		// 0.0 to 1.0 -> 0.0 to 0.5
		scale *= 0.5f;
		// 0.0 to 0.5 -> 1.0 to 0.5
		scale = 1.0f - scale;

		// And, finally, cap it.
		reducedDamage = qtrue;
		if (scale >= 1.0f) { scale = 1.0f; reducedDamage = qfalse; }
		else if (scale < 0.5f) scale = 0.5f;

		damage *= scale;
	}

	
	// send bullet impact
	if ( traceEnt->takedamage && traceEnt->client )
	{
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;

		if ( reflectNum ) {
			SnapVector( start );
			VectorCopy( start, tent->s.origin2);
			tent->s.dmgFlags = 1;
		} else {
			VectorClear(tent->s.origin2);
			tent->s.dmgFlags = 0;
		} 

		if( AccuracyHit( traceEnt, attacker )) 
		{
			hitClient = qtrue;
		}

		if(g_debugBullets.integer >= 2) // show hit player bb
		{	
			/*gentity_t *bboxEnt;
			vec3_t b1, b2;*/
			vec3_t maxs;
			if (g_antilag.integer) 
			{
				// josh: Backwards reconcile the bbox.
				G_AdjustSingleClientPosition(traceEnt,attacker->client->pers.cmd.serverTime,attacker);
			}
			/*VectorCopy(traceEnt->r.currentOrigin, b1);
			VectorCopy(traceEnt->r.currentOrigin, b2);
			VectorAdd(b1, traceEnt->r.mins, b1);
			*/
			VectorCopy(traceEnt->r.maxs, maxs);
			maxs[2] = ClientHitboxMaxZ(traceEnt);
			/*
			VectorAdd(b2, maxs, b2);
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy(b2, bboxEnt->s.origin2);
			bboxEnt->s.dmgFlags = 1;	// ("type")*/
			G_RailBox( traceEnt->r.currentOrigin, traceEnt->r.mins, maxs, tv(0.f,0.f,1.f), traceEnt->s.number);
			if (g_antilag.integer) {
				// josh: restore after
				G_ReAdjustSingleClientPosition(traceEnt);
			}

		}
	// not a client that can take damage
	}
	else 
	{
		trace_t tr2;
		vec3_t	reflect, end2;
		float	dot;

		if(g_debugBullets.integer <= -2) {	// show hit thing bb
			/*gentity_t *bboxEnt;
			vec3_t b1, b2;
			VectorCopy(traceEnt->r.currentOrigin, b1);
			VectorCopy(traceEnt->r.currentOrigin, b2);
			VectorAdd(b1, traceEnt->r.mins, b1);
			VectorAdd(b2, traceEnt->r.maxs, b2);
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy(b2, bboxEnt->s.origin2);
			bboxEnt->s.dmgFlags = 1;	// ("type")*/
			G_RailBox( traceEnt->r.currentOrigin, traceEnt->r.mins, traceEnt->r.maxs, tv(0.f,1.f,0.f), traceEnt->s.number);
		}

		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );

		G_Trace(source, &tr2, start, NULL, NULL, end, source->s.number, MASK_WATER | MASK_SHOT, !G_WeaponCanGib( GetAmmoTableData(attacker->s.weapon)->mod ));

		// jaquboss make sure we hit water/slime surface 
		if(tr.entityNum != tr2.entityNum && tr2.fraction != 1 && (tr2.contents & CONTENTS_WATER || tr2.contents & CONTENTS_SLIME) ) {
			vec3_t v;

			VectorSubtract( tr.endpos, start, v );

			tent->s.modelindex2 = (8192 * tr2.fraction) / VectorLength( v );
		} else {
			tent->s.modelindex2 = 0;
		}

		dot = DotProduct( forward, tr.plane.normal );
		VectorMA( forward, -2*dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );

		tent->s.eventParm = DirToByte( reflect );
		tent->s.otherEntityNum2 = ENTITYNUM_NONE;

		if ( reflectNum ) {
			SnapVector( start );
			VectorCopy( start, tent->s.origin2);
			tent->s.dmgFlags = 1;
		} else {
			VectorClear(tent->s.origin2);
			tent->s.dmgFlags = 0;
		} 

		if ( jP_BulletReflects(attacker->s.weapon) ){
		// Ridah, bullet impact should reflect off surface

			VectorMA( tr.endpos, 8192, reflect, end2 );

			// jet Pilot - Allow some bullets to reflect off metal surfaces
			if ( tr.surfaceFlags & SURF_METAL && damage >= 2 )
			{
				reflectNum++;
				tent->s.otherEntityNum = attacker->s.number;
				return Bullet_Fire_Extended(attacker, attacker, tr.endpos, end2, 0, damage*0.5f, distance_falloff, reflectNum);
			}
		}
	}

	tent->s.otherEntityNum = attacker->s.number;

	if ( traceEnt->takedamage ) 
	{
		int mod=GetAmmoTableData(attacker->s.weapon)->mod;
		vec3_t dir;

		VectorSubtract( tr.endpos, start, dir );
		VectorNormalize( dir );

		G_Damage( traceEnt, attacker, attacker, dir, tr.endpos, damage, ( distance_falloff ? DAMAGE_DISTANCEFALLOFF : 0 ), mod );
		
		// allow bullets to "pass through" func_explosives if they break by taking another simultanious shot
		if( traceEnt->s.eType == ET_EXPLOSIVE ) 
		{
			if(traceEnt->health <= damage)
			{
				// start new bullet at position this hit the bmodel and continue to the end position (ignoring shot-through bmodel in next trace)
				// spread = 0 as this is an extension of an already spread shot
				return Bullet_Fire_Extended(traceEnt, attacker, tr.endpos, end, 0, damage, distance_falloff, reflectNum);
			}
		}
	}
	

	return hitClient;
}



/*
======================================================================

GRENADE LAUNCHER

  700 has been the standard direction multiplier in fire_grenade()

======================================================================
*/
extern void G_ExplodeMissilePoisonGas (gentity_t *ent);

gentity_t *weapon_gpg40_fire (gentity_t *ent, int grenType) {
	gentity_t	*m/*, *te*/; // JPW NERVE
	trace_t		tr;
	vec3_t		viewpos;
//	float		upangle = 0, pitch;			//	start with level throwing and adjust based on angle
	vec3_t		tosspos;
	//bani - to prevent nade-through-teamdoor sploit
	vec3_t 	orig_viewpos;

	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);

	VectorCopy(muzzleEffect, tosspos);

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( ent->s.pos.trBase, viewpos );
	viewpos[2] += ent->client->ps.viewheight;
	VectorCopy( viewpos, orig_viewpos );	//bani - to prevent nade-through-teamdoor sploit
	VectorMA( viewpos, 32, forward, viewpos);

	//bani - to prevent nade-through-teamdoor sploit
	trap_Trace( &tr, orig_viewpos, tv( -4.f, -4.f, 0.f ), tv( 4.f, 4.f, 6.f ), viewpos, ent->s.number, MASK_MISSILESHOT );
	if( tr.fraction < 1 ) { // oops, bad launch spot ) {
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, orig_viewpos );
	} else {
		trap_Trace (&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, ent->s.number, MASK_MISSILESHOT );
		if( tr.fraction < 1 ) { // oops, bad launch spot
			VectorCopy(tr.endpos, tosspos);
			SnapVectorTowards( tosspos, viewpos );
		}
	}

	VectorScale(forward, 2000, forward);

	m = fire_grenade (ent, tosspos, forward, grenType);

	m->damage = 0;
	
	// Ridah, return the grenade so we can do some prediction before deciding if we really want to throw it or not
	return m;
}

gentity_t *weapon_mortar_fire( gentity_t *ent, int grenType ) {
	gentity_t	*m;
	trace_t		tr;
	vec3_t		launchPos, testPos;
	vec3_t		angles;

	VectorCopy( ent->client->ps.viewangles, angles );
	angles[PITCH] -= 60.f;
/*	if( angles[PITCH] < -89.f )
		angles[PITCH] = -89.f;*/
	AngleVectors( angles, forward, NULL, NULL );

	VectorCopy( muzzleEffect, launchPos );

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorMA( launchPos, 32, forward, testPos);

	// Gordon: hack so i can do inverse trajectory calcs easily :p
	if(G_IsSinglePlayerGame() && ent->r.svFlags & SVF_BOT) {
/*		forward[0] *= 3000;
		forward[1] *= 3000;
		forward[2] *= 3000;*/
		VectorCopy( ent->gDelta, forward );
	} else {
		forward[0] *= 3000*1.1f;
		forward[1] *= 3000*1.1f;
		forward[2] *= 1500*1.1f;
	}

	trap_Trace (&tr, testPos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), launchPos, ent->s.number, MASK_MISSILESHOT);

	if( tr.fraction < 1 ) {	// oops, bad launch spot
		VectorCopy( tr.endpos, launchPos );
		SnapVectorTowards( launchPos, testPos );
	}

	m = fire_grenade( ent, launchPos, forward, grenType );

	return m;
}

gentity_t *weapon_grenadelauncher_fire (gentity_t *ent, int grenType) {
	gentity_t	*m;
	trace_t		tr;
	vec3_t		viewpos;
	float		upangle = 0, pitch;			//	start with level throwing and adjust based on angle
	vec3_t		tosspos;
	qboolean	underhand = qtrue;

	pitch = ent->s.apos.trBase[0];

	// JPW NERVE -- smoke grenades always overhand
	if( pitch >= 0 ) {
		forward[2] += 0.5f;
		// Used later in underhand boost
		pitch = 1.3f;
	}
	else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		forward[2] += (pitch * 0.5f);

		// Used later in underhand boost
		pitch *= 0.3f;
		pitch += 1.f;
	}

	VectorNormalizeFast( forward );			//	make sure forward is normalized

	upangle = -(ent->s.apos.trBase[0]);	//	this will give between	-90 / 90
	upangle = min(upangle, 50);
	upangle = max(upangle, -50);		//	now clamped to	-50 / 50	(don't allow firing straight up/down)
	upangle = upangle/100.0f;			//				   -0.5 / 0.5
	upangle += 0.5f;					//				    0.0 / 1.0

	if(upangle < .1)
		upangle = .1;

	// pineapples are not thrown as far as mashers // Gordon: um, no?
	if(grenType == WP_GRENADE_LAUNCHER)
		upangle *= 900;
	else if(grenType == WP_GRENADE_PINEAPPLE)
		upangle *= 900;
	else if (grenType == WP_SMOKE_MARKER)
		upangle *= 900;
	else if (grenType == WP_SMOKE_BOMB)
		upangle *= 900;
	else	// WP_DYNAMITE // Gordon: or WP_LANDMINE / WP_SATCHEL
		upangle *= 400;

	VectorCopy(muzzleEffect, tosspos);

	if(underhand) {
		// move a little bit more away from the player (so underhand tosses don't get caught on nearby lips)
		VectorMA(muzzleEffect, 8, forward, tosspos);
		tosspos[2] -= 8;	// lower origin for the underhand throw
		upangle *= pitch;
		SnapVector( tosspos );
	}

	VectorScale(forward, upangle, forward);

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( ent->s.pos.trBase, viewpos );
	viewpos[2] += ent->client->ps.viewheight;

	if( grenType == WP_DYNAMITE || grenType == WP_SATCHEL )
		trap_Trace (&tr, viewpos, tv(-12.f, -12.f, 0.f), tv(12.f, 12.f, 20.f), tosspos, ent->s.number, MASK_MISSILESHOT);
	else if( grenType == WP_LANDMINE )
		trap_Trace (&tr, viewpos, tv(-16.f, -16.f, 0.f), tv(16.f, 16.f, 16.f), tosspos, ent->s.number, MASK_MISSILESHOT);
	else
		trap_Trace (&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, ent->s.number, MASK_MISSILESHOT);

	if( tr.startsolid ) {
		// Arnout: this code is a bit more solid than the previous code
		VectorCopy( forward, viewpos );
		VectorNormalizeFast( viewpos );
		VectorMA( ent->r.currentOrigin, -24.f, viewpos, viewpos ); 

		if( grenType == WP_DYNAMITE || grenType == WP_SATCHEL )
			trap_Trace (&tr, viewpos, tv(-12.f, -12.f, 0.f), tv(12.f, 12.f, 20.f), tosspos, ent->s.number, MASK_MISSILESHOT);
		else if( grenType == WP_LANDMINE )
			trap_Trace (&tr, viewpos, tv(-16.f, -16.f, 0.f), tv(16.f, 16.f, 16.f), tosspos, ent->s.number, MASK_MISSILESHOT);
		else
			trap_Trace (&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, ent->s.number, MASK_MISSILESHOT);

		VectorCopy( tr.endpos, tosspos );
	} else if( tr.fraction < 1 ) {	// oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

	m = fire_grenade (ent, tosspos, forward, grenType);

	m->damage = 0;	// Ridah, grenade's don't explode on contact
	
	if (grenType == WP_LANDMINE) {
		if (ent->client->sess.sessionTeam == TEAM_AXIS) // store team so we can generate red or blue smoke
			m->s.otherEntityNum2 = 1;
		else
			m->s.otherEntityNum2 = 0;
	}

	// Arnout: override for smoke gren
	if( grenType == WP_SMOKE_BOMB ) {
		m->s.effect1Time = 16;
		m->think = weapon_smokeBombExplode;
	}

	// JPW NERVE
	if (grenType == WP_SMOKE_MARKER) {
		m->s.teamNum = ent->client->sess.sessionTeam;	// store team so we can generate red or blue smoke
		if( ent->client->sess.skillBits[SK_FIELDOPS] & (1<<3) ) {
			m->count = 2;
			m->nextthink = level.time + 3500;
			m->think = weapon_checkAirStrikeThink2;
		} else {
			m->count = 1;
			m->nextthink = level.time + 2500;
			m->think = weapon_checkAirStrikeThink1;
		}
	}
	// jpw

	//----(SA)	adjust for movement of character.  TODO: Probably comment in later, but only for forward/back not strafing
	//VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics

	// Ridah, return the grenade so we can do some prediction before deciding if we really want to throw it or not
	return m;
}

/*
======================================================================

ROCKET

======================================================================
*/

gentity_t *Weapon_Panzerfaust_Fire( gentity_t *ent, vec3_t dir ) {
	float		r, u;

	r = crandom()*(G_GetWeaponSpread(WP_PANZERFAUST)/1000);
	u = crandom()*(G_GetWeaponSpread(WP_PANZERFAUST)/1000);

	VectorScale(forward, 16, dir);
	VectorMA (dir, r, right, dir);
	VectorMA (dir, u, up, dir);
	VectorNormalize(dir);

	return fire_rocket (ent, muzzleEffect, dir, WP_PANZERFAUST);
}

gentity_t *Weapon_Bazooka_Fire( gentity_t *ent ) {
	vec3_t		dir;
	float		r, u;

	r = crandom()*(G_GetWeaponSpread(WP_BAZOOKA)/1000);
	u = crandom()*(G_GetWeaponSpread(WP_BAZOOKA)/1000);

	VectorScale(forward, 16, dir);
	VectorMA (dir, r, right, dir);
	VectorMA (dir, u, up, dir);
	VectorNormalize(dir);

	return  fire_rocket (ent, muzzleEffect, dir, WP_BAZOOKA);
}

void Use_Item( gentity_t *ent, gentity_t *other, gentity_t *activator );

void ThrowKnife( gentity_t *ent )
{
	gentity_t	*knife;
	float		speed;
	vec3_t		dir;
	float		r, u;

	CalcMuzzlePoints(ent, ent->s.weapon);

	// 'spread'
	r = crandom()*(G_GetWeaponSpread(ent->s.weapon)/1000);
	u = crandom()*(G_GetWeaponSpread(ent->s.weapon)/1000);
	VectorScale(forward, 16, dir);
	VectorMA (dir, r, right, dir);
	VectorMA (dir, u, up, dir);
	VectorNormalize(dir);

	// entity handling
	knife						= G_Spawn();
	knife->classname 			= "knife";
	knife->nextthink 			= level.time + 20000;
	knife->think				= G_FreeEntity;

	// misc
	knife->s.clientNum			= ent->client->ps.clientNum;
	knife->s.eType				= ET_ITEM;
	knife->s.weapon				= ent->s.weapon;						// Use the correct weapon in multiplayer
	knife->s.teamNum			= ent->client->sess.sessionTeam;
	knife->parent 				= ent;

	// usage
	knife->touch				= Touch_Item;	// no auto-pickup, only activate
	knife->use					= Use_Item;

	// damage
	knife->damage 				= G_GetWeaponDamage(ent->s.weapon); 	// JPW NERVE
	knife->splashDamage			= 0;
	knife->splashRadius			= 0;
	knife->methodOfDeath 		= MOD_THROWKNIFE;

	// clipping
	knife->clipmask 			= CONTENTS_SOLID|MASK_MISSILESHOT;
	knife->r.contents			= CONTENTS_TRIGGER|CONTENTS_ITEM;

	// angles/origin
	G_SetAngle( knife, ent->client->ps.viewangles );
	G_SetOrigin( knife, muzzleEffect);

	// trajectory
	knife->s.pos.trType 		= TR_GRAVITY_LOW;
	knife->s.pos.trTime 		= level.time - 50;	// move a bit on the very first frame

	// bouncing
	knife->physicsBounce		= 0.35;
	knife->physicsObject		= qtrue;

	// NQ physics
	knife->physicsSlide			= qfalse;
	knife->physicsFlush			= qtrue;

	// bounding box
	VectorSet( knife->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( knife->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2*ITEM_RADIUS );

	// speed / dir
	speed = ent->client->ps.grenadeTimeLeft*0.75f;

	// minimal toss speed
	if ( speed < 350.f )
		speed = 350.f;

	VectorScale( dir, speed, knife->s.pos.trDelta );	
	SnapVector( knife->s.pos.trDelta );				

	// rotation
	knife->s.apos.trTime = level.time - 50;
	knife->s.apos.trType = TR_LINEAR;
	VectorCopy( ent->client->ps.viewangles, knife->s.apos.trBase );
	knife->s.apos.trDelta[0] = speed*3;

	// item
	knife->item =  BG_FindItemForWeapon( ent->s.weapon );
	knife->s.modelindex = knife->item - bg_itemlist;	// store item number in modelindex
	knife->s.otherEntityNum2 = 1;	// DHM - Nerve :: this is taking modelindex2's place for a dropped item
	knife->flags |= FL_DROPPED_ITEM;	// so it gets removd after being picked up

	// add knife to game
	trap_LinkEntity (knife);

	// player himself
	ent->client->ps.grenadeTimeLeft = 0;
	ent->client->sess.aWeaponStats[WS_KNIFE].atts++;

}

/*
======================================================================

SPEARGUN

======================================================================
*/
/*void Weapon_Speargun_Fire (gentity_t *ent) {
	gentity_t	*m;

	m = fire_speargun (ent, muzzleEffect, forward);
}*/


/*
======================================================================

LIGHTNING GUN

======================================================================
*/

// rain - BurnMeGood now takes the flamechunk separately, because
// the old 'set-self-in-flames' method doesn't have a flamechunk to
// pass, and deaths were getting blamed on the world/player 0
void G_BurnMeGood( gentity_t *self, gentity_t *body, gentity_t *chunk )
{
	vec3_t origin;

	// add the new damage
	body->flameQuota += 5;
	body->flameQuotaTime = level.time;

	// rain - fill in our own origin if we have no flamechunk
	if (chunk != NULL)
		VectorCopy(chunk->r.currentOrigin, origin);
	else
		VectorCopy(self->r.currentOrigin, origin);
	
	
	// JPW NERVE -- yet another flamethrower damage model, trying to find a feels-good damage combo that isn't overpowered
	if (body->lastBurnedFrameNumber != level.framenum) {
		G_Damage( body, self, self, vec3_origin, self->r.currentOrigin, G_GetWeaponDamage(WP_FLAMETHROWER), 0, MOD_FLAMETHROWER ); // was 2 dmg in release ver, hit avg. 2.5 times per frame
		body->lastBurnedFrameNumber = level.framenum;
	}
	// jpw
	
	// make em burn
	if( body->client && (body->health <= 0 || body->flameQuota > 0) ) { // JPW NERVE was > FLAME_THRESHOLD
		if (body->s.onFireEnd < level.time)
			body->s.onFireStart = level.time;

		body->s.onFireEnd = level.time + FIRE_FLASH_TIME;
		// rain - use ourself as the attacker if we have no flamechunk
		body->flameBurnEnt = chunk != NULL ? chunk->r.ownerNum : self->s.number;
		// add to playerState for client-side effect
		body->client->ps.onFireStart = level.time;
	}
}

// TTimo - for traces calls
static vec3_t	flameChunkMins = {-4, -4, -4};
static vec3_t	flameChunkMaxs = { 4,  4,  4};

gentity_t *Weapon_FlamethrowerFire( gentity_t *ent ) {
	gentity_t	*traceEnt;
	vec3_t		start;
	vec3_t		trace_start;
	vec3_t		trace_end;
	trace_t 	trace;

	VectorCopy( ent->r.currentOrigin, start );
	start[2] += ent->client->ps.viewheight;
	VectorCopy( start, trace_start );

	VectorMA( start, -8, forward, start );
	VectorMA( start, 10, right, start );
	VectorMA( start, -6, up, start );

	AddLean( ent, start );

	// prevent flame thrower cheat, run & fire while aiming at the ground, don't get hurt
	// 72 total box height, 18 xy -> 77 trace radius (from view point towards the ground) is enough to cover the area around the feet
	VectorMA( trace_start, 77.0, forward, trace_end);
	trap_Trace( &trace, trace_start, flameChunkMins, flameChunkMaxs, trace_end, ent->s.number, MASK_SHOT | MASK_WATER );	
	if (trace.fraction != 1.0)
	{
		// additional checks to filter out false positives
		if (trace.endpos[2] > (ent->r.currentOrigin[2]+ent->r.mins[2]-8) && trace.endpos[2] < ent->r.currentOrigin[2]) 
		{
			// trigger in a 21 radius around origin
			trace_start[0] -= trace.endpos[0];
			trace_start[1] -= trace.endpos[1];
			if (trace_start[0]*trace_start[0]+trace_start[1]*trace_start[1] < 441)
			{
				// set self in flames
				// rain - fixed BurnMeGood
				G_BurnMeGood( ent, ent, NULL );
			}
		}
	}

	traceEnt = fire_flamechunk ( ent, start, forward );

	// bani - flamethrower exploit fix
	ent->r.svFlags |= SVF_BROADCAST;
	ent->client->flametime = level.time + 2500;
	return traceEnt;
}

//======================================================================



/*
===============
AccuracyHit
===============
*/
qboolean AccuracyHit( gentity_t *target, gentity_t *attacker ) 
{
	if( !target->takedamage ) {
		return qfalse;
	}

	if( !attacker ) {
		return qfalse;
	}

	if ( target == attacker ) {
		return qfalse;
	}

	if( !target->client ) {
		return qfalse;
	}

	if( !attacker->client ) {
		return qfalse;
	}

	if ( OnSameTeam( target, attacker ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint ( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) 
{

	VectorCopy( ent->r.currentOrigin, muzzlePoint );

	// tjw: if a player is hovering over a wounded player
	//      he should be able to shoot/stab that player
	//      so we need to raise the muzzleOrigin
	//      otherwise the start of the trace will
	//      be inside the wounded player and not find them.
	if(ent->client->ps.viewangles[PITCH] > 30 && (ent->client->ps.eFlags & EF_CROUCHING))
	{		
		muzzlePoint[2] += 30;
	}
	else
	{
		muzzlePoint[2] += ent->client->ps.viewheight;
	}


	// Ridah, offset for more realistic firing from actual gun position
	//----(SA) modified
	switch(weapon)	// Ridah, changed this so I can predict weapons
	{
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_MEDIC_SYRINGE:
			break;
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
			VectorMA(muzzlePoint,10,right,muzzlePoint);
			break;
		case WP_DYNAMITE:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_SATCHEL:
		case WP_SMOKE_BOMB:
			VectorMA( muzzlePoint, 20, right, muzzlePoint );
			break;
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
			VectorMA( muzzlePoint, -6, right, muzzlePoint );
			VectorMA( muzzlePoint, -4, up, muzzlePoint );
			break;
		default:
			VectorMA( muzzlePoint, 6, right, muzzlePoint );
			VectorMA( muzzlePoint, -4, up, muzzlePoint );
			break;
	}

	// done.

	// (SA) actually, this is sort of moot right now since
	// you're not allowed to fire when leaning.  Leave in
	// in case we decide to enable some lean-firing.
	// (SA) works with gl now
	if ( weapon )
		AddLean(ent, muzzlePoint);

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

// Rafael - for activate
void CalcMuzzlePointForActivate ( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint, qboolean attack )
{

	VectorCopy( ent->s.pos.trBase, muzzlePoint );

	if (ent->health <= 0 )
	{
		muzzlePoint[2] += 25;
 // jaquboss revived hack, so revive will work a bit better
	} else if( !attack && ent->client->ps.viewangles[PITCH] > 30 && (ent->client->ps.eFlags & EF_CROUCHING)) 
	{
		muzzlePoint[2] += 30;
	}
	else 
	{
		muzzlePoint[2] += ent->client->ps.viewheight;
	}

	AddLean(ent, muzzlePoint);

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}
// done.

// Ridah
void CalcMuzzlePoints(gentity_t *ent, int weapon) 
{
	vec3_t	viewang;

	VectorCopy(ent->client->ps.viewangles, viewang);

	{	// non ai's take into account scoped weapon 'sway' (just another way aimspread is visualized/utilized)
		float spreadfrac, phase;

		if(BG_IsScopedWeapon(weapon)) 
		{
			float pitchAmp, yawAmp;
			float pitchMinAmp, yawMinAmp;

			spreadfrac = ent->client->currentAimSpreadScale;

			if( weapon == WP_FG42_SCOPE )
			{
				pitchAmp	= 4*ZOOM_PITCH_AMPLITUDE;
				yawAmp		= 4*ZOOM_YAW_AMPLITUDE;
				pitchMinAmp = 4*ZOOM_PITCH_MIN_AMPLITUDE;
				yawMinAmp	= 4*ZOOM_YAW_MIN_AMPLITUDE;
			} 
			else 
			{
				pitchAmp	= ZOOM_PITCH_AMPLITUDE;
				yawAmp		= ZOOM_YAW_AMPLITUDE;
				pitchMinAmp = ZOOM_PITCH_MIN_AMPLITUDE;
				yawMinAmp	= ZOOM_YAW_MIN_AMPLITUDE;
			}

			// rotate 'forward' vector by the sway
			phase = level.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
			viewang[PITCH]	+= ZOOM_PITCH_AMPLITUDE * sin( phase ) * (spreadfrac+pitchMinAmp);

			phase = level.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
			viewang[YAW]	+= ZOOM_YAW_AMPLITUDE * sin( phase ) * (spreadfrac+yawMinAmp);
		}
	}

	// SYNC WITH THE CG_VIEW.C CODE!!!!
	if ( ent->client && ent->client->ps.eFlags & EF_POISONED ){
		float phase;

		phase = level.time / 1000.0 * 0.3 * M_PI; // time / 1000 * frequency * 2pi
		viewang[ROLL] += 36 * sin( phase ); // amplitude * sin
		viewang[YAW] += 24 * sin( phase ); // amplitude * sin
		viewang[PITCH] += 12 * sin( phase*2.5 ); // amplitude * sin

	}


	// set aiming directions
	AngleVectors (viewang, forward, right, up);

//----(SA)	modified the muzzle stuff so that weapons that need to fire down a perfect trace
//			straight out of the camera (SP5, Mauser right now) can have that accuracy, but
//			weapons that need an offset effect (bazooka/grenade/etc.) can still look like
//			they came out of the weap.
	CalcMuzzlePointForActivate( ent, forward, right, up, muzzleTrace, qtrue );
	CalcMuzzlePoint ( ent, weapon, forward, right, up, muzzleEffect );
}

qboolean G_PlayerCanBeSeenByOthers( gentity_t *ent ) 
{
	int			i;
	gentity_t	*ent2;
	vec3_t		pos[3];

	VectorCopy( ent->client->ps.origin, pos[0] );
	pos[0][2] += ent->client->ps.mins[2];
	VectorCopy( ent->client->ps.origin, pos[1] );
	VectorCopy( ent->client->ps.origin, pos[2] );
	pos[2][2] += ent->client->ps.maxs[2];

	for( i = 0, ent2 = g_entities; i < level.maxclients; i++, ent2++ )
	{
		if( !ent2->inuse || ent2 == ent ) 
			continue;
		
		if( ent2->client->sess.sessionTeam == TEAM_SPECTATOR )
			continue;

		if( ent2->health <= 0 || ent2->client->sess.sessionTeam == ent->client->sess.sessionTeam ) 
			continue;
		
		if( ent2->client->ps.eFlags & EF_ZOOMING ) 
		{
			G_SetupFrustum_ForBinoculars( ent2 );
		}
		else 
		{
			G_SetupFrustum( ent2 );
		}

		if( G_VisibleFromBinoculars( ent2, ent, pos[0] ) ||
			G_VisibleFromBinoculars( ent2, ent, pos[1] ) ||
			G_VisibleFromBinoculars( ent2, ent, pos[2] ) ) 
		{
			return qtrue;
		}
	}

	return qfalse;
}

qboolean jP_CheckDisguise( gentity_t *ent )
{
	qboolean stealthy		= qfalse; 
	int		 weapon			= ent->s.weapon;
	
	// It is assumned that this function will NOT be called unless ent is wearing a disguise

	switch ( weapon )
	{
		// check for weapons that will never compromise a disguise
		case WP_SMOKE_BOMB:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_BINOCULARS:	
		case WP_MEDIC_ADRENALINE: // with g_skill may be possible
			return qtrue;			
		// these weapons will only compromise a disguise if they are seen by another player
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_STEN:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_K43:
		case WP_K43_SCOPE:
		case WP_GARAND:
		case WP_GARAND_SCOPE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
			stealthy = !G_PlayerCanBeSeenByOthers( ent );
			break;
		default:	
			stealthy = qfalse;				
	}

	if (!stealthy)
	{		
		if (jP_DoChargeCost(ent, SK_COVERTOPS, BG_GetFromTable(disguiseTestCost, ent->client->sess.skillBits, SK_COVERTOPS)))
		{
			return qtrue;
		}
		else
		{
			return qfalse;
		}
	}
	return qtrue;
}

void Kick_CheckTriggers (  gentity_t *ent, vec3_t start, vec3_t end )
{
	trace_t		tr;
	gentity_t	*traceEnt;

	trap_Trace( &tr, start, tv(-6.f,-6.f,-6.f), tv(6.f,6.f,6.f), end, ent->s.number,  CONTENTS_TRIGGER);

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.fraction == 1.0 )
		return;

	traceEnt = &g_entities[tr.entityNum];

	if ( !Q_stricmp( traceEnt->classname, "func_invisible_user") )
	{
		traceEnt->flags |= FL_KICKACTIVATE;		
		traceEnt->use( traceEnt, ent, ent );
		//traceEnt->flags &= ~FL_KICKACTIVATE;
	}
}

void Kick_Fire( gentity_t *ent ) 
{
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	vec3_t		forward, right, up, offset;
	gentity_t	*tent;

	AngleVectors( ent->client->ps.viewangles, forward, right, up );

	//G_Printf("%f \n", forward[2] );
	// only kick low
	if ( forward[2] > -0.5)
		forward[2] = -0.5;

	CalcMuzzlePointForActivate( ent, forward, right, up, offset, qtrue );

	// note to self: we need to determine the usable distance for wolf
	VectorMA( offset, 76, forward, end );
	// @TODO adjust maxs/mins
	G_HistoricalTrace(ent, &tr, offset, tv(-6.f,-6.f,-6.f), tv(6.f,6.f,6.f), end, ent->s.number, MASK_SHOT);

	// kick the baby!
	if(g_canisterKick.integer) 
		G_CanisterKick(ent);

	Kick_CheckTriggers( ent, offset, end );

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.fraction == 1.0 )
	{
		tent = G_TempEntity( tr.endpos, EV_WOLFKICK_MISS );
		tent->s.eventParm = ent->s.number;
		return;
	}

	traceEnt = &g_entities[tr.entityNum];

	if ( Q_stricmp( traceEnt->classname, "func_door_rotating") == 0 )
	{
		traceEnt->flags |= FL_KICKACTIVATE;						// no noise
		G_TryDoor( traceEnt, ent, ent );						// (door,other,activator)
	//	traceEnt->flags &= ~FL_KICKACTIVATE;					// jaquboss do not reset, it is done when it reach the end pos
	}
	else if	((Q_stricmp( traceEnt->classname, "func_button") == 0) &&
			(traceEnt->s.apos.trType == TR_STATIONARY && traceEnt->s.pos.trType == TR_STATIONARY) &&
			traceEnt->active == qfalse )
	{
		Use_BinaryMover( traceEnt, ent, ent );
		traceEnt->active = qtrue;
	}
	else if ( !Q_stricmp( traceEnt->classname, "props_flippy_table") && traceEnt->use )
	{
		traceEnt->use( traceEnt, ent, ent );
	}
	
	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, offset );

	if ( tr.entityNum >= MAX_CLIENTS )
	{
		vec3_t	reflect;
		float	dot;

		tent = G_TempEntity( tr.endpos, EV_WOLFKICK_HIT_WALL );

		dot = DotProduct( forward, tr.plane.normal );
		VectorMA( forward, -2 * dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );
		tent->s.eventParm = DirToByte( reflect );
	} 
	else 
	{
		tent = G_TempEntity( tr.endpos, EV_WOLFKICK_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
	}

	tent->s.otherEntityNum = ent->s.number;

	if ( traceEnt->takedamage )
		G_Damage( traceEnt, ent, ent, forward, tr.endpos, BG_Weapons[WP_NONE].damage ? BG_Weapons[WP_NONE].damage : 16 , 0, MOD_KICKED );	//----(SA)	modified

	// knock us back so we don't bleed into walls
	VectorMA (ent->client->ps.velocity, -96, forward, ent->client->ps.velocity);

}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent ) 
{
	float	aimSpreadScale	= 1.0f;
	int		shots			= 1;
	gentity_t *pFiredShot = 0; // Omni-bot To tell bots about projectiles	


	// ydnar: dead guys don't fire guns
	if( ent->client->ps.pm_type == PM_DEAD ) 
	{
		return;
	}
	
	// Rafael emplaced/mounted mg42
	if (ent->client->ps.persistant[PERS_HWEAPON_USE] && ent->active)
	{
		return;
	}

	// Ridah, need to call this for AI prediction also
	CalcMuzzlePoints(ent, ent->s.weapon);

	if (g_userAim.integer) 
	{
		aimSpreadScale = ent->client->currentAimSpreadScale;
		
		// Ridah, TODO : add accuracy factor for AI here


		aimSpreadScale += 0.15f;	// (SA) just adding a temp /maximum/ accuracy for player (this will be re-visited in greater detail :)

		if(aimSpreadScale > 1)
		{
			aimSpreadScale = 1.0f;	// still cap at 1.0
		}
	} 

	// jP Note - call arty
	if( (ent->client->ps.eFlags & EF_ZOOMING) && (ent->client->ps.stats[STAT_KEYS] & (1 << INV_BINOCS)) ) 
	{
		if( ent->client->sess.playerType == PC_FIELDOPS) 
		{
			if( !(ent->client->ps.leanf) )
			{
				Weapon_Artillery(ent);
			}
			return;
		}
	}

	// jP Note - double spread if airborne
	if( ent->client->ps.groundEntityNum == ENTITYNUM_NONE )
	{
		aimSpreadScale = 2.0f;
	}

	// jet Pilot - Test covert ops disguise against loss
	if (ent->client->ps.powerups[PW_OPS_DISGUISED] && !jP_CheckDisguise( ent ))
	{
		ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
	}

/*
	// covert ops disguise handling
	if( ent->client->ps.powerups[PW_OPS_DISGUISED] &&
		ent->s.weapon != WP_SMOKE_BOMB	&&
		ent->s.weapon != WP_SATCHEL		&&
		ent->s.weapon != WP_SATCHEL_DET &&		
		// rain - add binoculars - they can be selected manually and fired
		ent->s.weapon != WP_BINOCULARS ) {
		if( !( ent->s.weapon == WP_KNIFE				||
			ent->s.weapon == WP_STEN					||
			ent->s.weapon == WP_SILENCER				||
			ent->s.weapon == WP_SILENCED_COLT			||
			ent->s.weapon == WP_AKIMBO_SILENCEDCOLT 	||
			ent->s.weapon == WP_AKIMBO_SILENCEDLUGER	||
			ent->s.weapon == WP_K43						||
			ent->s.weapon == WP_K43_SCOPE				||
			ent->s.weapon == WP_GARAND					||
			ent->s.weapon == WP_GRENADE_LAUNCHER		||
			ent->s.weapon == WP_GRENADE_PINEAPPLE		||
			ent->s.weapon == WP_GARAND_SCOPE ) ) {
			ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
		} else if( G_PlayerCanBeSeenByOthers( ent ) ) {
			ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
		}
	}
*/

	// fire the specified weapon
	switch( ent->s.weapon ) 
	{
		/*case WP_FIREBOLT:
			Weapon_Firebolt( ent ); // insanity mode BTW
		break;*/
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			Weapon_Knife( ent );
			break;
		// NERVE - SMF
		case WP_MEDKIT:
			Weapon_Medic( ent );
			break;
		case WP_PLIERS:
			Weapon_Engineer( ent );
			break;
		case WP_SMOKE_MARKER:
			jP_DoChargeCost(ent, SK_SIGNALS, BG_GetFromTable(chargeCost_Airstrike, ent->client->sess.skillBits, SK_SIGNALS));
			pFiredShot = weapon_grenadelauncher_fire(ent, WP_SMOKE_MARKER);
			break;
		// -NERVE - SMF

		case WP_POISON_SYRINGE:
			if (!Weapon_Poison(ent))
				ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_POISON_SYRINGE)] += 1;
			else if(g_gamestate.integer == GS_PLAYING)
				ent->client->sess.aWeaponStats[WS_POISON].hits++;
			break;
		case WP_MEDIC_SYRINGE:
			if (!Weapon_Syringe(ent))
				ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_MEDIC_SYRINGE)] += 1;
			else if(g_gamestate.integer == GS_PLAYING)
				ent->client->sess.aWeaponStats[WS_SYRINGE].hits++;
			break;
		case WP_MEDIC_ADRENALINE:
			jP_DoChargeCost(ent, SK_MEDIC, BG_GetFromTable(chargeCost_Adrenaline, ent->client->sess.skillBits, SK_FIRST_AID));
			//ent->client->ps.classWeaponTime = level.time;
			Weapon_AdrenalineSyringe(ent);
			break;
		case WP_AMMO:
			Weapon_MagicAmmo( ent );
			break;
		case WP_LUGER:
			Bullet_Fire( ent, LUGER_SPREAD*aimSpreadScale, LUGER_DAMAGE, qtrue );
			break;
		case WP_SILENCER:
			Bullet_Fire( ent, SILENCER_SPREAD*aimSpreadScale, SILENCER_DAMAGE, qtrue );
			break;
		case WP_AKIMBO_LUGER:
			Bullet_Fire( ent, AKIMBO_LUGER_SPREAD*aimSpreadScale, AKIMBO_LUGER_DAMAGE, qtrue );
			break;
		case WP_AKIMBO_SILENCEDLUGER:
			Bullet_Fire( ent, AKIMBO_SILENCEDLUGER_SPREAD*aimSpreadScale, AKIMBO_SILENCEDLUGER_DAMAGE, qtrue );
			break;
		case WP_COLT:
			Bullet_Fire( ent, COLT_SPREAD*aimSpreadScale, COLT_DAMAGE, qtrue );
			break;
		case WP_SILENCED_COLT:
			Bullet_Fire( ent, SILENCED_COLT_SPREAD*aimSpreadScale, SILENCED_COLT_DAMAGE, qtrue );
			break;
		case WP_AKIMBO_COLT:
			Bullet_Fire( ent, AKIMBO_COLT_SPREAD*aimSpreadScale, AKIMBO_COLT_DAMAGE, qtrue );
			break;
		case WP_AKIMBO_SILENCEDCOLT:
			Bullet_Fire( ent, AKIMBO_SILENCEDCOLT_SPREAD*aimSpreadScale, AKIMBO_SILENCEDCOLT_DAMAGE, qtrue );
			break;
		case WP_KAR98:
			aimSpreadScale = 1.0f;
			Bullet_Fire( ent, KAR98_SPREAD*aimSpreadScale, KAR98_DAMAGE, qfalse );
			break;
		case WP_CARBINE:
			aimSpreadScale = 1.0f;
			Bullet_Fire( ent, CARBINE_SPREAD*aimSpreadScale, CARBINE_DAMAGE, qfalse );
			break;
		case WP_FG42_SCOPE:
			Bullet_Fire( ent, FG42SCOPE_SPREAD*aimSpreadScale, FG42SCOPE_DAMAGE, qfalse );
			break;
		case WP_FG42:
			Bullet_Fire( ent, FG42_SPREAD*aimSpreadScale, FG42_DAMAGE, qtrue );
			break;
		case WP_GARAND_SCOPE:
			Bullet_Fire( ent, GARANDSCOPE_SPREAD*aimSpreadScale, GARANDSCOPE_DAMAGE, qfalse );
			break;
		case WP_GARAND:
			aimSpreadScale = 1.0f;
			Bullet_Fire( ent, GARAND_SPREAD*aimSpreadScale, GARAND_DAMAGE, qfalse );
			break;
		case WP_SHOTGUN:	// jet Pilot - fire shotgun
			Weapon_Shotgun( ent );	// Jaybird
			break;
		case WP_SATCHEL_DET:
			if( G_ExplodeSatchels( ent ) ) {
				ent->client->ps.ammo[WP_SATCHEL_DET] = 0;
				ent->client->ps.ammoclip[WP_SATCHEL_DET] = 0;
				ent->client->ps.ammoclip[WP_SATCHEL] = 1;
				G_AddEvent( ent, EV_NOAMMO, 0 );
			}
			break;
		case WP_TRIPMINE:
			G_PlaceTripmine(ent);
			break;
		case WP_MOBILE_MG42_SET:
			Bullet_Fire( ent, MOBILE_MG42_SPREAD * 0.05f * aimSpreadScale, MOBILE_MG42_DAMAGE, qfalse );
			break;
		case WP_MOBILE_BROWNING_SET:
			Bullet_Fire( ent, MOBILE_BROWNING_SPREAD * 0.05f * aimSpreadScale, MOBILE_BROWNING_DAMAGE, qfalse );
			break;
		case WP_BAR_SET:
			Bullet_Fire( ent, BAR_SPREAD * 0.33f * aimSpreadScale, BAR_DAMAGE, qfalse );
			break;
		case WP_MOBILE_MG42:
			if( ent->client->ps.pm_flags & PMF_DUCKED || ent->client->ps.eFlags & EF_PRONE ) {
				Bullet_Fire( ent, MOBILE_MG42_SPREAD * 0.6f * aimSpreadScale, MOBILE_MG42_DAMAGE, qfalse );
			} else {
				Bullet_Fire( ent, MOBILE_MG42_SPREAD*aimSpreadScale, MOBILE_MG42_DAMAGE, qfalse );
			}
			break;
		case WP_MOBILE_BROWNING:
			if( ent->client->ps.pm_flags & PMF_DUCKED || ent->client->ps.eFlags & EF_PRONE ) {
				Bullet_Fire( ent, MOBILE_BROWNING_SPREAD * 0.6f * aimSpreadScale, MOBILE_BROWNING_DAMAGE, qfalse );
			} else {
				Bullet_Fire( ent, MOBILE_BROWNING_SPREAD*aimSpreadScale, MOBILE_BROWNING_DAMAGE, qfalse );
			}
			break;
		case WP_BAR:
			if( ent->client->ps.pm_flags & PMF_DUCKED || ent->client->ps.eFlags & EF_PRONE ) {
				Bullet_Fire( ent, BAR_SPREAD * 0.6f * aimSpreadScale, BAR_DAMAGE, qfalse );
			} else {
				Bullet_Fire( ent, BAR_SPREAD*aimSpreadScale, BAR_DAMAGE, qfalse );
			}
			break;
		case WP_K43_SCOPE:
			Bullet_Fire( ent, K43SCOPE_SPREAD*aimSpreadScale, K43SCOPE_DAMAGE, qfalse );
			break;
		case WP_K43:
			aimSpreadScale = 1.0;
			Bullet_Fire( ent, K43_SPREAD * aimSpreadScale, K43_DAMAGE, qfalse );
			break;
		case WP_STG44:		
			Bullet_Fire( ent, STG44_SPREAD * aimSpreadScale, STG44_DAMAGE, qtrue );
			break;
		case WP_STEN:		
			Bullet_Fire( ent, STEN_SPREAD * aimSpreadScale, STEN_DAMAGE, qtrue );
			break;
		case WP_STEN_MKII:
			Bullet_Fire( ent, STENMKII_SPREAD * aimSpreadScale, STENMKII_DAMAGE, qtrue );
			break;
		case WP_MP40:
			Bullet_Fire( ent, MP40_SPREAD * aimSpreadScale, MP40_DAMAGE, qtrue );
			break;
		case WP_THOMPSON:
			Bullet_Fire( ent, THOMPSON_SPREAD * aimSpreadScale, THOMPSON_DAMAGE, qtrue );
			break;
		case WP_MP34:
			Bullet_Fire( ent, MP34_SPREAD * aimSpreadScale, MP34_DAMAGE, qtrue );
			break;
		case WP_VENOM:
			Bullet_Fire( ent, VENOM_SPREAD*aimSpreadScale, VENOM_DAMAGE, qtrue );
			break;
		case WP_BAZOOKA:
			jP_DoChargeCost(ent, SK_SOLDIER, BG_GetFromTable(chargeCost_Bazooka, ent->client->sess.skillBits, SK_SOLDIER));
			pFiredShot = Weapon_Bazooka_Fire(ent);

			if( ent->client )
			{
				vec3_t forward;
				AngleVectors (ent->client->ps.viewangles, forward, NULL, NULL);
				VectorMA (ent->client->ps.velocity, -64, forward, ent->client->ps.velocity);
			}
			break;
		
		case WP_PANZERFAUST:
			jP_DoChargeCost(ent, SK_SOLDIER, BG_GetFromTable(chargeCost_Panzer, ent->client->sess.skillBits, SK_SOLDIER));
			pFiredShot = Weapon_Panzerfaust_Fire(ent, forward);

			if( ent->client )
			{
				vec3_t forward;
				AngleVectors (ent->client->ps.viewangles, forward, NULL, NULL);
				VectorMA (ent->client->ps.velocity, -64, forward, ent->client->ps.velocity);
			}
			break;

		case WP_GPG40:
		case WP_M7:
			jP_DoChargeCost(ent, SK_ENGINEER, BG_GetFromTable(chargeCost_Riflenade, ent->client->sess.skillBits, SK_ENGINEER));
			pFiredShot = weapon_gpg40_fire( ent, ent->s.weapon );
			break;
		case WP_MORTAR2_SET:
		case WP_MORTAR_SET:
			jP_DoChargeCost(ent, SK_SOLDIER, BG_GetFromTable(chargeCost_Mortar, ent->client->sess.skillBits, SK_SOLDIER));
			weapon_mortar_fire( ent, ent->s.weapon );
			break;

		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
			pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
			break;

		case WP_SATCHEL:
			jP_DoChargeCost(ent, SK_COVERTOPS, BG_GetFromTable(chargeCost_Satchel, ent->client->sess.skillBits, SK_COVERTOPS));
			pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
			break;

		case WP_DYNAMITE:
			jP_DoChargeCost(ent, SK_ENGINEER, BG_GetFromTable(chargeCost_Dynamite, ent->client->sess.skillBits, SK_ENGINEER));
			pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
			break;

		case WP_LANDMINE:
			jP_DoChargeCost(ent, SK_ENGINEER, BG_GetFromTable(chargeCost_Landmine, ent->client->sess.skillBits, SK_ENGINEER));
			pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
			break;
		case WP_SMOKE_BOMB:
			jP_DoChargeCost(ent, SK_COVERTOPS, BG_GetFromTable(chargeCost_Smoke, ent->client->sess.skillBits, SK_COVERTOPS));
			pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
			break;
		case WP_FLAMETHROWER:
			pFiredShot = Weapon_FlamethrowerFire( ent );
			break;
	/*	case WP_MAPMORTAR:
			break;*/
		default:
			break;
	}

#ifdef OMNIBOTS
	// Omni-bot - Send a fire event.
	if(ent->r.svFlags & SVF_BOT)
	{
		int iSource = Bot_WeaponGameToBot(ent->s.weapon);
		BotUserData bud;

		if(pFiredShot)
		{
			bud.DataType = dtEntity;
			bud.udata.m_Entity = pFiredShot;
			Bot_Interface_SendEvent(ACTION_WEAPON_FIRE_PROJECTILE, ent-g_entities, iSource, 0, &bud);
		}

		bud.DataType = dtInt;
		bud.udata.m_Int = Bot_WeaponGameToBot(ent->s.weapon);
		bud.DataType = dtEntity;
		bud.udata.m_Entity = pFiredShot;
		Bot_Interface_SendEvent(ACTION_WEAPON_FIRE, ent-g_entities, iSource, 0, &bud);
	}
#endif

	// OSP
#ifndef DEBUG_STATS
	if(g_gamestate.integer == GS_PLAYING)
#endif
		ent->client->sess.aWeaponStats[BG_WeapStatForWeapon(ent->s.weapon)].atts += shots;
}

// Created by: dvl
// used in G_shrubbot_lol
void G_createClusterNade( gentity_t *ent, int numNades )
{
	vec3_t dir;
	int it, grenadeWPID, degPerNade;
	float x, y;
	gentity_t *nade;

	if( ent->client->sess.sessionTeam == TEAM_AXIS ) {
		grenadeWPID = WP_GRENADE_LAUNCHER;
	} else {
		grenadeWPID = WP_GRENADE_PINEAPPLE;
	}
	if( numNades < 1 ) {
		numNades = 1;
	} else if( numNades > 32 ) {
		numNades = 32;
	}
	degPerNade = 360 / numNades;
	for( it = 0; it < numNades; it++ ) {
		// there's probly a better way to do this, but this works
		x = 200 * cos( DEG2RAD(degPerNade * it) );  
		y = 200 * sin( DEG2RAD(degPerNade * it) );
		VectorSet( dir, x, y, 30 );
		VectorSubtract( ent->client->ps.velocity, dir, dir );
		dir[2] = 60;
		nade = fire_grenade( ent, ent->r.currentOrigin, 
					dir, grenadeWPID );
		nade->nextthink = level.time + 500 + (rand()%1000); // jet Pilot - randomize the timers on grenades a bit
	}
}

// forty - canister kicking
void G_CanisterKick( gentity_t *ent )
{

	gentity_t	*traceEnt;
	vec3_t		forward, right, up, angles, start, mins, maxs, origin;
	int i, num;
	int touch[MAX_GENTITIES];
	//dead players  
	if ( ent->client->ps.pm_flags & PMF_LIMBO ||  
		ent->client->ps.pm_type == PM_DEAD ||  
		ent->client->ps.eFlags & EF_PRONE  ) { //prone players    
		// do not pass go, do not kick canisters  
		return;  
	}
	// Find straight ahead, no pitching...  
	// We do this since we don't actually want to have to look at the item we  
	// are going to kick  

	VectorCopy(ent->client->ps.viewangles, angles);
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AngleVectors(angles, forward, right, up);

	//move straight ahead from origin 24 units
	// Not the full 32 since we want to be able to kick thing we are just about standing on. 
	VectorMA(ent->r.currentOrigin,24,forward,origin);

	//only kick things that are up to CH_KNIFE_DIST away in front of us.
	//and only up to about waist high.
	VectorAdd( origin, tv(-32.f,-32.f,-24.f), mins );
	VectorAdd( origin, tv(32.f,32.f,8.f), maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if(g_debugBullets.integer) {
		/*gentity_t *tent;

		tent = G_TempEntity( mins, EV_RAILTRAIL );
		VectorCopy(maxs, tent->s.origin2);
		tent->s.dmgFlags = 1;*/
		G_RailBox( origin, mins, maxs, tv(1.f,0.f,0.f), -1 );

	}

	for(i=0; i<num; i++) {

		traceEnt = &g_entities[touch[i]];

	// prevents a nasty kick corpses bug if they died holding any of  
	// the kick weapon types.  
		if( traceEnt->s.eType != ET_MISSILE)  
			continue;

	// check for grenades, and misc canisters otherwise reject...
		if(	traceEnt->s.weapon == WP_GRENADE_PINEAPPLE ||
			traceEnt->s.weapon == WP_GRENADE_LAUNCHER ||
			traceEnt->s.weapon == WP_SMOKE_BOMB ||
			traceEnt->s.weapon == WP_SMOKE_MARKER ) {

			VectorCopy(ent->client->ps.viewangles, angles);
			// Negative pitch points up, positive points down towards ground.  
				if(angles[PITCH] > -15)  
					angles[PITCH] = -15;  

			AngleVectors(angles, forward, right, up);
			VectorCopy(traceEnt->s.pos.trBase, start);
			
			traceEnt->s.pos.trType = TR_GRAVITY;
			traceEnt->s.pos.trTime = level.time - 50;		// move a bit on the very first frame
		
			// jaquboss put it up
			// put it up so it is less buggy on slopes
			start[2] += 30;

			VectorCopy(start, traceEnt->r.currentOrigin);
			VectorCopy(start, traceEnt->s.pos.trBase);

			//One Healthy Kick....
			//FIXME make scale value adjustable, 750 is probably a good base.
			VectorScale(forward, g_canisterKick.integer * 10, traceEnt->s.pos.trDelta);

			// jaquboss fly pretty higher
			//so it is less buggy on slopes
			traceEnt->s.pos.trDelta[2] += g_canisterKick.integer*2;
			SnapVector(traceEnt->s.pos.trDelta);

			if(g_canisterKickOwner.integer) {  
					// canister owner gets set to kicker.  
					traceEnt->parent = ent;  
					traceEnt->r.ownerNum = ent->s.number;  
					traceEnt->s.teamNum = ent->client->sess.sessionTeam;  
			}  
		}
		return;
	}

	return;
}

/*
==============
Weapon_Shotgun
--------------
Jaybird - #60 - Only gives one accuracy hit per shot,
instead of how many bullets hit in one shot.  This
fixes the issue where players can achieve 400% accuracy
with the shotgun.
==============
*/
void Weapon_Shotgun( gentity_t *ent ) {
	int			i;
	int			hitClient = 0;
	int			hits;
	vec3_t		end;

	// Back these up so that we only give them a single hit
	hits = ent->client->sess.aWeaponStats[BG_WeapStatForWeapon(WP_SHOTGUN)].hits;

	// generate the "random" spread pattern
	G_HistoricalTraceBegin( ent );
	for ( i = 0; i < max( 1, g_shotgunPellets.integer ); i++ ) {
		Bullet_Endpos( ent, SHOTGUN_SPREAD, &end );
		if( Bullet_Fire_Extended(ent, ent, muzzleTrace, end, SHOTGUN_SPREAD, SHOTGUN_DAMAGE, qtrue, 0) && !hitClient ) {
			hitClient++;
		}
	}
	G_HistoricalTraceEnd( ent );

	// Now give stats
	if( hitClient ) {
		ent->client->sess.aWeaponStats[BG_WeapStatForWeapon(WP_SHOTGUN)].hits = ++hits;
	}
}
