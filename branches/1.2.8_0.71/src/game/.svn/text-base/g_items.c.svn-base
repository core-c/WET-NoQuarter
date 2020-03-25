/*
 * name:		g_items.c
 *
 * desc:		Items are any object that a player can touch to gain some effect.
 *				Pickup will return the number of seconds until they should respawn.
 *				all items should pop when dropped in lava or slime.
 *				Respawnable items don't actually go away when picked up, they are
 *				just made invisible and untouchable.  This allows them to ride
 *				movers and respawn apropriately.
 *
*/

#include "g_local.h"

#ifdef PROFILER
#include "bg_profiler.h"
#endif

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

#define RESPAWN_NEVER		-1
#define	RESPAWN_KEY			4
#define RESPAWN_PARTIAL		998		// for multi-stage ammo/health
#define RESPAWN_PARTIAL_DONE 999	// for multi-stage ammo/health


//----(SA) Wolf keys
//======================================================================
int Pickup_Key( gentity_t *ent, gentity_t *other )
{
	other->client->ps.stats[STAT_KEYS] |= ( 1 << ent->item->giTag );
	return RESPAWN_KEY;
}


//======================================================================
// xkan, 10/26/2002
// extracted from Fill_Clip: add the specified ammount of ammo into the clip
// returns whether ammo was added to the clip
int AddToClip(
			  playerState_t *ps,// which player
			  int weapon,		// weapon to add ammo for
			  int ammomove,		// ammount to add. 0 means fill the clip if possible
			  int outOfReserve)	// is the amount to be added out of reserve
{
	int inclip, maxclip;
	int ammoweap = WeaponTable[weapon].ammoindex;

	if(weapon < WP_LUGER || weapon >= WP_NUM_WEAPONS)
		return qfalse;

	inclip	= ps->ammoclip[WeaponTable[weapon].clipindex];
	maxclip = GetWeaponTableData(weapon)->maxclip;

	if (!ammomove)	// amount to add to the clip not specified
		ammomove = maxclip - inclip;	// max amount that can be moved into the clip
	else if (ammomove > maxclip - inclip)
		ammomove = maxclip - inclip;

	if (outOfReserve) {
		// cap move amount if it's more than you've got in reserve
		if(ammomove > ps->ammo[ammoweap])
			ammomove = ps->ammo[ammoweap];
	}

	if(ammomove) {
		if (outOfReserve)
			ps->ammo[ammoweap] -= ammomove;
		ps->ammoclip[WeaponTable[weapon].clipindex] += ammomove;
		return qtrue;
	}
	return qfalse;
}

//======================================================================

/*
==============
Fill_Clip
	push reserve ammo into available space in the clip
==============
*/
void Fill_Clip (playerState_t *ps, int weapon) {
	AddToClip(ps, weapon, 0, qtrue);
}

/*
==============
Add_Ammo
	Try to always add ammo here unless you have specific needs
	(like the AI "infinite ammo" where they get below 900 and force back up to 999)

	fillClip will push the ammo straight through into the clip and leave the rest in reserve
==============
*/
//----(SA)	modified
// xkan, 10/25/2002 - modified to return whether any ammo was added.
int Add_Ammo(gentity_t *ent, int weapon, int count, qboolean fillClip) {
	int ammoweap = WeaponTable[weapon].ammoindex;
	int originalCount;
	int maxammo = BG_MaxAmmoForWeapon( ammoweap, ent->client->sess.skillBits, ent->client->sess.playerType );

	originalCount = ent->client->ps.ammo[ammoweap];

	switch (ammoweap) {
		// make sure if he picks up a grenade that he get's the "launcher" too
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_DYNAMITE:
		case WP_SATCHEL_DET:
			fillClip = qtrue;	// grenades always filter into the "clip"
			// no break here..
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			COM_BitSet( ent->client->ps.weapons, ammoweap );
			break;
		default:
			break;
	}

	if( fillClip ) {
		Fill_Clip(&ent->client->ps, weapon);
	}

	switch (ammoweap) {
		case WP_PANZERFAUST:
		case WP_FLAMETHROWER:
		case WP_BAZOOKA:
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			ent->client->ps.ammoclip[ammoweap] += count;

			if( ent->client->ps.ammoclip[ammoweap] > maxammo ) {
				ent->client->ps.ammoclip[ammoweap] = maxammo;
			}
			break;
		default:
			ent->client->ps.ammo[ammoweap] += count;

			if( ent->client->ps.ammo[ammoweap] > maxammo ) {
				ent->client->ps.ammo[ammoweap] = maxammo;
			}
			break;
	}

	if(count >= 999) { // 'really, give /all/'
		ent->client->ps.ammo[ammoweap] = count;
	}

	return (ent->client->ps.ammo[ammoweap] > originalCount);
}


// xkan, 9/18/2002 - Extracted AddMagicAmmo from Pickup_Weapon()
/*
=================================================================
AddMagicAmmo - added the specified number of clips of magic ammo
for any two-handed weapon

- returns whether any ammo was actually added
=================================================================
*/
qboolean AddMagicAmmo(gentity_t *receiver, int numOfClips)
{
	return BG_AddMagicAmmo(&receiver->client->ps, receiver->client->sess.skillBits, receiver->client->sess.sessionTeam, numOfClips);
}

//======================================================================

weapon_t G_GetPrimaryWeaponForClient( gclient_t *client )
{
	weapon_t			primary		= WP_NONE;

	// early out if not on a team
	if( client->sess.sessionTeam != TEAM_ALLIES && client->sess.sessionTeam != TEAM_AXIS )
		return WP_NONE;

// Record our Primary weapon
	// heavy weapons
	if ( COM_BitCheck( client->ps.weapons, WP_BAZOOKA ) )	{
		primary = WP_BAZOOKA;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_PANZERFAUST ) )	{
		primary = WP_PANZERFAUST;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MOBILE_BROWNING ) )	{
		primary = WP_MOBILE_BROWNING;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MOBILE_MG42 ) )	{
		primary = WP_MOBILE_MG42;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MORTAR ) )	{
		primary = WP_MORTAR;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MORTAR2 ) )	{
		primary = WP_MORTAR2;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_FLAMETHROWER ) )	{
		primary = WP_FLAMETHROWER;
	}
	// rifles and shotgun
	else if ( COM_BitCheck( client->ps.weapons, WP_SHOTGUN ) )	{
		primary = WP_SHOTGUN;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_K43 ) )	{
		primary = WP_K43;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_GARAND ) )	{
		primary = WP_GARAND;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_CARBINE ) )	{
		primary = WP_CARBINE;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_KAR98 ) )	{
		primary = WP_KAR98;
	}
	// Assault Rifles
	else if ( COM_BitCheck( client->ps.weapons, WP_STG44 ) )	{
		primary = WP_STG44;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_BAR ) )	{
		primary = WP_BAR;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_FG42 ) )	{
		primary = WP_FG42;
	}
	// SMGs
	else if ( COM_BitCheck( client->ps.weapons, WP_MP34 ) )	{
		primary = WP_MP34;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_STEN ) )	{
		primary = WP_STEN;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_STEN_MKII ) )	{
		primary = WP_STEN_MKII;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MP40 ) )	{
		primary = WP_MP40;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_THOMPSON ) )	{
		primary = WP_THOMPSON;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_VENOM ) )	{
		primary = WP_VENOM;
	}

	return primary;
}

weapon_t G_GetSecondaryWeaponForClient( gclient_t *client, weapon_t primary )
{
	weapon_t			secondary	= WP_NONE;

	// early out if not on a team
	if( client->sess.sessionTeam != TEAM_ALLIES && client->sess.sessionTeam != TEAM_AXIS ) {
		return WP_NONE;
	}

// Record our secondary weapon (usually a pistol sidearm)
	// Colts
	if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_SILENCEDCOLT ) ) {
		secondary = WP_AKIMBO_SILENCEDCOLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_COLT ) ) {
		secondary = WP_AKIMBO_COLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_SILENCED_COLT ) ) {
		secondary = WP_SILENCED_COLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_COLT ) ) {
		secondary = WP_COLT;
	}

	// Lugers
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_SILENCEDLUGER ) ) {
		secondary = WP_AKIMBO_SILENCEDLUGER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_LUGER ) ) {
		secondary = WP_AKIMBO_LUGER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_SILENCER ) ) {
		secondary = WP_SILENCER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_LUGER ) ) {
		secondary = WP_LUGER;
	}
// Special case for high level soldiers
	// Soldier 4+ *can* have an SMG as a secondary weapon
	if ( client->sess.playerType == PC_SOLDIER && client->sess.skillBits[SK_SOLDIER] & (1<<4)  ) {
		// don't have a secondary, *do* have a primary, and *can* have an SMG secondary
		if (secondary == WP_NONE && primary != WP_NONE ) {
			if ( COM_BitCheck( client->ps.weapons, WP_MP40 ) && primary != WP_MP40) {
				secondary = WP_MP40;
			}
			else if ( COM_BitCheck( client->ps.weapons, WP_THOMPSON ) && primary != WP_THOMPSON) {
				secondary = WP_THOMPSON;
			}
		}
	}

	return secondary;
}

weapon_t G_GetWeaponsForClient( gclient_t *client, weapon_t secondaryWeap )
{
	weapon_t			primary		= WP_NONE;
	weapon_t			secondary	= WP_NONE;

	// early out if not on a team
	if( client->sess.sessionTeam != TEAM_ALLIES && client->sess.sessionTeam != TEAM_AXIS ) {
		secondaryWeap = WP_NONE;

		return WP_NONE;
	}

// Record our Primary weapon
	// heavy weapons
	if ( COM_BitCheck( client->ps.weapons, WP_BAZOOKA ) )	{
		primary = WP_BAZOOKA;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_PANZERFAUST ) )	{
		primary = WP_PANZERFAUST;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MOBILE_BROWNING ) )	{
		primary = WP_MOBILE_BROWNING;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MOBILE_MG42 ) )	{
		primary = WP_MOBILE_MG42;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MORTAR ) )	{
		primary = WP_MORTAR;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MORTAR2 ) )	{
		primary = WP_MORTAR2;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_FLAMETHROWER ) )	{
		primary = WP_FLAMETHROWER;
	}
	// rifles and shotgun
	else if ( COM_BitCheck( client->ps.weapons, WP_SHOTGUN ) )	{
		primary = WP_SHOTGUN;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_K43 ) )	{
		primary = WP_K43;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_GARAND ) )	{
		primary = WP_GARAND;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_CARBINE ) )	{
		primary = WP_CARBINE;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_KAR98 ) )	{
		primary = WP_KAR98;
	}
	// Assault Rifles
	else if ( COM_BitCheck( client->ps.weapons, WP_STG44 ) )	{
		primary = WP_STG44;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_BAR ) )	{
		primary = WP_BAR;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_FG42 ) )	{
		primary = WP_FG42;
	}
	// SMGs
	else if ( COM_BitCheck( client->ps.weapons, WP_MP34 ) )	{
		primary = WP_MP34;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_STEN ) )	{
		primary = WP_STEN;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_STEN_MKII ) )	{
		primary = WP_STEN_MKII;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_MP40 ) )	{
		primary = WP_MP40;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_THOMPSON ) )	{
		primary = WP_THOMPSON;
	}
	else if ( COM_BitCheck( client->ps.weapons, WP_VENOM ) )	{
		primary = WP_VENOM;
	}

// Record our secondary weapon (usually a pistol sidearm)
	// Colts
	if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_SILENCEDCOLT ) ) {
		secondary = WP_AKIMBO_SILENCEDCOLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_COLT ) ) {
		secondary = WP_AKIMBO_COLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_SILENCED_COLT ) ) {
		secondary = WP_SILENCED_COLT;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_COLT ) ) {
		secondary = WP_COLT;
	}

	// Lugers
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_SILENCEDLUGER ) ) {
		secondary = WP_AKIMBO_SILENCEDLUGER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_AKIMBO_LUGER ) ) {
		secondary = WP_AKIMBO_LUGER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_SILENCER ) ) {
		secondary = WP_SILENCER;
	}
	else if( COM_BitCheck( client->ps.weapons, WP_LUGER ) ) {
		secondary = WP_LUGER;
	}
// Special case for high level soldiers
	// Soldier 4+ *can* have an SMG as a secondary weapon
	if ( client->sess.playerType == PC_SOLDIER && client->sess.skillBits[SK_SOLDIER] & (1<<4) ) {
		// don't have a secondary, *do* have a primary, and *can* have an SMG secondary
		if (secondary == WP_NONE && primary != WP_NONE) {
			if ( COM_BitCheck( client->ps.weapons, WP_MP40 )) {
				secondary = WP_MP40;
			}
			else if ( COM_BitCheck( client->ps.weapons, WP_THOMPSON )) {
				secondary = WP_THOMPSON;
			}
		}
	}
// end jP weapon check
	if (primary != WP_NONE || secondary != WP_NONE) {
		if (secondary != WP_NONE) {
			secondaryWeap = secondary;
		}
#ifdef DEBUG
		G_Printf("^dG_GetWeaponsForClient() : ^9Primary : ^2%s^9, Secondary : ^2%s\n",
				BG_Weapons[primary].name , BG_Weapons[secondary].name);
#endif
		return primary;
	}

#ifdef DEBUG
	G_Printf("^dG_GetWeaponsForClient() : ^9Primary : ^1WP_NONE^9, Secondary : ^1WP_NONE\n");
#endif

	secondaryWeap = WP_NONE;
	return WP_NONE;
}

void G_DropClip( gentity_t *ent, weapon_t weapon )
{
	vec3_t		angles, velocity, org, offset, mins, maxs;
	gclient_t	*client = ent->client;
	gentity_t	*ent2;
	gitem_t		*item;
	trace_t		tr;
	int			dropAmnt=0;
	qboolean	clipOnly=qfalse;

	// get ammount of dropped ammo
	if ( IS_PANZER_WEAPON(weapon) ) {
		clipOnly = qtrue;
	}

	dropAmnt = clipOnly ? 1 :GetWeaponTableData(WeaponTable[weapon].ammoindex)->maxclip;

	if ( !clipOnly && dropAmnt > ent->client->ps.ammo[WeaponTable[weapon].ammoindex]  )
		dropAmnt = ent->client->ps.ammo[WeaponTable[weapon].ammoindex];

	if ( clipOnly && ent->client->ps.ammoclip[WeaponTable[weapon].clipindex] <= 1 )
		return;

	if ( dropAmnt < 1 )
		return;

	item = BG_FindItemForAmmo( WeaponTable[weapon].ammoindex );

	// jet Pilot - Sanity Check
	if (item == NULL) {
		return;
	}

	VectorCopy( client->ps.viewangles, angles );

	// clamp pitch
	if ( angles[PITCH] < -30 )
		angles[PITCH] = -30;
	else if ( angles[PITCH] > 30 )
		angles[PITCH] = 30;

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 76, offset );
	offset[2] += client->ps.viewheight / 2.f;
	VectorScale( velocity, 80, velocity );
	velocity[2] += 50 + random() * 35;
	VectorAdd( client->ps.origin, offset, org );

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2*ITEM_RADIUS );

	trap_Trace( &tr, client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	ent2 = LaunchItem( item, org, velocity, client->ps.clientNum, FL_DROPPED_ITEM );
	ent2->count  = dropAmnt;
	ent2->s.teamNum = ent->client->sess.sessionTeam;
	if ( clipOnly )
		client->ps.ammoclip[WeaponTable[weapon].clipindex] -= dropAmnt;
	else
		client->ps.ammo[WeaponTable[weapon].ammoindex] -= dropAmnt;
}

void G_DropWeapon( gentity_t *ent, weapon_t weapon )
{
	vec3_t		angles, velocity, org, offset, mins, maxs;
	gclient_t	*client = ent->client;
	gentity_t	*ent2;
	gitem_t		*item = BG_FindItemForWeapon( weapon );
	trace_t		tr;

	// jet Pilot - Sanity Check
	if (item == NULL) {
		return;
	}

	VectorCopy( client->ps.viewangles, angles );

	// clamp pitch
	if ( angles[PITCH] < -30 ) {
		angles[PITCH] = -30;
	}
	else if ( angles[PITCH] > 30 ) {
		angles[PITCH] = 30;
	}

	// IlDuca - Joined the two "if" in one
	if ( IS_SECONDARY(weapon) ) {
		// IlDuca - Drop secondary weapon a bit more right then primary weapon
		// so them will not go one in top of the other
		angles[YAW] += 5;
		AngleVectors( angles, velocity, NULL, NULL );

		VectorScale( velocity, 96, offset );
		VectorScale( velocity, 100, velocity );
	}
	else {
		AngleVectors( angles, velocity, NULL, NULL );

		VectorScale( velocity, 64, offset );
		VectorScale( velocity, 75, velocity );
	}

	offset[2] += client->ps.viewheight / 2.f;
	velocity[2] += 50 + random() * 35;

	VectorAdd( client->ps.origin, offset, org );

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2*ITEM_RADIUS );

	trap_Trace( &tr, client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	ent2 = LaunchItem( item, org, velocity, client->ps.clientNum, FL_DROPPED_ITEM );

	// check to add silencer flag to pistols with silencers
	if ( IS_SILENCED_PISTOL(weapon) ||
		(IS_PISTOL(weapon) && COM_BitCheck(client->ps.weapons, GET_ALT_WEAPON(weapon))) )
	{
			ent2->flags |= FL_SILENCER; // add silencer flag to pistols with silencers
	}

	// jet Pilot - clear the alt-fire modes of the weapon
	COM_BitClear( client->ps.weapons, GET_ALT_WEAPON(weapon));

	// Clear the weapon from our inventory
	COM_BitClear( client->ps.weapons, weapon );


	// Clear out empty weapon, change to next best weapon
	G_AddEvent( ent, EV_WEAPONSWITCHED, 0 );

	// if it's our current weapon, switch away to no weapon.
	if( client->ps.weapon == weapon || client->ps.weapon == GET_ALT_WEAPON(weapon) ) {
		client->ps.weapon = WP_NONE;
	}

	// give the dropped weapon ammo
	if( weapon == WP_MORTAR || weapon == WP_MORTAR2 ) {
		ent2->count  = client->ps.ammo[WeaponTable[weapon].ammoindex] +
			           client->ps.ammoclip[WeaponTable[weapon].clipindex];
	}
	else {
		ent2->count  = client->ps.ammoclip[WeaponTable[weapon].clipindex];
		ent2->count2 = client->ps.ammo[WeaponTable[weapon].ammoindex];

		if ( IS_AKIMBO_WEAPON( weapon ) ) // jaquboss
			ent2->count3 =	client->ps.ammoclip[WeaponTable[BG_AkimboSidearm(weapon)].clipindex];

	}

	// give the rifle nades ammo if we had some
	if( weapon == WP_KAR98 || weapon == WP_CARBINE ) {
		ent2->delay = client->ps.ammo[WeaponTable[weapAlts[weapon]].ammoindex];
	}
	else {
		ent2->delay = 0;
	}

	ent2->s.teamNum = ent->client->sess.sessionTeam; // jaquboss

	// Gordon: um, modifying an item is not a good idea
	ent2->item->quantity = client->ps.ammoclip[WeaponTable[weapon].clipindex];
	client->ps.ammoclip[WeaponTable[weapon].clipindex] = 0;
#ifdef OMNIBOTS
	Bot_Event_RemoveWeapon(client->ps.clientNum, Bot_WeaponGameToBot(weapon));
#endif
}

qboolean G_CanPickupWeapon( weapon_t weapon, gentity_t* ent )
{
	if ( weapon == WP_KNIFE || weapon == WP_KNIFE_KABAR )
		return qtrue;

	// tjw: check heavy weapon restrictions
	if ( G_IsWeaponDisabled(ent, weapon, qfalse) )
		return qfalse;

	// jet Pilot - insanity mode always allows anyone to pick up anything
	if (jp_insanity.integer & JP_INSANITY_ALLWEAPONS)
		return qtrue;

	switch (ent->client->sess.playerType) {
		case PC_SOLDIER:
			if ( ent->client->sess.skillBits[SK_SOLDIER] & (1<<8) ) return qtrue;
			if ( IS_AKIMBO_PISTOL(weapon) && ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) ) return qtrue;
			if ( IS_PISTOL(weapon) ) return qtrue;
			return IS_SOLDIER_WEAPON(weapon);

		case PC_MEDIC:
			if ( IS_AKIMBO_PISTOL(weapon) && ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) && !(g_medics.integer & MEDIC_NOAKIMBO) ) return qtrue;
			if ( IS_PISTOL(weapon) ) return qtrue;
			if ( g_medics.integer & MEDIC_PISTOLONLY ) return qfalse;
			return IS_SUBMACHINEGUN(weapon);

		case PC_ENGINEER:
			if ( IS_AKIMBO_PISTOL(weapon) && ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) ) return qtrue;
			if ( IS_PISTOL(weapon) ) return qtrue;
			return IS_ENGINEER_WEAPON(weapon);

		case PC_FIELDOPS:
			if ( IS_AKIMBO_PISTOL(weapon) && ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) ) return qtrue;
			if ( IS_PISTOL(weapon) ) return qtrue;
			return IS_FIELDOPS_WEAPON(weapon);

		case PC_COVERTOPS:
			if ( IS_AKIMBO_WEAPON(weapon) && ent->client->sess.skillBits[SK_LIGHT_WEAPONS] & (1<<4) ) return qtrue;
			if ( IS_PISTOL(weapon) || IS_SILENCED_PISTOL(weapon) ) return qtrue;
			return IS_COVERT_WEAPON(weapon);

		default:
			return qfalse;
	}

	return BG_WeaponIsPrimaryForClassAndTeam( ent->client->sess.playerType, ent->client->sess.sessionTeam, weapon );
}


int Pickup_Weapon( gentity_t *ent, gentity_t *other )
{
	int			quantity=0, quantity2=0, quantity3=0; // jaquboss - storing ammo in weapons
	qboolean	alreadyHave = qfalse;
	weapon_t	secondary   = WP_NONE;

	// JPW NERVE -- magic ammo for any two-handed weapon
	if( ent->item->giTag == WP_AMMO ) {
		AddMagicAmmo( other, ent->count );
		// tjw: added for g_shortcuts
		if(ent->parent && ent->parent->client)
			other->client->pers.lastammo_client = ent->parent->s.clientNum;


		// if LT isn't giving ammo to self or another LT or the enemy, give him some props
		if( other->client->ps.stats[STAT_PLAYER_CLASS] != PC_FIELDOPS ) {
			if ( ent->parent && ent->parent->client && other->client->sess.sessionTeam == ent->parent->client->sess.sessionTeam ) {
				if (!(ent->parent->client->PCSpecialPickedUpCount % LT_SPECIAL_PICKUP_MOD)) {
					AddScore(ent->parent, WOLF_AMMO_UP);

					if(ent->parent && ent->parent->client)
						G_LogPrintf("Ammo_Pack: %d %d\n", ent->parent - g_entities, other - g_entities);	// OSP
				}

				ent->parent->client->PCSpecialPickedUpCount++;

				G_AddSkillPoints( ent->parent, SK_SIGNALS, 1.f );
				G_DEBUG_ADD_SKILL_POINTS( ent->parent, SK_SIGNALS, 1.f, "ammo pack picked up" );

#ifdef OMNIBOTS
				//omni-bot event
				if ( ent->parent )
					Bot_Event_RecievedAmmo(other-g_entities, ent->parent);
#endif
				// extracted code originally here into AddMagicAmmo -xkan, 9/18/2002
				// add 1 clip of magic ammo for any two-handed weapon
			}
		}
		return RESPAWN_NEVER;
	}

	quantity = ent->count;

	if ( g_weaponItems.integer & WIF_PICKUPAMMO)
		quantity2 = ent->count2; // jaquboss stored ammo

	if ( IS_AKIMBO_WEAPON( ent->item->giTag )) // jaquboss
		quantity3 = ent->count3; // jaquboss - akimbo ammo

	// always only one
	if ( ent->item->giTag == WP_KNIFE || ent->item->giTag == WP_KNIFE_KABAR ){
		if ( other->client->ps.ammoclip[ent->item->giTag] < BG_MaxAmmoForWeapon( ent->item->giTag, other->client->sess.skillBits, other->client->ps.stats[STAT_PLAYER_CLASS] ) ){
			Add_Ammo( other, ent->item->giTag, 1, qfalse );
			return -1;
		}
		return 0;
	}

	// check if player already had the weapon
	alreadyHave = COM_BitCheck( other->client->ps.weapons, ent->item->giTag );

	// JPW NERVE  prevents drop/pickup weapon "quick reload" exploit
	if( alreadyHave ) {
		int pickuped = 0;

		if ( quantity && ( !(g_weaponItems.integer & WIF_NOGARANDAMMOCLIP ) || ( ent->item->giTag != WP_GARAND && ent->item->giTag != WP_CARBINE ) ) )
			pickuped += Add_Ammo( other, ent->item->giTag, quantity, qfalse );

		if ( quantity2 )
			pickuped += Add_Ammo( other, ent->item->giTag, quantity2, qfalse );// jaquboss stored ammo

		if ( IS_AKIMBO_WEAPON( ent->item->giTag ) && quantity3) { // jaquboss
			pickuped += Add_Ammo( other, ent->item->giTag, quantity3, qfalse );// akimbo pistols should give all they have
		}

		// Gordon: secondary weapon ammo
		if( ent->delay )
			pickuped += Add_Ammo( other, weapAlts[ ent->item->giTag ], ent->delay, qfalse );

		if ( !pickuped && g_weaponItems.integer & WIF_KEEPWEAPONITEM2 )
			return 0;

		// keep the weapon itselfs, just steal ammo from it
		if ( g_weaponItems.integer & WIF_KEEPWEAPONITEM ) {
			if ( ent->count || ent->count2 || ent->count3 || ent->delay ) {
				int	makenoise = EV_ITEM_PICKUP;

				if( ent->noise_index ) {
					makenoise = EV_ITEM_PICKUP_QUIET;
					G_AddEvent( other, EV_GENERAL_SOUND, ent->noise_index );
				}
				G_AddEvent( other, makenoise, ent->s.modelindex );
			}

			ent->count = ent->count2 = ent->count3 = ent->delay = 0;
			return 0;
		}

	}
	else {
		if( level.time - other->client->dropWeaponTime < 1000 )
			return 0;

		if( other->client->ps.weapon == WP_MORTAR2_SET || other->client->ps.weapon == WP_MORTAR_SET || other->client->ps.weapon == WP_MOBILE_MG42_SET || other->client->ps.weapon == WP_MOBILE_BROWNING_SET  || other->client->ps.weapon == WP_BAR_SET )
			return 0;


		// See if we can pick it up
		if( G_CanPickupWeapon( ent->item->giTag, other )) {
			qboolean soldier4 = (other->client->sess.playerType == PC_SOLDIER && other->client->sess.skillBits[SK_SOLDIER] & (1<<4))? qtrue : qfalse;
			qboolean pickupSecondary = (IS_SECONDARY(ent->item->giTag))? qtrue : qfalse;
			weapon_t primaryWeapon = G_GetPrimaryWeaponForClient( other->client );
			secondary = G_GetSecondaryWeaponForClient( other->client, primaryWeapon );

			// core: a soldier with skill 4 can have a primary & secondary weapon
			if ( soldier4 ) {
				if ( primaryWeapon != WP_NONE ) {
					if ( IS_SOLDIER4_PRIMARYWEAPON(primaryWeapon) ) {
						// a soldier 4 with a primary4 weapon,..
						if ( IS_SOLDIER4_PRIMARYWEAPON(ent->item->giTag) ) {
							// ..wanting to pickup a primary4 weapon
							G_DropWeapon( other, primaryWeapon );
						} else {
							// ..wanting to pickup a secondary weapon..
							if ( secondary != WP_NONE ) {
								G_DropWeapon( other, secondary );
							}
						}
					} else {
						// a soldier 4 without a primary4 weapon,..(must be SMG)
						if ( IS_SOLDIER4_PRIMARYWEAPON(ent->item->giTag) ) {
							// ..wanting to pickup a primary4 weapon
							if ( secondary != WP_NONE ) {
								// soldier4 has SMG + pistols,..
								if ( IS_SUBMACHINEGUN(other->client->sess.latchPlayerWeapon2) ) {
									// ..and wants to keep SMG
									G_DropWeapon( other, secondary );
								} else {
									// ..and wants to keep pistols
									G_DropWeapon( other, primaryWeapon );
								}
							}
						}
					}
				} else {
					// a soldier 4 without a primary weapon,..
					if ( pickupSecondary ) {
						if ( secondary != WP_NONE ) {
							G_DropWeapon( other, secondary );
						}
					}
				}
			} else {
				if( primaryWeapon != WP_NONE && !pickupSecondary) // dont drop primary if we pickup secondary
					G_DropWeapon( other, primaryWeapon ); // drop our primary weapon
				else if ( secondary && pickupSecondary)
					G_DropWeapon( other, secondary );
			}

			// now pickup the other one
			other->client->dropWeaponTime = level.time;

			// add the weapon
			COM_BitSet( other->client->ps.weapons, ent->item->giTag );

			// DHM - Fixup mauser/sniper issues
			switch (ent->item->giTag) {
				case WP_FG42:
					COM_BitSet( other->client->ps.weapons, WP_FG42SCOPE);
					break;
				case WP_GARAND:
					COM_BitSet( other->client->ps.weapons, WP_GARAND_SCOPE);
					break;
				case WP_K43:
					COM_BitSet( other->client->ps.weapons, WP_K43_SCOPE );
					break;

				case WP_MORTAR:
					COM_BitSet( other->client->ps.weapons, WP_MORTAR_SET );
					break;
				case WP_MORTAR2:
					COM_BitSet( other->client->ps.weapons, WP_MORTAR2_SET );
					break;
				case WP_MOBILE_MG42:
					COM_BitSet( other->client->ps.weapons, WP_MOBILE_MG42_SET );
					break;
				case WP_MOBILE_BROWNING:
					COM_BitSet( other->client->ps.weapons, WP_MOBILE_BROWNING_SET );
					break;
				case WP_BAR:
					COM_BitSet( other->client->ps.weapons, WP_BAR_SET );
					break;

				case WP_CARBINE:
					if ( !G_IsWeaponDisabled( other, WP_M7, qtrue ) ) {
						COM_BitSet( other->client->ps.weapons, WP_M7 );
						if ( other->client->sess.rifleNade != 1 ){
							other->client->sess.rifleNade = 1;
							ClientUserinfoChanged( other->client->ps.clientNum );
						}
					}
					break;
				case WP_KAR98:
					if ( !G_IsWeaponDisabled( other, WP_GPG40, qtrue ) ) {
						COM_BitSet( other->client->ps.weapons, WP_GPG40 );
						if ( other->client->sess.rifleNade != 1 ){
							other->client->sess.rifleNade = 1;
							ClientUserinfoChanged( other->client->ps.clientNum );
						}
					}
					break;

				case WP_COLT:
					if ( ent->flags & FL_SILENCER ) {
						COM_BitSet( other->client->ps.weapons, WP_SILENCED_COLT );
					}
					break;
				case WP_LUGER:
					if ( ent->flags & FL_SILENCER ) {
						COM_BitSet( other->client->ps.weapons, WP_SILENCER );
					}
					break;
				case WP_SILENCED_COLT:
					COM_BitSet( other->client->ps.weapons, WP_COLT );
					if ( ent->flags & FL_SILENCER ) {
						COM_BitSet( other->client->ps.weapons, WP_SILENCED_COLT );
					}
					break;
				case WP_SILENCER:
					COM_BitSet( other->client->ps.weapons, WP_LUGER );
					if ( ent->flags & FL_SILENCER ) {
						COM_BitSet( other->client->ps.weapons, WP_SILENCER );
					}
					break;
				default:
					break;
			}

			other->client->ps.ammoclip[WeaponTable[ent->item->giTag].clipindex] = 0;
			other->client->ps.ammo[WeaponTable[ent->item->giTag].ammoindex] = 0;

			if( ent->item->giTag == WP_MORTAR || ent->item->giTag == WP_MORTAR2 ) {
				other->client->ps.ammo[WeaponTable[ent->item->giTag].clipindex] = quantity;
				// Gordon: secondary weapon ammo
				if( ent->delay )
					Add_Ammo( other, weapAlts[ ent->item->giTag ], ent->delay, qfalse );
			}
			else {
				other->client->ps.ammoclip[WeaponTable[ent->item->giTag].clipindex] = quantity;
				other->client->ps.ammo[WeaponTable[ent->item->giTag].ammoindex] = quantity2; // jaquboss give stored ammo

				if ( IS_AKIMBO_WEAPON( ent->item->giTag ) ) // jaquboss
					other->client->ps.ammoclip[WeaponTable[BG_AkimboSidearm(ent->item->giTag)].clipindex] = ent->count3;


				// Gordon: secondary weapon ammo
				if( ent->delay )
					other->client->ps.ammo[ weapAlts[ ent->item->giTag ] ] = ent->delay;
				CP(va("chat \"Picked up %d ammo\"", quantity));
			}
		}
		else {
			return 0;
		}
	}
#ifdef OMNIBOTS
	Bot_Event_AddWeapon(other->client->ps.clientNum, Bot_WeaponGameToBot(ent->item->giTag));
#endif
	return -1;
}


// jaquboss, unobsolete IT_AMMO
int Pickup_Ammo( gentity_t *ent, gentity_t *other )
{
	int			quantity;
	int			multiplier=1;

	// jaquboss don't eat when we drop weapon/clip for a sec
	if ( level.time - other->client->dropWeaponTime < 1000 )
		return 0;

	if ( ent->s.density )
		multiplier = ent->s.density;

	// JPW NERVE -- magic ammo for any two-handed weapon
	if( ent->item->giTag == WP_AMMO ) {
		AddMagicAmmo( other, ent->count * multiplier ); // jaquboss

		if ( ent->s.density == 2 ) {
			// multi-stage health first stage
			return ( RESPAWN_PARTIAL );
		}

		return -1;

	}

	if ( ent->count ) { // overwrite ammo, for dropclip
		quantity = ent->count;
	}
	else {
		quantity = ent->item->quantity * multiplier;
	}

	Add_Ammo( other, ent->item->giTag, quantity, qfalse );

	if ( ent->s.density == 2 ) {
		// multi-stage health first stage
		return ( RESPAWN_PARTIAL );
	}

	return -1;
}

//======================================================================

// jaquboss
int Pickup_Powerup (gentity_t *ent, gentity_t *other) {

	// uniforms
	if ( ent->item->giTag == PW_OPS_DISGUISED ) {

	other->client->ps.eFlags &= ~EF_PANTSED;

	if ( other->client->sess.sessionTeam == ent->item->giAmmoIndex ) // already took pants, but can't disguise
		return -1;

	other->client->ps.powerups[PW_OPS_DISGUISED] = 1;
	other->client->ps.powerups[PW_OPS_CLASS_1] = ent->item->quantity & 1;
	other->client->ps.powerups[PW_OPS_CLASS_2] = ent->item->quantity & 2;
	other->client->ps.powerups[PW_OPS_CLASS_3] = ent->item->quantity & 4;

	// jaquboss give him some name and rank
	Q_strncpyz(other->client->disguiseNetname,
		other->client->pers.netname,
		sizeof(other->client->disguiseNetname));

	other->client->disguiseRank = other->client->sess.rank;

	// jaquboss - these needs to be forwarded
	ClientUserinfoChanged( other->s.clientNum );

	return -1;
	}

	// helmet
	if ( ent->item->giTag == PW_HELMET ) {

	other->client->ps.eFlags &= ~EF_HEADSHOT;

	return -1;
	}

	// generic powerups
	if (ent->item->giTag == PW_NOFATIGUE || ent->item->quantity == INT_MAX  ) // sync with g_active @ 2231
	other->client->ps.powerups[ent->item->giTag] = ent->item->quantity;
		else
	other->client->ps.powerups[ent->item->giTag] = level.time + ent->item->quantity;


	return -1;
}


int Pickup_Health (gentity_t *ent, gentity_t *other) {
	int			max;
	int			quantity = 0;

	// tjw: addef for g_shortcuts
	if(ent->parent && ent->parent->client)
			other->client->pers.lasthealth_client = ent->parent->s.clientNum;

	max = other->client->ps.stats[STAT_MAX_HEALTH];
	if( other->client->sess.playerType == PC_MEDIC && g_medics.integer & MEDIC_EXTRAHP ) {
		max *= 1.12f;
	}

	if ( ent->s.density ) {	// multi-stage health
		if ( ent->s.density == 2 ) {	// first stage (it counts down)
			quantity = ent->item->quantity * 2;
		}
		else if ( ent->s.density == 1 ) {		// second stage
			quantity = ent->item->quantity;
		}
	}
	else {
		quantity = ent->item->quantity;
	}

	if ( other->client->ps.stats[STAT_TEAMAURA] & AURABIT_MEDIC )
		quantity += 10;


	if( other->client->ps.powerups[PW_HEAVY_BURNS] > 0 )
		quantity *= 0.5;

	other->health += quantity;


	if (other->health > max ) {
		other->health = max;
	}
	other->client->ps.stats[STAT_HEALTH] = other->health;

	// if medic isn't giving ammo to self or another medic or the enemy, give him some props
	if ( other->client->ps.stats[STAT_PLAYER_CLASS] != PC_MEDIC ) {
		if( ent->parent && ent->parent->client && other->client->sess.sessionTeam == ent->parent->client->sess.sessionTeam ) {
			if (!(ent->parent->client->PCSpecialPickedUpCount % MEDIC_SPECIAL_PICKUP_MOD)) {
				AddScore(ent->parent, WOLF_HEALTH_UP);
				G_LogPrintf("Health_Pack: %d %d\n", ent->parent - g_entities, other - g_entities);	// OSP
			}
			G_AddSkillPoints( ent->parent, SK_FIRST_AID, 1.f );
			G_DEBUG_ADD_SKILL_POINTS( ent->parent, SK_FIRST_AID, 1.f, "health pack picked up" );
			ent->parent->client->PCSpecialPickedUpCount++;

			// core: medics get HP for giving healthpacks (to non-medic teammates)..
			if ( g_medics.integer & MEDIC_MIRRORHEAL ) {
				// prevent medics shooting teammates to be able to heal them again,
				// and get HP themselves for it..
				if ( ent->parent->health > 0 &&
					((other->client->lasthurt_client != ent->parent->s.number) ||
					(other->client->lasthurt_client == ent->parent->s.number && level.time > other->client->lasthurt_time + 30000)) )
				{
					max = ent->parent->client->ps.stats[STAT_MAX_HEALTH];
					if( g_medics.integer & MEDIC_EXTRAHP ) max *= 1.12f;
					ent->parent->health += quantity;
					if (ent->parent->health > max ) ent->parent->health = max;
					ent->parent->client->ps.stats[STAT_HEALTH] = ent->parent->health;
				}
			}
		}
	}

	if ( ent->s.density == 2 ) {
		// multi-stage health first stage
		return ( RESPAWN_PARTIAL );
	}

#ifdef OMNIBOTS
	//omni-bot event
	if ( ent->parent )
		Bot_Event_Healed(other-g_entities, ent->parent);
#endif

	return -1;
}

//======================================================================

/*
===============
RespawnItem
===============
*/
void RespawnItem( gentity_t *ent ) {
	// randomly select from teamed entities
	if (ent->team) {
		gentity_t	*master;
		int	count;
		int choice;

		if ( !ent->teammaster ) {
			G_Error( "RespawnItem: bad teammaster");
		}
		master = ent->teammaster;

		for (count = 0, ent = master; ent; ent = ent->teamchain, count++)
			;

		choice = rand() % count;

		for (count = 0, ent = master; count < choice; ent = ent->teamchain, count++)
			;
	}

	ent->r.contents = CONTENTS_TRIGGER;
	//ent->s.eFlags &= ~EF_NODRAW;
	ent->flags &= ~FL_NODRAW;
	ent->r.svFlags &= ~SVF_NOCLIENT;
	trap_LinkEntity (ent);

#ifdef _DEBUG
	// play the normal respawn sound only to nearby clients
	G_AddEvent( ent, EV_ITEM_RESPAWN, 0 );
#endif

	ent->nextthink = 0;
}


/*
==============
Touch_Item
	if other->client->pers.autoActivate == PICKUP_ACTIVATE	(0), he will pick up items only when using +activate
	if other->client->pers.autoActivate == PICKUP_TOUCH		(1), he will pickup items when touched
	if other->client->pers.autoActivate == PICKUP_FORCE		(2), he will pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)
==============
*/
void Touch_Item_Auto( gentity_t *ent, gentity_t *other, trace_t *trace )
{
 	if( other->client->pers.autoActivate == PICKUP_ACTIVATE )
		return;

	if( !ent->active && ent->item->giType == IT_WEAPON ) {
		if( ent->item->giTag != WP_AMMO ) {
			if( !COM_BitCheck( other->client->ps.weapons, ent->item->giTag ) ) {
				return;	// force activate only
			}
		}
	}

	ent->active = qtrue;
	Touch_Item( ent, other, trace );

	if( other->client->pers.autoActivate == PICKUP_FORCE )		// autoactivate probably forced by the "Cmd_Activate_f()" function
		other->client->pers.autoActivate = PICKUP_ACTIVATE;		// so reset it.
}
void func_explosive_explode(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
/*
===============
Touch_Item
===============
*/
void Touch_Item( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	int		respawn;
	int		makenoise = EV_ITEM_PICKUP;

	// only activated items can be picked up
	if( !ent->active ) {
		return;
	}
	else {
		// need to set active to false if player is maxed out
		ent->active = qfalse;
	}

	if( !other->client ) {
		// only players can pick stuff up
		return;
	}

	if( other->health <= 0 ) {
		// dead people can't pickup
		return;
	}

	// tjw: if someone plays dead on a medkit don't let them pick it up or they will become the living dead.
	if((other->client->ps.eFlags & EF_PLAYDEAD))
		return;

	// prevent medics picking up own med packs if turned on
	if ( ent->item->giType == IT_HEALTH && (g_medics.integer & MEDIC_NOSELFPACK ||
		(g_medics.integer & MEDIC_NOSELFPACKPOISON && other->client->ps.eFlags & EF_POISONED) ) && ent->parent &&
			ent->parent->client && other->s.clientNum == ent->parent->s.clientNum )
		return;

	// the same pickup rules are used for client side and server side
	if ( !BG_CanItemBeGrabbed( &ent->s, &other->client->ps, other->client->sess.skillBits, other->client->sess.sessionTeam ) ) {
		return;
	}

	// jaquboss, dont catch hot knives
	if ( ent->damage && ent->s.pos.trType != TR_STATIONARY && ent->s.pos.trType != TR_GRAVITY_PAUSED && ent->s.pos.trType != TR_GRAVITY_FLOAT ) {
		return;
	}

	if(g_gamestate.integer != GS_PLAYING) {
		// OSP - Don't let them pickup winning stuff in warmup
		if( ent->item->giType != IT_POWERUP && ent->item->giType != IT_WEAPON && ent->item->giType != IT_AMMO && ent->item->giType != IT_HEALTH) {
			return;
		}
	}

	// call the item-specific pickup function
	switch( ent->item->giType ) {
		case IT_WEAPON:
			respawn = Pickup_Weapon( ent, other );
			break;
		case IT_AMMO:
			respawn = Pickup_Ammo( ent, other );
			break;
		case IT_HEALTH:
			respawn = Pickup_Health(ent, other);
			break;
		case IT_TEAM:
			respawn = Pickup_Team(ent, other);
			break;
		case IT_POWERUP:
			respawn = Pickup_Powerup(ent, other);
			break;
		default:
			return;
	}

//	G_LogPrintf( "Finished pickup function\n" );

	if ( !respawn ) {
		return;
	}

	// jaquboss - moved here to not spam
	G_LogPrintf( "Item: %i %s\n", other->s.number, ent->item->classname );

	// play sounds
	if( ent->noise_index ) {
		// (SA) a sound was specified in the entity, so play that sound
		// (this G_AddEvent) and send the pickup as "EV_ITEM_PICKUP_QUIET"
		// so it doesn't make the default pickup sound when the pickup event is recieved
		makenoise = EV_ITEM_PICKUP_QUIET;
		G_AddEvent( other, EV_GENERAL_SOUND, ent->noise_index );
	}

	G_AddEvent( other, makenoise, ent->s.modelindex );

	// powerup pickups are global broadcasts
	if ( ent->item->giType == IT_TEAM ) {
		gentity_t* te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_ITEM_PICKUP );
		te->s.eventParm = ent->s.modelindex;
		te->r.svFlags |= SVF_BROADCAST;
	}

//	G_LogPrintf( "Firing item targets\n" );

	// fire item targets
	G_UseTargets (ent, other);

	if ( respawn == RESPAWN_PARTIAL ) {
		// multi-stage health
		ent->s.density--;
		if ( ent->s.density ) {
			// still not completely used up ( (SA) this will change to == 0 and stage 1 will be a destroyable item (plate/etc.) )
			ent->active = qtrue;		// re-activate
			trap_LinkEntity( ent );
			return;
		}
	}

	// dropped items will not respawn
	if ( ent->flags & FL_DROPPED_ITEM ) {
		ent->freeAfterEvent = qtrue;
	}

	// picked up items still stay around, they just don't draw anything.
	// This allows respawnable items to be placed on movers.
	ent->r.svFlags	|= SVF_NOCLIENT;
	ent->flags		|= FL_NODRAW;
	ent->r.contents =  0;

	// ZOID
	// A negative respawn times means to never respawn this item (but don't
	// delete it).  This is used by items that are respawned by third party
	// events such as ctf flags
	if ( respawn <= 0 ) {
		ent->nextthink = 0;
		ent->think = 0;
	}
	else {
		ent->nextthink = level.time + respawn * 1000;
		ent->think = RespawnItem;
	}
	trap_LinkEntity( ent );
}


//======================================================================

/*
================
LaunchItem

Spawns an item and tosses it forward
================
*/
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum, int flags )
{
	gentity_t	*dropped;
	trace_t		tr;
	vec3_t		vec, temp;
	int			i;

	dropped = G_Spawn();

	dropped->s.eType = ET_ITEM;
	dropped->s.modelindex = item - bg_itemlist;	// store item number in modelindex
	dropped->s.otherEntityNum2 = 1;	// DHM - Nerve :: this is taking modelindex2's place for a dropped item

	dropped->classname = item->classname;
	dropped->item = item;
	VectorSet( dropped->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );			//----(SA)	so items sit on the ground
	VectorSet( dropped->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2*ITEM_RADIUS );	//----(SA)	so items sit on the ground
	dropped->r.contents = CONTENTS_TRIGGER|CONTENTS_ITEM;

	dropped->clipmask = CONTENTS_SOLID | CONTENTS_MISSILECLIP;		// NERVE - SMF - fix for items falling through grates

	dropped->touch = Touch_Item_Auto;

	trap_Trace( &tr, origin, dropped->r.mins, dropped->r.maxs, origin, ownerNum, MASK_SOLID );
	if ( tr.startsolid ) {
		VectorSubtract( g_entities[ownerNum].s.origin, origin, temp );
		VectorNormalize( temp );

		for ( i=16; i<=48; i+=16 ) {
			VectorScale( temp, i, vec );
			VectorAdd( origin, vec, origin );

			trap_Trace( &tr, origin, dropped->r.mins, dropped->r.maxs, origin, ownerNum, MASK_SOLID );
			if ( !tr.startsolid )
				break;
		}
	}

	G_SetOrigin( dropped, origin );
	dropped->s.pos.trType = TR_GRAVITY;
	dropped->s.pos.trTime = level.time;
	VectorCopy( velocity, dropped->s.pos.trDelta );

	// ydnar: set yaw to parent angles
	temp[ PITCH ] = 0;
	temp[ YAW ] = g_entities[ ownerNum ].s.apos.trBase[ YAW ];
	temp[ ROLL ] = 0;
	G_SetAngle( dropped, temp );

	dropped->s.eFlags |= EF_BOUNCE_HALF;

	if (item->giType == IT_TEAM) {
		// Special case for CTF flags
		gentity_t* flag = &g_entities[ g_entities[ownerNum].client->flagParent ];

		dropped->s.otherEntityNum =	g_entities[ownerNum].client->flagParent;	// store the entitynum of our original flag spawner
		dropped->s.density = 1;
		dropped->think = Team_DroppedFlagThink;
		dropped->nextthink = level.time + 30000;

		if(level.gameManager) {
			int n = 0;
			char *params[2] = {
				"axis_object_dropped",
				"allied_object_dropped"
			};

			if(flag->item && flag->item->giTag == PW_REDFLAG) {
				n = 1;
			}
			G_Script_ScriptEvent(level.gameManager, SE_TRIGGER, params[n]);
		}
		G_Script_ScriptEvent( flag, SE_TRIGGER, "dropped" );
	}
	else {
		// auto-remove after 30 seconds
		dropped->think = G_FreeEntity;

		dropped->nextthink = level.time + 30000;
	}

	// jet Pilot - Implementation of c3a's Dropped Weapon code
	dropped->flags = flags; // FL_DROPPED_ITEM;

	dropped->physicsBounce = g_weaponBounce.value;
	dropped->physicsSlide = qtrue;
	dropped->physicsFlush = qtrue;

	// health/ammo can potentially be multi-stage (multiple use)
	if( dropped->item->giType == IT_HEALTH || dropped->item->giType == IT_AMMO ) {
		int i;

		for(i=0; i<MAX_ITEM_MODELS && dropped->item->world_model[i] ;i++) {}
		dropped->s.density = i-1;
	}

	trap_LinkEntity (dropped);

	return dropped;
}

/*
================
Drop_Item

Spawns an item and tosses it forward
================
*/
gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle, qboolean novelocity) {
	vec3_t	velocity;
	vec3_t	angles;

	VectorCopy( ent->s.apos.trBase, angles );
	angles[YAW] += angle;
	angles[PITCH] = 0;	// always forward

	if (novelocity) {
		VectorClear (velocity);
	}
	else {
		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 150, velocity );
		velocity[2] += 200 + crandom() * 50;
	}

	return LaunchItem( item, ent->s.pos.trBase, velocity, ent->s.number, FL_DROPPED_ITEM );
}

/*
================
Use_Item

Respawn the item
================
*/
void Use_Item( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	RespawnItem( ent );
}

//======================================================================

/*
================
FinishSpawningItem

Traces down to find where an item should rest, instead of letting them
free fall from their spawn points
================
*/
void FinishSpawningItem( gentity_t *ent ) {
	trace_t		tr;
	vec3_t		dest;
	vec3_t		maxs;

	if ( ent->spawnflags & 1 ) {	// suspended

		VectorSet( ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS );
		VectorSet( ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS );
		VectorCopy (ent->r.maxs, maxs);
	}
	else {
		// Rafael
		// had to modify this so that items would spawn in shelves
		VectorSet( ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS );
		VectorCopy (ent->r.maxs, maxs);
		maxs[2] /= 2;
	}

	ent->r.contents = CONTENTS_TRIGGER|CONTENTS_ITEM;
	ent->touch = Touch_Item_Auto;
	ent->s.eType = ET_ITEM;
	ent->s.modelindex = ent->item - bg_itemlist;		// store item number in modelindex

	ent->s.otherEntityNum2 = 0;		// DHM - Nerve :: takes modelindex2's place in signaling a dropped item
	if(ent->model) {
		ent->s.modelindex2 = G_ModelIndex(ent->model);
	}

	// using an item causes it to respawn
	ent->use = Use_Item;

//----(SA) moved this up so it happens for suspended items too (and made it a function)
	G_SetAngle (ent, ent->s.angles);

	if ( ent->spawnflags & 1 ) {	// suspended
		G_SetOrigin( ent, ent->s.origin );
	}
	else {

		VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
		trap_Trace( &tr, ent->s.origin, ent->r.mins, maxs, dest, ent->s.number, MASK_SOLID );

		if ( tr.startsolid )
		{
			vec3_t temp;

			VectorCopy (ent->s.origin, temp);
			temp[2] -= ITEM_RADIUS;

			VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
			trap_Trace( &tr, temp, ent->r.mins, maxs, dest, ent->s.number, MASK_SOLID );
		}

		if ( tr.startsolid ) {
			G_Printf ("FinishSpawningItem: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin));
			G_FreeEntity( ent );
			return;
		}

		// allow to ride movers
		ent->s.groundEntityNum = tr.entityNum;

		G_SetOrigin( ent, tr.endpos );
	}

	if (ent->spawnflags & 2) {		// spin

		ent->s.eFlags |= EF_SPINNING;
	}


	// team slaves and targeted items aren't present at start
	if ( ( ent->flags & FL_TEAMSLAVE ) || ent->targetname ) {
		ent->flags |= FL_NODRAW;
		//ent->s.eFlags |= EF_NODRAW;
		ent->r.contents = 0;
		return;
	}

	// health/ammo can potentially be multi-stage (multiple use)
	if( ent->item->giType == IT_HEALTH || ent->item->giType == IT_AMMO ) {
		int i;

		// having alternate models defined in bg_misc.c for a health or ammo item specify it as "multi-stage"
    // TTimo left-hand operand of comma expression has no effect
		// initial line: for(i=0;i<4,ent->item->world_model[i];i++) {}
		for(i=0; i<MAX_ITEM_MODELS && ent->item->world_model[i] ;i++) ;

		ent->s.density = i-1;	// store number of stages in 'density' for client (most will have '1')
	}

	trap_LinkEntity (ent);
}


/*
============
G_SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/
void G_SpawnItem (gentity_t *ent, gitem_t *item) {
	char	*noise;

	G_SpawnFloat( "random", "0", &ent->random );
	G_SpawnFloat( "wait", "0", &ent->wait );

	ent->item = item;
	// some movers spawn on the second frame, so delay item
	// spawns until the third frame so they can ride trains
	ent->nextthink = level.time + FRAMETIME * 2;
	ent->think = FinishSpawningItem;

	if(G_SpawnString("noise", 0, &noise))
		ent->noise_index = G_SoundIndex(noise);

	ent->physicsBounce = 0.50;		// items are bouncy
	ent->physicsSlide = qtrue;
	ent->physicsFlush = qtrue;

	if(ent->model) {
		ent->s.modelindex2 = G_ModelIndex(ent->model);
	}

	if ( item->giType == IT_TEAM ) {
		G_SpawnInt( "count", "1", &ent->s.density );
		G_SpawnInt( "speedscale", "100", &ent->splashDamage );
		if( !ent->splashDamage ) {
			ent->splashDamage = 100;
		}
	}
}

/*
================
G_BounceItem

================
*/

void G_FlushItem( gentity_t *ent, trace_t *trace )
{
	vec3_t  forward, start, end;
	trace_t tr;
	vec3_t outAxis[ 3 ];
	float s;

	if ( g_flushItems.integer &&
		ent->physicsFlush &&
		trace->plane.normal[2] > 0.7f &&
		( trace->plane.normal[0] != 0.0f || trace->plane.normal[1] != 0.0f  || trace->plane.normal[2] != 1.0f ) ) { // no need to adjust flat ground, make it faster
		AngleVectors( ent->r.currentAngles, forward, NULL, NULL );
		VectorCopy( trace->plane.normal, outAxis[ 2 ] );
		ProjectPointOnPlane( outAxis[ 0 ], forward, outAxis[ 2 ] );

		if( !VectorNormalize( outAxis[ 0 ] ) )
		{
			AngleVectors( ent->r.currentAngles, NULL, NULL, forward );
			ProjectPointOnPlane( outAxis[ 0 ], forward, outAxis[ 2 ] );
			VectorNormalize( outAxis[ 0 ] );
		}

		CrossProduct( outAxis[ 0 ], outAxis[ 2 ], outAxis[ 1 ] );
		outAxis[ 1 ][ 0 ] = -outAxis[ 1 ][ 0 ];
		outAxis[ 1 ][ 1 ] = -outAxis[ 1 ][ 1 ];
		outAxis[ 1 ][ 2 ] = -outAxis[ 1 ][ 2 ];

		AxisToAngles( outAxis, ent->r.currentAngles );
		VectorMA( trace->endpos, -64.0f, trace->plane.normal, end );
		VectorMA( trace->endpos, 1.0f, trace->plane.normal, start );

		trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, MASK_SOLID );

		if ( !tr.startsolid ) {
			s = tr.fraction * -64;
			VectorMA( trace->endpos, s, trace->plane.normal, trace->endpos );
		}

		// make sure it is off ground
		VectorMA( trace->endpos, 1.0f, trace->plane.normal, trace->endpos );

	}
	else {
		trace->endpos[2] += 1.0;

		if ( ent->physicsFlush )
			ent->r.currentAngles[0] = ent->r.currentAngles[2] = 0;
	}

	G_SetAngle( ent, ent->r.currentAngles);
	SnapVector( trace->endpos );
	G_SetOrigin( ent, trace->endpos );
	ent->s.groundEntityNum = trace->entityNum;

	if ( ent->s.groundEntityNum != ENTITYNUM_WORLD )
		ent->s.pos.trType = TR_GRAVITY_PAUSED; // jaquboss, so items will fall down when needed

}

qboolean G_ItemStick( gentity_t *ent, trace_t *trace, vec3_t velocity )
{
	float	dot;

	// check if it is knife
	if ( !ent->damage )
		return qfalse;

	if ( ent->s.weapon != WP_KNIFE && ent->s.weapon != WP_KNIFE_KABAR )
		return qfalse;

	// reset flushing
	ent->physicsFlush = qtrue;

	// get a direction
	VectorNormalize( velocity );
	dot = DotProduct( velocity, trace->plane.normal );

	// do not lodge
	if ( dot > -0.75 )
		return qfalse;

	if ( trace->surfaceFlags & (SURF_GRASS|SURF_SNOW|SURF_WOOD|SURF_GRAVEL) ){

		vectoangles( velocity, ent->r.currentAngles );
		ent->physicsFlush = qfalse;
		return qtrue;
	}

	return qfalse;
}

void G_BounceItem( gentity_t *ent, trace_t *trace ) {
	vec3_t   velocity;
	float	dot;
	int      hitTime;

	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;

	// reflect the velocity on the trace plane
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity, qfalse, ent->s.effect2Time );
	dot = -2 * DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, dot, trace->plane.normal, ent->s.pos.trDelta );


	// bounce or just slide? - check if stuck or surface not too step
	if ( trace->plane.normal[2] >= 0.7 || VectorLength( ent->s.pos.trDelta ) < 16 || !ent->physicsSlide ) {
		// cut the velocity to keep from bouncing forever
		VectorScale( ent->s.pos.trDelta, ent->physicsBounce, ent->s.pos.trDelta );

		// do a bounce
		if ( ent->item && ( ent->item->giTag == WP_KNIFE || ent->item->giTag == WP_KNIFE_KABAR ))
		{
			G_AddEvent( ent, EV_GRENADE_BOUNCE, BG_FootstepForSurface( trace->surfaceFlags ) );
		}
		else if ( ent->item && ( ent->item->giType == IT_WEAPON || ent->item->giType == IT_AMMO ) && ent->item->giTag > WP_NONE && ent->item->giTag < WP_NUM_WEAPONS)
		{
			G_AddEvent( ent, EV_BOUNCE_SOUND, ent->item->giType == IT_WEAPON ? 0 : 1 );
			ent->s.weapon = ent->item->giTag;
		}

		// check for stop
		if ( G_ItemStick( ent, trace, velocity ) || ( VectorLength(ent->s.pos.trDelta) < 40 && trace->plane.normal[2] > 0) )
		{
			G_FlushItem( ent, trace );
			return;
		}

		// bounce the angles
		if ( ent->s.apos.trType != TR_STATIONARY )
		{
			VectorScale( ent->s.apos.trDelta, ent->physicsBounce, ent->s.apos.trDelta );
			ent->s.apos.trTime = level.time;
		}


	} else if ( ent->physicsSlide ) {
		BG_ClipVelocity( ent->s.pos.trDelta, trace->plane.normal, ent->s.pos.trDelta, OVERCLIP );
	}

	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	ent->s.pos.trTime = level.time;
	VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
}

/*
=================
G_RunItemProp
=================
*/

void G_RunItemProp (gentity_t *ent, vec3_t origin)
{
	gentity_t	*traceEnt;
	trace_t		trace;
	gentity_t	*owner;
	vec3_t		end;

	owner = &g_entities[ent->r.ownerNum];

	VectorCopy (origin, end);
	end[2] += 1;

	trap_Trace( &trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, end,
				ent->r.ownerNum, MASK_SHOT );

	traceEnt = &g_entities[ trace.entityNum ];

	if (traceEnt && traceEnt->takedamage && traceEnt != ent)
		ent->enemy = traceEnt;

	if (owner->client && trace.startsolid && traceEnt != owner && traceEnt != ent ){
		ent->takedamage = qfalse;
		ent->die (ent, ent, NULL, 10, 0);
		Prop_Break_Sound (ent);

		return;
	}
	else if ( trace.surfaceFlags & SURF_NOIMPACT) {
		ent->takedamage = qfalse;
		Props_Chair_Skyboxtouch (ent);

		return;
	}
}

void G_TestWater (gentity_t *ent, vec3_t origin)
{
	trace_t		trace;

	trap_Trace( &trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin,
				ent->r.ownerNum, MASK_WATER );


	if ( trace.fraction == 1 && !trace.startsolid && !(trap_PointContents( ent->r.currentOrigin, -1 ) & MASK_WATER ) ){
		if ( ent->s.pos.trType != TR_GRAVITY) {
			ent->s.pos.trTime = level.time;
			ent->s.pos.trType = TR_GRAVITY;
			VectorCopy( origin, ent->s.pos.trBase);
		}
		return;
	}

	if ( ent->s.pos.trType != TR_GRAVITY_FLOAT ) {
		ent->s.pos.trTime = level.time;
		ent->s.pos.trType = TR_GRAVITY_FLOAT;
		VectorCopy( trace.endpos, ent->s.pos.trBase);

		VectorScale( ent->s.pos.trDelta, 0.125f, ent->s.pos.trDelta );
		VectorScale( ent->s.apos.trDelta, 0.125f, ent->s.apos.trDelta );
	}

}

#define TYPE_HEALTH 1
#define TYPE_AMMO 2

qboolean G_CheckForCabinetResupply( gentity_t *ent, gentity_t *traceEnt )
{
	int i;
	int	type;

	if ( !traceEnt )
		return qfalse;

	if ( !ent->item )
		return qfalse;

	if ( !ent->parent || !ent->parent->client )
		return qfalse;

	if ( ent->item->giType == IT_HEALTH )
		type = TYPE_HEALTH;
	else if ( ent->item->giTag == WP_AMMO )
		type = TYPE_AMMO;
	else
		return qfalse;

	switch ( type ){
		case TYPE_HEALTH:
			if ( traceEnt->s.eType != ET_CABINET_H || !( ent->parent->client->sess.skillBits[SK_MEDIC] & (1<<7)) )
				return qfalse;
			else
				break;
		case TYPE_AMMO:
			if ( traceEnt->s.eType != ET_CABINET_A || !( ent->parent->client->sess.skillBits[SK_FIELDOPS] & (1<<7))  )
				return qfalse;
			else
				break;
		default:
			return qfalse;
	}

	// now take a look which trigger is our master
	for (i=0;i<level.num_entities;i++) {
		// here is it
		if ( g_entities[i].target_ent && g_entities[i].target_ent->s.number  == traceEnt->s.number ) {
			 // add it if this is not unlimited cabinet
			if ( ( g_entities[i].count != -9999 ) && ( g_entities[i].health < g_entities[i].count ) ) {

				if ( type == TYPE_HEALTH ) {
					g_entities[i].health += ent->item->quantity;
					if(g_entities[i].health > g_entities[i].count)
						g_entities[i].health = g_entities[i].count;
				}
				else {
					g_entities[i].health += ent->count;
					if(g_entities[i].health > g_entities[i].count)
						g_entities[i].health = g_entities[i].count;
				}
				// remove it
				G_FreeEntity( ent );
				return qtrue;
			}
			// ok end cycle anyway
			break;
		}
	}
	// done
	return qfalse;
}

/*
================
G_RunItem

================
*/
void G_RunItem( gentity_t *ent ) {
	vec3_t		origin;
	trace_t		tr;
	int			contents;
	int			mask;

#ifdef PROFILER
	BG_ProfilerBlockStartTimer(PROFILER_G_RunItem, qfalse);
#endif
	if ( ent->clipmask )
		mask = ent->clipmask;
	else
		mask = MASK_SOLID;

	// if groundentity has been set to -1, it may have been pushed off an edge
	if ( ent->s.groundEntityNum == -1  ) {
		if ( ent->s.pos.trType != TR_GRAVITY ) {
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
		}
	}

    if ( ent->s.pos.trType == TR_STATIONARY || ent->s.pos.trType == TR_GRAVITY_PAUSED) { //----(SA)
		// trace a line from previous position down, to see if I should start falling again
		if ( ent->s.pos.trType == TR_GRAVITY_PAUSED ) {
			vec3_t	newOrigin;

			VectorCopy(ent->r.currentOrigin, newOrigin);
			newOrigin [2] -= 8;
			trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, newOrigin, ent->s.number, mask );

			if(tr.fraction > 0.5 && !tr.startsolid)	{ // it's clear, start moving again
				VectorClear(ent->s.pos.trDelta);
				ent->s.pos.trType = TR_GRAVITY;	// nothing below me, start falling again
				ent->s.pos.trTime = level.time;
			}
		}

        G_RunThink( ent );
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
        return;
    }

    if ( ent->s.pos.trType == TR_LINEAR && ( !ent->clipmask && !ent->r.contents ) ) {
        // check think function
        G_RunThink( ent );
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
        return;
    }

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin, qfalse, ent->s.effect2Time );

	G_TestWater( ent, origin );

	// jaquboss
	// need to ignore player and self
	if ( ent->damage && ent->parent) {
		ent->parent->r.linked = qfalse;
		// ignore self
		trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin,
			ent->s.number, mask );
		ent->parent->r.linked = qtrue;
	}
	else {
		// trace a line from the previous position to the current position
		trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin,
			ent->r.ownerNum, mask );
	}

	if (ent->isProp && ent->takedamage)
		G_RunItemProp (ent, origin);

	VectorCopy( tr.endpos, ent->r.currentOrigin );

	if ( tr.startsolid )
		tr.fraction = 0;

	trap_LinkEntity( ent );	// FIXME: avoid this for stationary?

	// check think function
	G_RunThink( ent );

	if ( tr.fraction == 1 )
	{
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
		return;
	}

	if (G_CheckForCabinetResupply(ent, &g_entities[tr.entityNum]))
	{
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
		return;
	}

	// if it is in a nodrop volume, remove it
	contents = trap_PointContents( ent->r.currentOrigin, -1 );
	if ( contents & CONTENTS_NODROP ) {
		if (ent->item && ent->item->giType == IT_TEAM)
			Team_ReturnFlag(ent, NULL); //core: no player who did it
		else
			G_FreeEntity( ent );
#ifdef PROFILER
		BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
		return;
	}

	if ( ent->damage && tr.entityNum != ENTITYNUM_NONE ) {
		float	speed;
		vec3_t	delta, dir;
		int      hitTime;

		hitTime = level.previousTime + ( level.time - level.previousTime ) * tr.fraction;

		BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, delta, qfalse, ent->s.effect2Time );
		VectorCopy ( delta, dir );
		VectorNormalize( dir );
		speed = VectorLength( delta );

		if ( speed > 300 ){
			gentity_t		*temp, *traceEnt;

			traceEnt = &g_entities[tr.entityNum];

			// do damage
			// note that onSameTeam looks up for client so we dont have to worry about nonclients crashing/not being damaged
			if ( traceEnt->takedamage && ( G_FRIENDLYFIRE || !OnSameTeam(ent->parent, traceEnt) )){
				int	damage = ent->damage + ((speed-300)/20);

				G_Damage( traceEnt, ent, ent->parent, dir, tr.endpos, damage, 0, ent->methodOfDeath );
			}

			// do impact
			if ( traceEnt->client && traceEnt->takedamage ) {
				temp = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
				temp->s.otherEntityNum = traceEnt->s.number;
				temp->s.weapon = ent->s.weapon;
				temp->s.clientNum = ent->r.ownerNum;
			}

		}

	}

	G_BounceItem( ent, &tr );
#ifdef PROFILER
	BG_ProfilerBlockStopTimer(PROFILER_G_RunItem);
#endif
}
