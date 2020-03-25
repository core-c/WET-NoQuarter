/*
 * name:	g_team.h
 *
 * desc:
 *
 * NQQS:
 *
 */

#ifndef __G_TEAM_H__
#define __G_TEAM_H__

// JPW NERVE -- more #defs for GT_WOLF gametype
#define WOLF_CAPTURE_BONUS		15		// capturing major game objective
#define WOLF_STEAL_OBJ_BONUS	10		// stealing objective (first part of capture)
#define WOLF_SECURE_OBJ_BONUS	10		// securing objective from slain enemy
#define WOLF_MEDIC_BONUS		2		// medic resurrect teammate
#define WOLF_REPAIR_BONUS		2		// engineer repair (mg42 etc) bonus
#define WOLF_DYNAMITE_BONUS		5		// engineer dynamite barriacade (dynamite only flag)
#define WOLF_FRAG_CARRIER_BONUS	10		// bonus for fragging enemy carrier
#define WOLF_FLAG_DEFENSE_BONUS	5		// bonus for frag when shooter or target near flag position
#define WOLF_CP_CAPTURE			3		// uncapped checkpoint bonus
#define WOLF_CP_RECOVER			5		// capping an enemy-held checkpoint bonus
#define WOLF_SP_CAPTURE			1		// uncapped spawnpoint bonus
#define WOLF_SP_RECOVER			2		// recovering enemy-held spawnpoint
#define WOLF_CP_PROTECT_BONUS	3		// protect a capture point by shooting target near it
#define WOLF_SP_PROTECT_BONUS	1		// protect a spawnpoint
#define WOLF_FRIENDLY_PENALTY	-3		// penalty for fragging teammate
#define WOLF_FRAG_BONUS			1		// score for fragging enemy soldier
#define WOLF_DYNAMITE_PLANT		5		// planted dynamite at objective
#define WOLF_DYNAMITE_DIFFUSE	5		// diffused dynamite at objective
#define WOLF_CP_PROTECT_RADIUS	600		// wolf capture protect radius
#define WOLF_AMMO_UP			1		// pt for giving ammo not to self
#define WOLF_HEALTH_UP			1		// pt for giving health not to self

// objective spawnflags?
#define	AXIS_OBJECTIVE			1
#define ALLIED_OBJECTIVE		2
#define	OBJECTIVE_DESTROYED		4
// jpw

#define CONSTRUCTIBLE_START_BUILT	1
#define CONSTRUCTIBLE_INVULNERABLE	2
#define AXIS_CONSTRUCTIBLE			4
#define ALLIED_CONSTRUCTIBLE		8
#define CONSTRUCTIBLE_BLOCK_PATHS_WHEN_BUILD	16
#define CONSTRUCTIBLE_NO_AAS_BLOCKING			32
#define CONSTRUCTIBLE_AAS_SCRIPTED				64

#define EXPLOSIVE_START_INVIS		1
#define EXPLOSIVE_TOUCHABLE			2
#define EXPLOSIVE_USESHADER			4
#define EXPLOSIVE_LOWGRAV			8
#define EXPLOSIVE_NO_AAS_BLOCKING	16
#define EXPLOSIVE_TANK				32

#define CTF_TARGET_PROTECT_RADIUS			400	// the radius around an object being defended where a target will be worth extra frags

// Prototypes

int OtherTeam(int team);
const char *TeamName(int team);
const char *OtherTeamName(int team);
const char *TeamColorString(int team);

void Team_RemoveFlag(int team);
void Team_DroppedFlagThink(gentity_t *ent);
void Team_FragBonuses(gentity_t *targ, gentity_t *inflictor, gentity_t *attacker);
void Team_CheckHurtCarrier(gentity_t *targ, gentity_t *attacker);
void Team_InitGame(void);
void Team_ReturnFlag(gentity_t *ent, gentity_t *other);
void Team_FreeEntity(gentity_t *ent);
gentity_t *SelectCTFSpawnPoint ( team_t team, int teamstate, vec3_t origin, vec3_t angles, int spawnObjective );
gentity_t *SelectPlayerSpawnPoint ( team_t team, int teamstate, vec3_t origin, vec3_t angles);
void TeamplayInfoMessage( team_t team );
void CheckTeamStatus(void);

int Pickup_Team( gentity_t *ent, gentity_t *other );

#endif
