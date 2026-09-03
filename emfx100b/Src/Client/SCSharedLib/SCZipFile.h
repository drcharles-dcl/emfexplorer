/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCZIPFILE_H_
#define _SCZIPFILE_H_

#include "SCGenInclude.h"
#include SC_INC_ZIPLIB(zlib.h)

#define SC_GZHEADER_MINSIZE		10

BOOL SCIsGZFile(LPCTSTR lpszFName);
BOOL SCIsGZFile(HANDLE hFile);
BOOL SCIsGZCandidate(LPBYTE lpBuff, DWORD dwSize, BOOL bFullCheck=FALSE);
BOOL SCUnzipGZFile(LPCTSTR lpszSrcFName, LPCTSTR lpszDestName);

BOOL SCUnzipGZFile(LPCTSTR lpszSrcFName, CMemFile& rMemFile);
BOOL SCUnzipGZFilePart(LPCTSTR lpszSrcFName, LPBYTE pBuff, DWORD dwQuant);

class CSCMemZip
{
public:
	CSCMemZip();
	~CSCMemZip();

	// getters
	CMemFile& SCGetMemFile() { return m_MemFile; }
	int SCGetState() const { return m_nRet; }
	int SCGetTotalIn() const { return m_zstrm.total_in; }
	LPCSTR SCGetNextIn() const { return (LPCSTR)m_zstrm.next_in; }
	DWORD SCGetHeaderSize() const { return m_dwHeaderSize; }

	// operation
	BOOL SCBeginUnzip(LPBYTE lpszSrcMem, long lSrcSize);
	BOOL SCUnzipChunk(LPBYTE lpszSrcMem, long lSrcSize);
	BOOL SCEndUnzip();
	BOOL SCIsAtEnd() const { return (m_nRet==Z_STREAM_END); }


private:
	z_stream	m_zstrm;
	CMemFile	m_MemFile;
	int			m_nRet;
	DWORD		m_dwHeaderSize;
};

#endif //_SCZIPFILE_H_
//  ------------------------------------------------------------
