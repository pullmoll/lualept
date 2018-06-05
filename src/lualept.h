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
#include <allheaders.h>

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include "environ.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Local type definition of lua_State to avoid including lua.h here */
typedef struct lua_State lua_State;

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

#ifdef __cplusplus
}
#endif

#endif	/* !defined(LUALEPT_H) */
