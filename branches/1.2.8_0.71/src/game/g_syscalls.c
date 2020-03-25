// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"

// this file is only included when building a dll
// g_syscalls.asm is included instead when building a qvm

// IRATA: Parts of documentation are taken from http://www.quake3world.com/ubb/Forum4/HTML/005563.html - THX !

static int (QDECL *syscall)( int arg, ... ) = (int (QDECL *)( int, ...))-1;

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif
void dllEntry( int (QDECL *syscallptr)( int arg,... ) ) {
	syscall = syscallptr;
}
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif

int PASSFLOAT( float x ) {
	float	floatTemp;
	floatTemp = x;
	return *(int *)&floatTemp;
}

/*
=================
trap_Printf
const char *fmt: Text to be displayed

Prints the string pointed to by fmt to the Console.
This should not usually be used directly by the user -
the G_Printf function provides its functionality and more.
=================
*/
void	trap_Printf( const char *fmt ) {
	syscall( G_PRINT, fmt );
}


/*
=================
trap_Error
const char *fmt: Text to be displayed

This is very simliar to the trap_Printf function with the exception that
this will halt execution of the module it is called from.
Usually the user shouldn't call this directly, but using G_Error instead.
=================
*/
void	trap_Error( const char *fmt ) {
	syscall( G_ERROR, fmt );
}

/*
=================
trap_Milliseconds

Returns the number of milliseconds that have elapsed since Quake 3 was executed.
A Quake 3, multi-platform version of the Win32 function timeGetTime,
which also returns the number of milliseconds since the program was executed.
=================
*/
int		trap_Milliseconds( void ) {
	return syscall( G_MILLISECONDS );
}


/*
=================
trap_Argc

Whenever a client issues a console command it can have arguments passed with it.
This function returns the number of arguments identically to the argc found in C programs with this prototype
for main:

int main( int argc, char **argv )
=================
*/
int		trap_Argc( void ) {
	return syscall( G_ARGC );
}


/*
=================
trap_Argv

int n: The argument number to return.
char *buffer: A point to a buffer to fill.
int bufferLength: The size of the buffer.

This will fill the buffer pointed to by buffer with an argument to a client command where n is the
argument to return. 0 is the command itself, 1 is the first argument and so on... Very
similiar to char **argv in a C program.
=================
*/
void	trap_Argv( int n, char *buffer, int bufferLength ) {
	syscall( G_ARGV, n, buffer, bufferLength );
}


/*
=================
trap_FS_FOpenFile

const char *qpath: The file to be opened.
fileHandle_t *f: Reference to a local fileHandle_t.
fsMode_t mode: Mode to open the file in.

This opens a file specified by the string qpath and allocates a fileHandle_t (just a typedef int) to that file.
This is then used in subsequent operations involving the file. The fsMode_t enumeration refers to the mode the
file is opened in. FS_READ for reading the file. FS_WRITE will create the file qpath if it doesn't exist or 0
length it if it does. FS_APPEND allows writing to the end of an existing file.
FS_APPEND_SYNC is similar to FS_APPEND except you can read from the file at the same time.

An int is returned representing the length of the file being opened.
=================
*/
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	return syscall( G_FS_FOPEN_FILE, qpath, f, mode );
}


/*
=================
trap_FS_Read

void *buffer: A buffer to fill.
int len: The amount to read from the file.
fileHandle_t: A file handle provided by trap_FS_FOpenFile.

This reads len bytes of data from the file specified by f and fills buffer with it.
=================
*/
void	trap_FS_Read( void *buffer, int len, fileHandle_t f ) {
	syscall( G_FS_READ, buffer, len, f );
}


/*
=================
trap_FS_Write

const void *buffer: A buffer to read from.
int len: The amount to write to the file.
fileHandle_t: A file handle provided by trap_FS_FOpenFile.

This writes len bytes of data from buffer to the file specified by f.
=================
*/
int		trap_FS_Write( const void *buffer, int len, fileHandle_t f ) {
	return syscall( G_FS_WRITE, buffer, len, f );
}

/*
=================
trap_FS_Rename
=================
*/
int		trap_FS_Rename( const char *from, const char *to ) {
	return syscall( G_FS_RENAME, from, to );
}

/*
=================
trap_FS_FCloseFile

fileHandle_t f: The file to be closed.

Closes the file specified by f.
=================
*/
void	trap_FS_FCloseFile( fileHandle_t f ) {
	syscall( G_FS_FCLOSE_FILE, f );
}


/*
=================
trap_FS_GetFileList

const char *path: String reflecting a directory.
const char *extension: File extension to list (incluing a leading period).
char *listbuf: Pointer to a buffer to fill.
int bufsize: Length of the buffer.

This fills listbuf with a NULL delimited list of all the files in the directory specified by path ending in extension.
The number of files listed is returned.
=================
*/
int trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize ) {
	return syscall( G_FS_GETFILELIST, path, extension, listbuf, bufsize );
}


/*
=================
trap_SendConsoleCommand

int exec_when: When to execute this command.
const char *text: The command to be executed.

This call adds a command to the command buffer i.e. so that the programmer may issue server commands
that would normally need to be entered into the console.
exec_when takes an element of the cbufExec_t enumeration:

    code:
    // paramters for command buffer stuffing
    typedef enum {
      EXEC_NOW,     // don't return until completed, a VM should NEVER use this,
                    // because some commands might cause the VM to be unloaded...
      EXEC_INSERT,  // insert at current position, but don't run yet
      EXEC_APPEND   // add to end of the command buffer (normal case)
    } cbufExec_t;

=================
*/
void	trap_SendConsoleCommand( int exec_when, const char *text ) {
	syscall( G_SEND_CONSOLE_COMMAND, exec_when, text );
}

/*
=================
G_CvarUpdateSafe
=================
*/
qboolean G_CvarUpdateSafe(const char *var_name, const char *value)
{
	const char *serverinfo_cvars[] = {
		"mod_url",
		"mod_version",
		"g_tyranny",
		"P",
		"voteFlags",
		"g_balancedteams",
		"g_bluelimbotime",
		"g_redlimbotime",
		"gamename",
		"g_gametype",
		"g_voteFlags",
		"g_alliedmaxlives",
		"g_axismaxlives",
		"g_minGameClients",
		"g_needpass",
		"sv_allowAnonymous",
		"sv_privateClients",
		"mapname",
		"protocol",
		"version",
		"g_maxlivesRespawnPenalty",
		"g_maxGameClients",
		"sv_maxclients",
		"timelimit",
		"g_heavyWeaponRestriction",
		"g_antilag",
		"g_maxlives",
		"g_friendlyFire",
		"sv_floodProtect",
		"sv_maxPing",
		"sv_minPing",
		"sv_maxRate",
		"sv_minguidage",
		"sv_punkbuster",
		"sv_hostname",
		"Players_Axis",
		"Players_Allies",
		"campaign_maps",
		"C",
		"g_medicChargeTime"
		"g_LTChargeTime"
		"g_engineerChargeTime"
		"g_soldierChargeTime"
		"g_covertopsChargeTime",
		NULL
	};
	char cs[MAX_INFO_STRING] = {""};
	int i = 0;
	int size = 0;

	if(!var_name)
		return qtrue;
	if(!value)
		return qtrue;

	for(i=0; serverinfo_cvars[i]; i++) {
		if(!Q_stricmp(serverinfo_cvars[i], var_name)) {
			trap_GetServerinfo(cs, sizeof(cs));
			size = strlen(cs);
			Info_SetValueForKey(cs, var_name, value);
			if((size + 1) >= MAX_INFO_STRING) {
				G_Printf("WARNING: skipping SERVERINFO cvar "
					"set for %s (MAX_INFO_STRING "
					"protection)\n",
					var_name);
				return qfalse;
			}
			// tjw: q3 engine always makes the first char of
			//      GAMESTATE null
			if((level.csLenTotal + size + 1 - level.csLen[0])
				>= (MAX_GAMESTATE_CHARS - 1)) {

				G_Printf("WARNING: skipping SERVERINFO cvar "
					"set for %s (MAX_GAMESTATE_CHARS "
					"protection)\n",
					var_name);
				return qfalse;
			}
			level.csLenTotal += (size - level.csLen[0] + 1);
			level.csLen[0] = (size + 1);
			return qtrue;
		}
	}
	return qtrue;
}


/*
=================
trap_Cvar_Register

vmCvar_t *cvar: A pointer the location in which to store the local data.
const char *var_name: A null-terminated string that will be used to set the vmCvar's name field.
const char *value: A null-terminated string that will be used to set both the floating point and integer value fields of the vmCvar.

This function is used to create a new vmCvar.
It sends all the information to the engine's main database and then it stores a local copy of the data into
the vmCvar_t passed as the first argument.
=================
*/
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags ) {
        if(!G_CvarUpdateSafe(var_name, value)) {
				syscall( G_CVAR_REGISTER, cvar, var_name, "", flags );
		}
		else {
				syscall( G_CVAR_REGISTER, cvar, var_name, value, flags );
		}
}


/*
=================
trap_Cvar_Update

vmCvar_t *cvar: A pointer to the vmCvar to update.

This function first parses out the name of the vmCvar and then searches for the corresponding database entry.
Once it finds the vmCvar it will copy the "master copy" of the vmCvar into the local address provided.
=================
*/
void	trap_Cvar_Update( vmCvar_t *cvar ) {
	syscall( G_CVAR_UPDATE, cvar );
}


/*
=================
trap_Cvar_Set

const char *var_name: A null-terminated string containing the name of the vmCvar to set.
const char *value: A null-terminated string containing the desired value.

This function searchs for the name of the vmCvar and stores the value into both the float and integer
fields of the corresponding vmCvar entry. The only way to effectly change the value of a vmCvar is to use
this function, the ANSI storage operator only modifies the local buffer and not the "master copy".
=================
*/
void trap_Cvar_Set( const char *var_name, const char *value ) {
	syscall( G_CVAR_SET, var_name, value );
}


/*
=================
trap_Cvar_VariableIntegerValue

const char *var_name: A null-terminated string containing the name of the vmCvar to retrieve a value from.

This function returns the integer value of var_name that is currently stored in the "master copy".
=================
*/
int trap_Cvar_VariableIntegerValue( const char *var_name ) {
	return syscall( G_CVAR_VARIABLE_INTEGER_VALUE, var_name );
}


/*
=================
trap_Cvar_VariableStringBuffer

var_name: name of cvar (console variable).
buffer: destination string buffer.
bufsize: maximum limit of string buffer.

This trap copies the content of a cvar named by var_name into a mod-space string.
This is useful for obtaining cvar contents that are not bound, or cannot be bound, to a vmCvar_t variable in the mod code.

The terminating NULL is attached to the end of the copied string by this trap, guaranteeing buffer holds a
null-terminated string. The total number of characters copied, including terminating NULL, fits within the
size specified by bufsize, truncating the string if necessary.

A nonexistent cvar provides an empty string. An existing cvar with empty content also provides an empty string.
Thus, this trap is not sufficient to determine if a cvar actually exists or not.

This trap does not create a new cvar; to create a new cvar during run-time, you can use trap_Cvar_Set or trap_SendConsoleCommand.
=================
*/
void trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( G_CVAR_VARIABLE_STRING_BUFFER, var_name, buffer, bufsize );
}


/*
=================
trap_Cvar_LatchedVariableStringBuffer
=================
*/
void trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( G_CVAR_LATCHEDVARIABLESTRINGBUFFER, var_name, buffer, bufsize );
}


/*
=================
trap_LocateGameData

gentity_t *gEnts: Usually level.gentities.
int numGEntities: Usually level.num_entities.
int sizeofGEntity_t: Usually sizeof( gentity_t ).
playerState_t *gameClients: Usually &level.clients[0].ps.
int sizeofGameClient: Usually sizeof( level.clients[0] ).

It would be very unusual for a mod programmer to want to call this - especially with parameters other
than those outlined above. Its purpose it to notify the Q3 binary when new entities are created.
In the Q3 source it is used twice - once on level load and in G_Spawn where it is called whenever an entity is created.
=================
*/
void trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t,
						 playerState_t *clients, int sizeofGClient ) {
	syscall( G_LOCATE_GAME_DATA, gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGClient );
}


/*
=================
trap_DropClient

int clientNum: The client to drop.
const char *reason: The reason for the drop.

This provides a mechanism to unconditionally drop a client from a server via their clientNum ( ent->client->ps.clientNum).
The reason is displayed to the client when they are dropped.
=================
*/
void trap_DropClient( int clientNum, const char *reason, int length ) {
	syscall( G_DROP_CLIENT, clientNum, reason, length );
}


/*
=================
trap_SendServerCommand

int clientNum: The client that the command is being sent from.
const char *text: The command to be sent.

This has the same effect as typing a command in a clients console for evaluation by the server.
This includes the commands evaluated in g_cmds.c. Its main use in the Q3 source is to print information to a specific
client's console via the "print" command. Passing -1 as the clientNum sends the command to every client.
=================
*/
void trap_SendServerCommand( int clientNum, const char *text )
{
	// rain - #433 - commands over 1022 chars will crash the
	// client engine upon receipt, so ignore them

	// jet Pilot - BUGFIX implementation
	// CHRUKER: b001 - Truncating the oversize server command before writing it to the log
	if( strlen( text ) > 1022 )
	{
		G_LogPrintf( "%s: trap_SendServerCommand( %d, ... ) length exceeds 1022.\n", GAMEVERSION, clientNum );
		G_LogPrintf( "%s: text [%.950s]... truncated\n", GAMEVERSION, text );
		return;
	}
	syscall( G_SEND_SERVER_COMMAND, clientNum, text );
}


// this is probably quite expensive for nothing
/*
qboolean CS_UpdateSafe( int num, const char *string ) {
	int dataCount=1;
	int	i, len;
	char *dup;
	char cs[MAX_STRING_CHARS];

	for(i = 0; i < MAX_CONFIGSTRINGS; i++)
	{
		if(i == num)
		{
			dup = va("%s",string);
		}
		else
		{
			memset( &cs, 0, sizeof(cs) );
			trap_GetConfigstring( i, cs, sizeof(cs) );
			dup = va("%s",cs);
		}

		if(!dup[0])
			continue;

		len = strlen(dup);

		//G_Printf("ConfigString %i, has lenght of %i \n", i, len );

		if(( len + 1 + dataCount ) >= (MAX_GAMESTATE_CHARS -1))
		{
			G_Printf("Couldn't set configString %i, MAX_GAMESTATE_CHARS exceeded ( %i + %i ) \n", num, len + 1, dataCount);
			return qfalse;
		}

		dataCount += len + 1;

		level.csLenTotal += (len - level.csLen[i] + 1);
		level.csLen[i] = (len + 1);

	}
	//G_Printf("ConfigString %i set, dataCount %i out of %i \n", num, dataCount, MAX_GAMESTATE_CHARS);
	return qtrue;
}
*/


/*
=================
trap_SetConfigstring

int num: The config string to set.
const char *string: The value to set it to.

The Q3 Engine provides a mechanism for communicating data from the server to the client
via config strings.
This is (presumably) an array of strings within the engine that are indexed by the CS_* values in bg_public.c.
This trap call sets one of these strings to a value.
Config strings should usually be used to communicate complex data that changes relatively rarely.
=================
*/
void trap_SetConfigstring( int num, const char *string ) {
//	if ( CS_UpdateSafe(num, string) )
		syscall( G_SET_CONFIGSTRING, num, string );
}


/*
=================
trap_GetConfigstring

int num: The config string to be read
char *buffer: A pointer to a buffer to fill with the config string value.
int bufferSize: The size of the buffer.

This is a server side callback used to get the value of a config string.
It is only used once in the q3 source, by the G_FindConfigstringIndex function
which in turn is used by the G_ModelIndex and G_SoundIndex functions.
=================
*/
void trap_GetConfigstring( int num, char *buffer, int bufferSize ) {
	syscall( G_GET_CONFIGSTRING, num, buffer, bufferSize );
}


/*
=================
trap_GetUserinfo

int num: clientNum of the userinfo to retrieve.
char *buffer: Pointer to a buffer to fill.
int buffersize: Size of the buffer.

Each client has a "user info" string which is a backslash delimited key/value pair for the various options the user
can set. For example, model, skin and weapon colour. trap_GetUserinfo fills the buffer pointed to by *buffer with the
userinfo of the client referenced by num.
Key values are then retrieved using Info_ValueForKey( buffer, keyname ).
=================
*/
void trap_GetUserinfo( int num, char *buffer, int bufferSize ) {
	syscall( G_GET_USERINFO, num, buffer, bufferSize );
}


/*
=================
trap_SetUserinfo

int num: clientNum of userinfo to change.
const char *buffer: Userinfo string to set.

This does the opposite to trap_GetUserinfo and sets a userinfo string for a user.
However, it is not used much in the Q3 source, where config strings seem to be the
prefered method to transfer userinfo to the client.
=================
*/
void trap_SetUserinfo( int num, const char *buffer ) {
	//if ( CS_UpdateSafe(CS_PLAYERS+num, buffer) )
		syscall( G_SET_USERINFO, num, buffer );
}


/*
=================
trap_GetServerinfo

char *buffer: A buffer to fill with the server info.
int bufferSize: The size of the buffer.

The "server info" string is a backslash delimited list of server variables such as g_gametype and mapname.
Besides the bot code, its only other use in the Q3 source is to print to the value of serverinfo to the logfile.
=================
*/
void trap_GetServerinfo( char *buffer, int bufferSize ) {
	syscall( G_GET_SERVERINFO, buffer, bufferSize );
}


/*
=================
trap_SetBrushModel

gentity_t* ent : The entity to apply the collision map to.

const char* name : The name of the brush model. If the model is inline (built into the map) it will "*0", "*1", "*n".
These names are generated by the map compiler and cannot be changed.
"*0" is always the map itself. If the model is external to the map then it is the filename
of the BSP (another map is technically valid as a stand-alone model though it is not externally lit and all external
surfaces are backfaced culled).

This function does several special things to an entity which should not be done any other way.
# It sets the clipping bounds of the entity to use the collision map contained in the brush model specified.
  Brush models have a complex collision map which is far more detailed than a regular bounding-box (doors, plats, movers etc).
  The mins and maxes of a brush-model entity may differ wildly from reality.
# It sets the entity to have the SOLID_BMODEL type which serves to alert clients that the modelindex used to
  render this entity is not in the regular list, but instead is the index into the cgs.inlineDrawModel list which
  is built on start-up. This list is zero-indexed from *1 and therefore does not contain the map itself.
  Another method must be used to get the client to render the map a second time.
# It sets the "bmodel" member of the entityShared_t structure to 1 (entityShared_t is the "r" member of the gentity_t structure).
  This allows a simple method of checking if an entity uses a brush model.

There is nothing to prevent an entity from using a brush model which is not built into the map (not inline). However this ability is not used anywhere in Q3A or Q3TA. The map compilers have special command-line switches to build BSPs which are externally lit and designed to be stand-alone models. If this is attempted the cgame QVM will require special code (a bit-flag or similar) to allow it to know the difference between a brush-model index that is inline and one that is external.

And thats' the last function in the game list!!

=================
*/
void trap_SetBrushModel( gentity_t *ent, const char *name ) {
	syscall( G_SET_BRUSH_MODEL, ent, name );
}


/*
=================
trap_Trace

trace_t *results: a pointer to a variable of type trace_t where the result of the trace will be stored.
const vec3_t start: a vector representing the starting position of the trace.
const vec3_t mins: a vector representing the mins of the bounding box the trace will have.
const vec3_t maxs: a vector representing the maxs of the bounding box the trace will have.
const vec3_t end: a vector representing the end position of the trace (it's destination).
int passEntityNum: the trace will not collide against the entity with this number (usually set to the entity number of the entity that initiated the trace so it won't collide against itself).
int contentmask: the content mask it should collide against. Use any of the MASK_ or CONTENTS_ constants. You can logical OR several of them together if needed.

This function is used to trace from one position to another in the BSP tree and store the results in a trace_t structure.
The results consist of the following information (where tr is a variable of type trace_t):

tr.allsolid: if this is qtrue then the area (or some part of it) between the mins and maxs is inside a solid (a brush).
tr.contents: the contents mask of the plane that the trace hit.
tr.endpos: position where the trace ended.
tr.entityNum: entity number of the entity that the trace hit (if it hit solid geometry then this will equal ENTITYNUM_WORLD).
tr.fraction: this is the fraction of the vector between start and end where the trace ended. Therefore if this is less than 1.0 the trace hit something.
tr.plane: contains information about the plane that was hit (most notably the surface normal) stored in a cplane_t structure.
tr.startsolid: if this is qtrue then the trace started in a solid (brush).
tr.surfaceFlags: surfaceflags of the surface that was hit (look in surfaceflags.h at the SURF_* constants to see the which ones there are).

If the mins and maxs are NULL then the trace is treated as purely linear from point to point. If mins and maxs are defined however a
bounding box trace is performed whereby rather than a point the entire bounding box is traced so that it is possible to detect the
collision between solid entities. For example, tracing directly downwards onto a floor with mins and maxs set to { -15, -15, -15 } and { 15, 15, 15 }
respectively starting at some point above the floor will result in a trace_t with an endpoint 15 units from the floor.
=================
*/
void trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall(G_TRACE, results, start, mins, maxs, end, passEntityNum, contentmask);
}


/*
=================
trap_TraceNoEnts
=================
*/
void trap_TraceNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall( G_TRACE, results, start, mins, maxs, end, -2, contentmask );
}


/*
=================
trap_TraceCapsule
=================
*/
void trap_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall( G_TRACECAPSULE, results, start, mins, maxs, end, passEntityNum, contentmask );
}


/*
=================
trap_TraceCapsuleNoEnts
=================
*/
void trap_TraceCapsuleNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall( G_TRACECAPSULE, results, start, mins, maxs, end, -2, contentmask );
}


/*
=================
trap_PointContents

const vec3_t point: The point in space to be examined
int passEntityNum: An entityNum to be ignored.

This trap call returns the CONTENTS_* of a specific point in space - it is commonly
used to avoid spawning items in CONTENTS_NODROP areas.
The second parameter specifies an entityNum (ent->s.number)
which is ignored when evaluating the contents for a point.
=================
*/
int trap_PointContents( const vec3_t point, int passEntityNum ) {
	return syscall( G_POINT_CONTENTS, point, passEntityNum );
}


/*
=================
trap_InPVS

const vec3_t p1: Absolute coordinates of point 1
const vec3_t p2:Absolute coordinates of point 2

Returns true if point1 can _potentially_ see point2. Two points can return true, but not be visible to each other
due to detail brushes, curves, etc. A return value of false guarantees that the points can not see each other.
Normally used to test if two entities can potentially see each other, point1 would be the currentOrigin of the first
entitiy and point2 the currentOrigin of the second entity. This function relies on the Potentially Visible Set,
and areaportal data generated during the BSP compiling of a map.

Note: This function might not be cheap if many players are on server
=================
*/
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 ) {
	return syscall( G_IN_PVS, p1, p2 );
}


/*
=================
trap_InPVSIgnorePortals

const vec3_t p1: Absolute coordinates of point 1
const vec3_t p2: Absolute coordinates of point 2

Returns true if point1 can _potentially_ see point2. This function does not check areaportals.
Points hidden by closed doors (and other areaportals) will still return true.
=================
*/
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) {
	return syscall( G_IN_PVS_IGNORE_PORTALS, p1, p2 );
}


/*
=================
trap_AdjustAreaPortalState

gentity_t *ent: Entity to adjust
qboolean open: State to adjust to

Adjusts the state of a portal connecting two areas. Set "open" to true to allow visibility through the portal
and false to block visibility. This is normally used to adjust visibility when opening and closing doors.
When the door is open; "open" is set true and entities are visible through the door,
when the door is closed; "open" is set false and trap_InPVS will not see entities through the door.
See Use_BinaryMover in the game source for an example.
=================
*/
void trap_AdjustAreaPortalState( gentity_t *ent, qboolean open ) {
	syscall( G_ADJUST_AREA_PORTAL_STATE, ent, open );
}


/*
=================
trap_AreasConnected

int area1: First area to test.
int area2: Second area to test.

This functions tests whether or not the areas (presumably leaf nodes of the bsp tree) are adjacent physically.
FIXME: the values of area1 and area2 are unknown, however.
=================
*/
qboolean trap_AreasConnected( int area1, int area2 ) {
	return syscall( G_AREAS_CONNECTED, area1, area2 );
}


/*
=================
trap_LinkEntity

gentity_t *ent: Entity to be linked.

When an entity is "linked" it is subject to all evaluations that the world performs on it.
For example, all linked entities are accounted for when performing a trace.
Newly created entities are not linked into the world - they must be explcitly linked using this call.
Linking an entity that is already linked into the world has no effect unless the following properties have changed:

 * Its origin or angles have changed which might cause a change in the entity's PVS (ent->r.currentOrigin is used to calculate the entity's PVS cluster)
 * Its bounds have changed (again, is used in PVS cluster calculations, and to set abs(min/max))
 * Its contents have changed (will set ent->s.solid)
 * Its brush model has changed (again, sets ent->s.solid)
=================
*/
void trap_LinkEntity( gentity_t *ent ) {
	syscall( G_LINKENTITY, ent );
}


/*
=================
trap_UnlinkEntity

gentity_t *ent: Entity to be unlinked.

When an entity is "unlinked" from the world it remains in memory, but ceases to exist from the point of view of the world.
This is useful for temporarily deleting an entity or when performing major alterations to an entity's fields.
Unlinking an entity that is already unlinked has no effect.
=================
*/
void trap_UnlinkEntity( gentity_t *ent ) {
	syscall( G_UNLINKENTITY, ent );
}


/*
=================
trap_EntitiesInBox

const vec3_t mins: Defines the position of the corner of a cuboid.
const vec3_t maxs: Defines the position of the opposite corner of this cuboid.
int *list: A pointer to a list of entityNum's.
int maxcount: The maximum number of entities to return.

This fills an array of ints with the entity numbers (ent->s.number) of all the entities currently
within the cuboid defined by mins and maxs.
This is useful because it runs native and is hence significantly cheaper than doing something similar in the QVM.
=================
*/
int trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *list, int maxcount ) {
	return syscall( G_ENTITIES_IN_BOX, mins, maxs, list, maxcount );
}


/*
=================
trap_EntityContact
=================
*/
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent ) {
	return syscall( G_ENTITY_CONTACT, mins, maxs, ent );
}


/*
=================
trap_EntityContactCapsule

const vec3_t mins: Lower bounds.
const vec3_t maxs: Upper bounds.
const gentity_t *ent: The entity to test.

If the entity pointed to by ent is physically within the bounds specified by mins and maxs then this function
returns qtrue.
=================
*/
qboolean trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t *ent ) {
	return syscall( G_ENTITY_CONTACTCAPSULE, mins, maxs, ent );
}

#ifdef OMNIBOTS
int trap_BotAllocateClient( int clientNum ) {
	return syscall( G_BOT_ALLOCATE_CLIENT, clientNum );
}

int trap_BotGetServerCommand(int clientNum, char *message, int size) {
	return syscall( BOTLIB_GET_CONSOLE_MESSAGE, clientNum, message, size );
}

void trap_BotUserCommand(int clientNum, usercmd_t *ucmd) {
	syscall( BOTLIB_USER_COMMAND, clientNum, ucmd );
}

void trap_EA_Command(int client, char *command) {
	syscall( BOTLIB_EA_COMMAND, client, command );
}
#endif


/*
=================
trap_GetSoundLength
=================
*/
int trap_GetSoundLength(sfxHandle_t sfxHandle) {
	return syscall( G_GET_SOUND_LENGTH, sfxHandle );
}


/*
=================
trap_RegisterSound
=================
*/
sfxHandle_t	trap_RegisterSound( const char *sample, qboolean compressed ) {
	return syscall( G_REGISTERSOUND, sample, compressed );
}

#ifdef DEBUG
//#define FAKELAG
#ifdef FAKELAG
#define	MAX_USERCMD_BACKUP	256
#define	MAX_USERCMD_MASK	(MAX_USERCMD_BACKUP - 1)

static usercmd_t cmds[MAX_CLIENTS][MAX_USERCMD_BACKUP];
static int cmdNumber[MAX_CLIENTS];
#endif // FAKELAG
#endif // DEBUG


/*
=================
trap_GetUsercmd

int clientNum: The clientNum to retreive cmd from.
usercmd_t *cmd: Pointer to a usercmd_t to fill.

This function will retrieve the usercmd_t generated by the client portion of
the Q3 binary and place it in what it pointed to by cmd.
=================
*/
void trap_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	syscall( G_GET_USERCMD, clientNum, cmd );

#ifdef FAKELAG
	{
		char s[MAX_STRING_CHARS];
		int fakeLag;

		trap_Cvar_VariableStringBuffer( "g_fakelag", s, sizeof(s) );
		fakeLag = atoi(s);
		if( fakeLag < 0 )
			fakeLag = 0;

		if( fakeLag ) {
			int i;
			int realcmdtime, thiscmdtime;

			// store our newest usercmd
			cmdNumber[clientNum]++;
			memcpy( &cmds[clientNum][cmdNumber[clientNum] & MAX_USERCMD_MASK], cmd, sizeof(usercmd_t) );

			// find a usercmd that is fakeLag msec behind
			i = cmdNumber[clientNum] & MAX_USERCMD_MASK;
			realcmdtime = cmds[clientNum][i].serverTime;
			i--;
			do {
				thiscmdtime = cmds[clientNum][i & MAX_USERCMD_MASK].serverTime;

				if( realcmdtime - thiscmdtime > fakeLag ) {
					// found the right one
                    cmd = &cmds[clientNum][i & MAX_USERCMD_MASK];
					return;
				}

				i--;
			} while ( (i & MAX_USERCMD_MASK) != (cmdNumber[clientNum] & MAX_USERCMD_MASK) );

			// didn't find a proper one, just use the oldest one we have
			cmd = &cmds[clientNum][(cmdNumber[clientNum] - 1) & MAX_USERCMD_MASK];
			return;
		}
	}
#endif // FAKELAG
}


/*
=================
trap_GetEntityToken

char *buffer: A buffer to fill.
int bufferSize: The size of the buffer.

This call fills the buffer pointed to by buffer with all the key/value pairs of every map entity.
The string returned has a very similar format to that of the tail end of a GTKRadiant .map file before
it is compiled to a bsp. It is unlikely that a user would ever need to call this since it is handled already
in g_spawn.c
*/
qboolean trap_GetEntityToken( char *buffer, int bufferSize ) {
	return syscall( G_GET_ENTITY_TOKEN, buffer, bufferSize );
}


/*
=================
trap_DebugPolygonCreate

int color:
int numPoints:
vec3_t *points:

Creates a solid coloured permanent polygon in the renderer (and world) based on the given array of vertices/points.
it returns a reference handle to the polygon as it appears in the renderlist.
this function should be disabled, but it may still work.

Note:
In Q3 1.31, only a single debug polygon can be displayed at once.
Calling trap_DebugPolygonCreate() to draw a second polygon causes none of them to be drawn on the screen.
Delete the old polygon before trying to draw the new one.
=================
*/
int trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points) {
	return syscall( G_DEBUG_POLYGON_CREATE, color, numPoints, points );
}


/*
=================
trap_DebugPolygonDelete

int id: a value previously returned by trap_DebugPolygonCreate

Removes a polygon that was previously created via trap_DebugPolygonCreate.
=================
*/
void trap_DebugPolygonDelete(int id) {
	syscall( G_DEBUG_POLYGON_DELETE, id );
}


/*
=================
trap_RealTime(

qtime_t *qtime: Point in time - look at q_shared.h qtime_t for details.

Returns current system time and date at server.
=================
*/
int trap_RealTime( qtime_t *qtime ) {
	return syscall( G_REAL_TIME, qtime );
}


/*
=================
trap_SnapVector

float *v:Vector to snap.

This simply removes the fractional part of each axis of v - a form of "snap to grid".
Its purpose is to save network bandwidth by exploiting the fact that delta compression will only transmit
the changes in state of a struct.
A changing vector will change less frequently if only the integral part is taken into account.
=================
*/
void trap_SnapVector( float *v ) {
	syscall( G_SNAPVECTOR, v );
	return;
}


/*
=================
trap_GetTag
=================
*/
qboolean trap_GetTag( int clientNum, int tagFileNumber, char *tagName, orientation_t *or ) {
	return syscall( G_GETTAG, clientNum, tagFileNumber, tagName, or );
}

/*
=================
trap_LoadTag
=================
*/
qboolean trap_LoadTag( const char* filename ) {
	return syscall( G_REGISTERTAG, filename );
}

int trap_PC_LoadSource( const char *filename ) {
	return syscall( BOTLIB_PC_LOAD_SOURCE, filename );
}

int trap_PC_FreeSource( int handle ) {
	return syscall( BOTLIB_PC_FREE_SOURCE, handle );
}

int trap_PC_ReadToken( int handle, pc_token_t *pc_token ) {
	return syscall( BOTLIB_PC_READ_TOKEN, handle, pc_token );
}

int trap_PC_SourceFileAndLine( int handle, char *filename, int *line ) {
	return syscall( BOTLIB_PC_SOURCE_FILE_AND_LINE, handle, filename, line );
}

int trap_PC_UnReadToken( int handle ) {
	return syscall( BOTLIB_PC_UNREAD_TOKEN, handle );
}

/*
=================
trap_PbStat
=================
*/
void trap_PbStat ( int clientNum , char *category , char *values ) {
	syscall ( PB_STAT_REPORT , clientNum , category , values ) ;
}


/*
=================
trap_SendMessage
=================
*/
void trap_SendMessage( int clientNum, char *buf, int buflen ) {
	syscall( G_SENDMESSAGE, clientNum, buf, buflen );
}


/*
=================
trap_MessageStatus
=================
*/
messageStatus_t trap_MessageStatus( int clientNum ) {
	return syscall( G_MESSAGESTATUS, clientNum );
}
