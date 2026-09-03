/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _KSCEMFLIBERROR_H_
#define _KSCEMFLIBERROR_H_

#if 0
// Architecture stuff (for future extension)

#include "SCInclude.h"
#include SC_INC_CTTOOLS(ktoError.h)		//Error domains and Common error codes
#include SC_INC_ERRLIB(kToErr.h)		//CSCError, CSCErrorBlock (link with errorlib)
#include SC_INC_ERRLIB(kToModules.h)	//Macros for modules

#define SCEMFLIB_ERROR(lErrorNum)		SCEMFEXPLORER_ERROR(lErrorNum, SC_MODULE_CLIENT_RENDERER)
#define SCEMFLIB_ERRORMSG(szMsg)		SCEMFEXPLORER_ERRORBLOCK(szMsg, SC_SCEMFLIB_CONTEXT_MESSAGE_ERROR, SC_MODULE_CLIENT_RENDERER)

#endif

#include "kSCEMFLIBErrorIndex.h"			

#endif //_KSCEMFLIBERROR_H_
//  ------------------------------------------------------------
