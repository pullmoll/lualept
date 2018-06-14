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
 * \file llbox.cpp
 * \class Box
 *
 * A box: a quad of l_int32 (x, y, w, h).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BOX

/** Define a function's name (_fun) with prefix Box */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the box.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Box *box = ll_take_udata<Box>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "box", reinterpret_cast<void *>(box),
        "refcount", boxGetRefcount(box));
    boxDestroy(&box);
    return 0;
}

/**
 * \brief Printable string for a Box* (%box).
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Box *box = ll_check_Box(_fun, L, 1);
    luaL_Buffer B;
    l_int32 x, y, w, h;

    luaL_buffinit(L, &B);
    if (!box) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(box));
        luaL_addstring(&B, str);
        if (boxGetGeometry(box, &x, &y, &w, &h)) {
            snprintf(str, LL_STRBUFF, "\n    invalid");
        } else {
            snprintf(str, LL_STRBUFF, "\n    %s = %d, %s = %d, %s = %d, %s = %d, %s = %d",
                     "x", x, "y", y, "w", w, "h", h, "area", w * h);
        }
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Test equality of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Equal(lua_State *L)
{
    LL_FUNC("Equal");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 same = FALSE;
    if (boxEqual(box1, box2, &same))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, same);
}

/**
 * \brief Adjust sides of a Box* (%boxs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxd).
 * Arg #2 (i.e. self) is expected to be a Box* (boxs).
 * Arg #3 is expected to be a l_int32 (delleft).
 * Arg #4 is expected to be a l_int32 (delright).
 * Arg #5 is expected to be a l_int32 (deltop).
 * Arg #6 is expected to be a l_int32 (delbot).
 *
 * Leptonica's Notes:
 *      (1) Set boxd == NULL to get new box; boxd == boxs for in-place;
 *          or otherwise to resize existing boxd.
 *      (2) For usage, suggest one of these:
 *               boxd = boxAdjustSides(NULL, boxs, ...);   // new
 *               boxAdjustSides(boxs, boxs, ...);          // in-place
 *               boxAdjustSides(boxd, boxs, ...);          // other
 *      (3) New box dimensions are cropped at left and top to x >= 0 and y >= 0.
 *      (4) For example, to expand in-place by 20 pixels on each side, use
 *             boxAdjustSides(box, box, -20, 20, -20, 20);
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
AdjustSides(lua_State *L)
{
    LL_FUNC("AdjustSides");
    Box *boxd = ll_opt_Box(_fun, L, 1);
    Box *boxs = ll_check_Box(_fun, L, 2);
    l_int32 delleft = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 delright = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 deltop = ll_opt_l_int32(_fun, L, 5, 0);
    l_int32 delbot = ll_opt_l_int32(_fun, L, 6, 0);
    Box *box = boxAdjustSides(boxd, boxs, delleft, delright, deltop, delbot);
    ll_push_Box(_fun, L, box);
    return 1;
}

/**
 * \brief Get the bounding region of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 *
 * Leptonica's Notes:
 *      (1) This is the geometric union of the two rectangles.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
BoundingRegion(lua_State *L)
{
    LL_FUNC("BoundingRegion");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    Box *box = boxBoundingRegion(box1, box2);
    ll_push_Box(_fun, L, box);
    return 1;
}

/**
 * \brief Change the Box* reference count.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 (i.e. self) is expected to be a l_int32 (delta).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the stack.
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxChangeRefcount(box, delta));
}

/**
 * \brief Clip a Box* (%boxs) rectangle to width and height (%wi, %hi).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 is expected to be a l_int32 (wi).
 * Arg #3 is expected to be a l_int32 (hi).
 *
 * Leptonica's Notes:
 *      (1) This can be used to clip a rectangle to an image.
 *          The clipping rectangle is assumed to have a UL corner at (0, 0),
 *          and a LR corner at (wi - 1, hi - 1).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
ClipToRectangle(lua_State *L)
{
    LL_FUNC("ClipToRectangle");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_int32 wi = ll_check_l_int32(_fun, L, 2);
    l_int32 hi = ll_check_l_int32(_fun, L, 3);
    Box *box = boxClipToRectangle(boxs, wi, hi);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Clip a Box* (%boxs) rectangle to width and height (w,h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) The return value should be checked.  If it is 1, the
 *          returned parameter values are bogus.
 *      (2) This simplifies the selection of pixel locations within
 *          a given rectangle:
 *             for (i = ystart; i < yend; i++ {
 *                 ...
 *                 for (j = xstart; j < xend; j++ {
 *                     ....
 * </pre>
 * \param L Lua state.
 * \return 6 integers on the Lua stack (%xstart, %ystart, %xend, %yend, %bw, %bh).
 */
static int
ClipToRectangleParams(lua_State *L)
{
    LL_FUNC("ClipToRectangleParams");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    l_int32 xstart = 0;
    l_int32 ystart = 0;
    l_int32 xend = 0;
    l_int32 yend = 0;
    l_int32 bw = 0;
    l_int32 bh = 0;
    if (boxClipToRectangleParams(boxs, w, h, &xstart, &ystart, &xend, &yend, &bw, &bh))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xstart);
    ll_push_l_int32(_fun, L, ystart);
    ll_push_l_int32(_fun, L, xend);
    ll_push_l_int32(_fun, L, yend);
    ll_push_l_int32(_fun, L, bw);
    ll_push_l_int32(_fun, L, bh);
    return 6;
}

/**
 * \brief Clone a Box* (%boxs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Box *boxs = ll_check_Box(_fun, L, 1);
    Box *box = boxClone(boxs);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Compare the size of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * Arg #3 is expected to be a string describing the type of comparison (type).
 *
 * Leptonica's Notes:
 *      (1) We're re-using the SORT enum for these comparisons.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
CompareSize(lua_State *L)
{
    LL_FUNC("CompareSize");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 type = ll_check_sort_by(_fun, L, 3, L_SORT_BY_WIDTH);
    l_int32 rel = 0;
    if (boxCompareSize(box1, box2, type, &rel))
        return ll_push_nil(L);
    ll_push_string(_fun, L, ll_string_relation(rel));
    return 1;
}

/**
 * \brief Check if a Box* (%box1) contains another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Contains(lua_State *L)
{
    LL_FUNC("Contains");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 contains = 0;
    if (boxContains(box1, box2, &contains))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, contains);
}

/**
 * \brief Check if a Box* (%box) contains a point (%x, %y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a l_float32 (x).
 * Arg #3 is expected to be a l_float32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ContainsPt(lua_State *L)
{
    LL_FUNC("ContainsPt");
    Box *box = ll_check_Box(_fun, L, 1);
    l_float32 x = ll_check_l_float32(_fun, L, 2);
    l_float32 y = ll_check_l_float32(_fun, L, 3);
    l_int32 contains = FALSE;
    if (boxContainsPt(box, x, y, &contains))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, contains);
}

/**
 * \brief Convert corners (%ncorners) of a Box* (%box) to a Pta* (%pta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a l_int32 (ncorners).
 *
 * Leptonica's Notes:
 *      (1) If ncorners == 2, we select the UL and LR corners.
 *          Otherwise we save all 4 corners in this order: UL, UR, LL, LR.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
ConvertToPta(lua_State *L)
{
    LL_FUNC("ConvertToPta");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 ncorners = ll_check_l_int32(_fun, L, 2);
    Pta *pta = boxConvertToPta(box, ncorners);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Copy a Box* (%boxs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Box *boxs = ll_check_Box(_fun, L, 1);
    Box *box = boxCopy(boxs);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Create a new Box*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (x).
 * Arg #2 is expected to be a l_int32 (y).
 * Arg #3 is expected to be a l_int32 (w).
 * Arg #4 is expected to be a l_int32 (h).
 *
 * Leptonica's Notes:
 *      (1) This clips the box to the +quad.  If no part of the
 *          box is in the +quad, this returns NULL.
 *      (2) We allow you to make a box with w = 0 and/or h = 0.
 *          This does not represent a valid region, but it is useful
 *          as a placeholder in a boxa for which the index of the
 *          box in the boxa is important.  This is an atypical
 *          situation; usually you want to put only valid boxes with
 *          nonzero width and height in a boxa.  If you have a boxa
 *          with invalid boxes, the accessor boxaGetValidBox()
 *          will return NULL on each invalid box.
 *      (3) If you want to create only valid boxes, use boxCreateValid(),
 *          which returns NULL if either w or h is 0.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 x = ll_opt_l_int32(_fun, L, 1, 0);
    l_int32 y = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 w = ll_opt_l_int32(_fun, L, 3, 1);
    l_int32 h = ll_opt_l_int32(_fun, L, 4, 1);
    Box *box = boxCreate(x, y, w, h);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Create a new Box* (%box) if the parameters are valid.
 * <pre>
 * Arg #1 is expected to be a l_int32 (x).
 * Arg #2 is expected to be a l_int32 (y).
 * Arg #3 is expected to be a l_int32 (w).
 * Arg #4 is expected to be a l_int32 (h).
 *
 * Leptonica's Notes:
 *      (1) This returns NULL if either w = 0 or h = 0.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
CreateValid(lua_State *L)
{
    LL_FUNC("CreateValid");
    l_int32 x, y, w, h;
    Box *box;

    x = ll_opt_l_int32(_fun, L, 1, 0);
    y = ll_opt_l_int32(_fun, L, 2, 0);
    w = ll_opt_l_int32(_fun, L, 3, 1);
    h = ll_opt_l_int32(_fun, L, 4, 1);
    box = boxCreateValid(x, y, w, h);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get the center of a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * </pre>
 * \param L Lua state.
 * \return 2 numbers on the Lua stack (%cx, %cy).
 */
static int
GetCenter(lua_State *L)
{
    LL_FUNC("GetCenter");
    Box *box = ll_check_Box(_fun, L, 1);
    l_float32 cx = 0.0f;
    l_float32 cy = 0.0f;
    if (boxGetCenter(box, &cx, &cy))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, cx);
    ll_push_l_float32(_fun, L, cy);
    return 2;
}

/**
 * \brief Get the Box* geometry.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * </pre>
 * \param L Lua state.
 * \return 4 for four integers (or nil on error) on the stack.
 */
static int
GetGeometry(lua_State *L)
{
    LL_FUNC("GetGeometry");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 x, y, w, h;
    if (boxGetGeometry(box, &x, &y, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 4;
}

/**
 * \brief Get the Box* reference count.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * </pre>
 * \param L Lua state.
 * \return 1 integers (or nil on error) on the stack.
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    Box *box = ll_check_Box(_fun, L, 1);
    ll_push_l_int32(_fun, L, boxGetRefcount(box));
    return 1;
}

/**
 * \brief Get the BOX side locations (left, right, top, bottom).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) All returned values are within the box.
 * </pre>
 * \param L Lua state.
 * \return 4 for four integers (or nil on error) on the stack.
 */
static int
GetSideLocations(lua_State *L)
{
    LL_FUNC("GetSideLocations");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 l, r, t, b;
    if (boxGetSideLocations(box, &l, &r, &t, &b))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, l);
    ll_push_l_int32(_fun, L, r);
    ll_push_l_int32(_fun, L, t);
    ll_push_l_int32(_fun, L, b);
    return 4;
}

/**
 * \brief Intersect a Box* (%box) by a line (%x, %y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_float32 (slope).
 *
 * Leptonica's Notes:
 *      (1) If the intersection is at only one point (a corner), the
 *          coordinates are returned in (x1, y1).
 *      (2) Represent a vertical line by one with a large but finite slope.
 * </pre>
 * \param L Lua state.
 * \return 5 integers on the Lua stack (%x1, %y1, %x2, %y2, %n).
 */
static int
IntersectByLine(lua_State *L)
{
    LL_FUNC("IntersectByLine");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float32 slope = ll_check_l_float32(_fun, L, 4);
    l_int32 x1 = 0;
    l_int32 y1 = 0;
    l_int32 x2 = 0;
    l_int32 y2 = 0;
    l_int32 n = 0;
    if (boxIntersectByLine(box, x, y, slope, &x1, &y1, &x2, &y2, &n))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, x1);
    ll_push_l_int32(_fun, L, y1);
    ll_push_l_int32(_fun, L, x2);
    ll_push_l_int32(_fun, L, y2);
    ll_push_l_int32(_fun, L, n);
    return 5;
}

/**
 * \brief Check if a Box* (%box1) intersects another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Intersects(lua_State *L)
{
    LL_FUNC("Intersects");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 intersects = 0;
    if (boxIntersects(box1, box2, &intersects))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, intersects);
}

/**
 * \brief Check if a Box* is valid.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
IsValid(lua_State *L)
{
    LL_FUNC("IsValid");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 valid = 0;
    if (boxIsValid(box, &valid))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, valid);
}

/**
 * \brief Get the overlap area of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
OverlapArea(lua_State *L)
{
    LL_FUNC("OverlapArea");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 area = 0.0f;
    if (boxOverlapArea(box1, box2, &area))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, area);
    return 1;
}

/**
 * \brief Get the overlap fraction of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 *
 * Leptonica's Notes:
 *      (1) The result depends on the order of the input boxes,
 *          because the overlap is taken as a fraction of box2.
 * </pre>
 * \param L Lua state.
 * \return 1 number on the Lua stack.
 */
static int
OverlapFraction(lua_State *L)
{
    LL_FUNC("OverlapFraction");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_float32 fract = 0.0f;
    if (boxOverlapFraction(box1, box2, &fract))
        return ll_push_nil(L);
    return ll_push_l_float32(_fun, L, fract);
}

/**
 * \brief Get the overlap region of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 *
 * Leptonica's Notes:
 *      (1) This is the geometric intersection of the two rectangles.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
OverlapRegion(lua_State *L)
{
    LL_FUNC("OverlapRegion");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    Box *box = boxOverlapRegion(box1, box2);
    ll_push_Box(_fun, L, box);
    return 1;
}

/**
 * \brief Print info about a Box* (%box) to a Lua stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a luaL_Stream io handle (stream).
 *
 * Leptonica's Notes:
 *      (1) This outputs debug info.  Use serialization functions to
 *          write to file if you want to read the data back.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
PrintStreamInfo(lua_State *L)
{
    LL_FUNC("PrintStreamInfo");
    Box *box = ll_check_Box(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxPrintStreamInfo(stream->f, box));
}

/**
 * \brief Relocate one side of a Box* (%boxs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) Set boxd == NULL to get new box; boxd == boxs for in-place;
 *          or otherwise to resize existing boxd.
 *      (2) For usage, suggest one of these:
 *               boxd = boxRelocateOneSide(NULL, boxs, ...);   // new
 *               boxRelocateOneSide(boxs, boxs, ...);          // in-place
 *               boxRelocateOneSide(boxd, boxs, ...);          // other
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
RelocateOneSide(lua_State *L)
{
    LL_FUNC("RelocateOneSide");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_int32 loc = ll_check_l_int32(_fun, L, 2);
    l_int32 sideflag = ll_check_from_side(_fun, L, 3, L_FROM_LEFT);
    Box *boxd = boxRelocateOneSide(nullptr, boxs, loc, sideflag);
    ll_push_Box(_fun, L, boxd);
    return 1;
}

/**
 * \brief Rotate a Box* (%boxs) orthogonally.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 * Arg #4 is expected to be a rotation angle (rotation).
 *
 * Leptonica's Notes:
 *      (1) Rotate the image with the embedded box by the specified amount.
 *      (2) After rotation, the rotated box is always measured with
 *          respect to the UL corner of the image.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    l_int32 rotation = ll_check_rotation(_fun, L, 4, 0);
    Box *box = boxRotateOrth(boxs, w, h, rotation);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Get the separation distances of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 *
 * Leptonica's Notes:
 *      (1) This measures horizontal and vertical separation of the
 *          two boxes.  If the boxes are touching but have no pixels
 *          in common, the separation is 0.  If the boxes overlap by
 *          a distance d, the returned separation is -d.
 * </pre>
 * \param L Lua state.
 * \return 2 integers on the Lua stack.
 */
static int
SeparationDistance(lua_State *L)
{
    LL_FUNC("SeparationDistance");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 h_sep = 0;
    l_int32 v_sep = 0;
    if (boxSeparationDistance(box1, box2, &h_sep, &v_sep))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, h_sep);
    ll_push_l_int32(_fun, L, v_sep);
    return 2;
}

/**
 * \brief Set the Box* (%box) geometry.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 is expected to be a lua_Integer (x).
 * Arg #3 is expected to be a lua_Integer (y).
 * Arg #4 is expected to be a lua_Integer (w).
 * Arg #5 is expected to be a lua_Integer (h).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetGeometry(lua_State *L)
{
    LL_FUNC("SetGeometry");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 x = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 y = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 w = ll_opt_l_int32(_fun, L, 4, 1);
    l_int32 h = ll_opt_l_int32(_fun, L, 5, 1);
    return ll_push_boolean(_fun, L, 0 == boxSetGeometry(box, x, y, w, h));
}

/**
 * \brief Set the Box* (%box) side locations (%l, %r, %t, %b).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a lua_Integer (l).
 * Arg #3 is expected to be a lua_Integer (r).
 * Arg #4 is expected to be a lua_Integer (t).
 * Arg #5 is expected to be a lua_Integer (b).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetSideLocations(lua_State *L)
{
    LL_FUNC("SetSideLocations");
    Box *box = ll_check_Box(_fun, L, 1);
    l_int32 l = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 r = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 t = ll_opt_l_int32(_fun, L, 4, 0);
    l_int32 b = ll_opt_l_int32(_fun, L, 5, 0);
    return ll_push_boolean(_fun, L, 0 == boxSetSideLocations(box, l, r, t, b));
}

/**
 * \brief Test similarity of a Box* (%box1) and another Box* (%box2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box1).
 * Arg #2 is expected to be another Box* (box2).
 * Arg #3 is expected to be a l_int32 (leftdiff).
 * Arg #4 is expected to be a l_int32 (rightdiff).
 * Arg #5 is expected to be a l_int32 (topdiff).
 * Arg #6 is expected to be a l_int32 (botdiff).
 *
 * Leptonica's Notes:
 *      (1) The values of leftdiff (etc) are the maximum allowed deviations
 *          between the locations of the left (etc) sides.  If any side
 *          pairs differ by more than this amount, the boxes are not similar.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Similar(lua_State *L)
{
    LL_FUNC("Similar");
    Box *box1 = ll_check_Box(_fun, L, 1);
    Box *box2 = ll_check_Box(_fun, L, 2);
    l_int32 leftdiff = ll_check_l_int32(_fun, L, 3);
    l_int32 rightdiff = ll_opt_l_int32(_fun, L, 4, leftdiff);
    l_int32 topdiff = ll_opt_l_int32(_fun, L, 5, rightdiff);
    l_int32 botdiff = ll_opt_l_int32(_fun, L, 6, topdiff);
    l_int32 similar = FALSE;
    if (boxSimilar(box1, box2, leftdiff, rightdiff, topdiff, botdiff, &similar))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, similar);
}

/**
 * \brief Transform a Box* (%boxs) by shifting and scaling.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 is expected to be a l_int32 (shiftx).
 * Arg #3 is expected to be a l_int32 (shifty).
 * Arg #4 is an optional l_float32 (scalex).
 * Arg #5 is an optional l_float32 (scaley).
 *
 * Leptonica's Notes:
 *      (1) This is a very simple function that first shifts, then scales.
 *      (2) If the box is invalid, a new invalid box is returned.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
Transform(lua_State *L)
{
    LL_FUNC("Transform");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_int32 shiftx = ll_check_l_int32(_fun, L, 2);
    l_int32 shifty = ll_check_l_int32(_fun, L, 3);
    l_float32 scalex = ll_opt_l_float32(_fun, L, 4, 1.0f);
    l_float32 scaley = ll_opt_l_float32(_fun, L, 5, 1.0f);
    Box *box = boxTransform(boxs, shiftx, shifty, scalex, scaley);
    ll_push_Box(_fun, L, box);
    return 1;
}

/**
 * \brief Ordered transform a Box* (%boxs) by shifting, scaling, and rotation..
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxs).
 * Arg #2 is an optional l_int32 (shiftx).
 * Arg #3 is an optional l_int32 (shifty).
 * Arg #4 is an optional l_float32 (scalex).
 * Arg #5 is an optional l_float32 (scaley).
 * Arg #6 is an optional l_int32 (xcen).
 * Arg #7 is an optional l_int32 (ycen).
 * Arg #8 is an optional l_float32 (angle).
 * Arg #9 is an optional string describing the transform order (order).
 *
 * Leptonica's Notes:
 *      (1) This allows a sequence of linear transforms, composed of
 *          shift, scaling and rotation, where the order of the
 *          transforms is specified.
 *      (2) The rotation is taken about a point specified by (xcen, ycen).
 *          Let the components of the vector from the center of rotation
 *          to the box center be (xdif, ydif):
 *            xdif = (bx + 0.5 * bw) - xcen
 *            ydif = (by + 0.5 * bh) - ycen
 *          Then the box center after rotation has new components:
 *            bxcen = xcen + xdif * cosa + ydif * sina
 *            bycen = ycen + ydif * cosa - xdif * sina
 *          where cosa and sina are the cos and sin of the angle,
 *          and the enclosing box for the rotated box has size:
 *            rw = |bw * cosa| + |bh * sina|
 *            rh = |bh * cosa| + |bw * sina|
 *          where bw and bh are the unrotated width and height.
 *          Then the box UL corner (rx, ry) is
 *            rx = bxcen - 0.5 * rw
 *            ry = bycen - 0.5 * rh
 *      (3) The center of rotation specified by args %xcen and %ycen
 *          is the point BEFORE any translation or scaling.  If the
 *          rotation is not the first operation, this function finds
 *          the actual center at the time of rotation.  It does this
 *          by making the following assumptions:
 *             (1) Any scaling is with respect to the UL corner, so
 *                 that the center location scales accordingly.
 *             (2) A translation does not affect the center of
 *                 the image; it just moves the boxes.
 *          We always use assumption (1).  However, assumption (2)
 *          will be incorrect if the apparent translation is due
 *          to a clipping operation that, in effect, moves the
 *          origin of the image.  In that case, you should NOT use
 *          these simple functions.  Instead, use the functions
 *          in affinecompose.c, where the rotation center can be
 *          computed from the actual clipping due to translation
 *          of the image origin.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
TransformOrdered(lua_State *L)
{
    LL_FUNC("TransformOrdered");
    Box *boxs = ll_check_Box(_fun, L, 1);
    l_float32 xc, yc;
    l_int32 ok = boxGetCenter(boxs, &xc, &yc);
    l_int32 shiftx = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 shifty = ll_opt_l_int32(_fun, L, 3, 0);
    l_float32 scalex = ll_opt_l_float32(_fun, L, 4, 1.0f);
    l_float32 scaley = ll_opt_l_float32(_fun, L, 5, 1.0f);
    l_int32 xcen = ll_opt_l_int32(_fun, L, 6, ok ? static_cast<l_int32>(xc) : 0);
    l_int32 ycen = ll_opt_l_int32(_fun, L, 7, ok ? static_cast<l_int32>(yc) : 0);
    l_float32 angle = ll_opt_l_float32(_fun, L, 8, 0.0f);
    l_int32 order = ll_check_trans_order(_fun, L, 9, L_TR_SC_RO);
    Box *box = boxTransformOrdered(boxs, shiftx, shifty, scalex, scaley, xcen, ycen, angle, order);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Check Lua stack at index %arg for user data of class Box*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Box* contained in the user data.
 */
Box *
ll_check_Box(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Box>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Box* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Box* contained in the user data.
 */
Box *
ll_opt_Box(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Box(_fun, L, arg);
}

/**
 * \brief Push Box* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param box pointer to the BOX
 * \return 1 Box* on the Lua stack.
 */
int
ll_push_Box(const char *_fun, lua_State *L, Box *box)
{
    if (!box)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, box);
}

/**
 * \brief Create and push a new Box*.
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
int
ll_new_Box(lua_State *L)
{
    FUNC("ll_new_Box");
    Box *box = nullptr;
    l_int32 x = 0, y = 0, w = 0, h = 0;

    if (ll_isudata(_fun, L, 1, LL_BOX)) {
        Box *boxs = ll_opt_Box(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(boxs));
        box = boxCopy(boxs);
    }

    if (!box && ll_isinteger(_fun, L, 1)) {
        x = ll_opt_l_int32(_fun, L, 1, 0);
        y = ll_opt_l_int32(_fun, L, 2, 0);
        w = ll_opt_l_int32(_fun, L, 3, 1);
        h = ll_opt_l_int32(_fun, L, 4, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %d, %s = %d\n", _fun,
            "x", x, "y", y, "w", w, "h", h);
        box = boxCreate(x, y, w, h);
    }

    if (!box) {
        x = y = w = h = 0;
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %d, %s = %d\n", _fun,
            "x", x, "y", y, "w", w, "h", h);
        box = boxCreate(x, y, w, h);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(box));
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Register the Box* methods and functions in the Box meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Box(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Box},
        {"__tostring",              toString},
        {"__eq",                    Equal},
        {"__band",                  OverlapRegion},     /* box = box1 & box2 */
        {"__bor",                   BoundingRegion},    /* box = box1 | box2 */
        {"AdjustSides",             AdjustSides},
        {"BoundingRegion",          BoundingRegion},
        {"ChangeRefcount",          ChangeRefcount},
        {"ClipToRectangle",         ClipToRectangle},
        {"ClipToRectangleParams",   ClipToRectangleParams},
        {"Clone",                   Clone},
        {"CompareSize",             CompareSize},
        {"Contains",                Contains},
        {"ContainsPt",              ContainsPt},
        {"ConvertToPta",            ConvertToPta},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"CreateValid",             CreateValid},
        {"Destroy",                 Destroy},
        {"Equal",                   Equal},
        {"GetCenter",               GetCenter},
        {"GetGeometry",             GetGeometry},
        {"GetRefcount",             GetRefcount},
        {"GetSideLocations",        GetSideLocations},
        {"IntersectByLine",         IntersectByLine},
        {"Intersects",              Intersects},
        {"IsValid",                 IsValid},
        {"OverlapArea",             OverlapArea},
        {"OverlapFraction",         OverlapFraction},
        {"OverlapRegion",           OverlapRegion},
        {"PrintStreamInfo",         PrintStreamInfo},
        {"RelocateOneSide",         RelocateOneSide},
        {"RotateOrth",              RotateOrth},
        {"SeparationDistance",      SeparationDistance},
        {"SetGeometry",             SetGeometry},
        {"SetSideLocations",        SetSideLocations},
        {"Similar",                 Similar},
        {"Transform",               Transform},
        {"TransformOrdered",        TransformOrdered},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Box);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
