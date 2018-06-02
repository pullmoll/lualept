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

/**
 * \file lldllist.cpp
 * \class DoubleLinkedList
 *
 * A double linked list of pointers.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DLLIST

/** Define a function's name (_fun) with prefix DoubleLinkedList */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a DoubleLinkedList*.
 * <pre>
 * Arg #1 is expected to be a DoubleLinkedList* (head)
 *
 * Leptonica's Notes:
 *      (1) This only destroys the cons cells.  Before destroying
 *          the list, it is necessary to remove all data and set the
 *          data pointers in each cons cell to NULL.
 *      (2) listDestroy() will give a warning message for each data
 *          ptr that is not NULL.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    DoubleLinkedList **plist = ll_check_udata<DoubleLinkedList>(_fun, L, 1, TNAME);
    DoubleLinkedList *list = *plist;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "plist", reinterpret_cast<void *>(plist),
        "list", reinterpret_cast<void *>(list),
        "size", listGetCount(list));
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
        snprintf(str, sizeof(str), TNAME ": %p", reinterpret_cast<void *>(head));
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
 *
 * Leptonica's Notes:
 *      (1) This makes a new cell, attaches the data, and adds the
 *          cell to the head of the list.
 *      (2) When consing from NULL, be sure to initialize head to NULL
 *          before calling this function.
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
 *
 * Leptonica's Notes:
 *      (1) This makes a new cell, attaches the data, and adds the
 *          cell to the tail of the list.
 *      (2) &head is input to allow the list to be "cons'd" up from NULL.
 *      (3) &tail is input to allow the tail to be updated
 *          for efficient sequential operation with this function.
 *      (4) We assume that if *phead and/or *ptail are not NULL,
 *          then they are valid addresses.  Therefore:
 *           (a) when consing from NULL, be sure to initialize both
 *               head and tail to NULL.
 *           (b) when tail == NULL for an existing list, the tail
 *               will be found and updated.
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
 * \brief Find the element pointing to %data in an DoubleLinkedList* (%head).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DoubleLinkedList* (head).
 * Arg #2 is expected to be light userdata (data).
 *
 * Leptonica's Notes:
 *      (1) This returns a ptr to the cell, which is still embedded in
 *          the list.
 *      (2) This handle and the attached data have not been copied or
 *          reference counted, so they must not be destroyed.  This
 *          violates our basic rule that every handle returned from a
 *          function is owned by that function and must be destroyed,
 *          but if rules aren't there to be broken, why have them?
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
 *
 * Leptonica's Notes:
 *      (1) This can be called on a null list, in which case both
 *          head and elem must be null.  The head is included
 *          in the call to allow "consing" up from NULL.
 *      (2) If you are searching through a list, looking for a condition
 *          to add an element, you can do something like this:
 *            L_BEGIN_LIST_FORWARD(head, elem)
 *                // identify an elem to insert after
 *                listInsertAfter(&head, elem, data);
 *            L_END_LIST
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
 *
 * Leptonica's Notes:
 *      (1) This can be called on a null list, in which case both
 *          head and elem must be null.
 *      (2) If you are searching through a list, looking for a condition
 *          to add an element, you can do something like this:
 *            L_BEGIN_LIST_FORWARD(head, elem)
 *                // identify an elem to insert before
 *                listInsertBefore(&head, elem, data);
 *            L_END_LIST
 *
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
 *
 * Leptonica's Notes:
 *      (1) The concatenated list is returned with head1 as the new head.
 *      (2) Both input ptrs must exist, though either can have the value NULL.
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
 *
 * Leptonica's Notes:
 *      (1) in ANSI C, it is not necessary to cast return to actual type; e.g.,
 *             pix = listRemoveElement(&head, elem);
 *          but in ANSI C++, it is necessary to do the cast:
 *             pix = (Pix *)listRemoveElement(&head, elem);
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
 *
 * Leptonica's Notes:
 *      (1) in ANSI C, it is not necessary to cast return to actual type; e.g.,
 *            pix = listRemoveFromHead(&head);
 *          but in ANSI C++, it is necessary to do the cast; e.g.,
 *            pix = (Pix *)listRemoveFromHead(&head);
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
 *
 * Leptonica's Notes:
 *      (1) We include &head so that it can be set to NULL if
 *          if the only element in the list is removed.
 *      (2) The function is relying on the fact that if tail is
 *          not NULL, then is is a valid address.  You can use
 *          this function with tail == NULL for an existing list, in
 *          which case  the tail is found and updated, and the
 *          removed element is returned.
 *      (3) In ANSI C, it is not necessary to cast return to actual type; e.g.,
 *            pix = listRemoveFromTail(&head, &tail);
 *          but in ANSI C++, it is necessary to do the cast; e.g.,
 *            pix = (Pix *)listRemoveFromTail(&head, &tail);
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
 *
 * Leptonica's Notes:
 *      (1) This reverses the list in-place.
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
 * \brief Check Lua stack at index %arg for udata of class DoubleLinkedList.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DoubleLinkedList* contained in the user data
 */
DoubleLinkedList *
ll_check_DoubleLinkedList(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DoubleLinkedList>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a DoubleLinkedList* at index %arg on the Lua stack.
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
    return ll_push_udata(_fun, L, TNAME, head);
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
        TNAME, reinterpret_cast<void *>(head));
    return ll_push_DoubleLinkedList(_fun, L, head);
}

/**
 * \brief Register the DLLIST methods and functions in the DoubleLinkedList meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_DoubleLinkedList(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_DoubleLinkedList},
        {"__len",               GetCount},
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
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_DoubleLinkedList);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
