/*
 * g_shrubbot.c
 *
 * This code is the original work of Tony J. White
 *
 * The functionality of this code mimics the behaviour of the currently
 * inactive project shrubmod (http://www.etstats.com/shrubet/index.php?ver=2)
 * by Ryan Mannion.   However, shrubmod was a closed-source project and 
 * none of it's code has been copied, only it's functionality.
 *
 * You are free to use this implementation of shrubbot in you're 
 * own mod project if you wish.
 *
 */

#include "g_local.h"
#include "../../etmain/ui/menudef.h"

char *G_Shortcuts(gentity_t *ent, char *text); 

extern char bigTextBuffer[100000];

static const struct g_shrubbot_cmd g_shrubbot_cmds[] = 
{
	{"readconfig",	G_shrubbot_readconfig,	'G', 0,				"reloads the shrubbot config file and refreshes user flags", ""},
	{"time",		G_shrubbot_time,		'C', 0,				"displays the local time", "[^3name|slot#^9] (^hreason^9)"},
	{"setlevel",	G_shrubbot_setlevel,	's', 0,				"sets the admin level of a player", "[^3name|slot#^9] [^3level^9)"},
	{"kick",		G_shrubbot_kick,		'k', 0,				"kick a player with an optional reason", "^9(^hcommand^9)"},
	{"ban",			G_shrubbot_ban,			'b', 0,				"ban a player by IP and GUID with an optional expiration time (seconds) and reason", "[^3name|slot#^9] (^htime^9) (^hreason^9)"},
	{"unban",		G_shrubbot_unban,		'b', 0,				"unbans a player specified ban number as seen in ^2!showbans^9", "[^3ban slot#^9]"},
	{"putteam",		G_shrubbot_putteam,		'p', 0,				"move a player to a specified team", "[^3name|slot#^9] [^3r|b|s^9]"},
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
	{"spec999",		G_shrubbot_spec999,		'P', 0,				"moves 999 pingers to the spectator team", ""},
	{"shuffle",		G_shrubbot_shuffle,		'S', 0,				"shuffle the teams to try and even them", ""},
	{"rename",		G_shrubbot_rename,		'N', SCMDF_TYRANNY, "renames a player", "[^3name|slot#^9] [^3new name^9]"},
	{"gib",			G_shrubbot_gib,			'g', SCMDF_TYRANNY, "insantly gibs a player", "(^3name|slot#^9) (^hreason^9)"},
	{"slap",		G_shrubbot_slap,		'A', SCMDF_TYRANNY, "give a player a specified amount of damage for a specified reason", "[^3name|slot#^9] (^hdamage^9) (^hreason^9)"},
	{"burn",		G_shrubbot_burn,		'U', SCMDF_TYRANNY, "burns a player taking some of his health", "[^3name|slot#^9] (^hreason^9)"},	
	{"warn",		G_shrubbot_warn,		'R', 0,				"warns a player by displaying the reason", "[^3name|slot#^9] [^3reason^9]"},
	{"news",		G_shrubbot_news,		'W', 0,				"play the map's news reel or another map's news reel if specified", "^9(^hmapname^9)"},
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
//	{"flinga",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "flings all players in random direction", ""},
//	{"throwa",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "throws all players forward", ""},
//	{"launcha",		G_shrubbot_fling,		'L', SCMDF_TYRANNY, "launches all players vertically", ""},
	{"disorient",	G_shrubbot_disorient,	'd', SCMDF_TYRANNY, "flips a player's view", "^9[^3name|slot#^9] (^hreason^9)"},
	{"orient",		G_shrubbot_orient,		'd', SCMDF_TYRANNY, "unflips a disoriented player's view", "^9[^3name|slot#^9]"},
	{"resetxp",		G_shrubbot_resetxp,		'X', SCMDF_TYRANNY, "reset a player's XP", "^9[^3name|slot#^9] (^hreason^9)"},
	{"resetmyxp",   G_shrubbot_resetmyxp,   'M', 0,				"resets your own XP", ""}, 
	{"nextmap",     G_shrubbot_nextmap,     'n', 0,				"loads the next map", ""},
	{"swap",        G_shrubbot_swap,        'w', 0,				"swap teams", ""},

	// jet Pilot - new shrubbot commands
	{"420",			G_shrubbot_420,			'T', SCMDF_TYRANNY, "gives a blunt to a player or all players", "^9(^3name|slot#^9)"},
	{"revive",		G_shrubbot_revive,		'v', SCMDF_TYRANNY, "revives a dead player", "^9[^3name|slot#^9] (^hreason^9)"},
#ifdef __CLIENTCMD__
	{"clientcmd",	G_shrubbot_clientcmd,	'j', SCMDF_TYRANNY, "can force a player to perform some commands", "^9[^3name|slot#^9] [^3command^9]"},
#endif
	{"rocket",		G_shrubbot_rocket,		'j', SCMDF_TYRANNY, "have a rocket shoot from a player", "^9[^3name|slot#^9]"},
	{"disguise",	G_shrubbot_disguise,	'g', SCMDF_TYRANNY, "gives a player a disguise of the specified class", "^9[^3name|slot#^9] (^hclass number^9)"},
	{"poison",		G_shrubbot_poison,		'U', SCMDF_TYRANNY, "poisons a player", "^9(^3name|slot#^9) (^hreason^9)"},
	{"ammopack",	G_shrubbot_ammopack,	'J', SCMDF_TYRANNY, "an ammo pack will spawn out of the player if they have enough chargebar", "^9[^3name|slot#^9]"},
	{"medpack",		G_shrubbot_medpack,		'J', SCMDF_TYRANNY, "a health pack will spawn out of the player if they have enough chargebar", "^9[^3name|slot#^9]"},
	{"pants",		G_shrubbot_pants,		't', SCMDF_TYRANNY, "removes a player's pants!", "^9(^3name|slot#^9)"},
	{"give",		G_shrubbot_give,		'e', SCMDF_TYRANNY, "gives a player something...", "^9[^3name|slot#^9] item/thing"},

	// Jaybird
	{"finger",		G_shrubbot_finger,		'f', 0,				"gives specific information about a player", "^9[^3name|slot#^9]"},
	{"uptime",		G_shrubbot_uptime,		'u', 0,				"displays server uptime", "" },
	{"glow",		G_shrubbot_glow,		'o', SCMDF_TYRANNY, "makes player(s) glow", "^9(^3name|slot#^9)"},
	{"freeze",		G_shrubbot_freeze,		'E', SCMDF_TYRANNY, "freezes player(s) move", "^9(^3name|slot#^9)"},
	{"unfreeze",	G_shrubbot_unfreeze,	'E', SCMDF_TYRANNY, "makes player(s) moving again", "^9(^3name|slot#^9)"},
	{"",			NULL,					'\0', 0}
};

g_shrubbot_level_t		*g_shrubbot_levels[MAX_SHRUBBOT_LEVELS];
g_shrubbot_admin_t		*g_shrubbot_admins[MAX_SHRUBBOT_ADMINS];
g_shrubbot_ban_t		*g_shrubbot_bans[MAX_SHRUBBOT_BANS];
g_shrubbot_ban_t		*g_shrubbot_mutes[MAX_SHRUBBOT_BANS]; // jaquboss
g_shrubbot_command_t	*g_shrubbot_commands[MAX_SHRUBBOT_COMMANDS];

qboolean G_shrubbot_permission(gentity_t *ent, char flag) 
{
	int i;
	int l = 0;
	char *guid;
	char userinfo[MAX_INFO_STRING];
	char *flags;

	// console always wins
	if(!ent)
		return qtrue;
	
	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
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

qboolean _shrubbot_admin_higher(gentity_t *admin, gentity_t *victim) 
{
	int i;
	int alevel = 0;
	char *guid;
	char userinfo[MAX_INFO_STRING];

	// console always wins
	if(!admin) return qtrue;
	// just in case
	if(!victim) return qtrue;

	trap_GetUserinfo(admin-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			alevel = g_shrubbot_admins[i]->level;
		}
	}
	trap_GetUserinfo(victim-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			if(alevel < g_shrubbot_admins[i]->level)
				return qfalse;
		}
	}
	return qtrue;
}

void G_shrubbot_writeconfig_string(char *s, fileHandle_t f) 
{
	char buf[MAX_STRING_CHARS];

	buf[0] = '\0';
	if(s[0]) {
		//Q_strcat(buf, sizeof(buf), s);
		Q_strncpyz(buf, s, sizeof(buf));
		trap_FS_Write(buf, strlen(buf), f);
	}
	trap_FS_Write("\n", 1, f);
}

void G_shrubbot_writeconfig_int(int v, fileHandle_t f) 
{
	char buf[32];

	sprintf(buf, "%d", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}

void G_shrubbot_writeconfig_float(float v, fileHandle_t f) 
{
	char buf[32];

	sprintf(buf, "%f", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}

void _shrubbot_writeconfig() 
{
	fileHandle_t f;
	int len, i, j;
	time_t t;
	char levels[MAX_STRING_CHARS] = {""}; 

	if(!g_shrubbot.string[0]) return;
	time(&t);
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_WRITE);
	if(len < 0) {
		G_Printf("_shrubbot_writeconfig: could not open %s\n",
				g_shrubbot.string);
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

void G_shrubbot_readconfig_string(char **cnf, char *s, int size) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
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
		if((s[0] == '\0' && strlen(t) <= size) ||
			(strlen(t)+strlen(s) < size)) {

			Q_strcat(s, size, t);
			Q_strcat(s, size, " ");
		}
		t = COM_ParseExt(cnf, qfalse);
	}
	// trim the trailing space
	if(strlen(s) > 0 && s[strlen(s)-1] == ' ') 
		s[strlen(s)-1] = '\0';
}

void G_shrubbot_readconfig_int(char **cnf, int *v) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
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

void G_shrubbot_readconfig_float(char **cnf, float *v) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
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
int _shrubbot_level(gentity_t *ent) 
{
	int i;
	char *guid;
	qboolean found = qfalse;	
	char userinfo[MAX_INFO_STRING];

	if(!ent) {	
		// forty - we are on the console, return something high for now.
		return MAX_SHRUBBOT_LEVELS;
	}

	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			found = qtrue;
			break;	
		}
	}

	if(found) {
		return g_shrubbot_admins[i]->level;
	}

	return 0;
}

static qboolean _shrubbot_command_permission(gentity_t *ent, char *command)  
{  
	int i, j;  
	int level = _shrubbot_level(ent);
	//char userinfo[MAX_INFO_STRING];	// Jaybird - this is not used, but I wonder why?
										// This function needs MAJOR attention!

	if(!ent)  
		return qtrue;
	for(i = 0; g_shrubbot_commands[i]; i++) {  
		if(!Q_stricmp(command, g_shrubbot_commands[i]->command)) {  
			for(j = 0; g_shrubbot_commands[i]->levels[j] != -1; j++) {
				if(g_shrubbot_commands[i]->levels[j] == level) {
					return qtrue;  
				}  
			}  
		}  
	}  
	return qfalse;  
}

void _shrubbot_log(gentity_t *admin, char *cmd, int skiparg)
{
	fileHandle_t f;
	int len, i, j;
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

	sec = level.time / 1000;
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	*flags = '\0';
	if(admin) {
		for(i=0; g_shrubbot_admins[i]; i++) {
			if(!Q_stricmp(g_shrubbot_admins[i]->guid , 
				admin->client->sess.guid)) {

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
			(admin) ? admin->client->sess.guid 
				: "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
			(admin) ? admin->client->pers.netname : "console",
			flags,
			cmd,
			victim->client->sess.guid,
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
			(admin) ? admin->client->sess.guid
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

void jP_GetDurationString(int secs, char *dur, int len)
{
	int		 years, months, weeks, days, hours, minutes;
	qboolean addComma = qfalse;

	char temp[512];

	if (secs < 0) 
	{
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

	if (years) 
	{
		Q_strncpyz(temp, va("%i year%s", years, years > 1 ? "s" : ""), 512);
		addComma = qtrue;
	}

	if (months)
	{
		Q_strcat(temp, 512, va("%s%i month%s", addComma ? (weeks > 0 ? ", " : " and ") : "", months, months > 1 ? "s" : ""));
		addComma = qtrue;
	}
    
	if (weeks)
	{
		Q_strcat(temp, 512, va("%s%i week%s", addComma ? (days > 0 ? ", " : " and ") : "", weeks, weeks > 1 ? "s" : ""));
		addComma = qtrue;
	}
	
	if (days)
	{
		Q_strcat(temp, 512, va("%s%i day%s", addComma ? (hours > 0 ? ", " : " and ") : "", days, days > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (hours)
	{
		Q_strcat(temp, 512, va("%s%i hour%s", addComma ? (minutes > 0 ? ", " : " and ") : "", hours, hours > 1 ? "s" : ""));
		addComma = qtrue;
	}

	if (minutes)
	{
		Q_strcat(temp, 512, va("%s%i minute%s", addComma ? (secs > 0 ? ", " : " and ") : "", minutes, minutes > 1 ? "s" : ""));
		addComma = qtrue;
	}
	
	if (secs)
	{
		Q_strcat(temp, 512, va("%s%i second%s", addComma ? " and " : "", secs, secs > 1 ? "s" : ""));	
	}

	Q_strncpyz(dur, temp, len);
}


void G_shrubbot_duration(int secs, char *duration, int dursize) {

	if(secs > (SECONDS_IN_YEAR * 50) || secs < 0)
	{
		Q_strncpyz(duration, "PERMANENT", dursize);
	}
	else if(secs >= SECONDS_IN_YEAR)
	{
		Com_sprintf(duration, dursize, "%d years", (secs / SECONDS_IN_YEAR));
	}
	else if(secs >= SECONDS_IN_MONTH) 
	{
		Com_sprintf(duration, dursize, "%i months",	(secs / SECONDS_IN_MONTH));
	}	
	else if(secs >= SECONDS_IN_WEEK) 
	{
		Com_sprintf(duration, dursize, "%i weeks", (secs / SECONDS_IN_WEEK));
	}
	else if(secs >= SECONDS_IN_DAY) 
	{
		Com_sprintf(duration, dursize, "%i days", (secs / SECONDS_IN_DAY));
	}
	else if(secs >= SECONDS_IN_HOUR) 
	{
		Com_sprintf(duration, dursize, "%i hours", (secs / SECONDS_IN_HOUR));
	}	
	else if(secs >= SECONDS_IN_MINUTE) 
	{
		Com_sprintf(duration, dursize, "%i mins", (secs / SECONDS_IN_MINUTE));
	}	
	else 
	{
		Com_sprintf(duration, dursize, "%i secs", secs);
	}
}

qboolean G_shrubbot_ban_check(char *userinfo, char *reason) 
{
	char *guid, *ip;
	int i;
	time_t t;

	if(!time(&t)) return qfalse;
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	if(!*userinfo) return qfalse;
	ip = Info_ValueForKey(userinfo, "ip");
	if(!*ip) return qfalse;
	guid = Info_ValueForKey(userinfo, "cl_guid");
	if(!*guid) return qfalse;
	for(i=0; g_shrubbot_bans[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1)
			continue;
		if(strstr(ip, g_shrubbot_bans[i]->ip)) {
			if(reason) {
				Com_sprintf(
					reason, 
					MAX_STRING_CHARS, 
					"^9Reason: %s\nExpires in: %u seconds.^7\n", 
					g_shrubbot_bans[i]->reason, 
					g_shrubbot_bans[i]->expires - (int)t
				);
			}
			return qtrue;
		}
		if(!Q_stricmp(g_shrubbot_bans[i]->guid, guid)) {
			if(reason) {
				Com_sprintf(
					reason, 
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

qboolean G_shrubbot_mute_check(char *userinfo) 
{
	char *guid, *ip;
	int i;
	time_t t;

	if(!time(&t)) return qfalse;
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	if(!*userinfo) return qfalse;
	ip = Info_ValueForKey(userinfo, "ip");
	if(!*ip) return qfalse;
	guid = Info_ValueForKey(userinfo, "cl_guid");
	if(!*guid) return qfalse;
	for(i=0; g_shrubbot_mutes[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_mutes[i]->expires != 0 &&
			(g_shrubbot_mutes[i]->expires - t) < 1)
			continue;
		if(strstr(ip, g_shrubbot_mutes[i]->ip)) {
			return qtrue;
		}
		if(!Q_stricmp(g_shrubbot_mutes[i]->guid, guid)) {
			return qtrue;
		}
	}
	return qfalse;
}

char *G_shrubbot_mute_name(char *userinfo) 
{
	char *guid, *ip;
	int i;
	time_t t;

	if(!time(&t)) return "";
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	if(!*userinfo) return "";
	ip = Info_ValueForKey(userinfo, "ip");
	guid = Info_ValueForKey(userinfo, "cl_guid");
	if(!*guid && !*ip) return "";
	for(i=0; g_shrubbot_mutes[i]; i++) {
		if(g_shrubbot_mutes[i]->expires != 0 &&
			(g_shrubbot_mutes[i]->expires - t) < 1)
			continue;
		if(*ip && strstr(ip, g_shrubbot_mutes[i]->ip)) {
			return va("%s", g_shrubbot_mutes[i]->name);
		}
		if(*guid && !Q_stricmp(g_shrubbot_mutes[i]->guid, guid)) {
			return va("%s", g_shrubbot_mutes[i]->name);
		}
	}
	return "";
}

qboolean G_shrubbot_cmd_check(gentity_t *ent)
{
	int i;
	char command[MAX_SHRUBBOT_CMD_LEN];
	char *cmd;
	int skip = 0;

	if(g_shrubbot.string[0] == '\0') 
		return qfalse;

	command[0] = '\0';
	Q_SayArgv(0, command, sizeof(command));
	if(!Q_stricmp(command, "say") || 
		(G_shrubbot_permission(ent, SBF_TEAMFTSHRUBCMD) && 
			(!Q_stricmp(command, "say_team") || 
			!Q_stricmp(command, "say_buddy")))) {
				skip = 1;
				Q_SayArgv(1, command, sizeof(command));
	}
	if(!command[0]) 
		return qfalse;

	if(command[0] == '!') {
		cmd = &command[1];
	}
	else if(ent == NULL) {
		cmd = &command[0];
	}
	else {
		return qfalse;
	}

	for(i=0; g_shrubbot_commands[i]; i++) {
		if(Q_stricmp(cmd, g_shrubbot_commands[i]->command))
			continue;

		if(_shrubbot_command_permission(ent, cmd)) {
			char *cmdLine;
			int argIx;

			// Replace shortcuts
			if(g_shortcuts.integer){
				cmdLine = G_Shortcuts(ent, g_shrubbot_commands[i]->exec);
			} else {
				cmdLine = g_shrubbot_commands[i]->exec;
			}

				// Replace arguments
				for (argIx = 1; argIx <= 9; argIx++) {
					char arg[MAX_NAME_LENGTH];
					Q_SayArgv(1 + argIx, arg, sizeof(arg));
					cmdLine = Q_StrReplace(cmdLine, va("[%i]", argIx), arg);
				}

				trap_SendConsoleCommand(EXEC_APPEND, cmdLine);

			_shrubbot_log(ent, cmd, skip);
			return qtrue;
		}
		else {
			SP(va("%s: permission denied\n",
				g_shrubbot_commands[i]->command));
			_shrubbot_log(ent, "attempted", skip-1);
			return qfalse;
		}
	}

	for (i=0; g_shrubbot_cmds[i].keyword[0]; i++) {
		if(Q_stricmp(cmd, g_shrubbot_cmds[i].keyword)) 
			continue;
		if((g_shrubbot_cmds[i].cmdFlags & SCMDF_TYRANNY) &&
			!g_tyranny.integer) {

			SP(va("%s: g_tyranny is not enabled\n",
				g_shrubbot_cmds[i].keyword));
			_shrubbot_log(ent, "attempted", skip-1);
		}
		else if(G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag)) {
			g_shrubbot_cmds[i].handler(ent, skip);
			_shrubbot_log(ent, cmd, skip);
			return qtrue; 
		}
		else {
			SP(va("%s: permission denied\n",
				g_shrubbot_cmds[i].keyword));
			_shrubbot_log(ent, "attempted", skip-1);
		}
	}
	return qfalse;
}

qboolean G_shrubbot_readconfig(gentity_t *ent, int skiparg) 
{
	g_shrubbot_level_t	*l = NULL;
	g_shrubbot_admin_t	*a = NULL;
	g_shrubbot_ban_t	*b = NULL;
	g_shrubbot_ban_t	*m = NULL;
	g_shrubbot_command_t *c = NULL;  
	int lc = 0, ac = 0, bc = 0, cc = 0, mc = 0;  
	fileHandle_t		f;
	int					len;
	char				*cnf, *cnf2;	
	char				*t;
	qboolean			level_open, admin_open, ban_open, mute_open, command_open;  
	char				levels[MAX_STRING_CHARS] = {""};  

	if(!g_shrubbot.string[0]) return qfalse;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_READ); 
	if(len < 0)	{
		SP(va("^dreadconfig: ^9could not open shrubbot config file ^2%s\n", g_shrubbot.string));
		_shrubbot_default_levels();
		return qfalse;
	}

	cnf		= malloc(len+1);
	cnf2	= cnf;
	trap_FS_Read(cnf, len, f);
	*(cnf + len) = '\0';
	trap_FS_FCloseFile(f);
	
	G_shrubbot_cleanup();
	
	t = COM_Parse(&cnf);
	level_open = admin_open = ban_open = mute_open = command_open = qfalse;
	while(*t) {
		if(!Q_stricmp(t, "[level]") || 
			!Q_stricmp(t, "[admin]") ||
			!Q_stricmp(t, "[ban]") ||
			!Q_stricmp(t, "[mute]") ||
			!Q_stricmp(t, "[command]")) {
			
			if(level_open) g_shrubbot_levels[lc++] = l;
			else if(admin_open) g_shrubbot_admins[ac++] = a;
			else if(ban_open) g_shrubbot_bans[bc++] = b;
			else if(mute_open) g_shrubbot_mutes[mc++] = m;
			else if(command_open) g_shrubbot_commands[cc++] = c;
			level_open = admin_open = mute_open =
				ban_open = command_open = qfalse;
		}


		if(level_open) 
		{
			if(!Q_stricmp(t, "level")) 
			{
				G_shrubbot_readconfig_int(&cnf, &l->level);
			}
			else if(!Q_stricmp(t, "name")) 
			{
				G_shrubbot_readconfig_string(&cnf, l->name, sizeof(l->name)); 
			}
			else if(!Q_stricmp(t, "flags")) 
			{
				G_shrubbot_readconfig_string(&cnf, l->flags, sizeof(l->flags)); 
			}
			else 
			{
				SP(va("^dreadconfig: ^9[level] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(admin_open) 
		{
			if(!Q_stricmp(t, "name")) 
			{
				G_shrubbot_readconfig_string(&cnf, a->name, sizeof(a->name)); 
			}
			else if(!Q_stricmp(t, "guid")) 
			{
				G_shrubbot_readconfig_string(&cnf, a->guid, sizeof(a->guid)); 
			}
			else if(!Q_stricmp(t, "level")) 
			{
				G_shrubbot_readconfig_int(&cnf, &a->level); 
			}
			else if(!Q_stricmp(t, "flags")) 
			{
				G_shrubbot_readconfig_string(&cnf, a->flags, sizeof(a->flags)); 
			}
			else 
			{
				SP(va("^dreadconfig: ^9[admin] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		}
		else if(ban_open) 
		{
			if(!Q_stricmp(t, "name")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->name, sizeof(b->name)); 
			}
			else if(!Q_stricmp(t, "guid")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->guid, sizeof(b->guid)); 
			}
			else if(!Q_stricmp(t, "ip")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->ip, sizeof(b->ip)); 
			}
			else if(!Q_stricmp(t, "reason")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->reason, sizeof(b->reason)); 
			}
			else if(!Q_stricmp(t, "made")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->made, sizeof(b->made)); 
			}
			else if(!Q_stricmp(t, "expires")) 
			{
				G_shrubbot_readconfig_int(&cnf, &b->expires);
			}
			else if(!Q_stricmp(t, "banner")) 
			{
				G_shrubbot_readconfig_string(&cnf, b->banner, sizeof(b->banner)); 
			}
			else {
				SP(va("^dreadconfig: ^9[ban] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		} else if(mute_open) {
			if(!Q_stricmp(t, "name")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->name, sizeof(m->name)); 
			}
			else if(!Q_stricmp(t, "guid")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->guid, sizeof(m->guid)); 
			}
			else if(!Q_stricmp(t, "ip")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->ip, sizeof(m->ip)); 
			}
			else if(!Q_stricmp(t, "reason")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->reason, sizeof(m->reason)); 
			}
			else if(!Q_stricmp(t, "made")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->made, sizeof(m->made)); 
			}
			else if(!Q_stricmp(t, "expires")) 
			{
				G_shrubbot_readconfig_int(&cnf, &m->expires);
			}
			else if(!Q_stricmp(t, "muter")) 
			{
				G_shrubbot_readconfig_string(&cnf, m->banner, sizeof(m->banner)); 
			}
			else {
				SP(va("^dreadconfig: ^9[mute] parse error near %s on line %d^7\n", t, COM_GetCurrentParseLine()));
			}
		} else if(command_open) {
			if(!Q_stricmp(t, "command")) { 
				G_shrubbot_readconfig_string(&cnf,  
					c->command, sizeof(c->command));  
			} 
			else if(!Q_stricmp(t, "exec")) { 
				G_shrubbot_readconfig_string(&cnf,  
					c->exec, sizeof(c->exec));  
			} 
			else if(!Q_stricmp(t, "desc")) { 
				G_shrubbot_readconfig_string(&cnf,  
					c->desc, sizeof(c->desc));  
			} 
			else if(!Q_stricmp(t, "levels")) { 
				char level[4] = {""}; 
				char *lp = levels; 
				int cmdlevel = 0;

			G_shrubbot_readconfig_string(&cnf,  
				levels, sizeof(levels)); 

				while(*lp) { 
					if(*lp == ' ') { 
						c->levels[cmdlevel++] =  
						atoi(level); 
						level[0] = '\0'; 
						*lp++; 
						continue; 
					}
					Q_strcat(level, sizeof(level), 
					va("%c", *lp)); 
					*lp++; 
			} 
			if(level[0]) 
				c->levels[cmdlevel++] = atoi(level); 
				// tjw: ensure the list is -1 terminated 
				c->levels[MAX_SHRUBBOT_LEVELS] = -1; 
			} 
			else
			{ 
				SP(va("readconfig: [command] parse error near " 
					"%s on line %d\n", t,  
					COM_GetCurrentParseLine())); 
			}
		}

		if(!Q_stricmp(t, "[level]")) 
		{
			if(lc >= MAX_SHRUBBOT_LEVELS) return qfalse;
			l = malloc(sizeof(g_shrubbot_level_t));
			l->level	= 0;
			*l->name	= '\0';
			*l->flags	= '\0';
			level_open	= qtrue;
		}
		else if(!Q_stricmp(t, "[admin]"))
		{
			if(ac >= MAX_SHRUBBOT_ADMINS) return qfalse;
			a = malloc(sizeof(g_shrubbot_admin_t));
			*a->name	= '\0';
			*a->guid	= '\0';
			a->level	= 0;
			*a->flags	= '\0';
			admin_open	= qtrue;
		}
		else if(!Q_stricmp(t, "[ban]"))
		{
			if(bc >= MAX_SHRUBBOT_BANS) return qfalse;
			b = malloc(sizeof(g_shrubbot_ban_t));
			*b->name	= '\0';
			*b->guid	= '\0';
			*b->ip		= '\0';
			*b->made	= '\0';
			b->expires	= 0;
			*b->reason	= '\0';
			ban_open	= qtrue;
		}
		else if(!Q_stricmp(t, "[mute]"))
		{
			if(mc >= MAX_SHRUBBOT_BANS) return qfalse;
			m = malloc(sizeof(g_shrubbot_ban_t));
			*m->name	= '\0';
			*m->guid	= '\0';
			*m->ip		= '\0';
			*m->made	= '\0';
			m->expires	= 0;
			*m->reason	= '\0';
			mute_open	= qtrue;
		}
		else if(!Q_stricmp(t, "[command]")) {  
			if(cc >= MAX_SHRUBBOT_COMMANDS)  // jaquboss use command count instead
				return qfalse;  
					c = malloc(sizeof(g_shrubbot_command_t));  
					*c->command = '\0';  
					*c->exec = '\0';  
					*c->desc = '\0';  
				memset(c->levels, -1, sizeof(c->levels));  
				command_open = qtrue;  
		}  

		t = COM_Parse(&cnf);
	}
	if(level_open)	
		g_shrubbot_levels[lc++] = l;
	if(admin_open)	
		g_shrubbot_admins[ac++] = a;
	if(ban_open)	
		g_shrubbot_bans[bc++] = b;
	if(mute_open)	
		g_shrubbot_mutes[mc++] = m;
	if(command_open) 
		g_shrubbot_commands[cc++] = c;

	free(cnf2);
	SP(va("readconfig: loaded "  
			"%d levels, %d admins, %d bans, %d mutes, %d commands\n",  
			lc, ac, bc, mc, cc));  

	if(lc == 0) _shrubbot_default_levels();
	return qtrue;
}

qboolean G_shrubbot_time(gentity_t *ent, int skiparg) 
{
	time_t t;
	struct tm *lt;
	char s[50];

	if(!time(&t)) return qfalse;
	lt = localtime(&t);
	strftime(s, sizeof(s), "%I:%M%p %Z", lt);
	AP(va("chat \"Local Time: %s\" -1", s));
	return qtrue;
}

qboolean G_shrubbot_setlevel(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char lstr[11]; // 10 is max strlen() for 32-bit int
	char userinfo[MAX_INFO_STRING];
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
	if(l > _shrubbot_level(ent)) {
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
		SP("setlevel: sorry, but your intended victim has a higher"
			" admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	trap_GetUserinfo(pids[0], userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	// tjw: use raw name
	//SanitizeString(vic->client->pers.netname, n2, qtrue);
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

qboolean G_shrubbot_kick(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], *reason, err[MAX_STRING_CHARS];
	int minargc;

	minargc = 3+skiparg;
	if(G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) 
		minargc = 2+skiparg;
	
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

qboolean G_shrubbot_tempban(int clientnum, char *reason, int length) 
{

	char userinfo[MAX_INFO_STRING];
	char *guid, *ip;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic;

	if(!time(&t)) return qfalse;

	trap_GetUserinfo(clientnum, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	ip = Info_ValueForKey(userinfo, "ip");
	vic = &g_entities[clientnum];
	b = malloc(sizeof(g_shrubbot_ban_t));

	if(!b) 
		return qfalse;

	Q_strncpyz(b->name, vic->client->pers.netname, sizeof(b->name));
	Q_strncpyz(b->guid, guid, sizeof(b->guid));

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

qboolean G_shrubbot_ban(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason, err[MAX_STRING_CHARS];
	char userinfo[MAX_INFO_STRING];
	char *guid, *ip;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b = NULL;
	time_t t;
	struct tm *lt;
	gentity_t *vic;
	int minargc;
	char duration[MAX_STRING_CHARS];
	int modifier = 1;
	int lastchar;

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
		lastchar = strlen(secs) - 1;
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

	trap_GetUserinfo(pids[0], userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	ip = Info_ValueForKey(userinfo, "ip");
	vic = &g_entities[pids[0]];
	b = malloc(sizeof(g_shrubbot_ban_t));

	if(!b) 
		return qfalse;

	//SanitizeString(vic->client->pers.netname, tmp, qtrue);
	Q_strncpyz(b->name,
		vic->client->pers.netname,
		sizeof(b->name));
	Q_strncpyz(b->guid, guid, sizeof(b->guid));

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
		//SanitizeString(ent->client->pers.netname, tmp, qtrue);
		//Q_strncpyz(b->banner, tmp, sizeof(b->banner));
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
	trap_DropClient(pids[0],
		va("You have been banned %s, Reason: %s",
		duration,
		(*reason) ? reason : "banned by admin"),
		0);
	return qtrue;
}

qboolean G_shrubbot_unban(gentity_t *ent, int skiparg) 
{
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
	if(bnum < 1) {
		SP("^dunban: ^9invalid ban #\n");
		return qfalse;
	}
	if(!g_shrubbot_bans[bnum-1]) {
		SP("^dunban: ^9invalid ban #\n");
		return qfalse;
	}
	g_shrubbot_bans[bnum-1]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	SP(va("^dunban: ^9ban #%d removed\n", bnum));
	_shrubbot_writeconfig();
	return qtrue;
}

qboolean G_shrubbot_mute(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason, err[MAX_STRING_CHARS];
	char userinfo[MAX_INFO_STRING];
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
	int lastchar;

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
		SP("mute usage: !mute [name] [seconds] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));

	if(*secs) {
		lastchar = strlen(secs) - 1;
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
			SP("mute: seconds must be a positive integer\n");
			return qfalse;
		}
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("mute: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("mute: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}

	trap_GetUserinfo(pids[0], userinfo, sizeof(userinfo));

	if ( G_shrubbot_mute_check( userinfo )) {
		SP("mute: already muted\n");
		return qfalse;
	}
	

	guid = Info_ValueForKey(userinfo, "cl_guid");
	ip = Info_ValueForKey(userinfo, "ip");
	vic = &g_entities[pids[0]];
	m = malloc(sizeof(g_shrubbot_ban_t));

	if(!m) 
		return qfalse;

	//SanitizeString(vic->client->pers.netname, tmp, qtrue);
	Q_strncpyz(m->name,
		vic->client->pers.netname,
		sizeof(m->name));
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
		Q_strncpyz(m->banner,
			ent->client->pers.netname,
			sizeof(m->banner));
	}
	else Q_strncpyz(m->banner, "console", sizeof(m->banner));
	if(!seconds) 
		m->expires = 0;
	else 
		m->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;
	if(!*reason) {
		Q_strncpyz(m->reason,
			"muted by admin",
			sizeof(m->reason));
	}
	else {
		Q_strncpyz(m->reason, reason, sizeof(m->reason));
	}
	for(i=0; g_shrubbot_mutes[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("mute: too many mutes\n");
		free(m);
		return qfalse;
	}
	g_shrubbot_mutes[i] = m;

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

	AP(va("chat \"^dmute: ^*%s ^9has been muted %s\"",	vic->client->pers.netname, duration ));

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
	} else {
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


qboolean G_MatchOneMute(int *plist, char *err, int len) 
{
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
			sprintf(line, "%2i - %s^7\n",
				*p,	
				g_shrubbot_mutes[*p]->name);
			if(strlen(err)+strlen(line) > len)
				break;
			Q_strcat(err, len, line);
		}
		return qfalse;
	}
	return qtrue;
}

qboolean G_shrubbot_unmute(gentity_t *ent, int skiparg) 
{
	char ms[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	time_t t;
	int pids[MAX_CLIENTS];

	if(!time(&t)) return qfalse;
	if(Q_SayArgc() < 2+skiparg) {
		SP("^dunmute usage: ^2!unmute [name]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, ms, sizeof(ms));

	if(MuteNumbersFromString(ms, pids) != 1)
	{

		G_MatchOneMute(pids, err, sizeof(err));
		SP(va("unmute: %s\n", err));
		return qfalse;
	}

	g_shrubbot_mutes[pids[0]]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;

	AP(va("chat \"^dmute: ^*%s ^9has been unmuted\"", g_shrubbot_mutes[pids[0]]->name ));

	_shrubbot_writeconfig();
	return qtrue;
}

qboolean G_shrubbot_putteam(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], team[7], err[MAX_STRING_CHARS];
	gentity_t *vic;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));
	if(Q_SayArgc() < 3+skiparg) {
		SP("^dputteam usage: ^2!putteam ^9[name] [r|b|s]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dputteam: ^9%s\n", err));
		return qfalse;
	}	
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^dputteam: ^9sorry, %s ^9is a higher level admin than you are.\n", 
			g_entities[pids[0]].client->pers.netname));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!SetTeam(vic, team, qtrue, -1, -1, qfalse)) {
		SP("^dputteam: ^9Put team failed^7\n");
		return qfalse;
	}
	return qtrue;
}
/*
qboolean G_shrubbot_mute(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char command[MAX_SHRUBBOT_CMD_LEN], *cmd;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("^dmute usage: ^2!mute ^9[name|slot#]^7\n");
		return qfalse;
	}
	Q_SayArgv(skiparg, command, sizeof(command));
	cmd = command;
	if(*cmd == '!')
		*cmd++;
	Q_SayArgv(1+skiparg, name, sizeof(name)); 
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^dmute: ^9%s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^dmute: ^9sorry, %s ^9is a higher level admin than you are.^7\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(vic->client->sess.muted == qtrue) {
		if(!Q_stricmp(cmd, "mute")) {
			SP("^dmute: ^9player is already muted!^7\n");
			return qtrue;
		}
		vic->client->sess.muted = qfalse;
		vic->client->sess.auto_mute_time = -1;
                CPx(pids[0], "print \"^dYou've been unmuted^7\n\"" );
                AP(va("chat \"%s^9 has been unmuted^7\"",  
			vic->client->pers.netname ));
	}
	else {
		if(!Q_stricmp(cmd, "unmute")) {
			SP("^dunmute: ^9player is not currently muted^7\n");
			return qtrue;
		}
		vic->client->sess.muted = qtrue;
		vic->client->sess.auto_mute_time = -1;
                CPx(pids[0], "print \"^dYou've been muted^7\n\"" );
                AP(va("chat \"%s ^9has been muted^7\"",  
			vic->client->pers.netname ));
	}
	ClientUserinfoChanged(pids[0]);
	return qtrue;
}
*/

qboolean G_shrubbot_pause(gentity_t *ent, int skiparg) 
{
	G_refPause_cmd(ent, qtrue);
	return qtrue;
}

qboolean G_shrubbot_unpause(gentity_t *ent, int skiparg) 
{
	G_refPause_cmd(ent, qfalse);
	return qtrue;
}

qboolean G_shrubbot_listplayers(gentity_t *ent, int skiparg) 
{
	int i,j;
	gclient_t *p;
	char c[3], t[2]; // color and team letter
	char n[MAX_NAME_LENGTH] = {""};
	char n2[MAX_NAME_LENGTH] = {""};
	char n3[MAX_NAME_LENGTH] = {""};
	char lname[MAX_NAME_LENGTH];
	char lname2[MAX_NAME_LENGTH];
	char *guid;
	char guid_stub[9];
	char fireteam[2];
	char muted[2];
	int l;
	fireteamData_t *ft;	
	char userinfo[MAX_INFO_STRING];
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
	SBP(va("listplayers: %d players connected:\n", 
		level.numConnectedClients));
	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		Q_strncpyz(t, "S", sizeof(t));
		Q_strncpyz(c, S_COLOR_YELLOW, sizeof(c));
		if(p->sess.sessionTeam == TEAM_ALLIES) {
			Q_strncpyz(t, "B", sizeof(t));
			Q_strncpyz(c, S_COLOR_BLUE, sizeof(c));
		}
		else if(p->sess.sessionTeam == TEAM_AXIS) {
			Q_strncpyz(t, "R", sizeof(t));
			Q_strncpyz(c, S_COLOR_RED, sizeof(c));
		}

		if(p->pers.connected == CON_CONNECTING) {
			Q_strncpyz(t, "C", sizeof(t));
			Q_strncpyz(c, S_COLOR_ORANGE, sizeof(c));
		}
		else if(p->pers.connected != CON_CONNECTED) {
			continue;
		}

		trap_GetUserinfo(i, userinfo, sizeof(userinfo));
		guid = Info_ValueForKey(userinfo, "cl_guid");

		if (g_entities[p->ps.clientNum].r.svFlags & SVF_BOT) {
			Q_strncpyz(guid_stub, "OMNIBOT*", sizeof(guid_stub));
		} else if (!*guid) {
            Q_strncpyz(guid_stub, "NONE", sizeof(guid_stub));
        } else {
			for(j=0; j<=8; j++) 
				guid_stub[j] = guid[j+24];
		}

		fireteam[0] = '\0';
		if(G_IsOnFireteam(i, &ft)) {
			Q_strncpyz(fireteam, 
				bg_fireteamNames[ft->ident - 1],
				sizeof(fireteam));
		}

		muted[0] = '\0';
		if(p->sess.muted || G_shrubbot_mute_check(userinfo)) {
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
		Com_sprintf(lname_fmt, sizeof(lname_fmt), "%%%is", spaces + strlen(lname));

		Com_sprintf(lname2, sizeof(lname2), lname_fmt, lname);
		SBP(va("%2i %s%s^7 %-2i '%s'^7 (*%s) ^1%1s ^3%1s^7 %s^7 %s%s^7%s\n", 
			i,
			c,
			t,
			l,
			lname2,
			guid_stub,
			muted,
			fireteam,
			p->pers.netname,
			(*n) ? "(a.k.a. " : "",
			n,
			(*n) ? ")" : ""
			));
	}
	SBP_end();
	return qtrue;
}

qboolean G_shrubbot_listteams(gentity_t *ent, int skiparg)
{
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
		} else if(p->sess.sessionTeam == TEAM_AXIS) {
			playerCount[1]++;
			pings[1] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[1] += p->sess.skillpoints[j];
			}
		} else if(p->sess.sessionTeam == TEAM_SPECTATOR) {
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

qboolean G_shrubbot_showbans(gentity_t *ent, int skiparg) 
{
	time_t	t;
	int		i							= 0;
	int		j;
	int		ti;
	int		args;
	int		secs;
	int		found						= 0;
	int		max_name					= 1;
	int		max_banner					= 1;
	int		start						= 0;
	int		searching					= 0;
//	int		page						= 0;
	int		pages						= 0;
	int		displayed					= 0;
	int		skipIsArg					= 1;
	int		isMatch[MAX_SHRUBBOT_BANS];
	char	userinfo[MAX_INFO_STRING];
	char	duration[MAX_STRING_CHARS];
	char	fmt[MAX_STRING_CHARS];
	char	skip[11];
	char	date[11];
	char	searchString[16];
	char	pageStr[11];
	char	*made;
	char	*guid;

	static int first					= 1;
	static int lastViewed[MAX_SHRUBBOT_ADMINS];
	char tmp[MAX_NAME_LENGTH];  
	int spacesName;  
	int spacesBanner;  
	
	if(!time(&t))
	{
		return qfalse;
	}

	if (first)
	{
		memset (lastViewed, 0, sizeof(lastViewed));
		first = 0;
	}

	memset(isMatch, 0, sizeof(isMatch));

	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	
	args = Q_SayArgc() - skiparg;

	if ( ent-g_entities > MAX_CLIENTS || ent-g_entities < 0 )
		return qfalse;

	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");

	for(j = 0; g_shrubbot_admins[j]; j++) 
	{
		if(!Q_stricmp(g_shrubbot_admins[j]->guid, guid))
		{
			lastViewed[j] = searching ? i : (start + SHRUBBOT_MAX_SHOWBANS);
			break;
		}
	}

	if (args == 1)
	{
		for(i = 0; g_shrubbot_bans[i]; i++) 
		{
			if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
			{			
				continue;
			}
	
			found++;
		}
	}	
	else if (args >= 2)
	{
		Q_SayArgv(1 + skiparg, skip, sizeof(skip));
		
		if (args > 2) 
		{
			Q_SayArgv(2 + skiparg, searchString, sizeof(searchString));
		}

		if (args > 3)
		{			
			Q_SayArgv(3 + skiparg, pageStr, sizeof(pageStr));
			start = atoi(pageStr);			
		}

		if (((isdigit(skip[0]) || skip[0] == '-') && isdigit(skip[1])) || !Q_stricmp(skip, "next") || !Q_stricmp(skip, "prev"))
		{
			for(i = 0; g_shrubbot_bans[i]; i++) 
			{
				if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
				{			
					continue;
				}

				found++;
			}

			if (!Q_stricmp(skip, "next"))
			{
				start = lastViewed[j] + 1;
			}
			else if (!Q_stricmp(skip, "prev"))
			{
				start = lastViewed[j] - SHRUBBOT_MAX_SHOWBANS;
			}
			else
			{
				start = atoi(skip);
			}
			// tjw: !showbans 1 means start with ban 0
			if(start > 0) 
			{
				start -= 1;
			}
			else if(start < 0)
			{
				start = found + start;
			}
			skipIsArg = 0;
		}		
		else 
		{
			if (!Q_stricmp(skip, "find"))
			{
				if (args == 2)
				{
					SP("^dusage : ^2!showbans ^9find <match> ^7\n"
					   "^9        (shows banned players matching <match>)\n");
					return qfalse;
				}				
				 
			//	SP(va("^dshowbans : ^9searching bans for ^3%s....\n", searchString));
				searching = 1;
				
				for(i = 0; g_shrubbot_bans[i]; i++) 
				{
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
					{			
						continue;
					}	

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->name), Q_strlwr(searchString)))
					{
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else if (!Q_stricmp(skip, "banner"))
			{
				if (args == 2)
				{
					SP("^dusage : ^2!showbans ^9banner <match> [start]^7\n"
					   "^9        (shows bans made by admins matching <match>)\n");
					return qfalse;
				}
				
			//	SP(va("^dshowbans : ^9searching for bans made by ^3%s....\n", searchString));
				searching = 2;	

				for(i = 0; g_shrubbot_bans[i]; i++) 
				{
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
					{			
						continue;
					}	

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->banner), Q_strlwr(searchString)))
					{
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else if (!Q_stricmp(skip, "reason"))
			{
				if (args == 2)
				{
					SP("^dusage : ^2!showbans ^9reason <match>^7 \n"
					   "^9        (shows bans with reasons matching <match>)\n");
					return qfalse;
				}
				
			//	SP(va("^dshowbans : ^9searching for bans with reason matching ^3%s....\n", searchString));
				searching = 3;	

				for(i = 0; g_shrubbot_bans[i]; i++) 
				{
					if (g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
					{			
						continue;
					}	

					if (!strstr(Q_strlwr(g_shrubbot_bans[i]->reason), Q_strlwr(searchString)))
					{
						continue;
					}

					isMatch[i] = 1;

					found++;
				}
			}
			else
			{
				SP("^dusage : ^2!showbans ^9[start|find <match>|banner <match>|reason <match>]^7\n"
				   "^9        (displays current list of active bans, filtered by argument criteria)\n");

				return qfalse;
			}
		}
	}

	pages = (found / SHRUBBOT_MAX_SHOWBANS) + 1;

	for(i = start; (g_shrubbot_bans[i] && displayed < SHRUBBOT_MAX_SHOWBANS) ; i++) 
	{	
		// jet Pilot - ignore entries that don't match our search criteria (if any)
		if (searching && !isMatch[i])
		{
			continue;
		}

		// find the longest name
		if(strlen(g_shrubbot_bans[i]->name) > max_name) 
		{
			max_name = strlen(g_shrubbot_bans[i]->name);
		}
		// find longest banner name
		if(strlen(g_shrubbot_bans[i]->banner) > max_banner) 
		{
			max_banner = strlen(g_shrubbot_bans[i]->banner);
		}

		displayed++;
	}

	Com_sprintf(fmt, sizeof(fmt),
		"^F%%4i^7 %%-%is^7 ^F%%-10s^7 %%-%is^7 ^F%%-9s^7 %%s\n",
		max_name, max_banner);

	if(!searching && start > found) 
	{
		SP(va("^dshowbans: ^9there are only ^2%d ^9active bans!\n", found));
		return qfalse;
	}

	displayed = 0;

	for( i = start; (g_shrubbot_bans[i] && displayed < SHRUBBOT_MAX_SHOWBANS); i++) 
	{	
		// ignore expired bans
		if(g_shrubbot_bans[i]->expires != 0 && (g_shrubbot_bans[i]->expires - t) < 1) 
		{			
			continue;
		}

		// jet Pilot - ignore entries that don't match our search criteria (if any)
		if (searching && !isMatch[i])
		{
			continue;
		}
		
		// tjw: only print out the the date part of made
		date[0] = '\0';

		made = g_shrubbot_bans[i]->made;

		for(ti = 0; *made; ti++) 
		{
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
	
			Com_sprintf(fmt, sizeof(fmt),  
				"^F%%4i^7 %%-%is^7 ^F%%-10s^7 %%-%is^7 ^F%%-9s^7 %%s\n",  
				spacesName + strlen(g_shrubbot_bans[i]->name),  
				spacesBanner + strlen(g_shrubbot_bans[i]->banner));  

		SP(va(fmt, 
			(i+1),
			g_shrubbot_bans[i]->name,
			date,
			g_shrubbot_bans[i]->banner,
			duration,
			g_shrubbot_bans[i]->reason
			));

		displayed++;
	}

	if (searching == 1)
	{
		SP(va("^dshowbans : ^9%i bans found for players matching ^2%s ^9(^7%i ^9Page%s)\n", 
			found, 
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else if (searching == 2)
	{
		SP(va("^dshowbans : ^9%i bans found for admins matching ^2%s ^9(^7%i ^9Page%s)\n", 
			found, 
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else if (searching == 3)
	{
		SP(va("^dshowbans : ^9%i bans found with reasons matching ^2%s ^9(^7%i ^9Page%s)\n", 
			found, 
			searchString,
			pages,
			pages != 1 ? "s" : ""));
	}
	else
	{
		SP(va("^dshowbans: ^9showing bans ^7%d ^9- ^7%d ^9of ^7%d ^9(Page ^7%i ^9of ^7%i^9)\n",	
			(start + 1),
			((start + SHRUBBOT_MAX_SHOWBANS) > found) ?	found : (start + SHRUBBOT_MAX_SHOWBANS),
			found,
			(start / SHRUBBOT_MAX_SHOWBANS) + 1,
			pages));
	}
	
	if((start + SHRUBBOT_MAX_SHOWBANS) < found) 
	{
		SP(va("\n          ^9(type !showbans ^2%s%s%s ^9to continue to page %i)\n",
			(args > 1 && skipIsArg ? va("%s ", skip) : ""),
			(args > 2 ? va("%s ", searchString) : ""),
			"next", //searching ? i : (start + SHRUBBOT_MAX_SHOWBANS + 1),
			(start / SHRUBBOT_MAX_SHOWBANS) + 2));
	}		

	
	lastViewed[j] = searching ? i : (start + SHRUBBOT_MAX_SHOWBANS);

	return qtrue;
}

qboolean G_shrubbot_help(gentity_t *ent, int skiparg) 
{
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
		if(count)
			str = va("%s\n",str);
		CP(va("chat \"^dhelp: ^9%i available commands\" -1", count));
		
		SBP( str );
		SBP("^9Type ^2!help ^d[command] ^9for help with a specific command.^7\n");
		SBP_end();

		return qtrue;
	} else {
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
				SBP( va("^dFuntion: ^9%s\n", g_shrubbot_cmds[i].function ) );
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

qboolean G_shrubbot_admintest(gentity_t *ent, int skiparg) 
{
	int i, l = 0;	
	char *guid;
	qboolean found = qfalse;
	qboolean lname = qfalse;
	char userinfo[MAX_INFO_STRING];
	
	if(!ent)
	{	
		SP("^dadmintest: ^9you are on the console.^7\n");
		return qtrue;
	}

	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) 
	{
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

qboolean G_shrubbot_cancelvote(gentity_t *ent, int skiparg) 
{

	level.voteInfo.voteNo = level.numConnectedClients;
	CheckVote();
	SP("^dcancelvote: ^9turns out everyone voted ^3No\n");
	return qtrue;
}

qboolean G_shrubbot_passvote(gentity_t *ent, int skiparg) 
{
	level.voteInfo.voteYes = level.numConnectedClients;
	level.voteInfo.voteNo = 0;
	CheckVote();
	SP("^dpassvote: ^9turns out everyone voted ^3Yes\n");
	return qtrue;
}

qboolean G_shrubbot_spec999(gentity_t *ent, int skiparg) 
{
	int i;
	gentity_t *vic;

	for(i = 0; i < level.maxclients; i++) {
		vic = &g_entities[i];
		if(!vic->client) continue;
		if(vic->client->pers.connected != CON_CONNECTED) continue;		
		if(vic->client->ps.ping == 999) {
			SetTeam(vic, "s", qtrue, -1, -1, qfalse);
			AP(va("chat \"^dspec999: ^*%s^9 moved to spectators^7\" -1",
				vic->client->pers.netname));
		}
	}
	return qtrue;
}

qboolean G_shrubbot_shuffle(gentity_t *ent, int skiparg) 
{
	G_shuffleTeams();
	return qtrue;
}

void ClientCleanName( const char *in, char *out, int outSize );

qboolean G_shrubbot_rename(gentity_t *ent, int skiparg) 
{
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
	ClientCleanName( newname, level.clients[level.sortedClients[pids[0]]].pers.netname, sizeof(level.clients[level.sortedClients[pids[0]]].pers.netname) );
	ClientUserinfoChanged(pids[0]);

	return qtrue;
}

qboolean G_shrubbot_gib(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg) {
		/*SP("gib usage: !gib [name|slot#]\n");
		return qfalse;*/
		doAll = qtrue;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
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
		SP(va("mute: %s\n", err));
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

qboolean G_shrubbot_slap(gentity_t *ent, int skiparg)
{
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
	G_AddEvent(vic, EV_GENERAL_SOUND, G_SoundIndex("sound/jetpilot/whack.wav"));
	
	// announce to the world
	AP(va("chat \"^dslap: ^*%s ^9was slapped^7\"", vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9slapped you%s%s\"", (ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
		(*reason) ? " ^9because:\n" : "", (*reason) ? reason : ""));
	return qtrue;
}
/*
qboolean G_shrubbot_teleport2crosshair(gentity_t *ent, int skiparg)
{
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t	*vic;

	if(Q_SayArgc() < 2+skiparg) 
	{
		SP("^dteleport usage: ^2!teleport ^9[name|slot#]^7\n");
		return qfalse;
	}

	Q_SayArgv(1 + skiparg, name, sizeof(name));
	
	if(ClientNumbersFromString(name, pids) != 1) 
	{
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("putatcrosshair: %s\n", err));
		return qfalse;
	}

	vic = &g_entities[pids[0]];

	// do here

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) 
	{
		SP("^dteleport: ^9player must be on a team^7\n");
		return qfalse;
	}

	return qtrue;
}

qboolean G_shrubbot_teleport2ent(gentity_t *ent, int skiparg)
{
	return qtrue;
}
*/

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
	{"r_showimages",		-1, -1},
	{"cg_shadows",			 1,  0}, // cos cg_shadows 2 bug
	{"disconnect",			-1, -1},
	{"connect",				-1, -1},
	{"quit",				-1, -1},
	//{"cl_yawspeed",			-1, -1},
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
	{"r_dynamicTextures",	-1, -1}, // prevent crashing
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
	{"cg_gunx",				-1, -1}, 
	{"cg_guny",				-1, -1}, 
	{"cg_gunz",				-1, -1}, 
	{"cl_maxpackets",		100, 15}, 
	{"rate",				25000, 15000},
	{"cg_fov",				120, 90},
	{"r_customHeight"		-1, -1},	
	{"r_customWidth"		-1, -1},
	{"ENDOFLIST",			-1, -1} // KEEP ME LAST!!!!!!!!!
};



qboolean G_shrubbot_clientcmd(gentity_t *ent, int skiparg)
{
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], cmd[MAX_NAME_LENGTH], arg[MAX_STRING_CHARS];
/*	char		*cmd;
	char		*arg;*/
	gentity_t	*vic;
	int			i;

	if(Q_SayArgc() < 2+skiparg) 
	{
		SP("^dclientcmd usage: ^2!clientcmd ^9[name|slot#] [cmd] [args]^7\n");
		return qfalse;
	}

	Q_SayArgv(1 + skiparg, name, sizeof(name));
	Q_SayArgv(2 + skiparg, cmd, sizeof(cmd));
	Q_SayArgv(3 + skiparg, arg, sizeof(arg));

	for ( i=0; protectedcmds[i].name; i++ )
	{
		
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
   
	if(ClientNumbersFromString(name, pids) != 1) 
	{
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
gentity_t *Weapon_Panzerfaust_Fire( gentity_t *ent, vec3_t dir );
qboolean G_shrubbot_rocket(gentity_t *ent, int skiparg)
{		
	int			numRockets = 1;	
	char		*arg;
	int			i;
	vec3_t		dir;
	
	if ( !ent || !ent->client )
		return qfalse; // jaquboss
	
//	Q_SayArgv(1 + skiparg, name, sizeof(name));
	arg = Q_SayConcatArgs(1 + skiparg);

	if (arg && *arg)
	{
		numRockets = atoi(arg);
	}    

	AngleVectors( ent->client->ps.viewangles, dir, NULL, NULL );

	for (i = 0; i < numRockets; i++)
        Weapon_Panzerfaust_Fire(ent, dir);

	return qtrue;
}

qboolean ReviveEntity(gentity_t *ent, gentity_t *traceEnt, qboolean resuscitation);
qboolean G_shrubbot_revive(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) 
	{
		SP("^drevive usage: ^2!revive ^9[name|slot#] [reason]^7\n");
		return qfalse;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) 
	{
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("revive: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) 
	{
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

void Weapon_Medic_Ext(gentity_t *ent, vec3_t viewpos, vec3_t tosspos, vec3_t velocity);
qboolean G_shrubbot_medpack(gentity_t *ent, int skiparg)
{
	vec3_t	launchspot;
	vec3_t	launchvel;

	if ( !ent )
		return qfalse;

	launchvel[0] = crandom();
	launchvel[1] = crandom();
	launchvel[2] = 0;

	VectorScale(launchvel, 100, launchvel);

	launchvel[2] = g_throwDistance.integer;

	VectorCopy(ent->r.currentOrigin, launchspot);		
	Weapon_Medic_Ext(ent, launchspot, launchspot, launchvel);
	
	return qtrue;
}

qboolean G_shrubbot_ammopack(gentity_t *ent, int skiparg)
{
	vec3_t	launchspot;
	vec3_t	launchvel;

	if ( !ent )
		return qfalse;

	launchvel[0] = crandom();
	launchvel[1] = crandom();
	launchvel[2] = 0;

	VectorScale(launchvel, 100, launchvel);

	launchvel[2] = g_throwDistance.integer;

	VectorCopy(ent->r.currentOrigin, launchspot);		
	Weapon_MagicAmmo_Ext(ent, launchspot, launchspot, launchvel);		
	
	return qtrue;
}

qboolean G_shrubbot_disguise(gentity_t *ent, int skiparg)
{
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	//char		*reason;
	gentity_t	*vic;

	if(Q_SayArgc() < 3+skiparg) 
	{
		SP("^ddisguise usage: ^2!disguise ^9[name|slot#] [class]^7\n");
		return qfalse;
	}

	Q_SayArgv(1+skiparg, name, sizeof(name));
	//reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) 
	{
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("disguise: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) 
	{
		SP("^ddisguise: ^9player must be on a team!^7\n");
		return qfalse;
	}

	Cmd_GiveDisguise_f(vic, 1+skiparg);
	
	return qtrue;
}

qboolean G_shrubbot_poison(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->ps.eFlags |= EF_POISONED;
				vic->client->pmext.poisonerEnt = vic->client->ps.clientNum;
				vic->client->pmext.poisonAmnt = 1;

			count++;
		}
		AP(va("chat \"^dpoison: ^9%d players poisoned^7\"", count));
		return qtrue;
	}

/*	if(Q_SayArgc() < 2+skiparg) 
	{
		SP("^dpoison usage: ^2!poison ^9[name|slot#] [reason]^7\n");
		return qfalse;
	}*/


	if(ClientNumbersFromString(name, pids) != 1) 
	{
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("poison: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) 
	{
		SP("^dpoison: ^9sorry, but your intended victim has a higher admin level than you do.^7\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES)) 
	{
		SP("^dpoison: ^9player must be on a team^7\n");
		return qfalse;
	}
	
	vic->client->ps.eFlags |= EF_POISONED;
	vic->client->pmext.poisonerEnt = vic->client->ps.clientNum;
	vic->client->pmext.poisonAmnt = 1;

	AP(va("chat \"^dpoison: ^7%s ^9was poisoned^7\"", vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^9poisoned you^7%s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));

	return qtrue;
}


qboolean G_shrubbot_glow(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
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
		SP("^dglow: ^9sorry, but your intended victim has a higher admin^"
			" level than you do.^7\n");
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

qboolean G_shrubbot_pants(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

/*	if(Q_SayArgc() < 2+skiparg) {
		SP("^dpants usage: ^2!pants ^9[name|slot#] [reason]^7\n");
		return qfalse;
	}*/
	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
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

qboolean G_shrubbot_freeze(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->freezed = qtrue;
			count++;
		}
		AP(va("chat \"^dfreeze: ^9%d players frozen^7\"", count));
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
	
	AP(va("chat \"^dfreeze: ^7%s ^9was frozen^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9froze you^7%s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_unfreeze(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
				vic->client->freezed = qfalse;
			count++;
		}
		AP(va("chat \"^dunfreeze: ^9%d players unfrozen^7\"", count));
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
	
	AP(va("chat \"^dunfreeze: ^7%s ^9was unfroze^7\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"^7%s ^9unfroze you^7%s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? "^9 because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_burn(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;
	qboolean doAll = qfalse;
	int count=0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
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
		SP("^dpants: ^9sorry, but your intended victim has a higher admin^"
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

qboolean G_shrubbot_warn(gentity_t *ent, int skiparg)
{
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
	
	G_AddEvent(vic, EV_GENERAL_SOUND,
			G_SoundIndex("sound/misc/referee.wav"));

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

qboolean G_shrubbot_give(gentity_t *ent, int skiparg)
{
	int			pids[MAX_CLIENTS];
	char		name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char		*item;
	char		*amt;
	char		*amt2;
	gentity_t	*vic;

	if(Q_SayArgc() < 2+skiparg) 
	{
		SP("^dgive usage: ^2!give ^9[name|slot#] [item] [args]^7\n");
		return qfalse;
	}

	Q_SayArgv(1 + skiparg, name, sizeof(name));
	item = Q_SayConcatArgs(2 + skiparg);

	amt = Q_SayConcatArgs(3 + skiparg);
 	amt2 = Q_SayConcatArgs(4 + skiparg);
   
	if(ClientNumbersFromString(name, pids) != 1) 
	{
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("give: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	ExecGive( vic, item, amt?amt:"", amt2?amt2:"");


	return qtrue;
}

qboolean G_shrubbot_news(gentity_t *ent, int skiparg)
{
	char mapname[MAX_STRING_CHARS];

	if(Q_SayArgc() < 2+skiparg) {
		Q_strncpyz(mapname, level.rawmapname,
				sizeof(mapname));
	} else {
		Q_SayArgv(1+skiparg, mapname, sizeof(mapname));
	}

	G_globalSound(va("sound/vo/%s/news_%s.wav",mapname,mapname));

	return qtrue;
}

qboolean G_shrubbot_lock(gentity_t *ent, int skiparg)
{
	return G_shrubbot_lockteams(ent, skiparg, qtrue);
}

qboolean G_shrubbot_unlock(gentity_t *ent, int skiparg)
{
	return G_shrubbot_lockteams(ent, skiparg, qfalse);
}

qboolean G_shrubbot_lockteams(gentity_t *ent, int skiparg, qboolean toLock)
{
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
		} else {
			level.server_settings &= ~CV_SVS_LOCKSPECS;
		}
		AP(va("chat \"^d%s: ^9All teams %sed^7\" -1", cmd, cmd));
	} else if ( !Q_stricmp(team, "r") ) {
		teamInfo[TEAM_AXIS].team_lock = 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse;
		AP(va("chat \"^d%s: ^9Axis team %sed^7\" -1", cmd, cmd));
	} else if ( !Q_stricmp(team, "b") ) {
		teamInfo[TEAM_ALLIES].team_lock = 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse;
		AP(va("chat \"^d%s: ^9Allied team %sed^7\" -1", cmd, cmd));
	} else if ( !Q_stricmp(team, "s") ) {
		G_updateSpecLock(TEAM_AXIS, 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse);
		G_updateSpecLock(TEAM_ALLIES, 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse);
		if( toLock ) {
			level.server_settings |= CV_SVS_LOCKSPECS;
		} else {
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
qboolean G_shrubbot_lol(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], numNades[4];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, nades = 0, count = 0;

	if(Q_SayArgc() < 2+skiparg) doAll = qtrue;
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
		int it;
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
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP(va("^dnade: ^9sorry, but ^7%s^9 has a higher "
					"admin level than you do.^7\n", 
					vic->client->pers.netname));
				continue;
			} else if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
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
	} else if( pcount < 1 ) {
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
/*	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^olol: ^7player must be on a team to be lol'd\n");
		return qfalse;
	}*/
	
	if( nades )
		G_createClusterNade( vic, 2);//nades );
	else
		G_createClusterNade( vic, 1 );
	AP(va("chat \"^dnade: ^7%s^7\"", vic->client->pers.netname));
	return qtrue;

}
													
// Created by: dvl
qboolean G_shrubbot_pop( gentity_t *ent, int skiparg )
{
	vec3_t dir = { 5, 5, 5 };
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, count = 0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
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
	} else if(pcount < 1) {
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

qboolean G_shrubbot_420( gentity_t *ent, int skiparg )
{
//	vec3_t dir = { 5, 5, 5 };
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, count = 0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) || !(vic->client->sess.sessionTeam == TEAM_AXIS || vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
						
			G_420(vic);
			count++;
		}
		AP(va("chat \"^d420: ^9%d players shared a blunt^7\"", count));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP( va("^d420: ^9sorry, but ^7%s^9 has a higher admin level than you do.^7\n", 
					vic->client->pers.netname));
				continue;
			}
/*
			if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 		vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^o420: ^7%s^7 must be on a team"
					"to be popped\n", 
					vic->client->pers.netname));
				continue;
			}
*/
			
			G_420(vic);
			AP(va("chat \"^d420: ^7%s ^9smoked a joint^7\"", 
				vic->client->pers.netname));
		}
		return qtrue;
	} else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^d420: ^7%s^7\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, vic)) {
		SP( "^d420: ^9sorry, but your intended victim has a higher admin level than you do.^7\n" );
		return qfalse;
	}
/*	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^o420: ^7player must be on a team to be popped\n");
		return qfalse;
	}*/
	
	G_420(vic);
	AP(va("chat \"^d420: ^7%s ^9smoked a joint^7\"", vic->client->pers.netname));

	return qtrue;
}


// Created by: dvl
qboolean G_shrubbot_pip( gentity_t *ent, int skiparg )
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, count = 0;

	if(Q_SayArgc() < 2+skiparg)
		doAll = qtrue;
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
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
	} else if(pcount < 1) {
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

qboolean G_shrubbot_reset(gentity_t *ent, int skiparg)
{
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

qboolean G_shrubbot_swap(gentity_t *ent, int skiparg)  
{  
	Svcmd_SwapTeams_f();  
	AP("chat \"^dswap: ^9Teams were swapped^7\"");  
	return qtrue;  
}  

qboolean G_shrubbot_fling(gentity_t *ent, int skiparg)
{
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
	} else {
		flingCmd = &fling[0];
	}

	if(!Q_stricmp(flingCmd, "throw")) {
		flingType = 1;
		Q_strncpyz(pastTense, "thrown", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "launch")) {
		flingType = 2;
		Q_strncpyz(pastTense, "launched", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "fling")) {
		flingType = 0;
		Q_strncpyz(pastTense, "flung", sizeof(pastTense));
	} /*else if (!Q_stricmp(flingCmd, "throwa")) {
		doAll = qtrue;
		flingType = 1;
		Q_strncpyz(pastTense, "thrown", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "launcha")) {
		doAll = qtrue;
		flingType = 2;
		Q_strncpyz(pastTense, "launched", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "flinga")) {
		doAll = qtrue;
		flingType = 0;
		Q_strncpyz(pastTense, "flung", sizeof(pastTense));
	}*/

	if( /*doAll! &&*/ Q_SayArgc() < 2+skiparg) {
		/*SP(va("%s usage: %s [name|slot#]\n", fling, fling));
		return qfalse;*/
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


qboolean G_shrubbot_disorient(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

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

qboolean G_shrubbot_orient(gentity_t *ent, int skiparg)
{
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

qboolean G_shrubbot_resetxp(gentity_t *ent, int skiparg) 
{
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

	AP(va("chat \"^dresetxp: ^9XP has been reset for player %s\"",
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
	if(!ent || !ent->client)  
	return qfalse;  
	G_ResetXP(ent);  
	SP("^dresetmyxp: ^9you have reset your XP\n");  
	return qtrue;  
}  

qboolean G_shrubbot_nextmap(gentity_t *ent, int skiparg)  
{  
	// copied from G_Nextmap_v() in g_vote.c  
	g_campaignInfo_t *campaign;  
	if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {  
			campaign = &g_campaigns[level.currentCampaign];  
					if( campaign->current + 1 < campaign->mapCount ) {  
                         trap_Cvar_Set("g_currentCampaignMap",  
                                         va( "%i", campaign->current + 1));  
                         trap_SendConsoleCommand(EXEC_APPEND,  
                                 va( "map %s\n",  
                                 campaign->mapnames[campaign->current + 1]));  
                         AP("cp \"^3*** Loading next map in campaign ***\n\"");  
                 } else {  
                         // Load in the nextcampaign  
                         trap_SendConsoleCommand(EXEC_APPEND,   
                                         "vstr nextcampaign\n");  
                         AP("cp \"^3*** Loading Next Campaign ***\n\"");  
                 }  
         } else {  
                 // Load in the nextmap  
                 trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");  
                 AP("cp \"^3*** Loading Next Map ***\n\"");  
         }  
         AP("chat \"^dnextmap: ^9Next map was loaded\"");  
         return qtrue;  
}  



/*
 * This function facilitates the SP define.  SP() is similar to CP except that
 * it prints the message to the server console if ent is not defined.
 */
void G_shrubbot_print(gentity_t *ent, char *m) 
{

	if(ent) CP(va("print \"%s\"", m));
	else {
		char m2[MAX_STRING_CHARS];
		DecolorString(m, m2);
		G_Printf(m2);
	}
}

void G_shrubbot_buffer_begin()   
{  
	bigTextBuffer[0] = '\0';  
	}  
	  
	void G_shrubbot_buffer_end(gentity_t *ent)   
	{  
		SP(bigTextBuffer);        
	}  
	  
	void G_shrubbot_buffer_print(gentity_t *ent, char *m)  
	{  
		// tjw: 1022 - strlen("print 64 \"\"") - 1   
		if(strlen(m) + strlen(bigTextBuffer) >= 1009) {  
				SP(bigTextBuffer);  
				bigTextBuffer[0] = '\0';  
	}  
		Q_strcat(bigTextBuffer, sizeof(bigTextBuffer), m);  
	}

void G_shrubbot_cleanup()  
{  
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

// Jaybird
int G_shrubbot_adminlevel( gentity_t *ent ) {
	int i;
	char *guid;
	long hash;
	char userinfo[MAX_INFO_STRING];

	// Get GUID and hash
	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ));
	guid = Info_ValueForKey( userinfo, "cl_guid" );

	// Do not run if no GUID
	if( !*guid )
		return 0;

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
qboolean G_shrubbot_finger( gentity_t *ent, int skiparg )
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *guid, *ip, *tmp;
	gentity_t *vic;
	char userinfo[MAX_INFO_STRING];

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

	vic = &g_entities[pids[0]];

	trap_GetUserinfo( pids[0], userinfo, sizeof( userinfo ));
	guid = Info_ValueForKey( userinfo, "cl_guid" );
	ip = Info_ValueForKey( userinfo, "ip" );

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
	SP( va( "^dSlot:  ^2%i\n", vic->client->ps.clientNum ));
	SP( va( "^dAdmin: ^2%i\n", G_shrubbot_adminlevel( vic )));
	SP( va( "^dGUID:  ^2%s\n", guid ));
	SP( va( "^dIP:    ^2%s\n", ip ));

	return qtrue;
}

/*
====================
!uptime
--------------------
Jaybird
====================
*/
qboolean G_shrubbot_uptime( gentity_t *ent, int skiparg )
{
	int rawtime;
	int days;
	int hours;
	int minutes;

	rawtime = level.time / 1000;
	days = rawtime / (60 * 60 * 24);
	rawtime -= (days * 60 * 60 * 24);
	hours = rawtime / (60 * 60);
	rawtime -= (hours * 60 * 60);
	minutes = rawtime / 60;

	AP(va("chat \"^duptime: ^2%i days, %i hours, %i minutes.", days, hours, minutes));
	return qtrue;
}
