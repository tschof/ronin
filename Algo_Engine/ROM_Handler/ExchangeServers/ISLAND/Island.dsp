# Microsoft Developer Studio Project File - Name="Island" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Island - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Island.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Island.mak" CFG="Island - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Island - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Island - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderManager/Island", HJPAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Island - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\Common\ThreadPool" /I "..\..\..\Common\OrderTypeValidator" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r-can.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib DatabaseSettings.lib version.lib threadpool.lib OrderTypeValidator.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\Bin\Island.exe" /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\Island.exe ..\..\..\bin\Island_TDED.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Island - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ombase-dynamic-d-can.lib libcs.lib DboLib.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCMT" /out:"C:\OrderManager\ISLD\Island.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\Island.exe ..\..\..\bin\Island_TDED.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Island - Win32 Release"
# Name "Island - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Message Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FromIslandCancelReject.cpp
# End Source File
# Begin Source File

SOURCE=.\FromIslandExecution.cpp
# End Source File
# Begin Source File

SOURCE=.\FromIslandReject.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCancelReject.cpp
# End Source File
# End Group
# Begin Group "Message Handling Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IslandHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandSession.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandStatus.cpp
# End Source File
# End Group
# Begin Group "Island Mesage Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IslandCancel.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandFields.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandOrder.cpp
# End Source File
# End Group
# Begin Group "Database Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FixSequenceSet.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Island.cpp
# End Source File
# Begin Source File

SOURCE=.\Island.rc
# End Source File
# Begin Source File

SOURCE=.\IslandDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandSequenceSet.cpp
# End Source File
# Begin Source File

SOURCE=.\IslandView.cpp
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
# Begin Group "Message Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FromIslandCancelReject.h
# End Source File
# Begin Source File

SOURCE=.\FromIslandExecution.h
# End Source File
# Begin Source File

SOURCE=.\FromIslandReject.h
# End Source File
# Begin Source File

SOURCE=.\SendCancelReject.h
# End Source File
# End Group
# Begin Group "Message Handling Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IslandHandler.h
# End Source File
# Begin Source File

SOURCE=.\IslandSession.h
# End Source File
# Begin Source File

SOURCE=.\IslandStatus.h
# End Source File
# End Group
# Begin Group "Island Message Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IslandCancel.h
# End Source File
# Begin Source File

SOURCE=.\IslandFields.h
# End Source File
# Begin Source File

SOURCE=.\IslandHeartbeat.h
# End Source File
# Begin Source File

SOURCE=.\IslandLogin.h
# End Source File
# Begin Source File

SOURCE=.\IslandLogout.h
# End Source File
# Begin Source File

SOURCE=.\IslandMessage.h
# End Source File
# Begin Source File

SOURCE=.\IslandOrder.h
# End Source File
# Begin Source File

SOURCE=.\IslandRewind.h
# End Source File
# End Group
# Begin Group "Database Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FixSequenceSet.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Island.h
# End Source File
# Begin Source File

SOURCE=.\IslandDoc.h
# End Source File
# Begin Source File

SOURCE=.\IslandMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\IslandSequenceSet.h
# End Source File
# Begin Source File

SOURCE=.\IslandView.h
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

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
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

SOURCE=.\res\Island.ico
# End Source File
# Begin Source File

SOURCE=.\res\Island.rc2
# End Source File
# Begin Source File

SOURCE=.\res\IslandDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\Island.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
