; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CIslandApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Island.h"
LastPage=0

ClassCount=6
Class1=CIslandApp
Class2=CIslandDoc
Class3=CIslandView
Class4=CMainFrame

ResourceCount=7
Resource1=IDD_ISLAND_FORM
Resource2=IDD_ABOUTBOX
Resource3=IDR_MAINFRAME
Resource4=IDD_LISTVIEW
Resource5=IDR_TRAFFICTYPE
Class5=CAboutDlg
Class6=CChildFrame
Resource6=IDD_SEQDLG
Resource7=IDR_ISLANDTYPE

[CLS:CIslandApp]
Type=0
HeaderFile=Island.h
ImplementationFile=Island.cpp
Filter=N
LastObject=CIslandApp
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CIslandDoc]
Type=0
HeaderFile=IslandDoc.h
ImplementationFile=IslandDoc.cpp
Filter=N

[CLS:CIslandView]
Type=0
HeaderFile=IslandView.h
ImplementationFile=IslandView.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CIslandView

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
HeaderFile=Island.cpp
ImplementationFile=Island.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

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

[DLG:IDD_ISLAND_FORM]
Type=1
Class=CIslandView
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

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_APP_ABOUT
CommandCount=2

[MNU:IDR_ISLANDTYPE]
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

[DLG:IDD_SEQDLG]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SEQNUM,edit,1350639744

