# Microsoft Developer Studio Project File - Name="Actor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Actor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Actor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Actor.mak" CFG="Actor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Actor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Actor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Actor", FLNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Actor - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /I "Y:\components\rv\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 librvcpp.lib rvwin.lib libtib.lib rv.lib Xapi32mc.lib /nologo /subsystem:windows /machine:I386 /libpath:"l:\devlibs\rv\lib" /libpath:"l:\devlibs\rv\lib\save" /libpath:"C:\Sangoma X.25\Win32"

!ELSEIF  "$(CFG)" == "Actor - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "Y:\components\rv\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 librvcpp.lib rvwin.lib libtib.lib rv.lib Xapi32mc.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcmt.lib" /pdbtype:sept /libpath:"l:\devlibs\rv\lib" /libpath:"l:\devlibs\rv\lib\save" /libpath:"C:\Sangoma X.25\Win32"

!ENDIF 

# Begin Target

# Name "Actor - Win32 Release"
# Name "Actor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Message Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Csv.cpp
# End Source File
# Begin Source File

SOURCE=.\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\Order.cpp
# End Source File
# End Group
# Begin Group "Act Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Act.cpp
# End Source File
# End Group
# Begin Group "Message Handling Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActSession.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\Clients.cpp
# End Source File
# Begin Source File

SOURCE=.\Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\RvSession.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\X25Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\X25HostConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\X25Socket.cpp
# End Source File
# End Group
# Begin Group "Display Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionList.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Placement.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraNovaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView.cpp
# End Source File
# Begin Source File

SOURCE=.\TrafficDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TrafficView.cpp
# End Source File
# End Group
# Begin Group "Database Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CommonDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecutionMapSet.cpp
# End Source File
# Begin Source File

SOURCE=.\TagMapSet.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ActDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\Actor.rc

!IF  "$(CFG)" == "Actor - Win32 Release"

!ELSEIF  "$(CFG)" == "Actor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConnectionHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\Price.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView2.cpp
# End Source File
# Begin Source File

SOURCE=.\X25ConnectionList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Message Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Csv.h
# End Source File
# Begin Source File

SOURCE=.\Execution.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\Order.h
# End Source File
# End Group
# Begin Group "Act Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Act.h
# End Source File
# Begin Source File

SOURCE=.\ActFields.h
# End Source File
# End Group
# Begin Group "Message Handling Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActSession.h
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.h
# End Source File
# Begin Source File

SOURCE=.\Clients.h
# End Source File
# Begin Source File

SOURCE=.\Connection.h
# End Source File
# Begin Source File

SOURCE=.\ExchangeSession.h
# End Source File
# Begin Source File

SOURCE=.\HostConnection.h
# End Source File
# Begin Source File

SOURCE=.\MessageApp.h
# End Source File
# Begin Source File

SOURCE=.\OrderHandler.h
# End Source File
# Begin Source File

SOURCE=.\Reject.h
# End Source File
# Begin Source File

SOURCE=.\Replace.h
# End Source File
# Begin Source File

SOURCE=.\Request.h
# End Source File
# Begin Source File

SOURCE=.\RvSession.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\StatusHandler.h
# End Source File
# Begin Source File

SOURCE=.\TerraStatus.h
# End Source File
# Begin Source File

SOURCE=.\X25Connection.h
# End Source File
# Begin Source File

SOURCE=.\X25HostConnection.h
# End Source File
# Begin Source File

SOURCE=.\X25Socket.H
# End Source File
# Begin Source File

SOURCE="..\Sangoma X.25\WIN32\XAPI32MC.H"
# End Source File
# Begin Source File

SOURCE="..\Sangoma X.25\WIN32\XAPI_M.H"
# End Source File
# End Group
# Begin Group "Display Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionFrame.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionList.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Placement.h
# End Source File
# Begin Source File

SOURCE=.\TerraNovaDoc.h
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView.h
# End Source File
# Begin Source File

SOURCE=.\TrafficDoc.h
# End Source File
# Begin Source File

SOURCE=.\TrafficView.h
# End Source File
# End Group
# Begin Group "Database Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CommonDatabase.h
# End Source File
# Begin Source File

SOURCE=.\ExecutionMapSet.h
# End Source File
# Begin Source File

SOURCE=.\TagMapSet.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ActDialog.h
# End Source File
# Begin Source File

SOURCE=.\Actor.h
# End Source File
# Begin Source File

SOURCE=.\Price.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TerraNovaView2.h
# End Source File
# Begin Source File

SOURCE=.\X25ConnectionList.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Actor.ico
# End Source File
# Begin Source File

SOURCE=.\res\Actor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ActorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\idr_terr.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_terr.ico
# End Source File
# Begin Source File

SOURCE=.\res\TerraNova.rc2
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
