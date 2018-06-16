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
 * \file lldnaa.cpp
 * \class Dnaa
 *
 * An array of Dna.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DNAA

/** Define a function's name (_fun) with prefix Dnaa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dnaa *daa = ll_take_udata<Dnaa>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "daa", reinterpret_cast<void *>(daa),
        "count", l_dnaaGetCount(daa));
    l_dnaaDestroy(&daa);
    return 0;
}

/**
 * \brief Get the number of arrays stored in the Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, l_dnaaGetCount(daa));
    return 1;
}

/**
 * \brief Replace a L_Dna* int the Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a L_Dna* user data.
 *
 * Leptonica's Notes:
 *      (1) Any existing l_dna is destroyed, and the input one
 *          is inserted in its place.
 *      (2) If the index is invalid, return 1 (error)
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReplaceDna(lua_State *L)
{
    LL_FUNC("ReplaceDna");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    L_Dna *da = ll_check_Dna(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == l_dnaaReplaceDna(daa, idx, da));
}

/**
 * \brief Printable string for a Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i, j;
    l_float64 val;

    luaL_buffinit(L, &B);
    if (!daa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(daa));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        for (i = 0; i < l_dnaaGetCount(daa); i++) {
            L_Dna *da = l_dnaaGetDna(daa, i, L_CLONE);
            snprintf(str, LL_STRBUFF,
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (j = 0; j < l_dnaGetCount(da); j++) {
                l_dnaGetDValue(da, j, &val);
                snprintf(str, LL_STRBUFF, "\n        %d = %.15g", j+1, val);
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            l_dnaDestroy(&da);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add a L_Dna* to the Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a L_Dna* user data.
 * Arg #3 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddDna(lua_State *L)
{
    LL_FUNC("AddDna");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    L_Dna *da = ll_check_Dna(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == l_dnaaAddDna(daa, da, copyflag));
}

/**
 * \brief Add a number to the Dnaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number/l_float64 (val).
 *
 * Leptonica's Notes:
 *      (1) Adds to an existing l_dna only.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddNumber(lua_State *L)
{
    LL_FUNC("AddNumber");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_l_int32(_fun, L, 2);
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaaAddNumber(daa, idx, val));
    return 1;
}

/**
 * \brief Create a new Dnaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 DNAA* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Dnaa *daa = l_dnaaCreate(n);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Create a full new Dnaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (nptr).
 * Arg #2 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) This allocates a dnaa and fills the array with allocated dnas.
 *          In use, after calling this function, use
 *              l_dnaaAddNumber(dnaa, index, val);
 *          to add val to the index-th dna in dnaa.
 * </pre>
 * \param L Lua state.
 * \return 1 DNAA* on the Lua stack.
 */
static int
CreateFull(lua_State *L)
{
    LL_FUNC("CreateFull");
    l_int32 nptr = ll_opt_l_int32(_fun, L, 1, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    Dnaa *daa = l_dnaaCreateFull(nptr, n);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Flatten a Dnaa* to a single L_Dna*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the dnaa to a dna, by joining successively
 *          each dna in the dnaa.
 *      (2) It leaves the input dnaa unchanged.
 * </pre>
 * \param L Lua state.
 * \return 1 DNA on the Lua stack, or nil on error.
 */
static int
FlattenToDna(lua_State *L)
{
    LL_FUNC("FlattenToDna");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    return ll_push_Dna(_fun, L, l_dnaaFlattenToDna(daa));
}

/**
 * \brief Get the L_Dna* in the Dnaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (accessflag).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetDna(lua_State *L)
{
    LL_FUNC("GetDna");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    l_int32 accessflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    L_Dna *da = l_dnaaGetDna(daa, idx, accessflag);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Get the number of numbers stored in the Dnaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetDnaCount(lua_State *L)
{
    LL_FUNC("GetDnaCount");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    ll_push_l_int32(_fun, L, l_dnaaGetDnaCount(daa, idx));
    return 1;
}

/**
 * \brief Get the number of numbers stored in the entire Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetNumberCount(lua_State *L)
{
    LL_FUNC("GetNumberCount");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, l_dnaaGetNumberCount(daa));
    return 1;
}

/**
 * \brief Get the number in the Dnaa* at index %i, %j.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetValue(lua_State *L)
{
    LL_FUNC("GetValue");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 i = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    l_int32 j = ll_check_index(_fun, L, 3, INT32_MAX);
    l_float64 val;
    if (l_dnaaGetValue(daa, i, j, &val))
        return ll_push_nil(_fun, L);
    ll_push_l_float64(_fun, L, val);
    return 1;
}

/**
 * \brief Read a L_DNAA from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L Lua state.
 * \return 1 DNAA on the Lua stack, or nil on error.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dnaa *daa = l_dnaaRead(filename);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Read a L_DNAA from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 DNAA on the Lua stack, or nil on error.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dnaa *daa = l_dnaaReadStream(stream->f);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Truncate the arrays stored in the Dnaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 *
 * Leptonica's Notes:
 *      (1) This identifies the largest index containing a dna that
 *          has any numbers within it, destroys all dna beyond that
 *          index, and resets the count.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Truncate(lua_State *L)
{
    LL_FUNC("Truncate");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == l_dnaaTruncate(daa));
}

/**
 * \brief Write the L_DNAA to an external file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a string containing the filename.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaaWrite(filename, daa));
}

/**
 * \brief Write the L_DNAA to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dnaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaaWriteStream(stream->f, daa));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Dnaa*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dnaa* contained in the user data.
 */
Dnaa *
ll_check_Dnaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dnaa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Dnaa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dnaa* contained in the user data.
 */
Dnaa *
ll_opt_Dnaa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Dnaa(_fun, L, arg);
}

/**
 * \brief Push Dnaa* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param daa pointer to the L_DNAA
 * \return 1 DNAA* on the Lua stack.
 */
int
ll_push_Dnaa(const char *_fun, lua_State *L, Dnaa *daa)
{
    if (!daa)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, daa);
}
/**
 * \brief Create and push a new DNAA*.
 * \param L Lua state.
 * \return 1 DNAA* on the Lua stack.
 */
int
ll_new_Dnaa(lua_State *L)
{
    FUNC("ll_new_Dnaa");
    Dnaa *daa = nullptr;
    l_int32 nptr = 1;
    l_int32 n = 1;

    if (ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream* stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        daa = l_dnaaReadStream(stream->f);
    }

    if (!daa && ll_isinteger(_fun, L, 1)) {
        if (ll_isinteger(_fun, L, 2)) {
            nptr = ll_opt_l_int32(_fun, L, 1, nptr);
            n = ll_opt_l_int32(_fun, L, 2, n);
            DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
                "nptr", nptr,
                "n", n);
            daa = l_dnaaCreateFull(nptr, n);
        } else {
            n = ll_opt_l_int32(_fun, L, 1, 1);
            DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
                "n", n);
            daa = l_dnaaCreate(n);
        }
    }

    if (!daa && ll_isstring(_fun, L, 1)) {
        const char *filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        daa = l_dnaaRead(filename);
    }

    if (!daa) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        daa = l_dnaaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(daa));
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Register the Dnaa methods and functions in the Dnaa meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Dnaa(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Dnaa},
        {"__len",               GetCount},
        {"__newindex",          ReplaceDna},
        {"__tostring",          toString},
        {"AddDna",              AddDna},
        {"AddNumber",           AddNumber},
        {"Create",              Create},
        {"CreateFull",          CreateFull},
        {"FlattenToDna",        FlattenToDna},
        {"GetCount",            GetCount},
        {"GetDna",              GetDna},
        {"GetDnaCount",         GetDnaCount},
        {"GetNumberCount",      GetNumberCount},
        {"GetValue",            GetValue},
        {"Read",                Read},
        {"ReadStream",          ReadStream},
        {"ReplaceDna",          ReplaceDna},
        {"Truncate",            Truncate},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Dnaa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
