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
 *  Lua class ASET
 *
 *====================================================================*/

/**
 * @brief Printable string for a ASET*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_ASET ".toString");
    static char str[256];
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
    L_ASET_NODE *node = nullptr;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (!aset) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addstring(&B, ll_string_keytype(aset->keytype));
        luaL_addstring(&B, ": {");
        node = l_asetSize(aset) ? l_asetGetFirst(aset) : nullptr;
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
    FUNC(LL_ASET ".Create");
    l_int32 keytype = ll_check_keytype(_fun, L, 1, L_INT_TYPE);
    L_ASET *aset = l_asetCreate(keytype);
    return ll_push_Aset(_fun, L, aset);
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
    FUNC(LL_ASET ".Size");
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
    lua_pushinteger(L, l_asetSize(aset));
    return 1;
}

/**
 * \brief Destroy an ASET*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_ASET ".Destroy");
    L_Rbtree **paset = reinterpret_cast<L_Rbtree **>(ll_check_udata(_fun, L, 1, LL_ASET));
    DBG(LOG_DESTROY, "%s: '%s' paset=%p aset=%p size=%d\n", _fun,
        LL_ASET, paset, *paset, l_asetSize(*paset));
    l_asetDestroy(paset);
    *paset = nullptr;
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
    FUNC(LL_ASET ".Insert");
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
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
            lua_pushfstring(L, LL_ASET " key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumberx(L, 2, &isnum);
        if (isnum) {
            l_asetInsert(aset, key);
            result = TRUE;
        } else {
            lua_pushfstring(L, LL_ASET " key is not a number: '%s'", lua_tostring(L, 2));
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
    FUNC(LL_ASET ".Delete");
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
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
    FUNC(LL_ASET ".Find");
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
    RB_TYPE key;
    RB_TYPE *value = nullptr;

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
    lua_pushboolean(L, nullptr != value);
    return 1;
}

/**
 * \brief Get first node in an L_ASET* (%aset)
 *
 * Arg #1 (i.e. self) is expected to be a L_ASET* (aset)
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetFirst(lua_State *L)
{
    FUNC(LL_ASET ".GetFirst");
    L_ASET *aset = ll_check_Aset(_fun, L, 1);
    L_ASET_NODE *node = l_asetGetFirst(aset);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get next node of L_ASET_NODE* (%node)
 *
 * Arg #1 is expected to be a L_ASET_NODE* (node)
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetNext(lua_State *L)
{
    FUNC(LL_ASET ".GetNext");
    /* HACK: deconstify */
    L_ASET_NODE *node = reinterpret_cast<L_ASET_NODE *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    L_ASET_NODE *next = l_asetGetNext(node);
    lua_pushlightuserdata(L, next);
    return 1;
}

/**
 * \brief Get previous node of L_ASET_NODE* (%node)
 *
 * Arg #1 (i.e. self) is expected to be a L_ASET_NODE* (node)
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetPrev(lua_State *L)
{
    FUNC(LL_ASET ".GetPrev");
    /* HACK: deconstify */
    L_ASET_NODE *node = reinterpret_cast<L_ASET_NODE *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    L_ASET_NODE *prev = l_asetGetPrev(node);
    lua_pushlightuserdata(L, prev);
    return 1;
}

/**
 * \brief Get last node in an L_ASET* (%aset)
 *
 * Arg #1 (i.e. self) is expected to be a L_ASET* (aset)
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetLast(lua_State *L)
{
    FUNC(LL_ASET ".GetLast");
    L_ASET *aset = ll_check_Amap(_fun, L, 1);
    L_ASET_NODE *node = l_asetGetLast(aset);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * @brief Check Lua stack at index %arg for udata of class LL_ASET
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the ASET* contained in the user data
 */
L_ASET *
ll_check_Aset(const char* _fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<L_ASET **>(ll_check_udata(_fun, L, arg, LL_ASET)));
}

/**
 * \brief Push ASET user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param aset pointer to the ASET
 * \return 1 ASET* on the Lua stack
 */
int
ll_push_Aset(const char* _fun, lua_State *L, L_ASET *aset)
{
    if (!aset)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_ASET, aset);
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
ll_new_Aset(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the ASET methods and functions in the LL_ASET meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Aset(lua_State *L)
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
        {"GetFirst",            GetFirst},
        {"GetLast",             GetLast},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        {"GetNext",             GetNext},
        {"GetPrev",             GetPrev},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_ASET, methods, functions);
    lua_setglobal(L, LL_ASET);
    return res;
}
