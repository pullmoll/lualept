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
 * \file lldewarp.cpp
 * \class Dewarp
 *
 * A class to handle dewarping Pix.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DEWARP

/** Define a function's name (_fun) with prefix LL_DEWARP */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Dewarp*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dewarp *dew = ll_take_udata<Dewarp>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "dew", reinterpret_cast<void *>(dew));
    dewarpDestroy(&dew);
    return 0;
}

/**
 * \brief Printable string for a Dewarp*.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dew) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(dew));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF, "\n    dewa (parent) : " LL_DEWARPA "* %p", reinterpret_cast<void *>(dew->dewa));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    pixs          : " LL_PIX "* %p", reinterpret_cast<void *>(dew->pixs));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    sampvdispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->sampvdispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    samphdispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->samphdispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    sampydispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->sampydispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    fullvdispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->fullvdispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    fullhdispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->fullhdispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    fullydispar   : " LL_FPIX "* %p", reinterpret_cast<void *>(dew->fullydispar));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    namidys       : " LL_NUMA "* %p", reinterpret_cast<void *>(dew->namidys));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    nacurves      : " LL_NUMA "* %p", reinterpret_cast<void *>(dew->nacurves));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    w             :  %d", dew->w);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    h             :  %d", dew->h);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    pageno        :  %d", dew->pageno);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    sampling      :  %d", dew->sampling);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    redfactor     :  %d", dew->redfactor);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    minlines      :  %d", dew->minlines);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    nlines        :  %d", dew->nlines);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    mincurv       :  %d", dew->mincurv);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    maxcurv       :  %d", dew->maxcurv);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    leftslope     :  %d", dew->leftslope);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    rightslope    :  %d", dew->rightslope);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    leftcurv      :  %d", dew->leftcurv);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    rightcurv     :  %d", dew->rightcurv);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    nx            :  %d", dew->nx);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    ny            :  %d", dew->ny);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    hasref        :  %d", dew->hasref);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    refpage       :  %d", dew->refpage);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    vsuccess      :  %d", dew->vsuccess);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    hsuccess      :  %d", dew->hsuccess);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    ysuccess      :  %d", dew->ysuccess);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    vvalid        :  %d", dew->vvalid);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    hvalid        :  %d", dew->hvalid);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    skip_horiz    :  %d", dew->skip_horiz);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    debug         :  %d", dew->debug);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Build line model for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a l_int32 (opensize).
 * Arg #3 is expected to be a const (char *debugfile).
 *
 * Leptonica's Notes:
 *      (1) This builds the horizontal and vertical disparity arrays
 *          for an input of ruled lines, typically for calibration.
 *          In book scanning, you could lay the ruled paper over a page.
 *          Then for that page and several below it, you can use the
 *          disparity correction of the line model to dewarp the pages.
 *      (2) The dew has been initialized with the image of ruled lines.
 *          These lines must be continuous, but we do a small amount
 *          of pre-processing here to insure that.
 *      (3) %opensize is typically about 8.  It must be larger than
 *          the thickness of the lines to be extracted.  This is the
 *          default value, which is applied if %opensize < 3.
 *      (4) Sets vsuccess = 1 and hsuccess = 1 if the vertical and/or
 *          horizontal disparity arrays build.
 *      (5) Similar to dewarpBuildPageModel(), except here the vertical
 *          and horizontal disparity arrays are both built from ruled lines.
 *          See notes there.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
BuildLineModel(lua_State *L)
{
    LL_FUNC("BuildLineModel");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    l_int32 opensize = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpBuildLineModel(dew, opensize, nullptr));
}

/**
 * \brief Build page model for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_DEWARP* (dew).
 * Arg #2 is expected to be a const (char *debugfile).
 *
 * Leptonica's Notes:
 *      (1) This is the basic function that builds the horizontal and
 *          vertical disparity arrays, which allow determination of the
 *          src pixel in the input image corresponding to each
 *          dest pixel in the dewarped image.
 *      (2) Sets vsuccess = 1 if the vertical disparity array builds.
 *          Always attempts to build the horizontal disparity array,
 *          even if it will not be requested (useboth == 0).
 *          Sets hsuccess = 1 if horizontal disparity builds.
 *      (3) The method is as follows:
 *          (a) Estimate the points along the centers of all the
 *              long textlines.  If there are too few lines, no
 *              disparity models are built.
 *          (b) From the vertical deviation of the lines, estimate
 *              the vertical disparity.
 *          (c) From the ends of the lines, estimate the horizontal
 *              disparity, assuming that the text is made of lines
 *              that are close to left and right justified.
 *          (d) One can also compute an additional contribution to the
 *              horizontal disparity, inferred from slopes of the top
 *              and bottom lines.  We do not do this.
 *      (4) In more detail for the vertical disparity:
 *          (a) Fit a LS quadratic to center locations along each line.
 *              This smooths the curves.
 *          (b) Sample each curve at a regular interval, find the y-value
 *              of the mid-point on each curve, and subtract the sampled
 *              curve value from this value.  This is the vertical
 *              disparity at sampled points along each curve.
 *          (c) Fit a LS quadratic to each set of vertically aligned
 *              disparity samples.  This smooths the disparity values
 *              in the vertical direction.  Then resample at the same
 *              regular interval.  We now have a regular grid of smoothed
 *              vertical disparity valuels.
 *      (5) Once the sampled vertical disparity array is found, it can be
 *          interpolated to get a full resolution vertical disparity map.
 *          This can be applied directly to the src image pixels
 *          to dewarp the image in the vertical direction, making
 *          all textlines horizontal.  Likewise, the horizontal
 *          disparity array is used to left- and right-align the
 *          longest textlines.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
BuildPageModel(lua_State *L)
{
    LL_FUNC("BuildPageModel");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == dewarpBuildPageModel(dew, nullptr));
}

/**
 * \brief Create a new Dewarp*.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (pageno).
 *
 * Leptonica's Notes:
 *      (1) The input pixs is either full resolution or 2x reduced.
 *      (2) The page number is typically 0-based.  If scanned from a book,
 *          the even pages are usually on the left.  Disparity arrays
 *          built for even pages should only be applied to even pages.
 * </pre>
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpCreate(pixs, pageno);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Create reference Dewarp* (%dew).
 * <pre>
 * Arg #1 is expected to be a l_int32 (pageno).
 * Arg #2 is expected to be a l_int32 (refpage).
 *
 * Leptonica's Notes:
 *      (1) This specifies which dewarp struct should be used for
 *          the given page.  It is placed in dewarpa for pages
 *          for which no model can be built.
 *      (2) This page and the reference page have the same parity and
 *          the reference page is the closest page with a disparity model
 *          to this page.
 * </pre>
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
static int
CreateRef(lua_State *L)
{
    LL_FUNC("CreateRef");
    l_int32 pageno = ll_check_l_int32(_fun, L, 1);
    l_int32 refpage = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpCreateRef(pageno, refpage);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Find horizontal disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 *
 * Leptonica's Notes:
 *      (1) This builds a horizontal disparity model (HDM), but
 *          does not check it against constraints for validity.
 *          Constraint checking is done at rendering time.
 *      (2) Horizontal disparity is not required for a successful model;
 *          only the vertical disparity is required.  This will not be
 *          called if the function to build the vertical disparity fails.
 *      (3) This sets the hsuccess flag to 1 on success.
 *      (4) Internally in ptal1, ptar1, ptal2, ptar2: x and y are reversed,
 *          so the 'y' value is horizontal distance across the image width.
 *      (5) Debug output goes to /tmp/lept/dewmod/ for collection into a pdf.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
FindHorizDisparity(lua_State *L)
{
    LL_FUNC("FindHorizDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpFindHorizDisparity(dew, ptaa));
}

/**
 * \brief Find horizontal slope disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Pix* (pixb).
 * Arg #3 is expected to be a l_float32 (fractthresh).
 * Arg #4 is expected to be a l_int32 (parity).
 *
 * Leptonica's Notes:
 *      (1) %fractthresh is a threshold on the fractional difference in stroke
 *          density between between left and right sides.  Process this
 *          disparity only if the absolute value of the fractional
 *          difference equals or exceeds this threshold.
 *      (2) %parity indicates where the binding is: on the left for
 *          %parity == 0 and on the right for %parity == 1.
 *      (3) This takes a 1 bpp %pixb where both vertical and horizontal
 *          disparity have been applied, so the text lines are straight and,
 *          more importantly, the line end points are vertically aligned.
 *          It estimates the foreshortening of the characters on the
 *          binding side, and if significant, computes a one-dimensional
 *          horizontal disparity function to compensate.
 *      (4) The first attempt was to use the average width of the
 *          connected components (c.c.) in vertical slices.  This does not work
 *          reliably, because the horizontal compression of the text is
 *          often accompanied by horizontal joining of c.c.
 *      (5) We use the density of vertical strokes, measured by first using
 *          a vertical opening, which improves the signal.  The result
 *          is relatively insensitive to the size of the opening; we use
 *          a 10-pixel opening.  The relative density is measured by
 *          finding the number of c.c. in a full height sliding window
 *          of width 50 pixels, and compute every 25 pixels.  Similar results
 *          are obtained counting c.c. that either intersect the window
 *          or are fully contained within it.
 *      (6) Debug output goes to /tmp/lept/dewmod/ for collection into a pdf.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
FindHorizSlopeDisparity(lua_State *L)
{
    LL_FUNC("FindHorizSlopeDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Pix *pixb = ll_check_Pix(_fun, L, 2);
    l_float32 fractthresh = ll_check_l_float32(_fun, L, 3);
    l_int32 parity = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == dewarpFindHorizSlopeDisparity(dew, pixb, fractthresh, parity));
}

/**
 * \brief Find vertical disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * Arg #3 is expected to be a l_int32 (rotflag).
 *
 * Leptonica's Notes:
 *      (1) This starts with points along the centers of textlines.
 *          It does quadratic fitting (and smoothing), first along the
 *          lines and then in the vertical direction, to generate
 *          the sampled vertical disparity map.  This can then be
 *          interpolated to full resolution and used to remove
 *          the vertical line warping.
 *      (2) Use %rotflag == 1 if you are dewarping vertical lines, as
 *          is done in dewarpBuildLineModel().  The usual case is for
 *          %rotflag == 0.
 *      (3) Note that this builds a vertical disparity model (VDM), but
 *          does not check it against constraints for validity.
 *          Constraint checking is done after building the models,
 *          and before inserting reference models.
 *      (4) This sets the vsuccess flag to 1 on success.
 *      (5) Pix debug output goes to /tmp/dewvert/ for collection into
 *          a pdf.  Non-pix debug output goes to /tmp.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
FindVertDisparity(lua_State *L)
{
    LL_FUNC("FindVertDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    l_int32 rotflag = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == dewarpFindVertDisparity(dew, ptaa, rotflag));
}

/**
 * \brief Get text line centers for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a boolean (debugflag).
 *
 * Leptonica's Notes:
 *      (1) This in general does not have a point for each value
 *          of x, because there will be gaps between words.
 *          It doesn't matter because we will fit a quadratic to the
 *          points that we do have.
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
GetTextlineCenters(lua_State *L)
{
    LL_FUNC("GetTextlineCenters");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 debugflag = ll_opt_boolean(_fun, L, 2, FALSE);
    Ptaa *ptaa = dewarpGetTextlineCenters(pixs, debugflag);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Minimize Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 *
 * Leptonica's Notes:
 *      (1) This removes all data that is not needed for serialization.
 *          It keeps the subsampled disparity array(s), so the full
 *          resolution arrays can be reconstructed.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Minimize(lua_State *L)
{
    LL_FUNC("Minimize");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == dewarpMinimize(dew));
}

/**
 * \brief Populate full res for Dewarp* (%dew) using Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Pix* (pix).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 *
 * Leptonica's Notes:
 *      (1) If the full resolution vertical and horizontal disparity
 *          arrays do not exist, they are built from the subsampled ones.
 *      (2) If pixs is not given, the size of the arrays is determined
 *          by the original image from which the sampled version was
 *          generated.  Any values of (x,y) are ignored.
 *      (3) If pixs is given, the full resolution disparity arrays must
 *          be large enough to accommodate it.
 *          (a) If the arrays do not exist, the value of (x,y) determines
 *              the origin of the full resolution arrays without extension,
 *              relative to pixs.  Thus, (x,y) gives the amount of
 *              slope extension in (left, top).  The (right, bottom)
 *              extension is then determined by the size of pixs and
 *              (x,y); the values should never be < 0.
 *          (b) If the arrays exist and pixs is too large, the existing
 *              full res arrays are destroyed and new ones are made,
 *              again using (x,y) to determine the extension in the
 *              four directions.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
PopulateFullRes(lua_State *L)
{
    LL_FUNC("PopulateFullRes");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 y = ll_opt_l_int32(_fun, L, 4, 0);
    return ll_push_boolean(_fun, L, 0 == dewarpPopulateFullRes(dew, pix, x, y));
}

/**
 * \brief Read Dewarp* (%dew) from external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dewarp *dew = dewarpRead(filename);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Read Dewarp* (%dew) from a lstring (%str, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const (l_uint8 *data).
 * Arg #2 is expected to be a size_t (size).
 * </pre>
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    Dewarp *dew = dewarpReadMem(data, size);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Read Dewarp* (%dew) from luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FILE* (fp).
 *
 * Leptonica's Notes:
 *      (1) The dewarp struct is stored in minimized format, with only
 *          subsampled disparity arrays.
 *      (2) The sampling and extra horizontal disparity parameters are
 *          stored here.  During generation of the dewarp struct, they
 *          are passed in from the dewarpa.  In readback, it is assumed
 *          that they are (a) the same for each page and (b) the same
 *          as the values used to create the dewarpa.
 * </pre>
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dewarp *dew = dewarpReadStream(stream->f);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Remove short lines from Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Ptaa* (ptaas).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a boolean (debugflag).
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
RemoveShortLines(lua_State *L)
{
    LL_FUNC("RemoveShortLines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaas = ll_check_Ptaa(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 debugflag = ll_opt_boolean(_fun, L, 4, FALSE);
    Ptaa *ptaa = dewarpRemoveShortLines(pixs, ptaas, fract, debugflag);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Write Dewarp* (%dew) to external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpWrite(filename, dew));
}

/**
 * \brief Write Dewarp* (%dew) to a lstring (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 *
 * Leptonica's Notes:
 *      (1) Serializes a dewarp in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 l_int32 on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (dewarpWriteMem(&data, &size, dew))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write Dewarp* (%dew) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) This should not be written if there is no sampled
 *          vertical disparity array, which means that no model has
 *          been built for this page.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpWriteStream(stream->f, dew));
}

/**
 * \brief Check Lua stack at index %arg for user data of class LL_DEWARP.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarp* contained in the user data.
 */
Dewarp *
ll_check_Dewarp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dewarp>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DEWARP at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarp* contained in the user data.
 */
Dewarp *
ll_opt_Dewarp(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Dewarp(_fun, L, arg);
}

/**
 * \brief Push Dewarp* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param dew pointer to the Dewarp
 * \return 1 Dewarp* on the Lua stack.
 */
int
ll_push_Dewarp(const char *_fun, lua_State *L, Dewarp *dew)
{
    if (!dew)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, dew);
}

/**
 * \brief Create and push a new Dewarp*.
 * \param L Lua state.
 * \return 1 Dewarp* on the Lua stack.
 */
int
ll_new_Dewarp(lua_State *L)
{
    FUNC("ll_new_Dewarp");
    Dewarp *dew = nullptr;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        Pix *pixs = ll_opt_Pix(_fun, L, 1);
        l_int32 pageno = ll_opt_l_int32(_fun, L, 2, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        dew = dewarpCreate(pixs, pageno);
    }

    if (!dew && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        dew = dewarpReadStream(stream->f);
    }

    if (!dew && ll_isinteger(_fun, L, 1) && ll_isinteger(_fun, L, 2)) {
        l_int32 pageno = ll_check_l_int32(_fun, L, 1);
        l_int32 refpage = ll_check_l_int32(_fun, L, 2);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "pageno", pageno, "repage", refpage);
        dew = dewarpCreateRef(pageno, refpage);
    }

    if (!dew && ll_isstring(_fun, L, 1)) {
        const char *filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        dew = dewarpRead(filename);
    }

    if (!dew && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        dew = dewarpReadMem(data, size);
    }

    if (!dew) {
        /* FIXME: creat from null Pix* ? */
        Pix *pixs = nullptr;
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        dew = dewarpCreate(pixs, 1);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(dew));
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Register the Dewarp* methods and functions in the LL_DEWARP meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Dewarp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Dewarp},
        {"__tostring",              toString},
        {"BuildLineModel",          BuildLineModel},
        {"BuildPageModel",          BuildPageModel},
        {"Create",                  Create},
        {"CreateRef",               CreateRef},
        {"Destroy",                 Destroy},
        {"FindHorizDisparity",      FindHorizDisparity},
        {"FindHorizSlopeDisparity", FindHorizSlopeDisparity},
        {"FindVertDisparity",       FindVertDisparity},
        {"GetTextlineCenters",      GetTextlineCenters},
        {"Minimize",                Minimize},
        {"PopulateFullRes",         PopulateFullRes},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveShortLines",        RemoveShortLines},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Dewarp);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
