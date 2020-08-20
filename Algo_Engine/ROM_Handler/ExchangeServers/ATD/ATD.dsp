# Microsoft Developer Studio Project File - Name="ATD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ATD - Win32 ATDV Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ATD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ATD.mak" CFG="ATD - Win32 ATDV Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ATD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ATD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ATD - Win32 ATDV Release" (based on "Win32 (x86) Application")
!MESSAGE "ATD - Win32 ATDV Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers/ATD", TRHDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ATD - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 FixMsg42-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\ATD.exe" /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "ATD - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FixMsg42-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidatorD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\ATDD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "ATD - Win32 ATDV Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ATD___Win32_ATDV_Release"
# PROP BASE Intermediate_Dir "ATD___Win32_ATDV_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ATD___Win32_ATDV_Release"
# PROP Intermediate_Dir "ATD___Win32_ATDV_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_ATDV" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 FixMsg42-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\ATD.exe" /libpath:"..\..\..\Bin"
# ADD LINK32 FixMsg42-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\ATDV.exe" /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "ATD - Win32 ATDV Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ATD___Win32_ATDV_Debug"
# PROP BASE Intermediate_Dir "ATD___Win32_ATDV_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ATD___Win32_ATDV_Debug"
# PROP Intermediate_Dir "ATD___Win32_ATDV_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_ATDV" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 FixMsg42-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidatorD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\ATDD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# ADD LINK32 FixMsg42-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib OrderTypeValidatorD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\ATDVD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ENDIF 

# Begin Target

# Name "ATD - Win32 Release"
# Name "ATD - Win32 Debug"
# Name "ATD - Win32 ATDV Release"
# Name "ATD - Win32 ATDV Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ATD.cpp
# End Source File
# Begin Source File

SOURCE=.\ATD.rc
# End Source File
# Begin Source File

SOURCE=.\ATDDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ATDFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\ATDHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ATDView.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\OptionMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ATD.h
# End Source File
# Begin Source File

SOURCE=.\ATDDoc.h
# End Source File
# Begin Source File

SOURCE=.\ATDFixSession.h
# End Source File
# Begin Source File

SOURCE=.\ATDHandler.h
# End Source File
# Begin Source File

SOURCE=.\ATDView.h
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeApp.h
# End Source File
# Begin Source File

SOURCE=..\Common\Exchangehandler.h
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeMemTagSet.h
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeStatus.h
# End Source File
# Begin Source File

SOURCE=..\Common\OptionMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Common\SeqDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ATD.ico
# End Source File
# Begin Source File

SOURCE=.\res\ATD.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ATDDoc.ico
# End Source File
# Begin Source File

SOURCE=..\Common\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=..\Common\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\Common\res\idr_terr.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
