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
 *  Lua class NUMA
 *
 *====================================================================*/

/**
 * \brief Create a new Numa*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_NUMA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Numa *na = numaCreate(n);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Destroy a Numa*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_NUMA ".Destroy");
    Numa **pna = reinterpret_cast<Numa **>(ll_check_udata(_fun, L, 1, LL_NUMA));
    DBG(LOG_DESTROY, "%s: '%s' pna=%p na=%p refcount=%d\n",
         _fun, LL_NUMA, pna, *pna, numaGetRefcount(*pna));
    numaDestroy(pna);
    *pna = nullptr;
    return 0;
}

/**
 * \brief Get the number of stored numbers in the Numa* (na)
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_NUMA ".Destroy");
    Numa* na = ll_check_Numa(_fun, L, 1);
    l_int32 n = numaGetCount(na);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Set the number of stored numbers in the Numa*, i.e. resize Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetCount(lua_State *L)
{
    FUNC(LL_NUMA ".SetCount");
    Numa* na = ll_check_Numa(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == numaSetCount(na, n));
    return 1;
}

/**
 * \brief Copy a Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_NUMA ".Copy");
    Numa *nas = ll_check_Numa(_fun, L, 1);
    Numa *na = numaCopy(nas);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Clone a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    FUNC(LL_NUMA ".Clone");
    Numa *nas = ll_check_Numa(_fun, L, 1);
    Numa *na = numaClone(nas);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Set the number of stored numbes in the Numa* to zero
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
Empty(lua_State *L)
{
    FUNC(LL_NUMA ".Empty");
    Numa *na = ll_check_Numa(_fun, L, 1);
    lua_pushboolean(L, 0 == numaEmpty(na));
    return 1;
}

/**
 * \brief Add one number to the Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Number in the range of l_float32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
AddNumber(lua_State *L)
{
    FUNC(LL_NUMA ".AddNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 val = ll_check_l_float32(_fun, L, 2);
    lua_pushboolean(L, 0 == numaAddNumber(na, val));
    return 1;
}

/**
 * \brief Insert one number to the Numa* at the given index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 for the
 * index Arg #3 is expected to be a lua_Number in the range of l_float32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
InsertNumber(lua_State *L)
{
    FUNC(LL_NUMA ".InsertNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == numaInsertNumber(na, idx, val));
    return 1;
}

/**
 * \brief Remove one number to the Numa* at the given index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
RemoveNumber(lua_State *L)
{
    FUNC(LL_NUMA ".RemoveNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    lua_pushboolean(L, 0 == numaRemoveNumber(na, idx));
    return 1;
}

/**
 * \brief Replace one number to the Numa* at the given index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 for the
 * index Arg #3 is expected to be a lua_Number in the range of l_float32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
ReplaceNumber(lua_State *L)
{
    FUNC(LL_NUMA ".ReplaceNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == numaReplaceNumber(na, idx, val));
    return 1;
}

/**
 * \brief Get the l_float32 from the Numa* at index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetFValue(lua_State *L)
{
    FUNC(LL_NUMA ".GetFValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val;
    if (numaGetFValue(na, idx, &val))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)val);
    return 1;
}

/**
 * \brief Get the l_int32 from the Numa* at index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetIValue(lua_State *L)
{
    FUNC(LL_NUMA ".GetIValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_int32 val;
    if (numaGetIValue(na, idx, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Set the value for the Numa* at the given index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 for the
 * index Arg #3 is expected to be a lua_Number in the range of l_float32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetValue(lua_State *L)
{
    FUNC(LL_NUMA ".SetValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == numaSetValue(na, idx, val));
    return 1;
}

/**
 * \brief Add a difference to the value for the Numa* at the given index
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Integer in the range of l_int32 for the
 * index Arg #3 is expected to be a lua_Number in the range of l_float32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
ShiftValue(lua_State *L)
{
    FUNC(LL_NUMA ".ShiftValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 diff = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == numaShiftValue(na, idx, diff));
    return 1;
}

/**
 * \brief Get the Numa* as a table of lua_Number
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetFArray(lua_State *L)
{
    FUNC(LL_NUMA ".GetFArray");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    l_float32 *fa = numaGetFArray(na, copyflag);
    l_int32 n = numaGetCount(na);
    int res = ll_push_farray(L, fa, n);
    LEPT_FREE(fa);
    return res;
}

/**
 * \brief Get the Numa* as an array of lua_Integer
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetIArray(lua_State *L)
{
    FUNC(LL_NUMA ".GetIArray");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 *ia = numaGetIArray(na);
    l_int32 n = numaGetCount(na);
    int res = ll_push_iarray(L, ia, n);
    LEPT_FREE(ia);
    return res;
}

/**
 * \brief Get the parameters of the Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 *
 * \param L pointer to the lua_State
 * \return 2 for Numa* starx and deltax
 */
static int
GetParameters(lua_State *L)
{
    FUNC(LL_NUMA ".GetParameters");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 startx = 0;
    l_float32 deltax = 0;
    if (numaGetParameters(na, &startx, &deltax))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)startx);
    lua_pushnumber(L, (lua_Number)deltax);
    return 2;
}

/**
 * \brief Set the parameters of the Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Numa*
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (startx)
 * Arg #3 is expected to be a lua_Number in the range of l_float32 (deltax)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetParameters(lua_State *L)
{
    FUNC(LL_NUMA ".SetParameters");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 startx = ll_check_l_float32(_fun, L, 2);
    l_float32 deltax = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == numaSetParameters(na, startx, deltax));
    return 1;
}

/**
 * \brief Copy the parameters of the Numa* (%nas) to the Numa* (%nad)
 *
 * Arg #1 (i.e. self) is expected to be a Numa* (nad)
 * Arg #2 is expected to be another Numa* (nas)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyParameters(lua_State *L)
{
    FUNC(LL_NUMA ".CopyParameters");
    Numa *nad = ll_check_Numa(_fun, L, 1);
    Numa *nas = ll_check_Numa(_fun, L, 2);
    lua_pushboolean(L, 0 == numaCopyParameters(nad, nas));
    return 1;
}

/**
 * @brief Check Lua stack at index %arg for udata of class LL_NUMA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numa* contained in the user data
 */
Numa *
ll_check_Numa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Numa **>(ll_check_udata(_fun, L, arg, LL_NUMA)));
}

/**
 * \brief Push NUMA user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param na pointer to the NUMA
 * \return 1 Numa* on the Lua stack
 */
int
ll_push_Numa(const char *_fun, lua_State *L, Numa *na)
{
    if (!na)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_NUMA, na);
}

/**
 * \brief Create and push NUMA user data to the Lua stack
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
int
ll_new_Numa(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the NUMA methods and functions in the LL_NUMA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Numa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},          /* garbage collector */
        {"__new",               Create},           /* new Numa */
        {"__len",               GetCount},         /* #numa */
        {"__newitem",           ReplaceNumber},    /* numa[index] = number */
        {"Clone",               Clone},
        {"Copy",                Copy},
        {"Empty",               Empty},
        {"AddNumber",           AddNumber},
        {"InsertNumber",        InsertNumber},
        {"RemoveNumber",        RemoveNumber},
        {"ReplaceNumber",       ReplaceNumber},
        {"SetCount",            SetCount},
        {"GetFValue",           GetFValue},
        {"GetIValue",           GetIValue},
        {"SetValue",            SetValue},
        {"ShiftValue",          ShiftValue},
        {"GetFArray",           GetFArray},
        {"GetIArray",           GetIArray},
        {"GetParameters",       GetParameters},
        {"SetParameters",       SetParameters},
        {"CopyParameters",      CopyParameters},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_NUMA, methods, functions);
}
