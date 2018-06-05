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
 * \file llbmf.cpp
 * \class Bmf
 *
 * A bitmap font.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BMF

/** Define a function's name (_fun) with prefix Bmf */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Bmf*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    L_Bmf **pbmf = ll_check_udata<L_Bmf>(_fun, L, 1, TNAME);
    L_Bmf *bmf = *pbmf;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "pbmf", reinterpret_cast<void *>(pbmf),
        "bmf", reinterpret_cast<void *>(bmf));
    bmfDestroy(&bmf);
    *pbmf = nullptr;
    return 0;
}

/**
 * \brief Create a new Bmf*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 Bmf* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    const char* dir = ll_isstring(_fun, L, 1) ? ll_check_string(_fun, L, 1) : ".";
    l_int32 fontsize = ll_opt_l_int32(_fun, L, 2, 6);
    L_Bmf *bmf = bmfCreate(dir, fontsize);
    return ll_push_Bmf(_fun, L, bmf);
}

/**
 * \brief Get the baseline for a character from the Bmf* (%bmf).
 *
 * Arg #1 is expected to be a Bmf* (bmf).
 * Arg #2 is expected to be a character (chr).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBaseline(lua_State *L)
{
    LL_FUNC("GetBaseline");
    L_Bmf *bmf = ll_check_Bmf(_fun, L, 1);
    char chr = ll_check_char(_fun, L, 2);
    l_int32 baseline = 0;
    if (bmfGetBaseline(bmf, chr, &baseline))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, baseline);
    return 1;
}

/**
 * \brief Get the line strings for a string using the Bmf* (%bmf).
 *
 * Arg #1 is expected to be a Bmf* (bmf).
 * Arg #2 is expected to be a string (str).
 * Arg #3 is expected to be a l_int32 (maxw).
 * Arg #4 is an optional l_int32 (firstident).
 *
 * \param L pointer to the lua_State
 * \return 1 integer (h) plus a table of strings on the Lua stack
 */
static int
GetLineStrings(lua_State *L)
{
    LL_FUNC("GetLineStrings");
    L_Bmf *bmf = ll_check_Bmf(_fun, L, 1);
    const char* str = ll_check_string(_fun, L, 2);
    l_int32 maxw = ll_check_l_int32(_fun, L, 3);
    l_int32 firstident = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 h = 0;
    Sarray *sa = bmfGetLineStrings(bmf, str, maxw, firstident, &h);
    ll_push_l_int32(_fun, L, h);
    ll_pack_Sarray(_fun, L, sa);
    sarrayDestroy(&sa);
    return 2;
}

/**
 * \brief Get the Pix* for a character from the Bmf* (%bmf).
 *
 * Arg #1 is expected to be a Bmf* (bmf).
 * Arg #2 is expected to be a character (chr).
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
GetPix(lua_State *L)
{
    LL_FUNC("GetPix");
    L_Bmf *bmf = ll_check_Bmf(_fun, L, 1);
    char chr = ll_check_char(_fun, L, 2);
    Pix *pix = bmfGetPix(bmf, chr);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get the baseline for a character from the Bmf* (%bmf).
 *
 * Arg #1 is expected to be a Bmf* (bmf).
 * Arg #2 is expected to be a string (str).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetStringWidth(lua_State *L)
{
    LL_FUNC("GetStringWidth");
    L_Bmf *bmf = ll_check_Bmf(_fun, L, 1);
    const char* str = ll_check_string(_fun, L, 2);
    l_int32 w;
    if (bmfGetStringWidth(bmf, str, &w))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    return 1;
}

/**
 * \brief Get the width for a character from the Bmf* (%bmf).
 *
 * Arg #1 is expected to be a Bmf* (bmf).
 * Arg #2 is expected to be a character (chr).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWidth(lua_State *L)
{
    LL_FUNC("GetWidth");
    L_Bmf *bmf = ll_check_Bmf(_fun, L, 1);
    char chr = ll_check_char(_fun, L, 2);
    l_int32 w = 0;
    if (bmfGetWidth(bmf, chr, &w))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class Bmf*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bmf* contained in the user data
 */
L_Bmf *
ll_check_Bmf(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<L_Bmf>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Bmf* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bmf* contained in the user data
 */
Bmf *
ll_take_Bmf(const char *_fun, lua_State *L, int arg)
{
    Bmf **pbmf = ll_check_udata<Bmf>(_fun, L, arg, TNAME);
    Bmf *bmf = *pbmf;
    *pbmf = nullptr;
    return bmf;
}

/**
 * \brief Take a Bmf* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
Bmf *
ll_global_Bmf(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_Bmf(_fun, L, 1);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bmf* contained in the user data
 */
L_Bmf *
ll_opt_Bmf(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Bmf(_fun, L, arg);
}
/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param bmf pointer to the L_Bmf
 * \return 1 Bmf* on the Lua stack
 */
int
ll_push_Bmf(const char *_fun, lua_State *L, L_Bmf *bmf)
{
    if (!bmf)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, bmf);
}
/**
 * \brief Create and push a new Bmf*.
 * \param L pointer to the lua_State
 * \return 1 Bmf* on the Lua stack
 */
int
ll_new_Bmf(lua_State *L)
{
    FUNC("ll_new_Bmf");
    const char* dir = ll_isstring(_fun, L, 1) ? ll_check_string(_fun, L, 1) : ".";
    l_int32 fontsize = ll_opt_l_int32(_fun, L, 2, 6);
    L_Bmf *bmf = nullptr;

    DBG(LOG_NEW_PARAM, "%s: create for %s = '%s', %s = %d\n", _fun,
        "dir", dir,
        "fontsize", fontsize);
    bmf = bmfCreate(dir, fontsize);
    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(bmf));
    return ll_push_Bmf(_fun, L, bmf);
}
/**
 * \brief Register the BMF methods and functions in the TNAME meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_Bmf(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Bmf},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"GetBaseline",         GetBaseline},
        {"GetLineStrings",      GetLineStrings},
        {"GetPix",              GetPix},
        {"GetStringWidth",      GetStringWidth},
        {"GetWidth",            GetWidth},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_Bmf);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
