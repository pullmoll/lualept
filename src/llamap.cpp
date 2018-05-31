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

/** Define a function's name (_fun) with prefix LL_AMAP */
#define LL_FUNC(x) FUNC(LL_AMAP "." x)

/**
 * \brief Destroy a Amap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Amap **pamap = ll_check_udata<Amap>(_fun, L, 1, LL_AMAP);
    Amap *amap = *pamap;
    DBG(LOG_DESTROY, "%s: '%s' pamap=%p amap=%p size=%d\n", _fun,
        LL_AMAP, pamap, amap, l_amapSize(amap));
    l_amapDestroy(&amap);
    *pamap = nullptr;
    return 0;
}

/**
 * \brief Size of an Amap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
Size(lua_State *L)
{
    LL_FUNC("Size");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    ll_push_l_int32(_fun, L, l_amapSize(amap));
    return 1;
}

/**
 * \brief Create a new Amap*.
 * <pre>
 * Arg #1 is expected to be a string describing the key type (int,uint,float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Amap* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 keytype = ll_check_keytype(_fun, L, 1, L_INT_TYPE);
    Amap *amap = l_amapCreate(keytype);
    return ll_push_Amap(_fun, L, amap);
}

/**
 * \brief Insert a node into an Amap* (%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 * Arg #3 is expected to be a value (int, uint or float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Insert(lua_State *L)
{
    LL_FUNC("Insert");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    RB_TYPE key;
    RB_TYPE value;
    int result = FALSE;

    switch (amap->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        if (lua_isnil(L, 3)) {
            l_amapDelete(amap, key);
        } else {
            value.itype = ll_check_l_int64(_fun, L, 3);
            l_amapInsert(amap, key, value);
        }
        result = TRUE;
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        if (lua_isnil(L, 3)) {
            l_amapDelete(amap, key);
        } else {
            value.utype = ll_check_l_uint64(_fun, L, 3);
            l_amapInsert(amap, key, value);
        }
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        if (lua_isnil(L, 3)) {
            l_amapDelete(amap, key);
        } else {
            value.ftype = ll_check_l_float64(_fun, L, 3);
            l_amapInsert(amap, key, value);
        }
        result = TRUE;
        break;
    }
    return ll_push_boolean(_fun, L, result);
}

/**
 * \brief Printable string for a Amap*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Amap *amap = ll_check_Amap(_fun, L, 1);
    AmapNode *node = nullptr;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (!amap) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), LL_AMAP ": %p [%d: %s]",
                 reinterpret_cast<void *>(amap),
                 amap->keytype,
                 ll_string_keytype(amap->keytype));
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
                snprintf(str, sizeof(str), "    %" PRId64 " = %" PRId64,
                         static_cast<intptr_t>(node->key.itype),
                         static_cast<intptr_t>(node->value.itype));
                break;
            case L_UINT_TYPE:
                snprintf(str, sizeof(str), "    %" PRIu64 " = %" PRIu64,
                         static_cast<uintptr_t>(node->key.itype),
                         static_cast<uintptr_t>(node->value.itype));
                break;
            case L_FLOAT_TYPE:
                snprintf(str, sizeof(str), "    %g = %g",
                         node->key.ftype,
                         node->value.ftype);
                break;
            default:
                snprintf(str, sizeof(str), "    %p = %p",
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
 * \brief Delete a node from an Amap* (%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Delete(lua_State *L)
{
    LL_FUNC("Delete");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    RB_TYPE key;
    int result = FALSE;

    switch (amap->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        l_amapDelete(amap, key);
        result = TRUE;
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        l_amapDelete(amap, key);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        l_amapDelete(amap, key);
        result = TRUE;
        break;
    }
    return ll_push_boolean(_fun, L, result);
}

/**
 * \brief Find a key in an Amap* (%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * Arg #2 is expected to be a key (int, uint or float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 value on the Lua stack (either lua_Integer or lua_Number)
 */
static int
Find(lua_State *L)
{
    LL_FUNC("Find");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    RB_TYPE key;
    RB_TYPE *value;
    int result = 0;

    switch (amap->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        value = l_amapFind(amap, key);
        if (value) {
            result = ll_push_l_int64(_fun, L, value->itype);
        }
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        value = l_amapFind(amap, key);
        if (value) {
            result = ll_push_l_uint64(_fun, L, value->utype);
        }
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        value = l_amapFind(amap, key);
        if (value) {
            result = ll_push_l_float64(_fun, L, value->ftype);
        }
        break;
    }
    if (!result)
        return ll_push_nil(L);
    return result;
}

/**
 * \brief Get first node in an Amap* (%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetFirst(lua_State *L)
{
    LL_FUNC("GetFirst");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    AmapNode *node = l_amapGetFirst(amap);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get last node in an Amap* (%amap).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Amap* (amap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetLast(lua_State *L)
{
    LL_FUNC("GetLast");
    Amap *amap = ll_check_Amap(_fun, L, 1);
    AmapNode *node = l_amapGetLast(amap);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get next node of an AmapNode* (%node).
 * <pre>
 * Arg #1 is expected to be a AmapNode* (node).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetNext(lua_State *L)
{
    LL_FUNC("GetNext");
    /* XXX: deconstify */
    AmapNode *node = reinterpret_cast<AmapNode *>(reinterpret_cast<l_intptr_t>(lua_topointer(L, 2)));
    AmapNode *next = l_amapGetNext(node);
    lua_pushlightuserdata(L, next);
    return 1;
}

/**
 * \brief Get previous node of an AmapNode* (%node).
 * <pre>
 * Arg #1 is expected to be a AmapNode* (node).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetPrev(lua_State *L)
{
    LL_FUNC("GetPrev");
    /* XXX: deconstify */
    AmapNode *node = reinterpret_cast<AmapNode *>(reinterpret_cast<l_intptr_t>(lua_topointer(L, 2)));
    AmapNode *prev = l_amapGetPrev(node);
    lua_pushlightuserdata(L, prev);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_AMAP.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Amap* contained in the user data
 */
Amap *
ll_check_Amap(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Amap>(_fun, L, arg, LL_AMAP);
}

/**
 * \brief Optionally expect a LL_AMAP at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Amap* contained in the user data
 */
Amap *
ll_check_Amap_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Amap(_fun, L, arg);
}
/**
 * \brief Push Amap user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param amap pointer to the Amap
 * \return 1 Amap* on the Lua stack
 */
int
ll_push_Amap(const char *_fun, lua_State *L, Amap *amap)
{
    if (!amap)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_AMAP, amap);
}
/**
 * \brief Create and push a new Amap*.
 *
 * Arg #1 is expected to be a key type name (int, uint, or float).
 *
 * \param L pointer to the lua_State
 * \return 1 Amap* on the Lua stack
 */
int
ll_new_Amap(lua_State *L)
{
    FUNC("ll_new_Amap");
    l_int32 keytype = ll_check_keytype(_fun, L, 1, L_INT_TYPE);
    DBG(LOG_NEW_CLASS, "%s: create for %s = %s\n", _fun,
        "keytype", ll_string_keytype(keytype));
    Amap *amap = l_amapCreate(keytype);
    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        LL_AMAP, reinterpret_cast<void *>(amap));
    return ll_push_Amap(_fun, L, amap);
}

/**
 * \brief Register the Amap methods and functions in the LL_AMAP meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Amap(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},       /* garbage collector */
        {"__len",               Size},          /* #amap */
        {"__new",               ll_new_Amap},
        {"__newindex",          Insert},        /* amap[n] = value */
        {"__tostring",          toString},      /* print(amap) */
        {"Create",              Create},
        {"Delete",              Delete},
        {"Destroy",             Destroy},
        {"Find",                Find},
        {"GetFirst",            GetFirst},
        {"GetLast",             GetLast},
        {"GetNext",             GetNext},
        {"GetPrev",             GetPrev},
        {"Insert",              Insert},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"__index",             Find},
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_Amap);
    lua_setglobal(L, LL_AMAP);
    return ll_register_class(L, LL_AMAP, methods, functions);
}
