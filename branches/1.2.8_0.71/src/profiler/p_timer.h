#ifndef NQ_PROFILER_P_TIMER_H
#define NQ_PROFILER_P_TIMER_H

#include <sys/time.h>
#include "../game/g_local.h"

#define TIMER_USEC			1000000
#define TIMER_MSEC			1000
typedef unsigned long long	ulong64;

#define CONVERT_TO_ULONG64(X) ((X.tv_sec * TIMER_USEC) + X.tv_usec)
#define CONVERT_TO_TIMEVAL(X) { X / TIMER_USEC, X % TIMER_USEC }

typedef enum
{
	TIMER_STOPPED,
	TIMER_RUNNING,
} TimerMode;

typedef struct
{
	struct timeval 			elapsed_time;
	struct timeval			start_time;
	struct timeval			stop_time;
	TimerMode				status;
	char*					tag;
} Timer;

extern char*				profiler_DefaultTimerTag;

extern void 				profiler_TimerInitialise(Timer* timer);
extern struct timeval 		profiler_TimerSubtract(struct timeval lhs, struct timeval rhs);
extern struct timeval 		profiler_TimerAdd(struct timeval lhs, struct timeval rhs);
extern struct timeval 		profiler_TimerDivide(struct timeval lhs, ulong64 divisor);
extern qboolean				profiler_TimerStart(Timer* timer);
extern qboolean				profiler_TimerStop(Timer* timer);
extern void 				profiler_TimerAddElapsed(Timer* lhs, Timer* rhs);
extern ulong64			 	profiler_TimerAverageElapsed(Timer* lhs, ulong64 count);

#endif  // NQ_PROFILER_P_TIMER_H
