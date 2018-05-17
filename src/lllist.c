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
 *  Lua class DLLIST
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_DLLIST
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DLLIST* contained in the user data
 */
DLLIST *
ll_check_DLLIST(lua_State *L, int arg)
{
    return *(DLLIST **)ll_check_udata(L, arg, LL_DLLIST);
}

/**
 * \brief Push DLLIST user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param head pointer to the DLLIST
 * \return 1 DLLIST* on the Lua stack
 */
int
ll_push_DLLIST(lua_State *L, DLLIST *head)
{
    if (NULL == head)
        return 0;
    return ll_push_udata(L, LL_DLLIST, head);
}

/**
 * \brief Create and push a new DLLIST*
 *
 * Arg #1 is expected to be a key type name (int, uint, or float)
 *
 * \param L pointer to the lua_State
 * \return 1 DLLIST* on the Lua stack
 */
int
ll_new_DLLIST(lua_State *L)
{
    DLLIST *head = (DLLIST *)calloc(1, sizeof(DLLIST));
    return ll_push_DLLIST(L, head);
}

/**
 * @brief Printable string for a DLLIST*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *elem;
    luaL_Buffer B;
    int first = 1;

    luaL_buffinit(L, &B);
    if (NULL == head) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        L_BEGIN_LIST_FORWARD(head, elem)
            if (first) {
                first = 0;
            } else {
                luaL_addchar(&B, ',');
            }
            snprintf(str, sizeof(str), "%p=%p", (void *)elem, elem->data);
            luaL_addstring(&B, str);
        L_END_LIST
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new DLLIST*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float)
 *
 * \param L pointer to the lua_State
 * \return 1 DLLIST* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_DLLIST(L);
}

/**
 * \brief Size of an DLLIST*
 *
 * Arg #1 is expected to be a string describing the key type (int,uint,float)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    lua_pushinteger(L, listGetCount(head));
    return 1;
}

/**
 * \brief Destroy a DLLIST*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **plist = ll_check_udata(L, 1, LL_DLLIST);
    DBG(LOG_DESTROY, "%s: '%s' plist=%p head=%p size=%d\n", __func__,
        LL_DLLIST, (void *)plist, *plist, listGetCount(*(DLLIST **)plist));
    listDestroy((DLLIST **)plist);
    *plist = NULL;
    return 0;
}

/**
 * \brief Insert data before a DLLIST* (%elem) into an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be a DLLIST* (elem)
 * Arg #3 is expected to be a pointer (data)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
InsertBefore(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *elem = ll_check_DLLIST(L, 2);
    void *data = (void *)((intptr_t)lua_topointer(L, 3));
    lua_pushboolean(L, 0 == listInsertBefore(&head, elem, data));
    return 1;
}

/**
 * \brief Insert data after a DLLIST* (%elem) into an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be a DLLIST* (elem)
 * Arg #3 is expected to be a pointer (data)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
InsertAfter(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *elem = ll_check_DLLIST(L, 2);
    void *data = (void *)((intptr_t)lua_topointer(L, 3));
    lua_pushboolean(L, 0 == listInsertAfter(&head, elem, data));
    return 1;
}

/**
 * \brief At data to the head of a DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be a pointer (data)
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
AddToHead(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    void *data = (void *)((intptr_t)lua_topointer(L, 2));
    lua_pushboolean(L, 0 == listAddToHead(&head, data));
    return 1;
}

/**
 * \brief At data to the tail of a DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be a pointer (data)
 *
 * \param L pointer to the lua_State
 * \return 2 for boolean and lightuserdata (tail) on the Lua stack
 */
static int
AddToTail(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *tail = NULL;
    void *data = (void *)((intptr_t)lua_topointer(L, 2));
    lua_pushboolean(L, 0 == listAddToTail(&head, &tail, data));
    lua_pushlightuserdata(L, tail);
    return 2;
}

/**
 * \brief Remove an element DLLIST* (%elem) from an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be a DLLIST* (elem)
 *
 * \param L pointer to the lua_State
 * \return 1 for light userdata on the Lua stack
 */
static int
RemoveElement(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *elem = ll_check_DLLIST(L, 2);
    void *data = listRemoveElement(&head, elem);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Remove an element from the head of an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 *
 * \param L pointer to the lua_State
 * \return 1 for light userdata on the Lua stack
 */
static int
RemoveFromHead(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    void *data = listRemoveFromHead(&head);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Remove an element from the tail of an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 *
 * \param L pointer to the lua_State
 * \return 2 for light userdata (data, tail) on the Lua stack
 */
static int
RemoveFromTail(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *tail = NULL;
    void *data = listRemoveFromTail(&head, &tail);
    lua_pushlightuserdata(L, data);
    lua_pushlightuserdata(L, tail);
    return 2;
}

/**
 * \brief Find the element pointing to %data in an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 * Arg #2 is expected to be light userdata (data)
 *
 * \param L pointer to the lua_State
 * \return 1 for light userdata (elem) on the Lua stack
 */
static int
FindElement(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    void *data = (void *)((intptr_t)lua_topointer(L, 2));
    DLLIST *elem = listFindElement(head, data);
    lua_pushlightuserdata(L, elem);
    return 1;
}

/**
 * \brief Find the tail of an DLLIST* (%head)
 *
 * Arg #1 (i.e. self) is expected to be a DLLIST* (head)
 *
 * \param L pointer to the lua_State
 * \return 1 for light userdata (tail) on the Lua stack
 */
static int
FindTail(lua_State *L)
{
    DLLIST *head = ll_check_DLLIST(L, 1);
    DLLIST *tail = listFindTail(head);
    lua_pushlightuserdata(L, tail);
    return 1;
}

/**
 * \brief Register the DLLIST methods and functions in the LL_DLLIST meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_DLLIST(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__len",               GetCount},
        {"__new",               Create},
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"GetCount",            GetCount},
        {"InsertBefore",        InsertBefore},
        {"InsertAfter",         InsertAfter},
        {"AddToHead",           AddToHead},
        {"AddToTail",           AddToTail},
        {"RemoveElement",       RemoveElement},
        {"RemoveFromHead",      RemoveFromHead},
        {"RemoveFromTail",      RemoveFromTail},
        {"FindElement",         FindElement},
        {"FindTail",            FindTail},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_DLLIST, methods, functions);
    lua_setglobal(L, LL_DLLIST);
    return res;
}
