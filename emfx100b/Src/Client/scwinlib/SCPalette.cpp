/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCPalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Create a copy of the current system palette.
HPALETTE SCGetSystemPalette()
{
	HDC hDC;
	HPALETTE hPal;
	LPLOGPALETTE lpLogPal;
	
	// Get a DC for the desktop.
	hDC = GetDC(NULL);
	
	// Check to see if you are a running in a palette-based video mode.
	if (!(GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE))
	{
		ReleaseDC(NULL, hDC);
		return NULL;
	}
	
	// Allocate memory for the palette.
	lpLogPal = (LPLOGPALETTE)GlobalAlloc(GPTR, sizeof(LOGPALETTE) + 256 *
		sizeof(PALETTEENTRY));
	if (!lpLogPal)
	{
		TRACE(_T("SCGetSystemPalette.GlobalAlloc->NULL"));
		return NULL;
	}

	// Initialize.
	lpLogPal->palVersion = 0x300;
	lpLogPal->palNumEntries = 256;
	
	// Copy the current system palette into the logical palette.
	::GetSystemPaletteEntries(hDC, 0, 256,
		(LPPALETTEENTRY)(lpLogPal->palPalEntry));
	
	// Create the palette.
	hPal = CreatePalette(lpLogPal);
	
	// Clean up.
	GlobalFree(lpLogPal);
	ReleaseDC(NULL, hDC);
	
	return hPal;
}

// Create a copy of an enhanced metafile palette.
HPALETTE SCGetEMFPalette(HENHMETAFILE  hEmf)
{
	// Get the number of palette entries
	UINT uEntries = 0;
	uEntries = GetEnhMetaFilePaletteEntries(hEmf, 0, NULL);
	if (!uEntries)
		return NULL;
	
	// Create the logical palette based on the entries
	// Allocate memory for the palette.
	LPLOGPALETTE lpLogPal;
	lpLogPal = (LPLOGPALETTE)GlobalAlloc(GPTR, sizeof(LOGPALETTE) + uEntries *
		sizeof(PALETTEENTRY));
	if (!lpLogPal)
		return NULL;
	
	// Initialize.
	lpLogPal->palVersion = 0x300;
	lpLogPal->palNumEntries = uEntries;
	
	// Copy the EMF palette into the logical palette.
	if (GetEnhMetaFilePaletteEntries(hEmf, uEntries, (LPPALETTEENTRY)(lpLogPal->palPalEntry))==GDI_ERROR)
		return NULL;
	
	// Create the palette.
	HPALETTE hPal = CreatePalette(lpLogPal);
	
	// Clean up.
	GlobalFree(lpLogPal);
	
	return hPal;
}
