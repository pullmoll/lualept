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
 * \file llpixacomp.cpp
 * \class PixaComp
 *
 * A class to handle compressed Pix.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXACOMP

/** Define a function's name (_fun) with prefix PixaCompt */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a PixaComp*.
 *
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixaComp *pixac = ll_take_udata<PixaComp>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "pixac", reinterpret_cast<void *>(pixac),
        "count", pixacompGetCount(pixac));
    pixacompDestroy(&pixac);
    return 0;
}

/**
 * \brief Printable string for a PixaComp*.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    PixaComp *pixac = ll_check_PixaComp(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!pixac) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(pixac));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "n", pixac->n);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nalloc", pixac->nalloc);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "offset", pixac->offset);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s** %p",
                 "pixc", LL_PIXCOMP, reinterpret_cast<void *>(pixac->pixc));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s* %p",
                 "boxa", LL_BOXA, reinterpret_cast<void *>(pixac->boxa));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Create a new PixaComp*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 PixaComp* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    PixaComp *pixacomp = pixacompCreate(n);
    return ll_push_PixaComp(_fun, L, pixacomp);
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class PixaComp*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data.
 */
PixaComp *
ll_check_PixaComp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixaComp>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a PixaComp* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data.
 */
PixaComp *
ll_opt_PixaComp(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_PixaComp(_fun, L, arg);
}

/**
 * \brief Push PixaComp* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param pixacomp pointer to the PixaComp
 * \return 1 PixaComp* on the Lua stack.
 */
int
ll_push_PixaComp(const char *_fun, lua_State *L, PixaComp *pixacomp)
{
    if (!pixacomp)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, pixacomp);
}

/**
 * \brief Create and push a new PixaComp*.
 * \param L Lua state.
 * \return 1 PixaComp* on the Lua stack.
 */
int
ll_new_PixaComp(lua_State *L)
{
    FUNC("ll_new_PixaComp");
    PixaComp *pixacomp = nullptr;
    l_int32 n = 1;

    if (ll_isinteger(_fun, L, 1)) {
        n = ll_opt_l_int32(_fun, L, 1, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixacomp = pixacompCreate(n);
    }

    if (!pixacomp) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixacomp = pixacompCreate(n);
    }

    return ll_push_PixaComp(_fun, L, pixacomp);
}

/**
 * \brief Register the PixaComp methods and functions in the PixaComp meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_PixaComp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_PixaComp},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_PixaComp);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
