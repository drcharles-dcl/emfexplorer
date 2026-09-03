/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGENTYPES_H_
#define _SCGENTYPES_H_


#ifdef _WIN32
// Match WIN32 platform
#define SCgenBOOL			BOOL
#define SCgenBYTE			BYTE
#define SCgenTCHAR			TCHAR
#define SCgenWORD			WORD
#define SCgenSHORT			SHORT
#define SCgenLONG			LONG
#define SCgenINT			INT
#define SCgenFLOAT			FLOAT
#define SCgenDOUBLE			double

#define SCgenSIZE_T			size_t

#define SCgenUCHAR			UCHAR
#define SCgenUWORD			UWORD
#define SCgenUSHORT			USHORT
#define SCgenULONG			ULONG
#define SCgenUINT			UINT

#define SCgenTRUE			TRUE
#define SCgenFALSE			FALSE

#else
// Default to standard C++
typedef	bool				SCgenBOOL;
typedef	unsigned char		SCgenBYTE;

#ifdef _UNICODE
	typedef	wchar_t			SCgenTCHAR;
#else
	typedef	char			SCgenTCHAR;
#endif

typedef	short				SCgenWORD;
typedef	long int			SCgenLONG;
typedef long int			SCgenINT;
typedef	float				SCgenFLOAT;
typedef	double				SCgenDOUBLE;

typedef	size_t              SCgenSIZE_T;

typedef	unsigned char		SCgenUCHAR;
typedef	unsigned short		SCgenUWORD;
typedef	unsigned short		SCgenUSHORT;
typedef	unsigned long int	SCgenULONG;
typedef	unsigned long int	SCgenUINT;

#define SCgenTRUE			true
#define SCgenFALSE			false

#endif

#endif //_SCGENTYPES_H_
//  ------------------------------------------------------------
