/*
 * ET <-> Omni-Bot interface header file.
 * 
 */

// tjw: VC6 does not know __FUNCTION__
#ifdef _MSC_VER
	#if _MSC_VER <= 1200
		#define __FUNCTION__ "UNKNOWN"
	#endif
#endif

#ifndef __G_ETBOT_INTERFACE_H__
#define __G_ETBOT_INTERFACE_H__

#include "q_shared.h"
#include "g_local.h"
#include "../../Omnibot/Common/Omni-Bot_Types.h"
#include "../../Omnibot/Common/Omni-Bot_Events.h"
#include "../../Omnibot/ET/ET_Config.h"

#define NO_BOT_SUPPORT

#define OMNIBOT_MODNAME GAMEVERSION
#define OMNIBOT_MODVERSION ETPUB_VERSION

//////////////////////////////////////////////////////////////////////////
// g_OmniBotFlags bits
enum BotFlagOptions
{
	OBF_DONT_XPSAVE			= (1<<0), // Disables XPSave for bots 
	OBF_DONT_MOUNT_TANKS	= (1<<1), // Bots cannot mount tanks 
	OBF_DONT_MOUNT_GUNS		= (1<<2), // Bots cannot mount emplaced guns
	OBF_DONT_SHOW_BOTCOUNT	= (1<<3), // Don't count bots
	OBF_NEXT_FLAG			= (1<<16), // mod specific flags start from here
};
//////////////////////////////////////////////////////////////////////////

int Bot_Interface_Init();
int Bot_Interface_Shutdown();

void Bot_Interface_Update();

int	 Bot_Interface_ConsoleCommand();

void Bot_Util_AddGoal(const GameEntity _ent, int _goaltype, int _team, const char *_tag, obUserData *_bud);
void Bot_Util_SendTrigger(TriggerInfo *_triggerInfo);

void Bot_Util_EntityCreated(GameEntity _ent);

int Bot_WeaponGameToBot(int weapon);
int Bot_TeamGameToBot(int team);
int Bot_PlayerClassGameToBot(int playerClass);

//////////////////////////////////////////////////////////////////////////

void Bot_Event_ClientConnected(int _client, qboolean _isbot);
void Bot_Event_ClientDisConnected(int _client);

void Bot_Event_StartGame();
void Bot_Event_EndGame();

void Bot_Event_Spawn(int _client);
void Bot_Event_ResetWeapons(int _client);
void Bot_Event_AddWeapon(int _client, int _weaponId);
void Bot_Event_ChangeTeam(int _client, int _newteam);
void Bot_Event_ChangeClass(int _client, int _newclass);

void Bot_Event_TakeDamage(int _client, GameEntity _ent);
void Bot_Event_Death(int _client, GameEntity _killer, const char *_meansofdeath);
void Bot_Event_KilledSomeone(int _client, GameEntity _victim, const char *_meansofdeath);
void Bot_Event_Revived(int _client, GameEntity _whodoneit);
void Bot_Event_Healed(int _client, GameEntity _whodoneit);

void Bot_Event_FireWeapon(int _client, int _weaponId, GameEntity _projectile);
void Bot_Event_PreTriggerMine(int _client, GameEntity _mine);
void Bot_Event_PostTriggerMine(int _client, GameEntity _mine);
void Bot_Event_MortarImpact(int _client, vec3_t _pos);

void Bot_Event_Spectated(int _client, int _who);

void Bot_Event_ChatMessage(int _client, GameEntity _source, int _type, const char *_message);
void Bot_Event_VoiceMacro(int _client, GameEntity _source, int _type, const char *_message);

void Bot_Event_Sound(int _client, int _sndtype, GameEntity _source, float *_origin, const char *_name);

#endif


