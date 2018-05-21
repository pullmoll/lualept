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
 *  Lua class DNA
 *
 *====================================================================*/

/**
 * \brief Printable string for a L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_DNA ".toString");
    static char str[256];
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i;
    l_float64 val;

    luaL_buffinit(L, &B);
    if (!da) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_DNA ": %p",
                 reinterpret_cast<void *>(da));
        luaL_addstring(&B, str);
        for (i = 0; i < l_dnaGetCount(da); i++) {
            l_dnaGetDValue(da, i, &val);
            snprintf(str, sizeof(str), "\n    %d = %.15g", i+1, val);
            luaL_addstring(&B, str);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new L_Dna*
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_DNA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    L_Dna *da = l_dnaCreate(n);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Destroy a L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_DNA ".Destroy");
    L_Dna **pda = reinterpret_cast<L_Dna**>(ll_check_udata(_fun, L, 1, LL_DNA));
    DBG(LOG_DESTROY, "%s: '%s' pda=%p da=%p refcount=%d\n",
         _fun, LL_DNA, pda, *pda, l_dnaGetRefcount(*pda));
    l_dnaDestroy(pda);
    *pda = nullptr;
    return 0;
}

/**
 * \brief Get the number of numbers stored in the L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_DNA ".GetCount");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 n = l_dnaGetCount(da);
    lua_pushinteger(L, n);
    return 1;
}

/**
 * \brief Set the number of stored numbers in the L_Dna*, i.e. resize L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetCount(lua_State *L)
{
    FUNC(LL_DNA ".SetCount");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == l_dnaSetCount(da, n));
    return 1;
}

/**
 * \brief Copy a L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_DNA ".Copy");
    L_Dna *das = ll_check_Dna(_fun, L, 1);
    L_Dna *da = l_dnaCopy(das);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Clone a L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    FUNC(LL_DNA ".Clone");
    L_Dna *das = ll_check_Dna(_fun, L, 1);
    L_Dna *da = l_dnaClone(das);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Set the number of stored numbes in the L_Dna* to zero
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Empty(lua_State *L)
{
    FUNC(LL_DNA ".Empty");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    lua_pushboolean(L, 0 == l_dnaEmpty(da));
    return 1;
}

/**
 * \brief Add one number to the L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a lua_Number to add to the array.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddNumber(lua_State *L)
{
    FUNC(LL_DNA ".AddNumber");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 2, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaAddNumber(da, val));
    return 1;
}

/**
 * \brief Insert one number to the L_Dna* at the given index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to insert into the array.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertNumber(lua_State *L)
{
    FUNC(LL_DNA ".InsertNumber");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(
        L, isnumber && 0 == l_dnaInsertNumber(da, idx, val) ? TRUE : FALSE);
    return 1;
}

/**
 * \brief Remove one number to the L_Dna* at the given index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveNumber(lua_State *L)
{
    FUNC(LL_DNA ".RemoveNumber");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    lua_pushboolean(L, 0 == l_dnaRemoveNumber(da, idx));
    return 1;
}

/**
 * \brief Replace one number to the L_Dna* at the given index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to use to replace.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceNumber(lua_State *L)
{
    FUNC(LL_DNA ".ReplaceNumber");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaReplaceNumber(da, idx, val) ? TRUE : FALSE);
    return 1;
}

/**
 * \brief Get the l_float64 from the L_Dna* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDValue(lua_State *L)
{
    FUNC(LL_DNA ".GetDValue");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    lua_Number val;
    if (l_dnaGetDValue(da, idx, &val))
        return ll_push_nil(L);
    lua_pushnumber(L, val);
    return 1;
}

/**
 * \brief Get the l_int32 from the L_Dna* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetIValue(lua_State *L)
{
    FUNC(LL_DNA ".GetIValue");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_int32 val;
    if (l_dnaGetIValue(da, idx, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Set the value for the L_Dna* at the given index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to set in the array.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetValue(lua_State *L)
{
    FUNC(LL_DNA ".SetValue");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaSetValue(da, idx, val));
    return 1;
}

/**
 * \brief Add a difference to the value for the L_Dna* at the given index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float32.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ShiftValue(lua_State *L)
{
    FUNC(LL_DNA ".ShiftValue");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    int isnumber = 0;
    lua_Number diff = lua_tonumberx(L, 3, &isnumber);
    lua_pushboolean(L, isnumber && 0 == l_dnaShiftValue(da, idx, diff));
    return 1;
}

/**
 * \brief Get the L_Dna* as table of lua_Number
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
static int
GetDArray(lua_State *L)
{
    FUNC(LL_DNA ".GetDArray");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    l_float64 *darray = l_dnaGetDArray(da, L_COPY);
    l_int32 n = l_dnaGetCount(da);
    int res = ll_push_darray(L, darray, n);
    LEPT_FREE(darray);
    return res;
}

/**
 * \brief Get the L_Dna* as a table of lua_Integer
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
static int
GetIArray(lua_State *L)
{
    FUNC(LL_DNA ".GetIArray");
    L_Dna *dna = ll_check_Dna(_fun, L, 1);
    l_int32 *ia = l_dnaGetIArray(dna);
    l_int32 n = l_dnaGetCount(dna);
    int res = ll_push_iarray(L, ia, n);
    LEPT_FREE(ia);
    return res;
}

/**
 * \brief Get the parameters of the L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for L_Dna* starx and deltax
 */
static int
GetParameters(lua_State *L)
{
    FUNC(LL_DNA ".GetParameters");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    lua_Number startx = 0;
    lua_Number deltax = 0;
    if (l_dnaGetParameters(da, &startx, &deltax))
        return ll_push_nil(L);
    lua_pushnumber(L, startx);
    lua_pushnumber(L, deltax);
    return 2;
}

/**
 * \brief Set the parameters of the L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data.
 * Arg #2 is expected to be a lua_Number (startx).
 * Arg #3 is expected to be a lua_Number (deltax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetParameters(lua_State *L)
{
    FUNC(LL_DNA ".SetParameters");
    L_Dna *da = ll_check_Dna(_fun, L, 1);
    int isnumber1 = 0, isnumber2 = 0;
    lua_Number startx = lua_tonumberx(L, 2, &isnumber1);
    lua_Number deltax = lua_tonumberx(L, 3, &isnumber2);
    lua_pushboolean(L, isnumber1 && isnumber2 && 0 == l_dnaSetParameters(da, startx, deltax));
    return 1;
}

/**
 * \brief Copy the parameters of the L_Dna*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_Dna* user data (destination).
 * Arg #2 is expected to be another L_Dna* user data (source).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyParameters(lua_State *L)
{
    FUNC(LL_DNA ".CopyParameters");
    L_Dna *dad = ll_check_Dna(_fun, L, 1);
    L_Dna *das = ll_check_Dna(_fun, L, 2);
    lua_pushboolean(L, 0 == l_dnaCopyParameters(dad, das));
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_DNA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_Dna* contained in the user data
 */
L_Dna *
ll_check_Dna(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<L_Dna **>(ll_check_udata(_fun, L, arg, LL_DNA)));
}

/**
 * \brief Optionally expect a LL_DNA at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_Dna* contained in the user data
 */
L_Dna *
ll_check_Dna_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Dna(_fun, L, arg);
}

/**
 * \brief Push DNA user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param da pointer to the L_DNA
 * \return 1 DNA* on the Lua stack
 */
int
ll_push_Dna(const char *_fun, lua_State *L, L_Dna *da)
{
    if (!da)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DNA, da);
}

/**
 * \brief Create and push a new DNA*
 * \param L pointer to the lua_State
 * \return 1 DNA* on the Lua stack
 */
int
ll_new_Dna(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the L_DNA methods and functions in the LL_L_DNA meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Dna(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* Lua garbage collector */
        {"__new",               Create},    /* Lua new object */
        {"__len",               GetCount},  /* #dna */
        {"__tostring",          toString},
        {"Clone",               Clone},
        {"Copy",                Copy},
        {"Empty",               Empty},
        {"AddNumber",           AddNumber},
        {"InsertNumber",        InsertNumber},
        {"RemoveNumber",        RemoveNumber},
        {"ReplaceNumber",       ReplaceNumber},
        {"SetCount",            SetCount},
        {"GetFValue",           GetDValue},
        {"GetIValue",           GetIValue},
        {"SetValue",            SetValue},
        {"ShiftValue",          ShiftValue},
        {"GetDArray",           GetDArray},
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

    int res = ll_register_class(L, LL_DNA, methods, functions);
    lua_setglobal(L, LL_DNA);
    return res;
}
