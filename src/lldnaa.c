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
#include <lauxlib.h>
#include <lualib.h>

/*====================================================================*
 *
 *  Lua class DNAA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_DNAA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_DNAA* contained in the user data
 */
L_DNAA *
ll_check_DNAA(lua_State *L, int arg)
{
    return *(L_DNAA **)ll_check_udata(L, arg, LL_DNAA);
}

/**
 * \brief Push L_DNAA user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param daa pointer to the L_DNAA
 * \return 1 DNAA* on the Lua stack
 */
int
ll_push_DNAA(lua_State *L, L_DNAA *daa)
{
    if (NULL == daa)
        return 0;
    return ll_push_udata(L, LL_DNAA, daa);
}

/**
 * \brief Create and push a new DNAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
int
ll_new_DNAA(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(__func__, L, 1, 1);
    L_DNAA *daa = l_dnaaCreate(n);
    return ll_push_DNAA(L, daa);
}

/**
 * \brief Create a new L_DNAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_DNAA(L);
}

/**
 * \brief Create a full new L_DNAA*
 *
 * Arg #1 is expected to be a l_int32 (nptr)
 * Arg #2 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 DNAA* on the Lua stack
 */
static int
CreateFull(lua_State *L)
{
    l_int32 nptr = ll_check_l_int32_default(__func__, L, 1, 1);
    l_int32 n = ll_check_l_int32_default(__func__, L, 2, 1);
    L_DNAA *daa = l_dnaaCreateFull(nptr, n);
    return ll_push_DNAA(L, daa);
}

/**
 * \brief Truncate the arrays stored in the L_DNAA*
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    lua_pushboolean(L, 0 == l_dnaaTruncate(daa));
    return 1;
}

/**
 * \brief Destroy a L_DNAA*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pdaa = ll_check_udata(L, 1, LL_DNAA);
    DBG(LOG_DESTROY, "%s: '%s' pdaa=%p daa=%p\n", __func__,
        LL_DNAA, (void *)pdaa, *pdaa);
    l_dnaaDestroy((L_DNAA **)pdaa);
    *pdaa = NULL;
    return 0;
}

/**
 * \brief Get the number of arrays stored in the L_DNAA*
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    l_int32 n = l_dnaaGetCount(daa);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the L_DNAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDnaCount(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, l_dnaaGetCount(daa));
    l_int32 n = l_dnaaGetDnaCount(daa, idx);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the entire L_DNAA*
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetNumberCount(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    lua_pushinteger(L, l_dnaaGetNumberCount(daa));
    return 1;
}

/**
 * \brief Add a L_DNA* to the L_DNAA*
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a L_DNA* user data
 * Arg #3 is an optional string defining the storage flags (copyflag)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddDna(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    L_DNA *da = ll_check_DNA(L, 2);
    l_int32 copyflag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == l_dnaaAddDna(daa, da, copyflag));
    return 1;
}

/**
 * \brief Get the L_DNA* in the L_DNAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (accessflag)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDna(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, l_dnaaGetCount(daa));
    l_int32 accessflag = ll_check_access_storage(L, 3, L_COPY);
    L_DNA *da = l_dnaaGetDna(daa, idx, accessflag);
    return ll_push_DNA(L, da);
}

/**
 * \brief Get the number in the L_DNAA* at index %i, %j
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a l_int32 (i)
 * Arg #3 is expected to be a l_int32 (j)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetValue(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    l_int32 i = ll_check_index(__func__, L, 2, l_dnaaGetCount(daa));
    l_int32 j = ll_check_index(__func__, L, 3, INT32_MAX);
    lua_Number val;
    if (l_dnaaGetValue(daa, i, j, &val))
        return 0;
    lua_pushnumber(L, val);
    return 1;
}

/**
 * \brief Add a number to the L_DNAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a lua_Number/l_float64 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddNumber(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    l_int32 idx = ll_check_l_int32(__func__, L, 2);
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaaAddNumber(daa, idx, val));
    return 1;
}

/**
 * \brief Read a L_DNAA from an external file
 *
 * Arg #1 is expected to be a string containing the filename
 *
 * \param L pointer to the lua_State
 * \return 1 DNAA on the Lua stack, or nil on error
 */
static int
Read(lua_State *L)
{
    const char *filename = lua_tostring(L, 1);
    L_DNAA *daa = l_dnaaRead(filename);
    return ll_push_DNAA(L, daa);
}

/**
 * \brief Write the L_DNAA to an external file
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 * Arg #2 is expected to be a string containing the filename
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    const char *filename = lua_tostring(L, 2);
    lua_pushboolean(L, 0 == l_dnaaWrite(filename, daa));
    return 1;
}

/**
 * \brief Flatten a L_DNAA* to a single L_DNA*
 *
 * Arg #1 (i.e. self) is expected to be a L_DNAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 DNA on the Lua stack, or nil on error
 */
static int
FlattenToDna(lua_State *L)
{
    L_DNAA *daa = ll_check_DNAA(L, 1);
    return ll_push_DNA(L, l_dnaaFlattenToDna(daa));
}

/**
 * \brief Register the L_DNAA methods and functions in the LL_DNAA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_DNAA(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",            Destroy},      /* garbage collector */
        {"__new",           Create},       /* new DNAA */
        {"__len",           GetCount},     /* #dnaa */
        {"Truncate",        Truncate},
        {"GetCount",        GetCount},
        {"GetDnaCount",     GetDnaCount},
        {"GetNumberCount",  GetNumberCount},
        {"AddDna",          AddDna},
        {"GetDna",          GetDna},
        {"GetValue",        GetValue},
        {"AddNumber",       AddNumber},
        {"Read",            Read},
        {"Write",           Write},
        {"FlattenToDna",    FlattenToDna},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",          Create},
        {"CreateFull",      CreateFull},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_DNAA, methods, functions);
    lua_setglobal(L, LL_DNAA);
    return res;
}
