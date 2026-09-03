/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMF2TEXT_H_
#define _SCEMF2TEXT_H_

#include "SCBrkEMF.h"

// class used to parse metafile records
class CSCEMF2Text : public SCBrkEMF
{

public:
	CSCEMF2Text();
	virtual ~CSCEMF2Text();
	void STResetCracker();
//accessors
#ifdef _DEBUG
    virtual CString SCGetClassName() { return _T("CSCEMF2Text"); };
#endif //_DEBUG
	//getters

	//setters
	void SCSetCallbackThreshold(int iCbThreshold)
	{
		ASSERT(iCbThreshold>0);
		m_iCbThreshold = (iCbThreshold>0)? iCbThreshold : 1;
	}

public:
//operation
	SC_BRKRESULT SCMetafile2Text(HDC hdc,
								  HENHMETAFILE hemf,
								  LPVOID lpData,
								  CONST RECT *lpRect,
								  BOOL bLogRecord=FALSE);
	// helpers
	virtual void SCEndBreak();
	virtual SC_BRKRESULT OnBrkUNKRecord(long lMsg);

protected:
//implementation
	SC_BRKRESULT SCBasicLog();
	SC_BRKRESULT SCExtraLog(LPCTSTR lpszExtra);
	SC_BRKRESULT SCExtraLogNoplay(LPCTSTR lpszExtra);
	
//mapped methods
//Add new EMF event handlers inside this block
//NOTE: The AFX prefix is used only for color stuff
	//{{AFX_SC_EMF_MSG
	virtual SC_BRKRESULT OnEmfHEADER();
	virtual SC_BRKRESULT OnEmfPOLYGON();
	virtual SC_BRKRESULT OnEmfPOLYPOLYGON();
	virtual SC_BRKRESULT OnEmfSETWINDOWEXTEX();
	virtual SC_BRKRESULT OnEmfSETWINDOWORGEX();
	virtual SC_BRKRESULT OnEmfSETVIEWPORTEXTEX();
	virtual SC_BRKRESULT OnEmfSETVIEWPORTORGEX();
	virtual SC_BRKRESULT OnEmfSETBRUSHORGEX();
	virtual SC_BRKRESULT OnEmfEOF();
	virtual SC_BRKRESULT OnEmfSETPIXELV();
	virtual SC_BRKRESULT OnEmfSETMAPPERFLAGS();
	virtual SC_BRKRESULT OnEmfSETMAPMODE();
	virtual SC_BRKRESULT OnEmfSETBKMODE();
	virtual SC_BRKRESULT OnEmfSETPOLYFILLMODE();
	virtual SC_BRKRESULT OnEmfSETROP2();
	virtual SC_BRKRESULT OnEmfSETSTRETCHBLTMODE();
	virtual SC_BRKRESULT OnEmfSETTEXTALIGN();
	virtual SC_BRKRESULT OnEmfSETTEXTCOLOR();
	virtual SC_BRKRESULT OnEmfSETBKCOLOR();
	virtual SC_BRKRESULT OnEmfOFFSETCLIPRGN();
	virtual SC_BRKRESULT OnEmfMOVETOEX();
	virtual SC_BRKRESULT OnEmfEXCLUDECLIPRECT();
	virtual SC_BRKRESULT OnEmfINTERSECTCLIPRECT();
	virtual SC_BRKRESULT OnEmfSCALEVIEWPORTEXTEX();
	virtual SC_BRKRESULT OnEmfSCALEWINDOWEXTEX();
	virtual SC_BRKRESULT OnEmfRESTOREDC();
	virtual SC_BRKRESULT OnEmfSETWORLDTRANSFORM();
	virtual SC_BRKRESULT OnEmfMODIFYWORLDTRANSFORM();
	virtual SC_BRKRESULT OnEmfSELECTOBJECT();
	virtual SC_BRKRESULT OnEmfCREATEPEN();
	virtual SC_BRKRESULT OnEmfCREATEBRUSHINDIRECT();
	virtual SC_BRKRESULT OnEmfDELETEOBJECT();
	virtual SC_BRKRESULT OnEmfANGLEARC();
	virtual SC_BRKRESULT OnEmfELLIPSE();
	virtual SC_BRKRESULT OnEmfRECTANGLE();
	virtual SC_BRKRESULT OnEmfROUNDRECT();
	virtual SC_BRKRESULT OnEmfARC();
	virtual SC_BRKRESULT OnEmfSELECTPALETTE();
	virtual SC_BRKRESULT OnEmfCREATEPALETTE();
	virtual SC_BRKRESULT OnEmfSETPALETTEENTRIES();
	virtual SC_BRKRESULT OnEmfRESIZEPALETTE();
	virtual SC_BRKRESULT OnEmfEXTFLOODFILL();
	virtual SC_BRKRESULT OnEmfLINETO();
	virtual SC_BRKRESULT OnEmfPOLYDRAW();
	virtual SC_BRKRESULT OnEmfSETARCDIRECTION();
	virtual SC_BRKRESULT OnEmfSETMITERLIMIT();
	virtual SC_BRKRESULT OnEmfFILLPATH();
	virtual SC_BRKRESULT OnEmfSELECTCLIPPATH();
	virtual SC_BRKRESULT OnEmfGDICOMMENT();
	virtual SC_BRKRESULT OnEmfFILLRGN();
	virtual SC_BRKRESULT OnEmfFRAMERGN();
	virtual SC_BRKRESULT OnEmfPAINTRGN();
	virtual SC_BRKRESULT OnEmfEXTSELECTCLIPRGN();
	virtual SC_BRKRESULT OnEmfBITBLT();
	virtual SC_BRKRESULT OnEmfSTRETCHBLT();
	virtual SC_BRKRESULT OnEmfMASKBLT();
	virtual SC_BRKRESULT OnEmfPLGBLT();
	virtual SC_BRKRESULT OnEmfSETDIBITSTODEVICE();
	virtual SC_BRKRESULT OnEmfSTRETCHDIBITS();
	virtual SC_BRKRESULT OnEmfEXTCREATEFONTINDIRECTW();
	virtual SC_BRKRESULT OnEmfEXTTEXTOUTA();
	virtual SC_BRKRESULT OnEmfEXTTEXTOUTW();
	virtual SC_BRKRESULT OnEmfPOLYGON16();
	virtual SC_BRKRESULT OnEmfPOLYPOLYGON16();
	virtual SC_BRKRESULT OnEmfPOLYDRAW16();
	virtual SC_BRKRESULT OnEmfCREATEMONOBRUSH();
	virtual SC_BRKRESULT OnEmfCREATEDIBPATTERNBRUSHPT();
	virtual SC_BRKRESULT OnEmfEXTCREATEPEN();
	virtual SC_BRKRESULT OnEmfPOLYTEXTOUTA();
	virtual SC_BRKRESULT OnEmfSETICMMODE();
	virtual SC_BRKRESULT OnEmfCREATECOLORSPACE();
	virtual SC_BRKRESULT OnEmfSETCOLORSPACE();
	virtual SC_BRKRESULT OnEmfGLSRECORD();
	virtual SC_BRKRESULT OnEmfGLSBOUNDEDRECORD();
	virtual SC_BRKRESULT OnEmfPIXELFORMAT();
	virtual SC_BRKRESULT OnEmfDRAWESCAPE();
	virtual SC_BRKRESULT OnEmfSMALLTEXTOUT();
	virtual SC_BRKRESULT OnEmfNAMEDESCAPE();
	virtual SC_BRKRESULT OnEmfCOLORCORRECTPALETTE();
	virtual SC_BRKRESULT OnEmfSETICMPROFILEA();
	virtual SC_BRKRESULT OnEmfALPHABLEND();
	virtual SC_BRKRESULT OnEmfALPHADIBBLEND();
	virtual SC_BRKRESULT OnEmfTRANSPARENTBLT();
	virtual SC_BRKRESULT OnEmfGRADIENTFILL();
	virtual SC_BRKRESULT OnEmfCREATECOLORSPACEW();
	//}}AFX_SC_EMF_MSG
	//End of mapped messages
	SC_DECLARE_MESSAGE_MAP()

private:
//service methods

private:
// data
	BOOL		m_bLogRecord;
	CString		m_strLogBuf;
	int			m_iCbThreshold;
};

#endif //_SCEMF2TEXT_H_
//  ------------------------------------------------------------
