/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMF_H_
#define _SCEMF_H_

#include <vector>

#define WMFMETA_PLACEABLEKEY	0x9AC6CDD7

// Aldus placable metafile header
// Article ID: Q66949
#pragma pack(2)
typedef struct 
{
	DWORD		key;
	WORD		hmf;
	SMALL_RECT  bbox;
	WORD		inch;
	DWORD		reserved;
	WORD		checksum;
} APMFILEHEADER;
#pragma pack()

typedef std::vector<HENHMETAFILE, std::allocator<HENHMETAFILE> > HEMFVECTOR;

typedef BOOL (*fnSCPlayEnhMetaFile)(HDC hDC, HENHMETAFILE hemf, LPCRECT pRect);

BOOL SCGetEMFPlaySize(HENHMETAFILE hEMF, CSize& sizeEMF, HDC hDC=NULL, float* pfScale=NULL);
BOOL SCGetEMFElemsRect(HENHMETAFILE hEMF, CRect& rcElems, fnSCPlayEnhMetaFile fnPlayEMF=NULL);
void GetEMFDimension(HDC hDC, ENHMETAHEADER &emfh, int & width, int & height);
BOOL SCComputeEMFBlackBox(HENHMETAFILE hemf, CRect& ElemsRect, fnSCPlayEnhMetaFile fnPlayEMF=NULL);


BOOL SCGetEMFDPIs(HENHMETAFILE hEMF, long &lDPIX, long &lDPIY);
BOOL SCGetEMFInfos(HENHMETAFILE hEMF, long &lDPIX, long &lDPIY, long &lPaperCx, long &lPaperCy);
BOOL SCScaleImgSurface(HDC hDC, long lImgDPIX, long lImgDPIY, long &lImgCx, long &lImgCy);
HENHMETAFILE SCConvertWMFtoEMF(LPCTSTR lpszFname);
HENHMETAFILE SCConvertWMFtoEMF(DWORD dwSize, LPBYTE pBuffer);
HENHMETAFILE SCCloseEMF(HDC& hEMFDC);
void SCEMFDCDispose(HDC& hEMFDC);
void SCInitEMFDCForEnumeration(HDC hEMFDC, ENHMETAHEADER &hEmfHeader, HDC hDCRef);
BOOL SCPreFilterEMF(HENHMETAFILE& hEmf);
BOOL SCUpdateEMFheader(HENHMETAFILE& hemf, ENHMETAHEADER& rEmfHeader);

UINT SCConvertRTFtoEMF(LPCTSTR lpszFname, HEMFVECTOR& rVector, BOOL bRTF=TRUE);
UINT SCConvertRTFtoEMF(HANDLE hMem, HEMFVECTOR& rVector, BOOL bRTF=TRUE);
UINT SCConvertClipboardTextToEMF(HEMFVECTOR& rVector);

inline BOOL SCWriteEMFtoDisk(HENHMETAFILE hEmf, LPCTSTR szFileName)
{
	HENHMETAFILE hEmfCopy =	CopyEnhMetaFile(hEmf,(LPCTSTR)szFileName);
	if (hEmfCopy)
		DeleteEnhMetaFile(hEmfCopy);
	return (hEmfCopy!=NULL);
}

void SCConvertEMFtoWMF(HENHMETAFILE hEMF, LPCTSTR lpszFileName);

void SCRectDPtoHIMETRIC(CDC* pDC, RECT* pRect);
void SCRectLPtoHIMETRIC(CDC* pDC, RECT* pRect);
void SCRectToTwips(RECT* pRect, int ilogx, int ilogy, CDC* pDC);


#endif //_SCEMF_H_
//  ------------------------------------------------------------
