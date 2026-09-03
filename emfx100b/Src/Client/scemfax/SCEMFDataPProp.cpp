/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCEMFDataPProp.h"
#include "SCEMFAxCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSCEMFDataPathProperty, CDataPathProperty)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDataPathProperty

CSCEMFDataPathProperty::CSCEMFDataPathProperty(COleControl* pControl) :
	CDataPathProperty(pControl),
	m_dlState(dlNone),
	m_hEMF(NULL),
	m_dwArrivedBytes(0),
	m_dwRead(0),
	m_dwNeeded(0),
	m_dwTotal(0),
	m_dwPos(0),
	m_dwNbRecs(0),
	m_bCheckZip(TRUE),
	m_bIsGZ(FALSE)
{
}

CSCEMFDataPathProperty::~CSCEMFDataPathProperty()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSCEMFDataPathProperty, CDataPathProperty)
	//{{AFX_MSG_MAP(CSCEMFDataPathProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif  // 0

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDataPathProperty member functions

void CSCEMFDataPathProperty::OnDataAvailable(DWORD dwSize, DWORD bscfFlag)
{
	if (bscfFlag & BSCF_FIRSTDATANOTIFICATION)
		ResetData();

	CDataPathProperty::OnDataAvailable(dwSize, bscfFlag);

	if (dwSize <= m_dwArrivedBytes)
		return;

	DWORD dwArriving = dwSize - m_dwArrivedBytes;
	m_dwArrivedBytes = dwSize;

	BOOL bContinue = m_dlState != dlDone;
	while (bContinue)
	{
		switch (m_dlState)
		{
		case dlNone:
			// Try to read the EMF header.
			{
				if (m_bCheckZip &&  dwArriving>=SC_GZHEADER_MINSIZE)
				{
					m_bCheckZip = FALSE;

					BYTE buff[128];
					DWORD dwPos = this->GetPosition();
					DWORD dwRead = this->Read(buff, 128);

					m_bIsGZ = SCIsGZCandidate(buff, dwRead);
					if (m_bIsGZ)
					{
						m_MemZip.SCBeginUnzip(buff, dwRead);

						DWORD dwHdrSize = m_MemZip.SCGetHeaderSize();

						if (m_MemZip.SCGetState())
							this->Seek((LONG)dwPos, CFile::begin);
						else
						{// prepare for EMF header check
							dwArriving -= dwHdrSize;
							this->Seek((LONG)dwPos+dwHdrSize, CFile::begin);
						}
							
					} else
						this->Seek((LONG)dwPos, CFile::begin);
				}

				ENHMETAHEADER emfh;
				if (SCReadBytes(dwArriving, (LPBYTE)&emfh, sizeof(emfh))&&
					(emfh.dSignature == ENHMETA_SIGNATURE))
				{
					m_dwTotal = emfh.nBytes;
					m_dwPos = 0;
					m_dwNeeded = emfh.nSize;

					m_dlState = dlEMFBody;

					// resume reading
					dwArriving = m_dwArrivedBytes;
					if (!m_bIsGZ)
					{// Restart everything from the begining to get EMF header
						this->SeekToBegin();
						m_CacheFile.SetLength(0);
					}
					else
					{
						if (m_MemZip.SCIsAtEnd())
						{// The cache contains a complete EMF: keep it.
							dwArriving = 0;
						} else
						{// Restart everything from the begining (except zip header)
							DWORD dwHdrSize = m_MemZip.SCGetHeaderSize();
							dwArriving -= dwHdrSize;

							BYTE buff[128];
							this->SeekToBegin();
							DWORD dwRead = this->Read(buff, 128);
							m_MemZip.SCBeginUnzip(buff, dwRead);

							this->Seek(dwHdrSize, CFile::begin);
							m_CacheFile.SetLength(0);
						}
					}
					m_dwRead = m_CacheFile.GetLength(); // for control
				}
				else
				{
					bContinue = FALSE;
					break;
				}
			}
			break;

		case dlEMFBody:
			// Read some records.
			{
				bContinue = FALSE; // always quit

				LPBYTE pb = NULL;
				int nSize = SCReadRecords(dwArriving, pb);
				if (nSize > 0)
				{
					if (m_hEMF)
						DeleteEnhMetaFile(m_hEMF);
					m_hEMF = SetEnhMetaFileBits(nSize, pb);
					delete [] pb;
				}
				if (m_dwRead>=m_dwTotal)
				{
					m_dlState = dlDone;
					m_CacheFile.SetLength(0);
				}
			}
			break;
		}; //switch

		bContinue = bContinue && (m_dlState != dlDone);
	} //while

	ASSERT_KINDOF(CSCEMFAxCtrl, GetControl());
	((CSCEMFAxCtrl*)GetControl())->InformDlStatus(m_dlState);
}

void CSCEMFDataPathProperty::ResetData()
{
	CDataPathProperty::ResetData();
	m_dlState = dlNone;
	if (m_hEMF)
	{
		DeleteEnhMetaFile(m_hEMF);
		m_hEMF = NULL;
	}

	m_CacheFile.SetLength(0);
	m_dwArrivedBytes = 0;
	m_dwRead = 0;
	m_dwNeeded = 0;
	m_dwTotal = 0;
	m_dwPos = 0;
	m_dwNbRecs = 0;
}

DWORD CSCEMFDataPathProperty::SCTransferFileContent(CFile* pFrom, CFile* pTo, DWORD dwAmount)
{
	#define SC_BUFSIZE	1024*32
	DWORD dwRead = 0;

	if (m_bIsGZ && m_MemZip.SCIsAtEnd())
	{
#ifdef _DEBUG
		// You must manage to avoid calling this function in this case
		AfxMessageBox(_T("Attempt to uncompress past the end of incoming stream."));
		// And the data should be in the cache
		if (m_CacheFile.GetLength()<=m_dwArrivedBytes)
		{
			TRACE(_T("Invalid data in cache."));
		}
#endif
		return 0;
	}

	pTo->SeekToEnd();
	if (dwAmount>SC_BUFSIZE)
	{// Big chunk coming: attempt to use it in one call.
	 // Note however that we should put some limit.
		BYTE* pBuff = new BYTE[dwAmount]; // let Windows VMM do its job
		if (pBuff)
		{
			DWORD dwActual = pFrom->Read(pBuff, dwAmount);
			if (m_bIsGZ)
			{
				m_MemZip.SCUnzipChunk(pBuff, dwActual);

				CMemFile& rMemFile = m_MemZip.SCGetMemFile();
				dwActual = rMemFile.GetLength();
				delete [] pBuff;
				pBuff = rMemFile.Detach();
				if (m_MemZip.SCIsAtEnd())
					m_MemZip.SCEndUnzip();
			}

			pTo->Write(pBuff, dwActual);
			dwRead += dwActual;
			delete [] pBuff;
			return dwRead;
		}
		// fall through
	}

	// small one, or could not allocate big one
	BYTE buff[SC_BUFSIZE];
	DWORD dwActual = 1; // bogus value to enter the loop
	while ((dwAmount > 0) && (dwActual > 0))
	{
		dwActual = pFrom->Read(buff, min(dwAmount, SC_BUFSIZE));
		if (m_bIsGZ)
		{
			m_MemZip.SCUnzipChunk(buff, dwActual);
			
			CMemFile& rMemFile = m_MemZip.SCGetMemFile();
			dwActual = rMemFile.GetLength();
			LPBYTE pBuff = rMemFile.Detach();
			pTo->Write(pBuff, dwActual);
			delete [] pBuff;
			if (m_MemZip.SCIsAtEnd())
				m_MemZip.SCEndUnzip();
		} else
			pTo->Write(buff, dwActual);				
		dwAmount -= dwActual;
		dwRead += dwActual;
	}

	return dwRead;
}

BOOL CSCEMFDataPathProperty::SCReadBytes(DWORD dwArriving, LPBYTE pb, DWORD dwLen)
{
	// Cache contents.
	SCTransferFileContent(this, &m_CacheFile, dwArriving);
	m_dwRead = m_CacheFile.GetLength();

	// Do we have enough data yet?
	if (m_dwRead >= dwLen)
	{
		m_CacheFile.SeekToBegin();
		VERIFY(m_CacheFile.Read(pb, dwLen) == (UINT)dwLen);

		return TRUE;
	}
	return FALSE;
}

DWORD CSCEMFDataPathProperty::SCReadRecords(DWORD dwArriving, LPBYTE& pb)
{
	// Cache contents.
	if (dwArriving)
		SCTransferFileContent(this, &m_CacheFile, dwArriving);
#ifdef _DEBUG
	if (dwArriving &&
		m_dwRead >= m_CacheFile.GetLength())
	{// we missed something
		CString strmsg;
		strmsg.Format(_T("Error.Cache:dwArriving=%d, m_dwRead=%d, Cache.Length=%d"), dwArriving, m_dwRead, m_CacheFile.GetLength());
		AfxMessageBox(strmsg);
	}
#endif

	m_dwRead = m_CacheFile.GetLength();

	// Do we have enough data yet?
	DWORD dwFresh = m_dwRead - m_dwPos;
	if (dwFresh<m_dwNeeded)
	{ // eat fresh bytes, but we want more
#ifdef _DEBUG
		if (m_dwRead >= m_dwTotal)
		{// we are asking too much 
			CString strmsg;
			strmsg.Format(_T("Error.Waiting:dwFresh=%d, m_dwNeeded=%d, m_dwRead=%d"), dwFresh, m_dwNeeded, m_dwRead);
			AfxMessageBox(strmsg);
		}
#endif
		m_dwNeeded -= dwFresh;
		return 0;
	}

	m_CacheFile.SeekToBegin();
	
	pb = new BYTE[m_dwRead + sizeof(EMREOF)];
	UINT uiRead = m_CacheFile.Read(pb, m_dwRead);
	ASSERT(uiRead == (UINT)m_dwRead);
	
	// parse records in the block
	DWORD dwWholeRecs = 0; // size of integral records arrived
	PEMR pEmr = (PEMR)((BYTE*)pb + m_dwPos);
	
	while (m_dwPos < m_dwRead)
	{
		m_dwNeeded = pEmr->nSize;

		if (!m_dwNeeded)
		{// data error
			TRACE1("BAD RECORD AT m_dwNbRecs=%d\n", m_dwNbRecs);
			break;
		}

		if (m_dwPos + m_dwNeeded<=m_dwRead)
		{// integral record
			m_dwNbRecs++;
			// next record in same block
			m_dwPos += m_dwNeeded;
			pEmr = (PEMR)((BYTE*)pb + m_dwPos);
			m_dwNeeded = 0;
		} else
		{
			// next record in a next block
			m_dwNeeded -= (m_dwRead - m_dwPos);
			break;
		}
	}
	if (!m_dwPos)
	{// error
		delete [] pb;
		return 0;
	}
	dwWholeRecs = m_dwPos;
	
	// adjust buffer
	ENHMETAHEADER* pemfh = (ENHMETAHEADER*)pb;
	pemfh->nRecords = m_dwNbRecs;
	pemfh->nBytes = dwWholeRecs;
	if (dwWholeRecs<m_dwTotal)
	{
		pemfh->nPalEntries = 0;
		// update EOF so that the arrived data is usable
		if (pEmr->iType!=EMR_HEADER)
		{
			pEmr->iType = EMR_EOF;
			pEmr->nSize = sizeof(EMREOF);
			
			EMREOF* pRecEof = (EMREOF*)pEmr;
			pRecEof->nPalEntries = 0;
			pRecEof->offPalEntries = sizeof(EMREOF);
			pRecEof->nSizeLast = pEmr->nSize;
		}
	}

	return dwWholeRecs;
}

