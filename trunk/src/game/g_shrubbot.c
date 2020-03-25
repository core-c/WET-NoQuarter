/*
 * name:g_shrubbot.c
 *
 * desc:	This code is the original work of Tony J. White
 *
 * 			The functionality of this code mimics the behaviour of the currently
 * 			inactive project shrubmod (http://www.etstats.com/shrubet/index.php?ver=2)
 * 			by Ryan Mannion.   However, shrubmod was a closed-source project and
 * 			none of its code has been copied, only its functionality.
 *
 * 			You are free to use this implementation of shrubbot in your
 * 			own mod project if you wish.
 *
 * NQQS:	HACK!
 * 			IRATA: Test & add code 'unknown' and 'NO_GUID' GUID case, this was never used w/o PB (R.I.P)
 *          ...fu! ... this is not trivial ... bans/mute/kick are based on that shit
 *          ONE UNKNOWN GUID FITS ALL :/
 *
 */
#include "g_local.h"
#include "../../etmain/ui/menudef.h"

// IlDuca - patheticcockroach's fix for !nade overflow bug
int lol_flood_control = 0;

char *G_Shortcuts(gentity_t *ent, char *text);

extern char bigTextBuffer[100000];

// Note flag 'J' is free now - !ammopack & !medpack IS removed
static const struct g_shrubbot_cmd g_shrubbot_cmds[] =
{
	{"readconfig",	G_shrubbot_readconfig,	'G', 0,				"reloads the shrubbot config file and refreshes user flags", ""},
	{"time",		G_shrubbot_time,		'C', 0,				"displays the local time", "[^3name|slot#^9] (^hreason^9)"},
	{"setlevel",	G_shrubbot_setlevel,	's', 0,				"sets the admin level of a player", "[^3name|slot#^9] [^3level^9)"},
	{"kick",		G_shrubbot_kick,		'k', 0,				"kick a player with an optional reason", "^9(^hcommand^9)"},
	{"ban",			G_shrubbot_ban,			'b', 0,				"ban a player by NAME/slot with an optional expiration time (seconds) and reason", "[^3name|slot#^9] (^htime^9) (^hreason^9)"},
	{"banguid",		G_shrubbot_banguid,		'b', 0,				"ban a (perhaps not ingame) player by GUID with an optional expiration time (seconds) and reason", "[^3GUID^9] (^htime^9) (^hreason^9)"},
	{"banip",		G_shrubbot_banIP,		'b', 0,				"ban a (perhaps not ingame) player by IP with an optional expiration time (seconds) and reason", "[^3IP^9] (^htime^9) (^hreason^9)"},
	{"unban",		G_shrubbot_unban,		'b', 0,				"unbans a player specified ban number as seen in ^2!showbans^9", "[^3ban slot#^9]"},
	{"put",			G_shrubbot_putteam,		'p', 0,				"move a player to a specified team", "[^3name|slot#^9] [^3r|b|s^9]"},
	{"pause",		G_shrubbot_pause,		'Z', 0,				"pauses the game for all players", ""},
	{"unpause",		G_shrubbot_unpause,		'Z', 0,				"unpauses the game if it had been paused by ^2!pause^9", ""},
	{"listplayers",	G_shrubbot_listplayers,	'i', 0,				"display a list of connected players, their slot numbers as well as their admin levels",""},
	{"listteams",	G_shrubbot_listteams,	'I', 0, 			"displays info about the teams", ""},
	{"mute",		G_shrubbot_mute,		'm', 0,				"mutes a player", "[^3name|slot#^9]"},
	{"unmute",		G_shrubbot_unmute,		'm', 0,				"unmutes a muted player", "[^3name|slot#^9]"},
	{"showbans",	G_shrubbot_showbans,	'B', 0,				"display a (partial) list of active bans", "(^hstart at ban#^9) ((^hbanner^9) (^3banner's name^9)) ((^3find^9) (^hbanned player^9)) ((^3reason^9) (^hreason for ban^9))"},
	{"help",		G_shrubbot_help,		'h', 0,				"display commands available to you or help on a specific command", "^9(^hcommand^9)"},
	{"admintest",	G_shrubbot_admintest,	'a', 0,				"display your current admin level", ""},
	{"cancelvote",	G_shrubbot_cancelvote,	'c', 0,				"cancels the vote currently taking place", ""},
	{"passvote",	G_shrubbot_passvote,	'V', 0,				"passes the vote currently taking place", ""},
	{"spec999",		G_shrubbot_spec999,		'P', 0,				"moves 500-999 pingers to the spectator team", ""},
	{"shuffle",		G_shrubbot_shuffle,		'S', 0,				"shuffle the teams to try and even them", ""},
	{"rename",		G_shrubbot_rename,		'N', SCMDF_TYRANNY, "renames a player", "[^3name|slot#^9] [^3new name^9]"},
	{"gib",			G_shrubbot_gib,			'g', SCMDF_TYRANNY, "insantly gibs a player", "(^3name|slot#^9) (^hreason^9)"},
	{"slap",		G_shrubbot_slap,		'A', SCMDF_TYRANNY, "give a player a specified amount of damage for a specified reason", "[^3name|slot#^9] (^hdamage^9) (^hreason^9)"},
	{"burn",		G_shrubbot_burn,		'U', SCMDF_TYRANNY, "burns a player taking some of his health", "[^3name|slot#^9] (^hreason^9)"},
	{"warn",		G_shrubbot_warn,		'R', 0,				"warns a player by displaying the reason", "[^3name|slot#^9] [^3reason^9]"},
	//{"news",		G_shrubbot_news,		'W', 0,				"play the map's news reel or another map's news reel if specified", "^9(^hmapname^9)"},
	{"lock",		G_shrubbot_lock,		'K', 0,				"lock one or all of the teams from players joining", "[^3r|b|s|all^7]"},
	{"unlock",		G_shrubbot_unlock,		'K', 0,				"unlock one or all locked teams", "^9[^3r|b|s|all^9]"},
	{"nade",		G_shrubbot_lol,			'x', SCMDF_TYRANNY, "makes a player drop a bunch of grenades or all players", "^9[^3name|slot#^9]"},
	{"pip",			G_shrubbot_pip,			'z', SCMDF_TYRANNY, "show sparks around a player or all players", "^9[^3name|slot#^9]"},
	{"pop",			G_shrubbot_pop,			'z', SCMDF_TYRANNY, "pops the helmets of a player or all players", "^9[^3name|slot#^9]"},
	{"restart",		G_shrubbot_reset,		'r', 0,				"restarts the current map", ""},
	{"reset",		G_shrubbot_reset,		'r', 0,				"resets the current match", ""},
	{"fling",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "flings a player in a random direction", "^9[^3name|slot#^9]"},
	{"throw",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "throws a player forward", "^9[^3name|slot#^9]"},
	{"launch",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "launch a player vertically", "^9[^3name|slot#^9]"},
	{"disorient",	G_shrubbot_disorient,	'd', SCMDF_TYRANNY, "flips a player's view", "^9[^3name|slot#^9] (^hreason^9)"},
	{"orient",		G_shrubbot_orient,		'd', SCMDF_TYRANNY, "unflips a disoriented player's view", "^9[^3name|slot#^9]"},
	{"resetxp",		G_shrubbot_resetxp,		'X', SCMDF_TYRANNY, "reset a player's XP", "^9[^3name|slot#^9] (^hreason^9)"},
	{"resetmyxp",   G_shrubbot_resetmyxp,   'M', 0,				"resets your own XP", ""},
	{"nextmap",     G_shrubbot_nextmap,     'n', 0,				"loads the next map", ""},
	{"swap",        G_shrubbot_swap,        'w', 0,				"swap teams", ""},

	// jet Pilot - new shrubbot commands
	{"revive",		G_shrubbot_revive,		'v', SCMDF_CHEAT, 	"revives a dead player", "^9[^3name|slot#^9] (^hreason^9)"},
#ifdef __CLIENTCMD__
	{"clientcmd",	G_shrubbot_clientcmd,	'j', SCMDF_TYRANNY, "can force a player to perform some commands", "^9[^3name|slot#^9] [^3command^9]"},
#endif
	{"rocket",		G_shrubbot_rocket,		'j', SCMDF_TYRANNY, "have a rocket shoot from the player who uses this command", ""},
	{"disguise",	G_shrubbot_disguise,	'T', SCMDF_CHEAT, 	"gives a player a disguise of the specified class", "^9[^3name|slot#^9] (^hclass number^9)"},
	{"poison",		G_shrubbot_poison,		'U', SCMDF_TYRANNY, "poisons a player", "^9(^3name|slot#^9) (^hreason^9)"},
	{"pants",		G_shrubbot_pants,		't', SCMDF_TYRANNY, "removes a player's pants!", "^9(^3name|slot#^9)"},
	{"give",		G_shrubbot_give,		'e', SCMDF_CHEAT, 	"gives a player something...", "^9[^3name|slot#^9] item/thing"},

	{"dw",			G_shrubbot_dropweapon,	'D', SCMDF_TYRANNY, "drops a player's primary and secondary weapons!", "^9(^3name|slot#^9)"},
	// Jaybird
	{"finger",		G_shrubbot_finger,		'f', 0,				"gives specific information about a player", "^9[^3name|slot#^9]"},
	{"uptime",		G_shrubbot_uptime,		'u', 0,				"displays server uptime", "" },
	{"glow",		G_shrubbot_glow,		'o', SCMDF_TYRANNY, "makes player(s) glow", "^9(^3name|slot#^9)"},
	{"freeze",		G_shrubbot_freeze,		'E', SCMDF_TYRANNY, "freezes player(s) move", "^9(^3name|slot#^9)"},
	{"unfreeze",	G_shrubbot_unfreeze,	'E', SCMDF_TYRANNY, "makes player(s) moving again", "^9(^3name|slot#^9)"},
	{"weapons",		G_shrubbot_weapons,		'h', 0, 			"displays a list of disabled or restricted weapons"},
	{"",			NULL,					'\0', 0}
};

g_shrubbot_level_t		*g_shrubbot_levels[MAX_SHRUBBOT_LEVELS];
g_shrubbot_admin_t		*g_shrubbot_admins[MAX_SHRUBBOT_ADMINS];
g_shrubbot_ban_t		*g_shrubbot_bans[MAX_SHRUBBOT_BANS];
g_shrubbot_ban_t		*g_shrubbot_mutes[MAX_SHRUBBOT_BANS]; // jaquboss
g_shrubbot_command_t	*g_shrubbot_commands[MAX_SHRUBBOT_COMMANDS];

qboolean G_shrubbot_permission(gentity_t *ent, char flag) {
	int i;
	int l = 0;
	char *guid;
	char *flags;

	// console always wins
	if(!ent) {
		return qtrue;
	}

	guid = level.clients[ent-g_entities].pers.cl_guid;

	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			flags = g_shrubbot_admins[i]->flags;
			while(*flags) {
				if(*flags == flag)
					return qtrue;
				else if(*flags == '-') {
					while(*flags++) {
						if(*flags == flag)
							return qfalse;
						else if(*flags == '+')
							break;
					}
				}
				else if(*flags == '*') {
					while(*flags++) {
						if(*flags == flag)
							return qfalse;
					}
					// tjw: flags for individual admins
					switch(flag) {
					case SBF_IMMUTABLE:
					case SBF_INCOGNITO:
						return qfalse;
					default:
						return qtrue;
					}
				}
				*flags++;
			}
			l = g_shrubbot_admins[i]->level;
		}
	}
	for(i=0; g_shrubbot_levels[i]; i++) {
		if(g_shrubbot_levels[i]->level == l) {
			flags = g_shrubbot_levels[i]->flags;
			while(*flags) {
				if(*flags == flag)
					return qtrue;
				if(*flags == '*') {
					while(*flags++) {
						if(*flags == flag)
							return qfalse;
					}
					// tjw: flags for individual admins
					switch(flag) {
					case SBF_IMMUTABLE:
					case SBF_INCOGNITO:
						return qfalse;
					default:
						return qtrue;
					}
				}
				*flags++;
			}
		}
	}
	return qfalse;
}

qboolean _shrubbot_admin_higher(gentity_t *admin, gentity_t *victim) {
	int i;
	int alevel = 0;
	char *guid;

	// console always wins
	if(!admin) return qtrue;
	// just in case
	if(!victim) return qtrue;

	guid = level.clients[admin-g_entities].pers.cl_guid;
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			alevel = g_shrubbot_admins[i]->level;
		}
	}
	guid = level.clients[victim-g_entities].pers.cl_guid;
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			if(alevel < g_shrubbot_admins[i]->level)
				return qfalse;
		}
	}
	return qtrue;
}

void G_shrubbot_writeconfig_string(char *s, fileHandle_t f) {
	char buf[MAX_STRING_CHARS];

	buf[0] = '\0';
	if(s[0]) {
		Q_strncpyz(buf, s, sizeof(buf));
		trap_FS_Write(buf, strlen(buf), f);
	}
	trap_FS_Write("\n", 1, f);
}

void G_shrubbot_writeconfig_int(int v, fileHandle_t f) {
	char buf[32];

	Com_sprintf(buf, 32, "%d", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}

/* IRATA: unused
void G_shrubbot_writeconfig_float(float v, fileHandle_t f)
{
	char buf[32];

	Com_sprintf(buf, 32, "%f", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}
*/


// Note: since 1.3.0 levels are not permitted if a user has 'NO_GUID'
// TODO: don't write unknown guid entries !
void _shrubbot_writeconfig() {
	fileHandle_t f;
	int len, i, j;
	time_t t;
	char levels[MAX_STRING_CHARS] = {""};

	if (nq_noq.integer & NOQ_DONT_WRITE) return;
	if(!g_shrubbot.string[0]) return;

	time(&t);
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_WRITE);
	if(len < 0) {
		G_Printf("_shrubbot_writeconfig: could not open %s\n", g_shrubbot.string);
	}
	for(i=0; g_shrubbot_levels[i]; i++) {
		trap_FS_Write("[level]\n", 8, f);
		trap_FS_Write("level   = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_levels[i]->level, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_levels[i]->name, f);
		trap_FS_Write("flags   = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_levels[i]->flags, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_admins[i]; i++) {
		// don't write level 0 users
		if(g_shrubbot_admins[i]->level < 1) continue;
		trap_FS_Write("[admin]\n", 8, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->name, f);
		trap_FS_Write("guid    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->guid, f);
		trap_FS_Write("level   = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_admins[i]->level, f);
		trap_FS_Write("flags   = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->flags, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_bans[i]; i++) {
		// don't write expired bans
		// if expires is 0, then it's a perm ban
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1) continue;

		trap_FS_Write("[ban]\n", 6, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->name, f);
		trap_FS_Write("guid    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->guid, f);
#ifdef HW_BAN
		trap_FS_Write("hwguid  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->hwguid, f);
#endif // HW_BAN
		trap_FS_Write("ip      = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->ip, f);
		trap_FS_Write("reason  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->reason, f);
		trap_FS_Write("made    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->made, f);
		trap_FS_Write("expires = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_bans[i]->expires, f);
		trap_FS_Write("banner  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->banner, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_mutes[i]; i++) {
		if(g_shrubbot_mutes[i]->expires != 0 &&
			(g_shrubbot_mutes[i]->expires - t) < 1) continue;

		trap_FS_Write("[mute]\n", 7, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->name, f);
		trap_FS_Write("guid    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->guid, f);
		trap_FS_Write("ip      = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->ip, f);
		trap_FS_Write("reason  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->reason, f);
		trap_FS_Write("made    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->made, f);
		trap_FS_Write("expires = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_mutes[i]->expires, f);
		trap_FS_Write("muter   = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_mutes[i]->banner, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_commands[i]; i++) {
			levels[0] = '\0';
			trap_FS_Write("[command]\n", 10, f);
			trap_FS_Write("command = ", 10, f);
			G_shrubbot_writeconfig_string(g_shrubbot_commands[i]->command, f);
			trap_FS_Write("exec    = ", 10, f);
			G_shrubbot_writeconfig_string(g_shrubbot_commands[i]->exec, f);
			trap_FS_Write("desc    = ", 10, f);
			G_shrubbot_writeconfig_string(g_shrubbot_commands[i]->desc, f);
			trap_FS_Write("levels  = ", 10, f);
			for(j=0; g_shrubbot_commands[i]->levels[j] != -1; j++) {
			Q_strcat(levels, sizeof(levels),
			va("%i ", g_shrubbot_commands[i]->levels[j]));
			}
		G_shrubbot_writeconfig_string(levels, f);
		trap_FS_Write("\n", 1, f);
	}
	trap_FS_FCloseFile(f);
}

void G_shrubbot_readconfig_string(char **cnf, char *s, int size) {
	char *t = COM_ParseExt(cnf, qfalse);

	if(!strcmp(t, "=")) {
			t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("^dreadconfig: ^9warning missing = before "
				"\"%s\" on line %d\n",
				t,
				COM_GetCurrentParseLine());
	}
	s[0] = '\0';
	while(t[0]) {
		if((s[0] == '\0' && strlen(t) <= size) || (strlen(t)+strlen(s) < size)) {
			Q_strcat(s, size, t);
			Q_strcat(s, size, " ");
		}
		t = COM_ParseExt(cnf, qfalse);
	}
	// trim the trailing space
	if(strlen(s) > 0 && s[strlen(s)-1] == ' ')
		s[strlen(s)-1] = '\0';
}

void G_shrubbot_readconfig_int(char **cnf, int *v) {
	char *t = COM_ParseExt(cnf, qfalse);

	if(!strcmp(t, "=")) {
		t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("^dreadconfig: ^9warning missing = before "
				"\"%s\" on line %d\n",
				t,
				COM_GetCurrentParseLine());
	}
	*v = atoi(t);
}

void G_shrubbot_readconfig_float(char **cnf, float *v) {
	char *t = COM_ParseExt(cnf, qfalse);
	if(!strcmp(t, "=")) {
		t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("readconfig: warning missing = before "
				"\"%s\" on line %d\n",
				t,
				COM_GetCurrentParseLine());
	}
	*v = atof(t);
}


/*
  if we can't parse any levels from readconfig, set up default
  ones to make new installs easier for admins
*/
void _shrubbot_default_levels() {
	g_shrubbot_level_t *l;
	int i;

	for(i=0; g_shrubbot_levels[i]; i++) {
		free(g_shrubbot_levels[i]);
		g_shrubbot_levels[i] = NULL;
	}
	for(i=0; i<=5; i++) {
		l = malloc(sizeof(g_shrubbot_level_t));
		l->level = i;
		*l->name = '\0';
		*l->flags = '\0';
		g_shrubbot_levels[i] = l;
	}
	Q_strcat(g_shrubbot_levels[0]->flags, 6, "iahC");
	Q_strcat(g_shrubbot_levels[1]->flags, 6, "iahCp");
	Q_strcat(g_shrubbot_levels[2]->flags, 7, "iahCpP");
	Q_strcat(g_shrubbot_levels[3]->flags, 10, "i1ahCpPkm");
	Q_strcat(g_shrubbot_levels[4]->flags, 12, "i1ahCpPkmBb");
	Q_strcat(g_shrubbot_levels[5]->flags, 13, "i1ahCpPkmBbs");
}

/*
	return a level for a player entity.
*/
int G_shrubbot_level(gentity_t *ent) {
	int i;
	char *guid;

	if(!ent) {
		// forty - we are on the console, return something high for now.
		return MAX_SHRUBBOT_LEVELS;
	}

	guid = level.clients[ent-g_entities].pers.cl_guid;

	// note: if there is no guid, the level should be 0

	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			return g_shrubbot_admins[i]->level;
		}
	}
	// if looped through all admins and none fits:
	return 0;
}

/*
	_shrubbot_command_permission

	 This function needs MAJOR attention!


	Custom shrub commands that people can create. These are the [command] sections in the shrub config file.
	These custom command names are defined in the "command =" line.
	When executed, these custom commands will execute all that's defined in the "exec =" line.
	The "exec" line can contain one or more commands, which could also be shrub-commands, like "gib"..
	Before a custom command is really executed, check if the user has permission to execute the "exec" commands at all.
	If the user has the permission to execute the [command], but no permission to execute the [exec]-command(s),
	then he will have no permission, and this funcion returns qfalse..
*/
static qboolean _shrubbot_command_permission(gentity_t *ent, char *command) {
	int i, j;
	int level = G_shrubbot_level(ent);

	if (!ent) {
		return qtrue;
	}
	for (i=0; g_shrubbot_commands[i]; i++) {
		if (!Q_stricmp(command, g_shrubbot_commands[i]->command)) {
			for (j = 0; g_shrubbot_commands[i]->levels[j] != -1; j++) {
				if (g_shrubbot_commands[i]->levels[j] == level) {

					/*
					// core: The level is OK, now check the level of the command that is really executed.. (the one in the "exec = " line)
					// If that command is a native shrub-command, then the permissions of that command must be applied, finally..
					if ( !G_shrubbot_native_cmd_check(ent,command,0) ) {
						return qfalse;
					}
					*/

					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

void _shrubbot_log(gentity_t *admin, char *cmd, int skiparg) {
	fileHandle_t f;
	int len;
	char string[MAX_STRING_CHARS];
	int	min, tens, sec;
	g_shrubbot_admin_t *a;
	g_shrubbot_level_t *l;
	char flags[MAX_SHRUBBOT_FLAGS*2];
	gentity_t *victim = NULL;
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH];

	if(!g_logAdmin.string[0]) return;

	len = trap_FS_FOpenFile(g_logAdmin.string, &f, FS_APPEND);
	if(len < 0) {
		G_Printf("_shrubbot_log: error could not open %s\n",
			g_logAdmin.string);
		return;
	}

	sec = level.time / SECONDS_1;
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	*flags = '\0';
	if(admin) {
		int i,j;
		for(i=0; g_shrubbot_admins[i]; i++) {
			if(!Q_stricmp(g_shrubbot_admins[i]->guid ,
				admin->client->pers.cl_guid)) {

				a = g_shrubbot_admins[i];
				Q_strncpyz(flags, a->flags, sizeof(flags));
				for(j=0; g_shrubbot_levels[j]; j++) {
					if(g_shrubbot_levels[j]->level == a->level) {
						l = g_shrubbot_levels[j];
						Q_strcat(flags, sizeof(flags), l->flags);
						break;
					}
				}
				break;
			}
		}
	}

	if(Q_SayArgc() > 1+skiparg) {
		Q_SayArgv(1+skiparg, name, sizeof(name));
		if(ClientNumbersFromString(name, pids) == 1) {
			victim = &g_entities[pids[0]];
		}
	}

	if(victim && Q_stricmp(cmd, "attempted")) {
		Com_sprintf(string, sizeof(string),
			"%3i:%i%i: %i: %s: %s: %s: %s: %s: %s: \"%s\"\n",
			min,
			tens,
			sec,
			(admin) ? admin->s.clientNum : -1,
			(admin) ? admin->client->pers.cl_guid
				: "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
			(admin) ? admin->client->pers.netname : "console",
			flags,
			cmd,
			victim->client->pers.cl_guid,
			victim->client->pers.netname,
			Q_SayConcatArgs(2+skiparg));
	}
	else {
		Com_sprintf(string, sizeof(string),
			"%3i:%i%i: %i: %s: %s: %s: %s: \"%s\"\n",
			min,
			tens,
			sec,
			(admin) ? admin->s.clientNum : -1,
			(admin) ? admin->client->pers.cl_guid
				: "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
			(admin) ? admin->client->pers.netname : "console",
			flags,
			cmd,
			Q_SayConcatArgs(1+skiparg));
	}
	trap_FS_Write(string, strlen(string), f);
	trap_FS_FCloseFile(f);
}

#define SECONDS_IN_YEAR		31536000
#define SECONDS_IN_MONTH	2592000
#define SECONDS_IN_WEEK		604800
#define SECONDS_IN_DAY		86400
#define SECONDS_IN_HOUR		3600
#define SECONDS_IN_MINUTE	60

void jP_GetDurationString(int secs, char *dur, int len) {
	int		 years, months, weeks, days, hours, minutes;
	qboolean addComma = qfalse;
	char temp[512];

	if (secs < 0) {
		Q_strncpyz(dur, "PERMANENT", len);
		return;
	}

	years   = secs > SECONDS_IN_YEAR   ? (secs / SECONDS_IN_YEAR)   : 0;
	if (years) secs -= SECONDS_IN_YEAR * years;
	months  = secs > SECONDS_IN_MONTH  ? (secs / SECONDS_IN_MONTH)  : 0;
	if (months) secs -= SECONDS_IN_MONTH * months;
	weeks   = secs > SECONDS_IN_WEEK   ? (secs / SECONDS_IN_WEEK)   : 0;
	if (weeks) secs -= SECONDS_IN_WEEK * weeks;
	days    = secs > SECONDS_IN_DAY    ? (secs / SECONDS_IN_DAY)    : 0;
	if (days) secs -= SECONDS_IN_DAY * days;
	hours   = secs > SECONDS_IN_HOUR   ? (secs / SECONDS_IN_HOUR)   : 0;
	if (hours) secs -= SECONDS_IN_HOUR * hours;
	minutes = secs > SECONDS_IN_MINUTE ? (secs / SECONDS_IN_MINUTE) : 0;
	if (minutes) secs -= SECONDS_IN_MINUTE * minutes;

	if (years) {
		Q_strncpyz(temp, va("%i year%s", years, years > 1 ? "s" : ""), 512);
		addComma = qtrue;
	}

	if (months) {
		Q_strcat(temp, 512, va("%s%i month%s", addComma ? (weeks > 0 ? ", " : " and ") : "", months, months > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (weeks) {
		Q_strcat(temp, 512, va("%s%i week%s", addComma ? (days > 0 ? ", " : " and ") : "", weeks, weeks > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (days) {
		Q_strcat(temp, 512, va("%s%i day%s", addComma ? (hours > 0 ? ", " : " and ") : "", days, days > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (hours) {
		Q_strcat(temp, 512, va("%s%i hour%s", addComma ? (minutes > 0 ? ", " : " and ") : "", hours, hours > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (minutes) {
		Q_strcat(temp, 512, va("%s%i minute%s", addComma ? (secs > 0 ? ", " : " and ") : "", minutes, minutes > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (secs) {
		Q_strcat(temp, 512, va("%s%i second%s", addComma ? " and " : "", secs, secs > 1 ? "s" : ""));
	}

	Q_strncpyz(dur, temp, len);
}

void G_shrubbot_duration(int secs, char *duration, int dursize) {

	if(secs > (SECONDS_IN_YEAR * 50) || secs < 0) {
		Q_strncpyz(duration, "PERMANENT", dursize);
	}
	else if(secs >= SECONDS_IN_YEAR) {
		Com_sprintf(duration, dursize, "%d years", (secs / SECONDS_IN_YEAR));
	}
	else if(secs >= SECONDS_IN_MONTH) {
		Com_sprintf(duration, dursize, "%i months",	(secs / SECONDS_IN_MONTH));
	}
	else if(secs >= SECONDS_IN_WEEK) {
		Com_sprintf(duration, dursize, "%i weeks", (secs / SECONDS_IN_WEEK));
	}
	else if(secs >= SECONDS_IN_DAY) {
		Com_sprintf(duration, dursize, "%i days", (secs / SECONDS_IN_DAY));
	}
	else if(secs >= SECONDS_IN_HOUR) {
		Com_sprintf(duration, dursize, "%i hours", (secs / SECONDS_IN_HOUR));
	}
	else if(secs >= SECONDS_IN_MINUTE) {
		Com_sprintf(duration, dursize, "%i mins", (secs / SECONDS_IN_MINUTE));
	}
	else {
		Com_sprintf(duration, dursize, "%i secs", secs);
	}
}

qboolean G_shrubbot_ban_check(char const* ip, char const* guid, char const* reason) {
	int 	i;
	time_t 	t;

	if(!time(&t)) return qfalse;

	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;

	for(i=0; g_shrubbot_bans[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1)
			continue;
		// check GUID first..
		if(!Q_stricmp(g_shrubbot_bans[i]->guid, guid)) {
			if(reason) {
				Com_sprintf(
					(char*)reason,
					MAX_STRING_CHARS,
					"^9Reason: %s\nExpires in: %u seconds.^7\n",
					g_shrubbot_bans[i]->reason,
					g_shrubbot_bans[i]->expires - (int)t
					);
			}
			return qtrue;
		}

  		// ..then check IP
		if(*ip && strlen(ip) > 0 &&
			strlen(g_shrubbot_bans[i]->ip) > 0 &&
			Q_strncmp( g_shrubbot_bans[i]->ip, "localhost", sizeof(g_shrubbot_bans[i]->ip) ) &&
			strstr(ip, g_shrubbot_bans[i]->ip) )
		{
			if(reason) {
				Com_sprintf(
					(char*)reason,
					MAX_STRING_CHARS,
					"^9Reason: %s\nExpires in: %u seconds.^7\n",
					g_shrubbot_bans[i]->reason,
					g_shrubbot_bans[i]->expires - (int)t
				);
			}
			return qtrue;
		}

	}
	return qfalse;
}

#ifdef HW_BAN
qboolean G_shrubbot_hwban_check(char const* hwguid, char const* reason) {
	int 	i;
	time_t 	t;

	if(!time(&t)) return qfalse;
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	for(i=0; g_shrubbot_bans[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1)
			continue;
		if(!Q_stricmp(g_shrubbot_bans[i]->hwguid, hwguid)) {
			if(reason) {
				Com_sprintf(
					(char*)reason,
					MAX_STRING_CHARS,
					"^9Reason: %s\nExpires in: %u seconds.^7\n",
					g_shrubbot_bans[i]->reason,
					g_shrubbot_bans[i]->expires - (int)t
					);
			}
			return qtrue;
		}
	}
	return qfalse;
}
#endif // HW_BAN

qboolean G_shrubbot_mute_check(char const* ip, char const* guid) {
	int i;
	time_t t;

	if(!time(&t)) return qfalse;

	for(i=0; g_shrubbot_mutes[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_mutes[i]->expires != 0 && (g_shrubbot_mutes[i]->expires - (t - SHRUBBOT_BAN_EXPIRE_OFFSET)) < 1) {
			continue;
		}
		if(!Q_stricmp(g_shrubbot_mutes[i]->guid, guid)) {
			return qtrue;
		}
/*
		// core: GUID should be all to check,.. why IP too?
		// IRATA: see G_shrubbot_mute_name
		if(*ip && strlen(ip) > 0 &&
			strlen(g_shrubbot_mutes[i]->ip) > 0 &&
			Q_strncmp( g_shrubbot_mutes[i]->ip, "localhost", sizeof(g_shrubbot_mutes[i]->ip) ) &&
			strstr(ip, g_shrubbot_mutes[i]->ip) )
		{
			return qtrue;
		}
*/
	}
	return qfalse;
}

char *G_shrubbot_mute_name(char const* ip, char const* guid) {
	int i;
	time_t t;

	if(!time(&t))
		return "";

	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	for(i=0; g_shrubbot_mutes[i]; i++) {
		if(g_shrubbot_mutes[i]->expires != 0 &&
			(g_shrubbot_mutes[i]->expires - t) < 1)
			continue;
		if(*guid && !Q_stricmp(g_shrubbot_mutes[i]->guid, guid)) {
			return va("%s", g_shrubbot_mutes[i]->name);
		}
/*
		// core: GUID should be all to check,.. why IP too?
		// IRATA: I assume it was done because of security to ensure
		// a player doesn't connect immediately with new GUID
		// maybe we need it later on for nq_security although blocking IP areas
		// should do the better trick (comment kept as reminder)
		if(*ip && strlen(ip) > 0 &&
			strlen(g_shrubbot_mutes[i]->ip) > 0 &&
			Q_strncmp( g_shrubbot_mutes[i]->ip, "localhost", sizeof(g_shrubbot_mutes[i]->ip) ) &&
			strstr(ip, g_shrubbot_mutes[i]->ip) )
		{
			return va("%s", g_shrubbot_mutes[i]->name);
		}
*/
	}
	return "";
}

qboolean G_shrubbot_native_cmd_check( gentity_t *ent, char *cmd, int skip ) {
	int i;
	// commands that are native shrub-cmds..
	// Note: These commands are -NOT- the [command] secions in the shrub config file..
	for (i=0; g_shrubbot_cmds[i].keyword[0]; i++) {
		if(Q_stricmp(cmd, g_shrubbot_cmds[i].keyword)) {
			continue;
		}

		if( ((g_shrubbot_cmds[i].cmdFlags & SCMDF_TYRANNY) || (g_shrubbot_cmds[i].cmdFlags & SCMDF_CHEAT)) && !g_tyranny.integer) {
			SP(va("%s: g_tyranny is not enabled\n", g_shrubbot_cmds[i].keyword));
			_shrubbot_log(ent, "attempted", skip-1);
			return qfalse;
		}
		if(G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag)) {
			g_shrubbot_cmds[i].handler(ent, skip);
			_shrubbot_log(ent, cmd, skip);
			return qtrue;
		}
		// no permission..
		SP(va("%s: permission denied\n", g_shrubbot_cmds[i].keyword));
		_shrubbot_log(ent, "attempted", skip-1);
		return qfalse;
	}
	// it is no shrub command, so let it be executed..
	//return qtrue;
	// cs: #550. this needs to return false. follow it back to ConsoleCommand()
	return qfalse;
}

qboolean G_shrubbot_cmd_check(gentity_t *ent) {
	int i;
	char command[MAX_SHRUBBOT_CMD_LEN];
	char *cmd;
	int skip = 0;

	if(g_shrubbot.string[0] == '\0') {
		return qfalse;
	}

	command[0] = '\0';
	Q_SayArgv(0, command, sizeof(command));
	if(!Q_stricmp(command, "say") ||
		(G_shrubbot_permission(ent, SBF_TEAMFTSHRUBCMD) &&
			(!Q_stricmp(command, "say_team") ||
			!Q_stricmp(command, "say_buddy")))) {
				skip = 1;
				Q_SayArgv(1, command, sizeof(command));
	}

	if(!command[0]) {
		return qfalse;
	}

	if(command[0] == '!') {
		cmd = &command[1];
	}
	else if(ent == NULL) {
		cmd = &command[0];
	}
	else {
		return qfalse;
	}

	// Custom shrub commands that people can create. These are the [command] sections in the shrub config file.
	for(i=0; g_shrubbot_commands[i]; i++) {
		if(Q_stricmp(cmd, g_shrubbot_commands[i]->command)) {
			continue;
		}

		if(_shrubbot_command_permission(ent, cmd)) {
			char *cmdLine;
			int argIx;

			// Replace shortcuts
			if(g_shortcuts.integer){
				cmdLine = G_Shortcuts(ent, g_shrubbot_commands[i]->exec);
			}
			else {
				cmdLine = g_shrubbot_commands[i]->exec;
			}

			// Replace arguments
			for (argIx = 1; argIx <= 9; argIx++) {
				char arg[MAX_NAME_LENGTH];
				Q_SayArgv(skip + argIx, arg, sizeof(arg));
				cmdLine = Q_StrReplace(cmdLine, va("[%i]", argIx), arg);
			}

			trap_SendConsoleCommand(EXEC_APPEND, cmdLine);

			_shrubbot_log(ent, cmd, skip);
			return qtrue;
		}
		else {
			SP(va("%s: permission denied\n", g_shrubbot_commands[i]->command));
			_shrubbot_log(ent, "attempted", skip-1);
			return qfalse;
		}
	}

	// Now check for native shrub commands..
	return G_shrubbot_native_cmd_check( ent, cmd, skip );
}

// TODO: ignoring unkown guid entries ?
qboolean G_shrubbot_readconfig(gentity_t *ent, int skiparg) {
	g_shrubbot_level_t		*L = NULL;
	g_shrubbot_admin_t		*a = NULL;
	g_shrubbot_ban_t		*b = NULL;
	g_shrubbot_ban_t		*m = NULL;
	g_shrubbot_command_t	*c = NULL;
	int						lc = 0, ac = 0, bc = 0, cc = 0, mc = 0;
	fileHandle_t			f;
	int						len;
	char					*cnf, *cnf2;
	char					*t;
	qboolean				level_open, admin_open, ban_open, mute_open, command_open;
	char					levels[MAX_STRING_CHARS] = {""};
	int						pids[MAX_CLIENTS];
	gentity_t				*pidEnt = NULL;
	int						n;
	time_t					expiretime;
	int						seconds;
	char					duration[MAX_STRING_CHARS];

	if(!g_shrubbot.string[0]) return qfalse;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_READ);
	if(len < 0)	{
		SP(va("^dreadconfig: ^9could not open shrubbot config file ^2%s\n", g_shrubbot.string));
		_shrubbot_default_levels();
		return qfalse;
	}

	cnf = malloc(len+1);
	cnf2 = cnf;
	trap_FS_Read(cnf, len, f);
	*(cnf + len) = '\0';
	trap_FS_FCloseFile(f);

	G_shrubbot_cleanup();

	level_open = admin_open = mute_open = ban_open = command_open = qfalse;

	// IRATA: init com_lines
	// FIXME: although COM_BeginParseSession is called we don't get the exact line number by COM_GetCurrentParseLine
	// - Issues with SkipWhitespace in COM_ParseExt ?
	COM_BeginParseSession("ShrubbotConfigRead");

	while(qtrue) {
		t = COM_Parse(&cnf);
		if ( !(*t) ) break;

		if(!Q_stricmp(t, "[level]")) {
			if(lc >= MAX_SHRUBBOT_LEVELS) return qfalse;
			L = malloc(sizeof(g_shrubbot_level_t));
			L->level	= 0;
			*L->name	= '\0';
			*L->flags	= '\0';
			g_shrubbot_levels[lc++] = L;
			admin_open = mute_open = ban_open = command_open = qfalse;
			level_open = qtrue;
			continue;
		}
		else if(!Q_stricmp(t, "[admin]")) {
			if(ac >= MAX_SHRUBBOT_ADMINS) return qfalse;

			if (nq_noq.integer & NOQ_DONT_READ_ADMINS) continue;

			a = malloc(sizeof(g_shrubbot_admin_t));
			*a->name	= '\0';
			*a->guid	= '\0';
			a->level	= 0;
			*a->flags	= '\0';
			g_shrubbot_admins[ac++] = a;
			level_open = mute_open = ban_open = command_open = qfalse;
			admin_open = qtrue;
			continue;
		}
		else if(!Q_stricmp(t, "[ban]")) {
			if(bc >= MAX_SHRUBBOT_BANS) return qfalse;

			if (nq_noq.integer & NOQ_DONT_READ_BANS) continue;

			b = malloc(sizeof(g_shrubbot_ban_t));
			*b->name	= '\0';
			*b->guid	= '\0';
#ifdef HW_BAN
			*b->hwguid	= '\0';
#endif // HW_BAN
			*b->ip		= '\0';
			*b->made	= '\0';
			b->expires	= 0;
			*b->reason	= '\0';
			g_shrubbot_bans[bc++] = b;
			level_open = admin_open = mute_open = command_open = qfalse;
			ban_open = qtrue;
			continue;
		}
		else if(!Q_stricmp(t, "[mute]")) {
			if(mc >= MAX_SHRUBBOT_BANS) return qfalse;

			if (nq_noq.integer & NOQ_DONT_READ_MUTES) continue;

			m = malloc(sizeof(g_shrubbot_ban_t));
			*m->name	= '\0';
			*m->guid	= '\0';
			*m->ip		= '\0';
			*m->made	= '\0';
			m->expires	= 0;
			*m->reason	= '\0';
			g_shrubbot_mutes[mc++] = m;
			level_open = admin_open = ban_open = command_open = qfalse;
			mute_open = qtrue;
			continue;
		}
		else if(!Q_stricmp(t, "[command]")) {
			if(cc >= MAX_SHRUBBOT_COMMANDS) {  // jaquboss use command count instead
				return qfalse;
			}
			c = malloc(sizeof(g_shrubbot_command_t));
			*c->command = '\0';
			*c->exec = '\0';
			*c->desc = '\0';
			memset(c->levels, -1, sizeof(c->levels));
			g_shrubbot_commands[cc++] = c;
			level_open = admin_open = mute_open = ban_open = qfalse;
			command_open = qtrue;
			continue;
		}

		if ( !(level_open || admin_open || mute_open || ban_open || command_open) ) {
			continue;
		}

		if(level_open) {
			if(!Q_stricmp(t, "level")) {
				G_shrubbot_readconfig_int(&cnf, &L->level);
			}
			else if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, L->name, sizeof(L->name));
			}
			else if(!Q_stricmp(t, "flags")) {
				G_shrubbot_readconfig_string(&cnf, L->flags, sizeof(L->flags));
			}
			else {
				SP(va("^dreadconfig: ^9[level] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(admin_open) {
			if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, a->name, sizeof(a->name));
			}
			else if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, a->guid, sizeof(a->guid));
			}
			else if(!Q_stricmp(t, "level")) {
				G_shrubbot_readconfig_int(&cnf, &a->level);
			}
			else if(!Q_stricmp(t, "flags")) {
				G_shrubbot_readconfig_string(&cnf, a->flags, sizeof(a->flags));
			}
			else {
				SP(va("^dreadconfig: ^9[admin] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(ban_open) {
			if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, b->name, sizeof(b->name));
			}
			else if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, b->guid, sizeof(b->guid));
			}
#ifdef HW_BAN
			else if(!Q_stricmp(t, "hwguid")) {
				G_shrubbot_readconfig_string(&cnf, b->hwguid, sizeof(b->hwguid));
			}
#endif // HW_BAN
			else if(!Q_stricmp(t, "ip")) {
				G_shrubbot_readconfig_string(&cnf, b->ip, sizeof(b->ip));
			}
			else if(!Q_stricmp(t, "reason")) {
				G_shrubbot_readconfig_string(&cnf, b->reason, sizeof(b->reason));
			}
			else if(!Q_stricmp(t, "made")) {
				G_shrubbot_readconfig_string(&cnf, b->made, sizeof(b->made));
			}
			else if(!Q_stricmp(t, "expires")) {
				G_shrubbot_readconfig_int(&cnf, &b->expires);
			}
			else if(!Q_stricmp(t, "banner")) {
				G_shrubbot_readconfig_string(&cnf, b->banner, sizeof(b->banner));
			}
			else {
				SP(va("^dreadconfig: ^9[ban] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(mute_open) {
			if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, m->name, sizeof(m->name));
			}
			else if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, m->guid, sizeof(m->guid));
			}
			else if(!Q_stricmp(t, "ip")) {
				G_shrubbot_readconfig_string(&cnf, m->ip, sizeof(m->ip));
			}
			else if(!Q_stricmp(t, "reason")) {
				G_shrubbot_readconfig_string(&cnf, m->reason, sizeof(m->reason));
			}
			else if(!Q_stricmp(t, "made")) {
				G_shrubbot_readconfig_string(&cnf, m->made, sizeof(m->made));
			}
			else if(!Q_stricmp(t, "expires")) {
				G_shrubbot_readconfig_int(&cnf, &m->expires);
			}
			else if(!Q_stricmp(t, "muter")) {
				G_shrubbot_readconfig_string(&cnf, m->banner, sizeof(m->banner));
			}
			else {
				SP(va("^dreadconfig: ^9[mute] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(command_open) {
			if(!Q_stricmp(t, "command")) {
				G_shrubbot_readconfig_string(&cnf, c->command, sizeof(c->command));
			}
			else if(!Q_stricmp(t, "exec")) {
				G_shrubbot_readconfig_string(&cnf, c->exec, sizeof(c->exec));
			}
			else if(!Q_stricmp(t, "desc")) {
				G_shrubbot_readconfig_string(&cnf, c->desc, sizeof(c->desc));
			}
			else if(!Q_stricmp(t, "levels")) {
				char level[4] = {""};
				char *lp = levels;
				int cmdlevel = 0;

				G_shrubbot_readconfig_string(&cnf, levels, sizeof(levels));

				while(*lp) {
					if(*lp == ' ') {
						c->levels[cmdlevel++] = atoi(level);
						level[0] = '\0';
						*lp++;
						continue;
					}
					Q_strcat(level, sizeof(level), va("%c", *lp));
					*lp++;
				}

				if(level[0]) {
					c->levels[cmdlevel++] = atoi(level);
				}

				// tjw: ensure the list is -1 terminated
				c->levels[MAX_SHRUBBOT_LEVELS] = -1;
			}
			else {
				SP(va("readconfig: [command] parse error near %s on line %d\n", t, COM_GetCurrentParseLine()));
			}
		}
	}

	// core: put mute status of clients in the ConfigString CS_PLAYERS
	for ( len=0; len < mc; ++len ) {
		n = ClientNumbersFromString(g_shrubbot_mutes[len]->name, pids);
		pidEnt = &g_entities[pids[0]];
		if ( n == 1 ) {
			if ( localtime(&expiretime) ) {
				seconds = g_shrubbot_mutes[len]->expires - (expiretime - SHRUBBOT_BAN_EXPIRE_OFFSET);
				if(seconds) {
					Com_sprintf(duration, sizeof(duration), "for %i seconds", seconds);
				}
				// core: fix: let the mute status be put in CS_PLAYERS configstring
				pidEnt->client->sess.muted = qtrue;
				pidEnt->client->sess.auto_mute_time = level.time + seconds*SECONDS_1;
				pidEnt->client->sess.muted_by = MUTED_BY_SHRUB;
				ClientConfigStringChanged( pidEnt );
				AP(va("chat \"^dmute: ^*%s ^9has been muted %s\"",	pidEnt->client->pers.netname, duration ));
			}
		}
	}

	free(cnf2);
	SP(va("readconfig: loaded %d levels, %d admins, %d bans, %d mutes, %d commands\n", lc, ac, bc, mc, cc));

	if(lc == 0) _shrubbot_default_levels();
	return qtrue;
}

qboolean G_shrubbot_time(gentity_t *ent, int skiparg) {
	time_t t;
	struct tm *lt;
	char s[50];

	if(!time(&t)) return qfalse;

	lt = localtime(&t);
	strftime(s, sizeof(s), "%I:%M%p %Z", lt);
	AP(va("chat \"^dtime:^7 %s\" -1", s));
	return qtrue;
}

qboolean G_shrubbot_setlevel(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char lstr[11]; // 10 is max strlen() for 32-bit int
	char *guid;
	char n2[MAX_NAME_LENGTH];
	int l, i;
	gentity_t *vic;
	qboolean updated = qfalse;
	g_shrubbot_admin_t *a;
	qboolean found = qfalse;

	if(Q_SayArgc() < 3+skiparg) {
		SP("setlevel usage: !setlevel [name|slot#] [level]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, lstr, sizeof(lstr));
	l = atoi(lstr);

	// forty - don't allow privilege escalation
	if(l > G_shrubbot_level(ent)) {
		SP("setlevel: you may not setlevel higher than your current level\n");
		return qfalse;
	}

	// tjw: if somone sets g_shrubbot on a running server then uses
	//      setlevel in the console, we need to make sure we have
	//      at least the default levels loaded.
	if(!ent)
		G_shrubbot_readconfig(NULL, 0);

	for(i=0; g_shrubbot_levels[i]; i++) {
		if(g_shrubbot_levels[i]->level == l) {
			found = qtrue;
			break;
		}
	}
	if(!found) {
		SP("setlevel: level is not defined\n");
		return qfalse;
	}
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("setlevel: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("setlevel: sorry, but your intended victim has a higher admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	guid = level.clients[pids[0]].pers.cl_guid;

	// IRATA: don't level if there is no GUID
	// no need for an option to disable ... it doesn't make any sense to level non GUID players
	if ( !Q_stricmp(guid, "unknown") || !Q_stricmp(guid, "NO_GUID") )  {
		SP("setlevel: sorry, but your intended victim has no GUID!\n");
		return qfalse;
	}

	// tjw: use raw name
	Q_strncpyz(n2, vic->client->pers.netname, sizeof(n2));

	for(i=0;g_shrubbot_admins[i];i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			g_shrubbot_admins[i]->level = l;
			Q_strncpyz(g_shrubbot_admins[i]->name, n2,
				sizeof(g_shrubbot_admins[i]->name));
			updated = qtrue;
		}
	}
	if(!updated) {
		if(i == MAX_SHRUBBOT_ADMINS) {
			SP("setlevel: too many admins\n");
			return qfalse;
		}
		a = malloc(sizeof(g_shrubbot_admin_t));
		a->level = l;
		Q_strncpyz(a->name, n2, sizeof(a->name));
		Q_strncpyz(a->guid, guid, sizeof(a->guid));
		*a->flags = '\0';
		g_shrubbot_admins[i] = a;
	}

	AP(va("chat \"^dsetlevel: ^9%s^9 is now a level %d user^7\" -1",
		vic->client->pers.netname,
		l));
	_shrubbot_writeconfig();
	return qtrue;
}

// IRATA: lua / NOQ might handle no GUIDs differently
// 		  we don't check for unknown GUID for now but printing a warning
qboolean G_shrubbot_setlevel_lua(int slot, int shrubbotlevel) {
	char *guid;
	char n2[MAX_NAME_LENGTH];
	int i;
	gentity_t *vic;
	qboolean updated = qfalse;
	g_shrubbot_admin_t *a;
	qboolean found = qfalse;

	// slot check
	if (slot < 0 || slot > MAX_CLIENTS-1) {
		G_Printf("G_shrubbot_setlevel_lua: invalid slot. Aborting.\n");
			return qfalse;
	}

	// check if level structure is loaded ...
	// tjw: if somone sets g_shrubbot on a running server then uses
	//      setlevel in the console, we need to make sure we have
	//      at least the default levels loaded.
	// IRATA: if g_shrubbot.string is set the levels are loaded - see G_InitGame
	if ( !(g_shrubbot.string[0]) ) {
		G_Printf("G_shrubbot_setlevel_lua: levels are not loaded. Aborting.\n");
			return qfalse;
	}

	// check if level definition does exist
	for(i=0; g_shrubbot_levels[i]; ++i) {
		if(g_shrubbot_levels[i]->level == shrubbotlevel) {
			found = qtrue;
			break;
		}
	}
	if(!found) {
		G_Printf("G_shrubbot_setlevel_lua: level is not defined. Aborting.\n");
		return qfalse;
	}

    // get the ent from slot
	vic = &g_entities[slot];
	// do a sanity check if slot is used ...
	if (vic->client->pers.connected == CON_CONNECTING || vic->client->pers.connected == CON_CONNECTED ) {

		guid = level.clients[slot].pers.cl_guid;

		if ( !Q_stricmp(guid, "unknown") || !Q_stricmp(guid, "NO_GUID") || !guid[0])  {
			G_Printf("G_shrubbot_setlevel_lua WARNING: client has no GUID!\n");
		}

		// tjw: use raw name
		Q_strncpyz(n2, vic->client->pers.netname, sizeof(n2));

		for(i=0;g_shrubbot_admins[i];++i) {
			if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
				g_shrubbot_admins[i]->level = shrubbotlevel;
				Q_strncpyz(g_shrubbot_admins[i]->name, n2,
					sizeof(g_shrubbot_admins[i]->name));
				updated = qtrue;
			}
		}
		if(!updated) {
			if(i == MAX_SHRUBBOT_ADMINS) {
				G_Printf("G_shrubbot_setlevel_lua: too many admins\n");
				return qfalse;
			}
			a = malloc(sizeof(g_shrubbot_admin_t));
			a->level = shrubbotlevel;
			Q_strncpyz(a->name, n2, sizeof(a->name));
			Q_strncpyz(a->guid, guid, sizeof(a->guid));
			*a->flags = '\0';
			g_shrubbot_admins[i] = a;
		}

		return qtrue;
	}

	// should never happen ...
	G_Printf("G_shrubbot_setlevel_lua: player not in game. Aborting.\n");
	return qfalse;
}

qboolean G_shrubbot_kick(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], *reason, err[MAX_STRING_CHARS];
	int minargc;

	minargc = 3+skiparg;
	if(G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}

	if(Q_SayArgc() < minargc) {
		SP("^dkick usage: ^2!kick ^9[name] [reason]^7\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dkick: ^9%s\n", err));
		return qfalse;
	}

	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^dkick: ^9sorry, %s ^9is a higher level admin than you are.\n",
			g_entities[pids[0]].client->pers.netname));
		return qfalse;
	}

	// IRATA: still kick bots
	if ( g_entities[pids[0]].client->pers.localClient ) {
		SP("^dkick: ^9Cannot kick server host. Use bot commands to control bot count.\n");
		return qfalse;
	}

	if(G_shrubbot_permission(&g_entities[pids[0]], SBF_IMMUTABLE)) {
		SP( "Can't kick immune admins!\n" );
		return qfalse;
	}

	// IRATA - sound for kick
	if( g_announcer.integer & ANNOUNCE_KICKSOUND) {
		G_globalSoundEnum(GAMESOUND_MISC_KICKED);
	}

	// forty - enforce the temp ban consistently using shrubbot.
	if ( g_autoTempBan.integer && g_autoTempBanTime.integer ) {
		G_shrubbot_tempban(pids[0],
			va("^9You have been kicked, Reason: %s^7",
			(*reason) ? reason : "^9kicked by admin^7"),
			g_autoTempBanTime.integer);
	}


	//josh: 2.60 won't kick from the engine. This will call
	//      ClientDisconnect
	// 120 seconds. Make #define?
	trap_DropClient(pids[0],
		va("^9You have been kicked, Reason: %s^7",
		(*reason) ? reason : "^9kicked by admin^7"),
		120);
	return qtrue;
}

// TODO: check this for unkown GUID - R.I.P. punkbuster
qboolean G_shrubbot_tempban(int clientnum, char *reason, int length) {
	char *guid, *ip;
#ifdef HW_BAN
	char *hwguid;
#endif // HW_BAN
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic;

	if(!time(&t)) return qfalse;

	vic = &g_entities[clientnum];
	guid 	= vic->client->pers.cl_guid;

	// TODO?
	// IRATA: tempban unknown GUID users doesn't make sense ?!
	// if we keep it all unknown GUID players get kicked/banned
	// if we don't keep it there no way to kick/ban annoying players
	// possible solution kick by name/ip ... but this isn't the premium way to go :/
	// -> inspect!

#ifdef HW_BAN
	hwguid 	= vic->client->pers.cg_hwguid;
#endif // HW_BAN
	ip 		= vic->client->pers.client_ip;
	b 		= malloc(sizeof(g_shrubbot_ban_t));

	if(!b) {
		return qfalse;
	}

	Q_strncpyz(b->name, vic->client->pers.netname, sizeof(b->name));
	Q_strncpyz(b->guid, guid, sizeof(b->guid));
#ifdef HW_BAN
	Q_strncpyz(b->hwguid, hwguid, sizeof(b->hwguid));
#endif // HW_BAN
	// strip port off of ip
	for(i=0; *ip; *ip++) {
		if(i >= sizeof(tmp) || *ip == ':') break;
		tmp[i++] = *ip;
	}
	tmp[i] = '\0';
	Q_strncpyz(b->ip, tmp, sizeof(b->ip));

	lt = localtime(&t);
	strftime(b->made, sizeof(b->made), "%m/%d/%y %H:%M:%S", lt);
	Q_strncpyz(b->banner, "Temp Ban System", sizeof(b->banner));

	b->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + length;
	if(!*reason) {
		Q_strncpyz(b->reason,
			"banned by Temp Ban System",
			sizeof(b->reason));
	} else {
		Q_strncpyz(b->reason, reason, sizeof(b->reason));
	}

	for(i=0; g_shrubbot_bans[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		free(b);
		return qfalse;
	}
	g_shrubbot_bans[i] = b;
	_shrubbot_writeconfig();
	return qtrue;
}


// TODO: check for unknown GUID !!!
qboolean G_shrubbot_ban(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason, err[MAX_STRING_CHARS];
	char *guid, *ip;
#ifdef HW_BAN
	char *hwguid;
#endif // HW_BAN
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic;
	int minargc;
	char duration[MAX_STRING_CHARS];
	int modifier = 1;

	if(!time(&t)) return qfalse;

	if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) &&
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}
	else if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) ||
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {

		minargc = 3+skiparg;
	}
	else {
		minargc = 4+skiparg;
	}
	if(Q_SayArgc() < minargc) {
		SP("ban usage: !ban [name] [seconds] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));

	// tjw: support "w" (weeks), "d" (days), "h" (hours),
	//      and "m" (minutes) modifiers
	// Jaybird - you cannot assume they indeed do use a modifier.
	// This was chopping the last character off regardless of what it was.
	// This resulted in normal seconds being 1/10th of their intended length.
	if(*secs) {
		int lastchar = strlen(secs) - 1;
		if (secs[lastchar] < '0' || secs[lastchar] > '9') {
			if(secs[lastchar] == 'w')
				modifier = 60*60*24*7;
			else if(secs[lastchar] == 'd')
				modifier = 60*60*24;
			else if(secs[lastchar] == 'h')
				modifier = 60*60;
			else if(secs[lastchar] == 'm')
				modifier = 60;
			secs[lastchar] = '\0';
		}
	}
	seconds = atoi(secs);
	if(seconds > 0)
		seconds *= modifier;

	if(seconds <= 0) {
		if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN)) {
			seconds = 0;
		}
		else {
			SP("ban: seconds must be a positive integer\n");
			return qfalse;
		}
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("ban: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("ban: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}

	if ( g_entities[pids[0]].client->pers.localClient ) {
		if ( g_entities[pids[0]].r.svFlags & SVF_BOT ) {
			SP("^dban: ^9Cannot ban a bot.\n");
			return qfalse;
		}
		else {
			SP("^dban: ^9Cannot ban server host.\n");
			return qfalse;
		}
	}

	if(G_shrubbot_permission(&g_entities[pids[0]], SBF_IMMUTABLE)) {
		SP( "Can't ban immune admins!\n" );
		return qfalse;
	}


	vic 	= &g_entities[pids[0]];
	guid 	= vic->client->pers.cl_guid;


// TODO: ban unknown GUIDs ?...better: ban by IP instead ...

#ifdef HW_BAN
	hwguid 	= vic->client->pers.cg_hwguid;
#endif // HW_BAN
	ip 		= vic->client->pers.client_ip;
	b 		= malloc(sizeof(g_shrubbot_ban_t));

	if(!b) {
		return qfalse;
	}

	Q_strncpyz(b->name, vic->client->pers.netname, sizeof(b->name));
	Q_strncpyz(b->guid, guid, sizeof(b->guid));

#ifdef HW_BAN
	Q_strncpyz(b->hwguid, hwguid, sizeof(b->hwguid));
#endif // HW_BAN

	// strip port off of ip
	for(i=0; *ip; *ip++) {
		if(i >= sizeof(tmp) || *ip == ':') break;
		tmp[i++] = *ip;
	}
	tmp[i] = '\0';
	Q_strncpyz(b->ip, tmp, sizeof(b->ip));

	lt = localtime(&t);
	strftime(b->made, sizeof(b->made), "%m/%d/%y %H:%M:%S", lt);
	if(ent) {
		Q_strncpyz(b->banner,
			ent->client->pers.netname,
			sizeof(b->banner));
	}
	else Q_strncpyz(b->banner, "console", sizeof(b->banner));
	if(!seconds)
		b->expires = 0;
	else
		b->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;
	if(!*reason) {
		Q_strncpyz(b->reason,
			"banned by admin",
			sizeof(b->reason));
	}
	else {
		Q_strncpyz(b->reason, reason, sizeof(b->reason));
	}

	for(i=0; g_shrubbot_bans[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("ban: too many bans\n");
		free(b);
		return qfalse;
	}
	g_shrubbot_bans[i] = b;

	SP(va("ban: %s^7 is now banned\n", vic->client->pers.netname));
	_shrubbot_writeconfig();
	// 0 seconds. Let bans handle time
	if(seconds) {
		Com_sprintf(duration,
			sizeof(duration),
			"for %i seconds",
			seconds);
	}
	else {
		Q_strncpyz(duration, "PERMANENTLY", sizeof(duration));
	}

	// IRATA - sound for ban
	if( g_announcer.integer & ANNOUNCE_BANSOUND) {
		G_globalSoundEnum(GAMESOUND_MISC_BANNED);
	}

	trap_DropClient(pids[0],
		va("You have been banned %s, Reason: %s",
		duration,
		(*reason) ? reason : "banned by admin"),
		0);
	return qtrue;
}

// core: ban a player on GUID only..
// This can be used to ban a player that is not online/ingame.
// If however the player is ingame, then his data will be stored in the ban (netname, ip etc)..
qboolean G_shrubbot_banguid(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason;
	char guid[PB_GUID_LENGTH+1];
	char *guidOnline = NULL;
	char *ip = NULL;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic = NULL;
	int minargc;
	char duration[MAX_STRING_CHARS];
	int modifier = 1;
	qboolean isOnline = qfalse;
	gclient_t *p;

	if(!time(&t)) return qfalse;

	if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) &&
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}
	else if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) ||
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {

		minargc = 3+skiparg;
	}
	else {
		minargc = 4+skiparg;
	}
	if(Q_SayArgc() < minargc) {
		SP("banguid usage: !banguid [GUID] [seconds] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, guid, sizeof(guid));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));

	// check length of passed GUID-string..
	if ( strlen(guid) < PB_GUID_LENGTH ) {
		SP( va("banguid: GUID must have %i characters \n", PB_GUID_LENGTH) );
		return qfalse;
	}

	// tjw: support "w" (weeks), "d" (days), "h" (hours),
	//      and "m" (minutes) modifiers
	// Jaybird - you cannot assume they indeed do use a modifier.
	// This was chopping the last character off regardless of what it was.
	// This resulted in normal seconds being 1/10th of their intended length.
	if(*secs) {
		int lastchar = strlen(secs) - 1;
		if (secs[lastchar] < '0' || secs[lastchar] > '9') {
			if(secs[lastchar] == 'w')
				modifier = 60*60*24*7;
			else if(secs[lastchar] == 'd')
				modifier = 60*60*24;
			else if(secs[lastchar] == 'h')
				modifier = 60*60;
			else if(secs[lastchar] == 'm')
				modifier = 60;
			secs[lastchar] = '\0';
		}
	}
	seconds = atoi(secs);
	if(seconds > 0) {
		seconds *= modifier;
	}

	if(seconds <= 0) {
		if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN)) {
			seconds = 0;
		}
		else {
			SP("banguid: seconds must be a positive integer\n");
			return qfalse;
		}
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	// check if the player is online..
	pids[0] = -1;

	// TODO: check if player has unkown guid
// if we ban unkown GUID here ALL players with no guid are gone ...

	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED && p->pers.connected != CON_CONNECTING) {
			continue;
		}
		if ( !Q_strncmp( p->pers.cl_guid, guid, PB_GUID_LENGTH ) ) {
			// player is online..
			isOnline = qtrue;
			pids[0] = i;
			pids[1] = -1;
			strncpy( name, p->pers.netname, sizeof(name) );
			break;
		}
	}

	if ( isOnline ) {
		if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
			SP("banguid: sorry, but your intended victim has a higher admin"
				" level than you do.\n");
			return qfalse;
		}

		if ( g_entities[pids[0]].client->pers.localClient ) {
			if ( g_entities[pids[0]].r.svFlags & SVF_BOT ) {
				SP("^dbanguid: ^9Cannot ban bot.\n");
				return qfalse;
			}
			else {
				SP("^dbanguid: ^9Cannot ban server host.\n");
				return qfalse;
			}
		}

		vic 		= &g_entities[pids[0]];
		guidOnline 	= vic->client->pers.cl_guid;
		ip 			= vic->client->pers.client_ip;
	}

	b = malloc(sizeof(g_shrubbot_ban_t));
	if(!b)
		return qfalse;

	if ( isOnline ) {
		Q_strncpyz(b->name, vic->client->pers.netname, sizeof(b->name));
		Q_strncpyz(b->guid, guidOnline, sizeof(b->guid));

		// strip port off of ip
		for(i=0; *ip; *ip++) {
			if(i >= sizeof(tmp) || *ip == ':') break;
			tmp[i++] = *ip;
		}
		tmp[i] = '\0';
		Q_strncpyz(b->ip, tmp, sizeof(b->ip));
	}
	else {
		Q_strncpyz(b->name, "unknown", sizeof(b->name));
		Q_strncpyz(b->guid, guid, sizeof(b->guid));
		Q_strncpyz(b->ip, "offline", sizeof(b->ip));
	}

	lt = localtime(&t);
	strftime(b->made, sizeof(b->made), "%m/%d/%y %H:%M:%S", lt);
	if(ent) {
		Q_strncpyz(b->banner, ent->client->pers.netname, sizeof(b->banner));
	}
	else Q_strncpyz(b->banner, "console", sizeof(b->banner));

	if(!seconds)
		b->expires = 0;
	else
		b->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;

	if(!*reason) {
		Q_strncpyz(b->reason, "banned by admin", sizeof(b->reason));
	}
	else {
		Q_strncpyz(b->reason, reason, sizeof(b->reason));
	}

	for(i=0; g_shrubbot_bans[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("banguid: too many bans\n");
		free(b);
		return qfalse;
	}
	g_shrubbot_bans[i] = b;

	if ( isOnline ) {
		SP(va("banguid: %s^7 is now banned, GUID = %s\n", vic->client->pers.netname, guidOnline));
	} else {
		SP(va("banguid: %s^7 is now banned\n", guid));
	}

	_shrubbot_writeconfig();
	// 0 seconds. Let bans handle time
	if (seconds) {
		Com_sprintf(duration, sizeof(duration), "for %i seconds", seconds);
	}
	else {
		Q_strncpyz(duration, "PERMANENTLY", sizeof(duration));
	}

	// IRATA - sound for ban
	if( g_announcer.integer & ANNOUNCE_BANSOUND) {
		G_globalSoundEnum(GAMESOUND_MISC_BANNED);
	}

	if ( isOnline ) {
		trap_DropClient(pids[0], va("You have been banned %s, Reason: %s", duration, (*reason) ? reason : "banned by admin"), 0);
	}
	return qtrue;
}

// core: ban a player by IP only..
// This can be used to ban a player that is not online/ingame.
// If however the player is ingame, then his data will be stored in the ban (netname, ip etc)..
qboolean G_shrubbot_banIP(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7], ip[MAX_IP_LENGTH];
	char *reason;
	char guid[PB_GUID_LENGTH+1];
	char *guidOnline = NULL;
	char *ipOnline = NULL;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic = NULL;
	int minargc;
	char duration[MAX_STRING_CHARS];
	int modifier = 1;
	qboolean isOnline = qfalse;
	gclient_t *p;

	if(!time(&t)) return qfalse;

	if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) &&
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}
	else if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) ||
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 3+skiparg;
	}
	else {
		minargc = 4+skiparg;
	}
	if(Q_SayArgc() < minargc) {
		SP("banIP usage: !banIP [IP] [seconds] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, ip, sizeof(ip));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));

	// tjw: support "w" (weeks), "d" (days), "h" (hours),
	//      and "m" (minutes) modifiers
	// Jaybird - you cannot assume they indeed do use a modifier.
	// This was chopping the last character off regardless of what it was.
	// This resulted in normal seconds being 1/10th of their intended length.
	if(*secs) {
		int lastchar = strlen(secs) - 1;
		if (secs[lastchar] < '0' || secs[lastchar] > '9') {
			if(secs[lastchar] == 'w')
				modifier = 60*60*24*7;
			else if(secs[lastchar] == 'd')
				modifier = 60*60*24;
			else if(secs[lastchar] == 'h')
				modifier = 60*60;
			else if(secs[lastchar] == 'm')
				modifier = 60;
			secs[lastchar] = '\0';
		}
	}
	seconds = atoi(secs);
	if(seconds > 0) {
		seconds *= modifier;
	}

	if(seconds <= 0) {
		if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN)) {
			seconds = 0;
		}
		else {
			SP("banIP: seconds must be a positive integer\n");
			return qfalse;
		}
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	// check if the player is online..
	pids[0] = -1;
	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED && p->pers.connected != CON_CONNECTING) {
			continue;
		}
		if ( !Q_strncmp( p->pers.client_ip, ip, MAX_IP_LENGTH ) ) {
			// player is online..
			isOnline = qtrue;
			pids[0] = i;
			pids[1] = -1;
			strncpy( name, p->pers.netname, sizeof(name) );
			break;
		}
	}

	if ( isOnline ) {
		if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
			SP("banIP: sorry, but your intended victim has a higher admin"
				" level than you do.\n");
			return qfalse;
		}

		if ( g_entities[pids[0]].client->pers.localClient ) {
			if ( g_entities[pids[0]].r.svFlags & SVF_BOT ) {
				SP("^dbanIP: ^9Cannot ban bot.\n");
				return qfalse;
			}
			else {
				SP("^dbanIP: ^9Cannot ban server host.\n");
				return qfalse;
			}
		}

		vic 		= &g_entities[pids[0]];
		guidOnline 	= vic->client->pers.cl_guid;
		ipOnline	= vic->client->pers.client_ip;
	}

	b = malloc(sizeof(g_shrubbot_ban_t));
	if(!b) {
		return qfalse;
	}

	if ( isOnline ) {
		Q_strncpyz(b->name, vic->client->pers.netname, sizeof(b->name));
		Q_strncpyz(b->guid, guidOnline, sizeof(b->guid));

		// strip port off of ip
		for(i=0; *ipOnline; *ipOnline++) {
			if(i >= sizeof(tmp) || *ipOnline == ':') break;
			tmp[i++] = *ipOnline;
		}
		tmp[i] = '\0';
		Q_strncpyz(b->ip, tmp, sizeof(b->ip));
	}
	else {
		Q_strncpyz(b->name, "unknown", sizeof(b->name));
		Q_strncpyz(b->guid, "offline", sizeof(b->guid));
		Q_strncpyz(b->ip, ip, sizeof(b->ip));
	}

	lt = localtime(&t);
	strftime(b->made, sizeof(b->made), "%m/%d/%y %H:%M:%S", lt);
	if(ent) {
		Q_strncpyz(b->banner, ent->client->pers.netname, sizeof(b->banner));
	}
	else Q_strncpyz(b->banner, "console", sizeof(b->banner));

	if(!seconds)
		b->expires = 0;
	else
		b->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;

	if(!*reason) {
		Q_strncpyz(b->reason, "banned by admin", sizeof(b->reason));
	}
	else {
		Q_strncpyz(b->reason, reason, sizeof(b->reason));
	}

	for(i=0; g_shrubbot_bans[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("banIP: too many bans\n");
		free(b);
		return qfalse;
	}
	g_shrubbot_bans[i] = b;

	if ( isOnline ) {
		SP(va("banIP: %s^7 is now banned, IP = %s\n", vic->client->pers.netname, ip));
	}
	else {
		SP(va("banIP: %s^7 is now banned\n", guid));
	}

	_shrubbot_writeconfig();
	// 0 seconds. Let bans handle time
	if (seconds) {
		Com_sprintf(duration, sizeof(duration), "for %i seconds", seconds);
	}
	else {
		Q_strncpyz(duration, "PERMANENTLY", sizeof(duration));
	}

	// IRATA - sound for ban
	if( g_announcer.integer & ANNOUNCE_BANSOUND) {
		G_globalSoundEnum(GAMESOUND_MISC_BANNED);
	}

	if ( isOnline ) {
		trap_DropClient(pids[0], va("You have been banned %s, Reason: %s", duration, (*reason) ? reason : "banned by admin"), 0);
	}
	return qtrue;
}

qboolean G_shrubbot_unban(gentity_t *ent, int skiparg) {
	int bnum;
	char bs[4];
	time_t t;

	if(!time(&t)) return qfalse;
	if(Q_SayArgc() < 2+skiparg) {
		SP("^dunban usage: ^2!unban [ban #]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, bs, sizeof(bs));
	bnum = atoi(bs);
	if ( (bnum < 1) || (!g_shrubbot_bans[bnum-1]) ) {
		SP("^dunban: ^9invalid ban #\n");
		return qfalse;
	}
	g_shrubbot_bans[bnum-1]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	SP(va("^dunban: ^9ban #%d removed\n", bnum));
	_shrubbot_writeconfig();
	return qtrue;
}

#define DEFAULT_MUTE_SECONDS 300

qboolean G_shrubbot_mute(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason, err[MAX_STRING_CHARS];
	char *guid, *ip;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *m = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic;
	int minargc;
	char duration[MAX_STRING_CHARS];
	int modifier = 1;

	if(!time(&t)) return qfalse;

	if(G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}
	else {
		minargc = 3+skiparg;
	}

	if(Q_SayArgc() < minargc) {
		SP("^dmute usage: ^2!mute [name] [seconds] [reason]\n");
		return qfalse;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));

	if(*secs) {
		int lastchar = strlen(secs) - 1;

		if (secs[lastchar] < '0' || secs[lastchar] > '9') {
			if(secs[lastchar] == 'w')
				modifier = 60*60*24*7;
			else if(secs[lastchar] == 'd')
				modifier = 60*60*24;
			else if(secs[lastchar] == 'h')
				modifier = 60*60;
			else if(secs[lastchar] == 'm')
				modifier = 60;
			secs[lastchar] = '\0';
		}
	}

	seconds = atoi(secs);
	if(seconds > 0) {
		seconds *= modifier;
	}

	if(seconds <= 0) {
		seconds = DEFAULT_MUTE_SECONDS;
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dmute: ^9%s\n", err));
		return qfalse;
	}

	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dmute: ^9sorry, but your intended victim has a higher admin level than you do.\n");
		return qfalse;
	}

	if(G_shrubbot_permission(&g_entities[pids[0]], SBF_IMMUTABLE)) {
		SP( "Can't mute immune admins!\n" );
		return qfalse;
	}


	guid 	= level.clients[pids[0]].pers.cl_guid;
	ip 		= level.clients[pids[0]].pers.client_ip;

	vic = &g_entities[pids[0]];

	if ( vic->client->sess.muted || G_shrubbot_mute_check(ip, guid) ) {
		SP("^dmute: ^9already muted\n");
		return qfalse;
	}

	if ( level.clients[pids[0]].pers.localClient && !(vic->r.svFlags & SVF_BOT) ) {
		SP("^dmute: ^9Cannot mute server host.\n");
		return qfalse;
	}

	if ( vic->r.svFlags & SVF_BOT ) {
		SP("^dmute: ^9Cannot mute a bot.\n");
		return qfalse;
	}


	m = malloc(sizeof(g_shrubbot_ban_t));

	if(!m) {
		return qfalse;
	}

	Q_strncpyz(m->name, vic->client->pers.netname, sizeof(m->name));
	Q_strncpyz(m->guid, guid, sizeof(m->guid));

	// strip port off of ip
	for(i=0; *ip; *ip++) {
		if(i >= sizeof(tmp) || *ip == ':') break;
		tmp[i++] = *ip;
	}
	tmp[i] = '\0';
	Q_strncpyz(m->ip, tmp, sizeof(m->ip));

	lt = localtime(&t);
	strftime(m->made, sizeof(m->made), "%m/%d/%y %H:%M:%S", lt);

	if(ent) {
		Q_strncpyz(m->banner, ent->client->pers.netname, sizeof(m->banner));
	}
	else Q_strncpyz(m->banner, "console", sizeof(m->banner));

	// core: some extra checks for empty IP..
	// this should already be handled in ClientUserInfoChanged.
	if ( strlen(m->ip) == 0 ) {
		SP( va("^dmute: ^9IP is empty !!  muter = %s\n", m->banner) );
		free(m);
		return qfalse;
	}

	m->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;

	if(!*reason) {
		Q_strncpyz(m->reason, "muted by admin", sizeof(m->reason));
	}
	else {
		Q_strncpyz(m->reason, reason, sizeof(m->reason));
	}

	for(i=0; g_shrubbot_mutes[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("^dmute: ^9too many mutes\n");
		free(m);
		return qfalse;
	}

	g_shrubbot_mutes[i] = m;

	// core: if a player is already muted (by a referee or by censoring),
	// then increase the duration of the mute.
	// IlDuca - TODO : is this check needed ?? read above at line 2064, if that was
	// true we already left this function
	if ( !vic->client->sess.muted ) {
		// core: fix: let the mute status be put in CS_PLAYERS configstring
		vic->client->sess.muted = qtrue;
		vic->client->sess.auto_mute_time = level.time + seconds*SECONDS_1;	// unmute time
		vic->client->sess.muted_by = MUTED_BY_SHRUB;
		ClientConfigStringChanged( &g_entities[pids[0]] );
	}
	else {
		if ( vic->client->sess.auto_mute_time >= 0 ) {
			vic->client->sess.auto_mute_time += seconds*SECONDS_1;	// increase unmute time
		}
	}

	// 0 seconds. Let bans handle time
	if(seconds) {
		Com_sprintf(duration, sizeof(duration), "for %i seconds", seconds);
	}
	AP(va("chat \"^dmute: ^*%s ^9has been muted %s\"",	vic->client->pers.netname, duration ));

	_shrubbot_writeconfig();

	return qtrue;
}

int MuteNumbersFromString( char *s, int *plist) {
	int i, found = 0;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];
	char *m;
	qboolean is_slot = qtrue;
	time_t t;

	if(!time(&t)) return 0;

	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;

	*plist = -1;

	// if a number is provided, it might be a slot #
	for(i=0; i<(int)strlen(s); i++) {
		if(s[i] < '0' || s[i] > '9') {
			is_slot = qfalse;
			break;
		}
	}
	if(is_slot) {
		i = atoi(s);

		if(level.clients[i].pers.connected == CON_CONNECTED ||
			level.clients[i].pers.connected == CON_CONNECTING) {
				SanitizeString(level.clients[i].pers.netname, s2, qtrue);
			} else {
				return 0;
			}
	}
	else {
		// now look for name matches
		SanitizeString(s, s2, qtrue);
	}

	if(strlen(s2) < 1) return 0;
	for(i=0; g_shrubbot_mutes[i]; i++) {
		if (( g_shrubbot_mutes[i]->expires == 0) || ((g_shrubbot_mutes[i]->expires - t) > 0) ){
			SanitizeString(g_shrubbot_mutes[i]->name, n2, qtrue);
			m = strstr(n2, s2);
			if(m != NULL) {
				*plist++ = i;
				found++;
			}
			if ( found >= 63 ){
				*plist = -1;
				return found;
			}
		}
	}
	*plist = -1;
	return found;
}

qboolean G_MatchOneMute(int *plist, char *err, int len) {
	int *p;
	char line[MAX_NAME_LENGTH+10];

	err[0] = '\0';
	line[0] = '\0';

	if(plist[0] == -1) {
		Q_strcat(err, len,
			"no player by that name or slot # is muted");
		return qfalse;
	}

	if(plist[1] != -1) {
		Q_strcat(err, len, "more than one player name matches. "
			"be more specific or use the slot #:\n");
		for(p = plist;*p != -1; p++) {
			Com_sprintf(line, MAX_NAME_LENGTH+10, "%2i - %s^7 %s\n",
				*p,
				g_shrubbot_mutes[*p]->name,
				g_shrubbot_mutes[*p]->reason ? va("for %s^7", g_shrubbot_mutes[*p]->reason ) : "");
			if(strlen(err)+strlen(line) > len)
				break;
			Q_strcat(err, len, line);
		}
		return qfalse;
	}
	return qtrue;
}

qboolean G_shrubbot_unmute(gentity_t *ent, int skiparg) {
	char		ms[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	time_t		t;
	int			pids[MAX_CLIENTS];
	int			mutes = -1;
	gentity_t	*pidEnt = NULL;
	int			n, recIndex = -1;

	if(!time(&t)) {
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dunmute usage: ^2!unmute [name]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, ms, sizeof(ms));

	// core: check the shrub record index..
	for (n=0; g_shrubbot_mutes[n]; ++n) {
		if ( !Q_stricmpn(ms, g_shrubbot_mutes[n]->name, MAX_NAME_LENGTH) ) {
			recIndex = n;
			break;
		}
	}

	// redeye - search for auto-muted players
	n = ClientNumbersFromString(ms, pids);
	pidEnt = &g_entities[pids[0]];
	if ( n == 1 ) {
		if ( pidEnt->client->sess.muted ) {
			// core: fix: let the mute status be put in CS_PLAYERS configstring
			pidEnt->client->sess.muted = qfalse;
			pidEnt->client->sess.auto_mute_time = -1;
			pidEnt->client->sess.muted_by = UNMUTED;
			ClientConfigStringChanged( pidEnt );
			AP(va("chat \"^dunmute: ^*%s ^9has been unmuted\"", pidEnt->client->pers.netname));
			if ( recIndex != -1 ) {
				g_shrubbot_mutes[recIndex]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
			}
			_shrubbot_writeconfig();
			return qtrue;
		}
	}

	mutes = MuteNumbersFromString(ms, pids);
	pidEnt = &g_entities[pids[0]];
	if( mutes != 1) {
		if ( Q_SayArgc() == 3+skiparg ) {
			int num;

			Q_SayArgv(2+skiparg, ms, sizeof(ms));
			num = atoi(ms);

			if ( num >= 1 && num <= mutes){
				pids[0] = pids[num-1]; // to make code under compatible just copy the number
			}
			else {
				SP("invalid number\n");
				return qfalse;
			}
		}
		else {
			G_MatchOneMute(pids, err, sizeof(err));
			SP(va("^dunmute: ^9%s\n", err));
			return qfalse;
		}
	}

	// core: fix: let the mute status be put in CS_PLAYERS configstring
	pidEnt->client->sess.muted = qfalse;
	pidEnt->client->sess.auto_mute_time = -1;
	pidEnt->client->sess.muted_by = UNMUTED;
	ClientConfigStringChanged( pidEnt );
	if ( recIndex != -1 ) {
		AP(va("chat \"^dunmute: ^*%s ^9has been unmuted\"", g_shrubbot_mutes[recIndex]->name ));
		g_shrubbot_mutes[recIndex]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	}

	_shrubbot_writeconfig();

	return qtrue;
}

qboolean G_shrubbot_putteam(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], team[7], err[MAX_STRING_CHARS];
	gentity_t *vic;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));

	if(Q_SayArgc() < 3+skiparg) {
		SP("^dputteam usage: ^2!put ^9[name] [r|b|s]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dput: ^9%s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^dput: ^9sorry, %s ^9is a higher level admin than you are.\n",
			g_entities[pids[0]].client->pers.netname));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!SetTeam(vic, team, qtrue, -1, -1, qfalse)) {
		SP("^dput: ^9Put team failed^7\n");
		return qfalse;
	}
	return qtrue;
}

qboolean G_shrubbot_pause(gentity_t *ent, int skiparg) {
	G_refPause_cmd(ent, qtrue);
	return qtrue;
}

qboolean G_shrubbot_unpause(gentity_t *ent, int skiparg) {
	G_refPause_cmd(ent, qfalse);
	return qtrue;
}

char* G_SystemInfo(int clientnum) {
	char* buffer;

#ifdef SYSTEM_CHECK
	if (g_entities[clientnum].r.svFlags & SVF_BOT) {
		return buffer = va("%s", "(B) ");
	}

	// system
	if (level.clients[clientnum].sess.nqClientSystemInfo & (128|256)) {
		buffer = va("%s", "(W) ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & (512|1024)) {
		buffer = va("%s", "(L) ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & (2048|4096)) {
		buffer = va("%s", "(M) ");
	}
	else {
		buffer = va("%s", "(?) ");
	}

	// client exec
	if (level.clients[clientnum].sess.nqClientSystemInfo & 1) {
		buffer = va("%s%s", buffer , "2.60b  ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 2) {
		buffer = va("%s%s", buffer , "2.60   ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 4) {
		buffer = va("%s%s", buffer , "OWolf  ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 8) {
		buffer = va("%s%s", buffer , "ETleg  ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 16) {
		buffer = va("%s%s", buffer , "GPL    ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 32) { // FREE
		buffer = va("%s%s", buffer , "TODO   ");
	}
	else if (level.clients[clientnum].sess.nqClientSystemInfo & 64) { // if we can't detect
		buffer = va("%s%s", buffer , "OTHER  ");
	}

	if (level.clients[clientnum].sess.nqClientSystemInfo & 32768) {
		buffer = va("%s%s", buffer , "*untrusted*");
	}
#else
	buffer = "";
#endif
	return buffer;
}

// TODO
qboolean G_shrubbot_listplayers(gentity_t *ent, int skiparg) {
	int i,j;
	gclient_t *p;
	char c[3], t[2]; // color and team letter
	char n[MAX_NAME_LENGTH] = {""};
	char n2[MAX_NAME_LENGTH] = {""};
	char n3[MAX_NAME_LENGTH] = {""};
	char lname[MAX_NAME_LENGTH];
	char lname2[MAX_NAME_LENGTH];
	char* guid;
	char* ip;
	char guid_stub[9];
	char fireteam[2];
	char muted[2];
	int l;
	fireteamData_t *ft;
	int lname_max = 1;
	char lname_fmt[5];
	char cleanLevelName[MAX_NAME_LENGTH];
	int spaces;

	// detect the longest level name length
	for(i=0; g_shrubbot_levels[i]; i++) {
		//SanitizeString(g_shrubbot_levels[i]->name, n, qtrue);
		DecolorString(g_shrubbot_levels[i]->name, n);
		if(strlen(n) > lname_max)
			lname_max = strlen(n);
		Com_sprintf(n, sizeof(n), "%d", g_shrubbot_levels[i]->level);
		if(strlen(n) > lname_max)
			lname_max = strlen(n);
	}

	SBP_begin();
	SBP(va("listplayers: %d players connected:\n", level.numConnectedClients));

	//SBP(" # T L  Levelname               GUID-Stub   M FT Info       Name\n");

	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		Q_strncpyz(t, "S", sizeof(t));
		Q_strncpyz(c, S_COLOR_YELLOW, sizeof(c));
		if(p->sess.sessionTeam == TEAM_ALLIES) {
			Q_strncpyz(t, "B", sizeof(t));
			if (ent) {
				Q_strncpyz(c, S_COLOR_BLUE, sizeof(c));
			}
		}
		else if(p->sess.sessionTeam == TEAM_AXIS) {
			Q_strncpyz(t, "R", sizeof(t));
			if (ent) {
				Q_strncpyz(c, S_COLOR_RED, sizeof(c));
			}
		}

		if(p->pers.connected == CON_CONNECTING) {
			Q_strncpyz(t, "C", sizeof(t));
			if (ent) {
				Q_strncpyz(c, S_COLOR_ORANGE, sizeof(c));
			}
		}
		else if(p->pers.connected != CON_CONNECTED) {
			continue;
		}

		guid 	= level.clients[i].pers.cl_guid;
		ip		= level.clients[i].pers.client_ip;

		// pheno: Fixed wrong GUID in !listplayers output when following a bot
		if (g_entities[i].r.svFlags & SVF_BOT) {
			Q_strncpyz(guid_stub, "OMNIBOT*", sizeof(guid_stub));
		}
		else if (!*guid) {  // IRATA: have never seen this case
            Q_strncpyz(guid_stub, "NONE", sizeof(guid_stub));
        }
		else if (!Q_stricmp(guid, "unknown")) { // IRATA: there is no GUID
			 Q_strncpyz(guid_stub, "unknown", sizeof(guid_stub));
		}
		else {
			for(j=0; j<=8; j++) {
				guid_stub[j] = guid[j+24];
			}
		}

		fireteam[0] = '\0';
		if(G_IsOnFireteam(i, &ft)) {
			Q_strncpyz(fireteam,
				bg_fireteamNames[ft->ident - 1],
				sizeof(fireteam));
		}

		muted[0] = '\0';
		// CHECK/FIXME/TODO - mute check is done by IP ... do we do mute by IP only ?
		if ( p->sess.muted || G_shrubbot_mute_check(ip, guid) ) {
			Q_strncpyz(muted,"M",sizeof(muted));
		}

		l = 0;
		SanitizeString(p->pers.netname, n2, qtrue);
		n[0] = '\0';
		for(j=0; g_shrubbot_admins[j]; j++) {
			if(!Q_stricmp(g_shrubbot_admins[j]->guid, guid)) {
				// tjw: don't gather aka or level info if
				//      the admin is incognito
				if(G_shrubbot_permission(&g_entities[i],
					SBF_INCOGNITO)) {
					break;
				}
				l = g_shrubbot_admins[j]->level;
				SanitizeString(g_shrubbot_admins[j]->name,
					n3, qtrue);
				if(Q_stricmp(n2, n3)) {
					Q_strncpyz(n,
						g_shrubbot_admins[j]->name,
						sizeof(n));
				}
				break;
			}
		}
		lname[0] = '\0';
		for(j=0; g_shrubbot_levels[j]; j++) {
			if(g_shrubbot_levels[j]->level == l)
				Q_strncpyz(lname,
					g_shrubbot_levels[j]->name,
					sizeof(lname));
		}
		if(!*lname)  {
			Com_sprintf(lname,
				sizeof(lname),
				"%i",
				l);
		}
		// gabriel: level names can have color codes in them. Vary the number of
		// spaces in the print mask so the level names have the correct number
		// of leading spaces
		DecolorString(lname, cleanLevelName);
		spaces = lname_max - strlen(cleanLevelName);
		//Com_sprintf(lname_fmt, sizeof(lname_fmt), "%%%is", spaces + strlen(lname));
#if defined(__x86_64__)
		Com_sprintf(lname_fmt, sizeof(lname_fmt), "%%%ds", spaces + (int)strlen(lname)); // IRATA: int might overflow - but we shouldnt have these high values
#else
		Com_sprintf(lname_fmt, sizeof(lname_fmt), "%%%ds", spaces + strlen(lname));
#endif
		Com_sprintf(lname2, sizeof(lname2), lname_fmt, lname);

		if (!ent) { // console with no colors
			SBP(va("%2i %s %-2i '%s' (*%s)  %1s %1s %s %s %s%s%s\n",
				i,
				t,
				l,
				lname2,
				guid_stub,
				muted,
				fireteam,
				G_SystemInfo(i), // TODO: only for special shrubbot flag,
				p->pers.netname,
				(*n) ? "(a.k.a. " : "",
				n,
				(*n) ? ")" : ""
				));
		}
		else {
			SBP(va("%2i %s%s^7 %-2i '%s'^7 (*%s)  ^1%1s ^3%1s^7 %s %s^7 %s%s^7%s\n",
				i,
				c,
				t,
				l,
				lname2,
				guid_stub,
				muted,
				fireteam,
				G_SystemInfo(i), // TODO: only for special shrubbot flag,
				p->pers.netname,
				(*n) ? "(a.k.a. " : "",
				n,
				(*n) ? ")" : ""
				));
		}
	}
	SBP_end();
	return qtrue;
}

qboolean G_shrubbot_listteams(gentity_t *ent, int skiparg) {
	int playerCount[3], pings[3], totalXP[3];
	int i, j;
	gclient_t *p;

	playerCount[0] = pings[0] = totalXP[0] = 0;
	playerCount[1] = pings[1] = totalXP[1] = 0;
	playerCount[2] = pings[2] = totalXP[2] = 0;

	for(i=0; i<level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED) {
			continue;
		}
		if(p->sess.sessionTeam == TEAM_ALLIES) {
			playerCount[2]++;
			pings[2] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[2] += p->sess.skillpoints[j];
			}
		}
		else if(p->sess.sessionTeam == TEAM_AXIS) {
			playerCount[1]++;
			pings[1] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[1] += p->sess.skillpoints[j];
			}
		}
		else if(p->sess.sessionTeam == TEAM_SPECTATOR) {
			playerCount[0]++;
			pings[0] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[0] += p->sess.skillpoints[j];
			}
		}
	}
	SBP_begin();
	SBP(va("Desc       ^4Allies^7(%s^7)      ^1Axis^7(%s^7)       ^3Specs\n",
			teamInfo[TEAM_ALLIES].team_lock ? "^1L" : "^2U",
			teamInfo[TEAM_AXIS].team_lock ? "^1L" : "^2U"));
	SBP("^7---------------------------------------------\n");
	SBP(va("^2Players     ^7%8d     %8d    %8d\n",
			playerCount[2],
			playerCount[1],
			playerCount[0]));
	SBP(va("^2Avg Ping    ^7%8.2f     %8.2f    %8.2f\n",
			playerCount[2] > 0 ? (float)(pings[2]) / playerCount[2] : 0,
			playerCount[1] > 0 ? (float)(pings[1]) / playerCount[1] : 0,
			playerCount[0] > 0 ? (float)(pings[0]) / playerCount[0] : 0));
	SBP(va("^2Map XP      ^7%8d     %8d          --\n",
			level.teamScores[TEAM_ALLIES],
			level.teamScores[TEAM_AXIS]));
	SBP(va("^2Total XP    ^7%8d     %8d    %8d\n",
			totalXP[2],
			totalXP[1],
			totalXP[0]));
	SBP(va("^2Avg Map XP  ^7%8.2f     %8.2f          --\n",
			playerCount[2] > 0 ? (float)(level.teamScores[TEAM_ALLIES]) / playerCount[2] : 0,
			playerCount[1] > 0 ? (float)(level.teamScores[TEAM_AXIS])   / playerCount[1] : 0 ));
	SBP(va("^2Avg Tot XP  ^7%8.2f     %8.2f    %8.2f\n",
			playerCount[2] > 0 ? (float)(totalXP[2]) / playerCount[2] : 0,
			playerCount[1] > 0 ? (float)(totalXP[1]) / playerCount[1] : 0,
			playerCount[0] > 0 ? (float)(totalXP[0]) / playerCount[0] : 0 ));
	SBP_end();
	return qfalse;
}

qboolean G_shrubbot_showbans(gentity_t *ent, int skiparg) {
	time_t	t;
	int		i							= 0;
	int		j							= 0;
	int		ti;
	int		args;
	int		secs;
	int		found						= 0;
	int		max_name					= 1;
	int		max_banner					= 1;
	int		start						= 0;
	int		searching					= 0;
	int		pages						= 0;
	int		displayed					= 0;
	int		skipIsArg					= 1;
	int		isMatch[MAX_SHRUBBOT_BANS];
	char	duration[MAX_STRING_CHARS];
	char	fmt[MAX_STRING_CHARS];
	char	skip[11];
	char	date[11];
	char	searchString[16];
	char	pageStr[11];
	char	*made;
	char	*guid;

	static int first					= 1;
	static int lastViewed[MAX_SHRUBBOT_ADMINS + 1]; // include console admin
	char tmp[MAX_NAME_LENGTH];
	int spacesName;
	int spacesBanner;

	if(!time(&t)) {
		return qfalse;
	}

	if (first) {
		memset (lastViewed, 0, sizeof(lastViewed));
		first = 0;
	}

	memset(isMatch, 0, sizeof(isMatch));

	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;

	args = Q_SayArgc() - skiparg;

	if (ent != NULL && ent-g_entities > MAX_CLIENTS)
		return qfalse;

	if (ent == NULL)
		lastViewed[MAX_SHRUBBOT_ADMINS] = searching ? i : (start + SHRUBBOT_MAX_SHOWBANS);
	else {
		guid = level.clients[ent-g_entities].pers.cl_guid;

		for(j = 0; g_shrubbot_admins[j]; j++) {
			if(!Q_stricmp(g_shrubbot_admins[j]->guid, guid)) {
				lastViewed[j] = searching ? i : (start + SHRUBBOT_MAX_SHOWBANS);
				break;
			}
		}
	}

	if (args == 1) {
		for(i = 0; g_shrubbot_bans[i]; i++) {
			if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
				continue;
			}

			found++;
		}
	}
	else if (args >= 2) {
		Q_SayArgv(1 + skiparg, skip, sizeof(skip));

		if (args > 2) {
			Q_SayArgv(2 + skiparg, searchString, sizeof(searchString));
		}

		if (args > 3) {
			Q_SayArgv(3 + skiparg, pageStr, sizeof(pageStr));
			start = atoi(pageStr);
		}

		if (((isdigit(skip[0]) || skip[0] == '-') && isdigit(skip[1])) || !Q_stricmp(skip, "next") || !Q_stricmp(skip, "prev")) {
			for(i = 0; g_shrubbot_bans[i]; i++) {
				if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
					continue;
				}

				found++;
			}

			if (!Q_stricmp(skip, "next")) {
				start = lastViewed[j] + 1;
			}
			else if (!Q_stricmp(skip, "prev")) {
				start = lastViewed[j] - SHRUBBOT_MAX_SHOWBANS;
			}
			else {
				start = atoi(skip);
				// IRATA: sanity check - display the last one
				if(start >= MAX_SHRUBBOT_BANS)
					start = MAX_SHRUBBOT_BANS -1;
			}
			// tjw: !showbans 1 means start with ban 0
			if(start > 0) {
				start -= 1;
			}
			else if(start < 0) {
				// C3PEG4 !showbans negative fix
				if(found + start < 0)
					start = 0;
				else
				    start = found + start;
			}
			skipIsArg = 0;
		}
		else {
			if (!Q_stricmp(skip, "find")) {
				if (args == 2) {
					SPC("^dusage : ^2!showbans ^9find <match> ^7\n"
					   "^9        (shows banned players matching <match>)\n");
					return qfalse;
				}

			//	SP(va("^dshowbans : ^9searching bans for ^3%s....\n", searchString));
				searching = 1;

				for(i = 0; g_shrubbot_bans[i]; i++)
				{
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
						continue;
					}

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->name), Q_strlwr(searchString))) {
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else if (!Q_stricmp(skip, "banner")) {
				if (args == 2) {
					SPC("^dusage : ^2!showbans ^9banner <match> [start]^7\n"
					   "^9        (shows bans made by admins matching <match>)\n");
					return qfalse;
				}

			//	SP(va("^dshowbans : ^9searching for bans made by ^3%s....\n", searchString));
				searching = 2;

				for(i = 0; g_shrubbot_bans[i]; i++) {
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
						continue;
					}

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->banner), Q_strlwr(searchString))) {
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else if (!Q_stricmp(skip, "reason")) {
				if (args == 2) {
					SPC("^dusage : ^2!showbans ^9reason <match>^7 \n"
					   "^9        (shows bans with reasons matching <match>)\n");
					return qfalse;
				}

			//	SP(va("^dshowbans : ^9searching for bans with reason matching ^3%s....\n", searchString));
				searching = 3;

				for(i = 0; g_shrubbot_bans[i]; i++) {
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
						continue;
					}

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->reason), Q_strlwr(searchString))) {
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else {
				SPC("^dusage : ^2!showbans ^9[start|find <match>|banner <match>|reason <match>]^7\n"
				   "^9        (displays current list of active bans, filtered by argument criteria)\n");

				return qfalse;
			}
		}
	}

	// crapshoot: moved this here
	if(!searching && start > found) {
		SPC(va("^dshowbans: ^9there are only ^2%d ^9active bans!\n", found));
		return qfalse;
	}

	pages = (found / SHRUBBOT_MAX_SHOWBANS) + 1;

	for(i = start; (g_shrubbot_bans[i] && displayed < SHRUBBOT_MAX_SHOWBANS) ; i++) {
		// jet Pilot - ignore entries that don't match our search criteria (if any)
		if (searching && !isMatch[i]) {
			continue;
		}

		// find the longest name
		if(strlen(g_shrubbot_bans[i]->name) > max_name) {
			max_name = strlen(g_shrubbot_bans[i]->name);
		}
		// find longest banner name
		if(strlen(g_shrubbot_bans[i]->banner) > max_banner) {
			max_banner = strlen(g_shrubbot_bans[i]->banner);
		}

		displayed++;
	}

	Com_sprintf(fmt, sizeof(fmt),
		"^F%%4i^7 %%-%is^7 ^F%%-10s^7 %%-%is^7 ^F%%-9s^7 %%s\n",
		max_name, max_banner);

	displayed = 0;

	SBP_begin();
	for( i = start; (g_shrubbot_bans[i] && displayed < SHRUBBOT_MAX_SHOWBANS); i++) {
		// ignore expired bans
		if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) {
			continue;
		}

		// jet Pilot - ignore entries that don't match our search criteria (if any)
		if (searching && !isMatch[i]) {
			continue;
		}

		// tjw: only print out the the date part of made
		date[0] = '\0';

		made = g_shrubbot_bans[i]->made;

		for(ti = 0; *made; ti++) {
			if((ti + 1) >= sizeof(date))
				break;
			if(*made == ' ')
				break;
			date[ti]		= *made;
			date[ti + 1]	= '\0';
			*made++;
		}

		secs = (g_shrubbot_bans[i]->expires - t);
		G_shrubbot_duration(secs, duration, sizeof(duration));
			DecolorString(g_shrubbot_bans[i]->name, tmp);
			spacesName = max_name - strlen(tmp);
			DecolorString(g_shrubbot_bans[i]->banner, tmp);
			spacesBanner = max_banner - strlen(tmp);
#if defined(__x86_64__)
			Com_sprintf(fmt, sizeof(fmt),
				 "^F%%4i^7 %%-%ds^7 ^F%%-10s^7 %%-%ds^7 ^F%%-9s^7 %%s\n",
				spacesName + (int)strlen(g_shrubbot_bans[i]->name), // IRATA: cast int might overflow - but we shouldnt have these high values
				spacesBanner + (int)strlen(g_shrubbot_bans[i]->banner)); // IRATA: cast int might overflow - but we shouldnt have these high values
#else
			Com_sprintf(fmt, sizeof(fmt),
				 "^F%%4i^7 %%-%ds^7 ^F%%-10s^7 %%-%ds^7 ^F%%-9s^7 %%s\n",
				spacesName + strlen(g_shrubbot_bans[i]->name),
				spacesBanner + strlen(g_shrubbot_bans[i]->banner));
#endif
		SBP(va(fmt,
			(i+1),
			g_shrubbot_bans[i]->name,
			date,
			g_shrubbot_bans[i]->banner,
			duration,
			g_shrubbot_bans[i]->reason
			));

		displayed++;
	}

	if (searching == 1) {
		SBP(va("^dshowbans : ^9%i bans found for players matching ^2%s ^9(^7%i ^9Page%s)\n",
			found,
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else if (searching == 2) {
		SBP(va("^dshowbans : ^9%i bans found for admins matching ^2%s ^9(^7%i ^9Page%s)\n",
			found,
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else if (searching == 3) {
		SBP(va("^dshowbans : ^9%i bans found with reasons matching ^2%s ^9(^7%i ^9Page%s)\n",
			found,
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else {
		SBP(va("^dshowbans: ^9showing bans ^7%d ^9- ^7%d ^9of ^7%d ^9(Page ^7%i ^9of ^7%i^9)\n",
			(start + 1),
			((start + SHRUBBOT_MAX_SHOWBANS) > found) ?	found : (start + SHRUBBOT_MAX_SHOWBANS),
			found,
			(start / SHRUBBOT_MAX_SHOWBANS) + 1,
			pages));
	}

	if((start + SHRUBBOT_MAX_SHOWBANS) < found) {
		SBP(va("\n          ^9(type !showbans ^2%s%s%s ^9to continue to page %i)\n",
			(args > 1 && skipIsArg ? va("%s ", skip) : ""),
			(args > 2 ? va("%s ", searchString) : ""),
			"next", //searching ? i : (start + SHRUBBOT_MAX_SHOWBANS + 1),
			(start / SHRUBBOT_MAX_SHOWBANS) + 2));
	}

	lastViewed[j] = searching ? i : (start + SHRUBBOT_MAX_SHOWBANS);
	SBP_end();

	return qtrue;
}

qboolean G_shrubbot_help(gentity_t *ent, int skiparg) {
	int i;

	if(Q_SayArgc() < 2+skiparg) {
		//!help
		int j = 0;
		int count = 0;
		char *str = "";

		for (i=0; g_shrubbot_cmds[i].keyword[0]; i++) {

			if(G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag)) {
				str = va( "%s^f%-12s", str, g_shrubbot_cmds[i].keyword);
				j++;
				count++;
			}
			// show 6 commands per line
			if( j == 6) {
				str = va("%s\n",str);
				j = 0;
			}
		}
		SBP_begin();
		SBP(str);
		// tjw: break str into at least two parts to try to avoid
		//      1022 char limitation
		str = "";
		for (i=0; g_shrubbot_commands[i]; i++) {
			if(!_shrubbot_command_permission(ent,
				g_shrubbot_commands[i]->command)) {

				continue;
			}
			str = va( "%s^f%-12s", str,
				va("%s", g_shrubbot_commands[i]->command));
			j++;
			count++;
			// show 6 commands per line
			if( j == 6) {
				str = va("%s\n",str);
				j = 0;
			}
		}
		if(count) {
			str = va("%s\n",str);
		}
		CP(va("chat \"^dhelp: ^9%i available commands (open console for the full list)\" -1", count));

		SBP( str );
		SBP("^9Type ^2!help ^d[command] ^9for help with a specific command.^7\n");
		SBP_end();

		return qtrue;
	}
	else {
		//!help param
		char param[20];

		Q_SayArgv(1+skiparg, param, sizeof(param));
		SBP_begin();
		for( i=0; g_shrubbot_cmds[i].keyword[0]; i++ ){
			if(	!Q_stricmp(param, g_shrubbot_cmds[i].keyword ) ) {
				if(!G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag)) {
					SBP(va("^dhelp: ^9you have no permission to use '%s'\n",
						g_shrubbot_cmds[i].keyword ) );
					return qfalse;
				}
				SBP( va("^dhelp: ^9help for '^2%s^9':\n", g_shrubbot_cmds[i].keyword ) );
				SBP( va("^dFunction: ^9%s\n", g_shrubbot_cmds[i].function ) );
				SBP( va("^dSyntax: ^2!%s ^9%s\n", g_shrubbot_cmds[i].keyword,
					g_shrubbot_cmds[i].syntax ) );
				SBP( va("^dFlag: ^9'^2%c^9'\n", g_shrubbot_cmds[i].flag) );
				SBP_end();
				return qtrue;
			}
		}
		for( i=0; g_shrubbot_commands[i]; i++ ){
			if(	!Q_stricmp(param, g_shrubbot_commands[i]->command ) ) {
				if(!_shrubbot_command_permission(ent,
					g_shrubbot_commands[i]->command)) {

					SBP(va("^dhelp: ^9you have no permission to use '%s'\n",
						g_shrubbot_commands[i]->command ) );
					SBP_end();
					return qfalse;
				}
				SBP( va("^dhelp: ^9help for '%s':\n",
					g_shrubbot_commands[i]->command ) );
				SBP( va("^dDescription: ^9%s\n",
					g_shrubbot_commands[i]->desc ) );
				SBP( va("^dSyntax: ^9!%s\n",
					g_shrubbot_commands[i]->command) );
				SBP_end();
				return qtrue;
			}
		}
		SBP( va("^dhelp: ^9no help found for '%s'\n", param ) );
		SBP_end();
		return qfalse;
	}
}

qboolean G_shrubbot_admintest(gentity_t *ent, int skiparg) {
	int i, l = 0;
	char *guid;
	qboolean found = qfalse;
	qboolean lname = qfalse;

	if(!ent) {
		SP("^dadmintest: ^9you are on the console.^7\n");
		return qtrue;
	}

	guid = level.clients[ent-g_entities].pers.cl_guid;
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			found = qtrue;
			break;
		}
	}

	if(found) {
		l = g_shrubbot_admins[i]->level;
		for(i=0; g_shrubbot_levels[i]; i++) {
			if(g_shrubbot_levels[i]->level != l)
				continue;
			if(*g_shrubbot_levels[i]->name) {
				lname = qtrue;
				break;
			}
		}
	}

	AP(va("chat \"^dadmintest: ^7%s^9 is a level %d user %s%s%s\" -1",
		ent->client->pers.netname,
		l,
		(lname) ? "^9(^7" : "",
		(lname) ? g_shrubbot_levels[i]->name : "",
		(lname) ? "^9)^7" : ""));
	return qtrue;
}

qboolean G_shrubbot_cancelvote(gentity_t *ent, int skiparg) {

	level.voteInfo.voteNo = level.numConnectedClients;
	CheckVote();
	SP("^dcancelvote: ^9turns out everyone voted ^3No\n");
	return qtrue;
}

qboolean G_shrubbot_passvote(gentity_t *ent, int skiparg) {
	level.voteInfo.voteYes = level.numConnectedClients;
	level.voteInfo.voteNo = 0;
	CheckVote();
	SP("^dpassvote: ^9turns out everyone voted ^3Yes\n");
	return qtrue;
}

qboolean G_shrubbot_spec999(gentity_t *ent, int skiparg) {
	int i;
	gentity_t *vic;

	for(i = 0; i < level.maxclients; i++) {
		vic = &g_entities[i];
		if(!vic->client) continue;
		if(vic->client->pers.connected != CON_CONNECTED) continue;
		if(vic->client->sess.sessionTeam == TEAM_SPECTATOR) continue; // IRATA
		// if(vic->r.svFlags & SVF_BOT) continue; // IRATA: might happen if server is busy
		if(vic->client->ps.ping >= 500 && vic->client->ps.ping <= 999) {
			SetTeam(vic, "s", qtrue, -1, -1, qfalse);
			AP(va("chat \"^dspec999: ^*%s^9 moved to spectators^7\" -1",
				vic->client->pers.netname));
		}
	}
	return qtrue;
}

qboolean G_shrubbot_shuffle(gentity_t *ent, int skiparg) {
	G_shuffleTeams();
	return qtrue;
}

void ClientCleanName( const char *in, char *out, int outSize );

qboolean G_shrubbot_rename(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], *newname, *oldname,err[MAX_STRING_CHARS];
	char userinfo[MAX_INFO_STRING];

	if(Q_SayArgc() < 3+skiparg) {
		SP("^drename usage: ^2!rename ^9[name] [newname]^7\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	newname = Q_SayConcatArgs(2+skiparg);
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("rename: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^drename: ^9sorry, %s ^9is a higher level admin than you are.^7\n",
			g_entities[pids[0]].client->pers.netname));
		return qfalse;
	}
	SP(va("^d!rename: ^9renaming ^7%s ^9to ^7%s\n",name,newname));
	trap_GetUserinfo( pids[0], userinfo, sizeof( userinfo ) );
	oldname = Info_ValueForKey( userinfo, "name" );
	// Send to chat for shame
	AP(va("chat \"^drename: ^*%s ^9has been renamed to ^7%s\"",
		oldname,
		newname));
	Info_SetValueForKey( userinfo, "name", newname);
	trap_SetUserinfo( pids[0], userinfo );
	// jaquboss
	ClientCleanName( newname, g_entities[pids[0]].client->pers.netname, sizeof(level.clients[level.sortedClients[pids[0]]].pers.netname) );
	ClientUserinfoChanged(pids[0]);

	return qtrue;
}

qboolean G_shrubbot_gib(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dgib: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;

		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
			G_Damage(vic, NULL, NULL, NULL, NULL, 500, 0, MOD_UNKNOWN);
			count++;
		}
		AP(va("chat \"^dgib: ^9%d players gibbed^7\"", count));
		return qtrue;
	}

	/*
	Q_SayArgv(1+skiparg, name, sizeof(name));
	*/
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("gib: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dgib: ^9sorry, %s ^9is a higher level admin than you are.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dgib: ^9player must be on a team to be gibbed^7\n");
		return qfalse;
	}

	G_Damage(vic, NULL, NULL, NULL, NULL, 500, 0, MOD_UNKNOWN);
	AP(va("chat \"^dgib: ^7%s ^9was gibbed^7\"", vic->client->pers.netname));
	return qtrue;
}

qboolean G_shrubbot_slap(gentity_t *ent, int skiparg) {
	int			pids[MAX_CLIENTS], dmg;
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char		damage[4], *reason;
	gentity_t	*vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dusage: ^2!slap ^8[name|slot#] [damage] [reason]^7\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name,	 sizeof(name));
	Q_SayArgv(2+skiparg, damage, sizeof(damage));

	dmg = atoi(damage);
	if(dmg <= 0) {
		dmg = 20;
		reason = Q_SayConcatArgs(2 + skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3 + skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("slap: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dslap: ^9sorry, but your intended victim has a higher admin"
			"^9 level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dslap: ^3denied ^9(target must be currently playing).^7\n");
		return qfalse;
	}
	if(vic->health < 1 || vic->client->ps.pm_flags & PMF_LIMBO) {
		SP(va("^dslap: ^3denied ^9(^*%s ^9is not alive).^7\n", vic->client->pers.netname));
		return qfalse;
	}

	if ( vic->health > dmg ) {
		vic->health -= dmg;
	}
	else {
		vic->health = 1;
	}

	// play the sound
	G_AddEvent(vic, EV_GENERAL_SOUND, GAMESOUND_MISC_WHACK);

	// announce to the world
	AP(va("chat \"^dslap: ^*%s ^9was slapped^7\"", vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9slapped you%s%s\"", (ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
		(*reason) ? " ^9because:\n" : "", (*reason) ? reason : ""));
	return qtrue;
}

// don't allow to use these clientcmds to prevent total abuse
#ifdef __CLIENTCMD__
typedef struct clientCmd_s
{
	const char *name;
	float maxvalue; // -1 to not allow any change
	float minvalue;
} clientCmd_t;

static clientCmd_t protectedcmds[] =
{
	{"r_mode",				-1, -1},
	{"r_measureOverdraw",	-1, -1},
	{"r_showimages",		-1, -1}, // we could allow this so you can really fuck up someone's game...
	{"cg_shadows",			 1,  0}, // cos cg_shadows 2 bug
	{"disconnect",			-1, -1},
	{"connect",				-1, -1},
	{"quit",				-1, -1},
	{"cl_pitchspeed",		-1, -1},
	{"cl_autoupdate",		-1, -1},
	{"sensitivity",			-1, -1},
	{"cl_allowDownload",	-1, -1},
	{"cl_wwwDownload",		-1, -1},
	{"cl_doubletapdelay",	-1, -1},
	{"m_pitch",				-1, -1},
	{"m_yaw",				-1, -1},
	{"m_forward",			-1, -1},
	{"m_side",				-1, -1},
	{"cl_maxPing",			-1, -1},
	{"cl_anonymous",		-1, -1},
	{"cl_punkbuster",		-1, -1},
	{"cl_language",			-1, -1},
	{"r_picmip",			-1, -1},
	{"r_roundImagesDown",	-1, -1},
	{"r_rmse",				-1, -1},
	{"r_flares",			-1, -1},
	{"r_subdivisions",		-1, -1},
	{"r_fullscreen",		-1, -1},
	{"r_ignorehwgamma",		-1, -1},
	{"r_overBrightBits",	-1, -1},
	{"r_mapOverBrightBits",	-1, -1},
	{"r_gamma",				 2, 1.3},
	{"s_volume",			-1, -1}, // Meyer - don't allow at all
	{"s_musicvolume",		-1, -1}, // Meyer - don't allow at all
	{"r_texturebits",		-1, -1},
	{"r_colorbits",			-1, -1},
	{"r_stereo",			-1, -1},
	{"r_smp",				-1, -1},
	{"r_depthbits",			-1, -1},
	{"r_stencilbits",		-1, -1},
	{"r_shownormals",		-1, -1},
	{"say",					-1, -1},
	{"say_team",			-1, -1},
	{"say_buddy",			-1, -1},
	{"vsay",				-1, -1},
	{"vsay_team",			-1, -1},
	{"vsay_buddy",			-1, -1},
	{"com_maxFPS",			-1, -1}, // Meyer - don't let anyone change your FPS
	{"bind",				-1, -1},
	{"unbind",				-1, -1},
	{"cg_thirdperson",		-1, -1},
	{"vid_restart",			-1, -1},
#ifdef _DEBUG
	{"cg_gunx",				-1, -1},
	{"cg_guny",				-1, -1},
	{"cg_gunz",				-1, -1},
#endif
	{"cl_maxpackets",		100, 15},
	{"rate",				25000, 15000},
	{"cg_fov",				120, 90},
	{"r_customHeight"		-1, -1},
	{"r_customWidth"		-1, -1},
	{"ENDOFLIST",			-1, -1} // KEEP ME LAST!!!!!!!!!
};

qboolean G_shrubbot_clientcmd(gentity_t *ent, int skiparg) {
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], cmd[MAX_NAME_LENGTH], arg[MAX_STRING_CHARS];
	gentity_t	*vic;
	int			i;

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dclientcmd usage: ^2!clientcmd ^9[name|slot#] [cmd] [args]^7\n");
		return qfalse;
	}

	Q_SayArgv(1 + skiparg, name, sizeof(name));
	Q_SayArgv(2 + skiparg, cmd, sizeof(cmd));
	Q_SayArgv(3 + skiparg, arg, sizeof(arg));

	for ( i=0; protectedcmds[i].name; ++i ) {

		if ( !Q_stricmp(protectedcmds[i].name, "ENDOFLIST") )
			break;

		if ( !Q_stricmp(protectedcmds[i].name, cmd) ){
			if ( protectedcmds[i].maxvalue == -1 ){
				SP("^dclientcmd: this command is not allowed to issue^7\n");
				return qfalse;
			}
			if ( atof(arg) > protectedcmds[i].maxvalue ){
				SP("^dclientcmd: too big value issued for this command^7\n");
				return qfalse;

			}
			if ( atof(arg) < protectedcmds[i].minvalue ){
				SP("^dclientcmd: too small value issued for this command^7\n");
				return qfalse;
			}
		break;
		}

	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("clientcmd: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

		if( _shrubbot_admin_higher( ent, vic ))
	trap_SendServerCommand(vic-g_entities, va("clientcmd \"%s\" \"%s\"", cmd, arg));

	return qtrue;
}
#endif

//G_shrubbot_rocket
// Meyer - The concept was to shoot a rocket from the player.
// core: We always fire 1 rocket now.. and it seems that it only works for the one calling this command.
qboolean G_shrubbot_rocket(gentity_t *ent, int skiparg) {
	vec3_t		dir;
	int			backupWeapon;
	vec3_t		origin;

	if ( !ent || !ent->client ) {
		return qfalse; // jaquboss
	}

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^drocket: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	AngleVectors( ent->client->ps.viewangles, dir, NULL, NULL );
	VectorCopy(ent->client->ps.origin,origin);
	origin[2] += ent->client->ps.viewheight;

	backupWeapon = ent->s.weapon;
	ent->s.weapon = WP_PANZERFAUST;

	fire_rocket(ent, origin, dir, WP_PANZERFAUST);

	ent->s.weapon = backupWeapon;

	return qtrue;
}

qboolean ReviveEntity(gentity_t *ent, gentity_t *traceEnt, qboolean resuscitation);

qboolean G_shrubbot_revive(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^drevive: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		SP("^drevive usage: ^2!revive ^9[name|slot#] [reason]^7\n");
		return qfalse;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("revive: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^drevive: ^9player must be on a team^7\n");
		return qfalse;
	}

	if(vic->health > 0 || vic->client->ps.pm_flags & PMF_LIMBO) {
		SP(va("^drevive: ^7%s ^9is not dead!^7\n", vic->client->pers.netname));
		return qfalse;
	}

	ReviveEntity(ent? ent : NULL, vic, qfalse);

	AP(va("chat \"^drevive: ^*%s ^9was revived^7\"", vic->client->pers.netname));


	CPx(pids[0], va("cp \"%s ^9revived you^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));

	return qtrue;
}

qboolean G_shrubbot_disguise(gentity_t *ent, int skiparg) {
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t	*vic;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^ddisguise: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 3+skiparg) {
		SP("^ddisguise usage: ^2!disguise ^9[name|slot#] [class]^7\n");
		return qfalse;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("disguise: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^ddisguise: ^9player must be on a team!^7\n");
		return qfalse;
	}

	if (ent && ent->client && ent->client->ps.clientNum == vic->client->ps.clientNum) {
		AP(va( "chat \"^ddisguise: ^7%s ^9This admin cheats! ^7\"", vic->client->pers.netname));
	}
	else {
		AP(va( "chat \"^ddisguise: ^7%s ^9was disguised by %s^7\"", vic->client->pers.netname, (ent?ent->client->pers.netname:"^3SERVER CONSOLE") ));
	}

	Cmd_GiveDisguise_f(vic, 1+skiparg);

	return qtrue;
}

qboolean G_shrubbot_poison(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dpoison: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;

		for( it = 0; it < level.numConnectedClients; it++ ) {

			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) || !(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				continue;
			}
			vic->client->ps.eFlags |= EF_POISONED;
			// core: the poisoner is the one issuing the poison-command..
			// If the console is the poisoner, the message will say: tasted his own poison.
			vic->client->pmext.poisonerEnt = (ent)? ent->s.number : vic->client->ps.clientNum;
			vic->client->pmext.poisonAmnt = 1;
			// IlDuca - fix : set poisonTime
			vic->client->poisonTime = level.time;

			TossClientItems(vic);

			count++;
		}
		AP(va("chat \"^dpoison: ^9%d players poisoned^7\"", count));
		return qtrue;
	}


	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("poison: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dpoison: ^9sorry, but your intended victim has a higher admin level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dpoison: ^9player must be on a team^7\n");
		return qfalse;
	}

	vic->client->ps.eFlags |= EF_POISONED;
	// core: the poisoner is the one issuing the poison-command..
	// If the console is the poisoner, the message will say: tasted his own poison.
	vic->client->pmext.poisonerEnt = (ent)? ent->s.number : vic->client->ps.clientNum;
	vic->client->pmext.poisonAmnt = 1;
	// IlDuca - fix : set poisonTime
	vic->client->poisonTime = level.time;

	TossClientItems(vic);

	AP(va("chat \"^dpoison: ^7%s ^9was poisoned^7\"", vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9poisoned you^7%s%s\"",
			((ent && ent->client)?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));

	return qtrue;
}

// annoying players cmd
qboolean G_shrubbot_dropweapon(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^ddw: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {

			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;

			TossClientItems(vic);

			count++;
		}
		AP(va("chat \"^ddw: ^9%d players drop their weapons^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("dw: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^ddw: ^9sorry, but your intended victim has a higher admin level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^ddw: ^9player must be on a team^7\n");
		return qfalse;
	}

	TossClientItems(vic);

	AP(va("chat \"^ddw: ^7%s ^9was forced to drop weapons^7\"", vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9forced you to drop weapons^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));

	return qtrue;
}

qboolean G_shrubbot_glow(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dglow: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->ps.eFlags |= EF_GLOW; // jaquboss fixme, what a waste
			count++;
		}
		AP(va("chat \"^dglow: ^9%d players are now glowing^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dglow: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dglow: ^9sorry, but your intended victim has a higher admin level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dglow: ^9player must be on a team^7\n");
		return qfalse;
	}

	vic->client->ps.eFlags ^= EF_GLOW;

	if ( vic->client->ps.eFlags & EF_GLOW)
	AP(va("chat \"^dglow: ^7%s ^9is now glowing^7\"",vic->client->pers.netname));
	else
	AP(va("chat \"^dglow: ^7%s ^9stopped glowing^7\"",vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9changed your glow status^7%s%s\"",
		(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_pants(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dpants: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->ps.eFlags |= EF_PANTSED;
			count++;
		}
		AP(va("chat \"^dpants: ^9%d players pantsed^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dpants: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dpants: ^9sorry, but your intended victim has a higher admin^"
			" level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dpants: ^9player must be on a team^7\n");
		return qfalse;
	}

	vic->client->ps.eFlags |= EF_PANTSED;

	AP(va("chat \"^dpants: ^7%s ^9was pantsed^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9pantsed you^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_freeze(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dfreeze: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->freezed = qtrue;
				vic->takedamage = qfalse;
			count++;
		}
		AP(va("chat \"^dfreeze: ^9%d players are frozen^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dfreeze: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dfreeze: ^9sorry, but your intended victim has a higher admin^"
			" level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dfreeze: ^9player must be on a team^7\n");
		return qfalse;
	}

	vic->client->freezed = qtrue;
	vic->takedamage = qfalse;

	AP(va("chat \"^dfreeze: ^7%s ^9is frozen^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9froze you^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_unfreeze(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->freezed = qfalse;
				vic->takedamage = qtrue;
			count++;
		}
		AP(va("chat \"^dunfreeze: ^9%d players are unfrozen^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dunfreeze: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dunfreeze: ^9sorry, but your intended victim has a higher admin^"
			" level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dunfreeze: ^9player must be on a team^7\n");
		return qfalse;
	}

	vic->client->freezed = qfalse;
	vic->takedamage = qtrue;

	AP(va("chat \"^dunfreeze: ^7%s ^9is unfrozen^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9unfroze you^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_burn(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dburn: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				G_BurnMeGood(vic, vic, NULL);
			count++;
		}
		AP(va("chat \"^dburn: ^9%d players burned^7\"", count));
		return qtrue;
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dburn: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^burn: ^9sorry, but your intended victim has a higher admin^"
			" level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dburn: ^9player must be on a team^7\n");
		return qfalse;
	}

	G_BurnMeGood(vic, vic, NULL);
	AP(va("chat \"^dburn: ^7%s ^9was set ablaze^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9burned you^7%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_warn(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 3+skiparg) {
		SP("^dwarn usage: ^2!warn ^9[name|slot#] [reason]^7\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dwarn: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dwarn: ^9sorry, but your intended victim has a higher admin"
			" level than you do.^7\n");
		return qfalse;
	}

	G_AddEvent(vic, EV_GENERAL_SOUND,GAMESOUND_MISC_REFEREE);

	AP(va("chat \"^dwarn: ^7%s ^9was warned^7\"",
			vic->client->pers.netname));
	// tjw: can't do color code for the reason because the
	//      client likes to start new lines and lose the
	//      color with long cp strings
	CPx(pids[0], va("cp \"%s ^dwarned ^9you because:\n%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			reason));
	return qtrue;
}

qboolean G_shrubbot_give(gentity_t *ent, int skiparg) {
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char		*item;
	char		*amt;
	char		*amt2;
	gentity_t	*vic;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dgive: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dgive usage: ^2!give ^9[name|slot#] [item] [args]^7\n");
		return qfalse;
	}

	Q_SayArgv(1 + skiparg, name, sizeof(name));
	item = Q_SayConcatArgs(2 + skiparg);

	amt = Q_SayConcatArgs(3 + skiparg);
 	amt2 = Q_SayConcatArgs(4 + skiparg);

	if(ClientNumbersFromString(name, pids) != 1){
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("give: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP(va("^dgive: ^3denied ^9(^7%s ^9is not in game)^7\n", vic->client->pers.netname));
		return qfalse;
	}

	if ( ExecGive( vic, item, amt?amt:"", amt2?amt2:"") ) {
		if (ent && ent->client && ent->client->ps.clientNum == vic->client->ps.clientNum) {
			AP(va( "chat \"^dgive: ^7%s ^7%s^9 This admin cheats! ^7\"", vic->client->pers.netname, item ));
		}
		else {
			AP(va( "chat \"^dgive: ^7%s ^9received ^7%s ^9from ^7%s^7\"", vic->client->pers.netname, item, (ent?ent->client->pers.netname:"^3SERVER CONSOLE") ));
		}
	}

	return qtrue;
}

/* IRATA: removed ... way to annoying + fills bufferered sound scripts, global channel is blocked
qboolean G_shrubbot_news(gentity_t *ent, int skiparg) {
	char mapname[MAX_STRING_CHARS];

	if(Q_SayArgc() < 2+skiparg) {
		Q_strncpyz(mapname, level.rawmapname,
				sizeof(mapname));
	}
	else {
		Q_SayArgv(1+skiparg, mapname, sizeof(mapname));
	}

	G_globalSound(va("sound/vo/%s/news_%s.wav",mapname,mapname));

	return qtrue;
}
*/

qboolean G_shrubbot_lock(gentity_t *ent, int skiparg) {
	return G_shrubbot_lockteams(ent, skiparg, qtrue);
}

qboolean G_shrubbot_unlock(gentity_t *ent, int skiparg) {
	return G_shrubbot_lockteams(ent, skiparg, qfalse);
}

qboolean G_shrubbot_lockteams(gentity_t *ent, int skiparg, qboolean toLock) {
	char team[4];
	char command[MAX_SHRUBBOT_CMD_LEN], *cmd;

	Q_SayArgv(skiparg, command, sizeof(command));
	cmd = command;
	if(*cmd == '!')
		*cmd++;

	if(Q_SayArgc() < 2+skiparg) {
		SP(va("^d%s usage: ^2!%s ^9r|b|s|all^7\n",cmd,cmd));
		return qfalse;
	}
	Q_SayArgv(1+skiparg, team, sizeof(team));

	if ( !Q_stricmp(team, "all") ) {
		teamInfo[TEAM_AXIS].team_lock =
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse;
		teamInfo[TEAM_ALLIES].team_lock =
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse;
		G_updateSpecLock(TEAM_AXIS,
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse);
		G_updateSpecLock(TEAM_ALLIES,
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse);
		if( toLock ) {
			level.server_settings |= CV_SVS_LOCKSPECS;
		}
		else {
			level.server_settings &= ~CV_SVS_LOCKSPECS;
		}
		AP(va("chat \"^d%s: ^9All teams %sed^7\" -1", cmd, cmd));
	}
	else if ( !Q_stricmp(team, "r") ) {
		teamInfo[TEAM_AXIS].team_lock =
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse;
		AP(va("chat \"^d%s: ^9Axis team %sed^7\" -1", cmd, cmd));
	}
	else if ( !Q_stricmp(team, "b") ) {
		teamInfo[TEAM_ALLIES].team_lock =
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse;
		AP(va("chat \"^d%s: ^9Allied team %sed^7\" -1", cmd, cmd));
	}
	else if ( !Q_stricmp(team, "s") ) {
		G_updateSpecLock(TEAM_AXIS,
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse);
		G_updateSpecLock(TEAM_ALLIES,
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse);
		if( toLock ) {
			level.server_settings |= CV_SVS_LOCKSPECS;
		}
		else {
			level.server_settings &= ~CV_SVS_LOCKSPECS;
		}
		AP(va("chat \"^d%s: ^9Spectators %sed^7\" -1", cmd, cmd));
	} else {
		SP(va("^d%s usage: ^2!%s ^9r|b|s|all^7\n",cmd,cmd));
		return qfalse;
	}

	if( toLock ) {
		level.server_settings |= CV_SVS_LOCKTEAMS;
	} else {
		level.server_settings &= ~CV_SVS_LOCKTEAMS;
	}
	trap_SetConfigstring(CS_SERVERTOGGLES,
			va("%d", level.server_settings));

	return qtrue;
}

// created by: dvl
qboolean G_shrubbot_lol(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], numNades[4];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, nades = 0;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dnade: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	// IlDuca - patheticcockroach's fix for !nade overflow bug
	if( level.time - lol_flood_control < 600) {
		SP("^dnade: ^9denied by entities overflow protection.^7\n");
		return qfalse;
	}

	lol_flood_control = level.time;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;
	else if(Q_SayArgc() >= 3+skiparg) {
		Q_SayArgv( 2+skiparg, numNades, sizeof( numNades ) );
		nades = atoi( numNades );
		if( nades < 1 )
			nades = 1;
		else if( nades > SHRUBBOT_MAX_LOL_NADES )
			nades = SHRUBBOT_MAX_LOL_NADES;
	}
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;

		// IlDuca - patheticcockroach's fix for !nade overflow bug
		if( G_GentitiesAvailable() - level.numConnectedClients * nades * 2 < MIN_SPARE_GENTITIES ) {
			SP("^dnade: ^9too many entities, use fewer nades or nade fewer players.^7\n");
			return qfalse;
		}

		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher(ent, vic) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
			if( nades )
				G_createClusterNade( vic, 2 );//nades );
			else
				G_createClusterNade( vic, 1 );
			count++;
		}
		AP(va("chat \"^dnade: ^9%d players naded^7\"", count));
		return qtrue;
	}

	pcount = ClientNumbersFromString(name, pids);

	if(pcount > 1) {

		int it;

		// IlDuca - patheticcockroach's fix for !nade overflow bug
		if( G_GentitiesAvailable() - pcount * nades * 3 < MIN_SPARE_GENTITIES ) {
			SP("^dnade: ^9too many entities, use fewer nades or nade fewer players.^7\n");
			return qfalse;
		}

		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP(va("^dnade: ^9sorry, but ^7%s^9 has a higher "
					"admin level than you do.^7\n",
					vic->client->pers.netname));
				continue;
			}
			else if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
					vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^dnade: ^7%s^9 must be on a "
						"team to be naded^7\n",
						vic->client->pers.netname));
				continue;
			}

			if( nades )
				G_createClusterNade( vic, nades );
			else
				G_createClusterNade( vic, 8 );
			AP(va("chat \"^dnade: ^7%s\"",
					vic->client->pers.netname));
		}
		return qtrue;
	}
	else if( pcount < 1 ) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dnade: ^9%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dnade: ^9sorry, but your intended victim has a higher "
			"admin level than you do.^7\n");
		return qfalse;
	}

	if( nades )
		G_createClusterNade( vic, 2);//nades );
	else
		G_createClusterNade( vic, 1 );
	AP(va("chat \"^dnade: ^7%s^7\"", vic->client->pers.netname));
	return qtrue;

}

// Created by: dvl
qboolean G_shrubbot_pop( gentity_t *ent, int skiparg ) {
	vec3_t dir = { 5, 5, 5 };
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dpop: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;

		for( it = 0; it < level.numConnectedClients; ++it ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES) ||
				 (vic->client->ps.eFlags & EF_HEADSHOT))
				continue;
			// tjw: actually take of the players helmet instead of
			//      giving all players an unlimited supply of
			//      helmets.
			vic->client->ps.eFlags |= EF_HEADSHOT;
			G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
			count++;
		}
		AP(va("chat \"^dpop: ^9%d players popped\"", count));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP( va("^dpop: ^9sorry, but ^7%s^9 has a higher admin"
					" level than you do.^7\n",
					vic->client->pers.netname));
				continue;
			}
			if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 		vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^dpop: ^7%s^9 must be on a team"
					"^9to be popped^7\n",
					vic->client->pers.netname));
				continue;
			}
			G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
			AP(va("chat \"^dpop: ^7%s ^9lost his hat^7\"",
				vic->client->pers.netname));
		}
		return qtrue;
	}
	else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dpop: ^7%s^7\n", err));
		return qfalse;
	}

	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, vic)) {
		SP( "^dpop: ^9sorry, but your intended victim has a higher "
			"^9admin level than you do.^7\n" );
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dpop: ^9player must be on a team to be popped^7\n");
		return qfalse;
	}

	G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
	AP(va("chat \"^dpop: ^7%s ^9lost his hat^7\" -1",
			vic->client->pers.netname));
	return qtrue;
}

// Created by: dvl
qboolean G_shrubbot_pip( gentity_t *ent, int skiparg ) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dpip: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it, count=0;

		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
			G_MakePip( vic );
			count++;
		}
		AP(va("chat \"^dpip: ^9%d players pipped^7\"", count));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;

		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP( va("^dpip: ^9sorry, but ^7%s^9 has a higher admin"
					"level than you do.\n",
					vic->client->pers.netname));
				continue;
			}
			if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 		vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^dpip: ^7%s^9 must be on a team"
					" to be pipped^7\n",
					vic->client->pers.netname));
				continue;
			}
			G_MakePip( vic );
			AP(va("chat \"^dpip: ^7%s ^9was pipped^7\"",
					vic->client->pers.netname));
		}
		return qtrue;
	}
	else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dpip: ^7%s\n", err));
		return qfalse;
	}

	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dpip: ^9sorry, but your intended victim has a higher"
			" admin level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^dpip: ^9player must be on a team to be pipped\n");
		return qfalse;
	}

	G_MakePip( vic );
	AP(va("chat \"^dpip: ^7%s ^9was pipped^7\"", vic->client->pers.netname));
	return qtrue;
}

qboolean G_shrubbot_reset(gentity_t *ent, int skiparg) {
	char command[MAX_SHRUBBOT_CMD_LEN];

	Q_SayArgv(skiparg, command, sizeof(command));
	if(Q_stricmp(command, "reset")) {
		Svcmd_ResetMatch_f(qtrue, qtrue);
	}
	else {
		Svcmd_ResetMatch_f(qfalse, qtrue);
	}
	return qtrue;
}

qboolean G_shrubbot_swap(gentity_t *ent, int skiparg) {
	Svcmd_SwapTeams_f();
	AP("chat \"^dswap: ^9Teams were swapped^7\"");
	return qtrue;
}

qboolean G_shrubbot_fling(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char fling[9], pastTense[9];
	char *flingCmd;
	gentity_t *vic;
	int flingType=-1; // 0 = fling, 1 = throw, 2 = launch
	int i, count = 0, pcount;
	qboolean doAll = qfalse;


	Q_SayArgv(0+skiparg, fling, sizeof(fling));
	if(fling[0] == '!') {
		flingCmd = &fling[1];
	}
	else {
		flingCmd = &fling[0];
	}

	if(!Q_stricmp(flingCmd, "throw")) {
		flingType = 1;
		Q_strncpyz(pastTense, "thrown", sizeof(pastTense));
	}
	else if (!Q_stricmp(flingCmd, "launch")) {
		flingType = 2;
		Q_strncpyz(pastTense, "launched", sizeof(pastTense));
	}
	else if (!Q_stricmp(flingCmd, "fling")) {
		flingType = 0;
		Q_strncpyz(pastTense, "flung", sizeof(pastTense));
	}

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP(va("^d%s: ^9not allowed during intermission.^7\n", flingCmd));
		return qfalse;
	}

	if( Q_SayArgc() < 2+skiparg) {
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));

	if( doAll ) {
		for( i = 0; i < level.numConnectedClients; i++ ) {
			vic = g_entities + level.sortedClients[i];
			if( !_shrubbot_admin_higher( ent, vic ) )
				continue;
			count += G_FlingClient( vic, flingType );
		}
		AP(va("chat \"^d%s: ^9%d players %s^7\"",
					flingCmd, count, pastTense));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		for( i = 0; i < pcount; i++) {
			vic = &g_entities[pids[i]];
			if( !_shrubbot_admin_higher( ent, vic ) )
				continue;
			count += G_FlingClient( vic, flingType );
		}
		AP(va("chat \"^d%s: ^9%d players %s^7\"",
					flingCmd, count, pastTense));
		return qtrue;
	} else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^d%s: ^9%s^7\n", flingCmd, err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^d%s: ^9sorry, but your intended victim has a higher"
			" admin level than you do.^7\n",flingCmd));
		return qfalse;
	}

	if(G_FlingClient( vic, flingType ))
		AP(va("chat \"^d%s: ^7%s ^9was %s^7\"",
				flingCmd,
				vic->client->pers.netname,
				pastTense));
	return qtrue;
}

qboolean G_shrubbot_disorient(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^ddisorient: ^9not allowed during intermission.^7\n");
		return qfalse;
	}

	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !disorient [name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("disorient: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("disorient: sorry, but your intended victim has a"
			"higher admin level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("disorient: player must be on a team\n");
		return qfalse;
	}
	if(vic->client->pmext.disoriented) {
		SP(va("disorient: %s^7 is already disoriented\n",
			vic->client->pers.netname));
		return qfalse;
	}
	vic->client->pmext.disoriented = qtrue;
	AP(va("chat \"^ddisorient: ^7%s ^9is disoriented\" -1",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^7disoriented you%s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? " ^9because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_orient(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !disorient [name|slot#]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("orient: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!vic->client->pmext.disoriented) {
		SP(va("^dorient: ^7%s^9 is not currently disoriented\n",
			vic->client->pers.netname));
		return qfalse;
	}
	if(vic->client->ps.eFlags & EF_POISONED) {
		SP(va("^dorient: ^7%s^9 is poisoned at the moment\n",
			vic->client->pers.netname));
		return qfalse;
	}
	vic->client->pmext.disoriented = qfalse;
	AP(va("chat \"^dorient: ^7%s ^9is no longer disoriented\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9oriented you\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE")));
	return qtrue;
}

qboolean G_shrubbot_resetxp(gentity_t *ent, int skiparg) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dusage: ^2!resetxp ^9[name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = ConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("resetxp: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dresetxp: ^9sorry, but your intended victim has a higher "
			"^9 admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	G_ResetXP(vic);

	AP(va("chat \"^dresetxp: ^9XP have been reset for player %s\"",
			vic->client->pers.netname));

	if(ent && (ent - g_entities) == pids[0])
		return qtrue;

	CPx(pids[0], va("cp \"%s^9 has reset your XP %s%s\"",
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_resetmyxp(gentity_t *ent, int skiparg) {
	if(!ent || !ent->client) {
		return qfalse;
	}

	G_ResetXP(ent);
	SP("^dresetmyxp: ^9you have reset your XP\n");
	return qtrue;
}

qboolean G_shrubbot_nextmap(gentity_t *ent, int skiparg) {

	/*
	// ignore in intermission
	if ( level.intermissiontime ) {
		SP("^dnextmap: ^9not allowed during intermission.^7\n");
		return qfalse;
	}
	*/

	// copied (basically) from G_Nextmap_v() in g_vote.c
	switch (g_gametype.integer) {
		case GT_WOLF_CAMPAIGN:
			{
				g_campaignInfo_t *campaign = &g_campaigns[level.currentCampaign];

				if( campaign->current + 1 < campaign->mapCount ) {
					 trap_Cvar_Set("g_currentCampaignMap", va( "%i", campaign->current + 1));
					 trap_SendConsoleCommand(EXEC_APPEND,
							 va( "map %s\n", campaign->mapnames[campaign->current + 1]));
					 AP("cp \"^3*** Loading next map in campaign ***\n\"");
				}
				else {
					 // Load in the nextcampaign
					 trap_SendConsoleCommand(EXEC_APPEND, "vstr nextcampaign\n");
					 AP("cp \"^3*** Loading next campaign ***\n\"");
				}
			}
			break;

		case GT_WOLF_MAPVOTE:
			if ( g_gamestate.integer == GS_PLAYING // don't do in intermission // check warmup, warmup-countdown?
					&& g_mapVoteFlags.integer & MAPVOTE_NEXTMAP_VOTEMAP) {
				AP("cp \"^3*** Prepare for next map voting ***\n\"");
				LogExit( "Nextmap passed" );
			}
			else {
				// Load in the nextmap (common behaviour)
				AP("cp \"^3*** Loading next map ***\n\"");
				trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");
			}
			break;

		default:
			AP("cp \"^3*** Loading next map ***\n\"");
			// Load in the nextmap
			trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");
			break;
	}

	 AP("chat \"^dnextmap: ^9Next map was loaded\"");
	 return qtrue;
}

/* Dens: (SPC)
Exactly the same as G_shrubbot_print, but this time the text is printed in the
chat and console area of the client instead of console only. (If you want to
switch from SP to SPC, you need to remove the \n at the end, otherwise the text
isn't displayed right).
*/
void G_shrubbot_print_chat(gentity_t *ent, char *m) {
	char temp[MAX_STRING_CHARS], *p, *s;

	if(ent){
		Q_strncpyz(temp, m ,sizeof(temp));
		s = p = temp;
		while(*p) {
			if(*p == '\n') {
				*p++ = '\0';
				CP(va("chat \"%s\" -1", s));
				s = p;
			} else {
				p++;
			}
		}
		CP(va("chat \"%s\" -1", s));
	}
	else {
		char m2[MAX_STRING_CHARS];

		DecolorString(m, m2);
		G_Printf(va("%s\n",m2));
	}
}

/*
 * This function facilitates the SP define.  SP() is similar to CP except that
 * it prints the message to the server console if ent is not defined.
 */
void G_shrubbot_print(gentity_t *ent, char *m) {
	if(ent) {
		CP(va("print \"%s\"", m));
	}
	else {
		char m2[MAX_STRING_CHARS];

		DecolorString(m, m2);
		G_Printf(m2);
	}
}

void G_shrubbot_buffer_begin() {
	bigTextBuffer[0] = '\0';
}

void G_shrubbot_buffer_end(gentity_t *ent) {
	SP(bigTextBuffer);
}

void G_shrubbot_buffer_print(gentity_t *ent, char *m) {
	// gabriel: Slightly different processing for console prints (engine does
	// not like to send huge rcon replies)
	if (!ent) {
		// Remove color from console reply string (instead of waiting for
		// G_shrubbot_print() to do it) to spread console responses over as few
		// print replies as possible
		char m2[MAX_STRING_CHARS];
		DecolorString(m, m2);

		// We use a "magic number" for console prints.  If someone can get an
		// exact number for the max char limit that can be sent via rcon print
		// reply, we would be grateful :)
		if(strlen(m2) + strlen(bigTextBuffer) > 239) {
			SP(bigTextBuffer);
			bigTextBuffer[0] = '\0';
		}
		Q_strcat(bigTextBuffer, sizeof(bigTextBuffer), m2);
	}
	else {
		// tjw: 1022 - strlen("print 64 \"\"") - 1
		if(strlen(m) + strlen(bigTextBuffer) >= 1009) {
			SP(bigTextBuffer);
			bigTextBuffer[0] = '\0';
		}
		Q_strcat(bigTextBuffer, sizeof(bigTextBuffer), m);
	}
}

void G_shrubbot_cleanup() {
		int i = 0;

		for(i=0; g_shrubbot_levels[i]; i++) {
			free(g_shrubbot_levels[i]);
			g_shrubbot_levels[i] = NULL;
		}
		for(i=0; g_shrubbot_admins[i]; i++) {
			free(g_shrubbot_admins[i]);
			g_shrubbot_admins[i] = NULL;
		}
		for(i=0; g_shrubbot_bans[i]; i++) {
			free(g_shrubbot_bans[i]);
			g_shrubbot_bans[i] = NULL;
		}
		for(i=0; g_shrubbot_mutes[i]; i++) {
			free(g_shrubbot_mutes[i]);
			g_shrubbot_mutes[i] = NULL;
		}
		for(i=0; g_shrubbot_commands[i]; i++) {
			free(g_shrubbot_commands[i]);
			g_shrubbot_commands[i] = NULL;
		}
}

/*
====================
G_shrubbot_adminlevel
--------------------
Jaybird

Gets level via GUID
====================
*/
static int G_shrubbot_adminlevel( gentity_t *ent ) {
	int i;
	// Get GUID and hash
	char *guid = level.clients[ent-g_entities].pers.cl_guid;
	long hash;

// TODO
	// Do not run if no GUID
	if( !*guid ) {
		return 0;
	}

	hash = BG_StringHashValue( guid );

	for( i = 0; g_shrubbot_admins[i]; i++ ) {

		// Problematic admin
		if( !g_shrubbot_admins[i]->guid || !*g_shrubbot_admins[i]->guid )
			continue;

		// Match?
		if( !Q_stricmp( g_shrubbot_admins[i]->guid, guid )) {
			return g_shrubbot_admins[i]->level;
		}
	}
	return 0;
}

/*
====================
!finger
--------------------
Jaybird
====================
*/
qboolean G_shrubbot_finger( gentity_t *ent, int skiparg ) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *guid, *ip, *tmp;
	gentity_t *vic;

	if( Q_SayArgc() != 2 + skiparg ) {
		SP("^dusage: ^2!finger ^9[name|slot#]\n");
		return qfalse;
	}

	Q_SayArgv( 1 + skiparg, name, sizeof(name) );

	if( ClientNumbersFromString( name, pids ) != 1 ) {
		G_MatchOnePlayer( pids, err, sizeof(err) );
		SP( va( "^dfinger: ^2%s\n", err ));
		return qfalse;
	}

	vic 	= &g_entities[pids[0]];
	guid 	= vic->client->pers.cl_guid;
	ip 		= vic->client->pers.client_ip;

	// Strip port off of ip.
	tmp = ip;
	while( *tmp ) {
		if( *tmp == ':' ) {
			*tmp = 0;
			break;
		}
		tmp++;
	}

	SP( va( "^dInformation about ^7%s^d:\n", vic->client->pers.netname ));
	SP( va( "^dSlot:    ^2%i\n", vic->client->ps.clientNum ));
	SP( va( "^dAdmin:   ^2%i\n", G_shrubbot_adminlevel( vic )));
	SP( va( "^dGUID:    ^2%s\n", guid ));

#ifdef AUTO_GUID
	SP( va( "^dNQ-GUID: ^2%s\n", level.clients[vic->client->ps.clientNum].sess.nqKeyInfo.nquid) );
#endif

#ifdef SYSTEM_CHECK	
	SP( va( "^dSystem:  ^2%s\n", G_SystemInfo(vic->client->ps.clientNum) ));
#endif

#ifdef HW_BAN
	if ( nq_security.integer & NQS_USE_HW_BANS )
		SP( va( "^dHWGUID:  ^2%s\n", vic->client->pers.cg_hwguid ));
#endif // HW_BAN

	SP( va( "^dIP:      ^2%s\n", ip ));

	return qtrue;
}

/*
====================
!uptime
--------------------
Jaybird
====================
*/
qboolean G_shrubbot_uptime( gentity_t *ent, int skiparg ) {
	int rawtime;
	int days;
	int hours;
	int minutes;

	rawtime = level.time / SECONDS_1;
	days = rawtime / (60 * 60 * 24);
	rawtime -= (days * 60 * 60 * 24);
	hours = rawtime / (60 * 60);
	rawtime -= (hours * 60 * 60);
	minutes = rawtime / 60;

	SPC(va("^duptime: ^2%i days, %i hours, %i minutes.", days, hours, minutes));
	return qtrue;
}

/*
 * IRATA: 	displays a server-side list of disabled and restricted weapons (todo: implement this as client cmd?)
 *			I don't use arguments (weapon numbers) instead we only print infos
 *			about weapons which are disabled or restricted
 *			see g_svcmds.c "weap" cmd
 */
qboolean G_shrubbot_weapons(gentity_t *ent, int skiparg) {
	int i;
	int 	count = 0;
	char	minplayers[4], maxweap[4], percentage[4];

	SBP_begin();
	SBP(va("weapons: %d players in game:\n", level.numPlayingClients));

	if (!ent) {
		SBP("Used values from the config - see also server cmd /weap:\n");
	}

	// show all restricted weapons..
	for (i=WP_NONE+1; i<WP_NUM_WEAPONS; ++i) {
		if (!weapon_restrictions[i].restricted) continue;
		// print header..
		if (count == 0) {
			SBP("Weapon        Min Max Per Status\n");
			SBP("------------------------------------------------------------\n");
		}
		++count;
		// make up some strings..
		// not restricted? == empty string, which makes it more clear to read.
		if (weapon_restrictions[i].disabled) {
			SBP(va("%-25s %s\n", BG_Weapons[i].statname, "^1disabled by admin^7"));
		}
		else {
			// TODO: display what's required to get the weapon ??? ...

			if (weapon_restrictions[i].minplayers <= 1) {
				Q_strncpyz(minplayers, "    ", sizeof(minplayers));
			}
			else {
				Q_strncpyz(minplayers, va("%3i",weapon_restrictions[i].minplayers), sizeof(minplayers));
			}
			if (weapon_restrictions[i].maxweap <= 0) {
				Q_strncpyz(maxweap, "    ", sizeof(maxweap));
			}
			else {
				Q_strncpyz(maxweap, va("%3i",weapon_restrictions[i].maxweap), sizeof(maxweap));
			}
			if (weapon_restrictions[i].percentage <= 0 || weapon_restrictions[i].percentage >= 100) {
				Q_strncpyz(percentage, "    ", sizeof(percentage));
			}
			else {
				Q_strncpyz(percentage, va("%3i",weapon_restrictions[i].percentage), sizeof(percentage));
			}

			if (ent) {
				SBP(va("%-13s %s %s %s %s\n", BG_Weapons[i].statname, minplayers, maxweap, percentage, G_IsWeaponDisabled( ent, i, qtrue )? "^1weapon is restricted by rule^7":"^2weapon is available^7"));
			}
			else {
				SBP(va("%-13s %s %s %s %s\n", BG_Weapons[i].statname, minplayers, maxweap, percentage, "cant check rule - not connected"));
			}

		}
	}
	if (count == 0) {
		SBP("There are no weapon restrictions.\n");
	}
	else {
		SBP("------------------------------------------------------------\n");
		SBP(va("In total %d restriction rules set.\n", count));
	}

	SBP_end();
	return qtrue;
}
