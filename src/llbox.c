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

#include "llept.h"
#include <lauxlib.h>
#include <lualib.h>

/*====================================================================*
 *
 *  Lua class BOX
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_BOX
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the BOX* contained in the user data
 */
BOX *
ll_check_BOX(lua_State *L, int arg)
{
    return *(BOX **)ll_check_udata(L, arg, LL_BOX);
}

/**
 * \brief Push BOX user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param box pointer to the BOX
 * \return 1 BOX* on the Lua stack
 */
int
ll_push_BOX(lua_State *L, BOX *box)
{
    if (NULL == box)
        return 0;
    return ll_push_udata(L, LL_BOX, box);
}

/**
 * \brief Create and push a new BOX*
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack
 */
int
ll_new_BOX(lua_State *L)
{
    l_int32 x = ll_check_l_int32_default(L, 1, 0);
    l_int32 y = ll_check_l_int32_default(L, 2, 0);
    l_int32 w = ll_check_l_int32_default(L, 3, 1);
    l_int32 h = ll_check_l_int32_default(L, 4, 1);
    BOX *box = boxCreate(x, y, w, h);
    return ll_push_BOX(L, box);
}

/**
 * @brief Printable string for a BOX*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    BOX *box = ll_check_BOX(L, 1);
    luaL_Buffer B;
    l_int32 x, y, w, h;

    luaL_buffinit(L, &B);
    if (NULL == box) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        if (boxGetGeometry(box, &x, &y, &w, &h)) {
            snprintf(str, sizeof(str), "invalid");
        } else {
            snprintf(str, sizeof(str), "%d,%d,%d,%d", x, y, w, h);
        }
        luaL_addstring(&B, str);
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new BOX*
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_BOX(L);
}

/**
 * \brief Create a new BOX* if the parameters are valid
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack
 */
static int
CreateValid(lua_State *L)
{
    l_int32 x, y, w, h;
    BOX *box;

    x = ll_check_l_int32_default(L, 1, 0);
    y = ll_check_l_int32_default(L, 2, 0);
    w = ll_check_l_int32_default(L, 3, 1);
    h = ll_check_l_int32_default(L, 4, 1);
    box = boxCreateValid(x, y, w, h);
    return ll_push_BOX(L, box);
}

/**
 * \brief Copy a BOX*
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for BOX on the Lua stack
 */
static int
Copy(lua_State *L)
{
    BOX *boxs, *box;
    boxs = ll_check_BOX(L, 1);
    box = boxCopy(boxs);
    return ll_push_BOX(L, box);
}

/**
 * \brief Clone a BOX*
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for BOX on the Lua stack
 */
static int
Clone(lua_State *L)
{
    BOX *boxs, *box;
    boxs = ll_check_BOX(L, 1);
    box = boxClone(boxs);
    return ll_push_BOX(L, box);
}

/**
 * \brief Destroy a BOX*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pbox = ll_check_udata(L, 1, LL_BOX);
    DBG(LOG_DESTROY, "%s: '%s' pbox=%p box=%p refcount=%d\n", __func__,
        LL_BOX, (void *)pbox, *pbox, boxGetRefcount(*(BOX **)pbox));
    boxDestroy((BOX **)pbox);
    *pbox = NULL;
    return 0;
}

/**
 * \brief Get the BOX* geometry
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 4 for four integers (or nil on error) on the stack
 */
static int
GetGeometry(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 x, y, w, h;
    if (boxGetGeometry(box, &x, &y, &w, &h))
        return 0;
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Set the BOX geometry
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 * Arg #2 is expected to be a lua_Integer (x)
 * Arg #3 is expected to be a lua_Integer (y)
 * Arg #4 is expected to be a lua_Integer (w)
 * Arg #5 is expected to be a lua_Integer (h)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean result true or false
 */
static int
SetGeometry(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 x = ll_check_l_int32_default(L, 2, 0);
    l_int32 y = ll_check_l_int32_default(L, 3, 0);
    l_int32 w = ll_check_l_int32_default(L, 4, 1);
    l_int32 h = ll_check_l_int32_default(L, 5, 1);
    lua_pushboolean(L, 0 == boxSetGeometry(box, x, y, w, h));
    return 1;
}

/**
 * \brief Get the BOX side locations (left, right, top, bottom)
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 4 for four integers (or nil on error) on the stack
 */
static int
GetSideLocations(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 l, r, t, b;
    if (boxGetSideLocations(box, &l, &r, &t, &b))
        return 0;
    lua_pushinteger(L, l);
    lua_pushinteger(L, r);
    lua_pushinteger(L, t);
    lua_pushinteger(L, b);
    return 4;
}

/**
 * \brief Set the BOX side locations (left, right, top, bottom)
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 * Arg #2 is expected to be a lua_Integer (l)
 * Arg #3 is expected to be a lua_Integer (r)
 * Arg #4 is expected to be a lua_Integer (t)
 * Arg #5 is expected to be a lua_Integer (b)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean result true or false
 */
static int
SetSideLocations(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 l = ll_check_l_int32_default(L, 2, 0);
    l_int32 r = ll_check_l_int32_default(L, 3, 0);
    l_int32 t = ll_check_l_int32_default(L, 4, 0);
    l_int32 b = ll_check_l_int32_default(L, 5, 0);
    lua_pushboolean(L, 0 == boxSetSideLocations(box, l, r, t, b));
    return 1;
}

/**
 * \brief Get the BOX* reference count
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetRefcount(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    lua_pushinteger(L, boxGetRefcount(box));
    return 1;
}

/**
 * \brief Change the BOX* reference count
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 * Arg #1 (i.e. self) is expected to be a l_int32 (delta)
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
ChangeRefcount(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 delta = ll_check_l_int32(L, 2);
    lua_pushboolean(L, 0 == boxChangeRefcount(box, delta));
    return 1;
}

/**
 * \brief Check if a BOX* is valid
 *
 * Arg #1 (i.e. self) is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
IsValid(lua_State *L)
{
    BOX *box = ll_check_BOX(L, 1);
    l_int32 valid = 0;
    if (boxIsValid(box, &valid))
        return 0;
    lua_pushboolean(L, valid);
    return 1;
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_BOX(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},    /* new BOX */
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"Copy",                Copy},
        {"Clone",               Clone},
        {"GetGeometry",         GetGeometry},
        {"SetGeometry",         SetGeometry},
        {"GetSideLocations",    GetSideLocations},
        {"SetSideLocations",    SetSideLocations},
        {"GetRefcount",         GetRefcount},
        {"ChangeRefcount",      ChangeRefcount},
        {"IsValid",             IsValid},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        {"CreateValid",         CreateValid},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_BOX, methods, functions);
    lua_setglobal(L, LL_BOX);
    return res;
}
