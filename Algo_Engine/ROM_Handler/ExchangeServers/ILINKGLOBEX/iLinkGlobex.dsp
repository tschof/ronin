# Microsoft Developer Studio Project File - Name="iLinkGlobex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=iLinkGlobex - Win32 Debug FOC
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iLinkGlobex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iLinkGlobex.mak" CFG="iLinkGlobex - Win32 Debug FOC"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iLinkGlobex - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "iLinkGlobex - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "iLinkGlobex - Win32 Debug FOC" (based on "Win32 (x86) Application")
!MESSAGE "iLinkGlobex - Win32 Release FOC" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/ExchangeServers/iLinkGlobex", ATBDAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iLinkGlobex - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\common" /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\Fixmsg42" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r-can.lib FixMsg42-r-dyn.lib Version.lib DboLib-dyn.lib libcs.lib libct.lib libsybdb.lib libblk.lib threadpool-dyn.lib dbolib-dyn.lib DatabaseSettings-dyn.lib OrderTypeValidator-dyn.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Bin\iLinkGlobex.exe" /libpath:"..\..\..\Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "iLinkGlobex - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\Fixmsg42" /I "..\..\..\Common\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FixMsg42-d.lib ombase-dynamic-d-can.lib version.lib DBOLib.lib libct.lib libcs.lib threadpool.lib OrderTypeValidatorD.lib DatabaseSettingsD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"mfc42.lib" /nodefaultlib:"mfcs42.lib" /nodefaultlib:"LIBCMT" /nodefaultlib:"MSVCRT" /out:"C:\OrderManager\iLinkGlobex\iLinkGlobexD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "iLinkGlobex - Win32 Debug FOC"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "iLinkGlobex___Win32_Debug_FOC"
# PROP BASE Intermediate_Dir "iLinkGlobex___Win32_Debug_FOC"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "iLinkGlobex___Win32_Debug_FOC"
# PROP Intermediate_Dir "iLinkGlobex___Win32_Debug_FOC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\OrderExecutions\COMMON\OrderManagerBase" /I "C:\OrderExecutions\COMMON\FixMsg42" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "GLOBEX_FOC" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\OrderExecutions\COMMON\OrderManagerBase" /I "C:\OrderExecutions\COMMON\FixMsg42" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "GLOBEX_FOC" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 ombase-dynamic-d.lib FixMsg42-d.lib librvcpp.lib librvwin.lib libtibd.lib Version.lib DboLib.lib libct.lib libcs.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"mfc42.lib mfcs42.lib" /out:"..\..\..\Bin\iLinkGlobexFOCD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# ADD LINK32 ombase-dynamic-d.lib FixMsg42-d.lib librvcpp.lib librvwin.lib libtibd.lib Version.lib DboLib.lib libct.lib libcs.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"mfc42.lib mfcs42.lib" /out:"..\..\..\Bin\iLinkGlobexFOCD.exe" /pdbtype:sept /libpath:"..\..\..\Bin"

!ELSEIF  "$(CFG)" == "iLinkGlobex - Win32 Release FOC"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "iLinkGlobex___Win32_Release_FOC"
# PROP BASE Intermediate_Dir "iLinkGlobex___Win32_Release_FOC"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "iLinkGlobex___Win32_Release_FOC"
# PROP Intermediate_Dir "iLinkGlobex___Win32_Release_FOC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "C:\OrderExecutions\COMMON\OrderManagerBase" /I "C:\OrderExecutions\COMMON\FixMsg42" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\OrderExecutions\COMMON\OrderManagerBase" /I "C:\OrderExecutions\COMMON\FixMsg42" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "C:\OrderExecutions\COMMON\OrderManagerBase" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 ombase-dynamic-r.lib FixMsg42-r.lib librvcpp.lib librvwin.lib libtib.lib Version.lib DboLib.lib libct.lib libcs.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Bin\iLinkGlobex.exe" /libpath:"..\..\..\Bin"
# ADD LINK32 ombase-dynamic-r.lib FixMsg42-r.lib librvcpp.lib librvwin.lib libtib.lib Version.lib DboLib.lib libct.lib libcs.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Bin\iLinkGlobexFOC.exe" /libpath:"..\..\..\Bin"

!ENDIF 

# Begin Target

# Name "iLinkGlobex - Win32 Release"
# Name "iLinkGlobex - Win32 Debug"
# Name "iLinkGlobex - Win32 Debug FOC"
# Name "iLinkGlobex - Win32 Release FOC"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GlobexSequenceGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobex.cpp
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobex.rc
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobexDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobexView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ZLog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GlobexSequenceGenerator.h
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobex.h
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobexDoc.h
# End Source File
# Begin Source File

SOURCE=.\iLinkGlobexView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\ZLog.h
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

SOURCE=.\res\idr_iLinkGlobex.ico
# End Source File
# Begin Source File

SOURCE=.\res\iLinkGlobex.ico
# End Source File
# Begin Source File

SOURCE=.\res\iLinkGlobex.rc2
# End Source File
# Begin Source File

SOURCE=.\res\iLinkGlobexDoc.ico
# End Source File
# End Group
# Begin Group "OrderManagerFiles"

# PROP Default_Filter ".h,.cpp"
# Begin Source File

SOURCE=.\OrderManagerInterface.cpp
# End Source File
# End Group
# Begin Group "OrderHandlerFiles"

# PROP Default_Filter ".h,.cpp"
# Begin Source File

SOURCE=.\OrderHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\OrderHandler.h
# End Source File
# Begin Source File

SOURCE=.\OrderHandlerInterface.cpp
# End Source File
# End Group
# Begin Group "FixSessionFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FixSessionInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobexFixSession.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobexFixSession.h
# End Source File
# End Group
# Begin Group "OrderStatusFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GlobexStatusHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobexStatusHandler.h
# End Source File
# Begin Source File

SOURCE=.\OrderStatusInterface.cpp
# End Source File
# End Group
# Begin Group "MessageAppFiles"

# PROP Default_Filter ".h,.cpp"
# Begin Source File

SOURCE=.\ClientMessageProcessing.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageAppInterface.cpp
# End Source File
# End Group
# Begin Group "MemTagSet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GlobexMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobexMemTagSet.h
# End Source File
# End Group
# Begin Group "SequenceNumber"

# PROP Default_Filter ".h,.cpp"
# Begin Source File

SOURCE=.\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
