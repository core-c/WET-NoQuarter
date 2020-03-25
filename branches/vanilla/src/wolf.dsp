# Microsoft Developer Studio Project File - Name="wolf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wolf - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wolf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wolf.mak" CFG="wolf - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wolf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wolf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "wolf - Win32 Debug Dedicated" (based on "Win32 (x86) Application")
!MESSAGE "wolf - Win32 Release Dedicated" (based on "Win32 (x86) Application")
!MESSAGE "wolf - Win32 Debug Update Server" (based on "Win32 (x86) Application")
!MESSAGE "wolf - Win32 Release Update Server" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Wolfenstein MP/src", DTMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib Iphlpapi.lib curl-7.12.2\lib\libcurl.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /out:".\Release/ET.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib
# Begin Custom Build
InputPath=.\Release\ET.exe
SOURCE="$(InputPath)"

"libcurl.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd curl-7.12.2 
	cd lib 
	nmake /f Makefile.vc6 CFG=release 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=compile libcurl
PreLink_Cmds=cd curl-7.12.2\lib	nmake /f Makefile.vc6 CFG=release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"win32\winquake.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/wolf.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib Iphlpapi.lib curl-7.12.2\lib\libcurld.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd" /out:".\Debug/ET.exe"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=compile libcurl
PreLink_Cmds=cd curl-7.12.2\lib	nmake /f Makefile.vc6 CFG=debug
PostBuild_Cmds=copy "debug\et.exe" "$(WOLFDIR)\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wolf___Win32_Debug_Dedicated"
# PROP BASE Intermediate_Dir "wolf___Win32_Debug_Dedicated"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Dedicated"
# PROP Intermediate_Dir "Debug_Dedicated"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /Fr /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DO_LIGHT_DEDICATED" /D "DEDICATED" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"win32\winquake.res" /d "_DEBUG"
# ADD RSC /l 0x409 /fo"win32\winquake.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/wolf.bsc"
# ADD BSC32 /nologo /o"Debug/wolf.bsc"
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /out:".\Debug/WolfMP.exe"
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Debug\botlib.lib Iphlpapi.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /out:".\Debug_Dedicated/WolfDED.exe"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copying game..
PostBuild_Cmds=cp Debug/qagame_mp_x86.dll Debug_Dedicated
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wolf___Win32_Release_Dedicated"
# PROP BASE Intermediate_Dir "wolf___Win32_Release_Dedicated"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Dedicated"
# PROP Intermediate_Dir "Release_Dedicated"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DEDICATED" /D "DO_LIGHT_DEDICATED" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /out:".\Release/WolfMP.exe"
# SUBTRACT BASE LINK32 /incremental:yes /nodefaultlib
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Release\botlib.lib Iphlpapi.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /out:".\Release_Dedicated/WolfDED.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copying game..
PostBuild_Cmds=cp Release/qagame_mp_x86.dll Release_Dedicated
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wolf___Win32_Debug_Update_Server"
# PROP BASE Intermediate_Dir "wolf___Win32_Debug_Update_Server"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Update_Server"
# PROP Intermediate_Dir "Debug_Update_Server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DO_LIGHT_DEDICATED" /D "DEDICATED" /Fr /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DO_LIGHT_DEDICATED" /D "DEDICATED" /D "UPDATE_SERVER" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"win32\winquake.res" /d "_DEBUG"
# ADD RSC /l 0x409 /fo"win32\winquake.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/wolf.bsc"
# ADD BSC32 /nologo /o"Debug/wolf.bsc"
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Debug\botlib.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /out:".\Debug_Dedicated/WolfDED.exe"
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Debug\botlib.lib Iphlpapi.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /out:".\Debug_Update_Server/ETUpdate.exe"
# SUBTRACT LINK32 /profile /nodefaultlib

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wolf___Win32_Release_Update_Server"
# PROP BASE Intermediate_Dir "wolf___Win32_Release_Update_Server"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Update_Server"
# PROP Intermediate_Dir "Release_Update_Server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DEDICATED" /D "DO_LIGHT_DEDICATED" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__USEA3D" /D "__A3D_GEOM" /D "DEDICATED" /D "DO_LIGHT_DEDICATED" /D "UPDATE_SERVER" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Release\botlib.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /out:".\Release_Dedicated/WolfDED.exe"
# SUBTRACT BASE LINK32 /incremental:yes /nodefaultlib
# ADD LINK32 advapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib ole32.lib shell32.lib .\botlib\Release\botlib.lib Iphlpapi.lib /nologo /stack:0x800000 /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libc" /out:".\Release_Update_Server/ETUpdate.exe" /libpath:"libwww-cvs\Library\src\windows\VC6-static\Release" /libpath:"C:\Python-2.2.2\PCBuild"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib

!ENDIF 

# Begin Target

# Name "wolf - Win32 Release"
# Name "wolf - Win32 Debug"
# Name "wolf - Win32 Debug Dedicated"
# Name "wolf - Win32 Release Dedicated"
# Name "wolf - Win32 Debug Update Server"
# Name "wolf - Win32 Release Update Server"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\client\cl_cgame.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_cin.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_console.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_input.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_keys.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_main.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_net_chan.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_parse.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_scrn.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\cl_ui.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_load.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_patch.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_polylib.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_test.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_trace.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cmd.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\common.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\cvar.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\files.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\huffman.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\md4.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\msg.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\net_chan.c
# End Source File
# Begin Source File

SOURCE=.\null\null_client.c

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\null\null_input.c

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\null\null_snddma.c

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\game\q_math.c
# End Source File
# Begin Source File

SOURCE=.\game\q_shared.c
# End Source File
# Begin Source File

SOURCE=.\client\snd_adpcm.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\snd_dma.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\snd_mem.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\snd_mix.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\client\snd_wavelet.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\sv_bot.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_ccmds.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_client.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_game.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_init.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_main.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_net_chan.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_snapshot.c
# End Source File
# Begin Source File

SOURCE=.\server\sv_world.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\unzip.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\vm.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\vm_interpreted.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\vm_x86.c
# End Source File
# Begin Source File

SOURCE=.\win32\win_eh.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\win_input.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\win_main.c
# End Source File
# Begin Source File

SOURCE=.\win32\win_net.c
# End Source File
# Begin Source File

SOURCE=.\win32\win_shared.c
# End Source File
# Begin Source File

SOURCE=.\win32\win_snd.c

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\win_syscon.c
# End Source File
# Begin Source File

SOURCE=.\win32\win_wndproc.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\A3D\a3dWrapperdp.h
# End Source File
# Begin Source File

SOURCE=.\game\bg_public.h
# End Source File
# Begin Source File

SOURCE=.\botai\botai.h
# End Source File
# Begin Source File

SOURCE=.\game\botlib.h
# End Source File
# Begin Source File

SOURCE=.\cgame\cg_public.h
# End Source File
# Begin Source File

SOURCE=.\client\client.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_local.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_patch.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_polylib.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\cm_public.h
# End Source File
# Begin Source File

SOURCE=.\game\g_public.h
# End Source File
# Begin Source File

SOURCE=.\A3D\ia3dapi.h
# End Source File
# Begin Source File

SOURCE=.\ui\keycodes.h
# End Source File
# Begin Source File

SOURCE=.\client\keys.h
# End Source File
# Begin Source File

SOURCE=.\game\q_shared.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\qcommon.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\qfiles.h
# End Source File
# Begin Source File

SOURCE=.\renderer\qgl.h
# End Source File
# Begin Source File

SOURCE=.\server\server.h
# End Source File
# Begin Source File

SOURCE=.\A3D\snd_A3D.h
# End Source File
# Begin Source File

SOURCE=.\A3D\snd_a3dg.h
# End Source File
# Begin Source File

SOURCE=.\client\snd_local.h
# End Source File
# Begin Source File

SOURCE=.\client\snd_public.h
# End Source File
# Begin Source File

SOURCE=.\game\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=.\renderer\tr_local.h
# End Source File
# Begin Source File

SOURCE=.\renderer\tr_public.h
# End Source File
# Begin Source File

SOURCE=.\cgame\tr_types.h
# End Source File
# Begin Source File

SOURCE=.\ui\ui_public.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\unzip.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\vm_local.h
# End Source File
# Begin Source File

SOURCE=.\win32\win_local.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\win32\background.bmp
# End Source File
# Begin Source File

SOURCE=.\win32\clear.bmp
# End Source File
# Begin Source File

SOURCE=.\win32\qe3.ico
# End Source File
# Begin Source File

SOURCE=.\win32\winquake.rc
# End Source File
# Begin Source File

SOURCE=.\win32\wolfet.ico
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\main\ui\addfilter.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\bookp.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\bookv.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\bookz.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\cdkey.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\cinematicmenu.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\clipboard.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\connect.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\controls.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\createfavorite.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\createserver.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\credit.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\default.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\demo.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\demo_quit.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\endofgame.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\error.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\fight.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\findplayer.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\help.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\hud.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\hud.txt

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame.txt

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_about.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_addbot.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_callvote.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_controls.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_help.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_join.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_leave.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_loadsave.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_options.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_orders.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_player.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_system.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ingame_vote.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\joinserver.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\load.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\main.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\menudef.h
# End Source File
# Begin Source File

SOURCE=..\main\ui\menus.txt

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\min_hud.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\mod.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\multiplayer.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\notebook.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\options.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\ordershelp.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\password.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\play.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\player.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\powerup.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\quit.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\quitcredit.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\resetscore.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\score.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\serverinfo.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\setup.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\skirmish.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\system.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\teamscore.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\vid_restart.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\wm_limbo.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\wm_pickplayer.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\wm_pickteam.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\ui\wm_quickmessage.menu

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\main\scripts\ai.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\alpha.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\blacksmokeanim.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\blacksmokeanimb.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\clipboard.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\common.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\cursorhints.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\decals.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\eerie.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\expblue.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\explode1.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\fijets.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\firest.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\flamethrower.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\funnel.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\gfx.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\heat.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\lights.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\liquid.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\maxx.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\menu.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\metal.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\models.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\netest.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\oldwolf.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\organics.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\particle.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\q3view.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\sfx.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\skin.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\sky.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\solo.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\terrain.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\test.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\twiltb.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\twiltb2.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\ui.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\ui_hud.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\ui_kc.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\ui_notebook.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\viewflames.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\walls.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main\scripts\z_light.shader

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

!ENDIF 

# End Source File
# End Group
# Begin Group "text"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Group "Punk Buster"

# PROP Default_Filter ""
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pb\pbcl.cpp
# End Source File
# Begin Source File

SOURCE=.\pb\pbmd5.cpp
# End Source File
# Begin Source File

SOURCE=.\pb\pbsv.cpp
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pb\pbcl.h
# End Source File
# Begin Source File

SOURCE=.\pb\pbcommon.h
# End Source File
# Begin Source File

SOURCE=.\pb\pbmd5.h
# End Source File
# Begin Source File

SOURCE=.\pb\pbsv.h
# End Source File
# End Group
# End Group
# Begin Group "docs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\docs\Changelog
# End Source File
# Begin Source File

SOURCE=.\docs\Notes
# End Source File
# End Group
# Begin Group "download"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\qcommon\dl_local.h
# End Source File
# Begin Source File

SOURCE=.\qcommon\dl_main.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\qcommon\dl_main_curl.c
# End Source File
# Begin Source File

SOURCE=.\qcommon\dl_main_stubs.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\qcommon\dl_public.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\headers.txt

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\a3d\vc60.pdb

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A3D\a3dwrapperd.lib

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A3D\a3dwrapper.lib

!IF  "$(CFG)" == "wolf - Win32 Release"

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Dedicated"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Dedicated"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wolf - Win32 Release Update Server"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
