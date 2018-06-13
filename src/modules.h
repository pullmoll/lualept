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

#if !defined(LUALEPT_DEBUG)
#define LUALEPT_DEBUG       0     /*!< set to 1 to enable debugging */
#endif

#if !defined(LUALEPT_INTERNALS)
#define LUALEPT_INTERNALS   0     /*!< set to 1 to enable printing internals for __tostring */
#endif

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
#define LL_CHARPTR      "char*"         /*!< Lua variable of type char pointer */
#define LL_BYTEPTR      "l_uint8*"      /*!< Lua variable of type l_uint8 pointer */
#define	LL_AMAP		"Amap"          /*!< Lua class: Amap (key / value pairs) */
#define	LL_ASET		"Aset"          /*!< Lua class: Aset (key set) */
#define	LL_BBUFFER	"Bbuffer"       /*!< Lua class: Bbuffer (byte buffer) */
#define	LL_BMF		"Bmf"           /*!< Lua class: Bmf (Bitmap font) */
#define	LL_BYTEA        "Bytea"         /*!< Lua class: Bytea (array of bytes) */
#define	LL_BOX		"Box"           /*!< Lua class: Box (quad l_int32 for x,y,w,h) */
#define	LL_BOXA		"Boxa"          /*!< Lua class: Boxa (array of Box) */
#define	LL_BOXAA	"Boxaa"         /*!< Lua class: Boxaa (array of Boxa) */
#define	LL_COMPDATA     "CompData"      /*!< Lua class: CompData */
#define	LL_CCBORD       "CCBord"        /*!< Lua class: CCBord */
#define	LL_CCBORDA      "CCBorda"       /*!< Lua class: CCBorda (array of CCBord) */
#define	LL_DEWARP       "Dewarp"        /*!< Lua class: Dewarp */
#define	LL_DEWARPA      "Dewarpa"       /*!< Lua class: Dewarpa (array of Dewarp) */
#define	LL_DLLIST	"Dllist"        /*!< Lua class: DLList */
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
#define	LL_QUEUE        "Queue"         /*!< Lua class: Queue */
#define	LL_RBTNODE      "RbtreeNode"    /*!< Lua class: RbtreeNode (Amap and Aset nodes) */
#define	LL_RBTREE       "Rbtree"	/*!< Lua class: Rbtree (Amap and Aset base) */
#define	LL_SARRAY	"Sarray"        /*!< Lua class: Sarray (array of strings) */
#define	LL_SEL		"Sel"           /*!< Lua class: Sel */
#define	LL_SELA		"Sela"          /*!< Lua class: array of Sel */
#define	LL_STACK        "Stack"         /*!< Lua class: Stack */
#define	LL_WSHED        "WShed"         /*!< Lua class: Stack */

#define	LL_LUALEPT      "LuaLept"       /*!< Lua class: LuaLept (top level) */

/**
 * \brief The log_enable_flags enum defines bit masks
 * to define which kind of debug output is to be printed.
 */
enum dbg_enable_flags {
    LOG_REGISTER        = (1<< 0),  /*!< log Lua class registration */
    LOG_NEW_PARAM       = (1<< 1),  /*!< log ll_new_<ClassName> input parameters */
    LOG_NEW_CLASS       = (1<< 2),  /*!< log ll_new_<ClassName> class creation */
    LOG_DESTROY         = (1<< 3),  /*!< log class destruction */
    LOG_TAKE            = (1<< 4),  /*!< log ll_take_udata calls */
    LOG_PUSH_BOOLEAN    = (1<< 5),  /*!< log pushing booleans */
    LOG_CHECK_BOOLEAN   = (1<< 6),  /*!< log checking for booleans */
    LOG_PUSH_INTEGER    = (1<< 7),  /*!< log pushing integers */
    LOG_CHECK_INTEGER   = (1<< 8),  /*!< log checking for integers */
    LOG_PUSH_NUMBER     = (1<< 9),  /*!< log pushing numbers */
    LOG_CHECK_NUMBER    = (1<<10),  /*!< log checking for numbers */
    LOG_PUSH_STRING     = (1<<11),  /*!< log pushing strings */
    LOG_CHECK_STRING    = (1<<12),  /*!< log checking for strings */
    LOG_PUSH_TABLE      = (1<<13),  /*!< log pushing tables */
    LOG_CHECK_TABLE     = (1<<14),  /*!< log checking for tables */
    LOG_PUSH_CFUNCTION  = (1<<15),  /*!< log pushing C functions */
    LOG_CHECK_CFUNCTION = (1<<16),  /*!< log checking for C functions */
    LOG_PUSH_UDATA      = (1<<17),  /*!< log pushing user data */
    LOG_CHECK_UDATA     = (1<<18),  /*!< log checking for user data */
    LOG_PUSH_LUDATA     = (1<<19),  /*!< log pushing light user data */
    LOG_CHECK_LUDATA    = (1<<20),  /*!< log checking for light user data */
    LOG_PUSH_ARRAY      = (1<<21),  /*!< log pushing table arrays */
    LOG_CHECK_ARRAY     = (1<<22),  /*!< log checking for table arrays */
    LOG_SDL2            = (1<<30)   /*!< log SDL2 display code */
};

#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
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
#if defined(HAVE_TIME_H)
#include <time.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_SDL2)
#include <SDL.h>
#endif

#if !defined(ARRAYSIZE)
/** Return the number of elements in array %t */
#define ARRAYSIZE(t) (sizeof(t)/sizeof(t[0]))
#endif

#define LL_STRBUFF  256

#if defined(HAVE_STRCASECMP)
#define ll_strcasecmp strcasecmp
#elif defined(HAVE_STRICMP)
#define ll_strcasecmp stricmp
#else
/**
 * \brief Our own version of strcasecmp(3).
 * \param dst first string
 * \param src second string
 * \return -1 if dst < src, +1 if dst > str, 0 otherwise.
 */
inline int
ll_strcasecmp(const char* dst, const char* src)
{
    unsigned long lend, lens;
    int d;

    if (!dst || nullptr == src)
        return 0;
    lend = strlen(dst);
    lens = strlen(src);
    if (lend < lens)
        return -1;
    if (lend > lens)
        return +1;
    while (lend--) {
        d = toupper(*dst++) - toupper(*src++);
        if (d < 0)
            return -1;
        if (d > 0)
            return +1;
    }
    return 0;
}
#endif

#if defined(LUALEPT_DEBUG) && (LUALEPT_DEBUG > 0)
extern void dbg(int enable, const char* format, ...)
#if defined(__GNUC__)
    __attribute__((__format__(printf, 2, 3)))
#endif
;

#define DBG(enable, format, ...) dbg(enable, format, __VA_ARGS__)
#else
#define DBG(enable, format, ...)
#endif

#define UNUSED(x) (void)x

/** Local type definition of lua_State to avoid including lua.h here */
typedef struct lua_State lua_State;

/** Local type definition of Leptonica's new l_ok for pre 1.77.0 allheaders.h */
typedef int l_ok;

extern void die(const char *_fun, lua_State* L, const char *format, ...)
#if defined(__GNUC__)
    __attribute__((__format__(printf, 3, 4)))
#endif
;

#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>
#include <lualib.h>

#ifdef __cplusplus
}
#endif

/** Lua function table (luaL_Reg array[]) sentinel */
#define LUA_SENTINEL    {nullptr,nullptr}

/** Allocate a static string with a function's %name */
#define FUNC(name) static const char _fun[] = name; (void)_fun

/** Allocate a static string with a luaopen_%name */
#define LO_FUNC(name) FUNC("ll_open_" name)

/*! Dummy structure for the top level Lua class LL_LUALEPT */
typedef struct LuaLept {
    char str_version[32];                   /*!< Our own version number */
    char str_version_lua[32];               /*!< Lua's version number */
    char str_version_lept[32];              /*!< Leptonica's version number */
}   LuaLept;

/**
 * The structure lept_enum is used to define key strings (%key),
 * their Leptonica enum name (%name), and their enumeration value (%value)
 * Tables are sorted so that the longest form of a key comes first.
 * Most tables have abbreviations for keys. The will be listed, if you
 * specify an unknown string as a parameter to a function using
 * such enumerations.
*/
typedef struct lept_enum {
    const char *key;                        /*!< string for the enumeration value */
    const char *name;                       /*!< name of the enumeration value in Leptonica's header files */
    l_int32     value;                      /*!< l_int32 with enumeration value */
}   lept_enum;

/* llept.cpp */
extern void *ll_ludata(const char *_fun, lua_State* L, int arg);
extern void **ll_udata(const char *_fun, lua_State* L, int arg, const char *tname);

/**
 * \brief Cast the result of LEPT_MALLOC() to the given type.
 * T is the typename of the result pointer
 * \param _fun calling function's name
 * \param L Lua state.
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
 * T is the typename of the result pointer
 * \param _fun calling function's name
 * \param L Lua state.
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
 * \brief Alias for ll_calloc() with l_int32 nmemb and size.
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, l_int32 nmemb, l_int32 size)
{
    return ll_calloc<T>(_fun, L, static_cast<size_t>(nmemb), static_cast<size_t>(size));
}

/**
 * \brief Cast the result of LEPT_CALLOC() to the given type.
 * T is the typename of the result pointer
 * \param _fun calling function's name
 * \param L Lua state.
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
 * \brief Alias for ll_calloc() with l_int32 nmemb.
 */
template <typename T> T*
ll_calloc(const char* _fun, lua_State *L, l_int32 nmemb)
{
    return ll_calloc<T>(_fun, L, static_cast<size_t>(nmemb), sizeof(T));
}

/**
 * \brief Check Lua stack at index %arg for user data with type name %tname.
 * T is the typename of the expected return value.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg argument index
 * \param tname tname of the expected udata
 * \return pointer to the udata.
 */
template<typename T> T **
ll_check_udata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    return reinterpret_cast<T **>(ll_udata(_fun, L, arg, tname));
}

/**
 * \brief Check Lua stack at index %arg for user data with type name %tname.
 * T is the typename of the expected return value.
 * This version takes ownership of the pointer contained in the
 * user data and stores a nullptr there.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg argument index
 * \param tname tname of the expected udata
 * \return pointer to the T contained in the udata.
 */
template<typename T> T *
ll_take_udata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    T **pptr = reinterpret_cast<T **>(ll_udata(_fun, L, arg, tname));
    T *ptr = pptr ? *pptr : nullptr;
    DBG(LOG_TAKE, "%s: %s = %p, %s = %p\n", _fun,
        "pptr", reinterpret_cast<void *>(pptr),
        "ptr", reinterpret_cast<void *>(ptr));
    if (nullptr != pptr)
        *pptr = nullptr;
    return ptr;
}

/**
 * \brief Check Lua stack at index %arg for light user data.
 * T is the typename of the return value.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg argument index
 * \return pointer to the udata.
 */
template<typename T> T *
ll_check_ludata(const char *_fun, lua_State *L, int arg)
{
    return reinterpret_cast<T *>(ll_ludata(_fun, L, arg));
}

extern void             ll_free(void *ptr);

extern int              ll_isnumber(const char *_fun, lua_State *L, int arg);
extern int              ll_isstring(const char *_fun, lua_State *L, int arg);
extern int              ll_iscfuntion(const char *_fun, lua_State *L, int arg);
extern int              ll_isinteger(const char *_fun, lua_State *L, int arg);
extern int              ll_istable(const char *_fun, lua_State* L, int arg);
extern int              ll_isudata(const char *_fun, lua_State* L, int arg, const char *tname);

extern int              ll_register_class(const char *_fun, lua_State *L, const char *name, const luaL_Reg* methods);
extern int              ll_set_global_cfunct(const char *_fun, lua_State *L, const char* tname, lua_CFunction cfunct);
extern int              ll_set_global_table(const char *_fun, lua_State *L, const char* tname);
extern int              ll_push_udata(const char *_fun, lua_State *L, const char *name, void *udata);
extern int              ll_push_nil(lua_State *L);
extern int              ll_push_boolean(const char* _fun, lua_State *L, bool b);
extern int              ll_push_l_int8(const char *_fun, lua_State *L, l_int8 val);
extern int              ll_push_l_uint8(const char *_fun, lua_State *L, l_uint8 val);
extern int              ll_push_l_int16(const char *_fun, lua_State *, l_uint16 val);
extern int              ll_push_l_uint16(const char *_fun, lua_State *, l_uint16 val);
extern int              ll_push_l_int32(const char *_fun, lua_State *L, l_int32 val);
extern int              ll_push_l_uint32(const char *_fun, lua_State *L, l_uint32 val);
extern int              ll_push_l_int64(const char *_fun, lua_State *L, l_int64 val);
extern int              ll_push_l_uint64(const char *_fun, lua_State *L, l_uint64 val);
extern int              ll_push_size_t(const char *_fun, lua_State *L, size_t val);
extern int              ll_push_l_float32(const char *_fun, lua_State *L, l_float32 val);
extern int              ll_push_l_float64(const char *_fun, lua_State *L, l_float64 val);
extern int              ll_push_string(const char *_fun, lua_State *L, const char* str);
extern int              ll_push_lstring(const char *_fun, lua_State *L, const char* str, size_t len);
extern int              ll_push_bytes(const char *_fun, lua_State *L, l_uint8* data, size_t len);

extern int              ll_pack_Iarray(const char* _fun, lua_State *L, const l_int32* iarray, l_int32 n);
extern int              ll_pack_Uarray(const char* _fun, lua_State *L, const l_uint32* uarray, l_int32 n);
extern int              ll_pack_Uarray_2d(const char* _fun, lua_State *L, const l_uint32* data, l_int32 wpl, l_int32 h);
extern int              ll_pack_Farray(const char* _fun, lua_State *L, const l_float32* farray, l_int32 n);
extern int              ll_pack_Farray_2d(const char* _fun, lua_State *L, const l_float32* data, l_int32 wpl, l_int32 h);
extern int              ll_pack_Darray(const char* _fun, lua_State *L, const l_float64* da, l_int32 n);
extern int              ll_pack_Darray_2d(const char* _fun, lua_State *L, const l_float64* da, l_int32 wpl, l_int32 h);
extern int              ll_pack_Sarray(const char* _fun, lua_State *L, Sarray *sa);

extern l_int32        * ll_unpack_Iarray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_uint32       * ll_unpack_Uarray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_uint32       * ll_unpack_Uarray_2d(const char *_fun, lua_State *L, int arg, l_uint32 *data, l_int32 wpl, l_int32 h);
extern l_float32      * ll_unpack_Farray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_float32      * ll_unpack_Farray_2d(const char *_fun, lua_State *L, int arg, l_float32 *data, l_int32 wpl, l_int32 h);
extern l_float32      * ll_unpack_Matrix(const char *_fun, lua_State *L, int arg, int w, int h);
extern l_float64      * ll_unpack_Darray(const char *_fun, lua_State *L, int arg, l_int32 *pn);
extern l_float64      * ll_unpack_Darray_2d(const char *_fun, lua_State *L, int arg, l_float64 *data, l_int32 wpl, l_int32 h);
extern Sarray         * ll_unpack_Sarray(const char *_fun, lua_State *L, int arg, l_int32 *pn);

extern l_int32          ll_check_index(const char *_fun, lua_State *L, int arg, l_int32 imax = INT32_MAX);
extern l_uint32         ll_check_color_index(const char *_fun, lua_State *L, int arg, Pix* pix = nullptr);
extern char             ll_check_char(const char *_fun, lua_State *L, int arg);
extern const char     * ll_check_string(const char *_fun, lua_State *L, int arg);
extern const char     * ll_opt_string(const char *_fun, lua_State *L, int arg, const char* def = nullptr);
extern const char     * ll_check_lstring(const char *_fun, lua_State *L, int arg, size_t *plen = nullptr);
extern const l_uint8  * ll_check_lbytes(const char *_fun, lua_State *L, int arg, size_t *plen = nullptr);
extern const l_uint8  * ll_check_lbytes(const char *_fun, lua_State *L, int arg, l_int32 *plen = nullptr);
extern luaL_Stream    * ll_check_stream(const char *_fun, lua_State *L, int arg);
extern luaL_Stream    * ll_opt_stream(const char *_fun, lua_State *L, int arg);
extern l_int32          ll_check_boolean(const char *_fun, lua_State *L, int arg);
extern l_int32          ll_opt_boolean(const char *_fun, lua_State *L, int arg, int def = 0);
extern l_uint8          ll_check_l_uint8(const char *_fun, lua_State *L, int arg);
extern l_uint8          ll_opt_l_uint8(const char *_fun, lua_State *L, int arg, l_uint32 def = 0);
extern l_uint16         ll_check_l_uint16(const char *_fun, lua_State *L, int arg);
extern l_uint16         ll_opt_l_uint16(const char *_fun, lua_State *L, int arg, l_uint32 def = 0);
extern l_int32          ll_check_l_int32(const char *_fun, lua_State *L, int arg);
extern l_int32          ll_opt_l_int32(const char *_fun, lua_State *L, int arg, l_int32 def = 0);
extern l_uint32         ll_check_l_uint32(const char *_fun, lua_State *L, int arg);
extern l_uint32         ll_opt_l_uint32(const char *_fun, lua_State *L, int arg, l_uint32 def = 0);
extern l_int64          ll_check_l_int64(const char *_fun, lua_State *L, int arg);
extern l_int64          ll_opt_l_int64(const char *_fun, lua_State *L, int arg, l_int64 def = 0);
extern l_uint64         ll_check_l_uint64(const char *_fun, lua_State *L, int arg);
extern l_uint64         ll_opt_l_uint64(const char *_fun, lua_State *L, int arg, l_uint64 def = 0);
extern l_float32        ll_check_l_float32(const char *_fun, lua_State *L, int arg);
extern l_float32        ll_opt_l_float32(const char *_fun, lua_State *L, int arg, l_float32 def = 0.0f);
extern l_float64        ll_check_l_float64(const char *_fun, lua_State *L, int arg);
extern l_float64        ll_opt_l_float64(const char *_fun, lua_State *L, int arg, l_float32 def = 0.0);
extern l_float32      * ll_check_vector(const char *_fun, lua_State *L, int arg, int n = 6);
extern size_t           ll_check_size_t(const char *_fun, lua_State *L, int arg);
extern size_t           ll_opt_size_t(const char *_fun, lua_State *L, int arg, size_t def = 0);

/*
 *  lualept string Leptonica enumeration value lookup functions
 */

extern int              ll_list_tbl_options(const char *_fun, lua_State *L, const lept_enum *tbl, size_t len, const char *msg = nullptr);
extern const char*      ll_string_tbl(l_int32 value, const lept_enum *tbl, size_t len);
extern l_int32          ll_check_tbl(const char *_fun, lua_State *L, int arg, l_int32 def, const lept_enum *tbl, size_t len);

extern l_int32          ll_check_debug(const char *_fun, lua_State *L, int arg, l_int32 def = 0);
extern const char     * ll_string_debug(l_int32 flag);

extern l_int32          ll_check_access_storage(const char *_fun, lua_State *L, int arg, l_int32 def = L_CLONE);
extern const char     * ll_string_access_storage(l_int32 flag);

extern l_int32          ll_check_more_less_clip(const char *_fun, lua_State *L, int arg, l_int32 def = L_LS_BYTE);
extern const char     * ll_string_more_less_clip(l_int32 flag);

extern l_int32          ll_check_encoding(const char *_fun, lua_State *L, int arg, l_int32 def = L_DEFAULT_ENCODE);
extern const char     * ll_string_encoding(l_int32 flag);

extern l_int32          ll_check_input_format(const char *_fun, lua_State *L, int arg, l_int32 def = IFF_DEFAULT);
extern const char     * ll_string_input_format(int format);

extern l_int32          ll_check_keytype(const char *_fun, lua_State *L, int arg, l_int32 def = L_INT_TYPE);
extern const char     * ll_string_keytype(l_int32 type);

extern l_int32          ll_check_consecutive_skip_by(const char *_fun, lua_State *L, int arg, l_int32 def = L_CHOOSE_CONSECUTIVE);
extern const char     * ll_string_consecutive_skip_by(l_int32 choice);

extern l_int32          ll_check_text_orientation(const char *_fun, lua_State *L, int arg, l_int32 def = L_TEXT_ORIENT_UNKNOWN);
extern const char     * ll_string_text_orientation(l_int32 component);

extern l_int32          ll_check_edge_orientation(const char *_fun, lua_State *L, int arg, l_int32 def = L_HORIZONTAL_EDGES);
extern const char     * ll_string_edge_orientation(l_int32 component);

extern l_int32          ll_check_component(const char *_fun, lua_State *L, int arg, l_int32 def = COLOR_RED);
extern const char     * ll_string_component(l_int32 component);

extern l_int32          ll_check_compression(const char *_fun, lua_State *L, int arg, l_int32 def = IFF_DEFAULT);
extern const char     * ll_string_compression(l_int32 component);

extern l_int32          ll_check_choose_min_max(const char *_fun, lua_State *L, int arg, l_int32 def = L_CHOOSE_MIN);
extern const char     * ll_string_choose_min_max(l_int32 choice);

extern l_int32          ll_check_what_is_max(const char *_fun, lua_State *L, int arg, l_int32 def = L_WHITE_IS_MAX);
extern const char     * ll_string_what_is_max(l_int32 choice);

extern l_int32          ll_check_getval(const char *_fun, lua_State *L, int arg, l_int32 def = L_GET_WHITE_VAL);
extern const char     * ll_string_getval(l_int32 choice);

extern l_int32          ll_check_direction(const char *_fun, lua_State *L, int arg, l_int32 def = L_HORIZONTAL_LINE);
extern const char     * ll_string_direction(l_int32 dir);

extern l_int32          ll_check_distance(const char *_fun, lua_State *L, int arg, l_int32 def = L_MANHATTAN_DISTANCE);
extern const char     * ll_string_distance(l_int32 dir);

extern l_int32          ll_check_set_black_white(const char *_fun, lua_State *L, int arg, l_int32 def = L_SET_WHITE);
extern const char     * ll_string_set_black_white(l_int32 which);

extern l_int32          ll_check_rasterop(const char *_fun, lua_State *L, int arg, l_int32 def = PIX_SRC);
extern const char     * ll_string_rasterop(l_int32 op);

extern l_int32          ll_check_hint(const char *_fun, lua_State *L, int arg, l_int32 def = 0);
extern const char     * ll_string_hint(l_int32 dir);

extern l_int32          ll_check_searchdir(const char *_fun, lua_State *L, int arg, l_int32 def = L_HORIZ);
extern const char     * ll_string_searchir(l_int32 dir);

extern l_int32          ll_check_number_value(const char *_fun, lua_State *L, int arg, l_int32 def = L_INTEGER_VALUE);
extern const char     * ll_string_number_value(l_int32 dir);

extern l_int32          ll_check_position(const char *_fun, lua_State *L, int arg, l_int32 def = 0);
extern const char     * ll_string_position(l_int32 dir);

extern l_int32          ll_check_stats_type(const char *_fun, lua_State *L, int arg, l_int32 def = L_MEAN_ABSVAL);
extern const char     * ll_string_stats_type(l_int32 dir);

extern l_int32          ll_check_select_color(const char *_fun, lua_State *L, int arg, l_int32 def = L_SELECT_RED);
extern const char     * ll_string_select_color(l_int32 color);

extern l_int32          ll_check_select_min_max(const char *_fun, lua_State *L, int arg, l_int32 def = L_SELECT_MIN);
extern const char     * ll_string_select_min_max(l_int32 which);

extern l_int32          ll_check_sel(const char *_fun, lua_State *L, int arg, l_int32 def = SEL_DONT_CARE);
extern const char     * ll_string_sel(l_int32 which);

extern l_int32          ll_check_select_size(const char *_fun, lua_State *L, int arg, l_int32 def = L_SELECT_WIDTH);
extern const char     * ll_string_select_size(l_int32 which);

extern l_int32          ll_check_sort_by(const char *_fun, lua_State *L, int arg, l_int32 def = L_SORT_BY_X);
extern const char     * ll_string_sort_by(l_int32 sort_by);

extern l_int32          ll_check_set_side(const char *_fun, lua_State *L, int arg, l_int32 def = L_SET_LEFT);
extern const char     * ll_string_set_side(l_int32 which);

extern l_int32          ll_check_from_side(const char *_fun, lua_State *L, int arg, l_int32 def = L_FROM_LEFT);
extern const char     * ll_string_from_side(l_int32 which);

extern l_int32          ll_check_adjust_sides(const char *_fun, lua_State *L, int arg, l_int32 def = L_ADJUST_SKIP);
extern const char     * ll_string_adjust_sides(l_int32 which);

extern l_int32          ll_check_sort_mode(const char *_fun, lua_State *L, int arg, l_int32 def = L_SHELL_SORT);
extern const char     * ll_string_sort_mode(l_int32 order);

extern l_int32          ll_check_sort_order(const char *_fun, lua_State *L, int arg, l_int32 def = L_SORT_INCREASING);
extern const char     * ll_string_sort_order(l_int32 order);

extern l_int32          ll_check_trans_order(const char *_fun, lua_State *L, int arg, l_int32 def = L_TR_SC_RO);
extern const char     * ll_string_trans_order(l_int32 order);

extern l_int32          ll_check_region(const char *_fun, lua_State *L, int arg, l_int32 def = L_INCLUDE_REGION);
extern const char     * ll_string_region(l_int32 rotation);

extern l_int32          ll_check_relation(const char *_fun, lua_State *L, int arg, l_int32 def = L_SELECT_IF_LT);
extern const char     * ll_string_relation(l_int32 rotation);

extern l_int32          ll_check_rotation(const char *_fun, lua_State *L, int arg, l_int32 def = 0);
extern const char     * ll_string_rotation(l_int32 rotation);

extern l_int32          ll_check_overlap(const char *_fun, lua_State *L, int arg, l_int32 def = L_COMBINE);
extern const char     * ll_string_overlap(l_int32 rotation);

extern l_int32          ll_check_subflag(const char *_fun, lua_State *L, int arg, l_int32 def = L_USE_MINSIZE);
extern const char     * ll_string_subflag(l_int32 rotation);

extern l_int32          ll_check_useflag(const char *_fun, lua_State *L, int arg, l_int32 def = L_USE_ALL_BOXES);
extern const char     * ll_string_useflag(l_int32 rotation);

extern l_int32          ll_check_negvals(const char *_fun, lua_State *L, int arg, l_int32 def = L_CLIP_TO_ZERO);
extern const char     * ll_string_negvals(l_int32 rotation);

extern l_int32          ll_check_value_flags(const char *_fun, lua_State *L, int arg, l_int32 def = L_NEGATIVE);
extern const char     * ll_string_value_flags(l_int32 rotation);

extern l_int32          ll_check_paint_flags(const char *_fun, lua_State *L, int arg, l_int32 def = L_PAINT_LIGHT);
extern const char     * ll_string_paint_flags(l_int32 rotation);

extern l_int32          ll_check_pts_flag(const char *_fun, lua_State *L, int arg, l_int32 def = CCB_SAVE_ALL_PTS);
extern const char     * ll_string_pts_flag(l_int32 rotation);

extern l_int32          ll_check_coord_type(const char *_fun, lua_State *L, int arg, l_int32 def = CCB_LOCAL_COORDS);
extern const char     * ll_string_coord_type(l_int32 rotation);

extern l_int32          ll_check_color_name(const char *_fun, lua_State *L, int arg, l_int32 def = 0x1000000);
extern const char     * ll_string_color_name(l_uint32 rotation);

extern int              ll_check_color(const char *_fun, lua_State *L, int arg,
                                       l_int32 *pr = nullptr,
                                       l_int32 *pg = nullptr,
                                       l_int32 *pb = nullptr,
                                       l_int32 *pa = nullptr);
extern int              ll_check_color(const char *_fun, lua_State *L, int arg, l_uint32 *ppixel);

/* lualept.cpp */
extern LuaLept        * ll_check_lualept(const char *_fun, lua_State *L, int arg);
extern int              ll_push_lualept(const char *_fun, lua_State *L, LuaLept *lept);
extern int              ll_new_lualept(lua_State *L);

/* llamap.cpp */
extern Amap           * ll_check_Amap(const char *_fun, lua_State *L, int arg);
extern Amap           * ll_opt_Amap(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Amap(const char *_fun, lua_State *L, Amap *amap);
extern int              ll_new_Amap(lua_State *L);

/* llaset.cpp */
extern Aset           * ll_check_Aset(const char *_fun, lua_State *L, int arg);
extern Aset           * ll_opt_Aset(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Aset(const char *_fun, lua_State *L, Aset *aset);
extern int              ll_new_Aset(lua_State *L);

/* llbbuffer.cpp */
extern ByteBuffer     * ll_check_ByteBuffer(const char *_fun, lua_State *L, int arg);
extern ByteBuffer     * ll_opt_ByteBuffer(const char *_fun, lua_State *L, int arg);
extern int              ll_push_ByteBuffer(const char *_fun, lua_State *L, ByteBuffer *bb);
extern int              ll_new_ByteBuffer(lua_State *L);

/* llbmf.cpp */
extern Bmf            * ll_check_Bmf(const char *_fun, lua_State *L, int arg);
extern Bmf            * ll_opt_Bmf(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Bmf(const char *_fun, lua_State *L, Bmf *bmf);
extern int              ll_new_Bmf(lua_State *L);

/* llbytea.cpp */
extern Bytea          * ll_check_Bytea(const char *_fun, lua_State *L, int arg);
extern Bytea          * ll_opt_Bytea(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Bytea(const char *_fun, lua_State *L, Bytea *bmf);
extern int              ll_new_Bytea(lua_State *L);

/* lllist.cpp */
extern DLList         * ll_check_DLList(const char *_fun, lua_State *L, int arg);
extern DLList         * ll_opt_DLList(const char *_fun, lua_State *L, int arg);
extern int              ll_push_DLList(const char *_fun, lua_State *L, DLList *list);
extern int              ll_new_DLList(lua_State *L);

/* llnuma.cpp */
extern Numa           * ll_check_Numa(const char *_fun, lua_State *L, int arg);
extern Numa           * ll_opt_Numa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Numa(const char *_fun, lua_State *L, Numa *na);
extern int              ll_new_Numa(lua_State *L);

/* llnumaa.cpp */
extern Numaa          * ll_check_Numaa(const char *_fun, lua_State *L, int arg);
extern Numaa          * ll_opt_Numaa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Numaa(const char *_fun, lua_State *L, Numaa *naa);
extern int              ll_new_Numaa(lua_State *L);

/* lldewarp.cpp */
extern Dewarp         * ll_check_Dewarp(const char *_fun, lua_State *L, int arg);
extern Dewarp         * ll_opt_Dewarp(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Dewarp(const char *_fun, lua_State *L, Dewarp *dew);
extern int              ll_new_Dewarp(lua_State *L);

/* lldewarpa.cpp */
extern Dewarpa        * ll_check_Dewarpa(const char *_fun, lua_State *L, int arg);
extern Dewarpa        * ll_opt_Dewarpa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Dewarpa(const char *_fun, lua_State *L, Dewarpa *dew);
extern int              ll_new_Dewarpa(lua_State *L);

/* lldna.cpp */
extern Dna            * ll_check_Dna(const char *_fun, lua_State *L, int arg);
extern Dna            * ll_opt_Dna(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Dna(const char *_fun, lua_State *L, Dna *da);
extern int              ll_new_Dna(lua_State *L);

/* lldnaa.cpp */
extern Dnaa           * ll_check_Dnaa(const char *_fun, lua_State *L, int arg);
extern Dnaa           * ll_opt_Dnaa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Dnaa(const char *_fun, lua_State *L, Dnaa *naa);
extern int              ll_new_Dnaa(lua_State *L);

/* lldnahash.cpp */
extern DnaHash        * ll_check_DnaHash(const char *_fun, lua_State *L, int arg);
extern DnaHash        * ll_opt_DnaHash(const char *_fun, lua_State *L, int arg);
extern int              ll_push_DnaHash(const char *_fun, lua_State *L, DnaHash *dh);
extern int              ll_new_DnaHash(lua_State *L);

/* llpta.cpp */
extern Pta            * ll_check_Pta(const char *_fun, lua_State *L, int arg);
extern Pta            * ll_opt_Pta(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Pta(const char *_fun, lua_State *L, Pta *pta);
extern int              ll_new_Pta(lua_State *L);

/* llptaa.cpp */
extern Ptaa           * ll_check_Ptaa(const char *_fun, lua_State *L, int arg);
extern Ptaa           * ll_opt_Ptaa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Ptaa(const char *_fun, lua_State *L, Ptaa *ptaa);
extern int              ll_new_Ptaa(lua_State *L);

/* llbox.cpp */
extern Box            * ll_check_Box(const char *_fun, lua_State *L, int arg);
extern Box            * ll_opt_Box(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Box(const char *_fun, lua_State *L, Box *box);
extern int              ll_new_Box(lua_State *L);

/* llboxa.cpp */
extern Boxa           * ll_check_Boxa(const char *_fun, lua_State *L, int arg);
extern Boxa           * ll_opt_Boxa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Boxa(const char *_fun, lua_State *L, Boxa *boxa);
extern int              ll_new_Boxa(lua_State *L);

/* llboxaa.cpp */
extern Boxaa          * ll_check_Boxaa(const char *_fun, lua_State *L, int arg);
extern Boxaa          * ll_opt_Boxaa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Boxaa(const char *_fun, lua_State *L, Boxaa *boxaa);
extern int              ll_new_Boxaa(lua_State *L);

/* llccbord.cpp */
extern CCBord         * ll_check_CCBord(const char *_fun, lua_State *L, int arg);
extern CCBord         * ll_opt_CCBord(const char *_fun, lua_State *L, int arg);
extern int              ll_push_CCBord(const char *_fun, lua_State *L, CCBord *ccb);
extern int              ll_new_CCBord(lua_State *L);

/* llccborda.cpp */
extern CCBorda        * ll_check_CCBorda(const char *_fun, lua_State *L, int arg);
extern CCBorda        * ll_opt_CCBorda(const char *_fun, lua_State *L, int arg);
extern int              ll_push_CCBorda(const char *_fun, lua_State *L, CCBorda *ccba);
extern int              ll_new_CCBorda(lua_State *L);

/* llpixcmap.cpp */
extern PixColormap    * ll_check_PixColormap(const char *_fun, lua_State *L, int arg);
extern PixColormap    * ll_opt_PixColormap(const char *_fun, lua_State *L, int arg);
extern int              ll_push_PixColormap(const char *_fun, lua_State *L, PixColormap *cmap);
extern int              ll_new_PixColormap(lua_State *L);

/* llpixcomp.cpp */
extern PixComp        * ll_check_PixComp(const char *_fun, lua_State *L, int arg);
extern PixComp        * ll_opt_PixComp(const char *_fun, lua_State *L, int arg);
extern int              ll_push_PixComp(const char *_fun, lua_State *L, PixComp *pixcomp);
extern int              ll_new_PixComp(lua_State *L);

/* llpixacomp.cpp */
extern PixaComp       * ll_check_PixaComp(const char *_fun, lua_State *L, int arg);
extern PixaComp       * ll_opt_PixaComp(const char *_fun, lua_State *L, int arg);
extern int              ll_push_PixaComp(const char *_fun, lua_State *L, PixaComp *pixacomp);
extern int              ll_new_PixaComp(lua_State *L);

/* llpix.cpp */
extern Pix            * ll_check_Pix(const char *_fun, lua_State *L, int arg);
extern Pix            * ll_opt_Pix(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Pix(const char *_fun, lua_State *L, Pix *pix);
extern int              ll_new_Pix(lua_State *L);

/* llpixa.cpp */
extern Pixa           * ll_check_Pixa(const char *_fun, lua_State *L, int arg);
extern Pixa           * ll_opt_Pixa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Pixa(const char *_fun, lua_State *L, Pixa *pixa);
extern int              ll_new_Pixa(lua_State *L);

/* llpixaa.cpp */
extern Pixaa          * ll_check_Pixaa(const char *_fun, lua_State *L, int arg);
extern Pixaa          * ll_opt_Pixaa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Pixaa(const char *_fun, lua_State *L, Pixaa *pixaa);
extern int              ll_new_Pixaa(lua_State *L);

/* llfpix.cpp */
extern FPix           * ll_check_FPix(const char *_fun, lua_State *L, int arg);
extern FPix           * ll_opt_FPix(const char *_fun, lua_State *L, int arg);
extern int              ll_push_FPix(const char *_fun, lua_State *L, FPix *fpix);
extern int              ll_new_FPix(lua_State *L);

/* llfpixa.cpp */
extern FPixa          * ll_check_FPixa(const char *_fun, lua_State *L, int arg);
extern FPixa          * ll_opt_FPixa(const char *_fun, lua_State *L, int arg);
extern int              ll_push_FPixa(const char *_fun, lua_State *L, FPixa *fpixa);
extern int              ll_new_FPixa(lua_State *L);

/* lldpix.cpp */
extern DPix           * ll_check_DPix(const char *_fun, lua_State *L, int arg);
extern DPix           * ll_opt_DPix(const char *_fun, lua_State *L, int arg);
extern int              ll_push_DPix(const char *_fun, lua_State *L, DPix *fpix);
extern int              ll_new_DPix(lua_State *L);

/* llpixtiling.cpp */
extern PixTiling      * ll_check_PixTiling(const char *_fun, lua_State *L, int arg);
extern PixTiling      * ll_opt_PixTiling(const char *_fun, lua_State *L, int arg);
extern int              ll_push_PixTiling(const char *_fun, lua_State *L, PixTiling *pt);
extern int              ll_new_PixTiling(lua_State *L);

/* llsel.cpp */
extern Sel            * ll_check_Sel(const char *_fun, lua_State *L, int arg);
extern Sel            * ll_opt_Sel(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Sel(const char *_fun, lua_State *L, Sel *sel);
extern int              ll_new_Sel(lua_State *L);

/* llsela.cpp */
extern Sela           * ll_check_Sela(const char *_fun, lua_State *L, int arg);
extern Sela           * ll_opt_Sela(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Sela(const char *_fun, lua_State *L, Sela *sela);
extern int              ll_new_Sela(lua_State *L);

/* llkernel.cpp */
extern Kernel         * ll_check_Kernel(const char *_fun, lua_State *L, int arg);
extern Kernel         * ll_opt_Kernel(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Kernel(const char *_fun, lua_State *L, Kernel *sel);
extern int              ll_new_Kernel(lua_State *L);

/* llcompdata.cpp */
extern CompData       * ll_check_CompData(const char *_fun, lua_State *L, int arg);
extern CompData       * ll_opt_CompData(const char *_fun, lua_State *L, int arg);
extern int              ll_push_CompData(const char *_fun, lua_State *L, CompData *compdata);
extern int              ll_new_CompData(lua_State *L);

/* llpdfdata.cpp */
extern PdfData        * ll_check_PdfData(const char *_fun, lua_State *L, int arg);
extern PdfData        * ll_opt_PdfData(const char *_fun, lua_State *L, int arg);
extern int              ll_push_PdfData(const char *_fun, lua_State *L, PdfData *pdfdata);
extern int              ll_new_PdfData(lua_State *L);

/* llqueue.cpp */
extern Queue          * ll_check_Queue(const char *_fun, lua_State *L, int arg);
extern Queue          * ll_opt_Queue(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Queue(const char *_fun, lua_State *L, Queue *queue);
extern int              ll_new_Queue(lua_State *L);

/* llsarray.cpp */
extern Sarray         * ll_check_Sarray(const char *_fun, lua_State *L, int arg);
extern Sarray         * ll_opt_Sarray(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Sarray(const char *_fun, lua_State *L, Sarray *sa);
extern int              ll_new_Sarray(lua_State *L);

/* llstack.cpp */
extern Stack          * ll_check_Stack(const char *_fun, lua_State *L, int arg);
extern Stack          * ll_opt_Stack(const char *_fun, lua_State *L, int arg);
extern int              ll_push_Stack(const char *_fun, lua_State *L, Stack *stack);
extern int              ll_new_Stack(lua_State *L);

/* llwshed.cpp */
extern WShed          * ll_check_WShed(const char *_fun, lua_State *L, int arg);
extern WShed          * ll_opt_WShed(const char *_fun, lua_State *L, int arg);
extern int              ll_push_WShed(const char *_fun, lua_State *L, WShed *ws);
extern int              ll_new_WShed(lua_State *L);

/* lualept-sdl2.cpp */
extern int ViewSDL2(Pix* pix, const char* title = nullptr, int x0 = 0, int y0 = 0, float dscale = 0.0f);

/* lualept.cpp */

extern int ll_set_all_globals(const char *_fun, lua_State *L, const ll_global_var_t *vars);
extern int ll_get_all_globals(const char *_fun, lua_State *L, const ll_global_var_t *vars);


#endif /* !defined(LUALEPT_EXPORTS_H) */
