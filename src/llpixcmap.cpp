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

/**
 * \file llpixcmap.cpp
 * \class PixColormap
 *
 * A class to handle a Pix color map ("palette").
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXCMAP

/** Define a function's name (_fun) with prefix PixColormap */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixColormap *cmap = ll_take_udata<PixColormap>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "cmap", reinterpret_cast<void *>(cmap),
        "count", pixcmapGetCount(cmap));
    pixcmapDestroy(&cmap);
    return 0;
}

/**
 * \brief Create a new PixColormap*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    PixColormap *cmap = pixcmapCreate(depth);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Get count of colors in a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 count = pixcmapGetCount(cmap);
    ll_push_l_int32(_fun, L, count);
    return 1;
}

/**
 * \brief Printable string for a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    luaL_Buffer B;
    luaL_buffinit(L, &B);
    l_int32 i, r, g, b, a;

    if (!cmap) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(cmap));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "depth", cmap->depth);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nalloc", cmap->nalloc);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "n", cmap->n);
        luaL_addstring(&B, str);
        for (i = 0; i < pixcmapGetCount(cmap); i++) {
            pixcmapGetRGBA(cmap, i, &r, &g, &b, &a);
            snprintf(str, LL_STRBUFF, "\n    %-3d = { r = %-3d, g = %-3d, b = %-3d, a = %-3d }",
                     i + 1, r, g, b, a);
            luaL_addstring(&B, str);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add black or white to a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (color).
 *
 * Leptonica's Notes:
 *      (1) This only adds color if not already there.
 *      (2) The alpha component is 255 (opaque)
 *      (3) This sets index to the requested color.
 *      (4) If there is no room in the colormap, returns the index
 *          of the closest color.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
AddBlackOrWhite(lua_State *L)
{
    LL_FUNC("AddBlackOrWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 color = ll_check_set_black_white(_fun, L, 2, L_SET_BLACK);
    l_int32 idx = 0;
    if (pixcmapAddBlackOrWhite(cmap, color, &idx))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add a color to a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 *
 * Leptonica's Notes:
 *      (1) This always adds the color if there is room.
 *      (2) The alpha component is 255 (opaque)
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddColor(lua_State *L)
{
    LL_FUNC("AddColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = ll_check_l_int32(_fun, L, 2);
    l_int32 gval = ll_check_l_int32(_fun, L, 3);
    l_int32 bval = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixcmapAddColor(cmap, rval, gval, bval));
}

/**
 * \brief Add a nearest color to a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 *
 * Leptonica's Notes:
 *      (1) This only adds color if not already there.
 *      (2) The alpha component is 255 (opaque)
 *      (3) If it's not in the colormap and there is no room to add
 *          another color, this returns the index of the nearest color.
 * </pre>
 * \param L Lua state.
 * \return 1 lua_Integer on the Lua stack.
 */
static int
AddNearestColor(lua_State *L)
{
    LL_FUNC("AddNearestColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 idx = 0;
    ll_check_color(_fun, L, 2, &rval, &gval, &bval);
    if (pixcmapAddNearestColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add a new color to a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 *
 * Leptonica's Notes:
 *      (1) This only adds color if not already there.
 *      (2) The alpha component is 255 (opaque)
 *      (3) This returns the index of the new (or existing) color.
 *      (4) Returns 2 with a warning if unable to add this color;
 *          the caller should check the return value.
 * </pre>
 * \param L Lua state.
 * \return 1 lua_Integer on the Lua stack.
 */
static int
AddNewColor(lua_State *L)
{
    LL_FUNC("AddNewColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 idx = 0;
    ll_check_color(_fun, L, 2, &rval, &gval, &bval);
    if (pixcmapAddNewColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Add a RGBA color to a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * Arg #5 is expected to be a l_int32 (aval).
 *
 * Leptonica's Notes:
 *      (1) This always adds the color if there is room.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddRGBA(lua_State *L)
{
    LL_FUNC("RGBA");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    ll_check_color(_fun, L, 2, &rval, &gval, &bval, &aval);
    return ll_push_boolean(_fun, L, 0 == pixcmapAddRGBA(cmap, rval, gval, bval, aval));
}

/**
 * \brief Clear the colors of a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 *
 * Leptonica's Notes:
 *      (1) This removes the colors by setting the count to 0.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixcmapClear(cmap));
}

/**
 * \brief Convert a PixColormap* (%cmap) to a string of hexadecimal numbers in angle brackets.
 * <pre>
 * Arg #1 is expected to be a string (data).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 *
 * Leptonica's Notes:
 *      (1) The number of bytes in %data is 3 * ncolors.
 *      (2) Output is in form:
 *             < r0g0b0 r1g1b1 ... rngnbn >
 *          where r0, g0, b0 ... are each 2 bytes of hex ascii
 *      (3) This is used in pdf files to express the colormap as an
 *          array in ascii (human-readable) format.
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
ConvertToHex(lua_State *L)
{
    LL_FUNC("ConvertToHex");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = 0;
    l_uint8 *data = nullptr;
    if (!pixcmapSerializeToMemory(cmap, 3, &ncolors, &data))
        return 0;
    char *hex = pixcmapConvertToHex(data, ncolors);
    lua_pushstring(L, hex);
    ll_free(data);
    ll_free(hex);
    return 1;
}

/**
 * \brief Copy a PixColormap* (%cmaps).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    PixColormap *cmaps = ll_check_PixColormap(_fun, L, 1);
    PixColormap *cmap = pixcmapCopy(cmaps);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Count gray colors of a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 *
 * Leptonica's Notes:
 *      (1) This counts the unique gray colors, including black and white.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
CountGrayColors(lua_State *L)
{
    LL_FUNC("CountGrayColors");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ngray = 0;
    if (pixcmapCountGrayColors(cmap, &ngray))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, ngray);
    return 1;
}

/**
 * \brief Create a linear PixColormap* (%cmap).
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * Arg #2 is expected to be a l_int32 (levels).
 *
 * Leptonica's Notes:
 *      (1) Colormap has equally spaced gray color values
 *          from black (0, 0, 0) to white (255, 255, 255).
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
CreateLinear(lua_State *L)
{
    LL_FUNC("CreateLinear");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    l_int32 levels = ll_check_l_int32(_fun, L, 1);
    PixColormap *cmap = pixcmapCreateLinear(depth, levels);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Create a random PixColormap* (%cmap).
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * Arg #2 is an optional boolean (hasblack).
 * Arg #3 is an optional boolean (haswhite).
 *
 * Leptonica's Notes:
 *      (1) This sets up a colormap with random colors,
 *          where the first color is optionally black, the last color
 *          is optionally white, and the remaining colors are
 *          chosen randomly.
 *      (2) The number of randomly chosen colors is:
 *               2^(depth) - haswhite - hasblack
 *      (3) Because rand() is seeded, it might disrupt otherwise
 *          deterministic results if also used elsewhere in a program.
 *      (4) rand() is not threadsafe, and will generate garbage if run
 *          on multiple threads at once -- though garbage is generally
 *          what you want from a random number generator!
 *      (5) Modern rand()s have equal randomness in low and high order
 *          bits, but older ones don't.  Here, we're just using rand()
 *          to choose colors for output.
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
CreateRandom(lua_State *L)
{
    LL_FUNC("CreateRandom");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    l_int32 hasblack = ll_opt_boolean(_fun, L, 2, FALSE);
    l_int32 haswhite = ll_opt_boolean(_fun, L, 3, FALSE);
    PixColormap *cmap = pixcmapCreateRandom(depth, hasblack, haswhite);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Deserialize a PixColormap* (%cmap) from a Lua string (data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 * </pre>
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
static int
DeserializeFromMemory(lua_State *L)
{
    LL_FUNC("DeserializeFromMemory");
    size_t len = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &len);
    l_int32 cpc = ll_opt_l_int32(_fun, L, 2, 4);
    l_int32 ncolors = static_cast<l_int32>(len / static_cast<size_t>(cpc));
    l_uint8 *data = ll_malloc<l_uint8>(_fun, L, len);
    PixColormap* cmap = nullptr;
    memcpy(data, str, len);
    cmap = pixcmapDeserializeFromMemory(data, cpc, ncolors);
    ll_free(data);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Get a color from a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 3 integers on the Lua stack.
 */
static int
GetColor(lua_State *L)
{
    LL_FUNC("GetColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    if (pixcmapGetColor(cmap, idx, &rval, &gval, &bval))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, rval);
    ll_push_l_int32(_fun, L, gval);
    ll_push_l_int32(_fun, L, bval);
    return 3;
}

/**
 * \brief Get a color l_uint32 from a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) The returned alpha channel value is 255.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetColor32(lua_State *L)
{
    LL_FUNC("GetColor32");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_uint32 val32 = 0;
    if (pixcmapGetColor32(cmap, idx, &val32))
        return ll_push_nil(_fun, L);
    ll_push_l_uint32(_fun, L, val32);
    return 1;
}

/**
 * \brief Get the depth of a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetDepth(lua_State *L)
{
    LL_FUNC("GetDepth");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 depth = pixcmapGetDepth(cmap);
    ll_push_l_int32(_fun, L, depth);
    return 1;
}

/**
 * \brief Get the count of free colors of a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetFreeCount(lua_State *L)
{
    LL_FUNC("GetFreeCount");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 freecount = pixcmapGetFreeCount(cmap);
    ll_push_l_int32(_fun, L, freecount);
    return 1;
}

/**
 * \brief Get index (%idx) for color from a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetIndex(lua_State *L)
{
    LL_FUNC("GetIndex");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 idx = 0;
    ll_check_color(_fun, L, 2, &rval, &gval, &bval);
    if (pixcmapGetIndex(cmap, rval, gval, bval, &idx))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Get the minimum depth of a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 *
 * Leptonica's Notes:
 *      (1) On error, &mindepth is returned as 0.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetMinDepth(lua_State *L)
{
    LL_FUNC("GetMinDepth");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 mindepth = 0;
    if (pixcmapGetMinDepth(cmap, &mindepth))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, mindepth);
    return 1;
}

/**
 * \brief Get a RGBA from a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 4 integers on the Lua stack.
 */
static int
GetRGBA(lua_State *L)
{
    LL_FUNC("GetRGBA");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 aval = 0;
    if (pixcmapGetRGBA(cmap, idx, &rval, &gval, &bval, &aval))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, rval);
    ll_push_l_int32(_fun, L, gval);
    ll_push_l_int32(_fun, L, bval);
    ll_push_l_int32(_fun, L, aval);
    return 4;
}

/**
 * \brief Get a RGBA l_uint32 from a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetRGBA32(lua_State *L)
{
    LL_FUNC("GetRGBA32");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixcmapGetCount(cmap));
    l_uint32 val32 = 0;
    if (pixcmapGetRGBA32(cmap, idx, &val32))
        return ll_push_nil(_fun, L);
    ll_push_l_uint32(_fun, L, val32);
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) has color.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
HasColor(lua_State *L)
{
    LL_FUNC("HasColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 color = 0;
    if (pixcmapHasColor(cmap, &color))
        return ll_push_nil(_fun, L);
    lua_pushboolean(L, color);
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) is black and white.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
IsBlackAndWhite(lua_State *L)
{
    LL_FUNC("IsBlackAndWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 blackandwhite = 0;
    if (pixcmapIsBlackAndWhite(cmap, &blackandwhite))
        return ll_push_nil(_fun, L);
    lua_pushboolean(L, blackandwhite);
    return 1;
}

/**
 * \brief Check if a PixColormap* (%cmap) is opaque.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
IsOpaque(lua_State *L)
{
    LL_FUNC("IsOpaque");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 opaque = 0;
    if (pixcmapIsOpaque(cmap, &opaque))
        return ll_push_nil(_fun, L);
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Read a PixColormap* (%cmap) from a file.
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    PixColormap *cmap = pixcmapRead(filename);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Read a PixColormap* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    PixColormap *cmap = pixcmapReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Read a PixColormap* (%cmap) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    PixColormap *cmap = pixcmapReadStream(stream->f);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Reset a color for index (%idx) in a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (rval).
 * Arg #4 is expected to be a l_int32 (gval).
 * Arg #5 is expected to be a l_int32 (bval).
 *
 * Leptonica's Notes:
 *      (1) This resets sets the color of an entry that has already
 *          been set and included in the count of colors.
 *      (2) The alpha component is 255 (opaque)
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ResetColor(lua_State *L)
{
    LL_FUNC("ResetColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, 1 << pixcmapGetDepth(cmap));
    l_int32 rval = ll_check_l_int32(_fun, L, 3);
    l_int32 gval = ll_check_l_int32(_fun, L, 4);
    l_int32 bval = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixcmapResetColor(cmap, idx, rval, gval, bval));
}

/**
 * \brief Serialize a PixColormap* (%cmap) to a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 *
 * Leptonica's Notes:
 *      (1) When serializing to store in a pdf, use %cpc = 3.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
SerializeToMemory(lua_State *L)
{
    LL_FUNC("SerializeToMemory");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 cpc = ll_opt_l_int32(_fun, L, 2, 4);
    l_int32 ncolors = 0;
    l_uint8 *data = nullptr;
    if (pixcmapSerializeToMemory(cmap, cpc, &ncolors, &data))
        return ll_push_nil(_fun, L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), static_cast<size_t>(cpc) * ncolors);
    ll_free(data);
    return 1;
}

/**
 * \brief Set alpha channel for an index in a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (aval).
 *
 * Leptonica's Notes:
 *      (1) This modifies the transparency of one entry in a colormap.
 *          The alpha component by default is 255 (opaque).
 *          This is used when extracting the colormap from a PNG file
 *          without decoding the image.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetAlpha(lua_State *L)
{
    LL_FUNC("SetAlpha");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, 1 << pixcmapGetDepth(cmap));
    l_int32 aval = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixcmapSetAlpha(cmap, idx, aval));
}

/**
 * \brief Set black and white to a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a boolean (setblack).
 * Arg #3 is expected to be a boolean (setwhite).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetBlackAndWhite(lua_State *L)
{
    LL_FUNC("SetBlackAndWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 setblack = ll_opt_boolean(_fun, L, 2, FALSE);
    l_int32 setwhite = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == pixcmapSetBlackAndWhite(cmap, setblack, setwhite));
}

/**
 * \brief Return a PixColormap* (%cmap) as a 4 Lua array tables (%rmap,%gmap,%bmap,%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 4 array tables on the Lua stack.
 */
static int
ToArrays(lua_State *L)
{
    LL_FUNC("ToArrays");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = pixcmapGetCount(cmap);
    l_int32 *rmap = nullptr;
    l_int32 *gmap = nullptr;
    l_int32 *bmap = nullptr;
    l_int32 *amap = nullptr;
    if (pixcmapToArrays(cmap, &rmap, &gmap, &bmap, &amap))
        return ll_push_nil(_fun, L);
    ll_pack_Iarray(_fun, L, rmap, ncolors);
    ll_pack_Iarray(_fun, L, gmap, ncolors);
    ll_pack_Iarray(_fun, L, bmap, ncolors);
    ll_pack_Iarray(_fun, L, amap, ncolors);
    ll_free(rmap);
    ll_free(gmap);
    ll_free(bmap);
    ll_free(amap);
    return 4;
}

/**
 * \brief Return a PixColormap* (%cmap) as a Lua array table.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L Lua state.
 * \return 1 table array on the Lua stack.
 */
static int
ToRGBTable(lua_State *L)
{
    LL_FUNC("ToRGBTable");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = 0;
    l_uint32 *table = nullptr;
    if (pixcmapToRGBTable(cmap, &table, &ncolors))
        return ll_push_nil(_fun, L);
    ll_pack_Uarray(_fun, L, table, ncolors);
    ll_free(table);
    return 1;
}

/**
 * \brief Get usable color from a PixColormap* (%cmap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 *
 * Leptonica's Notes:
 *      (1) This checks if the color already exists or if there is
 *          room to add it.  It makes no change in the colormap.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
UsableColor(lua_State *L)
{
    LL_FUNC("UsableColor");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 idx = 0;
    ll_check_color(_fun, L, 2, &rval, &gval, &bval);
    if (pixcmapUsableColor(cmap, rval, gval, bval, &idx))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, idx + 1);  /* Lua index is 1-based */
    return 1;
}

/**
 * \brief Write a PixColormap* (%cmap) to a file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    if (pixcmapWrite(filename, cmap))
        return ll_push_nil(_fun, L);
    lua_pushboolean(L, TRUE);
    return 1;
}

/**
 * \brief Write the PixColormap* (%cmap) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 *
 * Leptonica's Notes:
 *      (1) Serializes a pixcmap in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixcmapWriteMem(&data, &size, cmap))
        return ll_push_nil(_fun, L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write a PixColormap* (%cmap) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    if (pixcmapWriteStream(stream->f, cmap))
        return ll_push_nil(_fun, L);
    lua_pushboolean(L, TRUE);
    return 1;
}

/**
 * \brief AddColorizedGrayToCmap() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (rval).
 * Arg #4 is expected to be a l_int32 (gval).
 * Arg #5 is expected to be a l_int32 (bval).
 *
 * Notes:
 *      (1) If type == L_PAINT_LIGHT, it colorizes non-black pixels,
 *          preserving antialiasing.
 *          If type == L_PAINT_DARK, it colorizes non-white pixels,
 *          preserving antialiasing.
 *      (2) This increases the colormap size by the number of
 *          different gray (non-black or non-white) colors in the
 *          input colormap.  If there is not enough room in the colormap
 *          for this expansion, it returns 1 (treated as a warning);
 *          the caller should check the return value.
 *      (3) This can be used to determine if the new colors will fit in
 *          the cmap, using null for &na.  Returns 0 if they fit; 2 if
 *          they don't fit.
 *      (4) The mapping table contains, for each gray color found, the
 *          index of the corresponding colorized pixel.  Non-gray
 *          pixels are assigned the invalid index 256.
 *      (5) See pixColorGrayCmap() for usage.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
AddColorizedGrayToCmap(lua_State *L)
{
    LL_FUNC("AddColorizedGrayToCmap");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 type = ll_check_paint_flags(_fun, L, 2, L_PAINT_LIGHT);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    Numa *na = nullptr;
    ll_check_color(_fun, L, 3, &rval, &gval, &bval);
    if (addColorizedGrayToCmap(cmap, type, rval, gval, bval, &na))
        return ll_push_nil(_fun, L);
    ll_push_Numa(_fun, L, na);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for user data of class PixColormap*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data.
 */
PixColormap *
ll_check_PixColormap(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixColormap>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a PixColormap* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data.
 */
PixColormap *
ll_opt_PixColormap(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_PixColormap(_fun, L, arg);
}

/**
 * \brief Push PixColormap* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cmap pointer to the PIXCMAP
 * \return 1 PixColormap* on the Lua stack.
 */
int
ll_push_PixColormap(const char *_fun, lua_State *L, PixColormap *cmap)
{
    if (!cmap)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cmap);
}

/**
 * \brief Create a new PixColormap*.
 * \param L Lua state.
 * \return 1 PixColormap* on the Lua stack.
 */
int
ll_new_PixColormap(lua_State *L)
{
    FUNC("ll_new_PixColormap");
    PixColormap *cmap = nullptr;
    PixColormap *cmaps = nullptr;
    luaL_Stream* stream = nullptr;
    const char *filename = nullptr;
    const l_uint8 *data = nullptr;
    size_t size = 0;
    l_int32 depth = 1;

    if (ll_isudata(_fun, L, 1, LL_PIXCMAP)) {
        cmaps = ll_opt_PixColormap(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIXCMAP, reinterpret_cast<void *>(cmaps));
        cmap = pixcmapCopy(cmaps);
    }

    if (!cmap && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        cmap = pixcmapReadStream(stream->f);
    }

    if (!cmap && ll_isinteger(_fun, L, 1)) {
        depth = ll_opt_l_int32(_fun, L, 1, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "depth", depth);
        cmap = pixcmapCreate(depth);
    }

    if (!cmap && ll_isstring(_fun, L, 1)) {
        filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        cmap = pixcmapRead(filename);
    }

    if (!cmap && ll_isstring(_fun, L, 1)) {
        data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        cmap = pixcmapReadMem(data, size);
    }

    if (!cmap) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "depth", depth);
        cmap = pixcmapCreate(depth);
    }

    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Register the PIXCMAP methods and functions in the LL_PIX meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_PixColormap(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_PixColormap},    /* new PixColormap */
        {"__len",                   GetCount},              /* #cmap */
        {"__tostring",              toString},
        {"AddBlackOrWhite",         AddBlackOrWhite},
        {"AddColor",                AddColor},
        {"AddColorizedGrayToCmap",  AddColorizedGrayToCmap},
        {"AddNearestColor",         AddNearestColor},
        {"AddNewColor",             AddNewColor},
        {"AddRGBA",                 AddRGBA},
        {"Clear",                   Clear},
        {"ConvertToHex",            ConvertToHex},
        {"Copy",                    Copy},
        {"CountGrayColors",         CountGrayColors},
        {"Create",                  Create},
        {"CreateLinear",            CreateLinear},
        {"CreateRandom",            CreateRandom},
        {"DeserializeFromMemory",   DeserializeFromMemory},
        {"Destroy",                 Destroy},
        {"GetColor",                GetColor},
        {"GetColor32",              GetColor32},
        {"GetCount",                GetCount},
        {"GetDepth",                GetDepth},
        {"GetFreeCount",            GetFreeCount},
        {"GetIndex",                GetIndex},
        {"GetMinDepth",             GetMinDepth},
        {"GetRGBA",                 GetRGBA},
        {"GetRGBA32",               GetRGBA32},
        {"HasColor",                HasColor},
        {"IsBlackAndWhite",         IsBlackAndWhite},
        {"IsOpaque",                IsOpaque},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"ResetColor",              ResetColor},
        {"SerializeToMemory",       SerializeToMemory},
        {"SetAlpha",                SetAlpha},
        {"SetBlackAndWhite",        SetBlackAndWhite},
        {"ToArrays",                ToArrays},
        {"ToRGBTable",              ToRGBTable},
        {"UsableColor",             UsableColor},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_PixColormap);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
