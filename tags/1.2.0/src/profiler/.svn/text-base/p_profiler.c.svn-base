#include "p_profiler.h"

// Profiler data tables
struct BlockProfiler
{
	Timer							times;
	long							count;
	double							max;
	double							min;
	qboolean						active;
	int								recurse_count;
	char							name[BLOCK_PROFILER_NAME_SZ];
	unsigned long					error_count;
};
static struct BlockProfiler			stats[MAX_STAT_BLOCKS];
static struct BlockProfiler			game_timer;
static struct BlockProfiler			mod_timer;

static char*						profiler_GameTimerName	= "[Game Timer]";
static char*						profiler_ModTimerName	= "[Mod Timer]";

static void profiler_ResetBlockProfilerArray(struct BlockProfiler* array, int count)
{
	memset(array, 0, sizeof(struct BlockProfiler) * count);
	for ( --count; count >= 0; --count )
		profiler_TimerInitialise(&array[count].times);
}

static void profiler_SetBlockProfilerName(struct BlockProfiler* block, const char* name)
{
	if ( strlen(name) >= BLOCK_PROFILER_NAME_SZ )
	{
		G_Printf("profiler_SetBlockProfilerName: invalid size for name, max %d characters!\n", BLOCK_PROFILER_NAME_SZ);
		return;
	}
	strncpy(block->name, name, BLOCK_PROFILER_NAME_SZ);
	block->times.tag = block->name;
}

static void profiler_Error(struct BlockProfiler* block, const char* error_msg)
{
	if ( block->error_count++ == 0 )
		G_Printf(error_msg);
}

static void profiler_StartBlockProfiler(struct BlockProfiler* block, qboolean recursive)
{
	// Skip blocks that haven't been named....
	if ( !strcmp(block->times.tag, profiler_DefaultTimerTag) )
		return;
	// Check for recursive calling
	if ( !recursive || block->recurse_count == 0 )
	{
		if ( block->times.status == TIMER_RUNNING )
		{
			profiler_Error(block, va("profiler_BlockStartTimer:%s: cannot start profiling a block that is already started!\n", block->name));
			return;
		}
		if ( qfalse == profiler_TimerStart(&block->times) )
			G_Printf("profiler_BlockStartTimer: failed to start timer for %s)\n", block->name);
	}
	++block->recurse_count;
}

static void profiler_StopBlockProfiler(struct BlockProfiler* block)
{
	struct timeval 		elapsed_tv;
	double				elapsed;
	// Skip blocks that haven't been named....
	if ( !strcmp(block->times.tag, profiler_DefaultTimerTag) )
		return;
	// If we're recursing.... only stop the timer at the end!
	if ( --block->recurse_count != 0 )
		return;
	if ( block->times.status == TIMER_STOPPED )
	{
		profiler_Error(block, va("profiler_BlockStopTimer:%s: cannot stop profiling a block that is already stoped!\n", block->name));
		return;
	}
	if ( qfalse == profiler_TimerStop(&block->times) )
		G_Printf("profiler_BlockStopTimer: failed to stop timer for %s\n", block->name);
	// Find out the elapsed time for the last start/stop
	elapsed_tv 	= profiler_TimerSubtract(block->times.stop_time, block->times.start_time);
	elapsed		= (double)CONVERT_TO_ULONG64(elapsed_tv) / (double)TIMER_MSEC;
	// Max?
	if ( block->max == 0 || block->max < elapsed )
		block->max = elapsed;
	// Min?
	if ( block->min == 0 || block->min > elapsed )
		block->min = elapsed;
	// Increment counter		
	++block->count;	
	// Set the block to active
	block->active = qtrue;	
}

// Reset all profiling data
void profiler_ResetProfilingData(void)
{
	int 	i;
	// Initialise mod and game timers
	profiler_ResetBlockProfilerArray(&game_timer, 	1);
	profiler_ResetBlockProfilerArray(&mod_timer, 	1);
	// Initialise the block profiling arrays
	profiler_ResetBlockProfilerArray(stats, MAX_STAT_BLOCKS);
	// Set the name for the vmmain command strings
	for ( i = 0; i < VMMAIN_COMMANDS; ++i )
		profiler_SetBlockName(i, profiler_GetCommandName(i));
	// Set game/mod timer names
	profiler_SetBlockProfilerName(&game_timer, 	profiler_GameTimerName);
	profiler_SetBlockProfilerName(&mod_timer, 	profiler_ModTimerName);
}

void profiler_SetBlockName(int block_id, char const* name)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_SetBlockName: invalid block id\n");
		return;
	}
	profiler_SetBlockProfilerName(&stats[block_id], name);
}

char const* profiler_GetBlockName(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetBlockName: invalid block id\n");
		return "(invalid block id)";
	}
	return stats[block_id].name;
}

// Profile total time spent in/out of the mod....
void profiler_EnterMod()
{
	if ( game_timer.times.status == TIMER_RUNNING )
		profiler_StopBlockProfiler(&game_timer);
	profiler_StartBlockProfiler(&mod_timer, qtrue);	
}

void profiler_LeaveMod()
{
	profiler_StopBlockProfiler(&mod_timer);
	profiler_StartBlockProfiler(&game_timer, qfalse);	
}

ulong64 profiler_GetGameTime()
{
	return CONVERT_TO_ULONG64(game_timer.times.elapsed_time);
}

ulong64 profiler_GetModTime()
{
	profiler_StopBlockProfiler(&mod_timer);
	ulong64		mod_time	= CONVERT_TO_ULONG64(mod_timer.times.elapsed_time);
	profiler_StartBlockProfiler(&mod_timer, qtrue);	
	return mod_time;
}

void profiler_BlockStartTimer(int block_id, qboolean recursive)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_BlockStartTimer: invalid block id\n");
		return;
	}
	profiler_StartBlockProfiler(&stats[block_id], recursive);
}

void profiler_BlockStopTimer(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_BlockStopTimer: invalid block id\n");
		return;
	}
	profiler_StopBlockProfiler(&stats[block_id]);
}

void profiler_UserBlockStartTimer(int user_block_id, qboolean recursive)
{
	int block_id = user_block_id + VMMAIN_COMMANDS;
	// Translate user id to our internal block id
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_UserBlockStartTimer: invalid block id\n");
		return;
	}
	profiler_StartBlockProfiler(&stats[block_id], recursive);
}

void profiler_UserBlockStopTimer(int user_block_id)
{
	int block_id = user_block_id + VMMAIN_COMMANDS;
	// Translate user id to our internal block id
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_UserBlockStopTimer: invalid block id\n");
		return;
	}
	profiler_StopBlockProfiler(&stats[block_id]);
}

void profiler_UserSetBlockName(int user_block_id, char const* name)
{
	int block_id = user_block_id + VMMAIN_COMMANDS;
	// Translate user id to our internal block id
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_UserSetBlockName: invalid block id\n");
		return;
	}
	profiler_SetBlockProfilerName(&stats[block_id], name);
}

qboolean profiler_ActiveBlock(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_ActiveBlock: invalid block id\n");
		return qfalse;
	}
	return stats[block_id].active;	
}

double profiler_GetProfilingBlockMaxTime(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetProfilingBlockMaxTime: invalid block id\n");
		return 0;
	}
	return stats[block_id].max;
}

double profiler_GetProfilingBlockMinTime(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetProfilingBlockMinTime: invalid block id\n");
		return 0;
	}
	return stats[block_id].min;
}

ulong64 profiler_GetProfilingBlockTimeElapsed(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetProfilingBlockTimeElapsed: invalid block id\n");
		return 0;
	}
	return CONVERT_TO_ULONG64(stats[block_id].times.elapsed_time);
}

long profiler_GetProfilingBlockCount(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetProfilingBlockCount: invalid block id\n");
		return 0;
	}
	return stats[block_id].count;
}

ulong64 profiler_GetProfilingBlockAverageOperationTime(int block_id)
{
	if ( block_id >= MAX_STAT_BLOCKS)
	{
		G_Printf("profiler_GetProfilingBlockAverageOperationTime: invalid block id\n");
		return 0;
	}
	ulong64		opTime		= profiler_GetProfilingBlockTimeElapsed(block_id);
	long		opCount		= profiler_GetProfilingBlockCount(block_id);
	return (opCount > 0 ? opTime/opCount : 0);
}