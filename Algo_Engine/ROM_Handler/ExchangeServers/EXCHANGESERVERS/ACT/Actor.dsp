# Microsoft Developer Studio Project File - Name="Actor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
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
# PROP AllowPerConfigDependencies 0
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
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\Common\ordermanagerbase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ombase-dynamic-r.lib librvcpp.lib rvwin.lib libtib.lib rv.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Common\ordermanagerbase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ombase-dynamic-d.lib librvcpp.lib rvwin.lib libtib.lib rv.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcmt.lib" /out:"../../../Bin/Actor.exe" /pdbtype:sept /libpath:"..\..\..\Bin"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Actor - Win32 Release"
# Name "Actor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Message Source"

# PROP Default_Filter ""
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
# Begin Group "Database Source"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\ActDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ActHost.cpp
# End Source File
# Begin Source File

SOURCE=.\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\Actor.rc
# End Source File
# Begin Source File

SOURCE=.\ActorConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\Msg.cpp
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
# End Group
# Begin Group "Act Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Act.h
# End Source File
# End Group
# Begin Group "Message Handling Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActSession.h
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
# Begin Group "Database Headers"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\ActDialog.h
# End Source File
# Begin Source File

SOURCE=.\ActHost.h
# End Source File
# Begin Source File

SOURCE=.\Actor.h
# End Source File
# Begin Source File

SOURCE=.\ActorConnection.h
# End Source File
# Begin Source File

SOURCE=..\ARCADropCopy\ARCADropCopyFixSession.h
# End Source File
# Begin Source File

SOURCE=.\Msg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
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
