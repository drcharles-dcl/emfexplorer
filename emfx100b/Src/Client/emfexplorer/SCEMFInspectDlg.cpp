/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFInspectDlg.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinGUI.h)
#include SC_INC_EMFLIB(SCEMF.h)
#include SC_INC_EMFLIB(SCEMFDoc.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_SPACE_X	10
#define SC_SPACE_Y	10

#define SC_NO_HSCROLL		1

#define SC_XMARGIN			72			// twips
#define SC_FIRSTLINE_INDENT	SC_XMARGIN	// twips

#define SC_PARA_TAB0		3*1440		// twips
#define SC_PARA_TAB1		(SC_PARA_TAB0 + 1440/4)
#define SC_PARA_INDENT		SC_PARA_TAB1
#define SC_RIGHT_INDENT		72			// twips

/////////////////////////////////////////////////////////////////////////////
// CSCEMFInspectDlg dialog


CSCEMFInspectDlg::CSCEMFInspectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCEMFInspectDlg::IDD, pParent),
	m_bDestroyed(FALSE),
	m_hEmf(NULL)
{
	//{{AFX_DATA_INIT(CSCEMFInspectDlg)
	m_EdBrkCount = _T("");
	m_strDPI = _T("");
	m_strSize = _T("");
	//}}AFX_DATA_INIT
	m_bStopBrk = FALSE;
	m_nMaxBrkPos = 0;
	m_bCracking = FALSE;
}

// auto-reset event to signal that the control has returned from breakmetafile;
// starts out not signaled
HANDLE g_hCrackerDead = CreateEvent(NULL, FALSE, FALSE, NULL);
// auto-reset event to signal that breakmetafile is requested to stop;
// starts out not signaled
HANDLE g_hEmergencyQuit = CreateEvent(NULL, FALSE, FALSE, NULL);

CSCEMFInspectDlg::~CSCEMFInspectDlg()
{
	if (m_bCracking)
	{
		m_bStopBrk = TRUE;
		m_bDestroyed = TRUE;
		SetEvent(g_hEmergencyQuit);
		while (WaitForSingleObject(g_hCrackerDead, 0) == WAIT_TIMEOUT)
		{
			SCYieldForAWhile();
		}
	}
}

void CSCEMFInspectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CSCEMFInspectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFInspectDlg)
	DDX_Control(pDX, IDC_PROGRESS_BRK, m_Progress);
	DDX_Text(pDX, IDC_EDIT_BRKCOUNT, m_EdBrkCount);
	DDX_Text(pDX, IDC_EDIT_DPI, m_strDPI);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_strSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSCEMFInspectDlg, CDialog)
	//{{AFX_MSG_MAP(CSCEMFInspectDlg)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_STOPBRK, OnBtnStopbrk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVERTF, OnBtnSaveRTF)
	ON_BN_CLICKED(IDC_BTN_COPYALL, OnBtnCopyAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFInspectDlg)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFInspectDlg support methods

void CSCEMFInspectDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// Add your message handler code here
	SCRelocateControls();
}

void CSCEMFInspectDlg::SCRelocateControls()
{
	if (!IsWindow(m_EdEMFContent.m_hWnd))
		return;
	
	CRect rect;
	GetClientRect(rect);
	
	// EMF edit
	CRect rect1;
	CWnd *pWnd;
	//stop button
	pWnd=GetDlgItem(IDC_BTN_STOPBRK);
	if (pWnd)
	{
		pWnd->GetWindowRect(&rect1);
		ScreenToClient(rect1);
		rect1.left = rect.left;
		pWnd->MoveWindow(rect1);
	}
	
	//progress
	m_Progress.GetWindowRect(&rect1);
	ScreenToClient(rect1);
	rect1.right = rect.right;
	m_Progress.MoveWindow(rect1);
	
	//EMF edit
	rect.top = rect1.bottom + SC_SPACE_Y;
	m_EdEMFContent.MoveWindow(rect);

	PARAFORMAT2 pf;
	memset(&pf, 0,  sizeof(pf));
	pf.cbSize = sizeof(pf);
	pf.dxStartIndent = SC_FIRSTLINE_INDENT;
	pf.dxOffset = SC_PARA_INDENT;
	pf.dxRightIndent = SC_RIGHT_INDENT;
	pf.dwMask = PFM_OFFSETINDENT | PFM_OFFSET | PFM_RIGHTINDENT;

	pf.dwMask |= PFM_TABSTOPS;
	pf.cTabCount = 2;
	pf.rgxTabs[0] = SC_PARA_TAB0;
	pf.rgxTabs[1] = SC_PARA_TAB1;
	
	m_EdEMFContent.SetParaFormat(pf);

#if SC_NO_HSCROLL
	HDC hTargetDC = ::CreateCompatibleDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hTargetDC, rect.Width(), rect.Height());
	HBITMAP hOldBm = (HBITMAP)SelectObject(hTargetDC, hbm);
	int iDpiX = GetDeviceCaps(hTargetDC, LOGPIXELSX);
	int iLineWidth = MulDiv(rect.Width() - GetSystemMetrics(SM_CXVSCROLL), 1440, iDpiX);
	iLineWidth -= 2*SC_XMARGIN;
	m_EdEMFContent.SetTargetDevice(hTargetDC, iLineWidth); // ok, it owns hTargetDC
	// but what happens to the bitmap (hbm)?
#endif
}

void CSCEMFInspectDlg::SCCheckTextContent() 
{
#if 0
	// for testing
	SCMarkText(_T("EMR_SAVEDC"), 0x000000FF);
	SCMarkText(_T("EMR_RESTOREDC"), 0x00FF0000);
#endif
}

void CSCEMFInspectDlg::SCMarkText(CString sText, COLORREF color) 
{
	//insert text at end
	CString sEdText;
	m_EdEMFContent.GetWindowText(sEdText);
	long tLen = sEdText.GetLength();
	if (tLen==0)
		return;

	CHARRANGE cr;

	// find the text
	long n = 0;

	while(n < tLen)
	{
		n = sEdText.Find(LPCTSTR(sText), n);
		if (n == -1)
			break;

		// go to start of line
		while ((n>0)&&(sEdText.Mid(n,1) != _T('\n')))
			n--;
		if (n>0)
			n++;
		cr.cpMin = n;

		// go to end of line
		n += sText.GetLength();
		while ((n<tLen)&&(sEdText.Mid(n,1) != _T('\n')))
			n++;

		//change color
		cr.cpMax = n;
		m_EdEMFContent.SetSel(cr);

		CHARFORMAT cf;
		m_EdEMFContent.GetSelectionCharFormat(cf);
		cf.cbSize = sizeof(cf);
		cf.dwEffects = NULL;
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = color;
		m_EdEMFContent.SetSelectionCharFormat(cf);

		// next occurrence
	}
	//clear selection
	cr.cpMin = cr.cpMax = tLen;
	m_EdEMFContent.SetSel(cr);
	//no undo allowed
	m_EdEMFContent.EmptyUndoBuffer();
}

/////////////////////////////////////////////////////////////////////////////
// CSCEMFInspectDlg message handlers

BOOL CSCEMFInspectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return SCInitDialog();
}
	
BOOL CSCEMFInspectDlg::SCInitDialog() 
{
	// create controls
	// edit text
	{
		CRect rect(0,0,200,100);
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN
			|WS_VSCROLL | ((SC_NO_HSCROLL)?0:WS_HSCROLL);
		if (!m_EdEMFContent.Create( dwStyle, rect, this, 1))
		{
			TRACE0("Could not create edit content\n");
			return FALSE;
		}
		m_EdEMFContent.SetOptions(ECOOP_OR, ECO_AUTOVSCROLL | 
			ECO_AUTOHSCROLL | ECO_AUTOWORDSELECTION | ECO_READONLY );

		//default font settings
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
		lf.lfHeight = 12;                      // request a 12-pixel-height font
		_tcscpy(lf.lfFaceName, _T("Arial"));   // request a face name "Arial"
		//get some parent font info
		CFont *pFont = GetFont();
		if (pFont)
		{
			pFont->GetLogFont(&lf);
		}
		VERIFY(m_Edfont.CreateFontIndirect(&lf));  // create the font

		m_EdEMFContent.SetFont(&m_Edfont, FALSE);
		m_EdEMFContent.SetBackgroundColor(FALSE, RGB(255, 255, 250));
	}

	//
	SCRelocateControls();

	// other controls
	SCUpdateAllControls();
	if (m_hEmf)
	{
		ModifyStyle(WS_CHILD, WS_DLGFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU,
			SWP_DRAWFRAME|SWP_SHOWWINDOW);
		SCShowControl(IDCANCEL, SW_SHOW);
		SCBeginCracking();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

long CSCEMFInspectDlg::OnSCBrkPos(long lCurPos)
{
	m_Progress.SetPos(lCurPos);
	CString strPos;
	m_EdBrkCount.Format(_T("%d/%d"), lCurPos, m_nMaxBrkPos);
	UpdateData(FALSE);
	SCYieldForAWhile(); // see comments before SCEMFcrkGuardian

	return (m_bStopBrk ? 0 : 1);
}

long CSCEMFInspectDlg::OnSCBrkMaxPos(long lMaxPos)
{
	m_Progress.SetRange32(0,lMaxPos);
	m_nMaxBrkPos = lMaxPos;
	m_EdBrkCount.Format(_T("%d recs"), m_nMaxBrkPos);
	UpdateData(FALSE);
	SCYieldForAWhile(); // see comments before SCEMFcrkGuardian

	return (m_bStopBrk ? 0 : 1);
}

long CSCEMFInspectDlg::OnSCBrkRecStr(long lCurPos, TCHAR *szRecStr)
{
	m_Progress.SetPos(lCurPos);
	long lLen0 = m_EdEMFContent.GetTextLength();
	m_EdEMFContent.SCAddText(szRecStr);
	m_EdBrkCount.Format(_T("%d/%d"), lCurPos, m_nMaxBrkPos);

	// Sometimes I want to deactivate it
#if 1
	long lLen1 = m_EdEMFContent.GetTextLength();
	CHARRANGE cr;

	// find the text
	FINDTEXTEX ft;
	//ft.chrg.cpMin = lLen0;
	ft.chrg.cpMax = lLen1;

	long lNext = lLen0;
	while (lNext<lLen1)
	{
		ft.chrg.cpMin = lNext;
#if (_RICHEDIT_VER >= 0x0200)
		ft.lpstrText = _T("EMR_");
#else
		ft.lpstrText = "EMR_";
#endif

		if (m_EdEMFContent.FindText(0, &ft)==-1)
			break;

		//change color
		cr.cpMin = ft.chrgText.cpMin;
		lNext = ft.chrgText.cpMax;

		ft.chrg.cpMin = lNext;
#if (_RICHEDIT_VER >= 0x0200)
		ft.lpstrText = _T("\t");
#else
		ft.lpstrText = "\t";
#endif

		if (m_EdEMFContent.FindText(0, &ft)!=-1)
		{
			cr.cpMax = ft.chrgText.cpMin;
			lNext = ft.chrgText.cpMax;

			m_EdEMFContent.SetSel(cr);
			
			CHARFORMAT cf;
			m_EdEMFContent.GetSelectionCharFormat(cf);
			cf.cbSize = sizeof(cf);
			cf.crTextColor = RGB(0, 0, 255);
			cf.dwEffects = CFE_BOLD;
			cf.dwMask = CFM_BOLD|CFM_COLOR;
			m_EdEMFContent.SetSelectionCharFormat(cf);
		}
	}
			
	//clear selection
	cr.cpMin = cr.cpMax = lLen1;
	m_EdEMFContent.SetSel(cr);
	//no undo allowed
	m_EdEMFContent.EmptyUndoBuffer();
#endif

	UpdateData(FALSE);
	SCYieldForAWhile(); // see comments before SCEMFcrkGuardian

	return (m_bStopBrk ? 0 : 1);
}

void CSCEMFInspectDlg::OnBtnStopbrk() 
{
	SCEnableGUI();
}

void CSCEMFInspectDlg::SCEnableGUI(BOOL bStop/*=TRUE*/) 
{
	m_bStopBrk = bStop;
	SCEnableControl(IDC_BTN_STOPBRK, !bStop);
	SCEnableControl(IDC_BTN_SAVERTF, bStop);
	SCEnableControl(IDC_BTN_COPYALL, bStop);
	SCEnableControl(IDCANCEL, bStop);
}


void CSCEMFInspectDlg::OnClose() 
{
	m_bStopBrk = TRUE;
	if (m_bCracking)
		return;

#if 0
// good for modal
	CDialog::OnClose();
#else
	CWnd* pWnd = ::AfxGetMainWnd();
	pWnd->PostMessage(WM_COMMAND, ID_VIEW_EMF_ASTEXT, 0);
#endif
}

void CSCEMFInspectDlg::OnCancel() 
{
	m_bStopBrk = TRUE;
	if (m_bCracking)
		return;
#if 0
// good for modal
	CDialog::OnCancel();
#else
	CWnd* pWnd = ::AfxGetMainWnd();
	pWnd->PostMessage(WM_COMMAND, ID_VIEW_EMF_ASTEXT, 0);
#endif
}

// Because we call SCYieldForAWhile in an enummetafile callback for GUI responsiveness,
// there are great dangers: the user may close CSCEMFInspectDlg in an uncontrolled manner
// (or even close the application), letting the enumeration process still running.
//
// We use a guardian thread to "wrap" the call to SCMetafile2Text, and
// until this function returns, in case CSCEMFInspectDlg is 
// destroyed while GDI is deep in metafile enumeration.
//
// TODO: solve the case of application destruction while enumeration is on progress;
//  for now, access violation will occur in GDI32 (inside EnumEnhMetaFile).
typedef struct tag_SCCrkInfo
{
	HDC				cri_hdc;
	CSCEMF2Text*	cri_pobj;
} SCCrkInfo, *PSCCrkInfo;

// this "wrapping" technique is not very classic; but we are using it to avoid calling
// inside this function an MFC object created in the main thread
DWORD WINAPI SCEMFcrkGuardian(LPVOID lpParam)
{
	PSCCrkInfo pCrk = (PSCCrkInfo)lpParam;

	WaitForSingleObject(g_hEmergencyQuit, INFINITE);

	CancelDC(pCrk->cri_hdc);
	pCrk->cri_pobj->SCSetStop(TRUE);
	SCEMFDCDispose(pCrk->cri_hdc);
	SetEvent(g_hCrackerDead);
	return 0;
}
/////////////////////////////////////////////////////////////////////////

void CSCEMFInspectDlg::SCBeginCracking()
{
	if (!m_hEmf)
		return;
	SetTimer(1, 1, NULL);
}

void CSCEMFInspectDlg::OnTimer(UINT nIDEvent)
{
	KillTimer(nIDEvent);

	m_EdEMFContent.SCSetEmpty();
	m_Progress.ShowWindow(SW_SHOW);
	m_Progress.SetRange32(0,0);
	m_Progress.SetPos(0);

	ENHMETAHEADER EmfHeader;
	
	if (!::GetEnhMetaFileHeader(m_hEmf, sizeof(ENHMETAHEADER), &EmfHeader))
	{
		TRACE0("Failed to get EMF header\n");
		return;
	}
	{// info
		long lEmfDPIX = 0;
		long lEmfDPIY = 0;
		long lEmfPaperCx = 0;
		long lEmfPaperCy = 0;
		SCGetEMFInfos(m_hEmf, lEmfDPIX, lEmfDPIY, lEmfPaperCx, lEmfPaperCy);
		m_strDPI.Format(_T("%d x %d dpi"), lEmfDPIX, lEmfDPIY);
		m_strSize.Format(_T("%d x %d"), lEmfPaperCx, lEmfPaperCy);
		UpdateData(FALSE);
	}
	
	HDC hNewEMFDC = NULL;
	RECT	rFrameRect;
	CopyRect(&rFrameRect,(const RECT *)&(EmfHeader.rclFrame));
	
	HDC hDc = ::GetWindowDC(m_EdEMFContent.m_hWnd);
	hNewEMFDC = ::CreateEnhMetaFile(hDc, (LPCTSTR)NULL, (LPRECT)&(EmfHeader.rclFrame) , NULL);
	::ReleaseDC(m_EdEMFContent.m_hWnd, hDc);

	SCInitEMFDCForEnumeration(hNewEMFDC, EmfHeader, hDc);
	CSCEMF2Text Cracker;
	Cracker.SCSetBrkCallBack(this);
#if 0
	// It is not harmful to activate it: high thresholds will reduce GUI responsiveness
	if (EmfHeader.nRecords>10000)
		Cracker.SCSetCallbackThreshold(EmfHeader.nRecords/100);
#endif
	m_nMaxBrkPos = 0;
	SCEnableGUI(FALSE);
	
	m_bCracking = TRUE;
	m_bDestroyed = FALSE;
#if 1
	// Note: pointers to local variables are beeing passed to the guardian thread
	DWORD dwThreadID;
	SCCrkInfo CrkInfo;
	CrkInfo.cri_hdc = hNewEMFDC;
	CrkInfo.cri_pobj = &Cracker;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SCEMFcrkGuardian, (LPVOID)&CrkInfo, 0, &dwThreadID);
	CloseHandle(hThread);

	SC_BRKRESULT brkError = Cracker.SCMetafile2Text(hNewEMFDC, m_hEmf,
		NULL,
		&rFrameRect,
		TRUE);

	if (m_bDestroyed)
		return;

	// force the guardian thread to stop
	SetEvent(g_hEmergencyQuit);
	WaitForSingleObject(g_hCrackerDead, INFINITE);
#else
	SC_BRKRESULT brkError = Cracker.SCMetafile2Text(hNewEMFDC, m_hEmf,
		NULL,
		&rFrameRect,
		TRUE);

	SCEMFDCDispose(hNewEMFDC);
#endif

	SCEnableGUI(TRUE);
	SCCheckTextContent();
	m_EdEMFContent.UpdateWindow();
	m_Progress.SetPos(0);
	
	m_bCracking = FALSE;
}

void CSCEMFInspectDlg::SCUpdateAllControls()
{
	UpdateData(FALSE);
}

void CSCEMFInspectDlg::SCCheckBtnControl(UINT uCtl, BOOL bCheck)
{
	CButton *pBtn = (CButton *)GetDlgItem(uCtl);
	if (pBtn)
		pBtn->SetCheck(bCheck);
}

void CSCEMFInspectDlg::SCShowControl(UINT uCtl, int nShow)
{
	CWnd *pWnd = GetDlgItem(uCtl);
	if (pWnd)
		pWnd->ShowWindow(nShow);
}

void CSCEMFInspectDlg::SCUpdateControl(UINT uCtl)
{
	CWnd *pWnd = GetDlgItem(uCtl);
	if (pWnd)
		pWnd->UpdateWindow();
}

static DWORD CALLBACK SCRTFSave(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	CFile* pFile = (CFile*)dwCookie;
	
	try
	{
		pFile->Write(pbBuff, cb);
		*pcb = cb;
	}
	catch (CFileException* pEx)
	{
		pEx->Delete();
		return 1; // Failed so stop calling us.
	}
	return 0;
}

BOOL CSCEMFInspectDlg::SCSaveDocument(LPCTSTR lpszPathname, BOOL bRTF/*=FALSE*/)
{
	if (m_bCracking)
		return FALSE;

	// Open the file for reading. 
	CFile outputFile;
	CFileException ex;
	if (!outputFile.Open(lpszPathname, CFile::modeCreate | CFile::modeWrite, &ex))
		return FALSE;

	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = SCRTFSave;
	es.dwCookie = (DWORD)&outputFile;
	m_EdEMFContent.StreamOut(bRTF ? SF_RTF : SF_TEXT, es);
	outputFile.Close();

	return TRUE;
}

void CSCEMFInspectDlg::SCCopy(BOOL bCopyAll/*=FALSE*/)
{
	if (m_bCracking)
		return;

	if (bCopyAll)
		SCSelectAll();

	m_EdEMFContent.Copy();
}

void CSCEMFInspectDlg::SCSelectAll()
{
	if (m_bCracking)
		return;

	CHARRANGE cr;
	cr.cpMin = 0;
	cr.cpMax = -1;
	m_EdEMFContent.SetSel(cr);
}

void CSCEMFInspectDlg::SCPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_bCracking)
		return;

	FORMATRANGE fr;
	fr.hdcTarget = pInfo->m_pPD->m_pd.hDC;
	ASSERT(fr.hdcTarget == pDC->m_hAttribDC);
	fr.hdc = pDC->m_hDC;
	fr.chrg.cpMax = -1;

	int nHorizRes = pDC->GetDeviceCaps(PHYSICALWIDTH);
	int nVertRes = pDC->GetDeviceCaps(PHYSICALHEIGHT);
	int nLogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	int nLogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
	int nMargin = MulDiv(250, 1440, 254); // twips (2.5cm)  all around

	::CopyRect(&fr.rcPage, &pInfo->m_rectDraw);
	SCRectToTwips(&fr.rcPage, nLogPixelsX, nLogPixelsY, pDC);

	::CopyRect(&fr.rc, &pInfo->m_rectDraw);
	SCRectToTwips(&fr.rc, nLogPixelsX, nLogPixelsY, pDC);
	::InflateRect(&fr.rc, -nMargin, -nMargin);

	if (pInfo->m_bPreview)
	{
		float fPreviewZoomRatio =  (pInfo->m_lpUserData) ? *((float*)pInfo->m_lpUserData): 1;
		// Prepare surface (scaling)
		CPoint	ptScrPageStart;
		// get offset of the printing area in the page
		ptScrPageStart.x = GetDeviceCaps(pDC->m_hDC, PHYSICALOFFSETX);
		ptScrPageStart.y = GetDeviceCaps(pDC->m_hDC, PHYSICALOFFSETY);
		
		//change window and viewport extent
		int iScrDevResX = GetDeviceCaps(pDC->m_hDC, LOGPIXELSX);
		int iScrDevResY = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);
		SetMapMode(pDC->m_hDC, MM_ISOTROPIC);
		SetWindowExtEx(pDC->m_hDC, iScrDevResX, iScrDevResY , NULL);
		SetViewportExtEx(pDC->m_hDC, (int)(iScrDevResX * fPreviewZoomRatio), (int)(iScrDevResY * fPreviewZoomRatio) , NULL);
	}

	long lTextOut = 0; 

	fr.chrg.cpMin = lTextOut;
	lTextOut = m_EdEMFContent.FormatRange(&fr, TRUE);
}

UINT CSCEMFInspectDlg::SCCountPages(CDC* pDC, CPrintInfo* pInfo)
{
	// Setup paper
	int nHorizRes;
	int nVertRes;
	int nMarginX = MulDiv(250, 1440, 254); // twips (2.5cm) 
	int nMarginY = nMarginX;			   // all around

	// Check default printer
	PRINTDLG pd = pInfo->m_pPD->m_pd;
	HDC hdc = pd.hDC;
	if (!pDC)
	{
		BOOL bExistPrinter = (AfxGetApp()->GetPrinterDeviceDefaults(&pd));
		pd.lStructSize = sizeof (PRINTDLG);
		pd.hwndOwner = m_EdEMFContent.m_hWnd;
		pd.hDevMode = (HANDLE)NULL;
		pd.hDevNames = (HANDLE)NULL;
		pd.nFromPage = 0;
		pd.nToPage = 0;
		pd.nMinPage = 0;
		pd.nMaxPage = 0;
		pd.nCopies = 0;
		pd.hInstance = (HINSTANCE)AfxGetApp()->m_hInstance;
		pd.Flags = PD_RETURNDEFAULT|PD_RETURNDC;
		pd.lpfnSetupHook = (LPSETUPHOOKPROC)(FARPROC)NULL;
		pd.lpSetupTemplateName = (LPTSTR)NULL;
		pd.lpfnPrintHook = (LPPRINTHOOKPROC)(FARPROC)NULL;
		pd.lpPrintTemplateName = (LPTSTR)NULL;
		if (bExistPrinter && (PrintDlg(&pd) == TRUE))
			hdc = pd.hDC;

		if (!hdc)
			return (unsigned)-1;

		pDC = CDC::FromHandle(hdc);
	}
	nHorizRes = GetDeviceCaps(hdc, PHYSICALWIDTH);
	nVertRes = GetDeviceCaps(hdc, PHYSICALHEIGHT);
	
	int nLogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	int nLogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
	
	FORMATRANGE fr;
	fr.hdcTarget = hdc;
	fr.hdc = hdc;
	fr.chrg.cpMax = -1;
	
	::SetRect(&fr.rcPage, 0, 0, nHorizRes, nVertRes);
	SCRectToTwips(&fr.rcPage, nLogPixelsX, nLogPixelsY, pDC);

	::CopyRect(&fr.rc, &fr.rcPage);
	::InflateRect(&fr.rc, -nMarginX, -nMarginY);

	long lTextLen = m_EdEMFContent.GetTextLength();
	long lTextOut = 0; 
	
	UINT uiNbPages = 0;
	while (lTextOut<lTextLen)
	{	
		fr.chrg.cpMin = lTextOut;
		lTextOut = m_EdEMFContent.FormatRange(&fr, FALSE);
		uiNbPages++;
	}
	
	// tell the control to release cached information
	m_EdEMFContent.FormatRange(NULL, FALSE);
	return uiNbPages;
}

void CSCEMFInspectDlg::OnBtnSaveRTF() 
{
	if (m_bCracking)
		return;

	CString sFilters(SC_BRKMODE_FILTERS);
	CFileDialog dlg(FALSE, NULL, _T("*.rtf"),
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
		(LPCTSTR)sFilters);
	if (dlg.DoModal() != IDOK)
		return;

	CString strPath = dlg.GetPathName();
	SCAddPathExtFromFilterIndex(strPath, sFilters, dlg.m_ofn.nFilterIndex);

	int iType = SC_FTYPE_UKN;
	switch (dlg.m_ofn.nFilterIndex)
	{
	case 1:  iType = SC_FILETYPE_RTF; break;
	case 2:  iType = SC_FILETYPE_TXT; break;
	default:
		ASSERT(0);
		return;
	}
	SCSaveDocument(strPath, (SC_FILETYPE_RTF==iType));
}

void CSCEMFInspectDlg::OnBtnCopyAll() 
{
	SCCopy(TRUE);	
}
