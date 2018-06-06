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
 * - DoubleLinkedList
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

#if defined(HAVE_STRCASECMP)
#define ll_strcasecmp strcasecmp
#elif defined(HAVE_STRICMP)
#define ll_strcasecmp stricmp
#else
/**
 * \brief Our own version of strcasecmp(3).
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

/**
 * @brief Bit mask (flags) for enabled log output
 */
static int dbg_enabled = LOG_REGISTER | LOG_SDL2 | LOG_NEW_CLASS | LOG_NEW_PARAM | LOG_TAKE;

/**
 * @brief Return a time stamp for the current date and time
 * @return Temporary string with date and time
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
 * @brief Print debug output to stdout
 * @param enable bit mask (flag) which defines the type of log output
 * @param format format string followed by optional varargs
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
 * \param arg argument index
 * \return pointer to the udata
 */
void *
ll_ludata(const char *_fun, lua_State *L, int arg)
{
    const void *cptr = lua_islightuserdata(L, arg) ? lua_topointer(L, arg) : nullptr;
    /* XXX: deconstify */
    void *ptr = reinterpret_cast<void *>(reinterpret_cast<l_intptr_t>(cptr));
    UNUSED(_fun);
    return ptr;
}

/**
 * \brief Check Lua stack at index %arg for udata with %tname.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg argument index
 * \param tname tname of the expected udata
 * \return pointer to the udata
 */
void **
ll_udata(const char *_fun, lua_State *L, int arg, const char* tname)
{
    char msg[128];
    void **pptr = reinterpret_cast<void **>(luaL_checkudata(L, arg, tname));
    if (nullptr == pptr) {
        snprintf(msg, sizeof(msg), "%s: expected '%s'", _fun, tname);
    }
    luaL_argcheck(L, pptr != nullptr, arg, msg);
    return pptr;
}

/**
 * \brief Test if Lua stack at index %arg is a number.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * @brief Create a global lua_CFunction (%cfunct) with name (%tname)
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param tname table name for the udata
 * \param cfunct lua_Cfunction to register
 * @return 0 for nothing on the Lua stack
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
 * @brief Create a global table with name (%tname)
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param tname table name for the udata
 * \param cfunct C function which creates a table on the Lua stack
 * @return 0 for nothing on the Lua stack
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \brief Check if an argument is a lua_Integer in the range of char.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \brief Return an user data argument (%arg) as luaL_Stream*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_check_l_float32(const char *_fun, lua_State *L, int arg)
{
    lua_Number val = luaL_checknumber(L, arg);

    if (val < static_cast<lua_Number>(-FLT_MAX) || val > static_cast<lua_Number>(FLT_MAX)) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        _fun, arg, (lua_Number)-FLT_MAX, val, (lua_Number)FLT_MAX);
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return static_cast<l_float32>(val);
}

/**
 * \brief Return an argument lua_Integer in the range of l_float32 or the default.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the integer
 * \param def default value
 * \return l_float32 for the number; lua_error if out of bounds
 */
l_float32
ll_opt_l_float32(const char *_fun, lua_State *L, int arg, l_float32 def)
{
    lua_Number val = luaL_optnumber(L, arg, (lua_Number)def);

    if (val < static_cast<lua_Number>(-FLT_MAX) || val > static_cast<lua_Number>(FLT_MAX)) {
        lua_pushfstring(L, "%s: l_float32 #%d out of bounds (%f < %f < %f)",
                        _fun, arg, (lua_Number)-FLT_MAX, val, (lua_Number)FLT_MAX);
        lua_error(L);
        return 0.0f;    /* NOTREACHED */
    }
    return static_cast<l_float32>(val);
}

/**
 * \brief Check if an argument is a lua_Number in the range of l_float64.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \brief Push a string to the Lua stack listing the table of options.
 * \param L pointer to the lua_State
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \param msg message to prepend to the listing of options.
 * \return string with enumeration value names and their keys
 */
int
ll_list_tbl_options(lua_State *L, const lept_enum *tbl, size_t len, const char *msg)
{
    char str[256];
    luaL_Buffer B;
    l_int32 value = -1;
    size_t i;

    luaL_buffinit(L, &B);

    /* Add the error message, if any */
    if (nullptr != msg)
        luaL_addstring(&B, msg);

    /* List the options */
    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (p->value != value) {
            /* this is a new enumeration value */
            if (nullptr != msg || i > 0)
                luaL_addchar(&B, '\n');
            snprintf(str, sizeof(str), "%s (%d): ", p->name, p->value);
            luaL_addstring(&B, str);
            value = p->value;
        } else {
            /* this is another key name for the same enumeration value */
            luaL_addstring(&B, ", ");
        }
        /* list the key string */
        snprintf(str, sizeof(str), "'%s' ", p->key);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Return a const char* with the (first) key for a enumeration value.
 * \param value value to search for
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \return pointer to string with the (first) key for that value
 */
const char*
ll_string_tbl(l_int32 value, const lept_enum *tbl, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (p->value == value)
            return p->key;
    }
    return "<undefined>";
}

/**
 * \brief Find a option %str in a lept_enum_t array %tbl of size %len.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \param tbl table of key/name/value tuples
 * \param len length of that table
 * \return value or def
 */
l_int32
ll_check_tbl(const char *_fun, lua_State *L, int arg, l_int32 def, const lept_enum *tbl, size_t len)
{
    char msg[256];
    size_t i;

    const char* str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;
    if (!str)
        return def;

    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (!ll_strcasecmp(str, p->key))
            return p->value;
    }

    snprintf(msg, sizeof(msg), "%s: Invalid option #%d '%s'\n"
                               "Enumeration options:",
             _fun, arg, str);
    ll_list_tbl_options(L, tbl, len, msg);
    lua_error(L);
    return def;    /* NOTREACHED */

}

#define TBL_ENTRY(key,ENUMVALUE) { key, #ENUMVALUE, ENUMVALUE }

/**
 * \brief Table of debug log flag names and enumeration values.
 */
static const lept_enum tbl_debug[] = {
    TBL_ENTRY("register",       LOG_REGISTER),
    TBL_ENTRY("new",            LOG_NEW_PARAM | LOG_NEW_CLASS),
    TBL_ENTRY("new-param",      LOG_NEW_PARAM),
    TBL_ENTRY("new-class",      LOG_NEW_CLASS),
    TBL_ENTRY("destroy",        LOG_DESTROY),
    TBL_ENTRY("boolean",        LOG_PUSH_BOOLEAN | LOG_CHECK_BOOLEAN),
    TBL_ENTRY("push-boolean",   LOG_PUSH_BOOLEAN),
    TBL_ENTRY("check-boolean",  LOG_CHECK_BOOLEAN),
    TBL_ENTRY("integer",        LOG_PUSH_INTEGER | LOG_CHECK_INTEGER),
    TBL_ENTRY("push-integer",   LOG_PUSH_INTEGER),
    TBL_ENTRY("check-integer",  LOG_CHECK_INTEGER),
    TBL_ENTRY("number",         LOG_PUSH_NUMBER | LOG_CHECK_NUMBER),
    TBL_ENTRY("push-number",    LOG_PUSH_NUMBER),
    TBL_ENTRY("check-number",   LOG_CHECK_NUMBER),
    TBL_ENTRY("string",         LOG_PUSH_STRING | LOG_CHECK_STRING),
    TBL_ENTRY("push-string",    LOG_PUSH_STRING),
    TBL_ENTRY("check-string",   LOG_CHECK_STRING),
    TBL_ENTRY("udata",          LOG_PUSH_UDATA | LOG_CHECK_UDATA),
    TBL_ENTRY("push-udata",     LOG_PUSH_UDATA),
    TBL_ENTRY("check-udata",    LOG_CHECK_UDATA),
    TBL_ENTRY("array",          LOG_PUSH_ARRAY | LOG_CHECK_ARRAY),
    TBL_ENTRY("push-array",     LOG_PUSH_ARRAY),
    TBL_ENTRY("check-array",    LOG_CHECK_ARRAY),
    TBL_ENTRY("sdl2",           LOG_SDL2)
};

/**
 * \brief Check for a debug flag as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_debug(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_debug, ARRAYSIZE(tbl_debug));
}

/**
 * \brief Return a string representing enabled debug flags.
 * \param flag debug enable flags
 * \return pointer to const string
 */
const char*
ll_string_debug(l_int32 flag)
{
    static char str[1024];
    char *dst = str;
    size_t i;

    for (i = 0; i < ARRAYSIZE(tbl_debug); i++) {
        const lept_enum *e = &tbl_debug[i];
        l_int32 bit = e->value;
        if (bit != (bit & flag))
            continue;
        flag &= ~bit;
        if (dst > str) {
            dst += snprintf(dst, sizeof(str) - (size_t(dst - str)), "|");
        }
        dst += snprintf(dst, sizeof(str) - (size_t(dst - str)), "%s", e->key);
    }
    return str;
}

/**
 * \brief Table of access/storage flag names and enumeration values.
 * <pre>
 * Access and storage flags.
 * L_NOCOPY      : do not copy the object; do not delete the ptr
 * L_INSERT      : stuff it in; do not copy or clone
 * L_COPY        : make/use a copy of the object
 * L_CLONE       : make/use clone (ref count) of the object
 * L_COPY_CLONE  : make a new array object (e.g., pixa) and fill the array with clones (e.g., pix)
 * </pre>
 */
static const lept_enum tbl_access_storage[] = {
    TBL_ENTRY("nocopy",         L_NOCOPY),      /* do not copy the object; do not delete the ptr */
    TBL_ENTRY("insert",         L_INSERT),      /* stuff it in; do not copy or clone */
    TBL_ENTRY("copy",           L_COPY),        /* make/use a copy of the object */
    TBL_ENTRY("clone",          L_CLONE),       /* make/use clone (ref count) of the object */
    TBL_ENTRY("copy-clone",     L_COPY_CLONE)   /* make a new array object (e.g., pixa) and fill the array with clones (e.g., pix) */
};

/**
 * \brief Check for an optional storage flag as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_access_storage(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Return the name for an access/storage flag value.
 * \param flag access/storage flag
 * \return pointer to const string
 */
const char*
ll_string_access_storage(l_int32 flag)
{
    return ll_string_tbl(flag, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Table of access/storage flag names and enumeration values.
 * <pre>
 * 16-bit conversion flags.
 * </pre>
 */
static const lept_enum tbl_more_less_clip[] = {
    TBL_ENTRY("ls-byte",        L_LS_BYTE),
    TBL_ENTRY("lsb",            L_LS_BYTE),
    TBL_ENTRY("l",              L_LS_BYTE),
    TBL_ENTRY("ms-byte",        L_MS_BYTE),
    TBL_ENTRY("msb",            L_MS_BYTE),
    TBL_ENTRY("m",              L_MS_BYTE),
    TBL_ENTRY("auto-byte",      L_AUTO_BYTE),
    TBL_ENTRY("auto",           L_AUTO_BYTE),
    TBL_ENTRY("a",              L_AUTO_BYTE),
    TBL_ENTRY("clip-to-ff",     L_CLIP_TO_FF),
    TBL_ENTRY("clip-ff",        L_CLIP_TO_FF),
    TBL_ENTRY("ff",             L_CLIP_TO_FF),
    TBL_ENTRY("ls-two-bytes",   L_LS_TWO_BYTES),
    TBL_ENTRY("ls-2-bytes",     L_LS_TWO_BYTES),
    TBL_ENTRY("ls2b",           L_LS_TWO_BYTES),
    TBL_ENTRY("l2",             L_LS_TWO_BYTES),
    TBL_ENTRY("ms-two-bytes",   L_MS_TWO_BYTES),
    TBL_ENTRY("ms-2-bytes",     L_MS_TWO_BYTES),
    TBL_ENTRY("ms2b",           L_MS_TWO_BYTES),
    TBL_ENTRY("m2",             L_MS_TWO_BYTES),
    TBL_ENTRY("clip-to-ffff",   L_CLIP_TO_FFFF),
    TBL_ENTRY("clip-ffff",      L_CLIP_TO_FFFF),
    TBL_ENTRY("ffff",           L_CLIP_TO_FFFF)
};

/**
 * \brief Check for a byte type as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_more_less_clip(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_more_less_clip, ARRAYSIZE(tbl_more_less_clip));
}

/**
 * \brief Return the name for byte type value.
 * \param flag access/storage flag
 * \return pointer to const string
 */
const char*
ll_string_more_less_clip(l_int32 flag)
{
    return ll_string_tbl(flag, tbl_more_less_clip, ARRAYSIZE(tbl_more_less_clip));
}

/**
 * \brief Table of PDF encoding format names and enumeration values.
 * <pre>
 * Pdf formatted encoding types.
 * </pre>
 */
static const lept_enum tbl_encoding[] = {
    TBL_ENTRY("default-encode", L_DEFAULT_ENCODE),
    TBL_ENTRY("default",        L_DEFAULT_ENCODE),
    TBL_ENTRY("jpeg-encode",    L_JPEG_ENCODE),
    TBL_ENTRY("jpeg",           L_JPEG_ENCODE),
    TBL_ENTRY("jpg",            L_JPEG_ENCODE),
    TBL_ENTRY("g4-encode",      L_G4_ENCODE),
    TBL_ENTRY("g4",             L_G4_ENCODE),
    TBL_ENTRY("flate-encode",   L_FLATE_ENCODE),
    TBL_ENTRY("flate",          L_FLATE_ENCODE),
    TBL_ENTRY("jp2k-encode",    L_JP2K_ENCODE),
    TBL_ENTRY("jp2k",           L_JP2K_ENCODE)
};

/**
 * \brief Check for an PDF encoding format name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_encoding(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_encoding, ARRAYSIZE(tbl_encoding));
}

/**
 * \brief Return the name for an input file format (IFF_*).
 * \param encoding encoding enumeration value
 * \return pointer to const string
 */
const char*
ll_string_encoding(l_int32 encoding)
{
    return ll_string_tbl(encoding, tbl_encoding, ARRAYSIZE(tbl_encoding));
}

/**
 * \brief Table of input file format names and enumeration values.
 * <pre>
 * The IFF_DEFAULT flag is used to write the file out in the
 * same (input) file format that the pix was read from.  If the pix
 * was not read from file, the input format field will be
 * IFF_UNKNOWN and the output file format will be chosen to
 * be compressed and lossless; namely, IFF_TIFF_G4 for d = 1
 * and IFF_PNG for everything else.
 *
 * In the future, new format types that have defined extensions
 * will be added before IFF_DEFAULT, and will be kept in sync with
 * the file format extensions in writefile.c.  The positions of
 * file formats before IFF_DEFAULT will remain invariant.
 * </pre>
 */
static const lept_enum tbl_input_format[] = {
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
 * \brief Check for an image format name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_input_format(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Return the name for an input file format (IFF_*).
 * \param format input file format value
 * \return pointer to const string
 */
const char*
ll_string_input_format(int format)
{
    return ll_string_tbl(format, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Table of key type names for AMAP and ASET.
 * <pre>
 * The three valid key types for red-black trees, maps and sets.
 * </pre>
 */
static const lept_enum tbl_keytype[] = {
    TBL_ENTRY("int",        L_INT_TYPE),
    TBL_ENTRY("uint",       L_UINT_TYPE),
    TBL_ENTRY("float",      L_FLOAT_TYPE)
};

/**
 * \brief Check for an L_AMAP keytype name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_keytype(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Return a string for the keytype of an AMAP/ASET.
 * \param type key type value
 * \return const string with the name
 */
const char*
ll_string_keytype(l_int32 type)
{
    return ll_string_tbl(type, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Table of choice names and enumeration values.
 * <pre>
 * Set selection flags.
 * </pre>
 */
static const lept_enum tbl_consecutive_skip_by[] = {
    TBL_ENTRY("choose-consecutive", L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("consecutive",        L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("cons",               L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("c",                  L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("choose-skip-by",     L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip-by",            L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip",               L_CHOOSE_SKIP_BY),
    TBL_ENTRY("s",                  L_CHOOSE_SKIP_BY)
};

/**
 * \brief Check for an choose name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_consecutive_skip_by(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Return a string for the choice between consecutive and skip_by.
 * \param choice consecutive/skip_by enumeration value
 * \return const string with the name
 */
const char*
ll_string_consecutive_skip_by(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Table of color component names and enumeration values.
 * <pre>
 *  Notes:
 *      (1) These are the byte indices for colors in 32 bpp images.
 *          They are used through the GET/SET_DATA_BYTE accessors.
 *          The 4th byte, typically known as the "alpha channel" and used
 *          for blending, is used to a small extent in leptonica.
 *      (2) Do not change these values!  If you redefine them, functions
 *          that have the shifts hardcoded for efficiency and conciseness
 *          (instead of using the constants below) will break.  These
 *          functions are labelled with "***"  next to their names at
 *          the top of the files in which they are defined.
 *      (3) The shifts to extract the red, green, blue and alpha components
 *          from a 32 bit pixel are defined here.
 * </pre>
 */
static const lept_enum tbl_component[] = {
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
 * \brief Check for an component name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_component(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Return a string for the color component name.
 * \param component color component value
 * \return const string with the name
 */
const char*
ll_string_component(l_int32 component)
{
    return ll_string_tbl(component, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Table of color compression names and enumeration values.
 * <pre>
 * Compression to use for PDF.
 * </pre>
 */
static const lept_enum tbl_compression[] = {
    TBL_ENTRY("default",        IFF_DEFAULT),
    TBL_ENTRY("def",            IFF_DEFAULT),
    TBL_ENTRY("def",            IFF_DEFAULT),
    TBL_ENTRY("d",              IFF_DEFAULT),
    TBL_ENTRY("tiff-g4",        IFF_TIFF_G4),
    TBL_ENTRY("g4",             IFF_TIFF_G4),
    TBL_ENTRY("png",            IFF_PNG),
    TBL_ENTRY("p",              IFF_PNG),
    TBL_ENTRY("jfif-jpeg",      IFF_JFIF_JPEG),
    TBL_ENTRY("jpeg",           IFF_JFIF_JPEG),
    TBL_ENTRY("jpg",            IFF_JFIF_JPEG),
    TBL_ENTRY("jfif",           IFF_JFIF_JPEG),
    TBL_ENTRY("j",              IFF_JFIF_JPEG)
};

/**
 * \brief Check for an compression name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_compression(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_compression, ARRAYSIZE(tbl_compression));
}

/**
 * \brief Return a string for the color compression name.
 * \param compression color compression value
 * \return const string with the name
 */
const char*
ll_string_compression(l_int32 compression)
{
    return ll_string_tbl(compression, tbl_compression, ARRAYSIZE(tbl_compression));
}

/**
 * \brief Table of choice min/max names and enumeration values.
 * <pre>
 * Min/max selection flags.
 * </pre>
 */
static const lept_enum tbl_choose_min_max[] = {
    TBL_ENTRY("choose-min",         L_CHOOSE_MIN),
    TBL_ENTRY("min",                L_CHOOSE_MIN),
    TBL_ENTRY("choose-max",         L_CHOOSE_MAX),
    TBL_ENTRY("max",                L_CHOOSE_MAX),
    TBL_ENTRY("choose-maxdiff",     L_CHOOSE_MAXDIFF),
    TBL_ENTRY("maxdiff",            L_CHOOSE_MAXDIFF),
    TBL_ENTRY("choose-min-boost",   L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("min-boost",          L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("minb",               L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("choose-max-boost",   L_CHOOSE_MAX_BOOST),
    TBL_ENTRY("max-boost",          L_CHOOSE_MAX_BOOST),
    TBL_ENTRY("maxb",               L_CHOOSE_MAX_BOOST)
};

/**
 * \brief Check for a min/max name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_choose_min_max(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param choice min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_choose_min_max(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Table of white/black is max names and enumeration values.
 * <pre>
 * Flags for 8 bit and 16 bit pixel sums.
 * </pre>
 */
static const lept_enum tbl_what_is_max[] = {
    TBL_ENTRY("white-is-max",    L_WHITE_IS_MAX),
    TBL_ENTRY("white",           L_WHITE_IS_MAX),
    TBL_ENTRY("w",               L_WHITE_IS_MAX),
    TBL_ENTRY("black-is-max",    L_BLACK_IS_MAX),
    TBL_ENTRY("black",           L_BLACK_IS_MAX),
    TBL_ENTRY("b",               L_BLACK_IS_MAX)
};

/**
 * \brief Check for a white or black is max name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_what_is_max(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param what white or black is max enumeration value
 * \return const string with the name
 */
const char*
ll_string_what_is_max(l_int32 what)
{
    return ll_string_tbl(what, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Table of get white/black val names and enumeration values.
 * <pre>
 * Flags for getting white or black value.
 * </pre>
 */
static const lept_enum tbl_getval[] = {
    TBL_ENTRY("get-white-val",  L_GET_WHITE_VAL),
    TBL_ENTRY("white-val",      L_GET_WHITE_VAL),
    TBL_ENTRY("white",          L_GET_WHITE_VAL),
    TBL_ENTRY("w",              L_GET_WHITE_VAL),
    TBL_ENTRY("get-black-val",  L_GET_BLACK_VAL),
    TBL_ENTRY("black-val",      L_GET_BLACK_VAL),
    TBL_ENTRY("black",          L_GET_BLACK_VAL),
    TBL_ENTRY("b",              L_GET_BLACK_VAL)
};

/**
 * \brief Check for a L_GET_XXXX_VAL name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_getval(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param val white or black getval enumeration value
 * \return const string with the name
 */
const char*
ll_string_getval(l_int32 val)
{
    return ll_string_tbl(val, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Table of direction names and enumeration values.
 * <pre>
 * Line orientation flags.
 * </pre>
 */
static const lept_enum tbl_direction[] = {
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
 * \brief Check for a L_XXX_LINE name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_direction(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Return a string for the direction name.
 * \param dir horizontal or vertical line direction enumeration value
 * \return const string with the name
 */
const char*
ll_string_direction(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Table of set white/black names and enumeration values.
 * <pre>
 * Flags for setting to white or black.
 * </pre>
 */
static const lept_enum tbl_set_black_white[] = {
    TBL_ENTRY("set-white",      L_SET_WHITE),
    TBL_ENTRY("white",          L_SET_WHITE),
    TBL_ENTRY("w",              L_SET_WHITE),
    TBL_ENTRY("set-black",      L_SET_BLACK),
    TBL_ENTRY("black",          L_SET_BLACK),
    TBL_ENTRY("b",              L_SET_BLACK)
};

/**
 * \brief Check for a set white or black name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_set_black_white(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_set_black_white, ARRAYSIZE(tbl_set_black_white));
}

/**
 * \brief Return a string for the choice between setting black or white.
 * \param which set black or white enumeration value
 * \return const string with the name
 */
const char*
ll_string_set_black_white(l_int32 which)
{
    return ll_string_tbl(which, tbl_set_black_white, ARRAYSIZE(tbl_set_black_white));
}

/**
 * \brief Table of rasterop names and enumeration values.
 * <pre>
 * The following operation bit flags have been modified from
 * Sun's pixrect.h.
 *
 * The 'op' in 'rasterop' is represented by an integer
 * composed with Boolean functions using the set of five integers
 * given below.  The integers, and the op codes resulting from
 * boolean expressions on them, need only be in the range from 0 to 15.
 * The function is applied on a per-pixel basis.
 *
 * Examples: the op code representing ORing the src and dest
 * is computed using the bit OR, as PIX_SRC | PIX_DST;  the op
 * code representing XORing src and dest is found from
 * PIX_SRC ^ PIX_DST;  the op code representing ANDing src and dest
 * is found from PIX_SRC & PIX_DST.  Note that
 * PIX_NOT(PIX_CLR) = PIX_SET, and v.v., as they must be.
 *
 * We use the following set of definitions:
 *
 *      #define   PIX_SRC      0xc
 *      #define   PIX_DST      0xa
 *      #define   PIX_NOT(op)  (op) ^ 0xf
 *      #define   PIX_CLR      0x0
 *      #define   PIX_SET      0xf
 *
 * These definitions differ from Sun's, in that Sun left-shifted
 * each value by 1 pixel, and used the least significant bit as a
 * flag for the "pseudo-operation" of clipping.  We don't need
 * this bit, because it is both efficient and safe ALWAYS to clip
 * the rectangles to the src and dest images, which is what we do.
 * See the notes in rop.h on the general choice of these bit flags.
 *
 * [If for some reason you need compatibility with Sun's xview package,
 * you can adopt the original Sun definitions to avoid redefinition conflicts:
 *
 *      #define   PIX_SRC      (0xc << 1)
 *      #define   PIX_DST      (0xa << 1)
 *      #define   PIX_NOT(op)  ((op) ^ 0x1e)
 *      #define   PIX_CLR      (0x0 << 1)
 *      #define   PIX_SET      (0xf << 1)
 * ]
 *
 * We have, for reference, the following 16 unique op flags:
 *
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
 *
 * </pre>
 */
static const lept_enum tbl_rasterop[] = {
    TBL_ENTRY("clr",            PIX_CLR),
    TBL_ENTRY("set",            PIX_SET),
    TBL_ENTRY("src",            PIX_SRC),
    TBL_ENTRY("dst",            PIX_DST),
    TBL_ENTRY("!src",           PIX_NOT(PIX_SRC)),
    TBL_ENTRY("!dst",           PIX_NOT(PIX_DST)),
    TBL_ENTRY("src|dst",        PIX_SRC | PIX_DST),
    TBL_ENTRY("paint",          PIX_SRC | PIX_DST),
    TBL_ENTRY("src&dst",        PIX_SRC & PIX_DST),
    TBL_ENTRY("mask",           PIX_SRC & PIX_DST),
    TBL_ENTRY("src^dst",        PIX_SRC ^ PIX_DST),
    TBL_ENTRY("xor",            PIX_SRC ^ PIX_DST),
    TBL_ENTRY("!src|dst",       PIX_NOT(PIX_SRC) | PIX_DST),
    TBL_ENTRY("!src&dst",       PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("subtract",       PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("sub",            PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("src|!dst",       PIX_SRC | PIX_NOT(PIX_DST)),
    TBL_ENTRY("src&!dst",       PIX_SRC & PIX_NOT(PIX_DST)),
    TBL_ENTRY("!(src|dst)",     PIX_NOT(PIX_SRC | PIX_DST)),
    TBL_ENTRY("!(src&dst)",     PIX_NOT(PIX_SRC & PIX_DST)),
    TBL_ENTRY("!(src^dst)",     PIX_NOT(PIX_SRC ^ PIX_DST))
};

/**
 * \brief Check for a rasterop name as string.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rasterop(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Return a string for the raster operation.
 * \param op enumeration value of the raster operation
 * \return const string with the name
 */
const char*
ll_string_rasterop(l_int32 op)
{
    return ll_string_tbl(op, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Table of JPEG reader hint names and enumeration values.
 * <pre>
 * Hinting bit flags in jpeg reader.
 * </pre>
 */
static const lept_enum tbl_hint[] = {
    TBL_ENTRY("none",               0),
    TBL_ENTRY("read-luminance",     L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("luminance",          L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("lum",                L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("l",                  L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("fail-on-bad-data",   L_JPEG_FAIL_ON_BAD_DATA),
    TBL_ENTRY("fail",               L_JPEG_FAIL_ON_BAD_DATA),
    TBL_ENTRY("f",                  L_JPEG_FAIL_ON_BAD_DATA),
    TBL_ENTRY("b",                  L_JPEG_READ_LUMINANCE | L_JPEG_FAIL_ON_BAD_DATA)
};

/**
 * \brief Check for a JPEG reader hint name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_hint(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_hint, ARRAYSIZE(tbl_hint));
}

/**
 * \brief Return a string for the JPEG reader hint.
 * \param dir enumeration value of the search direction
 * \return const string with the name
 */
const char*
ll_string_hint(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_hint, ARRAYSIZE(tbl_hint));
}

/**
 * \brief Table of search direction names and enumeration values.
 * <pre>
 * Direction flags for grayscale morphology, granulometry,         *
 * composable Sels, convolution, etc.
 * </pre>
 */
static const lept_enum tbl_searchdir[] = {
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
 * \brief Check for a search direction name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_searchdir(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Return a string for the search direction.
 * \param dir enumeration value of the search direction
 * \return const string with the name
 */
const char*
ll_string_searchir(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Table of stats type names and enumeration values.
 * <pre>
 * Flags for data type converted from Numa.
 * </pre>
 */
static const lept_enum tbl_number_value[] = {
    TBL_ENTRY("integer-value",      L_INTEGER_VALUE),
    TBL_ENTRY("integer",            L_INTEGER_VALUE),
    TBL_ENTRY("int",                L_INTEGER_VALUE),
    TBL_ENTRY("i",                  L_INTEGER_VALUE),
    TBL_ENTRY("float-value",        L_FLOAT_VALUE),
    TBL_ENTRY("float",              L_FLOAT_VALUE),
    TBL_ENTRY("f",                  L_FLOAT_VALUE)
};

/**
 * \brief Check for a stats type name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_number_value(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_number_value, ARRAYSIZE(tbl_number_value));
}

/**
 * \brief Return a string for the stats type enumeration value.
 * \param type enumeration value of the stats type
 * \return const string with the name
 */
const char*
ll_string_number_value(l_int32 type)
{
    return ll_string_tbl(type, tbl_number_value, ARRAYSIZE(tbl_number_value));
}

/**
 * \brief Table of position names and enumeration values.
 * <pre>
 * Pdf multi image flags.
 * </pre>
 */
static const lept_enum tbl_position[] = {
    TBL_ENTRY("single-image",       0),
    TBL_ENTRY("single",             0),
    TBL_ENTRY("s",                  0),
    TBL_ENTRY("first-image",        L_FIRST_IMAGE),
    TBL_ENTRY("first",              L_FIRST_IMAGE),
    TBL_ENTRY("f",                  L_FIRST_IMAGE),
    TBL_ENTRY("next-image",         L_NEXT_IMAGE),
    TBL_ENTRY("next",               L_NEXT_IMAGE),
    TBL_ENTRY("n",                  L_NEXT_IMAGE),
    TBL_ENTRY("last-image",         L_LAST_IMAGE),
    TBL_ENTRY("last",               L_LAST_IMAGE),
    TBL_ENTRY("l",                  L_LAST_IMAGE)
};

/**
 * \brief Check for a position name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_position(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_position, ARRAYSIZE(tbl_position));
}

/**
 * \brief Return a string for the position enumeration value.
 * \param type enumeration value of the position
 * \return const string with the name
 */
const char*
ll_string_position(l_int32 type)
{
    return ll_string_tbl(type, tbl_position, ARRAYSIZE(tbl_position));
}

/**
 * \brief Table of statistical measures names and enumeration values.
 * <pre>
 * Statistical measures.
 * </pre>
 */
static const lept_enum tbl_stats_type[] = {
    TBL_ENTRY("mean-absval",        L_MEAN_ABSVAL),
    TBL_ENTRY("mean-abs",           L_MEAN_ABSVAL),
    TBL_ENTRY("mean",               L_MEAN_ABSVAL),
    TBL_ENTRY("median-val",         L_MEDIAN_VAL),
    TBL_ENTRY("median",             L_MEDIAN_VAL),
    TBL_ENTRY("med",                L_MEDIAN_VAL),
    TBL_ENTRY("mode-val",           L_MODE_VAL),
    TBL_ENTRY("mode-count",         L_MODE_COUNT),
    TBL_ENTRY("root-mean-square",   L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("rms",                L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("r",                  L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("standard-deviation", L_STANDARD_DEVIATION),
    TBL_ENTRY("stddev",             L_STANDARD_DEVIATION),
    TBL_ENTRY("s",                  L_STANDARD_DEVIATION),
    TBL_ENTRY("variance",           L_VARIANCE),
    TBL_ENTRY("var",                L_VARIANCE),
    TBL_ENTRY("v",                  L_VARIANCE)
};

/**
 * \brief Check for a stats type name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_stats_type(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Return a string for the stats type enumeration value.
 * \param type enumeration value of the stats type
 * \return const string with the name
 */
const char*
ll_string_stats_type(l_int32 type)
{
    return ll_string_tbl(type, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Table of color component selection names and enumeration values.
 * <pre>
 * Color component selection flags.
 * </pre>
 */
static const lept_enum tbl_select_color[] = {
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
 * \brief Check for a color component selection.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_color(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Return a string for the color component selection enumeration value.
 * \param color selected color enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_color(l_int32 color)
{
    return ll_string_tbl(color, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Table of select min/max names and enumeration values.
 * <pre>
 * Color component selection flags. Only "min" and "max" subset.
 * </pre>
 */
static const lept_enum tbl_select_minmax[] = {
    TBL_ENTRY("min",                 L_SELECT_MIN),
    TBL_ENTRY("max",                 L_SELECT_MAX)
};

/**
 * \brief Check for a select min or max name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_min_max(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/**
 * \brief Return a string for the select min or max enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_min_max(l_int32 which)
{
    return ll_string_tbl(which, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/**
 * \brief Table of structuring element type names and enumeration values.
 * <pre>
 * Structuring element types.
 * </pre>
 */
static const lept_enum tbl_sel[] = {
    TBL_ENTRY("dont-care",          SEL_DONT_CARE),
    TBL_ENTRY("hit",                SEL_HIT),
    TBL_ENTRY("h",                  SEL_HIT),
    TBL_ENTRY("miss",               SEL_MISS),
    TBL_ENTRY("m",                  SEL_MISS),
    TBL_ENTRY("",                   SEL_DONT_CARE)
};

/**
 * \brief Check for a structuring element type name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sel(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sel, ARRAYSIZE(tbl_sel));
}

/**
 * \brief Return a string for the structuring element type enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_sel(l_int32 which)
{
    return ll_string_tbl(which, tbl_sel, ARRAYSIZE(tbl_sel));
}

/**
 * \brief Table of location filter names and enumeration values.
 * <pre>
 * Location filter flags.
 * </pre>
 */
static const lept_enum tbl_select_size[] = {
    TBL_ENTRY("width",              L_SELECT_WIDTH),
    TBL_ENTRY("w",                  L_SELECT_WIDTH),
    TBL_ENTRY("height",             L_SELECT_HEIGHT),
    TBL_ENTRY("h",                  L_SELECT_HEIGHT),
    TBL_ENTRY("xval",               L_SELECT_XVAL),
    TBL_ENTRY("x",                  L_SELECT_XVAL),
    TBL_ENTRY("yval",               L_SELECT_YVAL),
    TBL_ENTRY("y",                  L_SELECT_YVAL),
    TBL_ENTRY("if-either",          L_SELECT_IF_EITHER),
    TBL_ENTRY("either",             L_SELECT_IF_EITHER),
    TBL_ENTRY("e",                  L_SELECT_IF_EITHER),
    TBL_ENTRY("if-both",            L_SELECT_IF_BOTH),
    TBL_ENTRY("both",               L_SELECT_IF_BOTH),
    TBL_ENTRY("b",                  L_SELECT_IF_BOTH)
};

/**
 * \brief Check for a location filter name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_size(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_size, ARRAYSIZE(tbl_select_size));
}

/**
 * \brief Return a string for the location filter enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_size(l_int32 which)
{
    return ll_string_tbl(which, tbl_select_size, ARRAYSIZE(tbl_select_size));
}

/**
 * \brief Table of sort by names and enumeration values.
 * <pre>
 * Sort type flags.
 * </pre>
 */
static const lept_enum tbl_sort_by[] = {
    TBL_ENTRY("sort-by-x",              L_SORT_BY_X),
    TBL_ENTRY("x",                      L_SORT_BY_X),
    TBL_ENTRY("sort-by-y",              L_SORT_BY_Y),
    TBL_ENTRY("y",                      L_SORT_BY_Y),
    TBL_ENTRY("sort-by-right",          L_SORT_BY_RIGHT),
    TBL_ENTRY("right",                  L_SORT_BY_RIGHT),
    TBL_ENTRY("r",                      L_SORT_BY_RIGHT),
    TBL_ENTRY("sort-by-bot",            L_SORT_BY_BOT),
    TBL_ENTRY("bottom",                 L_SORT_BY_BOT),
    TBL_ENTRY("bot",                    L_SORT_BY_BOT),
    TBL_ENTRY("b",                      L_SORT_BY_BOT),
    TBL_ENTRY("sort-by-width",          L_SORT_BY_WIDTH),
    TBL_ENTRY("width",                  L_SORT_BY_WIDTH),
    TBL_ENTRY("w",                      L_SORT_BY_WIDTH),
    TBL_ENTRY("sort-by-height",         L_SORT_BY_HEIGHT),
    TBL_ENTRY("height",                 L_SORT_BY_HEIGHT),
    TBL_ENTRY("h",                      L_SORT_BY_HEIGHT),
    TBL_ENTRY("sort-by-min-dimension",  L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("min-dimension",          L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("min",                    L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("sort-by-max-dimension",  L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("max-dimension",          L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("max",                    L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("sort-by-perimeter",      L_SORT_BY_PERIMETER),
    TBL_ENTRY("perimeter",              L_SORT_BY_PERIMETER),
    TBL_ENTRY("perim",                  L_SORT_BY_PERIMETER),
    TBL_ENTRY("p",                      L_SORT_BY_PERIMETER),
    TBL_ENTRY("sort-by-area",           L_SORT_BY_AREA),
    TBL_ENTRY("area",                   L_SORT_BY_AREA),
    TBL_ENTRY("a",                      L_SORT_BY_AREA),
    TBL_ENTRY("sort-by-aspect-ratio",   L_SORT_BY_ASPECT_RATIO),
    TBL_ENTRY("aspect-ratio",           L_SORT_BY_ASPECT_RATIO),
    TBL_ENTRY("aspect",                 L_SORT_BY_ASPECT_RATIO),
};

/**
 * \brief Check for a sort type name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_by(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_by, ARRAYSIZE(tbl_sort_by));
}

/**
 * \brief Return a string for sort type enumeration value.
 * \param sort_by selected sort by enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_by(l_int32 sort_by)
{
    return ll_string_tbl(sort_by, tbl_sort_by, ARRAYSIZE(tbl_sort_by));
}

/**
 * \brief Table of set side names and enumeration values.
 * <pre>
 * Box size adjustment and location flags. Only the "set-" subset.
 * </pre>
 */
static const lept_enum tbl_set_side[] = {
    TBL_ENTRY("set-left",           L_SET_LEFT),
    TBL_ENTRY("left",               L_SET_LEFT),
    TBL_ENTRY("lft",                L_SET_LEFT),
    TBL_ENTRY("l",                  L_SET_LEFT),
    TBL_ENTRY("set-right",          L_SET_RIGHT),
    TBL_ENTRY("right",              L_SET_RIGHT),
    TBL_ENTRY("rgt",                L_SET_RIGHT),
    TBL_ENTRY("r",                  L_SET_RIGHT),
    TBL_ENTRY("set-top",            L_SET_TOP),
    TBL_ENTRY("top",                L_SET_TOP),
    TBL_ENTRY("t",                  L_SET_TOP),
    TBL_ENTRY("set-bot",            L_SET_BOT),
    TBL_ENTRY("bottom",             L_SET_BOT),
    TBL_ENTRY("bot",                L_SET_BOT),
    TBL_ENTRY("b",                  L_SET_BOT)
};

/**
 * \brief Check for a from side name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_set_side(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_set_side, ARRAYSIZE(tbl_set_side));
}

/**
 * \brief Return a string for the from side enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_set_side(l_int32 which)
{
    return ll_string_tbl(which, tbl_set_side, ARRAYSIZE(tbl_set_side));
}

/**
 * \brief Table of scan direction names and enumeration values.
 * <pre>
 * Scan direction flags. Only the "from-" subset.
 * </pre>
 */
static const lept_enum tbl_from_side[] = {
    TBL_ENTRY("from-left",      L_FROM_LEFT),
    TBL_ENTRY("left",           L_FROM_LEFT),
    TBL_ENTRY("lft",            L_FROM_LEFT),
    TBL_ENTRY("l",              L_FROM_LEFT),
    TBL_ENTRY("from-right",     L_FROM_RIGHT),
    TBL_ENTRY("right",          L_FROM_RIGHT),
    TBL_ENTRY("rgt",            L_FROM_RIGHT),
    TBL_ENTRY("r",              L_FROM_RIGHT),
    TBL_ENTRY("from-top",       L_FROM_TOP),
    TBL_ENTRY("top",            L_FROM_TOP),
    TBL_ENTRY("t",              L_FROM_TOP),
    TBL_ENTRY("from-bot",       L_FROM_BOT),
    TBL_ENTRY("bottom",         L_FROM_BOT),
    TBL_ENTRY("bot",            L_FROM_BOT),
    TBL_ENTRY("b",              L_FROM_BOT)
};

/**
 * \brief Check for a scan direction name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_from_side(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_from_side, ARRAYSIZE(tbl_from_side));
}

/**
 * \brief Return a string for the scan direction enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_from_side(l_int32 which)
{
    return ll_string_tbl(which, tbl_from_side, ARRAYSIZE(tbl_from_side));
}

/**
 * \brief Table of adjust side names and enumeration values.
 * <pre>
 * Box size adjustment and location flags.
 * </pre>
 */
static const lept_enum tbl_adjust_sides[] = {
    TBL_ENTRY("adjust-skip",            L_ADJUST_SKIP),
    TBL_ENTRY("adj-skip",               L_ADJUST_SKIP),
    TBL_ENTRY("skip",                   L_ADJUST_SKIP),
    TBL_ENTRY("adjust-left",            L_ADJUST_LEFT),
    TBL_ENTRY("adj-left",               L_ADJUST_LEFT),
    TBL_ENTRY("left",                   L_ADJUST_LEFT),
    TBL_ENTRY("lft",                    L_ADJUST_LEFT),
    TBL_ENTRY("l",                      L_ADJUST_LEFT),
    TBL_ENTRY("adjust-right",           L_ADJUST_RIGHT),
    TBL_ENTRY("adj-right",              L_ADJUST_RIGHT),
    TBL_ENTRY("right",                  L_ADJUST_RIGHT),
    TBL_ENTRY("rgt",                    L_ADJUST_RIGHT),
    TBL_ENTRY("r",                      L_ADJUST_RIGHT),
    TBL_ENTRY("adjust-left-and-right",  L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("adj-left-and-right",     L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("left-and-right",         L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("l-r",                    L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("adjust-top",             L_ADJUST_TOP),
    TBL_ENTRY("adj-top",                L_ADJUST_TOP),
    TBL_ENTRY("top",                    L_ADJUST_TOP),
    TBL_ENTRY("t",                      L_ADJUST_TOP),
    TBL_ENTRY("adjust-bot",             L_ADJUST_BOT),
    TBL_ENTRY("adj-bot",                L_ADJUST_BOT),
    TBL_ENTRY("adjust-bottom",          L_ADJUST_BOT),
    TBL_ENTRY("bottom",                 L_ADJUST_BOT),
    TBL_ENTRY("bot",                    L_ADJUST_BOT),
    TBL_ENTRY("b",                      L_ADJUST_BOT),
    TBL_ENTRY("adjust-top-and-bot",     L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adj-top-and-bot",        L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("top-and-bot",            L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adjust-top-and-bottom",  L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adj-top-and-bottom",     L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("top-and-bottom",         L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("t-b",                    L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adjust-choose-min",      L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("adj-choose-min",         L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("choose-min",             L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("adjust-choose-max",      L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("adj-choose-max",         L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("choose-max",             L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("set-left",               L_SET_LEFT),
    TBL_ENTRY("set-l",                  L_SET_LEFT),
    TBL_ENTRY("set-right",              L_SET_RIGHT),
    TBL_ENTRY("set-r",                  L_SET_RIGHT),
    TBL_ENTRY("set-top",                L_SET_TOP),
    TBL_ENTRY("set-t",                  L_SET_TOP),
    TBL_ENTRY("set-bot",                L_SET_BOT),
    TBL_ENTRY("set-b",                  L_SET_BOT),
    TBL_ENTRY("get-left",               L_GET_LEFT),
    TBL_ENTRY("get-l",                  L_GET_LEFT),
    TBL_ENTRY("get-right",              L_GET_RIGHT),
    TBL_ENTRY("get-r",                  L_GET_RIGHT),
    TBL_ENTRY("get-top",                L_GET_TOP),
    TBL_ENTRY("get-t",                  L_GET_TOP),
    TBL_ENTRY("get-bot",                L_GET_BOT),
    TBL_ENTRY("get-b",                  L_GET_BOT)
};

/**
 * \brief Check for a adjust side name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_adjust_sides(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_adjust_sides, ARRAYSIZE(tbl_adjust_sides));
}

/**
 * \brief Return a string for the from adjust side enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_adjust_sides(l_int32 which)
{
    return ll_string_tbl(which, tbl_adjust_sides, ARRAYSIZE(tbl_adjust_sides));
}

/**
 * \brief Table of sort mode by names and enumeration values.
 * <pre>
 * Sort mode flags.
 * </pre>
 */
static const lept_enum tbl_sort_mode[] = {
    TBL_ENTRY("shell-sort",             L_SHELL_SORT),
    TBL_ENTRY("shell",                  L_SHELL_SORT),
    TBL_ENTRY("s",                      L_SHELL_SORT),
    TBL_ENTRY("bin-sort",               L_BIN_SORT),
    TBL_ENTRY("bin",                    L_BIN_SORT),
    TBL_ENTRY("b",                      L_BIN_SORT)
};

/**
 * \brief Check for a sort mode name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_mode(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_mode, ARRAYSIZE(tbl_sort_mode));
}

/**
 * \brief Return a string for the sort mode.
 * \param sort_mode transform sort_mode enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_mode(l_int32 sort_mode)
{
    return ll_string_tbl(sort_mode, tbl_sort_mode, ARRAYSIZE(tbl_sort_mode));
}

/**
 * \brief Table of sort order by names and enumeration values.
 * <pre>
 * Sort order flags.
 * </pre>
 */
static const lept_enum tbl_sort_order[] = {
    TBL_ENTRY("increasing",             L_SORT_INCREASING),
    TBL_ENTRY("inc",                    L_SORT_INCREASING),
    TBL_ENTRY("i",                      L_SORT_INCREASING),
    TBL_ENTRY("decreasing",             L_SORT_DECREASING),
    TBL_ENTRY("dec",                    L_SORT_DECREASING),
    TBL_ENTRY("d",                      L_SORT_DECREASING)
};

/**
 * \brief Check for a sort order name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_order(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_order, ARRAYSIZE(tbl_sort_order));
}

/**
 * \brief Return a string for the sort order.
 * \param order transform sort_order enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_order(l_int32 order)
{
    return ll_string_tbl(order, tbl_sort_order, ARRAYSIZE(tbl_sort_order));
}

/**
 * \brief Table of transform order by names and enumeration values.
 */
static const lept_enum tbl_trans_order[] = {
    TBL_ENTRY("translate,scale,rotate", L_TR_SC_RO),
    TBL_ENTRY("tr,sc,ro",               L_TR_SC_RO),
    TBL_ENTRY("scale,rotate,translate", L_SC_RO_TR),
    TBL_ENTRY("sc,ro,tr",               L_SC_RO_TR),
    TBL_ENTRY("rotate,translate,scale", L_RO_TR_SC),
    TBL_ENTRY("ro,tr,sc",               L_RO_TR_SC),
    TBL_ENTRY("translate,rotate,scale", L_TR_RO_SC),
    TBL_ENTRY("tr,ro,sc",               L_TR_RO_SC),
    TBL_ENTRY("rotate,scale,translate", L_RO_SC_TR),
    TBL_ENTRY("ro,sc,tr",               L_RO_SC_TR),
    TBL_ENTRY("scale,translate,rotate", L_SC_TR_RO),
    TBL_ENTRY("sc,tr,ro",               L_SC_TR_RO)
};

/**
 * \brief Check for a transfort order name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_trans_order(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_trans_order, ARRAYSIZE(tbl_trans_order));
}

/**
 * \brief Return a string for the transform order.
 * \param order transform order enumeration value
 * \return const string with the name
 */
const char*
ll_string_trans_order(l_int32 order)
{
    return ll_string_tbl(order, tbl_trans_order, ARRAYSIZE(tbl_trans_order));
}

/**
 * \brief Table of transform relation by names and enumeration values.
 * <pre>
 * Size filter flags.
 * </pre>
 */
static const lept_enum tbl_relation[] = {
    TBL_ENTRY("less-than",              L_SELECT_IF_LT),
    TBL_ENTRY("lt",                     L_SELECT_IF_LT),
    TBL_ENTRY("<",                      L_SELECT_IF_LT),
    TBL_ENTRY("greater-than",           L_SELECT_IF_GT),
    TBL_ENTRY("gt",                     L_SELECT_IF_GT),
    TBL_ENTRY(">",                      L_SELECT_IF_GT),
    TBL_ENTRY("less-than-or-equal",     L_SELECT_IF_LTE),
    TBL_ENTRY("lte",                    L_SELECT_IF_LTE),
    TBL_ENTRY("<=",                     L_SELECT_IF_LTE),
    TBL_ENTRY("greater-than-or-equal",  L_SELECT_IF_GTE),
    TBL_ENTRY("gte",                    L_SELECT_IF_GTE),
    TBL_ENTRY(">=",                     L_SELECT_IF_GTE)
};

/**
 * \brief Check for a transform relation name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_relation(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_relation, ARRAYSIZE(tbl_relation));
}

/**
 * \brief Return a string for the transform relation.
 * \param relation relation value
 * \return const string with the name
 */
const char*
ll_string_relation(l_int32 relation)
{
    return ll_string_tbl(relation, tbl_relation, ARRAYSIZE(tbl_relation));
}

/**
 * \brief Table of transform rotation by names and enumeration values.
 * <pre>
 * Translates degrees to clockwise count.
 * </pre>
 */
static const lept_enum tbl_rotation[] = {
    TBL_ENTRY("0",      0),
    TBL_ENTRY("90",     1),
    TBL_ENTRY("180",    2),
    TBL_ENTRY("270",    3),
    TBL_ENTRY("-90",    3),
    TBL_ENTRY("",       0)
};

/**
 * \brief Check for a rotation name (actually a number).
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rotation(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_rotation, ARRAYSIZE(tbl_rotation));
}

/**
 * \brief Return a string for the rotation.
 * \param rotation rotation value
 * \return const string with the name
 */
const char*
ll_string_rotation(l_int32 rotation)
{
    return ll_string_tbl(rotation, tbl_rotation, ARRAYSIZE(tbl_rotation));
}

/**
 * \brief Table of handle overlap by names and enumeration values.
 * <pre>
 * Handling overlapping bounding boxes in Boxa.
 * </pre>
 */
static const lept_enum tbl_overlap[] = {
    TBL_ENTRY("combine",        L_COMBINE),
    TBL_ENTRY("comb",           L_COMBINE),
    TBL_ENTRY("c",              L_COMBINE),
    TBL_ENTRY("remove-small",   L_REMOVE_SMALL),
    TBL_ENTRY("remove",         L_REMOVE_SMALL),
    TBL_ENTRY("r",              L_REMOVE_SMALL)
};

/**
 * \brief Check for a handle overlap name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_overlap(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_overlap, ARRAYSIZE(tbl_overlap));
}

/**
 * \brief Return a string for the transform overlap.
 * \param overlap overlap value
 * \return const string with the name
 */
const char*
ll_string_overlap(l_int32 overlap)
{
    return ll_string_tbl(overlap, tbl_overlap, ARRAYSIZE(tbl_overlap));
}

/**
 * \brief Table of handle subflag by names and enumeration values.
 * <pre>
 * Flags for modifying box boundaries using a second box.
 * </pre>
 */
static const lept_enum tbl_subflag[] = {
    TBL_ENTRY("use-minsize",        L_USE_MINSIZE),
    TBL_ENTRY("minsize",            L_USE_MINSIZE),
    TBL_ENTRY("min",                L_USE_MINSIZE),
    TBL_ENTRY("use-maxsize",        L_USE_MAXSIZE),
    TBL_ENTRY("maxsize",            L_USE_MAXSIZE),
    TBL_ENTRY("max",                L_USE_MAXSIZE),
    TBL_ENTRY("sub-on-loc-diff",    L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("loc-diff",           L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("ldiff",              L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("ld",                 L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("sub-on-size-diff",   L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("size-diff",          L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("sdiff",              L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("sd",                 L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("use-capped-min",     L_USE_CAPPED_MIN),
    TBL_ENTRY("capped-min",         L_USE_CAPPED_MIN),
    TBL_ENTRY("capmin",             L_USE_CAPPED_MIN),
    TBL_ENTRY("cmin",               L_USE_CAPPED_MIN),
    TBL_ENTRY("use-capped-max",     L_USE_CAPPED_MAX),
    TBL_ENTRY("capped-max",         L_USE_CAPPED_MAX),
    TBL_ENTRY("capmax",             L_USE_CAPPED_MAX),
    TBL_ENTRY("cmax",               L_USE_CAPPED_MAX)
};

/**
 * \brief Check for a subflag name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_subflag(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_subflag, ARRAYSIZE(tbl_subflag));
}

/**
 * \brief Return a string for the subflag.
 * \param subflag subflag value
 * \return const string with the name
 */
const char*
ll_string_subflag(l_int32 subflag)
{
    return ll_string_tbl(subflag, tbl_subflag, ARRAYSIZE(tbl_subflag));
}

/**
 * \brief Table of handle use flag by names and enumeration values.
 * <pre>
 * Flags for replacing invalid boxes.
 * </pre>
 */
static const lept_enum tbl_useflag[] = {
    TBL_ENTRY("use-all-boxes",          L_USE_ALL_BOXES),
    TBL_ENTRY("use-all",                L_USE_ALL_BOXES),
    TBL_ENTRY("all",                    L_USE_ALL_BOXES),
    TBL_ENTRY("a",                      L_USE_ALL_BOXES),
    TBL_ENTRY("use-same-parity-boxes",  L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("use-same-parity",        L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same-parity-boxes",      L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same-parity",            L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same",                   L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("s",                      L_USE_SAME_PARITY_BOXES)
};

/**
 * \brief Check for a use flag name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_useflag(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_useflag, ARRAYSIZE(tbl_useflag));
}

/**
 * \brief Return a string for the use flag.
 * \param useflag useflag value
 * \return const string with the name
 */
const char*
ll_string_useflag(l_int32 useflag)
{
    return ll_string_tbl(useflag, tbl_useflag, ARRAYSIZE(tbl_useflag));
}

/**
 * \brief Table of dist select by names and enumeration values.
 * <pre>
 * Handling negative values in conversion to unsigned int.
 * </pre>
 */
static const lept_enum tbl_negvals[] = {
    TBL_ENTRY("clip-to-zero",   L_CLIP_TO_ZERO),
    TBL_ENTRY("zero",           L_CLIP_TO_ZERO),
    TBL_ENTRY("z",              L_CLIP_TO_ZERO),
    TBL_ENTRY("take-absval",    L_TAKE_ABSVAL),
    TBL_ENTRY("absval",         L_TAKE_ABSVAL),
    TBL_ENTRY("abs",            L_TAKE_ABSVAL),
    TBL_ENTRY("a",              L_TAKE_ABSVAL)
};

/**
 * \brief Check for a negvals name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_negvals(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_negvals, ARRAYSIZE(tbl_negvals));
}

/**
 * \brief Return a string for the negvals enumeration value.
 * \param negvals enumeration value
 * \return const string with the name
 */
const char*
ll_string_negvals(l_int32 negvals)
{
    return ll_string_tbl(negvals, tbl_negvals, ARRAYSIZE(tbl_negvals));
}

/**
 * \brief Table of dist select by names and enumeration values.
 * <pre>
 * Value flags.
 * </pre>
 */
static const lept_enum tbl_value_flags[] = {
    TBL_ENTRY("negative",       L_NEGATIVE),
    TBL_ENTRY("neg",            L_NEGATIVE),
    TBL_ENTRY("n",              L_NEGATIVE),
    TBL_ENTRY("non-negative",   L_NON_NEGATIVE),
    TBL_ENTRY("non-neg",        L_NON_NEGATIVE),
    TBL_ENTRY("nn",             L_NON_NEGATIVE),
    TBL_ENTRY("positive",       L_POSITIVE),
    TBL_ENTRY("pos",            L_POSITIVE),
    TBL_ENTRY("p",              L_POSITIVE),
    TBL_ENTRY("non-positive",   L_NON_POSITIVE),
    TBL_ENTRY("non-pos",        L_NON_POSITIVE),
    TBL_ENTRY("np",             L_NON_POSITIVE),
    TBL_ENTRY("zero",           L_ZERO),
    TBL_ENTRY("z",              L_ZERO),
    TBL_ENTRY("all",            L_ALL),
    TBL_ENTRY("a",              L_ALL)
};

/**
 * \brief Check for a select min or max name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_value_flags(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_value_flags, ARRAYSIZE(tbl_value_flags));
}

/**
 * \brief Return a string for the transform value_flags.
 * \param value_flags value_flags value
 * \return const string with the name
 */
const char*
ll_string_value_flags(l_int32 value_flags)
{
    return ll_string_tbl(value_flags, tbl_value_flags, ARRAYSIZE(tbl_value_flags));
}

/**
 * \brief Table of paint type names and enumeration values.
 * <pre>
 * Paint flags.
 * </pre>
 */
static const lept_enum tbl_paint_flags[] = {
    TBL_ENTRY("paint-light",    L_PAINT_LIGHT),
    TBL_ENTRY("light",          L_PAINT_LIGHT),
    TBL_ENTRY("l",              L_PAINT_LIGHT),
    TBL_ENTRY("paint-dark",     L_PAINT_DARK),
    TBL_ENTRY("dark",           L_PAINT_DARK),
    TBL_ENTRY("d",              L_PAINT_DARK)
};

/**
 * \brief Check for a paint type name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_paint_flags(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_paint_flags, ARRAYSIZE(tbl_paint_flags));
}

/**
 * \brief Return a string for paint type enumeration value.
 * \param paint_type paint_type value
 * \return const string with the name
 */
const char*
ll_string_paint_flags(l_int32 paint_type)
{
    return ll_string_tbl(paint_type, tbl_paint_flags, ARRAYSIZE(tbl_paint_flags));
}

static const lept_enum tbl_color_name[] = {
    TBL_ENTRY("Alice Blue",             0xF0F8FF),
    TBL_ENTRY("Antique White",          0xFAEBD7),
    TBL_ENTRY("Aqua",                   0x00FFFF),
    TBL_ENTRY("Aquamarine",             0x7FFFD4),
    TBL_ENTRY("Azure",                  0xF0FFFF),
    TBL_ENTRY("Beige",                  0xF5F5DC),
    TBL_ENTRY("Bisque",                 0xFFE4C4),
    TBL_ENTRY("Black",                  0x000000),
    TBL_ENTRY("Blanched Almond",        0xFFEBCD),
    TBL_ENTRY("Blue Violet",            0x8A2BE2),
    TBL_ENTRY("Blue",                   0x0000FF),
    TBL_ENTRY("Brown",                  0xA52A2A),
    TBL_ENTRY("Burly Wood",             0xDEB887),
    TBL_ENTRY("Cadet Blue",             0x5F9EA0),
    TBL_ENTRY("Chartreuse",             0x7FFF00),
    TBL_ENTRY("Chocolate",              0xD2691E),
    TBL_ENTRY("Coral",                  0xFF7F50),
    TBL_ENTRY("Cornflower Blue",        0x6495ED),
    TBL_ENTRY("Cornsilk",               0xFFF8DC),
    TBL_ENTRY("Crimson",                0xDC143C),
    TBL_ENTRY("Cyan",                   0x00FFFF),
    TBL_ENTRY("Dark Blue",              0x00008B),
    TBL_ENTRY("Dark Cyan",              0x008B8B),
    TBL_ENTRY("Dark Goldenrod",         0xB8860B),
    TBL_ENTRY("Dark Gray",              0xA9A9A9),
    TBL_ENTRY("Dark Green",             0x006400),
    TBL_ENTRY("Dark Khaki",             0xBDB76B),
    TBL_ENTRY("Dark Magenta",           0x8B008B),
    TBL_ENTRY("Dark Olive Green",       0x556B2F),
    TBL_ENTRY("Dark Orange",            0xFF8C00),
    TBL_ENTRY("Dark Orchid",            0x9932CC),
    TBL_ENTRY("Dark Red",               0x8B0000),
    TBL_ENTRY("Dark Salmon",            0xE9967A),
    TBL_ENTRY("Dark Sea Green",         0x8FBC8F),
    TBL_ENTRY("Dark Slate Blue",        0x483D8B),
    TBL_ENTRY("Dark Slate Gray",        0x2F4F4F),
    TBL_ENTRY("Dark Turquoise",         0x00CED1),
    TBL_ENTRY("Dark Violet",            0x9400D3),
    TBL_ENTRY("Deep Pink",              0xFF1493),
    TBL_ENTRY("Deep Sky Blue",          0x00BFFF),
    TBL_ENTRY("Dim Gray",               0x696969),
    TBL_ENTRY("Dodger Blue",            0x1E90FF),
    TBL_ENTRY("Fire Brick",             0xB22222),
    TBL_ENTRY("Floral White",           0xFFFAF0),
    TBL_ENTRY("Forest Green",           0x228B22),
    TBL_ENTRY("Fuchsia",                0xFF00FF),
    TBL_ENTRY("Gainsboro",              0xDCDCDC),
    TBL_ENTRY("Ghost White",            0xF8F8FF),
    TBL_ENTRY("Gold",                   0xFFD700),
    TBL_ENTRY("Goldenrod",              0xDAA520),
    TBL_ENTRY("Gray",                   0x808080),
    TBL_ENTRY("Green Yellow",           0xADFF2F),
    TBL_ENTRY("Green",                  0x008000),
    TBL_ENTRY("Honeydew",               0xF0FFF0),
    TBL_ENTRY("Hot Pink",               0xFF69B4),
    TBL_ENTRY("Indian Red",             0xCD5C5C),
    TBL_ENTRY("Indigo",                 0x4B0082),
    TBL_ENTRY("Ivory",                  0xFFFFF0),
    TBL_ENTRY("Khaki",                  0xF0E68C),
    TBL_ENTRY("Lavender Blush",         0xFFF0F5),
    TBL_ENTRY("Lavender",               0xE6E6FA),
    TBL_ENTRY("Lawn Green",             0x7CFC00),
    TBL_ENTRY("Lemon Chiffon",          0xFFFACD),
    TBL_ENTRY("Light Blue",             0xADD8E6),
    TBL_ENTRY("Light Coral",            0xF08080),
    TBL_ENTRY("Light Cyan",             0xE0FFFF),
    TBL_ENTRY("Light Goldenrod Yellow", 0xFAFAD2),
    TBL_ENTRY("Light Green",            0x90EE90),
    TBL_ENTRY("Light Grey",             0xD3D3D3),
    TBL_ENTRY("Light Pink",             0xFFB6C1),
    TBL_ENTRY("Light Salmon",           0xFFA07A),
    TBL_ENTRY("Light Sea Green",        0x20B2AA),
    TBL_ENTRY("Light Sky Blue",         0x87CEFA),
    TBL_ENTRY("Light Slate Gray",       0x778899),
    TBL_ENTRY("Light Steel Blue",       0xB0C4DE),
    TBL_ENTRY("Light Yellow",           0xFFFFE0),
    TBL_ENTRY("Lime Green",             0x32CD32),
    TBL_ENTRY("Lime",                   0x00FF00),
    TBL_ENTRY("Linen",                  0xFAF0E6),
    TBL_ENTRY("Magenta",                0xFF00FF),
    TBL_ENTRY("Maroon",                 0x800000),
    TBL_ENTRY("Medium Aquamarine",      0x66CDAA),
    TBL_ENTRY("Medium Blue",            0x0000CD),
    TBL_ENTRY("Medium Orchid",          0xBA55D3),
    TBL_ENTRY("Medium Purple",          0x9370DB),
    TBL_ENTRY("Medium Sea Green",       0x3CB371),
    TBL_ENTRY("Medium Slate Blue",      0x7B68EE),
    TBL_ENTRY("Medium Spring Green",    0x00FA9A),
    TBL_ENTRY("Medium Turquoise",       0x48D1CC),
    TBL_ENTRY("Medium Violet Red",      0xC71585),
    TBL_ENTRY("Midnight Blue",          0x191970),
    TBL_ENTRY("Mint Cream",             0xF5FFFA),
    TBL_ENTRY("Misty Rose",             0xFFE4E1),
    TBL_ENTRY("Moccasin",               0xFFE4B5),
    TBL_ENTRY("Navajo White",           0xFFDEAD),
    TBL_ENTRY("Navy",                   0x000080),
    TBL_ENTRY("Old Lace",               0xFDF5E6),
    TBL_ENTRY("Olive Drab",             0x6B8E23),
    TBL_ENTRY("Olive",                  0x808000),
    TBL_ENTRY("Orange Red",             0xFF4500),
    TBL_ENTRY("Orange",                 0xFFA500),
    TBL_ENTRY("Orchid",                 0xDA70D6),
    TBL_ENTRY("Pale Goldenrod",         0xEEE8AA),
    TBL_ENTRY("Pale Green",             0x98FB98),
    TBL_ENTRY("Pale Turquoise",         0xAFEEEE),
    TBL_ENTRY("Pale Violet Red",        0xDB7093),
    TBL_ENTRY("Papaya Whip",            0xFFEFD5),
    TBL_ENTRY("Peach Puff",             0xFFDAB9),
    TBL_ENTRY("Peru",                   0xCD853F),
    TBL_ENTRY("Pink",                   0xFFC0CB),
    TBL_ENTRY("Plum",                   0xDDA0DD),
    TBL_ENTRY("Powder Blue",            0xB0E0E6),
    TBL_ENTRY("Purple",                 0x800080),
    TBL_ENTRY("Red",                    0xFF0000),
    TBL_ENTRY("Rosy Brown",             0xBC8F8F),
    TBL_ENTRY("Royal Blue",             0x4169E1),
    TBL_ENTRY("Saddle Brown",           0x8B4513),
    TBL_ENTRY("Salmon",                 0xFA8072),
    TBL_ENTRY("Sandy Brown",            0xF4A460),
    TBL_ENTRY("Sea Green",              0x2E8B57),
    TBL_ENTRY("Seashell",               0xFFF5EE),
    TBL_ENTRY("Sienna",                 0xA0522D),
    TBL_ENTRY("Silver",                 0xC0C0C0),
    TBL_ENTRY("Silver",                 0xC0C0C0),
    TBL_ENTRY("Sky Blue",               0x87CEEB),
    TBL_ENTRY("Slate Blue",             0x6A5ACD),
    TBL_ENTRY("Slate Gray",             0x708090),
    TBL_ENTRY("Snow",                   0xFFFAFA),
    TBL_ENTRY("Spring Green",           0x00FF7F),
    TBL_ENTRY("Steel Blue",             0x4682B4),
    TBL_ENTRY("Tan",                    0xD2B48C),
    TBL_ENTRY("Teal",                   0x008080),
    TBL_ENTRY("Thistle",                0xD8BFD8),
    TBL_ENTRY("Tomato",                 0xFF6347),
    TBL_ENTRY("Turquoise",              0x40E0D0),
    TBL_ENTRY("Violet",                 0xEE82EE),
    TBL_ENTRY("Wheat",                  0xF5DEB3),
    TBL_ENTRY("White Smoke",            0xF5F5F5),
    TBL_ENTRY("White",                  0xFFFFFF),
    TBL_ENTRY("Yellow Green",           0x9ACD32),
    TBL_ENTRY("Yellow",                 0xFFFF00),
    TBL_ENTRY("aliceblue",              0xF0F8FF),
    TBL_ENTRY("antiquewhite",           0xFAEBD7),
    TBL_ENTRY("aqua",                   0x00FFFF),
    TBL_ENTRY("aquamarine",             0x7FFFD4),
    TBL_ENTRY("azure",                  0xF0FFFF),
    TBL_ENTRY("beige",                  0xF5F5DC),
    TBL_ENTRY("bisque",                 0xFFE4C4),
    TBL_ENTRY("black",                  0x000000),
    TBL_ENTRY("blanchedalmond",         0xFFEBCD),
    TBL_ENTRY("blue",                   0x0000FF),
    TBL_ENTRY("blueviolet",             0x8A2BE2),
    TBL_ENTRY("brown",                  0xA52A2A),
    TBL_ENTRY("burlywood",              0xDEB887),
    TBL_ENTRY("cadetblue",              0x5F9EA0),
    TBL_ENTRY("chartreuse",             0x7FFF00),
    TBL_ENTRY("chocolate",              0xD2691E),
    TBL_ENTRY("coral",                  0xFF7F50),
    TBL_ENTRY("cornflowerblue",         0x6495ED),
    TBL_ENTRY("cornsilk",               0xFFF8DC),
    TBL_ENTRY("crimson",                0xDC143C),
    TBL_ENTRY("cyan",                   0x00FFFF),
    TBL_ENTRY("darkblue",               0x00008B),
    TBL_ENTRY("darkcyan",               0x008B8B),
    TBL_ENTRY("darkgoldenrod",          0xB8860B),
    TBL_ENTRY("darkgray",               0xA9A9A9),
    TBL_ENTRY("darkgreen",              0x006400),
    TBL_ENTRY("darkkhaki",              0xBDB76B),
    TBL_ENTRY("darkmagenta",            0x8B008B),
    TBL_ENTRY("darkolivegreen",         0x556B2F),
    TBL_ENTRY("darkorange",             0xFF8C00),
    TBL_ENTRY("darkorchid",             0x9932CC),
    TBL_ENTRY("darkred",                0x8B0000),
    TBL_ENTRY("darksalmon",             0xE9967A),
    TBL_ENTRY("darkseagreen",           0x8FBC8F),
    TBL_ENTRY("darkslateblue",          0x483D8B),
    TBL_ENTRY("darkslategray",          0x2F4F4F),
    TBL_ENTRY("darkturquoise",          0x00CED1),
    TBL_ENTRY("darkviolet",             0x9400D3),
    TBL_ENTRY("deeppink",               0xFF1493),
    TBL_ENTRY("deepskyblue",            0x00BFFF),
    TBL_ENTRY("dimgray",                0x696969),
    TBL_ENTRY("dodgerblue",             0x1E90FF),
    TBL_ENTRY("firebrick",              0xB22222),
    TBL_ENTRY("floralwhite",            0xFFFAF0),
    TBL_ENTRY("forestgreen",            0x228B22),
    TBL_ENTRY("fuchsia",                0xFF00FF),
    TBL_ENTRY("gainsboro",              0xDCDCDC),
    TBL_ENTRY("ghostwhite",             0xF8F8FF),
    TBL_ENTRY("gold",                   0xFFD700),
    TBL_ENTRY("goldenrod",              0xDAA520),
    TBL_ENTRY("gray",                   0x808080),
    TBL_ENTRY("green",                  0x008000),
    TBL_ENTRY("greenyellow",            0xADFF2F),
    TBL_ENTRY("honeydew",               0xF0FFF0),
    TBL_ENTRY("hotpink",                0xFF69B4),
    TBL_ENTRY("indianred",              0xCD5C5C),
    TBL_ENTRY("indigo",                 0x4B0082),
    TBL_ENTRY("ivory",                  0xFFFFF0),
    TBL_ENTRY("khaki",                  0xF0E68C),
    TBL_ENTRY("lavender",               0xE6E6FA),
    TBL_ENTRY("lavenderblush",          0xFFF0F5),
    TBL_ENTRY("lawngreen",              0x7CFC00),
    TBL_ENTRY("lemonchiffon",           0xFFFACD),
    TBL_ENTRY("lightblue",              0xADD8E6),
    TBL_ENTRY("lightcoral",             0xF08080),
    TBL_ENTRY("lightcyan",              0xE0FFFF),
    TBL_ENTRY("lightgoldenrodyellow",   0xFAFAD2),
    TBL_ENTRY("lightgreen",             0x90EE90),
    TBL_ENTRY("lightgrey",              0xD3D3D3),
    TBL_ENTRY("lightpink",              0xFFB6C1),
    TBL_ENTRY("lightsalmon",            0xFFA07A),
    TBL_ENTRY("lightseagreen",          0x20B2AA),
    TBL_ENTRY("lightskyblue",           0x87CEFA),
    TBL_ENTRY("lightslategray",         0x778899),
    TBL_ENTRY("lightsteelblue",         0xB0C4DE),
    TBL_ENTRY("lightyellow",            0xFFFFE0),
    TBL_ENTRY("lime",                   0x00FF00),
    TBL_ENTRY("limegreen",              0x32CD32),
    TBL_ENTRY("linen",                  0xFAF0E6),
    TBL_ENTRY("magenta",                0xFF00FF),
    TBL_ENTRY("maroon",                 0x800000),
    TBL_ENTRY("mediumaquamarine",       0x66CDAA),
    TBL_ENTRY("mediumblue",             0x0000CD),
    TBL_ENTRY("mediumorchid",           0xBA55D3),
    TBL_ENTRY("mediumpurple",           0x9370DB),
    TBL_ENTRY("mediumseagreen",         0x3CB371),
    TBL_ENTRY("mediumslateblue",        0x7B68EE),
    TBL_ENTRY("mediumspringgreen",      0x00FA9A),
    TBL_ENTRY("mediumturquoise",        0x48D1CC),
    TBL_ENTRY("mediumvioletred",        0xC71585),
    TBL_ENTRY("midnightblue",           0x191970),
    TBL_ENTRY("mintcream",              0xF5FFFA),
    TBL_ENTRY("mistyrose",              0xFFE4E1),
    TBL_ENTRY("moccasin",               0xFFE4B5),
    TBL_ENTRY("navajowhite",            0xFFDEAD),
    TBL_ENTRY("navy",                   0x000080),
    TBL_ENTRY("oldlace",                0xFDF5E6),
    TBL_ENTRY("olive",                  0x808000),
    TBL_ENTRY("olivedrab",              0x6B8E23),
    TBL_ENTRY("orange",                 0xFFA500),
    TBL_ENTRY("orangered",              0xFF4500),
    TBL_ENTRY("orchid",                 0xDA70D6),
    TBL_ENTRY("palegoldenrod",          0xEEE8AA),
    TBL_ENTRY("palegreen",              0x98FB98),
    TBL_ENTRY("paleturquoise",          0xAFEEEE),
    TBL_ENTRY("palevioletred",          0xDB7093),
    TBL_ENTRY("papayawhip",             0xFFEFD5),
    TBL_ENTRY("peachpuff",              0xFFDAB9),
    TBL_ENTRY("peru",                   0xCD853F),
    TBL_ENTRY("pink",                   0xFFC0CB),
    TBL_ENTRY("plum",                   0xDDA0DD),
    TBL_ENTRY("powderblue",             0xB0E0E6),
    TBL_ENTRY("purple",                 0x800080),
    TBL_ENTRY("red",                    0xFF0000),
    TBL_ENTRY("rosybrown",              0xBC8F8F),
    TBL_ENTRY("royalblue",              0x4169E1),
    TBL_ENTRY("saddlebrown",            0x8B4513),
    TBL_ENTRY("salmon",                 0xFA8072),
    TBL_ENTRY("sandybrown",             0xF4A460),
    TBL_ENTRY("seagreen",               0x2E8B57),
    TBL_ENTRY("seashell",               0xFFF5EE),
    TBL_ENTRY("sienna",                 0xA0522D),
    TBL_ENTRY("skyblue",                0x87CEEB),
    TBL_ENTRY("slateblue",              0x6A5ACD),
    TBL_ENTRY("slategray",              0x708090),
    TBL_ENTRY("snow",                   0xFFFAFA),
    TBL_ENTRY("springgreen",            0x00FF7F),
    TBL_ENTRY("steelblue",              0x4682B4),
    TBL_ENTRY("tan",                    0xD2B48C),
    TBL_ENTRY("teal",                   0x008080),
    TBL_ENTRY("thistle",                0xD8BFD8),
    TBL_ENTRY("tomato",                 0xFF6347),
    TBL_ENTRY("turquoise",              0x40E0D0),
    TBL_ENTRY("violet",                 0xEE82EE),
    TBL_ENTRY("wheat",                  0xF5DEB3),
    TBL_ENTRY("white",                  0xFFFFFF),
    TBL_ENTRY("whitesmoke",             0xF5F5F5),
    TBL_ENTRY("yellow",                 0xFFFF00),
    TBL_ENTRY("yellowgreen",            0x9ACD32),
};

/**
 * \brief Check for a color name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_color_name(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_color_name, ARRAYSIZE(tbl_color_name));
}

/**
 * \brief Return a string for color RGB value.
 * \param color color value
 * \return const string with the name
 */
const char*
ll_string_color_name(l_uint32 color)
{
    static char buff[64];
    l_int32 r, g, b;
    extractRGBValues(color, &r, &g, &b);
    l_uint32 value = static_cast<l_uint32>((r << 16) | (g << 8) | (b << 0));
    const char* name = ll_string_tbl(static_cast<l_int32>(value), tbl_color_name, ARRAYSIZE(tbl_color_name));
    if (strcmp(name, "<undefined>"))
        return name;
    snprintf(buff, sizeof(buff), "#%02X%02x%02X", r, g, b);
    return buff;
}

/**
 * @brief Let's try our best to convert argument(s) to RGBA values
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index of the first parameter
 * @param pr [optional] pointer to red value to return
 * @param pg [optional] pointer to green value to return
 * @param pb [optional] pointer to blue value to return
 * @param pa [optional] pointer to alpah value to return
 * @return
 */
int
ll_check_color(const char *_fun, lua_State *L, int arg, l_int32 *pr, l_int32 *pg, l_int32 *pb, l_int32 *pa)
{
    l_int32 color = 1 << 24;
    l_int32 r = 0;
    l_int32 g = 0;
    l_int32 b = 0;
    l_int32 a = 0;

    if (pr)
        *pr = 0;
    if (pg)
        *pg = 0;
    if (pb)
        *pb = 0;
    if (pa)
        *pa = 0;

    if (ll_istable(_fun, L, arg)) {
           /* expect a table with up to 4 integer values */
           l_int32 len = 0;
           l_int32 *tbl = ll_unpack_Iarray(_fun, L, arg, &len);
           if (len > 0)
               r = tbl[0];
           if (len > 1)
               g = tbl[1];
           if (len > 2)
               b = tbl[2];
           if (len > 3)
               b = tbl[3];
           ll_free(tbl);
    } else if (ll_isinteger(_fun, L, arg)) {
           /*
            * Expect up to 4 integer values, where g defaults to r
            * and b defaults to g, so that color(60) => 60, 60, 60
            * Alpha defaults to 255.
            */
           r = ll_check_l_int32(_fun, L, arg);
           g = ll_opt_l_int32(_fun, L, arg + 1, r);
           b = ll_opt_l_int32(_fun, L, arg + 2, g);
           a = ll_opt_l_int32(_fun, L, arg + 3, 255);
    } else if (ll_isstring(_fun, L, arg)) {
        color = ll_check_color_name(_fun, L, arg);
        if (color >= 1 << 24) {
            /* not a color name */
            const char *str = ll_check_string(_fun, L, arg);
            if (*str == '#')
                str++;
            /* expect hexadecimal digits for 0xRRGGBB */
            color = static_cast<l_int32>(strtol(str, nullptr, 16));
        }
        if (color >= 1 << 24) {
            /* >= 24 bits: 0xRRGGBBAA */
            r = (color >> 24) & 0xff;
            g = (color >> 16) & 0xff;
            b = (color >>  8) & 0xff;
            a = (color >>  0) & 0xff;
        } else {
            /* < 24 bits: 0x00RRGGBB */
            r = (color >> 16) & 0xff;
            g = (color >>  8) & 0xff;
            b = (color >>  0) & 0xff;
            a = 0xff;
        }
    }

    if (pr)
        *pr = r;
    if (pg)
        *pg = g;
    if (pb)
        *pb = b;
    if (pa)
        *pa = a;

    return 0;
}

static global_var_t *global_vars = nullptr;

static int
ll_check_type(const char *type)
{
    static const char* types[] = {
        LL_BOOLEAN,
        LL_INT8,
        LL_UINT8,
        LL_INT16,
        LL_UINT16,
        LL_INT32,
        LL_UINT32,
        LL_INT64,
        LL_UINT64,
        LL_FLOAT32,
        LL_FLOAT64,
        LL_AMAP,
        LL_ASET,
        LL_BBUFFER,
        LL_BMF,
        LL_BOX,
        LL_BOXA,
        LL_BOXAA,
        LL_COMPDATA,
        LL_CCBORD,
        LL_CCBORDA,
        LL_DEWARP,
        LL_DEWARPA,
        LL_DLLIST,
        LL_DNA,
        LL_DNAA,
        LL_DNAHASH,
        LL_DPIX,
        LL_FPIX,
        LL_FPIXA,
        LL_KERNEL,
        LL_NUMA,
        LL_NUMAA,
        LL_PDFDATA,
        LL_PIX,
        LL_PIXA,
        LL_PIXAA,
        LL_PIXCMAP,
        LL_PIXTILING,
        LL_PIXCOMP,
        LL_PIXACOMP,
        LL_PTA,
        LL_PTAA,
        LL_RBTNODE,
        LL_SARRAY,
        LL_SEL,
        LL_SELA,
        LL_STACK,
        LL_WSHED
    };
    size_t i;

    for (i = 0; i < ARRAYSIZE(types); i++)
        if (!strcmp(type, types[i]))
            return 0;
    return 1;
}

/**
 * @brief Reset the linked list of global variables.
 *
 * Traverses the linked list global_vars and frees each entry.
 */
void
ll_res_globals(void)
{
    global_var_t *var = global_vars;
    global_var_t *next = nullptr;

    while (var) {
        next = var->next;
        ll_free(var);
        var = next;
    }

    global_vars = nullptr;
}

/**
 * @brief Add a pointer to a global variable to put into L when running the script.
 * @param type type of the variable (LL_...)
 * @param name name of the global variable.
 * @param in_ptr pointer to the contained type, e.g. Box*.
 * @return 0 on success, or 1 on error
 */
int
ll_set_global(const char *type, const char* name, void **in_ptr)
{
    FUNC("ll_set_global");
    char msg[256];
    global_var_t *var = nullptr;

    if (ll_check_type(type)) {
        snprintf(msg, sizeof(msg), "Parameter type='%s' is not known.\n", type);
        return ERROR_INT(msg, _fun, 1);
    }

    var = reinterpret_cast<global_var_t *>(LEPT_CALLOC(1, sizeof(*var)));
    if (!var) {
        fprintf(stderr, "%s: Could not allocate %s*.\n",
                _fun, "global_var_t");
    }
    var->type = type;
    var->name = name;
    var->i.pptr = in_ptr;
    var->next = global_vars;
    global_vars = var;
    return 0;
}

/**
 * @brief Add global variables to put into L when running the script.
 * The %vars array must be terminated with a {NULL, NULL, NULL} sentinel e.g. LL_SENTINEL.
 * @param vars pointer to an array of ll_global_var_t
 * @return 0 on success, or 1 on error
 */
int
ll_set_globals(const ll_global_var_t *vars)
{
    FUNC("ll_set_globals");
    const ll_global_var_t *var;
    for (var = vars; var->type; var++)
        ll_set_global(var->type, var->name, reinterpret_cast<void **>(var->ptr));
    return 0;
}

/**
 * @brief Add a pointer to a global variable to get from L after running the script.
 * @param type type of the variable (LL_...)
 * @param name name of the global variable.
 * @param out_ptr pointer to the result pointer to store.
 * @return 0 on success, or 1 on error
 */
int
ll_get_global(const char *type, const char *name, void **out_ptr)
{
    FUNC("ll_get_global");
    char msg[256];
    global_var_t *var = nullptr;

    if (ll_check_type(type)) {
        snprintf(msg, sizeof(msg), "Parameter type='%s' is not known.\n", type);
        return ERROR_INT(msg, _fun, 1);
    }

    var = reinterpret_cast<global_var_t *>(LEPT_CALLOC(1, sizeof(*var)));
    if (!var) {
        fprintf(stderr, "%s: Could not allocate %s*.\n",
                _fun, "global_var_t");
    }
    var->type = type;
    var->name = name;
    var->o.pptr = out_ptr;
    var->next = global_vars;
    global_vars = var;
    return 0;
}

/**
 * @brief Add global variables to get from L after running the script.
 * The %vars array must be terminated with a {NULL, NULL, NULL} sentinel e.g. LL_SENTINEL.
 * @param vars pointer to an array of ll_global_var_t
 * @return 0 on success, or 1 on error
 */
int
ll_get_globals(const ll_global_var_t *vars)
{
    FUNC("ll_get_globals");
    const ll_global_var_t *var;
    for (var = vars; var->type; var++)
        ll_get_global(var->type, var->name, reinterpret_cast<void **>(var->ptr));
    return 0;
}

/**
 * @brief Set all global variables defined in %vars.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * @param vars pointer to the root global_var_t
 * @return 0 on success, or die on error
 */
int
ll_set_all_globals(const char *_fun, lua_State *L, global_var_t *vars)
{
    global_var_t *var;

    for (var = vars; var; var = var->next) {
        if (nullptr == var->i.pptr)
            continue;

        if (!strcmp(LL_BOOLEAN, var->type)) {
            ll_push_boolean(_fun, L, *var->i.pb);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_INT8, var->type)) {
            ll_push_l_int8(_fun, L, *var->i.pi8);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_UINT8, var->type)) {
            ll_push_l_uint8(_fun, L, *var->i.pu8);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_INT16, var->type)) {
            ll_push_l_int16(_fun, L, *var->i.pi16);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_UINT16, var->type)) {
            ll_push_l_uint16(_fun, L, *var->i.pu16);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_INT32, var->type)) {
            ll_push_l_int32(_fun, L, *var->i.pi32);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_UINT32, var->type)) {
            ll_push_l_uint32(_fun, L, *var->i.pu32);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_INT64, var->type)) {
            ll_push_l_int64(_fun, L, *var->i.pi64);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_UINT64, var->type)) {
            ll_push_l_uint64(_fun, L, *var->i.pu64);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_FLOAT32, var->type)) {
            ll_push_l_float32(_fun, L, *var->i.pf32);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_FLOAT64, var->type)) {
            ll_push_l_float64(_fun, L, *var->i.pf64);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_AMAP, var->type)) {
            ll_push_Amap(_fun, L, *var->i.pamap);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_ASET, var->type)) {
            ll_push_Aset(_fun, L, *var->i.paset);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_BBUFFER, var->type)) {
            ll_push_ByteBuffer(_fun, L, *var->i.pbb);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_BMF, var->type)) {
            ll_push_Bmf(_fun, L, *var->i.pbmf);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_BOX, var->type)) {
            ll_push_Box(_fun, L, *var->i.pbox);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_BOXA, var->type)) {
            ll_push_Boxa(_fun, L, *var->i.pboxa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_BOXAA, var->type)) {
            ll_push_Boxaa(_fun, L, *var->i.pboxaa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_COMPDATA, var->type)) {
            ll_push_CompData(_fun, L, *var->i.pcid);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_CCBORD, var->type)) {
            ll_push_CCBord(_fun, L, *var->i.pccb);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_CCBORDA, var->type)) {
            ll_push_CCBorda(_fun, L, *var->i.pccba);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DEWARP, var->type)) {
            ll_push_Dewarp(_fun, L, *var->i.pdew);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DEWARPA, var->type)) {
            ll_push_Dewarpa(_fun, L, *var->i.pdewa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DLLIST, var->type)) {
            ll_push_DoubleLinkedList(_fun, L, *var->i.plist);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DNA, var->type)) {
            ll_push_Dna(_fun, L, *var->i.pda);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DNAA, var->type)) {
            ll_push_Dnaa(_fun, L, *var->i.pdaa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DNAHASH, var->type)) {
            ll_push_DnaHash(_fun, L, *var->i.pdah);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_DPIX, var->type)) {
            ll_push_DPix(_fun, L, *var->i.pdpix);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_FPIX, var->type)) {
            ll_push_FPix(_fun, L, *var->i.pfpix);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_FPIXA, var->type)) {
            ll_push_FPixa(_fun, L, *var->i.pfpixa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_KERNEL, var->type)) {
            ll_push_Kernel(_fun, L, *var->i.pkel);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_NUMA, var->type)) {
            ll_push_Numa(_fun, L, *var->i.pna);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_NUMAA, var->type)) {
            ll_push_Numaa(_fun, L, *var->i.pnaa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PDFDATA, var->type)) {
            ll_push_PdfData(_fun, L, *var->i.ppdd);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIX, var->type)) {
            ll_push_Pix(_fun, L, *var->i.ppix);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXA, var->type)) {
            ll_push_Pixa(_fun, L, *var->i.ppixa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXAA, var->type)) {
            ll_push_Pixaa(_fun, L, *var->i.ppixaa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXCMAP, var->type)) {
            ll_push_PixColormap(_fun, L, *var->i.pcmap);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXTILING, var->type)) {
            ll_push_PixTiling(_fun, L, *var->i.ppixt);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXCOMP, var->type)) {
            ll_push_PixComp(_fun, L, *var->i.ppixc);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXACOMP, var->type)) {
            ll_push_PixaComp(_fun, L, *var->i.ppixac);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PTA, var->type)) {
            ll_push_Pta(_fun, L, *var->i.ppta);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_PTAA, var->type)) {
            ll_push_Ptaa(_fun, L, *var->i.pptaa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_RBTNODE, var->type)) {
            lua_pushlightuserdata(L, *var->i.pnode);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_SARRAY, var->type)) {
            ll_push_Sarray(_fun, L, *var->i.psa);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_SEL, var->type)) {
            ll_push_Sel(_fun, L, *var->i.psel);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_SELA, var->type)) {
            ll_push_Sela(_fun, L, *var->i.psela);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_STACK, var->type)) {
            ll_push_Stack(_fun, L, *var->i.pstack);
            lua_setglobal(L, var->name);
            continue;
        }

        if (!strcmp(LL_WSHED, var->type)) {
            ll_push_WShed(_fun, L, *var->i.pwshed);
            lua_setglobal(L, var->name);
            continue;
        }

        die(_fun, L, "Unsupported type '%s' with name '%s'\n", var->type, var->name);
    }
    return 0;
}

/**
 * @brief Get all global variables defined in %vars.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * @param vars pointer to the root global_var_t
 * @return 0 on success, or die on error
 */
int
ll_get_all_globals(const char *_fun, lua_State *L, global_var_t *vars)
{
    global_var_t *var;

    for (var = vars; var; var = var->next) {
        if (nullptr == var->o.pptr)
            continue;

        if (!strcmp(LL_BOOLEAN, var->type)) {
            if (LUA_TBOOLEAN == lua_getglobal(L, var->name)) {
                *var->o.pb = static_cast<bool>(lua_toboolean(L, 1));
            } else {
                *var->o.pb = false;
            }
            continue;
        }

        if (!strcmp(LL_INT8, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pi8 = static_cast<l_int8>(lua_tointeger(L, 1));
            } else {
                *var->o.pi8 = 0;
            }
            continue;
        }

        if (!strcmp(LL_UINT8, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pu8 = static_cast<l_uint8>(lua_tointeger(L, 1));
            } else {
                *var->o.pu8 = 0;
            }
            continue;
        }

        if (!strcmp(LL_INT16, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pi16 = static_cast<l_int16>(lua_tointeger(L, 1));
            } else {
                *var->o.pi16 = 0;
            }
            continue;
        }

        if (!strcmp(LL_UINT16, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pu16 = static_cast<l_uint16>(lua_tointeger(L, 1));
            } else {
                *var->o.pu16 = 0;
            }
            continue;
        }

        if (!strcmp(LL_INT32, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pi32 = static_cast<l_int32>(lua_tointeger(L, 1));
            } else {
                *var->o.pi32 = 0;
            }
            continue;
        }

        if (!strcmp(LL_UINT32, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pu32 = static_cast<l_uint32>(lua_tointeger(L, 1));
            } else {
                *var->o.pu32 = 0;
            }
            continue;
        }

        if (!strcmp(LL_INT64, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pi64 = static_cast<l_int64>(lua_tointeger(L, 1));
            } else {
                *var->o.pi64 = 0;
            }
            continue;
        }

        if (!strcmp(LL_UINT64, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pu64 = static_cast<l_uint64>(lua_tointeger(L, 1));
            } else {
                *var->o.pu64 = 0;
            }
            continue;
        }

        if (!strcmp(LL_FLOAT32, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pf32 = static_cast<l_float32>(lua_tonumber(L, 1));
            } else {
                *var->o.pf32 = 0.0f;
            }
            continue;
        }

        if (!strcmp(LL_FLOAT64, var->type)) {
            if (LUA_TNUMBER == lua_getglobal(L, var->name)) {
                *var->o.pf64 = static_cast<l_float64>(lua_tonumber(L, 1));
            } else {
                *var->o.pf64 = 0.0;
            }
            continue;
        }

        if (!strcmp(LL_AMAP, var->type)) {
            *var->o.pamap = ll_get_global_Amap(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_ASET, var->type)) {
            *var->o.paset = ll_get_global_Aset(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_BBUFFER, var->type)) {
            *var->o.pbb = ll_get_global_ByteBuffer(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_BMF, var->type)) {
            *var->o.pbmf = ll_get_global_Bmf(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_BOX, var->type)) {
            *var->o.pbox = ll_get_global_Box(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_BOXA, var->type)) {
            *var->o.pboxa = ll_get_global_Boxa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_BOXAA, var->type)) {
            *var->o.pboxaa = ll_get_global_Boxaa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_COMPDATA, var->type)) {
            *var->o.pcid = ll_get_global_CompData(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_CCBORD, var->type)) {
            *var->o.pccb = ll_get_global_CCBord(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_CCBORDA, var->type)) {
            *var->o.pccba = ll_get_global_CCBorda(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DEWARP, var->type)) {
            *var->o.pdew = ll_get_global_Dewarp(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DEWARPA, var->type)) {
            *var->o.pdewa = ll_get_global_Dewarpa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DLLIST, var->type)) {
            *var->o.plist = ll_get_global_DoubleLinkedList(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DNA, var->type)) {
            *var->o.pda = ll_get_global_Dna(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DNAA, var->type)) {
            *var->o.pdaa = ll_get_global_Dnaa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DNAHASH, var->type)) {
            *var->o.pdah = ll_get_global_DnaHash(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_DPIX, var->type)) {
            *var->o.pdpix = ll_get_global_DPix(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_FPIX, var->type)) {
            *var->o.pfpix = ll_get_global_FPix(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_FPIXA, var->type)) {
            *var->o.pfpixa = ll_get_global_FPixa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_KERNEL, var->type)) {
            *var->o.pkel = ll_get_global_Kernel(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_NUMA, var->type)) {
            *var->o.pna = ll_get_global_Numa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_NUMAA, var->type)) {
            *var->o.pnaa = ll_get_global_Numaa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PDFDATA, var->type)) {
            *var->o.ppdd = ll_get_global_PdfData(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIX, var->type)) {
            *var->o.ppix = ll_get_global_Pix(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXA, var->type)) {
            *var->o.ppixa = ll_get_global_Pixa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXAA, var->type)) {
            *var->o.ppixaa = ll_get_global_Pixaa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXCMAP, var->type)) {
            *var->o.pcmap = ll_get_global_PixColormap(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXTILING, var->type)) {
            *var->o.ppixt = ll_get_global_PixTiling(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXCOMP, var->type)) {
            *var->o.ppixc = ll_get_global_PixComp(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PIXACOMP, var->type)) {
            *var->o.ppixac = ll_get_global_PixaComp(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PTA, var->type)) {
            *var->o.ppta = ll_get_global_Pta(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_PTAA, var->type)) {
            *var->o.pptaa = ll_get_global_Ptaa(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_RBTNODE, var->type)) {
            *var->o.pptr = nullptr;
            continue;
        }

        if (!strcmp(LL_SARRAY, var->type)) {
            *var->o.psa = ll_get_global_Sarray(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_SEL, var->type)) {
            *var->o.psel = ll_get_global_Sel(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_SELA, var->type)) {
            *var->o.psela = ll_get_global_Sela(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_STACK, var->type)) {
            *var->o.pstack = ll_get_global_Stack(_fun, L, var->name);
            continue;
        }

        if (!strcmp(LL_WSHED, var->type)) {
            *var->o.pwshed = ll_get_global_WShed(_fun, L, var->name);
            continue;
        }

        die(_fun, L, "Unsupported type '%s' with name '%s'\n", var->type, var->name);
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
 * \brief Enable a debug flag
 *
 * \param L pointer to the lua_State
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
#if defined(LLUA_DEBUG) && (LLUA_DEBUG > 0)
        dbg_enabled |= mask;
#endif
    }
    return 0;
}

/**
 * \brief Disable a debug flag
 *
 * \param L pointer to the lua_State
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
#if defined(LLUA_DEBUG) && (LLUA_DEBUG > 0)
        dbg_enabled &= ~mask;
#endif
    }
    return 0;
}

/**
 * \brief Disable a debug flag
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Debug(lua_State *L)
{
    LL_FUNC("Debug");
    luaL_Buffer B;

    luaL_buffinit(L, &B);
#if defined(LLUA_DEBUG) && (LLUA_DEBUG > 0)
    luaL_addstring(&B, ll_string_debug(dbg_enabled));
#endif
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a a LuaLept*.
 *
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    LuaLept **plept = ll_check_udata<LuaLept>(_fun, L, 1, TNAME);
    LuaLept *ll = *plept;
    DBG(LOG_DESTROY, "%s: '%s' plept=%p ll=%p\n",
        _fun, TNAME,
        reinterpret_cast<void *>(plept),
        reinterpret_cast<void *>(ll));
    ll_free(ll);
    *plept = nullptr;
    return 0;
}

/**
 * \brief Return the lualept version number.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
Version(lua_State *L)
{
    LL_FUNC("Version");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    lua_pushstring(L, ll->str_version);
    return 1;
}

/**
 * \brief Return the Lua version number.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
LuaVersion(lua_State *L)
{
    LL_FUNC("LuaVersion");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    lua_pushstring(L, ll->str_version_lua);
    return 1;
}

/**
 * \brief Return the Leptonica version number.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
LeptVersion(lua_State *L)
{
    LL_FUNC("LeptVersion");
    LuaLept *ll = ll_check_lualept(_fun, L, 1);
    lua_pushstring(L, ll->str_version_lept);
    return 1;
}

/**
 * \brief Compose a RGB pixel value.
 *
 * Arg #1 is expected to be a l_int32 (rval).
 * Arg #2 is expected to be a l_int32 (gval).
 * Arg #3 is expected to be a l_int32 (bval).
 *
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
 * \param L pointer to the lua_State
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
        {"LuaVersion",              LuaVersion},
        {"LeptVersion",             LeptVersion},
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
    ll_open_ByteBuffer(L);
    ll_open_Box(L);
    ll_open_Boxa(L);
    ll_open_Boxaa(L);
    ll_open_CCBord(L);
    ll_open_CCBorda(L);
    ll_open_CompData(L);
    ll_open_DPix(L);
    ll_open_Dewarp(L);
    ll_open_Dewarpa(L);
    ll_open_Dna(L);
    ll_open_Dnaa(L);
    ll_open_DnaHash(L);
    ll_open_DoubleLinkedList(L);
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

#define	DO_CHDIR	0

int
ll_RunScript(const char *script)
{
    FUNC("ll_RunScript");
#if DO_CHDIR
    char cwd[4096];
    char path[4096];
    char *slash;
#endif
    lua_State *L;
    global_var_t* var;
    int res;

#if DO_CHDIR
    /* Save current working directory */
    getcwd(cwd, sizeof(cwd));

    snprintf(path, sizeof(path), "%s", script);
    slash = strrchr(path, '/');
    if (!slash)
        slash = strrchr(path, '\\');
    if (slash) {
        *slash++ = '\0';
        script = slash;
    }
    if (chdir(path)) {
        DBG(1, "%s: chdir(\"%s\") failed (%s)\n", _fun,
            path, strerror(errno));
        return 1;
    }
#endif

    /* Disable Leptonica debugging (pixDisplay ...) */
    setLeptDebugOK(FALSE);

    /* Allocate a new Lua state */
    L = luaL_newstate();

    /* Open all Lua libraries */
    luaL_openlibs(L);

    /* Register our libraries */
    luaopen_lualept(L);

    /* Set any globals */
    ll_set_all_globals(_fun, L, global_vars);

    res = luaL_loadfile(L, script);
    if (LUA_OK != res) {
        const char* msg = lua_tostring(L, -1);
        lua_close(L);
#if DO_CHDIR
        chdir(cwd);
#endif
        return ERROR_INT(msg, _fun, 1);
    }

    /* Ask Lua to run our script */
    res = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (LUA_OK != res) {
        const char* msg = lua_tostring(L, -1);
        lua_close(L);
#if DO_CHDIR
        chdir(cwd);
#endif
        return ERROR_INT(msg, _fun, 1);
    }

    /* Get any globals */
    ll_get_all_globals(_fun, L, global_vars);

    lua_close(L);
#if DO_CHDIR
    chdir(cwd);
#endif
    return 0;
}
