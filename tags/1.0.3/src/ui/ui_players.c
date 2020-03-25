// ui_players.c

#include "ui_local.h"

#ifdef JAQU_WIP
/*
**  Character loading
*/

char bigTextBuffer[100000];			// <- was causing callstacks >64k

/*
=====================
UI_ParseAnimationFiles

  Read in all the configuration and script files for this model.
=====================
*/
static qboolean UI_ParseAnimationFiles( bg_character_t *character, const char *animationGroup, const char *animationScript )
{
	char			filename[MAX_QPATH];
	fileHandle_t	f;
	int				len;

	// set the name of the animationGroup and animationScript in the animModelInfo structure
	Q_strncpyz( character->animModelInfo->animationGroup, animationGroup, sizeof(character->animModelInfo->animationGroup) );
	Q_strncpyz( character->animModelInfo->animationScript, animationScript, sizeof(character->animModelInfo->animationScript) );

	BG_R_RegisterAnimationGroup( animationGroup, character->animModelInfo );

	// load the script file
	len = trap_FS_FOpenFile( animationScript, &f, FS_READ );
	if( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( bigTextBuffer ) - 1 ) {
		trap_Print( va( "File %s is too long\n", filename ) );
		return qfalse;
	}
	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( character->animModelInfo, &ui_animScriptData, animationScript, bigTextBuffer );

	return qtrue;
}

/*
==================
UI_CheckForExistingAnimModelInfo

  If this player model has already been parsed, then use the existing information.
  Otherwise, set the modelInfo pointer to the first free slot.

  returns qtrue if existing model found, qfalse otherwise
==================
*/
static qboolean UI_CheckForExistingAnimModelInfo( const char *animationGroup, const char *animationScript, animModelInfo_t **animModelInfo )
{
	int i;
	animModelInfo_t *trav, *firstFree = NULL;

	for( i = 0, trav = ui_animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
		if( *trav->animationGroup && *trav->animationScript ) {
			if( !Q_stricmp( trav->animationGroup, animationGroup ) && !Q_stricmp( trav->animationScript, animationScript ) ) {
				// found a match, use this animModelInfo
				*animModelInfo = trav;
				return qtrue;
			}
		} else if( !firstFree ) {
			firstFree = trav;
		}
	}

	if( !firstFree ) {
		trap_Error( "unable to find a free modelinfo slot, cannot continue\n" );
	} else {
		*animModelInfo = firstFree;
		// clear the structure out ready for use
		memset( *animModelInfo, 0, sizeof(*animModelInfo) );
	}

	// qfalse signifies that we need to parse the information from the script files
	return qfalse;
}

/*
==============
UI_RegisterAcc
==============
*//*
static qboolean UI_RegisterAcc( const char *modelName, int *model, const char* skinname, qhandle_t* skin ) {
	char	filename[MAX_QPATH];

	*model = trap_R_RegisterModel( modelName );

	if( !*model )
		return qfalse;

	COM_StripExtension( modelName, filename );
	Q_strcat( filename, sizeof(filename), va( "_%s.skin", skinname ) );
	*skin = trap_R_RegisterSkin( filename );

	return qtrue;
}
*/

/*
===================
UI_RegisterCharacter
===================
*/
qboolean UI_RegisterCharacter( const char *characterFile, bg_character_t *character )
{
	bg_characterDef_t	characterDef;
	char *filename;
	char buf[MAX_QPATH];
//	char accessoryname[MAX_QPATH];
	//int					i;

	memset( &characterDef, 0, sizeof(characterDef) );

	if( !BG_ParseCharacterFile( characterFile, &characterDef ) ) {
		return qfalse;	// the parser will provide the error message
	}

	// Register mesh
	if( !(character->mesh[0] = trap_R_RegisterModel( characterDef.mesh )) )
		trap_Print( va( S_COLOR_YELLOW "WARNING: failed to register mesh '%s' referenced from '%s'\n", characterDef.mesh, characterFile ) );
	// Register Skin
	COM_StripExtension( characterDef.mesh, buf );
	filename = va( "%s_%s.skin", buf, characterDef.skin );
	if( !(character->skin = trap_R_RegisterSkin( filename )) ) {
		trap_Print( va( S_COLOR_YELLOW "WARNING: failed to register skin '%s' referenced from '%s'\n", filename, characterFile) );
	} else {
		character->torsoSkin[0] = character->legsSkin[0] = character->skin;
	}

	// head
	if( !(character->hudhead[0] = trap_R_RegisterModel( characterDef.hudhead )) )
		trap_Print( va( S_COLOR_YELLOW "WARNING: failed to register hudhead '%s' referenced from '%s'\n", characterDef.hudhead, characterFile ) );
	
	if( !(character->hudheadskin[0] = trap_R_RegisterSkin( characterDef.hudheadskin )) )
		trap_Print( va( S_COLOR_YELLOW "WARNING: failed to register hudheadskin '%s' referenced from '%s'\n", characterDef.hudheadskin, characterFile ) );


	// Parse Animation Files
	if( !UI_CheckForExistingAnimModelInfo( characterDef.animationGroup, characterDef.animationScript, &character->animModelInfo ) ) {
		if( !UI_ParseAnimationFiles( character, characterDef.animationGroup, characterDef.animationScript ) ) {
			trap_Print( va ( S_COLOR_YELLOW "WARNING: failed to load animation files referenced from '%s'\n", characterFile ) );
			return qfalse;
		}
	}

	return qtrue;
}

/*
=======================
UI_RegisterPlayerClasses
=======================
*/
void UI_RegisterPlayerClasses( void )
{
	bg_playerclass_t	*classInfo;
	bg_character_t		*character;
	int					team, cls;

	for( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ ) {
		for( cls = PC_SOLDIER; cls < NUM_PLAYER_CLASSES; cls++ ) {
			classInfo = BG_GetPlayerClassInfo( team, cls );
			character = BG_GetCharacter( team, cls );

			Q_strncpyz( character->characterFile, classInfo->characterFile, sizeof(character->characterFile) );

			if( !UI_RegisterCharacter( character->characterFile, character ) ) {
				trap_Error(   va( S_COLOR_RED "ERROR: UI_RegisterPlayerClasses: failed to load character file '%s' for the %s %s\n", character->characterFile, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum )) );
			}
		}
	}
}


/*
======================
UI_PositionEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void UI_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent, const char *tagName, int startIndex, vec3_t *offset) {
	int				i;
	orientation_t	lerped;
	
	// lerp the tag
	trap_CM_LerpTag( &lerped, parent, tagName, startIndex );

	// allow origin offsets along tag
	VectorCopy( parent->origin, entity->origin );

	if( offset ) {
		VectorAdd( lerped.origin, *offset, lerped.origin );
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// had to cast away the const to avoid compiler problems...
	MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, entity->axis );
}


/*
======================
UI_PositionRotatedEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void UI_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent, const char *tagName ) {
	int				i;
	orientation_t	lerped;
	vec3_t			tempAxis[3];

	// lerp the tag
	trap_CM_LerpTag( &lerped, parent, tagName, 0 );

	// FIXME: allow origin offsets along tag?
	VectorCopy( parent->origin, entity->origin );
	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// had to cast away the const to avoid compiler problems...
	MatrixMultiply( entity->axis, lerped.axis, tempAxis );
	MatrixMultiply( tempAxis, ((refEntity_t *)parent)->axis, entity->axis );
}

#define CROUCHING(anim)	( (anim) && ((anim)->movetype & ((1<<ANIM_MT_IDLECR)|(1<<ANIM_MT_WALKCR)|(1<<ANIM_MT_WALKCRBK))) )

static void UI_SetLerpFrameAnimation( playerInfo_t *pi, lerpFrame_t *lf, int newAnimation, bg_character_t *character )
{
	animation_t *oldAnim;
	animation_t *anim;
	int transitionMin = -1;
	qboolean firstAnim = qfalse;

	if ( !lf->animation ) {
		firstAnim = qtrue;
	}

	oldAnim = lf->animation;

	lf->animationNumber = newAnimation;
	anim = BG_GetAnimationForIndex(character->animModelInfo, lf->animationNumber & ~ANIM_TOGGLEBIT);

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if ( !(anim->flags & ANIMFL_FIRINGANIM) || (lf != &pi->torso ) ) {
		if ( (lf == &pi->legs) && (CROUCHING(oldAnim) != CROUCHING(anim)) ) {
			if ( anim->moveSpeed || ( anim->movetype & ((1<<ANIM_MT_TURNLEFT)|(1<<ANIM_MT_TURNRIGHT)) ) ) // if unknown movetype, go there faster
				transitionMin = lf->frameTime + 200;	// slowly raise/drop
			else
				transitionMin = lf->frameTime + 350;	// slowly raise/drop
		} else if ( anim->moveSpeed ) {
			transitionMin = lf->frameTime + 120;	// always do some lerping (?)
		} else	// not moving, so take your time
			transitionMin = lf->frameTime + 170;	// always do some lerping (?)

		if ( oldAnim && oldAnim->animBlend ) { //transitionMin < lf->frameTime + oldanim->animBlend) {
			transitionMin = lf->frameTime + oldAnim->animBlend;
			lf->animationTime = transitionMin;
		} else {
			// slow down transitions according to speed
			if( anim->moveSpeed && lf->animSpeedScale < 1.0 )
				lf->animationTime += anim->initialLerp;
			if( lf->animationTime < transitionMin )
				lf->animationTime = transitionMin;
		}
	}

	// if first anim, go immediately
	if ( firstAnim ) {
		lf->frameTime = DC->realTime - 1;
		lf->animationTime = DC->realTime - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
	}
}


void UI_RunLerpFrame( playerInfo_t *pi, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;
	bg_character_t *character=BG_GetCharacter(pi->teamNum, pi->classNum );

	// see if the animation sequence is switching
	if( pi && (newAnimation != lf->animationNumber || !lf->animation) ) {
		UI_SetLerpFrameAnimation( pi, lf, newAnimation,  character);
	} 
	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( DC->realTime >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if( DC->realTime < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;

		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;		// adjust for haste, etc
		if( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = DC->realTime;

			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;

		if( DC->realTime > lf->frameTime ) {
			lf->frameTime = DC->realTime;

		}
	}

	if( lf->frameTime > DC->realTime + 200 ) {
		lf->frameTime = DC->realTime;
	
	}

	if( lf->oldFrameTime > DC->realTime ) {
		lf->oldFrameTime = DC->realTime;

	}
	// calculate current lerp value
	if( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( DC->realTime - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}

int UI_GetAnimationForName ( playerInfo_t *pi, const char *name )
{
	int				i;
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );
	if ( !character )
		return 0;

	for ( i = 0; i < character->animModelInfo->numAnimations; i++ ) {
		if ( !Q_stricmp( character->animModelInfo->animations[i]->name, name) )
			return i;
	}

	return 0; // safe fallback so we never end up without an animation (which will crash the game)
}

/*
===============
CG_PlayerAnimation
===============
*/
static void UI_PlayerAnimation( playerInfo_t *pi, refEntity_t *body ) {
	int				animIndex;
	bg_character_t	*character;

	character = BG_GetCharacter( pi->teamNum, pi->classNum );

	if( !character ) {
		return;
	}
	// default to whatever the legs are currently doing
	animIndex = UI_GetAnimationForName(pi, "stand_machinegun")/*legsanim*/;

	UI_RunLerpFrame( pi, &pi->legs, animIndex, 1 );

	body->oldframe = pi->legs.oldFrame;
	body->frame = pi->legs.frame;
	body->backlerp = pi->legs.backlerp;
	body->frameModel = pi->legs.frameModel;
	body->oldframeModel = pi->legs.oldFrameModel;

	UI_RunLerpFrame( pi, &pi->torso, UI_GetAnimationForName(pi, "stand_machinegun")/*torsoanim*/, 1 );


	body->oldTorsoFrame = pi->torso.oldFrame;
	body->torsoFrame = pi->torso.frame;
	body->torsoBacklerp = pi->torso.backlerp;
	body->torsoFrameModel = pi->torso.frameModel;
	body->oldTorsoFrameModel = pi->torso.oldFrameModel;
}

static void UI_SwingAngles ( float destination, float swingTolerance, float clampTolerance, float speed, float *angle, qboolean *swinging )
{
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance )
			*swinging = qtrue;
	}

	if ( !*swinging )
		return;

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	if ( scale < swingTolerance * 0.5 )
		scale = 0.5;
	else if ( scale < swingTolerance )
		scale = 1.0;
	else
		scale = 2.0;

	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = DC->frameTime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		}

		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = DC->frameTime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		}

		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
		*angle = AngleMod( destination - (clampTolerance - 1) );
	else if ( swing < -clampTolerance )
		*angle = AngleMod( destination + (clampTolerance - 1) );

}


/*
===============
UI_PlayerAngles
===============
*/
void UI_PlayerAngles( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
{
	vec3_t	legsAngles, torsoAngles, headAngles;
	float	dest;
	VectorCopy( pi->viewAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );
	torsoAngles[YAW] = 180;
	legsAngles[YAW] = 180;
	headAngles[YAW] = 180;

	// --------- pitch -------------
	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 )
		dest = ( -360 + headAngles[PITCH] ) * 0.75;
	else
		dest = headAngles[PITCH] * 0.75;

	UI_SwingAngles( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesSubtract( legsAngles, pi->moveAngles, legsAngles );	// NERVE - SMF
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}



void UI_SetHudHeadLerpFrameAnimation ( bg_character_t *ch, lerpFrame_t *lf, int newAnimation )
{
	animation_t *anim;
	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= MAX_HD_ANIMATIONS )
		newAnimation = 0;

	anim = &ch->hudheadanimations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
}

void UI_ClearHudHeadLerpFrame ( bg_character_t *ch, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = DC->realTime;
	UI_SetHudHeadLerpFrameAnimation( ch, lf, animationNumber );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
	lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
}

void UI_RunHudHeadLerpFrame ( bg_character_t *ch, lerpFrame_t *lf, int newAnimation, float speedScale )
{
	int			f;
	animation_t *anim;

	// see if the animation sequence is switching
	if ( !lf->animation )
		UI_ClearHudHeadLerpFrame( ch, lf, newAnimation );
	else if ( newAnimation != lf->animationNumber )
		UI_SetHudHeadLerpFrameAnimation( ch, lf, newAnimation );

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( DC->realTime >= lf->frameTime )
	{
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp )
			return;								// shouldn't happen
	
		if ( DC->realTime < lf->animationTime )
			lf->frameTime = lf->animationTime;	// initial lerp
		else
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;

		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;						// adjust for haste, etc

		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = DC->realTime;
			}
		}

		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( DC->realTime > lf->frameTime )
			lf->frameTime = DC->realTime;
	}

	if ( lf->frameTime > DC->realTime + 200 )
		lf->frameTime = DC->realTime;

	if ( lf->oldFrameTime > DC->realTime )
		lf->oldFrameTime = DC->realTime;

	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime )
		lf->backlerp = 0;
	else
		lf->backlerp = 1.0 - (float) ( DC->realTime - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
}

void UI_HudHeadAnimation( bg_character_t *ch, lerpFrame_t *lf, int *oldframe, int *frame, float *backlerp, hudHeadAnimNumber_t animation )
{

	//	centity_t *cent = &cg.predictedPlayerEntity;
	UI_RunHudHeadLerpFrame( ch, lf, (int) animation, 1.f );
	*oldframe = lf->oldFrame;
	*frame = lf->frame;
	*backlerp = lf->backlerp;
}

playerCharacter_t	*UI_TEMPGETCHAR( int teamNum, int classNum )
{
	playerCharacter_t *character;

	character = &customizedChar[classNum][teamNum-1];

	if ( !character )
		memset( character, 0, sizeof(character) );
	
	character->backPackIndex = 0;
	character->bLeftIndex = 0;
	character->bodyIndex = ui_bodyIndex.integer;
	character->bRightIndex = 0;
	character->faceIndex = ui_faceIndex.integer;
	character->glassIndex = 0;//ui_glassIndex.integer
	character->headIndex = ui_headIndex.integer;
	character->helmetIndex = 0;
	character->lBeltIndex = 0;
	character->torsoIndex = ui_torsoIndex.integer;
	character->uBeltIndex = 0;
	character->legIndex = ui_legIndex.integer;

	return character;
}

int UI_RenderPlayer( float x, float y, float w, float h, playerInfo_t *pi, int time, int viewHeight )
{
	refdef_t		refdef;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		gun;
	refEntity_t		acc;
	vec3_t			origin;
	int				renderfx;
	vec3_t			mins = { -16, -16, -24 };
	vec3_t			maxs = { 16, 16, 32 };
	float			len;
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );
	playerCharacter_t	*customized = UI_TEMPGETCHAR(pi->teamNum, pi->classNum);
	int				i;
	vec3_t			myAngle;

	UI_AdjustFrom640( &x, &y, &w, &h );
	memset( &refdef, 0, sizeof(refdef) );
	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	refdef.rdflags = RDF_NOWORLDMODEL;
	AxisClear( refdef.viewaxis );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;
	myAngle[0] = viewHeight;
	myAngle[1] = 0;
	myAngle[2] = 0;
	AnglesToAxis( myAngle, refdef.viewaxis );
	refdef.fov_x = 35;
	refdef.fov_y = 40;

	len = 0.9 * ( maxs[2] - mins[2] );	
	origin[0] = pi->y - 70 + ( len / tan( DEG2RAD( refdef.fov_x) * 0.5) );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );
	origin[2] = pi->z - 23 + ( -0.5 * (mins[2] + maxs[2]) );

	// END Mad Doc - TDF
	refdef.time = time;
	trap_R_ClearScene();

	// get the rotation information
	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW /*| RF_FORCENOLOD*/;
	UI_PlayerAngles( pi, body.axis, body.torsoAxis, head.axis );
	acc.renderfx = renderfx;

	//
	// add the body
	//
	body.renderfx = renderfx;
	VectorCopy( origin, body.origin );
	VectorCopy( origin, body.lightingOrigin );
	VectorCopy( body.origin, body.oldorigin );

	if ( customized->bodyIndex >= 0 && customized->bodyIndex <= MAX_CHARACTER_INDEXES ) 
	body.hModel = character->mesh[customized->bodyIndex];
	if ( !body.hModel )
		body.hModel = character->mesh[0];// trap_R_RegisterModel(/*"ui/wolfbody.mds"*/"models/players/temperate/axis/soldier/body.mdm");
	if ( !body.hModel )
		return 1;

	UI_PlayerAnimation( pi, &body );

	// add torso skin
	if ( customized->torsoIndex >= 0 && customized->torsoIndex <= MAX_CHARACTER_INDEXES )
		body.customSkin = character->torsoSkin[customized->torsoIndex];
	if ( !body.customSkin )
		body.customSkin = character->torsoSkin[0];
	if ( !body.customSkin )
		return 2;

	DC->addRefEntityToScene( &body );

	body.customSkin = 0;

	// add legs skin
	if ( customized->legIndex >= 0 && customized->legIndex <= MAX_CHARACTER_INDEXES ) 
		body.customSkin = character->legsSkin[customized->legIndex];
	if ( !body.customSkin )
		body.customSkin = character->legsSkin[0];
	if ( !body.customSkin )
		return 3;

	DC->addRefEntityToScene( &body );

	if ( customized->headIndex >= 0 && customized->headIndex <= MAX_CHARACTER_INDEXES ) 
		head.hModel = character->hudhead[customized->headIndex];
	if ( !head.hModel )
		head.hModel = character->hudhead[0];
	if ( !head.hModel )
		return 4;

	if ( customized->faceIndex >= 0 && customized->faceIndex <= MAX_CHARACTER_INDEXES ) 
		head.customSkin = character->hudheadskin[customized->headIndex];
	if ( !head.customSkin )
		head.customSkin = character->hudheadskin[0];
	if ( !head.customSkin )
		return 5;


	// ok play some anim
	UI_HudHeadAnimation( character, &pi->headAnim, &head.oldframe, &head.frame, &head.backlerp, (rand() % (HD_IDLE8 - HD_IDLE2 + 1)) + HD_IDLE2 );

	VectorCopy( origin, head.lightingOrigin );
	UI_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
	head.renderfx = renderfx;
	DC->addRefEntityToScene( &head );

	AxisCopy( body.torsoAxis, acc.axis );
	VectorCopy( origin, acc.lightingOrigin );

	// !!TODO!!
	/*for ( i = ACC_BELT_LEFT; i < ACC_MAX; i++ )
	{
		if ( !(character->accModels[i]) )
		{
			continue;
		}

		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];
		acc.renderfx = renderfx;
		switch ( i )
		{
			case ACC_BELT_LEFT:
				UI_PositionEntityOnTag( &acc, &body, "tag_bright", 0, NULL );
				break;
			case ACC_BELT_RIGHT:
				UI_PositionEntityOnTag( &acc, &body, "tag_bleft", 0, NULL );
				break;
			case ACC_BELT:
				UI_PositionEntityOnTag( &acc, &body, "tag_ubelt", 0, NULL );
				break;
			case ACC_BACK:
				UI_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
				break;
			case ACC_HAT:					// hat
			case ACC_MOUTH2:				// hat2
			case ACC_MOUTH3:				// hat3
				UI_PositionEntityOnTag( &acc, &head, "tag_mouth", 0, NULL );
				break;

			// weapon and weapon2
			// these are used by characters who have permanent weapons attached to their character in the skin
			case ACC_WEAPON:				// weap
				UI_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
				break;
			case ACC_WEAPON2:				// weap2
				UI_PositionEntityOnTag( &acc, &body, "tag_weapon2", 0, NULL );
				break;
			default:
				continue;
		}

		DC->addRefEntityToScene( &acc );
	}*/

	//
	// add the gun
	// - fixme - add something better
	{
		memset( &gun, 0, sizeof(gun) );

		if ( pi->teamNum == TEAM_AXIS )
			gun.hModel = trap_R_RegisterModel(  "models/weapons2/mp40/mp40.md3" );
		else
			gun.hModel = trap_R_RegisterModel(  "models/weapons2/thompson/thompson.md3" );

		VectorCopy( origin, gun.lightingOrigin );
		UI_PositionEntityOnTag( &gun, &body, "tag_weapon", 0, NULL );
		gun.renderfx = renderfx;
		DC->addRefEntityToScene( &gun );
	}

	DC->renderScene( &refdef );

	return 0;
}

void UI_PreparePlayerRender ( rectDef_t *rect, int playerClass, int playerTeam ){
	playerInfo_t	*pi;
	int				errorNum=0;

	pi = &renderedPlayerInfo[playerClass][playerTeam-1];

	if ( !pi->loaded  ){
		memset( pi, 0, sizeof(pi) );
		memset( &pi->headAnim, 0, sizeof(pi->headAnim) );
		memset( &pi->legs, 0, sizeof(pi->legs) );
		memset( &pi->torso, 0, sizeof(pi->torso) );
		pi->classNum = playerClass;
		pi->teamNum = playerTeam;
		pi->loaded = qtrue;
	} 

	errorNum = UI_RenderPlayer( rect->x, rect->y, rect->w, rect->h, pi, DC->realTime, 0 );
	
	if ( errorNum != 0 )
		 trap_Print( va( S_COLOR_YELLOW "WARNING: failed to render player, errornum: %i \n", errorNum ));

}
#endif
