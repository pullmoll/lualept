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
 *  Lua class BOXA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_BOXA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the BOXA* contained in the user data
 */
BOXA *
ll_check_BOXA(lua_State *L, int arg)
{
    return *(BOXA **)ll_check_udata(L, arg, LL_BOXA);
}

/**
 * \brief Push BOXA* user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param boxa pointer to the BOXA
 * \return 1 BOXA* on the Lua stack
 */
int
ll_push_BOXA(lua_State *L, BOXA *boxa)
{
    if (NULL == boxa)
        return 0;
    return ll_push_udata(L, LL_BOXA, boxa);
}

/**
 * \brief Create and push a new BOXA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 BOXA* on the Lua stack
 */
int
ll_new_BOXA(lua_State *L)
{
    l_int32 n = ll_check_l_int32(L, 1);
    BOXA *boxa = boxaCreate(n);
    return ll_push_BOXA(L, boxa);
}

/**
 * @brief Printable string for a BOXA*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    BOXA *boxa = ll_check_BOXA(L, 1);
    luaL_Buffer B;
    l_int32 i, x, y, w, h;

    luaL_buffinit(L, &B);
    if (NULL == boxa) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        for (i = 0; i < boxaGetCount(boxa); i++) {
            boxaGetBoxGeometry(boxa, i, &x, &y, &w, &h);
            snprintf(str, sizeof(str), "{%d,%d,%d,%d}", x, y, w, h);
            if (i > 0)
                luaL_addchar(&B, ',');
            luaL_addstring(&B, str);
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new BOXA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 BOXA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_BOXA(L);
}

/**
 * \brief Destroy a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **pboxa = ll_check_udata(L, 1, LL_BOXA);
    DBG(LOG_DESTROY, "%s: '%s' pboxa=%p boxa=%p\n", __func__,
        LL_BOXA, (void *)pboxa, *pboxa);
    boxaDestroy((BOXA **)pboxa);
    *pboxa = NULL;
    return 0;
}

/**
 * \brief Copy a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 BOXA* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    BOXA *boxas = ll_check_BOXA(L, 1);
    l_int32 copyflag = ll_check_access_storage(L, 2, L_COPY);
    BOXA *boxa = boxaCopy(boxas, copyflag);
    return ll_push_BOXA(L, boxa);
}

/**
 * \brief Add a BOX* to a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == boxaAddBox(boxa, box, flag));
    return 1;
}

/**
 * \brief Extend a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    lua_pushboolean(L, 0 == boxaExtendArray(boxa));
    return 1;
}

/**
 * \brief Extend a BOXA* to a given size %n
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 n = ll_check_l_int32(L, 2);
    lua_pushboolean(L, 0 == boxaExtendArrayToSize(boxa, n));
    return 1;
}

/**
 * \brief Get count for a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    lua_pushinteger(L, boxaGetCount(boxa));
    return 1;
}

/**
 * \brief Get valid count for a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetValidCount(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    lua_pushinteger(L, boxaGetValidCount(boxa));
    return 1;
}

/**
 * \brief Get BOX* from a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (copy, clone).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    BOX *box = boxaGetBox(boxa, idx, flag);
    return ll_push_BOX(L, box);
}

/**
 * \brief Get valid BOX* from a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (copy, clone).
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack
 */
static int
GetValidBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    BOX *box = boxaGetValidBox(boxa, idx, flag);
    return ll_push_BOX(L, box);
}

/**
 * \brief Find invalid BOX* in a BOXA* and return a NUMA* of indices
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack, or nil if no invalid boxes
 */
static int
FindInvalidBoxes(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    NUMA *na = boxaFindInvalidBoxes(boxa);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Get the geometry for a BOX* from a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 *
 * \param L pointer to the lua_State
 * \return 4 integers x, y, w, h or nil on error
 */
static int
GetBoxGeometry(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 x, y, w, h;
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    if (boxaGetBoxGeometry(boxa, idx, &x, &y, &w, &h))
        return 0;
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Get the IsFull state for a BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
IsFull(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    int isfull = 0;
    lua_pushboolean(L, 0 == boxaIsFull(boxa, &isfull) && isfull);
    return 1;
}

/**
 * \brief Replace the BOX* in a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    BOX *box = ll_check_BOX(L, 3);
    lua_pushboolean(L, box && 0 == boxaReplaceBox(boxa, idx, box));
    return 1;
}

/**
 * \brief Insert the BOX* in a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    BOX *boxs = ll_check_BOX(L, 3);
    BOX *box = boxClone(boxs);
    lua_pushboolean(L, box && 0 == boxaInsertBox(boxa, idx, box));
    return 1;
}

/**
 * \brief Reomve the BOX* from a BOXA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (%idx)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBox(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    lua_pushboolean(L, 0 == boxaRemoveBox(boxa, idx));
    return 1;
}

/**
 * \brief Reomve the BOX* from a BOXA* at index %idx and return it as BOX* user
 * data
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is expected to be a l_int32 (%idx)
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack, or 0 in case of error
 */
static int
RemoveBoxAndSave(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    l_int32 idx = ll_check_index(L, 2, boxaGetCount(boxa));
    BOX *box = NULL;
    int n = 0;
    if (0 == boxaRemoveBoxAndSave(boxa, idx, &box) && box) {
        n += ll_push_BOX(L, box);
    }
    return n;
}

/**
 * \brief Save the valid BOX* in a BOXA* boxas and return the resulting BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 * Arg #2 is an optional string defining the storage flags (copy, clone)
 *
 * \param L pointer to the lua_State
 * \return 1 BOXA* on the Lua stack; or nil on error
 */
static int
SaveValid(lua_State *L)
{
    BOXA *boxas = ll_check_BOXA(L, 1);
    l_int32 copyflag = ll_check_access_storage(L, 2, L_COPY);
    BOXA *boxa = boxaSaveValid(boxas, copyflag);
    return ll_push_BOXA(L, boxa);
}

/**
 * \brief Clear the BOXA*
 *
 * Arg #1 (i.e. self) is expected to be a BOXA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    BOXA *boxa = ll_check_BOXA(L, 1);
    lua_pushboolean(L, 0 == boxaClear(boxa));
    return 1;
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_BOXA(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},               /* garbage collect */
        {"__new",               Create},                /* new BOXA */
        {"__len",               GetCount},              /* #boxa */
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"Copy",                Copy},
        {"AddBox",              AddBox},
        {"ExtendArray",         ExtendArray},
        {"ExtendArrayToSize",   ExtendArrayToSize},
        {"GetValidCount",       GetValidCount},
        {"GetBox",              GetBox},
        {"GetValidBox",         GetValidBox},
        {"FindInvalidBoxes",    FindInvalidBoxes},
        {"GetBoxGeometry",      GetBoxGeometry},
        {"IsFull",              IsFull},
        {"ReplaceBox",          ReplaceBox},
        {"InsertBox",           InsertBox},
        {"RemoveBox",           RemoveBox},
        {"RemoveBoxAndSave",    RemoveBoxAndSave},
        {"TakeBox",             RemoveBoxAndSave},      /* alias */
        {"SaveValid",           SaveValid},
        {"Clear",               Clear},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_BOXA, methods, functions);
    lua_setglobal(L, LL_BOXA);
    return res;
}
