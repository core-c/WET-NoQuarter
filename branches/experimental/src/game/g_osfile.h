#ifndef __G_OSFILE_H_
#define __G_OSFILE_H_

// Allow cross platform access to the CRT stdio in a uniform fashion.
//
// We only need this because some of the shared file code requies servers to be able to access
// files outside of the fs path. Keep all the stdio stuff in one place here to aid in porting.
//
// Lucel

#include "q_shared.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(WIN32)
	#include <windows.h>	
	#include <io.h>
	#define open _open
	#define close _close
	#define read _read
	#define write _write
	#define stat _stat
	// redeye - when S_IRUSR isn't defined, assume the others aren't as well
	//          S_IRUSR & the others are not defined on Win32 but on Mingw 
	#ifndef S_IRUSR 
		#define S_IRUSR _S_IREAD
		#define S_IWUSR _S_IWRITE
		#define S_IFDIR _S_IFDIR
	#endif
#endif // WIN32

#ifndef WIN32
	#include <unistd.h>
	#include <dirent.h>
	#include <stdio.h>

	#define MAX_PATH 1024			// Static buffer size for a file path
#endif  // WIN32

// Fn_IterateDirectory: function pointer used in the G_IterateDirectory function.
// 	- Return qfalse to terminate processing
typedef qboolean(*Fn_IterateDirectory)(char const* filename, char const* fullpath, qboolean directory);

// Functions
extern char* G_BuildFilePath(char const* path, char const* file, char const* ext, char* dest, int destsz);
extern void G_IterateDirectory(char const* path, Fn_IterateDirectory handler);
extern int G_WriteDataToFile(char const* path, char const* buf, int sz);
extern int G_ReadDataFromFile(char const* path, char* buf, int sz);
extern qboolean G_IsFile(char const* path);
extern qboolean G_IsDirectory(char const* path);
extern qboolean G_DeleteFile(char const* path);
extern qboolean G_RenameFile(char const* src, char const* dest);

#endif  // __G_OSFILE_H_
