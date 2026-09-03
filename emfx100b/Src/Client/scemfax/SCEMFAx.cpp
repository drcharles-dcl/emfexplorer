/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCEMFAx.h"
#include "CatHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSCEMFAxApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xdefa7d60, 0xde12, 0x4055, { 0x95, 0x3e, 0xdf, 0xb2, 0xc9, 0xb8, 0x94, 0xd6 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

CLIPFORMAT s_cfRTF;

////////////////////////////////////////////////////////////////////////////
// CSCEMFAxApp::InitInstance - DLL initialization

BOOL CSCEMFAxApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// Add your own module initialization code here.
		s_cfRTF = (CLIPFORMAT)::RegisterClipboardFormat(CF_RTF);
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CSCEMFAxApp::ExitInstance - DLL termination

int CSCEMFAxApp::ExitInstance()
{
	// Add your own module termination code here.
	return COleControlModule::ExitInstance();
}



/////////////////////////////////////////////////////////////////////////////
// SCRegisterMimeType - Adds entries to the system registry
#define SC_REGSZ_SIZE(strg) ((_tcslen(strg)+1)*sizeof(TCHAR))

// Note: EMG is an alias for EMF, EMZ, WMZ (We don't want to change the application
// already installed for these types of file)
STDAPI SCRegisterMimeType()
{
    HKEY        hkey = NULL;
    HKEY        hkey1 = NULL;
    BOOL        fErr = TRUE;
    TCHAR       szSubKey[513];

    // file extension for new mime type
    const TCHAR* pszMTExt = _T(".emg");
    // text for new mime content type
    const TCHAR* pszMTContent = _T("application/emg");
    // text for mimetype subkey
    const TCHAR* pszMTSubKey = _T("MIME\\DataBase\\Content Type\\application/emg");
    // extension named value
    const TCHAR* pszMTExtVal = _T("Extension");
    // clsid
    const TCHAR* pszMTCLSID = _T("{59F9FF0E-0C91-4B80-8DE3-119E19A96AE0}");
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

        // Register .mtp as a file extension this is only necessary for new file extensions, addimg
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

        fErr = FALSE;
    } while (FALSE);

    if ( hkey )
        RegCloseKey(hkey);

    if ( hkey1 )
        RegCloseKey(hkey1);

    if ( fErr )
        MessageBox(0, _T("Cannot register 'application/emg' mime type"), _T("Registration Error"), MB_OK);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

//Define the GUID associated with the safety component categories:
const CATID CATID_SafeForScripting     =
	{0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
const CATID CATID_SafeForInitializing  =
	{0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
//Define the GUID associated with control
const GUID CDECL BASED_CODE _ctlid =
	{0x59f9ff0e, 0xc91, 0x4b80, {0x8d, 0xe3, 0x11, 0x9e, 0x19, 0xa9, 0x6a, 0xe0}};

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	/////////////////////////////////////////////////////////
	// Mark control as both Safe for Scripting and Initialization
	if (FAILED( CreateComponentCategory(
		CATID_SafeForScripting,
		L"Controls that are safely scriptable") ))
		return ResultFromScode(SELFREG_E_CLASS);
	
	if (FAILED( CreateComponentCategory(
		CATID_SafeForInitializing,
		L"Controls safely initializable from persistent data") ))
		return ResultFromScode(SELFREG_E_CLASS);
	
	if (FAILED( RegisterCLSIDInCategory(
		_ctlid, CATID_SafeForScripting) ))
		return ResultFromScode(SELFREG_E_CLASS);
	
	if (FAILED( RegisterCLSIDInCategory(
		_ctlid, CATID_SafeForInitializing) ))
		return ResultFromScode(SELFREG_E_CLASS);
	/////////////////////////////////////////////////////////

	return SCRegisterMimeType();
	//return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
