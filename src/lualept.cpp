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

/**
 * \mainpage LuaLept
 *
 * ## Lua language bindings for Leptonica.
 *
 * This is a WIP (work in progress) to create a Lua (https://www.lua.org/) wrapper
 * for Leptonica (http://leptonica.org/).
 *
 * The project's goal is to cover most if not all of Leptonica's huge number of
 * types and functions in a marriage of Lua's and Leptonica's concepts.
 *
 * This is my first *larger* Lua and C-Functions project and I may well be doing some
 * things in an awkward way or even outright wrong, so feel free to point your fingers
 * at me and tell me where I'm missing the point.
 *
 * Still, the project already runs some script (lua/script.lua) which I use for testing
 * the bindings as I write the wrappers.
 *
 * The globals (or Lua <i>classes</i>) defined by this library currently are:
 * - LuaLept the main class
 * - Amap
 * - Aset
 * - ByteBuffer
 * - Bmf
 * - Box
 * - Boxa
 * - Boxaa
 * - CompData
 * - CCBord
 * - CCBorda
 * - Dewarp
 * - Dewarpa
 * - DLList
 * - Dna
 * - Dnaa
 * - DPix
 * - FPix
 * - FPixa
 * - Kernel
 * - Numa
 * - Numaa
 * - PdfData
 * - Pix
 * - Pixa
 * - Pixaa
 * - PixColormap
 * - PixTiling
 * - PixComp
 * - PixaComp
 * - Pta
 * - Ptaa
 * - RbtreeNode
 * - Sarray
 * - Sel
 * - Sela
 * - Stack
 * - WShed
 *
 * Jürgen Buchmüller <pullmoll@t-online.de>
 */

#if !defined(HAVE_FLOAT_H)
/* FIXME: how are these values really defined? */
static const unsigned long _flt_min = 0xfeffffffUL;
static const unsigned long _flt_max = 0x7effffffUL;
#define FLT_MIN (*(const float*)&_flt_min)
#define FLT_MAX (*(const float*)&_flt_max)
#endif

#if defined(_MSC_VER)
/** This symbol is expected when linking agains Leptonica? */
int LeptMsgSeverity = 0;
#endif

#if !defined(HAVE_CTYPE_H)
/**
 * \brief Poor man's toupper(3).
 * \param ch ASCII character
 * \return upper case value for ch
 */
static __inline int toupper(const int ch) { return (ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch; }
#endif


#if defined(LUALEPT_DEBUG) && (LUALEPT_DEBUG>0)

/**
 * \brief Bit mask (flags) for enabled log output
 */
static int dbg_enabled = LOG_REGISTER | LOG_SDL2 | LOG_NEW_CLASS | LOG_NEW_PARAM | LOG_TAKE;

/**
 * \brief Return a time stamp for the current date and time
 * \return Temporary string with date and time
 */
static const char*
timestamp(void)
{
    static char str[128];
#if defined(HAVE_GETTIMEOFDAY)
    const char* tz = "";
    struct timeval tv;
    if (0 == gettimeofday(&tv, nullptr)) {
        time_t tval = static_cast<time_t>(tv.tv_sec);
        struct tm tm;
#if defined(HAVE_LOCALTIME_R)
        localtime_r(&tval, &tm);
#elif defined(HAVE_LOCALTIME)
        memcpy(&tm, localtime(&tval), sizeof(tm));
#elif defined(HAVE_GMTIME_R)
        gmtime_r(&tval, &tm);
        tz = "UTC";
#elif defined(HAVE_GMTIME)
        memcpy(&tm, gmtime(&tval), sizeof(tm));
        tz = "UTC";
#endif
        snprintf(str, sizeof(str),
                 "[%04d-%02d-%02d %02d:%02d:%02d.%04d%s] ",
                 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec,
                 static_cast<int>(tv.tv_usec / 100), tz);
    }
#else
    snprintf(str, sizeof(str), "[debug] ");
#endif
    return str;
}

/**
 * \brief Print debug output to stdout
 * \param enable bit mask (flag) which defines the type of log output
 * \param format format string followed by optional varargs
 */
void
dbg(int enable, const char* format, ...)
{
    va_list ap;
    if (0 == (enable & dbg_enabled))
        return;
    fprintf(stdout, "%s", timestamp());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
    fflush(stdout);
}
#endif

/**
 * \brief Die when memory allocation fails.
 * \param _fun calling function's name
 * \param L Lua state
 * \param format format string followed by parameters
 */
void
die(const char* _fun, lua_State *L, const char *format, ...)
{
    char str[256];
    va_list ap;
    va_start(ap, format);
    vsnprintf(str, sizeof(str), format, ap);
    va_end(ap);
    lua_pushfstring(L, "%s: %s", _fun, str);
    lua_error(L);
}

/**
 * \brief Check Lua stack at index %arg for light user data.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return pointer to the udata
 */
void *
ll_ludata(const char *_fun, lua_State *L, int arg)
{
    void *ptr = lua_islightuserdata(L, arg) ? lua_touserdata(L, arg) : nullptr;
    UNUSED(_fun);
    return ptr;
}

/**
 * \brief Check Lua stack at index %arg for udata with %tname.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \param tname tname of the expected udata
 * \return pointer to the udata
 */
void **
ll_udata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    char msg[128];
    void **pptr = nullptr;
    if (0 == strcmp(tname, "*")) {
        /* Wildcard: take any type */
        pptr = reinterpret_cast<void **>(lua_touserdata(L, arg));
    } else {
        pptr = reinterpret_cast<void **>(luaL_checkudata(L, arg, tname));
    }
    if (nullptr == pptr) {
        snprintf(msg, sizeof(msg), "%s: expected '%s'", _fun, tname);
    }
    luaL_argcheck(L, pptr != nullptr, arg, msg);
    return pptr;
}

/**
 * \brief Test if Lua stack at index %arg is a number.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return TRUE if there is a number at %arg, FALSE otherwise
 */
int
ll_isnumber(const char *_fun, lua_State *L, int arg)
{
    int res = lua_isnumber(L, arg);
    DBG(LOG_CHECK_INTEGER, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Test if Lua stack at index %arg is a string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return TRUE if there is a string at %arg, FALSE otherwise
 */
int
ll_isstring(const char *_fun, lua_State *L, int arg)
{
    int res = lua_isstring(L, arg);
    DBG(LOG_CHECK_STRING, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Test if Lua stack at index %arg is a C function.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return TRUE if there is a C function at %arg, FALSE otherwise
 */
int
ll_iscfunction(const char *_fun, lua_State *L, int arg)
{
    int res = lua_isstring(L, arg);
    DBG(LOG_CHECK_STRING, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Test if Lua stack at index %arg is an integer.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return TRUE if there is an integer at %arg, FALSE otherwise
 */
int
ll_isinteger(const char *_fun, lua_State *L, int arg)
{
    int res = lua_isinteger(L, arg);
    DBG(LOG_CHECK_INTEGER, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Test if Lua stack at index %arg is a table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \return TRUE if there is udata of type tname at %arg, FALSE otherwise
 */
int
ll_istable(const char *_fun, lua_State *L, int arg)
{
    int res = lua_istable(L, arg);
    DBG(LOG_CHECK_TABLE, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Test if Lua stack at index %arg is udata of type %tname.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg argument index
 * \param tname tname to check for
 * \return TRUE if there is udata of type tname at %arg, FALSE otherwise
 */
int
ll_isudata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    void **pptr = reinterpret_cast<void **>(luaL_testudata(L, arg, tname));
    int res = nullptr != pptr;
    DBG(LOG_CHECK_UDATA, "%s: res=%s\n", _fun, res ? "TRUE" : "FALSE");
    return res;
}

/**
 * \brief Free memory allocated by LEPT_MALLOC/LEPT_CALLOC.
 * \param ptr pointer to memory
 */
void
ll_free(void *ptr)
{
    LEPT_FREE(ptr);
}

/**
 * Register a luaL_Reg table of methods using a metatable
 * \param _fun calling function's name
 * \param L Lua state
 * \param tname table name for the udata
 * \param methods pointer to an array of luaL_Reg
 */
int
ll_register_class(const char *_fun, lua_State *L, const char *tname, const luaL_Reg *methods)
{
    luaL_Reg functions[1] = {
        LUA_SENTINEL
    };
    int nm;
    UNUSED(_fun);

    for (nm = 0; methods[nm].name; nm++)
        ;
    luaL_newmetatable(L, tname);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, methods, 0);
    lua_createtable(L, 0, 0);
    luaL_setfuncs(L, functions, 0);
    DBG(LOG_REGISTER, "%s: registered '%s' with %d methods\n", _fun,
        tname, nm);
    return 1;
}

/**
 * \brief Create a global lua_CFunction (%cfunct) with name (%tname)
 * \param _fun calling function's name
 * \param L Lua state
 * \param tname table name for the udata
 * \param cfunct lua_Cfunction to register
 * \return 0 for nothing on the Lua stack
 */
int
ll_set_global_cfunct(const char *_fun, lua_State *L, const char* tname, lua_CFunction cfunct)
{
    lua_pushcfunction(L, cfunct);
    lua_setglobal(L, tname);
    DBG(LOG_REGISTER, "%s: set global '%s' for lua_CFunction (%p)\n", _fun,
        tname, reinterpret_cast<void *>(cfunct));
    return 0;
}

/**
 * \brief Create a global table with name (%tname)
 * \param _fun calling function's name
 * \param L Lua state
 * \param tname table name for the udata
 * \param cfunct C function which creates a table on the Lua stack
 * \return 0 for nothing on the Lua stack
 */
int
ll_set_global_table(const char *_fun, lua_State *L, const char* tname, lua_CFunction cfunct)
{
    (*cfunct)(L);
    lua_setglobal(L, tname);
    DBG(LOG_REGISTER, "%s: set global '%s' for lua_CFunction (%p) result\n", _fun,
        tname, reinterpret_cast<void *>(cfunct));
    return 0;
}

/**
 * \brief Push user data %udata to the Lua stack and set its meta table %name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param name tname for the udata
 * \param udata pointer to the udata
 * \return 1 table on the stack
 */
int
ll_push_udata(const char *_fun, lua_State *L, const char* name, void *udata)
{
    void **ppvoid = reinterpret_cast<void **>(lua_newuserdata(L, sizeof(udata)));
    *ppvoid = udata;
    (void)_fun;
    lua_getfield(L, LUA_REGISTRYINDEX, name);
    lua_setmetatable(L, -2);
    DBG(LOG_PUSH_UDATA, "%s: pushed '%s' ppvoid=%p udata=%p\n",
        _fun, name ? name : "<nil>",
        reinterpret_cast<void *>(ppvoid),
        reinterpret_cast<void *>(udata));
    return 1;
}

/**
 * \brief Push nil to the Lua stack and return 1.
 * \param L Lua state
 * \return 1 nil on the Lua stack
 */
int
ll_push_nil(lua_State *L)
{
    lua_pushnil(L);
    return 1;
}

/**
 * \brief Push boolean (%b) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param b boolean value TRUE or FALSE
 * \return 1 boolean on the Lua stack
 */
int
ll_push_boolean(const char* _fun, lua_State *L, bool b)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_BOOLEAN, "%s: push %s\n", _fun, b ? "TRUE" : "FALSE");
    lua_pushboolean(L, b ? TRUE : FALSE);
    return 1;
}

/**
 * \brief Push l_uint8 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint8 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_int8(const char* _fun, lua_State *L, l_int8 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %d\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_uint8 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint8 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_uint8(const char* _fun, lua_State *L, l_uint8 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %u\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_int16 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint16 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_int16(const char* _fun, lua_State *L, l_int16 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %d\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_uint16 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint16 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_uint16(const char* _fun, lua_State *L, l_uint16 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %u\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_int32 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_int32 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_int32(const char* _fun, lua_State *L, l_int32 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %d\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_uint32 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint32 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_uint32(const char* _fun, lua_State *L, l_uint32 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %u\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_int64 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_int64 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_int64(const char* _fun, lua_State *L, l_int64 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %" PRId64 "\n", _fun, val);
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push l_uint64 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_uint64 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_uint64(const char* _fun, lua_State *L, l_uint64 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %" PRIu64 "\n", _fun,
        static_cast<l_uintptr_t>(val));
    lua_pushinteger(L, static_cast<lua_Integer>(val));
    return 1;
}

/**
 * \brief Push size_t (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param size size_t value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_size_t(const char* _fun, lua_State *L, size_t size)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %" PRIu64 "\n", _fun,
        static_cast<l_uintptr_t>(size));
    lua_pushinteger(L, static_cast<lua_Integer>(size));
    return 1;
}

/**
 * \brief Push l_float32 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_float32 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_float32(const char* _fun, lua_State *L, l_float32 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %g\n", _fun,
        static_cast<double>(val));
    lua_pushnumber(L, static_cast<lua_Number>(val));
    return 1;
}

/**
 * \brief Push l_float64 (%val) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param val l_float64 value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_l_float64(const char* _fun, lua_State *L, l_float64 val)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_INTEGER, "%s: push %g\n", _fun, val);
    lua_pushnumber(L, static_cast<lua_Number>(val));
    return 1;
}

/**
 * \brief Push string (%str) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param str string value
 * \return 1 boolean on the Lua stack
 */
int
ll_push_string(const char* _fun, lua_State *L, const char* str)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_STRING, "%s: push %s = %p %s = %" PRIu64 "\n", _fun,
        "str", reinterpret_cast<const void *>(str),
        "len", static_cast<l_uintptr_t>(strlen(str)));
    lua_pushstring(L, str);
    return 1;
}

/**
 * \brief Push lstring (%str) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param str lstring value
 * \param len lstring length
 * \return 1 boolean on the Lua stack
 */
int
ll_push_lstring(const char* _fun, lua_State *L, const char* str, size_t len)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_STRING, "%s: push %s = %p %s = %" PRIu64 "\n", _fun,
        "str", reinterpret_cast<const void *>(str),
        "len", static_cast<l_uintptr_t>(len));
    lua_pushlstring(L, str, len);
    return 1;
}

/**
 * \brief Push bytes (%data, %nbytes) as lstring to the Lua stack, free %data and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param data lstring value (array of bytes)
 * \param nbytes lstring length
 * \return 1 boolean on the Lua stack
 */
int
ll_push_bytes(const char* _fun, lua_State *L, l_uint8* data, size_t nbytes)
{
    UNUSED(_fun);
    DBG(LOG_PUSH_STRING, "%s: push %s = %p %s = %" PRIu64 "\n", _fun,
        "data", reinterpret_cast<const void *>(data),
        "nbytes", static_cast<l_uintptr_t>(nbytes));
    lua_pushlstring(L, reinterpret_cast<const char *>(data), nbytes);
    ll_free(data);
    return 1;
}

/**
 * \brief Push a l_int32 array (%ia) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param iarray pointer to the l_int32 array
 * \param n number of values in the array
 * \return 1 table on the stack
 */
int
ll_pack_Iarray(const char* _fun, lua_State *L, const l_int32 *iarray, l_int32 n)
{
    l_int32 i;
    UNUSED(_fun);
    if (!n || !iarray)
        return ll_push_nil(L);
    lua_newtable(L);
    for (i = 0; i < n; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s[%d] = 0x%08x\n", _fun,
            "iarray", i, iarray[i]);
        lua_pushinteger(L, iarray[i]);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_uint32 array (%ua) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param uarray pointer to the l_uint32 array
 * \param n number of values in the array
 * \return 1 table on the stack
 */
int
ll_pack_Uarray(const char* _fun, lua_State *L, const l_uint32 *uarray, l_int32 n)
{
    l_int32 i;
    UNUSED(_fun);
    if (!n || !uarray)
        return ll_push_nil(L);
    lua_newtable(L);
    for (i = 0; i < n; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s[%d] = 0x%08x\n", _fun,
            "uarray", i, uarray[i]);
        lua_pushinteger(L, uarray[i]);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_uint32 2D array (%data) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param data pointer to the l_uint32 array
 * \param wpl number of words in the row
 * \param h number of rows
 * \return 1 table containing (h) tables of (wpl) words on the stack
 */
int
ll_pack_Uarray_2d(const char* _fun, lua_State *L, const l_uint32 *data, l_int32 wpl, l_int32 h)
{
    l_int32 i;
    lua_newtable(L);
    for (i = 0; i < h; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
            "row", i,
            "data", reinterpret_cast<const void *>(data));
        ll_pack_Uarray(_fun, L, data, wpl);
        data += wpl;
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_float32 array (%fa) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param farray pointer to the l_float32 array
 * \param n number of values in the array
 * \return 1 table on the stack
 */
int
ll_pack_Farray(const char* _fun, lua_State *L, const l_float32 *farray, l_int32 n)
{
    l_int32 i;
    UNUSED(_fun);
    if (!n || !farray)
        return ll_push_nil(L);
    lua_newtable(L);
    for (i = 0; i < n; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s[%d] = %.8g\n", _fun,
            "farray", i, static_cast<lua_Number>(farray[i]));
        lua_pushnumber(L, static_cast<lua_Number>(farray[i]));
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_float32 2D array (%data) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param data pointer to the l_float32 array
 * \param wpl number of words in the row
 * \param h number of rows
 * \return 1 table containing (h) tables of (wpl) numbers on the stack
 */
int
ll_pack_Farray_2d(const char* _fun, lua_State *L, const l_float32 *data, l_int32 wpl, l_int32 h)
{
    l_int32 i;
    lua_newtable(L);
    for (i = 0; i < h; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
            "row", i,
            "data", reinterpret_cast<const void *>(data));
        ll_pack_Farray(_fun, L, data, wpl);
        data += wpl;
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_float64 array (%da) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param darray pointer to the l_float32 array
 * \param n number of values in the array
 * \return 1 table on the stack
 */
int
ll_pack_Darray(const char* _fun, lua_State *L, const l_float64 *darray, l_int32 n)
{
    l_int32 i;
    UNUSED(_fun);
    if (!n || !darray)
        return ll_push_nil(L);
    lua_newtable(L);
    for (i = 0; i < n; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s[%d] = %.8g\n", _fun,
            "darray", i, darray[i]);
        lua_pushnumber(L, darray[i]);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a l_float64 2D array (%data) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param data pointer to the l_float32 array
 * \param wpl number of words in the row
 * \param h number of rows
 * \return 1 table containing (h) tables of (wpl) numbers on the stack
 */
int
ll_pack_Darray_2d(const char* _fun, lua_State *L, const l_float64 *data, l_int32 wpl, l_int32 h)
{
    l_int32 i;
    lua_newtable(L);
    for (i = 0; i < h; i++) {
        DBG(LOG_PUSH_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
            "row", i,
            "data", reinterpret_cast<const void *>(data));
        ll_pack_Darray(_fun, L, data, wpl);
        data += wpl;
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Push a string array Sarray* (%sa) to the Lua stack and return 1.
 * \param _fun calling function's name
 * \param L Lua state
 * \param sa pointer to the Sarray
 * \return 1 table on the stack
 */
int
ll_pack_Sarray(const char* _fun, lua_State *L, Sarray *sa)
{
    l_int32 n = sarrayGetCount(sa);
    l_int32 i;
    UNUSED(_fun);
    if (!n || !sa)
        return ll_push_nil(L);
    lua_newtable(L);
    for (i = 0; i < n; i++) {
        const char* str = sarrayGetString(sa, i, L_NOCOPY);
        DBG(LOG_PUSH_ARRAY, "%s: %s[%d] = %p\n", _fun,
            "sarray", i, reinterpret_cast<const void *>(str));
        lua_pushstring(L, str);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

/**
 * \brief Unpack an array of lua_Integer from the Lua stack as l_int32*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param plen pointer to a l_int32 receiving the size of the array
 * \return allocated array l_int32* with *pn entries
 */
l_int32 *
ll_unpack_Iarray(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
        "arg", arg,
        "plen", reinterpret_cast<void *>(plen));
    l_int32 len = static_cast<l_int32>(luaL_len(L, arg));
    l_int32 *ia = ll_calloc<l_int32>(_fun, L, len);

    /* verify there is a table at %arg */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table */
    while (lua_next(L, arg)) {
        l_int32 key = ll_check_l_int32(_fun, L, -2);        /* key is at index -2 */
        l_int32 value = ll_check_l_int32(_fun, L, -1);      /* value is at index -1 */
        /* don't write out of bounds */
        if (0 < key && key <= len) {
            ia[key-1] = value;
        } else {
            /* FIXME: error? */
        }
        /* remove value; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (plen)
        *plen = len;
    return ia;
}

/**
 * \brief Unpack an array of lua_Integer from the Lua stack as l_uint32*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param plen pointer to a l_int32 receiving the size of the array
 * \return allocated array l_uint32* with *pn entries
 */
l_uint32 *
ll_unpack_Uarray(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
        "arg", arg,
        "plen", reinterpret_cast<void *>(plen));
    l_int32 len = static_cast<l_int32>(luaL_len(L, arg));
    l_uint32 *ua = ll_calloc<l_uint32>(_fun, L, len);

    /* verify there is a table at %arg */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table */
    while (lua_next(L, arg)) {
        l_int32 key = ll_check_l_int32(_fun, L, -2);        /* key is at index -2 */
        l_uint32 value = ll_check_l_uint32(_fun, L, -1);    /* value is at index -1 */
        /* don't write out of bounds */
        if (0 < key && key <= len) {
            ua[key-1] = value;
        } else {
            /* FIXME: error? */
        }
        /* remove value; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (plen)
        *plen = len;
    return ua;
}

/**
 * \brief Unpack an array of of arrays from the Lua stack as l_uint32* data.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param data pointer to a array of %wpl * %h * l_uint32
 * \param wpl words per line (inner array)
 * \param h height of the array (outer array)
 * \return pointer %data
 */
l_uint32 *
ll_unpack_Uarray_2d(const char *_fun, lua_State *L, int arg, l_uint32* data, l_int32 wpl, l_int32 h)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p, %s = %d, %s = %d\n", _fun,
        "arg", arg,
        "data", reinterpret_cast<void *>(data),
        "wpl", wpl,
        "h", h);
    /* verify there is a table at 2 */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table of tables */
    while (lua_next(L, arg)) {
        l_int32 y = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
        luaL_checktype(L, -1, LUA_TTABLE);                  /* value is at index -1 */
        /* push another nil key */
        lua_pushnil(L);
        /* iterate over the table of integers (table now at index -2) */
        while (lua_next(L, -2)) {
            l_int32 x = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
            l_uint32 value = ll_check_l_uint32(_fun, L, -1);    /* value is at index -1 */
            /* if x,y are in bounds */
            if (y > 0 && y <= h && x > 0 && x <= wpl) {
                data[(y - 1) * wpl + x - 1] = value;
            }
            /* remove value; keep 'key' for next iteration */
            lua_pop(L, 1);
        }
        /* remove table; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    return data;
}

/**
 * \brief Unpack an array of lua_Integer from the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param plen pointer to a l_int32 receiving the size of the array
 * \return allocated array l_uint32* with *pn entries
 */
l_float32 *
ll_unpack_Farray(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
        "arg", arg,
        "plen", reinterpret_cast<void *>(plen));
    l_int32 len = static_cast<l_int32>(luaL_len(L, arg));
    l_float32 *fa = ll_calloc<l_float32>(_fun, L, len);

    /* verify there is a table at %arg */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table */
    while (lua_next(L, arg)) {
        l_int32 key = ll_check_l_int32(_fun, L, -2);        /* key is at index -2 */
        l_float32 value = ll_check_l_float32(_fun, L, -1);  /* value is at index -1 */
        /* don't write out of bounds */
        if (0 < key && key <= len) {
            fa[key-1] = value;
        } else {
            /* FIXME: error? */
        }
        /* remove value; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (plen)
        *plen = len;
    return fa;
}

/**
 * \brief Unpack an array of of arrays from the Lua stack as l_float32* data.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param data pointer to a array of %wpl * %h * l_float32
 * \param wpl words per line (inner array)
 * \param h height of the array (outer array)
 * \return pointer %data
 */
l_float32 *
ll_unpack_Farray_2d(const char *_fun, lua_State *L, int arg, l_float32* data, l_int32 wpl, l_int32 h)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p, %s = %d, %s = %d\n", _fun,
        "arg", arg,
        "data", reinterpret_cast<void *>(data),
        "wpl", wpl,
        "h", h);
    /* verify there is a table at 2 */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table of tables */
    while (lua_next(L, arg)) {
        l_int32 y = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
        luaL_checktype(L, -1, LUA_TTABLE);                  /* value is at index -1 */
        /* push another nil key */
        lua_pushnil(L);
        /* iterate over the table of integers (table now at index -2) */
        while (lua_next(L, -2)) {
            l_int32 x = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
            l_float32 value = ll_check_l_float32(_fun, L, -1);  /* value is at index -1 */
            /* if x,y are in bounds */
            if (y > 0 && y <= h && x > 0 && x <= wpl) {
                data[(y - 1) * wpl + x - 1] = value;
            }
            /* remove value; keep 'key' for next iteration */
            lua_pop(L, 1);
        }
        /* remove table; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    return data;
}

/**
 * \brief Unpack a matrix from the Lua stack as l_float32* data.
 * Usually you would specify a matrix as a table array of table
 * arrays of Lua numbers, i.e. a 2D array of width (%w) times
 * height (%h) elements.
 *
 * Alternatively support a flattened array, i.e. a %w * %h
 * one-dimensional array of l_float32.
 *
 * And finally also support a %w * %h number of l_float32 arguments
 * starting at %arg.
 *
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param w width of the matrix (inner array)
 * \param h height of the matrix (outer array)
 * \return pointer %data, which must be free()d by the caller
 */
l_float32 *
ll_unpack_Matrix(const char *_fun, lua_State *L, int arg, l_int32 w, l_int32 h)
{
    l_float32 *data = ll_calloc<l_float32>(_fun, L, w * h);
    l_int32 x, y, i;
    l_float32 value;
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %d, %s = %d\n", _fun,
        "arg", arg,
        "w", w,
        "h", h);

    if (LUA_TTABLE == lua_type(L, arg)) {
        /* verify there is a table at 2 */
        luaL_checktype(L, arg, LUA_TTABLE);
        /* push a nil key */
        lua_pushnil(L);

        /* iterate over the table of tables */
        while (lua_next(L, arg)) {
            y = ll_check_l_int32(_fun, L, -2);                      /* key is at index -2 */
            if (LUA_TTABLE == lua_type(L, -1)) {
                luaL_checktype(L, -1, LUA_TTABLE);                  /* value is at index -1 */
                /* push another nil key */
                lua_pushnil(L);
                /* iterate over the table of integers (table now at index -2) */
                while (lua_next(L, -2)) {
                    x = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
                    value = ll_check_l_float32(_fun, L, -1);  /* value is at index -1 */
                    /* if x,y are in bounds */
                    if (y > 0 && y <= h && x > 0 && x <= w) {
                        data[(y - 1) * w + x - 1] = value;
                    }
                    /* remove value; keep 'key' for next iteration */
                    lua_pop(L, 1);
                }
            } else {
                i = y;
                value = ll_check_l_float32(_fun, L, -1);  /* value is at index -1 */
                if (i > 0 && i <= w * h) {
                    data[i - 1] = value;
                }
            }
            /* remove table; keep 'key' for next iteration */
            lua_pop(L, 1);
        }
    } else {
        for (i = 0; i < w * h; i++) {
            value = ll_check_l_float32(_fun, L, arg + i);
            data[i] = value;
        }
    }
    return data;
}

/**
 * \brief Unpack an array of lua_Number from the Lua stack as l_float64*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param plen pointer to a l_int32 receiving the size of the array
 * \return allocated array l_uint32* with *pn entries
 */
l_float64 *
ll_unpack_Darray(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
        "arg", arg,
        "plen", reinterpret_cast<void *>(plen));
    l_int32 len = static_cast<l_int32>(luaL_len(L, arg));
    l_float64 *da = ll_calloc<l_float64>(_fun, L, len);

    /* verify there is a table at %arg */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table */
    while (lua_next(L, arg)) {
        l_int32 key = ll_check_l_int32(_fun, L, -2);        /* key is at index -2 */
        l_float64 value = ll_check_l_float64(_fun, L, -1);  /* value is at index -1 */
        /* don't write out of bounds */
        if (0 < key && key <= len) {
            da[key-1] = value;
        } else {
            /* FIXME: error? */
        }
        /* remove value; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (plen)
        *plen = len;
    return da;
}

/**
 * \brief Unpack an array of of arrays from the Lua stack as l_float64* data.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param data pointer to a array of %wpl / 2 * %h * l_float64
 * \param wpl words per line (inner array)
 * \param h height of the array (outer array)
 * \return pointer %data
 */
l_float64 *
ll_unpack_Darray_2d(const char *_fun, lua_State *L, int arg, l_float64* data, l_int32 wpl, l_int32 h)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p, %s = %d, %s = %d\n", _fun,
        "arg", arg,
        "data", reinterpret_cast<void *>(data),
        "wpl", wpl,
        "h", h);
    /* verify there is a table at 2 */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table of tables */
    while (lua_next(L, arg)) {
        l_int32 y = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
        luaL_checktype(L, -1, LUA_TTABLE);                  /* value is at index -1 */
        /* push another nil key */
        lua_pushnil(L);
        /* iterate over the table of integers (table now at index -2) */
        while (lua_next(L, -2)) {
            l_int32 x = ll_check_l_int32(_fun, L, -2);          /* key is at index -2 */
            l_float64 value = ll_check_l_float64(_fun, L, -1);  /* value is at index -1 */
            /* if x,y are in bounds */
            if (y > 0 && y <= h && x > 0 && x <= wpl) {
                data[(y - 1) * wpl + x - 1] = value;
            }
            /* remove value; keep 'key' for next iteration */
            lua_pop(L, 1);
        }
        /* remove table; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    return data;
}

/**
 * \brief Unpack an array of lua_Integer from the Lua stack as Sarray*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the table
 * \param plen pointer to a l_int32 receiving the size of the array
 * \return allocated array l_uint32* with *pn entries
 */
Sarray *
ll_unpack_Sarray(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    DBG(LOG_CHECK_ARRAY, "%s: %s = %d, %s = %p\n", _fun,
        "arg", arg,
        "plen", reinterpret_cast<void *>(plen));
    l_int32 len = static_cast<l_int32>(luaL_len(L, arg));
    Sarray *sa = sarrayCreate(len);

    /* verify there is a table at %arg */
    luaL_checktype(L, arg, LUA_TTABLE);
    /* push a nil key */
    lua_pushnil(L);

    /* iterate over the table */
    while (lua_next(L, arg)) {
        l_int32 key = ll_check_l_int32(_fun, L, -2);        /* key is at index -2 */
        const char* value = ll_check_string(_fun, L, -1);   /* value is at index -1 */
        /* don't write out of bounds */
        if (0 < key && key <= len) {
            size_t slen = strlen(value);
            /* XXX: sarrayReplaceString() needs a non-const str */
            char *str = ll_calloc<char>(_fun, L, slen + 1);
            memcpy(str, value, slen);
            sarrayReplaceString(sa, key-1, str, L_CLONE);
            ll_free(str);
        } else {
            /* FIXME: error? */
        }
        /* remove value; keep 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (plen)
        *plen = len;
    return sa;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of 0 < index <= imax.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param imax maximum index value (for the 1-based value)
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_check_index(const char *_fun, lua_State *L, int arg, l_int32 imax)
{
    /* Lua indices are 1-based but Leptonica index is 0-based */
    lua_Integer index = luaL_checkinteger(L, arg) - 1;
    if (index < 0 || index >= imax) {
        lua_pushfstring(L, "%s: index #%d out of bounds (0 <= %d < %d)", _fun, arg, index, imax);
        lua_error(L);
        return 0;       /* NOTREACHED */
    }
    return static_cast<l_int32>(index);
}

/**
 * \brief Check if an argument (%arg) is a color or pixel index.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param pix [optional] pointer to a Pix*.
 * \return color or pixel index
 */
l_uint32
ll_check_color_index(const char *_fun, lua_State *L, int arg, Pix* pix)
{
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    l_int32 depth = pix ? pixGetDepth(pix) : 0;
    l_uint64 mask = (1u << depth) - 1;
    l_uint32 pixel = 0;

    if (0 == depth || 32 == depth) {
        /* no Pix* or Pix* is 32bpp */
        ll_check_color(_fun, L, arg, &rval, &gval, &bval, &aval);
        if (composeRGBAPixel(rval, gval, bval, aval, &pixel))
            return 0;
    } else if (ll_isinteger(_fun, L, arg) &&
               ll_isinteger(_fun, L, arg+1) &&
               ll_isinteger(_fun, L, arg+2)) {
        /* arg is probably 3 or 4 color components (rval, gval, bval[, aval]) */
        ll_check_color(_fun, L, arg, &rval, &gval, &bval, &aval);
        if (composeRGBAPixel(rval, gval, bval, aval, &pixel))
            return 0;
    } else {
        /* unsigned integer should be a color index */
        pixel = ll_check_l_uint32(_fun, L, arg);
        if (pixel > 0)
            pixel -= 1;
    }
    if (0 != mask)
        pixel = pixel & mask;
    return pixel;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of char.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return char for the integer; lua_error if out of bounds
 */
char
ll_check_char(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > 255) {
        lua_pushfstring(L, "%s: char #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<char>(val);
}

/**
 * \brief Check if an argument is a string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \return l_int32 for the integer; lua_error if out of bounds
 */
const char *
ll_check_string(const char *_fun, lua_State *L, int arg)
{
    const char *str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;

    if (!str) {
        lua_pushfstring(L, "%s: string #%d not defined", _fun, arg);
        lua_error(L);
        return nullptr;    /* NOTREACHED */
    }
    return str;
}

/**
 * \brief Return an argument string, if it is one.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default string to return, if no argument;
 * \return l_int32 for the integer; lua_error if out of bounds
 */
const char *
ll_opt_string(const char *_fun, lua_State *L, int arg, const char *def)
{
    const char *str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;
    UNUSED(_fun);
    return str ? str : def;
}

/**
 * \brief Optional argument string
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param plen optional pointer to a size_t where to store the string length
 * \return l_int32 for the integer; lua_error if out of bounds
 */
const char *
ll_check_lstring(const char *_fun, lua_State *L, int arg, size_t *plen)
{
    const char *str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;
    if (plen)
        *plen = 0;
    if (!str) {
        lua_pushfstring(L, "%s: string #%d not defined", _fun, arg);
        lua_error(L);
        return nullptr;    /* NOTREACHED */
    }
    if (plen)
        *plen = static_cast<size_t>(luaL_len(L, arg));
    return str;
}

/**
 * \brief Check if an argument is a string and return its length.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param plen optional pointer to a size_t where to store the string length
 * \return l_int32 for the integer; lua_error if out of bounds
 */
const l_uint8 *
ll_check_lbytes(const char *_fun, lua_State *L, int arg, size_t *plen)
{
    const char *str = ll_check_lstring(_fun, L, arg, plen);
    return reinterpret_cast<const l_uint8 *>(str);
}

/**
 * \brief Check if an argument is a string and return its length.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param plen optional pointer to a l_int32 where to store the string length
 * \return l_int32 for the integer; lua_error if out of bounds
 */
const l_uint8 *
ll_check_lbytes(const char *_fun, lua_State *L, int arg, l_int32 *plen)
{
    size_t size;
    const char *str = ll_check_lstring(_fun, L, arg, &size);
    if (plen)
        *plen = static_cast<l_int32>(size);
    return reinterpret_cast<const l_uint8 *>(str);
}

/**
 * \brief Return an user data argument (%arg) as luaL_Stream*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return pointer to luaL_Stream or error
 */
luaL_Stream *
ll_check_stream(const char *_fun, lua_State *L, int arg)
{
    if (LUA_TUSERDATA != lua_type(L, arg)) {
        lua_pushfstring(L, "%s: expected a '%s' as #%d, got '%s'",
                        _fun, LUA_FILEHANDLE, arg, lua_typename(L, lua_type(L, arg)));
        lua_error(L);
        return nullptr;    /* NOTREACHED */
    }
    return reinterpret_cast<luaL_Stream *>(luaL_checkudata(L, arg, LUA_FILEHANDLE));
}

/**
 * \brief Return an user data argument (%arg) as luaL_Stream*, if it is one.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return pointer to luaL_Stream or error
 */
luaL_Stream *
ll_opt_stream(const char *_fun, lua_State *L, int arg)
{
    if (LUA_TUSERDATA != lua_type(L, arg))
        return nullptr;
    return reinterpret_cast<luaL_Stream *>(luaL_checkudata(L, arg, LUA_FILEHANDLE));
}

/**
 * \brief Check if an argument is a boolean.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_int32 for the boolean (1 = true, 0 = false); lua_error if out of bounds
 */
l_int32
ll_check_boolean(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isboolean(L, arg)) {
        lua_pushfstring(L, "%s: boolean expected for arg #%d (got '%s')", _fun, arg, lua_typename(L, lua_type(L, arg)));
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return lua_toboolean(L, arg) ? TRUE : FALSE;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_uint8.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_uint8 for the integer; lua_error if out of bounds
 */
l_uint8
ll_check_l_uint8(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > UINT8_MAX) {
        lua_pushfstring(L, "%s: l_uint8 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<l_uint8>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_uint8 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_uint8 for the integer; lua_error if out of bounds
 */
l_uint8
ll_opt_l_uint8(const char *_fun, lua_State *L, int arg, l_uint8 def)
{
    lua_Integer val = luaL_optinteger(L, arg, def);

    if (val < 0 || val > UINT8_MAX) {
        lua_pushfstring(L, "%s: l_uint8 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<l_uint8>(val);
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_uint16.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_uint16 for the integer; lua_error if out of bounds
 */
l_uint16
ll_check_l_uint16(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > UINT16_MAX) {
        lua_pushfstring(L, "%s: l_uint16 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<l_uint16>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_uint16 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_uint16 for the integer; lua_error if out of bounds
 */
l_uint16
ll_opt_l_uint16(const char *_fun, lua_State *L, int arg, l_uint16 def)
{
    lua_Integer val = luaL_optinteger(L, arg, def);

    if (val < 0 || val > UINT16_MAX) {
        lua_pushfstring(L, "%s: l_uint16 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<l_uint16>(val);
}

/**
 * \brief Check if an argument is a boolean, or return the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_int32 for the boolean (1 = true, 0 = false); lua_error if out of bounds
 */
l_int32
ll_opt_boolean(const char *_fun, lua_State *L, int arg, int def)
{
    int val = lua_isboolean(L, arg) ? lua_toboolean(L, arg) : def;
    if (val != 0 && val != 1) {
        lua_pushfstring(L, "%s: boolean #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return val ? TRUE : FALSE;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_int32.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_check_l_int32(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "%s: l_int32 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<l_int32>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_int32 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_int32 for the integer; lua_error if out of bounds
 */
l_int32
ll_opt_l_int32(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    lua_Integer val = luaL_optinteger(L, arg, def);

    if (val < INT32_MIN || val > INT32_MAX) {
        lua_pushfstring(L, "%s: l_int32 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<l_int32>(val);
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_uint32.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_uint32 for the integer; lua_error if out of bounds
 */
l_uint32
ll_check_l_uint32(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "%s: l_uint32 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<l_uint32>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_uint32 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_uint32 for the integer; lua_error if out of bounds
 */
l_uint32
ll_opt_l_uint32(const char *_fun, lua_State *L, int arg, l_uint32 def)
{
    lua_Integer val = luaL_optinteger(L, arg, def);

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "%s: l_uint32 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<l_uint32>(val);
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_int64.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_int64 for the integer; lua_error if out of bounds
 */
l_int64
ll_check_l_int64(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < INT64_MIN || val > INT64_MAX) {
        /* XXX: can't happen */
        lua_pushfstring(L, "%s: l_int64 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<l_int64>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_int64 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_int64 for the integer; lua_error if out of bounds
 */
l_int64
ll_opt_l_int64(const char *_fun, lua_State *L, int arg, l_int64 def)
{
    lua_Integer val = luaL_optinteger(L, arg, def);

    if (val < INT64_MIN || val > INT64_MAX) {
        /* XXX: can't happen */
        lua_pushfstring(L, "%s: l_int64 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<l_int64>(val);
}

/**
 * \brief Check if an argument is a lua_Integer in the range of l_uint64.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_uint64 for the integer; lua_error if out of bounds
 */
l_uint64
ll_check_l_uint64(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);
    l_uint64 ret = static_cast<l_uint64>(val);

    if (ret > UINT64_MAX) {
        /* XXX: can't happen ? */
        lua_pushfstring(L, "%s: l_uint64 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return ret;
}

/**
 * \brief Return an argument lua_Integer in the range of l_uint64 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_uint64 for the integer; lua_error if out of bounds
 */
l_uint64
ll_opt_l_uint64(const char *_fun, lua_State *L, int arg, l_uint64 def)
{
    lua_Integer val = luaL_optinteger(L, arg, static_cast<lua_Integer>(def));
    l_uint64 ret = static_cast<l_uint64>(val);

    if (ret > UINT64_MAX) {
        /* XXX: can't happen ? */
        lua_pushfstring(L, "%s: l_uint64 #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return ret;
}

/**
 * \brief Check if an argument is a lua_Number in the range of l_float32.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_check_l_float32(const char *_fun, lua_State *L, int arg)
{
    lua_Number val = luaL_checknumber(L, arg);

    if (val < static_cast<lua_Number>(-FLT_MAX) || val > static_cast<lua_Number>(FLT_MAX)) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        _fun, arg,
                        static_cast<lua_Number>(-FLT_MAX),
                        val,
                        static_cast<lua_Number>(FLT_MAX));
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return static_cast<l_float32>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_float32 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_opt_l_float32(const char *_fun, lua_State *L, int arg, l_float32 def)
{
    lua_Number val = luaL_optnumber(L, arg, static_cast<lua_Number>(def));

    if (val < static_cast<lua_Number>(-FLT_MAX) || val > static_cast<lua_Number>(FLT_MAX)) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        _fun, arg,
                        static_cast<lua_Number>(-FLT_MAX),
                        val,
                        static_cast<lua_Number>(FLT_MAX));
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return static_cast<l_float32>(val);
}

/**
 * \brief Check if an argument is a lua_Number in the range of l_float64.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return l_float64 for the number; lua_error if out of bounds
 */
l_float64
ll_check_l_float64(const char *_fun, lua_State *L, int arg)
{
    lua_Number val = luaL_checknumber(L, arg);
    (void)_fun;
    return static_cast<l_float64>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_float64 or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return l_float64 for the number; lua_error if out of bounds
 */
l_float64
ll_opt_l_float64(const char *_fun, lua_State *L, int arg, l_float64 def)
{
    lua_Number val = luaL_optnumber(L, arg, def);
    (void)_fun;
    return static_cast<l_float64>(val);
}

/**
 * \brief Check for a vector of n l_float32 values
 *
 * The vector can be given as a table array of 6 float values,
 * or as 6 arguments at %arg, %arg+1, ..., %arg+5
 *
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param n number of values expected in the vector
 * \return pointer to n times l_float32; caller must free
 */
l_float32 *
ll_check_vector(const char *_fun, lua_State *L, int arg, int n)
{
    l_float32 *vec = ll_calloc<l_float32>(_fun, L, n);
    l_float32 *array = nullptr;
    l_int32 len;
    if (lua_istable(L, arg)) {
        array = ll_unpack_Farray(_fun, L, arg, &len);
        for (int i = 0; i < n && i < 6; i++)
            vec[i] = array[i];
        ll_free(array);
    } else {
        for (int i = 0; i < n; i++) {
            vec[i] = ll_check_l_float32(_fun, L, arg + i);
        }
    }
    return vec;
}

/**
 * \brief Check if an argument is a lua_Integer in the range of size_t.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \return size_t for the integer; lua_error if out of bounds
 */
size_t
ll_check_size_t(const char *_fun, lua_State *L, int arg)
{
    lua_Integer val = luaL_checkinteger(L, arg);

    if (val < 0 || static_cast<size_t>(val) > SIZE_MAX) {
        lua_pushfstring(L, "%s: size_t #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return 0;    /* NOTREACHED */
    }
    return static_cast<size_t>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of size_t or the default.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the integer
 * \param def default value
 * \return size_t for the integer; lua_error if out of bounds
 */
size_t
ll_opt_size_t(const char *_fun, lua_State *L, int arg, size_t def)
{
    lua_Integer val = luaL_optinteger(L, arg, static_cast<lua_Integer>(def));

    if (val < 0 || val > UINT32_MAX) {
        lua_pushfstring(L, "%s: size_t #%d out of bounds (%d)", _fun, arg, val);
        lua_error(L);
        return def;    /* NOTREACHED */
    }
    return static_cast<size_t>(val);
}

typedef struct ll_type_s {
    ll_type_e   type;
    const char  name[24];
}   ll_type_t;

static const ll_type_t types[] = {
    {ll_boolean,    LL_BOOLEAN},
    {ll_int8,       LL_INT8},
    {ll_uint8,      LL_UINT8},
    {ll_int16,      LL_INT16},
    {ll_uint16,     LL_UINT16},
    {ll_int32,      LL_INT32},
    {ll_uint32,     LL_UINT32},
    {ll_int64,      LL_INT64},
    {ll_uint64,     LL_UINT64},
    {ll_float32,    LL_FLOAT32},
    {ll_float64,    LL_FLOAT64},
    {ll_amap,       LL_AMAP},
    {ll_aset,       LL_ASET},
    {ll_bbuffer,    LL_BBUFFER},
    {ll_bmf,        LL_BMF},
    {ll_box,        LL_BOX},
    {ll_boxa,       LL_BOXA},
    {ll_boxaa,      LL_BOXAA},
    {ll_bytea,      LL_BYTEA},
    {ll_compdata,   LL_COMPDATA},
    {ll_ccbord,     LL_CCBORD},
    {ll_ccborda,    LL_CCBORDA},
    {ll_dewarp,     LL_DEWARP},
    {ll_dewarpa,    LL_DEWARPA},
    {ll_dllist,     LL_DLLIST},
    {ll_dna,        LL_DNA},
    {ll_dnaa,       LL_DNAA},
    {ll_dnahash,    LL_DNAHASH},
    {ll_dpix,       LL_DPIX},
    {ll_fpix,       LL_FPIX},
    {ll_fpixa,      LL_FPIXA},
    {ll_kernel,     LL_KERNEL},
    {ll_numa,       LL_NUMA},
    {ll_numaa,      LL_NUMAA},
    {ll_pdfdata,    LL_PDFDATA},
    {ll_pix,        LL_PIX},
    {ll_pixa,       LL_PIXA},
    {ll_pixaa,      LL_PIXAA},
    {ll_pixcmap,    LL_PIXCMAP},
    {ll_pixtiling,  LL_PIXTILING},
    {ll_pixcomp,    LL_PIXCOMP},
    {ll_pixacomp,   LL_PIXACOMP},
    {ll_pta,        LL_PTA},
    {ll_ptaa,       LL_PTAA},
    {ll_rbtnode,    LL_RBTNODE},
    {ll_sarray,     LL_SARRAY},
    {ll_sel,        LL_SEL},
    {ll_sela,       LL_SELA},
    {ll_stack,      LL_STACK},
    {ll_wshed,      LL_WSHED}
};

/**
 * \brief Check if %type is a known lualept type and return its name.
 * \param type one of ll_type_e values
 * \return const string pointer
 */
static const char *
ll_typestr(ll_type_e type)
{
    size_t i;

    for (i = 0; i < ARRAYSIZE(types); i++)
        if (type == types[i].type)
            return types[i].name;
    return "<unknown>";
}

/**
 * \brief Set all global variables defined in %vars.
 * \param _fun calling function's name
 * \param L Lua state
 * \param vars pointer to the ll_global_var_t array
 * \return 0 on success, or die on error
 */
int
ll_set_all_globals(const char *_fun, lua_State *L, const ll_global_var_t *vars)
{
    const ll_global_var_t *var;

    for (var = vars; ll_invalid != var->type; var++) {

        switch (var->type) {
        case ll_boolean:
            ll_push_boolean(_fun, L, *var->u.pb);
            lua_setglobal(L, var->name);
            break;

        case ll_int8:
            ll_push_l_int8(_fun, L, *var->u.pi8);
            lua_setglobal(L, var->name);
            break;

        case ll_uint8:
            ll_push_l_uint8(_fun, L, *var->u.pu8);
            lua_setglobal(L, var->name);
            break;

        case ll_int16:
            ll_push_l_int16(_fun, L, *var->u.pi16);
            lua_setglobal(L, var->name);
            break;

        case ll_uint16:
            ll_push_l_uint16(_fun, L, *var->u.pu16);
            lua_setglobal(L, var->name);
            break;

        case ll_int32:
            ll_push_l_int32(_fun, L, *var->u.pi32);
            lua_setglobal(L, var->name);
            break;

        case ll_uint32:
            ll_push_l_uint32(_fun, L, *var->u.pu32);
            lua_setglobal(L, var->name);
            break;

        case ll_int64:
            ll_push_l_int64(_fun, L, *var->u.pi64);
            lua_setglobal(L, var->name);
            break;

        case ll_uint64:
            ll_push_l_uint64(_fun, L, *var->u.pu64);
            lua_setglobal(L, var->name);
            break;

        case ll_float32:
            ll_push_l_float32(_fun, L, *var->u.pf32);
            lua_setglobal(L, var->name);
            break;

        case ll_float64:
            ll_push_l_float64(_fun, L, *var->u.pf64);
            lua_setglobal(L, var->name);
            break;

        case ll_pchars:
            ll_push_string(_fun, L, *var->u.pchars);
            *var->u.pchars = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pbytes:
            ll_push_bytes(_fun, L, var->u.pbytes->data, var->u.pbytes->size);
            (*var->u.pbytes).data = nullptr;
            (*var->u.pbytes).size = 0;
            lua_setglobal(L, var->name);
            break;

        case ll_amap:
            ll_push_Amap(_fun, L, *var->u.pamap);
            *var->u.pamap = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_aset:
            ll_push_Aset(_fun, L, *var->u.paset);
            *var->u.paset = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_bbuffer:
            ll_push_ByteBuffer(_fun, L, *var->u.pbb);
            *var->u.pbb = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_bmf:
            ll_push_Bmf(_fun, L, *var->u.pbmf);
            *var->u.pbmf = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_box:
            ll_push_Box(_fun, L, *var->u.pbox);
            *var->u.pbox = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_boxa:
            ll_push_Boxa(_fun, L, *var->u.pboxa);
            *var->u.pboxa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_boxaa:
            ll_push_Boxaa(_fun, L, *var->u.pboxaa);
            *var->u.pboxaa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_bytea:
            ll_push_Bytea(_fun, L, *var->u.pbytea);
            *var->u.pbytea = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_compdata:
            ll_push_CompData(_fun, L, *var->u.pcid);
            *var->u.pcid = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_ccbord:
            ll_push_CCBord(_fun, L, *var->u.pccb);
            *var->u.pccb = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_ccborda:
            ll_push_CCBorda(_fun, L, *var->u.pccba);
            *var->u.pccba = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dewarp:
            ll_push_Dewarp(_fun, L, *var->u.pdew);
            *var->u.pdew = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dewarpa:
            ll_push_Dewarpa(_fun, L, *var->u.pdewa);
            *var->u.pdewa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dllist:
            ll_push_DLList(_fun, L, *var->u.plist);
            *var->u.plist = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dna:
            ll_push_Dna(_fun, L, *var->u.pda);
            *var->u.pda = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dnaa:
            ll_push_Dnaa(_fun, L, *var->u.pdaa);
            *var->u.pdaa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dnahash:
            ll_push_DnaHash(_fun, L, *var->u.pdah);
            *var->u.pdah = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_dpix:
            ll_push_DPix(_fun, L, *var->u.pdpix);
            *var->u.pdpix = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_fpix:
            ll_push_FPix(_fun, L, *var->u.pfpix);
            *var->u.pfpix = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_fpixa:
            ll_push_FPixa(_fun, L, *var->u.pfpixa);
            *var->u.pfpixa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_kernel:
            ll_push_Kernel(_fun, L, *var->u.pkel);
            *var->u.pkel = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_numa:
            ll_push_Numa(_fun, L, *var->u.pna);
            *var->u.pna = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_numaa:
            ll_push_Numaa(_fun, L, *var->u.pnaa);
            *var->u.pnaa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pdfdata:
            ll_push_PdfData(_fun, L, *var->u.ppdd);
            *var->u.ppdd = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pix:
            ll_push_Pix(_fun, L, *var->u.ppix);
            *var->u.ppix = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixa:
            ll_push_Pixa(_fun, L, *var->u.ppixa);
            *var->u.ppixa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixaa:
            ll_push_Pixaa(_fun, L, *var->u.ppixaa);
            *var->u.ppixaa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixcmap:
            ll_push_PixColormap(_fun, L, *var->u.pcmap);
            *var->u.pcmap = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixtiling:
            ll_push_PixTiling(_fun, L, *var->u.ppixt);
            *var->u.ppixt = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixcomp:
            ll_push_PixComp(_fun, L, *var->u.ppixc);
            *var->u.ppixc = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pixacomp:
            ll_push_PixaComp(_fun, L, *var->u.ppixac);
            *var->u.ppixac = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_pta:
            ll_push_Pta(_fun, L, *var->u.ppta);
            *var->u.ppta = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_ptaa:
            ll_push_Ptaa(_fun, L, *var->u.pptaa);
            *var->u.pptaa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_rbtnode:
            lua_pushlightuserdata(L, *var->u.pnode);
            *var->u.pnode = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_sarray:
            ll_push_Sarray(_fun, L, *var->u.psa);
            *var->u.psa = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_sel:
            ll_push_Sel(_fun, L, *var->u.psel);
            *var->u.psel = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_sela:
            ll_push_Sela(_fun, L, *var->u.psela);
            *var->u.psela = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_stack:
            ll_push_Stack(_fun, L, *var->u.pstack);
            *var->u.pstack = nullptr;
            lua_setglobal(L, var->name);
            break;

        case ll_wshed:
            ll_push_WShed(_fun, L, *var->u.pwshed);
            *var->u.pwshed = nullptr;
            lua_setglobal(L, var->name);
            break;

        default:
            die(_fun, L, "Unsupported type '%d' with name '%s'\n", var->type, var->name);
        }
    }
    return 0;
}

/**
 * \brief Get all global variables defined in %vars.
 * \param _fun calling function's name
 * \param L Lua state
 * \param vars pointer to the ll_global_var_t array
 * \return 0 on success, or die on error
 */
int
ll_get_all_globals(const char *_fun, lua_State *L, const ll_global_var_t *vars)
{
    const ll_global_var_t *var;

    for (var = vars; ll_invalid != var->type; var++) {

        switch (var->type) {
        case ll_boolean:
            if (LUA_TBOOLEAN == lua_getglobal(L, var->name)) {
                *var->u.pb = static_cast<bool>(lua_toboolean(L, 1));
            } else {
                *var->u.pb = false;
            }
            break;

        case ll_int8:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pi8 = static_cast<l_int8>(lua_tointeger(L, 1));
            } else {
                *var->u.pi8 = 0;
            }
            break;

        case ll_uint8:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pu8 = static_cast<l_uint8>(lua_tointeger(L, 1));
            } else {
                *var->u.pu8 = 0;
            }
            break;

        case ll_int16:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pi16 = static_cast<l_int16>(lua_tointeger(L, 1));
            } else {
                *var->u.pi16 = 0;
            }
            break;

        case ll_uint16:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pu16 = static_cast<l_uint16>(lua_tointeger(L, 1));
            } else {
                *var->u.pu16 = 0;
            }
            break;

        case ll_int32:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pi32 = static_cast<l_int32>(lua_tointeger(L, 1));
            } else {
                *var->u.pi32 = 0;
            }
            break;

        case ll_uint32:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pu32 = static_cast<l_uint32>(lua_tointeger(L, 1));
            } else {
                *var->u.pu32 = 0;
            }
            break;

        case ll_int64:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pi64 = static_cast<l_int64>(lua_tointeger(L, 1));
            } else {
                *var->u.pi64 = 0;
            }
            break;

        case ll_uint64:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pu64 = static_cast<l_uint64>(lua_tointeger(L, 1));
            } else {
                *var->u.pu64 = 0;
            }
            break;

        case ll_float32:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pf32 = static_cast<l_float32>(lua_tonumber(L, 1));
            } else {
                *var->u.pf32 = 0.0f;
            }
            break;

        case ll_float64:
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->u.pf64 = static_cast<l_float64>(lua_tonumber(L, 1));
            } else {
                *var->u.pf64 = 0.0;
            }
            break;

        case ll_pchars:
            if (LUA_TSTRING == lua_getglobal(L, var->name)) {
                const char *str = lua_tostring(L, -1);
                const size_t len = str ? strlen(str) + 1 : 1;
                *var->u.pchars = ll_calloc<char>(_fun, L, len);
                memcpy(*var->u.pchars, str, len);
            } else {
                *var->u.pf64 = 0.0;
            }
            break;

        case ll_pbytes:
            if (LUA_TSTRING == lua_getglobal(L, var->name)) {
                size_t size;
                const l_uint8 *str = ll_check_lbytes(_fun, L, -1, &size);
                (*var->u.pbytes).data = ll_malloc<l_uint8>(_fun, L, size);
                (*var->u.pbytes).size = size;
                memcpy((*var->u.pbytes).data, str, size);
            } else {
                *var->u.pf64 = 0.0;
            }
            break;

        case ll_amap:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pamap = ll_take_udata<Amap>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pamap = nullptr;
            }
            break;

        case ll_aset:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.paset = ll_take_udata<Aset>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.paset = nullptr;
            }
            break;

        case ll_bbuffer:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pbb = ll_take_udata<ByteBuffer>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pbb = nullptr;
            }
            break;

        case ll_bmf:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pbmf = ll_take_udata<Bmf>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pbmf = nullptr;
            }
            break;

        case ll_box:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pbox = ll_take_udata<Box>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pbox = nullptr;
            }
            break;

        case ll_boxa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pboxa = ll_take_udata<Boxa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pboxa = nullptr;
            }
            break;

        case ll_boxaa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pboxaa = ll_take_udata<Boxaa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pboxaa = nullptr;
            }
            break;

        case ll_bytea:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pbytea = ll_take_udata<Bytea>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pbytea = nullptr;
            }
            break;

        case ll_compdata:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pcid = ll_take_udata<CompData>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pcid = nullptr;
            }
            break;

        case ll_ccbord:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pccb = ll_take_udata<CCBord>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pccb = nullptr;
            }
            break;

        case ll_ccborda:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pccba = ll_take_udata<CCBorda>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pccba = nullptr;
            }
            break;

        case ll_dewarp:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pdew = ll_take_udata<Dewarp>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pdew = nullptr;
            }
            break;

        case ll_dewarpa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pdewa = ll_take_udata<Dewarpa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pdewa = nullptr;
            }
            break;

        case ll_dllist:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.plist = ll_take_udata<DLList>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.plist = nullptr;
            }
            break;

        case ll_dna:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pda = ll_take_udata<Dna>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pda = nullptr;
            }
            break;

        case ll_dnaa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pdaa = ll_take_udata<Dnaa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pdaa = nullptr;
            }
            break;

        case ll_dnahash:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pdah = ll_take_udata<DnaHash>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pdah = nullptr;
            }
            break;

        case ll_dpix:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pdpix = ll_take_udata<DPix>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pdpix = nullptr;
            }
            break;

        case ll_fpix:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pfpix = ll_take_udata<FPix>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pfpix = nullptr;
            }
            break;

        case ll_fpixa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pfpixa = ll_take_udata<FPixa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pfpixa = nullptr;
            }
            break;

        case ll_kernel:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pkel = ll_take_udata<Kernel>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pkel = nullptr;
            }
            break;

        case ll_numa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pna = ll_take_udata<Numa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pna = nullptr;
            }
            break;

        case ll_numaa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pnaa = ll_take_udata<Numaa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pnaa = nullptr;
            }
            break;

        case ll_pdfdata:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppdd = ll_take_udata<PdfData>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppdd = nullptr;
            }
            break;

        case ll_pix:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppix = ll_take_udata<Pix>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppix = nullptr;
            }
            break;

        case ll_pixa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppixa = ll_take_udata<Pixa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppixa = nullptr;
            }
            break;

        case ll_pixaa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppixaa = ll_take_udata<Pixaa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppixaa = nullptr;
            }
            break;

        case ll_pixcmap:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pcmap = ll_take_udata<PixColormap>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pcmap = nullptr;
            }
            break;

        case ll_pixtiling:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppixt = ll_take_udata<PixTiling>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppixt = nullptr;
            }
            break;

        case ll_pixcomp:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppixc = ll_take_udata<PixComp>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppixc = nullptr;
            }
            break;

        case ll_pixacomp:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppixac = ll_take_udata<PixaComp>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppixac = nullptr;
            }
            break;

        case ll_pta:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.ppta = ll_take_udata<Pta>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.ppta = nullptr;
            }
            break;

        case ll_ptaa:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pptaa = ll_take_udata<Ptaa>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pptaa = nullptr;
            }
            break;

        case ll_rbtnode:
            *reinterpret_cast<void **>(var->u.pptr) = nullptr;
            break;

        case ll_sarray:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.psa = ll_take_udata<Sarray>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.psa = nullptr;
            }
            break;

        case ll_sel:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.psel = ll_take_udata<Sel>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.psel = nullptr;
            }
            break;

        case ll_sela:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.psela = ll_take_udata<Sela>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.psela = nullptr;
            }
            break;

        case ll_stack:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pstack = ll_take_udata<Stack>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pstack = nullptr;
            }
            break;

        case ll_wshed:
            if (LUA_TUSERDATA == lua_getglobal(L, var->name)) {
                *var->u.pwshed = ll_take_udata<WShed>(_fun, L, -1, ll_typestr(var->type));
            } else {
                *var->u.pwshed = nullptr;
            }
            break;

        default:
            die(_fun, L, "Unsupported type '%d' with name '%s'\n", var->type, var->name);
        }
    }
    return 0;
}

/**
 * \class LuaLept
 * The main class.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_LUALEPT

/** Define a function's name (_fun) with prefix lualept */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Enable a debug flag.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
DebugOn(lua_State *L)
{
    LL_FUNC("DebugOn");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    int i;

    UNUSED(ll);
    for (i = 2; i <= lua_gettop(L); i++) {
        l_int32 mask = ll_check_debug(_fun, L, i);
        if (!mask)
            return 1;
#if defined(LUALEPT_DEBUG) && (LUALEPT_DEBUG > 0)
        dbg_enabled |= mask;
#endif
    }
    return 0;
}

/**
 * \brief Disable a debug flag.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
DebugOff(lua_State *L)
{
    LL_FUNC("DebugOff");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    int i;

    UNUSED(ll);
    for (i = 2; i <= lua_gettop(L); i++) {
        l_int32 mask = ll_check_debug(_fun, L, i);
        if (!mask)
            return 1;
#if defined(LUALEPT_DEBUG) && (LUALEPT_DEBUG > 0)
        dbg_enabled &= ~mask;
#endif
    }
    return 0;
}

/**
 * \brief Return enabled debug options as strings.
 * \param L Lua state
 * \return n (0 .. 32) strings on the Lua stack
 */
static int
Debug(lua_State *L)
{
    LL_FUNC("Debug");
#if defined(LUALEPT_DEBUG) && (LUALEPT_DEBUG > 0)
    const char *debug = ll_string_debug(dbg_enabled);
    Sarray *sa = sarrayCreate(1);
    l_int32 i, n;
    sarraySplitString(sa, debug, "|");
    n = sarrayGetCount(sa);
    for (i = 0; i < n; i++) {
        const char *str = sarrayGetString(sa, i, L_NOCOPY);
        lua_pushstring(L, str);
    }
    sarrayDestroy(&sa);
    return n;
#else
    return ll_push_nil(L);
#endif
}

/**
 * \brief Create a a LuaLept*.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    return ll_new_lualept(L);
}

/**
 * \brief Destroy a LuaLept*.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    LuaLept *ll = ll_take_udata<LuaLept>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n",
        _fun, TNAME,
        "ll", reinterpret_cast<void *>(ll));
    ll_free(ll);
    return 0;
}

/**
 * \brief Return the a version number.
 * Arg #1 (i.e. self) is expected to be a LuaLept* (ll)
 * Arg #2 is expected to be a string describing the version of which library to return (which)
 * \param L Lua state
 * \return 1 string on the Lua stack
 */
static int
Version(lua_State *L)
{
    LL_FUNC("Version");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    const char *which = ll_opt_string(_fun, L, 2);
    if (nullptr == which || 0 == ll_strcasecmp(which, "lualept")) {
        lua_pushstring(L, ll->str_version);
    } else if (0 == ll_strcasecmp(which, "lua")) {
        lua_pushstring(L, ll->str_version_lua);
    } else if (0 == ll_strcasecmp(which, "lept") || 0 == ll_strcasecmp(which, "leptonica")) {
        lua_pushstring(L, ll->str_version_lept);
    } else {
        lua_pushfstring(L, "<unknown %s>", which);
    }
    return 1;
}

/**
 * \brief Compose a RGB pixel value.
 *
 * Arg #1 is expected to be a l_int32 (rval).
 * Arg #2 is expected to be a l_int32 (gval).
 * Arg #3 is expected to be a l_int32 (bval).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
ComposeRGB(lua_State *L)
{
    LL_FUNC("ComposeRGB");
    l_int32 rval = ll_check_l_int32(_fun, L, 1);
    l_int32 gval = ll_check_l_int32(_fun, L, 2);
    l_int32 bval = ll_check_l_int32(_fun, L, 3);
    l_uint32 pixel;
    if (composeRGBPixel(rval, gval, bval, &pixel))
        return ll_push_nil(L);
    ll_push_l_uint32(_fun, L, pixel);
    return 1;
}

/**
 * \brief Compose a RGBA pixel value.
 *
 * Arg #1 is expected to be a l_int32 (rval).
 * Arg #2 is expected to be a l_int32 (gval).
 * Arg #3 is expected to be a l_int32 (bval).
 * Arg #4 is expected to be a l_int32 (aval).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
ComposeRGBA(lua_State *L)
{
    LL_FUNC("ComposeRGBA");
    l_int32 rval = ll_check_l_int32(_fun, L, 1);
    l_int32 gval = ll_check_l_int32(_fun, L, 2);
    l_int32 bval = ll_check_l_int32(_fun, L, 3);
    l_int32 aval = ll_check_l_int32(_fun, L, 3);
    l_uint32 pixel;
    if (composeRGBAPixel(rval, gval, bval, aval, &pixel))
        return ll_push_nil(L);
    ll_push_l_uint32(_fun, L, pixel);
    return 1;
}

/**
 * \brief Compose a RGBA pixel value.
 *
 * Arg #1 is expected to be a l_int32 (rval).
 * Arg #2 is expected to be a l_int32 (gval).
 * Arg #3 is expected to be a l_int32 (bval).
 * Arg #4 is expected to be a l_int32 (aval).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
Color(lua_State *L)
{
    LL_FUNC("Color");
    l_int32 color = ll_check_color_name(_fun, L, 1);
    l_uint32 pixel = 0;
    l_int32 r = 0;
    l_int32 g = 0;
    l_int32 b = 0;
    l_int32 a = 0;
    if (color >= 1 << 24) {
        const char *str = ll_check_string(_fun, L, 1);
        if (*str == '#')
            str++;
        /* expect hexadecimal digits for 0xRRGGBB */
        color = static_cast<l_int32>(strtol(str, nullptr, 16));
    }
    if (color >= 1 << 24) {
        /* 0xRRGGBBAA */
        r = (color >> 24) & 0xff;
        g = (color >> 16) & 0xff;
        b = (color >>  8) & 0xff;
        a = (color >>  0) & 0xff;
    } else {
        /* 0x00RRGGBB */
        r = (color >> 16) & 0xff;
        g = (color >>  8) & 0xff;
        b = (color >>  0) & 0xff;
        a = 0xff;
    }
    if (composeRGBPixel(r, g, b, &pixel))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, pixel);
}

/**
 * \brief Extract a RGB pixel values.
 *
 * Arg #1 is expected to be a string (filename).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
ToRGB(lua_State *L)
{
    LL_FUNC("ToRGB");
    l_uint32 pixel = ll_check_l_uint32(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    extractRGBValues(pixel, &rval, &gval, &bval);
    ll_push_l_int32(_fun, L, rval);
    ll_push_l_int32(_fun, L, gval);
    ll_push_l_int32(_fun, L, bval);
    return 3;
}

/**
 * \brief Extract a RGBA pixel values.
 *
 * Arg #1 is expected to be a string (filename).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
ToRGBA(lua_State *L)
{
    LL_FUNC("ToRGBA");
    l_uint32 pixel = ll_check_l_uint32(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    extractRGBAValues(pixel, &rval, &gval, &bval, &aval);
    ll_push_l_int32(_fun, L, rval);
    ll_push_l_int32(_fun, L, gval);
    ll_push_l_int32(_fun, L, bval);
    ll_push_l_int32(_fun, L, aval);
    return 4;
}

/**
 * \brief MakeGrayQuantIndexTable() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (nlevels).
 *
 * Leptonica's Notes:
 *      (1) 'nlevels' is some number between 2 and 256 (typically 8 or less).
 *      (2) The table is typically used for quantizing 2, 4 and 8 bpp
 *          grayscale src pix, and generating a colormapped dest pix.
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
MakeGrayQuantIndexTable(lua_State *L)
{
    LL_FUNC("MakeGrayQuantIndexTable");
    l_int32 nlevels = ll_check_l_int32(_fun, L, 1);
    l_int32 *result = makeGrayQuantIndexTable(nlevels);
    ll_pack_Iarray(_fun, L, result, nlevels);
    ll_free(result);
    return 1;
}

/**
 * \brief MakeGrayQuantTableArb() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (outdepth).
 *
 * Leptonica's Notes:
 *      (1) The number of bins is the count of %na + 1.
 *      (2) The bin boundaries in na must be sorted in increasing order.
 *      (3) The table is an inverse colormap: it maps input gray level
 *          to colormap index (the bin number).
 *      (4) The colormap generated here has quantized values at the
 *          center of each bin.  If you want to use the average gray
 *          value of pixels within the bin, discard the colormap and
 *          compute it using makeGrayQuantColormapArb().
 *      (5) Returns an error if there are not enough levels in the
 *          output colormap for the number of bins.  The number
 *          of bins must not exceed 2^outdepth.
 * </pre>
 * \param L Lua state
 * \return 2 on the Lua stack
 */
static int
MakeGrayQuantTableArb(lua_State *L)
{
    LL_FUNC("MakeGrayQuantTableArb");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 2);
    l_int32 *tab = nullptr;
    PixColormap *cmap = nullptr;
    if (makeGrayQuantTableArb(na, outdepth, &tab, &cmap))
        return ll_push_nil(L);
    ll_pack_Iarray(_fun, L, tab, 1 << outdepth);
    ll_push_PixColormap(_fun, L, cmap);
    return 2;
}

/**
 * \brief Extract minimum or maximum component from pixel value.
 *
 * Arg #1 is expected to be a l_uint32 (pixel).
 * Arg #2 is expected to be a string (min or max).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
MinMaxComponent(lua_State *L)
{
    LL_FUNC("MinMaxComponent");
    l_uint32 pixel = ll_check_l_uint32(_fun, L, 1);
    l_int32 type = ll_check_choose_min_max(_fun, L, 2, 0);
    ll_push_l_int32(_fun, L, extractMinMaxComponent(pixel, type));
    return 1;
}

/**
 * \brief Extract minimum component from pixel value.
 *
 * Arg #1 is expected to be a l_uint32 (pixel).
 * Arg #2 is expected to be a string (min or max).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
MinComponent(lua_State *L)
{
    LL_FUNC("MinComponent");
    l_uint32 pixel = ll_check_l_uint32(_fun, L, 1);
    ll_push_l_int32(_fun, L, extractMinMaxComponent(pixel, L_CHOOSE_MIN));
    return 1;
}

/**
 * \brief Extract maximum component from pixel value.
 *
 * Arg #1 is expected to be a l_uint32 (pixel).
 * Arg #2 is expected to be a string (min or max).
 *
 * \param L Lua state
 * \return 1 Pix* on the Lua stack
 */
static int
MaxComponent(lua_State *L)
{
    LL_FUNC("MaxComponent");
    l_uint32 pixel = ll_check_l_uint32(_fun, L, 1);
    ll_push_l_int32(_fun, L, extractMinMaxComponent(pixel, L_CHOOSE_MAX));
    return 1;
}

/**
 * \brief Compress() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (%data, %nin).
 * </pre>
 * \param L Lua state
 * \return 1 lstring on the Lua stack
 */
static int
Compress(lua_State *L)
{
    LL_FUNC("Compress");
    size_t nin = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &nin);
    l_uint8 *datain = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(data));
    size_t nout = 0;
    l_uint8 *dataout = zlibCompress(datain, nin, &nout);
    return ll_push_bytes(_fun, L, dataout, nout);
}

/**
 * \brief Uncompress() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (%data, %nin).
 * </pre>
 * \param L Lua state
 * \return 1 lstring on the Lua stack
 */
static int
Uncompress(lua_State *L)
{
    LL_FUNC("Uncompress");
    size_t nin = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &nin);
    l_uint8 *datain = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(data));
    size_t nout = 0;
    l_uint8 *dataout = zlibUncompress(datain, nin, &nout);
    return ll_push_bytes(_fun, L, dataout, nout);
}


/**
 * \brief Check Lua stack at index %arg for udata of class lualept.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the LEPT contained in the user data
 */
LuaLept *
ll_check_lualept(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<LuaLept>(_fun, L, arg, TNAME);
}

/**
 * \brief Push LEPT* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param ll pointer to the LEPT
 * \return 1 LEPT* on the Lua stack
 */
int
ll_push_lualept(const char *_fun, lua_State *L, LuaLept *ll)
{
    if (!ll)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, ll);
}

/**
 * \brief Create a new LuaLept*.
 * \param L Lua state
 * \return 1 LEPT* on the Lua stack
 */
int
ll_new_lualept(lua_State *L)
{
    FUNC("ll_new_LuaLept");
    static const char lept_prefix[] = "leptonica-";
    LuaLept *lept = ll_calloc<LuaLept>(_fun, L, 1);
    const char* lept_ver = getLeptonicaVersion();
    const lua_Number *lua_ver = lua_version(L);

    snprintf(lept->str_version, sizeof(lept->str_version), "%s", PACKAGE_VERSION);

    snprintf(lept->str_version_lua, sizeof(lept->str_version_lua), "%d.%d",
             static_cast<int>(lua_ver[0])/100, static_cast<int>(lua_ver[0])%100);

    if (!strncmp(lept_ver, lept_prefix, strlen(lept_prefix)))
        lept_ver += strlen(lept_prefix);
    snprintf(lept->str_version_lept, sizeof(lept->str_version_lept), "%s", lept_ver);

    return ll_push_udata(_fun, L, TNAME, lept);
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table.
 * \param L Lua state
 * \return 1 table on the Lua stack
 */
int
luaopen_lualept(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_lualept},
        {"DebugOn",                 DebugOn},
        {"DebugOff",                DebugOff},
        {"Debug",                   Debug},
        {"Create",                  Create},
        {"Version",                 Version},
        {"ComposeRGB",              ComposeRGB},
        {"ComposeRGBA",             ComposeRGBA},
        {"RGB",                     ComposeRGB},
        {"RGBA",                    ComposeRGBA},
        {"Color",                   Color},
        {"ToRGB",                   ToRGB},
        {"ToRGBA",                  ToRGBA},
        {"MakeGrayQuantIndexTable", MakeGrayQuantIndexTable},
        {"MakeGrayQuantTableArb",   MakeGrayQuantTableArb},
        {"MinMaxComponent",         MinMaxComponent},
        {"MinComponent",            MinComponent},   /* alias without 2nd parameter */
        {"MaxComponent",            MaxComponent},   /* alias without 2nd parameter */
        {"Compress",                Compress},
        {"Uncompress",              Uncompress},
        LUA_SENTINEL
    };
    FUNC("luaopen_lualept");

    ll_register_class(_fun, L, TNAME, methods);
    ll_set_global_table(_fun, L, TNAME, ll_new_lualept);

    ll_open_Amap(L);
    ll_open_Aset(L);
    ll_open_Bmf(L);
    ll_open_Box(L);
    ll_open_Boxa(L);
    ll_open_Boxaa(L);
    ll_open_ByteBuffer(L);
    ll_open_Bytea(L);
    ll_open_CCBord(L);
    ll_open_CCBorda(L);
    ll_open_CompData(L);
    ll_open_DPix(L);
    ll_open_Dewarp(L);
    ll_open_Dewarpa(L);
    ll_open_Dna(L);
    ll_open_Dnaa(L);
    ll_open_DnaHash(L);
    ll_open_DLList(L);
    ll_open_FPix(L);
    ll_open_FPixa(L);
    ll_open_Kernel(L);
    ll_open_Numa(L);
    ll_open_Numaa(L);
    ll_open_PdfData(L);
    ll_open_Pix(L);
    ll_open_PixColormap(L);
    ll_open_PixComp(L);
    ll_open_PixTiling(L);
    ll_open_Pixa(L);
    ll_open_PixaComp(L);
    ll_open_Pixaa(L);
    ll_open_Pta(L);
    ll_open_Ptaa(L);
    ll_open_Sarray(L);
    ll_open_Sel(L);
    ll_open_Sela(L);
    ll_open_Stack(L);
    ll_open_WShed(L);
    return 1;
}

/**
 * \brief Run a Lua script.
 * \param name filename of an external file to run, if script == nullptr
 * \param script if != nullptr, load the string and run it
 *        using %name as chunk name for debug output
 * \param set_vars optional array of global variable definitions to set.
 * \param get_vars optional array of global variable definitions to get.
 * \return 0 on success, or 1 on error
 */
int
ll_run(const char *name, const char *script, ll_global_var_t *set_vars, ll_global_var_t *get_vars)
{
    FUNC("ll_run");
    lua_State *L;
    int res;

    /* Disable Leptonica debugging (pixDisplay ...) */
    setLeptDebugOK(FALSE);

    /* Allocate a new Lua state */
    L = luaL_newstate();

    /* Open all Lua libraries */
    luaL_openlibs(L);

    /* Register our libraries */
    luaopen_lualept(L);

    /* Set any global variables */
    ll_set_all_globals(_fun, L, set_vars);

    if (nullptr == script) {
        /* load from a file %name */
        res = luaL_loadfile(L, name);
        if (LUA_OK != res) {
            const char* msg = lua_tostring(L, -1);
            lua_close(L);
            return ERROR_INT(msg, _fun, 1);
        }
    } else {
        /* load from text string %script and use %name as chunk name */
        size_t size = strlen(script);
        res = luaL_loadbufferx(L, script, size, name, "t");
        if (LUA_OK != res) {
            const char* msg = lua_tostring(L, -1);
            lua_close(L);
            return ERROR_INT(msg, _fun, 1);
        }
    }

    /* Ask Lua to run our script */
    res = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (LUA_OK != res) {
        const char* msg = lua_tostring(L, -1);
        lua_close(L);
        return ERROR_INT(msg, _fun, 1);
    }

    /* Get any global variables */
    ll_get_all_globals(_fun, L, get_vars);
    lua_close(L);

    return 0;
}
