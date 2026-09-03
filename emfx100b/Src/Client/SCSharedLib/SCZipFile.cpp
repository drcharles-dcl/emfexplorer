/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCZipFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_FREAD_BUFLEN		(1024*32)

// Dup: we don't have them in a header
/* gzip flag byte */
#define SC_GZ_ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define SC_GZ_HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define SC_GZ_EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define SC_GZ_ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define SC_GZ_COMMENT      0x10 /* bit 4 set: file comment present */
#define SC_GZ_RESERVED     0xE0 /* bits 5..7: reserved */

///////////////////////////////////////////////////////////////////////
// Static
//
inline gzFile SCOpenGZFile(LPCTSTR lpszFName)
{
#ifdef _UNICODE
	char	szAnsiPath[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, lpszFName, MAX_PATH, szAnsiPath, MAX_PATH, NULL, NULL);
    gzFile	in = gzopen(szAnsiPath, "rb");
#else
    gzFile	in = gzopen(lpszFName, "rb");
#endif
	return in;
}


///
/// Would like to use an already opened handle to check if file is GZ
///
BOOL SCIsGZFile(HANDLE hFile)
{
	ASSERT(hFile);

	BOOL bYes = FALSE;

	long dwFilePosHigh=0;
	DWORD dwFilePos = SetFilePointer(hFile, 0L, &dwFilePosHigh, FILE_CURRENT);
	if (dwFilePos == 0xFFFFFFFF && (GetLastError() != NO_ERROR))
		return FALSE;
	SetFilePointer(hFile, 0L, NULL, FILE_BEGIN);

	BYTE gzheader[SC_GZHEADER_MINSIZE];
	DWORD dwRead;
	BOOL bResult = ReadFile(hFile, gzheader, SC_GZHEADER_MINSIZE, &dwRead, NULL);
	if (bResult && (SC_GZHEADER_MINSIZE==dwRead))
	{
		// Check the gzip magic header, and method
		if (gzheader[0] == 0x1f	&&
			gzheader[1] == 0x8b	&&
			gzheader[2] == Z_DEFLATED &&
			(gzheader[3] & SC_GZ_RESERVED) == 0)
		{
			// TODO: Now check the header
			bYes = TRUE;
		}
	}

	SetFilePointer(hFile, dwFilePos, &dwFilePosHigh, FILE_BEGIN);
	return bYes;
}

///
/// Check file is GZ
///
BOOL SCIsGZFile(LPCTSTR lpszFName)
{
	ASSERT(lpszFName);

	// Open the file for reading. 
	UINT uiMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    HANDLE hFile = CreateFile(lpszFName, GENERIC_READ, FILE_SHARE_READ, NULL,  
                        OPEN_EXISTING, 0, 0); 
    if (hFile == INVALID_HANDLE_VALUE)
	{
		SetErrorMode(uiMode);
		return FALSE;
	}

	BOOL bYes = SCIsGZFile(hFile);

	CloseHandle(hFile);
	SetErrorMode(uiMode);
	return bYes;
}

///
/// Check if buffer contains "would be GZ" data
///
BOOL SCIsGZCandidate(LPBYTE lpBuff, DWORD dwSize, BOOL bFullCheck/*=FALSE*/)
{
	ASSERT(lpBuff);

	if (dwSize>=SC_GZHEADER_MINSIZE)
	{
		BYTE flags;
		// Check the gzip magic header, and method
		if (lpBuff[0] == 0x1f	&&
			lpBuff[1] == 0x8b	&&
			lpBuff[2] == Z_DEFLATED &&
			((flags=lpBuff[3]) & SC_GZ_RESERVED) == 0)
		{
			if (!bFullCheck)
				return TRUE;

			/* Discard time, xflags and OS code */ 
			DWORD dwPos = SC_GZHEADER_MINSIZE; // skip 6 bytes
			
			if ((flags & SC_GZ_EXTRA_FIELD) != 0)
			{ /* skip the extra field */
				if (dwSize<dwPos+2) return FALSE;

				DWORD dwLen  =  (DWORD)lpBuff[dwPos++];
				dwLen += ((DWORD)lpBuff[dwPos++]) << 8;
				dwPos += dwLen;
				if (dwSize<dwPos) return FALSE;
			}
			if ((flags & SC_GZ_ORIG_NAME) != 0)
			{ /* skip the original file name */
				if (dwSize<dwPos+1) return FALSE;
				for (; dwPos < dwSize && lpBuff[dwPos] != 0; dwPos++)
				{
				}
			}
			if ((flags & SC_GZ_COMMENT) != 0)
			{   /* skip the .gz file comment */
				if (dwSize<dwPos+1) return FALSE;
				for (; dwPos < dwSize && lpBuff[dwPos] != 0; dwPos++)
				{
				}
			}
			if ((flags & SC_GZ_HEAD_CRC) != 0)
			{  /* skip the header crc */
				if (dwSize<dwPos+2) return FALSE;
			}

			return TRUE;
		}
	}
	return FALSE;
}


///
/// Uncompress and write to file
///
BOOL SC_GZUncompress(gzFile in, FILE   *out)
{
    char buf[SC_FREAD_BUFLEN];
    for (;;)
	{
        int len = gzread(in, buf, sizeof(buf));
        if (len < 0)
			 return FALSE;
		
        if (len == 0)
			break;

        if ((int)fwrite(buf, 1, (unsigned)len, out) != len)
            return FALSE;
    }
	return TRUE;
}

///
/// Inflate to the given file
///
BOOL SCUnzipGZFile(LPCTSTR lpszSrcFName, LPCTSTR lpszDestName)
{
	ASSERT(lpszSrcFName);
	ASSERT(lpszDestName);

	gzFile	in = SCOpenGZFile(lpszSrcFName);
	if (!in)
		return FALSE;

#ifdef _UNICODE
    FILE  *out = _wfopen(lpszDestName, L"wb");
#else
    FILE  *out = fopen(lpszDestName, "wb");
#endif
	BOOL bOk = FALSE;
    if (out)
	{
		bOk = SC_GZUncompress(in, out);
		int iRes = fclose(out);
		if (bOk)
			bOk = (0==iRes);
	}

	gzclose(in);
	return bOk;
}

///
/// Inflate directly in memory
///
BOOL SCUnzipGZFile(LPCTSTR lpszSrcFName, CMemFile& rMemFile)
{
	ASSERT(lpszSrcFName);
	
    gzFile	in = SCOpenGZFile(lpszSrcFName);
	if (!in)
		return FALSE;
	
	char buf[SC_FREAD_BUFLEN];
	BOOL bOk = FALSE;
	for (;;)
	{
        int len = gzread(in, buf, sizeof(buf));
        if (len < 0)
			break; // error
		
        if (0 == len)
		{
			bOk = TRUE;
			break;
		}
		
		rMemFile.Write(buf, len);
    }
	
	gzclose(in);
	return bOk;
}

///
/// Partial Inflate in memory
///
BOOL SCUnzipGZFilePart(LPCTSTR lpszSrcFName, LPBYTE pBuff, DWORD dwQuant)
{
	ASSERT(lpszSrcFName);
	ASSERT(pBuff && dwQuant);
	
    gzFile	in = SCOpenGZFile(lpszSrcFName);
	if (!in)
		return FALSE;
	
	BOOL bOk = FALSE;
	int len = gzread(in, (char*)pBuff, dwQuant);
	if (len>=0)
	{
		bOk = TRUE;
	}

	gzclose(in);
	return bOk;
}

///////////////////////////////////////////////////////////////////////
// class CSCMemZip
//

CSCMemZip::CSCMemZip():
	m_nRet(Z_STREAM_END),
	m_dwHeaderSize(0)
{
	memset(&m_zstrm, 0, sizeof(m_zstrm));
}

CSCMemZip::~CSCMemZip()
{
}

BOOL CSCMemZip::SCBeginUnzip(LPBYTE lpszSrcMem, long lSrcSize)
{
	memset(&m_zstrm, 0, sizeof(m_zstrm));
	m_zstrm.next_in = (unsigned char *)lpszSrcMem;
	m_zstrm.avail_in = lSrcSize;
	if ((m_nRet = inflateInit2(&m_zstrm, 15+32)) == Z_OK)
	{// skip header
		unsigned char buf[SC_FREAD_BUFLEN];
		m_zstrm.next_out = buf;
		m_zstrm.avail_out = SC_FREAD_BUFLEN;
		m_nRet = inflate(&m_zstrm, Z_BLOCK);

		if (Z_OK==m_nRet)
			m_dwHeaderSize = m_zstrm.total_in;
	}
	return (m_nRet == Z_OK);
}

BOOL CSCMemZip::SCEndUnzip()
{
	BOOL bOk = (inflateEnd(&m_zstrm) == Z_OK);
	if (m_nRet!=Z_STREAM_END) // SCEndUnzip called prematurely
		bOk = FALSE;
	return bOk;
}

BOOL CSCMemZip::SCUnzipChunk(LPBYTE lpszSrcMem, long lSrcSize)
{
    unsigned char buf[SC_FREAD_BUFLEN];
	long lRemain = lSrcSize;

	m_zstrm.next_in = (unsigned char *)lpszSrcMem;
	m_zstrm.avail_in = lSrcSize;
	
	do
	{
		long lOldRemain = m_zstrm.total_in;
		m_zstrm.next_out = buf;
		m_zstrm.avail_out = SC_FREAD_BUFLEN;

		m_nRet = inflate(&m_zstrm, Z_NO_FLUSH);

		TRACE3("m_nRet=%d total_in=%d m_zstrm.avail_out=%d\n", m_nRet, m_zstrm.total_in, m_zstrm.avail_out);
		long lDone = (m_zstrm.total_in - lOldRemain);
		if (lDone>0)
		{
			lRemain -= lDone;
			m_MemFile.Write(buf, SC_FREAD_BUFLEN - m_zstrm.avail_out);
		} else
			break; // error
	} while (m_nRet==Z_OK && lRemain>0);
	
	return (m_nRet==Z_OK || m_nRet==Z_STREAM_END);
}


