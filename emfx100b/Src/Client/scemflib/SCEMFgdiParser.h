/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFGDIPARSER_H_
#define _SCEMFGDIPARSER_H_

#include "SCBrkEMF.h"
#include "SCEMFdcRenderer.h"

// don't define SC_USE_DCCLONER before you fix cloner's problems
#ifdef SC_USE_DCCLONER
class SCDCCloner;
#endif

// class used to parse metafile records
class CSCEMFgdiParser : public SCBrkEMF
{
// Construction/Destruction
public:
	CSCEMFgdiParser();
	CSCEMFgdiParser(I_EMFRenderer* pRenderer);
	virtual ~CSCEMFgdiParser();
	void SCResetCracker();

// Accessors
	// getters
	#ifdef _DEBUG
    virtual CString SCGetClassName() const { return _T("CSCEMFgdiParser"); };
	#endif
	I_EMFRenderer*	SCGetRenderer() const { return m_pRenderer; }

	// setters
	void SCSetRenderer(I_EMFRenderer* pRenderer) { m_pRenderer = pRenderer; }

// Operation
public:
	SC_BRKRESULT SCParse(HENHMETAFILE hEMF, HDC hDC, CONST RECT *lpDestRect)
	{ return SCBreakMetafile(hDC, hEMF, NULL, lpDestRect); }
	void SCRestorePreRenderingState()
	{// Not called automatically
		ASSERT(m_pRenderer);
		m_pRenderer->SCSetWorldTransform(m_xformOrig);
	}

// Implementation
protected:
	// helpers
	virtual SC_BRKRESULT SCBeginBreak(HDC hdc, LPVOID lpData, CONST RECT *lpRect);
	virtual SC_BRKRESULT OnBrkUNKRecord(long lMsg);

	SC_BRKRESULT SCEmfTextoutA(EMREXTTEXTOUTA* pRec, EMRTEXT* pTextRec);
	SC_BRKRESULT SCEmfTextoutW(EMREXTTEXTOUTW* pRec, EMRTEXT* pTextRec);
	void SCFinishTextout(EMREXTTEXTOUTA* pRec, EMRTEXT* pTextRec, LPWSTR pwStr, UINT uiOptions);

//mapped methods
//Add new EMF event handlers inside this block
//NOTE: The AFX prefix is used only for color stuff
	//{{AFX_SC_EMF_MSG
	virtual SC_BRKRESULT OnEmfHEADER();
	virtual SC_BRKRESULT OnEmfPOLYBEZIER();
	virtual SC_BRKRESULT OnEmfPOLYGON();
	virtual SC_BRKRESULT OnEmfPOLYLINE();
	virtual SC_BRKRESULT OnEmfPOLYBEZIERTO();
	virtual SC_BRKRESULT OnEmfPOLYLINETO();
	virtual SC_BRKRESULT OnEmfPOLYPOLYLINE();
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
	virtual SC_BRKRESULT OnEmfSETCOLORADJUSTMENT();
	virtual SC_BRKRESULT OnEmfSETTEXTCOLOR();
	virtual SC_BRKRESULT OnEmfSETBKCOLOR();
	virtual SC_BRKRESULT OnEmfOFFSETCLIPRGN();
	virtual SC_BRKRESULT OnEmfMOVETOEX();
	virtual SC_BRKRESULT OnEmfSETMETARGN();
	virtual SC_BRKRESULT OnEmfEXCLUDECLIPRECT();
	virtual SC_BRKRESULT OnEmfINTERSECTCLIPRECT();
	virtual SC_BRKRESULT OnEmfSCALEVIEWPORTEXTEX();
	virtual SC_BRKRESULT OnEmfSCALEWINDOWEXTEX();
	virtual SC_BRKRESULT OnEmfSAVEDC();
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
	virtual SC_BRKRESULT OnEmfCHORD();
	virtual SC_BRKRESULT OnEmfPIE();
	virtual SC_BRKRESULT OnEmfSELECTPALETTE();
	virtual SC_BRKRESULT OnEmfCREATEPALETTE();
	virtual SC_BRKRESULT OnEmfSETPALETTEENTRIES();
	virtual SC_BRKRESULT OnEmfRESIZEPALETTE();
	virtual SC_BRKRESULT OnEmfREALIZEPALETTE();
	virtual SC_BRKRESULT OnEmfEXTFLOODFILL();
	virtual SC_BRKRESULT OnEmfLINETO();
	virtual SC_BRKRESULT OnEmfARCTO();
	virtual SC_BRKRESULT OnEmfPOLYDRAW();
	virtual SC_BRKRESULT OnEmfSETARCDIRECTION();
	virtual SC_BRKRESULT OnEmfSETMITERLIMIT();
	virtual SC_BRKRESULT OnEmfBEGINPATH();
	virtual SC_BRKRESULT OnEmfENDPATH();
	virtual SC_BRKRESULT OnEmfCLOSEFIGURE();
	virtual SC_BRKRESULT OnEmfFILLPATH();
	virtual SC_BRKRESULT OnEmfSTROKEANDFILLPATH();
	virtual SC_BRKRESULT OnEmfSTROKEPATH();
	virtual SC_BRKRESULT OnEmfFLATTENPATH();
	virtual SC_BRKRESULT OnEmfWIDENPATH();
	virtual SC_BRKRESULT OnEmfSELECTCLIPPATH();
	virtual SC_BRKRESULT OnEmfABORTPATH();
	virtual SC_BRKRESULT OnEmfGDICOMMENT();
	virtual SC_BRKRESULT OnEmfFILLRGN();
	virtual SC_BRKRESULT OnEmfFRAMERGN();
	virtual SC_BRKRESULT OnEmfINVERTRGN();
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
	virtual SC_BRKRESULT OnEmfPOLYBEZIER16();
	virtual SC_BRKRESULT OnEmfPOLYGON16();
	virtual SC_BRKRESULT OnEmfPOLYLINE16();
	virtual SC_BRKRESULT OnEmfPOLYBEZIERTO16();
	virtual SC_BRKRESULT OnEmfPOLYLINETO16();
	virtual SC_BRKRESULT OnEmfPOLYPOLYLINE16();
	virtual SC_BRKRESULT OnEmfPOLYPOLYGON16();
	virtual SC_BRKRESULT OnEmfPOLYDRAW16();
	virtual SC_BRKRESULT OnEmfCREATEMONOBRUSH();
	virtual SC_BRKRESULT OnEmfCREATEDIBPATTERNBRUSHPT();
	virtual SC_BRKRESULT OnEmfEXTCREATEPEN();
	virtual SC_BRKRESULT OnEmfPOLYTEXTOUTA();
	virtual SC_BRKRESULT OnEmfPOLYTEXTOUTW();
	virtual SC_BRKRESULT OnEmfSETICMMODE();
	virtual SC_BRKRESULT OnEmfCREATECOLORSPACE();
	virtual SC_BRKRESULT OnEmfSETCOLORSPACE();
	virtual SC_BRKRESULT OnEmfDELETECOLORSPACE();
	virtual SC_BRKRESULT OnEmfGLSRECORD();
	virtual SC_BRKRESULT OnEmfGLSBOUNDEDRECORD();
	virtual SC_BRKRESULT OnEmfPIXELFORMAT();
	virtual SC_BRKRESULT OnEmfDRAWESCAPE();
	virtual SC_BRKRESULT OnEmfEXTESCAPE();
	virtual SC_BRKRESULT OnEmfSTARTDOC();
	virtual SC_BRKRESULT OnEmfSMALLTEXTOUT();
	virtual SC_BRKRESULT OnEmfFORCEUFIMAPPING();
	virtual SC_BRKRESULT OnEmfNAMEDESCAPE();
	virtual SC_BRKRESULT OnEmfCOLORCORRECTPALETTE();
	virtual SC_BRKRESULT OnEmfSETICMPROFILEA();
	virtual SC_BRKRESULT OnEmfSETICMPROFILEW();
	virtual SC_BRKRESULT OnEmfALPHABLEND();
	virtual SC_BRKRESULT OnEmfALPHADIBBLEND();
	virtual SC_BRKRESULT OnEmfTRANSPARENTBLT();
	virtual SC_BRKRESULT OnEmfTRANSPARENTDIB();
	virtual SC_BRKRESULT OnEmfGRADIENTFILL();
	virtual SC_BRKRESULT OnEmfSETLINKEDUFIS();
	virtual SC_BRKRESULT OnEmfSETTEXTJUSTIFICATION();
	virtual SC_BRKRESULT OnEmfCOLORMATCHTOTARGETW();
	virtual SC_BRKRESULT OnEmfCREATECOLORSPACEW();
	//}}AFX_SC_EMF_MSG
	//End of mapped messages
	SC_DECLARE_MESSAGE_MAP()

// Service methods
private:

// Data
private:
//got from outside
		//before enumeration
	I_EMFRenderer*		m_pRenderer;
#ifdef SC_USE_DCCLONER
	SCDCCloner*			m_pCloneDC;
#endif
	XFORM				m_xformOrig;
		//after enumerating the first record
	HDC					m_hDestDC;
// specific
};

#endif //_SCEMFGDIPARSER_H_
//  ------------------------------------------------------------
