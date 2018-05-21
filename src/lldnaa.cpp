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

/*====================================================================*
 *
 *  Lua class Dnaa
 *
 *====================================================================*/

/**
 * \brief Printable string for a L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_DNAA ".toString");
    static char str[256];
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i, j;
    l_float64 val;

    luaL_buffinit(L, &B);
    if (!daa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_DNAA ": %p",
                 reinterpret_cast<void *>(daa));
        luaL_addstring(&B, str);
        for (i = 0; i < l_dnaaGetCount(daa); i++) {
            L_Dna *da = l_dnaaGetDna(daa, i, L_CLONE);
            snprintf(str, sizeof(str),
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (j = 0; j < l_dnaGetCount(da); j++) {
                l_dnaGetDValue(da, j, &val);
                snprintf(str, sizeof(str), "\n        %d = %.15g", j+1, val);
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            l_dnaDestroy(&da);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new L_Dnaa*
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_DNAA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    L_Dnaa *daa = l_dnaaCreate(n);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Create a full new L_Dnaa*
 * <pre>
 * Arg #1 is expected to be a l_int32 (nptr).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
static int
CreateFull(lua_State *L)
{
    FUNC(LL_DNAA ".CreateFull");
    l_int32 nptr = ll_check_l_int32_default(_fun, L, 1, 1);
    l_int32 n = ll_check_l_int32_default(_fun, L, 2, 1);
    L_Dnaa *daa = l_dnaaCreateFull(nptr, n);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Truncate the arrays stored in the L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    FUNC(LL_DNAA ".Truncate");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    lua_pushboolean(L, 0 == l_dnaaTruncate(daa));
    return 1;
}

/**
 * \brief Destroy a L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_DNAA ".Destroy");
    L_Dnaa **pdaa = reinterpret_cast<L_Dnaa **>(ll_check_udata(_fun, L, 1, LL_DNAA));
    DBG(LOG_DESTROY, "%s: '%s' pdaa=%p daa=%p\n",
        _fun, LL_DNAA, pdaa, *pdaa);
    l_dnaaDestroy(pdaa);
    *pdaa = nullptr;
    return 0;
}

/**
 * \brief Get the number of arrays stored in the L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_DNAA ".GetCount");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 n = l_dnaaGetCount(daa);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the L_Dnaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDnaCount(lua_State *L)
{
    FUNC(LL_DNAA ".GetDnaCount");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    l_int32 n = l_dnaaGetDnaCount(daa, idx);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the entire L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetNumberCount(lua_State *L)
{
    FUNC(LL_DNAA ".GetNumberCount");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    lua_pushinteger(L, l_dnaaGetNumberCount(daa));
    return 1;
}

/**
 * \brief Add a L_Dna* to the L_Dnaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a L_Dna* user data.
 * Arg #3 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddDna(lua_State *L)
{
    FUNC(LL_DNAA ".AddDna");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    L_Dna *da = ll_check_Dna(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    lua_pushboolean(L, 0 == l_dnaaAddDna(daa, da, copyflag));
    return 1;
}

/**
 * \brief Get the L_Dna* in the L_Dnaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (accessflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDna(lua_State *L)
{
    FUNC(LL_DNAA ".GetDna");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    l_int32 accessflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    L_Dna *da = l_dnaaGetDna(daa, idx, accessflag);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Get the number in the L_Dnaa* at index %i, %j
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetValue(lua_State *L)
{
    FUNC(LL_DNAA ".GetValue");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 i = ll_check_index(_fun, L, 2, l_dnaaGetCount(daa));
    l_int32 j = ll_check_index(_fun, L, 3, INT32_MAX);
    lua_Number val;
    if (l_dnaaGetValue(daa, i, j, &val))
        return ll_push_nil(L);
    lua_pushnumber(L, val);
    return 1;
}

/**
 * \brief Add a number to the L_Dnaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number/l_float64 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddNumber(lua_State *L)
{
    FUNC(LL_DNAA ".AddNumber");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    l_int32 idx = ll_check_l_int32(_fun, L, 2);
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaaAddNumber(daa, idx, val));
    return 1;
}

/**
 * \brief Read a L_DNAA from an external file
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNAA on the Lua stack, or nil on error
 */
static int
Read(lua_State *L)
{
    FUNC(LL_DNAA ".Read");
    const char *filename = ll_check_string(_fun, L, 1);
    L_Dnaa *daa = l_dnaaRead(filename);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Read a L_DNAA from a Lua io stream (%stream)
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNAA on the Lua stack, or nil on error
 */
static int
ReadStream(lua_State *L)
{
    FUNC(LL_DNAA ".ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    L_Dnaa *daa = l_dnaaReadStream(stream->f);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Write the L_DNAA to an external file
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    FUNC(LL_DNAA ".Write");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, 0 == l_dnaaWrite(filename, daa));
    return 1;
}

/**
 * \brief Write the L_DNAA to a Lua io stream (%stream)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    FUNC(LL_DNAA ".WriteStream");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    lua_pushboolean(L, 0 == l_dnaaWriteStream(stream->f, daa));
    return 1;
}

/**
 * \brief Flatten a L_Dnaa* to a single L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dnaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA on the Lua stack, or nil on error
 */
static int
FlattenToDna(lua_State *L)
{
    FUNC(LL_DNAA ".FlattenToDna");
    L_Dnaa *daa = ll_check_Dnaa(_fun, L, 1);
    return ll_push_Dna(_fun, L, l_dnaaFlattenToDna(daa));
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_DNAA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_Dnaa* contained in the user data
 */
L_Dnaa *
ll_check_Dnaa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<L_Dnaa **>(ll_check_udata(_fun, L, arg, LL_DNAA)));
}

/**
 * \brief Optionally expect a LL_DNAA at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_Dnaa* contained in the user data
 */
L_Dnaa *
ll_check_Dnaa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Dnaa(_fun, L, arg);
}

/**
 * \brief Push L_Dnaa* user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param daa pointer to the L_DNAA
 * \return 1 DNAA* on the Lua stack
 */
int
ll_push_Dnaa(const char *_fun, lua_State *L, L_Dnaa *daa)
{
    if (!daa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DNAA, daa);
}

/**
 * \brief Create and push a new DNAA*
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
int
ll_new_Dnaa(lua_State *L)
{
    FUNC("ll_new_Dnaa");
    L_Dnaa *daa = l_dnaaCreate(1);
    return ll_push_Dnaa(_fun, L, daa);
}

/**
 * \brief Register the L_DNAA methods and functions in the LL_DNAA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Dnaa(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",            Destroy},      /* garbage collector */
        {"__new",           Create},       /* new DNAA */
        {"__len",           GetCount},     /* #dnaa */
        {"__tostring",      toString},
        {"Truncate",        Truncate},
        {"GetCount",        GetCount},
        {"GetDnaCount",     GetDnaCount},
        {"GetNumberCount",  GetNumberCount},
        {"AddDna",          AddDna},
        {"GetDna",          GetDna},
        {"GetValue",        GetValue},
        {"AddNumber",       AddNumber},
        {"Read",            Read},
        {"ReadStream",      ReadStream},
        {"Write",           Write},
        {"WriteStream",     WriteStream},
        {"FlattenToDna",    FlattenToDna},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",          Create},
        {"CreateFull",      CreateFull},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_DNAA, methods, functions);
}
