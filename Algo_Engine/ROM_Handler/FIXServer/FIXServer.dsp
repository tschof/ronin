# Microsoft Developer Studio Project File - Name="FIXServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=FIXServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FIXServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FIXServer.mak" CFG="FIXServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FIXServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "FIXServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OrderExecutions/SERVERS/FIXServer", LAXCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FIXServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /I "..\..\Common\dbo" /I "..\..\Common\QuickFix" /I "..\..\Common\QuickFix\Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Z<none> /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 quickfix.lib ws2_32.lib kernel32.lib dboLib.lib Rpcrt4.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../bin/FIXServer.exe" /libpath:"..\..\bin"

!ELSEIF  "$(CFG)" == "FIXServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\Common\dbo" /I "..\..\Common\QuickFix" /I "..\..\Common\QuickFix\Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 quickfix_Debug.lib ws2_32.lib kernel32.lib dboLib.lib Rpcrt4.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../bin/FIXServer.exe" /pdbtype:sept /libpath:"..\..\bin"

!ENDIF 

# Begin Target

# Name "FIXServer - Win32 Release"
# Name "FIXServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Account.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountID.cpp
# End Source File
# Begin Source File

SOURCE=.\FIXServer.cpp
# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\OMAcceptor.cpp
# End Source File
# Begin Source File

SOURCE=.\OMInitiator.cpp
# End Source File
# Begin Source File

SOURCE=.\OMMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\OMMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\OMParser.cpp
# End Source File
# Begin Source File

SOURCE=.\OMSession.cpp
# End Source File
# Begin Source File

SOURCE=.\OMSocketAcceptor.cpp
# End Source File
# Begin Source File

SOURCE=.\OMSocketConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\OMSocketInitiator.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\TranslationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TranslationMessages.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Account.h
# End Source File
# Begin Source File

SOURCE=.\AccountID.h
# End Source File
# Begin Source File

SOURCE=.\FIXServer.h
# End Source File
# Begin Source File

SOURCE=".\getopt-repl.h"
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\OMAcceptor.h
# End Source File
# Begin Source File

SOURCE=.\OMApplication.h
# End Source File
# Begin Source File

SOURCE=.\OMInitiator.h
# End Source File
# Begin Source File

SOURCE=.\OMMessage.h
# End Source File
# Begin Source File

SOURCE=.\OMMessageCracker.h
# End Source File
# Begin Source File

SOURCE=.\OMMessages.h
# End Source File
# Begin Source File

SOURCE=.\OMParser.h
# End Source File
# Begin Source File

SOURCE=.\OMSession.h
# End Source File
# Begin Source File

SOURCE=.\OMSocketAcceptor.h
# End Source File
# Begin Source File

SOURCE=.\OMSocketConnection.h
# End Source File
# Begin Source File

SOURCE=.\OMSocketInitiator.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\TranslationManager.h
# End Source File
# Begin Source File

SOURCE=.\TranslationMessages.h
# End Source File
# Begin Source File

SOURCE=.\UserDefinedFields.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Test Files"

# PROP Default_Filter ".h,.cpp"
# End Group
# Begin Source File

SOURCE=.\OMSchema.xml
# End Source File
# Begin Source File

SOURCE=.\xml2cpp.xsl

!IF  "$(CFG)" == "FIXServer - Win32 Release"

!ELSEIF  "$(CFG)" == "FIXServer - Win32 Debug"

USERDEP__XML2C="OMSchema.xml"	
# Begin Custom Build
InputPath=.\xml2cpp.xsl

"OMMessages.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	msxsl omschema.xml xml2cpp.xsl -o OMMessages.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xml2h.xsl

!IF  "$(CFG)" == "FIXServer - Win32 Release"

# Begin Custom Build
InputPath=.\xml2h.xsl

"OMMessages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	msxsl omschema.xml xml2h.xsl -o OMMessages.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FIXServer - Win32 Debug"

USERDEP__XML2H="OMSchema.xml"	
# Begin Custom Build
InputPath=.\xml2h.xsl

"OMMessages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	msxsl omschema.xml xml2h.xsl -o OMMessages.h

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
