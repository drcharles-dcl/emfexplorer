# Microsoft Developer Studio Project File - Name="SCEMFLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SCEMFLib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SCEMFLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SCEMFLib.mak" CFG="SCEMFLib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SCEMFLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SCEMFLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SCEMFLib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SCEMFLib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SCEMFLib - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCEMFLib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCEMFLib - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SCEMFLib___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "SCEMFLib___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SCEMFLib - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SCEMFLib___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "SCEMFLib___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
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

# Name "SCEMFLib - Win32 Release"
# Name "SCEMFLib - Win32 Debug"
# Name "SCEMFLib - Win32 Unicode Release"
# Name "SCEMFLib - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "DCRenderer"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SCDCRendImages_i.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCRendLines_i.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCRendPaths_i.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCRendPolys_i.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCRendRgns_i.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDCRendTexts_i.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SCBrkEMF.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMF.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMF2Text.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFdcRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFDocPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFDocProp.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFDocUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFgdiParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFImage.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFImageProp.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFRasterizer.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFTxtUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\SCEMFViewHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\SCGDIplusStrLists.cpp
# End Source File
# Begin Source File

SOURCE=.\SCGdiplusUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\I_EMFRenderer.h
# End Source File
# Begin Source File

SOURCE=.\kSCEMFLibError.h
# End Source File
# Begin Source File

SOURCE=.\kSCEMFLibErrorIndex.h
# End Source File
# Begin Source File

SOURCE=.\kSCEMFLibMsgs.h
# End Source File
# Begin Source File

SOURCE=.\SCBrkEMF.h
# End Source File
# Begin Source File

SOURCE=.\SCEMF.h
# End Source File
# Begin Source File

SOURCE=.\SCEMF2Text.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFConverter.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFdcRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFDefs.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFDoc.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFDocPage.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFDocUtils.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFgdiParser.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFImage.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFRasterizer.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFRasterizerDefs.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFTxtUtils.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFViewDefs.h
# End Source File
# Begin Source File

SOURCE=.\SCEMFViewHelpers.h
# End Source File
# Begin Source File

SOURCE=.\SCFileTypes.h
# End Source File
# Begin Source File

SOURCE=.\SCGDIplusStrLists.h
# End Source File
# Begin Source File

SOURCE=.\SCGdiplusTypes.h
# End Source File
# Begin Source File

SOURCE=.\SCGdiplusUtils.h
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
