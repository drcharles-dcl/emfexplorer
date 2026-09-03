/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDCRENDERER_H_
#define _SCEMFDCRENDERER_H_

#include "kSCEMFLibError.h"
#include "I_EMFRenderer.h"

#include "SCGdiplusUtils.h"
#include "SCGdiplusTypes.h"

#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenMath.h)
#include SC_INC_GENLIB(SCRefdObjHolder_T.h)
#include SC_INC_WINLIB(SCRectPoly.h)
#include SC_INC_WINLIB(SCGDIRops.h)

#include <float.h>
#include <afxtempl.h>

////////////////////////////////////////////////////////////////////////////////////
/// 
#define SC_FLOAT_EQ(x,v) (((v - FLT_EPSILON) < x) && (x <( v + FLT_EPSILON)))

#define SC_RGBCOLOR(color) (color & 0x00FFFFFF)
#define SC_FINAL_COLOR(color) m_DrawingAttributes.bReverseVideo ? SC_RGBCOLOR(~color) : SC_RGBCOLOR(color)

#define SC_FONT_SIMUL_UNDERLINE		0x01
#define SC_FONT_SIMUL_OVERSTRIKE	0x02

// Let this undefined if GDI has to do the ROPs in the parser
//	#define SC_RENDERER_CAN_DOROP

////////////////////////////////////////////////////////////////////////////////////
/// 
typedef SCRefdObjHolder<GDPPen>					SCPenHolder;
typedef SCRefdObjContainer<GDPBrush, SCBrush>	SCBrushHolder;
typedef SCRefdObjHolder<SCFont>					SCFontHolder;
typedef SCRefdObjHolder<SCPath>					SCPathHolder;
typedef SCRefdObjHolder<SCFontCollection>		SCFontCollectionHolder;

class SCRectPolygon;
class CSCGDIState;
typedef CTypedPtrArray<CPtrArray, CSCGDIState*> SCStatesArray;

class CSCEMFmetaDCRenderer;

////////////////////////////////////////////////////////////////////////////////////
/// Class to render on DC
class CSCEMFdcRenderer : public I_EMFRenderer,
						 public I_SCDCGraphics
{
// Construction/Destruction
public:
	CSCEMFdcRenderer();
	virtual ~CSCEMFdcRenderer();

// Attributes
	// getters

	// setters
	void SCSetDrawingAttributes(SCGDIpDrawingAttributes& rDrawingAttributes) { m_DrawingAttributes = rDrawingAttributes;}
	void SCSetFontCollection(SCFontCollection* pColl) { m_pFontCollection = pColl; }
	static void SCCleanFontCopies();

// Operation

// Implementation
	// I_EMFRenderer
	// Rendering device
	virtual BOOL SCBeginRendering(HDC hDestDC, HDC hSrcDC);
	virtual void SCEndRendering() {}

	// Curves and surfaces
	virtual void SCDrawLines(POINT* pPoints, DWORD dwCount);
	virtual void SCDrawLinesS(POINTS* pPoints, DWORD dwCount);
	virtual void SCDrawLinesTo(POINT* pPoints, DWORD dwCount);
	virtual void SCDrawLinesToS(POINTS* pPoints, DWORD dwCount);

	virtual void SCDrawBezier(POINT* pPoints, DWORD dwCount);
	virtual void SCDrawBezierTo(POINT* pPoints, DWORD dwCount);
	virtual void SCDrawBezierS(POINTS* pPoints, DWORD dwCount);
	virtual void SCDrawBezierToS(POINTS* pPoints, DWORD dwCount);

	virtual void SCDrawPolygon(POINT* pPoints, DWORD dwCount);
	virtual void SCDrawPolygonS(POINTS* pPoints, DWORD dwCount);

	virtual void SCDrawPolyPolygon(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys);
	virtual void SCDrawPolyPolygonS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys);
	virtual void SCDrawPolyPolyline(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys);
	virtual void SCDrawPolyPolylineS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys);
	virtual void SCDrawPolyDraw(POINT* pPoints, DWORD dwCount, BYTE *pTypes);
	virtual void SCDrawPolyDrawS(POINTS* pPoints, DWORD dwCount, BYTE *pTypes);

	virtual void SCDrawRectangle(LPCRECT pBox);
	virtual void SCDrawRoundRectangle(LPCRECT pBox, SIZE szlCorner);

	virtual void SCDrawEllipse(LPCRECT pBox);
	virtual void SCDrawAngleArc(POINT* pPtCenter, DWORD dwRadius, FLOAT fStartAngle, FLOAT fSweepAngle);
	virtual void SCDrawArc(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd);
	virtual void SCDrawChord(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd);
	virtual void SCDrawPie(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd);
	virtual void SCDrawArcTo(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd);

	// Texts
	virtual void SCDrawText(INT x, INT y, UINT uiOptions, LPCRECT pRect,
		LPCWSTR pwString, UINT uiCount, LPCINT pDxWidths, FLOAT fScaleX, FLOAT fScaleY);

	// Images
	virtual void SCDrawImage(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3, XFORM* pXForm=NULL);
	virtual void SCDrawImagePlg(LPCPOINTL p3PtDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, XFORM* pXForm, COLORREF crBkColorSrc,
		LONG lxMask, LONG lyMask, LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask);
	virtual void SCDrawImageMsk(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop4, XFORM* pXForm, COLORREF crBkColorSrc,
		LONG lxMask, LONG lyMask, LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask);
	virtual void SCDrawImageTransparent(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc);
	virtual void SCDrawImageAlpha(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc);
	virtual void SCDrawPixel(POINTL  ptlPixel, COLORREF crColor);

	virtual void SCPatBlt(LPCRECT pDest, DWORD dwRop3, XFORM* pXForm=NULL);
	virtual void SCGradientFill(TRIVERTEX* pVer, DWORD dwCountVer, LPCBYTE pMesh, DWORD dwCountMesh, DWORD dwMode);
	virtual void SCFloodFill(POINTL& rPtStart, COLORREF crColor, DWORD iMode);

	// Paths
	virtual void SCDrawPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, DWORD dwDrawType=SC_PATH_STROKEANDFILL);
	virtual void SCSetClipPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, INT iClipMode);

	virtual void SCBeginPath();
	virtual void SCCloseFigure();
	virtual void SCEndPath();
	virtual void SCApplyPath(DWORD dwDrawType=SC_PATH_STROKEANDFILL, INT iClipMode=RGN_COPY);
	virtual void SCFlattenPath();
	virtual void SCWidenPath();
	virtual void SCAbortPath();

	// Drawing objects
	virtual void SCOnChangePen(HPEN hPen);
	virtual void SCOnChangeExtPen(HPEN hPen);
	virtual void SCOnChangeBrush(HBRUSH hBrush);
	virtual void SCOnChangeFont(HFONT hFont);
	virtual void SCOnChangePalette(HPALETTE hPal);

	// Clipping
	virtual void SCIntersectClipRect(RECTL& rRect);
	virtual void SCExcludeClipRect(RECTL& rRect);
	virtual void SCOffsetClipRect(POINTL& rPtOffset);
	virtual void SCSelectClipRgn(LPCBYTE pData, DWORD dwSize, DWORD dwMode);

	// Region
	virtual void SCOnSetMetaRgn();
	virtual void SCFillRgn(HBRUSH hBrush, LPCBYTE pData, DWORD dwSize);
	virtual void SCFrameRgn(HBRUSH hBrush, SIZEL szlStroke, LPCBYTE pData, DWORD dwSize);
	virtual void SCInvertRgn(LPCBYTE pData, DWORD dwSize);
	virtual void SCPaintRgn(LPCBYTE pData, DWORD dwSize);

	// States and drawing modes
	virtual void SCOnDCSaved();
	virtual void SCOnDCRestored(INT iRelative);

	virtual void SCMoveToEx(POINTL& PtDest);

	virtual void SCSetMapMode(DWORD dwMode);
	virtual void SCSetBkMode(DWORD dwMode);
	virtual void SCSetPolyFillMode(DWORD dwMode)	{ m_dwFillMode = dwMode; }
	virtual void SCSetStretchBltMode(DWORD dwMode)	{ m_dwStretchBltMode = dwMode; }
	virtual void SCSetTextAlign(DWORD dwMode)		{ m_dwTextAlign = dwMode; }
	virtual void SCSetROP2(DWORD dwMode)			{ m_dwROP2 = dwMode; }
	//virtual void SCEnableICM(DWORD dwMode);

	virtual void SCSetBkColor(COLORREF BkColor);
	virtual void SCSetTextColor(COLORREF TxtColor);
	virtual void SCSetColorAsjustment(CONST COLORADJUSTMENT *lpca) { ASSERT(0);} // TODO;  

	virtual void SCSetArcDirection(DWORD dwMode)	{ m_dwArcDirection = dwMode; }

	virtual void SCSetMiterLimit(FLOAT fMiterLimit)	{ m_fMiterLimit = fMiterLimit; }

	virtual void SCSetBrushOrg(POINTL& PtOrg);

	// Transform
	virtual void SCApplyTransform(XFORM& xform);
	virtual void SCSetWorldTransform(XFORM& xform);
	virtual void SCModifyWorldTransform(XFORM& xform, DWORD dwMode);

	virtual void SCSetWindowOrg(INT iX, INT iY);
	virtual void SCSetViewportOrg(INT iX, INT iY);
	virtual void SCSetWindowExtent(INT iCx, INT iCy);
	virtual void SCSetViewportExtent(INT iCx, INT iCy);
	virtual void SCScaleWindow(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom);
	virtual void SCScaleViewport(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom);


	// I_RendererAttribs I_SCDCGraphics
	virtual GDPGraphics* SCGetGraphics() { return m_pGraphics; }
	virtual HDC SCGetDC()				{ return m_pGraphics->GetHDC(); }
	virtual void SCReleaseDC(HDC hdc)	{ m_pGraphics->ReleaseHDC(hdc); }
	virtual BOOL SCUsingMonochromeDC()	{ return m_bMonochrome; }
	virtual COLORREF SCGetTextColor()	{ return m_TextColor; }
	virtual COLORREF SCGetBkColor()		{ return m_BkColor; }
	virtual DWORD SCGetBkMode()			{ return m_dwBkMode; }
	virtual DWORD SCGetROP2()			{ return m_dwROP2; }
	virtual Brush* SCGetBrush()			{ return m_pBrush; }

	virtual COLORREF SCGetFinalColor(COLORREF crColor);
	virtual void SCDrawBitmap(Bitmap* pBmp, Rect& rRcDest);

// Data
protected:
	virtual BOOL SCCreateSurface(HDC hDestDC)
	{
		return ((m_pGraphics = new Graphics(hDestDC))!=NULL);
	}
	void SCSetupRendering();
	template <class T> void T_SCDrawPolygon(T* pPoints, DWORD dwCount);
	template <class T> void T_SCDrawLines(T* pPoints, DWORD dwCount);
	template <class T> void T_SCDrawPolyPoly(T* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys, BOOL bLines);
	template <class T> void T_SCDrawPolyDraw(T* pPoints, DWORD dwCount, BYTE *pTypes);

	void SCBuildTextRect(ULONG ulTextAlign, POINT PtRef,
		RECT& ClipRect, Gdiplus::RectF TextSize, INT itmAscent);
	void SCComputeTextBBox(ULONG ulTextAlign, Gdiplus::RectF TextSize,
		SCRectPolygon& RectPolygon, POINT& PtRef, INT itmAscent);
	INT SCComputeTextWidth(LPCWSTR pwString, INT iCount, CONST INT* pDxWidths, INT flags);
	void SCOnExtentChanged();
	HRGN SCGetHRGNFromREGIONDATA(LPCBYTE pData, DWORD dwSize);
	void SCAddTextInPath(INT x, INT y, UINT uiOptions, LPCRECT pRect, LPCWSTR pwString,
								UINT uiCount, LPCINT pDxWidths, FLOAT fScaleX, FLOAT fScaleY);
	void SCSelectPrivateFont(LOGFONT& rLogFont, SCFontCollection* pFontCollection, DWORD dwSimul);

	void SCCropToPlayBox(CombineMode nCropMode=CombineModeReplace);
	void SCDPtoLP(Point* pPoints, INT iNbPts);
	void SCLPtoDP(Point* pPoints, INT iNbPts);
	// inlines
	inline void SCRefreshBrush(Brush* pBrush);
	inline void SCUpdateCurPos(INT x, INT y)
	{// See comments in cpp file
		MoveToEx(m_hDC, x, y, NULL);
		m_PtCurPos.X = x;
		m_PtCurPos.Y = y;
	}

#if 0
	// temp code for metaregion
	void SCOnChangeClipping();
#endif


private:
	friend class CSCEMFmetaDCRenderer; 

	DWORD					m_dwLastError;
	HDC						m_hDC;				// Source DC where records are played
	BOOL					m_bMonochrome;		// Source DC is monochrome
	SCStatesArray			m_States;			// Stack for save/restore DC
	GDPMatrix				m_DestMatrix;		// Original matrix of the destination

	// GDI+
	GDPGraphics*			m_pGraphics;
	SCFontCollection*		m_pFontCollection;

	// GDI DC state matching object
	GDPPen*					m_pPen;
	GDPBrush*				m_pBrush;
	SCFont*					m_pFont;
	SCPath*					m_pPath;

	// Holders
	SCPenHolder*			m_pPenHolder;
	SCBrushHolder*			m_pBrushHolder;
	SCFontHolder*			m_pFontHolder;
	SCPathHolder*			m_pPathHolder; // Just for consistency (see SCBeginPath).

	// Rest of GDI DC state
	GDPPoint				m_PtCurPos;
	DWORD					m_dwMapMode;
	DWORD					m_dwBkMode;
	DWORD					m_dwFillMode;
	DWORD					m_dwStretchBltMode;
	DWORD					m_dwTextAlign;
	DWORD					m_dwROP2;
	COLORREF				m_BkColor;
	COLORREF				m_TextColor;
	DWORD					m_dwArcDirection;
	FLOAT					m_fMiterLimit;
	SIZE					m_WinSize;
	SIZE					m_ViewSize;
	POINT					m_PtWinOrg;
	POINT					m_PtViewOrg;
	RECT					m_RcClipBox;

	// Extra stuff
	GDPImgAttribs*			m_pImgAttribs;
	SCGDIpDrawingAttributes m_DrawingAttributes;
};

// Class to save objects matching GDI DC objects
class CSCGDIState
{
	friend class CSCEMFdcRenderer;

		CSCGDIState(
				SCPenHolder*			pSvdPenHolder,
				SCBrushHolder*			pSvdBrushHolder,
				SCFontHolder*			pSvdFontHolder,
				SCPathHolder*			pSvdPathHolder,
				GDPPoint&				PtSvdCurPos,
				GDPGCState				SvdGraphicsState,
				DWORD					dwSvdMapMode,
				DWORD					dwSvdBkMode,
				DWORD					dwSvdFillMode,
				DWORD					dwSvdStretchBltMode,
				DWORD					dwSvdTextAlign,
				DWORD					dwSvdROP2,
				COLORREF				SvdBkColor,
				COLORREF				SvdTextColor,
				DWORD					dwSvdArcDirection,
				FLOAT					fSvdMiterLimit,
				SIZE					SvdWinSize,
				SIZE					SvdViewSize,
				POINT					PtSvdWinOrg,
				POINT					PtSvdViewOrg,
				RECT&					rRcSvdClipBox):
			m_pSvdPenHolder(pSvdPenHolder),
			m_pSvdBrushHolder(pSvdBrushHolder),
			m_pSvdFontHolder(pSvdFontHolder),
			m_pSvdPathHolder(pSvdPathHolder),
			m_PtSvdCurPos(PtSvdCurPos),
			m_SvdGraphicsState(SvdGraphicsState),
			m_dwSvdMapMode(dwSvdMapMode),
			m_dwSvdBkMode(dwSvdBkMode),
			m_dwSvdFillMode(dwSvdFillMode),
			m_dwSvdStretchBltMode(dwSvdStretchBltMode),
			m_dwSvdTextAlign(dwSvdTextAlign),
			m_dwSvdROP2(dwSvdROP2),
			m_SvdBkColor(SvdBkColor),
			m_SvdTextColor(SvdTextColor),
			m_dwSvdArcDirection(dwSvdArcDirection),
			m_fSvdMiterLimit(fSvdMiterLimit),
			m_SvdWinSize(SvdWinSize),
			m_SvdViewSize(SvdViewSize),
			m_PtSvdWinOrg(PtSvdWinOrg),
			m_PtSvdViewOrg(PtSvdViewOrg)
		{
			SC_REFCOUNTED_ADDREF(m_pSvdPenHolder);
			SC_REFCOUNTED_ADDREF(m_pSvdBrushHolder);
			SC_REFCOUNTED_ADDREF(m_pSvdFontHolder);
			SC_REFCOUNTED_ADDREF(m_pSvdPathHolder);

			CopyRect(&m_RcSvdClipBox, &rRcSvdClipBox);
		}
		
		~CSCGDIState()
		{
			SC_REFCOUNTED_RELEASE(m_pSvdPenHolder);
			SC_REFCOUNTED_RELEASE(m_pSvdBrushHolder);
			SC_REFCOUNTED_RELEASE(m_pSvdFontHolder);
			SC_REFCOUNTED_RELEASE(m_pSvdPathHolder);
		}
		
		SCPenHolder*			m_pSvdPenHolder;
		SCBrushHolder*			m_pSvdBrushHolder;
		SCFontHolder*			m_pSvdFontHolder;
		SCPathHolder*			m_pSvdPathHolder;
		GDPPoint				m_PtSvdCurPos;
		GDPGCState				m_SvdGraphicsState;
		DWORD					m_dwSvdMapMode;
		DWORD					m_dwSvdBkMode;
		DWORD					m_dwSvdFillMode;
		DWORD					m_dwSvdStretchBltMode;
		DWORD					m_dwSvdTextAlign;
		DWORD					m_dwSvdROP2;
		COLORREF				m_SvdBkColor;
		COLORREF				m_SvdTextColor;
		DWORD					m_dwSvdArcDirection;
		FLOAT					m_fSvdMiterLimit;
		SIZE					m_SvdWinSize;
		SIZE					m_SvdViewSize;
		POINT					m_PtSvdWinOrg;
		POINT					m_PtSvdViewOrg;
		RECT					m_RcSvdClipBox;
};

////////////////////////////////////////////////////////////////////////////////////
/// Class to render into metafiles
class CSCEMFmetaDCRenderer : public CSCEMFdcRenderer
{
// Construction/Destruction
public:
	CSCEMFmetaDCRenderer(GDPMetafile* pDestMeta):
	  CSCEMFdcRenderer(),
	  m_pMeta(pDestMeta)
	{
	}
	virtual ~CSCEMFmetaDCRenderer()
	{
	}

// Attributes
	// Getters

	// Setters

// Operation

// Implementation

protected:
	// Output device
	virtual BOOL SCCreateSurface(HDC hDestDC)
	{
		ASSERT(m_pMeta);

		m_pGraphics = new Graphics(m_pMeta);
		if (!m_pGraphics)
			return FALSE;

		// TODO: revise this when DC cloning is ractivated

		XFORM xform;
		GetWorldTransform(m_hDC, &xform);
		if (!SCIsIdentityXFORM(xform))
		{
			Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
			m_pGraphics->SetTransform(&matrix);
		}

		// Output goes to metafile. So we must copy destination transform
		// even if hDestDC==m_hDC. Becareful though to not apply the transformation twice.
		GetWorldTransform(hDestDC, &xform);
		m_DestMatrix.SetElements(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy); 

		return TRUE;
	}

// Data
private:
	GDPMetafile*	m_pMeta;

	// prohibited calls
	CSCEMFmetaDCRenderer();
};

#endif //_SCEMFDCRENDERER_H_
//  ------------------------------------------------------------
