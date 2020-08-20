; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CISEApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ISE.h"
LastPage=0

ClassCount=6
Class1=CISEApp
Class2=CISEDoc
Class3=CISEView
Class4=CMainFrame

ResourceCount=7
Resource1=IDR_MAINFRAME
Resource2=IDD_LISTVIEW
Resource3=IDR_TRAFFICTYPE
Resource4=IDR_ISETYPE
Resource5=IDD_ABOUTBOX
Resource6=IDD_SEQDLG
Class5=CChildFrame
Class6=CAboutDlg
Resource7=IDD_ISE_FORM

[CLS:CISEApp]
Type=0
HeaderFile=ISE.h
ImplementationFile=ISE.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CISEDoc]
Type=0
HeaderFile=ISEDoc.h
ImplementationFile=ISEDoc.cpp
Filter=N

[CLS:CISEView]
Type=0
HeaderFile=ISEView.h
ImplementationFile=ISEView.cpp
Filter=C
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CISEView


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
HeaderFile=ISE.cpp
ImplementationFile=ISE.cpp
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

[MNU:IDR_ISETYPE]
Type=1
Class=CISEView
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
Class=CISEApp
Command1=ID_APP_EXIT
Command2=IDM_LOGOUT
Command3=IDM_RESET_INCOMING
Command4=IDM_RESET_OUTGOING
Command5=IDM_CLEAR_INCOMING
Command6=IDM_CLEAR_OUTGOING
Command7=IDM_TAGGLE_DISPLAY
Command8=ID_WINDOW_NEW
Command9=ID_WINDOW_CASCADE
Command10=ID_WINDOW_TILE_HORZ
Command11=ID_WINDOW_ARRANGE
Command12=ID_APP_ABOUT
CommandCount=12

[DLG:IDD_SEQDLG]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SEQNUM,edit,1350631552

[DLG:IDD_LISTVIEW]
Type=1
Class=?
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

[DLG:IDD_ISE_FORM]
Type=1
Class=?
ControlCount=1
Control1=IDC_CONNECTION_LIST,listbox,1352663313

