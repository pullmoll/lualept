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

#define LLUA_DEBUG      0               /*!< set to 1 to enable debugging */
#define LOG_REGISTER    1               /*!< set to 1 to log Lua class registration */
#define LOG_DESTROY     1               /*!< set to 1 to log object destruction */
#define LOG_PUSH_UDATA  1               /*!< set to 1 to log pushing user data */
#define LOG_PUSH_TABLE  1               /*!< set to 1 to log pushing tables */

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
#if defined(HAVE_LIMITS_H)
#include <limits.h>
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

#define	LL_PTA		"Pta"           /*!< Lua class: array of points (pair of l_float32) */
#define	LL_PTAA		"Ptaa"          /*!< Lua class: array of Pta */
#define	LL_NUMA		"Numa"          /*!< Lua class: array of floats (l_float32) */
#define	LL_NUMAA	"Numaa"         /*!< Lua class: array of Numa */
#define	LL_DEWARP       "Dewarp"        /*!< Lua class: Dewarp */
#define	LL_DEWARPA      "Dewarpa"       /*!< Lua class: array of Dewarp */
#define	LL_DNA		"Dna"           /*!< Lua class: array of doubles (l_float64, equiv. to lua_Number) */
#define	LL_DNAA		"Dnaa"          /*!< Lua class: array of Dna */
#define	LL_RBTNODE      "RbtreeNode"    /*!< Lua class: RbtreeNode (Amap and Aset nodes) */
#define	LL_AMAP		"Amap"          /*!< Lua class: Amap (key / value pairs) */
#define	LL_ASET		"Aset"          /*!< Lua class: Aset (key set) */
#define	LL_BMF		"Bmf"           /*!< Lua class: Bmf (Bitmap font) */
#define	LL_DLLIST	"Dllist"        /*!< Lua class: DoubleLinkedList */
#define	LL_BOX		"Box"           /*!< Lua class: Box (quad l_int32 for x,y,w,h) */
#define	LL_BOXA		"Boxa"          /*!< Lua class: array of Box */
#define	LL_BOXAA	"Boxaa"         /*!< Lua class: array of Boxa */
#define	LL_PIXCMAP	"PixColormap"   /*!< Lua class: PixColormap (color map) */
#define	LL_PIX		"Pix"           /*!< Lua class: Pix (pixels and meta data) */
#define	LL_PIXA		"Pixa"          /*!< Lua class: array of Pix */
#define	LL_PIXAA        "Pixaa"         /*!< Lua class: array of Pixa */

#define	LL_LEPT		"LuaLept"       /*!< Lua class: LuaLept top level */

#if defined(LLUA_DEBUG) && (LLUA_DEBUG > 0)
extern void dbg(int enable, const char* format, ...);
#define DBG(enable, format, ...) dbg(enable, format, __VA_ARGS__)
#else
#define DBG(enable, format, ...)
#endif

typedef struct lua_State lua_State;
extern void die(const char *_fun, lua_State* L, const char *format, ...);
extern void **ll_udata(const char *_fun, lua_State* L, int arg, const char *tname);

/**
 * \brief Cast the result of LEPT_MALLOC() to the given type.
 * \param T typename of the result pointer
 * \param size size of the memory block
 * \result memory allocated and cast to T*
 */
template <typename T> T*
ll_malloc(const char* _fun, lua_State *L, size_t size)
{
    void *ptr = LEPT_MALLOC(size);
    if (!ptr) {
        die(_fun, L, "failed to malloc(%d)", size);
        return nullptr;
    }
    return reinterpret_cast<T *>(ptr);
}

/**
 * \brief Cast the result of LEPT_CALLOC() to the given type.
 * \param T typename of the result pointer
 * \param nmemb number of members
 * \param size size of one member
 * \result memory allocated, zeroed and cast to T*
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, size_t nmemb, size_t size)
{
    void *ptr = LEPT_CALLOC(nmemb, size);
    if (!ptr) {
        die(_fun, L, "failed to calloc(%d,%d)", nmemb, size);
        return nullptr;
    }
    return reinterpret_cast<T *>(ptr);
}

/**
 * \brief Alias with l_int32 nmemb and size.
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, l_int32 nmemb, l_int32 size)
{
    return ll_calloc<T>(_fun, L, static_cast<size_t>(nmemb), static_cast<size_t>(size));
}

/**
 * \brief Cast the result of LEPT_CALLOC() to the given type.
 * \param T typename of the result pointer
 * \param nmemb number of members
 * \result memory allocated, zeroed and cast to T*
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, size_t nmemb)
{
    void *ptr = LEPT_CALLOC(nmemb, sizeof(T));
    if (!ptr) {
        die(_fun, L, "failed to calloc(%d,%d)", nmemb, sizeof(T));
        return nullptr;
    }
    return reinterpret_cast<T *>(ptr);
}

/**
 * \brief Alias ll_calloc() with l_int32 nmemb.
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, l_int32 nmemb)
{
    return ll_calloc<T>(_fun, L, static_cast<size_t>(nmemb), sizeof(T));
}

/**
 * \brief Check Lua stack at index %arg for udata with %name.
 * \param T typename of the expected return value
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg argument index
 * \param tname tname of the expected udata
 * \return pointer to the udata
 */
template<typename T> T **
ll_check_udata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    return reinterpret_cast<T **>(ll_udata(_fun, L, arg, tname));
}

#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>
#include <lualib.h>

/** Lua function table (luaL_Reg array[]) sentinel */
#define LUA_SENTINEL    {nullptr,nullptr}

#define FUNC(name) static const char _fun[] = name; (void)_fun

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
extern void         ll_free(void *ptr);
extern int          ll_register_class(lua_State *L, const char *name, const luaL_Reg *methods, const luaL_Reg *functions);

extern int          ll_push_udata(const char *_fun, lua_State *L, const char* name, void *udata);
extern int          ll_push_nil(lua_State *L);
extern int          ll_push_bool(lua_State *L, bool b);
extern int          ll_push_iarray(lua_State *L, const l_int32* ia, l_int32 n);
extern int          ll_push_uarray(lua_State *L, const l_uint32* ua, l_int32 n);
extern int          ll_push_uarray_2d(lua_State *L, const l_uint32* data, l_int32 wpl, l_int32 h);
extern int          ll_push_farray(lua_State *L, const l_float32* fa, l_int32 n);
extern int          ll_push_darray(lua_State *L, const l_float64* da, l_int32 n);
extern int          ll_push_sarray(lua_State *L, Sarray *sa);

extern l_int32    * ll_unpack_iarray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_uint32   * ll_unpack_uarray(const char *_fun, lua_State *L, int arg, l_int32 *plen);
extern l_uint32   * ll_unpack_uarray_2d(const char *_fun, lua_State *L, int arg, l_uint32 *data, l_int32 wpl, l_int32 h);
extern l_float32  * ll_unpack_farray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_float64  * ll_unpack_darray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern Sarray     * ll_unpack_sarray(const char *_fun, lua_State *L, int arg, l_int32 *pn);

extern l_int32      ll_check_index(const char *_fun, lua_State *L, int arg, l_int32 imax);
extern char         ll_check_char(const char *_fun, lua_State *L, int arg);
extern const char * ll_check_string(const char *_fun, lua_State *L, int arg);
extern const char * ll_check_lstring(const char *_fun, lua_State *L, int arg, size_t *plen);
extern l_int32      ll_check_boolean(const char *_fun, lua_State *L, int arg);
extern l_int32      ll_check_boolean_default(const char *_fun, lua_State *L, int arg, int dflt);
extern l_uint8      ll_check_l_uint8(const char *_fun, lua_State *L, int arg);
extern l_uint8      ll_check_l_uint8_default(const char *_fun, lua_State *L, int arg, l_uint32 dflt);
extern l_uint16     ll_check_l_uint16(const char *_fun, lua_State *L, int arg);
extern l_uint16     ll_check_l_uint16_default(const char *_fun, lua_State *L, int arg, l_uint32 dflt);
extern l_int32      ll_check_l_int32(const char *_fun, lua_State *L, int arg);
extern l_int32      ll_check_l_int32_default(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern l_uint32     ll_check_l_uint32(const char *_fun, lua_State *L, int arg);
extern l_uint32     ll_check_l_uint32_default(const char *_fun, lua_State *L, int arg, l_uint32 dflt);
extern l_int64      ll_check_l_int64(const char *_fun, lua_State *L, int arg);
extern l_int64      ll_check_l_int64_default(const char *_fun, lua_State *L, int arg, l_int64 dflt);
extern l_uint64     ll_check_l_uint64(const char *_fun, lua_State *L, int arg);
extern l_uint64     ll_check_l_uint64_default(const char *_fun, lua_State *L, int arg, l_uint64 dflt);
extern l_float32    ll_check_l_float32(const char *_fun, lua_State *L, int arg);
extern l_float32    ll_check_l_float32_default(const char *_fun, lua_State *L, int arg, l_float32 dflt);
extern l_float64    ll_check_l_float64(const char *_fun, lua_State *L, int arg);
extern l_float64    ll_check_l_float64_default(const char *_fun, lua_State *L, int arg, l_float32 dflt);
extern l_int32      ll_check_tbl(const char *_fun, lua_State *L, int arg, l_int32 dflt, const lept_enums_t *tbl, size_t len);

extern luaL_Stream *ll_check_stream(const char *_fun, lua_State *L, int arg);

extern l_int32      ll_check_access_storage(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_access_storage(int flag);

extern l_int32      ll_check_more_less_clip(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_more_less_clip(int flag);

extern l_int32      ll_check_input_format(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_input_format(int format);

extern l_int32      ll_check_keytype(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_keytype(l_int32 type);

extern l_int32      ll_check_consecutive_skip_by(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_consecutive_skip_by(l_int32 choice);

extern l_int32      ll_check_component(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_component(l_int32 component);

extern l_int32      ll_check_choose_min_max(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_choose_min_max(l_int32 choice);

extern l_int32      ll_check_what_is_max(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_what_is_max(l_int32 choice);

extern l_int32      ll_check_getval(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_getval(l_int32 choice);

extern l_int32      ll_check_direction(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_direction(l_int32 dir);

extern l_int32      ll_check_blackwhite(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_blackwhite(l_int32 which);

extern l_int32      ll_check_rasterop(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_rasterop(l_int32 op);

extern l_int32      ll_check_hint(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_hint(l_int32 dir);

extern l_int32      ll_check_searchdir(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_searchir(l_int32 dir);

extern l_int32      ll_check_number_value(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_number_value(l_int32 dir);

extern l_int32      ll_check_stats_type(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_stats_type(l_int32 dir);

extern l_int32      ll_check_select_color(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_select_color(l_int32 color);

extern l_int32      ll_check_select_min_max(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_select_min_max(l_int32 which);

extern l_int32      ll_check_select_size(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_select_size(l_int32 which);

extern l_int32      ll_check_sort_by(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_sort_by(l_int32 sort_by);

extern l_int32      ll_check_set_side(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_set_side(l_int32 which);

extern l_int32      ll_check_from_side(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_from_side(l_int32 which);

extern l_int32      ll_check_adjust_sides(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_adjust_sides(l_int32 which);

extern l_int32      ll_check_sort_mode(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_sort_mode(l_int32 order);

extern l_int32      ll_check_sort_order(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_sort_order(l_int32 order);

extern l_int32      ll_check_trans_order(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_trans_order(l_int32 order);

extern l_int32      ll_check_relation(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_relation(l_int32 rotation);

extern l_int32      ll_check_rotation(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_rotation(l_int32 rotation);

extern l_int32      ll_check_overlap(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_overlap(l_int32 rotation);

extern l_int32      ll_check_subflag(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_subflag(l_int32 rotation);

extern l_int32      ll_check_useflag(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_useflag(l_int32 rotation);

extern l_int32      ll_check_value_flags(const char *_fun, lua_State *L, int arg, l_int32 dflt);
extern const char * ll_string_value_flags(l_int32 rotation);

extern LuaLept    * ll_check_LuaLept(const char *_fun, lua_State *L, int arg);
extern int          ll_push_LuaLept(const char *_fun, lua_State *L, LuaLept *lept);
extern int          ll_new_LuaLept(lua_State *L);
extern int          ll_register_LuaLept(lua_State *L);

/* llamap.cpp */
extern L_AMAP     * ll_check_Amap(const char *_fun, lua_State *L, int arg);
extern L_AMAP     * ll_check_Amap_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Amap(const char *_fun, lua_State *L, L_AMAP *amap);
extern int          ll_new_Amap(lua_State *L);
extern int          ll_register_Amap(lua_State *L);

/* llaset.cpp */
extern L_ASET     * ll_check_Aset(const char *_fun, lua_State *L, int arg);
extern L_ASET     * ll_check_Aset_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Aset(const char *_fun, lua_State *L, L_ASET *aset);
extern int          ll_new_Aset(lua_State *L);
extern int          ll_register_Aset(lua_State *L);

/* llbmf.cpp */
extern L_Bmf      * ll_check_Bmf(const char *_fun, lua_State *L, int arg);
extern L_Bmf      * ll_check_Bmf_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Bmf(const char *_fun, lua_State *L, L_Bmf *bmf);
extern int          ll_new_Bmf(lua_State *L);
extern int          ll_register_Bmf(lua_State *L);

/* lllist.cpp */
extern DoubleLinkedList* ll_check_DoubleLinkedList(const char *_fun, lua_State *L, int arg);
extern DoubleLinkedList* ll_check_DoubleLinkedList_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_DoubleLinkedList(const char *_fun, lua_State *L, DoubleLinkedList *list);
extern int          ll_new_DoubleLinkedList(lua_State *L);
extern int          ll_register_DoubleLinkedList(lua_State *L);

/* llnuma.cpp */
extern Numa       * ll_check_Numa(const char *_fun, lua_State *L, int arg);
extern Numa       * ll_check_Numa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Numa(const char *_fun, lua_State *L, Numa *na);
extern int          ll_new_Numa(lua_State *L);
extern int          ll_register_Numa(lua_State *L);

/* llnumaa.cpp */
extern Numaa      * ll_check_Numaa(const char *_fun, lua_State *L, int arg);
extern Numaa      * ll_check_Numaa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Numaa(const char *_fun, lua_State *L, Numaa *naa);
extern int          ll_new_Numaa(lua_State *L);
extern int          ll_register_Numaa(lua_State *L);

/* lldewarp.cpp */
extern L_Dewarp   * ll_check_Dewarp(const char *_fun, lua_State *L, int arg);
extern L_Dewarp   * ll_check_Dewarp_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Dewarp(const char *_fun, lua_State *L, L_Dewarp *dew);
extern int          ll_new_Dewarp(lua_State *L);
extern int          ll_register_Dewarp(lua_State *L);

/* lldewarpa.cpp */
extern L_Dewarpa  * ll_check_Dewarpa(const char *_fun, lua_State *L, int arg);
extern L_Dewarpa  * ll_check_Dewarpa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Dewarpa(const char *_fun, lua_State *L, L_Dewarpa *dew);
extern int          ll_new_Dewarpa(lua_State *L);
extern int          ll_register_Dewarpa(lua_State *L);

/* lldna.cpp */
extern L_Dna      * ll_check_Dna(const char *_fun, lua_State *L, int arg);
extern L_Dna      * ll_check_Dna_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Dna(const char *_fun, lua_State *L, L_Dna *da);
extern int          ll_new_Dna(lua_State *L);
extern int          ll_register_Dna(lua_State *L);

/* lldnaa.cpp */
extern L_Dnaa     * ll_check_Dnaa(const char *_fun, lua_State *L, int arg);
extern L_Dnaa     * ll_check_Dnaa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Dnaa(const char *_fun, lua_State *L, L_Dnaa *naa);
extern int          ll_new_Dnaa(lua_State *L);
extern int          ll_register_Dnaa(lua_State *L);

/* llpta.cpp */
extern Pta        * ll_check_Pta(const char *_fun, lua_State *L, int arg);
extern Pta        * ll_check_Pta_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Pta(const char *_fun, lua_State *L, Pta *pta);
extern int          ll_new_Pta(lua_State *L);
extern int          ll_register_Pta(lua_State *L);

/* llptaa.cpp */
extern Ptaa       * ll_check_Ptaa(const char *_fun, lua_State *L, int arg);
extern Ptaa       * ll_check_Ptaa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Ptaa(const char *_fun, lua_State *L, Ptaa *ptaa);
extern int          ll_new_Ptaa(lua_State *L);
extern int          ll_register_Ptaa(lua_State *L);

/* llbox.cpp */
extern Box        * ll_check_Box(const char *_fun, lua_State *L, int arg);
extern Box        * ll_check_Box_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Box(const char *_fun, lua_State *L, Box *box);
extern int          ll_new_Box(lua_State *L);
extern int          ll_register_Box(lua_State *L);

/* llboxa.cpp */
extern Boxa       * ll_check_Boxa(const char *_fun, lua_State *L, int arg);
extern Boxa       * ll_check_Boxa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Boxa(const char *_fun, lua_State *L, Boxa *boxa);
extern int          ll_new_Boxa(lua_State *L);
extern int          ll_register_Boxa(lua_State *L);

/* llboxaa.cpp */
extern Boxaa      * ll_check_Boxaa(const char *_fun, lua_State *L, int arg);
extern Boxaa      * ll_check_Boxaa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Boxaa(const char *_fun, lua_State *L, Boxaa *boxaa);
extern int          ll_new_Boxaa(lua_State *L);
extern int          ll_register_Boxaa(lua_State *L);

/* llpixcmap.cpp */
extern PixColormap* ll_check_PixColormap(const char *_fun, lua_State *L, int arg);
extern PixColormap* ll_check_PixColormap_opt(const char *_fun, lua_State *L, int arg);
extern PixColormap* ll_take_PixColormap(const char* _fun, lua_State *L, int arg);
extern int          ll_push_PixColormap(const char *_fun, lua_State *L, PixColormap *cmap);
extern int          ll_new_PixColormap(lua_State *L);
extern int          ll_register_PixColormap(lua_State *L);

/* llpix.cpp */
extern Pix        * ll_check_Pix(const char *_fun, lua_State *L, int arg);
extern Pix        * ll_check_Pix_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Pix(const char *_fun, lua_State *L, Pix *pix);
extern int          ll_new_Pix(lua_State *L);
extern int          ll_register_Pix(lua_State *L);

/* llpixa.cpp */
extern Pixa       * ll_check_Pixa(const char *_fun, lua_State *L, int arg);
extern Pixa       * ll_check_Pixa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Pixa(const char *_fun, lua_State *L, Pixa *pixa);
extern int          ll_new_Pixa(lua_State *L);
extern int          ll_register_Pixa(lua_State *L);

/* llpixaa.cpp */
extern Pixaa      * ll_check_Pixaa(const char *_fun, lua_State *L, int arg);
extern Pixaa      * ll_check_Pixaa_opt(const char *_fun, lua_State *L, int arg);
extern int          ll_push_Pixaa(const char *_fun, lua_State *L, Pixaa *pixaa);
extern int          ll_new_Pixaa(lua_State *L);
extern int          ll_register_Pixaa(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif /* !defined(LUALEPT_EXPORTS_H) */
