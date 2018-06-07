/************************************************************************
 * Copyright 2018 Jürgen Buchmüller <pullmoll@t-online.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#pragma once
#if !defined(LUALEPT_LLENVIRON_H)
#define LUALEPT_LLENVIRON_H

/* MS VC++ does not provide stdint.h, so define the missing types here */
#ifdef _MSC_VER
#include "winconf.h"

/* Note that _WIN32 is defined for both 32 and 64 bit applications,
   whereas _WIN64 is defined only for the latter */

#ifdef _WIN64
typedef __int64 intptr_t;
typedef unsigned __int64 uintptr_t;
#else
typedef int intptr_t;
typedef unsigned int uintptr_t;
#endif

/* VC++6 doesn't seem to have powf, expf. */
#if (_MSC_VER < 1400)
#define powf(x, y) (float)pow((double)(x), (double)(y))
#define expf(x) (float)exp((double)(x))
#endif
#endif /* _MSC_VER */

/* Windows specifics */
#ifdef _WIN32
  /* DLL EXPORTS and IMPORTS */
  #if defined(LUALEPT_LIB)
    #define LUALEPT_DLL __declspec(dllexport)
  #elif defined(LUALEPT_LIB)
    #define LUALEPT_DLL __declspec(dllimport)
  #else
    #define LUALEPT_DLL
  #endif
#else  /* non-Windows specifics */
  #include <stdint.h>
  #define LUALEPT_DLL
#endif  /* _WIN32 */

#endif	/* !defined(LUALEPT_LLENVIRON_H) */
