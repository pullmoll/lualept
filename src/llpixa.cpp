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
 *  Lua class PIXA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXA contained in the user data
 */
Pixa *
ll_check_Pixa(lua_State *L, int arg)
{
    return *(reinterpret_cast<Pixa **>(ll_check_udata(L, arg, LL_PIXA)));
}

/**
 * \brief Push Pixa* to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param pixa pointer to the PIXA
 * \return 1 Pixa* on the Lua stack
 */
int
ll_push_Pixa(lua_State *L, Pixa *pixa)
{
    if (!pixa)
        return ll_push_nil(L);
    return ll_push_udata(L, LL_PIXA, pixa);
}

/**
 * \brief Create a new Pixa*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
int
ll_new_Pixa(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(__func__, L, 1, 1);
    Pixa *pixa = pixaCreate(n);
    return ll_push_Pixa(L, pixa);
}

/**
 * \brief Create a new Pixa*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Pixa(L);
}

/**
 * \brief Destroy a Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 *
 * \param L pointer to the lua_State
 * \return 0 nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    Pixa **ppixa = reinterpret_cast<Pixa **>(ll_check_udata(L, 1, LL_PIXA));
    DBG(LOG_DESTROY, "%s: '%s' ppa=%p pa=%p\n", __func__,
        LL_PIXA, ppixa, *ppixa);
    pixaDestroy(ppixa);
    *ppixa = nullptr;
    return 0;
}

/**
 * \brief Copy a Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    Pixa *pixas = ll_check_Pixa(L, 1);
    l_int32 copyflag = ll_check_access_storage(__func__, L, 2, L_COPY);
    Pixa *pixa = pixaCopy(pixas, copyflag);
    return ll_push_Pixa(L, pixa);
}

/**
 * \brief Add a Pix* to a Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a Pix*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    Pix *pix = ll_check_Pix(L, 2);
    l_int32 flag = ll_check_access_storage(__func__, L, 3, L_COPY);
    lua_pushboolean(L, 0 == pixaAddPix(pixa, pix, flag));
    return 1;
}

/**
 * \brief Get count for a Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    lua_pushinteger(L, pixaGetCount(pixa));
    return 1;
}

/**
 * \brief Get Pix* from a Pixa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack: x, y, w, h
 */
static int
GetBoxGeometry(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    l_int32 x, y, w, h;
    if (pixaGetBoxGeometry(pixa, idx, &x, &y, &w, &h))
        return ll_push_nil(L);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Replace the Pix* in a Pixa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix*.
 * Arg #4 is optional and, if specified, expected to be a Box*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePix(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(L, 3);
    Box *boxs = lua_isuserdata(L, 4) ? ll_check_Box(L, 4) : nullptr;
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaReplacePix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Insert the Pix* in a Pixa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix*.
 * Arg #4 is optional and, if specified, expected to be a Box*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertPix(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(L, 3);
    Box *boxs = lua_isuserdata(L, 4) ? ll_check_Box(L, 4) : nullptr;
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaInsertPix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Remove the Pix* from a Pixa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemovePix(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    lua_pushboolean(L, 0 == pixaRemovePix(pixa, idx));
    return 1;
}

/**
 * \brief Remove the Pix* from a Pixa* at index %idx and return it
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*
 * Arg #2 is expected to be a l_int32 (%idx)
 *
 * \param L pointer to the lua_State
 * \return  2: one Pix* and one Box* on the Lua stack,
 *          1: one Pix* on the Lua stack,
 *          0: in case of error
 */
static int
RemovePixAndSave(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    Pix *pix = nullptr;
    Box *box = nullptr;
    if (pixaRemovePixAndSave(pixa, idx, &pix, &box))
        return ll_push_nil(L);
    return ll_push_Pix(L, pix) + ll_push_Box(L, box);
}

/**
 * \brief Join two Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be another Pixa*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    Pixa *pixad = ll_check_Pixa(L, 1);
    Pixa *pixas = ll_check_Pixa(L, 2);
    l_int32 istart = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 iend = ll_check_l_int32_default(__func__, L, 3, pixaGetCount(pixas));
    lua_pushboolean(L, 0 == pixaJoin(pixad, pixas, istart, iend));
    return 1;
}

/**
 * \brief Interleave two Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be another Pixa*.
 * Arg #3 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Interleave(lua_State *L)
{
    Pixa *pixa1 = ll_check_Pixa(L, 1);
    Pixa *pixa2 = ll_check_Pixa(L, 2);
    l_int32 copyflag = ll_check_access_storage(__func__, L, 3, L_CLONE);
    Pixa *pixa = pixaInterleave(pixa1, pixa2, copyflag);
    return ll_push_Pixa(L, pixa);
}

/**
 * \brief Clear the Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    lua_pushboolean(L, 0 == pixaClear(pixa));
    return 1;
}

/**
 * \brief Get pixel aligned statistics for Pixa*
 *
 * Arg #1 (i.e. self) is expected to be a Pixa*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetAlignedStats(lua_State *L)
{
    Pixa *pixa = ll_check_Pixa(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 3, 2);
    l_int32 thresh = ll_check_l_int32_default(__func__, L, 4, 0);
    Pix *pix = pixaGetAlignedStats(pixa, type, nbins, thresh);
    return ll_push_Pix(L, pix);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pixa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},           /* garbage collector */
        {"__new",               Create},            /* new Pixa */
        {"__len",               GetCount},          /* #pa */
        {"GetCount",            GetCount},
        {"Copy",                Copy},
        {"Destroy",             Destroy},
        {"AddPix",              AddPix},
        {"GetBoxGeometry",      GetBoxGeometry},
        {"ReplacePix",          ReplacePix},
        {"InsertPix",           InsertPix},
        {"RemovePix",           RemovePix},
        {"RemovePixAndSave",    RemovePixAndSave},
        {"TakePix",             RemovePixAndSave},  /* alias name */
        {"Join",                Join},
        {"Interleave",          Interleave},
        {"Clear",               Clear},
        {"GetAlignedStats",     GetAlignedStats},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_PIXA, methods, functions);
}
