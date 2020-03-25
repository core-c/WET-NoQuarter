
#include "g_local.h"
#include "q_shared.h"
#include "g_time.h"
#include "g_osfile.h"
#include <errno.h>

#ifdef OMNIBOTS
#include "../game/g_etbot_interface.h"
#endif

static void G_xpsave_init_struct(g_xpsave_t* data)
{
	if ( data == NULL )
		return;
	// Initialise everything to zero
	memset(data, 0, sizeof(data));
	// Set the signature and the version
	data->signature			= XP_SAVE_SIGNATURE;
	data->version			= XP_SAVE_CUR_VER;
}

static char* G_xpsave_get_path(char const* guid)
{
	static char							path[MAX_PATH];

	// This check should be done externally to this function but just be sure!
	if ( !g_XPSaveDirectory.string[0] || guid == NULL )
		return NULL;

	// Build file path
	G_BuildFilePath(g_XPSaveDirectory.string, guid, XP_SAVE_FILE_EXT, path, MAX_PATH);

	// Return the path
	return path;
}

static qboolean G_xpsave_write(g_xpsave_t* data, char const* path)
{
	// Ensure we have a path and some data
	if ( path == NULL || data == NULL )
		return qfalse;

	// Write data to file
	if ( -1 == G_WriteDataToFile(path, (char*)data, sizeof(g_xpsave_t) ) )
		return qfalse;

	// Success
	return qtrue;
}

static qboolean G_xpsave_valid_data(g_xpsave_t* data)
{
	return (data && (data->signature == XP_SAVE_SIGNATURE || LongSwap(data->signature) == XP_SAVE_SIGNATURE));
}

static void G_xpsave_fix_byte_order(g_xpsave_t* data)
{
	// We assume that the data is valid...
	if ( data == NULL )
		return;

	// If it's already in the correct byte order then don't do anything...
	if ( data->signature == XP_SAVE_SIGNATURE )
		return;

	// Swap byte order
	data->signature	= XP_SAVE_SIGNATURE;
	data->timestamp	= LongSwap(data->timestamp);
	data->version	= LongSwap(data->version);
}

static qboolean G_xpsave_entry_has_expired(g_xpsave_t* data, int* xp_age)
{
	time_t								t		= G_GetCurrentTime();
	int									age;

	// We assume that the data is valid...
	if ( data == NULL )
		return qfalse;

	// Calculate the time since we saved out the xpsave data
	age = t - data->timestamp;

	// Return the age of the xp via the parameter - if it's not NULL
	if ( xp_age != NULL )
		*xp_age = age;

	// Check if the age has expired?
	return (age > g_XPSaveMaxAge.integer) ? qtrue : qfalse;
}

qboolean G_xpsave_load(gentity_t *ent)
{
	g_xpsave_t							data;
	char*								path;
	int									i;
	char 								agestr[MAX_STRING_CHARS];
 	char*								guid;
	int									xp_age;

	// Check the XP Save modes...
	if ( !(g_XPSave.integer & XPSF_ENABLE) )
		return qfalse;

	if ( g_gametype.integer == GT_WOLF_STOPWATCH  && (g_XPSave.integer & XPSF_DISABLE_STOPWATCH) ) {
		return qfalse;
	}

	// Check the entity is valid
	if ( !ent || !ent->client )
		return qfalse;

	// Retrieve the client pb guid
	guid = ent->client->pers.cl_guid;

	// Check we have a pb guid
	if ( !guid[0] || !Q_strncmp(guid, "unknown", 7) )
	{
		G_Printf("G_xpsave_load: invalid guid \"%s\"\n", guid);
		return qfalse;
	}

	// Additional sanity checks
	if ( !g_XPSaveDirectory.string[0] )
	{
		G_Printf("G_xpsave_load: g_XPSaveDirectory is not configured. Not saving xp.\n");
		return qfalse;
	}
	if ( qfalse == G_IsDirectory(g_XPSaveDirectory.string) )
	{
		G_Printf("G_xpsave_load: g_XPSaveDirectory(%s) is not a valid directory path.\n", g_XPSaveDirectory.string);
		return qfalse;
	}

	// Get the path
	path = G_xpsave_get_path(guid);
	if ( path == NULL )
		return qfalse;

	// If the file doesn't exist then don't try and open it
	if ( qfalse == G_IsFile(path) )
		return qfalse;

	// Read the data from the file
	if ( -1 == G_ReadDataFromFile(path, (char*)&data, sizeof(data)) )
	{
		G_Printf("G_xpsave_load: failed to read xpsave file: %s\n", path);
		return qfalse;
	}

	// Check the signature
	if ( qfalse == G_xpsave_valid_data(&data) )
	{
		G_Printf("G_xpsave_load: xpsave data file has invalid signature: %s\n", path);
		return qfalse;
	}

	// Fix byte order - if necessary
	G_xpsave_fix_byte_order(&data);

	// Check if the age has expired?
	if ( qtrue == G_xpsave_entry_has_expired(&data, &xp_age) )
	{
		CP("print \"^3server: your saved xp has expired :(\n");
		return qfalse;
	}

	// Set the player skillpoints and xp
	for ( i = 0; i < SK_NUM_SKILLS; ++i )
	{
		ent->client->sess.skillpoints[i] = data.skillpoints[i];
		ent->client->sess.startxptotal += data.skillpoints[i];
	}
	ent->client->ps.stats[STAT_XP] = (int)ent->client->sess.startxptotal;

	// Handle XP decay
	if ( (g_XPDecay.integer & XPDF_ENABLE) && !(g_XPDecay.integer & XPDF_NO_DISCONNECT_DECAY) )
		G_XPDecay(ent, xp_age, qtrue);

	G_CalcRank(ent->client);
	BG_PlayerStateToEntityState(&ent->client->ps,
			&ent->s,
			level.time,
			qtrue);

	// Let the client know we restored their stats
	G_shrubbot_duration(xp_age, agestr, sizeof(agestr));
	CP(va(
		"print \"^3server: retrieved xp save state from %s ago\n\"",
		agestr));

	return qtrue;
}

qboolean G_xpsave_add(gentity_t *ent)
{
	g_xpsave_t							data;
	time_t								t;
 	char*								guid;

	// Check the XP Save modes...
	if ( !(g_XPSave.integer & XPSF_ENABLE) )
		return qfalse;

	if ( g_gametype.integer == GT_WOLF_STOPWATCH  && (g_XPSave.integer & XPSF_DISABLE_STOPWATCH) ) {
		return qfalse;
	}

	// Check the entity is valid
	if(!ent ||
	   !ent->client ||
	   ent->client->pers.connected != CON_CONNECTED)
		return qfalse;

#ifdef OMNIBOTS
	if ((g_OmniBotFlags.integer & OBF_DONT_XPSAVE) && (ent->r.svFlags & SVF_BOT))
		return qfalse;
#endif

	// Retrieve the client pb guid
	guid = ent->client->pers.cl_guid;

	// Check we have a pb guid
	if ( !guid[0] )
	{
		G_Printf("G_xpsave_add: invalid guid.\n");
		return qfalse;
	}

	// Additional sanity checks
	if ( !g_XPSaveDirectory.string[0] )
		return qfalse;
	if ( qfalse == G_IsDirectory(g_XPSaveDirectory.string) )
	{
		G_Printf("G_xpsave_add: g_XPSaveDirectory(%s) is not a valid directory path.\n", g_XPSaveDirectory.string);
		return qfalse;
	}

	// Get the current time (dont use time() I suspect it's slow)
	t = G_GetCurrentTime();

	// Create our output data structure
	G_xpsave_init_struct(&data);
	data.timestamp			= t;
	Q_strncpyz(data.netname, ent->client->pers.netname, sizeof(data.netname));
	memcpy(data.skillpoints, ent->client->sess.skillpoints, sizeof(data.skillpoints));

	// Write data to file
	if ( qfalse == G_xpsave_write(&data, G_xpsave_get_path(guid)) )
	{
		G_Printf("G_xpsave_add: failed to write xpsave data for: %s\n", guid);
		return qfalse;
	}

	G_Printf("G_xpsave_add: saved xp for %s\n", ent->client->pers.netname);
	return qtrue;
}

void G_xpsave_writestats()
{
	int			i;
	for ( i = 0; i < level.numConnectedClients; i++ )
		G_xpsave_add(&g_entities[level.sortedClients[i]]);
}

static qboolean G_xpsave_wipe_xp_file(char const* file, char const* path, qboolean directory)
{
	char*								s;
	g_xpsave_t							data;

	// Skip directories
	if ( directory == qtrue )
		return qtrue;

	// Check the file ends with our xp save file extension.. skip it if not
	s = Q_strrchr(file, '.');
	if ( s == NULL || Q_strncmp(s, XP_SAVE_FILE_EXT, strlen(XP_SAVE_FILE_EXT)) )
		return qtrue;

	// Check the validity of the file...
	if ( -1 == G_ReadDataFromFile(path, (char*)&data, sizeof(data)) )
	{
		G_Printf("G_xpsave_wipe_xp_file: failed to read xpsave file: %s, not wiping file\n", path);
		return qtrue;
	}
	if ( qfalse == G_xpsave_valid_data(&data) )
		return qtrue;

	// Ok we've made a damn good attempt at verifying this is an xp save file
	if ( qfalse == G_DeleteFile(path) )
		G_Printf("G_xpsave_wipe_xp_file: failed to wipe xp save file: %s\n", path);

	return qtrue;
}

void G_xpsave_resetxp()
{
	// Check the save directory is set before wiping the xpsave files...
	if ( g_XPSaveDirectory.string[0] )
		G_IterateDirectory(g_XPSaveDirectory.string, G_xpsave_wipe_xp_file);
}

static void G_xpsave_convert_old_config()
{
	static qboolean 		performed_conversion	= qfalse;
	fileHandle_t 			f;
	int 					len;
	char*					xpsave_data				= NULL;
	char*					next_token				= NULL;
	char*					t						= NULL;
	int						converted_items			= 0;
	qboolean				parsing_xpsave			= qfalse;
	g_xpsave_t				xpsave_entry;
	char					xpsave_guid[MAX_GUID_LENGTH+1];

	// Have we already performed the conversion?
	if ( performed_conversion == qtrue )
		return;
	performed_conversion = qtrue;

	// Don't convert the file if they've not got xp save enabled...
	if( !(g_XPSave.integer & XPSF_ENABLE) )
		return;

	if ( g_gametype.integer == GT_WOLF_STOPWATCH  && (g_XPSave.integer & XPSF_DISABLE_STOPWATCH) ) {
		return;
	}

	// Open the xp save file
	len = trap_FS_FOpenFile("xpsave.cfg", &f, FS_READ) ;
	if ( len < 0 )
	{
		G_Printf("G_xpsave_convert_old_config: could not open 'xpsave.cfg'. If it was "
			"already converted then it will have been renamed to xpsave.cfg.converted. You "
			"should probably remove the g_XPSaveFile variable as it's not longer used!\n");
		return;
	}

	// Allocate memory to read in the file...
	next_token = xpsave_data = malloc(len + 1);
	if ( NULL == xpsave_data )
	{
		G_Printf("G_xpsave_convert_old_config: unable to malloc memory for xpsave file data\n");
		trap_FS_FCloseFile(f);
		return;
	}

	// Read in the data into our buffer...
	trap_FS_Read(xpsave_data, len, f);

	// Ensure zero terminated
	*(xpsave_data + len) = '\0';

	// Close the file
	trap_FS_FCloseFile(f);

	// Initialise our xpsave entry data
	xpsave_guid[0] = 0;
	G_xpsave_init_struct(&xpsave_entry);

	// Parse the xp save file data
	while( *(t = COM_Parse(&next_token)) )
	{
		if ( !Q_stricmp(t, "[xpsave]") )
		{
			if ( parsing_xpsave == qfalse )
			{
				// Ok found our first entry....
				parsing_xpsave = qtrue;
				continue;
			}
			else
			{
				// Only valid if the guid is set..
				if ( xpsave_guid[0] != 0 )
				{
					// Output the xpsave data to our shared file format as long as it doesn't
					// already exist (don't overwrite anything!)
					char*					path	= G_xpsave_get_path(xpsave_guid);
					if ( NULL != path && qfalse == G_IsFile(path) )
					{
						// Write data to file
						if ( qfalse == G_xpsave_write(&xpsave_entry, path) )
							G_Printf("G_xpsave_convert_old_config: failed to write xpsave data for: %s\n", xpsave_guid);
						else
							++converted_items;
					}
				}

				// Wipe the current data structure to reset for the next entry
				xpsave_guid[0] = 0;
				G_xpsave_init_struct(&xpsave_entry);
			}
		}
		else if ( parsing_xpsave == qtrue )
		{
			// Parse each entry in the xpsave record and store them into out local structures
			if ( !Q_stricmp(t, "guid") )
				G_shrubbot_readconfig_string(&next_token, xpsave_guid, sizeof(xpsave_guid));
			else if ( !Q_stricmp(t, "name") )
				G_shrubbot_readconfig_string(&next_token, xpsave_entry.netname, sizeof(xpsave_entry.netname));
			else if ( !Q_stricmp(t, "time") )
				G_shrubbot_readconfig_int(&next_token, (int*)&xpsave_entry.timestamp);
			else if ( !Q_stricmpn(t, "skill[", 6) )
			{
				int				i;
				for ( i = 0; i < SK_NUM_SKILLS; ++i )
				{
					if ( !Q_stricmp(t, va("skill[%i]", i)) )
					{
						float	skill;
						G_shrubbot_readconfig_float(&next_token, &skill);
						xpsave_entry.skillpoints[i] = skill;
					}
				}
			}
			else if ( !Q_stricmp(t, "muted") )
			{
				// We're actually going to ignore this - but we need to read it to proceed with the parsing
				int				muted;
				G_shrubbot_readconfig_int(&next_token, &muted);
			}
			else
			{
				G_Printf("G_xpsave_convert_old_config: [xpsave] parse error near %s on line %d\n",
					t,
					COM_GetCurrentParseLine());
			}
		}
	} // while parsing tokens...

	// Free the allocated memory for the xp save data
	free(xpsave_data);

	// Output our success
	G_Printf("G_xpsave_convert_old_config: converted %d xpsave %s from the old format to the new shared format!\n",
		converted_items,
		converted_items == 1 ? "entry" : "entries");

	// Rename the xp save file so that we don't do it again...
	trap_FS_Rename("xpsave.cfg", va("%s.converted", "xpsave.cfg"));
}

static qboolean G_xpsave_cleanup_expired_xp_file(char const* file, char const* path, qboolean directory)
{
	char*								s;
	g_xpsave_t							data;

	// Skip directories
	if ( directory == qtrue )
		return qtrue;

	// Check the file ends with our xp save file extension.. skip it if not
	s = Q_strrchr(file, '.');
	if ( s == NULL || Q_strncmp(s, XP_SAVE_FILE_EXT, strlen(XP_SAVE_FILE_EXT)) )
		return qtrue;

	// Check the signature in the file...
	if ( -1 == G_ReadDataFromFile(path, (char*)&data, sizeof(data)) )
	{
		G_Printf("G_xpsave_cleanup_expired_xp_file: failed to read xpsave file: %s\n", path);
		return qtrue;
	}
	if ( qfalse == G_xpsave_valid_data(&data) )
		return qtrue;

	// Fix the byte order if necessary
	G_xpsave_fix_byte_order(&data);

	// Check if the entry has expired?
	if ( qfalse == G_xpsave_entry_has_expired(&data, NULL) )
		return qtrue;

	// Cleanup the expired file
	if ( qfalse == G_DeleteFile(path) )
		G_Printf("G_xpsave_cleanup_expired_xp_file: failed to wipe xp save file: %s\n", path);
	else
		G_Printf("G_xpsave_cleanup_expired_xp_file: deleted expired xp save file for: %s\n", data.netname);

	return qtrue;
}

static void G_xpsave_cleanup()
{
	// Check the save directory is set...
	if ( g_XPSaveDirectory.string[0] )
		G_IterateDirectory(g_XPSaveDirectory.string, G_xpsave_cleanup_expired_xp_file);
}

void G_xpsave_init_game()
{
	// Nothing to do if they've not got xp save enabled...
	if( !(g_XPSave.integer & XPSF_ENABLE) )
		return;

	if ( g_gametype.integer == GT_WOLF_STOPWATCH  && (g_XPSave.integer & XPSF_DISABLE_STOPWATCH) ) {
		return;
	}

	// If the XPSF_CONVERT flag is set then we attempt to perform a conversion
	// from the old format to the new shared format.
	if ( g_XPSave.integer & XPSF_CONVERT )
		G_xpsave_convert_old_config();

	// Cleanup expired files
	if ( g_XPSaveDirectory.string[0] )
		G_xpsave_cleanup();
}
