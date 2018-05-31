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
 *  Lua class DoubleLinkedList
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_DLLIST */
#define LL_FUNC(x) FUNC(LL_DLLIST "." x)

/**
 * \brief Destroy a DoubleLinkedList*.
 * <pre>
 * Arg #1 is expected to be a DoubleLinkedList* (head)
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    DoubleLinkedList **plist = ll_check_udata<DoubleLinkedList>(_fun, L, 1, LL_DLLIST);
    DoubleLinkedList *list = *plist;
    DBG(LOG_DESTROY, "%s: '%s' plist=%p head=%p size=%d\n",
        _fun, LL_DLLIST, plist, list, listGetCount(list));
    listDestroy(&list);
    *plist = nullptr;
    return 0;
}

/**
 * \brief Get the count of a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 is expected to be a DoubleLinkedList* (head)
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    ll_push_l_int32(_fun, L, listGetCount(head));
    return 1;
}

/**
 * \brief Create a new DoubleLinkedList*.
 * <pre>
 * Arg #1 is expected to be a string describing the key type (int,uint,float).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DoubleLinkedList* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    DoubleLinkedList *head = ll_calloc<DoubleLinkedList>(_fun, L, 1);
    return ll_push_DoubleLinkedList(_fun, L, head);
}

/**
 * \brief Printable string for a DoubleLinkedList*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *elem;
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!head) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), LL_DLLIST ": %p", reinterpret_cast<void *>(head));
        luaL_addstring(&B, str);
        L_BEGIN_LIST_FORWARD(head, elem)
            snprintf(str, sizeof(str),
                     "\n    %p = %p",
                     reinterpret_cast<void *>(elem),
                     elem->data);
            luaL_addstring(&B, str);
        L_END_LIST
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief At data to the head of a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a pointer (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddToHead(lua_State *L)
{
    LL_FUNC("AddToHead");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    void *data = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    return ll_push_boolean(_fun, L, 0 == listAddToHead(&head, data));
}

/**
 * \brief At data to the tail of a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a pointer (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for boolean and light userdata (tail) on the Lua stack
 */
static int
AddToTail(lua_State *L)
{
    LL_FUNC("AddToTail");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *tail = nullptr;
    void *data = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    ll_push_boolean(_fun, L, 0 == listAddToTail(&head, &tail, data));
    lua_pushlightuserdata(L, tail);
    return 2;
}

/**
 * \brief Find the element pointing to %data in an DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be light userdata (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (elem) on the Lua stack
 */
static int
FindElement(lua_State *L)
{
    LL_FUNC("FindElement");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    void *data = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(lua_topointer(L, 2)));
    DoubleLinkedList *elem = listFindElement(head, data);
    lua_pushlightuserdata(L, elem);
    return 1;
}

/**
 * \brief Find the tail of an DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (tail) on the Lua stack
 */
static int
FindTail(lua_State *L)
{
    LL_FUNC("FindTail");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *tail = listFindTail(head);
    lua_pushlightuserdata(L, tail);
    return 1;
}

/**
 * \brief Insert data after a DoubleLinkedList* (%elem) into an DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a DoubleLinkedList* (elem).
 * Arg #3 is expected to be a pointer (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertAfter(lua_State *L)
{
    LL_FUNC("InsertAfter");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *elem = ll_check_DoubleLinkedList(_fun, L, 2);
    void *data = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(lua_topointer(L, 3)));
    return ll_push_boolean(_fun, L, 0 == listInsertAfter(&head, elem, data));
}

/**
 * \brief Insert data before a DoubleLinkedList* (%elem) into an DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a DoubleLinkedList* (elem).
 * Arg #3 is expected to be a pointer (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBefore(lua_State *L)
{
    LL_FUNC("InsertBefore");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *elem = ll_check_DoubleLinkedList(_fun, L, 2);
    void *data = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(lua_topointer(L, 3)));
    return ll_push_boolean(_fun, L, 0 == listInsertBefore(&head, elem, data));
}

/**
 * \brief Join a DoubleLinkedList* (%list) to a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a nother DoubleLinkedList* (list).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (data) on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *list = ll_check_DoubleLinkedList(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == listJoin(&head, &list));
}

/**
 * \brief Remove an element DoubleLinkedList* (%elem) from a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be a DoubleLinkedList* (elem).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (data) on the Lua stack
 */
static int
RemoveElement(lua_State *L)
{
    LL_FUNC("RemoveElement");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *elem = ll_check_DoubleLinkedList(_fun, L, 2);
    void *data = listRemoveElement(&head, elem);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Remove an element from the head of a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (data) on the Lua stack
 */
static int
RemoveFromHead(lua_State *L)
{
    LL_FUNC("RemoveFromHead");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    void *data = listRemoveFromHead(&head);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Remove an element from the tail of a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for light userdata (data, tail) on the Lua stack
 */
static int
RemoveFromTail(lua_State *L)
{
    LL_FUNC("RemoveFromTail");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    DoubleLinkedList *tail = nullptr;
    void *data = listRemoveFromTail(&head, &tail);
    lua_pushlightuserdata(L, data);
    lua_pushlightuserdata(L, tail);
    return 2;
}

/**
 * \brief Reverse a DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light userdata (data) on the Lua stack
 */
static int
Reverse(lua_State *L)
{
    LL_FUNC("Reverse");
    DoubleLinkedList *head = ll_check_DoubleLinkedList(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == listReverse(&head));
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_DLLIST.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DoubleLinkedList* contained in the user data
 */
DoubleLinkedList *
ll_check_DoubleLinkedList(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DoubleLinkedList>(_fun, L, arg, LL_DLLIST);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DoubleLinkedList* contained in the user data
 */
DoubleLinkedList *
ll_opt_DoubleLinkedList(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_DoubleLinkedList(_fun, L, arg);
}

/**
 * \brief Push DLLIST user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param head pointer to the DLLIST
 * \return 1 DoubleLinkedList* on the Lua stack
 */
int
ll_push_DoubleLinkedList(const char *_fun, lua_State *L, DoubleLinkedList *head)
{
    if (!head)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DLLIST, head);
}
/**
 * \brief Create and push a new DoubleLinkedList*.
 * \param L pointer to the lua_State
 * \return 1 DoubleLinkedList* on the Lua stack
 */
int
ll_new_DoubleLinkedList(lua_State *L)
{
    FUNC("ll_new_DoubleLinkedList");
    DoubleLinkedList *head = ll_calloc<DoubleLinkedList>(_fun, L, 1);

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        LL_DLLIST, reinterpret_cast<void *>(head));
    return ll_push_DoubleLinkedList(_fun, L, head);
}

/**
 * \brief Register the DLLIST methods and functions in the LL_DLLIST meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_DoubleLinkedList(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},                   /* garbage collector */
        {"__len",               GetCount},                  /* #list */
        {"__new",               ll_new_DoubleLinkedList},   /* DoubleLinkedList(data) */
        {"__tostring",          toString},
        {"AddToHead",           AddToHead},
        {"AddToTail",           AddToTail},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"FindElement",         FindElement},
        {"FindTail",            FindTail},
        {"GetCount",            GetCount},
        {"InsertAfter",         InsertAfter},
        {"InsertBefore",        InsertBefore},
        {"Join",                Join},
        {"RemoveElement",       RemoveElement},
        {"RemoveFromHead",      RemoveFromHead},
        {"RemoveFromTail",      RemoveFromTail},
        {"Reverse",             Reverse},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_DoubleLinkedList);
    lua_setglobal(L, LL_DLLIST);
    return ll_register_class(L, LL_DLLIST, methods, functions);
}
