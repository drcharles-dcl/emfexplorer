/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _WERR_H_
#define _WERR_H_

void SCShowWinError();
void SCShowWinError(DWORD dwError, LPCTSTR lpszCaption=NULL, LPCTSTR lpszFnMsg=NULL);

#ifdef _DEBUG
	#define SC_LOGERROR(strMsg) \
	{ \
		AfxMessageBox(strMsg); \
	}
	#define SC_LOGERROR1(szFmt, Param) \
	{ \
		CString strMsg; \
		strMsg.Format(szFmt, Param); \
		AfxMessageBox(strMsg); \
	}
	#define SC_LOGERROR2(szFmt, Param1, Param2) \
	{ \
		CString strMsg; \
		strMsg.Format(szFmt, Param1, Param2); \
		AfxMessageBox(strMsg); \
	}
#else
	#define SC_LOGERROR(strMsg)
	#define SC_LOGERROR1(szFmt, Param)
	#define SC_LOGERROR2(szFmt, Param1, Param2)
#endif


#endif //_WERR_H_
//  ------------------------------------------------------------
