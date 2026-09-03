/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGENDEFS_H_
#define _SCGENDEFS_H_

// This macro is used to reduce size requirements of some classes
#ifndef SMC_ALWAYS_VTABLE
	#ifndef SMC_NOVTABLE
		#if _MSC_VER >= 1100 && !defined(_DEBUG)
			#define SMC_NOVTABLE __declspec(novtable)
		#else
			#define SMC_NOVTABLE
		#endif
	#endif
#else
			#define SMC_NOVTABLE
#endif

// This macro is used to declare interface classes
#define SMC_INTERFACE SMC_NOVTABLE

// Delete an object and NULL out the pointer so that an error is generated
// if it is reused without reallocation
#define SMC_SAFEDELETE(p)	\
	if (p)					\
	{						\
		delete p;			\
		p = NULL;			\
	}

#define SMC_SAFEARRAY_DELETE(p)	\
	if (p)						\
	{							\
		delete [] p;			\
		p = NULL;				\
	}

// Delete with test (typically to call in destructors)
#define SMC_DELETE(p)		\
	if (p)					\
		delete p

#define SMC_ARRAY_DELETE(p)		\
	if (p)						\
		delete [] p

// Swap anything having an assignment or copy operator
template <class T>
inline void SCSwap(T& rA, T& rB)
{
	T tTmp = rA; rA = rB; rB = tTmp;
}

// Swap ints (presumably)
#define SMC_ISWAP(x, y) SCSwap(x, y)

// Constrain l-value to range
#define SC_LIMIT_VALUE(v_lvalue, vMin, vMax)	\
	if (v_lvalue>(vMax))						\
		v_lvalue = (vMax);						\
	else										\
	if (v_lvalue<(vMin))						\
		v_lvalue = (vMin)

#endif //_SCGENDEFS_H_
//  ------------------------------------------------------------
