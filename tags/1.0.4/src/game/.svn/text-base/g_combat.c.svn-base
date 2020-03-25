/*
 * name:		g_combat.c
 *
 * desc:		
 *
*/

#include "g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"		//bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h"			//bot ai interface
#include "../botai/ai_main.h"
#include "../botai/chars.h"
#include "../botai/ai_team.h"
#include "../botai/ai_dmq3.h"
#include "etpro_mdx.h"

extern void BotRecordKill( int client, int enemy );
extern void BotRecordPain( int client, int enemy, int mod );
extern void BotRecordDeath( int client, int enemy );
#if 0
static void G_Obituary(int mod, int target, int attacker);
#endif
extern vec3_t muzzleTrace;

/*
jet Pilot - Check KillSpree levels, set and announce as necessary
*/

extern vmCvar_t		g_spreeSound[10];


void jP_UpdateSpree(gentity_t *ent, gentity_t *att, qboolean death, qboolean mapEnd)
{
	static int	spreeLevel[MAX_CLIENTS];
	static int	first = 1;
	gentity_t	*te;

	int			i	  = ent->s.clientNum;
	int			spree;


	if (first)
	{
		memset(&spreeLevel, 0, sizeof(spreeLevel));
		first = 0;
	}	

	spree = BG_GetKillSpree(&ent->client->ps); //.persistant[PERS_KILLSPREE];

	// clear killspree if player dies
	if (spree >= 5 && ( death || mapEnd ) && g_announcer.integer & ANNOUNCE_KILLINGSPREES)
	{
		// Update our best killing spree if it's less than our current one
		if (BG_GetStatBestSpree(&ent->client->ps) < spree)
			BG_SetStatBestSpree(&ent->client->ps, spree);			

		if ( mapEnd ) 
			AP(va("chat \"%s^d's killing spree ended after ^3%d^d kills! ^d(^1MAP END!^d)\"", ent->client->pers.netname, spree));
		else if (att == NULL || !att->client || ent == att)
			AP(va("chat \"%s^d's killing spree ended after ^3%d^d kills! ^d(^1SUICIDE!^d)\"", ent->client->pers.netname, spree));
		else
			AP(va("chat \"%s^d's killing spree was brought to an end by ^*%s%s ^dafter ^3%d ^dkills!\"", ent->client->pers.netname, OnSameTeam(ent, att) ? "^1TEAMMATE " : "", att->client->pers.netname, spree));

		spreeLevel[i] = 0;
	}
	// clear deathspree if player kills
	if (spree <= -10 && ( !death || mapEnd ) &&  g_announcer.integer & ANNOUNCE_DEATHSPREES)
	{
		spreeLevel[i] = 0;

		AP(va("chat \"%s^d's death spree has ended after ^3%d ^ddeaths!\"", ent->client->pers.netname, -spree));

	}

    // Increment spree if the player makes a kill, if spree is still less than one, set it to one.
	// Set spree to zero if the player dies, unless it's already zero or less, in which case decrement it
	spree = death ? (spree > 0 ? 0 : spree - 1) : (spree < 0 ? 1 : spree + 1);

	if (spree >= 5 && g_announcer.integer & ANNOUNCE_KILLINGSPREES)
	{
		if ((spree % 5) == 0)		// spree is a multiple of five
		{			

			AP(va("chat \"^1KILLING SPREE! ^*%s ^*%s ^d(^3%d ^dkills in a row!)\"", ent->client->pers.netname, g_spreeMessage[ spreeLevel[i] <= 9 ? spreeLevel[i] : 9 ].string, spree ));
			
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex(g_spreeSound[spreeLevel[i] < 10 ? spreeLevel[i] : 9].string);
			te->r.svFlags |= SVF_BROADCAST;

			spreeLevel[i]++;		// increment the current 'level' of the killing spree
		}
	}
	else if (spree <= -10 && g_announcer.integer & ANNOUNCE_DEATHSPREES)
	{
		if ((-spree % 10) == 0)		// -spree is a multiple of 5
		{			
			AP(va("chat \"^1DEATH SPREE! ^*%s ^*%s ^d(^3%d ^ddeaths without a kill!)\"", ent->client->pers.netname,  g_humiliationMessage[ -spreeLevel[i] <= 5 ? -spreeLevel[i] : 5 ].string, -spree));
		
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex(g_humiliationSound[-spreeLevel[i] <= 5 ? -spreeLevel[i] : 5].string);
			te->r.svFlags |= SVF_BROADCAST;


			spreeLevel[i]--;		// decrement the current 'level' of the killing spree
		}	
	}

	BG_SetKillSpree(&ent->client->ps, spree);
	// make sure spreeLevel never goes below zero, as we're using an unsigned char for netcode

	if (spreeLevel[i] >= 0)
		BG_SetStatBounty(&ent->client->ps, spreeLevel[i]);
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score )
{
	if ( !ent || !ent->client ) 
	{
		return;
	}
	
	// no scoring during pre-match warmup
	if ( g_gamestate.integer != GS_PLAYING ) 
	{
		return;
	}

	// no score in LMS
	if( g_gametype.integer == GT_WOLF_LMS ) 
	{
		return;
	}

	//ent->client->ps.persistant[PERS_SCORE] += score;
	ent->client->sess.game_points += score;

//	level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	CalculateRanks();
}

/*
============
AddKillScore

Adds score to both the client and his team, only used for playerkills, for lms
============
*/
void AddKillScore( gentity_t *ent, int score ) {
	if ( !ent || !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

	// someone already won
	if( level.lmsWinningTeam )
		return;

	if( g_gametype.integer == GT_WOLF_LMS ) {
		ent->client->ps.persistant[PERS_SCORE] += score;
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	ent->client->sess.game_points += score;


	CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
extern char *nq_wpNames[];

void TossClientItems( gentity_t *self ) 
{
	weapon_t primaryWeapon=WP_NONE;
	weapon_t secondaryWeapon=WP_NONE;

	if( g_gamestate.integer == GS_INTERMISSION ) {
		return;
	}

//	primaryWeapon = G_GetWeaponsForClient( self->client, secondaryWeapon );
	// jaquboss, above doesnt work properly, this does
	primaryWeapon = G_GetPrimaryWeaponForClient(self->client);
	secondaryWeapon = G_GetSecondaryWeaponForClient( self->client, primaryWeapon );
#ifdef DEBUG
	G_Printf("^7Primary : ^9%i (%s^9) ^7Secondary : ^9%i (%s^9)\n",
		primaryWeapon, 
		(primaryWeapon >= 0 && primaryWeapon < WP_NUM_WEAPONS) ? nq_wpNames[primaryWeapon] : "^1invalid",
		secondaryWeapon, 
		(secondaryWeapon >= 0 && secondaryWeapon < WP_NUM_WEAPONS) ? nq_wpNames[secondaryWeapon] : "^1invalid"
	);
#endif
// JAQUBOSS - F@CK!!
/*	if( self->client->sess.playerWeapon ==  primaryWeapon ) 
	{	// drop our primary weapon
		G_DropWeapon( self, primaryWeapon );
	}

	if(self->client->sess.playerWeapon == secondaryWeapon)
	{	// drop our secondary weapon
		G_DropWeapon( self, secondaryWeapon );
	}*/

	if ( primaryWeapon > WP_NONE && primaryWeapon < WP_NUM_WEAPONS && g_weaponItems.integer & WIF_DROPMAIN)
		G_DropWeapon( self, primaryWeapon );

	if ( secondaryWeapon > WP_NONE && secondaryWeapon < WP_NUM_WEAPONS && g_weaponItems.integer & WIF_DROPPISTOLS )
		G_DropWeapon( self, secondaryWeapon );

}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
	vec3_t		dir;
	vec3_t		angles;

	if ( attacker && attacker != self )
	{
		VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
	}
	else if ( inflictor && inflictor != self ) 
	{
		VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
	} 
	else
	{
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

	angles[YAW] = vectoyaw ( dir );
	angles[PITCH] = 0; 
	angles[ROLL] = 0;
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer ) 
{
	gentity_t *other=&g_entities[killer];
	vec3_t dir;

	VectorClear( dir );
	if (other->inuse) {
		if (other->client) {
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		} else if (!VectorCompare(other->s.pos.trDelta, vec3_origin)) {
			VectorNormalize2( other->s.pos.trDelta, dir );
		}
	}

	G_AddEvent( self, EV_GIB_PLAYER, DirToByte(dir) );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	if(self->health <= GIB_HEALTH) {
		GibEntity(self, 0);
	}
}


// these are just for logging, the client prints its own messages
char *modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_MACHINEGUN",
	"MOD_BROWNING",
	"MOD_MG42",
	"MOD_GRENADE",
	"MOD_ROCKET",

	// (SA) modified wolf weap mods
	"MOD_KNIFE",
	"MOD_LUGER",
	"MOD_COLT",
	"MOD_MP40",
	"MOD_THOMPSON",
	"MOD_STEN",
	"MOD_GARAND",
	"MOD_SNOOPERSCOPE",
	"MOD_SILENCER",	//----(SA)	
	"MOD_FG42",
	"MOD_FG42SCOPE",
	"MOD_PANZERFAUST",
	"MOD_GRENADE_LAUNCHER",
	"MOD_FLAMETHROWER",
	"MOD_GRENADE_PINEAPPLE",
	"MOD_CROSS",
	// end

	"MOD_MAPMORTAR",
	"MOD_MAPMORTAR_SPLASH",

	"MOD_KICKED",
	"MOD_GRABBER",

	"MOD_DYNAMITE",
	"MOD_AIRSTRIKE", // JPW NERVE
	"MOD_SYRINGE",	// JPW NERVE
	"MOD_AMMO",	// JPW NERVE
	"MOD_ARTY",	// JPW NERVE

	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",

	"MOD_CARBINE",
	"MOD_KAR98",
	"MOD_GPG40",
	"MOD_M7",
	"MOD_LANDMINE",
	"MOD_SATCHEL",
	"MOD_TRIPMINE",
	"MOD_SMOKEBOMB",
	"MOD_MOBILE_MG42",
	"MOD_SILENCED_COLT",
	"MOD_GARAND_SCOPE",

	"MOD_CRUSH_CONSTRUCTION",
	"MOD_CRUSH_CONSTRUCTIONDEATH",
	"MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER",

	"MOD_K43",
	"MOD_K43_SCOPE",

	"MOD_MORTAR",

	"MOD_AKIMBO_COLT",
	"MOD_AKIMBO_LUGER",
	"MOD_AKIMBO_SILENCEDCOLT",
	"MOD_AKIMBO_SILENCEDLUGER",

	"MOD_SMOKEGRENADE",

	// RF
	"MOD_SWAP_PLACES",

	// OSP -- keep these 2 entries last
	"MOD_SWITCHTEAM",

	"MOD_GOOMBA",
	"MOD_POISON",
	"MOD_FEAR"

	// jet Pilot - new means of death (MOD)
	"MOD_CENSORED",
	"MOD_BLEEDING",
	"MOD_SHOTGUN",	
//	"MOD_SHOTGUN_DOUBLE",
//	"MOD_BAYONETTE",
	"MOD_BACKSTAB",
	"MOD_MOBILE_BROWNING",
	"MOD_BAR",
	"MOD_STG44",
	"MOD_BAZOOKA",
	"MOD_STEN_MKII",
	"MOD_MP34",
	"MOD_FIREBOLT",
	"MOD_VENOM",
	"MOD_JETGUN",
	"MOD_COLISION",
	"MOD_SHOVE",
};

/*
==================
player_die
==================
*/
void BotRecordTeamDeath( int client );


void G_JetExplode( gentity_t *ent ) 
{
	vec3_t		dir;
	gentity_t*	tent;

	G_RadiusDamage( ent->r.currentOrigin, ent, ent, 300, 300, ent, MOD_EXPLOSIVE );	
		

	tent = G_TempEntity(ent->r.currentOrigin, EV_MISSILE_MISS_LARGE);
	tent->s.weapon = WP_PANZERFAUST;
	tent->r.svFlags |= SVF_BROADCAST;

	dir[0] = dir[1] = 0;
	dir[2] = 1;

	tent->s.eventParm = DirToByte( dir );

	tent = G_TempEntity(ent->r.currentOrigin, EV_SHAKE);
	tent->s.onFireStart = 1200;
	tent->r.svFlags |= SVF_BROADCAST;
}

void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	int				contents		= 0;
	int				i;
	int				killer			= ENTITYNUM_WORLD;
	unsigned int	bounty;
//	int				killSpree		= 0;		
	char			*killerName 	= "<world>";
	qboolean		nogib			= qtrue;
//	gitem_t			*item			= NULL;
	gentity_t		*ent;
	qboolean		killedintank	= qfalse;	
	weapon_t		weap;
	playerState_t	*ps;

	// tjw: for g_shortcuts  
		if(attacker && attacker->client) {  
					self->client->pers.lastkiller_client = attacker->s.clientNum;  
					attacker->client->pers.lastkilled_client = self->s.clientNum;  
		}  

	// tjw: if player has /kill'ed in response to recent
	//      damage, then we need to credit the last attacker
	//      with proper XP.
	if(meansOfDeath == MOD_FEAR)
	{
		weap = BG_WeaponForMOD(self->client->lasthurt_mod);
	}
	else if ( meansOfDeath == MOD_FALLING && self->client->pmext.shoved )
	{
		weap = WP_NONE;
		attacker = &g_entities[self->client->pmext.pusher];
		meansOfDeath = MOD_SHOVE;
	}
	else
	{
		weap = BG_WeaponForMOD(meansOfDeath);
	}
	
	// xkan, 1/13/2003 - record the time we died.
	// tjw: moved this to death instead of limbo
	//      this is used for G_DropLimboHealth()
	if(!self->client->deathTime)
	{
		self->client->deathTime = level.time;
	}
	

	//unlagged - backward reconciliation #2
	// make sure the body shows up in the client's current position
	G_ReAdjustSingleClientPosition( self );
	//unlagged - backward reconciliation #2

	// jaquboss - poison lasts on wounded players
/*
	self->client->ps.eFlags &= ~EF_POISONED;
	self->client->pmext.poisonAmnt = 0;
	self->s.eFlags &= ~EF_POISONED;*/

	// jet Pilot - record bounty, as jP_UpdateSpree will clear it
	bounty = BG_GetStatBounty(&self->client->ps); //.persistant[PERS_SPREELEVEL];
	jP_UpdateSpree(self, attacker, qtrue, qfalse);

	if(attacker == self) {
		if(self->client) {
			self->client->pers.playerStats.suicides++;
			ps = &self->client->ps;

			if (bounty > 0)	{
				AP(va("chat \"^9The bounty on ^*%s ^9is gone... he ended it himself!\"", self->client->pers.netname));
			}

			// jet Pilot - win a darwin award for killing yourself somehow
			//self->client->ps.persistant[PERS_DARWINDEATHS]++;
			BG_SetStatDarwin(ps, BG_GetStatDarwin(ps) + 1);

			trap_PbStat ( self - g_entities, "suicide", 
				va("%d %d %d", self->client->sess.sessionTeam, self->client->sess.playerType, weap)) ;
		}
	}
	else if ( OnSameTeam(self, attacker )) {
		G_LogTeamKill(attacker,	weap);
	}
	else {
		G_LogDeath( self,	  weap );
		G_LogKill(	attacker, weap );		

		if( g_gamestate.integer == GS_PLAYING )	{
			if( attacker->client ) {
				attacker->client->combatState |= (1 << COMBATSTATE_KILLEDPLAYER);
			}
		}
	}

	if ( meansOfDeath == MOD_FALLING )
		self->client->sess.newton++; // jaquboss

	// RF, record this death in AAS system so that bots avoid areas which have high death rates
	if( !OnSameTeam( self, attacker ) ) 
	{
#ifndef NO_BOT_SUPPORT
		BotRecordTeamDeath( self->s.number );
#endif

		self->isProp = qfalse;	// were we teamkilled or not?
	} else {
		self->isProp = qtrue;
	}	

	// if we got killed by a landmine, update our map
	if( self->client && meansOfDeath == MOD_LANDMINE ) 
	{
		// if it's an enemy mine, update both teamlists
		/*int teamNum;
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList;
	
		teamNum = inflictor->s.teamNum % 4;

		teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[0] : &mapEntityData[1];
		if((mEnt = G_FindMapEntityData(teamList, inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( teamList, mEnt );
		}

		if( teamNum != self->client->sess.sessionTeam ) {
			teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0];
			if((mEnt = G_FindMapEntityData(teamList, inflictor-g_entities)) != NULL) {
				G_FreeMapEntityData( teamList, mEnt );
			}
		}*/
		mapEntityData_t	*mEnt;

		if((mEnt = G_FindMapEntityData(&mapEntityData[0], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[0], mEnt );
		}

		if((mEnt = G_FindMapEntityData(&mapEntityData[1], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[1], mEnt );
		}
	}

	{
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0];	// swapped, cause enemy team

		mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, self->s.number, -1 );
		
		while( mEnt ) {
			if( mEnt->type == ME_PLAYER_DISGUISED ) {
				mapEntityData_t* mEntFree = mEnt;

				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );

				G_FreeMapEntityData( teamList, mEntFree );
			} else {
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );
			}
		}
	}

	if( self->tankLink ) 
	{
		G_LeaveTank( self, qfalse );
		killedintank = qtrue;
	}

	if((self->client->ps.pm_type == PM_DEAD && !(self->client->ps.eFlags & EF_PLAYDEAD)) || g_gamestate.integer == GS_INTERMISSION )
	{
		return;
	}
	
	// the player really died this time
	self->client->ps.eFlags &= ~EF_PLAYDEAD;

	// OSP - death stats handled out-of-band of G_Damage for external calls
	if(meansOfDeath == MOD_FEAR) 
	{
		G_addStats(self, attacker, damage, self->client->lasthurt_mod);
		// tjw: this would normally be handled in G_Damage()
		G_AddKillSkillPoints(attacker, self->client->lasthurt_mod, 0, qfalse, bounty);
	}
	else 
	{
		G_addStats(self, attacker, damage, meansOfDeath);
		if ( meansOfDeath == MOD_SHOVE && attacker && attacker->client && OnSameTeam (self, attacker ) )
		G_AddKillSkillPoints(attacker, MOD_SHOVE, 0, qfalse, bounty);

	}
	// OSP

	self->client->ps.pm_type = PM_DEAD;

	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0);

	if(attacker) {
		killer = attacker->s.number;
		killerName = (attacker->client) ? attacker->client->pers.netname : "<non-client>";
	}

	if(attacker == 0 || killer < 0 || killer >= MAX_CLIENTS) {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if(g_gamestate.integer == GS_PLAYING) {
		char *obit;

		if(meansOfDeath < 0 || meansOfDeath >= sizeof(modNames) / sizeof(modNames[0])) {
			obit = "<bad obituary>";
		} else {
			obit = modNames[meansOfDeath];
		}

		G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n", killer, self->s.number, meansOfDeath, killerName, self->client->pers.netname, obit );
	}

	// RF, record bot kills
#ifndef NO_BOT_SUPPORT
	if (attacker->r.svFlags & SVF_BOT) {
		BotRecordKill( attacker->s.number, self->s.number );
	}
#endif

#ifdef OMNIBOTS
	//////////////////////////////////////////////////////////////////////////
	// If the victim is a bot, send a death event.
	if(self->r.svFlags & SVF_BOT)
	{
		BotUserData bud;
		const int numMods = sizeof(modNames) / sizeof(modNames[0]);
		bud.DataType = dtString;
		bud.udata.m_String = "";
		if(meansOfDeath >= 0 && meansOfDeath < numMods) 
		{
			bud.udata.m_String = modNames[meansOfDeath];
		}
		Bot_Interface_SendEvent(MESSAGE_DEATH, self-g_entities, attacker-g_entities, 0, &bud);
	}

	// If the attacker is a bot, send a kill event.
	if(attacker->r.svFlags & SVF_BOT)
	{
		BotUserData bud;
		const int numMods = sizeof(modNames) / sizeof(modNames[0]);
		bud.DataType = dtString;
		bud.udata.m_String = "";
		if(meansOfDeath >= 0 && meansOfDeath < numMods) 
		{
			bud.udata.m_String = modNames[meansOfDeath];
		}
		Bot_Interface_SendEvent(MESSAGE_KILLEDSOMEONE, attacker-g_entities, self-g_entities, 0, &bud);
	}
	//////////////////////////////////////////////////////////////////////////
#endif

	// broadcast the death event to everyone
	{
		ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
		ent->s.eventParm = meansOfDeath;
		ent->s.otherEntityNum = self->s.number;
		ent->s.otherEntityNum2 = killer;
		ent->r.svFlags = SVF_BROADCAST;	// send to everyone
		ent->s.weapon = weap;
		// set death flags ...
		if ( self->client->executed )
		ent->s.dmgFlags |= OBIT_EXECUTED;
	}

	self->enemy = attacker;

	//self->client->ps.persistant[PERS_KILLED]++;
	// Jaybird - this is BAD.  G_addStats already does this!
	//self->client->sess.deaths++;

	// CHRUKER: b010 - Make sure covert ops lose their disguises
	self->client->ps.powerups[PW_OPS_DISGUISED] = 0;	 

	// JPW NERVE -- if player is holding ticking grenade, drop it
	if ((self->client->ps.grenadeTimeLeft)	&& 
		(self->s.weapon != WP_DYNAMITE)		&& 
		(self->s.weapon != WP_LANDMINE)		&& 
		(self->s.weapon != WP_SATCHEL)		&& 
		(self->s.weapon != WP_TRIPMINE)) 
	{
		vec3_t launchvel, launchspot;

		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = random();
		VectorScale( launchvel, 160, launchvel );
		VectorCopy(self->r.currentOrigin, launchspot);
		launchspot[2] += 40;
		
		{
			// Gordon: fixes premature grenade explosion, ta bani ;)
			gentity_t *m = fire_grenade(self, launchspot, launchvel, self->s.weapon);
			m->damage = 0;
		}

		}
		// forty -      After firing a rifle grenade and not having enough time to   
		//                      finish the automatic switching to the rifle before dieing   
		//                      end up with a useless rifle grenade or losing their rifle.  
		if(self->s.weapon == WP_M7 && !self->client->ps.ammoclip[BG_FindClipForWeapon(WP_M7)]) 
			{  
			self->s.weapon = WP_CARBINE;  
			self->client->ps.weapon = WP_CARBINE;  
			self->client->ps.weaponstate = WEAPON_READY;  
		}  
		
		if(self->s.weapon == WP_GPG40 && !self->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)]) 
			{  
			self->s.weapon = WP_KAR98;  
			self->client->ps.weapon = WP_KAR98;  
			self->client->ps.weaponstate = WEAPON_READY; 
	}

	if (attacker && attacker->client) {
		if ( attacker == self || OnSameTeam (self, attacker ) ) {

			// DHM - Nerve :: Complaint lodging
			if(attacker != self && 
				level.warmupTime <= 0 && 
				g_gamestate.integer == GS_PLAYING &&
				!G_shrubbot_permission(attacker, SBF_IMMUNITY)) {
				if( attacker->client->pers.localClient ) {
					trap_SendServerCommand( self-g_entities, "complaint -4" );
				} else {
					if( meansOfDeath != MOD_CRUSH_CONSTRUCTION && meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH && meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER ) {
						if( g_complaintlimit.integer ) {

							if( !(meansOfDeath == MOD_LANDMINE && g_disableComplaints.integer & TKFL_MINES ) &&
								!((meansOfDeath == MOD_ARTY || meansOfDeath == MOD_AIRSTRIKE) && g_disableComplaints.integer & TKFL_AIRSTRIKE ) &&
								!(meansOfDeath == MOD_MORTAR && g_disableComplaints.integer & TKFL_MORTAR ) ) {
								trap_SendServerCommand( self-g_entities, va( "complaint %i", attacker->s.number ) );
								if ( meansOfDeath != MOD_DYNAMITE || !(inflictor->etpro_misc_1 & 1) ){ // jaquboss do not allow complain when tked by dynamite on objective
									self->client->pers.complaintClient = attacker->s.clientNum;
									self->client->pers.complaintEndTime = level.time + 20500;
								}
							}
						}
					}
				}
			}

			// high penalty to offset medic heal
/*			AddScore( attacker, WOLF_FRIENDLY_PENALTY ); */

			if( g_gametype.integer == GT_WOLF_LMS ) {
				AddKillScore( attacker, WOLF_FRIENDLY_PENALTY );
			}
		} else {

			//G_AddExperience( attacker, 1 );

			// JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
			AddScore(attacker, WOLF_FRAG_BONUS);

			if( g_gametype.integer == GT_WOLF_LMS ) {
				if( level.firstbloodTeam == -1 )
					level.firstbloodTeam = attacker->client->sess.sessionTeam;

				AddKillScore( attacker, WOLF_FRAG_BONUS );
			}
			
			if ( attacker && attacker->client ){
				attacker->client->lastKillTime = level.time;
				attacker->client->multikill++;
				attacker->client->ps.persistant[PERS_KILLS]++;
			}
			
			// jet Pilot - add up and announce killing sprees
			if (jp_killSpreeLevels.integer)
			{				
				jP_UpdateSpree(attacker, NULL, qfalse, qfalse);
			}
		}
	} 
	else 
	{
		AddScore( self, -1 );

		if( g_gametype.integer == GT_WOLF_LMS )
			AddKillScore( self, -1 );
	}

	// Add team bonuses
	Team_FragBonuses(self, inflictor, attacker);

	G_DropItems(self);

	// send a fancy "MEDIC!" scream.  Sissies, ain' they?
	if (self->client != NULL) 
	{
		if( self->health > GIB_HEALTH && !self->client->executed && meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_SWITCHTEAM && self->waterlevel < 3 && ( !g_fastBackStab.integer || meansOfDeath != MOD_BACKSTAB)) 
		{
			if (!attacker->client || !BG_Weapons[weap].limboKill){
#ifdef __JETPILOT__
				if ( !(jp_insanity.integer & JP_INSANITY_FLYING ) ){ // jet's will explode instead 
#endif
					G_AddEvent( self, EV_MEDIC_CALL, 0 );
#ifdef OMNIBOTS
					// ATM: only register the goal if the target isn't in water.
					if(self->waterlevel <= 1)
					{
						if (self->client->sess.sessionTeam == TEAM_AXIS)
							Bot_Util_AddGoal((GameEntity)self, ET_GOAL_FALLENTEAMMATE, (1 << ET_TEAM_AXIS), NULL, NULL);
						else if (self->client->sess.sessionTeam == TEAM_ALLIES)
							Bot_Util_AddGoal((GameEntity)self, ET_GOAL_FALLENTEAMMATE, (1 << ET_TEAM_ALLIES), NULL, NULL);
					}			
#endif
#ifdef __JETPILOT__
				}
#endif
			}
		}
	}

	Cmd_Score_f( self );		// show scores

	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for(i=0; i<level.numConnectedClients; i++) {
		gclient_t *client = &level.clients[level.sortedClients[i]];

		if(client->pers.connected != CON_CONNECTED) continue;
		if(client->sess.sessionTeam != TEAM_SPECTATOR) continue;

		if(client->sess.spectatorClient == self->s.number) {
			Cmd_Score_f(g_entities + level.sortedClients[i]);
		}
	}

	self->takedamage = qtrue;	// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	//self->s.angles[2] = 0;
	self->s.powerups = 0;
	self->s.loopSound = 0;
	
	self->client->limboDropWeapon = self->s.weapon; // store this so it can be dropped in limbo

	LookAtKiller( self, inflictor, attacker );
	self->client->ps.viewangles[0] = 0;
	self->client->ps.viewangles[2] = 0;
	//VectorCopy( self->s.angles, self->client->ps.viewangles );

//	trap_UnlinkEntity( self );

	// ydnar: so bodies don't clip into world
	self->r.maxs[2] = self->client->ps.crouchMaxZ;	
	self->client->ps.maxs[2] = self->client->ps.crouchMaxZ;	


	trap_LinkEntity( self );

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.timeCurrent + 800;

	// remove powerups
	memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

	// never gib in a nodrop
	// FIXME: contents is always 0 here
	if ( self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) ) {
		GibEntity( self, killer );
		nogib = qfalse;
	}

	if(nogib){
		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH + 1;
		}
#if 0
		// Arnout: re-enable this for flailing
		if( self->client->ps.groundEntityNum == ENTITYNUM_NONE ) {
			self->client->ps.pm_flags |= PMF_FLAILING;
			self->client->ps.pm_time = 750;
			BG_AnimScriptAnimation( &self->client->ps, ANIM_MT_FLAILING, qtrue );

			// Face explosion directory
			{
				vec3_t angles;

				vectoangles( self->client->ps.velocity, angles );
				self->client->ps.viewangles[YAW] = angles[YAW];
				SetClientViewAngle( self, self->client->ps.viewangles );
			}
		} else

			// DHM - Play death animation, and set pm_time to delay 'fallen' animation
			if( G_IsSinglePlayerGame() && self->client->sess.sessionTeam == TEAM_ALLIES ) {
				// play "falldown" animation since allies bots won't ever die completely
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_FALLDOWN, qfalse, qtrue );
				G_StartPlayerAppropriateSound(self, "death");
			} else {
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );
				// death animation script already contains sound
			}

#endif

			// set enemy weapon
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, weap, qtrue );
		
			// set enemy location
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, 0, qfalse );
			if (infront( self, inflictor )) {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, POSITION_INFRONT, qtrue );
			} else {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, POSITION_BEHIND, qtrue );
			}

			if ( self->client->executed || ( meansOfDeath == MOD_SUICIDE && g_gamestate.integer == GS_PLAYING ) || ( g_fastBackStab.integer && meansOfDeath == MOD_BACKSTAB)){
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH_FROM_BEHIND, qfalse, qtrue );
			} else {
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );
			}

			// record the death animation to be used later on by the corpse
			self->client->torsoDeathAnim = self->client->ps.torsoAnim;
			self->client->legsDeathAnim = self->client->ps.legsAnim;

		//	G_AddEvent( self, EV_DEATH/*1 + 1*/,/*killer*/ weap );
			{
				gentity_t	*tent;
				int event = EV_DEATH1+(rand()%4);

			//	G_Printf( "%i\n", event );

				if ( event > EV_DEATH4 )
					event = EV_DEATH4;

				if ( event < EV_DEATH1 )
					event = EV_DEATH1;

				tent = G_TempEntity( self->r.currentOrigin, event );
				tent->s.otherEntityNum2 = self->s.number;
				tent->r.svFlags = SVF_BROADCAST;	// send to everyone
				tent->s.weapon = weap;
			}

		// the body can still be gibbed
		self->die = body_die;
	}

	if( meansOfDeath == MOD_MACHINEGUN ) {
		switch( self->client->sess.sessionTeam ) {
			case TEAM_AXIS:
				level.axisMG42Counter = level.time;
				break;
			case TEAM_ALLIES:
				level.alliesMG42Counter = level.time;
				break;
			default:
				break;
		}
	}

	G_FadeItems( self, MOD_SATCHEL );

	CalculateRanks();
#ifdef __JETPILOT__
	if ( jp_insanity.integer & JP_INSANITY_FLYING ) // jet's will explode instead 
	{
		limbo( self, qfalse ); // but no corpse
		G_JetExplode( self ); // and make a BOOM!
	}
	else
#endif
	if( killedintank /*Gordon: automatically go to limbo from tank*/ ) {
		limbo( self, qfalse ); // but no corpse
	}
	else if((meansOfDeath == MOD_SUICIDE || meansOfDeath == MOD_FEAR || ( g_fastBackStab.integer && meansOfDeath == MOD_BACKSTAB)) && g_gamestate.integer == GS_PLAYING) 
	{
		self->client->deathAnim = qtrue;	// add animation time
		limbo( self, qtrue );		
	}
	else if ( self->client->executed )
	{
		self->client->deathAnim = qtrue; // add animation time
		limbo( self, qtrue );
	}
	else if ( BG_Weapons[weap].limboKill )
	{
		self->client->deathAnim = qtrue; // add animation time
		limbo( self, qtrue );
	}
	else if( g_gametype.integer == GT_WOLF_LMS ) 
	{
		if( !G_CountTeamMedics( self->client->sess.sessionTeam, qtrue ) ) 
		{
			limbo( self, qtrue );
		}
	}
}

qboolean IsHeadShotWeapon (int mod) 
{

	if ( BG_Weapons[BG_WeaponForMOD(mod)].headShot == WEAPON_YES )
		return qtrue;

	if ( BG_Weapons[BG_WeaponForMOD(mod)].headShot == WEAPON_NO )
		return qfalse;

	// players are allowed headshots from these weapons
	if (	mod == MOD_LUGER			   	|| mod == MOD_AKIMBO_LUGER			||
			mod == MOD_SILENCER			   	|| mod == MOD_AKIMBO_SILENCEDLUGER	||
			mod == MOD_COLT					|| mod == MOD_AKIMBO_COLT		   	|| 
			mod == MOD_SILENCED_COLT		|| mod == MOD_AKIMBO_SILENCEDCOLT 	|| 
			mod == MOD_MP40				   	|| mod == MOD_THOMPSON				||
			mod == MOD_GARAND				|| mod == MOD_GARAND_SCOPE			|| mod == MOD_CARBINE				||
			mod == MOD_K43					|| mod == MOD_K43_SCOPE		   		|| mod == MOD_KAR98               	|| 
			mod == MOD_FG42				   	|| mod == MOD_FG42SCOPE				||
			mod == MOD_STEN				   	|| mod == MOD_STEN_MKII				||		
			mod == MOD_MP34					||
			mod == MOD_KNIFE				|| 
			mod == MOD_STG44				||
	//		mod == MOD_VENOM				||
			mod == MOD_BAR		//			||
	//		mod == MOD_BAYONETTE
		)
		return qtrue;

	return qfalse;
}

gentity_t* G_BuildHead(gentity_t *ent) {
	gentity_t* head;
	orientation_t or;			// DHM - Nerve

	head = G_Spawn ();
	head->classname = "head";
	
	// zinx - moved up here so realistic hitboxes can override
	VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6

	 if (g_realHead.integer & REALHEAD_HEAD) {
		// zinx - realistic hitboxes
		grefEntity_t refent;

		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		mdx_head_position( ent, &refent, or.origin );
		G_SetOrigin( head, or.origin );
		VectorSet (head->r.mins , -6, -6, -6);
		VectorSet (head->r.maxs , 6, 6, 6);
	 } else	if (trap_GetTag( ent->s.number, 0, "tag_head", &or )) {
		G_SetOrigin( head, or.origin );
	 } else {
		float height, dest;
		vec3_t v, angles, forward, up, right;
		VectorClear(v);
		G_SetOrigin (head, ent->r.currentOrigin); 

		if((ent->client->ps.eFlags & EF_PRONE)) {
			height = ent->client->ps.viewheight - 60;
		}
		else if((ent->client->ps.eFlags & EF_DEAD) ||
			(ent->client->ps.eFlags & EF_PLAYDEAD)) {

			height = ent->client->ps.viewheight - 64;
		} else if( ent->client->ps.pm_flags & PMF_DUCKED ) {	// closer fake offset for 'head' box when crouching
			height = ent->client->ps.crouchViewHeight - 12;
		} else {
			height = ent->client->ps.viewheight;
		}

		
		VectorCopy( ent->client->ps.viewangles, angles );

		// NERVE - SMF - this matches more closely with WolfMP models
		if ( angles[PITCH] > 180 ) {
			dest = (-360 + angles[PITCH]) * 0.75;
		} else {
			dest = angles[PITCH] * 0.75;
		}
		angles[PITCH] = dest;

		// tjw: the angles need to be clamped for prone 
		//      or the head entity will be underground or
		//      far too tall
		if((ent->client->ps.eFlags & EF_PRONE)) {
			if((ent->client->ps.eFlags & EF_MOTION)) 
				angles[PITCH] = -15;
			else 
				angles[PITCH] = -10;
		}

		AngleVectors( angles, forward, right, up );
		if( ent->client->ps.eFlags & EF_PRONE ) {
			if((ent->client->ps.eFlags & EF_MOTION)) {
				VectorScale(forward, 24, v);
			}
			else {
				VectorScale(forward, 28, v);
				VectorMA(v, 7, right, v);
			}
		}
		else if((ent->client->ps.eFlags & EF_DEAD) ||
			(ent->client->ps.eFlags & EF_PLAYDEAD)) {
			// tjw: -28 is right for the head but it makes
			//      a small gap between the head and body
			//      that cannot be hit.  I think this is worse.
			//VectorScale(forward, -28, v);
			VectorScale(forward, -26, v);
			VectorMA(v, 5, right, v);
		}
		else {
			// tjw: when moving, the head is drawn
			//      down and forward
			if((ent->client->ps.eFlags & EF_MOTION)) { 

				if((ent->client->ps.pm_flags & PMF_DUCKED)) {
					height += 2;
					VectorMA(v, 18, forward, v);
				}
				else {
					height -= 10;
					VectorMA(v, 10, forward, v);
				}
		}
			else {
				VectorScale( forward, 5, v);
			}
			VectorMA(v, 5, right, v);
		}
		VectorMA(v, 18, up, v);

		VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
		head->r.currentOrigin[2] += height / 2;
	}

	VectorCopy (head->r.currentOrigin, head->s.origin);

	// tjw: this seems equiv to setting all to 0
	//VectorCopy (ent->r.currentAngles, head->s.angles); 
	//VectorCopy (head->s.angles, head->s.apos.trBase);
	//VectorCopy (head->s.angles, head->s.apos.trDelta);

	// forty - realistic hitboxes
	//VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	//VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6
	head->clipmask = CONTENTS_SOLID;
	head->r.contents = CONTENTS_SOLID;
	head->parent = ent;
	head->s.eType = ET_TEMPHEAD;

	trap_LinkEntity (head);
	
	return head;
}

gentity_t* G_BuildLeg(gentity_t *ent) {
	gentity_t* leg;
	vec3_t flatforward, org;
	//orientation_t or;			// DHM - Nerve

	if( !(ent->client->ps.eFlags & EF_PRONE) &&
		!(ent->client->ps.eFlags & EF_DEAD) &&
		!(ent->client->ps.eFlags & EF_PLAYDEAD))
		return NULL;

	leg = G_Spawn ();
	leg->classname = "leg";

	if (g_realHead.integer & REALHEAD_HEAD) {

		// zinx - realistic hitboxes
		grefEntity_t refent;

		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		mdx_legs_position( ent, &refent, org );
		org[2] += ent->client->pmext.proneLegsOffset;
		org[2] -= (playerlegsProneMins[2] + playerlegsProneMaxs[2]) * 0.5;

	} 
	else
	{
		AngleVectors( ent->client->ps.viewangles, flatforward, NULL, NULL );
		flatforward[2] = 0;
		VectorNormalizeFast( flatforward );
		if(ent->client->ps.eFlags & EF_PRONE) 
		{
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * -32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * -32;
		}
		else
		{
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * 32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * 32;
		}
		org[2] = ent->r.currentOrigin[2] + ent->client->pmext.proneLegsOffset;
	}

	G_SetOrigin( leg, org );

	VectorCopy( leg->r.currentOrigin, leg->s.origin );

	// tjw: seems to be just setting all to 0?
	//VectorCopy( ent->r.currentAngles, leg->s.angles ); 
	//VectorCopy( leg->s.angles, leg->s.apos.trBase );
	//VectorCopy( leg->s.angles, leg->s.apos.trDelta );

	VectorCopy( playerlegsProneMins, leg->r.mins );
	VectorCopy( playerlegsProneMaxs, leg->r.maxs );
	leg->clipmask = CONTENTS_SOLID;
	leg->r.contents = CONTENTS_SOLID;
	leg->parent = ent;
	leg->s.eType = ET_TEMPLEGS;

	trap_LinkEntity( leg );
	
	return leg;
}

qboolean IsHeadShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod ) {
	gentity_t	*head;
	trace_t		tr;
	vec3_t		start, end;
	gentity_t	*traceEnt;

	// not a player or critter so bail
	if( !(targ->client) )
		return qfalse;

	//if( targ->health <= 0 )
	//	return qfalse;

	if (!IsHeadShotWeapon (mod) ) {
		return qfalse;
	}

	head = G_BuildHead( targ );
	
	// trace another shot see if we hit the head
	VectorCopy( point, start );
	VectorMA( start, 64, dir, end );
	trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
		
	traceEnt = &g_entities[ tr.entityNum ];

	if( g_debugBullets.integer >= 3 ) {	// show hit player head bb
		/*gentity_t *tent;
		vec3_t b1, b2;
		VectorCopy(head->r.currentOrigin, b1);
		VectorCopy(head->r.currentOrigin, b2);
		VectorAdd(b1, head->r.mins, b1);
		VectorAdd(b2, head->r.maxs, b2);
		tent = G_TempEntity( b1, EV_RAILTRAIL );
		VectorCopy(b2, tent->s.origin2);
		tent->s.dmgFlags = 1;*/

		G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(1.f,0.f,0.f));

		// show headshot trace
		// end the headshot trace at the head box if it hits
		if( tr.fraction != 1 ) {
			VectorMA(start, (tr.fraction * 64), dir, end);
		}
		/*tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy(end, tent->s.origin2);
		tent->s.dmgFlags = 0;*/
		G_RailTrail( start, end, tv(1.f,0.f,0.f));
	}

	if (g_antilag.integer) {
		G_ReAdjustSingleClientPosition(targ);
	}

	G_FreeEntity( head );

	if( traceEnt == head ) {
		level.totalHeadshots++;		// NERVE - SMF
		return qtrue;
	} else
		level.missedHeadshots++;	// NERVE - SMF

	return qfalse;
}

qboolean IsLegShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod ) {
	float height;
	float theight;
	gentity_t *leg;

	if (!(targ->client))
		return qfalse;

	//if (targ->health <= 0)
	//	return qfalse;

	if(!point) {
		return qfalse;
	}

	if(!IsHeadShotWeapon(mod)) {
		return qfalse;
	}

	leg = G_BuildLeg( targ );

	if( leg ) {
		gentity_t	*traceEnt;
		vec3_t		start, end;
		trace_t		tr;

		// trace another shot see if we hit the legs
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
			
		traceEnt = &g_entities[ tr.entityNum ];

		if( g_debugBullets.integer >= 3 ) {	// show hit player head bb
			/*gentity_t *tent;
			vec3_t b1, b2;
			VectorCopy( leg->r.currentOrigin, b1 );
			VectorCopy( leg->r.currentOrigin, b2 );
			VectorAdd( b1, leg->r.mins, b1 );
			VectorAdd( b2, leg->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;*/

			G_RailBox( leg->r.currentOrigin, leg->r.mins, leg->r.maxs, tv(1.f,0.f,0.f));

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if( tr.fraction != 1 ) {
				VectorMA( start, (tr.fraction * 64), dir, end );
			}
			/*tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;*/

			G_RailTrail( start, end, tv(1.f,0.f,0.f));
		}
		if (g_antilag.integer) {
			G_ReAdjustSingleClientPosition(targ);
		}

		G_FreeEntity( leg );

		if( traceEnt == leg ) {
			return qtrue;
		}
	} else {
		height = point[2] - targ->r.absmin[2];
		theight = targ->r.absmax[2] - targ->r.absmin[2];

		if(height < (theight * 0.4f)) {
			return qtrue;
		}
	}

	return qfalse;
}

qboolean IsArmShot( gentity_t *targ, gentity_t* ent, vec3_t point, int mod ) {
	vec3_t path, view;
	vec_t dot;

	if (!(targ->client))
		return qfalse;

	if (targ->health <= 0)
		return qfalse;

	if(!IsHeadShotWeapon (mod)) {
		return qfalse;
	}

	VectorSubtract(targ->client->ps.origin, point, path);
	path[2] = 0;

	AngleVectors(targ->client->ps.viewangles, view, NULL, NULL);
	view[2] = 0;

	VectorNormalize(path);

	dot = DotProduct(path, view);

	if(dot > 0.4f || dot < -0.75f ) {
		return qfalse;
	}

	return qtrue;
}


qboolean G_WeaponCanGib( meansOfDeath_t mod )
{
	weapon_t	weap=BG_WeaponForMOD(mod);

	if ( BG_Weapons[weap].gib == WEAPON_YES )
		return qtrue;

	if ( BG_Weapons[weap].gib == WEAPON_NO )
		return qfalse;

	if ( G_WeaponIsExplosive( mod ) )
		return qtrue;

	switch (mod)
	{
		case MOD_VENOM:
			return qtrue;
		default:
			break;
	}

	return qfalse;
}

qboolean G_HasFlakJacket(gentity_t *ent);

/*
============
G_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

void G_Damage(gentity_t *targ,	gentity_t	*inflictor, gentity_t	*attacker,	vec3_t	dir,
			  vec3_t	point,	int			damage,		int			dflags,		int		mod ) 
{
	gclient_t	*client;
	int			take;
	int			save;
	int			knockback;
	int			limbo_health;
	qboolean	headShot		= qfalse;
	qboolean	wasAlive;
	hitRegion_t	hr				= HR_NUM_HITREGIONS;

	// early out if target cannot take damage
	if (!targ->takedamage)
		return;

	// the intermission has allready been qualified for, so don't allow any extra scoring
	// CHRUKER: b024 - Don't do damage if at warmup and warmupdamage is set to 'None' and the target is a client.
	if(level.intermissionQueued || (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 0 &&	targ->client)) 
		return;

	// if there is no damage inflictor, then the world did it
	inflictor = inflictor ? inflictor : &g_entities[ENTITYNUM_WORLD];
	attacker  = attacker  ? attacker  : &g_entities[ENTITYNUM_WORLD];

	if ( mod == MOD_FIREBOLT ) // burn while hit with that
		G_BurnMeGood(attacker, targ, NULL);


	// Arnout: invisible entities can't be damaged
	if( targ->entstate == STATE_INVISIBLE || targ->entstate == STATE_UNDERCONSTRUCTION ) 
		return;

	// xkan, 12/23/2002 - was the bot alive before applying any damage?
	wasAlive = (targ->health > 0);

	limbo_health = FORCE_LIMBO_HEALTH;
	if(g_forceLimboHealth.integer > 0) {
		limbo_health = -g_forceLimboHealth.integer;
	} else if ( g_forceLimboHealth.integer < 0 ){
		limbo_health = g_forceLimboHealth.integer;
	}

	// Arnout: combatstate
	if( targ->client && attacker && attacker->client && attacker != targ ) 
	{
		if( g_gamestate.integer == GS_PLAYING ) 
		{
			if( !OnSameTeam( attacker, targ ) ) 
			{
				targ->client->combatState		|= (1 << COMBATSTATE_DAMAGERECEIVED);
				attacker->client->combatState	|= (1 << COMBATSTATE_DAMAGEDEALT);
			}
		}
	}

	// JPW NERVE - if submerged and on fire, dont do any more damage
	if ((targ->waterlevel >= 3) && (mod == MOD_FLAMETHROWER))
		return;

	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER && !(targ->isProp) && !targ->scriptName) 
	{
		if ( targ->use && targ->moverState == MOVER_POS1 ) 
			G_UseEntity( targ, inflictor, attacker );

		return;
	}

	// TAT 11/22/2002
	// In the old code, this check wasn't done for props, so I put that check back in to make props_statue properly wor 4 means destructible
	if ( targ->s.eType == ET_MOVER && (targ->spawnflags & 4) && !targ->isProp ) 
	{	
		// only explosive weapons can hurt this
		if( !G_WeaponIsExplosive( mod ) ) 
			return;

		// check for team
		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) )
		{
			// jet Pilot - MODIFIED - allow team damage to tanks if (g_friendlyFire & 8)
			if ( !(g_friendlyFire.integer & FRIENDLY_FIRE_MOVERS) )
				return;
		}
	} 
	else if ( targ->s.eType == ET_EXPLOSIVE ) 
	{
		// 32 Explosive
		// 64 Dynamite only
		// 256 Airstrike/artillery only
		// 512 Satchel only
		
		if( targ->parent && G_GetWeaponClassForMOD( mod ) == 2 ) 
			return;

		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) )
			return;

		if( G_GetWeaponClassForMOD( mod ) < targ->constructibleStats.weaponclass )
			return;
	}
	else if ( targ->s.eType == ET_MISSILE && targ->methodOfDeath == MOD_LANDMINE ) 
	{
		if( targ->s.modelindex2 ) 
		{
			if( G_WeaponIsExplosive( mod ) ) 
			{
				mapEntityData_t	*mEnt;

				if((mEnt = G_FindMapEntityData(&mapEntityData[0], targ-g_entities)) != NULL) 
					G_FreeMapEntityData( &mapEntityData[0], mEnt );

				if((mEnt = G_FindMapEntityData(&mapEntityData[1], targ-g_entities)) != NULL) 
					G_FreeMapEntityData( &mapEntityData[1], mEnt );

				if( attacker && attacker->client ) 
					AddScore( attacker, 1 );

				G_ExplodeMissile(targ);
			}
		}
		return;
	} 
	else if ( targ->s.eType == ET_CONSTRUCTIBLE ) 
	{
		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) 
			return;

		if( G_GetWeaponClassForMOD( mod ) < targ->constructibleStats.weaponclass ) 
			return;

		//bani - fix #238
		if ( mod == MOD_DYNAMITE ) 
		{
			if( !( inflictor->etpro_misc_1 & 1 ) )
				return;
		}
	}

	client = targ->client;

	if ( client ) 
	{
		if ( client->noclip || client->ps.powerups[PW_INVULNERABLE] ) 
			return;
	}

	// check for godmode
	if ( targ->flags & FL_GODMODE ) 
		return;

	if ( !dir ) 
		dflags |= DAMAGE_NO_KNOCKBACK;
	else 
		VectorNormalize(dir);

	knockback = damage;
	if ( knockback > 200 ) 
		knockback = 200;

	if ( targ->flags & FL_NO_KNOCKBACK ) 
		knockback = 0;

	if ( dflags & DAMAGE_NO_KNOCKBACK ) 
		knockback = 0;
	else if( dflags & DAMAGE_HALF_KNOCKBACK ) 
		knockback *= 0.5f;
	
	// ydnar: set weapons means less knockback
	if( client && ( client->ps.weapon == WP_MORTAR2_SET || client->ps.weapon == WP_MORTAR_SET || client->ps.weapon == WP_MOBILE_MG42_SET || 
		client->ps.weapon == WP_MOBILE_BROWNING_SET   || client->ps.weapon == WP_BAR_SET ) )
		knockback *= 0.5;

	// kicking hurts only a bit but knocks much more
	if ( knockback && mod == MOD_KICKED )
		knockback *= 5.0;

	// no knockback from friendly fire, unless specified
	if( targ->client && OnSameTeam(targ, attacker) )
	{
		if (!(g_friendlyFire.integer & FRIENDLY_FIRE_BOOSTING) && !(g_friendlyFire.integer & FRIENDLY_FIRE_DISABLED) && g_friendlyFire.integer > 0)
			knockback = 0;
	}

	// prevent 'tractor beam' knockback
	if (knockback < 0)
		knockback = 0;

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) 
	{
		vec3_t	kvel;
		float	mass	= 200.0f;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		// jaquboss - are we pushed? Do not count when already flying..
		if ( attacker && attacker->client && targ->client->ps.groundEntityNum != ENTITYNUM_NONE ){
			targ->client->pmext.shoved = qtrue;
			targ->client->pmext.pusher = attacker - g_entities;
		}
	
		// jP - TODO blast velocity cvar here?, for nade jumping, etc.
		if (targ == attacker && !(mod != MOD_ROCKET   && mod != MOD_GRENADE && mod != MOD_GRENADE_LAUNCHER &&
								  mod != MOD_DYNAMITE && mod != MOD_GPG40   && mod != MOD_M7 			   &&
								  mod != MOD_LANDMINE ))
			targ->client->ps.velocity[2] *= 0.25;

		// set the timer so that the other client can't cancel out the movement immediately
		if ( !targ->client->ps.pm_time ) 
		{
			int		t;

			t = knockback * 2;

			// cap it
			if ( t < 50 )  t = 50;			
			else if ( t > 200 ) t = 200;

			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}


	}

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) 
	{
		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target if the attacker was on the same team
		if ( targ != attacker && OnSameTeam (targ, attacker)  ) 
		{
			if ( (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 1)) 
			{
				return;
			}

			else if (!g_friendlyFire.integer /*g_friendlyFire.integer & FRIENDLY_FIRE_DISABLED*/) 
			{
				// record "fake" pain - although the bot is not really hurt, his feeling has been hurt :-)
				// well at least he wants to shout "watch your fire".
#ifndef NO_BOT_SUPPORT
				if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) {
					BotRecordPain( targ->s.number, attacker->s.number, mod );
				}
#endif
				return;
			}
		}
	}

	if ( damage < 0 ) 
		damage = 0;

	take = damage;
	save = 0;

	// jet Pilot - mirror damage from team g_friendlyFire 
	if ( (g_friendlyFire.integer & FRIENDLY_FIRE_MIRROR) && (targ != attacker)&& OnSameTeam(targ, attacker)  )
		G_Damage(attacker, attacker, attacker, dir, point, take, dflags, mod);

	// jet Pilot - half damage from team if specified. Inflictor player still takes full damage
	if ( (g_friendlyFire.integer & FRIENDLY_FIRE_HALF) && (targ != attacker)&& OnSameTeam(targ, attacker)  )
		take *= .5f;

	// adrenaline junkie!
	if( targ->client && targ->client->ps.powerups[PW_ADRENALINE] ) 
		take *= .5f;

	// Prevent 50% of explosive damage if wearing a Flak Jacket
	if (targ->client && G_WeaponIsExplosive(mod) && G_HasFlakJacket(targ))
		take *= .5;
	
	if (targ->client && IsHeadShot(attacker, targ, dir, point, mod))
		headShot = qtrue;
		
	// jaquboss 
	if ( g_headshot.integer > 0 && // if headshot mode is on...
		attacker && attacker->client && // .. and attacked by player ..
		!headShot && targ->client // .. and we are a client who didn't recieve headshot..
		)
		return; // .. then just quit ..

	if ( headShot ) 
	{	// jP optimized	
		vec_t	dist;
		vec3_t	shotvec;
	

		if ((take <<= 1) < 50) take = 50;

		VectorSubtract( point, muzzleTrace, shotvec );
		dist = VectorLength( shotvec );

		// jet Pilot - Falloff Damage - optionally disabled via b_falloff
		if( jp_falloff.integer && dflags & DAMAGE_DISTANCEFALLOFF ) 
		{
			float	scale;

			scale = 1.0f - (((dist - 1500.f) / (2500.f - 1500.f)) * 0.8f);
			// And, finally, cap it.
			if (scale > 1.0f)		scale = 1.0f;
			else if (scale < 0.2f)	scale = 0.2f;
			take *= scale;
		}

	
		// Lose our hat (and absorb some damage) if we're wearing one
		if( !(targ->client->ps.eFlags & EF_HEADSHOT) && ( targ->client->sess.skill[SK_BATTLE_SENSE] < 6 || (targ->client->sess.skill[SK_HEAVY_WEAPONS] >= 9 && targ->client->sess.playerType == PC_SOLDIER ))) 
		{	
			G_AddEvent( targ, EV_LOSE_HAT, DirToByte(dir) );

			if( mod != MOD_K43_SCOPE &&	mod != MOD_GARAND_SCOPE && mod != MOD_STG44) 
			{
				if ( targ->client->sess.skill[SK_HEAVY_WEAPONS] >= 9 && targ->client->sess.playerType == PC_SOLDIER )
				take *= .5f;	// helmet shield gives us a lot of protection
					else
				take *= .8f;	// helmet gives us some protection
			}			
		} else if ( targ->client->sess.skill[SK_BATTLE_SENSE] >= 6 || targ->client->sess.skill[SK_HEAVY_WEAPONS] >= 9 )  {
			if( mod != MOD_K43_SCOPE &&	mod != MOD_GARAND_SCOPE && mod != MOD_STG44) 
				take *= .8f;	// helmet gives us some protection
		}

		// jaquboss - if we survive execution..
		targ->client->executed = qfalse;

		// jaquboss - execution
		if ( attacker->client && targ->client && dist <= 76 && !OnSameTeam( targ, attacker) && IS_PISTOL_ALL(attacker->client->ps.weapon) && attacker->client->sess.skill[SK_LIGHT_WEAPONS] >= 9 )
		{
			if (G_GetEnemyPosition(attacker, targ) == POSITION_BEHIND){
				targ->client->executed = qtrue; // jaquboss - don't change MOD
				attacker->client->sess.executions++;
			}
		}

		attacker->client->headshots++;

		// OSP - Record the headshot
		if( client && attacker && attacker->client && attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam)
		{
			if (targ->health > 0) 
				G_addStatsHeadShot(attacker, mod); 
		}

		if( g_debugBullets.integer ) 
			trap_SendServerCommand( attacker-g_entities, "print \"Head Shot\n\"\n");
				
		G_LogRegionHit( attacker, HR_HEAD );
		hr = HR_HEAD;
	} 
	else if ( targ->client && IsLegShot(attacker, targ, dir, point, mod) ) 
	{
		G_LogRegionHit( attacker, HR_LEGS );
		hr = HR_LEGS;
		if( g_debugBullets.integer ) 
			trap_SendServerCommand( attacker-g_entities, "print \"Leg Shot\n\"\n");
	} 
	else if ( targ->client && IsArmShot(targ, attacker, point, mod) ) 
	{
		G_LogRegionHit( attacker, HR_ARMS );
		hr = HR_ARMS;
		if( g_debugBullets.integer ) 
			trap_SendServerCommand( attacker-g_entities, "print \"Arm Shot\n\"\n");
	}
	else if (targ->client && targ->health > 0 && IsHeadShotWeapon( mod ) ) 
	{
		G_LogRegionHit( attacker, HR_BODY );
		hr = HR_BODY;
		if( g_debugBullets.integer ) 
			trap_SendServerCommand( attacker-g_entities, va("print \"Body Shot\"\n"));
	}

	if ( g_debugDamage.integer )
		G_Printf( "client:%i health:%i damage:%i mod:%s\n", targ->s.number, targ->health, take, modNames[mod] );

	if ( targ->client && 
		take &&
		g_painAnims.integer &&
		random() < ((float)g_painChance.integer/100) &&
		mod != MOD_POISON
		){

		// set enemy weapon
		BG_UpdateConditionValue( targ->s.number, ANIM_COND_ENEMY_WEAPON, 0, qfalse );
		if (attacker->client && mod != MOD_KICKED ) // do not do that in some cases
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_ENEMY_WEAPON, inflictor->s.weapon, qtrue );
		// set enemy location-front-behind
		BG_UpdateConditionValue( targ->s.number, ANIM_COND_ENEMY_POSITION, 0, qfalse );
		if (infront( targ, inflictor ))
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_ENEMY_POSITION, POSITION_INFRONT, qtrue );
		else
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_ENEMY_POSITION, POSITION_BEHIND, qtrue );

		// set impact point
		BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_UNUSED, qtrue );
		switch ( hr )
		{
		default:
			break;
		case HR_LEGS:
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_KNEE_RIGHT, qtrue );
			break;
		case HR_HEAD:
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_HEAD, qtrue );
			break;
		case HR_ARMS:
			BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_SHOULDER_RIGHT, qtrue );
			break;
		case HR_BODY:
			if ( targ->client->ps.eFlags & EF_PRONE )
				BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_GUT, qtrue );
			else if(point[2] - targ->r.absmin[2] > 0.75f*(targ->r.absmax[2] - targ->r.absmin[2]))
				BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_CHEST, qtrue );
			else if(point[2] - targ->r.absmin[2] > 0.50f*(targ->r.absmax[2] - targ->r.absmin[2]))
				BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_GUT, qtrue );
			else
				BG_UpdateConditionValue( targ->s.number, ANIM_COND_IMPACT_POINT, IMPACTPOINT_GROIN, qtrue );
			break;
		}

		// do a pain anim
		if ( (level.time > targ->pain_debounce_time)&&
			!(targ->flags & FL_GODMODE) &&
			!(targ->s.powerups & PW_INVULNERABLE) &&
			(targ->health - take > 0) &&
			mod != MOD_POISON ) {	//----(SA)	
				int	time=0;
				
				time = BG_AnimScriptEvent( &targ->client->ps, targ->client->pers.character->animModelInfo, ANIM_ET_PAIN, qfalse, qtrue );
				
				if ( time && g_painAnims.integer == 2 ){
					targ->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
					targ->client->ps.pm_time = time;
				}
		}
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client )
	{
		if ( attacker ) 
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		else 
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;

		if ( mod != MOD_POISON ) {
			client->damage_blood		+= take;
			client->damage_knockback	+= knockback;
		}

		if ( dir ) 
		{
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		}
		else 
		{
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// add to the attacker's hit counter
	if ( attacker->client && targ->client && targ != attacker && targ->health > limbo_health && mod != MOD_SWITCHTEAM && mod != MOD_SWAP_PLACES && mod != MOD_SUICIDE && mod != MOD_FEAR && mod != MOD_POISON && g_hitsounds.integer ) 
	{		
		if(OnSameTeam( targ, attacker) || targ->client->ps.powerups[PW_OPS_DISGUISED] )  // jaquboss, like in ETPro do this
			attacker->client->ps.persistant[PERS_HITS] -= damage;
		else 
			attacker->client->ps.persistant[PERS_HITS] += damage;
	}

	// remember that this player has no helmet
	if( headShot ) {
		if (  targ->client->sess.skill[SK_BATTLE_SENSE] < 6 || (targ->client->sess.skill[SK_HEAVY_WEAPONS] >= 9 && targ->client->sess.playerType == PC_SOLDIER))
			targ->client->ps.eFlags |= EF_HEADSHOT; 
		attacker->client->ps.persistant[PERS_HEADSHOTS] ++;
	}

	if (targ->client)
	{
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod	  = mod;
		targ->client->lasthurt_time   = level.time;
	}

	// do the damage
	if( take ) 
	{		
		targ->health -= take;

		// execuation should kill!
		if ( targ->client && targ->client->executed )
			targ->health = -1;

		// Gordon: don't ever gib POWS
		if( ( targ->health <= 0 ) && ( targ->r.svFlags & SVF_POW ) ) 
			targ->health = -1;

		// Ridah, can't gib with bullet weapons (except VENOM)
		// Arnout: attacker == inflictor can happen in other cases as well! (movers trying to gib things)
		if( targ->health <= GIB_HEALTH ) 
		{
			if( !G_WeaponCanGib( mod ) ) 
				targ->health = GIB_HEALTH + 1;
		}

		// JPW NERVE overcome previous chunk of code for making grenades work again
		// Arnout: only player entities! messes up ents like func_constructibles and func_explosives otherwise
		if( ( (targ->s.number < MAX_CLIENTS) && (take > 190) ) && !(targ->r.svFlags & SVF_POW) ) 
			targ->health = GIB_HEALTH - 1;

		if( targ->s.eType == ET_MOVER && !Q_stricmp( targ->classname, "script_mover" ) ) 
			targ->s.dl_intensity = 255.f * (targ->health / (float)targ->count);	// send it to the client

		if( targ->health <= 0 ) 
		{
			if( client && !wasAlive ) 
			{
				targ->flags |= FL_NO_KNOCKBACK;
				// OSP - special hack to not count attempts for body gibbage
				if( targ->client->ps.pm_type == PM_DEAD ) 
					G_addStats(targ, attacker, take, mod);
	
				if( (targ->health < limbo_health) && (targ->health > GIB_HEALTH) ) 
					limbo(targ, qtrue);

				//bani - #389
				if( targ->health <= GIB_HEALTH )
					GibEntity( targ, 0 );

			} 
			else 
			{
				targ->sound1to2 = hr;
				targ->sound2to1 = mod;
				targ->sound2to3 = (dflags & DAMAGE_RADIUS) ? 1 : 0;

				if( client ) 
				{					
					if( G_GetTeamFromEntity( inflictor ) != G_GetTeamFromEntity( targ ) ) 
					// jet Pilot - award 'bounty' experience for ending killing sprees
						G_AddKillSkillPoints( attacker, mod, hr, (dflags & DAMAGE_RADIUS), BG_GetStatBounty(&targ->client->ps));						
				}

				if( targ->health < -999 ) 
					targ->health = -999;

				targ->enemy		= attacker;
				targ->deathType = mod;

				if( targ->die ) 
					targ->die( targ, inflictor, attacker, take, mod );

				if( targ->s.eType == ET_MOVER && !Q_stricmp( targ->classname, "script_mover" ) && (targ->spawnflags & 8) ) 
					return;	// repairable script mover doesn't unlink itself but we don't want a second death script to be called

				// if we freed ourselves in death function
				if (!targ->inuse)
					return;

				// RF, entity scripting
				if ( targ->health <= 0)			// might have revived itself in death function
				{	
					if( targ->r.svFlags & SVF_BOT )
					{
#ifndef NO_BOT_SUPPORT
						// See if this is the first kill of this bot
						if (wasAlive)
							Bot_ScriptEvent( targ->s.number, "death", "" );
#endif
					}
					else if(( targ->s.eType != ET_CONSTRUCTIBLE && targ->s.eType != ET_EXPLOSIVE ) ||
							( targ->s.eType == ET_CONSTRUCTIBLE && !targ->desstages ) )	
						G_Script_ScriptEvent( targ, "death", "" ); // call manually if using desstages
				}

				// RF, record bot death
#ifndef NO_BOT_SUPPORT
				if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) 
					BotRecordDeath( targ->s.number, attacker->s.number );
#endif
			}
		} 
		else if ( targ->pain ) 
		{		
			if (dir) // Ridah, had to add this to fix NULL dir crash
			{	
				VectorCopy (dir, targ->rotate);
				VectorCopy (point, targ->pos3); // this will pass loc of hit
			} 
			else 
			{
				VectorClear( targ->rotate );
				VectorClear( targ->pos3 );
			}

			targ->pain (targ, attacker, take, point);
		} 
		else 
		{			
			G_addStats(targ, attacker, take, mod);			// OSP - update weapon/dmg stats			
		}

		// RF, entity scripting

		G_Script_ScriptEvent( targ, "pain", va("%d %d", targ->health, targ->health+take) );

#ifndef NO_BOT_SUPPORT	
		if (targ->s.number < MAX_CLIENTS && (targ->r.svFlags & SVF_BOT)) 
			Bot_ScriptEvent( targ->s.number, "pain", va("%d %d", targ->health, targ->health+take) );
#endif
		// RF, record bot pain
		if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) 
		{
#ifndef NO_BOT_SUPPORT
			BotRecordPain( targ->s.number, attacker->s.number, mod );
#endif
#ifdef OMNIBOTS
			// notify omni-bot framework
			botUserData.DataType = dtEntity;
			botUserData.udata.m_Entity = (GameEntity)attacker;
			Bot_Interface_SendEvent(PERCEPT_FEEL_PAIN, targ-g_entities, 0, 0, &botUserData);
#endif
		}

		// Ridah, this needs to be done last, incase the health is altered in one of the event calls
		if ( targ->client && (!(targ->client->ps.eFlags & EF_PLAYDEAD) || targ->health < 1)) 
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
	}	
}

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/

void G_RailTrail( vec_t* start, vec_t* end, vec_t* color ) 
{
	gentity_t* temp = G_TempEntity( start, EV_RAILTRAIL );
	VectorCopy( end, temp->s.origin2 );
	temp->s.dmgFlags = 0;
	temp->s.angles[0] = (int)(color[0]*255);
	temp->s.angles[1] = (int)(color[1]*255);
	temp->s.angles[2] = (int)(color[2]*255);
}

void G_RailBox( vec_t* origin, vec_t* mins, vec_t* maxs, vec_t* color ) 
{
	vec3_t	b1;
	vec3_t	b2;
	gentity_t* temp;

	VectorCopy(origin, b1);
	VectorCopy(origin, b2);
	VectorAdd(b1, mins, b1);
	VectorAdd(b2, maxs, b2);

	temp = G_TempEntity( b1, EV_RAILTRAIL );

	VectorCopy( b2,	temp->s.origin2);
	VectorCopy( color, temp->s.angles);
	temp->s.dmgFlags = 1;

	temp->s.angles[0] = (int)(color[0]*255);
	temp->s.angles[1] = (int)(color[1]*255);
	temp->s.angles[2] = (int)(color[2]*255);
}

#define MASK_CAN_DAMAGE		(CONTENTS_SOLID | CONTENTS_BODY)

qboolean CanDamage (gentity_t *targ, vec3_t origin) {
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;
	vec3_t offsetmins = { -16.f, -16.f, -16.f };
	vec3_t offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if(targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2]) {
		VectorCopy( targ->r.currentOrigin, midpoint );

		if( targ->s.eType == ET_MOVER ) {
			midpoint[2] += 32;
		}
	} else {
		VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
		VectorScale (midpoint, 0.5, midpoint);
	}

//	G_RailTrail( origin, dest );

	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if (tr.fraction == 1.0)
		return qtrue;

	if(&g_entities[tr.entityNum] == targ)
		return qtrue;

	if( targ->client ) {
		VectorCopy( targ->client->ps.mins, offsetmins );
		VectorCopy( targ->client->ps.maxs, offsetmaxs );
	}

	// this should probably check in the plane of projection, 
	// rather than in world coordinate
	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	// =========================

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[2];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	return qfalse;
}

void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t v )
{
	int i;

	if (!ent->r.bmodel)
		VectorSubtract(ent->r.currentOrigin,origin,v); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
	else {
		for ( i = 0 ; i < 3 ; i++ ) {
			if ( origin[i] < ent->r.absmin[i] ) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}
	}
}

/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod ) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest; 
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE )
		flags |= DAMAGE_HALF_KNOCKBACK;

	if( radius < 1 ) 
		radius = 1;

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement -- 
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for( i = 0 ; i < 3 ; i++ ) 
	{
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for( e = 0 ; e < level.num_entities ; e++ )
		g_entities[e].dmginloop = qfalse;

	for( e = 0 ; e < numListedEntities ; e++ )
	{
		ent = &g_entities[entityList[ e ]];

		if( ent == ignore ) 
			continue;

		if( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage ))
			continue;

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );
		if ( dist >= radius )
			continue;

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) )
		{			
			if( ent->dmgparent ) 
				ent = ent->dmgparent;

			if( ent->dmginloop ) 
				continue;
			
			if( AccuracyHit( ent, attacker ) ) 
				hitClient = qtrue;
			
			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;
			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		} 
		else
		{
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
		
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );

			if( tr.fraction < 1.0 )
			{
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent )
						ent = ent->dmgparent;
	
					if( ent->dmginloop )
						continue;
	
					if( AccuracyHit( ent, attacker ) )
						hitClient = qtrue;

					VectorSubtract (ent->r.currentOrigin, origin, dir);
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int)(points*0.1f), flags, mod );
				}
			}
		}
	}
	return hitClient;
}

/*
============
etpro_RadiusDamage
mutation of G_RadiusDamage which lets us selectively damage only clients or only non clients
============
*/
qboolean etpro_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod, qboolean clientsonly ) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest; 
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE )
		flags |= DAMAGE_HALF_KNOCKBACK;

	if( radius < 1 )
		radius = 1;

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement -- 
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for( e = 0 ; e < level.num_entities ; e++ )
		g_entities[e].dmginloop = qfalse;

	for( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if( ent == ignore )
			continue;

		if( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage ))
			continue;

		// tjw: need to include corpses in clientsonly since they 
		//      will be neglected from G_TempTraceIgnorePlayersAndBodies();
		if( clientsonly && !ent->client && ent->s.eType != ET_CORPSE)
			continue;

		if( !clientsonly && ent->client )
			continue;

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );

		if ( dist >= radius )
			continue;

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) ) {
			if( ent->dmgparent )
				ent = ent->dmgparent;

			if( ent->dmginloop )
				continue;

			if( AccuracyHit( ent, attacker ) )
				hitClient = qtrue;

			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		} else {
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
		
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if( tr.fraction < 1.0 ) {
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent )
						ent = ent->dmgparent;

					if( ent->dmginloop )
						continue;

					if( AccuracyHit( ent, attacker ) )
						hitClient = qtrue;

					VectorSubtract (ent->r.currentOrigin, origin, dir);
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int)(points*0.1f), flags, mod );
				}
			}
		}
	}

	return hitClient;
}
