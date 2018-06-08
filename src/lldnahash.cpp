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
 *
 * Leptonica's Notes:
 *      (1) Destroys the byte array in the dnahash and then the dnahash;
 *          then nulls the contents of the input ptr.
 * \param L Lua state
 * \return 0 on the Lua stack
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
 * \param L Lua state
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    DnaHash *dh = ll_check_DnaHash(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dh) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), TNAME ": %p\n", reinterpret_cast<void *>(dh));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbuckets = 0x%x, initsize = 0x%x, dna = %p",
                 dh->nbuckets, dh->initsize,
                 reinterpret_cast<void *>(dh->dna));
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}


/**
 * \brief Create() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (data).
 * Arg #2 is expected to be a l_int32 (nalloc).
 *
 * Leptonica's Notes:
 *      (1) If a buffer address is given, you should read all the data in.
 *      (2) Allocates a dnahash with associated byte array of
 *          the given size.  If a buffer address is given,
 *          it then reads the number of bytes into the byte array.
 * </pre>
 * \param L Lua state
 * \return 1 DnaHash* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nbuckets = ll_opt_l_int32(_fun, L, 5);
    l_int32 initsize = ll_opt_l_int32(_fun, L, 10);
    DnaHash *result = l_dnaHashCreate(nbuckets, initsize);
    return ll_push_DnaHash(_fun, L, result);
}

/**
 * \brief Check Lua stack at index %arg for udata of class DnaHash*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DnaHash* contained in the user data
 */
DnaHash *
ll_check_DnaHash(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DnaHash>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DnaHash* contained in the user data
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
 * \param L Lua state
 * \param dh pointer to the DnaHash
 * \return 1 DnaHash* on the Lua stack
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
 * \param L Lua state
 * \return 1 DnaHash* on the Lua stack
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
 * \param L Lua state
 * \return 1 table on the Lua stack
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
