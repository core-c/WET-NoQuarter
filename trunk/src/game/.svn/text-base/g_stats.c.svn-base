/*
 * name:	g_stats.c
 *
 * desc:
 *
 * NQQS:
 *
 */
#include "g_local.h"

#ifdef ET_LUA
// *LUA*
#include "g_lua.h"
#endif

void G_LogDeath( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	ent->client->pers.playerStats.weaponStats[weap].killedby++;

#ifdef PUNKBUSTER
	trap_PbStat ( ent - g_entities , "death" ,
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
#endif
}

void G_LogKill( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	ent->client->pers.playerStats.weaponStats[weap].kills++;

#ifdef PUNKBUSTER
	trap_PbStat ( ent - g_entities , "kill" ,
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
#endif
}

void G_LogTeamKill( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	ent->client->pers.playerStats.weaponStats[weap].teamkills++;

#ifdef PUNKBUSTER
	trap_PbStat ( ent - g_entities , "tk" ,
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
#endif
}

// Logs regions hits only if player was alive to syncronize weaponstats/hitregions
// It's useless to do stats for dead players - gib them by headshots :/
// punkbuster still gets all crap ...
void G_LogRegionHit( gentity_t* ent, hitRegion_t hr , qboolean wasAlive) {
	if(!ent->client) {
		return;
	}

	if (wasAlive) {
		ent->client->pers.playerStats.hitRegions[hr]++;
	}

#ifdef PUNKBUSTER
	trap_PbStat ( ent - g_entities , "hr" ,
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , hr ) ) ;
#endif
}

void G_PrintAccuracyLog( gentity_t *ent ) {
	int i;
	char buffer[2048];

	Q_strncpyz(buffer, "WeaponStats", 2048);

	for( i = WP_KNIFE; i < WP_NUM_WEAPONS; i++ ) {
		if(!BG_ValidStatWeapon(i)) {
			continue;
		}

		Q_strcat(buffer, 2048, va(" %i %i %i",
			ent->client->pers.playerStats.weaponStats[i].kills,
			ent->client->pers.playerStats.weaponStats[i].killedby,
			ent->client->pers.playerStats.weaponStats[i].teamkills ));
	}

	Q_strcat( buffer, 2048, va(" %i", ent->client->pers.playerStats.suicides));

	for( i = 0; i < HR_NUM_HITREGIONS; i++ ) {
		Q_strcat( buffer, 2048, va(" %i", ent->client->pers.playerStats.hitRegions[i]));
	}

	Q_strcat( buffer, 2048, va(" %i", 6/*level.numOidTriggers*/ ));

	for( i = 0; i < 6/*level.numOidTriggers*/; i++ ) {
		Q_strcat( buffer, 2048, va(" %i", ent->client->pers.playerStats.objectiveStats[i]));
		Q_strcat( buffer, 2048, va(" %i", ent->client->sess.sessionTeam == TEAM_AXIS ? level.objectiveStatsAxis[i] : level.objectiveStatsAllies[i]));
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void G_SetPlayerScore( gclient_t *client ) {
	int i;

	for( client->ps.persistant[PERS_SCORE] = 0, i = 0; i < SK_NUM_SKILLS; i++ ) {
		client->ps.persistant[PERS_SCORE] += client->sess.skillpoints[i];
	}
}

// number of awarded skills
void G_SetPlayerSkill( gclient_t *client, skillType_t skill ) {
	int i;

#ifdef ET_LUA
	// *LUA* API callbacks
	if (G_LuaHook_SetPlayerSkill( client - level.clients, skill ) ) {
		return;
	}
#endif

	client->sess.skillBits[skill] = 0;
	client->sess.skill[skill] = 0;

	// just count awarded skills
	for( i = 1 ; i < NUM_SKILL_LEVELS ; i++ ) {
		if( skillLevels[skill][i] >= 0 && client->sess.skillpoints[skill] >= skillLevels[skill][i] ) {
			// core: we want to skip skills that are disabled..
			client->sess.skill[skill] = i;	// core: was client->sess.skill[skill]++
			client->sess.skillBits[skill] |= (1<<i);
		}
	}

	G_SetPlayerScore( client );
}

extern qboolean AddWeaponToPlayer( gclient_t *client, weapon_t weapon, int ammo, int ammoclip, qboolean setcurrent );

int rankTable[NUM_EXPERIENCE_LEVELS] =
{
	1,	2,	3,	4,	5, 	7,	9,	11,	14,	17,	20,	24,	28,	34,	40,	47,	54,	63,	999
};

// TAT 11/6/2002
//		Local func to actual do skill upgrade, used by both MP skill system, and SP scripted skill system
static void G_UpgradeSkill( gentity_t *ent, skillType_t skill )
{
	int i, cnt = 0;
	int next=-1;

#ifdef ET_LUA
	// *LUA* API callbacks
	if (G_LuaHook_UpgradeSkill(g_entities - ent, skill) ) {
		return;
	}
#endif

	G_DebugAddSkillLevel( ent, skill );

	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		cnt += ent->client->sess.skill[ i ];
	}

	// recompute rank..
	ent->client->sess.rank = 0;

	for ( i = NUM_EXPERIENCE_LEVELS-1 ; i > 0 ; i-- ) {
		next = rankTable[i-1];
		if ( cnt >= next && ent->client->sess.rank < NUM_EXPERIENCE_LEVELS-1  ) {
			ent->client->sess.rank = i;
			break;
		}
	}

	ClientUserinfoChanged( ent-g_entities );	//core: reverted ClientConfigStringChanged( ent );

	// Give em rightaway
	if( skill == SK_BATTLE_SENSE && ent->client->sess.skillBits[SK_BATTLE_SENSE] & (1<<1) ) {
		if( AddWeaponToPlayer( ent->client, WP_BINOCULARS, 1, 0, qfalse ) ) {
			ent->client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	}
	else if( skill == SK_FIRST_AID && ent->client->sess.playerType == PC_MEDIC && ent->client->sess.skillBits[SK_MEDIC] & (1<<4) && g_adrenaline.integer & ADF_MEDIC) {
		int needles = 10;

		if ( g_adrenaline.integer & ADF_JUSTONE )
			needles = 1;

		if ( g_adrenaline.integer & ADF_NOSPAWN )
			needles = 0;

		AddWeaponToPlayer( ent->client, WP_MEDIC_ADRENALINE, 0, needles, qfalse );
	}
	else if( skill == SK_ENGINEER && ent->client->sess.playerType == PC_ENGINEER && ent->client->sess.skillBits[SK_ENGINEER] & (1<<4) ) {
		 ent->client->ps.powerups[PW_FLAKJACKET] = INT_MAX;
	}
	else if( skill == SK_SOLDIER && ent->client->sess.playerType == PC_SOLDIER && ent->client->sess.skillBits[SK_SOLDIER] & (1<<9) ) {
		 ent->client->ps.powerups[PW_HELMETSHIELD] = INT_MAX;
	}
}

qboolean G_LoseSkillPointsExt(gentity_t *ent, skillType_t skill, float points)
{
	int oldskill;
	float oldskillpoints;

	if(!ent || !ent->client) {
		return qfalse;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		return qfalse; // Gordon: no xp in LMS
	}

	oldskillpoints = ent->client->sess.skillpoints[skill];
	ent->client->sess.skillpoints[skill] -= points;

	// see if player increased in skill
	oldskill = ent->client->sess.skill[skill];
	G_SetPlayerSkill( ent->client, skill );
	if(oldskill != ent->client->sess.skill[skill] &&
		ent->client->pers.connected == CON_CONNECTED) {
		G_UpgradeSkill(ent, skill);
	}

	level.teamScores[ent->client->ps.persistant[PERS_TEAM]] -=
			oldskillpoints - ent->client->sess.skillpoints[skill];
	level.teamXP[skill][ent->client->sess.sessionTeam - TEAM_AXIS] -=
			oldskillpoints - ent->client->sess.skillpoints[skill];

	return qtrue;
}

void G_LoseSkillPoints(gentity_t *ent, skillType_t skill, float points)
{
	float oldskillpoints;

	if(!ent || !ent->client)
		return;

	if(ent->client->sess.sessionTeam != TEAM_AXIS && ent->client->sess.sessionTeam != TEAM_ALLIES) {
		return;
	}

// no skill loss during warmup
	if( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	oldskillpoints = ent->client->sess.skillpoints[skill];

	if(!G_LoseSkillPointsExt(ent, skill, points)) {
		return;
	}

	// CHRUKER: b013 - Was printing this with many many decimals
	G_Printf( "%s just lost %.0f skill points for skill %s\n",
				ent->client->pers.netname,
				oldskillpoints - ent->client->sess.skillpoints[skill],
				skillNames[skill]);

#ifdef PUNKBUSTER
	trap_PbStat ((ent - g_entities), "loseskill" ,
				va("%d %d %d %f",
				ent->client->sess.sessionTeam,
				ent->client->sess.playerType,
				skill,
				oldskillpoints - ent->client->sess.skillpoints[skill]));
#endif
}

void G_XPDecay(gentity_t *ent, int seconds, qboolean force)
{
	int i =  0;
	float points;
	float p;
	int skill_for_class[] = {
		// PC_SOLDIER
		SK_HEAVY_WEAPONS,
		// PC_MEDIC
		SK_FIRST_AID,
		// PC_ENGINEER
		SK_EXPLOSIVES_AND_CONSTRUCTION,
		// PC_FIELDOPS
		SK_SIGNALS,
		// PC_COVERTOPS
		SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS
	};

	if(!(g_XPDecay.integer & XPDF_ENABLE))
		return;
	if(g_XPDecayRate.value == 0.0f)
		return;

	// tjw: state matters if XPSave is not forcing this decay
	if(!force) {
		if((g_XPDecay.integer & XPDF_NO_SPEC_DECAY) &&
			ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
				return;
		}
		if((g_XPDecay.integer & XPDF_NO_GAMESTATE_DECAY) &&
			g_gamestate.integer != GS_PLAYING) {
				return;
		}
		if((g_XPDecay.integer & XPDF_NO_PLAYING_DECAY) &&
			g_gamestate.integer == GS_PLAYING &&
			(ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES)) {
				return;
		}

	}

	points = (g_XPDecayRate.value * seconds);

	for(i=0; i<SK_NUM_SKILLS; i++) {
		if(!force && (g_XPDecay.integer & XPDF_NO_CLASS_DECAY) &&
			skill_for_class[ent->client->sess.playerType] == i) {
				continue;
		}
		else if(!force && (g_XPDecay.integer & XPDF_NO_BS_DECAY) && i == SK_BATTLE_SENSE) {
			continue;
		}
		else if(!force && (g_XPDecay.integer & XPDF_NO_LW_DECAY) && i == SK_LIGHT_WEAPONS) {
			continue;
		}

		p = points;

		// tjw: don't let xp be added
		if(g_XPDecayFloor.value < 0.0f)
			continue;

		// tjw: don't allow xp to be added up to the floor
		if(ent->client->sess.skillpoints[i] < g_XPDecayFloor.value)
			continue;

		// tjw: don't decay past floor
		if((ent->client->sess.skillpoints[i] - p) < (g_XPDecayFloor.value * 1.0f)) {
			p = (ent->client->sess.skillpoints[i] - (g_XPDecayFloor.value * 1.0f));
		}

		// tjw: don't decay past 0
		if(ent->client->sess.skillpoints[i] < p)
			p = ent->client->sess.skillpoints[i];

		G_LoseSkillPointsExt(ent, i, p);
	}
}

void G_ResetXP(gentity_t *ent)
{
	int		i = 0;
	int		ammo[MAX_WEAPONS], ammoclip[MAX_WEAPONS];
	int		oldWeapon, newWeapon;

	if(!ent || !ent->client)
			return;

	ent->client->sess.rank = 0;
	for(i = 0; i < SK_NUM_SKILLS; i++) {
		ent->client->sess.skillpoints[i] = 0.0f;
		ent->client->sess.skill[i] = 0;
	}

	G_CalcRank(ent->client);
	ent->client->ps.stats[STAT_XP] =  0;
	ent->client->ps.persistant[PERS_SCORE] = 0;

	// tjw: zero out all weapons and grab the default weapons for a player of this XP level.
	// backup..
	memcpy(ammo, ent->client->ps.ammo, sizeof(ammo));
	memcpy(ammoclip, ent->client->ps.ammoclip, sizeof(ammoclip));
	oldWeapon = ent->client->ps.weapon;
	// Check weapon validity, maybe dump some weapons for this (now) unskilled player..
	// It also sets the (possibly new) amounts of ammo for weapons.
	SetWolfSpawnWeapons(ent->client);
	// restore..
	newWeapon = ent->client->ps.weapon;

	for ( i=WP_NONE; i<WP_NUM_WEAPONS; ++i ) {	//i<MAX_WEAPONS
		// core: only restore ammo for valid weapons..
		// ..they might have lost some weapons because of skill changes.
		// Also restore to the old amount of ammo, because..
		// ..SetWolfSpawnWeapons sets amount of ammo for a fresh spawning player,
		// which is usually more than the player currently has left.
		if (i != WP_FOOTKICK) {
			if ( COM_BitCheck(ent->client->ps.weapons, i) ) {
				if ( ammo[i] < ent->client->ps.ammo[i] ) ent->client->ps.ammo[i] = ammo[i];
				if ( ammoclip[i] < ent->client->ps.ammoclip[i] ) ent->client->ps.ammoclip[i] = ammoclip[i];
			}
			else {
				ent->client->ps.ammo[i] = 0;
				ent->client->ps.ammoclip[i] = 0;
			}
		}
	}
	// core: check if the old weapon is still valid.
	// If so, restore to the last used weapon..
	if ( COM_BitCheck(ent->client->ps.weapons, oldWeapon) ) {
		ent->client->ps.weapon = oldWeapon;
	}
	ClientUserinfoChanged(ent-g_entities);
}


void G_AddSkillPoints( gentity_t *ent, skillType_t skill, float points )
{
	int oldskill;

	if( !ent->client ) {
		return;
	}

	// no skill gaining during warmup
	if( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	if( ent->client->sess.sessionTeam != TEAM_AXIS && ent->client->sess.sessionTeam != TEAM_ALLIES ) {
		return;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		return; // Gordon: no xp in LMS
	}

	level.teamXP[ skill ][ ent->client->sess.sessionTeam - TEAM_AXIS ] += points;

	ent->client->sess.skillpoints[skill] += points;

	if (g_maxXP.integer > 0 && (ent->client->ps.persistant[PERS_SCORE] + points) >= g_maxXP.integer) {
		G_ResetXP(ent);
		// tjw: be silent if g_maxXP is 0
		if(g_maxXP.integer) {
			CP("cp \"The good news is you've reached g_maxXP, the bad news is now you lose it all.\" 1");
		}
		return;
	}


	level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += points;

#ifdef PUNKBUSTER
	trap_PbStat ( ent - g_entities , "addskill" ,
		va ( "%d %d %d %f" , ent->client->sess.sessionTeam , ent->client->sess.playerType ,
			skill , points ) ) ;
#endif

	// see if player increased in skill
	oldskill = ent->client->sess.skill[skill];
	G_SetPlayerSkill( ent->client, skill );
	if( oldskill != ent->client->sess.skill[skill] ) {
		// TAT - call the new func that encapsulates the skill giving behavior
		G_UpgradeSkill( ent, skill );
	}
}

void G_LoseKillSkillPoints( gentity_t *tker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash ) {
	// for evil tkers :E

	if( !tker->client ) {
		return;
	}

	switch( mod ) {
		// Light Weapons
		// Knife
		case MOD_KNIFE:
		case MOD_BACKSTAB:
		case MOD_THROWKNIFE:

		// Axis Pistols
		case MOD_LUGER:
		case MOD_SILENCER:
		case MOD_AKIMBO_LUGER:
		case MOD_AKIMBO_SILENCEDLUGER:

		// Allied Pistols
		case MOD_COLT:
		case MOD_SILENCED_COLT:
		case MOD_AKIMBO_COLT:
		case MOD_AKIMBO_SILENCEDCOLT:

		// Submachineguns
		case MOD_MP40:
		case MOD_THOMPSON:
		case MOD_STEN:
		case MOD_MP34:			// jP

		// Rifles
		case MOD_GARAND:		// Covert Rifle
		case MOD_CARBINE:		// Engineer Rifle
		case MOD_KAR98:			// Engineer Rifle
		case MOD_K43:			// Covert Rifle

		// Assault Rifles
		case MOD_FG42:
		case MOD_BAR:
		case MOD_STG44:
		case MOD_JOHNSON:

		// Hand Grenades
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:

		// Shotgun
		case MOD_SHOTGUN:		// jP
			G_LoseSkillPoints( tker, SK_LIGHT_WEAPONS, 3.f );
			break;

		// Poison
		case MOD_POISON:		// Meyer - lose 5xp if poisoning teammates
			G_LoseSkillPoints( tker, SK_LIGHT_WEAPONS, 5.f );
			break;

		// Covert Ops Weapons
		case MOD_GARAND_SCOPE:
		case MOD_K43_SCOPE:
		case MOD_FG42_SCOPE:
		case MOD_JOHNSON_SCOPE:
		case MOD_SATCHEL:
			G_LoseSkillPoints( tker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 3.f );
			break;

		// Heavy Weapons
		case MOD_MOBILE_MG42:
		case MOD_MOBILE_BROWNING:
		case MOD_MACHINEGUN:
		case MOD_BROWNING:
		case MOD_MG42:
		case MOD_PANZERFAUST:
		case MOD_FLAMETHROWER:
		case MOD_MORTAR:
		case MOD_BAZOOKA:
		case MOD_VENOM:
			G_LoseSkillPoints( tker, SK_HEAVY_WEAPONS, 4.f );
			break;

		// Engineer Weapons
		case MOD_DYNAMITE:
		case MOD_LANDMINE:
		case MOD_GPG40:
		case MOD_M7:
			G_LoseSkillPoints( tker, SK_EXPLOSIVES_AND_CONSTRUCTION, 3.f );
			break;

		// Field Ops Weapons
		case MOD_ARTY:
		case MOD_AIRSTRIKE:
		case MOD_SMOKEGRENADE:	// Airstrike Marker
			G_LoseSkillPoints( tker, SK_SIGNALS, 5.f );
			break;

		// Goomba Kills
		case MOD_KICKED:
		case MOD_SHOVE:
		case MOD_GOOMBA:
			G_LoseSkillPoints( tker, SK_BATTLE_SENSE, 5.f );
			break;

		// no skills for anything else
		default:
			break;
	}
}

void G_AddKillSkillPoints( gentity_t *attacker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash )
{

	if( !attacker->client )
		return;

	{
		float xp = 3.0f;			// 3.0 is the default for most kills

		switch( mod ) {
			// jaquboss - give some xp for it
			case MOD_KICKED:
				G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp  );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp, "kick kill" );
				break;
			case MOD_SHOVE:
				G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp + 2.0f );	//Meyer - give more xp for shove kills
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp, "shove kill" );
				break;
			// jet Pilot - give xp for goombas
			case MOD_GOOMBA:
				G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp + 2.0f );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp + 2.0f, "goomba kill" );

				// jet Pilot - increment goomba kills
				attacker->client->sess.goomba++;
				break;

			// light weapons
			case MOD_THROWKNIFE:
			case MOD_KNIFE:
			case MOD_BACKSTAB:
				G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "blade kill" );
				break;
			case MOD_LUGER:
			case MOD_COLT:
			case MOD_MP40:
			case MOD_THOMPSON:
			case MOD_STEN:
			case MOD_GARAND:
			case MOD_SILENCER:
			case MOD_FG42:
			case MOD_CARBINE:
			case MOD_KAR98:
			case MOD_SILENCED_COLT:
			case MOD_K43:
			case MOD_AKIMBO_COLT:
			case MOD_AKIMBO_LUGER:
			case MOD_AKIMBO_SILENCEDCOLT:
			case MOD_AKIMBO_SILENCEDLUGER:
			// jet Pilot - Award XP for new weapons
			case MOD_JOHNSON:
			case MOD_SHOTGUN:
			case MOD_MP34:
				switch( hr ) {
					case HR_HEAD:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp + 2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp + 2.0f, "headshot kill" );
									break;
					case HR_ARMS:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "armshot kill" );
									break;
					case HR_BODY:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "bodyshot kill" );
									break;
					case HR_LEGS:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "legshot kill" );
									break;
					// for weapons that don't have localized damage
					default:		G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "kill" );
									break;
				}
				break;
			case MOD_STG44:
			case MOD_BAR:
				switch( hr ) {
					case HR_HEAD:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp+1.0f );
									G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp-1.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp + 1.0f, "headshot kill" );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp - 1.0f, "headshot kill" );
									break;
					case HR_ARMS:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp-1.0f );
									G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp-2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp - 1.0f, "headshot kill" );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp-2.0f, "armshot kill" );
									break;
					case HR_BODY:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp-1.0f );
									G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp-2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp - 1.0f, "headshot kill" );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp-2.0f, "armshot kill" );
									break;
					case HR_LEGS:	G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp-1.0f );
									G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp-2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp - 1.0f, "headshot kill" );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp-2.0f, "armshot kill" );
									break;
					// for weapons that don't have localized damage
					default:		G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp-1.0f );
									G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp-2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp - 1.0f, "headshot kill" );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp-2.0f, "armshot kill" );
									break;
				}
				break;

			// heavy weapons
			case MOD_VENOM:
				G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "venom kill" );
				break;

			case MOD_MOBILE_BROWNING:
			case MOD_MOBILE_MG42:
				G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "mobile machinegun kill" );
				break;

			// scoped weapons
			case MOD_GARAND_SCOPE:
			case MOD_K43_SCOPE:
			case MOD_FG42_SCOPE:
			case MOD_JOHNSON_SCOPE:
				switch( hr ) {
					case HR_HEAD:	G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp + 2.0f );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp + 2.0f, "headshot kill" );
									break;
					case HR_ARMS:	G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp, "armshot kill" );
									break;
					case HR_BODY:	G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp, "bodyshot kill" );
									break;
					case HR_LEGS:	G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp, "legshot kill" );
									break;
					// for weapons that don't have localized damage
					default:		G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp );
									G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp, "kill" );
									break;
				}
				break;

			// misc weapons (individual handling)
			case MOD_SATCHEL:
				G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp + 2.0f );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, xp + 2.0f, "satchel charge kill" );
				break;

			case MOD_MACHINEGUN:
			case MOD_BROWNING:
			case MOD_MG42:
				G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "emplaced machinegun kill" );
				break;

			case MOD_BAZOOKA:
			case MOD_PANZERFAUST:
				if( splash ) {
					G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
					G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "panzerfaust splash damage kill" );
				}
				else {
					G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
					G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "panzerfaust direct hit kill" );
				}
				break;
			case MOD_FLAMETHROWER:
				G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "flamethrower kill" );
				break;
			case MOD_MORTAR:
				if( splash ) {
					G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
					G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "mortar splash damage kill" );
				}
				else {
					G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, xp );
					G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, xp, "mortar direct hit kill" );
				}
				break;
			case MOD_GRENADE_LAUNCHER:
			case MOD_GRENADE_PINEAPPLE:
			case MOD_SMOKEGRENADE:			//bani - airstrike marker kills
				G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, xp, "hand grenade kill" );
				break;
			case MOD_DYNAMITE:
			case MOD_LANDMINE:
				G_AddSkillPoints( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, xp + 1.0f );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, xp + 1.0f, "dynamite or landmine kill" );
				break;
			case MOD_ARTY:
				G_AddSkillPoints( attacker, SK_SIGNALS, xp + 1.0f );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_SIGNALS, xp + 1.0f, "artillery kill" );
				break;
			case MOD_AIRSTRIKE:
				G_AddSkillPoints( attacker, SK_SIGNALS, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_SIGNALS, xp, "airstrike kill" );
				break;
			case MOD_GPG40:
			case MOD_M7:
				G_AddSkillPoints( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, xp );
				G_DEBUG_ADD_SKILL_POINTS( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, xp, "rifle grenade kill" );
				break;
			case MOD_POISON:
				G_AddSkillPoints( attacker, SK_FIRST_AID, xp );
				break;

			// no skills for anything else
			default:
				break;
		}
	}
}

void G_AddKillSkillPointsForDestruction( gentity_t *attacker, meansOfDeath_t mod, g_constructible_stats_t *constructibleStats )
{
	switch( mod ) {
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
			G_AddSkillPoints( attacker, SK_LIGHT_WEAPONS, constructibleStats->destructxpbonus );
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_LIGHT_WEAPONS, constructibleStats->destructxpbonus, "destroying a constructible/explosive" );
			break;
		case MOD_GPG40:
		case MOD_M7:
		case MOD_DYNAMITE:
		case MOD_LANDMINE:
			G_AddSkillPoints( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, constructibleStats->destructxpbonus );
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_EXPLOSIVES_AND_CONSTRUCTION, constructibleStats->destructxpbonus, "destroying a constructible/explosive" );
			break;
		case MOD_PANZERFAUST:
		case MOD_MORTAR:
			G_AddSkillPoints( attacker, SK_HEAVY_WEAPONS, constructibleStats->destructxpbonus );
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_HEAVY_WEAPONS, constructibleStats->destructxpbonus, "destroying a constructible/explosive" );
			break;
		case MOD_ARTY:
		case MOD_AIRSTRIKE:
			G_AddSkillPoints( attacker, SK_SIGNALS, constructibleStats->destructxpbonus );
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_SIGNALS, constructibleStats->destructxpbonus, "destroying a constructible/explosive" );
			break;
		case MOD_SATCHEL:
			G_AddSkillPoints( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, constructibleStats->destructxpbonus );
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, constructibleStats->destructxpbonus, "destroying a constructible/explosive" );
			break;
		default:
			break;
	}
}

/////// SKILL DEBUGGING ///////
static fileHandle_t skillDebugLog = -1;

void G_DebugOpenSkillLog( void )
{
	if( g_debugSkills.integer < 2 )
		return;

	{
		vmCvar_t	mapname;
		qtime_t		ct;
		char		*s;

		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

		trap_RealTime( &ct );

		if( trap_FS_FOpenFile( va( "skills-%d-%02d-%02d-%02d%02d%02d-%s.log",
									1900+ct.tm_year, ct.tm_mon+1,ct.tm_mday,
									ct.tm_hour, ct.tm_min, ct.tm_sec,
									mapname.string ), &skillDebugLog, FS_APPEND_SYNC ) < 0 ) {
			return;
		}

		s = va( "%02d:%02d:%02d : Logfile opened.\n", ct.tm_hour, ct.tm_min, ct.tm_sec );

		trap_FS_Write( s, strlen( s ), skillDebugLog );
	}
}

void G_DebugCloseSkillLog( void )
{
	if( skillDebugLog == -1 )
		return;

	{
		qtime_t		ct;
		char		*s;

		trap_RealTime( &ct );

		s = va( "%02d:%02d:%02d : Logfile closed.\n", ct.tm_hour, ct.tm_min, ct.tm_sec );

		trap_FS_Write( s, strlen( s ), skillDebugLog );

		trap_FS_FCloseFile( skillDebugLog );
	}
}

void G_DebugAddSkillLevel( gentity_t *ent, skillType_t skill )
{
	if( !g_debugSkills.integer )
		return;

	{
		qtime_t		ct;

		// CHRUKER: b013 - Was printing the float with 6.2 as max. numbers
		trap_SendServerCommand( ent-g_entities, va( "sdbg \"^%c(SK: %2i XP: %.0f) %s: You raised your skill level to %i.\"\n",
									COLOR_RED + skill, ent->client->sess.skill[skill], ent->client->sess.skillpoints[skill], skillNames[skill], ent->client->sess.skill[skill] ) );

		trap_RealTime( &ct );

		if( g_debugSkills.integer >= 2 && skillDebugLog != -1 ) {
			// CHRUKER: b013 - Was printing the float with 6.2 as max. numbers
			char *s = va( "%02d:%02d:%02d : ^%c(SK: %2i XP: %.0f) %s: %s raised in skill level to %i.\n",
				ct.tm_hour, ct.tm_min, ct.tm_sec,
				COLOR_RED + skill, ent->client->sess.skill[skill], ent->client->sess.skillpoints[skill], skillNames[skill], ent->client->pers.netname, ent->client->sess.skill[skill] );
			trap_FS_Write( s, strlen( s ), skillDebugLog );
		}
	}
}

void G_DebugAddSkillPoints( gentity_t *ent, skillType_t skill, float points, const char *reason )
{
	qtime_t		ct;
// jet Pilot - should be obsolete now
//	if( !g_debugSkills.integer )
//		return;

	// CHRUKER: b013 - Was printing the float with 6.2 as max. numbers
	trap_SendServerCommand( ent-g_entities, va( "sdbg \"^%c(SK: %2i XP: %.0f) %s: You gained %.0fXP, reason: %s.\"\n",
								COLOR_RED + skill, ent->client->sess.skill[skill], ent->client->sess.skillpoints[skill], skillNames[skill], points, reason ) );

	trap_RealTime( &ct );

	if( g_debugSkills.integer >= 2 && skillDebugLog != -1 ) {
		// CHRUKER: b013 - Was printing the float with 6.2 as max. numbers
		char *s = va( "%02d:%02d:%02d : ^%c(SK: %2i XP: %.0f) %s: %s gained %.0fXP, reason: %s.\n",
			ct.tm_hour, ct.tm_min, ct.tm_sec,
			COLOR_RED + skill, ent->client->sess.skill[skill], ent->client->sess.skillpoints[skill], skillNames[skill], ent->client->pers.netname, points, reason );
		trap_FS_Write( s, strlen( s ), skillDebugLog );
	}
}


void jP_BuildEndgameStats(void)
{
	int			i,j;
	char		buffer[1024];
	gclient_t	*cl				= NULL;
	gclient_t	*best			= NULL;
	gclient_t	*best2			= NULL;
	float		xp, xpbest		= 0.0f;
	float		v, vbest		= 0.0f;
	int			clientNum		= -1;
	int			bestClientNum	= -1;
	int			best2ClientNum	= -1;
	qboolean	awardBestMedic	= qtrue;
	qboolean	awardLifeSaver	= qtrue;

	G_CalcClientAccuracies();

	
	/*
	for( i = 0; i < level.numConnectedClients; i++ ) {
		level.clients[ i ].hasaward = 0;	// unused
	}
	*/

	*buffer = '\0';

	//--- Highest Ranking Officer ----------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.rank > best->sess.rank) {
			best = cl;
			bestClientNum = clientNum;
		}
		else if (cl->sess.rank == best->sess.rank && cl->medals > best->medals) {
			best = cl;
			bestClientNum = clientNum;
		}
		else if (cl->sess.rank == best->sess.rank && cl->medals == best->medals && cl->ps.persistant[PERS_SCORE] > best->ps.persistant[PERS_SCORE]) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best ) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i 0 %i ", bestClientNum, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Most Experience ------------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->ps.persistant[PERS_SCORE] > best->ps.persistant[PERS_SCORE]) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->ps.persistant[PERS_SCORE] > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i 0 %i ", bestClientNum, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Most Kills - Highest Fragger ----------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.kills > best->sess.kills) {
			best = cl;
			bestClientNum = clientNum;
		}
		else if (cl->sess.kills == best->sess.kills) {
			// same score? check deaths too..
			if (cl->sess.deaths < best->sess.deaths) {
				best = cl;
				bestClientNum = clientNum;
			}
			else if (cl->sess.deaths == best->sess.deaths) {
				// same score? check team-kills too..
				if (cl->sess.team_kills < best->sess.team_kills) {
					best = cl;
					bestClientNum = clientNum;
				}
				else if (cl->sess.team_kills == best->sess.team_kills) {
					// same score? check team-damage too..
					if (cl->sess.team_damage < best->sess.team_damage) {
						best = cl;
						bestClientNum = clientNum;
					}
				}
			}
		}
	}
	if( best && best->sess.kills > 0) { // jaquboss - need at least one frag to be highest fragger
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.kills, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Most Headshots -------------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR || cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->headshots > best->headshots) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->headshots > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->headshots, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Highest Overall Accuracy ---------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR || cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->acc > best->acc) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->acc > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %.1f %i ", bestClientNum, best->acc, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Best Killing Spree ---------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR || cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || BG_GetStatBestSpree(&cl->ps) > BG_GetStatBestSpree(&best->ps)) {
			best = cl;
			bestClientNum = clientNum;
		}
		else if (BG_GetStatBestSpree(&cl->ps) == BG_GetStatBestSpree(&best->ps)) {
			// if scores are equal, compare the current spree too..
			if ( BG_GetKillSpree(&cl->ps) > BG_GetKillSpree(&best->ps) ) {
				best = cl;
				bestClientNum = clientNum;
			}
			// if current-spree scores are also equal, compare the kill count too..
			else if ( BG_GetKillSpree(&cl->ps) == BG_GetKillSpree(&best->ps) ) {
				// less total kills (with same spree) wins
				if ( cl->sess.kills < best->sess.kills ) {
					best = cl;
					bestClientNum = clientNum;
				}
			}
		}
	}
	if( best && BG_GetStatBestSpree(&best->ps) > 5) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, BG_GetStatBestSpree(&best->ps), best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Bounty Hunter --------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || BG_GetStatBounty(&cl->ps) > BG_GetStatBounty(&best->ps)) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && BG_GetStatBounty(&best->ps) > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, BG_GetStatBounty(&best->ps), best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Most XP Gain ---------------------------------------------
	xpbest = 0.0f;
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		xp = 0.0f;
		for (j=0; j<SK_NUM_SKILLS; j++) xp += (cl->sess.skillpoints[j] - cl->sess.mapstartSkillpoints[j]);
		if (!best || xp > xpbest) {
			best = cl;
			xpbest = xp;
			bestClientNum = clientNum;
		}
	}
	if( best && xpbest > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %.1f %i ", bestClientNum, xpbest, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: War Hero -------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.missionair > best->sess.missionair) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.missionair > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.missionair, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Best Engineer --------------------------------------------
	xpbest = 0.0f;
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		xp = cl->sess.skillpoints[SK_EXPLOSIVES_AND_CONSTRUCTION] - cl->sess.mapstartSkillpoints[SK_EXPLOSIVES_AND_CONSTRUCTION];
		if (!best || xp > xpbest) {
			best = cl;
			xpbest = xp;
			bestClientNum = clientNum;
		}
	}
	if( best && xpbest > 0) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %.1f %i ", bestClientNum, xpbest, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Best Medic AND/OR Life Saver -----------------------------
	xpbest = 0.0f;	// medic
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		xp = cl->sess.skillpoints[SK_FIRST_AID] - cl->sess.mapstartSkillpoints[SK_FIRST_AID];
		if (!best || xp > xpbest) {
			best = cl;
			xpbest = xp;
			bestClientNum = clientNum;
		}

		if (!best2 || cl->sess.resuscetations > best2->sess.resuscetations) {
			best2 = cl;
			best2ClientNum = clientNum;
		}
	}
	// There will be no Best-Medic award if someone else has a better score on rescusetations..
	// There will be no Life-Saver award if the best-medic is also the life-saver..
	if (best && best2) {
		if (best != best2) {
			if (best2->sess.resuscetations >= xpbest) {
				// no best medic award is given..
				awardBestMedic = qfalse;
			}
		}
		else {
			// best medic == life saver player
			if (best2->sess.resuscetations >= xpbest) {
				// no best medic award is given..
				awardBestMedic = qfalse;
			}
			else {
				// no life saver award is given..
				awardLifeSaver = qfalse;
			}
		}
	}
	if (!best || xpbest == 0) awardBestMedic = qfalse;
	if (!best2 || best2->sess.resuscetations == 0) awardLifeSaver = qfalse;
	// core: Best Medic
	if (awardBestMedic) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %.1f %i ", bestClientNum, xpbest, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;
	// core: Life Saver Award // IRATA: This is an exception of 'awards'
	if (awardLifeSaver) {
		//best2->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", best2ClientNum, (int)(best2->sess.resuscetations / 4), best2->sess.sessionTeam));	// 4 xp per resuscetation
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best2 = NULL;

	//--- jaquboss most knife kills - jack the ripper award -------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.aWeaponStats[WS_KNIFE].kills > best->sess.aWeaponStats[WS_KNIFE].kills) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.aWeaponStats[WS_KNIFE].kills > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.aWeaponStats[WS_KNIFE].kills, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: most poison kills - dr. overdose award ----------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.poisoneer > best->sess.poisoneer) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.poisoneer > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.poisoneer, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- jaquboss most executions - The Executioner Award-----------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.executions > best->sess.executions) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.executions > 2) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.executions, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Kick Ass Award -------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.kickass > best->sess.kickass) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.kickass > 3)	{
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.kickass, best->sess.sessionTeam));
	} else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Most Goombas Super Mario Award -----------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.goomba > best->sess.goomba ) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if (best && best->sess.goomba > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.goomba, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Purple Heart, most deaths but excluding Darwin/Newton/Panzernoob
	vbest = 0;
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		v = cl->sess.deaths - (cl->sess.panzernoob + cl->sess.darwin + cl->sess.newton);
		if ( !best || v > vbest ) {
			best = cl;
			bestClientNum = clientNum;
		}
		else if (v == vbest) {
			// if scores are equal, compare kills too..
			if (cl->sess.kills < best->sess.kills) {
				best = cl;
				bestClientNum = clientNum;
			}
			else if (cl->sess.team_kills == best->sess.team_kills) {
				// if scores are still equal, compare teamkills..
				if (cl->sess.team_kills < best->sess.team_kills) {
					best = cl;
					bestClientNum = clientNum;
				}
			}
		}
		vbest = best->sess.deaths - (best->sess.panzernoob + best->sess.darwin + best->sess.newton);
	}
	if( best && vbest > 10) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, (int)(vbest), best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: most (throwing) grenade kills ----------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.aWeaponStats[WS_GRENADE].kills > best->sess.aWeaponStats[WS_GRENADE].kills) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.aWeaponStats[WS_GRENADE].kills > 5) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.aWeaponStats[WS_GRENADE].kills, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: most riflenade kills -------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.aWeaponStats[WS_GRENADELAUNCHER].kills > best->sess.aWeaponStats[WS_GRENADELAUNCHER].kills) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.aWeaponStats[WS_GRENADELAUNCHER].kills > 5) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.aWeaponStats[WS_GRENADELAUNCHER].kills, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- core: Panzernoob Award (these kills do not count for Darwin) ---
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.panzernoob > best->sess.panzernoob) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.panzernoob > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.panzernoob, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Darwin Award ---------------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.darwin > best->sess.darwin) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.darwin > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.darwin, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- Isaac Newton Award ---------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || cl->sess.newton > best->sess.newton) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.newton > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.newton, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- jaquboss - no friends Award------------------------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if(!best || cl->sess.team_kills > best->sess.team_kills) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && best->sess.team_kills > 3) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.team_kills, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//--- jaquboss - arty spammer award, todo autokick? ---------------------------
	for (i = 0; i < level.numConnectedClients; i++) {
		clientNum = level.sortedClients[i];
		cl = &level.clients[clientNum];

		if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

		if (!best || (cl->sess.aWeaponStats[WS_ARTILLERY].atts+cl->sess.aWeaponStats[WS_AIRSTRIKE].atts) > (best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts)) {
			best = cl;
			bestClientNum = clientNum;
		}
	}
	if( best && (best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts) > 5) {
		//best->hasaward++;
		Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts, best->sess.sessionTeam));
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	//---------- Soul Collector - Most Dogtags Collected ----------
	if ( jp_insanity.integer & JP_INSANITY_DOGTAGS ) {
		for (i = 0; i < level.numConnectedClients; i++) {
			clientNum = level.sortedClients[i];
			cl = &level.clients[clientNum];

			if( cl->sess.sessionTeam == TEAM_SPECTATOR|| cl->sess.sessionTeam == TEAM_FREE ) continue;

			if (!best || (cl->sess.dogtags) > (best->sess.dogtags)) {
				best = cl;
				bestClientNum = clientNum;
			}
		}
		if( best && (best->sess.dogtags) > 3) {
			//best->hasaward++;
			Q_strcat(buffer, 1024, va("%i %i %i ", bestClientNum, best->sess.dogtags, best->sess.sessionTeam));
		}
		else {
			Q_strcat(buffer, 1024, "-1 0 -1 ");
		}
	}
	else {
		Q_strcat(buffer, 1024, "-1 0 -1 ");
	}
	best = NULL;

	// closing entry
	Q_strcat(buffer, 1024, "-1 0 -1 ");

	// All done, now send the configstring to the clients
	trap_SetConfigstring( CS_ENDGAME_STATS, buffer );
}


// Michael
void G_ReassignSkillLevel(skillType_t skill)
{
	int count;
	for( count = 0; count < g_maxclients.integer; count++ ) {
		// doesn't work for those in xpsave mode?
		if(level.gentities[count].client != 0 &&
			level.gentities[count].client->pers.connected == CON_CONNECTED){
				G_SetPlayerSkill(level.gentities[count].client, skill);
				G_UpgradeSkill( &level.gentities[count], skill );
		}
	}
}
