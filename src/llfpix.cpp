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
 *  Lua class FPix
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_FPIX */
#define LL_FUNC(x) FUNC(LL_FPIX "." x)

/**
 * \brief Destroy a FPix*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    FPix **pfpix = ll_check_udata<FPix>(_fun, L, 1, LL_FPIX);
    FPix *fpix = *pfpix;
    DBG(LOG_DESTROY, "%s: '%s' pfpix=%p fpix=%p\n",
        _fun, LL_FPIX, pfpix, fpix);
    fpixDestroy(&fpix);
    *pfpix = nullptr;
    return 0;
}

/**
 * \brief Create a new FPix*.
 *
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 *
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \brief Printable string for a FPix*.
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    FPix *pix = ll_check_FPix(_fun, L, 1);
    luaL_Buffer B;
    void *data;
    l_int32 w, h, wpl, refcnt, xres, yres;
    long size;

    luaL_buffinit(L, &B);
    if (!pix) {
        luaL_addstring(&B, "nil");
    } else {
        if (fpixGetDimensions(pix, &w, &h)) {
            snprintf(str, sizeof(str), "invalid");
        } else {
            wpl = fpixGetWpl(pix);
            size = static_cast<long>(sizeof(l_float32)) * wpl * h;
            data = fpixGetData(pix);
            refcnt = fpixGetRefcount(pix);
            fpixGetResolution(pix, &xres, &yres);
            snprintf(str, sizeof(str),
                     LL_FPIX ": %p\n"
                     "    width = %d, height = %d, wpl = %d\n"
                     "    data = %p, size = %#" PRIx64 "\n"
                     "    xres = %d, yres = %d, refcount = %d",
                     reinterpret_cast<void *>(pix),
                     w, h, wpl, data, size, xres, yres, refcnt);
        }
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddMultConstant(lua_State *L)
{
    LL_FUNC("AddMultConstant");
    FPix *fpix = ll_check_FPix(_fun, L, 1);
    l_float32 addc = ll_check_l_float32(_fun, L, 2);
    l_float32 multc = ll_check_l_float32(_fun, L, 3);
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 2 FPix* (%fpix) and l_float32 (%vc) on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
AffinePta(lua_State *L)
{
    LL_FUNC("AffinePta");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 border = ll_check_l_int32(_fun, L, 4);
    l_float32 inval = ll_check_l_float32(_fun, L, 5);
    FPix *fpix = fpixAffinePta(fpixs, ptad, ptas, border, inval);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Auto render (%ncontours) contours of the FPix* (%fpix) to a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a l_int32 (ncontours).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Convert the FPix* (%fpixs) to DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Convert the FPix* (%fpixs) to Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (outdepth).
 * Arg #3 is expected to be a string describing what to do with negative values (negvals).
 * Arg #4 is expected to be a boolean (errorflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertToPix(lua_State *L)
{
    LL_FUNC("ConvertToPix");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 2);
    l_int32 negvals = ll_check_negvals(_fun, L, 3, L_CLIP_TO_ZERO);
    l_int32 errorflag = ll_opt_boolean(_fun, L, 4, FALSE);
    Pix *pix = fpixConvertToPix(fpixs, outdepth, negvals, errorflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convolution of the FPix* (%fpixs) using Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a Kernel* (kel).
 * Arg #3 is expected to be a boolean (normflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
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
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * \brief Create a FPix* (%fpix) from a template FPix* (%fpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
EndianByteSwap(lua_State *L)
{
    LL_FUNC("EndianByteSwap");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
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
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
    return ll_push_Farray_2d(_fun, L, farray, wpl, h);
}

/**
 * \brief Get the dimensions (%w, %h) of FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 3 one number (%maxval) and 2 integers (%xmaxloc, %ymaxloc) on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 3 one number (%minval) and 2 integers (%xminloc, %yminloc) on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 l_float32 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 2 FPix* (%fpix) and l_float32 (%vc) on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
static int
Rotate180(lua_State *L)
{
    LL_FUNC("Rotate180");
    FPix *fpixd = ll_check_FPix(_fun, L, 1);
    FPix *fpixs = ll_check_FPix(_fun, L, 2);
    FPix *fpix = fpixRotate180(fpixd, fpixs);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Rotate FPix* (%fpixs) by 90 degrees cw giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
static int
Rotate90(lua_State *L)
{
    LL_FUNC("Rotate90");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 direction = ll_check_l_int32(_fun, L, 2);
    FPix *fpix = fpixRotate90(fpixs, direction);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Rotate orthogonally FPix* (%fpixs) by rotation (%quads) giving FPix* (%fpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (quads).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    FPix *fpixs = ll_check_FPix(_fun, L, 1);
    l_int32 quads = ll_check_rotation(_fun, L, 2, 0);
    FPix *fpix = fpixRotateOrth(fpixs, quads);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Scale FPix* (%fpixs) by an integer factor (%factor).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
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
    return ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
}

/**
 * \brief Write FPix* (%fpix) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPix* (fpix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * \brief Check Lua stack at index (%arg) for udata of class LL_FPIX.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data
 */
FPix *
ll_check_FPix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<FPix>(_fun, L, arg, LL_FPIX);
}

/**
 * \brief Optionally expect a FPix* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPix* contained in the user data
 */
FPix *
ll_opt_FPix(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_FPix(_fun, L, arg);
}

/**
 * \brief Push FPix* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_FPix
 * \return 1 FPix* on the Lua stack
 */
int
ll_push_FPix(const char *_fun, lua_State *L, FPix *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_FPIX, cd);
}

/**
 * \brief Create and push a new FPix*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
int
ll_new_FPix(lua_State *L)
{
    FUNC("ll_new_FPix");
    FPix* fpix = nullptr;

    if (lua_isuserdata(L, 1)) {
        FPix *fpixs = ll_opt_FPix(_fun, L, 1);
        if (fpixs) {
            DBG(LOG_NEW_CLASS, "%s: create for %s* = %p\n", _fun,
                LL_FPIX, reinterpret_cast<void *>(fpixs));
            fpix = fpixCreateTemplate(fpixs);
        } else {
            luaL_Stream *stream = ll_check_stream(_fun, L, 1);
            DBG(LOG_NEW_CLASS, "%s: create for %s* = %p\n", _fun,
                LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
            fpix = fpixReadStream(stream->f);
        }
    }

    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_opt_l_int32(_fun, L, 1, 1);
        l_int32 height = ll_opt_l_int32(_fun, L, 2, 1);
        DBG(LOG_NEW_CLASS, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", width, "height", height);
        fpix = fpixCreate(width, height);
    }

    if (!fpix && lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_CLASS, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        fpix = fpixRead(filename);
    }

    if (!fpix && lua_isstring(L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_CLASS, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        fpix = fpixReadMem(data, size);
    }

    if (!fpix) {
        DBG(LOG_NEW_CLASS, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", 1, "height", 1);
        fpix = fpixCreate(1, 1);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        LL_FPIX, reinterpret_cast<void *>(fpix));
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Register the FPix methods and functions in the LL_FPIX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_FPix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},           /* garbage collector */
        {"__new",                   ll_new_FPix},       /* FPix() */
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

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_FPix);
    lua_setglobal(L, LL_FPIX);
    return ll_register_class(L, LL_FPIX, methods, functions);
}
