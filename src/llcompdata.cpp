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
 *  Lua class CompData
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_COMPDATA */
#define LL_FUNC(x) FUNC(LL_COMPDATA "." x)

/**
 * \brief Printable string for a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    CompressedData *cdata = ll_check_CompData(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!cdata) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), LL_COMPDATA ": %p\n", reinterpret_cast<void *>(cdata));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    type          : %s\n", ll_string_encoding(cdata->type));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    datacomp      : %p\n", reinterpret_cast<void *>(cdata->datacomp));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytescomp    : %" PRIu64 "\n", static_cast<l_uintptr_t>(cdata->nbytescomp));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    data85        : %p\n", reinterpret_cast<void *>(cdata->data85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytes85      : %" PRIu64 "\n", static_cast<l_uintptr_t>(cdata->nbytes85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    cmapdata85    : %p\n", reinterpret_cast<void *>(cdata->cmapdata85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    cmapdatahex   : %p\n", reinterpret_cast<void *>(cdata->cmapdatahex));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    ncolors       : %d\n", cdata->ncolors);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    w             : %d\n", cdata->w);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    h             : %d\n", cdata->h);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    bps           : %d\n", cdata->bps);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    spp           : %d\n", cdata->spp);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    minisblack    : %d\n", cdata->minisblack);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    predictor     : %d\n", cdata->predictor);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytes        : %" PRIu64 "\n", static_cast<l_uintptr_t>(cdata->nbytes));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    res           : %d\n", cdata->res);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Destroy a CompData*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CompressedData **pcdata = ll_check_udata<CompressedData>(_fun, L, 1, LL_COMPDATA);
    CompressedData *cdata = *pcdata;
    DBG(LOG_DESTROY, "%s: '%s' pcdata=%p cdata=%p\n",
        _fun, LL_COMPDATA, pcdata, cdata);
    ll_free(cdata);
    *pcdata = nullptr;
    return 0;
}

/**
 * \brief Create a new CompData*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    CompressedData *cdata = ll_calloc<CompressedData>(_fun, L, 1);
    return ll_push_CompData(_fun, L, cdata);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_COMPDATA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data
 */
CompressedData *
ll_check_CompData(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CompressedData>(_fun, L, arg, LL_COMPDATA);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data
 */
CompressedData *
ll_check_CompData_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_CompData(_fun, L, arg);
}
/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cdata pointer to the L_CompData
 * \return 1 CompData* on the Lua stack
 */
int
ll_push_CompData(const char *_fun, lua_State *L, CompressedData *cdata)
{
    if (!cdata)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_COMPDATA, cdata);
}
/**
 * \brief Create and push a new CompData*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
 */
int
ll_new_CompData(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the BMF methods and functions in the LL_COMPDATA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_CompData(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_COMPDATA);
    return ll_register_class(L, LL_COMPDATA, methods, functions);
}
