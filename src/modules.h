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
#if !defined(LUALEPT_EXPORTS_H)
#define LUALEPT_EXPORTS_H

#include "lualept.h"

#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif
#if defined(HAVE_FLOAT_H)
#include <float.h>
#endif
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#if defined(HAVE_MEMORY_H)
#include <memory.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#define LLUA_DEBUG      0

#define	LL_PTA		"Pta"           /*!< Lua class: array of points (pair of l_float32) */
#define	LL_PTAA		"Ptaa"          /*!< Lua class: array of Pta */
#define	LL_NUMA		"Numa"          /*!< Lua class: array of floats (l_float32) */
#define	LL_NUMAA	"Numaa"         /*!< Lua class: array of Numa */
#define	LL_DNA		"Dna"           /*!< Lua class: array of doubles (l_float64, equiv. to lua_Number) */
#define	LL_DNAA		"Dnaa"          /*!< Lua class: array of Dna */
#define	LL_AMAP		"Amap"          /*!< Lua class: Amap (key / value pairs) */
#define	LL_ASET		"Aset"          /*!< Lua class: Aset (key set) */
#define	LL_DLLIST	"Dllist"        /*!< Lua class: DoubleLinkedList */
#define	LL_BOX		"Box"           /*!< Lua class: Box (quad l_int32 for x,y,w,h) */
#define	LL_BOXA		"Boxa"          /*!< Lua class: array of Box */
#define	LL_BOXAA	"Boxaa"         /*!< Lua class: array of Boxa */
#define	LL_PIXCMAP	"PixColormap"   /*!< Lua class: PixColormap (color map) */
#define	LL_PIX		"Pix"           /*!< Lua class: Pix (pixels and meta data) */
#define	LL_PIXA		"Pixa"          /*!< Lua class: array of Pix */
#define	LL_PIXAA        "Pixaa"         /*!< Lua class: array of Pixa */

#define	LL_LEPT		"LuaLept"       /*!< Lua class: LuaLept top level */

#if LLUA_DEBUG
#define DBG(enable, fmt, ...)                                                  \
    do {                                                                       \
        if (enable) {                                                          \
            fprintf(stderr, fmt, __VA_ARGS__);                                 \
        }                                                                      \
    } while (0)
#else
#define DBG(enable, fmt, ...)
#endif

#define LOG_REGISTER    1
#define LOG_CREATE      1
#define LOG_DESTROY     1

#ifdef __cplusplus
extern "C" {
#else
#if !defined(nullptr)
#define nullptr 0
#endif
#endif

#include <lauxlib.h>
#include <lualib.h>

/** Lua function table (luaL_Reg array[]) sentinel */
#define LUA_SENTINEL    {nullptr,nullptr}

/*! Dummy structure for the top level Lua class LL_LEPT */
typedef struct LuaLept {
    char str_version[32];           /*!< Our own version number */
    char str_version_lua[32];       /*!< Lua's version number */
    char str_version_lept[32];      /*!< Leptonica's version number */
} LuaLept;

/** Structure to list keys (string) with their l_int32 (enumeration) values */
typedef struct lept_enums_s {
    const char *key;		/*!< string for the enumeration value */
    const char *name;		/*!< name of the enum value in Leptonica's alltypes.h */
    l_int32     value;		/*!< integer enumeration value */
}   lept_enums_t;

/* llept.c */
extern int          ll_register_class(lua_State *L, const char *name, const luaL_Reg *methods, const luaL_Reg *functions);
extern void **      ll_check_udata(lua_State *L, int arg, const char* name);
extern int          ll_push_udata(lua_State *L, const char* name, void *udata);
extern l_int32      ll_check_index(const char* func, lua_State *L, int arg, l_int32 imax);
extern l_int32      ll_check_l_int32(const char* func, lua_State *L, int arg);
extern l_int32      ll_check_l_int32_default(const char* func, lua_State *L, int arg, l_int32 dflt);
extern l_uint32     ll_check_l_uint32(const char* func, lua_State *L, int arg);
extern l_uint32     ll_check_l_uint32_default(const char* func, lua_State *L, int arg, l_uint32 dflt);
extern l_float32    ll_check_l_float32(const char* func, lua_State *L, int arg);
extern l_float32    ll_check_l_float32_default(const char* func, lua_State *L, int arg, l_float32 dflt);
extern l_int32      ll_check_tbl(lua_State *L, int arg, l_int32 dflt, const lept_enums_t *tbl, size_t len);
extern l_int32      ll_check_access_storage(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_access_storage(int flag);
extern l_int32      ll_check_input_format(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_input_format(int format);
extern l_int32      ll_check_keytype(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_keytype(l_int32 type);
extern l_int32      ll_check_consecutive_skip_by(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_consecutive_skip_by(l_int32 choice);
extern l_int32      ll_check_component(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_component(l_int32 component);
extern l_int32      ll_check_choose_min_max(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_choose_min_max(l_int32 choice);
extern l_int32      ll_check_what_is_max(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_what_is_max(l_int32 choice);
extern l_int32      ll_check_getval(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_getval(l_int32 choice);
extern l_int32      ll_check_direction(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_direction(l_int32 dir);
extern l_int32      ll_check_blackwhite(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_blackwhite(l_int32 which);
extern l_int32      ll_check_rasterop(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_rasterop(l_int32 op);
extern l_int32      ll_check_searchdir(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_searchir(l_int32 dir);
extern l_int32      ll_check_stats_type(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_stats_type(l_int32 dir);
extern l_int32      ll_check_select_color(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_select_color(l_int32 color);
extern l_int32      ll_check_select_min_max(lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_select_min_max(l_int32 which);

/* llnuma.cpp */
extern Numa       * ll_check_Numa(lua_State *L, int arg);
extern int          ll_push_Numa(lua_State *L, Numa *na);
extern int          ll_new_Numa(lua_State *L);
extern int          ll_register_Numa(lua_State *L);

/* llnumaa.cpp */
extern Numaa      * ll_check_Numaa(lua_State *L, int arg);
extern int          ll_push_Numaa(lua_State *L, Numaa *naa);
extern int          ll_new_Numaa(lua_State *L);
extern int          ll_register_Numaa(lua_State *L);

/* lldna.cpp */
extern L_Dna      * ll_check_Dna(lua_State *L, int arg);
extern int          ll_push_Dna(lua_State *L, L_Dna *da);
extern int          ll_new_Dna(lua_State *L);
extern int          ll_register_Dna(lua_State *L);

/* lldnaa.cpp */
extern L_Dnaa     * ll_check_Dnaa(lua_State *L, int arg);
extern int          ll_push_Dnaa(lua_State *L, L_Dnaa *naa);
extern int          ll_new_Dnaa(lua_State *L);
extern int          ll_register_Dnaa(lua_State *L);

/* llpta.cpp */
extern Pta        * ll_check_Pta(lua_State *L, int arg);
extern int          ll_push_Pta(lua_State *L, Pta *pta);
extern int          ll_new_Pta(lua_State *L);
extern int          ll_register_Pta(lua_State *L);

/* llptaa.cpp */
extern Ptaa       * ll_check_Ptaa(lua_State *L, int arg);
extern int          ll_push_Ptaa(lua_State *L, Ptaa *ptaa);
extern int          ll_new_Ptaa(lua_State *L);
extern int          ll_register_Ptaa(lua_State *L);

/* llamap.cpp */
extern L_AMAP     * ll_check_Amap(lua_State *L, int arg);
extern int          ll_push_Amap(lua_State *L, L_AMAP *amap);
extern int          ll_new_Amap(lua_State *L);
extern int          ll_register_Amap(lua_State *L);

/* llaset.cpp */
extern L_ASET     * ll_check_Aset(lua_State *L, int arg);
extern int          ll_push_Aset(lua_State *L, L_ASET *aset);
extern int          ll_new_Aset(lua_State *L);
extern int          ll_register_Aset(lua_State *L);

/* lllist.cpp */
extern DoubleLinkedList* ll_check_DoubleLinkedList(lua_State *L, int arg);
extern int          ll_push_DoubleLinkedList(lua_State *L, DoubleLinkedList *list);
extern int          ll_new_DoubleLinkedList(lua_State *L);
extern int          ll_register_DoubleLinkedList(lua_State *L);

/* llbox.cpp */
extern Box        * ll_check_Box(lua_State *L, int arg);
extern int          ll_push_Box(lua_State *L, Box *box);
extern int          ll_new_Box(lua_State *L);
extern int          ll_register_Box(lua_State *L);

/* llboxa.cpp */
extern Boxa       * ll_check_Boxa(lua_State *L, int arg);
extern int          ll_push_Boxa(lua_State *L, Boxa *boxa);
extern int          ll_new_Boxa(lua_State *L);
extern int          ll_register_BOXA(lua_State *L);

/* llboxaa.cpp */
extern Boxaa      * ll_check_Boxaa(lua_State *L, int arg);
extern int          ll_push_Boxaa(lua_State *L, Boxaa *boxaa);
extern int          ll_new_Boxaa(lua_State *L);
extern int          ll_register_Boxaa(lua_State *L);

/* llpixcmap.cpp */
extern PixColormap* ll_check_PixColormap(lua_State *L, int arg);
extern PixColormap* ll_take_PixColormap(lua_State *L, int arg);
extern int          ll_push_PixColormap(lua_State *L, PixColormap *cmap);
extern int          ll_new_PixColormap(lua_State *L);
extern int          ll_register_PixColormap(lua_State *L);

/* llpix.cpp */
extern Pix        * ll_check_Pix(lua_State *L, int arg);
extern int          ll_push_Pix(lua_State *L, Pix *pix);
extern int          ll_new_Pix(lua_State *L);
extern int          ll_register_Pix(lua_State *L);

/* llpixa.cpp */
extern Pixa       * ll_check_Pixa(lua_State *L, int arg);
extern int          ll_push_Pixa(lua_State *L, Pixa *pixa);
extern int          ll_new_Pixa(lua_State *L);
extern int          ll_register_Pixa(lua_State *L);

/* llpixaa.cpp */
extern Pixaa      * ll_check_Pixaa(lua_State *L, int arg);
extern int          ll_push_Pixaa(lua_State *L, Pixaa *pixaa);
extern int          ll_new_Pixaa(lua_State *L);
extern int          ll_register_Pixaa(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif /* !defined(LUALEPT_EXPORTS_H) */
