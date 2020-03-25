/*
============================
======  jp_weapons.c  ======
============================

Alternate implementation of weapons code.
*/

#include "g_local.h"
#include "jp_weapons.h"

/*
ammotable_t ammoTableMP[WP_NUM_WEAPONS] = {
	//	MAX		USES	MAX				START	START			RELOAD	FIRE			NEXT	HEAT,	COOL,	MOD,	...
	//	AMMO	AMT.	CLIP			AMMO	CLIP			TIME	DELAY			SHOT
	{	0,		0,		0,				0,		0,				0,		50,				0,		0,		0,		0						},	// WP_NONE					// 0
	{	999,	0,		999,			0,		0,				0,		50,				200,	0,		0,		MOD_KNIFE				},	// WP_KNIFE					// 1
	{	24,		1,		8,				24,		8,				1500,	DELAY_PISTOL,	400,	0,		0,		MOD_LUGER				},	// WP_LUGER					// 2	// NOTE: also 32 round 'snail' magazine
	{	90,		1,		30,				30,		30,				2400,	DELAY_LOW,		150,	0,		0,		MOD_MP40				},	// WP_MP40					// 3
	{	45,		1,		15,				0,		4,				1000,	DELAY_THROW,	1600,	0,		0,		MOD_GRENADE_LAUNCHER	},	// WP_GRENADE_LAUNCHER		// 4
	{	4,		1,		1,				0,		4,				1000,	750	,			2000,	0,		0,		MOD_PANZERFAUST			},	// WP_PANZERFAUST			// 5	// DHM - Nerve :: updated delay so prediction is correct
	{	200,	1,		200,			0,		200,			1000,	DELAY_LOW,		50,		0,		0,		MOD_FLAMETHROWER		},	// WP_FLAMETHROWER			// 6
	{	24,		1,		8,				24,		8,				1500,	DELAY_PISTOL,	400,	0,		0,		MOD_COLT				},	// WP_COLT					// 7
	{	90,		1,		30,				30,		30,				2400,	DELAY_LOW,		150,	0,		0,		MOD_THOMPSON			},	// WP_THOMPSON				// 8
	{	45,		1,		15,				0,		4,				1000,	DELAY_THROW,	1600,	0,		0,		MOD_GRENADE_PINEAPPLE	},	// WP_GRENADE_PINEAPPLE		// 9

	{	96,		1,		32,				64,		32,				3100,	DELAY_LOW,		150,	1200,	450,	MOD_STEN				},	// WP_STEN					// 10
	{	10,		1,		1,				0,		10,				1500,	50,				1000,	0,		0,		MOD_SYRINGE				},	// WP_MEDIC_SYRINGE			// 11
	{	1,		0,		1,				0,		0,				3000,	50,				1000,	0,		0,		MOD_AMMO,				},	// WP_AMMO					// 12
	{	1,		0,		1,				0,		1,				3000,	50,				1000,	0,		0,		MOD_ARTY,				},	// WP_ARTY					// 13												
	{	24,		1,		8,				24,		8,				1500,	DELAY_PISTOL,	400,	0,		0,		MOD_SILENCER			},	// WP_SILENCER				// 14
	{	1,		0,		10,				0,		0,				1000,	DELAY_THROW,	1600,	0,		0,		MOD_DYNAMITE			},	// WP_DYNAMITE				// 15
	{	999,	0,		999,			0,		0,				0,		50,				0,		0,		0,		0						},	// WP_SMOKETRAIL			// 16
	{	999,	0,		999,			0,		0,				0,		50,				0,		0,		0,		0						},	// WP_MAPMORTAR				// 17
	{	999,	0,		999,			0,		0,				0,		50,				0,		0,		0,		0						},	// VERYBIGEXPLOSION			// 18
	{	999,	0,		999,			1,		1,				0,		50,				0,		0,		0,		0						},	// WP_MEDKIT				// 19

	{	999,	0,		999,			0,		0,				0,		50,				0,		0,		0,		0						},	// WP_BINOCULARS			// 20
	{	999,	0,		999,			0,		0,				0,		50,				0,		0,		0,		0						},	// WP_PLIERS				// 21
	{	999,	0,		999,			0,		1,				0,		50,				0,		0,		0,		MOD_AIRSTRIKE			},	// WP_SMOKE_MARKER			// 22
	{	30,		1,		10,				20,		10,				2500,	DELAY_LOW,		400,	0,		0,		MOD_KAR98				},	// WP_KAR98					// 23		K43
	{	30,		1,		10,				20,		10,				1500,	DELAY_LOW,		400,	0,		0,		MOD_CARBINE				},	// WP_CARBINE				// 24		GARAND (Engineer)
	{	30,		1,		10,				20,		10,				1500,	DELAY_LOW,		400,	0,		0,		MOD_GARAND				},	// WP_GARAND				// 25		GARAND (Covert)
	{	1,		0,		1,				0,		1,				100,	DELAY_LOW,		100,	0,		0,		MOD_LANDMINE			},	// WP_LANDMINE				// 26
	{	1,		0,		1,				0,		0,				3000,	DELAY_LOW,		2000,	0,		0,		MOD_SATCHEL				},	// WP_SATCHEL				// 27
	{	1,		0,		1,				0,		0,				3000,	722,			2000,	0,		0,		0,						},	// WP_SATCHEL_DET			// 28
	{	6,		1,		1,				0,		0,				2000,	DELAY_HIGH,		2000,	0,		0,		MOD_TRIPMINE			},	// WP_TRIPMINE				// 29

	{	1,		0,		10,				0,		1,				1000,	DELAY_THROW,	1600,	0,		0,		MOD_SMOKEBOMB			},	// WP_SMOKE_BOMB			// 30
	{	450,	1,		150,			0,		150,			3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_MG42			},	// WP_MOBILE_MG42			// 31
	{	30,		1,		10,				20,		10,				2500,	DELAY_LOW,		400,	0,		0,		MOD_K43					},	// WP_K43					// 32		K43
	{	60,		1,		20,				40,		20,				2000,	DELAY_LOW,		100,	0,		0,		MOD_FG42				},	// WP_FG42					// 33
	{	0,		0,		0,				0,		0,				0,	    0,		        0,	    1500,	300,	0					    },	// WP_DUMMY_MG42			// 34
	{	15,		1,		1,				0,		0,				0,		750,			1600,	0,		0,		MOD_MORTAR				},	// WP_MORTAR				// 35
	{	999,	0,		1,				0,		0,				1000,	750,			1600,	0,		0,		0						},	// WP_LOCKPICK				// 36 
	{	48,		1,		8,				48,		8,				2700,	DELAY_PISTOL,	200,	0,		0,		MOD_AKIMBO_COLT			},	// WP_AKIMBO_COLT			// 37
	{	48,		1,		8,				48,		8,				2700,	DELAY_PISTOL,	200,	0,		0,		MOD_AKIMBO_LUGER		},	// WP_AKIMBO_LUGER			// 38
	{	4,		1,		1,				4,		1,				3000,	DELAY_LOW,		400,	0,		0,		MOD_GPG40				},	// WP_GPG40					// 39

	{	4,		1,		1,				4,		1,				3000,	DELAY_LOW,		400,	0,		0,		MOD_M7					},	// WP_M7					// 40
	{	24,		1,		8,				24,		8,				1500,	DELAY_PISTOL,	400,	0,		0,		MOD_SILENCED_COLT		},	// WP_SILENCED_COLT			// 41
	{	30,		1,		10,				20,		10,				1500,	0,				400,	0,		0,		MOD_GARAND_SCOPE		},	// WP_GARAND_SCOPE			// 42		GARAND
	{	30,		1,		10,				20,		10,				2500,	0,				400,	0,		0,		MOD_K43_SCOPE			},	// WP_K43_SCOPE				// 43		K43
	{	60,		1,		20,				40,		20,				2000,	DELAY_LOW,		400,	0,		0,		MOD_FG42SCOPE			},	// WP_FG42_SCOPE				// 44
	{	16,		1,		1,				12,		0,				0,		750,			1400,	0,		0,		MOD_MORTAR				},	// WP_MORTAR_SET			// 45
	{	10,		1,		1,				0,		10,				1500,	50,				1000,	0,		0,		MOD_SYRINGE				},	// WP_MEDIC_ADRENALINE		// 46
	{	48,		1,		8,				48,		8,				2700,	DELAY_PISTOL,	200,	0,		0,		MOD_AKIMBO_SILENCEDCOLT	},	// WP_AKIMBO_SILENCEDCOLT	// 47
	{	48,		1,		8,				48,		8,				2700,	DELAY_PISTOL,	200,	0,		0,		MOD_AKIMBO_SILENCEDLUGER},	// WP_AKIMBO_SILENCEDLUGER	// 48
	{	450,	1,		150,			0,		150,			3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_MG42			},	// WP_MOBILE_MG42_SET		// 49

	{	24,		1,		6,				24,		6,				1500,	DELAY_LOW,		800,	0,		0,		MOD_SHOTGUN				},	// WP_SHOTGUN				// 50		Winchester Model 97
	{	999,	0,		999,			0,		0,				0,		50,				200,	0,		0,		MOD_KNIFE				},	// WP_KNIFE_KABAR			// 51		Allied Knife
	{	450,	1,		150,			0,		150,			3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_BROWNING		},	// WP_MOBILE_BROWNING		// 52		Browning .30 cal
	{	450,	1,		150,			0,		150,			3000,	DELAY_LOW,		66,		1500,	300,	MOD_MOBILE_BROWNING		},	// WP_MOBILE_BROWNING_SET	// 53		Browning .30 cal set
};
*/

jp_weaponStat_t jp_weapon[WP_NUM_WEAPONS] = 
{
	{
		"<Unknown Weapon>",
		"<Unknown Weapon>",
		"Nothing really to say about this",
		0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 
		qfalse,
		0.0f, 0.0f, 
		-1
	},
	{
		"Knife",
		"Ka-Bar Combat Knife",
		"02-1217-K1 Fighting/Utility Knife - ",
		WP_KNIFE_KABAR,

		999,
		0,
		999,
		0,
		0,
		0,
		50,
		200,
		0,
		0,
		MOD_KNIFE,							// means of death
											
		JP_WPFLAG_SILENCED,					
		0,									// alternate fire mode
		qfalse,								// uses charge
		3.0f,								
		0.0f,								// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{
		"Knife",
		"SA/SS Dagger",
		"German standard issue combat dagger",
		WP_KNIFE,

		999,
		0,
		999,
		0,
		0,
		0,
		50,
		200,
		0,
		0,
		MOD_KNIFE,							// means of death
											
		JP_WPFLAG_SILENCED,					
		0,									// alternate fire mode
		qfalse,								// uses charge
		3.0f,								
		0.0f,								// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{	
		"Luger",
		"Luger 9mm",
		"German Standard issue sidearm",
		WP_LUGER,
        
		24,		
		1,
		8,
		24,
		8,
		1500,
		100,
		400,
		0,
		0,
		MOD_LUGER,				// means of death

		0,
		WP_SILENCER,			// alternate fire mode
		qfalse,
		3.0f,
		2.0f,					// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{	
		"Silenced Luger",
		"Silenced Luger 9mm",
		"German standard issue sidearm, fitted with a silencer",
		WP_SILENCER,

		24,		
		1,
		8,
		24,
		8,
		1500,
		100,
		400,
		0,
		0,
		MOD_SILENCER,				// means of death

		JP_WPFLAG_SILENCED,
		WP_LUGER,					// alternate fire mode
		qfalse,						// uses charge
		3.0f,
		2.0f,						// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{	
		"Akimbo Lugers",
		"Dual-Weild Luger 9mms",
		"Two German standard issue 9mm sidearms",
		WP_AKIMBO_LUGER,

		48,		
		1,
		8,
		48,
		8,
		2700,
		100,
		400,
		0,
		0,
		MOD_AKIMBO_LUGER,				// means of death

		0,
		0,								// alternate fire mode
		qfalse,							// uses charge
		3.0f,
		2.0f,							// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{	
		"Silenced Akimbo Lugers",
		"Dual Silenced Luger 9mms",
		"Two silenced German Luger 9mms",
		WP_AKIMBO_SILENCEDLUGER,

		48,		
		1,
		8,
		48,
		8,
		2700,
		100,
		400,
		0,
		0,
		MOD_AKIMBO_SILENCEDLUGER,		// means of death

		JP_WPFLAG_SILENCED,
		0,								// alternate fire mode
		qfalse,							// uses charge
		3.0f,
		2.0f,							// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
	{	
		"Colt",
		"Colt .45 APC",
		"U.S. Standard issue sidearm",
		WP_COLT,

		24,		
		1,
		8,
		24,
		8,
		1500,
		100,
		400,
		0,
		0,
		MOD_COLT,				// means of death

		0,
		WP_SILENCED_COLT,		// alternate fire mode
		qfalse,					// uses charge
		3.0f,
		2.0f,					// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},	
	{	
		"Silenced Colt",
		"Silenced Colt .45 APC",
		"U.S. Standard issue sidearm, fitted with a silencer",
		WP_SILENCED_COLT,

		24,		
		1,
		8,
		24,
		8,
		1500,
		100,
		400,
		0,
		0,
		MOD_SILENCED_COLT,		// means of death

		JP_WPFLAG_SILENCED,
		WP_COLT,				// alternate fire mode
		qfalse,					// uses charge
		3.0f,
		2.0f,					// bonus xp for a headshot kill
		SK_LIGHT_WEAPONS
	},
};