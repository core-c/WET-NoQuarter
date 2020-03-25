/*
**  Character loading
*/

#include "cg_local.h"

char bigTextBuffer[100000];


#ifdef __SSYSTEM__
void CG_LoadPcInfo( int cls, team_t team  )
{
	int handle;
	char filename[MAX_TOKEN_CHARS];
	char profile[MAX_CVAR_VALUE_STRING];
	pc_token_t token;

	//memset(&cgs.limboPcInfo, 0, sizeof(cgs.limboPcInfo)); -- this shouldn't be here

	trap_Cvar_VariableStringBuffer("cl_profile", profile, sizeof(profile));
	Q_strncpyz(filename, va("profiles/%s/playerCharacter_%s_%s.dat", profile, CG_TeamnameForNumber(team), BG_ClassnameForNumber_Filename(cls) ), MAX_TOKEN_CHARS);
	handle = trap_PC_LoadSource( filename );

	if(!handle) {
		CG_Printf("Unable to load playercharacter file: %s\n", filename);
		return;
	}

	while ( 1 ) {
		if ( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}
		if ( !Q_stricmp( token.string, "playerCharacter") ) {
			
			if ( !trap_PC_ReadToken( handle, &token ) )
				return;

			if ( !Q_stricmp( token.string, "{" ) ) {
				while ( 1 ) {
					if ( !trap_PC_ReadToken( handle, &token ) )
						break;

					if ( !Q_stricmp( token.string, "}" ) )
						break;

					if ( !Q_stricmp ( token.string, "body" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].bodyIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].torsoIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].legIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "head" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].headIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].faceIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "helmet" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].helmetIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].helmetSkinIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "backpack" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].backPackIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].backPackSkinIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "leftbelt" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].bLeftIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].bLeftSkinIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "rightbelt" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].bRightIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].bRightSkinIndex ) )
							continue;
					} else if ( !Q_stricmp ( token.string, "glasses" ) ) {
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].glassIndex ) )
							continue;
						if ( !PC_Int_Parse( handle, &cgs.limboPcInfo[cls][team-1].glassSkinIndex ) )
							continue;
					}      
				}
			}
		}   
	}
	trap_PC_FreeSource(handle);
}

void CG_SavePcInfo( int cls, team_t team ) {
	char path[MAX_QPATH];
	char buffer[1024];
	fileHandle_t handle;
	char profile[MAX_CVAR_VALUE_STRING];

	trap_Cvar_VariableStringBuffer("cl_profile", profile, sizeof(profile));

	Com_sprintf(path, sizeof(path), va("profiles/%s/playerCharacter_%s_%s.dat", profile, CG_TeamnameForNumber(team), BG_ClassnameForNumber_Filename(cls) ));

	if( trap_FS_FOpenFile(path, &handle, FS_WRITE ) < 0 ) {
		trap_FS_Write(buffer, strlen(buffer), handle);
		CG_Printf("Unable to open file: %s for writing\n", path);
		return;
	} 

	Com_sprintf(buffer, sizeof(buffer), "playerCharacter {\n");
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"body\" %i %i %i\n",
		cgs.limboPcInfo[cls][team-1].bodyIndex, cgs.limboPcInfo[cls][team-1].torsoIndex, cgs.limboPcInfo[cls][team-1].legIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"head\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].headIndex, cgs.limboPcInfo[cls][team-1].faceIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"helmet\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].helmetIndex, cgs.limboPcInfo[cls][team-1].helmetSkinIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"backpack\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].backPackIndex, cgs.limboPcInfo[cls][team-1].backPackSkinIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"leftbelt\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].bLeftIndex, cgs.limboPcInfo[cls][team-1].bLeftSkinIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"rightbelt\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].bRightIndex, cgs.limboPcInfo[cls][team-1].bRightSkinIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);

	Com_sprintf(buffer, sizeof(buffer), "  \"glasses\" %i %i\n",
		cgs.limboPcInfo[cls][team-1].glassIndex, cgs.limboPcInfo[cls][team-1].glassSkinIndex);
	trap_FS_Write(buffer, strlen(buffer), handle);


	Com_sprintf(buffer, sizeof(buffer), "}\n");
	trap_FS_Write(buffer, strlen(buffer), handle);



	trap_FS_FCloseFile(handle);
}

#endif
 
// TTimo - defined but not used - jaquboss reactivated
//#if 0
/*
======================
CG_ParseGibModels

Read a configuration file containing gib models for use with this character
======================
*/
static qboolean	CG_ParseGibModels( char *modelPath, bg_character_t *character ) {
	char		*text_p;
	int			len;
	int			i;
	char		*token;
	fileHandle_t	f;

	memset( character->gibModels, 0, sizeof(character->gibModels) );

	// load the file
	len = trap_FS_FOpenFile( va("%s.gibs", modelPath), &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( bigTextBuffer ) - 1 ) {
		CG_Printf( "File %s too long\n", va("%s.gibs", modelPath) );
		return qfalse;
	}
	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = bigTextBuffer;

	for (i=0; i<MAX_GIB_MODELS; i++) {
		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		// cache this model
		character->gibModels[i] = trap_R_RegisterModel( token );
	}

	return qtrue;
}
//#endif

/*
======================
CG_ParseHudHeadConfig
======================
*/
static qboolean	CG_ParseHudHeadConfig( const char *filename, animation_t* hha ) {
	char		*text_p;
	int			len;
	int			i;
	float		fps;
	char		*token;
	fileHandle_t	f;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if( len <= 0 ) {
		return qfalse;
	}

	if( len >= sizeof( bigTextBuffer ) - 1 ) {
		CG_Printf( "File %s too long\n", filename );
		return qfalse;
	}

	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = bigTextBuffer;

	for ( i = 0 ; i < MAX_HD_ANIMATIONS ; i++ ) {
		token = COM_Parse( &text_p );	// first frame
		if( !token ) {
			break;
		}
		hha[i].firstFrame = atoi( token );

		token = COM_Parse( &text_p );	// length
		if( !token ) {
			break;
		}
		hha[i].numFrames = atoi( token );

		token = COM_Parse( &text_p );	// fps
		if( !token ) {
			break;
		}
		fps = atof( token );
		if ( fps == 0 ) {
			fps = 1;
		}

		hha[i].frameLerp = 1000 / fps;
		hha[i].initialLerp = 1000 / fps;

		token = COM_Parse( &text_p );	// looping frames
		if( !token ) {
			break;
		}
		hha[i].loopFrames = atoi( token );

		if( hha[i].loopFrames > hha[i].numFrames ) {
			hha[i].loopFrames = hha[i].numFrames;
		} else if( hha[i].loopFrames < 0 ) {
			hha[i].loopFrames = 0;
		}
	}

	if( i != MAX_HD_ANIMATIONS ) {
		CG_Printf( "Error parsing hud head animation file: %s", filename );
		return qfalse;
	}

	return qtrue;
}

/*
==================
CG_CalcMoveSpeeds
==================
*/
static void CG_CalcMoveSpeeds( bg_character_t *character )
{
	char			*tags[2] = {"tag_footleft", "tag_footright"};
	vec3_t			oldPos[2];
	refEntity_t		refent;
	animation_t		*anim;
	int				i, j, k;
	float			totalSpeed;
	int				numSpeed;
	int				lastLow, low;
	orientation_t	o[2];

	memset( &refent, 0, sizeof(refent) );

#ifndef __SSYSTEM__
	refent.hModel = character->mesh;
#else
	refent.hModel = character->body[0].mesh;
#endif

	for( i = 0; i < character->animModelInfo->numAnimations; i++ ) {
		anim = character->animModelInfo->animations[i];

		if( anim->moveSpeed >= 0 ) {
			continue;
		}

		totalSpeed = 0;
		lastLow = -1;
		numSpeed = 0;

		// for each frame
		for( j = 0; j < anim->numFrames; j++ ) {

			refent.frame = anim->firstFrame + j;
			refent.oldframe = refent.frame;
			refent.torsoFrameModel = refent.oldTorsoFrameModel = refent.frameModel = refent.oldframeModel = anim->mdxFile;

			// for each foot
			for( k = 0; k < 2; k++ ) {
				if( trap_R_LerpTag( &o[k], &refent, tags[k], 0 ) < 0 ) {
					CG_Error( "CG_CalcMoveSpeeds: unable to find tag %s, cannot calculate movespeed", tags[k] );
				}
			}

			// find the contact foot
			if( anim->flags & ANIMFL_LADDERANIM ) {
				if( o[0].origin[0] > o[1].origin[0] )
					low = 0;
				else
					low = 1;
				totalSpeed += fabs( oldPos[low][2] - o[low].origin[2] );
			} else {
				if( o[0].origin[2] < o[1].origin[2] )
					low = 0;
				else
					low = 1;
				totalSpeed += fabs( oldPos[low][0] - o[low].origin[0] );
			}

			numSpeed++;

			// save the positions
			for( k = 0; k < 2; k++ ) {
				VectorCopy( o[k].origin, oldPos[k] );
			}
			lastLow = low;
		}

		// record the speed
		anim->moveSpeed = (int)((totalSpeed/numSpeed) * 1000.0 / anim->frameLerp);
	}
}

/*
======================
CG_ParseAnimationFiles

  Read in all the configuration and script files for this model.
======================
*/
static qboolean CG_ParseAnimationFiles( bg_character_t *character, const char *animationGroup, const char *animationScript )
{
	char			filename[MAX_QPATH];
	fileHandle_t	f;
	int				len;

	// set the name of the animationGroup and animationScript in the animModelInfo structure
	Q_strncpyz( character->animModelInfo->animationGroup, animationGroup, sizeof(character->animModelInfo->animationGroup) );
	Q_strncpyz( character->animModelInfo->animationScript, animationScript, sizeof(character->animModelInfo->animationScript) );

	BG_R_RegisterAnimationGroup( animationGroup, character->animModelInfo );

	// calc movespeed values if required
	CG_CalcMoveSpeeds( character );

	// load the script file
	len = trap_FS_FOpenFile( animationScript, &f, FS_READ );
	if( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( bigTextBuffer ) - 1 ) {
		CG_Printf( "File %s is too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( character->animModelInfo, &cgs.animScriptData, animationScript, bigTextBuffer );

	return qtrue;
}

/*
==================
CG_CheckForExistingAnimModelInfo

  If this player model has already been parsed, then use the existing information.
  Otherwise, set the modelInfo pointer to the first free slot.

  returns qtrue if existing model found, qfalse otherwise
==================
*/
static qboolean CG_CheckForExistingAnimModelInfo( const char *animationGroup, const char *animationScript, animModelInfo_t **animModelInfo )
{
	int i;
	animModelInfo_t *trav, *firstFree = NULL;

	for( i = 0, trav = cgs.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
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
		CG_Error( "unable to find a free modelinfo slot, cannot continue\n" );
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
CG_RegisterAcc
==============
*/
static qboolean CG_RegisterAcc( const char *modelName, int *model, const char* skinname, qhandle_t* skin ) {
	char	filename[MAX_QPATH];

	*model = trap_R_RegisterModel( modelName );

	if( !*model )
		return qfalse;

	COM_StripExtension( modelName, filename );
	Q_strcat( filename, sizeof(filename), va( "_%s.skin", skinname ) );
	*skin = trap_R_RegisterSkin( filename );

	return qtrue;
}
#ifndef __SSYSTEM__
typedef struct {
	char		*type;
	accType_t	index;
} acc_t;

static acc_t cg_accessories[] = {
	{ "md3_beltr",		ACC_BELT_LEFT },
	{ "md3_beltl",		ACC_BELT_RIGHT },
	{ "md3_belt",		ACC_BELT },
	{ "md3_back",		ACC_BACK },
	{ "md3_weapon",		ACC_WEAPON },
	{ "md3_weapon2",	ACC_WEAPON2 },
};
static int cg_numAccessories = sizeof(cg_accessories) / sizeof(cg_accessories[0]);

static acc_t cg_headAccessories[] = {
	{ "md3_hat",		ACC_HAT },
	{ "md3_rank",		ACC_RANK },
	{ "md3_hat2",		ACC_MOUTH2 },
	{ "md3_hat3",		ACC_MOUTH3 },
};

static int cg_numHeadAccessories = sizeof(cg_headAccessories) / sizeof(cg_headAccessories[0]);


#endif

/*
====================
CG_RegisterCharacter
====================
*/
qboolean CG_RegisterCharacter( const char *characterFile, bg_character_t *character )
{
	bg_characterDef_t	characterDef;
	char *filename;
	char buf[MAX_QPATH];
#ifndef __SSYSTEM__
	char accessoryname[MAX_QPATH];
#else
	int					j;
#endif
	int					i;

	
	memset( &characterDef, 0, sizeof(characterDef) );

	if( !BG_ParseCharacterFile( characterFile, &characterDef ) ) {
		return qfalse;	// the parser will provide the error message
	}

#ifdef __SSYSTEM__ 
	for ( i = 0 ; i < characterDef.numBodies ; i++ ) {
		if( !(character->body[i].mesh = trap_R_RegisterModel( characterDef.body[i].mesh )) )
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register mesh for body %i, '%s', referenced from '%s'\n", i, characterDef.body[i].mesh, characterFile );
		for ( j = 0 ; j < characterDef.body[i].numLegsSkins; j++ ){
			if( !(character->body[i].legsSkin[j] = trap_R_RegisterSkin( characterDef.body[i].legsSkin[j] )) )
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register legsSkin for body %i, '%s', referenced from '%s'\n", i, characterDef.body[i].legsSkin[j], characterFile );
		}
		for ( j = 0 ; j < characterDef.body[i].numTorsoSkins; j++ ){
			if( !(character->body[i].torsoSkin[j] = trap_R_RegisterSkin( characterDef.body[i].torsoSkin[j] )) )
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register torsoSkin for body %i, '%s', referenced from '%s'\n", i, characterDef.body[i].torsoSkin[j], characterFile );
		}
		character->body[i].numLegsSkins = characterDef.body[i].numLegsSkins;
		character->body[i].numTorsoSkins = characterDef.body[i].numTorsoSkins;
	}
	for ( i = 0 ; i < characterDef.numHeads ; i++ ) {
		if( !(character->head[i].model = trap_R_RegisterModel( characterDef.head[i].model )) )
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register model for head %i, '%s', referenced from '%s'\n", i, characterDef.head[i].model, characterFile );
		if( !CG_ParseHudHeadConfig( characterDef.head[i].animations, character->head->animations ) )
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head animations '%s' referenced from '%s'\n", characterDef.head[i].animations, characterFile );

		for ( j = 0 ; j < characterDef.head[i].numSkins ; j++ ){
			if( !(character->head[i].skin[j] = trap_R_RegisterSkin( characterDef.head[i].skin[j] )) )
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register skin for head %i, '%s', referenced from '%s'\n", i, characterDef.head[i].skin[j], characterFile );
		}	
		character->head[i].numSkins = characterDef.head[i].numSkins;
	}
	for ( i = 0 ; i < ACC_MAX ; i++ ) {
		character->numAcc[i] = characterDef.numAcc[i];
		
		for ( j = 0 ; j < characterDef.numAcc[i] ; j++ ) {
			int k;

			if( !(character->acc[i][j].model = trap_R_RegisterModel( characterDef.acc[i][j].model )) )
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register model for acc '%s' referenced from '%s'\n", characterDef.acc[i][j].model, characterFile );
			for ( k = 0 ; k < characterDef.acc[i][j].numSkins ; k++ ){
				if( !(character->acc[i][j].skin[k] = trap_R_RegisterSkin( characterDef.acc[i][j].skin[k] )) )
					CG_Printf( S_COLOR_YELLOW "WARNING: failed to register skin for acc '%s' referenced from '%s'\n", characterDef.acc[i][j].skin[k], characterFile );
			}	
			character->acc[i][j].numSkins = characterDef.acc[i][j].numSkins;
		}
	}
	character->numBodies = characterDef.numBodies;
	character->numHeads = characterDef.numHeads;
#else
	// Register Mesh
	if( !(character->mesh = trap_R_RegisterModel( characterDef.mesh )) )
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register mesh '%s' referenced from '%s'\n", characterDef.mesh, characterFile );

	// Register Skin
	COM_StripExtension( characterDef.mesh, buf );
	filename = va( "%s_%s.skin", buf, characterDef.skin );
	if( !(character->skin = trap_R_RegisterSkin( filename )) ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register skin '%s' referenced from '%s'\n", filename, characterFile );
	} else {
		for( i = 0; i < cg_numAccessories; i++ ) {
			if( trap_R_GetSkinModel( character->skin, cg_accessories[i].type, accessoryname ) ) {
				if( !CG_RegisterAcc( accessoryname, &character->accModels[cg_accessories[i].index], characterDef.skin, &character->accSkins[cg_accessories[i].index] ) ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: failed to register accessory '%s' referenced from '%s'->'%s'\n", accessoryname, characterFile, filename );
				}
			}
		}

		for( i = 0; i < cg_numHeadAccessories; i++ ) {
			if( trap_R_GetSkinModel( character->skin, cg_headAccessories[i].type, accessoryname ) ) {
				if( !CG_RegisterAcc( accessoryname, &character->accModels[cg_headAccessories[i].index], characterDef.skin, &character->accSkins[cg_headAccessories[i].index] ) ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: failed to register accessory '%s' referenced from '%s'->'%s'\n", accessoryname, characterFile, filename );
				}
			}
		}
	}
#endif

#ifndef __SSYSTEM__
	COM_StripExtension( characterDef.mesh, buf );
#else 
	COM_StripExtension( characterDef.body[0].mesh, buf );
#endif

	// jaquboss - gib models
	CG_ParseGibModels( buf, character ); // it is here so we can use modelpath from above


	// Register Undressed Corpse Media
	if( *characterDef.undressedCorpseModel ) {
		// Register Undressed Corpse Model
		if( !(character->undressedCorpseModel = trap_R_RegisterModel( characterDef.undressedCorpseModel )) )
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register undressed corpse model '%s' referenced from '%s'\n", characterDef.undressedCorpseModel, characterFile );

		// Register Undressed Corpse Skin
		COM_StripExtension( characterDef.undressedCorpseModel, buf );
		filename = va( "%s_%s.skin", buf, characterDef.undressedCorpseSkin );
		if( !(character->undressedCorpseSkin = trap_R_RegisterSkin( filename )) )
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register undressed corpse skin '%s' referenced from '%s'\n", filename, characterFile );
	}

	// Register the head for the hud
#ifndef __SSYSTEM__
	if( !(character->hudhead = trap_R_RegisterModel( characterDef.hudhead )) ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head model '%s' referenced from '%s'\n", characterDef.hudhead, characterFile );
	}

	if( !(character->hudheadskin = trap_R_RegisterSkin( characterDef.hudheadskin )) ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head skin '%s' referenced from '%s'\n", characterDef.hudheadskin, characterFile );
	}

	if( !CG_ParseHudHeadConfig( characterDef.hudheadanims, character->hudheadanimations ) ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head animations '%s' referenced from '%s'\n", characterDef.hudheadanims, characterFile );
	}
#endif
	// Parse Animation Files
	if( !CG_CheckForExistingAnimModelInfo( characterDef.animationGroup, characterDef.animationScript, &character->animModelInfo ) ) {
		if( !CG_ParseAnimationFiles( character, characterDef.animationGroup, characterDef.animationScript ) ) {
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to load animation files referenced from '%s'\n", characterFile );
			return qfalse;
		}
	}


	return qtrue;
}

bg_character_t *CG_CharacterForClientinfo( clientInfo_t *ci, centity_t *cent )
{
	int		team, cls;

	if( cent && cent->currentState.eType == ET_CORPSE ) {
		if( cent->currentState.onFireStart >= 0 )
			return cgs.gameCharacters[ cent->currentState.onFireStart ];
		else {
			if( cent->currentState.modelindex < 4 )
				return BG_GetCharacter( cent->currentState.modelindex, cent->currentState.modelindex2 );
			else
				return BG_GetCharacter( cent->currentState.modelindex - 4, cent->currentState.modelindex2 );
		}
	}

	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
		team = ci->team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		cls = ( cent->currentState.powerups >> PW_OPS_CLASS_1 ) & 7;

		return BG_GetCharacter( team, cls );
	}

	if( ci->character ) {
		return ci->character;
	}

    return BG_GetCharacter( ci->team, ci->cls );
}

bg_character_t *CG_CharacterForPlayerstate( playerState_t* ps ) {
	int	team, cls;

	if( ps->powerups[PW_OPS_DISGUISED] ) {
		team = cgs.clientinfo[ps->clientNum].team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		cls = 0;
		if( ps->powerups[PW_OPS_CLASS_1] ) {
			cls |= 1;
		}
		if( ps->powerups[PW_OPS_CLASS_2] ) {
			cls |= 2;
		}
		if( ps->powerups[PW_OPS_CLASS_3] ) {
			cls |= 4;
		}

		return BG_GetCharacter( team, cls );
	}

	return BG_GetCharacter( cgs.clientinfo[ps->clientNum].team, cgs.clientinfo[ps->clientNum].cls );
}

/*
========================
CG_RegisterPlayerClasses
========================
*/
void CG_RegisterPlayerClasses( void )
{
	bg_playerclass_t	*classInfo;
	bg_character_t		*character;
	int					team, cls;

	for( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ ) {
		for( cls = PC_SOLDIER; cls < NUM_PLAYER_CLASSES; cls++ ) {
			classInfo = BG_GetPlayerClassInfo( team, cls );
			character = BG_GetCharacter( team, cls );

			Q_strncpyz( character->characterFile, classInfo->characterFile, sizeof(character->characterFile) );

			if( !CG_RegisterCharacter( character->characterFile, character ) ) {
				CG_Error( "ERROR: CG_RegisterPlayerClasses: failed to load character file '%s' for the %s %s\n", character->characterFile, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum ) );
			}

			if( !(classInfo->icon = trap_R_RegisterShaderNoMip( classInfo->iconName )) ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to load class icon '%s' for the %s %s\n", classInfo->iconName, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum ) );
			}

			if( !(classInfo->arrow = trap_R_RegisterShaderNoMip( classInfo->iconArrow ))) {
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to load icon arrow '%s' for the %s %s\n", classInfo->iconArrow, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum ) );
			}
		}
	}
}
