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
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixaComp **ppixac = ll_check_udata<PixaComp>(_fun, L, 1, TNAME);
    PixaComp *pixac = *ppixac;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "ppixac", reinterpret_cast<void *>(ppixac),
        "pixac", reinterpret_cast<void *>(pixac),
        "count", pixacompGetCount(pixac));
    pixacompDestroy(&pixac);
    *ppixac = nullptr;
    return 0;
}

/**
 * \brief Create a new PixaComp*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixaComp* on the Lua stack
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
 * \brief Check Lua stack at index (%arg) for udata of class PixaComp*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data
 */
PixaComp *
ll_check_PixaComp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixaComp>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class PixaComp* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data
 */
PixaComp *
ll_take_PixaComp(const char *_fun, lua_State *L, int arg)
{
    PixaComp **ppixac = ll_check_udata<PixaComp>(_fun, L, arg, TNAME);
    PixaComp *pixac = *ppixac;
    *ppixac = nullptr;
    return pixac;
}

/**
 * \brief Take a PixaComp* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
PixaComp *
ll_global_PixaComp(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_PixaComp(_fun, L, 1);
}

/**
 * \brief Optionally expect a PixaComp* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data
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
 * \param L pointer to the lua_State
 * \param pixacomp pointer to the PixaComp
 * \return 1 PixaComp* on the Lua stack
 */
int
ll_push_PixaComp(const char *_fun, lua_State *L, PixaComp *pixacomp)
{
    if (!pixacomp)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, pixacomp);
}

/**
 * \brief Create and push a new PixaComp*.
 * \param L pointer to the lua_State
 * \return 1 PixaComp* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_PixaComp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_PixaComp},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_PixaComp);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
