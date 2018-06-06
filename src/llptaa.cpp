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
 * \file llptaa.cpp
 * \class Ptaa
 *
 * An array of Pta.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PTAA

/** Define a function's name (_fun) with prefix Ptaa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Ptaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Ptaa **pptaa = ll_check_udata<Ptaa>(_fun, L, 1, TNAME);
    Ptaa *ptaa = *pptaa;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "pptaa", reinterpret_cast<void *>(pptaa),
        "ptaa", reinterpret_cast<void *>(ptaa),
        "count", ptaaGetCount(ptaa));
    ptaaDestroy(&ptaa);
    *pptaa = nullptr;
    return 0;
}

/**
 * \brief Create a new Ptaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Ptaa *ptaa = ptaaCreate(n);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Get the number of arrays stored in the Ptaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, ptaaGetCount(ptaa));
    return 1;
}

/**
 * \brief Get the Pta* in the Ptaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a Pta* user data.
 * Arg #3 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPta(lua_State *L)
{
    LL_FUNC("AddPta");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == ptaaAddPta(ptaa, pta, copyflag));
}

/**
 * \brief Get the Point stored in the Ptaa* at Pta* index %ipta and Point index %jpt.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a l_int32 (ipta).
 * Arg #3 is expected to be a l_int32 (jpt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for two lua_Numbers on the Lua stack
 */
static int
GetPt(lua_State *L)
{
    LL_FUNC("GetPt");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    l_int32 ipta = ll_check_index(_fun, L, 2, ptaaGetCount(ptaa));
    l_int32 ipt = ll_check_index(_fun, L, 3, INT32_MAX);
    l_float32 x;
    l_float32 y;
    if (ptaaGetPt(ptaa, ipta, ipt, &x, &y))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, x);
    ll_push_l_float32(_fun, L, y);
    return 2;
}

/**
 * \brief Get the Pta* in the Ptaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (accessflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta* on the Lua stack
 */
static int
GetPta(lua_State *L)
{
    LL_FUNC("GetPta");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaaGetCount(ptaa));
    l_int32 accessflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Pta *pta = ptaaGetPta(ptaa, idx, accessflag);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Read a Ptaa* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Ptaa *ptaa = ptaaRead(filename);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Read a Ptaa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Ptaa *ptaa = ptaaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Read a Ptaa* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Ptaa *ptaa = ptaaReadStream(stream->f);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Replace a Pta* in the Ptaa* at %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pta* user data.
 *
 * Leptonica's Notes:
 *      (1) Any existing pta is destroyed, and the input one
 *          is inserted in its place.
 *      (2) If the index is invalid, return 1 (error)
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePta(lua_State *L)
{
    LL_FUNC("ReplacePta");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaaGetCount(ptaa));
    Pta *pta = ll_check_Pta(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == ptaaReplacePta(ptaa, idx, pta));
}

/**
 * \brief Truncate the arrays stored in the Ptaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 *
 * Leptonica's Notes:
 *      (1) This identifies the largest index containing a pta that
 *          has any points within it, destroys all pta above that index,
 *          and resets the count.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    LL_FUNC("Truncate");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ptaaTruncate(ptaa));
}

/**
 * \brief Write the Ptaa* (%ptaa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be string containing the filename.
 * Arg #3 is an optional boolean (type)
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 type = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == ptaaWrite(filename, ptaa, type));
}

/**
 * \brief Write the Ptaa* (%ptaa) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is an optional boolean (type)
 *
 * Leptonica's Notes:
 *      (1) Serializes a ptaa in memory and puts the result in a buffer.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    l_int32 type = ll_opt_boolean(_fun, L, 2, FALSE);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (ptaaWriteMem(&data, &size, ptaa, type))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Ptaa* to an external file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is an optional boolean (type)
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 type = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == ptaaWriteStream(stream->f, ptaa, type));
}

/**
 * \brief Check Lua stack at index %arg for udata of class Ptaa.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Ptaa* contained in the user data
 */
Ptaa *
ll_check_Ptaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Ptaa>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Ptaa* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Ptaa* contained in the user data
 */
Ptaa *
ll_take_Ptaa(const char *_fun, lua_State *L, int arg)
{
    Ptaa **pptaa = ll_check_udata<Ptaa>(_fun, L, arg, TNAME);
    Ptaa *ptaa = *pptaa;
    DBG(LOG_TAKE, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "pptaa", reinterpret_cast<void *>(pptaa),
        "ptaa", reinterpret_cast<void *>(ptaa));
    *pptaa = nullptr;
    return ptaa;
}

/**
 * \brief Take a Ptaa* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
Ptaa *
ll_get_global_Ptaa(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_Ptaa(_fun, L, -1);
}

/**
 * \brief Optionally expect a Ptaa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Ptaa* contained in the user data
 */
Ptaa *
ll_opt_Ptaa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Ptaa(_fun, L, arg);
}

/**
 * \brief Push PTAA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param ptaa pointer to the PTAA
 * \return 1 Ptaa* on the Lua stack
 */
int
ll_push_Ptaa(const char *_fun, lua_State *L, Ptaa *ptaa)
{
    if (!ptaa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, ptaa);
}

/**
 * \brief Create and push a new Ptaa*.
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
int
ll_new_Ptaa(lua_State *L)
{
    FUNC("ll_new_Ptaa");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Ptaa *ptaa = ptaaCreate(n);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Register the Ptaa methods and functions in the Ptaa meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_Ptaa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",        Destroy},
        {"__new",       ll_new_Ptaa},
        {"__len",       GetCount},
        {"AddPta",      AddPta},
        {"Create",      Create},
        {"GetPt",       GetPt},
        {"GetPta",      GetPta},
        {"Read",        Read},
        {"ReadMem",     ReadMem},
        {"ReadStream",  ReadStream},
        {"ReplacePta",  ReplacePta},
        {"Truncate",    Truncate},
        {"Write",       Write},
        {"WriteMem",    WriteMem},
        {"WriteStream", WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Ptaa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
