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

/*====================================================================*
 *
 *  Lua class PTAA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PTAA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PTAA* contained in the user data
 */
PTAA *
ll_check_PTAA(lua_State *L, int arg)
{
    return *(PTAA **)ll_check_udata(L, arg, LL_PTAA);
}

/**
 * \brief Push PTAA user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param ptaa pointer to the PTAA
 * \return 1 PTAA* on the Lua stack
 */
int
ll_push_PTAA(lua_State *L, PTAA *ptaa)
{
    if (NULL == ptaa)
        return 0;
    return ll_push_udata(L, LL_PTAA, ptaa);
}

/**
 * \brief Create and push a new PTAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 for PTAA* on the Lua stack
 */
int
ll_new_PTAA(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(L, 1, 1);
    PTAA *ptaa = ptaaCreate(n);
    return ll_push_PTAA(L, ptaa);
}

/**
 * \brief Create a new PTAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 for PTAA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_PTAA(L);
}

/**
 * \brief Truncate the arrays stored in the PTAA*
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    lua_pushboolean(L, 0 == ptaaTruncate(ptaa));
    return 1;
}

/**
 * \brief Destroy a PTAA*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pptaa = ll_check_udata(L, 1, LL_PTAA);
    DBG(LOG_DESTROY, "%s: '%s' pptaa=%p ptaa=%p\n", __func__,
        LL_PTAA, (void *)pptaa, *pptaa);
    ptaaDestroy((PTAA **)pptaa);
    *pptaa = NULL;
    return 0;
}

/**
 * \brief Get the number of arrays stored in the PTAA*
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    l_int32 n = ptaaGetCount(ptaa);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Get the Point stored in the PTAA* at PTA* index %ipta and Point index
 * %jpt
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 * Arg #2 is expected to be a l_int32 (ipta)
 * Arg #3 is expected to be a l_int32 (jpt)
 *
 * \param L pointer to the lua_State
 * \return 2 for two lua_Numbers on the Lua stack
 */
static int
GetPt(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    l_int32 ipta = ll_check_index(L, 2, ptaaGetCount(ptaa));
    l_int32 ipt = ll_check_index(L, 3, INT32_MAX);
    l_float32 x;
    l_float32 y;
    if (ptaaGetPt(ptaa, ipta, ipt, &x, &y))
        return 0;
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
    return 2;
}

/**
 * \brief Get the PTA* in the PTAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 PTA* on the Lua stack
 */
static int
GetPta(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    l_int32 idx = ll_check_index(L, 2, ptaaGetCount(ptaa));
    l_int32 accessflag = ll_check_access_storage(L, 3, L_COPY);
    PTA *pta = ptaaGetPta(ptaa, idx, accessflag);
    return ll_push_PTA(L, pta);
}

/**
 * \brief Get the PTA* in the PTAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 * Arg #2 is expected to be a PTA* user data
 * Arg #3 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPta(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    PTA *pta = ll_check_PTA(L, 2);
    l_int32 copyflag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == ptaaAddPta(ptaa, pta, copyflag));
    return 1;
}

/**
 * \brief Replace a PTA* in the PTAA* at %idx
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a PTA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePta(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    l_int32 idx = ll_check_index(L, 2, ptaaGetCount(ptaa));
    PTA *pta = ll_check_PTA(L, 3);
    lua_pushboolean(L, 0 == ptaaReplacePta(ptaa, idx, pta));
    return 1;
}

/**
 * \brief Read a PTAA* from an external file
 *
 * Arg #1 is expected to be a string containing the filename
 *
 * \param L pointer to the lua_State
 * \return 1 PTAA* on the Lua stack
 */
static int
Read(lua_State *L)
{
    const char *filename = lua_tostring(L, 1);
    PTAA *ptaa = ptaaRead(filename);
    return ll_push_PTAA(L, ptaa);
}

/**
 * \brief Write the PTAA* to and external file
 *
 * Arg #1 (i.e. self) is expected to be a PTAA* user data
 * Arg #2 is expected to be string containing the filename
 * Arg #3 is an optional boolean; if true, the data is
 *                written as integers, otherwise as floats
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    PTAA *ptaa = ll_check_PTAA(L, 1);
    const char *filename = lua_tostring(L, 2);
    int type = lua_toboolean(L, 3);
    lua_pushboolean(L, 0 == ptaaWrite(filename, ptaa, type));
    return 1;
}

/**
 * \brief Register the PTAA methods and functions in the LL_PTAA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PTAA(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",        Destroy},      /* garbage collector */
        {"__new",       Create},       /* new PTAA */
        {"__len",       GetCount},     /* #ptaa */
        {"Truncate",    Truncate},
        {"GetPt",       GetPt},
        {"AddPta",      AddPta},
        {"GetPta",      GetPta},
        {"ReplacePta",  ReplacePta},
        {"Read",        Read},
        {"Write",       Write},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",      Create},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_PTAA, methods, functions);
}
