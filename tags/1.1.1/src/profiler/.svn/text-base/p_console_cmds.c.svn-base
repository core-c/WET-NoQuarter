#include "p_profiler.h"

typedef qboolean(*fnCommandHandler)(int, char**);

typedef struct
{
	char*						name;				// Command name as it would be entered in the console
	int							minArgs;			// -1 forgoes check
	int							maxArgs;			// -1 forgoes check
	fnCommandHandler			delegate;			// Function handler
	char*						description;		// Description
} CommandDescriptor;

static const CommandDescriptor		cmdTable[] =
{
	{"profiler_help",			0,		1,		profiler_CmdHelp,			"Displays the builtin profiler help system."},
	{"profiler_status",			0,		0,		profiler_CmdStatus,			"Displays a snapshot of the current rounds profiling data."},
	{"profiler_reset",			0,		0,		profiler_CmdReset,			"Reset all profiling data."},
};

#define PROFILE_MAX_ARG_COUNT	7

CommandDescriptor const* profiler_FindDescriptor(char const* cmd)
{
	int 	i;
	for ( i = 0; i < (sizeof(cmdTable)/sizeof(CommandDescriptor)); ++i )
	{
		CommandDescriptor const*	dsc	= &cmdTable[i];	
		if ( strcmp(cmd, dsc->name) == 0 )
			return dsc;
	}
	return NULL;
}

qboolean profiler_ConsoleCommand(void)
{
	static char 				argArray[MAX_TOKEN_CHARS][PROFILE_MAX_ARG_COUNT];
	char*						argv[PROFILE_MAX_ARG_COUNT];
	int							argc;
	char						cmd[MAX_TOKEN_CHARS];
	CommandDescriptor const*	dsc;
	int							arg;
	
	// Get the command
	trap_Argv(0, cmd, sizeof(cmd));
	
	// Ok search a descriptor for this command in our command table
	dsc	= profiler_FindDescriptor(cmd);
	
	// Not found?
	if ( !dsc )
		return qfalse;
		
	// Get the argument count
	argc = trap_Argc() - 1;
	
	// Check minimum argument count
	if ( dsc->minArgs > -1 && argc < dsc->minArgs )
	{
		G_Printf("error: %s: invalid argument count, you need a minimum of %d arguments!\n",
			dsc->name, dsc->minArgs);
		return qtrue;
	}
	
	// Check maximum argument count
	if ( dsc->maxArgs > -1 && argc > dsc->maxArgs )
	{
		G_Printf("error: %s: invalid argument count, you need a maximum of %d arguments!\n",
			dsc->name, dsc->maxArgs);
		return qtrue;				
	}
	
	// Fetch all the arguments and construct the argument array
	memset(argArray, 0, sizeof(argArray));
	memset(argv, 0, sizeof(argv));
	for ( arg = 0; arg < argc; ++arg )
	{
		argv[arg] = &argArray[0][arg];
		trap_Argv(arg + 1, argv[arg], MAX_TOKEN_CHARS);
	}
	
	// Call the handler - return it's response
	return dsc->delegate(argc, argv);
}

qboolean profiler_CmdHelp(int argc, char* argv[])
{
	G_Printf("%s: help:\n\n", PROFILER_NAME);
	if ( argc )
	{
		CommandDescriptor const*	dsc	= profiler_FindDescriptor(argv[0]);
		if ( !dsc )
			G_Printf("error: command \"%s\" is not a profiler command!\n", argv[0]);
		else
			G_Printf("%s:\n\t%s\n", argv[0], dsc->description);
	}
	else
	{
		int		i;
		for ( i = 0; i < (sizeof(cmdTable)/sizeof(CommandDescriptor)); ++i )
		{
			CommandDescriptor const* dsc	= &cmdTable[i];	
			G_Printf("%s:\n\t%s\n", dsc->name, dsc->description);
		}
	}
	return qtrue;
}

qboolean profiler_CmdStatus(int argc, char* argv[])
{
	int		i;
	
	G_Printf("%s: vmMain command statistics:\n\n", PROFILER_NAME);
	G_Printf("%-30s |   Called   |  Total(ms)  |  Avg(ms)\n", "vmMain Cmd");
	G_Printf("----------------------------------------------------------------------\n");
	
	for ( i = 0; i < VMMAIN_COMMANDS; ++i )
	{
		long		opTime		= profiler_GetvmMainOperationTime(i);
		long		opCount		= profiler_GetvmMainOperationCount(i);
		long		opAvgTime	= opCount > 0 ? opTime/opCount : 0;
		G_Printf("%-30s | 0x%08lX |  0x%08lX | 0x%08lX\n",
			profiler_GetCommandName(i),
			opCount,
			opTime,
			opAvgTime);
	}
	return qtrue;
}

qboolean profiler_CmdReset(int argc, char* argv[])
{
	profiler_ResetProfilingData();
	G_Printf("%s: profiling data has been reset!\n", PROFILER_NAME);
	return qtrue;
}