/*
 * name:		g_active.c
 *
 * desc:
 *
 */
#include "g_local.h"
#include "etpro_mdx.h"

#ifdef PROFILER
#include "bg_profiler.h"
#endif

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
void P_DamageFeedback( gentity_t *player )
{
	gclient_t	*client = player->client;
	float	count;

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
	}
	else {
		vec3_t	angles;

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

	// clear totals
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
void P_WorldEffects( gentity_t *ent )
{
	int			waterlevel;

	if ( ent->client->noclip ) {
		if ( ent->client->sess.skillBits[SK_BATTLE_SENSE] & (1<<6) )  {
			ent->client->airOutTime = level.time + HOLDBREATHTIME_LONG;
			// core: air left while underwater..
			ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
			ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME_LONG;
		}
		else {
			ent->client->airOutTime = level.time + HOLDBREATHTIME;
			// core: air left while underwater..
			ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
			ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME;
		}
		return;
	}

	waterlevel = ent->waterlevel;

	// check for drowning
	if ( waterlevel == 3 ) {
		// if out of air, start drowning
		if ( ent->client->airOutTime < level.time) {
			// drown!
			ent->client->airOutTime += SECONDS_1;
			if ( ent->health > 0 ) {
				// take more damage the longer underwater
				ent->damage += 2;
				if (ent->damage > 15) {
					ent->damage = 15;
				}

				// play a gurp sound instead of a normal pain sound
				if (ent->health <= ent->damage) {
				//	G_Sound(ent, G_SoundIndex("*drown.wav"));
				}
				else if (rand()&1) {
					G_Sound(ent, GAMESOUND_PLAYER_GURP1);

				}
				else {
					G_Sound(ent, GAMESOUND_PLAYER_GURP2);
				}

				// don't play a normal pain sound
				ent->pain_debounce_time = level.time + 200;

				G_Damage (ent, NULL, NULL, NULL, NULL, ent->damage, 0, MOD_WATER);
			}
		}
	}
	else {
		if ( ent->client->sess.skillBits[SK_BATTLE_SENSE] & (1<<6) ) {
			ent->client->airOutTime = level.time + HOLDBREATHTIME_LONG;
			// core: air left while underwater..
			ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
			ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME_LONG;
		}
		else {
			ent->client->airOutTime = level.time + HOLDBREATHTIME;
			// core: air left while underwater..
			ent->client->pmext.airleft	= ent->client->airOutTime - level.time;
			ent->client->ps.stats[STAT_AIRLEFT] = HOLDBREATHTIME;
		}
		ent->damage = 2;
	}


	// check for sizzle damage (move to pmove?)
	if (waterlevel && (ent->watertype&CONTENTS_LAVA) ) {
		if (ent->health > 0	&& ent->pain_debounce_time <= level.time ) {

				if (ent->watertype & CONTENTS_LAVA) {
					G_Damage (ent, NULL, NULL, NULL, NULL,
						30*waterlevel, 0, MOD_LAVA);
				}

		}
	}


	// check for burning from flamethrower
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
				G_Damage (ent, attacker, attacker, NULL, NULL, BG_Weapons[WP_FLAMETHROWER].damage , DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER); // JPW NERVE was 7
			}
		}
	}
}

#ifdef OMNIBOTS
/*
==============
PushBot
==============
*/
void PushBot( gentity_t *ent, gentity_t *other )
{
	vec3_t dir, ang, f, r;
	float oldspeed;
	float s;

	// dont push when mounted in certain stationary weapons or scripted not to be pushed
	if(other->client)
	{
		if (Bot_Util_AllowPush(other->client->ps.weapon) == qfalse || !other->client->sess.botPush)
			return;
	}

	oldspeed = VectorLength( other->client->ps.velocity );
	if (oldspeed < 200) {
		oldspeed = 200;
	}

	VectorSubtract( other->r.currentOrigin, ent->r.currentOrigin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ang );
	AngleVectors( ang, f, r, NULL );
	f[2] = 0;
	r[2] = 0;

	VectorMA( other->client->ps.velocity, 200, f, other->client->ps.velocity );
	s = 100 * ((level.time+(ent->s.number*SECONDS_1))%SECONDS_4 < SECONDS_2 ? 1.0 : -1.0);
	VectorMA( other->client->ps.velocity, s, r, other->client->ps.velocity );

	if (VectorLengthSquared( other->client->ps.velocity ) > SQR(oldspeed)) {
		VectorNormalize( other->client->ps.velocity );
		VectorScale( other->client->ps.velocity, oldspeed, other->client->ps.velocity );
	}
}
#endif

/*
==============
ClientNeedsAmmo
==============
*/
qboolean ClientNeedsAmmo( int client )
{
	return AddMagicAmmo( &g_entities[client], 0 ) ? qtrue : qfalse;
}

/*
==============
ReadyToCallArtillery

	Does ent have enough "energy" to call artillery?
==============
*/
qboolean ReadyToCallArtillery( gentity_t* ent )
{
	if( ent->client->sess.skillBits[SK_SIGNALS] & (1<<2) ) {
		if( level.time - ent->client->ps.classWeaponTime <= (level.lieutenantChargeTime[ent->client->sess.sessionTeam-1]*0.66f) )
			return qfalse;
	}
	else if( level.time - ent->client->ps.classWeaponTime <= level.lieutenantChargeTime[ent->client->sess.sessionTeam-1] ) {
		return qfalse;
	}

	return qtrue;
}

/*
==============
ReadyToConstruct

	Are we ready to construct?  Optionally, will also update the time while we are constructing
==============
*/
qboolean ReadyToConstruct(gentity_t *ent, gentity_t *constructible, qboolean updateState)
{
	int weaponTime = ent->client->ps.classWeaponTime;

	// "Ammo" for this weapon is time based
	if( weaponTime + level.engineerChargeTime[ent->client->sess.sessionTeam-1] < level.time ) {
		weaponTime = level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1];
	}

	if( g_debugConstruct.integer ) {
		weaponTime += 0.5f*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)SERVER_FRAMETIME_DELTA));
	}
	else {
		weaponTime += BG_GetFromTable(chargeCost_Construct, ent->client->sess.skillBits, SK_ENGINEER )*
			constructible->constructibleStats.chargebarreq*((float)level.engineerChargeTime[ent->client->sess.sessionTeam-1]/(constructible->constructibleStats.duration/(float)SERVER_FRAMETIME_DELTA));
	}

	// if the time is in the future, we have NO energy left
	if (weaponTime > level.time) {
		return qfalse;
	}

	// only set the actual weapon time for this entity if they want us to
	if( updateState ) {
		ent->client->ps.classWeaponTime = jp_insanity.integer & JP_INSANITY_CHARGEBAR ?
			level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1] : weaponTime;
	}

	return qtrue;
}
//==============================================================

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm )
{
	int		i, j;
	gentity_t	*other;
	trace_t	trace;

	memset( &trace, 0, sizeof(trace) );
	for (i = 0; i < pm->numtouch; ++i) {
		for (j = 0; j < i; ++j) {
			if (pm->touchents[j] == pm->touchents[i] ) {
				break;
			}
		}
		if (j != i) {
			continue;	// duplicated
		}
		other = &g_entities[ pm->touchents[i] ];

#ifdef OMNIBOTS
		if ( (ent->client) /*&& !(ent->r.svFlags & SVF_BOT)*/ && (other->r.svFlags & SVF_BOT) &&
			!other->client->ps.powerups[PW_INVULNERABLE] ) {
			PushBot( ent, other );
		}

		// if we are standing on their head, then we should be pushed also
		if ( (ent->r.svFlags & SVF_BOT) && (ent->s.groundEntityNum == other->s.number && other->client) &&
			!other->client->ps.powerups[PW_INVULNERABLE]) {
			PushBot( other, ent );
		}
#endif

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
void G_TouchTriggers( gentity_t *ent )
{
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

	for ( i = 0 ; i < num ; ++i ) {
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
		}
		else {
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
	}
}

/*
=================
G_SpectatorAttackFollow

returns true if a player was found to follow
=================
*/
qboolean G_SpectatorAttackFollow(gentity_t *ent)
{
	trace_t			tr;
	vec3_t			forward, right, up;
	vec3_t			start, end;
	vec3_t			mins, maxs;
	static vec3_t	enlargeMins = { -64.0f, -64.0f, -48.0f };
	static vec3_t	enlargeMaxs = { 64.0f, 64.0f, 0.0f };

	if(!ent->client) {
		return qfalse;
	}

	AngleVectors(ent->client->ps.viewangles, forward, right, up);
	VectorCopy(ent->client->ps.origin, start);
	VectorMA(start, MAX_TRACE, forward, end);

	// core: enlarge the hitboxes, so spectators can more easily click on them..
	VectorCopy(ent->r.mins, mins);
	VectorCopy(ent->r.maxs, maxs);
	VectorAdd(mins, enlargeMins, mins);
	VectorAdd(maxs, enlargeMaxs, maxs);
	// also put the start-point a bit forward, so we don't start the trace in solid..
	VectorMA(start, 75.0f, forward, start);

	trap_Trace(&tr, start, mins, maxs, end, ent->client->ps.clientNum, CONTENTS_BODY|CONTENTS_CORPSE);

	if((&g_entities[tr.entityNum])->client) {
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
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd )
{
	gclient_t *client = ent->client;
	gentity_t *crosshairEnt = &g_entities[ent->client->ps.identifyClient];

	// rain - #480 - sanity check - check .active in case the client sends us
	// something completely bogus

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

	if ( client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		pmove_t	pm;

		client->ps.pm_type = PM_SPECTATOR;

		client->ps.speed = 800;	// was: 400 // faster than normal
		if (client->ps.sprintExertTime) {
			client->ps.speed *= 3;	// (SA) allow sprint in free-cam mode
		}

		// OSP - dead players are frozen too, in a timeout
		if((client->ps.pm_flags & PMF_LIMBO) && level.match_pause != PAUSE_NONE) {
			client->ps.pm_type = PM_FREEZE;
		}
		else if( client->noclip ) {
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
		if (client->latched_buttons & BUTTON_ACTIVATE) {
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


#ifdef MV_SUPPORT
	// MV clients use these buttons locally for other things
	if(client->pers.mvCount < 1) {
#endif
		// attack button + "firing" a players = spectate that "victim"
		if((client->buttons & BUTTON_ATTACK) &&
			!(client->oldbuttons & BUTTON_ATTACK) &&
			client->sess.spectatorState != SPECTATOR_FOLLOW &&
			client->sess.sessionTeam == TEAM_SPECTATOR ) { // cs: don't do it if on a team. #467
				if(G_SpectatorAttackFollow(ent)) {
					return;
				}
				else {
					// not clicked on a player?.. then follow next,
					// to prevent constant traces done by server.
					if ( client->buttons & BUTTON_SPRINT ) {
						Cmd_FollowCycle_f(ent, 1, qtrue);
					}
					// no humans playing?.. then follow a bot
					if ( client->sess.spectatorState != SPECTATOR_FOLLOW ) {
						Cmd_FollowCycle_f(ent, 1, qfalse);
					}
				}
		// attack + sprint button cycles through non-bot/human players
		// attack button cycles through all players
		}
		else if ((client->buttons & BUTTON_ATTACK) &&
			!(client->oldbuttons & BUTTON_ATTACK) &&
			!(client->buttons & BUTTON_ACTIVATE) ) {
				Cmd_FollowCycle_f(ent, 1, (client->buttons & BUTTON_SPRINT) );
		}
		else if ( (client->wbuttons & WBUTTON_ATTACK2) &&
			!(client->oldwbuttons & WBUTTON_ATTACK2) &&
			!(client->buttons & BUTTON_ACTIVATE) ) {
				Cmd_FollowCycle_f(ent, -1, (client->buttons & BUTTON_SPRINT) );
		}
#ifdef DEBUG
#ifdef OMNIBOTS
		// activate button swaps places with bot
		else if( client->sess.sessionTeam != TEAM_SPECTATOR && g_allowBotSwap.integer &&
				( ( client->buttons & BUTTON_ACTIVATE ) && ! ( client->oldbuttons & BUTTON_ACTIVATE ) ) &&
				( g_entities[ent->client->sess.spectatorClient].client ) &&
				( g_entities[ent->client->sess.spectatorClient].r.svFlags & SVF_BOT ) )
		{
			Cmd_SwapPlacesWithBot_f( ent, ent->client->sess.spectatorClient );
		}
#endif
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
#ifdef MV_SUPPORT
	}
#endif
}


/*
=================
ClientInactivityTimer - function reworked by IlDuca

Returns qfalse if the client is dropped

Note:
g_inactivity and g_spectatorinactivity :
Values have to be higher then 10 seconds, that is the time after the warn message is sent.
(if it's lower than that value, it will not work at all)
=================
*/
qboolean ClientInactivityTimer( gclient_t *client )
{
	int			inactivity = (g_inactivity.integer)? g_inactivity.integer : 60;
	int			inactivityspec = (g_spectatorInactivity.integer)? g_spectatorInactivity.integer : 60;
	qboolean	inTeam = (client->sess.sessionTeam == TEAM_ALLIES || client->sess.sessionTeam == TEAM_AXIS)? qtrue : qfalse;

#ifdef OMNIBOTS
	qboolean	doDrop = (g_spectatorInactivity.integer && (g_maxclients.integer - level.numNonSpectatorClients + g_OmniBotPlaying.integer <= 0))? qtrue : qfalse;
#else
	qboolean	doDrop = (g_spectatorInactivity.integer && (g_maxclients.integer - level.numNonSpectatorClients <= 0))? qtrue : qfalse;
#endif

	// inactivity settings disabled?
	if ( g_inactivity.integer == 0 && g_spectatorInactivity.integer == 0 ) {
		// Give everyone some time, so if the operator sets g_inactivity or g_spectatorinactivity during
		// gameplay, everyone isn't kicked or moved to spectators
		client->inactivityTime = level.time + SECONDS_60;
		client->inactivityWarning = qfalse;
		return qtrue;
	}

	// the client is still active?
	if ( client->pers.cmd.forwardmove || client->pers.cmd.rightmove || client->pers.cmd.upmove ||
	     client->pers.cmd.wbuttons & (WBUTTON_ATTACK2 | WBUTTON_LEANLEFT | WBUTTON_LEANRIGHT)  ||
	     client->pers.cmd.buttons & BUTTON_ATTACK ||
	     client->ps.eFlags & (EF_MOUNTEDTANK | EF_MG42_ACTIVE) ||
		 (client->ps.pm_type == PM_DEAD && !(client->ps.eFlags & EF_PLAYDEAD)) ) { // cs: playdead sets PM_DEAD, so check if playing dead ...

		client->inactivityWarning = qfalse;

		if ( inTeam ) {
			client->inactivityTime = level.time + SECONDS_1 * inactivity;
		}
		else {
			client->inactivityTime = level.time + SECONDS_1 * inactivityspec;
		}
		return qtrue;
	}

	// No inactivity for localhost and shrubbot-set clients
	if ( client->pers.localClient || G_shrubbot_permission(&g_entities[client-level.clients], SBF_ACTIVITY)) {
		return qtrue;
	}

	// start displaying the initial warning when the countdown is halfway (1000/2=500)..
	if ( !client->inactivityWarning ) {
		if ( g_inactivity.integer &&
			(level.time > client->inactivityTime - SECOND_HALF * inactivity) &&
			inTeam )
		{
			CPx(client - level.clients, va("cp \"^3%i seconds until moving to spectator for inactivity!\n\"", inactivity));
			CPx(client - level.clients, va("print \"^3%i seconds until moving to spectator for inactivity!\n\"", inactivity));
			G_Printf("Inactivity warning issued to: %s\n", client->pers.netname);
			client->inactivityWarning = qtrue;
			client->inactivityTime = level.time + SECOND_HALF * inactivity;
			client->inactivitySecondsLeft = inactivity;
		}
		// if a player will not be kicked from the server (because there are still free slots),
		// do not display messages for inactivity-drop/kick.
		else if ( doDrop && g_spectatorInactivity.integer &&
				(level.time > client->inactivityTime - SECOND_HALF * inactivityspec) &&
	  			!inTeam )
		{
			CPx(client - level.clients, va("cp \"^3%i seconds until inactivity drop!\n\"", inactivityspec));
			CPx(client - level.clients, va("print \"^3%i seconds until inactivity drop!\n\"", inactivityspec));
			G_Printf("Spectator Inactivity warning issued to: %s\n", client->pers.netname);
			client->inactivityWarning = qtrue;
			client->inactivityTime = level.time + SECOND_HALF * inactivityspec;
			client->inactivitySecondsLeft = inactivityspec;
		}
	}
	else {
		if ( level.time > client->inactivityTime ) {
			if ( inTeam ) {
				SetTeam(&g_entities[client - level.clients], "s", qtrue, -1, -1, qfalse);
				client->inactivityWarning = qfalse;
				client->inactivityTime = level.time + SECONDS_1 * inactivityspec;
				client->inactivitySecondsLeft = inactivityspec;
			}
			else {
				// core: Slots occupied by bots should be considered "free",
				// because bots will disconnect if a human player connects..
				if ( doDrop ) {
					trap_DropClient(client - level.clients, "Dropped due to inactivity", 0 );
					return qfalse;
				}
			}
		}
		else {
			qboolean	showMsg = qfalse;
			int			secondsLeft = (client->inactivityTime - level.time) / SECONDS_1;

			// a second has passed since last message was displayed??
			if ( secondsLeft != client->inactivitySecondsLeft ) {
				client->inactivitySecondsLeft = secondsLeft;
				if ( secondsLeft <= 0 ) {
					// countdown expired..
					showMsg = qtrue;
				}
				// wait at least half the (inactivity)timeout, before sending new messages.
				// to prevent players seeing messages immediately when they don't move anymore..
				else if ( inTeam && g_inactivity.integer && (secondsLeft > (g_inactivity.integer >> 1)) ) {	// integer >>1 == /2 == *0.5
					return qtrue;
				}
				else if ( !inTeam && g_spectatorInactivity.integer && (secondsLeft > (g_spectatorInactivity.integer >> 1)) ) {
					return qtrue;
				}
				// display a message every second..
				else if ( secondsLeft < 15 ) {
					showMsg = qtrue;
				}
				// display a message every 5 seconds..
				else if ( secondsLeft < 30 && secondsLeft % 5 == 0 ) {
					showMsg = qtrue;
				}
				// display a message every 10 seconds..
				else if ( secondsLeft < 60 && secondsLeft % 10 == 0 ) {
					showMsg = qtrue;
				}
				// display a message every 30 seconds..
				else if ( secondsLeft % 30 == 0 ) {
					showMsg = qtrue;
				}
			}
			if ( showMsg ) {
				// give a few more messages, so people can see the time counting down..
				if ( g_inactivity.integer && inTeam ) {
					if ( secondsLeft <= 0 ) {
						CPx(client - level.clients, "cp \"^3moved to spectator for inactivity!\n\"");
					}
					else {
						CPx(client - level.clients, va("cp \"^3%i seconds until moving to spectator for inactivity!\n\"", secondsLeft));
					}
				}
				else if ( g_spectatorInactivity.integer && !inTeam && doDrop ) {
					if ( secondsLeft <= 0 ) {
						CPx(client - level.clients, "cp \"^3dropped for inactivity!\n\"");
					}
					else {
						CPx(client - level.clients, va("cp \"^3%i seconds until inactivity drop!\n\"", secondsLeft));
					}
				}
			}
		}
	}

	// do not kick by default..
	return qtrue;
}

/*
==================
ClientTimerActions

Actions that happen once a second
==================
*/
void ClientTimerActions( gentity_t *ent, int msec )
{
	gclient_t	*client = ent->client;
	int			regenRate1 = 3;
	int			regenRate2 = 2;
	qboolean	canRegenerate = (client->sess.playerType == PC_MEDIC && !(client->ps.eFlags & (EF_DEAD|EF_POISONED)) && !(g_medics.integer & MEDIC_NOREGENERATION));

	client->timeResidual   += msec;

	while( client->timeResidual >= SECONDS_1 ) {
		client->timeResidual -= SECONDS_1;

		// jet Pilot - Medic Regeneration
		// allow all players to regenerate if proper insanity flag is active, and the player is not dead
		if ( canRegenerate ) {
			float extraHP = ( g_medics.integer & MEDIC_EXTRAHP )? 1.12f : 1.0f;
			float maxHP = client->ps.stats[STAT_MAX_HEALTH] * extraHP;

			// Calculate regen rate
			if ( g_medics.integer & MEDIC_NOREGENERATION ) {
				regenRate1 = 0;
				regenRate2 = 0;
			}
			else if ( g_medics.integer & MEDIC_REGENRATE21 ) {
				regenRate1 = 2;
				regenRate2 = 1;
			}
			else if ( g_medics.integer & MEDIC_REGENRATE10 ) {
				regenRate1 = 1;
				regenRate2 = 0;
			}

			if( ent->health < client->ps.stats[STAT_MAX_HEALTH]) {
				ent->health += regenRate1;
				if ( ent->health > maxHP) {
					ent->health = maxHP;
				}
			}
			else {
				if( ent->health < maxHP) {
					ent->health += regenRate2;
					if( ent->health > maxHP ) {
						ent->health = maxHP;
					}
				}
			}
		}
		else {
			if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] && ent->health > 1) {
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
void ClientIntermissionThink( gclient_t *client )
{
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

	if ( ent->s.eType != ET_PLAYER ) {
		return;		// not in the player model
	}

	victim = &level.gentities[ent->s.groundEntityNum];
	// groundEntityNum won't be set to the entity number
	// of a wounded player if you landed on one.
	// trace to see if we're on a wounded player.
	if(!victim->client) {
		trace_t tr;
		vec3_t start, stop;

		VectorCopy(ent->r.currentOrigin, start);
		VectorCopy(ent->r.currentOrigin, stop);
		stop[2] -= 4;
		trap_Trace (&tr, start, NULL, NULL, stop, ent->s.number, MASK_SHOT);

		victim = &level.gentities[tr.entityNum];
	}

	switch(event) {
		case EV_FALL_NDIE:
			damage = 500;
			break;
		case EV_FALL_DMG_50:
			damage = 50;
			kb_time = SECONDS_1;
			break;
		case EV_FALL_DMG_25:
			damage = 25;
			kb_time = SECOND_HALF;
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
			if( victim && victim->client ) {
				qboolean onSameTeam = OnSameTeam(ent,victim);

				if ( (onSameTeam && (g_goombaFlags.integer & GBF_NO_HOP_TEAMDAMAGE)) ||
				(!onSameTeam && (g_goombaFlags.integer & GBF_NO_HOP_ENEMYDAMAGE)) ) {
					return;
				}
			}
			break;
		default:
			return;
	}

	if((!g_goomba.integer || !victim || !victim->client || !victim->takedamage)) {
		if(damage) {
			if(kb_time) {
				ent->client->ps.pm_time = kb_time;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
			// no normal pain sound
			ent->pain_debounce_time = level.time + 200;
			G_Damage(ent, NULL, NULL, NULL, NULL, damage, 0, MOD_FALLING);

			// jet Pilot - win a darwin award for a fatal fall
			if (ent->health <= 0) {
				ent->client->sess.darwin++;
			}
		}
		return;
	}

	// tjw: if we make it this far, do goomba damage to victim
	if((g_goombaFlags.integer & GBF_ENEMY_ONLY) && victim->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
		return;
	}

	if(!damage) {
		damage = 5;
	}

	if(kb_time) {
		victim->client->ps.pm_time = kb_time;
		victim->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	}

	// no normal pain sound
	victim->pain_debounce_time = level.time + 200;

	if (g_goombaFlags.integer & GBF_INSTAGIB) {
		G_Damage(victim, ent, ent, NULL, NULL, 500, 0, MOD_GOOMBA);
	}
	else {
		G_Damage(victim, ent, ent, NULL, NULL, (damage * g_goomba.integer), 0, MOD_GOOMBA);
	}

	if(damage > 5) {
		G_AddEvent(victim, EV_GENERAL_SOUND, G_SoundIndex("sound/jetpilot/goomba.wav"));

		if(!(g_goombaFlags.integer & GBF_NO_SELF_DAMAGE)) {
			// faller has a soft landing
			damage *= 0.2f;
			G_Damage(ent, NULL, NULL, NULL, NULL, damage, 0, MOD_FALLING);
		}
	}
}

/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence )
{
	int			i;
	int			event;
	gclient_t	*client = ent->client;

	if ( oldEventSequence < client->ps.eventSequence - MAX_EVENTS ) {
		oldEventSequence = client->ps.eventSequence - MAX_EVENTS;
	}

	for ( i = oldEventSequence ; i < client->ps.eventSequence ; ++i ) {
		event = client->ps.events[ i & (MAX_EVENTS-1) ];

		switch ( event ) {
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
					ent->client->sess.aWeaponStats[WS_MG42].atts++;

				break;
			case EV_FIRE_WEAPON_MOUNTEDMG42:
				// Gordon: reset player disguise on stealing docs
				ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;

				if ( g_noAttacklInvul.integer ) {
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;
				}

				mountedmg42_fire( ent );

				// Only 1 stats bin for mg42
#ifndef DEBUG_STATS
				if(g_gamestate.integer == GS_PLAYING)
#endif
					ent->client->sess.aWeaponStats[WS_MG42].atts++;

				break;
			case EV_FIRE_WEAPON:
			case EV_FIRE_WEAPONB:
			case EV_FIRE_WEAPON_LASTSHOT:

				if ( g_noAttacklInvul.integer && ( level.time - ent->client->pers.lastSpawnTime ) > SECOND_HALF )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				// jet Pilot - Test covert ops disguise against loss
				// IRATA - moved from g_active FireWeapon to centralize PW_OPS_DISGUISED code - fixes jp_insanity panzer cops loose disguise too
				if (ent->client->ps.powerups[PW_OPS_DISGUISED] && !jP_CheckDisguise( ent ))
				{
					ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
				}

				FireWeapon( ent );
				break;
			case EV_SHOTGUN_FIRED:

				if ( g_noAttacklInvul.integer && ( level.time - ent->client->pers.lastSpawnTime ) > SECOND_HALF )
					ent->client->ps.powerups[PW_INVULNERABLE] = 0;

				// IRATA - if cops use shotgun in jp_insanity
				if (ent->client->ps.powerups[PW_OPS_DISGUISED] && !jP_CheckDisguise( ent )) {
					ent->client->ps.powerups[PW_OPS_DISGUISED] = 0;
				}
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


// DHM - Nerve
void WolfFindMedic( gentity_t *self )
{
	int i, medic = -1;
	gclient_t	*cl;
	vec3_t	start, end;
	trace_t	tr;
	float	bestdist = 1024, dist;

	self->client->ps.viewlocked_entNum		= 0;
	self->client->ps.viewlocked				= VIEWLOCK_NONE;
	self->client->ps.stats[STAT_DEAD_YAW]	= 999;

	VectorCopy( self->s.pos.trBase, start );
	start[2] += self->client->ps.viewheight;

	for( i = 0; i < level.numConnectedClients; ++i ) {
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
		if(cl->sess.playerType != PC_MEDIC ) {
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
			bestdist = dist;
		}
	}

	if ( medic >= 0 ) {
		self->client->ps.viewlocked_entNum = medic;
		self->client->ps.viewlocked = VIEWLOCK_MEDIC;
	}
}

// core: Killer cam
void KillerCam( gentity_t *self ) {
	gclient_t	*killer;
	int			duration = g_killerLockTime.integer;

	if ( duration <= 1 ) {
		return;
	}
	// killercam will last 1 second minimum & 20 seconds maximum..
	if (duration > 20) {
		duration = 20;
	}

	// if the cam is already locked, don't use the killercam..
	if ( self->client->ps.viewlocked ) {
		return;
	}

	// cam expired?..
	if ( !self->client->pmext.viewlocked_killer ) {
		return;
	}

	// is there a last killer?..
	if ( self->client->pers.lastkiller_client == -1 ) {
		return;
	}

	// get the killer..
	killer = &level.clients[ self->client->pers.lastkiller_client ];
	if ( !g_entities[self->client->pers.lastkiller_client].inuse || !g_entities[self->client->pers.lastkiller_client].client ) {
		return;
	}

	if( self->client->pers.lastkiller_client == self->client->ps.clientNum ) {
		return;
	}

	if( killer->ps.pm_type == PM_DEAD ) {
		return;
	}

	// zinx - limbo'd players are not PM_DEAD or STAT_HEALTH <= 0.
	// and we certainly don't want to lock to them
	if( killer->ps.pm_flags & PMF_LIMBO ) {
		return;
	}

	if( killer->ps.stats[ STAT_HEALTH ] <= 0 ) {
		return;
	}

	self->client->ps.viewlocked_entNum = killer->ps.clientNum;
	self->client->ps.viewlocked = VIEWLOCK_KILLER;
	// check if the killercam has expired..
	if ( self->client->pmext.viewlocked_time ) {
		if ( level.time >= self->client->pmext.viewlocked_time ) {
			// cam expired..
			self->client->ps.viewlocked = VIEWLOCK_NONE;
			self->client->pmext.viewlocked_time = 0;
			self->client->pmext.viewlocked_killer = 0;
		}
		return;
	}

	self->client->pmext.viewlocked_time = level.time + (duration * SECONDS_1);
}




int	BG_SetAuraLevel(playerState_t *ps, int cls, int value);

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
	COM_BitClear(ent->client->ps.weapons, WP_FG42SCOPE);
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
	COM_BitClear(ent->client->ps.weapons, WP_JOHNSON);
	COM_BitClear(ent->client->ps.weapons, WP_JOHNSON_SCOPE);
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
	// remove really all except the knives
	COM_BitClear(ent->client->ps.weapons, WP_BINOCULARS);
	COM_BitClear(ent->client->ps.weapons, WP_POISON_SYRINGE);
	COM_BitClear(ent->client->ps.weapons, WP_PLIERS);
	COM_BitClear(ent->client->ps.weapons, WP_DYNAMITE);
	COM_BitClear(ent->client->ps.weapons, WP_LANDMINE);
	COM_BitClear(ent->client->ps.weapons, WP_MEDIC_ADRENALINE);
	COM_BitClear(ent->client->ps.weapons, WP_MEDIC_SYRINGE);
	COM_BitClear(ent->client->ps.weapons, WP_MEDKIT);
	COM_BitClear(ent->client->ps.weapons, WP_AMMO);
	COM_BitClear(ent->client->ps.weapons, WP_ARTY);
	COM_BitClear(ent->client->ps.weapons, WP_SMOKE_MARKER);
	COM_BitClear(ent->client->ps.weapons, WP_SMOKE_BOMB);
	COM_BitClear(ent->client->ps.weapons, WP_SATCHEL);
}

qboolean G_AvailableAirstrikes( gentity_t* ent );
qboolean G_AvailableArty( gentity_t* ent );

void G_SetIdentifyClient( gentity_t *ent )
{
	gentity_t	*other = &g_entities[ent->client->ps.identifyClient];
	qboolean	clipOnly = qfalse;
	qboolean	skipAmmo = qfalse;

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
				skipAmmo = qtrue;
				break;
			case WP_KNIFE:
			case WP_KNIFE_KABAR:
				if ( !(other->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<8)) )
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
		}
		else if ( clipOnly ) { // send clips as ammo
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = -1;
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = (int)((other->client->ps.ammoclip[WeaponTable[other->s.weapon].clipindex] / (float)GetWeaponTableData(other->s.weapon)->maxclip ) * 255);

			// make sure we send positive values
			if ( ent->client->ps.stats[STAT_IDENTIFYAMMO] < 0 )
				ent->client->ps.stats[STAT_IDENTIFYAMMO] = 0;

		}
		else {
			ent->client->ps.stats[STAT_IDENTIFYAMMO] = (int)((other->client->ps.ammo[WeaponTable[other->s.weapon].ammoindex] / (float)BG_MaxAmmoForWeapon( other->s.weapon, other->client->sess.skillBits, other->client->sess.playerType) ) * 255);
			ent->client->ps.stats[STAT_IDENTIFYCLIP] = (int)((other->client->ps.ammoclip[WeaponTable[other->s.weapon].clipindex] / (float)GetWeaponTableData(other->s.weapon)->maxclip ) * 255);

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
	else {
		ent->client->ps.identifyClient = -1;
		ent->client->ps.identifyClientHealth = 0;
		ent->client->ps.stats[STAT_IDENTIFYAMMO] = -1;
		ent->client->ps.stats[STAT_IDENTIFYCLIP] = -1;
	}
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/
void ClientThink_checkWeapons( gentity_t *ent )
{
	int			i;
	gclient_t	*client = ent->client;
	int			weapon = WP_NONE;
	qboolean	update = qfalse;
	qboolean	doWar = (nq_War.integer & WARMODE_ENABLE)? qtrue : qfalse;

	if ( !ent->client->weaponsRemoved && doWar ) {

		// disable/remove all weapons..
		ent->client->ps.weapons[0] = 0;
		ent->client->ps.weapons[1] = 0;
		for ( i = WP_KNIFE; i < WP_NUM_WEAPONS; ++i ) {
			ent->client->ps.ammo[i] = 0;
			ent->client->ps.ammoclip[i] = 0;
#ifdef OMNIBOTS
			// cs: this lets bots know they have lost the weapon
			Bot_Event_RemoveWeapon(ent-g_entities, Bot_WeaponGameToBot(i));
#endif
		}

		// Also disable flack jackets, helmetshields, heavy_burns & binocs.
		ent->client->ps.powerups[PW_FLAKJACKET] = 0;
		ent->client->ps.powerups[PW_HELMETSHIELD] = 0;
		ent->client->ps.powerups[PW_HEAVY_BURNS] = 0;
		ent->client->ps.stats[STAT_KEYS] &= ~(1 << INV_BINOCS);

		// spectators get nothing (except when they are bot)..
		if ( ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES
			|| (ent->r.svFlags & SVF_BOT) )
		{

			// smoke grenades
			if ( nq_War.integer & WARMODE_SMOKE ) {
				weapon = WP_SMOKE_BOMB;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// binoculars
			if ( nq_War.integer & WARMODE_BINOCS ) {
				weapon = WP_BINOCULARS;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1;
				ent->client->ps.ammoclip[weapon] = 1;
				ent->client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// pliers
			if ( nq_War.integer & WARMODE_PLIERS ) {
				weapon = WP_PLIERS;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1;
				ent->client->ps.ammoclip[weapon] = 1;
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
				// core: also give a dynamite, so pliers make sense.. :-)
				weapon = WP_DYNAMITE;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 0;
				ent->client->ps.ammoclip[weapon] = 1;
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// poison
			if ( nq_War.integer & WARMODE_POISON ) {
				weapon = WP_POISON_SYRINGE;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// knives
			if ( nq_War.integer & WARMODE_KNIFE ) {
				weapon = (ent->client->sess.sessionTeam == TEAM_AXIS) ? WP_KNIFE : WP_KNIFE_KABAR;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				// cs: this lets bots know they have recieved the weapon
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// grenades
			if ( nq_War.integer & WARMODE_NADE ) {
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// flamethrowers
			if ( nq_War.integer & WARMODE_FLAMER ) {
				weapon = WP_FLAMETHROWER;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 9999;
				ent->client->ps.ammoclip[weapon] = 9999;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// panzers
			if ( nq_War.integer & WARMODE_PANZER ) {
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_PANZERFAUST : WP_BAZOOKA;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// sniperguns
			if ( nq_War.integer & WARMODE_SNIPER ) {
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_K43_SCOPE : WP_GARAND_SCOPE;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 10;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_K43 : WP_GARAND;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 10;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// rifles
			if ( nq_War.integer & WARMODE_RIFLE ) {
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_KAR98 : WP_CARBINE;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 10;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
				weapon = ( ent->client->sess.sessionTeam == TEAM_AXIS )? WP_GPG40 : WP_M7;
				COM_BitSet(ent->client->ps.weapons, weapon);
				// note: Do not give more than 1 grenade in ammoclip,
				// because that will make the 1st-person playermodel dismount the grenade after every shot..
				// The player will get a new grenade (in weapon_gpg40_fire()), so he'll never run out of ammo.
				ent->client->ps.ammo[weapon] = 0;
				ent->client->ps.ammoclip[weapon] = 1;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}
			// shotguns
			if ( nq_War.integer & WARMODE_SHOTGUN ) {
				weapon = WP_SHOTGUN;
				COM_BitSet(ent->client->ps.weapons, weapon);
				ent->client->ps.ammo[weapon] = 1000;
				ent->client->ps.ammoclip[weapon] = 1000;
#ifdef OMNIBOTS
				Bot_Event_AddWeapon(ent-g_entities, Bot_WeaponGameToBot(weapon));
#endif
			}

			if ( weapon != WP_NONE ) {
				ent->client->sess.latchPlayerWeapon = weapon;
				ent->client->sess.playerWeapon = weapon;
				ent->client->ps.weapon = weapon;
				ent->client->ps.weaponstate = WEAPON_READY;
				update = qtrue;
			}

			if ( update ) {
				ClientConfigStringChanged( ent );
			}

			G_AddEvent( ent, EV_WEAPONSWITCHED, 0 );
		}
		ent->client->weaponsRemoved = qtrue;
	}

	if ( doWar ) {
		// core: in warmode no weapon is disabled/restricted..
		return;
	}

	if ( ent->health <= 0 ) {
		return;
	}

	if ( ent->client->sess.sessionTeam != TEAM_AXIS && ent->client->sess.sessionTeam != TEAM_ALLIES ) {
		return;
	}

	if ( ent->client->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	if ( COM_BitCheck(client->ps.weapons, WP_MEDIC_ADRENALINE) ) {
		qboolean clearBit = qtrue;
		switch(ent->client->sess.playerType) {
			case PC_SOLDIER:
				clearBit = !(g_adrenaline.integer & ADF_SOLDIER);
				break;
			case PC_MEDIC:
				clearBit = !(g_adrenaline.integer & ADF_MEDIC);
				break;
			case PC_ENGINEER:
				clearBit = !(g_adrenaline.integer & ADF_ENGINEER);
				break;
			case PC_FIELDOPS:
				clearBit = !(g_adrenaline.integer & ADF_FIELDOPS);
				break;
			case PC_COVERTOPS:
				clearBit = !(g_adrenaline.integer & ADF_COVERTOPS);
				break;
		}
		if (clearBit) COM_BitClear(ent->client->ps.weapons, WP_MEDIC_ADRENALINE);
	}
}

void G_SendVoiceChat( int clientNum, const char *chatId );

void ClientThink_doPmove ( gentity_t *ent, usercmd_t *ucmd ){
	pmove_t		pm;
	gclient_t	*client = ent->client;
	int			oldEventSequence;

	if(level.match_pause != PAUSE_NONE) { // intermission/pause ...
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->doubleTap = 0;

		// freeze player (RELOAD_FAILED still allowed to move/look)
		// IRATA: no need to check twice
		//if(level.match_pause != PAUSE_NONE) {
		client->ps.pm_type = PM_FREEZE;
		//}
	}
	else if ( client->noclip ) { // not clipped
		client->ps.pm_type = PM_NOCLIP;
	}
	else if( client->ps.pm_type == PM_PLAYDEAD ) { // playdead
		// no need to change it since it will
		// be adjusted by PM_CheckPlayDead regardless
	}
	else if( client->ps.stats[STAT_HEALTH] <= 0 || client->ps.eFlags & EF_PLAYDEAD ) { // dead
		client->ps.pm_type = PM_DEAD;
	}
	else { // in game
		if ( client->freezed ) { // !freeze
			client->ps.pm_type = PM_FREEZE;
		}
		else {
			client->ps.pm_type = PM_NORMAL;
		}
	}

	client->ps.aiState = AISTATE_COMBAT;
	client->ps.gravity = g_gravity.value;

	// set speed
	client->ps.speed = g_speed.value;

	if( client->speedScale ) {				// Goalitem speed scale
		client->ps.speed *= (client->speedScale * 0.01);
	}

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
	pm.trace		= trap_TraceCapsule;

	// jaquboss - voice chats for alt weapon
	pm.voice		= G_SendVoiceChat;

	if ( pm.ps->pm_type == PM_DEAD ) {
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
		// DHM-Nerve added:: EF_DEAD is checked for in Pmove functions, but wasn't being set
		//              until after Pmove
		pm.ps->eFlags |= EF_DEAD;
		// dhm-Nerve end
	}
	else {
		pm.tracemask = MASK_PLAYERSOLID;
	}

	if(client->ps.eFlags & EF_POISONED && client->poisonTime < level.time) {
		gentity_t *attacker;
		int damage = BG_Weapons[WP_POISON_SYRINGE].damage * ent->client->pmext.poisonAmnt;

		attacker = g_entities + client->pmext.poisonerEnt;
		G_Damage(ent, attacker, attacker, NULL, NULL, damage, 0, MOD_POISON);
		client->poisonTime = level.time + 50;
	}

	//DHM - Nerve :: We've gone back to using normal bbox traces
	//pm.trace = trap_Trace;
	pm.pointcontents	= trap_PointContents;
	pm.debugLevel		= g_debugMove.integer;

	// jaquboss pmove fixing
	if( client->pers.pmoveFixed ){
		pm.pmove_fixed		= client->pers.pmoveFixed;
		pm.pmove_msec		= client->pers.pmoveMsec;
	}
	else {
		pm.pmove_fixed		= pmove_fixed.integer;
		pm.pmove_msec		= pmove_msec.integer;
	}

	VectorCopy( client->ps.origin, client->oldOrigin );

	// NERVE - SMF
	pm.gametype				= g_gametype.integer;

	pm.soldierChargeTime	= level.soldierChargeTime[client->sess.sessionTeam-1];
	pm.medicChargeTime		= level.medicChargeTime[client->sess.sessionTeam-1];
	pm.engineerChargeTime	= level.engineerChargeTime[client->sess.sessionTeam-1];
	pm.ltChargeTime			= level.lieutenantChargeTime[client->sess.sessionTeam-1];
	pm.covertopsChargeTime	= level.covertopsChargeTime[client->sess.sessionTeam-1];

	pm.skill				= client->sess.skill;

	memcpy( pm.skillBits,  client->sess.skillBits, sizeof(pm.skillBits));

	client->pmext.airleft	= ent->client->airOutTime - level.time;
	// core: air left while underwater..
	client->ps.stats[STAT_AIRLEFT] = client->pmext.airleft;


	// Gordon: bit hacky, stop the slight lag from client -> server even on locahost, switching back to the weapon you were holding
	//			and then back to what weapon you should have, became VERY noticible for the kar98/carbine + gpg40, esp now i've added the
	//			animation locking
	if( level.time - client->pers.lastSpawnTime < SECONDS_1 )
		pm.cmd.weapon = client->ps.weapon;

	Pmove( &pm );

	// server cursor hints
	// TAT 1/10/2003 - bots don't need to check for cursor hints
	if ( !(ent->r.svFlags & SVF_BOT) && ent->lastHintCheckTime < level.time ) {
		G_CheckForCursorHints(ent);
		ent->lastHintCheckTime = level.time + SERVER_FRAMETIME_DELTA;
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
	BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, level.time, qtrue );
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
		if ( ent->client->ps.groundEntityNum != ENTITYNUM_NONE ) {
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

void ClientThink_real( gentity_t *ent )
{
	int			msec;
	usercmd_t	*ucmd;
	gclient_t	*client = ent->client;
	qboolean	doAntiWarp = G_DoAntiwarp(ent);

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
	// note: This hack is done to see skill-bars change at realtime..
	client->ps.ammo[VERYBIGEXPLOSION]		= client->sess.skillpoints[SK_BATTLE_SENSE];
	client->ps.ammo[WP_DUMMY_MG42]			= client->sess.skillpoints[SK_LIGHT_WEAPONS];
	client->ps.ammoclip[VERYBIGEXPLOSION]	= client->sess.skillpoints[SK_HEAVY_WEAPONS];
	client->ps.ammoclip[WP_DUMMY_MG42]		= client->sess.skillpoints[BG_ClassSkillForClass(client->sess.playerType)];

	// jaquboss check for airstrike
	client->ps.ammo[WP_ARTY] = 0;
	if( !G_AvailableAirstrikes( ent ) )
		client->ps.ammo[WP_ARTY] |= NO_AIRSTRIKE;
	if( !G_AvailableArty( ent ) )
		client->ps.ammo[WP_ARTY] |= NO_ARTY;

	// mark the time, so the connection sprite can be removed
	ucmd = &ent->client->pers.cmd;

	ent->client->ps.identifyClient = ucmd->identClient;		// NERVE - SMF


	// Do the g_trueping code only if g_trueping is set and if it's a real player
	if ( g_truePing.integer && !(ent->r.svFlags & SVF_BOT)) {
		//unlagged - true ping
		// save the estimated ping in a queue for averaging later
		// we use level.previousTime to account for 50ms lag correction
		// besides, this will turn out numbers more like what players are used to
		// josh: changed it back. People don't like it.

		int newsample = level.previousTime + client->frameOffset - ucmd->serverTime;
		int oldindex = client->pers.samplehead++;	// increase the index afterwards..
		int oldsample = client->pers.pingsamples[oldindex];

		// if samplehead >= NUM_PING_SAMPLES then samplehead -= NUM_PING_SAMPLES;
		// Is the same as doing: samplehead %= NUM_PING_SAMPLES;
		// Which can also be done like this (eliminates a branch, because there is no more "if"): samplehead &= (NUM_PING_SAMPLES-1)
		// note: this method requires NUM_PING_SAMPLES to have a value that is a power of 2.
		// note: modulo will most likely perform a division, while an AND-instruction does not..
		/*
		if ( client->pers.samplehead >= NUM_PING_SAMPLES ) {
			client->pers.samplehead -= NUM_PING_SAMPLES;
		}
		*/
		client->pers.samplehead &= (NUM_PING_SAMPLES-1);

		client->pers.pingsamples[oldindex] = newsample;
		client->pers.pingsamplessum += (newsample - oldsample); // sum = sum - oldsample, sum = sum + newsample
		client->pers.realPing = client->pers.pingsamplessum / NUM_PING_SAMPLES;	// if there are 64 samples, this also works: realping = (pingsamplessum >> 6)
	}
	else {
		// if g_truePing is off, use the normal ping
		client->pers.realPing = client->ps.ping;
	}
	//unlagged - true ping

	// IlDuca - Fixing antiwarp : removed !
	if(client->warping && g_maxWarp.integer && doAntiWarp) {
		int frames = (level.framenum - client->lastUpdateFrame);

		if(frames > g_maxWarp.integer) {
			frames = g_maxWarp.integer;
		}
		// if the difference between commandTime and the last command
		// time is small, you won't move as far since it's doing
		// velocity*time for updating your position

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
	if ( !doAntiWarp && ucmd->serverTime > level.time + 200 )
		ucmd->serverTime = level.time + 200;

	if ( !doAntiWarp && ucmd->serverTime < level.time - SECONDS_1 )
		ucmd->serverTime = level.time - SECONDS_1;

	// tjw: save ucmd->serverTime before it gets mutilated further
	//      tack on 2 server frames time for "50ms built-in lag"
	client->attackTime = ucmd->serverTime + (2 *(level.time - level.previousTime));

	//josh: Check for auto-mute, unmute if appropriate
	// IlDuca - added \n for a better output, but not in chat
	if ( client->sess.muted && client->sess.auto_mute_time != -1 && level.time > client->sess.auto_mute_time ) {
		if ( client->sess.muted_by & MUTED_BY_CENSOR ) {
			CPx(ent - g_entities, "print \"^5You've been auto-unmuted. Language penalty lifted.\n\"");
			AP(va("chat \"%s^7 has been auto-unmuted. Language penalty lifted.\"", ent->client->pers.netname ));
		}
		else if ( client->sess.muted_by & MUTED_BY_SHRUB ) {
			CPx(ent - g_entities, "print \"^5You've been auto-unmuted. Shrub penalty lifted.\n\"");
			AP(va("chat \"%s^7 has been auto-unmuted. Shrub penalty lifted.\"", ent->client->pers.netname ));
		}
		else if ( client->sess.muted_by & MUTED_BY_LUA ) {
			CPx(ent - g_entities, "print \"^5You've been auto-unmuted. Lua penalty lifted.\n\"");
			AP(va("chat \"%s^7 has been auto-unmuted. Lua penalty lifted.\"", ent->client->pers.netname ));
		}
		// core: fix: let the mute status be put in CS_PLAYERS configstring
		client->sess.muted = qfalse;
		client->sess.auto_mute_time = -1;
		client->sess.muted_by = UNMUTED;
		ClientConfigStringChanged( ent );
	}

	msec = ucmd->serverTime - client->ps.commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		return;
	}

	if ( msec > 200 ) {
		msec = 200;
	}

	// IlDuca - Fixing antiwarp : removed !
	if ( doAntiWarp && ( pmove_fixed.integer || client->pers.pmoveFixed ) ) {
		ucmd->serverTime = ((ucmd->serverTime + client->pers.pmoveMsec-1) /
			client->pers.pmoveMsec) * client->pers.pmoveMsec;
	}

	if( client->wantsscore ) {
		G_SendScore( ent );
		client->wantsscore = qfalse;
	}


	// check for exiting intermission
	if ( level.intermissiontime ) {
		ClientIntermissionThink( client );
		return;
	}

	// check for inactivity timer, but never drop the local client of a non-dedicated server
	// OSP - moved here to allow for spec inactivity checks as well
	if ( !ClientInactivityTimer( client ) )
		return;

	if( !(ent->r.svFlags & SVF_BOT) && level.time - client->pers.lastCCPulseTime > SECONDS_2 ) {
		G_SendMapEntityInfo( ent );
		client->pers.lastCCPulseTime = level.time;
	}

	if( !(ucmd->flags & 0x01) || ucmd->forwardmove || ucmd->rightmove || ucmd->upmove || ucmd->wbuttons || ucmd->doubleTap )
		ent->r.svFlags &= ~(SVF_SELF_PORTAL_EXCLUSIVE|SVF_SELF_PORTAL);

	// spectators don't do much
	// DHM - Nerve :: In limbo use SpectatorThink
	if ( client->sess.sessionTeam == TEAM_SPECTATOR || client->ps.pm_flags & PMF_LIMBO ) {
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
			}
			else if( client->combatState & (1<<COMBATSTATE_DAMAGEDEALT) && client->combatState & (1<<COMBATSTATE_DAMAGERECEIVED) ) {
				G_AddSkillPoints( ent, SK_BATTLE_SENSE, 5.f ); G_DEBUG_ADD_SKILL_POINTS( ent, SK_BATTLE_SENSE, 5.f, "combatstate hot" );
			}
			else {
				G_AddSkillPoints( ent, SK_BATTLE_SENSE, 2.f ); G_DEBUG_ADD_SKILL_POINTS( ent, SK_BATTLE_SENSE, 2.f, "combatstate warm" );
			}
		}

		client->pers.lastBattleSenseBonusTime = level.timeCurrent;
		client->combatState = COMBATSTATE_COLD;	// cool down again
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
	if( client->latched_buttons & BUTTON_ACTIVATE &&
        ent->health > 0 && !(client->ps.eFlags & EF_PLAYDEAD) &&
        !IS_SET_WEAPON(ent->s.weapon) )
	{
		Cmd_Activate_f( ent );
	}

	if (ent->flags & FL_NOFATIGUE)
		ent->client->pmext.sprintTime = SPRINTTIME;

	G_SetIdentifyClient( ent );

#ifdef OMNIBOTS
	// Omni-bot: used for class changes, bot will /kill 2 seconds before spawn
	Bot_Util_CheckForSuicide(ent);
#endif

	// check for respawning
	if( client->ps.stats[STAT_HEALTH] <= 0 && !(client->ps.eFlags & EF_PLAYDEAD)) {

		// DHM - Nerve
		WolfFindMedic( ent );

		// core: killer cam..
		KillerCam( ent );

		if ( level.time > ent->reviveTime  ) {
			ent->reviveTime = level.time + 64;

			if ( ent->reviveVal > 0 )
				ent->reviveVal -= REVIVE_DECAYRATE;

			if ( ent->reviveVal < 0 )
				ent->reviveVal = 0;
		}

		// See if we need to hop to limbo
		if( level.timeCurrent > client->respawnTime && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
			if( ucmd->upmove > 0 ) {
				if( g_gametype.integer == GT_WOLF_LMS && !(ent->r.svFlags & SVF_BOT) ) {
					trap_SendServerCommand( ent-g_entities, "reqforcespawn" );
				}
				else {
					limbo( ent, ( client->ps.stats[STAT_HEALTH] > GIB_HEALTH ) );
				}
			}

			if((g_forcerespawn.integer > 0 && level.timeCurrent - client->respawnTime > g_forcerespawn.integer * SECONDS_1) || client->ps.stats[STAT_HEALTH] <= GIB_HEALTH) {
				limbo(ent, (client->ps.stats[STAT_HEALTH] > GIB_HEALTH));
			}
		}
		return;
	}

	// IRATA don't sweep if in dead, poisened state :)  bots don't sweep!
	if( level.gameManager
	  && !(ent->r.svFlags & SVF_BOT)
	  && !(ent->client->ps.eFlags & (EF_DEAD|EF_POISONED)) // check this
	  && level.timeCurrent - client->pers.lastHQMineReportTime > SECONDS_20 ) {	// NOTE: check is every 20 secs

		if( level.gameManager->s.modelindex && client->sess.sessionTeam == TEAM_AXIS ) {
			if( G_SweepForLandmines( ent->r.currentOrigin, 256.f, TEAM_AXIS ) ) {
				trap_SendServerCommand(ent-g_entities, "cp \"Mines have been reported in this area.\" 1");
			}
		}
		else if( level.gameManager->s.modelindex2 && client->sess.sessionTeam == TEAM_ALLIES ) {
			if( G_SweepForLandmines( ent->r.currentOrigin, 256.f, TEAM_ALLIES ) ) {
				trap_SendServerCommand(ent-g_entities, "cp \"Mines have been reported in this area.\" 1");
			}
		}

		client->pers.lastHQMineReportTime = level.timeCurrent;
	}

	// IRATA arty detection - battlefield briefing
	// if skill is reached player is warned with additional
	// arty gun fire sound from pk0 . First arty spot is shown with awareness icon
	// similar to grenade awareness - see also CG_DrawDanger
	if (ent->client->sess.skillBits[SK_BATTLE_SENSE] & (1<<9)) {
		if( level.gameManager
		  && !(ent->r.svFlags & SVF_BOT)
		  && !(ent->client->ps.eFlags & (EF_DEAD|EF_POISONED))
		  && level.timeCurrent - client->pers.lastHQArtyReportTime > SECONDS_3 ) {
			if( G_SweepForArty( ent->r.currentOrigin, 256.f))
					G_AddEvent( ent, EV_ARTY_DETECTED, 0 );

			client->pers.lastHQArtyReportTime = level.timeCurrent;
		}
	}

	if(g_debugPlayerHitboxes.integer & 2) {
		gentity_t *head;
		vec3_t maxs;
		grefEntity_t refent;

		VectorCopy(ent->r.maxs, maxs);
		maxs[2] = ClientHitboxMaxZ(ent);
		G_RailBox( ent->r.currentOrigin, ent->r.mins, maxs, tv(0.f,0.f,1.f), ent->s.number );

		head = G_BuildHead(ent, &refent, qtrue);
		G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(0.f,0.f,1.f), head->s.number|HITBOXBIT_HEAD );
		G_FreeEntity( head );
	}

	// perform once-a-second actions
	if(level.match_pause == PAUSE_NONE) {
		ClientTimerActions( ent, msec );
	}
}

void ClientThink_cmd( gentity_t *ent, usercmd_t *cmd) {
	ent->client->pers.oldcmd = ent->client->pers.cmd;
	ent->client->pers.cmd = *cmd;
	ClientThink_real( ent );
}


/*
==================
ClientThink

A new command has arrived from the client
==================
*/
void ClientThink( int clientNum )
{
	gentity_t *ent = g_entities + clientNum;
	usercmd_t newcmd;

	trap_GetUsercmd( clientNum, &newcmd );

#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer )
#endif // ALLOW_GSYNC
	{
		if (G_DoAntiwarp(ent)) {
			// IlDuca - if we are using antiwarp, then use antiwarp way...
			// josh: use zinx antiwarp code
			etpro_AddUsercmd( clientNum, &newcmd );
			DoClientThinks( ent );
		}
		else {
			// IlDuca - If we are not using antiwarp, then follow the normal way
			ClientThink_cmd( ent, &newcmd );
		}
	}
}


void G_RunClient( gentity_t *ent )
{
#ifdef PROFILER
	BG_ProfilerBlockStartTimer(PROFILER_G_RunClient, qfalse);
#endif
	// Gordon: special case for uniform grabbing
	if( ent->client->pers.cmd.buttons & BUTTON_ACTIVATE ) {
		Cmd_Activate2_f( ent );
	}

	if( ent->health <= 0 && ent->client->ps.pm_flags & PMF_LIMBO ) {
		if( ent->r.linked ) {
			trap_UnlinkEntity( ent );
		}
	}

	// IlDuca - if we are using antiwarp, then follow the antiwarp way
	// josh: adding zinx antiwarp
	if (G_DoAntiwarp(ent)) {
		// josh: use zinx antiwarp code
		DoClientThinks( ent );
	}

#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer )
#endif // ALLOW_GSYNC
	{
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunClient);
#endif
		return;
	}

#ifdef ALLOW_GSYNC
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink_real( ent );
#endif

#ifdef PROFILER
	BG_ProfilerBlockStopTimer(PROFILER_G_RunClient);
#endif
}

/*
==================
SpectatorClientEndFrame

==================
*/
void SpectatorClientEndFrame( gentity_t *ent )
{

#ifdef MV_SUPPORT
	// OSP - specs periodically get score updates for useful demo playback info
	if(/*ent->client->pers.mvCount > 0 &&*/ ent->client->pers.mvScoreUpdate < level.time) {
		ent->client->pers.mvScoreUpdate = level.time + MV_SCOREUPDATE_INTERVAL;
		ent->client->wantsscore = qtrue;
	}
#endif

	// do this to keep current xp of spectators up to date especially on first connect to get xpsave state in limbo
 	if (ent->client->sess.spectatorState == SPECTATOR_FREE) {
 		int i;
		ent->client->ps.stats[STAT_XP] = 0;

		for( i = 0; i < SK_NUM_SKILLS; ++i ) {
			ent->client->ps.stats[STAT_XP] += ent->client->sess.skillpoints[i];
		}

		// redeye - to avoid overflows for big XP values(>= 32768), count each overflow and add it
		// again in cg_draw.c at display time
		ent->client->ps.stats[STAT_XP_OVERFLOW] = ent->client->ps.stats[STAT_XP] >> 15;		// >>15 == /32768
		ent->client->ps.stats[STAT_XP] = ent->client->ps.stats[STAT_XP] & 0x7FFF;
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
		}
		else if(ent->client->sess.sessionTeam == TEAM_AXIS) {
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

		if( g_gametype.integer == GT_WOLF_LMS && g_gamestate.integer == GS_PLAYING ) {
			// Force respawn in LMS when nobody is playing and we aren't at the timelimit yet
			if( !level.teamEliminateTime &&
				level.numTeamClients[0] == level.numFinalDead[0] && level.numTeamClients[1] == level.numFinalDead[1] &&
				ent->client->respawnTime <= level.timeCurrent && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
				do_respawn = qtrue;
			}
			else {
				do_respawn = qfalse;
			}
		}

		if ( do_respawn ) {
			reinforce(ent);
			return;
		}

#ifdef MV_SUPPORT
		// Limbos aren't following while in MV
		if((ent->client->ps.pm_flags & PMF_LIMBO) && ent->client->pers.mvCount > 0) {
			return;
		}
#endif

		clientNum = ent->client->sess.spectatorClient;

		// team follow1 and team follow2 go to whatever clients are playing
		if ( clientNum == -1 && ( ent->client->ps.pm_flags & PMF_LIMBO )) {
			clientNum = level.follow1;
		}
		else if ( clientNum == -2 && ( ent->client->ps.pm_flags & PMF_LIMBO )) {
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
#ifdef MV_SUPPORT
					int savedMVList			= ent->client->ps.powerups[PW_MVCLIENTLIST];
#endif

					do_respawn = ent->client->ps.pm_time;

					ent->client->ps = cl->ps;
					ent->client->ps.pm_time = do_respawn;							// put pm_time back
					ent->client->ps.persistant[PERS_SCORE] = savedScore;			// put score back
#ifdef MV_SUPPORT
					ent->client->ps.powerups[PW_MVCLIENTLIST] = savedMVList;
#endif
					ent->client->ps.pm_flags |= PMF_FOLLOW;
					ent->client->ps.pm_flags |= PMF_LIMBO;

				}
				else {
					ent->client->ps = cl->ps;
					ent->client->ps.pm_flags |= PMF_FOLLOW;
					ent->client->ps.persistant[PERS_SCORE] = 0; // Martijn's - serverstatus look who spec you
				}

				// DHM - Nerve :: carry flags over
				ent->client->ps.eFlags = flags;
				ent->client->ps.ping = ping;

				return;
			}
			else {
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
#ifdef MV_SUPPORT
	if(ent->client->pers.mvCount < 1) {
#endif
		ent->client->ps.powerups[PW_BLACKOUT] = (G_blockoutTeam(ent, TEAM_AXIS) * TEAM_AXIS) |
												(G_blockoutTeam(ent, TEAM_ALLIES) * TEAM_ALLIES);
#ifdef MV_SUPPORT
	}
#endif
}


// DHM - Nerve :: After reviving a player, their contents stay CONTENTS_CORPSE until it is determined
//					to be safe to return them to PLAYERSOLID

qboolean StuckInClient( gentity_t *self )
{
	int i;
	vec3_t	hitmin, hitmax;
	vec3_t	selfmin, selfmax;
	gentity_t *hit;

	// IRATA: moved up - selfmin/maxmin won't change each loop
	VectorAdd(self->r.currentOrigin, self->r.mins, selfmin);
	VectorAdd(self->r.currentOrigin, self->r.maxs, selfmax);

	for(i=0; i<level.numConnectedClients; ++i) {
		hit = g_entities + level.sortedClients[i];

		if(!hit->inuse || hit == self || !hit->client ||
		  !hit->s.solid || hit->health <= 0) {
			continue;
		}

		VectorAdd(hit->r.currentOrigin, hit->r.mins, hitmin);
		VectorAdd(hit->r.currentOrigin, hit->r.maxs, hitmax);

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

void WolfRevivePushEnt( gentity_t *self, gentity_t *other )
{
	vec3_t	dir, push;

	if ( self->props_frame_state == -1 ) {
		return;
	}

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
void WolfReviveBbox( gentity_t *self )
{
	int			touch[MAX_GENTITIES];
	int			num,i, touchnum=0;
	gentity_t	*hit = G_TestEntityPosition( self );
	vec3_t		mins, maxs;

	if( hit && ( hit->s.number == ENTITYNUM_WORLD || ( hit->client && (hit->client->ps.persistant[PERS_HWEAPON_USE] || (hit->client->ps.eFlags & EF_MOUNTEDTANK))) ) ) {
		G_DPrintf( "WolfReviveBbox: Player stuck in world or MG42 using player\n" );
		// Move corpse directly to the person who revived them
		if ( self->props_frame_state >= 0 ) {
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

	for (i=0 ; i<num ; ++i) {
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
		}
		else if ( hit->r.contents & ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_PLAYERCLIP ) ) {
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
==================
UpdatePlayerAura

==================
*/
void UpdatePlayerAura( gentity_t *ent )
{
	// Lucel: Try a slightly different approach to the aura checks.
	// All aura's are cleared in G_RunFrame... we then recalculate them here
	// based on whether this client is radiating any auras
	//
	// IRATA:
	// Aura also removed if p is poisoned
	if ( level.framenum % NQ_AURA_FRAME_FREQ == 0 && !( ent->client->ps.eFlags & (EF_DEAD|EF_PLAYDEAD|EF_POISONED) ) ) {
		int			skill		= 0;
		int			aurabit		= 0;
		// Figure if I'm able to radiate aura skills...
		switch (ent->client->sess.playerType) {
			case PC_MEDIC:
				skill 	= SK_MEDIC;
				aurabit = AURABIT_MEDIC;
				break;
			case PC_ENGINEER:
				skill 	= SK_ENGINEER;
				aurabit = AURABIT_ENGINEER;
				break;
			case PC_FIELDOPS:
				skill 	= SK_FIELDOPS;
				aurabit = AURABIT_FIELDOPS;
				break;
			default:
				return;
		}
		// So am I radiating any auras?
		if ( ent->client->sess.skillBits[skill] & (1<<5) ) {
			gentity_t	*target;
			int i;
			// Check all teammates within a certain radius...
			for ( i = 0 ; i < level.numConnectedClients; ++i ) {
				target		= &g_entities[level.sortedClients[i]];

				// Only scan players on your team
				// Dont aura yourself
				// IRATA: Dont aura spectators
				if( target->client->sess.sessionTeam != ent->client->sess.sessionTeam ||
					ent->s.clientNum == target->client->ps.clientNum ||
					target->client->sess.sessionTeam == TEAM_SPECTATOR ||
					target->client->sess.sessionTeam == TEAM_FREE) {
					continue;
				}

				// Are we close enough to affect them with our Aura??
				// IRATA: optimized to hav a non PVS mode
				// cs: changed to distance squared as its faster to multiply than getting sqrt
				if( DistanceSquared(target->r.currentOrigin, ent->r.currentOrigin) <= Square(NQ_AURA_RADIUS) ){

					// IRATA: trap_InPVS calls may be expansive even with high player count
					// we check now in range first and depending on realism flag we act ...
					if(g_realism.integer & REALISM_AURA_WITH_PVS) {
						if( trap_InPVS(target->r.currentOrigin, ent->r.currentOrigin)) {
							target->client->ps.stats[STAT_TEAMAURA] |= aurabit;
						}
					}
					else {
						target->client->ps.stats[STAT_TEAMAURA] |= aurabit;
					}
				}
			}
		}
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
void ClientEndFrame( gentity_t *ent )
{
	int			i;
//unlagged - smooth clients #1
	int frames;

	if((g_XPDecay.integer & XPDF_ENABLE) && !(level.time % SECONDS_60)) {
		G_XPDecay(ent, 60, qfalse);
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
	for ( i = 0 ; i < PW_NUM_POWERUPS ; ++i ) {

		if(	i == PW_NOFATIGUE ||
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

		if ( i == PW_ADRENALINE ) {
			if ( ent->client->ps.powerups[i] ) {
				if ( ent->client->ps.powerups[i] < 0 ) {
					// adrenaline no longer active,.. charging now.
					if ( -ent->client->ps.powerups[i] < level.time ) {
						ent->client->ps.powerups[i] = 0;
					}
				}
				else {
					// adrenaline is active,.. using now.
					if ( ent->client->ps.powerups[i] < level.time ) {
						int totalcharge = level.medicChargeTime[ent->client->sess.sessionTeam-1] * BG_GetFromTable(chargeCost_Adrenaline, ent->client->sess.skillBits, SK_MEDIC);
						int chargeleft = totalcharge - ADRENALINE_TIME;
						ent->client->ps.powerups[i] = -(level.time + chargeleft);
					}
				}
			}
		}
		else {
			if ( ent->client->ps.powerups[ i ] < level.time ) {
				ent->client->ps.powerups[ i ] = 0;
			}
		}
	}

	// core: stamina/adrenaline chargebar
	if (jp_insanity.integer & JP_INSANITY_CHARGEBAR) {
		ent->client->ps.powerups[PW_ADRENALINE] = level.time + ADRENALINE_TIME;
	}

	ent->client->ps.stats[STAT_XP] = 0;

	for( i = 0; i < SK_NUM_SKILLS; ++i ) {
		ent->client->ps.stats[STAT_XP] += ent->client->sess.skillpoints[i];
	}

	// redeye - to avoid overflows for big XP values(>= 32768), count each overflow and add it
	// again in cg_draw.c at display time
	ent->client->ps.stats[STAT_XP_OVERFLOW] = ent->client->ps.stats[STAT_XP] >> 15;		// >>15 == /32768
	ent->client->ps.stats[STAT_XP] = ent->client->ps.stats[STAT_XP] & 0x7FFF;			// & 0x7FFF == %32768

	// OSP - If we're paused, make sure other timers stay in sync
	//		--> Any new things in ET we should worry about?
	if(level.match_pause != PAUSE_NONE) {
		int time_delta = level.time - level.previousTime;

		ent->client->airOutTime += time_delta;
		ent->client->inactivityTime += time_delta;
		ent->client->lastBurnTime += time_delta;
		ent->client->pers.connectTime += time_delta;
		ent->client->pers.enterTime += time_delta;
		// IRATA: unused see playerTeamState_s
		//ent->client->pers.teamState.lastreturnedflag += time_delta;
		//ent->client->pers.teamState.lasthurtcarrier += time_delta;
		//ent->client->pers.teamState.lastfraggedcarrier += time_delta;
		ent->client->ps.classWeaponTime += time_delta;
		ent->lastHintCheckTime += time_delta;
		ent->pain_debounce_time += time_delta;
		ent->s.onFireEnd += time_delta;
	}


	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if ( level.intermissiontime ) {
		return;
	}

	// update the players aura
	UpdatePlayerAura(ent);

	// burn from lava, etc
	P_WorldEffects (ent);

	// apply all the damage taken this frame
	P_DamageFeedback (ent);

	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	ent->s.loopSound = 0;

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
	if ( !(ent->client->buttons & BUTTON_ATTACK) ) {
		ent->count2 = 0;
	}
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

	// IlDuca - fixing antiwarp : removed !
	if(g_maxWarp.integer && frames > g_maxWarp.integer && G_DoAntiwarp(ent))
		ent->client->warping = qtrue;

	if(g_skipCorrection.integer && !ent->client->warped && frames > 0 && !G_DoAntiwarp(ent)) {
		if (frames > 3) {
			// josh: I need frames to be = 2 here
			frames = 3;
			// these are disabled because the phone jack can give
			// away other players position through walls.
			ent->client->ps.eFlags |= EF_CONNECTION;
			ent->s.eFlags |= EF_CONNECTION;
		}
		G_PredictPmove(ent, (float)frames / (float)sv_fps.integer);
	}
	ent->client->warped = qfalse;

//unlagged - smooth clients #1

	// zinx - realistic hitboxes
    mdx_PlayerAnimation( ent );

	if(g_debugPlayerHitboxes.integer & 1 ) {
		gentity_t *head;
		vec3_t maxs;
		grefEntity_t refent;

		VectorCopy(ent->r.maxs, maxs);
		maxs[2] = ClientHitboxMaxZ(ent);
		G_RailBox( ent->r.currentOrigin, ent->r.mins, maxs, tv(0.f,1.f,0.f), ent->s.number);

		head = G_BuildHead(ent, &refent, qtrue);
		G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(0.f,1.f,0.f), ent->s.number|HITBOXBIT_HEAD);
		G_FreeEntity( head );
	}

	// josh: moved over from ClientThink see the note there
	// We want this to track the server's viewpoint
	G_StoreClientPosition( ent );
}
