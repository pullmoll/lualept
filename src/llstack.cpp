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
 * \file llstack.cpp
 * \class Stack
 *
 * A stack of pointers.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_STACK

/** Define a function's name (_fun) with prefix Stack */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Stack* (%lstack).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Stack* (lstack).
 *
 * Leptonica's Notes:
 *      (1) If freeflag is TRUE, frees each struct in the array.
 *      (2) If freeflag is FALSE but there are elements on the array,
 *          gives a warning and destroys the array.  This will
 *          cause a memory leak of all the items that were on the lstack.
 *          So if the items require their own destroy function, they
 *          must be destroyed before the lstack.
 *      (3) To destroy the lstack, we destroy the ptr array, then
 *          the lstack, and then null the contents of the input ptr.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Stack **pstack = ll_check_udata<Stack>(_fun, L, 1, TNAME);
    l_int32 freeflag = ll_opt_boolean(_fun, L, 2, FALSE);
    Stack *stack = *pstack;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d, %s = %s\n", _fun,
        TNAME,
        "pstack", reinterpret_cast<void *>(pstack),
        "stack", reinterpret_cast<void *>(stack),
        "count", lstackGetCount(stack),
        "freeflag", freeflag ? "TRUE" : "FALSE");
    lstackDestroy(&stack, freeflag);
    *pstack = nullptr;
    return 0;
}

/**
 * \brief Get the number of items on the Stack* (%lstack).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Stack* (lstack).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Stack *stack = ll_check_Stack(_fun, L, 1);
    l_int32 result = lstackGetCount(stack);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Add an item (%data) to the Stack* (%lstack).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Stack* (lstack).
 * Arg #2 is expected to be a light user data (item).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Add(lua_State *L)
{
    LL_FUNC("Add");
    Stack *lstack = ll_check_Stack(_fun, L, 1);
    const void *data = lua_topointer(L, 2);
    /* XXX: deconstify */
    void *item = reinterpret_cast<void *>(reinterpret_cast<l_intptr_t>(data));
    l_int32 result = lstackAdd(lstack, item);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Create a Stack* (%lstack) of size (%nalloc).
 * <pre>
 * Arg #1 is expected to be a l_int32 (nalloc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Stack * on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nalloc = ll_check_l_int32(_fun, L, 1);
    Stack *result = lstackCreate(nalloc);
    return ll_push_Stack(_fun, L, result);
}

/**
 * \brief Remove top item from the Stack* (%lstack)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Stack* (stack).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 light user data on the Lua stack
 */
static int
Remove(lua_State *L)
{
    LL_FUNC("Remove");
    Stack *lstack = ll_check_Stack(_fun, L, 1);
    void *data = lstackRemove(lstack);
    lua_pushlightuserdata(L, data);
    return 1;
}

/**
 * \brief Print a Stack* (%lstack) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Stack* (lstack).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Print(lua_State *L)
{
    LL_FUNC("Print");
    Stack *lstack = ll_check_Stack(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == lstackPrint(stream->f, lstack));
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class Stack*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Stack* contained in the user data
 */
Stack *
ll_check_Stack(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Stack>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Stack* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Stack* contained in the user data
 */
Stack *
ll_opt_Stack(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Stack(_fun, L, arg);
}

/**
 * \brief Push Stack* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_Stack
 * \return 1 Stack* on the Lua stack
 */
int
ll_push_Stack(const char *_fun, lua_State *L, Stack *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new Stack*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 Stack* on the Lua stack
 */
int
ll_new_Stack(lua_State *L)
{
    FUNC("ll_new_Stack");
    Stack *lstack = nullptr;
    l_int32 nalloc = 1;

    if (lua_isinteger(L, 1)) {
        nalloc = ll_opt_l_int32(_fun, L, 1, nalloc);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "nalloc", nalloc);
        lstack = lstackCreate(nalloc);
    }

    if (!lstack) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "nalloc", nalloc);
        lstack = lstackCreate(nalloc);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(lstack));
    return ll_push_Stack(_fun, L, lstack);
}

/**
 * \brief Register the Stack methods and functions in the Stack meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Stack(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Stack},
        {"__len",               GetCount},
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
    ll_global_cfunct(_fun, L, TNAME, ll_new_Stack);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
