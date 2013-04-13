# Microsoft Developer Studio Project File - Name="Nabul" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Nabul - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Nabul.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Nabul.mak" CFG="Nabul - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Nabul - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Nabul - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Nabul - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Nabul - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Nabul - Win32 Release"
# Name "Nabul - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Book.cpp
# End Source File
# Begin Source File

SOURCE=.\end.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Midi.cpp
# End Source File
# Begin Source File

SOURCE=.\Nabul.cpp
# End Source File
# Begin Source File

SOURCE=.\Nabul.rc
# End Source File
# Begin Source File

SOURCE=.\NabulDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Open.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Book.h
# End Source File
# Begin Source File

SOURCE=.\end.h
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\Midi.h
# End Source File
# Begin Source File

SOURCE=.\Nabul.h
# End Source File
# Begin Source File

SOURCE=.\NabulDlg.h
# End Source File
# Begin Source File

SOURCE=.\Open.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\res\end.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ending1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ending2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ending3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ending4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ending5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Nabul.ico
# End Source File
# Begin Source File

SOURCE=.\res\Nabul.rc2
# End Source File
# Begin Source File

SOURCE=.\res\open.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shop2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shop3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage32.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage42.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage62.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage63.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage65.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage66.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage67.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage68.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage69.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stage7.bmp
# End Source File
# Begin Source File

SOURCE=".\res\»ç¸·.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\½£2.bmp"
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\ak.wav
# End Source File
# Begin Source File

SOURCE=.\res\beauty.wav
# End Source File
# Begin Source File

SOURCE=.\res\boss.wav
# End Source File
# Begin Source File

SOURCE=.\res\Cold1.wav
# End Source File
# Begin Source File

SOURCE=.\res\Flame1.wav
# End Source File
# Begin Source File

SOURCE=.\res\Item1.wav
# End Source File
# Begin Source File

SOURCE=.\res\Kill11.wav
# End Source File
# Begin Source File

SOURCE=.\res\Monster2.wav
# End Source File
# Begin Source File

SOURCE=.\res\Power.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\Recovery3.wav
# End Source File
# Begin Source File

SOURCE=.\res\Sample1.wav
# End Source File
# Begin Source File

SOURCE=.\res\Shot2.wav
# End Source File
# Begin Source File

SOURCE=.\res\Success2.wav
# End Source File
# End Target
# End Project
