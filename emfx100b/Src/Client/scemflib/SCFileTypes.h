/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCFILETYPES_H_
#define _SCFILETYPES_H_

// states
#define SC_FSTATE_CLEARED		0x0000	// all checks successfully completed
#define SC_FSTATE_DELAYED		0x1000  // actual type to check on load
#define SC_FSTATE_NOTFOUND		0x2000	// file not found
#define SC_FSTATE_DOWNLOADING	0x3000	// file download in progress (in the future)
#define SC_FSTATE_ERROR			0x4000	// checks completed with error
#define SC_FSTATE_MASK			0xF000

// types
#define SC_FTYPE_UKN			0x0000	// unknown after check
#define SC_FTYPE_EMF			0x0001
#define SC_FTYPE_WMF			0x0002
#define SC_FTYPE_BGP			0x0003	// special file format (Bound Graphics Pages: group of files)
#define SC_FTYPE_IMG			0x0004
#define SC_FTYPE_TXT			0x0005
#define SC_FTYPE_EMZ			0x0006
#define SC_FTYPE_WMZ			0x0007

#define SC_FTYPE_TMF			0x000E	// special file format (to come)
#define SC_FTYPE_MASK			0x000F

// subtypes
#define SC_SUBTYPE_EMF_EMF		0x0000
#define SC_SUBTYPE_EMF_PLUS		0x0010
#define SC_SUBTYPE_EMF_DUAL		0x0020

#define SC_SUBTYPE_IMG_JPG		0x0100
#define SC_SUBTYPE_IMG_PNG		0x0110
#define SC_SUBTYPE_IMG_BMP		0x0120
#define SC_SUBTYPE_IMG_GIF		0x0130
#define SC_SUBTYPE_IMG_TIFF		0x0140
#define SC_SUBTYPE_IMG_EXIF		0x0150

#define SC_SUBTYPE_TXT_TXT		0x0200
#define SC_SUBTYPE_TXT_RTF		0x0210

#define SC_SUBTYPE_MASK			0x0FF0

// combinations
#define SC_FILETYPE_EMFONLY		(SC_FTYPE_EMF|SC_SUBTYPE_EMF_EMF)
#define SC_FILETYPE_EMFPLUS		(SC_FTYPE_EMF|SC_SUBTYPE_EMF_PLUS)
#define SC_FILETYPE_EMFDUAL		(SC_FTYPE_EMF|SC_SUBTYPE_EMF_DUAL)
#define SC_FILETYPE_WMF			SC_FTYPE_WMF

#define SC_FILETYPE_JPG			(SC_FTYPE_IMG|SC_SUBTYPE_IMG_JPG)
#define SC_FILETYPE_PNG			(SC_FTYPE_IMG|SC_SUBTYPE_IMG_PNG)
#define SC_FILETYPE_BMP			(SC_FTYPE_IMG|SC_SUBTYPE_IMG_BMP)
#define SC_FILETYPE_GIF			(SC_FTYPE_IMG|SC_SUBTYPE_IMG_GIF)
#define SC_FILETYPE_TIFF		(SC_FTYPE_IMG|SC_SUBTYPE_IMG_TIFF)
#define SC_FILETYPE_EXIF		(SC_FTYPE_IMG|SC_SUBTYPE_IMG_EXIF)

#define SC_FILETYPE_RTF			(SC_FTYPE_TXT|SC_SUBTYPE_TXT_RTF)
#define SC_FILETYPE_TXT			(SC_FTYPE_TXT|SC_SUBTYPE_TXT_TXT)

inline BOOL SCIsGDIpEMFType(int iType)
{
	return ((iType & SC_SUBTYPE_MASK)==SC_SUBTYPE_EMF_PLUS);
}

#endif //_SCFILETYPES_H_
//  ------------------------------------------------------------
