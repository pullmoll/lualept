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

#define	LL_PTA		"PTA"           /*!< Lua class: array of points (pair of l_float32) */
#define	LL_PTAA		"PTAA"          /*!< Lua class: array of PTA */
#define	LL_NUMA		"NUMA"          /*!< Lua class: array of floats (l_float32) */
#define	LL_NUMAA	"NUMAA"         /*!< Lua class: array of NUMA */
#define	LL_DNA		"DNA"           /*!< Lua class: array of doubles (l_float64, equiv. to lua_Number) */
#define	LL_DNAA		"DNAA"          /*!< Lua class: array of DNA */
#define	LL_AMAP		"AMAP"          /*!< Lua class: AMAP (key / value pairs) */
#define	LL_ASET		"ASET"          /*!< Lua class: ASET (key set) */
#define	LL_DLLIST	"DLLIST"        /*!< Lua class: DLLIST (doubly linked list) */
#define	LL_BOX		"BOX"           /*!< Lua class: BOX (quad l_int32 for x,y,w,h) */
#define	LL_BOXA		"BOXA"          /*!< Lua class: array of BOX */
#define	LL_BOXAA	"BOXAA"         /*!< Lua class: array of BOXA */
#define	LL_PIXCMAP	"PIXCMAP"       /*!< Lua class: PIXCMAP (color map) */
#define	LL_PIX		"PIX"           /*!< Lua class: PIX (pixels and meta data) */
#define	LL_PIXA		"PIXA"          /*!< Lua class: array of PIX */
#define	LL_PIXAA        "PIXAA"         /*!< Lua class: array of PIXA */

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

typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;

/** Lua function table (luaL_Reg array[]) sentinel */
#define LUA_SENTINEL    {NULL,NULL}

/*! Dummy structure for the top level Lua class LL_LEPT */
struct LuaLept {
    char str_version[32];           /*!< Our own version number */
    char str_version_lua[32];       /*!< Lua's version number */
    char str_version_lept[32];      /*!< Leptonica's version number */
};

/*! Dummy type for the top level Lua class LL_LEPT */
typedef struct LuaLept LUALEPT;

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

/* llnuma.c */
extern NUMA       * ll_check_NUMA(lua_State *L, int arg);
extern int          ll_push_NUMA(lua_State *L, NUMA *na);
extern int          ll_new_NUMA(lua_State *L);
extern int          ll_register_NUMA(lua_State *L);

/* llnumaa.c */
extern NUMAA      * ll_check_NUMAA(lua_State *L, int arg);
extern int          ll_push_NUMAA(lua_State *L, NUMAA *naa);
extern int          ll_new_NUMAA(lua_State *L);
extern int          ll_register_NUMAA(lua_State *L);

/* lldna.c */
extern L_DNA      * ll_check_DNA(lua_State *L, int arg);
extern int          ll_push_DNA(lua_State *L, L_DNA *da);
extern int          ll_new_DNA(lua_State *L);
extern int          ll_register_DNA(lua_State *L);

/* lldnaa.c */
extern L_DNAA     * ll_check_DNAA(lua_State *L, int arg);
extern int          ll_push_DNAA(lua_State *L, L_DNAA *naa);
extern int          ll_new_DNAA(lua_State *L);
extern int          ll_register_DNAA(lua_State *L);

/* llpta.c */
extern PTA        * ll_check_PTA(lua_State *L, int arg);
extern int          ll_push_PTA(lua_State *L, PTA *pta);
extern int          ll_new_PTA(lua_State *L);
extern int          ll_register_PTA(lua_State *L);

/* llptaa.c */
extern PTAA       * ll_check_PTAA(lua_State *L, int arg);
extern int          ll_push_PTAA(lua_State *L, PTAA *ptaa);
extern int          ll_new_PTAA(lua_State *L);
extern int          ll_register_PTAA(lua_State *L);

/* llamap.c */
extern L_AMAP     * ll_check_AMAP(lua_State *L, int arg);
extern int          ll_push_AMAP(lua_State *L, L_AMAP *amap);
extern int          ll_new_AMAP(lua_State *L);
extern int          ll_register_AMAP(lua_State *L);

/* llaset.c */
extern L_ASET     * ll_check_ASET(lua_State *L, int arg);
extern int          ll_push_ASET(lua_State *L, L_ASET *aset);
extern int          ll_new_ASET(lua_State *L);
extern int          ll_register_ASET(lua_State *L);

/* lllist.c */
extern DLLIST     * ll_check_DLLIST(lua_State *L, int arg);
extern int          ll_push_DLLIST(lua_State *L, DLLIST *list);
extern int          ll_new_DLLIST(lua_State *L);
extern int          ll_register_DLLIST(lua_State *L);

/* llbox.c */
extern BOX        * ll_check_BOX(lua_State *L, int arg);
extern int          ll_push_BOX(lua_State *L, BOX *box);
extern int          ll_new_BOX(lua_State *L);
extern int          ll_register_BOX(lua_State *L);

/* llboxa.c */
extern BOXA       * ll_check_BOXA(lua_State *L, int arg);
extern int          ll_push_BOXA(lua_State *L, BOXA *boxa);
extern int          ll_new_BOXA(lua_State *L);
extern int          ll_register_BOXA(lua_State *L);

/* llboxaa.c */
extern BOXAA      * ll_check_BOXAA(lua_State *L, int arg);
extern int          ll_push_BOXAA(lua_State *L, BOXAA *boxaa);
extern int          ll_new_BOXAA(lua_State *L);
extern int          ll_register_BOXAA(lua_State *L);

/* llpixcmap.c */
extern PIXCMAP    * ll_check_PIXCMAP(lua_State *L, int arg);
extern PIXCMAP    * ll_take_PIXCMAP(lua_State *L, int arg);
extern int          ll_push_PIXCMAP(lua_State *L, PIXCMAP *cmap);
extern int          ll_new_PIXCMAP(lua_State *L);
extern int          ll_register_PIXCMAP(lua_State *L);

/* llpix.c */
extern PIX        * ll_check_PIX(lua_State *L, int arg);
extern int          ll_push_PIX(lua_State *L, PIX *pix);
extern int          ll_new_PIX(lua_State *L);
extern int          ll_register_PIX(lua_State *L);

/* llpixa.c */
extern PIXA       * ll_check_PIXA(lua_State *L, int arg);
extern int          ll_push_PIXA(lua_State *L, PIXA *pixa);
extern int          ll_new_PIXA(lua_State *L);
extern int          ll_register_PIXA(lua_State *L);

/* llpixaa.c */
extern PIXAA      * ll_check_PIXAA(lua_State *L, int arg);
extern int          ll_push_PIXAA(lua_State *L, PIXAA *pixaa);
extern int          ll_new_PIXAA(lua_State *L);
extern int          ll_register_PIXAA(lua_State *L);

#endif
