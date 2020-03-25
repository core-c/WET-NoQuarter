#define NO_OPTIMIZATIONS

#include "cg_local.h"

void CG_Shutdown(void);

// yada - log to file
static FILE *crashLog=NULL;

void Crash_Printf(const char *fmt, ...){
	va_list	argptr;
	char		text[1024];

	va_start(argptr,fmt);
	Q_vsnprintf(text,sizeof(text),fmt,argptr);
	va_end(argptr);

	if(crashLog) fputs(text,crashLog);
	trap_Print(text);
}

#if defined __linux__

#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#define __USE_GNU
#include <link.h>
#include <sys/ucontext.h>
#include <features.h>

#if __GLIBC__ == 2 && __GLIBC_MINOR__ == 1
#define GLIBC_21
#endif

extern char *strsignal (int __sig) __THROW;

//use sigaction instead.
__sighandler_t INTHandler (int signal, struct sigcontext ctx);
void CrashHandler(int signal, siginfo_t *siginfo, ucontext_t *ctx);
void (*OldHandler)(int signal);
struct sigaction oldact[NSIG];


int segvloop = 0;

void installcrashhandler() {

	struct sigaction act;

	memset(&act, 0, sizeof(act));
	memset(&oldact, 0, sizeof(oldact));
	act.sa_sigaction = (void *)CrashHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &act, &oldact[SIGSEGV]);
	sigaction(SIGILL, &act, &oldact[SIGILL]);
	sigaction(SIGFPE, &act, &oldact[SIGFPE]);
	sigaction(SIGBUS, &act, &oldact[SIGBUS]);

}

void restorecrashhandler() {
	sigaction(SIGSEGV, &oldact[SIGSEGV], NULL);
}

void installinthandler() {
	// Trap Ctrl-C
	signal(SIGINT, (void *)INTHandler);
}

void linux_siginfo(int signal, siginfo_t *siginfo) {
	Crash_Printf("Signal: %s (%d)\n", strsignal(signal), signal);
	Crash_Printf("Siginfo: %p\n", siginfo);
	if(siginfo) {
		Crash_Printf("Code: %d\n", siginfo->si_code);
		Crash_Printf("Faulting Memory Ref/Instruction: %p\n",siginfo->si_addr);
	}
}

// tjw: i'm disabling etpub_dsnoinfo() because it depends on
//      glibc 2.3.3 only  (it also won't build on earlier glibc 2.3
//      versions).
//      We're only doing glibc 2.1.3 release builds anyway.
void linux_dsoinfo(void) {
	struct link_map *linkmap = NULL;
	ElfW(Ehdr)      * ehdr = (ElfW(Ehdr) *)0x08048000;
	ElfW(Phdr)      * phdr;
	ElfW(Dyn) *dyn;
	struct r_debug *rdebug = NULL;

	phdr = (ElfW(Phdr) *)((char *)ehdr + ehdr->e_phoff);

	while (phdr++<(ElfW(Phdr) *)((char *)phdr + (ehdr->e_phnum * sizeof(ElfW(Phdr)))))
		if (phdr->p_type == PT_DYNAMIC)
			break;

	for (dyn = (ElfW(Dyn) *)phdr->p_vaddr; dyn->d_tag != DT_NULL; dyn++)
		if (dyn->d_tag == DT_DEBUG) {
			rdebug = (void *)dyn->d_un.d_ptr;
			break;
		}

		linkmap = rdebug->r_map;

		//rewind to top item.
		while(linkmap->l_prev)
			linkmap=linkmap->l_prev;

		Crash_Printf("DSO Information:\n");

		while(linkmap) {

			if(linkmap->l_addr) {

				if(strcmp(linkmap->l_name,"")==0)
					Crash_Printf("0x%08x\t(unknown)\n", linkmap->l_addr);
				else
					Crash_Printf("0x%08x\t%s\n", linkmap->l_addr, linkmap->l_name);

			}

			linkmap=linkmap->l_next;

		}
}

void linux_backtrace(ucontext_t *ctx) {

	// See <asm/sigcontext.h>

	// ctx.eip contains the actual value of eip
	// when the signal was generated.

	// ctx.cr2 contains the value of the cr2 register
	// when the signal was generated.

	// the cr2 register on i386 contains the address
	// that caused the page fault if there was one.

	int i;

	char **strings;
	void *array[1024];
	size_t size = (size_t)backtrace(array, 1024);

	//Set the actual calling address for accurate stack traces.
	//If we don't do this stack traces are less accurate.
#ifdef GLIBC_21
	Crash_Printf("Stack frames: %Zd entries\n", size-1);
	array[1] = (void *)ctx->uc_mcontext.gregs[EIP];
#else
	Crash_Printf("Stack frames: %zd entries\n", size-1);
	array[1] = (void *)ctx->uc_mcontext.gregs[REG_EIP];
#endif
	Crash_Printf("Backtrace:\n");

	strings = (char **)backtrace_symbols(array, (int)size);

	//Start at one and climb up.
	//The first entry points back to this function.
	for (i = 1; i < (int)size; i++)
		Crash_Printf("(%i) %s\n", i, strings[i]);

	free(strings);
}

__sighandler_t INTHandler(int signal, struct sigcontext ctx) {
	CG_Printf("------------------------------------------------\n");
	CG_Printf("Ctrl-C is not the proper way to kill the server.\n");
	CG_Printf("------------------------------------------------\n");
	return 0;
}

void CrashHandler(int signal, siginfo_t *siginfo, ucontext_t *ctx) {
	char gamepath[MAX_CVAR_VALUE_STRING];
	char homepath[MAX_CVAR_VALUE_STRING];

	//we are real cautious here.
	restorecrashhandler();

	if(signal == SIGSEGV)
		segvloop++;

	if(segvloop < 2) {
		// yada - try logging to disk too
		trap_Cvar_VariableStringBuffer("fs_homepath", homepath, sizeof(homepath));
		trap_Cvar_VariableStringBuffer("fs_game", gamepath, sizeof(gamepath));
		crashLog=fopen(va("%s/%s/nq_crash.log",homepath,gamepath),"w");

		Crash_Printf("-8<--- Client Crash Information ---->8-\n");
		Crash_Printf("Please forward to No Quarter mod team. \n");
		Crash_Printf("---------------------------------------\n");
		Crash_Printf("Version Linux: %s %s %s\n", NQ_VERSION, __DATE__, GAME_VERSION_DATED);
		Crash_Printf("Map: %s\n",cgs.rawmapname);
		linux_siginfo(signal, siginfo);
		linux_dsoinfo();
		linux_backtrace(ctx);
		CG_Printf("-8<--------------------------------->8-\n\n");
		CG_Printf("Attempting to clean up.\n");
		if(crashLog) fclose(crashLog);

		CG_Shutdown();
		//pass control to the default handler.
		if(signal == SIGSEGV) {
			OldHandler = (void *)oldact[SIGSEGV].sa_sigaction;
			(*OldHandler)(signal);
		} else {
			exit(1);
		}
	} else {
		//end this madness we are looping.
		CG_Error("Recursive segfault. Bailing out.");
		OldHandler = (void *)oldact[SIGSEGV].sa_sigaction;
		(*OldHandler)(signal);
	}

	return;

}

#elif defined WIN32
#define Rectangle LCC_Rectangle
#include <windows.h>
#undef Rectangle

#include <process.h>
#include <imagehlp.h>

HMODULE	imagehlp = NULL;

typedef BOOL (WINAPI *PFNSYMINITIALIZE)(HANDLE, LPSTR, BOOL);
typedef BOOL (WINAPI *PFNSYMCLEANUP)(HANDLE);
typedef PGET_MODULE_BASE_ROUTINE PFNSYMGETMODULEBASE;
typedef BOOL (WINAPI *PFNSTACKWALK)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID, PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef BOOL (WINAPI *PFNSYMGETSYMFROMADDR)(HANDLE, DWORD, LPDWORD, PIMAGEHLP_SYMBOL);
typedef BOOL (WINAPI *PFNSYMENUMERATEMODULES)(HANDLE, PSYM_ENUMMODULES_CALLBACK, PVOID);
typedef PFUNCTION_TABLE_ACCESS_ROUTINE PFNSYMFUNCTIONTABLEACCESS;

PFNSYMINITIALIZE pfnSymInitialize = NULL;
PFNSYMCLEANUP pfnSymCleanup = NULL;
PFNSYMGETMODULEBASE pfnSymGetModuleBase = NULL;
PFNSTACKWALK pfnStackWalk = NULL;
PFNSYMGETSYMFROMADDR pfnSymGetSymFromAddr = NULL;
PFNSYMENUMERATEMODULES pfnSymEnumerateModules = NULL;
PFNSYMFUNCTIONTABLEACCESS pfnSymFunctionTableAccess = NULL;

/*
Visual C 7 Users, place the PDB file generated with the build into your etpub
directory otherwise your stack traces will be useless.

Visual C 6 Users, shouldn't need the PDB file since the DLL will contain COFF symbols.

Watch this space for mingw.
*/

BOOL CALLBACK EnumModules( LPSTR ModuleName, DWORD BaseOfDll, PVOID UserContext ) {
	Crash_Printf("0x%08x\t%s\n", BaseOfDll, ModuleName);
	return TRUE;
}

char *ExceptionName(DWORD exceptioncode){
	switch (exceptioncode){
			case EXCEPTION_ACCESS_VIOLATION: return "Access violation"; break;
			case EXCEPTION_DATATYPE_MISALIGNMENT: return "Datatype misalignment"; break;
			case EXCEPTION_BREAKPOINT: return "Breakpoint"; break;
			case EXCEPTION_SINGLE_STEP: return "Single step"; break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "Array bounds exceeded"; break;
			case EXCEPTION_FLT_DENORMAL_OPERAND: return "Float denormal operand"; break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "Float divide by zero"; break;
			case EXCEPTION_FLT_INEXACT_RESULT: return "Float inexact result"; break;
			case EXCEPTION_FLT_INVALID_OPERATION: return "Float invalid operation"; break;
			case EXCEPTION_FLT_OVERFLOW: return "Float overflow"; break;
			case EXCEPTION_FLT_STACK_CHECK: return "Float stack check"; break;
			case EXCEPTION_FLT_UNDERFLOW: return "Float underflow"; break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO: return "Integer divide by zero"; break;
			case EXCEPTION_INT_OVERFLOW: return "Integer overflow"; break;
			case EXCEPTION_PRIV_INSTRUCTION: return "Privileged instruction"; break;
			case EXCEPTION_IN_PAGE_ERROR: return "In page error"; break;
			case EXCEPTION_ILLEGAL_INSTRUCTION: return "Illegal instruction"; break;

				// yada - LCC thinks this is the same as EXCEPTION_IN_PAGE_ERROR
#ifndef __LCC__
			case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception"; break;
#endif

			case EXCEPTION_STACK_OVERFLOW: return "Stack overflow"; break;
			case EXCEPTION_INVALID_DISPOSITION: return "Invalid disposition"; break;
			case EXCEPTION_GUARD_PAGE: return "Guard page"; break;
			case EXCEPTION_INVALID_HANDLE: return "Invalid handle"; break;
			default: break;
	}
	return "Unknown exception";
}

void win32_exceptioninfo(LPEXCEPTION_POINTERS e) {
	Crash_Printf("Exception: %s (0x%08x)\n", ExceptionName(e->ExceptionRecord->ExceptionCode), e->ExceptionRecord->ExceptionCode);
	Crash_Printf("Exception Address: 0x%08x\n", e->ExceptionRecord->ExceptionAddress);
}

void win32_dllinfo(void) {
	Crash_Printf("DLL Information:\n");
	pfnSymEnumerateModules( (HANDLE)GetCurrentProcess(), (PSYM_ENUMMODULES_CALLBACK)EnumModules, NULL);
}

#if 1
// yada - hey at least it produces output oO

void win32_backtrace(LPEXCEPTION_POINTERS e){
	PIMAGEHLP_SYMBOL	pSym;
	int								cnt=0;
	DWORD							lastBP,lastRET,modBase,disp;
	char 							modname[MAX_PATH]="";
	HANDLE 						process;

	pSym=(PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 16384);
	process=GetCurrentProcess();

	//_asm("int $3");
	lastBP=e->ContextRecord->Ebp;
	lastRET=e->ContextRecord->Eip;

	while(1){
		if(IsBadReadPtr((void*)lastRET,1)){
			break;
		}

		modBase=pfnSymGetModuleBase(process,lastRET);
		if(modBase){
			GetModuleFileName((HINSTANCE)modBase,modname,MAX_PATH);
		}else{
			wsprintf(modname,"Unknown");
		}

		pSym->SizeOfStruct=sizeof(IMAGEHLP_SYMBOL);
		pSym->MaxNameLength=MAX_PATH;
		if(pfnSymGetSymFromAddr(process,lastRET,&disp,pSym)){
			Crash_Printf("(%d) %s(%s+%#0x) [0x%08x]\n",cnt,modname,pSym->Name,disp,lastRET);
		}else{
			Crash_Printf("(%d) %s [0x%08x]\n",cnt,modname,lastRET);
		}

		if(IsBadReadPtr((void*)lastBP,8)){
			break;
		}

		cnt++;
		if(cnt>20){
			break;
		}

		/*_asm("push %eax");
		_asm("push %ebx");
		_asm("movl %lastBP,%eax");
		_asm("movl (%eax),%ebx");
		_asm("movl %ebx,%lastBP");
		_asm("movl 4(%eax),%ebx");
		_asm("movl %ebx,%lastRET");
		_asm("popl %ebx");
		_asm("popl %eax");*/

		lastRET=*((DWORD*)(lastBP+4));
		lastBP=*((DWORD*)lastBP);
	}
	GlobalFree(pSym);
}

#else
// yada - original code...
// from my experience it doesnt do shit whatever you try
// maybe it works with another compiler i have NO clue

void win32_backtrace(LPEXCEPTION_POINTERS e) {
	PIMAGEHLP_SYMBOL pSym;
	STACKFRAME sf;
	HANDLE process, thread;
	DWORD dwModBase, Disp;
	BOOL more = FALSE;
	int cnt = 0;
	char modname[MAX_PATH] = "";

	pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 16384);

	ZeroMemory(&sf, sizeof(sf));
	sf.AddrPC.Offset = e->ContextRecord->Eip;
	sf.AddrStack.Offset = e->ContextRecord->Esp;
	sf.AddrFrame.Offset = e->ContextRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	process = GetCurrentProcess();
	thread = GetCurrentThread();

	Crash_Printf("Backtrace:\n");

	while(1) {

		more = pfnStackWalk(
			IMAGE_FILE_MACHINE_I386,
			process,
			thread,
			&sf,
			e->ContextRecord,
			ReadProcessMemory,
			pfnSymFunctionTableAccess,
			pfnSymGetModuleBase,
			NULL
			);

		if(!more || sf.AddrFrame.Offset == 0) {
			break;
		}

		dwModBase = pfnSymGetModuleBase(process, sf.AddrPC.Offset);

		if(dwModBase) {
			GetModuleFileName((HINSTANCE)dwModBase, modname, MAX_PATH);
		} else {
			wsprintf(modname, "Unknown");
		}

		pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		pSym->MaxNameLength = MAX_PATH;

		if(pfnSymGetSymFromAddr(process, sf.AddrPC.Offset, &Disp, pSym))
			Crash_Printf("(%d) %s(%s+%#0x) [0x%08x]\n", cnt, modname, pSym->Name, Disp, sf.AddrPC.Offset);
		else
			Crash_Printf("(%d) %s [0x%08x]\n", cnt, modname, sf.AddrPC.Offset);

		cnt++;
	}

	GlobalFree(pSym);
}
#endif

LONG CALLBACK win32_exception_handler(LPEXCEPTION_POINTERS e) {
	char basepath[MAX_PATH];
	char gamepath[MAX_PATH];
	char homepath[MAX_PATH];

	//search path for symbols...
	trap_Cvar_VariableStringBuffer("fs_basepath", basepath, sizeof(basepath));
	trap_Cvar_VariableStringBuffer("fs_homepath", homepath, sizeof(homepath));
	trap_Cvar_VariableStringBuffer("fs_game", gamepath, sizeof(gamepath));

	// yada - try logging to disk too
	crashLog=fopen(va("%s\\%s\\nq_crash.log",homepath,gamepath),"w");
	pfnSymInitialize(GetCurrentProcess(), va("%s\\%s", basepath, gamepath), TRUE);
	Crash_Printf("-8<--- Client Crash Information ---->8-\n");
	Crash_Printf("Please forward to No Quarter mod team. \n");
	Crash_Printf("---------------------------------------\n");
	Crash_Printf("Version Windows: %s %s %s\n", NQ_VERSION, __DATE__, GAME_VERSION_DATED);
	Crash_Printf("Map: %s\n",cgs.rawmapname);
	win32_exceptioninfo(e);
	win32_dllinfo();
	win32_backtrace(e);
	if(crashLog) fclose(crashLog);

	CG_Printf("-8<--------------------------------->8-\n\n");
	CG_Printf("Attempting to clean up.\n");
	CG_Shutdown();
	pfnSymCleanup(GetCurrentProcess());
	//MessageBox(NULL,"Nothing","NULL",0);
	return 1;
}

void win32_initialize_handler(void) {

	imagehlp = LoadLibrary("IMAGEHLP.DLL");
	if(!imagehlp) {
		CG_Printf("imagehlp.dll unavailable\n");
		return;
	}

	pfnSymInitialize = (PFNSYMINITIALIZE) GetProcAddress(imagehlp, "SymInitialize");
	pfnSymCleanup = (PFNSYMCLEANUP) GetProcAddress(imagehlp, "SymCleanup");
	pfnSymGetModuleBase = (PFNSYMGETMODULEBASE) GetProcAddress(imagehlp, "SymGetModuleBase");
	pfnStackWalk = (PFNSTACKWALK) GetProcAddress(imagehlp, "StackWalk");
	pfnSymGetSymFromAddr = (PFNSYMGETSYMFROMADDR) GetProcAddress(imagehlp, "SymGetSymFromAddr");
	pfnSymEnumerateModules = (PFNSYMENUMERATEMODULES) GetProcAddress(imagehlp, "SymEnumerateModules");
	pfnSymFunctionTableAccess = (PFNSYMFUNCTIONTABLEACCESS) GetProcAddress(imagehlp, "SymFunctionTableAccess");

	if(
		!pfnSymInitialize ||
		!pfnSymCleanup ||
		!pfnSymGetModuleBase ||
		!pfnStackWalk ||
		!pfnSymGetSymFromAddr ||
		!pfnSymEnumerateModules ||
		!pfnSymFunctionTableAccess
		) {
			FreeLibrary(imagehlp);
			CG_Printf("imagehlp.dll missing exports.\n");
			return;
	}

	// Install exception handler
	SetUnhandledExceptionFilter(win32_exception_handler);
}

void win32_deinitialize_handler(void) {
	// Deinstall exception handler
	SetUnhandledExceptionFilter(NULL);
	pfnSymInitialize = NULL;
	pfnSymCleanup = NULL;
	pfnSymGetModuleBase = NULL;
	pfnStackWalk = NULL;
	pfnSymGetSymFromAddr = NULL;
	pfnSymEnumerateModules = NULL;
	pfnSymFunctionTableAccess = NULL;
	FreeLibrary(imagehlp);
}

#else //other platforms
//FIXME: Get rich, buy a mac, figure out how to do this on OSX.
#endif

void EnableCoreDumps() {
#if defined __linux__

#elif defined WIN32

#else

#endif
}

void DisableCoreDump() {
#if defined __linux__

#elif defined WIN32

#else

#endif
}

void EnableStackTrace() {
#if defined __linux__
	installcrashhandler();
#elif defined WIN32
	win32_initialize_handler();
#else

#endif
}

void DisableStackTrace() {
#if defined __linux__
	restorecrashhandler();
#elif defined WIN32
	win32_deinitialize_handler();
#else

#endif
}
