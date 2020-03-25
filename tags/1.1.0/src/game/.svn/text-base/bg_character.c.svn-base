/*
**  Character definition code
*/

#include "q_shared.h"
#include "bg_public.h"

bg_character_t	alliedClassCharacters[NUM_PLAYER_CLASSES];
bg_character_t	axisClassCharacters[NUM_PLAYER_CLASSES];
#ifdef __SSYSTEM__
acc_t bg_accessories[] = {
	{ "md3_beltr",		ACC_BELT_LEFT },
	{ "md3_beltl",		ACC_BELT_RIGHT },
	{ "md3_belt",		ACC_BELT },
	{ "md3_back",		ACC_BACK },
//	{ "md3_weapon",		ACC_WEAPON },
//	{ "md3_weapon2",	ACC_WEAPON2 },
	{ "md3_hat",		ACC_HAT },
	{ "md3_rank",		ACC_RANK },
	{ "md3_hat2",		ACC_MOUTH2 },
	{ "md3_hat3",		ACC_MOUTH3 },
};
#endif

static qboolean BG_PCF_ParseError( int handle, char *format, ... )
{
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );

	Com_Printf( S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string );

	trap_PC_FreeSource( handle );

	return qfalse;
}
#ifdef __SSYSTEM__
static qboolean BG_ParseBody( int handle, bodyDef_t *body )
{
	pc_token_t	token;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) )
		return BG_PCF_ParseError( handle, "expected '{'" );

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) )
			break;

		if( token.string[0] == '}' )
			break;

		if( !Q_stricmp( token.string, "mesh" ) ) {
			if( !PC_String_ParseNoAlloc( handle, body->mesh, sizeof(body->mesh) ) )
				return BG_PCF_ParseError( handle, "expected mesh filename" );
		} else if( !Q_stricmp( token.string, "torsoSkin" ) ) {
			if( body->numTorsoSkins == MAX_CHARACTER_INDEXES )
				BG_PCF_ParseError( handle, "only up to %i torsoSkin supported per body\n", MAX_CHARACTER_INDEXES );

			if( !PC_String_ParseNoAlloc( handle, body->torsoSkin[body->numTorsoSkins], MAX_QPATH))
				return BG_PCF_ParseError( handle, "expected torsoSkin filename" );
					
			body->numTorsoSkins++;
		} else if( !Q_stricmp( token.string, "legsSkin" ) ) {
			if( body->numLegsSkins == MAX_CHARACTER_INDEXES )
				BG_PCF_ParseError( handle, "only up to %i legsSkin supported per body\n", MAX_CHARACTER_INDEXES );

			if( !PC_String_ParseNoAlloc( handle, body->legsSkin[body->numLegsSkins], MAX_QPATH))
				return BG_PCF_ParseError( handle, "expected legsSkin filename" );
					
			body->numLegsSkins++;
		} else {
			return BG_PCF_ParseError( handle, "unknown token '%s'", token.string );
		}
	}

	return qtrue;
}

static qboolean BG_ParseHead( int handle, headDef_t *head )
{
	pc_token_t	token;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return BG_PCF_ParseError( handle, "expected '{'" );
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) )
			break;

		if( token.string[0] == '}' )
			break;

		if( !Q_stricmp( token.string, "model" ) ) {
			if( !PC_String_ParseNoAlloc( handle, head->model, sizeof(head->model) ) )
				return BG_PCF_ParseError( handle, "expected model filename" );
		} else if( !Q_stricmp( token.string, "skin" ) ) {
			if( head->numSkins == MAX_CHARACTER_INDEXES )
				BG_PCF_ParseError( handle, "only up to %i skins supported per head\n", MAX_CHARACTER_INDEXES );

			if( !PC_String_ParseNoAlloc( handle, head->skin[head->numSkins], MAX_QPATH))
				return BG_PCF_ParseError( handle, "expected skin filename" );
					
			head->numSkins++;
		} else if( !Q_stricmp( token.string, "animations" ) ) {
			if( !PC_String_ParseNoAlloc( handle, head->animations, sizeof(head->animations) ) )
				return BG_PCF_ParseError( handle, "expected animations filename" );
		} else {
			return BG_PCF_ParseError( handle, "unknown token '%s'", token.string );
		}
	}

	return qtrue;
}

static qboolean BG_ParseAcc( int handle, accDef_t *acc )
{
	pc_token_t	token;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return BG_PCF_ParseError( handle, "expected '{'" );
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) )
			break;

		if( token.string[0] == '}' )
			break;

		if( !Q_stricmp( token.string, "model" ) ) {
			if( !PC_String_ParseNoAlloc( handle, acc->model, sizeof(acc->model) ) )
				return BG_PCF_ParseError( handle, "expected model filename" );
		} else if( !Q_stricmp( token.string, "skin" ) ) {
			if( acc->numSkins == MAX_CHARACTER_INDEXES )
				BG_PCF_ParseError( handle, "only up to %i skins supported per acc\n", MAX_CHARACTER_INDEXES );

			if( !PC_String_ParseNoAlloc( handle, acc->skin[acc->numSkins], MAX_QPATH))
				return BG_PCF_ParseError( handle, "expected skin filename" );
					
			acc->numSkins++;
		} else {
			return BG_PCF_ParseError( handle, "unknown token '%s'", token.string );
		}
	}

	acc->registred = qtrue;
	return qtrue;
}

#endif 

qboolean BG_ParseCharacterFile( const char *filename, bg_characterDef_t* characterDef )
{
	pc_token_t	token;
	int			handle;
#ifdef __SSYSTEM__
	int			i;
#endif

	handle = trap_PC_LoadSource( filename );

	if( !handle )
		return qfalse;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "characterDef" ) )
		return BG_PCF_ParseError( handle, "expected 'characterDef'" );

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) 
		return BG_PCF_ParseError( handle, "expected '{'" );

	while( 1 ) 
	{
		if( !trap_PC_ReadToken( handle, &token ) ) 
			break;

		if( token.string[0] == '}' ) 
			break;

		if( !Q_stricmp( token.string, "animationGroup" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->animationGroup, sizeof(characterDef->animationGroup) ) ) 
				return BG_PCF_ParseError( handle, "expected animationGroup filename" );
		} else if( !Q_stricmp( token.string, "animationScript" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->animationScript, sizeof(characterDef->animationScript) ) ) 
				return BG_PCF_ParseError( handle, "expected animationScript filename" );
		} else if( !Q_stricmp( token.string, "undressedCorpseModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->undressedCorpseModel, sizeof(characterDef->undressedCorpseModel) ) ) 
				return BG_PCF_ParseError( handle, "expected undressedCorpseModel filename" );
		} else if( !Q_stricmp( token.string, "undressedCorpseSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->undressedCorpseSkin, sizeof(characterDef->undressedCorpseSkin) ) )
				return BG_PCF_ParseError( handle, "expected undressedCorpseSkin filename" );
		} 
#ifdef __SSYSTEM__
		else if( !Q_stricmp( token.string, "body" ) ){
			if( !BG_ParseBody( handle, &characterDef->body[characterDef->numBodies] ) ) {
				return BG_PCF_ParseError( handle, "failed to load body struct" );
			} else {
				characterDef->numBodies++;
			}
		} else if( !Q_stricmp( token.string, "head" ) ){
			if( !BG_ParseHead( handle, &characterDef->head[characterDef->numHeads] ) ) {
				return BG_PCF_ParseError( handle, "failed to load head struct" );
			} else {
				characterDef->numHeads++;
			}
		} else if ( strstr( token.string, "md3_" )){
			for ( i = 0 ; i < NUM_ACC ; i++ ) {
				if (!Q_stricmp( token.string, bg_accessories[i].type)) {
					int index = bg_accessories[i].index;

					if( !BG_ParseAcc( handle, &characterDef->acc[index][characterDef->numAcc[index]] ) )
						return BG_PCF_ParseError( handle, "failed to load acc struct" );
					else
						characterDef->numAcc[index]++;		
				}
			}
		}
#else
		else if( !Q_stricmp( token.string, "mesh" ) ){
			if( !PC_String_ParseNoAlloc( handle, characterDef->mesh, sizeof(characterDef->mesh) ) ) 
				return BG_PCF_ParseError( handle, "expected mesh filename" );
		} else if( !Q_stricmp( token.string, "skin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->skin, sizeof(characterDef->skin) ) ) 
				return BG_PCF_ParseError( handle, "expected skin filename" );
		} else if( !Q_stricmp( token.string, "hudhead" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->hudhead, sizeof(characterDef->hudhead) ) )
				return BG_PCF_ParseError( handle, "expected hudhead filename" );
		} else if( !Q_stricmp( token.string, "hudheadskin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->hudheadskin, sizeof(characterDef->hudheadskin) ) )
				return BG_PCF_ParseError( handle, "expected hudheadskin filename" );

		} else if( !Q_stricmp( token.string, "hudheadanims" ) ) {
			if( !PC_String_ParseNoAlloc( handle, characterDef->hudheadanims, sizeof(characterDef->hudheadanims) ) )
				return BG_PCF_ParseError( handle, "expected hudheadanims filename" );
		}
#endif
		 else {
			return BG_PCF_ParseError( handle, "unknown token '%s'", token.string );
		}

	}

	trap_PC_FreeSource( handle );

	return qtrue;
}

bg_character_t *BG_GetCharacter( int team, int cls ) {
	switch( team ) {
		default:
		case TEAM_AXIS:
			return &axisClassCharacters[cls];
		case TEAM_ALLIES:
			return &alliedClassCharacters[cls];
	}
}

bg_character_t *BG_GetCharacterForPlayerstate( playerState_t *ps ) {
	// FIXME: add disguise?
	switch( ps->persistant[PERS_TEAM] ) {
		default:
		case TEAM_AXIS:
			return &axisClassCharacters[ps->stats[STAT_PLAYER_CLASS]];
		case TEAM_ALLIES:
			return &alliedClassCharacters[ps->stats[STAT_PLAYER_CLASS]];
	}
}

//
// Character Pool - used for custom characters
//

bg_character_t	bg_characterPool[MAX_CHARACTERS];
qboolean		bg_characterPoolInuse[MAX_CHARACTERS];

void BG_ClearCharacterPool( void )
{
	memset( &bg_characterPool, 0, sizeof(bg_characterPool) );
	memset( &bg_characterPoolInuse, 0, sizeof(bg_characterPoolInuse) );
}

bg_character_t *BG_FindFreeCharacter( const char *characterFile )
{
	int i;

	// see if we already got it
	for( i = 0; i < MAX_CHARACTERS; i++ ) {
		if( !bg_characterPoolInuse[i] )
			continue;

		if( !Q_stricmp( characterFile, bg_characterPool[i].characterFile ) ) {
			return &bg_characterPool[i];
		}
	}

	// else get a free one
	for( i = 0; i < MAX_CHARACTERS; i++ ) {
		if( !bg_characterPoolInuse[i] ) {
			bg_characterPoolInuse[i] = qtrue;
			Q_strncpyz( bg_characterPool[i].characterFile, characterFile, sizeof(bg_characterPool[i].characterFile) );
			return &bg_characterPool[i];
		}
	}

	// should never get here
	return NULL;
}

bg_character_t *BG_FindCharacter( const char *characterFile )
{
	int i;

	// see if we already got it
	for( i = 0; i < MAX_CHARACTERS; i++ ) {
		if( !bg_characterPoolInuse[i] )
			continue;

		if( !Q_stricmp( characterFile, bg_characterPool[i].characterFile ) ) {
			return &bg_characterPool[i];
		}
	}

	// nope didn't find it
	return NULL;
}
