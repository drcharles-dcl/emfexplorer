/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGENINCLUDE_H_
#define _SCGENINCLUDE_H_

// Note: this stuff relies on Visual C++ IDE
// 1. This file must be placed in a directory declared in the VC options tab
// 2. All paths are computed relatively to the main workspace directory
//    (the one containing the main .dsw file)
//	  For example, if AMakeAllEMFx.dsw is in d:\devl\emfx\src, this directory must be
//	  . Set as a global include directory in the IDE
//	  . (for command line builds) Set with the /I compiler directive
//		 or in INCLUDE environment variable
// 3. Usage:
//		a) include this file	ex.=> #include "SCGenInclude.h"
//		b) call a macro			ex.=> #include SC_INC_EMFLIB(SCEMFDefs.h)
//		this gives access to SCEMFDefs.h from anywhere in the workspace
// 4. When project architecture changes, you have one line to modify in this file.
// 5. The bad news is that "Open document" in VC's contextual menu won't work,
//	  as VC won't solve the macro on the fly.

//-- General purpose macros ----
#define STRX(x)	#x

//--- Macros for includes ------------------------------
#define SC_INC_COMMON(incfile)	STRX(Common/Include/ ##incfile)
#define SC_INC_GENLIB(incfile)	STRX(Common/SCGenLib/ ##incfile)
#define SC_INC_ERRLIB(incfile)	STRX(Common/ErrorLib/ ##incfile)
#define SC_INC_ZIPLIB(incfile)	STRX(Common/zlib/zlib-1.2.1/ ##incfile)
#define SC_INC_WINLIB(incfile)	STRX(Client/SCWinLib/ ##incfile)
#define SC_INC_EMFLIB(incfile)	STRX(Client/SCEMFLib/ ##incfile)
#define SC_INC_SHARED(incfile)	STRX(Client/SCSharedLib/ ##incfile)

#define SC_INC_EMFEXP(incfile)	STRX(Client/EMFexplorer/ ##incfile)
#define SC_INC_EMFAX(incfile)	STRX(Client/SCEMFAx/ ##incfile)




#endif //_SCGENINCLUDE_H_
//  ------------------------------------------------------------
