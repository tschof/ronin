# Microsoft Developer Studio Project File - Name="TerraNova" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TerraNova - Win32 Static Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TerraNova.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TerraNova.mak" CFG="TerraNova - Win32 Static Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TerraNova - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TerraNova - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "TerraNova - Win32 Static Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderManager/TerraNova", XGLAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TerraNova - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zi /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\FixSession" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 fixsession-dynamic-r.lib ombase-static-r-can.lib Version.lib ThreadPool.lib dboLib.lib libcs.lib libct.lib libsybdb.lib libblk.lib DatabaseSettings.lib OrderTypeValidator.lib ExchangeHandlerBase.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /force /out:"..\..\..\Bin\TerraNova.exe" /libpath:"..\..\..\Lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\TerraNova.exe ..\..\..\bin\TerraNova_TDED.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TerraNova - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\Common\FixSession" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fixsession-dynamic-d.lib ombase-dynamic-d-can.lib Version.lib DboLib.lib libct.lib libcs.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /nodefaultlib:"MSVCRT" /force /out:"..\..\..\Bin\TerraNova.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\bin\TerraNova.exe ..\..\..\bin\TerraNova_TDED.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TerraNova - Win32 Static Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TerraNova___Win32_Static_Debug"
# PROP BASE Intermediate_Dir "TerraNova___Win32_Static_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Static_Debug"
# PROP Intermediate_Dir "Static_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "L:\DevLibs\ExecutionSystems\OMBase\Include" /I "L:\DevLibs\ExecutionSystems\FixSession\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "L:\DevLibs\ExecutionSystems\OMBase\Include" /I "L:\DevLibs\ExecutionSystems\FixSession\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 ombase-dynamic-d.lib fixsession-dynamic-d.lib fixmsg-dynamic-d.lib librvcpp.lib rvwin.lib libtib.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"L:\DevLibs\ExecutionSystems\OMBase\lib\v6" /libpath:"L:\DevLibs\ExecutionSystems\FixSession\lib\v6"
# ADD LINK32 ombase-static-d.lib fixsession-static-d.lib fixmsg-static-d.lib librvcpp.lib librvwin.lib libtib.lib /nologo /subsystem:windows /debug /machine:I386 /force /pdbtype:sept /libpath:"L:\DevLibs\ExecutionSystems\OMBase\lib\v6" /libpath:"L:\DevLibs\ExecutionSystems\FixSession\lib\v6"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TerraNova - Win32 Release"
# Name "TerraNova - Win32 Debug"
# Name "TerraNova - Win32 Static Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Message Handling Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TerraHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraStatus.cpp
# End Source File
# End Group
# Begin Group "Display Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TerraNovaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SeqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraMemTagSet.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraNova.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraNova.rc
# End Source File
# Begin Source File

SOURCE=.\TerraNovaSession.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Message Handling Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TerraHandler.h
# End Source File
# Begin Source File

SOURCE=.\TerraStatus.h
# End Source File
# End Group
# Begin Group "Display Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TerraNovaDoc.h
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TerraMemTagSet.h
# End Source File
# Begin Source File

SOURCE=.\TerraNova.h
# End Source File
# Begin Source File

SOURCE=.\TerraNovaSession.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
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

SOURCE=.\res\TerraNova.ico
# End Source File
# Begin Source File

SOURCE=.\res\TerraNova.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TerraNovaDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
