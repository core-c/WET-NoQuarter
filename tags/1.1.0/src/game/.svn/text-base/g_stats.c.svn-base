#include "g_local.h"

void G_LogDeath( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	ent->client->pers.playerStats.weaponStats[weap].killedby++;

	trap_PbStat ( ent - g_entities , "death" , 
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
}

void G_LogKill( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	// CHRUKER: b068 - This block didn't do anything, so it has been
	//          commented out.
	/* 
	if(ent->client->sess.playerType == PC_SOLDIER) {
		int i, j;
		qboolean pass = qtrue;

		ent->client->soliderKillTimes[ent->client->soldierKillMarker++] = level.timeCurrent;

		if ( ent->client->soldierKillMarker >= NUM_SOLDIERKILL_TIMES ) {
			ent->client->soldierKillMarker = 0;
		}

		for( i = 0, j = ent->client->soldierKillMarker; i < NUM_SOLDIERKILL_TIMES; i++ ) {

			if( !ent->client->soliderKillTimes[j] || (ent->client->soliderKillTimes[j] < level.timeCurrent - SOLDIERKILL_MAXTIME) ) {
				pass = qfalse;
				break;
			}

			if( ++j == NUM_SOLDIERKILL_TIMES ) {
				j = 0;
			}
		}
	}
	*/

	ent->client->pers.playerStats.weaponStats[weap].kills++;

	trap_PbStat ( ent - g_entities , "kill" , 
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
}

void G_LogTeamKill( gentity_t* ent, weapon_t weap ) {
	weap = BG_DuplicateWeapon(weap);

	if(!ent->client) {
		return;
	}

	ent->client->pers.playerStats.weaponStats[weap].teamkills++;

	trap_PbStat ( ent - g_entities , "tk" , 
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , weap ) ) ;
}

void G_LogRegionHit( gentity_t* ent, hitRegion_t hr ) {
	if(!ent->client) {
		return;
	}
	ent->client->pers.playerStats.hitRegions[hr]++;

	trap_PbStat ( ent - g_entities , "hr" , 
		va ( "%d %d %d" , ent->client->sess.sessionTeam , ent->client->sess.playerType , hr ) ) ;
}

void G_PrintAccuracyLog( gentity_t *ent ) {
	int i;
	char buffer[2048];

	Q_strncpyz(buffer, "WeaponStats", 2048);

	for( i = 0; i < WP_NUM_WEAPONS; i++ ) {
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
/*
	// jet Pilot - communicate extra stats to client
	Q_strcat( buffer, 2048, va(" %i", ent->client->pmext.killSpree ));
	Q_strcat( buffer, 2048, va(" %i", ent->client->pmext.goombas ));
	Q_strcat( buffer, 2048, va( "%i", ent->client->pmext.darwinDeaths ));
*/

	trap_SendServerCommand( ent-g_entities, buffer );
}

void G_SetPlayerScore( gclient_t *client ) {
	int i;

	for( client->ps.persistant[PERS_SCORE] = 0, i = 0; i < SK_NUM_SKILLS; i++ ) {
		client->ps.persistant[PERS_SCORE] += client->sess.skillpoints[i];
	}
}

void G_SetPlayerSkill( gclient_t *client, skillType_t skill ) {
	int i;
	int	skillMax;

	// jet Pilot - limit max levels
	switch (skill)
	{
		case SK_BATTLE_SENSE:
			skillMax = jp_maxLevelBattleSense.integer;
			break;
		case SK_ENGINEER:
			skillMax = jp_maxLevelEngineer.integer;
			break;
		case SK_MEDIC:
			skillMax = jp_maxLevelMedic.integer;
			break;		
		case SK_FIELDOP:
			skillMax = jp_maxLevelFieldOp.integer;
			break;		
		case SK_LIGHT_WEAPONS:
			skillMax = jp_maxLevelLightWeapons.integer;
			break;
		case SK_SOLDIER:
			skillMax = jp_maxLevelSoldier.integer;
			break;
		case SK_COVERTOP:
			skillMax = jp_maxLevelCovertOp.integer;
			break;
		default:
			skillMax = 5;
			break;
	}

	for( i = skillMax - 1; i >= 0; i-- ) 
	{
		if( client->sess.skillpoints[skill] >= skillLevels[skill][i] )
		{
			client->sess.skill[skill] = i;
			break;
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
static void G_UpgradeSkill( gentity_t *ent, skillType_t skill ) {
	int i, cnt = 0;
	int next=-1;


	G_DebugAddSkillLevel( ent, skill );

	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		cnt += ent->client->sess.skill[ i ];
	}

	// recompute rank..
	ent->client->sess.rank = 0;

	for ( i=NUM_EXPERIENCE_LEVELS-1; i> 0; i-- )
	{
		next = rankTable[i-1];
		if ( cnt >= next && ent->client->sess.rank < NUM_EXPERIENCE_LEVELS-1  ) {
			ent->client->sess.rank=i;
			break;
		}
	}

	ClientUserinfoChanged( ent-g_entities );

	// Give em rightaway
	if( skill == SK_BATTLE_SENSE && ent->client->sess.skill[skill] == 1 ) {
		if( AddWeaponToPlayer( ent->client, WP_BINOCULARS, 1, 0, qfalse ) ) {
			ent->client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	} else if( skill == SK_FIRST_AID && ent->client->sess.playerType == PC_MEDIC && ent->client->sess.skill[skill] == 4 && !(g_medics.integer & MEDIC_NOSELFADREN)) {
		int needles = 10;

		if ( g_adrenaline.integer & ADF_JUSTONE )
			needles = 1;

		if ( g_adrenaline.integer & ADF_NOSPAWN )
			needles = 0;

		AddWeaponToPlayer( ent->client, WP_MEDIC_ADRENALINE, 0, needles, qfalse );
	} else if( skill == SK_ENGINEER && ent->client->sess.playerType == PC_ENGINEER && ent->client->sess.skill[skill] == 4 ) {
		 ent->client->ps.powerups[PW_FLAKJACKET] = INT_MAX;
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

void G_LoseSkillPoints(gentity_t *ent, skillType_t skill, float points) {  
	float oldskillpoints;

	if(!ent || !ent->client)  
		return;

	if(ent->client->sess.sessionTeam != TEAM_AXIS &&  
			ent->client->sess.sessionTeam != TEAM_ALLIES) {

		return;  
	}
	
// no skill loss during warmup  
	if( g_gamestate.integer != GS_PLAYING ) {  
		return;  
	}  

	oldskillpoints = ent->client->sess.skillpoints[skill];

	if(!G_LoseSkillPointsExt(ent, skill, points))  
	return; 

	// CHRUKER: b013 - Was printing this with many many decimals
			G_Printf( "%s just lost %.0f skill points for skill %s\n",  
	                 ent->client->pers.netname,  
	                 oldskillpoints - ent->client->sess.skillpoints[skill],  
	                 skillNames[skill]);  
	   
	        trap_PbStat ((ent - g_entities), "loseskill" ,   
	                 va("%d %d %d %f",  
	                         ent->client->sess.sessionTeam,  
	                         ent->client->sess.playerType,   
	                         skill,  
	                         oldskillpoints - ent->client->sess.skillpoints[skill]));  
	}  
	   
	void G_XPDecay(gentity_t *ent, int seconds, qboolean force)  
	{  
	         int i =  0;  
	         float points;  
	         float p;  
	         // translates primary SK_ enum for each PC_ define  
			 //int skill_for_class[] = {5,2,1,3,6};  
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
					else if(!force && (g_XPDecay.integer & XPDF_NO_BS_DECAY) &&  
								i == SK_BATTLE_SENSE) {  	
								continue;  
					}  
					else if(!force && (g_XPDecay.integer & XPDF_NO_LW_DECAY) &&  
								i == SK_LIGHT_WEAPONS) {	                   
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
					if((ent->client->sess.skillpoints[i] - p) <  
						(g_XPDecayFloor.value * 1.0f)) {

					p = (ent->client->sess.skillpoints[i] -   
						(g_XPDecayFloor.value * 1.0f));  
					}  
					// tjw: don't decay past 0  
					if(ent->client->sess.skillpoints[i] < p)  
					p = ent->client->sess.skillpoints[i];  

	                 G_LoseSkillPointsExt(ent, i, p);  
	         }      
}

void G_ResetXP(gentity_t *ent)  
{  
	int i = 0;  
	int     ammo[MAX_WEAPONS];  
	int     ammoclip[MAX_WEAPONS];

	if(!ent || !ent->client)  
			return;  
		ent->client->sess.rank = 0;  
		for(i = 0; i < SK_NUM_SKILLS; i++) {  
				ent->client->sess.skillpoints[i] = 0.0f;  
				ent->client->sess.skill[i] = 0;  
		}  
				G_CalcRank(ent->client);  
				ent->client->ps.stats[STAT_XP] = 0;  
				ent->client->ps.persistant[PERS_SCORE] = 0;

				// tjw: zero out all weapons and grab the default weapons for  
				//      a player of this XP level.  
				memset(ent->client->ps.weapons, 0,  
					sizeof(*ent->client->ps.weapons));  
				memcpy(ammo, ent->client->ps.ammo, sizeof(ammo));  
				memcpy(ammoclip, ent->client->ps.ammoclip, sizeof(ammoclip));  
				SetWolfSpawnWeapons(ent->client);  
				memcpy(ent->client->ps.ammo, ammo, sizeof(ammo));  
				memcpy(ent->client->ps.ammoclip, ammoclip, sizeof(ammoclip));  
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
	 
	if(g_maxXP.integer >= 0 &&  
		(ent->client->ps.persistant[PERS_SCORE] + points) >=  
			g_maxXP.integer) {

			G_ResetXP(ent);
		// tjw: be silent if g_maxXP is 0  
			if(g_maxXP.integer)  
				CP("cp \"The good news is you've reached g_maxXP, "  
					"the bad news is now you lose it all.\" 1");  
			return;  
	}  


	level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += points;
	// CHRUKER: b013 - Was printing this with many many decimals
//	G_Printf( "%s just got %.0f skill points for skill %s\n", ent->client->pers.netname, points, skillNames[skill] );

	trap_PbStat ( ent - g_entities , "addskill" , 
		va ( "%d %d %d %f" , ent->client->sess.sessionTeam , ent->client->sess.playerType , 
			skill , points ) ) ;

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

	switch( mod ) 
	{
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
		case MOD_STEN_MKII:		// jP
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
		
		// Hand Grenades
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:

		// Shotgun		
		case MOD_SHOTGUN:		// jP			
		
			G_LoseSkillPoints( tker, SK_LIGHT_WEAPONS, 3.f ); 
			break;

	// Covert Ops Weapons		
		case MOD_GARAND_SCOPE:
		case MOD_K43_SCOPE:
		case MOD_FG42SCOPE:
		case MOD_SATCHEL:
			G_LoseSkillPoints( tker, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 3.f );
			break;

	// Heavy Weapons
		case MOD_JETGUN:
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
			G_LoseSkillPoints( tker, SK_HEAVY_WEAPONS, 3.f );
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
			G_LoseSkillPoints( tker, SK_SIGNALS, 3.f );
			break;

	// Goomba Kills
		case MOD_KICKED:
		case MOD_SHOVE:
		case MOD_GOOMBA:
			G_LoseSkillPoints( tker, SK_BATTLE_SENSE, 2.f );
			break;

	// no skills for anything else
		default:
			break;
	}
}

void G_AddKillSkillPoints( gentity_t *attacker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash, float bounty ) 
{

	gentity_t *ent;

	// jet Pilot - add 'bounty' experience for ending killing sprees
	float xp = 3.0f;			// 3.0 is the default for most kills

	if( !attacker->client )
		return;

	if (bounty > 1)
	{
		AP(va("cpm \"%s ^9has claimed a bounty of ^2%d ^9points!^7\"", attacker->client->pers.netname, (int)bounty));
		xp += bounty;
		BG_AdjustBounty(&attacker->client->ps, bounty);
	} 
	else if (bounty == 1)
	{
		AP(va("cpm \"%s ^9has claimed a bounty point!^7\"", attacker->client->pers.netname));
		xp += bounty;
		BG_AdjustBounty(&attacker->client->ps, bounty);
	}

	ent = attacker;

	switch( mod ) 
	{
		// jaquboss - give some xp for it
		case MOD_KICKED:
			G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp  );	
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp, "kick kill" ); 
			break;
		case MOD_SHOVE:			
			G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp + 2.0f );	//Meyer - give more xp
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp, "shove kill" ); 
			break;
		// jet Pilot - give xp for goombas
		case MOD_GOOMBA:			
			G_AddSkillPoints( attacker, SK_BATTLE_SENSE, xp + 2.0f ); 
			G_DEBUG_ADD_SKILL_POINTS( attacker, SK_BATTLE_SENSE, xp + 2.0f, "goomba kill" ); 

			// jet Pilot - increment goomba kills
			BG_AdjustGoombas(&attacker->client->ps, 1);
			break;

		// light weapons
		case MOD_THROWKNIFE:
		case MOD_KNIFE:
		case MOD_BACKSTAB:
//		case MOD_BAYONETTE:
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
		case MOD_STEN_MKII:
		case MOD_SHOTGUN:
		case MOD_MP34:
		case MOD_FIREBOLT:
			switch( hr ) 
			{
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
			switch( hr ) 
			{
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
		case MOD_FG42SCOPE:
			switch( hr ) 
			{
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

		case MOD_JETGUN:
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
			} else {
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
			} else {
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
	vmCvar_t	mapname;
	qtime_t		ct;
	char		*s;

	if( g_debugSkills.integer < 2 )
		return;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	trap_RealTime( &ct );

	if( trap_FS_FOpenFile( va( "skills-%d-%02d-%02d-%02d%02d%02d-%s.log",
								1900+ct.tm_year, ct.tm_mon+1,ct.tm_mday,
								ct.tm_hour, ct.tm_min, ct.tm_sec,
								mapname.string ), &skillDebugLog, FS_APPEND_SYNC ) < 0 )
		return;

	s = va( "%02d:%02d:%02d : Logfile opened.\n", ct.tm_hour, ct.tm_min, ct.tm_sec );

	trap_FS_Write( s, strlen( s ), skillDebugLog );
}

void G_DebugCloseSkillLog( void )
{
	qtime_t		ct;
	char		*s;

	if( skillDebugLog == -1 )
		return;

	trap_RealTime( &ct );

	s = va( "%02d:%02d:%02d : Logfile closed.\n", ct.tm_hour, ct.tm_min, ct.tm_sec );

	trap_FS_Write( s, strlen( s ), skillDebugLog );

	trap_FS_FCloseFile( skillDebugLog );
}

void G_DebugAddSkillLevel( gentity_t *ent, skillType_t skill )
{
	qtime_t		ct;

	if( !g_debugSkills.integer )
		return;

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
// CHRUKER: b017 - Added a check to make sure that the best result is larger than 0
#define CHECKSTAT1( XX )														\
	best = NULL;																\
	for( i = 0; i < level.numConnectedClients; i++ ) {							\
		gclient_t* cl = &level.clients[ level.sortedClients[ i ] ];				\
		if( cl->sess.sessionTeam == TEAM_SPECTATOR ) {							\
			continue;															\
		}																		\
		if( cl->XX <= 0 ) {														\
			 continue; 															\
		}																		\
		if( !best || cl->XX > best->XX ) {									\
			best = cl;															\
		}																		\
	}																			\
	if( best ) { best->hasaward = qtrue; }										\
	Q_strcat( buffer, 1024, va( ";%s (%i); %i ", best ? best->pers.netname : "", best ? best->XX : 0, best ? best->sess.sessionTeam : -1 ) )

#define CHECKSTATMIN( XX, YY )													\
	best = NULL;																\
	for( i = 0; i < level.numConnectedClients; i++ ) {							\
		gclient_t* cl = &level.clients[ level.sortedClients[ i ] ];				\
		if( cl->sess.sessionTeam == TEAM_SPECTATOR ) {							\
			continue;															\
		}																		\
		if( !best || cl->XX > best->XX ) {									\
			best = cl;															\
		}																		\
	}																			\
	if( best ) { best->hasaward = qtrue; }										\
	Q_strcat( buffer, 1024, va( ";%s; %i ", best && best->XX >= YY ? best->pers.netname : "", best && best->XX >= YY ? best->sess.sessionTeam : -1 ) )
// CHRUKER: b017 - Moved the minimum skill requirement into a seperate if sentence
// Added a check to make sure only people who have increased their skills are considered
#define CHECKSTATSKILL( XX )															\
	best = NULL;																\
	for( i = 0; i < level.numConnectedClients; i++ ) {							\
		gclient_t* cl = &level.clients[ level.sortedClients[ i ] ];				\
		if( cl->sess.sessionTeam == TEAM_SPECTATOR ) {							\
			continue;															\
		}																		\
		if ((cl->sess.skillpoints[XX] - cl->sess.startskillpoints[XX]) <= 0) {	\
			continue;															\
		}																		\
		if( cl->sess.skill[XX] < 1 ) {											\
			continue;															\
		}																		\
		if( !best || (cl->sess.skillpoints[XX] - cl->sess.startskillpoints[XX]) > (best->sess.skillpoints[XX] - best->sess.startskillpoints[XX]) ) { \
			best = cl;															\
		}																		\
	}																			\
	if( best ) { best->hasaward = qtrue; }										\
	Q_strcat( buffer, 1024, va( ";%s; %i ", best ? best->pers.netname : "", best && best->sess.skillpoints[XX] >= 20 ? best->sess.sessionTeam : -1 ) )

#define CHECKSTAT3( XX, YY, ZZ )												\
	best = NULL;																\
	for( i = 0; i < level.numConnectedClients; i++ ) {							\
		gclient_t* cl = &level.clients[ level.sortedClients[ i ] ];				\
		if( cl->sess.sessionTeam == TEAM_SPECTATOR ) {							\
			continue;															\
		}																		\
		if( !best || cl->XX > best->XX ) {									\
			best = cl;															\
		} else if( cl->XX == best->XX && cl->YY > best->YY) {			\
			best = cl;															\
		} else if( cl->XX == best->XX && cl->YY == best->YY && cl->ZZ > best->ZZ) {			\
			best = cl;															\
		}																		\
	}																			\
	if( best ) { best->hasaward = qtrue; }										\
	Q_strcat( buffer, 1024, va( ";%s; %i ", best ? best->pers.netname : "", best ? best->sess.sessionTeam : -1 ) )

#define CHECKSTATTIME( XX, YY )													\
	best = NULL;																\
	for( i = 0; i < level.numConnectedClients; i++ ) {							\
		gclient_t* cl = &level.clients[ level.sortedClients[ i ] ];				\
		if( cl->sess.sessionTeam == TEAM_SPECTATOR ) {							\
			continue;															\
		}																		\
		if( !best || (cl->XX/(float)(level.time - cl->YY)) > (best->XX/(float)(level.time - best->YY)) ) {\
			best = cl;															\
		}																		\
	}																			\
	if( best ) {																\
		if( (best->sess.startxptotal - best->ps.persistant[PERS_SCORE]) >= 100 || best->medals || best->hasaward) {	\
			best = NULL;														\
		}																		\
	}																			\
	Q_strcat( buffer, 1024, va( ";%s; %i ", best ? best->pers.netname : "", best ? best->sess.sessionTeam : -1 ) )


void jP_BuildEndgameStats(void)
{
	gclient_t	*cl				= NULL;
	gclient_t	*best			= NULL;
	int			i;
	char		buffer[1024];

	G_CalcClientAccuracies();

	for( i = 0; i < level.numConnectedClients; i++ ) {
		level.clients[ i ].hasaward = qfalse;
	}

	*buffer = '\0';

// Most Kills	
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];
		
		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.kills > best->sess.kills)
		{
			best = cl;
		}			
	}
	if( best && best->sess.kills > 0)  // jaquboss - need at least one frag to be highest fragger
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname,	best->sess.kills, best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ; %i ", "", -1));	
	}

	best = NULL;

// Most Experience
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->ps.persistant[PERS_SCORE] > best->ps.persistant[PERS_SCORE])
		{
			best = cl;
		}			
	}
	if( best && best->ps.persistant[PERS_SCORE] > 0) 
	{
		best->hasaward = qtrue; 
	}
	
	Q_strcat(buffer, 1024, va(";%s; %i ", best ? best->pers.netname : "", best ? best->sess.sessionTeam : -1));
	best = NULL;

// Highest Ranking Officer
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.rank > best->sess.rank)
		{
			best = cl;
		}
		else if (cl->sess.rank == best->sess.rank && cl->medals > best->medals)
		{
			best = cl;
		}
		else if (cl->sess.rank == best->sess.rank && cl->medals == best->medals && cl->ps.persistant[PERS_SCORE] > best->ps.persistant[PERS_SCORE])
		{
			best = cl;
		}
	}
	if( best ) 
	{
		best->hasaward = qtrue; 
	}

	Q_strcat(buffer, 1024, va(";%s; %i ", best ? best->pers.netname : "", best ? best->sess.sessionTeam : -1));
	best = NULL;

// Most Goombas
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || (BG_GetStatGoombas(&cl->ps) > BG_GetStatGoombas(&best->ps) ))
		{
			best = cl;
		}			
	}
//	if( best && cl->ps.persistant[PERS_GOOMBAS] > 0) 
	if (best && BG_GetStatGoombas(&best->ps) > 0)
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, BG_GetStatGoombas(&best->ps), best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ; %i ", "", -1));
	}
	
	best = NULL;

// Best Killing Spree
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;
		
		if (!best || BG_GetStatBestSpree(&cl->ps) > BG_GetStatBestSpree(&best->ps))
		{
			best = cl;
		}			
	}

	if( best && BG_GetStatBestSpree(&best->ps) > 5) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, BG_GetStatBestSpree(&best->ps), best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}
	
	best = NULL;			

// Most Headshots
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;
		
		if (!best || cl->headshots > best->headshots)
		{
			best = cl;
		}			
	}
	
	if( best && best->headshots > 0) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->headshots, best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}
	
	best = NULL;

// Darwin Awards
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || BG_GetStatDarwin(&cl->ps) > BG_GetStatDarwin(&best->ps))
		{
			best = cl;
		}			
	}

	if( best && BG_GetStatDarwin(&best->ps) > 0) 
	{ 
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, BG_GetStatDarwin(&best->ps), best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ; %i ", "",	-1));
	}

	best = NULL;

// Isaac Newton Award
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.newton > best->sess.newton)
		{
			best = cl;
		}			
	}

	if( best && best->sess.newton > 0) 
	{ 
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->sess.newton, best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ; %i ", "",	-1));
	}

	best = NULL;
	
// Best Medic

// Best Engineer

// Highest Light Weapons Accuracy (Same as overall accuracy for now)
/*
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->acc > best->acc) 
		{
			best = cl;
		}			
	}
	
	if( best && best->acc > 0) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->acc, best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}
	
	best = NULL;
*/
// Highest Overall Accuracy

	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->acc > best->acc) 
		{
			best = cl;
		}			
	}
	if( best && best->acc > 0) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%.1f); %i ", 
			best->pers.netname, best->acc, best->sess.sessionTeam));
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}

	best = NULL;

// jaquboss - no friends
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.team_kills > best->sess.team_kills) 
		{
			best = cl;
		}			
	}
	if( best && best->sess.team_kills > 4) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->sess.team_kills, best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}

	best = NULL;

// Bounty Hunter Award
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || BG_GetStatBounty(&cl->ps) > BG_GetStatBounty(&best->ps))
		{
			best = cl;
		}			
	}

	if( best && BG_GetStatBounty(&best->ps) > 0) 
	{ 
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i points); %i ", 
			best->pers.netname, BG_GetStatBounty(&best->ps), best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ; %i ", "",	-1));
	}

	best = NULL;

// jaquboss - arty noob, todo autokick?
	
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || (cl->sess.aWeaponStats[WS_ARTILLERY].atts+cl->sess.aWeaponStats[WS_AIRSTRIKE].atts) > (best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts)) 
		{
			best = cl;
		}			
	}
	if( best && (best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts) > 5) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->sess.aWeaponStats[WS_ARTILLERY].atts+best->sess.aWeaponStats[WS_AIRSTRIKE].atts, best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}

	best = NULL;

// jaquboss most knife kills
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.aWeaponStats[WS_KNIFE].kills > best->sess.aWeaponStats[WS_KNIFE].kills) 
		{
			best = cl;
		}			
	}
	if( best && best->sess.aWeaponStats[WS_KNIFE].kills > 0) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->sess.aWeaponStats[WS_KNIFE].kills, best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}

	best = NULL;


// jaquboss most executions
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if (!best || cl->sess.executions > best->sess.executions) 
		{
			best = cl;
		}			
	}
	if( best && best->sess.executions > 0) 
	{
		best->hasaward = qtrue; 
		Q_strcat(buffer, 1024, va(";%s ^7(%i); %i ", 
			best->pers.netname, best->sess.executions, best->sess.sessionTeam));	
	}
	else
	{
		Q_strcat(buffer, 1024, va(";%s ;%i ", "", -1));
	}

	best = NULL;

// Welcome Newbie! Award
/*
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[ level.sortedClients[ i ] ];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR) continue;

		if( !best || (cl->ps.persistant[PERS_SCORE]/(float)(level.time - cl->pers.enterTime)) > (best->ps.persistant[PERS_SCORE]/(float)(level.time - best->pers.enterTime))) 
		{
			best = cl;															
		}
		if( (best->sess.startxptotal - best->ps.persistant[PERS_SCORE]) >= 100 || best->medals || best->hasaward)
		{
			best = NULL;
		}
	}	*/
	Q_strcat(buffer, 1024, va(";%s; %i ", best ? best->pers.netname : "", best ? best->sess.sessionTeam : -1));

// All done, now send the configstring to the clients
	trap_SetConfigstring( CS_ENDGAME_STATS, buffer );
}

// jet Pilot - Determine Awards Here
void G_BuildEndgameStats( void ) 
{
	char buffer[1024];
	int i;
//	gclient_t* best;

	G_CalcClientAccuracies();

	for( i = 0; i < level.numConnectedClients; i++ ) {
		level.clients[ i ].hasaward = qfalse;
	}

	*buffer = '\0';	

	trap_SetConfigstring( CS_ENDGAME_STATS, buffer );
}

// Michael
void G_ReassignSkillLevel(skillType_t skill)
{
	int count;
	for( count = 0; count < MAX_CLIENTS; count++ ){
		// doesn't work for those in xpsave mode?
		if(level.gentities[count].client != 0 &&
			level.gentities[count].client->pers.connected == CON_CONNECTED){
				G_SetPlayerSkill(level.gentities[count].client, skill);
				G_UpgradeSkill( &level.gentities[count], skill );
		}
	}
}

qboolean G_HasFlakJacket(gentity_t *ent)
{
	if ( ent->client->ps.powerups[PW_FLAKJACKET])
		return qtrue;
	// jaquboss, engineers will get flakjacket powerup
	/*if ( ent->client->sess.skill[SK_ENGINEER] >= 4 && (ent->client->sess.playerType == PC_ENGINEER || jp_keepAwards.integer & JP_KEEP_FLAKJACKET))
	{
		return qtrue;
	}*/
	return qfalse;
}
