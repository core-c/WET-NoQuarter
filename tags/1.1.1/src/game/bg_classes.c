#include "q_shared.h"
#include "bg_public.h"
#include "bg_classes.h"

bg_playerclass_t bg_allies_playerclasses[NUM_PLAYER_CLASSES] = 
{
	{	
		PC_SOLDIER,
		"characters/temperate/allied/soldier.char",
		"ui/assets/mp_gun_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_BAR,					// jP - modified - allied soldiers get BAR instead of Thompson  
			WP_MOBILE_BROWNING,		// jP - modified - allied soldiers get Browning instead of MG42	
			WP_FLAMETHROWER,
			WP_BAZOOKA,				// jP - modified - allied soldiers get Bazooka instead of PANZERFAUST
			WP_MORTAR,
			WP_VENOM
		},
	},

	{
		PC_MEDIC,
		"characters/temperate/allied/medic.char",
		"ui/assets/mp_health_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_THOMPSON,
		},	
	},

	{
		PC_ENGINEER,
		"characters/temperate/allied/engineer.char",
		"ui/assets/mp_wrench_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_THOMPSON,
			WP_CARBINE,
			WP_SHOTGUN,				// jP - added 'shotgun'
		},	
	},

	{
		PC_FIELDOPS,
		"characters/temperate/allied/fieldops.char",
		"ui/assets/mp_ammo_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_THOMPSON,
			WP_SHOTGUN,				// jP - added 'shotgun'
		},	
	},

	{
		PC_COVERTOPS,
		"characters/temperate/allied/cvops.char",
		"ui/assets/mp_spy_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_STEN,
		//	WP_STEN_MKII,
			WP_BAR,					// jP - modified - allied coverts get BAR instead of FG42
			WP_GARAND,
		},	
	},
};

bg_playerclass_t bg_axis_playerclasses[NUM_PLAYER_CLASSES] =
{
	{
		PC_SOLDIER,
		"characters/temperate/axis/soldier.char",
		"ui/assets/mp_gun_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_STG44,				// jP - modified - axis soldiers get StG44 instead of mp40
			WP_MOBILE_MG42,
			WP_FLAMETHROWER,
			WP_PANZERFAUST,
			WP_MORTAR2,
			WP_VENOM
		},	
	},

	{
		PC_MEDIC,
		"characters/temperate/axis/medic.char",
		"ui/assets/mp_health_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
		},	
	},

	{
		PC_ENGINEER,
		"characters/temperate/axis/engineer.char",
		"ui/assets/mp_wrench_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
			WP_KAR98,
			WP_SHOTGUN,				// jP - added 'shotgun'
		},	
	},

	{
		PC_FIELDOPS,
		"characters/temperate/axis/fieldops.char",
		"ui/assets/mp_ammo_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
			WP_SHOTGUN,				// jP - added 'shotgun'
		},	
	},

	{
		PC_COVERTOPS,
		"characters/temperate/axis/cvops.char",
		"ui/assets/mp_spy_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP34,				// jP - modified - axis coverts get MP34
			WP_FG42,
			WP_K43,
		},	
	},
};

bg_playerclass_t* BG_GetPlayerClassInfo( int team, int cls )
{
	bg_playerclass_t* teamList;

	if( cls < PC_SOLDIER || cls >= NUM_PLAYER_CLASSES ) 
	{
		cls = PC_SOLDIER;
	}

	switch( team )
	{
		default:
		case TEAM_AXIS:
			teamList = bg_axis_playerclasses;
			break;
		case TEAM_ALLIES:
			teamList = bg_allies_playerclasses;
			break;
	}

	return &teamList[cls];
}

bg_playerclass_t* BG_PlayerClassForPlayerState(playerState_t* ps)
{
	return BG_GetPlayerClassInfo(ps->persistant[PERS_TEAM], ps->stats[STAT_PLAYER_CLASS]);
}

qboolean BG_ClassHasWeapon(bg_playerclass_t* classInfo, weapon_t weap)
{
	int i;

	if(!weap) 
	{
		return qfalse;
	}

	for( i = 0; i < MAX_WEAPS_PER_CLASS; i++) 
	{
		if(classInfo->classWeapons[i] == weap) 
		{
			return qtrue;
		}
	}

	return qfalse;
}

qboolean BG_WeaponIsPrimaryForClassAndTeam( int classnum, team_t team, weapon_t weapon )
{
	bg_playerclass_t *classInfo;

	if( team == TEAM_ALLIES ) 
	{
		classInfo = &bg_allies_playerclasses[classnum];

		if( BG_ClassHasWeapon( classInfo, weapon ) ) 
		{
			return qtrue;
		}
	} 
	else if( team == TEAM_AXIS ) 
	{
		classInfo = &bg_axis_playerclasses[classnum];

		if( BG_ClassHasWeapon( classInfo, weapon ) )
		{
			return qtrue;
		}
	}

	return qfalse;
}

const char* BG_ShortClassnameForNumber( int classNum ) {
	switch( classNum ) {
		case PC_SOLDIER:
			return "^wSoldr";
		case PC_MEDIC:
			return "^wMedic";
		case PC_ENGINEER:
			return "^wEngr";
		case PC_FIELDOPS:
			return "^wFdOps";
		case PC_COVERTOPS:
			return "^wCvOps";
		default:
			return "^1ERROR";
	}
}

const char* BG_ClassnameForNumber( int classNum )
{
	switch( classNum )
	{
		case PC_SOLDIER:
			return "Soldier";
		case PC_MEDIC:
			return "Medic";
		case PC_ENGINEER:
			return "Engineer";
		case PC_FIELDOPS:
			return "Field Ops";
		case PC_COVERTOPS:
			return "Covert Ops";
		default:
			return "^1ERROR";
	}
}

const char* BG_ClassnameForNumber_Filename( int classNum )
{
	switch( classNum )
	{
		case PC_SOLDIER:
			return "Soldier";
		case PC_MEDIC:
			return "Medic";
		case PC_ENGINEER:
			return "Engineer";
		case PC_FIELDOPS:
			return "Fieldops";
		case PC_COVERTOPS:
			return "Covertops";
		default:
			return "ERROR";
	}
}

const char* BG_ClassLetterForNumber( int classNum ) 
{
	switch( classNum ) 
	{
		case PC_SOLDIER:
			return "S";
		case PC_MEDIC:
			return "M";
		case PC_ENGINEER:
			return "E";
		case PC_FIELDOPS:
			return "F";
		case PC_COVERTOPS:
			return "C";
		default:
			return "^1E";
	}
}

int BG_ClassTextToClass(char *token)
{
	if (!Q_stricmp(token, "soldier") || !Q_stricmp(token, "soldat"))
	{
		return PC_SOLDIER;
	} 
	else if (!Q_stricmp(token, "medic"))
	{
		return PC_MEDIC;
	}
	else if (!Q_stricmp(token, "fieldops") || !Q_stricmp(token, "fieldop")) 
	{
		return PC_FIELDOPS;
	}
	else if (!Q_stricmp(token, "engineer")) 
	{
		return PC_ENGINEER;
	} 
	else if (!Q_stricmp(token, "covertops") || !Q_stricmp(token, "covert"))
	{
		return PC_COVERTOPS;
	}

	return -1;
}

skillType_t BG_ClassSkillForClass( int classNum )
{
	skillType_t classSkill[NUM_PLAYER_CLASSES] = 
	{ 
		SK_HEAVY_WEAPONS, 
		SK_FIRST_AID,
		SK_EXPLOSIVES_AND_CONSTRUCTION, 
		SK_SIGNALS, 
		SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS 
	};

	if( classNum < 0 || classNum >= NUM_PLAYER_CLASSES )
	{
		return SK_BATTLE_SENSE;
	}

	return classSkill[ classNum ];
}

