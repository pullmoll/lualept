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
#if !defined(LUALEPT_H)
#define  LUALEPT_H

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include "llenviron.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <allheaders.h>

/** Local type definition of lua_State to avoid including lua.h here */
typedef struct lua_State lua_State;

#define LL_BOOLEAN      "l_ok"          /*!< Lua variable of type boolean */
#define LL_INT8         "l_int8"        /*!< Lua variable of type integer; range l_int8 */
#define LL_UINT8        "l_uint8"       /*!< Lua variable of type integer; range l_uint8 */
#define LL_INT16        "l_int16"       /*!< Lua variable of type integer; range l_int16 */
#define LL_UINT16       "l_uint16"      /*!< Lua variable of type integer; range l_uint16 */
#define LL_INT32        "l_int32"       /*!< Lua variable of type integer; range l_int32 */
#define LL_UINT32       "l_uint32"      /*!< Lua variable of type integer; range l_uint32 */
#define LL_INT64        "l_int64"       /*!< Lua variable of type integer; range l_int64 */
#define LL_UINT64       "l_uint64"      /*!< Lua variable of type integer; range l_uint64 */
#define LL_FLOAT32      "l_int64"       /*!< Lua variable of type number; range l_float32 */
#define LL_FLOAT64      "l_uint64"      /*!< Lua variable of type number; range l_float64 */
#define	LL_AMAP		"Amap"          /*!< Lua class: Amap (key / value pairs) */
#define	LL_ASET		"Aset"          /*!< Lua class: Aset (key set) */
#define	LL_BBUFFER	"Bbuffer"       /*!< Lua class: Bbuffer (byte buffer) */
#define	LL_BMF		"Bmf"           /*!< Lua class: Bmf (Bitmap font) */
#define	LL_BOX		"Box"           /*!< Lua class: Box (quad l_int32 for x,y,w,h) */
#define	LL_BOXA		"Boxa"          /*!< Lua class: Boxa (array of Box) */
#define	LL_BOXAA	"Boxaa"         /*!< Lua class: Boxaa (array of Boxa) */
#define	LL_COMPDATA     "CompData"      /*!< Lua class: CompData */
#define	LL_CCBORD       "CCBord"        /*!< Lua class: CCBord */
#define	LL_CCBORDA      "CCBorda"       /*!< Lua class: CCBorda (array of CCBord) */
#define	LL_DEWARP       "Dewarp"        /*!< Lua class: Dewarp */
#define	LL_DEWARPA      "Dewarpa"       /*!< Lua class: Dewarpa (array of Dewarp) */
#define	LL_DLLIST	"Dllist"        /*!< Lua class: DoubleLinkedList */
#define	LL_DNA		"Dna"           /*!< Lua class: array of doubles (l_float64, equiv. to lua_Number) */
#define	LL_DNAA		"Dnaa"          /*!< Lua class: Dnaa (array of Dna) */
#define LL_DNAHASH      "DnaHash"       /*!< Lua class: DnaHash */
#define	LL_DPIX		"DPix"          /*!< Lua class: DPix */
#define	LL_FPIX		"FPix"          /*!< Lua class: FPix */
#define	LL_FPIXA	"FPixa"         /*!< Lua class: FPixa (array of FPix) */
#define	LL_KERNEL       "Kernel"        /*!< Lua class: Kernel */
#define	LL_NUMA		"Numa"          /*!< Lua class: Numa array of floats (l_float32) */
#define	LL_NUMAA	"Numaa"         /*!< Lua class: Numaa (array of Numa) */
#define	LL_PDFDATA      "PdfData"       /*!< Lua class: PdfData */
#define	LL_PIX		"Pix"           /*!< Lua class: Pix (pixels and meta data) */
#define	LL_PIXA		"Pixa"          /*!< Lua class: Pixa (array of Pix) */
#define	LL_PIXAA        "Pixaa"         /*!< Lua class: Pixaa (array of Pixa) */
#define	LL_PIXCMAP	"PixColormap"   /*!< Lua class: PixColormap (color map) */
#define	LL_PIXTILING	"PixTiling"     /*!< Lua class: PixTiling */
#define	LL_PIXCOMP      "PixComp"       /*!< Lua class: PixComp (compressed Pix) */
#define	LL_PIXACOMP     "PixaComp"      /*!< Lua class: PixaComp (array of PixComp) */
#define	LL_PTA		"Pta"           /*!< Lua class: Pta (array of points, i.e. pair of l_float32) */
#define	LL_PTAA		"Ptaa"          /*!< Lua class: Ptaa (array of Pta) */
#define	LL_RBTNODE      "RbtreeNode"    /*!< Lua class: RbtreeNode (Amap and Aset nodes) */
#define	LL_SARRAY	"Sarray"        /*!< Lua class: Sarray (array of strings) */
#define	LL_SEL		"Sel"           /*!< Lua class: Sel */
#define	LL_SELA		"Sela"          /*!< Lua class: array of Sel */
#define	LL_STACK        "Stack"         /*!< Lua class: Stack */
#define	LL_WSHED        "WShed"         /*!< Lua class: Stack */

typedef struct ll_global_var_s {
    const char *type;       /*!< Type is one of LL_... macros */
    const char *name;       /*!< Variable name when executing the Lua script */
    void *ptr;              /*!< Pointer to the variable */
}   ll_global_var_t;

/** Use this sentinel as the last entry in an array of ll_global_var_t */
#define LL_SENTINEL {NULL, NULL, NULL}

LUALEPT_DLL extern void ll_res_globals(void);
LUALEPT_DLL extern int ll_set_global(const char *type, const char *name, void** data);
LUALEPT_DLL extern int ll_set_globals(const ll_global_var_t *vars);
LUALEPT_DLL extern int ll_get_global(const char *type, const char *name, void **pdata);
LUALEPT_DLL extern int ll_get_globals(const ll_global_var_t *vars);

LUALEPT_DLL extern int ll_open_Amap(lua_State *L);
LUALEPT_DLL extern int ll_open_Aset(lua_State *L);
LUALEPT_DLL extern int ll_open_ByteBuffer(lua_State *L);
LUALEPT_DLL extern int ll_open_Bmf(lua_State *L);
LUALEPT_DLL extern int ll_open_DoubleLinkedList(lua_State *L);
LUALEPT_DLL extern int ll_open_Numa(lua_State *L);
LUALEPT_DLL extern int ll_open_Numaa(lua_State *L);
LUALEPT_DLL extern int ll_open_Dewarp(lua_State *L);
LUALEPT_DLL extern int ll_open_Dewarpa(lua_State *L);
LUALEPT_DLL extern int ll_open_Dna(lua_State *L);
LUALEPT_DLL extern int ll_open_Dnaa(lua_State *L);
LUALEPT_DLL extern int ll_open_DnaHash(lua_State *L);
LUALEPT_DLL extern int ll_open_Pta(lua_State *L);
LUALEPT_DLL extern int ll_open_Ptaa(lua_State *L);
LUALEPT_DLL extern int ll_open_Box(lua_State *L);
LUALEPT_DLL extern int ll_open_Boxa(lua_State *L);
LUALEPT_DLL extern int ll_open_Boxaa(lua_State *L);
LUALEPT_DLL extern int ll_open_CCBord(lua_State *L);
LUALEPT_DLL extern int ll_open_CCBorda(lua_State *L);
LUALEPT_DLL extern int ll_open_PixColormap(lua_State *L);
LUALEPT_DLL extern int ll_open_PixComp(lua_State *L);
LUALEPT_DLL extern int ll_open_PixaComp(lua_State *L);
LUALEPT_DLL extern int ll_open_Pix(lua_State *L);
LUALEPT_DLL extern int ll_open_Pixa(lua_State *L);
LUALEPT_DLL extern int ll_open_Pixaa(lua_State *L);
LUALEPT_DLL extern int ll_open_FPix(lua_State *L);
LUALEPT_DLL extern int ll_open_FPixa(lua_State *L);
LUALEPT_DLL extern int ll_open_DPix(lua_State *L);
LUALEPT_DLL extern int ll_open_PixTiling(lua_State *L);
LUALEPT_DLL extern int ll_open_Sel(lua_State *L);
LUALEPT_DLL extern int ll_open_Sela(lua_State *L);
LUALEPT_DLL extern int ll_open_Kernel(lua_State *L);
LUALEPT_DLL extern int ll_open_CompData(lua_State *L);
LUALEPT_DLL extern int ll_open_PdfData(lua_State *L);
LUALEPT_DLL extern int ll_open_Sarray(lua_State *L);
LUALEPT_DLL extern int ll_open_Stack(lua_State *L);
LUALEPT_DLL extern int ll_open_WShed(lua_State *L);

LUALEPT_DLL extern int luaopen_lualept(lua_State *L);
LUALEPT_DLL extern int ll_RunScript(const char* script);

#if defined(_MSC_VER)
LEPT_DLL extern int LeptMsgSeverity;
#endif

#ifdef __cplusplus
}
#endif

#endif	/* !defined(LUALEPT_H) */
