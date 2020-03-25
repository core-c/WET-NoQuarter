/*
* ET <-> Omni-Bot interface source file.
* 
*/

#include <sstream>
#include <iomanip>

#ifdef OMNIBOTS

extern "C"
{
#include "g_local.h"
};

#include "../../Omnibot/Common/BotExports.h"
#include "../../Omnibot/ET/ET_Messages.h"

#define OMNIBOT_MIN_ENG 2
#define OMNIBOT_MIN_MED 2
#define OMNIBOT_MIN_FOP 1
#define OMNIBOT_MIN_SOL 2
#define OMNIBOT_MIN_COP 1

//////////////////////////////////////////////////////////////////////////

const int		MAX_CACHE = 32;
int				g_NumSmokeGrenades = 0;
gentity_t		*g_SmokeGrenadeCache[MAX_CACHE] = {0};

//////////////////////////////////////////////////////////////////////////

extern "C" const char *_GetEntityName(gentity_t *_ent)
{    
	// For goal names.
	if(_ent)
	{
		if(_ent->scriptName)
			return _ent->scriptName;
		else if(_ent->targetname)
			return _ent->targetname;
		else if(_ent->message)
			return _ent->message;
	}
	return NULL;
}

static void pfnPrintError(const char *_error);
static void pfnPrintMessage(const char *_msg);

// utils partly taken from id code
#define WC_WEAPON_TIME_LEFT level.time - ps->classWeaponTime
#define WC_SOLDIER_TIME		level.soldierChargeTime		[team-TEAM_AXIS]
#define WC_ENGINEER_TIME	level.engineerChargeTime	[team-TEAM_AXIS]
#define WC_FIELDOPS_TIME	level.lieutenantChargeTime	[team-TEAM_AXIS]
#define WC_MEDIC_TIME		level.medicChargeTime		[team-TEAM_AXIS]
#define WC_COVERTOPS_TIME	level.covertopsChargeTime	[team-TEAM_AXIS]

//////////////////////////////////////////////////////////////////////////

gentity_t *_GetEntityFromGameEntity(GameEntity _ent)
{
	gentity_t *pEnt = (gentity_t*)_ent;
	// Make sure it's within bounds.
	if(pEnt >= &g_entities[0] && pEnt < &g_entities[MAX_GENTITIES])
	{
		switch(pEnt-g_entities)
		{
		case ENTITYNUM_WORLD: // world ent not marked inuse for some reason
			return &g_entities[ENTITYNUM_WORLD];
		default:
			return pEnt->inuse ? pEnt : 0;
		}
	}
	return 0;
}

gentity_t *_GetEntityFromGameId(const int _gameId)
{
	if(_gameId >= 0 && _gameId < MAX_GENTITIES)
	{
		switch(_gameId)
		{
		case ENTITYNUM_WORLD: // world ent not marked inuse for some reason
			return &g_entities[ENTITYNUM_WORLD];
		default:
			return g_entities[_gameId].inuse ? &g_entities[_gameId] : 0;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

static qboolean weaponCharged(playerState_t* ps, team_t team, int weapon, int* skill)
{
	switch (weapon)
	{
	case WP_PANZERFAUST:
		if ( ps->eFlags & EF_PRONE )
			return qfalse;
		if ( skill[SK_HEAVY_WEAPONS] >= 1 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_SOLDIER_TIME * 0.66f)
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_SOLDIER_TIME)
		{
			return qfalse;
		}
		break;
	case WP_MORTAR_SET:
		if ( skill[SK_HEAVY_WEAPONS] >= 1 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_SOLDIER_TIME * 0.33f)
				return qfalse;
		}
		else if( WC_WEAPON_TIME_LEFT < WC_SOLDIER_TIME * 0.5f)
		{
			return qfalse;
		}
		break;
	case WP_SMOKE_BOMB:
	case WP_SATCHEL:
		if ( skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 2 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_COVERTOPS_TIME * 0.66f )
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_COVERTOPS_TIME )
		{
			return qfalse;
		}
		break;
	case WP_LANDMINE:
		if ( skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 )
		{
			if ( WC_WEAPON_TIME_LEFT < (WC_ENGINEER_TIME * 0.33f))
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < (WC_ENGINEER_TIME * 0.5f))
		{
			return qfalse;
		}
		break;
	case WP_DYNAMITE:
		if ( skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3 )
		{
			if ( WC_WEAPON_TIME_LEFT < (WC_ENGINEER_TIME * 0.66f) )
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_ENGINEER_TIME )
		{
			return qfalse;
		}
		break;
	case WP_MEDKIT:
		if ( skill[SK_FIRST_AID] >= 2 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_MEDIC_TIME * 0.15f )
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_MEDIC_TIME * 0.25f )
		{
			return qfalse;
		}
		break;
	case WP_AMMO:
		if ( skill[SK_SIGNALS] >= 1 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_FIELDOPS_TIME * 0.15f)
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_FIELDOPS_TIME * 0.25f)
		{
			return qfalse;
		}
		break;
	case WP_SMOKE_MARKER:
		if ( skill[SK_SIGNALS] >= 2 )
		{
			if ( WC_WEAPON_TIME_LEFT < WC_FIELDOPS_TIME * 0.66f)
				return qfalse;
		}
		else if ( WC_WEAPON_TIME_LEFT < WC_FIELDOPS_TIME )
		{
			return qfalse;
		}
		break;
	case WP_MEDIC_ADRENALINE:
		if ( WC_WEAPON_TIME_LEFT < WC_MEDIC_TIME )
		{
			return qfalse;
		}
		break;
	case WP_BINOCULARS:
		switch (ps->stats[ STAT_PLAYER_CLASS ])
		{
		case PC_FIELDOPS:
			if ( skill[SK_SIGNALS] >= 2 )
			{
				if ( WC_WEAPON_TIME_LEFT <= WC_FIELDOPS_TIME * 0.66f )
					return qfalse;
			}
			else if ( WC_WEAPON_TIME_LEFT <= WC_FIELDOPS_TIME )
			{
				return qfalse;
			}
			break;
		default:
			return qfalse;
		}
		break;
	case WP_GPG40:
	case WP_M7:
		if ( WC_WEAPON_TIME_LEFT < WC_ENGINEER_TIME * 0.5f )
		{
			return qfalse;
		}
		break;
	}
	return qtrue;
}

static int CountPlayerClass(team_t team, int playerClass)
{
	int num = 0;
	int i, j;

	for( i = 0; i < level.numConnectedClients; i++ )
	{
		j = level.sortedClients[i];

		if( level.clients[j].sess.sessionTeam != team )
			continue;

		if( level.clients[j].sess.latchPlayerType != playerClass )
			continue;
		num++;
	}
	return num;
}

int Bot_TeamGameToBot(int team)
{
	switch (team)
	{
	case TEAM_AXIS:
		return ET_TEAM_AXIS;
	case TEAM_ALLIES:
		return ET_TEAM_ALLIES;
	default:
		return ET_TEAM_NONE;
	}
}

static int playerClassBotToGame(int playerClass)
{
	switch (playerClass)
	{
	case ET_CLASS_SOLDIER:
		return PC_SOLDIER;
	case ET_CLASS_MEDIC:
		return PC_MEDIC;
	case ET_CLASS_ENGINEER:
		return PC_ENGINEER;
	case ET_CLASS_FIELDOPS:
		return PC_FIELDOPS;
	case ET_CLASS_COVERTOPS:
		return PC_COVERTOPS;
	default:
		return -1;
	}
}

int Bot_PlayerClassGameToBot(int playerClass)
{
	switch (playerClass)
	{
	case PC_SOLDIER:
		return ET_CLASS_SOLDIER;
	case PC_MEDIC:
		return ET_CLASS_MEDIC;
	case PC_ENGINEER:
		return ET_CLASS_ENGINEER;
	case PC_FIELDOPS:
		return ET_CLASS_FIELDOPS;
	case PC_COVERTOPS:
		return ET_CLASS_COVERTOPS;
	default:
		return ET_CLASS_NULL;
	}
}

static int _weaponBotToGame(int weapon)
{
	switch (weapon)
	{
	case ET_WP_ADRENALINE:
		return WP_MEDIC_ADRENALINE;
	case ET_WP_AKIMBO_COLT:
		return WP_AKIMBO_COLT;
	case ET_WP_AKIMBO_LUGER:
		return WP_AKIMBO_LUGER;
	case ET_WP_AKIMBO_SILENCED_COLT:
		return WP_AKIMBO_SILENCEDCOLT;
	case ET_WP_AKIMBO_SILENCED_LUGER:
		return WP_AKIMBO_SILENCEDLUGER;
	case ET_WP_AMMO_PACK:
		return WP_AMMO;
	case ET_WP_BINOCULARS:
		return WP_BINOCULARS;
	case ET_WP_CARBINE:
		return WP_CARBINE;
	case ET_WP_COLT:
		return WP_COLT;
	case ET_WP_DYNAMITE:
		return WP_DYNAMITE;
	case ET_WP_FG42:
		return WP_FG42;
	case ET_WP_FG42SCOPE:
		return WP_FG42SCOPE;
	case ET_WP_FLAMETHROWER:
		return WP_FLAMETHROWER;
	case ET_WP_GARAND:
		return WP_GARAND;
	case ET_WP_GARAND_SCOPE:
		return WP_GARAND_SCOPE;
	case ET_WP_GPG40:
		return WP_GPG40;
	case ET_WP_GREN_ALLIES:
		return WP_GRENADE_PINEAPPLE;
	case ET_WP_GREN_AXIS:
		return WP_GRENADE_LAUNCHER;
	case ET_WP_K43:
		return WP_K43;
	case ET_WP_K43_SCOPE:
		return WP_K43_SCOPE;
	case ET_WP_KAR98:
		return WP_KAR98;
	case ET_WP_KNIFE:
		return WP_KNIFE;
	case ET_WP_LANDMINE:
		return WP_LANDMINE;
	case ET_WP_LUGER:
		return WP_LUGER;
	case ET_WP_M7:
		return WP_M7;
	case ET_WP_MEDKIT:
		return WP_MEDKIT;
	case ET_WP_MOBILE_MG42:
		return WP_MOBILE_MG42;
	case ET_WP_MOBILE_MG42_SET:
		return WP_MOBILE_MG42_SET;
	case ET_WP_MORTAR:
		return WP_MORTAR;
	case ET_WP_MORTAR_SET:
		return WP_MORTAR_SET;
	case ET_WP_MP40:
		return WP_MP40;
	case ET_WP_PANZERFAUST:
		return WP_PANZERFAUST;
	case ET_WP_PLIERS:
		return WP_PLIERS;
	case ET_WP_SATCHEL:
		return WP_SATCHEL;
	case ET_WP_SATCHEL_DET:
		return WP_SATCHEL_DET;
	case ET_WP_SILENCED_COLT:
		return WP_SILENCED_COLT;
	case ET_WP_SILENCED_LUGER:
		return WP_SILENCER;
	case ET_WP_SMOKE_GRENADE:
		return WP_SMOKE_BOMB;
	case ET_WP_SMOKE_MARKER:
		return WP_SMOKE_MARKER;
	case ET_WP_STEN:
		return WP_STEN;
	case ET_WP_SYRINGE:
		return WP_MEDIC_SYRINGE;
	case ET_WP_THOMPSON:
		return WP_THOMPSON;
#ifdef JAYMOD_name
	case 75:
		return WP_POISON_SYRINGE;
	case 76:
		return WP_ADRENALINE_SHARE;
	case 77:
		return WP_M97;
	case 78:
		return WP_POISON_GAS;
	case 79:
		return WP_LANDMINE_BBETTY;
	case 80:
		return WP_LANDMINE_PGAS;
#endif
#ifdef NOQUARTER
	case 85:
		return WP_STG44;
	case 86:
		return WP_BAR;
	case 87:
		return WP_BAR_SET;
	case 88:
		return WP_MOBILE_BROWNING;
	case 89:
		return WP_MOBILE_BROWNING_SET;
	case 90:
		return WP_SHOTGUN;
	case 91:
		return WP_MP34;
	case 92:
		return WP_MORTAR2;
	case 93:
		return WP_MORTAR2_SET;
	case 94:
		return WP_KNIFE_KABAR;
	case 95:
		return WP_STEN_MKII;
	case 96:
		return WP_BAZOOKA;
	/*case 97:
		return WP_FIREBOLT;*/
	case 98:
		return WP_VENOM;
	case 99:
		return WP_POISON_SYRINGE;
#endif
	default:
		return WP_NONE;
	}
}

int Bot_WeaponGameToBot(int weapon)
{
	switch (weapon)
	{
	case WP_MEDIC_ADRENALINE:
		return ET_WP_ADRENALINE;
	case WP_AKIMBO_COLT:
		return ET_WP_AKIMBO_COLT;
	case WP_AKIMBO_LUGER:
		return ET_WP_AKIMBO_LUGER;
	case WP_AKIMBO_SILENCEDCOLT:
		return ET_WP_AKIMBO_SILENCED_COLT;
	case WP_AKIMBO_SILENCEDLUGER:
		return ET_WP_AKIMBO_SILENCED_LUGER;
	case WP_AMMO:
		return ET_WP_AMMO_PACK;
	case WP_BINOCULARS:
		return ET_WP_BINOCULARS;
	case WP_CARBINE:
		return ET_WP_CARBINE;
	case WP_COLT:
		return ET_WP_COLT;
	case WP_DYNAMITE:
		return ET_WP_DYNAMITE;
	case WP_FG42:
		return ET_WP_FG42;
	case WP_FG42SCOPE:
		return ET_WP_FG42SCOPE;
	case WP_FLAMETHROWER:
		return ET_WP_FLAMETHROWER;
	case WP_GARAND:
		return ET_WP_GARAND;
	case WP_GARAND_SCOPE:
		return ET_WP_GARAND_SCOPE;
	case WP_GPG40:
		return ET_WP_GPG40;
	case WP_GRENADE_PINEAPPLE:
		return ET_WP_GREN_ALLIES;
	case WP_GRENADE_LAUNCHER:
		return ET_WP_GREN_AXIS;
	case WP_K43:
		return ET_WP_K43;
	case WP_K43_SCOPE:
		return ET_WP_K43_SCOPE;
	case WP_KAR98:
		return ET_WP_KAR98;
	case WP_KNIFE:
		return ET_WP_KNIFE;
	case WP_LANDMINE:
		return ET_WP_LANDMINE;
	case WP_LUGER:
		return ET_WP_LUGER;
	case WP_M7:
		return ET_WP_M7;
	case WP_MEDKIT:
		return ET_WP_MEDKIT;
	case WP_MOBILE_MG42:
		return ET_WP_MOBILE_MG42;
	case WP_MOBILE_MG42_SET:
		return ET_WP_MOBILE_MG42_SET;
	case WP_MORTAR:
		return ET_WP_MORTAR;
	case WP_MORTAR_SET:
		return ET_WP_MORTAR_SET;
	case WP_MP40:
		return ET_WP_MP40;
	case WP_PANZERFAUST:
		return ET_WP_PANZERFAUST;
	case WP_PLIERS:
		return ET_WP_PLIERS;
	case WP_SATCHEL:
		return ET_WP_SATCHEL;
	case WP_SATCHEL_DET:
		return ET_WP_SATCHEL_DET;
	case WP_SILENCED_COLT:
		return ET_WP_SILENCED_COLT;
	case WP_SILENCER:
		return ET_WP_SILENCED_LUGER;
	case WP_SMOKE_BOMB:
		return ET_WP_SMOKE_GRENADE;
	case WP_SMOKE_MARKER:
		return ET_WP_SMOKE_MARKER;
	case WP_STEN:
		return ET_WP_STEN;
	case WP_MEDIC_SYRINGE:
		return ET_WP_SYRINGE;
	case WP_THOMPSON:
		return ET_WP_THOMPSON;
#ifdef JAYMOD_name
	case WP_POISON_SYRINGE:
		return 75;
	case WP_ADRENALINE_SHARE:
		return 76;
	case WP_M97:
		return 77;
	case WP_POISON_GAS:
		return 78;
	case WP_LANDMINE_BBETTY:
		return 79;
	case WP_LANDMINE_PGAS:
		return 80;
#endif
#ifdef NOQUARTER
	case WP_STG44:
		return 85;
	case WP_BAR:
		return 86;
	case WP_BAR_SET:
		return 87;
	case WP_MOBILE_BROWNING:
		return 88;
	case WP_MOBILE_BROWNING_SET:
		return 89;
	case WP_SHOTGUN:
		return 90;
	case WP_MP34:
		return 91;
	case WP_MORTAR2:
		return 92;
	case WP_MORTAR2_SET:
		return 93;
	case WP_KNIFE_KABAR:
		return 94;
	case WP_STEN_MKII:
		return 95;
	case WP_BAZOOKA:
		return 96;
/*	case WP_FIREBOLT:
		return 97;*/
	case WP_VENOM:
		return 98;
	case WP_POISON_SYRINGE:
		return 99;
#endif
	default:
		return ET_WP_NONE;
	}
}

static int Bot_HintGameToBot(gentity_t *_ent)
{
	int iHintType = CURSOR_HINT_NONE;
	if(_ent && _ent->client)
	{
		switch(_ent->client->ps.serverCursorHint)
		{
		case HINT_PLAYER:
			iHintType = CURSOR_HINT_PLAYER;
			break;
		case HINT_ACTIVATE:
			iHintType = CURSOR_HINT_ACTIVATE;
			break;
		case HINT_DOOR:
			iHintType = CURSOR_HINT_DOOR;
			break;
		case HINT_DOOR_ROTATING:
			iHintType = CURSOR_HINT_DOOR_ROTATING;
			break;
		case HINT_DOOR_LOCKED:
			iHintType = CURSOR_HINT_DOOR_LOCKED;
			break;
		case HINT_DOOR_ROTATING_LOCKED:
			iHintType = CURSOR_HINT_DOOR_LOCKED;
			break;
		case HINT_MG42:
			iHintType = CURSOR_HINT_MG42;
			break;
		case HINT_BREAKABLE:
			iHintType = CURSOR_HINT_BREAKABLE;
			break;
		case HINT_BREAKABLE_DYNAMITE:
			iHintType = CURSOR_HINT_BREAKABLE_DYNAMITE;
			break;
		case HINT_CHAIR:
			iHintType = CURSOR_HINT_CHAIR;
			break;
		case HINT_ALARM:
			iHintType = CURSOR_HINT_ALARM;
			break;
		case HINT_HEALTH:
			iHintType = CURSOR_HINT_HEALTH;
			break;
		case HINT_TREASURE:
			iHintType = CURSOR_HINT_TREASURE;
			break;
		case HINT_KNIFE:
			iHintType = CURSOR_HINT_KNIFE;
			break;
		case HINT_LADDER:
			iHintType = CURSOR_HINT_LADDER;
			break;
		case HINT_BUTTON:
			iHintType = CURSOR_HINT_BUTTON;
			break;
		case HINT_WATER:
			iHintType = CURSOR_HINT_WATER;
			break;
#ifndef NOQUARTER
		case HINT_CAUTION:
			iHintType = CURSOR_HINT_CAUTION;
			break;
		case HINT_DANGER:
			iHintType = CURSOR_HINT_DANGER;
			break;
		case HINT_SECRET:
			iHintType = CURSOR_HINT_SECRET;
			break;
		case HINT_QUESTION:
			iHintType = CURSOR_HINT_QUESTION;
			break;
		case HINT_EXCLAMATION:
			iHintType = CURSOR_HINT_EXCLAMATION;
			break;
		case HINT_CLIPBOARD:
			iHintType = CURSOR_HINT_CLIPBOARD;
			break;
#endif
		case HINT_WEAPON:
			iHintType = CURSOR_HINT_WEAPON;
			break;
		case HINT_AMMO:
			iHintType = CURSOR_HINT_AMMO;
			break;
#ifndef NOQUARTER
		case HINT_ARMOR:
			iHintType = CURSOR_HINT_ARMOR;
			break;
#endif
		case HINT_POWERUP:
			iHintType = CURSOR_HINT_POWERUP;
			break;
		case HINT_HOLDABLE:
			iHintType = CURSOR_HINT_HOLDABLE;
			break;
		case HINT_INVENTORY:
			iHintType = CURSOR_HINT_INVENTORY;
			break;
		case HINT_SCENARIC:
			iHintType = CURSOR_HINT_SCENARIC;
			break;
		case HINT_EXIT:
			iHintType = CURSOR_HINT_EXIT;
			break;
		case HINT_NOEXIT:
			iHintType = CURSOR_HINT_NOEXIT;
			break;
		case HINT_PLYR_FRIEND:
			iHintType = CURSOR_HINT_PLYR_FRIEND;
			break;
		case HINT_PLYR_NEUTRAL:
			iHintType = CURSOR_HINT_PLYR_NEUTRAL;
			break;
		case HINT_PLYR_ENEMY:
			iHintType = CURSOR_HINT_PLYR_ENEMY;
			break;
		case HINT_PLYR_UNKNOWN:
			iHintType = CURSOR_HINT_PLYR_UNKNOWN;
			break;
		case HINT_BUILD:
			iHintType = CURSOR_HINT_BUILD;
			break;
		case HINT_DISARM:
			iHintType = CURSOR_HINT_DISARM;
			break;
		case HINT_REVIVE:
			iHintType = CURSOR_HINT_REVIVE;
			break;
		case HINT_DYNAMITE:
			iHintType = CURSOR_HINT_DYNAMITE;
			break;
		case HINT_CONSTRUCTIBLE:
			iHintType = CURSOR_HINT_CONSTRUCTIBLE;
			break;
		case HINT_UNIFORM:
			iHintType = CURSOR_HINT_UNIFORM;
			break;
		case HINT_LANDMINE:
			iHintType = CURSOR_HINT_LANDMINE;
			break;
		case HINT_TANK:
			iHintType = CURSOR_HINT_TANK;
			break;
		case HINT_SATCHELCHARGE:
			iHintType = CURSOR_HINT_SATCHELCHARGE;
			break;
#ifndef NOQUARTER
		case HINT_LOCKPICK:
			iHintType = CURSOR_HINT_LOCKPICK;
			break;
#endif
		}
	}
	return iHintType;
}

static int _choosePriWeap(int playerClass, int team)
{
	switch (playerClass)
	{
	case ET_CLASS_SOLDIER:
		{
#ifdef NOQUARTER
			if (jp_insanity.integer & JP_INSANITY_VENOM && ( rand() % 6 ) == 5 ){
				return 98;
			} 
			else if(team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					86, // BAR
					96, // BAZOOKA
					88, // BROWNING
					ET_WP_FLAMETHROWER,
					ET_WP_MORTAR
				};

				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					85, // STG44
					ET_WP_PANZERFAUST,
					ET_WP_MOBILE_MG42, 
					ET_WP_FLAMETHROWER,
					92 // MORTAR2
				};

				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
#else
			int wpns[] = 
			{
				// add shit as needed
				//ET_WP_THOMPSON/ET_WP_MP40, // pointless?
				ET_WP_PANZERFAUST,
				ET_WP_MOBILE_MG42,
				ET_WP_FLAMETHROWER,
				ET_WP_MORTAR,
#ifdef JAYMOD_name
				77 //WP_M97
#endif
			};
			int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
			return wpns[rInt];
#endif
		}
	case ET_CLASS_MEDIC:
		{
			if(team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_THOMPSON,
#ifdef JAYMOD_name
						77 //WP_M97
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_MP40,
#ifdef JAYMOD_name
					77 //WP_M97
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	case ET_CLASS_ENGINEER:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_THOMPSON,
					ET_WP_CARBINE,
#ifdef JAYMOD_name
					77 //WP_M97
#endif
#ifdef NOQUARTER
						90 //SHOTGUN
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_MP40,
					ET_WP_KAR98,
#ifdef JAYMOD_name
					77 //WP_M97
#endif
#ifdef NOQUARTER
						90 //SHOTGUN
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}	
	case ET_CLASS_FIELDOPS:
		{
			if(team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_THOMPSON,
#ifdef JAYMOD_name
					77 //WP_M97
#endif
#ifdef NOQUARTER
						90 //SHOTGUN
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_MP40,
#ifdef JAYMOD_name
					77 //WP_M97
#endif
#ifdef NOQUARTER
						90 //SHOTGUN
#endif
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	case ET_CLASS_COVERTOPS:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_STEN,
#ifdef NOQUARTER
						86, //BAR
#else
					ET_WP_FG42,
#endif
					ET_WP_GARAND
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
#ifdef NOQUARTER
						91, //MP34
#else
					ET_WP_STEN,
#endif
					ET_WP_FG42,
					ET_WP_K43
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	default:
		return ET_WP_NONE;
	}
}

static int _chooseSecWeap(int playerClass, int team)
{
	switch (playerClass)
	{
	case ET_CLASS_SOLDIER:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_COLT,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_LUGER,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	case ET_CLASS_MEDIC:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_COLT,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_LUGER,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	case ET_CLASS_ENGINEER:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_COLT,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_LUGER,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}	
	case ET_CLASS_FIELDOPS:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_COLT,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_LUGER,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	case ET_CLASS_COVERTOPS:
		{
			if (team == ET_TEAM_ALLIES)
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_SILENCED_COLT,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
			else
			{
				int wpns[] = 
				{
					// add shit as needed
					ET_WP_SILENCED_LUGER,
				};
				int rInt = rand() % (sizeof(wpns) / sizeof(wpns[0]));
				return wpns[rInt];
			}
		}
	default:
		return ET_WP_NONE;
	}
}

static void ReTransmitWeapons(const gentity_t* bot)
{
	int weapon;

	if (!bot || !bot->client)
		return;

	Bot_Event_ResetWeapons(bot-g_entities);
	
	for (weapon = WP_NONE + 1; weapon < WP_NUM_WEAPONS; weapon++)
	{
		if (COM_BitCheck(bot->client->ps.weapons, weapon))
		{
			Bot_Event_AddWeapon(bot-g_entities, Bot_WeaponGameToBot(weapon));
		}
	}
}

#define MAX_SMOKE_RADIUS 320.0
#define MAX_SMOKE_RADIUS_TIME 10000.0
#define UNAFFECTED_BY_SMOKE_DIST SQR(100)

gentity_t *Bot_EntInvisibleBySmokeBomb(vec3_t start, vec3_t end) 
{
	gentity_t *ent = NULL;
	vec3_t smokeCenter;
	float smokeRadius;

	// if the target is close enough, vision is not affected by smoke bomb 
	if( DistanceSquared(start, end) < UNAFFECTED_BY_SMOKE_DIST ) 
	{
		//pfnPrintMessage("within unaffected dist");
		return 0;
	}

	//while (ent = G_FindSmokeBomb( ent )) 
	for(int i = 0; i < g_NumSmokeGrenades; ++i)
	{
		ent = g_SmokeGrenadeCache[i];

		if (ent->s.effect1Time == 16)
		{
			//pfnPrintMessage("smoke not up yet");
			// xkan, the smoke has not really started yet, see weapon_smokeBombExplode
			// and CG_RenderSmokeGrenadeSmoke
			continue;
		}
		// check the distance 
		VectorCopy( ent->s.pos.trBase, smokeCenter );
		// raise the center to better match the position of the smoke, see
		// CG_SpawnSmokeSprite().
		smokeCenter[2] += 32;
		// smoke sprite has a maximum radius of 640/2. and it takes a while for it to 
		// reach that size, so adjust the radius accordingly.
		smokeRadius = MAX_SMOKE_RADIUS * ((level.time - ent->grenadeExplodeTime)/MAX_SMOKE_RADIUS_TIME);
		if (smokeRadius > MAX_SMOKE_RADIUS)
			smokeRadius = MAX_SMOKE_RADIUS;
		// if distance from line is short enough, vision is blocked by smoke

		//float fColorRed[3] = { 1.0f, 0.0f, 0.0f };
		//float fColorGrn[3] = { 0.0f, 1.0f, 0.0f };

		if (DistanceFromLineSquared(smokeCenter, start, end) < (smokeRadius * smokeRadius))
		{
			//pfnPrintMessage("hid by smoke");			
			//pfnAddTempDisplayLine(smokeCenter, end, fColorGrn);
			return ent;
		}
		//pfnAddTempDisplayLine(smokeCenter, start, fColorRed);
	}

	return 0;
}

static int _GetEntityTeam(gentity_t *_ent)
{
	switch(_ent->s.eType) 
	{		
	case ET_PLAYER:
		return Bot_TeamGameToBot(_ent->client->sess.sessionTeam);
	case ET_CORPSE:
		return Bot_TeamGameToBot(BODY_TEAM(_ent));
	case ET_MISSILE:
		if(_ent->s.weapon == WP_LANDMINE || _ent->s.weapon == WP_DYNAMITE)
			return Bot_TeamGameToBot(G_LandmineTeam(_ent));
		// Let this fall through
	default:
		return Bot_TeamGameToBot(_ent->s.teamNum);
	}

	return 0;
}

static int _GetEntityClass(gentity_t *_ent)
{
	switch(_ent->s.eType)
	{
	case ET_GENERAL:
		{
			if(!Q_stricmp(_ent->classname, "func_invisible_user"))
				return ENT_CLASS_GENERIC_BUTTON;
			else if(!Q_stricmp(_ent->classname, "func_button"))
				return ENT_CLASS_GENERIC_BUTTON;
			break;
		}
	case ET_PLAYER:
		{
			if (!_ent->client || (_ent->entstate == STATE_INVISIBLE) ||
				(_ent->client->ps.pm_type == PM_SPECTATOR) ||
				(_ent->client->sess.sessionTeam != TEAM_AXIS &&
				_ent->client->sess.sessionTeam != TEAM_ALLIES))
				break;

			// Special case for dead players that haven't respawned.
			if(_ent->health <= 0)
				return ET_CLASSEX_INJUREDPLAYER;

			return Bot_PlayerClassGameToBot(_ent->client->sess.playerType);			
		}
	case ET_ITEM:
		{				
			if(!Q_strncmp(_ent->classname, "item_health", strlen("item_health"))) 
				return ENT_CLASS_GENERIC_HEALTH;
			else if(!Q_strncmp(_ent->classname, "weapon_magicammo", strlen("weapon_magicammo")))
				return ENT_CLASS_GENERIC_AMMO;
			else if(!Q_stricmp(_ent->classname, "item_treasure")) 
				return ET_CLASSEX_TREASURE;
			break;
		}
	case ET_CORPSE:
		{
			return ET_CLASSEX_CORPSE;
		}
	case ET_MISSILE:
		{
			// Register certain weapons as threats to avoid or whatever.
			switch(_ent->s.weapon) 
			{
			case WP_GRENADE_PINEAPPLE:
				return ET_CLASSEX_GRENADE;
			case WP_PANZERFAUST:
				return ET_CLASSEX_ROCKET;
			case WP_ARTY:
				return ET_CLASSEX_ARTY;
			case WP_DYNAMITE:
				return ET_CLASSEX_DYNAMITE;
			case WP_SMOKE_MARKER:
				return ET_CLASSEX_SMOKEMARKER;
			case WP_SMOKE_BOMB:
				return ET_CLASSEX_SMOKEBOMB;
			case WP_LANDMINE:
				return ET_CLASSEX_MINE;
			case WP_SATCHEL:
				return ET_CLASSEX_SATCHEL;
			case WP_M7:
				return ET_CLASSEX_M7_GRENADE;
			case WP_GPG40:
				return ET_CLASSEX_GPG40_GRENADE;
			case WP_MORTAR_SET:
				return ET_CLASSEX_MORTAR;
			default:
				if(!Q_strncmp(_ent->classname, "air strike", sizeof("air strike")))
				{
					return ET_CLASSEX_AIRSTRIKE;
				}
			}
			break;
		}
	case ET_FLAMETHROWER_CHUNK:
		{
			return ET_CLASSEX_FLAMECHUNK;
			break;
		}
	case ET_MOVER:
		{
			if (!Q_stricmp(_ent->classname, "script_mover"))
			{
				if(_ent->count > 0)
					return (_ent->spawnflags & 4) ? ET_CLASSEX_VEHICLE_HVY : ET_CLASSEX_VEHICLE;
				if(_ent->model2)
					return ET_CLASSEX_VEHICLE_NODAMAGE;
			}
			/*else if (!Q_stricmp(pCurrent->classname, "props_flamebarrel"))
			{
			info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			}
			else if (!Q_stricmp(pCurrent->classname, "props_statue"))
			{
			info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			}*/
			break;
		}
	case ET_MG42_BARREL:
		{
			if((_ent->health > 0) &&
				(_ent->entstate != STATE_INVISIBLE) && 
				(_ent->entstate != STATE_UNDERCONSTRUCTION))
			{
				return ET_CLASSEX_MG42MOUNT;			
			}			
			break;
		}
	case ET_AAGUN:
		{
			/*if((pCurrent->health > 0) &&
			(pCurrent->entstate != STATE_INVISIBLE) && 
			(pCurrent->entstate != STATE_UNDERCONSTRUCTION))
			{
			}*/
			break;
		}
	case ET_EXPLOSIVE:
		{
			if (!(_ent->spawnflags & EXPLOSIVE_TANK) &&
				(_ent->constructibleStats.weaponclass != 1) &&
				(_ent->constructibleStats.weaponclass != 2))// &&
				//(_ent->health > 0) && (_ent->takedamage == qtrue))
			{
				return ET_CLASSEX_BREAKABLE;
			}
			break;
		}
	case ET_CONSTRUCTIBLE:
		{
			//if (G_ConstructionIsPartlyBuilt(pCurrent) &&
			//	!(pCurrent->spawnflags & CONSTRUCTIBLE_INVULNERABLE) &&
			//	 (pCurrent->constructibleStats.weaponclass == 0))
			//{
			//	info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			//	info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			//}
			//else
			//{
			//continue;
			//}
			//break;
		}
	case ET_HEALER:
		{
			return ET_CLASSEX_HEALTHCABINET;
		}
	case ET_SUPPLIER:
		{
			return ET_CLASSEX_AMMOCABINET;
		}
	default:
		break;
	};
	return 0;
}

qboolean _TankIsMountable( gentity_t *_ent ) 
{
	if( !(_ent->spawnflags & 128) ) 
		return qfalse;

	if( level.disableTankEnter )
		return qfalse;

	/*if( G_TankIsOccupied( ent ) )
	return qfalse;*/
	if(g_OmniBotFlags.integer & OBF_DONT_MOUNT_TANKS)
		return qfalse;

	if( _ent->health <= 0 )
		return qfalse;

	return qtrue;
}

qboolean _EmplacedGunIsMountable( gentity_t *_ent )
{
	if(g_OmniBotFlags.integer & OBF_DONT_MOUNT_GUNS)
		return qfalse;
	return qtrue;
}


// debug utils
static void pfnPrintError(const char *_error)
{
	if (_error)
		G_Printf("%s%s\n", S_COLOR_RED, _error);
}

static void pfnPrintMessage(const char *_msg)
{
	if (_msg)
		G_Printf("%s%s\n", S_COLOR_GREEN, _msg);
}

void Omnibot_Load_PrintMsg(const char *_msg)
{
	G_Printf("Omni-bot: %s%s\n", S_COLOR_GREEN, _msg);
}

void Omnibot_Load_PrintErr(const char *_msg)
{
	G_Printf("Omni-bot: %s%s\n", S_COLOR_RED, _msg);
}

int g_LastScreenMessageTime = 0;
static void pfnPrintScreenMessage(const int _client, const float _pos[3], float _duration, 
								  const obColor &_color, const char *_msg)
{
	if(_msg && (g_LastScreenMessageTime != level.time))
	{
		trap_SendServerCommand(_client, va("cp \"%s\"", _msg));
		g_LastScreenMessageTime = level.time;
	}
}

// core functions
static obResult pfnChangeTeam(int _client, int _newteam, const MessageHelper *_data)
{
	gentity_t* bot;
	char* teamName;

	bot = &g_entities[_client];

	// find a team if we didn't get one and we need one ;-)
	if (_newteam != ET_TEAM_ALLIES && _newteam != ET_TEAM_AXIS)
	{
		if ((_newteam == RANDOM_TEAM) ||
			(bot->client->sess.sessionTeam != TEAM_AXIS &&
			bot->client->sess.sessionTeam != TEAM_ALLIES))
		{
			if (TeamCount(_client, TEAM_ALLIES) <= TeamCount(_client, TEAM_AXIS)) 
			{
				_newteam = ET_TEAM_ALLIES;
			}
			else
			{
				_newteam = ET_TEAM_AXIS;
			}
		}
		else
		{
			// remember old team
			_newteam = Bot_TeamGameToBot(bot->client->sess.sessionTeam);
		}
	}

	if (_newteam == ET_TEAM_AXIS) {
		teamName = "axis";
	} else {
		teamName = "allies";
	}

	// always go to spectator first to solve problems on map restarts
	//SetTeam(bot, "spectator", qtrue, -1, -1, qfalse);

	Msg_PlayerChooseEquipment *pMsg = 0;
	if(_data) _data->Get2(pMsg);
	if(pMsg)
	{
		if(pMsg->m_WeaponChoice[0])
			bot->client->sess.latchPlayerWeapon = _weaponBotToGame(pMsg->m_WeaponChoice[0]);
		if(pMsg->m_WeaponChoice[1])
			bot->client->sess.latchPlayerWeapon2 = _weaponBotToGame(pMsg->m_WeaponChoice[1]);
	}
	else
	{
		//G_IsWeaponDisabled ?
		if(!bot->client->sess.latchPlayerWeapon || Bot_TeamGameToBot(bot->client->sess.sessionTeam) != _newteam)
			bot->client->sess.latchPlayerWeapon = 
			_weaponBotToGame(_choosePriWeap(Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType), _newteam));
		if(!bot->client->sess.latchPlayerWeapon2 || Bot_TeamGameToBot(bot->client->sess.sessionTeam) != _newteam)
			bot->client->sess.latchPlayerWeapon2 = 
			_weaponBotToGame(_chooseSecWeap(Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType), _newteam));
	}

	//if (!SetTeam(bot, teamName, qtrue, -1, -1, qfalse)) {
	//	pfnPrintError("Bot could not join team!");
	//	return 0;
	//}

	// if SetTeam() fails, be sure to at least send a note to the bot about the current team
	// (else this won't be neccessary because on respawn messages will be sent automatically)
	if (!SetTeam(bot, teamName, qtrue, (weapon_t)-1, (weapon_t)-1, qfalse))
	{
		Bot_Event_ChangeTeam(bot-g_entities, Bot_TeamGameToBot(bot->client->sess.sessionTeam));

		// also retransmit weapons stuff
		//ReTransmitWeapons(bot);
	}

	return Success;
}

static obResult pfnChangeClass(int _client, int _newclass, const MessageHelper *_data)
{
	gentity_t* bot;
	int team;
	team_t predictedTeam = TEAM_FREE;

	bot = &g_entities[_client];

	// find playerclass if we didn't got one
	if (_newclass <= ET_CLASS_NULL || _newclass >= ET_CLASS_MAX)
	{
		if ((_newclass == RANDOM_CLASS) || (bot->client->sess.latchPlayerType < 0) ||
			(bot->client->sess.latchPlayerType >= NUM_PLAYER_CLASSES))
		{
			if (TeamCount(_client, TEAM_ALLIES) <= TeamCount(_client, TEAM_AXIS)) 
			{
				predictedTeam = TEAM_ALLIES;
			} else {
				predictedTeam = TEAM_AXIS;
			}

			if (CountPlayerClass(predictedTeam, PC_ENGINEER) < OMNIBOT_MIN_ENG) {
				_newclass = ET_CLASS_ENGINEER;
			} else if (CountPlayerClass(predictedTeam, PC_MEDIC) < OMNIBOT_MIN_MED) {
				_newclass = ET_CLASS_MEDIC;
			} else if (CountPlayerClass(predictedTeam, PC_FIELDOPS) < OMNIBOT_MIN_FOP) {
				_newclass = ET_CLASS_FIELDOPS;
			} else if (CountPlayerClass(predictedTeam, PC_SOLDIER) < OMNIBOT_MIN_SOL) {
				_newclass = ET_CLASS_SOLDIER;
			} else if (CountPlayerClass(predictedTeam, PC_COVERTOPS) < OMNIBOT_MIN_COP) {
				_newclass = ET_CLASS_COVERTOPS;
			} else {
				_newclass = Bot_PlayerClassGameToBot(rand() % NUM_PLAYER_CLASSES);
			}
		}
		else
		{
			_newclass = Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType);
		}
	}

	team = Bot_TeamGameToBot(bot->client->sess.sessionTeam);
	bot->client->sess.latchPlayerType = playerClassBotToGame(_newclass);

	Msg_PlayerChooseEquipment *pMsg = 0;
	if(_data) _data->Get2(pMsg);
	if(pMsg)
	{
		if(pMsg->m_WeaponChoice[0])
			bot->client->sess.latchPlayerWeapon = _weaponBotToGame(pMsg->m_WeaponChoice[0]);
		if(pMsg->m_WeaponChoice[1])
			bot->client->sess.latchPlayerWeapon2 = _weaponBotToGame(pMsg->m_WeaponChoice[1]);
	}
	else
	{
		//G_IsWeaponDisabled ?
		if(!bot->client->sess.latchPlayerWeapon || bot->client->sess.latchPlayerType != bot->client->sess.playerType)
			bot->client->sess.latchPlayerWeapon = 
			_weaponBotToGame(_choosePriWeap(Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType), team));
		if(!bot->client->sess.latchPlayerWeapon2 || bot->client->sess.latchPlayerType != bot->client->sess.playerType)
			bot->client->sess.latchPlayerWeapon2 = 
			_weaponBotToGame(_chooseSecWeap(Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType), team));
	}

	// commit suicide to ensure new class is used
	if (bot->client->sess.latchPlayerType != bot->client->sess.playerType)
	{
		Cmd_Kill_f(bot);
		Bot_Event_ChangeClass(_client, Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType));
	}
	else
	{
		// be sure to at least send a note to the bot about the current class
		// (else this won't be neccessary because on respawn messages will be sent automatically)		
		Bot_Event_ChangeClass(_client, Bot_PlayerClassGameToBot(bot->client->sess.playerType));

		// also retransmit weapons stuff
		ReTransmitWeapons(bot);
	}

	return Success;
}

static int pfnAddBot(const MessageHelper &_data)
{
	Msg_Addbot *pMsg = _data.Get<Msg_Addbot>();

	int num;
	char userinfo[MAX_INFO_STRING] = {0};
	gentity_t* bot;

	num = trap_BotAllocateClient(0);

	std::stringstream guid;
	guid << "OMNIBOT" << std::setw(2) << std::setfill('0') << num;

	if (num < 0) 
	{
		pfnPrintError("Could not add bot!");
		pfnPrintError("No free slots!");
		return -1;
	}
	
	bot = &g_entities[num];

	Info_SetValueForKey(userinfo, "name", pMsg->m_Name);
	Info_SetValueForKey(userinfo, "rate", "25000");
	Info_SetValueForKey(userinfo, "snaps", "20");
	Info_SetValueForKey(userinfo, "ip", "localhost");
	Info_SetValueForKey(userinfo, "cl_guid", guid.str().c_str());

	trap_SetUserinfo(num, userinfo);

	const char *s = 0;
	if(s = ClientConnect(num, qtrue, qtrue)) 
	{
		pfnPrintError(va("Could not connect bot: %s", s));
		num = -1;
	}
	return num;
}

static int pfnRemoveBot(const char *_name)
{
	int i;
	char cleanNetName[MAX_NETNAME];
	char cleanName[MAX_NAME_LENGTH];

	Q_strncpyz(cleanName, _name, MAX_NAME_LENGTH);
	Q_CleanStr(cleanName);

	for(i = 0; i < MAX_CLIENTS; i++) 
	{
		if(!g_entities[i].inuse)
			continue;
		if (!(g_entities[i].r.svFlags & SVF_BOT))
			continue;

		// clean stuff
		Q_strncpyz(cleanNetName, g_entities[i].client->pers.netname, MAX_NETNAME);
		Q_CleanStr(cleanNetName);

		if(!Q_stricmp(cleanNetName, cleanName))
		{
			trap_DropClient(i, "disconnected", 0);
			return i;
		}
	}
	return -1;
}

int obUtilBotContentsFromGameContents(int _contents)
{
	int iBotContents = 0;
	if(_contents & CONTENTS_SOLID)
		iBotContents |= CONT_SOLID;
	if(_contents & CONTENTS_WATER)
		iBotContents |= CONT_WATER;
	if(_contents & CONTENTS_SLIME)
		iBotContents |= CONT_SLIME;
	if(_contents & CONTENTS_FOG)
		iBotContents |= CONT_FOG;
	if(_contents & CONTENTS_TELEPORTER)
		iBotContents |= CONT_TELEPORTER;
	if(_contents & CONTENTS_MOVER)
		iBotContents |= CONT_MOVER;
	if(_contents & CONTENTS_TRIGGER)
		iBotContents |= CONT_TRIGGER;
	if(_contents & CONTENTS_LAVA)
		iBotContents |= CONT_LAVA;
	return iBotContents;
}

int obUtilBotSurfaceFromGameSurface(int _surfaceflags)
{
	int iBotSurface = 0;
	if(_surfaceflags & SURF_SLICK)
		iBotSurface |= SURFACE_SLICK;
	return iBotSurface;
}

int pfnGetPointContents(const float _pos[3])
{
	vec3_t vpos = { _pos[0], _pos[1], _pos[2] };
	int iContents = trap_PointContents(vpos, -1);
	return obUtilBotContentsFromGameContents(iContents);
}

static obBool pfnIsInPVS(const float _pos[3], const float _target[3])
{
	return trap_InPVS(_pos, _target) ? True : False;
}

static obResult pfnTraceLine(obTraceResult *_result,
						const float _start[3],
						const float _end[3],
						const AABB *_pBBox ,
						int _mask, int _user, obBool _bUsePVS)
{
	trace_t tr;
	int iMask = 0;
	qboolean bInPVS = _bUsePVS ? trap_InPVS(_start, _end) : qtrue;
	if(bInPVS)
	{
		// Set up the collision masks
		if(_mask & TR_MASK_ALL)
			iMask = MASK_ALL;
		else
		{
			if(_mask & TR_MASK_SOLID)
				iMask |= MASK_SOLID;
			if(_mask & TR_MASK_PLAYER)
				iMask |= MASK_PLAYERSOLID;
			if(_mask & TR_MASK_SHOT)
				iMask |= MASK_SHOT;
			if(_mask & TR_MASK_OPAQUE)
				iMask |= MASK_OPAQUE;
			if(_mask & TR_MASK_WATER)
				iMask |= MASK_WATER;
			if(_mask & TR_MASK_PLAYERCLIP)
				iMask |= CONTENTS_PLAYERCLIP;
			if(_mask & TR_MASK_FLOODFILL)
				iMask |= CONTENTS_PLAYERCLIP|CONTENTS_SOLID;
			
			if(_mask & TR_MASK_SMOKEBOMB)
			{
				gentity_t *pSmokeBlocker = Bot_EntInvisibleBySmokeBomb((float*)_start, (float*)_end);
				if(pSmokeBlocker)
				{
					_result->m_Fraction = 0.0f;
					_result->m_HitEntity = pSmokeBlocker;
					return Success;
				}
			}
		}

		trap_Trace(&tr, _start,
			_pBBox ? _pBBox->m_Mins : NULL,
			_pBBox ? _pBBox->m_Maxs : NULL,
			_end, _user, iMask);

		if((tr.entityNum != ENTITYNUM_WORLD) && (tr.entityNum != ENTITYNUM_NONE))
			_result->m_HitEntity = &g_entities[tr.entityNum];
		else 
			_result->m_HitEntity = 0;

		// Fill in the bot traceflag.
		_result->m_Fraction = tr.fraction;
		_result->m_StartSolid = tr.startsolid;
		_result->m_Endpos[0] = tr.endpos[0];
		_result->m_Endpos[1] = tr.endpos[1];
		_result->m_Endpos[2] = tr.endpos[2];
		_result->m_Normal[0] = tr.plane.normal[0];
		_result->m_Normal[1] = tr.plane.normal[1];
		_result->m_Normal[2] = tr.plane.normal[2];
		_result->m_Contents = obUtilBotContentsFromGameContents(tr.contents);
		_result->m_Surface = obUtilBotSurfaceFromGameSurface(tr.surfaceFlags);
	}
	else
	{
		// Not in PVS
		_result->m_Fraction = 0.0f;
		_result->m_HitEntity = 0;
	}
	return bInPVS ? Success : OutOfPVS;
}

static void pfnUpdateBotInput(int _client, const ClientInput *_input)
{
	static usercmd_t cmd;
	vec3_t angles, forward, right;
	gentity_t *bot = &g_entities[_client];

	// only causes problems
	bot->client->ps.pm_flags &= ~PMF_RESPAWNED;

	memset(&cmd, 0, sizeof(cmd));

	cmd.identClient = _client;
	cmd.serverTime = level.time;

	// Set the weapon
	cmd.weapon = _weaponBotToGame(_input->m_CurrentWeapon);

	// Process the bot keypresses.	
	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_ATTACK1))
		cmd.buttons |= BUTTON_ATTACK;

	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_WALK))
		cmd.buttons |= BUTTON_WALKING;
	else if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_SPRINT))
		cmd.buttons |= BUTTON_SPRINT;

	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_USE))
		cmd.buttons |= BUTTON_ACTIVATE;

	// wbuttons
	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_ATTACK2))
		cmd.wbuttons |= WBUTTON_ATTACK2;

	// if we have prone held
	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_PRONE))
	{
		// and we're not prone, go prone
		if(!(bot->client->ps.eFlags & EF_PRONE))
			cmd.wbuttons |= WBUTTON_PRONE;
	}
	else
	{
		// if prone is not held, and we're prone, exit prone
		if(bot->client->ps.eFlags & EF_PRONE)
			cmd.wbuttons |= WBUTTON_PRONE;
	}

	// if we have aim held
	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_AIM))
	{
		//if(!(bot->s.eFlags & EF_ZOOMING))
		cmd.wbuttons |= WBUTTON_ZOOM;
	}
	else
	{
		// if aim not held and we're zooming, zoom out.
		//if(bot->s.eFlags & EF_ZOOMING)
		//{
		//	trap_EA_Command(_client, "-zoom");
		//	//cmd.wbuttons |= WBUTTON_ZOOM;
		//}
	}

	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_LEANLEFT))
		cmd.wbuttons |= WBUTTON_LEANLEFT;
	else if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_LEANRIGHT))
		cmd.wbuttons |= WBUTTON_LEANRIGHT;

	if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_RELOAD))
	{
		// watch for gpg40 and m7 as these need to be switched for reloading
		switch(_input->m_CurrentWeapon)
		{
		case ET_WP_GPG40:
			cmd.weapon = _weaponBotToGame(ET_WP_KAR98);
			break;
		case ET_WP_M7:
			cmd.weapon = _weaponBotToGame(ET_WP_CARBINE);
			break;
		case ET_WP_GARAND_SCOPE:
			cmd.weapon = _weaponBotToGame(ET_WP_GARAND);
			break;
		case ET_WP_FG42SCOPE:
			cmd.weapon = _weaponBotToGame(ET_WP_FG42);
			break;
		case ET_WP_K43_SCOPE:
			cmd.weapon = _weaponBotToGame(ET_WP_K43);
			break;
		default:
			cmd.wbuttons |= WBUTTON_RELOAD;
		}
	}

	// don't process view angles and moving stuff when dead
	if(bot->client->ps.pm_type >= PM_DEAD || bot->client->ps.pm_flags & (PMF_LIMBO|PMF_TIME_LOCKPLAYER))
	{
		// cant move in these states
		cmd.buttons &= ~BUTTON_ATTACK;
		cmd.wbuttons &= ~WBUTTON_ATTACK2;
	}
	else
	{
		float fMaxSpeed = 127.f;

		// Convert the bots vector to angles and set the view angle to the orientation
		vectoangles(_input->m_Facing, angles);
		SetClientViewAngle(bot, angles);

		if(cmd.buttons & BUTTON_WALKING)
			fMaxSpeed = 64.f;

		// Convert the move direction into forward and right moves to
		// take the bots orientation into account.
		AngleVectors(angles, forward, right, NULL);
		cmd.forwardmove = (char)(DotProduct(forward, _input->m_MoveDir) * fMaxSpeed);
		cmd.rightmove = (char)(DotProduct(right, _input->m_MoveDir) * fMaxSpeed);

		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_FWD))
			cmd.forwardmove = fMaxSpeed;
		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_BACK))
			cmd.forwardmove = -fMaxSpeed;
		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_RSTRAFE))
			cmd.rightmove = fMaxSpeed;
		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_LSTRAFE))
			cmd.rightmove = -fMaxSpeed;
		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_JUMP))
			cmd.upmove = fMaxSpeed;
		if(_input->m_ButtonFlags.CheckFlag(BOT_BUTTON_CROUCH))
			cmd.upmove = -fMaxSpeed;
	}
	trap_BotUserCommand(_client, &cmd);
}

static void pfnBotCommand(int _client, const char *_cmd)
{
	trap_EA_Command(_client, (char *)_cmd);
}

// client stuff
static obResult pfnGetClientPosition(int _client, float _pos[3])
{
	const gentity_t* bot = &g_entities[_client];
	if (bot->inuse)
	{
		_pos[0] = bot->r.currentOrigin[0];
		_pos[1] = bot->r.currentOrigin[1];
		_pos[2] = bot->r.currentOrigin[2];
		return Success;
	}
	return InvalidEntity;
}

static obResult pfnGetClientOrientation(int _client, float _fwd[3], float _right[3], float _up[3])
{
	const gentity_t *pEnt = &g_entities[_client];
	if (pEnt->inuse)
	{
		if(pEnt->client)
		{
			AngleVectors(pEnt->client->ps.viewangles, _fwd, _right, _up);
		}
		else
		{
			AngleVectors(pEnt->s.angles, _fwd, _right, _up);
		}
		return Success;
	}
	return InvalidEntity;
}

static const char *pfnGetClientName(int _client)
{
	if (g_entities[_client].inuse && g_entities[_client].client)
	{
		if(g_entities[_client].client->pers.netname[0])
			return g_entities[_client].client->pers.netname;
		else
		{
			static char	userinfo[MAX_INFO_STRING] = {0};
			trap_GetUserinfo( _client, userinfo, sizeof(userinfo) );
			return Info_ValueForKey( userinfo, "name" );
		}		
	}
	else
	{
		return _GetEntityName(&g_entities[_client]);
	}
	return NULL;
}

// entity stuff
static obResult pfnGetEntityOrientation(const GameEntity _ent, float _fwd[3], float _right[3], float _up[3])
{	
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		if(pEnt->client)
		{
			AngleVectors(pEnt->client->ps.viewangles, _fwd, _right, _up);
		} 
		else
		{
			AngleVectors(pEnt->s.angles, _fwd, _right, _up);
		}
		return Success;
	}
	return InvalidEntity;
}

static obResult pfnGetEntityVelocity(const GameEntity _ent, float _velocity[3])
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		if(pEnt->s.eType == ET_MOVER)
		{
			// Calculate the velocity ourselves. for some reason mover velocity
			// isn't in s.pos.trDelta
			const float fDeltaTime = 50.0f; // get this dynamically?
			_velocity[0] = (pEnt->r.currentOrigin[0] - pEnt->oldOrigin[0]) * fDeltaTime;
			_velocity[1] = (pEnt->r.currentOrigin[1] - pEnt->oldOrigin[1]) * fDeltaTime;
			_velocity[2] = (pEnt->r.currentOrigin[2] - pEnt->oldOrigin[2]) * fDeltaTime;
		}
		else
		{
			_velocity[0] = pEnt->s.pos.trDelta[0];
			_velocity[1] = pEnt->s.pos.trDelta[1];
			_velocity[2] = pEnt->s.pos.trDelta[2];
		}		
		return Success;
	}
	return InvalidEntity;
}

static obResult pfnGetEntityPosition(const GameEntity _ent, float _pos[3])
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		if(!pEnt->client)
		{
			// Need to do special case stuff for certain entities.
			if ((pEnt->s.eType == ET_CONSTRUCTIBLE) ||
				(pEnt->s.eType == ET_EXPLOSIVE) ||
				(pEnt->s.eType == ET_OID_TRIGGER) ||
				(pEnt->s.eType == ET_HEALER) ||
				(pEnt->s.eType == ET_SUPPLIER) ||
				/*(pEnt->s.eType == ET_MOVER) ||*/
				/*(pEnt->s.eType == ET_CORPSE) ||*/
				!Q_stricmp(pEnt->classname, "trigger_multiple") || 
				!Q_stricmp(pEnt->classname, "func_commandpoint") ||
				!Q_stricmp(pEnt->classname, "plat_trigger") ||
				!Q_stricmp(pEnt->classname, "door_trigger"))
			{
				// Center of bounding box, cuz currentOrigin isn't valid.
				/*if(!pEnt->r.linked && pEnt->parent && pEnt->parent->s.eType == ET_OID_TRIGGER)
					pEnt = pEnt->parent;*/

				_pos[0] = pEnt->r.currentOrigin[0] + ((pEnt->r.maxs[0] + pEnt->r.mins[0]) * 0.5f);
				_pos[1] = pEnt->r.currentOrigin[1] + ((pEnt->r.maxs[1] + pEnt->r.mins[1]) * 0.5f);
				_pos[2] = pEnt->r.currentOrigin[2] + ((pEnt->r.maxs[2] + pEnt->r.mins[2]) * 0.5f);
				return Success;
			}
		}

		// Clients and entities not caught above will return normal position.
		_pos[0] = pEnt->r.currentOrigin[0];
		_pos[1] = pEnt->r.currentOrigin[1];
		_pos[2] = pEnt->r.currentOrigin[2];

		return Success;
	}
	return InvalidEntity;
}

static obResult pfnGetEntityWorldAABB(const GameEntity _ent, AABB *_aabb)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		/*if(!pEnt->r.linked && pEnt->parent && pEnt->parent->s.eType == ET_OID_TRIGGER)
			pEnt = pEnt->parent;*/

		if(pEnt->s.eType == ET_CONSTRUCTIBLE)
		{
			gentity_t *pAxis = G_ConstructionForTeam(pEnt->parent, TEAM_AXIS);
			gentity_t *pAlly = G_ConstructionForTeam(pEnt->parent, TEAM_ALLIES);
			if(pAxis /*&& pAxis->entstate == STATE_DEFAULT*/)
				pEnt = pAxis;
			else if(pAlly /*&& pAlly->entstate == STATE_DEFAULT*/)
				pEnt = pAlly;
		}

		_aabb->m_Mins[0] = pEnt->r.absmin[0];
		_aabb->m_Mins[1] = pEnt->r.absmin[1];
		_aabb->m_Mins[2] = pEnt->r.absmin[2];
		_aabb->m_Maxs[0] = pEnt->r.absmax[0];
		_aabb->m_Maxs[1] = pEnt->r.absmax[1];
		_aabb->m_Maxs[2] = pEnt->r.absmax[2];
		return Success;
	}
	return InvalidEntity;
}

static int pfnGetEntityOwner(const GameEntity _ent)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		switch(pEnt->s.eType)
		{
		case ET_ITEM:
			{
				if(!Q_stricmp(pEnt->classname, "team_CTF_redflag") || !Q_stricmp(pEnt->classname, "team_CTF_blueflag"))
				{
					int i, iFlagEntNum = pEnt - g_entities;
					for(i = 0; i < MAX_CLIENTS; ++i)
					{
						if(g_entities[i].client && g_entities[i].client->flagParent == iFlagEntNum)
						{
							return i;
						}
					}                    
				}				
				break;
			}
		default:
			// -1 means theres no owner.
			return (pEnt->r.ownerNum < MAX_GENTITIES ? pEnt->r.ownerNum : -1);
		}		
	}
	return -1;
}

static int pfnGetEntityTeam(const GameEntity _ent)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	return pEnt && pEnt->inuse ? _GetEntityTeam(pEnt) : ET_TEAM_NONE;
}

static int pfnGetEntityClass(const GameEntity _ent)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	return pEnt && pEnt->inuse ? _GetEntityClass(pEnt) : ET_TEAM_NONE;
}

static obResult obGetEntityEyePosition(const GameEntity _ent, float _pos[3])
{
	if(pfnGetEntityPosition(_ent, _pos) == Success)
	{
		gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
		if(pEnt && pEnt->client)
		{
			_pos[2] += pEnt->client->ps.viewheight;
		}
		return Success;
	}
	return InvalidEntity;
}

static obResult obGetEntityBonePosition(const GameEntity _ent, int _boneid, float _pos[3])
{
	// ET doesnt really support bones
	return pfnGetEntityPosition(_ent, _pos);
}

static GameEntity pfnEntityFromID(const int _id)
{
	return (GameEntity)_GetEntityFromGameId(_id);
}

static int pfnIDFromEntity(const GameEntity _ent)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if(pEnt)
	{
		gentity_t *pStart = g_entities;
		int iIndex = pEnt - pStart;
		assert(iIndex >= 0);
		return (iIndex < MAX_GENTITIES) ? iIndex : -1;
	}
	return -1;
}

// weapon stuff
static obResult pfnBotGetCurrentAmmo(int _client, int _ammotype, int &_cur, int &_max) 
{
	gentity_t *bot = &g_entities[_client];
	int maxclip;

	if (bot->inuse)
	{
		_ammotype = _weaponBotToGame(_ammotype);
		_cur = bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)_ammotype)] +
			bot->client->ps.ammo[BG_FindAmmoForWeapon((weapon_t)_ammotype)];

		// sanity check for non-clipped weapons
		switch (_ammotype) 
		{
		case WP_MEDIC_ADRENALINE:
		case WP_AMMO:
		case WP_BINOCULARS:
		case WP_DYNAMITE:
		case WP_FLAMETHROWER:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_KNIFE:
		case WP_LANDMINE:
		case WP_MEDKIT:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_PANZERFAUST:
		case WP_PLIERS:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_SMOKE_MARKER:
		case WP_MEDIC_SYRINGE:
			maxclip = 0;
			break;
		default:
			maxclip = GetAmmoTableData(_ammotype)->maxclip;
		}
#ifdef NOQUARTER
		_max = maxclip + BG_MaxAmmoForWeapon((weapon_t)_ammotype, bot->client->sess.skillBits);
#else 
		_max = maxclip + BG_MaxAmmoForWeapon((weapon_t)_ammotype, bot->client->sess.skill);
#endif
		return Success;
	}
	return InvalidEntity;
}

static obResult pfnBotGetCurrentWeaponClip(int _client, FireMode _mode, int &_curclip, int &_maxclip) 
{
	gentity_t *bot = &g_entities[_client];

	if(bot->inuse && bot->client)
	{
		int iWeapon = bot->client->ps.weapon;

		_curclip = bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)iWeapon)];

		// sanity check for non-clipped weapons
		switch (iWeapon)
		{
		case WP_MEDIC_ADRENALINE:
		case WP_AMMO:
		case WP_BINOCULARS:
		case WP_DYNAMITE:
		case WP_FLAMETHROWER:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_KNIFE:
		case WP_LANDMINE:
		case WP_MEDKIT:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_PANZERFAUST:
		case WP_PLIERS:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_SMOKE_MARKER:
		case WP_MEDIC_SYRINGE:
			_maxclip = 0;
			break;
		default:
			_maxclip = GetAmmoTableData(iWeapon)->maxclip;
		}
		return Success;
	}
	return InvalidEntity;
}

// game stuff
static int pfnGetGameTime() 
{
	return level.time;
}

void Bot_Util_EntityCreated(GameEntity _ent)
{
	if(IsOmnibotLoaded())
	{
		gentity_t *pEnt = (gentity_t*)_ent;

		const char *pGoalName = _GetEntityName(pEnt);
		if(pEnt->inuse)
		{
			switch(pEnt->s.eType)
			{
			case ET_ITEM:
				{
					char buffer[256] = {};
					if(!Q_stricmp(pEnt->classname, "team_CTF_redflag"))
					{
						// allies flag
						if(pEnt->s.otherEntityNum != -1)
							pGoalName = _GetEntityName(&g_entities[pEnt->s.otherEntityNum]);
						sprintf(buffer, "%s_dropped", pGoalName ? pGoalName : "allies_flag");
						g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, GOAL_CTF_FLAG, 
							(1 << ET_TEAM_ALLIES), 
							buffer, NULL);

						g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, GOAL_CTF_RETURN_FLAG, 
							(1 << ET_TEAM_AXIS), 
							buffer, NULL);
					}
					else if(!Q_stricmp(pEnt->classname, "team_CTF_blueflag"))
					{
						// axis flag
						if(pEnt->s.otherEntityNum != -1)
							pGoalName = _GetEntityName(&g_entities[pEnt->s.otherEntityNum]);
						sprintf(buffer, "%s_dropped", pGoalName ? pGoalName : "axis_flag");
						g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, GOAL_CTF_FLAG, 
							(1 << ET_TEAM_AXIS), 
							buffer, NULL);

						g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, GOAL_CTF_RETURN_FLAG, 
							(1 << ET_TEAM_ALLIES), 
							buffer, NULL);
					}
					break;
				}
			default:
				break;
			}
		}	
	}
}

static void pfnGetGoals() 
{
	for (int i = MAX_CLIENTS; i < level.num_entities; i++) 
	{
		gentity_t *e = &g_entities[i];
		const char *pGoalName = 0;

		if (!e->inuse)
			continue;

		pGoalName = _GetEntityName(e);
		switch(e->s.eType)
		{
		case ET_ITEM:
			{
				if(!Q_stricmp(e->classname, "team_CTF_redflag"))
				{
					// allies flag
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, GOAL_CTF_FLAG, (1 << ET_TEAM_ALLIES), pGoalName, NULL);
				}
				else if(!Q_stricmp(e->classname, "team_CTF_blueflag"))
				{
					// axis flag
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, GOAL_CTF_FLAG, (1 << ET_TEAM_AXIS), pGoalName, NULL);
				}
				break;
			}
		case ET_OID_TRIGGER:
			{
				// detect constructibles
				char *pTmp = 0;
				gentity_t *eAxis = G_ConstructionForTeam(e, TEAM_AXIS);
				gentity_t *eAlly = G_ConstructionForTeam(e, TEAM_ALLIES);
				const char *pAllyGoalName = 0, *pAxisGoalName = 0;

				//////////////////////////////////////////////////////////////////////////
				if(eAxis)
				{
					/*if(e->target_ent)
						pAxisGoalName = _GetEntityName(e->target_ent);*/
					if(!pAxisGoalName)
						pAxisGoalName = _GetEntityName(eAxis);
					if(!pAxisGoalName)
						pAxisGoalName = _GetEntityName(e);
					if(!pAxisGoalName && e->parent)
						pAxisGoalName = _GetEntityName(e->parent);	
					// detect fakeobj's
					if ( strncmp( eAxis->targetname, "fakeobj", 7 ) == 0 )
						continue;
				}
				//////////////////////////////////////////////////////////////////////////
				if(eAlly)
				{
					/*if(e->target_ent)
						pAllyGoalName = _GetEntityName(e->target_ent);*/
					if(!pAllyGoalName)
						pAllyGoalName = _GetEntityName(eAlly);
					if(!pAllyGoalName)
						pAllyGoalName = _GetEntityName(e);
					if(!pAllyGoalName && e->parent)
						pAllyGoalName = _GetEntityName(e->parent);
					// detect fakeobj's
					if ( strncmp( eAlly->targetname, "fakeobj", 7 ) == 0 )
						continue;
				}
				
				//////////////////////////////////////////////////////////////////////////
				if(pAxisGoalName && pAllyGoalName)
				{
					if(!Q_stricmp(pAxisGoalName, pAllyGoalName))
					{
						pfnPrintMessage("Goals Have Same Name!!");
					}
				}
				//////////////////////////////////////////////////////////////////////////
				if(eAxis)
				{
					bool bMover = pAxisGoalName ? strstr(pAxisGoalName, "_construct")!=0 : false;
					obUserData botUserData = obUserData(bMover ? 1 : 0);

					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_CONSTRUCTION, 
						(1 << ET_TEAM_AXIS), pAxisGoalName, &botUserData);
				}

				if(eAlly)
				{
					bool bMover = pAllyGoalName ? strstr(pAllyGoalName, "_construct")!=0 : false;
					obUserData botUserData = obUserData(bMover ? 1 : 0);

					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_CONSTRUCTION, 
						(1 << ET_TEAM_ALLIES), pAllyGoalName, &botUserData);
				}
				//////////////////////////////////////////////////////////////////////////

				// register constructions as dyno targets, too
				// check if this is an enemy construction and check spawnflags if this is a dynamite target
				if (!e->target_ent)
					continue;

				// If we didn't find a name from the entity, try to get one from its target_ent
				/*pAxisGoalName = pAxisGoalName ? pAxisGoalName : _GetEntityName(e->target_ent);
				pAllyGoalName = pAllyGoalName ? pAllyGoalName : _GetEntityName(e->target_ent);*/
				pAxisGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : pAxisGoalName;
				pAllyGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : pAllyGoalName;

				//////////////////////////////////////////////////////////////////////////
				// Is this a movable?, skip it if so.
				if(e->target_ent->targetname && 
					(pTmp = strstr(e->target_ent->targetname, "_construct")) != NULL)
				{
					char strName[256];
					Q_strncpyz(strName, e->target_ent->targetname, 256);
					strName[pTmp-e->target_ent->targetname] = 0;
					gentity_t *pMover = G_FindByTargetname(NULL, strName);
					if(pMover && 
						pMover->s.eType == ET_MOVER && 
						!Q_stricmp(pMover->classname, "script_mover"))
						continue;
				}
				//////////////////////////////////////////////////////////////////////////

				if (e->target_ent->s.eType == ET_CONSTRUCTIBLE)
				{
					obUserData bud;
					bud.DataType = obUserData::dtInt;
					bud.udata.m_Int = 0;
					switch(e->target_ent->constructibleStats.weaponclass)
					{
					case 1: // Satchelable, it should fall through and also register as dyno.
						bud.udata.m_Int |= XPLO_TYPE_SATCHEL;
					case 2: // Dynamite
						bud.udata.m_Int |= XPLO_TYPE_DYNAMITE;
						break;
					default:
						// < 0 is apparently satchel only.
						if(e->target_ent->constructibleStats.weaponclass < 0)
							bud.udata.m_Int |= XPLO_TYPE_SATCHEL;
						break;
					}

					// NOTE: sometimes goals get registered that don't have 1 or 2 weaponclass
					// is this correct? for now let's not register them and see what happens.
					if(bud.udata.m_Int != 0)
					{
						// If there's a goal for each team, and it has the same name, just register 1 goal
						if(eAxis && eAlly && !Q_stricmp(pAxisGoalName, pAllyGoalName))
						{
							g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_DYNTARGET, 
								(1 << ET_TEAM_ALLIES) | (1 << ET_TEAM_AXIS), pAxisGoalName, &bud);
						}
						else
						{
							if(eAxis)
								g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_DYNTARGET, 
								(1 << ET_TEAM_ALLIES), pAxisGoalName, &bud);

							if(eAlly)
								g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_DYNTARGET, 
								(1 << ET_TEAM_AXIS), pAllyGoalName, &bud);
						}
					}					
				}
				// check for objective targets
				else if (e->target_ent->s.eType == ET_EXPLOSIVE)
				{
					switch(e->target_ent->constructibleStats.weaponclass) 
					{
					case 1: // SATCHEL
						// note ALLIED/AXIS stuff is reversed here (ET is strange sometimes)
						if (e->spawnflags & ALLIED_OBJECTIVE)
							g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_SATCHELTARGET, 
							(1 << ET_TEAM_AXIS), pAxisGoalName, NULL);
						if (e->spawnflags & AXIS_OBJECTIVE)
							g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_SATCHELTARGET, 
							(1 << ET_TEAM_ALLIES), pAllyGoalName, NULL);
						// fall through to register dyna too.
					case 2: // DYNAMITE
						// note ALLIED/AXIS stuff is reversed here (ET is strange sometimes)
						if (e->spawnflags & ALLIED_OBJECTIVE)
							g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_DYNTARGET, 
							(1 << ET_TEAM_AXIS), pAxisGoalName, NULL);
						if (e->spawnflags & AXIS_OBJECTIVE)
							g_BotFunctions.pfnBotAddGoal((GameEntity)e->target_ent, ET_GOAL_DYNTARGET, 
							(1 << ET_TEAM_ALLIES), pAllyGoalName, NULL);
						break;
					}				
				}
				break;
			}
		case ET_MG42_BARREL:
			{
				int team = 0;
				team |= (1 << ET_TEAM_ALLIES);
				team |= (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e);
				g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_MG42MOUNT, team, pGoalName, NULL);
				break;
			}
		case ET_MOVER:
			{
				int team = 0;
				if (e->spawnflags & 32)
                    team = (1 << ET_TEAM_ALLIES);
				else if (e->spawnflags & 64)
					team = (1 << ET_TEAM_AXIS);
				else
				{
					team |= (1 << ET_TEAM_ALLIES);
					team |= (1 << ET_TEAM_AXIS);
				}
				pGoalName = _GetEntityName(e);
				if(pGoalName && !Q_stricmp(e->classname, "script_mover"))
				{
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_MOVER, team, pGoalName, NULL);
				}                
				break;
			}
		case ET_CONSTRUCTIBLE:
			{
				if(e->classname)
				{
					//pfnPrintMessage("constructible");
				}
				break;
			}
		//case ET_CABINET_H:
		//	{
		//		int team = 0;
		//		team |= (1 << ET_TEAM_ALLIES);
		//		team |= (1 << ET_TEAM_AXIS);
		//		pGoalName = _GetEntityName(e);
		//		//g_BotFunctions.pfnBotAddGoal((GameEntity)e, GOAL_HEALTH, team, pGoalName, NULL);
		//		break;
		//	}
		//case ET_CABINET_A:
		//	{
		//		int team = 0;
		//		team |= (1 << ET_TEAM_ALLIES);
		//		team |= (1 << ET_TEAM_AXIS);
		//		pGoalName = _GetEntityName(e);
		//		//g_BotFunctions.pfnBotAddGoal((GameEntity)e, GOAL_AMMO, team, pGoalName, NULL);
		//		break;
		//	}
		case ET_HEALER:
			{
				//obUserData bud(e->damage); // heal rate
				int team = (1 << ET_TEAM_ALLIES) | (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : _GetEntityName(e);
				g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_HEALTH_CAB, team, pGoalName, NULL);
				break;
			}
		case ET_SUPPLIER:
			{
				//obUserData bud(e->damage); // heal rate
				int team = (1 << ET_TEAM_ALLIES) | (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : _GetEntityName(e);
				g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_AMMO_CAB, team, pGoalName, NULL);
				break;
			}
		case ET_TRAP:
			{
				if (!Q_stricmp( e->classname, "team_WOLF_checkpoint" )) 
				{
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_CHECKPOINT, 0, pGoalName, NULL);
				}
				break;
			}
		}
	}
}

static int pfnGetCurNumPlayers() 
{ 
	//return level.numNonSpectatorClients;
	return level.numConnectedClients; 
}

static int pfnGetMaxNumPlayers() 
{ 
	return level.maxclients; 
}

static obResult pfnInterfaceSendMessage(const MessageHelper &_data, const GameEntity _ent)
{
#define GETMSG(msgtype) msgtype *pMsg = 0; _data.Get2(pMsg);

	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);

	switch(_data.GetMessageId())
	{
		// general messages
	case GEN_MSG_ISALIVE:
		{
			GETMSG(Msg_IsAlive);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client &&pEnt->health > 0 && 
					pEnt->client->ps.pm_type == PM_NORMAL)
				{
					pMsg->m_IsAlive = True;
				}
			}
			break;
		}
	case GEN_MSG_ISRELOADING:
		{
			GETMSG(Msg_Reloading);
			if(pMsg)
			{
				if((pEnt && pEnt->inuse && pEnt->client && pEnt->client->ps.weaponstate >= WEAPON_RAISING) &&
					(pEnt->client->ps.weaponstate <= WEAPON_RELOADING))
				{
					pMsg->m_Reloading = True;
				}
			}				
			break;
		}
	case GEN_MSG_ISREADYTOFIRE:
		{
			GETMSG(Msg_ReadyToFire);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
				{
					if(pEnt->client->ps.weaponstate == WEAPON_READY || pEnt->client->ps.weaponstate == WEAPON_FIRING)
					{
						pMsg->m_Ready = True;
					}
				}
			}				
			break;
		}
	case GEN_MSG_GETEQUIPPEDWEAPON:
		{
			GETMSG(WeaponStatus);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
					pMsg->m_WeaponId = Bot_WeaponGameToBot(pEnt->client->ps.weapon);
				else
					pMsg->m_WeaponId = 0;
			}				
			break;
		}
	case GEN_MSG_GETMOUNTEDWEAPON:
		{
			GETMSG(WeaponStatus);
			if (pMsg && pEnt && pEnt->inuse && pEnt->client)
				pMsg->m_WeaponId = BG_PlayerMounted(pEnt->s.eFlags) ? ET_WP_MOUNTABLE_MG42 : ET_WP_NONE;
			break;
		}
	case GEN_MSG_GETWEAPONLIMITS:
		{
			GETMSG(WeaponLimits);
			if(pMsg)
			{
				
				if (pEnt && pEnt->inuse && pEnt->client && 
					pMsg->m_WeaponId == ET_WP_MOUNTABLE_MG42 &&
					BG_PlayerMounted(pEnt->client->ps.eFlags))
				{
					pMsg->m_Limited = True;
					AngleVectors(pEnt->client->pmext.centerangles, pMsg->m_CenterFacing, NULL, NULL);
					if(pEnt->client->ps.eFlags & EF_MOUNTEDTANK)
					{
						// seems tanks have complete horizonal movement, and fixed vertical							
						pMsg->m_MinHorizontalArc = -360;
						pMsg->m_MaxHorizontalArc = -360;
						pMsg->m_MinVerticalArc = 14;
						pMsg->m_MaxVerticalArc = 50;
					}
					else
					{	
						pMsg->m_MinHorizontalArc = -pEnt->client->pmext.harc;
						pMsg->m_MaxHorizontalArc = pEnt->client->pmext.harc;
						pMsg->m_MinVerticalArc = -pEnt->client->pmext.varc;
						pMsg->m_MaxVerticalArc = pEnt->client->pmext.varc;
					}
				}
				else
					pMsg->m_Limited = False;
			}
			//////////////////////////////////////////////////////////////////////////
			break;
		}
	case GEN_MSG_GETHEALTHARMOR:
		{
			GETMSG(Msg_PlayerHealthArmor);
			if(pMsg)
			{
				// No Armor in ET				
				pMsg->m_CurrentArmor = pMsg->m_MaxArmor = 0;
				if(pEnt)
				{
					if (pEnt->client)
					{						
						pMsg->m_CurrentHealth = pEnt->client->ps.stats[STAT_HEALTH];
						pMsg->m_MaxHealth = pEnt->client->ps.stats[STAT_MAX_HEALTH];
					}
					else if(pEnt->s.eType == ET_CONSTRUCTIBLE)
					{
						pMsg->m_CurrentHealth = pEnt->takedamage ? pEnt->constructibleStats.health : 0;
						pMsg->m_MaxHealth = 0;
					}
					else
					{
						pMsg->m_CurrentHealth = pEnt->takedamage ? pEnt->health : 0;
						pMsg->m_MaxHealth = 0;
					}
				}
			}				
			break;
		}
	case GEN_MSG_GETMAXSPEED:
		{
			GETMSG(Msg_PlayerMaxSpeed);
			if(pMsg && pEnt)
			{
				pMsg->m_MaxSpeed = (float)g_movespeed.integer;
			}
			break;
		}
	case GEN_MSG_ISALLIED:
		{
			GETMSG(Msg_IsAllied);
			if(pMsg)
			{
				gentity_t *pEntOther = _GetEntityFromGameEntity(pMsg->m_TargetEntity);
				if(pEntOther && pEnt)
				{						
					if(pEntOther->s.number == ENTITYNUM_WORLD)
						pMsg->m_IsAllied = True;
					else
					{
						pMsg->m_IsAllied = _GetEntityTeam(pEnt) == _GetEntityTeam(pEntOther) ? True : False;
					}
				}
			}				
			break;
		}
	case GEN_MSG_ISOUTSIDE:
		{
			GETMSG(Msg_IsOutside);
			if(pMsg)
			{
				trace_t tr;
				vec3_t end;
				end[0] = pMsg->m_Position[0];
				end[1] = pMsg->m_Position[1];
				end[2] = pMsg->m_Position[2] + 4096;
				trap_Trace(&tr, pMsg->m_Position, NULL, NULL, end, -1, MASK_SOLID);

				if((tr.fraction < 1.0) && !(tr.surfaceFlags & SURF_NOIMPACT))
					pMsg->m_IsOutside = False;
				else
					pMsg->m_IsOutside = True;
			}
			break;
		}
	case GEN_MSG_CHANGENAME:
		{
			GETMSG(Msg_ChangeName);
			if(pMsg && pEnt && pEnt->client)
			{
				char userinfo[MAX_INFO_STRING];
				int iClientNum = pEnt-g_entities;
				trap_GetUserinfo(iClientNum, userinfo, MAX_INFO_STRING);
				Info_SetValueForKey(userinfo, "name", pMsg->m_NewName);
				trap_SetUserinfo(iClientNum, userinfo);
				ClientUserinfoChanged(iClientNum);
			}
			break;
		}
	case GEN_MSG_ENTITYKILL:
		{
			GETMSG(Msg_KillEntity);
			if(pMsg && pMsg->m_WhoToKill && g_cheats.integer)
			{
				gentity_t *pWho = _GetEntityFromGameEntity(pMsg->m_WhoToKill);
				if(pWho)
					G_Damage( pWho, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG );
			}
			break;
		}
	case GEN_MSG_SERVERCOMMAND:
		{
			GETMSG(Msg_ServerCommand);
			if(pMsg && pMsg->m_Command[0] && g_cheats.integer)
			{
				trap_SendConsoleCommand(EXEC_NOW, pMsg->m_Command);
			}
			break;
		}
	case GEN_MSG_GAMESTATE:
		{
			GETMSG(Msg_GameState);
			if(pMsg)
			{
				int iTimeLimit = (int)(g_timelimit.value * 60000);
				int iMatchTime = level.timeCurrent - level.startTime;
				pMsg->m_TimeLeft = (iTimeLimit - iMatchTime) / 1000.f;
				switch(g_gamestate.integer)
				{
				case GS_PLAYING:
					pMsg->m_GameState = GAME_STATE_PLAYING;
					break;					
				case GS_WARMUP_COUNTDOWN:
					pMsg->m_GameState = GAME_STATE_WARMUP_COUNTDOWN;
					break;
				case GS_WARMUP:
					pMsg->m_GameState = GAME_STATE_WARMUP;
					break;
				case GS_INTERMISSION:
					pMsg->m_GameState = GAME_STATE_INTERMISSION;
					break;
				case GS_WAITING_FOR_PLAYERS:
					pMsg->m_GameState = GAME_STATE_WAITINGFORPLAYERS;
					break;
				default:
					assert(0 && "Invalid Game State");
				}
			}				
			break;
		}
	case GEN_MSG_ENTITYSTAT:
		{
			GETMSG(Msg_EntityStat);
			if(pMsg)
			{
				if(pEnt && pEnt->client && !strcmp(pMsg->m_StatName, "kills"))
					pMsg->m_Result = obUserData(pEnt->client->sess.kills);
				else if(pEnt && pEnt->client && !strcmp(pMsg->m_StatName, "deaths"))
					pMsg->m_Result = obUserData(pEnt->client->sess.deaths);
				//else if(pEnt && pEnt->client && !Q_strcmp(pMsg->m_StatName, "score"))
				//	pMsg->m_Result = obUserData(0); // TODO:
			}
			break;
		}
	case GEN_MSG_TEAMSTAT:
		{
			GETMSG(Msg_TeamStat);
			if(pMsg)
			{
				/*if(!strcmp(pMsg->m_StatName, "score"))
					pMsg->m_Result = obUserData(0);
				else if(!strcmp(pMsg->m_StatName, "deaths"))
					pMsg->m_Result = obUserData(0);*/
			}
			break;
		}
	case GEN_MSG_WPCHARGED:
		{
			GETMSG(WeaponCharged);
			if (pMsg && pEnt && pEnt->inuse && pEnt->client)
			{
				pMsg->m_IsCharged = 
					(weaponCharged(&pEnt->client->ps,pEnt->client->sess.sessionTeam,
					_weaponBotToGame(pMsg->m_Weapon), pEnt->client->sess.skill) == qtrue) ? True : False;
			}
			break;
		}
	case GEN_MSG_WPHEATLEVEL:
		{
			GETMSG(WeaponHeatLevel);
			if (pMsg && pEnt && pEnt->inuse && pEnt->client)
			{
				pMsg->m_CurrentHeat = pEnt->client->ps.weapHeat[WP_DUMMY_MG42];
				pMsg->m_MaxHeat = MAX_MG42_HEAT;
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////
	case ET_MSG_GOTOLIMBO:
		{
			GETMSG(ET_GoLimbo);
			if(pMsg)
			{
				int limbo_health = GIB_HEALTH; 
				if (pEnt && pEnt->inuse && pEnt->client && pEnt->health > limbo_health && 
					pEnt->client->ps.pm_type == PM_DEAD && !(pEnt->client->ps.pm_flags & PMF_LIMBO))
				{
					limbo(pEnt, qtrue);
					pMsg->m_GoLimbo = True;
				}
				else
				{
					pMsg->m_GoLimbo = False;
				}
			}				
			break;
		}		
	case ET_MSG_ISMEDICNEAR:
		{
			GETMSG(ET_MedicNear);
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client &&
					pEnt->client->ps.pm_type == PM_DEAD && !(pEnt->client->ps.pm_flags & PMF_LIMBO))
				{						
					pMsg->m_MedicNear = (pEnt->client->ps.viewlocked == 7) ? True : False;
				}
				else 
				{
					pMsg->m_MedicNear = Invalid;
				}
			}				
			break;
		}
	case ET_MSG_ISWAITINGFORMEDIC:
		{
			GETMSG(ET_WaitingForMedic);
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client &&
					(pEnt->health <= 0 || pEnt->client->ps.pm_type == PM_DEAD) &&
					!(pEnt->client->ps.pm_flags & PMF_LIMBO))
				{
					pMsg->m_WaitingForMedic = True;
				}
				else if (pEnt && pEnt->inuse && pEnt->client &&
					pEnt->client->ps.pm_type == PM_NORMAL)
				{
					pMsg->m_WaitingForMedic = False;
				}
				else
				{
					pMsg->m_WaitingForMedic = Invalid;
				}
			}				
			break;
		}
	case ET_MSG_REINFORCETIME:
		{
			GETMSG(ET_ReinforceTime);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
				{
					if(pEnt->client->sess.sessionTeam == TEAM_AXIS)
					{						
						pMsg->m_ReinforceTime = g_redlimbotime.integer -
							((level.dwRedReinfOffset + level.timeCurrent - level.startTime) %
							g_redlimbotime.integer);
					}
					else if(pEnt->client->sess.sessionTeam == TEAM_ALLIES)
					{
						pMsg->m_ReinforceTime = g_bluelimbotime.integer -
							((level.dwBlueReinfOffset + level.timeCurrent - level.startTime) %
							g_bluelimbotime.integer);
					}
				}					
			}				
			break;
		}
	case ET_MSG_GETGUNHEALTH:
		{
			GETMSG(ET_MG42Health);
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_MG42Entity);
				if (pGunEntity && pGunEntity->inuse && pGunEntity->r.linked && 
					pGunEntity->entstate == STATE_DEFAULT)
				{						
					if(pGunEntity->mg42BaseEnt != -1)
						pMsg->m_Health = g_entities[pGunEntity->mg42BaseEnt].health;
					else
						pMsg->m_Health = pGunEntity->health;
				}
				else
				{
					pMsg->m_Health = -1;
				}
			}				
			break;
		}
	case ET_MSG_GETGUNHEAT:
		{
			GETMSG(ET_WeaponHeatLevel);
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_Entity);
				if(pEnt && pEnt->client && pGunEntity)
				{
					pMsg->m_Current = pEnt->client->ps.weapHeat[WP_DUMMY_MG42];
					pMsg->m_Max = MAX_MG42_HEAT;
				}
				else
				{
					pMsg->m_Current = -1;
					pMsg->m_Max = -1;
				}
			}
			break;
		}
	case ET_MSG_ISGUNMOUNTED:
		{
			GETMSG(ET_MG42MountedPlayer);
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_MG42Entity);
				if(pGunEntity && pGunEntity->inuse && (pGunEntity->r.ownerNum < level.maxclients))
					pMsg->m_MountedEntity = &g_entities[pGunEntity->r.ownerNum];
				else
					pMsg->m_MountedEntity = 0;
			}				
			break;
		}
	case ET_MSG_ISGUNREPAIRABLE:
		{
			GETMSG(ET_MG42MountedRepairable);
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_MG42Entity);
				if(pEnt && pGunEntity && pGunEntity->inuse)
					pMsg->m_Repairable = G_EmplacedGunIsRepairable(pGunEntity, pEnt) == qtrue ? True : False;
				else
					pMsg->m_Repairable = False;
			}
			break;
		}
	case ET_MSG_MOUNTEDMG42INFO:
		{
			GETMSG(ET_MG42Info);
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client && BG_PlayerMounted(pEnt->client->ps.eFlags))
				{
					AngleVectors(pEnt->client->pmext.centerangles, pMsg->m_CenterFacing, NULL, NULL);
					//AngleVectors(pEnt->client->pmext.mountedWeaponAngles, pMsg->, NULL, NULL);
					if(pEnt->client->ps.eFlags & EF_MOUNTEDTANK)
					{
						// seems tanks have complete horizonal movement, and fixed vertical							
						pMsg->m_MinHorizontalArc = -360;
						pMsg->m_MaxHorizontalArc = -360;
						pMsg->m_MinVerticalArc = 14;
						pMsg->m_MaxVerticalArc = 50;
					}
					else
					{	
						pMsg->m_MinHorizontalArc = -pEnt->client->pmext.harc;
						pMsg->m_MaxHorizontalArc = pEnt->client->pmext.harc;
						pMsg->m_MinVerticalArc = -pEnt->client->pmext.varc;
						pMsg->m_MaxVerticalArc = pEnt->client->pmext.varc;
					}
				}
			}
			break;
		}	
	case ET_MSG_WPOVERHEATED:
		{
			GETMSG(ET_WeaponOverheated);
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client)
				{
					if(pMsg->m_Weapon != ET_WP_MOUNTABLE_MG42)
					{
						int iCurHeat = pEnt->client->ps.weapHeat[_weaponBotToGame(pMsg->m_Weapon)];
						int iMaxHeat = GetAmmoTableData(_weaponBotToGame(pMsg->m_Weapon))->maxHeat;
						pMsg->m_IsOverheated = iMaxHeat ? ((iCurHeat >= iMaxHeat) ? True : False) : False;
					}
					else
					{
						pMsg->m_IsOverheated = (pEnt->s.eFlags & EF_OVERHEATING) ? True : False;
					}
				}
			}				
			break;
		}
	case ET_MSG_PICKWEAPON:
	case ET_MSG_PICKWEAPON2:
		{
			GETMSG(ET_SelectWeapon);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
				{
					if(pMsg->m_Selection == ET_WP_GPG40)
						pMsg->m_Selection = ET_WP_KAR98;
					else if(pMsg->m_Selection == ET_WP_M7)
						pMsg->m_Selection = ET_WP_CARBINE;

					if(_data.GetMessageId() == ET_MSG_PICKWEAPON2)
					{
						pEnt->client->sess.playerWeapon2 = _weaponBotToGame(pMsg->m_Selection);
						pEnt->client->sess.latchPlayerWeapon2 = _weaponBotToGame(pMsg->m_Selection);
					}
					else
					{
						pEnt->client->sess.playerWeapon = _weaponBotToGame(pMsg->m_Selection);
						pEnt->client->sess.latchPlayerWeapon = _weaponBotToGame(pMsg->m_Selection);
					}
					pMsg->m_Good = True;
				}
				else
				{
					pMsg->m_Good = False;
				}
			}				
			break;
		}
	case ET_MSG_GETHINT:
		{
			GETMSG(ET_CursorHint);
			if(pMsg)
			{
				if(pEnt && pEnt->client)
				{
					pMsg->m_Type = Bot_HintGameToBot(pEnt);
					pMsg->m_Value = pEnt->client->ps.serverCursorHintVal;
				}
			}
			break;
		}
	case ET_MSG_CHECKPOINTTEAM:
		{
			GETMSG(ET_CheckpointTeam);
			if(pMsg && pEnt && pEnt->s.eType == ET_TRAP)
				pMsg->m_OwningTeam = Bot_TeamGameToBot(pEnt->count);
			else
				return InvalidEntity;
			break;
		}
	case ET_MSG_CHANGESPAWNPOINT:
		{
			GETMSG(ET_SpawnPoint);
			if(pMsg)
			{
				if(pEnt && pEnt->client)
				{
					SetPlayerSpawn( pEnt, pMsg->m_SpawnPoint, qtrue);
				}
			}
			break;
		}
	case ET_MSG_GHASFLAG:
		{
			GETMSG(ET_HasFlag);
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client && (pEnt->health >= 0))
				{
					if(pEnt->client->ps.powerups[PW_REDFLAG] || pEnt->client->ps.powerups[PW_BLUEFLAG]) 
					{
						pMsg->m_HasFlag = True;
					} 
				}
			}			
			break;
		}
	case ET_MSG_GCONSTRUCTABLE:
		{
			GETMSG(ET_ConstructionState);
			if(pMsg)
			{
				gentity_t *pConstructable = _GetEntityFromGameEntity(pMsg->m_Constructable);				
				if (pEnt && pEnt->inuse && pConstructable && pConstructable->inuse) 
				{
					if (G_IsConstructible(pEnt->client->sess.sessionTeam, pConstructable))
						pMsg->m_State = CONST_UNBUILT;
					else if ((pConstructable = G_ConstructionForTeam(pConstructable, pEnt->client->sess.sessionTeam)) && 
						G_ConstructionIsFullyBuilt(pConstructable))
						pMsg->m_State = CONST_BUILT;
					else
						pMsg->m_State = CONST_INVALID;
				} 
				else 
				{
					pMsg->m_State = CONST_INVALID;
				}
			}
			break;
		}
	case ET_MSG_GDYNDESTROYABLE:
		{
			GETMSG(ET_Destroyable);
			if(pMsg)
			{
				gentity_t *pDestroyable = _GetEntityFromGameEntity(pMsg->m_Entity);
				if (pEnt && pEnt->inuse && pDestroyable && pDestroyable->inuse)
				{
					if(pDestroyable->s.eType == ET_OID_TRIGGER)
						pDestroyable = pDestroyable->target_ent;

					if (pDestroyable->s.eType == ET_CONSTRUCTIBLE)
					{	
						//qboolean d = G_ConstructionIsDestroyable(pDestroyable);

						if (pDestroyable->spawnflags & CONSTRUCTIBLE_INVULNERABLE)
							pMsg->m_State = CONST_INVALID;
						else if (G_ConstructionIsPartlyBuilt(pDestroyable) && pDestroyable->s.teamNum != pEnt->client->sess.sessionTeam)
							pMsg->m_State = CONST_DESTROYABLE;
						else if(pDestroyable->parent && pDestroyable->parent->s.eType == ET_OID_TRIGGER)
						{
							gentity_t *pCurrent = pDestroyable->parent->chain;
							while(pCurrent)
							{
								if (G_ConstructionIsPartlyBuilt(pCurrent) && 
									pCurrent->s.teamNum != pEnt->client->sess.sessionTeam)
								{
									pMsg->m_State = CONST_DESTROYABLE;
									break;
								}
								pCurrent = pCurrent->chain;
								if(pCurrent == pDestroyable->parent->chain)
									break;
							}
						}
						else
							pMsg->m_State = CONST_NOTDESTROYABLE;
					}
					else if (pDestroyable->s.eType == ET_EXPLOSIVE && pDestroyable->parent && pDestroyable->parent->s.eType == ET_OID_TRIGGER &&
						(((pDestroyable->parent->spawnflags & ALLIED_OBJECTIVE) && pEnt->client->sess.sessionTeam == TEAM_AXIS) ||
						((pDestroyable->parent->spawnflags & AXIS_OBJECTIVE) && pEnt->client->sess.sessionTeam == TEAM_ALLIES)))
					{
						if (pDestroyable->health > 0)
							pMsg->m_State = CONST_DESTROYABLE;
						else
							pMsg->m_State = CONST_NOTDESTROYABLE;
					}
				}
			}
			break;
		}
	case ET_MSG_GEXPLOSIVESTATE:
		{
			GETMSG(ET_ExplosiveState);
			if(pMsg)
			{
				gentity_t *pExplo = _GetEntityFromGameEntity(pMsg->m_Explosive);
				if(pExplo && pExplo->inuse)
				{
					if(pExplo->s.eType == ET_MISSILE)
					{
						switch(pExplo->s.weapon)
						{
						case WP_DYNAMITE:
						case WP_LANDMINE:								
							pMsg->m_State = (pExplo->s.teamNum < 4) ? XPLO_ARMED : XPLO_UNARMED;
							break;
						case WP_SATCHEL:
							pMsg->m_State = (pExplo->health >= 250) ? XPLO_UNARMED : XPLO_ARMED;
							break;
						}
					}					
				}
			}
			break;
		}
	case ET_MSG_GCANBEGRABBED:
		{
			GETMSG(ET_CanBeGrabbed);
			if(pMsg)
			{
				gentity_t *pFlagEnt = _GetEntityFromGameEntity(pMsg->m_Entity);
				if(pEnt && pEnt->client && pFlagEnt)
				{
					// DUPLICATE ERROR CHECK, so BG_CanItemBeGrabbed doesn't screw up.
					if ( pFlagEnt->s.modelindex < 1 || pFlagEnt->s.modelindex >= bg_numItems ) 
					{
						//Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
						pMsg->m_CanBeGrabbed = Invalid;
					}
					else
					{
						pMsg->m_CanBeGrabbed = 
							BG_CanItemBeGrabbed(&pFlagEnt->s, &pEnt->client->ps, 
							pEnt->client->sess.skill, pEnt->client->sess.sessionTeam) ? True : False;
					}
				}
			}
			break;
		}
	case ET_MSG_GNUMTEAMMINES:
		{
			GETMSG(ET_TeamMines);
			if(pMsg)
			{
				if(pEnt && pEnt->client)
				{
					pMsg->m_Current = G_CountTeamLandmines(pEnt->client->sess.sessionTeam);
					pMsg->m_Max = MAX_TEAM_LANDMINES;
				}
			}
			break;
		}
	case ET_MSG_CABINETDATA:
		{
			GETMSG(ET_CabinetData);
			if(pMsg)
			{
				if(pEnt && (pEnt->s.eType == ET_HEALER || pEnt->s.eType == ET_SUPPLIER))
				{
					pMsg->m_Rate = pEnt->damage;
					pMsg->m_CurrentAmount = pEnt->health;
					pMsg->m_MaxAmount = pEnt->count;
				}
				else
				{
					return InvalidEntity;
				}
			}
			break;
		}
	case ET_MSG_SKILLLEVEL:
		{
			if(pEnt && pEnt->client)
			{
				GETMSG(ET_PlayerSkills);
				pMsg->m_Skill[ET_SKILL_BATTLE_SENSE] = pEnt->client->sess.skill[SK_BATTLE_SENSE];
				pMsg->m_Skill[ET_SKILL_ENGINEERING] = pEnt->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION];
				pMsg->m_Skill[ET_SKILL_FIRST_AID] = pEnt->client->sess.skill[SK_FIRST_AID];
				pMsg->m_Skill[ET_SKILL_SIGNALS] = pEnt->client->sess.skill[SK_SIGNALS];
				pMsg->m_Skill[ET_SKILL_LIGHT_WEAPONS] = pEnt->client->sess.skill[SK_LIGHT_WEAPONS];
				pMsg->m_Skill[ET_SKILL_HEAVY_WEAPONS] = pEnt->client->sess.skill[SK_HEAVY_WEAPONS];
				pMsg->m_Skill[ET_SKILL_COVERTOPS] = pEnt->client->sess.skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS];
			}
			else
			{
				return InvalidEntity;
			}
			break;
		}
	default:
		{
			return UnknownMessageType;
		}
	}
	return Success;
}

static obResult obGetEntityCategory(const GameEntity _ent, BitFlag64 &_category)
{
	obResult res = Success;
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	switch(pEnt->s.eType)
	{
	case ET_GENERAL:
		{
			if(!Q_stricmp(pEnt->classname, "func_invisible_user")) 
			{
				// The damage flags tells us the type.
				switch(pEnt->s.dmgFlags)
				{
				case HINT_BUTTON:
					_category.SetFlag(ENT_CAT_TRIGGER);
					_category.SetFlag(ENT_CAT_STATIC);
					break;
				}
			}
			else if(!Q_stricmp(pEnt->classname, "func_button")) 
			{
				_category.SetFlag(ENT_CAT_TRIGGER);
				_category.SetFlag(ENT_CAT_STATIC);
				// continue for now so it doesnt get regged
				//continue;
			}
			else
				res = InvalidEntity;
			break;
		}
	case ET_PLAYER:
		{
			if (!pEnt->client || (pEnt->entstate == STATE_INVISIBLE) ||
				(pEnt->client->ps.pm_type == PM_SPECTATOR) ||
				(pEnt->client->sess.sessionTeam != TEAM_AXIS &&
				pEnt->client->sess.sessionTeam != TEAM_ALLIES))
			{
				res = InvalidEntity;
				break;
			}

			// Special case for dead players that haven't respawned.
			if(pEnt->health <= 0)
			{
				_category.SetFlag(ENT_CAT_MISC);
				break;
			}
			_category.SetFlag(ENT_CAT_SHOOTABLE);
			_category.SetFlag(ENT_CAT_PLAYER);
			break;
		}

	case ET_ITEM:
		{				
			if(!Q_strncmp(pEnt->classname, "item_health", strlen("item_health")))
				_category.SetFlag(ENT_CAT_PICKUP);
			else if(!Q_strncmp(pEnt->classname, "weapon_magicammo", strlen("weapon_magicammo")))
				_category.SetFlag(ENT_CAT_PICKUP);
			else if(!Q_stricmp(pEnt->classname, "item_treasure"))
				_category.SetFlag(ENT_CAT_PICKUP);
			else
				res = InvalidEntity;
			break;
		}
	case ET_CORPSE:
		{
			_category.SetFlag(ENT_CAT_MISC);
			break;
		}
	case ET_MISSILE:
		{
			// Register certain weapons as threats to avoid or whatever.
			switch(pEnt->s.weapon) 
			{
			case WP_GRENADE_PINEAPPLE:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_PANZERFAUST:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_ARTY:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_DYNAMITE:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_SMOKE_MARKER:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_SMOKE_BOMB:
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_LANDMINE:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_SATCHEL:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_M7:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_GPG40:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			case WP_MORTAR_SET:
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_PROJECTILE);
				break;
			default:
				if(!Q_strncmp(pEnt->classname, "air strike", sizeof("air strike")))
				{
					_category.SetFlag(ENT_CAT_AVOID);
					_category.SetFlag(ENT_CAT_PROJECTILE);
					break;
				}
				else
					res = InvalidEntity;
			}
			break;
		}
	case ET_FLAMETHROWER_CHUNK:
		{
			_category.SetFlag(ENT_CAT_AVOID);
			_category.SetFlag(ENT_CAT_PROJECTILE);
			break;
		}
	case ET_MOVER:
		{
			if (!Q_stricmp(pEnt->classname, "script_mover"))
			{
				if(pEnt->model2)
					_category.SetFlag(ENT_CAT_VEHICLE);
				else
					_category.SetFlag(ENT_CAT_MOVER);

				_category.SetFlag(ENT_CAT_STATIC);
				if(pEnt->health > 0)
				{
					_category.SetFlag(ENT_CAT_SHOOTABLE);
				}
			}
			/*else if (!Q_stricmp(pCurrent->classname, "props_flamebarrel"))
			{
			info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			}
			else if (!Q_stricmp(pCurrent->classname, "props_statue"))
			{
			info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			}*/
			else
				res = InvalidEntity;
			break;
		}
	case ET_MG42_BARREL:
		{
			if((pEnt->health > 0) &&
				(pEnt->entstate != STATE_INVISIBLE) && 
				(pEnt->entstate != STATE_UNDERCONSTRUCTION))
			{
				_category.SetFlag(ENT_CAT_MOUNTEDWEAPON);
				_category.SetFlag(ENT_CAT_SHOOTABLE);
			}
			else
				res = InvalidEntity;
			break;
		}
		/*case ET_AAGUN:
		{
		if((pCurrent->health > 0) &&
		(pCurrent->entstate != STATE_INVISIBLE) && 
		(pCurrent->entstate != STATE_UNDERCONSTRUCTION))
		{
		}
		break;
		}*/
	case ET_EXPLOSIVE:
		{
			if (!(pEnt->spawnflags & EXPLOSIVE_TANK) &&
				(pEnt->constructibleStats.weaponclass != 1) &&
				(pEnt->constructibleStats.weaponclass != 2))// &&
				//(pEnt->health > 0) && (pEnt->takedamage == qtrue))
			{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
			}
			else
				res = InvalidEntity;
			break;
		}
		//case ET_CONSTRUCTIBLE:
		//{
		//if (G_ConstructionIsPartlyBuilt(pCurrent) &&
		//	!(pCurrent->spawnflags & CONSTRUCTIBLE_INVULNERABLE) &&
		//	 (pCurrent->constructibleStats.weaponclass == 0))
		//{
		//	info.m_EntityClass = ET_CLASSEX_BREAKABLE;
		//	info.m_EntityCategory = ENT_CAT_SHOOTABLE;
		//}
		//else
		//{
		//}
		//break;
		//}
	case ET_HEALER:
		{
			_category.SetFlag(ENT_CAT_PICKUP);
			break;
		}
	case ET_SUPPLIER:
		{
			_category.SetFlag(ENT_CAT_PICKUP);
			break;
		}
	default:
		res = InvalidEntity;
		break; // ignore this type.
	};
	return res;
}

static obResult obGetEntityFlags(const GameEntity _ent, BitFlag64 &_entflags)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);

	if(pEnt && pEnt->inuse)
	{
		// Set any flags.
		if(pEnt->health <= 0)
			_entflags.SetFlag(ENT_FLAG_DEAD);
		if(pEnt->client && !(pEnt->r.svFlags & SVF_BOT))
			_entflags.SetFlag(ENT_FLAG_HUMANCONTROLLED);

		if(pEnt->waterlevel >= 3)
			_entflags.SetFlag(ENT_FLAG_UNDERWATER);
		else if(pEnt->waterlevel > 0)
			_entflags.SetFlag(ENT_FLAG_INWATER);
		if(pEnt->s.eFlags & EF_ZOOMING)
			_entflags.SetFlag(ENT_FLAG_ZOOMING);

		if(pEnt->s.eFlags & EF_MG42_ACTIVE)
		{
			_entflags.SetFlag(ET_ENT_FLAG_MNT_MG42);
			_entflags.SetFlag(ET_ENT_FLAG_MOUNTED);
		}
		if(pEnt->s.eFlags & EF_MOUNTEDTANK)
		{
			_entflags.SetFlag(ET_ENT_FLAG_MNT_TANK);
			_entflags.SetFlag(ET_ENT_FLAG_MOUNTED);
		}
		if(pEnt->s.eFlags & EF_AAGUN_ACTIVE)
		{
			_entflags.SetFlag(ET_ENT_FLAG_MNT_AAGUN);	
			_entflags.SetFlag(ET_ENT_FLAG_MOUNTED);
		}

		if((pEnt->s.eType == ET_MOVER) && _TankIsMountable(pEnt))
		{
			_entflags.SetFlag(ET_ENT_FLAG_ISMOUNTABLE);
			if(G_TankIsOccupied(pEnt))
				_entflags.SetFlag(ET_ENT_FLAG_MOUNTED);
		}

		if((pEnt->s.eType == ET_MG42_BARREL) && _EmplacedGunIsMountable(pEnt))
		{
			_entflags.SetFlag(ET_ENT_FLAG_ISMOUNTABLE);
		}

		if(pEnt->client)
		{
			if(pEnt->client->ps.pm_flags & PMF_LADDER)
				_entflags.SetFlag(ENT_FLAG_ONLADDER);
			if(pEnt->client->ps.eFlags & EF_PRONE)
				_entflags.SetFlag(ENT_FLAG_PRONED);
			if(pEnt->client->ps.pm_flags & PMF_DUCKED)
				_entflags.SetFlag(ENT_FLAG_CROUCHED);
			if(pEnt->client->ps.groundEntityNum != ENTITYNUM_NONE)
				_entflags.SetFlag(ENT_FLAG_ONGROUND);
			if(pEnt->client->ps.weaponstate == WEAPON_RELOADING)
				_entflags.SetFlag(ENT_FLAG_RELOADING);

			if (pEnt->client->ps.powerups[PW_OPS_DISGUISED])
				_entflags.SetFlag(ET_ENT_FLAG_DISGUISED);
			if (pEnt->client->ps.powerups[PW_REDFLAG] || pEnt->client->ps.powerups[PW_BLUEFLAG])
				_entflags.SetFlag(ET_ENT_FLAG_CARRYINGGOAL);
			if(pEnt->client->ps.pm_flags & PMF_LIMBO)
				_entflags.SetFlag(ET_ENT_FLAG_INLIMBO);
		/*	if(pEnt->client->pmext.poisoned)
				_entflags.SetFlag(ET_ENT_FLAG_POISONED);*/
		}

		switch(pEnt->s.eType)
		{
		case ET_PLAYER:
			{
				if(pEnt->health <= 0)
					if(!pEnt->r.linked || BODY_TEAM(pEnt) >= 4 || BODY_VALUE(pEnt) >= 250 || pEnt->health < GIB_HEALTH)
						_entflags.SetFlag(ENT_FLAG_DISABLED);
				break;
			}
		case ET_CORPSE:
			{
				if(!pEnt->r.linked || BODY_TEAM(pEnt) >= 4 || BODY_VALUE(pEnt) >= 250 || pEnt->health < GIB_HEALTH)
					_entflags.SetFlag(ENT_FLAG_DISABLED);
				break;
			}
		}
	}
	return Success;
}

static obResult obGetEntityPowerups(const GameEntity _ent, BitFlag64 &_flags)
{
	return Success;
}

static void pfnGetThreats() 
{
	int i = 0;
	EntityInfo info;

	// Reset the cache.
	g_NumSmokeGrenades = 0;	

	for ( i = 0; i < level.num_entities; ++i)
	{
		gentity_t* pCurrent = &g_entities[i];

		// Skip bad/unused entities.
		if(!pCurrent->inuse)
			continue;

		// cache smoke bombs
		if(pCurrent->s.eType == ET_MISSILE && 
			pCurrent->s.weapon == WP_SMOKE_BOMB && 
			g_NumSmokeGrenades < MAX_CACHE-1)
		{
			g_SmokeGrenadeCache[g_NumSmokeGrenades++] = pCurrent;
		}
		
		// default data.
		info.m_EntityFlags.ClearAll();
		info.m_EntityCategory.ClearAll();
		info.m_EntityPowerups.ClearAll();
		info.m_EntityClass = _GetEntityClass(pCurrent);
        
		if(info.m_EntityClass != ET_CLASS_NULL &&
			obGetEntityCategory(pCurrent, info.m_EntityCategory) == Success &&
			obGetEntityFlags(pCurrent, info.m_EntityFlags) == Success &&
			obGetEntityPowerups(pCurrent, info.m_EntityFlags) == Success)
		{
			g_BotFunctions.pfnBotAddEntityInfo((GameEntity)pCurrent, &info);			
		}
	}
}

static const char *pfnGetMapName() 
{
	return level.rawmapname;
}

static void pfnGetMapExtents(AABB *_aabb)
{
	if(level.mapcoordsValid)
	{
		_aabb->m_Mins[0] = level.mapcoordsMins[0] * 2.F;
		_aabb->m_Mins[1] = level.mapcoordsMins[1] * 2.F;
		_aabb->m_Mins[2] = -65535.0f;

		_aabb->m_Maxs[0] = level.mapcoordsMaxs[0] * 2.F;
		_aabb->m_Maxs[1] = level.mapcoordsMaxs[1] * 2.F;
		_aabb->m_Maxs[2] = 65535.0f;
	}
	else
	{
		memset(_aabb, 0, sizeof(AABB));
	}
}

static const char *pfnGetGameName() 
{
	return GAME_VERSION;
}

static const char *pfnGetModName() 
{
	return OMNIBOT_MODNAME;
}

static const char *pfnGetModVers() 
{
	return OMNIBOT_MODVERSION;
}

static const char *pfnGetBotPath() 
{
	return Omnibot_GetLibraryPath();
}

static const char *pfnGetLogPath() 
{
	static char logpath[512];
	trap_Cvar_VariableStringBuffer("fs_homepath", logpath, sizeof(logpath));
	return Omnibot_FixPath(logpath);
}

GameEntity pfnFindEntityInSphere(const float _pos[3], float _radius, GameEntity _pStart, int classId)
{
	// square it to avoid the square root in the distance check.
	gentity_t *pStartEnt = _pStart ? _GetEntityFromGameEntity(_pStart) : 0;
	
	const char *pClassName = 0;
	int iPlayerClass = 0;
	int iSpawnFlags = 0;
	switch(classId)
	{
	case ET_CLASS_SOLDIER:
	case ET_CLASS_MEDIC:
	case ET_CLASS_ENGINEER:
	case ET_CLASS_FIELDOPS:
	case ET_CLASS_COVERTOPS:
	case ET_CLASS_ANY:
		iPlayerClass = classId != ET_CLASS_ANY ? classId : 0;
		pClassName = "player";
		break;
		//////////////////////////////////////////////////////////////////////////
	case ET_CLASSEX_MG42MOUNT:
		pClassName = "misc_mg42";
		break;
	case ET_CLASSEX_DYNAMITE:
		pClassName = "dynamite";
		break;
	case ET_CLASSEX_MINE:
		pClassName = "landmine";
		break;
	case ET_CLASSEX_SATCHEL:
		pClassName = "satchel_charge";
		break;
	case ET_CLASSEX_SMOKEBOMB:
		pClassName = "smoke_bomb";
		break;
	case ET_CLASSEX_SMOKEMARKER:
		pClassName = "air strike";
		break;
	case ET_CLASSEX_VEHICLE:
	case ET_CLASSEX_VEHICLE_HVY:
		iSpawnFlags = classId == ET_CLASSEX_VEHICLE_HVY ? 4 : 0;
		pClassName = "script_mover";
		break;
	case ET_CLASSEX_BREAKABLE:
		break;
	case ET_CLASSEX_CORPSE:
		pClassName = "corpse";
		break;
	case ET_CLASSEX_TREASURE:
		pClassName = "item_treasure";
		break;
	case ET_CLASSEX_GRENADE:
		pClassName = "grenade";
		break;
	case ET_CLASSEX_ROCKET:
		pClassName = "rocket";
		break;
	case ET_CLASSEX_MORTAR:
		pClassName = "mortar_grenade";
		break;
	case ET_CLASSEX_ARTY:
		pClassName = "air strike";
		break;
	case ET_CLASSEX_AIRSTRIKE:
		pClassName = "air strike";
		break;
	case ET_CLASSEX_FLAMECHUNK:
		pClassName = "flamechunk";
		break;
	case ET_CLASSEX_M7_GRENADE:
		pClassName = "m7_grenade";
		break;
	case ET_CLASSEX_GPG40_GRENADE:
		pClassName = "gpg40_grenade";
		break;
	case ET_CLASSEX_HEALTHCABINET:
		pClassName = "misc_cabinet_health";
		break;
	case ET_CLASSEX_AMMOCABINET:
		pClassName = "misc_cabinet_supply";
		break;
	}

	if(pClassName)
	{
		float fSqRad = _radius * _radius;
		vec3_t toent;

		while((pStartEnt = G_Find(pStartEnt, FOFS(classname), pClassName)) != NULL)
		{
			if(iPlayerClass && pStartEnt->client && 
				pStartEnt->client->sess.sessionTeam != iPlayerClass)
				continue;

			if(iSpawnFlags && !(pStartEnt->spawnflags & iSpawnFlags))
				continue;

			VectorSubtract(_pos, pStartEnt->r.currentOrigin, toent);
			if(VectorLengthSquared(toent) < fSqRad)
				break;
		}
		return (GameEntity)pStartEnt;
	}
    return NULL;
}

// helper stuff
qboolean InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles)
{
	int i;
	float diff, angle;

	for (i = 0; i < 2; i++) {
		angle = AngleMod(viewangles[i]);
		angles[i] = AngleMod(angles[i]);
		diff = angles[i] - angle;
		if (angles[i] > angle) {
			if (diff > 180.0) diff -= 360.0;
		}
		else {
			if (diff < -180.0) diff += 360.0;
		}
		if (diff > 0) {
			if (diff > fov * 0.5) return qfalse;
		}
		else {
			if (diff < -fov * 0.5) return qfalse;
		}
	}
	return qtrue;
}

int Bot_Interface_Init() 
{
	if(g_OmniBotEnable.integer == 0)
	{
		pfnPrintMessage("Omni-bot is currently disabled with \"omnibot_enable 0\"");
		return 1;
	}

	g_InterfaceFunctions.pfnAddBot						= pfnAddBot;
	g_InterfaceFunctions.pfnRemoveBot					= pfnRemoveBot;	
	g_InterfaceFunctions.pfnChangeClass					= pfnChangeClass;
	g_InterfaceFunctions.pfnChangeTeam					= pfnChangeTeam;
	g_InterfaceFunctions.pfnGetClientPosition			= pfnGetClientPosition;
	g_InterfaceFunctions.pfnGetClientOrientation		= pfnGetClientOrientation;

	// Set up all the utility functions.
	g_InterfaceFunctions.pfnPrintScreenText				= pfnPrintScreenMessage;
	g_InterfaceFunctions.pfnPrintError					= pfnPrintError;
	g_InterfaceFunctions.pfnPrintMessage				= pfnPrintMessage;
	g_InterfaceFunctions.pfnIsInPVS						= pfnIsInPVS;
	g_InterfaceFunctions.pfnTraceLine					= pfnTraceLine;
	g_InterfaceFunctions.pfnGetPointContents			= pfnGetPointContents;
	g_InterfaceFunctions.pfnUpdateBotInput				= pfnUpdateBotInput;
	g_InterfaceFunctions.pfnBotCommand					= pfnBotCommand;

	g_InterfaceFunctions.pfnGetMapExtents				= pfnGetMapExtents;
	g_InterfaceFunctions.pfnGetMapName					= pfnGetMapName;
	g_InterfaceFunctions.pfnGetGameName					= pfnGetGameName;
	g_InterfaceFunctions.pfnGetModName					= pfnGetModName;
	g_InterfaceFunctions.pfnGetModVers					= pfnGetModVers;
	g_InterfaceFunctions.pfnGetGameTime					= pfnGetGameTime;
	g_InterfaceFunctions.pfnGetClientName				= pfnGetClientName;
	g_InterfaceFunctions.pfnGetGoals					= pfnGetGoals;
	g_InterfaceFunctions.pfnGetThreats					= pfnGetThreats;
	g_InterfaceFunctions.pfnGetCurNumPlayers			= pfnGetCurNumPlayers;
	g_InterfaceFunctions.pfnGetMaxNumPlayers			= pfnGetMaxNumPlayers;
	g_InterfaceFunctions.pfnInterfaceSendMessage		= pfnInterfaceSendMessage;
	g_InterfaceFunctions.pfnGetBotPath					= pfnGetBotPath;
	g_InterfaceFunctions.pfnGetLogPath					= pfnGetLogPath;	

	// Entity info.
	g_InterfaceFunctions.pfnGetEntityCategory			= obGetEntityCategory;
	g_InterfaceFunctions.pfnGetEntityFlags				= obGetEntityFlags;
	g_InterfaceFunctions.pfnGetEntityPowerups			= obGetEntityPowerups;
	g_InterfaceFunctions.pfnGetEntityEyePosition		= obGetEntityEyePosition;
	g_InterfaceFunctions.pfnGetEntityBonePosition		= obGetEntityBonePosition;
	g_InterfaceFunctions.pfnGetEntityVelocity			= pfnGetEntityVelocity;
	g_InterfaceFunctions.pfnGetEntityPosition			= pfnGetEntityPosition;
	g_InterfaceFunctions.pfnGetEntityOrientation		= pfnGetEntityOrientation;
	g_InterfaceFunctions.pfnGetEntityWorldAABB			= pfnGetEntityWorldAABB;
	g_InterfaceFunctions.pfnGetEntityOwner				= pfnGetEntityOwner;
	g_InterfaceFunctions.pfnGetEntityTeam				= pfnGetEntityTeam;
	g_InterfaceFunctions.pfnGetEntityClass				= pfnGetEntityClass;

	g_InterfaceFunctions.pfnBotGetCurrentAmmo			= pfnBotGetCurrentAmmo;
	g_InterfaceFunctions.pfnBotGetCurrentWeaponClip		= pfnBotGetCurrentWeaponClip;

	g_InterfaceFunctions.pfnEntityFromID				= pfnEntityFromID;
	g_InterfaceFunctions.pfnIDFromEntity				= pfnIDFromEntity;

	g_InterfaceFunctions.pfnFindEntityInSphere			= pfnFindEntityInSphere;

	// Waypoint functions.
	g_InterfaceFunctions.pfnDebugLine					= 0;
	g_InterfaceFunctions.pfnDebugRadius					= 0;

	eomnibot_error err = Omnibot_LoadLibrary(ET_VERSION_LATEST, 
		"omnibot_et", Omnibot_FixPath(g_OmniBotPath.string));
	if(err == BOT_ERROR_NONE)
		return true;
	return false;
}

int Bot_Interface_Shutdown()
{
	if(IsOmnibotLoaded())
	{
		Bot_Event_EndGame();		
		g_BotFunctions.pfnBotShutdown();
	}
	Omnibot_FreeLibrary();
	return 1;
}

void Bot_Interface_Update()
{
	if(IsOmnibotLoaded())
	{
		char buf[1024] = {0};

		if (level.framenum == GAME_INIT_FRAMES)
			Bot_Event_StartGame();

		//////////////////////////////////////////////////////////////////////////
		{
			static float serverGravity = 0.0f;
			if(serverGravity != g_gravity.value)
			{
				Event_SystemGravity d = { -g_gravity.value };
				g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_GRAVITY, &d, sizeof(d)));
				serverGravity = g_gravity.value;
			}
			static int cheatsEnabled = 0;
			if(g_cheats.integer != cheatsEnabled)
			{
				Event_SystemCheats d = { g_cheats.integer?True:False };
				g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_CHEATS, &d, sizeof(d)));
				cheatsEnabled = g_cheats.integer;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// Call the libraries update.
		g_BotFunctions.pfnBotUpdate();

		//////////////////////////////////////////////////////////////////////////
		int iNumBots = 0;
		for( int i = 0; i < MAX_CLIENTS; i++ )
		{
			if(!g_entities[i].inuse)
				continue;
			if (!g_entities[i].client)
				continue;
			if(g_entities[i].client->pers.connected != CON_CONNECTED)
				continue;

			/*if(i==1)
				g_entities[i].flags |= FL_GODMODE;*/

			// Send a spectated message to bots that are being spectated.
			if ((g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR) && 
				(g_entities[i].client->sess.spectatorState == SPECTATOR_FOLLOW))
			{
				int iDestination = g_entities[i].client->sess.spectatorClient;
				Bot_Event_Spectated(iDestination, i);
			}

			// fake handle server commands (to prevent server command overflow)
			if((g_entities[i].inuse == qtrue) && ( g_entities[i].r.svFlags & SVF_BOT))
			{
				++iNumBots;
				while (trap_BotGetServerCommand(i, buf, sizeof(buf))) {}
			}			
		}

		if(!(g_OmniBotFlags.integer & OBF_DONT_SHOW_BOTCOUNT))
			g_OmniBotPlaying.integer = iNumBots;
		else
			g_OmniBotPlaying.integer = -1;
	}
}

//////////////////////////////////////////////////////////////////////////

int Bot_Interface_ConsoleCommand() 
{
	if(IsOmnibotLoaded())
	{
		char token[MAX_TOKEN_CHARS] = {0};
		char buffer[MAX_TOKEN_CHARS] = {0};
		int ipos = 0;
		int i = 1;

		do
		{
			// get the next token
			trap_Argv(i++, token, sizeof(token));
			// append it to the buffer
			strncpy(&buffer[ipos], token, strlen(token));
			ipos += strlen(token);
			buffer[ipos++] = ' ';
		} while(token[0]);

		return g_BotFunctions.pfnBotConsoleCommand(buffer, strlen(buffer));
	}
	return 1;
}

void Bot_Util_AddGoal(const GameEntity _ent, int _goaltype, int _team, const char *_tag, obUserData *_bud)
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotAddGoal(_ent, _goaltype, _team, _tag, _bud);
	}
}

void Bot_Util_SendTrigger(TriggerInfo *_triggerInfo)
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotSendTrigger(_triggerInfo);
	}
}

//////////////////////////////////////////////////////////////////////////

void Bot_Event_ClientConnected(int _client, qboolean _isbot)
{
	if(IsOmnibotLoaded())
	{
		Event_SystemClientConnected d;
		d.m_GameId = _client;
		d.m_IsBot = _isbot == qtrue ? True : False;
		g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_CLIENTCONNECTED, &d, sizeof(d)));
	}
}

void Bot_Event_ClientDisConnected(int _client)
{
	if(IsOmnibotLoaded())
	{
		Event_SystemClientDisConnected d = { _client };
		g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_CLIENTDISCONNECTED, &d, sizeof(d)));
	}
}

void Bot_Event_StartGame()
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_STARTGAME));
	}
}

void Bot_Event_EndGame()
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotSendGlobalEvent(MessageHelper(GAME_ID_ENDGAME));
	}
}

void Bot_Event_Spawn(int _client)
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_SPAWN));
	}
}

void Bot_Event_ResetWeapons(int _client)
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_RESETWEAPONS));
	}
}

void Bot_Event_AddWeapon(int _client, int _weaponId)
{
	if(IsOmnibotLoaded())
	{
		Event_AddWeapon d = { _weaponId };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_ADDWEAPON, &d, sizeof(d)));
	}
}

void Bot_Event_ChangeTeam(int _client, int _newteam)
{
	if(IsOmnibotLoaded())
	{
		Event_ChangeTeam d = { _newteam };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_CHANGETEAM, &d, sizeof(d)));
	}
}

void Bot_Event_ChangeClass(int _client, int _newclass)
{
	if(IsOmnibotLoaded())
	{
		Event_ChangeClass d = { _newclass };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_CHANGECLASS, &d, sizeof(d)));
	}
}

void Bot_Event_TakeDamage(int _client, GameEntity _ent)
{
	if(IsOmnibotLoaded())
	{
		Event_TakeDamage d = { _ent };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(PERCEPT_FEEL_PAIN, &d, sizeof(d)));
	}
}

void Bot_Event_Death(int _client, GameEntity _killer, const char *_meansofdeath)
{
	if(IsOmnibotLoaded())
	{
		Event_Death d;
		d.m_WhoKilledMe = _killer;
		Q_strncpyz(d.m_MeansOfDeath, 
			_meansofdeath ? _meansofdeath : "<unknown>", sizeof(d.m_MeansOfDeath));
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_DEATH, &d, sizeof(d)));
	}
}

void Bot_Event_Healed(int _client, GameEntity _whodoneit)
{
	if(IsOmnibotLoaded())
	{
		Event_Healed d = { _whodoneit };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_HEALED, &d, sizeof(d)));
	}
}

void Bot_Event_Revived(int _client, GameEntity _whodoneit)
{
	if(IsOmnibotLoaded())
	{
		Event_Revived d = { _whodoneit };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_REVIVED, &d, sizeof(d)));
	}
}

void Bot_Event_KilledSomeone(int _client, GameEntity _victim, const char *_meansofdeath)
{
	if(IsOmnibotLoaded())
	{
		Event_KilledSomeone d;
		d.m_WhoIKilled = _victim;
		Q_strncpyz(d.m_MeansOfDeath, 
			_meansofdeath ? _meansofdeath : "<unknown>", 
			sizeof(d.m_MeansOfDeath) / sizeof(d.m_MeansOfDeath[0]));
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_KILLEDSOMEONE, &d, sizeof(d)));
	}
}

void Bot_Event_FireWeapon(int _client, int _weaponId, GameEntity _projectile)
{
	if(IsOmnibotLoaded())
	{
		Event_WeaponFire d = { };
		d.m_WeaponId = _weaponId;
		d.m_Projectile = _projectile;
		d.m_FireMode = Primary;
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(ACTION_WEAPON_FIRE, &d, sizeof(d)));
	}
}

void Bot_Event_PreTriggerMine(int _client, GameEntity _mine)
{
	if(IsOmnibotLoaded())
	{
		Event_TriggerMine_ET d = { _mine };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(ET_EVENT_PRETRIGGER_MINE, &d, sizeof(d)));
	}
}

void Bot_Event_PostTriggerMine(int _client, GameEntity _mine)
{
	if(IsOmnibotLoaded())
	{
		Event_TriggerMine_ET d = { _mine };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(ET_EVENT_POSTTRIGGER_MINE, &d, sizeof(d)));
	}
}

void Bot_Event_MortarImpact(int _client, vec3_t _pos)
{
	if(IsOmnibotLoaded())
	{
		Event_MortarImpact_ET d = { };
		d.m_Position[0] = _pos[0];
		d.m_Position[1] = _pos[1];
		d.m_Position[2] = _pos[2];
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(ET_EVENT_MORTAR_IMPACT, &d, sizeof(d)));
	}
}

void Bot_Event_Spectated(int _client, int _who)
{
	if(IsOmnibotLoaded())
	{
		Event_Spectated d = { _who };
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(MESSAGE_SPECTATED, &d, sizeof(d)));
	}
}

void Bot_Event_ChatMessage(int _to, GameEntity _source, int _type, const char *_message)
{
	if(IsOmnibotLoaded())
	{
		Event_ChatMessage d;
		d.m_WhoSaidIt = _source;
		Q_strncpyz(d.m_Message, _message ? _message : "<unknown>",
			sizeof(d.m_Message) / sizeof(d.m_Message[0]));
		g_BotFunctions.pfnBotSendEvent(_to, MessageHelper(_type, &d, sizeof(d)));
	}
}

void Bot_Event_VoiceMacro(int _client, GameEntity _source, int _type, const char *_message)
{
	if(IsOmnibotLoaded())
	{
		Event_VoiceMacro d;
		d.m_WhoSaidIt = _source;
		Q_strncpyz(d.m_MacroString, _message ? _message : "<unknown>",
			sizeof(d.m_MacroString) / sizeof(d.m_MacroString[0]));
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(_type, &d, sizeof(d)));
	}
}

void Bot_Event_Sound(int _client, int _sndtype, GameEntity _source, float *_origin, const char *_name)
{
	if(IsOmnibotLoaded())
	{
		Event_HearSound d = {};
		d.m_Source = _source;
		d.m_SoundType = _sndtype;		
		if(_origin)
		{
			d.m_Origin[0] = _origin[0];
			d.m_Origin[1] = _origin[1];
			d.m_Origin[2] = _origin[2];
		}
		Q_strncpyz(d.m_SoundName, _name ? _name : "<unknown>", sizeof(d.m_SoundName) / sizeof(d.m_SoundName[0]));
		g_BotFunctions.pfnBotSendEvent(_client, MessageHelper(PERCEPT_HEAR_SOUND, &d, sizeof(d)));
	}
}

//////////////////////////////////////////////////////////////////////////

//#include "IEngineInterface.h"
//
//class EngineInterface : public IEngineInterface
//{
//	int AddBot(const MessageHelper &_data)
//	{
//		return -1;
//	}
//
//	// Function: RemoveBot
//	//		This function should remove/kick a bot from the game by its name
//	int RemoveBot(const char *_name)
//	{
//		return 0;
//	}
//
//	// Function: ChangeTeam
//	//		This function should force a bot to a certain team
//	obResult ChangeTeam(int _client, int _newteam, const MessageHelper *_data)
//	{
//		return Success;
//	}
//
//	// Function: ChangeClass
//	//		This function should force a bot to change to a certain class
//	obResult ChangeClass(int _client, int _newclass, const MessageHelper *_data)
//	{
//		return Success;
//	}
//
//	// Function: UpdateBotInput
//	//		This function should interpret and handle the bots input
//	void UpdateBotInput(int _client, const ClientInput *_input)
//	{
//
//	}
//
//	// Function: BotCommand
//	//		This function should perform a bot 'console command'
//	void BotCommand(int _client, const char *_cmd)
//	{
//
//	}
//
//	// Function: IsInPVS
//	//		This function should check if a target position is within pvs of another position.
//	obBool IsInPVS(const float _pos[3], const float _target[3])
//	{
//		return False;
//	}
//
//	// Function: TraceLine
//	//		This bot should intepret and perform a traceline, returning
//	//		the results into the <BotTraceResult> parameter.
//	obResult TraceLine(obTraceResult *_result, const float _start[3], const float _end[3], const AABB *_pBBox , int _mask, int _user, obBool _bUsePVS)
//	{
//		return Success;
//	}
//
//	// Function: GetPointContents
//	//		Gets the content bitflags for a location.
//	int GetPointContents(const float _pos[3])
//	{
//		return 0;
//	}
//
//	// Function: FindEntityInSphere
//	//		This function should return entities matching the class id, and in a radius, and should be
//	//		compatible with a while loop using the _pStart and returning 0 at the end of search
//	GameEntity FindEntityInSphere(const float _pos[3], float _radius, GameEntity _pStart, int classId)
//	{
//		return NULL;
//	}
//
//	// Function: GetEntityClass
//	//		This function should return the bot class of the entity.
//	int GetEntityClass(const GameEntity _ent)
//	{
//		return 0;
//	}
//
//	// Function: GetEntityCategory
//	//		This function should return the bot class of the entity.
//	obResult  GetEntityCategory(const GameEntity _ent, BitFlag64 &_flags)
//	{
//		return Success;
//	}
//
//	// Function: GetEntityFlags
//	//		This function should return the entity flags for an entity
//	obResult GetEntityFlags(const GameEntity _ent, BitFlag64 &_flags) 
//	{
//		return Success;
//	}
//
//	// Function: GetEntityPowerups
//	//		This function should return the powerup flags for an entity.
//	obResult GetEntityPowerups(const GameEntity _ent, BitFlag64 &_flags)
//	{
//		return Success;
//	}
//
//	// Function: GetEntityEyePosition
//	//		This function should return the eye position of an entity
//	obResult GetEntityEyePosition(const GameEntity _ent, float _pos[3])
//	{
//		return Success;
//	}
//
//	// Function: GetEntityEyePosition
//	//		This function should return the bone position of an entity
//	obResult GetEntityBonePosition(const GameEntity _ent, int _boneid, float _pos[3])
//	{
//		return Success;
//	}
//
//	// Function: GetEntityOrientation
//	//		This function should return the orientation of a <GameEntity> as fwd, right, up vectors
//	obResult GetEntityOrientation(const GameEntity _ent, float _fwd[3], float _right[3], float _up[3])
//	{
//		return Success;
//	}
//
//	// Function: GetEntityVelocity
//	//		This function should return the velocity of a <GameEntity> in world space
//	obResult GetEntityVelocity(const GameEntity _ent, float _velocity[3])
//	{
//		return Success;
//	}
//
//	// Function: GetEntityPosition
//	//		This function should return the position of a <GameEntity> in world space
//	obResult GetEntityPosition(const GameEntity _ent, float _pos[3])
//	{
//		return Success;
//	}
//
//	// Function: GetEntityWorldAABB
//	//		This function should return the axis aligned box of a <GameEntity> in world space
//	obResult GetEntityWorldAABB(const GameEntity _ent, AABB *_aabb)
//	{
//		return Success;
//	}
//
//	// Function: GetEntityOwner
//	//		This function should return the <GameID> of a client that owns this item
//	int GetEntityOwner(const GameEntity _ent)
//	{
//		return 0;
//	}
//
//	// Function: GetEntityTeam
//	//		This function should return the bot team of the entity.
//	int GetEntityTeam(const GameEntity _ent)
//	{
//		return 0;
//	}
//
//	// Function: EntityFromID
//	//		This function should return the <GameEntity> that matches the provided Id
//	GameEntity EntityFromID(const int _id)
//	{
//		return NULL;
//	}
//
//	// Function: IDFromEntity
//	//		This function should return the Id that matches the provided <GameEntity>
//	int IDFromEntity(const GameEntity _ent)
//	{
//		return 0;
//	}
//
//	// Function: GetClientPosition
//	//		This function should return the position of a client in world space
//	obResult GetClientPosition(int _client, float _pos[3])
//	{
//		return Success;
//	}
//
//	// Function: GetClientOrientation
//	//		This function should return the orientation of a <GameEntity> as fwd, right, up vectors
//	obResult GetClientOrientation(int _client, float _fwd[3], float _right[3], float _up[3])
//	{
//		return Success;
//	}
//
//	// Function: GetClientName
//	//		This function should give access to the in-game name of the client
//	const char *GetClientName(int _client)
//	{
//		return "";
//	}
//
//	// Function: GetCurrentWeaponClip
//	//		This function should update weapon clip count for the current weapon
//	obResult GetCurrentWeaponClip(int _client, FireMode _mode, int &_curclip, int &_maxclip)
//	{
//		return Success;
//	}
//
//	// Function: BotGetCurrentAmmo
//	//		This function should update ammo stats for a client and ammotype
//	obResult GetCurrentAmmo(int _client, int _ammotype, int &_cur, int &_max)
//	{
//		return Success;
//	}
//
//	// Function: GetGameTime
//	//		This function should return the current game time in milli-seconds
//	int GetGameTime()
//	{
//
//	}
//
//	// Function: GetGoals
//	//		This function should tell the game to register all <MapGoal>s with the bot
//	void GetGoals()
//	{
//
//	}
//
//	// Function: GetThreats
//	//		This function should tell the game to register all potential threats with the bot
//	//		Threats include potential targets, projectiles, or other entities of interest to the bot
//	void GetThreats()
//	{
//
//	}
//
//	// Function: GetMaxNumPlayers
//	//		Gets the currently set maximum number of players from the game
//	int GetMaxNumPlayers()
//	{
//		return 0;
//	}
//
//	// Function: GetCurNumPlayers
//	//		Gets the current number of players from the game. Combine with above?
//	int GetCurNumPlayers()
//	{
//		return 0;
//	}
//
//	// Function: InterfaceSendMessage
//	//		This function sends a message to the game with optional <MessageHelper> in/out parameters
//	//		to request additional or mod specific info from the game
//	obResult InterfaceSendMessage(const MessageHelper &_data, const GameEntity _ent)
//	{
//		return Success;
//	}
//
//	// Function: AddTempDisplayLine
//	//		Adds a line to immediately display between 2 positions, with a specific color
//	bool DebugLine(const float _start[3], const float _end[3], const obColor &_color, float _time)
//	{
//		return false;
//	}
//
//	// Function: DebugRadius
//	//		Adds a radius indicator to be displayed at a certain position with radius and color
//	bool DebugRadius(const float _pos[3], const float _radius, const obColor &_color, float _time)
//	{
//		return false;
//	}
//
//	// Function: PrintError
//	//		This function should print an error the the game however desired,
//	//		whether it be to the console, messagebox,...
//	void PrintError(const char *_error)
//	{
//
//	}
//
//	// Function: PrintMessage
//	//		This function should print a message the the game however desired,
//	//		whether it be to the console, messagebox,...
//	void PrintMessage(const char *_msg)
//	{
//
//	}
//
//	// Function: PrintScreenMessage
//	//		This function should print a message the the game screen if possible
//	void PrintScreenText(const int _client, const float _pos[3], float _duration, const obColor &_color, const char *_msg)
//	{
//
//	}
//
//	// Function: GetMapName
//	//		This function should give access to the name of the currently loaded map
//	const char *GetMapName()
//	{
//		return "";
//	}
//
//	// Function: GetMapExtents
//	//		This function gets the extents of the current map.
//	void GetMapExtents(AABB *_aabb)
//	{
//	}
//
//	// Function: GetGameName
//	//		This function should give access to the name of the currently loaded game
//	const char *GetGameName()
//	{
//		return "";
//	}
//
//	// Function: GetModName
//	//		This function should give access to the name of the currently loaded mod
//	const char *GetModName()
//	{
//		return "";
//	}
//
//	// Function: GetModVers
//	//		This function should give access to the version of the currently loaded mod
//	const char *GetModVers()
//	{
//		return "";
//	}
//
//	// Function: GetBotPath
//	//		This function should get the bot path to the bot dll and base path to supplemental files.
//	const char *GetBotPath()
//	{
//		return "";
//	}
//};

//////////////////////////////////////////////////////////////////////////


#endif // OMNIBOTS
