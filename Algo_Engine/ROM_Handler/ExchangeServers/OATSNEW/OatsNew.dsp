# Microsoft Developer Studio Project File - Name="OatsNew" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=OatsNew - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OatsNew.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OatsNew.mak" CFG="OatsNew - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OatsNew - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OatsNew - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers/OATSNEW", AVHDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OatsNew - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\ThreadPool" /I "..\..\..\Common\dbo" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ThreadPool-dyn.lib ombase-dynamic-r-can.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib-dyn.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../bin_vc6/OatsNew.exe" /libpath:"..\..\..\Lib_vc6_r"

!ELSEIF  "$(CFG)" == "OatsNew - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\ThreadPool" /I "..\..\..\Common\dbo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ThreadPool.lib ombase-dynamic-d-can.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolibd.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../bin/OatsNew.exe" /pdbtype:sept /libpath:"..\..\..\lib_vc6"

!ENDIF 

# Begin Target

# Name "OatsNew - Win32 Release"
# Name "OatsNew - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AdminServer.cpp
# End Source File
# Begin Source File

SOURCE=.\AdminSession.cpp
# End Source File
# Begin Source File

SOURCE=.\AdminThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Count.cpp
# End Source File
# Begin Source File

SOURCE=.\DataStore.cpp
# End Source File
# Begin Source File

SOURCE=.\DBConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\Logger.cpp
# End Source File
# Begin Source File

SOURCE=.\mdump.cpp
# End Source File
# Begin Source File

SOURCE=.\OATS.cpp
# End Source File
# Begin Source File

SOURCE=.\oats.rc
# End Source File
# Begin Source File

SOURCE=.\OatsCallback.cpp
# End Source File
# Begin Source File

SOURCE=.\OatsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OatsServer.cpp
# End Source File
# Begin Source File

SOURCE=.\OMRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\OmSession.cpp
# End Source File
# Begin Source File

SOURCE=.\OrderMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionBase.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TestDB.cpp
# End Source File
# Begin Source File

SOURCE=.\VDateTimeEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ZLog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdminServer.h
# End Source File
# Begin Source File

SOURCE=.\AdminSession.h
# End Source File
# Begin Source File

SOURCE=.\AdminThread.h
# End Source File
# Begin Source File

SOURCE=.\ClientMsg.h
# End Source File
# Begin Source File

SOURCE=.\Count.h
# End Source File
# Begin Source File

SOURCE=.\DataStore.h
# End Source File
# Begin Source File

SOURCE=.\DBConnection.h
# End Source File
# Begin Source File

SOURCE=.\DbgHelp.h
# End Source File
# Begin Source File

SOURCE=.\Logger.h
# End Source File
# Begin Source File

SOURCE=.\mdump.h
# End Source File
# Begin Source File

SOURCE=.\OATS.h
# End Source File
# Begin Source File

SOURCE=.\OatsCallback.h
# End Source File
# Begin Source File

SOURCE=.\OatsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OatsServer.h
# End Source File
# Begin Source File

SOURCE=.\OMRequest.h
# End Source File
# Begin Source File

SOURCE=.\OmSession.h
# End Source File
# Begin Source File

SOURCE=.\OrderMsg.h
# End Source File
# Begin Source File

SOURCE=.\RequestFactory.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SessionBase.h
# End Source File
# Begin Source File

SOURCE=.\StatusMsg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TestDB.h
# End Source File
# Begin Source File

SOURCE=.\VDateTimeEdit.h
# End Source File
# Begin Source File

SOURCE=.\ZLog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\OATS.ico
# End Source File
# Begin Source File

SOURCE=.\res\OatsNew.ico
# End Source File
# Begin Source File

SOURCE=.\res\OatsNew.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
