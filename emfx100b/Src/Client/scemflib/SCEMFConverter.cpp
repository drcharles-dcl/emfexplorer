/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFConverter.h"

#include "SCEMF.h"
#include "SCEMFRasterizer.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_WINLIB(SCBitmap.h)
#include <afxext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SCEMFConverter::SCEMFConverter():
	m_pEMFDoc(NULL),
	m_iCurZoom(SC_ZOOM100),
	m_rectMargins(0,0,0,0),
	m_bReverseVideo(FALSE),
	m_uiRasEngine(SC_ENGINE_GDIP),
	m_crPaperColor(RGB(255,255,255)),
	m_rectClient(0,0,0,0),
	m_iFitMode(SC_FIT_NONE),
	//
	m_iCtxAngle(0),
	m_iCtxPage(0),
	m_rcCtxDest(0,0,0,0),
	m_bCtxMeta(FALSE),
	m_hdcCtxAttrib(NULL),
	m_bCtxSaveAsIs(TRUE),
	m_bCtxInvert(FALSE)
{
}

SCEMFConverter::~SCEMFConverter()
{
}

// Save/Copy/Print
void SCEMFConverter::SCDisplayPageEx(HDC hDC, HENHMETAFILE hEmf,
									   CRect& rcDest, CRect& rcPaper,
									   SCGDIpDrawingAttributes& rAttributes,
									   int iAngle/*=0*/, int iZoom/*=SC_ZOOM100*/,
									   BOOL bInvert/*=FALSE*/)
{
	// Paper
	CRect PaperRect = rcPaper;

	// Position of the EMF rectangle in world coordinates for playing
	CSize sizeEMF;
	SCGetEMFPlaySize(hEmf, sizeEMF);

	int iPlayX = rcDest.left - PaperRect.left;
	int iPlayY = rcDest.top - PaperRect.top;
	CRect rcPlay(iPlayX, iPlayY,
		iPlayX + MulDiv(sizeEMF.cx, iZoom, SC_ZOOM100),
		iPlayY + MulDiv(sizeEMF.cy, iZoom, SC_ZOOM100));

	int iState = SaveDC(hDC);
	::IntersectClipRect(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);

	int iGrOldMode;
	if (iAngle)
	{// Position of the PAGE rectangle for rotation in world coordinates
		CRect rcRotate = PaperRect;
		rcRotate.OffsetRect(iPlayX, iPlayY);
		SCRotateDC(hDC, iAngle, rcRotate, rcDest.left, rcDest.top, iGrOldMode);
	}
	
	// Rasterize
	BOOL bOK = FALSE;
	if (SCGdipGetEMFType(hEmf)!=EmfTypeEmfOnly)
		bOK = SCGdipPlayMetafile(hDC, hEmf, rcPlay);
	else
	if (SC_ENGINE_GDIP==m_uiRasEngine)
	{
		SCEMFRasterizer Rasterizer;
		CSCEMFdcRenderer& rRenderer = Rasterizer.SCGetRenderer();
		rRenderer.SCSetDrawingAttributes(rAttributes);
		Rasterizer.SCBreakMetafile(hDC, hEmf, NULL, (LPRECT)&rcPlay);
		bOK = TRUE;
	} else
	{
		bOK = PlayEnhMetaFile(hDC, hEmf, (LPRECT)&rcPlay);
	}
	if (m_bCtxSaveAsIs)
		SCPostRender(m_hdcCtxAttrib, hDC, NULL);

	// Restore DC state
	RestoreDC(hDC, iState);

	// Perform full rvideo (AFTER restore DC to prevent gaps due to rounding errors)
	if (bInvert)
		PatBlt(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), DSTINVERT);
}

#define SC_USING_TRICK81

///
/// Copy the given page to another format
///
HANDLE SCEMFConverter::SCCopyPage(int iPage,
								  int iOutputType, int iSubType, BOOL bSaveAsIs)
{
	ASSERT(m_pEMFDoc);
	if (!m_pEMFDoc)
		return NULL;

	ASSERT(iPage>=1 && iPage<=(int)m_pEMFDoc->SCGetNbPages());
	HENHMETAFILE hEMF = m_pEMFDoc->SCGetPageEMF(iPage-1);
	if (!hEMF)
		return NULL;

	m_iCtxPage = iPage;
	m_bCtxSaveAsIs = bSaveAsIs;

	HANDLE hRes = NULL;
	CSize sizeEMF;
	SCGetEMFPlaySize(hEMF, sizeEMF);
	
	COLORREF crBkColor = RGB(255, 255, 255);
	SCGDIpDrawingAttributes rAttributes = m_CtxDrawingAttributes;

	CRect PaperRect;
	if (bSaveAsIs)
	{// sort of wysiwyg, including current zoom, borders, rotation, etc.
		PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(iPage-1);
		ASSERT(pDocPage);
		pDocPage->SCComputePageInfo(m_bReverseVideo, m_rectMargins);
		pDocPage->SCGetPageBlackBox(PaperRect);
		SCComputeFitZoom(PaperRect);

		PaperRect.InflateRect(m_rectMargins.left, m_rectMargins.top, m_rectMargins.right, m_rectMargins.bottom);
		PaperRect.left = MulDiv(PaperRect.left, m_iCurZoom, SC_ZOOM100);
		PaperRect.right = MulDiv(PaperRect.right, m_iCurZoom, SC_ZOOM100);
		PaperRect.top = MulDiv(PaperRect.top, m_iCurZoom, SC_ZOOM100);
		PaperRect.bottom = MulDiv(PaperRect.bottom, m_iCurZoom, SC_ZOOM100);
		
		// background
		crBkColor = (m_bReverseVideo && !SC_RVIDEOFULL(rAttributes))?
					SCGetReversedPaperColor() : SCGetTranslatedPaperColor();
		rAttributes.bBkSolid = (SCGetTranslatedPaperColor()!=RGB(255, 255, 255));
	} else
	{// convert only
		PaperRect.SetRect(0, 0, sizeEMF.cx, sizeEMF.cy);
		rAttributes.bBkSolid = FALSE;
	}

	CRect rcPlay(0, 0,	PaperRect.Width(), PaperRect.Height());
	m_rcCtxDest = rcPlay; // unrotated paper
	
	if (bSaveAsIs && (90==m_iCtxAngle || 270==m_iCtxAngle))
	{// rotate play rect, since the paper is not rotated
		SMC_ISWAP(rcPlay.left, rcPlay.top);
		SMC_ISWAP(rcPlay.right, rcPlay.bottom);
		SMC_ISWAP(sizeEMF.cx, sizeEMF.cy);
	}
	
	CSCMemDC MemDC;
	if (!MemDC.SCPrepareSurface(rcPlay.Width(), rcPlay.Height(), hEMF, crBkColor))
	{// A large zoom may require a lot of Megs.
		AfxMessageBox(SC_EMFLIB_UFMTERR_FSAVE);
		return NULL;
	}
	CDC* pDC = &MemDC;
	CMetaFileDC* pMeta = NULL;
	HBITMAP hMetaBm = NULL;
	HBITMAP hOldMetaBm = NULL;
	CRect rcMeta;
	m_hdcCtxAttrib = MemDC.m_hDC;
	m_bCtxInvert = (bSaveAsIs && SCIsFullReverseVideo());

	switch (iOutputType)
	{
	case SC_FTYPE_EMF:
	case SC_FTYPE_WMF:
		{
			m_bCtxMeta = TRUE;
			// create metafile
			rcMeta = rcPlay;
			SCRectLPtoHIMETRIC(pDC, &rcMeta);
#ifdef SC_USING_TRICK81
			{// Sorry for this. Any other technique is welcome.
				// Note: MemDC is based on screen, and GetDeviceCaps may return 96
				// MetaDC is based on MemDC should contain a picture at 96.
				// But the physical vs logical DPI mess in Windows yields 81.
				CMetaFileDC metaDC;
				metaDC.CreateEnhanced(&MemDC, NULL, &rcMeta, NULL);
				metaDC.SetAttribDC(MemDC.m_hDC);
				HENHMETAFILE hVoidEMF = metaDC.CloseEnhanced();
				if (hVoidEMF)
				{
					long lEmfDPIX = 0;
					long lEmfDPIY = 0;
					long lEmfPaperCx = 0;
					long lEmfPaperCy = 0;
					if (SCGetEMFInfos(hVoidEMF, lEmfDPIX, lEmfDPIY,
						lEmfPaperCx, lEmfPaperCy))
					{
						long lDevDPIX = MemDC.GetDeviceCaps(LOGPIXELSX);
						long lDevDPIY = MemDC.GetDeviceCaps(LOGPIXELSY);
						// Note: picture me be larger than expected
						rcMeta.right = MulDiv(rcMeta.right, lDevDPIX, lEmfDPIX);
						rcMeta.bottom = MulDiv(rcMeta.bottom, lDevDPIY, lEmfDPIY);
					}
					::DeleteEnhMetaFile(hVoidEMF);
				}
			}
#endif

			if (SC_ENGINE_GDIP==m_uiRasEngine ||
				EmfTypeEmfPlusOnly==SCGdipGetEMFType(hEMF))
			{
				EmfType eType = EmfTypeEmfOnly;
				switch (iSubType)
				{
				case SC_SUBTYPE_EMF_EMF: eType = EmfTypeEmfOnly; break;
				case SC_SUBTYPE_EMF_PLUS: eType = EmfTypeEmfPlusOnly; break;
				case SC_SUBTYPE_EMF_DUAL: eType = EmfTypeEmfPlusDual; break;
				default:
					ASSERT(0);
				}
				
				HENHMETAFILE hNewEMF;
				if (bSaveAsIs)
					hNewEMF = SCBBoxConvertEMFtoEMFp(hEMF, rAttributes, eType,
					rcPlay, rcMeta, PaperRect, MemDC.m_hDC,
					float(m_iCurZoom)/SC_ZOOM100, m_iCtxAngle, this);
				else
				{
					SCGDIpDrawingAttributes CleanAttributes;
					CleanAttributes.bBkSolid = FALSE;
					hNewEMF = SCConvertEMFtoEMFp(hEMF, CleanAttributes, eType);
				}
				
				ASSERT(hNewEMF);
				return (HANDLE)hNewEMF;
			}
			
			// Interpret GDI EMF or dual EMF, storing to GDI EMF
			hMetaBm = CreateCompatibleBitmap(MemDC.m_hDC, sizeEMF.cx, sizeEMF.cy);
			hOldMetaBm = (HBITMAP)SelectObject(MemDC.m_hDC, hMetaBm);
			
			pMeta = new CMetaFileDC;
			pMeta->CreateEnhanced(&MemDC, NULL, &rcMeta, NULL);
			pMeta->SetAttribDC(MemDC.m_hDC);
			pDC = pMeta;
			
			// background
			if (rAttributes.bBkSolid)
			{
				CBrush brush;
				brush.CreateSolidBrush(crBkColor);
				pDC->FillRect(&rcPlay, &brush);
			}
		}
		break;

		case SC_FTYPE_IMG:
			break;

		default:
			ASSERT(0);
	}

	m_bCtxInvert = FALSE; // prefer invert AFTER restoring DC 
	if (bSaveAsIs)
	{// sort of wysiwyg, including current zoom, borders, rotation, etc.
		SCDisplayPageEx(pDC->m_hDC, hEMF, rcPlay, PaperRect, rAttributes,
			m_iCtxAngle, m_iCurZoom, SCIsFullReverseVideo());
	} else
	{// convert only: save image at 100% with no rotation
		SCDisplayPageEx(pDC->m_hDC, hEMF, rcPlay, PaperRect, rAttributes);
	}
	
	switch (iOutputType)
	{
	case SC_FTYPE_EMF:
	case SC_FTYPE_WMF:
		{
			ASSERT(pMeta);
			hRes = (HANDLE)pMeta->CloseEnhanced();
			ASSERT(hRes);

			ASSERT(hMetaBm);
			SelectObject(MemDC.m_hDC, hOldMetaBm);
			DeleteObject(hMetaBm);
			delete pMeta;
		}
		break;
		
	case SC_FTYPE_IMG:
		{
			CBitmap *pBmp = MemDC.SCDetachDIB();
			ASSERT(pBmp);
			if (pBmp)
			{
				HBITMAP hBM = (HBITMAP)pBmp->Detach();
				delete pBmp;

				hRes = (HANDLE)SCCopyBitmap(hBM);
				DeleteObject(hBM);
			}
		}
	}

	return hRes;
}

void SCEMFConverter::SCPostRender(HDC hAttribDC, HDC hOutDC, GDPGraphics* pOutGraphics)
{
	PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(m_iCtxPage - 1);
	ASSERT(pDocPage);

	int xDest = m_rcCtxDest.left;
	int yDest = m_rcCtxDest.top;
	int iWdt = m_rcCtxDest.Width();
	int iHgt = m_rcCtxDest.Height();

	if (hOutDC)
	{
		if (m_bCtxMeta)
		{
			ASSERT(hAttribDC);
			pDocPage->SCDisplayPageComments(hOutDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iWdt, iHgt, hAttribDC);
		} else
			pDocPage->SCDisplayPageComments(hOutDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iWdt, iHgt);
		
		// Perform full rvideo
		if (m_bCtxInvert)
		{
			// this strange swap is because postrender is called before DC is unrotated
			switch (m_iCtxAngle)
			{
			case 90:
			case 270:
				SMC_ISWAP(iWdt, iHgt);
				break;
			}
			PatBlt(hOutDC, xDest-1, yDest-1, iWdt+2, iHgt+2, DSTINVERT);
		}
	} else
	{
		ASSERT(pOutGraphics);
		pDocPage->SCDisplayPageComments(pOutGraphics, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iWdt, iHgt);

		// Perform full rvideo
		if (m_bCtxInvert)
		{
			// this strange swap is because postrender is called before DC is unrotated
			switch (m_iCtxAngle)
			{
			case 90:
			case 270:
				SMC_ISWAP(iWdt, iHgt);
				break;
			}
			hOutDC = pOutGraphics->GetHDC();
			ASSERT(hOutDC);

			PatBlt(hOutDC, xDest-1, yDest-1, iWdt+2, iHgt+2, DSTINVERT);

			pOutGraphics->ReleaseHDC(hOutDC);
		}
	}
}

void SCEMFConverter::SCComputeFitZoom(CRect& rcElems)
{
	if (SC_FIT_NONE==m_iFitMode)
		return;

	// account for extra margins
	CSize sizeEMF(rcElems.Width() + m_rectMargins.left + m_rectMargins.right + MIN_MARGIN*2,
		rcElems.Height() + m_rectMargins.top + m_rectMargins.bottom + MIN_MARGIN*2);

	// Rotate
	if ((90==m_iCtxAngle || 270==m_iCtxAngle))
	{
		SMC_ISWAP(sizeEMF.cx, sizeEMF.cy);
	}

	// Available client
	int iWndCx = m_rectClient.Width();
	int iWndCy = m_rectClient.Height();

	// scale
	float fScale;
	switch (m_iFitMode)
	{
	case SC_FIT_WIDTH:
		fScale = float(iWndCx)/float(sizeEMF.cx);
		break;

	case SC_FIT_PAGE:
		if (iWndCx>=iWndCy)
		{// attempt keep y, and reduce x
			fScale = float(iWndCy)/float(sizeEMF.cy);
			if (fScale*sizeEMF.cx > iWndCx)
				fScale = float(iWndCx)/float(sizeEMF.cx);
		} else
		{// attempt keep x, and reduce y
			fScale = float(iWndCx)/float(sizeEMF.cx);
			if (fScale*sizeEMF.cy > iWndCy)
				fScale = float(iWndCy)/float(sizeEMF.cy);
		}
		break;

	default:
		ASSERT(0);
		return;
	}

	m_iCurZoom = (int)(fScale*SC_ZOOM100);
	if (m_iCurZoom<SC_MIN_ZOOM)
		m_iCurZoom = SC_MIN_ZOOM;
	else
	if (m_iCurZoom>SC_MAX_ZOOM)
		m_iCurZoom = SC_MAX_ZOOM;
}


