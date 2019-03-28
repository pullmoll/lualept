/************************************************************************
 * Copyright (c) Jürgen Buchmüller <pullmoll@t-online.de>
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


typedef L_AMAP              Amap;           /*!< Local type name for L_AMAP */
typedef L_AMAP_NODE         AmapNode;       /*!< Local type name for L_AMAP_NODE */
typedef L_ASET              Aset;           /*!< Local type name for L_ASET */
typedef L_ASET_NODE         AsetNode;       /*!< Local type name for L_ASET_NODE */
typedef L_BBUFFER           ByteBuffer;     /*!< Local type name for L_BBUFFER */
typedef L_BMF               Bmf;            /*!< Local type name for L_BMF */
typedef L_BYTEA             Bytea;          /*!< Local type name for L_BYTEA */
typedef L_DEWARP            Dewarp;         /*!< Local type name for L_DEWAP */
typedef L_DEWARPA           Dewarpa;        /*!< Local type name for L_DEWARPA */
typedef DoubleLinkedList    DLList;         /*!< Local type name for L_DLLIST */
typedef L_DNA               Dna;            /*!< Local type name for L_DNA */
typedef L_DNAA              Dnaa;           /*!< Local type name for L_DNAA */
typedef L_DNAHASH           DnaHash;        /*!< Local type name for L_DNAHASH */
typedef L_KERNEL            Kernel;         /*!< Local type name for L_KERNEL */
typedef L_COMP_DATA         CompData;       /*!< Local type name for L_COMP_DATA */
typedef L_PDF_DATA          PdfData;        /*!< Local type name for L_PDF_DATA */
typedef L_QUEUE             Queue;          /*!< Local type name for L_QUEUE */
typedef L_RBTREE	    Rbtree;         /*!< Local type name for L_RBTREE */
typedef L_RBTREE_NODE       RbtreeNode;     /*!< Local type name for L_RBTREE_NODE */
typedef L_STACK             Stack;          /*!< Local type name for L_STACK */
typedef void*               Timer;          /*!< Local type name for L_TIMER */
typedef L_WALLTIMER         WallTimer;      /*!< Local type name for L_WALLTIMER */
typedef L_WSHED             WShed;          /*!< Local type name for L_WSHED */

/**
 * Enumeration of types supported in the set / get global variable functions
 */
typedef enum {
    ll_invalid,
    ll_boolean,
    ll_int8,
    ll_uint8,
    ll_int16,
    ll_uint16,
    ll_int32,
    ll_uint32,
    ll_int64,
    ll_uint64,
    ll_float32,
    ll_float64,
    ll_pchars,
    ll_pbytes,
    ll_amap,
    ll_aset,
    ll_bbuffer,
    ll_bmf,
    ll_bytea,
    ll_box,
    ll_boxa,
    ll_boxaa,
    ll_compdata,
    ll_ccbord,
    ll_ccborda,
    ll_dewarp,
    ll_dewarpa,
    ll_dllist,
    ll_dna,
    ll_dnaa,
    ll_dnahash,
    ll_dpix,
    ll_fpix,
    ll_fpixa,
    ll_kernel,
    ll_numa,
    ll_numaa,
    ll_pdfdata,
    ll_pix,
    ll_pixa,
    ll_pixaa,
    ll_pixacc,
    ll_pixcmap,
    ll_pixtiling,
    ll_pixcomp,
    ll_pixacomp,
    ll_pta,
    ll_ptaa,
    ll_queue,
    ll_rbtnode,
    ll_rbtree,
    ll_sarray,
    ll_sel,
    ll_sela,
    ll_stack,
    ll_wshed
}   ll_type_e;

/**
 * The ll_bytes_s structure is used to point to an array of
 * unsigned char (bytes) and defining its size.
 * This indirection type is used for "byte strings", i.e.
 * strings which may contain the '\0' character.
 */
typedef struct ll_bytes_s {
    l_uint8 *data;
    size_t   size;
}   ll_bytes_t;

/**
 * The structure ll_global_var_s is used to define global
 * variables to set before and get after the script is run.
 */
typedef struct ll_global_var_s {
    ll_type_e   type;       /*!< Type is one value from the ll_type_e enumeration */
    const char *name;       /*!< Variable name when executing the Lua script */

    /*! union of pointers to all supported types */
    union {
        void         *pptr;                 /*!< generic pointer to variable */
        bool         *pb;                   /*!< pointer to a bool */
        l_int8       *pi8;                  /*!< pointer to a l_int8 */
        l_uint8      *pu8;                  /*!< pointer to a l_uint8 */
        l_int16      *pi16;                 /*!< pointer to a l_int16 */
        l_uint16     *pu16;                 /*!< pointer to a l_uint16 */
        l_int32      *pi32;                 /*!< pointer to a l_int32 */
        l_uint32     *pu32;                 /*!< pointer to a l_uint32 */
        l_int64      *pi64;                 /*!< pointer to a l_int64 */
        l_uint64     *pu64;                 /*!< pointer to a l_uint64 */
        l_float32    *pf32;                 /*!< pointer to a l_float32 */
        l_float64    *pf64;                 /*!< pointer to a l_float64 */
        char        **pchars;               /*!< pointer to a char array */
        ll_bytes_t   *pbytes;               /*!< pointer to a ll_bytes_t (pointer to data and size) */
        Amap        **pamap;                /*!< pointer to a Amap */
        Aset        **paset;                /*!< pointer to a Aset */
        Bmf         **pbmf;                 /*!< pointer to a Bmf */
        Box         **pbox;                 /*!< pointer to a Box */
        Boxa        **pboxa;                /*!< pointer to a Boxa */
        Boxaa       **pboxaa;               /*!< pointer to a Boxaa */
        ByteBuffer  **pbb;                  /*!< pointer to a ByteBuffer */
        Bytea       **pbytea;               /*!< pointer to a Bytea */
        CompData    **pcid;                 /*!< pointer to a CompData */
        CCBord      **pccb;                 /*!< pointer to a CCBord */
        CCBorda     **pccba;                /*!< pointer to a CCBorda */
        Dewarp      **pdew;                 /*!< pointer to a Dewarp */
        Dewarpa     **pdewa;                /*!< pointer to a Dewarpa */
        DLList      **plist;                /*!< pointer to a DLList */
        Dna         **pda;                  /*!< pointer to a Dna */
        Dnaa        **pdaa;                 /*!< pointer to a Dnaa */
        DnaHash     **pdah;                 /*!< pointer to a DnaHash */
        DPix        **pdpix;                /*!< pointer to a DPix */
        FPix        **pfpix;                /*!< pointer to a FPix */
        FPixa       **pfpixa;               /*!< pointer to a FPixa */
        Kernel      **pkel;                 /*!< pointer to a Kernel */
        Numa        **pna;                  /*!< pointer to a Numa */
        Numaa       **pnaa;                 /*!< pointer to a Numaa */
        PdfData     **ppdd;                 /*!< pointer to a PdfData */
        Pix         **ppix;                 /*!< pointer to a Pix */
        Pixa        **ppixa;                /*!< pointer to a Pixa */
        Pixaa       **ppixaa;               /*!< pointer to a Pixaa */
        Pixacc      **ppixacc;              /*!< pointer to a Pixacc */
        PixColormap **pcmap;                /*!< pointer to a PixColormap */
        PixTiling   **ppixt;                /*!< pointer to a PixTiling */
        PixComp     **ppixc;                /*!< pointer to a PixComp */
        PixaComp    **ppixac;               /*!< pointer to a PixaComp */
        Pta         **ppta;                 /*!< pointer to a Pta */
        Ptaa        **pptaa;                /*!< pointer to a Ptaa */
        Queue       **pqueue;               /*!< pointer to a Queue */
        Rbtree      **ptree;                /*!< pointer to a Rbtree (head) */
        RbtreeNode  **pnode;                /*!< pointer to a RbtreeNode */
        Sarray      **psa;                  /*!< pointer to a Sarray */
        Sel         **psel;                 /*!< pointer to a Sel */
        Sela        **psela;                /*!< pointer to a Sela */
        Stack       **pstack;               /*!< pointer to a Stack */
        WShed       **pwshed;               /*!< pointer to a WShed */
    }   u;
}   ll_global_var_t;

/** Use this macro to initialize one entry in a ll_global_var_t array */
#define LL_GLOBAL(type, name, ptr) {type, name, {ptr}}

/** Use this sentinel as the last entry in an array of ll_global_var_t */
#define LL_SENTINEL {ll_invalid, nullptr, {nullptr}}

LUALEPT_DLL extern int ll_open_Amap(lua_State *L);
LUALEPT_DLL extern int ll_open_Aset(lua_State *L);
LUALEPT_DLL extern int ll_open_Bmf(lua_State *L);
LUALEPT_DLL extern int ll_open_ByteBuffer(lua_State *L);
LUALEPT_DLL extern int ll_open_Bytea(lua_State *L);
LUALEPT_DLL extern int ll_open_DLList(lua_State *L);
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
LUALEPT_DLL extern int ll_open_Pixacc(lua_State *L);
LUALEPT_DLL extern int ll_open_FPix(lua_State *L);
LUALEPT_DLL extern int ll_open_FPixa(lua_State *L);
LUALEPT_DLL extern int ll_open_DPix(lua_State *L);
LUALEPT_DLL extern int ll_open_PixTiling(lua_State *L);
LUALEPT_DLL extern int ll_open_Sel(lua_State *L);
LUALEPT_DLL extern int ll_open_Sela(lua_State *L);
LUALEPT_DLL extern int ll_open_Kernel(lua_State *L);
LUALEPT_DLL extern int ll_open_CompData(lua_State *L);
LUALEPT_DLL extern int ll_open_PdfData(lua_State *L);
LUALEPT_DLL extern int ll_open_Queue(lua_State *L);
LUALEPT_DLL extern int ll_open_Sarray(lua_State *L);
LUALEPT_DLL extern int ll_open_Stack(lua_State *L);
LUALEPT_DLL extern int ll_open_WShed(lua_State *L);

LUALEPT_DLL extern int ll_set_globals(lua_State *L, const ll_global_var_t *vars);
LUALEPT_DLL extern int ll_get_globals(lua_State *L, const ll_global_var_t *vars);
LUALEPT_DLL extern int luaopen_lualept(lua_State *L);
LUALEPT_DLL extern lua_State* ll_open(bool debug);
LUALEPT_DLL extern int ll_set_arg(lua_State *L, int argc, char **argv);
LUALEPT_DLL extern int ll_run(lua_State *L, const char* filename, const char* script = nullptr);
LUALEPT_DLL extern int ll_close(lua_State *L);

#if defined(_MSC_VER)
LEPT_DLL extern int LeptMsgSeverity;
#endif

#ifdef __cplusplus
}
#endif

#endif	/* !defined(LUALEPT_H) */
