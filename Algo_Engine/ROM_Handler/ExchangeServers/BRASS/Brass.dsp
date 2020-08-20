# Microsoft Developer Studio Project File - Name="Brass" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Brass - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Brass.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Brass.mak" CFG="Brass - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Brass - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Brass - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers", TIVCAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Brass - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\ExchangeHandlerBase" /I "..\..\..\Common\Dbo" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_RECORDER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r.lib fixsession-dynamic-r.lib librvcpp.lib rvwin.lib rv.lib libtib.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidator.lib ExchangeHandlerBase.lib DatabaseSettings.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\Brass.exe" /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\Brass.exe ..\..\..\bin\Brass_TDED.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Brass - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_TEST" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ombase-dynamic-d.lib fixsession-dynamic-d.lib librvcpp.lib rvwin.lib rv.lib libtibd.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /nodefaultlib:"MSVCRT" /force /out:"..\..\..\Bin\Brass.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\Brass.exe ..\..\..\bin\Brass_TDED.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Brass - Win32 Release"
# Name "Brass - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Brass.cpp
# End Source File
# Begin Source File

SOURCE=.\Brass.rc
# End Source File
# Begin Source File

SOURCE=.\BRASSData.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\BRASSInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\BRASSRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\brassreject.cpp
# End Source File
# Begin Source File

SOURCE=.\brassreplace.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\BrassView.cpp
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Brass.h
# End Source File
# Begin Source File

SOURCE=.\BrassDoc.h
# End Source File
# Begin Source File

SOURCE=.\BrassException.h
# End Source File
# Begin Source File

SOURCE=.\BrassFixSession.h
# End Source File
# Begin Source File

SOURCE=.\BrassHandler.h
# End Source File
# Begin Source File

SOURCE=.\BrassMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\BrassReject.h
# End Source File
# Begin Source File

SOURCE=.\BrassReplace.h
# End Source File
# Begin Source File

SOURCE=.\BrassStatus.h
# End Source File
# Begin Source File

SOURCE=.\BrassView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Brass.ico
# End Source File
# Begin Source File

SOURCE=.\res\Brass.rc2
# End Source File
# Begin Source File

SOURCE=.\res\BrassDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_brass.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
