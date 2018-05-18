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
 *  Lua class PTA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PTA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pta* contained in the user data
 */
Pta *
ll_check_Pta(lua_State *L, int arg)
{
    return *(reinterpret_cast<Pta **>(ll_check_udata(L, arg, LL_PTA)));
}

/**
 * \brief Push PTA user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param pta pointer to the PTA
 * \return 1 Pta* on the Lua stack
 */
int
ll_push_Pta(lua_State *L, Pta *pta)
{
    if (!pta)
        return 0;
    return ll_push_udata(L, LL_PTA, pta);
}

/**
 * \brief Create and push a new Pta*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 for Pta* on the Lua stack
 */
int
ll_new_Pta(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(__func__, L, 1, 1);
    Pta *pa = ptaCreate(n);
    return ll_push_Pta(L, pa);
}

/**
 * @brief Printable string for a Box*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    Pta *pta = ll_check_Pta(L, 1);
    luaL_Buffer B;
    l_int32 i;
    l_float32 px, py;

    luaL_buffinit(L, &B);
    if (!pta) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        for (i = 0; i < ptaGetCount(pta); i++) {
            if (ptaGetPt(pta, i, &px, &py)) {
                snprintf(str, sizeof(str), "{nil}");
            } else {
                snprintf(str, sizeof(str), "{%g,%g}", (lua_Number)px, (lua_Number)py);
            }
            if (i > 0)
                luaL_addchar(&B, ',');
            luaL_addstring(&B, str);
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new Pta*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 for Pta* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Pta(L);
}

/**
 * \brief Destroy a Pta*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    Pta **ppta = reinterpret_cast<Pta **>(ll_check_udata(L, 1, LL_PTA));
    DBG(LOG_DESTROY, "%s: '%s' ppta=%p pta=%p\n",
        __func__, LL_PTA, ppta, *ppta);
    ptaDestroy(ppta);
    *ppta = nullptr;
    return 0;
}

/**
 * \brief Get the number of stored numbers in the Pta*
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    lua_pushinteger(L, ptaGetCount(pta));
    return 1;
}

/**
 * \brief Copy a Pta*
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for Pta* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    Pta *ptas = ll_check_Pta(L, 1);
    Pta *pta = ptaCopy(ptas);
    return ll_push_Pta(L, pta);
}

/**
 * \brief Clone a Pta*
 *
 * \param L pointer to the lua_State
 * \return 1 for Pta* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    Pta *ptas = ll_check_Pta(L, 1);
    Pta *pta = ptaClone(ptas);
    return ll_push_Pta(L, pta);
}

/**
 * \brief Copy a range %istart - %iend from a Pta* %ptas
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a l_int32 (istart)
 * Arg #3 is expected to be a l_int32 (uend)
 *
 * \param L pointer to the lua_State
 * \return 1 for Pta* on the Lua stack
 */
static int
CopyRange(lua_State *L)
{
    Pta *ptas = ll_check_Pta(L, 1);
    l_int32 istart = ll_check_l_int32_default(__func__, L, 2, 1);
    l_int32 iend = ll_check_l_int32_default(__func__, L, 3, ptaGetCount(ptas));
    Pta *pta = ptaCopyRange(ptas, istart, iend);
    return ll_push_Pta(L, pta);
}

/**
 * \brief Set the number of stored numbes in the Pta* to zero
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
Empty(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    lua_pushboolean(L, 0 == ptaEmpty(pta));
    return 1;
}

/**
 * \brief Add one number to the Pta*
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (x)
 * Arg #3 is expected to be a lua_Number in the range of l_float32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
AddPt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_float32 x = ll_check_l_float32(__func__, L, 2);
    l_float32 y = ll_check_l_float32(__func__, L, 3);
    lua_pushboolean(L, 0 == ptaAddPt(pta, x, y));
    return 1;
}

/**
 * \brief Insert one number to the Pta* at the given index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 (idx)
 * Arg #3 is expected to be a lua_Integer in the range of l_int32 (x)
 * Arg #4 is expected to be a lua_Integer in the range of l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
InsertPt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, ptaGetCount(pta));
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    lua_pushboolean(L, 0 == ptaInsertPt(pta, idx, x, y));
    return 1;
}

/**
 * \brief Remove one number to the Pta* at the given index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
RemovePt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, ptaGetCount(pta));
    lua_pushboolean(L, 0 == ptaRemovePt(pta, idx));
    return 1;
}

/**
 * \brief Get the Point from the Pta* at index %idx as two l_floa32 numbers
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, ptaGetCount(pta));
    l_float32 x = 0;
    l_float32 y = 0;
    if (ptaGetPt(pta, idx, &x, &y))
        return 0;
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
    return 2;
}

/**
 * \brief Get the Point from the Pta* at index %idx as two l_int32 numbers
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetIPt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, ptaGetCount(pta));
    l_int32 x;
    l_int32 y;
    if (ptaGetIPt(pta, idx, &x, &y))
        return 0;
    lua_pushinteger(L, (lua_Integer)x);
    lua_pushinteger(L, (lua_Integer)y);
    return 2;
}

/**
 * \brief Set the value for the Pta* at the given index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 (idx)
 * Arg #3 is expected to be a lua_Number in the range of l_float32 (x)
 * Arg #4 is expected to be a lua_Number in the range of l_float32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetPt(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, ptaGetCount(pta));
    l_float32 x = ll_check_l_float32(__func__, L, 3);
    l_float32 y = ll_check_l_float32(__func__, L, 4);
    lua_pushboolean(L, 0 == ptaSetPt(pta, idx, x, y));
    return 1;
}

/**
 * \brief Get the Pta* as an two Numa* for X and Y
 *
 * Arg #1 (i.e. self) is expected to be a Pta* user data
 *
 * \param L pointer to the lua_State
 * \return 2 for two Numa* on the Lua stack, or 0 in case of error
 */
static int
GetArrays(lua_State *L)
{
    Pta *pta = ll_check_Pta(L, 1);
    Numa *ptax = nullptr;
    Numa *ptay = nullptr;
    if (ptaGetArrays(pta, &ptax, &ptay))
        return 0;
    return ll_push_Numa(L, ptax) + ll_push_Numa(L, ptay);
}

/**
 * \brief Register the PTA methods and functions in the LL_PTA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pta(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",        Destroy},       /* garbage collector */
        {"__new",       Create},        /* new Pta */
        {"__len",       GetCount},      /* #pta */
        {"__tostring",  toString},
        {"Destroy",     Destroy},
        {"Clone",       Clone},
        {"Copy",        Copy},
        {"CopyRange",   CopyRange},
        {"Empty",       Empty},
        {"AddPt",       AddPt},
        {"InsertPt",    InsertPt},
        {"RemovePt",    RemovePt},
        {"GetPt",       GetPt},
        {"GetIPt",      GetIPt},
        {"SetPt",       SetPt},
        {"GetArrays",   GetArrays},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",      Create},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_PTA, methods, functions);
}
