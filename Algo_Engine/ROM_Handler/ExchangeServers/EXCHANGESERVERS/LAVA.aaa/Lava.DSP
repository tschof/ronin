# Microsoft Developer Studio Project File - Name="LAVA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LAVA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LAVA.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LAVA.MAK" CFG="LAVA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LAVA - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LAVA - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderManager/LAVA", NNUAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LAVA - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\FixSession" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r-can.lib fixsession-dynamic-r.lib DBOLib.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib ExchangeHandlerBase.lib /nologo /subsystem:windows /machine:I386 /out:"LAVA.exe" /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /profile /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\LAVA.exe ..\..\..\bin\LAVA_TDED.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LAVA - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\FixSession" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fixsession-dynamic-d.lib ombase-dynamic-d-can.lib libcs.lib DboLib.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /out:"C:\OrderManager\LAVA\LAVA.exe" /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\LAVA.exe ..\..\..\bin\LAVA_TDED.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LAVA - Win32 Release"
# Name "LAVA - Win32 Debug"
# Begin Source File

SOURCE=.\LAVA.cpp
# End Source File
# Begin Source File

SOURCE=.\LAVA.h
# End Source File
# Begin Source File

SOURCE=.\LAVA.rc
# End Source File
# Begin Source File

SOURCE=.\res\LAVA.rc2
# End Source File
# Begin Source File

SOURCE=.\LAVADoc.cpp
# End Source File
# Begin Source File

SOURCE=.\LAVADoc.h
# End Source File
# Begin Source File

SOURCE=.\LavaFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaFixSession.h
# End Source File
# Begin Source File

SOURCE=.\LAVAHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\LAVAHandler.h
# End Source File
# Begin Source File

SOURCE=.\LavaMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\LAVAStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\LAVAStatus.h
# End Source File
# Begin Source File

SOURCE=.\LAVAView.cpp
# End Source File
# Begin Source File

SOURCE=.\LAVAView.h
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_terr.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# End Target
# End Project
