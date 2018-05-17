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
 *  Lua class PIXAA
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIXAA
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIX contained in the user data
 */
PIXAA *
ll_check_PIXAA(lua_State *L, int arg)
{
    return *(PIXAA **)ll_check_udata(L, arg, LL_PIXAA);
}

/**
 * \brief Push PIXAA* user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param boxa pointer to the PIXAA
 * \return 1 PIXAA* on the Lua stack
 */
int
ll_push_PIXAA(lua_State *L, PIXAA *pixaa)
{
    if (NULL == pixaa)
        return 0;
    return ll_push_udata(L, LL_PIXAA, pixaa);
}

/**
 * \brief Create a new PIXAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXAA* on the Lua stack
 */
int
ll_new_PIXAA(lua_State *L)
{
    l_int32 n = ll_check_l_int32_default(L, 1, 1);
    PIXAA *pixaa = pixaaCreate(n);
    return ll_push_PIXAA(L, pixaa);
}

/**
 * \brief Create a new PIXAA*
 *
 * Arg #1 is expected to be a l_int32 (n)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXAA* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_PIXAA(L);
}

/**
 * \brief Create a new PIXAA* from a PIXA*
 *
 * Arg #1 is expected to be a PIXA* use data
 * Arg #2 is expected to be a l_int32 (n)
 * Arg #3 is expected to be a string (type = "consecutive", "skip_by")
 * Arg #4 is optionally a string (copyflag)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXAA* on the Lua stack
 */
static int
CreateFromPixa(lua_State *L)
{
    PIXA *pixa = ll_check_PIXA(L, 1);
    l_int32 n = ll_check_l_int32_default(L, 2, 1);
    l_int32 type = ll_check_consecutive_skip_by(L, 3, L_CHOOSE_CONSECUTIVE);
    l_int32 copyflag = ll_check_access_storage(L, 4, L_CLONE);
    PIXAA *pixaa = pixaaCreateFromPixa(pixa, n, type, copyflag);
    return ll_push_PIXAA(L, pixaa);
}

/**
 * \brief Destroy a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **ppixaa = ll_check_udata(L, 1, LL_PIXAA);
    DBG(LOG_DESTROY, "%s: '%s' ppaa=%p paa=%p\n", __func__,
        LL_PIXAA, (void *)ppixaa, *ppixaa);
    pixaaDestroy((PIXAA **)ppixaa);
    *ppixaa = NULL;
    return 0;
}

/**
 * \brief Add a PIX* and its BOX* to a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a PIX* user data (pix)
 * Arg #4 is expected to be a BOX* user data (box)
 * Arg #5 is optionally a string defining the copyflag
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    l_int32 idx = ll_check_l_int32(L, 2) - 1;
    PIX *pix = ll_check_PIX(L, 3);
    BOX *box = ll_check_BOX(L, 4);
    l_int32 copyflag = ll_check_access_storage(L, 5, L_COPY);
    lua_pushboolean(L, 0 == pixaaAddPix(pixaa, idx, pix, box, copyflag));
    return 1;
}

/**
 * \brief Add a BOX* to a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is optionally a string defining the copyflag
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 copyflag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == pixaaAddBox(pixaa, box, copyflag));
    return 1;
}

/**
 * \brief Add a PIXA* to a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be a PIXA* user data
 * Arg #3 is optionally a string defining the copyflag
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPixa(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    PIXA *pixa = ll_check_PIXA(L, 2);
    l_int32 flag = ll_check_access_storage(L, 3, L_COPY);
    lua_pushboolean(L, 0 == pixaaAddPixa(pixaa, pixa, flag));
    return 1;
}

/**
 * \brief Extend array of a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    lua_pushboolean(L, 0 == pixaaExtendArray(pixaa));
    return 1;
}

/**
 * \brief Truncate array of a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    lua_pushboolean(L, 0 == pixaaTruncate(pixaa));
    return 1;
}

/**
 * \brief Replace the PIXA* in a PIXAA* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is expected to be a PIX* user data (pixa)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePixa(lua_State *L)
{
    PIXAA *paa = ll_check_PIXAA(L, 1);
    l_int32 idx = ll_check_index(L, 2, pixaaGetCount(paa, NULL));
    PIXA *pa = ll_check_PIXA(L, 3);
    lua_pushboolean(L, 0 == pixaaReplacePixa(paa, idx, pa));
    return 1;
}

/**
 * \brief Clear the PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    PIXAA *paa = ll_check_PIXAA(L, 1);
    lua_pushboolean(L, 0 == pixaaClear(paa));
    return 1;
}

/**
 * \brief Join the PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be another PIXAA* user data
 * Arg #3 is optional and expected to be a l_int32 (istart)
 * Arg #4 is optional and expected to be a l_int32 (iend)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    PIXAA *pixaad = ll_check_PIXAA(L, 1);
    PIXAA *pixaas = ll_check_PIXAA(L, 2);
    l_int32 istart = ll_check_l_int32_default(L, 3, 1) - 1;
    l_int32 iend = ll_check_l_int32_default(L, 3, pixaaGetCount(pixaas, NULL)) - 1;
    lua_pushboolean(L, 0 == pixaaJoin(pixaad, pixaas, istart, iend));
    return 1;
}

/**
 * \brief Get count for a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 *
 * \param L pointer to the lua_State
 * \return 1: integer on the Lua stack (count), or
 *         2: integer and a NUMA* on the Lua stack (count, PIXA* counts)
 */
static int
GetCount(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    NUMA *na = NULL;
    lua_pushinteger(L, pixaaGetCount(pixaa, &na));
    return 1 + ll_push_NUMA(L, na);
}

/**
 * \brief Get a PIXA* from a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is expected to be a l_int32 (idx)
 * Arg #3 is optionally a string defining the access flag (copy, clone)
 *
 * \param L pointer to the lua_State
 * \return 1: PIXA* on the Lua stack , or 0 on error
 */
static int
GetPixa(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    l_int32 idx = ll_check_l_int32(L, 2)- 1;
    l_int32 accesstype = ll_check_access_storage(L, 3, L_CLONE);
    PIXA *pixa = pixaaGetPixa(pixaa, idx, accesstype);
    return ll_push_PIXA(L, pixa);
}

/**
 * \brief Get a BOXA* from a PIXAA*
 *
 * Arg #1 (i.e. self) is expected to be a PIXAA* user data
 * Arg #2 is optionally a string defining the access flag (copy, clone)
 *
 * \param L pointer to the lua_State
 * \return 1: BOXA* on the Lua stack , or 0 on error
 */
static int
GetBoxa(lua_State *L)
{
    PIXAA *pixaa = ll_check_PIXAA(L, 1);
    l_int32 accesstype = ll_check_access_storage(L, 2, L_CLONE);
    BOXA *boxa = pixaaGetBoxa(pixaa, accesstype);
    return ll_push_BOXA(L, boxa);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PIXAA(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               Create},
        {"__len",               GetCount},
        {"ExtendArray",		ExtendArray},
        {"Truncate",		Truncate},
        {"Destroy",             Destroy},
        {"AddPix",              AddPix},
        {"AddBox",              AddBox},
        {"AddPixa",		AddPixa},
        {"GetCount",            GetCount},
        {"GetPixa",             GetPixa},
        {"GetBoxa",             GetBoxa},
        {"ReplacePixa",		ReplacePixa},
        {"Join",                Join},
        {"Clear",               Clear},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_PIXAA, methods, functions);
}
