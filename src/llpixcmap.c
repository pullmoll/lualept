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
 *  Lua class PIXCMAP
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXCMAP
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXCMAP* contained in the user data
 */
PIXCMAP *
ll_check_PIXCMAP(lua_State *L, int arg)
{
    return *(PIXCMAP **)ll_check_udata(L, arg, LL_PIXCMAP);
}

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXCMAP
 *
 * This version removes the PIXCMAP from the object.
 * It is used when the PIXCMAP is e.g. attached to a PIX.
 * The reason is that a PIXCMAP does not have a reference
 * count and thus can be used exactly once in PIX:SetColormap().
 *
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXCMAP* contained in the user data
 */
PIXCMAP *
ll_take_PIXCMAP(lua_State *L, int arg)
{
    PIXCMAP **pcmap = (PIXCMAP **)ll_check_udata(L, arg, LL_PIXCMAP);
    PIXCMAP *cmap = *pcmap;
    *pcmap = NULL;
    return cmap;
}

/**
 * \brief Push PIXCMAP* user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param cmap pointer to the PIXCMAP
 * \return 1 PIXCMAP* on the Lua stack
 */
int
ll_push_PIXCMAP(lua_State *L, PIXCMAP *cmap)
{
    if (NULL == cmap)
        return 0;
    return ll_push_udata(L, LL_PIXCMAP, cmap);
}

/**
 * \brief Create a new PIXCMAP*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
int
ll_new_PIXCMAP(lua_State *L)
{
    l_int32 depth = ll_check_l_int32(L, 1);
    PIXCMAP *cmap = pixcmapCreate(depth);
    return ll_push_PIXCMAP(L, cmap);
}

/**
 * \brief Create a new PIXCMAP*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_PIXCMAP(L);
}

/**
 * \brief Create a random PIXCMAP*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 * Arg #2 is optional and, if specified, expected to be a boolean (hasblack)
 * Arg #3 is optional and, if specified, expected to be a boolean (haswhite)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
CreateRandom(lua_State *L)
{
    l_int32 depth = ll_check_l_int32(L, 1);
    l_int32 hasblack = lua_toboolean(L, 2);
    l_int32 haswhite = lua_toboolean(L, 3);
    PIXCMAP *cmap = pixcmapCreateRandom(depth, hasblack, haswhite);
    return ll_push_PIXCMAP(L, cmap);
}

/**
 * \brief Create a linear PIXCMAP*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 * Arg #2 is expected to be a l_int32 (levels)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
CreateLinear(lua_State *L)
{
    l_int32 depth = ll_check_l_int32(L, 1);
    l_int32 levels = ll_check_l_int32(L, 1);
    PIXCMAP *cmap = pixcmapCreateLinear(depth, levels);
    return ll_push_PIXCMAP(L, cmap);
}

/**
 * \brief Destroy a PIXCMAP*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pcmap = ll_check_udata(L, 1, LL_PIXCMAP);
    DBG(LOG_DESTROY, "%s: '%s' pcmap=%p cmap=%p\n", __func__,
        LL_PIXCMAP, (void *)pcmap, *pcmap);
    pixcmapDestroy((PIXCMAP **)pcmap);
    *pcmap = NULL;
    return 0;
}

/**
 * \brief Copy a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmaps)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    PIXCMAP *cmaps = ll_check_PIXCMAP(L, 1);
    PIXCMAP *cmap = pixcmapCopy(cmaps);
    return ll_push_PIXCMAP(L, cmap);
}

/**
 * \brief Add a color to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddColor(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 rval = ll_check_l_int32(L, 2);
    l_int32 gval = ll_check_l_int32(L, 3);
    l_int32 bval = ll_check_l_int32(L, 4);
    lua_pushboolean(L, 0 == pixcmapAddColor(cmap, rval, gval, bval));
    return 1;
}

/**
 * \brief Add a RGBA color to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 * Arg #5 is expected to be a l_int32 (aval)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddRGBA(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 rval = ll_check_l_int32(L, 2);
    l_int32 gval = ll_check_l_int32(L, 3);
    l_int32 bval = ll_check_l_int32(L, 4);
    l_int32 aval = ll_check_l_int32(L, 5);
    lua_pushboolean(L, 0 == pixcmapAddRGBA(cmap, rval, gval, bval, aval));
    return 1;
}

/**
 * \brief Add a new color to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
AddNewColor(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 rval = ll_check_l_int32(L, 2);
    l_int32 gval = ll_check_l_int32(L, 3);
    l_int32 bval = ll_check_l_int32(L, 4);
    l_int32 index = 0;
    if (pixcmapAddNewColor(cmap, rval, gval, bval, &index))
        return 0;
    lua_pushinteger(L, index + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add a nearest color to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
AddNearestColor(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 rval = ll_check_l_int32(L, 2);
    l_int32 gval = ll_check_l_int32(L, 3);
    l_int32 bval = ll_check_l_int32(L, 4);
    l_int32 index = 0;
    if (pixcmapAddNearestColor(cmap, rval, gval, bval, &index))
        return 0;
    lua_pushinteger(L, index + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Get usable color from a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
UsableColor(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 rval = ll_check_l_int32(L, 2);
    l_int32 gval = ll_check_l_int32(L, 3);
    l_int32 bval = ll_check_l_int32(L, 4);
    l_int32 usable = 0;
    if (pixcmapUsableColor(cmap, rval, gval, bval, &usable))
        return 0;
    lua_pushinteger(L, usable + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add black or white to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a l_int32 (color)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
AddBlackOrWhite(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 color = ll_check_l_int32(L, 2);
    l_int32 index = 0;
    if (pixcmapAddBlackOrWhite(cmap, color, &index))
        return 0;
    lua_pushinteger(L, index + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Set black and white to a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 * Arg #2 is expected to be a boolean (setblack)
 * Arg #3 is expected to be a boolean (setwhite)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBlackAndWhite(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 setblack = lua_toboolean(L, 2);
    l_int32 setwhite = lua_toboolean(L, 3);
    lua_pushboolean(L, 0 == pixcmapSetBlackAndWhite(cmap, setblack, setwhite));
    return 1;
}

/**
 * \brief Get count of colors in a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 count = pixcmapGetCount(cmap);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the depth of a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
GetDepth(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 depth = pixcmapGetDepth(cmap);
    lua_pushinteger(L, depth);
    return 1;
}

/**
 * \brief Get the minimum depth of a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
GetMinDepth(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 mindepth = 0;
    if (pixcmapGetMinDepth(cmap, &mindepth))
        return 0;
    lua_pushinteger(L, mindepth);
    return 1;
}

/**
 * \brief Get the count of free colors of a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
GetFreeCount(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    l_int32 freecount = pixcmapGetFreeCount(cmap);
    lua_pushinteger(L, freecount);
    return 1;
}

/**
 * \brief Clear the colors of a PIXCMAP*
 *
 * Arg #1 (i.e. self) is expected to be a PIXCMAP* user data (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 lua_Integer on the Lua stack
 */
static int
Clear(lua_State *L)
{
    PIXCMAP *cmap = ll_check_PIXCMAP(L, 1);
    lua_pushboolean(L, 0 == pixcmapClear(cmap));
    return 1;
}

/**
 * \brief Register the PIXCMAP methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PIXCMAP(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},    /* new PIXCMAP */
        {"__len",               GetCount},  /* #cmap */
        {"Copy",                Copy},
        {"Destroy",             Destroy},
        {"AddColor",            AddColor},
        {"AddRGBA",             AddRGBA},
        {"AddNewColor",         AddNewColor},
        {"AddNearestColor",     AddNearestColor},
        {"UsableColor",         UsableColor},
        {"AddBlackOrWhite",     AddBlackOrWhite},
        {"SetBlackAndWhite",    SetBlackAndWhite},
        {"GetCount",            GetCount},  /* same as #cmap */
        {"GetDepth",            GetDepth},
        {"GetMinDepth",         GetMinDepth},
        {"GetFreeCount",        GetFreeCount},
        {"Clear",               Clear},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        {"CreateRandom",        CreateRandom},
        {"CreateLinear",        CreateLinear},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_PIXCMAP, methods, functions);
    lua_setglobal(L, LL_PIXCMAP);
    return res;
}
