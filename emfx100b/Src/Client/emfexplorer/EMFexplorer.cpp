/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "MainFrm.h"
#include "IpFrame.h"
#include "EMFexplorerDoc.h"
#include "EMFexplorerView.h"
#include "SCGenInclude.h"
#include SC_INC_COMMON(kSCProdDefs.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLIPFORMAT s_cfRTF;

//	#define SC_UNREGISTER

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerApp

BEGIN_MESSAGE_MAP(CEMFexplorerApp, CWinApp)
	//{{AFX_MSG_MAP(CEMFexplorerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerApp construction

CEMFexplorerApp::CEMFexplorerApp()
{
	// Add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEMFexplorerApp object

CEMFexplorerApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {BD61D528-D628-42E2-9F75-79B8535F78C1}
static const CLSID clsid =
{ 0xbd61d528, 0xd628, 0x42e2, { 0x9f, 0x75, 0x79, 0xb8, 0x53, 0x5f, 0x78, 0xc1 } };

#ifdef SC_UNREGISTER
STDAPI SCUnRegisterMimeType(CSingleDocTemplate* pDocTemplate);
#else
STDAPI SCRegisterMimeType(CSingleDocTemplate* pDocTemplate);
#endif
BOOL SCCanAccessClassRoot(CSingleDocTemplate* pDocTemplate);

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerApp initialization

BOOL CEMFexplorerApp::InitInstance()
{
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	if (!m_GDIPlus.SCInitInstance())
	{
		TRACE0("Failed to initialize GDI+");
		return FALSE;
	}
	s_cfRTF = (CLIPFORMAT)::RegisterClipboardFormat(CF_RTF);

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(SC_COMPANYNAME);

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEMFexplorerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CEMFexplorerView));
	pDocTemplate->SetServerInfo(
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
		RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// Note: SDI applications register server objects only if /Embedding
		//   or /Automation is present on the command line.

	// Enable DDE Execute open
	EnableShellOpen();
#ifndef SC_UNREGISTER
	RegisterShellFileTypes(TRUE);
#endif
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	if (SCCanAccessClassRoot(pDocTemplate))
	{
#ifdef SC_UNREGISTER
		SCUnRegisterMimeType(pDocTemplate);
#else
		m_server.UpdateRegistry(OAT_DOC_OBJECT_SERVER);
		SCRegisterMimeType(pDocTemplate);
#endif
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	//m_pMainWnd->ShowWindow(SW_SHOW);
	((CMainFrame*)m_pMainWnd)->SCInitialShow();
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEMFexplorerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerApp message handlers


int CEMFexplorerApp::ExitInstance() 
{
	// Add your specialized code here and/or call the base class
	
	m_GDIPlus.SCExitInstance();
	return CWinApp::ExitInstance();
}

void CEMFexplorerApp::OnFileOpen() 
{
	CString sFilters(SC_EMFMODE_FILTERS_LOAD);
	CFileDialog dlg(TRUE, NULL, _T("*.emf"), OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
		(LPCTSTR)sFilters);
	if (dlg.DoModal() != IDOK)
		return;

	OpenDocumentFile(dlg.GetPathName());
}

BOOL SCCanAccessClassRoot(CSingleDocTemplate* pDocTemplate)
{
	CString strFtype;
	pDocTemplate->GetDocString(strFtype, CDocTemplate::regFileTypeId);
	if (strFtype.IsEmpty())
		return FALSE;

	HKEY hTestKey = NULL;
	LONG lResult = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, 
		strFtype, 0, _T(""), 
		REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS, NULL, 
		&hTestKey, NULL);
	if ((ERROR_SUCCESS == lResult) && (hTestKey != NULL))
	{
		// It's ok to call UpdateRegistry
		RegCloseKey(hTestKey);
		return TRUE;
	}
	return FALSE;
}

#define SC_REGSZ_SIZE(strg) ((_tcslen(strg)+1)*sizeof(TCHAR))

#ifndef SC_UNREGISTER
STDAPI SCRegisterMimeType(CSingleDocTemplate* pDocTemplate)
{
    HKEY        hkey = NULL;
    HKEY        hkey1 = NULL;
    BOOL        fErr = TRUE;
    TCHAR       szSubKey[513];

    // file extension for new mime type
    const TCHAR* pszMTExt = _T(".bgp");
    // text for new mime content type
    const TCHAR* pszMTContent = _T("application/bgp");
    // text for mimetype subkey
    const TCHAR* pszMTSubKey = _T("MIME\\DataBase\\Content Type\\application/bgp");
    // extension named value
    const TCHAR* pszMTExtVal = _T("Extension");
    // clsid
    const TCHAR* pszMTCLSID = _T("{BD61D528-D628-42E2-9F75-79B8535F78C1}");
    // clsid named value name
    const TCHAR* pszMTCLSIDVal = _T("CLSID");
    // content type named value name
    const TCHAR* pszMTContentVal = _T("Content Type");
    // EnableFullPage key
    const TCHAR* pszMTFullPage = _T("EnableFullPage");

    do 
    {
        // create new mime type key for our new mimetype.  Only necessary for new mime types
        if ( ERROR_SUCCESS != RegCreateKey(HKEY_CLASSES_ROOT, pszMTSubKey, &hkey) )
            break;

        // add extension value to that mime type key to associate .mtp files with the 
        // application/x-mimetype mime type
        if ( ERROR_SUCCESS != RegSetValueEx(hkey, pszMTExtVal, 0, REG_SZ, 
            (const BYTE *)pszMTExt, SC_REGSZ_SIZE(pszMTExt)) )
            break;

        // Add class id to associate this object with the mime type
        if ( ERROR_SUCCESS != RegSetValueEx(hkey, pszMTCLSIDVal, 0, REG_SZ,
            (const BYTE *)pszMTCLSID, SC_REGSZ_SIZE(pszMTCLSID)) )
            break;

        RegCloseKey(hkey);

        // Register .bgp as a file extension this is only necessary for new file extensions, adding
        // a new player for .avi files for instance would not require this
        if ( ERROR_SUCCESS != RegCreateKey(HKEY_CLASSES_ROOT, pszMTExt, &hkey) )
            break;

        // Add content type to associate this extension with the content type.  This is required
        // and is used when the mime type is unknown and IE looks up associations by extension
        if ( ERROR_SUCCESS != RegSetValueEx(hkey, pszMTContentVal, 0, REG_SZ,
            (const BYTE *)pszMTContent, SC_REGSZ_SIZE(pszMTContent)) )
            break;

        RegCloseKey(hkey);

        // Open the key under the control's clsid HKEY_CLASSES_ROOT\CLSID\<CLSID>
        wsprintf(szSubKey, _T("%s\\%s"), pszMTCLSIDVal, pszMTCLSID);
        if ( ERROR_SUCCESS != RegOpenKey(HKEY_CLASSES_ROOT, szSubKey, &hkey) )
            break;

        // Create the EnableFullPage and extension key under this so that we can display files
        // with the extension full frame in the browser
        wsprintf(szSubKey, _T("%s\\%s"), pszMTFullPage, pszMTExt);
        if ( ERROR_SUCCESS != RegCreateKey(hkey, szSubKey, &hkey1) )
            break;

		// If you deactivate this, you know what you're doing
#if 1
		// Make the object 'not insertable'
        if ( ERROR_SUCCESS != RegDeleteKey(hkey, _T("Insertable")) )
            break;
		RegCloseKey(hkey1);
		hkey1 = NULL;
        if ( ERROR_SUCCESS != RegCreateKey(hkey, _T("NotInsertable"), &hkey1) )
            break;

		if (pDocTemplate)
		{
			// under EMFexplorer.Document, add "NotInsertable"
			CString strFtype;
			pDocTemplate->GetDocString(strFtype, CDocTemplate::regFileTypeId);
			if (!strFtype.IsEmpty())
			{
				HKEY        hkey2 = NULL;
				strFtype += _T("\\NotInsertable");
				if ( ERROR_SUCCESS != RegCreateKey(HKEY_CLASSES_ROOT, strFtype, &hkey2) )
					break;
				RegCloseKey(hkey2);
			}
		}
#endif

        fErr = FALSE;
    } while (FALSE);

    if ( hkey )
        RegCloseKey(hkey);

    if ( hkey1 )
        RegCloseKey(hkey1);

    if ( fErr )
        MessageBox(0, _T("Cannot register 'application/bgp' mime type"), _T("Registration Error"), MB_OK);

	return NOERROR;
}

#else

// Warning: this will only remove additional keys we added,
// not what is stored by RegisterShellTypes.
STDAPI SCUnRegisterMimeType(CSingleDocTemplate* pDocTemplate)
{
    HKEY        hkey = NULL;
    HKEY        hkey1 = NULL;
    BOOL        fErr = TRUE;
    TCHAR       szSubKey[513];

    // file extension for new mime type
    const TCHAR* pszMTExt = _T(".bgp");
    // text for new mime content type
    const TCHAR* pszMTContent = _T("application/bgp");
    // text for mimetype subkey
    const TCHAR* pszMTSubKey = _T("MIME\\DataBase\\Content Type\\application/bgp");
    // extension named value
    const TCHAR* pszMTExtVal = _T("Extension");
    // clsid
    const TCHAR* pszMTCLSID = _T("{BD61D528-D628-42E2-9F75-79B8535F78C1}");
    // clsid named value name
    const TCHAR* pszMTCLSIDVal = _T("CLSID");
    // content type named value name
    const TCHAR* pszMTContentVal = _T("Content Type");
    // EnableFullPage key
    const TCHAR* pszMTFullPage = _T("EnableFullPage");

    do 
    {
        // our new mimetype key.
        if ( ERROR_SUCCESS != RegCreateKey(HKEY_CLASSES_ROOT, pszMTSubKey, &hkey) )
            break;

        // del extension value to that mime type
		RegDeleteKey(hkey, pszMTExtVal);
        // del class id to dissociate this object from the mime type
		RegDeleteKey(hkey, pszMTCLSIDVal);

        RegCloseKey(hkey);
		RegDeleteKey(HKEY_CLASSES_ROOT, pszMTSubKey);

        // UnRegister .bgp as a file extension
        if ( ERROR_SUCCESS != RegCreateKey(HKEY_CLASSES_ROOT, pszMTExt, &hkey) )
            break;

        // del content type to dissociate this extension from the content type.
		RegDeleteKey(hkey, pszMTContentVal);

        RegCloseKey(hkey);
		RegDeleteKey(HKEY_CLASSES_ROOT, pszMTExt);

        // Open the key under the control's clsid HKEY_CLASSES_ROOT\CLSID\<CLSID>
        wsprintf(szSubKey, _T("%s\\%s"), pszMTCLSIDVal, pszMTCLSID);
        if ( ERROR_SUCCESS != RegOpenKey(HKEY_CLASSES_ROOT, szSubKey, &hkey) )
            break;

        // del the EnableFullPage and extension key under it
        wsprintf(szSubKey, _T("%s\\%s"), pszMTFullPage, pszMTExt);
		RegDeleteKey(hkey, szSubKey);
		RegDeleteKey(hkey, pszMTFullPage);

		// If you deactivate this, you know what you're doing
#if 1
		// Make the object 'not insertable'
        RegDeleteKey(hkey, _T("Insertable"));
        RegDeleteKey(hkey, _T("NotInsertable"));
		RegCloseKey(hkey);
        wsprintf(szSubKey, _T("%s\\%s"), pszMTCLSIDVal, pszMTCLSID);
		// wont work if it contains keys we did not installed (auxUsertype, DefaultIcon, etc...)
		RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);


		if (pDocTemplate)
		{
			// under EMFexplorer.Document, add "NotInsertable"
			CString strFtype;
			pDocTemplate->GetDocString(strFtype, CDocTemplate::regFileTypeId);
			if (!strFtype.IsEmpty())
			{
				strFtype += _T("\\NotInsertable");
				RegDeleteKey(HKEY_CLASSES_ROOT, strFtype);
			}
		}
#endif

        fErr = FALSE;
    } while (FALSE);

    if ( fErr )
        MessageBox(0, _T("Cannot unregister 'application/bgp' mime type"), _T("UnRegistration Error"), MB_OK);

	return NOERROR;
}
#endif
