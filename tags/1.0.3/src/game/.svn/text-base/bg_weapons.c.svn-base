#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#else
#include "q_shared.h"
#include "bg_public.h"
#endif // CGAMEDLL

#ifdef GAMEDLL
#include "g_local.h"
#endif //GAMEDLL

#include "bg_local.h"


#ifdef GAMEDLL	// client has this already
typedef enum {
	W_PART_1,
	W_PART_2,
	W_PART_3,
	W_PART_4,
	W_PART_5,
	W_PART_6,
	W_PART_7,
	W_PART_8,
	W_PART_9,
	W_PART_10,
	W_PART_11,
	W_PART_12,
	W_PART_13,
	W_PART_14,
	W_PART_15,
	W_PART_16,
	W_MAX_PARTS
} barrelType_t;
#endif

extern char *nq_wpNames[];

static qboolean BG_RW_ParseError( int handle, char *format, ... )
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

/*
======================
BG_ParseWeaponConfig
	read information for weapon animations (first/length/fps)
======================
*/
static qboolean	BG_ParseWeaponConfig( const char *filename, BGweaponInfo_t *wi, ammotable_t *ammoTable ) {
	char		*text_p, *prev;
	int			len;
	int			i;
	float		fps;
	char		*token;
	qboolean	newfmt = qfalse;	//----(SA)	
	char		text[20000];
	fileHandle_t	f;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}

	if ( len >= sizeof( text ) - 1 ) {
		Com_Printf( "File %s too long\n", filename ); // fixme all, repair thse warnings
		return qfalse;
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;

	// read optional parameters
	while ( 1 ) {
		prev = text_p;	// so we can unget
		token = COM_Parse( &text_p );
		if ( !token ) {						// get the variable
			break;
		}


		if ( !Q_stricmp( token, "newfmt" ) ) {
			newfmt = qtrue;
			continue;
		}

		// if it is a number, start parsing animations
		if ( token[0] >= '0' && token[0] <= '9' ) {
			text_p = prev;	// unget the token
			break;
		}
		Com_Printf( "unknown token in weapon cfg '%s' is %s\n", token, filename );
	}

	for ( i = 0 ; i < MAX_WP_ANIMATIONS  ; i++ ) {

		token = COM_Parse( &text_p );	// first frame
		if ( !token ) break;
		wi->weapAnimations[i].firstFrame = atoi( token );

		token = COM_Parse( &text_p );	// length
		if ( !token ) break;
		wi->weapAnimations[i].numFrames = atoi( token );

		token = COM_Parse( &text_p );	// fps
		if ( !token ) break;
		fps = atof( token );
		if ( fps == 0 ) {
			fps = 1;
		}

//		wi->weapAnimations[i].time = wi->weapAnimations[i].numFrames * fps; // omg we thought this is correct :S
		wi->weapAnimations[i].time = 1000/fps * wi->weapAnimations[i].numFrames;


		wi->weapAnimations[i].frameLerp = 1000 / fps;
		wi->weapAnimations[i].initialLerp = 1000 / fps;

		token = COM_Parse( &text_p );	// looping frames
		if ( !token ) break;
		wi->weapAnimations[i].loopFrames = atoi( token );
		if(wi->weapAnimations[i].loopFrames > wi->weapAnimations[i].numFrames)
			wi->weapAnimations[i].loopFrames = wi->weapAnimations[i].numFrames;
		else if(wi->weapAnimations[i].loopFrames < 0)
			wi->weapAnimations[i].loopFrames = 0;


		// store animation/draw bits in '.moveSpeed'

		wi->weapAnimations[i].moveSpeed = 0;

		if(newfmt) {
			token = COM_Parse( &text_p );	// barrel anim bits
			if ( !token ) break;
			wi->weapAnimations[i].moveSpeed = atoi(token);

			token = COM_Parse( &text_p );	// animated weapon
			if ( !token ) break;
			if(atoi(token))
				wi->weapAnimations[i].moveSpeed |= (1<<W_MAX_PARTS);	// set the bit one higher than can be set by the barrel bits

			token = COM_Parse( &text_p );	// barrel hide bits (so objects can be flagged to not be drawn during all sequences (a reloading hand that comes in from off screen for that one animation for example)
			if ( !token ) break;
			wi->weapAnimations[i].moveSpeed |= ((atoi(token))<<8 );	// use 2nd byte for draw bits
		}

	}


	

	if ( i != MAX_WP_ANIMATIONS ) {
		Com_Printf( "Error parsing weapon animation file: %s", filename );
		return qfalse;
	}


	return qtrue;
}

static qboolean BG_RW_ParseClient( int handle, BGweaponInfo_t *weaponInfo, ammotable_t *ammoTable )		// For now will get needed weapon animations , but l8er it will read all stuff and has a lot of ifdefs
{
	pc_token_t	token;
	char		filename[MAX_QPATH];
//	int			i;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return qfalse;
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}

		if( token.string[0] == '}' ) {
			break;
		}

		 if( !Q_stricmp( token.string, "weaponConfig" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return BG_RW_ParseError( handle, "missing weaponConfig string!!!" ); // note put error here!
			} else {//5 6
				if( !BG_ParseWeaponConfig( filename, weaponInfo, ammoTable ) ) { // using ammotable but i think we dont need it there right now
				return BG_RW_ParseError( handle, "error in weaponConfig!!!" ); // note put error here!
				}
			}
		} 
	}

	return qtrue;
}


static qboolean BG_RW_ParseDefinition( int handle, BGweaponInfo_t *weaponInfo, ammotable_t *ammoTable/*, qboolean noparse*/ )		// For now will get needed weapon animations , but l8er it will read all stuff and has a lot of ifdefs
{
	pc_token_t	token;
	char		text[64];


	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return qfalse;
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}

		if( token.string[0] == '}' ) {
			break;
		}


		if (!Q_stricmp( token.string, "name" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->name, sizeof(weaponInfo->name) )) {
				return BG_RW_ParseError( handle, "name needs string" );
			}
		}  else if( !Q_stricmp( token.string, "damage" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->damage ) ) {
				return BG_RW_ParseError( handle, "damage needs int" );
			}
		}  else if( !Q_stricmp( token.string, "splashdamage" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->splashdamage ) ) {
				return BG_RW_ParseError( handle, "splashdamage needs int" );
			}
		}  else if( !Q_stricmp( token.string, "splashdamage_radius" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->splashdamage_radius ) ) {
				return BG_RW_ParseError( handle, "splashdamage_radius needs int" );
			}
		}  else if( !Q_stricmp( token.string, "spread" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->spread ) ) {
				return BG_RW_ParseError( handle, "spread needs int" );
			}
		}
		// ammo table filling 
		else if( !Q_stricmp( token.string, "coolRate" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->coolRate ) ) {
				return BG_RW_ParseError( handle, "coolRate needs int" );
			}
		} else if( !Q_stricmp( token.string, "defaultStartingAmmo" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->defaultStartingAmmo ) ) {
				return BG_RW_ParseError( handle, "defaultStartingAmmo needs int" );
			}
		} else if( !Q_stricmp( token.string, "defaultStartingClip" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->defaultStartingClip ) ) {
				return BG_RW_ParseError( handle, "defaultStartingClip needs int" );
			}
		} else if( !Q_stricmp( token.string, "fireDelayTime" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->fireDelayTime ) ) {
				return BG_RW_ParseError( handle, "fireDelayTime needs int" );;
			}
		} else if( !Q_stricmp( token.string, "maxammo" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxammo ) ) {
				return BG_RW_ParseError( handle, "maxammo needs int" );
			}
		} else if( !Q_stricmp( token.string, "maxclip" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxclip ) ) {
				return BG_RW_ParseError( handle, "maxclip needs int" );
			}
		} else if( !Q_stricmp( token.string, "maxHeat" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxHeat ) ) {
				return BG_RW_ParseError( handle, "maxHeat needs int" );
			}
		} else if( !Q_stricmp( token.string, "mod" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->mod ) ) {
				return BG_RW_ParseError( handle, "mod needs int" );
			}
		} else if( !Q_stricmp( token.string, "nextShotTime" ) /*&& !noparse */ ) {
			if( !PC_Int_Parse( handle, &ammoTable->nextShotTime ) ) {
				return BG_RW_ParseError( handle, "nextShotTime needs int" );
			}
		} else if( !Q_stricmp( token.string, "reloadTime" ) /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &ammoTable->reloadTime ) ) { // note kinda useless
				return BG_RW_ParseError( handle, "reloadTime needs int" );
			}
		} 
		// end		
		

		else if( !Q_stricmp( token.string, "SpreadScaleAdd" )  /*&& !noparse */ ) {
			if( !PC_Int_Parse( handle, &weaponInfo->aimSpreadScaleAdd ) ) {
				return BG_RW_ParseError( handle, "SpreadScaleAdd needs int" );
			}
		} else if( !Q_stricmp( token.string, "SpreadScaleAddRand")  /*&& !noparse*/  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->aimSpreadScaleAddRand ) ) {
				return BG_RW_ParseError( handle, "aimSpreadScaleAddRand needs int" );
			}
		} else if( !Q_stricmp( token.string, "recoilDuration" ) /*&& !noparse*/  ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Int_Parse( handle, &weaponInfo->recoilDuration ) ) {
				return BG_RW_ParseError( handle, "recoilDuration needs int" );
			} 
		} else if( !Q_stricmp( token.string, "recoilYaw" ) /*&& !noparse*/  ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Float_Parse( handle, &weaponInfo->recoilYaw )   ) {
				return BG_RW_ParseError( handle, "recoilYaw needs float" );
			} 
		} else if( !Q_stricmp( token.string, "recoilPitch" ) /*&& !noparse*/  ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Float_Parse( handle, &weaponInfo->recoilPitch )  ) {
				return BG_RW_ParseError( handle, "recoilPitch needs float" );
			} 
		} else if( !Q_stricmp( token.string, "CrouchSpreadRatio" ) /*&& !noparse */ ) {
			if( !PC_Float_Parse( handle, &weaponInfo->CrouchSpreadRatio )  ) {
				return BG_RW_ParseError( handle, "ProneSpreadRatio needs float" );
			} 
		} else if( !Q_stricmp( token.string, "ProneSpreadRatio" ) /*&& !noparse */ ) {
			if( !PC_Float_Parse( handle, &weaponInfo->ProneSpreadRatio )   ) {
				return BG_RW_ParseError( handle, "ProneSpreadRatio needs float" );
			} 
		} else if( !Q_stricmp( token.string, "limboKill" ) /*&& !noparse */ ) {
			weaponInfo->limboKill = qtrue;
		} else if( !Q_stricmp( token.string, "bulletReflection" ) /*&& !noparse */ ) {
			weaponInfo->bulletReflection = qtrue;
		} else if( !Q_stricmp( token.string, "noMidclipReload" ) /*&& !noparse */ ) {
			weaponInfo->noMCReload = qtrue;
		} else if (!Q_stricmp( token.string, "selfKillMessage" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->selfKillMessage, sizeof(weaponInfo->selfKillMessage) )) {
				return BG_RW_ParseError( handle, "selfKillMessage needs string" );
			}
		} else if (!Q_stricmp( token.string, "KillMessage" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->KillMessage, sizeof(weaponInfo->KillMessage) )) {
				return BG_RW_ParseError( handle, "KillMessage needs string" );
			}
		} else if (!Q_stricmp( token.string, "KillMessage2" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->KillMessage2, sizeof(weaponInfo->KillMessage2) )) {
				return BG_RW_ParseError( handle, "KillMessage2 needs string" );
			}
		} else if (!Q_stricmp( token.string, "HeadshotWeapon" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text) )) {
				return BG_RW_ParseError( handle, "HeadshotWeapon needs 'yes' or 'no'" );
			} else {
				if( !Q_stricmp( text, "yes" ) ) {
					weaponInfo->headShot = WEAPON_YES;
				} else if( !Q_stricmp( text, "no" ) ) {
					weaponInfo->headShot = WEAPON_NO;
				} else {
					Com_Printf("expected yes/no \n");
					weaponInfo->headShot = WEAPON_DEFAULT;
				}
			}
		} else if (!Q_stricmp( token.string, "DistanceFalloff" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text) )) {
				return BG_RW_ParseError( handle, "DistanceFalloff needs 'yes' or 'no'" );
			} else {
				if( !Q_stricmp( text, "yes" ) ) {
					weaponInfo->falloff = WEAPON_YES;
				} else if( !Q_stricmp( text, "no" ) ) {
					weaponInfo->falloff = WEAPON_NO;
				} else {
					Com_Printf("expected yes/no \n");
					weaponInfo->falloff = WEAPON_DEFAULT;
				}
			}
		}  else if (!Q_stricmp( token.string, "GibbingWeapon" ) /*&& !noparse*/ ){
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text) )) {
				return BG_RW_ParseError( handle, "GibbingWeapon needs 'yes' or 'no'" );
			} else {
				if( !Q_stricmp( text, "yes" ) ) {
					weaponInfo->gib = WEAPON_YES;
				} else if( !Q_stricmp( text, "no" ) ) {
					weaponInfo->gib = WEAPON_NO;
				} else {
					Com_Printf("expected yes/no \n");
					weaponInfo->gib = WEAPON_DEFAULT;
				}
			}
		} else if( !Q_stricmp( token.string, "movementSpeedScale" ) /*&& !noparse*/  ) {
			if( !PC_Float_Parse( handle, &weaponInfo->movementSpeedScale )  ) {
				return BG_RW_ParseError( handle, "movementSpeedScale needs float" );
			} 
		}
	}

	return qtrue;
}

static qboolean BG_RW_ParseEmpty( int handle)		// For now will get needed weapon animations , but l8er it will read all stuff and has a lot of ifdefs
{
	pc_token_t	token;


	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return qfalse;
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}

		if( token.string[0] == '}' ) {
			break;
		}

	}
	return qtrue;
}


static qboolean BG_RegisterWeaponFromWeaponFile( int handle, BGweaponInfo_t *weaponInfo, ammotable_t *ammoTable, qboolean altWeap, int weaponNum )
{
	pc_token_t token;

	if( !handle )
		return qfalse;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "weaponDef" ) ) {
		return BG_RW_ParseError( handle, "expected 'weaponDef'" );
	}

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return qfalse;
			return BG_RW_ParseError( handle, "expected '{'" );
	}

	while( 1 ) {
		if( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}

		if( token.string[0] == '}' ) {
			break;
		}

		if ( !Q_stricmp( token.string, "client" ) ) { // for now
			if( !BG_RW_ParseClient( handle, weaponInfo, ammoTable ) ) {
			//	BG_RW_ParseError( handle, "Parsing client for weapon %s from %s failed\n", nq_wpNames[weaponNum], filename );
			}
		}
		
		if ( !Q_stricmp( token.string, "both" ) ) { 
			if ( altWeap )
			{
				BG_RW_ParseEmpty( handle );
			} else if( !BG_RW_ParseDefinition( handle, weaponInfo, ammoTable ) ) {
			//	BG_RW_ParseError( handle, "Parsing both for weapon %s from %s failed\n", nq_wpNames[weaponNum], filename );
			} else {
#ifdef DEBUG
			//	Com_Printf( S_COLOR_BLUE "Parsed both for weapon %s from %s\n", nq_wpNames[weaponNum], filename );		
#endif
			}
		} 

		if ( !Q_stricmp( token.string, "both_altweap" ) ) { 
			if ( !altWeap )
			{
				BG_RW_ParseEmpty( handle );
			} else if( !BG_RW_ParseDefinition( handle, weaponInfo, ammoTable) ) {
			//	BG_RW_ParseError( handle, "Parsing both_altweap for weapon %s from %s failed\n", nq_wpNames[weaponNum], filename );
			} else {
#ifdef DEBUG
			//	Com_Printf( S_COLOR_BLUE "Parsed both_altweap for weapon %s from %s\n", nq_wpNames[weaponNum], filename );	
#endif
			}
		} 
		
	}

	trap_PC_FreeSource( handle );

	return qtrue;
}


/*
=================
G_RegisterWeapon

@TODO, we will want to move ALL here and also search for EVERY weapon file
=================
*/
void BG_RegisterWeapon( int weaponNum, qboolean force) {
	BGweaponInfo_t	*weaponInfo;
	char			*filename;
	ammotable_t *ammoTable;
	qboolean	altWeap=qfalse;
	int			i, handle=-1;

	if( weaponNum <= 0 || weaponNum >= WP_NUM_WEAPONS )
		return;

	weaponInfo = &BG_Weapons[weaponNum];
	ammoTable = &ammoTableMP[weaponNum]; // jaquboss, make it easy 

	if( weaponInfo->registered && !force ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;
	weaponInfo->weaponIndex = weaponNum;

	switch( weaponNum ) {
		case WP_STG44:					filename = "StG44.weap"; break;
		case WP_STEN_MKII:				filename = "stenMk2.weap"; break;
		case WP_BAR_SET:				altWeap = qtrue; 
		case WP_BAR:					filename = "bar.weap";  break;
		case WP_BAZOOKA:				filename = "bazooka.weap"; break;		
		case WP_KNIFE_KABAR:			filename = "knife_kbar.weap"; break;
		case WP_MOBILE_BROWNING_SET:	altWeap = qtrue;		
		case WP_MOBILE_BROWNING:		filename = "browning.weap";	break;
		case WP_SHOTGUN:				filename = "shotgun.weap"; break;	
		case WP_MP34:					filename = "MP34.weap";	break;
		case WP_KNIFE:					filename = "knife.weap"; break;		
		case WP_LUGER:					filename = "luger.weap"; break;
		case WP_COLT:					filename = "colt.weap"; break;
		case WP_MP40:					filename = "mp40.weap"; break;
		case WP_THOMPSON:				filename = "thompson.weap"; break;		
		case WP_STEN:					filename = "sten.weap"; break;
		case WP_GRENADE_LAUNCHER:		filename = "grenade.weap"; break;
		case WP_GRENADE_PINEAPPLE:		filename = "pineapple.weap"; break;
		case WP_PANZERFAUST:			filename = "panzerfaust.weap"; break;
		case WP_FLAMETHROWER:			filename = "flamethrower.weap"; break;
		case WP_AMMO:					filename = "ammopack.weap"; break;
		case WP_SMOKETRAIL:				filename = "smoketrail.weap"; break;
		case WP_MEDKIT:					filename = "medpack.weap"; break;
		case WP_PLIERS:					filename = "pliers.weap"; break;
		case WP_SMOKE_MARKER:			filename = "smokemarker.weap"; break;
		case WP_DYNAMITE:				filename = "dynamite.weap"; break;
		case WP_MEDIC_ADRENALINE:		filename = "adrenaline.weap"; break;
		case WP_MEDIC_SYRINGE:			filename = "syringe.weap"; break;
		case WP_BINOCULARS:				filename = "binocs.weap"; break;
		case WP_KAR98:					filename = "kar98.weap"; break;
		case WP_GPG40:					filename = "gpg40.weap"; break;
		case WP_CARBINE:				filename = "m1_garand.weap"; break;
		case WP_M7:						filename = "m7.weap"; break;
		case WP_GARAND_SCOPE:			altWeap = qtrue;
		case WP_GARAND:					filename = "m1_garand_s.weap"; break;
		case WP_FG42_SCOPE:				altWeap = qtrue;
		case WP_FG42:					filename = "fg42.weap"; break;
		case WP_LANDMINE:				filename = "landmine.weap"; break;
		case WP_SATCHEL:				filename = "satchel.weap"; break;
		case WP_SATCHEL_DET:			filename = "satchel_det.weap"; break;
		case WP_SMOKE_BOMB:				filename = "smokegrenade.weap"; break;		
		case WP_MOBILE_MG42_SET:		altWeap = qtrue;
		case WP_MOBILE_MG42:			filename = "mg42.weap"; break;
		case WP_SILENCER:				filename = "silenced_luger.weap"; break;
		case WP_SILENCED_COLT:			filename = "silenced_colt.weap"; break;
		case WP_K43_SCOPE:				altWeap = qtrue;			
		case WP_K43:					filename = "k43.weap"; break;
		case WP_MORTAR:					filename = "mortar.weap"; break;
		case WP_MORTAR_SET:				filename = "mortar_set.weap"; break;
		case WP_MORTAR2:				filename = "axis_mortar.weap"; break;
		case WP_MORTAR2_SET:			filename = "axis_mortar_set.weap"; break;
		case WP_AKIMBO_LUGER:			filename = "akimbo_luger.weap"; break;
		case WP_AKIMBO_SILENCEDLUGER:	filename = "akimbo_silenced_luger.weap"; break;
		case WP_AKIMBO_COLT:			filename = "akimbo_colt.weap"; break;
		case WP_AKIMBO_SILENCEDCOLT:	filename = "akimbo_silenced_colt.weap"; break;
//		case WP_MAPMORTAR:				filename = "mapmortar.weap"; break;	// do we really need this?
		case WP_FIREBOLT:				filename = "firebolt.weap"; break;
		case WP_VENOM:					filename = "venom.weap"; break;

		default:						 return;
	}

#ifdef CGAMEDLL
	for ( i=0; i < cgs.numWeaponScriptsDir; i++ ){

		handle = trap_PC_LoadSource( va( "%s/%s", cgs.weaponScriptsDir[i], filename ) );
		
		if ( handle )
			break;
	}
#else
	for ( i=0; i < level.numWeaponScriptsDir; i++ ){
		handle = trap_PC_LoadSource( va( "%s/%s", level.weaponScriptsDir[i], filename ) );
		
		if ( handle )
			break;
	}
#endif

#ifdef CGAMEDLL
	if( !handle || cgs.numWeaponScriptsDir < 1 )
#else
	if( !handle || level.numWeaponScriptsDir < 1 )
#endif
		handle = trap_PC_LoadSource( va( "weapons/%s", filename ) );

	BG_RegisterWeaponFromWeaponFile( handle, weaponInfo, ammoTable,altWeap, weaponNum );

}
