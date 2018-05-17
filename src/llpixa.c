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
 *  Lua class PIXA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXA contained in the user data
 */
PIXA *
ll_check_PIXA(lua_State *L, int arg)
{
    return *(PIXA **)ll_check_udata(L, arg, LL_PIXA);
}

/**
 * \brief Push PIXA* to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param pixa pointer to the PIXA
 * \return 1 PIXA* on the Lua stack
 */
int
ll_push_PIXA(lua_State *L, PIXA *pixa)
{
    if (NULL == pixa)
        return 0;
    return ll_push_udata(L, LL_PIXA, pixa);
}

/**
 * \brief Create a new PIXA*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 PIXA* on the Lua stack
 */
int
ll_new_PIXA(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(__func__, L, 1, 1);
    PIXA *pixa = pixaCreate(n);
    return ll_push_PIXA(L, pixa);
}

/**
 * \brief Create a new PIXA*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 PIXA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_PIXA(L);
}

/**
 * \brief Destroy a PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 *
 * \param L pointer to the lua_State
 * \return 0 nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **ppixa = ll_check_udata(L, 1, LL_PIXA);
    DBG(LOG_DESTROY, "%s: '%s' ppa=%p pa=%p\n", __func__,
        LL_PIXA, (void *)ppixa, *ppixa);
    pixaDestroy((PIXA **)ppixa);
    *ppixa = NULL;
    return 0;
}

/**
 * \brief Copy a PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * \param L pointer to the lua_State
 * \return 1 PIXA* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    PIXA *pixas = ll_check_PIXA(L, 1);
    l_int32 copyflag = ll_check_access_storage(L, 2, L_COPY);
    PIXA *pixa = pixaCopy(pixas, copyflag);
    return ll_push_PIXA(L, pixa);
}

/**
 * \brief Add a PIX* to a PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be a PIX*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    PIX *pix = ll_check_PIX(L, 2);
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == pixaAddPix(pixa, pix, flag));
    return 1;
}

/**
 * \brief Get count for a PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    lua_pushinteger(L, pixaGetCount(pixa));
    return 1;
}

/**
 * \brief Get PIX* from a PIXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack: x, y, w, h
 */
static int
GetBoxGeometry(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    l_int32 x, y, w, h;
    if (pixaGetBoxGeometry(pixa, idx, &x, &y, &w, &h))
        return 0;
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Replace the PIX* in a PIXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a PIX*.
 * Arg #4 is optional and, if specified, expected to be a BOX*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePix(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    PIX *pixs = ll_check_PIX(L, 3);
    BOX *boxs = lua_isuserdata(L, 4) ? ll_check_BOX(L, 4) : NULL;
    PIX *pix = pixClone(pixs);
    BOX *box = boxs ? boxClone(boxs) : NULL;
    lua_pushboolean(L, pix && 0 == pixaReplacePix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Insert the PIX* in a PIXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a PIX*.
 * Arg #4 is optional and, if specified, expected to be a BOX*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertPix(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    PIX *pixs = ll_check_PIX(L, 3);
    BOX *boxs = lua_isuserdata(L, 4) ? ll_check_BOX(L, 4) : NULL;
    PIX *pix = pixClone(pixs);
    BOX *box = boxs ? boxClone(boxs) : NULL;
    lua_pushboolean(L, pix && 0 == pixaInsertPix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Remove the PIX* from a PIXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemovePix(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    lua_pushboolean(L, 0 == pixaRemovePix(pixa, idx));
    return 1;
}

/**
 * \brief Remove the PIX* from a PIXA* at index %idx and return it
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*
 * Arg #2 is expected to be a l_int32 (%idx)
 *
 * \param L pointer to the lua_State
 * \return  2: one PIX* and one BOX* on the Lua stack,
 *          1: one PIX* on the Lua stack,
 *          0: in case of error
 */
static int
RemovePixAndSave(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, pixaGetCount(pixa));
    PIX *pix = NULL;
    BOX *box = NULL;
    if (pixaRemovePixAndSave(pixa, idx, &pix, &box))
        return 0;
    return ll_push_PIX(L, pix) + ll_push_BOX(L, box);
}

/**
 * \brief Join two PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be another PIXA*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    PIXA *pixad = ll_check_PIXA(L, 1);
    PIXA *pixas = ll_check_PIXA(L, 2);
    l_int32 istart = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 iend = ll_check_l_int32_default(__func__, L, 3, pixaGetCount(pixas));
    lua_pushboolean(L, 0 == pixaJoin(pixad, pixas, istart, iend));
    return 1;
}

/**
 * \brief Interleave two PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*.
 * Arg #2 is expected to be another PIXA*.
 * Arg #3 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * \param L pointer to the lua_State
 * \return 1 PIXA* on the Lua stack
 */
static int
Interleave(lua_State *L)
{
    PIXA *pixa1 = ll_check_PIXA(L, 1);
    PIXA *pixa2 = ll_check_PIXA(L, 2);
    l_int32 copyflag = ll_check_access_storage(L, 3, L_CLONE);
    PIXA *pixa = pixaInterleave(pixa1, pixa2, copyflag);
    return ll_push_PIXA(L, pixa);
}

/**
 * \brief Clear the PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    lua_pushboolean(L, 0 == pixaClear(pixa));
    return 1;
}

/**
 * \brief Get pixel aligned statistics for PIXA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXA*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetAlignedStats(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 3, 2);
    l_int32 thresh = ll_check_l_int32_default(__func__, L, 4, 0);
    PIX *pix = pixaGetAlignedStats(pixa, type, nbins, thresh);
    return ll_push_PIX(L, pix);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PIXA(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},           /* garbage collector */
        {"__new",               Create},            /* new PIXA */
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
