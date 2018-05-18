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
 * @brief Check Lua stack at index %arg for udata of class LL_BOXAA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data
 */
Boxaa *
ll_check_Boxaa(lua_State *L, int arg)
{
    return *(reinterpret_cast<Boxaa **>(ll_check_udata(L, arg, LL_BOXAA)));
}

/**
 * \brief Push Boxaa* user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param boxaa pointer to the BOXAA
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_push_Boxaa(lua_State *L, Boxaa *boxaa)
{
    if (!boxaa)
        return 0;
    return ll_push_udata(L, LL_BOXAA, boxaa);
}

/**
 * \brief Create and push a new Boxaa*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_new_Boxaa(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(__func__, L, 1, 1);
    Boxaa *boxaa = boxaaCreate(n);
    return ll_push_Boxaa(L, boxaa);
}

/**
 * \brief Create a new Boxaa*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Boxaa(L);
}

/**
 * \brief Destroy a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    Boxaa **pboxaa = reinterpret_cast<Boxaa **>(ll_check_udata(L, 1, LL_BOXAA));
    DBG(LOG_DESTROY, "%s: '%s' pboxaa=%p boxaa=%p\n",
        __func__, LL_BOXAA, pboxaa, *pboxaa);
    boxaaDestroy(pboxaa);
    *pboxaa = nullptr;
    return 0;
}

/**
 * @brief Printable string for a Boxaa*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    static char str[256];
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    luaL_Buffer B;
    l_int32 i, j, x, y, w, h;

    luaL_buffinit(L, &B);
    if (!boxaa) {
        luaL_addstring(&B, "nil");
    } else {
        luaL_addchar(&B, '{');
        for (i = 0; i < boxaaGetCount(boxaa); i++) {
            Boxa *boxa = boxaaGetBoxa(boxaa, i, L_CLONE);
            if (i > 0)
                luaL_addchar(&B, ',');
            snprintf(str, sizeof(str), "%d={", i+1);
            luaL_addstring(&B, str);
            for (j = 0; j < boxaGetCount(boxa); j++) {
                boxaGetBoxGeometry(boxa, j, &x, &y, &w, &h);
                snprintf(str, sizeof(str), "%d={%d,%d,%d,%d}", j+1, x, y, w, h);
                if (j > 0)
                    luaL_addchar(&B, ',');
                luaL_addstring(&B, str);
            }
            luaL_addchar(&B, '}');
        }
        luaL_addchar(&B, '}');
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Copy a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is an optional string defining the storage flags (copy, clone,
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    Boxaa *boxaas = ll_check_Boxaa(L, 1);
    l_int32 copyflag = ll_check_access_storage(L, 2, L_COPY);
    Boxaa *boxaa = boxaaCopy(boxaas, copyflag);
    return ll_push_Boxaa(L, boxaa);
}

/**
 * \brief Add a Box* to a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a Boxa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    Boxa *boxa = ll_check_Boxa(L, 2);
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == boxaaAddBoxa(boxaa, boxa, flag));
    return 1;
}

/**
 * \brief Extend a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    lua_pushboolean(L, 0 == boxaaExtendArray(boxaa));
    return 1;
}

/**
 * \brief Extend a Boxaa* to a given size %n
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 n = ll_check_l_int32(__func__, L, 2);
    lua_pushboolean(L, 0 == boxaaExtendArrayToSize(boxaa, n));
    return 1;
}

/**
 * \brief Get count for a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    lua_pushinteger(L, boxaaGetCount(boxaa));
    return 1;
}

/**
 * \brief Copy a Boxaa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxCount(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    lua_pushinteger(L, boxaaGetBoxCount(boxaa));
    return 1;
}

/**
 * \brief Get Boxa* from a Boxaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is an optional string defining the storage flags (copy, clone).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, boxaaGetCount(boxaa));
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    Boxa *boxa = boxaaGetBoxa(boxaa, idx, flag);
    return ll_push_Boxa(L, boxa);
}

/**
 * \brief Get Box* from a Boxaa* at index %iboxa and %ibox
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (iboxa)
 * Arg #2 is expected to be a l_int32 (ibox)
 * Arg #3 is an optional string defining the storage flags (copy, clone).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 iboxa = ll_check_index(__func__, L, 2, boxaaGetCount(boxaa));
    l_int32 ibox = ll_check_index(__func__, L, 3, INT32_MAX);
    l_int32 flag = ll_check_access_storage(L, 4, L_COPY);
    Box *box = boxaaGetBox(boxaa, iboxa, ibox, flag);
    return ll_push_Box(L, box);
}

/**
 * \brief Replace the Box* in a Boxaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a Boxa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, boxaaGetCount(boxaa));
    Boxa *boxa = ll_check_Boxa(L, 3);
    lua_pushboolean(L, boxa && 0 == boxaaReplaceBoxa(boxaa, idx, boxa));
    return 1;
}

/**
 * \brief Insert the Boxa* in a Boxaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a Boxa* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, boxaaGetCount(boxaa));
    Boxa *boxas = ll_check_Boxa(L, 3);
    Boxa *boxa = boxaCopy(boxas, L_CLONE);
    lua_pushboolean(L, boxa && 0 == boxaaInsertBoxa(boxaa, idx, boxa));
    return 1;
}

/**
 * \brief Reomve the Boxa* from a Boxaa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data
 * Arg #2 is expected to be a l_int32 (%idx)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 idx = ll_check_index(__func__, L, 2, boxaaGetCount(boxaa));
    lua_pushboolean(L, 0 == boxaaRemoveBoxa(boxaa, idx));
    return 1;
}

/**
 * \brief Flatten the Boxaa* to a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data (boxaa)
 * Arg #2 is expected to be a string describing the copy flag (copyflag)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenToBoxa(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 copyflag = ll_check_access_storage(L, 2, L_COPY);
    Numa *naindex = nullptr;
    Boxa *boxa = boxaaFlattenToBoxa(boxaa, &naindex, copyflag);
    return ll_push_Boxa(L, boxa);
}

/**
 * \brief Aligned flatten the Boxaa* to a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxaa* user data (boxaa)
 * Arg #2 is expected to be a l_int32 (num)
 * Arg #3 is expected to be a string describing the copy flag (copyflag)
 * Arg #3 is optional and, if given, expected to be a Box* (fillerbox)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenAligned(lua_State *L)
{
    Boxaa *boxaa = ll_check_Boxaa(L, 1);
    l_int32 num = ll_check_l_int32(__func__, L, 2);
    l_int32 copyflag = ll_check_access_storage(L, 3, L_COPY);
    Box *fillerbox = lua_isuserdata(L, 4) ? ll_check_Box(L, 4) : nullptr;
    Boxa *boxa = boxaaFlattenAligned(boxaa, num, fillerbox, copyflag);
    return ll_push_Boxa(L, boxa);
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