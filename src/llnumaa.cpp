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
 *  Lua class NUMAA
 *
 *====================================================================*/

/**
 * \brief Create a new Numaa*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_NUMAA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Numaa *naa = numaaCreate(n);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Create a full new Numaa*
 *
 * Arg #1 is expected to be a l_int32 (nptr)
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
static int
CreateFull(lua_State *L)
{
    FUNC(LL_NUMAA ".CreateFull");
    l_int32 nptr = ll_check_l_int32_default(_fun, L, 1, 1);
    l_int32 n = ll_check_l_int32_default(_fun, L, 2, 1);
    Numaa *naa = numaaCreateFull(nptr, n);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Truncate the arrays stored in the Numaa*
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    FUNC(LL_NUMAA ".Truncate");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    lua_pushboolean(L, 0 == numaaTruncate(naa));
    return 1;
}

/**
 * \brief Destroy a Numaa*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_NUMAA ".Destroy");
    Numaa **pnaa = reinterpret_cast<Numaa **>(ll_check_udata(_fun, L, 1, LL_NUMAA));
    DBG(LOG_DESTROY, "%s: '%s' pnaa=%p naa=%p\n",
         _fun, LL_NUMAA, pnaa, *pnaa);
    numaaDestroy(pnaa);
    *pnaa = nullptr;
    return 0;
}

/**
 * \brief Get the number of arrays stored in the Numaa*
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_NUMAA ".GetCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 n = numaaGetCount(naa);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the Numaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetNumaCount(lua_State *L)
{
    FUNC(LL_NUMAA ".GetNumaCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaaGetCount(naa));
    l_int32 n = numaaGetNumaCount(naa, idx);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the number of numbers stored in the entire Numaa*
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetNumberCount(lua_State *L)
{
    FUNC(LL_NUMAA ".GetNumberCount");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 n = numaaGetNumberCount(naa);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the Numa* in the Numaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetNuma(lua_State *L)
{
    FUNC(LL_NUMAA ".GetNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaaGetCount(naa));
    l_int32 accessflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Numa *na = numaaGetNuma(naa, idx, accessflag);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Add the Numa* to the Numaa*
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 * Arg #2 is expected to be a Numa*
 * Arg #3 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
AddNuma(lua_State *L)
{
    FUNC(LL_NUMAA ".AddNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    lua_pushboolean(L, 0 == numaaAddNuma(naa, na, copyflag));
    return 1;
}

/**
 * \brief Replace a Numa* in the Numaa* at %idx
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 * Arg #2 is expected to be a l_int32
 * Arg #3 is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceNuma(lua_State *L)
{
    FUNC(LL_NUMAA ".ReplaceNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    l_int32 idx = ll_check_l_int32(_fun, L, 2);
    Numa *na = ll_check_Numa(_fun, L, 3);
    lua_pushboolean(L, 0 == numaaReplaceNuma(naa, idx, na));
    return 1;
}

/**
 * \brief Write the Numaa* to and external file
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 * Arg #2 is expected to be string containing the filename
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    FUNC(LL_NUMAA ".Write");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, 0 == numaaWrite(filename, naa));
    return 1;
}

/**
 * \brief Read a Numaa* from an external file
 *
 * Arg #1 is expected to be a string containing the filename
 *
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    FUNC(LL_NUMAA ".Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Numaa *naa = numaaRead(filename);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Flatten the Numaa* to a Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numaa*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenToNuma(lua_State *L)
{
    FUNC(LL_NUMAA ".FlattenToNuma");
    Numaa *naa = ll_check_Numaa(_fun, L, 1);
    Numa *na = numaaFlattenToNuma(naa);
    return ll_push_Numa(_fun, L, na);
}

/**
 * @brief Check Lua stack at index %arg for udata of class LL_NUMAA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numaa* contained in the user data
 */
Numaa *
ll_check_Numaa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Numaa **>(ll_check_udata(_fun, L, arg, LL_NUMAA)));
}

/**
 * \brief Push Numaa* user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param naa pointer to the NUMAA
 * \return 1 Numaa* on the Lua stack
 */
int
ll_push_Numaa(const char *_fun, lua_State *L, Numaa *naa)
{
    if (!naa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_NUMAA, naa);
}

/**
 * \brief Create and push a new Numaa*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
int
ll_new_Numaa(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the NUMAA methods and functions in the LL_NUMAA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Numaa(lua_State *L) {
    static const luaL_Reg methods[] = {
        {"__gc",            Destroy},     /* Lua garbage collector */
        {"__new",           Create},      /* new NUMAA([n]) */
        {"__len",           GetCount},    /* #numa */
        {"Truncate",        Truncate},
        {"GetCount",        GetCount},
        {"GetNumaCount",    GetNumaCount},
        {"GetNumberCount",  GetNumberCount},
        {"AddNuma",         AddNuma},
        {"GetNuma",         GetNuma},
        {"ReplaceNuma",     ReplaceNuma},
        {"Read",            Read},
        {"Write",           Write},
        {"FlattenToNuma",   FlattenToNuma},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",          Create},
        {"CreateFull",      CreateFull},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_NUMAA, methods, functions);
}
