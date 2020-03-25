/*
* ET <-> Omni-Bot interface source file.
* 
*/
#ifdef OMNIBOTS

extern "C"
{
#include "g_local.h"
};

#define OB_snprintf _snprintf
#include "../../Omnibot/Common/BotExports.h"
#include "../../Omnibot/ET/ET_Messages.h"

#define OMNIBOT_MIN_ENG 2
#define OMNIBOT_MIN_MED 2
#define OMNIBOT_MIN_FOP 1
#define OMNIBOT_MIN_SOL 1
#define OMNIBOT_MIN_COP 1

int g_BotLoaded = 0;

BotUserData botUserData;

typedef struct
{
	float	start[3];
	float	end[3];
	float	color[3];
	float	dist;
	char	drawme;
	char	type;
} debugLines_t;

// Constants.
const int OMNIBOT_LINES_INCREASE_SIZE = 512;
const int OMNIBOT_LINES_INITIAL_SIZE = 4096;
const int OMNIBOT_MAX_RADIUS = 8;

typedef struct 
{
	debugLines_t	*m_pDebugLines;
	int				m_NumDebugLines;
	int				m_MaxDebugLines;
} LineList;

LineList		g_DebugLines = { 0,0,0 };
LineList		g_BlockableDebugLines = { 0,0,0 };

//////////////////////////////////////////////////////////////////////////

const int MAX_CACHE = 32;

int g_NumSmokeGrenades = 0;
gentity_t *g_SmokeGrenadeCache[MAX_CACHE] = {};

//////////////////////////////////////////////////////////////////////////

static int wp_compare(const void *_wp1, const void *_wp2)
{
	debugLines_t *wp1 = (debugLines_t*)_wp1;
	debugLines_t *wp2 = (debugLines_t*)_wp2;

	if(wp1->type == LINE_NONE)
		return 1;
	if(wp2->type == LINE_NONE)
		return -1;
	if(wp1->drawme == qfalse)
		return 1;
	if(wp2->drawme == qfalse)
		return -1;
	if(wp1->dist < wp2->dist)
		return -1;
	if(wp1->dist > wp2->dist)
		return 1;
	return 0;
}

// Utility functions to manage the line list.
void AddToLineList(LineList *_list, const debugLines_t *_line);
void ClearLineList(LineList *_list, qboolean _freememory);
void InitLineList(LineList *_list, int _initialsize);

static void pfnPrintError(const char *_error);
static void pfnPrintMessage(const char *_msg);
static void pfnAddTempDisplayLine(const float _start[3], const float _end[3], const float _color[3]);

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
	case ET_WP_FG42_SCOPE:
		return WP_FG42_SCOPE;
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
	// jaquboss - NQ stuff
//#ifdef NOQAURTER
// SMGs
	case ET_WP_STEN_MKII:
		return WP_STEN_MKII;
	case ET_WP_MP34:
		return WP_MP34;
// AR
	case ET_WP_BAR:
		return WP_BAR;
	case ET_WP_BAR_SET:
		return WP_BAR_SET;
	case ET_WP_STG44:
		return WP_STG44;
// MG
	case ET_WP_MOBILE_BROWNING:
		return WP_MOBILE_BROWNING;
	case ET_WP_MOBILE_BROWNING_SET:
		return WP_MOBILE_BROWNING_SET;
// RL
	case ET_WP_BAZOOKA:
		return WP_BAZOOKA;
// Mortar
	case ET_WP_MORTAR2:
		return WP_MORTAR2;
	case ET_WP_MORTAR2_SET:
		return WP_MORTAR2_SET;
// Sg
	case ET_WP_SHOTGUN:
		return WP_SHOTGUN;
// Fun
	case ET_WP_FIREBOLT:
		return WP_FIREBOLT;
	case ET_WP_VENOM:
		return WP_VENOM;
// KABAR
	case ET_WP_KNIFE_KABAR:
		return WP_KNIFE_KABAR;
//#endif
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
	case WP_FG42_SCOPE:
		return ET_WP_FG42_SCOPE;
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
	// jaquboss - NQ stuff
//#ifdef NOQAURTER
// SMGs
	case WP_STEN_MKII:
		return ET_WP_STEN_MKII;
	case WP_MP34:
		return ET_WP_MP34;
// AR
	case WP_BAR:
		return ET_WP_BAR;
	case WP_BAR_SET:
		return ET_WP_BAR_SET;
	case WP_STG44:
		return ET_WP_STG44;
// MG
	case WP_MOBILE_BROWNING:
		return ET_WP_MOBILE_BROWNING;
	case WP_MOBILE_BROWNING_SET:
		return ET_WP_MOBILE_BROWNING_SET;
// RL
	case WP_BAZOOKA:
		return ET_WP_BAZOOKA;
// Mortar
	case WP_MORTAR2:
		return ET_WP_MORTAR2;
	case WP_MORTAR2_SET:
		return ET_WP_MORTAR2_SET;
// Sg
	case WP_SHOTGUN:
		return ET_WP_SHOTGUN;
// Fun
	case WP_FIREBOLT:
		return ET_WP_FIREBOLT;
	case WP_VENOM:
		return ET_WP_VENOM;
// KABAR
	case WP_KNIFE_KABAR:
		return ET_WP_KNIFE_KABAR;
//#endif
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
/*
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
*/
		case HINT_WEAPON:
			iHintType = CURSOR_HINT_WEAPON;
			break;
		case HINT_AMMO:
			iHintType = CURSOR_HINT_AMMO;
			break;
		/*case HINT_ARMOR:
			iHintType = CURSOR_HINT_ARMOR;
			break;*/
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
/*
		case HINT_LOCKPICK:
			iHintType = CURSOR_HINT_LOCKPICK;
			break;
*/
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
#ifndef NOQUARTER // for now before we can fix it in bot core, don't declare unless you know what you're doing
		/*	int rInt = rand() % 3;
			if (rInt == 0)
				return ET_WP_PANZERFAUST;
			else if (rInt == 1)
				return ET_WP_MOBILE_MG42;
			else*/
				return ET_WP_FLAMETHROWER;
#else
			int rInt = rand() % 4;
			if (rInt == 0)
				return team == ET_TEAM_AXIS ? ET_WP_PANZERFAUST : ET_WP_BAZOOKA;
			else if (rInt == 1)
				return ET_TEAM_AXIS ? ET_WP_MOBILE_MG42 : ET_WP_MOBILE_BROWNING;
			else if (rInt == 2)
				return ET_WP_FLAMETHROWER;
			else 
				return  ET_TEAM_AXIS ? ET_WP_STG44 : ET_WP_BAR;
#endif
		}
	case ET_CLASS_MEDIC:
		{
			if (team == ET_TEAM_ALLIES)
				return ET_WP_THOMPSON;
			else
				return ET_WP_MP40;
		}
	case ET_CLASS_ENGINEER:
		{
#ifndef NOQUARTER
			int rInt = rand() % 2;

			if (team == ET_TEAM_ALLIES)
				if (rInt == 0)
					return ET_WP_THOMPSON;
				else
					return ET_WP_CARBINE;
			else
				if (rInt == 0)
					return ET_WP_MP40;
				else
					return ET_WP_KAR98;

#else
			int rInt = rand() % 3;

			if (team == ET_TEAM_ALLIES)
				if (rInt == 0)
					return ET_WP_THOMPSON;
				else if (rInt == 1)
					return ET_WP_CARBINE;
				else 
					return ET_WP_SHOTGUN;
			else
				if (rInt == 0)
					return ET_WP_MP40;
				else if(rInt == 1)
					return ET_WP_KAR98;
				else 
					return ET_WP_SHOTGUN;
#endif

		}
	case ET_CLASS_FIELDOPS:
		{
#ifndef NOQUARTER
			if (team == ET_TEAM_ALLIES)
				return ET_WP_THOMPSON;
			else
				return ET_WP_MP40;		
#else
		int rInt = rand() % 2;

			if (team == ET_TEAM_ALLIES)
				if (rInt == 0)
					return ET_WP_THOMPSON;
				else
					return ET_WP_SHOTGUN;
			else
				if (rInt == 0)
					return ET_WP_MP40;
				else
					return ET_WP_SHOTGUN;
#endif
		}
	case ET_CLASS_COVERTOPS:
		{
			int rInt = rand() % 3;

			if (rInt == 0){
#ifndef NOQUARTER
			if (team == ET_TEAM_ALLIES)
				return ET_WP_STEN;
			else
				return ET_WP_K43;
#else				
			if (team == ET_TEAM_ALLIES)
				return ET_WP_STEN;
			else
				return ET_WP_MP34;	
#endif
			} else if (rInt == 1){
#ifndef NOQUARTER
				if (team == ET_TEAM_ALLIES)
					return ET_WP_GARAND;
				else
					return ET_WP_K43;
#else			
			if (team == ET_TEAM_ALLIES)
				return ET_WP_BAR;
			else
				return ET_WP_FG42;
#endif
			} else {
				if (team == ET_TEAM_ALLIES)
					return ET_WP_GARAND;
				else
					return ET_WP_K43;
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

	Bot_Interface_SendEvent(MESSAGE_RESETWEAPONS, bot-g_entities, 0, 0, 0);

	for (weapon = WP_NONE + 1; weapon < WP_NUM_WEAPONS; weapon++)
	{
		if (COM_BitCheck(bot->client->ps.weapons, weapon))
		{
			botUserData.DataType = BotUserData::dtInt;
			botUserData.udata.m_Int = Bot_WeaponGameToBot(weapon);
			Bot_Interface_SendEvent(MESSAGE_ADDWEAPON, bot-g_entities, 0, 0, &botUserData);
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
			return G_LandmineTeam(_ent);
		// Let this fall through
	default:
		return Bot_TeamGameToBot(_ent->s.teamNum);
	}

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

	if( _ent->health <= 0 )
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

static void pfnPrintScreenMessage(const int _client, const float _pos[3], const float _color[3], const char *_msg)
{
	if(_msg)
		trap_SendServerCommand(_client, va("cp \"%s\"", _msg));
}

// core functions
static int pfnChangeTeam(int _client, int _newteam, const MessageHelper *_data)
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

	bot->client->sess.latchPlayerWeapon = _weaponBotToGame(
		_choosePriWeap(Bot_PlayerClassGameToBot(bot->client->sess.latchPlayerType), _newteam));

	//if (!SetTeam(bot, teamName, qtrue, -1, -1, qfalse)) {
	//	pfnPrintError("Bot could not join team!");
	//	return 0;
	//}

	// if SetTeam() fails, be sure to at least send a note to the bot about the current team
	// (else this won't be neccessary because on respawn messages will be sent automatically)
	if (!SetTeam(bot, teamName, qtrue, (weapon_t)-1, (weapon_t)-1, qfalse))
	{
		botUserData.DataType = BotUserData::dtInt;
		botUserData.udata.m_Int = Bot_TeamGameToBot(bot->client->sess.sessionTeam);
		Bot_Interface_SendEvent(MESSAGE_CHANGETEAM, bot-g_entities, 0,0, &botUserData);

		// also retransmit weapons stuff
		//ReTransmitWeapons(bot);
	}

	return 1;
}

static int pfnChangeClass(int _client, int _newclass, const MessageHelper *_data)
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
	if (team != ET_TEAM_NONE)
	{
		bot->client->sess.latchPlayerWeapon = _weaponBotToGame(_choosePriWeap(_newclass, team));
	}

	bot->client->sess.latchPlayerType = playerClassBotToGame(_newclass);

	// commit suicide to ensure new class is used
	if (bot->client->sess.latchPlayerType != bot->client->sess.playerType)
	{
		Cmd_Kill_f(bot);
	}
	else
	{
		// be sure to at least send a note to the bot about the current class
		// (else this won't be neccessary because on respawn messages will be sent automatically)
		botUserData.DataType = BotUserData::dtInt;
		botUserData.udata.m_Int = Bot_PlayerClassGameToBot(bot->client->sess.playerType);
		Bot_Interface_SendEvent(MESSAGE_CHANGECLASS, _client, 0,0, &botUserData);

		// also retransmit weapons stuff
		ReTransmitWeapons(bot);
	}

	return 1;
}

static int pfnAddBot(const char *_name, const MessageHelper *_data)
{
	int num;
	char userinfo[MAX_INFO_STRING] = {0};
	gentity_t* bot;
	char* s = 0, *pName = va("OMNIBOT%25s", _name);

	num = trap_BotAllocateClient(0);

	if (num < 0) 
	{
		pfnPrintError("Could not add bot!");
		pfnPrintError("No free slots!");
		return -1;
	}

	bot = &g_entities[num];

	Info_SetValueForKey(userinfo, "name", _name);
	Info_SetValueForKey(userinfo, "rate", "25000");
	Info_SetValueForKey(userinfo, "snaps", "20");
	Info_SetValueForKey(userinfo, "ip", "localhost");
	Info_SetValueForKey(userinfo, "cl_guid", pName);

	trap_SetUserinfo(num, userinfo);

	if ((s = ClientConnect(num, qtrue, qtrue)))
	{
		pfnPrintError(va("Could not connect bot: %s", s));
		return -1;
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

static int pfnTraceLine(BotTraceResult *_result,
						const float _start[3],
						const float _end[3],
						const AABB *_pBBox ,
						int _mask, int _user, obBool _bUsePVS)
{
	trace_t tr;
	int iMask = 0;

	if((_bUsePVS == False) || (qtrue == trap_InPVS(_start, _end)))
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
			if(_mask & TR_MASK_SMOKEBOMB)
			{
				gentity_t *pSmokeBlocker = Bot_EntInvisibleBySmokeBomb((float*)_start, (float*)_end);
				if(pSmokeBlocker)
				{
					_result->m_Fraction = 0.0f;
					_result->m_HitEntity = pSmokeBlocker;
					return 1;
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

		_result->m_iUser1 = tr.surfaceFlags;
		_result->m_iUser2 = tr.contents;

		// Fill in the bot traceflag.
		_result->m_Fraction = tr.fraction;
		_result->m_Endpos[0] = tr.endpos[0];
		_result->m_Endpos[1] = tr.endpos[1];
		_result->m_Endpos[2] = tr.endpos[2];
		_result->m_Normal[0] = tr.plane.normal[0];
		_result->m_Normal[1] = tr.plane.normal[1];
		_result->m_Normal[2] = tr.plane.normal[2];
	}
	else
	{
		// Not in PVS
		_result->m_Fraction = 0.0f;
		_result->m_HitEntity = 0;
	}
	return 1;
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
	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_ATTACK1))
		cmd.buttons |= BUTTON_ATTACK;

	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_WALK))
		cmd.buttons |= BUTTON_WALKING;
	else if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_SPRINT))
		cmd.buttons |= BUTTON_SPRINT;

	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_USE))
		cmd.buttons |= BUTTON_ACTIVATE;

	// wbuttons
	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_ATTACK2))
		cmd.wbuttons |= WBUTTON_ATTACK2;

	// if we have prone held
	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_PRONE))
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
	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_AIM))
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


	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_LEANLEFT))
		cmd.wbuttons |= WBUTTON_LEANLEFT;
	else if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_LEANRIGHT))
		cmd.wbuttons |= WBUTTON_LEANRIGHT;

	if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_RELOAD))
	{
		// watch for gpg40 and m7 as these need to be switched for reloading
		if (_input->m_CurrentWeapon == ET_WP_GPG40 || _input->m_CurrentWeapon == ET_WP_M7)
		{
			cmd.weapon = bot->client->sess.playerWeapon;
		} 
		else
		{
			cmd.wbuttons |= WBUTTON_RELOAD;
		}
	}

	// don't process view angles and moving stuff when dead
	if (bot->client->ps.pm_type == PM_NORMAL)
	{
		// Convert the bots vector to angles and set the view angle to the orientation
		vectoangles(_input->m_Facing, angles);
		SetClientViewAngle(bot, angles);

		// Convert the move direction into forward and right moves to
		// take the bots orientation into account.
		AngleVectors(angles, forward, right, NULL);
		cmd.forwardmove = (signed char)(DotProduct(forward, _input->m_MoveDir) * 127);
		cmd.rightmove = (signed char)(DotProduct(right, _input->m_MoveDir) * 127);

		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_FWD))
			cmd.forwardmove = 127;
		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_BACK))
			cmd.forwardmove = -127;
		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_RSTRAFE))
			cmd.rightmove = 127;
		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_LSTRAFE))
			cmd.rightmove = -127;
		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_JUMP))
			cmd.upmove = 127;
		if(BOT_CHECK_BUTTON(_input->m_ButtonFlags, BOT_BUTTON_CROUCH))
			cmd.upmove = -127;
	}

	trap_BotUserCommand(_client, &cmd);
}

static void pfnBotCommand(int _client, const char *_cmd)
{
	trap_EA_Command(_client, (char *)_cmd);
}

// client stuff
static int pfnGetClientPosition(int _client, float _pos[3])
{
	const gentity_t* bot = &g_entities[_client];
	if (bot->inuse)
	{
		_pos[0] = bot->r.currentOrigin[0];
		_pos[1] = bot->r.currentOrigin[1];
		_pos[2] = bot->r.currentOrigin[2];
		return 1;
	}
	return 0;
}

static int pfnGetClientOrientation(int _client, float _fwd[3], float _right[3], float _up[3])
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
		return 1;
	}
	return 0;
}

static const char *pfnGetClientName(int _client)
{
	if (g_entities[_client].inuse)
	{
		static char	userinfo[MAX_INFO_STRING] = {0};
		const char *pName = NULL;
		trap_GetUserinfo( _client, userinfo, sizeof(userinfo) );
		pName = Info_ValueForKey( userinfo, "name" );
		return pName;
	}
	return NULL;
}

// entity stuff
static int pfnGetEntityOrientation(const GameEntity _ent, float _fwd[3], float _right[3], float _up[3])
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
		return 1;
	}
	return 0;
}

static int pfnGetEntityVelocity(const GameEntity _ent, float _velocity[3])
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
		return 1;
	}
	return 0;
}

static int pfnGetEntityPosition(const GameEntity _ent, float _pos[3])
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		if(!pEnt->client)
		{
			// Need to do special case stuff for certain entities.
			if ((pEnt->s.eType == ET_CONSTRUCTIBLE) ||
				(pEnt->s.eType == ET_EXPLOSIVE) ||
				/*(pEnt->s.eType == ET_MOVER) ||*/
				/*(pEnt->s.eType == ET_CORPSE) ||*/
				!Q_stricmp(pEnt->classname, "trigger_multiple") || 
				!Q_stricmp(pEnt->classname, "func_commandpoint") ||
				!Q_stricmp(pEnt->classname, "plat_trigger") ||
				!Q_stricmp(pEnt->classname, "door_trigger"))
			{
				// Center of bounding box, cuz currentOrigin isn't valid.
				_pos[0] = pEnt->r.currentOrigin[0] + ((pEnt->r.maxs[0] + pEnt->r.mins[0]) * 0.5f);
				_pos[1] = pEnt->r.currentOrigin[1] + ((pEnt->r.maxs[1] + pEnt->r.mins[1]) * 0.5f);
				_pos[2] = pEnt->r.currentOrigin[2] + ((pEnt->r.maxs[2] + pEnt->r.mins[2]) * 0.5f);
				return 1;
			}
		}

		// Clients and entities not caught above will return normal position.
		_pos[0] = pEnt->r.currentOrigin[0];
		_pos[1] = pEnt->r.currentOrigin[1];
		_pos[2] = pEnt->r.currentOrigin[2];

		return 1;
	}
	return 0;
}

static int pfnGetEntityWorldAABB(const GameEntity _ent, AABB *_aabb)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
	if (pEnt && pEnt->inuse)
	{
		_aabb->m_Mins[0] = pEnt->r.absmin[0];
		_aabb->m_Mins[1] = pEnt->r.absmin[1];
		_aabb->m_Mins[2] = pEnt->r.absmin[2];
		_aabb->m_Maxs[0] = pEnt->r.absmax[0];
		_aabb->m_Maxs[1] = pEnt->r.absmax[1];
		_aabb->m_Maxs[2] = pEnt->r.absmax[2];
		return 1;
	}
	return 0;
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

static int obGetEntityFlags(const GameEntity _ent)
{
	int iFlags = 0;
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);

	if(pEnt && pEnt->inuse)
	{
		// Set any flags.
		if(pEnt->health <= 0)
			iFlags |= ENT_FLAG_DEAD;

		if(pEnt->waterlevel >= 3)
			iFlags |= ENT_FLAG_UNDERWATER;
		else if(pEnt->waterlevel > 0)
			iFlags |= ENT_FLAG_INWATER;
		if(pEnt->s.eFlags & EF_ZOOMING)
			iFlags |= ENT_FLAG_ZOOMING;

		if(pEnt->s.eFlags & EF_MG42_ACTIVE)
			iFlags |= (ET_ENT_FLAG_MNT_MG42 | ET_ENT_FLAG_MOUNTED);
		if(pEnt->s.eFlags & EF_MOUNTEDTANK)
			iFlags |= (ET_ENT_FLAG_MNT_TANK | ET_ENT_FLAG_MOUNTED);
		if(pEnt->s.eFlags & EF_AAGUN_ACTIVE)
			iFlags |= (ET_ENT_FLAG_MNT_AAGUN | ET_ENT_FLAG_MOUNTED);	

		if((pEnt->s.eType == ET_MOVER) && _TankIsMountable(pEnt))
		{
			iFlags |= ET_ENT_FLAG_ISMOUNTABLE;
			if(G_TankIsOccupied(pEnt))
				iFlags |= ET_ENT_FLAG_MOUNTED;
		}

		if(pEnt->client)
		{
			if(pEnt->client->ps.pm_flags & PMF_LADDER)
				iFlags |= ENT_FLAG_LADDER;
			if(pEnt->client->ps.eFlags & EF_PRONE)
				iFlags |= ENT_FLAG_PRONED;
			if(pEnt->client->ps.pm_flags & PMF_DUCKED)
				iFlags |= ENT_FLAG_CROUCHED;
			if (pEnt->client->ps.powerups[PW_OPS_DISGUISED])
				iFlags |= ET_ENT_FLAG_DISGUISED;
			if (pEnt->client->ps.powerups[PW_REDFLAG] || pEnt->client->ps.powerups[PW_BLUEFLAG])
				iFlags |= ET_ENT_FLAG_CARRYINGGOAL;
			if(pEnt->client->ps.pm_flags & PMF_LIMBO)
				iFlags |= ET_ENT_FLAG_INLIMBO;
		}		
	}
	return iFlags;
}

static int obGetEntityPowerups(const GameEntity _ent)
{
	int iPowerUp = 0;

	return iPowerUp;
}

static int obGetEntityEyePosition(const GameEntity _ent, float _pos[3])
{
	if(pfnGetEntityPosition(_ent, _pos))
	{
		gentity_t *pEnt = _GetEntityFromGameEntity(_ent);
		if(pEnt && pEnt->client)
		{
			_pos[2] += pEnt->client->ps.viewheight;
		}
		return 1;
	}
	return 0;
}

static int obGetEntityBonePosition(const GameEntity _ent, int _boneid, float _pos[3])
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

// search stuff
static int pfnPrintEntitiesInRadius(const float _pos[3], float _radius)
{
	gentity_t *check;
	vec3_t toent;
	float sqRad = _radius*_radius;
	int i;
	char buffer[64];

	sprintf(buffer, "Entities within %f radius", _radius);
	g_BotFunctions.pfnBotLog(buffer);
	pfnPrintMessage(buffer);

	for(i = 0; i < level.num_entities; ++i)
	{
		check = &g_entities[i];

		if (!check->inuse)
			continue;

		// Is it within the specified radius
		VectorSubtract(_pos, check->r.currentOrigin, toent);
		if(VectorLengthSquared(toent) > sqRad)
			continue;

		// Interpret the entity into the appropriate flags for the bot.
		switch (check->s.eType)
		{
		case ET_GENERAL:
			g_BotFunctions.pfnBotLog("ET_GENERAL");
			pfnPrintMessage("ET_GENERAL");
			break;
		case ET_PLAYER:
			g_BotFunctions.pfnBotLog("ET_PLAYER");
			pfnPrintMessage("ET_PLAYER");
			break;
		case ET_ITEM:
			g_BotFunctions.pfnBotLog("ET_ITEM");
			pfnPrintMessage("ET_ITEM");
			break;
		case ET_MISSILE:
			g_BotFunctions.pfnBotLog("ET_MISSILE");
			pfnPrintMessage("ET_MISSILE");
			break;
		case ET_MOVER:
			g_BotFunctions.pfnBotLog("ET_MOVER");
			pfnPrintMessage("ET_MOVER");
			break;
		case ET_BEAM:
			g_BotFunctions.pfnBotLog("ET_BEAM");
			pfnPrintMessage("ET_BEAM");
			break;
		case ET_PORTAL:
			g_BotFunctions.pfnBotLog("ET_PORTAL");
			pfnPrintMessage("ET_PORTAL");
			break;
		case ET_SPEAKER:
			g_BotFunctions.pfnBotLog("ET_SPEAKER");
			pfnPrintMessage("ET_SPEAKER");
			break;
		case ET_PUSH_TRIGGER:
			g_BotFunctions.pfnBotLog("ET_PUSH_TRIGGER");
			pfnPrintMessage("ET_PUSH_TRIGGER");
			break;
		case ET_TELEPORT_TRIGGER:
			g_BotFunctions.pfnBotLog("ET_TELEPORT_TRIGGER");
			pfnPrintMessage("ET_TELEPORT_TRIGGER");
			break;
		case ET_INVISIBLE:
			g_BotFunctions.pfnBotLog("ET_INVISIBLE");
			pfnPrintMessage("ET_INVISIBLE");
			break;
		default:
			break;
		}
		g_BotFunctions.pfnBotLog(check->classname);
		pfnPrintMessage(check->classname);
	}
	return 1;
}

static GameEntity pfnFindEntityByClassName(GameEntity _pStart, const char *_name)
{
	gentity_t *pStartEnt = _GetEntityFromGameEntity(_pStart);
	return (GameEntity)G_Find(pStartEnt, FOFS(classname), _name);
}

static GameEntity pfnFindEntityInSphere(const float _pos[3], float _radius,
										GameEntity _pStart, const char *_name)
{
	// square it to avoid the square root in the distance check.
	gentity_t *pStartEnt = _GetEntityFromGameEntity(_pStart);
	float fSqRad = _radius * _radius;
	vec3_t toent;

	while((pStartEnt = G_Find(pStartEnt, FOFS(classname), _name)) != NULL)
	{
		VectorSubtract(_pos, pStartEnt->r.currentOrigin, toent);
		if(VectorLengthSquared(toent) < fSqRad)
			break;
	}
	return (GameEntity)pStartEnt;
}

// weapon stuff
static int pfnBotGetCurrentAmmo(int _client, int _ammotype, int *_cur, int *_max) 
{
	gentity_t *bot = &g_entities[_client];
	int maxclip;

	if (bot->inuse)
	{
		_ammotype = _weaponBotToGame(_ammotype);

		*_cur = bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)_ammotype)] +
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
		// jaquboss - NQ stuff
//#ifdef NOQAURTER
		case WP_BAZOOKA:
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_FIREBOLT:
		case WP_KNIFE_KABAR:
//#endif

			maxclip = 0;
			break;
		default:
			maxclip = GetAmmoTableData(_ammotype)->maxclip;
		}

		*_max = maxclip + BG_MaxAmmoForWeapon((weapon_t)_ammotype, bot->client->sess.skill);

		return 1;
	}
	return 0;
}

static int pfnBotGetCurrentWeaponClip(int _client, int *_curclip, int *_maxclip) 
{
	gentity_t *bot = &g_entities[_client];

	if(bot->inuse && bot->client)
	{
		int iWeapon = bot->client->ps.weapon;

		*_curclip = bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)iWeapon)];

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
		// jaquboss - NQ stuff
//#ifdef NOQAURTER
		case WP_BAZOOKA:
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_FIREBOLT:
		case WP_KNIFE_KABAR:
//#endif
			*_maxclip = 0;
			break;
		default:
			*_maxclip = GetAmmoTableData(iWeapon)->maxclip;
		}
		return 1;
	}
	return 0;
}

// game stuff
static int pfnGetGameTime() 
{
	return level.time;
}

static const char *_GetEntityName(gentity_t *_ent)
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

void Bot_Util_EntityCreated(GameEntity _ent)
{
	gentity_t *pEnt = (gentity_t*)_ent;

	const char *pGoalName = _GetEntityName(pEnt);

	if (!g_BotLoaded)
		return;

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
					sprintf(buffer, "%s", pGoalName ? pGoalName : "allies_flag");
					g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, ET_GOAL_FLAG, (1 << ET_TEAM_ALLIES), buffer, NULL);
				}
				else if(!Q_stricmp(pEnt->classname, "team_CTF_blueflag"))
				{
					// axis flag
					sprintf(buffer, "%s", pGoalName ? pGoalName : "axis_flag");
					g_BotFunctions.pfnBotAddGoal((GameEntity)pEnt, ET_GOAL_FLAG, (1 << ET_TEAM_AXIS), buffer, NULL);
				}
				break;
			}
		default:
			break;
		}
	}	
}

static int pfnGetGoals() 
{
	int i;
	for (i = MAX_CLIENTS; i < level.num_entities; i++) 
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
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_FLAG, (1 << ET_TEAM_ALLIES), pGoalName, NULL);
				}
				else if(!Q_stricmp(e->classname, "team_CTF_blueflag"))
				{
					// axis flag
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_FLAG, (1 << ET_TEAM_AXIS), pGoalName, NULL);
				}
				break;
			}
		case ET_OID_TRIGGER:
			{
				// detect constructibles
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
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_CONSTRUCTION, 
						(1 << ET_TEAM_AXIS), pAxisGoalName, NULL);
				}

				if(eAlly)
				{
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_CONSTRUCTION, 
						(1 << ET_TEAM_ALLIES), pAllyGoalName, NULL);
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

				if (e->target_ent->s.eType == ET_CONSTRUCTIBLE)
				{
					BotUserData bud;
					bud.DataType = BotUserData::dtInt;
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
				team |= (1 << ET_TEAM_ALLIES);
				team |= (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e);
				if(pGoalName && !Q_stricmp(e->classname, "script_mover") && (e->health > 0))
				{
					g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_VEHICLE, team, pGoalName, NULL);
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
				//BotUserData bud(e->damage); // heal rate
				int team = 0;
				team |= (1 << ET_TEAM_ALLIES);
				team |= (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : _GetEntityName(e);
				g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_HEALTH_CAB, team, pGoalName, NULL);
				break;
			}
		case ET_SUPPLIER:
			{
				//BotUserData bud(e->damage); // heal rate
				int team = 0;
				team |= (1 << ET_TEAM_ALLIES);
				team |= (1 << ET_TEAM_AXIS);
				pGoalName = _GetEntityName(e->target_ent) ? _GetEntityName(e->target_ent) : _GetEntityName(e);
				g_BotFunctions.pfnBotAddGoal((GameEntity)e, ET_GOAL_AMMO_CAB, team, pGoalName, NULL);
				break;
			}
		default:
			break;
		}
	}

	return 1;
}

static int pfnGetCurNumPlayers() { 
	//return level.numNonSpectatorClients;
	return level.numConnectedClients; 
}

static int pfnGetMaxNumPlayers() { 
	return level.maxclients; 
}

static obBool pfnInterfaceSendMessage(const MessageHelper &_data, const GameEntity _ent)
{
	gentity_t *pEnt = _GetEntityFromGameEntity(_ent);

	switch(_data.GetMessageId())
	{
		// general messages
	case GEN_MSG_ISALIVE:
		{
			Msg_IsAlive *pMsg = _data.Get<Msg_IsAlive>();
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
			Msg_Reloading *pMsg = _data.Get<Msg_Reloading>();
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
			Msg_ReadyToFire *pMsg = _data.Get<Msg_ReadyToFire>();
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
			Msg_EquippedWeapon *pMsg = _data.Get<Msg_EquippedWeapon>();
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
					pMsg->m_Weapon = Bot_WeaponGameToBot(pEnt->client->ps.weapon);
				else
					pMsg->m_Weapon = 0;
			}				
			break;
		}
	case GEN_MSG_GETCURRENTCLASS:
		{
			Msg_EntityClass *pMsg = _data.Get<Msg_EntityClass>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client)
					pMsg->m_Class = Bot_PlayerClassGameToBot(pEnt->client->sess.playerType);
				else
					pMsg->m_Class = 0;
			}				
			break;
		}
	case GEN_MSG_GETCURRENTTEAM:
		{
			Msg_EntityTeam *pMsg = _data.Get<Msg_EntityTeam>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client)
					pMsg->m_Team = Bot_TeamGameToBot(pEnt->client->sess.sessionTeam);
				else
					pMsg->m_Team = 0;
			}				
			break;
		}
	case GEN_MSG_GETHEALTHARMOR:
		{
			Msg_PlayerHealthArmor *pMsg = _data.Get<Msg_PlayerHealthArmor>();
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
	case GEN_MSG_ISALLIED:
		{
			Msg_IsAllied *pMsg = _data.Get<Msg_IsAllied>();
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
	case GEN_MSG_ISHUMAN:
		{
			Msg_IsHuman *pMsg = _data.Get<Msg_IsHuman>();
			if(pMsg)
			{					
				if(pEnt && pEnt->inuse && pEnt->client && !(pEnt->r.svFlags & SVF_BOT))
					pMsg->m_IsHuman = True;
				else
					pMsg->m_IsHuman = False;
			}				
			break;
		}
	case GEN_MSG_GETPOINTCONTENTS:
		{
			Msg_PointContents *pMsg = _data.Get<Msg_PointContents>();
			if(pMsg)
			{
				vec3_t vec = { pMsg->x, pMsg->y, pMsg->z };
				trace_t tr;
				trap_Trace(&tr, vec, NULL, NULL, vec, pEnt ? pEnt->s.number : ENTITYNUM_NONE, 
					CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_FOG);

				// Pack the contents into the results.
				if(tr.contents & CONTENTS_SOLID)
					pMsg->m_Contents |= CONT_SOLID;
				if(tr.contents & CONTENTS_WATER)
					pMsg->m_Contents |= CONT_WATER;
				if(tr.contents & CONTENTS_SLIME)
					pMsg->m_Contents |= CONT_SLIME;
				if(tr.contents & CONTENTS_FOG)
					pMsg->m_Contents |= CONT_FOG;
				if(tr.contents & CONTENTS_TELEPORTER)
					pMsg->m_Contents |= CONT_TELEPORTER;
				if(tr.contents & CONTENTS_MOVER)
					pMsg->m_Contents |= CONT_MOVER;
				if(tr.contents & CONTENTS_TRIGGER)
					pMsg->m_Contents |= CONT_TRIGGER;
				if(tr.contents & CONTENTS_LAVA)
					pMsg->m_Contents |= CONT_LAVA;
			}
			break;
		}
	case GEN_MSG_GAMESTATE:
		{
			Msg_GameState *pMsg = _data.Get<Msg_GameState>();
			if(pMsg)
			{
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
		// ET specific messages
		//case ET_MSG_PLANTDYNAMITE:
		//	{
		//		gentity_t *pEntOther = _GetEntityFromGameEntity(_in->udata.m_Entity);
		//		vec3_t tPos, dir;
		//		assert(_in->m_DataType == BotUserData::dtEntity);

		//		_out->m_DataType = BotUserData::dtEntity;

		//		if (pEnt && pEnt->inuse && pEntOther && pEntOther->inuse)
		//		{
		//			VectorAdd(pEntOther->r.absmax, pEntOther->r.absmin, tPos);
		//			VectorScale(tPos, 0.5f, tPos);
		//			VectorSubtract(tPos, pEnt->r.currentOrigin, dir);

		//			_out->udata.m_Entity = (GameEntity)fire_grenade(pEnt, pEnt->r.currentOrigin, dir, WP_DYNAMITE);

		//			// update energy bar
		//			// taken from FireWeapon()
		//			if (level.time - pEnt->client->ps.classWeaponTime >
		//				level.engineerChargeTime[pEnt->client->sess.sessionTeam-1])
		//			{
		//				pEnt->client->ps.classWeaponTime = level.time -
		//					level.engineerChargeTime[pEnt->client->sess.sessionTeam-1];
		//			}
		//			if (pEnt->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3)
		//			{
		//				pEnt->client->ps.classWeaponTime += .66f *
		//					level.engineerChargeTime[pEnt->client->sess.sessionTeam-1];
		//			}
		//			else
		//			{
		//				pEnt->client->ps.classWeaponTime = level.time;
		//			}
		//		}
		//		else
		//		{
		//			_out->udata.m_Entity = NULL;
		//		}
		//		break;
		//	}	
		//case ET_MSG_PLANTMINE:
		//	{
		//		_out->m_DataType = BotUserData::dtEntity;

		//		if (pEnt && pEnt->inuse)
		//		{
		//			vec3_t dir;
		//			pfnGetEntityOrientation(pEnt, dir, NULL, NULL);					
		//			_out->udata.m_Entity = (GameEntity)fire_grenade(pEnt, 
		//				pEnt->r.currentOrigin, 
		//				dir, WP_LANDMINE);

		//			// update energy bar
		//			// taken from FireWeapon()
		//			if (level.time - pEnt->client->ps.classWeaponTime >
		//				level.engineerChargeTime[pEnt->client->sess.sessionTeam-1])
		//			{
		//				pEnt->client->ps.classWeaponTime = level.time -
		//					level.engineerChargeTime[pEnt->client->sess.sessionTeam-1];
		//			}
		//			if (pEnt->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3)
		//			{
		//				pEnt->client->ps.classWeaponTime += .66f *
		//					level.engineerChargeTime[pEnt->client->sess.sessionTeam-1];
		//			}
		//			else
		//			{
		//				pEnt->client->ps.classWeaponTime = level.time;
		//			}
		//		}
		//		else
		//		{
		//			_out->udata.m_Entity = NULL;
		//		}
		//		break;
		//	}	
	case ET_MSG_GOTOLIMBO:
		{
			ET_GoLimbo *pMsg = _data.Get<ET_GoLimbo>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client &&
					pEnt->client->ps.pm_type == PM_DEAD &&
					!(pEnt->client->ps.eFlags & EF_PLAYDEAD) &&
					!(pEnt->client->ps.pm_flags & PMF_LIMBO))
				{
					/*static usercmd_t cmd;

					memset(&cmd, 0, sizeof(cmd));
					cmd.identClient = pEnt->s.clientNum;
					cmd.serverTime = level.time;
					cmd.upmove = -127;
					trap_BotUserCommand(pEnt->s.clientNum, &cmd);*/
					limbo( pEnt, qtrue ); // this is damn cheap!!
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
			ET_MedicNear *pMsg = _data.Get<ET_MedicNear>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client &&
					pEnt->client->ps.pm_type == PM_DEAD &&
					!(pEnt->client->ps.eFlags & EF_PLAYDEAD) &&
					!(pEnt->client->ps.pm_flags & PMF_LIMBO))
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
			ET_WaitingForMedic *pMsg = _data.Get<ET_WaitingForMedic>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client &&
					(pEnt->health <= 0 || pEnt->client->ps.pm_type == PM_DEAD) &&
					pEnt->client->ps.eFlags & EF_PLAYDEAD &&
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
			ET_ReinforceTime *pMsg = _data.Get<ET_ReinforceTime>();
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
			ET_MG42Health *pMsg = _data.Get<ET_MG42Health>();
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_MG42Entity);
				if (pGunEntity && pGunEntity->inuse && pGunEntity->r.linked && 
					pGunEntity->entstate == STATE_DEFAULT)
				{						
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
			ET_WeaponHeatLevel *pMsg = _data.Get<ET_WeaponHeatLevel>();
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_Entity);
				if(pEnt && pEnt->client && pGunEntity)
				{
					pMsg->m_Current = pEnt->client->ps.weapHeat[WP_DUMMY_MG42];
					pMsg->m_Max = (int)MAX_MG42_HEAT;
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
			ET_MG42MountedPlayer *pMsg = _data.Get<ET_MG42MountedPlayer>();
			if(pMsg)
			{
				gentity_t *pGunEntity = _GetEntityFromGameEntity(pMsg->m_MG42Entity);
				if (pGunEntity && pGunEntity->inuse && (pGunEntity->r.ownerNum < level.maxclients))
				{
					pMsg->m_MountedEntity = &g_entities[pGunEntity->r.ownerNum];
				}
				else
				{
					pMsg->m_MountedEntity = 0;
				}
			}				
			break;
		}		
	case ET_MSG_MOUNTEDMG42INFO:
		{
			ET_MG42Info *pMsg = _data.Get<ET_MG42Info>();
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
	case ET_MSG_WPCHARGED:
		{
			ET_WeaponCharged *pMsg = _data.Get<ET_WeaponCharged>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client)
				{
					pMsg->m_IsCharged = 
						(weaponCharged(&pEnt->client->ps,pEnt->client->sess.sessionTeam,
						_weaponBotToGame(pMsg->m_Weapon), pEnt->client->sess.skill) == qtrue) ? True : False;
				}
			}
			break;
		}
	case ET_MSG_WPOVERHEATED:
		{				
			ET_WeaponOverheated *pMsg = _data.Get<ET_WeaponOverheated>();
			if(pMsg)
			{
				if (pEnt && pEnt->inuse && pEnt->client)
				{
					int iCurHeat = pEnt->client->ps.weapHeat[_weaponBotToGame(pMsg->m_Weapon)];
					int iMaxHeat = GetAmmoTableData(_weaponBotToGame(pMsg->m_Weapon))->maxHeat;
					pMsg->m_IsOverheated = iMaxHeat ? ((iCurHeat >= iMaxHeat) ? True : False) : False;
				}
			}				
			break;
		}
		//case ET_MSG_WPGRENSPEED:
		//	{
		//		float upangle, pitch;
		//		assert(_in == NULL);

		//		_out->m_DataType = BotUserData::dtFloat;

		//		if (pEnt && pEnt->inuse)
		//		{
		//			// calculation taken from g_weapon.c
		//			pitch = pEnt->s.apos.trBase[0];
		//			if( pitch >= 0 ) {
		//				pitch = 1.3f;
		//			}
		//			else {
		//				pitch = -pitch;
		//				pitch = min( pitch, 30 );
		//				pitch /= 30.f;
		//				pitch = 1 - pitch;
		//				pitch *= 0.3f;
		//				pitch += 1.f;
		//			}
		//			upangle = -(pEnt->s.apos.trBase[0]);
		//			upangle = min(upangle, 50);
		//			upangle = max(upangle, -50);
		//			upangle = upangle/100.0f;
		//			upangle += 0.5f;
		//			if(upangle < .1)
		//				upangle = .1;
		//			upangle *= 900;
		//			upangle *= pitch;
		//			_out->udata.m_Float = upangle;
		//		}
		//		else
		//		{
		//			_out->udata.m_Float = 0;
		//		}
		//		break;
		//	}
	case ET_MSG_PICKWEAPON:
	case ET_MSG_PICKWEAPON2:
		{
			ET_SelectWeapon *pMsg = _data.Get<ET_SelectWeapon>();
			if(pMsg)
			{
				if(pEnt && pEnt->inuse && pEnt->client)
				{						
					if(_data.GetMessageId() == ET_MSG_PICKWEAPON2)
						pEnt->client->sess.latchPlayerWeapon2 = _weaponBotToGame(pMsg->m_Selection);
					else
						pEnt->client->sess.latchPlayerWeapon = _weaponBotToGame(pMsg->m_Selection);
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
			ET_CursorHint *pMsg = _data.Get<ET_CursorHint>();
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
	case ET_MSG_CHANGESPAWNPOINT:
		{
			ET_SpawnPoint *pMsg = _data.Get<ET_SpawnPoint>();
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
			ET_HasFlag *pMsg = _data.Get<ET_HasFlag>();
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
			ET_ConstructionState *pMsg = _data.Get<ET_ConstructionState>();
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
			ET_Destroyable *pMsg = _data.Get<ET_Destroyable>();
			if(pMsg)
			{
				gentity_t *pDestroyable = _GetEntityFromGameEntity(pMsg->m_Entity);
				if (pEnt && pEnt->inuse && pDestroyable && pDestroyable->inuse)
				{
					if (pDestroyable->s.eType == ET_CONSTRUCTIBLE)
					{	
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
			ET_ExplosiveState *pMsg = _data.Get<ET_ExplosiveState>();
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
			ET_CanBeGrabbed *pMsg = _data.Get<ET_CanBeGrabbed>();
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
			ET_TeamMines *pMsg = _data.Get<ET_TeamMines>();
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
			ET_CabinetData *pMsg = _data.Get<ET_CabinetData>();
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
					return False;
				}
			}
			break;
		}
	case ET_MSG_SKILLLEVEL:
		{
			if(pEnt && pEnt->client)
			{
				ET_PlayerSkills *pMsg = _data.Get<ET_PlayerSkills>();
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
				return False;
			}
			break;
		}
	default:
		{
			const char *pErrorMsg = va("Unknown Omni-bot Message '%i'", _data.GetMessageId());
			pfnPrintError(pErrorMsg);
			g_BotFunctions.pfnBotLog(pErrorMsg); 
			assert(1 && "Invalid Interface Message");
		}
		return False;
	}
	return True;
}

static int pfnGetThreats() 
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

		// default data.
		info.m_EntityFlags = 0;
		info.m_EntityCategory = 0;
		info.m_EntityClass = ET_CLASS_NULL;
		info.m_UserData.DataType = BotUserData::dtNone;

		switch(pCurrent->s.eType)
		{
		case ET_GENERAL:
			{
				if(!Q_stricmp(pCurrent->classname, "func_invisible_user")) 
				{
					// The damage flags tells us the type.
					switch(pCurrent->s.dmgFlags)
					{
					case HINT_BUTTON:
						info.m_EntityCategory = ENT_CAT_TRIGGER | ENT_CAT_STATIC;
						info.m_EntityClass = ENT_CLASS_GENERIC_BUTTON;
						break;
					default:
						continue;
					}
				}
				else if(!Q_stricmp(pCurrent->classname, "func_button")) 
				{
					info.m_EntityCategory = ENT_CAT_TRIGGER | ENT_CAT_STATIC;
					info.m_EntityClass = ENT_CLASS_GENERIC_BUTTON;
					// continue for now so it doesnt get regged
					//continue;
				}
				continue;
			}
		case ET_PLAYER:
			{
				if (!pCurrent->client || (pCurrent->entstate == STATE_INVISIBLE) ||
					(pCurrent->client->ps.pm_type == PM_SPECTATOR) ||
					(pCurrent->client->sess.sessionTeam != TEAM_AXIS &&
					pCurrent->client->sess.sessionTeam != TEAM_ALLIES))
					continue;

				// Special case for dead players that haven't respawned.
				if(pCurrent->health <= 0)
				{
					info.m_EntityClass = ET_CLASSEX_CORPSE;
					info.m_EntityCategory = ENT_CAT_MISC;
					break;
				}

				// Set the entity class.
				info.m_EntityClass = Bot_PlayerClassGameToBot(pCurrent->client->sess.playerType);
				// Set the entity category
				info.m_EntityCategory = ENT_CAT_SHOOTABLE | ENT_CAT_PLAYER;
				// Set any flags.
				info.m_EntityFlags = obGetEntityFlags(pCurrent);
				break;
			}

		case ET_ITEM:
			{				
				if(!Q_strncmp(pCurrent->classname, "item_health", strlen("item_health"))) 
				{
					info.m_EntityCategory = ENT_CAT_PICKUP;
					info.m_EntityClass = ENT_CLASS_GENERIC_HEALTH;
				}
				else if(!Q_strncmp(pCurrent->classname, "weapon_magicammo", strlen("weapon_magicammo"))) 
				{
					info.m_EntityCategory = ENT_CAT_PICKUP;
					info.m_EntityClass = ENT_CLASS_GENERIC_AMMO;
				}
				else if(!Q_stricmp(pCurrent->classname, "item_treasure")) 
				{
					info.m_EntityCategory = ENT_CAT_PICKUP;
					info.m_EntityClass = ET_CLASSEX_TREASURE;
				}
				else
				{
					continue;
				}
				break;
			}
		case ET_CORPSE:
			{
				info.m_EntityClass = ET_CLASSEX_CORPSE;
				info.m_EntityCategory = ENT_CAT_MISC;

				if(pCurrent->s.eType == ET_CORPSE)
				{
					if(BODY_TEAM(pCurrent) >= 4)
						info.m_EntityFlags |= ENT_FLAG_DISABLED;
					else if(BODY_VALUE(pCurrent) >= 250)
						info.m_EntityFlags |= ENT_FLAG_DISABLED;
				}
				// for now don't reg
				continue;
			}
		case ET_MISSILE:
			{
				// Register certain weapons as threats to avoid or whatever.
				switch(pCurrent->s.weapon) 
				{
				case WP_GRENADE_PINEAPPLE:
					info.m_EntityClass = ET_CLASSEX_GRENADE;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_PANZERFAUST:
					info.m_EntityClass = ET_CLASSEX_ROCKET;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_ARTY:
					info.m_EntityClass = ET_CLASSEX_ARTY;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_DYNAMITE:
					info.m_EntityClass = ET_CLASSEX_DYNAMITE;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_SMOKE_MARKER:
					info.m_EntityClass = ET_CLASSEX_SMOKEMARKER;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_SMOKE_BOMB:
					// Add the smoke grenade to the cache.
					if(g_NumSmokeGrenades < MAX_CACHE-1)
						g_SmokeGrenadeCache[g_NumSmokeGrenades++] = pCurrent;

					info.m_EntityClass = ET_CLASSEX_SMOKEBOMB;
					info.m_EntityCategory = /*ENT_CAT_AVOID |*/ ENT_CAT_PROJECTILE;
					break;
				case WP_LANDMINE:
					info.m_EntityClass = ET_CLASSEX_MINE;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_SATCHEL:
					info.m_EntityClass = ET_CLASSEX_SATCHEL;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_M7:
					info.m_EntityClass = ET_CLASSEX_M7_GRENADE;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_GPG40:
					info.m_EntityClass = ET_CLASSEX_GPG40_GRENADE;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				case WP_MORTAR_SET:
					info.m_EntityClass = ET_CLASSEX_MORTAR;
					info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
					break;
				default:
					if(!Q_strncmp(pCurrent->classname, "air strike", sizeof("air strike")))
					{
						info.m_EntityClass = ET_CLASSEX_AIRSTRIKE;
						info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
						break;
					}
					continue;
				}
				break;
			}
		case ET_FLAMETHROWER_CHUNK:
			{
				info.m_EntityClass = ET_CLASSEX_FLAMECHUNK;
				info.m_EntityCategory = ENT_CAT_AVOID | ENT_CAT_PROJECTILE;
				break;
			}
		case ET_MOVER:
			{
				if (!Q_stricmp(pCurrent->classname, "script_mover") && (pCurrent->spawnflags & 4))
				{
					info.m_EntityClass = ET_CLASSEX_VEHICLE;
					info.m_EntityCategory = ENT_CAT_MOVER;
					if(pCurrent->health > 0)
					{
						info.m_EntityCategory |= ET_ENT_FLAG_EXPLOSIVEDAMAGEONLY;
						info.m_EntityCategory |= ENT_CAT_SHOOTABLE;
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
				{
					continue;
				}
				break;
			}
		case ET_MG42_BARREL:
			{
				if((pCurrent->health > 0) &&
					(pCurrent->entstate != STATE_INVISIBLE) && 
					(pCurrent->entstate != STATE_UNDERCONSTRUCTION))
				{
					info.m_EntityClass = ET_CLASSEX_MG42MOUNT;
					info.m_EntityCategory = ENT_CAT_MOUNTEDWEAPON | ENT_CAT_SHOOTABLE;					
				}
				else
					continue;
				break;
			}
		case ET_AAGUN:
			{
				/*if((pCurrent->health > 0) &&
				(pCurrent->entstate != STATE_INVISIBLE) && 
				(pCurrent->entstate != STATE_UNDERCONSTRUCTION))
				{
				}*/

				continue;
			}
		case ET_EXPLOSIVE:
			{
				if (!(pCurrent->spawnflags & EXPLOSIVE_TANK) &&
					(pCurrent->constructibleStats.weaponclass != 1) &&
					(pCurrent->constructibleStats.weaponclass != 2) &&
					(pCurrent->health > 0) && (pCurrent->takedamage == qtrue))
				{
					info.m_EntityClass = ET_CLASSEX_BREAKABLE;
					info.m_EntityCategory = ENT_CAT_SHOOTABLE;
				}
				else
				{
					continue;
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
				continue;
				//}
				//break;
			}
		case ET_HEALER:
			{
				info.m_EntityCategory = ENT_CAT_PICKUP;
				info.m_EntityClass = ET_CLASSEX_HEALTHCABINET;
				break;
			}
		case ET_SUPPLIER:
			{
				info.m_EntityCategory = ENT_CAT_PICKUP;
				info.m_EntityClass = ET_CLASSEX_AMMOCABINET;
				break;
			}

		default:
			continue; // ignore this type.
		};

		if(info.m_EntityClass != ET_CLASS_NULL)
		{
			g_BotFunctions.pfnBotAddThreatEntity((GameEntity)pCurrent, &info);
		}
	}

	return 1;
}

// waypoint stuff

// uses G_Spawn() behaviour to count free entities
static int _CountFreeEntities()
{
	int i, count = 0;

	for (i = MAX_CLIENTS; i < ENTITYNUM_MAX_NORMAL; i++)
	{
		if (g_entities[i].inuse == qfalse)
			count++;
	}
	return count;
}

static void _AddDebugLineToDraw(obLineType _type, const float _start[3], const float _end[3], const float _color[3])
{
	debugLines_t line;
	line.type = _type;
	line.start[0] = _start[0];
	line.start[1] = _start[1];
	line.start[2] = _start[2];
	line.end[0] = _end[0];
	line.end[1] = _end[1];
	line.end[2] = _end[2];
	line.color[0] = _color[0];
	line.color[1] = _color[1];
	line.color[2] = _color[2];
	line.drawme = qfalse;
	line.dist = 0.f;

	if(_type == LINE_BLOCKABLE)
	{		
		AddToLineList(&g_BlockableDebugLines, &line);
	}
	else
	{
		AddToLineList(&g_DebugLines, &line);
	}	
}

static void pfnAddDisplayLine(int _type, const float _start[3], const float _end[3], const float _color[3])
{
	switch(_type)
	{
	case LINE_NORMAL:
		{
			_AddDebugLineToDraw(LINE_NORMAL, _start, _end, _color);
			break;
		}
	case LINE_WAYPOINT:
		{
			float _startPos[3] = { _start[0], _start[1], _start[2] - 24.0f };
			float _endPos[3] = { _start[0], _start[1], _start[2] + 40.0f };
			_AddDebugLineToDraw(LINE_WAYPOINT, _startPos, _endPos, _color);
			break;
		}
	case LINE_PATH:
		{
			float _startPos[3] = { _start[0], _start[1], _start[2] + 36.0f };
			float _endPos[3] = { _end[0], _end[1], _end[2] + 36.0f };
			_AddDebugLineToDraw(LINE_PATH, _startPos, _endPos, _color);
			break;
		}
	case LINE_BLOCKABLE:
		{
			_AddDebugLineToDraw(LINE_BLOCKABLE, _start, _end, _color);
			break;
		}
	}
}

static void pfnAddDisplayRadius(const float _pos[3], const float _radius, const float _color[3])
{
	float p1Pos[3];
	if(pfnGetClientPosition(0, p1Pos))
	{
		if(trap_InPVS(_pos, p1Pos) && _CountFreeEntities() > 100)
		{
			gentity_t *tent = G_TempEntity((float*)_pos, EV_BOT_DEBUG_RADIUS);
			tent->s.clientNum = 0;

			tent->s.angles2[0] = _color[0];
			tent->s.angles2[1] = _color[1];
			tent->s.angles2[2] = _color[2];

			tent->s.origin2[0] = _radius; // radius
			tent->s.origin2[1] = 9.0f; // number of segments
			tent->s.origin2[2] = 0.0f;
		}
	}
}

static void pfnAddTempDisplayLine(const float _start[3], const float _end[3], const float _color[3])
{
	float p1Pos[3];
	if(pfnGetClientPosition(0, p1Pos) && _CountFreeEntities() > 100)
	{
		if(trap_InPVS(_start, p1Pos) || trap_InPVS(_end, p1Pos))
		{
			gentity_t *tent = G_TempEntity((float*)_end, EV_BOT_DEBUG_LINE);
			tent->s.clientNum = 0;

			tent->s.angles2[0] = _color[0];
			tent->s.angles2[1] = _color[1];
			tent->s.angles2[2] = _color[2];

			VectorCopy(_start, tent->s.origin2);
		}
	}	
}

static void pfnClearDebugLines(int _wpview, int _clearNav, int _clearRadius)
{
	// If waypoint viewing is on, but we have no buffer...
	if((_wpview == 1) && !g_DebugLines.m_pDebugLines)
	{
		InitLineList(&g_DebugLines, OMNIBOT_LINES_INITIAL_SIZE);
		InitLineList(&g_BlockableDebugLines, OMNIBOT_LINES_INITIAL_SIZE / 4);
	}

	// If waypoint viewing is off, and we have a buffer, free it.
	if((_wpview == 0) && g_DebugLines.m_pDebugLines)
	{
		ClearLineList(&g_DebugLines, qtrue);
		ClearLineList(&g_BlockableDebugLines, qtrue);
	}

	if(_clearNav)
	{
		ClearLineList(&g_DebugLines, qfalse);
		ClearLineList(&g_BlockableDebugLines, qfalse);
	}
}

// info stuff
static const char *pfnGetMapName() 
{
	return level.rawmapname;
}

static void pfnGetMapExtents(AABB *_aabb)
{
	if(level.mapcoordsValid)
	{
		_aabb->m_Mins[0] = level.mapcoordsMins[0];
		_aabb->m_Mins[1] = level.mapcoordsMins[1];
		_aabb->m_Mins[2] = 0.0f;

		_aabb->m_Maxs[0] = level.mapcoordsMaxs[0];
		_aabb->m_Maxs[1] = level.mapcoordsMaxs[1];
		_aabb->m_Maxs[2] = 0.0f;
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
	// TODO:
	return 0;
}

// helper stuff
static void _UpdateDrawnWaypoints(float _radius) 
{
	static int nextWpUpdate = 0;
	vec3_t playerPos;
	int i, iDrawnLines = 0;

	if(!g_entities[0].client)
		return;

	VectorCopy(g_entities[0].r.currentOrigin, playerPos);

	// Do we need to draw the waypoints again?

	if(g_DebugLines.m_NumDebugLines && (nextWpUpdate < level.time))
	{
		// Count how many lines we're allowed to draw.
		int iNumAllowedLines = _CountFreeEntities() - 100;

		for(i = 0; i < g_DebugLines.m_NumDebugLines; i++)
		{			
			g_DebugLines.m_pDebugLines[i].drawme = qfalse;

			// Only display if it's in the pvs.
			if(trap_InPVS(g_DebugLines.m_pDebugLines[i].start, playerPos) ||
				trap_InPVS(g_DebugLines.m_pDebugLines[i].end, playerPos))
			{
				vec3_t towp1, towp2;
				float radSq = _radius * _radius;
				VectorSubtract(g_DebugLines.m_pDebugLines[i].start, playerPos, towp1);
				VectorSubtract(g_DebugLines.m_pDebugLines[i].end, playerPos, towp2);

				g_DebugLines.m_pDebugLines[i].dist = min(VectorLengthSquared(towp1), VectorLengthSquared(towp2));
				if(g_DebugLines.m_pDebugLines[i].dist > radSq)
					continue;

				g_DebugLines.m_pDebugLines[i].drawme = qtrue;
			}
		}

		// Sort the points based on distance.
		qsort(g_DebugLines.m_pDebugLines, g_DebugLines.m_NumDebugLines, sizeof(debugLines_t), wp_compare);

		// Render blockables first.
		for(i = 0; (i < g_BlockableDebugLines.m_NumDebugLines) && (iDrawnLines < iNumAllowedLines); ++i)
		{			
			// Only display if it's in the pvs.
			if(trap_InPVS(g_BlockableDebugLines.m_pDebugLines[i].start, playerPos) ||
				trap_InPVS(g_BlockableDebugLines.m_pDebugLines[i].end, playerPos))
			{
				vec3_t towp1, towp2;
				float radSq = _radius * _radius;
				VectorSubtract(g_BlockableDebugLines.m_pDebugLines[i].start, playerPos, towp1);
				VectorSubtract(g_BlockableDebugLines.m_pDebugLines[i].end, playerPos, towp2);

				g_BlockableDebugLines.m_pDebugLines[i].dist = min(VectorLengthSquared(towp1), VectorLengthSquared(towp2));
				if(g_BlockableDebugLines.m_pDebugLines[i].dist > radSq)
					continue;

				pfnAddTempDisplayLine(g_BlockableDebugLines.m_pDebugLines[i].start, 
					g_BlockableDebugLines.m_pDebugLines[i].end, 
					g_BlockableDebugLines.m_pDebugLines[i].color);
				++iDrawnLines;
			}
		}

		// Render closest to farthest
		for(i = 0; (i < g_DebugLines.m_NumDebugLines) && (iDrawnLines < iNumAllowedLines); ++i)
		{
			if(g_DebugLines.m_pDebugLines[i].drawme)
			{
				// Draw it.
				switch(g_DebugLines.m_pDebugLines[i].type)
				{
				case LINE_NORMAL:
				case LINE_WAYPOINT:
					//case LINE_BLOCKABLE:
					{
						pfnAddTempDisplayLine(g_DebugLines.m_pDebugLines[i].start, g_DebugLines.m_pDebugLines[i].end, g_DebugLines.m_pDebugLines[i].color);
						++iDrawnLines;
						break;
					}
				case LINE_PATH:
					{
						// adjust the end of the line for paths so they slant down
						// toward the end of the connection instead of drawing on top
						// of each other.
						vec3_t end;
						VectorCopy(g_DebugLines.m_pDebugLines[i].end, end);
						end[2] -= 30.0f;
						pfnAddTempDisplayLine(g_DebugLines.m_pDebugLines[i].start, end, g_DebugLines.m_pDebugLines[i].color);
						++iDrawnLines;
						break;
					}
				}
			}
		}
#ifdef _DEBUG
		G_Printf("%s %s: %d/%d debug lines, %d/%d blockable lines %d drawn lines\n", S_COLOR_GREEN, __FUNCTION__, 
			g_DebugLines.m_NumDebugLines, 
			g_DebugLines.m_MaxDebugLines, 
			g_BlockableDebugLines.m_NumDebugLines, 
			g_BlockableDebugLines.m_MaxDebugLines, 
			iDrawnLines);
#endif
		// Clear the blockable debug lines each pulse.
		ClearLineList(&g_BlockableDebugLines, qfalse);

		// Schedule the next update.
		nextWpUpdate = level.time + 1999;
	}
}

int Bot_Interface_Init() 
{
	int iLoadResult = -1;
	memset(&g_InterfaceFunctions, 0, sizeof(g_InterfaceFunctions));

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
	g_InterfaceFunctions.pfnTraceLine					= pfnTraceLine;
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

	// Entity info.
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

	g_InterfaceFunctions.pfnBotGetCurrentAmmo			= pfnBotGetCurrentAmmo;
	g_InterfaceFunctions.pfnBotGetCurrentWeaponClip		= pfnBotGetCurrentWeaponClip;

	g_InterfaceFunctions.pfnEntityFromID				= pfnEntityFromID;
	g_InterfaceFunctions.pfnIDFromEntity				= pfnIDFromEntity;

	// Waypoint functions.
	g_InterfaceFunctions.pfnAddDisplayLine				= pfnAddDisplayLine;
	g_InterfaceFunctions.pfnAddDisplayRadius			= pfnAddDisplayRadius;
	g_InterfaceFunctions.pfnAddTempDisplayLine			= pfnAddTempDisplayLine;
	g_InterfaceFunctions.pfnClearDebugLines				= pfnClearDebugLines;
	g_InterfaceFunctions.pfnFindEntityByClassName		= pfnFindEntityByClassName;
	g_InterfaceFunctions.pfnFindEntityInSphere			= pfnFindEntityInSphere;
	g_InterfaceFunctions.pfnPrintEntitiesInRadius		= pfnPrintEntitiesInRadius;

	INITBOTLIBRARY(ET_VERSION_LATEST, NAVID_WP, "omnibot_et.dll", "omnibot_et.so", g_OmniBotPath.string, iLoadResult);
	if(iLoadResult != BOT_ERROR_NONE)
	{
		SHUTDOWNBOTLIBRARY;
		g_BotLoaded = 0;
		pfnPrintError(va("Omni-bot Error: (%d) %s", iLoadResult, BOT_ERR_MSG(iLoadResult)));
		return 0;
	}

	g_BotLoaded = 1;
	return 1;
}

int Bot_Interface_Shutdown()
{
	if(g_BotLoaded)
	{
		Bot_Interface_SendGlobalEvent(GAME_ID_ENDGAME, -1, 0, 0);
		g_BotFunctions.pfnBotShutdown();
		g_BotLoaded = 0;

		// Free the waypoint info.
		ClearLineList(&g_DebugLines, qtrue);
		ClearLineList(&g_BlockableDebugLines, qtrue);
	}
	SHUTDOWNBOTLIBRARY;
	return 1;
}

void Bot_Interface_Update()
{
	if(g_BotLoaded)
	{
		char buf[1024] = {0};
		int i;	

		if (level.framenum == GAME_INIT_FRAMES)
			Bot_Interface_SendGlobalEvent(GAME_ID_STARTGAME, -1, 0, 0);

		_UpdateDrawnWaypoints(2000.0f);

		for( i = 0; i < MAX_CLIENTS; i++ )
		{
			if(!g_entities[i].inuse)
				continue;
			if (!g_entities[i].client)
				continue;
			if(g_entities[i].client->pers.connected != CON_CONNECTED)
				continue;

			// Send a spectated message to bots that are being spectated.
			if ((g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR) && 
				(g_entities[i].client->sess.spectatorState == SPECTATOR_FOLLOW))
			{
				int iDestination = g_entities[i].client->sess.spectatorClient;
				Bot_Interface_SendEvent(MESSAGE_SPECTATED, iDestination, i, 0, NULL);
			}

			// fake handle server commands (to prevent server command overflow)
			if((g_entities[i].inuse == qtrue) && ( g_entities[i].r.svFlags & SVF_BOT))
			{
				while (trap_BotGetServerCommand(i, buf, sizeof(buf))) 
				{

				}
			}			
		}

		// Call the libraries update.
		g_BotFunctions.pfnBotUpdate();
	}	
}

int Bot_Interface_ConsoleCommand() {
	if(g_BotLoaded)
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

		return g_BotFunctions.pfnBotConsoleCommand(buffer, strlen(buffer));;
	}
	return 1;
}

void Bot_Interface_LogOutput(const char *_txt) 
{
	if(g_BotLoaded)	
		g_BotFunctions.pfnBotLog(_txt);
}

void Bot_Interface_SendEvent(int _eid, int _dest, int _source, int _msdelay, BotUserData * _data) 
{
	if(g_BotLoaded)
	{
		g_BotFunctions.pfnBotSendEvent(_eid, _dest, _source, _msdelay, _data);
	}	
}

void Bot_Interface_SendGlobalEvent(int _eid, int _source, int _msdelay, BotUserData * _data) 
{
	if(g_BotLoaded)	
	{
		g_BotFunctions.pfnBotSendGlobalEvent(_eid, _source, _msdelay, _data);
	}	
}

void Bot_Util_AddGoal(const GameEntity _ent, int _goaltype, int _team, const char *_tag, BotUserData *_bud)
{
	if(g_BotLoaded)
	{
		g_BotFunctions.pfnBotAddGoal(_ent, _goaltype, _team, _tag, _bud);
	}
}

void Bot_Util_SendSoundEvent(int _client, int _sndtype, GameEntity _source)
{
	if (g_BotLoaded)
	{
		botUserData.DataType = BotUserData::dtEntity;
		botUserData.udata.m_Entity = _source;
		Bot_Interface_SendEvent(PERCEPT_HEAR_SOUND, _client, _sndtype, 0, &botUserData);
	}
}

void Bot_Util_SendTrigger(TriggerInfo *_triggerInfo)
{
	if(g_BotLoaded)
		g_BotFunctions.pfnBotSendTrigger(_triggerInfo);
}

//////////////////////////////////////////////////////////////////////////
// Utility functions for the debug lists.

void InitLineList(LineList *_list, int _initialsize)
{
	if(_list->m_pDebugLines)
	{
		free(_list->m_pDebugLines);
		_list->m_pDebugLines = 0;
	}

	_list->m_NumDebugLines = 0;
	_list->m_MaxDebugLines = _initialsize;
	_list->m_pDebugLines = (debugLines_t*)calloc(_list->m_MaxDebugLines, sizeof(debugLines_t));
}

void AddToLineList(LineList *_list, const debugLines_t *_line)
{
	if(_list->m_NumDebugLines >= _list->m_MaxDebugLines)
	{
		// We've gone over, so we need to reallocate.
		int iNewBufferSize = _list->m_MaxDebugLines + OMNIBOT_LINES_INCREASE_SIZE;
		debugLines_t *pNewBuffer = (debugLines_t*)calloc(iNewBufferSize, sizeof(debugLines_t));

		// Copy the old buffer to the new.
		memcpy(pNewBuffer, _list->m_pDebugLines, sizeof(debugLines_t) * _list->m_MaxDebugLines);

		// Free the old buffer.
		free(_list->m_pDebugLines);

		// Update the pointer to the new buffer.
		_list->m_pDebugLines = pNewBuffer;

		// Save the new size.
		_list->m_MaxDebugLines = iNewBufferSize;		
	}

	// Add it to the list.
	_list->m_pDebugLines[_list->m_NumDebugLines] = *_line;
	++_list->m_NumDebugLines;
}

void ClearLineList(LineList *_list, qboolean _freememory)
{
	if(_freememory == qfalse)
	{
		_list->m_NumDebugLines = 0;
	}
	else
	{
		if(_list->m_pDebugLines)
		{
			free(_list->m_pDebugLines);
		}
		_list->m_pDebugLines = 0;
		_list->m_NumDebugLines = 0;
		_list->m_MaxDebugLines = 0;
	}
}

#endif
//////////////////////////////////////////////////////////////////////////
