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
 * \file llpixtiling.cpp
 * \class PixTiling
 *
 * A class to handle a Pix tiling.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXTILING

/** Define a function's name (_fun) with prefix PixTiling */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (freeflag).
 * </pre>
 * \param L Lua state
 * \return 1 void on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixTiling *pt = ll_take_udata<PixTiling>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "pt", reinterpret_cast<void *>(pt));
    pixTilingDestroy(&pt);
    return 0;
}

/**
 * \brief Printable string for a PixColormap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L Lua state
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    PixTiling *pixt = ll_check_PixTiling(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);

    if (!pixt) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME ": %p",
                 reinterpret_cast<void *>(pixt));
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    " LL_PIX "*: %p\n",
                 reinterpret_cast<void *>(pixt->pix));
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    nx = %d, ny = %d, w = %d, h = %d\n",
                 pixt->nx, pixt->ny, pixt->w, pixt->h);
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    xoverlap = %d, yoverlap = %d, strip = %d\n",
                 pixt->xoverlap, pixt->yoverlap, pixt->strip);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. stackf) is expected to be a PixTiling* (lstack).
 * </pre>
 * \param L Lua state
 * \return 1 l_int32 on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 nx = 0;
    l_int32 ny = 0;
    if (pixTilingGetCount(pt, &nx, &ny))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, nx * ny);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nx).
 * Arg #3 is expected to be a l_int32 (ny).
 * Arg #4 is expected to be a l_int32 (w).
 * Arg #5 is expected to be a l_int32 (h).
 * Arg #6 is expected to be a l_int32 (xoverlap).
 * Arg #7 is expected to be a l_int32 (yoverlap).
 *
 * Leptonica's Notes:
 *      (1) We put a clone of pixs in the PixTiling.
 *      (2) The input to pixTilingCreate() for horizontal tiling can be
 *          either the number of tiles across the image or the approximate
 *          width of the tiles.  If the latter, the actual width will be
 *          determined by making all tiles but the last of equal width, and
 *          making the last as close to the others as possible.  The same
 *          consideration is applied independently to the vertical tiling.
 *          To specify tile width, set nx = 0; to specify the number of
 *          tiles horizontally across the image, set w = 0.
 *      (3) If pixs is to be tiled in one-dimensional strips, use ny = 1 for
 *          vertical strips and nx = 1 for horizontal strips.
 *      (4) The overlap must not be larger than the width or height of
 *          the leftmost or topmost tile(s).
 * </pre>
 * \param L Lua state
 * \return 1 PixTiling * on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nx = ll_check_l_int32(_fun, L, 2);
    l_int32 ny = ll_check_l_int32(_fun, L, 3);
    l_int32 w = ll_check_l_int32(_fun, L, 4);
    l_int32 h = ll_check_l_int32(_fun, L, 5);
    l_int32 xoverlap = ll_check_l_int32(_fun, L, 6);
    l_int32 yoverlap = ll_check_l_int32(_fun, L, 7);
    PixTiling *pixt = pixTilingCreate(pixs, nx, ny, w, h, xoverlap, yoverlap);
    return ll_push_PixTiling (_fun, L, pixt);
}

/**
 * \brief GetCount() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
GetCountXY(lua_State *L)
{
    LL_FUNC("GetCountXY");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 nx = 0;
    l_int32 ny = 0;
    if (pixTilingGetCount(pt, &nx, &ny))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nx);
    ll_push_l_int32(_fun, L, ny);
    return 2;
}

/**
 * \brief GetSize() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
GetSize(lua_State *L)
{
    LL_FUNC("GetSize");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (pixTilingGetSize(pt, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief GetTile() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * Arg #2 is expected to be an index (i).
 * Arg #3 is expected to be an index (j).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
GetTile(lua_State *L)
{
    LL_FUNC("GetTile");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 i = ll_check_index(_fun, L, 2);
    l_int32 j = ll_check_index(_fun, L, 3);
    Pix *pix = pixTilingGetTile(pt, i, j);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief NoStripOnPaint() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 *
 * Leptonica's Notes:
 *      (1) The default for paint is to strip out the overlap pixels
 *          that are added by pixTilingGetTile().  However, some
 *          operations will generate an image with these pixels
 *          stripped off.  This tells the paint operation not
 *          to strip the added boundary pixels when painting.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
NoStripOnPaint(lua_State *L)
{
    LL_FUNC("NoStripOnPaint");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_ok ok = pixTilingNoStripOnPaint(pt);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief PaintTile() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * Arg #4 is expected to be a Pix* (pixd).
 * Arg #5 is expected to be a Pix* (pixs).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
PaintTile(lua_State *L)
{
    LL_FUNC("PaintTile");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 i = ll_check_l_int32(_fun, L, 2);
    l_int32 j = ll_check_l_int32(_fun, L, 3);
    Pix *pixd = ll_check_Pix(_fun, L, 4);
    Pix *pixs = ll_check_Pix(_fun, L, 5);
    l_ok ok = pixTilingPaintTile(pixd, i, j, pixs, pt);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class PixTiling*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixTiling* contained in the user data
 */
PixTiling *
ll_check_PixTiling(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixTiling>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a PixTiling* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixTiling* contained in the user data
 */
PixTiling *
ll_opt_PixTiling(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_PixTiling(_fun, L, arg);
}
/**
 * \brief Push PixTiling* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param cd pointer to the L_PixTiling
 * \return 1 PixTiling* on the Lua stack
 */
int
ll_push_PixTiling(const char *_fun, lua_State *L, PixTiling *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new PixTiling*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L Lua state
 * \return 1 PixTiling* on the Lua stack
 */
int
ll_new_PixTiling(lua_State *L)
{
    FUNC("ll_new_PixTiling");
    PixTiling *pixt = nullptr;
    Pix *pixs = ll_opt_Pix(_fun, L, 1);
    if (pixs) {
        l_int32 nx = ll_opt_l_int32(_fun, L, 2, 2);
        l_int32 ny = ll_opt_l_int32(_fun, L, 3, 2);
        l_int32 w = ll_opt_l_int32(_fun, L, 4);
        l_int32 h = ll_opt_l_int32(_fun, L, 5);
        l_int32 xoverlap = ll_opt_l_int32(_fun, L, 6);
        l_int32 yoverlap = ll_opt_l_int32(_fun, L, 7);
        pixt = pixTilingCreate(pixs, nx, ny, w, h, xoverlap, yoverlap);
    }
    return ll_push_PixTiling (_fun, L, pixt);
}

/**
 * \brief Register the PixTiling methods and functions in the PixTiling meta table.
 * \param L Lua state
 * \return 1 table on the Lua stack
 */
int
ll_open_PixTiling(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_PixTiling},
        {"__len",               GetCount},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"GetCount",            GetCountXY},
        {"GetSize",             GetSize},
        {"GetTile",             GetTile},
        {"NoStripOnPaint",      NoStripOnPaint},
        {"PaintTile",           PaintTile},         /* same as Pix.PaintTile() with arguments swapped */
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_PixTiling);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
