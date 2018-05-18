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

#include "modules.h"

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(t) (sizeof(t)/sizeof(t[0]))
#endif

#if !defined(HAVE_FLOAT_H)
/* FIXME: how are these values really defined? */
static const unsigned long _flt_min = 0xfeffffffUL;
static const unsigned long _flt_max = 0x7effffffUL;
#define FLT_MIN (*(const float*)&_flt_min)
#define FLT_MAX (*(const float*)&_flt_max)
#endif

#if !defined(HAVE_CTYPE_H)
/**
 * \brief Poor man's toupper(3)
 * \param ch ASCII character
 * \return upper case value for ch
 */
static __inline int toupper(const int ch) { return (ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch; }
#endif

#if defined(HAVE_STRCASECMP)
#define ll_strcasecmp strcasecmp
#elif defined(HAVE_STRICMP)
#define ll_strcasecmp stricmp
#else
/**
 * \brief Our own version of strcasecmp(3)
 * \param dst first string
 * \param src second string
 * \return -1 if dst < src, +1 if dst > str, 0 otherwise
 */
int
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

#if defined(LLUA_DEBUG) && (LLUA_DEBUG>0)
void dbg(int enable, const char* format, ...)
{
    va_list ap;
    if (!enable)
        return;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
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
    luaL_setfuncs(L, functions, 0);
    DBG(LOG_REGISTER, "%s: '%s' registered with %d methods and %d functions\n",
         __func__, name, nmethods, nfunctions);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata with %name
 * \param L pointer to the lua_State
 * \param arg argument index
 * \param name tname of the expected udata
 * \return pointer to the udata
 */
void **
ll_check_udata(lua_State *L, int arg, const char* name)
{
    static char msg[128];
    void **ppvoid = (void **)luaL_checkudata(L, arg, name);
    snprintf(msg, sizeof(msg), "'%s' expected", name);
    luaL_argcheck(L, ppvoid != nullptr, arg, msg);
    return ppvoid;

}

/**
 * \brief Push user data %udata to the Lua stack and set its meta table %name
 * \param L pointer to the lua_State
 * \param name tname for the udata
 * \param udata pointer to the udata
 * \return 1 table on the stack
 */
int
ll_push_udata(lua_State *L, const char* name, void *udata)
{
    void **ppvoid = (void **)lua_newuserdata(L, sizeof(udata));
    *ppvoid = udata;
    lua_getfield(L, LUA_REGISTRYINDEX, name);
    lua_setmetatable(L, -2);
    DBG(LOG_CREATE, "%s: pushed '%s' ppvoid=%p udata=%p\n",
         __func__, name, ppvoid, udata);
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
ll_check_index(const char* func, lua_State *L, int arg, l_int32 imax)
{
    /* Lua indices are 1-based but Leptonica index is 0-based */
    lua_Integer index = luaL_checkinteger(L, arg) - 1;
    if (index < 0 || index >= imax) {
        lua_pushfstring(L, "%s: index #%d out of bounds (0 <= %d < %d)", func, arg, index, imax);
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
ll_check_l_int32(const char* func, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "%s: l_int32 #%d out of bounds (%d)", func, arg, val);
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
ll_check_l_int32_default(const char* func, lua_State *L, int arg, l_int32 dflt)
{
    lua_Integer val = luaL_optinteger(L, arg, dflt);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "%s: l_int32 #%d out of bounds (%d)", func, arg, val);
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
ll_check_l_uint32(const char* func, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "%s: l_uint32 #$d out of bounds (%d)", func, arg, val);
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
ll_check_l_uint32_default(const char* func, lua_State *L, int arg, l_uint32 dflt)
{
    lua_Integer val = luaL_optinteger(L, arg, dflt);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "%s: l_uint32 #%d out of bounds (%d)", func, arg, val);
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
ll_check_l_float32(const char* func, lua_State *L, int arg)
{
    lua_Number val = luaL_checknumber(L, arg);

    if (val < (lua_Number)-FLT_MAX || val > (lua_Number)FLT_MAX) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        func, arg, (lua_Number)-FLT_MAX, val, (lua_Number)FLT_MAX);
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
ll_check_l_float32_default(const char* func, lua_State *L, int arg, l_float32 dflt)
{
    lua_Number val = luaL_optnumber(L, arg, (lua_Number)dflt);

    if (val < (lua_Number)-FLT_MAX || val > (lua_Number)FLT_MAX) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        func, arg, (lua_Number)-FLT_MAX, val, (lua_Number)FLT_MAX);
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return (l_float32)val;
}

/**
 * \brief Push a string listing the table of keys to the Lua stack
 * \param L pointer to the lua_State
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \return value or dflt
 */
int
ll_push_tbl(lua_State *L, const lept_enums_t *tbl, size_t len)
{
    luaL_Buffer B;
    l_int32 value = -1;
    size_t i;

    luaL_buffinit(L, &B);

    for (i = 0; i < len; i++) {
        const lept_enums_t* p = &tbl[i];
        if (p->value != value) {
            if (i > 0)
                luaL_addchar(&B, '\n');
            luaL_addstring(&B, p->name);
            luaL_addstring(&B, ": ");
            value = p->value;
        } else {
            luaL_addstring(&B, ", ");
        }
        luaL_addstring(&B, p->key);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Return a const char* with the (first) key for a enumeration value
 * \param value value to search for
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \return pointer to string with the (first) key for that value
 */
const char*
ll_string_tbl(l_int32 value, const lept_enums_t *tbl, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {
        const lept_enums_t* p = &tbl[i];
        if (p->value == value)
            return p->key;
    }
    return "<undefined>";
}

/**
 * \brief Find a option %str in a key_value_t array %tbl of size %len
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \return value or dflt
 */
l_int32
ll_check_tbl(lua_State *L, int arg, l_int32 dflt, const lept_enums_t *tbl, size_t len)
{
    size_t i;

    const char* str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;
    if (!str)
        return dflt;

    for (i = 0; i < len; i++) {
        const lept_enums_t* p = &tbl[i];
        if (!ll_strcasecmp(str, p->key))
            return p->value;
    }

    ll_push_tbl(L, tbl, len);
    lua_pushfstring(L, "Invalid option #%d '%s'\n%s",
                    arg, str, lua_tostring(L, 1));
    lua_error(L);
    return dflt;    /* NOTREACHED */

}

#define TBL_ENTRY(key,ENUMVALUE) { key, #ENUMVALUE, ENUMVALUE }

/**
 * \brief Table of access/storage flag names and enumeration values
 */
static const lept_enums_t tbl_access_storage[] = {
    TBL_ENTRY("nocopy",         L_NOCOPY),      /* do not copy the object; do not delete the ptr */
    TBL_ENTRY("insert",         L_INSERT),      /* stuff it in; do not copy or clone */
    TBL_ENTRY("copy",           L_COPY),        /* make/use a copy of the object */
    TBL_ENTRY("clone",          L_CLONE),       /* make/use clone (ref count) of the object */
    TBL_ENTRY("copy-clone",     L_COPY_CLONE)   /* make a new array object (e.g., pixa) and fill the array with clones (e.g., pix) */
};

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
    return ll_check_tbl(L, arg, dflt, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Return the name for an access/storage flag value
 * \param flag access/storage flag
 * \return pointer to const string
 */
const char*
ll_string_access_storage(int flag)
{
    return ll_string_tbl(flag, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Table of file input format names and enumeration values
 */
static const lept_enums_t tbl_input_format[] = {
    TBL_ENTRY("unknown",         IFF_UNKNOWN),
    TBL_ENTRY("bmp",             IFF_BMP),
    TBL_ENTRY("jpg",             IFF_JFIF_JPEG),
    TBL_ENTRY("jpeg",            IFF_JFIF_JPEG),
    TBL_ENTRY("jfif",            IFF_JFIF_JPEG),
    TBL_ENTRY("png",             IFF_PNG),
    TBL_ENTRY("tiff",            IFF_TIFF),
    TBL_ENTRY("tif",             IFF_TIFF),
    TBL_ENTRY("tiff-packbits",   IFF_TIFF_PACKBITS),
    TBL_ENTRY("packbits",        IFF_TIFF_PACKBITS),
    TBL_ENTRY("tiff-rle",        IFF_TIFF_RLE),
    TBL_ENTRY("rle",             IFF_TIFF_RLE),
    TBL_ENTRY("tiff-g3",         IFF_TIFF_G3),
    TBL_ENTRY("g3",              IFF_TIFF_G3),
    TBL_ENTRY("tiff-g4",         IFF_TIFF_G4),
    TBL_ENTRY("g4",              IFF_TIFF_G4),
    TBL_ENTRY("tiff-lzw",        IFF_TIFF_LZW),
    TBL_ENTRY("lzw",             IFF_TIFF_LZW),
    TBL_ENTRY("tiff-zip",        IFF_TIFF_ZIP),
    TBL_ENTRY("zip",             IFF_TIFF_ZIP),
    TBL_ENTRY("pnm",             IFF_PNM),
    TBL_ENTRY("pbm",             IFF_PNM),
    TBL_ENTRY("pgm",             IFF_PNM),
    TBL_ENTRY("ppm",             IFF_PNM),
    TBL_ENTRY("ps",              IFF_PS),
    TBL_ENTRY("gif",             IFF_GIF),
    TBL_ENTRY("jp2",             IFF_JP2),
    TBL_ENTRY("jpeg2k",          IFF_JP2),
    TBL_ENTRY("webp",            IFF_WEBP),
    TBL_ENTRY("lpdf",            IFF_LPDF),
    TBL_ENTRY("default",         IFF_DEFAULT),
    TBL_ENTRY("spix",            IFF_SPIX)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Push a string listing the input format keys
 * \param L pointer to lua_State
 * \return 1 string on the Lua stack
 */
int
ll_print_input_format(lua_State *L)
{
    return ll_push_tbl(L, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Return the name for an input file format (IFF_*)
 * \param format input file format value
 * \return pointer to const string
 */
const char*
ll_string_input_format(int format)
{
    return ll_string_tbl(format, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Table of key type names for AMAP and ASET
 */
static const lept_enums_t tbl_keytype[] = {
    TBL_ENTRY("int",         L_INT_TYPE),
    TBL_ENTRY("uint",        L_UINT_TYPE),
    TBL_ENTRY("float",       L_FLOAT_TYPE)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Return a string for the keytype of an AMAP/ASET
 * \param type key type value
 * \return const string with the name
 */
const char*
ll_string_keytype(l_int32 type)
{
    return ll_string_tbl(type, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Table of choice names and enumeration values
 */
static const lept_enums_t tbl_consecutive_skip_by[] = {
    TBL_ENTRY("consecutive",     L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("cons",            L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("skip-by",         L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip_by",         L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip",            L_CHOOSE_SKIP_BY),
};

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
    return ll_check_tbl(L, arg, dflt, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Return a string for the choice between consecutive and skip_by
 * \param choice consecutive/skip_by enumeration value
 * \return const string with the name
 */
const char*
ll_string_consecutive_skip_by(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Table of color component names and enumeration values
 */
static const lept_enums_t tbl_component[] = {
    TBL_ENTRY("red",             COLOR_RED),
    TBL_ENTRY("r",               COLOR_RED),
    TBL_ENTRY("green",           COLOR_GREEN),
    TBL_ENTRY("grn",             COLOR_GREEN),
    TBL_ENTRY("g",               COLOR_GREEN),
    TBL_ENTRY("blue",            COLOR_BLUE),
    TBL_ENTRY("blu",             COLOR_BLUE),
    TBL_ENTRY("b",               COLOR_BLUE),
    TBL_ENTRY("alpha",           L_ALPHA_CHANNEL),
    TBL_ENTRY("a",               L_ALPHA_CHANNEL)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Return a string for the color component name
 * \param component color component value
 * \return const string with the name
 */
const char*
ll_string_component(l_int32 component)
{
    return ll_string_tbl(component, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Table of choice min/max names and enumeration values
 */
static const lept_enums_t tbl_choose_min_max[] = {
    TBL_ENTRY("min",             L_CHOOSE_MIN),
    TBL_ENTRY("max",             L_CHOOSE_MAX)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Return a string for the choice between min and max
 * \param choice min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_choose_min_max(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Table of white/black is max names and enumeration values
 */
static const lept_enums_t tbl_what_is_max[] = {
    TBL_ENTRY("white-is-max",    L_WHITE_IS_MAX),
    TBL_ENTRY("white",           L_WHITE_IS_MAX),
    TBL_ENTRY("w",               L_WHITE_IS_MAX),
    TBL_ENTRY("black-is-max",    L_BLACK_IS_MAX),
    TBL_ENTRY("black",           L_BLACK_IS_MAX),
    TBL_ENTRY("b",               L_BLACK_IS_MAX)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Return a string for the choice between min and max
 * \param what white or black is max enumeration value
 * \return const string with the name
 */
const char*
ll_string_what_is_max(l_int32 what)
{
    return ll_string_tbl(what, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Table of get white/black val names and enumeration values
 */
static const lept_enums_t tbl_getval[] = {
    TBL_ENTRY("white",           L_GET_WHITE_VAL),
    TBL_ENTRY("w",               L_GET_WHITE_VAL),
    TBL_ENTRY("black",           L_GET_BLACK_VAL),
    TBL_ENTRY("b",               L_GET_BLACK_VAL)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Return a string for the choice between min and max
 * \param val white or black getval enumeration value
 * \return const string with the name
 */
const char*
ll_string_getval(l_int32 val)
{
    return ll_string_tbl(val, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Table of direction names and enumeration values
 */
static const lept_enums_t tbl_direction[] = {
    TBL_ENTRY("horizontal-line", L_HORIZONTAL_LINE),
    TBL_ENTRY("horizontal",      L_HORIZONTAL_LINE),
    TBL_ENTRY("horiz",           L_HORIZONTAL_LINE),
    TBL_ENTRY("h",               L_HORIZONTAL_LINE),
    TBL_ENTRY("vertical-line",   L_VERTICAL_LINE),
    TBL_ENTRY("vertical",        L_VERTICAL_LINE),
    TBL_ENTRY("vert",            L_VERTICAL_LINE),
    TBL_ENTRY("v",               L_VERTICAL_LINE)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Return a string for the direction name
 * \param dir horizontal or vertical line direction enumeration value
 * \return const string with the name
 */
const char*
ll_string_direction(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Table of set white/black names and enumeration values
 */
static const lept_enums_t tbl_blackwhite[] = {
    TBL_ENTRY("white",           L_SET_WHITE),
    TBL_ENTRY("w",               L_SET_WHITE),
    TBL_ENTRY("black",           L_SET_BLACK),
    TBL_ENTRY("b",               L_SET_BLACK)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_blackwhite, ARRAYSIZE(tbl_blackwhite));
}

/**
 * \brief Return a string for the choice between setting black or white
 * \param which set black or white enumeration value
 * \return const string with the name
 */
const char*
ll_string_blackwhite(l_int32 which)
{
    return ll_string_tbl(which, tbl_blackwhite, ARRAYSIZE(tbl_blackwhite));
}

/**
 * \brief Table of rasterop names and enumeration values
 */
static const lept_enums_t tbl_rasterop[] = {
    TBL_ENTRY("clr",             PIX_CLR),
    TBL_ENTRY("set",             PIX_SET),
    TBL_ENTRY("src",             PIX_SRC),
    TBL_ENTRY("dst",             PIX_DST),
    TBL_ENTRY("!src",            PIX_NOT(PIX_SRC)),
    TBL_ENTRY("!dst",            PIX_NOT(PIX_DST)),
    TBL_ENTRY("src|dst",         PIX_SRC | PIX_DST),
    TBL_ENTRY("paint",           PIX_SRC | PIX_DST),
    TBL_ENTRY("src&dst",         PIX_SRC & PIX_DST),
    TBL_ENTRY("mask",            PIX_SRC & PIX_DST),
    TBL_ENTRY("src^dst",         PIX_SRC ^ PIX_DST),
    TBL_ENTRY("xor",             PIX_SRC ^ PIX_DST),
    TBL_ENTRY("!src|dst",        PIX_NOT(PIX_SRC) | PIX_DST),
    TBL_ENTRY("!src&dst",        PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("subtract",        PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("src|!dst",        PIX_SRC | PIX_NOT(PIX_DST)),
    TBL_ENTRY("src&!dst",        PIX_SRC & PIX_NOT(PIX_DST)),
    TBL_ENTRY("!(src|dst)",      PIX_NOT(PIX_SRC | PIX_DST)),
    TBL_ENTRY("!(src&dst)",      PIX_NOT(PIX_SRC & PIX_DST)),
    TBL_ENTRY("!(src^dst)",      PIX_NOT(PIX_SRC ^ PIX_DST))
};

/**
 * \brief Check for a rasterop name as string
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param dflt default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rasterop(lua_State *L, int arg, l_int32 dflt)
{
    return ll_check_tbl(L, arg, dflt, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Return a string for the raster operation
 * \param op enumeration value of the raster operation
 * \return const string with the name
 */
const char*
ll_string_rasterop(l_int32 op)
{
    return ll_string_tbl(op, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Table of search direction names and enumeration values
 */
static const lept_enums_t tbl_searchdir[] = {
    TBL_ENTRY("horizontal",          L_HORIZ),
    TBL_ENTRY("horiz",               L_HORIZ),
    TBL_ENTRY("h",                   L_HORIZ),
    TBL_ENTRY("vertical",            L_VERT),
    TBL_ENTRY("vert",                L_VERT),
    TBL_ENTRY("v",                   L_VERT),
    TBL_ENTRY("both-directions",     L_BOTH_DIRECTIONS),
    TBL_ENTRY("both",                L_BOTH_DIRECTIONS),
    TBL_ENTRY("b",                   L_BOTH_DIRECTIONS)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Return a string for the search direction
 * \param dir enumeration value of the search direction
 * \return const string with the name
 */
const char*
ll_string_searchir(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Table of stats type names and enumeration values
 */
static const lept_enums_t tbl_stats_type[] = {
    TBL_ENTRY("mean-absval",         L_MEAN_ABSVAL),
    TBL_ENTRY("mean-abs",            L_MEAN_ABSVAL),
    TBL_ENTRY("mean",                L_MEAN_ABSVAL),
    TBL_ENTRY("m",                   L_MEAN_ABSVAL),
    TBL_ENTRY("root-mean-square",    L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("rms",                 L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("r",                   L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("standard-deviation",  L_STANDARD_DEVIATION),
    TBL_ENTRY("stddev",              L_STANDARD_DEVIATION),
    TBL_ENTRY("s",                   L_STANDARD_DEVIATION),
    TBL_ENTRY("variance",            L_VARIANCE),
    TBL_ENTRY("var",                 L_VARIANCE),
    TBL_ENTRY("v",                   L_VARIANCE)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Return a string for the stats type
 * \param type enumeration value of the stats type
 * \return const string with the name
 */
const char*
ll_string_stats_type(l_int32 type)
{
    return ll_string_tbl(type, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Table of select color names and enumeration values
 */
static const lept_enums_t tbl_select_color[] = {
    TBL_ENTRY("red",                 L_SELECT_RED),
    TBL_ENTRY("r",                   L_SELECT_RED),
    TBL_ENTRY("green",               L_SELECT_GREEN),
    TBL_ENTRY("grn",                 L_SELECT_GREEN),
    TBL_ENTRY("g",                   L_SELECT_GREEN),
    TBL_ENTRY("blue",                L_SELECT_BLUE),
    TBL_ENTRY("blu",                 L_SELECT_BLUE),
    TBL_ENTRY("b",                   L_SELECT_BLUE),
    TBL_ENTRY("min",                 L_SELECT_MIN),
    TBL_ENTRY("max",                 L_SELECT_MAX),
    TBL_ENTRY("average",             L_SELECT_AVERAGE),
    TBL_ENTRY("avg",                 L_SELECT_AVERAGE),
    TBL_ENTRY("hue",                 L_SELECT_HUE),
    TBL_ENTRY("saturation",          L_SELECT_SATURATION),
    TBL_ENTRY("sat",                 L_SELECT_SATURATION)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Return a string for the selected color
 * \param color selected color enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_color(l_int32 color)
{
    return ll_string_tbl(color, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Table of select min/max names and enumeration values
 */
static const lept_enums_t tbl_select_minmax[] = {
    TBL_ENTRY("min",                 L_SELECT_MIN),
    TBL_ENTRY("max",                 L_SELECT_MAX)
};

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
    return ll_check_tbl(L, arg, dflt, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/**
 * \brief Return a string for the selection minimum or maximum
 * \param color selected color enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_min_max(l_int32 which)
{
    return ll_string_tbl(which, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/*====================================================================*
 *
 *  Lua LEPT class
 *
 *====================================================================*/

/**
 * \brief Check Lua stack at index %arg for udata of class LL_LEPT
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
 * \brief Push LEPT* to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param lept pointer to the LEPT
 * \return 1 LEPT* on the Lua stack
 */
static int
ll_push_LEPT(lua_State *L, LuaLept *lept)
{
    if (!lept)
        return 0;
    return ll_push_udata(L, LL_LEPT, lept);
}

/**
 * \brief Create a new LEPT*
 * \param L pointer to the lua_State
 * \return 1 LEPT* on the Lua stack
 */
static int
ll_new_LEPT(lua_State *L)
{
    static const char lept_prefix[] = "leptonica-";
    LuaLept *lept = (LuaLept *) LEPT_CALLOC(1, sizeof(LuaLept));
    const char* lept_ver = getLeptonicaVersion();
    const lua_Number *lua_ver = lua_version(L);

    snprintf(lept->str_version, sizeof(lept->str_version), "%s", PACKAGE_VERSION);
    snprintf(lept->str_version_lua, sizeof(lept->str_version_lua), "%d.%d",
             ((int)lua_ver[0])/100, ((int)lua_ver[0])%100);
    if (!strncmp(lept_ver, lept_prefix, strlen(lept_prefix)))
        lept_ver += strlen(lept_prefix);
    snprintf(lept->str_version_lept, sizeof(lept->str_version_lept), "%s", lept_ver);

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
    DBG(LOG_DESTROY, "%s: '%s' plept=%p lept=%p\n",
         __func__, LL_LEPT, plept, *plept);
    free(*plept);
    *plept = nullptr;
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
    LuaLept *lept = *(LuaLept **)plept;
    lua_pushstring(L, lept->str_version);
    return 1;
}

/**
 * \brief Return the Leptonica version number
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
LuaVersion(lua_State *L)
{
    void **plept = ll_check_LEPT(L, 1);
    LuaLept *lept = *(LuaLept **)plept;
    lua_pushstring(L, lept->str_version_lua);
    return 1;
}

/**
 * \brief Return the Leptonica version number
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
LeptVersion(lua_State *L)
{
    void **plept = ll_check_LEPT(L, 1);
    LuaLept *lept = *(LuaLept **)plept;
    lua_pushstring(L, lept->str_version_lept);
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
 * \return 1 Pix* on the Lua stack
 */
static int
RGB(lua_State *L)
{
    l_int32 rval = ll_check_l_int32(__func__, L, 1);
    l_int32 gval = ll_check_l_int32(__func__, L, 2);
    l_int32 bval = ll_check_l_int32(__func__, L, 3);
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
 * \return 1 Pix* on the Lua stack
 */
static int
RGBA(lua_State *L)
{
    l_int32 rval = ll_check_l_int32(__func__, L, 1);
    l_int32 gval = ll_check_l_int32(__func__, L, 2);
    l_int32 bval = ll_check_l_int32(__func__, L, 3);
    l_int32 aval = ll_check_l_int32(__func__, L, 3);
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
 * \return 1 Pix* on the Lua stack
 */
static int
ToRGB(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(__func__, L, 1);
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
 * \return 1 Pix* on the Lua stack
 */
static int
ToRGBA(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(__func__, L, 1);
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
 * \return 1 Pix* on the Lua stack
 */
static int
MinMaxComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(__func__, L, 1);
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
 * \return 1 Pix* on the Lua stack
 */
static int
MinComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(__func__, L, 1);
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
 * \return 1 Pix* on the Lua stack
 */
static int
MaxComponent(lua_State *L)
{
    l_uint32 pixel = ll_check_l_uint32(__func__, L, 1);
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
        {"LuaVersion",              LuaVersion},
        {"LeptVersion",             LeptVersion},
        {LL_NUMA,                   ll_new_Numa},
        {LL_NUMAA,                  ll_new_Numaa},
        {LL_DNA,                    ll_new_Dna},
        {LL_DNAA,                   ll_new_Dnaa},
        {LL_PTA,                    ll_new_Pta},
        {LL_PTAA,                   ll_new_Ptaa},
        {LL_AMAP,                   ll_new_Amap},
        {LL_ASET,                   ll_new_Aset},
        {LL_DLLIST,                 ll_new_DoubleLinkedList},
        {LL_BOX,                    ll_new_Box},
        {LL_BOXA,                   ll_new_Boxa},
        {LL_BOXAA,                  ll_new_Boxaa},
        {LL_PIXCMAP,                ll_new_PixColormap},
        {LL_PIX,                    ll_new_Pix},
        {LL_PIXA,                   ll_new_Pixa},
        {LL_PIXAA,                  ll_new_Pixaa},
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

    ll_register_Numa(L);
    ll_register_Numaa(L);
    ll_register_Dna(L);
    ll_register_Dnaa(L);
    ll_register_Pta(L);
    ll_register_Ptaa(L);
    ll_register_Amap(L);
    ll_register_Aset(L);
    ll_register_DoubleLinkedList(L);
    ll_register_Box(L);
    ll_register_BOXA(L);
    ll_register_Boxaa(L);
    ll_register_PixColormap(L);
    ll_register_Pix(L);
    ll_register_Pixa(L);
    ll_register_Pixaa(L);

    return ll_register_class(L, LL_LEPT, methods, functions);
}

int
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