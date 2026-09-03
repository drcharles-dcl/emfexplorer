/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMF2Text.h"
#include "SCEMFDefs.h"
#include "SCEMFTxtUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	#define SC_LOG_RAWDATA_ONLY

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCEMF2Text::CSCEMF2Text():
	SCBrkEMF(),
	m_bLogRecord(FALSE),
	m_iCbThreshold(100)
{
}

CSCEMF2Text::~CSCEMF2Text()
{
}

//////////////////////////////////////////////////////////////////////
// Events methods
//////////////////////////////////////////////////////////////////////
SC_BRKRESULT CSCEMF2Text::OnEmfHEADER()
{
	// TRACE0("**EMR_HEADER\n");
	ENHMETAHEADER *pRec = (ENHMETAHEADER*)m_pRecord;
	if (m_pIBrkCallBack)
	{
		if (0==m_pIBrkCallBack->OnSCBrkMaxPos(pRec->nRecords))
			return SC_BRK_STOPBREAKING;
		
		if (m_bLogRecord)
		{
			CString sExtra;
			
#if(WINVER >= 0x0400)
			sExtra.Format(_T("{ Bounds(%d,%d,%d,%d), Frame(%d,%d,%d,%d), ver(0x%x), size(%d), recs(%d), handles(%d), pals(%d), dev_pix(%d,%d), dev_mil(%d,%d), pixf_size(%d), pixf_ofs(%d), openGL(%d) }"),
				SC_PARAM_RCLBOUNDS(pRec->rclBounds), SC_PARAM_RECT(pRec->rclFrame), pRec->nVersion, pRec->nBytes, pRec->nRecords, pRec->nHandles, pRec->nPalEntries, SC_PARAM_SIZE(pRec->szlDevice),SC_PARAM_SIZE(pRec->szlMillimeters),
				pRec->cbPixelFormat, pRec->offPixelFormat, pRec->bOpenGL);
#else
			sExtra.Format(_T("{ Bounds(%d,%d,%d,%d), Frame(%d,%d,%d,%d), ver(%d), size(%d), recs(%d), handles(%d), pals(%d), dev_pix(%d,%d), dev_mil(%d,%d) }"),
				SC_PARAM_RCLBOUNDS(pRec->rclBounds), SC_PARAM_RECT(pRec->rclFrame), pRec->nVersion, pRec->nBytes, pRec->nRecords, pRec->nHandles, pRec->nPalEntries, SC_PARAM_SIZE(pRec->szlDevice),SC_PARAM_SIZE(pRec->szlMillimeters));
#endif /* WINVER >= 0x0400 */
			
			return SCExtraLog(sExtra);
		}
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYGON()
{
	// TRACE0("**EMR_POLYGON\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatEmrPolygon((EMRPOLYGON*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYPOLYGON()
{
	// TRACE0("**EMR_POLYPOLYGON\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatEmrPolyPolygon((EMRPOLYPOLYGON*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETWINDOWEXTEX()
{
	// TRACE0("**EMR_SETWINDOWEXTEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("szlExtent(%d,%d)"),
			SC_PARAM_SIZE(((EMRSETWINDOWEXTEX*)m_pRecord)->szlExtent));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETWINDOWORGEX()
{
	// TRACE0("**EMR_SETWINDOWORGEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ptlOrigin(%d,%d)"),
			SC_PARAM_POINT(((EMRSETWINDOWORGEX*)m_pRecord)->ptlOrigin));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETVIEWPORTEXTEX()
{
	// TRACE0("**EMR_SETVIEWPORTEXTEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("szlExtent(%d,%d)"),
			SC_PARAM_SIZE(((EMRSETVIEWPORTEXTEX*)m_pRecord)->szlExtent));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETVIEWPORTORGEX()
{
	// TRACE0("**EMR_SETVIEWPORTORGEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ptlOrigin(%d,%d)"),
			SC_PARAM_POINT(((EMRSETVIEWPORTORGEX*)m_pRecord)->ptlOrigin));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETBRUSHORGEX()
{
	// TRACE0("**EMR_SETBRUSHORGEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ptlOrigin(%d,%d)"),
			SC_PARAM_POINT(((EMRSETBRUSHORGEX*)m_pRecord)->ptlOrigin));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEOF()
{
	// TRACE0("**EMR_EOF\n");
	if (m_bLogRecord)
	{
		EMREOF* pRec = (EMREOF*)m_pRecord;

		int iSizeLast = pRec->nSizeLast;
		if (pRec->nPalEntries && pRec->offPalEntries)
		{
			BYTE* p = (BYTE*)pRec + pRec->offPalEntries;
			p += pRec->nPalEntries*sizeof(PALETTEENTRY);
			iSizeLast = *((DWORD*)p);
		}

		CString sExtra;
		sExtra.Format(_T("nPalEntries:%d, offPalEntries:%d, nSizeLast:%d"),
			pRec->nPalEntries,
			pRec->offPalEntries,
			iSizeLast);
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETPIXELV()
{
	// TRACE0("**EMR_SETPIXELV\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ptlPixel(%d,%d), crColor(") SC_STRFMT_COLOR _T(")"),
			SC_PARAM_POINT(((EMRSETPIXELV*)m_pRecord)->ptlPixel),
						 ((EMRSETPIXELV*)m_pRecord)->crColor);
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETMAPPERFLAGS()
{
	// TRACE0("**EMR_SETMAPPERFLAGS\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("dwFlags(%d)"),
			((EMRSETMAPPERFLAGS*)m_pRecord)->dwFlags);
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETMAPMODE()
{
	// TRACE0("**EMR_SETMAPMODE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETMAPMODE*)m_pRecord)->iMode,
			SCMapModeText(((EMRSETMAPMODE*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETBKMODE()
{
	// TRACE0("**EMR_SETBKMODE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETBKMODE*)m_pRecord)->iMode,
			SCBkModeText(((EMRSETBKMODE*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETPOLYFILLMODE()
{
	// TRACE0("**EMR_SETPOLYFILLMODE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETPOLYFILLMODE*)m_pRecord)->iMode,
			SCPolyFillModeText(((EMRSETPOLYFILLMODE*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETROP2()
{
	// TRACE0("**EMR_SETROP2\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETROP2*)m_pRecord)->iMode,
			SCRop2Text(((EMRSETROP2*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETSTRETCHBLTMODE()
{
	// TRACE0("**EMR_SETSTRETCHBLTMODE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETSTRETCHBLTMODE*)m_pRecord)->iMode,
			SCStretchBltModeText(((EMRSETSTRETCHBLTMODE*)m_pRecord)->iMode)); 

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETTEXTALIGN()
{
	// TRACE0("**EMR_SETTEXTALIGN\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_MODE2,
			((EMRSETTEXTALIGN*)m_pRecord)->iMode,
			SCTextAlignModeText(((EMRSETTEXTALIGN*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETTEXTCOLOR()
{
	// TRACE0("**EMR_SETTEXTCOLOR\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_COLOR,
			((EMRSETTEXTCOLOR*)m_pRecord)->crColor);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETBKCOLOR()
{
	// TRACE0("**EMR_SETBKCOLOR\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_COLOR,
			((EMRSETBKCOLOR*)m_pRecord)->crColor);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfOFFSETCLIPRGN()
{
	// TRACE0("**EMR_OFFSETCLIPRGN\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T(" ptlOffset(%d,%d)"),
			SC_PARAM_POINT(((EMROFFSETCLIPRGN*)m_pRecord)->ptlOffset));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfMOVETOEX()
{
	// TRACE0("**EMR_MOVETOEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T(" ptl(%d,%d)"),
			SC_PARAM_POINT(((EMRMOVETOEX*)m_pRecord)->ptl));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXCLUDECLIPRECT()
{
	// TRACE0("**EMR_EXCLUDECLIPRECT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("rclClip(%d,%d,%d,%d)"),
			SC_PARAM_RECT(((EMREXCLUDECLIPRECT*)m_pRecord)->rclClip));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfINTERSECTCLIPRECT()
{
	// TRACE0("**EMR_INTERSECTCLIPRECT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("rclClip(%d,%d,%d,%d)"),
			SC_PARAM_RECT(((EMRINTERSECTCLIPRECT*)m_pRecord)->rclClip));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSCALEVIEWPORTEXTEX()
{
	// TRACE0("**EMR_SCALEVIEWPORTEXTEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("xNumDenom(%d,%d), yNumDenom(%d,%d)"),
			((EMRSCALEVIEWPORTEXTEX*)m_pRecord)->xNum,
			((EMRSCALEVIEWPORTEXTEX*)m_pRecord)->xDenom,
			((EMRSCALEVIEWPORTEXTEX*)m_pRecord)->yNum,
			((EMRSCALEVIEWPORTEXTEX*)m_pRecord)->yDenom
			);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSCALEWINDOWEXTEX()
{
	// TRACE0("**EMR_SCALEWINDOWEXTEX\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("xNumDenom(%d,%d), yNumDenom(%d,%d)"),
			((EMRSCALEWINDOWEXTEX*)m_pRecord)->xNum,
			((EMRSCALEWINDOWEXTEX*)m_pRecord)->xDenom,
			((EMRSCALEWINDOWEXTEX*)m_pRecord)->yNum,
			((EMRSCALEWINDOWEXTEX*)m_pRecord)->yDenom
			);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfRESTOREDC()
{
	// TRACE0("**EMR_RESTOREDC\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("iRelative(%d)"),
			((EMRRESTOREDC*)m_pRecord)->iRelative);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETWORLDTRANSFORM()
{
	// TRACE0("**EMR_SETWORLDTRANSFORM\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_XFORM,
			SC_PARAM_XFORM(((EMRSETWORLDTRANSFORM*)m_pRecord)->xform));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfMODIFYWORLDTRANSFORM()
{
	// TRACE0("**EMR_MODIFYWORLDTRANSFORM\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("%s, ") SC_STRFMT_XFORM,
			SCFormatModifyWorldTransformModeText(((EMRMODIFYWORLDTRANSFORM*)m_pRecord)->iMode),
			SC_PARAM_XFORM(((EMRMODIFYWORLDTRANSFORM*)m_pRecord)->xform));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSELECTOBJECT()
{
	// TRACE0("**EMR_SELECTOBJECT\n");
	if (m_bLogRecord)
	{
		EMRSELECTOBJECT *pRec = (EMRSELECTOBJECT*)m_pRecord;

		// Check stock objects
		HGDIOBJ hGdiObj;
		if (pRec->ihObject & 0x80000000) // this is a stock object
			hGdiObj = GetStockObject( pRec->ihObject & 0x7fffffff );
		else
			hGdiObj = (HGDIOBJ)m_lpEnumHandleTable->objectHandle[pRec->ihObject];

		CString sExtra;
		sExtra.Format(_T("%s=%s"),
			SCObjectIndexText(pRec->ihObject),
			SCGDIObjectTypeText(hGdiObj));


		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATEPEN()
{
	// TRACE0("**EMR_CREATEPEN\n");
	if (m_bLogRecord)
	{
		EMRCREATEPEN *pRec = (EMRCREATEPEN*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihPen(%d), style(%d=%s), width(%d), color(0x%08X)"),
			pRec->ihPen,
			pRec->lopn.lopnStyle,
			SCPenStyleText(pRec->lopn.lopnStyle),
			pRec->lopn.lopnWidth.x,
			pRec->lopn.lopnColor);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATEBRUSHINDIRECT()
{
	// TRACE0("**EMR_CREATEBRUSHINDIRECT\n");
	if (m_bLogRecord)
	{
#ifdef SC_LOG_RAWDATA_ONLY
// raw data
		EMRCREATEBRUSHINDIRECT *pRec = (EMRCREATEBRUSHINDIRECT*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihBrush(%d), style(%d = %s), color(0x%08X), hatch(0x%08X)"),
			pRec->ihBrush,
			pRec->lb.lbStyle,
			SCBrushStyleText(pRec->lb.lbStyle),
			pRec->lb.lbColor,
			pRec->lb.lbHatch);
#else
// interpreted data
		CString sExtra;
		sExtra.Format(_T("ihBrush(%d), %s"),
			((EMRCREATEBRUSHINDIRECT*)m_pRecord)->ihBrush,
			SCLogBrushStyleText(((EMRCREATEBRUSHINDIRECT*)m_pRecord)->lb));
#endif

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfDELETEOBJECT()
{
	// TRACE0("**EMR_DELETEOBJECT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihObject(%d)"),
			((EMRDELETEOBJECT*)m_pRecord)->ihObject);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfANGLEARC()
{
	// TRACE0("**EMR_ANGLEARC\n");
	EMRANGLEARC *pRec = (EMRANGLEARC*)m_pRecord;
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ptlCenter(%d,%d), Radius:%d, StartAngle:%.3f, SweepAngle:%.3f"),
			SC_PARAM_POINT(pRec->ptlCenter),
			pRec->nRadius,
			pRec->eStartAngle,
			pRec->eSweepAngle);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfELLIPSE()
{
	// TRACE0("**EMR_ELLIPSE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Box(%d,%d,%d,%d)"),
			SC_PARAM_RECT(((EMRELLIPSE*)m_pRecord)->rclBox));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfRECTANGLE()
{
	// TRACE0("**EMR_RECTANGLE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Box(%d,%d,%d,%d)"),
			SC_PARAM_RECT(((EMRRECTANGLE*)m_pRecord)->rclBox));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfROUNDRECT()
{
	// TRACE0("**EMR_ROUNDRECT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Box(%d,%d,%d,%d), Corner(%d,%d)"),
			SC_PARAM_RECT(((EMRROUNDRECT*)m_pRecord)->rclBox),
			SC_PARAM_SIZE(((EMRROUNDRECT*)m_pRecord)->szlCorner));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfARC()
{
	// TRACE0("**EMR_ARC\n");
	EMRARC *pRec = (EMRARC*)m_pRecord;
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Box(%d,%d,%d,%d), Start(%d,%d), End(%d,%d)"),
			SC_PARAM_RECT(pRec->rclBox),
			SC_PARAM_POINT(pRec->ptlStart),
			SC_PARAM_POINT(pRec->ptlEnd));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSELECTPALETTE()
{
	// TRACE0("**EMR_SELECTPALETTE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihPal(%s)"),
			SCObjectIndexText(((EMRSELECTPALETTE*)m_pRecord)->ihPal));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATEPALETTE()
{
	// TRACE0("**EMR_CREATEPALETTE\n");
	if (m_bLogRecord)
	{
		EMRCREATEPALETTE *pRec = (EMRCREATEPALETTE*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihPal(%d) LOGPAL[ver:%d, entries:%d]"),
			pRec->ihPal, pRec->lgpl.palVersion, pRec->lgpl.palNumEntries);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETPALETTEENTRIES()
{
	// TRACE0("**EMR_SETPALETTEENTRIES\n");
	EMRSETPALETTEENTRIES *pRec = (EMRSETPALETTEENTRIES*)m_pRecord;
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihPal(%d) [iStart:%d, cEntries:%d]"),
			pRec->ihPal, pRec->iStart, pRec->cEntries);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfRESIZEPALETTE()
{
	// TRACE0("**EMR_RESIZEPALETTE\n");
	if (m_bLogRecord)
	{
		EMRRESIZEPALETTE *pRec = (EMRRESIZEPALETTE*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihPal(%d) [cEntries:%d]"),
			pRec->ihPal, pRec->cEntries);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTFLOODFILL()
{
	// TRACE0("**EMR_EXTFLOODFILL\n");
	if (m_bLogRecord)
	{
		EMREXTFLOODFILL *pRec = (EMREXTFLOODFILL*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T(" ptlStart(%d,%d), crColor(") SC_STRFMT_COLOR _T(") FillMode(%d=%s)"),
			SC_PARAM_POINT(pRec->ptlStart),
			pRec->crColor,
			pRec->iMode, SCFloodFillModeText(pRec->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfLINETO()
{
	// TRACE0("**EMR_LINETO\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T(" ptl(%d,%d)"),
			SC_PARAM_POINT(((EMRLINETO*)m_pRecord)->ptl));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYDRAW()
{
	// TRACE0("**EMR_POLYDRAW\n");
	if (m_bLogRecord)
	{
		EMRPOLYDRAW *pRec = (EMRPOLYDRAW*)m_pRecord;
		CString sExtra = SCPolygonText(pRec->rclBounds, pRec->aptl, pRec->cptl);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETARCDIRECTION()
{
	// TRACE0("**EMR_SETARCDIRECTION\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Direction:%d"),
			((EMRSETARCDIRECTION*)m_pRecord)->iArcDirection);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETMITERLIMIT()
{
	// TRACE0("**EMR_SETMITERLIMIT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("Limit:%.3f"),
			((EMRSETMITERLIMIT*)m_pRecord)->eMiterLimit);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfFILLPATH()
{
	// TRACE0("**EMR_FILLPATH\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCBoundsText(((EMRFILLPATH*)m_pRecord)->rclBounds);		

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSELECTCLIPPATH()
{
	// TRACE0("**EMR_SELECTCLIPPATH\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatRgnModeText(((EMRSELECTCLIPPATH*)m_pRecord)->iMode);
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfGDICOMMENT()
{
	// TRACE0("**EMR_GDICOMMENT\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		DWORD* pDW = (DWORD*)((EMRGDICOMMENT*)m_pRecord)->Data;
		if (GDICOMMENT_IDENTIFIER==*pDW)
		{
			pDW++;
			sExtra = SCGDICommentText(*pDW);
		} else
			sExtra = _T("Private");
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfFILLRGN()
{
	// TRACE0("**EMR_FILLRGN\n");
	if (m_bLogRecord)
	{
		EMRFILLRGN *pRec = (EMRFILLRGN*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("%s, ihBrush(%d), %s"),
			SCBoundsText(pRec->rclBounds),
			pRec->ihBrush,
			SCRegionDataText((RGNDATA*)pRec->RgnData, pRec->cbRgnData));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfFRAMERGN()
{
	// TRACE0("**EMR_FRAMERGN\n");
	if (m_bLogRecord)
	{
		EMRFRAMERGN *pRec = (EMRFRAMERGN*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("%s, Strokes(h:%d, v:%d), ihBrush(%d), %s"),
			SCBoundsText(pRec->rclBounds),
			SC_PARAM_SIZE(pRec->szlStroke),
			pRec->ihBrush,
			SCRegionDataText((RGNDATA*)pRec->RgnData, pRec->cbRgnData));
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPAINTRGN()
{
	// TRACE0("**EMR_PAINTRGN\n");
	if (m_bLogRecord)
	{
		EMRPAINTRGN *pRec = (EMRPAINTRGN*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("%s, %s"),
			SCBoundsText(pRec->rclBounds),
			SCRegionDataText((RGNDATA*)pRec->RgnData, pRec->cbRgnData));
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTSELECTCLIPRGN()
{
	// TRACE0("**EMR_EXTSELECTCLIPRGN\n");
	if (m_bLogRecord)
	{
		EMREXTSELECTCLIPRGN *pRec = (EMREXTSELECTCLIPRGN*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("%s, %s"),
			SCFormatRgnModeText(pRec->iMode),
			SCRegionDataText((RGNDATA*)pRec->RgnData, pRec->cbRgnData));
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfBITBLT()
{
	// TRACE0("**EMR_BITBLT\n");
	if (m_bLogRecord)
	{
		EMRBITBLT *pRec = (EMRBITBLT*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_BLTSRC,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_BLTSRC(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSTRETCHBLT()
{
	// TRACE0("**EMR_STRETCHBLT\n");
	if (m_bLogRecord)
	{
		EMRSTRETCHBLT *pRec = (EMRSTRETCHBLT*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_SBLTSRC,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_SBLTSRC(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfMASKBLT()
{
	// TRACE0("**EMR_MASKBLT\n");
	if (m_bLogRecord)
	{
		EMRMASKBLT *pRec = (EMRMASKBLT*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_BLTSRC _T(", ") SC_STRFMT_BMPMASK,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_BLTSRC(pRec),
			SC_PARAM_BMPMASK(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPLGBLT()
{
	// TRACE0("**EMR_PLGBLT\n");
	if (m_bLogRecord)
	{
		EMRPLGBLT *pRec = (EMRPLGBLT*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", Dest[") SC_STRFMT_TRIPOINT _T("], ")  SC_STRFMT_BLTSRC	_T(", ") SC_STRFMT_BMPSRC _T(", ") SC_STRFMT_BMPMASK,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_TRIPOINT(pRec->aptlDest),
			SC_PARAM_BLTSRC(pRec),
			SC_PARAM_BMPSRC(pRec),
			SC_PARAM_BMPMASK(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETDIBITSTODEVICE()
{
	// TRACE0("**EMR_SETDIBITSTODEVICE\n");
	if (m_bLogRecord)
	{
		EMRSETDIBITSTODEVICE *pRec = (EMRSETDIBITSTODEVICE*)m_pRecord;

		CString sBmiText = _T("");
		if (pRec->offBmiSrc)
			sBmiText = SCBmiText((BITMAPINFOHEADER *)((BYTE*)pRec + pRec->offBmiSrc));
		
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", Dest[x:%d, y:%d], Scans[start:%d, num:%d], ") SC_STRFMT_SETBITSSRC _T(" BmiHeader[%s]"),
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),

			pRec->xDest,
			pRec->yDest,

			pRec->iStartScan,
			pRec->cScans,

			SC_PARAM_SETBITSSRC(pRec),

			sBmiText
			);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSTRETCHDIBITS()
{
	// TRACE0("**EMR_STRETCHDIBITS\n");
	EMRSTRETCHDIBITS *pRec = (EMRSTRETCHDIBITS*)m_pRecord;
	if (m_bLogRecord)
	{
		CString sBmiText = _T("");
		if (pRec->offBmiSrc)
			sBmiText = SCBmiText((BITMAPINFOHEADER *)((BYTE*)pRec + pRec->offBmiSrc));

		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_SETBITSSRC _T(" BmiHeader[%s]"),
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_SETBITSSRC(pRec),

			sBmiText
			
			);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTCREATEFONTINDIRECTW()
{
	// TRACE0("**EMR_EXTCREATEFONTINDIRECTW\n");
	EMREXTCREATEFONTINDIRECTW *pRec = (EMREXTCREATEFONTINDIRECTW*)m_pRecord;
	if (m_bLogRecord)
	{
		CString strVendor;
		strVendor.Format(_T("0x%08X"), *(DWORD*)pRec->elfw.elfVendorId);

		CString strLogStyle;
		if (pRec->elfw.elfLogFont.lfWeight>=FW_BOLD)
			strLogStyle += _T("Bold | ");
		if (pRec->elfw.elfLogFont.lfItalic)
			strLogStyle += _T("Italic | ");
		if (pRec->elfw.elfLogFont.lfUnderline)
			strLogStyle += _T("Underlined | ");
		if (pRec->elfw.elfLogFont.lfStrikeOut)
			strLogStyle += _T("StrikeOut | ");
		if (!strLogStyle.IsEmpty())
			strLogStyle = strLogStyle.Left(strLogStyle.GetLength()-3);

		CString sExtra;
		// extra info about height and width
		sExtra.Format(_T("ihFont(%d) ELF[name(%ls) style(%ls) vendor(%s)] LOG[face(%ls), style(%s), charset(%d),family(%d),precision(%d), height(%d), width(%d)]"),
			pRec->ihFont,
			pRec->elfw.elfFullName,pRec->elfw.elfStyle, strVendor,
			pRec->elfw.elfLogFont.lfFaceName,
			strLogStyle,
			pRec->elfw.elfLogFont.lfCharSet,
			pRec->elfw.elfLogFont.lfPitchAndFamily,pRec->elfw.elfLogFont.lfOutPrecision,
			pRec->elfw.elfLogFont.lfHeight,
			pRec->elfw.elfLogFont.lfWidth);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTTEXTOUTA()
{
	// TRACE0("**EMR_EXTTEXTOUTA\n");
	if (m_bLogRecord)
	{
		CString sExtra = STFormatEmrTextA(
			((EMREXTTEXTOUTA*)m_pRecord)->emrtext,
			(EMREXTTEXTOUTA*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTTEXTOUTW()
{
	// TRACE0("**EMR_EXTTEXTOUTW\n");
	if (m_bLogRecord)
	{
		CString sExtra = STFormatEmrTextW((
			(EMREXTTEXTOUTW*)m_pRecord)->emrtext,
			(EMREXTTEXTOUTW*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYGON16()
{
	// TRACE0("**EMR_POLYGON16\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatEmrPolygonS((EMRPOLYGON16*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYPOLYGON16()
{
	// TRACE0("**EMR_POLYPOLYGON16\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatEmrPolyPolygonS((EMRPOLYPOLYGON16*)m_pRecord);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYDRAW16()
{
	// TRACE0("**EMR_POLYDRAW16\n");
	if (m_bLogRecord)
	{
		EMRPOLYDRAW16 *pRec = (EMRPOLYDRAW16*)m_pRecord;
		CString sExtra = SCPolygonTextS(pRec->rclBounds, pRec->apts, pRec->cpts);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATEMONOBRUSH()
{
	// TRACE0("**EMR_CREATEMONOBRUSH\n");
	if (m_bLogRecord)
	{
		EMRCREATEMONOBRUSH *pRec = (EMRCREATEMONOBRUSH*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihBrush(%d), ") SC_STRFMT_BMPDIB,
			pRec->ihBrush,
			SC_PARAM_BMPDIB(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATEDIBPATTERNBRUSHPT()
{
	// TRACE0("**EMR_CREATEDIBPATTERNBRUSHPT\n");
	if (m_bLogRecord)
	{
		EMRCREATEDIBPATTERNBRUSHPT *pRec = (EMRCREATEDIBPATTERNBRUSHPT*)m_pRecord;
		CString sExtra;
		sExtra.Format(_T("ihBrush(%d), ") SC_STRFMT_BMPDIB,
			pRec->ihBrush,
			SC_PARAM_BMPDIB(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfEXTCREATEPEN()
{
	// TRACE0("**EMR_EXTCREATEPEN\n");
	SCPlayRecord();
	if (m_bLogRecord)
	{
		EMREXTCREATEPEN *pRec = (EMREXTCREATEPEN*)m_pRecord;
		CString sExtra;

#ifdef SC_LOG_RAWDATA_ONLY
// display raw data
			sExtra.Format(_T(ihPen(%d), style(%d=%s), width(%d), color(0x%08X), brush[style:%d=%s, hatch:0x%08X, user_style_entries:%d], BMP[offBmi:%d, Bmi:%d, offBits:%d, Bits:%d]"),
				pRec->ihPen, 
				pRec->elp.elpPenStyle, SCPenStyleText(pRec->elp.elpPenStyle), 
				pRec->elp.elpWidth, 
				pRec->elp.elpColor, 
				pRec->elp.elpBrushStyle, SCBrushStyleText(pRec->elp.elpBrushStyle),
				pRec->elp.elpHatch, 
				pRec->elp.elpNumEntries,
				pRec->offBmi, pRec->cbBmi, pRec->offBits, pRec->cbBits);
#else
// display interpreted data
		// Pen style
		CString strPenStyle;
		strPenStyle.Format(_T("style(%d=%s"),
			pRec->elp.elpPenStyle, SCPenStyleText(pRec->elp.elpPenStyle));

		if (PS_USERSTYLE==pRec->elp.elpPenStyle)
		{// custom dashes
			CString strDashes;
			strDashes.Format(_T(", Dashes[entries:%d])"), pRec->elp.elpNumEntries);
			strPenStyle += strDashes;
		} else
			strPenStyle += _T(")");

		sExtra.Format(_T("ihPen(%d), %s, width(%d), %s"),
				pRec->ihPen, 
				strPenStyle, 
				pRec->elp.elpWidth, 
				SCExtLogpenBrushStyleText(pRec));
#endif
		
		return SCExtraLog(sExtra);
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPOLYTEXTOUTA()
{
	// TRACE0("**EMR_POLYTEXTOUTA\n");
	if (m_bLogRecord)
	{
		EMRPOLYTEXTOUTA *pRec = (EMRPOLYTEXTOUTA*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_TXTFORM _T(", nbStrings:%d"),
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_TXTFORM(pRec),
			pRec->cStrings);
		
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETICMMODE()
{
	// TRACE0("**EMR_SETICMMODE\n");
	if (m_bLogRecord)
	{
		CString sExtra = SCFormatICMModeText(((EMRSETICMMODE*)m_pRecord)->iMode);
		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATECOLORSPACE()
{
	// TRACE0("**EMR_CREATECOLORSPACE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihCS(%d), %s"),
			((EMRCREATECOLORSPACE*)m_pRecord)->ihCS,
			SCFormatLogColorSpaceAsTextA(((EMRCREATECOLORSPACE*)m_pRecord)->lcs));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCREATECOLORSPACEW()
{
	// TRACE0("**EMR_CREATECOLORSPACEW\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihCS(%d), %s"),
			((EMRCREATECOLORSPACEW*)m_pRecord)->ihCS,
			SCFormatLogColorSpaceAsTextW(((EMRCREATECOLORSPACEW*)m_pRecord)->lcs));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}


SC_BRKRESULT CSCEMF2Text::OnEmfSETCOLORSPACE()
{
	// TRACE0("**EMR_SETCOLORSPACE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihCS(%d)"),
			((EMRSELECTCOLORSPACE*)m_pRecord)->ihCS);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfGLSRECORD()
{
	// TRACE0("**EMR_GLSRECORD\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("cbData:%d, Data:0x%08x"),
			((EMRGLSRECORD*)m_pRecord)->cbData,
			((EMRGLSRECORD*)m_pRecord)->Data);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfGLSBOUNDEDRECORD()
{
	// TRACE0("**EMR_GLSBOUNDEDRECORD\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", cbData:%d, Data:0x%08x"),
			SC_PARAM_RCLBOUNDS(((EMRGLSBOUNDEDRECORD*)m_pRecord)->rclBounds),
			((EMRGLSBOUNDEDRECORD*)m_pRecord)->cbData,
			((EMRGLSBOUNDEDRECORD*)m_pRecord)->Data);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfPIXELFORMAT()
{
	// TRACE0("**EMR_PIXELFORMAT\n");
	EMRPIXELFORMAT *pRec = (EMRPIXELFORMAT*)m_pRecord;
	if (m_bLogRecord)
	{
		CString sExtra = SCPixelFormatText(((EMRPIXELFORMAT*)m_pRecord)->pfd);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfDRAWESCAPE()
{
	// TRACE0("**EMR_DRAWESCAPE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("iEscape(%d), cbEscData:%d, EscData:0x%8x"),
			((EMRDRAWESCAPE*)m_pRecord)->iEscape,
			((EMRDRAWESCAPE*)m_pRecord)->cbEscData,
			((EMRDRAWESCAPE*)m_pRecord)->EscData[1]);
		
		return SCExtraLogNoplay(sExtra);
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSMALLTEXTOUT()
{
	// TRACE0("**EMR_SMALLTEXTOUT\n");
	EMR *pRec = (EMR*)m_pRecord;
	if (m_bLogRecord)
	{
		SCEMRSMALLTEXTOUTA *pSmallText = (SCEMRSMALLTEXTOUTA*)m_pRecord;
		// TODO: review this line
		SCEMRSMALLTEXTOUTA2 *pTextRec2 = (SCEMRSMALLTEXTOUTA2*)m_pRecord;

		BOOL bUnicode = (pSmallText->fOptions == 0x100);
		CRect rcText;
		if (pSmallText->fOptions==0x204)
		{
			rcText.CopyRect((LPRECT)&pTextRec2->rclBoundsSC);
		} else
		{
			rcText.SetRectEmpty();
		}

		CString sExtra;
		CString strOpt = SCFormatTextOptions(pSmallText->fOptions);
			
		int nChars = pSmallText->nChars;
		TCHAR *pszText = new TCHAR[nChars+1];
		if (pszText)
		{
			if (bUnicode)
			{
				LPWSTR lpw = (LPWSTR)pTextRec2->Text;
#ifdef _UNICODE
				wcsncpy(pszText, lpw, nChars);
#else
				WideCharToMultiByte(CP_ACP, 0, lpw, nChars, pszText, (nChars+1)*sizeof(TCHAR), NULL, NULL);
#endif
			} else
			{
#ifdef _UNICODE
				MultiByteToWideChar(CP_ACP, 0, pSmallText->Text, nChars, pszText, nChars);
#else
				strncpy(pszText, pSmallText->Text, nChars);
#endif
			}
			pszText[nChars] = 0;
			sExtra.Format(_T(" TXT=[%s] ") SC_STRFMT_TXTFORM _T(" TxOPT[fOptions(%d=%s), nChars(%d), ptlRef(%d,%d)]"),
				pszText,
				pSmallText->exScale, pSmallText->eyScale, pSmallText->iGraphicsMode, SC_PARAM_RECT(rcText),
				pSmallText->fOptions, strOpt, nChars, SC_PARAM_POINT(pSmallText->ptlReference));

			delete [] pszText;
		} else
			sExtra.Format(_T(" TXT=[%s] ") SC_STRFMT_TXTFORM _T(" TxOPT[fOptions(%d=%s), nChars(%d), ptlRef(%d,%d)]"),
			_T("ERR"),
			pSmallText->exScale, pSmallText->eyScale, pSmallText->iGraphicsMode,
			pSmallText->fOptions, strOpt, nChars, SC_PARAM_POINT(pSmallText->ptlReference));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfNAMEDESCAPE()
{
	// TRACE0("**EMR_NAMEDESCAPE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("iEscape(%d), cbEscData:%d, EscData:0x%8x, cbDriver:%d"),
			((EMRNAMEDESCAPE*)m_pRecord)->iEscape,
			((EMRNAMEDESCAPE*)m_pRecord)->cbEscData,
			((EMRNAMEDESCAPE*)m_pRecord)->EscData[1],
			((EMRNAMEDESCAPE*)m_pRecord)->cbDriver);

		return SCExtraLogNoplay(sExtra);
	}
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfCOLORCORRECTPALETTE()
{
	// TRACE0("**EMR_COLORCORRECTPALETTE\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("ihPalette(%d) [nFirstEntry:%d, nPalEntries:%d]"),
			((EMRCOLORCORRECTPALETTE*)m_pRecord)->ihPalette,
			((EMRCOLORCORRECTPALETTE*)m_pRecord)->nFirstEntry,
			((EMRCOLORCORRECTPALETTE*)m_pRecord)->nPalEntries);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfSETICMPROFILEA()
{
	// TRACE0("**EMR_SETICMPROFILEA\n");
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("dwFlags:%d, cbName:%d, cbData:%d, Data:0x%8x"),
			((EMRSETICMPROFILEA*)m_pRecord)->dwFlags,
			((EMRSETICMPROFILEA*)m_pRecord)->cbName,
			((EMRSETICMPROFILEA*)m_pRecord)->cbData,
			((EMRSETICMPROFILEA*)m_pRecord)->Data);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfALPHABLEND()
{
	// TRACE0("**EMR_ALPHABLEND\n");
	if (m_bLogRecord)
	{
		EMRALPHABLEND *pRec = (EMRALPHABLEND*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_SBLTSRC,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_SBLTSRC(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfALPHADIBBLEND()
{
	// TRACE0("**EMR_SETLAYOUT\n");
	// Now it's SCEMF_SETLAYOUT
	if (m_bLogRecord)
	{
		CString sExtra;
		sExtra.Format(_T("iMode(%d=%s)"),
			((EMRSETLAYOUT*)m_pRecord)->iMode,
			SCLayoutModeText(((EMRSETLAYOUT*)m_pRecord)->iMode));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfTRANSPARENTBLT()
{
	// TRACE0("**EMR_TRANSPARENTBLT\n");
	if (m_bLogRecord)
	{
		EMRTRANSPARENTBLT *pRec = (EMRTRANSPARENTBLT*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T(", ") SC_STRFMT_DEST_CXCY _T(", ") SC_STRFMT_ROP _T(", ") SC_STRFMT_SBLTSRC,
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			SC_PARAM_DEST_CXCY(pRec),
			SC_PARAM_ROP(pRec->dwRop),
			SC_PARAM_SBLTSRC(pRec));

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnEmfGRADIENTFILL()
{
	// TRACE0("**EMR_GRADIENTFILL\n");
	if (m_bLogRecord)
	{
		EMRGRADIENTFILL *pRec = (EMRGRADIENTFILL*)m_pRecord;
		CString sExtra;
		sExtra.Format(SC_STRFMT_RCLBOUNDS _T("ulMode(%d=%s), nVer:%d, nTri:%d, 0x%8x"),
			SC_PARAM_RCLBOUNDS(pRec->rclBounds),
			pRec->ulMode, SCGradientFillModeText(pRec->ulMode),
			pRec->nVer,
			pRec->nTri,
			pRec->Ver);

		return SCExtraLog(sExtra);
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::OnBrkUNKRecord(long lMsg)
{
	// TRACE0("**UNKRecord\n");
	return SCBasicLog();
}

//////////////////////////////////////////////////////////////////////
// Utilities
//////////////////////////////////////////////////////////////////////

SC_BRKRESULT CSCEMF2Text::SCBasicLog()
{
	if (m_bLogRecord)
	{
		ASSERT(m_pIBrkCallBack);
		EMR *pRec = (EMR*)m_pRecord;

		CString strLog;
		strLog.Format(_T("R%04d: [%03d] %s\t(s=%d)\n"), m_nCurRec, pRec->iType, SCGetRecordName(pRec->iType), pRec->nSize);
		m_strLogBuf += strLog;
		if (0==(m_nCurRec % m_iCbThreshold))
		{
			if (0==m_pIBrkCallBack->OnSCBrkRecStr(m_nCurRec, (TCHAR*)LPCTSTR(m_strLogBuf)))
				return SC_BRK_STOPBREAKING;
			m_strLogBuf.Empty();
		}
	}
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::SCExtraLog(LPCTSTR lpszExtra)
{
	ASSERT(m_pIBrkCallBack);

	EMR *pRec = (EMR*)m_pRecord;
	
	CString strLog;
	strLog.Format(_T("R%04d: [%03d] %s\t(s=%d)\t{%s}\n"), m_nCurRec, pRec->iType, SCGetRecordName(pRec->iType), pRec->nSize, lpszExtra);
	m_strLogBuf += strLog;
	if (0==(m_nCurRec % m_iCbThreshold))
	{
		if (0==m_pIBrkCallBack->OnSCBrkRecStr(m_nCurRec, (TCHAR*)LPCTSTR(m_strLogBuf)))
			return SC_BRK_STOPBREAKING;
		m_strLogBuf.Empty();
	}
	
	SCPlayRecord();
	return SC_BRK_NOERROR;
}

SC_BRKRESULT CSCEMF2Text::SCExtraLogNoplay(LPCTSTR lpszExtra)
{
	ASSERT(m_pIBrkCallBack);

	EMR *pRec = (EMR*)m_pRecord;
	
	CString strLog;
	strLog.Format(_T("R%04d: [%03d] %s\t(s=%d)\t{%s}\n"), m_nCurRec, pRec->iType, SCGetRecordName(pRec->iType), pRec->nSize, lpszExtra);
	m_strLogBuf += strLog;
	if (0==(m_nCurRec % m_iCbThreshold))
	{
		if (0==m_pIBrkCallBack->OnSCBrkRecStr(m_nCurRec, (TCHAR*)LPCTSTR(m_strLogBuf)))
			return SC_BRK_STOPBREAKING;
		m_strLogBuf.Empty();
	}
	
	return SC_BRK_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Events dispatcher
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Starter methods
//////////////////////////////////////////////////////////////////////

SC_BRKRESULT CSCEMF2Text::SCMetafile2Text(HDC hdc,
										   HENHMETAFILE hemf,
										   LPVOID lpData,
										   CONST RECT *lpRect,
										   BOOL bLogRecord/*=FALSE*/)
{
	m_bLogRecord = ((m_pIBrkCallBack!=NULL) && bLogRecord);
	return SCBreakMetafile(hdc, hemf, lpData, lpRect);
}



//////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////
void CSCEMF2Text::SCEndBreak()
{
	if (m_bLogRecord)
	{
		ASSERT(m_pIBrkCallBack);
		if (!m_strLogBuf.IsEmpty())
		{
			ASSERT(m_nCurRec>0);
			m_pIBrkCallBack->OnSCBrkRecStr(m_nCurRec, (TCHAR*)LPCTSTR(m_strLogBuf));
			m_strLogBuf.Empty();
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Message map
//////////////////////////////////////////////////////////////////////
SC_BEGIN_MESSAGE_MAP(CSCEMF2Text, SCBrkTarget)
	ON_VECTRECORD(SCEMF_HEADER, OnEmfHEADER)
	ON_VECTRECORD(SCEMF_POLYBEZIER, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYGON, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYLINE, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYBEZIERTO, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYLINETO, OnEmfPOLYGON)
	ON_VECTRECORD(SCEMF_POLYPOLYLINE, OnEmfPOLYPOLYGON)
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
	ON_VECTRECORD(SCEMF_SETCOLORADJUSTMENT, SCBasicLog)
	ON_VECTRECORD(SCEMF_SETTEXTCOLOR, OnEmfSETTEXTCOLOR)
	ON_VECTRECORD(SCEMF_SETBKCOLOR, OnEmfSETBKCOLOR)
	ON_VECTRECORD(SCEMF_OFFSETCLIPRGN, OnEmfOFFSETCLIPRGN)
	ON_VECTRECORD(SCEMF_MOVETOEX, OnEmfMOVETOEX)
	ON_VECTRECORD(SCEMF_SETMETARGN, SCBasicLog)
	ON_VECTRECORD(SCEMF_EXCLUDECLIPRECT, OnEmfEXCLUDECLIPRECT)
	ON_VECTRECORD(SCEMF_INTERSECTCLIPRECT, OnEmfINTERSECTCLIPRECT)
	ON_VECTRECORD(SCEMF_SCALEVIEWPORTEXTEX, OnEmfSCALEVIEWPORTEXTEX)
	ON_VECTRECORD(SCEMF_SCALEWINDOWEXTEX, OnEmfSCALEWINDOWEXTEX)
	ON_VECTRECORD(SCEMF_SAVEDC, SCBasicLog)
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
	ON_VECTRECORD(SCEMF_CHORD, OnEmfARC)
	ON_VECTRECORD(SCEMF_PIE, OnEmfARC)
	ON_VECTRECORD(SCEMF_SELECTPALETTE, OnEmfSELECTPALETTE)
	ON_VECTRECORD(SCEMF_CREATEPALETTE, OnEmfCREATEPALETTE)
	ON_VECTRECORD(SCEMF_SETPALETTEENTRIES, OnEmfSETPALETTEENTRIES)
	ON_VECTRECORD(SCEMF_RESIZEPALETTE, OnEmfRESIZEPALETTE)
	ON_VECTRECORD(SCEMF_REALIZEPALETTE, SCBasicLog)
	ON_VECTRECORD(SCEMF_EXTFLOODFILL, OnEmfEXTFLOODFILL)
	ON_VECTRECORD(SCEMF_LINETO, OnEmfLINETO)
	ON_VECTRECORD(SCEMF_ARCTO, OnEmfARC)
	ON_VECTRECORD(SCEMF_POLYDRAW, OnEmfPOLYDRAW)
	ON_VECTRECORD(SCEMF_SETARCDIRECTION, OnEmfSETARCDIRECTION)
	ON_VECTRECORD(SCEMF_SETMITERLIMIT, OnEmfSETMITERLIMIT)
	ON_VECTRECORD(SCEMF_BEGINPATH, SCBasicLog)
	ON_VECTRECORD(SCEMF_ENDPATH, SCBasicLog)
	ON_VECTRECORD(SCEMF_CLOSEFIGURE, SCBasicLog)
	ON_VECTRECORD(SCEMF_FILLPATH, OnEmfFILLPATH)
	ON_VECTRECORD(SCEMF_STROKEANDFILLPATH, OnEmfFILLPATH)
	ON_VECTRECORD(SCEMF_STROKEPATH, OnEmfFILLPATH)
	ON_VECTRECORD(SCEMF_FLATTENPATH, SCBasicLog)
	ON_VECTRECORD(SCEMF_WIDENPATH, SCBasicLog)
	ON_VECTRECORD(SCEMF_SELECTCLIPPATH, OnEmfSELECTCLIPPATH)
	ON_VECTRECORD(SCEMF_ABORTPATH, SCBasicLog)
	ON_VECTRECORD(SCEMF_GDICOMMENT, OnEmfGDICOMMENT)
	ON_VECTRECORD(SCEMF_FILLRGN, OnEmfFILLRGN)
	ON_VECTRECORD(SCEMF_FRAMERGN, OnEmfFRAMERGN)
	ON_VECTRECORD(SCEMF_INVERTRGN, OnEmfPAINTRGN)
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
	ON_VECTRECORD(SCEMF_POLYBEZIER16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYGON16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYLINE16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYBEZIERTO16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYLINETO16, OnEmfPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYPOLYLINE16, OnEmfPOLYPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYPOLYGON16, OnEmfPOLYPOLYGON16)
	ON_VECTRECORD(SCEMF_POLYDRAW16, OnEmfPOLYDRAW16)
	ON_VECTRECORD(SCEMF_CREATEMONOBRUSH, OnEmfCREATEMONOBRUSH)
	ON_VECTRECORD(SCEMF_CREATEDIBPATTERNBRUSHPT, OnEmfCREATEDIBPATTERNBRUSHPT)
	ON_VECTRECORD(SCEMF_EXTCREATEPEN, OnEmfEXTCREATEPEN)
	ON_VECTRECORD(SCEMF_POLYTEXTOUTA, OnEmfPOLYTEXTOUTA)
	ON_VECTRECORD(SCEMF_POLYTEXTOUTW, OnEmfPOLYTEXTOUTA)
	ON_VECTRECORD(SCEMF_SETICMMODE, OnEmfSETICMMODE)
	ON_VECTRECORD(SCEMF_CREATECOLORSPACE, OnEmfCREATECOLORSPACE)
	ON_VECTRECORD(SCEMF_SETCOLORSPACE, OnEmfSETCOLORSPACE)
	ON_VECTRECORD(SCEMF_DELETECOLORSPACE, OnEmfSETCOLORSPACE)
	ON_VECTRECORD(SCEMF_GLSRECORD, OnEmfGLSRECORD)
	ON_VECTRECORD(SCEMF_GLSBOUNDEDRECORD, OnEmfGLSBOUNDEDRECORD)
	ON_VECTRECORD(SCEMF_PIXELFORMAT, OnEmfPIXELFORMAT)
	ON_VECTRECORD(SCEMF_DRAWESCAPE, OnEmfDRAWESCAPE)
	ON_VECTRECORD(SCEMF_EXTESCAPE, OnEmfDRAWESCAPE)
	ON_VECTRECORD(SCEMF_STARTDOC, SCBasicLog)
	ON_VECTRECORD(SCEMF_SMALLTEXTOUT, OnEmfSMALLTEXTOUT)
	ON_VECTRECORD(SCEMF_FORCEUFIMAPPING, SCBasicLog)
	ON_VECTRECORD(SCEMF_NAMEDESCAPE, OnEmfNAMEDESCAPE)
	ON_VECTRECORD(SCEMF_COLORCORRECTPALETTE, OnEmfCOLORCORRECTPALETTE)
	ON_VECTRECORD(SCEMF_SETICMPROFILEA, OnEmfSETICMPROFILEA)
	ON_VECTRECORD(SCEMF_SETICMPROFILEW, OnEmfSETICMPROFILEA)
	ON_VECTRECORD(SCEMF_ALPHABLEND, OnEmfALPHABLEND)
	ON_VECTRECORD(SCEMF_ALPHADIBBLEND, OnEmfALPHADIBBLEND)
	ON_VECTRECORD(SCEMF_TRANSPARENTBLT, OnEmfTRANSPARENTBLT)
	ON_VECTRECORD(SCEMF_TRANSPARENTDIB, SCBasicLog)
	ON_VECTRECORD(SCEMF_GRADIENTFILL, OnEmfGRADIENTFILL)
	ON_VECTRECORD(SCEMF_SETLINKEDUFIS, SCBasicLog)
	ON_VECTRECORD(SCEMF_SETTEXTJUSTIFICATION, SCBasicLog)
	ON_VECTRECORD(SCEMF_CREATECOLORSPACEW, OnEmfCREATECOLORSPACEW)
SC_END_MESSAGE_MAP()

