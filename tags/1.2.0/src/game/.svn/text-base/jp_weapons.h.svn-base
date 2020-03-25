#ifndef JP_WEAPONS_HEADER
#define JP_WEAPONS_HEADER

#define JP_WPFLAG_NORELOAD	0x0001	// weapon cannot be reloaded mid-clip
#define JP_WPFLAG_HEADSHOT	0x0002	// weapon can deliver intentional headshots
#define JP_WPFLAG_ALTFIRE1	0x0004	// alternate fire switches weapon modes (scope)
#define JP_WPFLAG_ALTFIRE2	0x0008  // alternate fire occurs immediately (bayonette thrust)
#define JP_WPFLAG_IMPROVES	0x0010  // weapon accuracy improves with light weapons levels
#define JP_WPFLAG_AMMO		0x0020	// weapon recieves additional clips when skill levels increase
#define JP_WPFLAG_ISALT		0x0040	// is the weapon an alternate fire mode of another?
#define JP_WPFLAG_SILENCED	0x0080	// weapon is silenced
#define JP_WPFLAG_CANDROP	0x0100  // weapon can be dropped

#define JP_WEAPON_CLASS_ANY			0x0031
#define JP_WEAPON_CLASS_SOLDIER		0x0001
#define JP_WEAPON_CLASS_MEDIC		0x0002
#define JP_WEAPON_CLASS_ENGINEER	0x0004 
#define JP_WEAPON_CLASS_FIELDOPS	0x0008
#define JP_WEAPON_CLASS_COVERTOPS	0x0010

typedef struct	jp_weaponStat_s
{
	char		weaponName[32];				// name of the weapon (Garand)
	char		weaponFullName[32];			// full name of the weapon (M1 Garand Carbine)
	char		weaponDesc[64];				// description of the weapon
	int			weaponID;				// id number of weapon (WP_KNIFE, etc)

	int			maxAmmo;				// maximum ammo weapon carries (before skill bonuses)
	int			useAmmo;				// ammo used each time the weapon is fired
	int			maxClip;				// amount of ammo in each of the the weapon's clips
	int			startAmmo;				// ammount of ammo to start with
	int			startClip;				// amount of ammo in the starting clip
	int			reloadTime;				// time (in milliseconds) to reload the weapon
	int			nextShot;				// 
	int			fireDelay;				//
	int			heatRate;				// rate at which the weapon generates heat
	int			coolRate;				// rate at which the weapon disperses heat
	int			mod;					// Means of Death 

	float		barrelLength;			// length of barrel (in mm)
	float		weight;					// weight of weapon (in kg)
	char		*cartridge;

	int			flags;					// bitmask for weapon abilities
	int			altIndex;				// index of the alternate fire mode of the weapon (scope, bipod, etc)	
	qboolean	chargeCost;				// does the weapon use the charge bar
	float		xpReward;				// experience reward for a kill with this weapon
	float		xpBonus;				// experience bonus for a headshot with this weapon
	skillType_t	skillUsed;				// which skill does this weapon give experience for?    
	
} jp_weaponStat_t;

/*
EXAMPLE 
*/
/*

*/
#endif//JP_WEAPONS_HEADER

