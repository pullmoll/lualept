/************************************************************************
 * Copyright (c) Jürgen Buchmüller <pullmoll@t-online.de>
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
 * \file llqueue.cpp
 * \class Queue
 *
 * A stack of pointers.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_QUEUE

/** Define a function's name (_fun) with prefix Queue */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Queue* (%lqueue).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* (lqueue).
 *
 * Leptonica's Notes:
 *      (1) If freeflag is TRUE, frees each struct in the array.
 *      (2) If freeflag is FALSE but there are elements on the array,
 *          gives a warning and destroys the array.  This will
 *          cause a memory leak of all the items that were on the queue.
 *          So if the items require their own destroy function, they
 *          must be destroyed before the queue.  The same applies to the
 *          auxiliary stack, if it is used.
 *      (3) To destroy the L_Queue, we destroy the ptr array, then
 *          the lqueue, and then null the contents of the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 1 void on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Queue *lq = ll_take_udata<Queue>(_fun, L, 1, TNAME);
    l_int32 freeflag = ll_opt_boolean(_fun, L, 2, FALSE);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d, %s = %s\n", _fun,
        TNAME,
        "stack", reinterpret_cast<void *>(lq),
        "count", lqueueGetCount(lq),
        "freeflag", freeflag ? "TRUE" : "FALSE");
    lqueueDestroy(&lq, freeflag);
    return 0;
}

/**
 * \brief Get the number of items on the Queue* (%lqueue).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* (lq).
 * </pre>
 * \param L Lua state.
 * \return 1 l_int32 on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Queue *lq = ll_check_Queue(_fun, L, 1);
    l_int32 result = lqueueGetCount(lq);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Printable string for a Queue* (%queue).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Queue *lq = ll_check_Queue(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!lq) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(lq));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nalloc", lq->nalloc);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nhead", lq->nhead);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nelem", lq->nelem);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s** %p",
                 "array", "void", reinterpret_cast<void *>(lq->array));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s* %p",
                 "array", LL_STACK, reinterpret_cast<void *>(lq->stack));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add an item (%data) to the Queue* (%lq).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* (lq).
 * Arg #2 is expected to be a light user data (item).
 *
 * Leptonica's Notes:
 *      (1) The algorithm is as follows.  If the queue is populated
 *          to the end of the allocated array, shift all ptrs toward
 *          the beginning of the array, so that the head of the queue
 *          is at the beginning of the array.  Then, if the array is
 *          more than 0.75 full, realloc with double the array size.
 *          Finally, add the item to the tail of the queue.
 * </pre>
 * \param L Lua state.
 * \return 1 l_int32 on the Lua stack.
 */
static int
Add(lua_State *L)
{
    LL_FUNC("Add");
    Queue *lq = ll_check_Queue(_fun, L, 1);
    void *item = ll_take_udata<void>(_fun, L, 2, "*");
    l_int32 result = lqueueAdd(lq, item);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Create a Queue* (%lq) of size (%nalloc).
 * <pre>
 * Arg #1 is expected to be a l_int32 (nalloc).
 *
 * Leptonica's Notes:
 *      (1) Allocates a ptr array of given size, and initializes counters.
 * </pre>
 * \param L Lua state.
 * \return 1 Queue * on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nalloc = ll_check_l_int32(_fun, L, 1);
    Queue *lq = lqueueCreate(nalloc);
    return ll_push_Queue(_fun, L, lq);
}

/**
 * \brief Remove top item from the Queue* (%lq)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* (lq).
 *
 * Leptonica's Notes:
 *      (1) If this is the last item on the queue, so that the queue
 *          becomes empty, nhead is reset to the beginning of the array.
 * </pre>
 * \param L Lua state.
 * \return 1 light user data on the Lua stack.
 */
static int
Remove(lua_State *L)
{
    LL_FUNC("Remove");
    Queue *lq = ll_check_Queue(_fun, L, 1);
    void *data = lqueueRemove(lq);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Print a Queue* (%lq) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Queue* (lq).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Print(lua_State *L)
{
    LL_FUNC("Print");
    Queue *lq = ll_check_Queue(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == lqueuePrint(stream->f, lq));
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class Queue*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Queue* contained in the user data.
 */
Queue *
ll_check_Queue(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Queue>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Queue* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Queue* contained in the user data.
 */
Queue *
ll_opt_Queue(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Queue(_fun, L, arg);
}

/**
 * \brief Push Queue* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_Queue
 * \return 1 Queue* on the Lua stack.
 */
int
ll_push_Queue(const char *_fun, lua_State *L, Queue *cd)
{
    if (!cd)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new Queue*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L Lua state.
 * \return 1 Queue* on the Lua stack.
 */
int
ll_new_Queue(lua_State *L)
{
    FUNC("ll_new_Queue");
    Queue *lqueue = nullptr;
    l_int32 nalloc = 1;

    if (ll_isinteger(_fun, L, 1)) {
        nalloc = ll_opt_l_int32(_fun, L, 1, nalloc);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "nalloc", nalloc);
        lqueue = lqueueCreate(nalloc);
    }

    if (!lqueue) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "nalloc", nalloc);
        lqueue = lqueueCreate(nalloc);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(lqueue));
    return ll_push_Queue(_fun, L, lqueue);
}

/**
 * \brief Register the Queue methods and functions in the Queue meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Queue(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Queue},
        {"__len",               GetCount},
        {"__tostring",          toString},
        {"Add",                 Add},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"GetCount",            GetCount},
        {"Pop",                 Remove},    /* alias for Remove */
        {"Print",               Print},
        {"Push",                Add},       /* alias for Add */
        {"Remove",              Remove},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Queue);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
