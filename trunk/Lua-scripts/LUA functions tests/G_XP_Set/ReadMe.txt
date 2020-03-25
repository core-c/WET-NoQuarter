This LUA script is to test the new function "et.G_XP_Set".

It will add a new server console command called "luaxpset".

The usage of this command is easy:

luaxpset clientNum xp skill add

clientNum	=	the client to change XP
xp		=	the amount of xp
skill		=	the skill where to add the xp
add		=	a boolean value: 1 will add the xp to the amount the player already
			has, 0 will set the xp for the specified skill to the specified
			value

The possible values for the skill are:

SK_BATTLE_SENSE					=	0
SK_EXPLOSIVES_AND_CONSTRUCTION			=	1
SK_FIRST_AID					=	2
SK_SIGNALS					=	3
SK_LIGHT_WEAPONS				=	4
SK_HEAVY_WEAPONS				=	5
SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS	=	6

NOTE:

To know the XP values for each you can read entity array:

sess.skillpoints[skill]

where "skill" is a number from the list just up to this last lines

LUA EXAMPLES

et.G_XP_Set( 10, 100, 4, 0 )

This will set the xp points of client "10" to "100" in the light weapons skill

et.G_XP_Set( 2, 23, 0, 1 )

This will increase by "23" points the xp of client "2" in the battle sense skill