/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _CATHELP_H_
#define _CATHELP_H_

////////////////////////////////////////////////////////////////////////////////////////
/// (c) Microsoft, 1998
// MSDN: Article ID: Q161873
// HOWTO: Mark MFC Controls Safe for Scripting/Initialization

#include "comcat.h"


// Helper function to create a component category and associated
// description
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);


// Helper function to register a CLSID as belonging to a component
// category
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid);
////////////////////////////////////////////////////////////////////////////////////////


#endif //_CATHELP_H_
//  ------------------------------------------------------------
