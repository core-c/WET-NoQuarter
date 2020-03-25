#ifndef NQ_PROFILER_P_PROFILER_H
#define NQ_PROFILER_P_PROFILER_H

#include "../game/g_local.h"

#define PROFILER_NAME			"ET Server Profiler"
#define PROFILER_VERSION		"0.0.1"
#define PROFILER_AUTHOR			"Richard (snl//lucel) Deighton"
#define PROFILER_COLOUR_AUTHOR	"^:Richard (^9snl^7/^2/^7lucel^:) Deighton"
#define VMMAIN_COMMANDS 		(GAME_MESSAGERECEIVED+1)

typedef int(*fn_vmMain)(int, int, int, int, int, int, int, int);
typedef int(QDECL *fn_SyscallPtr)(int,... );
typedef void(*fn_dllEntry)(fn_SyscallPtr);

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
extern void				profiler_AddvmMainOperationTimes(int command, long start, long end);
extern long				profiler_GetvmMainOperationTime(int command);
extern long				profiler_GetvmMainOperationCount(int command);

// p_console_cmds.c
extern qboolean			profiler_CmdHelp(int argc, char* argv[]);
extern qboolean			profiler_CmdStatus(int argc, char* argv[]);
extern qboolean			profiler_CmdReset(int argc, char* argv[]);

#endif  // NQ_PROFILER_P_PROFILER_H