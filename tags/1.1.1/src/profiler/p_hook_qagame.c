#include "../game/g_local.h"
#include "p_profiler.h"
#include <dlfcn.h>

static void*			qagame_handle	= NULL;
static fn_vmMain		qagame_vmmain	= NULL;

char* profiler_GetOrigQAGameFileName(char* path, int path_sz)
 {
	trap_Cvar_VariableStringBuffer("profiler_qagame", path, path_sz);
	if ( path[0] == 0 )
	{
		G_Printf("%s:error: cvar profiler_qagame is not configured with the qagame shared library location!\n", PROFILER_NAME);
		return NULL;
	}
	return path;
}

#define PATH_SZ 0x1000

fn_vmMain profiler_HooKQAGame(void)
{
	char 				path[PATH_SZ] 		= { 0 };
	fn_dllEntry			qagame_dllentry		= NULL;
	
	// Are we already hooked?
	if ( qagame_vmmain != NULL )
		return qagame_vmmain;
		
	// Open the shared library
	if ( NULL == profiler_GetOrigQAGameFileName(path, PATH_SZ) )
	{
		G_Printf("%s:error: failed to get the original qagame shared library file!\n", PROFILER_NAME);
		return NULL;
	}
	qagame_handle = dlopen(path, RTLD_LAZY);
	if ( qagame_handle == NULL )
	{
		G_Printf("%s:error: failed to hook original qagame: %s\n", PROFILER_NAME, dlerror());
		return NULL;
	}
	
	// Find vmMain
	qagame_vmmain = dlsym(qagame_handle, "vmMain");
	if ( qagame_vmmain == NULL )
	{
		G_Printf("%s:error: failed to find vmMain: %s\n", PROFILER_NAME, dlerror());
		return NULL;
	}
	
	// Set up their syscall ptr...
	qagame_dllentry = dlsym(qagame_handle, "dllEntry");
	if ( qagame_dllentry == NULL )
	{
		G_Printf("%s:error: failed to find dllEntry: %s\n", PROFILER_NAME, dlerror());
		qagame_vmmain = NULL;
		return NULL;
	}
	// Set up their syscall table
	qagame_dllentry(profiler_GetSyscallPtr());
	
	// Return the hooked vmMain
	return qagame_vmmain;
}
