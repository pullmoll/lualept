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
 *  Lua class DPix
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_DPIX */
#define LL_FUNC(x) FUNC(LL_DPIX "." x)


/**
 * \brief Destroy a DPix*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    DPix **pdpix = ll_check_udata<DPix>(_fun, L, 1, LL_DPIX);
    DPix *dpix = *pdpix;
    DBG(LOG_DESTROY, "%s: '%s' pdpix=%p dpix=%p refcount=%d\n",
        _fun, LL_DPIX, pdpix, dpix, dpixGetRefcount(dpix));
    dpixDestroy(&dpix);
    *pdpix = nullptr;
    return 0;
}

/**
 * \brief Create a new DPix*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DPix* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    DPix *dpix = nullptr;
    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
            l_int32 width = ll_check_l_int32_default(_fun, L, 1, 1);
            l_int32 height = ll_check_l_int32_default(_fun, L, 2, 1);
            dpix = dpixCreate(width, height);
    } else if (lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        dpix = dpixRead(filename);
    } else if (ll_check_DPix_opt(_fun, L, 1)) {
        DPix *dpixs = ll_check_DPix(_fun, L, 1);
        dpix = dpixCreateTemplate(dpixs);
    } else if (luaL_checkudata(L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        dpix = dpixReadStream(stream->f);
    }
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Printable string for a DPix*.
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    DPix *pix = ll_check_DPix(_fun, L, 1);
    luaL_Buffer B;
    void *data;
    l_int32 w, h, wpl, refcnt, xres, yres;
    long size;

    luaL_buffinit(L, &B);
    if (!pix) {
        luaL_addstring(&B, "nil");
    } else {
        if (dpixGetDimensions(pix, &w, &h)) {
            snprintf(str, sizeof(str), "invalid");
        } else {
            wpl = dpixGetWpl(pix);
            size = static_cast<long>(sizeof(l_float64)) * wpl * h;
            data = dpixGetData(pix);
            refcnt = dpixGetRefcount(pix);
            dpixGetResolution(pix, &xres, &yres);
            snprintf(str, sizeof(str),
                     LL_DPIX ": %p\n"
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_float64 (addc).
 * Arg #3 is expected to be a l_float64 (multc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddMultConstant(lua_State *L)
{
    LL_FUNC("AddMultConstant");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 addc = ll_check_l_float64(_fun, L, 2);
    l_float64 multc = ll_check_l_float64(_fun, L, 3);
    return ll_push_bool(_fun, L, 0 == dpixAddMultConstant(dpix, addc, multc));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dpixChangeRefcount(dpix, delta));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DPix* on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * Arg #2 is expected to be a l_int32 (outdepth).
 * Arg #3 is expected to be a l_int32 (negvals).
 * Arg #4 is expected to be a l_int32 (errorflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertToPix(lua_State *L)
{
    LL_FUNC("ConvertToPix");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 2);
    l_int32 negvals = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_int32 errorflag = ll_check_boolean_default(_fun, L, 4, FALSE);
    Pix *pix = dpixConvertToPix(dpixs, outdepth, negvals, errorflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyResolution(lua_State *L)
{
    LL_FUNC("CopyResolution");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dpixCopyResolution(dpixd, dpixs));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 2D table array of l_float64 on the Lua stack
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
    return ll_push_Darray_2d(_fun, L, data, wpl, h);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 l_int32 (%w, %h) on the Lua stack
 */
static int
GetDimensions(lua_State *L)
{
    LL_FUNC("GetDimensions");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (dpixGetDimensions(dpix, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 value (%maxval) and coordinates (%xmaxloc, %ymaxloc) on the Lua stack
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
        return ll_push_nil(L);
    ll_push_l_float64(_fun, L, maxval);
    ll_push_l_int32(_fun, L, xmaxloc);
    ll_push_l_int32(_fun, L, ymaxloc);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 value (%minval) and coordinates (%xminloc, %yminloc) on the Lua stack
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
        return ll_push_nil(L);
    ll_push_l_float64(_fun, L, minval);
    ll_push_l_int32(_fun, L, xminloc);
    ll_push_l_int32(_fun, L, yminloc);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_float64 on the Lua stack
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
        return ll_push_nil(L);
    return ll_push_l_float64(_fun, L, val);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    return ll_push_l_int32(_fun, L, dpixGetRefcount(dpix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
GetResolution(lua_State *L)
{
    LL_FUNC("GetResolution");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 xres = 0;
    l_int32 yres = 0;
    if (dpixGetResolution(dpix, &xres, &yres))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xres);
    ll_push_l_int32(_fun, L, yres);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs1).
 * Arg #3 is expected to be a DPix* (dpixs2).
 * Arg #4 is expected to be a l_float32 (a).
 * Arg #5 is expected to be a l_float32 (b).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DPix* on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    DPix *dpix = dpixReadMem(data, size);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixd).
 * Arg #2 is expected to be a DPix* (dpixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ResizeImageData(lua_State *L)
{
    LL_FUNC("ResizeImageData");
    DPix *dpixd = ll_check_DPix(_fun, L, 1);
    DPix *dpixs = ll_check_DPix(_fun, L, 2);
    l_int32 result = dpixResizeImageData(dpixd, dpixs);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ScaleByInteger(lua_State *L)
{
    LL_FUNC("ScaleByInteger");
    DPix *dpixs = ll_check_DPix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    DPix *result = dpixScaleByInteger(dpixs, factor);
    return ll_push_DPix(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_float64 (inval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
SetAllArbitrary(lua_State *L)
{
    LL_FUNC("SetAllArbitrary");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_float64 inval = ll_check_l_float64(_fun, L, 2);
    l_int32 result = dpixSetAllArbitrary(dpix, inval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 2D table array of l_float64 on the Lua stack
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
        return ll_push_nil(L);
    l_float64 *data = ll_calloc<l_float64>(_fun, L, wpl / 2 * h);
    if (!ll_unpack_Darray_2d(_fun, L, 2, data, wpl, h))
        return ll_push_nil(L);
    if (dpixSetData(dpix, data))
        return ll_push_nil(L);
    ll_push_Darray_2d(_fun, L, data, wpl, h);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
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
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    return ll_push_bool(_fun, L, 0 == dpixSetDimensions(dpix, w, h));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_float64 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPixel(lua_State *L)
{
    LL_FUNC("SetPixel");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_float64 val = ll_check_l_float64(_fun, L, 4);
    return ll_push_bool(_fun, L, 0 == dpixSetPixel(dpix, x, y, val));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
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
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32(_fun, L, 2);
    l_int32 yres = ll_check_l_int32(_fun, L, 3);
    return ll_push_bool(_fun, L, 0 == dpixSetResolution(dpix, xres, yres));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a l_int32 (wpl).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWpl(lua_State *L)
{
    LL_FUNC("SetWpl");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_int32 wpl = ll_check_l_int32(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dpixSetWpl(dpix, wpl));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 result = dpixWrite(filename, dpix);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (dpixWriteMem(&data, &size, dpix))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a DPix* (dpix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    DPix *dpix = ll_check_DPix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 result = dpixWriteStream(stream->f, dpix);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_DPIX.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DPix* contained in the user data
 */
DPix *
ll_check_DPix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<DPix>(_fun, L, arg, LL_DPIX);
}
/**
 * \brief Optionally expect a DPix* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the DPix* contained in the user data
 */
DPix *
ll_check_DPix_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_DPix(_fun, L, arg);
}
/**
 * \brief Push DPix* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_DPix
 * \return 1 DPix* on the Lua stack
 */
int
ll_push_DPix(const char *_fun, lua_State *L, DPix *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DPIX, cd);
}
/**
 * \brief Create and push a new DPix*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 DPix* on the Lua stack
 */
int
ll_new_DPix(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the DPix methods and functions in the LL_DPIX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_DPix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},   /* garbage collector */
        {"__new",                   Create},
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

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_DPIX);
    return ll_register_class(L, LL_DPIX, methods, functions);
}
