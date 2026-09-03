/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDOCUTILS_H_
#define _SCEMFDOCUTILS_H_

// Playlist sections and variables
#define SC_DOC_GEN_SECTION		_T("General")
#define SC_DOC_FILES_SECTION	_T("Files")
#define SC_DOC_SETTINGS_SECTION	_T("Settings")
#define SC_DOC_NBFILES_KEY		_T("NbFiles")
#define SC_DOC_FILEKEY_FORMAT	_T("File%d")
#define SC_DOC_FILEID_KEY		_T("ID")
#define SC_DOC_FILEID_VALUE		_T("BGP")
#define SC_DOC_EXTENSION		_T(".bgp")
#define SC_DOC_CREDITPAGE_KEY	_T("DfltCrdPage")
#define SC_DOC_PGSECTION_FORMAT	_T("Page%d")
#define SC_DOC_URL_KEY			_T("URL")
#define SC_DOC_CREDIT_KEY		_T("Credit")
#define SC_DOC_COMMENT_KEY		_T("Comment")

#define SC_PGCOMMENT_MAXSIZE	1024

#define SC_EMFMODE_FILTERS_LOAD								\
		_T("Enhanced metafiles (*.emf)|*.emf|")				\
		_T("Standard metafiles (*.wmf)|*.wmf|")				\
		_T("Compressed Enhanced metafiles (*.emz)|*.emz|")	\
		_T("Compresed Standard metafiles (*.wmz)|*.wmz|")	\
		_T("EMFexplorer Bound Graphics Pages (*.bgp)|*.bgp|")			\
		_T("Document files (*.rtf;*.txt)|*.rtf;*.txt|")		\
		_T("Images (*.bmp;*.jpg;*.png;*.gif;*.tiff)|*.bmp;*.jpg;*.png;*.gif;*.tiff|") \
		_T("|")


#define SC_EMFMODE_FILTERS_REPLACE							\
		_T("Enhanced metafiles (*.emf)|*.emf|")				\
		_T("Standard metafiles (*.wmf)|*.wmf|")				\
		_T("Compressed Enhanced metafiles (*.emz)|*.emz|")	\
		_T("Compresed Standard metafiles (*.wmz)|*.wmz|")	\
		_T("Images (*.bmp;*.jpg;*.png;*.gif;*.tiff)|*.bmp;*.jpg;*.png;*.gif;*.tiff|") \
		_T("|")


#define SC_EMFMODE_FILTERS_INSERT							\
		_T("Enhanced metafiles (*.emf)|*.emf|")				\
		_T("Standard metafiles (*.wmf)|*.wmf|")				\
		_T("Compressed Enhanced metafiles (*.emz)|*.emz|")	\
		_T("Compresed Standard metafiles (*.wmz)|*.wmz|")	\
		_T("Document files (*.rtf;*.txt)|*.rtf;*.txt|")		\
		_T("Images (*.bmp;*.jpg;*.png;*.gif;*.tiff)|*.bmp;*.jpg;*.png;*.gif;*.tiff|") \
		_T("|")


#define SC_EMFMODE_FILTERS_IMG								\
		_T("Enhanced metafile EMF (*.emf)|*.emf|")			\
		_T("Enhanced metafile EMF+ (*.emf)|*.emf|")			\
		_T("Enhanced metafile EMF+ dual (*.emf)|*.emf|")	\
		_T("Standard metafile (*.wmf)|*.wmf|")				\
		_T("Image Windows bitmap (*.bmp)|*.bmp|")			\
		_T("Image JPEG (*.jpg)|*.jpg|")						\
		_T("Image PNG (*.png)|*.png|")						\
		_T("Image GIF (*.gif)|*.gif|")						\
		_T("Image TIFF (*.tiff)|*.tiff|")					\
		_T("|")

#ifdef SC_CANSAVEVECTOR_ASTEXT
#define SC_EMFMODE_FILTERS									\
		_T("Enhanced metafile EMF (*.emf)|*.emf|")			\
		_T("Enhanced metafile EMF+ (*.emf)|*.emf|")			\
		_T("Enhanced metafile EMF+ dual (*.emf)|*.emf|")	\
		_T("Standard metafile (*.wmf)|*.wmf|")				\
		_T("Text file (*.txt)|*.txt|")						\
		_T("Image Windows bitmap (*.bmp)|*.bmp|")			\
		_T("Image JPEG (*.jpg)|*.jpg|")						\
		_T("Image PNG (*.png)|*.png|")						\
		_T("Image GIF (*.gif)|*.gif|")						\
		_T("Image TIFF (*.tiff)|*.tiff|")					\
		_T("|")

#endif

#define SC_BRKMODE_FILTERS									\
		_T("RTF file (*.rtf)|*.rtf|")						\
		_T("Text file (*.txt)|*.txt|")						\
		_T("|")


#ifndef RECT_WIDTH
#define RECT_WIDTH(r) ((r).right - (r).left)
#endif
#ifndef RECT_HEIGHT
#define RECT_HEIGHT(r) ((r).bottom - (r).top)
#endif

LPTSTR SCCopyText(LPCTSTR lpszText);
CString SCFNameExtFromPath(LPCTSTR lpszPath);

HENHMETAFILE SCEMFFromFile(LPCTSTR lpzsFname, UINT uiFileType);
BOOL SCGetFilesInsertList(CStringList& strLFiles, LPCTSTR lpszCurFile, BOOL bImgOnly=FALSE);

int SCFileTypeFromFilterIndexImg(int iIndex);
#ifdef SC_CANSAVEVECTOR_ASTEXT
	int SCFileTypeFromFilterIndex(int iIndex);
#endif

UINT SCFileTypeFromExt(LPCTSTR lpszFname, BOOL bExcludeMeta=FALSE);
void SCAddPathExtFromFilterIndex(CString& strPath, CString& sFilters, int iIndex);
CString SCMakeupDocDir(LPCTSTR lpszUnidocName);
void SCFilterDocDirFileList(CStringList& rFilesList, CString& strDocDir, BOOL bFilterTxt=TRUE);

#endif //_SCEMFDOCUTILS_H_
//  ------------------------------------------------------------
