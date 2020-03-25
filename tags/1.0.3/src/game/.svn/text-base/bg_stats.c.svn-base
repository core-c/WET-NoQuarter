#include "q_shared.h"
#include "bg_public.h"

//	--> WP_* to WS_* conversion
//
// WS_MAX = no equivalent/not used
//
// FIXME: Remove everything that maps to WS_MAX to save space
//
// Gordon: why not er, just map directly to the stat?
// JK: er, several reasons: its less than half the size of a brute-force approach,
//							gives us a simple map to what we actually care about, and the
//							fact that we dont want to go above (if at all possible) 32 bits
static const weap_ws_convert_t aWeapID[WP_NUM_WEAPONS] = {

	{ WP_NONE,					WS_MAX				},		// 0

	// German weapons
	{ WP_KNIFE,					WS_KNIFE			},
	{ WP_LUGER,					WS_LUGER			},
	{ WP_MP40,					WS_MP40				},
	{ WP_GRENADE_LAUNCHER,		WS_GRENADE			},		// 5
	{ WP_PANZERFAUST,			WS_PANZERFAUST		},
	{ WP_FLAMETHROWER,			WS_FLAMETHROWER		},

	// American equivalents
	{ WP_COLT,					WS_COLT				},		// 10
	{ WP_THOMPSON,				WS_THOMPSON			},
	{ WP_GRENADE_PINEAPPLE,		WS_GRENADE			},
	{ WP_STEN,					WS_STEN				},
	{ WP_MEDIC_SYRINGE,			WS_SYRINGE			},
	{ WP_AMMO,					WS_MAX				},
	{ WP_ARTY,					WS_ARTILLERY		},


	{ WP_SILENCER,				WS_LUGER			},		// 20
	{ WP_DYNAMITE,				WS_DYNAMITE			},
	{ WP_SMOKETRAIL,			WS_ARTILLERY		},
//	{ WP_MAPMORTAR,				WS_MORTAR			},
	{ VERYBIGEXPLOSION,			WS_MAX 				},
	{ WP_MEDKIT,				WS_MAX 				},
	{ WP_BINOCULARS,			WS_MAX 				},

	{ WP_PLIERS,				WS_MAX 				},		// 30
	{ WP_SMOKE_MARKER,			WS_AIRSTRIKE		},

	{ WP_KAR98,					WS_K43				},
	{ WP_CARBINE,				WS_GARAND			},
	{ WP_GARAND,				WS_GARAND			},
	{ WP_LANDMINE,				WS_LANDMINE 		},		// 35
	{ WP_SATCHEL,				WS_MAX				},
	{ WP_SATCHEL_DET,			WS_SATCHEL			},
	{ WP_TRIPMINE,				WS_LANDMINE 		},
	{ WP_SMOKE_BOMB,			WS_SMOKE			},

	{ WP_MOBILE_MG42,			WS_MG42 			},		// 40
	{ WP_MOBILE_MG42_SET,		WS_MG42 			},
	{ WP_K43,					WS_K43				},
	{ WP_FG42,					WS_FG42 			},
	{ WP_DUMMY_MG42,			WS_MG42 			},	    // ??
//	{ WP_LOCKPICK,				WS_MAX				},
	
	{ WP_GPG40,					WS_GRENADELAUNCHER	},
	{ WP_M7,					WS_GRENADELAUNCHER	},
	{ WP_SILENCED_COLT,			WS_COLT				},		// 50
	{ WP_GARAND_SCOPE,			WS_GARAND			},
	{ WP_K43_SCOPE,				WS_K43				},
	{ WP_FG42_SCOPE,			WS_FG42				},
	{ WP_MORTAR_SET,			WS_MORTAR			},
	{ WP_MORTAR,				WS_MORTAR			},
	{ WP_MEDIC_ADRENALINE,		WS_MAX				},		// 55

	{ WP_AKIMBO_SILENCEDCOLT,	WS_COLT				},
	{ WP_AKIMBO_SILENCEDLUGER,	WS_LUGER			},
	{ WP_AKIMBO_COLT,			WS_COLT				},
	{ WP_AKIMBO_LUGER,			WS_LUGER			},
	{ WP_SHOTGUN,				WS_SHOTGUN  		},		// 60
	{ WP_KNIFE_KABAR,			WS_KNIFE			},
	{ WP_MOBILE_BROWNING,		WS_BROWNING 		},
	{ WP_MOBILE_BROWNING_SET,	WS_BROWNING 		},
	{ WP_STG44,					WS_STG44			},
	{ WP_BAR,					WS_BAR				},		// 65
	{ WP_BAR_SET,				WS_BAR				},
	{ WP_STEN_MKII,				WS_STEN_MKII		},		// WS_STEN_MKII
	{ WP_BAZOOKA,				WS_BAZOOKA			},		// 58 Bazooka
	{ WP_MP34,					WS_MP34				},		// 59 MP34
	{ WP_FIREBOLT,				WS_FIREBOLT			},	
	{ WP_MORTAR2_SET,			WS_MORTAR			},
	{ WP_MORTAR2,				WS_MORTAR			},

	{ WP_VENOM,					WS_VENOM			},


};


// Get right stats index based on weapon id
extWeaponStats_t BG_WeapStatForWeapon( weapon_t iWeaponID ) {
	weapon_t i;

	for( i = WP_NONE; i < WP_NUM_WEAPONS; i++) {
		if( iWeaponID == aWeapID[i].iWeapon ) {
			return aWeapID[i].iWS;
		}
	}

	return WS_MAX;
}



// jet Pilot - Bitmask operators for stats overloading


int BG_GetAuraLevel(int cls, playerState_t *ps)
{
	switch (cls)
	{
		case PC_SOLDIER:
			return ((ps->stats[STAT_TEAMAURA] & AURAMASK_SOLDIER)	>> AURASHIFT_SOLDIER);
		case PC_MEDIC:
			return ((ps->stats[STAT_TEAMAURA] & AURAMASK_MEDIC)		>> AURASHIFT_MEDIC);
		case PC_ENGINEER:
			return ((ps->stats[STAT_TEAMAURA] & AURAMASK_ENGINEER)	>> AURASHIFT_ENGINEER);
		case PC_FIELDOPS:
			return ((ps->stats[STAT_TEAMAURA] & AURAMASK_FIELDOPS)	>> AURASHIFT_FIELDOPS);
		case PC_COVERTOPS:
			return ((ps->stats[STAT_TEAMAURA] & AURAMASK_COVERTOPS) >> AURASHIFT_COVERTOPS);
		default:
			return 0;
	}
}

int BG_GetAuraLevels(int auras[], playerState_t *ps)
{
	auras[PC_SOLDIER]	= ((ps->stats[STAT_TEAMAURA] & AURAMASK_SOLDIER)	>> AURASHIFT_SOLDIER);
	auras[PC_MEDIC]		= ((ps->stats[STAT_TEAMAURA] & AURAMASK_MEDIC)		>> AURASHIFT_MEDIC);
	auras[PC_ENGINEER]	= ((ps->stats[STAT_TEAMAURA] & AURAMASK_MEDIC)     	>> AURASHIFT_MEDIC);
	auras[PC_FIELDOPS]	= ((ps->stats[STAT_TEAMAURA] & AURAMASK_FIELDOPS)  	>> AURASHIFT_FIELDOPS);
	auras[PC_COVERTOPS] = ((ps->stats[STAT_TEAMAURA] & AURAMASK_COVERTOPS)	>> AURASHIFT_COVERTOPS);

	return sizeof(auras);
}

int	BG_SetAuraLevel(playerState_t *ps, int cls, int value)
{
	switch (cls)
	{
		case PC_SOLDIER:
			ps->persistant[STAT_TEAMAURA] &= ~AURAMASK_SOLDIER;
			ps->persistant[STAT_TEAMAURA] |= (AURAMASK_SOLDIER & value);
			return 0;
		case PC_MEDIC:
			ps->persistant[STAT_TEAMAURA] &= ~AURAMASK_MEDIC;
			ps->persistant[STAT_TEAMAURA] |= (AURAMASK_MEDIC & value);
			return 0;
		case PC_ENGINEER:
			ps->persistant[STAT_TEAMAURA] &= ~AURAMASK_ENGINEER;
			ps->persistant[STAT_TEAMAURA] |= (AURAMASK_ENGINEER & value);
			return 0;
		case PC_FIELDOPS:
			ps->persistant[STAT_TEAMAURA] &= ~AURAMASK_FIELDOPS;
			ps->persistant[STAT_TEAMAURA] |= (AURAMASK_FIELDOPS & value);
			return 0;
		case PC_COVERTOPS:
			ps->persistant[STAT_TEAMAURA] &= ~AURAMASK_COVERTOPS;
			ps->persistant[STAT_TEAMAURA] |= (AURAMASK_COVERTOPS & value);
			return 0;
		default:
			return -1;
	}
}

// PERS_AWARDS

int	BG_GetStatGoombas(playerState_t *ps)
{
	if (ps != NULL)
		return ( ps->persistant[PERS_AWARDS] & AWARDMASK_GOOMBA );
	return 0;
}

int	BG_GetStatDarwin(playerState_t *ps)
{
	if (ps != NULL)
		return (( ps->persistant[PERS_AWARDS] & AWARDMASK_DARWIN ) >> AWARDSHIFT_DARWIN);
	return 0;
}

int	BG_GetStatBestSpree(playerState_t *ps)
{
	if (ps != NULL)
		return (( ps->persistant[PERS_AWARDS] & AWARDMASK_BEST_SPREE ) >> AWARDSHIFT_BEST_SPREE);
	return 0;
}

int BG_GetStatBounty(playerState_t *ps)
{
	if (ps != NULL)
		return (( ps->persistant[PERS_AWARDS] & AWARDMASK_BOUNTY ) >> AWARDSHIFT_BOUNTY);
	return 0;
}



int	BG_SetStatGoombas(playerState_t *ps, int value)
{
	ps->persistant[PERS_AWARDS] &= ~AWARDMASK_GOOMBA;
	ps->persistant[PERS_AWARDS] |= (AWARDMASK_GOOMBA & value);
	return 0;
}

int	BG_SetStatBestSpree(playerState_t *ps, int value)
{
	ps->persistant[PERS_AWARDS] &= ~AWARDMASK_BEST_SPREE;
	ps->persistant[PERS_AWARDS] |= (value << AWARDSHIFT_BEST_SPREE);
	return 0;
}

int	BG_SetStatDarwin(playerState_t *ps, int value)
{
	ps->persistant[PERS_AWARDS] &= ~AWARDMASK_DARWIN;
	ps->persistant[PERS_AWARDS] |= (value << AWARDSHIFT_DARWIN);
	return 0;
}

int BG_SetStatBounty(playerState_t *ps, int value)
{
	ps->persistant[PERS_AWARDS] &= ~AWARDMASK_BOUNTY;
	ps->persistant[PERS_AWARDS] |= (value << AWARDSHIFT_BOUNTY);
	return 0;
}

// PERS_STATS



int BG_GetLastEnemyWeapon(playerState_t *ps)
{
	if (!ps) return 0;
	
	return (( ps->persistant[PERS_STATS] & STATSMASK_ENEMY_WEAPON ) >> STATSHIFT_ENEMY_WEAPON );	
}

int	BG_GetImpactPoint(playerState_t *ps)
{	
	if (!ps) return 0;

	return (( ps->persistant[PERS_STATS] & STATSMASK_IMPACT_POINT ) >> STATSHIFT_IMPACT_POINT );	
}

int	BG_GetPainType(playerState_t *ps)
{
	if (!ps) return 0;

	return (( ps->persistant[PERS_STATS] & STATSMASK_PAIN_TYPE ) >> STATSHIFT_PAIN_TYPE);	
}

int	BG_GetKillSpree(playerState_t *ps)
{
	if (!ps) return 0;
		
	return ps->persistant[PERS_KILLSPREE];
	//	return (( ps->persistant[PERS_STATS] & STATSMASK_KILLSPREE ) >> STATSHIFT_KILLSPREE);
}

int	BG_GetDamageDirection(playerState_t *ps)
{
	if (!ps) return 0;
		
	return (( ps->persistant[PERS_STATS] & STATSMASK_DIRECTION ) >> STATSHIFT_DIRECTION);	
}


int BG_SetLastEnemyWeapon(playerState_t *ps, int value)
{
	ps->persistant[PERS_STATS] &= ~STATSMASK_ENEMY_WEAPON;
	ps->persistant[PERS_STATS] |= (value << STATSHIFT_ENEMY_WEAPON);
	return 0;
}

int BG_SetImpactPoint(playerState_t *ps, int value)
{
	if (ps->stats[STAT_HEALTH] <= 0) value = 0;
	ps->persistant[PERS_STATS] &= ~STATSMASK_IMPACT_POINT;
	ps->persistant[PERS_STATS] |= (value << STATSHIFT_IMPACT_POINT);
	return 0;
}

int BG_SetDamageDirection(playerState_t *ps, int value)
{
	ps->persistant[PERS_STATS] &= ~STATSMASK_DIRECTION;
	ps->persistant[PERS_STATS] |= (value << STATSHIFT_DIRECTION);
	return 0;
}

int	BG_SetPainType(playerState_t *ps, int value)
{
	ps->persistant[PERS_STATS] &= ~STATSMASK_PAIN_TYPE;
	ps->persistant[PERS_STATS] |= (value << STATSHIFT_PAIN_TYPE);
	return 0;
}

int	BG_SetKillSpree(playerState_t *ps, int value)
{
//	ps->persistant[PERS_STATS] &= ~STATSMASK_KILLSPREE;
//	ps->persistant[PERS_STATS] |= (value << STATSHIFT_KILLSPREE);
	ps->persistant[PERS_KILLSPREE] = value;
	return 0;
}

// Adjust a statmask
int	BG_AdjustGoombas(playerState_t *ps, int value)
{
	int tmp;
	tmp = BG_GetStatGoombas(ps);
	BG_SetStatGoombas(ps, tmp + value);	
	return 0;
}

int	BG_AdjustKillSpree(playerState_t *ps, int value)
{
	//int tmp;
	//tmp = BG_GetKillSpree(ps);
	//BG_SetKillSpree(ps, tmp + value);
	ps->persistant[PERS_KILLSPREE] += value;
	return 0;
}

int BG_AdjustBounty(playerState_t *ps, int value)
{
	unsigned int tmp;
	tmp = BG_GetStatBounty(ps);
	BG_SetStatBounty(ps, tmp + value);
	return 0;
}

int	BG_AdjustBestSpree(playerState_t *ps, int value)
{
	int tmp;
	tmp = BG_GetStatBestSpree(ps);
	BG_SetStatBestSpree(ps, tmp + value);
	return 0;
}

int	BG_AdjustDarwin(playerState_t *ps, int value)
{
	int tmp;
	tmp = BG_GetStatDarwin(ps);
	BG_SetStatDarwin(ps, tmp + value);
	return 0;
}
