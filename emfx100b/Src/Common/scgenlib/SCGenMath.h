/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGENMATH_H_
#define _SCGENMATH_H_

#include <math.h>

//	#define SC_PI 3.141592653589793f
#define SC_PI	3.14159265358979323846f

// Neper
#define SC_E	2.7182818284590452354f

// Convert degrees to radians for math functions.
#define SC_RAD(x) ((x) * SC_PI / 180.0f)

// Convert radians to degrees for math functions.
#define SC_DEGREES(x) ((x) * 180.0f / SC_PI)

#endif //_SCGENMATH_H_
//  ------------------------------------------------------------
