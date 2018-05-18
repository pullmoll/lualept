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
 *  Lua class BOX
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_BOX
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Box* contained in the user data
 */
Box *
ll_check_Box(lua_State *L, int arg)
{
    return *(reinterpret_cast<Box **>(ll_check_udata(L, arg, LL_BOX)));
}

/**
 * \brief Push BOX user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param box pointer to the BOX
 * \return 1 Box* on the Lua stack
 */
int
ll_push_Box(lua_State *L, Box *box)
{
    if (!box)
        return 0;
    return ll_push_udata(L, LL_BOX, box);
}

/**
 * \brief Create and push a new Box*
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
int
ll_new_Box(lua_State *L)
{
    l_int32 x = ll_check_l_int32_default(__func__, L, 1, 0);
    l_int32 y = ll_check_l_int32_default(__func__, L, 2, 0);
    l_int32 w = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 h = ll_check_l_int32_default(__func__, L, 4, 1);
    Box *box = boxCreate(x, y, w, h);
    return ll_push_Box(L, box);
}

/**
 * @brief Printable string for a Box*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    Box *box = ll_check_Box(L, 1);
    luaL_Buffer B;
    l_int32 x, y, w, h;

    luaL_buffinit(L, &B);
    if (!box) {
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
 * \brief Create a new Box*
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Box(L);
}

/**
 * \brief Create a new Box* if the parameters are valid
 *
 * Arg #1 is expected to be a l_int32 (x)
 * Arg #2 is expected to be a l_int32 (y)
 * Arg #3 is expected to be a l_int32 (w)
 * Arg #4 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
CreateValid(lua_State *L)
{
    l_int32 x, y, w, h;
    Box *box;

    x = ll_check_l_int32_default(__func__, L, 1, 0);
    y = ll_check_l_int32_default(__func__, L, 2, 0);
    w = ll_check_l_int32_default(__func__, L, 3, 1);
    h = ll_check_l_int32_default(__func__, L, 4, 1);
    box = boxCreateValid(x, y, w, h);
    return ll_push_Box(L, box);
}

/**
 * \brief Copy a Box*
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for BOX on the Lua stack
 */
static int
Copy(lua_State *L)
{
    Box *boxs, *box;
    boxs = ll_check_Box(L, 1);
    box = boxCopy(boxs);
    return ll_push_Box(L, box);
}

/**
 * \brief Clone a Box*
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for BOX on the Lua stack
 */
static int
Clone(lua_State *L)
{
    Box *boxs, *box;
    boxs = ll_check_Box(L, 1);
    box = boxClone(boxs);
    return ll_push_Box(L, box);
}

/**
 * \brief Destroy a Box*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    Box **pbox = reinterpret_cast<Box **>(ll_check_udata(L, 1, LL_BOX));
    DBG(LOG_DESTROY, "%s: '%s' pbox=%p box=%p refcount=%d\n", __func__,
        LL_BOX, pbox, *pbox, boxGetRefcount(*pbox));
    boxDestroy(pbox);
    *pbox = nullptr;
    return 0;
}

/**
 * \brief Get the Box* geometry
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 4 for four integers (or nil on error) on the stack
 */
static int
GetGeometry(lua_State *L)
{
    Box *box = ll_check_Box(L, 1);
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
 * Arg #1 (i.e. self) is expected to be a Box* user data
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
    Box *box = ll_check_Box(L, 1);
    l_int32 x = ll_check_l_int32_default(__func__, L, 2, 0);
    l_int32 y = ll_check_l_int32_default(__func__, L, 3, 0);
    l_int32 w = ll_check_l_int32_default(__func__, L, 4, 1);
    l_int32 h = ll_check_l_int32_default(__func__, L, 5, 1);
    lua_pushboolean(L, 0 == boxSetGeometry(box, x, y, w, h));
    return 1;
}

/**
 * \brief Get the BOX side locations (left, right, top, bottom)
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 4 for four integers (or nil on error) on the stack
 */
static int
GetSideLocations(lua_State *L)
{
    Box *box = ll_check_Box(L, 1);
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
 * Arg #1 (i.e. self) is expected to be a Box* user data
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
    Box *box = ll_check_Box(L, 1);
    l_int32 l = ll_check_l_int32_default(__func__, L, 2, 0);
    l_int32 r = ll_check_l_int32_default(__func__, L, 3, 0);
    l_int32 t = ll_check_l_int32_default(__func__, L, 4, 0);
    l_int32 b = ll_check_l_int32_default(__func__, L, 5, 0);
    lua_pushboolean(L, 0 == boxSetSideLocations(box, l, r, t, b));
    return 1;
}

/**
 * \brief Get the Box* reference count
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetRefcount(lua_State *L)
{
    Box *box = ll_check_Box(L, 1);
    lua_pushinteger(L, boxGetRefcount(box));
    return 1;
}

/**
 * \brief Change the Box* reference count
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 * Arg #1 (i.e. self) is expected to be a l_int32 (delta)
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
ChangeRefcount(lua_State *L)
{
    Box *box = ll_check_Box(L, 1);
    l_int32 delta = ll_check_l_int32(__func__, L, 2);
    lua_pushboolean(L, 0 == boxChangeRefcount(box, delta));
    return 1;
}

/**
 * \brief Check if a Box* is valid
 *
 * Arg #1 (i.e. self) is expected to be a Box* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
IsValid(lua_State *L)
{
    Box *box = ll_check_Box(L, 1);
    l_int32 valid = 0;
    if (boxIsValid(box, &valid))
        return 0;
    lua_pushboolean(L, valid);
    return 1;
}

/**
 * \brief Check if a Box* (%box1) contains another Box* (%box2)
 *
 * Arg #1 (i.e. self) is expected to be a Box* (box1)
 * Arg #2 is expected to be another Box* (box2)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
Contains(lua_State *L)
{
    Box *box1 = ll_check_Box(L, 1);
    Box *box2 = ll_check_Box(L, 2);
    l_int32 result = 0;
    if (boxContains(box1, box2, &result))
        return 0;
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Check if a Box* (%box1) intersects another Box* (%box2)
 *
 * Arg #1 (i.e. self) is expected to be a Box* (box1)
 * Arg #2 is expected to be another Box* (box2)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
Intersects(lua_State *L)
{
    Box *box1 = ll_check_Box(L, 1);
    Box *box2 = ll_check_Box(L, 2);
    l_int32 result = 0;
    if (boxIntersects(box1, box2, &result))
        return 0;
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Get the overlap region of a Box* (%box1) and another Box* (%box2)
 *
 * Arg #1 (i.e. self) is expected to be a Box* (box1)
 * Arg #2 is expected to be another Box* (box2)
 *
 * \param L pointer to the lua_State
 * \return 1 for Box* on the Lua stack
 */
static int
OverlapRegion(lua_State *L)
{
    Box *box1 = ll_check_Box(L, 1);
    Box *box2 = ll_check_Box(L, 2);
    Box *box = boxOverlapRegion(box1, box2);
    ll_push_Box(L, box);
    return 1;
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Box(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},    /* new Box */
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
        {"Contains",            Contains},
        {"Intersects",          Intersects},
        {"OverlapRegion",       OverlapRegion},
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
