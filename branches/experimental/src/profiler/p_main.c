#include "p_profiler.h"
#include "p_timer.h"

static int 					profiler_initialised 	= 0;

int profiler_initialise(void)
{
	if ( profiler_initialised == 0 )
	{
		// Only hook the original qagame first time we initialise
		if ( NULL == profiler_HooKQAGame() )
			return 0;
	}
	profiler_initialised = 1;

	// Cleanup the profiling data
	profiler_ResetProfilingData();

	// Success!
	return 1;
}

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif
	int		vmmain_result		= -1;

	// Initialised?
	if ( !profiler_initialised )
	{
		if ( 0 == profiler_initialise() )
		{
			G_Printf("%s:v%s: failed to initialise the profiler!\n", PROFILER_NAME, PROFILER_VERSION);
			trap_SendConsoleCommand(EXEC_APPEND, "quit");
		}
		else
		{
			G_Printf("%s:v%s: initialised ok!\n", PROFILER_NAME, PROFILER_VERSION);
			G_Printf("%s coded by %s\n", PROFILER_NAME, PROFILER_AUTHOR);
		}
	}

	// Get the vmmain from the original qagame
	fn_vmMain vmmain_hooked	= profiler_HooKQAGame();

	if ( vmmain_hooked == NULL )
		return -1;

	// Profile Mod/Game time
	profiler_EnterMod();

	// Handle our console commands?
	if ( command == GAME_CONSOLE_COMMAND )
	{
		if ( qtrue == profiler_ConsoleCommand() )
		{
			profiler_LeaveMod();
			return qtrue;
		}
	}

	// Time the hooked mod command...
	profiler_BlockStartTimer(command, qfalse);
	vmmain_result = vmmain_hooked(command, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	profiler_BlockStopTimer(command);

	// Return their result
	profiler_LeaveMod();
	return vmmain_result;
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	G_Printf ("%s", text);
}
//bani
void QDECL Com_Printf( const char *msg, ... )_attribute((format(printf,1,2)));

void QDECL Com_sprintf( char *dest, int size, const char *fmt, ...) {
	int		ret;
	va_list		argptr;

	va_start (argptr,fmt);
	ret = Q_vsnprintf (dest, size, fmt, argptr);
	va_end (argptr);
	if (ret == -1) {
		Com_Printf ("Com_sprintf: overflow of %i bytes buffer\n", size);
	}
}

void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}
//bani
void QDECL G_Printf( const char *fmt, ... )_attribute((format(printf,1,2)));

int Q_vsnprintf( char *dest, int size, const char *fmt, va_list argptr ) {
	int ret;

#ifdef _WIN32
#undef _vsnprintf
	ret = _vsnprintf( dest, size-1, fmt, argptr );
#define _vsnprintf	use_idStr_vsnPrintf
#else
#undef vsnprintf
	ret = vsnprintf( dest, size, fmt, argptr );
#define vsnprintf	use_idStr_vsnPrintf
#endif
	dest[size-1] = '\0';
	if ( ret < 0 || ret >= size ) {
		return -1;
	}
	return ret;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday

Ridah, modified this into a circular list, to further prevent stepping on
previous strings
============
*/
char	* QDECL va( const char *format, ... ) {
	va_list		argptr;
	#define	MAX_VA_STRING	32000
	static char		temp_buffer[MAX_VA_STRING];
	static char		string[MAX_VA_STRING];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;
	int len;


	va_start (argptr, format);
	vsprintf (temp_buffer, format,argptr);
	va_end (argptr);

	if ((len = strlen(temp_buffer)) >= MAX_VA_STRING) {
		G_Printf("Attempted to overrun string in call to va()\n" );
	}

	if (len + index >= MAX_VA_STRING-1) {
		index = 0;
	}

	buf = &string[index];
	memcpy( buf, temp_buffer, len+1 );

	index += len + 1;

	return buf;
}

// Initialised?
int profiler_IsInitialised(void)
{
	return profiler_initialised;
}

char* commandNames[] =
{
	"GAME_INIT",
	"GAME_SHUTDOWN",
	"GAME_CLIENT_CONNECT",
	"GAME_CLIENT_BEGIN",
	"GAME_CLIENT_USERINFO_CHANGED",
	"GAME_CLIENT_DISCONNECT",
	"GAME_CLIENT_COMMAND",
	"GAME_CLIENT_THINK",
	"GAME_RUN_FRAME",
	"GAME_CONSOLE_COMMAND",
	"GAME_SNAPSHOT_CALLBACK",
	"BOTAI_START_FRAME",
	"BOT_VISIBLEFROMPOS",
	"BOT_CHECKATTACKATPOS",
	"GAME_MESSAGERECEIVED",
};

char* profiler_GetCommandName(int command)
{
	if ( command >= VMMAIN_COMMANDS )
	{
		G_Printf("profiler_GetCommandName: invalid command value!\n");
		return "(invalid)";
	}
	return commandNames[command];
}
