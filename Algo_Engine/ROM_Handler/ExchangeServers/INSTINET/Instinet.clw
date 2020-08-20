; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CInstinetView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Instinet.h"
LastPage=0

ClassCount=6
Class1=CInstinetApp
Class2=CInstinetDoc
Class3=CInstinetView
Class4=CMainFrame

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_INSTTYPE
Resource3=IDD_LISTVIEW
Class5=CAboutDlg
Class6=CChildFrame
Resource4=IDR_MAINFRAME
Resource5=IDR_TRAFFICTYPE
Resource6=IDD_INSTINET_FORM

[CLS:CInstinetApp]
Type=0
HeaderFile=Instinet.h
ImplementationFile=Instinet.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=IDM_LOGOUT

[CLS:CInstinetDoc]
Type=0
HeaderFile=InstinetDoc.h
ImplementationFile=InstinetDoc.cpp
Filter=N

[CLS:CInstinetView]
Type=0
HeaderFile=InstinetView.h
ImplementationFile=InstinetView.cpp
Filter=D
LastObject=CInstinetView
BaseClass=CFormView
VirtualFilter=VWC

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
HeaderFile=Instinet.cpp
ImplementationFile=Instinet.cpp
Filter=D

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

[MNU:IDR_INSTTYPE]
Type=1
Class=CInstinetView
Command1=ID_APP_EXIT
Command2=IDM_LOGOUT
Command3=ID_WINDOW_NEW
Command4=ID_WINDOW_CASCADE
Command5=ID_WINDOW_TILE_HORZ
Command6=ID_WINDOW_ARRANGE
Command7=ID_APP_ABOUT
CommandCount=7

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

[DLG:IDD_INSTINET_FORM]
Type=1
Class=CInstinetView
ControlCount=1
Control1=IDC_CONNECTION_LIST,listbox,1352728849

[MNU:IDR_TRAFFICTYPE]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=IDM_LOGOUT
Command3=ID_WINDOW_NEW
Command4=ID_WINDOW_CASCADE
Command5=ID_WINDOW_TILE_HORZ
Command6=ID_WINDOW_ARRANGE
Command7=ID_APP_ABOUT
CommandCount=7

[DLG:IDD_LISTVIEW]
Type=1
Class=?
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

