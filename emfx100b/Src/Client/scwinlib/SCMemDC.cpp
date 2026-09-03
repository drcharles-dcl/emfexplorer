/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCMemDC.h"
#include "SCBitmap.h"
#include "SCPalette.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCMemDC::CSCMemDC():
	CDC(),
	m_hStockBM(NULL),
	m_pBmpMem(NULL),
	m_pPal(NULL),
	m_iWidth(32),
	m_iHeight(32),
	m_InitColor(RGB(255,255,255)),
	m_iGrayScale(STGRAY_SCALE_NONE),
	m_iGrMode(0)
{
}

CSCMemDC::~CSCMemDC()
{
	// Clean up
	SCCleanUp();

	// delete Windows DC
	this->DeleteDC();
}

void CSCMemDC::SCCleanUp()
{
	// restore and delete palette
	if (m_pPal)
	{
		// reselect the stock palette
		::SelectObject(m_hDC, GetStockObject(DEFAULT_PALETTE));
		// delete the palette
		m_pPal->DeleteObject();
		delete m_pPal;
		m_pPal = NULL;
	}

	// restore and delete bitmap
	if (m_pBmpMem)
	{
		// reselect the monochrome stock bitmap
		ASSERT(m_hStockBM);
		::SelectObject(m_hDC, m_hStockBM);
		// now we can delete the bitmap
		m_pBmpMem->DeleteObject();
		delete m_pBmpMem;
		m_pBmpMem = NULL;
	}
}

CBitmap *CSCMemDC::SCDetachDIB()
{ 
	CBitmap *pBmp = m_pBmpMem;
	m_pBmpMem = NULL;
	if (m_hStockBM)
	{
		::SelectObject(m_hDC, m_hStockBM);
		m_hStockBM = NULL;
	}
	return pBmp;
}

BOOL CSCMemDC::SCPrepareSurface(int width, int height, HENHMETAFILE hemf/*=NULL*/,
								COLORREF crBkColor/*=RGB(255, 255, 255)*/)
{
	SCCleanUp();
	m_iWidth = width;
	m_iHeight = height;
	// Create an output compatible DC
	//
	if (!m_hDC)
	if (!CreateCompatibleDC(NULL))
		return FALSE;

	// Use screen DC as compatible output device
	HBITMAP hbm = NULL;
	HPALETTE hPalette = NULL;
	if (m_iGrayScale)
		hbm = SCCreateGrayScaleDIBSection(NULL, m_iWidth, m_iHeight, m_iGrayScale);
	else
	{
		hbm = SCCreate24BPPDIBSection(NULL, m_iWidth, m_iHeight);
		if (hemf)
			hPalette = SCGetEMFPalette(hemf);
		//else
		//	hPalette = SCGetSystemPalette();
	}

	if (!hbm)
	{
		return FALSE;
	}
	m_pBmpMem = new CBitmap;
	m_pBmpMem->Attach(hbm);
	// Select the bitmap into the off-screen DC.
	//
	// Do not use MFC. MFC would return a temporary object linked to the stock bitmap
	// m_pBmOld = this->SelectObject(m_pBmpMem);	  // no!
	// Instead: obtain the stock monochrome bitmap from GDI and save it, as we cant use
	// GetStockObject for this bitmap.
	m_hStockBM = (HBITMAP)::SelectObject(m_hDC, hbm);
	ASSERT(m_hStockBM); // if NULL, GDI failed (maybe memory is low)

	// Palette management
	if (hPalette)
	{
		m_pPal = new CPalette;
		m_pPal->Attach(hPalette);
		CPalette* pPalOld = this->SelectPalette(m_pPal, TRUE); // pPalOld is temporary
		if (pPalOld)
		{
			if (GDI_ERROR == this->RealizePalette())
			{// Error management ?
				// We can grab something (not too pretty) without palette
				
			}
		}
		// Error management ?
		// We can grab something (not too pretty) without palette
	}

	// Erase the background.
	//
	m_InitColor = crBkColor;
	return SCEraseBkgn();
}

BOOL CSCMemDC::SCShrinkSurface(CRect rc)
{
	// Create an output compatible DC
	//
	CDC tmpDC;
	if (!tmpDC.CreateCompatibleDC(NULL))
		return FALSE;

	// Use screen DC as compatible output device
	HBITMAP hbm = NULL;
	HPALETTE hPalette = NULL;
	if (m_iGrayScale)
		hbm = SCCreateGrayScaleDIBSection(NULL, rc.Width(), rc.Height(), m_iGrayScale);
	else
	{
		hbm = SCCreate24BPPDIBSection(NULL, rc.Width(), rc.Height());
	}

	if (!hbm)
		return FALSE;

	CBitmap* pBmp = new CBitmap;
	pBmp->Attach(hbm);
	// Select the bitmap into the off-screen DC and init tmp surface.
	//
	CBitmap* pBmOld = tmpDC.SelectObject(pBmp);
	tmpDC.BitBlt(0, 0, rc.Width(), rc.Height(), this, rc.left, rc.top, SRCCOPY);
	tmpDC.SelectObject(pBmOld);

	// Swap surfaces
	this->SelectObject(pBmp); // select new bmp
	// delete the old one
	if (m_pBmpMem)
	{
		m_pBmpMem->DeleteObject();
		delete m_pBmpMem;
	}
	m_pBmpMem = pBmp;

	// new surface size
	m_iWidth = rc.Width();
	m_iHeight = rc.Height();
	return TRUE;
}

BOOL CSCMemDC::SCResetSurfaceColor(int iColor)
{
	if (m_InitColor == iColor)
		return TRUE;

	m_InitColor = iColor;
	return SCEraseBkgn();
}

BOOL CSCMemDC::SCEraseBkgn()
{
	CBrush brush;
	CRect rc(0, 0, m_iWidth, m_iHeight);
	brush.CreateSolidBrush(m_InitColor);
	DPtoLP(&rc);
	FillRect(&rc, &brush);
	return TRUE;
}

BOOL CSCMemDC::SCEraseBkgn(CBrush* pBrush)
{
	ASSERT(pBrush);
	CRect rc(0, 0, m_iWidth, m_iHeight);
	DPtoLP(&rc);
	FillRect(&rc, pBrush);
	return TRUE;
}

///
/// Rotate and translate DC so that the origin is at (rect.left, rect.top)
///
BOOL CSCMemDC::SCRotateDC(int iAngle, CRect rect, int iXPos, int iYPos)
{
	return ::SCRotateDC(m_hDC, iAngle, rect, iXPos, iYPos, m_iGrMode);
}

///
/// Translate DC so that the origin return at (0, 0) after it was set at (rect.left, rect.top)
///
BOOL CSCMemDC::SCRestoreOrigin(int iAngle, CRect rect, int iXPos, int iYPos)
{
	XFORM xform;
	memset(&xform, 0, sizeof(xform));
	xform.eM11 = 1;
	xform.eM22 = 1;
	switch (iAngle)
	{
	case 90:
		xform.eDx = (float)(-rect.top + iXPos);
		xform.eDy = (float)(rect.left + iYPos);
		break;

	case 270:
		xform.eDx = (float)(rect.top + iXPos);
		xform.eDy = (float)(-rect.left + iYPos);
		break;

	case 180:
		xform.eDx = (float)(-rect.left + iXPos);
		xform.eDy = (float)(-rect.top + iYPos);
		break;

	case 0:
		xform.eDx = (float)(rect.left + iXPos);
		xform.eDy = (float)(rect.top + iYPos);
		break;

	default:
		ASSERT(0);
	}
	BOOL bOk = ::ModifyWorldTransform(m_hDC, &xform, MWT_RIGHTMULTIPLY);
	ASSERT(bOk);

	return bOk;
}

BOOL CSCMemDC::SCSelfRotate(int iAngle, CSize sizeDelta)
{
	m_iGrMode = ::SetGraphicsMode(m_hDC, GM_ADVANCED);
	ASSERT(m_iGrMode);
	if (!m_iGrMode)
		return FALSE;

	BOOL bOk = FALSE;
	XFORM xform;
	memset(&xform, 0, sizeof(xform));
	// Save xform
	XFORM xformSvd;
	bOk = ::GetWorldTransform(m_hDC, &xformSvd);
	ASSERT(bOk);

	/* translate back to the origin. */
	xform.eM11 =
    xform.eM22 = (float) 1.0;
	::SetWorldTransform (m_hDC, &xform);

	// rotate
	switch (iAngle)
	{
	case 90:
		xform.eM11 = 0;
		xform.eM12 = 1;
		xform.eDx = (float)(sizeDelta.cy);
		break;
	case 270:
		xform.eM11 = 0;
		xform.eM12 = -1;
		xform.eDy = (float)(m_iWidth);
		break;
	case 180:
		xform.eM11 = -1;
		xform.eM12 = 0;
		xform.eDy = (float)m_iHeight;
		xform.eDx = (float)m_iWidth;
		break;
	case 0:
		xform.eM11 = 1;
		xform.eM12 = 0;
		break;
	default:
		ASSERT(0);
	}
	xform.eM22 = xform.eM11;
	xform.eM21 = -xform.eM12;
	bOk=::ModifyWorldTransform(m_hDC, &xform, MWT_RIGHTMULTIPLY);
	ASSERT(bOk);

	/* translate back to the original offset. */
	xform.eM11 =
    xform.eM22 = (float) 1.0;
	xform.eM12 =
    xform.eM21 = (float) 0.0;
	
	xform.eDx = xformSvd.eDx;
	xform.eDy = xformSvd.eDy;
	::ModifyWorldTransform (m_hDC, &xform, MWT_RIGHTMULTIPLY);
	return bOk;
}

BOOL CSCMemDC::SCUnRotateDC()
{
	return ::SCUnRotateDC(m_hDC, m_iGrMode);
}

////////////////////////////////////////////////////////////////////////////////////
/// Static
///

/// Rotate and translate DC so that the origin is at (rect.left, rect.top)
///
BOOL SCRotateDC(HDC hDC, int iAngle, CRect rect, int iXPos, int iYPos, int& iGrOldMode)
{
	iGrOldMode = SetGraphicsMode(hDC, GM_ADVANCED);
	ASSERT(iGrOldMode);
	if (!iGrOldMode)
		return FALSE;

	BOOL bOk = FALSE;
	XFORM xform;
	memset(&xform, 0, sizeof(xform));

	// Formulas:	
	//xform.eM11 = (float)cos((float)iAngle*(ST_PI)/180.0f); => 0 for PI/2 and 3PI/2, 1 for 0 and PI
	//xform.eM12 = (float)sin((float)iAngle*(ST_PI)/180.0f); => 0 for 0 and PI, 1 for PI/2 and 3PI/2

	// rotate and translate at once
	switch (iAngle)
	{
	case 90:
		xform.eM11 = 0;
		xform.eM12 = 1;
		xform.eDx = (float)(rect.bottom + iXPos);
		xform.eDy = (float)(-rect.left + iYPos);
		break;

	case 270:
		xform.eM11 = 0;
		xform.eM12 = -1;
		xform.eDx = (float)(-rect.top + iXPos);
		xform.eDy = (float)(rect.right + iYPos);
		break;

	case 180:
		xform.eM11 = -1;
		xform.eM12 = 0;
		xform.eDx = (float)(rect.right + iXPos);
		xform.eDy = (float)(rect.bottom + iYPos);
		break;

	case 0:
		xform.eM11 = 1;
		xform.eM12 = 0;
		xform.eDx = (float)(-rect.left + iXPos);
		xform.eDy = (float)(-rect.top + iYPos);
		break;

	default:
		ASSERT(0);
	}
	xform.eM22 = xform.eM11;
	xform.eM21 = -xform.eM12;
	bOk=SetWorldTransform(hDC, &xform);
	ASSERT(bOk);

	return bOk;
}

BOOL SCUnRotateDC(HDC hDC, int iGrOldMode)
{
	if (!iGrOldMode)
		return FALSE;

	BOOL bOk = ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
	ASSERT(bOk);
	int iGrMode = SetGraphicsMode(hDC, iGrOldMode);
	ASSERT(iGrMode);
	return bOk;
}

HDC SCGetAttribDC(HDC hDC)
{
	ASSERT(hDC);
	CDC* pDC = CDC::FromHandle(hDC);
	return pDC->m_hAttribDC;
}
////////////////////////////////////////////////////////////////////////////////////

