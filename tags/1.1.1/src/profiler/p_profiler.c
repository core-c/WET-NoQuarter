#include "p_profiler.h"

// Profiler data tables
static long					vmmain_command_times[VMMAIN_COMMANDS];
static long					vmmain_command_count[VMMAIN_COMMANDS];

// Reset all profiling data
void profiler_ResetProfilingData(void)
{
	memset(vmmain_command_times, 0, sizeof(vmmain_command_times));
	memset(vmmain_command_count, 0, sizeof(vmmain_command_count));	
}

// Add profiling data for each top level vmMain command
void profiler_AddvmMainOperationTimes(int command, long start, long end)
{
	long	duration = end - start;
	vmmain_command_times[command] += duration;
	vmmain_command_count[command] += 1;	
}

long profiler_GetvmMainOperationTime(int command)
{
	if ( command >= VMMAIN_COMMANDS)
	{
		G_Printf("profiler_GetvmMainOperationTime: invalid command\n");
		return 0;
	}
	return vmmain_command_times[command];
}

long profiler_GetvmMainOperationCount(int command)
{
	if ( command >= VMMAIN_COMMANDS)
	{
		G_Printf("profiler_GetvmMainOperationCount: invalid command\n");
		return 0;
	}
	return vmmain_command_count[command];
}