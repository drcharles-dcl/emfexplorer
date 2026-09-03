/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCWINTYPES_H_
#define _SCWINTYPES_H_

// Some rect macros
#ifndef RECT_WIDTH
	#define RECT_WIDTH(Rect)		((Rect).right - (Rect).left)
	#define RECT_HEIGHT(Rect)		((Rect).bottom - (Rect).top)
#endif

#ifndef PRECT_WIDTH
	#define PRECT_WIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
	#define PRECT_HEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)
#endif

// Some pointers to const
#ifndef _LPCBYTE_DEFINED
	#define _LPCBYTE_DEFINED
	typedef CONST BYTE * LPCBYTE;
#endif

#ifndef _LPCINT_DEFINED
	#define _LPCINT_DEFINED
	typedef CONST INT * LPCINT;
#endif

#ifndef _LPCPOINT_DEFINED
	#define _LPCPOINT_DEFINED
	typedef CONST POINT * LPCPOINT;
#endif

#ifndef _LPCPOINTS_DEFINED
	#define _LPCPOINTS_DEFINED
	typedef CONST POINTS * LPCPOINTS;
#endif

#ifndef _LPCPOINTL_DEFINED
	#define _LPCPOINTL_DEFINED
	typedef CONST POINTL * LPCPOINTL;
#endif

#ifndef _LPCBITMAPINFO_DEFINED
	#define _LPCBITMAPINFO_DEFINED
	typedef CONST BITMAPINFO * LPCBITMAPINFO;
#endif

// Yet another rect class
class SCWHRect
{
public:
	LONG nX;
    LONG nY;
    LONG nWidth;
    LONG nHeight;

	SCWHRect() {}
	SCWHRect(LONG x, LONG y, LONG w, LONG h) { nX = x; nY = y; nWidth = w; nHeight = h; }
	SCWHRect(const RECT rc)
	{
		nX = rc.left; nY = rc.top;
		nWidth = RECT_WIDTH(rc); nHeight = RECT_HEIGHT(rc);
	}
	SCWHRect(LPCRECT lprc)
	{
		ASSERT(lprc);
		nX = lprc->left; nY = lprc->top;
		nWidth = PRECT_WIDTH(lprc); nHeight = PRECT_HEIGHT(lprc);
	}

	~SCWHRect() {}
};

typedef SCWHRect *PSCWHRect;
typedef PSCWHRect LPSCWHRect;
typedef const LPSCWHRect LPCSCWHRect;

#endif //_SCWINTYPES_H_
//  ------------------------------------------------------------
