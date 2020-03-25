#ifdef PROFILER

#include "g_local.h"


typedef void(*fn_profilerBlockStartTimer)(int, qboolean);
typedef void(*fn_profilerBlockStopTimer)(int);
typedef void(*fn_profilerBlockName)(int, char const*);

static fn_profilerBlockStartTimer	fnBlockStartTimer	= NULL;
static fn_profilerBlockStopTimer	fnBlockStopTimer	= NULL;
static fn_profilerBlockName			fnBlockName			= NULL;

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif

void dllProfilerTimer(int op, void* fn)
{
	if ( op == 0 )
		fnBlockStartTimer 	= (fn_profilerBlockStartTimer)fn;
	else
		fnBlockStopTimer	= (fn_profilerBlockStopTimer)fn;
}

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif

void dllProfilerName(fn_profilerBlockName fn)
{
	fnBlockName = fn;
}

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif

void BG_ProfilerBlockStartTimer(int block_id, qboolean recursive)
{
	if ( fnBlockStartTimer == NULL )
		return;
	fnBlockStartTimer(block_id, recursive);
}

void BG_ProfilerBlockStopTimer(int block_id)
{
	if ( fnBlockStopTimer == NULL )
		return;
	fnBlockStopTimer(block_id);
}

void BG_ProfilerBlockSetName(int block_id, char const* name)
{
	if ( fnBlockName == NULL )
		return;
	fnBlockName(block_id, name);
}
#endif // PROFILER
