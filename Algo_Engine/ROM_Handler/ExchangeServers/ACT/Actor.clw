; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CActorConnection
LastTemplate=CAsyncSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "actor.h"
LastPage=0

ClassCount=21
Class1=CTerraNovaApp
Class2=CAboutDlg
Class3=CActorDoc
Class4=CTerraNovaView
Class5=CChildFrame
Class6=CClientConnection
Class7=CConnection
Class8=CConnectionFrame
Class9=CConnectionList
Class10=CExecutionMapSet
Class11=CIncomingMessageSet
Class12=CMainFrame
Class13=COutgoingMessageSet
Class14=CServer
Class15=CTagMapSet
Class16=CTerraNovaDoc
Class17=CTerraNovaView2
Class18=CTrafficDoc
Class19=CTrafficView

ResourceCount=7
Resource1=IDD_LISTVIEW
Resource2=IDR_TRAFFICTYPE
Resource3=IDR_TERRATYPE
Resource4=IDD_TERRANOVA_FORM
Resource5=IDR_MAINFRAME
Resource6=IDD_ABOUTBOX
Class20=CActDialog
Class21=CActorConnection
Resource7=IDD_ACTDIALOG

[CLS:CTerraNovaApp]
Type=0
BaseClass=CWinApp
HeaderFile=Actor.h
ImplementationFile=Actor.cpp
LastObject=CTerraNovaApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Actor.cpp
ImplementationFile=Actor.cpp
LastObject=CAboutDlg

[CLS:CActorDoc]
Type=0
BaseClass=CDocument
HeaderFile=ActorDoc.h
ImplementationFile=ActorDoc.cpp

[CLS:CTerraNovaView]
Type=0
BaseClass=CFormView
HeaderFile=ActorView.h
ImplementationFile=ActorView.cpp
LastObject=CTerraNovaView

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CClientConnection]
Type=0
BaseClass=CAsyncSocket
HeaderFile=ClientConnection.h
ImplementationFile=ClientConnection.cpp

[CLS:CConnection]
Type=0
BaseClass=CAsyncSocket
HeaderFile=Connection.h
ImplementationFile=Connection.cpp

[CLS:CConnectionFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ConnectionFrame.h
ImplementationFile=ConnectionFrame.cpp

[CLS:CConnectionList]
Type=0
BaseClass=CListBox
HeaderFile=ConnectionList.h
ImplementationFile=ConnectionList.cpp

[CLS:CExecutionMapSet]
Type=0
HeaderFile=ExecutionMapSet.h
ImplementationFile=ExecutionMapSet.cpp

[CLS:CIncomingMessageSet]
Type=0
HeaderFile=IncomingMessageSet.h
ImplementationFile=IncomingMessageSet.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=ID_ACT_MESSAGE

[CLS:COutgoingMessageSet]
Type=0
HeaderFile=OutgoingMessageSet.h
ImplementationFile=OutgoingMessageSet.cpp

[CLS:CServer]
Type=0
BaseClass=CAsyncSocket
HeaderFile=Server.h
ImplementationFile=Server.cpp

[CLS:CTagMapSet]
Type=0
HeaderFile=TagMapSet.h
ImplementationFile=TagMapSet.cpp

[CLS:CTerraNovaDoc]
Type=0
BaseClass=CDocument
HeaderFile=TerraNovaDoc.h
ImplementationFile=TerraNovaDoc.cpp

[CLS:CTerraNovaView2]
Type=0
BaseClass=CFormView
HeaderFile=TerraNovaView2.h
ImplementationFile=TerraNovaView2.cpp

[CLS:CTrafficDoc]
Type=0
BaseClass=CDocument
HeaderFile=TrafficDoc.h
ImplementationFile=TrafficDoc.cpp

[CLS:CTrafficView]
Type=0
BaseClass=CFormView
HeaderFile=TrafficView.h
ImplementationFile=TrafficView.cpp

[DB:CExecutionMapSet]
DB=1

[DB:CIncomingMessageSet]
DB=1

[DB:COutgoingMessageSet]
DB=1

[DB:CTagMapSet]
DB=1

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TERRANOVA_FORM]
Type=1
Class=CTerraNovaView
ControlCount=1
Control1=IDC_CONNECTION_LIST,listbox,1352663313

[DLG:IDD_LISTVIEW]
Type=1
Class=CTrafficView
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_APP_EXIT
Command4=ID_APP_ABOUT
CommandCount=4

[MNU:IDR_TERRATYPE]
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
Command11=IDM_LOGOUT
Command12=ID_WINDOW_NEW
Command13=ID_WINDOW_CASCADE
Command14=ID_WINDOW_TILE_HORZ
Command15=ID_WINDOW_ARRANGE
Command16=ID_APP_ABOUT
CommandCount=16

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

[ACL:IDR_MAINFRAME]
Type=1
Class=?
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

[DLG:IDD_ACTDIALOG]
Type=1
Class=CActDialog
ControlCount=47
Control1=IDC_COMBO_FUNCTION,combobox,1344339970
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COMBO_ASOF,combobox,1344339970
Control5=IDC_STATIC,static,1342308352
Control6=IDC_COMBO_SECCLS,combobox,1344339970
Control7=IDC_STATIC,static,1342308352
Control8=IDC_COMBO_BSX,combobox,1344339970
Control9=IDC_EDIT_REFNUM,edit,1350639744
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_VOLUME,edit,1350639744
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_EDIT_PRICEDOLLAR,edit,1350639752
Control16=IDC_EDIT_PRICENUM,edit,1350639752
Control17=IDC_COMBO_TRADEMOD,combobox,1344339970
Control18=IDC_STATIC,static,1342308352
Control19=IDC_PRICEOVERIDE,button,1342242819
Control20=IDC_STATIC,static,1342308352
Control21=IDC_EDIT_OEID,edit,1350631560
Control22=IDC_STATIC,static,1342308352
Control23=IDC_EDIT_OEGU,edit,1350631560
Control24=IDC_STATIC,static,1342308352
Control25=IDC_EDIT_OECLEARNUM,edit,1350639752
Control26=IDC_STATIC,static,1342308352
Control27=IDC_EDIT_MMID,edit,1350631560
Control28=IDC_STATIC,static,1342308352
Control29=IDC_EDIT_MMGU,edit,1350631560
Control30=IDC_STATIC,static,1342308352
Control31=IDC_EDIT_MMCLEARNUM,edit,1350639752
Control32=IDC_STATIC,static,1342308352
Control33=IDC_COMBO_MMPA,combobox,1344339970
Control34=IDC_STATIC,static,1342308352
Control35=IDC_COMBO_SPECIALINDICATOR,combobox,1344339970
Control36=IDC_STATIC,static,1342308352
Control37=IDC_EDIT_MEMO,edit,1350631560
Control38=IDC_STATIC,static,1342308352
Control39=IDC_EDIT_TRADEPRICEEXP,edit,1350631560
Control40=IDC_COMBO_DEN,combobox,1344339970
Control41=IDC_REPORTFLAG,button,1342243363
Control42=IDC_EDIT_TRADEPRICEEXP2,edit,1350631560
Control43=IDC_COMBO_CLEARFLAG,combobox,1344339970
Control44=IDC_STATIC,static,1342308352
Control45=IDC_SENDACT,button,1342242816
Control46=IDC_EDIT_NUMSEND,edit,1350631552
Control47=IDC_SYMBOL_EDIT,edit,1350631552

[CLS:CActDialog]
Type=0
HeaderFile=ActDialog.h
ImplementationFile=ActDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CActDialog
VirtualFilter=dWC

[CLS:CActorConnection]
Type=0
HeaderFile=ActorConnection.h
ImplementationFile=ActorConnection.cpp
BaseClass=CAsyncSocket
Filter=N
LastObject=CActorConnection

