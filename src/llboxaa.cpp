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
 * \file llboxaa.cpp
 * \class Boxaa
 *
 * An array of Boxa.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BOXAA

/** Define a function's name (_fun) with prefix Boxaa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Boxaa *baa = ll_take_udata<Boxaa>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "baa", reinterpret_cast<void *>(baa),
        "count", boxaaGetCount(baa));
    boxaaDestroy(&baa);
    return 0;
}

/**
 * \brief Get count for a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, boxaaGetCount(boxaa));
    return 1;
}

/**
 * \brief Printable string for a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!baa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(baa));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        for (l_int32 i = 0; i < boxaaGetCount(baa); i++) {
            Boxa *boxa = boxaaGetBoxa(baa, i, L_CLONE);
            snprintf(str, LL_STRBUFF,
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (l_int32 j = 0; j < boxaGetCount(boxa); j++) {
                l_int32 x, y, w, h;
                boxaGetBoxGeometry(boxa, j, &x, &y, &w, &h);
                snprintf(str, LL_STRBUFF,
                         "\n       %d = { x = %d, y = %d, w = %d, h = %d }",
                         j+1, x, y, w, h);
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            boxaDestroy(&boxa);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add a Box* (%box) to the Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (index).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (accessflag).
 *
 * Leptonica's Notes:
 *      (1) Adds to an existing boxa only.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddBox(lua_State *L)
{
    LL_FUNC("AddBox");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 accessflag = ll_check_access_storage(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == boxaaAddBox(baa, index, box, accessflag));
}

/**
 * \brief Add a Boxa* (%ba) to a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a Boxa* (%ba).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddBoxa(lua_State *L)
{
    LL_FUNC("AddBoxa");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    Boxa *ba = ll_check_Boxa(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == boxaaAddBoxa(baa, ba, flag));
}

/**
 * \brief Align a Boxa* from the Boxaa* (%baa) to a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (delta).
 *
 * Leptonica's Notes:
 *      (1) This is not greedy.  It finds the boxa whose vertical
 *          extent has the closest overlap with the input box.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
AlignBox(lua_State *L)
{
    LL_FUNC("AlignBox");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 delta = ll_check_l_int32(_fun, L, 3);
    l_int32 index = 0;
    if (boxaaAlignBox(baa, box, delta, &index))
        return ll_push_nil(L);
    ll_push_l_int32 (_fun, L, index);
    return 1;
}

/**
 * \brief Copy a Boxaa* (%baas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baas).
 * Arg #2 is an optional string defining the storage flags (copyflag).
 *
 * Leptonica's Notes:
 *      (1) L_COPY makes a copy of each boxa in baas.
 *          L_CLONE makes a clone of each boxa in baas.
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Boxaa *baas = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2);
    Boxaa *baa = boxaaCopy(baas, copyflag);
    return ll_push_Boxaa(_fun, L, baa);
}

/**
 * \brief Create a new Boxaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Boxaa *boxaa = boxaaCreate(n);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Display a Boxaa* (%baa) in a Pix* (%pix) optionally created from a Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (linewba).
 * Arg #4 is expected to be a l_int32 (linewb).
 * Arg #5 is expected to be a l_uint32 (colorba).
 * Arg #6 is expected to be a l_uint32 (colorb).
 * Arg #7 is expected to be a l_int32 (w).
 * Arg #8 is expected to be a l_int32 (h).
 *
 * Leptonica's Notes:
 *      (1) If %pixs exists, this renders the boxes over an 8 bpp version
 *          of it.  Otherwise, it renders the boxes over an empty image
 *          with a white background.
 *      (2) If %pixs exists, the dimensions of %pixd are the same,
 *          and input values of %w and %h are ignored.
 *          If %pixs is NULL, the dimensions of %pixd are determined by
 *            - %w and %h if both are > 0, or
 *            - the minimum size required using all boxes in %baa.
 *
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Display(lua_State *L)
{
    LL_FUNC("Display");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    Pix *pixs = ll_opt_Pix(_fun, L, 2);
    l_int32 linewba = ll_check_l_int32(_fun, L, 3);
    l_int32 linewb = ll_check_l_int32(_fun, L, 4);
    l_uint32 colorba = ll_check_l_uint32(_fun, L, 5);
    l_uint32 colorb = ll_check_l_uint32(_fun, L, 6);
    l_int32 w = ll_check_l_int32(_fun, L, 7);
    l_int32 h = ll_check_l_int32(_fun, L, 8);
    Pix *pix = boxaaDisplay(pixs, baa, linewba, linewb, colorba, colorb, w, h);
    return ll_push_Pix (_fun, L, pix);
}

/**
 * \brief Extend a Boxaa* (%baa) array.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ExtendArray(lua_State *L)
{
    LL_FUNC("ExtendArray");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == boxaaExtendArray(baa));
}

/**
 * \brief Extend a Boxaa* to a given size %n.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) If necessary, reallocs the boxa ptr array to %size.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ExtendArrayToSize(lua_State *L)
{
    LL_FUNC("ExtendArrayToSize");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaExtendArrayToSize(boxaa, size));
}

/**
 * \brief Extend a Boxaa* (%baa) and initialize with Boxa* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a index (maxindex).
 * Arg #3 is expected to be a Boxa* (ba).
 *
 * Leptonica's Notes:
 *      (1) This should be used on an existing boxaa that has been
 *          fully loaded with boxa.  It then extends the boxaa,
 *          loading all the additional ptrs with copies of boxa.
 *          Typically, boxa will be empty.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ExtendWithInit(lua_State *L)
{
    LL_FUNC("ExtendWithInit");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 maxindex = ll_check_index(_fun, L, 2);
    Boxa *ba = ll_check_Boxa(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == boxaaExtendWithInit(baa, maxindex, ba));
}

/**
 * \brief Aligned flatten the Boxaa* (%baa) to a Boxa* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (boxaa).
 * Arg #2 is expected to be a l_int32 (num).
 * Arg #3 is expected to be a string describing the copy flag (copyflag).
 * Arg #3 is an optional Box* (fillerbox).
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the baa to a boxa, taking the first %num
 *          boxes from each boxa.
 *      (2) In each boxa, if there are less than %num boxes, we preserve
 *          the alignment between the input baa and the output boxa
 *          by inserting one or more fillerbox(es) or, if %fillerbox == NULL,
 *          one or more invalid placeholder boxes.
 * </pre>
 * \param L Lua state.
 * \return 1 Boxa* on the Lua stack.
 */
static int
FlattenAligned(lua_State *L)
{
    LL_FUNC("FlattenAligned");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 num = ll_check_l_int32(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *fillerbox = ll_opt_Box(_fun, L, 4);
    Boxa *ba = boxaaFlattenAligned(baa, num, fillerbox, copyflag);
    return ll_push_Boxa(_fun, L, ba);
}

/**
 * \brief Flatten the Boxaa* (%baa) to a Boxa* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a string describing the copy flag (copyflag).
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the baa to a boxa, taking the boxes in
 *          order in the first boxa, then the second, etc.
 *      (2) If a boxa is empty, we generate an invalid, placeholder box
 *          of zero size.  This is useful when converting from a baa
 *          where each boxa has either 0 or 1 boxes, and it is necessary
 *          to maintain a 1:1 correspondence between the initial
 *          boxa array and the resulting box array.
 *      (3) If &naindex is defined, we generate a Numa that gives, for
 *          each box in the baa, the index of the boxa to which it belongs.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
FlattenToBoxa(lua_State *L)
{
    LL_FUNC("FlattenToBoxa");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Numa *naindex = nullptr;
    Boxa *ba = boxaaFlattenToBoxa(baa, &naindex, copyflag);
    return ll_push_Boxa(_fun, L, ba);
}

/**
 * \brief Get Box* (%box) from a Boxaa* (%baa) at index %iboxa and %ibox.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (iboxa).
 * Arg #3 is expected to be a l_int32 (ibox).
 * Arg #4 is an optional string defining the storage flags (copy, clone).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
GetBox(lua_State *L)
{
    LL_FUNC("GetBox");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 iboxa = ll_check_index(_fun, L, 2, boxaaGetCount(baa));
    l_int32 ibox = ll_check_index(_fun, L, 3, INT32_MAX);
    l_int32 flag = ll_check_access_storage(_fun, L, 4, L_COPY);
    Box *box = boxaaGetBox(baa, iboxa, ibox, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get count of boxes in a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetBoxCount(lua_State *L)
{
    LL_FUNC("GetBoxCount");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, boxaaGetBoxCount(baa));
    return 1;
}

/**
 * \brief Get Boxa* (%ba) from a Boxaa* (%baa) at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a index (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L Lua state.
 * \return 1 Boxa* (%ba) on the Lua stack.
 */
static int
GetBoxa(lua_State *L)
{
    LL_FUNC("GetBoxa");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(baa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Boxa *ba = boxaaGetBoxa(baa, idx, flag);
    return ll_push_Boxa(_fun, L, ba);
}

/**
 * \brief Get the extent of boxes in a Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 *
 * Leptonica's Notes:
 *      (1) The returned w and h are the minimum size image
 *          that would contain all boxes untranslated.
 *      (2) Each box in the returned boxa is the minimum box required to
 *          hold all the boxes in the respective boxa of baa.
 *      (3) If there are no valid boxes in a boxa, the box corresponding
 *          to its extent has all fields set to 0 (an invalid box).
 * </pre>
 * \param L Lua state.
 * \return 4 on the Lua stack (%w, %h, %box, %ba).
 */
static int
GetExtent(lua_State *L)
{
    LL_FUNC("GetExtent");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    Box *box = nullptr;
    Boxa *ba = nullptr;
    if (boxaaGetExtent(baa, &w, &h, &box, &ba))
        return ll_push_nil(L);
    ll_push_l_int32 (_fun, L, w);
    ll_push_l_int32 (_fun, L, h);
    ll_push_Box (_fun, L, box);
    ll_push_Boxa (_fun, L, ba);
    return 4;
}

/**
 * \brief Filling up initialization of a Boxaa* (%baa) with Boxa* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a Boxa* (ba).
 *
 * Leptonica's Notes:
 *      (1) This initializes a boxaa by filling up the entire boxa ptr array
 *          with copies of %boxa.  Any existing boxa are destroyed.
 *          After this operation, the number of boxa is equal to
 *          the number of allocated ptrs.
 *      (2) Note that we use boxaaReplaceBox() instead of boxaInsertBox().
 *          They both have the same effect when inserting into a NULL ptr
 *          in the boxa ptr array
 *      (3) Example usage.  This function is useful to prepare for a
 *          random insertion (or replacement) of boxa into a boxaa.
 *          To randomly insert boxa into a boxaa, up to some index "max":
 *             Boxaa *baa = boxaaCreate(max);
 *               // initialize the boxa
 *             Boxa *boxa = boxaCreate(...);
 *             ...  [optionally fix with boxes]
 *             boxaaInitFull(baa, boxa);
 *          A typical use is to initialize the array with empty boxa,
 *          and to replace only a subset that must be aligned with
 *          something else, such as a pixa.
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
InitFull(lua_State *L)
{
    LL_FUNC("InitFull");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    Boxa *ba = ll_check_Boxa(_fun, L, 2);
    l_ok result = boxaaInitFull(baa, ba);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief Insert the Boxa* (%ba) in a Boxaa* (%baa) at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 *
 * Leptonica's Notes:
 *      (1) This shifts boxa[i] --> boxa[i + 1] for all i >= index,
 *          and then inserts boxa as boxa[index].
 *      (2) To insert at the beginning of the array, set index = 0.
 *      (3) To append to the array, it's easier to use boxaaAddBoxa().
 *      (4) This should not be used repeatedly to insert into large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
InsertBoxa(lua_State *L)
{
    LL_FUNC("InsertBoxa");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(baa));
    Boxa *bas = ll_check_Boxa(_fun, L, 3);
    Boxa *ba = boxaCopy(bas, L_CLONE);
    return ll_push_boolean(_fun, L, 0 == boxaaInsertBoxa(baa, idx, ba));
}

/**
 * \brief Join Boxaa* (%baas) with Boxa* (%baad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (baad).
 * Arg #2 is expected to be another Box* (baas).
 * Arg #3 is an optional l_int32 (istart).
 * Arg #4 is an optional l_int32 (iend).
 *
 * Leptonica's Notes:
 *      (1) This appends a clone of each indicated boxa in baas to baad
 *      (2) istart < 0 is taken to mean 'read from the start' (istart = 0)
 *      (3) iend < 0 means 'read to the end'
 *      (4) if baas == NULL, this is a no-op.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Boxaa *baad = ll_check_Boxaa(_fun, L, 1);
    Boxaa *baas = ll_check_Boxaa(_fun, L, 2);
    l_int32 istart = ll_check_index(_fun, L, 3, 1);
    l_int32 iend = ll_check_index(_fun, L, 3, boxaaGetCount(baas));
    return ll_push_boolean(_fun, L, 0 == boxaaJoin(baad, baas, istart, iend));
}

/**
 * \brief Create a quad tree Boxaa* (%baa) of regions for width (%w) height (%h) for %nlevels levels.
 * <pre>
 * Arg #1 is expected to be a l_int32 (w).
 * Arg #2 is expected to be a l_int32 (h).
 * Arg #3 is expected to be a l_int32 (nlevels).
 *
 * Leptonica's Notes:
 *      (1) The returned boxaa has %nlevels of boxa, each containing
 *          the set of rectangles at that level.  The rectangle at
 *          level 0 is the entire region; at level 1 the region is
 *          divided into 4 rectangles, and at level n there are n^4
 *          rectangles.
 *      (2) At each level, the rectangles in the boxa are in "raster"
 *          order, with LR (fast scan) and TB (slow scan).
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* (%baa) on the Lua stack.
 */
static int
QuadtreeRegions(lua_State *L)
{
    LL_FUNC("QuadtreeRegions");
    l_int32 w = ll_check_l_int32(_fun, L, 1);
    l_int32 h = ll_check_l_int32(_fun, L, 2);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 3);
    Boxaa *baa = boxaaQuadtreeRegions(w, h, nlevels);
    return ll_push_Boxaa(_fun, L, baa);
}

/**
 * \brief Read a Boxaa* (%baa) from a file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* (%baa) on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Boxaa *baa = boxaaRead(filename);
    return ll_push_Boxaa(_fun, L, baa);
}

/**
 * \brief ReadFromFiles() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (dirname).
 * Arg #2 is expected to be a const char* (substr).
 * Arg #3 is expected to be a l_int32 (first).
 * Arg #4 is expected to be a l_int32 (nfiles).
 *
 * Leptonica's Notes:
 *      (1) The files must be serialized boxa files (e.g., *.ba).
 *          If some files cannot be read, warnings are issued.
 *      (2) Use %substr to filter filenames in the directory.  If
 *          %substr == NULL, this takes all files.
 *      (3) After filtering, use %first and %nfiles to select
 *          a contiguous set of files, that have been lexically
 *          sorted in increasing order.
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* (%baa) on the Lua stack.
 */
static int
ReadFromFiles(lua_State *L)
{
    LL_FUNC("ReadFromFiles");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = ll_opt_string(_fun, L, 2);
    l_int32 first = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 nfiles = ll_opt_l_int32(_fun, L, 4, -1);
    Boxaa *baa = boxaaReadFromFiles(dirname, substr, first, nfiles);
    return ll_push_Boxaa (_fun, L, baa);
}

/**
 * \brief Read a Boxaa* (%boxaa) from memory (%data).
 * <pre>
 * Arg #1 is expected to be a pstring (data).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    Boxaa *boxaa = boxaaReadMem(data, size);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Read a Boxaa* (%boxaa) from a stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Boxaa *boxaa = boxaaReadStream(stream->f);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Reomve the Boxa* from a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (%idx).
 *
 * Leptonica's Notes:
 *      (1) This removes boxa[index] and then shifts
 *          boxa[i] --> boxa[i - 1] for all i > index.
 *      (2) The removed boxaa is destroyed.
 *      (2) This should not be used repeatedly on large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
RemoveBoxa(lua_State *L)
{
    LL_FUNC("RemoveBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    return ll_push_boolean(_fun, L, 0 == boxaaRemoveBoxa(boxaa, idx));
}

/**
 * \brief Replace the Box* in a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 *
 * Leptonica's Notes:
 *      (1) Any existing boxa is destroyed, and the input one
 *          is inserted in its place.
 *      (2) If the index is invalid, return 1 (error)
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReplaceBoxa(lua_State *L)
{
    LL_FUNC("ReplaceBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    Boxa *boxa = ll_check_Boxa(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == boxaaReplaceBoxa(boxaa, idx, boxa));
}

/**
 * \brief Select a range of boxa (%first, %last) from Boxaa* (%baas)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baas).
 * Arg #2 is expected to be a l_int32 (first).
 * Arg #3 is expected to be a l_int32 (last).
 * Arg #4 is expected to be a l_int32 (copyflag).
 *
 * Leptonica's Notes:
 *      (1) The copyflag specifies what we do with each boxa from baas.
 *          Specifically, L_CLONE inserts a clone into baad of each
 *          selected boxa from baas.
 * </pre>
 * \param L Lua state.
 * \return 1 Boxaa* on the Lua stack.
 */
static int
SelectRange(lua_State *L)
{
    LL_FUNC("SelectRange");
    Boxaa *baas = ll_check_Boxaa(_fun, L, 1);
    l_int32 first = ll_check_index(_fun, L, 2, boxaaGetCount(baas));
    l_int32 last = ll_check_index(_fun, L, 3, boxaaGetCount(baas));
    l_int32 copyflag = ll_check_access_storage(_fun, L, 4);
    Boxaa *boxaa = boxaaSelectRange(baas, first, last, copyflag);
    return ll_push_Boxaa (_fun, L, boxaa);
}

/**
 * \brief Determin the size range of boxes in the Boxaa* (%baa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 *
 * </pre>
 * \param L Lua state.
 * \return 4 l_int32 on the Lua stack (%minw, %minh, %maxw, %maxh).
 */
static int
SizeRange(lua_State *L)
{
    LL_FUNC("SizeRange");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 minw = 0;
    l_int32 minh = 0;
    l_int32 maxw = 0;
    l_int32 maxh = 0;
    if (boxaaSizeRange(baa, &minw, &minh, &maxw, &maxh))
        return ll_push_nil(L);
    ll_push_l_int32 (_fun, L, minw);
    ll_push_l_int32 (_fun, L, minh);
    ll_push_l_int32 (_fun, L, maxw);
    ll_push_l_int32 (_fun, L, maxh);
    return 4;
}

/**
 * \brief Sort2dByIndex() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Numaa* (naa).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Sort2dByIndex(lua_State *L)
{
    LL_FUNC("Sort2dByIndex");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Numaa *naa = ll_check_Numaa(_fun, L, 2);
    Boxaa *boxaa = boxaSort2dByIndex(boxas, naa);
    return ll_push_Boxaa (_fun, L, boxaa);
}

/**
 * \brief Transpose() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baas).
 *
 * Leptonica's Notes:
 *      (1) If you think of a boxaa as a 2D array of boxes that is accessed
 *          row major, then each row is represented by one of the boxa.
 *          This function creates a new boxaa related to the input boxaa
 *          as a column major traversal of the input boxaa.
 *      (2) For example, if %baas has 2 boxa, each with 10 boxes, then
 *          %baad will have 10 boxa, each with 2 boxes.
 *      (3) Require for this transpose operation that each boxa in
 *          %baas has the same number of boxes.  This operation is useful
 *          when the i-th boxes in each boxa are meaningfully related.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Transpose(lua_State *L)
{
    LL_FUNC("Transpose");
    Boxaa *baas = ll_check_Boxaa(_fun, L, 1);
    Boxaa *baa = boxaaTranspose(baas);
    return ll_push_Boxaa (_fun, L, baa);
}

/**
 * \brief Write a Boxaa* (%boxaa) to a file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaWrite(filename, boxaa));
}

/**
 * \brief Write a Boxaa* (%boxaa) to memory (%data).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) Serializes a boxaa in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (boxaaWriteMem(&data, &size, boxaa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write a Boxaa* (%boxaa) to a stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaWriteStream(stream->f, boxaa));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Boxaa.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data.
 */
Boxaa *
ll_check_Boxaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Boxaa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Boxaa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data.
 */
Boxaa *
ll_opt_Boxaa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Boxaa(_fun, L, arg);
}

/**
 * \brief Push Boxaa* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param boxaa pointer to the BOXAA
 * \return 1 Boxaa* on the Lua stack.
 */
int
ll_push_Boxaa(const char *_fun, lua_State *L, Boxaa *boxaa)
{
    if (!boxaa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, boxaa);
}
/**
 * \brief Create and push a new Boxaa*.
 * \param L Lua state.
 * \return 1 Boxaa* on the Lua stack.
 */
int
ll_new_Boxaa(lua_State *L)
{
    FUNC("ll_new_Boxaa");
    Boxaa *boxaa = nullptr;

    if (ll_isudata(_fun, L, 1, LL_BOXAA)) {
        Boxaa *boxaas = ll_opt_Boxaa(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_BOXAA, reinterpret_cast<void *>(boxaas));
        boxaa = boxaaCopy(boxaas, L_COPY);
    }

    if (!boxaa && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
            luaL_Stream *stream = ll_check_stream(_fun, L, 2);
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
            boxaa = boxaaReadStream(stream->f);
    }

    if (!boxaa && ll_isinteger(_fun, L, 1)) {
        l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxaa = boxaaCreate(n);
    }

    if (!boxaa && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        boxaa = boxaaRead(filename);
    }

    if (!boxaa && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        boxaa = boxaaReadMem(data, size);
    }

    if (!boxaa) {
        l_int32 n = 1;
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxaa = boxaaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(boxaa));
    return ll_push_Boxaa(_fun, L, boxaa);
}
/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Boxaa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Boxaa},
        {"__len",                   GetCount},
        {"__tostring",              toString},
        {"AddBox",                  AddBox},
        {"AddBoxa",                 AddBoxa},
        {"AlignBox",                AlignBox},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"Destroy",                 Destroy},
        {"Display",                 Display},
        {"ExtendArray",             ExtendArray},
        {"ExtendArrayToSize",       ExtendArrayToSize},
        {"ExtendWithInit",          ExtendWithInit},
        {"FlattenAligned",          FlattenAligned},
        {"FlattenToBoxa",           FlattenToBoxa},
        {"GetBox",                  GetBox},
        {"GetBoxCount",             GetBoxCount},
        {"GetBoxa",                 GetBoxa},
        {"GetCount",                GetCount},
        {"GetExtent",               GetExtent},
        {"InitFull",                InitFull},
        {"InsertBoxa",              InsertBoxa},
        {"Join",                    Join},
        {"QuadtreeRegions",         QuadtreeRegions},
        {"Read",                    Read},
        {"ReadFromFiles",           ReadFromFiles},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveBoxa",              RemoveBoxa},
        {"ReplaceBoxa",             ReplaceBoxa},
        {"SelectRange",             SelectRange},
        {"SizeRange",               SizeRange},
        {"Sort2dByIndex",           Sort2dByIndex},
        {"Transpose",               Transpose},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Boxaa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
