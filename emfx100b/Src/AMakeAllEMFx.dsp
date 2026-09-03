# Microsoft Developer Studio Project File - Name="AMakeAllEMFx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=AMakeAllEMFx - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AMakeAllEMFx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMakeAllEMFx.mak" CFG="AMakeAllEMFx - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMakeAllEMFx - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "AMakeAllEMFx - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE "AMakeAllEMFx - Win32 Unicode Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "AMakeAllEMFx - Win32 Unicode Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
MTL=midl.exe

!IF  "$(CFG)" == "AMakeAllEMFx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Bin"
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AMakeAllEMFx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Bin"
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AMakeAllEMFx - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AMakeAllEMFx___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "AMakeAllEMFx___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AMakeAllEMFx - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AMakeAllEMFx___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "AMakeAllEMFx___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "AMakeAllEMFx - Win32 Release"
# Name "AMakeAllEMFx - Win32 Debug"
# Name "AMakeAllEMFx - Win32 Unicode Release"
# Name "AMakeAllEMFx - Win32 Unicode Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Common\Include\kSCProdDefs.h
# End Source File
# Begin Source File

SOURCE=.\Common\Include\SCEMFxVersion.h
# End Source File
# Begin Source File

SOURCE=.\SCGenInclude.h
# End Source File
# End Group
# End Target
# End Project
