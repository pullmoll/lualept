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
 * \file llwshed.cpp
 * \class WShed
 *
 * A bitmap font.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_WSHED

/** Define a function's name (_fun) with prefix WShed */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a WShed* (wshed).
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    WShed *wshed = ll_take_udata<WShed>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "wshed", reinterpret_cast<void *>(wshed));
    wshedDestroy(&wshed);
    return 0;
}

/**
 * \brief Printable string for a WShed*.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    WShed *ws = ll_check_WShed(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!ws) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(ws));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    pixs              : %s* %p", LL_PIX, reinterpret_cast<void *>(ws->pixs));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    pixm              : %s* %p", LL_PIX, reinterpret_cast<void *>(ws->pixm));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    mindepth          : %d", ws->mindepth);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    pixlab            : %s* %p", LL_PIX, reinterpret_cast<void *>(ws->pixlab));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    pixt              : %s* %p", LL_PIX, reinterpret_cast<void *>(ws->pixt));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    lines8            : %s* %p", "void*", reinterpret_cast<void *>(ws->lines8));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    linem1            : %s* %p", "void*", reinterpret_cast<void *>(ws->linem1));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    linelab32         : %s* %p", "void*", reinterpret_cast<void *>(ws->linelab32));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    linet1            : %s* %p", "void*", reinterpret_cast<void *>(ws->linet1));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    pixad             : %s* %p", LL_PIXA, reinterpret_cast<void *>(ws->pixad));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    ptas              : %s* %p", LL_PTA, reinterpret_cast<void *>(ws->ptas));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    nasi              : %s* %p", LL_NUMA, reinterpret_cast<void *>(ws->nasi));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    nash              : %s* %p", LL_NUMA, reinterpret_cast<void *>(ws->nash));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    namh              : %s* %p", LL_NUMA, reinterpret_cast<void *>(ws->namh));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    nalevels          : %s* %p", LL_NUMA, reinterpret_cast<void *>(ws->nalevels));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    nseeds            : %d", ws->nseeds);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    nother            : %d", ws->nother);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    lut               : %s* %p", "l_int32", reinterpret_cast<void *>(ws->lut));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    links             : %s* %p", LL_NUMA, reinterpret_cast<void *>(ws->links));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    arraysize         : %d", ws->arraysize);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    debug             : %d", ws->debug);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Apply the WShed* (%wsched).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a WShed* (wshed).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Apply(lua_State *L)
{
    LL_FUNC("Apply");
    WShed *wshed = ll_check_WShed(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == wshedApply(wshed));
}

/**
 * \brief Get the basins() for the WShed (%wshed).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a WShed* (wshed).
 * </pre>
 * \param L Lua state.
 * \return 2 Pixa* (%pixa) and Numa* (%nalevels) on the Lua stack.
 */
static int
Basins(lua_State *L)
{
    LL_FUNC("Basins");
    WShed *wshed = ll_check_WShed(_fun, L, 1);
    Pixa *pixa = nullptr;
    Numa *nalevels = nullptr;
    if (wshedBasins(wshed, &pixa, &nalevels))
        return ll_push_nil(_fun, L);
    ll_push_Pixa(_fun, L, pixa);
    ll_push_Numa(_fun, L, nalevels);
    return 2;
}

/**
 * \brief Create() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (mindepth).
 * Arg #4 is expected to be a l_int32 (debugflag).
 *
 * Leptonica's Notes:
 *      (1) It is not necessary for the fg pixels in the seed image
 *          be at minima, or that they be isolated.  We extract a
 *          single pixel from each connected component, and a seed
 *          anywhere in a watershed will eventually label the watershed
 *          when the filling level reaches it.
 *      (2) Set mindepth to some value to ignore noise in pixs that
 *          can create small local minima.  Any watershed shallower
 *          than mindepth, even if it has a seed, will not be saved;
 *          It will either be incorporated in another watershed or
 *          eliminated.
 * </pre>
 * \param L Lua state.
 * \return 1 WShed* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 mindepth = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 debugflag = ll_opt_boolean(_fun, L, 4, FALSE);
    WShed *ws = wshedCreate(pixs, pixm, mindepth, debugflag);
    return ll_push_WShed(_fun, L, ws);
}

/**
 * \brief Render colors for a WShed* (%wshed) and return the Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a WShed* (wshed).
 * </pre>
 * \param L Lua state.
 * \return 1 Pix* on the Lua stack.
 */
static int
RenderColors(lua_State *L)
{
    LL_FUNC("RenderColors");
    WShed *wshed = ll_check_WShed(_fun, L, 1);
    Pix *pix = wshedRenderColors(wshed);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Render fill a WShed* (%wshed) and return the Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a WShed* (wshed).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Pix* on the Lua stack.
 */
static int
RenderFill(lua_State *L)
{
    LL_FUNC("RenderFill");
    WShed *wshed = ll_check_WShed(_fun, L, 1);
    Pix *pix = wshedRenderFill(wshed);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check Lua stack at index %arg for user data of class WShed*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the WShed* contained in the user data.
 */
WShed *
ll_check_WShed(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<WShed>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the WShed* contained in the user data.
 */
WShed *
ll_opt_WShed(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_WShed(_fun, L, arg);
}
/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param ws pointer to the WShed
 * \return 1 WShed* on the Lua stack.
 */
int
ll_push_WShed(const char *_fun, lua_State *L, WShed *ws)
{
    if (!ws)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, ws);
}
/**
 * \brief Create and push a new WShed*.
 * \param L Lua state.
 * \return 1 WShed* on the Lua stack.
 */
int
ll_new_WShed(lua_State *L)
{
    FUNC("ll_new_WShed");
    WShed *wshed = nullptr;
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 mindepth = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 debugflag = ll_opt_boolean(_fun, L, 4, FALSE);

    DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s* = %p, %s = %d, %s = %s\n", _fun,
        LL_PIX, reinterpret_cast<void *>(pixs),
        LL_PIX, reinterpret_cast<void *>(pixm),
        "mindepth", mindepth,
        "debugflag", debugflag ? "TRUE" : "FALSE");
    wshed = wshedCreate(pixs, pixm, mindepth, debugflag);
    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(wshed));
    return ll_push_WShed(_fun, L, wshed);
}

/**
 * \brief Register the WShed methods and functions in the TNAME meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_WShed(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_WShed},
        {"__tostring",          toString},
        {"Apply",               Apply},
        {"Basins",              Basins},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"RenderColors",        RenderColors},
        {"RenderFill",          RenderFill},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_WShed);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
