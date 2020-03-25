/*
 * name:	bg_stats.c
 *
 * desc:
 *
 *
 * NQQS:
 *
 */
#include "q_shared.h"
#include "bg_public.h"

//	--> WP_* to WS_* conversion
//
// WS_MAX = no equivalent/not used
//
// Gordon: why not er, just map directly to the stat?
// JK: er, several reasons: its less than half the size of a brute-force approach,
//							gives us a simple map to what we actually care about, and the
//							fact that we dont want to go above (if at all possible) 32 bits
static const weap_ws_convert_t aWeapID[WP_NUM_WEAPONS] = {
	// WP_						// WS_						// WP_ index
	{ WP_NONE,					WS_MAX				},		// 0
	{ WP_KNIFE,					WS_KNIFE			},
	{ WP_LUGER,					WS_LUGER			},
	{ WP_MP40,					WS_MP40				},
	{ WP_GRENADE_LAUNCHER,		WS_GRENADE			},
	{ WP_PANZERFAUST,			WS_PANZERFAUST		},		// 5
	{ WP_FLAMETHROWER,			WS_FLAMETHROWER		},
	{ WP_COLT,					WS_COLT				},
	{ WP_THOMPSON,				WS_THOMPSON			},
	{ WP_GRENADE_PINEAPPLE,		WS_GRENADE			},
	{ WP_STEN,					WS_STEN				},		// 10
	{ WP_MEDIC_SYRINGE,			WS_MAX				},
	{ WP_AMMO,					WS_MAX				},
	{ WP_ARTY,					WS_ARTILLERY		},
	{ WP_SILENCER,				WS_LUGER			},
	{ WP_DYNAMITE,				WS_DYNAMITE			},		// 15
	{ WP_SMOKETRAIL,			WS_ARTILLERY		},
	{ VERYBIGEXPLOSION,			WS_MAX 				},
	{ WP_MEDKIT,				WS_MAX 				},
	{ WP_BINOCULARS,			WS_MAX 				},
	{ WP_PLIERS,				WS_MAX 				},		// 20
	{ WP_SMOKE_MARKER,			WS_AIRSTRIKE		},
	{ WP_KAR98,					WS_K43				},
	{ WP_CARBINE,				WS_GARAND			},
	{ WP_GARAND,				WS_GARAND			},
	{ WP_LANDMINE,				WS_LANDMINE 		},		// 25
	{ WP_SATCHEL,				WS_MAX				},
	{ WP_SATCHEL_DET,			WS_SATCHEL			},
	{ WP_SMOKE_BOMB,			WS_MAX				},
	{ WP_MOBILE_MG42,			WS_MG42 			},
	{ WP_K43,					WS_K43				},		// 30
	{ WP_FG42,					WS_FG42 			},
	{ WP_DUMMY_MG42,			WS_MG42 			},
	{ WP_MORTAR,				WS_MORTAR			},
	{ WP_AKIMBO_COLT,			WS_COLT				},
	{ WP_AKIMBO_LUGER,			WS_LUGER			},		// 35
	{ WP_GPG40,					WS_GRENADELAUNCHER	},
	{ WP_M7,					WS_GRENADELAUNCHER	},
	{ WP_SILENCED_COLT,			WS_COLT				},
	{ WP_GARAND_SCOPE,			WS_GARAND			},
	{ WP_K43_SCOPE,				WS_K43				},		// 40
	{ WP_FG42SCOPE,				WS_FG42				},
	{ WP_MORTAR_SET,			WS_MORTAR			},
	{ WP_MEDIC_ADRENALINE,		WS_MAX				},
	{ WP_AKIMBO_SILENCEDCOLT,	WS_COLT				},
	{ WP_AKIMBO_SILENCEDLUGER,	WS_LUGER			},		// 45
	{ WP_MOBILE_MG42_SET,		WS_MG42 			},
	{ WP_SHOTGUN,				WS_SHOTGUN  		},
	{ WP_KNIFE_KABAR,			WS_KNIFE			},
	{ WP_MOBILE_BROWNING,		WS_BROWNING 		},
	{ WP_MOBILE_BROWNING_SET,	WS_BROWNING 		},		// 50
	{ WP_BAR,					WS_BAR				},
	{ WP_BAR_SET,				WS_BAR				},
	{ WP_STG44,					WS_STG44			},
	{ WP_STEN_MKII,				WS_MAX			},
	{ WP_BAZOOKA,				WS_BAZOOKA			},		// 55
	{ WP_MP34,					WS_MP34				},
	{ WP_MORTAR2,				WS_MORTAR			},
	{ WP_MORTAR2_SET,			WS_MORTAR			},
	{ WP_VENOM,					WS_VENOM			},
	{ WP_POISON_SYRINGE,		WS_POISON			},		// 60
	{ WP_FOOTKICK,				WS_MAX				},		// 61
};

// Get right stats index based on weapon id
extWeaponStats_t BG_WeapStatForWeapon( weapon_t iWeaponID ) {
	if ( IS_VALID_WEAPON(iWeaponID) ) {
		return aWeapID[iWeaponID].iWS;
	}
	return WS_MAX;
}

// PERS_AWARDS2
int	BG_GetStatBestSpree(playerState_t *ps)
{
	if (ps != NULL) {
		return (( ps->persistant[PERS_AWARDS2] & AWARDMASK_BEST_SPREE ) >> AWARDSHIFT_BEST_SPREE);
	}
	return 0;
}

int BG_GetStatBounty(playerState_t *ps)
{
	if (ps != NULL) {
		return (( ps->persistant[PERS_AWARDS2] & AWARDMASK_BOUNTY ) >> AWARDSHIFT_BOUNTY);
	}
	return 0;
}

int	BG_SetStatBestSpree(playerState_t *ps, int value)
{
	value &= 255;
	ps->persistant[PERS_AWARDS2] &= ~AWARDMASK_BEST_SPREE;
	ps->persistant[PERS_AWARDS2] |= (AWARDMASK_BEST_SPREE & value);
	return 0;
}

int BG_SetStatBounty(playerState_t *ps, int value)
{
	value &= 255;
	ps->persistant[PERS_AWARDS2] &= ~AWARDMASK_BOUNTY;
	ps->persistant[PERS_AWARDS2] |= (value << AWARDSHIFT_BOUNTY);
	return 0;
}

// PERS_KILLSPREE
int	BG_GetKillSpree(playerState_t *ps)
{
	if ( ps != NULL ) {
		return	ps->persistant[PERS_KILLSPREE];
	}
	return 0;
}

int	BG_SetKillSpree(playerState_t *ps, int value)
{
	ps->persistant[PERS_KILLSPREE] = value;
	return 0;
}

// Adjust a statmask
int BG_AdjustBounty(playerState_t *ps, int value)
{
	BG_SetStatBounty(ps, (BG_GetStatBounty(ps) + value) );
	return 0;
}

int	BG_AdjustBestSpree(playerState_t *ps, int value)
{
	BG_SetStatBestSpree(ps, (BG_GetStatBestSpree(ps) + value) );
	return 0;
}
