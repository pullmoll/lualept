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
 *  Lua class BOXA
 *
 *====================================================================*/

/**
 * \brief Printable string for a Boxa*
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    FUNC(LL_BOXA ".toString");
    static char str[256];
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i, x, y, w, h;

    luaL_buffinit(L, &B);
    if (!boxa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_BOXA ": %p",
                 reinterpret_cast<void *>(boxa));
        luaL_addstring(&B, str);
        for (i = 0; i < boxaGetCount(boxa); i++) {
            boxaGetBoxGeometry(boxa, i, &x, &y, &w, &h);
            snprintf(str, sizeof(str), "\n    %d = { x = %d, y = %d, w = %d, h = %d }",
                     i+1, x, y, w, h);
            luaL_addstring(&B, str);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new Boxa*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_BOXA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Boxa *boxa = boxaCreate(n);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Destroy a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_BOXA ".Destroy");
    Boxa **pboxa = reinterpret_cast<Boxa **>(ll_check_udata(_fun, L, 1, LL_BOXA));
    DBG(LOG_DESTROY, "%s: '%s' pboxa=%p boxa=%p\n",
        _fun, LL_BOXA, pboxa, *pboxa);
    boxaDestroy(pboxa);
    *pboxa = nullptr;
    return 0;
}

/**
 * \brief Copy a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is an optional string defining the storage flags (copy, clone,.
 * copy_clone)
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_BOXA ".Copy");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxa *boxa = boxaCopy(boxas, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Add a Box* to a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a Box*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    FUNC(LL_BOXA ".AddBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    lua_pushboolean(L, 0 == boxaAddBox(boxa, box, flag));
    return 1;
}

/**
 * \brief Extend a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    FUNC(LL_BOXA ".ExtendArray");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushboolean(L, 0 == boxaExtendArray(boxa));
    return 1;
}

/**
 * \brief Extend a Boxa* to a given size %n
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    FUNC(LL_BOXA ".ExtendArrayToSize");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == boxaExtendArrayToSize(boxa, n));
    return 1;
}

/**
 * \brief Get count for a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_BOXA ".GetCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushinteger(L, boxaGetCount(boxa));
    return 1;
}

/**
 * \brief Get valid count for a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetValidCount(lua_State *L)
{
    FUNC(LL_BOXA ".GetValidCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushinteger(L, boxaGetValidCount(boxa));
    return 1;
}

/**
 * \brief Get Box* from a Boxa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    FUNC(LL_BOXA ".GetBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *box = boxaGetBox(boxa, idx, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get valid Box* from a Boxa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
GetValidBox(lua_State *L)
{
    FUNC(LL_BOXA ".GetValidBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *box = boxaGetValidBox(boxa, idx, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Find invalid Box* in a Boxa* and return a Numa* of indices
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack, or nil if no invalid boxes
 */
static int
FindInvalidBoxes(lua_State *L)
{
    FUNC(LL_BOXA ".FindInvalidBoxes");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Numa *na = boxaFindInvalidBoxes(boxa);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Get the geometry for a Box* from a Boxa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 4 integers x, y, w, h or nil on error
 */
static int
GetBoxGeometry(lua_State *L)
{
    FUNC(LL_BOXA ".GetBoxGeometry");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 x, y, w, h;
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    if (boxaGetBoxGeometry(boxa, idx, &x, &y, &w, &h))
        return ll_push_nil(L);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Get the IsFull state for a Boxa*
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
IsFull(lua_State *L)
{
    FUNC(LL_BOXA ".IsFull");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    int isfull = 0;
    lua_pushboolean(L, 0 == boxaIsFull(boxa, &isfull) && isfull);
    return 1;
}

/**
 * \brief Replace the Box* in a Boxa* at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Box*.
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBox(lua_State *L)
{
    FUNC(LL_BOXA ".ReplaceBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *box = ll_check_Box(_fun, L, 3);
    lua_pushboolean(L, box && 0 == boxaReplaceBox(boxa, idx, box));
    return 1;
}

/**
 * \brief Insert the Box* (%box) in a Boxa* (%boxa) at index %idx
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Box* (boxs).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBox(lua_State *L)
{
    FUNC(LL_BOXA ".InsertBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *boxs = ll_check_Box(_fun, L, 3);
    Box *box = boxClone(boxs);
    lua_pushboolean(L, box && 0 == boxaInsertBox(boxa, idx, box));
    return 1;
}

/**
 * \brief Reomve the Box* from a Boxa* (%boxa) at index (%idx)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBox(lua_State *L)
{
    FUNC(LL_BOXA ".RemoveBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    lua_pushboolean(L, 0 == boxaRemoveBox(boxa, idx));
    return 1;
}

/**
 * \brief Reomve the Box* (%box) from a Boxa* (%boxa) at index (%idx) and push it
 * data
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack, or 0 in case of error
 */
static int
RemoveBoxAndSave(lua_State *L)
{
    FUNC(LL_BOXA ".RemoveBoxAndSave");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *box = nullptr;
    if (boxaRemoveBoxAndSave(boxa, idx, &box))
        return ll_push_nil(L);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Save the valid boxes in Boxa* (%boxas) and return the resulting Boxa* (%boxa)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is an optional string defining the storage flags (copy, clone).
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack; or nil on error
 */
static int
SaveValid(lua_State *L)
{
    FUNC(LL_BOXA ".SaveValid");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxa *boxa = boxaSaveValid(boxas, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Clear the Boxa* (%boxa)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    FUNC(LL_BOXA ".Clear");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushboolean(L, 0 == boxaClear(boxa));
    return 1;
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) contained within Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
ContainedInBox(lua_State *L)
{
    FUNC(LL_BOXA ".ContainedInBox");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Boxa *boxad = boxaContainedInBox(boxas, box);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Return the count of boxes from Boxa* (%boxas) contained within Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ContainedInBoxCount(lua_State *L)
{
    FUNC(LL_BOXA ".ContainedInBoxCount");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (boxaContainedInBoxCount(boxas, box, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Return true, if every box of Boxa* (%boxa2) is contained in a box of Boxa* (%boxa1)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be a Boxa* (boxa2).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ContainedInBoxa(lua_State *L)
{
    FUNC(LL_BOXA ".ContainedInBoxa");
    Boxa *boxa1 = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa2 = ll_check_Boxa(_fun, L, 2);
    l_int32 contained = 0;
    if (boxaContainedInBoxa(boxa1, boxa2, &contained))
        return ll_push_nil(L);
    lua_pushboolean(L, contained);
    return 1;
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) which intersect within Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
IntersectsBox(lua_State *L)
{
    FUNC(LL_BOXA ".IntersectsBox");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Boxa *boxad = boxaIntersectsBox(boxas, box);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Return the count of boxes from Boxa* (%boxa) which intersect with Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a Box* (box).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
IntersectsBoxCount(lua_State *L)
{
    FUNC(LL_BOXA ".IntersectsBoxCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (boxaIntersectsBoxCount(boxa, box, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Rotate a Boxa* (%boxas)
 *
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
RotateOrth(lua_State *L)
{
    FUNC(LL_BOXA ".RotateOrth");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    l_int32 rotation = ll_check_rotation(_fun, L, 4, 0);
    Boxa *boxa = boxaRotateOrth(boxas, w, h, rotation);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_BOXA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_check_Boxa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Boxa **>(ll_check_udata(_fun, L, arg, LL_BOXA)));
}

/**
 * \brief Optionally expect a LL_BOXA at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_check_Boxa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Boxa(_fun, L, arg);
}

/**
 * \brief Push Boxa* user data to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param boxa pointer to the BOXA
 * \return 1 Boxa* on the Lua stack
 */
int
ll_push_Boxa(const char *_fun, lua_State *L, Boxa *boxa)
{
    if (!boxa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_BOXA, boxa);
}

/**
 * \brief Create and push a new Boxa*
 *
 * Arg #1 is expected to be a l_int32 (n).
 *
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
int
ll_new_Boxa(lua_State *L)
{
    return Create(L);
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
        {"__new",               Create},                /* new Boxa */
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
        {"ContainedInBox",      ContainedInBox},
        {"ContainedInBoxCount", ContainedInBoxCount},
        {"ContainedInBoxa",     ContainedInBoxa},
        {"IntersectsBox",       IntersectsBox},
        {"IntersectsBoxCount",  IntersectsBoxCount},
        {"RotateOrth",          RotateOrth},
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
