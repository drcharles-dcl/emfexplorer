/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFCONVERTER_H_
#define _SCEMFCONVERTER_H_

#include "SCEMFDoc.h"
#include "SCEMFViewDefs.h"
#include "SCGdiplusUtils.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCMemDC.h)

class SCEMFConverter : public I_SCRenderingContext
{
public:
	SCEMFConverter();
	~SCEMFConverter();

public:
	// Setters
	void SCSetEMFDoc(SCEMFDoc* pEMFDoc) { m_pEMFDoc = pEMFDoc; }
	void SCSetCurZoom(int	iCurZoom) { m_iCurZoom = iCurZoom; }
	void SCSetAngle(int iAngle) { m_iCtxAngle = iAngle; }
	void SCSetMargins(CRect& rectMargins) { m_rectMargins = rectMargins; }
	void SCSetReverseVideo(BOOL bReverseVideo) { m_bReverseVideo = bReverseVideo; }
	void SCSetRasEngine(UINT uiRasEngine) { m_uiRasEngine = uiRasEngine; }
	void SCSetPaperColor(COLORREF crPaperColor) { m_crPaperColor = crPaperColor; }
	void SCSetRectClient(CRect& rcClient) { m_rectClient = rcClient; }
	void SCSetFitMode(int iFitMode) { m_iFitMode = iFitMode; }
	void SCSetDrawingAttributes(SCGDIpDrawingAttributes& rAttributes)
	{
		m_CtxDrawingAttributes = rAttributes;
	}

	// Operation
	HANDLE SCCopyPage(int iPage, int iOutputType, int iSubType, BOOL bSaveAsIs);
	void SCDisplayPageEx(HDC hDC, HENHMETAFILE hEmf, CRect& rcDest, CRect& rcPaper,
						SCGDIpDrawingAttributes& rAttributes,
						int iAngle=0, int iZoom=SC_ZOOM100, BOOL bInvert=FALSE);

// Implemantation
	// I_SCRenderingContext
	virtual void SCPostRender(HDC hAttribDC, HDC hOutDC, GDPGraphics* pOutGraphics);

protected:
	void SCComputeFitZoom(CRect& rcElems);
	inline COLORREF SCGetTranslatedPaperColor() { return m_crPaperColor; }
	inline COLORREF SCGetReversedPaperColor() { return ~SCGetTranslatedPaperColor() & 0x00FFFFFF; }
	inline BOOL SCIsFullReverseVideo()
	{
		return (m_bReverseVideo && SC_RVIDEOFULL(m_CtxDrawingAttributes));
	}

private:
	// dup
	SCEMFDoc*		m_pEMFDoc;		 // Source document
	int				m_iCurZoom;		 // Current zoom
	CRect			m_rectMargins;	 // User-defined margins (not an actual rect)
	BOOL			m_bReverseVideo; // Reverse video effect (dup in renderer)
	UINT			m_uiRasEngine;	 // Raster angine to use
	COLORREF		m_crPaperColor;	 // Translated paper color
	CRect			m_rectClient;
	int				m_iFitMode;

	//
	int							m_iCtxAngle;
	int							m_iCtxPage;
	CRect						m_rcCtxDest;
	BOOL						m_bCtxMeta;
	HDC							m_hdcCtxAttrib;
	BOOL						m_bCtxSaveAsIs;
	BOOL						m_bCtxInvert;
	SCGDIpDrawingAttributes 	m_CtxDrawingAttributes;
};

#endif //_SCEMFCONVERTER_H_
//  ------------------------------------------------------------
