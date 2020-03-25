/*
 * name:		cg_effects.c
 *
 * desc:		these functions generate localentities, usually as a result of event processing
 *
 * NQQS:
 */

#include "cg_local.h"

/*
==================
CG_BubbleTrail

Bullets shot underwater
==================
*/
void CG_BubbleTrail( vec3_t start, vec3_t end, float size, float spacing ) {
	vec3_t		move;
	vec3_t		vec;
	float		len;
	int			i;
	localEntity_t	*le;
	refEntity_t		*re;

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);

	// advance a random amount first
	i = rand() % (int)spacing;
	VectorMA( move, i, vec, move );

	VectorScale (vec, spacing, vec);

	for ( ; i < len; i += spacing ) {

		le = CG_AllocLocalEntity();
		le->leFlags = LEF_PUFF_DONT_SCALE;
		le->leType = LE_MOVE_SCALE_FADE;
		le->startTime = cg.time;
		le->endTime = cg.time + SECONDS_1 + random() * 250;
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );

		re = &le->refEntity;
		re->shaderTime = cg.time / SECONDS_1_F;

		re->reType = RT_SPRITE;
		re->rotation = 0;
		re->radius = size; // (SA)
		re->customShader = cgs.media.waterBubbleShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;

		le->color[3] = 1.0;

		le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		VectorCopy( move, le->pos.trBase );
		le->pos.trDelta[0] = crandom()*3;
		le->pos.trDelta[1] = crandom()*3;
		le->pos.trDelta[2] = crandom()*5 + 20;	// (SA)

		VectorAdd (move, vec, move);
	}
}

/*
=====================
CG_SmokePuff

Adds a smoke puff or blood trail localEntity.

(SA) boy, it would be nice to have an acceleration vector for this as well.
		big velocity vector with a negative acceleration for deceleration, etc.
		(breath could then come out of a guys mouth at the rate he's walking/running and it
		would slow down once it's created)
=====================
*/
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel,
				   float radius,
				   float r, float g, float b, float a,
				   float duration,
				   int startTime,
				   int fadeInTime,
				   int leFlags,
				   qhandle_t hShader ) {
	static int	seed = 0x92;
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	le->leFlags = leFlags;
	le->radius = radius;

	re = &le->refEntity;
	re->rotation = Q_random( &seed ) * 360;
	re->radius = radius;
	re->shaderTime = startTime / SECONDS_1_F;

	le->leType = LE_MOVE_SCALE_FADE;
	le->startTime = startTime;
	le->endTime = startTime + duration;
	le->fadeInTime = fadeInTime;
	if ( fadeInTime > startTime )
		le->lifeRate = 1.0 / ( le->endTime - le->fadeInTime );
	else
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	le->color[0] = r;
	le->color[1] = g;
	le->color[2] = b;
	le->color[3] = a;


	le->pos.trType = TR_LINEAR;
	le->pos.trTime = startTime;
	VectorCopy( vel, le->pos.trDelta );
	VectorCopy( p, le->pos.trBase );

	VectorCopy( p, re->origin );
	re->customShader = hShader;

	// rage pro can't alpha fade, so use a different shader
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		re->customShader = cgs.media.smokePuffRageProShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;
	}
	else {
		re->shaderRGBA[0] = le->color[0] * 0xff;
		re->shaderRGBA[1] = le->color[1] * 0xff;
		re->shaderRGBA[2] = le->color[2] * 0xff;
		re->shaderRGBA[3] = 0xff;
	}

	re->reType = RT_SPRITE;
	re->radius = le->radius;

	return le;
}

/*
====================
CG_MakeExplosion
====================
*/
localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir,
								qhandle_t hModel, qhandle_t shader,
								int msec, qboolean isSprite ) {
	localEntity_t	*ex;
	vec3_t			newOrigin;

	if ( msec <= 0 ) {
		CG_Error( "CG_MakeExplosion: msec = %i", msec );
	}

	ex = CG_AllocLocalEntity();
	if ( isSprite ) {
		vec3_t			tmpVec;

		ex->leType = LE_SPRITE_EXPLOSION;

		// randomly rotate sprite orientation
		ex->refEntity.rotation = rand() % 360;
		VectorScale( dir, 16, tmpVec );
		VectorAdd( tmpVec, origin, newOrigin );
	}
	else {
		ex->leType = LE_EXPLOSION;
		VectorCopy( origin, newOrigin );

		// set axis with random rotate
		if ( !dir ) {
			AxisClear( ex->refEntity.axis );
		}
		else {
			VectorCopy( dir, ex->refEntity.axis[0] );
			RotateAroundDirection( ex->refEntity.axis, (rand() % 360) );
		}
	}

	// skew the time a bit so they aren't all in sync ... offset (rand() & 63)
	ex->startTime = cg.time - (rand() & 63);
	ex->endTime = ex->startTime + msec;

	// bias the time so all shader effects start correctly
	ex->refEntity.shaderTime = ex->startTime / SECONDS_1_F;

	ex->refEntity.hModel = hModel;
	ex->refEntity.customShader = shader;

	// set origin
	VectorCopy( newOrigin, ex->refEntity.origin );
	VectorCopy( newOrigin, ex->refEntity.oldorigin );

	// Ridah, move away from the wall as the sprite expands
	ex->pos.trType = TR_LINEAR;
	ex->pos.trTime = cg.time;
	VectorCopy( newOrigin, ex->pos.trBase );
	VectorScale( dir, 48, ex->pos.trDelta );
	// done.

	ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

	return ex;
}

/*
=================
CG_AddBloodTrails
=================
*/
void CG_AddBloodTrails( vec3_t origin, vec3_t dir, int speed, int duration, int count, float randScale ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t	velocity;
	int	i;

	for (i=0; i<count; ++i) {
		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		VectorSet( velocity, dir[0] + crandom()*randScale, dir[1] + crandom()*randScale, dir[2] + crandom() * randScale);
		VectorScale( velocity, (float)speed, velocity );

		le->leType = LE_BLOOD;
		le->startTime = cg.time;
		le->endTime = le->startTime + duration; // DHM - Nerve :: (removed) - (int)(0.5 * random() * duration);
		le->lastTrailTime = cg.time;

		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );

		le->pos.trType = TR_GRAVITY_LOW;
		VectorCopy( origin, le->pos.trBase );
		// s = 2 + random()*4;
		VectorMA( le->pos.trBase, (2 + random()*4), dir, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;

		le->bounceFactor = 0.9;
	}
}

/*
=================
CG_Bleed

This is the spurt of blood when a character gets hit
=================
*/
void CG_Bleed( vec3_t origin, int entityNum ) {
#define	BLOOD_SPURT_COUNT	4
	centity_t *cent;

	if( !cg_gibs.integer ) {
		return;
	}

	cent = &cg_entities[entityNum];

	// Ridah, blood spurts
	if ( entityNum != cg.snap->ps.clientNum ) {
		int		i;
		vec3_t	ndir;

		// core: let's keep it simple..
		// origin is the point where the player gets hit.
		// Create a direction-vector (ndir) pointing out the player:
		VectorSubtract( origin, cent->currentState.pos.trBase, ndir );
		ndir[2] = 0.0f; //only in XY plane, discard Z direction..
		VectorNormalize( ndir );

		// DHM - Nerve :: Made minor adjustments
		for( i = 0; i < BLOOD_SPURT_COUNT; ++i ) {
			ndir[0] += crandom()*0.3;
			ndir[1] += crandom()*0.3;
			ndir[2] += crandom()*0.3;

			CG_AddBloodTrails( origin, ndir,
							   100,								// speed
							   450 + (int)(crandom () * 50),	// duration
							   2 + rand()%2,					// count
							   0.1 );							// rand scale
		}


	}
}

/*
==================
CG_LaunchGib
==================
*/
void CG_FreeLocalEntity( localEntity_t *le );

void CG_LaunchGib( centity_t *cent, vec3_t origin, vec3_t angles, vec3_t velocity, qhandle_t hModel, float sizeScale, int breakCount ) {
	localEntity_t	*le;
	refEntity_t		*re;
	float			scale;
	vec3_t			maxs,mins;

	if ( !cg_gibs.integer ) {
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + 40000 + (crandom() * SECONDS_5);
	le->breakCount = breakCount;
	le->sizeScale = sizeScale;

	VectorCopy( angles, le->angles.trBase );
	VectorCopy( origin, re->origin );
	AnglesToAxis( angles, re->axis );
	if (sizeScale != 1.0) {
		VectorScale( re->axis[0], sizeScale, re->axis[0] );
		VectorScale( re->axis[1], sizeScale, re->axis[1] );
		VectorScale( re->axis[2], sizeScale, re->axis[2] );
	}
	re->hModel = hModel;

	if ( !re->hModel ) {
		CG_FreeLocalEntity( le );
		return;
	}

	trap_R_ModelBounds( re->hModel, mins, maxs );
	scale = RadiusFromBounds( mins, maxs )/10;

	if ( scale <= 0 ) {
		CG_FreeLocalEntity( le );
		return;
	}

	if ( scale < 0.2 ) {
		scale = 0.2;
	}
	if ( scale > 5 ) {
		scale = 5;
	}

	re->fadeStartTime		= le->endTime - SECONDS_1;
	re->fadeEndTime			= le->endTime;

	le->leBounceSoundType = LEBS_BLOOD;
	le->leMarkType = LEMT_BLOOD;
	le->pos.trType = TR_GRAVITY;

	le->angles.trDelta[0] = ((10 + (rand()&50)) - 30)	/ scale;
	le->angles.trDelta[1] = ((100 + (rand()&500)) - 300)/ scale;
	le->angles.trDelta[2] = ((10 + (rand()&50)) - 30)	/ scale;

	le->bounceFactor = 0.5f-(scale/10.f);

	if ( le->bounceFactor < 0 ) {
		le->bounceFactor= 0;
	}

	VectorCopy( origin, le->pos.trBase );
	VectorCopy( velocity, le->pos.trDelta );
	le->pos.trTime = cg.time;


	le->angles.trType = TR_LINEAR;

	le->angles.trTime = cg.time;

	le->ownerNum = cent->currentState.number;

	// Ridah, if the player is on fire, then spawn some flaming gibs
	if (cent && CG_EntOnFire(cent)) {
		le->onFireStart = cent->currentState.onFireStart;
		le->onFireEnd = re->fadeEndTime+SECONDS_1;
	}
}

#define	GIB_VELOCITY	75
#define	GIB_JUMP		250

/*
==============
CG_LoseACC
==============
*/
void CG_LoseACC( centity_t *cent, vec3_t dir, int accIndex, char *tagIndex, qboolean head )
{
	clientInfo_t	*ci;
	int				clientNum;
	vec3_t			origin, velocity;
	bg_character_t	*character;

	memset(&origin, 0, sizeof(origin));

	clientNum = cent->currentState.clientNum;
	if( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];
	character = CG_CharacterForClientinfo( ci,  cent );

	// don't launch anything if they don't have one
	if( !character->accModels[accIndex] ) {
		return;
	}

	CG_GetOriginForTag( cent, head ? &cent->pe.headRefEnt : &cent->pe.bodyRefEnt, tagIndex, 0, origin, NULL );

	velocity[0] = dir[0]*(0.75+random())*GIB_VELOCITY;
	velocity[1] = dir[1]*(0.75+random())*GIB_VELOCITY;
	velocity[2] = GIB_JUMP - 50 + dir[2]*(0.5+random())*GIB_VELOCITY;

	{
		localEntity_t	*le = CG_AllocLocalEntity();
		refEntity_t		*re = &le->refEntity;

		le->leType = LE_FRAGMENT;
		le->startTime = cg.time;
		le->endTime = le->startTime + SECONDS_20 + (crandom() * SECONDS_5);

		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );

		re->hModel = character->accModels[accIndex];
		re->customSkin = character->accSkins[accIndex];

		if ( !re->hModel ){
			CG_FreeLocalEntity( le );
			return;
		}

		re->fadeStartTime		= le->endTime - SECONDS_1;
		re->fadeEndTime			= le->endTime;

		// (SA) FIXME: origin of hat md3 is offset from center.  need to center the origin when you toss it
		le->pos.trType = TR_GRAVITY;
		VectorCopy( origin, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;

		// spin it a bit
		le->angles.trType		= TR_LINEAR;
		VectorCopy( tv(0, 0, 0), le->angles.trBase );
		le->angles.trDelta[0]	= 0;
		le->angles.trDelta[1]	= (100 + (rand()&500)) - 300;
		if ( !head ) {
			le->angles.trDelta[2]	= 50;
			le->effectFlags |= 2; // flush
		}
		else {
			le->angles.trDelta[2]	= 400;	// (SA) this is set with a very particular value to try to get it
											// to flip exactly once before landing (based on player alive
											// (standing) and on level ground) and will be unnecessary when
											// I have things landing properly on their own
		}

		le->angles.trTime		= cg.time;

		le->bounceFactor		= 0.2;

		// Ridah, if the player is on fire, then make the hat on fire
		if( cent && CG_EntOnFire(cent) ) {
			le->onFireStart = cent->currentState.onFireStart;
			le->onFireEnd = cent->currentState.onFireEnd + 4000;
		}
	}

}

/*
======================
CG_GetOriginForTag

  places the position of the tag into "org"

  returns the index of the tag it used, so we can cycle through tag's with the same name
======================
*/
int CG_GetOriginForTag( centity_t *cent, refEntity_t *parent, char *tagName, int startIndex, vec3_t org, vec3_t axis[3] ) {

	orientation_t	lerped;
	int				retval = trap_R_LerpTag( &lerped, parent, tagName, startIndex ); // lerp the tag

	if (retval < 0)
		return retval;

	VectorCopy( parent->origin, org );

	VectorMA( org, lerped.origin[0], parent->axis[0], org );
	VectorMA( org, lerped.origin[1], parent->axis[1], org );
	VectorMA( org, lerped.origin[2], parent->axis[2], org );

	if (axis) {
		// had to cast away the const to avoid compiler problems...
		MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, axis );
	}

	return retval;
}

/*
===================
CG_GibPlayer

Generated a bunch of gibs launching out from the bodies location
===================
*/
#define MAXJUNCTIONS 8

void CG_GibPlayer( centity_t *cent, vec3_t playerOrigin, vec3_t gdir )
{
	trace_t			trace;
	qboolean		foundtag;
	clientInfo_t	*ci;
	int				clientNum;
	bg_character_t	*character;
	vec4_t			projection;

	// Rafael
	// BloodCloud
	qboolean		newjunction[MAXJUNCTIONS];
	vec3_t			junctionOrigin[MAXJUNCTIONS];
	vec3_t			axis[3], angles;

	char *JunctiongibTags[] = {
		// leg tag
		"tag_footright",
		"tag_footleft",
		"tag_legright",
		"tag_legleft",

		// torsotags
		"tag_armright",
		"tag_armleft",

		"tag_torso",
		"tag_chest"
		};

	char *ConnectTags[] = {
		// legs tags
		"tag_legright",
		"tag_legleft",
		"tag_torso",
		"tag_torso",

		// torso tags
		"tag_chest",
		"tag_chest",

		"tag_chest",
		"tag_torso",
	};

	char *gibTags[] = {
		// tags in the legs
		"tag_footright",
		"tag_footleft",
		"tag_legright",
		"tag_legleft",
		"tag_torso",

		// tags in the torso
		"tag_chest",
		"tag_armright",
		"tag_armleft",
		"tag_head",
		NULL
	};

	clientNum = cent->currentState.clientNum;
	if( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];

	if( cg_gibs.integer ) {
		refEntity_t 	*re;
		int				i, count = 0, tagIndex, gibIndex;
		vec3_t			origin, velocity, dir;
		vec4_t			color;
		int 			junction, j;

		// Rafael
		for( i = 0; i < MAXJUNCTIONS; ++i ) {
			newjunction[i] = qfalse;
		}

		character = CG_CharacterForClientinfo( ci, cent );

		// Ridah, fetch the various positions of the tag_gib*'s
		// and spawn the gibs from the correct places (especially the head)
		for( gibIndex = 0, count = 0, foundtag = qtrue; foundtag && gibIndex < MAX_GIB_MODELS && gibTags[gibIndex]; ++gibIndex ) {
			re = 0;

			foundtag = qfalse;

			if( !character->gibModels[gibIndex] ) {
				continue;
			}

			re = &cent->pe.bodyRefEnt;

			for( tagIndex=0; (tagIndex = CG_GetOriginForTag( cent, re, gibTags[gibIndex], tagIndex, origin, axis )) >= 0; ++count, ++tagIndex ) {

				foundtag = qtrue;

				VectorSubtract( origin, re->origin, dir );
				VectorNormalize( dir );

				// spawn a gib
				velocity[0] = dir[0]*(0.5+random())*GIB_VELOCITY*0.3;
				velocity[1] = dir[1]*(0.5+random())*GIB_VELOCITY*0.3;
				velocity[2] = (GIB_JUMP*0.66) + dir[2]*(0.5+random())*GIB_VELOCITY*0.3;

				VectorMA( velocity, GIB_VELOCITY, gdir, velocity );
				AxisToAngles( axis, angles );

				CG_LaunchGib( cent, origin, angles, velocity, character->gibModels[gibIndex], 1.0, 0 );

				for( junction = 0; junction < MAXJUNCTIONS; ++junction ) {
					if( !Q_stricmp (gibTags[gibIndex], JunctiongibTags[junction]) ) {
						VectorCopy (origin, junctionOrigin[junction]);
						newjunction[junction] = qtrue;
					}
				}
			}
		}

		for( i=0; i<MAXJUNCTIONS; ++i ) {
			if( newjunction[i] == qtrue ) {
				for( j=0; j<MAXJUNCTIONS; ++j ) {
					if( !Q_stricmp (JunctiongibTags[j], ConnectTags[i]) ) {
						if( newjunction[j] == qtrue ) {
							// spawn a blood cloud somewhere on the vec from
							VectorSubtract (junctionOrigin[i], junctionOrigin[j], dir);
							CG_ParticleBloodCloud (cent, junctionOrigin[i], dir);
						}
					}
				}
			}
		}

		// Ridah, spawn a bunch of blood dots around the place
		#define GIB_BLOOD_DOTS	3
		for (i=0, count=0; i<GIB_BLOOD_DOTS*2; ++i) {
			if (i>0) {
				velocity[0] = ((i%2)*2 - 1)*(40 + 40*random());
				velocity[1] = (((i/2)%2)*2 - 1)*(40 + 40*random());
				velocity[2] = (((i<GIB_BLOOD_DOTS)*2)-1)*40;
			}
			else {
				VectorClear( velocity );
				velocity[2] = -64;
			}

			VectorAdd( playerOrigin, velocity, origin );

			CG_Trace( &trace, playerOrigin, NULL, NULL, origin, -1, CONTENTS_SOLID );
			if (trace.fraction < 1.0) {
					VectorSet( projection, 0, 0, -1 );
					projection[ 3 ] = 30.0f;
					Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
					trap_R_ProjectDecal( cgs.media.bloodDotShaders[ rand() % 5 ], 1, (vec3_t*) trace.endpos, projection, color,
						cg_bloodTime.integer * SECONDS_1, (cg_bloodTime.integer * SECONDS_1) >> 4 );
				if (count++ > GIB_BLOOD_DOTS)
					break;
			}
		}
	}

	if(!(cent->currentState.eFlags & EF_HEADSHOT)) {
		if ( cent->currentState.powerups & (1<<PW_HELMETSHIELD) ){
			CG_LoseACC( cent, tv(0, 0, 1), ACC_SHIELD, "tag_mouth", qtrue );
			CG_LoseACC( cent, tv(0, 0, 1), ACC_HAT, "tag_mouth", qtrue );
		}
		else {
			CG_LoseACC( cent, tv(0, 0, 1), ci->rank >= NUM_EXPERIENCE_LEVELS-1 ? ACC_MOUTH2 : ACC_HAT, "tag_mouth", qtrue );

		}
	}

	// try to throw out something more
	if( !(cent->currentState.eType == ET_CORPSE && cent->currentState.time2 == 1 ))
		CG_LoseACC( cent, tv(0, 0, 1), ACC_BACK, "tag_back", qfalse );
}

/*
==============
CG_RumbleEfx
==============
*/
void CG_RumbleEfx ( float pitch, float yaw ) {
	float	pitchRecoilAdd = 0;
	float	pitchAdd = 0;
	float	yawRandom = 0;
	vec3_t	recoil;

	if (pitch < 1) {
		pitch = 1;
	}

	pitchRecoilAdd = pow(random(),8)*(10 + VectorLength(cg.snap->ps.velocity)/5);
	pitchAdd = (rand()%(int)pitch) - (pitch*0.5);//5
	yawRandom = yaw;//2

	pitchRecoilAdd *= 0.5;
	pitchAdd *= 0.5;
	yawRandom *= 0.5;

	// calc the recoil

	// xkan, 11/04/2002 - the following used to be "recoil[YAW] = crandom()*yawRandom()"
	// but that seems to skew the effect either to the left or to the right for long streches
	// of time. The idea here is to keep it skewed for short period of time and then switches
	// to the other direction - switch the sign of recoil[YAW] when random() < 0.05 and keep the
	// sign otherwise. This seems better at balancing out the effect.
	if (cg.kickAVel[YAW] > 0) {
		if (random() < 0.05)
			recoil[YAW] = -random()*yawRandom;
		else
			recoil[YAW] = random()*yawRandom;
	}
	else if (cg.kickAVel[YAW] < 0) {
		if (random() < 0.05)
			recoil[YAW] = random()*yawRandom;
		else
			recoil[YAW] = -random()*yawRandom;
	}
	else {
		if (random() < 0.5)
			recoil[YAW] = random()*yawRandom;
		else
			recoil[YAW] = -random()*yawRandom;
	}

	recoil[ROLL] = -recoil[YAW];	// why not
	recoil[PITCH] = -pitchAdd;
	// scale it up a bit (easier to modify this while tweaking)
	VectorScale( recoil, 30, recoil );
	// set the recoil
	VectorCopy( recoil, cg.kickAVel );
	// set the recoil
	cg.recoilPitch -= pitchRecoilAdd;
}

#define MAX_SMOKESPRITES 512
#define SMOKEBOMB_SPAWNRATE 10
#define SMOKEBOMB_SMOKEVELOCITY ((640.f - 16.f)/8)/1000.f	// units per msec

typedef struct smokesprite_s {
	struct smokesprite_s *next;
	struct smokesprite_s *prev;				// this one is only valid for alloced smokesprites

	vec3_t pos;
	vec4_t colour;

	vec3_t dir;
	float dist;
	float size;
	centity_t *smokebomb;
} smokesprite_t;

static smokesprite_t SmokeSprites[MAX_SMOKESPRITES];
static int SmokeSpriteCount = 0;
static smokesprite_t *firstfreesmokesprite;			// pointer to the first free smokepuff in the SmokeSprites pool
static smokesprite_t *lastusedsmokesprite;			// pointer to the last used smokepuff

/*
==============
InitSmokeSprites
==============
*/
void InitSmokeSprites( void ) {
	int i;

	memset( &SmokeSprites, 0, sizeof(SmokeSprites) );
	for( i = 0; i < MAX_SMOKESPRITES - 1; ++i ) {
		SmokeSprites[i].next = &SmokeSprites[i+1];
	}

	firstfreesmokesprite = &SmokeSprites[0];
	lastusedsmokesprite = NULL;
	SmokeSpriteCount = 0;
}

/*
==============
AllocSmokeSprite
==============
*/
static smokesprite_t *AllocSmokeSprite( void ) {
	smokesprite_t *alloc;

	if( SmokeSpriteCount >= MAX_SMOKESPRITES )
		return( NULL );

	alloc = firstfreesmokesprite;

	firstfreesmokesprite = alloc->next;

	if( lastusedsmokesprite )
		lastusedsmokesprite->next = alloc;

	alloc->next = NULL;
	alloc->prev = lastusedsmokesprite;
	lastusedsmokesprite = alloc;

	SmokeSpriteCount++;
	return( alloc );
}

/*
==============
DeAllocSmokeSprite

	Returns previous alloced smokesprite in list (or NULL when there are no more alloced smokesprites left)
==============
*/
static smokesprite_t *DeAllocSmokeSprite( smokesprite_t *dealloc ) {
	smokesprite_t *ret_smokesprite;

	if( dealloc->prev )
		dealloc->prev->next = dealloc->next;

	if( dealloc->next )
		dealloc->next->prev = dealloc->prev;
	else { // no next particle, so this particle was 'lastusedsmokesprite'
		lastusedsmokesprite = dealloc->prev;
		if( lastusedsmokesprite ) // incase that there was no previous particle (happens when there is only one particle and this one gets dealloced)
			lastusedsmokesprite->next = NULL;
	}

	ret_smokesprite = dealloc->prev;

	memset( dealloc, 0, sizeof(smokesprite_t) );
	dealloc->next = firstfreesmokesprite;
	firstfreesmokesprite = dealloc;

	SmokeSpriteCount--;
	return( ret_smokesprite );
}

/*
==============
CG_SmokeSpritePhysics
==============
*/
static qboolean CG_SmokeSpritePhysics( smokesprite_t *smokesprite, const float dist ) {
	trace_t tr;
	vec3_t oldpos;
	//vec3_t mins, maxs;

	VectorCopy( smokesprite->pos, oldpos );
	VectorMA( oldpos, dist, smokesprite->dir, smokesprite->pos );

	smokesprite->dist += dist;

	smokesprite->size += 1.25f * dist;

	// see if we hit a solid
	// FIXME: use mins and max with smoke sprite  minimum radius and then expand to max possible distance or real current sprite size?
	// would definately look nice I think
	//VectorSet( maxs, .3f * smokesprite->size, .3f * smokesprite->size, .3f * smokesprite->size );
	//VectorNegate( maxs, mins );
	//CG_Trace( &tr, oldpos, mins, maxs, smokesprite->pos, -1, CONTENTS_SOLID );
	CG_Trace( &tr, oldpos, NULL, NULL, smokesprite->pos, -1, CONTENTS_SOLID );

	if( tr.fraction != 1.f ) {
		//float dot;

		if( smokesprite->dist < 24.f ) {
			return( qfalse );
		}
		VectorCopy( tr.endpos, smokesprite->pos );

		// bounce off
		//dot = DotProduct( smokesprite->dir, tr.plane.normal );
		//VectorMA( smokesprite->dir, -2*dot, tr.plane.normal, smokesprite->dir );
		//VectorScale( smokesprite->dir, .25f, smokesprite->dir );
	}// else {
	//	smokesprite->size += 1.25f * dist;
	//}
	return( qtrue );
}

/*
==============
CG_SpawnSmokeSprite
==============
*/
qboolean CG_SpawnSmokeSprite( centity_t *cent, float dist ) {
	smokesprite_t *smokesprite = AllocSmokeSprite();

	if( smokesprite ) {
		smokesprite->smokebomb = cent;
		VectorCopy( cent->origin2, smokesprite->pos );
		VectorCopy( bytedirs[rand()%NUMVERTEXNORMALS], smokesprite->dir );
		smokesprite->dir[2] *= .5f;
		smokesprite->size = 16.f;
		smokesprite->colour[0] = .35f; // + crandom() * .1f;
		smokesprite->colour[1] = smokesprite->colour[0];
		smokesprite->colour[2] = smokesprite->colour[0];
		smokesprite->colour[3] = .8f;

		// Advance sprite
		if( !CG_SmokeSpritePhysics( smokesprite, dist ) ) {
			DeAllocSmokeSprite( smokesprite );
			return( qfalse );
		} else {
			cent->miscTime++;
		}
	}

	return( qtrue );
}

/*
==============
CG_RenderSmokeGrenadeSmoke
==============
*/
void CG_RenderSmokeGrenadeSmoke( centity_t *cent, const weaponInfo_t *weapon )
{
	float spawnrate = ( 1.f / SMOKEBOMB_SPAWNRATE ) * SECONDS_1_F;

	if( cent->currentState.effect1Time == 16 ) {
		cent->miscTime = 0;
		cent->lastFuseSparkTime = 0;	// last spawn time
		cent->muzzleFlashTime = 0;		// delta time
		cent->dl_atten = 0;
		return;
	}

	if( cent->currentState.effect1Time > 16 ) {
		int volume = 16 + ((cent->currentState.effect1Time/640.f)*(100-16));
		int spritesNeeded = 0;

		if( !cent->dl_atten ||
			cent->currentState.pos.trType != TR_STATIONARY ||
			( cent->currentState.groundEntityNum != ENTITYNUM_WORLD && !VectorCompare( cent->lastLerpOrigin, cent->lerpOrigin ) ) ) {
			trace_t tr;

			VectorCopy( cent->lerpOrigin, cent->origin2 );
			cent->origin2[2] += 32;
			CG_Trace( &tr, cent->currentState.pos.trBase, NULL, NULL, cent->origin2, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				cent->dl_atten = 2;
			}
			else {
				VectorCopy( tr.endpos, cent->origin2 );
				cent->dl_atten = 1;
			}
		}

		trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, weapon->overheatSound, volume, 0 );

		// emitter is stuck in solid
		if( cent->dl_atten == 2 ) {
			return;
		}

		if( cg.oldTime && cent->lastFuseSparkTime != cg.time ) {
			cent->muzzleFlashTime += cg.frametime;
			spritesNeeded = cent->muzzleFlashTime / spawnrate;
			cent->muzzleFlashTime -= ( spawnrate * spritesNeeded  );
			cent->lastFuseSparkTime = cg.time;
		}


		if( !spritesNeeded )
			return;
		else if( spritesNeeded == 1 ) {
			// this is theoretically fine, till the smokegrenade ends up in a solid
			// while( !CG_SpawnSmokeSprite( cent, 0.f ) );

			// this is better
			if( !CG_SpawnSmokeSprite( cent, 0.f ) )
				// try again, just in case, so we don't get lots of gaps and remain quite constant
				CG_SpawnSmokeSprite( cent, 0.f );
		}
		else {
			float lerp = 1.0f;
			float dtime;

			for( dtime = spritesNeeded * spawnrate; dtime > 0; dtime-=spawnrate ) {
				if( !CG_SpawnSmokeSprite( cent, lerp * cg.frametime * SMOKEBOMB_SMOKEVELOCITY ) )
					// try again, just in case, so we don't get lots of gaps and remain quite constant
					CG_SpawnSmokeSprite( cent, lerp * cg.frametime * SMOKEBOMB_SMOKEVELOCITY );
			}
		}
	}
	else if ( cent->currentState.effect1Time == -1 ) {
		// unlink smokesprites from smokebomb
		if( cent->miscTime > 0 ) {
			smokesprite_t *smokesprite = lastusedsmokesprite;

			while( smokesprite ) {
				if( smokesprite->smokebomb == cent ) {
					smokesprite->smokebomb = NULL;
					cent->miscTime--;
				}

				smokesprite = smokesprite->prev;
			}
		}
	}
}

/*
==============
CG_AddSmokeSprites
==============
*/
void CG_AddSmokeSprites( void ) {
	smokesprite_t	*smokesprite = lastusedsmokesprite;
	qhandle_t		shader;
	byte			color[4];
	polyVert_t		verts[4];
	vec3_t			top, bottom, right, up, tmp;
	float			radius, halfradius, r;
	float			halfSmokeSpriteWidth, halfSmokeSpriteHeight;
	float			dist = SMOKEBOMB_SMOKEVELOCITY * cg.frametime;

	while( smokesprite ) {
		if( smokesprite->smokebomb && !smokesprite->smokebomb->currentValid ) {
			smokesprite = smokesprite->prev;
			continue;
		}

		// Do physics
		if( !CG_SmokeSpritePhysics( smokesprite, dist ) ) {
			if( smokesprite->smokebomb ) smokesprite->smokebomb->miscTime--;
			smokesprite = DeAllocSmokeSprite( smokesprite );
			continue;
		}

		if( smokesprite->smokebomb )
			radius = smokesprite->smokebomb->currentState.effect1Time;
		else
			radius = -1.f;

		if( radius < 0 ) radius = 640.f;	// max radius

		halfradius = radius * .5f;

		// Expire sprites
		if( smokesprite->dist > halfradius ) {
			if( smokesprite->smokebomb ) smokesprite->smokebomb->miscTime--;
			smokesprite = DeAllocSmokeSprite( smokesprite );
			continue;
		}

		// Now render it
		halfSmokeSpriteWidth = 0.5f * smokesprite->size;
		halfSmokeSpriteHeight = 0.5f * smokesprite->size;

		VectorCopy( cg.refdef_current->viewaxis[1], tmp );
		RotatePointAroundVector( right, cg.refdef_current->viewaxis[0], tmp, 0 );
		CrossProduct( cg.refdef_current->viewaxis[0], right, up );

		VectorMA( smokesprite->pos, halfSmokeSpriteHeight, up, top );
		VectorMA( smokesprite->pos, -halfSmokeSpriteHeight, up, bottom );

		color[0] = smokesprite->colour[0] * 0xff;
		color[1] = smokesprite->colour[1] * 0xff;
		color[2] = smokesprite->colour[2] * 0xff;
		color[3] = smokesprite->colour[3] * 0xff;

		// fadeout
		r = halfradius * .8f;
		if( smokesprite->dist > r ) {
			color[3] = (smokesprite->colour[3] -  smokesprite->colour[3] * ((smokesprite->dist - r)/(halfradius-r))) * 0xff;
		}
		else {
			color[3] = smokesprite->colour[3] * 0xff;
		}

		VectorMA( top, halfSmokeSpriteWidth, right, verts[0].xyz );
		verts[0].st[0] = 1;
		verts[0].st[1] = 0;
		memcpy( verts[0].modulate, color, 4 );

		VectorMA( top, -halfSmokeSpriteWidth, right, verts[1].xyz );
		verts[1].st[0] = 0;
		verts[1].st[1] = 0;
		memcpy( verts[1].modulate, color, 4 );

		VectorMA( bottom, -halfSmokeSpriteWidth, right, verts[2].xyz );
		verts[2].st[0] = 0;
		verts[2].st[1] = 1;
		memcpy( verts[2].modulate, color, 4 );

		VectorMA( bottom, halfSmokeSpriteWidth, right, verts[3].xyz );
		verts[3].st[0] = 1;
		verts[3].st[1] = 1;
		memcpy( verts[3].modulate, color, 4 );

		shader = cgs.media.smokePuffShader;

		trap_R_AddPolyToScene( shader, 4, verts );

		smokesprite = smokesprite->prev;
	}
}
