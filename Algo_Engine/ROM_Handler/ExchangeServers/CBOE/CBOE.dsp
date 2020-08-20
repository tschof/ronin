# Microsoft Developer Studio Project File - Name="CBOE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CBOE - Win32 FIX42 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CBOE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CBOE.mak" CFG="CBOE - Win32 FIX42 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CBOE - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CBOE - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CBOE - Win32 471 Release" (based on "Win32 (x86) Application")
!MESSAGE "CBOE - Win32 471 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CBOE - Win32 FIX42 Release" (based on "Win32 (x86) Application")
!MESSAGE "CBOE - Win32 FIX42 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers/CBOE", IKXCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CBOE - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 FixMsg42-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib DatabaseSettings.lib OrderTypeValidator.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CBOE.exe" /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "CBOE - Win32 Debug"

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
# ADD LINK32 FixMsg42-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\CBOED.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "CBOE - Win32 471 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CBOE___Win32_471_Release"
# PROP BASE Intermediate_Dir "CBOE___Win32_471_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "471_Release"
# PROP Intermediate_Dir "471_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CBOE_471" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 fixsession-dynamic-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CBOE.exe" /libpath:"..\..\..\Bin"
# ADD LINK32 fixsession-dynamic-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CBOE_471.exe" /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "CBOE - Win32 471 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CBOE___Win32_471_Debug"
# PROP BASE Intermediate_Dir "CBOE___Win32_471_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "CBOE___Win32_471_Debug"
# PROP Intermediate_Dir "CBOE___Win32_471_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CBOE_471" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 fixsession-dynamic-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\CBOED.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# ADD LINK32 fixsession-dynamic-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\CBOE_471D.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "CBOE - Win32 FIX42 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CBOE___Win32_FIX42_Release"
# PROP BASE Intermediate_Dir "CBOE___Win32_FIX42_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "FIX42_Release"
# PROP Intermediate_Dir "FIX42_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\common" /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\Fixmsg42" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CBOE_FIX42" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 FixMsg42-r.lib ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CBOE.exe" /libpath:"..\..\..\Bin"
# ADD LINK32 FixMsg42-r.lib ombase-dynamic-r-can.lib version.lib DBOLib.lib libct.lib libcs.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\CBOE.exe" /libpath:"..\..\..\lib"

!ELSEIF  "$(CFG)" == "CBOE - Win32 FIX42 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CBOE___Win32_FIX42_Debug"
# PROP BASE Intermediate_Dir "CBOE___Win32_FIX42_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "FIX42_Debug"
# PROP Intermediate_Dir "FIX42_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\common" /I "..\..\..\Common\ordermanagerbase" /I "..\..\..\Common\FixSession" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\Fixmsg42" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CBOE_FIX42" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 FixMsg42-d.lib ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtibd.lib rv.lib version.lib DBOLib.lib libct.lib libcs.lib version.lib threadpool.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\CBOED.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# ADD LINK32 FixMsg42-d.lib ombase-dynamic-d-can.lib version.lib DBOLib.lib libct.lib libcs.lib threadpool.lib OrderTypeValidatorD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\Bin\CBOED_FIX42.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ENDIF 

# Begin Target

# Name "CBOE - Win32 Release"
# Name "CBOE - Win32 Debug"
# Name "CBOE - Win32 471 Release"
# Name "CBOE - Win32 471 Debug"
# Name "CBOE - Win32 FIX42 Release"
# Name "CBOE - Win32 FIX42 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CBOE.cpp
# End Source File
# Begin Source File

SOURCE=.\CBOE.rc
# End Source File
# Begin Source File

SOURCE=.\CBOEDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CBOEFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\CBOEHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\CBOEMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CBOEView.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ExchangeStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\OptFixSession.cpp
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

SOURCE=.\CBOE.h
# End Source File
# Begin Source File

SOURCE=.\CBOEDoc.h
# End Source File
# Begin Source File

SOURCE=.\CBOEFixSession.h
# End Source File
# Begin Source File

SOURCE=.\CBOEHandler.h
# End Source File
# Begin Source File

SOURCE=.\CBOEMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\CBOEView.h
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

SOURCE=..\Common\OptFixSession.h
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

SOURCE=.\res\CBOE.ico
# End Source File
# Begin Source File

SOURCE=.\res\CBOE.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CBOEDoc.ico
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
