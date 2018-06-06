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
 * \file llboxa.cpp
 * \class Boxa
 *
 * An array of Box.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BOXA

/** Define a function's name (_fun) with prefix TNAME */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa*.
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the boxa.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Boxa **pboxa = ll_check_udata<Boxa>(_fun, L, 1, TNAME);
    Boxa *boxa = *pboxa;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "pboxa", reinterpret_cast<void *>(pboxa),
        "boxa", reinterpret_cast<void *>(boxa),
        "count", boxaGetCount(boxa));
    boxaDestroy(&boxa);
    *pboxa = nullptr;
    return 0;
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
    ll_push_l_int32(_fun, L, boxaGetCount(boxa));
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
                 TNAME ": %p",
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
    l_int32 target = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 thresh = ll_opt_l_int32(_fun, L, 4, 0);
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
 *
 * Leptonica's Notes:
 *      (1) New box dimensions are cropped at left and top to x >= 0 and y >= 0.
 *      (2) If the width or height of a box goes to 0, we generate a box with
 *          w == 1 and h == 1, as a placeholder.
 *      (3) See boxAdjustSides().
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
AdjustSides(lua_State *L)
{
    LL_FUNC("AdjustSides");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 delleft = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 delright = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 deltop = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 delbot = ll_opt_l_int32(_fun, L, 5, 0);
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
 *
 * Leptonica's Notes:
 *      (1) Conditionally adjusts the width of each box, by moving
 *          the indicated edges (left and/or right) if the width differs
 *          by %thresh or more from %target.
 *      (2) Use boxad == NULL for a new boxa, and boxad == boxas for in-place.
 *          Use one of these:
 *               boxad = boxaAdjustWidthToTarget(NULL, boxas, ...);   // new
 *               boxaAdjustWidthToTarget(boxas, boxas, ...);  // in-place
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
    l_int32 target = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 thresh = ll_opt_l_int32(_fun, L, 4, 0);
    Boxa *boxad = boxaAdjustWidthToTarget(nullptr, boxas, sides, target, thresh);
    ll_push_Boxa(_fun, L, boxad);
    return 1;
}

/**
 * \brief Affine transformation of the boxes in the Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a matrix of 3x3 l_float32 (mat).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack (%boxa)
 */
static int
AffineTransform(lua_State *L)
{
    LL_FUNC("AffineTransform");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_float32 *mat = ll_unpack_Matrix(_fun, L, 2, 3, 3);
    Boxa *boxa = boxaAffineTransform(boxas, mat);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Sort a (large number of) Boxa* (%boxas) by given type (%type) and order (%order).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string defining the sort type (type).
 * Arg #3 is expected to be a string defining the sort order (order).
 *
 * Leptonica's Notes:
 *      (1) For a large number of boxes (say, greater than 1000), this
 *          O(n) binsort is much faster than the O(nlogn) shellsort.
 *          For 5000 components, this is over 20x faster than boxaSort().
 *      (2) Consequently, boxaSort() calls this function if it will
 *          likely go much faster.
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
 *
 * Leptonica's Notes:
 *      (1) This destroys all boxes in the boxa, setting the ptrs
 *          to null.  The number of allocated boxes, n, is set to 0.
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
 *
 * Leptonica's Notes:
 *      (1) All boxes in boxa not intersecting with box are removed, and
 *          the remaining boxes are clipped to box.
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
 * Arg #2 is an optional Pixa* (pixadb).
 *
 * Leptonica's Notes:
 *      (1) If there are no overlapping boxes, it simply returns a copy
 *          of %boxas.
 *      (2) Input an empty %pixadb, using pixaCreate(0), for debug output.
 *          The output gives 2 visualizations of the boxes per iteration;
 *          boxes in red before, and added boxes in green after. Note that
 *          all pixels in the red boxes are contained in the green ones.
 *      (3) The alternative method of painting each rectangle and finding
 *          the 4-connected components gives a different result in
 *          general, because two non-overlapping (but touching)
 *          rectangles, when rendered, are 4-connected and will be joined.
 *      (4) A bad case computationally is to have n boxes, none of which
 *          overlap.  Then you have one iteration with O(n^2) compares.
 *          This is still faster than painting each rectangle and finding
 *          the bounding boxes of the connected components, even for
 *          thousands of rectangles.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
CombineOverlaps(lua_State *L)
{
    LL_FUNC("CombineOverlaps");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    Pixa *pixadb = ll_opt_Pixa(_fun, L, 2);
    Boxa *boxad = boxaCombineOverlaps(boxas, pixadb);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Combine overlaps in pairs of boxes of two Boxa* (%boxa1, %boxa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa1).
 * Arg #2 is expected to be a another Boxa* (boxa2).
 * Arg #3 is an optional Pixa* (pixadb).
 *
 * Leptonica's Notes:
 *      (1) One of three things happens to each box in %boxa1 and %boxa2:
 *           * it gets absorbed into a larger box that it overlaps with
 *           * it absorbs a smaller (by area) box that it overlaps with
 *             and gets larger, using the bounding region of the 2 boxes
 *           * it is unchanged (including absorbing smaller boxes that
 *             are contained within it).
 *      (2) If all the boxes from one of the input boxa are absorbed, this
 *          returns an empty boxa.
 *      (3) Input an empty %pixadb, using pixaCreate(0), for debug output
 *      (4) This is useful if different operations are to be carried out
 *          on possibly overlapping rectangular regions, and it is desired
 *          to have only one operation on any rectangular region.
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
    Pixa *pixadb = ll_opt_Pixa(_fun, L, 3);
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
 * Arg #3 is an optional Pix* (pixdb).
 *
 * Leptonica's Notes:
 *      (1) This takes 2 boxa, removes all boxes smaller than a given area,
 *          and compares the remaining boxes between the boxa.
 *      (2) The area threshold is introduced to help remove noise from
 *          small components.  Any box with a smaller value of w * h
 *          will be removed from consideration.
 *      (3) The xor difference is the most stringent test, requiring alignment
 *          of the corresponding boxes.  It is also more computationally
 *          intensive and is optionally returned.  Alignment is to the
 *          UL corner of each region containing all boxes, as given by
 *          boxaGetExtent().
 *      (4) Both fractional differences are with respect to the total
 *          area in the two boxa.  They range from 0.0 to 1.0.
 *          A perfect match has value 0.0.  If both boxa are empty,
 *          we return 0.0; if one is empty we return 1.0.
 *      (5) An example input might be the rectangular regions of a
 *          segmentation mask for text or images from two pages.
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
    Pix *pixdb = ll_opt_Pix(_fun, L, 4);
    l_int32 nsame = 0;
    l_float32 diffarea = 0.0f;
    l_float32 diffxor = 0.0f;
    if (boxaCompareRegions(boxa1, boxa2, areathresh, &nsame, &diffarea, &diffxor, &pixdb))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nsame);
    ll_push_l_float32(_fun, L, diffarea);
    ll_push_l_float32(_fun, L, diffxor);
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
 *
 * Leptonica's Notes:
 *      (1) Forces either width or height (or both) of every box in
 *          the boxa to a specified size, by moving the indicated sides.
 *      (2) Not all input boxes need to be valid.  Median values will be
 *          used with invalid boxes.
 *      (3) Typical input might be the output of boxaLinearFit(),
 *          where each side has been fit.
 *      (4) Unlike boxaAdjustWidthToTarget() and boxaAdjustHeightToTarget(),
 *          this is not dependent on a difference threshold to change the size.
 *      (5) On error, a message is issued and a copy of the input boxa
 *          is returned.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ConstrainSize(lua_State *L)
{
    LL_FUNC("ConstrainSize");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 width = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 widthflag = ll_check_adjust_sides(_fun, L, 3, L_ADJUST_LEFT_AND_RIGHT);
    l_int32 height = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 heightflag = ll_check_adjust_sides(_fun, L, 5, L_ADJUST_TOP_AND_BOT);
    Boxa *boxa = boxaConstrainSize(boxas, width, widthflag, height, heightflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) contained within Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) All boxes in boxa that are entirely outside box are removed.
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
    ll_push_l_int32(_fun, L, count);
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
 *
 * Leptonica's Notes:
 *      (1) If ncorners == 2, we select the UL and LR corners.
 *          Otherwise we save all 4 corners in this order: UL, UR, LL, LR.
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
 *
 * Leptonica's Notes:
 *      (1) See pix.h for description of the copyflag.
 *      (2) The copy-clone makes a new boxa that holds clones of each box.
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
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Boxa *boxa = boxaCreate(n);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Encapsulate Boxa* (%boxa) aligned into a Boxaa* (%boxaa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (num).
 * Arg #3 is an optional string defining the storage flags (copyflag).
 *
 * Leptonica's Notes:
 *      (1) This puts %num boxes from the input %boxa into each of a
 *          set of boxa within an output baa.
 *      (2) This assumes that the boxes in %boxa are in sets of %num each.
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
 *
 * Leptonica's Notes:
 *      (1) Reallocs with doubled size of ptr array.
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
 *
 * Leptonica's Notes:
 *      (1) If necessary, reallocs new boxa ptr array to %size.
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
 *
 * Leptonica's Notes:
 *      (1) If you are counting or sorting values, such as determining
 *          rank order, you must remove invalid boxes.
 *      (2) If you are parametrizing the values, or doing an evaluation
 *          where the position in the boxa sequence is important, you
 *          must replace the invalid boxes with valid ones before
 *          doing the extraction. This is easily done with boxaFillSequence().
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
 * Arg #2 is an optional boolean (keepinvalid).
 *
 * Leptonica's Notes:
 *      (1) For most applications, such as counting, sorting, fitting
 *          to some parametrized form, plotting or filtering in general,
 *          you should remove the invalid boxes.  Each pta saves the
 *          box index in the x array, so replacing invalid boxes by
 *          filling with boxaFillSequence(), which is required for
 *          boxaExtractAsNuma(), is not necessary.
 *      (2) If invalid boxes are retained, each one will result in
 *          entries (typically 0) in all selected output pta.
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 Pta* on the Lua stack (%ptal, %ptar, %ptat, %ptab, %ptaw, %ptah)
 */
static int
ExtractAsPta(lua_State *L)
{
    LL_FUNC("ExtractAsPta");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 keepinvalid = ll_opt_boolean(_fun, L, 2, FALSE);
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
 *
 * Leptonica's Notes:
 *      (1) The input is expected to come from pixGetWordBoxesInTextlines().
 *      (2) Each numa in the output consists of an average y coordinate
 *          of the first box in the textline, followed by pairs of
 *          x coordinates representing the left and right edges of each
 *          of the boxes in the textline.
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
 * Arg #3 is an optional boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) This simple function replaces invalid boxes with a copy of
 *          the nearest valid box, selected from either the entire
 *          sequence (L_USE_ALL_BOXES) or from the boxes with the
 *          same parity (L_USE_SAME_PARITY_BOXES).  It returns a new boxa.
 *      (2) This is useful if you expect boxes in the sequence to
 *          vary slowly with index.
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
    l_int32 debug = ll_opt_boolean(_fun, L, 3, FALSE);
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
 * Leptonica's Notes:
 *      (1) See boxaGetNearestByDirection() for usage of %dist_select
 *          and %range.
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
 *
 * Leptonica's Notes:
 *      (1) Measures the total area of the boxes, without regard to overlaps.
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
    ll_push_l_int32(_fun, L, area);
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
    ll_push_l_float32(_fun, L, w);
    ll_push_l_float32(_fun, L, h);
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
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 4;
}

/**
 * \brief Get the extent of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (wc)
 * Arg #3 is expected to be a l_int32 (hc)
 * Arg #4 is an optional boolean (exactflag)
 *
 * Leptonica's Notes:
 *      (1) The boxes in boxa are clipped to the input rectangle.
 *      (2) * When %exactflag == 1, we generate a 1 bpp pix of size
 *            wc x hc, paint all the boxes black, and count the fg pixels.
 *            This can take 1 msec on a large page with many boxes.
 *          * When %exactflag == 0, we clip each box to the wc x hc region
 *            and sum the resulting areas.  This is faster.
 *          * The results are the same when none of the boxes overlap
 *            within the wc x hc region.
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
    l_int32 exactflag = ll_opt_boolean(_fun, L, 3, FALSE);
    l_float32 fract = 0.0f;
    if (boxaGetCoverage(boxas, wc, hc, exactflag, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    return 1;
}

/**
 * \brief Get the extent of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 *
 * Leptonica's Notes:
 *      (1) The returned w and h are the minimum size image
 *          that would contain all boxes untranslated.
 *      (2) If there are no valid boxes, returned w and h are 0 and
 *          all parameters in the returned box are 0.  This
 *          is not an error, because an empty boxa is valid and
 *          boxaGetExtent() is required for serialization.
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
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    ll_push_Box(_fun, L, box);
    return 3;
}

/**
 * \brief Get median values for Boxa* (%boxa) as four integers (%x,%y,%w,%h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 *
 * Leptonica's Notes:
 *      (1) See boxaGetRankVals()
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
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
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
 *
 * Leptonica's Notes:
 *      (1) For efficiency, use a LR/TD sorted %boxa, which can be
 *          made by flattening a 2D sorted boxaa.  In that case,
 *          %range can be some positive integer like 50.
 *      (2) If boxes overlap, the distance will be < 0.  Use %dist_select
 *          to determine if these should count or not.  If L_ALL, then
 *          one box will match as the nearest to another in 2 or more
 *          directions.
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
    ll_push_l_int32(_fun, L, index);
    ll_push_l_int32(_fun, L, dist);
    return 2;
}

/**
 * \brief Get Box* (box) of Boxa* (%boxa) which is nearest to line (x,y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 *
 * Leptonica's Notes:
 *      (1) For a horizontal line at some value y, get the minimum of the
 *          distance |yc - y| from the box centroid yc value to y;
 *          likewise minimize |xc - x| for a vertical line at x.
 *      (2) Input y < 0, x >= 0 to indicate a vertical line at x, and
 *          x < 0, y >= 0 for a horizontal line at y.
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
 *
 * Leptonica's Notes:
 *      (1) Uses euclidean distance between centroid and point.
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
 *
 * Leptonica's Notes:
 *      (1) This function does not assume that all boxes in the boxa are valid
 *      (2) The four box parameters are sorted independently.
 *          For rank order, the width and height are sorted in increasing
 *          order.  But what does it mean to sort x and y in "rank order"?
 *          If the boxes are of comparable size and somewhat
 *          aligned (e.g., from multiple images), it makes some sense
 *          to give a "rank order" for x and y by sorting them in
 *          decreasing order.  But in general, the interpretation of a rank
 *          order on x and y is highly application dependent.  In summary:
 *             ~ x and y are sorted in decreasing order
 *             ~ w and h are sorted in increasing order
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
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
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
 *
 * Leptonica's Notes:
 *      (1) This returns NULL for an invalid box in a boxa.
 *          For a box to be valid, both the width and height must be > 0.
 *      (2) We allow invalid boxes, with w = 0 or h = 0, as placeholders
 *          in boxa for which the index of the box in the boxa is important.
 *          This is an atypical situation; usually you want to put only
 *          valid boxes in a boxa.
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
    ll_push_l_int32(_fun, L, boxaGetValidCount(boxa));
    return 1;
}

/**
 * \brief Get white blocks for boxes from a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is an optional Box* (box)
 * Arg #3 is expected to be a string describing the sort by type (sortflag).
 * Arg #4 is expected to be a l_int32 (maxboxes)
 * Arg #5 is expected to be a l_float32 (maxoverlap)
 * Arg #6 is expected to be a l_int32 (maxperim)
 * Arg #7 is expected to be a l_float32 (fract)
 * Arg #8 is expected to be a l_int32 (maxpops)
 *
 * Leptonica's Notes:
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
 *                   // Faster pivot choosing
 *               boxaPermuteRandom(boxa, boxa);
 *
 *                   // Remove anything either large width or height
 *               boxat = boxaSelectBySize(boxa, maxwidth, maxheight,
 *                                        L_SELECT_IF_BOTH, L_SELECT_IF_LT,
 *                                        NULL);
 *
 *               boxad = boxaGetWhiteblocks(boxat, box, type, maxboxes,
 *                                          maxoverlap, maxperim, fract,
 *                                          maxpops);
 *
 *          The result will be rectangular regions of "white space" that
 *          extend into (and often through) the excluded components.
 *     (11) As a simple example, suppose you wish to find the columns on a page.
 *          First exclude large c.c. that may block the columns, and then call:
 *
 *               boxad = boxaGetWhiteblocks(boxa, box, L_SORT_BY_HEIGHT,
 *                                          20, 0.15, 200, 0.2, 2000);
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
    l_int32 maxboxes = ll_opt_l_int32(_fun, L, 4, 100);
    l_float32 maxoverlap = ll_opt_l_float32(_fun, L, 5, 1.0f);
    l_int32 maxperim = ll_opt_l_int32(_fun, L, 6, 40);
    l_float32 fract = ll_opt_l_float32(_fun, L, 7, 0.0f);
    l_int32 maxpops = ll_opt_l_int32(_fun, L, 8, 1000);
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
 * Arg #6 is an optional Numa* (namap).
 *
 * Leptonica's Notes:
 *      (1) For all n(n-1)/2 box pairings, if two boxes overlap, either:
 *          (a) op == L_COMBINE: get the bounding region for the two,
 *              replace the larger with the bounding region, and remove
 *              the smaller of the two, or
 *          (b) op == L_REMOVE_SMALL: just remove the smaller.
 *      (2) If boxas is 2D sorted, range can be small, but if it is
 *          not spatially sorted, range should be large to allow all
 *          pairwise comparisons to be made.
 *      (3) The %min_overlap parameter allows ignoring small overlaps.
 *          If %min_overlap == 1.0, only boxes fully contained in larger
 *          boxes can be considered for removal; if %min_overlap == 0.0,
 *          this constraint is ignored.
 *      (4) The %max_ratio parameter allows ignoring overlaps between
 *          boxes that are not too different in size.  If %max_ratio == 0.0,
 *          no boxes can be removed; if %max_ratio == 1.0, this constraint
 *          is ignored.
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
    Numa *namap = ll_opt_Numa(_fun, L, 6);
    Boxa *boxad = boxaHandleOverlaps(boxas, op, range, min_overlap, max_ratio, &namap);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Insert the Box* (%box) in a Boxa* (%boxa) at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) This shifts box[i] --> box[i + 1] for all i >= index,
 *          and then inserts box as box[index].
 *      (2) To insert at the beginning of the array, set index = 0.
 *      (3) To append to the array, it's easier to use boxaAddBox().
 *      (4) This should not be used repeatedly to insert into large arrays,
 *          because the function is O(n).
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
    return ll_push_boolean(_fun, L, 0 == boxaInsertBox(boxa, idx, box));
}

/**
 * \brief Return a Boxa* (%boxad) of boxes from Boxa* (%boxas) which intersect within Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) All boxes in boxa that intersect with box (i.e., are completely
 *          or partially contained in box) are retained.
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
    ll_push_l_int32(_fun, L, count);
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
 * Arg #3 is an optional l_int32 (istart).
 * Arg #4 is an optional l_int32 (iend).
 *
 * Leptonica's Notes:
 *      (1) This appends a clone of each indicated box in boxas to boxad
 *      (2) istart < 0 is taken to mean 'read from the start' (istart = 0)
 *      (3) iend < 0 means 'read to the end'
 *      (4) if boxas == NULL or has no boxes, this is a no-op.
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
 * Arg #3 is an optional boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) This finds a set of boxes (boxad) where each edge of each box is
 *          a linear least square fit (LSF) to the edges of the
 *          input set of boxes (boxas).  Before fitting, outliers in
 *          the boxes in boxas are removed (see below).
 *      (2) This is useful when each of the box edges in boxas are expected
 *          to vary linearly with box index in the set.  These could
 *          be, for example, noisy measurements of similar regions
 *          on successive scanned pages.
 *      (3) Method: there are 2 steps:
 *          (a) Find and remove outliers, separately based on the deviation
 *              from the median of the width and height of the box.
 *              Use %factor to specify tolerance to outliers; use a very
 *              large value of %factor to avoid rejecting any box sides
 *              in the linear LSF.
 *          (b) On the remaining boxes, do a linear LSF independently
 *              for each of the four sides.
 *      (4) Invalid input boxes are not used in computation of the LSF.
 *      (5) The returned boxad can then be used in boxaModifyWithBoxa()
 *          to selectively change the boxes in boxas.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
LinearFit(lua_State *L)
{
    LL_FUNC("LinearFit");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 factor = ll_opt_l_int32(_fun, L, 2, 3);
    l_int32 debug = ll_opt_boolean(_fun, L, 3, FALSE);
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
    ll_push_l_int32(_fun, L, minx);
    ll_push_l_int32(_fun, L, miny);
    ll_push_l_int32(_fun, L, maxx);
    ll_push_l_int32(_fun, L, maxy);
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
 *
 * Leptonica's Notes:
 *      (1) The args specify constraints on the size of the
 *          components that are kept.
 *      (2) If the selection type is L_SELECT_WIDTH, the input
 *          height is ignored, and v.v.
 *      (3) To keep small components, use relation = L_SELECT_IF_LT or
 *          L_SELECT_IF_LTE.
 *          To keep large components, use relation = L_SELECT_IF_GT or
 *          L_SELECT_IF_GTE.
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
 *
 * Leptonica's Notes:
 *      (1) To keep narrow components, use relation = L_SELECT_IF_LT or
 *          L_SELECT_IF_LTE.
 *          To keep wide components, use relation = L_SELECT_IF_GT or
 *          L_SELECT_IF_GTE.
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
 *
 * Leptonica's Notes:
 *      (1) This is essentially the inverse of boxaSplitEvenOdd().
 *          Typically, boxae and boxao were generated by boxaSplitEvenOdd(),
 *          and the value of %fillflag needs to be the same in both calls.
 *      (2) If %fillflag == 1, both boxae and boxao are of the same size;
 *          otherwise boxae may have one more box than boxao.
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
    l_int32 fillflag = ll_opt_l_int32(_fun, L, 3, 0);
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
 *
 * Leptonica's Notes:
 *      (1) This takes two input boxa (boxas, boxam) and constructs boxad,
 *          where each box in boxad is generated from the corresponding
 *          boxes in boxas and boxam.  The rule for constructing each
 *          output box depends on %subflag and %maxdiff.  Let boxs be
 *          a box from %boxas and boxm be a box from %boxam.
 *          * If %subflag == L_USE_MINSIZE: the output box is the intersection
 *            of the two input boxes.
 *          * If %subflag == L_USE_MAXSIZE: the output box is the union of the
 *            two input boxes; i.e., the minimum bounding rectangle for the
 *            two input boxes.
 *          * If %subflag == L_SUB_ON_LOC_DIFF: each side of the output box
 *            is found separately from the corresponding side of boxs and boxm.
 *            Use the boxm side, expanded by %extrapixels, if greater than
 *            %maxdiff pixels from the boxs side.
 *          * If %subflag == L_SUB_ON_SIZE_DIFF: the sides of the output box
 *            are determined in pairs from the width and height of boxs
 *            and boxm.  If the boxm width differs by more than %maxdiff
 *            pixels from boxs, use the boxm left and right sides,
 *            expanded by %extrapixels.  Ditto for the height difference.
 *          For the last two flags, each side of the output box is found
 *          separately from the corresponding side of boxs and boxm,
 *          according to these rules, where "smaller"("bigger") mean in a
 *          direction that decreases(increases) the size of the output box:
 *          * If %subflag == L_USE_CAPPED_MIN: use the Min of boxm
 *            with the Max of (boxs, boxm +- %maxdiff), where the sign
 *            is adjusted to make the box smaller (e.g., use "+" on left side).
 *          * If %subflag == L_USE_CAPPED_MAX: use the Max of boxm
 *            with the Min of (boxs, boxm +- %maxdiff), where the sign
 *            is adjusted to make the box bigger (e.g., use "-" on left side).
 *          Use of the last 2 flags is further explained in (3) and (4).
 *      (2) boxas and boxam must be the same size.  If boxam == NULL,
 *          this returns a copy of boxas with a warning.
 *      (3) If %subflag == L_SUB_ON_LOC_DIFF, use boxm for each side
 *          where the corresponding sides differ by more than %maxdiff.
 *          Two extreme cases:
 *          (a) set %maxdiff == 0 to use only values from boxam in boxad.
 *          (b) set %maxdiff == 10000 to ignore all values from boxam;
 *              then boxad will be the same as boxas.
 *      (4) If %subflag == L_USE_CAPPED_MAX: use boxm if boxs is smaller;
 *          use boxs if boxs is bigger than boxm by an amount up to %maxdiff;
 *          and use boxm +- %maxdiff (the 'capped' value) if boxs is
 *          bigger than boxm by an amount larger than %maxdiff.
 *          Similarly, with interchange of Min/Max and sign of %maxdiff,
 *          for %subflag == L_USE_CAPPED_MIN.
 *      (5) If either of corresponding boxes in boxas and boxam is invalid,
 *          an invalid box is copied to the result.
 *      (6) Typical input for boxam may be the output of boxaLinearFit().
 *          where outliers have been removed and each side is LS fit to a line.
 *      (7) Unlike boxaAdjustWidthToTarget() and boxaAdjustHeightToTarget(),
 *          this uses two boxes and does not specify target dimensions.
 *          Additional constraints on the size of each box can be enforced
 *          by following this operation with boxaConstrainSize(), taking
 *          boxad as input.
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
    l_int32 maxdiff = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 extrapixels = ll_opt_l_int32(_fun, L, 5, 0);
    Boxa *boxa = boxaModifyWithBoxa(boxas, boxam, subflag, maxdiff, extrapixels);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Permute boxes in Boxa* (%boxas) by a pseudo random algorithm.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 *
 * Leptonica's Notes:
 *      (1) This does a pseudorandom in-place permutation of the boxes.
 *      (2) The result is guaranteed not to have any boxes in their
 *          original position, but it is not very random.  If you
 *          need randomness, use boxaPermuteRandom().
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
 *
 * Leptonica's Notes:
 *      (1) If boxad is null, make a copy of boxas and permute the copy.
 *          Otherwise, boxad must be equal to boxas, and the operation
 *          is done in-place.
 *      (2) If boxas is empty, return an empty boxad.
 *      (3) This does a random in-place permutation of the boxes,
 *          by swapping each box in turn with a random box.  The
 *          result is almost guaranteed not to have any boxes in their
 *          original position.
 *      (4) MSVC rand() has MAX_RAND = 2^15 - 1, so it will not do
 *          a proper permutation is the number of boxes exceeds this.
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
 *
 * Leptonica's Notes:
 *      (1) This selectively removes smaller boxes when they are overlapped
 *          by any larger box by more than the input 'maxoverlap' fraction.
 *      (2) To avoid all pruning, use maxoverlap = 1.0.  To select only
 *          boxes that have no overlap with each other (maximal pruning),
 *          set maxoverlap = 0.0.
 *      (3) If there are no boxes in boxas, returns an empty boxa.
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
 *
 * Leptonica's Notes:
 *      (1) The basic idea is to reconcile differences in box height
 *          in the even and odd boxes, by moving the top and/or bottom
 *          edges in the even and odd boxes.  Choose the edge or edges
 *          to be moved, whether to adjust the boxes with the min
 *          or the max of the medians, and the threshold on the median
 *          difference between even and odd box heights for the operations
 *          to take place.  The same threshold is also used to
 *          determine if each individual box edge is to be adjusted.
 *      (2) Boxes are conditionally reset with either the same top (y)
 *          value or the same bottom value, or both.  The value is
 *          determined by the greater or lesser of the medians of the
 *          even and odd boxes, with the choice depending on the value
 *          of %op, which selects for either min or max median height.
 *          If the median difference between even and odd boxes is
 *          greater than %dely, then any individual box edge that differs
 *          from the selected median by more than %dely is set to
 *          the selected median times a factor typically near 1.0.
 *      (3) Note that if selecting for minimum height, you will choose
 *          the largest y-value for the top and the smallest y-value for
 *          the bottom of the box.
 *      (4) Typical input might be the output of boxaSmoothSequence(),
 *          where even and odd boxa have been independently regulated.
 *      (5) Require at least 3 valid even boxes and 3 valid odd boxes.
 *          Median values will be used for invalid boxes.
 *      (6) If the median height is not representative of the boxes
 *          in %boxas, this can make things much worse.  In that case,
 *          ignore the value of %op, and force pairwise equality of the
 *          heights, with pairwise maximal vertical extension.
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
    l_float32 factor = ll_opt_l_float32(_fun, L, 5, 1.0f);
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
 *
 * Leptonica's Notes:
 *      (1) This reconciles differences in the width of adjacent boxes,
 *          by moving one side of one of the boxes in each pair.
 *          If the widths in the pair differ by more than some
 *          threshold, move either the left side for even boxes or
 *          the right side for odd boxes, depending on if we're choosing
 *          the min or max.  If choosing min, the width of the max is
 *          set to factor * (width of min).  If choosing max, the width
 *          of the min is set to factor * (width of max).
 *      (2) If %na exists, it is an indicator array corresponding to the
 *          boxes in %boxas.  If %na != NULL, only boxes with an
 *          indicator value of 1 are allowed to adjust; otherwise,
 *          all boxes can adjust.
 *      (3) Typical input might be the output of boxaSmoothSequence(),
 *          where even and odd boxa have been independently regulated.
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
    l_float32 factor = ll_opt_l_float32(_fun, L, 4, 1.0f);
    Numa *na = ll_check_Numa(_fun, L, 5);
    Boxa *boxa = boxaReconcilePairWidth(boxas, delw, op, factor, na);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Reomve the Box* from a Boxa* (%boxa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This removes box[index] and then shifts
 *          box[i] --> box[i - 1] for all i > index.
 *      (2) It should not be used repeatedly to remove boxes from
 *          large arrays, because the function is O(n).
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
 *
 * Leptonica's Notes:
 *      (1) This removes box[index] and then shifts
 *          box[i] --> box[i - 1] for all i > index.
 *      (2) It should not be used repeatedly to remove boxes from
 *          large arrays, because the function is O(n).
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
 *
 * Leptonica's Notes:
 *      (1) In-place replacement of one box.
 *      (2) The previous box at that location, if any, is destroyed.
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
    return ll_push_boolean(_fun, L, 0 == boxaReplaceBox(boxa, idx, box));
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
    l_float32 xc = ll_opt_l_float32(_fun, L, 2, 1.0f);
    l_float32 yc = ll_opt_l_float32(_fun, L, 3, 1.0f);
    l_float32 angle = ll_opt_l_float32(_fun, L, 4, 1.0f);
    Boxa *boxa = boxaRotate(boxas, xc, yc, angle);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Rotate a Boxa* (%boxas) orthogonally.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 *
 * Leptonica's Notes:
 *      (1) See boxRotateOrth() for details.
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
 *
 * Leptonica's Notes:
 *      (1) This makes a copy/clone of each valid box.
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
    l_float32 scalex = ll_opt_l_float32(_fun, L, 2, 1.0f);
    l_float32 scaley = ll_opt_l_float32(_fun, L, 3, 1.0f);
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
 *
 * Leptonica's Notes:
 *      (1) The args specify constraints on the size of the
 *          components that are kept.
 *      (2) Uses box copies in the new boxa.
 *      (3) If the selection type is L_SELECT_WIDTH, the input
 *          height is ignored, and v.v.
 *      (4) To keep small components, use relation = L_SELECT_IF_LT or
 *          L_SELECT_IF_LTE.
 *          To keep large components, use relation = L_SELECT_IF_GT or
 *          L_SELECT_IF_GTE.
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
    ll_push_l_int32(_fun, L, changed);
    return 2;
}

/**
 * \brief Select boxes from Boxa* (%boxa) by width/height ratio.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (ratio).
 * Arg #3 is expected to be a string describing the relation (relation).
 *
 * Leptonica's Notes:
 *      (1) Uses box copies in the new boxa.
 *      (2) To keep narrow components, use relation = L_SELECT_IF_LT or
 *          L_SELECT_IF_LTE.
 *          To keep wide components, use relation = L_SELECT_IF_GT or
 *          L_SELECT_IF_GTE.
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
    ll_push_l_int32(_fun, L, changed);
    return 2;
}

/**
 * \brief Select a large box in the upper, left area of Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_float32 (areaslop).
 * Arg #3 is expected to be a l_int32 (yslop).
 *
 * Leptonica's Notes:
 *      (1) See usage notes in pixSelectLargeULComp().
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
 *
 * Leptonica's Notes:
 *      (1) The copyflag specifies what we do with each box from boxas.
 *          Specifically, L_CLONE inserts a clone into boxad of each
 *          selected box from boxas.
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
 *
 * Leptonica's Notes:
 *      (1) Returns a copy of the boxa if no components are removed.
 *      (2) Uses box copies in the new boxa.
 *      (3) The indicator numa has values 0 (ignore) and 1 (accept).
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
    ll_push_l_int32(_fun, L, changed);
    return 2;
}

/**
 * \brief Set a side of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a string describing the side (side).
 * Arg #3 is expected to be a l_int32 (val).
 * Arg #4 is expected to be a l_int32 (thresh).
 *
 * Leptonica's Notes:
 *      (1) Sets the given side of each box.  Use boxad == NULL for a new
 *          boxa, and boxad == boxas for in-place.
 *      (2) Use one of these:
 *               boxad = boxaSetSide(NULL, boxas, ...);   // new
 *               boxaSetSide(boxas, boxas, ...);  // in-place
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
    l_int32 val = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 thresh = ll_opt_l_int32(_fun, L, 4, 0);
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
 *
 * Leptonica's Notes:
 *      (1) See boxSimilar() for parameter usage.
 *      (2) Corresponding boxes are taken in order in the two boxa.
 *      (3) %nasim is an indicator array with a (0/1) for each box pair.
 *      (4) With %nasim or debug == 1, boxes continue to be tested
 *          after failure.
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
    l_int32 rightdiff = ll_opt_l_int32(_fun, L, 4, leftdiff);
    l_int32 topdiff = ll_opt_l_int32(_fun, L, 5, rightdiff);
    l_int32 botdiff = ll_opt_l_int32(_fun, L, 6, topdiff);
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
    ll_push_l_int32(_fun, L, minw);
    ll_push_l_int32(_fun, L, minh);
    ll_push_l_int32(_fun, L, maxw);
    ll_push_l_int32(_fun, L, maxh);
    return 4;
}

/**
 * \brief Determine the size variation of boxes in a Boxa* (%boxas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string describing the select size (type).
 *
 * Leptonica's Notes:
 *      (1) This gives several measures of the smoothness of either the
 *          width or height of a sequence of boxes.
 *      (2) Statistics can be found separately for even and odd boxes.
 *          Additionally, the average pair-wise difference between
 *          adjacent even and odd boxes can be returned.
 *      (3) The use case is bounding boxes for scanned page images,
 *          where ideally the sizes should have little variance.
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
    ll_push_l_float32(_fun, L, del_evenodd);
    ll_push_l_float32(_fun, L, rms_even);
    ll_push_l_float32(_fun, L, rms_odd);
    ll_push_l_float32(_fun, L, rms_all);
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
 * Arg #5 is an optional boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) This returns a modified version of %boxas by constructing
 *          for each input box a box that has been linear least square fit
 *          (LSF) to the entire set.  The linear fitting is done to each of
 *          the box sides independently, after outliers are rejected,
 *          and it is computed separately for sequences of even and
 *          odd boxes.  Once the linear LSF box is found, the output box
 *          (in %boxad) is constructed from the input box and the LSF
 *          box, depending on %subflag.  See boxaModifyWithBoxa() for
 *          details on the use of %subflag and %maxdiff.
 *      (2) This is useful if, in both the even and odd sets, the box
 *          edges vary roughly linearly with its index in the set.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
SmoothSequenceLS(lua_State *L)
{
    LL_FUNC("SmoothSequenceLS");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 factor = ll_opt_l_int32(_fun, L, 2, 3);
    l_int32 subflag = ll_check_subflag(_fun, L, 3, L_USE_MINSIZE);
    l_int32 maxdiff = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 extrapixels = ll_opt_l_int32(_fun, L, 5, 0);
    l_int32 debug = ll_opt_boolean(_fun, L, 6, FALSE);
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
 * Arg #5 is an optional boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) The target width of the sliding window is 2 * %halfwin + 1.
 *          If necessary, this will be reduced by boxaWindowedMedian().
 *      (2) This returns a modified version of %boxas by constructing
 *          for each input box a box that has been smoothed with windowed
 *          median filtering.  The filtering is done to each of the
 *          box sides independently, and it is computed separately for
 *          sequences of even and odd boxes.  The output %boxad is
 *          constructed from the input boxa and the filtered boxa,
 *          depending on %subflag.  See boxaModifyWithBoxa() for
 *          details on the use of %subflag, %maxdiff and %extrapixels.
 *      (3) This is useful for removing noise separately in the even
 *          and odd sets, where the box edge locations can have
 *          discontinuities but otherwise vary roughly linearly within
 *          intervals of size %halfwin or larger.
 *      (4) If you don't need to handle even and odd sets separately,
 *          just do this:
 *              boxam = boxaWindowedMedian(boxas, halfwin, debug);
 *              boxad = boxaModifyWithBoxa(boxas, boxam, subflag, maxdiff,
 *                                         extrapixels);
 *              boxaDestroy(&boxam);
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
SmoothSequenceMedian(lua_State *L)
{
    LL_FUNC("SmoothSequenceMedian");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 halfwin = ll_opt_l_int32(_fun, L, 2, 20);
    l_int32 subflag = ll_check_subflag(_fun, L, 3, L_USE_MINSIZE);
    l_int32 maxdiff = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 extrapixels = ll_opt_l_int32(_fun, L, 5, 0);
    l_int32 debug = ll_opt_boolean(_fun, L, 6, FALSE);
    Boxa *boxa = boxaSmoothSequenceMedian(boxas, halfwin, subflag, maxdiff, extrapixels, debug);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Sort a Boxa* (%boxas) by given type (%type) and order (%order).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxas).
 * Arg #2 is expected to be a string defining the sort type (type).
 * Arg #3 is expected to be a string defining the sort order (order).
 *
 * Leptonica's Notes:
 *      (1) An empty boxa returns a copy, with a warning.
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
 *
 * Leptonica's Notes:
 *      (1) The final result is a sort where the 'fast scan' direction is
 *          left to right, and the 'slow scan' direction is from top
 *          to bottom.  Each boxa in the baa represents a sorted set
 *          of boxes from left to right.
 *      (2) Three passes are used to aggregate the boxas, which can correspond
 *          to characters or words in a line of text.  In pass 1, only
 *          taller components, which correspond to xheight or larger,
 *          are permitted to start a new boxa.  In pass 2, the remaining
 *          vertically-challenged components are allowed to join an
 *          existing boxa or start a new one.  In pass 3, boxa whose extent
 *          is overlapping are joined.  After that, the boxes in each
 *          boxa are sorted horizontally, and finally the boxa are
 *          sorted vertically.
 *      (3) If delta1 < 0, the first pass allows aggregation when
 *          boxes in the same boxa do not overlap vertically.
 *          The distance by which they can miss and still be aggregated
 *          is the absolute value |delta1|.   Similar for delta2 on
 *          the second pass.
 *      (4) On the first pass, any component of height less than minh1
 *          cannot start a new boxa; it's put aside for later insertion.
 *      (5) On the second pass, any small component that doesn't align
 *          with an existing boxa can start a new one.
 *      (6) This can be used to identify lines of text from
 *          character or word bounding boxes.
 *      (7) Typical values for the input parameters on 300 ppi text are:
 *                 delta1 ~ 0
 *                 delta2 ~ 0
 *                 minh1 ~ 5
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxaa* (%boxaa) and Numaa* (%naad) on the Lua stack
 */
static int
Sort2d(lua_State *L)
{
    LL_FUNC("Sort2d");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 delta1 = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 delta2 = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 minh1 = ll_opt_l_int32(_fun, L, 4, 5);
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
 *
 * Leptonica's Notes:
 *      (1) If %fillflag == 1, boxae has copies of the even boxes
 *          in their original location, and nvalid boxes are placed
 *          in the odd array locations.  And v.v.
 *      (2) If %fillflag == 0, boxae has only copies of the even boxes.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Boxa* on the Lua stack (boxae, boxao)
 */
static int
SplitEvenOdd(lua_State *L)
{
    LL_FUNC("SplitEvenOdd ");
    Boxa *boxa = ll_check_Boxa(_fun, L, 1);
    l_int32 fillflag = ll_opt_l_int32(_fun, L, 2, 0);
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
    l_int32 transx = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 transy = ll_opt_l_int32(_fun, L, 3, 0);
    Boxa *boxa = boxaTranslate(boxas, transx, transy);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Windowed median for Boxa* (%boxas) giving Boxa* (%boxa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxa* (boxa).
 * Arg #2 is expected to be a l_int32 (halfwin).
 * Arg #3 is an optional boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) This finds a set of boxes (boxad) where each edge of each box is
 *          a windowed median smoothed value to the edges of the
 *          input set of boxes (boxas).
 *      (2) Invalid input boxes are filled from nearby ones.
 *      (3) The returned boxad can then be used in boxaModifyWithBoxa()
 *          to selectively change the boxes in the source boxa.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
WindowedMedian(lua_State *L)
{
    LL_FUNC("WindowedMedian");
    Boxa *boxas = ll_check_Boxa(_fun, L, 1);
    l_int32 halfwin = ll_opt_l_int32(_fun, L, 2, 3);
    l_int32 debug = ll_opt_boolean(_fun, L, 3, FALSE);
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
 *
 * Leptonica's Notes:
 *      (1) Serializes a boxa in memory and puts the result in a buffer.
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
    ll_free(data);
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
 * \brief Check Lua stack at index %arg for udata of class Boxa*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_check_Boxa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Boxa>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Boxa* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_take_Boxa(const char *_fun, lua_State *L, int arg)
{
    Boxa **pboxa = ll_check_udata<Boxa>(_fun, L, arg, TNAME);
    Boxa *boxa = *pboxa;
    DBG(LOG_TAKE, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "pboxa", reinterpret_cast<void *>(pboxa),
        "boxa", reinterpret_cast<void *>(boxa));
    *pboxa = nullptr;
    return boxa;
}

/**
 * \brief Take a Boxa* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
Boxa *
ll_get_global_Boxa(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_Boxa(_fun, L, -1);
}

/**
 * \brief Optionally expect a Boxa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxa* contained in the user data
 */
Boxa *
ll_opt_Boxa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
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
    return ll_push_udata(_fun, L, TNAME, boxa);
}

/**
 * \brief Create and push a new Boxa*.
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
int
ll_new_Boxa(lua_State *L)
{
    FUNC("ll_new_Boxa");
    Boxa* boxa = nullptr;

    if (ll_isudata(_fun, L, 1, LL_BOXA)) {
        Boxa *boxas = ll_opt_Boxa(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(boxas));
        boxa = boxaCopy(boxas, L_COPY);
    }

    if (!boxa && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        boxa = boxaReadStream(stream->f);
    }

    if (!boxa && ll_isinteger(_fun, L, 1)) {
        l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxa = boxaCreate(n);
    }

    if (!boxa && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        boxa = boxaRead(filename);
    }

    if (!boxa && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        boxa = boxaReadMem(data, size);
    }

    if (!boxa) {
        l_int32 n = 1;
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxa = boxaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(boxa));
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_Boxa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Boxa},
        {"__len",                   GetCount},
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
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Boxa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
