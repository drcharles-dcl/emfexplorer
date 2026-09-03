/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCZoomShrd.h"
#include SC_INC_EMFLIB(SCEMFViewDefs.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const SCZoomEntry sc_ZoomList[] = {
	_T(""), SC_MIN_ZOOM,
	_T(""), 833,
	_T(""), 1250,
	_T(""), 2500,
	_T(""), 5000,
	_T(""), 7500,
	_T(""), 10000,
	_T(""), 12500,
	_T(""), 13333,
	_T(""), 15000,
	_T(""), 20000,
	_T(""), 30000,
	_T(""), 40000,
	_T(""), 60000,
	_T(""), 80000,
#if (SC_MAX_ZOOM>100000)
	_T(""), 100000,
#endif
	_T(""), SC_MAX_ZOOM,
	_T("Fit Width"), SC_ZOOM_FITWITDH,
	_T("Fit Page"), SC_ZOOM_FITPAGE
};
extern const USHORT cs_usZoomCount = sizeof(sc_ZoomList)/sizeof(SCZoomEntry);

