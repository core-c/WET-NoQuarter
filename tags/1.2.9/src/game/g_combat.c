/*
 * name:		g_combat.c
 *
 * desc:
 *
 */
#include "g_local.h"
#include "../game/q_shared.h"
#include "etpro_mdx.h"

#ifdef ET_LUA
// *LUA*
#include "g_lua.h"
#endif

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

extern vec3_t muzzleTrace;

static qboolean	firstblood; // redeye
static qboolean	firstheadshot; // IRATA
static int		firstannouncetime;

int spreeSound[] = {
    GAMESOUND_SPREE_KILLINGSPREE,
    GAMESOUND_SPREE_RAMPAGE,
    GAMESOUND_SPREE_DOMINATING,
    GAMESOUND_SPREE_UNSTOPPABLE,
    GAMESOUND_SPREE_GODLIKE,
    GAMESOUND_SPREE_WICKEDSICK,
    GAMESOUND_SPREE_POTTER,
    GAMESOUND_SPREE_HOLYCOW,
    GAMESOUND_SPREE_HOLYCOW,
    GAMESOUND_SPREE_HOLYCOW,
};

const char *spreeMessage[] = {
	"^dis on a ^2Killing Spree^d!",
	"^dis on a ^2Rampage^d!",
	"^dis ^2Dominating^d!",
	"^drevels in his ^2Bloodbath^d!",
	"^dis a walking ^2Slaughterhouse^d!",
	"^dwreaks ^2Havoc ^dupon his foes!",
	"^dcuts through enemies like a ^2God ^2of ^2War^d!",
	"^dis the ^2Prophet of Doom^d!",
	"^dunleashes the ^>Shitstorm^d!",
	"^dis still unleashing the ^>Shitstorm^d!"
};

const char *humiliationMessage[] = {
	"^dmust be having a bad day!",
	"^dis getting his ass kicked!",
	"^dis a death magnet!",
	"^dneeds remedial combat training!",
	"^dseems well-aquainted with Death!",
	"^dstill can't kill shit!"
};

/*
==================
G_UpdateKillingSpree
==================
*/
static void G_UpdateKillingSpree(gentity_t *ent, gentity_t *att, qboolean death)
{
	int divisor;
	int old_spree = BG_GetKillSpree(&ent->client->ps);

    // Get new spree
    // Increment spree if the player makes a kill, if spree is still less than one, set it to one.
	// Set spree to zero if the player dies, unless it's already zero or less, in which case decrement it
	int spree = death ? (old_spree > 0 ? 0 : old_spree - 1) : ((old_spree < 0) ? 1 : old_spree + 1);

	// end of killing spree
	if (old_spree >= 5 && death) {
		// Update our best killing spree if it's less than our current one
		if (BG_GetStatBestSpree(&ent->client->ps) < old_spree) {
			BG_SetStatBestSpree(&ent->client->ps, old_spree);
		}

		if (g_announcer.integer & ANNOUNCE_KILLINGSPREES) {
			if (att == NULL || !att->client || ent == att) {
				AP(va("chat \"%s^d's killing spree ended after ^3%d^d kills! ^d(^1SUICIDE!^d)\"",
					ent->client->pers.netname, old_spree));
			}
			else {
				AP(va("chat \"%s^d's killing spree ended by ^*%s%s ^dafter ^3%d ^dkills!\"",
					ent->client->pers.netname, OnSameTeam(ent, att) ? "^1TEAMMATE^7 " : "",
					att->client->pers.netname, old_spree));
			}
		}
	}

	// end of death spree
	if (old_spree <= -10 && ! death && (g_announcer.integer & ANNOUNCE_DEATHSPREES)) {
		AP(va("chat \"%s^d's death spree has ended after ^3%d ^ddeaths!\"",
			ent->client->pers.netname, -old_spree));
	}

	// new killing spree (killing spree every 5 kills, % 5)
	if (spree >= 5 && (spree % 5) == 0 && (g_announcer.integer & ANNOUNCE_KILLINGSPREES)) {
		int snd_idx = (spree / 5) - 1;
		gentity_t *te = G_TempEntity(ent->s.pos.trBase, EV_GLOBAL_SOUND);
		snd_idx = (snd_idx <= 9)? snd_idx : 9;
/*
			AP(va("chat \"^1KILLING SPREE! ^*%s ^*%s ^d(^3%d ^dkills in a row!)\"",
				ent->client->pers.netname, spreeMessage[snd_idx], spree));
*/
		te->s.eventParm = spreeSound[snd_idx];
		te->r.svFlags |= SVF_BROADCAST;
		// core: a flag for spree/multikill events: effect3Time..
		te->s.effect3Time = -1966;
		te->s.effect2Time = ( 0x80 + snd_idx);
		te->s.effect1Time = ent->s.number;
	}
	// new death spree (death spree every 10 deaths, % 10)
	else if (spree <= -10 && (-spree % 10) == 0 && (g_announcer.integer & ANNOUNCE_DEATHSPREES)) {
		int snd_idx = (-spree / 10) - 1;
		gentity_t *te = G_TempEntity(ent->s.pos.trBase, EV_GLOBAL_SOUND);
        te->s.eventParm = GAMESOUND_SPREE_HUMILIATION;
		te->r.svFlags |= SVF_BROADCAST;

		AP(va("chat \"^1DEATH SPREE! ^*%s ^*%s ^d(^3%d ^ddeaths without a kill!)\"",
			ent->client->pers.netname,  humiliationMessage[snd_idx < 6 ? snd_idx : 5], -spree));
	}

	BG_SetKillSpree(&ent->client->ps, spree);

	// set bounty
	divisor = (spree < 0) ? 10 : 5;
	if ((spree / divisor) >= 0) {
		BG_SetStatBounty(&ent->client->ps, spree / divisor);
	}
}

/*
==================
G_UpdateKillingSpreeForMapEnd

only called on map end (before intermission starts)
==================
*/
void G_UpdateKillingSpreeForMapEnd(gentity_t *ent)
{
	int spree = BG_GetKillSpree(&ent->client->ps);

	// end active killing sprees and count them for best spree awards
	if (spree >= 5) {
		// Update our best killing spree if it's less than our current one
		if (BG_GetStatBestSpree(&ent->client->ps) < spree) {
			BG_SetStatBestSpree(&ent->client->ps, spree);
		}
	}

	// redeye - clear bounty (to avoid "bounty on player is gone" messages in intermission mode,
	// happens when people with a killing spree at map end change teams in intermission)
	BG_SetStatBounty(&ent->client->ps, 0);
	BG_SetKillSpree(&ent->client->ps, 0);
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score )
{
	if ( !ent || !ent->client ) {
		return;
	}

	// no scoring during pre-match warmup
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	// no score in LMS
	if( g_gametype.integer == GT_WOLF_LMS ) {
		return;
	}

	//ent->client->ps.persistant[PERS_SCORE] += score;
	ent->client->sess.game_points += score;

	//	level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	CalculateRanks( qfalse );
}

/*
============
AddKillScore

Adds score to both the client and his team, only used for playerkills, for lms
============
*/
void AddKillScore( gentity_t *ent, int score )
{
	if ( !ent || !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

	// someone already won
	if( level.lmsWinningTeam ) {
		return;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		ent->client->ps.persistant[PERS_SCORE] += score;
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	ent->client->sess.game_points += score;

	CalculateRanks( qfalse );
}

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self )
{
	if( g_gamestate.integer == GS_INTERMISSION ) {
		return;
	}

	{
		weapon_t primaryWeapon = G_GetPrimaryWeaponForClient(self->client);
		weapon_t secondaryWeapon = G_GetSecondaryWeaponForClient( self->client, primaryWeapon );

		switch (self->client->ps.weapon) {
			// rifle-guns dropping hack
			case WP_GPG40:
				self->client->ps.weapon = WP_KAR98;
				if( self->client->sess.rifleNade != 0 ) {
					self->client->sess.rifleNade = 0;
				}
				break;
			case WP_M7:
				self->client->ps.weapon = WP_CARBINE;
				if( self->client->sess.rifleNade != 0 ) {
					self->client->sess.rifleNade = 0;
				}
				break;
			// active scoped guns dropping hack
			case WP_GARAND_SCOPE:
				self->client->ps.weapon = WP_GARAND;
				break;
			case WP_K43_SCOPE:
				self->client->ps.weapon = WP_K43;
				break;
			case WP_FG42SCOPE:
				self->client->ps.weapon = WP_FG42;
				break;
			// set weapons mortar/browning/bar/mg42 hack
			case WP_MORTAR_SET:
				self->client->ps.weapon = WP_MORTAR;
				break;
			case WP_MORTAR2_SET:
				self->client->ps.weapon = WP_MORTAR2;
				break;
			case WP_MOBILE_BROWNING_SET:
				self->client->ps.weapon = WP_MOBILE_BROWNING;
				break;
			case WP_MOBILE_MG42_SET:
				self->client->ps.weapon = WP_MOBILE_MG42;
				break;
			case WP_BAR_SET:
				self->client->ps.weapon = WP_BAR;
				break;
			default:
				break;

		}

		if ( (primaryWeapon > WP_NONE && primaryWeapon < WP_FOOTKICK) &&
			(g_weaponItems.integer & WIF_DROPMAIN || self->client->ps.eFlags & EF_POISONED) ) {
			G_DropWeapon( self, primaryWeapon );
		}

		if ( (secondaryWeapon > WP_NONE && secondaryWeapon < WP_FOOTKICK) &&
			(g_weaponItems.integer & WIF_DROPPISTOLS ||	self->client->ps.eFlags & EF_POISONED) ) {
			G_DropWeapon( self, secondaryWeapon );
		}
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
	vec3_t		dir;

	if ( attacker && attacker != self ) {
		VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
	}
	else if ( inflictor && inflictor != self ) {
		VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
	}
	else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer )
{
	gentity_t *other = &g_entities[killer];
	vec3_t dir;

	VectorClear( dir );
	if (other->inuse) {
		if (other->client) {
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		}
		else if (!VectorCompare(other->s.pos.trDelta, vec3_origin)) {
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

/* IRATA: TODO unused function, new/old shrubbot cmd ?
==================
G_JetExplode
==================

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
*/

extern char *modNames[];
extern int bg_numMods;

/*
==================
IsHeadShotWeapon
==================
*/
qboolean IsHeadShotWeapon (int mod)
{
	if ( modWeaponTableMP[mod][0] != WP_NONE ) {
		return BG_Weapons[ modWeaponTableMP[mod][0] ].headShot;
    }
    return qfalse;
}

/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	int				contents		= 0;
	int				killer			= ENTITYNUM_WORLD;
	unsigned int	bounty;
	char			*killerName 	= "<world>";
	qboolean		nogib			= qtrue;
	qboolean		killedintank	= qfalse;
	weapon_t		weap;
	qboolean 		onSameTeam 		= OnSameTeam (self, attacker); // IRATA
	qboolean		fastBackstabbed	= (g_fastBackStab.integer && meansOfDeath == MOD_BACKSTAB)? qtrue : qfalse;

	// tjw: for g_shortcuts
	if(attacker && attacker->client) {
		self->client->pers.lastkiller_client = attacker->s.clientNum;
		attacker->client->pers.lastkilled_client = self->s.clientNum;
	}

	// tjw: if player has /kill'ed in response to recent
	//      damage, then we need to credit the last attacker
	//      with proper XP.
	if(meansOfDeath == MOD_FEAR) {
		weap = modWeaponTableMP[self->client->lasthurt_mod][0];
	}
	else if ( meansOfDeath == MOD_FALLING && self->client->pmext.shoved ) {
		weap = WP_NONE;
		attacker = &g_entities[self->client->pmext.pusher];
		meansOfDeath = MOD_SHOVE;
	}
	else {
		weap = modWeaponTableMP[meansOfDeath][0];
	}

	// xkan, 1/13/2003 - record the time we died.
	// tjw: moved this to death instead of limbo
	//      this is used for G_DropLimboHealth()
	if(!self->client->deathTime) {
		self->client->deathTime = level.time;
	}

	//unlagged - backward reconciliation #2
	// make sure the body shows up in the client's current position
	G_ReAdjustSingleClientPosition( self );
	//unlagged - backward reconciliation #2

	// jet Pilot - record bounty, as G_UpdateKillingSpree will clear it
	bounty = BG_GetStatBounty(&self->client->ps);
	G_UpdateKillingSpree(self, attacker, qtrue);

	if(attacker == self) {
		if(self->client) {
			self->client->pers.playerStats.suicides++;
			self->client->sess.deaths++;

			if (bounty > 0)	{
				AP(va("chat \"^9The bounty on ^*%s ^9is gone... he ended it himself!\"", self->client->pers.netname));
			}

			// jet Pilot - win a darwin award for killing yourself somehow,..
			// core: ..but only if it was not done with panzerfaust/bazooka.
			if ( meansOfDeath == MOD_PANZERFAUST || meansOfDeath == MOD_BAZOOKA ) {
				self->client->sess.panzernoob++;
			}
			else {
				self->client->sess.darwin++;
			}

			trap_PbStat ( self - g_entities, "suicide",
				va("%d %d %d", self->client->sess.sessionTeam, self->client->sess.playerType, weap)) ;
		}
	}
	else if ( onSameTeam) {
		// core: teamkill with panzerfaust/bazooka..
		if ( meansOfDeath == MOD_PANZERFAUST || meansOfDeath == MOD_BAZOOKA ) {
			attacker->client->sess.panzernoob += 2; // panzerkilling teammates is even more noobish..
		}
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

		// core: awards
		if ( meansOfDeath == MOD_POISON ) {
			attacker->client->sess.poisoneer++;
		}
		else if ( meansOfDeath == MOD_KICKED )
			attacker->client->sess.kickass++;
	}

	if ( meansOfDeath == MOD_FALLING )
		self->client->sess.newton++; // jaquboss

	// RF, record this death in AAS system so that bots avoid areas which have high death rates
	if( !onSameTeam ) {
		self->isProp = qfalse;	// were we teamkilled or not?
	}
	else {
		self->isProp = qtrue;
	}

	// if we got killed by a landmine, update our map
	if( self->client && meansOfDeath == MOD_LANDMINE ) {
		mapEntityData_t	*mEnt;

		if((mEnt = G_FindMapEntityData(&mapEntityData[0], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[0], mEnt );
		}

		if((mEnt = G_FindMapEntityData(&mapEntityData[1], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[1], mEnt );
		}
	}

	{
		mapEntityData_Team_t *teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0];	// swapped, cause enemy team
		mapEntityData_t	* mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, self->s.number, -1 );

		while( mEnt ) {
			if( mEnt->type == ME_PLAYER_DISGUISED ) {
				mapEntityData_t* mEntFree = mEnt;

				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );

				G_FreeMapEntityData( teamList, mEntFree );
			}
			else {
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );
			}
		}
	}

	// core: remove the player from the commandmap..
	// This to prevent interpolation when respawning (death-position -> spawnpoint).
	{
		mapEntityData_Team_t *teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[0] : &mapEntityData[1];
		mapEntityData_t *mEnt = teamList->activeMapEntityData.next;

		while( mEnt && mEnt != &teamList->activeMapEntityData ) {
			if( mEnt->data == self->s.number && (mEnt->type == ME_PLAYER || mEnt->type == ME_PLAYER_REVIVE) ) {
				mEnt = G_FreeMapEntityData( teamList, mEnt );
				break;
			}
			mEnt = mEnt->next;
		}
	}

	if( self->tankLink ) {
		G_LeaveTank( self, qfalse );
		killedintank = qtrue;
	}

	if ( (self->client->ps.pm_type == PM_DEAD && !(self->client->ps.eFlags & EF_PLAYDEAD)) ||
			g_gamestate.integer == GS_INTERMISSION ) {
		return;
	}

	// OSP - death stats handled out-of-band of G_Damage for external calls
	if(meansOfDeath == MOD_FEAR) {
		G_addStats(self, attacker, damage, self->client->lasthurt_mod);
		// tjw: this would normally be handled in G_Damage()
		G_AddKillSkillPoints(attacker, self->client->lasthurt_mod, 0, qfalse, bounty);
	}
	else if ( meansOfDeath != MOD_SWAP_PLACES ) {
		G_addStats(self, attacker, damage, meansOfDeath);
		if ( meansOfDeath == MOD_SHOVE && attacker && attacker->client && !onSameTeam ) {
			G_AddKillSkillPoints(attacker, MOD_SHOVE, 0, qfalse, bounty);
		}

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

		if(meansOfDeath < 0 || meansOfDeath >= bg_numMods ) {
			obit = "<bad obituary>";
		}
		else {
			obit = modNames[meansOfDeath];
		}


#ifdef OMNIBOTS
	//////////////////////////////////////////////////////////////////////////
		// send the events

		Bot_Event_Death(self-g_entities, &g_entities[attacker-g_entities], obit);
		Bot_Event_KilledSomeone(attacker-g_entities, &g_entities[self-g_entities], obit);
#endif

		G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n", killer, self->s.number, meansOfDeath, killerName, self->client->pers.netname, obit );
	}

#ifdef ET_LUA
	// *LUA*
	// pheno: Lua API callbacks
	// IRATA NQ like rework (Etpro style)
	if ( G_LuaHook_Obituary(self->s.number, killer, meansOfDeath) ) {
		if ( self->s.number < 0 || self->s.number >= MAX_CLIENTS ) {
			G_Error("G_LuaHook_Obituary: target out of range");
		}
	}
#endif

	// broadcast the death event to everyone
	{
		gentity_t *ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
		ent->s.eventParm = meansOfDeath;
		ent->s.otherEntityNum = self->s.number;
		ent->s.otherEntityNum2 = killer;
		ent->r.svFlags = SVF_BROADCAST;	// send to everyone
		ent->s.weapon = weap;
		// set death flags ...
		if ( self->client->executed ) {
			ent->s.dmgFlags |= OBIT_EXECUTED;
		}
	}

	self->enemy = attacker;

	// CHRUKER: b010 - Make sure covert ops lose their disguises
	self->client->ps.powerups[PW_OPS_DISGUISED] = 0;

	// JPW NERVE -- if player is holding ticking grenade, drop it
	if ((self->client->ps.grenadeTimeLeft)	&&
		(self->s.weapon != WP_DYNAMITE)		&&
		(self->s.weapon != WP_LANDMINE)		&&
		(self->s.weapon != WP_SATCHEL) )
	{
		vec3_t launchvel = {crandom(), crandom(), random()}, launchspot;

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
	if (self->s.weapon == WP_M7 && !self->client->ps.ammoclip[WeaponTable[WP_M7].clipindex]) {
		self->s.weapon = WP_CARBINE;
		self->client->ps.weapon = WP_CARBINE;
		self->client->ps.weaponstate = WEAPON_READY;
	}
	else if (self->s.weapon == WP_GPG40 && !self->client->ps.ammoclip[WeaponTable[WP_GPG40].clipindex]) {
		self->s.weapon = WP_KAR98;
		self->client->ps.weapon = WP_KAR98;
		self->client->ps.weaponstate = WEAPON_READY;
	}

	if (attacker && attacker->client) {
		if ( attacker == self || onSameTeam ) {

			// DHM - Nerve :: Complaint lodging
			if(attacker != self &&
				level.warmupTime <= 0 &&
				g_gamestate.integer == GS_PLAYING &&
				!G_shrubbot_permission(attacker, SBF_IMMUNITY)) {
                if( attacker->client->pers.localClient ) {
                	if(attacker->r.svFlags & SVF_BOT) {
                		trap_SendServerCommand( self-g_entities, "complaint -5" );
                	}
					else {
						trap_SendServerCommand( self-g_entities, "complaint -4" );
					}
				}
                else {
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

			if( g_gametype.integer == GT_WOLF_LMS ) {
				// calls CalculateRanks
				AddKillScore( attacker, WOLF_FRIENDLY_PENALTY );
			}
			else {
				// IRATA removed CalculateRanks from bottom of method
				// this makes sure it's only called once per player_die
				// in this case for TK - for normal kills it was called twice
				// see Addscore & AddKillScore
				CalculateRanks( qfalse );
			}
		}
		else {

			// JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
			// calls CalculateRanks (old way CalculateRanks was called here and at bottom)
			AddScore(attacker, WOLF_FRAG_BONUS);

			if( g_gametype.integer == GT_WOLF_LMS ) {
				if( level.firstbloodTeam == -1 )
					level.firstbloodTeam = attacker->client->sess.sessionTeam;

				AddKillScore( attacker, WOLF_FRAG_BONUS );
			}

			if ( attacker && attacker->client ) {
				attacker->client->lastKillTime = level.time;
				attacker->client->multikill++;
				attacker->client->ps.persistant[PERS_KILLS]++;
			}

			// jet Pilot - add up and announce killing sprees
			// cs: not while in limbo since PERS_KILLSPREE can be the value of who the client is spectating
			if ( attacker->client && !(attacker->client->ps.pm_flags & PMF_LIMBO) ) {
				G_UpdateKillingSpree(attacker, NULL, qfalse);
			}
		}
		// redeye - ETPub's hp left display + disable for bots
		// IRATA - we only display for head shot weapons & enemy kills
		if (!(self->r.svFlags & SVF_BOT) &&
		  attacker &&
		  attacker->health > 0 &&
		  IsHeadShotWeapon(meansOfDeath) &&
		  !onSameTeam ) {
			// TODO: maybe exclude each other
			if (jp_insanity.integer & JP_INSANITY_HPLEFT) {
				CPx( self - g_entities, va("%s \"%s ^7had %i HP left\"", "chat", attacker->client->pers.netname, attacker->health));
			}

			if (jp_insanity.integer & JP_INSANITY_HPLEFT_EXT) {
				float d = Distance (self->r.currentOrigin, attacker->r.currentOrigin);
				CPx( self - g_entities, va("%s \"%s ^7had %i HP left. Distance was %f\"", "chat", attacker->client->pers.netname, attacker->health, d));
			}
		}

	}
	else {
		// calls CalculateRanks
		AddScore( self, -1 );

		if( g_gametype.integer == GT_WOLF_LMS ) {
			AddKillScore( self, -1 );
		}
	}

	// Add team bonuses
	Team_FragBonuses(self, inflictor, attacker);

	G_DropItems(self);

	// send a fancy "MEDIC!" scream.  Sissies, ain' they?
	if (self->client != NULL) {
		if( self->health > GIB_HEALTH && !self->client->executed &&
			meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_SWITCHTEAM &&
			self->waterlevel < 3 &&
			!fastBackstabbed )
		{
			if (!attacker->client || !BG_Weapons[weap].limboKill){
					G_AddEvent( self, EV_MEDIC_CALL, 0 );
#ifdef OMNIBOTS
					// ATM: only register the goal if the target isn't in water.
					if(self->waterlevel <= 1) {
						Bot_AddFallenTeammateGoals(self, self->client->sess.sessionTeam);
					}
#endif
			}
		}
	}

	self->takedamage = qtrue;	// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	self->s.powerups = 0;
	self->s.loopSound = 0;

	self->client->limboDropWeapon = self->s.weapon; // store this so it can be dropped in limbo

	LookAtKiller( self, inflictor, attacker );
	self->client->ps.viewangles[0] = 0;
	self->client->ps.viewangles[2] = 0;

	// ydnar: so bodies don't clip into world
	self->r.maxs[2] = self->client->ps.crouchMaxZ;
	self->client->ps.maxs[2] = self->client->ps.crouchMaxZ;

	trap_LinkEntity( self );

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.timeCurrent + 800;

	// remove powerups
	{
		int flakJacket=self->client->ps.powerups[PW_FLAKJACKET];
		int helmetArmor=self->client->ps.powerups[PW_HELMETSHIELD];

		memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

		self->client->ps.powerups[PW_FLAKJACKET]=flakJacket;
		self->client->ps.powerups[PW_HELMETSHIELD]=helmetArmor;
	}

	// never gib in a nodrop
	// FIXME: contents is always 0 here
	if ( self->health <= GIB_HEALTH &&
			!fastBackstabbed &&
			!(contents & CONTENTS_NODROP) ) {
		GibEntity( self, killer );
		nogib = qfalse;
	}

	// normal death
	// for the no-blood option, we need to prevent the health
	// from going to gib level
	if(nogib && meansOfDeath != MOD_SWAP_PLACES){
		gentity_t	*tent;
		int event = EV_DEATH1+(rand()%4);

		if ( event > EV_DEATH4 ) {
			event = EV_DEATH4;
		}

		if ( event < EV_DEATH1 ) {
			event = EV_DEATH1;
		}

		tent = G_TempEntity( self->r.currentOrigin, event );
		tent->s.otherEntityNum2 = self->s.number;
		tent->r.svFlags = SVF_BROADCAST;	// send to everyone
		tent->s.weapon = weap;

		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH + 1;
		}

		if ( !(self->client->ps.eFlags & EF_PLAYDEAD) ){

			// set enemy weapon
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, weap, qtrue );

			// set enemy location
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, 0, qfalse );
			if (infront( self, inflictor )) {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, POSITION_INFRONT, qtrue );
			}
			else {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_POSITION, POSITION_BEHIND, qtrue );
			}

			if ( self->client->executed ||
				(meansOfDeath == MOD_SUICIDE && g_gamestate.integer == GS_PLAYING) || fastBackstabbed ) {
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH_FROM_BEHIND, qfalse, qtrue );
			}
			else {
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );
			}

		}

		// record the death animation to be used later on by the corpse
		self->client->torsoDeathAnim = self->client->ps.torsoAnim;
		self->client->legsDeathAnim = self->client->ps.legsAnim;
		self->client->deathAnimTime = level.time + self->client->ps.pm_time;

		// the body can still be gibbed
		self->die = body_die;
	}

	// redeye - firstblood message
	if( !firstblood &&
	  g_announcer.integer & ANNOUNCE_FIRSTBLOOD &&
	  self &&
	  self->client &&
	  attacker &&
	  attacker->client &&
	  attacker->s.number != ENTITYNUM_NONE &&
	  attacker->s.number != ENTITYNUM_WORLD &&
	  attacker != self &&
	  g_gamestate.integer == GS_PLAYING &&
	  !onSameTeam ) {
		if (!firstannouncetime || level.time > firstannouncetime) {
			G_globalSoundEnum(GAMESOUND_ANN_FIRSTBLOOD);
			firstannouncetime = level.time + 3000;
		}
		AP(va("cpm \"^7%s ^7drew ^1FIRST BLOOD ^7from ^7%s^7!\" -1", attacker->client->pers.netname, self->client->pers.netname));
		firstblood = qtrue;
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

	// the player really died this time
	self->client->ps.eFlags &= ~EF_PLAYDEAD;

	G_FadeSatchels( self );

	// IRATA done at top of method (
	// this makes sure it's only called once per player_die and should save cpu usage at collective deaths
	// see Addscore & AddKillScore
	// CalculateRanks();

	//Gordon: automatically go to limbo from tank
	if( killedintank  ) {
		limbo( self, qfalse ); // but no corpse
	}
	else if ( ((meansOfDeath == MOD_SUICIDE || meansOfDeath == MOD_FEAR) && g_gamestate.integer == GS_PLAYING) ||
				fastBackstabbed ) {
		self->client->deathAnim = qtrue;	// add animation time
		limbo( self, qtrue );
	}
	else if ( self->client->executed ) {
		self->client->deathAnim = qtrue; // add animation time
		limbo( self, qtrue );
	}
	else if ( BG_Weapons[weap].limboKill ) {
		self->client->deathAnim = qtrue; // add animation time
		limbo( self, qtrue );
	}
	else if( g_gametype.integer == GT_WOLF_LMS ) {
		if( !G_CountTeamMedics( self->client->sess.sessionTeam, qtrue ) ) {
			limbo( self, qtrue );
		}
	}
}

gentity_t* G_BuildHead(gentity_t *ent, grefEntity_t *refent, qboolean newRefent)
{
	gentity_t* head = G_Spawn();
	orientation_t or;			// DHM - Nerve

	head->classname = "head";

	// zinx - moved up here so realistic hitboxes can override
	VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6

	// core: For some reason the trap_Trace() (called in IsHeadShot()) does quite often
	// not detect a hit on the head-hitbox when a player is prone (when it -should- hit)..
	// However, the non-realhead code seems to work much better for prone players.
	// What is happening?..
	if ( g_realHead.integer & REALHEAD_HEAD && !(ent->client->ps.eFlags & EF_PRONE) ) {
		// zinx - realistic hitboxes
		if ( newRefent ) {
			mdx_gentity_to_grefEntity( ent, refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		}
		mdx_head_position( ent, refent, or.origin );

		G_SetOrigin( head, or.origin );

		VectorSet (head->r.mins , -6, -6, -6);
		VectorSet (head->r.maxs , 6, 6, 6);
	}
	else if ( trap_GetTag(ent->s.number, 0, "tag_head", &or) ) {
		G_SetOrigin( head, or.origin );
	}
	else {
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
		}
		else if( ent->client->ps.pm_flags & PMF_DUCKED ) {	// closer fake offset for 'head' box when crouching
			height = ent->client->ps.crouchViewHeight - 12;
		}
		else {
			height = ent->client->ps.viewheight;
		}


		VectorCopy( ent->client->ps.viewangles, angles );

		// NERVE - SMF - this matches more closely with WolfMP models
		if ( angles[PITCH] > 180 ) {
			dest = (-360 + angles[PITCH]) * 0.75;
		}
		else {
			dest = angles[PITCH] * 0.75;
		}
		angles[PITCH] = dest;

		// tjw: the angles need to be clamped for prone
		//      or the head entity will be underground or
		//      far too tall
		if((ent->client->ps.eFlags & EF_PRONE)) {
			angles[PITCH] = -10;
		}

		AngleVectors( angles, forward, right, up );
		if( ent->client->ps.eFlags & EF_PRONE ) {
			VectorScale(forward, 28, v);
			VectorMA(v, 7.0f, right, v);
		}
		else if((ent->client->ps.eFlags & EF_DEAD) ||
			(ent->client->ps.eFlags & EF_PLAYDEAD)) {
			VectorScale(forward, -26, v);
			VectorMA(v, 5.0f, right, v);
		}
		else {
			VectorScale( forward, 5, v);
			VectorMA(v, 5.0f, right, v);
		}
		VectorMA(v, 18.0f, up, v);

		VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
		head->r.currentOrigin[2] += height / 2;
	}

	VectorCopy (head->r.currentOrigin, head->s.origin);

	head->clipmask = CONTENTS_SOLID;
	head->r.contents = CONTENTS_SOLID;
	head->parent = ent;
	head->s.eType = ET_TEMPHEAD;

	trap_LinkEntity(head);

	return head;
}

/*
==================
G_BuildLeg
==================
*/
gentity_t* G_BuildLeg(gentity_t *ent, grefEntity_t *refent, qboolean newRefent)
{
	gentity_t* leg;
	vec3_t org;

	if( !(ent->client->ps.eFlags & (EF_PRONE | EF_DEAD | EF_PLAYDEAD)) ) {
		return NULL;
	}

	leg = G_Spawn();
	leg->classname = "leg";

	if (g_realHead.integer & REALHEAD_HEAD) {
		// zinx - realistic hitboxes
		if ( newRefent ) {
			mdx_gentity_to_grefEntity( ent, refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		}
		mdx_legs_position( ent, refent, org );

		org[2] += ent->client->pmext.proneLegsOffset;
		org[2] -= (playerlegsProneMins[2] + playerlegsProneMaxs[2]) * 0.5;

	}
	else {
		vec3_t flatforward;
		AngleVectors( ent->client->ps.viewangles, flatforward, NULL, NULL );
		flatforward[2] = 0;
		VectorNormalizeFast( flatforward );
		if(ent->client->ps.eFlags & EF_PRONE) {
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * -32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * -32;
		}
		else {
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * 32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * 32;
		}
		org[2] = ent->r.currentOrigin[2] + ent->client->pmext.proneLegsOffset;
	}

	G_SetOrigin( leg, org );

	VectorCopy( leg->r.currentOrigin, leg->s.origin );

	VectorCopy( playerlegsProneMins, leg->r.mins );
	VectorCopy( playerlegsProneMaxs, leg->r.maxs );
	leg->clipmask = CONTENTS_SOLID;
	leg->r.contents = CONTENTS_SOLID;
	leg->parent = ent;
	leg->s.eType = ET_TEMPLEGS;

	trap_LinkEntity( leg );

	return leg;
}

/*
==================
IsHeadShot
==================
*/
qboolean IsHeadShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod, grefEntity_t *refent, qboolean newRefent )
{
	// not a player or critter so bail
	if( !(targ->client) ) {
		return qfalse;
	}

	if (!IsHeadShotWeapon(mod) ) {
		return qfalse;
	}

	{
		gentity_t	*head = G_BuildHead( targ, refent, newRefent );
		trace_t		tr;
		vec3_t		start, end;
		gentity_t	*traceEnt;

		// trace another shot see if we hit the head
		VectorCopy( point, start );
		VectorMA( start, 64.0f, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );

		traceEnt = &g_entities[ tr.entityNum ];

		if( g_debugBullets.integer >= 3 ) {	// show hit player head bb

			G_RailBox( head->r.currentOrigin, head->r.mins, head->r.maxs, tv(1.f,0.f,0.f), head->s.number|HITBOXBIT_HEAD );

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if( tr.fraction != 1 ) {
				float s = (tr.fraction * 64);
				VectorMA(start, s, dir, end);
			}
			G_RailTrail( start, end, tv(1.f,0.f,0.f));
		}

		if (g_antilag.integer) {
			G_ReAdjustSingleClientPosition(targ);
		}

		G_FreeEntity( head );

		if( traceEnt == head ) {
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
IsLegShot
==================
*/
qboolean IsLegShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod, grefEntity_t *refent, qboolean newRefent )
{
	if (!(targ->client)) {
		return qfalse;
	}

	if(!point) {
		return qfalse;
	}

	if(!IsHeadShotWeapon(mod)) {
		return qfalse;
	}

	{
		gentity_t *leg = G_BuildLeg( targ, refent, newRefent );

		if( leg ) {
			gentity_t	*traceEnt;
			vec3_t		start, end;
			trace_t		tr;

			// trace another shot see if we hit the legs
			VectorCopy( point, start );
			VectorMA( start, 64.0f, dir, end );
			trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );

			traceEnt = &g_entities[ tr.entityNum ];

			if( g_debugBullets.integer >= 3 ) {	// show hit player head bb

				G_RailBox( leg->r.currentOrigin, leg->r.mins, leg->r.maxs, tv(1.f,0.f,0.f), leg->s.number|HITBOXBIT_LEGS);

				// show headshot trace
				// end the headshot trace at the head box if it hits
				if( tr.fraction != 1 ) {
					float s = (tr.fraction * 64);
					VectorMA( start, s, dir, end );
				}

				G_RailTrail( start, end, tv(1.f,0.f,0.f));
			}
			if (g_antilag.integer) {
				G_ReAdjustSingleClientPosition(targ);
			}

			G_FreeEntity( leg );

			if( traceEnt == leg ) {
				return qtrue;
			}
		}
		else {
			if( (point[2] - targ->r.absmin[2]) < ((targ->r.absmax[2] - targ->r.absmin[2]) * 0.4f) ) {
				return qtrue;
			}
		}
	}
	return qfalse;
}

/*
==================
IsArmShot
==================
*/
qboolean IsArmShot( gentity_t *targ, gentity_t* ent, vec3_t point, int mod )
{
	if (!(targ->client)) {
		return qfalse;
	}

	if (targ->health <= 0) {
		return qfalse;
	}

	if(!IsHeadShotWeapon (mod)) {
		return qfalse;
	}

	{
		vec3_t path, view;
		vec_t dot;

		VectorSubtract(targ->client->ps.origin, point, path);
		path[2] = 0;

		AngleVectors(targ->client->ps.viewangles, view, NULL, NULL);
		view[2] = 0;

		VectorNormalize(path);

		dot = DotProduct(path, view);

		if(dot > 0.4f || dot < -0.75f ) {
			return qfalse;
		}
	}
	return qtrue;
}

/*
==================
G_WeaponCanGib
==================
*/
qboolean G_WeaponCanGib( meansOfDeath_t mod )
{
    if (modWeaponTableMP[mod][0] != WP_NONE) {
        return BG_Weapons[modWeaponTableMP[mod][0]].gib;
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

// core: This variable needs to be here in order for G_BuildLeg() to access it..
static grefEntity_t refent;

void G_Damage(gentity_t *targ,	gentity_t	*inflictor, gentity_t	*attacker,	vec3_t	dir,
			  vec3_t	point,	int			damage,		int			dflags,		int		mod )
{
	int			take;
	int			knockback				= damage;
	int			limbo_health;
	qboolean	playerAttackedByPlayer	= (attacker && attacker->client && targ->client);
	qboolean	headShot				= qfalse;
	qboolean	wasAlive;
	hitRegion_t	hr						= HR_NUM_HITREGIONS;
	qboolean 	onSameTeam				= OnSameTeam( targ, attacker); // IRATA
	qboolean	fastBackstabbed			= (g_fastBackStab.integer && mod == MOD_BACKSTAB)? qtrue : qfalse;

	// early out if target cannot take damage

	if (!targ->takedamage) {
		return;
	}

	// the intermission has allready been qualified for, so don't allow any extra scoring
	// CHRUKER: b024 - Don't do damage if at warmup and warmupdamage is set to 'None' and the target is a client.
	if(level.intermissionQueued || (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 0 &&	targ->client)) {
		return;
	}

	// if there is no damage inflictor, then the world did it
	inflictor = inflictor ? inflictor : &g_entities[ENTITYNUM_WORLD];
	attacker  = attacker  ? attacker  : &g_entities[ENTITYNUM_WORLD];

	// Arnout: invisible entities can't be damaged
	if( targ->entstate == STATE_INVISIBLE || targ->entstate == STATE_UNDERCONSTRUCTION ) {
		return;
	}

	// xkan, 12/23/2002 - was the bot alive before applying any damage?
	wasAlive = (targ->health > 0);

	limbo_health = FORCE_LIMBO_HEALTH;
	if(g_forceLimboHealth.integer > 0) {
		limbo_health = -g_forceLimboHealth.integer;
	}
	else if ( g_forceLimboHealth.integer < 0 ) {
		limbo_health = g_forceLimboHealth.integer;
	}

	// Arnout: combatstate
	if ( playerAttackedByPlayer ) {
		if( mod == MOD_FLAMETHROWER && attacker->client->sess.skillBits[SK_SOLDIER] & (1<<7) ) {
			targ->client->ps.powerups[PW_HEAVY_BURNS] = level.time + 6000;
		}

		if( g_gamestate.integer == GS_PLAYING && attacker != targ ) {
			if( !onSameTeam ) {
				targ->client->combatState		|= (1 << COMBATSTATE_DAMAGERECEIVED);
				attacker->client->combatState	|= (1 << COMBATSTATE_DAMAGEDEALT);
			}
		}
	}

	// JPW NERVE - if submerged and on fire, dont do any more damage
	if ((targ->waterlevel >= 3) && (mod == MOD_FLAMETHROWER)) {
		return;
	}

	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER && !(targ->isProp) && !targ->scriptName) {
		if ( targ->use && targ->moverState == MOVER_POS1 ) {
			G_UseEntity( targ, inflictor, attacker );
		}
		return;
	}

	// TAT 11/22/2002
	// In the old code, this check wasn't done for props, so I put that check back in to make props_statue properly wor 4 means destructible
	switch( targ->s.eType ) {
		case ET_MOVER:
			if ( (targ->spawnflags & 4) && !targ->isProp ) {
				// core: a mover with no health, can not be damaged..
				if ( !targ->count ) {
					return;
				}
				// only explosive weapons can hurt this
				if( !G_WeaponIsExplosive( mod ) ) {
					return;
				}
				// check for team
				if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) {
					// jet Pilot - MODIFIED - allow team damage to tanks if (g_friendlyFire & 8)
					if ( !(g_friendlyFire.integer & FRIENDLY_FIRE_MOVERS) ) {
						return;
					}
				}
			}
			break;

		case ET_EXPLOSIVE:
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
			break;

		case ET_MISSILE:
			if ( targ->methodOfDeath == MOD_LANDMINE ) {
				if( targ->s.modelindex2 ) {
					if( G_WeaponIsExplosive( mod ) ) {
						mapEntityData_t	*mEnt;

						if((mEnt = G_FindMapEntityData(&mapEntityData[0], targ-g_entities)) != NULL) {
							G_FreeMapEntityData( &mapEntityData[0], mEnt );
						}
						if((mEnt = G_FindMapEntityData(&mapEntityData[1], targ-g_entities)) != NULL) {
							G_FreeMapEntityData( &mapEntityData[1], mEnt );
						}
						if( attacker && attacker->client ) {
							AddScore( attacker, 1 );
						}
						G_ExplodeMissile(targ);
					}
				}
				return;
			}
			break;

		case ET_CONSTRUCTIBLE:
			if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) {
				return;
			}
			if( G_GetWeaponClassForMOD( mod ) < targ->constructibleStats.weaponclass ) {
				return;
			}
			//bani - fix #238
			if ( mod == MOD_DYNAMITE ) {
				if( !( inflictor->etpro_misc_1 & 1 ) ) {
					return;
				}
			}
			break;

		default:
			break;
	}

	if ( targ->client ) {
		if ( targ->client->noclip || targ->client->ps.powerups[PW_INVULNERABLE] ) {
			return;
		}
	}

	// check for godmode
	if ( targ->flags & FL_GODMODE ) {
		return;
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	}
	else {
		VectorNormalize(dir);
	}

	//knockback = damage;
	if ( knockback > 200 ) {
		knockback = 200;
	}

	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}

	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}
	else if( dflags & DAMAGE_HALF_KNOCKBACK ) {
		knockback *= 0.5f;
	}

	// ydnar: set weapons means less knockback
	if( targ->client && ( targ->client->ps.weapon == WP_MORTAR2_SET || targ->client->ps.weapon == WP_MORTAR_SET || targ->client->ps.weapon == WP_MOBILE_MG42_SET ||
			targ->client->ps.weapon == WP_MOBILE_BROWNING_SET   || targ->client->ps.weapon == WP_BAR_SET ) ) {
		knockback *= 0.5;
	}
	// kicking hurts only a bit but knocks much more // IRATA: changed from 3.0 to 2.0
	if ( knockback && mod == MOD_KICKED ) {
		knockback *= 1.5;
	}

	// no knockback from friendly fire, unless specified
	if( targ->client && onSameTeam ) {
		if (!(g_friendlyFire.integer & FRIENDLY_FIRE_BOOSTING) && !G_FRIENDLYFIRE) {
			knockback = 0;
		}
	}

	// prevent 'tractor beam' knockback
	if (knockback < 0) {
		knockback = 0;
	}

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) {
		vec3_t			kvel;
		static float	mass = 200.0f;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		// jaquboss - are we pushed? Do not count when already flying..
		if ( attacker && attacker->client && ( targ->client->ps.groundEntityNum != ENTITYNUM_NONE || G_WeaponIsExplosive(mod) )){
			targ->client->pmext.shoved = qtrue;
			targ->client->pmext.pusher = attacker - g_entities;
		}

		// jP - TODO blast velocity cvar here?, for nade jumping, etc.
		if ( targ == attacker && !(mod != MOD_GRENADE && mod != MOD_GRENADE_LAUNCHER &&
								  mod != MOD_DYNAMITE && mod != MOD_GPG40 && mod != MOD_M7 &&
								  mod != MOD_LANDMINE) )
		{
			targ->client->ps.velocity[2] *= 0.25;
		}

		// set the timer so that the other client can't cancel out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int		t = knockback * 2;

			// cap it
			if ( t < 50 ) {
				t = 50;
			}
			else if ( t > 200 ) {
				t = 200;
			}

			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}

	}

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {
		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target if the attacker was on the same team
		if ( targ != attacker && onSameTeam  ) {
			if ( (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 1)) {
				return;
			}
			else if (!G_FRIENDLYFIRE) {
				if (mod == MOD_LANDMINE) {
					if (! (g_friendlyFire.integer & FRIENDLY_FIRE_LANDMINESHURTTEAMMATES)) {
						return;
					}
				}
				else {
					return; // if MOD is not a landmine, don't care about FRIENDLY_FIRE_LANDMINESHURTTEAMMATES
				}
			}
		}
	}

	if ( damage < 0 ) {
		damage = 0;
	}

	take = damage;

	// jet Pilot - mirror damage from team g_friendlyFire
	if ( (g_friendlyFire.integer & FRIENDLY_FIRE_MIRROR) && (targ != attacker)&& onSameTeam  ) {
		G_Damage(attacker, attacker, attacker, dir, point, take, dflags, mod);
	}

	// jet Pilot - half damage from team if specified. Inflictor player still takes full damage
	if ( (g_friendlyFire.integer & FRIENDLY_FIRE_HALF) && (targ != attacker)&& onSameTeam  ) {
		take *= .5f;
	}

	// adrenaline junkie!
	if( targ->client && (targ->client->ps.powerups[PW_ADRENALINE]>0) && (g_adrenaline.integer & ADF_HALFDAMAGE) ) {
		take *= .5f;
	}

	// Prevent 50% of explosive damage if wearing a Flak Jacket
	if (targ->client && G_WeaponIsExplosive(mod) && G_HasFlakJacket(targ)) {
		take *= .5;
	}

	if ( targ->client && IsHeadShot(attacker, targ, dir, point, mod, &refent, qtrue) ) {
		headShot = qtrue;
	}

	// core: There are 3 headshot modes:
	// headshot mode 1: HS does normal damage,  body-shots do no damage
	// headshot mode 2: 1 HS is immediate kill, body-shots do no damage
	// headshot mode 3: 1 HS is immediate kill, body-shots do normal damage
	if ( (g_headshot.integer == 1 || g_headshot.integer == 2) &&	// if headshot mode is on...
		playerAttackedByPlayer &&		// .. and attacked by player ..
		!headShot &&					// .. and we are a client who didn't receive headshot..
		modHeadshotModeWeapon[mod] )	// .. and not using certain weapons; Some weapons always do damage, even if it's no HS
	{
		return;							// .. then just quit ..
	}

	// core: falloff damage for non-headshots..
	if ( !headShot && point && jp_falloff.integer == 2 && dflags & DAMAGE_DISTANCEFALLOFF ) {
		if ( modFalloffDamage[mod] ) {
			vec3_t	shotvec;
			vec_t	dist;
			VectorSubtract( point, muzzleTrace, shotvec );
			dist = VectorLength( shotvec );
			take *= G_DistanceDamageFalloff( (float)dist );
		}
	}

	if ( headShot ) {
		int		minHS = BG_Weapons[attacker->s.weapon].minHSDamage;
        float	HSRatio = BG_Weapons[attacker->s.weapon].HSRatio;
		vec3_t	shotvec;
		vec_t	dist;

		take = (int)(HSRatio*take);

		if (take < minHS) {
			take = minHS;
		}

		if ( mod == MOD_SHOTGUN ) {
            take = 20;
		}

		VectorSubtract( point, muzzleTrace, shotvec );
		dist = VectorLength( shotvec );
		// damage falloff over distance..
		if ( jp_falloff.integer && dflags & DAMAGE_DISTANCEFALLOFF ) {
			if ( modFalloffDamage[mod] ) {
				take *= G_DistanceDamageFalloff( (float)dist );
			}
		}

		// Lose our hat (and absorb some damage) if we're wearing one
		if( !(targ->client->ps.eFlags & EF_HEADSHOT)) {
			if ( mod != MOD_SHOTGUN ) // shotgun cannot take off helmet
				G_AddEvent( targ, EV_LOSE_HAT, DirToByte(dir) );

			if( mod != MOD_K43_SCOPE &&	mod != MOD_GARAND_SCOPE && mod != MOD_STG44) {
				if ( targ->client->ps.powerups[PW_HELMETSHIELD] ) {
					take *= .5f;	// helmet shield gives us a lot of protection
				}
				else {
					take *= .8f;	// helmet gives us some protection
				}
			}
		}
		// jaquboss - if we survive execution..
		targ->client->executed = qfalse;

		// jaquboss - execution
		if ( playerAttackedByPlayer && dist <= 76 && !onSameTeam &&
		  IS_PISTOL_ALL(attacker->client->ps.weapon) &&
		  attacker->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<9) ) {
			if (G_GetEnemyPosition(attacker, targ) == POSITION_BEHIND) {
				targ->client->executed = qtrue; // jaquboss - don't change MOD
				attacker->client->sess.executions++;
			}
		}

		if ( mod != MOD_SHOTGUN ) {
			attacker->client->headshots++;
		}

		// OSP - Record the headshot
		if( playerAttackedByPlayer && attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam) {
			if (targ->health > 0 && mod != MOD_SHOTGUN) {
				G_addStatsHeadShot(attacker, mod);
			}
		}

		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Head Shot\n\"\n");
		}

		G_LogRegionHit( attacker, HR_HEAD , wasAlive );
		hr = HR_HEAD;

		// IRATA [*] - first-headshot message - new award ?
		if( !firstheadshot &&
			g_announcer.integer & ANNOUNCE_FIRSTHEADSHOT &&
			mod != MOD_SHOTGUN &&
			playerAttackedByPlayer &&
			attacker->s.number != ENTITYNUM_NONE &&
			attacker->s.number != ENTITYNUM_WORLD &&
			attacker != targ &&
			g_gamestate.integer == GS_PLAYING &&
			!onSameTeam )
		{
			if (!firstannouncetime || level.time > firstannouncetime) {
				G_globalSoundEnum(GAMESOUND_ANN_FIRTSHEADSHOT);
				firstannouncetime = level.time + 3000;
			}
			AP(va("cpm \"^7%s ^7blew out %s ^7brains with the ^sFIRST HEAD SHOT^7!\" -1", attacker->client->pers.netname , targ->client->pers.netname));
			firstheadshot = qtrue;
		}
	}
	else if ( targ->client && IsLegShot(attacker, targ, dir, point, mod, &refent, qfalse) ) {
		G_LogRegionHit( attacker, HR_LEGS, wasAlive );
		hr = HR_LEGS;
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Leg Shot\n\"\n");
		}
	}
	else if ( targ->client && IsArmShot(targ, attacker, point, mod) ) {
		G_LogRegionHit( attacker, HR_ARMS, wasAlive );
		hr = HR_ARMS;
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Arm Shot\n\"\n");
		}
	}
	else if (targ->client && targ->health > 0 && IsHeadShotWeapon( mod ) ) {
		G_LogRegionHit( attacker, HR_BODY, wasAlive);
		hr = HR_BODY;
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, va("print \"Body Shot\"\n"));
		}
	}


	if ( g_debugDamage.integer ) {
		// this doesn't print the real take for poison and foot kick  ...
		G_Printf( "client:%i health:%i damage:%i mod:%s\n", targ->s.number, targ->health, take, modNames[mod] );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( targ->client ) {

		if ( attacker ) {
			targ->client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		}
		else {
			targ->client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}

		if ( mod != MOD_POISON ) {
			targ->client->damage_blood		+= take;
			targ->client->damage_knockback	+= knockback;
		}

		// IRATA: Battle Sense 5 - Boots with Steel Toe
		// put it here so we avoid too much blood and knockback (see above)
		if ( mod == MOD_KICKED && attacker->client->sess.skillBits[SK_BATTLE_SENSE] & (1<<5) ) {
			take *= 2.0;
		}

		if ( dir ) {
			VectorCopy ( dir, targ->client->damage_from );
			targ->client->damage_fromWorld = qfalse;
		}
		else {
			VectorCopy ( targ->r.currentOrigin, targ->client->damage_from );
			targ->client->damage_fromWorld = qtrue;
		}
	}

	// add to the attacker's hit counter
	if ( playerAttackedByPlayer && targ != attacker && targ->health > limbo_health &&
			mod != MOD_SWITCHTEAM && mod != MOD_SWAP_PLACES && mod != MOD_SUICIDE && mod != MOD_FEAR &&	mod != MOD_POISON )
	{
		if ( onSameTeam || (targ->client->ps.powerups[PW_OPS_DISGUISED] && G_FRIENDLYFIRE) ) { // jaquboss, like in ETPro do this
			attacker->client->ps.persistant[PERS_HITS] -= damage;
		}
		else if ( !targ->client->ps.powerups[PW_OPS_DISGUISED] ) {
			attacker->client->ps.persistant[PERS_HITS] += damage;
		}
	}

	// remember that this player has no helmet
	if( headShot && mod != MOD_SHOTGUN  ) {
		targ->client->ps.eFlags |= EF_HEADSHOT;
		attacker->client->ps.persistant[PERS_HEADSHOTS]++;
	}

	if (targ->client) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod	  = mod;
		targ->client->lasthurt_time   = level.time;
		targ->client->lasthurt_location	= hr;
		// set the last teammate who damaged the target, and the damage..
		if ( onSameTeam && wasAlive && attacker != targ ) {
			targ->client->pers.lastteambleed_client = attacker->s.number;
			targ->client->pers.lastteambleed_dmg = take;
		}
	}

	// do the damage
	if( take ) {
		// core: headshot mode with an immediate kill?..
		if ( headShot && (g_headshot.integer == 2 || g_headshot.integer == 3) ) {
			if ( playerAttackedByPlayer ) {
				take = targ->health + (-GIB_HEALTH) - 2;
				targ->health = GIB_HEALTH + 1;
			}
		}
		else {
			targ->health -= take;

			// execution should kill!
			if ( targ->client && targ->client->executed ) {
				targ->health = -1;
			}

			// Ridah, can't gib with bullet weapons (except VENOM)
			// Arnout: attacker == inflictor can happen in other cases as well! (movers trying to gib things)
			if( targ->health <= GIB_HEALTH ) {
				if( !G_WeaponCanGib( mod ) && !fastBackstabbed ) {
					targ->health = GIB_HEALTH + 1;
				}
			}

			// JPW NERVE overcome previous chunk of code for making grenades work again
			// Arnout: only player entities! messes up ents like func_constructibles and func_explosives otherwise
			if ( targ->s.number < MAX_CLIENTS && take > 190 ) {
				targ->health = GIB_HEALTH - 1;
			}

			if ( targ->s.eType == ET_MOVER && targ->classnamehash == SCRIPT_MOVER_HASH && !Q_stricmp(targ->classname, "script_mover") ) {
				targ->s.dl_intensity = 255.f * (targ->health / (float)targ->count);	// send it to the client
				// core: The fix for the "damaged tank that cannot be repaired" bug.
				// Double-check for rounding errors..
				// targ->count = the mover-health set by the mapper (it never changes).
				// targ->health = current health.
				// targ->s.dl_intensity = the health as send to clients (as an int.  (a byte really)).
				// If health == 1, and count == 1200, then dl_intensity is rounded: 255*(1/1200) = 0.2125 = 0
				// .. so clients think it is damaged already, even if the mover still has some health left.
				if ( targ->health > 0 && targ->s.dl_intensity == 0 ) {
					targ->s.dl_intensity = 1;
				}
			}
		}

		if( targ->health <= 0 ) {
			if( targ->client && !wasAlive ) {
				targ->flags |= FL_NO_KNOCKBACK;
				// OSP - special hack to not count attempts for body gibbage
				if( targ->client->ps.pm_type != PM_DEAD ) {
					G_addStats(targ, attacker, take, mod);
				}
				if( (targ->health < limbo_health) && (targ->health > GIB_HEALTH) ) {
					limbo(targ, qtrue);
				}
				//bani - #389
				if( targ->health <= GIB_HEALTH ) {
					GibEntity( targ, 0 );
				}
			}
			else {
				targ->sound1to2 = hr;
				targ->sound2to1 = mod;
				targ->sound2to3 = (dflags & DAMAGE_RADIUS) ? 1 : 0;

				if( targ->client ) {
					if( G_GetTeamFromEntity( inflictor ) != G_GetTeamFromEntity( targ ) )
						// jet Pilot - award 'bounty' experience for ending killing sprees
						G_AddKillSkillPoints( attacker, mod, hr, (dflags & DAMAGE_RADIUS), BG_GetStatBounty(&targ->client->ps));
				}

				if( targ->health < -999 ) {
					targ->health = -999;
				}

				targ->enemy		= attacker;
				targ->deathType = mod;

				if( targ->die ) {
					targ->die( targ, inflictor, attacker, take, mod );
				}

				if( targ->s.eType == ET_MOVER &&
					targ->classnamehash == SCRIPT_MOVER_HASH && !Q_stricmp(targ->classname, "script_mover") &&
					(targ->spawnflags & 8) ) {
					return;	// repairable script mover doesn't unlink itself but we don't want a second death script to be called
				}

				// if we freed ourselves in death function
				if (!targ->inuse) {
					return;
				}

				// RF, entity scripting
				if ( targ->health <= 0)	{		// might have revived itself in death function

					if( (!(targ->r.svFlags & SVF_BOT) && targ->s.eType != ET_CONSTRUCTIBLE && targ->s.eType != ET_EXPLOSIVE ) ||
							( targ->s.eType == ET_CONSTRUCTIBLE && !targ->desstages ) ) {
						G_Script_ScriptEvent( targ, SE_DEATH, "" ); // call manually if using desstages
					}
				}

			}
		}
		else if ( targ->pain ) {
			if (dir) { // Ridah, had to add this to fix NULL dir crash
				VectorCopy (dir, targ->rotate);
				VectorCopy (point, targ->pos3); // this will pass loc of hit
			}
			else {
				VectorClear( targ->rotate );
				VectorClear( targ->pos3 );
			}

			targ->pain (targ, attacker, take, point);
		}
		else {
			 // FIX: SHOTGUN stats BUG
			 if (mod != MOD_POISON && mod != MOD_FLAMETHROWER) {
				G_addStats(targ, attacker, take, mod);			// OSP - update weapon/dmg stats
			 }
		}

		// RF, entity scripting
		G_Script_ScriptEvent( targ, SE_PAIN, va("%d %d", targ->health, targ->health+take) );

#ifdef OMNIBOTS
		// RF, record bot pain
		if (targ->s.number < level.maxclients) {
			// notify omni-bot framework
			Bot_Event_TakeDamage(targ-g_entities, attacker);
		}
#endif

		// Ridah, this needs to be done last, incase the health is altered in one of the event calls
		if ( targ->client && (!(targ->client->ps.eFlags & EF_PLAYDEAD) || targ->health < 1)) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
	}
}

/*
==================
G_RailTrail
==================
*/
void G_RailTrail( vec_t* start, vec_t* end, vec_t* color )
{
	gentity_t* temp = G_TempEntity( start, EV_RAILTRAIL );
	VectorCopy( end, temp->s.origin2 );
	temp->s.dmgFlags = 0;
	temp->s.angles[0] = (int)(color[0]*255);
	temp->s.angles[1] = (int)(color[1]*255);
	temp->s.angles[2] = (int)(color[2]*255);
	temp->s.density = -1;
}

/*
==================
G_RailBox
==================
*/
void G_RailBox( vec_t* origin, vec_t* mins, vec_t* maxs, vec_t* color, int index )
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

	temp->s.effect1Time = index+1;
}

#define MASK_CAN_DAMAGE		(CONTENTS_SOLID | CONTENTS_BODY)

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin)
{
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;
	static vec3_t offsetmins = { -16.f, -16.f, -16.f };
	static vec3_t offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if(targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2]) {
		VectorCopy( targ->r.currentOrigin, midpoint );

		if( targ->s.eType == ET_MOVER ) {
			midpoint[2] += 32;
		}
	}
	else {
		VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
		VectorScale (midpoint, 0.5, midpoint);
	}


	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	if(&g_entities[tr.entityNum] == targ) {
		return qtrue;
	}

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

/*
============
G_AdjustedDamageVec
============
*/
void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t v )
{
	if (!ent->r.bmodel) {
		VectorSubtract(ent->r.currentOrigin,origin,v); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
	}
	else {
		if ( origin[0] < ent->r.absmin[0] ) {
			v[0] = ent->r.absmin[0] - origin[0];
		}
		else if ( origin[0] > ent->r.absmax[0] ) {
			v[0] = origin[0] - ent->r.absmax[0];
		}
		else {
			v[0] = 0;
		}

		if ( origin[1] < ent->r.absmin[1] ) {
			v[1] = ent->r.absmin[1] - origin[1];
		}
		else if ( origin[1] > ent->r.absmax[1] ) {
			v[1] = origin[1] - ent->r.absmax[1];
		}
		else {
			v[1] = 0;
		}

		if ( origin[2] < ent->r.absmin[2] ) {
			v[2] = ent->r.absmin[2] - origin[2];
		}
		else if ( origin[2] > ent->r.absmax[2] ) {
			v[2] = origin[2] - ent->r.absmax[2];
		}
		else {
			v[2] = 0;
		}
	}
}

/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod )
{
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest;
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE ) {
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if( radius < 1 ) {
		radius = 1;
	}

	boxradius = M_SQRT2 * radius; // radius * sqrt(2) for bounding box enlargement --
	// bounding box was checking against radius / sqrt(2) if collision is along box plane

	mins[0] = origin[0] - boxradius;
	maxs[0] = origin[0] + boxradius;

	mins[1] = origin[1] - boxradius;
	maxs[1] = origin[1] + boxradius;

	mins[2] = origin[2] - boxradius;
	maxs[2] = origin[2] + boxradius;


	for( e = 0 ; e < trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES ) ; ++e ) {
		ent = &g_entities[entityList[ e ]];

		if( ent == ignore ) {
			continue;
		}

		if( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage )) {
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) ) {
			if( ent->dmgparent ) {
				ent = ent->dmgparent;
			}

			if( AccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}

			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		}
		else {
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );

			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );

			if( tr.fraction < 1.0 ) {
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent ) {
						ent = ent->dmgparent;
					}

					if( AccuracyHit( ent, attacker ) ) {
						hitClient = qtrue;
					}

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
qboolean etpro_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod, qboolean clientsonly )
{
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest;
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE ) {
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if( radius < 1 ) {
		radius = 1;
	}

	boxradius = M_SQRT2 * radius; // radius * sqrt(2) for bounding box enlargement --
	// bounding box was checking against radius / sqrt(2) if collision is along box plane

	mins[0] = origin[0] - boxradius;
	maxs[0] = origin[0] + boxradius;

	mins[1] = origin[1] - boxradius;
	maxs[1] = origin[1] + boxradius;

	mins[2] = origin[2] - boxradius;
	maxs[2] = origin[2] + boxradius;

	for( e = 0 ; e < trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES ) ; ++e ) {
		ent = &g_entities[entityList[ e ]];

		if( ent == ignore ) {
			continue;
		}

		if( !ent->takedamage && (!ent->dmgparent || !ent->dmgparent->takedamage) ) {
			continue;
		}

		// tjw: need to include corpses in clientsonly since they
		//      will be neglected from G_TempTraceIgnorePlayersAndBodies();
		if( clientsonly && !ent->client && ent->s.eType != ET_CORPSE) {
			continue;
		}

		if( !clientsonly && ent->client ) {
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );

		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) ) {
			if( ent->dmgparent ) {
				ent = ent->dmgparent;
			}

			if( AccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}

			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		}
		else {
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );

			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if( tr.fraction < 1.0 ) {
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent ) {
						ent = ent->dmgparent;
					}

					if( AccuracyHit( ent, attacker ) ) {
						hitClient = qtrue;
					}

					VectorSubtract (ent->r.currentOrigin, origin, dir);
					dir[2] += 24;

					G_Damage( ent, inflictor, attacker, dir, origin, (int)(points*0.1f), flags, mod );
				}
			}
		}
	}
	return hitClient;
}
