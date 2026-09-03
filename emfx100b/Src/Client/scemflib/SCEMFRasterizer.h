/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFRASTERIZER_H_
#define _SCEMFRASTERIZER_H_

#include "SCEMFdcRenderer.h"
#include "SCEMFgdiParser.h"

class SCEMFRasterizer : public CSCEMFgdiParser
{
// Construction/Destruction
public:
	SCEMFRasterizer();
	virtual ~SCEMFRasterizer();

// Accessors
	// getters
	#ifdef _DEBUG
    virtual CString SCGetClassName() { return _T("SCEMFRasterizer"); };
	#endif
	CSCEMFdcRenderer&	SCGetRenderer() { return m_Renderer; }

	// setters

// Operation
public:

// Implementation
protected:
	// helpers

private:
	CSCEMFdcRenderer	m_Renderer;
};


#endif //_SCEMFRASTERIZER_H_
//  ------------------------------------------------------------
