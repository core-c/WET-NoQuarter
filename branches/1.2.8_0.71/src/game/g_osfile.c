#include "g_osfile.h"
#include "g_local.h"
#include "q_shared.h"
#include <errno.h>

char* G_BuildFilePath(char const* path, char const* file, char const* ext, char* dest, int destsz)
{
       int                             pathsz                  = strlen(path);
       
       // Wipe the destination string
       dest[0] = 0;
       
       // Add the path
       Q_strcat(dest, destsz, path);           
       if ( pathsz && !(path[pathsz-1] == '\\' || path[pathsz-1] == '/') )
               Q_strcat(dest, destsz, "/");
       
       // Add the file
       Q_strcat(dest, destsz, file);
       
       // Add the extension
       Q_strcat(dest, destsz, ext);
       
       return dest;
}

qboolean G_IsFile(char const* path)
{
	struct stat	sb;
	int			result	= stat(path, &sb);
	if ( -1 == result || sb.st_mode & S_IFDIR )
		return qfalse;

	return qtrue;
}

qboolean G_IsDirectory(char const* path)
{
	struct stat	sb;
	int			result	= stat(path, &sb);
	if ( -1 == result )
		return qfalse;
	if ( sb.st_mode & S_IFDIR )
		return qtrue;
	return qfalse;
}

#ifdef WIN32

void G_IterateDirectory(char const* path, Fn_IterateDirectory handler)
{
	HANDLE			handle;
	WIN32_FIND_DATA	findData;
	char 			buf[MAX_PATH];
	
	// Open the directory
	G_BuildFilePath(path, "*", "", buf, MAX_PATH);
	handle	= FindFirstFile(buf, &findData);
	if ( handle == INVALID_HANDLE_VALUE )
	{
		G_Printf("G_WipeDirectory: failed to open path: %s: %d\n", path, (int) GetLastError());
		return;
	}

	// Iterate over each file in the directory calling the handler for each file in turn...
	do
	{
		qboolean		directory	= qfalse;
		
		// Skip current/previous directory files...
		if ( 0 == Q_strncmp(findData.cFileName, ".", 1) ||
			 0 == Q_strncmp(findData.cFileName , "..", 2) )
			continue;
			
		// Build the path...
		G_BuildFilePath(path, findData.cFileName, "", buf, MAX_PATH);
		
		// Directory?
		if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			directory = qtrue;
			
		// Call the handler with the details
		if ( qfalse == handler(findData.cFileName, buf, directory) )
			// return qfalse to terminate processing
			break;
	} while ( FindNextFile(handle, &findData) );
	
	// Close the directory handle
	FindClose(handle);
}

int G_WriteDataToFile(char const* path, char const* data, int sz)
{
	DWORD			processed;

	// Open the file
	HANDLE			handle	= CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( handle == INVALID_HANDLE_VALUE )
	{
		G_Printf("G_WriteDataToFile: failed to open file: %s: %d\n", path, (int) GetLastError());
		return -1;
	}
	
	// Write the data
	if ( !WriteFile(handle, data, sz, &processed, NULL) || processed != sz )
	{
		G_Printf("G_WriteDataToFile: failed to write data to file: %s: %d\n", path, (int) GetLastError());
		return -1;
	}
	
	// Close the handle
	if ( FALSE == CloseHandle(handle) )
	{
		G_Printf("G_WriteDataToFile: faile to close handle: %s: %d\n", path, (int) GetLastError());
		return -1;
	}
	
	// Success
	return 0;
}

int G_ReadDataFromFile(char const* path, char* data, int sz)
{
	DWORD			processed;

	// Open the file
	HANDLE			handle	= CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( handle == INVALID_HANDLE_VALUE )
	{
		G_Printf("G_ReadDataFromFile: failed to open file: %s: %d\n", path, (int) GetLastError());
		return -1;
	}
	
	// Read the data
	if ( !ReadFile(handle, data, sz, &processed, NULL) || processed != sz )
	{
		G_Printf("G_ReadDataFromFile: failed to read data to file (%d/%d): %s: %d\n", 
			(int) processed, sz, path, (int) GetLastError());
		return -1;
	}
	
	// Close the handle
	if ( FALSE == CloseHandle(handle) )
	{
		G_Printf("G_ReadDataFromFile: faile to close handle: %s: %d\n", path, (int) GetLastError());
		return -1;
	}
	
	// Success
	return 0;
}

qboolean G_DeleteFile(char const* path)
{
	return (DeleteFile(path) ? qtrue : qfalse);
}

qboolean G_RenameFile(char const* src, char const* dest)
{
	return (MoveFile(src, dest) ? qtrue : qfalse);
}

#else

void G_IterateDirectory(char const* path, Fn_IterateDirectory handler)
{
	struct dirent*	dir;
	DIR*			handle;
	
	// Open the directory
	handle = opendir(path);
	if ( NULL == handle )
	{
		G_Printf("G_IterateDirectory: failed to open path: %s: %d\n", path, errno);
		return;
	}
	
	// Iterate over each file in the directory in turn calling the handler as we go...
	while ( (dir = readdir(handle)) )
	{
#ifdef __MACOSX__
		if ( dir->d_namlen > 0 )
		{
#endif // __MACOSX__
			char 		buf[MAX_PATH];
			qboolean	directory		= qfalse;
			
			// Skip current/previous directory files...
			if ( 0 == Q_strncmp(dir->d_name, ".", 1) ||
				 0 == Q_strncmp(dir->d_name, "..", 2) )
				continue;
				
			// Build the path...
			G_BuildFilePath(path, dir->d_name, "", buf, MAX_PATH);
			
			// Directory?
			if ( dir->d_type == DT_DIR )
				directory = qtrue;
				
			// Call the handler with the details
			if ( qfalse == handler(dir->d_name, buf, directory) )
				// return qfalse to terminate processing
				break;
#ifdef __MACOSX__
		}
#endif // __MACOSX__
	}
	
	// Close the directory handle
	closedir(handle);
}

int G_WriteDataToFile(char const* path, char const* data, int sz)
{
	// Open the file
	int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	if ( -1 == fd )
	{
		G_Printf("G_WriteDataToFile: failed to open file: %s: %d\n", path, errno);
		return -1;
	}
		
	// redeye - force group read/write permissions (open sets them only if umask allows it)
#if defined(_BSD_SOURCE) || _XOPEN_SOURCE >= 500
	// working on the existing file descriptor should be a little bit faster
	fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
#else
	chmod(path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
#endif

	// Output the data
	if ( -1 == write(fd, data, sz) )
	{
		G_Printf("G_WriteDataToFile: failed to write file: %s: %d\n", path, errno);
		return -1;
	}
	
	// Close the file
	if ( -1 == close(fd) )
	{
		G_Printf("G_WriteDataToFile: failed to close file: %s: %d\n", path, errno);
		return -1;
	}
	
	// Success
	return 0;
}

int G_ReadDataFromFile(char const* path, char* data, int sz)
{
	int		byte_count;
	
	// Open the file
	int fd = open(path, O_RDONLY, 0);
	if ( -1 == fd )
	{
		G_Printf("G_ReadDataFromFile: failed to open file: %s: %d\n", path, errno);
		return -1;
	}
		
	// Retrieve the xpsave data
	byte_count = read(fd, data, sz);
	if ( byte_count != sz )
	{
		G_Printf("G_ReadDataFromFile: failed to read required data (%d/%d): %s: %d\n", byte_count, sz, path, errno);
		return -1;
	}
	
	// Close the file
	if ( -1 == close(fd) )
	{
		G_Printf("G_ReadDataFromFile: failed to close file: %s: %d\n", path, errno);
		return -1;
	}
	
	// Success
	return 0;
}

qboolean G_DeleteFile(char const* path)
{
	return (-1 == unlink(path) ? qfalse : qtrue);
}

qboolean G_RenameFile(char const* src, char const* dest)
{
	return (rename(src, dest) == 0 ? qtrue : qfalse);
}

#endif  // WIN32
