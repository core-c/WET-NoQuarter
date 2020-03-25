/*
 * name:		cg_view.c
 *
 * desc:		setup all the parameters (position, angle, etc)
 *
 * NQQS:		Needs inspection - open TODOs
 *
 */

#include "cg_local.h"

// extern 	pmove_t		cg_pmove;

#ifdef _DEBUG
/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

If a gun is being tested, the "gun_x", "gun_y", and "gun_z" variables will let
you adjust the positioning.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/

/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestModel_f (void) {
	vec3_t		angles;
	int			args;

	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	args = trap_Argc();

	if ( args ) {
		return;
	}

	Q_strncpyz (cg.testModelName, CG_Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );

	if (! cg.testModelEntity.hModel ) {
		CG_Printf( "Can't register model\n" );
		return;
	}

	if ( args >= 3 ) {
		cg.testModelEntity.backlerp = atof( CG_Argv( 2 ) );
		cg.testModelEntity.frame = args > 3 ? atoi( CG_Argv(3) ) : 1;
		cg.testModelEntity.oldframe = 0;
	}

	VectorMA( cg.refdef.vieworg, 100.0f, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH]	= 0;
	angles[YAW]		= 180 + cg.refdefViewAngles[1];
	angles[ROLL]	= 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}

/*
=================
CG_TestGun_f

Replaces the current view weapon with the given model
=================
*/
void CG_TestGun_f (void) {
	CG_TestModel_f();
	cg.testGun = qtrue;
	cg.testModelEntity.renderfx = RF_MINLIGHT | RF_DEPTHHACK | RF_FIRST_PERSON;
}


void CG_TestModelNextFrame_f (void) {
	cg.testModelEntity.frame++;
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelPrevFrame_f (void) {
	cg.testModelEntity.frame--;
	if ( cg.testModelEntity.frame < 0 ) {
		cg.testModelEntity.frame = 0;
	}
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelNextSkin_f (void) {
	cg.testModelEntity.skinNum++;
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_TestModelPrevSkin_f (void) {
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 ) {
		cg.testModelEntity.skinNum = 0;
	}
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

static void CG_AddTestModel (void) {
	// re-register the model, because the level may have changed
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );
	if (! cg.testModelEntity.hModel ) {
		CG_Printf ("Can't register model\n");
		return;
	}

	// if testing a gun, set the origin reletive to the view origin
	if ( cg.testGun ) {
		int		i;

		VectorCopy( cg.refdef.vieworg, cg.testModelEntity.origin );
		VectorCopy( cg.refdef.viewaxis[0], cg.testModelEntity.axis[0] );
		VectorCopy( cg.refdef.viewaxis[1], cg.testModelEntity.axis[1] );
		VectorCopy( cg.refdef.viewaxis[2], cg.testModelEntity.axis[2] );

		// allow the position to be adjusted
		for (i=0 ; i<3 ; i++) {
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[0][i] * cg_gun_x.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[1][i] * cg_gun_y.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[2][i] * cg_gun_z.value;
		}
	}

	trap_R_AddRefEntityToScene( &cg.testModelEntity );
}
//============================================================================
#endif

/*
=================
CG_CalcVrect

Sets the coordinates of the rendered window
=================
*/

//static float letterbox_frac = 1.0f;	// used for transitioning to letterbox for cutscenes
void CG_Letterbox( float xsize, float ysize, qboolean center ) {
	// normal aspect is xx:xx

	// letterbox is yy:yy  (85% of 'normal' height)
	/*
	if(cg_letterbox.integer) { // IRATA: removed cg_letterbox
		float	lbheight = ysize * 0.85, lbdiff = ysize - lbheight;

		if( !center ) {
			int offset = (cgs.glconfig.vidHeight*(.5f * lbdiff))*0.01f;	// /100;
			offset &= ~1;
			cg.refdef.y += offset;
		}

		ysize = lbheight;
	}
	 */
	cg.refdef.width = cgs.glconfig.vidWidth*xsize/100;
	cg.refdef.width &= ~1;

	cg.refdef.height = cgs.glconfig.vidHeight*ysize*0.01f;	// /100;
	cg.refdef.height &= ~1;

	if( center ) {
		cg.refdef.x = (cgs.glconfig.vidWidth - cg.refdef.width)/2;
		cg.refdef.y = (cgs.glconfig.vidHeight - cg.refdef.height)/2;
	}
}

static void CG_CalcVrect (void) {
	if ( cg.showGameView ) {
 		float x = LIMBO_3D_X, y = LIMBO_3D_Y, w = LIMBO_3D_W, h = LIMBO_3D_H;

		// core: the limbopanel is horizontally centered on widescreens..
		x += cgs.wideXoffset;

		CG_AdjustFrom640( &x, &y, &w, &h );

 		cg.refdef.x = x;
 		cg.refdef.y = y;
 		cg.refdef.width = w;
 		cg.refdef.height = h;

		CG_Letterbox( (LIMBO_3D_W/640.f)*100, (LIMBO_3D_H/480.f)*100, qfalse );

		// core: the limbopanel objective camera is always rendered at a 4:3 aspectratio..
		if (!Ccg_Is43Screen()) cg.refdef.width *= cgs.r43da;

		return;
	}

	CG_Letterbox( 100, 100, qtrue );
}

//==============================================================================

/*
===============
core:
C_ViewlockTransition

Creates a transition of view yaw angles when VIEWLOCK_MEDIC/VIEWLOCK_KILLER toggles..
Smoothly "turning the head" to look at the medic/killer, and/or to look away from him again.
===============
*/
#define VIEWYAW_LOOKAT_TIME		200		// 200 ms
#define VIEWYAW_LOOKAWAY_TIME	100		// 100 ms

void C_ViewlockTransition( int action ) {
	static int		viewlockTime;	// view is locked to medic/killer..
	static int		viewunlockTime;	// view is unlocked from medic/killer..
	static float	viewyaw;		// current transition yaw angle..
	vec3_t			vec;
	float			frac;
	vec3_t			tmpvec;
	float			yaw;

	switch( action ) {

		case 1:	// transition view unlocked -> lock.. (look at medic/killer)
			VectorCopy( cg_entities[cg.predictedPlayerState.viewlocked_entNum].lerpOrigin, vec );
			VectorSubtract( vec, cg.refdef_current->vieworg, vec );
			vectoangles( vec, tmpvec );
			// make a smooth yaw angle transition when the dead player starts looking at the medic..
			if (viewlockTime == 0) viewlockTime = cg.time + VIEWYAW_LOOKAT_TIME;
			if (cg.time < viewlockTime) {
				cg.refdefViewAngles[PITCH] = tmpvec[PITCH];
				frac = (float)(viewlockTime - cg.time) / VIEWYAW_LOOKAT_TIME;
				frac = sin(M_PI * 0.5 * frac); // dampout
				yaw = ((tmpvec[YAW] - cg.refdefViewAngles[YAW] > 180.0f) || (tmpvec[YAW] - cg.refdefViewAngles[YAW] < -180.0f))? cg.refdefViewAngles[YAW] + 360.0f : cg.refdefViewAngles[YAW];
				yaw = ((frac * yaw) + ((1.0 - frac) * tmpvec[YAW]));
				if (yaw >= 360.0f) yaw -= 360.0f;
				cg.refdefViewAngles[YAW] = yaw;
				viewyaw = yaw;
			} else {
				VectorCopy( tmpvec, cg.refdefViewAngles );
				viewyaw = tmpvec[YAW];
			}
			viewunlockTime = 0;	// cancel any unlock transition..
			break;

		case 0:	// transition view locked -> unlock.. (look away from medic/killer)
			if ( !(cg.snap->ps.pm_flags & PMF_LIMBO) && cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
				// start the unlock transition..
				if (viewunlockTime == 0) viewunlockTime = cg.time + VIEWYAW_LOOKAWAY_TIME;
				if (cg.time < viewunlockTime) {
					tmpvec[YAW] = SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);
					frac = (float)(viewunlockTime - cg.time) / VIEWYAW_LOOKAWAY_TIME;
					yaw = ((tmpvec[YAW] - viewyaw > 180.0f) || (tmpvec[YAW] - viewyaw < -180.0f))? tmpvec[YAW] + 360.0f : tmpvec[YAW];
					yaw = ((frac * viewyaw) + ((1.0 - frac) * yaw));
					if (yaw >= 360.0f) yaw -= 360.0f;
					cg.refdefViewAngles[YAW] = yaw;
				} else {
					cg.refdefViewAngles[YAW] = SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);
				}
			}
			viewlockTime = 0;	// cancel any lock transition..
			break;
	}
}



/*
===============
CG_OffsetThirdPersonView

===============
*/
#define	FOCUS_DISTANCE	400	//800	//512

void CG_OffsetThirdPersonView( void ) {
	vec3_t			forward, right, up;
	vec3_t			view;
	vec3_t			focusAngles;
	trace_t			trace;
	static vec3_t	mins = {-4,-4,-4 };
	static vec3_t	maxs = { 4, 4, 4 };
	vec3_t			focusPoint;
	float			focusDist;
	float			forwardScale, sideScale;
	float			s;
	cg.refdef_current->vieworg[2] += cg.predictedPlayerState.viewheight;

	VectorCopy( cg.refdefViewAngles, focusAngles );

	// rain - if dead, look at medic or allow freelook if none in range
	if( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		// rain - #254 - force yaw to 0 if we're tracking a medic
		if( cg.snap->ps.viewlocked != VIEWLOCK_MEDIC && cg.snap->ps.viewlocked != VIEWLOCK_KILLER ) {
			C_ViewlockTransition( 0 );
			focusAngles[YAW] = cg.refdefViewAngles[YAW];
		}
		else {
			cg.refdefViewAngles[YAW] = SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);
		}
	}

	if ( focusAngles[PITCH] > 45 ) {
		focusAngles[PITCH] = 45;		// don't go too far overhead
	}
	AngleVectors( focusAngles, forward, NULL, NULL );

	if( cg_thirdPerson.integer == 2 ) {
		VectorCopy( cg.predictedPlayerState.origin, focusPoint );
	}
	else {
		VectorMA( cg.refdef_current->vieworg, FOCUS_DISTANCE, forward, focusPoint );
	}

	VectorCopy( cg.refdef_current->vieworg, view );

	view[2] += 8;
	cg.refdefViewAngles[PITCH] *= 0.5;

	AngleVectors( cg.refdefViewAngles, forward, right, up );

	forwardScale = cos( DEG2RAD(cg_thirdPersonAngle.value) );
	sideScale = sin( DEG2RAD(cg_thirdPersonAngle.value) );
	s = -cg_thirdPersonRange.value * forwardScale;
	VectorMA( view, s, forward, view );
	s = -cg_thirdPersonRange.value * sideScale;
	VectorMA( view, s, right, view );

	// trace a ray from the origin to the viewpoint to make sure the view isn't
	// in a solid block.  Use an 8 by 8 block to prevent the view from near clipping anything

	CG_Trace( &trace, cg.refdef_current->vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );

	if ( trace.fraction != 1.0 ) {
		VectorCopy( trace.endpos, view );
		view[2] += (1.0 - trace.fraction) * 32;
		// try another trace to this position, because a tunnel may have the ceiling
		// close enogh that this is poking out

		CG_Trace( &trace, cg.refdef_current->vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
		VectorCopy( trace.endpos, view );
	}


	VectorCopy( view, cg.refdef_current->vieworg );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, cg.refdef_current->vieworg, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );
	if ( focusDist < 1 ) {
		focusDist = 1;	// should never happen
	}
	cg.refdefViewAngles[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );
	cg.refdefViewAngles[YAW] -= cg_thirdPersonAngle.value;
}

// this causes a compiler bug on mac MrC compiler
static void CG_StepOffset( void ) {
	// smooth out stair climbing
	int		timeDelta = cg.time - cg.stepTime;

	// Ridah
	if (timeDelta < 0) {
		cg.stepTime = cg.time;
	}
	if ( timeDelta < STEP_TIME ) {
		cg.refdef_current->vieworg[2] -= cg.stepChange * (STEP_TIME - timeDelta) / STEP_TIME;
	}
}

/*
================
CG_KickAngles
================
*/
#define RECOIL_CENTER_SPEED 200.f
#define RECOIL_IGNORE_CUT_OFF 15.f
#define RECOIL_MAX_SPEED 50.f

void CG_KickAngles(void) {
	static vec3_t centerSpeed = {2400, 2400, 2400};
	static vec3_t maxKickAngles = {10,10,10};
	float idealCenterSpeed, kickChange;
	int i, frametime, t;
	float	ft;
	#define STEP 20
	char buf[32];				// NERVE - SMF

	// this code is frametime-dependant, so split it up into small chunks
	//cg.kickAngles[PITCH] = 0;
	cg.recoilPitchAngle = 0;
	for (t=cg.frametime; t>0; t-=STEP) {
		if (t > STEP) {
			frametime = STEP;
		}
		else {
			frametime = t;
		}

		ft = ((float)frametime/SECONDS_1_F);

		// kickAngles is spring-centered
		for (i=0; i<3; ++i) {
			if (cg.kickAVel[i] || cg.kickAngles[i]) {
				// apply centering forces to kickAvel
				if (cg.kickAngles[i] && frametime) {
					idealCenterSpeed = -(2.0*(cg.kickAngles[i] > 0) - 1.0) * centerSpeed[i];
					if (idealCenterSpeed) {
						cg.kickAVel[i] += idealCenterSpeed * ft;
					}
				}
				// add the kickAVel to the kickAngles
				kickChange = cg.kickAVel[i] * ft;
				if (cg.kickAngles[i] && (cg.kickAngles[i] < 0) != (kickChange < 0))	// slower when returning to center
					kickChange *= 0.06;
				// check for crossing back over the center point
				if (!cg.kickAngles[i] || ((cg.kickAngles[i] + kickChange) < 0) == (cg.kickAngles[i] < 0)) {
					cg.kickAngles[i] += kickChange;
					if (!cg.kickAngles[i] && frametime) {
						cg.kickAVel[i] = 0;
					}
					else if (fabs(cg.kickAngles[i]) > maxKickAngles[i]) {
						cg.kickAngles[i] = maxKickAngles[i] * ((2*(cg.kickAngles[i]>0))-1);
						cg.kickAVel[i] = 0;	// force Avel to return us to center rather than keep going outside range
					}
				}
				else { // about to cross, so just zero it out
					cg.kickAngles[i] = 0;
					cg.kickAVel[i] = 0;
				}
			}
		}

		// recoil is added to input viewangles per frame
		if (cg.recoilPitch) {
			// apply max recoil
			if (fabs(cg.recoilPitch) > RECOIL_MAX_SPEED) {
				if (cg.recoilPitch > 0)
					cg.recoilPitch = RECOIL_MAX_SPEED;
				else
					cg.recoilPitch = -(RECOIL_MAX_SPEED);
			}
			// apply centering forces to kickAvel
			if (frametime) {
				idealCenterSpeed = -(2.0*(cg.recoilPitch > 0) - 1.0) * RECOIL_CENTER_SPEED * ft;
				if (idealCenterSpeed) {
					if (fabs(idealCenterSpeed) < fabs(cg.recoilPitch)) {
						cg.recoilPitch += idealCenterSpeed;
					}
					else {	// back zero out
						cg.recoilPitch = 0;
					}
				}
			}
		}
		if (fabs(cg.recoilPitch) > RECOIL_IGNORE_CUT_OFF) {
			cg.recoilPitchAngle += cg.recoilPitch*ft;
		}
	}

	// NERVE - SMF - only change cg_recoilPitch cvar when we need to
	trap_Cvar_VariableStringBuffer( "cg_recoilPitch", buf, sizeof( buf ) );

	if ( atof( buf ) != cg.recoilPitchAngle ) {
		// encode the kick angles into a 24bit number, for sending to the client exe
		trap_Cvar_Set( "cg_recoilPitch", va("%f", cg.recoilPitchAngle) );
	}
}

/*
==============
CG_ZoomSway
	sway for scoped weapons.
	this takes aimspread into account so the view settles after a bit
==============
*/
static void CG_ZoomSway( void ) {
	float spreadfrac;
	float phase;
	float pitchMinAmp, yawMinAmp;

	if(!cg.zoomval)	// not zoomed
		return;

	if( cg.snap->ps.eFlags & EF_MG42_ACTIVE ) {	// don't draw when on mg_42
		return;
	}

	// remove this to have fg42 bug again
	if( cg.snap->ps.weapon == WP_FG42SCOPE || cg.snap->ps.weapon == WP_JOHNSON_SCOPE ) {
		pitchMinAmp = 4*ZOOM_PITCH_MIN_AMPLITUDE;
		yawMinAmp	= 4*ZOOM_YAW_MIN_AMPLITUDE;
	}
	else {
		pitchMinAmp = ZOOM_PITCH_MIN_AMPLITUDE;
		yawMinAmp	= ZOOM_YAW_MIN_AMPLITUDE;
	}


	spreadfrac = (float)cg.snap->ps.aimSpreadScale / 255.0;

	phase = cg.time / SECONDS_1_F * ZOOM_PITCH_FREQUENCY * M_PI * 2;
	cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * (spreadfrac+pitchMinAmp);

	phase = cg.time / SECONDS_1_F * ZOOM_YAW_FREQUENCY * M_PI * 2;
	cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * (spreadfrac+yawMinAmp);
}

/*
===============
CG_OffsetFirstPersonView

===============
*/
static void CG_OffsetFirstPersonView( void ) {
	float			*origin, *angles, bob, delta, speed,f;
	vec3_t			predictedVelocity;
	int				timeDelta;
	qboolean		useLastValidBob = qfalse;

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		return;
	}

	origin = cg.refdef_current->vieworg;
	angles = cg.refdefViewAngles;

	// if dead, fix the angle and don't add any kick
	if ( !(cg.snap->ps.pm_flags & PMF_LIMBO) && cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
		angles[ROLL] = 40;
		angles[PITCH] = -15;

		// rain - #254 - force yaw to 0 if we're tracking a medic
		// rain - medic tracking doesn't seem to happen in this case?
		if ( cg.snap->ps.viewlocked == VIEWLOCK_MEDIC || cg.snap->ps.viewlocked == VIEWLOCK_KILLER ) {
			angles[YAW] = 0;
		}
		else {
			// rain - do short2angle AFTER the network part
			angles[YAW] = SHORT2ANGLE(cg.snap->ps.stats[STAT_DEAD_YAW]);
		}

		origin[2] += cg.predictedPlayerState.viewheight;
		return;
	}
	else if ( cg.snap->ps.eFlags & EF_PLAYDEAD ) {
		// smooth out height change
		timeDelta = cg.time - cg.duckTime;

		if ( timeDelta < 0 ) {	// Ridah
			cg.duckTime = cg.time - 750;
		}
		if ( timeDelta < 750 ) {
			cg.refdef_current->vieworg[2] -= cg.duckChange * (750 - timeDelta) / 750;
		}
		// rotate around
		angles[YAW] += SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);
		return;
	}

	// add angles based on weapon kick
	VectorAdd (angles, cg.kick_angles, angles);

	// RF, add new weapon kick angles
	CG_KickAngles();
	VectorAdd (angles, cg.kickAngles, angles);

	// add angles based on damage kick
	if ( cg.damageTime ) {
		float ratio = cg.time - cg.damageTime;
		if ( ratio < DAMAGE_DEFLECT_TIME ) {
			ratio /= DAMAGE_DEFLECT_TIME;
			angles[PITCH] += ratio * cg.v_dmg_pitch;
			angles[ROLL] += ratio * cg.v_dmg_roll;
		}
		else {
			ratio = 1.0 - ( ratio - DAMAGE_DEFLECT_TIME ) / DAMAGE_RETURN_TIME;
			if ( ratio > 0 ) {
				angles[PITCH] += ratio * cg.v_dmg_pitch;
				angles[ROLL] += ratio * cg.v_dmg_roll;
			}
		}
	}

	// add angles based on velocity
	VectorCopy( cg.predictedPlayerState.velocity, predictedVelocity );

	delta = DotProduct( predictedVelocity, cg.refdef_current->viewaxis[0] );
	angles[PITCH] += delta * cg_runpitch.value;

	delta = DotProduct( predictedVelocity, cg.refdef_current->viewaxis[1] );
	angles[ROLL] -= delta * cg_runroll.value;

	// add angles based on bob

	// make sure the bob is visible even at low speeds
	speed = cg.xyspeed > 200 ? cg.xyspeed : 200;

	if( !cg.bobfracsin && cg.lastvalidBobfracsin > 0 ) {
		// 200 msec to get back to center from 1
		// that's 1/200 per msec = 0.005 per msec
		cg.lastvalidBobfracsin -= 0.005 * cg.frametime;
		useLastValidBob = qtrue;
	}

	delta = useLastValidBob ? cg.lastvalidBobfracsin * cg_bobpitch.value * speed : cg.bobfracsin * cg_bobpitch.value * speed;
	if ( cg.predictedPlayerState.pm_flags & PMF_DUCKED ) {
		delta *= 3;		// crouching
	}
	angles[PITCH] += delta;
	delta = useLastValidBob ? cg.lastvalidBobfracsin * cg_bobroll.value * speed : cg.bobfracsin * cg_bobroll.value * speed;
	if ( cg.predictedPlayerState.pm_flags & PMF_DUCKED ) {
		delta *= 3;		// crouching accentuates roll
	}
	if ( useLastValidBob ) {
		if( cg.lastvalidBobcycle & 1 ) {
			delta = -delta;
		}
	}
	else if ( cg.bobcycle & 1 ) {
		delta = -delta;
	}
	angles[ROLL] += delta;

//===================================

	// add view height
	origin[2] += cg.predictedPlayerState.viewheight;

	// smooth out duck height changes
	timeDelta = cg.time - cg.duckTime;
	if ( cg.predictedPlayerState.eFlags & EF_PRONE ) {
		if ( timeDelta < 0 ) {	// Ridah
			cg.duckTime = cg.time - PRONE_TIME;
		}
		if ( timeDelta < PRONE_TIME ) {
			cg.refdef_current->vieworg[2] -= cg.duckChange * (PRONE_TIME - timeDelta) / PRONE_TIME;
		}
		// need to slower raising speed, it was same as for crouch before..
	}
	else if ( cg.time + cg.pmext.proneTime < cg.pmext.proneRaiseTime && cgs.realism & REALISM_EXTENDED_PRONE ) {
		if ( timeDelta < 0 ) {	// Ridah
			cg.duckTime = cg.time - cg.pmext.proneRaiseTime;
		}
		if ( timeDelta < cg.pmext.proneRaiseTime ) {
			cg.refdef_current->vieworg[2] -= cg.duckChange * (cg.pmext.proneRaiseTime - timeDelta) / cg.pmext.proneRaiseTime;
		}
	}
	else if ( cg.duckPlaydead ) {
		if ( timeDelta < 0 ) {	// Ridah
			cg.duckTime = cg.time - 750;
		}
		if ( timeDelta < 750 ) {
			cg.refdef_current->vieworg[2] -= cg.duckChange * (750 - timeDelta) / 750;
		}
	}
	else {
		if ( timeDelta < 0 ) {	// Ridah
			cg.duckTime = cg.time - DUCK_TIME;
		}
		if ( timeDelta < DUCK_TIME ) {
			cg.refdef_current->vieworg[2] -= cg.duckChange * (DUCK_TIME - timeDelta) / DUCK_TIME;
		}
	}

	// add bob height
	bob = cg.bobfracsin * cg.xyspeed * cg_bobup.value;
	if (bob > 6) {
		bob = 6;
	}

	// kw: exploit fix // IRATA: Added from ETPub
	if( bob < 0 ) {
		bob = 0;
	}

	origin[2] += bob;

	// add fall height
	delta = cg.time - cg.landTime;
	if ( delta < 0 ) { // Ridah
		cg.landTime = cg.time - (LAND_DEFLECT_TIME + LAND_RETURN_TIME);
	}
	if ( delta < LAND_DEFLECT_TIME ) {
		f = delta / LAND_DEFLECT_TIME;
		cg.refdef_current->vieworg[2] += cg.landChange * f;
	}
	else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		delta -= LAND_DEFLECT_TIME;
		f = 1.0 - ( delta / LAND_RETURN_TIME );
		cg.refdef_current->vieworg[2] += cg.landChange * f;
	}

	// add step offset
	CG_StepOffset();

	CG_ZoomSway();

	// SYNC WITH THE G_WEAPONS.C CODE!!!
	if ( cg.predictedPlayerState.eFlags & EF_POISONED ) {
		float phase = cg.time / SECONDS_1_F * 0.3f * M_PI; // cg.time / 1000.0 * 0.5 * M_PI * 2;

		cg.refdefViewAngles[ROLL] += 36 * sin( phase ); // amplitude * sin  cg.refdefViewAngles[ROLL] += 4 * sin( phase )
		cg.refdefViewAngles[YAW] += 24 * sin( phase ); // amplitude * sin  cg.refdefViewAngles[YAW] += 4 * sin( phase )
		cg.refdefViewAngles[PITCH] += 12 * sin( phase*2.5 ); // amplitude * sin  cg.refdefViewAngles[YAW] += 4 * sin( phase )
	}

	// adjust for 'lean'
	if ( cg.predictedPlayerState.leanf != 0 ) {
		float leanf = cg.predictedPlayerState.leanf;
		vec3_t	right;

		//add leaning offset
		cg.refdefViewAngles[2] += leanf/2.0f;
		AngleVectors(cg.refdefViewAngles, NULL, right, NULL);
		VectorMA(cg.refdef_current->vieworg, cg.predictedPlayerState.leanf, right, cg.refdef_current->vieworg);
	}

	// add kick offset
	VectorAdd (origin, cg.kick_origin, origin);
}
//======================================================================

//
// Zoom controls
//

// probably move to server variables
float zoomTable[ZOOM_MAX_ZOOMS][2] = {
// max {out,in}
	{0, 0},

	{36, 8},	//	binoc
	{20, 4},	//	sniper
	{60, 20},	//	snooper
	{55, 55},	//	fg42 & johnson
	{55, 55}	//	mg42
};

void CG_AdjustZoomVal(float val, int type) {
	cg.zoomval += val;
	if(cg.zoomval > zoomTable[type][ZOOM_OUT]) cg.zoomval = zoomTable[type][ZOOM_OUT];
	if(cg.zoomval < zoomTable[type][ZOOM_IN]) cg.zoomval = zoomTable[type][ZOOM_IN];
}

void CG_ZoomIn_f( void ) {
	// Gordon: fixed being able to "latch" your zoom by weaponcheck + quick zoomin
	// OSP - change for zoom view in demos

	if( cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_GARAND_SCOPE ) {
		CG_AdjustZoomVal(-(cg_zoomStepSniper.value), ZOOM_SNIPER);
	}
	else if( cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_K43_SCOPE) {
		CG_AdjustZoomVal(-(cg_zoomStepSniper.value), ZOOM_SNIPER);
	}
	else if(cg.zoomedBinoc) {
		CG_AdjustZoomVal(-(cg_zoomStepSniper.value), ZOOM_SNIPER); // JPW NERVE per atvi request all use same vals to match menu (was zoomStepBinoc, ZOOM_BINOC);
	}
}

void CG_ZoomOut_f( void ) {
	if( cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_GARAND_SCOPE ) {
		CG_AdjustZoomVal(cg_zoomStepSniper.value, ZOOM_SNIPER);
	}
	else if( cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_K43_SCOPE ) {
		CG_AdjustZoomVal(cg_zoomStepSniper.value, ZOOM_SNIPER);
	}
	else if(cg.zoomedBinoc) {
		CG_AdjustZoomVal(cg_zoomStepSniper.value, ZOOM_SNIPER); // JPW NERVE per atvi request BINOC);
	}
}

/*
==============
CG_Zoom
==============
*/
void CG_Zoom( void ) {
	// OSP - Fix for demo playback
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) {
		cg.predictedPlayerState.eFlags = cg.snap->ps.eFlags;
		cg.predictedPlayerState.weapon = cg.snap->ps.weapon;

		// check for scope wepon in use, and switch to if necessary
		// OSP - spec/demo scaling allowances
		switch( cg.predictedPlayerState.weapon ) {
			case WP_FG42SCOPE:
			case WP_JOHNSON_SCOPE:
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
				cg.zoomval = (cg.zoomval == 0) ? cg_zoomDefaultSniper.value : cg.zoomval; // JPW NERVE was DefaultFG, changed per atvi req
				break;
			default:
				// core: show a zoomed binoculars view for spectators.. (still not actively zooming in/out)
				if ( cg.predictedPlayerState.eFlags & EF_ZOOMING ) {
					cg.zoomval = (cg.zoomval == 0) ? cg_zoomDefaultSniper.value : cg.zoomval;
				} else {
					cg.zoomval = 0;
				}
				break;
		}

	}
	if(cg.predictedPlayerState.eFlags & EF_ZOOMING) {
		if ( cg.zoomedBinoc ) return;
		cg.zoomedBinoc	= qtrue;
		cg.zoomTime	= cg.time;
		cg.zoomval = cg_zoomDefaultSniper.value; // JPW NERVE was DefaultBinoc, changed per atvi req
	}
	else {
		if (cg.zoomedBinoc) {
			cg.zoomedBinoc	= qfalse;
			cg.zoomTime		= cg.time;

			switch( cg.weaponSelect ) {
				case WP_FG42SCOPE:
				case WP_JOHNSON_SCOPE:
				case WP_GARAND_SCOPE:
				case WP_K43_SCOPE:
					cg.zoomval = cg_zoomDefaultSniper.value; // JPW NERVE was DefaultFG, changed per atvi req
					break;
				default:
					cg.zoomval = 0;
					break;
			}

		}
		else {
			//bani - we now sanity check to make sure we can't zoom non-zoomable weapons
			//zinx - fix for #423 - don't sanity check while following
			if (!((cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback)) {
				switch( cg.weaponSelect ) {
					case WP_FG42SCOPE:
					case WP_JOHNSON_SCOPE:
					case WP_GARAND_SCOPE:
					case WP_K43_SCOPE:
						break;
					default:
						cg.zoomval = 0;
						break;
				}
			}
		}
	}
}

/*
====================
CG_CalcFov

Fixed fov at intermissions, otherwise account for fov variable and zooms.
====================
*/
#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

// core: calculate the proper Field-Of-View for the vertical angle..
// here are two equations for retrieving the values:
//   fovy = atan2(screenwidth, screenheight/tan(fovx));
//   fovx = (screenwidth/screenheight) * fovy;
float C_CalcFovy( float fovx ) {
	float x = cg.refdef_current->width / tan( DEG2RAD(fovx) );
	return RAD2DEG( atan2(cg.refdef_current->height, x) );
}

static int CG_CalcFov( void ) {
	static float lastfov = 90;		// for transitions back from zoomed in modes
	float	phase = 0, v = 0, fov_x, fov_y;
	int		contents, inwater=qfalse;

	CG_Zoom();

	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 && !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
		cg.zoomedBinoc = qfalse;
		cg.zoomTime = 0;
		cg.zoomval = 0;
	}

	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		// if in intermission, use a fixed value
		fov_x = 90;
		fov_y = 73.74f;
	}
	else if( cg.showGameView ) {
		fov_x = fov_y = 60.f;
	}
	// Arnout: mg42 zoom
	else if (cg.snap->ps.persistant[PERS_HWEAPON_USE]) {
		fov_x = 55;
		fov_y = 32.64f;
	}
	else if( cg.snap->ps.weapon == WP_MOBILE_MG42_SET || cg.snap->ps.weapon == WP_MOBILE_BROWNING_SET || cg.snap->ps.weapon == WP_BAR_SET) {
		fov_x = 55;
		fov_y = 32.64f;
	}
	else if( cg.snap->ps.eFlags & EF_MOUNTEDTANK ) {
		fov_x = 75;
		fov_y = 46.69f;
	}
	else {
		fov_x = cg_fov.value;
		if( !developer.integer ) {
			// core: changed the lower limit for fov_x to meet the requirements for selecting a 5:4 monitor aspectratio..
			// 90 degrees was not low enough since the 5:4 ratio will result in an fov_x of 86.3 degrees.
			if ( fov_x < 85 ) {
				fov_x = 85;
			}
			else if ( fov_x > 160 ) {
				fov_x = 160;
			}
		}

		fov_y = cg_fovy.value;
		if( !developer.integer ) {
			if (fov_y < 60.0f) {
				fov_y = 60.0f;
			}
			else if (fov_y > 110.0f) {
				fov_y = 110.0f;
			}
		}

		if( !cg.renderingThirdPerson || developer.integer ) {
			float zoomFov, f;

			// account for zooms
			if(cg.zoomval) {
				zoomFov = cg.zoomval;	// (SA) use user scrolled amount

				if ( zoomFov < 1 ) {
					zoomFov = 1;
				}
				else if ( zoomFov > 160 ) {
					zoomFov = 160;
				}
			}
			else {
					zoomFov = lastfov;
			}

			// do smooth transitions for the binocs
			if(cg.zoomedBinoc) {		// binoc zooming in
				f = ( cg.time - cg.zoomTime ) / (float)ZOOM_TIME;
				if ( f > 1.0 ) {
					fov_x = zoomFov;
					fov_y = C_CalcFovy(fov_x);
				}
				else {
					fov_x = fov_x + f * ( zoomFov - fov_x );
					fov_y = C_CalcFovy(fov_x);
					fov_y = (1.0f-f)*cg_fovy.value + f*fov_y;
				}
				lastfov = fov_x;
			}
			else if (cg.zoomval) {	// zoomed by sniper/snooper
				fov_x = cg.zoomval;
				lastfov = fov_x;
				fov_y = C_CalcFovy(fov_x);
			}
			else {					// binoc zooming out
				f = ( cg.time - cg.zoomTime ) / (float)ZOOM_TIME;
				if ( f > 1.0 ) {
					fov_x = fov_x;	// ..what is this?
					fov_y = cg_fovy.value;
				}
				else {
					fov_x = zoomFov + f * ( fov_x - zoomFov);
					fov_y = C_CalcFovy(fov_x);
					fov_y = f*cg_fovy.value + (1.0f-f)*fov_y;
				}
			}
		}
	}

	cg.refdef_current->rdflags &= ~RDF_SNOOPERVIEW;

	// warp if underwater
	contents = CG_PointContents( cg.refdef.vieworg, -1 );

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA )) {
		phase = cg.time / SECONDS_1_F * WAVE_FREQUENCY * M_PI * 2;
		v = WAVE_AMPLITUDE * sin( phase );
		fov_x += v;
		fov_y -= v;
		inwater = qtrue;
		cg.refdef_current->rdflags |= RDF_UNDERWATER;
	}
	else {
		cg.refdef_current->rdflags &= ~RDF_UNDERWATER;
		inwater = qfalse;
	}

	if (  cg.predictedPlayerState.eFlags & EF_POISONED ) {
		phase = cg.time / SECONDS_1_F * 0.3 * M_PI * 2;
		v = 12 * sin( phase );	//v = 2 * sin( phase );
		fov_x += v;
		fov_y -= v;
		cg.refdef_current->rdflags |= RDF_UNDERWATER;

		inwater = qtrue;
	}

	// set it
	cg.refdef_current->fov_x = fov_x;
	cg.refdef_current->fov_y = fov_y;

	// rain - allow freelook when dead until we tap out into limbo
	if( cg.snap->ps.pm_type == PM_FREEZE || (cg.snap->ps.pm_type == PM_DEAD && (cg.snap->ps.pm_flags & PMF_LIMBO)) || cg.snap->ps.pm_flags & PMF_TIME_LOCKPLAYER ) {
		// No movement for pauses
		cg.zoomSensitivity = 0;
	}
	else if ( !cg.zoomedBinoc ) {
		// NERVE - SMF - fix for zoomed in/out movement bug
		if ( cg.zoomval ) {
			cg.zoomSensitivity = 0.6f * ( cg.zoomval / 90.f );	// NERVE - SMF - changed to get less sensitive as you zoom in
		}
		else {
			cg.zoomSensitivity = 1;
		}
		// -NERVE - SMF
	}
	else {
		cg.zoomSensitivity = cg.refdef_current->fov_y / 75.0f;
	}

	return inwater;
}

/*
==============
CG_UnderwaterSounds
==============
*/
#define UNDERWATER_BIT 16
static void CG_UnderwaterSounds( void ) {
	trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.underWaterSound, 255 | (1<<UNDERWATER_BIT), 0 );
}

/*
===============
CG_DamageBlendBlob

===============
*/
static void CG_DamageBlendBlob( void ) {

	// Gordon: no damage blend blobs if in limbo or spectator, and in the limbo menu
	if( (cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) && cg.showGameView ) {
		return;
	}

	// ragePro systems can't fade blends, so don't obscure the screen
	if( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		return;
	}

	/*
	if (!cg_blood.integer) {
		return;
	}
	*/

	{
		int			t, i;
		int			maxTime;
		refEntity_t	ent;
		qboolean	pointDamage;
		viewDamage_t *vd;
		float		redFlash = 0;
		float		s;

		for (i=0; i<MAX_VIEWDAMAGE; ++i) {
			vd = &cg.viewDamage[i];

			if ( !vd->damageValue ) {
				continue;
			}

			maxTime = vd->damageDuration;
			t = cg.time - vd->damageTime;
			if ( t <= 0 || t >= maxTime ) {
				vd->damageValue = 0;
				continue;
			}

			pointDamage = !(!vd->damageX && !vd->damageY);

			// if not point Damage, only do flash blend
			if (!pointDamage) {
				redFlash += 10.0 * (1.0 - (float)t/maxTime);
				continue;
			}

			memset( &ent, 0, sizeof( ent ) );
			ent.reType = RT_SPRITE;
			ent.renderfx = RF_FIRST_PERSON;

			VectorMA( cg.refdef_current->vieworg, 8, cg.refdef_current->viewaxis[0], ent.origin );
			s = vd->damageX * -8;
			VectorMA( ent.origin, s, cg.refdef_current->viewaxis[1], ent.origin );
			s = vd->damageY * 8;
			VectorMA( ent.origin, s, cg.refdef_current->viewaxis[2], ent.origin );

			ent.radius = vd->damageValue * 0.4 * (0.5 + 0.5*(float)t/maxTime) * (0.75 + 0.5 * fabs(sin(vd->damageTime)));

			ent.customShader = cgs.media.viewBloodAni[(int)(floor(((float)t / maxTime)*4.9))];//cgs.media.viewBloodShader;
			ent.shaderRGBA[0] = 255;
			ent.shaderRGBA[1] = 255;
			ent.shaderRGBA[2] = 255;
			ent.shaderRGBA[3] = 255 * ((cg_bloodDamageBlend.value > 1.0f) ? 1.0f :
									   (cg_bloodDamageBlend.value < 0.0f) ? 0.0f : cg_bloodDamageBlend.value);

			trap_R_AddRefEntityToScene( &ent );

			redFlash += ent.radius;
		}
	}
}


/*
===============
CG_CalcViewValues

Sets cg.refdef view values
===============
*/
int CG_CalcViewValues( void ) {
	playerState_t	*ps;

	memset( cg.refdef_current, 0, sizeof( cg.refdef ) );

	// calculate size of 3D view
	CG_CalcVrect();

	ps = &cg.predictedPlayerState;

	// intermission view
	if ( ps->pm_type == PM_INTERMISSION ) {
		VectorCopy( ps->origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
		AnglesToAxis( cg.refdefViewAngles, cg.refdef_current->viewaxis );
		return CG_CalcFov();
	}

	if( cg.bobfracsin > 0 && !ps->bobCycle ) {
		cg.lastvalidBobcycle = cg.bobcycle;
		cg.lastvalidBobfracsin = cg.bobfracsin;
	}

	cg.bobcycle = ( ps->bobCycle & 128 ) >> 7;
	cg.bobfracsin = fabs( sin( ( ps->bobCycle & 127 ) / 127.0 * M_PI ) );
	cg.xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1] );


	if( cg.showGameView ) {
		VectorCopy( cgs.ccPortalPos, cg.refdef_current->vieworg );
		if( cg.showGameView && cgs.ccPortalEnt != -1 ) {
			vec3_t vec;
			VectorSubtract( cg_entities[cgs.ccPortalEnt].lerpOrigin, cg.refdef_current->vieworg, vec );
			vectoangles( vec, cg.refdefViewAngles );
		}
		else {
			VectorCopy( cgs.ccPortalAngles, cg.refdefViewAngles );
		}
	}
	else if( cg.renderingThirdPerson && ps->eFlags & EF_MG42_ACTIVE ) { // Arnout: see if we're attached to a gun
		centity_t *mg42 = &cg_entities[ps->viewlocked_entNum];
		vec3_t	forward;

		AngleVectors ( ps->viewangles, forward, NULL, NULL );
		VectorMA ( mg42->currentState.pos.trBase, -36, forward, cg.refdef_current->vieworg );
		cg.refdef_current->vieworg[2] = ps->origin[2];
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}
	else if( ps->eFlags & EF_MOUNTEDTANK ) {
		centity_t *tank = &cg_entities[cg_entities[cg.snap->ps.clientNum].tagParent];

		VectorCopy( tank->mountedMG42Player.origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}
	else {
		VectorCopy( ps->origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}

	if( !cg.showGameView ) {
		// add error decay
		if( cg_errorDecay.value > 0 ) {
			int		t = cg.time - cg.predictedErrorTime;
			float	f, errorDecay = cg_errorDecay.value;

			// kw: really make sure errordecay isn't exploitable
			if(errorDecay > 500) {
				errorDecay = 500;
			}

			// IRATA: deny the exploit for real ... see CG_PredictPlayerState
			f = ( errorDecay - t ) / errorDecay;

			if ( f > 0 && f < 1 ) {
				VectorMA( cg.refdef_current->vieworg, f, cg.predictedError, cg.refdef_current->vieworg );
			}
			else {
				cg.predictedErrorTime = 0;
			}
		}

		// Ridah, lock the viewangles if the game has told us to
		if( ps->viewlocked ) {

			// DHM - Nerve :: don't bother evaluating if set to 7 (look at medic)
			if( ps->viewlocked != VIEWLOCK_MEDIC && ps->viewlocked != VIEWLOCK_KILLER && ps->viewlocked != VIEWLOCK_MG42 && ps->viewlocked != VIEWLOCK_JITTER ) {
				BG_EvaluateTrajectory( &cg_entities[ps->viewlocked_entNum].currentState.apos, cg.time, cg.refdefViewAngles, qtrue, cg_entities[ps->viewlocked_entNum].currentState.effect2Time );
			}

			if( ps->viewlocked == VIEWLOCK_JITTER ) {
				cg.refdefViewAngles[0] += crandom();
				cg.refdefViewAngles[1] += crandom();
			}
		}

		if ( cg.renderingThirdPerson ) {
			// back away from character
			CG_OffsetThirdPersonView();
		}
		else {

			// offset for local bobbing and kicks
			CG_OffsetFirstPersonView();

			if( cg.editingSpeakers ) {
				CG_SetViewanglesForSpeakerEditor();
			}
		}

		// Ridah, lock the viewangles if the game has told us to
		if (ps->viewlocked == VIEWLOCK_MEDIC || ps->viewlocked == VIEWLOCK_KILLER) {
			C_ViewlockTransition( 1 );
		}
		else if (ps->viewlocked == 4) {
			vec3_t fwd;

			AngleVectors( cg.refdefViewAngles, fwd, NULL, NULL );
			VectorMA( cg_entities[ps->viewlocked_entNum].lerpOrigin, 16, fwd, cg.refdef_current->vieworg );
		}
		else if (ps->viewlocked && !(cg.renderingThirdPerson) ) {
			vec3_t fwd;
			// set our position to be behind it
			float oldZ = cg.refdef_current->vieworg[2];

			AngleVectors( cg.refdefViewAngles, fwd, NULL, NULL );
			VectorMA( cg_entities[ps->viewlocked_entNum].lerpOrigin, -34, fwd, cg.refdef_current->vieworg );
			cg.refdef_current->vieworg[2] = oldZ;
		}
		// done.
	}

	// position eye reletive to origin
	AnglesToAxis( cg.refdefViewAngles, cg.refdef_current->viewaxis );

	if ( cg.hyperspace ) {
		cg.refdef.rdflags |= RDF_NOWORLDMODEL | RDF_HYPERSPACE;
	}

	// field of view
	return CG_CalcFov();
}

//=========================================================================

char* CG_MustParse( char** pString, const char* pErrorMsg ) {
	char* token = COM_Parse( pString );
	if(!*token) {
		CG_Error( pErrorMsg );
	}
	return token;
}

void CG_ParseSkyBox( void ) {
	char *cstr = (char*)CG_ConfigString(CS_SKYBOXORG), *token;

	if (!*cstr) {
		cg.skyboxEnabled = qfalse;
		return;
	}

	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[0] = atof(token);

	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[1] = atof(token);

	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[2] = atof(token);

	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewFov = atoi(token);

	if (!cg.skyboxViewFov) {
		cg.skyboxViewFov = 90;
	}

	// setup fog the first time, ignore this part of the configstring after that
	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring.  No fog state\n" );
	if(atoi(token)) {	// this camera has fog
		int fogStart, fogEnd;
		vec4_t fogColor;

		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[0]\n" );
		fogColor[0] = atof(token);

		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[1]\n" );
		fogColor[1] = atof(token);

		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[2]\n" );
		fogColor[2] = atof(token);

		token = COM_ParseExt(&cstr, qfalse);
			fogStart = atoi(token);

		token = COM_ParseExt(&cstr, qfalse);
			fogEnd = atoi(token);

		trap_R_SetFog(FOG_PORTALVIEW, fogStart, fogEnd, fogColor[0], fogColor[1], fogColor[2], 1.1f);
	}
	else {
		trap_R_SetFog(FOG_PORTALVIEW, 0, 0, 0, 0, 0, 0);	// init to null
	}

	cg.skyboxEnabled = qtrue;
}

/*
==============
CG_ParseTagConnects
==============
*/
void CG_ParseTagConnects( void ) {
	int i;

	for( i = CS_TAGCONNECTS; i < CS_TAGCONNECTS + MAX_TAGCONNECTS; ++i ) {
		CG_ParseTagConnect( i );
	}
}

void CG_ParseTagConnect( int tagNum ) {
	char *token, *pString = (char*)CG_ConfigString( tagNum ); // Gordon: bleh, i hate that cast away of the const
	int entNum;

	if(!*pString) {
		return;
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );

	entNum = atoi(token);
	if(entNum < 0 || entNum >= MAX_GENTITIES) {
		CG_Error( "Invalid TAGCONNECT entitynum\n" );
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );

	cg_entities[entNum].tagParent = atoi(token);
	if(cg_entities[entNum].tagParent < 0 || cg_entities[entNum].tagParent >= MAX_GENTITIES) {
		CG_Error( "Invalid TAGCONNECT tagparent\n" );
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );
	Q_strncpyz( cg_entities[entNum].tagName, token, MAX_QPATH );
}

/*
==============
CG_DrawSkyBoxPortal
==============
*/
void CG_DrawSkyBoxPortal(qboolean fLocalView) {
	refdef_t rd;
	static float lastfov = 90;		// for transitions back from zoomed in modes

	if(!cg_skybox.integer || !cg.skyboxEnabled) {
		return;
	}

	memcpy( &rd, cg.refdef_current, sizeof(refdef_t) );
	VectorCopy( cg.skyboxViewOrg, rd.vieworg );

	if(fLocalView) {
		float	fov_x;
		float	fov_y;
		float	x;

		if(cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
			// if in intermission, use a fixed value
			fov_x = 90;
		}
		else {
			float	f;
			float	zoomFov;

			// user selectable
			fov_x = cg_fov.value;
			if(fov_x < 1) {
				fov_x = 1;
			}
			else if(fov_x > 160) {
				fov_x = 160;
			}

			// account for zooms
			if(cg.zoomval) {
				zoomFov = cg.zoomval;	// (SA) use user scrolled amount
				if(zoomFov < 1) zoomFov = 1;
				else if(zoomFov > 160) zoomFov = 160;
			}
			else {
				zoomFov = lastfov;
			}

			// do smooth transitions for the binocs
			if(cg.zoomedBinoc) {		// binoc zooming in
				f = (cg.time - cg.zoomTime) / (float)ZOOM_TIME;
				fov_x = (f > 1.0) ? zoomFov : fov_x + f * (zoomFov - fov_x);
				lastfov = fov_x;
			}
			else if(cg.zoomval) {	// zoomed by sniper/snooper
				fov_x = cg.zoomval;
				lastfov = fov_x;
			}
			else {					// binoc zooming out
				f = (cg.time - cg.zoomTime) / (float)ZOOM_TIME;
				fov_x = (f > 1.0) ? fov_x : zoomFov + f * (fov_x - zoomFov);
			}
		}

		rd.rdflags &= ~RDF_SNOOPERVIEW;

		if( BG_PlayerMounted( cg.snap->ps.eFlags ) ||
			/*cg.predictedPlayerState.weapon == WP_JOHNSON_SCOPE || */
			cg.predictedPlayerState.weapon == WP_MOBILE_MG42_SET ||
			cg.predictedPlayerState.weapon == WP_MOBILE_BROWNING_SET ||
			cg.predictedPlayerState.weapon == WP_BAR_SET)
		{
			fov_x = 55;
		}

		x = rd.width / tan(fov_x / 360 * M_PI);
		fov_y = atan2(rd.height, x);
		fov_y = fov_y * 360 / M_PI;

		rd.fov_x = fov_x;
		rd.fov_y = fov_y;
	}

	rd.time = cg.time;
	rd.rdflags |= RDF_SKYBOXPORTAL;

	// draw the skybox
	trap_R_RenderScene(&rd);
}

//=========================================================================

/*
 *  Frustum code
 */

// some culling bits
typedef struct plane_s {
	vec3_t	normal;
	float	dist;
} plane_t;

static plane_t frustum[4];

/*
==============
CG_SetupFrustum
==============
*/
void CG_SetupFrustum( void ) {

	float	ang = DEG2RAD(cg.refdef_current->fov_x) * 0.5f;
	float	xs, xc;
	SinCos( ang, xs,xc );

	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[0].normal );
	VectorMA( frustum[0].normal, xc, cg.refdef_current->viewaxis[1], frustum[0].normal );

	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[1].normal );
	VectorMA( frustum[1].normal, -xc, cg.refdef_current->viewaxis[1], frustum[1].normal );

	ang = DEG2RAD(cg.refdef.fov_y) * 0.5f;
	SinCos( ang, xs,xc );

	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[2].normal );
	VectorMA( frustum[2].normal, xc, cg.refdef_current->viewaxis[2], frustum[2].normal );

	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[3].normal );
	VectorMA( frustum[3].normal, -xc, cg.refdef_current->viewaxis[2], frustum[3].normal );

	frustum[0].dist = DotProduct( cg.refdef_current->vieworg, frustum[0].normal);
	frustum[1].dist = DotProduct( cg.refdef_current->vieworg, frustum[1].normal);
	frustum[2].dist = DotProduct( cg.refdef_current->vieworg, frustum[2].normal);
	frustum[3].dist = DotProduct( cg.refdef_current->vieworg, frustum[3].normal);
}

/*
==============
CG_CullPoint - returns true if culled
==============
*/
qboolean CG_CullPoint( vec3_t pt ) {

	if( (DotProduct(pt, frustum[0].normal) - frustum[0].dist) < 0 ) return( qtrue );
	if( (DotProduct(pt, frustum[1].normal) - frustum[1].dist) < 0 ) return( qtrue );
	if( (DotProduct(pt, frustum[2].normal) - frustum[2].dist) < 0 ) return( qtrue );
	if( (DotProduct(pt, frustum[3].normal) - frustum[3].dist) < 0 ) return( qtrue );

	return( qfalse );
}

/*
==============
CG_CullPointAndRadius
==============
*/
qboolean CG_CullPointAndRadius( const vec3_t pt, vec_t radius) {

	if( (DotProduct(pt, frustum[0].normal) - frustum[0].dist) < -radius ) return( qtrue );
	if( (DotProduct(pt, frustum[1].normal) - frustum[1].dist) < -radius ) return( qtrue );
	if( (DotProduct(pt, frustum[2].normal) - frustum[2].dist) < -radius ) return( qtrue );
	if( (DotProduct(pt, frustum[3].normal) - frustum[3].dist) < -radius ) return( qtrue );

	return( qfalse );
}

//=========================================================================

static void CG_RenderLocations( void ) {
	refEntity_t		re;
	location_t		*location;
    int i;

	if ( cgs.numLocations < 1 ) {
		return;
	}

	for(i = 0; i < cgs.numLocations; ++i ) {
		location = &cgs.location[i];

		if (!location )
			break;

		if ( VectorDistance( cg.refdef.vieworg, location->origin ) > 3000 )
			continue;

		if ( !trap_R_inPVS( cg.refdef.vieworg, location->origin ) )
			continue;

		memset( &re, 0, sizeof( re ) );
		re.reType = RT_SPRITE;
		VectorCopy( location->origin, re.origin );
		VectorCopy( location->origin, re.oldorigin );
		re.radius = 8;
    }
}

extern void CG_SetupDlightstyles(void);

#ifdef _DEBUG
//#define FAKELAG
#ifdef FAKELAG
extern int snapshotDelayTime;
#endif // FAKELAG
#endif // _DEBUG

/*
=================
CG_DrawActiveFrame

Generates and draws a game scene and status information at the given time.
=================
*/

void CG_DrawMissileCamera(rectDef_t *rect );
void CG_DrawKillerCamera(rectDef_t *rect );
qboolean CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle );

void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback ) {
	char		currentVal[256], tmp[256];
	float		cvalF, val1F, val2F;
	int			i, cvalI, val1I, val2I;
	qboolean	cvalIsF, val1IsF, val2IsF;

	cg.time = serverTime;
	cgDC.realTime = cg.time;
	cg.demoPlayback = demoPlayback;

#ifdef FAKELAG
	cg.time -= snapshotDelayTime;
#endif // _DEBUG


	for ( i=0; i< cg.forceCvarCount; ++i) {
		trap_Cvar_VariableStringBuffer(cg.forceCvars[i].cvarName, tmp, sizeof(tmp));

		if ( Q_stricmp( cg.forceCvars[i].cvarValue, tmp) ) {
			trap_Cvar_Set(cg.forceCvars[i].cvarName, cg.forceCvars[i].cvarValue);
		}
	}

	for ( i=0; i< cg.svCvarCount; ++i) {
		trap_Cvar_VariableStringBuffer(cg.svCvars[i].cvarName, currentVal, sizeof(currentVal));

		cvalF = atof( currentVal );
		val1F = atof(cg.svCvars[i].Val1);
		val2F = atof(cg.svCvars[i].Val2);
		cvalI = atoi( currentVal );
		val1I = atoi(cg.svCvars[i].Val1);
		val2I = atoi(cg.svCvars[i].Val2);
		cvalIsF = (strstr(currentVal,"."))? qtrue : qfalse;
		val1IsF = (strstr(cg.svCvars[i].Val1,"."))? qtrue : qfalse;
		val2IsF = (strstr(cg.svCvars[i].Val2,"."))? qtrue : qfalse;

		switch ( cg.svCvars[i].mode ) {
			case SVC_EQUAL:
				if ( Q_stricmp( cg.svCvars[i].Val1, currentVal) )
					trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val1);
				break;
			case SVC_GREATER:
				if ( cvalF <= val1F ) {
					if ( cvalIsF || val1IsF ) {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%8.4f", val1F+0.0001f));
					} else {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%i", val1I+1));
					}
				}
				break;
			case SVC_GREATEREQUAL:
				if ( cvalF < val1F ) {
					trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val1);
				}
				break;
			case SVC_LOWER:
				if ( cvalF >= val1F ) {
					if ( cvalIsF || val1IsF ) {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%8.4f", val1F-0.0001f));
					} else {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%i", val1I-1));
					}
				}
				break;
			case SVC_LOWEREQUAL:
				if ( cvalF > val1F ) {
					if ( cvalIsF || val1IsF ) {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%8.4f", val1F));
					} else {
						trap_Cvar_Set(cg.svCvars[i].cvarName, va("%i", val1I));
					}
				}
				break;
			case SVC_INSIDE:
				if ( val1F || val1I ) {
					if ( cvalF < val1F ) {
						trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val1);
					}
				}
				if ( val2F || val2I ) {
					if ( cvalF > val2F ) {
						trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val2);
					}
				}
				break;
			case SVC_OUTSIDE:
				if ( val1F || val1I ) {
					if ( cvalF >= val1F ) {
						if ( !val2F || cvalF < val2F ) {
							if ( cvalIsF || val1IsF ) {
								trap_Cvar_Set(cg.svCvars[i].cvarName, va("%8.4f", val1F-0.0001f));
							} else {
								trap_Cvar_Set(cg.svCvars[i].cvarName, va("%i", val1I-1));
							}
						}
					}
				}
				if ( val2F || val2I ) {
					if ( cvalF <= val2F ) {
						if ( cvalF > val1F ) {
							if ( cvalIsF || val2IsF ) {
								trap_Cvar_Set(cg.svCvars[i].cvarName, va("%8.4f", val2F+0.0001f));
							} else {
								trap_Cvar_Set(cg.svCvars[i].cvarName, va("%i", val2I+1));
							}
						}
					}
				}
				break;
			case SVC_INCLUDE:
				if ( !strstr( currentVal, cg.svCvars[i].Val1) ){
					trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val2);
				}
				break;
			case SVC_EXCLUDE:
				if ( strstr( currentVal, cg.svCvars[i].Val1) ){
						trap_Cvar_Set(cg.svCvars[i].cvarName, cg.svCvars[i].Val2);
				}
				break;
		}
	}

	CG_UpdateCvars();

	if ( cg_FTAutoSelect.integer && CG_IsOnFireteam( cg.clientNum )) {
		clientInfo_t *ci;

		for(i = 0; i < MAX_FIRETEAM_MEMBERS; ++i) {
			ci = CG_SortedFireTeamPlayerForPosition( i );
			if( !ci ) {
				break;
			}

			ci->selected = qtrue;
		}
	}

	// if we are only updating the screen as a loading
	// pacifier, don't even try to read snapshots
	if ( cg.infoScreenText[0] != 0 ) {
		CG_DrawInformation( qfalse );
		return;
	}

	CG_PB_ClearPolyBuffers();

	CG_UpdatePMLists();

	CG_UpdateMMLists();

	// any looped sounds will be respecified as entities
	// are added to the render list
	trap_S_ClearLoopingSounds();

	CG_UpdateBufferedSoundScripts();

	// set up cg.snap and possibly cg.nextSnap
	CG_ProcessSnapshots();

	// if we haven't received any snapshots yet, all
	// we can draw is the information screen
	if ( !cg.snap || ( cg.snap->snapFlags & SNAPFLAG_NOT_ACTIVE ) ) {
		CG_DrawInformation( qfalse );
		return;
	}

	// check for server set weapons we might not know about
	// (FIXME: this is a hack for the time being since a scripted "selectweapon" does
	// not hit the first snap, the server weapon set in cg_playerstate.c line 219 doesn't
	// do the trick)
	if( !cg.weaponSelect && cg.snap->ps.weapon) {
		cg.weaponSelect = cg.snap->ps.weapon;
		cg.weaponSelectTime = cg.time;
	}

	// The very first frame
	if( cg.clientFrame == 0) {
		// kw: autorecord on late joins // IRATA: Added
		if( !cg.demoPlayback && cgs.gamestate == GS_PLAYING && cg_autoAction.integer & AA_DEMORECORD  ) {
			if( /* !cg.demoRecording && // TESTME w/o this var/this is 2.55 fix */ !cl_demorecording.integer )
				CG_autoRecord_f();
		}
	}

	if(!cg.lightstylesInited) {
		CG_SetupDlightstyles();
	}

	// this counter will be bumped for every valid scene we generate
	cg.clientFrame++;

	// update cg.predictedPlayerState
	CG_PredictPlayerState();

	// OSP -- MV handling
#ifdef MV_SUPPORT
	if(cg.mvCurrentMainview != NULL && cg.snap->ps.pm_type != PM_INTERMISSION && cgs.mvAllowed ) {
		CG_mvDraw(cg.mvCurrentMainview);
		// FIXME: not valid for demo playback
		cg.zoomSensitivity = mv_sensitivity.value / int_sensitivity.value;
	} else
#endif
		{
		int		inwater;

		// clear all the render lists
		trap_R_ClearScene();

		// decide on third person view
		cg.renderingThirdPerson = cg_thirdPerson.integer || (cg.snap->ps.stats[STAT_HEALTH] <= 0) || cg.showGameView;
		// build cg.refdef
		inwater = CG_CalcViewValues();
		CG_SetupFrustum();

		// RF, draw the skyboxportal
		CG_DrawSkyBoxPortal(qtrue);

		if(inwater) {
			CG_UnderwaterSounds();
		}

		// first person blend blobs, done after AnglesToAxis
		if ( !cg.renderingThirdPerson ) {
			CG_DamageBlendBlob();
		}

		// build the render lists
		if ( !cg.hyperspace ) {
			CG_AddPacketEntities();			// after calcViewValues, so predicted player state is correct
			CG_AddMarks();
			CG_AddScriptSpeakers();

			if (cg_locations.integer & LOC_DEBUG ) {
				CG_RenderLocations();
			}
			// Rafael particles
			CG_AddParticles ();
			// done.
			CG_AddLocalEntities();
			CG_AddSmokeSprites();
			CG_AddAtmosphericEffects();
		}

		// Rafael mg42
		if( !cg.showGameView && !cgs.dbShowing ) {
			// jP Note - dont draw player's gun if on a mounted MG
			if( !cg.snap->ps.persistant[PERS_HWEAPON_USE] ){

				CG_AddViewWeapon( &cg.predictedPlayerState );
			}
			else {
				if( cg.time - cg.predictedPlayerEntity.overheatTime < SECONDS_3 ) {
					vec3_t muzzle;

					CG_CalcMuzzlePoint( cg.snap->ps.clientNum, muzzle );

					muzzle[2] -= 32;

					if(!(rand()%3)) {
						float alpha = 1.0f - ((float)( cg.time - cg.predictedPlayerEntity.overheatTime)/SECONDS_3_F );
						alpha *= 0.25f;		// .25 max alpha
						CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, muzzle, SECONDS_1, 8, 20, 30, alpha, 8.f );
					}
				}
			}
		}

		// NERVE - SMF - play buffered voice chats
		CG_PlayBufferedVoiceChats();

		// Ridah, trails
		if( !cg.hyperspace ) {
			CG_AddFlameChunks ();
			CG_AddTrails ();		// this must come last, so the trails dropped this frame get drawn
		}
		// done.

		// finish up the rest of the refdef
#ifdef _DEBUG
		if( cg.testModelEntity.hModel ) {
			CG_AddTestModel();
		}
#endif
		cg.refdef.time = cg.time;
		memcpy( cg.refdef.areamask, cg.snap->areamask, sizeof( cg.refdef.areamask ) );

		// make sure the lagometerSample and frame timing isn't done twice when in stereo
		if ( stereoView != STEREO_RIGHT ) {
			cg.frametime = cg.time - cg.oldTime;
			if ( cg.frametime < 0 ) {
				cg.frametime = 0;
			}
			cg.oldTime = cg.time;
			CG_AddLagometerFrameInfo();
		}

		// DHM - Nerve :: let client system know our predicted origin
		trap_SetClientLerpOrigin( cg.refdef.vieworg[0], cg.refdef.vieworg[1], cg.refdef.vieworg[2] );

		// actually issue the rendering calls
		CG_DrawActive( stereoView );

		// update audio positions
		trap_S_Respatialize( cg.snap->ps.clientNum, cg.refdef.vieworg, cg.refdef.viewaxis, inwater );

		// jet Pilot - Mortar Cam?, jaquboss here is it better
		if ( !cg.showGameView && !cg.intermissionStarted && !cgs.dbShowing ) {
			static rectDef_t rect = {16, 160, 160, 120};

			if ( cg.latestMissile && (cg_insanity.integer & (JP_INSANITY_MORTARCAM | JP_INSANITY_MISSILECAM | JP_INSANITY_RIFLENADECAM)) ) {
				static vec4_t	color= {1, 1, 1, .8}, colorShade = {0, 0, 0, .8};

				switch(cg.latestMissile->currentState.weapon) {
					case WP_PANZERFAUST:
					case WP_BAZOOKA:
						CG_DrawSmallStringColor( 48, 146, "MISSILE CAM", colorShade);
						CG_DrawSmallStringColor( 49, 145, "MISSILE CAM", color);
						CG_DrawMissileCamera( &rect );
						break;
					case WP_GPG40:
					case WP_M7:
						CG_DrawSmallStringColor( 48, 146, "RIFLENADE CAM", colorShade);
						CG_DrawSmallStringColor( 49, 145, "RIFLENADE CAM", color);
						CG_DrawMissileCamera( &rect );
						break;
					case WP_MORTAR_SET:
					case WP_MORTAR2_SET:
						CG_DrawSmallStringColor( 48, 146, "MORTAR CAM", colorShade);
						CG_DrawSmallStringColor( 49, 145, "MORTAR CAM", color);
						CG_DrawMissileCamera( &rect );
						break;
					default:
						break;
				}
			}
			// core: The killer Cam..
			else if ( cg.latestKiller && (cg_insanity.integer & JP_INSANITY_KILLERCAM) ) {
				// IRATA: print the name of our killer instead of "KILLER CAM" ... 20 chars ...
				CG_DrawStringExt( 16, 145, va("^W%s^W", cgs.clientinfo[ cg.latestKiller->currentState.number ].name), colorWhite, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 20);
				CG_DrawKillerCamera( &rect );
			}
		}
	}

#ifdef _DEBUG
	if ( cg_stats.integer ) {
		CG_Printf( "cg.clientFrame:%i\n", cg.clientFrame );
	}
#endif

	// let the client system know what our weapon, holdable item and zoom settings are
	trap_SetUserCmdValue( cg.weaponSelect, cg.showGameView ? 0x01 : 0x00, cg.zoomSensitivity, cg.identifyClientRequest );
}
