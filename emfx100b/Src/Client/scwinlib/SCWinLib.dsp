# Microsoft Developer Studio Project File - Name="SCWinLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SCWinLib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SCWinLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SCWinLib.mak" CFG="SCWinLib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SCWinLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SCWinLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SCWinLib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SCWinLib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SCWinLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCWinLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCWinLib - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SCWinLib___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "SCWinLib___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCWinLib - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SCWinLib___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "SCWinLib___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SCWinLib - Win32 Release"
# Name "SCWinLib - Win32 Debug"
# Name "SCWinLib - Win32 Unicode Release"
# Name "SCWinLib - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MSDib.cpp
# End Source File
# Begin Source File

SOURCE=.\SCAlphaBand.cpp
# End Source File
# Begin Source File

SOURCE=.\SCBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\SCColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCCloner.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SCGDIPlus.cpp
# End Source File
# Begin Source File

SOURCE=.\SCGDIRops.cpp
# End Source File
# Begin Source File

SOURCE=.\SCGDIUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\SCImgStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\SCMemDC.cpp
# End Source File
# Begin Source File

SOURCE=.\SCOleBaseOlePPg.cpp
# End Source File
# Begin Source File

SOURCE=.\SCPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\SCPreviewView.cpp
# End Source File
# Begin Source File

SOURCE=.\SCRectPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\SCRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SCRichEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SCTbComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SCWinFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SCWinGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\SCWinStrLists.cpp
# End Source File
# Begin Source File

SOURCE=.\SCWinThumbs.cpp
# End Source File
# Begin Source File

SOURCE=.\SCZoomBox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MSDib.h
# End Source File
# Begin Source File

SOURCE=.\SCAlphaBand.h
# End Source File
# Begin Source File

SOURCE=.\SCBitmap.h
# End Source File
# Begin Source File

SOURCE=.\SCColorBox.h
# End Source File
# Begin Source File

SOURCE=.\SCDCCloner.h
# End Source File
# Begin Source File

SOURCE=.\SCEditBox.h
# End Source File
# Begin Source File

SOURCE=.\SCGDIPlus.h
# End Source File
# Begin Source File

SOURCE=.\SCGDIRops.h
# End Source File
# Begin Source File

SOURCE=.\SCGDIUtils.h
# End Source File
# Begin Source File

SOURCE=.\SCImgStatic.h
# End Source File
# Begin Source File

SOURCE=.\SCMemDC.h
# End Source File
# Begin Source File

SOURCE=.\SCOleBaseOlePPg.h
# End Source File
# Begin Source File

SOURCE=.\SCPalette.h
# End Source File
# Begin Source File

SOURCE=.\SCPreviewView.h
# End Source File
# Begin Source File

SOURCE=.\SCPropHelpers.h
# End Source File
# Begin Source File

SOURCE=.\SCRectPoly.h
# End Source File
# Begin Source File

SOURCE=.\SCRegistry.h
# End Source File
# Begin Source File

SOURCE=.\SCRichEdit.h
# End Source File
# Begin Source File

SOURCE=.\SCTbComboBox.h
# End Source File
# Begin Source File

SOURCE=.\SCWinFile.h
# End Source File
# Begin Source File

SOURCE=.\SCWinGUI.h
# End Source File
# Begin Source File

SOURCE=.\SCWinStrLists.h
# End Source File
# Begin Source File

SOURCE=.\SCWinThumbs.h
# End Source File
# Begin Source File

SOURCE=.\SCWinTypes.h
# End Source File
# Begin Source File

SOURCE=.\SCZoomBox.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
