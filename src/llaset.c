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
 *  Lua class ASET
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_ASET
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the ASET* contained in the user data
 */
L_ASET *
ll_check_ASET(lua_State *L, int arg)
{
    return *(L_ASET **)ll_check_udata(L, arg, LL_ASET);
}

/**
 * \brief Push ASET user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param box pointer to the ASET
 * \return 1 ASET* on the Lua stack
 */
int
ll_push_ASET(lua_State *L, L_ASET *aset)
{
    if (NULL == aset)
        return 0;
    return ll_push_udata(L, LL_ASET, aset);
}

/**
 * \brief Create and push a new ASET*
 *
 * Arg #1 is expected to be a key type name (int, uint, or float)
 *
 * \param L pointer to the lua_State
 * \return 1 ASET* on the Lua stack
 */
int
ll_new_ASET(lua_State *L)
{
    l_int32 keytype = ll_check_keytype(L, 1, L_INT_TYPE);
    L_ASET *aset = l_asetCreate(keytype);
    return ll_push_ASET(L, aset);
}

/**
 * @brief Printable string for a ASET*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    L_ASET *aset = ll_check_ASET(L, 1);
    L_ASET_NODE *node = NULL;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (NULL == aset) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addstring(&B, ll_string_aset_type(aset->keytype));
        luaL_addstring(&B, ": {");
        node = l_asetSize(aset) ? l_asetGetFirst(aset) : NULL;
        while (node) {
            if (first) {
                first = 0;
            } else {
                luaL_addchar(&B, ',');
            }
            switch (aset->keytype) {
            case L_INT_TYPE:
                snprintf(str, sizeof(str), "%lld", node->key.itype);
                break;
            case L_UINT_TYPE:
                snprintf(str, sizeof(str), "%llu", node->key.utype);
                break;
            case L_FLOAT_TYPE:
                snprintf(str, sizeof(str), "%g", node->key.ftype);
                break;
            default:
                snprintf(str, sizeof(str), "%p", node->key.ptype);
                break;
            }
            luaL_addstring(&B, str);
            node = l_asetGetNext(node);
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new ASET*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float)
 *
 * \param L pointer to the lua_State
 * \return 1 ASET* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_ASET(L);
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
    L_ASET *aset = ll_check_ASET(L, 1);
    lua_pushinteger(L, l_asetSize(aset));
    return 1;
}

/**
 * \brief Destroy a ASET*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **paset = ll_check_udata(L, 1, LL_ASET);
    DBG(LOG_DESTROY, "%s: '%s' paset=%p aset=%p size=%d\n", __func__,
        LL_ASET, (void *)paset, *paset, l_asetSize(*(L_ASET **)paset));
    l_asetDestroy((L_ASET **)paset);
    *paset = NULL;
    return 0;
}

/**
 * \brief Insert a node into an ASET* (%aset)
 *
 * Arg #1 (i.e. self) is expected to be a ASET* (aset)
 * Arg #2 is expected to be a key (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Insert(lua_State *L)
{
    L_ASET *aset = ll_check_ASET(L, 1);
    RB_TYPE key;
    int isnum = 0;
    int result = FALSE;

    switch (aset->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointegerx(L, 2, &isnum);
        if (isnum) {
            l_asetInsert(aset, key);
            result = TRUE;
        } else {
            lua_pushfstring(L, "ASET key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumberx(L, 2, &isnum);
        if (isnum) {
            l_asetInsert(aset, key);
            result = TRUE;
        } else {
            lua_pushfstring(L, "ASET key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    }
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Delete a node from an ASET* (%aset)
 *
 * Arg #1 (i.e. self) is expected to be a ASET* (aset)
 * Arg #2 is expected to be a key (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Delete(lua_State *L)
{
    L_ASET *aset = ll_check_ASET(L, 1);
    RB_TYPE key;
    int result = FALSE;

    switch (aset->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointeger(L, 2);
        l_asetDelete(aset, key);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumber(L, 2);
        l_asetDelete(aset, key);
        result = TRUE;
        break;
    }
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Find a key in an ASET* (%aset)
 *
 * Arg #1 (i.e. self) is expected to be a ASET* (aset)
 * Arg #2 is expected to be a key (int, uint or float)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Find(lua_State *L)
{
    L_ASET *aset = ll_check_ASET(L, 1);
    RB_TYPE key;
    RB_TYPE *value = NULL;

    switch (aset->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointeger(L, 2);
        value = l_asetFind(aset, key);
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumber(L, 2);
        value = l_asetFind(aset, key);
        break;
    }
    lua_pushboolean(L, NULL != value);
    return 1;
}

/**
 * \brief Register the ASET methods and functions in the LL_ASET meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_ASET(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__len",               Size},
        {"__new",               Create},
        {"__newindex",          Insert},
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"Size",                Size},
        {"Insert",              Insert},
        {"Delete",              Delete},
        {"Find",                Find},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_ASET, methods, functions);
    lua_setglobal(L, LL_ASET);
    return res;
}
