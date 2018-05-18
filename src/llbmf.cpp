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
 *  Lua class BMF
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_BMF
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bmf* contained in the user data
 */
L_Bmf *
ll_check_Bmf(lua_State *L, int arg)
{
    return *(reinterpret_cast<L_Bmf **>(ll_check_udata(L, arg, LL_BMF)));
}

/**
 * \brief Push BMF user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param head pointer to the BMF
 * \return 1 Bmf* on the Lua stack
 */
int
ll_push_Bmf(lua_State *L, L_Bmf *bmf)
{
    if (!bmf)
        return ll_push_nil(L);
    return ll_push_udata(L, LL_BMF, bmf);
}

/**
 * \brief Create and push a new Bmf*
 *
 * Arg #1 is expected to be a string (dir)
 * Arg #2 is expected to be a l_int32 (fontsize)
 *
 * \param L pointer to the lua_State
 * \return 1 Bmf* on the Lua stack
 */
int
ll_new_Bmf(lua_State *L)
{
    const char* dir = lua_tostring(L, 1);
    l_int32 fontsize = ll_check_l_int32_default(__func__, L, 2, 6);
    L_Bmf *bmf = bmfCreate(dir, fontsize);
    return ll_push_Bmf(L, bmf);
}

/**
 * \brief Create a new Bmf*
 *
 * Arg #1 is expected to be a string (dir)
 * Arg #2 is expected to be a l_int32 (fontsize)
 *
 * \param L pointer to the lua_State
 * \return 1 Bmf* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Bmf(L);
}

/**
 * \brief Destroy a Bmf*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    L_Bmf **pbmf = reinterpret_cast<L_Bmf **>(ll_check_udata(L, 1, LL_BMF));
    DBG(LOG_DESTROY, "%s: '%s' pbmf=%p bmf=%p\n",
        __func__, LL_BMF, pbmf, *pbmf);
    bmfDestroy(pbmf);
    *pbmf = nullptr;
    return 0;
}

/**
 * \brief Get the Pix* for a character from the Bmf* (%bmf)
 *
 * Arg #1 is expected to be a Bmf* (bmf)
 * Arg #2 is expected to be a character (chr)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
GetPix(lua_State *L)
{
    L_Bmf *bmf = ll_check_Bmf(L, 1);
    char chr = ll_check_char(__func__, L, 2);
    Pix *pix = bmfGetPix(bmf, chr);
    return ll_push_Pix(L, pix);
}

/**
 * \brief Get the width for a character from the Bmf* (%bmf)
 *
 * Arg #1 is expected to be a Bmf* (bmf)
 * Arg #2 is expected to be a character (chr)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWidth(lua_State *L)
{
    L_Bmf *bmf = ll_check_Bmf(L, 1);
    char chr = ll_check_char(__func__, L, 2);
    l_int32 w = 0;
    if (bmfGetWidth(bmf, chr, &w))
        return ll_push_nil(L);
    lua_pushinteger(L, w);
    return 1;
}

/**
 * \brief Get the baseline for a character from the Bmf* (%bmf)
 *
 * Arg #1 is expected to be a Bmf* (bmf)
 * Arg #2 is expected to be a character (chr)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBaseline(lua_State *L)
{
    L_Bmf *bmf = ll_check_Bmf(L, 1);
    char chr = ll_check_char(__func__, L, 2);
    l_int32 baseline = 0;
    if (bmfGetBaseline(bmf, chr, &baseline))
        return ll_push_nil(L);
    lua_pushinteger(L, baseline);
    return 1;
}

/**
 * \brief Get the baseline for a character from the Bmf* (%bmf)
 *
 * Arg #1 is expected to be a Bmf* (bmf)
 * Arg #2 is expected to be a string (str)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetStringWidth(lua_State *L)
{
    L_Bmf *bmf = ll_check_Bmf(L, 1);
    const char* str = lua_tostring(L, 2);
    l_int32 w;
    if (bmfGetStringWidth(bmf, str, &w))
        return ll_push_nil(L);
    lua_pushinteger(L, w);
    return 1;
}

/**
 * \brief Get the line strings for a string using the Bmf* (%bmf)
 *
 * Arg #1 is expected to be a Bmf* (bmf)
 * Arg #2 is expected to be a string (str)
 * Arg #3 is expected to be a l_int32 (maxw)
 * Arg #4 is optional and, if given, expected to be a l_int32 (firstident)
 *
 * \param L pointer to the lua_State
 * \return 1 integer (h) plus a table of strings on the Lua stack
 */
static int
GetLineStrings(lua_State *L)
{
    L_Bmf *bmf = ll_check_Bmf(L, 1);
    const char* str = lua_tostring(L, 2);
    l_int32 maxw = ll_check_l_int32(__func__, L, 3);
    l_int32 firstident = ll_check_l_int32_default(__func__, L, 4, 0);
    l_int32 h = 0;
    SARRAY *sa = bmfGetLineStrings(bmf, str, maxw, firstident, &h);
    lua_pushinteger(L, h);
    return 1 + ll_push_sarray(L, sa);
}

/**
 * \brief Register the BMF methods and functions in the LL_BMF meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Bmf(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
        {"Destroy",             Destroy},
        {"GetPix",              GetPix},
        {"GetWidth",            GetWidth},
        {"GetBaseline",         GetBaseline},
        {"GetStringWidth",      GetStringWidth},
        {"GetLineStrings",      GetLineStrings},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_BMF, methods, functions);
    lua_setglobal(L, LL_BMF);
    return res;
}
