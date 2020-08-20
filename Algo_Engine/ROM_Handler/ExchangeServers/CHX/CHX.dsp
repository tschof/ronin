# Microsoft Developer Studio Project File - Name="CHX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CHX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CHX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CHX.mak" CFG="CHX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CHX - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CHX - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers", TIVCAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CHX - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r.lib fixsession-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidator.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CHX.exe" /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CHX - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /I /I /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ombase-dynamic-d.lib fixsession-dynamic-d.lib librvcpp.lib librvwin.lib libtibd.lib Version.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidatorD.lib /nologo /subsystem:windows /debug /machine:I386 /force /out:"..\..\..\Bin\CHX.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CHX - Win32 Release"
# Name "CHX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CHX.cpp
# End Source File
# Begin Source File

SOURCE=.\CHX.rc
# End Source File
# Begin Source File

SOURCE=.\CHXDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ChxFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXreject.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXreplace.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\CHXView.cpp
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

SOURCE=.\CHX.h
# End Source File
# Begin Source File

SOURCE=.\CHXDoc.h
# End Source File
# Begin Source File

SOURCE=.\ChxFixSession.h
# End Source File
# Begin Source File

SOURCE=.\CHXHandler.h
# End Source File
# Begin Source File

SOURCE=.\CHXMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\CHXReject.h
# End Source File
# Begin Source File

SOURCE=.\CHXReplace.h
# End Source File
# Begin Source File

SOURCE=.\CHXStatus.h
# End Source File
# Begin Source File

SOURCE=.\CHXView.h
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
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_CHX.ico
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
