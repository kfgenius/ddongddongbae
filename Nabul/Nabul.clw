; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Nabul.h"

ClassCount=9
Class1=CNabulApp
Class2=CNabulDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_NABUL_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CGame
Resource4=IDD_BOOK
Class5=CBook
Class6=CGameOver
Resource5=IDD_GAME
Class7=Cend
Class8=COpen
Class9=COpen1
Resource6=IDD_OPEN
Resource7=IDD_END

[CLS:CNabulApp]
Type=0
HeaderFile=Nabul.h
ImplementationFile=Nabul.cpp
Filter=N
LastObject=CNabulApp

[CLS:CNabulDlg]
Type=0
HeaderFile=NabulDlg.h
ImplementationFile=NabulDlg.cpp
Filter=D
LastObject=CNabulDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=NabulDlg.h
ImplementationFile=NabulDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_NABUL_DIALOG]
Type=1
Class=CNabulDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177294
Control2=IDC_GAME,button,1342242816
Control3=IDC_BOOK,button,1342242816
Control4=IDC_OK,button,1342242816

[DLG:IDD_GAME]
Type=1
Class=CGame
ControlCount=25
Control1=IDC_TYPE,edit,1350635588
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,button,1342177287
Control13=IDC_ITEM1,static,1342308352
Control14=IDC_ITEM2,static,1342308352
Control15=IDC_ROD,static,1342308352
Control16=IDC_ROBE,static,1342308352
Control17=IDC_CURE,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342177294
Control24=IDC_STATIC,static,1342177294
Control25=IDC_STATIC,static,1342177294

[CLS:CGame]
Type=0
HeaderFile=Game.h
ImplementationFile=Game.cpp
BaseClass=CDialog
Filter=D
LastObject=CGame
VirtualFilter=dWC

[DLG:IDD_BOOK]
Type=1
Class=CBook
ControlCount=6
Control1=IDC_TEXT,static,1342308352
Control2=IDC_TYPE,edit,1342246980
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_PAGE,static,1342308354

[CLS:CBook]
Type=0
HeaderFile=Book.h
ImplementationFile=Book.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_TYPE

[CLS:CGameOver]
Type=0
HeaderFile=GameOver.h
ImplementationFile=GameOver.cpp
BaseClass=CDialog
Filter=D
LastObject=CGameOver
VirtualFilter=dWC

[CLS:Cend]
Type=0
HeaderFile=end.h
ImplementationFile=end.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=Cend

[CLS:COpen]
Type=0
HeaderFile=Open.h
ImplementationFile=Open.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COpen

[DLG:IDD_OPEN]
Type=1
Class=COpen1
ControlCount=1
Control1=IDC_TEXT,static,1342308352

[CLS:COpen1]
Type=0
HeaderFile=Open1.h
ImplementationFile=Open1.cpp
BaseClass=CDialog
Filter=D
LastObject=COpen1

[DLG:IDD_END]
Type=1
Class=Cend
ControlCount=1
Control1=IDC_TEXT,static,1342308352

