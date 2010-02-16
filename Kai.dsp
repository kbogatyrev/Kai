# Microsoft Developer Studio Project File - Name="Kai" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Kai - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Kai.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Kai.mak" CFG="Kai - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Kai - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Kai - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Kai - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"KaiStdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /version:1.2 /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Kai - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "X:\Debug"
# PROP Intermediate_Dir "X:\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"KaiStdAfx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /version:1.2 /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"C:\Dev\X\Debug"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "Kai - Win32 Release"
# Name "Kai - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Kai.cpp
# End Source File
# Begin Source File

SOURCE=.\Kai.rc
# End Source File
# Begin Source File

SOURCE=.\kai_md5.c
# End Source File
# Begin Source File

SOURCE=.\KaiAppDescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiCompound.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiDocDescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiException.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiFontDescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiGeneric.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiLineDescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiMainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiParagraph.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiScreenItemDescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiStdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiString.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiTextPos.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiView.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiViewDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiViewHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiViewKeyDn.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiViewSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\KaiWord.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\Program Files\Microsoft Visual Studio\Vc98\Include\Basetsd.h"
# End Source File
# Begin Source File

SOURCE=.\Kai.h
# End Source File
# Begin Source File

SOURCE=.\Kai_md5.h
# End Source File
# Begin Source File

SOURCE=.\Kai_md5_global.h
# End Source File
# Begin Source File

SOURCE=.\KaiAppDescriptor.h
# End Source File
# Begin Source File

SOURCE=.\KaiChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\KaiCompound.h
# End Source File
# Begin Source File

SOURCE=.\KaiDoc.h
# End Source File
# Begin Source File

SOURCE=.\KaiDocDescriptor.h
# End Source File
# Begin Source File

SOURCE=.\KaiException.h
# End Source File
# Begin Source File

SOURCE=.\KaiFontDescriptor.h
# End Source File
# Begin Source File

SOURCE=.\KaiGeneric.h
# End Source File
# Begin Source File

SOURCE=.\KaiLineDescriptor.h
# End Source File
# Begin Source File

SOURCE=.\KaiMainFrm.h
# End Source File
# Begin Source File

SOURCE=.\KaiParagraph.h
# End Source File
# Begin Source File

SOURCE=.\KaiScreenItemDescriptor.h
# End Source File
# Begin Source File

SOURCE=.\KaiStdAfx.h
# End Source File
# Begin Source File

SOURCE=.\KaiString.h
# End Source File
# Begin Source File

SOURCE=.\KaiTextPos.h
# End Source File
# Begin Source File

SOURCE=.\KaiTypes.h
# End Source File
# Begin Source File

SOURCE=.\KaiView.h
# End Source File
# Begin Source File

SOURCE=.\KaiWord.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\Kai.ico
# End Source File
# Begin Source File

SOURCE=.\res\Kai.rc2
# End Source File
# Begin Source File

SOURCE=.\res\KaiDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Kai.reg
# End Source File
# Begin Source File

SOURCE=.\Notes.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Schedule.txt
# End Source File
# Begin Source File

SOURCE=.\TimeLog.txt
# End Source File
# End Target
# End Project
