# Microsoft Developer Studio Generated NMAKE File, Based on ArcaDirect.dsp
!IF "$(CFG)" == ""
CFG=ArcaDirect - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ArcaDirect - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ArcaDirect - Win32 Release" && "$(CFG)" != "ArcaDirect - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ArcaDirect - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : ".\ArcaDirect.exe" "$(OUTDIR)\ArcaDirect.bsc"


CLEAN :
	-@erase "$(INTDIR)\ArcaDirect.obj"
	-@erase "$(INTDIR)\ArcaDirect.res"
	-@erase "$(INTDIR)\ArcaDirect.sbr"
	-@erase "$(INTDIR)\ArcaDirectDoc.obj"
	-@erase "$(INTDIR)\ArcaDirectDoc.sbr"
	-@erase "$(INTDIR)\ArcaDirectHandler.obj"
	-@erase "$(INTDIR)\ArcaDirectHandler.sbr"
	-@erase "$(INTDIR)\ArcaDirectMessage.obj"
	-@erase "$(INTDIR)\ArcaDirectMessage.sbr"
	-@erase "$(INTDIR)\ArcaDirectSequenceSet.obj"
	-@erase "$(INTDIR)\ArcaDirectSequenceSet.sbr"
	-@erase "$(INTDIR)\ArcaDirectSession.obj"
	-@erase "$(INTDIR)\ArcaDirectSession.sbr"
	-@erase "$(INTDIR)\ArcaDirectStatus.obj"
	-@erase "$(INTDIR)\ArcaDirectStatus.sbr"
	-@erase "$(INTDIR)\ArcaDirectView.obj"
	-@erase "$(INTDIR)\ArcaDirectView.sbr"
	-@erase "$(INTDIR)\DirectConnection.obj"
	-@erase "$(INTDIR)\DirectConnection.sbr"
	-@erase "$(INTDIR)\SeqDlg.obj"
	-@erase "$(INTDIR)\SeqDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ArcaDirect.bsc"
	-@erase ".\ArcaDirect.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /I "C:\sybase\OCS-12_5\lib" /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ArcaDirect.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ArcaDirect.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ArcaDirect.sbr" \
	"$(INTDIR)\ArcaDirectDoc.sbr" \
	"$(INTDIR)\ArcaDirectHandler.sbr" \
	"$(INTDIR)\ArcaDirectMessage.sbr" \
	"$(INTDIR)\ArcaDirectSequenceSet.sbr" \
	"$(INTDIR)\ArcaDirectSession.sbr" \
	"$(INTDIR)\ArcaDirectStatus.sbr" \
	"$(INTDIR)\ArcaDirectView.sbr" \
	"$(INTDIR)\DirectConnection.sbr" \
	"$(INTDIR)\SeqDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\ArcaDirect.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=ombase-dynamic-r-can.lib DBOLib.lib libcs.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib OrderTypeValidator.lib DatabaseSettings.lib ExchangeHandlerBase.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ArcaDirect.pdb" /machine:I386 /out:"ArcaDirect.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ArcaDirect.obj" \
	"$(INTDIR)\ArcaDirectDoc.obj" \
	"$(INTDIR)\ArcaDirectHandler.obj" \
	"$(INTDIR)\ArcaDirectMessage.obj" \
	"$(INTDIR)\ArcaDirectSequenceSet.obj" \
	"$(INTDIR)\ArcaDirectSession.obj" \
	"$(INTDIR)\ArcaDirectStatus.obj" \
	"$(INTDIR)\ArcaDirectView.obj" \
	"$(INTDIR)\DirectConnection.obj" \
	"$(INTDIR)\SeqDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ArcaDirect.res"

".\ArcaDirect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ArcaDirect - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "C:\OrderManager\ArcaDirect\ArcaDirect.exe" "$(OUTDIR)\ArcaDirect.bsc"


CLEAN :
	-@erase "$(INTDIR)\ArcaDirect.obj"
	-@erase "$(INTDIR)\ArcaDirect.res"
	-@erase "$(INTDIR)\ArcaDirect.sbr"
	-@erase "$(INTDIR)\ArcaDirectDoc.obj"
	-@erase "$(INTDIR)\ArcaDirectDoc.sbr"
	-@erase "$(INTDIR)\ArcaDirectHandler.obj"
	-@erase "$(INTDIR)\ArcaDirectHandler.sbr"
	-@erase "$(INTDIR)\ArcaDirectMessage.obj"
	-@erase "$(INTDIR)\ArcaDirectMessage.sbr"
	-@erase "$(INTDIR)\ArcaDirectSequenceSet.obj"
	-@erase "$(INTDIR)\ArcaDirectSequenceSet.sbr"
	-@erase "$(INTDIR)\ArcaDirectSession.obj"
	-@erase "$(INTDIR)\ArcaDirectSession.sbr"
	-@erase "$(INTDIR)\ArcaDirectStatus.obj"
	-@erase "$(INTDIR)\ArcaDirectStatus.sbr"
	-@erase "$(INTDIR)\ArcaDirectView.obj"
	-@erase "$(INTDIR)\ArcaDirectView.sbr"
	-@erase "$(INTDIR)\DirectConnection.obj"
	-@erase "$(INTDIR)\DirectConnection.sbr"
	-@erase "$(INTDIR)\SeqDlg.obj"
	-@erase "$(INTDIR)\SeqDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ArcaDirect.bsc"
	-@erase "C:\OrderManager\ArcaDirect\ArcaDirect.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Common" /I "..\..\..\Common\ordermanagerbasecan" /I "..\..\..\COMMON\MemoryMapInitializer" /I "..\..\..\Common\Dbo" /I "..\..\..\Common\DatabaseSettings" /I "..\..\..\Common\OrderIdGenerator" /I "..\..\..\Common\OrderTypeValidator" /I "..\..\..\Common\ExchangeHandlerBase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ArcaDirect.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ArcaDirect.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ArcaDirect.sbr" \
	"$(INTDIR)\ArcaDirectDoc.sbr" \
	"$(INTDIR)\ArcaDirectHandler.sbr" \
	"$(INTDIR)\ArcaDirectMessage.sbr" \
	"$(INTDIR)\ArcaDirectSequenceSet.sbr" \
	"$(INTDIR)\ArcaDirectSession.sbr" \
	"$(INTDIR)\ArcaDirectStatus.sbr" \
	"$(INTDIR)\ArcaDirectView.sbr" \
	"$(INTDIR)\DirectConnection.sbr" \
	"$(INTDIR)\SeqDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\ArcaDirect.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=ombase-dynamic-d-can.lib libcs.lib DboLib.lib libct.lib libsybdb.lib libblk.lib dbolib.lib version.lib threadpool.lib DatabaseSettingsD.lib OrderTypeValidatorD.lib ExchangeHandlerBaseD.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"C:\OrderManager\ArcaDirect\ArcaDirect.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ArcaDirect.obj" \
	"$(INTDIR)\ArcaDirectDoc.obj" \
	"$(INTDIR)\ArcaDirectHandler.obj" \
	"$(INTDIR)\ArcaDirectMessage.obj" \
	"$(INTDIR)\ArcaDirectSequenceSet.obj" \
	"$(INTDIR)\ArcaDirectSession.obj" \
	"$(INTDIR)\ArcaDirectStatus.obj" \
	"$(INTDIR)\ArcaDirectView.obj" \
	"$(INTDIR)\DirectConnection.obj" \
	"$(INTDIR)\SeqDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ArcaDirect.res"

"C:\OrderManager\ArcaDirect\ArcaDirect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ArcaDirect.dep")
!INCLUDE "ArcaDirect.dep"
!ELSE 
!MESSAGE Warning: cannot find "ArcaDirect.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ArcaDirect - Win32 Release" || "$(CFG)" == "ArcaDirect - Win32 Debug"
SOURCE=.\ArcaDirect.cpp

"$(INTDIR)\ArcaDirect.obj"	"$(INTDIR)\ArcaDirect.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirect.rc

"$(INTDIR)\ArcaDirect.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ArcaDirectDoc.cpp

"$(INTDIR)\ArcaDirectDoc.obj"	"$(INTDIR)\ArcaDirectDoc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectHandler.cpp

"$(INTDIR)\ArcaDirectHandler.obj"	"$(INTDIR)\ArcaDirectHandler.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectMessage.cpp

"$(INTDIR)\ArcaDirectMessage.obj"	"$(INTDIR)\ArcaDirectMessage.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectSequenceSet.cpp

"$(INTDIR)\ArcaDirectSequenceSet.obj"	"$(INTDIR)\ArcaDirectSequenceSet.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectSession.cpp

"$(INTDIR)\ArcaDirectSession.obj"	"$(INTDIR)\ArcaDirectSession.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectStatus.cpp

"$(INTDIR)\ArcaDirectStatus.obj"	"$(INTDIR)\ArcaDirectStatus.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcaDirectView.cpp

"$(INTDIR)\ArcaDirectView.obj"	"$(INTDIR)\ArcaDirectView.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DirectConnection.cpp

"$(INTDIR)\DirectConnection.obj"	"$(INTDIR)\DirectConnection.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SeqDlg.cpp

"$(INTDIR)\SeqDlg.obj"	"$(INTDIR)\SeqDlg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

