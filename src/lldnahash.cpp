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
 * \file lldnahash.cpp
 * \class DnaHash
 *
 * A byte buffer.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DNAHASH

/** Define a function's name (_fun) with prefix DnaHash */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DnaHash* (dnahash).
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    DnaHash *bb = ll_take_udata<DnaHash>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "bb", reinterpret_cast<void *>(bb));
    l_dnaHashDestroy(&bb);
    return 0;
}

/**
 * \brief Printable string for a DnaHash*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DnaHash* (dnahash).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    DnaHash *dh = ll_check_DnaHash(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dh) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(dh));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %s = 0x%x, %s = 0x%x, %s = %p",
                 "nbuckets", dh->nbuckets,
                 "initsize", dh->initsize,
                 "dna", reinterpret_cast<void *>(dh->dna));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}


/**
 * \brief Create a new DnaHash* (%dh)
 * <pre>
 * Arg #1 is expected to be a l_int32 (nbuckets).
 * Arg #2 is expected to be a l_int32 (initsize).
 *
 * Leptonica's Notes:
 *      (1) Actual dna are created only as required by l_dnaHashAdd()
 * </pre>
 * \param L Lua state.
 * \return 1 DnaHash* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nbuckets = ll_opt_l_int32(_fun, L, 5);
    l_int32 initsize = ll_opt_l_int32(_fun, L, 10);
    DnaHash *dh = l_dnaHashCreate(nbuckets, initsize);
    return ll_push_DnaHash(_fun, L, dh);
}

/**
 * \brief Check Lua stack at index %arg for user data of class DnaHash*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DnaHash* contained in the user data.
 */
DnaHash *
ll_check_DnaHash(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DnaHash>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DnaHash* contained in the user data.
 */
DnaHash *
ll_opt_DnaHash(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_DnaHash(_fun, L, arg);
}

/**
 * \brief Push DnaHash user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param dh pointer to the DnaHash
 * \return 1 DnaHash* on the Lua stack.
 */
int
ll_push_DnaHash(const char *_fun, lua_State *L, DnaHash *dh)
{
    if (!dh)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, dh);
}

/**
 * \brief Create and push a new DnaHash*.
 * \param L Lua state.
 * \return 1 DnaHash* on the Lua stack.
 */
int
ll_new_DnaHash(lua_State *L)
{
    FUNC("ll_new_DnaHash");
    DnaHash *dh = nullptr;
    l_int32 nbuckets = 5;
    l_int32 initsize = 10;

    if (ll_isinteger(_fun, L, 1)) {
        nbuckets = ll_opt_l_int32(_fun, L, nbuckets);
        initsize = ll_opt_l_int32(_fun, L, initsize);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "nbuckets", nbuckets,
            "initsize", initsize);
        dh = l_dnaHashCreate(nbuckets, initsize);
    }

    if (!dh) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "nbuckets", nbuckets,
            "initsize", initsize);
        dh = l_dnaHashCreate(nbuckets, initsize);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(dh));
    return ll_push_DnaHash(_fun, L, dh);
}

/**
 * \brief Register the DnaHash methods and functions in the TNAME meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_DnaHash(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_DnaHash},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_DnaHash);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
