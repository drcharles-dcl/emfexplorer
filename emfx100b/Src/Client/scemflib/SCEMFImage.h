/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFIMAGE_H_
#define _SCEMFIMAGE_H_

#include "SCEMFDoc.h"
#include "SCEMFViewDefs.h"
#include "SCGdiplusUtils.h"
#include "SCEMFConverter.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCMemDC.h)

class I_EMFImageOwner;

// Uncomment this line to paint client area directly
//	#define SC_NO_MEMDC

class CSCEMFImage : public CWnd
{
// Construction/Derstruction
public:
	CSCEMFImage();
	virtual ~CSCEMFImage();
	DECLARE_DYNCREATE(CSCEMFImage)

// Attributes
public:
	//=== Getters =======================================================
		//--- properties ------------------------------------------------
	long SCGetCtlBorderType() const;
	long SCGetCtlColorStyle() const;
	COLORREF SCGetCtlColor() const { return m_crCtlColor; }

	long SCGetPageBorderVisible() const { return ((m_dwColorScheme & SC_CSM_BORDER_MASK)==SC_CSM_BORDER_YES); }
	long SCGetPageShadowVisible() const { return ((m_dwColorScheme & SC_CSM_SHADOW_MASK)==SC_CSM_SHADOW_YES); }
	long SCGetPaperColorStyle() const;
	COLORREF SCGetPaperColor() const { return m_crPaperColor; }
	int SCGetFitMode() const { return m_iFitMode; }
	int SCGetCurZoom() const { return m_iCurZoom; }
	long SCGetLMargin() const { return m_rectMargins.left; }
	long SCGetTMargin() const { return m_rectMargins.top; }
	long SCGetRMargin() const { return m_rectMargins.right; }
	long SCGetBMargin() const { return m_rectMargins.bottom; }
	void SCGetMargins(CRect& rc) const { rc = m_rectMargins; }
	int SCGetCurAngle() { return m_iAngle; } // PageOrientation
	long SCGetTransparencyMode() const { return m_DrawingAttributes.dwInkingMode & SC_TRANSPARENCY_MASK; }
	long SCGetReverseVideoMode() const { return m_DrawingAttributes.dwInkingMode & SC_REVERSE_VIDEO_MASK; }

	BOOL SCGetGDIplusEnabled() const { return (SC_ENGINE_GDIP==m_uiRasEngine); }
	int SCGetTextRenderingHint() const { return m_DrawingAttributes.eTextRenderingHint; }
	UINT SCGetTextContrast() const { return m_DrawingAttributes.uiTextContrast; }
	int SCGetSmoothingMode() const { return m_DrawingAttributes.eSmoothingMode; }
	int SCGetInterpolationMode() const { return m_DrawingAttributes.eInterpolationMode; }
	int SCGetPixelOffsetMode() const { return m_DrawingAttributes.ePixelOffsetMode; }
		//--- extra -----------------------------------------------------
	float SCGetScale() const { return float(m_iCurZoom)/float(SC_ZOOM100);}
	int SCGetReverseVideo() const { return m_bReverseVideo; }
	UINT SCGetRasEngine() const { return m_uiRasEngine; }
	DWORD SCGetColorScheme() const { return m_dwColorScheme; }
	SCGDIpDrawingAttributes& SCGetDrawingAttributes() { return m_DrawingAttributes; }
	//===================================================================
	int SCGetNumPages() const { return (m_pEMFDoc) ?(int)m_pEMFDoc->SCGetNbPages() : 0; }
	int SCGetCurPage() const { return m_iCurPage; }
	HENHMETAFILE SCGetCurEMF() { return m_hEmf; }
	HMENU SCGetCtxMenu() const { return m_hCtxMenu; }
	I_EMFImageOwner* SCGetCtlOwner() const { return m_pICtlOwner; }


	//=== Setters =======================================================
		//--- properties ------------------------------------------------
	void SCSetCtlBorder(DWORD dwBorderType);
	void SCSetCtlColorStyle(DWORD dwColorStyle, COLORREF crColor, BOOL bRefresh=TRUE);
	void SCSetCtlColor(COLORREF crColor, BOOL bRefresh=TRUE);

	void SCSetPageBorderVisible(BOOL bVisible, BOOL bRefresh=TRUE);
	void SCSetPageShadowVisible(BOOL bVisible, BOOL bRefresh=TRUE);
	void SCSetPaperColorStyle(DWORD dwColorStyle, COLORREF crColor, BOOL bRefresh=TRUE);
	void SCSetPaperColor(COLORREF crColor, BOOL bRefresh=TRUE);
	void SCSetFitMode(int iFit, BOOL bRefresh=TRUE);
	void SCSetCurZoom(int iZoom, BOOL bRefresh=TRUE);
	void SCSetLMargin(long lCx, BOOL bRefresh=TRUE);
	void SCSetTMargin(long lCy, BOOL bRefresh=TRUE);
	void SCSetRMargin(long lCx, BOOL bRefresh=TRUE);
	void SCSetBMargin(long lCy, BOOL bRefresh=TRUE);
	void SCSetMargins(CRect& rc, BOOL bRefresh=TRUE);
	void SCSetAngle(int iAngle, BOOL bRefresh=TRUE); // PageOrientation
	void SCSetTransparencyMode(DWORD dwMode, BOOL bRefresh=TRUE);
	void SCSetReverseVideoMode(DWORD dwMode, BOOL bRefresh=TRUE);

	void SCEnableGDIp(BOOL bEnable=TRUE, BOOL bRefresh=TRUE);
	void SCSetTextRenderingHint(int iHint, BOOL bRefresh=TRUE);
	void SCSetTextContrast(UINT uiContrast, BOOL bRefresh=TRUE);
	void SCSetSmoothingMode(int iSmoothing, BOOL bRefresh=TRUE);
	void SCSetInterpolationMode(int uiInterpolation, BOOL bRefresh=TRUE);
	void SCSetPixelOffsetMode(int uiPixelOffset, BOOL bRefresh=TRUE);
		//--- extra -----------------------------------------------------
	void SCSetScale(float fScale, BOOL bRefresh=TRUE);
	void SCSetReverseVideo(BOOL bReverseVideo, BOOL bRefresh=TRUE);
	void SCSetRasEngine(UINT uiEngine, BOOL bRefresh=TRUE);
	void SCSetColorScheme(DWORD dwColorScheme, BOOL bRefresh=TRUE);
	void SCSetColorSchemeEx(DWORD dwColorScheme, COLORREF* pcrCtlColor=NULL,
						  COLORREF* pcrPaperColor=NULL, BOOL bRefresh=TRUE);
	void SCSetDrawingAttributes(SCGDIpDrawingAttributes& rDrawingAttributes, BOOL bRefresh=TRUE);
	//===================================================================
	void SCSetEMFDoc(SCEMFDoc* pEMFDoc) { m_pEMFDoc = pEMFDoc; }
	void SCSetCtxMenu(HMENU hMenu) { m_hCtxMenu = hMenu; }
	void SCSetCtlOwner(I_EMFImageOwner* pICtlOwner) { m_pICtlOwner = pICtlOwner; }


// Operations
public:
	void SCReset();

	void SCGotoFirstPage();
	void SCGotoPrevPage();
	void SCGotoNextPage();
	void SCGotoLastPage();
	void SCGotoPage(int iPage);

	void SCOnAttributesChanged();
	void SCRefresh()
	{
		if (m_iCurPage)
			SCGotoPage(m_iCurPage);
		else
		if (IsWindow(m_hWnd))
			Invalidate();
	}

	void SCRotateLeft(); 
	void SCRotateRight();

	void SCSaveDocument(LPCTSTR lpszPathname, int iType, BOOL bSaveAll=FALSE, BOOL bSaveAsIs=TRUE);
	void SCPrintImage(CDC* pDC, CPrintInfo* pInfo, BOOL bFit=FALSE);
	BOOL SCCopyToClipBoard(CRect* pSrcRect=NULL);
	BOOL SCPasteFromClipBoard(CRect* pDestRect=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFImage)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	void SCComputeFitZoom();
	void SCComputePageInfo(int iPage);
	void InitializeScrollBars(int width, int height);
	BOOL ScrollChildWindow(int nScrollBar, UINT wScrollCode, int iCount=1 );
	void SizeScrollBars(int uiWindowWidth, int uiWindowHeight);
	void SCComputeRowsCols();
	void SCGetPageSize(CSize& PgSize);
	void SCGetPageBlackBox(int iPage, CRect& rcElems);

	BOOL SCRasterizeEMF(HDC hDC, HENHMETAFILE hEMF, CRect& rcPlay, BOOL bPrinting=FALSE);
	void SCDisplayPage(HDC hDC, int xDest, int yDest, int iWdt, int iHgt);
	void SCDisplayPageEx(HDC hDC, HENHMETAFILE hEmf, CRect& rcDest, CRect& rcPaper,
						SCGDIpDrawingAttributes& rAttributes,
						int iAngle=0, int iZoom=SC_ZOOM100, BOOL bInvert=FALSE);

//		void SCDisplayPageComments(HDC hDC, int xDest, int yDest, int iWdt, int iHgt);

// Generated message map functions
protected:
	//{{AFX_MSG(CSCEMFImage)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SCPaint(CDC* pDC);
	void SCUpdatePageInfoColors();

private:
	void SCInitConverter(SCEMFConverter& converter);
	void SCSaveImageDiscardHandle(HANDLE hRes, LPCTSTR lpszPathname, int iType, int iSubType);

	COLORREF SCGetTranslatedPaperColor();
	COLORREF SCGetTranslatedCtlColor();
	// some helpers
	inline BOOL SCIsFullReverseVideo()
	{
		return (m_bReverseVideo && SC_RVIDEOFULL(m_DrawingAttributes));
	}
	inline BOOL SCIsPartialReverseVideo()
	{
		return (m_bReverseVideo && !SC_RVIDEOFULL(m_DrawingAttributes));
	}
	inline COLORREF SCGetReversedPaperColor() { return ~SCGetTranslatedPaperColor() & 0x00FFFFFF; }

	// data
#ifndef SC_NO_MEMDC
	CSCMemDC		m_WorkMemDC;	 // Memory DC for flicker-free painting
#endif
	HMENU			m_hCtxMenu;		 // Contextual menu
	I_EMFImageOwner* m_pICtlOwner;	 // Callback channel

	SCEMFDoc*		m_pEMFDoc;		 // Source document
	int				m_iCurPage;		 // Current page
	HENHMETAFILE	m_hEmf;			 // EMF of current page
	int				m_iWorldCx;		 // x-extent of the raster surface 
	int				m_iWorldCy;		 // y-extent of the raster surface

	// Renderer-independent attributes
	int				m_iAngle;		 // current view angle
	int				m_iCurZoom;		 // Current zoom
	int				m_iFitMode;		 // Current fit mode
	BOOL			m_bReverseVideo; // Reverse video effect (dup in renderer)
	int				m_iPgPosX;		 // x at which drawing begins
	int				m_iPgPosY;		 // y at which drawing begins
	CRect			m_rectMargins;	 // User-defined margins (not an actual rect)
	CRect			m_rectPaper;	 // Computed page size (including user margins)
	UINT			m_uiRasEngine;	 // Raster angine to use
	DWORD			m_dwColorScheme;  // Border, Shade, transparent/solid color around/under page
	COLORREF		m_crCtlColor;	  // Color around page (RGB or system index)
	COLORREF		m_crPaperColor;	  // Color under page (RGB or system index) (dup in renderer)

	// User-selected renderer drawing attributes
	SCGDIpDrawingAttributes m_DrawingAttributes;
};

// Interface that a CSCEMFImage container may implement to get called back
// I_EMFImageOwner
class SMC_INTERFACE I_EMFImageOwner
{
public:
	virtual void SCSetupCtxMenu(CMenu* pMenu) = 0;
	virtual void SCOnCtxCommand(WPARAM wParam, LPARAM lParam) = 0;
	virtual CWnd* SCGetMenuOwner() = 0;
};


#endif //_SCEMFIMAGE_H_
//  ------------------------------------------------------------
