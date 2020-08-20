; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDocument
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TerraNova.h"
LastPage=0

ClassCount=8
Class1=CTerraNovaApp
Class2=CTerraNovaDoc
Class3=CTerraNovaView
Class4=CMainFrame

ResourceCount=7
Resource1=IDR_MAINFRAME
Resource2=IDD_SEQDLG
Resource3=IDR_TRAFFICTYPE
Class5=CAboutDlg
Class6=CChildFrame
Resource4=IDD_TERRANOVA_FORM
Class7=CTrafficView
Resource5=IDR_TERRATYPE
Class8=CTrafficDoc
Resource6=IDD_LISTVIEW
Resource7=IDD_ABOUTBOX

[CLS:CTerraNovaApp]
Type=0
HeaderFile=TerraNova.h
ImplementationFile=TerraNova.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CTerraNovaApp

[CLS:CTerraNovaDoc]
Type=0
HeaderFile=TerraNovaDoc.h
ImplementationFile=TerraNovaDoc.cpp
Filter=N

[CLS:CTerraNovaView]
Type=0
HeaderFile=TerraNovaView.h
ImplementationFile=TerraNovaView.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CTerraNovaView

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
HeaderFile=TerraNova.cpp
ImplementationFile=TerraNova.cpp
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

[MNU:IDR_TERRATYPE]
Type=1
Class=CTerraNovaView
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
Command12=IDM_RESET_INCOMING
Command13=IDM_RESET_OUTGOING
Command14=IDM_CLEAR_INCOMING
Command15=IDM_CLEAR_OUTGOING
Command16=ID_WINDOW_NEW
Command17=ID_WINDOW_CASCADE
Command18=ID_WINDOW_TILE_HORZ
Command19=ID_WINDOW_ARRANGE
Command20=ID_APP_ABOUT
CommandCount=20

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

[CLS:CTrafficView]
Type=0
HeaderFile=TrafficView.h
ImplementationFile=TrafficView.cpp
BaseClass=CFormView
Filter=D
LastObject=CTrafficView
VirtualFilter=VWC

[CLS:CTrafficDoc]
Type=0
HeaderFile=TrafficDoc.h
ImplementationFile=TrafficDoc.cpp
BaseClass=CDocument
Filter=N
LastObject=CTrafficDoc
VirtualFilter=DC

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
Command11=IDM_LOGOUT
Command12=IDM_RESET_INCOMING
Command13=IDM_RESET_OUTGOING
Command14=IDM_CLEAR_INCOMING
Command15=IDM_CLEAR_OUTGOING
Command16=ID_WINDOW_NEW
Command17=ID_WINDOW_CASCADE
Command18=ID_WINDOW_TILE_HORZ
Command19=ID_WINDOW_ARRANGE
Command20=ID_APP_ABOUT
CommandCount=20

[DLG:IDD_SEQDLG]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SEQNUM,edit,1350631552

