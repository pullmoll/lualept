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
 *  Lua class Amap
 *
 *====================================================================*/

/**
 * \brief Printable string for a L_AMAP*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_ASET ".toString");
    static char str[256];
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
    L_AMAP_NODE *node = nullptr;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (!amap) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), "[%d: %s] L_AMAP* %p",
                 amap->keytype,
                 ll_string_keytype(amap->keytype),
                 reinterpret_cast<void *>(amap));
        luaL_addstring(&B, str);
        node = l_amapSize(amap) ? l_amapGetFirst(amap) : nullptr;
        while (node) {
            if (first) {
                first = 0;
                luaL_addstring(&B, "\n");
            } else {
                luaL_addstring(&B, ",\n");
            }
            switch (amap->keytype) {
            case L_INT_TYPE:
                snprintf(str, sizeof(str), "    %" PRId64 "\t= %" PRId64,
                         static_cast<intptr_t>(node->key.itype),
                         static_cast<intptr_t>(node->value.itype));
                break;
            case L_UINT_TYPE:
                snprintf(str, sizeof(str), "    %" PRIu64 "\t= %" PRIu64,
                         static_cast<uintptr_t>(node->key.itype),
                         static_cast<uintptr_t>(node->value.itype));
                break;
            case L_FLOAT_TYPE:
                snprintf(str, sizeof(str), "    %g\t= %g",
                         node->key.ftype,
                         node->value.ftype);
                break;
            default:
                snprintf(str, sizeof(str), "    %p\t= %p",
                         node->key.ptype,
                         node->value.ptype);
            }
            luaL_addstring(&B, str);
            node = l_amapGetNext(node);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new L_AMAP*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float).
 *
 * \param L pointer to the lua_State
 * \return 1 L_AMAP* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_AMAP ".Create");
    l_int32 keytype = ll_check_keytype(_fun, L, 1, L_INT_TYPE);
    L_AMAP *amap = l_amapCreate(keytype);
    return ll_push_Amap(_fun, L, amap);
}

/**
 * \brief Destroy a L_AMAP*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_AMAP ".Destroy");
    L_Rbtree **pamap = reinterpret_cast<L_Rbtree **>(ll_check_udata(_fun, L, 1, LL_AMAP));
    L_Rbtree *amap = *pamap;
    DBG(LOG_DESTROY, "%s: '%s' pamap=%p amap=%p size=%d\n", _fun,
        LL_AMAP, pamap, amap, l_amapSize(amap));
    l_amapDestroy(&amap);
    *pamap = nullptr;
    return 0;
}

/**
 * \brief Size of an AMAP*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
Size(lua_State *L)
{
    FUNC(LL_AMAP ".Size");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
    lua_pushinteger(L, l_amapSize(amap));
    return 1;
}

/**
 * \brief Insert a node into an L_AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a L_AMAP* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 * Arg #3 is expected to be a value (int, uint or float).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Insert(lua_State *L)
{
    FUNC(LL_AMAP ".Insert");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
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
            lua_pushfstring(L, LL_AMAP " key is not a number: '%s'", lua_tostring(L, 2));
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
            lua_pushfstring(L, LL_AMAP " key is not a number: '%s'", lua_tostring(L, 2));
            lua_error(L);
        }
        break;
    }
    lua_pushboolean(L, result);
    return 1;
}

/**
 * \brief Delete a node from an L_AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a L_AMAP* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Delete(lua_State *L)
{
    FUNC(LL_AMAP ".Delete");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
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
 * \brief Find a key in an L_AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a L_AMAP* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 *
 * \param L pointer to the lua_State
 * \return 1 value on the Lua stack (either lua_Integer or lua_Number)
 */
static int
Find(lua_State *L)
{
    FUNC(LL_AMAP ".Find");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
    RB_TYPE key;
    RB_TYPE *value;
    int result = 0;

    switch (amap->keytype) {
    case L_INT_TYPE:
    case L_UINT_TYPE:
        key.itype = lua_tointeger(L, 2);
        value = l_amapFind(amap, key);
        if (nullptr != value) {
            lua_pushinteger(L, value->itype);
            result++;
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = lua_tonumber(L, 2);
        value = l_amapFind(amap, key);
        if (nullptr != value) {
            lua_pushnumber(L, value->ftype);
            result++;
        }
        break;
    }
    return result;
}

/**
 * \brief Get first node in an L_AMAP* (%amap)
 *
 * Arg #1 (i.e. self) is expected to be a L_AMAP* (amap).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetFirst(lua_State *L)
{
    FUNC(LL_AMAP ".GetFirst");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
    L_AMAP_NODE *node = l_amapGetFirst(amap);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get next node of an L_AMAP_NODE* (%node)
 *
 * Arg #1 is expected to be a L_AMAP_NODE* (node).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetNext(lua_State *L)
{
    FUNC(LL_AMAP ".GetNext");
    /* HACK: deconstify */
    L_AMAP_NODE *node = reinterpret_cast<L_AMAP_NODE *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    L_AMAP_NODE *next = l_amapGetNext(node);
    lua_pushlightuserdata(L, next);
    return 1;
}

/**
 * \brief Get previous node of an L_AMAP_NODE* (%node)
 *
 * Arg #1 is expected to be a L_AMAP_NODE* (node).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetPrev(lua_State *L)
{
    FUNC(LL_AMAP ".GetPrev");
    /* HACK: deconstify */
    L_AMAP_NODE *node = reinterpret_cast<L_AMAP_NODE *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    L_AMAP_NODE *prev = l_amapGetPrev(node);
    lua_pushlightuserdata(L, prev);
    return 1;
}

/**
 * \brief Get last node in an L_AMAP* (%amap)
 *
 * Arg #1 is expected to be a L_AMAP* (amap).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetLast(lua_State *L)
{
    FUNC(LL_AMAP ".GetLast");
    L_AMAP *amap = ll_check_Amap(_fun, L, 1);
    L_AMAP_NODE *node = l_amapGetLast(amap);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_AMAP
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_AMAP* contained in the user data
 */
L_AMAP *
ll_check_Amap(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<L_AMAP **>(ll_check_udata(_fun, L, arg, LL_AMAP)));
}

/**
 * \brief Optionally expect a LL_AMAP at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the L_AMAP* contained in the user data
 */
L_AMAP *
ll_check_Amap_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Amap(_fun, L, arg);
}

/**
 * \brief Push L_AMAP user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param amap pointer to the L_AMAP
 * \return 1 L_AMAP* on the Lua stack
 */
int
ll_push_Amap(const char *_fun, lua_State *L, L_AMAP *amap)
{
    if (!amap)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_AMAP, amap);
}

/**
 * \brief Create and push a new L_AMAP*
 *
 * Arg #1 is expected to be a key type name (int, uint, or float).
 *
 * \param L pointer to the lua_State
 * \return 1 L_AMAP* on the Lua stack
 */
int
ll_new_Amap(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the L_AMAP methods and functions in the LL_AMAP meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Amap(lua_State *L)
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

    return ll_register_class(L, LL_AMAP, methods, functions);
}
