/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _I_EMFRENDERER_H_
#define _I_EMFRENDERER_H_


#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)
#include SC_INC_WINLIB(SCGDIUtils.h)


// I_EMFRenderer
class SMC_INTERFACE I_EMFRenderer
{
public:
	// Rendering device
	virtual BOOL SCBeginRendering(HDC hDestDC, HDC hSrcDC) = 0;
	virtual void SCEndRendering() = 0;

	// Curves and surfaces
	virtual void SCDrawLines(POINT* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawLinesS(POINTS* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawLinesTo(POINT* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawLinesToS(POINTS* pPoints, DWORD dwCount) = 0;

	virtual void SCDrawBezier(POINT* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawBezierTo(POINT* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawBezierS(POINTS* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawBezierToS(POINTS* pPoints, DWORD dwCount) = 0;

	virtual void SCDrawPolygon(POINT* pPoints, DWORD dwCount) = 0;
	virtual void SCDrawPolygonS(POINTS* pPoints, DWORD dwCount) = 0;

	virtual void SCDrawPolyPolygon(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys) = 0;
	virtual void SCDrawPolyPolygonS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys) = 0;
	virtual void SCDrawPolyPolyline(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys) = 0;
	virtual void SCDrawPolyPolylineS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys) = 0;
	virtual void SCDrawPolyDraw(POINT* pPoints, DWORD dwCount, BYTE *pTypes) = 0;
	virtual void SCDrawPolyDrawS(POINTS* pPoints, DWORD dwCount, BYTE *pTypes) = 0;

	virtual void SCDrawRectangle(LPCRECT pBox) = 0;
	virtual void SCDrawRoundRectangle(LPCRECT pBox, SIZE szlCorner) = 0;

	virtual void SCDrawEllipse(LPCRECT pBox) = 0;
	virtual void SCDrawAngleArc(POINT* pPtCenter, DWORD dwRadius, FLOAT fStartAngle, FLOAT fSweepAngle) = 0;
	virtual void SCDrawArc(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd) = 0;
	virtual void SCDrawChord(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd) = 0;
	virtual void SCDrawPie(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd) = 0;
	virtual void SCDrawArcTo(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd) = 0;

	// Texts
	virtual void SCDrawText(INT x, INT y, UINT uiOptions, LPCRECT pRect,
		LPCWSTR pwString, UINT uiCount, LPCINT pDxWidths, FLOAT fScaleX, FLOAT fScaleY) = 0;

	// Images
	virtual void SCDrawImage(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3, XFORM* pXForm=NULL) = 0;
	virtual void SCDrawImagePlg(LPCPOINTL p3PtDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, XFORM* pXForm, COLORREF crBkColorSrc,
		LONG lxMask, LONG lyMask, LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask) = 0;
	virtual void SCDrawImageMsk(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop4, XFORM* pXForm, COLORREF crBkColorSrc,
		LONG lxMask, LONG lyMask, LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask) = 0;
	virtual void SCDrawImageTransparent(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc) = 0;
	virtual void SCDrawImageAlpha(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc) = 0;
	virtual void SCDrawPixel(POINTL  ptlPixel, COLORREF crColor) = 0;

	virtual void SCPatBlt(LPCRECT pDest, DWORD dwRop3, XFORM* pXForm=NULL) = 0;
	virtual void SCGradientFill(TRIVERTEX* pVer, DWORD dwCountVer, LPCBYTE pMesh, DWORD dwCountMesh, DWORD dwMode) = 0;
	virtual void SCFloodFill(POINTL& rPtStart, COLORREF crColor, DWORD iMode) = 0;

	// Paths
	virtual void SCDrawPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, DWORD dwDrawType=SC_PATH_STROKEANDFILL) = 0;
	virtual void SCSetClipPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, INT iClipMode) = 0;

	virtual void SCBeginPath() = 0;
	virtual void SCCloseFigure() = 0;
	virtual void SCEndPath() = 0;
	virtual void SCApplyPath(DWORD dwDrawType=SC_PATH_STROKEANDFILL, INT iClipMode=RGN_COPY) = 0;
	virtual void SCFlattenPath() = 0;
	virtual void SCWidenPath() = 0;
	virtual void SCAbortPath() = 0;

	// Drawing objects
	virtual void SCOnChangePen(HPEN hPen) = 0;
	virtual void SCOnChangeExtPen(HPEN hPen) = 0;
	virtual void SCOnChangeBrush(HBRUSH hBrush) = 0;
	virtual void SCOnChangeFont(HFONT hFont) = 0;
	virtual void SCOnChangePalette(HPALETTE hPal) = 0;

	// Clipping
	virtual void SCIntersectClipRect(RECTL& rRect) = 0;
	virtual void SCExcludeClipRect(RECTL& rRect) = 0;
	virtual void SCOffsetClipRect(POINTL& rPtOffset) = 0;
	virtual void SCSelectClipRgn(LPCBYTE pData, DWORD dwSize, DWORD dwMode) = 0;

	// Region
	virtual void SCOnSetMetaRgn() = 0;
	virtual void SCFillRgn(HBRUSH hBrush, LPCBYTE pData, DWORD dwSize) = 0;
	virtual void SCFrameRgn(HBRUSH hBrush, SIZEL szlStroke, LPCBYTE pData, DWORD dwSize) = 0;
	virtual void SCInvertRgn(LPCBYTE pData, DWORD dwSize) = 0;
	virtual void SCPaintRgn(LPCBYTE pData, DWORD dwSize) = 0;

	// States and drawing modes
	virtual void SCOnDCSaved() = 0;
	virtual void SCOnDCRestored(INT iRelative) = 0;

	virtual void SCMoveToEx(POINTL& PtDest)=0;

	virtual void SCSetMapMode(DWORD dwMode) = 0;
	virtual void SCSetBkMode(DWORD dwMode) = 0;
	virtual void SCSetPolyFillMode(DWORD dwMode) = 0;
	virtual void SCSetStretchBltMode(DWORD dwMode) = 0;
	virtual void SCSetTextAlign(DWORD dwMode) = 0;
	virtual void SCSetROP2(DWORD dwMode) = 0;
	//virtual void SCEnableICM(DWORD dwMode) = 0;

	virtual void SCSetBkColor(COLORREF BkColor) = 0;
	virtual void SCSetTextColor(COLORREF TxtColor) = 0;
	virtual void SCSetColorAsjustment(CONST COLORADJUSTMENT *lpca) = 0; 

	virtual void SCSetArcDirection(DWORD dwMode) = 0;

	virtual void SCSetMiterLimit(FLOAT fMiterLimit) = 0;

	virtual void SCSetBrushOrg(POINTL& PtOrg) = 0;

	// Transform
	virtual void SCApplyTransform(XFORM& xform) = 0;
	virtual void SCSetWorldTransform(XFORM& xform) = 0;
	virtual void SCModifyWorldTransform(XFORM& xform, DWORD dwMode) = 0;

	virtual void SCSetWindowOrg(INT iX, INT iY) = 0;
	virtual void SCSetViewportOrg(INT iX, INT iY) = 0;
	virtual void SCSetWindowExtent(INT iCx, INT iCy) = 0;
	virtual void SCSetViewportExtent(INT iCx, INT iCy) = 0;
	virtual void SCScaleWindow(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom) = 0;
	virtual void SCScaleViewport(LONG lxNum, LONG lxDenom, LONG lyNum, LONG lyDenom) = 0;
};

#endif //_I_EMFRENDERER_H_
//  ------------------------------------------------------------
