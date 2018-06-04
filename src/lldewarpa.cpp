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
 * \file lldewarpa.cpp
 * \class Dewarpa
 *
 * An array of Dewarp.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DEWARPA

/** Define a function's name (_fun) with prefix LL_DEWARPA */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dewarpa **pdewa = ll_check_udata<Dewarpa>(_fun, L, 1, TNAME);
    Dewarpa *dewa = *pdewa;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "pdewa", reinterpret_cast<void *>(pdewa),
        "dewa", reinterpret_cast<void *>(dewa),
        "pages", dewarpaListPages(dewa));
    dewarpaDestroy(&dewa);
    *pdewa = nullptr;
    return 0;
}

/**
 * \brief Printable string for a Dewarpa*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dewa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 TNAME ": %p\n",
                 reinterpret_cast<void *>(dewa));
        /* TODO: more info */
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Apply disparity for a Dewarpa* (%dewa) and return the Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a l_int32 (grayin).
 * Arg #5 is expected to be a l_int32 (x).
 * Arg #6 is expected to be a l_int32 (y).
 * Arg #7 is expected to be a string (debugfile).
 *
 * Leptonica's Notes:
 *      (1) This applies the disparity arrays to the specified image.
 *      (2) Specify gray color for pixels brought in from the outside:
 *          0 is black, 255 is white.  Use -1 to select pixels from the
 *          boundary of the source image.
 *      (3) If the models and ref models have not been validated, this
 *          will do so by calling dewarpaInsertRefModels().
 *      (4) This works with both stripped and full resolution page models.
 *          If the full res disparity array(s) are missing, they are remade.
 *      (5) The caller must handle errors that are returned because there
 *          are no valid models or ref models for the page -- typically
 *          by using the input pixs.
 *      (6) If there is no model for %pageno, this will use the model for
 *          'refpage' and put the result in the dew for %pageno.
 *      (7) This populates the full resolution disparity arrays if
 *          necessary.  If x and/or y are positive, they are used,
 *          in conjunction with pixs, to determine the required
 *          slope-based extension of the full resolution disparity
 *          arrays in each direction.  When (x,y) == (0,0), all
 *          extension is to the right and down.  Nonzero values of (x,y)
 *          are useful for dewarping when pixs is deliberately undercropped.
 *      (8) Important: when applying disparity to a number of images,
 *          after calling this function and saving the resulting pixd,
 *          you should call dewarpMinimize(dew) on the dew for %pageno.
 *          This will remove pixs and pixd (or their clones) stored in dew,
 *          as well as the full resolution disparity arrays.  Together,
 *          these hold approximately 16 bytes for each pixel in pixs.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (%pixd) on the Lua stack
 */
static int
ApplyDisparity(lua_State *L)
{
    LL_FUNC("ApplyDisparity");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    l_int32 grayin = ll_check_l_int32(_fun, L, 4);
    l_int32 x = ll_check_l_int32(_fun, L, 5);
    l_int32 y = ll_check_l_int32(_fun, L, 6);
    const char *debugfile = ll_check_string(_fun, L, 7);
    Pix *pixd = nullptr;
    if (dewarpaApplyDisparity(dewa, pageno, pixs, grayin, x, y, &pixd, debugfile))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Apply disparity for a Dewarpa* (%dewa) and return the Boxa* (%boxad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a Boxa* (boxas).
 * Arg #5 is expected to be a l_int32 (mapdir).
 * Arg #6 is expected to be a l_int32 (x).
 * Arg #7 is expected to be a l_int32 (y).
 * Arg #8 is expected to be a string (debugfile).
 *
 * Leptonica's Notes:
 *      (1) This applies the disparity arrays in one of two mapping directions
 *          to the specified boxa.  It can be used in the backward direction
 *          to locate a box in the original coordinates that would have
 *          been dewarped to to the specified image.
 *      (2) If there is no model for %pageno, this will use the model for
 *          'refpage' and put the result in the dew for %pageno.
 *      (3) This works with both stripped and full resolution page models.
 *          If the full res disparity array(s) are missing, they are remade.
 *      (4) If an error occurs, a copy of the input boxa is returned.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxad) on the Lua stack
 */
static int
ApplyDisparityBoxa(lua_State *L)
{
    LL_FUNC("ApplyDisparityBoxa");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Boxa *boxas = ll_check_Boxa(_fun, L, 4);
    l_int32 mapdir = ll_check_l_int32(_fun, L, 5);
    l_int32 x = ll_check_l_int32(_fun, L, 6);
    l_int32 y = ll_check_l_int32(_fun, L, 7);
    const char *debugfile = ll_check_string(_fun, L, 8);
    Boxa *boxad = nullptr;
    if (dewarpaApplyDisparityBoxa(dewa, pageno, pixs, boxas, mapdir, x, y, &boxad, debugfile))
        return ll_push_nil(L);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Create a new Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (nptrs).
 * Arg #2 is expected to be a l_int32 (sampling).
 * Arg #3 is expected to be a l_int32 (redfactor).
 * Arg #4 is expected to be a l_int32 (minlines).
 * Arg #5 is expected to be a l_int32 (maxdist).
 *
 * Leptonica's Notes:
 *      (1) The sampling, minlines and maxdist parameters will be
 *          applied to all images.
 *      (2) The sampling factor is used for generating the disparity arrays
 *          from the input image.  For 2x reduced input, use a sampling
 *          factor that is half the sampling you want on the full resolution
 *          images.
 *      (3) Use %redfactor = 1 for full resolution; 2 for 2x reduction.
 *          All input images must be at one of these two resolutions.
 *      (4) %minlines is the minimum number of nearly full-length lines
 *          required to generate a vertical disparity array.  The default
 *          number is 15.  Use a smaller number to accept a questionable
 *          array, but not smaller than 4.
 *      (5) When a model can't be built for a page, it looks up to %maxdist
 *          in either direction for a valid model with the same page parity.
 *          Use -1 for the default value of %maxdist; use 0 to avoid using
 *          a ref model.
 *      (6) The ptr array is expanded as necessary to accommodate page images.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nptrs = ll_check_l_int32(_fun, L, 1);
    l_int32 sampling = ll_check_l_int32(_fun, L, 2);
    l_int32 redfactor = ll_check_l_int32(_fun, L, 3);
    l_int32 minlines = ll_check_l_int32(_fun, L, 4);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 5);
    Dewarpa *dewa = dewarpaCreate(nptrs, sampling, redfactor, minlines, maxdist);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixac).
 * Arg #2 is expected to be a l_int32 (useboth).
 * Arg #3 is expected to be a l_int32 (sampling).
 * Arg #4 is expected to be a l_int32 (minlines).
 * Arg #5 is expected to be a l_int32 (maxdist).
 *
 * Leptonica's Notes:
 *      (1) The returned dewa has disparity arrays calculated and
 *          is ready for serialization or for use in dewarping.
 *      (2) The sampling, minlines and maxdist parameters are
 *          applied to all images.  See notes in dewarpaCreate() for details.
 *      (3) The pixac is full.  Placeholders, if any, are w=h=d=1 images,
 *          and the real input images are 1 bpp at full resolution.
 *          They are assumed to be cropped to the actual page regions,
 *          and may be arbitrarily sparse in the array.
 *      (4) The output dewarpa is indexed by the page number.
 *          The offset in the pixac gives the mapping between the
 *          array index in the pixac and the page number.
 *      (5) This adds the ref page models.
 *      (6) This can be used to make models for any desired set of pages.
 *          The direct models are only made for pages with images in
 *          the pixacomp; the ref models are made for pages of the
 *          same parity within %maxdist of the nearest direct model.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
CreateFromPixacomp(lua_State *L)
{
    LL_FUNC("CreateFromPixacomp");
    PixaComp *pixac = ll_check_PixaComp(_fun, L, 1);
    l_int32 useboth = ll_check_l_int32(_fun, L, 2);
    l_int32 sampling = ll_check_l_int32(_fun, L, 3);
    l_int32 minlines = ll_check_l_int32(_fun, L, 4);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 5);
    Dewarpa * result = dewarpaCreateFromPixacomp(pixac, useboth, sampling, minlines, maxdist);
    return ll_push_Dewarpa(_fun, L, result);
}

/**
 * \brief Destroy a Dewarp* for page %pageno in the Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
DestroyDewarp(lua_State *L)
{
    LL_FUNC("DestroyDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaDestroyDewarp(dewa, pageno));
}

/**
 * \brief Get the Dewarp* for page %pageno from the Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (index).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp * on the Lua stack
 */
static int
GetDewarp(lua_State *L)
{
    LL_FUNC("GetDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 index = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpaGetDewarp(dewa, index);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Info about Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Info(lua_State *L)
{
    LL_FUNC("Info");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaInfo(stream->f, dewa));
}

/**
 * \brief Insert a Dewarp* (%dew) into Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a Dewarp* (dew).
 *
 * Leptonica's Notes:
 *      (1) This inserts the dewarp into the array, which now owns it.
 *          It also keeps track of the largest page number stored.
 *          It must be done before the disparity model is built.
 *      (2) Note that this differs from the usual method of filling out
 *          arrays in leptonica, where the arrays are compact and
 *          new elements are typically added to the end.  Here,
 *          the dewarp can be added anywhere, even beyond the initial
 *          allocation.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertDewarp(lua_State *L)
{
    LL_FUNC("InsertDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    Dewarp *dew = ll_check_Dewarp(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaInsertDewarp(dewa, dew));
}

/**
 * \brief Insert reference models into Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a boolean (notests).
 * Arg #3 is expected to be a boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) This destroys all dewarp models that are invalid, and then
 *          inserts reference models where possible.
 *      (2) If %notests == 1, this ignores the curvature constraints
 *          and assumes that all successfully built models are valid.
 *      (3) If useboth == 0, it uses the closest valid model within the
 *          distance and parity constraints.  If useboth == 1, it tries
 *          to use the closest allowed hvalid model; if it doesn't find
 *          an hvalid model, it uses the closest valid model.
 *      (4) For all pages without a model, this clears out any existing
 *          invalid and reference dewarps, finds the nearest valid model
 *          with the same parity, and inserts an empty dewarp with the
 *          reference page.
 *      (5) Then if it is requested to use both vertical and horizontal
 *          disparity arrays (useboth == 1), it tries to replace any
 *          hvalid == 0 model or reference with an hvalid == 1 reference.
 *      (6) The distance constraint is that any reference model must
 *          be within maxdist.  Note that with the parity constraint,
 *          no reference models will be used if maxdist < 2.
 *      (7) This function must be called, even if reference models will
 *          not be used.  It should be called after building models on all
 *          available pages, and after setting the rendering parameters.
 *      (8) If the dewa has been serialized, this function is called by
 *          dewarpaRead() when it is read back.  It is also called
 *          any time the rendering parameters are changed.
 *      (9) Note: if this has been called with useboth == 1, and useboth
 *          is reset to 0, you should first call dewarpaRestoreModels()
 *          to bring real models from the cache back to the primary array.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertRefModels(lua_State *L)
{
    LL_FUNC("InsertRefModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 notests = ll_check_boolean(_fun, L, 2);
    l_int32 debug = ll_check_boolean(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == dewarpaInsertRefModels(dewa, notests, debug));
}

/**
 * \brief List pages for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 *
 * Leptonica's Notes:
 *      (1) This generates two numas, stored in the dewarpa, that give:
 *          (a) the page number for each dew that has a page model.
 *          (b) the page number for each dew that has either a page
 *              model or a reference model.
 *          It can be called at any time.
 *      (2) It is called by the dewarpa serializer before writing.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ListPages(lua_State *L)
{
    LL_FUNC("ListPages");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 res = dewarpaListPages(dewa);
    ll_push_l_int32(_fun, L, res);
    return 1;
}

/**
 * \brief Get model stats for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 *
 * Leptonica's Notes:
 *      (1) A page without a model has no dew.  It most likely failed to
 *          generate a vertical model, and has not been assigned a ref
 *          model from a neighboring page with a valid vertical model.
 *      (2) A page has vsuccess == 1 if there is at least a model of the
 *          vertical disparity.  The model may be invalid, in which case
 *          dewarpaInsertRefModels() will stash it in the cache and
 *          attempt to replace it by a valid ref model.
 *      (3) A vvvalid model is a vertical disparity model whose parameters
 *          satisfy the constraints given in dewarpaSetValidModels().
 *      (4) A page has hsuccess == 1 if both the vertical and horizontal
 *          disparity arrays have been constructed.
 *      (5) An  hvalid model has vertical and horizontal disparity
 *          models whose parameters satisfy the constraints given
 *          in dewarpaSetValidModels().
 *      (6) A page has a ref model if it failed to generate a valid
 *          model but was assigned a vvalid or hvalid model on another
 *          page (within maxdist) by dewarpaInsertRefModel().
 *      (7) This calls dewarpaTestForValidModel(); it ignores the vvalid
 *          and hvalid fields.
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 integers on the Lua stack
 */
static int
ModelStats(lua_State *L)
{
    LL_FUNC("ModelStats");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 nnone = 0;
    l_int32 nvsuccess = 0;
    l_int32 nvvalid = 0;
    l_int32 nhsuccess = 0;
    l_int32 nhvalid = 0;
    l_int32 nref = 0;
    if (dewarpaModelStats(dewa, &nnone, &nvsuccess, &nvvalid, &nhsuccess, &nhvalid, &nref))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nnone);
    ll_push_l_int32(_fun, L, nvsuccess);
    ll_push_l_int32(_fun, L, nvvalid);
    ll_push_l_int32(_fun, L, nhsuccess);
    ll_push_l_int32(_fun, L, nhvalid);
    ll_push_l_int32(_fun, L, nref);
    return 6;
}

/**
 * \brief Get model status for a page (%pageno) in Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 *
 * Leptonica's Notes:
 *      (1) This tests if a model has been built, not if it is valid.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 l_int32 (%vsuccess, %hsuccess) on the Lua stack
 */
static int
ModelStatus(lua_State *L)
{
    LL_FUNC("ModelStatus");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    l_int32 vsuccess = 0;
    l_int32 hsuccess = 0;
    if (dewarpaModelStatus(dewa, pageno, &vsuccess, &hsuccess))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, vsuccess);
    ll_push_l_int32(_fun, L, hsuccess);
    return 2;
}

/**
 * \brief Read a Dewarpa* from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dewarpa *dewa = dewarpaRead(filename);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Read a Dewarpa* from a lstring (%str, %size).
 * <pre>
 * Arg #1 is expected to be a lstring (str).
 * Arg #2 is expected to be a size_t (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    /* XXX: deconstify */
    l_uint8 *data = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(str));
    Dewarpa *dewa = dewarpaReadMem(data, size);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Read a Dewarpa* from a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (%stream).
 *
 * Leptonica's Notes:
 *      (1) The serialized dewarp contains a Numa that gives the
 *          (increasing) page number of the dewarp structs that are
 *          contained.
 *      (2) Reference pages are added in after readback.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dewarpa *dewa = dewarpaReadStream(stream->f);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Restore models for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 *
 * Leptonica's Notes:
 *      (1) This puts all real models (and only real models) in the
 *          primary dewarpa array.  First remove all dewarps that are
 *          only references to other page models.  Then move all models
 *          that had been cached back into the primary dewarp array.
 *      (2) After this is done, we still need to recompute and insert
 *          the reference models before dewa->modelsready is true.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RestoreModels(lua_State *L)
{
    LL_FUNC("RestoreModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == dewarpaRestoreModels(dewa));
}

/**
 * \brief Set check columns for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a boolean (check_columns).
 *
 * Leptonica's Notes:
 *      (1) This sets the 'check_columns" field.  If set, and if
 *          'useboth' is set, this will count the number of text
 *          columns.  If the number is larger than 1, this will
 *          prevent the application of horizontal disparity arrays
 *          if they exist.  Note that the default value of check_columns
 *          if 0 (FALSE).
 *      (2) This field is set to 0 by default.  For horizontal disparity
 *          correction to take place on a single column of text, you must have:
 *           - a valid horizontal disparity array
 *           - useboth = 1 (TRUE)
 *          If there are multiple columns, additionally
 *           - check_columns = 0 (FALSE)
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetCheckColumns(lua_State *L)
{
    LL_FUNC("SetCheckColumns");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 check_columns = ll_opt_boolean(_fun, L, 2, TRUE);
    return ll_push_boolean(_fun, L, 0 == dewarpaSetCheckColumns(dewa, check_columns));
}

/**
 * \brief Set curvatures for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (max_linecurv).
 * Arg #3 is expected to be a l_int32 (min_diff_linecurv).
 * Arg #4 is expected to be a l_int32 (max_diff_linecurv).
 * Arg #5 is expected to be a l_int32 (max_edgecurv).
 * Arg #6 is expected to be a l_int32 (max_diff_edgecurv).
 * Arg #7 is expected to be a l_int32 (max_edgeslope).
 *
 * Leptonica's Notes:
 *      (1) Approximating the line by a quadratic, the coefficent
 *          of the quadratic term is the curvature, and distance
 *          units are in pixels (of course).  The curvature is very
 *          small, so we multiply by 10^6 and express the constraints
 *          on the model curvatures in micro-units.
 *      (2) This sets five curvature thresholds and a slope threshold:
 *          * the maximum absolute value of the vertical disparity
 *            line curvatures
 *          * the minimum absolute value of the largest difference in
 *            vertical disparity line curvatures (Use a value of 0
 *            to accept all models.)
 *          * the maximum absolute value of the largest difference in
 *            vertical disparity line curvatures
 *          * the maximum absolute value of the left and right edge
 *            curvature for the horizontal disparity
 *          * the maximum absolute value of the difference between
 *            left and right edge curvature for the horizontal disparity
 *          all in micro-units, for dewarping to take place.
 *          Use -1 for default values.
 *      (3) An image with a line curvature less than about 0.00001
 *          has fairly straight textlines.  This is 10 micro-units.
 *      (4) For example, if %max_linecurv == 100, this would prevent dewarping
 *          if any of the lines has a curvature exceeding 100 micro-units.
 *          A model having maximum line curvature larger than about 150
 *          micro-units should probably not be used.
 *      (5) A model having a left or right edge curvature larger than
 *          about 100 micro-units should probably not be used.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetCurvatures(lua_State *L)
{
    LL_FUNC("SetCurvatures");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 max_linecurv = ll_check_l_int32(_fun, L, 2);
    l_int32 min_diff_linecurv = ll_check_l_int32(_fun, L, 3);
    l_int32 max_diff_linecurv = ll_check_l_int32(_fun, L, 4);
    l_int32 max_edgecurv = ll_check_l_int32(_fun, L, 5);
    l_int32 max_diff_edgecurv = ll_check_l_int32(_fun, L, 6);
    l_int32 max_edgeslope = ll_check_l_int32(_fun, L, 7);
    return ll_push_boolean(_fun, L, 0 == dewarpaSetCurvatures(dewa,
                                              max_linecurv, min_diff_linecurv, max_diff_linecurv,
                                              max_edgecurv, max_diff_edgecurv, max_edgeslope));
}

/**
 * \brief Set max distance for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (maxdist).
 *
 * Leptonica's Notes:
 *      (1) This sets the maxdist field.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMaxDistance(lua_State *L)
{
    LL_FUNC("SetMaxDistance");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaSetMaxDistance(dewa, maxdist));
}

/**
 * \brief Set valid models for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a boolean (notests).
 * Arg #3 is expected to be a boolean (debug).
 *
 * Leptonica's Notes:
 *      (1) A valid model must meet the rendering requirements, which
 *          include whether or not a vertical disparity model exists
 *          and conditions on curvatures for vertical and horizontal
 *          disparity models.
 *      (2) If %notests == 1, this ignores the curvature constraints
 *          and assumes that all successfully built models are valid.
 *      (3) This function does not need to be called by the application.
 *          It is called by dewarpaInsertRefModels(), which
 *          will destroy all invalid dewarps.  Consequently, to inspect
 *          an invalid dewarp model, it must be done before calling
 *          dewarpaInsertRefModels().
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetValidModels(lua_State *L)
{
    LL_FUNC("SetValidModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 notests = ll_opt_boolean(_fun, L, 2, FALSE);
    l_int32 debug = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == dewarpaSetValidModels(dewa, notests, debug));
}

/**
 * \brief Show arrays for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_float32 (scalefact).
 * Arg #3 is expected to be a l_int32 (first).
 * Arg #4 is expected to be a l_int32 (last).
 *
 * Leptonica's Notes:
 *      (1) Generates a pdf of contour plots of the disparity arrays.
 *      (2) This only shows actual models; not ref models
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ShowArrays(lua_State *L)
{
    LL_FUNC("ShowArrays");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_float32 scalefact = ll_check_l_float32(_fun, L, 2);
    l_int32 first = ll_check_l_int32(_fun, L, 3);
    l_int32 last = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == dewarpaShowArrays(dewa, scalefact, first, last));
}

/**
 * \brief Strip reference models from Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 *
 * Leptonica's Notes:
 *      (1) This examines each dew in a dewarpa, and removes
 *          all that don't have their own page model (i.e., all
 *          that have "references" to nearby pages with valid models).
 *          These references were generated by dewarpaInsertRefModels(dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
StripRefModels(lua_State *L)
{
    LL_FUNC("StripRefModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == dewarpaStripRefModels(dewa));
}

/**
 * \brief Set use both arrays for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (useboth).
 *
 * Leptonica's Notes:
 *      (1) This sets the useboth field.  If set, this will attempt
 *          to apply both vertical and horizontal disparity arrays.
 *          Note that a model with only a vertical disparity array will
 *          always be valid.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
UseBothArrays(lua_State *L)
{
    LL_FUNC("UseBothArrays");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 useboth = ll_check_boolean(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaUseBothArrays(dewa, useboth));
}

/**
 * \brief Write Dewarpa* (%dewa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaWrite(filename, dewa));
}

/**
 * \brief Write Dewarpa* (%dewa) to a lstring (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 *
 * Leptonica's Notes:
 *      (1) Serializes a dewarpa in memory and puts the result in a buffer.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    size_t size = 0;
    l_uint8 *data = nullptr;
    if (dewarpaWriteMem(&data, &size, dewa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write Dewarpa* (%dewa) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a luaL_Stream* (%stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dewarpaWriteStream(stream->f, dewa));
}

/**
 * \brief Check Lua stack at index %arg for udata of class Dewarpa*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarpa* contained in the user data
 */
Dewarpa *
ll_check_Dewarpa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dewarpa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Dewarpa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarpa* contained in the user data
 */
Dewarpa *
ll_opt_Dewarpa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Dewarpa(_fun, L, arg);
}
/**
 * \brief Push Dewarpa* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param dew pointer to the Dewarpa
 * \return 1 Dewarpa* on the Lua stack
 */
int
ll_push_Dewarpa(const char *_fun, lua_State *L, Dewarpa *dew)
{
    if (!dew)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, dew);
}

/**
 * \brief Create and push a new Dewarpa*.
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
int
ll_new_Dewarpa(lua_State *L)
{
    FUNC("ll_new_Dewarpa");
    PixaComp *pixac = nullptr;
    l_int32 nptrs = 1;
    l_int32 useboth = TRUE;
    l_int32 sampling = 1;
    l_int32 redfactor = 1;
    l_int32 minlines = 5;
    l_int32 maxdist = 20;
    Dewarpa *dewa = nullptr;

    if (ll_isudata(_fun, L, 1, LL_PIXACOMP)) {
        pixac = ll_opt_PixaComp(_fun, L, 1);
        useboth = ll_opt_boolean(_fun, L, 2, useboth);
        sampling = ll_opt_l_int32(_fun, L, 3, sampling);
        minlines = ll_opt_l_int32(_fun, L, 4, minlines);
        maxdist = ll_opt_l_int32(_fun, L, 5, maxdist);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %s, %s = %d, %s = %d, %s = %d\n", _fun,
            LL_PIXACOMP, reinterpret_cast<void *>(pixac),
            "useboth", useboth ? "true" : "false",
            "sampling", sampling,
            "minlines", minlines,
            "maxdist", maxdist);
        dewa = dewarpaCreateFromPixacomp(pixac, useboth, sampling, minlines, maxdist);
    }

    if (!dewa && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        dewa = dewarpaReadStream(stream->f);
    }

    if (!dewa && ll_isinteger(_fun, L, 1)) {
        nptrs = ll_opt_l_int32(_fun, L, 1, nptrs);
        sampling = ll_opt_l_int32(_fun, L, 2, sampling);
        redfactor = ll_opt_l_int32(_fun, L, 3, redfactor);
        minlines = ll_opt_l_int32(_fun, L, 4, minlines);
        maxdist = ll_opt_l_int32(_fun, L, 5, maxdist);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %d, %s = %d, %s = %d\n", _fun,
            "nptrs", nptrs,
            "sampling", sampling,
            "redfactor", redfactor,
            "minlines", minlines,
            "maxdist", maxdist);
        dewa = dewarpaCreate(nptrs, sampling, redfactor, minlines, maxdist);
    }

    if (!dewa) {
        /* FIXME: default parameters? */
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %d, %s = %d, %s = %d\n", _fun,
            "nptrs", nptrs,
            "sampling", sampling,
            "redfactor", redfactor,
            "minlines", minlines,
            "maxdist", maxdist);
        dewa = dewarpaCreate(nptrs, sampling, redfactor, minlines, maxdist);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(dewa));
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Register the Dewarpa* methods and functions in the LL_DEWARP meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Dewarpa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Dewarpa},
        {"__tostring",              toString},
        {"ApplyDisparity",          ApplyDisparity},
        {"ApplyDisparityBoxa",      ApplyDisparityBoxa},
        {"Create",                  Create},
        {"CreateFromPixacomp",      CreateFromPixacomp},
        {"Destroy",                 Destroy},
        {"DestroyDewarp",           DestroyDewarp},
        {"GetDewarp",               GetDewarp},
        {"Info",                    Info},
        {"InsertDewarp",            InsertDewarp},
        {"InsertRefModels",         InsertRefModels},
        {"ListPages",               ListPages},
        {"ModelStats",              ModelStats},
        {"ModelStatus",             ModelStatus},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RestoreModels",           RestoreModels},
        {"SetCheckColumns",         SetCheckColumns},
        {"SetCurvatures",           SetCurvatures},
        {"SetMaxDistance",          SetMaxDistance},
        {"SetValidModels",          SetValidModels},
        {"ShowArrays",              ShowArrays},
        {"StripRefModels",          StripRefModels},
        {"UseBothArrays",           UseBothArrays},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_Dewarpa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
