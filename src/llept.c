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

#include "llept.h"
#include <lauxlib.h>
#include <lualib.h>

#if !defined(HAVE_FLOAT_H)
/* FIXME: how are these values really defined? */
static const unsigned long _flt_min = 0xfeffffffUL;
static const unsigned long _flt_max = 0x7effffffUL;
#define FLT_MIN (*(const float*)&_flt_min)
#define FLT_MAX (*(const float*)&_flt_max)
#endif

#if !defined(HAVE_CTYPE_H)
/**
 * @brief Poor man's toupper(3)
 * \param ch ASCII character
 * @return upper case value for ch
 */
static __inline int toupper(const int ch) { return (ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch; }
#endif

#if defined(HAVE_STRCASECMP)
#define ll_strcasecmp strcasecmp
#elif defined(HAVE_STRICMP)
#define ll_strcasecmp stricmp
#else
/**
 * @brief Our own version of strcasecmp(3)
 * \param dst first string
 * \param src second string
 * @return -1 if dst < src, +1 if dst > str, 0 otherwise
 */
int
ll_strcasecmp(const char* dst, const char* src)
{
    unsigned long lend, lens;
    int d;

    if (NULL == dst || NULL == src)
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

/**
 * \brief Register a class for Lua
 * \param L pointer to the lua_State
 * \param name tname of the table to register
 * \param methods array of methods to register
 * \param functions array of functions to register
 * \return 1 table on the Lua stack
 */
int
ll_register_class(lua_State *L, const char *name, const luaL_Reg *methods, const luaL_Reg *functions)
{
    int nfunctions = 0, nmethods = 0;

    while (methods[nmethods].name)
        nmethods++;
    while (functions[nfunctions].name)
        nfunctions++;

    luaL_newmetatable(L, name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, methods, 0);
    lua_createtable(L, 0, nfunctions);
    luaL_setfuncs(L, functions, 0)
    DBG(LOG_REGISTER, "%s: '%s' registered with %d methods and %d functions\n", __func__,
        name, nmethods, nfunctions);
    return 1;
}

/**
 * @brief Check Lua stack at index %arg for udata with %name
 * \param L pointer to the lua_State
 * \param arg argument index
 * \param name tname of the expected udata
 * @return pointer to the udata
 */
void **
ll_check_udata(lua_State *L, int arg, const char* name)
{
    static char msg[128];
    void **ppvoid = luaL_checkudata(L, arg, name);
    snprintf(msg, sizeof(msg), "'%s' expected", name);
    luaL_argcheck(L, ppvoid != NULL, arg, msg);
    return ppvoid;

}

/**
 * @brief Push user data %udata to the Lua stack and set its meta table %name
 * \param L pointer to the lua_State
 * \param name tname for the udata
 * \param udata pointer to the udata
 * @return 1 table on the stack
 */
int
ll_push_udata(lua_State *L, const char* name, void *udata)
{
    void **ppvoid = lua_newuserdata(L, sizeof(udata));
    *ppvoid = udata;
    lua_getfield(L, LUA_REGISTRYINDEX, name);
    lua_setmetatable(L, -2);
    DBG(LOG_CREATE, "%s: pushed '%s' ppvoid=%p udata=%p\n", __func__, name, (void *)ppvoid, udata);
    return 1;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of 0 < index <= imax
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \param imax maximum index value (for the 1-based value)
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_check_index(lua_State *L, int arg, l_int32 imax)
{
    /* Lua indices are 1-based but Leptonica index is 0-based */
    lua_Integer index = luaL_checkinteger(L, arg) - 1;
    if (index < 0 || index >= imax) {
        lua_pushfstring(L, "index out of bounds (%d)", index);
        lua_error(L);
        return 0;       /* NOTREACHED */
    }
    return (l_int32)index;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_int32
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_check_l_int32(lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "l_int32 out of bounds (%d)", val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return (l_int32)val;
}

/**
 * \brief Return an argument lua_Integer in the range of l_int32 or the default
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \param dflt default value
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_check_l_int32_default(lua_State *L, int arg, l_int32 dflt)
{
    lua_Integer val = luaL_optinteger(L, arg, dflt);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "l_int32 #%d out of bounds (%d)", arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return (l_int32)val;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_uint32
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \return l_uint32 for the integer; lua_error if out of bounds
 */
l_uint32
ll_check_l_uint32(lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "l_uint32 out of bounds (%d)", val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return (l_uint32)val;
}

/**
 * \brief Return an argument lua_Integer in the range of l_uint32 or the default
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \param dflt default value
 * \return l_uint32 for the integer; lua_error if out of bounds
 */
l_uint32
ll_check_l_uint32_default(lua_State *L, int arg, l_uint32 dflt)
{
    lua_Integer val = luaL_optinteger(L, arg, dflt);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "l_uint32 #%d out of bounds (%d)", arg, val);
        lua_error(L);
        return dflt;    /* NOTREACHED */
    }
    return (l_uint32)val;
}

/**
 * \brief Check if an argument is a lua_Number in the range of l_float32
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_check_l_float32(lua_State *L, int arg)
{
    lua_Number val = luaL_checknumber(L, arg);

    if (val < (lua_Number)FLT_MIN || val > (lua_Number)FLT_MAX) {
        lua_pushfstring(L, "l_float32 #%d out of bounds (%g)", arg, val);
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return (l_float32)val;
}

/**
 * \brief Return an argument lua_Integer in the range of l_float32 or the default
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \param dflt default value
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_check_l_float32_default(lua_State *L, int arg, l_float32 dflt)
{
    lua_Number val = luaL_optnumber(L, arg, (lua_Number)dflt);

    if (val < (lua_Number)FLT_MIN || val > (lua_Number)FLT_MAX) {
        lua_pushfstring(L, "l_float32 #%d out of bounds (%g)", arg, val);
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return (l_float32)val;
}

/**
 * @brief Find a option %str in a key_value_t array %tbl of size %len
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \param tbl table of key/value pairs
 * \param len length of that table
 * @return value or dflt
 */
l_int32
ll_check_tbl(lua_State *L, int arg, l_int32 dflt, const key_value_t *tbl, size_t len)
{
    size_t i;

    const char* str = lua_isstring(L, arg) ? lua_tostring(L, arg) : NULL;
    if (!str)
        return dflt;

    for (i = 0; i < len; i++) {
        const key_value_t* p = &tbl[i];
        if (!ll_strcasecmp(str, p->key))
            return p->value;
    }

    lua_pushfstring(L, "Invalid option #%d '%s'", arg, str);
    lua_error(L);
    return dflt;

}

/**
 * \brief Check for an optional storage flag as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_access_storage(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"nocopy",      L_NOCOPY},      /* do not copy the object; do not delete the ptr */
        {"insert",      L_INSERT},      /* stuff it in; do not copy or clone */
        {"copy",        L_COPY},        /* make/use a copy of the object */
        {"clone",       L_CLONE},       /* make/use clone (ref count) of the object */
        {"copy_clone",  L_COPY_CLONE}   /* make a new array object (e.g., pixa) and fill the array with clones (e.g., pix) */
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for an image format name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_input_format(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"unknown",         IFF_UNKNOWN},
        {"bmp",             IFF_BMP},
        {"jpg",             IFF_JFIF_JPEG},
        {"jpeg",            IFF_JFIF_JPEG},
        {"jfif",            IFF_JFIF_JPEG},
        {"png",             IFF_PNG},
        {"tif",             IFF_TIFF},
        {"tiff",            IFF_TIFF},
        {"tiff-packbits",   IFF_TIFF_PACKBITS},
        {"packbits",        IFF_TIFF_PACKBITS},
        {"tiff-rle",        IFF_TIFF_RLE},
        {"rle",             IFF_TIFF_RLE},
        {"tiff-g3",         IFF_TIFF_G3},
        {"g3",              IFF_TIFF_G3},
        {"tiff-g4",         IFF_TIFF_G4},
        {"g4",              IFF_TIFF_G4},
        {"tiff-lzw",        IFF_TIFF_LZW},
        {"lzw",             IFF_TIFF_LZW},
        {"tiff-zip",        IFF_TIFF_ZIP},
        {"zip",             IFF_TIFF_ZIP},
        {"pnm",             IFF_PNM},
        {"pbm",             IFF_PNM},
        {"pgm",             IFF_PNM},
        {"ppm",             IFF_PNM},
        {"ps",              IFF_PS},
        {"gif",             IFF_GIF},
        {"jp2",             IFF_JP2},
        {"jpeg2k",          IFF_JP2},
        {"webp",            IFF_WEBP},
        {"lpdf",            IFF_LPDF},
        {"default",         IFF_DEFAULT},
        {"spix",            IFF_SPIX}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * @brief Return the name for an input file format (IFF_*)
 * \param format input file format
 * @return pointer to const string
 */
const char*
ll_string_input_format(int format)
{
    switch (format) {
    case IFF_UNKNOWN:
        return "unknown";
    case IFF_BMP:
        return "bmp";
    case IFF_JFIF_JPEG:
        return "jpeg";
    case IFF_PNG:
        return "png";
    case IFF_TIFF:
        return "tiff";
    case IFF_TIFF_PACKBITS:
        return "tiff-packbits";
    case IFF_TIFF_RLE:
        return "tiff-rle";
    case IFF_TIFF_G3:
        return "tiff-g3";
    case IFF_TIFF_G4:
        return "tiff-g4";
    case IFF_TIFF_LZW:
        return "tiff-lzw";
    case IFF_TIFF_ZIP:
        return "tiff-zip";
    case IFF_PNM:
        return "pnm";
    case IFF_PS:
        return "ps";
    case IFF_GIF:
        return "gif";
    case IFF_JP2:
        return "jp2";
    case IFF_WEBP:
        return "webp";
    case IFF_LPDF:
        return "lpdf";
    case IFF_DEFAULT:
        return "default";
    case IFF_SPIX:
        return "spix";
    }
    return "invalid";
}

/**
 * \brief Check for an L_AMAP keytype name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_keytype(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"int",             L_INT_TYPE},
        {"uint",            L_UINT_TYPE},
        {"float",           L_FLOAT_TYPE}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * @brief Return a string for the keytype of an ASET
 * \param type key type
 * @return const string with the name
 */
const char* ll_string_keytype(l_int32 type)
{
    switch (type) {
    case L_INT_TYPE:
        return "int";
    case L_UINT_TYPE:
        return "uint";
    case L_FLOAT_TYPE:
        return "float";
    }
    return "undefined";
}

/**
 * \brief Check for an choose name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_consecutive_skip_by(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"consecutive",     L_CHOOSE_CONSECUTIVE},
        {"cons",            L_CHOOSE_CONSECUTIVE},
        {"skip_by",         L_CHOOSE_SKIP_BY},
        {"skip-by",         L_CHOOSE_SKIP_BY},
        {"skip",            L_CHOOSE_SKIP_BY},
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for an component name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_component(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"red",             COLOR_RED},
        {"r",               COLOR_RED},
        {"green",           COLOR_GREEN},
        {"grn",             COLOR_GREEN},
        {"g",               COLOR_GREEN},
        {"blue",            COLOR_BLUE},
        {"blu",             COLOR_BLUE},
        {"b",               COLOR_BLUE},
        {"alpha",           L_ALPHA_CHANNEL},
        {"a",               L_ALPHA_CHANNEL}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for an min/max name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_choose_min_max(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"min",             L_CHOOSE_MIN},
        {"max",             L_CHOOSE_MAX}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a white or black is max name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_what_is_max(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"white-is-max",    L_WHITE_IS_MAX},
        {"white",           L_WHITE_IS_MAX},
        {"w",               L_WHITE_IS_MAX},
        {"black-is-max",    L_BLACK_IS_MAX},
        {"black",           L_BLACK_IS_MAX},
        {"b",               L_BLACK_IS_MAX}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a L_GET_XXXX_VAL name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_getval(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"white",           L_GET_WHITE_VAL},
        {"w",               L_GET_WHITE_VAL},
        {"black",           L_GET_BLACK_VAL},
        {"b",               L_GET_BLACK_VAL}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a L_XXX_LINE name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_direction(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"horizontal-line", L_HORIZONTAL_LINE},
        {"horizontal",      L_HORIZONTAL_LINE},
        {"horiz",           L_HORIZONTAL_LINE},
        {"h",               L_HORIZONTAL_LINE},
        {"vertical-line",   L_VERTICAL_LINE},
        {"vertical",        L_VERTICAL_LINE},
        {"vert",            L_VERTICAL_LINE},
        {"v",               L_VERTICAL_LINE}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a L_SET_XXXX name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_blackwhite(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"white",           L_SET_WHITE},
        {"w",               L_SET_WHITE},
        {"black",           L_SET_BLACK},
        {"b",               L_SET_BLACK}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a PIX_XXX name as string
 * <pre>
 *      PIX_CLR                           0000             0x0
 *      PIX_SET                           1111             0xf
 *      PIX_SRC                           1100             0xc
 *      PIX_DST                           1010             0xa
 *      PIX_NOT(PIX_SRC)                  0011             0x3
 *      PIX_NOT(PIX_DST)                  0101             0x5
 *      PIX_SRC | PIX_DST                 1110             0xe
 *      PIX_SRC & PIX_DST                 1000             0x8
 *      PIX_SRC ^ PIX_DST                 0110             0x6
 *      PIX_NOT(PIX_SRC) | PIX_DST        1011             0xb
 *      PIX_NOT(PIX_SRC) & PIX_DST        0010             0x2
 *      PIX_SRC | PIX_NOT(PIX_DST)        1101             0xd
 *      PIX_SRC & PIX_NOT(PIX_DST)        0100             0x4
 *      PIX_NOT(PIX_SRC | PIX_DST)        0001             0x1
 *      PIX_NOT(PIX_SRC & PIX_DST)        0111             0x7
 *      PIX_NOT(PIX_SRC ^ PIX_DST)        1001             0x9
 * </pre>
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rasterop(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"clr",                         PIX_CLR},
        {"set",                         PIX_SET},
        {"src",                         PIX_SRC},
        {"dst",                         PIX_DST},
        {"!src",                        PIX_NOT(PIX_SRC)},
        {"!dst",                        PIX_NOT(PIX_DST)},
        {"src|dst",                     PIX_SRC | PIX_DST},
        {"paint",                       PIX_SRC | PIX_DST},
        {"src&dst",                     PIX_SRC & PIX_DST},
        {"mask",                        PIX_SRC & PIX_DST},
        {"src^dst",                     PIX_SRC ^ PIX_DST},
        {"xor",                         PIX_SRC ^ PIX_DST},
        {"!src|dst",                    PIX_NOT(PIX_SRC) | PIX_DST},
        {"!src&dst",                    PIX_NOT(PIX_SRC) & PIX_DST},
        {"subtract",                    PIX_NOT(PIX_SRC) & PIX_DST},
        {"src|!dst",                    PIX_SRC | PIX_NOT(PIX_DST)},
        {"src&!dst",                    PIX_SRC & PIX_NOT(PIX_DST)},
        {"!(src|dst)",                  PIX_NOT(PIX_SRC | PIX_DST)},
        {"!(src&dst)",                  PIX_NOT(PIX_SRC & PIX_DST)},
        {"!(src^dst)",                  PIX_NOT(PIX_SRC ^ PIX_DST)}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a search direction name (%L_HORIZ, %L_VERT, or %L_BOTH_DIRECTIONS)
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_searchdir(lua_State *L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"horizontal",          L_HORIZ},
        {"horiz",               L_HORIZ},
        {"h",                   L_HORIZ},
        {"vertical",            L_VERT},
        {"vert",                L_VERT},
        {"v",                   L_VERT},
        {"both-directions",     L_BOTH_DIRECTIONS},
        {"both",                L_BOTH_DIRECTIONS},
        {"b",                   L_BOTH_DIRECTIONS}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a stats type name (%L_MEAN_ABSVAL, %L_ROOT_MEAN_SQUARE, %L_STANDARD_DEVIATION, %L_VARIANCE)
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_stats_type(lua_State* L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"mean-absval",         L_MEAN_ABSVAL},
        {"mean-abs",            L_MEAN_ABSVAL},
        {"mean",                L_MEAN_ABSVAL},
        {"m",                   L_MEAN_ABSVAL},
        {"root-mean-square",    L_ROOT_MEAN_SQUARE},
        {"rms",                 L_ROOT_MEAN_SQUARE},
        {"r",                   L_ROOT_MEAN_SQUARE},
        {"standard-deviation",  L_STANDARD_DEVIATION},
        {"stddev",              L_STANDARD_DEVIATION},
        {"s",                   L_STANDARD_DEVIATION},
        {"variance",            L_VARIANCE},
        {"var",                 L_VARIANCE},
        {"v",                   L_VARIANCE}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a select color name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_color(lua_State* L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"red",                 L_SELECT_RED},
        {"r",                   L_SELECT_RED},
        {"green",               L_SELECT_GREEN},
        {"grn",                 L_SELECT_GREEN},
        {"g",                   L_SELECT_GREEN},
        {"blue",                L_SELECT_BLUE},
        {"blu",                 L_SELECT_BLUE},
        {"b",                   L_SELECT_BLUE},
        {"min",                 L_SELECT_MIN},
        {"max",                 L_SELECT_MAX},
        {"average",             L_SELECT_AVERAGE},
        {"avg",                 L_SELECT_AVERAGE},
        {"hue",                 L_SELECT_HUE},
        {"saturation",          L_SELECT_SATURATION},
        {"sat",                 L_SELECT_SATURATION}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/**
 * \brief Check for a select min or max name (%L_SELECT_MIN, %L_SELECT_MAX)
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_min_max(lua_State* L, int arg, l_int32 dflt)
{
    static const key_value_t tbl[] = {
        {"min",                 L_SELECT_MIN},
        {"max",                 L_SELECT_MAX}
    };
    return ll_check_tbl(L, arg, dflt, tbl, sizeof(tbl)/sizeof(tbl[0]));
}

/*====================================================================*
 *
 *  Lua LEPT class
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_LEPT
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the LEPT contained in the user data
 */
static void **
ll_check_LEPT(lua_State *L, int arg)
{
    return ll_check_udata(L, arg, LL_LEPT);
}

/**
 * \brief Push LEPT* user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param lept pointer to the LEPT
 * \return 1 LEPT* on the Lua stack
 */
static int
ll_push_LEPT(lua_State *L, LEPT *lept)
{
    if (NULL == lept)
        return 0;
    return ll_push_udata(L, LL_LEPT, lept);
}

/**
 * \brief Create a new LEPT* user data
 * \param L pointer to the lua_State
 * \return 1 LEPT* on the Lua stack
 */
static int
ll_new_LEPT(lua_State *L)
{
    static const char leptonica[] = "leptonica-";
    LEPT *lept = (LEPT *) calloc(1, sizeof(LEPT));
    const char* version = getLeptonicaVersion();

    if (!strncmp(version, leptonica, strlen(leptonica)))
        version += strlen(leptonica);
    snprintf(lept->version, sizeof(lept->version), "%s", version);

    return ll_push_udata(L, LL_LEPT, lept);
}
/**
 * \brief Destroy a LEPT*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **plept = ll_check_udata(L, 1, LL_LEPT);
    DBG(LOG_DESTROY, "%s: '%s' plept=%p lept=%p\n", __func__,
        LL_LEPT, (void *)plept, *plept);
    free(*plept);
    *plept = NULL;
    return 0;
}

/**
 * \brief Return the Leptonica version number
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
Version(lua_State *L)
{
    void **plept = ll_check_LEPT(L, 1);
    LEPT *lept = *(LEPT **)plept;
    lua_pushstring(L, lept->version);
    return 1;
}

/**
 * \brief Compose a RGB pixel value
 *
 * Arg #1 is expected to be a l_int32 (rval)
 * Arg #2 is expected to be a l_int32 (gval)
 * Arg #3 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
RGB(lua_State *L)
{
    l_int32 rval = ll_check_l_int32(L, 1);
    l_int32 gval = ll_check_l_int32(L, 2);
    l_int32 bval = ll_check_l_int32(L, 3);
    l_uint32 pixel;
    if (composeRGBPixel(rval, gval, bval, &pixel))
        return 0;
    lua_pushinteger(L, pixel);
    return 1;
}

/**
 * \brief Compose a RGBA pixel value
 *
 * Arg #1 is expected to be a l_int32 (rval)
 * Arg #2 is expected to be a l_int32 (gval)
 * Arg #3 is expected to be a l_int32 (bval)
 * Arg #4 is expected to be a l_int32 (aval)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
RGBA(lua_State *L)
{
    l_int32 rval = ll_check_l_int32(L, 1);
    l_int32 gval = ll_check_l_int32(L, 2);
    l_int32 bval = ll_check_l_int32(L, 3);
    l_int32 aval = ll_check_l_int32(L, 3);
    l_uint32 pixel;
    if (composeRGBAPixel(rval, gval, bval, aval, &pixel))
        return 0;
    lua_pushinteger(L, pixel);
    return 1;
}

/**
 * \brief Extract a RGB pixel values
 *
 * Arg #1 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
ToRGB(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    extractRGBValues(pixel, &rval, &gval, &bval);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    return 3;
}

/**
 * \brief Extract a RGBA pixel values
 *
 * Arg #1 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
ToRGBA(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    extractRGBAValues(pixel, &rval, &gval, &bval, &aval);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    lua_pushinteger(L, aval);
    return 4;
}

/**
 * \brief Extract minimum or maximum component from pixel value
 *
 * Arg #1 is expected to be a l_uint32 (pixel)
 * Arg #2 is expected to be a string (min or max)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
MinMaxComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(L, 1);
    l_int32 type = ll_check_choose_min_max(L, 2, 0);
    lua_pushinteger(L, extractMinMaxComponent(pixel, type));
    return 1;
}

/**
 * \brief Extract minimum component from pixel value
 *
 * Arg #1 is expected to be a l_uint32 (pixel)
 * Arg #2 is expected to be a string (min or max)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
MinComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(L, 1);
    lua_pushinteger(L, extractMinMaxComponent(pixel, L_CHOOSE_MIN));
    return 1;
}

/**
 * \brief Extract maximum component from pixel value
 *
 * Arg #1 is expected to be a l_uint32 (pixel)
 * Arg #2 is expected to be a string (min or max)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
MaxComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(L, 1);
    lua_pushinteger(L, extractMinMaxComponent(pixel, L_CHOOSE_MAX));
    return 1;
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
static int register_LEPT(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"Version",                 Version},
        {LL_NUMA,                   ll_new_NUMA},
        {LL_NUMAA,                  ll_new_NUMAA},
        {LL_DNA,                    ll_new_DNA},
        {LL_DNAA,                   ll_new_DNAA},
        {LL_PTA,                    ll_new_PTA},
        {LL_PTAA,                   ll_new_PTAA},
        {LL_AMAP,                   ll_new_AMAP},
        {LL_ASET,                   ll_new_ASET},
        {LL_DLLIST,                 ll_new_DLLIST},
        {LL_BOX,                    ll_new_BOX},
        {LL_BOXA,                   ll_new_BOXA},
        {LL_BOXAA,                  ll_new_BOXAA},
        {LL_PIXCMAP,                ll_new_PIXCMAP},
        {LL_PIX,                    ll_new_PIX},
        {LL_PIXA,                   ll_new_PIXA},
        {LL_PIXAA,                  ll_new_PIXAA},
        {"RGB",                     RGB},
        {"RGBA",                    RGBA},
        {"ToRGB",                   ToRGB},
        {"ToRGBA",                  ToRGBA},
        {"MinComponent",            MinComponent},   /* alias without 2nd parameter */
        {"MaxComponent",            MaxComponent},   /* alias without 2nd parameter */
        {"MinMaxComponent",         MinMaxComponent},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    ll_register_NUMA(L);
    ll_register_NUMAA(L);
    ll_register_DNA(L);
    ll_register_DNAA(L);
    ll_register_PTA(L);
    ll_register_PTAA(L);
    ll_register_AMAP(L);
    ll_register_ASET(L);
    ll_register_DLLIST(L);
    ll_register_BOX(L);
    ll_register_BOXA(L);
    ll_register_BOXAA(L);
    ll_register_PIXCMAP(L);
    ll_register_PIX(L);
    ll_register_PIXA(L);
    ll_register_PIXAA(L);

    return ll_register_class(L, LL_LEPT, methods, functions);
}

l_int32
ll_RunScript(const char *script)
{
    lua_State *L;
    int res;

    /* Allow Leptonica debugging (pixDisplay ...) */
    setLeptDebugOK(TRUE);

    /* Allocate a new Lua state */
    L = luaL_newstate();

    /* Open all Lua libraries */
    luaL_openlibs(L);

    /* Register our libraries */
    register_LEPT(L);

    res = luaL_loadfile(L, script);
    if (LUA_OK != res) {
        const char* msg = lua_tostring(L, -1);
        lua_close(L);
        return ERROR_INT(msg, __func__, 1);
    }

    /* Create a global instance of the LL_LEPT */
    ll_new_LEPT(L);
    lua_setglobal(L, LL_LEPT);

    /* Ask Lua to run our script */
    res = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (LUA_OK != res) {
        const char* msg = lua_tostring(L, -1);
        lua_close(L);
        return ERROR_INT(msg, __func__, 1);
    }

    lua_close(L);
    return 0;
}
