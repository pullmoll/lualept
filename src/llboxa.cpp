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
 *  Lua class Boxa
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_BOXA */
#define LL_FUNC(x) FUNC(LL_BOXA "." x)

/**
 * \brief Destroy a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Boxa **pboxa = ll_check_udata<Boxa>(_fun, L, 1, LL_BOXA);
    Boxa *boxa = *pboxa;
    DBG(LOG_DESTROY, "%s: '%s' pboxa=%p boxa=%p count=%d\n",
        _fun, LL_BOXA, pboxa, boxa, boxaGetCount(boxa));
    boxaDestroy(&boxa);
    *pboxa = nullptr;
    return 0;
}

/**
 * \brief Create a new Boxa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Boxa *boxa = boxaCreate(n);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Get count for a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushinteger(L, boxaGetCount(boxa));
    return 1;
}

/**
 * \brief Printable string for a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
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
 * \brief Add a Box* to a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a Box*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    LL_FUNC("AddBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == boxaAddBox(boxa, box, flag));
}

/**
 * \brief Adjust height of boxes changing (%sides) in a Boxa* (%boxas) to a (%target).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the side (sides).
 * Arg #3 is expected to be a l_int32 (target).
 * Arg #4 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
AdjustHeightToTarget(lua_State *L)
{
    LL_FUNC("AdjustHeightToTarget");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 sides = ll_check_adjust_sides(_fun, L, 2, 0);
    l_int32 target = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    Boxa *boxad = boxaAdjustWidthToTarget(nullptr, boxas, sides, target, thresh);
    ll_push_Boxa(_fun, L, boxad);
    return 1;
}

/**
 * \brief Adjust sides of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (delleft).
 * Arg #3 is expected to be a l_int32 (delright).
 * Arg #4 is expected to be a l_int32 (deltop).
 * Arg #5 is expected to be a l_int32 (delbot).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
AdjustSides(lua_State *L)
{
    LL_FUNC("AdjustSides");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 delleft = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 delright = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 deltop = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 delbot = ll_check_l_int32_default(_fun, L, 5, 0);
    Boxa *boxad = boxaAdjustSides(boxas, delleft, delright, deltop, delbot);
    ll_push_Boxa(_fun, L, boxad);
    return 1;
}

/**
 * \brief Adjust width of boxes changing (%sides) in a Boxa* (%boxas) to a (%target).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a string describing the side (sides).
 * Arg #3 is expected to be a l_int32 (target).
 * Arg #4 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
AdjustWidthToTarget(lua_State *L)
{
    LL_FUNC("AdjustWidthToTarget");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 sides = ll_check_adjust_sides(_fun, L, 2, 0);
    l_int32 target = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    Boxa *boxad = boxaAdjustWidthToTarget(nullptr, boxas, sides, target, thresh);
    ll_push_Boxa(_fun, L, boxad);
    return 1;
}

/**
 * \brief Affine transformation of the boxes in the Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a table of 3x3 l_float32 (mat).
 * or
 * Arg #2 .. 10 is expected to be l_float32 (mat[0..8]).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
AffineTransform(lua_State *L)
{
    LL_FUNC("AffineTransform");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 i, n;
    l_float32 *mat;
    if (lua_istable(L, 2)) {
        /* expect a table of 9 numbers */
        ll_unpack_Farray(_fun, L, 2, &n);
    } else {
        /* expect 9 numbers */
        n = 9;
        mat = ll_calloc<l_float32>(_fun, L, n);
        for (i = 0; i < n; i++)
            mat[i] = ll_check_l_float32(_fun, L, 2+i);
    }
    if (n < 9) {
        lua_pushfstring(L, "%s: wrong matrix size (%d), expected 3x3", _fun, n);
        lua_error(L);
        return 0;
    }
    Boxa *boxa = boxaAffineTransform(boxas, mat);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Sort a (large number of) Boxa* (%boxas) by given type (%type) and order (%order).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string defining the sort type (type).
 * Arg #3 is expected to be a string defining the sort order (order).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* (boxa) and Numa* (naindex) on the Lua stack
 */
static int
BinSort(lua_State *L)
{
    LL_FUNC("BinSort");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 type = ll_check_sort_by(_fun, L, 2, L_SORT_BY_X);
    l_int32 order = ll_check_sort_order(_fun, L, 3, L_SORT_INCREASING);
    Numa *naindex = nullptr;
    Boxa *boxa = boxaBinSort(boxas, type, order, &naindex);
    return ll_push_Boxa(_fun, L, boxa) + ll_push_Numa(_fun, L, naindex);
}

/**
 * \brief Clear the Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == boxaClear(boxa));
}

/**
 * \brief Clip the boxes of Boxa* (%boxa) to a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClipToBox(lua_State *L)
{
    LL_FUNC("ClipToBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaClipToBox(boxa, box));
}

/**
 * \brief Combine overlaps in boxes of Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is optional and, if given, expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
CombineOverlaps(lua_State *L)
{
    LL_FUNC("CombineOverlaps");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Pixa *pixadb = ll_check_Pixa_opt(_fun, L, 2);
    Boxa *boxad = boxaCombineOverlaps(boxas, pixadb);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Combine overlaps in pairs of boxes of two Boxa* (%boxa1, %boxa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be a another Boxa* (boxa2).
 * Arg #3 is optional and, if given, expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* on the Lua stack
 */
static int
CombineOverlapsInPair(lua_State *L)
{
    LL_FUNC("CombineOverlapsInPair");
    Boxa *boxa1 = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa2 = ll_check_Boxa(_fun, L, 2);
    Pixa *pixadb = ll_check_Pixa_opt(_fun, L, 3);
    Boxa *boxad1;
    Boxa *boxad2;
    if (boxaCombineOverlapsInPair(boxa1, boxa2, &boxad1, &boxad2, pixadb))
        return ll_push_nil(L);
    return ll_push_Boxa(_fun, L, boxad1) + ll_push_Boxa(_fun, L, boxad2);
}

/**
 * \brief Compare regions of Boxa* (%boxa1) with Boxa* (%boxa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be another Boxa* (boxa2).
 * Arg #3 is expected to be a l_int32 (areathresh).
 * Arg #3 is optional and, if given, expected to be a Pix* (pixdb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 one integer (%nsame) and two numbers (%diffarea, %diffxor) on the Lua stack
 */
static int
CompareRegions(lua_State *L)
{
    LL_FUNC("CompareRegions");
    Boxa *boxa1 = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa2 = ll_check_Boxa(_fun, L, 2);
    l_int32 areathresh = ll_check_l_int32(_fun, L, 3);
    Pix *pixdb = ll_check_Pix_opt(_fun, L, 4);
    l_int32 nsame = 0;
    l_float32 diffarea = 0.0f;
    l_float32 diffxor = 0.0f;
    if (boxaCompareRegions(boxa1, boxa2, areathresh, &nsame, &diffarea, &diffxor, &pixdb))
        return ll_push_nil(L);
    lua_pushinteger(L, nsame);
    lua_pushnumber(L, static_cast<lua_Number>(diffarea));
    lua_pushnumber(L, static_cast<lua_Number>(diffxor));
    return 3;
}

/**
 * \brief Constrain the size of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a string describing the adjust sides (widthflag).
 * Arg #4 is expected to be a l_int32 (height).
 * Arg #5 is expected to be a string describing the adjust sides (heightflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ConstrainSize(lua_State *L)
{
    LL_FUNC("ConstrainSize");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 width = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 widthflag = ll_check_adjust_sides(_fun, L, 3, L_ADJUST_LEFT_AND_RIGHT);
    l_int32 height = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 heightflag = ll_check_adjust_sides(_fun, L, 5, L_ADJUST_TOP_AND_BOT);
    Boxa *boxa = boxaConstrainSize(boxas, width, widthflag, height, heightflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) contained within Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
ContainedInBox(lua_State *L)
{
    LL_FUNC("ContainedInBox");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Boxa *boxad = boxaContainedInBox(boxas, box);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Return the count of boxes from Boxa* (%boxas) contained within Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ContainedInBoxCount(lua_State *L)
{
    LL_FUNC("ContainedInBoxCount");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (boxaContainedInBoxCount(boxas, box, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Return true, if every box of Boxa* (%boxa2) is contained in a box of Boxa* (%boxa1).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be a Boxa* (boxa2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ContainedInBoxa(lua_State *L)
{
    LL_FUNC("ContainedInBoxa");
    Boxa *boxa1 = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa2 = ll_check_Boxa(_fun, L, 2);
    l_int32 contained = 0;
    if (boxaContainedInBoxa(boxa1, boxa2, &contained))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, contained);
}

/**
 * \brief Convert boxes from Boxa* (%boxa) to a Pta* (%pta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (ncorners).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta* (%pta) on the Lua stack
 */
static int
ConvertToPta(lua_State *L)
{
    LL_FUNC("ConvertToPta");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 ncorners = ll_check_l_int32(_fun, L, 2);
    Pta *pta = boxaConvertToPta(boxa, ncorners);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Copy a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxa *boxa = boxaCopy(boxas, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Encapsulate Boxa* (%boxa) aligned into a Boxaa* (%boxaa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (num).
 * Arg #3 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* (%boxaa) on the Lua stack
 */
static int
EncapsulateAligned(lua_State *L)
{
    LL_FUNC("EncapsulateAligned");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 num = ll_check_l_int32(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Boxaa *boxaa = boxaEncapsulateAligned(boxa, num, copyflag);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Extend a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    LL_FUNC("ExtendArray");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == boxaExtendArray(boxa));
}

/**
 * \brief Extend a Boxa* to a given size %n.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    LL_FUNC("ExtendArrayToSize");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaExtendArrayToSize(boxa, n));
}

/**
 * \brief Extract Boxa* (%boxa) as six Numa* (%nal, %nar, %nat, %nab, %naw, %nah).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a boolean (keepinvalid).
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 Numa* on the Lua stack (%nal, %nar, %nat, %nab, %naw, %nah)
 */
static int
ExtractAsNuma(lua_State *L)
{
    LL_FUNC("ExtractAsNuma");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 keepinvalid = ll_check_boolean(_fun, L, 2);
    Numa *nal = nullptr;
    Numa *nar = nullptr;
    Numa *nat = nullptr;
    Numa *nab = nullptr;
    Numa *naw = nullptr;
    Numa *nah = nullptr;
    if (boxaExtractAsNuma(boxa, &nal, &nar, &nat, &nab, &naw, &nah, keepinvalid))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, nal) + ll_push_Numa(_fun, L, nar) +
            ll_push_Numa(_fun, L, nat) + ll_push_Numa(_fun, L, nab) +
            ll_push_Numa(_fun, L, naw) + ll_push_Numa(_fun, L, nah);
}

/**
 * \brief Extract Boxa* (%boxa) as six Pta* (%ptal, %ptar, %ptat, %ptab, %ptaw, %ptah).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is optional and, if given, expected to be a boolean (keepinvalid).
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 Pta* on the Lua stack (%ptal, %ptar, %ptat, %ptab, %ptaw, %ptah)
 */
static int
ExtractAsPta(lua_State *L)
{
    LL_FUNC("ExtractAsPta");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 keepinvalid = ll_check_boolean_default(_fun, L, 2, FALSE);
    Pta *ptal = nullptr;
    Pta *ptar = nullptr;
    Pta *ptat = nullptr;
    Pta *ptab = nullptr;
    Pta *ptaw = nullptr;
    Pta *ptah = nullptr;
    if (boxaExtractAsPta(boxa, &ptal, &ptar, &ptat, &ptab, &ptaw, &ptah, keepinvalid))
        return ll_push_nil(L);
    return ll_push_Pta(_fun, L, ptal) + ll_push_Pta(_fun, L, ptar) +
            ll_push_Pta(_fun, L, ptat) + ll_push_Pta(_fun, L, ptab) +
            ll_push_Pta(_fun, L, ptaw) + ll_push_Pta(_fun, L, ptah);
}

/**
 * \brief Extract a sorted pattern of boxes from a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack (%naa)
 */
static int
ExtractSortedPattern(lua_State *L)
{
    LL_FUNC("ExtractSortedPattern");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    Numaa *naa = boxaExtractSortedPattern(boxas, na);
    return ll_push_Numaa(_fun, L, naa);
}

/**
 * \brief Fill a sequence of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the use flag (useflag).
 * Arg #3 is optional and, if given, expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
FillSequence(lua_State *L)
{
    LL_FUNC("FillSequence");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 useflag = ll_check_useflag(_fun, L, 2, L_USE_ALL_BOXES);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    Boxa *boxa = boxaFillSequence(boxas, useflag, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Find invalid Box* in a Boxa* and return a Numa* of indices.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack, or nil if no invalid boxes
 */
static int
FindInvalidBoxes(lua_State *L)
{
    LL_FUNC("FindInvalidBoxes");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Numa *na = boxaFindInvalidBoxes(boxa);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Get Boxa* (%boxad) of nearest boxes from Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 *
 * Note:
 * x < 0 && y >= 0 horizontal line at y.
 * x >= 0 && y < 0 vertical line at x.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Numaa* on the Lua stack
 */
static int
FindNearestBoxes(lua_State *L)
{
    LL_FUNC("FindNearestBoxes");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 dist_select = ll_check_value_flags(_fun, L, 2, L_NON_NEGATIVE);
    l_int32 range = ll_check_l_int32(_fun, L, 3);
    Numaa *naaindex = nullptr;
    Numaa *naadist = nullptr;
    if (boxaFindNearestBoxes(boxas, dist_select, range, &naaindex, &naadist))
        return ll_push_nil(L);
    return ll_push_Numaa(_fun, L, naaindex) + ll_push_Numaa(_fun, L, naadist);
}

/**
 * \brief Get the area of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack (%area)
 */
static int
GetArea(lua_State *L)
{
    LL_FUNC("GetArea");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 area = 0;
    if (boxaGetArea(boxas, &area))
        return ll_push_nil(L);
    lua_pushinteger(L, area);
    return 1;
}

/**
 * \brief Get average size for Boxa* (%boxa) as two numbers (%w,%h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 numbers on the Lua stack (%w,%h)
 */
static int
GetAverageSize(lua_State *L)
{
    LL_FUNC("GetAverageSize");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 w = 0.0f;
    l_float32 h = 0.0f;
    if (boxaGetAverageSize(boxas, &w, &h))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(w));
    lua_pushnumber(L, static_cast<lua_Number>(h));
    return 2;
}

/**
 * \brief Get Box* from a Boxa* (%boxa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    LL_FUNC("GetBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *box = boxaGetBox(boxa, idx, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get the geometry for a Box* from a Boxa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers x, y, w, h on the Lua stack
 */
static int
GetBoxGeometry(lua_State *L)
{
    LL_FUNC("GetBoxGeometry");
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
 * \brief Get the extent of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (wc)
 * Arg #3 is expected to be a l_int32 (hc)
 * Arg #4 is optional and, if given, expected to be a boolean (exactflag)
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack (%fract)
 */
static int
GetCoverage(lua_State *L)
{
    LL_FUNC("GetCoverage");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    l_int32 exactflag = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 fract = 0.0f;
    if (boxaGetCoverage(boxas, wc, hc, exactflag, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(fract));
    return 1;
}

/**
 * \brief Get the extent of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 two integers (%w, %h) and a Box* (%box) on the Lua stack
 */
static int
GetExtent(lua_State *L)
{
    LL_FUNC("GetExtent");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    Box *box = nullptr;
    if (boxaGetExtent(boxas, &w, &h, &box))
        return ll_push_nil(L);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2 + ll_push_Box(_fun, L, box);;
}

/**
 * \brief Get median values for Boxa* (%boxa) as four integers (%x,%y,%w,%h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (%x,%y,%w,%h)
 */
static int
GetMedianVals(lua_State *L)
{
    LL_FUNC("GetMedianVals");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 x = 0;
    l_int32 y = 0;
    l_int32 w = 0;
    l_int32 h = 0;
    if (boxaGetMedianVals(boxas, &x, &y, &w, &h))
        return ll_push_nil(L);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Get index and distance for Box* at (%i) from Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a string describing the direction (dir).
 * Arg #4 is expected to be a string describing the value flag (dist_select).
 * Arg #5 is expected to be a l_int32 (range).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack
 */
static int
GetNearestByDirection(lua_State *L)
{
    LL_FUNC("GetNearestByDirection");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 i = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 dir = ll_check_direction(_fun, L, 3, L_FROM_LEFT);
    l_int32 dist_select = ll_check_value_flags(_fun, L, 4, L_NON_NEGATIVE);
    l_int32 range = ll_check_l_int32(_fun, L, 5);
    l_int32 index = 0;
    l_int32 dist = 0;
    if (boxaGetNearestByDirection(boxa, i, dir, dist_select, range, &index, &dist))
        return ll_push_nil(L);
    lua_pushinteger(L, index);
    lua_pushinteger(L, dist);
    return 2;
}

/**
 * \brief Get Box* (box) of Boxa* (%boxa) which is nearest to line (x,y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 *
 * Note:
 * x < 0 && y >= 0 horizontal line at y.
 * x >= 0 && y < 0 vertical line at x.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
GetNearestToLine(lua_State *L)
{
    LL_FUNC("GetNearestToLine");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    Box *box = boxaGetNearestToLine(boxa, x, y);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get Box* (box) of Boxa* (%boxa) which is nearest to point (x,y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
GetNearestToPt(lua_State *L)
{
    LL_FUNC("GetNearestToPt");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    Box *box = boxaGetNearestToPt(boxa, x, y);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get rank values for Boxa* (%boxa) as four integers (%x,%y,%w,%h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (%x,%y,%w,%h)
 */
static int
GetRankVals(lua_State *L)
{
    LL_FUNC("GetRankVals");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 fract = ll_check_l_float32(_fun, L, 2);
    l_int32 x = 0;
    l_int32 y = 0;
    l_int32 w = 0;
    l_int32 h = 0;
    if (boxaGetRankVals(boxas, fract, &x, &y, &w, &h))
        return ll_push_nil(L);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Get the size of boxes in a Boxa* (%boxas) as Numa* (%naw, %nah).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Numa* on the Lua stack (%naw, %nah)
 */
static int
GetSizes(lua_State *L)
{
    LL_FUNC("GetSizes");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numa *naw = nullptr;
    Numa *nah = nullptr;
    if (boxaGetSizes(boxas, &naw, &nah))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, naw) + ll_push_Numa(_fun, L, nah);
}

/**
 * \brief Get valid Box* from a Boxa* (%boxa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
GetValidBox(lua_State *L)
{
    LL_FUNC("GetValidBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *box = boxaGetValidBox(boxa, idx, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get valid count for a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetValidCount(lua_State *L)
{
    LL_FUNC("GetValidCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    lua_pushinteger(L, boxaGetValidCount(boxa));
    return 1;
}

/**
 * \brief Get white blocks for boxes from a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is optional and, if given, expected to be a Box* (box)
 * Arg #3 is expected to be a string describing the sort by type (sortflag).
 * Arg #4 is expected to be a l_int32 (maxboxes)
 * Arg #5 is expected to be a l_float32 (maxoverlap)
 * Arg #6 is expected to be a l_int32 (maxperim)
 * Arg #7 is expected to be a l_float32 (fract)
 * Arg #8 is expected to be a l_int32 (maxpops)
 *
 * Notes:
 *      (1) This uses the elegant Breuel algorithm, found in "Two
 *          Geometric Algorithms for Layout Analysis", 2002,
 *          url: "citeseer.ist.psu.edu/breuel02two.html".
 *          It starts with the bounding boxes (b.b.) of the connected
 *          components (c.c.) in a region, along with the rectangle
 *          representing that region.  It repeatedly divides the
 *          rectangle into four maximal rectangles that exclude a
 *          pivot rectangle, sorting them in a priority queue
 *          according to one of the six sort flags.  It returns a boxa
 *          of the "largest" set that have no intersection with boxes
 *          from the input boxas.
 *      (2) If box == NULL, the initial region is the minimal region
 *          that includes the origin and every box in boxas.
 *      (3) maxboxes is the maximum number of whitespace boxes that will
 *          be returned.  The actual number will depend on the image
 *          and the values chosen for maxoverlap and maxpops.  In many
 *          cases, the actual number will be 'maxboxes'.
 *      (4) maxoverlap allows pruning of whitespace boxes depending on
 *          the overlap.  To avoid all pruning, use maxoverlap = 1.0.
 *          To select only boxes that have no overlap with each other
 *          (maximal pruning), choose maxoverlap = 0.0.
 *          Otherwise, no box can have more than the 'maxoverlap' fraction
 *          of its area overlapped by any larger (in the sense of the
 *          sortflag) box.
 *      (5) Choose maxperim (actually, maximum half-perimeter) to
 *          represent a c.c. that is small enough so that you don't care
 *          about the white space that could be inside of it.  For all such
 *          c.c., the pivot for 'quadfurcation' of a rectangle is selected
 *          as having a reasonable proximity to the rectangle centroid.
 *      (6) Use fract in the range [0.0 ... 1.0].  Set fract = 0.0
 *          to choose the small box nearest the centroid as the pivot.
 *          If you choose fract > 0.0, it is suggested that you call
 *          boxaPermuteRandom() first, to permute the boxes (see usage below).
 *          This should reduce the search time for each of the pivot boxes.
 *      (7) Choose maxpops to be the maximum number of rectangles that
 *          are popped from the heap.  This is an indirect way to limit the
 *          execution time.  Use 0 for default (a fairly large number).
 *          At any time, you can expect the heap to contain about
 *          2.5 times as many boxes as have been popped off.
 *      (8) The output result is a sorted set of overlapping
 *          boxes, constrained by 'maxboxes', 'maxoverlap' and 'maxpops'.
 *      (9) The main defect of the method is that it abstracts out the
 *          actual components, retaining only the b.b. for analysis.
 *          Consider a component with a large b.b.  If this is chosen
 *          as a pivot, all white space inside is immediately taken
 *          out of consideration.  Furthermore, even if it is never chosen
 *          as a pivot, as the partitioning continues, at no time will
 *          any of the whitespace inside this component be part of a
 *          rectangle with zero overlapping boxes.  Thus, the interiors
 *          of all boxes are necessarily excluded from the union of
 *          the returned whitespace boxes.
 *     (10) It should be noted that the algorithm puts a large number
 *          of partels on the queue.  Setting a limit of X partels to
 *          remove from the queue, one typically finds that there will be
 *          several times that number (say, 2X - 3X) left on the queue.
 *          For an efficient algorithm to find the largest white or
 *          or black rectangles, without permitting them to overlap,
 *          see pixFindLargeRectangles().
 *     (11) USAGE: One way to accommodate to this weakness is to remove such
 *          large b.b. before starting the computation.  For example,
 *          if 'box' is an input image region containing 'boxa' b.b. of c.c.:
 *
 *          // Faster pivot choosing
 *          boxa:PermuteRandom();
 *
 *          // Remove anything either large width or height
 *          boxat = boxa:SelectBySize(maxwidth, maxheight, 'both', '<', nil)
 *
 *          boxad = boxat:GetWhiteblocks(box, type, maxboxes,
 *                                 maxoverlap, maxperim, fract,
 *                                 maxpops);
 *
 *          The result will be rectangular regions of "white space" that
 *          extend into (and often through) the excluded components.
 *     (11) As a simple example, suppose you wish to find the columns on a page.
 *          First exclude large c.c. that may block the columns, and then call:
 *
 *          boxad = boxa:GetWhiteblocks(box, 'height',
 *                             20, 0.15, 200, 0.2, 2000);
 *
 *          to get the 20 tallest boxes with no more than 0.15 overlap
 *          between a box and any of the taller ones, and avoiding the
 *          use of any c.c. with a b.b. half perimeter greater than 200
 *          as a pivot.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
GetWhiteblocks(lua_State *L)
{
    LL_FUNC("GetWhiteblocks");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 sortflag = ll_check_sort_by(_fun, L, 3, L_SORT_BY_WIDTH);
    l_int32 maxboxes = ll_check_l_int32_default(_fun, L, 4, 100);
    l_float32 maxoverlap = ll_check_l_float32_default(_fun, L, 5, 1.0f);
    l_int32 maxperim = ll_check_l_int32_default(_fun, L, 6, 40);
    l_float32 fract = ll_check_l_float32_default(_fun, L, 7, 0.0f);
    l_int32 maxpops = ll_check_l_int32_default(_fun, L, 8, 1000);
    Boxa *boxa = boxaGetWhiteblocks(boxas, box, sortflag, maxboxes, maxoverlap, maxperim, fract, maxpops);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Handle overlaps in boxes of Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the operation (op).
 * Arg #3 is expected to be a l_int32 (range).
 * Arg #4 is expected to be a l_float32 (min_overlap).
 * Arg #5 is expected to be a l_float32 (max_ratio).
 * Arg #6 is optional and, if given, expected to be a Numa* (namap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
HandleOverlaps(lua_State *L)
{
    LL_FUNC("HandleOverlaps");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 op = ll_check_overlap(_fun, L, 2, L_COMBINE);
    l_int32 range = ll_check_l_int32(_fun, L, 3);
    l_float32 min_overlap = ll_check_l_float32(_fun, L, 4);
    l_float32 max_ratio = ll_check_l_float32(_fun, L, 5);
    Numa *namap = ll_check_Numa_opt(_fun, L, 6);
    Boxa *boxad = boxaHandleOverlaps(boxas, op, range, min_overlap, max_ratio, &namap);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Insert the Box* (%box) in a Boxa* (%boxa) at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Box* (boxs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBox(lua_State *L)
{
    LL_FUNC("InsertBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *boxs = ll_check_Box(_fun, L, 3);
    Box *box = boxClone(boxs);
    return ll_push_boolean(_fun, L, box && 0 == boxaInsertBox(boxa, idx, box));
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) which intersect within Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
IntersectsBox(lua_State *L)
{
    LL_FUNC("IntersectsBox");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Boxa *boxad = boxaIntersectsBox(boxas, box);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Return the count of boxes from Boxa* (%boxa) which intersect with Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
IntersectsBoxCount(lua_State *L)
{
    LL_FUNC("IntersectsBoxCount");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (boxaIntersectsBoxCount(boxa, box, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the IsFull state for a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
IsFull(lua_State *L)
{
    LL_FUNC("IsFull");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    int isfull = 0;
    return ll_push_boolean(_fun, L, 0 == boxaIsFull(boxa, &isfull) && isfull);
}

/**
 * \brief Join Boxa* (%boxas) with Boxa* (%boxad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxad).
 * Arg #2 is expected to be another Boxa* (boxas).
 * Arg #3 is optional and, if given, expected to be a l_int32 (istart).
 * Arg #4 is optional and, if given, expected to be a l_int32 (iend).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Boxa *boxad = ll_check_Boxa(_fun, L, 1);
    Boxa *boxas = ll_check_Boxa(_fun, L, 2);
    l_int32 istart = ll_check_index(_fun, L, 3, 1);
    l_int32 iend = ll_check_index(_fun, L, 3, boxaGetCount(boxas));
    return ll_push_boolean(_fun, L, 0 == boxaJoin(boxad, boxas, istart, iend));
}

/**
 * \brief Linear fit for Boxa* (%boxas) giving Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is optional and, if given, expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
LinearFit(lua_State *L)
{
    LL_FUNC("LinearFit");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 3);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    Boxa *boxa = boxaLinearFit(boxas, factor, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Get the location range of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (%minx, %miny, %maxx, %maxy)
 */
static int
LocationRange(lua_State *L)
{
    LL_FUNC("LocationRange");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 minx = 0;
    l_int32 miny = 0;
    l_int32 maxx = 0;
    l_int32 maxy = 0;
    if (boxaLocationRange(boxas, &minx, &miny, &maxx, &maxy))
        return ll_push_nil(L);
    lua_pushinteger(L, minx);
    lua_pushinteger(L, miny);
    lua_pushinteger(L, maxx);
    lua_pushinteger(L, maxy);
    return 4;
}

/**
 * \brief Make indicator for boxes from Boxa* (%boxa) by size.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a l_int32 (height).
 * Arg #4 is expected to be a string describing the type (type).
 * Arg #5 is expected to be a string describing the relation (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* (%na) on the Lua stack
 */
static int
MakeSizeIndicator(lua_State *L)
{
    LL_FUNC("MakeSizeIndicator");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 height = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_select_size(_fun, L, 4, L_SELECT_WIDTH);
    l_int32 relation = ll_check_relation(_fun, L, 5, L_SELECT_IF_LT);
    Numa *na = boxaMakeSizeIndicator(boxas, width, height, type, relation);
    ll_push_Numa(_fun, L, na);
    return 1;
}

/**
 * \brief Make indicator for boxes from Boxa* (%boxa) by width/height ratio.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (ratio).
 * Arg #3 is expected to be a string describing the relation (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* (%na) on the Lua stack
 */
static int
MakeWHRatioIndicator(lua_State *L)
{
    LL_FUNC("MakeWHRatioIndicator");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 ratio = ll_check_l_float32(_fun, L, 2);
    l_int32 relation = ll_check_relation(_fun, L, 3, L_SELECT_IF_LT);
    Numa *na = boxaMakeWHRatioIndicator(boxas, ratio, relation);
    ll_push_Numa(_fun, L, na);
    return 1;
}

/**
 * \brief Merge even and odd Boxa* (%boxae, %boxao) into one Boxa* (%boxad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxae).
 * Arg #2 is expected to be a Boxa* (boxao).
 * Arg #3 is optional and, if gived, expected to be a l_int32 (fillflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
MergeEvenOdd(lua_State *L)
{
    LL_FUNC("MergeEvenOdd ");
    Boxa *boxae = ll_check_Boxa(_fun, L, 1);
    Boxa *boxao = ll_check_Boxa(_fun, L, 2);
    l_int32 fillflag = ll_check_l_int32_default(_fun, L, 3, 0);
    Boxa *boxa = boxaMergeEvenOdd(boxae, boxao, fillflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Modify a Boxa* (%boxas) with another Boxa* (%boxam) giving Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be another Boxa* (boxam).
 * Arg #3 is expected to be a string describing the sub flag (subflag).
 * Arg #4 is expected to be a l_int32 (maxdiff).
 * Arg #5 is expected to be a l_int32 (extrapixels).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ModifyWithBoxa(lua_State *L)
{
    LL_FUNC("ModifyWithBoxa");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Boxa *boxam = ll_check_Boxa(_fun, L, 2);
    l_int32 subflag = ll_check_subflag(_fun, L, 3, L_USE_MINSIZE);
    l_int32 maxdiff = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 extrapixels = ll_check_l_int32_default(_fun, L, 5, 0);
    Boxa *boxa = boxaModifyWithBoxa(boxas, boxam, subflag, maxdiff, extrapixels);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Permute boxes in Boxa* (%boxas) by a pseudo random algorithm.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
PermutePseudorandom(lua_State *L)
{
    LL_FUNC("PermutePseudorandom");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa = boxaPermutePseudorandom(boxas);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Permute boxes in Boxa* (%boxas) by a random algorithm.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxad) on the Lua stack
 */
static int
PermuteRandom(lua_State *L)
{
    LL_FUNC("PermuteRandom");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Boxa *boxad = boxaPermuteRandom(nullptr, boxas);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Get the area of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (maxoverlap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
PruneSortedOnOverlap(lua_State *L)
{
    LL_FUNC("PruneSortedOnOverlap");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 maxoverlap = ll_check_l_int32(_fun, L, 2);
    Boxa *boxa = boxaPruneSortedOnOverlap(boxas, maxoverlap);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Read a Boxa* (%boxa) from a file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Boxa *boxa = boxaRead(filename);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Read a Boxa* (%boxa) from memory (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    const char *data = ll_check_string(_fun, L, 1);
    lua_Integer size = luaL_len(L, 1);
    Boxa *boxa = boxaReadMem(reinterpret_cast<const l_uint8 *>(data), static_cast<size_t>(size));
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Read a Boxa* (%boxa) from a stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Boxa *boxa = boxaReadStream(stream->f);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Reconcile the height of even/odd boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the adjust sides (sides).
 * Arg #3 is expected to be a l_int32 (delh).
 * Arg #4 is expected to be a string describing the adjust choice (op).
 * Arg #5 is expected to be a l_float32 (factor).
 * Arg #6 is expected to be a l_int32 (start).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ReconcileEvenOddHeight(lua_State *L)
{
    LL_FUNC("ReconcileEvenOddHeight");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 sides = ll_check_adjust_sides(_fun, L, 2, L_ADJUST_TOP_AND_BOT);
    l_int32 delh = ll_check_l_int32(_fun, L, 3);
    l_int32 op = ll_check_adjust_sides(_fun, L, 4, L_ADJUST_CHOOSE_MIN);
    l_float32 factor = ll_check_l_float32_default(_fun, L, 5, 1.0f);
    l_int32 start = ll_check_index(_fun, L, 6, 1);
    Boxa *boxa = boxaReconcileEvenOddHeight(boxas, sides, delh, op, factor, start);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Reconcile the width of box pairs in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (delh).
 * Arg #3 is expected to be a string describing the adjust choice (op).
 * Arg #4 is expected to be a l_float32 (factor).
 * Arg #5 is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ReconcilePairWidth(lua_State *L)
{
    LL_FUNC("ReconcilePairWidth");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 delw = ll_check_l_int32(_fun, L, 2);
    l_int32 op = ll_check_adjust_sides(_fun, L, 3, L_ADJUST_CHOOSE_MIN);
    l_float32 factor = ll_check_l_float32_default(_fun, L, 4, 1.0f);
    Numa *na = ll_check_Numa(_fun, L, 5);
    Boxa *boxa = boxaReconcilePairWidth(boxas, delw, op, factor, na);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Reomve the Box* from a Boxa* (%boxa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBox(lua_State *L)
{
    LL_FUNC("RemoveBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    return ll_push_boolean(_fun, L, 0 == boxaRemoveBox(boxa, idx));
}

/**
 * \brief Reomve the Box* (%box) from a Boxa* (%boxa) at index (%idx) and push it.
 * data
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack, or 0 in case of error
 */
static int
RemoveBoxAndSave(lua_State *L)
{
    LL_FUNC("RemoveBoxAndSave");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *box = nullptr;
    if (boxaRemoveBoxAndSave(boxa, idx, &box))
        return ll_push_nil(L);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Replace the Box* in a Boxa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Box*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBox(lua_State *L)
{
    LL_FUNC("ReplaceBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    Box *box = ll_check_Box(_fun, L, 3);
    return ll_push_boolean(_fun, L, box && 0 == boxaReplaceBox(boxa, idx, box));
}

/**
 * \brief Rotate the boxes in the Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_float32 (xc).
 * Arg #3 is expected to be a l_float32 (yc).
 * Arg #4 is expected to be a l_float32 (angle).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
Rotate(lua_State *L)
{
    LL_FUNC("Rotate");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 xc = ll_check_l_float32_default(_fun, L, 2, 1.0f);
    l_float32 yc = ll_check_l_float32_default(_fun, L, 3, 1.0f);
    l_float32 angle = ll_check_l_float32_default(_fun, L, 4, 1.0f);
    Boxa *boxa = boxaRotate(boxas, xc, yc, angle);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Rotate a Boxa* (%boxas) orthogonally.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    l_int32 rotation = ll_check_rotation(_fun, L, 4, 0);
    Boxa *boxa = boxaRotateOrth(boxas, w, h, rotation);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Save the valid boxes in Boxa* (%boxas) and return the resulting Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is an optional string defining the storage flags (copy, clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack; or nil on error
 */
static int
SaveValid(lua_State *L)
{
    LL_FUNC("SaveValid");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxa *boxa = boxaSaveValid(boxas, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Scale the boxes in the Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
Scale(lua_State *L)
{
    LL_FUNC("Scale");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32_default(_fun, L, 2, 1.0f);
    l_float32 scaley = ll_check_l_float32_default(_fun, L, 3, 1.0f);
    Boxa *boxa = boxaScale(boxas, scalex, scaley);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Select boxes from Boxa* (%boxa) by size.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a l_int32 (height).
 * Arg #4 is expected to be a string describing the type (type).
 * Arg #5 is expected to be a string describing the relation (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* (%boxa) and integer (%changed) on the Lua stack
 */
static int
SelectBySize(lua_State *L)
{
    LL_FUNC("SelectBySize");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 height = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_select_size(_fun, L, 4, L_SELECT_WIDTH);
    l_int32 relation = ll_check_relation(_fun, L, 5, L_SELECT_IF_LT);
    l_int32 changed;
    Boxa *boxa = boxaSelectBySize(boxas, width, height, type, relation, &changed);
    ll_push_Boxa(_fun, L, boxa);
    lua_pushinteger(L, changed);
    return 2;
}

/**
 * \brief Select boxes from Boxa* (%boxa) by width/height ratio.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (ratio).
 * Arg #3 is expected to be a string describing the relation (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* (%boxa) and integer (%changed) on the Lua stack
 */
static int
SelectByWHRatio(lua_State *L)
{
    LL_FUNC("SelectByWHRatio");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 ratio = ll_check_l_float32(_fun, L, 2);
    l_int32 relation = ll_check_relation(_fun, L, 3, L_SELECT_IF_LT);
    l_int32 changed;
    Boxa *boxa = boxaSelectByWHRatio(boxas, ratio, relation, &changed);
    ll_push_Boxa(_fun, L, boxa);
    lua_pushinteger(L, changed);
    return 2;
}

/**
 * \brief Select a large box in the upper, left area of Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (areaslop).
 * Arg #3 is expected to be a l_int32 (yslop).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* (%box) on the Lua stack
 */
static int
SelectLargeULBox(lua_State *L)
{
    LL_FUNC("SelectLargeULBox");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_float32 areaslop = ll_check_l_float32(_fun, L, 2);
    l_int32 yslop = ll_check_l_int32(_fun, L, 3);
    Box *box = boxaSelectLargeULBox(boxa, areaslop, yslop);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Select a range (%istart, %iend) of boxes from Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (first).
 * Arg #3 is expected to be a l_int32 (last).
 * Arg #4 is an optional string defining the storage flags (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* (%boxaa) on the Lua stack
 */
static int
SelectRange(lua_State *L)
{
    LL_FUNC("SelectRange");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 first = ll_check_index(_fun, L, 2, boxaGetCount(boxas));
    l_int32 last = ll_check_index(_fun, L, 3, boxaGetCount(boxas));
    l_int32 copyflag = ll_check_access_storage(_fun, L, 4, L_COPY);
    Boxa *boxa = boxaSelectRange(boxas, first, last, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Select boxes from Boxa* (%boxa) with indicator (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a Numa* (na)
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* (%boxa) and integer (%changed) on the Lua stack
 */
static int
SelectWithIndicator(lua_State *L)
{
    LL_FUNC("SelectWithIndicator");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    l_int32 changed;
    Boxa *boxa = boxaSelectWithIndicator(boxas, na, &changed);
    ll_push_Boxa(_fun, L, boxa);
    lua_pushinteger(L, changed);
    return 2;
}

/**
 * \brief Set a side of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a string describing the side (side).
 * Arg #3 is expected to be a l_int32 (val).
 * Arg #4 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
SetSide(lua_State *L)
{
    LL_FUNC("SetSide");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 side = ll_check_set_side(_fun, L, 2, 0);
    l_int32 val = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    Boxa *boxad = boxaSetSide(nullptr, boxas, side, val, thresh);
    ll_push_Boxa(_fun, L, boxad);
    return 1;
}

/**
 * \brief Test similarity of a Boxa* (%boxa1) and another Boxa* (%boxa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be another Boxa* (boxa2).
 * Arg #3 is expected to be a l_int32 (leftdiff).
 * Arg #4 is expected to be a l_int32 (rightdiff).
 * Arg #5 is expected to be a l_int32 (topdiff).
 * Arg #6 is expected to be a l_int32 (botdiff).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* (nasim) on the Lua stack
 */
static int
Similar(lua_State *L)
{
    LL_FUNC("Similar");
    Boxa *boxa1 = ll_check_Boxa(_fun, L, 1);
    Boxa *boxa2 = ll_check_Boxa(_fun, L, 2);
    l_int32 leftdiff = ll_check_l_int32(_fun, L, 3);
    l_int32 rightdiff = ll_check_l_int32_default(_fun, L, 4, leftdiff);
    l_int32 topdiff = ll_check_l_int32_default(_fun, L, 5, rightdiff);
    l_int32 botdiff = ll_check_l_int32_default(_fun, L, 6, topdiff);
    l_int32 similar = FALSE;
    Numa *nasim = nullptr;
    if (boxaSimilar(boxa1, boxa2, leftdiff, rightdiff, topdiff, botdiff, 0, &similar, &nasim))
        return ll_push_nil(L);
    ll_push_boolean(_fun, L, similar);
    ll_push_Numa(_fun, L, nasim);
    return 2;
}

/**
 * \brief Get the size range of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (%minw, %minh, %maxw, %maxh)
 */
static int
SizeRange(lua_State *L)
{
    LL_FUNC("SizeRange");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 minw = 0;
    l_int32 minh = 0;
    l_int32 maxw = 0;
    l_int32 maxh = 0;
    if (boxaSizeRange(boxas, &minw, &minh, &maxw, &maxh))
        return ll_push_nil(L);
    lua_pushinteger(L, minw);
    lua_pushinteger(L, minh);
    lua_pushinteger(L, maxw);
    lua_pushinteger(L, maxh);
    return 4;
}

/**
 * \brief Determine the size variation of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the select size (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 numbers on the Lua stack (%del_evenodd, %rms_even, %rms_odd, %rms_all)
 */
static int
SizeVariation(lua_State *L)
{
    LL_FUNC("SizeVariation");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 type = ll_check_select_size(_fun, L, 2, L_SELECT_WIDTH);
    l_float32 del_evenodd = 0.0f;
    l_float32 rms_even = 0.0f;
    l_float32 rms_odd = 0.0f;
    l_float32 rms_all = 0.0f;
    if (boxaSizeVariation(boxas, type, &del_evenodd, &rms_even, &rms_odd, &rms_all))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(del_evenodd));
    lua_pushnumber(L, static_cast<lua_Number>(rms_even));
    lua_pushnumber(L, static_cast<lua_Number>(rms_odd));
    lua_pushnumber(L, static_cast<lua_Number>(rms_all));
    return 4;
}

/**
 * \brief Smooth sequence by least square fit for Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a string describing the sub flag (subflag).
 * Arg #4 is expected to be a l_int32 (maxdiff).
 * Arg #5 is expected to be a l_int32 (extrapixels).
 * Arg #5 is optional and, if given, expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
SmoothSequenceLS(lua_State *L)
{
    LL_FUNC("SmoothSequenceLS");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 3);
    l_int32 subflag = ll_check_subflag(_fun, L, 3, L_USE_MINSIZE);
    l_int32 maxdiff = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 extrapixels = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 debug = ll_check_boolean_default(_fun, L, 6, FALSE);
    Boxa *boxa = boxaSmoothSequenceLS(boxas, factor, subflag, maxdiff, extrapixels, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Smooth sequence by median for Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (halfwin).
 * Arg #3 is expected to be a string describing the sub flag (subflag).
 * Arg #4 is expected to be a l_int32 (maxdiff).
 * Arg #5 is expected to be a l_int32 (extrapixels).
 * Arg #5 is optional and, if given, expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
SmoothSequenceMedian(lua_State *L)
{
    LL_FUNC("SmoothSequenceMedian");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 halfwin = ll_check_l_int32_default(_fun, L, 2, 20);
    l_int32 subflag = ll_check_subflag(_fun, L, 3, L_USE_MINSIZE);
    l_int32 maxdiff = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 extrapixels = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 debug = ll_check_boolean_default(_fun, L, 6, FALSE);
    Boxa *boxa = boxaSmoothSequenceMedian(boxas, halfwin, subflag, maxdiff, extrapixels, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Sort a Boxa* (%boxas) by given type (%type) and order (%order).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string defining the sort type (type).
 * Arg #3 is expected to be a string defining the sort order (order).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* (boxa) and Numa* (naindex) on the Lua stack
 */
static int
Sort(lua_State *L)
{
    LL_FUNC("Sort");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 type = ll_check_sort_by(_fun, L, 2, L_SORT_BY_X);
    l_int32 order = ll_check_sort_order(_fun, L, 3, L_SORT_INCREASING);
    Numa *naindex = nullptr;
    Boxa *boxa = boxaSort(boxas, type, order, &naindex);
    return ll_push_Boxa(_fun, L, boxa) + ll_push_Numa(_fun, L, naindex);
}

/**
 * \brief Sort a Boxa* (%boxas) in two dimensions into Boxaa* (boxaa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (delta1).
 * Arg #3 is expected to be a l_int32 (delta2).
 * Arg #4 is expected to be a l_int32 (minh1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxaa* (%boxaa) and Numaa* (%naad) on the Lua stack
 */
static int
Sort2d(lua_State *L)
{
    LL_FUNC("Sort2d");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 delta1 = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 delta2 = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 minh1 = ll_check_l_int32_default(_fun, L, 4, 5);
    Numaa *naad = nullptr;
    Boxaa *boxaa = boxaSort2d(boxas, &naad, delta1, delta2, minh1);
    return ll_push_Boxaa(_fun, L, boxaa) + ll_push_Numaa(_fun, L, naad);
}

/**
 * \brief Sort a Boxa* (%boxas) by index (%naa) in two dimensions into Boxaa* (boxaa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (delta1).
 * Arg #3 is expected to be a l_int32 (delta2).
 * Arg #4 is expected to be a l_int32 (minh1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* (%boxaa) on the Lua stack
 */
static int
Sort2dByIndex(lua_State *L)
{
    LL_FUNC("Sort2dByIndex");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numaa *naa = ll_check_Numaa(_fun, L, 2);
    Boxaa *boxaa = boxaSort2dByIndex(boxas, naa);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Sort a Boxa* (%boxas) by index.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Numa* (naindex).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (boxa) on the Lua stack
 */
static int
SortByIndex(lua_State *L)
{
    LL_FUNC("SortByIndex");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numa *naindex = ll_check_Numa(_fun, L, 2);
    Boxa *boxa = boxaSortByIndex(boxas, naindex);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Split Boxa* (%boxa) into even and odd (%boxae, %boxao).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is optional and, if gived, expected to be a l_int32 (fillflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* on the Lua stack (boxae, boxao)
 */
static int
SplitEvenOdd(lua_State *L)
{
    LL_FUNC("SplitEvenOdd ");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 fillflag = ll_check_l_int32_default(_fun, L, 2, 0);
    Boxa *boxae = nullptr;
    Boxa *boxao = nullptr;
    if (boxaSplitEvenOdd(boxa, fillflag, &boxae, &boxao))
        return ll_push_nil(L);
    return ll_push_Boxa(_fun, L, boxae) + ll_push_Boxa(_fun, L, boxao);
}

/**
 * \brief Swap boxes (%i, %j) in Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SwapBoxes(lua_State *L)
{
    LL_FUNC("SwapBoxes");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 i = ll_check_index(_fun, L, 2, boxaGetCount(boxa));
    l_int32 j = ll_check_index(_fun, L, 3, boxaGetCount(boxa));
    return ll_push_boolean(_fun, L, 0 == boxaSwapBoxes(boxa, i, j));
}

/**
 * \brief Translate the boxes in the Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (transx).
 * Arg #3 is expected to be a l_int32 (transy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
Translate(lua_State *L)
{
    LL_FUNC("Translate");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 transx = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 transy = ll_check_l_int32_default(_fun, L, 3, 0);
    Boxa *boxa = boxaTranslate(boxas, transx, transy);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Windowed median for Boxa* (%boxas) giving Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (halfwin).
 * Arg #3 is optional and, if given, expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
WindowedMedian(lua_State *L)
{
    LL_FUNC("WindowedMedian");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 halfwin = ll_check_l_int32_default(_fun, L, 2, 3);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    Boxa *boxa = boxaWindowedMedian(boxas, halfwin, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Write a Boxa* (%boxa) to a file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaWrite(filename, boxa));
}

/**
 * \brief Write a Boxa* (%boxa) to memory (%data).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (boxaWriteMem(&data, &size, boxa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Write a Boxa* (%boxa) to a stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaWriteStream(stream->f, boxa));
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_BOXA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_check_Boxa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Boxa>(_fun, L, arg, LL_BOXA);
}

/**
 * \brief Optionally expect a LL_BOXA at index %arg on the Lua stack.
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
 * \brief Push Boxa* user data to the Lua stack and set its meta table.
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
 * \brief Create and push a new Boxa*.
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
int
ll_new_Boxa(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Boxa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},               /* garbage collect */
        {"__new",                   Create},                /* new Boxa */
        {"__len",                   GetCount},              /* #boxa */
        {"__tostring",              toString},
        {"AddBox",                  AddBox},
        {"AdjustHeightToTarget",    AdjustHeightToTarget},
        {"AdjustSides",             AdjustSides},
        {"AdjustWidthToTarget",     AdjustWidthToTarget},
        {"AffineTransform",         AffineTransform},
        {"BinSort",                 BinSort},
        {"Clear",                   Clear},
        {"ClipToBox",               ClipToBox},
        {"CombineOverlaps",         CombineOverlaps},
        {"CombineOverlapsInPair",   CombineOverlapsInPair},
        {"CompareRegions",          CompareRegions},
        {"ConstrainSize",           ConstrainSize},
        {"ContainedInBox",          ContainedInBox},
        {"ContainedInBoxCount",     ContainedInBoxCount},
        {"ContainedInBoxa",         ContainedInBoxa},
        {"ConvertToPta",            ConvertToPta},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"Destroy",                 Destroy},
        {"EncapsulateAligned",      EncapsulateAligned},
        {"ExtendArray",             ExtendArray},
        {"ExtendArrayToSize",       ExtendArrayToSize},
        {"ExtractAsNuma",           ExtractAsNuma},
        {"ExtractAsPta",            ExtractAsPta},
        {"ExtractSortedPattern",    ExtractSortedPattern},
        {"FillSequence",            FillSequence},
        {"FindInvalidBoxes",        FindInvalidBoxes},
        {"FindNearestBoxes",        FindNearestBoxes},
        {"GetArea",                 GetArea},
        {"GetAverageSize",          GetAverageSize},
        {"GetBox",                  GetBox},
        {"GetBoxGeometry",          GetBoxGeometry},
        {"GetCoverage",             GetCoverage},
        {"GetExtent",               GetExtent},
        {"GetMedianVals",           GetMedianVals},
        {"GetNearestByDirection",   GetNearestByDirection},
        {"GetNearestToLine",        GetNearestToLine},
        {"GetNearestToPt",          GetNearestToPt},
        {"GetRankVals",             GetRankVals},
        {"GetSizes",                GetSizes},
        {"GetValidBox",             GetValidBox},
        {"GetValidCount",           GetValidCount},
        {"GetWhiteblocks",          GetWhiteblocks},
        {"HandleOverlaps",          HandleOverlaps},
        {"InsertBox",               InsertBox},
        {"IntersectsBox",           IntersectsBox},
        {"IntersectsBoxCount",      IntersectsBoxCount},
        {"IsFull",                  IsFull},
        {"Join",                    Join},
        {"LinearFit",               LinearFit},
        {"LocationRange",           LocationRange},
        {"MakeSizeIndicator",       MakeSizeIndicator},
        {"MakeWHRatioIndicator",    MakeWHRatioIndicator},
        {"MergeEvenOdd",            MergeEvenOdd},
        {"ModifyWithBoxa",          ModifyWithBoxa},
        {"PermutePseudorandom",     PermutePseudorandom},
        {"PermuteRandom",           PermuteRandom},
        {"PruneSortedOnOverlap",    PruneSortedOnOverlap},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"ReconcileEvenOddHeight",  ReconcileEvenOddHeight},
        {"ReconcilePairWidth",      ReconcilePairWidth},
        {"RemoveBox",               RemoveBox},
        {"RemoveBoxAndSave",        RemoveBoxAndSave},
        {"ReplaceBox",              ReplaceBox},
        {"Rotate",                  Rotate},
        {"RotateOrth",              RotateOrth},
        {"SaveValid",               SaveValid},
        {"Scale",                   Scale},
        {"SelectBySize",            SelectBySize},
        {"SelectByWHRatio",         SelectByWHRatio},
        {"SelectLargeULBox",        SelectLargeULBox},
        {"SelectRange",             SelectRange},
        {"SelectWithIndicator",     SelectWithIndicator},
        {"SetSide",                 SetSide},
        {"Similar",                 Similar},
        {"SizeRange",               SizeRange},
        {"SizeVariation",           SizeVariation},
        {"SmoothSequenceLS",        SmoothSequenceLS},
        {"SmoothSequenceMedian",    SmoothSequenceMedian},
        {"Sort",                    Sort},
        {"Sort2d",                  Sort2d},
        {"Sort2dByIndex",           Sort2dByIndex},
        {"SortByIndex",             SortByIndex},
        {"SplitEvenOdd",            SplitEvenOdd},
        {"SwapBoxes",               SwapBoxes},
        {"TakeBox",                 RemoveBoxAndSave},      /* alias for RemoveBoxAndSave */
        {"Translate",               Translate},
        {"WindowedMedian",          WindowedMedian},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_BOXA);
    return ll_register_class(L, LL_BOXA, methods, functions);
}
