; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CArcaDirectApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ArcaDirect.h"
LastPage=0

ClassCount=6
Class1=CArcaDirectApp
Class2=CArcaDirectDoc
Class3=CArcaDirectView
Class4=CMainFrame

ResourceCount=12
Resource1=IDD_BRUT_FORM
Resource2=IDR_BRUTTYPE
Resource3=IDR_ARCADITYPE
Resource8=IDD_SEQDLG
Resource9=IDR_MAINFRAME
Resource10=IDD_LISTVIEW
Resource11=IDD_ABOUTBOX
Class5=CChildFrame
Class6=CAboutDlg
Resource12=IDR_TRAFFICTYPE

[CLS:CArcaDirectApp]
Type=0
HeaderFile=ArcaDirect.h
ImplementationFile=ArcaDirect.cpp
Filter=N
LastObject=IDM_RESET_ORDER_ID

[CLS:CArcaDirectDoc]
Type=0
HeaderFile=ArcaDirectDoc.h
ImplementationFile=ArcaDirectDoc.cpp
Filter=N
LastObject=CArcaDirectDoc

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
HeaderFile=ArcaDirect.cpp
ImplementationFile=ArcaDirect.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342373889
Control3=IDC_VERSION,edit,1342244992
Control4=IDC_COPYRIGHT,edit,1342244992

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_APP_EXIT
Command4=ID_APP_ABOUT
CommandCount=4

[MNU:IDR_ARCADITYPE]
Type=1
Class=CArcaDirectView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_VIEW_STATUS_BAR
Command13=ID_WINDOW_NEW
Command14=ID_WINDOW_CASCADE
CommandCount=17
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_UNDO
Command15=ID_WINDOW_TILE_HORZ
Command16=ID_WINDOW_ARRANGE
Command17=ID_APP_ABOUT

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

[MNU:IDR_BRUTTYPE]
Type=1
Class=CArcaDirectApp
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
Command13=IDM_RESET_ORDER_ID
Command14=IDM_RESET_OUTGOING
Command15=IDM_CLEAR_INCOMING
Command16=IDM_CLEAR_OUTGOING
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

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
Command13=IDM_RESET_ORDER_ID
Command14=IDM_RESET_OUTGOING
Command15=IDM_CLEAR_INCOMING
Command16=IDM_CLEAR_OUTGOING
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

[DLG:IDD_BRUT_FORM]
Type=1
Class=CArcaDirectView
ControlCount=1
Control1=IDC_CONNECTION_LIST,listbox,1352663313

[DLG:IDD_LISTVIEW]
Type=1
Class=?
ControlCount=1
Control1=IDC_TRAFFIC_LIST,listbox,1352728961

[DLG:IDD_SEQDLG]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SEQNUM,edit,1350631552

[CLS:CArcaDirectView]
Type=0
HeaderFile=arcadirectview.h
ImplementationFile=arcadirectview.cpp
BaseClass=CFormView
Filter=D
LastObject=IDC_CONNECTION_LIST

