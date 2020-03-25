/*
 * name:		cg_players.c
 *
 * desc:		handle the media and animation for player entities
 *
*/

#include "cg_local.h"
#include "../game/bg_classes.h"

#define	SWING_RIGHT	1
#define SWING_LEFT	2

#define JUMP_HEIGHT				56
#define SWINGSPEED				0.3

//static int			dp_realtime;
//static float		jumpHeight;

animation_t		*lastTorsoAnim;
animation_t		*lastLegsAnim;

extern const char* cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS-1];

/*
================
CG_EntOnFire
================
*/
qboolean CG_EntOnFire( centity_t *cent ) {
	if ( cent->currentState.number == cg.snap->ps.clientNum ) {
		// TAT 11/15/2002 - the player is always starting out on fire, which is easily seen in cinematics
		//		so make sure onFireStart is not 0
		return	( cg.snap->ps.onFireStart
			&& (cg.snap->ps.onFireStart < cg.time)
			&& ((cg.snap->ps.onFireStart+2000) > cg.time) );
	}
	else {
		return	(	(cent->currentState.onFireStart < cg.time) &&
					(cent->currentState.onFireEnd > cg.time));
	}
}

/*
================
CG_IsCrouchingAnim
================
*/
qboolean CG_IsCrouchingAnim( animModelInfo_t* animModelInfo, int animNum ) {
	animation_t *anim;

	// FIXME: make compatible with new scripting
	animNum &= ~ANIM_TOGGLEBIT;
	//
	anim = BG_GetAnimationForIndex( animModelInfo, animNum );
	//
	if( anim->movetype & ((1<<ANIM_MT_IDLECR)|(1<<ANIM_MT_WALKCR)|(1<<ANIM_MT_WALKCRBK)) )
		return qtrue;
	//
	return qfalse;
}

/*
================
CG_CustomSound
================
*/
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName ) {
    if (!soundName) {
        return 0;
    }
	else if ( *soundName != '*' ) {
		return trap_S_RegisterSound( soundName, qfalse );
	}
    else {
        return 0;
		//const char *snd = soundName + 1;
		//return trap_S_RegisterSound(va("sound/player/%s/%s", cgs.clientinfo[clientNum].team == TEAM_AXIS ? "axis" : "allied", snd), qfalse);
	}

}

/*
=============================================================================

CLIENT INFO

=============================================================================
*/

/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void CG_LoadClientInfo( int clientNum ) {
	int			i;

	// reset any existing players and bodies, because they might be in bad
	// frames for this new model
	for( i = 0 ; i < MAX_GENTITIES ; i++ ) {
		if ( cg_entities[i].currentState.clientNum == clientNum && cg_entities[i].currentState.eType == ET_PLAYER ) {
			CG_ResetPlayerEntity( &cg_entities[i] );
		}
	}
}

void CG_ParseTeamXPs( int n ) {
	int i, j;
	char* cs = (char*)CG_ConfigString( CS_AXIS_MAPS_XP + n );
	const char* token;

	for( i = 0; i < MAX_MAPS_PER_CAMPAIGN; i++ ) {
		for( j = 0; j < SK_NUM_SKILLS; j++ ) {
			token = COM_ParseExt( &cs, qfalse );

			if( !token || !*token ) {
				return;
			}

			if( n == 0 ) {
				cgs.tdbAxisMapsXP[ j ][ i ] = atoi( token );
			} else {
				cgs.tdbAlliedMapsXP[ j ][ i ] = atoi( token );
			}
		}
	}
}

void CG_LimboPanel_SendSetupMsg( qboolean forceteam );

/*
======================
CG_NewClientInfo
======================
*/
void CG_NewClientInfo( int clientNum ) {
	clientInfo_t *ci;
	clientInfo_t newInfo;
	const char	*configstring;
	const char	*v;
	//Chruker Bug fix 020  
	//int   oldclass;  

	ci = &cgs.clientinfo[clientNum];

	configstring = CG_ConfigString( clientNum + CS_PLAYERS );
	if ( !*configstring ) {
		memset( ci, 0, sizeof( *ci ) );
		return;		// player just left
	}

	// build into a temp buffer so the defer checks can use
	// the old value
	memset( &newInfo, 0, sizeof( newInfo ) );

	// Gordon: grabbing some older stuff, if it's a new client, tinfo will update within one second anyway, otherwise you get the health thing flashing red
	// NOTE: why are we bothering to do all this setting up of a new clientInfo_t anyway? it was all for deffered clients iirc, which we dont have
	newInfo.location[0] =	ci->location[0];
	newInfo.location[1] =	ci->location[1];
	newInfo.location[2] =	ci->location[2];
	newInfo.health =		ci->health;
	newInfo.fireteamData =	ci->fireteamData;
	newInfo.clientNum =		clientNum;
	newInfo.selected =		ci->selected;
	newInfo.totalWeapAcc =	ci->totalWeapAcc;

	// isolate the player's name
	v = Info_ValueForKey(configstring, "n");
	Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );
	Q_strncpyz( newInfo.cleanname, v, sizeof( newInfo.cleanname ) );
	Q_CleanStr( newInfo.cleanname );
	

	// bot skill
	v = Info_ValueForKey( configstring, "skill" );
	newInfo.botSkill = atoi( v );

	// team
	v = Info_ValueForKey( configstring, "t" );
	newInfo.team = atoi( v );

	// class
	v = Info_ValueForKey( configstring, "c" );
	newInfo.cls = atoi( v );

	// rank
	v = Info_ValueForKey( configstring, "r" );
	newInfo.rank = atoi( v );

	v = Info_ValueForKey( configstring, "f" );
	newInfo.fireteam = atoi( v );

	v = Info_ValueForKey( configstring, "m" );
	if( *v ) {
		int i;
		char buf[2];
		buf[1] = '\0';
		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			buf[0] = *v;
			newInfo.medals[i] = atoi( buf );
			v++;
		}
	}

	v = Info_ValueForKey( configstring, "ch" );
	if( *v ) {
		newInfo.character = cgs.gameCharacters[atoi(v)];
	}

	v = Info_ValueForKey( configstring, "s");
	if( *v ) {
		int i;

		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			char skill[2];

			skill[0] = v[i];
			skill[1] = '\0';

			newInfo.skill[i] = atoi(skill);
		}
	}

	// diguiseName
	v = Info_ValueForKey( configstring, "dn" );
	Q_strncpyz( newInfo.disguiseName, v, sizeof( newInfo.disguiseName ) );

	// disguise Clientnum, useful especialy for SSYSTEM
	v = Info_ValueForKey( configstring, "dc" );
	newInfo.disguiseClient = atoi( v );

	// disguiseRank
	v = Info_ValueForKey( configstring, "dr" );
	newInfo.disguiseRank = atoi( v );

	// Gordon: weapon and latchedweapon ( FIXME: make these more secure )
	v = Info_ValueForKey( configstring, "w" );
	newInfo.weapon = atoi( v );

	v = Info_ValueForKey( configstring, "lw" );
	newInfo.latchedweapon = atoi( v );

	v = Info_ValueForKey( configstring, "lc" );
	newInfo.latchedCls = atoi( v );

	v = Info_ValueForKey( configstring, "sw" );
	newInfo.secondaryweapon = atoi( v );

	v = Info_ValueForKey( configstring, "ref" );
	newInfo.refStatus = atoi( v );

	v = Info_ValueForKey( configstring, "rn" );
	newInfo.rifleNade = atoi( v );
	

	// Gordon: detect rank/skill changes client side
	if( clientNum == cg.clientNum ) {
		int i;

		if( newInfo.team != cgs.clientinfo[ cg.clientNum ].team ) {
			if( cgs.autoFireteamCreateEndTime != cg.time + 20000) { 
				cgs.autoFireteamCreateEndTime = 0;
			}
			if( cgs.autoFireteamJoinEndTime != cg.time + 20000) { 
				cgs.autoFireteamJoinEndTime = 0;
			}
		}

		if( newInfo.rank > cgs.clientinfo[ cg.clientNum ].rank ) {

			CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team == TEAM_ALLIES ? rankSoundNames_Allies[ newInfo.rank ] : rankSoundNames_Axis[ newInfo.rank ], NULL, -1, qtrue );

			CG_AddPMItemBig( PM_RANK, va("Promoted to rank %s!", cgs.clientinfo[ cg.clientNum ].team == TEAM_AXIS ? rankNames_Axis[newInfo.rank] : rankNames_Allies[newInfo.rank] ), rankicons[ newInfo.rank ][   cgs.clientinfo[cg.clientNum].team == TEAM_AXIS ? 1 : 0 ][0].shader, NULL ); // jaquboss, team support
		}

		// CHRUKER: b020 - Make sure player class and primary weapons are correct for
		//          subsequent calls to CG_LimboPanel_SendSetupMsg
		CG_LimboPanel_Setup();

		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			if( newInfo.skill[i] > cgs.clientinfo[ cg.clientNum ].skill[i] ) {
                // Jaybird - back up this figure
                int oldskill = cgs.clientinfo[cg.clientNum].skill[i];

				// Gordon: slick hack so that funcs we call use teh new value now
				cgs.clientinfo[cg.clientNum].skill[i] = newInfo.skill[i];

                // If player got light weapons upgrade >= 4 and
                // is currently using single pistol, upgrade them.
				if( i == SK_LIGHT_WEAPONS &&
                    oldskill < 4 &&
                    newInfo.skill[i] >= 4 &&
                    cgs.ccSelectedWeapon2 == 0)
                {
                    // Select akimbos
					CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 );
					CG_LimboPanel_SendSetupMsg( qfalse );
				}

                // If player got level >= 4 heavy, and is currently a soldier
                // set secondary to SMG
				if( i == SK_HEAVY_WEAPONS &&
                    newInfo.cls == PC_SOLDIER &&
                    oldskill < 4 &&
                    newInfo.skill[i] >= 4)
                {
						CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 2 );		// Selects SMG
						CG_LimboPanel_SendSetupMsg( qfalse );
				}

				CG_AddPMItemBig( PM_SKILL, va("Increased %s skill to level %i!", skillNames[i], newInfo.skill[i] ), cgs.media.skillPics[ i ], NULL );

				CG_PriorityCenterPrint( va( "You have been rewarded with %s", cg_skillRewards[ i ][ newInfo.skill[i]-1 ]), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH, 99999 );
			}
		}

		if( newInfo.team != cgs.clientinfo[ cg.clientNum ].team ) {
			// clear these
			memset( cg.artilleryRequestPos, 0, sizeof(cg.artilleryRequestPos) );
			memset( cg.artilleryRequestTime, 0, sizeof(cg.artilleryRequestTime) );
		}

		trap_Cvar_Set( "authLevel", va( "%i", newInfo.refStatus ) );

		if( newInfo.refStatus != ci->refStatus ) {
			if( newInfo.refStatus <= RL_NONE ) {
				const char *info = CG_ConfigString( CS_SERVERINFO );

				trap_Cvar_Set( "cg_ui_voteFlags", Info_ValueForKey(info, "voteFlags"));
				CG_Printf("[cgnotify]^3*** You have been stripped of your referee status! ***\n");

			} else {
				trap_Cvar_Set( "cg_ui_voteFlags", "0");
				CG_Printf("[cgnotify]^2*** You have been authorized \"%s\" status ***\n", ((newInfo.refStatus == RL_RCON) ? "rcon" : "referee"));
				CG_Printf("Type: ^3ref^7 (by itself) for a list of referee commands.\n");
			}
		}
	}


	// rain - passing the clientNum since that's all we need, and we
	// can't calculate it properly from the clientinfo
	CG_LoadClientInfo( clientNum );

	// replace whatever was there with the new one
	newInfo.infoValid = qtrue;
	*ci = newInfo;
#ifdef __SSYSTEM__
	CG_NewPCInfo( clientNum );
#endif
	// make sure we have a character set
	if( !ci->character ) {
		ci->character = BG_GetCharacter( ci->team, ci->cls );
	}

	// Gordon: need to resort the fireteam list, incase ranks etc have changed
	CG_SortClientFireteam();
}

#ifdef __SSYSTEM__
void CG_NewPCInfo( int clientNum ) {
	clientInfo_t *ci;
	playerCharacter_t	*newPC;
	const char	*configstring;
	const char	*v;

	ci = &cgs.clientinfo[clientNum];

	newPC = &ci->pc;

	configstring = CG_ConfigString( clientNum + CS_PCINFO );

	if ( !*configstring ) {
		memset( &ci->pc, 0, sizeof( ci->pc ) );
		return;		// player just left
	}

	memset( newPC, 0, sizeof( newPC ) );

	// body model
	v = Info_ValueForKey( configstring, "bd" );
	//CG_Printf( "%i\n", atoi(v) );
	newPC->bodyIndex = atoi(v);

	// head model
	v = Info_ValueForKey( configstring, "hd" );
	newPC->headIndex = atoi(v);
	// head skin
	v = Info_ValueForKey( configstring, "f" );
	newPC->faceIndex = atoi(v);
	// torso skin
	v = Info_ValueForKey( configstring, "t" );
	newPC->torsoIndex = atoi(v);
	// legs skin
	v = Info_ValueForKey( configstring, "l" );
	newPC->legIndex = atoi(v);
	// backpack model
	v = Info_ValueForKey( configstring, "bc" );
	newPC->backPackIndex = atoi(v);
	// backpack skin
	v = Info_ValueForKey( configstring, "bcs" );
	newPC->backPackSkinIndex = atoi(v);
	// left belt item model
	v = Info_ValueForKey( configstring, "bl" );
	newPC->bLeftIndex = atoi(v);
	// left belt item skin
	v = Info_ValueForKey( configstring, "bls" );
	newPC->bLeftSkinIndex = atoi(v);
	// right belt item model
	v = Info_ValueForKey( configstring, "br" );
	newPC->bRightIndex = atoi(v);
	// right belt item skin
	v = Info_ValueForKey( configstring, "brs" );
	newPC->bRightSkinIndex = atoi(v);
	// helmet model
	v = Info_ValueForKey( configstring, "he" );
	newPC->helmetIndex = atoi(v);
	// helmet skin
	v = Info_ValueForKey( configstring, "hs" );
	newPC->helmetSkinIndex = atoi(v);
	// glasses model
	v = Info_ValueForKey( configstring, "g" );
	newPC->glassIndex = atoi(v);
	// glasses skin
	v = Info_ValueForKey( configstring, "gs" );
	newPC->glassSkinIndex = atoi(v);
}
#endif

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/

bg_playerclass_t* CG_PlayerClassForClientinfo( clientInfo_t *ci, centity_t* cent) {
	int team, cls;

	if( cent && cent->currentState.eType == ET_CORPSE ) {
		return BG_GetPlayerClassInfo( cent->currentState.modelindex, cent->currentState.modelindex2 );
	}

	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
		team = ci->team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		// rain - fixed incorrect class determination (was & 6,
		// should be & 7)
		cls = (cent->currentState.powerups >> PW_OPS_CLASS_1) & 7;

		return BG_GetPlayerClassInfo( team, cls );
	}

	return BG_GetPlayerClassInfo( ci->team, ci->cls );
}

/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_SetLerpFrameAnimation( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t	*anim;

	bg_character_t* character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations ) {
		CG_Error( "CG_SetLerpFrameAnimation: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if( cg_debugAnim.integer) {// & 1 ) { // jP - make inclusive
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void CG_RunLerpFrame( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;

	// debugging tool to get no animations
	if( cg_animSpeed.integer == 0 ) {
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	// see if the animation sequence is switching
	if( ci && (newAnimation != lf->animationNumber || !lf->animation) ) {
		CG_SetLerpFrameAnimation( cent, ci, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if( cg.time < lf->animationTime ) {
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
				lf->frameTime = cg.time;

			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;

		if( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;

			if( cg_debugAnim.integer ) {
				CG_Printf( "Clamp lf->frameTime\n");
			}
		}
	}

	if( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	
	}

	if( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;

	}
	// calculate current lerp value
	if( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
CG_ClearLerpFrame
===============
*/
static void CG_ClearLerpFrame( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber ) {
	lf->frameTime = lf->oldFrameTime = cg.time;
	
	CG_SetLerpFrameAnimation( cent, ci, lf, animationNumber );
	if( lf->animation ) {
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}


/*
===============
CG_SetLerpFrameAnimationRate

may include ANIM_TOGGLEBIT
===============
*/
void
CG_SetLerpFrameAnimationRate ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation )
{
	animation_t		*anim, *oldanim;
	int				transitionMin = -1, oldAnimTime, oldAnimNum;
	qboolean		firstAnim = qfalse;
	bg_character_t	*character = CG_CharacterForClientinfo( ci, cent );
	if ( !character )
	{
		return;
	}

	oldAnimTime = lf->animationTime;
	oldanim = lf->animation;
	oldAnimNum = lf->animationNumber;
	if ( !lf->animation )
	{
		firstAnim = qtrue;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations )
	{
		CG_Error( "CG_SetLerpFrameAnimationRate: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if ( !(anim->flags & ANIMFL_FIRINGANIM) || (lf != &cent->pe.torso) )
	{
		if
		(
			(lf == &cent->pe.legs) &&
			(
				CG_IsCrouchingAnim( character->animModelInfo, newAnimation) != CG_IsCrouchingAnim( character->animModelInfo,
																								   oldAnimNum)
			)
		)
		{
			if ( anim->moveSpeed || (anim->movetype & ((1 << ANIM_MT_TURNLEFT) | (1 << ANIM_MT_TURNRIGHT))) )
			{											// if unknown movetype, go there faster
				transitionMin = lf->frameTime + 200;	// slowly raise/drop
			}
			else
			{
				transitionMin = lf->frameTime + 350;	// slowly raise/drop
			}
		}
		else if ( anim->moveSpeed )
		{
			transitionMin = lf->frameTime + 120;		// always do some lerping (?)
		}
		else
		{												// not moving, so take your time
			transitionMin = lf->frameTime + 170;		// always do some lerping (?)
		}

		if ( oldanim && oldanim->animBlend )
		{												//transitionMin < lf->frameTime + oldanim->animBlend) {
			transitionMin = lf->frameTime + oldanim->animBlend;
			lf->animationTime = transitionMin;
		}
		else
		{

			// slow down transitions according to speed
			if ( anim->moveSpeed && lf->animSpeedScale < 1.0 )
			{
				lf->animationTime += anim->initialLerp;
			}

			if ( lf->animationTime < transitionMin )
			{
				lf->animationTime = transitionMin;
			}
		}
	}

	// if first anim, go immediately
	if ( firstAnim )
	{
		lf->frameTime = cg.time - 1;
		lf->animationTime = cg.time - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
	}

	if ( cg_debugAnim.integer == 1 )
	{													// DHM - Nerve :: extra debug info
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}


/*
===============
CG_RunLerpFrameRate

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void
CG_RunLerpFrameRate ( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, centity_t *cent, int recursion )
{
	int			f;
	animation_t *anim, *oldAnim;
	animation_t *otherAnim = NULL;
	qboolean	isLadderAnim;
#define ANIM_SCALEMAX_LOW	1.1
#define ANIM_SCALEMAX_HIGH	1.6
#define ANIM_SPEEDMAX_LOW	100
#define ANIM_SPEEDMAX_HIGH	20

	// debugging tool to get no animations
	if ( cg_animSpeed.integer == 0 )
	{
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	isLadderAnim = lf->animation && ( lf->animation->flags & ANIMFL_LADDERANIM );
	oldAnim = lf->animation;

	// see if the animation sequence is switching
	if ( newAnimation != lf->animationNumber || !lf->animation )
	{
		CG_SetLerpFrameAnimationRate( cent, ci, lf, newAnimation );
	}

	// Ridah, make sure the animation speed is updated when possible
	anim = lf->animation;

	// check for forcing last frame
	if (	/*cent->currentState.eFlags & EF_FORCE_END_FRAME */
			// xkan, 12/27/2002 - In SP, corpse also stays at the last frame (of the death animation)
			// so that the death animation can end up in different positions
			// and the body will stay in that position
			/*||*/
				( /*CG_IsSinglePlayer() &&*/ cent->currentState.eType == ET_CORPSE)
		)
	{
		lf->oldFrame = lf->frame = anim->firstFrame + anim->numFrames - 1;
		lf->oldFrameModel = lf->frameModel = anim->mdxFile;
		lf->backlerp = 0;
		return;
	}

	if ( anim->moveSpeed && lf->oldFrameSnapshotTime )
	{
		float	moveSpeed;

		// calculate the speed at which we moved over the last frame
		if ( cg.latestSnapshotTime != lf->oldFrameSnapshotTime && cg.nextSnap )
		{
			if ( cent->currentState.number == cg.snap->ps.clientNum )
			{
				if ( isLadderAnim )
				{								// only use Z axis for speed
					lf->oldFramePos[0] = cent->lerpOrigin[0];
					lf->oldFramePos[1] = cent->lerpOrigin[1];
				}
				else
				{								// only use x/y axis
					lf->oldFramePos[2] = cent->lerpOrigin[2];
				}

				moveSpeed = Distance( cent->lerpOrigin, lf->oldFramePos ) / ( (float) (cg.time - lf->oldFrameTime) / 1000.0 );
			}
			else
			{
				if ( isLadderAnim )
				{								// only use Z axis for speed
					lf->oldFramePos[0] = cent->currentState.pos.trBase[0];
					lf->oldFramePos[1] = cent->currentState.pos.trBase[1];
				}

				moveSpeed = Distance( cent->lerpOrigin, lf->oldFramePos ) / ( (float) (cg.time - lf->oldFrameTime) / 1000.0 );
			}

			//
			// convert it to a factor of this animation's movespeed
			lf->animSpeedScale = moveSpeed / (float) anim->moveSpeed;
			lf->oldFrameSnapshotTime = cg.latestSnapshotTime;
		}
	}
	else
	{

		// move at normal speed
		lf->animSpeedScale = 1.0;
		lf->oldFrameSnapshotTime = cg.latestSnapshotTime;
	}

	// adjust with manual setting (pain anims)
	lf->animSpeedScale *= cent->pe.animSpeed;

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime )
	{
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;
		VectorCopy( cent->lerpOrigin, lf->oldFramePos );

		// restrict the speed range
		if ( lf->animSpeedScale < 0.25 )
		{										// if it's too slow, then a really slow spped, combined with a sudden take-off, can leave them playing a really slow frame while they a moving really fast
			if ( lf->animSpeedScale < 0.01 && isLadderAnim )
			{
				lf->animSpeedScale = 0.0;
			}
			else
			{
				lf->animSpeedScale = 0.25;
			}
		}
		else if ( lf->animSpeedScale > ANIM_SCALEMAX_LOW )
		{
			if ( !(anim->flags & ANIMFL_LADDERANIM) )
			{

				// allow slower anims to speed up more than faster anims
				if ( anim->moveSpeed > ANIM_SPEEDMAX_LOW )
				{
					lf->animSpeedScale = ANIM_SCALEMAX_LOW;
				}
				else if ( anim->moveSpeed < ANIM_SPEEDMAX_HIGH )
				{
					if ( lf->animSpeedScale > ANIM_SCALEMAX_HIGH )
					{
						lf->animSpeedScale = ANIM_SCALEMAX_HIGH;
					}
				}
				else
				{
					lf->animSpeedScale = ANIM_SCALEMAX_HIGH - ( ANIM_SCALEMAX_HIGH - ANIM_SCALEMAX_LOW ) * (float) ( anim->moveSpeed - ANIM_SPEEDMAX_HIGH ) / (float) ( ANIM_SPEEDMAX_LOW - ANIM_SPEEDMAX_HIGH );
				}
			}
			else if ( lf->animSpeedScale > 4.0 )
			{
				lf->animSpeedScale = 4.0;
			}
		}

		if ( lf == &cent->pe.legs )
		{
			otherAnim = cent->pe.torso.animation;
		}
		else if ( lf == &cent->pe.torso )
		{
			otherAnim = cent->pe.legs.animation;
		}

		// get the next frame based on the animation
		if ( !lf->animSpeedScale )
		{

			// stopped on the ladder, so stay on the same frame
			f = lf->frame - anim->firstFrame;
			lf->frameTime += anim->frameLerp;	// don't wait too long before starting to move again
		}
		else if
			(
				lf->oldAnimationNumber != lf->animationNumber &&
				(
					!anim->moveSpeed ||
					lf->oldFrame < anim->firstFrame ||
					lf->oldFrame >= anim->firstFrame +
					anim->numFrames
				)
			)
		{										// Ridah, added this so walking frames don't always get reset to 0, which can happen in the middle of a walking anim, which looks wierd
			lf->frameTime = lf->animationTime;	// initial lerp
			if ( oldAnim && anim->moveSpeed )
			{									// keep locomotions going continuously
				f = ( lf->frame - oldAnim->firstFrame ) + 1;
				while ( f < 0 )
				{
					f += anim->numFrames;
				}
			}
			else
			{
				f = 0;
			}
		}
		else if
			(
				(lf == &cent->pe.legs) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (cent->pe.torso.animationNumber &~ANIM_TOGGLEBIT)) &&
				(!anim->moveSpeed)
			)
		{

			// legs should synch with torso
			f = cent->pe.torso.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = cent->pe.torso.frameTime;
		}
		else if
			(
				(lf == &cent->pe.torso) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (cent->pe.legs.animationNumber &~ANIM_TOGGLEBIT)) &&
				(otherAnim->moveSpeed)
			)
		{

			// torso needs to sync with legs
			f = cent->pe.legs.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = cent->pe.legs.frameTime;
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + (int) ( (float) anim->frameLerp * (1.0 / lf->animSpeedScale) );
			if ( lf->frameTime < cg.time )
			{
				lf->frameTime = cg.time;
			}

			// check for skipping frames (eg. death anims play in slo-mo if low framerate)
			if ( anim->flags & ANIMFL_REVERSED )
			{
				if ( cg.time > lf->frameTime && !anim->moveSpeed )
				{
					f = ( anim->numFrames - 1 ) - ( (lf->frame - anim->firstFrame) - (1 + (cg.time - lf->frameTime) / anim->frameLerp) );
				}
				else
				{
					f = ( anim->numFrames - 1 ) - ( (lf->frame - anim->firstFrame) - 1 );
				}
			}
			else
			{
				if ( cg.time > lf->frameTime && !anim->moveSpeed )
				{
					f = ( lf->frame - anim->firstFrame ) + 1 + ( cg.time - lf->frameTime ) / anim->frameLerp;
				}
				else
				{
					f = ( lf->frame - anim->firstFrame ) + 1;
				}
			}

			if ( f < 0 )
			{
				f = 0;
			}
		}

		//f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames )
		{
			f -= anim->numFrames;
			if ( anim->loopFrames )
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}

		if ( anim->flags & ANIMFL_REVERSED )
		{
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
			lf->frameModel = anim->mdxFile;
		}
		else
		{
			lf->frame = anim->firstFrame + f;
			lf->frameModel = anim->mdxFile;
		}

		if ( cg.time > lf->frameTime )
		{

			// Ridah, run the frame again until we move ahead of the current time, fixes walking speeds for zombie
			if ( /*!anim->moveSpeed ||*/ recursion > 4 )
			{
				lf->frameTime = cg.time;
			}
			else
			{
				CG_RunLerpFrameRate( ci, lf, newAnimation, cent, recursion + 1 );
			}

			if ( cg_debugAnim.integer > 3 )
			{
				CG_Printf( "Clamp lf->frameTime\n" );
			}
		}

		lf->oldAnimationNumber = lf->animationNumber;
	}

	// Gordon: BIG hack, occaisionaly (VERY occaisionally), the frametime gets totally wacked
	if ( lf->frameTime > cg.time + 5000 )
	{
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time )
	{
		lf->oldFrameTime = cg.time;
	}

	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime )
	{
		lf->backlerp = 0;
	}
	else
	{
		lf->backlerp = 1.0 - (float) ( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
CG_ClearLerpFrameRate
===============
*/
void
CG_ClearLerpFrameRate ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimationRate( cent, ci, lf, animationNumber );
	if ( lf->animation )
	{
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}
/*
===============
CG_SetLerpFrameAnimationRate

may include ANIM_TOGGLEBIT
===============
*/
void CG_SetLerpFrameAnimationRateCorpse( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t		*anim;


	bg_character_t *character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations ) {
		CG_Error( "CG_SetLerpFrameAnimationRate: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	lf->frameTime = cg.time - 1;
	lf->animationTime = cg.time - 1;
	lf->frame = anim->firstFrame;
	lf->frameModel = anim->mdxFile;


	// jP - make non-mutually exclusive
	if( cg_debugAnim.integer) {// & 1 ) {			// DHM - Nerve :: extra debug info
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}

/*
===============
CG_RunLerpFrameRateCorpse

  Simplifed code for corpses
===============
*/
void CG_RunLerpFrameRateCorpse( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, centity_t *cent, int recursion ) {
	int			f;
	animation_t	*anim, *oldAnim;

	oldAnim = lf->animation;


	// see if the animation sequence is switching
	if( newAnimation != lf->animationNumber || !lf->animation) {
		CG_SetLerpFrameAnimationRateCorpse( cent, ci, lf, newAnimation );
	}

	// Ridah, make sure the animation speed is updated when possible
	anim = lf->animation;

	// animation time gone, fixme dont use with all anims, short animations are sometimes played twice even they dont need it
	if( cent->currentState.effect1Time  < cg.time  ) {
			lf->oldFrame = lf->frame = anim->firstFrame + anim->numFrames - 1;
			lf->oldFrameModel = lf->frameModel = anim->mdxFile;
			lf->backlerp = 0;
			return;
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if ( cg.time < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;
			if ( cg_debugAnim.integer ) {
				CG_Printf( "Clamp lf->frameTime\n");
			}
		}
	}

	if ( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}


}

/*
===============
CG_PlayerAnimation
===============
*/
static void CG_PlayerAnimation( centity_t *cent, refEntity_t *body ) {
	clientInfo_t	*ci;
	int				clientNum;
	int				animIndex, tempIndex;
	bg_character_t	*character;

	clientNum = cent->currentState.clientNum;

	ci = &cgs.clientinfo[ clientNum ];
	character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	if( cg_noPlayerAnims.integer ) {
		body->frame = body->oldframe = body->torsoFrame = body->oldTorsoFrame = 0;
		body->frameModel = body->oldframeModel = body->torsoFrameModel= body->oldTorsoFrameModel = character->animModelInfo->animations[0]->mdxFile;
		return;
	}

	// default to whatever the legs are currently doing
	animIndex = cent->currentState.legsAnim;

	// do the shuffle turn frames locally
	if( !(cent->currentState.eFlags & EF_DEAD) && cent->pe.legs.yawing ) {
		//CG_Printf("turn: %i\n", cg.time );
		tempIndex = BG_GetAnimScriptAnimation( clientNum, character->animModelInfo, cent->currentState.aiState, (cent->pe.legs.yawing == SWING_RIGHT ? ANIM_MT_TURNRIGHT : ANIM_MT_TURNLEFT) );
		if (tempIndex > -1) {
			animIndex = tempIndex;
		}
	}

	// run the animation
	if ( cent->currentState.eType == ET_CORPSE )
	CG_RunLerpFrameRateCorpse( ci, &cent->pe.legs, animIndex, cent, 0 );
		else 
	CG_RunLerpFrameRate( ci, &cent->pe.legs, animIndex, cent, 0 );


	body->oldframe = cent->pe.legs.oldFrame;
	body->frame = cent->pe.legs.frame;
	body->backlerp = cent->pe.legs.backlerp;
	body->frameModel = cent->pe.legs.frameModel;
	body->oldframeModel = cent->pe.legs.oldFrameModel;

	if ( cent->currentState.eType == ET_CORPSE )
	CG_RunLerpFrameRateCorpse( ci, &cent->pe.torso, cent->currentState.torsoAnim, cent, 0 );
		else 
	CG_RunLerpFrameRate( ci, &cent->pe.torso, cent->currentState.torsoAnim, cent, 0 );


	body->oldTorsoFrame = cent->pe.torso.oldFrame;
	body->torsoFrame = cent->pe.torso.frame;
	body->torsoBacklerp = cent->pe.torso.backlerp;
	body->torsoFrameModel = cent->pe.torso.frameModel;
	body->oldTorsoFrameModel = cent->pe.torso.oldFrameModel;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/

/*
==================
CG_SwingAngles
==================
*/
static void CG_SwingAngles( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging ) {
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		float centerAngle;

		// zinx - use predictable center so server can match cgame easier
		centerAngle = rint(*angle / swingTolerance) * swingTolerance;

		swing = AngleSubtract( destination, *angle );
		if ( swing >= swingTolerance || swing < -swingTolerance ) {
			*swinging = qtrue;
		}
	}

	if ( !*swinging ) {
		return;
	}
	
	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	scale *= 0.05;
	if (scale < 0.5)
		scale = 0.5;

	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = cg.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_LEFT;		// left
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_RIGHT;	// right
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) {
		*angle = AngleMod( destination - (clampTolerance - 1) );
	} else if ( swing < -clampTolerance ) {
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}

#if 0 // unused
static void CG_SwingAngles_Old( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging ) {
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance ) {
			*swinging = qtrue;
		}
	}

	if ( !*swinging ) {
		return;
	}
	
	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	scale *= 0.05;
	if (scale < 0.5)
		scale = 0.5;

	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = cg.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_LEFT;		// left
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_RIGHT;	// right
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) {
		*angle = AngleMod( destination - (clampTolerance - 1) );
	} else if ( swing < -clampTolerance ) {
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}
#endif
/*
=================
CG_AddPainTwitch
=================
*/
static void CG_AddPainTwitch( centity_t *cent, vec3_t torsoAngles ) {
	int		t;
	float	f;
	int		duration;
	float	direction;

	if( !cent->pe.animSpeed ) {
		// we need to inititialize this stuff
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
	}

	if( cent->currentState.eFlags & EF_DEAD ) {
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
		return;
	}

	if( cent->pe.painDuration ) {
		duration = cent->pe.painDuration;
	} else {
		duration = PAIN_TWITCH_TIME;
	}
	direction = (float)duration * 0.085;
	if (direction > 30)
		direction = 30;
	if (direction < 10)
		direction = 10;
	direction *= (float)(cent->pe.painDirection*2) - 1;

	t = cg.time - cent->pe.painTime;
	if( t >= duration ) {
		return;
	}

	f = 1.0 - (float)t / duration;
	if( cent->pe.painDirection ) {
		torsoAngles[ROLL] += 20 * f;
	} else {
		torsoAngles[ROLL] -= 20 * f;
	}
}

#define LEAN_MAX	28.0f
#define LEAN_TIME_TO	200.0f	// time to get to/from full lean
#define LEAN_TIME_FR	300.0f	// time to get to/from full lean

void CG_PredictLean(centity_t *cent, vec3_t	torsoAngles, vec3_t	headAngles,int viewHeight) {
	vec3_t		start, end, tmins, tmaxs, right;
	int			leaning = 0;	// -1 left, 1 right
	float		leanofs = 0;
	vec3_t		viewangles;
	trace_t		trace;
	int			time;


	if(cent->currentState.constantLight < 0)
		leaning -= 1;
	if(cent->currentState.constantLight > 0)
		leaning += 1;

// note not really needed, just for better prediction
	if(	BG_PlayerMounted(cent->currentState.eFlags) ) {
		leaning = 0;	// leaning not allowed on mg42
	}

	if(cent->currentState.eFlags & EF_FIRING /*&& (!(cgs.lean & LEAN_VISIBLE)||!(cgs.lean & LEAN_SHOOT)) cgs.clientinfo[cent->currentState.clientNum].skill[SK_LIGHT_WEAPONS] < 9*/ )
		leaning = 0;	// not allowed to lean while firing

	if(cent->currentState.eFlags & EF_DEAD)
		leaning = 0;	// not allowed to lean while dead

	if( cent->currentState.eFlags & EF_PRONE || cent->currentState.weapon == WP_MORTAR_SET || cent->currentState.weapon == WP_MORTAR2_SET )
		leaning = 0;	// not allowed to lean while prone
//

	leanofs = cent->pe.leanDirection;

	if ( leaning != cent->pe.leanDir ){
		cent->pe.leanTime = cg.time;
		cent->pe.leanDir = leaning;
	}

	time = cg.time - cent->pe.leanTime;

	if ( time < 1 )
	{
		time = 1;
	}
	else if ( time > 200 )
	{
		time = 200;
	}

	cent->pe.leanTime = cg.time;


	if(!leaning) {	// go back to center position
		if ( leanofs > 0 ) {		// right
			leanofs -= (((float)time/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs < 0 )
				leanofs = 0;
		}
		else if ( leanofs < 0 ) {	// left
			leanofs += (((float)time/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs > 0 )
				leanofs = 0;
		}
	}

	if(leaning) 
	{
		if(leaning > 0) // right
		{			
			if(leanofs < LEAN_MAX)
				leanofs += (((float)time/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs > LEAN_MAX)
				leanofs = LEAN_MAX;
		}
		else			// left
		{

			if(leanofs > -LEAN_MAX)
				leanofs -= (((float)time/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs < -LEAN_MAX)
				leanofs = -LEAN_MAX;
		}
	}


	cent->pe.leanDirection = leanofs;

	if(leaning){
		VectorCopy( cent->lerpOrigin, start );
		start[2] += viewHeight;

		VectorCopy( cent->lerpAngles, viewangles );
		viewangles[ROLL] += leanofs/2.0f;
		AngleVectors( viewangles, NULL, right, NULL );
		VectorMA( start, leanofs, right, end );		
		
		VectorSet( tmins, -8, -8, -7 ); // ATVI Wolfenstein Misc #472, bumped from -4 to cover gun clipping issue
		VectorSet( tmaxs, 8, 8, 4 );

		CG_Trace( &trace, start, tmins, tmaxs, end, cent->currentState.clientNum, MASK_PLAYERSOLID );

		cent->pe.leanDirection *= trace.fraction;		
	}

	if ( torsoAngles )
	torsoAngles[ROLL] += cent->pe.leanDirection *1.25;
	if ( headAngles )
	headAngles[ROLL] += cent->pe.leanDirection; //* 0.75 ;



}

/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
//extern float PlayerLeanValues[MAX_CLIENTS];

static void CG_PlayerAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t			legsAngles, torsoAngles, headAngles;
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	int				legsSet, torsoSet;
	clientInfo_t	*ci;
	bg_character_t	*character;
	centity_t		*cgsnap;	//jP
//	int				leanValue;	// jP
//	float			leanScale = 0.0f;

	ci = &cgs.clientinfo[ cent->currentState.clientNum ];

	character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	legsSet = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	torsoSet = cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT;

	VectorCopy( cent->lerpAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );

	// jaquboss
	if ( cent->currentState.eType == ET_CORPSE )
		headAngles[0] = headAngles[2] = 0;

	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------

	// allow yaw to drift a bit, unless these conditions don't allow them
	if( !(BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE ) ||
		  BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLECR ) )/*
		||	 (BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFELEFT) | (1<<ANIM_MT_STRAFERIGHT)) )*/) {

		// always point all in the same direction
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
		cent->pe.legs.yawing = qtrue;	// always center

	// if firing, make sure torso and head are always aligned
	} else if( BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_FIRING, qtrue ) ) {
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
	}

	// adjust legs for movement dir
	if( cent->currentState.eFlags & EF_DEAD || cent->currentState.eFlags & EF_MOUNTEDTANK ) {
		// don't let dead bodies twitch
		legsAngles[YAW] = headAngles[YAW];
		torsoAngles[YAW] = headAngles[YAW];
	} else {
		legsAngles[YAW] = headAngles[YAW] + cent->currentState.angles2[YAW];

		if( !(cent->currentState.eFlags & EF_FIRING) ) {
			torsoAngles[YAW] = headAngles[YAW] + 0.35 * cent->currentState.angles2[YAW];
			clampTolerance = 90;
		} else {	// must be firing
			torsoAngles[YAW] = headAngles[YAW];	// always face firing direction
			//if (fabs(cent->currentState.angles2[YAW]) > 30)
			//	legsAngles[YAW] = headAngles[YAW];
			clampTolerance = 60;
		}

		// torso
		CG_SwingAngles( torsoAngles[YAW], 25, clampTolerance, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );

		// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
		// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy

		clampTolerance = 150;

		if( BG_GetConditionBitFlag( ci->clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) ) {
			cent->pe.legs.yawing = qfalse; // set it if they really need to swing
			CG_SwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5*cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
		} else if( strstr( BG_GetAnimString( character->animModelInfo, legsSet ), "strafe" ) ) {
		// FIXME: what is this strstr hack??
		//if	( BG_GetConditionValue( ci->clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFERIGHT)|(1<<ANIM_MT_STRAFELEFT)) )
			cent->pe.legs.yawing = qfalse; // set it if they really need to swing
			legsAngles[YAW] = headAngles[YAW];
			CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
		} else if(cent->pe.legs.yawing) {
			CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
		} else {
			CG_SwingAngles( legsAngles[YAW], 40, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
		}

		torsoAngles[YAW] = cent->pe.torso.yawAngle;
		legsAngles[YAW] = cent->pe.legs.yawAngle;
	}

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	//CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
	//torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

	if ( BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_REVIVE))
	{
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
		if ( headAngles[PITCH] - torsoAngles[PITCH] > 25)
		headAngles[PITCH] = torsoAngles[PITCH]+25;
		if ( torsoAngles[PITCH] - headAngles[PITCH] > 25 )
		headAngles[PITCH] = torsoAngles[PITCH]-25;
	} else if( cent->currentState.eFlags & EF_PRONE  ) {
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	} else {
		CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
		torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	}


	// --------- roll -------------


	// lean towards the direction of travel
	VectorCopy( cent->currentState.pos.trDelta, velocity );
	speed = VectorNormalize( velocity );

	if( speed )
	{
		vec3_t	axis[3];
		float	side;

		speed *= 0.05;

		AnglesToAxis( legsAngles, axis );
		side = speed * DotProduct( velocity, axis[1] );
		legsAngles[ROLL] -= side;

		side = speed * DotProduct( velocity, axis[0] );
		legsAngles[PITCH] += side;
	}
	
//	leanValue = BG_GetConditionValue( ci->clientNum, ANIM_COND_LEANING, qtrue);

    // jet Pilot - add leaning animation - modified from FalkonET
/*	torsoAngles[ROLL] += (cent->currentState.constantLight) ;
	headAngles[ROLL] += (cent->currentState.constantLight) * 0.75 ;
*/
//	if ( cgs.lean & LEAN_VISIBLE )
	CG_PredictLean( cent, torsoAngles, headAngles, cg.snap->ps.clientNum == cent->currentState.clientNum ? cg.snap->ps.viewheight :  cent->pe.headRefEnt.origin[2] );

	// pain twitch
	CG_AddPainTwitch( cent, torsoAngles );

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );

	// jet Pilot - moved this from CG_Player
	cgsnap = &cg_entities[cg.snap->ps.clientNum];

	if(	cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
		memcpy( torso, legs, sizeof(torso) );
}

/*
==============
CG_BreathPuffs
==============
*/
static void CG_BreathPuffs( centity_t *cent, refEntity_t *head ) {
	clientInfo_t	*ci;
	vec3_t			up, forward;
	int				contents;
	vec3_t			mang, morg, maxis[3];

	ci = &cgs.clientinfo[ cent->currentState.number ];

	/*if( !cg_enableBreath.integer ) {
		return;
	}*/

	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		return;
	}

	if( !(cent->currentState.eFlags & EF_DEAD) ) {
		return;
	}

	// allow cg_enableBreath to force everyone to have breath
	if( !(cent->currentState.eFlags & EF_BREATH) ) {
		return;
	}

	contents = CG_PointContents( head->origin, 0 );
	if( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}
	if( ci->breathPuffTime > cg.time ) {
		return;
	}

	CG_GetOriginForTag( cent, head, "tag_mouth", 0, morg, maxis );
	AxisToAngles( maxis, mang );
	AngleVectors( mang, forward, NULL, up );

	//push the origin out a tad so it's not right in the guys face (tad==4)
	VectorMA(morg, 4, forward, morg);

	forward[0] = up[0] * 8 + forward[0] * 5;
	forward[1] = up[1] * 8 + forward[1] * 5;
	forward[2] = up[2] * 8 + forward[2] * 5;

	CG_SmokePuff( morg, forward, 4, 1, 1, 1, 0.5f, 2000, cg.time, cg.time + 400, 0, cgs.media.shotgunSmokePuffShader );

	ci->breathPuffTime = cg.time + 3000 + random()*1000;
}

/*
===============
CG_TrailItem
===============
*/
/*static void CG_TrailItem( centity_t *cent, qhandle_t hModel ) {
	refEntity_t		ent;
	vec3_t			angles;
	qboolean		ducking;

	// DHM - Nerve :: Don't draw icon above your own head
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
		return;

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpAngles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AnglesToAxis( angles, ent.axis );

	// DHM - Nerve :: adjusted values
	VectorCopy( cent->lerpOrigin, ent.origin );

	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
		ducking = (cg.snap->ps.pm_flags & PMF_DUCKED);
	else
		ducking = (qboolean)cent->currentState.animMovetype;

	if ( ducking )
		ent.origin[2] += 38;
	else
		ent.origin[2] += 56;

	ent.hModel = hModel;
	trap_R_AddRefEntityToScene( &ent );
}*/

/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
DHM - Nerve :: added height parameter
===============
*/

qboolean CG_WorldCoordToScreenCoordFloat ( vec3_t worldCoord, float *x, float *y )
{
	float	xcenter, ycenter;
	vec3_t	local, transformed;
	vec3_t	vfwd;
	vec3_t	vright;
	vec3_t	vup;
	float	xzi;
	float	yzi;

	//	xcenter = cg.refdef.width / 2;//gives screen coords adjusted for resolution
	//	ycenter = cg.refdef.height / 2;//gives screen coords adjusted for resolution
	//NOTE: did it this way because most draw functions expect virtual 640x480 coords
	//	and adjust them for current resolution
	xcenter = 640.0f / 2.0f;	//gives screen coords in virtual 640x480, to be adjusted when drawn
	ycenter = 480.0f / 2.0f;	//gives screen coords in virtual 640x480, to be adjusted when drawn
	AngleVectors( cg.refdefViewAngles, vfwd, vright, vup );
	VectorSubtract( worldCoord, cg.refdef.vieworg, local );
	transformed[0] = DotProduct( local, vright );
	transformed[1] = DotProduct( local, vup );
	transformed[2] = DotProduct( local, vfwd );

	// Make sure Z is not negative.
	if ( transformed[2] < 0.01f )
	{
		return qfalse;
	}

	xzi = xcenter / transformed[2] * ( 96.0f / cg.refdef.fov_x );
	yzi = ycenter / transformed[2] * ( 102.0f / cg.refdef.fov_y );
	*x = xcenter + xzi * transformed[0];
	*y = ycenter - yzi * transformed[1];
	return qtrue;
}

void CG_AddOnScreenText ( const char *text, vec3_t origin )
{
	float x,y;

	if ( cg.specOnScreenNamesNum >= 16 )
		return;
	
		if (CG_WorldCoordToScreenCoordFloat( origin, &x, &y )){
			float		dist, scale, w, h;

			dist = VectorDistance(origin, cg.refdef_current->vieworg);

			if ( dist > 1500)
				dist = 1500;

			scale = 0.5f - (dist/3000 );
			if ( scale < 0.125f)
				scale = 0.125f;

	
			w = CG_Text_Width_Ext(text, scale, 0, &cgs.media.limboFont1 );
			h = CG_Text_Height_Ext(text, scale, 0, &cgs.media.limboFont1 );

			x -= w/2;
			y -= h/2;

			// save it
			cg.specOnScreenNames[cg.specOnScreenNamesNum].x = x;
			cg.specOnScreenNames[cg.specOnScreenNamesNum].y = y;
			cg.specOnScreenNames[cg.specOnScreenNamesNum].scale = scale;
			cg.specOnScreenNames[cg.specOnScreenNamesNum].text = text;
			VectorCopy( origin, cg.specOnScreenNames[cg.specOnScreenNamesNum].origin);
			cg.specOnScreenNamesNum++; 
		} 
}

static void CG_PlayerFloatText( centity_t *cent, const char *text, int height ) 
{
	vec3_t		origin;


	VectorCopy( cent->lerpOrigin, origin );
	origin[2] += height;			// DHM - Nerve :: was '48'


	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
	{
		if( cg.snap->ps.pm_flags & PMF_DUCKED )
		{
			origin[2] -= 18;
		}
	}
	else
	{
		if( (qboolean)cent->currentState.animMovetype )
		{
			origin[2] -= 18;
		}
	}

	CG_AddOnScreenText( text, origin );

}

static void CG_PlayerFloatSprite( centity_t *cent, qhandle_t shader, int height, int off ) 
{
	int				rf;
	vec3_t			right;
	refEntity_t		ent;
	int				hPos[]={0,-13,13, 
							0,-13,13,
							0,-13,13};
	int				vPos[]={0,0,0,
							13,13,13,
							26,26,26};

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) 
	{
		rf = RF_THIRD_PERSON;		// only show in mirrors
	}
	else
	{
		rf = 0;
	}

	memset( &ent, 0, sizeof( ent ) );
	VectorCopy( cent->lerpOrigin, ent.origin );
	ent.origin[2] += height;			// DHM - Nerve :: was '48'

	AngleVectors( cg.refdefViewAngles, NULL, right, NULL);

	if ( off > 8 ) // I haven't coded pattern for more than 9, note current code have 8 sprites...
		return; 

	// jaquboss move it!
	ent.origin[2] += vPos[off];
	VectorMA(ent.origin, hPos[off], right , ent.origin) ;


	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
	{
		if( cg.snap->ps.pm_flags & PMF_DUCKED )
			ent.origin[2] -= 18;
	}
	else
	{
		if( (qboolean)cent->currentState.animMovetype )
			ent.origin[2] -= 18;
	}

	ent.reType			= RT_SPRITE;
	ent.customShader	= shader;
	ent.radius			= 6.66;
	ent.renderfx		= rf;
	ent.shaderRGBA[0] 	= 255;
	ent.shaderRGBA[1] 	= 255;
	ent.shaderRGBA[2] 	= 255;
	ent.shaderRGBA[3] 	= 255;

	trap_R_AddRefEntityToScene( &ent );
}



/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
*/
static void CG_PlayerSprites( centity_t *cent ) 
{
	int				team;
	int				numIcons = 0;
	clientInfo_t	*ci;
	qboolean		sameTeam;
	qboolean		disguise=qfalse;
	int				height = 48;

	ci		 = &cgs.clientinfo[cent->currentState.clientNum];


	if( cent->currentState.powerups & ( 1 << PW_REDFLAG ) || cent->currentState.powerups & ( 1 << PW_BLUEFLAG ) ) 
		CG_PlayerFloatSprite( cent, cgs.media.objectiveShader, 56, numIcons++ );

	if ( cent->currentState.eFlags & EF_DEAD )
		height = 8;

	if( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR && cgs.spectator & SPEC_SHOWNAMES  ) 
	{
		CG_PlayerFloatText( cent, ci->name, height+16 );
		return;
	}

	if ( cent->currentState.powerups & (1<<PW_INVULNERABLE) ) 
		CG_PlayerFloatSprite( cent, cgs.media.spawnInvincibleShader, 8+height, numIcons++ );

	team = ci->team;
	sameTeam = (cg.snap->ps.persistant[PERS_TEAM] == team);

	// jet Pilot - level 8 and above covert ops can chat to enemy team as well
	if (!sameTeam && cent->currentState.powerups & (1<<PW_OPS_DISGUISED) && ci->skill[SK_COVERTOPS] >= 8 )
		disguise = qtrue;//sameTeam = qtrue;

	// DHM - Nerve :: If this client is a medic, draw a 'revive' icon over dead players that are not in limbo yet.
	if( (cent->currentState.eFlags & EF_DEAD)
		&& cent->currentState.number == cent->currentState.clientNum
		&& cg.snap->ps.stats[ STAT_HEALTH ] > 0
		&& sameTeam )
	{
		if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC)
			CG_PlayerFloatSprite( cent, cgs.media.medicReviveShader, height, numIcons++ );
		else if ( cgs.clientinfo[cg.clientNum].skill[SK_MEDIC] >= 9)
			CG_PlayerFloatSprite( cent, cgs.media.resuscitateShader, height, numIcons++ );
	}

	if ( cent->currentState.eFlags & EF_CONNECTION && sameTeam  ) 
		CG_PlayerFloatSprite( cent, cgs.media.disconnectIcon, height, numIcons++ );

	if ( cent->currentState.eFlags & EF_POISONED /*&& sameTeam*/  ) 
		CG_PlayerFloatSprite( cent, cgs.media.poisonedIcon, height , numIcons++ );

	// DHM - Nerve :: show voice chat signal so players know who's talking
	if( cent->voiceChatSpriteTime > cg.time && ( sameTeam || disguise ) )
		CG_PlayerFloatSprite( cent, cent->voiceChatSprite, height+8, numIcons++ );

	// DHM - Nerve :: only show talk icon to team-mates
	if( cent->currentState.eFlags & EF_TALK && sameTeam )
		CG_PlayerFloatSprite( cent, cgs.media.balloonShader, height, numIcons++ );	

	// jet Pilot - draw disguised icon over disguised teammates
	if ( cent->currentState.powerups & (1<<PW_OPS_DISGUISED) && cg.snap->ps.persistant[PERS_TEAM] == team )
		CG_PlayerFloatSprite( cent, cgs.media.disguiseShader, height+8, numIcons++ );

	{
		fireteamData_t* ft;
		if ((ft = CG_IsOnFireteam( cent->currentState.number ))) 
		{
			if( ft == CG_IsOnFireteam( cg.clientNum ) && cgs.clientinfo[ cent->currentState.number ].selected ) 
			{
				CG_PlayerFloatSprite( cent, cgs.media.fireteamicons[ft->ident], 56, numIcons++ );				
			}
		}		
	}
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define	SHADOW_DISTANCE		64
#define	ZOFS	6.0
#define	SHADOW_MIN_DIST	250.0
#define	SHADOW_MAX_DIST	512.0

typedef struct {
	char *tagname;
	float size;
	float maxdist;
	float maxalpha;
	qhandle_t shader;
} shadowPart_t;

static qboolean CG_PlayerShadow( centity_t *cent, float *shadowPlane )
{
	vec3_t			end;
	trace_t			trace;
	float			dist, distFade;
	int				tagIndex, subIndex;
	vec3_t			origin, angles, axis[ 3 ];
	vec4_t			projection = { 0, 0, -1, 64 };
	shadowPart_t	shadowParts[] = {
		{"tag_footleft",	10,	4,	1.0,	0},
		{"tag_footright",	10,	4,	1.0,	0},
		{"tag_torso",		18,	96,	0.8,	0},
		{NULL, 0}
	};

	shadowParts[0].shader = cgs.media.shadowFootShader;		//DAJ pulled out of initliization
	shadowParts[1].shader = cgs.media.shadowFootShader;
	shadowParts[2].shader = cgs.media.shadowTorsoShader;

	*shadowPlane = 0;

	if ( cg_shadows.integer == 0 ) {
		return qfalse;
	}

	// send a trace down from the player to the ground
	VectorCopy( cent->lerpOrigin, end );
	end[2] -= SHADOW_DISTANCE;

	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, NULL, NULL, 0, MASK_PLAYERSOLID );

	// no shadow if too high
	//%	if ( trace.fraction == 1.0 || trace.fraction == 0.0f ) {
	//%		return qfalse;
	//%	}

	*shadowPlane = trace.endpos[2] + 1;

	if ( cg_shadows.integer != 1 ) {	// no mark for stencil or projection shadows
		return qtrue;
	}

	// no shadows when dead
	if( cent->currentState.eFlags & EF_DEAD ) {
		return qfalse;
	}
	
	// fade the shadow out with height
	//%	alpha = 1.0 - trace.fraction;

	// add the mark as a temporary, so it goes directly to the renderer
	// without taking a spot in the cg_marks array
	dist = VectorDistance( cent->lerpOrigin, cg.refdef_current->vieworg );	//%	cg.snap->ps.origin );
	distFade = 1.0f;
	if (!(cent->currentState.eFlags & EF_ZOOMING) && (dist > SHADOW_MIN_DIST)) {
		if( dist > SHADOW_MAX_DIST )
		{
			if (dist > SHADOW_MAX_DIST*2)
				return qfalse;
			else	// fade out
				distFade = 1.0f - ((dist - SHADOW_MAX_DIST) / SHADOW_MAX_DIST);
			
			if( distFade > 1.0f )
				distFade = 1.0f;
			else if( distFade < 0.0f )
				distFade = 0.0f;
		}
		
		// set origin
		VectorCopy( cent->lerpOrigin, origin );
		
		// project it onto the shadow plane
		if( origin[2] < *shadowPlane )
			origin[2] = *shadowPlane;
		
		// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
		origin[ 2 ] += 18.0f;
		
		//%	alpha *= distFade;
		
		// ydnar: decal remix
		//%	CG_ImpactMark( cgs.media.shadowTorsoShader, trace.endpos, trace.plane.normal, 
		//%		0, alpha,alpha,alpha,1, qfalse, 16, qtrue, -1 );
		CG_ImpactMark( cgs.media.shadowTorsoShader, origin, projection, 18.0f,
				cent->lerpAngles[ YAW ], distFade, distFade, distFade, distFade, -1 );
		return qtrue;
	}
	
	if (dist < SHADOW_MAX_DIST) {	// show more detail
		// now add shadows for the various body parts
		for (tagIndex=0; shadowParts[tagIndex].tagname; tagIndex++ ) {
			// grab each tag with this name
			for (subIndex=0; (subIndex = CG_GetOriginForTag( cent, &cent->pe.bodyRefEnt, shadowParts[tagIndex].tagname, subIndex, origin, axis )) >= 0; subIndex++)
			{
				// project it onto the shadow plane
				if (origin[2] < *shadowPlane)
					origin[2] = *shadowPlane;
				
				// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
				origin[ 2 ] += 5.0f;
				
				#if 0
					alpha = 1.0 - ((origin[2] - (*shadowPlane+ZOFS)) / shadowParts[tagIndex].maxdist);
					if (alpha < 0)
						continue;
					if (alpha > shadowParts[tagIndex].maxalpha)
						alpha = shadowParts[tagIndex].maxalpha;
					alpha *= (1.0 - distFade);
					origin[2] = *shadowPlane;
				#endif
				
				AxisToAngles( axis, angles );

				// ydnar: decal remix
				//%	CG_ImpactMark( shadowParts[tagIndex].shader, origin, trace.plane.normal, 
				//%		angles[YAW]/*cent->pe.legs.yawAngle*/, alpha,alpha,alpha,1, qfalse, shadowParts[tagIndex].size, qtrue, -1 );
				
				//%	CG_ImpactMark( shadowParts[ tagIndex ].shader, origin, up, 
				//%			cent->lerpAngles[ YAW ], 1.0f, 1.0f, 1.0f, 1.0f, qfalse, shadowParts[ tagIndex ].size, qtrue, -1 );
				CG_ImpactMark( shadowParts[ tagIndex ].shader, origin, projection, shadowParts[ tagIndex ].size,
						angles[ YAW ], distFade, distFade, distFade, distFade, -1 );
			}
		}
	}

	return qtrue;
}

/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void CG_PlayerSplash( centity_t *cent ) {
	vec3_t		start, end;
	trace_t		trace;
	int			contents;
	polyVert_t	verts[4];

	if ( !cg_shadows.integer ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, end );
	end[2] -= 24;

	// if the feet aren't in liquid, don't make a mark
	// this won't handle moving water brushes, but they wouldn't draw right anyway...
	contents = CG_PointContents( end, 0 );
	if ( !( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, start );
	start[2] += 32;

	// if the head isn't out of liquid, don't make a mark
	contents = CG_PointContents( start, 0 );
	if ( contents & ( CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}

	// trace down to find the surface
	trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) );

	if ( trace.fraction == 1.0 ) {
		return;
	}

	// create a mark polygon
	VectorCopy( trace.endpos, verts[0].xyz );
	verts[0].xyz[0] -= 32;
	verts[0].xyz[1] -= 32;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[1].xyz );
	verts[1].xyz[0] -= 32;
	verts[1].xyz[1] += 32;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[2].xyz );
	verts[2].xyz[0] += 32;
	verts[2].xyz[1] += 32;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[3].xyz );
	verts[3].xyz[0] += 32;
	verts[3].xyz[1] -= 32;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}

//==========================================================================

/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int team, entityState_t *es, const vec3_t fireRiseDir ) {
	centity_t *cent;
	refEntity_t backupRefEnt;//, parentEnt;
	qboolean	onFire=qfalse;
	float		alpha=0.0;
	float		fireStart, fireEnd;

	cent = &cg_entities[es->number];

	ent->entityNum = es->number;

	backupRefEnt = *ent;

/*	if (CG_EntOnFire(&cg_entities[es->number])) { // jaquboss, this is what we dont need, however option might be useful
		ent->reFlags |= REFLAG_FORCE_LOD;
	}*/

	trap_R_AddRefEntityToScene( ent );

	// jaquboss - make nice colored glow on players
	if ( ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR && cgs.spectator & SPEC_PLAYERGLOW ) || es->eFlags & EF_GLOW )
	{

		if( cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
		{
			if ( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR || !(cgs.spectator  & SPEC_PLAYERGLOW) )
			team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		}

		if ( team == TEAM_AXIS ){
			ent->shaderRGBA[0] = 255; 
			ent->shaderRGBA[1] = 0; 
			ent->shaderRGBA[2] = 0; 
			ent->shaderRGBA[3] = 255; 
		} else {
			ent->shaderRGBA[0] = 0; 
			ent->shaderRGBA[1] = 0; 
			ent->shaderRGBA[2] = 255; 
			ent->shaderRGBA[3] = 255; 

		}

		ent->customShader = cgs.media.specGlowShader[0];
		trap_R_AddRefEntityToScene( ent );


	}

	if (!onFire && CG_EntOnFire(&cg_entities[es->number])) {
		onFire = qtrue;
		// set the alpha
		if ( ent->entityNum == cg.snap->ps.clientNum ) {
			fireStart = cg.snap->ps.onFireStart;
			fireEnd = cg.snap->ps.onFireStart + 1500;
		}
		else {
			fireStart = es->onFireStart;
			fireEnd = es->onFireEnd;
		}

		alpha = (cg.time - fireStart) / 1500.0;
		if (alpha > 1.0) {
			alpha = (fireEnd - cg.time) / 1500.0;
			if (alpha > 1.0) {
				alpha = 1.0;
			}
		}
	}

	if (onFire) {
		if (alpha < 0.0) alpha = 0.0;
		ent->shaderRGBA[3] = (unsigned char)(255.0*alpha);
		VectorCopy( fireRiseDir, ent->fireRiseDir );
		if (VectorCompare(ent->fireRiseDir, vec3_origin)) {
			VectorSet( ent->fireRiseDir, 0, 0, 1 );
		}
		ent->customShader = cgs.media.onFireShader;
		trap_R_AddRefEntityToScene( ent );

		ent->customShader = cgs.media.onFireShader2;
		trap_R_AddRefEntityToScene( ent );

		if (ent->hModel == cent->pe.bodyRefEnt.hModel)
			trap_S_AddLoopingSound( ent->origin, vec3_origin, cgs.media.flameCrackSound, (int)(255.0*alpha), 0 );
	} 

	*ent = backupRefEnt;
}

char	*vtosf( const vec3_t v ) {
	static	int		index;
	static	char	str[8][64];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 64, "(%f %f %f)", v[0], v[1], v[2]);

	return s;
}


/*
===============
CG_AnimPlayerConditions

	predict, or calculate condition for this entity, if it is not the local client
===============
*/
void CG_AnimPlayerConditions( bg_character_t *character, centity_t *cent ) 
{
	entityState_t	*es;
	int				legsAnim;
//	int				impactPoint;	// jP

	if( !character ) 
	{
		return;
	}

	// don't worry about all this for 1st person views
	if( cg.snap && cg.snap->ps.clientNum == cent->currentState.number && !cg.renderingThirdPerson ) 
	{
		return;
	}

	es = &cent->currentState;
	// jet Pilot - Animation - added leaning
	// LEANING
	// - jaquboss - WTF?
	/*if (cg.predictedPlayerState.leanf < 0)
	{
        BG_UpdateConditionValue( es->clientNum, ANIM_COND_LEANING, LEANING_LEFT, qtrue );
	}
	else if (cg.predictedPlayerState.leanf > 0)
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_LEANING, LEANING_RIGHT, qtrue );
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_LEANING, LEANING_UNUSED, qtrue );
	}*/

	// jet Pilot - Animation - added impact points
	// IMPACT_POINT 
/*	impactPoint = BG_GetImpactPoint(&cg.snap->ps);
	if (cg.snap->ps.stats[STAT_HEALTH] > 0 && impactPoint > IMPACTPOINT_UNUSED && impactPoint < NUM_ANIM_COND_IMPACTPOINT)
    {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_IMPACT_POINT, impactPoint, qtrue );	
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_IMPACT_POINT, IMPACTPOINT_UNUSED, qtrue );
	}
*/

	// WEAPON
	if( es->eFlags & EF_ZOOMING )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, WP_BINOCULARS, qtrue );
	} 
	else 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, es->weapon, qtrue );
	}

	// MOUNTED
	if( (es->eFlags & EF_MG42_ACTIVE) || (es->eFlags & EF_MOUNTEDTANK) ) 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue );
	} 
	else if (es->eFlags & EF_AAGUN_ACTIVE)
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_AAGUN, qtrue );
	} 
	else 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_UNUSED, qtrue );
	}



	// PLAYER CLASS
	// jaquboss
	BG_UpdateConditionValue( es->clientNum, ANIM_COND_PLAYERCLASS, cgs.clientinfo[es->clientNum].cls, qtrue );

	// HEALTH LEVEL
	// jaquboss -- so can be for example coded to struggle when level is too low
	// a bit doom like - 66% and higher - 3, 33% and higher - 2, lower than 33% - 1 
	// jaquboss simplifed so it can be in sync with cgame
	// entityState_t.dl_intensity is player health
	if ( es->dl_intensity >= 66 )
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_HEALTH_LEVEL, 3, qtrue );
	else if ( es->dl_intensity >= 33 )
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_HEALTH_LEVEL, 2, qtrue );
	else 
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_HEALTH_LEVEL, 1, qtrue );

	// UNDERHAND
	// jaquboss can't predict this ..
	//BG_UpdateConditionValue( es->clientNum, ANIM_COND_UNDERHAND, cent->lerpAngles[0] > 0, qtrue );

	// CROUCHING
	if( es->eFlags & EF_CROUCHING ) 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qtrue, qtrue );
	} 
	else 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qfalse, qtrue );
	}

	// FIRING
	if( es->eFlags & EF_FIRING )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qtrue, qtrue );
	} 
	else 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qfalse, qtrue );
	}

	// reverse engineer the legs anim -> movetype (if possible)
	legsAnim = es->legsAnim & ~ANIM_TOGGLEBIT;

	if( character->animModelInfo->animations[legsAnim]->movetype ) 
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOVETYPE, character->animModelInfo->animations[legsAnim]->movetype, qfalse );
	}

}
#ifdef __SSYSTEM__
int	CG_AccIndex( int accNum, playerCharacter_t	*pc )
{
	switch ( accNum ){
	case ACC_BELT_LEFT:
		return pc->bLeftIndex;
	case ACC_BELT_RIGHT:
		return pc->bRightIndex;
// undeselectable
	case ACC_BELT:
		return pc->uBeltIndex >=0 ? pc->uBeltIndex : 0;
// helmet, shield on it, and rank are shared
	case ACC_HAT:
	case ACC_RANK:
	case ACC_MOUTH2:
		return pc->helmetIndex >=0 ? pc->helmetIndex : 0;
	case ACC_BACK:
		return pc->backPackIndex;
	case ACC_MOUTH3:
		return pc->glassIndex;
	}
	return -1;
}

int	CG_AccSkinIndex( int accNum, playerCharacter_t	*pc )
{
	switch ( accNum ){
	case ACC_BELT_LEFT:
		return pc->bLeftSkinIndex;
	case ACC_BELT_RIGHT:
		return pc->bRightSkinIndex;
	case ACC_BELT:
		return pc->uBeltSkinIndex;
// helmet, shield on it, and rank are shared
	case ACC_HAT:
		return pc->helmetSkinIndex;
	case ACC_RANK:
	case ACC_MOUTH2:
		return 0; // @TODO: use helmet skin for shield?
	case ACC_BACK:
		return pc->backPackSkinIndex;
	case ACC_MOUTH3:
		return pc->glassSkinIndex;
	}
	return -1;
}
#endif
/*
===============
CG_Player
===============
*/
void CG_Player( centity_t *cent )
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum,i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;
//	float			gumsflappin = 0;	// talking amplitude
	qboolean		usingBinocs = qfalse;
	centity_t		*cgsnap;
	bg_character_t	*character;
	bg_character_t	*headCharacter; // jaquboss
	float			hilightIntensity = 0.f;
	qboolean		painShader=qfalse;
#ifdef __SSYSTEM__
	playerCharacter_t	*headPc;
	playerCharacter_t	*pc;
#endif

	cgsnap = &cg_entities[cg.snap->ps.clientNum];

	shadow = qfalse;												// gjd added to make sure it was initialized
	shadowPlane = 0.0;												// ditto

	// Jaybird - cleaning warnings
	memset(&playerOrigin, 0, sizeof(playerOrigin));

	// if set to invisible, skip
	// jaquboss lets save one more eflag
/*	if( cent->currentState.eFlags & EF_NODRAW )
	{
		return;
	}*/

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) 
	{
		CG_Error( "Bad clientNum on player entity");
	}

	ci = &cgs.clientinfo[ clientNum ];

	// it is possible to see corpses from disconnected players that may not have valid clientinfo
	if ( !ci->infoValid ) 
		return;

#ifdef __SSYSTEM__
	if ( cent->currentState.powerups & (1 << PW_OPS_DISGUISED))
	{ 
		if ( cgs.clientinfo[ci->disguiseClient].infoValid )
			pc = &cgs.clientinfo[ci->disguiseClient].pc;
	} else { 
		pc = &ci->pc;
	}

	headPc = &ci->pc;
#endif
	character = CG_CharacterForClientinfo( ci, cent );
	headCharacter = BG_GetCharacter( ci->team, ci->cls );

	if( cent->currentState.eFlags & EF_MOUNTEDTANK ) 
	{
		VectorCopy( cg_entities[ cg_entities[ cent->currentState.clientNum ].tagParent ].mountedMG42Player.origin, playerOrigin );
	}
	else if( cent->currentState.eFlags & EF_MG42_ACTIVE || cent->currentState.eFlags & EF_AAGUN_ACTIVE ) 
	{ 	// Arnout: see if we're attached to a gun
		centity_t *mg42;
		int num;

		// find the mg42 we're attached to
		for ( num = 0 ; num < cg.snap->numEntities ; num++ ) 
		{
			mg42 = &cg_entities[ cg.snap->entities[ num ].number ];
			if( mg42->currentState.eType == ET_MG42_BARREL && mg42->currentState.otherEntityNum == cent->currentState.number ) 
			{
				// found it, clamp behind gun
				vec3_t	forward, right, up;

				//AngleVectors (mg42->s.apos.trBase, forward, right, up);
				AngleVectors ( cent->lerpAngles, forward, right, up );
				VectorMA ( mg42->currentState.pos.trBase, -36, forward, playerOrigin );
				playerOrigin[2] = cent->lerpOrigin[2];
				break;
			}
		}

		if( num == cg.snap->numEntities ) 
		{
			VectorCopy( cent->lerpOrigin, playerOrigin );
		}
	} 
	else 
	{
		VectorCopy( cent->lerpOrigin, playerOrigin );
	}

	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc,  0, sizeof(acc) );

	// get the rotation information
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );

	// FIXME: move this into CG_PlayerAngles
//	if(	cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
//		memcpy( body.torsoAxis, body.axis, sizeof(body.torsoAxis) );

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );

	// calculate client-side conditions
	CG_AnimPlayerConditions( character, cent );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation( cent, &body );

	// forcibly set binoc animation
	if( cent->currentState.eFlags & EF_ZOOMING ) {
		usingBinocs = qtrue;
	}

	// add the any sprites hovering above the player
	// rain - corpses don't get icons (fireteam check ran out of bounds)
	if( cent->currentState.eType != ET_CORPSE ) {
		CG_PlayerSprites( cent );
	}

	// jet Pilot - Fix the misplacement of the shadows
	shadow = CG_PlayerShadow( cent, &shadowPlane );

	// add a water splash if partially in and out of water
	CG_PlayerSplash( cent );

	// get the player model information
	renderfx = 0;
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		renderfx = RF_THIRD_PERSON;			// only draw in mirrors
	}

	// draw the player in cameras
	if( cg.cameraMode || cg.subscene)
		renderfx &= ~RF_THIRD_PERSON;

	// add shadows if activated
	if( cg_shadows.integer == 3 && shadow ) {
		renderfx |= RF_SHADOW_PLANE;
	}

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	// set renderfx for everything
	body.renderfx = renderfx;
	acc.renderfx = renderfx;
	head.renderfx = renderfx;

	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

	if ( cg_modelHilights.integer & 1){
		vec3_t	dist;
		vec_t	distSquared;

		VectorSubtract( lightorigin, cg.refdef_current->vieworg, dist );
		distSquared = VectorLengthSquared( dist );
		if( distSquared > Square(384.f) )
		{
			renderfx |= RF_MINLIGHT;

			distSquared -= Square(384.f);

			if( distSquared > Square(768.f) )
			{
                hilightIntensity = 1.f;
			}
			else 
			{
				hilightIntensity = 1.f * (distSquared / Square(768.f));
			}

			//CG_Printf( "%f\n", hilightIntensity );
		}
	}

	VectorCopy( lightorigin, body.lightingOrigin );	
	VectorCopy( lightorigin, head.lightingOrigin );
	VectorCopy( lightorigin, acc.lightingOrigin );
	body.hilightIntensity = hilightIntensity;
	head.hilightIntensity = hilightIntensity;
	acc.hilightIntensity  = hilightIntensity;
	body.shadowPlane	  = shadowPlane;
	head.shadowPlane	  = shadowPlane;
	acc.shadowPlane		  = shadowPlane;

	//
	// add the body
	//
	// Jaybird - cleaning up warnings
	if(( cent->currentState.eType == ET_CORPSE && cent->currentState.time2 == 1 ) || ( cent->currentState.eFlags & EF_PANTSED )) 
	{
		body.hModel		= character->undressedCorpseModel;
		body.customSkin	= character->undressedCorpseSkin;
	}
	else 
	{
#ifndef __SSYSTEM__ 
		body.hModel		= character->mesh;
		body.customSkin	= character->skin;		
#else 
		body.hModel		= character->body[pc->bodyIndex].mesh; 
		body.customSkin	= character->body[pc->bodyIndex].torsoSkin[pc->torsoIndex];	
#endif
	}

	// add dlight once
	if (cent->currentState.eFlags & EF_GLOW){
		float r,b;
		int	 glowPass;
		int	 maxGlowPass = 1;
		char buff[8];
		team_t team= ci->team;

		if( cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
			team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		if ( team == TEAM_AXIS ){
			r=1.0f;
			b=0.0f;
		} else {
			b=1.0f;
			r=0.0f;
		}

		trap_Cvar_VariableStringBuffer( "r_dynamicLight", buff, 8 );

		// jaquboss do it more times if it will do any effect
		if ( atoi(buff) == 2 )
			maxGlowPass = 4;

		for ( glowPass=0; glowPass < maxGlowPass; glowPass++ ){
			trap_R_AddLightToScene( cent->lerpOrigin, 200, 1, r, 0, b, 0, 0 );
		}


	}

//	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	VectorCopy( playerOrigin, body.origin );	
//	VectorCopy( body.origin, body.oldorigin );	// don't positionally lerp at all

//	cent->pe.bodyRefEnt = body;

	// if the model failed, allow the default nullmodel to be displayed
	// Gordon: whoever wrote that comment sucks
	if( !body.hModel )
		return;

// DEBUG
	// position marker?
	if (cg_drawHitbox.integer & 4)
	{
		int			x, zd, zu;
		vec3_t		bmins, bmaxs;

		x = (cent->currentState.solid & 255);
		zd = ((cent->currentState.solid>>8) & 255);
		zu = ((cent->currentState.solid>>16) & 255) - 32;

		bmins[0] = bmins[1] = -x;
		bmaxs[0] = bmaxs[1] = x;
		bmins[2] = -zd;
		bmaxs[2] = zu;

		VectorAdd( bmins, cent->lerpOrigin, bmins );
		VectorAdd( bmaxs, cent->lerpOrigin, bmaxs );

		CG_RailTrail( tv( 0.25f, 0.5f, 1.f), bmins, bmaxs, 1, qtrue );
	}

	// head axis
	if (cg_drawHitbox.integer & 2)
	{
		orientation_t tag;
		int idx;
		vec3_t start;
		vec3_t ends[3];
		vec3_t axis[3];
		trap_R_LerpTag( &tag, &body, "tag_head", 0 );

		VectorCopy( body.origin, start );

		for( idx = 0; idx < 3; idx++ ) {
			VectorMA( start, tag.origin[idx], body.axis[idx], start );
		}

		MatrixMultiply( tag.axis, body.axis, axis );

		for( idx = 0; idx < 3; idx++ ) {
			VectorMA( start, 32, axis[idx], ends[idx] );
			CG_RailTrail2( tv( 0.25f, 0.5f, 1.f), start, ends[idx], qtrue );
		}
	}
	// hitbox
	if (cg_drawHitbox.integer & 1)
	{
		vec3_t mins, maxs;
		VectorCopy( cg.predictedPlayerState.mins, mins );
		VectorCopy( cg.predictedPlayerState.maxs, maxs );

		if( cg.predictedPlayerState.eFlags & EF_PRONE ) {
			maxs[2] = maxs[2] - (cg.predictedPlayerState.standViewHeight - PRONE_VIEWHEIGHT + 8);
		} else if( cg.predictedPlayerState.pm_flags & PMF_DUCKED ) {
			maxs[2] = cg.predictedPlayerState.crouchMaxZ;
		}

		VectorAdd( cent->lerpOrigin, mins, mins );
		VectorAdd( cent->lerpOrigin, maxs, maxs );
		CG_RailTrail( tv( 0.25f, 0.5f, 1.f), mins, maxs, 1, qtrue );

		if( cg.predictedPlayerState.eFlags & EF_PRONE ) {
			vec3_t org, forward;

			// The legs
			VectorCopy( playerlegsProneMins, mins );
			VectorCopy( playerlegsProneMaxs, maxs );

			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			forward[2] = 0;
			VectorNormalizeFast( forward );

			org[0] = cent->lerpOrigin[0] + forward[0] * -32;
			org[1] = cent->lerpOrigin[1] + forward[1] * -32;
			org[2] = cent->lerpOrigin[2] + cg.pmext.proneLegsOffset;

			VectorAdd( org, mins, mins );
			VectorAdd( org, maxs, maxs );
			CG_RailTrail( tv( 0.25f, 0.5f, 1.f), mins, maxs, 1, qtrue );

			// And the head
			VectorSet( mins, -6, -6, -22 );
			VectorSet( maxs, 6, 6, -10 );

			org[0] = cent->lerpOrigin[0] + forward[0] * 12;
			org[1] = cent->lerpOrigin[1] + forward[1] * 12;
			org[2] = cent->lerpOrigin[2];

			VectorAdd( org, mins, mins );
			VectorAdd( org, maxs, maxs );
			CG_RailTrail( tv( 0.25f, 0.5f, 1.f), mins, maxs, 1, qtrue );
		}
	}
// DEBUG

	//
	// add the head
	//
	if ( cent->currentState.eType == ET_CORPSE )
	headCharacter  = character;

#ifdef __SSYSTEM__
	if( !(head.hModel = headCharacter->head[headPc->headIndex].model) )
		return;
	head.customSkin = headCharacter->head[headPc->headIndex].skin[pc->faceIndex];
#else 
	if( !(head.hModel = headCharacter->hudhead) )
		return;
	head.customSkin = headCharacter->hudheadskin;
#endif

	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );

//	head.shadowPlane = shadowPlane;
//	head.renderfx = renderfx;


	if( cent->currentState.eFlags & EF_FIRING ) {
		cent->pe.lastFiredWeaponTime = 0;
		cent->pe.weaponFireTime += cg.frametime;
	} else {
		if( cent->pe.weaponFireTime > 500 && cent->pe.weaponFireTime ) {
			cent->pe.lastFiredWeaponTime = cg.time;
		}

		cent->pe.weaponFireTime = 0;
	}

	if( cg.time > cent->pe.lastIdleTimeEnd || ((cg.time -  cent->pe.spawnTime) < 150) || ((cg.time - cent->pe.deathTime) < 150 && cent->currentState.eFlags & EF_DEAD) ) {
		if( cent->currentState.dl_intensity < 40 ) {
			cent->pe.idleAnim = HD_DAMAGED_IDLE1;
		} else {
			cent->pe.idleAnim = HD_IDLE1;
		}
	}

	if( cent->currentState.eType != ET_CORPSE /*&& !(cent->currentState.eFlags & EF_DEAD)*/ ) {
		hudHeadAnimNumber_t anim;

		anim = cent->pe.idleAnim;

		if( cent->pe.weaponFireTime > 500 && !(cent->currentState.eFlags & EF_DEAD) ) {
			anim = HD_ATTACK;
		} else if( cg.time - cent->pe.lastFiredWeaponTime < 500 && !(cent->currentState.eFlags & EF_DEAD) ) {
			anim = HD_ATTACK_END;
#ifdef __SSYSTEM__
		} else if( cg.time - cent->pe.painTime < (headCharacter->head[pc->headIndex].animations[ HD_PAIN ].numFrames * headCharacter->head[pc->headIndex].animations[ HD_PAIN ].frameLerp) ){
#else
		} else if( cg.time - cent->pe.painTime < (headCharacter->hudheadanimations[ HD_PAIN ].numFrames * headCharacter->hudheadanimations[ HD_PAIN ].frameLerp) ){
#endif
			anim = HD_PAIN;
		} else if( cg.time > cent->pe.nextIdleTime ) { 

			cent->pe.nextIdleTime = cg.time + 7000 + rand() % 1000;

			if(  cent->currentState.dl_intensity < 40 || cent->currentState.eFlags & EF_DEAD ) {
				cent->pe.idleAnim = (rand() % (HD_DAMAGED_IDLE3 - HD_DAMAGED_IDLE2 + 1)) + HD_DAMAGED_IDLE2;
			} else {
				cent->pe.idleAnim = (rand() % (HD_IDLE8 - HD_IDLE2 + 1)) + HD_IDLE2;
			}
#ifdef __SSYSTEM__
			cent->pe.lastIdleTimeEnd = cg.time + headCharacter->head[pc->headIndex].animations[ cent->pe.idleAnim ].numFrames * headCharacter->head[pc->headIndex].animations[ cent->pe.idleAnim ].frameLerp;
#else
			cent->pe.lastIdleTimeEnd = cg.time + headCharacter->hudheadanimations[ cent->pe.idleAnim ].numFrames * headCharacter->hudheadanimations[ cent->pe.idleAnim ].frameLerp;
#endif
		}		

		CG_HudHeadAnimation( headCharacter, &cent->pe.head, &head.oldframe, &head.frame, &head.backlerp, anim, ci );
	} else	{
		head.frame = 0;
		head.oldframe = 0;
		head.backlerp = 0.f;
	}
	
	// add normal head
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	
	if(  cent->currentState.dl_intensity < 5 || cent->currentState.eType == ET_CORPSE || cent->currentState.eFlags & EF_DEAD ) {
		painShader = qtrue;
		head.customSkin =  cgs.media.hudDamagedStates[3];
	} else if(  cent->currentState.dl_intensity < 20 ) {
		painShader = qtrue;
		head.customSkin =  cgs.media.hudDamagedStates[2];
	} else if(  cent->currentState.dl_intensity < 40 ) {
		painShader = qtrue;
		head.customSkin =  cgs.media.hudDamagedStates[1];
	} else if(  cent->currentState.dl_intensity < 60 ) {
		painShader = qtrue;
		head.customSkin =  cgs.media.hudDamagedStates[0];
	}

	if ( painShader )
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	cent->pe.headRefEnt = head;

	// set the shadowplane for accessories
//	acc.shadowPlane = shadowPlane;
	CG_BreathPuffs( cent, &head );

	//
	// add the gun / barrel / flash
	//
	if( (!(cent->currentState.eFlags & EF_DEAD) &&
		 !(cent->currentState.eFlags & EF_PLAYDEAD)) 
#ifdef __SELFDEFENSE__
		||	cent->currentState.eFlags & EF_DEFENDING  
#endif
		){
			if (cent->currentState.eFlags & EF_TALK ){
				acc.hModel = cg_weapons[WP_SATCHEL_DET].weaponModel[W_TP_MODEL].model[0];
				CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
				CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
			} else {
				CG_AddPlayerWeapon( &body, NULL, cent );
			}
	}

	//
	// add binoculars (if it's not the player)
	//
	if( usingBinocs ) {			// NERVE - SMF
		acc.hModel = cgs.media.thirdPersonBinocModel;
		CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	}

	//
	// add accessories
	//

	for( i = ACC_BELT_LEFT; i < ACC_MAX; i++ )
	{
#ifndef __SSYSTEM__
		if( !(character->accModels[i]) )
			continue;
		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];
#else
		int	 index=CG_AccIndex(i,pc);
		int	 skinIndex=CG_AccSkinIndex(i,pc);

		// deselected
		if ( index < 0 )
			continue;

		if( !(character->acc[i][index].model) )
			continue;

		acc.hModel = character->acc[i][index].model;
		acc.customSkin = character->acc[i][index].skin[skinIndex];
#endif
		// Gordon: looted corpses dont have any accsserories, evil looters :E
		if( !(cent->currentState.eType == ET_CORPSE && cent->currentState.time2 == 1 )) 
		{
			switch(i)
			{
				case ACC_BELT_LEFT:
					CG_PositionEntityOnTag( &acc,	&body,	"tag_bright", 0, NULL);
					break;
				case ACC_BELT_RIGHT:
					CG_PositionEntityOnTag( &acc,	&body,	"tag_bleft", 0, NULL);
					break;

				case ACC_BELT:
					CG_PositionEntityOnTag( &acc,	&body,	"tag_ubelt", 0, NULL);
					break;
				case ACC_BACK:
					CG_PositionEntityOnTag( &acc,	&body,	"tag_back", 0, NULL);
					break;
				
				case ACC_HAT:			// Regular Helmet
				case ACC_RANK:			// Rank Insignia
				case ACC_MOUTH2:		// Officer's Cap
					// if we've not taken a headhot
					if (cent->currentState.eFlags & EF_HEADSHOT && (ci->skill[SK_HEAVY_WEAPONS] < 9 || ci->cls != PC_SOLDIER))
						continue;
					// get a rank icon
					if (i == ACC_RANK) {
						int iconNum;

						if( ci->rank == 0 || ci->rank > 18)
							continue;
		
						if ( cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
							iconNum = ci->team == TEAM_AXIS ? 0 : 1;
								else 
							iconNum = ci->team == TEAM_AXIS ? 1 : 0;

						acc.customShader = rankicons[ ci->rank ][ iconNum ][1].shader;	// jaquboss, team support					
					}

					// helmet shield
					if (i == ACC_MOUTH2) {
						if ( ci->skill[SK_HEAVY_WEAPONS] < 9 || ci->cls != PC_SOLDIER || cent->currentState.eFlags & EF_HEADSHOT )
							continue;
					}				
					CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
					break;
				
				case ACC_MOUTH3:		// jP - eyeglasses
#ifdef __SSYSTEM__
				CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
#endif
				break;
				// weapon and weapon2
				// these are used by characters who have permanent weapons attached to their character in the skin
#ifndef __SSYSTEM__ // do not want these
				case ACC_WEAPON:	// weap
					CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon", 0, NULL);
					break;
				case ACC_WEAPON2:	// weap2
					CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon2", 0, NULL);
					break;
#endif
				default:
					continue;
			}

			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		}
	}


	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	// now add legs
#ifdef __SSYSTEM__
	body.customSkin	= character->body[pc->bodyIndex].legsSkin[pc->legIndex];
	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
#endif

	cent->pe.bodyRefEnt = body;
	cent->pe.bodyRefEnt.renderfx = renderfx;

	if( cg_shadows.integer == 3 && shadow ) {
		body.renderfx &= ~RF_THIRD_PERSON;
		body.customShader = cgs.media.projectionshadow;
		trap_R_AddRefEntityToScene( &body );

	}

}


void CG_FireboltTrail( centity_t *ent,  int ammount, vec3_t particle_origin, qboolean attached );

void CG_Jet( centity_t *cent ) // jaquboss - players as jet's for jet pilot insanity
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		muzzle[4];
	int				clientNum,i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;
	int				num_engines = 2;	// changed to 2, for me262
	vec3_t			velocity;
	float			 speed;


	shadow = qfalse;												// gjd added to make sure it was initialized
	shadowPlane = 0.0;												// ditto

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) 
	{
		CG_Error( "Bad clientNum on player entity");
	}

	ci = &cgs.clientinfo[ clientNum ];

	// it is possible to see corpses from disconnected players that may not have valid clientinfo
	if ( !ci->infoValid ) 
	{
		return;
	}

	memset( &body, 0, sizeof(body) );

	for ( i=0;i<4;i++)
	memset( &muzzle[i], 0, sizeof(muzzle[i]) );


	CG_PlayerSprites( cent );

	// jet Pilot - Fix the misplacement of the shadows
	shadow = CG_PlayerShadow( cent, &shadowPlane );

	// get the player model information
	renderfx = 0;
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		renderfx = RF_THIRD_PERSON;			// only draw in mirrors
	}

	// draw the player in cameras
	if( cg.cameraMode || cg.subscene)
		renderfx &= ~RF_THIRD_PERSON;

	// add shadows if activated
	if( cg_shadows.integer == 3 && shadow ) {
		renderfx |= RF_SHADOW_PLANE;
	}

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	// set renderfx for everything
	body.renderfx = renderfx;
	VectorCopy( cent->lerpOrigin, body.lightingOrigin );	
	body.shadowPlane	  = shadowPlane;
	// set the exact origin
	VectorCopy( cent->lerpOrigin, body.origin );
	// copy exact angles - note that will include leany roll angle which is good
	AnglesToAxis( cent->lerpAngles, body.axis );

	//
	// get a speed
	//

	VectorCopy( cent->currentState.pos.trDelta, velocity );
	speed = VectorNormalize( velocity );

	if ( speed > 500 )
		speed = 500;

	//
	// add the body
	//
	body.hModel		= cgs.media.jetModel[ci->team-1];
	body.customSkin	= cgs.media.jetSkin[ci->team-1];	
	
	//
	// glow dlight
	//
	if (cent->currentState.eFlags & EF_GLOW){
		float r,b;
		int	 glowPass;
		int	 maxGlowPass = 1;
		char buff[8];
		team_t team= ci->team;

		if( cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
		{
			team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		}

		if ( team == TEAM_AXIS ){
			r=1.0f;
			b=0.0f;
		} else {
			b=1.0f;
			r=0.0f;
		}

		trap_Cvar_VariableStringBuffer( "r_dynamicLight", buff, 8 );

		// jaquboss do it more times if it will do any effect
		if ( atoi(buff) == 2 )
			maxGlowPass = 4;

		for ( glowPass=0; glowPass < maxGlowPass; glowPass++ ){
			trap_R_AddLightToScene( cent->lerpOrigin, 200, 1, r, 0, b, 0, 0 );
		}

	}

	// jaquboss better to run away, so tags won't try to crash it..
	if ( !body.hModel )
		return;

	//
	// muzzle flashes
	//	
	for (i=0;i<4;i++) {
		vec3_t	angles;

		muzzle[i].renderfx = renderfx;
		muzzle[i].hModel = cgs.media.mg42muzzleflash;

		angles[0] = angles[1]	= 0;
		angles[ROLL] = crandom() * 10;
		AnglesToAxis( angles, muzzle[i].axis );

		CG_PositionRotatedEntityOnTag( &muzzle[i], &body, va("tag_flash%i", i+1));

		if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME ) { 
			trap_R_AddRefEntityToScene( &muzzle[i] );
		}
	}

	//
	// engine effect
	//
	for (i=0;i<num_engines;i++) { 
		int				lightPass;
		int				maxLightPass = 1;
		char			buff[8];
		orientation_t	lerped;
		int				idx;
		vec3_t			origin;

		// ENGINE ORIGIN
		trap_R_LerpTag( &lerped, &body, va("tag_engine%i", i+1), 0 );
		VectorCopy( body.origin, origin );

		for( idx = 0; idx < 3; idx++ ) {
			VectorMA( origin, lerped.origin[idx], body.axis[idx], origin );
		}

		// ENGINE ORIGIN

		// ENGINE TRAILS
		CG_FireboltTrail( cent, speed, origin, qtrue );
		// ENGINE TRAILS

		// ENGINE DLIGHTS
		trap_Cvar_VariableStringBuffer( "r_dynamicLight", buff, 8 );
		// jaquboss do it more times if it will do any effect
		if ( atoi(buff) == 2 )
			maxLightPass = 4;

		if ( speed < 5 )
			maxLightPass = 0;

		for ( lightPass=0; lightPass < maxLightPass; lightPass++ ){
			trap_R_AddLightToScene( origin, speed/2, 1, 1, 1, 0, 0, 0 ); // add yellow light at engine
		}
		// ENGINE DLIGHTS

	}

	//
	// engine sound
	//
	if ( speed >= 5 )
	trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.jetFlySound, speed, 0 );


	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

}

//=====================================================================

extern void CG_ClearWeapLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber );

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void CG_ResetPlayerEntity( centity_t *cent ) {
	// Gordon: these are unused
//	cent->errorTime = -99999;		// guarantee no error decay added
//	cent->extrapolated = qfalse;	

	if (!(cent->currentState.eFlags & EF_DEAD)) {
		CG_ClearLerpFrameRate( cent, &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim );
		CG_ClearLerpFrame( cent, &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim );

		memset( &cent->pe.legs, 0, sizeof( cent->pe.legs ) );
		cent->pe.legs.yawAngle = cent->rawAngles[YAW];
		cent->pe.legs.yawing = qfalse;
		cent->pe.legs.pitchAngle = 0;
		cent->pe.legs.pitching = qfalse;

		memset( &cent->pe.torso, 0, sizeof( cent->pe.legs ) );
		cent->pe.torso.yawAngle = cent->rawAngles[YAW];
		cent->pe.torso.yawing = qfalse;
		cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
		cent->pe.torso.pitching = qfalse;
	}

	BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin, qfalse, cent->currentState.effect2Time );
	BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles, qtrue, cent->currentState.effect2Time  );

	VectorCopy( cent->lerpOrigin, cent->rawOrigin );
	VectorCopy( cent->lerpAngles, cent->rawAngles );

	if ( cg_debugPosition.integer ) {
		CG_Printf("%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
	}

	cent->pe.painAnimLegs = -1;
	cent->pe.painAnimTorso = -1;
	cent->pe.animSpeed = 1.0;

}

void CG_GetBleedOrigin( vec3_t head_origin, vec3_t body_origin, int fleshEntityNum )
{
		clientInfo_t		*ci;
		refEntity_t			body;
		refEntity_t			head;
		centity_t			*cent, backupCent;
		bg_character_t		*character;

		ci = &cgs.clientinfo[ fleshEntityNum ];

		if ( !ci->infoValid ) {
			return;
		}

		character = CG_CharacterForClientinfo( ci, NULL );

		cent = &cg_entities [ fleshEntityNum ];
		backupCent = *cent;

		memset( &body, 0, sizeof(body) );
		memset( &head, 0, sizeof(head) );

		CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );
		CG_PlayerAnimation ( cent, &body );
#ifdef __SSYSTEM__
		body.hModel = character->body[ci->pc.bodyIndex].mesh;
#else
		body.hModel = character->mesh;
#endif
		if( !body.hModel ) {
			return;
		}
#ifdef __SSYSTEM__
		body.hModel = character->head[ci->pc.headIndex].model;
#else
		head.hModel = character->hudhead;
#endif
		if( !head.hModel ) {
			return;
		}

		VectorCopy( cent->lerpOrigin, body.origin );
		VectorCopy( body.origin, body.oldorigin );

		// Ridah, restore the cent so we don't interfere with animation timings
		*cent = backupCent;

		CG_PositionRotatedEntityOnTag( &head, &body, "tag_head");
	
		VectorCopy( head.origin, head_origin );
		VectorCopy( body.origin, body_origin );
}

/*
===============
CG_GetTag
===============
*/
qboolean CG_GetTag( int clientNum, char *tagname, orientation_t *or ) {
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;

	ci = &cgs.clientinfo[ clientNum ];

	if( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson ) {
		cent = &cg.predictedPlayerEntity;
	} else {
		cent = &cg_entities[ci->clientNum];
		if (!cent->currentValid)
			return qfalse;		// not currently in PVS
	}

	refent = &cent->pe.bodyRefEnt;

	if( trap_R_LerpTag( or, refent, tagname, 0 ) < 0 )
		return qfalse;

	VectorCopy( refent->origin, org );

	for( i = 0 ; i < 3 ; i++ ) {
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );

	return qtrue;
}

/*
===============
CG_GetWeaponTag
===============
*/
qboolean CG_GetWeaponTag( int clientNum, char *tagname, orientation_t *or ) {
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;

	ci = &cgs.clientinfo[ clientNum ];

	if( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson ) {
		cent = &cg.predictedPlayerEntity;
	} else {
		cent = &cg_entities[ci->clientNum];
		if (!cent->currentValid)
			return qfalse;		// not currently in PVS
	}

	if (cent->pe.gunRefEntFrame < cg.clientFrame - 1)
		return qfalse;

	refent = &cent->pe.gunRefEnt;

	if (trap_R_LerpTag( or, refent, tagname, 0 ) < 0)
		return qfalse;

	VectorCopy( refent->origin, org );

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );

	return qtrue;
}

void CG_SetHudHeadLerpFrameAnimation( bg_character_t* ch, lerpFrame_t *lf, int newAnimation, clientInfo_t *ci ) {
	animation_t	*anim;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= MAX_HD_ANIMATIONS ) {
		CG_Error( "Bad animation number (CG_SetHudHeadLerpFrameAnimation): %i", newAnimation );
	}
#ifdef __SSYSTEM__
	anim = &ch->head[ci->pc.headIndex].animations[ newAnimation ];
#else
	anim = &ch->hudheadanimations[ newAnimation ];
#endif

	lf->animation		= anim;
	lf->animationTime	= lf->frameTime + anim->initialLerp;
}

void CG_ClearHudHeadLerpFrame( bg_character_t* ch, lerpFrame_t *lf, int animationNumber, clientInfo_t *ci )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetHudHeadLerpFrameAnimation( ch, lf, animationNumber, ci );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
	lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
}

void CG_RunHudHeadLerpFrame( bg_character_t* ch, lerpFrame_t *lf, int newAnimation, float speedScale, clientInfo_t *ci ) {
	int			f;
	animation_t	*anim;



	// see if the animation sequence is switching
	if( !lf->animation ) {
		CG_ClearHudHeadLerpFrame( ch, lf, newAnimation, ci );
	} else if( newAnimation != lf->animationNumber ) {
		CG_SetHudHeadLerpFrameAnimation( ch, lf, newAnimation, ci );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if ( cg.time < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;		// adjust for haste, etc
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;
		}
	}

	if ( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}

void CG_HudHeadAnimation( bg_character_t* ch, lerpFrame_t* lf, int *oldframe, int *frame, float *backlerp, hudHeadAnimNumber_t animation, clientInfo_t *ci  ) {
//	centity_t *cent = &cg.predictedPlayerEntity;

	CG_RunHudHeadLerpFrame( ch, lf, (int)animation, 1.f, ci );

	*oldframe		= lf->oldFrame;
	*frame			= lf->frame;
	*backlerp		= lf->backlerp;
}


// =============
// Menu Versions
// =============

#ifdef __SSYSTEM__
#define CROUCHING(anim)	( (anim) && ((anim)->movetype & ((1<<ANIM_MT_IDLECR)|(1<<ANIM_MT_WALKCR)|(1<<ANIM_MT_WALKCRBK))) )

static void CG_SetLerpFrameAnimation_Limbo( playerInfo_t *pi, lerpFrame_t *lf, int newAnimation, bg_character_t *character )
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
		lf->frameTime = cg.time- 1;
		lf->animationTime = cg.time - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
	}
}


void CG_RunLerpFrame_Limbo( playerInfo_t *pi, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;
	bg_character_t *character=BG_GetCharacter(pi->teamNum, pi->classNum );

	// see if the animation sequence is switching
	if( pi && (newAnimation != lf->animationNumber || !lf->animation) ) {
		CG_SetLerpFrameAnimation_Limbo( pi, lf, newAnimation,  character);
	} 
	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if( !anim->frameLerp )
			return;		// shouldn't happen

		if( cg.time < lf->animationTime )
			lf->frameTime = lf->animationTime;		// initial lerp
		 else
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;

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
				lf->frameTime = cg.time;

			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;

		if( cg.time > lf->frameTime )
			lf->frameTime = cg.time;
	}

	if( lf->frameTime > cg.time + 200 )
		lf->frameTime = cg.time;

	if( lf->oldFrameTime > cg.time )
		lf->oldFrameTime = cg.time;

	// calculate current lerp value
	if( lf->frameTime == lf->oldFrameTime )
		lf->backlerp = 0;
	 else 
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
}

int CG_GetAnimationForName_Limbo ( playerInfo_t *pi, const char *name )
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
static void CG_PlayerAnimation_Limbo( playerInfo_t *pi, refEntity_t *body ) {
	int				animIndex;
	bg_character_t	*character;

	character = BG_GetCharacter( pi->teamNum, pi->classNum );

	if( !character ) {
		return;
	}
	// default to whatever the legs are currently doing
	animIndex = CG_GetAnimationForName_Limbo(pi, "stand_machinegun")/*legsanim*/;

	CG_RunLerpFrame_Limbo( pi, &pi->legs, animIndex, 1 );

	body->oldframe = pi->legs.oldFrame;
	body->frame = pi->legs.frame;
	body->backlerp = pi->legs.backlerp;
	body->frameModel = pi->legs.frameModel;
	body->oldframeModel = pi->legs.oldFrameModel;

	CG_RunLerpFrame_Limbo( pi, &pi->torso, CG_GetAnimationForName_Limbo(pi, "stand_machinegun")/*torsoanim*/, 1 );


	body->oldTorsoFrame = pi->torso.oldFrame;
	body->torsoFrame = pi->torso.frame;
	body->torsoBacklerp = pi->torso.backlerp;
	body->torsoFrameModel = pi->torso.frameModel;
	body->oldTorsoFrameModel = pi->torso.oldFrameModel;
}

static void CG_SwingAngles_Limbo( float destination, float swingTolerance, float clampTolerance, float speed, float *angle, qboolean *swinging )
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
		move = cg.time * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		}

		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
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
CG_PlayerAngles_Limbo
===============
*/
void CG_PlayerAngles_Limbo( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
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

	CG_SwingAngles_Limbo( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesSubtract( legsAngles, pi->moveAngles, legsAngles );	// NERVE - SMF
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}
void CG_RenderPlayer( float x, float y, float w, float h, int playerClass, int playerTeam, clientInfo_t *ci) {
	refdef_t		refdef;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	vec3_t			origin;
	int				renderfx;
	vec3_t			mins = {-18 -18, -24};
	vec3_t			maxs = {18, 18, 48};
	float			len;
	vec4_t			hcolor = { 1, 0, 0, 0.5 };
	bg_character_t	*character = BG_GetCharacter( playerTeam, playerClass );
	int				i;
	playerInfo_t	*pi;

	pi = &cgs.renderedPlayerInfo;

	memset( &refdef, 0, sizeof( refdef ) );
	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	CG_AdjustFrom640( &x, &y, &w, &h );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = 35;
	refdef.fov_y = 35;

	len = 2.5f * ( maxs[2] - mins[2] );	
	origin[0] = len / tan( 20 / 2 ); 
	origin[1] = 0.5 * ( mins[1] + maxs[1] );
	origin[2] = (0.5 * ( mins[2] + maxs[2] ))-20;


	refdef.time = cg.time;

	trap_R_SetColor( hcolor );
	trap_R_ClearScene();
	trap_R_SetColor( NULL );

	// get the rotation information
	CG_PlayerAngles_Limbo( pi, body.axis, body.torsoAxis, head.axis );
	
	CG_PlayerAnimation_Limbo( pi, &body );

	// ok play some anim
	CG_HudHeadAnimation( character, &pi->headAnim, &head.oldframe, &head.frame, &head.backlerp, (rand() % (HD_IDLE8 - HD_IDLE2 + 1)) + HD_IDLE2, ci );


	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW | RF_FORCENOLOD;

	acc.renderfx = renderfx;
 
	//
	// add the body
	//
	body.hModel = character->body[cgs.limboPcInfo[playerClass][playerTeam-1].bodyIndex].mesh;
	body.customSkin = character->body[cgs.limboPcInfo[playerClass][playerTeam-1].bodyIndex].torsoSkin[cgs.limboPcInfo[pi->classNum][pi->teamNum].torsoIndex];
	body.renderfx = renderfx;

	VectorCopy( origin, body.origin );
	VectorCopy( origin, body.lightingOrigin );
	VectorCopy( body.origin, body.oldorigin );


	trap_R_AddRefEntityToScene( &body );

	body.customSkin = character->body[cgs.limboPcInfo[playerClass][playerTeam-1].bodyIndex].legsSkin[cgs.limboPcInfo[pi->classNum][pi->teamNum].legIndex];
	trap_R_AddRefEntityToScene( &body );


	//
	// add the head
	//
	head.hModel = character->head[cgs.limboPcInfo[playerClass][playerTeam-1].headIndex].model;
	head.customSkin = character->head[cgs.limboPcInfo[playerClass][playerTeam-1].headIndex].skin[cgs.limboPcInfo[pi->classNum][pi->teamNum].faceIndex];

	if( !head.hModel ) {
		return;
	}

	VectorCopy( origin, head.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );

	head.renderfx = renderfx;

	head.frame = 0;
	head.oldframe = 0;
	head.backlerp = 0.f;

	trap_R_AddRefEntityToScene( &head );

	AxisCopy( body.torsoAxis, acc.axis );
	VectorCopy( origin, acc.lightingOrigin );

	for( i = ACC_BELT_LEFT; i < ACC_MAX; i++ ) {
		int	 index=CG_AccIndex(i,&cgs.limboPcInfo[playerClass][playerTeam-1]);
		int	 skinIndex=CG_AccSkinIndex(i,&cgs.limboPcInfo[playerClass][playerTeam-1]);

		// deselected
		if ( index < 0 )
			continue;

		if( !(character->acc[i][index].model) )
			continue;

		acc.hModel = character->acc[i][index].model;
		acc.customSkin = character->acc[i][index].skin[skinIndex];

		switch(i) {
			case ACC_BELT_LEFT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_bright", 0, NULL);
				break;
			case ACC_BELT_RIGHT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_bleft", 0, NULL);
				break;

			case ACC_BELT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_ubelt", 0, NULL);
				break;
			case ACC_BACK:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_back", 0, NULL);
				break;
			
			case ACC_MOUTH2:		// hat2
				if ( ci->skill[SK_HEAVY_WEAPONS] < 9 || ci->cls != PC_SOLDIER )
					continue;
			case ACC_HAT:			// hat
			case ACC_MOUTH3:		// glasses
				CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
				break;
			default:
				continue;
		}

		trap_R_AddRefEntityToScene( &acc );
	}
	//
	// add the gun
	//
	/*{
		int weap = CG_GetSelectedWeapon();

		memset( &gun, 0, sizeof(gun) );
		memset( &barrel, 0, sizeof(barrel) );

		gun.hModel = cg_weapons[weap].weaponModel[W_TP_MODEL].model[0];

		VectorCopy( origin, gun.lightingOrigin );
		CG_PositionEntityOnTag( &gun, &body, "tag_weapon", 0, NULL );
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &gun );
	}*/

	// Save out the old render info so we don't kill the LOD system here
	trap_R_SaveViewParms();

	//
	// add an accent light
	//
	/*origin[0] -= 100;	// + = behind, - = in front
	origin[1] += 100;	// + = left, - = right
	origin[2] += 100;	// + = above, - = below
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );
*/
	trap_R_RenderScene( &refdef );

	// Reset the view parameters
	trap_R_RestoreViewParms();
}

void CG_DrawPlayer ( rectDef_t *rect, int playerClass, int playerTeam ){
	CG_RenderPlayer( rect->x, rect->y, rect->w, rect->h, playerClass, playerTeam , &cgs.clientinfo[cg.clientNum] );
}
#endif

weaponType_t weaponTypes[] =
{
	{ WP_MP40,					"MP 40"			},
	{ WP_THOMPSON,				"THOMPSON"		},
	{ WP_STEN,					"STEN",			},
	{ WP_PANZERFAUST,			"PANZERFAUST",	},
	{ WP_FLAMETHROWER,			"FLAMETHROWER",	},
	{ WP_KAR98,					"K43", 			},
	{ WP_CARBINE,				"M1 GARAND", 	},
	{ WP_FG42,					"FG42",			},
	{ WP_GARAND,				"M1 GARAND",	},
	{ WP_MOBILE_MG42,			"MOBILE MG42",	},
	{ WP_K43,					"K43",			},
	{ WP_MORTAR,				"MORTAR",		},
	{ WP_MORTAR2,				"GRANATWERFER",	},
	{ WP_COLT,					"COLT",			},
	{ WP_LUGER,					"LUGER",		},
	{ WP_AKIMBO_COLT,			"AKIMBO COLTS",	},
	{ WP_AKIMBO_LUGER,			"AKIMBO LUGERS",},
	{ WP_SILENCED_COLT,			"COLT",			},
	{ WP_SILENCER,				"LUGER",		},
	{ WP_AKIMBO_SILENCEDCOLT,	"AKIMBO COLTS",	},
	{ WP_AKIMBO_SILENCEDLUGER,	"AKIMBO LUGERS",},

	{ WP_SHOTGUN,				"SHOTGUN",		},
	{ WP_MOBILE_BROWNING,		"BROWNING",		},
	{ WP_KNIFE_KABAR,			"KNIFE",		}, // jP - KABAR Knife for allies
	{ WP_STG44,					"STG 44",		},
	{ WP_STEN_MKII,				"STEN MKII",	},
	{ WP_BAR,					"BAR",			},
	{ WP_BAZOOKA,				"BAZOOKA",		}, // jP - Bazooka Anti-tank for allies
	{ WP_MP34,					"MP34",			},
	{ WP_VENOM,					"VENOM",		},

	{ WP_NONE,					NULL,			},
	{ -1,						NULL,			},
};

weaponType_t* WM_FindWeaponTypeForWeapon ( weapon_t weapon )
{
	weaponType_t* w = weaponTypes;

	if(!weapon) 
	{
		return NULL;
	}


	while(w->weapindex != -1) 
	{
		if(w->weapindex == weapon) 
		{
			return w;
		}
		
		w++;
	}
	return NULL;
}

void WM_RegisterWeaponTypeShaders () {
	weaponType_t* w = weaponTypes;

	while(w->weapindex) {	
//		w->shaderHandle = trap_R_RegisterShaderNoMip( w->shader );
		w++;
	}
}



