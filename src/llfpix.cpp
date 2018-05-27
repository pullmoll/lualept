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
 *  Lua class FPix
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_FPIX */
#define LL_FUNC(x) FUNC(LL_FPIX "." x)

/**
 * \brief Destroy a FPix*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    FPix **pfpix = ll_check_udata<FPix>(_fun, L, 1, LL_FPIX);
    FPix *fpix = *pfpix;
    DBG(LOG_DESTROY, "%s: '%s' pfpix=%p fpix=%p\n",
        _fun, LL_FPIX, pfpix, fpix);
    fpixDestroy(&fpix);
    *pfpix = nullptr;
    return 0;
}

/**
 * \brief Create a new FPix*.
 *
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 *
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 width = ll_check_l_int32_default(_fun, L, 1, 1);
    l_int32 height = ll_check_l_int32_default(_fun, L, 2, 1);
    FPix *fpix = fpixCreate(width, height);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_FPIX.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data
 */
FPix *
ll_check_FPix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<FPix>(_fun, L, arg, LL_FPIX);
}

/**
 * \brief Optionally expect a FPix* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data
 */
FPix *
ll_check_FPix_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_FPix(_fun, L, arg);
}
/**
 * \brief Push FPix* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_FPix
 * \return 1 FPix* on the Lua stack
 */
int
ll_push_FPix(const char *_fun, lua_State *L, FPix *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_FPIX, cd);
}
/**
 * \brief Create and push a new FPix*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
int
ll_new_FPix(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the FPix methods and functions in the LL_FPIX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_FPix(lua_State *L)
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
    lua_setglobal(L, LL_FPIX);
    return ll_register_class(L, LL_FPIX, methods, functions);
}
