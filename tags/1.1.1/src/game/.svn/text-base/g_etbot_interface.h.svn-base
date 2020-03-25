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

#include "../../Omnibot/Common/Omni-Bot_Types.h"
#include "../../Omnibot/Common/Omni-Bot_Events.h"
#include "../../Omnibot/ET/ET_Config.h"

#define NO_BOT_SUPPORT

#define OMNIBOT_MODNAME GAMEVERSION
#define OMNIBOT_MODVERSION ETPUB_VERSION

extern BotUserData botUserData;
int Bot_Interface_Init();
int Bot_Interface_Shutdown();

void Bot_Interface_SendEvent(int _eid, int _dest, int _source, int _msdelay, BotUserData *_data);
void Bot_Interface_SendGlobalEvent(int _eid, int _source, int _msdelay, BotUserData * _data);

void Bot_Interface_Update();

int	 Bot_Interface_ConsoleCommand();
void Bot_Interface_LogOutput(const char *_txt);

void Bot_Util_AddGoal(const GameEntity _ent, int _goaltype, int _team, const char *_tag, BotUserData *_bud);
void Bot_Util_SendSoundEvent(int _client, int _sndtype, GameEntity _source);
void Bot_Util_SendTrigger(TriggerInfo *_triggerInfo);

void Bot_Util_EntityCreated(GameEntity _ent);

int Bot_WeaponGameToBot(int weapon);
int Bot_TeamGameToBot(int team);
int Bot_PlayerClassGameToBot(int playerClass);

#endif

