/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

// GDI+ won't allow DEBUG_NEW to work
//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif


using namespace Gdiplus;

#define SC_CHECKGDIP_OBJ(pObj) \
	if (pObj) ASSERT(pObj->GetLastStatus()==Ok)

///
/// g_pFontCopies contains copies of TT fonts already installed for GDI and that
/// that GDI+ accepts to use only if they are placed in a private collection.
/// We don't want to create this private collection on a per-object basis.
/// To ensure a per-module caching, the collection is ref-counted.
/// Call SCCleanFontCopies() when a new document is loaded.
///
SCFontCollection*		g_pFontCopies = NULL;
SCFontCollectionHolder*	g_pFontCopiesHolder = NULL;

///////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////
CSCEMFdcRenderer::CSCEMFdcRenderer():
	m_hDC(NULL),
	m_pGraphics(NULL),
	m_bMonochrome(FALSE),
	m_pPen(NULL),
	m_pBrush(NULL),
	m_pFont(NULL),
	m_pPath(NULL),
	m_dwLastError(SC_EMFERR_NOERROR),
	m_PtCurPos(0,0),
	m_pPenHolder(NULL),
	m_pBrushHolder(NULL),
	m_pFontHolder(NULL),
	m_pPathHolder(NULL),
	m_dwMapMode(MM_TEXT),
	m_dwBkMode(OPAQUE),
	m_dwFillMode(ALTERNATE),
	m_dwStretchBltMode(COLORONCOLOR),
	m_dwTextAlign(TA_LEFT|TA_BASELINE),
	m_dwROP2(R2_COPYPEN),
	m_BkColor(RGB(255, 255, 255)),
	m_TextColor(RGB(0, 0, 0)),
	m_dwArcDirection(AD_COUNTERCLOCKWISE),
	m_fMiterLimit(10.0),
	m_pImgAttribs(NULL),
	m_pFontCollection(NULL)
	// no constructor
		// m_WinSize(1,1),
		// m_ViewSize(1,1),
		// m_PtWinOrg(0,0),
		// m_PtViewOrg(0,0),
		// m_RcClipBox(0,0,0,0)
{
	m_WinSize.cx = m_WinSize.cy = 1;
	m_ViewSize.cx = m_ViewSize.cy = 1;
	m_PtWinOrg.x = m_PtWinOrg.y = 0;
	m_PtViewOrg.x = m_PtViewOrg.y = 0;
	SetRectEmpty(&m_RcClipBox);

	if (!g_pFontCopiesHolder)
	{
		ASSERT(NULL==g_pFontCopies);
		g_pFontCopies = new SCFontCollection;
		g_pFontCopiesHolder = new SCFontCollectionHolder(g_pFontCopies);
	}
	SC_REFCOUNTED_ADDREF(g_pFontCopiesHolder);
}

/*static*/
void CSCEMFdcRenderer::SCCleanFontCopies()
{
	SC_OBJHOLDER_SAFERELEASE_T(g_pFontCopiesHolder, g_pFontCopies, SCFontCollection);
}

CSCEMFdcRenderer::~CSCEMFdcRenderer()
{
	SMC_DELETE(m_pGraphics);
	SMC_DELETE(m_pImgAttribs);

	SC_OBJHOLDER_RELEASE_T(m_pPenHolder, m_pPen, GDPPen);
	SC_OBJCONTAINER_RELEASE_T(m_pBrushHolder, m_pBrush, GDPBrush, SCBrush);
	SC_OBJHOLDER_RELEASE_T(m_pFontHolder, m_pFont, SCFont);
	SC_OBJHOLDER_RELEASE_T(m_pPathHolder, m_pPath, SCPath);

	SC_OBJHOLDER_SAFERELEASE_T(g_pFontCopiesHolder, g_pFontCopies, SCFontCollection);
}

///////////////////////////////////////////////////////////////////////////////////
// Utilities
///////////////////////////////////////////////////////////////////////////////////

///
/// Prepare the graphic context for the rendering session.
///
void CSCEMFdcRenderer::SCSetupRendering()
{
	ASSERT(m_pGraphics);

	// 1. Get current DC state
	m_dwMapMode = GetMapMode(m_hDC);
	m_dwBkMode = GetBkMode(m_hDC);
	m_dwFillMode = GetPolyFillMode(m_hDC);
	m_dwStretchBltMode = GetStretchBltMode(m_hDC);
	m_dwTextAlign = GetTextAlign(m_hDC);
	m_dwROP2 = GetROP2(m_hDC);
	m_BkColor = SC_RGBCOLOR(GetBkColor(m_hDC));
	m_TextColor = SC_FINAL_COLOR(GetTextColor(m_hDC));
	m_dwArcDirection = GetArcDirection(m_hDC);
	m_bMonochrome = SCIsMonochromeDC(m_hDC);
	GetMiterLimit(m_hDC, &m_fMiterLimit);
	GetWindowExtEx(m_hDC, &m_WinSize);
	GetViewportExtEx(m_hDC, &m_ViewSize);
	GetWindowOrgEx(m_hDC, &m_PtWinOrg);
	GetViewportOrgEx(m_hDC, &m_PtViewOrg);
	// GetClipBox(m_hDC, &m_RcClipBox); // done elsewhere

	// clipping
	SCCropToPlayBox();

	// 2. Add user-requested attributes
	// texts
	m_pGraphics->SetTextRenderingHint(m_DrawingAttributes.eTextRenderingHint);
	m_pGraphics->SetTextContrast(m_DrawingAttributes.uiTextContrast);

	// curves
	m_pGraphics->SetSmoothingMode(m_DrawingAttributes.eSmoothingMode);

	// images
	m_pGraphics->SetInterpolationMode(m_DrawingAttributes.eInterpolationMode);
	m_pGraphics->SetPixelOffsetMode(m_DrawingAttributes.ePixelOffsetMode);

	// paper color
	COLORREF PaperColor = m_DrawingAttributes.crPaperColor;
	if (m_DrawingAttributes.bReverseVideo)
	{
		if ((m_DrawingAttributes.dwInkingMode & SC_REVERSE_VIDEO_MASK)!=SC_REVERSE_VIDEO_SKIPIMAGES)
		{
			if (PaperColor!=RGB(255, 255, 255))
			{
				ColorMap clrMap;
				clrMap.oldColor = Color(255, 255, 255, 255);
				clrMap.newColor = Color(255, GetRValue(PaperColor), GetGValue(PaperColor), GetBValue(PaperColor));
				
				m_pImgAttribs = new ImageAttributes;
				if (m_pImgAttribs)
					m_pImgAttribs->SetRemapTable(1, &clrMap);
			}
		}
		if (RGB(255, 255, 255)==m_BkColor)
			m_BkColor = SC_RGBCOLOR(~PaperColor);
	} else
	{
		if ((m_DrawingAttributes.dwInkingMode & SC_TRANSPARENCY_MASK)==SC_TRANSPARENCY_WHITECOLOR)
		{
			if (PaperColor!=RGB(255, 255, 255))
			{
				ColorMap clrMap;
				clrMap.oldColor = Color(255, 255, 255, 255);
				clrMap.newColor = Color(255, GetRValue(PaperColor), GetGValue(PaperColor), GetBValue(PaperColor));
				
				m_pImgAttribs = new ImageAttributes;
				if (m_pImgAttribs)
					m_pImgAttribs->SetRemapTable(1, &clrMap);
			}
		}
		if (RGB(255, 255, 255)==m_BkColor)
			m_BkColor = SC_RGBCOLOR(PaperColor);
	}

	// printing
	if (m_DrawingAttributes.bPrinting)
	{
		m_pGraphics->SetPageUnit(UnitPixel);
		m_pGraphics->SetPageScale(m_DrawingAttributes.fPageScale);
	}

	// default objects
	HGDIOBJ hGdiObj;
	if (hGdiObj = GetCurrentObject(m_hDC, OBJ_PEN))
	{
		SCOnChangePen((HPEN)hGdiObj);
	}
	if (hGdiObj = GetCurrentObject(m_hDC, OBJ_BRUSH))
	{
		SCOnChangeBrush((HBRUSH)hGdiObj);
	}
	if (hGdiObj = GetCurrentObject(m_hDC, OBJ_FONT))
	{
		SCOnChangeFont((HFONT)hGdiObj);
	}

	// background
	if (m_DrawingAttributes.bBkSolid)
	{
		Color BrushColor;
		BrushColor.SetFromCOLORREF(m_BkColor);
		SolidBrush brush(BrushColor);

		CRect rcBox(m_RcClipBox);
		SCDPtoLP((Point*)&rcBox, 2);

		Rect rectClip(rcBox.left, rcBox.top, RECT_WIDTH(rcBox), RECT_HEIGHT(rcBox));
		rectClip.Inflate(1, 1);
		m_pGraphics->FillRectangle(&brush, rectClip);
	}
}

void CSCEMFdcRenderer::SCDPtoLP(Point* pPoints, INT iNbPts)
{
	ASSERT(pPoints);
	ASSERT(m_pGraphics);

	Matrix matrix;
	m_pGraphics->GetTransform(&matrix);
	matrix.Invert();
	matrix.TransformPoints(pPoints, iNbPts);
}

void CSCEMFdcRenderer::SCLPtoDP(Point* pPoints, INT iNbPts)
{
	ASSERT(pPoints);
	ASSERT(m_pGraphics);

	Matrix matrix;
	m_pGraphics->GetTransform(&matrix);
	matrix.TransformPoints(pPoints, iNbPts);
}

#if 0
///
/// Update current position
///
/// Compiler bug?
/// Other compilation units won't see this inline if it's not exported
/// (definition is discarded when it's not called from this file).
/// Linker will complain. (Note: functions like SCCropToPlayBox doesn't show this problem).
inline void CSCEMFdcRenderer::SCUpdateCurPos(INT x, INT y)
{
	MoveToEx(m_hDC, x, y, NULL);
	m_PtCurPos.X = x;
	m_PtCurPos.Y = y;
}
/// To export it, use:
//	inline __declspec( dllexport ) void CSCEMFdcRenderer::SCUpdateCurPos(INT x, INT y)
/// But I'm not sure VC6 would really inline it.
#endif

///
///	Change color to conform to drawing attributes.
///
inline COLORREF CSCEMFdcRenderer::SCGetFinalColor(COLORREF crColor)
{
	if ((m_DrawingAttributes.dwInkingMode & SC_TRANSPARENCY_MASK)!=SC_TRANSPARENCY_NORMAL)
	{
		if (RGB(255, 255, 255)==SC_RGBCOLOR(crColor))
			return SC_FINAL_COLOR(m_DrawingAttributes.crPaperColor);
	}
	
	return SC_FINAL_COLOR(crColor);
}

///
///	Replace the current brush with a new one.
///
inline void CSCEMFdcRenderer::SCRefreshBrush(Brush* pBrush)
{
	ASSERT(m_pBrushHolder); // we are refreshing
	ASSERT(pBrush);			// can't refresh to NULL

	SCBrush* pSCBrush = m_pBrushHolder->SCDetachSubObj();
	ASSERT(pSCBrush);

	SC_OBJCONTAINER_RELEASE_T(m_pBrushHolder, m_pBrush, GDPBrush, SCBrush);
	m_pBrush = pBrush;
	SC_OBJCONTAINER_RENEW_T(m_pBrushHolder, m_pBrush, GDPBrush, pSCBrush, SCBrush);
}

///////////////////////////////////////////////////////////////////////////////////
// I_EMFRasterizer
///////////////////////////////////////////////////////////////////////////////////

///
///	Create the unique graphic context for the rendering session.
/// hDestDC is where the raster is written.
/// hSrcDC is a reference DC from which information is collected
///
BOOL CSCEMFdcRenderer::SCBeginRendering(HDC hDestDC, HDC hSrcDC)
{
	m_hDC = hSrcDC;
	
	// Copy the initial source transformation to the destination
	if (hDestDC != hSrcDC)
	{
		// TODO: revise this when DC cloning is ractivated

		// Combine with the initial source transformation
		XFORM xformSrc;
		GetWorldTransform(m_hDC, &xformSrc);
		int iGM = SetGraphicsMode(hDestDC, GM_ADVANCED);
		
		ModifyWorldTransform(hDestDC, &xformSrc, MWT_LEFTMULTIPLY);
		
		// should fail if current matrix is not identity
		SetGraphicsMode(hDestDC, iGM);

		// Save original transformation of destination
		XFORM xform;
		GetWorldTransform(hDestDC, &xform);
		m_DestMatrix.SetElements(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy); 
	}
	
	// Compute clip box to use as metaregion
	{
#pragma message( __FILE__  "(335): TODO: Remove MFC dependency ")
		// For now it is needed to render into GDI metafiles
		CDC* pDC = CDC::FromHandle(hDestDC);
		HDC hAttribDC = pDC->m_hAttribDC;
		ASSERT(hAttribDC);
		
		HRGN hRgn = CreateRectRgn(0,0,0,0);
		int iRes = GetMetaRgn(hAttribDC, hRgn);
		ASSERT(iRes||(hAttribDC==hDestDC));
		if (!iRes)
		{// Sadly the meta region isn't meta enough. GetMetaRgn fails if you forget
			// to set a clipping region before calling it.
			iRes = GetClipBox(hAttribDC, &m_RcClipBox); // No! it's good for screen only
		} else
		{
			iRes = GetRgnBox(hRgn, &m_RcClipBox);
			DPtoLP(hAttribDC, (POINT*)&m_RcClipBox, 2); // warning: rounding errors here
			ASSERT(NULLREGION!=iRes && ERROR!=iRes); // there must be a rectangle
		}
		DeleteObject(hRgn);
	}
	
	SCNormalizeRect(&m_RcClipBox);
	if (IsRectEmpty(&m_RcClipBox))
	{// Set 'infinite' metaregion
		SetRect(&m_RcClipBox, -32767, -32767, 32767, 32767);
	}
	
	// Create GDI+ output surface
	SMC_DELETE(m_pGraphics);
	if (!SCCreateSurface(hDestDC))
	{
		m_dwLastError = SC_EMFERR_OUTOFMEMORY;
		return FALSE;
	}
	SCLPtoDP((Point*)&m_RcClipBox, 2); // we need it in target (GDI+) device coordinates

	// Setup graphic state (get other attributes from source DC)
	SCSetupRendering();
	return m_pGraphics->GetLastStatus()==Ok;
}

///
///	Push the current graphic state.
///
void CSCEMFdcRenderer::SCOnDCSaved()
{
	ASSERT(m_pGraphics);

	// Lock the objects matching GDI DC objects
	CSCGDIState* pStat = new CSCGDIState(
		m_pPenHolder,
		m_pBrushHolder,
		m_pFontHolder,
		m_pPathHolder,
		m_PtCurPos,
		m_pGraphics->Save(),
		m_dwMapMode,
		m_dwBkMode,
		m_dwFillMode,
		m_dwStretchBltMode,
		m_dwTextAlign,
		m_dwROP2,
		m_BkColor,
		m_TextColor,
		m_dwArcDirection,
		m_fMiterLimit,
		m_WinSize,
		m_ViewSize,
		m_PtWinOrg,
		m_PtViewOrg,
		m_RcClipBox
		);

	m_States.Add(pStat);
}

///
///	Pop the indicated graphic state, and discard all states from its index to top of stack.
///
void CSCEMFdcRenderer::SCOnDCRestored(INT iRelative)
{
	ASSERT(m_pGraphics);

	INT iPos;
	if (iRelative<0)
		iPos = m_States.GetSize() + iRelative;	// count from top of stack
	else
		iPos = iRelative;						// count from bottom of stack
	//ASSERT(iPos>=0 && iPos<m_States.GetSize());
	if (iPos<0 || iPos>=m_States.GetSize())
		return;

	// GDI+ state
	CSCGDIState* pStat = m_States[iPos];
	m_pGraphics->Restore(pStat->m_SvdGraphicsState);

	// Additional state information
		// Get flat data 
	m_PtCurPos = pStat->m_PtSvdCurPos;
	m_dwMapMode = pStat->m_dwSvdMapMode;
	m_dwBkMode = pStat->m_dwSvdBkMode;
	m_dwFillMode = pStat->m_dwSvdFillMode;
	m_dwStretchBltMode = pStat->m_dwSvdStretchBltMode;
	m_dwTextAlign = pStat->m_dwSvdTextAlign;
	m_dwROP2 = pStat->m_dwSvdROP2;
	m_BkColor = pStat->m_SvdBkColor;
	m_TextColor = pStat->m_SvdTextColor;
	m_dwArcDirection = pStat->m_dwSvdArcDirection;
	m_fMiterLimit = pStat->m_fSvdMiterLimit;
	m_WinSize = pStat->m_SvdWinSize;
	m_ViewSize = pStat->m_SvdViewSize;
	m_PtWinOrg = pStat->m_PtSvdWinOrg;
	m_PtViewOrg = pStat->m_PtSvdViewOrg;
	CopyRect(&m_RcClipBox, &pStat->m_RcSvdClipBox);

		// Get objects 
	SC_OBJHOLDER_RESTORE_T(m_pPenHolder, m_pPen, pStat->m_pSvdPenHolder, GDPPen);
	SC_OBJCONTAINER_RESTORE_T(m_pBrushHolder, m_pBrush, pStat->m_pSvdBrushHolder, GDPBrush, SCBrush);
	SC_OBJHOLDER_RESTORE_T(m_pFontHolder, m_pFont, pStat->m_pSvdFontHolder, SCFont);
	SC_OBJHOLDER_RESTORE_T(m_pPathHolder, m_pPath, pStat->m_pSvdPathHolder, SCPath);

	// Delete all states, starting from iPos
	INT iLen = m_States.GetSize();
	for (INT i=iPos; (i<iLen); i++)
	{
		delete m_States[i];
	}
	m_States.RemoveAt(iPos, iLen - iPos);

#ifdef _DEBUG
	// Check dangling pointers
	SC_CHECKGDIP_OBJ(m_pPen);
	SC_CHECKGDIP_OBJ(m_pBrush);
	SC_CHECKGDIP_OBJ(m_pFont);
	SC_CHECKGDIP_OBJ(m_pPath);
#endif
}

///
///	Move the current position.
///
void CSCEMFdcRenderer::SCMoveToEx(POINTL& PtDest)
{
	if (m_pPath)
	{
		if (m_pPath->SCGetEnd())
			SC_OBJHOLDER_RELEASE_T(m_pPathHolder, m_pPath, SCPath);
		else
			m_pPath->StartFigure();
	}
	m_PtCurPos.X = PtDest.x;
	m_PtCurPos.Y = PtDest.y;
}


///
///	Change the background mode and update hatched brush transparency.
///
void CSCEMFdcRenderer::SCSetBkMode(DWORD dwMode)
{
	ASSERT(m_pGraphics);
	m_dwBkMode = dwMode;
	if (m_pBrush && m_pBrush->GetType()==BrushTypeHatchFill)
	{// We must update transparency
		SCColor FgColor;
		((HatchBrush*)m_pBrush)->GetForegroundColor(&FgColor);

		SCColor BkColor;
		((HatchBrush*)m_pBrush)->GetBackgroundColor(&BkColor);
		BkColor.SCSetAlpha(((TRANSPARENT==m_dwBkMode) ? 0 : 255));

		HatchBrush* pBrush = new HatchBrush(((HatchBrush*)m_pBrush)->GetHatchStyle(), FgColor, BkColor);

		// Refresh the brush object
		SCRefreshBrush(pBrush);
	}
}

///
///	Change the text color and update monochrome brush color.
///
void CSCEMFdcRenderer::SCSetBkColor(COLORREF BkColor)
{
	m_BkColor = SCGetFinalColor(BkColor);

	if (!m_pBrushHolder)
		return;

	SCBrush* pSCBrush = m_pBrushHolder->SCGetSubObj();
	ASSERT(pSCBrush);

	if (!pSCBrush->SCIsMonoPattern())
		return;

	HBRUSH hBrush;
	if (hBrush = pSCBrush->SCGetHandle())
	{
		SCOnChangeBrush(hBrush);
	}
}

///
///	Change the background color and update monochrome brush color.
///
void CSCEMFdcRenderer::SCSetTextColor(COLORREF TxtColor)
{
	m_TextColor = SCGetFinalColor(TxtColor);

	if (!m_pBrushHolder)
		return;

	SCBrush* pSCBrush = m_pBrushHolder->SCGetSubObj();
	ASSERT(pSCBrush);

	if (!pSCBrush->SCIsMonoPattern())
		return;

	HBRUSH hBrush;
	if (hBrush = pSCBrush->SCGetHandle())
	{
		SCOnChangeBrush(hBrush);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Drawing objects
//

///
///	Convert the newly selected pen in m_hDC to its corresponding GDI+ object.
///
void CSCEMFdcRenderer::SCOnChangePen(HPEN hPen)
{
	ASSERT(m_pGraphics);
	SC_OBJHOLDER_RELEASE_T(m_pPenHolder, m_pPen, GDPPen);

	ASSERT(hPen);
	LOGPEN LogPen;
	if (::GetObject(hPen, sizeof(LogPen), &LogPen))
	{
		LogPen.lopnColor = SCGetFinalColor(LogPen.lopnColor);

		m_pPen = SCPenFromLogPen(LogPen);
	} else
	{// most likely the handle is not valid (maybe a deleted object)
		TRACE0("Invalid pen handle in DC");
	}

	SC_OBJHOLDER_RENEW_T(m_pPenHolder, m_pPen, GDPPen);
}

///
///	Convert the newly selected pen in m_hDC to its corresponding GDI+ object.
///
void CSCEMFdcRenderer::SCOnChangeExtPen(HPEN hPen)
{
	ASSERT(m_pGraphics);
	SC_OBJHOLDER_RELEASE_T(m_pPenHolder, m_pPen, GDPPen);

	ASSERT(hPen);
	DWORD dwSize = ::GetObject(hPen, sizeof(EXTLOGPEN), NULL);
	BYTE* pBytes = new BYTE[dwSize];
	if (::GetObject(hPen, dwSize, pBytes))
	{
		EXTLOGPEN* pExtLogPen = (EXTLOGPEN*)pBytes;
		if (BS_SOLID==pExtLogPen->elpBrushStyle || BS_HATCHED==pExtLogPen->elpBrushStyle)
		{
			pExtLogPen->elpColor = SCGetFinalColor(pExtLogPen->elpColor);
		}

		SCShortDCState TmpDCState(m_bMonochrome, m_dwROP2, m_TextColor, m_BkColor, m_dwBkMode);
		m_pPen = SCPenFromExtLogPen(pExtLogPen, TmpDCState);

		// Since a GDI+ pen has its own matrix, scaling problems may arise some day
		// (see comments in SCOnChangeBrush). In that event, think about something like:
		//		m_pPen->SetTransform(&BogusMatrix);
		// This looks as if, after giving the width of a pen, we should give a matrix
		// to scale it. Poor us, we don't have such information.
	}
	else
	{// most likely the handle is not valid (maybe a deleted object)
		TRACE0("Invalid extpen handle in DC");
	}
	delete [] pBytes;
	SC_OBJHOLDER_RENEW_T(m_pPenHolder, m_pPen, GDPPen);
}


///
///	Convert the newly selected brush in m_hDC to its corresponding GDI+ object.
///
void CSCEMFdcRenderer::SCOnChangeBrush(HBRUSH hBrush)
{
	ASSERT(m_pGraphics);
	SC_OBJCONTAINER_RELEASE_T(m_pBrushHolder, m_pBrush, GDPBrush, SCBrush);
	
	ASSERT(hBrush);
	LOGBRUSH LogBrush;
	LogBrush.lbHatch = 0;
	BOOL bMonoBrush = FALSE;
	if (::GetObject(hBrush, sizeof(LogBrush), &LogBrush))
	{
		if (BS_SOLID==LogBrush.lbStyle)
		{
			LogBrush.lbColor = SCGetFinalColor(LogBrush.lbColor);
		}
		SCShortDCState TmpDCState(m_bMonochrome, m_dwROP2, m_TextColor, m_BkColor, m_dwBkMode);

		m_pBrush = SCBrushFromLogBrush(LogBrush, TmpDCState);

		bMonoBrush = TmpDCState.bMonoBrush; // for brush update

#if 1
	// Strange!!
		if (m_pBrush && m_pBrush->GetType()==BrushTypeTextureFill)
		{
			// Note: When the brush is used, GDI+ won't use the transform in m_pGraphics
			// to properly display the brush.
			// I must exerce a strong moral on myself to just say that:
			// - GDI+ is a BS_... PATTERN!

			// Back to civilization.
			// Try to guess what these lines are doing.
			Matrix CurMatrix;
			m_pGraphics->GetTransform(&CurMatrix);
			if (CurMatrix.IsInvertible())
			{
				// Note: unexpected results in perspective, as we are setting up this
				// brush with the CURRENT state of m_pGraphics
				CurMatrix.Invert();
				((TextureBrush*)m_pBrush)->SetTransform(&CurMatrix);
			} else
			{ // else, let things fall down
				ASSERT(0);
			}
		}
#endif
	} else
	{// most likely the handle is not valid (maybe a deleted object)
		TRACE0("Invalid brush handle in DC");
	}
	SCBrush* pSCbrush = m_pBrush ? new SCBrush(hBrush, bMonoBrush) : NULL;
	SC_OBJCONTAINER_RENEW_T(m_pBrushHolder, m_pBrush, GDPBrush, pSCbrush, SCBrush);

	// Brush origin should be set. But it seems to have no effect on GDI+.
	// See comments in SCSetBrushOrg.
}

void CSCEMFdcRenderer::SCSetBrushOrg(POINTL& PtOrg)
{
	ASSERT(m_pGraphics);
#if 0
	// won't work? Maybe a device/world units problem (PtOrg is in device units)
	m_pGraphics->SetRenderingOrigin(PtOrg.x, PtOrg.y);
#else
	// won't work either. TODO: find the bug.
	Point Pts[2];
	Pts[1].X = PtOrg.x;
	Pts[1].Y = PtOrg.y;
	Pts[0].X = 0;
	Pts[0].Y = 0;
	
	Matrix matrix;
	m_pGraphics->GetTransform(&matrix);
	matrix.Invert();
	matrix.TransformPoints((Point*)&Pts, 2);
	Pts[1].X -= Pts[0].X;
	Pts[1].Y -= Pts[0].Y;
	
	m_pGraphics->SetRenderingOrigin(Pts[1].X, Pts[1].Y);
#endif
	// Origin should be stored for brush update. But since SetRenderingOrigin is called
	// on GDPGraphics, not on the brush, the setting is likely to stay until the next
	// SCSetBrushOrg call.
}


void CSCEMFdcRenderer::SCSelectPrivateFont(LOGFONT& rLogFont, SCFontCollection* pFontCollection, DWORD dwSimul)
{
	ASSERT(pFontCollection);

	// VC6 compiler won't accept the assignment OR with an enum (and that's correct).
	// FontStyle Style = FontStyleRegular;
	INT Style = FontStyleRegular;
	if (rLogFont.lfWeight>=FW_BOLD)
		Style = FontStyleBold;
	if (rLogFont.lfItalic)
		Style |= FontStyleItalic;
#if 0
	// You know what? The text would disappear as well (see attributes simulation).
	if (rLogFont.lfUnderline)
		Style |= FontStyleUnderline;
	if (rLogFont.lfStrikeOut)
		Style |= FontStyleStrikeout;
#endif

#ifdef _UNICODE
		WCHAR*	wFaceName = (WCHAR*)rLogFont.lfFaceName;
#else
		WCHAR	wFaceName[LF_FACESIZE];
		MultiByteToWideChar(CP_ACP, 0, (LPSTR)rLogFont.lfFaceName, LF_FACESIZE, wFaceName, LF_FACESIZE);
#endif

	TEXTMETRIC tm;
	GetTextMetrics(m_hDC, &tm);
	m_pFont = new SCFont(wFaceName, (REAL)(tm.tmHeight - tm.tmInternalLeading), (FontStyle)Style, pFontCollection);
	if (m_pFont)
	{
		if (m_pFont->IsAvailable())
		{
			m_pFont->SCFinalInit(m_hDC, rLogFont.lfEscapement, dwSimul, NULL);
			SC_OBJHOLDER_RENEW_T(m_pFontHolder, m_pFont, SCFont);
		} else
		{
			SMC_SAFEDELETE(m_pFont);
		}
	}
}

///
///	Convert the newly selected font in m_hDC to its corresponding GDI+ object.
///
void CSCEMFdcRenderer::SCOnChangeFont(HFONT hFont)
{
	ASSERT(m_pGraphics);
	SC_OBJHOLDER_RELEASE_T(m_pFontHolder, m_pFont, SCFont);

	// get the logfont
	ASSERT(hFont);
	LOGFONT LogFont;
	INT iRes = ::GetObject(hFont, sizeof(LogFont), &LogFont);
	if (!iRes)
	{// most likely the handle is not valid (maybe a deleted object)
		TRACE0("Invalid font handle in DC");
		SC_OBJHOLDER_RENEW_T(m_pFontHolder, m_pFont, SCFont);
		return;
	}

	ASSERT(abs(LogFont.lfEscapement)<=3600);

	// TODO: solve the case of GM_ADVANCED. For now, assume GM_COMPATIBLE.
	ASSERT((LogFont.lfEscapement==LogFont.lfOrientation)||
		   (GetGraphicsMode(m_hDC)==GM_ADVANCED));

	DWORD dwSimul = 0;
	// 0) Check attributes
	// We can't do:
	// m_pFont = new SCFont(m_hDC);
	// GDI+ does not support underlined/overstriked/symbol fonts (this is not a joke).
	// We must remove unsupported attributes. Otherwise, the text would simply disappear,
	// though m_pFont->IsAvailable() would return TRUE.
	if (LogFont.lfUnderline)
	{
		LogFont.lfUnderline = 0;
		dwSimul |= SC_FONT_SIMUL_UNDERLINE;
		// I know, I can use FontStyleUnderline in a FontStyle.
		// But, what am I doing here? Am I creating a font? What is a font now?
	}
	if (LogFont.lfStrikeOut)
	{
		LogFont.lfStrikeOut = 0;
		dwSimul |= SC_FONT_SIMUL_OVERSTRIKE;
		// I know, I can use FontStyleStrikeout in a FontStyle.
	}

	// 1) Check application installed fonts
	// GDI+ is unable to recognize a temporary/private GDI TT font installed by the
	// application. It bogusly replies: 'NotTrueTypeFont!'
	// So the caller must install temporary fonts in a SCFontCollection and pass it
	// to the renderer. And we check this collection first.
	if (m_pFontCollection &&
		m_pFontCollection->SCIsInstalledFacename((LPTSTR)LogFont.lfFaceName))
	{
		SCSelectPrivateFont(LogFont, m_pFontCollection, dwSimul);
		ASSERT(m_pFont);
		return;
	}

	// 2) Check DC font. Don't forget, We can't do: m_pFont = new SCFont(m_hDC);
	m_pFont = new SCFont(m_hDC, &LogFont);
	if (m_pFont)
	{
		if (m_pFont->IsAvailable())
		{
			m_pFont->SCFinalInit(m_hDC, LogFont.lfEscapement, dwSimul, NULL);
			SC_OBJHOLDER_RENEW_T(m_pFontHolder, m_pFont, SCFont);
			return;
		}
		SMC_SAFEDELETE(m_pFont);
	}

#if 1
	// 3) Attempt copying TT font in local private collection
	ASSERT(!m_pFont);
	ASSERT(g_pFontCopies);
	
	BOOL bFound = g_pFontCopies->SCIsInstalledFacename((LPTSTR)LogFont.lfFaceName);
	if (!bFound)
	{
		long lDataSize = ::GetFontData(m_hDC, 0, 0, NULL, 0);
		if (GDI_ERROR==lDataSize)
		{
			DWORD dwError = ::GetLastError();
			//ASSERT(0);
		} else
		{// read font data
			LPBYTE pData = new BYTE[lDataSize];
			ASSERT(pData);
			if (pData)
			{
				if (bFound=(::GetFontData(m_hDC, 0, 0, pData, lDataSize)>0))
				{
					g_pFontCopies->AddMemoryFont(pData, lDataSize);
				}
				delete [] pData;
				ASSERT(bFound);
			}
		}
	}
	if (bFound)
		SCSelectPrivateFont(LogFont, g_pFontCopies, dwSimul);
#endif

	// 4) Last chance: approximate font.
	//    We are lived to the good will of GDI to give us a TT font
	if (!m_pFont)
	{
		HFONT hFontSubs = SCTTFontFromLOGFONT(LogFont, m_hDC);
		// hFontSubs is selected
		m_pFont = new SCFont(m_hDC, &LogFont);

		// along with other attributes, store hFontSubs for deletion
		m_pFont->SCFinalInit(m_hDC, LogFont.lfEscapement, dwSimul, hFontSubs);
		SC_OBJHOLDER_RENEW_T(m_pFontHolder, m_pFont, SCFont);
	}
	// Yet GDI+ may refuse it
	ASSERT(m_pFont && m_pFont->IsAvailable());
}

///
///	Convert the newly selected palette in m_hDC to its corresponding GDI+ object.
///
void CSCEMFdcRenderer::SCOnChangePalette(HPALETTE hPal)
{
#pragma message( __FILE__  "(880): TODO: Palette management.")
}


///////////////////////////////////////////////////////////////////////////////////
/// Transforms

///
/// Force the transform to be exactly what is given
///
void CSCEMFdcRenderer::SCApplyTransform(XFORM& xform)
{
	ASSERT(m_pGraphics);
	Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
	m_pGraphics->SetTransform(&matrix);
}

///
///	Combine World to Page and Page to Device into a single World to Device transform
/// (since we don't have the possibility to make the distinction)
///
void CSCEMFdcRenderer::SCSetWorldTransform(XFORM& xform)
{
	ASSERT(m_pGraphics);

	// World to page
	Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
#if 1
	// Page to device
	POINT PtOrg;
	PtOrg.x = MulDiv(m_PtViewOrg.x, m_WinSize.cx, m_ViewSize.cx) - m_PtWinOrg.x;
	PtOrg.y = MulDiv(m_PtViewOrg.y, m_WinSize.cy, m_ViewSize.cy) - m_PtWinOrg.y;
	Matrix MatOrg(1, 0, 0, 1, (REAL)PtOrg.x, (REAL)PtOrg.y);

	matrix.Multiply(&MatOrg);
#endif
	m_pGraphics->SetTransform(&matrix);
}

///
///	.
///
void CSCEMFdcRenderer::SCModifyWorldTransform(XFORM& xform, DWORD dwMode)
{
	ASSERT(m_pGraphics);

#if 0
	// Won't work. We have seen dwMode=4 (?), exceeding MWT_MAX;
	if (MWT_IDENTITY==dwMode)
	{
		m_pGraphics->ResetTransform();
	} else
	{
		Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
		m_pGraphics->MultiplyTransform(&matrix, (MWT_LEFTMULTIPLY==dwMode)? MatrixOrderPrepend : MatrixOrderAppend);
	}
#else
	// So we just ignore bad values.
	// ASSERT(dwMode>=MWT_MIN && dwMode<=MWT_MAX); // Would fail too often
	switch (dwMode)
	{
	case MWT_IDENTITY:
		m_pGraphics->ResetTransform();
		//SCOnExtentChanged();
		break;

	case MWT_LEFTMULTIPLY:
		{
			Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
			m_pGraphics->MultiplyTransform(&matrix, MatrixOrderPrepend);
		}
		break;

	case MWT_RIGHTMULTIPLY:
		{
			Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
			m_pGraphics->MultiplyTransform(&matrix, MatrixOrderAppend);
		}
		break;

	case 4:
		{// Maybe a printer driver calling a Microsoft private, hidden, function into GDI
			
			// Tests (but how on Earth could it work?)
			// 1) do nothing => no image
			// 2) reset  => big image
			// 3) append  => no image
			// 4) prepend  => no image

//				// 5) panic, panic
//				XFORM xformp;
//				if (GetWorldTransform(m_hDC, &xformp))
//				{
//					//Matrix matrix(xformp.eM11, xformp.eM12, xformp.eM21, xformp.eM22, xformp.eDx, xformp.eDy);
//					//m_pGraphics->SetTransform(&matrix);
//					break;
//				} // else sing "Desperado..."
		}
		// the man is falling through

	default:
		{
			//ASSERT(0);
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////
/// Mapping modes and scaling

void CSCEMFdcRenderer::SCSetMapMode(DWORD dwMode)
{
	m_dwMapMode = dwMode;
	switch (m_dwMapMode)
	{
	case MM_ANISOTROPIC:
		return;

	case MM_TEXT:
        m_WinSize.cx	= 1;
        m_WinSize.cy	= 1;
		m_ViewSize.cx	= 1;
		m_ViewSize.cy	= 1;
		SCOnExtentChanged();
		return;
	}
#pragma message( __FILE__  "(997): TODO: use destination DC, not m_hDC ")
	INT horzSize = GetDeviceCaps( m_hDC, HORZSIZE ); // in mm
	INT vertSize = GetDeviceCaps( m_hDC, VERTSIZE ); // in mm
	INT horzRes  = GetDeviceCaps( m_hDC, HORZRES );  // in pixels
	INT vertRes  = GetDeviceCaps( m_hDC, VERTRES );  // in pixels
	
	switch (m_dwMapMode)
	{
	case MM_LOMETRIC:
        m_WinSize.cx	= horzSize;
        m_WinSize.cy	= vertSize;
        m_ViewSize.cx	= horzRes / 10;
        m_ViewSize.cy	= -vertRes / 10;
        break;

    case MM_ISOTROPIC:
		{
			// MSDN: "Coordinate Mapping"
			// yVE = (yWE * ((xVE * xA) / yA)) / xWE
			//	where:
			//	
			//	yVE : viewport y-extent 
			//	yWE : window y-extent 
			//	xVE : viewport x-extent 
			//	xWE : window x-extent 
			//	xA : device x-aspect (XASPECT index to GetDeviceCaps) 
			//	yA : device y-aspect (YASPECT index to GetDeviceCaps) 
			INT xA = GetDeviceCaps( m_hDC, ASPECTX );
			INT yA = GetDeviceCaps( m_hDC, ASPECTY );
			m_WinSize.cx	= horzSize;
			m_WinSize.cy	= vertSize;
			m_ViewSize.cx	= horzRes / 10;
			m_ViewSize.cy = -(INT)((m_WinSize.cy * ((m_ViewSize.cx * xA) / yA)) / m_WinSize.cx);
			// TODO: revise to keep the smallest between horzRes and vertRes, and reduce the other
		}
        break;

    case MM_HIMETRIC:
        m_WinSize.cx	= horzSize * 10;
        m_WinSize.cy	= vertSize * 10;
        m_ViewSize.cx	= horzRes / 10;
        m_ViewSize.cy	= -vertRes / 10;
        break;

    case MM_LOENGLISH:
        m_WinSize.cx	= horzSize;
        m_WinSize.cy	= vertSize;
        m_ViewSize.cx	= MulDiv(254, horzRes, 1000);
        m_ViewSize.cy	= -MulDiv(254, vertRes, 1000);
        break;

    case MM_HIENGLISH:
        m_WinSize.cx	= horzSize * 10;
        m_WinSize.cy	= vertSize * 10;
        m_ViewSize.cx	= MulDiv(254, horzRes, 1000);
        m_ViewSize.cy	= -MulDiv(254, vertRes, 1000);
        break;

    case MM_TWIPS:
        m_WinSize.cx	= MulDiv(144, horzSize, 10);
        m_WinSize.cy	= MulDiv(144, vertSize, 10);
        m_ViewSize.cx	= MulDiv(254, horzRes, 1000);
        m_ViewSize.cy	= -MulDiv(254, vertRes, 1000);
        break;

	default:
		ASSERT(0);
	}
	SCOnExtentChanged();
}

void CSCEMFdcRenderer::SCSetWindowOrg(INT iX, INT iY)
{
	ASSERT(m_pGraphics);
	m_PtWinOrg.x = iX;
	m_PtWinOrg.y = iY;
	SCOnExtentChanged();
}

void CSCEMFdcRenderer::SCSetViewportOrg(INT iX, INT iY)
{
	ASSERT(m_pGraphics);
	m_PtViewOrg.x = iX;
	m_PtViewOrg.y = iY;
	SCOnExtentChanged();
}

//	"When the following mapping modes are set, calls to the SetWindowExtEx and
//	SetViewportExtEx functions are ignored. 
//	
//	MM_HIENGLISH, MM_HIMETRIC, MM_LOENGLISH, MM_LOMETRIC, MM_TEXT, MM_TWIPS"
//

void CSCEMFdcRenderer::SCSetWindowExtent(INT iCx, INT iCy)
{
	ASSERT(m_pGraphics);

	switch (m_dwMapMode)
	{
	case MM_ANISOTROPIC:
		m_WinSize.cx = iCx;
		m_WinSize.cy = iCy;
		break;

	case MM_ISOTROPIC:
		m_WinSize.cx = iCx;
		m_WinSize.cy = iCy;
		// For MM_ISOTROPIC, adjust viewport extent
		// "An application must call the SetWindowExtEx function before it calls
		// SetViewportExtEx"
		{
			ASSERT(m_WinSize.cx);
			INT xA = GetDeviceCaps( m_hDC, ASPECTX );
			INT yA = GetDeviceCaps( m_hDC, ASPECTY );
			m_ViewSize.cx	= GetDeviceCaps( m_hDC, HORZRES ) / 10;
			m_ViewSize.cy = -(INT)((m_WinSize.cy * ((m_ViewSize.cx * xA) / yA)) / m_WinSize.cx);
		}
		break;

	// just to see
	case MM_TEXT:
		// and I saw MM_TEXT
		m_WinSize.cx = 1;
		m_WinSize.cy = 1;
		break;

	default:
		ASSERT(0); // just to see
	}

	SCOnExtentChanged();
}

//	"When the following mapping modes are set, calls to the SetWindowExtEx and
//	SetViewportExtEx functions are ignored. 
//	
//	MM_HIENGLISH, MM_HIMETRIC, MM_LOENGLISH, MM_LOMETRIC, MM_TEXT, MM_TWIPS"
//

void CSCEMFdcRenderer::SCSetViewportExtent(INT iCx, INT iCy)
{
	ASSERT(m_pGraphics);

	switch (m_dwMapMode)
	{
	case MM_ANISOTROPIC:
		m_ViewSize.cx = iCx;
		m_ViewSize.cy = iCy;
		break;

	case MM_ISOTROPIC:
		m_ViewSize.cx = iCx;
		m_ViewSize.cy = iCy;
		// For MM_ISOTROPIC, adjust viewport extent
		// "An application must call the SetWindowExtEx function before it calls
		// SetViewportExtEx"
#pragma message( __FILE__  "(1153): TODO: use destination DC ")
		{
			INT xA = GetDeviceCaps( m_hDC, ASPECTX );
			INT yA = GetDeviceCaps( m_hDC, ASPECTY );
			m_ViewSize.cy = -(INT)((m_WinSize.cy * ((m_ViewSize.cx * xA) / yA)) / m_WinSize.cx);
			// TODO: revise to keep the smallest between iCx and iCy, and reduce the other
		}
		break;

	// just to see
	case MM_TEXT:
		// and I saw MM_TEXT
		m_ViewSize.cx = 1;
		m_ViewSize.cy = 1;
		break;

	default:
		ASSERT(0); // just to see
	}

	SCOnExtentChanged();
}

void CSCEMFdcRenderer::SCScaleWindow(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom)
{
	ASSERT(m_pGraphics);
	if (MM_ANISOTROPIC==m_dwMapMode || MM_ISOTROPIC==m_dwMapMode)
	{// Note: We have no specification about these conditions (seem natural)
		m_WinSize.cx = MulDiv(m_WinSize.cx, lxNum, lxDenom);
		m_WinSize.cy = MulDiv(m_WinSize.cy, lyNum, lyDenom);
	}

#if 1
	// Normally, origin is not concerned (this seems to reproduce a GDI bug)
	m_PtWinOrg.x = MulDiv(m_PtWinOrg.x, lxNum, lxDenom);
	m_PtWinOrg.y = MulDiv(m_PtWinOrg.y, lyNum, lyDenom);
#endif

	SCOnExtentChanged();
}

void CSCEMFdcRenderer::SCScaleViewport(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom)
{
	ASSERT(m_pGraphics);
	if (MM_ANISOTROPIC==m_dwMapMode || MM_ISOTROPIC==m_dwMapMode)
	{// Note: We have no specification about these conditions (seem natural)
		m_ViewSize.cx = MulDiv(m_ViewSize.cx, lxNum, lxDenom);
		m_ViewSize.cy = MulDiv(m_ViewSize.cy, lyNum, lyDenom);
	}

#if 1
	// Normally, origin is not concerned (this seems to reproduce a GDI bug)
	m_PtViewOrg.x = MulDiv(m_PtViewOrg.x, lxNum, lxDenom);
	m_PtViewOrg.y = MulDiv(m_PtViewOrg.y, lyNum, lyDenom);
#endif

	SCOnExtentChanged();
}

void CSCEMFdcRenderer::SCOnExtentChanged()
{
	ASSERT(m_pGraphics);
	ASSERT(m_WinSize.cx && m_WinSize.cy);

	Matrix matrix;
	matrix.Scale(float(m_ViewSize.cx)/float(m_WinSize.cx), float(m_ViewSize.cy)/float(m_WinSize.cy));

	POINT PtOrg;
	PtOrg.x = MulDiv(m_PtViewOrg.x, m_WinSize.cx, m_ViewSize.cx) - m_PtWinOrg.x;
	PtOrg.y = MulDiv(m_PtViewOrg.y, m_WinSize.cy, m_ViewSize.cy) - m_PtWinOrg.y;

	matrix.Translate((REAL)PtOrg.x, (REAL)PtOrg.y);
	// Combine the new transform with the original one that
	// was in the destitation DC before rendering started.
#pragma message( __FILE__  "(1227): TODO: Investigation: revise the use of m_DestMatrix")
	matrix.Multiply(&m_DestMatrix);
	m_pGraphics->SetTransform(&matrix);
}

