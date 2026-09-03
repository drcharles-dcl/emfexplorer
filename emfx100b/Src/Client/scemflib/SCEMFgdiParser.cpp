/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFgdiParser.h"
#include "SCEMFRasterizerDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SCEMFDefs.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCDCCloner.h)

// to allow token pasting at the beginig of line
#define SC_TKNPASTE(fake)

#define SC_RECORD_DECL0(recType) \
	SC_TKNPASTE(nothing)##recType *pRec = (##recType*)m_pRecord; \
	if (pRec->emr.nSize < sizeof(##recType)) \
		return SC_BRK_NOERROR

#define SC_RECORD_DECL_PLAY(recType) \
	SC_TKNPASTE(nothing)##recType *pRec = (##recType*)m_pRecord; \
	if (pRec->emr.nSize < sizeof(##recType)) \
	{ \
		SCPlayRecord(); \
		return SC_BRK_NOERROR; \
	}

// Reminder
#ifndef SC_RENDERER_CAN_DOROP
// Warning: when m_hPlayDC is not a clone, calling functions like SetMapmode on it may
// perturbate GDI+. This was observed for printer DC.
#pragma message(__FILE__ "(47): SC_RENDERER_CAN_DOROP not defined. Potential printing problems.")
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCEMFgdiParser::CSCEMFgdiParser():
	SCBrkEMF(),
	m_pRenderer(NULL),
	m_hDestDC(NULL)
#ifdef SC_USE_DCCLONER
	,m_pCloneDC(NULL)
#endif
{
	memset(&m_xformOrig, 0, sizeof(m_xformOrig));
	m_xformOrig.eM11 = m_xformOrig.eM22 = 1;
}

CSCEMFgdiParser::CSCEMFgdiParser(I_EMFRenderer* pRenderer):
	SCBrkEMF(),
	m_pRenderer(pRenderer),
	m_hDestDC(NULL)
#ifdef SC_USE_DCCLONER
	,m_pCloneDC(NULL)
#endif
{
	memset(&m_xformOrig, 0, sizeof(m_xformOrig));
	m_xformOrig.eM11 = m_xformOrig.eM22 = 1;
}


CSCEMFgdiParser::~CSCEMFgdiParser()
{
#ifdef SC_USE_DCCLONER
	// delete temporary DC
	if (m_pCloneDC)
		delete m_pCloneDC;
#endif
}

void CSCEMFgdiParser::SCResetCracker()
{
}

//////////////////////////////////////////////////////////////////////
// Events methods
//////////////////////////////////////////////////////////////////////
SC_BRKRESULT CSCEMFgdiParser::OnEmfHEADER()
{
	// TRACE0("**EMR_HEADER\n");
	ASSERT(m_pRenderer);

	//ENHMETAHEADER *pRec = (ENHMETAHEADER*)m_pRecord;

	SCPlayRecord();
	m_pRenderer->SCBeginRendering(m_hDestDC, m_hPlayDC);

	return SCBrkEMF::OnEmfHEADER();
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYBEZIER()
{
	// TRACE0("**EMR_POLYBEZIER\n");
	ASSERT(m_pRenderer);

	EMRPOLYBEZIER *pRec = (EMRPOLYBEZIER*)m_pRecord;
	m_pRenderer->SCDrawBezier((LPPOINT)pRec->aptl, pRec->cptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYGON()
{
	// TRACE0("**EMR_POLYGON\n");
	ASSERT(m_pRenderer);

	EMRPOLYGON *pRec = (EMRPOLYGON*)m_pRecord;
	m_pRenderer->SCDrawPolygon((LPPOINT)pRec->aptl, pRec->cptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYLINE()
{
	// TRACE0("**EMR_POLYLINE\n");
	ASSERT(m_pRenderer);

	EMRPOLYLINE *pRec = (EMRPOLYLINE*)m_pRecord;
	m_pRenderer->SCDrawLines((LPPOINT)pRec->aptl, pRec->cptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYBEZIERTO()
{
	// TRACE0("**EMR_POLYBEZIERTO\n");
	ASSERT(m_pRenderer);

	EMRPOLYBEZIERTO *pRec = (EMRPOLYBEZIERTO*)m_pRecord;
	m_pRenderer->SCDrawBezierTo((LPPOINT)pRec->aptl, pRec->cptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYLINETO()
{
	// TRACE0("**EMR_POLYLINETO\n");
	ASSERT(m_pRenderer);

	EMRPOLYLINETO *pRec = (EMRPOLYLINETO*)m_pRecord;
	m_pRenderer->SCDrawLinesTo((LPPOINT)pRec->aptl, pRec->cptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYPOLYLINE()
{
	// TRACE0("**EMR_POLYPOLYLINE\n");
	ASSERT(m_pRenderer);

	EMRPOLYPOLYLINE *pRec = (EMRPOLYPOLYLINE*)m_pRecord;
	LPPOINT pPts = (LPPOINT)((DWORD*)pRec->aPolyCounts + pRec->nPolys);
	m_pRenderer->SCDrawPolyPolyline(pPts, pRec->cptl, (DWORD*)pRec->aPolyCounts, pRec->nPolys);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYPOLYGON()
{
	// TRACE0("**EMR_POLYPOLYGON\n");
	ASSERT(m_pRenderer);

	EMRPOLYPOLYGON *pRec = (EMRPOLYPOLYGON*)m_pRecord;
	LPPOINT pPts = (LPPOINT)((DWORD*)pRec->aPolyCounts + pRec->nPolys);
	m_pRenderer->SCDrawPolyPolygon(pPts, pRec->cptl, (DWORD*)pRec->aPolyCounts, pRec->nPolys);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETWINDOWEXTEX()
{
	// TRACE0("**EMR_SETWINDOWEXTEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetWindowExtent(((EMRSETWINDOWEXTEX*)m_pRecord)->szlExtent.cx,
									((EMRSETWINDOWEXTEX*)m_pRecord)->szlExtent.cy);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETWINDOWORGEX()
{
	// TRACE0("**EMR_SETWINDOWORGEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetWindowOrg(((EMRSETWINDOWORGEX*)m_pRecord)->ptlOrigin.x,
								((EMRSETWINDOWORGEX*)m_pRecord)->ptlOrigin.y);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETVIEWPORTEXTEX()
{
	// TRACE0("**EMR_SETVIEWPORTEXTEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetViewportExtent(((EMRSETVIEWPORTEXTEX*)m_pRecord)->szlExtent.cx,
									 ((EMRSETVIEWPORTEXTEX*)m_pRecord)->szlExtent.cy);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETVIEWPORTORGEX()
{
	// TRACE0("**EMR_SETVIEWPORTORGEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetViewportOrg(((EMRSETVIEWPORTORGEX*)m_pRecord)->ptlOrigin.x,
								  ((EMRSETVIEWPORTORGEX*)m_pRecord)->ptlOrigin.y);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETBRUSHORGEX()
{
	// TRACE0("**EMR_SETBRUSHORGEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management (via stretchbltmode and halftone)
#endif

	m_pRenderer->SCSetBrushOrg(((EMRSETBRUSHORGEX*)m_pRecord)->ptlOrigin);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEOF()
{
	// TRACE0("**EMR_EOF\n");
	ASSERT(m_pRenderer);

	// (EMREOF*)m_pRecord;
	SCPlayRecord();
	m_pRenderer->SCEndRendering();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETPIXELV()
{
	// TRACE0("**EMR_SETPIXELV\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawPixel(((EMRSETPIXELV*)m_pRecord)->ptlPixel,
							 ((EMRSETPIXELV*)m_pRecord)->crColor);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETMAPPERFLAGS()
{
	// TRACE0("**EMR_SETMAPPERFLAGS\n");
	ASSERT(m_pRenderer);

	// (EMRSETMAPPERFLAGS*)m_pRecord;

	// No aspect ratio management, as we only use TT fonts.
	// Anyway let Windows do it.
	SCPlayRecord();		
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETMAPMODE()
{
	// TRACE0("**EMR_SETMAPMODE\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management (via viewport/window settings)
#endif

	m_pRenderer->SCSetMapMode(((EMRSETMAPMODE*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETBKMODE()
{
	// TRACE0("**EMR_SETBKMODE\n");
	ASSERT(m_pRenderer);
	//SCPlayRecord(); // wait and see

	m_pRenderer->SCSetBkMode(((EMRSETBKMODE*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETPOLYFILLMODE()
{
	// TRACE0("**EMR_SETPOLYFILLMODE\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetPolyFillMode(((EMRSETPOLYFILLMODE*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETROP2()
{
	// TRACE0("**EMR_SETROP2\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetROP2(((EMRSETROP2*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETSTRETCHBLTMODE()
{
	// TRACE0("**EMR_SETSTRETCHBLTMODE\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetStretchBltMode(((EMRSETSTRETCHBLTMODE*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETTEXTALIGN()
{
	// TRACE0("**EMR_SETTEXTALIGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetTextAlign(((EMRSETTEXTALIGN*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETCOLORADJUSTMENT()
{
	// TRACE0("**EMR_SETCOLORADJUSTMENT\n");
	ASSERT(m_pRenderer);


#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // for ROP (just in case)
#endif
	// "The color adjustment values are used to adjust the input color of the source bitmap
	// for calls to the StretchBlt and StretchDIBits functions when HALFTONE mode is set."
	// But it's NT specific.

	m_pRenderer->SCSetColorAsjustment(&((EMRSETCOLORADJUSTMENT*)m_pRecord)->ColorAdjustment);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETTEXTCOLOR()
{
	// TRACE0("**EMR_SETTEXTCOLOR\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetTextColor(((EMRSETTEXTCOLOR*)m_pRecord)->crColor);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETBKCOLOR()
{
	// TRACE0("**EMR_SETBKCOLOR\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetBkColor(((EMRSETBKCOLOR*)m_pRecord)->crColor);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfOFFSETCLIPRGN()
{
	// TRACE0("**EMR_OFFSETCLIPRGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCOffsetClipRect(((EMROFFSETCLIPRGN*)m_pRecord)->ptlOffset);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfMOVETOEX()
{
	// TRACE0("**EMR_MOVETOEX\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCMoveToEx(((EMRMOVETOEX*)m_pRecord)->ptl);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETMETARGN()
{
	// TRACE0("**EMR_SETMETARGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCOnSetMetaRgn();
	
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXCLUDECLIPRECT()
{
	// TRACE0("**EMR_EXCLUDECLIPRECT\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCExcludeClipRect(((EMREXCLUDECLIPRECT*)m_pRecord)->rclClip);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfINTERSECTCLIPRECT()
{
	// TRACE0("**EMR_INTERSECTCLIPRECT\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCIntersectClipRect(((EMRINTERSECTCLIPRECT*)m_pRecord)->rclClip);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSCALEVIEWPORTEXTEX()
{
	// TRACE0("**EMR_SCALEVIEWPORTEXTEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	EMRSCALEVIEWPORTEXTEX *pRec = (EMRSCALEVIEWPORTEXTEX*)m_pRecord;
	m_pRenderer->SCScaleViewport(pRec->xNum, pRec->xDenom, pRec->yNum, pRec->yDenom);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSCALEWINDOWEXTEX()
{
	// TRACE0("**EMR_SCALEWINDOWEXTEX\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	EMRSCALEWINDOWEXTEX *pRec = (EMRSCALEWINDOWEXTEX*)m_pRecord;
	m_pRenderer->SCScaleWindow(pRec->xNum, pRec->xDenom, pRec->yNum, pRec->yDenom);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSAVEDC()
{
	// TRACE0("**EMR_SAVEDC\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	m_pRenderer->SCOnDCSaved();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfRESTOREDC()
{
	// TRACE0("**EMR_RESTOREDC\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();
	
	// Be careful : sometimes we get bad values from the record.
	// Some documents contain values like 30,
	// though there is only one DC state on the stack
	m_pRenderer->SCOnDCRestored(((EMRRESTOREDC*)m_pRecord)->iRelative);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETWORLDTRANSFORM()
{
	// TRACE0("**EMR_SETWORLDTRANSFORM\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCSetWorldTransform(((EMRSETWORLDTRANSFORM*)m_pRecord)->xform);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfMODIFYWORLDTRANSFORM()
{
	// TRACE0("**EMR_MODIFYWORLDTRANSFORM\n");
	ASSERT(m_pRenderer);

#ifndef SC_RENDERER_CAN_DOROP
	SCPlayRecord(); // required for ROP management
#endif

	m_pRenderer->SCModifyWorldTransform(((EMRMODIFYWORLDTRANSFORM*)m_pRecord)->xform,
										((EMRMODIFYWORLDTRANSFORM*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSELECTOBJECT()
{
	// TRACE0("**EMR_SELECTOBJECT\n");
	ASSERT(m_pRenderer);

	// First, play the record to realize GDI objects
	SCPlayRecord();

	// Then extract objects characteristics
	EMRSELECTOBJECT *pRec = (EMRSELECTOBJECT*)m_pRecord;
	HGDIOBJ hGdiObj;
	// Is the high order bit set?
	if (pRec->ihObject & 0x80000000)
	{
		// High order bit is set - its a stock object
		// Strip the high bit to get the index
		DWORD dwIndex = pRec->ihObject & 0x7fffffff;
		// Pass the index to GetStockObject()
		hGdiObj = GetStockObject( dwIndex );
	} else
		// High order bit isn't set - not a stock object
		hGdiObj = (HGDIOBJ)m_lpEnumHandleTable->objectHandle[pRec->ihObject];

	long lType = GetObjectType(hGdiObj);
	switch(lType)
	{
		case OBJ_EXTPEN:
			m_pRenderer->SCOnChangeExtPen((HPEN)hGdiObj);
			break;

		case OBJ_PEN:
			m_pRenderer->SCOnChangePen((HPEN)hGdiObj);
			break;

		case OBJ_BRUSH:
			m_pRenderer->SCOnChangeBrush((HBRUSH)hGdiObj);
			break;
			
		case OBJ_FONT:
			m_pRenderer->SCOnChangeFont((HFONT)hGdiObj);
			break;
			
		//case OBJ_PAL:
		// unlikely to occur (The SelectObject function does not work with palettes)
		//m_pRenderer->SCOnChangePalette((HPALETTE)hGdiObj);
		//break;								
	}

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATEPEN()
{
	// TRACE0("**EMR_CREATEPEN\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object creation: do nothing
	// EMRCREATEPEN *pRec = (EMRCREATEPEN*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATEBRUSHINDIRECT()
{
	// TRACE0("**EMR_CREATEBRUSHINDIRECT\n");
	ASSERT(m_pRenderer);

	// Note: due to dithering, Windows may change BS_SOLID in BS_PATTERN
	SCPlayRecord();

	// Object creation: do nothing
	// EMRCREATEBRUSHINDIRECT *pRec = (EMRCREATEBRUSHINDIRECT*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfDELETEOBJECT()
{
	// TRACE0("**EMR_DELETEOBJECT\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object deletion: do nothing
	// EMRDELETEOBJECT *pRec = (EMRDELETEOBJECT*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfANGLEARC()
{
	// TRACE0("**EMR_ANGLEARC\n");
	ASSERT(m_pRenderer);

	EMRANGLEARC *pRec = (EMRANGLEARC*)m_pRecord;
	m_pRenderer->SCDrawAngleArc((LPPOINT)&pRec->ptlCenter, pRec->nRadius, pRec->eStartAngle, pRec->eSweepAngle);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfELLIPSE()
{
	// TRACE0("**EMR_ELLIPSE\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawEllipse((LPCRECT)&((EMRELLIPSE*)m_pRecord)->rclBox);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfRECTANGLE()
{
	// TRACE0("**EMR_RECTANGLE\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawRectangle((LPCRECT)&((EMRRECTANGLE*)m_pRecord)->rclBox);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfROUNDRECT()
{
	// TRACE0("**EMR_ROUNDRECT\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawRoundRectangle((LPCRECT)&((EMRROUNDRECT*)m_pRecord)->rclBox,
												((EMRROUNDRECT*)m_pRecord)->szlCorner);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfARC()
{
	// TRACE0("**EMR_ARC\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawArc(&((EMRARC*)m_pRecord)->rclBox,
						   &((EMRARC*)m_pRecord)->ptlStart,
						   &((EMRARC*)m_pRecord)->ptlEnd);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCHORD()
{
	// TRACE0("**EMR_CHORD\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawChord(&((EMRCHORD*)m_pRecord)->rclBox,
							 &((EMRCHORD*)m_pRecord)->ptlStart,
							 &((EMRCHORD*)m_pRecord)->ptlEnd);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPIE()
{
	// TRACE0("**EMR_PIE\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawPie(&((EMRPIE*)m_pRecord)->rclBox,
						   &((EMRPIE*)m_pRecord)->ptlStart,
						   &((EMRPIE*)m_pRecord)->ptlEnd);
	return SC_BRK_NOERROR;

}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSELECTPALETTE()
{
	// TRACE0("**EMR_SELECTPALETTE\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	m_pRenderer->SCOnChangePalette((HPALETTE)((EMRSELECTPALETTE*)m_pRecord)->ihPal);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATEPALETTE()
{
	// TRACE0("**EMR_CREATEPALETTE\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object creation: do nothing
	// EMRCREATEPALETTE *pRec = (EMRCREATEPALETTE*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETPALETTEENTRIES()
{
	// TRACE0("**EMR_SETPALETTEENTRIES\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object creation: do nothing
	// EMRSETPALETTEENTRIES *pRec = (EMRSETPALETTEENTRIES*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfRESIZEPALETTE()
{
	// TRACE0("**EMR_RESIZEPALETTE\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object creation: do nothing
	// EMRRESIZEPALETTE *pRec = (EMRRESIZEPALETTE*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfREALIZEPALETTE()
{
	// TRACE0("**EMR_REALIZEPALETTE\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// Object creation: do nothing
	// EMRREALIZEPALETTE *pRec = (EMRREALIZEPALETTE*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTFLOODFILL()
{
	// TRACE0("**EMR_EXTFLOODFILL\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCFloodFill(((EMREXTFLOODFILL*)m_pRecord)->ptlStart,
							 ((EMREXTFLOODFILL*)m_pRecord)->crColor,
							 ((EMREXTFLOODFILL*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfLINETO()
{
	// TRACE0("**EMR_LINETO\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawLinesTo((LPPOINT)&((EMRLINETO*)m_pRecord)->ptl, 1);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfARCTO()
{
	// TRACE0("**EMR_ARCTO\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawArcTo(&((EMRARCTO*)m_pRecord)->rclBox,
							 &((EMRARCTO*)m_pRecord)->ptlStart,
							 &((EMRARCTO*)m_pRecord)->ptlEnd);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYDRAW()
{
	// TRACE0("**EMR_POLYDRAW\n");
	ASSERT(m_pRenderer);

	EMRPOLYDRAW *pRec = (EMRPOLYDRAW*)m_pRecord;

	BYTE* pTypes = (BYTE*)((POINTL*)pRec->aptl + pRec->cptl);
	m_pRenderer->SCDrawPolyDraw((POINT*)pRec->aptl,
								pRec->cptl,
								pTypes);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETARCDIRECTION()
{
	// TRACE0("**EMR_SETARCDIRECTION\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetArcDirection(((EMRSETARCDIRECTION*)m_pRecord)->iArcDirection);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETMITERLIMIT()
{
	// TRACE0("**EMR_SETMITERLIMIT\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSetMiterLimit(((EMRSETMITERLIMIT*)m_pRecord)->eMiterLimit);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfBEGINPATH()
{
	// TRACE0("**EMR_BEGINPATH\n");
	ASSERT(m_pRenderer);

	//EMRBEGINPATH *pRec = (EMRBEGINPATH*)m_pRecord;

	m_pRenderer->SCBeginPath();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfENDPATH()
{
	// TRACE0("**EMR_ENDPATH\n");
	ASSERT(m_pRenderer);

	//EMRENDPATH *pRec = (EMRENDPATH*)m_pRecord;

	m_pRenderer->SCEndPath();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCLOSEFIGURE()
{
	// TRACE0("**EMR_CLOSEFIGURE\n");
	ASSERT(m_pRenderer);

	//EMRCLOSEFIGURE *pRec = (EMRCLOSEFIGURE*)m_pRecord;

	m_pRenderer->SCCloseFigure();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfFILLPATH()
{
	// TRACE0("**EMR_FILLPATH\n");
	ASSERT(m_pRenderer);

	//EMRFILLPATH *pRec = (EMRFILLPATH*)m_pRecord;

	// Note: Don't know what the rclBounds is doing in the record
	m_pRenderer->SCApplyPath(SC_PATH_FILL);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSTROKEANDFILLPATH()
{
	// TRACE0("**EMR_STROKEANDFILLPATH\n");
	ASSERT(m_pRenderer);

	//EMRSTROKEANDFILLPATH *pRec = (EMRSTROKEANDFILLPATH*)m_pRecord;

	// Note: Don't know what the rclBounds is doing in the record
	m_pRenderer->SCApplyPath(SC_PATH_STROKEANDFILL);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSTROKEPATH()
{
	// TRACE0("**EMR_STROKEPATH\n");
	ASSERT(m_pRenderer);

	//EMRSTROKEPATH *pRec = (EMRSTROKEPATH*)m_pRecord;

	// Note: Don't know what the rclBounds is doing in the record
	m_pRenderer->SCApplyPath(SC_PATH_STROKE);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfFLATTENPATH()
{
	// TRACE0("**EMR_FLATTENPATH\n");
	ASSERT(m_pRenderer);

	//EMRFLATTENPATH *pRec = (EMRFLATTENPATH*)m_pRecord;

	m_pRenderer->SCFlattenPath();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfWIDENPATH()
{
	// TRACE0("**EMR_WIDENPATH\n");
	ASSERT(m_pRenderer);

	//EMRWIDENPATH *pRec = (EMRWIDENPATH*)m_pRecord;

	m_pRenderer->SCWidenPath();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSELECTCLIPPATH()
{
	// TRACE0("**EMR_SELECTCLIPPATH\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCApplyPath(SC_PATH_CLIP, ((EMRSELECTCLIPPATH*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfABORTPATH()
{
	// TRACE0("**EMR_ABORTPATH\n");
	ASSERT(m_pRenderer);

	//EMRABORTPATH *pRec = (EMRABORTPATH*)m_pRecord;

	m_pRenderer->SCAbortPath();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfGDICOMMENT()
{
	// TRACE0("**EMR_GDICOMMENT\n");
	ASSERT(m_pRenderer);

#pragma message( __FILE__  "(894): TODO: Recurse parser on embedded WMF? ")
#if 0
	EMRGDICOMMENT *pRec = (EMRGDICOMMENT*)m_pRecord;

	DWORD* pDW = (DWORD*)pRec->Data;
	if (GDICOMMENT_IDENTIFIER==*pDW)
	{
		pDW++;
		switch(*pDW)
		{
		case GDICOMMENT_WINDOWS_METAFILE:
			// embedded WMF
				// TODO:
			break;
			
		case GDICOMMENT_MULTIFORMATS:
			// embedded formats
				// TODO:
			break;
		}
	}
#endif

	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfFILLRGN()
{
	// TRACE0("**EMR_FILLRGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCFillRgn((HBRUSH)m_lpEnumHandleTable->objectHandle[((EMRFILLRGN*)m_pRecord)->ihBrush],
						   ((EMRFILLRGN*)m_pRecord)->RgnData,
						   ((EMRFILLRGN*)m_pRecord)->cbRgnData);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfFRAMERGN()
{
	// TRACE0("**EMR_FRAMERGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCFrameRgn((HBRUSH)m_lpEnumHandleTable->objectHandle[((EMRFRAMERGN*)m_pRecord)->ihBrush],
							((EMRFRAMERGN*)m_pRecord)->szlStroke,
							((EMRFRAMERGN*)m_pRecord)->RgnData,
							((EMRFRAMERGN*)m_pRecord)->cbRgnData);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfINVERTRGN()
{
	// TRACE0("**EMR_INVERTRGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCInvertRgn(((EMRINVERTRGN*)m_pRecord)->RgnData,
							 ((EMRINVERTRGN*)m_pRecord)->cbRgnData);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPAINTRGN()
{
	// TRACE0("**EMR_PAINTRGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCPaintRgn(((EMRPAINTRGN*)m_pRecord)->RgnData,
							((EMRPAINTRGN*)m_pRecord)->cbRgnData);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTSELECTCLIPRGN()
{
	// TRACE0("**EMR_EXTSELECTCLIPRGN\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCSelectClipRgn(((EMREXTSELECTCLIPRGN*)m_pRecord)->RgnData,
								 ((EMREXTSELECTCLIPRGN*)m_pRecord)->cbRgnData,
								 ((EMREXTSELECTCLIPRGN*)m_pRecord)->iMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfBITBLT()
{
	// TRACE0("**EMR_BITBLT\n");
	ASSERT(m_pRenderer);

	EMRBITBLT *pRec = (EMRBITBLT*)m_pRecord;

	// Destination
	CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);
	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);
		// Source and Destination have same size for BitBlt function
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxDest, pRec->ySrc + pRec->cyDest);
		m_pRenderer->SCDrawImage(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc, pRec->dwRop, &pRec->xformSrc);
	} else
		m_pRenderer->SCPatBlt(&RcDest, pRec->dwRop, &pRec->xformSrc);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSTRETCHBLT()
{
	// TRACE0("**EMR_STRETCHBLT\n");
	ASSERT(m_pRenderer);

	EMRSTRETCHBLT *pRec = (EMRSTRETCHBLT*)m_pRecord;

	// Destination
	CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);
	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);
		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);
		m_pRenderer->SCDrawImage(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc, pRec->dwRop, &pRec->xformSrc);
	} else
		m_pRenderer->SCPatBlt(&RcDest, pRec->dwRop, &pRec->xformSrc);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfMASKBLT()
{
	// TRACE0("**EMR_MASKBLT\n");
	ASSERT(m_pRenderer);

	EMRMASKBLT *pRec = (EMRMASKBLT*)m_pRecord;

	// Destination
	CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);
	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		// the source bitmap
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);

		// the mask bitmap is optional
		BITMAPINFO *pBmiMask = NULL;
		BYTE *pBitsMask = NULL;
		if (pRec->offBmiMask && pRec->offBitsMask)
		{
			pBmiMask = (BITMAPINFO *)((BYTE *)pRec + pRec->offBmiMask); 
			pBitsMask = (BYTE *)((BYTE *)pRec + pRec->offBitsMask);
		}

		// Source equals dest in dimensions
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxDest, pRec->ySrc + pRec->cyDest);

		m_pRenderer->SCDrawImageMsk(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc, pRec->dwRop, &pRec->xformSrc,
		pRec->crBkColorSrc, pRec->xMask, pRec->yMask, pBitsMask, pBmiMask, pRec->iUsageMask);
	} else
		m_pRenderer->SCPatBlt(&RcDest, pRec->dwRop, &pRec->xformSrc);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPLGBLT()
{
	// TRACE0("**EMR_PLGBLT\n");
	ASSERT(m_pRenderer);

	EMRPLGBLT *pRec = (EMRPLGBLT*)m_pRecord;

	ASSERT(pRec->offBmiSrc && pRec->offBitsSrc);
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		// the source bitmap
		BITMAPINFO *pBmi = (BITMAPINFO *)((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits = (BYTE *)((BYTE *)pRec + pRec->offBitsSrc);

		// the mask bitmap is optional
		BITMAPINFO *pBmiMask = NULL;
		BYTE *pBitsMask = NULL;
		if (pRec->offBmiMask && pRec->offBitsMask)
		{
			pBmiMask = (BITMAPINFO *)((BYTE *)pRec + pRec->offBmiMask); 
			pBitsMask = (BYTE *)((BYTE *)pRec + pRec->offBitsMask);
		}

		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);

		m_pRenderer->SCDrawImagePlg(pRec->aptlDest,
		&RcSrc, pBits, pBmi, pRec->iUsageSrc, &pRec->xformSrc, pRec->crBkColorSrc,
		pRec->xMask, pRec->yMask, pBitsMask, pBmiMask, pRec->iUsageMask);
		
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETDIBITSTODEVICE()
{
	// TRACE0("**EMR_SETDIBITSTODEVICE\n");
	ASSERT(m_pRenderer);

	EMRSETDIBITSTODEVICE *pRec = (EMRSETDIBITSTODEVICE*)m_pRecord;

	// Destination
	CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxSrc, pRec->yDest + pRec->cySrc);
	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);
		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);
		m_pRenderer->SCDrawImage(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc, SRCCOPY);
	} else
		m_pRenderer->SCPatBlt(&RcDest, SRCCOPY);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSTRETCHDIBITS()
{
	ASSERT(m_pRenderer);

	// TRACE0("**EMR_STRETCHDIBITS\n");
	EMRSTRETCHDIBITS *pRec = (EMRSTRETCHDIBITS*)m_pRecord;

	// Destination
	CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);
	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);
		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);
		m_pRenderer->SCDrawImage(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc, pRec->dwRop);
	} else
		m_pRenderer->SCPatBlt(&RcDest, pRec->dwRop);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTCREATEFONTINDIRECTW()
{
	// TRACE0("**EMR_EXTCREATEFONTINDIRECTW\n");
	ASSERT(m_pRenderer);

	// Play the record on the DC, in order to get information about the physical font,
	// which is the font really used by the DC.
#if 0
	// Raster and vector fonts should be replaced by substitutes.

	// Example of early font substitution.
	// This is cleaner than late substitution (SCTTFontFromLOGFONT in SCGdiplusUtils.cpp);
	// but it penalizes good TT fonts
	// (memory allocations and string comparisons)
	EMREXTCREATEFONTINDIRECTW *pSubs = (EMREXTCREATEFONTINDIRECTW *)SCCloneRecord();
	if (pSubs)
	{
		LOGFONTW& rLogFont = pSubs->elfw.elfLogFont;
		DWORD dwFamily = SCFontFamilyApproximantW((WCHAR*)rLogFont.lfFaceName);
		if (FF_DONTCARE!=dwFamily)
		{
			rLogFont.lfPitchAndFamily |= dwFamily;
			rLogFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		}
		
		SCPlayRecordAndFree(pSubs);
	} else
		SCPlayRecord();
#else
	SCPlayRecord();
#endif
	return SC_BRK_NOERROR;
}


SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTTEXTOUTA()
{
	// TRACE0("**EMR_EXTTEXTOUTA or **EMR_EXTTEXTOUTW\n");
	ASSERT(m_pRenderer);

	SC_BRKRESULT error(SC_BRK_NOERROR);

	// the structure EMREXTTEXTOUTA is same as EMREXTTEXTOUTW
	EMREXTTEXTOUTA *pRec = (EMREXTTEXTOUTA*)m_pRecord;

	// get the EMRTEXT
	EMRTEXT *pTextRec = &(pRec->emrtext);

	// number of characters	
	INT iNbChars = pTextRec->nChars;
	if (iNbChars<=0)
	{
		// We don't know the behavior of ExtTextOut:
		// - if number of characters is 0
		// - if, moreover, TA_UPDATECP is set in the DC
#if 0
		// if required, play the record
		SCPlayRecord();
#endif
		return 0;
	}

	UINT uiOptions = pTextRec->fOptions; // we might have to alter it

	// Build the string
	LPWSTR pwStr = NULL;
	if (pRec->emr.iType == EMR_EXTTEXTOUTW) 
	{
		// The string is in unicode -> we let it as is.
		// Offset is from the beginning of the struct and is in bytes.
		// 	
		// Get the unicode string
		pwStr = (LPWSTR) ((BYTE *)pRec + pTextRec->offString);

#if 0
		// TODO:
		// Translate glyph indices to code points when the rasterizer does not
		// support string measurement for glyph indices.
		if ((uiOptions & ETO_GLYPH_INDEX) && (!m_pRenderer->SCCanMeasureGlyphs())
		{
			int iRes = SCUnicodeCharsFromGlyphs(m_hPlayDC, pString, iNbChars);
			if (iRes)
				uiOptions &= ~ETO_GLYPH_INDEX;
		}
#endif
	} else
	{
		// The string is single-byte encoded.
		// Get adress of the string	then construct the string.			
		LPSTR pString = (char *)pRec + pTextRec->offString;
		pwStr = new wchar_t[iNbChars+1];
		if (pwStr)
		{// We will use UNICODE strings exclusively
			pwStr[iNbChars] = 0;
			int iNum = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pString, iNbChars, pwStr, iNbChars+1);
			ASSERT(iNum==iNbChars);
		}
	}
	ASSERT(pwStr);

	// Get intercharacter spacing array.
	// Be careful ! there are cases where OffDx==0.
	DWORD OffDx = pTextRec->offDx;
	INT *lpDx = (INT*)((BYTE*)pRec + OffDx);
	if (0==OffDx)
	{
		// If the lpDx parameter is NULL, the ExtTextOut function uses the default
		// spacing between characters. So we do.
		//
		// Compute intercharacter spacing, based on each character width.
		lpDx = new INT[iNbChars];
		BOOL bOk = SCGetTextCharWidthsW(m_hPlayDC, pwStr, iNbChars, lpDx);
		ASSERT(bOk);
	}
	else
	{ // else use distances between origins of adjacent character cells as passed to ExtTextOut.
		// except that, sometimes, the last character is given a spacing of 0, which is
		// not good for us if this character has a non-null width.
		if (0==lpDx[iNbChars-1])
		{
			INT* pInts = new INT[iNbChars];
			memmove(pInts, lpDx, iNbChars*sizeof(INT));

			// compute only the last character's width
			BOOL bOk = SCGetTextCharWidthsW(m_hPlayDC, pwStr+iNbChars-1, 1, pInts+iNbChars-1);
			ASSERT(bOk);
			lpDx = pInts;
			OffDx = 0; // for cleanup
			//at this point, 0 is accepted
			//ASSERT(lpDx[iNbChars-1]);
		}
	}

#if 0
	// This thing is not accurate. We prefer to let the rasterizer use its computed rectangle
	// when no opaquing rectangle is supplied
	CRect rcText = (LPCRECT)&pTextRec->rcl;
	if (rcText.IsRectEmpty())
	{
		rcText.CopyRect((LPCRECT)&pRec->rclBounds);
		DPtoLP(m_hPlayDC, (LPPOINT)&rcText, 2);
	}
	if (GM_COMPATIBLE==pRec->iGraphicsMode)
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&rcText, pwStr, iNbChars, lpDx, pRec->exScale, pRec->eyScale);
	else
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&rcText, pwStr, iNbChars, lpDx, 1, 1);
#else
	if (GM_COMPATIBLE==pRec->iGraphicsMode)
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&pTextRec->rcl, pwStr, iNbChars, lpDx, pRec->exScale, pRec->eyScale);
	else
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&pTextRec->rcl, pwStr, iNbChars, lpDx, 1, 1);
#endif

	// Clean up
	if (pRec->emr.iType == EMR_EXTTEXTOUTA) 
		delete [] pwStr;
	if (0==OffDx)
		delete [] lpDx;

	return error;
}

//////////////////////////////////////////////////////////////////////////////////////
// It is unlikely that SCEmfTextoutA/SCEmfTextoutW are called. So I prefer to let
// them separated (with duplicated code). See OnEmfTextoutA for documentation
SC_BRKRESULT CSCEMFgdiParser::SCEmfTextoutA(EMREXTTEXTOUTA* pRec, EMRTEXT* pTextRec)
{
	INT iNbChars = pTextRec->nChars;
	if (iNbChars<=0)
		return SC_BRK_NOERROR;

	// Get adress of the string	then construct the widestring.			
	LPWSTR pwStr = new wchar_t[iNbChars + 1];
	if (pwStr)
	{// We will use UNICODE strings exclusively
		pwStr[iNbChars] = 0;
		int iNum = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)((char *)pRec + pTextRec->offString),
			iNbChars, pwStr, iNbChars + 1);
		ASSERT(iNum==iNbChars);
		
		SCFinishTextout(pRec, pTextRec, pwStr, pTextRec->fOptions);
		
		// Clean up
		delete [] pwStr;
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::SCEmfTextoutW(EMREXTTEXTOUTW* pRec, EMRTEXT* pTextRec)
{
	INT iNbChars = pTextRec->nChars;
	if (iNbChars<=0)
		return SC_BRK_NOERROR;

	UINT uiOptions = pTextRec->fOptions; // we might have to alter it

	// Get the unicode string
	LPWSTR pwStr = (LPWSTR) ((BYTE *)pRec + pTextRec->offString);

#if 0
	ASSERT(m_pRenderer);

	// TODO:
	// Translate glyph indices to code points when the rasterizer does not
	// support string measurement for glyph indices.
	if ((uiOptions & ETO_GLYPH_INDEX) && (!m_pRenderer->SCCanMeasureGlyphs())
	{
		int iRes = SCUnicodeCharsFromGlyphs(m_hPlayDC, pwStr, iNbChars);
		if (iRes)
			uiOptions &= ~ETO_GLYPH_INDEX;
	}
#endif

	SCFinishTextout(pRec, pTextRec, pwStr, uiOptions);
	return SC_BRK_NOERROR;
}

void CSCEMFgdiParser::SCFinishTextout(EMREXTTEXTOUTA* pRec, EMRTEXT* pTextRec, LPWSTR pwStr, UINT uiOptions)
{
	ASSERT(m_pRenderer);

	INT iNbChars = pTextRec->nChars;
	// Get intercharacter spacing array.
	DWORD OffDx = pTextRec->offDx;
	INT *lpDx = (INT*)((BYTE*)pRec + OffDx);
	if (0==OffDx)
	{
		// Compute intercharacter spacing, based on each character width.
		lpDx = new INT[iNbChars];
		BOOL bOk = SCGetTextCharWidthsW(m_hPlayDC, pwStr, iNbChars, lpDx);
		ASSERT(bOk);
	}
	else
	{ // else use distances between origins of adjacent character cells as passed to ExtTextOut.
		// except that, sometimes, the last character is given a spacing of 0, which is not good for us.
		if (0==lpDx[iNbChars-1])
		{
			INT* pInts = new INT[iNbChars];
			memmove(pInts, lpDx, iNbChars*sizeof(INT));

			// compute only the last character's width
			BOOL bOk = SCGetTextCharWidthsW(m_hPlayDC, pwStr+iNbChars-1, 1, pInts+iNbChars-1);
			ASSERT(bOk);
			lpDx = pInts;
			OffDx = 0; // for cleanup
			ASSERT(lpDx[iNbChars-1]);
		}
	}

	if (GM_COMPATIBLE==pRec->iGraphicsMode)
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&pTextRec->rcl, pwStr, iNbChars, lpDx, pRec->exScale, pRec->eyScale);
	else
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPCRECT)&pTextRec->rcl, pwStr, iNbChars, lpDx, 1, 1);

	// Clean up
	if (0==OffDx)
		delete [] lpDx;
}
///////////////////////////////////////////////////////////////////////////////////////////

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTTEXTOUTW()
{
	// TRACE0("**EMR_EXTTEXTOUTW\n");
	//EMREXTTEXTOUTW *pRec = (EMREXTTEXTOUTW*)m_pRecord;

	// pass the record to 
	return OnEmfEXTTEXTOUTA();
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYBEZIER16()
{
	// TRACE0("**EMR_POLYBEZIER16\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawBezierS(((EMRPOLYBEZIER16*)m_pRecord)->apts,
							  ((EMRPOLYBEZIER16*)m_pRecord)->cpts);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYGON16()
{
	// TRACE0("**EMR_POLYGON16\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawPolygonS(((EMRPOLYGON16*)m_pRecord)->apts,
								((EMRPOLYGON16*)m_pRecord)->cpts);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYLINE16()
{
	// TRACE0("**EMR_POLYLINE16\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawLinesS(((EMRPOLYLINE16*)m_pRecord)->apts,
							 ((EMRPOLYLINE16*)m_pRecord)->cpts);
	return SC_BRK_NOERROR;
}


SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYBEZIERTO16()
{
	// TRACE0("**EMR_POLYBEZIERTO16\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawBezierToS(((EMRPOLYBEZIERTO16*)m_pRecord)->apts,
								((EMRPOLYBEZIERTO16*)m_pRecord)->cpts);
	return SC_BRK_NOERROR;
}


SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYLINETO16()
{
	// TRACE0("**EMR_POLYLINETO16\n");
	ASSERT(m_pRenderer);

	m_pRenderer->SCDrawLinesToS(((EMRPOLYLINETO16*)m_pRecord)->apts,
								((EMRPOLYLINETO16*)m_pRecord)->cpts);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYPOLYLINE16()
{
	// TRACE0("**EMR_POLYPOLYLINE16\n");
	ASSERT(m_pRenderer);

	EMRPOLYPOLYLINE16 *pRec = (EMRPOLYPOLYLINE16*)m_pRecord;
	LPPOINTS pPts = (LPPOINTS)((DWORD*)pRec->aPolyCounts + pRec->nPolys);
	m_pRenderer->SCDrawPolyPolylineS(pPts, pRec->cpts, (DWORD*)pRec->aPolyCounts, pRec->nPolys);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYPOLYGON16()
{
	// TRACE0("**EMR_POLYPOLYGON16\n");
	ASSERT(m_pRenderer);

	EMRPOLYPOLYGON16 *pRec = (EMRPOLYPOLYGON16*)m_pRecord;
	LPPOINTS pPts = (LPPOINTS)((DWORD*)pRec->aPolyCounts + pRec->nPolys);
	m_pRenderer->SCDrawPolyPolygonS(pPts, pRec->cpts, (DWORD*)pRec->aPolyCounts, pRec->nPolys);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYDRAW16()
{
	// TRACE0("**EMR_POLYDRAW16\n");
	ASSERT(m_pRenderer);

	EMRPOLYDRAW16 *pRec = (EMRPOLYDRAW16*)m_pRecord;

	BYTE* pTypes = (BYTE*)((POINTS*)pRec->apts + pRec->cpts);
	m_pRenderer->SCDrawPolyDrawS(pRec->apts,
								pRec->cpts,
								pTypes);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATEMONOBRUSH()
{
	// TRACE0("**EMR_CREATEMONOBRUSH\n");
	ASSERT(m_pRenderer);

	EMRCREATEMONOBRUSH *pRec = (EMRCREATEMONOBRUSH*)m_pRecord;

	// SCPlayRecord() won't work: the brush will reference a deleted bitmap (XP specific bug?).
	// See comments in the final block.
	SCPlayRecord();

	#ifndef DIB_PAL_INDICES
		#define DIB_PAL_INDICES	2
	#endif
	ASSERT(DIB_PAL_INDICES==pRec->iUsage);

	// DIB_PAL_INDICES management. Special case: the DIB has no color table.
	BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmi); 
	DWORD *pBitsDW     = (DWORD *) ((BYTE *) pRec + pRec->offBits);
	// Attach a monochrome palette to the DIB specification,
	// and create a device-dependent bitmap
	HBITMAP hBm = NULL;
	DWORD dwSize = pBmi->bmiHeader.biSize + 2*sizeof(RGBQUAD);
	BITMAPINFO* pBmi2 = (BITMAPINFO*) new BYTE[dwSize];
	memmove(pBmi2, pBmi, dwSize);
	
	HDC hMonoDC = CreateCompatibleDC(m_hPlayDC); // must be monochrome
	ASSERT(hMonoDC);
	SCFillMonochromePalette(hMonoDC, (PPALETTEENTRY)pBmi2->bmiColors);

	// May create resource leak, as we don't call DeleteObject on hBm (see comments in the final block)
	hBm = CreateDIBitmap(hMonoDC, &pBmi2->bmiHeader, CBM_INIT, pBitsDW, pBmi2, DIB_RGB_COLORS);
	
	DeleteDC(hMonoDC);
	delete [] (BYTE*)pBmi2;
	ASSERT(hBm);

#ifdef _DEBUG
	{// check
		BITMAP bm;
		int iRes = GetObject(hBm, sizeof(BITMAP), &bm);
		ASSERT(iRes);
	}
#endif

	HBRUSH hBrush = CreatePatternBrush(hBm);
	ASSERT(hBrush);
	m_lpEnumHandleTable->objectHandle[pRec->ihBrush] = hBrush;

#if 0
	// Don't do this normal step of the code:
	//		DeleteObject(hBm);
	// It seems that XP's CreatePatternBrush does not make a copy of the given bitmap.
	// To prove it, activate this code, and you will see that GetObjectType
	// will fail on the LogBrushVerif2.lbHatch handle returned by GetObject.
	{
		// Check before delete
		LOGBRUSH LogBrushVerif1;
		ASSERT(::GetObject(hBrush, sizeof(LogBrushVerif1), &LogBrushVerif1));
		ASSERT(LogBrushVerif1.lbStyle==BS_PATTERN);
		int iType1 = GetObjectType((HGDIOBJ)LogBrushVerif1.lbHatch);
		ASSERT(iType1==OBJ_BITMAP);
		
		// Suppose we delete it
		DeleteObject(hBm);
		
		// Check after delete
		LOGBRUSH LogBrushVerif2;
		ASSERT(::GetObject(hBrush, sizeof(LogBrushVerif2), &LogBrushVerif2));
		ASSERT(LogBrushVerif2.lbStyle==BS_PATTERN);
		int iType2 = GetObjectType((HGDIOBJ)LogBrushVerif2.lbHatch);
		ASSERT(iType2==OBJ_BITMAP); // fails
	}
#endif

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATEDIBPATTERNBRUSHPT()
{
	// TRACE0("**EMR_CREATEDIBPATTERNBRUSHPT\n");
	ASSERT(m_pRenderer);

#if 0
	// On NT.SP5, Same problem as monobrush
	SCPlayRecord();
#else
	EMRCREATEDIBPATTERNBRUSHPT *pRec = (EMRCREATEDIBPATTERNBRUSHPT*)m_pRecord;
	ASSERT(DIB_PAL_COLORS==pRec->iUsage || DIB_RGB_COLORS==pRec->iUsage);
	BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmi);

	// Note: On Windows 95 and Windows 98, "creating brushes from bitmaps or DIBs
	//	larger than 8x8 pixels is not supported". So we should restrict the pattern to 8x8.
	// But what if the EMF was created on NT/2K/XP? It seems that GDI doesn't shrink the
	// pattern in order to render the EMF OS-independently.

	DWORD dwSize = pRec->cbBmi + pRec->cbBits;
	// should agree with:
	// BITMAPINFOHEADER* pBmih = &pBmi->bmiHeader;
	// DWORD dwSize = pBmih->biSize +
	//						ColorTableSize(pBmih) +
	//						((pBmih->biSizeImage) ? pBmih->biSizeImage :
	//						abs(pBmih->biHeight)*WIDTHBYTES(pBmih->biWidth*pBmih->biBitCount));

	// May create resource leak, as we don't call GlobalFree on hMem
	// (in fact, this is the sole purpose of this code)
	HGLOBAL hMem = GlobalAlloc(GPTR, dwSize);
	BITMAPINFO* pBmi2 = (BITMAPINFO*)GlobalLock(hMem);
	memmove(pBmi2, pBmi, dwSize);

	HBRUSH hBrush = CreateDIBPatternBrushPt(pBmi2, pRec->iUsage);

	GlobalUnlock(hMem);
	ASSERT(hBrush);
	m_lpEnumHandleTable->objectHandle[pRec->ihBrush] = hBrush;

	#ifdef _DEBUG
	{// Check after play
		LOGBRUSH LogBrushVerif;
		ASSERT(::GetObject((HBRUSH)m_lpEnumHandleTable->objectHandle[pRec->ihBrush], sizeof(LogBrushVerif), &LogBrushVerif));
		ASSERT(LogBrushVerif.lbStyle==BS_DIBPATTERN);
	}
	#endif
#endif
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTCREATEPEN()
{
	// TRACE0("**EMR_EXTCREATEPEN\n");
	ASSERT(m_pRenderer);

	SCPlayRecord();

	// EMREXTCREATEPEN *pRec = (EMREXTCREATEPEN*)m_pRecord;
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYTEXTOUTA()
{
	// TRACE0("**EMR_POLYTEXTOUTA\n");
	ASSERT(m_pRenderer);

	EMRPOLYTEXTOUTA *pRec = (EMRPOLYTEXTOUTA*)m_pRecord;
	for (int i=0; (i<pRec->cStrings); i++)
	{
		SCEmfTextoutA((EMREXTTEXTOUTA*)pRec, &pRec->aemrtext[i]);
	}

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPOLYTEXTOUTW()
{
	// TRACE0("**EMR_POLYTEXTOUTW\n");
	ASSERT(m_pRenderer);

	EMRPOLYTEXTOUTW *pRec = (EMRPOLYTEXTOUTW*)m_pRecord;
	for (int i=0; (i<pRec->cStrings); i++)
	{
		SCEmfTextoutW((EMREXTTEXTOUTW*)pRec, &pRec->aemrtext[i]);
	}

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETICMMODE()
{
	// TRACE0("**EMR_SETICMMODE\n");
	ASSERT(m_pRenderer);

//		EMRSETICMMODE *pRec = (EMRSETICMMODE*)m_pRecord;

	// MSDN: see "DIBINFO.C"
	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATECOLORSPACE()
{
	// TRACE0("**EMR_CREATECOLORSPACE\n");
	ASSERT(m_pRenderer);

//		EMRCREATECOLORSPACE *pRec = (EMRCREATECOLORSPACE*)m_pRecord;

	// MSDN: see "Basic ICM 2.0 Functions for Use Within a Device Context"
	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCREATECOLORSPACEW()
{
	// TRACE0("**EMR_CREATECOLORSPACE\n");
	ASSERT(m_pRenderer);

	//EMRCREATECOLORSPACEW *pRec = (EMRCREATECOLORSPACEW*)m_pRecord;

	// MSDN: see "Basic ICM 2.0 Functions for Use Within a Device Context"
	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETCOLORSPACE()
{
	// TRACE0("**EMR_SETCOLORSPACE\n");
	ASSERT(m_pRenderer);

//		EMRSELECTCOLORSPACE *pRec = (EMRSELECTCOLORSPACE*)m_pRecord;

	// MSDN: see "Basic ICM 2.0 Functions for Use Within a Device Context"
	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfDELETECOLORSPACE()
{
	// TRACE0("**EMR_DELETECOLORSPACE\n");
	ASSERT(m_pRenderer);

//		EMRDELETECOLORSPACE *pRec = (EMRDELETECOLORSPACE*)m_pRecord;

	// MSDN: see "Basic ICM 2.0 Functions for Use Within a Device Context"
	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfGLSRECORD()
{
	// TRACE0("**EMR_GLSRECORD\n");
	ASSERT(m_pRenderer);

	SC_BRKRESULT error(SC_BRK_NOERROR);
	EMRGLSRECORD *pRec = (EMRGLSRECORD*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return error;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfGLSBOUNDEDRECORD()
{
	// TRACE0("**EMR_GLSBOUNDEDRECORD\n");
	ASSERT(m_pRenderer);

//		EMRGLSBOUNDEDRECORD *pRec = (EMRGLSBOUNDEDRECORD*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfPIXELFORMAT()
{
	// TRACE0("**EMR_PIXELFORMAT\n");
	ASSERT(m_pRenderer);

//		EMRPIXELFORMAT *pRec = (EMRPIXELFORMAT*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfDRAWESCAPE()
{
	// TRACE0("**EMR_DRAWESCAPE\n");
	ASSERT(m_pRenderer);

	// EMR *pRec = (EMR*)m_pRecord;

	// Do not play Escape
	// Anyway, now it's SCEMF_RESERVED_105
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfEXTESCAPE()
{
	// TRACE0("**EMR_EXTESCAPE\n");
	ASSERT(m_pRenderer);

	// EMR *pRec = (EMR*)m_pRecord;

	// Do not play Escape
	// Anyway, now it's SCEMF_RESERVED_106
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSTARTDOC()
{
	// TRACE0("**EMR_STARTDOC\n");
	ASSERT(m_pRenderer);

	// EMR *pRec = (EMR*)m_pRecord;

	// Do not play Escape
	// Anyway, now it's SCEMF_RESERVED_107
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSMALLTEXTOUT()
{
	// TRACE0("**EMR_SMALLTEXTOUT\n");
	ASSERT(m_pRenderer);

	// WARNING: it's SCEMF_RESERVED_108
	SCEMRSMALLTEXTOUTA *pRec = (SCEMRSMALLTEXTOUTA*)m_pRecord;
	// TODO: review this line
	SCEMRSMALLTEXTOUTA2 *pTextRec2 = (SCEMRSMALLTEXTOUTA2*)m_pRecord;

	// get the EMRTEXT
	SCEMRSMALLTEXTOUTA *pTextRec = pRec;

	// number of characters	
	INT iNbChars = pTextRec->nChars;
	if (iNbChars<=0)
	{
		// We don't know the behavior of ExtTextOut:
		// - if number of characters is 0
		// - if, moreover, TA_UPDATECP is set in the DC
#if 0
		// if required, play the record
		SCPlayRecord();
#endif
		return SC_BRK_NOERROR;
	}

	UINT uiOptions = pRec->fOptions; // we might have to alter it
	// TODO: review this line
	BOOL bUnicode = (uiOptions == 0x100);

	// Build the string
	LPWSTR pwStr = NULL;
	if (bUnicode) 
	{
		// The string is in unicode -> we let it as is.
		// Offset is from the beginning of the struct and is in bytes.
		// 	
		// Get the unicode string
		pwStr = (LPWSTR)pTextRec->Text;

#if 0
		// TODO:
		// Translate glyph indices to code points when the rasterizer does not
		// support string measurement for glyph indices.
		if ((uiOptions & ETO_GLYPH_INDEX) && (!m_pRenderer->SCCanMeasureGlyphs())
		{
			int iRes = SCUnicodeCharsFromGlyphs(m_hPlayDC, pString, iNbChars);
			if (iRes)
				uiOptions &= ~ETO_GLYPH_INDEX;
		}
#endif
	} else
	{
		// The string is single-byte encoded.
		// Get adress of the string	then construct the string.			
		LPSTR pString = (LPSTR)pTextRec->Text;
		// TODO: review this line
		if (uiOptions==0x204)
			pString = (char *)pTextRec2->Text;

		pwStr = new wchar_t[iNbChars+1];
		if (pwStr)
		{// We will use UNICODE strings exclusively
			pwStr[iNbChars] = 0;
			int iNum = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pString, iNbChars, pwStr, iNbChars+1);
			#ifdef _DEBUG
			if (0==iNum)
			{
				DWORD dwError = GetLastError();
			}
			#endif
			ASSERT(iNum==iNbChars);
		}
	}
	ASSERT(pwStr);

	// Get intercharacter spacing array.
	// Be careful ! there are cases where OffDx==0.
	// TODO: review this line
	DWORD OffDx = 0;//pTextRec->offDx;
	INT *lpDx = (INT*)((BYTE*)pRec + OffDx);
	//if (0==OffDx)
	{
		// If the lpDx parameter is NULL, the ExtTextOut function uses the default
		// spacing between characters. So we do.
		//
		// Compute intercharacter spacing, based on each character width.
		lpDx = new INT[iNbChars];
		BOOL bOk = SCGetTextCharWidthsW(m_hPlayDC, pwStr, iNbChars, lpDx);
		ASSERT(bOk);
	} // else use distances between origins of adjacent character cells as passed to ExtTextOut.

	CRect rcText;
	if (uiOptions==0x204)
	{
		rcText.CopyRect((LPRECT)&pTextRec2->rclBoundsSC);
	} else
	{
		uiOptions &= ~ETO_CLIPPED; // we don't have the rectangle
		rcText.SetRectEmpty();
	}
	if (GM_COMPATIBLE==pTextRec->iGraphicsMode)
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPRECT)&rcText, pwStr, iNbChars, lpDx, pRec->exScale, pRec->eyScale);
	else
		m_pRenderer->SCDrawText(pTextRec->ptlReference.x, pTextRec->ptlReference.y, uiOptions,
		(LPRECT)&rcText, pwStr, iNbChars, lpDx, 1, 1);
	

	// Clean up
	if (!bUnicode)
		delete [] pwStr;
	if (0==OffDx)
		delete [] lpDx;

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfFORCEUFIMAPPING()
{
	// TRACE0("**EMR_FORCEUFIMAPPING\n");
	ASSERT(m_pRenderer);

	// EMR *pRec = (EMR*)m_pRecord;

	// Danger if it writes to the DC
	// Worse if it attempts escape-like behavior
	//SCPlayRecord();

	// Anyway, now it's SCEMF_RESERVED_109
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfNAMEDESCAPE()
{
	// TRACE0("**EMR_NAMEDESCAPE\n");
	ASSERT(m_pRenderer);

	// Do not play escape
	// Anyway, now it's SCEMF_RESERVED_110
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCOLORCORRECTPALETTE()
{
	// TRACE0("**EMR_COLORCORRECTPALETTE\n");
	ASSERT(m_pRenderer);

//		EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETICMPROFILEA()
{
	// TRACE0("**EMR_SETICMPROFILEA\n");
	ASSERT(m_pRenderer);

//		EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETICMPROFILEW()
{
	// TRACE0("**EMR_SETICMPROFILEW\n");
	ASSERT(m_pRenderer);

//		EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfALPHABLEND()
{
	// TRACE0("**EMR_ALPHABLEND\n");
	ASSERT(m_pRenderer);

	EMRALPHABLEND *pRec = (EMRALPHABLEND*)m_pRecord;

	ASSERT(pRec->offBmiSrc && pRec->offBitsSrc);

	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);

		// Destination
		CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);

		m_pRenderer->SCDrawImageAlpha(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc,
			pRec->dwRop, &pRec->xformSrc, pRec->crBkColorSrc);
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfALPHADIBBLEND()
{
	// TRACE0("**EMR_SETLAYOUT\n");
	ASSERT(m_pRenderer);

	// Now it's SCEMF_SETLAYOUT
	//DWORD SetLayout(
	//	HDC hdc,             // handle to DC
	//	DWORD dwLayout,      // layout options
	//	);
	// dwLayout==LAYOUT_BITMAPORIENTATIONPRESERVED Disables any reflection during BitBlt
	//											   and StretchBlt operations. 
	//           LAYOUT_RTL						   Sets the default horizontal layout
	//											   to be right to left. 
	// OSes: Windows 2000 and later, Windows 98 and later

	// TODO_EMF: horizontal reflection when the LAYOUT_RTL flag is selected
	// m_pRenderer->SCSetLayout(((EMRSETLAYOUT*)m_pRecord)->iMode);

	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfTRANSPARENTBLT()
{
	// TRACE0("**EMR_TRANSPARENTBLT\n");
	ASSERT(m_pRenderer);

	EMRTRANSPARENTBLT *pRec = (EMRTRANSPARENTBLT*)m_pRecord;
	ASSERT(pRec->offBmiSrc && pRec->offBitsSrc);

	// Check if there is a bitmap
	if (pRec->offBmiSrc && pRec->offBitsSrc)
	{
		// Source
		CRect RcSrc(pRec->xSrc, pRec->ySrc, pRec->xSrc + pRec->cxSrc, pRec->ySrc + pRec->cySrc);
		BITMAPINFO *pBmi    = (BITMAPINFO *) ((BYTE *)pRec + pRec->offBmiSrc); 
		BYTE *pBits     = (BYTE *) ((BYTE *) pRec + pRec->offBitsSrc);

		// Destination
		CRect RcDest(pRec->xDest, pRec->yDest, pRec->xDest + pRec->cxDest, pRec->yDest + pRec->cyDest);

		m_pRenderer->SCDrawImageTransparent(&RcDest, &RcSrc, pBits, pBmi, pRec->iUsageSrc,
			pRec->dwRop, &pRec->xformSrc, pRec->crBkColorSrc);
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfTRANSPARENTDIB()
{
	// TRACE0("**EMR_TRANSPARENTDIB\n");
	ASSERT(m_pRenderer);

	// EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	// Anyway, now it's SCEMF_RESERVED_117
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfGRADIENTFILL()
{
	// TRACE0("**EMR_GRADIENTFILL\n");
	ASSERT(m_pRenderer);

	EMRGRADIENTFILL *pRec = (EMRGRADIENTFILL*)m_pRecord;

	LPBYTE pMesh = (LPBYTE)&pRec->Ver[pRec->nVer];
	m_pRenderer->SCGradientFill(pRec->Ver, pRec->nVer, pMesh, pRec->nTri, pRec->ulMode);
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETLINKEDUFIS()
{
	// TRACE0("**EMR_SETLINKEDUFIS\n");
	ASSERT(m_pRenderer);

//		EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	// Anyway, now it's SCEMF_RESERVED_119
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfSETTEXTJUSTIFICATION()
{
	// TRACE0("**EMR_SETTEXTJUSTIFICATION\n");
	ASSERT(m_pRenderer);

//		EMR *pRec = (EMR*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	// Anyway, now it's SCEMF_RESERVED_120
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMFgdiParser::OnEmfCOLORMATCHTOTARGETW()
{
	// TRACE0("**EMR_CREATECOLORSPACE\n");
	ASSERT(m_pRenderer);

//		COLORMATCHTOTARGETW *pRec = (COLORMATCHTOTARGETW*)m_pRecord;

	// TODO_EMF: Place code here to handle EMF record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}


SC_BRKRESULT CSCEMFgdiParser::OnBrkUNKRecord(long lMsg)
{
	// TRACE0("**OnBrkUNKRecord\n");
	ASSERT(m_pRenderer);

	UNUSED(lMsg);   // unused in release builds

	// By default, play unkown record
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////

// called before enumeration
SC_BRKRESULT CSCEMFgdiParser::SCBeginBreak(HDC hDC, LPVOID lpData, CONST RECT *lpRect)
{
	SC_BRKRESULT error(SC_BRK_NOERROR);

	ASSERT(m_pRenderer);
	if (!m_pRenderer)
		return SC_BRK_NORENDERER;

	GetWorldTransform(hDC, &m_xformOrig);

	// call inherited
	error = SCBrkEMF::SCBeginBreak(hDC, lpData, lpRect);

	// before enumeration
	// clone the DC so that this class does not write on the destination DC
	m_hDestDC = hDC;					 // for GDI+

#ifdef SC_USE_DCCLONER
	// TODO: fix the bitmap problem in the cloner
	m_pCloneDC = new SCDCCloner(hDC);
	ASSERT(m_pCloneDC);
	m_hPlayDC = m_pCloneDC->SCGetHDC();	 // for GDI
#else
	m_hPlayDC = hDC;					 // for GDI
#endif

	ASSERT(m_hPlayDC && m_hDestDC);
		
	return error;
}


//////////////////////////////////////////////////////////////////////
// Message map
//////////////////////////////////////////////////////////////////////
SC_BEGIN_MESSAGE_MAP(CSCEMFgdiParser, SCBrkEMF)
	ON_VECTRECORD(SCEMF_HEADER, OnEmfHEADER)
	ON_VECTRECORD(SCEMF_POLYBEZIER, OnEmfPOLYBEZIER)
	ON_VECTRECORD(SCEMF_POLYGON, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYLINE, OnEmfPOLYLINE)
	ON_VECTRECORD(SCEMF_POLYBEZIERTO, OnEmfPOLYBEZIERTO)
	ON_VECTRECORD(SCEMF_POLYLINETO, OnEmfPOLYLINETO)
	ON_VECTRECORD(SCEMF_POLYPOLYLINE, OnEmfPOLYPOLYLINE)
	ON_VECTRECORD(SCEMF_POLYPOLYGON, OnEmfPOLYPOLYGON)
	ON_VECTRECORD(SCEMF_SETWINDOWEXTEX, OnEmfSETWINDOWEXTEX)
	ON_VECTRECORD(SCEMF_SETWINDOWORGEX, OnEmfSETWINDOWORGEX)
	ON_VECTRECORD(SCEMF_SETVIEWPORTEXTEX, OnEmfSETVIEWPORTEXTEX)
	ON_VECTRECORD(SCEMF_SETVIEWPORTORGEX, OnEmfSETVIEWPORTORGEX)
	ON_VECTRECORD(SCEMF_SETBRUSHORGEX, OnEmfSETBRUSHORGEX)
	ON_VECTRECORD(SCEMF_EOF, OnEmfEOF)
	ON_VECTRECORD(SCEMF_SETPIXELV, OnEmfSETPIXELV)
	ON_VECTRECORD(SCEMF_SETMAPPERFLAGS, OnEmfSETMAPPERFLAGS)
	ON_VECTRECORD(SCEMF_SETMAPMODE, OnEmfSETMAPMODE)
	ON_VECTRECORD(SCEMF_SETBKMODE, OnEmfSETBKMODE)
	ON_VECTRECORD(SCEMF_SETPOLYFILLMODE, OnEmfSETPOLYFILLMODE)
	ON_VECTRECORD(SCEMF_SETROP2, OnEmfSETROP2)
	ON_VECTRECORD(SCEMF_SETSTRETCHBLTMODE, OnEmfSETSTRETCHBLTMODE)
	ON_VECTRECORD(SCEMF_SETTEXTALIGN, OnEmfSETTEXTALIGN)
	ON_VECTRECORD(SCEMF_SETCOLORADJUSTMENT, OnEmfSETCOLORADJUSTMENT)
	ON_VECTRECORD(SCEMF_SETTEXTCOLOR, OnEmfSETTEXTCOLOR)
	ON_VECTRECORD(SCEMF_SETBKCOLOR, OnEmfSETBKCOLOR)
	ON_VECTRECORD(SCEMF_OFFSETCLIPRGN, OnEmfOFFSETCLIPRGN)
	ON_VECTRECORD(SCEMF_MOVETOEX, OnEmfMOVETOEX)
	ON_VECTRECORD(SCEMF_SETMETARGN, OnEmfSETMETARGN)
	ON_VECTRECORD(SCEMF_EXCLUDECLIPRECT, OnEmfEXCLUDECLIPRECT)
	ON_VECTRECORD(SCEMF_INTERSECTCLIPRECT, OnEmfINTERSECTCLIPRECT)
	ON_VECTRECORD(SCEMF_SCALEVIEWPORTEXTEX, OnEmfSCALEVIEWPORTEXTEX)
	ON_VECTRECORD(SCEMF_SCALEWINDOWEXTEX, OnEmfSCALEWINDOWEXTEX)
	ON_VECTRECORD(SCEMF_SAVEDC, OnEmfSAVEDC)
	ON_VECTRECORD(SCEMF_RESTOREDC, OnEmfRESTOREDC)
	ON_VECTRECORD(SCEMF_SETWORLDTRANSFORM, OnEmfSETWORLDTRANSFORM)
	ON_VECTRECORD(SCEMF_MODIFYWORLDTRANSFORM, OnEmfMODIFYWORLDTRANSFORM)
	ON_VECTRECORD(SCEMF_SELECTOBJECT, OnEmfSELECTOBJECT)
	ON_VECTRECORD(SCEMF_CREATEPEN, OnEmfCREATEPEN)
	ON_VECTRECORD(SCEMF_CREATEBRUSHINDIRECT, OnEmfCREATEBRUSHINDIRECT)
	ON_VECTRECORD(SCEMF_DELETEOBJECT, OnEmfDELETEOBJECT)
	ON_VECTRECORD(SCEMF_ANGLEARC, OnEmfANGLEARC)
	ON_VECTRECORD(SCEMF_ELLIPSE, OnEmfELLIPSE)
	ON_VECTRECORD(SCEMF_RECTANGLE, OnEmfRECTANGLE)
	ON_VECTRECORD(SCEMF_ROUNDRECT, OnEmfROUNDRECT)
	ON_VECTRECORD(SCEMF_ARC, OnEmfARC)
	ON_VECTRECORD(SCEMF_CHORD, OnEmfCHORD)
	ON_VECTRECORD(SCEMF_PIE, OnEmfPIE)
	ON_VECTRECORD(SCEMF_SELECTPALETTE, OnEmfSELECTPALETTE)
	ON_VECTRECORD(SCEMF_CREATEPALETTE, OnEmfCREATEPALETTE)
	ON_VECTRECORD(SCEMF_SETPALETTEENTRIES, OnEmfSETPALETTEENTRIES)
	ON_VECTRECORD(SCEMF_RESIZEPALETTE, OnEmfRESIZEPALETTE)
	ON_VECTRECORD(SCEMF_REALIZEPALETTE, OnEmfREALIZEPALETTE)
	ON_VECTRECORD(SCEMF_EXTFLOODFILL, OnEmfEXTFLOODFILL)
	ON_VECTRECORD(SCEMF_LINETO, OnEmfLINETO)
	ON_VECTRECORD(SCEMF_ARCTO, OnEmfARCTO)
	ON_VECTRECORD(SCEMF_POLYDRAW, OnEmfPOLYDRAW)
	ON_VECTRECORD(SCEMF_SETARCDIRECTION, OnEmfSETARCDIRECTION)
	ON_VECTRECORD(SCEMF_SETMITERLIMIT, OnEmfSETMITERLIMIT)
	ON_VECTRECORD(SCEMF_BEGINPATH, OnEmfBEGINPATH)
	ON_VECTRECORD(SCEMF_ENDPATH, OnEmfENDPATH)
	ON_VECTRECORD(SCEMF_CLOSEFIGURE, OnEmfCLOSEFIGURE)
	ON_VECTRECORD(SCEMF_FILLPATH, OnEmfFILLPATH)
	ON_VECTRECORD(SCEMF_STROKEANDFILLPATH, OnEmfSTROKEANDFILLPATH)
	ON_VECTRECORD(SCEMF_STROKEPATH, OnEmfSTROKEPATH)
	ON_VECTRECORD(SCEMF_FLATTENPATH, OnEmfFLATTENPATH)
	ON_VECTRECORD(SCEMF_WIDENPATH, OnEmfWIDENPATH)
	ON_VECTRECORD(SCEMF_SELECTCLIPPATH, OnEmfSELECTCLIPPATH)
	ON_VECTRECORD(SCEMF_ABORTPATH, OnEmfABORTPATH)
	ON_VECTRECORD(SCEMF_GDICOMMENT, OnEmfGDICOMMENT)
	ON_VECTRECORD(SCEMF_FILLRGN, OnEmfFILLRGN)
	ON_VECTRECORD(SCEMF_FRAMERGN, OnEmfFRAMERGN)
	ON_VECTRECORD(SCEMF_INVERTRGN, OnEmfINVERTRGN)
	ON_VECTRECORD(SCEMF_PAINTRGN, OnEmfPAINTRGN)
	ON_VECTRECORD(SCEMF_EXTSELECTCLIPRGN, OnEmfEXTSELECTCLIPRGN)
	ON_VECTRECORD(SCEMF_BITBLT, OnEmfBITBLT)
	ON_VECTRECORD(SCEMF_STRETCHBLT, OnEmfSTRETCHBLT)
	ON_VECTRECORD(SCEMF_MASKBLT, OnEmfMASKBLT)
	ON_VECTRECORD(SCEMF_PLGBLT, OnEmfPLGBLT)
	ON_VECTRECORD(SCEMF_SETDIBITSTODEVICE, OnEmfSETDIBITSTODEVICE)
	ON_VECTRECORD(SCEMF_STRETCHDIBITS, OnEmfSTRETCHDIBITS)
	ON_VECTRECORD(SCEMF_EXTCREATEFONTINDIRECTW, OnEmfEXTCREATEFONTINDIRECTW)
	ON_VECTRECORD(SCEMF_EXTTEXTOUTA, OnEmfEXTTEXTOUTA)
	ON_VECTRECORD(SCEMF_EXTTEXTOUTW, OnEmfEXTTEXTOUTW)
	ON_VECTRECORD(SCEMF_POLYBEZIER16, OnEmfPOLYBEZIER16)
	ON_VECTRECORD(SCEMF_POLYGON16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYLINE16, OnEmfPOLYLINE16)
	ON_VECTRECORD(SCEMF_POLYBEZIERTO16, OnEmfPOLYBEZIERTO16)
	ON_VECTRECORD(SCEMF_POLYLINETO16, OnEmfPOLYLINETO16)
	ON_VECTRECORD(SCEMF_POLYPOLYLINE16, OnEmfPOLYPOLYLINE16)
	ON_VECTRECORD(SCEMF_POLYPOLYGON16, OnEmfPOLYPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYDRAW16, OnEmfPOLYDRAW16)
	ON_VECTRECORD(SCEMF_CREATEMONOBRUSH, OnEmfCREATEMONOBRUSH)
	ON_VECTRECORD(SCEMF_CREATEDIBPATTERNBRUSHPT, OnEmfCREATEDIBPATTERNBRUSHPT)
	ON_VECTRECORD(SCEMF_EXTCREATEPEN, OnEmfEXTCREATEPEN)
	ON_VECTRECORD(SCEMF_POLYTEXTOUTA, OnEmfPOLYTEXTOUTA)
	ON_VECTRECORD(SCEMF_POLYTEXTOUTW, OnEmfPOLYTEXTOUTW)
	ON_VECTRECORD(SCEMF_SETICMMODE, OnEmfSETICMMODE)
	ON_VECTRECORD(SCEMF_CREATECOLORSPACE, OnEmfCREATECOLORSPACE)
	ON_VECTRECORD(SCEMF_SETCOLORSPACE, OnEmfSETCOLORSPACE)
	ON_VECTRECORD(SCEMF_DELETECOLORSPACE, OnEmfDELETECOLORSPACE)
	ON_VECTRECORD(SCEMF_GLSRECORD, OnEmfGLSRECORD)
	ON_VECTRECORD(SCEMF_GLSBOUNDEDRECORD, OnEmfGLSBOUNDEDRECORD)
	ON_VECTRECORD(SCEMF_PIXELFORMAT, OnEmfPIXELFORMAT)
	ON_VECTRECORD(SCEMF_DRAWESCAPE, OnEmfDRAWESCAPE)
	ON_VECTRECORD(SCEMF_EXTESCAPE, OnEmfEXTESCAPE)
	ON_VECTRECORD(SCEMF_STARTDOC, OnEmfSTARTDOC)
	ON_VECTRECORD(SCEMF_SMALLTEXTOUT, OnEmfSMALLTEXTOUT)
	ON_VECTRECORD(SCEMF_FORCEUFIMAPPING, OnEmfFORCEUFIMAPPING)
	ON_VECTRECORD(SCEMF_NAMEDESCAPE, OnEmfNAMEDESCAPE)
	ON_VECTRECORD(SCEMF_COLORCORRECTPALETTE, OnEmfCOLORCORRECTPALETTE)
	ON_VECTRECORD(SCEMF_SETICMPROFILEA, OnEmfSETICMPROFILEA)
	ON_VECTRECORD(SCEMF_SETICMPROFILEW, OnEmfSETICMPROFILEW)
	ON_VECTRECORD(SCEMF_ALPHABLEND, OnEmfALPHABLEND)
	ON_VECTRECORD(SCEMF_ALPHADIBBLEND, OnEmfALPHADIBBLEND)
	ON_VECTRECORD(SCEMF_TRANSPARENTBLT, OnEmfTRANSPARENTBLT)
	ON_VECTRECORD(SCEMF_TRANSPARENTDIB, OnEmfTRANSPARENTDIB)
	ON_VECTRECORD(SCEMF_GRADIENTFILL, OnEmfGRADIENTFILL)
	ON_VECTRECORD(SCEMF_SETLINKEDUFIS, OnEmfSETLINKEDUFIS)
	ON_VECTRECORD(SCEMF_SETTEXTJUSTIFICATION, OnEmfSETTEXTJUSTIFICATION)
	ON_VECTRECORD(SCEMF_COLORMATCHTOTARGETW, OnEmfCOLORMATCHTOTARGETW)
	ON_VECTRECORD(SCEMF_CREATECOLORSPACEW, OnEmfCREATECOLORSPACEW)
SC_END_MESSAGE_MAP()

