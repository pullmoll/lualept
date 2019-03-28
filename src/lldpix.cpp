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
 * \file lldpix.cpp
 * \class DPix
 *
 * A 2-D pixels array of doubles (l_float64).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DPIX

/** Define a function's name (_fun) with prefix DPix */
#define LL_FUNC(x) FUNC(TNAME "." x)


/**
 * \brief Destroy a DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    DPix *dpix = ll_take_udata<DPix>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "dpix", reinterpret_cast<void *>(dpix),
        "refcount", dpixGetRefcount(dpix));
    dpixDestroy(&dpix);
    return 0;
}

/**
 * \brief Printable string for a DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    luaL_Buffer B;
    void *data;
    l_int32 w, h, wpl, refcnt, xres, yres;
    long size;

    luaL_buffinit(L, &B);
    if (!dpix) {
        luaL_addstring(&B, "nil");
    } else if (dpixGetDimensions(dpix, &w, &h)) {
            luaL_addstring(&B, "invalid");
    } else {

        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(dpix));
        luaL_addstring(&B, str);
        wpl = dpixGetWpl(dpix);
        snprintf(str, LL_STRBUFF,
                 "\n    %s = %d, %s = %d, %s = %d",
                 "width", w,
                 "height", h,
                 "wpl", wpl);
        luaL_addstring(&B, str);
        refcnt = dpixGetRefcount(dpix);
        dpixGetResolution(dpix, &xres, &yres);
        snprintf(str, LL_STRBUFF,
                 "\n    %s = %d, %s = %d, %s = %d",
                 "xres", xres,
                 "yres", yres,
                 "refcnt", refcnt);
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        size = static_cast<long>(sizeof(l_float64)) * wpl * h;
        data = dpixGetData(dpix);
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
 * \brief Multiply by a constant (%multc), then add a constant (%addc) to each pixel of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_float64 (addc).
 * Arg #3 is expected to be a l_float64 (multc).
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
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 addc = ll_check_l_float64(_fun, L, 2);
    l_float64 multc = ll_check_l_float64(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == dpixAddMultConstant(dpix, addc, multc));
}

/**
 * \brief Modify the reference count of DPix* (%dpix) by a value (%delta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixChangeRefcount(dpix, delta));
}

/**
 * \brief Clone a DPix* (%dpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 *
 * Leptonica's Notes:
 *      (1) See pixClone() for definition and usage.
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* on the Lua stack.
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    DPix *dpix = dpixClone(dpixs);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Convert a DPix* (%dpix) to a FPix* (%fpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 FPix* on the Lua stack.
 */
static int
ConvertToFPix(lua_State *L)
{
    LL_FUNC("ConvertToFPix");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    FPix *fpix = dpixConvertToFPix(dpix);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Convert a DPix* (%dpix) to a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * Arg #2 is expected to be a l_int32 (outdepth).
 * Arg #3 is expected to be a l_int32 (negvals).
 * Arg #4 is expected to be a l_int32 (errorflag).
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
 * \return 1 Pix* on the Lua stack.
 */
static int
ConvertToPix(lua_State *L)
{
    LL_FUNC("ConvertToPix");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    l_int32 outdepth = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 negvals = ll_check_negvals(_fun, L, 3);
    l_int32 errorflag = ll_opt_boolean(_fun, L, 4);
    Pix *pix = dpixConvertToPix(dpixs, outdepth, negvals, errorflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Copy a DPix* (%dpixs) to a DPix* (%dpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 *
 * Leptonica's Notes:
 *      (1) There are three cases:
 *            (a) dpixd == null  (makes a new dpix; refcount = 1)
 *            (b) dpixd == dpixs  (no-op)
 *            (c) dpixd != dpixs  (data copy; no change in refcount)
 *          If the refcount of dpixd > 1, case (c) will side-effect
 *          these handles.
 *      (2) The general pattern of use is:
 *             dpixd = dpixCopy(dpixd, dpixs);
 *          This will work for all three cases.
 *          For clarity when the case is known, you can use:
 *            (a) dpixd = dpixCopy(NULL, dpixs);
 *            (c) dpixCopy(dpixd, dpixs);
 *      (3) For case (c), we check if dpixs and dpixd are the same size.
 *          If so, the data is copied directly.
 *          Otherwise, the data is reallocated to the correct size
 *          and the copy proceeds.  The refcount of dpixd is unchanged.
 *      (4) This operation, like all others that may involve a pre-existing
 *          dpixd, will side-effect any existing clones of dpixd.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    DPix *dpix = dpixCopy(dpixd, dpixs);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Copy the resolution from a DPix* (%dpixs) to a DPix* (%dpixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
CopyResolution(lua_State *L)
{
    LL_FUNC("CopyResolution");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixCopyResolution(dpixd, dpixs));
}

/**
 * \brief Create a new DPix* (%dpix) with width (%width) times height (%height) pixels.
 * <pre>
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 *
 * Leptonica's Notes:
 *      (1) Makes a DPix of specified size, with the data array
 *          allocated and initialized to 0.
 *      (2) The number of pixels must be less than 2^28.
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 width = ll_opt_l_int32(_fun, L, 1, 1);
    l_int32 height = ll_opt_l_int32(_fun, L, 2, 1);
    DPix *dpix = dpixCreate(width, height);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Create a DPix* (%dpix) of the same size as input DPix* (%dpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 *
 * Leptonica's Notes:
 *      (1) Makes a DPix of the same size as the input DPix, with the
 *          data array allocated and initialized to 0.
 *      (2) Copies the resolution.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
CreateTemplate(lua_State *L)
{
    LL_FUNC("CreateTemplate");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    DPix *dpix = dpixCreateTemplate(dpixs);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Swap bytes in the DPix* (%dpixs) data to little endian.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 *
 * Leptonica's Notes:
 *      (1) On big-endian hardware, this does byte-swapping on each of
 *          the 4-byte words in the dpix data.  On little-endians,
 *          the data is unchanged.  This is used for serialization
 *          of dpix; the data is serialized in little-endian byte
 *          order because most hardware is little-endian.
 *      (2) The operation can be either in-place or, if dpixd == NULL,
 *          a new dpix is made.  If not in-place, caller must catch
 *          the returned pointer.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
EndianByteSwap(lua_State *L)
{
    LL_FUNC("EndianByteSwap");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    DPix *dpix = dpixEndianByteSwap(dpixd, dpixs);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Get the DPix* (%dpix) data as a 2D table array of numbers.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 2D table array of l_float64 on the Lua stack.
 */
static int
GetData(lua_State *L)
{
    LL_FUNC("GetData");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 *data = dpixGetData(dpix);
    l_int32 wpl = dpixGetWpl(dpix);
    l_int32 w = 0;
    l_int32 h = 0;
    if (dpixGetDimensions(dpix, &w, &h))
        return ll_push_nil(_fun, L);
    return ll_pack_Darray_2d(_fun, L, data, wpl, h);
}

/**
 * \brief Get the dimensions (%w, %h) of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 2 l_int32 (%w, %h) on the Lua stack.
 */
static int
GetDimensions(lua_State *L)
{
    LL_FUNC("GetDimensions");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (dpixGetDimensions(dpix, &w, &h))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Get the maximum value (%maxval) and location (%xmaxloc, %ymaxloc) of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 3 value (%maxval) and location (%xmaxloc, %ymaxloc) on the Lua stack.
 */
static int
GetMax(lua_State *L)
{
    LL_FUNC("GetMax");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 maxval = 0;
    l_int32 xmaxloc = 0;
    l_int32 ymaxloc = 0;
    if (dpixGetMax(dpix, &maxval, &xmaxloc, &ymaxloc))
        return ll_push_nil(_fun, L);
    ll_push_l_float64(_fun, L, maxval);
    ll_push_l_int32(_fun, L, xmaxloc);
    ll_push_l_int32(_fun, L, ymaxloc);
    return 3;
}

/**
 * \brief Get the minimum value (%minval) and location (%xminloc, %yminloc) of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 3 value (%minval) and location (%xminloc, %yminloc) on the Lua stack.
 */
static int
GetMin(lua_State *L)
{
    LL_FUNC("GetMin");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 minval = 0;
    l_int32 xminloc = 0;
    l_int32 yminloc = 0;
    if (dpixGetMin(dpix, &minval, &xminloc, &yminloc))
        return ll_push_nil(_fun, L);
    ll_push_l_float64(_fun, L, minval);
    ll_push_l_int32(_fun, L, xminloc);
    ll_push_l_int32(_fun, L, yminloc);
    return 3;
}

/**
 * \brief Get the value (%val) of pixel at (%x, %y) from DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 l_float64 (%val) on the Lua stack.
 */
static int
GetPixel(lua_State *L)
{
    LL_FUNC("GetPixel");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float64 val = 0;
    if (dpixGetPixel(dpix, x, y, &val))
        return ll_push_nil(_fun, L);
    return ll_push_l_float64(_fun, L, val);
}

/**
 * \brief Get the reference count of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 l_int32 (%refcount) on the Lua stack.
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    return ll_push_l_int32(_fun, L, dpixGetRefcount(dpix));
}

/**
 * \brief Get the resolution (%xres, %yres) of DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 2 integers (%xres, %yres) on the Lua stack.
 */
static int
GetResolution(lua_State *L)
{
    LL_FUNC("GetResolution");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 xres = 0;
    l_int32 yres = 0;
    if (dpixGetResolution(dpix, &xres, &yres))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, xres);
    ll_push_l_int32(_fun, L, yres);
    return 2;
}

/**
 * \brief Get the words-per-line value (%wpl) for DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 integer (%wpl) on the Lua stack..
 */
static int
GetWpl(lua_State *L)
{
    LL_FUNC("GetWpl");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 result = dpixGetWpl(dpix);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Compute the pixelwise linear combination (%a) * (%dpixs1) + (%b) * (%dpixs2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs1).
 * Arg #3 is expected to be a DPix* (dpixs2).
 * Arg #4 is expected to be a l_float32 (a).
 * Arg #5 is expected to be a l_float32 (b).
 *
 * Leptonica's Notes:
 *      (1) Computes pixelwise linear combination: a * src1 + b * src2
 *      (2) Alignment is to UL corner.
 *      (3) There are 3 cases.  The result can go to a new dest,
 *          in-place to dpixs1, or to an existing input dest:
 *          * dpixd == null:   (src1 + src2) --> new dpixd
 *          * dpixd == dpixs1:  (src1 + src2) --> src1  (in-place)
 *          * dpixd != dpixs1: (src1 + src2) --> input dpixd
 *      (4) dpixs2 must be different from both dpixd and dpixs1.
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* (%dpix) on the Lua stack.
 */
static int
LinearCombination(lua_State *L)
{
    LL_FUNC("LinearCombination");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs1 = ll_check_DPix(_fun, L, 2);
    DPix *dpixs2 = ll_check_DPix(_fun, L, 3);
    l_float32 a = ll_check_l_float32(_fun, L, 4);
    l_float32 b = ll_check_l_float32(_fun, L, 5);
    DPix *dpix = dpixLinearCombination(dpixd, dpixs1, dpixs2, a, b);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Read a DPix* (%dpix) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* (%dpix) on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    DPix *result = dpixRead(filename);
    return ll_push_DPix(_fun, L, result);
}

/**
 * \brief Read a DPix* (%dpix) from memory (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* (%dpix) on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    DPix *dpix = dpixReadMem(data, size);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Read a DPix* (%dpix) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* (%dpix) on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    DPix *result = dpixReadStream(stream->f);
    return ll_push_DPix(_fun, L, result);
}

/**
 * \brief Resize the image data of DPix* (%dpixd) to that of DPix* (%dpixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ResizeImageData(lua_State *L)
{
    LL_FUNC("ResizeImageData");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixResizeImageData(dpixd, dpixs));
}

/**
 * \brief Scale a DPix* (%dpixs) by an integer factor (%factor).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * Arg #2 is expected to be a l_int32 (factor).
 *
 * Leptonica's Notes:
 *      (1) The width wd of dpixd is related to ws of dpixs by:
 *              wd = factor * (ws - 1) + 1   (and ditto for the height)
 *          We avoid special-casing boundary pixels in the interpolation
 *          by constructing fpixd by inserting (factor - 1) interpolated
 *          pixels between each pixel in fpixs.  Then
 *               wd = ws + (ws - 1) * (factor - 1)    (same as above)
 *          This also has the advantage that if we subsample by %factor,
 *          throwing out all the interpolated pixels, we regain the
 *          original low resolution dpix.
 * </pre>
 * \param L Lua state.
 * \return 1 DPix* (%dpixd) on the Lua stack.
 */
static int
ScaleByInteger(lua_State *L)
{
    LL_FUNC("ScaleByInteger");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    DPix *dpixd = dpixScaleByInteger(dpixs, factor);
    return ll_push_DPix(_fun, L, dpixd);
}

/**
 * \brief Set all pixels in DPix* (%dpix) to arbitrary value (%inval).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_float64 (inval).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetAllArbitrary(lua_State *L)
{
    LL_FUNC("SetAllArbitrary");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 inval = ll_check_l_float64(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixSetAllArbitrary(dpix, inval));
}

/**
 * \brief Set data in a DPix (%dpix) from a 2D table array (%data, %wpl, %h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L Lua state.
 * \return 1 2D table array of l_float64 on the Lua stack.
 */
static int
SetData(lua_State *L)
{
    LL_FUNC("SetData");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 wpl = dpixGetWpl(dpix);
    l_int32 w = 0;
    l_int32 h = 0;
    if (dpixGetDimensions(dpix, &w, &h))
        return ll_push_nil(_fun, L);
    l_float64 *data = ll_unpack_Darray_2d(_fun, L, 2, wpl, h);
    if (nullptr == data)
        return ll_push_nil(_fun, L);
    if (dpixSetData(dpix, data))
        return ll_push_nil(_fun, L);
    ll_free(data);
    return 1;
}

/**
 * \brief Set the dimensions of DPix* (%dpix) to width (%w) and height (%h).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
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
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == dpixSetDimensions(dpix, w, h));
}

/**
 * \brief Set a pixel at (%x, %y) in DPix* (%dpix) to value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_float64 (val).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetPixel(lua_State *L)
{
    LL_FUNC("SetPixel");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float64 val = ll_check_l_float64(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == dpixSetPixel(dpix, x, y, val));
}

/**
 * \brief Set the resolution (%xres, %yres) for DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
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
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32(_fun, L, 2);
    l_int32 yres = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == dpixSetResolution(dpix, xres, yres));
}

/**
 * \brief Set the words-per-line (%wpl) for DPix* (%dpix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (wpl).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetWpl(lua_State *L)
{
    LL_FUNC("SetWpl");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 wpl = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixSetWpl(dpix, wpl));
}

/**
 * \brief Write a DPix* (%dpix) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixWrite(filename, dpix));
}

/**
 * \brief Write a DPix* (%dpix) to memory (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 *
 * Leptonica's Notes:
 *      (1) Serializes a dpix in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 2 on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (dpixWriteMem(&data, &size, dpix))
        return ll_push_nil(_fun, L);
    ll_push_bytes(_fun, L, data, size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write a DPix* (%dpix) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == dpixWriteStream(stream->f, dpix));
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class DPix*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DPix* contained in the user data.
 */
DPix *
ll_check_DPix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DPix>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a DPix* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DPix* contained in the user data.
 */
DPix *
ll_opt_DPix(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_DPix(_fun, L, arg);
}

/**
 * \brief Push DPix* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_DPix
 * \return 1 DPix* on the Lua stack.
 */
int
ll_push_DPix(const char *_fun, lua_State *L, DPix *cd)
{
    if (!cd)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new DPix*.
 * \param L Lua state.
 * \return 1 DPix* on the Lua stack.
 */
int
ll_new_DPix(lua_State *L)
{
    FUNC("ll_new_DPix");
    DPix *dpix = nullptr;
    l_int32 width = 1;
    l_int32 height = 1;

    if (ll_isudata(_fun, L, 1, LL_DPIX)) {
        DPix *dpixs = ll_opt_DPix(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(dpixs));
        dpix = dpixCreateTemplate(dpixs);
    }

    if (!dpix && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        dpix = dpixReadStream(stream->f);
    }

    if (!dpix && ll_isinteger(_fun, L, 1) && ll_isinteger(_fun, L, 2)) {
        width = ll_opt_l_int32(_fun, L, 1, width);
        height = ll_opt_l_int32(_fun, L, 2, height);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", width, "height", height);
        dpix = dpixCreate(width, height);
    }

    if (!dpix && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        dpix = dpixRead(filename);
    }

    if (!dpix && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        dpix = dpixReadMem(data, size);
    }

    if (!dpix) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "width", width, "height", height);
        dpix = dpixCreate(1,1);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(dpix));
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Register the DPix methods and functions in the DPix meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_DPix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_DPix},
        {"__tostring",              toString},
        {"AddMultConstant",         AddMultConstant},
        {"ChangeRefcount",          ChangeRefcount},
        {"Clone",                   Clone},
        {"ConvertToFPix",           ConvertToFPix},
        {"ConvertToPix",            ConvertToPix},
        {"Copy",                    Copy},
        {"CopyResolution",          CopyResolution},
        {"Create",                  Create},
        {"Create",                  Create},
        {"CreateTemplate",          CreateTemplate},
        {"Destroy",                 Destroy},
        {"Destroy",                 Destroy},
        {"EndianByteSwap",          EndianByteSwap},
        {"GetData",                 GetData},
        {"GetDimensions",           GetDimensions},
        {"GetMax",                  GetMax},
        {"GetMin",                  GetMin},
        {"GetPixel",                GetPixel},
        {"GetRefcount",             GetRefcount},
        {"GetResolution",           GetResolution},
        {"GetWpl",                  GetWpl},
        {"LinearCombination",       LinearCombination},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"ResizeImageData",         ResizeImageData},
        {"ScaleByInteger",          ScaleByInteger},
        {"SetAllArbitrary",         SetAllArbitrary},
        {"SetData",                 SetData},
        {"SetDimensions",           SetDimensions},
        {"SetPixel",                SetPixel},
        {"SetResolution",           SetResolution},
        {"SetWpl",                  SetWpl},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_DPix);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
