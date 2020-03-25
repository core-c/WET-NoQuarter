/*
 * name:		cg_playerstate.c
 *
 * desc: 		This file acts on changes in a new playerState_t
 * 				With normal play, this will be done after local prediction, but when
 * 				following another player or playing back a demo, it will be checked
 * 				when the snapshot transitions like all the other entities
 *
 * NQQS:
 *
 */

#include "cg_local.h"

/*
==============
CG_DamageFeedback
==============
*/
void CG_DamageFeedback( int yawByte, int pitchByte, int damage ) {
	float		kick;
	int			health = cg.snap->ps.stats[STAT_HEALTH];
	float		scale;
	int			slot;
	viewDamage_t *vd;

	// show the attacking player's head and name in corner
	cg.attackerTime = cg.time;

	// the lower on health you are, the greater the view kick will be

	if ( health < 40 ) {
		scale = 1;
	}
	else {
		scale = 40.0 / health;
	}
	kick = damage * scale;

	if (kick < 5)
		kick = 5;
	if (kick > 10)
		kick = 10;

	// find a free slot
	for (slot=0; slot<MAX_VIEWDAMAGE; slot++) {
		if (cg.viewDamage[slot].damageTime + cg.viewDamage[slot].damageDuration < cg.time)
			break;
	}

	if (slot == MAX_VIEWDAMAGE)
		return;		// no free slots, never override or splats will suddenly disappear

	vd = &cg.viewDamage[slot];

	// if yaw and pitch are both 255, make the damage always centered (falling, etc)
	if ( yawByte == 255 && pitchByte == 255 ) {
		vd->damageX = 0;
		vd->damageY = 0;
		cg.v_dmg_roll = 0;
		cg.v_dmg_pitch = -kick;
	}
	else {
		float front, up, left, dist, yaw, pitch;
		vec3_t		dir;
		vec3_t		angles;

		// positional
		pitch = pitchByte / 255.0 * 360;
		yaw = yawByte / 255.0 * 360;

		angles[PITCH] = pitch;
		angles[YAW] = yaw;
		angles[ROLL] = 0;

		AngleVectors( angles, dir, NULL, NULL );
		VectorSubtract( vec3_origin, dir, dir );

		front = DotProduct (dir, cg.refdef.viewaxis[0] );
		left = DotProduct (dir, cg.refdef.viewaxis[1] );
		up = DotProduct (dir, cg.refdef.viewaxis[2] );

		dir[0] = front;
		dir[1] = left;
		dir[2] = 0;
		dist = VectorLength( dir );
		if ( dist < 0.1 ) {
			dist = 0.1;
		}

		cg.v_dmg_roll = kick * left;

		cg.v_dmg_pitch = -kick * front;

		if ( front <= 0.1 ) {
			front = 0.1;
		}
		vd->damageX = crandom()*0.3 + -left / front;
		vd->damageY = crandom()*0.3 + up / dist;
	}

	// clamp the position
	if ( vd->damageX > 1.0 ) {
		vd->damageX = 1.0;
	}
	if ( vd->damageX < - 1.0 ) {
		vd->damageX = -1.0;
	}

	if ( vd->damageY > 1.0 ) {
		vd->damageY = 1.0;
	}
	if ( vd->damageY < - 1.0 ) {
		vd->damageY = -1.0;
	}

	// don't let the screen flashes vary as much
	if ( kick > 10 ) {
		kick = 10;
	}
	vd->damageValue = kick;
	cg.v_dmg_time = cg.time + DAMAGE_TIME;
	vd->damageTime = cg.snap->serverTime;
	vd->damageDuration = kick * 50 * (1 + 2*(!vd->damageX && !vd->damageY));
	cg.damageTime = cg.snap->serverTime;
	cg.damageIndex = slot;
}


const char* CG_TeamnameForNumber( team_t teamNum )
{
	switch( teamNum ) {
		case TEAM_FREE:
			return "free";
		case TEAM_AXIS:
			return "axis";
		case TEAM_ALLIES:
			return "allies";
		case TEAM_SPECTATOR:
			return "spectator";
		default:
			return "^1ERROR";
	}
}
/*
================
CG_Respawn

A respawn happened this snapshot
================
*/
qboolean CG_execFile( char *filename );

void CG_Respawn( qboolean revived ) {
	static int oldTeam = -1;
	static int oldCls = -1;
	cg.serverRespawning = qfalse;	// Arnout: just in case

	// no error decay on player movement
	cg.thisFrameTeleport = qtrue;

	// need to reset client-side weapon animations
	cg.predictedPlayerState.weapAnim = ( ( cg.predictedPlayerState.weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | PM_IdleAnimForWeapon(cg.snap->ps.weapon);	// reset weapon animations
	cg.predictedPlayerState.weaponstate = WEAPON_READY;	// hmm, set this?  what to?

	// display weapons available
	cg.weaponSelectTime = cg.time;

	cg.cursorHintIcon = 0;
	cg.cursorHintTime = 0;

	// select the weapon the server says we are using
	cg.weaponSelect = cg.snap->ps.weapon;
	// DHM - Nerve :: Clear even more things on respawn
	cg.zoomedBinoc	= qfalse;
	cg.zoomedScope	= qfalse;
	cg.zoomTime		= 0;
	cg.zoomval		= 0;
	cg.leanTime		= 0;

	cg.idleAnim		= 0;
	cg.nextIdleTime = 0;
	cg.painTime		= 0;


	trap_SendConsoleCommand( "-zoom\n" );
	cg.binocZoomTime = 0;
	cg.knifeBloodVal = 0;


	// clear pmext
	memset( &cg.pmext, 0, sizeof(cg.pmext) );

	cg.pmext.bAutoReload = (cg_autoReload.integer > 0);
	cg.pmext.weapAltReload = (cg_weapAltReloads.integer > 0);

	cg.pmext.sprintTime = SPRINTTIME;

	if( !revived ) {
		cgs.limboLoadoutSelected = qfalse;
	}

	if( cg.predictedPlayerState.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
		cg.pmext.silencedSideArm = 1;
	}

	cg.proneMovingTime = 0;

	// reset fog to world fog (if present)
	trap_R_SetFog(FOG_CMD_SWITCHFOG, FOG_MAP,20,0,0,0,0);
	// dhm - end

	if ( !revived ) {
		if ( ( cgs.clientinfo[cg.clientNum].team == TEAM_AXIS || cgs.clientinfo[cg.clientNum].team == TEAM_ALLIES ) && ( cgs.clientinfo[cg.clientNum].cls != oldCls ) ){
			CG_execFile( va("autoexec_%s", BG_ClassnameForNumber_Filename(cgs.clientinfo[cg.clientNum].cls)) );
			oldCls = cgs.clientinfo[cg.clientNum].cls;
		}
		if ( cgs.clientinfo[cg.clientNum].team != oldTeam ){
			CG_execFile( va("autoexec_%s", CG_TeamnameForNumber(cgs.clientinfo[cg.clientNum].team)) );
			oldTeam = cgs.clientinfo[cg.clientNum].team;
		}
	}
}

#ifdef _DEBUG
	extern char *eventnames[];
#endif

void CG_CheckPlayerstateEvents( playerState_t *ps, playerState_t *ops ) {
	int			i;
	int			event;
	centity_t	*cent;

	if ( ps->externalEvent && ps->externalEvent != ops->externalEvent ) {
		cent = &cg_entities[ ps->clientNum ];
		cent->currentState.event = ps->externalEvent;
		cent->currentState.eventParm = ps->externalEventParm;
		CG_EntityEvent( cent, cent->lerpOrigin );
	}

	cent = &cg.predictedPlayerEntity; // cg_entities[ ps->clientNum ];
	// go through the predictable events buffer
	for ( i = ps->eventSequence - MAX_EVENTS ; i < ps->eventSequence ; i++ ) {
		// if we have a new predictable event
		if ( i >= ops->eventSequence
		  // or the server told us to play another event instead of a predicted event we already issued
		  // or something the server told us changed our prediction causing a different event
		  || (i > ops->eventSequence - MAX_EVENTS && ps->events[i & (MAX_EVENTS-1)] != ops->events[i & (MAX_EVENTS-1)]) ) {

			event = ps->events[ i & (MAX_EVENTS-1) ];
			cent->currentState.event = event;
			cent->currentState.eventParm = ps->eventParms[ i & (MAX_EVENTS-1) ];
			CG_EntityEvent( cent, cent->lerpOrigin );

			cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

			cg.eventSequence++;
		}
	}
}

/*
==================
CG_CheckChangedPredictableEvents
==================
*/
void CG_CheckChangedPredictableEvents( playerState_t *ps ) {
	int i;
	int event;
	centity_t	*cent = &cg.predictedPlayerEntity;

	for ( i = ps->eventSequence - MAX_EVENTS ; i < ps->eventSequence ; i++ ) {
		//
		if (i >= cg.eventSequence) {
			continue;
		}
		// if this event is not further back in than the maximum predictable events we remember
		if (i > cg.eventSequence - MAX_PREDICTED_EVENTS) {
			// if the new playerstate event is different from a previously predicted one
			if ( ps->events[i & (MAX_EVENTS-1)] != cg.predictableEvents[i & (MAX_PREDICTED_EVENTS-1) ] ) {

				event = ps->events[ i & (MAX_EVENTS-1) ];
				cent->currentState.event = event;
				cent->currentState.eventParm = ps->eventParms[ i & (MAX_EVENTS-1) ];
				CG_EntityEvent( cent, cent->lerpOrigin );

				cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

				if ( cg_showmiss.integer ) {
					CG_Printf("WARNING: changed predicted event\n");
				}
			}
		}
	}
}

/*
==================
CG_CheckLocalSounds
==================
*/
void CG_CheckLocalSounds( playerState_t *ps, playerState_t *ops ) {
	// health changes of more than -1 should make pain sounds
	if ( ps->stats[STAT_HEALTH] < ops->stats[STAT_HEALTH] - 1 ) {
		if ( ps->stats[STAT_HEALTH] > 0 ) {
			CG_PainEvent( &cg.predictedPlayerEntity, ps->stats[STAT_HEALTH], qfalse );

			cg.painTime = cg.time;
		}
	}

	// jaquboss - hitsounds
	if ( ops->persistant[PERS_HITS] != ps->persistant[PERS_HITS] && cg_hitSounds.integer & HITSOUNDS_ON) {
		// easy and nice way
		if ( ps->persistant[PERS_HITS] < ops->persistant[PERS_HITS] ) {
			if ( !(cg_hitSounds.integer & HITSOUNDS_NOTEAMSHOT ) )
			trap_S_StartSound (NULL, ps->clientNum, CHAN_AUTO, cgs.media.teamShot );
		}
		else if (ps->persistant[PERS_HEADSHOTS] > ops->persistant[PERS_HEADSHOTS]) {
			if ( !(cg_hitSounds.integer & HITSOUNDS_NOHEADSHOT ) )
			trap_S_StartSound (NULL, ps->clientNum, CHAN_AUTO, cgs.media.headShot );
		}
		else {
			if ( !(cg_hitSounds.integer & HITSOUNDS_NOBODYSHOT ) )
			trap_S_StartSound (NULL, ps->clientNum, CHAN_AUTO, cgs.media.bodyShot );
		}
	}

	// timelimit warnings
	if ( cgs.timelimit > 0 && cgs.gamestate == GS_PLAYING) {
		int		msec;

		msec = cg.time - cgs.levelStartTime;

		if ( cgs.timelimit > 5 && !( cg.timelimitWarnings & 1 ) && (msec > (cgs.timelimit - 5) * 60 * 1000) &&
			( msec < (cgs.timelimit-5)*60*1000+1000 ) ) {
			cg.timelimitWarnings |= 1;
			if( ps->persistant[PERS_TEAM] == TEAM_AXIS ) {
				if( cgs.media.fiveMinuteSound_g == -1 ) {
					CG_SoundPlaySoundScript( cg.fiveMinuteSound_g, NULL, -1, qtrue );
				}
				else if( cgs.media.fiveMinuteSound_g ) {
					trap_S_StartLocalSound( cgs.media.fiveMinuteSound_g, CHAN_ANNOUNCER );
				}
			}
			else if( ps->persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if( cgs.media.fiveMinuteSound_a == -1 ) {
					CG_SoundPlaySoundScript( cg.fiveMinuteSound_a, NULL, -1, qtrue );
				}
				else if( cgs.media.fiveMinuteSound_a ) {
					trap_S_StartLocalSound( cgs.media.fiveMinuteSound_a, CHAN_ANNOUNCER );
				}
			}
		}
		if ( cgs.timelimit > 2 && !( cg.timelimitWarnings & 2 ) && (msec > (cgs.timelimit - 2) * 60 * 1000) &&
			( msec < (cgs.timelimit-2)*60*1000+1000 ) ) {
			cg.timelimitWarnings |= 2;
			if( ps->persistant[PERS_TEAM] == TEAM_AXIS ) {
				if( cgs.media.twoMinuteSound_g == -1 ) {
					CG_SoundPlaySoundScript( cg.twoMinuteSound_g, NULL, -1, qtrue );
				}
				else if( cgs.media.twoMinuteSound_g ) {
					trap_S_StartLocalSound( cgs.media.twoMinuteSound_g, CHAN_ANNOUNCER );
				}
			}
			else if( ps->persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if( cgs.media.twoMinuteSound_a == -1 ) {
					CG_SoundPlaySoundScript( cg.twoMinuteSound_a, NULL, -1, qtrue );
				}
				else if( cgs.media.twoMinuteSound_a ) {
					trap_S_StartLocalSound( cgs.media.twoMinuteSound_a, CHAN_ANNOUNCER );
				}
			}
		}
		if ( !( cg.timelimitWarnings & 4 ) && (msec > (cgs.timelimit) * 60 * 1000 - 30000) &&
			(msec < (cgs.timelimit) * 60 * 1000 - 29000 ) ) {
			cg.timelimitWarnings |= 4;
			if( ps->persistant[PERS_TEAM] == TEAM_AXIS ) {
				if( cgs.media.thirtySecondSound_g == -1 ) {
					CG_SoundPlaySoundScript( cg.thirtySecondSound_g, NULL, -1, qtrue );
				}
				else if( cgs.media.thirtySecondSound_g ) {
					trap_S_StartLocalSound( cgs.media.thirtySecondSound_g, CHAN_ANNOUNCER );
				}
			}
			else if( ps->persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if( cgs.media.thirtySecondSound_a == -1 ) {
					CG_SoundPlaySoundScript( cg.thirtySecondSound_a, NULL, -1, qtrue );
				}
				else if( cgs.media.thirtySecondSound_a ) {
					trap_S_StartLocalSound( cgs.media.thirtySecondSound_a, CHAN_ANNOUNCER );
				}
			}
		}
	}
}

/*
===============
CG_TransitionPlayerState

===============
*/
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops )
{
#ifdef MV_SUPPORT
	// OSP - MV client handling
	if(cg.mvTotalClients > 0) {
		if (ps->clientNum != ops->clientNum) {
			cg.thisFrameTeleport = qtrue;

			// clear voicechat
			cg.predictedPlayerEntity.voiceChatSpriteTime	= 0;	// CHECKME: should we do this here?
			cg_entities[ps->clientNum].voiceChatSpriteTime	= 0;

			*ops = *ps;
		}
		CG_CheckLocalSounds( ps, ops );
		return;
	}
#endif

	// check for changing follow mode
	if ( ps->clientNum != ops->clientNum ) {
		cg.thisFrameTeleport = qtrue;

		// clear voicechat
		cg.predictedPlayerEntity.voiceChatSpriteTime	= 0;
		cg_entities[ps->clientNum].voiceChatSpriteTime	= 0;

		// make sure we don't get any unwanted transition effects
		*ops = *ps;

		// DHM - Nerve :: After Limbo, make sure and do a CG_Respawn
		if ( ps->clientNum == cg.clientNum )
			ops->persistant[PERS_SPAWN_COUNT]--;
	}

	if( ps->eFlags & EF_FIRING ) {
		cg.lastFiredWeaponTime = 0;
		cg.weaponFireTime += cg.frametime;
	}
	else {
		if( cg.weaponFireTime > 500 && cg.weaponFireTime ) {
			cg.lastFiredWeaponTime = cg.time;
		}

		cg.weaponFireTime = 0;
	}

	// damage events (player is getting wounded)
	if( ps->damageEvent != ops->damageEvent && ps->damageCount ) {
		CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount );
	}

	// respawning
	if( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT] ) {
		CG_Respawn( (ps->persistant[PERS_REVIVE_COUNT]) != (ops->persistant[PERS_REVIVE_COUNT]) ? qtrue : qfalse );
	}

	if ( cg.mapRestart ) {
		CG_Respawn( qfalse );
		cg.mapRestart = qfalse;
	}

	if ( cg.snap->ps.pm_type != PM_INTERMISSION
		&& ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		CG_CheckLocalSounds( ps, ops );
	}

	// check for going low on ammo
	// CG_CheckAmmo();

	if( ps->eFlags & EF_PRONE_MOVING ) {
		if( ps->weapon == WP_BINOCULARS ) {
			if( ps->eFlags & EF_ZOOMING ) {
				trap_SendConsoleCommand( "-zoom\n" );
			}
		}

		if( !(ops->eFlags & EF_PRONE_MOVING) ) {
			// ydnar: this screws up auto-switching when dynamite planted or grenade thrown/out of ammo
			//%	CG_FinishWeaponChange( cg.weaponSelect, ps->nextWeapon );

			cg.proneMovingTime = cg.time;
		}
	}
	else if( ops->eFlags & EF_PRONE_MOVING ) {
		cg.proneMovingTime = -cg.time;
	}

	if( !(ps->eFlags & EF_PRONE) && ops->eFlags & EF_PRONE ) {
		if( cg.weaponSelect == WP_MOBILE_MG42_SET || cg.weaponSelect == WP_MOBILE_BROWNING_SET || cg.weaponSelect == WP_BAR_SET) {
			CG_FinishWeaponChange( cg.weaponSelect, ps->nextWeapon );
		}
	}

	// run events
	CG_CheckPlayerstateEvents( ps, ops );

	// smooth the ducking viewheight change
	if ( ps->viewheight != ops->viewheight ) {
		cg.duckChange = ps->viewheight - ops->viewheight;
		cg.duckTime = cg.time;

		if ( ops->eFlags & EF_PLAYDEAD && !(ps->eFlags & EF_PLAYDEAD) ) {
			cg.duckPlaydead = qtrue;
		}
		else {
			cg.duckPlaydead = qfalse;
		}
	}
}
