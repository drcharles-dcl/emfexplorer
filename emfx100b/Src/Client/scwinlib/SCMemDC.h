/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_SCMEMDC_H__790E5B99_35E2_4C34_B5E2_DA992D699D20__INCLUDED_)
#define AFX_SCMEMDC_H__790E5B99_35E2_4C34_B5E2_DA992D699D20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STGRAY_SCALE_NONE		0
#define STGRAY_SCALE_BW			2
#define STGRAY_SCALE_COARSE		4
#define STGRAY_SCALE_FINE		8

// static
BOOL SCRotateDC(HDC hDC, int iAngle, CRect rect, int iXPos, int iYPos, int& iGrOldMode);
BOOL SCUnRotateDC(HDC hDC, int iGrOldMode);
HDC SCGetAttribDC(HDC hDC);
//

class CSCMemDC : public CDC  
{
private:
	HBITMAP		m_hStockBM;		// to retore DC
	CBitmap		*m_pBmpMem;
	CPalette	*m_pPal;
	int			m_iWidth;
	int			m_iHeight;
	int			m_InitColor;
	int			m_iGrayScale;
	int			m_iGrMode;

public:
	CSCMemDC();
	virtual ~CSCMemDC();

	BOOL SCPrepareSurface(int width, int height, HENHMETAFILE hemf=NULL, COLORREF crBkColor=RGB(255, 255, 255));
	void SCCleanUp();
	int	SCGetWidth() {return m_iWidth;}
	int	SCGetHeight() {return m_iHeight;}
	HBITMAP STGetDCDIB() {  if (m_pBmpMem)
								return (HBITMAP)(*m_pBmpMem);
							return NULL;
						}
	CBitmap *SCDetachDIB();
	void SCSetGrayScale(int iGrayScale) {m_iGrayScale = iGrayScale;}
	BOOL SCShrinkSurface(CRect rc);
	BOOL SCResetSurfaceColor(int iColor);
	BOOL SCEraseBkgn();
	BOOL SCEraseBkgn(CBrush* pBrush);
	BOOL SCRotateDC(int iAngle, CRect rect, int iXPos, int iYPos);
	BOOL SCSelfRotate(int iAngle, CSize sizeDelta);
	BOOL SCUnRotateDC();
	BOOL SCRestoreOrigin(int iAngle, CRect rect, int iXPos, int iYPos);
};

#endif // !defined(AFX_SCMEMDC_H__790E5B99_35E2_4C34_B5E2_DA992D699D20__INCLUDED_)
