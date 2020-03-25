#include "p_profiler.h"

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
	int		start_time			= 0;
	int		end_time			= 0;

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
	
	// Handle our console commands?
	if ( command == GAME_CONSOLE_COMMAND )
	{
		if ( qtrue == profiler_ConsoleCommand() )
			return qtrue;
	}
	
	// Time the hooked mod command...
	start_time = trap_Milliseconds();
	vmmain_result = vmmain_hooked(command, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	end_time = trap_Milliseconds();
	
	// Add our command profiling
	profiler_AddvmMainOperationTimes(command, start_time, end_time);
	
	// Return their result
	return vmmain_result;
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