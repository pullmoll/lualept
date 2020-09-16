/************************************************************************
 * Copyright (c) Jürgen Buchmüller <pullmoll@t-online.de>
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
 * \file llccborda.cpp
 * \class CCBorda
 *
 * An array of %CCBord.
 *
 * Excerpt from Leptonica's file comment:
 *
 *     Border finding is tricky because components can have
 *     holes, which also need to be traced out.  The outer
 *     border can be connected with all the hole borders,
 *     so that there is a single border for each component.
 *     [Alternatively, the connecting paths can be eliminated if
 *     you're willing to have a set of borders for each
 *     component (an exterior border and some number of
 *     interior ones), with "line to" operations tracing
 *     out each border and "move to" operations going from
 *     one border to the next.]
 *
 *     Here's the plan.  We get the pix for each connected
 *     component, and trace its exterior border.  We then
 *     find the holes (if any) in the pix, and separately
 *     trace out their borders, all using the same
 *     border-following rule that has ON pixels on the right
 *     side of the path.
 *
 *     [For svg, we may want to turn each set of borders for a c.c.
 *     into a closed path.  This can be done by tunnelling
 *     through the component from the outer border to each of the
 *     holes, going in and coming out along the same path so
 *     the connection will be invisible in any rendering
 *     (display or print) from the outline.  The result is a
 *     closed path, where the outside border is traversed
 *     cw and each hole is traversed ccw.  The svg renderer
 *     is assumed to handle these closed borders properly.]
 *
 *     Each border is a closed path that is traversed in such
 *     a way that the stuff inside the c.c. is on the right
 *     side of the traveller.  The border of a singly-connected
 *     component is thus traversed cw, and the border of the
 *     holes inside a c.c. are traversed ccw.  Suppose we have
 *     a list of all the borders of each c.c., both the cw and ccw
 *     traversals.  How do we reconstruct the image?
 *
 *   Reconstruction:
 *
 *     Method 1.  Topological method using connected components.
 *     We have closed borders composed of cw border pixels for the
 *     exterior of c.c. and ccw border pixels for the interior (holes)
 *     in the c.c.
 *         (a) Initialize the destination to be OFF.  Then,
 *             in any order:
 *         (b) Fill the components within and including the cw borders,
 *             and sequentially XOR them onto the destination.
 *         (c) Fill the components within but not including the ccw
 *             borders and sequentially XOR them onto the destination.
 *     The components that are XOR'd together can be generated as follows:
 *         (a) For each closed cw path, use pixFillClosedBorders():
 *               (1) Turn on the path pixels in a subimage that
 *                   minimally supports the border.
 *               (2) Do a 4-connected fill from a seed of 1 pixel width
 *                   on the border, using the inverted image in (1) as
 *                   a filling mask.
 *               (3) Invert the fill result: this gives the component
 *                   including the exterior cw path, with all holes
 *                   filled.
 *         (b) For each closed ccw path (hole):
 *               (1) Turn on the path pixels in a subimage that minimally
 *                   supports the path.
 *               (2) Find a seed pixel on the inside of this path.
 *               (3) Do a 4-connected fill from this seed pixel, using
 *                   the inverted image of the path in (1) as a filling
 *                   mask.
 *
 *     ------------------------------------------------------
 *
 *     Method 2.  A variant of Method 1.  Topological.
 *     In Method 1, we treat the exterior border differently from
 *     the interior (hole) borders.  Here, all borders in a c.c.
 *     are treated equally:
 *         (1) Start with a pix with a 1 pixel OFF boundary
 *             enclosing all the border pixels of the c.c.
 *             This is the filling mask.
 *         (2) Make a seed image of the same size as follows:  for
 *             each border, put one seed pixel OUTSIDE the border
 *             (where OUTSIDE is determined by the inside/outside
 *             convention for borders).
 *         (3) Seedfill into the seed image, filling in the regions
 *             determined by the filling mask.  The fills are clipped
 *             by the border pixels.
 *         (4) Inverting this, we get the c.c. properly filled,
 *             with the holes empty!
 *         (5) Rasterop using XOR the filled c.c. (but not the 1
 *             pixel boundary) into the full dest image.
 *
 *     Method 2 is about 1.2x faster than Method 1 on text images,
 *     and about 2x faster on complex images (e.g., with halftones).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_CCBORDA

/** Define a function's name (_fun) with prefix CCBorda */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CCBorda *ccba = ll_take_udata<CCBorda>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
	TNAME,
	"ccba", reinterpret_cast<void *>(ccba),
	"count", ccbaGetCount(ccba));
    ccbaDestroy(&ccba);
    return 0;
}

/**
 * \brief Printable string for a CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!ccba) {
	luaL_addstring(&B, "nil");
    } else {
	snprintf(str, LL_STRBUFF,
		TNAME "*: %p",
		reinterpret_cast<void *>(ccba));
	luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
	snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
		 "pix", LL_PIX, reinterpret_cast<void *>(ccba->pix));
	luaL_addstring(&B, str);
	snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
		 "w", ccba->w);
	luaL_addstring(&B, str);
	snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
		 "h", ccba->h);
	luaL_addstring(&B, str);
	snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
		 "n", ccba->n);
	luaL_addstring(&B, str);
	snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
		 "nalloc", ccba->nalloc);
	luaL_addstring(&B, str);
	snprintf(str, LL_STRBUFF, "\n    %-14s: %s** %p",
		 "ccb", LL_CCBORD, reinterpret_cast<void *>(ccba->ccb));
	luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_l_int32(_fun, L, ccbaGetCount(ccba));
}

/**
 * \brief Create a new CCBorda*.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 CCBorda* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix* pixs = ll_check_Pix(_fun, L, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    CCBorda *ccba = ccbaCreate(pixs, n);
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Add the CCBord* (%ccb) to the CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a CCBord* (ccb).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddCcb(lua_State *L)
{
    LL_FUNC("AddCcb");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    CCBord *ccb = ll_check_CCBord(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaAddCcb(ccba, ccb));
}

/**
 * \brief Display the border of CCBorda* (%ccba) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Uses global ptaa, which gives each border pixel in
 *          global coordinates, and must be computed in advance
 *          by calling ccbaGenerateGlobalLocs().
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
DisplayBorder(lua_State *L)
{
    LL_FUNC("DisplayBorder");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayBorder(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Display local chain point array for CCBorda* (%ccba) with method 1.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Uses local ptaa, which gives each border pixel in
 *          local coordinates, so the actual pixel positions must
 *          be computed using all offsets.
 *      (2) For the holes, use coordinates relative to the c.c.
 *      (3) This is slower than Method 2.
 *      (4) This uses topological properties (Method 1) to do scan
 *          conversion to raster
 *
 *  This algorithm deserves some commentary.
 *
 *  I first tried the following:
 *    ~ outer borders: 4-fill from outside, stopping at the
 *         border, using pixFillClosedBorders()
 *    ~ inner borders: 4-fill from outside, stopping again
 *         at the border, XOR with the border, and invert
 *         to get the hole.  This did not work, because if
 *         you have a hole border that looks like:
 *
 *                x x x x x x
 *                x          x
 *                x   x x x   x
 *                  x x o x   x
 *                      x     x
 *                      x     x
 *                        x x x
 *
 *         if you 4-fill from the outside, the pixel 'o' will
 *         not be filled!  XORing with the border leaves it OFF.
 *         Inverting then gives a single bad ON pixel that is not
 *         actually part of the hole.
 *
 *  So what you must do instead is 4-fill the holes from inside.
 *  You can do this from a seedfill, using a pix with the hole
 *  border as the filling mask.  But you need to start with a
 *  pixel inside the hole.  How is this determined?  The best
 *  way is from the contour.  We have a right-hand shoulder
 *  rule for inside (i.e., the filled region).   Take the
 *  first 2 pixels of the hole border, and compute dx and dy
 *  (second coord minus first coord:  dx = sx - fx, dy = sy - fy).
 *  There are 8 possibilities, depending on the values of dx and
 *  dy (which can each be -1, 0, and +1, but not both 0).
 *  These 8 cases can be broken into 4; see the simple algorithm below.
 *  Once you have an interior seed pixel, you fill from the seed,
 *  clipping with the hole border pix by filling into its invert.
 *
 *  You then successively XOR these interior filled components, in any order.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
DisplayImage1(lua_State *L)
{
    LL_FUNC("DisplayImage1");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayImage1(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Display local chain point array for CCBorda* (%ccba) with method 2.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Uses local chain ptaa, which gives each border pixel in
 *          local coordinates, so the actual pixel positions must
 *          be computed using all offsets.
 *      (2) Treats exterior and hole borders on equivalent
 *          footing, and does all calculations on a pix
 *          that spans the c.c. with a 1 pixel added boundary.
 *      (3) This uses topological properties (Method 2) to do scan
 *          conversion to raster
 *      (4) The algorithm is described at the top of this file (Method 2).
 *          It is preferred to Method 1 because it is between 1.2x and 2x
 *          faster than Method 1.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
DisplayImage2(lua_State *L)
{
    LL_FUNC("DisplayImage2");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayImage2(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Display the single path border for CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Uses spglobal pta, which gives each border pixel in
 *          global coordinates, one path per c.c., and must
 *          be computed in advance by calling ccbaGenerateSPGlobalLocs().
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
DisplaySPBorder(lua_State *L)
{
    LL_FUNC("DisplaySPBorder");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplaySPBorder(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Generate global locations for the CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 *  Action: this uses the pixel locs in the local ptaa, which are all
 *          relative to each c.c., to find the global pixel locations,
 *          and stores them in the global ptaa.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
GenerateGlobalLocs(lua_State *L)
{
    LL_FUNC("GenerateGlobalLocs");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateGlobalLocs(ccba));
}

/**
 * \brief Generate single path global locations for the CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (ptsflag).
 *
 * Leptonica's Notes:
 *      (1) This calculates the splocal rep if not yet made.
 *      (2) It uses the local pixel values in splocal, the single
 *          path pta, which are all relative to each c.c., to find
 *          the corresponding global pixel locations, and stores
 *          them in the spglobal pta.
 *      (3) This lists only the turning points: it both makes a
 *          valid svg file and is typically about half the size
 *          when all border points are listed.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
GenerateSPGlobalLocs(lua_State *L)
{
    LL_FUNC("GenerateSPGlobalLocs");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 ptsflag = ll_check_pts_flag(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateSPGlobalLocs(ccba, ptsflag));
}

/**
 * \brief Generate a single path for CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Generates a single border in local pixel coordinates.
 *          For each c.c., if there is just an outer border, copy it.
 *          If there are also hole borders, for each hole border,
 *          determine the smallest horizontal or vertical
 *          distance from the border to the outside of the c.c.,
 *          and find a path through the c.c. for this cut.
 *          We do this in a way that guarantees a pixel from the
 *          hole border is the starting point of the path, and
 *          we must verify that the path intersects the outer
 *          border (if it intersects it, then it ends on it).
 *          One can imagine pathological cases, but they may not
 *          occur in images of text characters and un-textured
 *          line graphics.
 *      (2) Once it is verified that the path through the c.c.
 *          intersects both the hole and outer borders, we
 *          generate the full single path for all borders in the
 *          c.c.  Starting at the start point on the outer
 *          border, when we hit a line on a cut, we take
 *          the cut, do the hold border, and return on the cut
 *          to the outer border.  We compose a pta of the
 *          outer border pts that are on cut paths, and for
 *          every point on the outer border (as we go around),
 *          we check against this pta.  When we find a matching
 *          point in the pta, we do its cut path and hole border.
 *          The single path is saved in the ccb.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
GenerateSinglePath(lua_State *L)
{
    LL_FUNC("GenerateSinglePath");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateSinglePath(ccba));
}

/**
 * \brief Generate step chains for CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) This uses the pixel locs in the local ptaa,
 *          which are all relative to each c.c., to find
 *          the step directions for successive pixels in
 *          the chain, and stores them in the step numaa.
 *      (2) To get the step direction, use
 *              1   2   3
 *              0   P   4
 *              7   6   5
 *          where P is the previous pixel at (px, py).  The step direction
 *          is the number (from 0 through 7) for each relative location
 *          of the current pixel at (cx, cy).  It is easily found by
 *          indexing into a 2-d 3x3 array (dirtab).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
GenerateStepChains(lua_State *L)
{
    LL_FUNC("GenerateStepChains");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateStepChains(ccba));
}

/**
 * \brief Get the CCBord* (%ccb) at index (%index) for the CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a index (index).
 *
 * Leptonica's Notes:
 *      (1) This returns a clone of the ccb; it must be destroyed
 * </pre>
 * \param L Lua state.
 * \return 1 CCBord* on the Lua stack.
 */
static int
GetCcb(lua_State *L)
{
    LL_FUNC("GetCcb");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2, ccbaGetCount(ccba));
    CCBord *ccb = ccbaGetCcb(ccba, index);
    return ll_push_CCBord(_fun, L, ccb);
}

/**
 * \brief Read a CCBorda* (%ccba) from an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 CCBorda* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    CCBorda *ccba = ccbaRead(filename);
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Read a CCBorda* (%ccba) from a Lua stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 CCBorda* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    CCBorda *ccba = ccbaReadStream(stream->f);
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Convert step chains to pixel coordinates for CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (coordtype).
 *
 * Leptonica's Notes:
 *      (1) This uses the step chain data in each ccb to determine
 *          the pixel locations, either global or local,
 *          and stores them in the appropriate ptaa,
 *          either global or local.  For the latter, the
 *          pixel locations are relative to the c.c.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
StepChainsToPixCoords(lua_State *L)
{
    LL_FUNC("StepChainsToPixCoords");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 coordtype = ll_check_coord_type(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaStepChainsToPixCoords(ccba, coordtype));
}

/**
 * \brief Write the CCBorda* (%ccba) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaWrite(filename, ccba));
}

/**
 * \brief Write the CCBorda* (%ccba) as a SVG file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteSVG(lua_State *L)
{
    LL_FUNC("WriteSVG");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaWriteSVG(filename, ccba));
}

/**
 * \brief Write the CCBorda* (%ccba) to a file (%filename) and return a SVG string (%str).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
WriteSVGString(lua_State *L)
{
    LL_FUNC("WriteSVGString");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 2);
    const char *filename = ll_check_string(_fun, L, 1);
    char *str = ccbaWriteSVGString(ccba);
    FILE* fp;
    fp = fopen(filename, "w");
    fwrite(str, 1, strlen(str), fp);
    fclose(fp);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief Write the CCBorda* (%ccba) to a Lua stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, ccbaWriteStream(stream->f, ccba));
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class CCBorda.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data.
 */
CCBorda *
ll_check_CCBorda(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CCBorda>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a CCBorda* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data.
 */
CCBorda *
ll_opt_CCBorda(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
	return nullptr;
    return ll_check_CCBorda(_fun, L, arg);
}

/**
 * \brief Push CCBorda* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_CCBorda
 * \return 1 CCBorda* on the Lua stack.
 */
int
ll_push_CCBorda(const char *_fun, lua_State *L, CCBorda *cd)
{
    if (!cd)
	return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new CCBorda*.
 * \param L Lua state.
 * \return 1 CCBorda* on the Lua stack.
 */
int
ll_new_CCBorda(lua_State *L)
{
    FUNC("ll_new_CCBorda");
    CCBorda *ccba = nullptr;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
	Pix* pixs = ll_opt_Pix(_fun, L, 1);
	l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
	DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %d\n", _fun,
	    LL_PIX, reinterpret_cast<void *>(pixs),
	    "n", n);
	ccba = ccbaCreate(pixs, n);
    }

    if (ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
	luaL_Stream *stream = ll_check_stream(_fun, L, 1);
	DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
	    LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
	ccba = ccbaReadStream(stream->f);
    }

    if (!ccba && ll_isstring(_fun, L, 1)) {
	const char* filename = ll_check_string(_fun, L, 1);
	DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
	    "filename", filename);
	ccba = ccbaRead(filename);
    }

    if (!ccba) {
	/* FIXME: create data for no pix? */
	Pix* pix = nullptr;
	l_int32 n = 1;
	DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %d\n", _fun,
	    LL_PIX, reinterpret_cast<void *>(pix),
	    "n", n);
	ccba = ccbaCreate(pix, n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
	TNAME, reinterpret_cast<void *>(ccba));
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Register the CCBorda methods and functions in the CCBorda meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_CCBorda(lua_State *L)
{
    static const luaL_Reg methods[] = {
	{"__gc",                    Destroy},
	{"__new",                   ll_new_CCBorda},
	{"__len",                   GetCount},
	{"__tostring",              toString},
	{"AddCcb",                  AddCcb},
	{"Create",                  Create},
	{"Destroy",                 Destroy},
	{"DisplayBorder",           DisplayBorder},
	{"DisplayImage1",           DisplayImage1},
	{"DisplayImage2",           DisplayImage2},
	{"DisplaySPBorder",         DisplaySPBorder},
	{"GenerateGlobalLocs",      GenerateGlobalLocs},
	{"GenerateSPGlobalLocs",    GenerateSPGlobalLocs},
	{"GenerateSinglePath",      GenerateSinglePath},
	{"GenerateStepChains",      GenerateStepChains},
	{"GetCcb",                  GetCcb},
	{"GetCount",                GetCount},
	{"Read",                    Read},
	{"ReadStream",              ReadStream},
	{"StepChainsToPixCoords",   StepChainsToPixCoords},
	{"Write",                   Write},
	{"WriteSVG",                WriteSVG},
	{"WriteSVGString",          WriteSVGString},
	{"WriteStream",             WriteStream},
	LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_CCBorda);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
