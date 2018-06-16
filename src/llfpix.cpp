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
 * \file llfpix.cpp
 * \class FPix
 *
 * A 2-D pixels array of floats (l_float32).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_FPIX

/** Define a function's name (_fun) with prefix FPix */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the fpix.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    FPix *fpix = ll_take_udata<FPix>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "fpix", reinterpret_cast<void *>(fpix),
        "refcount", fpixGetRefcount(fpix));
    fpixDestroy(&fpix);
    return 0;
}

/**
 * \brief Printable string for a FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    luaL_Buffer B;
    void *data;
    l_int32 w, h, wpl, refcount, xres, yres;
    long size;

    luaL_buffinit(L, &B);
    if (!fpix) {
        luaL_addstring(&B, "nil");
    } else if (fpixGetDimensions(fpix, &w, &h)) {
            luaL_addstring(&B, "invalid");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(fpix));
        luaL_addstring(&B, str);
        wpl = fpixGetWpl(fpix);
        snprintf(str, LL_STRBUFF,
                 "\n    %s = %d, %s = %d, %s = %d",
                 "width", w,
                 "height", h,
                 "wpl", wpl);
        luaL_addstring(&B, str);
        luaL_addstring(&B, str);
        fpixGetResolution(fpix, &xres, &yres);
        refcount = fpixGetRefcount(fpix);
        snprintf(str, LL_STRBUFF,
                 "\n    %s = %d, %s = %d, %s = %d",
                 "xres", xres,
                 "yres", yres,
                 "refcount", refcount);
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        data = fpixGetData(fpix);
        size = static_cast<long>(sizeof(l_float64)) * wpl * h;
        snprintf(str, LL_STRBUFF,
                 "\n    %s = %p, %s = %#" PRIx64,
                 "data", data,
                 "size", size);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add a border (%left, %right, %top, %bot) to the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bot).
 *
 * Leptonica's Notes:
 *      (1) Adds border of '0' 32-bit pixels
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
AddBorder(lua_State *L)
{
    LL_FUNC("AddBorder");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bot = ll_check_l_int32(_fun, L, 5);
    FPix *fpix = fpixAddBorder(fpixs, left, right, top, bot);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Add a continued border (%left, %right, %top, %bot) to the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bot).
 *
 * Leptonica's Notes:
 *      (1) This adds pixels on each side whose values are equal to
 *          the value on the closest boundary pixel.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* (%fpix) on the Lua stack.
 */
static int
AddContinuedBorder(lua_State *L)
{
    LL_FUNC("AddContinuedBorder");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bot = ll_check_l_int32(_fun, L, 5);
    FPix *fpix = fpixAddContinuedBorder(fpixs, left, right, top, bot);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Add a mirrored border (%left, %right, %top, %bot) to the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bot).
 *
 * Leptonica's Notes:
 *      (1) See pixAddMirroredBorder() for situations of usage.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
AddMirroredBorder(lua_State *L)
{
    LL_FUNC("AddMirroredBorder");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bot = ll_check_l_int32(_fun, L, 5);
    FPix *fpix = fpixAddMirroredBorder(fpixs, left, right, top, bot);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Add (%addc) to and multiply by (%multc) the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_float32 (addc).
 * Arg #3 is expected to be a l_float32 (multc).
 *
 * Leptonica's Notes:
 *      (1) This is an in-place operation.
 *      (2) It can be used to multiply each pixel by a constant,
 *          and also to add a constant to each pixel.  Multiplication
 *          is done first.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddMultConstant(lua_State *L)
{
    LL_FUNC("AddMultConstant");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 addc = ll_opt_l_float32(_fun, L, 2, 0.0f);
    l_float32 multc = ll_opt_l_float32(_fun, L, 3, 1.0f);
    return ll_push_boolean(_fun, L, 0 == fpixAddMultConstant(fpix, addc, multc));
}

/**
 * \brief Add a slope border (%left, %right, %top, %bot) to the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bot).
 *
 * Leptonica's Notes:
 *      (1) This adds pixels on each side whose values have a normal
 *          derivative equal to the normal derivative at the boundary
 *          of fpixs.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
AddSlopeBorder(lua_State *L)
{
    LL_FUNC("AddSlopeBorder");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bot = ll_check_l_int32(_fun, L, 5);
    FPix *fpix = fpixAddSlopeBorder(fpixs, left, right, top, bot);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Affine transformation of the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_float32 (inval).
 * </pre>
 * \param L Lua state.
 * \return 2 FPix* (%fpix) and l_float32 (%vc) on the Lua stack.
 */
static int
Affine(lua_State *L)
{
    LL_FUNC("Affine");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_float32 inval = ll_check_l_float32(_fun, L, 2);
    l_float32 vc = 0;
    FPix *fpix = fpixAffine(fpixs, &vc, inval);
    ll_push_FPix(_fun, L, fpix);
    ll_push_l_float32(_fun, L, vc);
    return 2;
}

/**
 * \brief Affine transformation with Pta* (%ptad, %ptas) of the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (border).
 * Arg #5 is expected to be a l_float32 (inval).
 *
 * Leptonica's Notes:
 *      (1) If %border > 0, all four sides are extended by that distance,
 *          and removed after the transformation is finished.  Pixels
 *          that would be brought in to the trimmed result from outside
 *          the extended region are assigned %inval.  The purpose of
 *          extending the image is to avoid such assignments.
 *      (2) On the other hand, you may want to give all pixels that
 *          are brought in from outside fpixs a specific value.  In that
 *          case, set %border == 0.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
AffinePta(lua_State *L)
{
    LL_FUNC("AffinePta");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 border = ll_opt_l_int32(_fun, L, 4);
    l_float32 inval = ll_opt_l_float32(_fun, L, 5);
    FPix *fpix = fpixAffinePta(fpixs, ptad, ptas, border, inval);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Auto render (%ncontours) contours of the FPix* (%fpix) to a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (ncontours).
 *
 * Leptonica's Notes:
 *      (1) The increment is set to get approximately %ncontours.
 *      (2) The proximity to the target value for contour display
 *          is set to 0.15.
 *      (3) Negative values are rendered in red; positive values as black.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
AutoRenderContours(lua_State *L)
{
    LL_FUNC("AutoRenderContours");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 ncontours = ll_check_l_int32(_fun, L, 2);
    Pix *pix = fpixAutoRenderContours(fpix, ncontours);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Change the reference count of FPix* (%fpix) by delta (%delta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixChangeRefcount(fpix, delta));
}

/**
 * \brief Clone the FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 *
 * Leptonica's Notes:
 *      (1) See pixClone() for definition and usage.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    FPix *fpix = fpixClone(fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Convert the FPix* (%fpix) to DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
ConvertToDPix(lua_State *L)
{
    LL_FUNC("ConvertToDPix");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    DPix *dpix = fpixConvertToDPix(fpix);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Convert the FPix* (%fpix) to Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (outdepth).
 * Arg #3 is expected to be a string describing what to do with negative values (negvals).
 * Arg #4 is expected to be a boolean (errorflag).
 *
 * Leptonica's Notes:
 *      (1) Use %outdepth = 0 to programmatically determine the
 *          output depth.  If no values are greater than 255,
 *          it will set outdepth = 8; otherwise to 16 or 32.
 *      (2) Because we are converting a float to an unsigned int
 *          with a specified dynamic range (8, 16 or 32 bits), errors
 *          can occur.  If errorflag == TRUE, output the number
 *          of values out of range, both negative and positive.
 *      (3) If a pixel value is positive and out of range, clip to
 *          the maximum value represented at the outdepth of 8, 16
 *          or 32 bits.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
ConvertToPix(lua_State *L)
{
    LL_FUNC("ConvertToPix");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 2);
    l_int32 negvals = ll_check_negvals(_fun, L, 3);
    l_int32 errorflag = ll_opt_boolean(_fun, L, 4);
    Pix *pix = fpixConvertToPix(fpix, outdepth, negvals, errorflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convolution of the FPix* (%fpixs) using Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a Kernel* (kel).
 * Arg #3 is expected to be a boolean (normflag).
 *
 * Leptonica's Notes:
 *      (1) This gives a float convolution with an arbitrary kernel.
 *      (2) If normflag == 1, the result is normalized by scaling all
 *          kernel values for a unit sum.  If the sum of kernel values
 *          is very close to zero, the kernel can not be normalized and
 *          the convolution will not be performed.  A warning is issued.
 *      (3) With the FPix, there are no issues about negative
 *          array or kernel values.  The convolution is performed
 *          with single precision arithmetic.
 *      (4) To get a subsampled output, call l_setConvolveSampling().
 *          The time to make a subsampled output is reduced by the
 *          product of the sampling factors.
 *      (5) This uses a mirrored border to avoid special casing on
 *          the boundaries.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Convolve(lua_State *L)
{
    LL_FUNC("Convolve");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Kernel *kel = ll_check_Kernel(_fun, L, 2);
    l_int32 normflag = ll_check_boolean(_fun, L, 3);
    FPix *fpix = fpixConvolve(fpixs, kel, normflag);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Separate x,y convolution of the FPix* (%fpixs) using Kernel*s (%kelx, %kely).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a Kernel* (kelx).
 * Arg #3 is expected to be a Kernel* (kely).
 * Arg #4 is expected to be a l_int32 (normflag).
 *
 * Leptonica's Notes:
 *      (1) This does a convolution with a separable kernel that is
 *          is a sequence of convolutions in x and y.  The two
 *          one-dimensional kernel components must be input separately;
 *          the full kernel is the product of these components.
 *          The support for the full kernel is thus a rectangular region.
 *      (2) The normflag parameter is used as in fpixConvolve().
 *      (3) Warning: if you use l_setConvolveSampling() to get a
 *          subsampled output, and the sampling factor is larger than
 *          the kernel half-width, it is faster to use the non-separable
 *          version pixConvolve().  This is because the first convolution
 *          here must be done on every raster line, regardless of the
 *          vertical sampling factor.  If the sampling factor is smaller
 *          than kernel half-width, it's faster to use the separable
 *          convolution.
 *      (4) This uses mirrored borders to avoid special casing on
 *          the boundaries.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
ConvolveSep(lua_State *L)
{
    LL_FUNC("ConvolveSep");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Kernel *kelx = ll_check_Kernel(_fun, L, 2);
    Kernel *kely = ll_check_Kernel(_fun, L, 3);
    l_int32 normflag = ll_check_boolean(_fun, L, 4);
    FPix *fpix = fpixConvolveSep(fpixs, kelx, kely, normflag);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Copy the FPix* (%fpixs) to FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) There are three cases:
 *            (a) fpixd == null  (makes a new fpix; refcount = 1)
 *            (b) fpixd == fpixs  (no-op)
 *            (c) fpixd != fpixs  (data copy; no change in refcount)
 *          If the refcount of fpixd > 1, case (c) will side-effect
 *          these handles.
 *      (2) The general pattern of use is:
 *             fpixd = fpixCopy(fpixd, fpixs);
 *          This will work for all three cases.
 *          For clarity when the case is known, you can use:
 *            (a) fpixd = fpixCopy(NULL, fpixs);
 *            (c) fpixCopy(fpixd, fpixs);
 *      (3) For case (c), we check if fpixs and fpixd are the same size.
 *          If so, the data is copied directly.
 *          Otherwise, the data is reallocated to the correct size
 *          and the copy proceeds.  The refcount of fpixd is unchanged.
 *      (4) This operation, like all others that may involve a pre-existing
 *          fpixd, will side-effect any existing clones of fpixd.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    FPix *fpixd = ll_opt_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixCopy(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Copy the resolution of FPix* (%fpixs) to FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
CopyResolution(lua_State *L)
{
    LL_FUNC("CopyResolution");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixCopyResolution(fpixd, fpixs));
}

/**
 * \brief Create a new FPix*.
 *
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 *
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 width = ll_opt_l_int32(_fun, L, 1, 1);
    l_int32 height = ll_opt_l_int32(_fun, L, 2, 1);
    FPix *fpix = fpixCreate(width, height);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Create a FPix* (%fpix) from a template FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) Makes a FPix of the same size as the input FPix, with the
 *          data array allocated and initialized to 0.
 *      (2) Copies the resolution.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
CreateTemplate(lua_State *L)
{
    LL_FUNC("CreateTemplate");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    FPix *fpix = fpixCreateTemplate(fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Display the max dynamic range of a FPix* (%fpix) into a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 Pix* on the Lua stack.
 */
static int
DisplayMaxDynamicRange(lua_State *L)
{
    LL_FUNC("DisplayMaxDynamicRange");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Pix *pix = fpixDisplayMaxDynamicRange(fpixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Swap endianness of FPix* (%fpixs) giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) On big-endian hardware, this does byte-swapping on each of
 *          the 4-byte floats in the fpix data.  On little-endians,
 *          the data is unchanged.  This is used for serialization
 *          of fpix; the data is serialized in little-endian byte
 *          order because most hardware is little-endian.
 *      (2) The operation can be either in-place or, if fpixd == NULL,
 *          a new fpix is made.  If not in-place, caller must catch
 *          the returned pointer.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
EndianByteSwap(lua_State *L)
{
    LL_FUNC("EndianByteSwap");
    FPix *fpixd = ll_opt_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixEndianByteSwap(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Flip left-right FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
FlipLR(lua_State *L)
{
    LL_FUNC("FlipLR");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixFlipLR(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Flip top-bottom FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) This does a top-bottom flip of the image, which is
 *          equivalent to a rotation out of the plane about a
 *          horizontal line through the image center.
 *      (2) There are 3 cases for input:
 *          (a) fpixd == null (creates a new fpixd)
 *          (b) fpixd == fpixs (in-place operation)
 *          (c) fpixd != fpixs (existing fpixd)
 *      (3) For clarity, use these three patterns, respectively:
 *          (a) fpixd = fpixFlipTB(NULL, fpixs);
 *          (b) fpixFlipTB(fpixs, fpixs);
 *          (c) fpixFlipTB(fpixd, fpixs);
 *      (4) If an existing fpixd is not the same size as fpixs, the
 *          image data will be reallocated.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
FlipTB(lua_State *L)
{
    LL_FUNC("FlipTB");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixFlipTB(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Get the data of FPix* (%fpix) as 2D table array of l_float32 (%farray).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
GetData(lua_State *L)
{
    LL_FUNC("GetData");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 wpl = fpixGetWpl(fpix);
    l_int32 w = 0;
    l_int32 h = 0;
    if (fpixGetDimensions(fpix, &w, &h))
        return ll_push_nil(L);
    l_float32 *farray = fpixGetData(fpix);
    return ll_pack_Farray_2d(_fun, L, farray, wpl, h);
}

/**
 * \brief Get the dimensions (%w, %h) of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 2 integers on the Lua stack.
 */
static int
GetDimensions(lua_State *L)
{
    LL_FUNC("GetDimensions");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (fpixGetDimensions(fpix, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Get the maximum value (%maxval) and coordinates (%xmaxloc, %ymaxloc) in FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 3 number (%maxval) and integers (%xmaxloc, %ymaxloc) on the Lua stack.
 */
static int
GetMax(lua_State *L)
{
    LL_FUNC("GetMax");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 maxval = 0;
    l_int32 xmaxloc = 0;
    l_int32 ymaxloc = 0;
    if (fpixGetMax(fpix, &maxval, &xmaxloc, &ymaxloc))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, maxval);
    ll_push_l_int32(_fun, L, xmaxloc);
    ll_push_l_int32(_fun, L, ymaxloc);
    return 3;
}

/**
 * \brief Get the minmum value (%minval) and coordinates (%xminloc, %yminloc) in FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 3 number (%minval) and integers (%xminloc, %yminloc) on the Lua stack.
 */
static int
GetMin(lua_State *L)
{
    LL_FUNC("GetMin");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 minval = 0;
    l_int32 xminloc = 0;
    l_int32 yminloc = 0;
    if (fpixGetMin(fpix, &minval, &xminloc, &yminloc))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, minval);
    ll_push_l_int32(_fun, L, xminloc);
    ll_push_l_int32(_fun, L, yminloc);
    return 3;
}

/**
 * \brief Get the pixel value at (%x, %y) from FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 l_float32 on the Lua stack.
 */
static int
GetPixel(lua_State *L)
{
    LL_FUNC("GetPixel");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float32 val = 0;
    if (fpixGetPixel(fpix, x, y, &val))
        return ll_push_nil(L);
    return ll_push_l_float32(_fun, L, val);
}

/**
 * \brief Get the reference count of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    return ll_push_l_int32(_fun, L, fpixGetRefcount(fpix));
}

/**
 * \brief Get the resolution (%xres, %yres) of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 2 on the Lua stack.
 */
static int
GetResolution(lua_State *L)
{
    LL_FUNC("GetResolution");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 xres = 0;
    l_int32 yres = 0;
    if (fpixGetResolution(fpix, &xres, &yres))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xres);
    ll_push_l_int32(_fun, L, yres);
    return 2;
}

/**
 * \brief Get the words-per-line (%wpl) value for FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 1 l_int32 on the Lua stack.
 */
static int
GetWpl(lua_State *L)
{
    LL_FUNC("GetWpl");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 result = fpixGetWpl(fpix);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Create a linear combination of two FPix* (%fpix1, %fpix2) using fractions (%a, %b).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs1).
 * Arg #3 is expected to be a FPix* (fpixs2).
 * Arg #4 is expected to be a l_float32 (a).
 * Arg #5 is expected to be a l_float32 (b).
 *
 * Leptonica's Notes:
 *      (1) Computes pixelwise linear combination: a * src1 + b * src2
 *      (2) Alignment is to UL corner.
 *      (3) There are 3 cases.  The result can go to a new dest,
 *          in-place to fpixs1, or to an existing input dest:
 *          * fpixd == null:   (src1 + src2) --> new fpixd
 *          * fpixd == fpixs1:  (src1 + src2) --> src1  (in-place)
 *          * fpixd != fpixs1: (src1 + src2) --> input fpixd
 *      (4) fpixs2 must be different from both fpixd and fpixs1.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
LinearCombination(lua_State *L)
{
    LL_FUNC("LinearCombination");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs1 = ll_check_FPix(_fun, L, 2);
    FPix *fpixs2 = ll_check_FPix(_fun, L, 3);
    l_float32 a = ll_check_l_float32(_fun, L, 4);
    l_float32 b = ll_check_l_float32(_fun, L, 5);
    FPix *result = fpixLinearCombination(fpixd, fpixs1, fpixs2, a, b);
    return ll_push_FPix(_fun, L, result);
}

/**
 * \brief Print information about FPix* (%fpix) to a stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (factor).
 *
 * Leptonica's Notes:
 *      (1) Subsampled printout of fpix for debugging.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
PrintStream(lua_State *L)
{
    LL_FUNC("PrintStream");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == fpixPrintStream(stream->f, fpix, factor));
}

/**
 * \brief Get a projective for value (%inval) of FPix* (%fpixs) giving FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_float32 (inval).
 * </pre>
 * \param L Lua state.
 * \return 2 FPix* (%fpix) and l_float32 (%vc) on the Lua stack.
 */
static int
Projective(lua_State *L)
{
    LL_FUNC("Projective");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_float32 inval = ll_check_l_float32(_fun, L, 2);
    l_float32 vc = 0;
    FPix *fpix = fpixProjective(fpixs, &vc, inval);
    ll_push_FPix(_fun, L, fpix);
    ll_push_l_float32(_fun, L, vc);
    return 2;
}

/**
 * \brief Get a projective for value (%inval) of FPix* (%fpixs) using Pta* (%ptad, %ptad) giving FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (border).
 * Arg #5 is expected to be a l_float32 (inval).
 *
 * Leptonica's Notes:
 *      (1) If %border > 0, all four sides are extended by that distance,
 *          and removed after the transformation is finished.  Pixels
 *          that would be brought in to the trimmed result from outside
 *          the extended region are assigned %inval.  The purpose of
 *          extending the image is to avoid such assignments.
 *      (2) On the other hand, you may want to give all pixels that
 *          are brought in from outside fpixs a specific value.  In that
 *          case, set %border == 0.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
ProjectivePta(lua_State *L)
{
    LL_FUNC("ProjectivePta");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 border = ll_check_l_int32(_fun, L, 4);
    l_float32 inval = ll_check_l_float32(_fun, L, 5);
    FPix *fpix = fpixProjectivePta(fpixs, ptad, ptas, border, inval);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Raster operation on FPix* (%fpixs) giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a l_int32 (dx).
 * Arg #3 is expected to be a l_int32 (dy).
 * Arg #4 is expected to be a l_int32 (dw).
 * Arg #5 is expected to be a l_int32 (dh).
 * Arg #6 is expected to be a FPix* (fpixs).
 * Arg #7 is expected to be a l_int32 (sx).
 * Arg #8 is expected to be a l_int32 (sy).
 *
 * Leptonica's Notes:
 *      (1) This is similar in structure to pixRasterop(), except
 *          it only allows copying from the source into the destination.
 *          For that reason, no op code is necessary.  Additionally,
 *          all pixels are 32 bit words (float values), which makes
 *          the copy very simple.
 *      (2) Clipping of both src and dest fpix are done automatically.
 *      (3) This allows in-place copying, without checking to see if
 *          the result is valid:  use for in-place with caution!
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Rasterop(lua_State *L)
{
    LL_FUNC("Rasterop");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    l_int32 dx = ll_check_l_int32(_fun, L, 2);
    l_int32 dy = ll_check_l_int32(_fun, L, 3);
    l_int32 dw = ll_check_l_int32(_fun, L, 4);
    l_int32 dh = ll_check_l_int32(_fun, L, 5);
    FPix *fpixs = ll_check_FPix(_fun, L, 6);
    l_int32 sx = ll_check_l_int32(_fun, L, 7);
    l_int32 sy = ll_check_l_int32(_fun, L, 8);
    return ll_push_boolean(_fun, L, 0 == fpixRasterop(fpixd, dx, dy, dw, dh, fpixs, sx, sy));
}

/**
 * \brief Read a FPix* (%fpix) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    FPix *fpix = fpixRead(filename);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Read a FPix* (%fpix) from a lstring (%data, %size).
 * <pre>
 * Arg #1 is expected to be a lstring (data).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    FPix *fpix = fpixReadMem(data, size);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Read a FPix* (%fpix) from a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    FPix *fpix = fpixReadStream(stream->f);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Remove border (%left, %right, %top, %bot) from FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bot).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
RemoveBorder(lua_State *L)
{
    LL_FUNC("RemoveBorder");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bot = ll_check_l_int32(_fun, L, 5);
    FPix *fpix = fpixRemoveBorder(fpixs, left, right, top, bot);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Render contours (%incr, %proxim) of a FPix* (%fpixs) giving a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_float32 (incr).
 * Arg #3 is expected to be a l_float32 (proxim).
 *
 * Leptonica's Notes:
 *      (1) Values are displayed when val/incr is within +-proxim
 *          to an integer.  The default value is 0.15; smaller values
 *          result in thinner contour lines.
 *      (2) Negative values are rendered in red; positive values as black.
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
RenderContours(lua_State *L)
{
    LL_FUNC("RenderContours");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_float32 incr = ll_check_l_float32(_fun, L, 2);
    l_float32 proxim = ll_check_l_float32(_fun, L, 3);
    Pix *pix = fpixRenderContours(fpixs, incr, proxim);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Resize image data of FPix* (%fpixd) to that of another FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) If the data sizes differ, this destroys the existing
 *          data in fpixd and allocates a new, uninitialized, data array
 *          of the same size as the data in fpixs.  Otherwise, this
 *          doesn't do anything.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ResizeImageData(lua_State *L)
{
    LL_FUNC("ResizeImageData");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixResizeImageData(fpixd, fpixs));
}

/**
 * \brief Rotate FPix* (%fpixs) by 180 degrees giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixd).
 * Arg #2 is expected to be a FPix* (fpixs).
 *
 * Leptonica's Notes:
 *      (1) This does a 180 rotation of the image about the center,
 *          which is equivalent to a left-right flip about a vertical
 *          line through the image center, followed by a top-bottom
 *          flip about a horizontal line through the image center.
 *      (2) There are 3 cases for input:
 *          (a) fpixd == null (creates a new fpixd)
 *          (b) fpixd == fpixs (in-place operation)
 *          (c) fpixd != fpixs (existing fpixd)
 *      (3) For clarity, use these three patterns, respectively:
 *          (a) fpixd = fpixRotate180(NULL, fpixs);
 *          (b) fpixRotate180(fpixs, fpixs);
 *          (c) fpixRotate180(fpixd, fpixs);
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
Rotate180(lua_State *L)
{
    LL_FUNC("Rotate180");
    FPix *fpixd = ll_opt_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixRotate180(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Rotate FPix* (%fpixs) by 90 degrees cw giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (direction).
 *
 * Leptonica's Notes:
 *      (1) This does a 90 degree rotation of the image about the center,
 *          either cw or ccw, returning a new pix.
 *      (2) The direction must be either 1 (cw) or -1 (ccw).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
Rotate90(lua_State *L)
{
    LL_FUNC("Rotate90");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 direction = ll_opt_l_int32(_fun, L, 2, 1);
    FPix *fpixd = fpixRotate90(fpixs, direction);
    return ll_push_FPix(_fun, L, fpixd);
}

/**
 * \brief Rotate orthogonally FPix* (%fpixs) by rotation (%quads) giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a string describing the angle (quads).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 quads = ll_check_rotation(_fun, L, 2, 0);
    FPix *fpixd = fpixRotateOrth(fpixs, quads);
    return ll_push_FPix(_fun, L, fpixd);
}

/**
 * \brief Scale FPix* (%fpixs) by an integer factor (%factor).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (factor).
 *
 * Leptonica's Notes:
 *      (1) The width wd of fpixd is related to ws of fpixs by:
 *              wd = factor * (ws - 1) + 1   (and ditto for the height)
 *          We avoid special-casing boundary pixels in the interpolation
 *          by constructing fpixd by inserting (factor - 1) interpolated
 *          pixels between each pixel in fpixs.  Then
 *               wd = ws + (ws - 1) * (factor - 1)    (same as above)
 *          This also has the advantage that if we subsample by %factor,
 *          throwing out all the interpolated pixels, we regain the
 *          original low resolution fpix.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
ScaleByInteger(lua_State *L)
{
    LL_FUNC("ScaleByInteger");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    FPix *result = fpixScaleByInteger(fpixs, factor);
    return ll_push_FPix(_fun, L, result);
}

/**
 * \brief Set all pixels of FPix* (%fpix) to arbitrary value (%inval).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_float32 (inval).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetAllArbitrary(lua_State *L)
{
    LL_FUNC("SetAllArbitrary");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 inval = ll_check_l_float32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixSetAllArbitrary(fpix, inval));
}

/**
 * \brief Set image data of FPix* (%fpix) to a 2D array of l_float32 (%data).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetData(lua_State *L)
{
    LL_FUNC("SetData");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 w, h;
    l_int32 wpl = fpixGetWpl(fpix);
    l_float32* data = nullptr;
    if (fpixGetDimensions(fpix, &w, &h))
        return ll_push_nil(L);
    data = ll_calloc<l_float32>(_fun, L, h * wpl);
    if (!ll_unpack_Farray_2d(_fun, L, 2, data, wpl, h))
        return ll_push_nil(L);
    ll_push_boolean(_fun, L, 0 == fpixSetData(fpix, data));
    ll_free(data);
    return 1;
}

/**
 * \brief Set dimensions (%w, %h) of a FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetDimensions(lua_State *L)
{
    LL_FUNC("SetDimensions");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == fpixSetDimensions(fpix, w, h));
}

/**
 * \brief Set pixel at (%x, %y) in FPix* (%fpix) to value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_float32 (val).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetPixel(lua_State *L)
{
    LL_FUNC("SetPixel");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float32 val = ll_check_l_float32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == fpixSetPixel(fpix, x, y, val));
}

/**
 * \brief Set the resolution (%xres, %yres) of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (xres).
 * Arg #3 is expected to be a l_int32 (yres).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetResolution(lua_State *L)
{
    LL_FUNC("SetResolution");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32(_fun, L, 2);
    l_int32 yres = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == fpixSetResolution(fpix, xres, yres));
}

/**
 * \brief Set words-per-line (%wpl) of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (wpl).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetWpl(lua_State *L)
{
    LL_FUNC("SetWpl");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_int32 wpl = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixSetWpl(fpix, wpl));
}

/**
 * \brief Threshold FPix* (%fpix) using value (%thresh) to a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_float32 (thresh).
 *
 * Leptonica's Notes:
 *      (1) For all values of fpix that are <= thresh, sets the pixel
 *          in pixd to 1.
 * </pre>
 * \param L Lua state.
 * \return 1 Pix* on the Lua stack.
 */
static int
ThresholdToPix(lua_State *L)
{
    LL_FUNC("ThresholdToPix");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    Pix *pix = fpixThresholdToPix(fpix, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Write FPix* (%fpix) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixWrite(filename, fpix));
}

/**
 * \brief Write FPix* (%fpix) to a lstring (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 *
 * Leptonica's Notes:
 *      (1) Serializes a fpix in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 lstring on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (fpixWriteMem(&data, &size, fpix))
        return ll_push_nil(L);
    ll_push_bytes(_fun, L, data, size);
    return 1;
}

/**
 * \brief Write FPix* (%fpix) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 result = fpixWriteStream(stream->f, fpix);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class FPix*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data.
 */
FPix *
ll_check_FPix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<FPix>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a FPix* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data.
 */
FPix *
ll_opt_FPix(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_FPix(_fun, L, arg);
}

/**
 * \brief Push FPix* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_FPix
 * \return 1 FPix* on the Lua stack.
 */
int
ll_push_FPix(const char *_fun, lua_State *L, FPix *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new FPix*.
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
int
ll_new_FPix(lua_State *L)
{
    FUNC("ll_new_FPix");
    FPix* fpix = nullptr;
    l_int32 width = 1;
    l_int32 height = 1;

    if (ll_isudata(_fun, L, 1, LL_FPIX)) {
        FPix *fpixs = ll_opt_FPix(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(fpixs));
        fpix = fpixCreateTemplate(fpixs);
    }

    if (!fpix && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        fpix = fpixReadStream(stream->f);
    }

    if (!fpix && ll_isinteger(_fun, L, 1) && ll_isinteger(_fun, L, 2)) {
        width = ll_opt_l_int32(_fun, L, 1, width);
        height = ll_opt_l_int32(_fun, L, 2, height);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", width, "height", height);
        fpix = fpixCreate(width, height);
    }

    if (!fpix && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        fpix = fpixRead(filename);
    }

    if (!fpix && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        fpix = fpixReadMem(data, size);
    }

    if (!fpix) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", width, "height", height);
        fpix = fpixCreate(width, height);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(fpix));
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Register the FPix methods and functions in the FPix meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_FPix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_FPix},
        {"__tostring",              toString},
        {"AddBorder",               AddBorder},
        {"AddContinuedBorder",      AddContinuedBorder},
        {"AddMirroredBorder",       AddMirroredBorder},
        {"AddMultConstant",         AddMultConstant},
        {"AddSlopeBorder",          AddSlopeBorder},
        {"Affine",                  Affine},
        {"AffinePta",               AffinePta},
        {"AutoRenderContours",      AutoRenderContours},
        {"ChangeRefcount",          ChangeRefcount},
        {"Clone",                   Clone},
        {"ConvertToDPix",           ConvertToDPix},
        {"ConvertToPix",            ConvertToPix},
        {"Convolve",                Convolve},
        {"ConvolveSep",             ConvolveSep},
        {"Copy",                    Copy},
        {"CopyResolution",          CopyResolution},
        {"Create",                  Create},
        {"CreateTemplate",          CreateTemplate},
        {"Destroy",                 Destroy},
        {"DisplayMaxDynamicRange",  DisplayMaxDynamicRange},
        {"EndianByteSwap",          EndianByteSwap},
        {"FlipLR",                  FlipLR},
        {"FlipTB",                  FlipTB},
        {"GetData",                 GetData},
        {"GetDimensions",           GetDimensions},
        {"GetMax",                  GetMax},
        {"GetMin",                  GetMin},
        {"GetPixel",                GetPixel},
        {"GetRefcount",             GetRefcount},
        {"GetResolution",           GetResolution},
        {"GetWpl",                  GetWpl},
        {"LinearCombination",       LinearCombination},
        {"PrintStream",             PrintStream},
        {"Projective",              Projective},
        {"ProjectivePta",           ProjectivePta},
        {"Rasterop",                Rasterop},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveBorder",            RemoveBorder},
        {"RenderContours",          RenderContours},
        {"ResizeImageData",         ResizeImageData},
        {"Rotate180",               Rotate180},
        {"Rotate90",                Rotate90},
        {"RotateOrth",              RotateOrth},
        {"ScaleByInteger",          ScaleByInteger},
        {"SetAllArbitrary",         SetAllArbitrary},
        {"SetData",                 SetData},
        {"SetDimensions",           SetDimensions},
        {"SetPixel",                SetPixel},
        {"SetResolution",           SetResolution},
        {"SetWpl",                  SetWpl},
        {"ThresholdToPix",          ThresholdToPix},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_FPix);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
