# Microsoft Developer Studio Project File - Name="OMDSServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=OMDSServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OMDSServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OMDSServer.mak" CFG="OMDSServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OMDSServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OMDSServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/OMDS", TSWCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OMDSServer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /I "..\..\Common\WinRvTool" /I "..\..\Common\RivasTool" /I "..\..\Common\pthreads\src" /I "..\..\Common\ThreadPool" /I "..\..\Common\dbo" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "ENABLE_ENTITLEMENT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 DbgHelp.lib psapi.lib pthreadvc.lib rivas.lib threadpool.lib winrvtool.lib librvcpp.lib dboLib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../bin/OMDSServer.exe" /libpath:"..\..\bin"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "OMDSServer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\Common\WinRvTool" /I "..\..\Common\RivasTool" /I "..\..\Common\pthreads\src" /I "..\..\Common\ThreadPool" /I "..\..\Common\dbo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "ENABLE_ENTITLEMENT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 DbgHelp.lib psapi.lib librvcpp.lib libtib.lib pthreadvc.lib winrvtool.lib rivas.lib threadpool.lib dboLib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /nodefaultlib:"MSVCRT" /out:"../../bin/OMDSServer.exe" /pdbtype:sept /libpath:"..\..\bin"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "OMDSServer - Win32 Release"
# Name "OMDSServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ConClientListener.cpp
# End Source File
# Begin Source File

SOURCE=.\ConClientMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\ConClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\Connections.cpp
# End Source File
# Begin Source File

SOURCE=.\ConTib.cpp
# End Source File
# Begin Source File

SOURCE=.\DataManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DATib.cpp
# End Source File
# Begin Source File

SOURCE=.\DClientRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\DTibMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Entitlement.cpp
# End Source File
# Begin Source File

SOURCE=.\ExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\FutureHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\GreekHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\OMDSCommon.h
# End Source File
# Begin Source File

SOURCE=.\OMDSServer.cpp
# End Source File
# Begin Source File

SOURCE=.\OMDSServer.rc
# End Source File
# Begin Source File

SOURCE=.\OMDSServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionQuoteBestHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionQuoteHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\QuoteHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Runable.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Stock2QuoteHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\StockQuoteHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadClientProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadTibProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\UserData.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConClient.h
# End Source File
# Begin Source File

SOURCE=.\ConClientListener.h
# End Source File
# Begin Source File

SOURCE=.\ConClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\Connections.h
# End Source File
# Begin Source File

SOURCE=.\ConTib.h
# End Source File
# Begin Source File

SOURCE=.\DataManager.h
# End Source File
# Begin Source File

SOURCE=.\DATib.h
# End Source File
# Begin Source File

SOURCE=.\DClientRequest.h
# End Source File
# Begin Source File

SOURCE=.\DTibMessage.h
# End Source File
# Begin Source File

SOURCE=.\Entitlement.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=.\FutureHandler.h
# End Source File
# Begin Source File

SOURCE=.\GreekHandler.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\Main.h
# End Source File
# Begin Source File

SOURCE=.\MessageProcessor.h
# End Source File
# Begin Source File

SOURCE=.\OMDSServer.h
# End Source File
# Begin Source File

SOURCE=.\OMDSServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionQuoteBestHandler.h
# End Source File
# Begin Source File

SOURCE=.\OptionQuoteHandler.h
# End Source File
# Begin Source File

SOURCE=.\QuoteHandler.h
# End Source File
# Begin Source File

SOURCE=.\RequestManager.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Runable.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Stock2QuoteHandler.h
# End Source File
# Begin Source File

SOURCE=.\StockQuoteHandler.h
# End Source File
# Begin Source File

SOURCE=.\ThreadClientProcessor.h
# End Source File
# Begin Source File

SOURCE=.\ThreadTibProcessor.h
# End Source File
# Begin Source File

SOURCE=.\UserData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\OMDSServer.ico
# End Source File
# Begin Source File

SOURCE=.\res\OMDSServer.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
