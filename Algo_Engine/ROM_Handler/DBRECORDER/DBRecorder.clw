; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRecorderThread
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DBRecorder.h"
LastPage=0

ClassCount=10
Class1=CRecorderApp
Class2=CRecorderDoc
Class3=CRecorderView
Class4=CMainFrame

ResourceCount=6
Resource1=IDR_MAINFRAME
Resource2=IDR_TRAFFICTYPE
Resource3=IDR_DBRECOTYPE
Class5=CAboutDlg
Class6=CChildFrame
Resource4=IDD_ABOUTBOX
Resource5=IDD_DBRECORDER_FORM
Class7=CServer
Class8=CClientConnection
Class9=COrderSet
Class10=CRecorderThread
Resource6=IDD_LISTVIEW

[CLS:CRecorderApp]
Type=0
HeaderFile=DBRecorder.h
ImplementationFile=DBRecorder.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CRecorderApp

[CLS:CRecorderDoc]
Type=0
HeaderFile=RecorderDoc.h
ImplementationFile=RecorderDoc.cpp
Filter=N

[CLS:CRecorderView]
Type=0
HeaderFile=RecorderView.h
ImplementationFile=RecorderView.cpp
Filter=D
LastObject=CRecorderView
BaseClass=CFormView
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
BaseClass=CMDIChildWnd
VirtualFilter=mfWC
LastObject=CChildFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=DBRecorder.cpp
ImplementationFile=DBRecorder.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_APP_EXIT
Command4=ID_APP_ABOUT
CommandCount=4

[MNU:IDR_DBRECOTYPE]
Type=1
Class=CRecorderView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
Command15=ID_APP_ABOUT
CommandCount=15

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_DBRECORDER_FORM]
Type=1
Class=CRecorderView
ControlCount=1
Control1=IDC_LIST,listbox,1352728849

[DLG:IDD_LISTVIEW]
Type=1
Class=?
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

[MNU:IDR_TRAFFICTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
Command15=ID_APP_ABOUT
CommandCount=15

[CLS:CServer]
Type=0
HeaderFile=Server.h
ImplementationFile=Server.cpp
BaseClass=CAsyncSocket
Filter=N
VirtualFilter=q
LastObject=CServer

[CLS:CClientConnection]
Type=0
HeaderFile=ClientConnection.h
ImplementationFile=ClientConnection.cpp
BaseClass=CAsyncSocket
Filter=N

[DB:COrderSet]
DB=1
DBType=ODBC
ColumnCount=31
Column1=[Exchange], 5, 2
Column2=[Status], 5, 2
Column3=[Type], 5, 2
Column4=[Capacity], 5, 2
Column5=[TimeInForce], 5, 2
Column6=[Side], 5, 2
Column7=[ClearingAccount], 12, 36
Column8=[ClearingId], 12, 12
Column9=[LocalAccount], 12, 36
Column10=[Firm], 12, 36
Column11=[Instructions], 12, 36
Column12=[Symbol], 12, 36
Column13=[Trader], 12, 36
Column14=[Price], 6, 8
Column15=[Owner], 12, 36
Column16=[SourceId], 12, 36
Column17=[Floor], 4, 4
Column18=[MinQty], 4, 4
Column19=[PriceCheck], -6, 1
Column20=[Shares], 4, 4
Column21=[OriginalShares], 4, 4
Column22=[ReplacementPrice], 6, 8
Column23=[PendingReplace], -6, 1
Column24=[StopPrice], 6, 8
Column25=[Discretionary], -6, 1
Column26=[Solicited], -6, 1
Column27=[OMTag], 12, 36
Column28=[ClientTag], 12, 36
Column29=[TimeStamp], 11, 16
Column30=[Client], 12, 36
Column31=[TradeFor], 12, 36

[CLS:COrderSet]
Type=0
HeaderFile=OrderSet.h
ImplementationFile=OrderSet.cpp
BaseClass=CRecordset
Filter=N
VirtualFilter=r
LastObject=COrderSet

[CLS:CRecorderThread]
Type=0
HeaderFile=RecorderThread.h
ImplementationFile=RecorderThread.cpp
BaseClass=CWinThread
Filter=N
LastObject=CRecorderThread

