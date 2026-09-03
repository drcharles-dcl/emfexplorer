/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFXVERSION_H_
#define _SCEMFXVERSION_H_
#if 0
// short names are required by old driver compilers
#ifdef SC_USE_MSDOSNAME
	#include "kSCPro~1.h"
#else
	#include "kSCProdDefs.h"
#endif

#define EMFEXPLORER_VERSION_TAG			SC_SOFTWARENAME_PREFIX("1.0.a")

#define SC_EMFEXPLORER_VERMAJ			1
#define SC_EMFEXPLORER_VERMIN			0
#define SC_TAG_BUILD_NUMBER				'a'
#define LONG_EMFEXPLORER_VERSION_TAG	L(EMFEXPLORER_VERSION_TAG)

#define SC_EMFEXPLORER_VERSION			SC_EMFEXPLORER_VERMAJ + (SC_EMFEXPLORER_VERMIN << 8) + (SC_TAG_BUILD_NUMBER << 16)	// Vikao version

/////////////////////////////////////////////////////////////////////////////
// WARNING: Do not delete the trailing \0 in the text values below.

/////////////////////////////////////////////////////////////////////////////
// The following macros are used in each specific _.rc2 file.
//
// When you change the SC_VER_PRODUCT_VER or the SC_TAG_BUILD_NUMBER 
// don't forget to put the same value
// in the SC_VER_FILEVERSION_TXT text macro.
//
#define SC_VER_PRODUCT_VER		1, 0, 'a', 0
#define SC_VER_FILEVERSION		SC_VER_PRODUCT_VER

#define SC_VER_COMPANY_TXT		"CompanyName",		"FrazMitic\0"
#define SC_VER_PRODUCT_VER_TXT	"ProductVersion",	"1.0.a\0"
#define SC_VER_FILEVERSION_TXT	"FileVersion",		"1.000.a.00\0"
#define SC_VER_COPYRIGHT_TXT	"LegalCopyright",	"Copyright (c) 2004 FrazMitic\0"
#define SC_VER_TRADEMARK_TXT	"LegalTrademarks",	"EMFExplorer® is a trademark of FrazMitic.\0"
#define SC_VER_PRODUCT_NAME_TXT	"ProductName",		"EMFExplorer\0"

// The following field values are left specific to each module
/*
VALUE "InternalName",		"EMFExplorer\0"
VALUE "OriginalFilename",	"EMFExplorer.EXE\0"
VALUE "FileDescription",	"EMFExplorer MFC Application\0"
VALUE "Comments",			"EMFExplorer Test\0"
VALUE "PrivateBuild",		"EMFExplorer private build\0"
VALUE "SpecialBuild",		"EMFExplorer special build\0"
*/


/////////////////////////////////////////////////////////////////////////////
//	Here is an example of use in EMFExplorer_v.rc2 :
//
/*
...
BLOCK "040904B0"
BEGIN
	VALUE SC_VER_COMPANY_TXT
	VALUE SC_VER_FILEVERSION_TXT
	VALUE SC_VER_COPYRIGHT_TXT
	VALUE SC_VER_TRADEMARK_TXT
	VALUE SC_VER_PRODUCT_VER_TXT
	VALUE SC_VER_PRODUCT_NAME_TXT
	VALUE "InternalName", "EMFExplorer\0"
	VALUE "OriginalFilename", "EMFExplorer.EXE\0"
	VALUE "FileDescription", "EMFExplorer MFC Application\0"
	//VALUE "Comments", "<add comments here>\0"
	//VALUE "PrivateBuild", "<add private build info here>\0"
	//VALUE "SpecialBuild", "<add special build here>\0"
END
...

// If you want to make a special build where only some modules are modified, do as follows:
  1) modify the build version in SCEMFxVersion.h
  2) add an information in the SpecialBuild field.
		for example:
		VALUE "SpecialBuild", "EMFExplorer patch 2.1\0"
		3) make the build
*/
#endif
#endif //_SCEMFXVERSION_H_
//  ------------------------------------------------------------
