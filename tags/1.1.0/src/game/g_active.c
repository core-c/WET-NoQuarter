#include "g_local.h"
#include "etpro_mdx.h"
/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
void P_DamageFeedback( gentity_t *player ) {
	gclient_t	*client;
	float	count;
	vec3_t	angles;

	client = player->client;
	if ( client->ps.pm_type == PM_DEAD &&
		!(client->ps.eFlags & EF_PLAYDEAD)) {
		return;
	}

	// tjw: handle disorientation
	if(client->pmext.disoriented) {
		client->ps.delta_angles[ROLL] = 32000;
		client->ps.viewangles[ROLL] = 0;
		client->pmext.wasDisoriented = qtrue;
	}
	else if(client->pmext.wasDisoriented) {
		client->ps.delta_angles[ROLL] = 0;
		client->pmext.wasDisoriented = qfalse;
	}

	// total points of damage shot at the player this frame
	count = client->damage_blood;

	/*if ( client->lasthurt_mod == MOD_POISON )
		return;*/

	if ( count == 0 )
		return;		// didn't take any damage

	if ( count > 127 ) {
		count = 127;
	}

	// send the information to the client

	// world damage (falling, slime, etc) uses a special code
	// to make the blend blob centered instead of positional
	if ( client->damage_fromWorld ) {
		client->ps.damagePitch = 255;
		client->ps.damageYaw = 255;

		client->damage_fromWorld = qfalse;
	} else {
		vectoangles( client->damage_from, angles );
		client->ps.damagePitch = angles[PITCH]/360.0 * 256;
		client->ps.damageYaw = angles[YAW]/360.0 * 256;
	}

	// play an apropriate pain sound
	if ( (level.time > player->pain_debounce_time) &&
		!(player->flags & FL_GODMODE) &&
		!(player->s.powerups & PW_INVULNERABLE) /*&&
		!(client->ps.eFlags & EF_POISONED)*/) {	//----(SA)	

		player->pain_debounce_time = level.time + 700;
		G_AddEvent( player, EV_PAIN, player->health );
	}

	client->ps.damageEvent++;	// Ridah, always increment this since we do multiple view damage anims

	client->ps.damageCount = count;

	//
	// clear totals
	//
	client->damage_blood = 0;
	client->damage_knockback = 0;
}

#define MIN_BURN_INTERVAL 399 // JPW NERVE set burn timeinterval so we can do more precise damage (was 199 old model)

/*
=============
P_WorldEffects

Check for lava / slime contents and drowning
=============
*/
void P_WorldEffects( gentity_t *ent ) {
	int			waterlevel;

	if ( ent->client->noclip ) {
		if ( ent->client->sess.skill[SK_BATTLE_SENSE] >= 6 )
			ent->client->airOutTime = level.time + HOLDBREATHTIME_LONG;
		else
			ent->client->airOutTime = level.time + HOLDBREATHTIME;

		return;
	}

	waterlevel = ent->waterlevel;

	//
	// check for drowning
	//
	if ( waterlevel == 3 ) {
		// if out of air, start drowning
		if ( ent->client->airOutTime < level.time) {

			if(ent->client->ps.powerups[PW_BREATHER]) {	// take air from the breather now that we need it
				ent->client->ps.powerups[PW_BREATHER] -= (level.time - ent->client->airOutTime);
				ent->client->airOutTime = level.time + (level.time - ent->client->airOutTime);
			}
			else {


				// drown!
				ent->client->airOutTime += 1000;
				if ( ent->health > 0 ) {
					// take more damage the longer underwater
					ent->damage += 2;
					if (ent->damage > 15)
						ent->damage = 15;

					// play a gurp sound instead of a normal pain sound
					if (ent->health <= ent->damage) {
					//	G_Sound(ent, G_SoundIndex("*drown.wav"));
					} else if (rand()&1) {
						G_Sound(ent, G_SoundIndex("sound/player/gurp1.wav"));
					} else {
						G_Sound(ent, G_SoundIndex("sound/player/gurp2.wav"));
					}

					// don't play a normal pain sound
					ent->pain_debounce_time = level.time + 200;

					G_Damage (ent, NULL, NULL, NULL, NULL, ent->damage, 0, MOD_WATER);
				}
			}
		}
	} else {
		if ( ent->client->sess.skill[SK_BATTLE_SENSE] >= 6 )
			ent->client->airOutTime = level.time + HOLDBREATHTIME_LONG;
		else
			ent->client->airOutTime = level.time + HOLDBREATHTIME;

		ent->damage = 2;
	}

	//
	// check for sizzle damage (move to pmove?)
	//
	if (waterlevel && (ent->watertype&CONTENTS_LAVA) ) {
		if (ent->health > 0	&& ent->pain_debounce_time <= level.time ) {

				if (ent->watertype & CONTENTS_LAVA) {
					G_Damage (ent, NULL, NULL, NULL, NULL, 
						30*waterlevel, 0, MOD_LAVA);
				}

		}
	}

	//
	// check for burning from flamethrower
	//
	// JPW NERVE MP way
	if (ent->s.onFireEnd && ent->client) {
		if (level.time - ent->client->lastBurnTime >= MIN_BURN_INTERVAL) { 

			// JPW NERVE server-side incremental damage routine / player damage/health is int (not float)
			// so I can't allocate 1.5 points per server tick, and 1 is too weak and 2 is too strong.  
			// solution: allocate damage far less often (MIN_BURN_INTERVAL often) and do more damage.
			// That way minimum resolution (1 point) damage changes become less critical.

			ent->client->lastBurnTime = level.time;
			if ((ent->s.onFireEnd > level.time) && (ent->health > 0)) {
				gentity_t *attacker;
   				attacker = g_entities + ent->flameBurnEnt;
				G_Damage (ent, attacker, attacker, NULL, NULL, G_GetWeaponDamage(WP_FLAMETHROWER), DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER); // JPW NERVE was 7
			}
		}
	}
	// jpw
}



/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent ) {
/*	if (ent->waterlevel && (ent->watertype & CONTENTS_LAVA) )	//----(SA)	modified since slime is no longer deadly
		ent->s.loopSound = level.snd_fry;
	else*/ // Gordon: doesnt exist
		ent->s.loopSound = 0;
}

/*
==============
PushBot
==============
*/
void BotVoiceChatAfterIdleTime( int client, const char *id, int mode, int delay, qboolean voiceonly, int idleTime, qboolean forceIfDead );

void PushBot( gentity_t *ent, gentity_t *other ) {
	vec3_t dir, ang, f, r;
	float oldspeed;
	//
	oldspeed = VectorLength( other->client->ps.velocity );
	if (oldspeed < 200)
		oldspeed = 200;
	//
	VectorSubtract( other->r.currentOrigin, ent->r.currentOrigin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ang );
	AngleVectors( ang, f, r, NULL );
	f[2] = 0;
	r[2] = 0;
	//
	VectorMA( other->client->ps.velocity, 200, f, other->client->ps.velocity );
	VectorMA( other->client->ps.velocity, 100 * ((level.time+(ent->s.number*1000))%4000 < 2000 ? 1.0 : -1.0), r, other->client->ps.velocity );
	//
	if (VectorLengthSquared( other->client->ps.velocity ) > SQR(oldspeed)) {
		VectorNormalize( other->client->ps.velocity );
		VectorScale( other->client->ps.velocity, oldspeed, other->client->ps.velocity );
	}
	//
	// also, if "ent" is a bot, tell "other" to move!
#ifndef NO_BOT_SUPPORT
	if (rand()%50 == 0 && (ent->r.svFlags & SVF_BOT) && oldspeed < 10) {
		BotVoiceChatAfterIdleTime( ent->s.number, "Move", SAY_TEAM, 1000, qfalse, 20000, qfalse );
	}
#endif
}

/*
==============
ClientNeedsAmmo
==============
*/
qboolean ClientNeedsAmmo( int client ) {
	return AddMagicAmmo( &g_entities[client], 0 ) ? qtrue : qfalse;
}

// Does ent have enough "energy" to call artillery?
qboolean ReadyToCallArtillery( gentity_t* ent ) {
	if( ent->client->sess.skill[SK_SIGNALS] >= 2 ) {
		if( level.time - ent->client->ps.classWeaponTime <= (level.lieutenantChargeTime[ent->client->sess.sessionTeam-1]*0.66f) )
			return qfalse;
	} else if( level.time - ent->client->ps.classWeaponTime <= level.lieutenantChargeTime[ent->client->sess.sessionTeam-1] ) {
		return qfalse;
	}

	return qtrue;
}


// Are we ready to construct?  Optionally, will also update the time while we are constructing
qboolean ReadyToConstruct(gentity_t *ent, gentity_t *constructible, qboolean updateState)
{
	int weaponTime = ent->client->ps.classWeaponTime;

	// "Ammo" for this weapon is time based
	if( weaponTime + level.engineerChargeTime[ent->client->sess.sessionTeam-1] < level.time )
	{
		weaponTime = level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1];
	}

	if( g_debugConstruct.integer )
	{
		weaponTime += 0.5f*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)FRAMETIME));
	} 
	else 
	{

		// jet Pilot - Use charge cost array (Construction)
		weaponTime += chargeCost_Repair[ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION]] * 
				constructible->constructibleStats.chargebarreq*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)FRAMETIME));
		
		/*
		if( ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3 )
			weaponTime += 0.66f * 
				constructible->constructibleStats.chargebarreq*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)FRAMETIME));
			//weaponTime += 0.66f*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->wait/(float)FRAMETIME));
			//weaponTime += 0.66f * 2.f * ((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->wait/(float)FRAMETIME));
		else
			weaponTime += constructible->constructibleStats.chargebarreq*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)FRAMETIME));
			//weaponTime += 2.f * ((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->wait/(float)FRAMETIME));
		*/
	}

	// if the time is in the future, we have NO energy left
	if (weaponTime > level.time)
	{
		// if we're supposed to update the state, reset the time to now
//		if( updateState )
//			ent->client->ps.classWeaponTime = level.time;

		return qfalse;
	}

	// only set the actual weapon time for this entity if they want us to
	if( updateState ) //&& (!jp_insanity.integer & JP_INSANITY_CHARGEBAR) )
	{
		ent->client->ps.classWeaponTime = jp_insanity.integer & JP_INSANITY_CHARGEBAR ? 
			level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1] : weaponTime;
	}

	return qtrue;
}

void BotSetBlockEnt( int client, int blocker );
/*
==============
CheckBotImpacts
==============
*/
void CheckBotImpacts( gentity_t *ent, gentity_t *other ) {
	char *blockEnts[] = {"func_explosive", NULL};
	int j;

	for (j=0; blockEnts[j]; j++) {
		if (other->classname && !Q_stricmp( other->classname, blockEnts[j] )) {
			BotSetBlockEnt( ent->s.number, other->s.number );
		}
	}
}

//==============================================================

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm ) {
	int		i, j;
	gentity_t	*other;
	trace_t	trace;

	memset( &trace, 0, sizeof(trace) );
	for (i=0 ; i<pm->numtouch ; i++) {
		for (j=0 ; j<i ; j++) {
			if (pm->touchents[j] == pm->touchents[i] ) {
				break;
			}
		}
		if (j != i) {
			continue;	// duplicated
		}
		other = &g_entities[ pm->touchents[i] ];

		if ( !other->touch ) {
			continue;
		}

		other->touch( other, ent, &trace );
	}

}

/*
============
G_TouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void	G_TouchTriggers( gentity_t *ent ) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	trace_t		trace;
	vec3_t		mins, maxs;
	static vec3_t	range = { 40, 40, 52 };

	if ( !ent->client ) {
		return;
	}

	// Arnout: reset the pointer that keeps track of trigger_objective_info tracking
	ent->client->touchingTOI = NULL;

	// dead clients don't activate triggers!
	// tjw: unless they're only playing.
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 &&
		!(ent->client->ps.eFlags & EF_PLAYDEAD)) {
		return;
	}

	VectorSubtract( ent->client->ps.origin, range, mins );
	VectorAdd( ent->client->ps.origin, range, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	// can't use ent->absmin, because that has a one unit pad
	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

	for ( i=0 ; i<num ; i++ ) {
		hit = &g_entities[touch[i]];

		if ( !hit->touch && !ent->touch ) {
			continue;
		}
		if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
			continue;
		}

		// Arnout: invisible entities can't be touched
		// Gordon: radiant tabs arnout! ;)
		if( hit->entstate == STATE_INVISIBLE ||
			hit->entstate == STATE_UNDERCONSTRUCTION ) {
			continue;
		}

		// ignore most entities if a spectator
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			if ( hit->s.eType != ET_TELEPORT_TRIGGER ) {
				continue;
			}
		}

		// use seperate code for determining if an item is picked up
		// so you don't have to actually contact its bounding box
		if ( hit->s.eType == ET_ITEM ) {
			if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) ) {
				continue;
			}
		} else {
			// MrE: always use capsule for player
			if ( !trap_EntityContactCapsule( mins, maxs, hit ) ) {
			//if ( !trap_EntityContact( mins, maxs, hit ) ) {
				continue;
			}
		}

		memset( &trace, 0, sizeof(trace) );

		if ( hit->touch ) {
			hit->touch (hit, ent, &trace);
		}
#ifndef NO_BOT_SUPPORT
		if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
			ent->touch( ent, hit, &trace );
		}
#endif
	}
}

// returns true if a player was found to follow
qboolean G_SpectatorAttackFollow(gentity_t *ent) {
	trace_t tr;
	vec3_t forward, right, up;
	vec3_t start, end;
	gentity_t *vic;

	if(!ent->client)
		return qfalse;

	AngleVectors(ent->client->ps.viewangles, forward, right, up);
	VectorCopy(ent->client->ps.origin, start);
	VectorMA(start, 8192, forward, end);

	G_HistoricalTrace(ent,
		&tr,
		start,
		NULL,
		NULL,
		end,
		ent->s.number,
		MASK_SHOT);
	vic = &g_entities[tr.entityNum];
	if(vic->client) {
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		ent->client->sess.spectatorClient = tr.entityNum;
		return qtrue;
	}
	return qfalse;
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd ) {
	pmove_t	pm;
	gclient_t	*client;
	gentity_t *crosshairEnt = NULL; // rain - #480

	client = ent->client;


	// rain - #480 - sanity check - check .active in case the client sends us
	// something completely bogus
	crosshairEnt = &g_entities[ent->client->ps.identifyClient];

	if (crosshairEnt->inuse && crosshairEnt->client &&
		(ent->client->sess.sessionTeam == crosshairEnt->client->sess.sessionTeam ||
		crosshairEnt->client->ps.powerups[PW_OPS_DISGUISED])) {

		// rain - identifyClientHealth sent as unsigned char, so we
		// can't transmit negative numbers
		if (crosshairEnt->health >= 0)
			ent->client->ps.identifyClientHealth = crosshairEnt->health;
		else
			ent->client->ps.identifyClientHealth = 0;
	}

	if ( client->sess.spectatorState != SPECTATOR_FOLLOW )
	{
		client->ps.pm_type = PM_SPECTATOR;
		//client->ps.pm_type = PM_JETPILOT;

		client->ps.speed = 800;	// was: 400 // faster than normal
		if (client->ps.sprintExertTime)
			client->ps.speed *= 3;	// (SA) allow sprint in free-cam mode


		// OSP - dead players are frozen too, in a timeout
		if((client->ps.pm_flags & PMF_LIMBO) && level.match_pause != PAUSE_NONE) {
			client->ps.pm_type = PM_FREEZE;
		} else if( client->noclip ) {
			client->ps.pm_type = PM_NOCLIP;
		}

		// set up for pmove
		memset (&pm, 0, sizeof(pm));
		pm.ps = &client->ps;
		pm.pmext = &client->pmext;
		pm.character = client->pers.character;
		pm.cmd = *ucmd;
		pm.skill = client->sess.skill;
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;	// spectators can fly through bodies
		pm.trace = trap_TraceCapsuleNoEnts;
		pm.pointcontents = trap_PointContents;

		Pmove( &pm ); // JPW NERVE

		// Rafael - Activate
		// Ridah, made it a latched event (occurs on keydown only)
		if (client->latched_buttons & BUTTON_ACTIVATE)
		{
			Cmd_Activate_f (ent);
		}

		// save results of pmove
		VectorCopy( client->ps.origin, ent->s.origin );

		G_TouchTriggers( ent );
		trap_UnlinkEntity( ent );
	}

	if (ent->flags & FL_NOFATIGUE)
		ent->client->pmext.sprintTime = SPRINTTIME;


	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;

//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;


	// MV clients use these buttons locally for other things
	if(client->pers.mvCount < 1) {
		// attack button cycles through spectators
		if(((client->buttons & BUTTON_ATTACK) &&
			!(client->oldbuttons & BUTTON_ATTACK)) &&
			!(client->buttons & BUTTON_ACTIVATE) &&
			ucmd->upmove == 0) {

			if(client->sess.spectatorState != SPECTATOR_FOLLOW) {
				if(g_spectator.integer & SPECF_FL_CLICK_FOLLOW) {
					if(G_SpectatorAttackFollow(ent))
						return;
					/*if(!(g_spectator.integer & SPECF_FL_MISS_FOLLOW_NEXT))
						return;*/
				}
			}
			Cmd_FollowCycle_f(ent, 1);
			} else if ((client->wbuttons & WBUTTON_ATTACK2) &&
				!(client->oldwbuttons & WBUTTON_ATTACK2) &&
				!(client->buttons & BUTTON_ACTIVATE) &&
				ucmd->upmove == 0)   {
			Cmd_FollowCycle_f(ent, -1);
		}
#ifndef NO_BOT_SUPPORT
		// activate button swaps places with bot
		else if( client->sess.sessionTeam != TEAM_SPECTATOR &&
				( ( client->buttons & BUTTON_ACTIVATE ) && ! ( client->oldbuttons & BUTTON_ACTIVATE ) ) &&
				( g_entities[ent->client->sess.spectatorClient].client ) &&
				( g_entities[ent->client->sess.spectatorClient].r.svFlags & SVF_BOT ) )
		{
			Cmd_SwapPlacesWithBot_f( ent, ent->client->sess.spectatorClient );
		}
#endif
		else if(client->sess.sessionTeam == TEAM_SPECTATOR &&
			client->sess.spectatorState == SPECTATOR_FOLLOW &&
			(((client->buttons & BUTTON_ACTIVATE) &&
			  !(client->oldbuttons & BUTTON_ACTIVATE)) ||
			 ucmd->upmove > 0 ) &&
			G_allowFollow(ent, TEAM_AXIS) &&
			G_allowFollow(ent, TEAM_ALLIES)) {
			
			StopFollowing(ent);
		}
	}
}


/*
=================
ClientInactivityTimer

Returns qfalse if the client is dropped
=================
*/
qboolean ClientInactivityTimer( gclient_t *client ) {
	// OSP - modified
	if( ( g_inactivity.integer == 0 && client->sess.sessionTeam != TEAM_SPECTATOR ) || ( g_spectatorInactivity.integer == 0 && client->sess.sessionTeam == TEAM_SPECTATOR ) ) {
		// give everyone some time, so if the operator sets g_inactivity during
		// gameplay, everyone isn't kicked
		client->inactivityTime = level.time + 60 * 1000;
		client->inactivityWarning = qfalse;
	} else if ( client->pers.cmd.forwardmove || 
		client->pers.cmd.rightmove || 
		client->pers.cmd.upmove ||
		(client->pers.cmd.wbuttons & WBUTTON_ATTACK2) ||
		(client->pers.cmd.buttons & BUTTON_ATTACK) ||
		(client->pers.cmd.wbuttons & WBUTTON_LEANLEFT) ||
		(client->pers.cmd.wbuttons & WBUTTON_LEANRIGHT)
		|| client->ps.pm_type == PM_DEAD ) {

		client->inactivityWarning = qfalse;
		client->inactivityTime = level.time + 1000 *
								 ((client->sess.sessionTeam != TEAM_SPECTATOR) ?
												g_inactivity.integer :
												g_spectatorInactivity.integer);

	} else if ( !client->pers.localClient ) {
		if(level.time > client->inactivityTime && 
			client->inactivityWarning &&
			!G_shrubbot_permission(&g_entities[client-level.clients], SBF_ACTIVITY))
		{
			client->inactivityWarning = qfalse;
			client->inactivityTime = level.time + 60 * 1000;
			if ( level.numConnectedClients < level.maxclients && client->sess.sessionTeam != TEAM_SPECTATOR )
			{
				SetTeam(&g_entities[client - level.clients], "s", qtrue, -1, -1, qfalse);
				client->inactivityTime = level.time + 1000 * (g_spectatorInactivity.integer ? g_spectatorInactivity.integer : 60);
			}
			else
			{
				trap_DropClient(client - level.clients, "Dropped due to inactivity", 0 );
			}
			return(qfalse);
		}
		if ( level.numConnectedClients < level.maxclients && client->sess.sessionTeam != TEAM_SPECTATOR )
		{
			if ( !client->inactivityWarning && level.time > client->inactivityTime - 10000  )
			{
				CPx(client - level.clients, "cp \"^310 seconds until moving to spectator for inactivity!\n\"");
				CPx(client - level.clients, "print \"^310 seconds until moving to spectator for inactivity!\n\"");
				G_Printf("10s inactivity warning issued to: %s\n", client->pers.netname);

				client->inactivityWarning = qtrue;
				client->inactivityTime = level.time + 10000;	// Just for safety
			}
			else
			{
				client->inactivityTime = level.time + 60000;	// we're safe for now
				client->inactivityWarning = qfalse;
			}
		} 
		else if ( !client->inactivityWarning && level.time > client->inactivityTime - 10000 )
		{
			CPx(client - level.clients, "cp \"^310 seconds until inactivity drop!\n\"");
			CPx(client - level.clients, "print \"^310 seconds until inactivity drop!\n\"");
			G_Printf("10s inactivity warning issued to: %s\n", client->pers.netname);

			client->inactivityWarning = qtrue;
			client->inactivityTime = level.time + 10000;	// Just for safety
		}
	}
	return qtrue;
}

/*
==================
ClientTimerActions

Actions that happen once a second
==================
*/

#define CAN_REGENERATE ( client->sess.playerType == PC_MEDIC && !(client->ps.eFlags & EF_DEAD) && !(client->ps.eFlags & EF_POISONED))

void ClientTimerActions( gentity_t *ent, int msec ) {
	gclient_t	*client;
	//gentity_t	*attacker;
	int			regenRate1 = 3;
	int			regenRate2 = 2;	

	client					= ent->client;
	client->timeResidual   += msec;

	while( client->timeResidual >= 1000 ) 
	{
		client->timeResidual -= 1000;

		// Calculate regen rate
		if (g_medics.integer & MEDIC_REGENRATE21) 
		{
			regenRate1 = 2;
			regenRate2 = 1;
		} 
		else if (g_medics.integer & MEDIC_REGENRATE10) 
		{
			regenRate1 = 1;
			regenRate2 = 0;
		}
		
		// jet Pilot - Medic Regeneration 
		// allow all players to regenerate if proper insanity flag is active, and the player is not dead		
		if( CAN_REGENERATE ) 
		{
			if( ent->health < client->ps.stats[STAT_MAX_HEALTH]) 
			{
				ent->health += regenRate1;
				if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.1)
				{
					ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.1;
				}
			} 
			else if( ent->health < client->ps.stats[STAT_MAX_HEALTH] * 1.12) 
			{
				ent->health += regenRate2;
				if( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.12 ) 
				{
					ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.12;
				}
			}
		} 
		else 
		{
			
			if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] && ent->health > 1)
			{
				ent->health--;
			}
		}

	}
	
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gclient_t *client ) {
	client->ps.eFlags &= ~EF_TALK;
	client->ps.eFlags &= ~EF_FIRING;

	// the level will exit when everyone wants to or after timeouts

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = client->pers.cmd.buttons;

//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = client->pers.cmd.wbuttons;
}


void G_FallDamage(gentity_t *ent, int event)
{
	int damage = 0;
	int kb_time = 0;
	gentity_t *victim;

	if ( ent->s.eType != ET_PLAYER ) 
	{
		return;		// not in the player model
	}

	victim = &level.gentities[ent->s.groundEntityNum];
	// groundEntityNum won't be set to the entity number
	// of a wounded player if you landed on one.
	// trace to see if we're on a wounded player.
	if(!victim->client)
	{
		trace_t tr;
		vec3_t start, stop;

		VectorCopy(ent->r.currentOrigin, start);
		VectorCopy(ent->r.currentOrigin, stop);
		stop[2] -= 4;
		trap_Trace (&tr, start, NULL, NULL, stop, ent->s.number, MASK_SHOT);

		victim = &level.gentities[tr.entityNum];		
	}

	switch(event)
	{
		case EV_FALL_NDIE:
			damage = 500;
			break;
		case EV_FALL_DMG_50:
			damage = 50;
			kb_time = 1000;
			break;
		case EV_FALL_DMG_25:
			damage = 25;
			kb_time = 500;
			break;
		case EV_FALL_DMG_15:
			damage = 15;
			kb_time = 250;
			break;
		case EV_FALL_DMG_10:
			damage = 10;
			kb_time = 250;
			break;
		case EV_FALL_SHORT:
			if(g_goombaFlags.integer & GBF_NO_HOP_DAMAGE)
			{
				return;
			}

			if(victim && victim->client && victim->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
				(g_goombaFlags.integer & GBF_NO_HOP_TEAMDAMAGE))
			{

				return;
			}
			break;
		default:
			return;
	}	

	if((!g_goomba.integer || !victim || !victim->client || !victim->takedamage)) 
	{
		if(damage) 
		{
			if(kb_time) 
			{
				ent->client->ps.pm_time = kb_time;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
			// no normal pain sound
			ent->pain_debounce_time = level.time + 200;
			G_Damage(ent, NULL, NULL, NULL, NULL, damage, 0, MOD_FALLING);

			// jet Pilot - win a darwin award for a fatal fall
			if (ent->health <= 0)
			{
				BG_AdjustDarwin( &ent->client->ps, 1 );
			}			
		}
		return;
	}

	// tjw: if we make it this far, do goomba damage to victim
	if((g_goombaFlags.integer & GBF_ENEMY_ONLY) && victim->client->sess.sessionTeam == ent->client->sess.sessionTeam) 
	{
		return;
	}

	if(!damage)
	{
		damage = 5;
	}

	if(kb_time) 
	{
		victim->client->ps.pm_time = kb_time;
		victim->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	}

	// no normal pain sound
	victim->pain_debounce_time = level.time + 200;	

	if((g_goombaFlags.integer & GBF_INSTAGIB) && damage > 5) 
	{
		G_Damage(victim, ent, ent, NULL, NULL, 500, 0, MOD_GOOMBA);
	}
	else 
	{
		G_Damage(victim, ent, ent, NULL, NULL, (damage * g_goomba.integer), 0, MOD_GOOMBA);
	}

	if(damage > 5)
	{
		G_AddEvent(victim, EV_GENERAL_SOUND, G_SoundIndex("sound/jetpilot/goomba.wav"));

		if(!(g_goombaFlags.integer & GBF_NO_SELF_DAMAGE)) 
		{
			// faller has a soft landing
			damage *= 0.2f;
			G_Damage(ent, NULL, NULL, NULL, NULL, damage, 0, MOD_FALLING);
		}
	}
	else 
	{
		G_AddEvent(victim, EV_GENERAL_SOUND, G_SoundIndex("sound/player/land_hurt.wav"));
	}


}


/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence ) {
	int			i;
	int			event;
	gclient_t	*client;

	client = ent->client;

	if ( oldEventSequence < client->ps.eventSequence - MAX_EVENTS )
	{
		oldEventSequence = client->ps.eventSequence - MAX_EVENTS;
	}

	for ( i = oldEventSequence ; i < client->ps.eventSequence ; i++ ) 
	{
		event = client->ps.events[ i & (MAX_EVENTS-1) ];

		switch ( event )
		{
			case EV_FALL_NDIE:
			case EV_FALL_SHORT:
			case EV_FALL_DMG_10:
			case EV_FALL_DMG_15:
			case EV_FALL_DMG_25:
			case EV_FALL_DMG_50:
				G_FallDamage(ent, event);
				ent->client->pmext.shoved = qfalse;
				break;
			case EV_FIRE_WEAPON_MG42:

				if ( g_noAttacklInvul.integer )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				// Gordon: reset player disguise on stealing docs
				ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;

				mg42_fire( ent );

				// Only 1 stats bin for mg42
#ifndef DEBUG_STATS
				if(g_gamestate.integer == GS_PLAYING)
#endif
					ent->client->sess.aWeaponStats[BG_WeapStatForWeapon(WP_MOBILE_MG42)].atts++;

				break;
			case EV_FIRE_WEAPON_MOUNTEDMG42:
				// Gordon: reset player disguise on stealing docs
				ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;

				if ( g_noAttacklInvul.integer )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				mountedmg42_fire( ent );
				// Only 1 stats bin for mg42
#ifndef DEBUG_STATS
				if(g_gamestate.integer == GS_PLAYING)
#endif
					ent->client->sess.aWeaponStats[BG_WeapStatForWeapon(WP_MOBILE_MG42)].atts++;

				break;
			case EV_FIRE_WEAPON_AAGUN:

				if ( g_noAttacklInvul.integer )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				// Gordon: reset player disguise on stealing docs
				ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;

				aagun_fire( ent );
				break;

			case EV_FIRE_WEAPON:
			case EV_FIRE_WEAPONB:
			case EV_FIRE_WEAPON_LASTSHOT:

				if ( g_noAttacklInvul.integer && ( level.time - ent->client->pers.lastSpawnTime ) > 500 )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				FireWeapon( ent );
				break;
			case EV_KICK:

				if ( g_noAttacklInvul.integer )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				Kick_Fire( ent );
				break;
			case EV_THROWKNIFE:

				if ( g_noAttacklInvul.integer )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				ThrowKnife( ent );
				break;
			default:
				break;
		}
	}
}

/*
==============
SendPendingPredictableEvents
==============
*/
void SendPendingPredictableEvents( playerState_t *ps ) {
	/*
	gentity_t *t;
	int event, seq;
	int extEvent, number;

	// if there are still events pending
	if ( ps->entityEventSequence < ps->eventSequence ) {
		// create a temporary entity for this event which is sent to everyone
		// except the client generated the event
		seq = ps->entityEventSequence & (MAX_EVENTS-1);
		event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		// set external event to zero before calling BG_PlayerStateToEntityState
		extEvent = ps->externalEvent;
		ps->externalEvent = 0;
		// create temporary entity for event
		t = G_TempEntity( ps->origin, event );
		number = t->s.number;
		BG_PlayerStateToEntityState( ps, &t->s, qtrue );
		t->s.number = number;
		t->s.eType = ET_EVENTS + event;
		t->s.eFlags |= EF_PLAYER_EVENT;
		t->s.otherEntityNum = ps->clientNum;
		// send to everyone except the client who generated the event
		t->r.svFlags |= SVF_NOTSINGLECLIENT;
		t->r.singleClient = ps->clientNum;
		// set back external event
		ps->externalEvent = extEvent;
	}
	*/
}

// DHM - Nerve
void WolfFindMedic( gentity_t *self ) {
	int i, medic=-1;
	gclient_t	*cl;
	vec3_t	start, end;
//	vec3_t	temp;	// rain - unused
	trace_t	tr;
	float	bestdist=1024, dist;

	self->client->ps.viewlocked_entNum		= 0;
	self->client->ps.viewlocked				= 0;
	self->client->ps.stats[STAT_DEAD_YAW]	= 999;

	VectorCopy( self->s.pos.trBase, start );
	start[2] += self->client->ps.viewheight;

	for( i = 0; i < level.numConnectedClients; i++ ) 
	{
		cl = &level.clients[ level.sortedClients[i] ];

		if( level.sortedClients[i] == self->client->ps.clientNum ) {
			continue;
		}

		if( cl->sess.sessionTeam != self->client->sess.sessionTeam ) {
			continue;
		}

		if( cl->ps.pm_type == PM_DEAD ) {
			continue;
		}

		// zinx - limbo'd players are not PM_DEAD or STAT_HEALTH <= 0.
		// and we certainly don't want to lock to them
		// fix for bug #345
		if( cl->ps.pm_flags & PMF_LIMBO ) {
			continue;
		}

		if( cl->ps.stats[ STAT_HEALTH ] <= 0 ) {
			continue;
		}

		// tjw: ps.stats updated before spawn?
		//if( cl->ps.stats[ STAT_PLAYER_CLASS ] != PC_MEDIC ) {
		if(cl->sess.playerType != PC_MEDIC /*&& cl->sess.skill[SK_MEDIC] < 9*/) {
			continue;
		}

		VectorCopy( g_entities[level.sortedClients[i]].s.pos.trBase, end );
		end[2] += cl->ps.viewheight;

		trap_Trace (&tr, start, NULL, NULL, end, self->s.number, CONTENTS_SOLID);
		if( tr.fraction < 0.95 ) {
			continue;
		}

		VectorSubtract( end, start, end );
		dist = VectorNormalize( end );

		if ( dist < bestdist ) {
			medic = cl->ps.clientNum;
#if 0 // rain - not sure what the point of this is
			vectoangles( end, temp );
			self->client->ps.stats[STAT_DEAD_YAW] = temp[YAW];
#endif
			bestdist = dist;
		}
	}

	if ( medic >= 0 ) {
		self->client->ps.viewlocked_entNum = medic;
		self->client->ps.viewlocked = 7;
	}
}


//void ClientDamage( gentity_t *clent, int entnum, int enemynum, int id );		// NERVE - SMF

//int BG_GetAuraLevels(int auras[], playerState_t *ps);
int	BG_SetAuraLevel(playerState_t *ps, int cls, int value);

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/

extern vec3_t	playerMins, playerMaxs;

vec3_t		jetMins= {-48, -48, -48};
vec3_t		jetMaxs= { 48,  48,  48};

void G_RemoveWeapons ( gentity_t *ent ) {

	COM_BitClear(ent->client->ps.weapons, WP_PANZERFAUST);
	COM_BitClear(ent->client->ps.weapons, WP_FLAMETHROWER);
	COM_BitClear(ent->client->ps.weapons, WP_MOBILE_MG42);
	COM_BitClear(ent->client->ps.weapons, WP_MOBILE_MG42_SET);
	COM_BitClear(ent->client->ps.weapons, WP_MORTAR);
	COM_BitClear(ent->client->ps.weapons, WP_MORTAR_SET);
	COM_BitClear(ent->client->ps.weapons, WP_MORTAR2);
	COM_BitClear(ent->client->ps.weapons, WP_MORTAR2_SET);
	COM_BitClear(ent->client->ps.weapons, WP_FG42);
	COM_BitClear(ent->client->ps.weapons, WP_FG42_SCOPE);
	COM_BitClear(ent->client->ps.weapons, WP_K43);
	COM_BitClear(ent->client->ps.weapons, WP_GARAND);
	COM_BitClear(ent->client->ps.weapons, WP_K43_SCOPE);
	COM_BitClear(ent->client->ps.weapons, WP_GARAND_SCOPE);
	COM_BitClear(ent->client->ps.weapons, WP_STEN);
	COM_BitClear(ent->client->ps.weapons, WP_MP40);
	COM_BitClear(ent->client->ps.weapons, WP_THOMPSON);
	COM_BitClear(ent->client->ps.weapons, WP_KAR98);
	COM_BitClear(ent->client->ps.weapons, WP_GPG40);
	COM_BitClear(ent->client->ps.weapons, WP_CARBINE);
	COM_BitClear(ent->client->ps.weapons, WP_M7);
	COM_BitClear(ent->client->ps.weapons, WP_MOBILE_BROWNING);
	COM_BitClear(ent->client->ps.weapons, WP_MOBILE_BROWNING_SET);
	COM_BitClear(ent->client->ps.weapons, WP_SHOTGUN);
	COM_BitClear(ent->client->ps.weapons, WP_STEN_MKII);
	COM_BitClear(ent->client->ps.weapons, WP_STG44);
	COM_BitClear(ent->client->ps.weapons, WP_BAR);
	COM_BitClear(ent->client->ps.weapons, WP_BAR_SET);
	COM_BitClear(ent->client->ps.weapons, WP_BAZOOKA);
	COM_BitClear(ent->client->ps.weapons, WP_MP34);
	COM_BitClear(ent->client->ps.weapons, WP_COLT);
	COM_BitClear(ent->client->ps.weapons, WP_SILENCED_COLT);
	COM_BitClear(ent->client->ps.weapons, WP_AKIMBO_COLT);
	COM_BitClear(ent->client->ps.weapons, WP_AKIMBO_SILENCEDCOLT);
	COM_BitClear(ent->client->ps.weapons, WP_LUGER);
	COM_BitClear(ent->client->ps.weapons, WP_SILENCER);
	COM_BitClear(ent->client->ps.weapons, WP_AKIMBO_LUGER);
	COM_BitClear(ent->client->ps.weapons, WP_AKIMBO_SILENCEDLUGER);
	COM_BitClear(ent->client->ps.weapons, WP_GRENADE_PINEAPPLE);
	COM_BitClear(ent->client->ps.weapons, WP_GRENADE_LAUNCHER);
	COM_BitClear(ent->client->ps.weapons, WP_VENOM);
}

qboolean G_AvailableAirstrikes( gentity_t* ent );
qboolean G_AvailableArty( gentity_t* ent );

void G_SetIdentifyClient( gentity_t *ent ){
	gentity_t	*other; 
	qboolean	clipOnly=qfalse;
	qboolean	skipAmmo=qfalse;

	other = &g_entities[ent->client->ps.identifyClient];  
	
	if(other->inuse && other->client &&  
		(other->team == ent->team ||  
		other->client->ps.powerups[PW_OPS_DISGUISED])) {

		ent->client->ps.identifyClientHealth = other->health;  

		// jaquboss - info for fieldops
		switch ( other->s.weapon ){
			case WP_AMMO:
			case WP_MEDKIT:
			case WP_PLIERS:
			case WP_SMOKE_MARKER:
			case WP_DYNAMITE:
			case WP_SATCHEL:
			case WP_SATCHEL_DET:
			case WP_SMOKE_BOMB:
			case WP_BINOCULARS:
			case WP_FIREBOLT:
				skipAmmo = qtrue;
				break;
			case WP_KNIFE:
			case WP_KNIFE_KABAR:
				if ( other->client->sess.skill[SK_LIGHT_WEAPONS] < 8 )
					skipAmmo = qtrue;
				else 
					clipOnly = qtrue;
				break;
			case WP_LANDMINE:
			case WP_MEDIC_SYRINGE:
			case WP_MEDIC_ADRENALINE:
			case WP_POISON_SYRINGE:
			case WP_GRENADE_LAUNCHER:
			case WP_GRENADE_PINEAPPLE:
			case WP_FLAMETHROWER:
			case WP_MORTAR:
			case WP_MORTAR_SET:
			case WP_MORTAR2:
			case WP_MORTAR2_SET:
			case WP_PANZERFAUST:	
			case WP_BAZOOKA:	
				clipOnly = qtrue;
				break;
			default:
				break;
		}

		// just send that we want nothing
		if ( skipAmmo ) {
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = -1; 
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = -1; 
		} else if ( clipOnly ) { // send clips as ammo
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = (int)((other->client->ps.ammoclip[BG_FindClipForWeapon(other->s.weapon)] / (float) GetAmmoTableData(other->s.weapon)->maxclip ) * 255); 
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = -1; 
	
			// make sure we send positive values
			if ( ent->client->ps.stats[STAT_IDENTIFYAMMO] < 0 )
				ent->client->ps.stats[STAT_IDENTIFYAMMO] = 0;

		} else {
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = (int)((other->client->ps.ammo[BG_FindAmmoForWeapon(other->s.weapon)] / (float)BG_MaxAmmoForWeapon( other->s.weapon, other->client->sess.skill) ) * 255);
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = (int)((other->client->ps.ammoclip[BG_FindClipForWeapon(other->s.weapon)] / (float) GetAmmoTableData(other->s.weapon)->maxclip ) * 255); 
	
			// make sure we send positive values
			if ( ent->client->ps.stats[STAT_IDENTIFYAMMO] < 0 )
				ent->client->ps.stats[STAT_IDENTIFYAMMO] = 0;

			if ( ent->client->ps.stats[STAT_IDENTIFYCLIP] < 0 )
				ent->client->ps.stats[STAT_IDENTIFYCLIP] = 0;		
		}

		// ok clamp max
		if ( ent->client->ps.stats[STAT_IDENTIFYAMMO] > 255 )
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = 255;

		if ( ent->client->ps.stats[STAT_IDENTIFYCLIP] > 255 )
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = 255;

	}
	else
	{
		ent->client->ps.identifyClient = -1;
		ent->client->ps.identifyClientHealth = 0;
		ent->client->ps.stats[STAT_IDENTIFYAMMO] = -1;
		ent->client->ps.stats[STAT_IDENTIFYCLIP] = -1;
	}
}

void ClientThink_checkWeapons( gentity_t *ent ){
	int i;
	gclient_t	*client = ent->client;

	if ( g_knifeonly.integer && !ent->client->weaponsRemoved ){
		G_RemoveWeapons( ent );
		ent->client->ps.weapon = WP_NONE;
		G_AddEvent( ent, EV_NOAMMO, 0 );
		ent->client->weaponsRemoved = qtrue;
	} else if ( !g_knifeonly.integer && !(jp_insanity.integer & JP_INSANITY_FIREBOLT) ) {
		ent->client->weaponsRemoved = qfalse;
	}

	if ( jp_insanity.integer & JP_INSANITY_FIREBOLT && !ent->client->weaponsRemoved ){
		G_RemoveWeapons( ent );
		AddWeaponToPlayer( client, WP_FIREBOLT, 1, 0, qtrue );
		G_AddEvent( ent, EV_NOAMMO, 0 );
		ent->client->weaponsRemoved = qtrue;
	} else if ( !(jp_insanity.integer & JP_INSANITY_FIREBOLT) && COM_BitCheck(client->ps.weapons, WP_FIREBOLT) ) {
		COM_BitClear(ent->client->ps.weapons, WP_FIREBOLT);
		if ( ent->client->ps.weapon == WP_FIREBOLT ){
			ent->client->ps.weapon = WP_NONE;
			G_AddEvent( ent, EV_NOAMMO, 0 );
		}
		ent->client->weaponsRemoved  = qfalse;
	}
		
	for ( i=0; i< WP_NUM_WEAPONS; i++ ) // check for all weapons if we don't own forbidden one
	{
		qboolean drop=qfalse;

		if ( ent->health <= 0 )
			break;

		if ( ent->client->sess.sessionTeam != TEAM_AXIS && ent->client->sess.sessionTeam != TEAM_ALLIES )
			break;

		if ( ent->client->ps.pm_flags & PMF_FOLLOW )
			break;

		if (COM_BitCheck(client->ps.weapons, i) && G_IsWeaponDisabled(ent, i, qtrue))
		{
			if ( i != WP_GPG40 && i != WP_M7 ){ // jaquboss don't throw these
				qboolean update = qfalse;

				G_DropWeapon( ent, i );

				// unmark us, we no longer use that weapon, that will allow others to keep/pickup it if restriction allows
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

				if( update ) 
					ClientUserinfoChanged( ent - g_entities );

			} else {
				COM_BitClear(ent->client->ps.weapons, i); // just clear them
				if( client->sess.rifleNade != 0 ) {
					client->sess.rifleNade = 0;
					ClientUserinfoChanged( ent - g_entities );
				}
			}
			drop = qtrue;
		}

		if ( COM_BitCheck(client->ps.weapons, WP_MEDIC_ADRENALINE) )
		{
			if ( !(g_medics.integer & MEDIC_NOSELFADREN) && ent->client->sess.playerType == PC_MEDIC )
				goto passed;

			if ( jp_keepAwards.integer & JP_KEEP_ADRENALINE && ent->client->sess.playerType != PC_MEDIC ) 
				goto passed;

			drop = qtrue;
			COM_BitClear(ent->client->ps.weapons, WP_MEDIC_ADRENALINE); 
		}

	passed:

		if ( drop )
			trap_SendServerCommand( ent-g_entities,	"cp \"^1Weapon Restriction^7: Dropping restricted weapons\"" );

	}

}

void G_SendVoiceChat( int clientNum, const char *chatId );

void ClientThink_doPmove ( gentity_t *ent, usercmd_t *ucmd ){
	pmove_t		pm;
	gclient_t	*client = ent->client;
	int			oldEventSequence;


	if((client->ps.eFlags & EF_VIEWING_CAMERA) || level.match_pause != PAUSE_NONE)
	{
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->doubleTap = 0;

		// freeze player (RELOAD_FAILED still allowed to move/look)
		if(level.match_pause != PAUSE_NONE) {
			client->ps.pm_type = PM_FREEZE;
		} 
		else if((client->ps.eFlags & EF_VIEWING_CAMERA))
		{
			VectorClear(client->ps.velocity);
			client->ps.pm_type = PM_FREEZE;
		}
	} 
	else if ( client->noclip )
	{
		client->ps.pm_type = PM_NOCLIP;
	} 
	else if( client->ps.pm_type == PM_PLAYDEAD )
	{
		// no need to change it since it will
		// be adjusted by PM_CheckPlayDead regardless
	}
	else if( client->ps.stats[STAT_HEALTH] <= 0 || client->ps.eFlags & EF_PLAYDEAD )
	{
		client->ps.pm_type = PM_DEAD;
	} 
	else
	{
		if ( client->freezed ) // !freeze
			client->ps.pm_type = PM_FREEZE;
		else
			client->ps.pm_type = PM_NORMAL;
	}

	client->ps.aiState = AISTATE_COMBAT;
	client->ps.gravity = g_gravity.value;

	// set speed
	client->ps.speed = g_speed.value;

	if( client->speedScale )				// Goalitem speed scale
		client->ps.speed *= (client->speedScale * 0.01);

	// set up for pmove
	oldEventSequence = client->ps.eventSequence;

	client->currentAimSpreadScale = (float)client->ps.aimSpreadScale/255.0;

	memset (&pm, 0, sizeof(pm));

	pm.ps			= &client->ps;
	pm.pmext		= &client->pmext;
	pm.character	= client->pers.character;
	pm.cmd			= *ucmd;
	pm.oldcmd		= client->pers.oldcmd;

	// MrE: always use capsule for AI and player
	/*if( client->ps.pm_type == PM_PLAYDEAD || client->ps.eFlags & EF_PLAYDEAD )
		pm.trace		= trap_Trace;
	else*/
		pm.trace		= trap_TraceCapsule;

	// jaquboss - voice chats for alt weapon
	pm.voice		= G_SendVoiceChat;

	if ( pm.ps->pm_type == PM_DEAD )
	{
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
		// DHM-Nerve added:: EF_DEAD is checked for in Pmove functions, but wasn't being set
		//              until after Pmove
		pm.ps->eFlags |= EF_DEAD;
		// dhm-Nerve end
	}
	else 
	{
		pm.tracemask = MASK_PLAYERSOLID;
	}

	if(client->ps.eFlags & EF_POISONED && client->poisonTime < level.time) 
	{
		gentity_t *attacker;
		int damage = 1;
		int	waitTiem = 50;

		attacker = g_entities + client->pmext.poisonerEnt;

		if ( BG_Weapons[WP_POISON_SYRINGE].damage )
			damage =  BG_Weapons[WP_POISON_SYRINGE].damage;

		if ( BG_Weapons[WP_POISON_SYRINGE].spread )
			waitTiem =  BG_Weapons[WP_POISON_SYRINGE].spread;

		damage *= ent->client->pmext.poisonAmnt;

		G_Damage(ent, attacker, attacker, NULL, NULL, damage, 0, MOD_POISON);
		client->poisonTime = level.time + waitTiem; 
	}

	//DHM - Nerve :: We've gone back to using normal bbox traces
	//pm.trace = trap_Trace;
	pm.pointcontents	= trap_PointContents;
	pm.debugLevel		= g_debugMove.integer;
	pm.noFootsteps		= qfalse;

	// jaquboss pmove fixing
	if( client->pers.pmoveFixed ){
		pm.pmove_fixed		= client->pers.pmoveFixed;
		pm.pmove_msec		= client->pers.pmoveMsec;
	} else {
		pm.pmove_fixed		= pmove_fixed.integer;
		pm.pmove_msec		= pmove_msec.integer;
	}

	pm.noWeapClips		= qfalse;

	VectorCopy( client->ps.origin, client->oldOrigin );

	// NERVE - SMF
	pm.gametype				= g_gametype.integer;

	pm.soldierChargeTime	= level.soldierChargeTime[client->sess.sessionTeam-1];
	pm.medicChargeTime		= level.medicChargeTime[client->sess.sessionTeam-1];
	pm.engineerChargeTime	= level.engineerChargeTime[client->sess.sessionTeam-1];
	pm.ltChargeTime			= level.lieutenantChargeTime[client->sess.sessionTeam-1];	
	pm.covertopsChargeTime	= level.covertopsChargeTime[client->sess.sessionTeam-1];

	pm.skill				= client->sess.skill;

	client->pmext.airleft	= ent->client->airOutTime - level.time;

	pm.leadership = qfalse;

	// Gordon: bit hacky, stop the slight lag from client -> server even on locahost, switching back to the weapon you were holding
	//			and then back to what weapon you should have, became VERY noticible for the kar98/carbine + gpg40, esp now i've added the
	//			animation locking
	if( level.time - client->pers.lastSpawnTime < 1000 ) 
		pm.cmd.weapon = client->ps.weapon;

	Pmove( &pm );

	// server cursor hints
	// TAT 1/10/2003 - bots don't need to check for cursor hints
	if ( !(ent->r.svFlags & SVF_BOT) && ent->lastHintCheckTime < level.time )
	{
		G_CheckForCursorHints(ent);
		ent->lastHintCheckTime = level.time + FRAMETIME;
	}

	// DHM - Nerve :: Set animMovetype to 1 if ducking
	if ( ent->client->ps.pm_flags & PMF_DUCKED )
		ent->s.animMovetype = 1;
	else
		ent->s.animMovetype = 0;

	// save results of pmove
	if ( ent->client->ps.eventSequence != oldEventSequence ) {
		ent->eventTime = level.time;
		ent->r.eventTime = level.time;
	}

	//unlagged - smooth clients #2
	// clients no longer do extrapolation if cg_smoothClients is 1, because
	// skip correction is all handled server-side now
	// since that's the case, it makes no sense to store the extra info
	// in the client's snapshot entity, so let's save a little bandwidth
	BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, level.time, /*g_fixedphysics.integer ? qfalse :*/ qtrue );
	//unlagged - smooth clients #2

	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

	VectorCopy (pm.mins, ent->r.mins);
	VectorCopy (pm.maxs, ent->r.maxs);

	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;

	// execute client events
	if(level.match_pause == PAUSE_NONE) {
		ClientEvents( ent, oldEventSequence );
		if ( ent->client->ps.groundEntityNum != ENTITYNUM_NONE )
		{
			if ( !(ent->client->ps.pm_flags & PMF_TIME_LAND) )
				ent->client->pmext.shoved = qfalse;
		}
	}

	// link entity now, after any personal teleporters have been used
	trap_LinkEntity (ent);
	if ( !ent->client->noclip ) {
		G_TouchTriggers( ent );
	}

	// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );

	// touch other objects
	ClientImpacts( ent, &pm );

	// save results of triggers and client events
	if (ent->client->ps.eventSequence != oldEventSequence) {
		ent->eventTime = level.time;
	}

}

void ClientThink_real( gentity_t *ent ) {
	int			msec;
	usercmd_t	*ucmd;
	gclient_t	*client = ent->client;
	int			i;
	
	// don't think if the client is not yet connected (and thus not yet spawned in)
	if (client->pers.connected != CON_CONNECTED) {
		return;
	}

	// tjw: this is causing the segfault associated with antilag 
	//      I'm assuming it's not important since currentAngles never
	//      seem to get used for anything (always 0?)
	// tjw: this is needed to set the correct limitations on tank mg
	//      angles.  added check to make sure ent->tagParent is set
	//      to avoid the crash.
	if(ent->s.eFlags & EF_MOUNTEDTANK && ent->tagParent) {
		client->pmext.centerangles[YAW] = ent->tagParent->r.currentAngles[ YAW ];
		client->pmext.centerangles[PITCH] = ent->tagParent->r.currentAngles[ PITCH ];
	}

	ClientThink_checkWeapons( ent );

// jaquboss althud skill handling
	{
		int rank, clsSkill, playerNum;

		rank = client->sess.rank;
		clsSkill = BG_ClassSkillForClass(client->sess.playerType);

		playerNum = ent->client->ps.clientNum;
		// jaquboss fixed for following
		client->ps.ammo[VERYBIGEXPLOSION]		= g_entities[playerNum].client->sess.skillpoints[SK_BATTLE_SENSE];
		client->ps.ammo[WP_DUMMY_MG42]			= g_entities[playerNum].client->sess.skillpoints[SK_LIGHT_WEAPONS];
		client->ps.ammoclip[VERYBIGEXPLOSION]	= g_entities[playerNum].client->sess.skillpoints[SK_HEAVY_WEAPONS];
		client->ps.ammoclip[WP_DUMMY_MG42]		= g_entities[playerNum].client->sess.skillpoints[clsSkill];
	}

	// jaquboss check for airstrike
	client->ps.ammo[WP_ARTY] = 0;

	if( !G_AvailableAirstrikes( ent ) )
		client->ps.ammo[WP_ARTY] |= NO_AIRSTRIKE;
	if( !G_AvailableArty( ent ) )
		client->ps.ammo[WP_ARTY] |= NO_ARTY;


	// mark the time, so the connection sprite can be removed
	ucmd = &ent->client->pers.cmd;

	ent->client->ps.identifyClient = ucmd->identClient;		// NERVE - SMF

	//unlagged - true ping
	// save the estimated ping in a queue for averaging later
	// we use level.previousTime to account for 50ms lag correction
	// besides, this will turn out numbers more like what players are used to
	// josh: changed it back. People don't like it.
	client->pers.pingsamples[client->pers.samplehead] = 
		level.previousTime +
		client->frameOffset - 
		ucmd->serverTime;
	client->pers.samplehead++;
	if ( client->pers.samplehead >= NUM_PING_SAMPLES ) {
		client->pers.samplehead -= NUM_PING_SAMPLES;
	}

	// initialize the real ping
	if ( g_truePing.integer ) {
		int i, sum = 0;

		// get an average of the samples we saved up
		for ( i = 0; i < NUM_PING_SAMPLES; i++ ) {
			sum += client->pers.pingsamples[i];
		}

		client->pers.realPing = sum / NUM_PING_SAMPLES;
	}
	else 
	{
		// if g_truePing is off, use the normal ping
		client->pers.realPing = client->ps.ping;
	}
//unlagged - true ping

	if(client->warping && g_maxWarp.integer) {
		int frames = (level.framenum - client->lastUpdateFrame);

		if(frames > g_maxWarp.integer)
			frames = g_maxWarp.integer;
		ucmd->serverTime = level.previousTime;
		client->ps.commandTime = level.previousTime -
			    (frames  * (level.time - level.previousTime));
		client->warped = qtrue;
	}

	client->warping = qfalse;


	//unlagged - smooth clients #1
	// keep track of this for later - we'll use this to decide whether or not
	// to send extrapolated positions for this client
	client->lastUpdateFrame = level.framenum;
	//unlagged - smooth clients #1

	//unlagged - true ping
	// make sure the true ping is over 0 - with cl_timenudge it can be less
	if ( client->pers.realPing < 0 ) {
		client->pers.realPing = 0;
	}
	//unlagged - true ping

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 )
		ucmd->serverTime = level.time + 200;

	if ( ucmd->serverTime < level.time - 1000 )
		ucmd->serverTime = level.time - 1000;

	// tjw: save ucmd->serverTime before it gets mutilated further 
	//      tack on 2 server frames time for "50ms built-in lag" 
	client->attackTime = ucmd->serverTime 
		+ (2 *(level.time - level.previousTime)); 

	//josh: Check for auto-mute and unmute if appropriate
	if (client->sess.muted && 
			client->sess.auto_mute_time != -1 &&
			level.time - client->sess.auto_mute_time > 
				g_censorMuteTime.integer*1000) {
		CPx(ent - g_entities, "print \"^5You've been auto-unmuted. Language penalty lifted.\"");
		client->sess.muted = qfalse;
		client->sess.auto_mute_time = -1;
		AP(va("chat \"%s's^7 has been auto-unmuted. Language penalty lifted.\"",  
		ent->client->pers.netname ));
	}
	
	msec = ucmd->serverTime - client->ps.commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		return;
	}

	if ( msec > 200 )
		msec = 200;

	if( client->pers.pmoveFixed )
		ucmd->serverTime = ((ucmd->serverTime + client->pers.pmoveMsec-1) / client->pers.pmoveMsec) * client->pers.pmoveMsec;
	else if ( pmove_fixed.integer )
		ucmd->serverTime = ((ucmd->serverTime + pmove_msec.integer-1) / pmove_msec.integer) * pmove_msec.integer;

	if( client->wantsscore ) {
		G_SendScore( ent );
		client->wantsscore = qfalse;
	}

	//
	// check for exiting intermission
	//
	if ( level.intermissiontime ) {
		ClientIntermissionThink( client );
		return;
	}

	// check for inactivity timer, but never drop the local client of a non-dedicated server
	// OSP - moved here to allow for spec inactivity checks as well
	if ( !ClientInactivityTimer( client ) )
		return;
	
	if( !(ent->r.svFlags & SVF_BOT) && level.time - client->pers.lastCCPulseTime > 2000 ) {
		G_SendMapEntityInfo( ent );
		client->pers.lastCCPulseTime = level.time;
	}

	if( !(ucmd->flags & 0x01) || ucmd->forwardmove || ucmd->rightmove || ucmd->upmove || ucmd->wbuttons || ucmd->doubleTap )
		ent->r.svFlags &= ~(SVF_SELF_PORTAL_EXCLUSIVE|SVF_SELF_PORTAL);

	// spectators don't do much
	// DHM - Nerve :: In limbo use SpectatorThink
	if ( client->sess.sessionTeam == TEAM_SPECTATOR || client->ps.pm_flags & PMF_LIMBO )
	{
		SpectatorThink( ent, ucmd );
		return;
	}

	// bani's flamethrower exploit fix
	if( client->flametime && level.time > client->flametime ) {
		client->flametime = 0;
		ent->r.svFlags &= ~SVF_BROADCAST;
	}

	if( client->ps.pm_type != PM_DEAD && level.timeCurrent - client->pers.lastBattleSenseBonusTime > 45000 ) {
		if( client->combatState != COMBATSTATE_COLD ) {
			if( client->combatState & (1<<COMBATSTATE_KILLEDPLAYER) && client->combatState & (1<<COMBATSTATE_DAMAGERECEIVED) ) {
				G_AddSkillPoints( ent, SK_BATTLE_SENSE, 8.f ); G_DEBUG_ADD_SKILL_POINTS( ent, SK_BATTLE_SENSE, 8.f, "combatstate super-hot" );
			} else if( client->combatState & (1<<COMBATSTATE_DAMAGEDEALT) && client->combatState & (1<<COMBATSTATE_DAMAGERECEIVED) ) {
				G_AddSkillPoints( ent, SK_BATTLE_SENSE, 5.f ); G_DEBUG_ADD_SKILL_POINTS( ent, SK_BATTLE_SENSE, 5.f, "combatstate hot" );
			} else {
				G_AddSkillPoints( ent, SK_BATTLE_SENSE, 2.f ); G_DEBUG_ADD_SKILL_POINTS( ent, SK_BATTLE_SENSE, 2.f, "combatstate warm" );
			}
		}

		client->pers.lastBattleSenseBonusTime = level.timeCurrent;
		client->combatState = COMBATSTATE_COLD;	// cool down again
	}

	// jet Pilot - Team Aura Effects
	//   if a player is within 512 units of a field ops with level 5 signals (or higher), double his sprint recharge

	// first reset auras
	for (i=0; i<NUM_PLAYER_CLASSES; i++){
		ent->client->ps.stats[STAT_TEAMAURA] &= ~(AURABIT_MEDIC | AURABIT_ENGINEER | AURABIT_FIELDOPS);
	}


	for ( i = 0 ; i < level.numConnectedClients; i++ ) 
	{
		gclient_t   *cl			= &level.clients[level.sortedClients[i]];
		int			nLevel		= 0;
		int			aurabit		= 0;
		float		distance	= 0.0f;

		// Only scan players on your team
		if( cl->sess.sessionTeam != client->sess.sessionTeam )
			continue;

		// dont aura yourselfs
		if( ent->s.clientNum == cl->ps.clientNum )
			continue;

		// dont aura from dead
		if( cl->ps.eFlags & EF_DEAD || cl->ps.eFlags & EF_PLAYDEAD )
		{
			continue;
		}
	
		switch (cl->sess.playerType)
		{
			case PC_MEDIC:
				nLevel = cl->sess.skill[SK_MEDIC];
				aurabit = AURABIT_MEDIC;
				break;
			case PC_ENGINEER:
				nLevel = cl->sess.skill[SK_ENGINEER];
				aurabit = AURABIT_ENGINEER;
				break;
			case PC_FIELDOPS:
				nLevel = cl->sess.skill[SK_FIELDOPS];
				aurabit = AURABIT_FIELDOPS;
				break;
			default:
				continue;
		}

		if ( nLevel >= 5 &&
			trap_InPVS( g_entities[level.sortedClients[i]].r.currentOrigin, ent->r.currentOrigin ) )
		{
			distance = Distance(g_entities[level.sortedClients[i]].r.currentOrigin, ent->r.currentOrigin);
			
			if (distance <= 512)
			{				
				ent->client->ps.stats[STAT_TEAMAURA] |= aurabit;
			}
		}
	}

	ClientThink_doPmove( ent, ucmd );

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons = client->buttons & ~client->oldbuttons;

	//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;
	client->latched_wbuttons = client->wbuttons & ~client->oldwbuttons;

	// Rafael - Activate
	// Ridah, made it a latched event (occurs on keydown only)
	if( client->latched_buttons & BUTTON_ACTIVATE ) {
		Cmd_Activate_f( ent );
	}


	if (ent->flags & FL_NOFATIGUE)
		ent->client->pmext.sprintTime = SPRINTTIME;

	G_SetIdentifyClient( ent );


	// check for respawning
	if( client->ps.stats[STAT_HEALTH] <= 0 && !(client->ps.eFlags & EF_PLAYDEAD)) {

		// DHM - Nerve
		WolfFindMedic( ent );

		if ( level.time > ent->reviveTime  )
		{
			ent->reviveTime = level.time + 64;

			if ( ent->reviveVal > 0 )
				ent->reviveVal -= REVIVE_DECAYRATE;

			if ( ent->reviveVal < 0 )
				ent->reviveVal = 0;
		}

		// See if we need to hop to limbo
		if( level.timeCurrent > client->respawnTime && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
			if( ucmd->upmove > 0 ) {
				if( (g_gametype.integer == GT_WOLF_LMS || client->ps.persistant[PERS_RESPAWNS_LEFT] >= 0) && !(ent->r.svFlags & SVF_BOT) ) {
					trap_SendServerCommand( ent-g_entities, "reqforcespawn" );
				} else {
					limbo( ent, ( client->ps.stats[STAT_HEALTH] > GIB_HEALTH ) );
				}
			}

			if((g_forcerespawn.integer > 0 && level.timeCurrent - client->respawnTime > g_forcerespawn.integer * 1000) || client->ps.stats[STAT_HEALTH] <= GIB_HEALTH) {
				limbo(ent, (client->ps.stats[STAT_HEALTH] > GIB_HEALTH));
			}
		}
		return;
	}

	if( level.gameManager && level.timeCurrent - client->pers.lastHQMineReportTime > 20000 ) {	// NOTE: 60 seconds? bit much innit
		if( level.gameManager->s.modelindex && client->sess.sessionTeam == TEAM_AXIS ) {
			if( G_SweepForLandmines( ent->r.currentOrigin, 256.f, TEAM_AXIS ) ) {
				client->pers.lastHQMineReportTime = level.timeCurrent;
				trap_SendServerCommand(ent-g_entities, "cp \"Mines have been reported in this area.\" 1");
			}
		} else if( level.gameManager->s.modelindex2 && client->sess.sessionTeam == TEAM_ALLIES ) {
			if( G_SweepForLandmines( ent->r.currentOrigin, 256.f, TEAM_ALLIES ) ) {
				client->pers.lastHQMineReportTime = level.timeCurrent;
				trap_SendServerCommand(ent-g_entities, "cp \"Mines have been reported in this area.\" 1");
			}
		}
	}

	if(g_debugPlayerHitboxes.integer & 2) {
		gentity_t *head;
		vec3_t maxs;

		VectorCopy(ent->r.maxs, maxs);
		maxs[2] = ClientHitboxMaxZ(ent);
		G_RailBox( ent->r.currentOrigin, ent->r.mins, maxs, tv(0.f,0.f,1.f), ent->s.number );

		head = G_BuildHead(ent);
		G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(0.f,0.f,1.f), head->s.number|HITBOXBIT_HEAD );
		G_FreeEntity( head );
	}

	// perform once-a-second actions
	if(level.match_pause == PAUSE_NONE) {
		ClientTimerActions( ent, msec );
	}
}

/*
==================
ClientThink

A new command has arrived from the client
==================
*/
void ClientThink( int clientNum ) {
	gentity_t *ent;

	ent = g_entities + clientNum;
	ent->client->pers.oldcmd = ent->client->pers.cmd;
	trap_GetUsercmd( clientNum, &ent->client->pers.cmd );

//unlagged - smooth clients #1
	// this is handled differently now
/*
	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
	ent->client->lastCmdTime = level.time;
*/
//unlagged - smooth clients #1

#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer ) 
#endif // ALLOW_GSYNC
	{
		ClientThink_real( ent );
	}

	// if this is the locally playing client, do bot thinks
#ifndef NO_BOT_SUPPORT
	if( bot_enable.integer && !g_dedicated.integer && clientNum == 0 ) {
		BotAIThinkFrame(ent->client->pers.cmd.serverTime);
		level.lastClientBotThink = level.time;
	}
#endif // NO_BOT_SUPPORT
}


void G_RunClient( gentity_t *ent ) {
	// Gordon: special case for uniform grabbing
	if( ent->client->pers.cmd.buttons & BUTTON_ACTIVATE ) {
		Cmd_Activate2_f( ent );
	}

	if( ent->health <= 0 && ent->client->ps.pm_flags & PMF_LIMBO ) {
		if( ent->r.linked ) {
			trap_UnlinkEntity( ent );
		}
	}

#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer )
#endif // ALLOW_GSYNC
	{
		return;
	}

	ent->client->pers.cmd.serverTime = level.time;
	ClientThink_real( ent );
}

/*
==================
SpectatorClientEndFrame

==================
*/
void SpectatorClientEndFrame( gentity_t *ent )
{
	// OSP - specs periodically get score updates for useful demo playback info
	if(/*ent->client->pers.mvCount > 0 &&*/ ent->client->pers.mvScoreUpdate < level.time) {
		ent->client->pers.mvScoreUpdate = level.time + MV_SCOREUPDATE_INTERVAL;
		ent->client->wantsscore = qtrue;
//		G_SendScore(ent);
	}

	// if we are doing a chase cam or a remote view, grab the latest info
	if((ent->client->sess.spectatorState == SPECTATOR_FOLLOW) || (ent->client->ps.pm_flags & PMF_LIMBO)) {
		int clientNum, testtime;
		gclient_t *cl;
		qboolean do_respawn = qfalse; // JPW NERVE

		/*
		G_Printf("(dwRedReinfOffset %d + timeCurrent %d - startTime %d) lastReinforceTime %d\n",
			level.dwRedReinfOffset,
			level.timeCurrent,
			level.startTime,
			ent->client->pers.lastReinforceTime);
		*/
		// Players can respawn quickly in warmup
		if((g_gamestate.integer != GS_PLAYING || jp_insanity.integer & JP_INSANITY_SPAWN) && ent->client->respawnTime <= level.timeCurrent &&
		  ent->client->sess.sessionTeam != TEAM_SPECTATOR) {
			do_respawn = qtrue;
		} else if(ent->client->sess.sessionTeam == TEAM_AXIS) {
			testtime = (level.dwRedReinfOffset + level.timeCurrent - level.startTime) % g_redlimbotime.integer;
			do_respawn = (testtime < ent->client->pers.lastReinforceTime);
			ent->client->pers.lastReinforceTime = testtime;
		}
		else if (ent->client->sess.sessionTeam == TEAM_ALLIES) {
			testtime = (level.dwBlueReinfOffset + level.timeCurrent - level.startTime) % g_bluelimbotime.integer;
			do_respawn = (testtime < ent->client->pers.lastReinforceTime);
			ent->client->pers.lastReinforceTime = testtime;
		}
		/*
		G_Printf("testtime %d lastReinforceTime %d\n\n",
			testtime,
			ent->client->pers.lastReinforceTime);
		*/

		if( g_gametype.integer != GT_WOLF_LMS ) {
			if ( ( g_maxlives.integer > 0 || g_alliedmaxlives.integer > 0 || g_axismaxlives.integer > 0 )
				&& ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
				if( do_respawn ) {
					if( g_maxlivesRespawnPenalty.integer ) {
						if( ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] > 0 ) {
							ent->client->ps.persistant[PERS_RESPAWNS_PENALTY]--;
							do_respawn = qfalse;
						}
					} else {
						do_respawn = qfalse;
					}
				}
			}
		}

		if( g_gametype.integer == GT_WOLF_LMS && g_gamestate.integer == GS_PLAYING ) {
			// Force respawn in LMS when nobody is playing and we aren't at the timelimit yet
			if( !level.teamEliminateTime &&
				level.numTeamClients[0] == level.numFinalDead[0] && level.numTeamClients[1] == level.numFinalDead[1] &&
				ent->client->respawnTime <= level.timeCurrent && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
				do_respawn = qtrue;
			} else {
				do_respawn = qfalse;
			}
		}

		if ( do_respawn ) {
			reinforce(ent);
			return;
		}

		// Limbos aren't following while in MV
		if((ent->client->ps.pm_flags & PMF_LIMBO) && ent->client->pers.mvCount > 0) {
			return;
		}

		clientNum = ent->client->sess.spectatorClient;

		// team follow1 and team follow2 go to whatever clients are playing
		if ( clientNum == -1 && ( ent->client->ps.pm_flags & PMF_LIMBO )) {
			clientNum = level.follow1;
		} else if ( clientNum == -2 && ( ent->client->ps.pm_flags & PMF_LIMBO )) {
			clientNum = level.follow2;
		}

		if ( clientNum >= 0 ) {
			cl = &level.clients[ clientNum ];
			if(cl->pers.connected == CON_CONNECTED && cl->sess.sessionTeam != TEAM_SPECTATOR) {
				int flags = (cl->ps.eFlags & ~(EF_VOTED)) | (ent->client->ps.eFlags & (EF_VOTED));
				int ping = ent->client->ps.ping;

				if(ent->client->sess.sessionTeam != TEAM_SPECTATOR &&
					(ent->client->ps.pm_flags & PMF_LIMBO)) {

					int savedScore			= ent->client->ps.persistant[PERS_SCORE];
					int savedRespawns		= ent->client->ps.persistant[PERS_RESPAWNS_LEFT];
					int savedRespawnPenalty = ent->client->ps.persistant[PERS_RESPAWNS_PENALTY];
//					int savedClass			= ent->client->ps.stats[STAT_PLAYER_CLASS];
					int savedMVList			= ent->client->ps.powerups[PW_MVCLIENTLIST];

					do_respawn = ent->client->ps.pm_time;

					ent->client->ps = cl->ps;
					ent->client->ps.pm_time = do_respawn;							// put pm_time back
					ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = savedRespawns;
					ent->client->ps.persistant[PERS_RESPAWNS_PENALTY] = savedRespawnPenalty;
					ent->client->ps.persistant[PERS_SCORE] = savedScore;			// put score back
					ent->client->ps.powerups[PW_MVCLIENTLIST] = savedMVList;
				//	ent->client->ps.stats[STAT_PLAYER_CLASS] = savedClass;			// jaquboss - WTF? this tells clients that they are diffirent class, it may fuck up chargebar
					ent->client->ps.pm_flags |= PMF_FOLLOW;
					ent->client->ps.pm_flags |= PMF_LIMBO;

				} else {
					ent->client->ps = cl->ps;
					ent->client->ps.pm_flags |= PMF_FOLLOW;
				}

				// DHM - Nerve :: carry flags over
				ent->client->ps.eFlags = flags;
				ent->client->ps.ping = ping;

				return;
			} else {
				// drop them to free spectators unless they are dedicated camera followers
				if ( ent->client->sess.spectatorClient >= 0 ) {
					ent->client->sess.spectatorState = SPECTATOR_FREE;
					ClientBegin( ent->client - level.clients );
				}
			}
		}
	}

	// we are at a free-floating spec state for a player,
	// set speclock status, as appropriate
	//	 --> Can we use something besides a powerup slot?
	if(ent->client->pers.mvCount < 1) {
		ent->client->ps.powerups[PW_BLACKOUT] = (G_blockoutTeam(ent, TEAM_AXIS) * TEAM_AXIS) |
												(G_blockoutTeam(ent, TEAM_ALLIES) * TEAM_ALLIES);
	}
}


// DHM - Nerve :: After reviving a player, their contents stay CONTENTS_CORPSE until it is determined
//					to be safe to return them to PLAYERSOLID

qboolean StuckInClient( gentity_t *self ) {
	int i;
	vec3_t	hitmin, hitmax;
	vec3_t	selfmin, selfmax;
	gentity_t *hit;

	for(i=0; i<level.numConnectedClients; i++) {
		hit = g_entities + level.sortedClients[i];

		if(!hit->inuse || hit == self || !hit->client ||
		  !hit->s.solid || hit->health <= 0) {
			continue;
		}

		VectorAdd(hit->r.currentOrigin, hit->r.mins, hitmin);
		VectorAdd(hit->r.currentOrigin, hit->r.maxs, hitmax);
		VectorAdd(self->r.currentOrigin, self->r.mins, selfmin);
		VectorAdd(self->r.currentOrigin, self->r.maxs, selfmax);

		if(hitmin[0] > selfmax[0]) continue;
		if(hitmax[0] < selfmin[0]) continue;
		if(hitmin[1] > selfmax[1]) continue;
		if(hitmax[1] < selfmin[1]) continue;
		if(hitmin[2] > selfmax[2]) continue;
		if(hitmax[2] < selfmin[2]) continue;

		return(qtrue);
	}

	return(qfalse);
}


#define WR_PUSHAMOUNT 25

void WolfRevivePushEnt( gentity_t *self, gentity_t *other ) {
	vec3_t	dir, push;

	VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
	dir[2] = 0;
	VectorNormalizeFast( dir );

	VectorScale( dir, WR_PUSHAMOUNT, push );

	if ( self->client ) {
		VectorAdd( self->s.pos.trDelta, push, self->s.pos.trDelta );
		VectorAdd( self->client->ps.velocity, push, self->client->ps.velocity );
	}

	VectorScale( dir, -WR_PUSHAMOUNT, push );
	push[2] = WR_PUSHAMOUNT/2;

	VectorAdd( other->s.pos.trDelta, push, other->s.pos.trDelta );
	VectorAdd( other->client->ps.velocity, push, other->client->ps.velocity );
}

// Arnout: completely revived for capsules
void WolfReviveBbox( gentity_t *self ) {
	int			touch[MAX_GENTITIES];
	int			num,i, touchnum=0;
	gentity_t	*hit = NULL; // TTimo: init
	vec3_t		mins, maxs;

	hit = G_TestEntityPosition( self );

	if( hit && ( hit->s.number == ENTITYNUM_WORLD || ( hit->client && (hit->client->ps.persistant[PERS_HWEAPON_USE] || (hit->client->ps.eFlags & EF_MOUNTEDTANK))) ) ) {
		G_DPrintf( "WolfReviveBbox: Player stuck in world or MG42 using player\n" );
		// Move corpse directly to the person who revived them
		if ( self->props_frame_state >= 0 ) {
//			trap_UnlinkEntity( self );
			VectorCopy( g_entities[self->props_frame_state].client->ps.origin, self->client->ps.origin );
			VectorCopy( self->client->ps.origin, self->r.currentOrigin );
			trap_LinkEntity( self );

			// Reset value so we don't continue to warp them
			self->props_frame_state = -1;
		}
		return;
	}

	VectorAdd( self->r.currentOrigin, playerMins, mins );
	VectorAdd( self->r.currentOrigin, playerMaxs, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) {
		hit = &g_entities[touch[i]];

		// Always use capsule for player
		if ( !trap_EntityContactCapsule( mins, maxs, hit ) ) {
		//if ( !trap_EntityContact( mins, maxs, hit ) ) {
			continue;
		}

		if ( hit->client && hit->health > 0 ) {
			if ( hit->s.number != self->s.number ) {
				WolfRevivePushEnt( hit, self );
				touchnum++;
			}
		} else if ( hit->r.contents & ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_PLAYERCLIP ) ) {
			WolfRevivePushEnt( hit, self );
			touchnum++;
		}
	}

	G_DPrintf( "WolfReviveBbox: Touchnum: %d\n", touchnum );

	if ( touchnum == 0 ) {
		G_DPrintf( "WolfReviveBbox:  Player is solid now!\n" );
		self->r.contents = CONTENTS_BODY;
	}
}

/*
==============
ClientEndFrame

Called at the end of each server frame for each connected client
A fast client will have multiple ClientThink for each ClientEndFrame,
while a slow client may have multiple ClientEndFrame between ClientThink.
==============
*/
void ClientEndFrame( gentity_t *ent ) {
	int			i;
//unlagged - smooth clients #1
	int frames;

	if((g_XPDecay.integer & XPDF_ENABLE) && !(level.time % 60000)) {  
		G_XPDecay(ent, 60, qfalse);  
	}  

	// forty - in mod flood protection 
	// forty - if they have been quiet for over a second start decrementing 
	//      intermission time) 
	if( level.time >= (ent->client->nextReliableTime + 1000) &&
		ent->client->numReliableCmds ) { 

			ent->client->numReliableCmds--; 

		// forty - reset the threshold because they were good for a bit 
		if(!ent->client->numReliableCmds) 
			ent->client->thresholdTime=0; 
	} 


	// used for informing of speclocked teams.
	// Zero out here and set only for certain specs
	ent->client->ps.powerups[PW_BLACKOUT] = 0;

	if (( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) || (ent->client->ps.pm_flags & PMF_LIMBO)) { // JPW NERVE
		SpectatorClientEndFrame( ent );
		return;
	}

		// turn off any expired powerups
		// OSP -- range changed for MV
	for ( i = 0 ; i < PW_NUM_POWERUPS ; i++ ) {

		if(	i == PW_BREATHER ||
			i == PW_NOFATIGUE ||
			ent->client->ps.powerups[i] == 0		// OSP
			|| i == PW_OPS_CLASS_1
			|| i == PW_OPS_CLASS_2
			|| i == PW_OPS_CLASS_3
			|| i == PW_OPS_DISGUISED
			) {
			continue;
		}
		// OSP -- If we're paused, update powerup timers accordingly.
		// Make sure we dont let stuff like CTF flags expire.
		if(level.match_pause != PAUSE_NONE &&
		  ent->client->ps.powerups[i] != INT_MAX) {
			ent->client->ps.powerups[i] += level.time - level.previousTime;
		}

		if ( ent->client->ps.powerups[ i ] < level.time ) {
			ent->client->ps.powerups[ i ] = 0;
		}
	}

	ent->client->ps.stats[STAT_XP] = 0;

	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		ent->client->ps.stats[STAT_XP] += ent->client->sess.skillpoints[i];
	}

	// OSP - If we're paused, make sure other timers stay in sync
	//		--> Any new things in ET we should worry about?
	if(level.match_pause != PAUSE_NONE) 
	{
		int time_delta = level.time - level.previousTime;

		ent->client->airOutTime += time_delta;
		ent->client->inactivityTime += time_delta;
		ent->client->lastBurnTime += time_delta;
		ent->client->pers.connectTime += time_delta;
		ent->client->pers.enterTime += time_delta;
		ent->client->pers.teamState.lastreturnedflag += time_delta;
		ent->client->pers.teamState.lasthurtcarrier += time_delta;
		ent->client->pers.teamState.lastfraggedcarrier += time_delta;
		ent->client->ps.classWeaponTime += time_delta;
		ent->lastHintCheckTime += time_delta;
		ent->pain_debounce_time += time_delta;
		ent->s.onFireEnd += time_delta;
	}


	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if ( level.intermissiontime ) {
		return;
	}

	// burn from lava, etc
	P_WorldEffects (ent);

	// apply all the damage taken this frame
	P_DamageFeedback (ent);
	
	ent->client->ps.stats[STAT_HEALTH] = ent->health; 

	G_SetClientSound (ent);

	// set the latest infor

	BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, level.time, qfalse );

	// DHM - Nerve :: If it's been a couple frames since being revived, and props_frame_state
	//					wasn't reset, go ahead and reset it
	if ( ent->props_frame_state >= 0 && ( (level.time - ent->s.effect3Time) > 100 ) )
		ent->props_frame_state = -1;

	if ( ent->health > 0 && StuckInClient( ent ) ) {
		G_DPrintf( "%s is stuck in a client.\n", ent->client->pers.netname );
		ent->r.contents = CONTENTS_CORPSE;
	}

	if ( ent->health > 0 && 
		ent->r.contents == CONTENTS_CORPSE && 
		!(ent->s.eFlags & EF_MOUNTEDTANK) &&
		!(ent->s.eFlags & EF_PLAYDEAD)
		) {
		WolfReviveBbox( ent );
	}

	// DHM - Nerve :: Reset 'count2' for flamethrower
	if ( !(ent->client->buttons & BUTTON_ATTACK) )
		ent->count2 = 0;
	// dhm

	// zinx - #280 - run touch functions here too, so movers don't have to wait
	// until the next ClientThink, which will be too late for some map
	// scripts (railgun)
	G_TouchTriggers( ent );

	// run entity scripting
	G_Script_ScriptRun( ent );

	// mark as not missing updates initially
	ent->client->ps.eFlags &= ~EF_CONNECTION;

	// see how many frames the client has missed
	frames = level.framenum - ent->client->lastUpdateFrame - 1;
	
	if(g_maxWarp.integer && frames > g_maxWarp.integer) 
		ent->client->warping = qtrue;

	if(g_skipCorrection.integer && !ent->client->warped && frames > 0)
	{
		if (frames > 3) 
		{	// josh: I need frames to be = 2 here
			frames = 3;
			// these are disabled because the phone jack can give
			// away other players position through walls.
			ent->client->ps.eFlags |= EF_CONNECTION;
			ent->s.eFlags |= EF_CONNECTION;
		}
		G_PredictPmove(ent, (float)frames / sv_fps.integer);
	}
	ent->client->warped = qfalse;

//unlagged - smooth clients #1

	// zinx - realistic hitboxes
    mdx_PlayerAnimation( ent );

	if(g_debugPlayerHitboxes.integer & 1 ) {
		gentity_t *head;
		vec3_t maxs;

		VectorCopy(ent->r.maxs, maxs);
		maxs[2] = ClientHitboxMaxZ(ent);
		G_RailBox( ent->r.currentOrigin, ent->r.mins, maxs, tv(0.f,1.f,0.f), ent->s.number);

		head = G_BuildHead(ent);
		G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(0.f,1.f,0.f), ent->s.number|HITBOXBIT_HEAD);
		G_FreeEntity( head );
	}

	// josh: moved over from ClientThink see the note there
	// We want this to track the server's viewpoint
	G_StoreClientPosition( ent );
}

