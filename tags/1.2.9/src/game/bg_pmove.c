 /*
 * name:		g_mover.c
 *
 * desc:		both games player movement code
 *				takes a playerstate and a usercmd as input and returns a modifed playerstate
 *
 * NQQS:
 *
 */

#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#else
#include "q_shared.h"
#include "bg_public.h"
#endif // CGAMEDLL

#include "bg_local.h"

#ifdef CGAMEDLL

#define PM_GameType				cg_gameType.integer
#define PM_DOUBLE_JUMP			cgs.doubleJump
#define PM_DOUBLE_JUMP_HEIGHT	cgs.doubleJumpHeight
#define PM_INSANITY				cg_insanity.integer
#define	PM_REALISM				cgs.realism // extended prone etc..
#define PM_FIXEDPHYSICS			cgs.fixedphysics
#define PM_FIXEDPHYSICSFPS		cgs.fixedphysicsfps
#define PM_ADRENALINE			cgs.adrenaline
// #define PM_SV_FPS			// IRATA: TODO/FIXME

#elif GAMEDLL
extern	vmCvar_t				jp_insanity;
extern	vmCvar_t				g_realism;
extern	vmCvar_t				g_adrenaline;
extern	vmCvar_t				g_fixedphysics;
extern	vmCvar_t				g_fixedphysicsfps;
extern  vmCvar_t				sv_fps;

#define PM_FIXEDPHYSICS			g_fixedphysics.integer
#define PM_FIXEDPHYSICSFPS		g_fixedphysicsfps.integer
#define PM_DOUBLE_JUMP			g_doubleJump.integer
#define PM_DOUBLE_JUMP_HEIGHT	g_doubleJumpHeight.value
#define PM_GameType				g_gametype.integer
#define PM_INSANITY				jp_insanity.integer
#define PM_REALISM				g_realism.integer
#define PM_ADRENALINE			g_adrenaline.integer
#define PM_SV_FPS				sv_fps.integer
#endif


#define PM_LEAN					(!(PM_REALISM & REALISM_NOLEAN))


pmove_t		*pm;
pml_t		pml;

// movement parameters
float	pm_stopspeed = 100;
//----(SA)	modified
float	pm_waterSwimScale	= 0.5;
float	pm_waterWadeScale	= 0.70;
float	pm_slagSwimScale	= 0.30;
float	pm_slagWadeScale	= 0.70;
float	pm_proneSpeedScale	= 0.21;	// was: 0.18 (too slow) then: 0.24 (too fast)

float	pm_accelerate		= 10;
float	pm_airaccelerate	= 1;
float	pm_wateraccelerate	= 4;
float	pm_slagaccelerate	= 2;
float	pm_flyaccelerate	= 8;

float	pm_friction			= 6;
float	pm_waterfriction	= 1;
float	pm_slagfriction		= 1;
float	pm_flightfriction	= 3;
float	pm_ladderfriction	= 14;
float	pm_spectatorfriction = 5.0f;

//----(SA)	end

int		c_pmove = 0;

#ifdef GAMEDLL

// In just the GAME DLL, we want to store the groundtrace surface stuff,
// so we don't have to keep tracing.
void ClientStoreSurfaceFlags( int clientNum, int surfaceFlags);

#endif

qboolean CheckSilent( void ){

	if ( !(pm->skillBits[SK_COVERTOPS] & (1<<6)) || pm->ps->stats[STAT_PLAYER_CLASS] != PC_COVERTOPS ) {
		return qfalse;
	}

	if ( pm->cmd.buttons & BUTTON_SPRINT && pm->pmext->sprintTime > 50 ) {
		return qfalse;
	}
	return qtrue;
}

/*
===============
PM_AddEvent

===============
*/
void PM_AddEvent( int newEvent ) {
	BG_AddPredictableEventToPlayerstate( newEvent, 0, pm->ps );
}

void PM_AddEventExt( int newEvent, int eventParm ) {
	BG_AddPredictableEventToPlayerstate( newEvent, eventParm, pm->ps );
}

int PM_IdleAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_SATCHEL_DET:
		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
		case WP_BAR_SET:
			return WEAP_IDLE2;
		default:
			return WEAP_IDLE1;
	}
}

int PM_AltSwitchFromForWeapon ( int weapon ) {
	return WEAP_ALTSWITCHFROM;
}

int PM_AltSwitchToForWeapon ( int weapon )
{
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_MORTAR:
		case WP_MORTAR2:
		case WP_MOBILE_MG42:
		case WP_MOBILE_BROWNING:
		case WP_BAR:
			return WEAP_ALTSWITCHFROM;
		default:
			return WEAP_ALTSWITCHTO;
	}
}

int PM_AttackAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_SATCHEL_DET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
		case WP_BAR_SET:
			return WEAP_ATTACK2;
		default:
			return WEAP_ATTACK1;
	}
}

int PM_LastAttackAnimForWeapon ( int weapon )
{
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
		case WP_BAR_SET:
			return WEAP_ATTACK2;
		case WP_BAR:
		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			return WEAP_ATTACK1;
		default:
			return WEAP_ATTACK_LASTSHOT;
	}
}

// jet Pilot - special case reloading animation for shotgun
// 0  = first shell
// -1 = finished
// 1+ = current shell
int PM_ReloadAnimForShotgun( int shell )
{
	if (shell == 0)
		return WEAP_RELOAD1;
	if (shell == -1)
		return WEAP_RELOAD3;
	return WEAP_RELOAD2;
}

int PM_ReloadAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_RELOAD2;
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
		case WP_BAR_SET:
			return WEAP_RELOAD3;
		default:
			if( pm->skillBits[SK_LIGHT_WEAPONS] & (1<<2) && BG_isLightWeaponSupportingFastReload( weapon )  )
				return WEAP_RELOAD2;	// faster reload
			else
				return WEAP_RELOAD1;
	}
}

int PM_RaiseAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_RELOAD3;
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
		case WP_BAR_SET:
			return WEAP_DROP2;
		case WP_SATCHEL_DET:
			return WEAP_RELOAD2;

		default:
			if( pm && ( pm->skillBits[SK_LIGHT_WEAPONS] & (1<<5)) && WeaponTable[weapon].isFastSwitch )
				return WEAP_FASTRAISE;	// faster reload
			else
				return WEAP_RAISE;
	}
}

int PM_DropAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_DROP2;
		case WP_SATCHEL_DET:
			return WEAP_RELOAD1;

		default:
			if( pm && ( pm->skillBits[SK_LIGHT_WEAPONS] & (1<<5)) && WeaponTable[weapon].isFastSwitch)
				return WEAP_FASTDROP;	// faster reload
			else
				return WEAP_DROP;
	}
}

/*
===============
PM_AddTouchEnt
===============
*/
void PM_AddTouchEnt( int entityNum ) {
	int		i;

	if ( entityNum == ENTITYNUM_WORLD || pm->numtouch == MAXTOUCH) {
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; ++i ) {
		if ( pm->touchents[ i ] == entityNum ) {
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
	pm->numtouch++;
}

/*
==============
PM_StartWeaponAnim
==============
*/
static void PM_StartWeaponAnim( int anim ) {

	if ( pm->ps->pm_type >= PM_DEAD )
		return;

	if ( pm->pmext->weapAnimTimer > 0 )
		return;

	if(pm->cmd.weapon == WP_NONE)
		return;

	pm->ps->weapAnim = ( ( pm->ps->weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
}

void PM_ContinueWeaponAnim( int anim ) {
	if(pm->cmd.weapon == WP_NONE)
		return;

	if ( ( pm->ps->weapAnim & ~ANIM_TOGGLEBIT ) == anim ) {
		return;
	}
	if ( pm->pmext->weapAnimTimer > 0 ) {
		return;		// a high priority animation is running
	}
	PM_StartWeaponAnim( anim );
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce ) {
	float	backoff = DotProduct (in, normal);

	if ( backoff < 0 ) {
		backoff *= overbounce;
	}
	else {
		backoff /= overbounce;
	}

	out[0] = in[0] - (normal[0]*backoff);
	out[1] = in[1] - (normal[1]*backoff);
	out[2] = in[2] - (normal[2]*backoff);
}

/*
==================
PM_TraceAll

finds worst trace of body/legs, for collision.
==================
*/

void PM_TraceLegs( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end, trace_t *bodytrace, vec3_t viewangles, void (tracefunc)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int ignoreent, int tracemask )
{
	vec3_t ofs, org, point;
	vec3_t flatforward;
	float angle;

	// zinx - don't let players block legs
	tracemask &= ~(CONTENTS_BODY | CONTENTS_CORPSE);

	if (legsOffset) {
		*legsOffset = 0;
	}

	angle = DEG2RAD(viewangles[YAW]);
	flatforward[0] = cos(angle);
	flatforward[1] = sin(angle);
	flatforward[2] = 0;

	if(pm->ps->eFlags & EF_PRONE) {
		VectorScale(flatforward, -32, ofs);
	}
	else {
		VectorScale(flatforward, 32, ofs);
	}

	VectorAdd(start, ofs, org);
	VectorAdd(end, ofs, point);
	tracefunc(trace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
	if (!bodytrace || trace->fraction < bodytrace->fraction || trace->allsolid) {
		trace_t steptrace;

		// legs are clipping sooner than body
		// see if our legs can step up
		ofs[2] += STEPSIZE;

		VectorAdd(start, ofs, org);
		VectorAdd(end, ofs, point);
		tracefunc(&steptrace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
		if (!steptrace.allsolid && !steptrace.startsolid && steptrace.fraction > trace->fraction) {
			// the step trace did better -- use it instead
			*trace = steptrace;

			// get legs offset
			if (legsOffset) {
				*legsOffset = ofs[2];

				VectorCopy(steptrace.endpos, org);
				VectorCopy(steptrace.endpos, point);
				point[2] -= STEPSIZE;

				tracefunc(&steptrace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
				if (!steptrace.allsolid) {
					*legsOffset = ofs[2] - (org[2] - steptrace.endpos[2]);
				}
			}
		}
	}
}

void PM_TraceHead(trace_t *trace,
				  vec3_t start,
				  vec3_t end,
				  trace_t *bodytrace,
				  vec3_t viewangles,
				  void (tracefunc)(trace_t *results,
				  	const vec3_t start,
				  	const vec3_t mins,
				  	const vec3_t maxs,
					const vec3_t end,
					int passEntityNum,
					int contentMask),
					int ignoreent,
					int tracemask )
{
	vec3_t ofs;
	vec3_t flatforward;
	vec3_t point;
	float angle;
	// tjw: more than just head, try to make a box for all the
	//      player model that extends out (weapons and arms too)
	vec3_t	mins = { -18.f, -18.f, -2.f };
	vec3_t	maxs = { 18.f, 18.f, 10.f };

	// don't let players block head
	tracemask &= ~(CONTENTS_BODY | CONTENTS_CORPSE);

	angle = DEG2RAD(viewangles[YAW]);
	flatforward[0] = cos(angle);
	flatforward[1] = sin(angle);
	flatforward[2] = 0;

	if(pm->ps->eFlags & EF_PRONE) {;
		VectorScale(flatforward, 36, ofs);
	}
	else {
		VectorScale(flatforward, -36, ofs);
	}

	VectorAdd(end, ofs, point);
	tracefunc(trace, start, mins, maxs, point, ignoreent, tracemask);
}

/* Traces all player bboxes -- body. legs, and head */
void	PM_TraceAllParts( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end )
{
	pm->trace(trace, start, pm->mins, pm->maxs, end, pm->ps->clientNum, pm->tracemask);

	// legs and head
	if((pm->ps->eFlags & EF_PRONE) ||
		(pm->ps->eFlags & EF_DEAD) ||
		(pm->ps->eFlags & EF_PLAYDEAD)) {

		trace_t legtrace;
		trace_t headtrace;
		qboolean adjust = qfalse;

		PM_TraceLegs( &legtrace, legsOffset, start, end, trace,
			pm->ps->viewangles, pm->trace, pm->ps->clientNum,
			pm->tracemask);

		if(legtrace.fraction < trace->fraction ||
		    legtrace.startsolid ||
		    legtrace.allsolid) {

			*trace = legtrace;
			adjust = qtrue;
		}

		PM_TraceHead( &headtrace, start, end, trace,
			pm->ps->viewangles, pm->trace, pm->ps->clientNum,
			pm->tracemask);

		if(headtrace.fraction < trace->fraction ||
		    headtrace.startsolid ||
		    headtrace.allsolid) {

			*trace = headtrace;
			adjust = qtrue;
		}

		if(adjust) {
			VectorSubtract(end, start, trace->endpos);
			VectorMA(start, trace->fraction, trace->endpos,
				trace->endpos);
		}
	}
}

void PM_TraceAll( trace_t *trace, vec3_t start, vec3_t end )
{
	PM_TraceAllParts( trace, NULL, start, end );
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
static void PM_Friction( void ) {
	vec3_t	vec;
	float	*vel = pm->ps->velocity;
	float	speed, newspeed;
	float	drop;

	VectorCopy( vel, vec );
	if ( pml.walking ) {
		vec[2] = 0;	// ignore slope movement
	}

	speed = VectorLength(vec);
	// rain - #179 don't do this for PM_SPECTATOR/PM_NOCLIP, we always want them to stop
	if (speed < 1 && pm->ps->pm_type != PM_SPECTATOR && pm->ps->pm_type != PM_NOCLIP) {

		vel[0] = 0;
		vel[1] = 0;		// allow sinking underwater
		// FIXME: still have z friction underwater?
		return;
	}

	drop = 0;

	// apply end of dodge friction
	if( pm->cmd.serverTime - pm->pmext->dodgeTime < 350 && pm->cmd.serverTime - pm->pmext->dodgeTime > 250 ) {
		drop += speed * 20 * pml.frametime;
	}

	// apply ground friction
	if ( pm->waterlevel <= 1 ) {
		if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
			// if getting knocked back, no friction
			if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
				float friction = 1.f ;
				float control = speed < pm_stopspeed ? pm_stopspeed : speed;
				drop += control*pm_friction*friction*pml.frametime;
			}
		}
	}

	// apply water friction even if just wading
	if ( pm->waterlevel ) {
		if ( pm->watertype == CONTENTS_SLIME )	//----(SA)	slag
			drop += speed*pm_slagfriction*pm->waterlevel*pml.frametime;
		else
			drop += speed*pm_waterfriction*pm->waterlevel*pml.frametime;
	}

	if ( pm->ps->pm_type == PM_SPECTATOR) {
		drop += speed * pm_spectatorfriction * pml.frametime;
	}

	// apply ladder strafe friction
	if ( pml.ladder ) {
		drop += speed*pm_ladderfriction*pml.frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0) {
		newspeed = 0;
	}
	newspeed /= speed;

	// rain - if we're barely moving and barely slowing down, we want to
	// help things along--we don't want to end up getting snapped back to
	// our previous speed
	if (pm->ps->pm_type == PM_SPECTATOR || pm->ps->pm_type == PM_NOCLIP) {
		if (drop < 1.0f && speed < 3.0f) {
			newspeed = 0.0;
		}
	}

	// rain - used VectorScale instead of multiplying by hand
	VectorScale(vel, newspeed, vel);
}

/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
static void PM_Accelerate( vec3_t wishdir, float wishspeed, float accel ) {
#if 1
	// q2 style
	float		addspeed = wishspeed - DotProduct (pm->ps->velocity, wishdir);
	float       accelspeed;

	if (addspeed <= 0) {
		return;
	}
	accelspeed = accel*pml.frametime*wishspeed;
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}

	// Ridah, variable friction for AI's
	if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
		accelspeed *= (1.0 / pm->ps->friction);
	}
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}

	pm->ps->velocity[0] += accelspeed*wishdir[0];
	pm->ps->velocity[1] += accelspeed*wishdir[1];
	pm->ps->velocity[2] += accelspeed*wishdir[2];

#else
	// proper way (avoids strafe jump maxspeed bug), but feels bad
	vec3_t		wishVelocity;
	vec3_t		pushDir;
	float		pushLen;
	float		canPush;

	VectorScale( wishdir, wishspeed, wishVelocity );
	VectorSubtract( wishVelocity, pm->ps->velocity, pushDir );
	pushLen = VectorNormalize( pushDir );

	canPush = accel*pml.frametime*wishspeed;
	if (canPush > pushLen) {
		canPush = pushLen;
	}

	VectorMA( pm->ps->velocity, canPush, pushDir, pm->ps->velocity );
#endif
}

// JPW NERVE -- added because I need to check single/multiplayer instances and branch accordingly
#ifdef CGAMEDLL
	extern vmCvar_t			cg_gameType;
	extern vmCvar_t			cg_movespeed;
#endif
#ifdef GAMEDLL
	extern	vmCvar_t		g_gametype;
	extern	vmCvar_t		g_movespeed;
#endif

/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale( usercmd_t *cmd ) {
	int		max		= abs( cmd->forwardmove );
	int		maxr	= abs( cmd->rightmove );
	int		maxu	= abs( cmd->upmove );
	float	scale;

	if ( maxr > max ) {
		max = maxr;
	}
	if ( maxu > max ) {
		max = maxu;
	}

	if ( !max ) {
		return 0;
	}

	// scale = speed * max / 127 * total
	// ^^total = dotproduct of cmd->xxxmove vector, sqrt(dot)=vectorlength
	// Q_rsqrt is faster than 1/sqrt()
	// 1/127.0 will be precalulated by the compiler, so the calculation leaves only multiplications at runtime..
	scale = (float)(pm->ps->speed * max) * (1/127.0) * Q_rsqrt(cmd->forwardmove * cmd->forwardmove + cmd->rightmove * cmd->rightmove + cmd->upmove * cmd->upmove);

	if (pm->cmd.buttons & BUTTON_SPRINT && pm->pmext->sprintTime > 50) {
		scale *= pm->ps->sprintSpeedScale;
	}
	else {
		scale *= pm->ps->runSpeedScale;
	}

	if (pm->ps->pm_type == PM_NOCLIP)
		scale *= 3;

// JPW NERVE -- half move speed if heavy weapon is carried
// this is the counterstrike way of doing it -- ie you can switch to a non-heavy weapon and move at
// full speed.  not completely realistic (well, sure, you can run faster with the weapon strapped to your
// back than in carry position) but more fun to play.  If it doesn't play well this way we'll bog down the
// player if the own the weapon at all.
//
	switch (pm->ps->weapon) {
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_VENOM:
		case WP_MORTAR2:
		case WP_MORTAR:
			if( pm->skillBits[SK_HEAVY_WEAPONS] & (1<<3) ) {
				scale *= 1.5f*BG_Weapons[pm->ps->weapon].movementSpeedScale;
			}
			else {
				scale *= BG_Weapons[pm->ps->weapon].movementSpeedScale;
			}
			break;
        case WP_FLAMETHROWER:
	        if ( !( pm->skillBits[SK_HEAVY_WEAPONS] & (1<<7) ) ) {
		        if( !( pm->skillBits[SK_HEAVY_WEAPONS] & (1<<3) ) || pm->cmd.buttons & BUTTON_ATTACK )
			        scale *= BG_Weapons[pm->ps->weapon].movementSpeedScale;
	        }
            break;
        // Only Soldier is fast moving with BAR
        case WP_BAR:
        case WP_BAR_SET:
		        if( !pm->ps->stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) {
			        scale *= BG_Weapons[pm->ps->weapon].movementSpeedScale;
	        }
            break;
		default:
	        scale *= BG_Weapons[pm->ps->weapon].movementSpeedScale;
			break;
	}

	return scale;
}

/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
static void PM_SetMovementDir( void ) {
// Ridah, changed this for more realistic angles (at the cost of more network traffic?)
	float	speed;
	vec3_t	moved;

	VectorSubtract (pm->ps->origin, pml.previous_origin, moved);

	if (	(pm->cmd.forwardmove || pm->cmd.rightmove)
		&&	(pm->ps->groundEntityNum != ENTITYNUM_NONE)
		&&	(speed = VectorLength( moved ))
		&&	(speed > pml.frametime*5))		// if moving slower than 20 units per second, just face head angles
	{
		vec3_t	dir;
		int moveyaw;

		VectorNormalize2( moved, dir );
		vectoangles( dir, dir );

		moveyaw = (int)AngleDelta( dir[YAW], pm->ps->viewangles[YAW] );

		if (pm->cmd.forwardmove < 0)
			moveyaw = (int)AngleNormalize180(moveyaw + 180);

		if (abs(moveyaw) > 75) {
			if (moveyaw > 0) {
				moveyaw = 75;
			}
			else {
				moveyaw = -75;
			}
		}

		pm->ps->movementDir = (signed char)moveyaw;
	}
	else {
		pm->ps->movementDir = 0;
	}
}

/*
=============
PM_CheckJump
=============
*/
static qboolean PM_CheckJump( void ) {
	// no jumpin when prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return qfalse;
	}

	// JPW NERVE -- jumping in multiplayer uses and requires sprint juice (to prevent turbo skating, sprint + jumps)
	// don't allow jump accel
	if (pm->cmd.serverTime - pm->pmext->jumpTime < 850) {
		return qfalse;
	}

	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return qfalse;		// don't allow jump until all buttons are up
	}

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		return qfalse;
	}

	// must wait for jump to be released
	if ( pm->ps->pm_flags & PMF_JUMP_HELD ) {
		// clear upmove so cmdscale doesn't lower running speed
		pm->cmd.upmove = 0;
		return qfalse;
	}

	pml.groundPlane = qfalse;		// jumping away
	pml.walking = qfalse;
	pm->ps->pm_flags |= PMF_JUMP_HELD;

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pm->ps->velocity[2] = JUMP_VELOCITY;

	if ( pm->cmd.forwardmove >= 0 ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
		pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	}

	return qtrue;
}

/*
=============
PM_CheckDoubleJump
=============
*/
static qboolean PM_CheckDoubleJump( void ) {

	if ( !PM_DOUBLE_JUMP ) {
		return qfalse;
	}

	// no jumpin when prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return qfalse;
	}

	// josh: Don't allow if already double jumping
	if (pm->ps->pm_flags & (PMF_DOUBLEJUMPING|PMF_RESPAWNED))	{
		return qfalse;
	}

	// jpw

	// Only allow double jump when going up
	if ( (pm->ps->velocity[2] <= 0) && (PM_DOUBLE_JUMP != 2) ) {	// Meyer - If set to 2, do Jaymod style
		return qfalse;
	}

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		return qfalse;
	}

	// must wait for jump to be released
	if ( pm->ps->pm_flags & PMF_JUMP_HELD ) {
		// clear upmove so cmdscale doesn't lower running speed
		pm->cmd.upmove = 0;
		return qfalse;
	}

	pml.groundPlane = qfalse;		// jumping away
	pml.walking = qfalse;
	pm->ps->pm_flags |= PMF_JUMP_HELD;

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pm->ps->velocity[2] = PM_DOUBLE_JUMP_HEIGHT*JUMP_VELOCITY;

	if ( pm->cmd.forwardmove >= 0 ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
		pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	}

	return qtrue;
}

/*
=============
PM_CheckWaterJump
=============
*/
static qboolean	PM_CheckWaterJump( void ) {
	vec3_t	spot;
	int		cont;
	vec3_t	flatforward;

	if (pm->ps->pm_time) {
		return qfalse;
	}

	// check for water jump
	if ( pm->waterlevel != 2 ) {
		return qfalse;
	}

	flatforward[0] = pml.forward[0];
	flatforward[1] = pml.forward[1];
	flatforward[2] = 0;
	VectorNormalize (flatforward);

	VectorMA (pm->ps->origin, 30.0f, flatforward, spot);
	spot[2] += 4;
	cont = pm->pointcontents (spot, pm->ps->clientNum );
	if ( !(cont & CONTENTS_SOLID) ) {
		return qfalse;
	}

	spot[2] += 16;
	cont = pm->pointcontents (spot, pm->ps->clientNum );
	if ( cont ) {
		return qfalse;
	}

	// jump out of water
	VectorScale (pml.forward, 200, pm->ps->velocity);
	pm->ps->velocity[2] = 350;

	pm->ps->pm_flags |= PMF_TIME_WATERJUMP;
	pm->ps->pm_time = 2000;

	return qtrue;
}

/*
 * PM_CheckPlayDead
 * see if this player can lay down and look dead
*/
static qboolean PM_CheckPlayDead (void)
{
	trace_t trace;

	if(pm->ps->pm_type != PM_PLAYDEAD) return qfalse; // fixme, client will never have this

	// PM_PLAYDEAD is a one time only pm_type
	pm->ps->pm_type = pm->ps->eFlags & EF_PLAYDEAD ? PM_DEAD : PM_NORMAL;

	if(!(pm->ps->eFlags & EF_PLAYDEAD)) {
		if(pm->ps->pm_flags & PMF_LADDER) {
			return qfalse;
		}

		if(pm->ps->persistant[PERS_HWEAPON_USE] ||
			pm->ps->eFlags & EF_MOUNTEDTANK) {
			return qfalse;
		}

		if(pm->waterlevel > 1) {
			return qfalse;
		}

		if ( pm->cmd.serverTime - pm->pmext->playDeadTime < 1750 )
			return qfalse;

		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];
		pm->maxs[2] = 0; // jaquboss - fixmehere, we need something small but not weird, fuck capsules//pm->ps->crouchMaxZ;

		pm->ps->eFlags |= EF_PLAYDEAD;
		PM_TraceAll(&trace, pm->ps->origin, pm->ps->origin);
		pm->ps->eFlags &= ~EF_PLAYDEAD;

		if(!trace.startsolid && !trace.allsolid) {
			pm->ps->eFlags |= EF_PLAYDEAD;
			pm->ps->pm_type = PM_DEAD;
			// make the hitbox like a dead guy
			pm->maxs[2] = pm->ps->maxs[2] = pm->ps->crouchMaxZ;

			// make sure we play normal death anim
			BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_ENEMY_WEAPON, 0, qtrue );
			BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_ENEMY_POSITION, 0, qtrue );

			pm->ps->pm_time = BG_AnimScriptEvent(pm->ps, pm->character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue);

			pm->pmext->playDeadTime = -pm->cmd.serverTime;

			// set view forward ( eg to be 0 )
			pm->ps->delta_angles[1] = -pm->cmd.angles[1];
			pm->ps->pm_type = PM_DEAD;
			return qtrue;
		}
	}
	else {

		if ( pm->cmd.serverTime - -pm->pmext->playDeadTime < 1750 ) {
			return qfalse;
		}

		// see if we have the space to stop playing dead
		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];
		pm->maxs[2] = pm->ps->crouchMaxZ;

		pm->ps->eFlags &= ~EF_PLAYDEAD;
		PM_TraceAll(&trace, pm->ps->origin, pm->ps->origin);
		pm->ps->eFlags |= EF_PLAYDEAD;

		if(trace.allsolid) {
			return qfalse;
		}

		// crouch for a bit
		pm->ps->pm_flags |= PMF_DUCKED;

		// turn the hitbox back up
		pm->maxs[2] = pm->ps->maxs[2] = pm->ps->standViewHeight;

		// stop playdead
		pm->ps->eFlags &= ~EF_PLAYDEAD;
		pm->ps->eFlags &= ~EF_DEAD;

		pm->ps->pm_flags |= PMF_TIME_LOCKPLAYER;
		pm->ps->pm_time = BG_AnimScriptEvent(pm->ps, pm->character->animModelInfo, ANIM_ET_RAISE, qfalse, qtrue);

		pm->pmext->playDeadTime = pm->cmd.serverTime;
		pm->ps->pm_type = PM_NORMAL;
		pm->ps->viewangles[1] += SHORT2ANGLE(pm->ps->stats[STAT_DEAD_YAW]);
		return qtrue;
	}
	return qfalse;
}

/*
==============
PM_CheckProne

Sets mins, maxs, and pm->ps->viewheight
==============
*/

static qboolean PM_CheckProne (void)
{
	if( !(pm->ps->eFlags & EF_PRONE) ) {

		// can't go prone on ladders
		if( pm->ps->pm_flags & PMF_LADDER ) {
			return qfalse;
		}

		// no prone when using mg42's
		if( pm->ps->persistant[PERS_HWEAPON_USE] || pm->ps->eFlags & EF_MOUNTEDTANK ) {
			return qfalse;
		}

		if( pm->ps->weaponDelay && IS_PANZER_WEAPON(pm->ps->weapon) ) {
			return qfalse;
		}

		if( pm->ps->weapon == WP_MORTAR_SET || pm->ps->weapon == WP_MORTAR2_SET ) {
			return qfalse;
		}

		// can't go prone while swimming
		if( pm->waterlevel > 1 ) {
			return qfalse;
		}

		if(((pm->ps->pm_flags & PMF_DUCKED &&
			pm->cmd.doubleTap == DT_FORWARD) ||
			(pm->cmd.wbuttons & WBUTTON_PRONE)) &&
			!pm->pmext->kickDelay &&
			pm->cmd.serverTime - -pm->pmext->proneTime > ((PM_REALISM & REALISM_EXTENDED_PRONE) ? 1300 : 750 ) ) {

			trace_t trace;
			pm->mins[0] = pm->ps->mins[0];
			pm->mins[1] = pm->ps->mins[1];

			pm->maxs[0] = pm->ps->maxs[0];
			pm->maxs[1] = pm->ps->maxs[1];

			pm->mins[2] = pm->ps->mins[2];
			pm->maxs[2] = pm->ps->crouchMaxZ;

			pm->ps->eFlags |= EF_PRONE;
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			pm->ps->eFlags &= ~EF_PRONE;

			if(trace.fraction == 1.0f) {
				// go prone
				if (PM_REALISM & REALISM_EXTENDED_PRONE) // play animation, this is freefall and player is not supposed to be freezed
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DIVE, qfalse, qtrue ); // DIVE

				pm->ps->pm_flags |= PMF_DUCKED;	// crouched as well
				pm->ps->eFlags |= EF_PRONE;
				pm->pmext->proneTime = pm->cmd.serverTime;	// timestamp 'go prone'
				pm->pmext->proneGroundTime = pm->cmd.serverTime;

			}
		}
	}

	if( pm->ps->eFlags & EF_PRONE ) {
		if( pm->waterlevel > 1 ||
			pm->ps->pm_type == PM_DEAD ||
			pm->ps->eFlags & EF_MOUNTEDTANK ||
			((pm->cmd.doubleTap == DT_BACK ||
			  pm->cmd.upmove > 10 ||
			  pm->cmd.wbuttons & WBUTTON_PRONE)
			 && pm->cmd.serverTime - pm->pmext->proneTime > ((PM_REALISM & REALISM_EXTENDED_PRONE) ? 1300 : 750 ) )) {

			trace_t trace;

			// see if we have the space to stop prone
			pm->mins[0] = pm->ps->mins[0];
			pm->mins[1] = pm->ps->mins[1];

			pm->maxs[0] = pm->ps->maxs[0];
			pm->maxs[1] = pm->ps->maxs[1];

			pm->mins[2] = pm->ps->mins[2];

			pm->ps->eFlags &= ~EF_PRONE;
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			pm->ps->eFlags |= EF_PRONE;

			if(trace.fraction == 1.0f) {
				// crouch for a bit
				pm->ps->pm_flags |= PMF_DUCKED;

				// stop prone
				pm->ps->eFlags &= ~EF_PRONE;
				pm->ps->eFlags &= ~EF_PRONE_MOVING;
				pm->pmext->proneTime = -pm->cmd.serverTime;	// timestamp 'stop prone'

				if (PM_REALISM & REALISM_EXTENDED_PRONE) { // freeze player according to animation lenght
					if ( pm->waterlevel <= 1 &&
						pm->ps->pm_type != PM_DEAD &&
						!(pm->ps->eFlags & EF_MOUNTEDTANK)  ){
							pm->ps->pm_flags |= PMF_TIME_LOCKPLAYER;
							pm->pmext->proneRaiseTime = pm->ps->pm_time = BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_PRONE_TO_CROUCH, qfalse, qtrue ); // PRONE_TO_CROUCH
							pm->cmd.rightmove = 0;
							pm->cmd.forwardmove = 0;
						}
				}
				else {
					pm->pmext->proneRaiseTime = 0;
				}

				// switch from these weapons since we're no longer prone
				if (pm->ps->weapon == WP_MOBILE_BROWNING_SET ) {
					PM_BeginWeaponChange( WP_MOBILE_BROWNING_SET, WP_MOBILE_BROWNING, qfalse );
				}

				if (pm->ps->weapon == WP_MOBILE_MG42_SET ) {
					PM_BeginWeaponChange( WP_MOBILE_MG42_SET, WP_MOBILE_MG42, qfalse );
				}

				if (pm->ps->weapon == WP_BAR_SET ) {
					PM_BeginWeaponChange( WP_BAR_SET, WP_BAR, qfalse );
				}

				// don't jump for a bit
				pm->pmext->jumpTime = pm->cmd.serverTime - 650;
				pm->ps->jumpTime	= pm->cmd.serverTime - 650;
			}
		}
	}

	if( pm->ps->eFlags & EF_PRONE ) {

		// See if we are moving
		float spd = VectorLength( pm->ps->velocity );
		qboolean userinput = abs(pm->cmd.forwardmove) + abs(pm->cmd.rightmove) > 10 ? qtrue : qfalse;

		if( userinput && spd > 40.f && !(pm->ps->eFlags & EF_PRONE_MOVING) ) {
			pm->ps->eFlags |= EF_PRONE_MOVING;

			// Lose the scope view if moving too fast while prone
			switch( pm->ps->weapon ) {
				case WP_FG42SCOPE:
					PM_BeginWeaponChange( WP_FG42SCOPE, WP_FG42, qfalse );
					break;
				case WP_GARAND_SCOPE:
					PM_BeginWeaponChange( WP_GARAND_SCOPE, WP_GARAND, qfalse );
					break;
				case WP_K43_SCOPE:
					PM_BeginWeaponChange( WP_K43_SCOPE, WP_K43, qfalse );
					break;
			}
		}
		else if( !userinput && spd < 20.0f && (pm->ps->eFlags & EF_PRONE_MOVING) ) {
			pm->ps->eFlags &= ~EF_PRONE_MOVING;
		}

		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];


		//pm->maxs[2] = pm->ps->maxs[2] - pm->ps->standViewHeight - PRONE_VIEWHEIGHT;
		// tjw: it appears that 12 is the magic number
		//      for the minimum maxs[2] that prevents
		//      player from getting stuck into the world.
		pm->maxs[2] = 12;
		pm->ps->viewheight = PRONE_VIEWHEIGHT;

		return( qtrue );
	}

	return( qfalse );
}

//============================================================================

/*
===================
PM_WaterJumpMove

Flying out of the water
===================
*/
static void PM_WaterJumpMove( void ) {
	// waterjump has no control, but falls

	PM_StepSlideMove( qtrue );

	pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	if (pm->ps->velocity[2] < 0) {
		// cancel as soon as we are falling down again
		pm->ps->pm_flags &= ~PMF_ALL_TIMES;
		pm->ps->pm_time = 0;
	}
}

/*
===================
PM_WaterMove

===================
*/
static void PM_WaterMove( void ) {
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;

	if ( PM_CheckWaterJump() ) {
		PM_WaterJumpMove();
		return;
	}

	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );
	//
	// user intentions
	//
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = -60;		// sink towards bottom
	}
	else {

		wishvel[0] = scale * pml.forward[0]*pm->cmd.forwardmove + scale * pml.right[0]*pm->cmd.rightmove;
		wishvel[1] = scale * pml.forward[1]*pm->cmd.forwardmove + scale * pml.right[1]*pm->cmd.rightmove;
		wishvel[2] = scale * pml.forward[2]*pm->cmd.forwardmove + scale * pml.right[2]*pm->cmd.rightmove;

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if ( pm->watertype == CONTENTS_SLIME ) {	//----(SA)	slag
		if ( wishspeed > pm->ps->speed * pm_slagSwimScale ) {
			wishspeed = pm->ps->speed * pm_slagSwimScale;
		}

		PM_Accelerate (wishdir, wishspeed, pm_slagaccelerate);
	}
	else {
		if ( wishspeed > pm->ps->speed * pm_waterSwimScale ) {
			wishspeed = pm->ps->speed * pm_waterSwimScale;
		}

		PM_Accelerate (wishdir, wishspeed, pm_wateraccelerate);
	}


	// make sure we can go up slopes easily under water
	if ( pml.groundPlane && DotProduct( pm->ps->velocity, pml.groundTrace.plane.normal ) < 0 ) {
		float vel = VectorLength(pm->ps->velocity);
		// slide along the ground plane
		PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
			pm->ps->velocity, OVERCLIP );

		VectorNormalize(pm->ps->velocity);
		VectorScale(pm->ps->velocity, vel, pm->ps->velocity);
	}

	PM_SlideMove( qfalse );
}

/*
===================
PM_FlyMove

Only with the flight powerup
===================
*/
static void PM_FlyMove( void ) {
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;

	// normal slowdown
	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );

	// user intentions
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = 0;
	}
	else {
		wishvel[0] = scale * pml.forward[0]*pm->cmd.forwardmove + scale * pml.right[0]*pm->cmd.rightmove;
		wishvel[1] = scale * pml.forward[1]*pm->cmd.forwardmove + scale * pml.right[1]*pm->cmd.rightmove;
		wishvel[2] = scale * pml.forward[2]*pm->cmd.forwardmove + scale * pml.right[2]*pm->cmd.rightmove;

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	PM_Accelerate (wishdir, wishspeed, pm_flyaccelerate);

	PM_StepSlideMove( qfalse );
}

/*
===================
PM_AirMove
===================
*/
static void PM_AirMove( void ) {
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;

	if ( PM_CheckDoubleJump () ) {
		pm->ps->pm_flags |=  PMF_DOUBLEJUMPING;
		// jumped away
		if ( pm->waterlevel > 1 ) {
			PM_WaterMove();
		}
		// this will probably never happen so the second jump won't
		// drain your sprint meter. Just like shrub.
		// Take out the if {} to make it drain
		if (!(pm->cmd.serverTime - pm->pmext->jumpTime < 850)) {

			pm->pmext->sprintTime -= 2500;
			if (pm->pmext->sprintTime < 0)
				pm->pmext->sprintTime = 0;

			pm->pmext->jumpTime = pm->cmd.serverTime;
		}

		// JPW NERVE
		pm->ps->jumpTime = pm->cmd.serverTime;	// Arnout: NOTE : TEMP DEBUG

		//return;
	}


	PM_Friction();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	// project moves down to flat plane
	if( pm->cmd.serverTime - pm->pmext->dodgeTime < 350 ) {
		pml.forward[2] = fmove = 0;
		smove = pm->pmext->dtmove == DT_MOVELEFT ? -2070 : 2070;
		scale = 1.f;
	}
	else {
		usercmd_t cmd = pm->cmd;
		scale = PM_CmdScale( &cmd );

		pml.forward[2] = 0;
		pml.right[2] = 0;
	}
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	wishvel[0] = pml.forward[0]*fmove + pml.right[0]*smove;
	wishvel[1] = pml.forward[1]*fmove + pml.right[1]*smove;

	wishvel[2] = 0;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// not on ground, so little effect on velocity
	PM_Accelerate (wishdir, wishspeed, pm_airaccelerate);

	// we may have a ground plane that is very steep, even
	// though we don't have a groundentity slide along the steep plane
	if ( pml.groundPlane ) {
		PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
			pm->ps->velocity, OVERCLIP );
	}

	PM_StepSlideMove ( qtrue );

	// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();
}

/*
===================
PM_WalkMove

===================
*/
static void PM_WalkMove( void ) {
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;
	float		accelerate;
	float		vel;

	if ( pm->waterlevel > 2 && DotProduct( pml.forward, pml.groundTrace.plane.normal ) > 0 ) {
		// begin swimming
		PM_WaterMove();
		return;
	}

	if ( PM_CheckJump () ) {
		// jumped away
		if ( pm->waterlevel > 1 ) {
			PM_WaterMove();
		}
		else {
			PM_AirMove();
		}

		if (!(pm->cmd.serverTime - pm->pmext->jumpTime < 850)) {

			pm->pmext->sprintTime -= 2500;
			if (pm->pmext->sprintTime < 0)
				pm->pmext->sprintTime = 0;

			pm->pmext->jumpTime = pm->cmd.serverTime;
		}

		// JPW NERVE
		pm->ps->jumpTime = pm->cmd.serverTime;	// Arnout: NOTE : TEMP DEBUG

		return;
	}

	PM_Friction ();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale( &cmd );

	// project moves down to flat plane
	pml.forward[2] = 0;
	pml.right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP );
	PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP );
	//
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	wishvel[0] = pml.forward[0]*fmove + pml.right[0]*smove;
	wishvel[1] = pml.forward[1]*fmove + pml.right[1]*smove;
	wishvel[2] = pml.forward[2]*fmove + pml.right[2]*smove;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// clamp the speed lower if prone
	if ( pm->ps->eFlags & EF_PRONE ) {
		if ( wishspeed > pm->ps->speed * pm_proneSpeedScale ) {
			wishspeed = pm->ps->speed * pm_proneSpeedScale;
		}
	}
	else if ( pm->ps->pm_flags & PMF_DUCKED ) { // clamp the speed lower if ducking
		if ( wishspeed > pm->ps->speed * pm->ps->crouchSpeedScale ) {
			wishspeed = pm->ps->speed * pm->ps->crouchSpeedScale;
		}
	}

	// clamp the speed lower if wading or walking on the bottom
	if ( pm->waterlevel ) {
		float	waterScale;

		waterScale = pm->waterlevel / 3.0;
		if ( pm->watertype == CONTENTS_SLIME )	//----(SA)	slag
			waterScale = 1.0 - ( 1.0 - pm_slagSwimScale ) * waterScale;
		else
			waterScale = 1.0 - ( 1.0 - pm_waterSwimScale ) * waterScale;

		if ( wishspeed > pm->ps->speed * waterScale ) {
			wishspeed = pm->ps->speed * waterScale;
		}
	}

	// when a player gets hit, they temporarily lose
	// full control, which allows them to be moved a bit
	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK ) {
		accelerate = pm_airaccelerate;
	}
	else {
		accelerate = pm_accelerate;
	}

	PM_Accelerate (wishdir, wishspeed, accelerate);

	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK ) {
		pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	}

	//----(SA)	added
	// show breath when standing on 'snow' surfaces
	if(pml.groundTrace.surfaceFlags & SURF_SNOW)
		pm->ps->eFlags |= EF_BREATH;
	else
		pm->ps->eFlags &= ~EF_BREATH;
	//----(SA)	end

	vel = VectorLength(pm->ps->velocity);

	// slide along the ground plane
	PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, pm->ps->velocity, OVERCLIP );

	// don't do anything if standing still
	if (!pm->ps->velocity[0] && !pm->ps->velocity[1]) {
		if( pm->ps->eFlags & EF_PRONE ) {
			pm->pmext->proneGroundTime = pm->cmd.serverTime;
		}
		return;
	}

	// don't decrease velocity when going up or down a slope
	VectorNormalize(pm->ps->velocity);
	VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

	PM_StepSlideMove( qfalse );

	// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();
}

/*
==============
PM_DeadMove
==============
*/
static void PM_DeadMove( void ) {
	float	forward;

	if ( !pml.walking ) {
		return;
	}

	// extra friction

	forward = VectorLength (pm->ps->velocity);
	forward -= 20;
	if ( forward <= 0 ) {
		VectorClear (pm->ps->velocity);
	}
	else {
		VectorNormalize (pm->ps->velocity);
		VectorScale (pm->ps->velocity, forward, pm->ps->velocity);
	}

}

/*
===============
PM_NoclipMove
===============
*/
static void PM_NoclipMove( void ) {
	float		speed;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;

	pm->ps->viewheight = DEFAULT_VIEWHEIGHT;

	// friction

	speed = VectorLength (pm->ps->velocity);
	if (speed < 1) {
		VectorCopy (vec3_origin, pm->ps->velocity);
	}
	else {
		float newspeed;
		float drop = 0;
		float friction = pm_friction*1.5;	// extra friction
		float control = speed < pm_stopspeed ? pm_stopspeed : speed;

		drop += control*friction*pml.frametime;

		// scale the velocity
		newspeed = speed - drop;
		if (newspeed < 0) {
			newspeed = 0;
		}
		newspeed /= speed;

		VectorScale (pm->ps->velocity, newspeed, pm->ps->velocity);
	}

	// accelerate
	scale = PM_CmdScale( &pm->cmd );

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	wishvel[0] = pml.forward[0]*fmove + pml.right[0]*smove;
	wishvel[1] = pml.forward[1]*fmove + pml.right[1]*smove;
	wishvel[2] = pml.forward[2]*fmove + pml.right[2]*smove;

	wishvel[2] += pm->cmd.upmove;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate( wishdir, wishspeed, pm_accelerate );

	// move
	VectorMA (pm->ps->origin, pml.frametime, pm->ps->velocity, pm->ps->origin);
}

//============================================================================

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
static int PM_FootstepForSurface( void )
{
#ifdef GAMEDLL
	// In just the GAME DLL, we want to store the groundtrace surface stuff,
	// so we don't have to keep tracing.
	ClientStoreSurfaceFlags(pm->ps->clientNum, pml.groundTrace.surfaceFlags);

#endif // GAMEDLL

	return BG_FootstepForSurface( pml.groundTrace.surfaceFlags );
}

/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
static void PM_CrashLand( void ) {
	float		delta;
	float		dist;
	float		vel, acc;
	float		t;
	float		a, b, c, den;

	// Ridah, only play this if coming down hard
	if (!pm->ps->legsTimer)	{
		if (pml.previous_velocity[2] < -220) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_LAND, qfalse, qtrue );
		}
	}

	// calculate the exact velocity on landing
	dist = pm->ps->origin[2] - pml.previous_origin[2];
	vel = pml.previous_velocity[2];
	acc = -pm->ps->gravity;

	a = acc / 2;
	b = vel;
	c = -dist;

	den =  b * b - 4 * a * c;
	if ( den < 0 ) {
		return;
	}
	t = (-b - sqrt( den ) ) / ( 2 * a );

	delta = vel + t * acc;
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if ( pm->waterlevel == 3 ) {
#ifdef GAMEDLL // nothing to predict
		pm->pmext->shoved = qfalse;
#endif
		return;
	}

	// reduce falling damage if there is standing water
	if ( pm->waterlevel == 2 ) {
		delta *= 0.25;
	}
	if ( pm->waterlevel == 1 ) {
		delta *= 0.5;
	}

	if ( delta < 1 ) {
		return;
	}

	// create a local entity event to play the sound

	// SURF_NODAMAGE is used for bounce pads where you don't ever
	// want to take damage or play a crunch sound
	if(!(pml.groundTrace.surfaceFlags & SURF_NODAMAGE) && !pm->predict)  {
		if ( pm->debugLevel )
			Com_Printf ("delta: %5.2f\n", delta);

		if (delta > 77) {
			PM_AddEventExt( EV_FALL_NDIE, PM_FootstepForSurface() );
		}
		else if (delta > 67) {
			PM_AddEventExt( EV_FALL_DMG_50, PM_FootstepForSurface() );
		}
		else if (delta > 58) {
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 )
			{
				PM_AddEventExt( EV_FALL_DMG_25, PM_FootstepForSurface() );
			}
		}
		else if (delta > 48) {
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 ) {
				PM_AddEventExt( EV_FALL_DMG_15, PM_FootstepForSurface() );
			}
		}
		else if (delta > 38.75) {
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 ) {
				PM_AddEventExt( EV_FALL_DMG_10, PM_FootstepForSurface() );
			}
		}
		else if ( delta > 7 ) {
			PM_AddEventExt( EV_FALL_SHORT, PM_FootstepForSurface() );
		}
		else if ( !CheckSilent()) {
			PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
		}
	}

	// rain - when falling damage happens, velocity is cleared, but
	// this needs to happen in pmove, not g_active!  (prediction will be
	// wrong, otherwise.)
	if (delta > 38.75)
		VectorClear(pm->ps->velocity);

	// start footstep cycle over
	pm->ps->bobCycle = 0;
}

/*
=============
PM_CorrectAllSolid
=============
*/
static int PM_CorrectAllSolid( trace_t *trace )
{
	int			i, j, k;
	vec3_t		point;

	if ( pm->debugLevel ) {
		Com_Printf("%i:allsolid\n", c_pmove);
	}

	// jitter around
 	for (i = -1; i <= 1; ++i) {
		for (j = -1; j <= 1; ++j) {
			for (k = -1; k <= 1; ++k) {
				if ( !i && !j && !k ) continue;	// same as checking input origin again..
				VectorCopy(pm->ps->origin, point);
				point[0] += (float) i;
				point[1] += (float) j;
				point[2] += (float) k;
				PM_TraceAll(trace, point, point);
				if ( !trace->allsolid ) {
					point[0] = pm->ps->origin[0];
					point[1] = pm->ps->origin[1];
					point[2] = pm->ps->origin[2] - 0.25;

					PM_TraceAll(trace, pm->ps->origin, point);
					pml.groundTrace = *trace;
					return qtrue;
				}
			}
		}
	}

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pml.groundPlane = qfalse;
	pml.walking = qfalse;

	return qfalse;
}

/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
static void PM_GroundTraceMissed( void )
{
	trace_t		trace;

	if ( pm->ps->groundEntityNum != ENTITYNUM_NONE ) {
		vec3_t		point;

		// we just transitioned into freefall
		if ( pm->debugLevel ) {
			Com_Printf("%i:lift\n", c_pmove);
		}

		// if they aren't in a jumping animation and the ground is a ways away, force into it
		// if we didn't do the trace, the player would be backflipping down staircases
		VectorCopy( pm->ps->origin, point );
		point[2] -= 64;

		PM_TraceAll( &trace, pm->ps->origin, point );
		if ( trace.fraction == 1.0 ) {
			if ( pm->cmd.forwardmove >= 0 ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
				pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
			}
			else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
				pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
			}
		}
	}

	// If we've never yet touched the ground, it's because we're spawning, so don't
	// set to "in air"
	if (pm->ps->groundEntityNum != -1) {
		// Signify that we're in mid-air
		pm->ps->groundEntityNum = ENTITYNUM_NONE;

	} // if (pm->ps->groundEntityNum != -1)...
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
}

/*
=============
PM_GroundTrace
=============
*/
static void PM_GroundTrace( void )
{
	vec3_t		point;
	trace_t		trace;

	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];


	if ( pm->ps->eFlags & EF_MG42_ACTIVE ) {
		point[2] = pm->ps->origin[2] - 1.f;
	}
	else {
		point[2] = pm->ps->origin[2] - 0.25f;
	}

	PM_TraceAllParts( &trace, &pm->pmext->proneLegsOffset, pm->ps->origin, point );
	pml.groundTrace = trace;

	// do something corrective if the trace starts in a solid...
	if ( trace.allsolid && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
		if ( !PM_CorrectAllSolid(&trace) )
			return;
	}

	// if the trace didn't hit anything, we are in free fall
	if ( trace.fraction == 1.0 ) {
		PM_GroundTraceMissed();
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// check if getting thrown off the ground
	if ( pm->ps->velocity[2] > 0 && DotProduct( pm->ps->velocity, trace.plane.normal ) > 10 && !( pm->ps->eFlags & EF_PRONE ) ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:kickoff\n", c_pmove);
		}
		// go into jump animation
		if ( pm->cmd.forwardmove >= 0 ) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qfalse );
			pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
		}
		else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qfalse );
			pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
		}

		pm->ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// slopes that are too steep will not be considered onground
	// jet Pilot - TODO High battle sense enables walking on steeper slopes
	if ( trace.plane.normal[2] < MIN_WALK_NORMAL) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:steep\n", c_pmove);
		}
		// FIXME: if they can't slide down the slope, let them
		// walk (sharp crevices)
		pm->ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qtrue;
		pml.walking = qfalse;
		return;
	}

	pml.groundPlane = qtrue;
	pml.walking = qtrue;
	pm->ps->pm_flags &= ~PMF_DOUBLEJUMPING;

	// hitting solid ground will end a waterjump
	if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) {
		pm->ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
		pm->ps->pm_time = 0;
	}

	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
		// just hit the ground
		if ( pm->debugLevel ) {
			Com_Printf("%i:Land\n", c_pmove);
		}

		PM_CrashLand();

		// don't do landing time if we were just going down a slope
		if ( pml.previous_velocity[2] < -200 ) {
			// don't allow another jump for a little while
			pm->ps->pm_flags |= PMF_TIME_LAND;
			pm->ps->pm_time = 250;

		}
	}

	pm->ps->groundEntityNum = trace.entityNum;
	PM_AddTouchEnt( trace.entityNum );
}

/*
=============
PM_SetWaterLevel	FIXME: avoid this twice?  certainly if not moving
=============
*/
static void PM_SetWaterLevel( void ) {
	vec3_t		point;
	int			cont;
	//
	// get waterlevel, accounting for ducking
	//
	pm->waterlevel = 0;
	pm->watertype = 0;

	// Ridah, modified this
	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];
	point[2] = pm->ps->origin[2] + pm->ps->mins[2] + 1;
	cont = pm->pointcontents( point, pm->ps->clientNum );

	if ( cont & MASK_WATER ) {
		int	sample2 = pm->ps->viewheight - pm->ps->mins[2];
		int sample1 = sample2 / 2;

		pm->watertype = cont;
		pm->waterlevel = 1;
		point[2] = pm->ps->origin[2] + pm->ps->mins[2] + sample1;
		cont = pm->pointcontents (point, pm->ps->clientNum );
		if ( cont & MASK_WATER ) {
			pm->waterlevel = 2;
			point[2] = pm->ps->origin[2] + pm->ps->mins[2] + sample2;
			cont = pm->pointcontents (point, pm->ps->clientNum );
			if ( cont & MASK_WATER ){
				pm->waterlevel = 3;
			}
		}
	}
	// done.

	// UNDERWATER
	BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_UNDERWATER, (pm->waterlevel > 2), qtrue );
}

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/
static void PM_CheckDuck (void)
{
	// Ridah, modified this for configurable bounding boxes
	pm->mins[0] = pm->ps->mins[0];
	pm->mins[1] = pm->ps->mins[1];

	pm->maxs[0] = pm->ps->maxs[0];
	pm->maxs[1] = pm->ps->maxs[1];

	pm->mins[2] = pm->ps->mins[2];

	if( pm->ps->pm_type == PM_DEAD ) {
		pm->maxs[2] = pm->ps->maxs[2];			// NOTE: must set death bounding box in game code
		pm->ps->viewheight = pm->ps->deadViewHeight;
		return;
	}

	if( (pm->cmd.upmove < 0 && !pm->pmext->kickDelay && !(pm->ps->eFlags & EF_MOUNTEDTANK) && !(pm->ps->pm_flags & PMF_LADDER) /*&& pm->ps->pm_type != PM_JETPILOT */) || pm->ps->weapon == WP_MORTAR_SET || pm->ps->weapon == WP_MORTAR2_SET || pm->ps->pm_flags & PMF_MEDICUSE  ) {	// duck
		pm->ps->pm_flags |= PMF_DUCKED;
	}
	else {
		// stand up if possible
		if (pm->ps->pm_flags & PMF_DUCKED) {
			trace_t	trace;

			// try to stand up
			pm->maxs[2] = pm->ps->maxs[2];
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			if (trace.fraction == 1.0f)
				pm->ps->pm_flags &= ~PMF_DUCKED;
		}
	}

	if (pm->ps->pm_flags & PMF_DUCKED) {
		pm->maxs[2] = pm->ps->crouchMaxZ;
		pm->ps->viewheight = pm->ps->crouchViewHeight;
	}
	else {
		pm->maxs[2] = pm->ps->maxs[2];
		pm->ps->viewheight = pm->ps->standViewHeight;
	}
	// done.
}

//===================================================================

/*
===============
PM_Footsteps
===============
*/
static void PM_Footsteps( void )
{
	float		bobmove;
	int			old;
	qboolean	footstep;
	qboolean	iswalking;
	int			animResult=-1;

	if ( pm->ps->eFlags & EF_PLAYDEAD ) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_DEAD, qtrue );
		return;
	}

	if (pm->ps->eFlags & EF_DEAD) {
		if( pm->ps->pm_flags & PMF_FLAILING ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FLAILING, qtrue );

			if( !pm->ps->pm_time )
				pm->ps->pm_flags &= ~PMF_FLAILING;	// the eagle has landed
		}
		else if(!pm->ps->pm_time && !(pm->ps->pm_flags & PMF_LIMBO) ) {// DHM - Nerve :: before going to limbo, play a wounded/fallen animation

			if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
				// takeoff!
				pm->ps->pm_flags |= PMF_FLAILING;
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FLAILING, qtrue );
			}
			else {
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FALLEN, qtrue );
			}
		}

		return;
	}

	iswalking = qfalse;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	pm->xyspeed = sqrt( pm->ps->velocity[0] * pm->ps->velocity[0] +  pm->ps->velocity[1] * pm->ps->velocity[1] );

	// mg42, always idle
	if ( pm->ps->persistant[PERS_HWEAPON_USE] ) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
		//
		return;
	}

	// medic reviving
	if ( pm->ps->pm_flags & PMF_MEDICUSE ) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_REVIVE, qtrue );
		return;
	}

	// swimming
	if ( pm->waterlevel > 2 ) {
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_SWIMBK, qtrue );
		}
		else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_SWIM, qtrue );
		}

		return;
	}

	// in the air
	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
		if(pm->ps->pm_flags & PMF_LADDER) {			// on ladder

			if (pm->ps->velocity[2] >= 0) {
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_CLIMBUP, qtrue );
			}
			else if (pm->ps->velocity[2] < 0) {
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_CLIMBDOWN, qtrue );
			}
		}

		return;
	}

	// if not trying to move
	if ( !pm->cmd.forwardmove && !pm->cmd.rightmove ) {
		if (  pm->xyspeed < 5 ) {
			pm->ps->bobCycle = 0;	// start at beginning of cycle again
		}

		if (pm->xyspeed > 120) {
			return;	// continue what they were doing last frame, until we stop
		}

		if ( pm->ps->eFlags & EF_PRONE ) {
			if ( pm->ps->eFlags & EF_TALK )
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RADIOPRONE, qtrue );
			else
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLEPRONE, qtrue );
		}
		else if ( pm->ps->pm_flags & PMF_DUCKED ) {
			if ( pm->ps->eFlags & EF_TALK )
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RADIOCR, qtrue );
			else
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLECR, qtrue );
		}

		if (animResult < 0) {
			if ( pm->ps->eFlags & EF_TALK )
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RADIO, qtrue );
			else
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
		}

		return;
	}

	footstep = qfalse;


	if ( pm->ps->eFlags & EF_PRONE ) {
		bobmove = 0.2;	// prone characters bob slower
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_PRONEBK, qtrue );
		}
		else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_PRONE, qtrue );
		}
		// prone characters never play footsteps
	}
	else if ( pm->ps->pm_flags & PMF_DUCKED ) {
		bobmove = 0.5;	// ducked characters bob much faster
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKCRBK, qtrue );
		}
		else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKCR, qtrue );
		}
		// ducked characters never play footsteps
	}
	else if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
		if ( !( pm->cmd.buttons & BUTTON_WALKING ) ) {
			bobmove = 0.4;	// faster speeds bob faster

			// jet Pilot - Lvl 6 covert ops make no sound at normal speed
			if ( !CheckSilent() ) {
                footstep = qtrue;
			}

			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				}
				else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) { // if we havent found an anim yet, play the run

				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RUNBK, qtrue );
			}
		}
		else {
			bobmove = 0.3;
			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				}
				else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {
				// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKBK, qtrue );
			}
		}

	}
	else {
		if ( !( pm->cmd.buttons & BUTTON_WALKING ) ) {
			bobmove = 0.4;	// faster speeds bob faster
			footstep = qtrue;
			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				}
				else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RUN, qtrue );
			}
		}
		else {
			bobmove = 0.3;	// walking bobs slow
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				}
				else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALK, qtrue );
			}
		}
	}

	// if no anim found yet, then just use the idle as default
	if (animResult < 0) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
	}

	// check for footstep / splash sounds
	old = pm->ps->bobCycle;
	pm->ps->bobCycle = (int)( old + bobmove * pml.msec ) & 255;

	// if we just crossed a cycle boundary, play an apropriate footstep event
	if (iswalking) {
		// sounds much more natural this way
		if (old > pm->ps->bobCycle) {
			if ( pm->waterlevel == 0 ) {
				if ( footstep && !CheckSilent() ) {
					PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
				}
			}
			else if ( pm->waterlevel == 1 && !CheckSilent() ) {
				// splashing
				PM_AddEvent( EV_FOOTSPLASH );
			}
			else if ( pm->waterlevel == 2 && !CheckSilent() ) {
				// wading / swimming at surface
				PM_AddEvent( EV_SWIM );
			}
		}
	}
	else if ( ( ( old + 64 ) ^ ( pm->ps->bobCycle + 64 ) ) & 128 ) {
		if ( pm->waterlevel == 0 ) {
			// on ground will only play sounds if running
			// jet Pilot - Lvl 7 covert ops make no sound while moving
			if ( footstep && !CheckSilent()) {
				PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
			}
		}
		else if ( pm->waterlevel == 1 && !CheckSilent()) {
			// splashing
			PM_AddEvent( EV_FOOTSPLASH );
		}
		else if ( pm->waterlevel == 2 && !CheckSilent()) {
			// wading / swimming at surface
			PM_AddEvent( EV_SWIM );
		}
	}
}

/*
==============
PM_WaterEvents

Generate sound events for entering and leaving water
==============
*/
static void PM_WaterEvents( void ) {		// FIXME?
	// if just entered a water volume, play a sound
	if (!pml.previous_waterlevel && pm->waterlevel && !CheckSilent()) {
		PM_AddEvent( EV_WATER_TOUCH );
	}

	// if just completely exited a water volume, play a sound
	if (pml.previous_waterlevel && !pm->waterlevel && !CheckSilent()) {
		PM_AddEvent( EV_WATER_LEAVE );
	}

	// check for head just going under water
	if (pml.previous_waterlevel != 3 && pm->waterlevel == 3) {
		PM_AddEventExt( EV_WATER_UNDER, CheckSilent() ? 0 : 1 );
	}

	// check for head just coming out of water
	if (pml.previous_waterlevel == 3 && pm->waterlevel != 3 && !CheckSilent()) {
		PM_AddEventExt( EV_WATER_CLEAR,( pm->pmext->airleft < 6000 ) ? 1 : 0 );
	}
}

/*
=================
PM_BeginShotgunReload - jaquboss - special function for shotgun
=================
*/
static void PM_BeginShotgunReload( void )
{
	int anim;

	// start animation - FIXME do own animation - should be just turning shotgun, nothing more
	if ( pm->ps->eFlags & EF_PRONE ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE_SG1, qfalse, qtrue );
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD_SG1, qfalse, qtrue );
	}


	if ( pm->ps->ammoclip[WeaponTable[WP_SHOTGUN].clipindex] == 0 )
		anim = WEAP_ALTSWITCHFROM; // loading a first shell
	else
		anim = WEAP_RELOAD1; // classic


	PM_StartWeaponAnim(anim); // this is putdown anim
    pm->ps->weaponTime += BG_Weapons[pm->ps->weapon].animationTime[anim];


	if ( anim == WEAP_RELOAD1 ) {
		pm->ps->holdable[0] = SHOTGUN_RELOADING_BEGIN;
	}
	else {
		pm->ps->holdable[0] = SHOTGUN_RELOADING_BEGIN_1ST;
		PM_AddEvent( EV_SHOTGUN_PUMP );	// play reload sound
	}
	pm->ps->weaponstate = WEAPON_RELOADING;
}

/*
=================
PM_ContinueShotgunReload
=================
*/
static void PM_ContinueShotgunReload( void )
{
	// start animation - FIXME do own animation - here should be just filling gun
	if ( pm->ps->eFlags & EF_PRONE ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE_SG2, qfalse, qtrue );
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD_SG2, qfalse, qtrue );
	}

	PM_StartWeaponAnim(WEAP_RELOAD2);

    pm->ps->weaponTime += BG_Weapons[pm->ps->weapon].animationTime[WEAP_RELOAD2];


	pm->ps->holdable[0] = SHOTGUN_RELOADING_LOOP;
	PM_AddEvent( EV_FILL_CLIP );	// play reload sound
}

// transition animation
static void PM_Shotgun_PumpToLoop( void )
{
	PM_StartWeaponAnim(WEAP_ALTSWITCHTO);
    pm->ps->weaponTime += BG_Weapons[pm->ps->weapon].animationTime[WEAP_ALTSWITCHTO];
	pm->ps->holdable[0] = SHOTGUN_RELOADING_AFTER_1ST;
}

/*
=================
PM_StopShotgunReload
=================
*/
static void PM_StopShotgunReload( void )
{
	// start animation - FIXME do own animation
	if ( pm->ps->eFlags & EF_PRONE ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE_SG3, qfalse, qtrue );
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD_SG3, qfalse, qtrue );
	}

	PM_StartWeaponAnim(WEAP_RELOAD3);
    pm->ps->weaponTime += BG_Weapons[pm->ps->weapon].animationTime[WEAP_RELOAD3];
	pm->ps->holdable[0] = SHOTGUN_RELOADING_STOP;
}

static void PM_Shotgun_StopFromPump( void )
{
	// start animation - FIXME do own animation
	if ( pm->ps->eFlags & EF_PRONE ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE_SG3, qfalse, qtrue );
	}
	else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD_SG3, qfalse, qtrue );
	}

	PM_StartWeaponAnim(WEAP_DROP2);
    pm->ps->weaponTime += BG_Weapons[pm->ps->weapon].animationTime[WEAP_DROP2];
	pm->ps->holdable[0] = SHOTGUN_RELOADING_STOP;
}

/*
==============
PM_BeginWeaponReload
==============
*/
static void PM_BeginWeaponReload( int weapon ) {
	int		 reloadTime;

	// jaquboss, don't start reloads when not possible
	if ( pm->ps->ammo[WeaponTable[pm->ps->weapon].ammoindex] == 0 ){
		pm->ps->weaponstate = WEAPON_READY;
		return;
	}

	switch(weapon) {
		//case WP_GPG40:	// not the riflenades, otherwise spawning is corrupted..
		//case WP_M7:
		case WP_FLAMETHROWER:
		case WP_KNIFE:
		case WP_GRENADE_LAUNCHER:
		case WP_PANZERFAUST:
		case WP_GRENADE_PINEAPPLE:
		case WP_MEDIC_SYRINGE:
		case WP_AMMO:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_SMOKETRAIL:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_LANDMINE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_DUMMY_MG42:
		case WP_MEDIC_ADRENALINE:
		case WP_KNIFE_KABAR:
		case WP_BAZOOKA:
		case WP_POISON_SYRINGE:
		case WP_FOOTKICK:
		case WP_NONE:
			return;
		default:
			break;
	}

	// only allow reload if the weapon isn't already occupied (firing is okay)
	// jet Pilot - special case for reloading shotgun
	// what is point of allowing to reload weapon again when it is reloading ?!
	if(pm->ps->weaponstate != WEAPON_READY && pm->ps->weaponstate != WEAPON_FIRING && pm->ps->weaponstate != WEAPON_FIRINGALT ) {
		return;
	}

	switch(weapon) {
		// jet Pilot - Skill - Level 5+ Soldiers can reload MGs mid clip
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
			if (pm->ps->ammoclip[WP_MOBILE_MG42]	!= 0 && !(pm->skillBits[SK_HEAVY_WEAPONS] & (1<<5)) ) {
				return;
			}
			break;
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
			if (pm->ps->ammoclip[WP_MOBILE_BROWNING] != 0 && !(pm->skillBits[SK_HEAVY_WEAPONS] & (1<<5)) ) {
				return;
			}
			break;
		// jet Pilot - allow Garand to reload, Jaquboss - added back and inverted, make a note at docs please
		default:
			if ( BG_Weapons[weapon].noMCReload && pm->ps->ammoclip[WeaponTable[weapon].clipindex] != 0)
				return;
			break;
	}

	// (SA) easier check now that the animation system handles the specifics
	if ( !(weapon == WP_SHOTGUN && pm->ps->holdable[0] != SHOTGUN_NOT_RELOADING) ) {
		// DHM - Nerve :: override current animation (so reloading after firing will work)
		if( pm->ps->eFlags & EF_PRONE ) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE, qfalse, qtrue );
		}
		else {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD, qfalse, qtrue );
		}
	}

	if ( weapon == WP_SHOTGUN) {
		PM_BeginShotgunReload();
		return;
	}
    else if( weapon != WP_MORTAR && weapon != WP_MORTAR_SET && weapon != WP_MORTAR2 && weapon != WP_MORTAR2_SET ) {
		PM_ContinueWeaponAnim(PM_ReloadAnimForWeapon(pm->ps->weapon));
	}

    reloadTime = BG_Weapons[pm->ps->weapon].animationTime[PM_ReloadAnimForWeapon(pm->ps->weapon)];

	if( pm->ps->weaponstate == WEAPON_READY ) {
		pm->ps->weaponTime += reloadTime;
	}
	else if( pm->ps->weaponTime < reloadTime) {
		pm->ps->weaponTime += (reloadTime - pm->ps->weaponTime);
	}

	pm->ps->weaponstate = WEAPON_RELOADING;
	PM_AddEvent( EV_FILL_CLIP );	// play reload sound
}

static void PM_ReloadClip( int weapon );
static void PM_FinishWeaponChange( void );
/*
===============
PM_BeginWeaponChange
===============

SA - modified to play 1st person alt-mode transition animations.
*/
void PM_BeginWeaponChange( int oldweapon, int newweapon, qboolean reload )
{
	int			switchtime;
	qboolean	altSwitchAnim = qfalse;
	qboolean	fixSwitch = qfalse;

	if ( pm->ps->weapon == newweapon ) {
		return;
	}

	if ( pm->ps->nextWeapon == newweapon ) {
		return;
	}

	// don't allow weapon switch until all buttons are up
	if( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;
	}

	// don't allow switching to an undefined weapon
	if ( newweapon <= WP_NONE || newweapon >= WP_FOOTKICK ) {
		return;
	}

	// don't switch to a weapon if it's not in the inventory
	if( !( COM_BitCheck( pm->ps->weapons, newweapon ) ) ){
		return;
	}

	// don't allow switching while rasing or lowering a weapon
	if( pm->ps->weaponstate == WEAPON_DROPPING ||
		pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ||
		pm->ps->weaponstate == WEAPON_RAISING ||	// core
		pm->ps->weaponstate == WEAPON_HOLSTER_IN || // jaquboss
		pm->ps->weaponstate == WEAPON_RELOADING ) {
		return;
	}

	// Gordon: don't allow change during spinup
	if( pm->ps->weaponDelay ) {
		return;
	}

	// don't allow switch if holding a primed grenade or dynamite
	if( pm->ps->grenadeTimeLeft > 0 ) {
		return;
	}

	pm->ps->nextWeapon = newweapon;

	switch( newweapon ) {
		case WP_CARBINE:
		case WP_KAR98:
			if( newweapon != weapAlts[oldweapon] ) {
				PM_AddEvent( EV_CHANGE_WEAPON );
			}
			break;
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_DYNAMITE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
			// initialize the timer on the grenade you're switching to
			pm->ps->grenadeTimeLeft = 0;
			PM_AddEvent( EV_CHANGE_WEAPON );
			break;
		case WP_MORTAR2_SET:
		case WP_MORTAR_SET:
			// don't allow switching to a set mortar if prone or underwater
			if (( pm->ps->eFlags & EF_PRONE ) || ( pm->waterlevel == 3 )) {
				return;
			}
			PM_AddEvent( EV_CHANGE_WEAPON );
			break;

		default:
			//----(SA)	only play the weapon switch sound for the player
			PM_AddEvent( reload ? EV_CHANGE_WEAPON_2 : EV_CHANGE_WEAPON );
			break;
	}

	// it's an alt mode, play different anim
	if(newweapon == weapAlts[oldweapon]) {
		PM_StartWeaponAnim(PM_AltSwitchFromForWeapon(oldweapon));
	}
	else {
		PM_StartWeaponAnim(PM_DropAnimForWeapon(oldweapon));
	}

	// jaquboss check if we have loaded weapon animation cfg and use it to do correct time
	if ( newweapon == weapAlts[oldweapon] ) {
        switchtime =  BG_Weapons[oldweapon].animationTime[PM_AltSwitchFromForWeapon(oldweapon)];
	}
	else {
        switchtime =  BG_Weapons[oldweapon].animationTime[PM_DropAnimForWeapon(oldweapon)];
    }

	// sometimes different switch times for alt weapons
	switch(oldweapon) {
		// engineer carbines
		case WP_CARBINE:
		case WP_KAR98:
			if(newweapon == weapAlts[oldweapon]) {
				fixSwitch = qtrue;
				switchtime = 0;
				if(	!pm->ps->ammoclip[newweapon] && pm->ps->ammo[newweapon] ) {
					PM_ReloadClip( newweapon );
				}
			}
			break;
		// grenade launchers
		case WP_M7:
		case WP_GPG40:
			if(newweapon == weapAlts[oldweapon]) {
				fixSwitch = qtrue;
				switchtime = 0;
			}
			break;

		// Handguns
		case WP_LUGER:
		case WP_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 0;
			}
			break;

		// Silenced handguns
		case WP_SILENCER:
		case WP_SILENCED_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				altSwitchAnim = qtrue;
			}
			break;
		// Deployable MGs
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_MG42:
		case WP_BAR:
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];
				fixSwitch = qtrue;
				switchtime = 0;
				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
			break;

		// Deployed MGs
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42_SET:
		case WP_BAR_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				fixSwitch = qtrue;
				switchtime = 0;
			}
			break;

		// Mortar
		case WP_MORTAR:
		case WP_MORTAR2:
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];
				fixSwitch = qtrue;
				switchtime = 0;
				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
			break;

		case WP_MORTAR2_SET:
		case WP_MORTAR_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				fixSwitch = qtrue;
				switchtime = 0;
			}
            break;

		default:
			break;
	}

	// play an animation
	if( altSwitchAnim ) {
		if( pm->ps->eFlags & EF_PRONE ) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_UNDO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
		}
		else {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_UNDO_ALT_WEAPON_MODE, qfalse, qfalse );
		}
	}
	else {
		BG_AnimScriptEvent( pm->ps,pm->character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qfalse );
	}

	// core: The client-FPS < server-FPS fix.. aka. rifleglitch
	// (Alt-weapon switching messes up selection of the chosen weapon.. riflenade detach.attach.detach. aargh)
	if ( fixSwitch ) {
		switch(newweapon) {
			case WP_CARBINE:
			case WP_KAR98:
				pm->pmext->silencedSideArm &= ~2;
				PM_FinishWeaponChange();
				break;
			case WP_M7:
			case WP_GPG40:
				pm->pmext->silencedSideArm |= 2;
				PM_FinishWeaponChange();
				break;
			case WP_MOBILE_BROWNING:
			case WP_MOBILE_MG42:
			case WP_BAR:
			case WP_MOBILE_BROWNING_SET:
			case WP_MOBILE_MG42_SET:
			case WP_BAR_SET:
			case WP_MORTAR:
			case WP_MORTAR2:
			case WP_MORTAR2_SET:
			case WP_MORTAR_SET:
				PM_FinishWeaponChange();
				break;
		}
	}

	if( reload ) {
		pm->ps->weaponstate = WEAPON_DROPPING_TORELOAD;
	}
	else {
		pm->ps->weaponstate = WEAPON_DROPPING;
	}

	pm->ps->weaponTime += switchtime;
}

/*
===============
PM_FinishWeaponChange
===============
*/
static void PM_FinishWeaponChange( void )
{
	int		 oldweapon;
	int		 newweapon = pm->ps->nextWeapon;
	int		 switchtime;
	qboolean altSwitchAnim = qfalse;
	qboolean doSwitchAnim  = qtrue;

	// Cannot switch to an invalid weapon
	if ( newweapon < WP_NONE || newweapon >= WP_FOOTKICK ) {
		newweapon = WP_NONE;
	}

	// Cannot switch to a weapon you don't have
	if ( !( COM_BitCheck( pm->ps->weapons, newweapon ) ) ) {
		newweapon = WP_NONE;
	}

	oldweapon = pm->ps->weapon;

	pm->ps->weapon = newweapon;

	if( pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ) {
		pm->ps->weaponstate = WEAPON_RAISING_TORELOAD;
	}
	else {
		pm->ps->weaponstate = WEAPON_RAISING;
	}

	switch(newweapon) {
		// don't really care about anim since these weapons don't show in view.
		// However, need to set the animspreadscale so they are initally at worst accuracy
		case WP_K43_SCOPE:
		case WP_GARAND_SCOPE:
		case WP_FG42SCOPE:
			pm->ps->aimSpreadScale		= 255;		// initially at lowest accuracy
			pm->ps->aimSpreadScaleFloat = 255.0f;	// initially at lowest accuracy
			break;
		case WP_SILENCER:
			pm->pmext->silencedSideArm |= 1;
			break;
		case WP_LUGER:
			pm->pmext->silencedSideArm &= ~1;
			break;
		case WP_SILENCED_COLT:
			pm->pmext->silencedSideArm |= 1;
			break;
		case WP_COLT:
			pm->pmext->silencedSideArm &= ~1;
			break;
		case WP_CARBINE:
			pm->pmext->silencedSideArm &= ~2;
			break;
		case WP_M7:
			pm->pmext->silencedSideArm |= 2;
			break;
		case WP_KAR98:
			pm->pmext->silencedSideArm &= ~2;
			break;
		case WP_GPG40:
			pm->pmext->silencedSideArm |= 2;
			break;
		default:
			break;
	}

	// doesn't happen too often (player switched weapons away then back very quickly)
	if(oldweapon == newweapon &&
		newweapon != WP_KNIFE &&
		newweapon != WP_KNIFE_KABAR &&
		newweapon != WP_GRENADE_PINEAPPLE &&
		newweapon != WP_GRENADE_LAUNCHER &&
		newweapon != WP_SMOKE_BOMB )
		return;

	// dropping/raising usually takes 1/4 sec.
	// jaquboss check if we have loaded weapon animation cfg and use it to do correct time
	if(newweapon == weapAlts[oldweapon]) {
        switchtime =  BG_Weapons[newweapon].animationTime[PM_AltSwitchToForWeapon(newweapon)];
	}
	else {
        switchtime =  BG_Weapons[newweapon].animationTime[PM_RaiseAnimForWeapon(newweapon)];
	}

	// sometimes different switch times for alt weapons
	switch(newweapon) {
		case WP_LUGER:
		case WP_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime	  = 0;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_SILENCER:
		case WP_SILENCED_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_CARBINE:		// Allied Engineer's Garand
		case WP_KAR98:			// Axis Engineer Kar98
			if(newweapon == weapAlts[oldweapon]) {
				if( !pm->ps->ammoclip[ WeaponTable[oldweapon].ammoindex ] ) {
					switchtime	 = 0;
					doSwitchAnim = qfalse;
				}
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_M7:
		case WP_GPG40:
			if(newweapon == weapAlts[oldweapon]) {
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_FG42:
		case WP_FG42SCOPE:
			if(newweapon == weapAlts[oldweapon])
				switchtime	  = 50;	// fast
			break;
		case WP_MORTAR:
		case WP_MORTAR2:
			if( newweapon == weapAlts[oldweapon] ) {
				altSwitchAnim = qtrue;
			}
			break;
		case WP_MORTAR2_SET:
		case WP_MORTAR_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				altSwitchAnim = qtrue;
			}
			break;
	}


	pm->ps->weaponTime += switchtime;

	BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_WEAPON, newweapon, qtrue );

	// play an animation
	if( doSwitchAnim ) {
		if( altSwitchAnim ) {
			if( pm->ps->eFlags & EF_PRONE ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
			}
			else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DO_ALT_WEAPON_MODE, qfalse, qfalse );
			}
		}
		else {
			if( pm->ps->eFlags & EF_PRONE ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RAISEWEAPONPRONE, qfalse, qfalse );
			}
			else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RAISEWEAPON, qfalse, qfalse );
			}
		}

		// alt weapon switch was played when switching away, just go into idle
		if(weapAlts[oldweapon] == newweapon) {
			PM_StartWeaponAnim(PM_AltSwitchToForWeapon(newweapon));
		}
		else {
			PM_StartWeaponAnim(PM_RaiseAnimForWeapon(newweapon));
		}
	}
}

/*
==============
PM_ReloadClip
==============
*/
static void PM_ReloadClip( int weapon )
{
	int ammoreserve = pm->ps->ammo[ WeaponTable[weapon].ammoindex ];
	int ammoclip	= pm->ps->ammoclip[WeaponTable[weapon].clipindex];
	int ammomove = GetWeaponTableData(weapon)->maxclip - ammoclip;

	if(ammoreserve < ammomove) {
		ammomove = ammoreserve;
	}

	if(ammomove) {
		pm->ps->ammo[ WeaponTable[weapon].ammoindex] -= ammomove;
		pm->ps->ammoclip[WeaponTable[weapon].clipindex] += ammomove;
	}

	// reload akimbo stuff
	if( IS_AKIMBO_WEAPON( weapon ) ) {
		PM_ReloadClip( BG_AkimboSidearm( weapon ) );
	}
}

/*
==============
PM_FinishWeaponReload
==============
*/

static void PM_FinishWeaponReload(void) {
	PM_ReloadClip(pm->ps->weapon);			// move ammo into clip
	pm->ps->weaponstate = WEAPON_READY;		// ready to fire
	PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
}

/*
==============
PM_CheckforReload
==============
*/
void PM_CheckForReload( int weapon )
{
	qboolean	autoreload;
	qboolean	reloadRequested;
	int			clipWeap, ammoWeap;

	// GPG40 and M7 don't reload
	// core: as do many more..
	switch(weapon) {
		case WP_GPG40:
		case WP_M7:
		case WP_FLAMETHROWER:
		case WP_KNIFE:
		case WP_GRENADE_LAUNCHER:
		case WP_PANZERFAUST:
		case WP_GRENADE_PINEAPPLE:
		case WP_MEDIC_SYRINGE:
		case WP_AMMO:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_SMOKETRAIL:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_LANDMINE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_DUMMY_MG42:
		case WP_MEDIC_ADRENALINE:
		case WP_KNIFE_KABAR:
		case WP_BAZOOKA:
		case WP_POISON_SYRINGE:
		case WP_FOOTKICK:
		case WP_NONE:
			return;
		default:
			break;
	}

	switch(pm->ps->weaponstate) {
		case WEAPON_RAISING:
		case WEAPON_RAISING_TORELOAD:
		case WEAPON_DROPPING:
		case WEAPON_DROPPING_TORELOAD:
		case WEAPON_RELOADING:
			return;
		default:
			break;
	}

	// user is forcing a reload (manual reload)
	reloadRequested = (qboolean)(pm->cmd.wbuttons & WBUTTON_RELOAD);

    if (pm->cmd.wbuttons & WBUTTON_ATTACK2 && pm->pmext->weapAltReload && !IS_SCOPED_WEAPON(pm->ps->weapon)) {
		reloadRequested = qtrue;
    }

	autoreload	= pm->pmext->bAutoReload || !WeaponTable[weapon].isAutoReload;
	clipWeap	= WeaponTable[weapon].clipindex;
	ammoWeap	= WeaponTable[weapon].ammoindex;

	if( pm->ps->weaponTime <= 0 ) {
		qboolean doReload = qfalse;

	    switch( weapon ) {
		    case WP_FG42SCOPE:
		    case WP_GARAND_SCOPE:
		    case WP_K43_SCOPE:
				if( reloadRequested && pm->ps->ammo[ammoWeap] && pm->ps->ammoclip[clipWeap] < GetWeaponTableData(weapon)->maxclip) {
					// higher level coverts to reload without leaving scoped view
					if( pm->skillBits[SK_COVERTOPS] & (1<<6) ) {
						PM_BeginWeaponReload( weapon );
					}
					else {
						PM_BeginWeaponChange( weapon, weapAlts[weapon], !(pm->ps->ammo[ammoWeap]) ? qfalse : qtrue );
					}
				}
			    return;
		    default:
			    break;
	    }

		if( reloadRequested ) {
			if( pm->ps->ammo[ammoWeap] ) {
				if( pm->ps->ammoclip[clipWeap] < GetWeaponTableData(weapon)->maxclip ) {
					doReload = qtrue;
				}

				// akimbo should also check other weapon status
				if( IS_AKIMBO_WEAPON( weapon ) ) {
					if( pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(weapon)].clipindex] < GetWeaponTableData(WeaponTable[BG_AkimboSidearm(weapon)].clipindex)->maxclip ) {
						doReload = qtrue;
					}
				}
			}
		}
		else if( autoreload ) {
			if(	!pm->ps->ammoclip[clipWeap] && pm->ps->ammo[ammoWeap] ) {
				if( IS_AKIMBO_WEAPON( weapon ) ) {
					if( !pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(weapon)].clipindex] ) {
						doReload = qtrue;
					}
				}
				else {
					doReload = qtrue;
				}
			}
		}

		if( doReload ) {
			PM_BeginWeaponReload( weapon );
		}
	}
}

/*
==============
PM_SwitchIfEmpty
==============
*/
static void PM_SwitchIfEmpty(void)
{
	// weapon from here down will be a thrown explosive
	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_DYNAMITE:
		case WP_SMOKE_BOMB:
		case WP_LANDMINE:
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
		case WP_SATCHEL_DET:
			break;
		default:
			return;
	}

	if(pm->ps->ammoclip[WeaponTable[pm->ps->weapon].clipindex])	// still got ammo in clip
		return;

	if(pm->ps->ammo[ WeaponTable[pm->ps->weapon].ammoindex ])	// still got ammo in reserve
		return;

	// If this was the last one, remove the weapon and switch away before the player tries to fire next

	// NOTE: giving grenade ammo to a player will re-give him the weapon (if you do it through add_ammo())
	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			COM_BitClear( pm->ps->weapons, pm->ps->weapon);
	}

	PM_AddEvent( EV_NOAMMO );
}

/*
==============
PM_WeaponUseAmmo
	accounts for clips being used/not used
==============
*/
void PM_WeaponUseAmmo( int wp, int amount ) {
	int takeweapon;
	qboolean fillEmpty=qfalse;

	// jet Pilot - enable infinite ammo if specified
	if (PM_INSANITY & JP_INSANITY_AMMUNITION) {
		fillEmpty=qtrue;
	}

	takeweapon = WeaponTable[wp].clipindex;

	if( IS_AKIMBO_WEAPON( wp ) ) {
		if( !BG_AkimboFireSequence( wp, pm->ps->ammoclip[WeaponTable[wp].clipindex], pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(wp)].clipindex] ) )
			takeweapon = BG_AkimboSidearm(wp);
	}

	pm->ps->ammoclip[takeweapon] -= amount;

	if ( pm->ps->ammoclip[ takeweapon] == 0 && fillEmpty)
		pm->ps->ammoclip[ takeweapon ] = GetWeaponTableData(takeweapon)->defaultStartingClip;
}

/*
==============
PM_WeaponAmmoAvailable
	accounts for clips being used/not used
==============
*/
int PM_WeaponAmmoAvailable( int wp ) {
	int takeweapon = WeaponTable[wp].clipindex;

	if( IS_AKIMBO_WEAPON( wp ) ) {
		if( !BG_AkimboFireSequence( wp, pm->ps->ammoclip[WeaponTable[wp].clipindex], pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(wp)].clipindex] ) )
			takeweapon = BG_AkimboSidearm(wp);
	}

	return pm->ps->ammoclip[takeweapon];
}

/*
==============
PM_WeaponClipEmpty
	accounts for clips being used/not used
==============
*/
int PM_WeaponClipEmpty( int wp ) {
	if(!(pm->ps->ammoclip[WeaponTable[wp].clipindex]))
		return 1;

	return 0;
}

/*
==============
PM_CoolWeapons
==============
*/
void PM_CoolWeapons( void )
{
	int wp, maxHeat, i;

	for (i=0; i<sizeof(weapOverheat); ++i ) {
		wp = weapOverheat[i];
		// if you have the weapon
		if( COM_BitCheck( pm->ps->weapons, wp) ) {
			// and it's hot
			if(pm->ps->weapHeat[wp]) {
				// jet Pilot - Modified cooling based on heavy weapon skill
				if(( pm->skillBits[SK_HEAVY_WEAPONS] & (1<<2) && WeaponTable[wp].isHeavy) || pm->ps->stats[STAT_TEAMAURA] & AURABIT_ENGINEER ) {
					pm->ps->weapHeat[wp] -= ((float)GetWeaponTableData(wp)->coolRate * 1.5f * pml.frametime);
				}
				else {
					pm->ps->weapHeat[wp] -= ((float)GetWeaponTableData(wp)->coolRate * pml.frametime);
				}

				if(pm->ps->weapHeat[wp] < 0) {
					pm->ps->weapHeat[wp] = 0;
				}
			}
		}
	}

	// a weapon is currently selected, convert current heat value to 0-255 range for client transmission
	if(pm->ps->weapon) {
		if( pm->ps->persistant[PERS_HWEAPON_USE] || pm->ps->eFlags & EF_MOUNTEDTANK ) {
			// rain - floor to prevent 8-bit wrap
			pm->ps->curWeapHeat = floor( ( ( (float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) ) * 255.0f);
		}
		else {
			// rain - #172 - don't divide by 0
			maxHeat = GetWeaponTableData(pm->ps->weapon)->maxHeat;

			// rain - floor to prevent 8-bit wrap
			if (maxHeat != 0) {
				pm->ps->curWeapHeat = floor( ( ( (float)pm->ps->weapHeat[pm->ps->weapon] / (float)maxHeat) ) * 255.0f);
			}
			else {
				pm->ps->curWeapHeat = 0;
			}
		}
	}
}

/*
==============
PM_AdjustAimSpreadScale
==============
*/
#define	AIMSPREAD_DECREASE_RATE		200.0f		// (SA) when I made the increase/decrease floats (so slower weapon recover could happen for scoped weaps) the average rate increased significantly
#define	AIMSPREAD_INCREASE_RATE		800.0f
#define	AIMSPREAD_VIEWRATE_MIN		30.0f		// degrees per second
#define	AIMSPREAD_VIEWRATE_RANGE	120.0f		// degrees per second

void PM_AdjustAimSpreadScale( void )
{
	float	increase, decrease;		// (SA) was losing lots of precision on slower weapons (scoped)
	float	cmdTime, wpnScale;

	// all weapons are very inaccurate in zoomed mode
	if(pm->ps->eFlags & EF_ZOOMING) {
		pm->ps->aimSpreadScale		= 255;
		pm->ps->aimSpreadScaleFloat = 255;
		return;
	}

	cmdTime = (float)(pm->cmd.serverTime - pm->oldcmd.serverTime) / 1000.0;
	wpnScale = BG_Weapons[pm->ps->weapon].wpnScale;

	if ( wpnScale ) {
		float	viewchange, speed, anglechange, f;

		// JPW NERVE crouched players recover faster (mostly useful for snipers)
		if( pm->ps->eFlags & EF_CROUCHING || pm->ps->eFlags & EF_PRONE ) {
			wpnScale *= 0.5;
		}

        if( IS_SCOPED_WEAPON( pm->ps->weapon ) && pm->skillBits[SK_COVERTOPS] & (1<<3) ) {
            wpnScale *= 0.5;
        }

		decrease = (cmdTime * AIMSPREAD_DECREASE_RATE) / wpnScale;

		viewchange = speed = anglechange = 0.f;

		if ( BG_Weapons[pm->ps->weapon].velocity2spread ) {
			// take player movement into account (even if only for the scoped weapons)
			// TODO: also check for jump/crouch and adjust accordingly
			speed += fabs(pm->ps->velocity[0]);
			speed += fabs(pm->ps->velocity[1]);

			viewchange += speed;
		}

		// core: viewchange2spread can be removed by a g_realism bitflag..
		if ( !(PM_REALISM & REALISM_NO_VIEWCHANGE2SPREAD) ) {
			if ( BG_Weapons[pm->ps->weapon].viewchange2spread ) {
				// take player view rotation into account
				anglechange += fabs( AngleSubtract(  SHORT2ANGLE(AngleNormalizeInt(pm->cmd.angles[0])),
									SHORT2ANGLE(AngleNormalizeInt(pm->oldcmd.angles[0]))));
				anglechange += fabs( AngleSubtract(  SHORT2ANGLE(AngleNormalizeInt(pm->cmd.angles[1])),
									SHORT2ANGLE(AngleNormalizeInt(pm->oldcmd.angles[1]))));

				viewchange += anglechange;
			}
		}

		viewchange = (float)viewchange / cmdTime;	// convert into this movement for a second
		viewchange -= AIMSPREAD_VIEWRATE_MIN / wpnScale;
		f = (float)(AIMSPREAD_VIEWRATE_RANGE / wpnScale);
		if (viewchange <= 0) {
			viewchange = 0;
		}
		else if( viewchange > f ) {
			viewchange = f;
		}

		// now give us a scale from 0.0 to 1.0 to apply the spread increase
		viewchange = viewchange / f;

		increase = /*(int)*/(cmdTime * viewchange * AIMSPREAD_INCREASE_RATE);
	}
	else {
		increase = 0;
		decrease = AIMSPREAD_DECREASE_RATE;
	}

	// update the aimSpreadScale
	pm->ps->aimSpreadScaleFloat += (increase - decrease);

	if (pm->ps->aimSpreadScaleFloat < 0) {
		pm->ps->aimSpreadScaleFloat = 0;
	}
	else if (pm->ps->aimSpreadScaleFloat > 255) {
		pm->ps->aimSpreadScaleFloat = 255;
	}

	pm->ps->aimSpreadScale = (int)pm->ps->aimSpreadScaleFloat;	// update the int for the client
}

#define weaponstateFiring (pm->ps->weaponstate == WEAPON_FIRING || pm->ps->weaponstate == WEAPON_FIRINGALT)

/*
==============
PM_Weapon

Generates weapon events and modifes the weapon counter
==============
*/

static qboolean PM_MountedFire( void )
{
	switch(pm->ps->persistant[PERS_HWEAPON_USE]) {
		case 1:
			if( pm->ps->weapHeat[WP_DUMMY_MG42] ) {
				pm->ps->weapHeat[WP_DUMMY_MG42] -= (300.f * pml.frametime);

				if( pm->ps->weapHeat[WP_DUMMY_MG42] < 0 )
					pm->ps->weapHeat[WP_DUMMY_MG42] = 0;

				// rain - floor() to prevent 8-bit wrap
				pm->ps->curWeapHeat = floor((((float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) ) * 255.0f);
			}

			if( pm->ps->weaponTime > 0 ) {
				pm->ps->weaponTime -= pml.msec;
				if (pm->ps->weaponTime <= 0) {
					if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
						pm->ps->weaponTime = 0;
						return qtrue;
					}
				} else {
					return qtrue;
				}
			}

			if( pm->cmd.buttons & BUTTON_ATTACK ) {
				pm->ps->weapHeat[WP_DUMMY_MG42] += MG42_RATE_OF_FIRE_MP;
				PM_AddEvent( EV_FIRE_WEAPON_MG42 );
				pm->ps->weaponTime += MG42_RATE_OF_FIRE_MP;

				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				pm->ps->viewlocked = VIEWLOCK_JITTER;		// this enable screen jitter when firing

				if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
					pm->ps->weapHeat[WP_DUMMY_MG42] = MAX_MG42_HEAT;	// cap heat to max
					PM_AddEvent( EV_WEAP_OVERHEAT );
					pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
				}
			}
			return qtrue;
	}

	if( pm->ps->eFlags & EF_MOUNTEDTANK ) {
		if( pm->ps->weapHeat[WP_DUMMY_MG42] ) {
			pm->ps->weapHeat[WP_DUMMY_MG42] -= (300.f * pml.frametime);

			if( pm->ps->weapHeat[WP_DUMMY_MG42] < 0 )
				pm->ps->weapHeat[WP_DUMMY_MG42] = 0;

			// rain - floor() to prevent 8-bit wrap
			pm->ps->curWeapHeat = floor(((float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) * 255.0f);
		}

		if( pm->ps->weaponTime > 0 ) {
			pm->ps->weaponTime -= pml.msec;
			if (pm->ps->weaponTime <= 0) {
				if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
					pm->ps->weaponTime = 0;
					return qtrue;
				}
			}
			else {
				return qtrue;
			}
		}

		if( pm->cmd.buttons & BUTTON_ATTACK ) {
			pm->ps->weapHeat[WP_DUMMY_MG42] += MG42_RATE_OF_FIRE_MP;

			PM_AddEvent( EV_FIRE_WEAPON_MOUNTEDMG42 );

			pm->ps->weaponTime += MG42_RATE_OF_FIRE_MP;

			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
			//pm->ps->viewlocked = VIEWLOCK_JITTER;		// this enable screen jitter when firing

			if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
				pm->ps->weapHeat[WP_DUMMY_MG42] = MAX_MG42_HEAT;	// cap heat to max
				PM_AddEvent( EV_WEAP_OVERHEAT );
				pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
			}

		}
		return qtrue;
	}

	return qfalse;
}

qboolean PM_AltFire ( void )
{
	if ( pm->cmd.wbuttons & WBUTTON_ATTACK2 ) {
		if ( pm->ps->weapon == WP_KNIFE ||  pm->ps->weapon == WP_KNIFE_KABAR ) {
			if ( pm->skillBits[SK_LIGHT_WEAPONS] & (1<<8) )
				return qtrue;
		}
	}
	return qfalse;
}

// throwing knife
qboolean PM_AltFiring ( qboolean delayedFire )
{
	if ( pm->ps->weaponstate == WEAPON_FIRINGALT ) {
		if ( pm->ps->weaponDelay > 0 || delayedFire  ) {
			if ( pm->ps->weapon == WP_KNIFE ||  pm->ps->weapon == WP_KNIFE_KABAR )
				return qtrue;
		}
	}

	return qfalse;
}

static void PM_HandleRecoil ( void ) {
	vec3_t muzzlebounce;
	int i, deltaTime;

	if( !pm->pmext->weapRecoilTime ) {
		return;
	}

	deltaTime = pm->cmd.serverTime - pm->pmext->weapRecoilTime;
	VectorCopy( pm->ps->viewangles, muzzlebounce );

	if( deltaTime > pm->pmext->weapRecoilDuration )
		deltaTime = pm->pmext->weapRecoilDuration;

	for( i = pm->pmext->lastRecoilDeltaTime; i < deltaTime; i += 15 ) {
		if( pm->pmext->weapRecoilPitch > 0.f ) {
			muzzlebounce[PITCH] -= 2*pm->pmext->weapRecoilPitch*cos(2.5*(i)/pm->pmext->weapRecoilDuration);
			muzzlebounce[PITCH] -= 0.25*fabs(cos(pm->cmd.serverTime))*(1.0f-(i)/pm->pmext->weapRecoilDuration); // cosine > 0
		}

		if( pm->pmext->weapRecoilYaw > 0.f ) {
			muzzlebounce[YAW] += 0.5*pm->pmext->weapRecoilYaw*cos(1.0-(i)*3/pm->pmext->weapRecoilDuration);
			muzzlebounce[YAW] += 0.5*cos(pm->cmd.serverTime)*(1.0f-(i)/pm->pmext->weapRecoilDuration); // jaquboss cosine
		}
	}

	// set the delta angle
	pm->ps->delta_angles[0] = ANGLE2SHORT(muzzlebounce[0]) - pm->cmd.angles[0];
	pm->ps->delta_angles[1] = ANGLE2SHORT(muzzlebounce[1]) - pm->cmd.angles[1];
	pm->ps->delta_angles[2] = ANGLE2SHORT(muzzlebounce[2]) - pm->cmd.angles[2];

	VectorCopy( muzzlebounce, pm->ps->viewangles );

	if( deltaTime == pm->pmext->weapRecoilDuration ) {
		pm->pmext->weapRecoilTime = 0;
		pm->pmext->lastRecoilDeltaTime = 0;
	}
	else {
		pm->pmext->lastRecoilDeltaTime = deltaTime;
	}

}

static qboolean PM_CheckGrenade() {
	qboolean forcethrow = qfalse;

	if(pm->ps->weapon != WP_GRENADE_LAUNCHER &&
		pm->ps->weapon != WP_GRENADE_PINEAPPLE &&
		pm->ps->weapon != WP_DYNAMITE &&
		pm->ps->weapon != WP_SMOKE_BOMB &&
		pm->ps->weapon != WP_KNIFE &&
		pm->ps->weapon != WP_KNIFE_KABAR ) {
			return qfalse;
		}

	if( pm->ps->grenadeTimeLeft > 0 ) { // do we have time?

		if( pm->ps->weapon == WP_KNIFE ||  pm->ps->weapon == WP_KNIFE_KABAR ) { // dynamite never run of time
			pm->ps->grenadeTimeLeft += pml.msec;

			if (pm->ps->grenadeTimeLeft > KNIFECHARGETIME)
				pm->ps->grenadeTimeLeft = KNIFECHARGETIME;

		}
		else if( pm->ps->weapon == WP_DYNAMITE ) {
			pm->ps->grenadeTimeLeft += pml.msec;

			if (pm->ps->grenadeTimeLeft < 5000)
				pm->ps->grenadeTimeLeft = 5000;
		}
		else { // but nades do
			pm->ps->grenadeTimeLeft -= pml.msec;

			if( pm->ps->grenadeTimeLeft <= 100 ) { // give two frames advance notice so there's time to launch and detonate
				forcethrow = qtrue;
				pm->ps->grenadeTimeLeft = 100;
			}
		} // time handling

		// jaquboss don't allow to catch nade again
		if ( pm->ps->holdable[1] ){
			pm->cmd.buttons &= ~BUTTON_ATTACK;
			pm->cmd.wbuttons &= ~WBUTTON_ATTACK2;
		}

		if ( (( pm->ps->weapon == WP_KNIFE || pm->ps->weapon == WP_KNIFE_KABAR ) && !(pm->cmd.wbuttons & WBUTTON_ATTACK2 )) || pm->ps->eFlags & EF_PRONE_MOVING ) {
			if( pm->ps->weaponDelay == GetWeaponTableData(pm->ps->weapon)->fireDelayTime ) { // forced to throw, or we just released button and returned here ( after setting all stuff )
				// released fire button.  Fire!!!

				PM_StartWeaponAnim(WEAP_ATTACK_LASTSHOT); // jaquboss - play first person anim as well

				if( pm->ps->eFlags & EF_PRONE )
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
				else
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2, qfalse, qtrue );

				pm->ps->holdable[1] = 1; // released knife...
			}
		}
		else if( (pm->ps->weapon != WP_KNIFE && pm->ps->weapon != WP_KNIFE_KABAR && !(pm->cmd.buttons & BUTTON_ATTACK)) || forcethrow || pm->ps->eFlags & EF_PRONE_MOVING ) { // released button, or forced to throw
			if( pm->ps->weaponDelay == GetWeaponTableData(pm->ps->weapon)->fireDelayTime || forcethrow) { // forced to throw, or we just released button and returned here ( after setting all stuff )
				// released fire button.  Fire!!!

				if (pm->ps->weapon != WP_DYNAMITE)
					PM_StartWeaponAnim(WEAP_ATTACK2); // jaquboss - play first person anim as well

				if( pm->ps->eFlags & EF_PRONE )
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				else
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );

				 pm->ps->holdable[1] = 1; // released nade...
			}
		}
		else {
			// not for dynamite
			if ( pm->ps->weapon != WP_DYNAMITE )
				BG_SetConditionBitFlag( pm->ps->clientNum, ANIM_COND_HOLDING, ANIM_BITFLAG_HOLDING );

			return qtrue;
		}
	}
	return qfalse;
}

static int getAltWeapon( int weapnum ) {
	if(weapAlts[weapnum]) {
		return weapAlts[weapnum];
	}
	return weapnum;
}

static qboolean PM_WeaponHasAmmo( int i )
{
	// ydnar: certain weapons don't have ammo
	if( i == WP_PLIERS )
		return qtrue;

	if (!(pm->ps->ammo[WeaponTable[i].ammoindex]) &&
		!(pm->ps->ammoclip[WeaponTable[i].clipindex]) ) {
		return qfalse;
	}

	return qtrue;
}

static qboolean PM_WeaponSelectable( int i ) {

	// allow the player to unselect all weapons
	if(i == WP_NONE) {
		return qtrue;
	}

	if( BG_PlayerMounted( pm->ps->eFlags ) ) {
		return qfalse;
	}

	// check for weapon
	if (!(COM_BitCheck( pm->ps->weapons, i ) ) ) {
		return qfalse;
	}

	if(!PM_WeaponHasAmmo(i)) {
		return qfalse;
	}

	return qtrue;
}

static void PM_AltWeapon( void )
{
	int original;
	int num;

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;
	}

	// ignore if spectator
	if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 || pm->ps->eFlags & EF_PLAYDEAD ) {
		return;
	}

	if ( BG_PlayerMounted(pm->ps->eFlags))
		return;

	if ( !((pm->cmd.wbuttons & WBUTTON_ATTACK2) && !(pm->oldcmd.wbuttons & WBUTTON_ATTACK2)) )
		return;

#ifdef GAMEDLL
	// some alt vsays
	switch (pm->ps->weapon) {
		case WP_DYNAMITE:
			pm->voice(pm->ps->clientNum, "FTExploreArea");
			return;
		case WP_SMOKE_BOMB:
			switch( (rand()%2) ) {
				case 0:
					pm->voice(pm->ps->clientNum, "FTGoUndercover");
					break;
				case 1:
					pm->voice(pm->ps->clientNum, "FTInfiltrate");
					break;
			}
			return;
		case WP_SMOKE_MARKER:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
			pm->voice(pm->ps->clientNum, "FireInTheHole");
			return;
		case WP_PLIERS:
			switch( (rand()%3) ) {
				case 0:
					pm->voice(pm->ps->clientNum, "CoverMe");
					break;
				case 1:
					pm->voice(pm->ps->clientNum, "NeedBackup");
					break;
				case 2:
					pm->voice(pm->ps->clientNum, "ClearPath");
					break;
			}
			return;
		case WP_SATCHEL:
			pm->voice(pm->ps->clientNum, "LetsGo");
			return;
		case WP_MEDKIT:
		case WP_MEDIC_SYRINGE:
			pm->voice(pm->ps->clientNum, "IamMedic");
			return;
		case WP_AMMO:
			pm->voice(pm->ps->clientNum, "IamFieldOps");
			return;
		case WP_POISON_SYRINGE:
			pm->voice(pm->ps->clientNum, "EnemyWeak");
			return;
		default:
			break;
	}
#endif

	// don't allow switching while rasing or lowering a weapon
	if( pm->ps->weaponstate == WEAPON_DROPPING ||
		pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ||
		pm->ps->weaponstate == WEAPON_RAISING ||	// core
		pm->ps->weaponstate == WEAPON_HOLSTER_IN || // jaquboss
		pm->ps->weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	switch(pm->ps->weapon) {
		case WP_MORTAR:
		case WP_MORTAR2:
			// Need ground for this
			{
				int contents;
				vec3_t point;

				if( !pml.groundPlane ) {
					return;
				}

				if( !pm->ps->ammoclip[WP_MORTAR] ) {
					return;
				}

				if( pm->ps->eFlags & EF_PRONE ) {
					return;
				}

				if( pm->waterlevel == 3 ) {
					return;
				}

				// ydnar: don't allow set if moving
				if( VectorLengthSquared( pm->ps->velocity ) ) {
					return;
				}

				// eurgh, need it here too else we play sounds :/
				point[0] = pm->ps->origin[0];
				point[1] = pm->ps->origin[1];
				point[2] = pm->ps->origin[2] + pm->ps->crouchViewHeight;
				contents = pm->pointcontents( point, pm->ps->clientNum );

				if ( contents & MASK_WATER ) {
					return;
				}
			}
			break;

		case WP_MOBILE_MG42:
		case WP_MOBILE_BROWNING:
		case WP_BAR:
			// IS_BIPOD_WEAPON
			if( !(pm->ps->eFlags & EF_PRONE) ) {
				return;
			}
			break;

		default:
			break;
	}

	original = pm->ps->weapon;

	num = getAltWeapon(original);

#ifdef CGAMEDLL
	if( original == WP_BINOCULARS ) {
		if(pm->ps->eFlags & EF_ZOOMING) {
			trap_SendConsoleCommand( "-zoom\n" );
			cg.binocZoomTime = -cg.time;
		}
		else if( !cg.binocZoomTime ) {
			cg.binocZoomTime = cg.time;
		}
	}
#endif

	if( original != num && PM_WeaponSelectable( num ) ) {
		// new weapon is valid
#ifdef GAMEDLL
		PM_AddEventExt( EV_WEAPALT, num );
#endif
		PM_BeginWeaponChange( original, num, qfalse );
	}
}

static void PM_Weapon( void ) {
	int			addTime = 0; // TTimo: init
	int			ammoNeeded;
	qboolean	delayedFire;	//----(SA)  true if the delay time has just expired and this is the frame to send the fire event
	int			aimSpreadScaleAdd;
	int			weapattackanim;
	qboolean	akimboFire;
	qboolean	isAkimbo;

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;
	}

	// ignore if spectator
	if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 || pm->ps->eFlags & EF_PLAYDEAD ) {
		if(!(pm->ps->pm_flags & PMF_LIMBO)) {
			PM_CoolWeapons();
		}
		return;
	}

	// weapon cool down
	PM_CoolWeapons();

	// special mounted mg42 handling
	if ( PM_MountedFire() ) {
		return;
	}

	pm->watertype = 0;

	isAkimbo = IS_AKIMBO_WEAPON( pm->ps->weapon );

	if( isAkimbo ) {
		akimboFire = BG_AkimboFireSequence( pm->ps->weapon, pm->ps->ammoclip[WeaponTable[pm->ps->weapon].clipindex], pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(pm->ps->weapon)].clipindex] );

	}
	else {
		akimboFire = qfalse;
	}

	// check for weapon recoil
	// do the recoil before setting the values, that way it will be shown next frame and not this
	PM_HandleRecoil();

	delayedFire = qfalse;

	BG_ClearConditionBitFlag( pm->ps->clientNum, ANIM_COND_HOLDING, ANIM_BITFLAG_HOLDING );

	if ( PM_CheckGrenade() )
		return;

	if( pm->ps->weaponDelay > 0 ) {
		pm->ps->weaponDelay -= pml.msec;

		if ( pm->ps->weaponDelay <= 0 ) {
			pm->ps->weaponDelay = 0;
			delayedFire = qtrue;		// weapon delay has expired.  Fire this frame

			// double check the player is still holding the fire button down for these weapons
			// so you don't get a delayed "non-fire" (fire hit and released, then shot fires)
			switch(pm->ps->weapon) {
				case WP_VENOM:
					if(pm->ps->weaponstate == WEAPON_FIRING)
						delayedFire = qfalse;
				default:
					break;
			}
		}
	}

	if( pm->ps->eFlags & EF_PRONE_MOVING && !delayedFire )
		return;

	// make weapon function
	if ( pm->ps->weaponTime > 0 ) {
		pm->ps->weaponTime -= pml.msec;
		if( pm->ps->weaponTime < 0 && !(pm->cmd.buttons & BUTTON_ATTACK) && !PM_AltFire() )
			pm->ps->weaponTime = 0;

		// Gordon: aha, THIS is the kewl quick fire mode :)
		// JPW NERVE -- added back for multiplayer pistol balancing
		switch(pm->ps->weapon) {
			case WP_LUGER:
			case WP_COLT:
			case WP_SILENCER:
			case WP_SILENCED_COLT:
			case WP_KAR98:
			case WP_K43:
			case WP_CARBINE:
			case WP_GARAND:
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_SILENCEDCOLT:
			case WP_AKIMBO_SILENCEDLUGER:
				// rain - moved releasedFire into pmext instead of ps
				if ( pm->pmext->releasedFire ) {
					if ( pm->cmd.buttons & BUTTON_ATTACK ) {
						// rain - akimbo weapons only have a 200ms delay, so
						// use a shorter time for quickfire (#255)
						if ( isAkimbo ) {
							if (pm->ps->weaponTime <= 50)
								pm->ps->weaponTime = 0;
						}
						else {
							if (pm->ps->weaponTime <= 150)
								pm->ps->weaponTime = 0;
						}
					}
				}
				else if (!(pm->cmd.buttons & BUTTON_ATTACK)) {
					// rain - moved releasedFire into pmext instead of ps
					pm->pmext->releasedFire = qtrue;
				}
				break;

			default:
				break;
		}
	}


	// check for weapon change
	// can't change if weapon is firing, but can change
	// again if lowering or raising
	if( (pm->ps->weaponTime <= 0 || ( !weaponstateFiring && pm->ps->weaponDelay <= 0 )) && !delayedFire) {
		if ( pm->ps->weapon != pm->cmd.weapon ) {
			PM_BeginWeaponChange( pm->ps->weapon, pm->cmd.weapon, qfalse );	//----(SA)	modified
		}
	}

    // Weapon is waiting for attack animation to complete (eg. Mortar)
	if( pm->ps->weaponDelay > 0 )
		return;


	// check for clip change:
	PM_CheckForReload( pm->ps->weapon );


    // Check for Shotgun reload interrupt
	if ( pm->ps->weapon == WP_SHOTGUN ) {
		if (pm->ps->weaponstate == WEAPON_RELOADING &&
			(pm->ps->holdable[0] == SHOTGUN_RELOADING_LOOP ||
			pm->ps->holdable[0] == SHOTGUN_RELOADING_AFTER_1ST ||
			pm->ps->holdable[0] == SHOTGUN_RELOADING_BEGIN_1ST ) &&
			pm->ps->weaponTime > 0 && pm->cmd.buttons & BUTTON_ATTACK ){
			pm->ps->holdable[0] = SHOTGUN_RELOADING_INTERRUPT;
			return;
		}
	}

    // Check if weapon is busy, if weapon is free unset artyFire
    // so binoculars can be disabled when weapon is busy again
	if( pm->ps->weaponTime > 0 || pm->ps->weaponDelay > 0 )
		return;
	else
		pm->pmext->artyFire = qfalse;

	// jaquboss - shotgun reloading
	// after putting weapon to reload position
	if (pm->ps->weaponstate == WEAPON_RELOADING) {
		if (pm->ps->weapon == WP_SHOTGUN) {
			if (pm->ps->holdable[0] == SHOTGUN_RELOADING_BEGIN_1ST ||
				( pm->ps->holdable[0] == SHOTGUN_RELOADING_INTERRUPT && pm->ps->ammoclip[WeaponTable[WP_SHOTGUN].clipindex] == 0 ))
			{
				pm->ps->ammoclip[WeaponTable[WP_SHOTGUN].clipindex] += 1;
				pm->ps->ammo[WeaponTable[WP_SHOTGUN].ammoindex] -= 1;

				if ( pm->ps->holdable[0] == SHOTGUN_RELOADING_INTERRUPT || pm->ps->ammo[WeaponTable[WP_SHOTGUN].ammoindex] == 0)
					PM_Shotgun_StopFromPump();
				else
					PM_Shotgun_PumpToLoop();
				return;
			}

			if( pm->ps->holdable[0] == SHOTGUN_RELOADING_BEGIN || pm->ps->holdable[0] == SHOTGUN_RELOADING_AFTER_1ST ) {
				PM_ContinueShotgunReload();
				return;
			}

			// loading one shell
			if( pm->ps->holdable[0] == SHOTGUN_RELOADING_LOOP || pm->ps->holdable[0] == SHOTGUN_RELOADING_INTERRUPT ) {
				int ammo;

				ammo = pm->ps->ammoclip[WeaponTable[WP_SHOTGUN].clipindex];
				ammo += 1;

				if ( ammo > GetWeaponTableData(WP_SHOTGUN)->maxclip || pm->ps->ammo[WeaponTable[WP_SHOTGUN].ammoindex] == 0 ) {
					PM_StopShotgunReload();
					return;
				}
				else {
					pm->ps->ammoclip[WeaponTable[WP_SHOTGUN].clipindex] += 1;// add one shell into shotgun
					pm->ps->ammo[WeaponTable[WP_SHOTGUN].ammoindex] -= 1;// grab one from ammo

					if ( pm->ps->holdable[0] == SHOTGUN_RELOADING_INTERRUPT ||  ammo == GetWeaponTableData(WP_SHOTGUN)->maxclip || pm->ps->ammo[WeaponTable[WP_SHOTGUN].ammoindex] == 0 )
						PM_StopShotgunReload();
					else
						PM_ContinueShotgunReload();

					return;
				}
			}

			// after pumping and returning weapon to home
			if( pm->ps->holdable[0] == SHOTGUN_RELOADING_STOP ) {
				pm->ps->weaponstate = SHOTGUN_NOT_RELOADING;
				pm->ps->weaponstate = WEAPON_READY;		// ready to fire
				PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
			}
			// end
		}
		else {
			PM_FinishWeaponReload();
		}
	}

	// change weapon if time
	switch (pm->ps->weaponstate) {
		case WEAPON_DROPPING:
		case WEAPON_DROPPING_TORELOAD:
			PM_FinishWeaponChange();
			return;

		case WEAPON_RAISING:
			pm->ps->weaponstate = WEAPON_READY;
			PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
			return;

		case WEAPON_RAISING_TORELOAD:
			pm->ps->weaponstate = WEAPON_READY;
			PM_BeginWeaponReload( pm->ps->weapon );
			return;

		default:
			break;
	}

	// unable to use weapon
	if ( !delayedFire ) {
		if ( pm->ps->pm_flags & PMF_MEDICUSE || ( pm->cmd.serverTime < pm->pmext->kickTime ) || ( pm->ps->pm_flags & PMF_LADDER && (PM_REALISM & REALISM_NOLADDERGUN) ) ){
			if ( pm->ps->weaponstate != WEAPON_HOLSTER_IN ) {
				pm->ps->weaponstate = WEAPON_HOLSTER_IN;
				PM_StartWeaponAnim(PM_DropAnimForWeapon(pm->ps->weapon));
				pm->ps->weaponTime = BG_Weapons[pm->ps->weapon].animationTime[PM_DropAnimForWeapon(pm->ps->weapon)];
			}
			else {
				pm->ps->weaponTime += 50;
			}
			return;
		}
		else if (pm->ps->weaponstate == WEAPON_HOLSTER_IN ){
			pm->ps->weaponstate = WEAPON_HOLSTER_OUT;
			PM_StartWeaponAnim(PM_RaiseAnimForWeapon(pm->ps->weapon));
            pm->ps->weaponTime = BG_Weapons[pm->ps->weapon].animationTime[PM_RaiseAnimForWeapon(pm->ps->weapon)];
			return;
		}
		else if (pm->ps->weaponstate == WEAPON_HOLSTER_OUT ) {
			pm->ps->weaponstate = WEAPON_READY;
			PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
			return;
		}
	}

	if(pm->ps->weapon == WP_NONE) {	// this is possible since the player starts with nothing
		return;
	}

	if ( !BG_CheckCharge(pm) ){
		if( pm->ps->weapon == WP_AMMO ||  pm->ps->weapon == WP_MEDKIT ) {
			if( pm->cmd.buttons & BUTTON_ATTACK )
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );
		}
		return;
	}

	switch (pm->ps->weapon) {
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
			if( pm->ps->eFlags & EF_PRONE ) {
				return;
			}
			break;

		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			if( !delayedFire ) {
				pm->ps->weaponstate = WEAPON_READY;
			}
			break;

		default:
			break;
	}


	// check for fire
	// if not on fire button and there's not a delayed shot this frame...
	// consider also leaning, with delayed attack reset
	if ( (!(pm->cmd.buttons & BUTTON_ATTACK) && !PM_AltFire() && !delayedFire) ||
		(pm->ps->leanf != 0 && !PM_AltFiring(delayedFire) && pm->ps->weapon != WP_GRENADE_LAUNCHER && pm->ps->weapon != WP_GRENADE_PINEAPPLE && pm->ps->weapon != WP_SMOKE_BOMB) )
	{
		pm->ps->weaponTime	= 0;
		pm->ps->weaponDelay	= 0;

		if(weaponstateFiring) {	// you were just firing, time to relax
			PM_ContinueWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
		}

		pm->ps->weaponstate = WEAPON_READY;
		return;
	}

	// a not mounted mortar can't fire
	if( pm->ps->weapon == WP_MORTAR || pm->ps->weapon == WP_MORTAR2 ) {
		return;
	}

	// player is zooming - no fire
	// JPW NERVE in MP, LT needs to zoom to call artillery
	if(pm->ps->eFlags & EF_ZOOMING) {
		if(pm->ps->stats[STAT_PLAYER_CLASS] == PC_FIELDOPS) {
			pm->ps->weaponTime += 500;
			pm->pmext->artyFire = qtrue; // used binocs
            PM_AddEventExt( EV_FIRE_WEAPON, WP_BINOCULARS );
		}
		return;
	}

	// player is underwater - no fire
	if(pm->waterlevel == 3) {
		switch(pm->ps->weapon) {
			case WP_KNIFE:
			case WP_KNIFE_KABAR:
			case WP_GRENADE_LAUNCHER:
			case WP_GRENADE_PINEAPPLE:
			case WP_DYNAMITE:
			case WP_LANDMINE:
			case WP_SMOKE_BOMB:
			case WP_MEDIC_SYRINGE:
			case WP_POISON_SYRINGE:
			case WP_PLIERS:
			case WP_SATCHEL:
				break;

			default:
				PM_AddEvent(EV_NOFIRE_UNDERWATER);
				pm->ps->weaponTime	= 500;
				// avoid insta-fire after water exit on delayed weapon attacks
				pm->ps->weaponDelay	= 0;
				return;
		}
	}

	// start the animation even if out of ammo
	switch(pm->ps->weapon) {
		default:
			if(!weaponstateFiring) {
				// delay so the weapon can get up into position before firing (and showing the flash)
				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				}
				else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;

		// machineguns should continue the anim, rather than start each fire
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_STEN_MKII:
		case WP_STG44:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_FG42:
		case WP_FG42SCOPE:
		case WP_MOBILE_BROWNING:
		case WP_BAR:
		case WP_MOBILE_MG42:
		case WP_VENOM:
		case WP_BAR_SET:
		case WP_MOBILE_MG42_SET:
		case WP_MOBILE_BROWNING_SET:
			if(!weaponstateFiring) {
				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qtrue, qtrue );
				}
				// jet Pilot - FIXME: these weapons should never be not prone
				else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qtrue, qtrue );
				}
			}
			break;

		case WP_SHOTGUN:
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		case WP_LUGER:
		case WP_COLT:
		case WP_GARAND:
		case WP_K43:
		case WP_KAR98:
		case WP_CARBINE:
		case WP_GPG40:
		case WP_M7:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_MEDKIT:
		case WP_AMMO:
			if(!weaponstateFiring) {
				if ( pm->ps->weapon == WP_PANZERFAUST || pm->ps->weapon == WP_BAZOOKA ) {
					PM_AddEvent( EV_SPINUP );
				}
				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			else {
				if( pm->ps->eFlags & EF_PRONE ) {
					if( akimboFire ) {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
					}
					else {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
					}
				}
				else {
					if( akimboFire )
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2, qfalse, qtrue );
					else
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;

		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			if(!weaponstateFiring) {
				PM_AddEvent( EV_SPINUP );
				PM_StartWeaponAnim(PM_AttackAnimForWeapon(pm->ps->weapon));
                pm->ps->weaponDelay = BG_Weapons[pm->ps->weapon].animationTime[PM_AttackAnimForWeapon(pm->ps->weapon)];
			}
			else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				}
				else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;

		// melee
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			if(!delayedFire) {
				// throw
				if ( pm->cmd.wbuttons & WBUTTON_ATTACK2 && pm->skillBits[SK_LIGHT_WEAPONS] & (1<<8) && PM_WeaponAmmoAvailable(pm->ps->weapon) ) {
					if( pm->ps->eFlags & EF_PRONE )
						 BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON3PRONE, qfalse, qtrue );
					else
						 BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON3, qfalse, qtrue );

					pm->ps->grenadeTimeLeft = 50;
					pm->ps->holdable[1] = 0;
					PM_StartWeaponAnim(WEAP_ATTACK2);

					pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
				}
				else {  // stab
					if( pm->ps->eFlags & EF_PRONE )
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qfalse );
					else
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qfalse );
				}
			}
			break;

		// throw
		case WP_DYNAMITE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					if(pm->ps->weapon == WP_DYNAMITE) {
						pm->ps->grenadeTimeLeft = 50;
					}
					else {
						pm->ps->grenadeTimeLeft = 4000; // start at four seconds and count down

						if( pm->ps->eFlags & EF_PRONE )
							 BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON3PRONE, qfalse, qtrue );
						else
							 BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON3, qfalse, qtrue );
					}

					pm->ps->holdable[1] = 0; // holding nade
					PM_StartWeaponAnim(PM_AttackAnimForWeapon(pm->ps->weapon));
				}

				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;

		case WP_LANDMINE:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					if( pm->ps->eFlags & EF_PRONE ) {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
					}
					else {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
					}
				}

				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;

		case WP_SATCHEL:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					PM_StartWeaponAnim(PM_AttackAnimForWeapon(pm->ps->weapon));
				}

				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;

		case WP_SATCHEL_DET:
			if(!weaponstateFiring) {
				PM_AddEvent( EV_SPINUP );
				pm->ps->weaponDelay = GetWeaponTableData(pm->ps->weapon)->fireDelayTime;
				PM_ContinueWeaponAnim(PM_AttackAnimForWeapon(WP_SATCHEL_DET));
			}
			else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				}
				else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;
	}

	if ( PM_AltFiring(delayedFire) || PM_AltFire() )
		pm->ps->weaponstate = WEAPON_FIRINGALT;
	else
		pm->ps->weaponstate = WEAPON_FIRING;

	// check for out of ammo

	ammoNeeded = GetWeaponTableData(pm->ps->weapon)->uses;
	if ( pm->ps->weapon ) {
		qboolean	playswitchsound = qtrue;
		int			ammoAvailable = PM_WeaponAmmoAvailable(pm->ps->weapon);

		// jaquboss drain ammo if throwing
		// make sure we have one to hold
		if( ( pm->ps->weapon == WP_KNIFE || pm->ps->weapon == WP_KNIFE_KABAR ) && pm->ps->weaponstate == WEAPON_FIRINGALT  )
			ammoNeeded = 1;

		if(ammoNeeded > ammoAvailable && ammoAvailable != -1) {
			// you have ammo for this, just not in the clip
			qboolean reloading = (qboolean)(ammoNeeded <= pm->ps->ammo[ WeaponTable[pm->ps->weapon].ammoindex ]);

			// if not in auto-reload mode, and reload was not explicitely requested, just play the 'out of ammo' sound
			if (!pm->pmext->bAutoReload && WeaponTable[pm->ps->weapon].isAutoReload && !(pm->cmd.wbuttons & WBUTTON_RELOAD) ) {
				reloading = qfalse;
			}

			switch(pm->ps->weapon) {
				// Ridah, only play if using a triggered weapon
				case WP_DYNAMITE:
				case WP_GRENADE_LAUNCHER:
				case WP_GRENADE_PINEAPPLE:
				case WP_LANDMINE:
				case WP_SMOKE_BOMB:
				case WP_KNIFE:
				case WP_KNIFE_KABAR:
					playswitchsound = qfalse;
					break;

				// some weapons not allowed to reload.  must switch back to primary first
				case WP_FG42SCOPE:
				case WP_GARAND_SCOPE:
				case WP_K43_SCOPE:
					reloading = qfalse;
					break;
			}

			if(playswitchsound) {
				if( !reloading ) {
					PM_AddEvent( EV_NOAMMO );
				}
				// else PM_AddEvent( EV_EMPTYCLIP );
			}

			if(reloading) {
				PM_ContinueWeaponAnim(PM_ReloadAnimForWeapon(pm->ps->weapon));
			}
			else {
				PM_ContinueWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
				pm->ps->weaponTime += 500;
			}

			return;
		}
	}


	if(pm->ps->weaponDelay > 0) {
		// if it hits here, the 'fire' has just been hit and the weapon dictated a delay.
		// animations have been started, weaponstate has been set, but no weapon events yet. (except possibly EV_NOAMMO)
		// checks for delayed weapons that have already been fired are return'ed above.
		return;
	}

	if( !(pm->ps->eFlags & EF_PRONE) && (pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
		float fwdmove_knockback = 0.f;
		float bckmove_knockback = 0.f;

		switch( pm->ps->weapon ) {
			case WP_MOBILE_BROWNING:
			case WP_MOBILE_MG42:
			case WP_VENOM:
				fwdmove_knockback = 4000.f;
				bckmove_knockback = 400.f;
				break;

			case WP_BAZOOKA:
			case WP_PANZERFAUST:
				fwdmove_knockback = 32000.f;
				bckmove_knockback = 1200.f;
				break;

			case WP_FLAMETHROWER:
			case WP_SHOTGUN:
				fwdmove_knockback = 2000.f;
				bckmove_knockback = 40.f;
				break;

			default:
				break;
		}

		if( fwdmove_knockback > 0.f ) {
			// Add some knockback on slick
			vec3_t kvel;
			float mass = 200;

			if( DotProduct( pml.forward, pm->ps->velocity ) > 0  ) {
				VectorScale( pml.forward, -1.f * (fwdmove_knockback / mass), kvel );	// -1 as we get knocked backwards
			}
			else {
				VectorScale( pml.forward, -1.f * (fwdmove_knockback / mass), kvel );	// -1 as we get knocked backwards
			}

			VectorAdd( pm->ps->velocity, kvel, pm->ps->velocity );

			if ( !pm->ps->pm_time ) {
				pm->ps->pm_time = 100;
				pm->ps->pm_flags |= PMF_TIME_KNOCKBACK;
			}
		}
	}

	// take an ammo away if not infinite
	if(PM_WeaponAmmoAvailable(pm->ps->weapon) != -1 ) {
		// Rafael - check for being mounted on mg42
		if (!(pm->ps->persistant[PERS_HWEAPON_USE]) && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
			PM_WeaponUseAmmo(pm->ps->weapon, ammoNeeded);
		}
	}


	// fire weapon

	// add weapon heat
	if(GetWeaponTableData(pm->ps->weapon)->maxHeat) {
		pm->ps->weapHeat[pm->ps->weapon] += GetWeaponTableData(pm->ps->weapon)->nextShotTime;
	}

	// first person weapon animations

	// if this was the last round in the clip, play the 'lastshot' animation
	// this animation has the weapon in a "ready to reload" state
	if( isAkimbo ) {
		if( akimboFire ) {
			weapattackanim = WEAP_ATTACK1;
		}
		else {
			weapattackanim = WEAP_ATTACK2;
		}
	}
	else {
		if( PM_WeaponClipEmpty( pm->ps->weapon ) ) {
			weapattackanim = PM_LastAttackAnimForWeapon(pm->ps->weapon);
		}
		else {
			weapattackanim = PM_AttackAnimForWeapon(pm->ps->weapon);
		}
	}

	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
		case WP_MORTAR2_SET:
		case WP_MORTAR_SET:
			break;	// no animation
/*
		case WP_DYNAMITE:
		case WP_K43:
		case WP_KAR98:
		case WP_GPG40:
		case WP_CARBINE:
		case WP_M7:
		case WP_LANDMINE:
			PM_StartWeaponAnim(weapattackanim); // jaquboss - grenades has here 'pullanother' anim
			break;
*/
		case WP_MP40:
		case WP_STG44:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_STEN_MKII:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_SATCHEL_DET:
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_BAR:
		case WP_BAR_SET:
		case WP_VENOM:
			PM_ContinueWeaponAnim(weapattackanim);
			break;

		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			if ( pm->ps->weaponstate != WEAPON_FIRINGALT )
				PM_StartWeaponAnim(weapattackanim);
			break;

		default:
			PM_StartWeaponAnim(weapattackanim);
			break;
	}

	// JPW NERVE -- in multiplayer, pfaust fires once then switches to pistol since it's useless for a while
	// WP_M7 and WP_GPG40 run out of ammo immediately after firing their last grenade
	switch (pm->ps->weapon) {
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
			// core: do not let panzerfaust/bazooka switch to another weapon if we have unlimited charge,..
			// because during a panzerwar it's annoying to get a pistol after every shot.
			if ( !(PM_INSANITY & JP_INSANITY_CHARGEBAR) ) {
				PM_AddEvent( EV_NOAMMO );
			}
			break;

		case WP_DYNAMITE:
		case WP_SMOKE_BOMB:
		case WP_LANDMINE:
			PM_AddEvent( EV_NOAMMO );
			break;

		case WP_M7:
		case WP_GPG40:
			if ( !pm->ps->ammo[ WeaponTable[pm->ps->weapon].ammoindex ] ) {
				PM_AddEvent( EV_NOAMMO );
			}
			break;

		case WP_MORTAR_SET:
		case WP_MORTAR2_SET:
			if ( !pm->ps->ammo[WP_MORTAR] ) {
				PM_AddEvent( EV_NOAMMO );
			}
			break;

		case WP_SATCHEL:
			PM_AddEvent( EV_NOAMMO );
			pm->ps->ammoclip[WP_SATCHEL_DET] = 1;
			pm->ps->ammo[WP_SATCHEL] = 0;
			pm->ps->ammoclip[WP_SATCHEL] = 0;
			PM_BeginWeaponChange( WP_SATCHEL, WP_SATCHEL_DET, qfalse );
			break;

		// core: satchel shot-count bugfix..
		//       get another weapon immediately after throwing an airstrike..
		case WP_SATCHEL_DET:
		case WP_SMOKE_MARKER:
			PM_AddEvent( EV_FIRE_WEAPON );
			PM_AddEvent( EV_NOAMMO );
			return;

		default:
			break;
	}

	if( isAkimbo ) {
		if( akimboFire ) {
			PM_AddEvent( EV_FIRE_WEAPON );
		}
		else {
			PM_AddEvent( EV_FIRE_WEAPONB );
		}
	}
	else {
		if ( ( pm->ps->weapon == WP_KNIFE || pm->ps->weapon == WP_KNIFE_KABAR ) && pm->ps->weaponstate == WEAPON_FIRINGALT ){
			PM_AddEvent( EV_THROWKNIFE );
		}
		else if( PM_WeaponClipEmpty( pm->ps->weapon ) ) {
			PM_AddEvent( EV_FIRE_WEAPON_LASTSHOT );
		}
		else {
			PM_AddEvent( EV_FIRE_WEAPON );
		}
	}

	pm->pmext->releasedFire = qfalse;
	pm->ps->lastFireTime = pm->cmd.serverTime;
	pm->pmext->lastFireTime = pm->cmd.serverTime;

	aimSpreadScaleAdd = BG_Weapons[pm->ps->weapon].aimSpreadScaleAdd;
    // calc only if present for weapon
	if ( BG_Weapons[pm->ps->weapon].aimSpreadScaleAddRand ) {
		aimSpreadScaleAdd += rand() % BG_Weapons[pm->ps->weapon].aimSpreadScaleAddRand;
	}

	switch( pm->ps->weapon ) {
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			addTime = pm->ps->weaponstate == WEAPON_FIRINGALT ? 750 : GetWeaponTableData(pm->ps->weapon)->nextShotTime;
			break;

		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
			// if you're firing an akimbo weapon, and your other gun is dry,
			// nextshot needs to take 2x time
			addTime = GetWeaponTableData(pm->ps->weapon)->nextShotTime;

			// rain - fixed the swapped usage of akimboFire vs. the colt
			// so that the last shot isn't delayed
			if( !pm->ps->ammoclip[WeaponTable[pm->ps->weapon].clipindex] ) {
				if( !akimboFire )
					addTime = 2 * GetWeaponTableData(pm->ps->weapon)->nextShotTime;
			}
			else if( !pm->ps->ammoclip[WeaponTable[BG_AkimboSidearm(pm->ps->weapon)].clipindex] ) {
				if( akimboFire )
					addTime = 2 * GetWeaponTableData(pm->ps->weapon)->nextShotTime;
			}
			break;

		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_BAR:
		case WP_BAR_SET:
			if(weapattackanim == WEAP_ATTACK_LASTSHOT) {
				addTime = 2000;
			}
			else {
				addTime = GetWeaponTableData(pm->ps->weapon)->nextShotTime;
			}
			break;

        case WP_SATCHEL_DET:
        case WP_SATCHEL:
            break;

		default:
			addTime = GetWeaponTableData(pm->ps->weapon)->nextShotTime;
			break;
	}


	// set weapon recoil
	pm->pmext->lastRecoilDeltaTime = 0;

	// see if we have recoil in weapon file
	if ( BG_Weapons[pm->ps->weapon].recoil ){
		pm->pmext->weapRecoilTime = pm->cmd.serverTime;
		pm->pmext->weapRecoilDuration = BG_Weapons[pm->ps->weapon].recoilDuration;
		pm->pmext->weapRecoilYaw = BG_Weapons[pm->ps->weapon].recoilYaw;
		pm->pmext->weapRecoilPitch = BG_Weapons[pm->ps->weapon].recoilPitch;
	}
	else {
		// no recoil settings in .weap file
		switch( pm->ps->weapon ) {
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 300;
				pm->pmext->weapRecoilYaw = .5f;
				pm->pmext->weapRecoilPitch = .5f;
				break;
			case WP_FG42SCOPE:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 100;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = .45f;
				break;
			case WP_MOBILE_BROWNING:
			case WP_MOBILE_MG42:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 200;
				if( pm->ps->pm_flags & PMF_DUCKED || pm->ps->eFlags & EF_PRONE ) {
					pm->pmext->weapRecoilYaw = .5f;
					pm->pmext->weapRecoilPitch = .45f;
				} else {
					pm->pmext->weapRecoilYaw = .25f;
					pm->pmext->weapRecoilPitch = .75f;
				}
				break;
			//Meyer - Added Shotgun recoil
			case WP_SHOTGUN:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 250;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = .6f;
				break;
			//Meyer - Added BAR and BAR Bipod recoil
			case WP_BAR:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 25;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = .1f;
				break;
			case WP_BAR_SET:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 10;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = .05f;
				break;
			//Meyer - Added StG44 recoil
			case WP_STG44:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 20;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = .15f;
				break;

			case WP_LUGER:
			case WP_SILENCER:
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_SILENCEDLUGER:
			case WP_COLT:
			case WP_SILENCED_COLT:
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_SILENCEDCOLT:
			case WP_VENOM:
				pm->pmext->weapRecoilTime = pm->cmd.serverTime;
				pm->pmext->weapRecoilDuration = 100;
				pm->pmext->weapRecoilYaw = 0.f;
				pm->pmext->weapRecoilPitch = 0.45f;
				break;

			default:
				pm->pmext->weapRecoilTime = 0;
				pm->pmext->weapRecoilYaw = 0.0f;
				pm->pmext->weapRecoilPitch = 0.0f;
				break;
		}
	}

	// recoil: apply sway & reduction by skill..
	// jet Pilot - make higher level light weapons even lower recoil
	switch (pm->ps->weapon) {
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			pm->pmext->weapRecoilYaw *= cos(pm->cmd.serverTime);
			if (pm->skillBits[SK_COVERTOPS] & (1<<3)) {
				pm->pmext->weapRecoilPitch *= 0.5;
			}
			break;

		case WP_FG42SCOPE:
			pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime)) * .15f;
			if (pm->skillBits[SK_COVERTOPS] & (1<<3)) {
				pm->pmext->weapRecoilPitch *= 0.5;
			}
			break;

		case WP_MOBILE_BROWNING:
		case WP_MOBILE_MG42:
			pm->pmext->weapRecoilYaw *= cos(pm->cmd.serverTime);
			if( pm->ps->pm_flags & PMF_DUCKED || pm->ps->eFlags & EF_PRONE ) {
				pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime)) * .2f;
			} else {
				pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime)) * .15f;
			}
			break;

		case WP_SHOTGUN:
			pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime));
			break;

		case WP_BAR:
		case WP_BAR_SET:
			pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime));
			break;

		case WP_STG44:
			pm->pmext->weapRecoilPitch *= fabs(cos(pm->cmd.serverTime));
			break;

		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		// don't do it for the VENOM
			if (pm->skillBits[SK_LIGHT_WEAPONS] & (1<<3)) {
				pm->pmext->weapRecoilDuration *= 0.7;
				pm->pmext->weapRecoilPitch = 0.25f;
			}
			else {
				pm->pmext->weapRecoilPitch *= random() * .15f;
			}
			break;

		default:
			break;
	}


	// check for overheat - the weapon can overheat, and it's hot
	if(	GetWeaponTableData(pm->ps->weapon)->maxHeat && pm->ps->weapHeat[pm->ps->weapon]) {
		// it is overheating
		if(pm->ps->weapHeat[pm->ps->weapon] >= GetWeaponTableData(pm->ps->weapon)->maxHeat) {
			pm->ps->weapHeat[pm->ps->weapon] = GetWeaponTableData(pm->ps->weapon)->maxHeat;	// cap heat to max
			PM_AddEvent( EV_WEAP_OVERHEAT);
			// force "heat recovery minimum" to 2 sec right now
			// jet Pilot - Engineer Aura cuts cool time in half
			addTime = pm->ps->stats[STAT_TEAMAURA] & AURABIT_ENGINEER ? 1500 : 2000;
		}
	}

	// add the recoil amount to the aimSpreadScale
	pm->ps->aimSpreadScaleFloat += 3.0*aimSpreadScaleAdd;
	if (pm->ps->aimSpreadScaleFloat > 255)
		pm->ps->aimSpreadScaleFloat = 255;

	if(pm->skillBits[SK_COVERTOPS] & (1<<3) && pm->ps->stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
		pm->ps->aimSpreadScaleFloat *= .5f;
	}

	pm->ps->aimSpreadScale = (int)(pm->ps->aimSpreadScaleFloat);

	pm->ps->weaponTime += addTime;

	// jaquboss, pull another of those
	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
		case WP_SMOKE_MARKER:
			pm->ps->weaponstate = WEAPON_DROPPING;
			pm->ps->holdable[1] = 0;
			break;

		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			if ( pm->ps->weaponstate == WEAPON_FIRINGALT ){
				pm->ps->weaponstate = WEAPON_DROPPING;
				pm->ps->holdable[1] = 0;
			}
			break;

		default:
			break;
	}

	PM_SwitchIfEmpty();
}

// jaquboss kickin' handling
static void PM_Foot( void ) {
	qboolean delayedFire=qfalse;

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED )
		return;

	// ignore if spectator
	if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR )
		return;

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 )
		return;

	if ( pm->ps->eFlags & EF_PLAYDEAD )
		return;

	// special mounted mg42 handling
	if(pm->ps->persistant[PERS_HWEAPON_USE])
		return;

	if( pm->ps->eFlags & EF_MOUNTEDTANK )
		return;

	if ( pm->ps->pm_flags & PMF_DUCKED )
		return;

	if( pm->ps->eFlags & EF_PRONE )
		return;

	if( pm->pmext->kickDelay > 0 ) {
		pm->pmext->kickDelay -= pml.msec;

		if ( pm->pmext->kickDelay <= 0 ) {
			pm->pmext->kickDelay = 0;
			delayedFire = qtrue;
		}
	}

	// check for fire
	if( !(pm->cmd.buttons & BUTTON_GESTURE) || (pm->ps->leanf != 0) || delayedFire ){
		if ( delayedFire )
			PM_AddEvent( EV_KICK );
		return;
	}

	if ( pm->ps->pm_flags & PMF_TIME_LOCKPLAYER )
		return;

	if ( pm->cmd.serverTime < pm->pmext->kickTime )
		return;

	// Do not kick on mounted guns and do not allow on some heavy weapons either
	if( pm->ps->weapon == WP_MORTAR_SET ||
		pm->ps->weapon == WP_MORTAR2_SET ||
		pm->ps->weapon == WP_PANZERFAUST ||
		pm->ps->weapon == WP_BAZOOKA)
		return;

	// player is zooming - no kick
	if(pm->ps->eFlags & EF_ZOOMING)
		return;

	if(pm->waterlevel > 1)
		return;

	// lock us
	pm->ps->pm_flags |= PMF_TIME_LOCKPLAYER;
	pm->ps->pm_time = BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_KICK, qtrue, qtrue );
	pm->pmext->kickDelay = 250;
	pm->ps->ammoclip[WP_FOOTKICK] = ( (pm->ps->ammoclip[WP_FOOTKICK] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT ) | WEAP_ATTACK1;
	pm->pmext->kickTime = pm->cmd.serverTime + pm->ps->pm_time;
}

/*
================
PM_DropTimers
================
*/
static void PM_DropTimers( void ) {
	// drop misc timing counter
	if ( pm->ps->pm_time ) {
		if ( pml.msec >= pm->ps->pm_time ) {
			pm->ps->pm_flags &= ~PMF_ALL_TIMES;
			pm->ps->pm_time = 0;
		}
		else {
			pm->ps->pm_time -= pml.msec;
		}
	}

	// drop animation counter
	if ( pm->ps->legsTimer > 0 ) {
		pm->ps->legsTimer -= pml.msec;
		if ( pm->ps->legsTimer < 0 ) {
			pm->ps->legsTimer = 0;
		}
	}

	if ( pm->ps->torsoTimer > 0 ) {
		pm->ps->torsoTimer -= pml.msec;
		if ( pm->ps->torsoTimer < 0 ) {
			pm->ps->torsoTimer = 0;
		}
	}

	// first person weapon counter
	if ( pm->pmext->weapAnimTimer > 0 ) {
		pm->pmext->weapAnimTimer -= pml.msec;
		if ( pm->pmext->weapAnimTimer < 0 ) {
			pm->pmext->weapAnimTimer = 0;
		}
	}
}

#define LEAN_MAX		28.0f
#define LEAN_TIME_TO	200.0f	// time to get to/from full lean
#define LEAN_TIME_FR	300.0f	// time to get to/from full lean

/*
==============
PM_CalcLean

==============
*/

void PM_UpdateLean(playerState_t *ps, usercmd_t *cmd, pmove_t *tpm) {
	int			leaning = 0;	// -1 left, 1 right
	float		leanofs = 0;

	if( cmd->wbuttons & (WBUTTON_LEANLEFT | WBUTTON_LEANRIGHT)) {
		// jet Pilot - allow spectators to lean while moving
		if (ps->pm_type == PM_SPECTATOR) {
			if(cmd->wbuttons & WBUTTON_LEANLEFT)
				leaning -= 1;
			if(cmd->wbuttons & WBUTTON_LEANRIGHT)
				leaning += 1;
		}
		else if ((!cmd->forwardmove && cmd->upmove <= 0)) {
			// if both are pressed, result is no lean
			if(cmd->wbuttons & WBUTTON_LEANLEFT)
				leaning -= 1;
			if(cmd->wbuttons & WBUTTON_LEANRIGHT)
				leaning += 1;
		}
	}

	if(	BG_PlayerMounted(ps->eFlags) ) {
		leaning = 0;	// leaning not allowed on mg42
	}

	if(ps->eFlags & EF_FIRING )
		leaning = 0;	// not allowed to lean while firing

	if(ps->eFlags & EF_DEAD)
		leaning = 0;	// not allowed to lean while dead

	if (ps->eFlags & EF_PLAYDEAD )
		leaning = 0;

	// ATVI Wolfenstein Misc #479 - initial fix to #270 would crash in g_synchronousClients 1 situation
	if( ps->weaponstate == WEAPON_FIRING && ps->weapon == WP_DYNAMITE )
		leaning = 0; // not allowed while tossing dynamite

	if( ps->eFlags & EF_PRONE || ps->weapon == WP_MORTAR_SET || ps->weapon == WP_MORTAR2_SET )
		leaning = 0;	// not allowed to lean while prone

	leanofs = ps->leanf;

	if(!leaning) {	// go back to center position
		if ( leanofs > 0 ) {		// right
			//FIXME: play lean anim backwards?
			leanofs -= (((float)pml.msec/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs < 0 )
				leanofs = 0;
		}
		else if ( leanofs < 0 ) {	// left
			//FIXME: play lean anim backwards?
			leanofs += (((float)pml.msec/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs > 0 )
				leanofs = 0;
		}
	}

	if(leaning) {
		if(leaning > 0) { // right
			if(leanofs < LEAN_MAX)
				leanofs += (((float)pml.msec/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs > LEAN_MAX)
				leanofs = LEAN_MAX;
            ps->stats[STAT_PS_FLAGS] |= STAT_LEAN_RIGHT;
		}
		else {			// left
			if(leanofs > -LEAN_MAX)
				leanofs -= (((float)pml.msec/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs < -LEAN_MAX)
				leanofs = -LEAN_MAX;
            ps->stats[STAT_PS_FLAGS] |= STAT_LEAN_LEFT;
		}
    }
	else {
        ps->stats[STAT_PS_FLAGS] &= ~(STAT_LEAN_LEFT | STAT_LEAN_RIGHT);
    }

	ps->leanf = leanofs;

	if(leaning){
		vec3_t		start, end, tmins, tmaxs, right;
		vec3_t		viewangles;
		trace_t		trace;

		VectorCopy( ps->origin, start );
		start[2] += ps->viewheight;

		VectorCopy( ps->viewangles, viewangles );
		viewangles[ROLL] += leanofs/2.0f;
		AngleVectors( viewangles, NULL, right, NULL );
		VectorMA( start, leanofs, right, end );

		VectorSet( tmins, -8, -8, -7 ); // ATVI Wolfenstein Misc #472, bumped from -4 to cover gun clipping issue
		VectorSet( tmaxs, 8, 8, 4 );

		if( pm )
			pm->trace (&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);
		else
			tpm->trace (&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);

		ps->leanf *= trace.fraction;
	}

	// jet Pilot - Allow Spectators to lean while moving
	if(ps->leanf && ps->pm_type != PM_SPECTATOR ) { // jaquboss, was teamNum, but that actualy holds player class
		cmd->rightmove = 0;		// also disallowed in cl_input ~391
	}
}

#define	FLY_LEAN_TIME_FR	200.0f	// time to get to/from full lean
#define	FLY_LEAN_MAX		20.f
#define FLY_LEAN_TIME_TO	500.0f	// time to get to/from full lean

void PM_UpdateFlyLean(playerState_t *ps, float diff) {
	int			leaning = 0;	// -1 left, 1 right
	float		leanofs = 0;

	if( diff ) {
		if(diff > 70)
			leaning -= 1;
		if(diff < -70)
			leaning += 1;
	}

	if(	BG_PlayerMounted(ps->eFlags) ) {
		leaning = 0;	// leaning not allowed on mg42
	}

	leanofs = ps->viewangles[2];

	if(!leaning) {	// go back to center position
		if ( leanofs > 0 ) {		// right
			leanofs -= (((float)pml.msec/(float)FLY_LEAN_TIME_FR)*FLY_LEAN_MAX);
			if ( leanofs < 0 )
				leanofs = 0;
		}
		else if ( leanofs < 0 ) {	// left
			leanofs += (((float)pml.msec/(float)FLY_LEAN_TIME_FR)*FLY_LEAN_MAX);
			if ( leanofs > 0 )
				leanofs = 0;
		}
	}

	if(leaning) {
		if(leaning > 0) { // right
			if(leanofs < FLY_LEAN_MAX)
				leanofs += (((float)pml.msec/(float)FLY_LEAN_TIME_TO)*FLY_LEAN_MAX);

			if(leanofs > FLY_LEAN_MAX)
				leanofs = FLY_LEAN_MAX;
		}
		else {		// left

			if(leanofs > -FLY_LEAN_MAX)
				leanofs -= (((float)pml.msec/(float)FLY_LEAN_TIME_TO)*FLY_LEAN_MAX);

			if(leanofs < -FLY_LEAN_MAX)
				leanofs = -FLY_LEAN_MAX;
		}
	}

	ps->viewangles[2] = leanofs;
}

/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move

	!! NOTE !! Any changes to mounted/prone view should be duplicated in BotEntityWithinView()
	rain - take a tracemask as well - we can't use anything out of pm
================
*/
void PM_UpdateViewAngles( playerState_t *ps, pmoveExt_t *pmext, usercmd_t *cmd, void (trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int tracemask )
{
	short	temp;
	int		i;
	pmove_t tpm;
	vec3_t 	oldViewAngles;

	// DHM - Nerve :: Added support for PMF_TIME_LOCKPLAYER
	if ( ps->pm_type == PM_INTERMISSION || ps->pm_flags & PMF_TIME_LOCKPLAYER  ) {
		// jaquboss reset all angle changes, so it does not suddenly happens after unlocked
		ps->delta_angles[0] = ANGLE2SHORT( ps->viewangles[0] ) - cmd->angles[0];
		ps->delta_angles[1] = ANGLE2SHORT( ps->viewangles[1] ) - cmd->angles[1];
		ps->delta_angles[2] = ANGLE2SHORT( ps->viewangles[2] ) - cmd->angles[2];

		return;		// no view changes at all
	}

	if ( ps->pm_type != PM_SPECTATOR && ( ps->stats[STAT_HEALTH] <= 0 || ps->eFlags & EF_PLAYDEAD ) ) {
		// DHM - Nerve :: Allow players to look around while 'wounded' or lock to a medic if nearby
		temp = cmd->angles[1] + ps->delta_angles[1];

		if ( ps->eFlags & EF_PLAYDEAD ) {
			if ( temp > 16000 ) {
				ps->delta_angles[1] = 16000 - cmd->angles[1];
				temp = 16000;
			}
			else if ( temp < -16000 ) {
				ps->delta_angles[1] = -16000 - cmd->angles[1];
				temp = -16000;
			}
		}

		ps->stats[STAT_DEAD_YAW] = temp;
		ps->viewangles[2] = cmd->angles[2] = 0;
		ps->viewangles[0] = cmd->angles[0] = 0;

		tpm.trace = trace;
		//~ PM_UpdateLean( ps, cmd, &tpm );
		return;
	}

	VectorCopy( ps->viewangles, oldViewAngles );

	// circularly clamp the angles with deltas
	// Jaybird - #63 & #62 & #61
	// game-side delta_angles modifications are broken here if you exclude the ROLL calculation
	for (i=0 ; i<3 ; ++i) {
		temp = cmd->angles[i] + ps->delta_angles[i];
		if ( i == PITCH ) {
			// don't let the player look up or down more than 90 degrees
			if ( temp > 16000 ) {
				ps->delta_angles[i] = 16000 - cmd->angles[i];
				temp = 16000;
			}
			else if ( temp < -16000 ) {
				ps->delta_angles[i] = -16000 - cmd->angles[i];
				temp = -16000;
			}
		}
		ps->viewangles[i] = SHORT2ANGLE(temp);
	}

	if( BG_PlayerMounted(ps->eFlags) ) {
		float yaw = ps->viewangles[YAW], oldYaw = oldViewAngles[YAW];
		float arcMin, arcMax, arcDiff;

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		// MG42_YAWSPEED is degsSec
		if( yaw > oldYaw ) {
			if( yaw - oldYaw > MG42_YAWSPEED * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + MG42_YAWSPEED * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
		else if( oldYaw > yaw ) {
			if( oldYaw - yaw > MG42_YAWSPEED * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - MG42_YAWSPEED * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}

		// limit harc and varc

		// pitch (varc)
		arcMax = pmext->varc;
		if (ps->eFlags & EF_MOUNTEDTANK) {
			float angle;

			arcMin = 14;
			arcMax = 50;

			angle = cos( DEG2RAD( AngleNormalize180( pmext->centerangles[ 1 ] - ps->viewangles[ 1 ] )));
			angle = -AngleNormalize360(angle * AngleNormalize180( 0 - pmext->centerangles[ 0 ] ));

			pmext->centerangles[ PITCH ] = angle;
		}
		else {
			arcMin = pmext->varc / 2;
		}

		arcDiff = AngleNormalize180( ps->viewangles[PITCH] - pmext->centerangles[PITCH] );

		if( arcDiff > arcMin ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->centerangles[PITCH] + arcMin );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}
		else if( arcDiff < -arcMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->centerangles[PITCH] - arcMax );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}

		if (!(ps->eFlags & EF_MOUNTEDTANK)) {
			// yaw (harc)
			arcMin = arcMax = pmext->harc;
			arcDiff = AngleNormalize180( ps->viewangles[YAW] - pmext->centerangles[YAW] );

			if( arcDiff > arcMin ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->centerangles[YAW] + arcMin );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
			else if( arcDiff < -arcMax ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->centerangles[YAW] - arcMax );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
	}
	else if( ps->weapon == WP_MORTAR_SET ||  ps->weapon == WP_MORTAR2_SET ) {
		float degsSec = 60.f;
		float yaw = ps->viewangles[YAW], oldYaw = oldViewAngles[YAW];
		float pitch, oldPitch;
		float pitchMax = 30.f;
		float yawDiff, pitchDiff;

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
		else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}

		pitch = ps->viewangles[PITCH];
		oldPitch = oldViewAngles[PITCH];

		if ( pitch - oldPitch > 180 ) {
			pitch -= 360;
		}
		if ( pitch - oldPitch < -180 ) {
			pitch += 360;
		}

		if( pitch > oldPitch ) {
			if( pitch - oldPitch > degsSec * pml.frametime ) {
				ps->viewangles[PITCH] = oldPitch + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
			}
		}
		else if( oldPitch > pitch ) {
			if( oldPitch - pitch > degsSec * pml.frametime ) {
				ps->viewangles[PITCH] = oldPitch - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
			}
		}

		// yaw
		yawDiff = ps->viewangles[YAW] - pmext->mountedWeaponAngles[YAW];

		if( yawDiff > 180 ) {
			yawDiff -= 360;
		}
		else if( yawDiff < -180 ) {
			yawDiff += 360;
		}

		if( yawDiff > 30 ) {
			ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] + 30.f );

			// Set delta_angles properly
			ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
		}
		else if( yawDiff < -30 ) {
			ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] - 30.f );

			// Set delta_angles properly
			ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
		}

		// pitch
		pitchDiff = ps->viewangles[PITCH] - pmext->mountedWeaponAngles[PITCH];

		if( pitchDiff > 180 ) {
			pitchDiff -= 360;
		}
		else if( pitchDiff < -180 ) {
			pitchDiff += 360;
		}

		if( pitchDiff > (pitchMax -10.f) ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] + (pitchMax -10.f) );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}
		else if( pitchDiff < -(pitchMax) ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] - (pitchMax) );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}
	}
	else if((ps->eFlags & EF_PRONE) ||
			(ps->eFlags & EF_DEAD) ||
			(ps->eFlags & EF_PLAYDEAD)) {
		float oldYaw;
		trace_t traceres; // rain - renamed
		int newDeltaAngle = ps->delta_angles[YAW];
		float pitchMax = ps->weapon == WP_BAR_SET ? 50.f : 40.f;
		float pitchDiff;

		oldYaw = oldViewAngles[YAW];

		// Check if we are allowed to rotate to there
		if( ps->weapon == WP_MOBILE_MG42_SET || ps->weapon == WP_MOBILE_BROWNING_SET || ps->weapon == WP_BAR_SET ) {
			// yaw
			float yawDiff = ps->viewangles[YAW] - pmext->mountedWeaponAngles[YAW];

			pitchMax = 20.f;

			if( yawDiff > 180 ) {
				yawDiff -= 360;
			}
			else if( yawDiff < -180 ) {
				yawDiff += 360;
			}

			if( yawDiff > 20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] + 20.f );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
			else if( yawDiff < -20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] - 20.f );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}

		// pitch
		pitchDiff = ps->viewangles[PITCH] - pmext->mountedWeaponAngles[PITCH];

		if( pitchDiff > 180 ) {
			pitchDiff -= 360;
		}
		else if( pitchDiff < -180 ) {
			pitchDiff += 360;
		}

		if( pitchDiff > pitchMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] + pitchMax );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}
		else if( pitchDiff < -pitchMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] - pitchMax );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}

		// Check if we rotated into a wall with our legs, if so, undo yaw
		if( ps->viewangles[YAW] != oldYaw ) {
			// see if we have the space to go prone
			// we know our main body isn't in a solid, check for our legs

			ps->delta_angles[YAW] = newDeltaAngle;

			// rain - bugfix - use supplied trace - pm may not be set
			PM_TraceLegs( &traceres, &pmext->proneLegsOffset, ps->origin, ps->origin, NULL, ps->viewangles, pm->trace, ps->clientNum, tracemask );

			if(traceres.fraction < 1.0f) {
				// starting in a solid, no space
				ps->viewangles[YAW] = oldYaw;
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}

			memset(&traceres, 0, sizeof(traceres));

			PM_TraceHead(&traceres, ps->origin, ps->origin,
				NULL, ps->viewangles, pm->trace, ps->clientNum,
				tracemask );

			if(traceres.fraction < 1.0f) {
				ps->viewangles[YAW] = oldYaw;
				ps->delta_angles[YAW] =
					ANGLE2SHORT(ps->viewangles[YAW])
					- cmd->angles[YAW];
			}

		}
	}

	tpm.trace = trace;
	// forty - pmove_fixed - moved updatelean outta here. (Was causing lean to suck with pmove_fixed).
}

/*
================
PM_CheckLadderMove

  Checks to see if we are on a ladder
================
*/
qboolean	ladderforward;
vec3_t		laddervec;

void PM_CheckLadderMove (void)
{
	vec3_t	spot;
	vec3_t	flatforward;
	trace_t	trace;
	float	tracedist;
	#define	TRACE_LADDER_DIST	48.0
	qboolean wasOnLadder;

	if (pm->ps->pm_time)
		return;

	if (pml.walking) {
		tracedist = 1.0;
	}
	else {
		tracedist = TRACE_LADDER_DIST;
	}

	wasOnLadder = ((pm->ps->pm_flags & PMF_LADDER) != 0);

	pml.ladder = qfalse;
	pm->ps->pm_flags &= ~PMF_LADDER;	// clear ladder bit
	ladderforward = qfalse;


	if (pm->ps->stats[STAT_HEALTH] <= 0) {
		pm->ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// Can't climb ladders while prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return;
	}

	// check for ladder
	flatforward[0] = pml.forward[0];
	flatforward[1] = pml.forward[1];
	flatforward[2] = 0;
	VectorNormalize (flatforward);

	VectorMA (pm->ps->origin, tracedist, flatforward, spot);
	pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, spot, pm->ps->clientNum, pm->tracemask);
	if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER)) {
		pml.ladder = qtrue;
	}

	if (pml.ladder) {
		VectorCopy( trace.plane.normal, laddervec );
	}

	if (pml.ladder && !pml.walking && (trace.fraction * tracedist > 1.0)) {
		vec3_t mins;
		// if we are only just on the ladder, don't do this yet, or it may throw us back off the ladder
		pml.ladder = qfalse;
		VectorCopy( pm->mins, mins );
		mins[2] = -1;
		VectorMA (pm->ps->origin, -tracedist, laddervec, spot);
		pm->trace (&trace, pm->ps->origin, mins, pm->maxs, spot, pm->ps->clientNum, pm->tracemask);
		if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER)) {
			ladderforward = qtrue;
			pml.ladder = qtrue;
			pm->ps->pm_flags |= PMF_LADDER;	// set ladder bit
		}
		else {
			pml.ladder = qfalse;
		}
	}
	else if (pml.ladder) {
		pm->ps->pm_flags |= PMF_LADDER;	// set ladder bit
	}

	// create some up/down velocity if touching ladder
	if ( pml.ladder ) {
		if (pml.walking) {
			// we are currently on the ground, only go up and prevent X/Y if we are pushing forwards
			if (pm->cmd.forwardmove <= 0) {
				pml.ladder = qfalse;
			}
		}
	}

	// if we have just dismounted the ladder at the top, play dismount
	if (!pml.ladder && wasOnLadder && pm->ps->velocity[2] > 0) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_CLIMB_DISMOUNT, qfalse, qfalse );
	}
	// if we have just mounted the ladder
	if (pml.ladder && !wasOnLadder && pm->ps->velocity[2] < 0) {	// only play anim if going down ladder
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_CLIMB_MOUNT, qfalse, qfalse );
	}
}

/*
============
PM_LadderMove
============
*/
void PM_LadderMove (void) {
	float	wishspeed, scale;
	vec3_t	wishdir, wishvel;
	float	upscale;

	if (ladderforward) {
		// move towards the ladder
		VectorScale( laddervec, -200.0, wishvel );
		pm->ps->velocity[0] = wishvel[0];
		pm->ps->velocity[1] = wishvel[1];
	}

	upscale = (pml.forward[2] + 0.5)*2.5;
	if (upscale > 1.0) {
		upscale = 1.0;
	}
	else if (upscale < -1.0) {
		upscale = -1.0;
	}

	// forward/right should be horizontal only
	pml.forward[2] = 0;
	pml.right[2] = 0;
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	// move depending on the view, if view is straight forward, then go up
	// if view is down more then X degrees, start going down
	// if they are back pedalling, then go in reverse of above
	scale = PM_CmdScale( &pm->cmd );
	VectorClear( wishvel );

	if( pm->cmd.forwardmove ) {
		wishvel[2] = 0.9 * upscale * scale * (float)pm->cmd.forwardmove;
	}

	if( pm->cmd.rightmove ) {
		// strafe, so we can jump off ladder
		vec3_t	ladder_right, ang;
		vectoangles( laddervec, ang );
		AngleVectors( ang, NULL, ladder_right, NULL );

		// if we are looking away from the ladder, reverse the right vector
		if( DotProduct( laddervec, pml.forward ) < 0 ) {
			VectorInverse( ladder_right );
		}

		VectorMA( wishvel,  0.5f * scale * (float)pm->cmd.rightmove, ladder_right, wishvel );
	}

	// do strafe friction
	PM_Friction();

	if( pm->ps->velocity[0] < 1 && pm->ps->velocity[0] > -1  )
		pm->ps->velocity[0] = 0;
	if( pm->ps->velocity[1] < 1&& pm->ps->velocity[1] > -1 )
		pm->ps->velocity[1] = 0;

	wishspeed = VectorNormalize2( wishvel, wishdir );

	PM_Accelerate( wishdir, wishspeed, pm_accelerate );
	if( !wishvel[2] ) {
		if (pm->ps->velocity[2] > 0) {
			pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
			if (pm->ps->velocity[2] < 0)
				pm->ps->velocity[2]  = 0;
		}
		else {
			pm->ps->velocity[2] += pm->ps->gravity * pml.frametime;
			if (pm->ps->velocity[2] > 0)
				pm->ps->velocity[2]  = 0;
		}
	}

	PM_StepSlideMove (qfalse);	// no gravity while going up ladder

	// always point legs forward
	pm->ps->movementDir = 0;
}

/*
==============
PM_Sprint
==============
*/
void PM_Sprint( void )
{
	//       sprint button              +   forward or side movement                 +  not crouching                    +  not prone
	if (pm->cmd.buttons & BUTTON_SPRINT && (pm->cmd.forwardmove || pm->cmd.rightmove) && !(pm->ps->pm_flags & PMF_DUCKED) && !(pm->ps->eFlags & EF_PRONE) ) {
		if( (pm->ps->powerups[PW_ADRENALINE]>0) && !(PM_ADRENALINE & ADF_NOSTAMINA) ) {
			pm->pmext->sprintTime = SPRINTTIME;
		}
		else if(pm->ps->powerups[PW_NOFATIGUE]) {
			// take time from powerup before taking it from sprintTime
			pm->ps->powerups[PW_NOFATIGUE] -= 50;

			// (SA) go ahead and continue to recharge stamina at double
			// rate with stamina powerup even when exerting
			pm->pmext->sprintTime += 10;
			if (pm->pmext->sprintTime > SPRINTTIME)
				pm->pmext->sprintTime = SPRINTTIME;

			if(pm->ps->powerups[PW_NOFATIGUE] < 0)
				 pm->ps->powerups[PW_NOFATIGUE] = 0;
		}
		// JPW NERVE -- sprint time tuned for multiplayer
		else {
			// JPW NERVE adjusted for framerate independence
			if (pm->ps->stats[STAT_TEAMAURA] & AURABIT_FIELDOPS) {
				pm->pmext->sprintTime -= 2500*pml.frametime;
			}
			else {
				pm->pmext->sprintTime -= 5000*pml.frametime;
			}
		}
		// jpw

		if (pm->pmext->sprintTime < 0)
			pm->pmext->sprintTime = 0;

		if (!pm->ps->sprintExertTime) {
			pm->ps->sprintExertTime = 1;
		}
	}
	else {
		// JPW NERVE -- in multiplayer, recharge faster for top 75% of sprint bar
		// (for people that *just* use it for jumping, not sprint) this code was
		// mucked about with to eliminate client-side framerate-dependancy in wolf single player
		if( (pm->ps->powerups[PW_ADRENALINE]>0) && !(PM_ADRENALINE & ADF_NOSTAMINA) ) {
			pm->pmext->sprintTime = SPRINTTIME;
		}
		else if(pm->ps->powerups[PW_NOFATIGUE]) { 	// (SA) recharge at 2x with stamina powerup

			pm->pmext->sprintTime += 10;
		}
		else {
			// jet Pilot - reintroduce leadership
			int rechargebase = (pm->ps->stats[STAT_TEAMAURA] & AURABIT_FIELDOPS) ? 1500 : 500;

			if( pm->skillBits[SK_BATTLE_SENSE] & (1<<2) ) {
				rechargebase *= 1.6f;
			}

			pm->pmext->sprintTime += rechargebase * pml.frametime;		// JPW NERVE adjusted for framerate independence
			if (pm->pmext->sprintTime > 5000)
				pm->pmext->sprintTime += rechargebase * pml.frametime;	// JPW NERVE adjusted for framerate independence
			// jpw
		}
		if (pm->pmext->sprintTime > SPRINTTIME) {
			pm->pmext->sprintTime = SPRINTTIME;
		}

		pm->ps->sprintExertTime = 0;
	}
}

/*
================
PmoveSingle

================
*/
void trap_SnapVector( float *v );

// Check attack command
void PM_CheckAttack( void )
{
	if ( pm->ps->pm_flags & PMF_RESPAWNED )
		return;

	if ( pm->ps->pm_type == PM_INTERMISSION )
		return;

	// check for ammo
	if( !PM_WeaponAmmoAvailable(pm->ps->weapon))
		return;

	// check if zooming or leaning
	if( pm->ps->eFlags & EF_ZOOMING)
		return;

	// check if the weapon is ready to fire or already firing
	if ( pm->ps->weaponstate != WEAPON_READY && pm->ps->weaponstate != WEAPON_FIRING )
		return;

	// make sure attack button is down and the talk button isn't
	if (!(pm->cmd.buttons & BUTTON_ATTACK))
		return;
	if ( pm->cmd.buttons & BUTTON_TALK )
		return;

	// jet Pilot - lean-fire is now lvl 9 lw
	if (pm->ps->leanf != 0 )
		return;

	pm->ps->eFlags |= EF_FIRING;
}

void PmoveSingle (pmove_t *pmove)
{
	// RF, update conditional values for anim system
	BG_AnimUpdatePlayerStateConditions( pmove );

	pm = pmove;

	// this counter lets us debug movement problems with a journal
	// by setting a conditional breakpoint fot the previous frame
	c_pmove++;

	// clear results
	pm->numtouch	= 0;
	pm->watertype	= 0;
	pm->waterlevel	= 0;

	// check health and set contents and zoom flags
	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		pm->tracemask  &= ~CONTENTS_BODY;	// corpses can fly through bodies
		pm->ps->eFlags &= ~EF_ZOOMING;
	}

	// make sure walking button is clear if they are running, to avoid
	// proxy no-footsteps cheats
	if ( abs( pm->cmd.forwardmove ) > 64 || abs( pm->cmd.rightmove ) > 64 ) {
		pm->cmd.buttons &= ~BUTTON_WALKING;
	}

	// set the talk balloon flag
	if ( pm->cmd.buttons & BUTTON_TALK ) {
		pm->ps->eFlags |= EF_TALK;
	}
	else {
		pm->ps->eFlags &= ~EF_TALK;
	}

	// Clear Zoom and Firing flags
	pm->ps->eFlags &= ~(EF_FIRING | EF_ZOOMING);

	// Check zoom command
    if( (pm->cmd.wbuttons & WBUTTON_ZOOM) && (pm->ps->eFlags & (EF_DEAD|EF_PLAYDEAD)) == 0 && !( pm->ps->weaponDelay )) {
		if(pm->ps->stats[STAT_KEYS] & (1<<INV_BINOCS)) {		// (SA) binoculars are an inventory item (inventory==keys)

			if (!IS_SCOPED_WEAPON(pm->ps->weapon)		 &&		// don't allow binocs if using the sniper scope
				!BG_PlayerMounted(pm->ps->eFlags)		 &&		// or if mounted on a weapon
				// rain - #215 - don't allow binocs w/ mounted mob. MG42 or mortar either.
				pm->ps->weapon != WP_MOBILE_MG42_SET	 &&
				pm->ps->weapon != WP_MOBILE_BROWNING_SET &&
				pm->ps->weapon != WP_BAR_SET			 &&
				pm->ps->weapon != WP_MORTAR_SET			 &&
				pm->ps->weapon != WP_MORTAR2_SET )
			{
				// jaquboss do not zoom while reloading and suc, artyFire is set if used binocs
				if ( (!pm->ps->weaponTime || pm->pmext->artyFire) && !pm->pmext->kickDelay && ( pm->cmd.serverTime >= pm->pmext->kickTime ) )
					pm->ps->eFlags |= EF_ZOOMING;
			}
		}

		// don't allow binocs if in the middle of throwing grenade
		if((pm->ps->weapon == WP_GRENADE_LAUNCHER || pm->ps->weapon == WP_GRENADE_PINEAPPLE || pm->ps->weapon == WP_DYNAMITE) && pm->ps->grenadeTimeLeft > 0) {
			pm->ps->eFlags &= ~EF_ZOOMING;
		}
	}

	PM_CheckAttack(); // jaquboss, so we don't have to list million of conditions here..


	// give a silencer to covert ops
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		if( pm->ps->stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
			pm->pmext->silencedSideArm |= 1;
		}
	}

	// clear the respawned flag if attack and use are cleared
	if ( pm->ps->stats[STAT_HEALTH] > 0 && !( pm->cmd.buttons & (BUTTON_ATTACK | BUTTON_USE_HOLDABLE) ) ) {
		pm->ps->pm_flags &= ~PMF_RESPAWNED;
	}

	// if talk button is down, dissallow all other input
	// this is to prevent any possible intercept proxy from
	// adding fake talk balloons
	if ( pmove->cmd.buttons & BUTTON_TALK ) {
		// keep the talk button set tho for when the cmd.serverTime > 66 msec
		// and the same cmd is used multiple times in Pmove
		pmove->cmd.buttons		= BUTTON_TALK;
		pmove->cmd.wbuttons		= 0;
		pmove->cmd.forwardmove	= 0;
		pmove->cmd.rightmove	= 0;
		pmove->cmd.upmove		= 0;
		pmove->cmd.doubleTap	= 0;
	}

	// fretn, moved from engine to this very place
	// no movement while using a static mg42
	if (pm->ps->persistant[PERS_HWEAPON_USE]) {
		pmove->cmd.forwardmove	= 0;
		pmove->cmd.rightmove	= 0;
		pmove->cmd.upmove		= 0;
	}

	// clear all pmove local vars
	memset (&pml, 0, sizeof(pml));

	// determine the time
	pml.msec = pmove->cmd.serverTime - pm->ps->commandTime;
	if ( pml.msec < 1 ) {
		pml.msec = 1;
	}
	else if ( pml.msec > 200 ) {
		pml.msec = 200;
	}

	pm->ps->commandTime = pmove->cmd.serverTime;

	// save old org in case we get stuck
	VectorCopy (pm->ps->origin, pml.previous_origin);

	// save old velocity for crashlanding
	VectorCopy (pm->ps->velocity, pml.previous_velocity);

	pml.frametime = pml.msec * 0.001;

	// update the viewangles
	if( pm->ps->pm_type != PM_FREEZE ) {	// Arnout: added PM_FREEZE
		if (!(pm->ps->pm_flags & PMF_LIMBO)) { // JPW NERVE
			// rain - added tracemask
				PM_UpdateViewAngles( pm->ps, pm->pmext, &pm->cmd, pm->trace, pm->tracemask );	//----(SA)	modified
				if ( PM_LEAN ) {
					// forty - pmove_fixed - moved update lean out of UpdateViewAngles
					PM_UpdateLean( pm->ps, &pm->cmd, pm );
				}
		}
	}

	AngleVectors (pm->ps->viewangles, pml.forward, pml.right, pml.up);

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		pm->ps->pm_flags &= ~PMF_JUMP_HELD;
	}

	// decide if backpedaling animations should be used
	if ( pm->cmd.forwardmove < 0 ) {
		pm->ps->pm_flags |= PMF_BACKWARDS_RUN;
	}
	else if ( pm->cmd.forwardmove > 0 || ( pm->cmd.forwardmove == 0 && pm->cmd.rightmove ) ) {
		pm->ps->pm_flags &= ~PMF_BACKWARDS_RUN;
	}

	// clear movement if the player shouldn't be able to move on their own
	if ( pm->ps->pm_type >= PM_DEAD || pm->ps->pm_flags & ( PMF_LIMBO | PMF_TIME_LOCKPLAYER ) ) { // DHM - Nerve

		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove	= 0;
		pm->cmd.upmove		= 0;
	}


	switch ( pm->ps->pm_type ) {
		case PM_SPECTATOR: // Setup spectator movement
			PM_CheckDuck ();
			PM_FlyMove ();
			PM_DropTimers ();
			return;
		case PM_NOCLIP:
			PM_NoclipMove ();
			PM_DropTimers ();
			return;
		case PM_FREEZE:
		case PM_INTERMISSION:
			return; // no movement at all

		default:
			break;
	}

	// Make sure players with stationary mortars dont move
	if( (pm->ps->weapon == WP_MORTAR_SET || pm->ps->weapon == WP_MORTAR2_SET || pm->ps->pm_flags & PMF_MEDICUSE  )  && pm->ps->pm_type == PM_NORMAL ) {
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove	= 0;
		pm->cmd.upmove		= 0;
	}

	// set watertype, and waterlevel
	PM_SetWaterLevel();
	pml.previous_waterlevel = pmove->waterlevel;

	// Check playdead status
	PM_CheckPlayDead();

	// set mins, maxs, and viewheight
	if( !PM_CheckProne() ) {
		PM_CheckDuck();
	}

	// set groundentity
	PM_GroundTrace();

	if ( pm->ps->pm_type == PM_DEAD ) {
		PM_DeadMove ();

		if( pm->ps->weapon == WP_MORTAR_SET ) {
			pm->ps->weapon = WP_MORTAR;
		}

		if( pm->ps->weapon == WP_MORTAR2_SET ) {
			pm->ps->weapon = WP_MORTAR2;
		}
	}
	else {	// Switch from deployed weapons to non-deployed if leaving prone

		if( pm->ps->weapon == WP_MOBILE_BROWNING_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) ) {
				PM_BeginWeaponChange( WP_MOBILE_BROWNING_SET, WP_MOBILE_BROWNING, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_MOBILE_BROWNING;
#endif//CGAMEDLL
			}
		}
		else if( pm->ps->weapon == WP_BAR_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) ) {
				PM_BeginWeaponChange( WP_BAR_SET, WP_BAR, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_BAR;
#endif//CGAMEDLL
			}
		}
		else if( pm->ps->weapon == WP_MOBILE_MG42_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) ) {
				PM_BeginWeaponChange( WP_MOBILE_MG42_SET, WP_MOBILE_MG42, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_MOBILE_MG42;
#endif//CGAMEDLL
			}
		}
	}

	// Ridah, ladders
	PM_CheckLadderMove();

	PM_DropTimers();
	if (pml.ladder) {
		PM_LadderMove();
	}
	else if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) {
		PM_WaterJumpMove();
	}
	else if ( pm->waterlevel > 1 ) {	// swimming
		PM_WaterMove();
	}
	else if ( !(pm->ps->eFlags & EF_MOUNTEDTANK) ) {
		if ( pml.walking ) {
			PM_WalkMove();	// walking on ground
		}
		else {
			PM_AirMove();	// airborne
		}
	}
	else {
		VectorClear( pm->ps->velocity );
		pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
		BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
	}


	PM_Sprint();

	// set groundentity, watertype, and waterlevel
	PM_GroundTrace();
	PM_SetWaterLevel();

	// weapons
	PM_AltWeapon();
	PM_Weapon();
	// jaquboss
	PM_Foot();

	// footstep events / legs animations
	PM_Footsteps();

	// entering / leaving water splashes
	PM_WaterEvents();

	// snap some parts of playerstate to save network bandwidth
	if(PM_FIXEDPHYSICS) {
		// forty - Haste's Pmove Accurate Code
		// the new way: don't care so much about 6 bytes/frame
		// or so of network bandwidth, and add some mostly framerate-
		// independent error to make up for the lack of rounding error
		// halt if not going fast enough (0.5 units/sec)
		if ( VectorLengthSquared( pm->ps->velocity ) < 0.25f ) {
			VectorClear( pm->ps->velocity );
		}
		else {
			int i;
			float fac;
			int	fps = PM_FIXEDPHYSICSFPS;

			if ( fps > 333 )
				fps = 333;
			else if ( fps < 60 )
				fps = 60;

			fac = (float)pml.msec / (1000.0f/(float)fps);

			// add some error...
			for ( i = 0; i < 3; ++i ) {
				// ...if the velocity in this direction changed enough
				if ( fabs(pm->ps->velocity[i] - pml.previous_velocity[i]) > 0.5f / fac ) {
					if ( pm->ps->velocity[i] < 0 ) {
						pm->ps->velocity[i] -= 0.5f * fac;
					}
					else {
						pm->ps->velocity[i] += 0.5f * fac;
					}
				}
			}
			// we can stand a little bit of rounding error for the sake
			// of lower bandwidth
			VectorScale( pm->ps->velocity, 64.0f, pm->ps->velocity );
			trap_SnapVector( pm->ps->velocity );
			VectorScale( pm->ps->velocity, 1.0f / 64.0f, pm->ps->velocity );
		}
	}
	else {
		// snap some parts of playerstate to save network bandwidth
		trap_SnapVector( pm->ps->velocity );
	}

	// core: save sprinttime for CG_DrawStaminaBar()
	pm->ps->stats[STAT_SPRINTTIME] = pm->pmext->sprintTime;
	// core: save airtime for CG_DrawBreathBar()
	pm->ps->stats[STAT_AIRLEFT] = pm->pmext->airleft;
}

/*
================
Pmove

Can be called by either the server or the client
================
*/
int Pmove (pmove_t *pmove)
{
	int		finalTime	= pmove->cmd.serverTime;
	int		gravity 	= pmove->ps->gravity;

	if ( finalTime < pmove->ps->commandTime ) {
		return (0);	// should not happen
	}

	if ( finalTime > pmove->ps->commandTime + 1000 ) {
		pmove->ps->commandTime = finalTime - 1000;
	}

	pmove->ps->pmove_framecount = (pmove->ps->pmove_framecount+1) & ((1<<PS_PMOVEFRAMECOUNTBITS)-1);

	// RF
	pm = pmove;

	// chop the move up if it is too long, to prevent framerate
	// dependent behavior
	while ( pmove->ps->commandTime != finalTime ) {
		int		msec = finalTime - pmove->ps->commandTime;

		if ( pmove->pmove_fixed ) {
			if ( msec > pmove->pmove_msec ) {
				msec = pmove->pmove_msec;
			}
		}
		else {
			// rain - this was 66 (15fps), but I've changed it to
			// 50 (20fps, max rate of mg42) to alleviate some of the
			// framerate dependency with the mg42.
			// in reality, this should be split according to sv_fps,
			// and pmove() shouldn't handle weapon firing
#ifdef GAMEDLL
			if ( msec > 1000/PM_SV_FPS ) { // @SV_FPS
				msec = 50;
			}
#else
			if ( msec > 50 ) { // @SV_FPS
				msec = 50;
			}
#endif
		}
		pmove->cmd.serverTime = pmove->ps->commandTime + msec;

		PM_AdjustAimSpreadScale (); // jaquboss, moved here
		pmove->ps->gravity = gravity;
		PmoveSingle( pmove );

		if ( pmove->ps->pm_flags & PMF_JUMP_HELD ) {
			pmove->cmd.upmove = 20;
		}
	}

	// rain - sanity check weapon heat
	if (pmove->ps->curWeapHeat > 255)
		pmove->ps->curWeapHeat = 255;
	else if (pmove->ps->curWeapHeat < 0)
		pmove->ps->curWeapHeat = 0;


	if ( (pm->ps->stats[STAT_HEALTH] <= 0 || pm->ps->pm_type == PM_DEAD ) && pml.groundTrace.surfaceFlags & SURF_MONSTERSLICK )
		return (pml.groundTrace.surfaceFlags);
	else
		return (0);

}

// PmovePredict
//
// Used to calculate player movement for g_skipCorrection.
//
// Before calling PmovePredict() the following player state
// values should be backed up and then restored after the
// new values have been copied to the entity state .
//
// PM_GroundTrace() and friends modify
// 	ps->groundEntityNum
// 	ps->pm_flags
// 	ps->pm_time
// 	ps->eFlags
//
// PM_StepSlideMove() and friends modify
// 	ps->origin
// 	ps->velocity

void PmovePredict(pmove_t *pmove, float frametime)
{
	pm = pmove;
	memset(&pml, 0, sizeof(pml));
	pml.frametime = frametime;
	PM_GroundTrace();

	// tjw: don't bother to figure out gravity if already on the ground
	//      or moving on a ladder.
	if(pml.groundPlane || (pm->ps->pm_flags & PMF_LADDER)) {
		PM_StepSlideMove(qfalse);
	}
	else {
		PM_StepSlideMove(qtrue);
	}
}
