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
 *  Lua class BOXAA
 *
 *====================================================================*/

/**
 * \brief Printable string for a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_BOXAA ".toString");
    static char str[256];
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i, j, x, y, w, h;

    luaL_buffinit(L, &B);
    if (!boxaa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_BOXAA ": %p",
                 reinterpret_cast<void *>(boxaa));
        luaL_addstring(&B, str);
        for (i = 0; i < boxaaGetCount(boxaa); i++) {
            Boxa *boxa = boxaaGetBoxa(boxaa, i, L_CLONE);
            snprintf(str, sizeof(str),
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (j = 0; j < boxaGetCount(boxa); j++) {
                boxaGetBoxGeometry(boxa, j, &x, &y, &w, &h);
                snprintf(str, sizeof(str),
                         "\n       %d = { x = %d, y = %d, w = %d, h = %d }",
                         j+1, x, y, w, h);
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            boxaDestroy(&boxa);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new Boxaa*
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_BOXAA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Boxaa *boxaa = boxaaCreate(n);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Destroy a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_BOXAA ".Destroy");
    Boxaa **pboxaa = reinterpret_cast<Boxaa **>(ll_check_udata(_fun, L, 1, LL_BOXAA));
    DBG(LOG_DESTROY, "%s: '%s' pboxaa=%p boxaa=%p\n",
        _fun, LL_BOXAA, pboxaa, *pboxaa);
    boxaaDestroy(pboxaa);
    *pboxaa = nullptr;
    return 0;
}

/**
 * \brief Copy a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_BOXAA ".Copy");
    Boxaa *boxaas = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxaa *boxaa = boxaaCopy(boxaas, copyflag);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Add a Box* to a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a Boxa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".AddBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    lua_pushboolean(L, 0 == boxaaAddBoxa(boxaa, boxa, flag));
    return 1;
}

/**
 * \brief Extend a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    FUNC(LL_BOXAA ".ExtendArray");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    lua_pushboolean(L, 0 == boxaaExtendArray(boxaa));
    return 1;
}

/**
 * \brief Extend a Boxaa* to a given size %n
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    FUNC(LL_BOXAA ".ExtendArrayToSize");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == boxaaExtendArrayToSize(boxaa, n));
    return 1;
}

/**
 * \brief Get count for a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_BOXAA ".GetCount");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    lua_pushinteger(L, boxaaGetCount(boxaa));
    return 1;
}

/**
 * \brief Copy a Boxaa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxCount(lua_State *L)
{
    FUNC(LL_BOXAA ".GetBoxCount");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    lua_pushinteger(L, boxaaGetBoxCount(boxaa));
    return 1;
}

/**
 * \brief Get Boxa* from a Boxaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".GetBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Boxa *boxa = boxaaGetBoxa(boxaa, idx, flag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Get Box* from a Boxaa* at index %iboxa and %ibox
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (iboxa).
 * Arg #2 is expected to be a l_int32 (ibox).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    FUNC(LL_BOXAA ".GetBox");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 iboxa = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    l_int32 ibox = ll_check_index(_fun, L, 3, INT32_MAX);
    l_int32 flag = ll_check_access_storage(_fun, L, 4, L_COPY);
    Box *box = boxaaGetBox(boxaa, iboxa, ibox, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Replace the Box* in a Boxaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".ReplaceBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    Boxa *boxa = ll_check_Boxa(_fun, L, 3);
    lua_pushboolean(L, boxa && 0 == boxaaReplaceBoxa(boxaa, idx, boxa));
    return 1;
}

/**
 * \brief Insert the Boxa* in a Boxaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".InsertBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    Boxa *boxas = ll_check_Boxa(_fun, L, 3);
    Boxa *boxa = boxaCopy(boxas, L_CLONE);
    lua_pushboolean(L, boxa && 0 == boxaaInsertBoxa(boxaa, idx, boxa));
    return 1;
}

/**
 * \brief Reomve the Boxa* from a Boxaa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data.
 * Arg #2 is expected to be a l_int32 (%idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".RemoveBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    lua_pushboolean(L, 0 == boxaaRemoveBoxa(boxaa, idx));
    return 1;
}

/**
 * \brief Flatten the Boxaa* to a Boxa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data (boxaa).
 * Arg #2 is expected to be a string describing the copy flag (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenToBoxa(lua_State *L)
{
    FUNC(LL_BOXAA ".FlattenToBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Numa *naindex = nullptr;
    Boxa *boxa = boxaaFlattenToBoxa(boxaa, &naindex, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Aligned flatten the Boxaa* to a Boxa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data (boxaa).
 * Arg #2 is expected to be a l_int32 (num).
 * Arg #3 is expected to be a string describing the copy flag (copyflag).
 * Arg #3 is optional and, if given, expected to be a Box* (fillerbox).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenAligned(lua_State *L)
{
    FUNC(LL_BOXAA ".FlattenAligned");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 num = ll_check_l_int32(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *fillerbox = ll_check_Box_opt(_fun, L, 4);
    Boxa *boxa = boxaaFlattenAligned(boxaa, num, fillerbox, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Join Boxaa* (%boxaas) with Boxa* (%boxaad)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxaad).
 * Arg #2 is expected to be another Box* (boxaas).
 * Arg #3 is optional and, if given, expected to be a l_int32 (istart).
 * Arg #4 is optional and, if given, expected to be a l_int32 (iend).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 boolean and Numa* on the Lua stack
 */
static int
Join(lua_State *L)
{
    FUNC(LL_BOXAA ".Join");
    Boxaa *boxaad = ll_check_Boxaa(_fun, L, 1);
    Boxaa *boxaas = ll_check_Boxaa(_fun, L, 2);
    l_int32 istart = ll_check_index(_fun, L, 3, 1);
    l_int32 iend = ll_check_index(_fun, L, 3, boxaaGetCount(boxaas));
    lua_pushboolean(L, 0 == boxaaJoin(boxaad, boxaas, istart, iend));
    return 2;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_BOXAA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data
 */
Boxaa *
ll_check_Boxaa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Boxaa **>(ll_check_udata(_fun, L, arg, LL_BOXAA)));
}

/**
 * \brief Optionally expect a LL_BOXAA at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data
 */
Boxaa *
ll_check_Boxaa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Boxaa(_fun, L, arg);
}

/**
 * \brief Push Boxaa* user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param boxaa pointer to the BOXAA
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_push_Boxaa(const char *_fun, lua_State *L, Boxaa *boxaa)
{
    if (!boxaa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_BOXAA, boxaa);
}

/**
 * \brief Create and push a new Boxaa*
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_new_Boxaa(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Boxaa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},               /* garbage collect */
        {"__new",               Create},                /* new Boxaa */
        {"__len",               GetCount},              /* #boxa */
        {"__tostring",          toString},
        {"Destroy",             Destroy},
        {"Copy",                Copy},
        {"GetCount",            GetCount},
        {"GetBoxCount",         GetBoxCount},
        {"AddBoxa",             AddBoxa},
        {"ExtendArray",         ExtendArray},
        {"ExtendArrayToSize",   ExtendArrayToSize},
        {"GetBoxa",             GetBoxa},
        {"GetBox",              GetBox},
        {"ReplaceBoxa",         ReplaceBoxa},
        {"InsertBoxa",          InsertBoxa},
        {"RemoveBoxa",          RemoveBoxa},
        {"FlattenToBoxa",       FlattenToBoxa},
        {"FlattenAligned",      FlattenAligned},
        {"Join",                Join},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_BOXAA, methods, functions);
    lua_setglobal(L, LL_BOXAA);
    return res;
}
