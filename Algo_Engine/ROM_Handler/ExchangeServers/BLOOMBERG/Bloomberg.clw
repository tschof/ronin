; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Bloomberg.h"
LastPage=0

ClassCount=7
Class1=CBloombergApp
Class2=CBloombergDoc
Class3=CBloombergView
Class4=CMainFrame

ResourceCount=7
Resource1=IDC_BLOOMBERG_FORM
Resource2=IDR_TRAFFICTYPE
Resource3=IDD_LISTVIEW
Class5=CChildFrame
Class6=CAboutDlg
Resource4=IDR_BLOOMBTYPE
Resource5=IDR_MAINFRAME
Resource6=IDD_ABOUTBOX
Class7=CSeqDlg
Resource7=IDD_SEQDLG

[CLS:CBloombergApp]
Type=0
HeaderFile=Bloomberg.h
ImplementationFile=Bloomberg.cpp
Filter=N
LastObject=CBloombergApp

[CLS:CBloombergDoc]
Type=0
HeaderFile=BloombergDoc.h
ImplementationFile=BloombergDoc.cpp
Filter=N
LastObject=CBloombergDoc

[CLS:CBloombergView]
Type=0
HeaderFile=BloombergView.h
ImplementationFile=BloombergView.cpp
Filter=D
LastObject=CBloombergView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M


[CLS:CAboutDlg]
Type=0
HeaderFile=Bloomberg.cpp
ImplementationFile=Bloomberg.cpp
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
Command1=ID_APP_EXIT
Command2=ID_APP_ABOUT
CommandCount=2

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_BLOOMBTYPE]
Type=1
Class=CBloombergView
Command1=ID_APP_EXIT
Command2=IDM_LOGOUT
Command3=IDM_RESET_INCOMING
Command4=IDM_RESET_OUTGOING
Command5=IDM_CLEAR_INCOMING
Command6=IDM_CLEAR_OUTGOING
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[MNU:IDR_TRAFFICTYPE]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=IDM_LOGOUT
Command3=IDM_RESET_INCOMING
Command4=IDM_RESET_OUTGOING
Command5=IDM_CLEAR_INCOMING
Command6=IDM_CLEAR_OUTGOING
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
CommandCount=11

[DLG:IDC_BLOOMBERG_FORM]
Type=1
Class=CBloombergView
ControlCount=1
Control1=IDC_CONNECTION_LIST,listbox,1352728849

[DLG:IDD_LISTVIEW]
Type=1
Class=?
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

[DLG:IDD_SEQDLG]
Type=1
Class=CSeqDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SEQNUM,edit,1350639744

[CLS:CSeqDlg]
Type=0
HeaderFile=SeqDlg.h
ImplementationFile=SeqDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDCANCEL

