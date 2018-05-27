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
 *  Lua class Aset
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_ASET */
#define LL_FUNC(x) FUNC(LL_ASET "." x)

/**
 * \brief Destroy an Aset*.
 * <pre>
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    L_Rbtree **paset = ll_check_udata<L_Rbtree>(_fun, L, 1, LL_ASET);
    L_Rbtree *aset = *paset;
    DBG(LOG_DESTROY, "%s: '%s' paset=%p aset=%p size=%d\n", _fun,
        LL_ASET, paset, aset, l_asetSize(aset));
    l_asetDestroy(&aset);
    *paset = nullptr;
    return 0;
}

/**
 * \brief Size of an Aset*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Aset* on the Lua stack
 */
static int
Size(lua_State *L)
{
    LL_FUNC("Size");
    Aset *aset = ll_check_Aset(_fun, L, 1);
    lua_pushinteger(L, l_asetSize(aset));
    return 1;
}

/**
 * \brief Create a new Aset*.
 * <pre>
 * Arg #1 is expected to be a string describing the key type (int,uint,float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Aset* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 keytype = ll_check_keytype(_fun, L, 1, L_INT_TYPE);
    Aset *aset = l_asetCreate(keytype);
    return ll_push_Aset(_fun, L, aset);
}

/**
 * \brief Insert a node into an Aset* (%aset).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 * Arg #2 is expected to be a key (int, uint or float).
 * Arg #3 is optional and, if given, expected to be a boolean (value).
 *
 * Note: if Arg #3 is false, the node is deleted instead of inserted.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Insert(lua_State *L)
{
    LL_FUNC("Insert");
    Aset *aset = ll_check_Aset(_fun, L, 1);
    l_int32 value = ll_check_boolean_default(_fun, L, 3, TRUE);
    RB_TYPE key;
    int result = FALSE;

    switch (aset->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        result = TRUE;
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        result = TRUE;
        break;
    }
    if (result) {
        if (value)
            l_asetInsert(aset, key);
        else
            l_asetDelete(aset, key);
    }
    return ll_push_bool(_fun, L, result);
}

/**
 * \brief Printable string for a Aset*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Aset *aset = ll_check_Aset(_fun, L, 1);
    AsetNode *node = nullptr;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (!aset) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), "Aset: %p [%d: %s]",
                 reinterpret_cast<void *>(aset),
                 aset->keytype,
                 ll_string_keytype(aset->keytype));
        luaL_addstring(&B, str);
        node = l_asetSize(aset) ? l_asetGetFirst(aset) : nullptr;
        while (node) {
            if (first) {
                first = 0;
                luaL_addstring(&B, "\n");
            } else {
                luaL_addstring(&B, ",\n");
            }
            switch (aset->keytype) {
            case L_INT_TYPE:
                snprintf(str, sizeof(str), "    %" PRId64,
                         static_cast<l_intptr_t>(node->key.itype));
                break;
            case L_UINT_TYPE:
                snprintf(str, sizeof(str), "    %" PRIu64,
                         static_cast<l_uintptr_t>(node->key.itype));
                break;
            case L_FLOAT_TYPE:
                snprintf(str, sizeof(str), "    %g",
                         node->key.ftype);
                break;
            default:
                snprintf(str, sizeof(str), "    %p",
                         node->key.ptype);
            }
            luaL_addstring(&B, str);
            node = l_amapGetNext(node);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Delete a node from an Aset* (%aset).
 *
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 * Arg #2 is expected to be a key (int, uint or float).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Delete(lua_State *L)
{
    LL_FUNC("Delete");
    Aset *aset = ll_check_Aset(_fun, L, 1);
    RB_TYPE key;
    int result = FALSE;

    switch (aset->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        result = TRUE;
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        result = TRUE;
        break;
    }
    if (result)
        l_asetDelete(aset, key);
    return ll_push_bool(_fun, L, result);
}

/**
 * \brief Find a key in an Aset* (%aset).
 *
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 * Arg #2 is expected to be a key (int, uint or float).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Find(lua_State *L)
{
    LL_FUNC("Find");
    Aset *aset = ll_check_Aset(_fun, L, 1);
    RB_TYPE key;
    RB_TYPE *value = nullptr;
    int result = FALSE;

    switch (aset->keytype) {
    case L_INT_TYPE:
        key.itype = ll_check_l_int64(_fun, L, 2);
        result = TRUE;
        break;
    case L_UINT_TYPE:
        key.utype = ll_check_l_uint64(_fun, L, 2);
        result = TRUE;
        break;
    case L_FLOAT_TYPE:
        key.ftype = ll_check_l_float64(_fun, L, 2);
        result = TRUE;
        break;
    }
    if (!result)
        return ll_push_nil(L);
    value = l_asetFind(aset, key);
    return ll_push_bool(_fun, L, nullptr != value);
}

/**
 * \brief Get first node in an Aset* (%aset).
 *
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetFirst(lua_State *L)
{
    LL_FUNC("GetFirst");
    Aset *aset = ll_check_Aset(_fun, L, 1);
    AsetNode *node = l_asetGetFirst(aset);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get last node in an Aset* (%aset).
 *
 * Arg #1 (i.e. self) is expected to be a Aset* (aset).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetLast(lua_State *L)
{
    LL_FUNC("GetLast");
    Aset *aset = ll_check_Amap(_fun, L, 1);
    AsetNode *node = l_asetGetLast(aset);
    lua_pushlightuserdata(L, node);
    return 1;
}

/**
 * \brief Get next node of AsetNode* (%node).
 *
 * Arg #1 is expected to be a AsetNode* (node).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetNext(lua_State *L)
{
    LL_FUNC("GetNext");
    /* HACK: deconstify */
    AsetNode *node = reinterpret_cast<AsetNode *>(reinterpret_cast<l_intptr_t>(lua_topointer(L, 2)));
    AsetNode *next = l_asetGetNext(node);
    lua_pushlightuserdata(L, next);
    return 1;
}

/**
 * \brief Get previous node of AsetNode* (%node).
 *
 * Arg #1 (i.e. self) is expected to be a AsetNode* (node).
 *
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
GetPrev(lua_State *L)
{
    LL_FUNC("GetPrev");
    /* HACK: deconstify */
    AsetNode *node = reinterpret_cast<AsetNode *>(reinterpret_cast<l_intptr_t>(lua_topointer(L, 2)));
    AsetNode *prev = l_asetGetPrev(node);
    lua_pushlightuserdata(L, prev);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_ASET.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Aset* contained in the user data
 */
Aset *
ll_check_Aset(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Aset>(_fun, L, arg, LL_ASET);
}

/**
 * \brief Optionally expect a LL_ASET at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Aset* contained in the user data
 */
Aset *
ll_check_Aset_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Aset(_fun, L, arg);
}
/**
 * \brief Push ASET user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param aset pointer to the ASET
 * \return 1 Aset* on the Lua stack
 */
int
ll_push_Aset(const char *_fun, lua_State *L, Aset *aset)
{
    if (!aset)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_ASET, aset);
}
/**
 * \brief Create and push a new Aset*.
 *
 * Arg #1 is expected to be a key type name (int, uint, or float).
 *
 * \param L pointer to the lua_State
 * \return 1 Aset* on the Lua stack
 */
int
ll_new_Aset(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the ASET methods and functions in the LL_ASET meta table.
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
        {"Create",              Create},
        {"Delete",              Delete},
        {"Destroy",             Destroy},
        {"Find",                Find},
        {"GetFirst",            GetFirst},
        {"GetLast",             GetLast},
        {"GetNext",             GetNext},
        {"GetPrev",             GetPrev},
        {"Insert",              Insert},
        {"Size",                Size},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"__index",             Find},
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_ASET);
    return ll_register_class(L, LL_ASET, methods, functions);
}
