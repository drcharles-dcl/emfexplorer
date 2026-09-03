/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////////////
// Texts management
//
#ifdef _DEBUG
#define SC_SHOW_BBOX
#endif


///
///	Use the current font to draw the given text as GDI ExtTextout.
//  pRect (the clipping and/or opaquing rectangle) is optional,
/// but not pDxWidths (the intercharacter spacing array).
///
/// Note: Computing the rectangle needed by some GDI+ string functions is not a solution.
/// It would result in characters distributed uniformly in that rectangle. For example:
///    [A B C     D E]
/// Would result in something like:
///    [A  B  C  D  E]
///
void CSCEMFdcRenderer::SCDrawText(INT x, INT y, UINT uiOptions, LPCRECT pRect, LPCWSTR pwString,
								UINT uiCount, LPCINT pDxWidths, FLOAT fScaleX, FLOAT fScaleY)
{
	ASSERT(m_pGraphics);
	ASSERT(m_pFont);

	// DrawString isn't powerful enough.
	// It won't let us position the characters acurately, using pDxWidths.
	// And GDI+1.0 Path API is not coherent with Graphics API. We can't do:
	// m_pPath->AddString((WCHAR*)pwString, (INT)uiCount, m_pFont, PtOrg);
	// Good place to say: GDI+ does not work! again!
	// So we must do a lot of work.

	if (m_pPath)
	{
		SCAddTextInPath(x, y, uiOptions, pRect, pwString,
						uiCount, pDxWidths, fScaleX, fScaleY);
		return;
	}

	// String's reference point
	POINT PtRef;
	BOOL bUpdatingPos = (TA_UPDATECP == (m_dwTextAlign & (TA_NOUPDATECP|TA_UPDATECP)));
	if (bUpdatingPos)
	{
		// We must update/use current position.
		// But GDI+1.0 won't track the position for us
		PtRef.x = m_PtCurPos.X;
		PtRef.y = m_PtCurPos.Y;
	} else
	{
		// TA_NOUPDATECP: use the given point
		PtRef.x = x;
		PtRef.y = y;
	}

	INT flags = DriverStringOptionsCmapLookup;
	// "If the DriverStringOptionsCmapLookup flag is set,
	// each value specifies a Unicode character to be displayed.
	// Otherwise, each value specifies an index to a font glyph that defines a
	// character to be displayed."
	if (uiOptions & ETO_GLYPH_INDEX)
		flags = 0;

	// Save last char size and text's vertical extent
	RectF LastChSize;
	Matrix matrix;
	{
		SIZE size;
		BOOL bRes = GetTextExtentPoint32W(m_hDC, (pwString + uiCount - 1), 1, &size);
		ASSERT(bRes);
		LastChSize.Width = (REAL)size.cx;
		LastChSize.Height = (REAL)size.cy;

		//ASSERT(LastChSize.Width>0); // tricky:complex characters can have A+B+C=0
		ASSERT(LastChSize.Height>0);
	}

	// Compute text horizontal extent
	RectF TextSize;
	TextSize.Height = LastChSize.Height;
	TextSize.Width = (REAL)SCComputeTextWidth(pwString, uiCount, pDxWidths, flags);

	SCRectPolygon BoundingPoly;
	INT itmAscent = m_pFont->SCGetAscent();
	SCComputeTextBBox(m_dwTextAlign, TextSize, BoundingPoly, PtRef, itmAscent);

	// Compute placements for baseline glyph positioning
	// Glyph's vertical alignment
	// GDI+1.0 seems to use TA_BASELINE.
	INT iYPos = PtRef.y;
	switch (m_dwTextAlign & SC_VTXTALIGN_MASK)
	{
	case TA_BASELINE:
		// do nothing
		break;

	case TA_TOP:
		// We must shitf down.
		iYPos += itmAscent;
		break;
		
	case TA_BOTTOM:
		// We must shitf up.
		iYPos -= m_pFont->SCGetDescent();
		break;
	}

	// Glyph's horizontal alignment
	REAL fXPos = (REAL)PtRef.x;
	switch (m_dwTextAlign & SC_HTXTALIGN_MASK)
	{
	case TA_LEFT:
		break;
		
	case TA_CENTER:
		fXPos -= TextSize.Width / 2; 
		break;
		
	case TA_RIGHT:
		fXPos -= TextSize.Width;
		break;
	}

	// Compute characters' horizontal placements
	PointF* positions = new PointF[uiCount];
	for (UINT j = 0; (j < uiCount); j++)
	{
		positions[j].X = fXPos;
		positions[j].Y = (REAL)iYPos;
		fXPos += pDxWidths[j];
	}
	
	// Save clipping region
	Region ClipRgn;
	if (uiOptions & ETO_CLIPPED)
		m_pGraphics->GetClip(&ClipRgn);

	Matrix svdMatrix;
	BOOL bRestoreMat = FALSE;
	INT iFontAngle = m_pFont->SCGetAngle();
	if (iFontAngle)
	{// Perform manual font rotation, as GDI+1.0 ignores font escapement
		// (Note that they did not forget to rotate pen objects - yes, rotate pens !!??
		//  But they forgot font objects.)
		// Good place to say: GDI+ does not work!

		// save transform
		bRestoreMat = TRUE;
		m_pGraphics->GetTransform(&svdMatrix);

		// translate to the reference point
		m_pGraphics->TranslateTransform((REAL)PtRef.x, (REAL)PtRef.y);

		// rotate about the reference point
		// (GDI uses positive values for counterclockwise rotation.
		// Prepend negative transformation to obtain counterclockwise rotation)
		m_pGraphics->RotateTransform(-iFontAngle/10.0f);

		// restore original translation
		m_pGraphics->TranslateTransform(-(REAL)PtRef.x, -(REAL)PtRef.y);
	}

	// Special case: We don't know how to use the scaling factors here.
	// But if they contain a reflection, we will reflect the text.
	// Good place to say that our code does not work!
	// 
	// Note also that we are reflecting AFTER rotating (see comments in SCComputeTextBBox)
	// 
	if (fScaleY<0)
	{// Vertical reflection
		// We will reflect about a line passing by the reference point and
		// parallel to the base line

		// Save transform
		if (!bRestoreMat)
		{
			bRestoreMat = TRUE;
			m_pGraphics->GetTransform(&svdMatrix);
		}
		XFORM xform;
		switch (m_dwTextAlign & SC_VTXTALIGN_MASK)
		{
		case TA_BASELINE:
			// Reflect about the base line
			BoundingPoly.SCComputeReflectionBase(xform, itmAscent);
			break;
			
		case TA_TOP:
			// Reflect about the top border
			BoundingPoly.SCComputeReflectionTop(xform);
			break;
			
		case TA_BOTTOM:
			// Reflect about the bottom border
			BoundingPoly.SCComputeReflectionBottom(xform);
			break;
		}

		Matrix MatReflect(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
		m_pGraphics->MultiplyTransform(&MatReflect);
	}
	if (fScaleX<0)
	{// Horizontal reflection
		// Save transform
		// We will reflect about a line passing by the reference point and
		// orthogonal with the base line
		if (!bRestoreMat)
		{
			bRestoreMat = TRUE;
			m_pGraphics->GetTransform(&svdMatrix);
		}
								
		XFORM xform;
		switch (m_dwTextAlign & SC_HTXTALIGN_MASK)
		{
		case TA_LEFT:
			// Reflect about the left border
			BoundingPoly.SCComputeReflectionLeft(xform);
			break;
			
		case TA_CENTER:
			// TODO: Reflect about the center
			//
			break;
			
		case TA_RIGHT:
			// Reflect about the right border
			BoundingPoly.SCComputeReflectionRight(xform);
			break;
		}

		Matrix MatReflect(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
		m_pGraphics->MultiplyTransform(&MatReflect);
	}
	// also, this is an attempt to condense/inflate text independently of font and DC
	// (again, I have no documentation about this)
	float fxRatio = (float)(fabs(fScaleY)/fabs(fScaleX));
	if (fxRatio!=1)
	{
		if (!bRestoreMat)
		{
			bRestoreMat = TRUE;
			m_pGraphics->GetTransform(&svdMatrix);
		}
		m_pGraphics->TranslateTransform(PtRef.x*(1 - fxRatio), 0);
		m_pGraphics->ScaleTransform(fxRatio, 1);
	}

	// Clipping
	if (uiOptions & ETO_CLIPPED)
	{
		ASSERT(pRect);
		SCIntersectClipRect((RECTL&)*pRect); // TODO: consider setting instead of intersecting
	}

	// Paint background
	if ((uiOptions & ETO_OPAQUE) || OPAQUE==m_dwBkMode)
	{
		// Fill up the background under pRect
		ASSERT(pRect);
		SCRectPolygon ClipPoly;
		if (!IsRectEmpty(pRect))
			ClipPoly.SCFromRect(pRect);
		else
			ClipPoly = BoundingPoly;

		Color BkBrushColor;
		BkBrushColor.SetFromCOLORREF(m_BkColor);
		SolidBrush BkBrush(BkBrushColor);
		m_pGraphics->FillPolygon(&BkBrush, (const Point*)ClipPoly.m_Points, 4);
	}

	// text color
	Color TextBrushColor;
	TextBrushColor.SetFromCOLORREF(m_TextColor);
	SolidBrush TextBrush(TextBrushColor);
	//  -> fails with application-installed fonts selected in DC
	// (that's why we use private font collection)
    m_pGraphics->DrawDriverString(pwString, (INT)uiCount, m_pFont, &TextBrush,
     positions, flags, &matrix);

	// Attributes simulation
	DWORD dwFontSimul = m_pFont->SCGetSimulStatus();
	if (dwFontSimul & (SC_FONT_SIMUL_UNDERLINE|SC_FONT_SIMUL_OVERSTRIKE))
	{
		OUTLINETEXTMETRIC otm;
		GetOutlineTextMetrics(m_hDC, sizeof(otm), &otm);

		PointF PtA = positions[0];
		PointF PtB(PtA.X + TextSize.Width, 0);
		// underline
		if (dwFontSimul & SC_FONT_SIMUL_UNDERLINE)
		{
			Pen AttrPen(TextBrushColor, (REAL)otm.otmsUnderscoreSize); // TODO: review

			PtA.Y = PtA.Y - otm.otmsUnderscorePosition + otm.otmsUnderscoreSize/2; // TODO: review
			PtB.Y = PtA.Y;
			m_pGraphics->DrawLine(&AttrPen, PtA, PtB);
		}
		
		// overstrike
		if (dwFontSimul & SC_FONT_SIMUL_OVERSTRIKE)
		{
			Pen AttrPen(TextBrushColor, (REAL)otm.otmsStrikeoutSize);

			PtA.Y = positions[0].Y - otm.otmsStrikeoutPosition + otm.otmsStrikeoutSize/2;
			PtB.Y = PtA.Y;
			m_pGraphics->DrawLine(&AttrPen, PtA, PtB);
		}
	}

#ifdef SC_SHOW_BBOX
	if (0)
	{// Debug stuff
		Color PenColor;
		PenColor.SetFromCOLORREF(BoundingPoly.m_ColorTopAndRight);
		Pen TRPen(PenColor, 1.0);
		m_pGraphics->DrawLines(&TRPen, (const Point*)&BoundingPoly.m_Points, 3);

		PenColor.SetFromCOLORREF(BoundingPoly.m_ColorBottomAndLeft);
		Pen BLPen(PenColor, 1.0);
		m_pGraphics->DrawLines(&BLPen, (const Point*)&BoundingPoly.m_Points[SCRectPolygon::cnr_RB], 2);
		m_pGraphics->DrawLine(&BLPen, BoundingPoly.m_Points[SCRectPolygon::cnr_LB].x, BoundingPoly.m_Points[SCRectPolygon::cnr_LB].y,
			BoundingPoly.m_Points[SCRectPolygon::cnr_LT].x, BoundingPoly.m_Points[SCRectPolygon::cnr_LT].y);
	}
#endif

	// Update current position
	if (bUpdatingPos)
	{
		m_PtCurPos.X = (INT)positions[uiCount-1].X;
		m_PtCurPos.Y = (INT)positions[uiCount-1].Y;
		switch (m_dwTextAlign & (TA_TOP|TA_BASELINE|TA_BOTTOM))
		{
		case TA_BASELINE:
			// do nothing
			break;

		case TA_TOP:
			// We must shift up from base line to top.
			m_PtCurPos.Y -= itmAscent;
			break;
			
		case TA_BOTTOM:
			// We must shift down from base line to bottom.
			m_PtCurPos.Y += m_pFont->SCGetDescent();
			break;
		}

		m_PtCurPos.X += (INT)LastChSize.Width;
	}

	delete [] positions;
	// restore transformation
	if (bRestoreMat)
		m_pGraphics->SetTransform(&svdMatrix);
	if (uiOptions & ETO_CLIPPED)
		m_pGraphics->SetClip(&ClipRgn);
}

///
///	Add a text to the current path.
///
void CSCEMFdcRenderer::SCAddTextInPath(INT x, INT y, UINT uiOptions, LPCRECT pRect, LPCWSTR pwString,
								UINT uiCount, LPCINT pDxWidths, FLOAT fScaleX, FLOAT fScaleY)
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);
	ASSERT(m_pFont);

	// String's reference point
	POINT PtRef;
	BOOL bUpdatingPos = (TA_UPDATECP == (m_dwTextAlign & (TA_NOUPDATECP|TA_UPDATECP)));
	if (bUpdatingPos)
	{
		// We must update/use current position.
		PtRef.x = m_PtCurPos.X;
		PtRef.y = m_PtCurPos.Y;
	} else
	{
		// TA_NOUPDATECP: use the passed-on point
		PtRef.x = x;
		PtRef.y = y;
	}
	INT flags = DriverStringOptionsCmapLookup;
	if (uiOptions & ETO_GLYPH_INDEX)
	{
		flags = 0;
		// TODO: AddString is unaware of ETO_GLYPH_INDEX
	}

	// Save last char size and text's vertical extent
	RectF LastChSize;
	Matrix matrix;
	{
		PointF PtDummy(0, 0); // cause we don't know if the corresponding parameter can be NULL
		Status Res = m_pGraphics->MeasureDriverString((pwString + uiCount - 1), 1, m_pFont,
			&PtDummy, flags, &matrix, &LastChSize);
	}

	// Compute text horizontal extent
	RectF TextSize;
	TextSize.Height = LastChSize.Height;
	TextSize.Width = (REAL)SCComputeTextWidth(pwString, uiCount, pDxWidths, flags);

	SCRectPolygon BoundingPoly;
	INT itmAscent = m_pFont->SCGetAscent();
	SCComputeTextBBox(m_dwTextAlign, TextSize, BoundingPoly, PtRef, itmAscent);

	// Compute placements for (left,top) glyph positioning
	// Glyph's vertical alignment (we can't use StringFormat to do it)
	INT iYPos = PtRef.y;
	switch (m_dwTextAlign & SC_VTXTALIGN_MASK)
	{
	case TA_BASELINE:
		iYPos -= itmAscent; // AddString uses (left,top) alignment
		break;

	case TA_TOP:
		// Do nothing.
		break;
		
	case TA_BOTTOM:
		// We must shitf up.
		iYPos -= m_pFont->SCGetDescent() + itmAscent;
		break;
	}

	// Glyph's horizontal alignment
	REAL fXPos = (REAL)PtRef.x;
	switch (m_dwTextAlign & SC_HTXTALIGN_MASK)
	{
	case TA_LEFT:
		break;
		
	case TA_CENTER:
		fXPos -= TextSize.Width / 2; 
		break;
		
	case TA_RIGHT:
		fXPos -= TextSize.Width;
		break;
	}
#if 1
	// TODO: explain why the text is shifted by (tm.tmInternalLeading + tm.tmExternalLeading)
	fXPos -= m_pFont->SCGetInterExternal();
	// I know, this is really unacceptable code! Sorry for the inconvenience.
#endif

	// Compute characters' horizontal placements
	PointF* positions = new PointF[uiCount];
	for (UINT j = 0; (j < uiCount); j++)
	{
		positions[j].X = fXPos;
		positions[j].Y = (REAL)iYPos;
		fXPos += pDxWidths[j];
	}

	GraphicsPath* pPath = (GraphicsPath*)m_pPath;
	// if local transformations are required, we will use a sub path to hold them
	float fxRatio = (float)(fabs(fScaleY)/fabs(fScaleX));
	INT iFontAngle = m_pFont->SCGetAngle();
	BOOL bSubPath = ((iFontAngle!=0) || (fScaleX<0) || (fScaleY<0) || (fxRatio!=1));
	if (bSubPath)
	{
		pPath = new GraphicsPath;
	}

	// Can't use AddString if we don't have FontFamily, emSize, etc... Are you kidding?
	// Can't do: pPath->AddString((WCHAR*)pwString, (INT)uiCount, m_pFont, ...);
	FontFamily family;
	m_pFont->GetFamily(&family);
	REAL emSize = m_pFont->GetSize();
	INT style = m_pFont->GetStyle();
	DWORD dwFontSimul = m_pFont->SCGetSimulStatus();
	if (dwFontSimul & SC_FONT_SIMUL_UNDERLINE)
		style |= FontStyleUnderline;
	if (dwFontSimul & SC_FONT_SIMUL_OVERSTRIKE)
		style |= FontStyleStrikeout;

	// GDI+1.0 can't position characters in path. 
	// Good place to say: GDI+ does not work!
	// Try doing it one character at a time (but the ETO_GLYPH_INDEX problem still remains)
	for (UINT i=0; (i<uiCount); i++)
	{
		pPath->AddString(pwString+i, 1, &family,
			style, emSize, positions[i], NULL);
	}

#ifdef SC_SHOW_BBOX
	{// Debug stuff
		pPath->StartFigure();
		pPath->AddLines((const Point*)&BoundingPoly.m_Points, 4);
		pPath->CloseFigure();
	}
#endif

	// Transformations (Documentation in SCDrawText)
	if (bSubPath)
	{
		Matrix pathMat;
		// rotate
		if (iFontAngle)
		{
			pathMat.Translate((REAL)PtRef.x, (REAL)PtRef.y);
			pathMat.Rotate(-iFontAngle/10.0f);
			pathMat.Translate(-(REAL)PtRef.x, -(REAL)PtRef.y);
		}

		// reflect
		if (fScaleY<0)
		{// Vertical reflection
			XFORM xform;
			switch (m_dwTextAlign & SC_VTXTALIGN_MASK)
			{
			case TA_BASELINE:
				BoundingPoly.SCComputeReflectionBase(xform, itmAscent);
				break;
				
			case TA_TOP:
				BoundingPoly.SCComputeReflectionTop(xform);
				break;
				
			case TA_BOTTOM:
				BoundingPoly.SCComputeReflectionBottom(xform);
				break;
			}
			
			Matrix MatReflect(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
			pathMat.Multiply(&MatReflect);
		}
		if (fScaleX<0)
		{// Horizontal reflection
			XFORM xform;
			switch (m_dwTextAlign & SC_HTXTALIGN_MASK)
			{
			case TA_LEFT:
				BoundingPoly.SCComputeReflectionLeft(xform);
				break;
				
			case TA_CENTER:
				// TODO: Reflect about the center
				//
				break;
				
			case TA_RIGHT:
				BoundingPoly.SCComputeReflectionRight(xform);
				break;
			}
			
			Matrix MatReflect(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
			pathMat.Multiply(&MatReflect);
		}
		// scale
		if (fxRatio!=1)
		{
			pathMat.Translate(PtRef.x*(1 - fxRatio), 0);
			pathMat.Scale(fxRatio, 1);
		}
		// clip
		// TODO: if ETO_CLIPPED is set, we should clip the path.
		// pPath->Intersect(ClipRect);

		// transform and add
		pPath->Transform(&pathMat);
		m_pPath->AddPath(pPath, FALSE);
	}

	// Update current position? (Don't know what GDI would do)
	if (bUpdatingPos)
	{
		m_PtCurPos.X = (INT)positions[uiCount-1].X;
		m_PtCurPos.Y = (INT)positions[uiCount-1].Y;
		switch (m_dwTextAlign & (TA_TOP|TA_BASELINE|TA_BOTTOM))
		{
		case TA_BASELINE:
			// do nothing
			break;

		case TA_TOP:
			// We must shift up from base line to top.
			m_PtCurPos.Y -= itmAscent;
			break;
			
		case TA_BOTTOM:
			// We must shift down from base line to bottom.
			m_PtCurPos.Y += m_pFont->SCGetDescent();
			break;
		}

		m_PtCurPos.X += (INT)LastChSize.Width;
	}

	delete [] positions;
}

///
/// Build a non-rotated text box from a reference point text extents
/// This rectangle is ready to use for a (TA_TOP,TA_LEFT) aligned text.
/// 
void CSCEMFdcRenderer::SCBuildTextRect(ULONG ulTextAlign, POINT PtRef, RECT& ClipRect,
					RectF TextSize, INT itmAscent)
{
	// Text's horizontal alignment
	switch (ulTextAlign & SC_HTXTALIGN_MASK)
	{
	case TA_LEFT:
		ClipRect.left = PtRef.x;
		break;
		
	case TA_CENTER:
		ClipRect.left = (long)(PtRef.x - TextSize.Width / 2); 
		break;
		
	case TA_RIGHT:
		ClipRect.left = (long)(PtRef.x - TextSize.Width);
		break;
	}
	
	// Text's vertical alignment
	switch (ulTextAlign & SC_VTXTALIGN_MASK)
	{
	case TA_TOP:
		ClipRect.top = PtRef.y;
		break;
		
	case TA_BASELINE:
		ClipRect.top = PtRef.y - itmAscent;
		break;
		
	case TA_BOTTOM:
		ClipRect.top = (long)(PtRef.y - TextSize.Height);
		break;
	}		
	ClipRect.right = (long)(ClipRect.left + TextSize.Width);
	ClipRect.bottom = (long)(ClipRect.top + TextSize.Height);
}

///
/// Compute text witdh
///
INT CSCEMFdcRenderer::SCComputeTextWidth(LPCWSTR pwString, INT iCount, CONST INT* pDxWidths, INT flags)
{
	// Compute text's horizontal extent
	INT iWdth = 0;
	CONST INT* pWdt = pDxWidths;
	for (INT i = 0; (i < iCount); i++)
		iWdth += *pWdt++;

	ASSERT(iWdth>0);
	return iWdth;
}

///
/// Construct the parallelogram surrounding a text.
/// 
void CSCEMFdcRenderer::SCComputeTextBBox(ULONG ulTextAlign, RectF TextSize,
					   SCRectPolygon& RectPolygon, POINT& PtRef, INT itmAscent)
{
	ASSERT(TextSize.Width>0 && TextSize.Height>0);

	// make a rectangle for ta_left/ta_top aligned text
	RECT rcText;
	SCBuildTextRect(ulTextAlign, PtRef, rcText, TextSize, itmAscent);
	
	// transform into polygon
	RectPolygon.SCFromRect(&rcText);

#if 0
	// rotate if necessary
	ASSERT(m_pFont);
	INT iFontAngle = m_pFont->SCGetAngle();
	if (iFontAngle)
	{
		// Caution: use this only if reflection is done BEFORE rotation
		RectPolygon.SCRotate(iFontAngle, PtRef.x, PtRef.y);
		// But if the font angle is already taken into account in another transformation,
		// do not rotate the text box.
	}
#endif
}

	
