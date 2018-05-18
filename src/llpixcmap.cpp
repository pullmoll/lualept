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
 *  Lua class PIXCMAP
 *
 *====================================================================*/

/**
 * \brief Copy a PixColormap* (%cmaps)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_PIXCMAP ".toString");
    static char str[256];
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    luaL_Buffer B;
    luaL_buffinit(L, &B);
    l_int32 i, r, g, b, a;

    if (!cmap) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        for (i = 0; i < pixcmapGetCount(cmap); i++) {
            pixcmapGetRGBA(cmap, i, &r, &g, &b, &a);
            snprintf(str, sizeof(str), "{#%02x,#%02x,#%02x,#%02x}",
                     r, g, b, a);
            luaL_addstring(&B, str);
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new PixColormap*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Create");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    PixColormap *cmap = pixcmapCreate(depth);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Create a random PixColormap* (%cmap)
 *
 * Arg #1 is expected to be a l_int32 (depth)
 * Arg #2 is optional and, if specified, expected to be a boolean (hasblack)
 * Arg #3 is optional and, if specified, expected to be a boolean (haswhite)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
CreateRandom(lua_State *L)
{
    FUNC(LL_PIXCMAP ".CreateRandom");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    l_int32 hasblack = lua_toboolean(L, 2);
    l_int32 haswhite = lua_toboolean(L, 3);
    PixColormap *cmap = pixcmapCreateRandom(depth, hasblack, haswhite);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Create a linear PixColormap* (%cmap)
 *
 * Arg #1 is expected to be a l_int32 (depth)
 * Arg #2 is expected to be a l_int32 (levels)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
CreateLinear(lua_State *L)
{
    FUNC(LL_PIXCMAP ".CreateLinear");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    l_int32 levels = ll_check_l_int32(_fun, L, 1);
    PixColormap *cmap = pixcmapCreateLinear(depth, levels);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Destroy a PixColormap*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Destroy");
    PixColormap **pcmap = reinterpret_cast<PixColormap **>(ll_check_udata(_fun, L, 1, LL_PIXCMAP));
    DBG(LOG_DESTROY, "%s: '%s' pcmap=%p cmap=%p\n",
        _fun, LL_PIXCMAP, pcmap, *pcmap);
    pixcmapDestroy(pcmap);
    *pcmap = nullptr;
    return 0;
}

/**
 * \brief Copy a PixColormap* (%cmaps)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Copy");
    PixColormap *cmaps = ll_check_PixColormap(_fun, L, 1);
    PixColormap *cmap = pixcmapCopy(cmaps);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Add a color to a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
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
    FUNC(LL_PIXCMAP ".AddColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    lua_pushboolean(L, 0 == pixcmapAddColor(cmap, rval, gval, bval));
    return 1;
}

/**
 * \brief Add a RGBA color to a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
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
    FUNC(LL_PIXCMAP ".RGBA");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    l_int32 aval = ll_check_l_int32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixcmapAddRGBA(cmap, rval, gval, bval, aval));
    return 1;
}

/**
 * \brief Add a new color to a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
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
    FUNC(LL_PIXCMAP ".AddNewColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    l_int32 idx = 0;
    if (pixcmapAddNewColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(L);
    lua_pushinteger(L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add a nearest color to a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
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
    FUNC(LL_PIXCMAP ".AddNearestColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    l_int32 idx = 0;
    if (pixcmapAddNearestColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(L);
    lua_pushinteger(L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Get usable color from a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
UsableColor(lua_State *L)
{
    FUNC(LL_PIXCMAP ".UsableColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    l_int32 idx = 0;
    if (pixcmapUsableColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(L);
    lua_pushinteger(L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add black or white to a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (color)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
AddBlackOrWhite(lua_State *L)
{
    FUNC(LL_PIXCMAP ".AddBlackOrWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 color = ll_check_blackwhite(_fun, L, 2, L_SET_BLACK);
    l_int32 idx = 0;
    if (pixcmapAddBlackOrWhite(cmap, color, &idx))
        return ll_push_nil(L);
    lua_pushinteger(L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Set black and white to a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a boolean (setblack)
 * Arg #3 is expected to be a boolean (setwhite)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBlackAndWhite(lua_State *L)
{
    FUNC(LL_PIXCMAP ".SetBlackAndWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 setblack = lua_toboolean(L, 2);
    l_int32 setwhite = lua_toboolean(L, 3);
    lua_pushboolean(L, 0 == pixcmapSetBlackAndWhite(cmap, setblack, setwhite));
    return 1;
}

/**
 * \brief Get count of colors in a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetCount");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 count = pixcmapGetCount(cmap);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the depth of a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDepth(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetDepth");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 depth = pixcmapGetDepth(cmap);
    lua_pushinteger(L, depth);
    return 1;
}

/**
 * \brief Get the minimum depth of a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetMinDepth(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetMinDepth");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 mindepth = 0;
    if (pixcmapGetMinDepth(cmap, &mindepth))
        return ll_push_nil(L);
    lua_pushinteger(L, mindepth);
    return 1;
}

/**
 * \brief Get the count of free colors of a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetFreeCount(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetFreeCount");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 freecount = pixcmapGetFreeCount(cmap);
    lua_pushinteger(L, freecount);
    return 1;
}

/**
 * \brief Clear the colors of a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
Clear(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Clear");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    lua_pushboolean(L, 0 == pixcmapClear(cmap));
    return 1;
}

/**
 * \brief Get a color from a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack
 */
static int
GetColor(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    if (pixcmapGetColor(cmap, idx, &rval, &gval, &bval))
        return ll_push_nil(L);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    return 3;
}

/**
 * \brief Get a color l_uint32 from a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColor32(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetColor32");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_uint32 val32 = 0;
    if (pixcmapGetColor32(cmap, idx, &val32))
        return ll_push_nil(L);
    lua_pushinteger(L, val32);
    return 1;
}

/**
 * \brief Get a RGBA from a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack
 */
static int
GetRGBA(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetRGBA");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    if (pixcmapGetRGBA(cmap, idx, &rval, &gval, &bval, &aval))
        return ll_push_nil(L);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    lua_pushinteger(L, aval);
    return 4;
}

/**
 * \brief Get a RGBA l_uint32 from a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetRGBA32(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetRGBA32");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_uint32 val32 = 0;
    if (pixcmapGetRGBA32(cmap, idx, &val32))
        return ll_push_nil(L);
    lua_pushinteger(L, val32);
    return 1;
}

/**
 * \brief Reset a color for index %idx in a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a l_int32 (rval)
 * Arg #4 is expected to be a l_int32 (gval)
 * Arg #5 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ResetColor(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ResetColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, 1 << pixcmapGetDepth(cmap));
    l_int32 rval = ll_check_l_int32(_fun, L, 3);
    l_int32 gval = ll_check_l_int32(_fun, L, 4);
    l_int32 bval = ll_check_l_int32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixcmapResetColor(cmap, idx, rval, gval, bval));
    return 1;
}

/**
 * \brief Set alpha channel for an index in a PixColormap*
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a l_int32 (aval)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetAlpha(lua_State *L)
{
    FUNC(LL_PIXCMAP ".SetAlpha");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, 1 << pixcmapGetDepth(cmap));
    l_int32 aval = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == pixcmapSetAlpha(cmap, idx, aval));
    return 1;
}

/**
 * \brief Get index (%idx) for color from a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a l_int32 (rval)
 * Arg #3 is expected to be a l_int32 (gval)
 * Arg #4 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetIndex(lua_State *L)
{
    FUNC(LL_PIXCMAP ".GetIndex");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    l_int32 idx = 0;
    if (pixcmapGetIndex(cmap, rval, gval, bval, &idx))
        return ll_push_nil(L);
    lua_pushinteger(L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) has color
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
HasColor(lua_State *L)
{
    FUNC(LL_PIXCMAP ".HasColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 color = 0;
    if (pixcmapHasColor(cmap, &color))
        return ll_push_nil(L);
    lua_pushboolean(L, color);
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) is opaque
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
IsOpaque(lua_State *L)
{
    FUNC(LL_PIXCMAP ".IsOpaque");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 opaque = 0;
    if (pixcmapIsOpaque(cmap, &opaque))
        return ll_push_nil(L);
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) is black and white
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
IsBlackAndWhite(lua_State *L)
{
    FUNC(LL_PIXCMAP ".IsBlackAndWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 blackandwhite = 0;
    if (pixcmapIsBlackAndWhite(cmap, &blackandwhite))
        return ll_push_nil(L);
    lua_pushboolean(L, blackandwhite);
    return 1;
}

/**
 * \brief Count gray colors of a PixColormap* (%cmap)
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountGrayColors(lua_State *L)
{
    FUNC(LL_PIXCMAP ".CountGrayColors");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ngray = 0;
    if (pixcmapCountGrayColors(cmap, &ngray))
        return ll_push_nil(L);
    lua_pushinteger(L, ngray);
    return 1;
}

/**
 * \brief Read a PixColormap* (%cmap) from a file
 *
 * Arg #1 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Read(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Read");
    const char *filename = ll_check_string(_fun, L, 1);
    PixColormap *cmap = pixcmapRead(filename);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Write a PixColormap* (%cmap) to a file
 *
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap)
 * Arg #2 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Write");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    if (pixcmapWrite(filename, cmap))
        return ll_push_nil(L);
    lua_pushboolean(L, TRUE);
    return 1;
}

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXCMAP
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data
 */
PixColormap *
ll_check_PixColormap(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<PixColormap **>(ll_check_udata(_fun, L, arg, LL_PIXCMAP)));
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
 * \return pointer to the PixColormap* contained in the user data
 */
PixColormap *
ll_take_PixColormap(lua_State *L, int arg)
{
    FUNC("ll_take_PixColormap");
    PixColormap **pcmap = (PixColormap **)ll_check_udata(_fun, L, arg, LL_PIXCMAP);
    PixColormap *cmap = *pcmap;
    *pcmap = nullptr;
    return cmap;
}

/**
 * \brief Push PixColormap* user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cmap pointer to the PIXCMAP
 * \return 1 PixColormap* on the Lua stack
 */
int
ll_push_PixColormap(const char *_fun, lua_State *L, PixColormap *cmap)
{
    if (!cmap)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PIXCMAP, cmap);
}

/**
 * \brief Create a new PixColormap*
 *
 * Arg #1 is expected to be a l_int32 (depth)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
int
ll_new_PixColormap(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the PIXCMAP methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PixColormap(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},    /* new PixColormap */
        {"__len",               GetCount},  /* #cmap */
        {"__tostring",          toString},
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
        {"GetColor",            GetColor},
        {"GetColor32",          GetColor32},
        {"GetRGBA",             GetRGBA},
        {"GetRGBA32",           GetRGBA32},
        {"ResetColor",          ResetColor},
        {"SetAlpha",            SetAlpha},
        {"GetIndex",            GetIndex},
        {"HasColor",            HasColor},
        {"IsOpaque",            IsOpaque},
        {"IsBlackAndWhite",     IsBlackAndWhite},
        {"CountGrayColors",     CountGrayColors},
        {"Write",               Write},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        {"CreateRandom",        CreateRandom},
        {"CreateLinear",        CreateLinear},
        {"Read",                Read},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_PIXCMAP, methods, functions);
    lua_setglobal(L, LL_PIXCMAP);
    return res;
}
