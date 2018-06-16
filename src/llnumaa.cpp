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
 * \file llnumaa.cpp
 * \class Numaa
 *
 * An array of Numa.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_NUMAA

/** Define a function's name (_fun) with prefix Numaa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Numaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Numaa *naa = ll_take_udata<Numaa>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "naa", reinterpret_cast<void *>(naa),
        "count", numaaGetCount(naa));
    numaaDestroy(&naa);
    return 0;
}

/**
 * \brief Get the number of arrays stored in the Numaa* (%naa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, numaaGetCount(naa));
    return 1;
}

/**
 * \brief Printable string for a Numaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    luaL_Buffer B;
    l_float32 val;

    luaL_buffinit(L, &B);
    if (!naa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(naa));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        for (l_int32 i = 0; i < numaaGetCount(naa); i++) {
            Numa *na = numaaGetNuma(naa, i, L_CLONE);
            snprintf(str, LL_STRBUFF,
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (l_int32 j = 0; j < numaGetCount(na); j++) {
                numaGetFValue(na, j, &val);
                snprintf(str, LL_STRBUFF,
                         "\n        %d = %.8g",
                         j+1, static_cast<double>(val));
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            numaDestroy(&na);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add the Numa* (%na) to the Numaa* (%naa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * Arg #2 is expected to be a Numa* (na).
 * Arg #3 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddNuma(lua_State *L)
{
    LL_FUNC("AddNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    return ll_push_boolean(_fun, L, 0 == numaaAddNuma(naa, na, copyflag));
}

/**
 * \brief Create a new Numaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Numaa *naa = numaaCreate(n);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Create a full new Numaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (nptr).
 * Arg #1 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) This allocates numaa and fills the array with allocated numas.
 *          In use, after calling this function, use
 *              numaaAddNumber(naa, index, val);
 *          to add val to the index-th numa in naa.
 * </pre>
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
static int
CreateFull(lua_State *L)
{
    LL_FUNC("CreateFull");
    l_int32 nptr = ll_opt_l_int32(_fun, L, 1, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    Numaa *naa = numaaCreateFull(nptr, n);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Flatten the Numaa* (%naa) to a Numa* (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the Numaa to a Numa, by joining successively
 *          each Numa in the Numaa.
 *      (2) It doesn't make any assumptions about the location of the
 *          Numas in the Numaa array, unlike most Numaa functions.
 *      (3) It leaves the input Numaa unchanged.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
FlattenToNuma(lua_State *L)
{
    LL_FUNC("FlattenToNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    Numa *na = numaaFlattenToNuma(naa);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Get the Numa* (%na) from the Numaa* (%naa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (accessflag).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetNuma(lua_State *L)
{
    LL_FUNC("GetNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaaGetCount(naa));
    l_int32 accessflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Numa *na = numaaGetNuma(naa, idx, accessflag);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Get the number of numbers stored in the Numaa* (%naa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetNumaCount(lua_State *L)
{
    LL_FUNC("GetNumaCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaaGetCount(naa));
    ll_push_l_int32(_fun, L, numaaGetNumaCount(naa, idx));
    return 1;
}

/**
 * \brief Get the number of numbers stored in the entire Numaa* (%naa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetNumberCount(lua_State *L)
{
    LL_FUNC("GetNumberCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, numaaGetNumberCount(naa));
    return 1;
}

/**
 * \brief Read a Numaa* (%naa) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Numaa *naa = numaaRead(filename);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Read a Numaa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Numaa *naa = numaaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Read a Numaa* (%naa) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Numaa *naa = numaaReadStream(stream->f);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Replace a Numa* in the Numaa* at %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* (naa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Numa* (na).
 *
 * Leptonica's Notes:
 *      (1) Any existing numa is destroyed, and the input one
 *          is inserted in its place.
 *      (2) If the index is invalid, return 1 (error)
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReplaceNuma(lua_State *L)
{
    LL_FUNC("ReplaceNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaaGetCount(naa));
    Numa *na = ll_check_Numa(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaaReplaceNuma(naa, idx, na));
}

/**
 * \brief Truncate the arrays stored in the Numaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 *
 * Leptonica's Notes:
 *      (1) This identifies the largest index containing a numa that
 *          has any numbers within it, destroys all numa beyond that
 *          index, and resets the count.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Truncate(lua_State *L)
{
    LL_FUNC("Truncate");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == numaaTruncate(naa));
}

/**
 * \brief Write the Numaa* to an external file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaaWrite(filename, naa));
}

/**
 * \brief Write the Numaa* (%naa) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa* user data.
 *
 * Leptonica's Notes:
 *      (1) Serializes a numaa in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (numaaWriteMem(&data, &size, naa))
        return ll_push_nil(_fun, L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Numaa* (%naa) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numaa*.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaaWriteStream(stream->f, naa));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Numaa*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numaa* contained in the user data.
 */
Numaa *
ll_check_Numaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Numaa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Numaa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numaa* contained in the user data.
 */
Numaa *
ll_opt_Numaa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Numaa(_fun, L, arg);
}

/**
 * \brief Push Numaa* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param naa pointer to the NUMAA
 * \return 1 Numaa* on the Lua stack.
 */
int
ll_push_Numaa(const char *_fun, lua_State *L, Numaa *naa)
{
    if (!naa)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, naa);
}
/**
 * \brief Create and push a new Numaa*.
 * \param L Lua state.
 * \return 1 Numaa* on the Lua stack.
 */
int
ll_new_Numaa(lua_State *L)
{
    FUNC("ll_new_Numaa");
    Numaa *naa = nullptr;
    luaL_Stream* stream = nullptr;
    const char *filename = nullptr;
    const l_uint8 *data = nullptr;
    size_t size = 0;
    l_int32 nptr = 1;
    l_int32 n = 1;

    if (ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        naa = numaaReadStream(stream->f);
    }

    if (!naa && ll_isinteger(_fun, L, 1)) {
        if (ll_isinteger(_fun, L, 2)) {
            nptr = ll_opt_l_int32(_fun, L, 1, nptr);
            n = ll_opt_l_int32(_fun, L, 2, n);
            DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
                "nptr", nptr, "n", n);
            naa = numaaCreateFull(nptr, n);
        } else {
            n = ll_opt_l_int32(_fun, L, 1, n);
            DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
                "n", n);
            naa = numaaCreate(n);
        }
    }

    if (!naa && ll_isstring(_fun, L, 1)) {
        filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        naa = numaaRead(filename);
    }

    if (!naa && ll_isstring(_fun, L, 1)) {
        data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        naa = numaaReadMem(data, size);
    }

    if (!naa) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        naa = numaaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(naa));
    return ll_push_Numaa(_fun, L, naa);
}
/**
 * \brief Register the Numaa methods and functions in the Numaa meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Numaa(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",            Destroy},
        {"__new",           ll_new_Numaa},
        {"__len",           GetCount},
        {"__tostring",      toString},
        {"AddNuma",         AddNuma},
        {"Create",          Create},
        {"CreateFull",      CreateFull},
        {"FlattenToNuma",   FlattenToNuma},
        {"GetCount",        GetCount},
        {"GetNuma",         GetNuma},
        {"GetNumaCount",    GetNumaCount},
        {"GetNumberCount",  GetNumberCount},
        {"Read",            Read},
        {"ReadMem",         ReadMem},
        {"ReadStream",      ReadStream},
        {"ReplaceNuma",     ReplaceNuma},
        {"Truncate",        Truncate},
        {"Write",           Write},
        {"WriteMem",        WriteMem},
        {"WriteStream",     WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Numaa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
