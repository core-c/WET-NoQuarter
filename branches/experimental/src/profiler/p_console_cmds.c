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
	{"profiler_filter",			2,		2,		profiler_CmdFilter,			"Filter results. arg1 = [per], arg2=[value]."},
};

#define PROFILE_MAX_ARG_COUNT	7

double 			filter_perc	= 0.0;

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
		argv[arg] = &argArray[arg][0];
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

static char const* profiler_CleanBlockName(char const* name, char* buf, int len)
{
	size_t		name_sz		= strlen(name);
	size_t		off;
	if ( name_sz < len )
		return name;
	off = (name_sz - len) + 3;
	Com_sprintf(buf, len+1, "...%s", &name[off]);
	return buf;
}

qboolean profiler_CmdFilter(int argc, char* argv[])
{
	if ( strcmp(argv[0], "per") == 0 )
	{
		filter_perc = atof(argv[1]);
		G_Printf("profiler_CmdFilter: setting percentage filter to show results greater than %f\n", filter_perc);
	}
	else
		G_Printf("profiler_CmdFilter: error: unrecognised statistic to filter (%s)\n", argv[0]);
	return qtrue;
}

qboolean profiler_CmdStatus(int argc, char* argv[])
{
	int			i;
	double		mod_time;
	
	// Get the game and mod times...
	G_Printf("%s: general statistics:\n\n", PROFILER_NAME);
	{
		double		game_time	= profiler_GetGameTime() / TIMER_MSEC;
		mod_time	= profiler_GetModTime() / TIMER_MSEC;
		double		total_time	= game_time + mod_time;
		double		game_frac	= game_time / total_time;
		double		mod_frac	= mod_time / total_time;
		double		game_perc	= game_frac * 100.0;
		double		mod_perc	= mod_frac * 100.0;
		
		G_Printf("Time in game: %015.6f (%6.2f), time in mod: %015.6f (%6.2f)\n\n",
			game_time, game_perc, mod_time, mod_perc);
	}	
	
	G_Printf("%s: profiled block statistics:\n\n", PROFILER_NAME);
	G_Printf("Blk# | %-30s |  Called  |    Total(ms)    | Percent |     Avg(ms)     |     Min(ms)     |    Max(ms)\n", "Profiled Block");
	G_Printf("---------------------------------------------------------------------------------------------------------------------------------\n");
	
	for ( i = 0; i < MAX_STAT_BLOCKS; ++i )
	{
		char		name_buf[31];
		if ( profiler_ActiveBlock(i) == qfalse )
			continue;
		double		opTime		= (double)profiler_GetProfilingBlockTimeElapsed(i) / (double)TIMER_MSEC;
		double		blockPerc	= (opTime/mod_time) * 100.0;
		long		opCount		= profiler_GetProfilingBlockCount(i);
		double		opAvgTime	= (double)profiler_GetProfilingBlockAverageOperationTime(i) / (double)TIMER_MSEC;
		double		minTime		= profiler_GetProfilingBlockMinTime(i);
		double		maxTime		= profiler_GetProfilingBlockMaxTime(i);
		if ( filter_perc != 0.0 && blockPerc < filter_perc )
			continue;
		G_Printf("%04d | %-30s | %08ld | %015.6f | %07.3f | %015.6f | %015.6f | %015.6f\n",
			i,
			profiler_CleanBlockName(profiler_GetBlockName(i), name_buf, 30),
			opCount,
			opTime,
			blockPerc,
			opAvgTime,
			minTime,
			maxTime);
	}
	return qtrue;
}

qboolean profiler_CmdReset(int argc, char* argv[])
{
	profiler_ResetProfilingData();
	G_Printf("%s: profiling data has been reset!\n", PROFILER_NAME);
	return qtrue;
}
