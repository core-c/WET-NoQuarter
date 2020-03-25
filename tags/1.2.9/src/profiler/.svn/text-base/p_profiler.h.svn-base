#ifndef NQ_PROFILER_P_PROFILER_H
#define NQ_PROFILER_P_PROFILER_H

#include "../game/g_local.h"
#include "p_timer.h"

#define PROFILER_NAME			"ET Server Profiler"
#define PROFILER_VERSION		"0.0.1"
#define PROFILER_AUTHOR			"Richard (snl//lucel) Deighton"
#define PROFILER_COLOUR_AUTHOR	"^:Richard (^9snl^7/^2/^7lucel^:) Deighton"
#define VMMAIN_COMMANDS 		(GAME_MESSAGERECEIVED+1)
#define MAX_USER_STAT_BLOCKS	1024
#define BLOCK_PROFILER_NAME_SZ	64
#define MAX_STAT_BLOCKS			(VMMAIN_COMMANDS + MAX_USER_STAT_BLOCKS)

typedef int(*fn_vmMain)(int, int, int, int, int, int, int, int);
typedef int(QDECL *fn_SyscallPtr)(int,... );
typedef void(*fn_dllEntry)(fn_SyscallPtr);
typedef void(*fn_profilerBlockTimer)(int, qboolean);
typedef void(*fn_profilerBlockName)(int, char const*);
typedef void(*fn_dllProfilerTimer)(int, fn_profilerBlockTimer);
typedef void(*fn_dllProfilerName)(fn_profilerBlockName);

// p_main.c
extern char*			profiler_GetCommandName(int command);
extern int 				profiler_IsInitialised(void);

// p_hook_qagame.c
extern fn_vmMain 		profiler_HooKQAGame(void);
extern fn_SyscallPtr	profiler_GetSyscallPtr(void);

// p_console_cmds.c
extern qboolean 		profiler_ConsoleCommand(void);

// p_profiler.c
extern void				profiler_ResetProfilingData(void);
extern void				profiler_SetBlockName(int block_id, char const* name);
extern char const*		profiler_GetBlockName(int block_id);
extern void				profiler_EnterMod();
extern void				profiler_LeaveMod();
extern ulong64			profiler_GetGameTime();
extern ulong64			profiler_GetModTime();
extern void				profiler_BlockStartTimer(int block_id, qboolean recursive);
extern void				profiler_BlockStopTimer(int block_id);
extern void				profiler_UserBlockStartTimer(int user_block_id, qboolean recursive);
extern void				profiler_UserBlockStopTimer(int user_block_id);
extern void				profiler_UserSetBlockName(int user_block_id, char const* name);
extern qboolean			profiler_ActiveBlock(int block_id);
extern double 			profiler_GetProfilingBlockMaxTime(int block_id);
extern double 			profiler_GetProfilingBlockMinTime(int block_id);
extern ulong64 			profiler_GetProfilingBlockTimeElapsed(int block_id);
extern long 			profiler_GetProfilingBlockCount(int block_id);
extern ulong64 			profiler_GetProfilingBlockAverageOperationTime(int block_id);

// p_console_cmds.c
extern qboolean			profiler_CmdHelp(int argc, char* argv[]);
extern qboolean			profiler_CmdStatus(int argc, char* argv[]);
extern qboolean			profiler_CmdReset(int argc, char* argv[]);
extern qboolean 		profiler_CmdFilter(int argc, char* argv[]);

#endif  // NQ_PROFILER_P_PROFILER_H
