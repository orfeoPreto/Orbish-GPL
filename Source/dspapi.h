/** Blue Cat PlugNScript dsp api header file.
 *  Contains all definitions required for DSP scripting in C or C++.
 *  Copyright (c) 2015-2017 Blue Cat Audio. All rights reserved.
 */

#ifndef _dspapi_h
#define _dspapi_h

// handling "bool" definition for C compilers
#ifndef __cplusplus
#if(!defined(_MSC_VER) || _MSC_VER >= 1900)
#include <stdbool.h> // using the C99 definition of bool
#else
#define bool char // for binary compatibility with C++
#define true 1
#define false 0
#endif
#endif

// Visual Studio does not support the inline keyword in C before VS2015
#if(!defined(_MSC_VER) || _MSCVER>=1900 || defined(__cplusplus))
#define INLINE_API inline
#else
#define INLINE_API
#endif

// basic types definitions for angelscript compatibility-----------------
typedef char const*     string;
typedef unsigned int    uint;
typedef signed char     int8;
typedef unsigned char   uint8;
#ifdef _MSC_VER // Visual Studio
typedef _int64          int64;
typedef unsigned _int64 uint64;
#else // GCC compatible compiler assumed
#include <stdint.h>
typedef int64_t         int64;
typedef uint64_t        uint64;
#endif
#endif


