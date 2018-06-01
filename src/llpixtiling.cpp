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

/** Define a function's name (_fun) with prefix LL_PIXTILING */
#define LL_FUNC(x) FUNC(LL_PIXTILING "." x)

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (freeflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixTiling **ppt = ll_check_udata<PixTiling>(_fun, L, 1, LL_PIXTILING);
    PixTiling *pt = *ppt;
    DBG(LOG_DESTROY, "%s: '%s' ppt=%p pt=%p\n",
        _fun, LL_SEL, ppt, pt);
    pixTilingDestroy(&pt);
    *ppt = nullptr;
    return 0;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. stackf) is expected to be a PixTiling* (lstack).
 * </pre>
 * \param L pointer to the lua_State
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
    return ll_push_l_int32(_fun, L, nx) + ll_push_l_int32(_fun, L, ny);
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
 * \param L pointer to the lua_State
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
 * \brief Check Lua stack at index (%arg) for udata of class LL_PIXTILING.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixTiling* contained in the user data
 */
PixTiling *
ll_check_PixTiling(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixTiling>(_fun, L, arg, LL_PIXTILING);
}

/**
 * \brief Optionally expect a PixTiling* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixTiling* contained in the user data
 */
PixTiling *
ll_opt_PixTiling(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_PixTiling(_fun, L, arg);
}
/**
 * \brief Push PixTiling* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_PixTiling
 * \return 1 PixTiling* on the Lua stack
 */
int
ll_push_PixTiling(const char *_fun, lua_State *L, PixTiling *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PIXTILING, cd);
}

/**
 * \brief Create and push a new PixTiling*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 PixTiling* on the Lua stack
 */
int
ll_new_PixTiling(lua_State *L)
{
    FUNC("ll_new_PixTiling");
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
 * \brief Register the PixTiling methods and functions in the LL_PIXTILING meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PixTiling(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               ll_new_PixTiling},
        {"__len",               GetCount},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_PixTiling);
    lua_setglobal(L, LL_PIXTILING);
    return ll_register_class(L, LL_PIXTILING, methods, functions);
}
