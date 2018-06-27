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
 * \file llpixa.cpp
 * \class Pixa
 *
 * An array of Pix.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXA

/** Define a function's name (_fun) with prefix Pixa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Pixa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the pixa.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pixa *pixa = ll_take_udata<Pixa>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "pixa", reinterpret_cast<void *>(pixa),
        "count", pixaGetCount(pixa));
    pixaDestroy(&pixa);
    return 0;
}

/**
 * \brief Get count for a Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    ll_push_l_int32(_fun, L, pixaGetCount(pixa));
    return 1;
}

/**
 * \brief Printable string for a Pixa*.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!pixa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(pixa));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "n", pixa->n);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "nalloc", pixa->nalloc);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "refcount", pixa->refcount);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s** %p",
                 "pix", LL_PIX, reinterpret_cast<void *>(pixa->pix));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s* %p",
                 "boxa", LL_BOXA, reinterpret_cast<void *>(pixa->boxa));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief AccumulateSamples() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a Pta* (pta).
 *
 * Leptonica's Notes:
 *      (1) This generates an aligned (by centroid) sum of the input pix.
 *      (2) We use only the first 256 samples; that's plenty.
 *      (3) If pta is not input, we generate two tables, and discard
 *          after use.  If this is called many times, it is better
 *          to precompute the pta.
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 on the Lua stack
 */
static int
AccumulateSamples(lua_State *L)
{
    LL_FUNC("AccumulateSamples");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    Pix *pixd = nullptr;
    l_float32 x = 0;
    l_float32 y = 0;
    pixaAccumulateSamples(pixa, pta, &pixd, &x, &y);
    ll_push_Pix(_fun, L, pixd);
    ll_push_l_float32(_fun, L, x);
    ll_push_l_float32(_fun, L, y);
    return 3;
}

/**
 * \brief AddBorderGeneral() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixad).
 * Arg #2 is expected to be a Pixa* (pixas).
 * Arg #3 is expected to be a l_int32 (left).
 * Arg #4 is expected to be a l_int32 (right).
 * Arg #5 is expected to be a l_int32 (top).
 * Arg #6 is expected to be a l_int32 (bot).
 * Arg #7 is expected to be a l_uint32 (val).
 *
 * Leptonica's Notes:
 *      (1) For binary images:
 *             white:  val = 0
 *             black:  val = 1
 *          For grayscale images:
 *             white:  val = 2 ** d - 1
 *             black:  val = 0
 *          For rgb color images:
 *             white:  val = 0xffffff00
 *             black:  val = 0
 *          For colormapped images, use 'index' found this way:
 *             white: pixcmapGetRankIntensity(cmap, 1.0, &index);
 *             black: pixcmapGetRankIntensity(cmap, 0.0, &index);
 *      (2) For in-place replacement of each pix with a bordered version,
 *          use %pixad = %pixas.  To make a new pixa, use %pixad = NULL.
 *      (3) In both cases, the boxa has sides adjusted as if it were
 *          expanded by the border.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
AddBorderGeneral(lua_State *L)
{
    LL_FUNC("AddBorderGeneral");
    Pixa *pixad = ll_check_Pixa(_fun, L, 1);
    Pixa *pixas = ll_check_Pixa(_fun, L, 2);
    l_int32 left = ll_check_l_int32(_fun, L, 3);
    l_int32 right = ll_check_l_int32(_fun, L, 4);
    l_int32 top = ll_check_l_int32(_fun, L, 5);
    l_int32 bot = ll_check_l_int32(_fun, L, 6);
    l_uint32 val = ll_check_l_uint32(_fun, L, 7);
    Pixa *pixa = pixaAddBorderGeneral(pixad, pixas, left, right, top, bot, val);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief AddBox() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (copyflag).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    LL_FUNC("AddBox");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3);
    l_ok ok = pixaAddBox(pixa, box, copyflag);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Add a Pix* (%pix) to a Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a Pix*.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddPix(lua_State *L)
{
    LL_FUNC("AddPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == pixaAddPix(pixa, pix, flag));
}

/**
 * \brief AddPixWithText() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a Bmf* (bmf).
 * Arg #5 is expected to be a const char* (textstr).
 * Arg #6 is expected to be a l_uint32 (val).
 * Arg #7 is expected to be a string describing the location (location).
 *
 * Leptonica's Notes:
 *      (1) This function generates a new pix with added text, and adds
 *          it by insertion into the pixa.
 *      (2) If the input pixs is not cmapped and not 32 bpp, it is
 *          converted to 32 bpp rgb.  %val is a standard 32 bpp pixel,
 *          expressed as 0xrrggbb00.  If there is a colormap, this does
 *          the best it can to use the requested color, or something close.
 *      (3) if %bmf == NULL, generate an 8 pt font; this takes about 5 msec.
 *      (4) If %textstr == NULL, use the text field in the pix.
 *      (5) In general, the text string can be written in multiple lines;
 *          use newlines as the separators.
 *      (6) Typical usage is for debugging, where the pixa of labeled images
 *          is used to generate a pdf.  Suggest using 1.0 for scalefactor.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
AddPixWithText(lua_State *L)
{
    LL_FUNC("AddPixWithText");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    Bmf *bmf = ll_opt_Bmf(_fun, L, 4);
    const char *textstr = ll_opt_string(_fun, L, 5);
    l_uint32 val = ll_opt_l_uint32(_fun, L, 6);
    l_int32 location = ll_check_location(_fun, L, 7);
    l_ok ok = pixaAddPixWithText(pixa, pixs, reduction, bmf, textstr, val, location);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief AddTextNumber() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a Numa* (na).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a l_int32 (location).
 *
 * Leptonica's Notes:
 *      (1) Typical usage is for labelling each pix in a pixa with a number.
 *      (2) This function paints numbers external to each pix, in a position
 *          given by %location.  In all cases, the pix is expanded on
 *          on side and the number is painted over white in the added region.
 *      (3) %val is the pixel value to be painted through the font mask.
 *          It should be chosen to agree with the depth of pixs.
 *          If it is out of bounds, an intermediate value is chosen.
 *          For RGB, use hex notation: 0xRRGGBB00, where RR is the
 *          hex representation of the red intensity, etc.
 *      (4) If na == NULL, number each pix sequentially, starting with 1.
 *      (5) If there is a colormap, this does the best it can to use
 *          the requested color, or something similar to it.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
AddTextNumber(lua_State *L)
{
    LL_FUNC("AddTextNumber");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Bmf *bmf = ll_opt_Bmf(_fun, L, 2);
    Numa *na = ll_opt_Numa(_fun, L, 3);
    l_uint32 val = ll_opt_l_uint32(_fun, L, 4);
    l_int32 location = ll_check_location(_fun, L, 5);
    Pixa *pixa = pixaAddTextNumber(pixas, bmf, na, val, location);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief AddTextlines() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a Sarray* (sa).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a string describing the location (location).
 *
 * Leptonica's Notes:
 *      (1) This function adds one or more lines of text externally to
 *          each pix, in a position given by %location.  In all cases,
 *          the pix is expanded as necessary to accommodate the text.
 *      (2) %val is the pixel value to be painted through the font mask.
 *          It should be chosen to agree with the depth of pixs.
 *          If it is out of bounds, an intermediate value is chosen.
 *          For RGB, use hex notation: 0xRRGGBB00, where RR is the
 *          hex representation of the red intensity, etc.
 *      (3) If sa == NULL, use the text embedded in each pix.  In all
 *          cases, newlines in the text string are used to separate the
 *          lines of text that are added to the pix.
 *      (4) If sa has a smaller count than pixa, issue a warning
 *          and do not use any embedded text.
 *      (5) If there is a colormap, this does the best it can to use
 *          the requested color, or something similar to it.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
AddTextlines(lua_State *L)
{
    LL_FUNC("AddTextlines");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Bmf *bmf = ll_opt_Bmf(_fun, L, 2);
    Sarray *sa = lua_isnil(L, 3) ? nullptr : ll_unpack_Sarray(_fun, L, 3, nullptr);
    l_uint32 val = ll_opt_l_uint32(_fun, L, 4);
    l_int32 location = ll_check_location(_fun, L, 5);
    Pixa *pixa = pixaAddTextlines(pixas, bmf, sa, val, location);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Check if any of the Pix* in Pixa* (%pixa) have a colormap.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AnyColormaps(lua_State *L)
{
    LL_FUNC("AnyColormaps");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 hascmap = 0;
    if (pixaAnyColormaps(pixa, &hascmap))
        return ll_push_nil(_fun, L);
    ll_push_boolean(_fun, L, hascmap);
    return 1;
}

/**
 * \brief BinSort() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a l_int32 (sorttype).
 * Arg #3 is expected to be a l_int32 (sortorder).
 * Arg #4 is expected to be a l_int32 (copyflag).
 *
 * Leptonica's Notes:
 *      (1) This sorts based on the data in the boxa.  If the boxa
 *          count is not the same as the pixa count, this returns an error.
 *      (2) The copyflag refers to the pix and box copies that are
 *          inserted into the sorted pixa.  These are either L_COPY
 *          or L_CLONE.
 *      (3) For a large number of boxes (say, greater than 1000), this
 *          O(n) binsort is much faster than the O(nlogn) shellsort.
 *          For 5000 components, this is over 20x faster than boxaSort().
 *      (4) Consequently, pixaSort() calls this function if it will
 *          likely go much faster.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
BinSort(lua_State *L)
{
    LL_FUNC("BinSort");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 sorttype = ll_check_sort_mode(_fun, L, 2);
    l_int32 sortorder = ll_check_sort_order(_fun, L, 3);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 4);
    Numa *naindex = nullptr;
    Pixa *pixad = pixaBinSort(pixas, sorttype, sortorder, &naindex, copyflag);
    ll_push_Pixa(_fun, L, pixad);
    ll_push_Numa(_fun, L, naindex);
    return 2;
}

/**
 * \brief Centroids() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) An error message is returned if any pix has something other
 *          than 1 bpp or 8 bpp depth, and the centroid from that pix
 *          is saved as (0, 0).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Centroids(lua_State *L)
{
    LL_FUNC("Centroids");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pta *pta = pixaCentroids(pixa);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief ChangeRefcount() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (delta).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    l_ok ok = pixaChangeRefcount(pixa, delta);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Clear the Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) This destroys all pix in the pixa, as well as
 *          all boxes in the boxa.  The ptrs in the pix ptr array
 *          are all null'd.  The number of allocated pix, n, is set to 0.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixaClear(pixa));
}

/**
 * \brief Clip Pix* in the Pixa* (%pixas) to their foreground.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 *
 * Leptonica's Notes:
 *      (1) At least one of [&pixd, &boxa] must be specified.
 *      (2) Any pix with no fg pixels is skipped.
 *      (3) See pixClipToForeground().
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Pixa* (%pixad) and Boxa* (%boxa) on the Lua stack
 */
static int
ClipToForeground(lua_State *L)
{
    LL_FUNC("ClipToForeground");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Pixa *pixad = nullptr;
    Boxa *boxa = nullptr;
    if (pixaClipToForeground(pixas, &pixad, &boxa))
        return ll_push_nil(_fun, L);
    ll_push_Pixa(_fun, L, pixad);
    ll_push_Boxa(_fun, L, boxa);
    return 2;
}

/**
 * \brief ClipToPix() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a Pix* (pixs).
 *
 * Leptonica's Notes:
 *      (1) This is intended for use in situations where pixas
 *          was originally generated from the input pixs.
 *      (2) Returns a pixad where each pix in pixas is ANDed
 *          with its associated region of the input pixs.  This
 *          region is specified by the the box that is associated
 *          with the pix.
 *      (3) In a typical application of this function, pixas has
 *          a set of region masks, so this generates a pixa of
 *          the parts of pixs that correspond to each region
 *          mask component, along with the bounding box for
 *          the region.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ClipToPix(lua_State *L)
{
    LL_FUNC("ClipToPix");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pixa *pixad = pixaClipToPix(pixas, pixs);
    return ll_push_Pixa(_fun, L, pixad);
}

/**
 * \brief CompareInPdf() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa1).
 * Arg #2 is expected to be a Pixa* (pixa2).
 * Arg #3 is expected to be a l_int32 (nx).
 * Arg #4 is expected to be a l_int32 (ny).
 * Arg #5 is expected to be a l_int32 (tw).
 * Arg #6 is expected to be a l_int32 (spacing).
 * Arg #7 is expected to be a l_int32 (border).
 * Arg #8 is expected to be a l_int32 (fontsize).
 * Arg #9 is expected to be a const char* (fileout).
 *
 * Leptonica's Notes:
 *      (1) This takes two pixa and renders them interleaved, side-by-side
 *          in a pdf.  A warning is issued if the input pixa arrays
 *          have different lengths.
 *      (2) %nx and %ny specify how many side-by-side pairs are displayed
 *          on each pdf page.  For example, if %nx = 1 and %ny = 2, then
 *          two pairs are shown, one above the other, on each page.
 *      (3) The input pix are scaled to a target width of %tw, and
 *          then paired with optional %spacing between and optional
 *          black border of width %border.
 *      (4) After a pixa is generated of these tiled images, it is
 *          written to %fileout as a pdf.
 *      (5) Typical numbers for the input parameters are:
 *            %nx = small integer (1 - 4)
 *            %ny = 2 * %nx
 *            %tw = 200 - 500 pixels
 *            %spacing = 10
 *            %border = 2
 *            %fontsize = 10
 *      (6) If %fontsize != 0, the index of the pix pair in their pixa
 *          is printed out below each pair.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
CompareInPdf(lua_State *L)
{
    LL_FUNC("CompareInPdf");
    Pixa *pixa1 = ll_check_Pixa(_fun, L, 1);
    Pixa *pixa2 = ll_check_Pixa(_fun, L, 2);
    l_int32 nx = ll_opt_l_int32(_fun, L, 3, 2);
    l_int32 ny = ll_opt_l_int32(_fun, L, 4, 4);
    l_int32 tw = ll_opt_l_int32(_fun, L, 5, 100);
    l_int32 spacing = ll_opt_l_int32(_fun, L, 6, 10);
    l_int32 border = ll_opt_l_int32(_fun, L, 7, 2);
    l_int32 fontsize = ll_opt_l_int32(_fun, L, 8);
    const char *fileout = ll_check_string(_fun, L, 9);
    l_ok ok = pixaCompareInPdf(pixa1, pixa2, nx, ny, tw, spacing, border, fontsize, fileout);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief ComparePhotoRegionsByHisto() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_float32 (minratio).
 * Arg #3 is expected to be a l_float32 (textthresh).
 * Arg #4 is expected to be a l_int32 (factor).
 * Arg #5 is expected to be a l_int32 (nx).
 * Arg #6 is expected to be a l_int32 (ny).
 * Arg #7 is expected to be a l_float32 (simthresh).
 *
 * Leptonica's Notes:
 *      (1) This function takes a pixa of cropped photo images and
 *          compares each one to the others for similarity.
 *          Each image is first tested to see if it is a photo that can
 *          be compared by tiled histograms.  If so, it is padded to put
 *          the centroid in the center of the image, and the histograms
 *          are generated.  The final step of comparing each histogram
 *          with all the others is very fast.
 *      (2) An initial filter gives %score = 0 if the ratio of widths
 *          and heights (smallest / largest) does not exceed a
 *          threshold %minratio.  If set at 1.0, both images must be
 *          exactly the same size.  A typical value for %minratio is 0.9.
 *      (3) The comparison score between two images is a value in [0.0 .. 1.0].
 *          If the comparison score >= %simthresh, the images are placed in
 *          the same similarity class.  Default value for %simthresh is 0.25.
 *      (4) An array %nai of similarity class indices for pix in the
 *          input pixa is returned.
 *      (5) There are two debugging options:
 *          * An optional 2D matrix of scores is returned as a 1D array.
 *            A visualization of this is written to a temp file.
 *          * An optional pix showing the similarity classes can be
 *            returned.  Text in each input pix is reproduced.
 *      (6) See the notes in pixComparePhotoRegionsByHisto() for details
 *          on the implementation.
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 on the Lua stack
 */
static int
ComparePhotoRegionsByHisto(lua_State *L)
{
    LL_FUNC("ComparePhotoRegionsByHisto");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_float32 minratio = ll_check_l_float32(_fun, L, 2);
    l_float32 textthresh = ll_check_l_float32(_fun, L, 3);
    l_int32 factor = ll_check_l_int32(_fun, L, 4);
    l_int32 nx = ll_check_l_int32(_fun, L, 5);
    l_int32 ny = ll_check_l_int32(_fun, L, 6);
    l_float32 simthresh = ll_check_l_float32(_fun, L, 7);
    l_int32 debug = ll_opt_boolean(_fun, L, 8);
    Numa *nai = nullptr;
    Pix *pixd = nullptr;
    if (pixaComparePhotoRegionsByHisto(pixa, minratio, textthresh, factor, nx, ny, simthresh, &nai, nullptr, &pixd, debug))
        return ll_push_nil(_fun, L);
    ll_push_Numa(_fun, L, nai);
    ll_push_Pix(_fun, L, pixd);
    return 3;
}

/**
 * \brief ConstrainedSelect() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a l_int32 (first).
 * Arg #3 is expected to be a l_int32 (last).
 * Arg #4 is expected to be a l_int32 (nmax).
 * Arg #5 is expected to be a boolean (use_pairs).
 * Arg #6 is expected to be a string describing the copy mode (copyflag).
 *
 * Leptonica's Notes:
 *     (1) See notes in genConstrainedNumaInRange() for how selection
 *         is made.
 *     (2) This returns a selection of the pix in the input pixa.
 *     (3) Use copyflag == L_COPY if you don't want changes in the pix
 *         in the returned pixa to affect those in the input pixa.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConstrainedSelect(lua_State *L)
{
    LL_FUNC("ConstrainedSelect");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 first = ll_check_l_int32(_fun, L, 2);
    l_int32 last = ll_check_l_int32(_fun, L, 3);
    l_int32 nmax = ll_check_l_int32(_fun, L, 4);
    l_int32 use_pairs = ll_opt_boolean(_fun, L, 5);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 6, L_COPY);
    Pixa *pixa = pixaConstrainedSelect(pixas, first, last, nmax, use_pairs, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Convert all Pix* in Pixa* (%pixas) to 1 bpp.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a l_int32 (thresh).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* (%pixa) on the Lua stack
 */
static int
ConvertTo1(lua_State *L)
{
    LL_FUNC("ConvertTo1");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    Pixa *pixa = pixaConvertTo1(pixas, thresh);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Convert all Pix* in Pixa* (%pixas) to 8 bpp.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a boolean (cmapflag).
 *
 * Leptonica's Notes:
 *      (1) See notes for pixConvertTo8(), applied to each pix in pixas.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* (%pixa) on the Lua stack
 */
static int
ConvertTo8(lua_State *L)
{
    LL_FUNC("ConvertTo8");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 cmapflag = ll_opt_boolean(_fun, L, 2);
    Pixa *pixa = pixaConvertTo8(pixas, cmapflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief ConvertTo8Colormap() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a boolean (dither).
 *
 * Leptonica's Notes:
 *      (1) See notes for pixConvertTo8Colormap(), applied to each pix in pixas.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertTo8Colormap(lua_State *L)
{
    LL_FUNC("ConvertTo8Colormap");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 dither = ll_opt_boolean(_fun, L, 2);
    Pixa *pixa = pixaConvertTo8Colormap(pixas, dither);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Convert all Pix* in Pixa* (%pixas) to 32 bpp.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 *
 * Leptonica's Notes:
 *      (1) See notes for pixConvertTo32(), applied to each pix in pixas.
 *      (2) This can be used to allow 1 bpp pix in a pixa to be displayed
 *          with color.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertTo32(lua_State *L)
{
    LL_FUNC("ConvertTo32");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Pixa *pixa = pixaConvertTo32(pixas);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief ConvertToNUpPixa() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 * Arg #2 is expected to be a Sarray* (sa).
 * Arg #3 is expected to be a l_int32 (nx).
 * Arg #4 is expected to be a l_int32 (ny).
 * Arg #5 is expected to be a l_int32 (tw).
 * Arg #6 is expected to be a l_int32 (spacing).
 * Arg #7 is expected to be a l_int32 (border).
 * Arg #8 is expected to be a l_int32 (fontsize).
 *
 * Leptonica's Notes:
 *      (1) This takes an input pixa and an optional array of strings, and
 *          generates a pixa of NUp tiles from the input, labeled with
 *          the strings if they exist and %fontsize != 0.
 *      (2) See notes for convertToNUpFiles()
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertToNUpPixa(lua_State *L)
{
    LL_FUNC("ConvertToNUpPixa");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Sarray *sa = lua_isnil(L, 2) ? nullptr : ll_unpack_Sarray(_fun, L, 2, nullptr);
    l_int32 nx = ll_opt_l_int32(_fun, L, 3, 2);
    l_int32 ny = ll_opt_l_int32(_fun, L, 4, 4);
    l_int32 tw = ll_opt_l_int32(_fun, L, 5, 100);
    l_int32 spacing = ll_opt_l_int32(_fun, L, 6, 20);
    l_int32 border = ll_opt_l_int32(_fun, L, 7, 2);
    l_int32 fontsize = ll_opt_l_int32(_fun, L, 8);
    Pixa *pixa = pixaConvertToNUpPixa(pixas, sa, nx, ny, tw, spacing, border, fontsize);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief ConvertToPdf() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a l_float32 (scalefactor).
 * Arg #4 is expected to be a l_int32 (type).
 * Arg #5 is expected to be a l_int32 (quality).
 * Arg #6 is expected to be a const char* (title).
 * Arg #7 is expected to be a const char* (fileout).
 *
 * Leptonica's Notes:
 *      (1) The images are encoded with G4 if 1 bpp; JPEG if 8 bpp without
 *          colormap and many colors, or 32 bpp; FLATE for anything else.
 *      (2) The scalefactor must be > 0.0; otherwise it is set to 1.0.
 *      (3) Specifying one of the three encoding types for %type forces
 *          all images to be compressed with that type.  Use 0 to have
 *          the type determined for each image based on depth and whether
 *          or not it has a colormap.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ConvertToPdf(lua_State *L)
{
    LL_FUNC("ConvertToPdf");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 3);
    l_int32 type = ll_check_encoding(_fun, L, 4);
    l_int32 quality = ll_check_l_int32(_fun, L, 5);
    const char *title = ll_check_string(_fun, L, 6);
    const char *fileout = ll_check_string(_fun, L, 7);
    l_ok ok = pixaConvertToPdf(pixas, res, scalefactor, type, quality, title, fileout);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief ConvertToPdfData() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a l_float32 (scalefactor).
 * Arg #4 is expected to be a l_int32 (type).
 * Arg #5 is expected to be a l_int32 (quality).
 * Arg #6 is expected to be a const char* (title).
 *
 * Leptonica's Notes:
 *      (1) See pixaConvertToPdf().
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
ConvertToPdfData(lua_State *L)
{
    LL_FUNC("ConvertToPdfData");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    l_int32 quality = ll_check_l_int32(_fun, L, 5);
    const char *title = ll_check_string(_fun, L, 6);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (pixaConvertToPdfData(pixa, res, scalefactor, type, quality, title, &data, &nbytes))
        return ll_push_nil(_fun, L);
    ll_push_bytes(_fun, L, data, nbytes);
    return 1;
}

/**
 * \brief ConvertToSameDepth() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixas).
 *
 * Leptonica's Notes:
 *      (1) If any pix has a colormap, they are all converted to rgb.
 *          Otherwise, they are all converted to the maximum depth of
 *          all the pix.
 *      (2) This can be used to allow lossless rendering onto a single pix.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertToSameDepth(lua_State *L)
{
    LL_FUNC("ConvertToSameDepth");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    Pixa *pixad = pixaConvertToSameDepth(pixas);
    return ll_push_Pixa(_fun, L, pixad);
}

/**
 * \brief Copy a Pixa* (%pixas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is an optional string defining the storage flags (copy, clone, copy_clone).
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Pixa *pixa = pixaCopy(pixas, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief CountPixels() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CountPixels(lua_State *L)
{
    LL_FUNC("CountPixels");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Numa *na = pixaCountPixels(pixa);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief CountText() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) All pix have non-empty text strings if the returned value %ntext
 *          equals the pixa count.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CountText(lua_State *L)
{
    LL_FUNC("CountText");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 ntext = 0;
    if (pixaCountText(pixa, &ntext))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, ntext);
    return 1;
}

/**
 * \brief Create a new Pixa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) This creates an empty boxa.
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Pixa *pixa = pixaCreate(n);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief CreateFromBoxa() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 *
 * Leptonica's Notes:
 *      (1) This simply extracts from pixs the region corresponding to each
 *          box in the boxa.
 *      (2) The 3rd arg is optional.  If the extent of the boxa exceeds the
 *          size of the pixa, so that some boxes are either clipped
 *          or entirely outside the pix, a warning is returned as TRUE.
 *      (3) pixad will have only the properly clipped elements, and
 *          the internal boxa will be correct.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
CreateFromBoxa(lua_State *L)
{
    LL_FUNC("CreateFromBoxa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 cropwarn = 0;
    Pixa* pixa = pixaCreateFromBoxa(pixs, boxa, &cropwarn);
    ll_push_l_int32(_fun, L, cropwarn);
    return 2;
}

/**
 * \brief CreateFromPix() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (n).
 * Arg #3 is expected to be a l_int32 (cellw).
 * Arg #4 is expected to be a l_int32 (cellh).
 *
 * Leptonica's Notes:
 *      (1) For bpp = 1, we truncate each retrieved pix to the ON
 *          pixels, which we assume for now start at (0,0)
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    l_int32 cellw = ll_opt_l_int32(_fun, L, 3, pixGetWidth(pixs));
    l_int32 cellh = ll_opt_l_int32(_fun, L, 4, pixGetHeight(pixs));
    Pixa *pixa = pixaCreateFromPix(pixs, n, cellw, cellh);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief CreateFromPixacomp() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixac).
 * Arg #2 is expected to be a l_int32 (accesstype).
 *
 * Leptonica's Notes:
 *      (1) Because the pixa has no notion of offset, the offset must
 *          be set to 0 before the conversion, so that pixacompGetPix()
 *          fetches all the pixcomps.  It is reset at the end.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
CreateFromPixacomp(lua_State *L)
{
    LL_FUNC("CreateFromPixacomp");
    PixaComp *pixac = ll_check_PixaComp(_fun, L, 1);
    l_int32 accesstype = ll_check_access_storage(_fun, L, 2, L_COPY);
    Pixa *pixa = pixaCreateFromPixacomp(pixac, accesstype);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Display() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 *
 * Leptonica's Notes:
 *      (1) This uses the boxes to place each pix in the rendered composite.
 *      (2) Set w = h = 0 to use the b.b. of the components to determine
 *          the size of the returned pix.
 *      (3) Uses the first pix in pixa to determine the depth.
 *      (4) The background is written "white".  On 1 bpp, each successive
 *          pix is "painted" (adding foreground), whereas for grayscale
 *          or color each successive pix is blitted with just the src.
 *      (5) If the pixa is empty, returns an empty 1 bpp pix.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Display(lua_State *L)
{
    LL_FUNC("Display");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixaDisplay(pixa, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get pixel aligned statistics for Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) Each pixel in the returned pix represents an average
 *          (or median, or mode) over the corresponding pixels in each
 *          pix in the pixa.
 *      (2) The %thresh parameter works with L_MODE_VAL only, and
 *          sets a minimum occupancy of the mode bin.
 *          If the occupancy of the mode bin is less than %thresh, the
 *          mode value is returned as 0.  To always return the actual
 *          mode value, set %thresh = 0.  See pixGetRowStats().
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
GetAlignedStats(lua_State *L)
{
    LL_FUNC("GetAlignedStats");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 nbins = ll_opt_l_int32(_fun, L, 3, 2);
    l_int32 thresh = ll_opt_l_int32(_fun, L, 4, 0);
    Pix *pix = pixaGetAlignedStats(pixa, type, nbins, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get box geometry for a Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 4 integers on the Lua stack: x, y, w, h.
 */
static int
GetBoxGeometry(lua_State *L)
{
    LL_FUNC("GetBoxGeometry");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    l_int32 x, y, w, h;
    if (pixaGetBoxGeometry(pixa, idx, &x, &y, &w, &h))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 4;
}

/**
 * \brief Get box geometry for a Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 4 integers on the Lua stack: x, y, w, h.
 */
static int
GetPix(lua_State *L)
{
    LL_FUNC("GetPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pix *pix = pixaGetPix(pixa, idx, copyflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Insert the Pix* (%pixs) in a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (%pixs).
 * Arg #4 is an optional Box* (%boxs).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i + 1] for all i >= index,
 *          and then inserts at pixa[index].
 *      (2) To insert at the beginning of the array, set index = 0.
 *      (3) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (4) To append a pix to a pixa, it's easier to use pixaAddPix().
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
InsertPix(lua_State *L)
{
    LL_FUNC("InsertPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_opt_Box(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaInsertPix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Interleave two Pixa* (%pixa1, %pixa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa1).
 * Arg #2 is expected to be another Pixa* (pixa2).
 * Arg #3 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * Leptonica's Notes:
 *      (1) %copyflag determines if the pix are copied or cloned.
 *          The boxes, if they exist, are copied.
 *      (2) If the two pixa have different sizes, a warning is issued,
 *          and the number of pairs returned is the minimum size.
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
Interleave(lua_State *L)
{
    LL_FUNC("Interleave");
    Pixa *pixa1 = ll_check_Pixa(_fun, L, 1);
    Pixa *pixa2 = ll_check_Pixa(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pixa *pixa = pixaInterleave(pixa1, pixa2, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Join Pixa* (%pixas) to Pixa* (%pixad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixad).
 * Arg #2 is expected to be another Pixa* (pixas).
 *
 * Leptonica's Notes:
 *      (1) This appends a clone of each indicated pix in pixas to pixad
 *      (2) istart < 0 is taken to mean 'read from the start' (istart = 0)
 *      (3) iend < 0 means 'read to the end'
 *      (4) If pixas is NULL or contains no pix, this is a no-op.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Pixa *pixad = ll_check_Pixa(_fun, L, 1);
    Pixa *pixas = ll_check_Pixa(_fun, L, 2);
    l_int32 istart = ll_opt_l_int32(_fun, L, 3, 1);
    l_int32 iend = ll_opt_l_int32(_fun, L, 3, pixaGetCount(pixas));
    return ll_push_boolean(_fun, L, 0 == pixaJoin(pixad, pixas, istart, iend));
}

/**
 * \brief Read a Pixa* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Pixa *pixa = pixaRead(filename);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief ReadBarcodes() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (format).
 * Arg #3 is expected to be a l_int32 (method).
 * Arg #4 is expected to be a l_int32 (debugflag).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
ReadBarcodes(lua_State *L)
{
    LL_FUNC("ReadBarcodes");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 format = ll_check_l_int32(_fun, L, 2);
    l_int32 method = ll_check_l_int32(_fun, L, 3);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 4);
    Sarray *saw = nullptr;
    Sarray *sa = pixReadBarcodes(pixa, format, method, &saw, debugflag);
    ll_push_Sarray(_fun, L, sa);
    ll_push_Sarray(_fun, L, saw);
    return 2;
}

/**
 * \brief Read a Pixa* (%pixa) from a number of external files.
 * <pre>
 * Arg #1 is expected to be a string containing the directory (dirname).
 * Arg #2 is expected to be a string (substr).
 *
 * Leptonica's Notes:
 *      (1) %dirname is the full path for the directory.
 *      (2) %substr is the part of the file name (excluding
 *          the directory) that is to be matched.  All matching
 *          filenames are read into the Pixa.  If substr is NULL,
 *          all filenames are read into the Pixa.
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
ReadFiles(lua_State *L)
{
    LL_FUNC("ReadFiles");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = ll_check_string(_fun, L, 2);
    Pixa *pixa = pixaReadFiles(dirname, substr);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pixa *pixa = pixaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pixa *pixa = pixaReadStream(stream->f);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Remove the Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (3) The corresponding box is removed as well, if it exists.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
RemovePix(lua_State *L)
{
    LL_FUNC("RemovePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    return ll_push_boolean(_fun, L, 0 == pixaRemovePix(pixa, idx));
}

/**
 * \brief Remove the Pix* (%pix) from a Pixa* (%pixa) at index (%idx) and return it and its box.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (%idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (3) The corresponding box is removed as well, if it exists.
 *      (4) The removed pix and box can either be retained or destroyed.
 * </pre>
 * \param L Lua state.
 * \return 2 Pix* (pix) and one Box* (box) on the Lua stack.
 */
static int
RemovePixAndSave(lua_State *L)
{
    LL_FUNC("RemovePixAndSave");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pix = nullptr;
    Box *box = nullptr;
    if (pixaRemovePixAndSave(pixa, idx, &pix, &box))
        return ll_push_nil(_fun, L);
    return ll_push_Pix(_fun, L, pix) + ll_push_Box(_fun, L, box);
}

/**
 * \brief Replace the Pix* in a Pixa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is an optional Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) In-place replacement of one pix.
 *      (2) The previous pix at that location is destroyed.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReplacePix(lua_State *L)
{
    LL_FUNC("ReplacePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_opt_Box(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    l_ok ok = pixaReplacePix(pixa, idx, pix, box);
    lua_pushboolean(L, pix && 0 == ok);
    return 1;
}

/**
 * \brief TemplatesFromComposites() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixac).
 * Arg #2 is expected to be a Numa* (na).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
TemplatesFromComposites(lua_State *L)
{
    LL_FUNC("TemplatesFromComposites");
    Pixa *pixac = ll_check_Pixa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    Pixa *pixa = jbTemplatesFromComposites(pixac, na);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Write the Pixa* (%pixa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be string containing the filename.
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaWrite(filename, pixa));
}

/**
 * \brief Write the Pixa* (%pixa) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 *
 * Leptonica's Notes:
 *      (1) Serializes a pixa in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixaWriteMem(&data, &size, pixa))
        return ll_push_nil(_fun, L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Pixa* to an Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaWriteStream(stream->f, pixa));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Pixa*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXA contained in the user data.
 */
Pixa *
ll_check_Pixa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pixa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Pixa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixa* contained in the user data.
 */
Pixa *
ll_opt_Pixa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Pixa(_fun, L, arg);
}

/**
 * \brief Push Pixa* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param pixa pointer to the PIXA
 * \return 1 Pixa* on the Lua stack.
 */
int
ll_push_Pixa(const char *_fun, lua_State *L, Pixa *pixa)
{
    if (!pixa)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, pixa);
}

/**
 * \brief Create a new Pixa*.
 * \param L Lua state.
 * \return 1 Pixa* on the Lua stack.
 */
int
ll_new_Pixa(lua_State *L)
{
    FUNC("ll_new_Pixa");
    Pixa *pixa = nullptr;
    Pixa *pixas = nullptr;
    Pix *pixs = nullptr;
    luaL_Stream* stream = nullptr;
    l_int32 copyflag = L_COPY;
    l_int32 n = 1;
    l_int32 cellw = 32;
    l_int32 cellh = 32;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        pixs = ll_opt_Pix(_fun, L, 1);
        n = ll_opt_l_int32(_fun, L, 2, 1);
        cellw = ll_opt_l_int32(_fun, L, 3, cellw);
        cellh = ll_opt_l_int32(_fun, L, 4, cellh);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        pixa = pixaCreateFromPix(pixs, n, cellw, cellh);
    }

    if (!pixa && ll_isudata(_fun, L, 1, LL_PIXA)) {
        pixas = ll_opt_Pixa(_fun, L, 1);
        copyflag = ll_check_access_storage(_fun, L, 2, copyflag);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIXA, reinterpret_cast<void *>(pixs));
        pixa = pixaCopy(pixas, copyflag);
    }

    if (!pixa && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        pixa = pixaReadStream(stream->f);
    }

    if (!pixa && ll_isinteger(_fun, L, 1)) {
        n = ll_check_l_int32(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixa = pixaCreate(n);
    }

    if (!pixa && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
            DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
                "filename", filename);
        pixa = pixaRead(filename);
    }

    if (!pixa && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        pixa = pixaReadMem(data, size);
    }

    if (!pixa) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixa = pixaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(pixa));
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Pixa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                        Destroy},
        {"__new",                       ll_new_Pixa},
        {"__newindex",                  ReplacePix},
        {"__len",                       GetCount},
        {"__tostring",                  toString},
        {"AccumulateSamples",           AccumulateSamples},
        {"AddBorderGeneral",            AddBorderGeneral},
        {"AddBox",                      AddBox},
        {"AddPix",                      AddPix},
        {"AddPixWithText",              AddPixWithText},
        {"AddTextNumber",               AddTextNumber},
        {"AddTextlines",                AddTextlines},
        {"AnyColormaps",                AnyColormaps},
        {"BinSort",                     BinSort},
        {"Centroids",                   Centroids},
        {"ChangeRefcount",              ChangeRefcount},
        {"Clear",                       Clear},
        {"ClipToForeground",            ClipToForeground},
        {"ClipToPix",                   ClipToPix},
        {"CompareInPdf",                CompareInPdf},
        {"ComparePhotoRegionsByHisto",  ComparePhotoRegionsByHisto},
        {"ConstrainedSelect",           ConstrainedSelect},
        {"ConvertTo1",                  ConvertTo1},
        {"ConvertTo32",                 ConvertTo32},
        {"ConvertTo8",                  ConvertTo8},
        {"ConvertTo8Colormap",          ConvertTo8Colormap},
        {"ConvertToNUpPixa",            ConvertToNUpPixa},
        {"ConvertToPdf",                ConvertToPdf},
        {"ConvertToPdfData",            ConvertToPdfData},
        {"ConvertToSameDepth",          ConvertToSameDepth},
        {"Copy",                        Copy},
        {"CountPixels",                 CountPixels},
        {"CountText",                   CountText},
        {"Create",                      Create},
        {"CreateFromBoxa",              CreateFromBoxa},
        {"CreateFromPix",               CreateFromPix},
        {"CreateFromPixacomp",          CreateFromPixacomp},
        {"Destroy",                     Destroy},
        {"Display",                     Display},
        {"GetAlignedStats",             GetAlignedStats},
        {"GetBoxGeometry",              GetBoxGeometry},
        {"GetCount",                    GetCount},
        {"GetPix",                      GetPix},
        {"InsertPix",                   InsertPix},
        {"Interleave",                  Interleave},
        {"Join",                        Join},
        {"Read",                        Read},
        {"ReadBarcodes",                ReadBarcodes},
        {"ReadFiles",                   ReadFiles},
        {"ReadMem",                     ReadMem},
        {"ReadStream",                  ReadStream},
        {"RemovePix",                   RemovePix},
        {"RemovePixAndSave",            RemovePixAndSave},
        {"ReplacePix",                  ReplacePix},
        {"TakePix",                     RemovePixAndSave},  /* alias name */
        {"TemplatesFromComposites",     TemplatesFromComposites},
        {"Write",                       Write},
        {"WriteMem",                    WriteMem},
        {"WriteStream",                 WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Pixa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
