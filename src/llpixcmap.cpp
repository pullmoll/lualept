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
 *  Lua class PixColormap
 *
 *====================================================================*/


/**
 * \brief Destroy a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_PIXCMAP ".Destroy");
    PixColormap **pcmap = ll_check_udata<PixColormap>(_fun, L, 1, LL_PIXCMAP);
    PixColormap *cmap = *pcmap;
    DBG(LOG_DESTROY, "%s: '%s' pcmap=%p cmap=%p count=%d\n",
        _fun, LL_PIXCMAP, pcmap, cmap, pixcmapGetCount(cmap));
    pixcmapDestroy(&cmap);
    *pcmap = nullptr;
    return 0;
}

/**
 * \brief Create a new PixColormap*
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * </pre>
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
 * \brief Get count of colors in a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Printable string for a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
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
        snprintf(str, sizeof(str),
                 LL_PIXCMAP ": %p",
                 reinterpret_cast<void *>(cmap));
        luaL_addstring(&B, str);
        for (i = 0; i < pixcmapGetCount(cmap); i++) {
            pixcmapGetRGBA(cmap, i, &r, &g, &b, &a);
            snprintf(str, sizeof(str), "\n    %d = { r = %d, g = %d, b = %d, a = %d }",
                     i + 1, r, g, b, a);
            luaL_addstring(&B, str);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Add black or white to a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (color).
 * </pre>
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
 * \brief Add a color to a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Add a nearest color to a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Add a new color to a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Add a RGBA color to a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * Arg #5 is expected to be a l_int32 (aval).
 * </pre>
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
 * \brief Clear the colors of a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Convert a PixColormap* (%cmap) to a string of hexadecimal numbers in angle brackets
 * <pre>
 * Arg #1 is expected to be a string (data).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
ConvertToHex(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ConvertToHex");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = 0;
    l_uint8 *data = nullptr;
    if (!pixcmapSerializeToMemory(cmap, 3, &ncolors, &data))
        return 0;
    char *hex = pixcmapConvertToHex(data, ncolors);
    lua_pushstring(L, hex);
    LEPT_FREE(hex);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Copy a PixColormap* (%cmaps)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
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
 * \brief Count gray colors of a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Deserialize a PixColormap* (%cmap) from a Lua string (data)
 * <pre>
 * Arg #1 is expected to be a string (data).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
DeserializeFromMemory(lua_State *L)
{
    FUNC(LL_PIXCMAP ".DeserializeFromMemory");
    size_t len = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &len);
    l_int32 cpc = ll_check_l_int32_default(_fun, L, 2, 4);
    l_int32 ncolors = static_cast<l_int32>(len / static_cast<size_t>(cpc));
    l_uint8 *data = ll_malloc<l_uint8>(_fun, L, len);
    PixColormap* cmap = nullptr;
    memcpy(data, str, len);
    cmap = pixcmapDeserializeFromMemory(data, cpc, ncolors);
    ll_free(data);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Get a color from a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
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
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
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
 * \brief Get the depth of a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Get the count of free colors of a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Get index (%idx) for color from a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Get the minimum depth of a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Get a RGBA from a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
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
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
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
 * \brief Check if a PixColormap* (%cmap) has color
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Check if a PixColormap* (%cmap) is black and white
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Check if a PixColormap* (%cmap) is opaque
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
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
 * \brief Read a PixColormap* (%cmap) from a file
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
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
 * \brief Read a PixColormap* from a Lua string (%data)
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    PixColormap *cmap = pixcmapReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Read a PixColormap* (%cmap) from a Lua io stream (%stream)
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    PixColormap *cmap = pixcmapReadStream(stream->f);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Reset a color for index (%idx) in a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (rval).
 * Arg #4 is expected to be a l_int32 (gval).
 * Arg #5 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Serialize a PixColormap* (%cmap) to a Lua string
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (cpc; 0 < cpc <= 4).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
SerializeToMemory(lua_State *L)
{
    FUNC(LL_PIXCMAP ".SerializeToMemory");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 cpc = ll_check_l_int32_default(_fun, L, 2, 4);
    l_int32 ncolors = 0;
    l_uint8 *data = nullptr;
    if (pixcmapSerializeToMemory(cmap, cpc, &ncolors, &data))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), static_cast<size_t>(cpc) * ncolors);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Set alpha channel for an index in a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (aval).
 * </pre>
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
 * \brief Set black and white to a PixColormap*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a boolean (setblack).
 * Arg #3 is expected to be a boolean (setwhite).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBlackAndWhite(lua_State *L)
{
    FUNC(LL_PIXCMAP ".SetBlackAndWhite");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 setblack = ll_check_boolean_default(_fun, L, 2, FALSE);
    l_int32 setwhite = ll_check_boolean_default(_fun, L, 3, FALSE);
    lua_pushboolean(L, 0 == pixcmapSetBlackAndWhite(cmap, setblack, setwhite));
    return 1;
}

/**
 * \brief Return a PixColormap* (%cmap) as a 4 Lua array tables (%rmap,%gmap,%bmap,%amap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 array tables on the Lua stack
 */
static int
ToArrays(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ToArrays");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = pixcmapGetCount(cmap);
    l_int32 *rmap = nullptr;
    l_int32 *gmap = nullptr;
    l_int32 *bmap = nullptr;
    l_int32 *amap = nullptr;
    if (pixcmapToArrays(cmap, &rmap, &gmap, &bmap, &amap))
        return ll_push_nil(L);
    ll_push_iarray(L, rmap, ncolors);
    ll_push_iarray(L, gmap, ncolors);
    ll_push_iarray(L, bmap, ncolors);
    ll_push_iarray(L, amap, ncolors);
    LEPT_FREE(rmap);
    LEPT_FREE(gmap);
    LEPT_FREE(bmap);
    LEPT_FREE(amap);
    return 4;
}

/**
 * \brief Return a PixColormap* (%cmap) as a Lua array table
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 *
 * FIXME: Pushing the table of l_uint32 with ll_push_uarray() appears to be broken.
 * When I print the table entries in Lua, each value is 255. It seems as if only
 * the least significant byte of each l_uint32 is pushed?
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 array tables on the Lua stack
 */
static int
ToRGBTable(lua_State *L)
{
    FUNC(LL_PIXCMAP ".ToRGBTable");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_int32 ncolors = 0;
    l_uint32 *table = nullptr;
    l_int32 i;
    if (pixcmapToRGBTable(cmap, &table, &ncolors))
        return ll_push_nil(L);
    ll_push_uarray(L, table, ncolors);
    LEPT_FREE(table);
    return 4;
}

/**
 * \brief Get usable color from a PixColormap* (%cmap)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a l_int32 (rval).
 * Arg #3 is expected to be a l_int32 (gval).
 * Arg #4 is expected to be a l_int32 (bval).
 * </pre>
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
 * \brief Write a PixColormap* (%cmap) to a file
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a string (filename).
 * </pre>
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
 * \brief Write the PixColormap* (%cmap) to memory and return it as a Lua string
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    FUNC(LL_PIXCMAP ".WriteMem");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixcmapWriteMem(&data, &size, cmap))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Write a PixColormap* (%cmap) to a Lua io stream (%stream)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmap).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    FUNC(LL_PIXCMAP ".WriteStream");
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    if (pixcmapWriteStream(stream->f, cmap))
        return ll_push_nil(L);
    lua_pushboolean(L, TRUE);
    return 1;
}

/**
 * \brief Create a random PixColormap* (%cmap)
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * Arg #2 is optional and, if given, expected to be a boolean (hasblack).
 * Arg #3 is optional and, if given, expected to be a boolean (haswhite).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
CreateRandom(lua_State *L)
{
    FUNC(LL_PIXCMAP ".CreateRandom");
    l_int32 depth = ll_check_l_int32(_fun, L, 1);
    l_int32 hasblack = ll_check_boolean_default(_fun, L, 2, FALSE);
    l_int32 haswhite = ll_check_boolean_default(_fun, L, 3, FALSE);
    PixColormap *cmap = pixcmapCreateRandom(depth, hasblack, haswhite);
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Create a linear PixColormap* (%cmap)
 * <pre>
 * Arg #1 is expected to be a l_int32 (depth).
 * Arg #2 is expected to be a l_int32 (levels).
 * </pre>
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
 * \brief Check Lua stack at index %arg for udata of class LL_PIXCMAP
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data
 */
PixColormap *
ll_check_PixColormap(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixColormap>(_fun, L, arg, LL_PIXCMAP);
}

/**
 * \brief Optionally expect a LL_PIXCMAP at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data
 */
PixColormap *
ll_check_PixColormap_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_PixColormap(_fun, L, arg);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PIXCMAP
 *
 * This version removes the PixColormap* from the object PIX;
 * It is used when the PixColormap* is e.g. attached to a Pix*.
 * The reason is that a PixColormap* does not have a reference
 * count and thus can be used exactly once in Pix:SetColormap().
 *
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixColormap* contained in the user data
 */
PixColormap *
ll_take_PixColormap(lua_State *L, int arg)
{
    FUNC("ll_take_PixColormap");
    PixColormap **pcmap = ll_check_udata<PixColormap>(_fun, L, arg, LL_PIXCMAP);
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
        {"__gc",                    Destroy},   /* garbage collector */
        {"__new",                   Create},    /* new PixColormap */
        {"__len",                   GetCount},  /* #cmap */
        {"__tostring",              toString},
        {"AddBlackOrWhite",         AddBlackOrWhite},
        {"AddColor",                AddColor},
        {"AddNearestColor",         AddNearestColor},
        {"AddNewColor",             AddNewColor},
        {"AddRGBA",                 AddRGBA},
        {"Clear",                   Clear},
        {"ConvertToHex",            ConvertToHex},
        {"Copy",                    Copy},
        {"CountGrayColors",         CountGrayColors},
        {"DeserializeFromMemory",   DeserializeFromMemory},
        {"Destroy",                 Destroy},
        {"GetColor",                GetColor},
        {"GetColor32",              GetColor32},
        {"GetCount",                GetCount},  /* same as #cmap */
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

    static const luaL_Reg functions[] = {
        {"Create",                  Create},
        {"CreateRandom",            CreateRandom},
        {"CreateLinear",            CreateLinear},
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_PIXCMAP);
    return ll_register_class(L, LL_PIXCMAP, methods, functions);
}
