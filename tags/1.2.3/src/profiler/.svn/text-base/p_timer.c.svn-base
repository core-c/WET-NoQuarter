#include "p_timer.h"
#include "p_profiler.h"

char*		profiler_DefaultTimerTag	= "(unset)";

void profiler_TimerInitialise(Timer* timer)
{
	memset(timer, 0, sizeof(Timer));
	timer->status	= TIMER_STOPPED;
	timer->tag		= profiler_DefaultTimerTag;
}

struct timeval profiler_TimerSubtract(struct timeval lhs, struct timeval rhs)
{
	ulong64		result_usec	= CONVERT_TO_ULONG64(lhs) - CONVERT_TO_ULONG64(rhs);
	struct timeval			result		= CONVERT_TO_TIMEVAL(result_usec);
	return result;
}

struct timeval profiler_TimerAdd(struct timeval lhs, struct timeval rhs)
{
	struct timeval			result		= { lhs.tv_sec + rhs.tv_sec, lhs.tv_usec + rhs.tv_usec };
	if ( result.tv_usec > TIMER_USEC )
	{
		++result.tv_sec;
		result.tv_usec -= TIMER_USEC;
	}
	return result;
}

struct timeval profiler_TimerDivide(struct timeval lhs, ulong64 divisor)
{
	if ( divisor == 0 )
	{
		G_Printf("profiler_TimerDivide: attempt to divide by zero!\n");
		return lhs;
	}
	else
	{
		ulong64				lhs_usec	= CONVERT_TO_ULONG64(lhs) / divisor;
		struct timeval		result		= CONVERT_TO_TIMEVAL(lhs_usec);
		return result;
	}
}

qboolean profiler_TimerStart(Timer* timer)
{
	if ( timer->status == TIMER_RUNNING )
	{
		G_Printf("profiler_TimerStart: timer \"%s\" already running!\n", timer->tag);
		return qfalse;
	}
	if ( -1 == gettimeofday(&timer->start_time, NULL) )
	{
		G_Printf("profiler_TimerStart: failed to get start time for \"%s\"!\n", timer->tag);
		return qfalse;
	}
	timer->status = TIMER_RUNNING;
	return qtrue;
}

qboolean profiler_TimerStop(Timer* timer)
{
	struct timeval			elapsed;
	// Stop the timer
	if ( timer->status == TIMER_STOPPED )
	{
		G_Printf("profiler_TimerStop: timer \"%s\" already stopped!\n", timer->tag);
		return qfalse;
	}
	if ( -1 == gettimeofday(&timer->stop_time, NULL) )
	{
		G_Printf("profiler_TimerStop: failed to get stop time for \"%s\"!\n", timer->tag);
		return qfalse;
	}
	timer->status = TIMER_STOPPED;
	// Calculate difference between start and stop
	elapsed = profiler_TimerSubtract(timer->stop_time, timer->start_time);
	timer->elapsed_time = profiler_TimerAdd(timer->elapsed_time, elapsed);
	return qtrue;
}

void profiler_TimerAddElapsed(Timer* lhs, Timer* rhs)
{
	lhs->elapsed_time = profiler_TimerAdd(lhs->elapsed_time, rhs->elapsed_time);
}

ulong64 profiler_TimerAverageElapsed(Timer* lhs, ulong64 count)
{
	struct timeval 		avg_elapsed;
	if ( count == 0 )
	{
		G_Printf("profiler_TimerAverageElapsed: invalid count, must not be zero!\n");
		return CONVERT_TO_ULONG64(lhs->elapsed_time);
	}
	avg_elapsed = profiler_TimerDivide(lhs->elapsed_time, count);
	return CONVERT_TO_ULONG64(avg_elapsed) / count;
}

