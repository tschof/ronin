# Microsoft Developer Studio Project File - Name="ArcaDirect" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ArcaDirect - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ArcaDirect.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ArcaDirect.mak" CFG="ArcaDirect - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ArcaDirect - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ArcaDirect - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecution/SERVERS/ExchangeServers/ARCA_D", HGZBAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ArcaDirect - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "C:\sybase\OCS-12_5\lib" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-static-r-can.lib DBOLib.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib ExchangeHandlerBase.lib /nologo /subsystem:windows /machine:I386 /out:"ArcaDirect.exe" /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ArcaDirect - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ombase-dynamic-d-can.lib libcs.lib DboLib.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"C:\OrderManager\ArcaDirect\ArcaDirect.exe"

!ENDIF 

# Begin Target

# Name "ArcaDirect - Win32 Release"
# Name "ArcaDirect - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ArcaDirect.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirect.rc
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectSequenceSet.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectSession.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectView.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ArcaDirect.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectDoc.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectHandler.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectMessage.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectSequenceSet.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectSession.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectStatus.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectTestRequest.h
# End Source File
# Begin Source File

SOURCE=.\ArcaDirectView.h
# End Source File
# Begin Source File

SOURCE=.\ClientMessages.h
# End Source File
# Begin Source File

SOURCE=.\DirectConnection.h
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

SOURCE=.\res\ArcaDirect.ico
# End Source File
# Begin Source File

SOURCE=.\res\ArcaDirect.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ArcaDirectDoc.ico
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
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
