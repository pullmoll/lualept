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
 *  Lua class AMAP
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_AMAP
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the AMAP* contained in the user data
 */
L_AMAP *
ll_check_AMAP(lua_State *L, int arg)
{
    return *(L_AMAP **)ll_check_udata(L, arg, LL_AMAP);
}

/**
 * \brief Push AMAP user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param box pointer to the AMAP
 * \return 1 AMAP* on the Lua stack
 */
int
ll_push_AMAP(lua_State *L, L_AMAP *amap)
{
    if (NULL == amap)
        return 0;
    return ll_push_udata(L, LL_AMAP, amap);
}

/**
 * \brief Create and push a new AMAP*
 *
 * Arg #1 is expected to be a key type name (int, uint, or float)
 *
 * \param L pointer to the lua_State
 * \return 1 AMAP* on the Lua stack
 */
int
ll_new_AMAP(lua_State *L)
{
    l_int32 keytype = ll_check_keytype(L, 1, L_INT_TYPE);
    L_AMAP *amap = l_amapCreate(keytype);
    return ll_push_AMAP(L, amap);
}

/**
 * @brief Printable string for a AMAP*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    L_AMAP *amap = ll_check_AMAP(L, 1);
    L_AMAP_NODE *node = NULL;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (NULL == amap) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addstring(&B, ll_string_amap_type(amap->keytype));
        luaL_addstring(&B, ": {");
        node = l_amapSize(amap) ? l_amapGetFirst(amap) : NULL;
        while (node) {
            if (first) {
                first = 0;
            } else {
                luaL_addchar(&B, ',');
            }
            switch (amap->keytype) {
            case L_INT_TYPE:
                snprintf(str, sizeof(str), "%lld=%lld", node->key.itype, node->value.itype);
                break;
            case L_UINT_TYPE:
                snprintf(str, sizeof(str), "%llu=%llu", node->key.utype, node->value.utype);
                break;
            case L_FLOAT_TYPE:
                snprintf(str, sizeof(str), "%g=%g", node->key.ftype, node->value.ftype);
                break;
            default:
                snprintf(str, sizeof(str), "%p=%p", node->key.ptype, node->value.ptype);
                break;
            }
            luaL_addstring(&B, str);
            node = l_amapGetNext(node);
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new AMAP*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float)
 *
 * \param L pointer to the lua_State
 * \return 1 AMAP* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_AMAP(L);
}

/**
 * \brief Destroy a AMAP*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pamap = ll_check_udata(L, 1, LL_AMAP);
    DBG(LOG_DESTROY, "%s: '%s' pamap=%p amap=%p size=%d\n", __func__,
        LL_AMAP, (void *)pamap, *pamap, l_amapSize(*(L_AMAP **)pamap));
    l_amapDestroy((L_AMAP **)pamap);
    *pamap = NULL;
    return 0;
}

/**
 * \brief Size of an ASET*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float)
 *
 * \param L pointer to the lua_State
 * \return 1 ASET* on the Lua stack
 */
static int
Size(lua_State *L)
{
    L_AMAP *amap = ll_check_AMAP(L, 1);
    lua_pushinteger(L, l_amapSize(amap));
    return 1;
}

/**
 * \brief Insert a node into an AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a AMAP* (amap)
 * Arg #2 is expected to be a key (int, uint or float)
 * Arg #3 is expected to be a value (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Insert(lua_State *L)
{
    L_AMAP *amap = ll_check_AMAP(L, 1);
    RB_TYPE key, value;
    int isnum;
    int result = FALSE;

    switch (amap->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointegerx(L, 2, &isnum);
        if (isnum) {
            if (lua_isnil(L, 3)) {
                l_amapDelete(amap, key);
            } else {
                value.itype = lua_tointeger(L, 3);
                l_amapInsert(amap, key, value);
            }
            result = TRUE;
        } else {
            lua_pushfstring(L, "AMAP key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumberx(L, 2, &isnum);
        if (isnum) {
            if (lua_isnil(L, 3)) {
                l_amapDelete(amap, key);
            } else {
                value.ftype = lua_tonumber(L, 3);
                l_amapInsert(amap, key, value);
            }
            result = TRUE;
        } else {
            lua_pushfstring(L, "AMAP key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    }
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Delete a node from an AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a AMAP* (amap)
 * Arg #2 is expected to be a key (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Delete(lua_State *L)
{
    L_AMAP *amap = ll_check_AMAP(L, 1);
    RB_TYPE key;
    int result = FALSE;

    switch (amap->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointeger(L, 2);
        l_amapDelete(amap, key);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumber(L, 2);
        l_amapDelete(amap, key);
        result = TRUE;
        break;
    }
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Find a key in an AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a AMAP* (amap)
 * Arg #2 is expected to be a key (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 1 value on the Lua stack (either lua_Integer or lua_Number)
 */
static int
Find(lua_State *L)
{
    L_AMAP *amap = ll_check_AMAP(L, 1);
    RB_TYPE key;
    RB_TYPE *value;
    int result = 0;

    switch (amap->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointeger(L, 2);
        value = l_amapFind(amap, key);
        if (NULL != value) {
            lua_pushinteger(L, value->itype);
            result++;
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumber(L, 2);
        value = l_amapFind(amap, key);
        if (NULL != value) {
            lua_pushnumber(L, value->ftype);
            result++;
        }
        break;
    }
    return result;
}

/**
 * \brief Register the AMAP methods and functions in the LL_AMAP meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_AMAP(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__len",               Size},
        {"__new",               Create},
        {"__newindex",          Insert},
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"Insert",              Insert},
        {"Delete",              Delete},
        {"Find",                Find},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_AMAP, methods, functions);
    lua_setglobal(L, LL_AMAP);
    return res;
}
