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
 *  Lua class PixaComp
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_PIXACOMP */
#define LL_FUNC(x) FUNC(LL_PIXACOMP "." x)

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
    PixaComp **ppixacomp = ll_check_udata<PixaComp>(_fun, L, 1, LL_PIXACOMP);
    PixaComp *pixacomp = *ppixacomp;
    DBG(LOG_DESTROY, "%s: '%s' ppixacomp=%p pixacomp=%p\n",
        _fun, LL_PIXACOMP, ppixacomp, pixacomp);
    pixacompDestroy(&pixacomp);
    *ppixacomp = nullptr;
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
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    PixaComp *pixacomp = pixacompCreate(n);
    return ll_push_PixaComp(_fun, L, pixacomp);
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_PIXACOMP.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data
 */
PixaComp *
ll_check_PixaComp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixaComp>(_fun, L, arg, LL_PIXACOMP);
}

/**
 * \brief Optionally expect a PixaComp* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixaComp* contained in the user data
 */
PixaComp *
ll_check_PixaComp_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
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
    return ll_push_udata(_fun, L, LL_PIXACOMP, pixacomp);
}
/**
 * \brief Create and push a new PixaComp*.
 * \param L pointer to the lua_State
 * \return 1 PixaComp* on the Lua stack
 */
int
ll_new_PixaComp(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the PixaComp methods and functions in the LL_PIXACOMP meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PixaComp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_PIXACOMP);
    return ll_register_class(L, LL_PIXACOMP, methods, functions);
}
