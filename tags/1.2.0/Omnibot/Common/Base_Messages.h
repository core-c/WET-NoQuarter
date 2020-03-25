////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2007-04-26 19:19:31 -0700 (Thu, 26 Apr 2007) $
// $LastChangedRevision: 1859 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_MESSAGES_H__
#define __BASE_MESSAGES_H__

#include "Omni-Bot_Types.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

typedef struct  
{
	int			m_Team;
	int			m_Class;
	char		m_Name[32];
	int			m_ModelId;
	int			m_SkinId;
} Msg_Addbot;

typedef struct  
{
	enum { NumItems = 16 };
	int			m_WeaponChoice[NumItems];
	int			m_ItemChoice[NumItems];
} Msg_PlayerChooseEquipment;

typedef struct 
{
	int			m_CurrentHealth;
	int			m_MaxHealth;
	int			m_CurrentArmor;
	int			m_MaxArmor;
} Msg_PlayerHealthArmor;

typedef struct 
{
	float		m_MaxSpeed;
} Msg_PlayerMaxSpeed;

typedef struct 
{	
	obBool		m_IsAlive;
} Msg_IsAlive;

typedef struct 
{
	GameEntity	m_TargetEntity;
	obBool		m_IsAllied;
} Msg_IsAllied;

typedef struct 
{
	float		m_Position[3];
	obBool		m_IsOutside;
} Msg_IsOutside;

typedef struct  
{
	char	m_NewName[32];
} Msg_ChangeName;

typedef struct 
{
	int			m_Contents;
	float		x,y,z;
} Msg_PointContents;

typedef struct 
{
	obBool		m_Ready;
} Msg_ReadyToFire;

typedef struct 
{
	obBool		m_Reloading;
} Msg_Reloading;

typedef struct 
{
	FlagState	m_FlagState;
	GameEntity	m_Owner;
} Msg_FlagState;

typedef struct 
{
	GameState	m_GameState;
	float		m_TimeLeft;
} Msg_GameState;

typedef struct 
{
	char		m_StatName[64];
	obUserData m_Result;
} Msg_EntityStat;

typedef struct 
{
	int			m_Team;
	char		m_StatName[64];
	obUserData m_Result;
} Msg_TeamStat;

typedef struct 
{
	char		m_Command[256];
} Msg_ServerCommand;

typedef struct 
{
	int			m_Weapon;
	FireMode	m_FireMode;
	obBool		m_IsCharged;
} WeaponCharged;

typedef struct 
{
	FireMode	m_FireMode;
	float		m_CurrentHeat;
	float		m_MaxHeat;
} WeaponHeatLevel;

struct WeaponStatus
{
	int			m_WeaponId;
	//FireMode	m_FireMode;

	WeaponStatus() : m_WeaponId(0) {}

	bool operator==(const WeaponStatus &_w2)
	{
		return m_WeaponId == _w2.m_WeaponId;
	}
	bool operator!=(const WeaponStatus &_w2)
	{
		return !(*this == _w2);
	}
};

typedef struct 
{
	float		m_CenterFacing[3];
	float		m_MinHorizontalArc, m_MaxHorizontalArc;
	float		m_MinVerticalArc, m_MaxVerticalArc;
	int			m_WeaponId;
	obBool		m_Limited;
} WeaponLimits;

typedef struct  
{
	GameEntity	m_WhoToKill;
} Msg_KillEntity;

//////////////////////////////////////////////////////////////////////////
// Events

typedef struct  
{
	int			m_ThreadId;
} Event_SystemThreadCreated;

typedef struct  
{
	int			m_ThreadId;
} Event_SystemThreadDestroyed;

typedef struct  
{
	int			m_GameId;
	obBool		m_IsBot;
} Event_SystemClientConnected;

typedef struct  
{
	int			m_GameId;
} Event_SystemClientDisConnected;

typedef struct  
{
	float		m_Gravity;
} Event_SystemGravity;

typedef struct  
{
	obBool		m_Enabled;
} Event_SystemCheats;

//////////////////////////////////////////////////////////////////////////

typedef struct  
{
	GameEntity	m_WhoKilledMe;
	char		m_MeansOfDeath[32];
} Event_Death;

typedef struct  
{
	GameEntity	m_WhoIKilled;
	char		m_MeansOfDeath[32];
} Event_KilledSomeone;

typedef struct  
{
	GameEntity	m_Inflictor;
} Event_TakeDamage;

typedef struct  
{
	GameEntity	m_WhoHealedMe;
} Event_Healed;

typedef struct  
{
	GameEntity	m_WhoRevivedMe;
} Event_Revived;

typedef struct  
{
	int			m_NewTeam;
} Event_ChangeTeam;

typedef struct  
{
	int			m_WeaponId;
} Event_WeaponChanged;

typedef struct  
{
	int			m_NewClass;
} Event_ChangeClass;

typedef struct  
{
	int			m_WhoSpectatingMe;
} Event_Spectated;

typedef struct  
{
	int			m_WeaponId;
} Event_AddWeapon;

typedef struct  
{
	int			m_WeaponId;
} Event_RemoveWeapon;

typedef struct  
{
	int			m_WeaponId;
	FireMode	m_FireMode;
	GameEntity	m_Projectile;
} Event_WeaponFire;

typedef struct  
{
	int			m_WeaponId;
} Event_WeaponChange;

typedef struct  
{
	GameEntity	m_WhoSaidIt;
	char		m_Message[512];
} Event_ChatMessage;

typedef struct  
{
	GameEntity	m_WhoSaidIt;
	char		m_MacroString[64];
} Event_VoiceMacro;

typedef struct  
{
	GameEntity	m_WhoDidIt;
} Event_PlayerUsed;

typedef struct  
{
	char		m_SoundName[32];
	float		m_Origin[3];
	GameEntity	m_Source;
	int			m_SoundType;
} Event_HearSound;

#pragma pack(pop)

#endif

