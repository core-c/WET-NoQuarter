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

/*
======================
BG_InitWeaponInfo

Fills data structure for weaponStatsInfo_t
======================
*/
void BG_InitWeaponInfo ( int weapon )
{
	int i;
    // Basicly fill whole structure with default data
    BG_Weapons[weapon].registered = qtrue;
    BG_Weapons[weapon].aimSpreadScaleAdd = 0;
    // Reset Animations WEAP_IDLE1 is first
    for ( i = WEAP_IDLE1; i < MAX_WP_ANIMATIONS; ++i) {
        BG_Weapons[weapon].animationTime[i] = 0;
    }
    // Weapon name
    memset( BG_Weapons[weapon].name, 0, sizeof( BG_Weapons[weapon].name ) );
    // Default damage is 1
	BG_Weapons[weapon].damage = 1;
	BG_Weapons[weapon].spread = 0;
    // No recoil by default
	BG_Weapons[weapon].recoil = qfalse;
	BG_Weapons[weapon].recoilDuration = 0;
	BG_Weapons[weapon].recoilYaw = 0.0f;
	BG_Weapons[weapon].recoilPitch = 0.0f;
    // None of weapons have this set, for scripting purposes only
	BG_Weapons[weapon].CrouchSpreadRatio = 1.0f;
	BG_Weapons[weapon].ProneSpreadRatio = 1.0f;
    // By default do not change spread when firing
	BG_Weapons[weapon].aimSpreadScaleAdd = 0;
	BG_Weapons[weapon].aimSpreadScaleAddRand = 0;
    // Do not really know what is point of this
	BG_Weapons[weapon].weaponIndex = weapon;
    // Flamethrower killing people instantly
	BG_Weapons[weapon].limboKill = qfalse;
    // Unused, for scripting purposes
	BG_Weapons[weapon].bulletReflection = qfalse;
	BG_Weapons[weapon].noMCReload = qfalse;
    // Movement speed when carrying weapon, default 1.0f
	BG_Weapons[weapon].movementSpeedScale = 1.0f;
    // Obituaries, FIXME fill weapon files and force them to be used from there
    memset( BG_Weapons[weapon].selfKillMessage, 0, sizeof( BG_Weapons[weapon].selfKillMessage ) );
    memset( BG_Weapons[weapon].KillMessage, 0, sizeof( BG_Weapons[weapon].KillMessage ) );
    memset( BG_Weapons[weapon].KillMessage2, 0, sizeof( BG_Weapons[weapon].KillMessage2 ) );
    // Other behaviour data
	BG_Weapons[weapon].headShot = qfalse;
	BG_Weapons[weapon].gib = qfalse;
	BG_Weapons[weapon].falloff = qfalse;
    // Splash Damage, usualy equals to damage, set to 0
	BG_Weapons[weapon].splashdamage = 0;
	BG_Weapons[weapon].splashdamage_radius = 0;
    // Minimal Headshot Damage, default is 50
	BG_Weapons[weapon].minHSDamage = 50;
    // Multiplier used when calculating Headshot damage, by default it is 2.0f
	BG_Weapons[weapon].HSRatio = 2.0f;
    // Spread increase when moving with mouse
	BG_Weapons[weapon].wpnScale = 0.f;
    // Weapon is converting player's velocity to addition spread
	BG_Weapons[weapon].velocity2spread = qfalse;
    // Weapon is converting mouse movement to additional spread
	BG_Weapons[weapon].viewchange2spread = qfalse;
	// Short name for use in weaponstats
    memset( BG_Weapons[weapon].statname, 0, sizeof( BG_Weapons[weapon].statname ) );
}

/*
======================
 BG_RW_ParseError
======================
*/
static qboolean BG_RW_ParseError( int handle, char *format, ... )
{
	int line = 0;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );

	filename[0] = '\0';

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
static qboolean	BG_ParseWeaponConfig( const char *filename, weaponStatsInfo_t *wi, weapontable_t *ammoTable ) {
	char			*text_p, *prev;
	int				i;
    int             frames;
	float			fps;
	char			*token;
	char			text[20000];
	fileHandle_t	f;
    qboolean        newfmt=qfalse;
    int				len = trap_FS_FOpenFile( filename, &f, FS_READ ); // load the file

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

	for ( i = WEAP_IDLE1 ; i < MAX_WP_ANIMATIONS  ; ++i ) {

		token = COM_Parse( &text_p );	// first frame
		if ( !token ) break;

		token = COM_Parse( &text_p );	// length
		if ( !token ) break;
		frames = atoi( token );

		token = COM_Parse( &text_p );	// fps
		if ( !token ) break;
		fps = atof( token );
		if ( fps == 0 ) {
			fps = 1;
		}

        wi->animationTime[i] = 1000/fps * frames;

		token = COM_Parse( &text_p );	// looping frames
		if ( !token ) break;

		if(newfmt) {
			token = COM_Parse( &text_p );	// barrel anim bits
			if ( !token ) break;
			token = COM_Parse( &text_p );	// animated weapon
			if ( !token ) break;
			token = COM_Parse( &text_p );
			if ( !token ) break;
		}
	}

	if ( i != MAX_WP_ANIMATIONS ) {
		Com_Printf( "Error parsing weapon animation file: %s", filename );
		return qfalse;
	}

	return qtrue;
}

/*
======================
BG_RW_ParseClient
======================
*/
static qboolean BG_RW_ParseClient( int handle, weaponStatsInfo_t *weaponInfo, weapontable_t *ammoTable, int weaponNum )		// For now will get needed weapon animations , but l8er it will read all stuff and has a lot of ifdefs
{
	pc_token_t	token;
	char		filename[MAX_QPATH];
#ifdef CGAMEDLL
	weaponInfo_t	*clientWeaponInfo;
    int i=0;

	clientWeaponInfo = &cg_weapons[weaponNum];
	memset( clientWeaponInfo, 0, sizeof( *clientWeaponInfo ) );
	clientWeaponInfo->registered = qtrue;
#endif


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
			}
			else {
				if( !BG_ParseWeaponConfig( filename, weaponInfo, ammoTable ) ) { // using ammotable but i think we dont need it there right now
				    return BG_RW_ParseError( handle, "error in weaponConfig!!!" ); // note put error here!
				}
#ifdef CGAMEDLL
				if( !CG_ParseWeaponConfig( filename, clientWeaponInfo ) ) {
				    return CG_RW_ParseError( handle, "error in weaponConfig!!!" );
				}
#endif
			}
		}
#ifdef CGAMEDLL
		else if( !Q_stricmp( token.string, "standModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected standModel filename" );
			}
			else {
				clientWeaponInfo->standModel = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "droppedAnglesHack" ) ) {
			clientWeaponInfo->droppedAnglesHack = qtrue;
		} else if( !Q_stricmp( token.string, "pickupModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupModel filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].model[0] = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "axisPickupModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupModel filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].model[TEAM_AXIS] = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "alliedPickupModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupModel filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].model[TEAM_ALLIES] = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "pickupSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupSkin filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].skin[0] = trap_R_RegisterSkin( filename );
			}
		} else if( !Q_stricmp( token.string, "axisPickupSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupSkin filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].skin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		} else if( !Q_stricmp( token.string, "alliedPickupSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupSkin filename" );
			} else {
				clientWeaponInfo->weaponModel[W_PU_MODEL].skin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		} else if( !Q_stricmp( token.string, "pickupSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected pickupSound filename" );
			}
		} else if( !Q_stricmp( token.string, "handsModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected handsModel filename" );
			} else {
				clientWeaponInfo->handsModel = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "flashDlightColor" ) ) {
			if( !PC_Vec_Parse( handle, &clientWeaponInfo->flashDlightColor ) ) {
				return CG_RW_ParseError( handle, "expected flashDlightColor as r g b" );
			}
		} else if( !Q_stricmp( token.string, "flashSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected flashSound filename" );
			} else {
				for( i = 0; i < 4; ++i ) {
					if( !clientWeaponInfo->flashSound[i] ) {
						clientWeaponInfo->flashSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}
				if( i == 4 )
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 flashSounds supported per weapon\n" );
			}
		} else if( !Q_stricmp( token.string, "flashEchoSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected flashEchoSound filename" );
			} else {
				for( i = 0; i < 4; ++i ) {
					if( !clientWeaponInfo->flashEchoSound[i] ) {
						clientWeaponInfo->flashEchoSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}
				if( i == 4 )
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 flashEchoSounds supported per weapon\n" );
			}
		} else if( !Q_stricmp( token.string, "lastShotSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected lastShotSound filename" );
			} else {
				for( i = 0; i < 4; ++i ) {
					if( !clientWeaponInfo->lastShotSound[i] ) {
						clientWeaponInfo->lastShotSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}
				if( i == 4 )
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 lastShotSound supported per weapon\n" );
			}
		} else if( !Q_stricmp( token.string, "deathBySound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected deathBySound filename" );
			} else {
				for( i = 0; i < 4; ++i ) {
					if( !clientWeaponInfo->deathBySound[i] ) {
						clientWeaponInfo->deathBySound[i] = trap_S_RegisterSound( filename, qfalse );
						clientWeaponInfo->deathBySoundNum = i+1;
						break;
					}
				}
				if( i == 4 )
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 deathBySound supported per weapon\n" );
			}
		} else if( !Q_stricmp( token.string, "deathBySoundFar" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected deathBySoundFar filename" );
			} else {
				for( i = 0; i < 4; ++i ) {
					if( !clientWeaponInfo->deathBySoundFar[i] ) {
						clientWeaponInfo->deathBySoundFar[i] = trap_S_RegisterSound( filename, qfalse );
						clientWeaponInfo->deathBySoundFarNum = i+1;
						break;
					}
				}
				if( i == 4 )
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 deathBySoundFar supported per weapon\n" );
			}
		}  else if( !Q_stricmp( token.string, "readySound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected readySound filename" );
			} else {
				clientWeaponInfo->readySound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "firingSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected firingSound filename" );
			} else {
				clientWeaponInfo->firingSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "overheatSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected overheatSound filename" );
			} else {
				clientWeaponInfo->overheatSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "reloadSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected reloadSound filename" );
			} else {
				clientWeaponInfo->reloadSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "reloadFastSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected reloadFastSound filename" );
			} else {
				clientWeaponInfo->reloadFastSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "spinupSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected spinupSound filename" );
			} else {
				clientWeaponInfo->spinupSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "spindownSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected spindownSound filename" );
			} else {
				clientWeaponInfo->spindownSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "switchSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected switchSound filename" );
			} else {
				clientWeaponInfo->switchSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "bounceSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected bounceSound filename" );
			} else {
				clientWeaponInfo->bounceSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "clipBounceSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected clipBounceSound filename" );
			} else {
				clientWeaponInfo->clipBounceSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "weaponIcon" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected weaponIcon filename" );
			} else {
				clientWeaponInfo->weaponIcon[0] = trap_R_RegisterShader( filename );
			}
		}  else if( !Q_stricmp( token.string, "weaponIconScale" ) ) {
			if( !PC_Int_Parse( handle, &clientWeaponInfo->weaponIconScale ) ) {
				return CG_RW_ParseError( handle, "expected weapon Icon Scale" );
			}
		} else if( !Q_stricmp( token.string, "weaponSelectedIcon" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected weaponSelectedIcon filename" );
			} else {
				clientWeaponInfo->weaponIcon[1] = trap_R_RegisterShader( filename );
			}
		} else if( !Q_stricmp( token.string, "missileModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected missileModel filename" );
			} else {
				clientWeaponInfo->missileModel[0] = trap_R_RegisterModel( filename );
			}
		} else if( !Q_stricmp( token.string, "AxisMissileModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected missileModel filename" );
			} else {
				clientWeaponInfo->missileModel[TEAM_AXIS] = trap_R_RegisterModel( filename );
				clientWeaponInfo->axiesMissileModel = qtrue;
			}
		} else if( !Q_stricmp( token.string, "AlliedMissileModel" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected missileModel filename" );
			} else {
				clientWeaponInfo->missileModel[TEAM_ALLIES] = trap_R_RegisterModel( filename );
				clientWeaponInfo->alliedMissileModel = qtrue;
			}
		} else if( !Q_stricmp( token.string, "missileAlliedSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				clientWeaponInfo->missileAlliedSkin = trap_R_RegisterSkin( filename );
			}
		} else if( !Q_stricmp( token.string, "missileAxisSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				clientWeaponInfo->missileAxisSkin = trap_R_RegisterSkin( filename );
			}
		} else if( !Q_stricmp( token.string, "missileSound" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected missileSound filename" );
			} else {
				clientWeaponInfo->missileSound = trap_S_RegisterSound( filename, qfalse );
			}
		} else if( !Q_stricmp( token.string, "missileTrailFunc" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected missileTrailFunc" );
			} else {
				if( !Q_stricmp( filename, "GrenadeTrail" ) ) {
					clientWeaponInfo->missileTrailFunc = CG_GrenadeTrail;
				} else if( !Q_stricmp( filename, "RocketTrail" ) ) {
					clientWeaponInfo->missileTrailFunc = CG_RocketTrail;
				} else if( !Q_stricmp( filename, "PyroSmokeTrail" ) ) {
					clientWeaponInfo->missileTrailFunc = CG_PyroSmokeTrail;
				} else if( !Q_stricmp( filename, "DynamiteTrail" ) ) {
					clientWeaponInfo->missileTrailFunc = CG_DynamiteTrail;
				}
			}
		} else if( !Q_stricmp( token.string, "missileDlight" ) ) {
			if( !PC_Float_Parse( handle, &clientWeaponInfo->missileDlight ) ) {
				return CG_RW_ParseError( handle, "expected missileDlight value" );
			}
		} else if( !Q_stricmp( token.string, "missileDlightColor" ) ) {
			if( !PC_Vec_Parse( handle, &clientWeaponInfo->missileDlightColor ) ) {
				return CG_RW_ParseError( handle, "expected missileDlightColor as r g b" );
			}
		} else if( !Q_stricmp( token.string, "ejectBrassFunc" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected ejectBrassFunc" );
			} else {
				if( !Q_stricmp( filename, "MachineGunEjectBrass" ) ) {
					clientWeaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
				} else if( !Q_stricmp( filename, "PanzerFaustEjectBrass" ) ) {
					clientWeaponInfo->ejectBrassFunc = CG_PanzerFaustEjectBrass;
				}
			}
		} else if( !Q_stricmp( token.string, "flashSmokeSize" ) ) {
				if( !PC_Float_Parse( handle, &clientWeaponInfo->flashSmokeSize ) ) {
					return CG_RW_ParseError( handle, "expected flashSmokeSize value" );
				}
		} else if( !Q_stricmp( token.string, "flashSmokeAlpha" ) ) {
				if( !PC_Float_Parse( handle, &clientWeaponInfo->flashSmokeAlpha ) ) {
					return CG_RW_ParseError( handle, "expected flashSmokeAlpha value" );
				}
		} else if( !Q_stricmp( token.string, "flashSmokeLife" ) ) {
				if( !PC_Int_Parse( handle, &clientWeaponInfo->flashSmokeLife ) ) {
					return CG_RW_ParseError( handle, "expected flashSmokeLife int" );
				}
		} else if( !Q_stricmp( token.string, "flashSmokeShader" ) ) {
				if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
					return CG_RW_ParseError( handle, "expected flashSmokeShader filename" );
				} else {
					clientWeaponInfo->flashSmokeShader = trap_R_RegisterShader( filename );
				}
		} else if( !Q_stricmp( token.string, "recoil_pitchAdd" ) ) {
				if( !PC_Int_Parse( handle, &clientWeaponInfo->recoil_pitchAdd ) ) {
					return CG_RW_ParseError( handle, "expected recoil_pitchAdd int" );
				}
		} else if( !Q_stricmp( token.string, "recoil_yawRandom" ) ) {
				if( !PC_Int_Parse( handle, &clientWeaponInfo->recoil_yawRandom ) ) {
					return CG_RW_ParseError( handle, "expected recoil_yawRandom int" );
				}
		} else if( !Q_stricmp( token.string, "recoil_pitchAddRandom" ) ) {
				if( !PC_Int_Parse( handle, &clientWeaponInfo->recoil_pitchAddRandom ) ) {
					return CG_RW_ParseError( handle, "expected recoil_pitchAddRandom int" );
				}
		} else if( !Q_stricmp( token.string, "brassModel" ) ) {
				if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
					return CG_RW_ParseError( handle, "expected brassModel filename" );
				} else {
					clientWeaponInfo->brassModel = trap_R_RegisterModel( filename );
				}

		} else if( !Q_stricmp( token.string, "offset" ) ) {
			if( !PC_Vec_Parse( handle, &clientWeaponInfo->offset ) ) {
				return CG_RW_ParseError( handle, "expected offset as x y z" );
			}
		} else if( !Q_stricmp( token.string, "foreShorten" ) ) {
				if( !PC_Float_Parse( handle, &clientWeaponInfo->foreShorten ) ) {
					return CG_RW_ParseError( handle, "expected foreShorten value" );
				}
		} else if( !Q_stricmp( token.string, "modModel" ) ) {
			if( !CG_RW_ParseModModel( handle, clientWeaponInfo ) )
				return qfalse;
		} else if( !Q_stricmp( token.string, "firstPerson" ) ) {
			if( !CG_RW_ParseViewType( handle, clientWeaponInfo, W_FP_MODEL ) )
				return qfalse;
		} else if( !Q_stricmp( token.string, "thirdPerson" ) ) {
			if( !CG_RW_ParseViewType( handle, clientWeaponInfo, W_TP_MODEL ) )
				return qfalse;
		} else {
			return CG_RW_ParseError( handle, "unknown token '%s'", token.string );
		}
#endif
	}
	return qtrue;
}


char *modNames[] = {
	"MOD_UNKNOWN",
	"MOD_MACHINEGUN",
	"MOD_BROWNING",
	"MOD_MG42",
	"MOD_GRENADE",
	"MOD_KNIFE",
	"MOD_LUGER",
	"MOD_COLT",
	"MOD_MP40",
	"MOD_THOMPSON",
	"MOD_STEN",
	"MOD_GARAND",
	"MOD_SILENCER",
	"MOD_FG42",
	"MOD_FG42_SCOPE",
	"MOD_PANZERFAUST",
	"MOD_GRENADE_LAUNCHER",
	"MOD_FLAMETHROWER",
	"MOD_GRENADE_PINEAPPLE",
	"MOD_MAPMORTAR",
	"MOD_MAPMORTAR_SPLASH",
	"MOD_KICKED",
	"MOD_DYNAMITE",
	"MOD_AIRSTRIKE",
	"MOD_SYRINGE",
	"MOD_AMMO",
	"MOD_ARTY",
	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
	"MOD_CARBINE",
	"MOD_KAR98",
	"MOD_GPG40",
	"MOD_M7",
	"MOD_LANDMINE",
	"MOD_SATCHEL",
	"MOD_SMOKEBOMB",
	"MOD_MOBILE_MG42",
	"MOD_SILENCED_COLT",
	"MOD_GARAND_SCOPE",
	"MOD_CRUSH_CONSTRUCTION",
	"MOD_CRUSH_CONSTRUCTIONDEATH",
	"MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER",
	"MOD_K43",
	"MOD_K43_SCOPE",
	"MOD_MORTAR",
	"MOD_AKIMBO_COLT",
	"MOD_AKIMBO_LUGER",
	"MOD_AKIMBO_SILENCEDCOLT",
	"MOD_AKIMBO_SILENCEDLUGER",
	"MOD_SMOKEGRENADE",
	"MOD_SWAP_PLACES",
	"MOD_SWITCHTEAM",
	"MOD_GOOMBA",
	"MOD_POISON",
	"MOD_FEAR",
	"MOD_CENSORED",
	"MOD_SHOTGUN",
	"MOD_BACKSTAB",
	"MOD_MOBILE_BROWNING",
	"MOD_BAR",
	"MOD_STG44",
	"MOD_BAZOOKA",
	"MOD_STEN_MKII",
	"MOD_MP34",
	"MOD_VENOM",
	"MOD_SHOVE",
	"MOD_THROWKNIFE",
	"MOD_NUM_MODS"
};

int		bg_numMods = sizeof(modNames) / sizeof(modNames[0]);


/*
======================
BG_RW_ParseDefinition
======================
*/
static qboolean BG_RW_ParseDefinition( int handle, weaponStatsInfo_t *weaponInfo, weapontable_t *ammoTable/*, qboolean noparse*/ )		// For now will get needed weapon animations , but l8er it will read all stuff and has a lot of ifdefs
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


		if (!Q_stricmp( token.string, "name" ) ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->name, sizeof(weaponInfo->name) )) {
				return BG_RW_ParseError( handle, "name needs string" );
			}
		}
		else if( !Q_stricmp( token.string, "damage" )  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->damage ) ) {
				return BG_RW_ParseError( handle, "damage needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "splashdamage" )   ) {
			if( !PC_Int_Parse( handle, &weaponInfo->splashdamage ) ) {
				return BG_RW_ParseError( handle, "splashdamage needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "splashdamage_radius" ) ) {
			if( !PC_Int_Parse( handle, &weaponInfo->splashdamage_radius ) ) {
				return BG_RW_ParseError( handle, "splashdamage_radius needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "spread" )  ) {
			if( !PC_Int_Parse( handle, &weaponInfo->spread ) ) {
				return BG_RW_ParseError( handle, "spread needs int" );
			}
		}
		// ammo table filling
		else if( !Q_stricmp( token.string, "coolRate" )   ) {
			if( !PC_Int_Parse( handle, &ammoTable->coolRate ) ) {
				return BG_RW_ParseError( handle, "coolRate needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "defaultStartingAmmo" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->defaultStartingAmmo ) ) {
				return BG_RW_ParseError( handle, "defaultStartingAmmo needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "defaultStartingClip" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->defaultStartingClip ) ) {
				return BG_RW_ParseError( handle, "defaultStartingClip needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "fireDelayTime" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->fireDelayTime ) ) {
				return BG_RW_ParseError( handle, "fireDelayTime needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "maxammo" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxammo ) ) {
				return BG_RW_ParseError( handle, "maxammo needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "maxclip" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxclip ) ) {
				return BG_RW_ParseError( handle, "maxclip needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "maxHeat" )  ) {
			if( !PC_Int_Parse( handle, &ammoTable->maxHeat ) ) {
				return BG_RW_ParseError( handle, "maxHeat needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "mod" )  ) {
			if( !PC_String_ParseNoAlloc( handle, text, sizeof(text) )) {
				return BG_RW_ParseError( handle, "valid MoD name must be entered" );
			}
			else {
				int	i;

				for ( i=0;i<bg_numMods; ++i) {
					if (!Q_stricmp( text, modNames[i] )){
						ammoTable->mod = i;
						break;
					}
				}

			}
		}
		else if( !Q_stricmp( token.string, "nextShotTime" ) ) {
			if( !PC_Int_Parse( handle, &ammoTable->nextShotTime ) ) {
				return BG_RW_ParseError( handle, "nextShotTime needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "reloadTime" ) ) {
			if( !PC_Int_Parse( handle, &ammoTable->reloadTime ) ) { // note kinda useless
				return BG_RW_ParseError( handle, "reloadTime needs int" );
			}
		}
		// end

		else if( !Q_stricmp( token.string, "SpreadScaleAdd" ) ) {
			if( !PC_Int_Parse( handle, &weaponInfo->aimSpreadScaleAdd ) ) {
				return BG_RW_ParseError( handle, "SpreadScaleAdd needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "SpreadScaleAddRand") ) {
			if( !PC_Int_Parse( handle, &weaponInfo->aimSpreadScaleAddRand ) ) {
				return BG_RW_ParseError( handle, "SpreadScaleAddRand needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "recoilDuration" ) ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Int_Parse( handle, &weaponInfo->recoilDuration ) ) {
				return BG_RW_ParseError( handle, "recoilDuration needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "recoilYaw" )  ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Float_Parse( handle, &weaponInfo->recoilYaw )   ) {
				return BG_RW_ParseError( handle, "recoilYaw needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "recoilPitch" )  ) {
			weaponInfo->recoil = qtrue;
			if( !PC_Float_Parse( handle, &weaponInfo->recoilPitch )  ) {
				return BG_RW_ParseError( handle, "recoilPitch needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "CrouchSpreadRatio" ) ) {
			if( !PC_Float_Parse( handle, &weaponInfo->CrouchSpreadRatio )  ) {
				return BG_RW_ParseError( handle, "ProneSpreadRatio needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "ProneSpreadRatio" ) ) {
			if( !PC_Float_Parse( handle, &weaponInfo->ProneSpreadRatio )   ) {
				return BG_RW_ParseError( handle, "ProneSpreadRatio needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "limboKill" ) ) {
			weaponInfo->limboKill = qtrue;
		}
		else if( !Q_stricmp( token.string, "bulletReflection" )  ) {
			weaponInfo->bulletReflection = qtrue;
		}
		else if( !Q_stricmp( token.string, "noMidclipReload" ) ) {
			weaponInfo->noMCReload = qtrue;
		}
		else if (!Q_stricmp( token.string, "selfKillMessage" ) ) {
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->selfKillMessage, sizeof(weaponInfo->selfKillMessage) )) {
				return BG_RW_ParseError( handle, "selfKillMessage needs string" );
			}
		}
		else if (!Q_stricmp( token.string, "KillMessage" ) ) {
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->KillMessage, sizeof(weaponInfo->KillMessage) )) {
				return BG_RW_ParseError( handle, "KillMessage needs string" );
			}
		}
		else if (!Q_stricmp( token.string, "KillMessage2" ) ) {
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->KillMessage2, sizeof(weaponInfo->KillMessage2) )) {
				return BG_RW_ParseError( handle, "KillMessage2 needs string" );
			}
		}
		else if( !Q_stricmp( token.string, "HeadshotWeapon" ) ) {
			weaponInfo->headShot = qtrue;
		}
		else if( !Q_stricmp( token.string, "DistanceFalloff" ) ) {
			weaponInfo->falloff = qtrue;
		}
		else if( !Q_stricmp( token.string, "GibbingWeapon" ) ) {
			weaponInfo->gib = qtrue;
		}
		else if( !Q_stricmp( token.string, "movementSpeedScale" ) ) {
			if( !PC_Float_Parse( handle, &weaponInfo->movementSpeedScale )  ) {
				return BG_RW_ParseError( handle, "movementSpeedScale needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "HeadshotRatio") ){
			if( !PC_Float_Parse( handle, &weaponInfo->HSRatio )  ) {
				return BG_RW_ParseError( handle, "HeadshotRatio needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "minHeadshotDamage" ) ) {
			if( !PC_Int_Parse( handle, &weaponInfo->minHSDamage ) ) {
				return BG_RW_ParseError( handle, "minHeadshotDamage needs int" );
			}
		}
		else if( !Q_stricmp( token.string, "spreadRatio") ){
			if( !PC_Float_Parse( handle, &weaponInfo->wpnScale )  ) {
				return BG_RW_ParseError( handle, "spreadRatio needs float" );
			}
		}
		else if( !Q_stricmp( token.string, "velocity2spread" ) ) {
			weaponInfo->velocity2spread = qtrue;
		}
		else if( !Q_stricmp( token.string, "viewchange2spread" ) ) {
			weaponInfo->viewchange2spread = qtrue;
		}
		else if (!Q_stricmp( token.string, "statname" ) ){
			if( !PC_String_ParseNoAlloc( handle, weaponInfo->statname, sizeof(weaponInfo->statname) )) {
				return BG_RW_ParseError( handle, "statname needs string" );
			}
		}
	}

	return qtrue;
}


/*
=================
BG_RW_ParseEmpty
=================
*/
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


/*
=================
BG_RegisterWeaponFromWeaponFile
=================
*/
static qboolean BG_RegisterWeaponFromWeaponFile( int handle, weaponStatsInfo_t *weaponInfo, weapontable_t *ammoTable, qboolean altWeap, int weaponNum )
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
			if( !BG_RW_ParseClient( handle, weaponInfo, ammoTable, weaponNum ) ) {
			//	BG_RW_ParseError( handle, "Parsing client for weapon %s from %s failed\n", BG_Weapons[weaponNum].name, filename );
			}
		}

		if ( strlen(token.string) == 4 && !Q_stricmpn( token.string, "both", 4 ) ) {
			if ( altWeap ) {
				BG_RW_ParseEmpty( handle );
			}
			else if( !BG_RW_ParseDefinition( handle, weaponInfo, ammoTable ) ) {
			//	BG_RW_ParseError( handle, "Parsing both for weapon %s from %s failed\n", BG_Weapons[weaponNum].name, filename );
			}
			else {
#ifdef DEBUG
			//	Com_Printf( S_COLOR_BLUE "Parsed both for weapon %s from %s\n", BG_Weapons[weaponNum].name, filename );
#endif
			}
		}

		if ( !Q_stricmpn( token.string, "both_altweap", 12 ) ) {
			if ( !altWeap ) {
				BG_RW_ParseEmpty( handle );
			}
			else if( !BG_RW_ParseDefinition( handle, weaponInfo, ammoTable) ) {
			//	BG_RW_ParseError( handle, "Parsing both_altweap for weapon %s from %s failed\n", BG_Weapons[weaponNum].name, filename );
			}
			else {
#ifdef DEBUG
			//	Com_Printf( S_COLOR_BLUE "Parsed both_altweap for weapon %s from %s\n", BG_Weapons[weaponNum].name, filename );
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
=================
*/
void BG_RegisterWeapon( int weaponNum, qboolean force ) {
	weaponStatsInfo_t	*weaponInfo;
	char			*filename;
	qboolean		altWeap=qfalse;
	int				handle=-1;

	if( weaponNum < WP_NONE || weaponNum >= WP_NUM_WEAPONS )
		return;

	weaponInfo = &BG_Weapons[weaponNum];

	if( weaponInfo->registered && !force ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
    BG_InitWeaponInfo(weaponNum);

	switch( weaponNum ) {
		case WP_STG44:					filename = "StG44.weap"; break;
		case WP_STEN_MKII:				filename = "stenMk2.weap"; break;
		case WP_BAR_SET:				altWeap	 = qtrue;
		case WP_BAR:					filename = "bar.weap";  break;
		case WP_BAZOOKA:				filename = "bazooka.weap"; break;
		case WP_KNIFE_KABAR:			filename = "knife_kbar.weap"; break;
		case WP_MOBILE_BROWNING_SET:	altWeap  = qtrue;
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
		case WP_POISON_SYRINGE:			filename = "poison.weap"; break;
		case WP_BINOCULARS:				filename = "binocs.weap"; break;
		case WP_KAR98:					filename = "kar98.weap"; break;
		case WP_GPG40:					filename = "gpg40.weap"; break;
		case WP_CARBINE:				filename = "m1_garand.weap"; break;
		case WP_M7:						filename = "m7.weap"; break;
		case WP_GARAND_SCOPE:			altWeap  = qtrue;
		case WP_GARAND:					filename = "m1_garand_s.weap"; break;
		case WP_FG42SCOPE:				altWeap  = qtrue;
		case WP_FG42:					filename = "fg42.weap"; break;
		case WP_LANDMINE:				filename = "landmine.weap"; break;
		case WP_SATCHEL:				filename = "satchel.weap"; break;
		case WP_SATCHEL_DET:			filename = "satchel_det.weap"; break;
		case WP_SMOKE_BOMB:				filename = "smokegrenade.weap"; break;
		case WP_MOBILE_MG42_SET:		altWeap  = qtrue;
		case WP_MOBILE_MG42:			filename = "mg42.weap"; break;
		case WP_SILENCER:				filename = "silenced_luger.weap"; break;
		case WP_SILENCED_COLT:			filename = "silenced_colt.weap"; break;
		case WP_K43_SCOPE:				altWeap  = qtrue;
		case WP_K43:					filename = "k43.weap"; break;
		case WP_MORTAR:					filename = "mortar.weap"; break;
		case WP_MORTAR_SET:				filename = "mortar_set.weap"; break;
		case WP_MORTAR2:				filename = "axis_mortar.weap"; break;
		case WP_MORTAR2_SET:			filename = "axis_mortar_set.weap"; break;
		case WP_AKIMBO_LUGER:			filename = "akimbo_luger.weap"; break;
		case WP_AKIMBO_SILENCEDLUGER:	filename = "akimbo_silenced_luger.weap"; break;
		case WP_AKIMBO_COLT:			filename = "akimbo_colt.weap"; break;
		case WP_AKIMBO_SILENCEDCOLT:	filename = "akimbo_silenced_colt.weap"; break;
		case WP_VENOM:					filename = "venom.weap"; break;
		case WP_NONE:					filename = "kick.weap"; break;
		default:						 return;
	}

#ifdef CGAMEDLL
	handle = trap_PC_LoadSource( va( "%s/%s", cgs.weaponScriptsDir, filename ) );
#else
	handle = trap_PC_LoadSource( va( "%s/%s", level.weaponScriptsDir, filename ) );
#endif

	if( !handle ) {
		handle = trap_PC_LoadSource( va( "weapons/%s", filename ) );
	}

	BG_RegisterWeaponFromWeaponFile( handle, weaponInfo, &WeaponTable[weaponNum], altWeap, weaponNum );
}
