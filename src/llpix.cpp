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

#if !defined(PATH_MAX)
#ifdef _MSC_VER
#define PATH_MAX    266
#else
#define PATH_MAX    4096
#endif
#endif

/** Table of bit counts in a byte */
static l_int32 tab8[256];

/*====================================================================*
 *
 *  Lua class Pix
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_PIX */
#define LL_FUNC(x) FUNC(LL_PIX "." x)

/**
 * \brief Destroy a Pix*.
 * <pre>
 * Arg #1 is expected to be Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pix **ppix = ll_check_udata<Pix>(_fun, L, 1, LL_PIX);
    Pix *pix = *ppix;
    DBG(LOG_DESTROY, "%s: '%s' ppix=%p pix=%p refcount=%d\n",
        _fun, LL_PIX, ppix, pix, pixGetRefcount(pix));
    pixDestroy(&pix);
    *ppix = nullptr;
    return 0;
}

/**
 * \brief Create a new Pix*.
 * <pre>
 * Arg #1 is expected to be Pix*.
 * or
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1).
 * or
 * Arg #1 is expected to be a string (filename).
 * or
 * No Arg creates a 1x1 1bpp Pix*
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix *pixs = ll_check_Pix_opt(_fun, L, 1);
    Pix *pix = nullptr;
    if (pixs) {
        pix = pixCreateTemplate(pixs);
    } else if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32_default(_fun, L, 1, 1);
        l_int32 height = ll_check_l_int32_default(_fun, L, 2, 1);
        l_int32 depth = ll_check_l_int32_default(_fun, L, 3, 1);
        pix = pixCreate(width, height, depth);
    } else if (lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        pix = pixRead(filename);
    } else {
        pix = pixCreate(1, 1, 1);
    }
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Subtract the Pix* (%pix2) from Pix* (%pix1).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Subtract(lua_State *L)
{
    LL_FUNC("Subtract");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixSubtract(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Invert the Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is optional and, if given, expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Invert(lua_State *L)
{
    LL_FUNC("Invert");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix_opt(_fun, L, 2);
    Pix *pix = pixInvert(pixs ? nullptr : pixd, pixs ? pixs : pixd);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief And the Pix* (%pix1) and Pix* (%pix2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
And(lua_State *L)
{
    LL_FUNC("And");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixAnd(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Or the Pix* (%pix1) and Pix* (%pix2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Or(lua_State *L)
{
    LL_FUNC("Or");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixOr(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Xor the Pix* (%pix1) and Pix* (%pix2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Xor(lua_State *L)
{
    LL_FUNC("Xor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixXor(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Printable string for a Pix*.
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Buffer B;
    const char* format = nullptr;
    const char* text = nullptr;
    void *data;
    l_int32 w, h, d, spp, wpl, refcnt, ccnt, ctot, xres, yres;
    long size;
    PixColormap *cmap = nullptr;

    luaL_buffinit(L, &B);
    if (!pix) {
        luaL_addstring(&B, "nil");
    } else {
        if (pixGetDimensions(pix, &w, &h, &d)) {
            snprintf(str, sizeof(str), "invalid");
        } else {
            spp = pixGetSpp(pix);
            wpl = pixGetWpl(pix);
            size = static_cast<long>(sizeof(l_uint32)) * wpl * h;
            data = pixGetData(pix);
            refcnt = pixGetRefcount(pix);
            xres = pixGetXRes(pix);
            yres = pixGetYRes(pix);
            format = ll_string_input_format(pixGetInputFormat(pix));
            snprintf(str, sizeof(str),
                     "Pix: %p\n"
                     "    width = %d, height = %d, depth = %d, spp = %d\n"
                     "    wpl = %d, data = %p, size = %#" PRIx64 "\n"
                                                                 "    xres = %d, yres = %d, refcount = %d\n", reinterpret_cast<void *>(pix),
                     w, h, d, spp, wpl, data, size, xres, yres, refcnt);
        }
        luaL_addstring(&B, str);

        cmap = pixGetColormap(pix);
        if (cmap) {
            ccnt = pixcmapGetCount(cmap);
            ctot = pixcmapGetFreeCount(cmap) + ccnt;
            snprintf(str, sizeof(str),
                     "    colormap: %d of %d colors\n", ccnt, ctot);
        } else {
            snprintf(str, sizeof(str),
                     "    no colormap\n");
        }
        luaL_addstring(&B, str);

        text = pixGetText(pix);
        if (text) {
            snprintf(str, sizeof(str),
                     "    text: %s", text);
        } else {
            snprintf(str, sizeof(str), "    no text");
        }
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Build the absolute difference by column of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffByColumn(lua_State *L)
{
    LL_FUNC("AbsDiffByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixAbsDiffByColumn(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the absolute difference by row of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffByRow(lua_State *L)
{
    LL_FUNC("AbsDiffByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixAbsDiffByRow(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the absolute difference inside a Box* (%box) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * Arg #2 is optional and, if given, expected to be a string (dir: horizontal-line or vertical-line).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffInRect(lua_State *L)
{
    LL_FUNC("AbsDiffInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 dir = ll_check_direction(_fun, L, 3, L_HORIZONTAL_LINE);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffInRect(pixs, box, dir, &absdiff))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, absdiff);
    return 1;
}

/**
 * \brief Build absolute difference on a line (%x1,%y1 to %x2,%y2) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffOnLine(lua_State *L)
{
    LL_FUNC("AbsDiffOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffOnLine(pixs, x1, y1, x2, y2, &absdiff))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, absdiff);
    return 1;
}

/**
 * \brief Absolute difference between Pix* (%pixs1) and another Pix* (%pixs2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AbsDifference(lua_State *L)
{
    LL_FUNC("AbsDifference");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixAbsDifference(pixs1, pixs2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Accumulate(lua_State *L)
{
    LL_FUNC("Accumulate");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 op = ll_check_l_int32(_fun, L, 3);  /* FIXME: operations */
    return ll_push_boolean(_fun, L, 0 == pixAccumulate(pixd, pixs, op));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a Pta* (pta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
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
    if (pixaAccumulateSamples(pixa, pta, &pixd, &x, &y))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    ll_push_l_float32(_fun, L, x);
    ll_push_l_float32(_fun, L, y);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_float32 (gamma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AdaptThresholdToBinary(lua_State *L)
{
    LL_FUNC("AdaptThresholdToBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixAdaptThresholdToBinary(pixs, pixm, gamma);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_float32 (gamma).
 * Arg #4 is expected to be a l_int32 (blackval).
 * Arg #5 is expected to be a l_int32 (whiteval).
 * Arg #6 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AdaptThresholdToBinaryGen(lua_State *L)
{
    LL_FUNC("AdaptThresholdToBinaryGen");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    l_int32 blackval = ll_check_l_int32(_fun, L, 4);
    l_int32 whiteval = ll_check_l_int32(_fun, L, 5);
    l_int32 thresh = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixAdaptThresholdToBinaryGen(pixs, pixm, gamma, blackval, whiteval, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddAlphaTo1bpp(lua_State *L)
{
    LL_FUNC("AddAlphaTo1bpp");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixAddAlphaTo1bpp(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (fract).
 * Arg #3 is expected to be a l_int32 (invert).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddAlphaToBlend(lua_State *L)
{
    LL_FUNC("AddAlphaToBlend");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = ll_check_l_float32(_fun, L, 2);
    l_int32 invert = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixAddAlphaToBlend(pixs, fract, invert);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Add black or white border pixels in a Pix* to a new Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * Arg #6 is expected to be a string describing the operation (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBlackOrWhiteBorder(lua_State *L)
{
    LL_FUNC("AddBlackOrWhiteBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_int32 op = ll_check_getval(_fun, L, 6, L_GET_BLACK_VAL);
    Pix* pixd = pixAddBlackOrWhiteBorder(pixs, left, right, top, bottom, op);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Add border pixels in a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (npix).
 * Arg #3 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBorder(lua_State *L)
{
    LL_FUNC("AddBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 npix = ll_check_l_int32(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    Pix* pixd = pixAddBorder(pixs, npix, val);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Add border pixels general (%val) in a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * Arg #6 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBorderGeneral(lua_State *L)
{
    LL_FUNC("AddBorderGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_uint32 val = ll_check_l_uint32(_fun, L, 6);
    Pix* pixd = pixAddBorderGeneral(pixs, left, right, top, bottom, val);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
AddConstantGray(lua_State *L)
{
    LL_FUNC("AddConstantGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixAddConstantGray(pixs, val));
}

/**
 * \brief Add continued border pixels in a Pix* to a new Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddContinuedBorder(lua_State *L)
{
    LL_FUNC("AddContinuedBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pix = pixAddContinuedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Add Gaussian noise to Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (stdev).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddGaussianNoise(lua_State *L)
{
    LL_FUNC("AddGaussianNoise");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 stdev = ll_check_l_float32(_fun, L, 2);
    Pix *pix = pixAddGaussianNoise(pixs, stdev);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Add two gray Pix* (%pixs1, %pixs2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddGray(lua_State *L)
{
    LL_FUNC("AddGray");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    Pix *pix = pixAddGray(pixd, pixs1, pixs2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Add a gray colormap to Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddGrayColormap8(lua_State *L)
{
    LL_FUNC("AddGrayColormap8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixAddGrayColormap8(pixs));
}

/**
 * \brief Add a minimal gray colormap to Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddMinimalGrayColormap8(lua_State *L)
{
    LL_FUNC("AddMinimalGrayColormap8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixAddMinimalGrayColormap8(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Add mirrored border pixels in a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddMirroredBorder(lua_State *L)
{
    LL_FUNC("AddMirroredBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pixd = pixAddMirroredBorder(pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Add mixed border pixels in a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddMixedBorder(lua_State *L)
{
    LL_FUNC("AddMixedBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pix = pixAddMixedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Add two RGB Pix* (%pixs1, %pixs2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddRGB(lua_State *L)
{
    LL_FUNC("AddRGB");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixAddRGB(pixs1, pixs2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Add repeated border pixels in a Pix* to a new Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddRepeatedBorder(lua_State *L)
{
    LL_FUNC("AddRepeatedBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pixd = pixAddRepeatedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a string (textstr).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a l_int32 (location).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddSingleTextblock(lua_State *L)
{
    LL_FUNC("AddSingleTextblock");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Bmf *bmf = ll_check_Bmf(_fun, L, 2);
    const char *textstr = ll_check_string(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4);
    l_int32 location = ll_check_l_int32(_fun, L, 5);
    l_int32 overflow = 0;
    if (pixAddSingleTextblock(pixs, bmf, textstr, val, location, &overflow))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, overflow);
    return 1;
}

/**
 * \brief Add to the text of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (text).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddText(lua_State *L)
{
    LL_FUNC("AddText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, pixAddText(pix, text));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a string (textstr).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a l_int32 (location).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AddTextlines(lua_State *L)
{
    LL_FUNC("AddTextlines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Bmf *bmf = ll_check_Bmf(_fun, L, 2);
    const char *textstr = ll_check_string(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4);
    l_int32 location = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixAddTextlines(pixs, bmf, textstr, val, location);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pixa* (pixa).
 * Arg #3 is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddWithIndicator(lua_State *L)
{
    LL_FUNC("AddWithIndicator");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    Numa *na = ll_check_Numa(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixAddWithIndicator(pixs, pixa, na));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Affine(lua_State *L)
{
    LL_FUNC("Affine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixAffine(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffineColor(lua_State *L)
{
    LL_FUNC("AffineColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 3);
    if (pixAffineColor(pixs, &vc, colorval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffineGray(lua_State *L)
{
    LL_FUNC("AffineGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 3);
    if (pixAffineGray(pixs, &vc, grayval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffinePta(lua_State *L)
{
    LL_FUNC("AffinePta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixAffinePta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffinePtaColor(lua_State *L)
{
    LL_FUNC("AffinePtaColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixAffinePtaColor(pixs, ptad, ptas, colorval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffinePtaGray(lua_State *L)
{
    LL_FUNC("AffinePtaGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 4);
    Pix *pix = pixAffinePtaGray(pixs, ptad, ptas, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a Pix* (pixg).
 * Arg #5 is expected to be a l_float32 (fract).
 * Arg #6 is expected to be a l_int32 (border).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffinePtaWithAlpha(lua_State *L)
{
    LL_FUNC("AffinePtaWithAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    Pix *pixg = ll_check_Pix(_fun, L, 4);
    l_float32 fract = ll_check_l_float32(_fun, L, 5);
    l_int32 border = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixAffinePtaWithAlpha(pixs, ptad, ptas, pixg, fract, border);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffineSampled(lua_State *L)
{
    LL_FUNC("AffineSampled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixAffineSampled(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffineSampledPta(lua_State *L)
{
    LL_FUNC("AffineSampledPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixAffineSampledPta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (bw).
 * Arg #5 is expected to be a l_int32 (bh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AffineSequential(lua_State *L)
{
    LL_FUNC("AffineSequential");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 bw = ll_check_l_int32(_fun, L, 4);
    l_int32 bh = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixAffineSequential(pixs, ptad, ptas, bw, bh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
AlphaBlendUniform(lua_State *L)
{
    LL_FUNC("AlphaBlendUniform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 color = ll_check_l_uint32(_fun, L, 2);
    Pix *pix = pixAlphaBlendUniform(pixs, color);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check alpha layer of a Pix* for opaqueness.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AlphaIsOpaque(lua_State *L)
{
    LL_FUNC("AlphaIsOpaque");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 opaque = 0;
    if (pixAlphaIsOpaque(pix, &opaque))
        return ll_push_nil(L);
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ApplyInvBackgroundGrayMap(lua_State *L)
{
    LL_FUNC("ApplyInvBackgroundGrayMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixApplyInvBackgroundGrayMap(pixs, pixm, sx, sy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixmr).
 * Arg #3 is expected to be a Pix* (pixmg).
 * Arg #4 is expected to be a Pix* (pixmb).
 * Arg #5 is expected to be a l_int32 (sx).
 * Arg #6 is expected to be a l_int32 (sy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ApplyInvBackgroundRGBMap(lua_State *L)
{
    LL_FUNC("ApplyInvBackgroundRGBMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixmr = ll_check_Pix(_fun, L, 2);
    Pix *pixmg = ll_check_Pix(_fun, L, 3);
    Pix *pixmb = ll_check_Pix(_fun, L, 4);
    l_int32 sx = ll_check_l_int32(_fun, L, 5);
    l_int32 sy = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixApplyInvBackgroundRGBMap(pixs, pixmr, pixmg, pixmb, sx, sy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixth).
 * Arg #3 is expected to be a l_int32 (redfactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ApplyLocalThreshold(lua_State *L)
{
    LL_FUNC("ApplyLocalThreshold");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixth = ll_check_Pix(_fun, L, 2);
    l_int32 redfactor = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixApplyLocalThreshold(pixs, pixth, redfactor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixg).
 * Arg #3 is expected to be a l_int32 (target).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ApplyVariableGrayMap(lua_State *L)
{
    LL_FUNC("ApplyVariableGrayMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    l_int32 target = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixApplyVariableGrayMap(pixs, pixg, target);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_int32 (level).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
AssignToNearestColor(lua_State *L)
{
    LL_FUNC("AssignToNearestColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 level = ll_check_l_int32(_fun, L, 4);
    l_int32 countarray = 0;
    if (pixAssignToNearestColor(pixd, pixs, pixm, level, &countarray))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, countarray);
    return 1;
}

/**
 * \brief Build the average by column of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * Arg #3 is optional and, if given, expected to be a string (type: white-is-max or black-is-max).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageByColumn(lua_State *L)
{
    LL_FUNC("AverageByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 type = ll_check_what_is_max(_fun, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByColumn(pixs, box, type);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the average by row of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * Arg #3 is optional and, if given, expected to be a string (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageByRow(lua_State *L)
{
    LL_FUNC("AverageByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 type = ll_check_what_is_max(_fun, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByRow(pixs, box, type);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the average inside a Box* (%box) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageInRect(lua_State *L)
{
    LL_FUNC("AverageInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_float32 ave = 0.0f;
    if (pixAverageInRect(pixs, box, &ave))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, ave);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (fract).
 * Arg #3 is expected to be a l_int32 (dir).
 * Arg #4 is expected to be a l_int32 (first).
 * Arg #5 is expected to be a l_int32 (last).
 * Arg #6 is expected to be a l_int32 (factor1).
 * Arg #7 is expected to be a l_int32 (factor2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
AverageIntensityProfile(lua_State *L)
{
    LL_FUNC("AverageIntensityProfile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = ll_check_l_float32(_fun, L, 2);
    l_int32 dir = ll_check_l_int32(_fun, L, 3);
    l_int32 first = ll_check_l_int32(_fun, L, 4);
    l_int32 last = ll_check_l_int32(_fun, L, 5);
    l_int32 factor1 = ll_check_l_int32(_fun, L, 6);
    l_int32 factor2 = ll_check_l_int32(_fun, L, 7);
    Numa *result = pixAverageIntensityProfile(pixs, fract, dir, first, last, factor1, factor2);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_float32 on the Lua stack
 */
static int
AverageOnLine(lua_State *L)
{
    LL_FUNC("AverageOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    l_float32 result = pixAverageOnLine(pixs, x1, y1, x2, y2, factor);
    return ll_push_l_float32(_fun, L, result);
}
/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_int32 (sx).
 * Arg #5 is expected to be a l_int32 (sy).
 * Arg #6 is expected to be a l_int32 (thresh).
 * Arg #7 is expected to be a l_int32 (mincount).
 * Arg #8 is expected to be a l_int32 (bgval).
 * Arg #9 is expected to be a l_int32 (smoothx).
 * Arg #10 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BackgroundNorm(lua_State *L)
{
    LL_FUNC("BackgroundNorm");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_int32 sx = ll_check_l_int32 (_fun, L, 4);
    l_int32 sy = ll_check_l_int32 (_fun, L, 5);
    l_int32 thresh = ll_check_l_int32 (_fun, L, 6);
    l_int32 mincount = ll_check_l_int32 (_fun, L, 7);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 8);
    l_int32 smoothx = ll_check_l_int32 (_fun, L, 9);
    l_int32 smoothy = ll_check_l_int32 (_fun, L, 10);

    Pix *pix = pixBackgroundNorm(pixs, pixim, pixg, sx, sy, thresh, mincount, bgval, smoothx, smoothy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sx).
 * Arg #3 is expected to be a l_int32 (sy).
 * Arg #4 is expected to be a l_int32 (smoothx).
 * Arg #5 is expected to be a l_int32 (smoothy).
 * Arg #6 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BackgroundNormFlex(lua_State *L)
{
    LL_FUNC("BackgroundNormFlex");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sx = ll_check_l_int32(_fun, L, 2);
    l_int32 sy = ll_check_l_int32(_fun, L, 3);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 4);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 5);
    l_int32 delta = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixBackgroundNormFlex(pixs, sx, sy, smoothx, smoothy, delta);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a l_int32 (thresh).
 * Arg #6 is expected to be a l_int32 (mincount).
 * Arg #7 is expected to be a l_int32 (bgval).
 * Arg #8 is expected to be a l_int32 (smoothx).
 * Arg #9 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
BackgroundNormGrayArray(lua_State *L)
{
    LL_FUNC("BackgroundNormGrayArray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32 (_fun, L, 3);
    l_int32 sy = ll_check_l_int32 (_fun, L, 4);
    l_int32 thresh = ll_check_l_int32 (_fun, L, 5);
    l_int32 mincount = ll_check_l_int32 (_fun, L, 6);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 7);
    l_int32 smoothx = ll_check_l_int32 (_fun, L, 8);
    l_int32 smoothy = ll_check_l_int32 (_fun, L, 9);
    Pix *pixd = nullptr;
    if (pixBackgroundNormGrayArray(pixs, pixim, sx, sy, thresh, mincount, bgval, smoothx, smoothy, &pixd))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixd);
}
/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (size).
 * Arg #5 is expected to be a l_int32 (bgval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
BackgroundNormGrayArrayMorph(lua_State *L)
{
    LL_FUNC("BackgroundNormGrayArrayMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32 (_fun, L, 3);
    l_int32 size = ll_check_l_int32 (_fun, L, 4);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 5);
    Pix *pixd = nullptr;
    if (pixBackgroundNormGrayArrayMorph(pixs, pixim, reduction, size, bgval, &pixd))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (size).
 * Arg #5 is expected to be a l_int32 (bgval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BackgroundNormMorph(lua_State *L)
{
    LL_FUNC("BackgroundNormMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32 (_fun, L, 3);
    l_int32 size = ll_check_l_int32 (_fun, L, 4);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 5);
    Pix *pix = pixBackgroundNormMorph(pixs, pixim, reduction, size, bgval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_int32 (sx).
 * Arg #5 is expected to be a l_int32 (sy).
 * Arg #6 is expected to be a l_int32 (thresh).
 * Arg #7 is expected to be a l_int32 (mincount).
 * Arg #8 is expected to be a l_int32 (bgval).
 * Arg #9 is expected to be a l_int32 (smoothx).
 * Arg #10 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 Pix* on the Lua stack (%pix_r, %pix_g, %pix_b)
 */
static int
BackgroundNormRGBArrays(lua_State *L)
{
    LL_FUNC("BackgroundNormRGBArrays");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_int32 sx = ll_check_l_int32 (_fun, L, 4);
    l_int32 sy = ll_check_l_int32 (_fun, L, 5);
    l_int32 thresh = ll_check_l_int32 (_fun, L, 6);
    l_int32 mincount = ll_check_l_int32 (_fun, L, 7);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 8);
    l_int32 smoothx = ll_check_l_int32 (_fun, L, 9);
    l_int32 smoothy = ll_check_l_int32 (_fun, L, 10);
    Pix *pix_r = ll_check_Pix(_fun, L, 11);
    Pix *pix_g = ll_check_Pix(_fun, L, 12);
    Pix *pix_b = ll_check_Pix(_fun, L, 13);

    if (pixBackgroundNormRGBArrays(pixs, pixim, pixg, sx, sy, thresh, mincount, bgval, smoothx, smoothy, &pix_r, &pix_g, &pix_b))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pix_r) + ll_push_Pix(_fun, L, pix_g) + ll_push_Pix(_fun, L, pix_b);
}

/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (size).
 * Arg #5 is expected to be a l_int32 (bgval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 Pix* on the Lua stack (%pixr, %pixg, %pixb)
 */
static int
BackgroundNormRGBArraysMorph(lua_State *L)
{
    LL_FUNC("BackgroundNormRGBArraysMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32 (_fun, L, 3);
    l_int32 size = ll_check_l_int32 (_fun, L, 4);
    l_int32 bgval = ll_check_l_int32 (_fun, L, 5);
    Pix *pixr = nullptr;
    Pix *pixg = nullptr;
    Pix *pixb = nullptr;
    if (pixBackgroundNormRGBArraysMorph(pixs, pixim, reduction, size, bgval, &pixr, &pixg, &pixb))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixr) + ll_push_Pix(_fun, L, pixg) + ll_push_Pix(_fun, L, pixb);
}

/**
 * \brief Comment here
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a Pix* (pixg).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BackgroundNormSimple(lua_State *L)
{
    LL_FUNC("BackgroundNormSimple");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);

    Pix *pix = pixBackgroundNormSimple(pixs, pixim, pixg);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (area1).
 * Arg #4 is expected to be a l_int32 (area2).
 * Arg #5 is expected to be a l_int32 (etransx).
 * Arg #6 is expected to be a l_int32 (etransy).
 * Arg #7 is expected to be a l_int32 (maxshift).
 * Arg #12 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
BestCorrelation(lua_State *L)
{
    LL_FUNC("BestCorrelation");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 area1 = ll_check_l_int32(_fun, L, 3);
    l_int32 area2 = ll_check_l_int32(_fun, L, 4);
    l_int32 etransx = ll_check_l_int32(_fun, L, 5);
    l_int32 etransy = ll_check_l_int32(_fun, L, 6);
    l_int32 maxshift = ll_check_l_int32(_fun, L, 7);
    l_int32 tab8 = 0;
    l_int32 delx = 0;
    l_int32 dely = 0;
    l_float32 score = 0;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 12);
    if (pixBestCorrelation(pix1, pix2, area1, area2, etransx, etransy, maxshift, &tab8, &delx, &dely, &score, debugflag))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab8);
    ll_push_l_int32(_fun, L, delx);
    ll_push_l_int32(_fun, L, dely);
    ll_push_l_float32(_fun, L, score);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (spatial_stdev).
 * Arg #3 is expected to be a l_float32 (range_stdev).
 * Arg #4 is expected to be a l_int32 (ncomps).
 * Arg #5 is expected to be a l_int32 (reduction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Bilateral(lua_State *L)
{
    LL_FUNC("Bilateral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 spatial_stdev = ll_check_l_float32(_fun, L, 2);
    l_float32 range_stdev = ll_check_l_float32(_fun, L, 3);
    l_int32 ncomps = ll_check_l_int32(_fun, L, 4);
    l_int32 reduction = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixBilateral(pixs, spatial_stdev, range_stdev, ncomps, reduction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (spatial_kel).
 * Arg #3 is expected to be a Kernel* (range_kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilateralExact(lua_State *L)
{
    LL_FUNC("BilateralExact");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *spatial_kel = ll_check_Kernel(_fun, L, 2);
    Kernel *range_kel = ll_check_Kernel(_fun, L, 3);
    Pix *pix = pixBilateralExact(pixs, spatial_kel, range_kel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (spatial_stdev).
 * Arg #3 is expected to be a l_float32 (range_stdev).
 * Arg #4 is expected to be a l_int32 (ncomps).
 * Arg #5 is expected to be a l_int32 (reduction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilateralGray(lua_State *L)
{
    LL_FUNC("BilateralGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 spatial_stdev = ll_check_l_float32(_fun, L, 2);
    l_float32 range_stdev = ll_check_l_float32(_fun, L, 3);
    l_int32 ncomps = ll_check_l_int32(_fun, L, 4);
    l_int32 reduction = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixBilateralGray(pixs, spatial_stdev, range_stdev, ncomps, reduction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (spatial_kel).
 * Arg #3 is expected to be a Kernel* (range_kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilateralGrayExact(lua_State *L)
{
    LL_FUNC("BilateralGrayExact");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *spatial_kel = ll_check_Kernel(_fun, L, 2);
    Kernel *range_kel = ll_check_Kernel(_fun, L, 3);
    Pix *pix = pixBilateralGrayExact(pixs, spatial_kel, range_kel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Bilinear(lua_State *L)
{
    LL_FUNC("Bilinear");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixBilinear(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearColor(lua_State *L)
{
    LL_FUNC("BilinearColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 3);
    if (pixBilinearColor(pixs, &vc, colorval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearGray(lua_State *L)
{
    LL_FUNC("BilinearGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 3);
    if (pixBilinearGray(pixs, &vc, grayval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearPta(lua_State *L)
{
    LL_FUNC("BilinearPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixBilinearPta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearPtaColor(lua_State *L)
{
    LL_FUNC("BilinearPtaColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixBilinearPtaColor(pixs, ptad, ptas, colorval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearPtaGray(lua_State *L)
{
    LL_FUNC("BilinearPtaGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 4);
    Pix *pix = pixBilinearPtaGray(pixs, ptad, ptas, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a Pix* (pixg).
 * Arg #5 is expected to be a l_float32 (fract).
 * Arg #6 is expected to be a l_int32 (border).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearPtaWithAlpha(lua_State *L)
{
    LL_FUNC("BilinearPtaWithAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    Pix *pixg = ll_check_Pix(_fun, L, 4);
    l_float32 fract = ll_check_l_float32(_fun, L, 5);
    l_int32 border = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixBilinearPtaWithAlpha(pixs, ptad, ptas, pixg, fract, border);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearSampled(lua_State *L)
{
    LL_FUNC("BilinearSampled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixBilinearSampled(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BilinearSampledPta(lua_State *L)
{
    LL_FUNC("BilinearSampledPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixBilinearSampledPta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Blend(lua_State *L)
{
    LL_FUNC("Blend");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_float32 fract = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixBlend(pixs1, pixs2, x, y, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_uint32 (color).
 * Arg #5 is expected to be a l_float32 (gamma).
 * Arg #6 is expected to be a l_int32 (minval).
 * Arg #7 is expected to be a l_int32 (maxval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendBackgroundToColor(lua_State *L)
{
    LL_FUNC("BlendBackgroundToColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_uint32 color = ll_check_l_uint32(_fun, L, 4);
    l_float32 gamma = ll_check_l_float32(_fun, L, 5);
    l_int32 minval = ll_check_l_int32(_fun, L, 6);
    l_int32 maxval = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixBlendBackgroundToColor(pixd, pixs, box, color, gamma, minval, maxval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendBoxaRandom(lua_State *L)
{
    LL_FUNC("BlendBoxaRandom");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixBlendBoxaRandom(pixs, boxa, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixb).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (sindex).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
BlendCmap(lua_State *L)
{
    LL_FUNC("BlendCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixb = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 sindex = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixBlendCmap(pixs, pixb, x, y, sindex));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * Arg #7 is expected to be a l_int32 (transparent).
 * Arg #8 is expected to be a l_uint32 (transpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendColor(lua_State *L)
{
    LL_FUNC("BlendColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    l_int32 transparent = ll_check_l_int32(_fun, L, 7);
    l_uint32 transpix = ll_check_l_uint32(_fun, L, 8);
    Pix *pix = pixBlendColor(pixd, pixs1, pixs2, x, y, fract, transparent, transpix);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (rfract).
 * Arg #7 is expected to be a l_float32 (gfract).
 * Arg #8 is expected to be a l_float32 (bfract).
 * Arg #9 is expected to be a l_int32 (transparent).
 * Arg #10 is expected to be a l_uint32 (transpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendColorByChannel(lua_State *L)
{
    LL_FUNC("BlendColorByChannel");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 rfract = ll_check_l_float32(_fun, L, 6);
    l_float32 gfract = ll_check_l_float32(_fun, L, 7);
    l_float32 bfract = ll_check_l_float32(_fun, L, 8);
    l_int32 transparent = ll_check_l_int32(_fun, L, 9);
    l_uint32 transpix = ll_check_l_uint32(_fun, L, 10);
    Pix *pix = pixBlendColorByChannel(pixd, pixs1, pixs2, x, y, rfract, gfract, bfract, transparent, transpix);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * Arg #7 is expected to be a l_int32 (type).
 * Arg #8 is expected to be a l_int32 (transparent).
 * Arg #9 is expected to be a l_uint32 (transpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendGray(lua_State *L)
{
    LL_FUNC("BlendGray");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    l_int32 type = ll_check_l_int32(_fun, L, 7);
    l_int32 transparent = ll_check_l_int32(_fun, L, 8);
    l_uint32 transpix = ll_check_l_uint32(_fun, L, 9);
    Pix *pix = pixBlendGray(pixd, pixs1, pixs2, x, y, fract, type, transparent, transpix);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * Arg #7 is expected to be a l_int32 (shift).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendGrayAdapt(lua_State *L)
{
    LL_FUNC("BlendGrayAdapt");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    l_int32 shift = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixBlendGrayAdapt(pixd, pixs1, pixs2, x, y, fract, shift);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendGrayInverse(lua_State *L)
{
    LL_FUNC("BlendGrayInverse");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    Pix *pix = pixBlendGrayInverse(pixd, pixs1, pixs2, x, y, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendHardLight(lua_State *L)
{
    LL_FUNC("BlendHardLight");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    Pix *pix = pixBlendHardLight(pixd, pixs1, pixs2, x, y, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Blend all pixels inside a Box* (%box) in a Pix* (%pix) with value fraction (%val, %fract).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_uint32 (val).
 * Arg #4 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
BlendInRect(lua_State *L)
{
    LL_FUNC("BlendInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    l_float32 fract = ll_check_l_float32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixBlendInRect(pix, box, val, fract));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_float32 (fract).
 * Arg #7 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendMask(lua_State *L)
{
    LL_FUNC("BlendMask");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    l_int32 type = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixBlendMask(pixd, pixs1, pixs2, x, y, fract, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlendWithGrayMask(lua_State *L)
{
    LL_FUNC("BlendWithGrayMask");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixBlendWithGrayMask(pixs1, pixs2, pixg, x, y);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (spatial_stdev).
 * Arg #3 is expected to be a l_float32 (range_stdev).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
BlockBilateralExact(lua_State *L)
{
    LL_FUNC("BlockBilateralExact");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 spatial_stdev = ll_check_l_float32(_fun, L, 2);
    l_float32 range_stdev = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixBlockBilateralExact(pixs, spatial_stdev, range_stdev);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Blockconv(lua_State *L)
{
    LL_FUNC("Blockconv");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixBlockconv(pixs, wc, hc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlockconvAccum(lua_State *L)
{
    LL_FUNC("BlockconvAccum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixBlockconvAccum(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixacc).
 * Arg #3 is expected to be a l_int32 (wc).
 * Arg #4 is expected to be a l_int32 (hc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlockconvGray(lua_State *L)
{
    LL_FUNC("BlockconvGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixacc = ll_check_Pix(_fun, L, 2);
    l_int32 wc = ll_check_l_int32(_fun, L, 3);
    l_int32 hc = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixBlockconvGray(pixs, pixacc, wc, hc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixacc).
 * Arg #3 is expected to be a l_int32 (wc).
 * Arg #4 is expected to be a l_int32 (hc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlockconvGrayTile(lua_State *L)
{
    LL_FUNC("BlockconvGrayTile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixacc = ll_check_Pix(_fun, L, 2);
    l_int32 wc = ll_check_l_int32(_fun, L, 3);
    l_int32 hc = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixBlockconvGrayTile(pixs, pixacc, wc, hc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlockconvGrayUnnormalized(lua_State *L)
{
    LL_FUNC("BlockconvGrayUnnormalized");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixBlockconvGrayUnnormalized(pixs, wc, hc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * Arg #4 is expected to be a l_int32 (nx).
 * Arg #5 is expected to be a l_int32 (ny).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
BlockconvTiled(lua_State *L)
{
    LL_FUNC("BlockconvTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    l_int32 nx = ll_check_l_int32(_fun, L, 4);
    l_int32 ny = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixBlockconvTiled(pixs, wc, hc, nx, ny);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixacc).
 * Arg #3 is expected to be a l_int32 (wc).
 * Arg #4 is expected to be a l_int32 (hc).
 * Arg #5 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Blockrank(lua_State *L)
{
    LL_FUNC("Blockrank");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixacc = ll_check_Pix(_fun, L, 2);
    l_int32 wc = ll_check_l_int32(_fun, L, 3);
    l_int32 hc = ll_check_l_int32(_fun, L, 4);
    l_float32 rank = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixBlockrank(pixs, pixacc, wc, hc, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixacc).
 * Arg #3 is expected to be a l_int32 (wc).
 * Arg #4 is expected to be a l_int32 (hc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Blocksum(lua_State *L)
{
    LL_FUNC("Blocksum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixacc = ll_check_Pix(_fun, L, 2);
    l_int32 wc = ll_check_l_int32(_fun, L, 3);
    l_int32 hc = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixBlocksum(pixs, pixacc, wc, hc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfsize).
 * Arg #3 is expected to be a Pix* (pixacc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CensusTransform(lua_State *L)
{
    LL_FUNC("CensusTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfsize = ll_check_l_int32(_fun, L, 2);
    Pix *pixacc = ll_check_Pix(_fun, L, 3);
    Pix *pix = pixCensusTransform(pixs, halfsize, pixacc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Centroid(lua_State *L)
{
    LL_FUNC("Centroid");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 centtab = 0;
    l_int32 sumtab = 0;
    l_float32 xave = 0;
    l_float32 yave = 0;
    if (pixCentroid(pix, &centtab, &sumtab, &xave, &yave))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, centtab);
    ll_push_l_int32(_fun, L, sumtab);
    ll_push_l_float32(_fun, L, xave);
    ll_push_l_float32(_fun, L, yave);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Centroid8(lua_State *L)
{
    LL_FUNC("Centroid8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_float32 cx = 0;
    l_float32 cy = 0;
    if (pixCentroid8(pixs, factor, &cx, &cy))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, cx);
    ll_push_l_float32(_fun, L, cy);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixChangeRefcount(pix, delta));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 (%iff) on the Lua stack
 */
static int
ChooseOutputFormat(lua_State *L)
{
    LL_FUNC("ChooseOutputFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 iff = pixChooseOutputFormat(pix);
    return ll_push_l_int32(_fun, L, iff);
}

/**
 * \brief Clean a Pix* (%pixs) background to white.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_float32 (gamma).
 * Arg #5 is expected to be a l_int32 (blackval).
 * Arg #6 is expected to be a l_int32 (whiteval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CleanBackgroundToWhite(lua_State *L)
{
    LL_FUNC("CleanBackgroundToWhite");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_float32 gamma = ll_check_l_float32(_fun, L, 4);
    l_int32 blackval = ll_check_l_int32(_fun, L, 5);
    l_int32 whiteval = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixCleanBackgroundToWhite(pixs, pixim, pixg, gamma, blackval, whiteval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CleanupByteProcessing(lua_State *L)
{
    LL_FUNC("CleanupByteProcessing");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint8 *lineptrs = nullptr;
    size_t size = static_cast<size_t>(pixGetHeight(pix));
    if (pixCleanupByteProcessing(pix, &lineptrs))
        return ll_push_nil(L);
    // ll_push_string(_fun, L, reinterpret_cast<const char *>(lineptrs), size);
    return 1;
}

/**
 * \brief Clear all pixels in a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClearAll(lua_State *L)
{
    LL_FUNC("ClearAll");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixClearAll(pix));
}

/**
 * \brief Clear all pixels inside a Box* (%box) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ClearInRect(lua_State *L)
{
    LL_FUNC("ClearInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixClearInRect(pix, box));
}

/**
 * \brief Clear a pixel value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClearPixel(lua_State *L)
{
    LL_FUNC("ClearPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixClearPixel(pix, x, y));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (boxs).
 * Arg #3 is expected to be a l_int32 (lowthresh).
 * Arg #4 is expected to be a l_int32 (highthresh).
 * Arg #5 is expected to be a l_int32 (maxwidth).
 * Arg #6 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ClipBoxToEdges(lua_State *L)
{
    LL_FUNC("ClipBoxToEdges");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *boxs = ll_check_Box(_fun, L, 2);
    l_int32 lowthresh = ll_check_l_int32(_fun, L, 3);
    l_int32 highthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 maxwidth = ll_check_l_int32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    Pix *pixd = nullptr;
    Box *boxd = nullptr;
    if (pixClipBoxToEdges(pixs, boxs, lowthresh, highthresh, maxwidth, factor, &pixd, &boxd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    ll_push_Box(_fun, L, boxd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (boxs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ClipBoxToForeground(lua_State *L)
{
    LL_FUNC("ClipBoxToForeground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *boxs = ll_check_Box(_fun, L, 2);
    Pix *pixd = nullptr;
    Box *boxd = nullptr;
    if (pixClipBoxToForeground(pixs, boxs, &pixd, &boxd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    ll_push_Box(_fun, L, boxd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_uint32 (outval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ClipMasked(lua_State *L)
{
    LL_FUNC("ClipMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_uint32 outval = ll_check_l_uint32(_fun, L, 5);
    Pix *pix = pixClipMasked(pixs, pixm, x, y, outval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ClipRectangle(lua_State *L)
{
    LL_FUNC("ClipRectangle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Box *boxc = nullptr;
    if (pixClipRectangle(pixs, box, &boxc))
        return ll_push_nil(L);
    ll_push_Box(_fun, L, boxc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa * on the Lua stack
 */
static int
ClipRectangles(lua_State *L)
{
    LL_FUNC("ClipRectangles");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    Pixa *result = pixClipRectangles(pixs, boxa);
    return ll_push_Pixa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ClipToForeground(lua_State *L)
{
    LL_FUNC("ClipToForeground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixd = nullptr;
    Box *box = nullptr;
    if (pixClipToForeground(pixs, &pixd, &box))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    ll_push_Box(_fun, L, box);
    return 2;
}

/**
 * \brief Clone a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixd = pixClone(pixs);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Close(lua_State *L)
{
    LL_FUNC("Close");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixClose(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseBrick(lua_State *L)
{
    LL_FUNC("CloseBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseBrickDwa(lua_State *L)
{
    LL_FUNC("CloseBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseCompBrick(lua_State *L)
{
    LL_FUNC("CloseCompBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseCompBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseCompBrickDwa(lua_State *L)
{
    LL_FUNC("CloseCompBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseCompBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseCompBrickExtendDwa(lua_State *L)
{
    LL_FUNC("CloseCompBrickExtendDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseCompBrickExtendDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseGeneralized(lua_State *L)
{
    LL_FUNC("CloseGeneralized");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixCloseGeneralized(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseGray(lua_State *L)
{
    LL_FUNC("CloseGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixCloseGray(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseGray3(lua_State *L)
{
    LL_FUNC("CloseGray3");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixCloseGray3(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseSafe(lua_State *L)
{
    LL_FUNC("CloseSafe");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixCloseSafe(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseSafeBrick(lua_State *L)
{
    LL_FUNC("CloseSafeBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseSafeBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CloseSafeCompBrick(lua_State *L)
{
    LL_FUNC("CloseSafeCompBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixCloseSafeCompBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (rwhite).
 * Arg #3 is expected to be a l_int32 (gwhite).
 * Arg #4 is expected to be a l_int32 (bwhite).
 * Arg #5 is expected to be a l_int32 (mingray).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ColorContent(lua_State *L)
{
    LL_FUNC("ColorContent");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 rwhite = ll_check_l_int32(_fun, L, 2);
    l_int32 gwhite = ll_check_l_int32(_fun, L, 3);
    l_int32 bwhite = ll_check_l_int32(_fun, L, 4);
    l_int32 mingray = ll_check_l_int32(_fun, L, 5);
    Pix *pixr = nullptr;
    Pix *pixg = nullptr;
    Pix *pixb = nullptr;
    if (pixColorContent(pixs, rwhite, gwhite, bwhite, mingray, &pixr, &pixg, &pixb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixr);
    ll_push_Pix(_fun, L, pixg);
    ll_push_Pix(_fun, L, pixb);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (darkthresh).
 * Arg #3 is expected to be a l_int32 (lightthresh).
 * Arg #4 is expected to be a l_int32 (diffthresh).
 * Arg #5 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ColorFraction(lua_State *L)
{
    LL_FUNC("ColorFraction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 2);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 3);
    l_int32 diffthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    l_float32 pixfract = 0;
    l_float32 colorfract = 0;
    if (pixColorFraction(pixs, darkthresh, lightthresh, diffthresh, factor, &pixfract, &colorfract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, pixfract);
    ll_push_l_float32(_fun, L, colorfract);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a l_int32 (rval).
 * Arg #6 is expected to be a l_int32 (gval).
 * Arg #7 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ColorGray(lua_State *L)
{
    LL_FUNC("ColorGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    l_int32 rval = ll_check_l_int32(_fun, L, 5);
    l_int32 gval = ll_check_l_int32(_fun, L, 6);
    l_int32 bval = ll_check_l_int32(_fun, L, 7);
    return ll_push_boolean(_fun, L, 0 == pixColorGray(pixs, box, type, thresh, rval, gval, bval));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (rval).
 * Arg #5 is expected to be a l_int32 (gval).
 * Arg #6 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ColorGrayCmap(lua_State *L)
{
    LL_FUNC("ColorGrayCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    return ll_push_boolean(_fun, L, 0 == pixColorGrayCmap(pixs, box, type, rval, gval, bval));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a l_int32 (rval).
 * Arg #6 is expected to be a l_int32 (gval).
 * Arg #7 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorGrayMasked(lua_State *L)
{
    LL_FUNC("ColorGrayMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    l_int32 rval = ll_check_l_int32(_fun, L, 5);
    l_int32 gval = ll_check_l_int32(_fun, L, 6);
    l_int32 bval = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixColorGrayMasked(pixs, pixm, type, thresh, rval, gval, bval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (rval).
 * Arg #5 is expected to be a l_int32 (gval).
 * Arg #6 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ColorGrayMaskedCmap(lua_State *L)
{
    LL_FUNC("ColorGrayMaskedCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    return ll_push_boolean(_fun, L, 0 == pixColorGrayMaskedCmap(pixs, pixm, type, rval, gval, bval));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a l_int32 (rval).
 * Arg #6 is expected to be a l_int32 (gval).
 * Arg #7 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorGrayRegions(lua_State *L)
{
    LL_FUNC("ColorGrayRegions");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    l_int32 rval = ll_check_l_int32(_fun, L, 5);
    l_int32 gval = ll_check_l_int32(_fun, L, 6);
    l_int32 bval = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixColorGrayRegions(pixs, boxa, type, thresh, rval, gval, bval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (rval).
 * Arg #5 is expected to be a l_int32 (gval).
 * Arg #6 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ColorGrayRegionsCmap(lua_State *L)
{
    LL_FUNC("ColorGrayRegionsCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    return ll_push_boolean(_fun, L, 0 == pixColorGrayRegionsCmap(pixs, boxa, type, rval, gval, bval));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (rwhite).
 * Arg #3 is expected to be a l_int32 (gwhite).
 * Arg #4 is expected to be a l_int32 (bwhite).
 * Arg #5 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorMagnitude(lua_State *L)
{
    LL_FUNC("ColorMagnitude");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 rwhite = ll_check_l_int32(_fun, L, 2);
    l_int32 gwhite = ll_check_l_int32(_fun, L, 3);
    l_int32 bwhite = ll_check_l_int32(_fun, L, 4);
    l_int32 type = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixColorMagnitude(pixs, rwhite, gwhite, bwhite, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorMorph(lua_State *L)
{
    LL_FUNC("ColorMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixColorMorph(pixs, type, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * Arg #4 is expected to be a l_int32 (dispy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorMorphSequence(lua_State *L)
{
    LL_FUNC("ColorMorphSequence");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    l_int32 dispy = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixColorMorphSequence(pixs, sequence, dispsep, dispy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxdist).
 * Arg #3 is expected to be a l_int32 (maxcolors).
 * Arg #4 is expected to be a l_int32 (selsize).
 * Arg #5 is expected to be a l_int32 (finalcolors).
 * Arg #6 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorSegment(lua_State *L)
{
    LL_FUNC("ColorSegment");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 2);
    l_int32 maxcolors = ll_check_l_int32(_fun, L, 3);
    l_int32 selsize = ll_check_l_int32(_fun, L, 4);
    l_int32 finalcolors = ll_check_l_int32(_fun, L, 5);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixColorSegment(pixs, maxdist, maxcolors, selsize, finalcolors, debugflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (selsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ColorSegmentClean(lua_State *L)
{
    LL_FUNC("ColorSegmentClean");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 selsize = ll_check_l_int32(_fun, L, 2);
    l_int32 countarray = 0;
    if (pixColorSegmentClean(pixs, selsize, &countarray))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, countarray);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxdist).
 * Arg #3 is expected to be a l_int32 (maxcolors).
 * Arg #4 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ColorSegmentCluster(lua_State *L)
{
    LL_FUNC("ColorSegmentCluster");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 2);
    l_int32 maxcolors = ll_check_l_int32(_fun, L, 3);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixColorSegmentCluster(pixs, maxdist, maxcolors, debugflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (finalcolors).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ColorSegmentRemoveColors(lua_State *L)
{
    LL_FUNC("ColorSegmentRemoveColors");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 finalcolors = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixColorSegmentRemoveColors(pixd, pixs, finalcolors));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rfract).
 * Arg #3 is expected to be a l_float32 (gfract).
 * Arg #4 is expected to be a l_float32 (bfract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ColorShiftRGB(lua_State *L)
{
    LL_FUNC("ColorShiftRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rfract = ll_check_l_float32(_fun, L, 2);
    l_float32 gfract = ll_check_l_float32(_fun, L, 3);
    l_float32 bfract = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixColorShiftRGB(pixs, rfract, gfract, bfract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Colorize a gray 8bpp Pix* (%pixs) to a 32bpp or 8bpp colormapped Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (color).
 * Arg #3 is expected to be a boolean (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ColorizeGray(lua_State *L)
{
    LL_FUNC("ColorizeGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 color = ll_check_l_uint32(_fun, L, 2);
    l_int32 cmapflag = ll_check_boolean(_fun, L, 3);
    Pix *pix = pixColorizeGray(pixs, color, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ColorsForQuantization(lua_State *L)
{
    LL_FUNC("ColorsForQuantization");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_int32 ncolors = 0;
    l_int32 iscolor = 0;
    if (pixColorsForQuantization(pixs, thresh, &ncolors, &iscolor, debug))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncolors);
    ll_push_l_int32(_fun, L, iscolor);
    return 2;
}

/**
 * \brief Get the column stats for Pix* (%pixs) as six Numa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 Numa* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
ColumnStats(lua_State *L)
{
    LL_FUNC("ColumnStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa *mean = nullptr;
    Numa *median = nullptr;
    Numa *mode = nullptr;
    Numa *modecount = nullptr;
    Numa *var = nullptr;
    Numa *rootvar = nullptr;
    if (pixColumnStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, mean) +
            ll_push_Numa(_fun, L, median) +
            ll_push_Numa(_fun, L, mode) +
            ll_push_Numa(_fun, L, modecount) +
            ll_push_Numa(_fun, L, var) +
            ll_push_Numa(_fun, L, rootvar);
}

/**
 * \brief Combine the Pix* (%pixs) with Pix* (%pixd) using another Pix* (%pixm) as mask.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm) with 1 bit/pixel.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMasked(lua_State *L)
{
    LL_FUNC("CombineMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixCombineMasked(pixd, pixs, pixm));
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp) at offset %x and %y.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm) with 1 bit/pixel.
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMaskedGeneral(lua_State *L)
{
    LL_FUNC("CombineMaskedGeneral");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixCombineMaskedGeneral(pixd, pixs, pixm, x, y));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (comptype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareBinary(lua_State *L)
{
    LL_FUNC("CompareBinary");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    l_float32 fract = 0;
    Pix *pixdiff = nullptr;
    if (pixCompareBinary(pix1, pix2, comptype, &fract, &pixdiff))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    ll_push_Pix(_fun, L, pixdiff);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a l_int32 (plottype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareGray(lua_State *L)
{
    LL_FUNC("CompareGray");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    l_int32 plottype = ll_check_l_int32(_fun, L, 4);
    l_int32 same = 0;
    l_float32 diff = 0;
    l_float32 rmsdiff = 0;
    Pix *pixdiff = nullptr;
    if (pixCompareGray(pix1, pix2, comptype, plottype, &same, &diff, &rmsdiff, &pixdiff))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    ll_push_l_float32(_fun, L, diff);
    ll_push_l_float32(_fun, L, rmsdiff);
    ll_push_Pix(_fun, L, pixdiff);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a Box* (box1).
 * Arg #4 is expected to be a Box* (box2).
 * Arg #5 is expected to be a l_float32 (minratio).
 * Arg #6 is expected to be a l_int32 (maxgray).
 * Arg #7 is expected to be a l_int32 (factor).
 * Arg #8 is expected to be a l_int32 (nx).
 * Arg #9 is expected to be a l_int32 (ny).
 * Arg #11 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareGrayByHisto(lua_State *L)
{
    LL_FUNC("CompareGrayByHisto");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    Box *box1 = ll_check_Box(_fun, L, 3);
    Box *box2 = ll_check_Box(_fun, L, 4);
    l_float32 minratio = ll_check_l_float32(_fun, L, 5);
    l_int32 maxgray = ll_check_l_int32(_fun, L, 6);
    l_int32 factor = ll_check_l_int32(_fun, L, 7);
    l_int32 nx = ll_check_l_int32(_fun, L, 8);
    l_int32 ny = ll_check_l_int32(_fun, L, 9);
    l_float32 score = 0;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 11);
    if (pixCompareGrayByHisto(pix1, pix2, box1, box2, minratio, maxgray, factor, nx, ny, &score, debugflag))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, score);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a l_int32 (plottype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareGrayOrRGB(lua_State *L)
{
    LL_FUNC("CompareGrayOrRGB");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    l_int32 plottype = ll_check_l_int32(_fun, L, 4);
    l_int32 same = 0;
    l_float32 diff = 0;
    l_float32 rmsdiff = 0;
    Pix *pixdiff = nullptr;
    if (pixCompareGrayOrRGB(pix1, pix2, comptype, plottype, &same, &diff, &rmsdiff, &pixdiff))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    ll_push_l_float32(_fun, L, diff);
    ll_push_l_float32(_fun, L, rmsdiff);
    ll_push_Pix(_fun, L, pixdiff);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a Box* (box1).
 * Arg #4 is expected to be a Box* (box2).
 * Arg #5 is expected to be a l_float32 (minratio).
 * Arg #6 is expected to be a l_int32 (factor).
 * Arg #7 is expected to be a l_int32 (nx).
 * Arg #8 is expected to be a l_int32 (ny).
 * Arg #10 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ComparePhotoRegionsByHisto(lua_State *L)
{
    LL_FUNC("ComparePhotoRegionsByHisto");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    Box *box1 = ll_check_Box(_fun, L, 3);
    Box *box2 = ll_check_Box(_fun, L, 4);
    l_float32 minratio = ll_check_l_float32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    l_int32 nx = ll_check_l_int32(_fun, L, 7);
    l_int32 ny = ll_check_l_int32(_fun, L, 8);
    l_float32 score = 0;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 10);
    if (pixComparePhotoRegionsByHisto(pix1, pix2, box1, box2, minratio, factor, nx, ny, &score, debugflag))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, score);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a l_int32 (plottype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareRGB(lua_State *L)
{
    LL_FUNC("CompareRGB");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    l_int32 plottype = ll_check_l_int32(_fun, L, 4);
    l_int32 same = 0;
    l_float32 diff = 0;
    l_float32 rmsdiff = 0;
    Pix *pixdiff = nullptr;
    if (pixCompareRGB(pix1, pix2, comptype, plottype, &same, &diff, &rmsdiff, &pixdiff))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    ll_push_l_float32(_fun, L, diff);
    ll_push_l_float32(_fun, L, rmsdiff);
    ll_push_Pix(_fun, L, pixdiff);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
CompareRankDifference(lua_State *L)
{
    LL_FUNC("CompareRankDifference");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Numa *result = pixCompareRankDifference(pix1, pix2, factor);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareTiled(lua_State *L)
{
    LL_FUNC("CompareTiled");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    l_int32 type = ll_check_l_int32(_fun, L, 5);
    Pix *pixdiff = nullptr;
    if (pixCompareTiled(pix1, pix2, sx, sy, type, &pixdiff))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixdiff);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (thresh).
 * Arg #7 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CompareWithTranslation(lua_State *L)
{
    LL_FUNC("CompareWithTranslation");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 thresh = ll_check_l_int32(_fun, L, 3);
    l_int32 delx = 0;
    l_int32 dely = 0;
    l_float32 score = 0;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 7);
    if (pixCompareWithTranslation(pix1, pix2, thresh, &delx, &dely, &score, debugflag))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, delx);
    ll_push_l_int32(_fun, L, dely);
    ll_push_l_float32(_fun, L, score);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_float32 (rnum).
 * Arg #3 is expected to be a l_float32 (gnum).
 * Arg #4 is expected to be a l_float32 (bnum).
 * Arg #5 is expected to be a l_float32 (rdenom).
 * Arg #6 is expected to be a l_float32 (gdenom).
 * Arg #7 is expected to be a l_float32 (bdenom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix * on the Lua stack
 */
static int
ComponentFunction(lua_State *L)
{
    LL_FUNC("ComponentFunction");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_float32 rnum = ll_check_l_float32(_fun, L, 2);
    l_float32 gnum = ll_check_l_float32(_fun, L, 3);
    l_float32 bnum = ll_check_l_float32(_fun, L, 4);
    l_float32 rdenom = ll_check_l_float32(_fun, L, 5);
    l_float32 gdenom = ll_check_l_float32(_fun, L, 6);
    l_float32 bdenom = ll_check_l_float32(_fun, L, 7);
    FPix *fpix = pixComponentFunction(pix, rnum, gnum, bnum, rdenom, gdenom, bdenom);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (dist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ConformsToRectangle(lua_State *L)
{
    LL_FUNC("ConformsToRectangle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 dist = ll_check_l_int32(_fun, L, 3);
    l_int32 conforms = 0;
    if (pixConformsToRectangle(pixs, box, dist, &conforms))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, conforms);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
ConnComp(lua_State *L)
{
    LL_FUNC("ConnComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = nullptr;
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    if (pixConnComp(pixs, &pixa, connectivity))
        return ll_push_nil(L);
    ll_push_Pixa(_fun, L, pixa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connect).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConnCompAreaTransform(lua_State *L)
{
    LL_FUNC("ConnCompAreaTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connect = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixConnCompAreaTransform(pixs, connect);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
ConnCompBB(lua_State *L)
{
    LL_FUNC("ConnCompBB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Boxa *result = pixConnCompBB(pixs, connectivity);
    return ll_push_Boxa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * Arg #3 is expected to be a l_float32 (x).
 * Arg #4 is expected to be a l_float32 (y).
 * Arg #5 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ConnCompIncrAdd(lua_State *L)
{
    LL_FUNC("ConnCompIncrAdd");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    l_float32 x = ll_check_l_float32(_fun, L, 3);
    l_float32 y = ll_check_l_float32(_fun, L, 4);
    l_int32 debug = ll_check_boolean_default(_fun, L, 5, FALSE);
    l_int32 ncc = 0;
    if (pixConnCompIncrAdd(pixs, ptaa, &ncc, x, y, debug))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (conn).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ConnCompIncrInit(lua_State *L)
{
    LL_FUNC("ConnCompIncrInit");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 conn = ll_check_l_int32(_fun, L, 2);
    Pix *pixd = nullptr;
    Ptaa *ptaa = nullptr;
    l_int32 ncc = 0;
    if (pixConnCompIncrInit(pixs, conn, &pixd, &ptaa, &ncc))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    ll_push_Ptaa(_fun, L, ptaa);
    ll_push_l_int32(_fun, L, ncc);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
ConnCompPixa(lua_State *L)
{
    LL_FUNC("ConnCompPixa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = nullptr;
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    if (pixConnCompPixa(pixs, &pixa, connectivity))
        return ll_push_nil(L);
    ll_push_Pixa(_fun, L, pixa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connect).
 * Arg #3 is expected to be a l_int32 (depth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConnCompTransform(lua_State *L)
{
    LL_FUNC("ConnCompTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connect = ll_check_l_int32(_fun, L, 2);
    l_int32 depth = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixConnCompTransform(pixs, connect, depth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a l_int32 (mindiff).
 * Arg #6 is expected to be a l_int32 (smoothx).
 * Arg #7 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ContrastNorm(lua_State *L)
{
    LL_FUNC("ContrastNorm");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 5);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 6);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixContrastNorm(pixd, pixs, sx, sy, mindiff, smoothx, smoothy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ContrastTRC(lua_State *L)
{
    LL_FUNC("ContrastTRC");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixContrastTRC(pixd, pixs, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_float32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ContrastTRCMasked(lua_State *L)
{
    LL_FUNC("ContrastTRCMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_float32 factor = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixContrastTRCMasked(pixd, pixs, pixm, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a gray 16bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the byte selection type (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert16To8(lua_State *L)
{
    LL_FUNC("Convert16To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_more_less_clip(_fun, L, 2, L_LS_BYTE);
    return ll_push_Pix(_fun, L, pixConvert16To8(pixs, type));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a 16 bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint16 (val0).
 * Arg #3 is expected to be a l_uint16 (val1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To16(lua_State *L)
{
    LL_FUNC("Convert1To16");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint16 val0 = ll_check_l_uint16(_fun, L, 2);
    l_uint16 val1 = ll_check_l_uint16(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvert1To16(nullptr, pixs, val0, val1));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a gray 2bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (val0).
 * Arg #3 is expected to be a l_int32 (val1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To2(lua_State *L)
{
    LL_FUNC("Convert1To2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val0 = ll_check_l_int32(_fun, L, 2);
    l_int32 val1 = ll_check_l_int32(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvert1To2(nullptr, pixs, val0, val1));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a colormapped 2bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To2Cmap(lua_State *L)
{
    LL_FUNC("Convert1To2Cmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert1To2Cmap(pixs));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (val0).
 * Arg #3 is expected to be a l_uint32 (val1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To32(lua_State *L)
{
    LL_FUNC("Convert1To32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 val0 = ll_check_l_uint32(_fun, L, 2);
    l_uint32 val1 = ll_check_l_uint32(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvert1To32(nullptr, pixs, val0, val1));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a 4bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (val0).
 * Arg #3 is expected to be a l_uint32 (val1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To4(lua_State *L)
{
    LL_FUNC("Convert1To4");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val0 = ll_check_l_int32(_fun, L, 2);
    l_int32 val1 = ll_check_l_int32(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvert1To4(nullptr, pixs, val0, val1));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a colormapped 4bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To4Cmap(lua_State *L)
{
    LL_FUNC("Convert1To4Cmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert1To4Cmap(pixs));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint8 (val0).
 * Arg #3 is expected to be a l_uint8 (val1).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To8(lua_State *L)
{
    LL_FUNC("Convert1To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint8 val0 = ll_check_l_uint8(_fun, L, 2);
    l_uint8 val1 = ll_check_l_uint8(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvert1To8(nullptr, pixs, val0, val1));
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a colormapped 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert1To8Cmap(lua_State *L)
{
    LL_FUNC("Convert1To8Cmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert1To8Cmap(pixs));
}

/**
 * \brief Convert a 24bpp Pix* (%pixs) to a 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert24To32(lua_State *L)
{
    LL_FUNC("Convert24To32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert24To32(pixs));
}

/**
 * \brief Convert a 2bpp Pix* (%pixs) to a 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint8 (val0).
 * Arg #3 is expected to be a l_uint8 (val1).
 * Arg #4 is expected to be a l_uint8 (val2).
 * Arg #5 is expected to be a l_uint8 (val3).
 * Arg #6 is expected to be a boolean (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert2To8(lua_State *L)
{
    LL_FUNC("Convert2To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint8 val0 = ll_check_l_uint8(_fun, L, 2);
    l_uint8 val1 = ll_check_l_uint8(_fun, L, 3);
    l_uint8 val2 = ll_check_l_uint8(_fun, L, 4);
    l_uint8 val3 = ll_check_l_uint8(_fun, L, 5);
    l_int32 cmapflag = ll_check_boolean(_fun, L, 6);
    return ll_push_Pix(_fun, L, pixConvert2To8(pixs, val0, val1, val2, val3, cmapflag));
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a 16bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the ms/ls 2 byte selection (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert32To16(lua_State *L)
{
    LL_FUNC("Convert32To16");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_more_less_clip(_fun, L, 2, L_LS_TWO_BYTES);
    return ll_push_Pix(_fun, L, pixConvert32To16(pixs, type));
}

/**
 * \brief Convert a 24bpp Pix* (%pixs) to a 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert32To24(lua_State *L)
{
    LL_FUNC("Convert32To24");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert24To32(pixs));
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a 16bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the ms/ls 2 byte selection (type16).
 * Arg #2 is expected to be a string describing the ms/ls byte selection (type8).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert32To8(lua_State *L)
{
    LL_FUNC("Convert32To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type16 = ll_check_more_less_clip(_fun, L, 2, L_LS_TWO_BYTES);
    l_int32 type8 = ll_check_more_less_clip(_fun, L, 2, L_LS_BYTE);
    return ll_push_Pix(_fun, L, pixConvert32To8(pixs, type16, type8));
}

/**
 * \brief Convert a 2bpp Pix* (%pixs) to a 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (val0).
 * Arg #3 is expected to be a l_uint32 (val1).
 * Arg #4 is expected to be a l_uint32 (val2).
 * Arg #5 is expected to be a l_uint32 (val3).
 * Arg #6 is expected to be a boolean (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert4To8(lua_State *L)
{
    LL_FUNC("Convert4To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 cmapflag = ll_check_boolean(_fun, L, 2);
    return ll_push_Pix(_fun, L, pixConvert4To8(pixs, cmapflag));
}

/**
 * \brief Convert a 8bpp Pix* (%pixs) to a 16bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (leftshift).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert8To16(lua_State *L)
{
    LL_FUNC("Convert4To8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 leftshift = ll_check_l_int32_default(_fun, L, 2, 0);
    return ll_push_Pix(_fun, L, pixConvert8To16(pixs, leftshift));
}

/**
 * \brief Convert a 8bpp Pix* (%pixs) to a 2bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert8To2(lua_State *L)
{
    LL_FUNC("Convert8To2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert8To2(pixs));
}

/**
 * \brief Convert a 8bpp Pix* (%pixs) to a 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert8To32(lua_State *L)
{
    LL_FUNC("Convert8To32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert8To32(pixs));
}

/**
 * \brief Convert a 8bpp Pix* (%pixs) to a 4bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Convert8To4(lua_State *L)
{
    LL_FUNC("Convert8To4");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvert8To4(pixs));
}

/**
 * \brief Convert a colormapped Pix* (%pixs) to a binary 1bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertCmapTo1(lua_State *L)
{
    LL_FUNC("ConvertCmapTo1");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertCmapTo1(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * Arg #4 is expected to be a l_int32 (order).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertColorToSubpixelRGB(lua_State *L)
{
    LL_FUNC("ConvertColorToSubpixelRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    l_int32 order = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixConvertColorToSubpixelRGB(pixs, scalex, scaley, order);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertForPSWrap(lua_State *L)
{
    LL_FUNC("ConvertForPSWrap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertForPSWrap(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a gray 8bpp Pix* (%pixs) to a colormapped Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertGrayToColormap(lua_State *L)
{
    LL_FUNC("ConvertGrayToColormap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertGrayToColormap(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a gray 8bpp Pix* (%pixs) to a colormapped Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mindepth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertGrayToColormap8(lua_State *L)
{
    LL_FUNC("ConvertGrayToColormap8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mindepth = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixConvertGrayToColormap8(pixs, mindepth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a colormapped Pix* (%pixs) to a binary 1bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (gamma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertGrayToFalseColor(lua_State *L)
{
    LL_FUNC("ConvertGrayToFalseColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 gamma = ll_check_l_float32_default(_fun, L, 2, 1.0f);
    return ll_push_Pix(_fun, L, pixConvertGrayToFalseColor(pixs, gamma));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * Arg #4 is expected to be a l_int32 (order).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertGrayToSubpixelRGB(lua_State *L)
{
    LL_FUNC("ConvertGrayToSubpixelRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    l_int32 order = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixConvertGrayToSubpixelRGB(pixs, scalex, scaley, order);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertHSVToRGB(lua_State *L)
{
    LL_FUNC("ConvertHSVToRGB");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixConvertHSVToRGB(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (d).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertLossless(lua_State *L)
{
    LL_FUNC("ConvertLossless");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 d = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixConvertLossless(pixs, d);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a binary 1bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rc).
 * Arg #3 is expected to be a l_float32 (gc).
 * Arg #4 is expected to be a l_float32 (bc).
 * Arg #5 is expected to be a l_int32 (thresh).
 * Arg #6 is expected to be a string describing the relation (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToBinaryArb(lua_State *L)
{
    LL_FUNC("ConvertRGBToBinaryArb");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rc = ll_check_l_float32_default(_fun, L, 2, 0.3f);
    l_float32 gc = ll_check_l_float32_default(_fun, L, 3, 0.5f);
    l_float32 bc = ll_check_l_float32_default(_fun, L, 4, 0.2f);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 5, 128);
    l_int32 relation = ll_check_relation(_fun, L, 6, L_SELECT_IF_LT);
    Pix *pix = pixConvertRGBToBinaryArb(pixs, rc, gc, bc, thresh, relation);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a colormapped 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a boolean (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToColormap(lua_State *L)
{
    LL_FUNC("ConvertRGBToColormap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ditherflag = ll_check_boolean(_fun, L, 2);
    Pix *pix = pixConvertRGBToColormap(pixs, ditherflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rwt).
 * Arg #3 is expected to be a l_float32 (gwt).
 * Arg #4 is expected to be a l_float32 (bwt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToGray(lua_State *L)
{
    LL_FUNC("ConvertRGBToGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rwt = ll_check_l_float32_default(_fun, L, 2, 0.3f);
    l_float32 gwt = ll_check_l_float32_default(_fun, L, 2, 0.5f);
    l_float32 bwt = ll_check_l_float32_default(_fun, L, 2, 0.2f);
    Pix *pix = pixConvertRGBToGray(pixs, rwt, gwt, bwt);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rc).
 * Arg #3 is expected to be a l_float32 (gc).
 * Arg #4 is expected to be a l_float32 (bc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToGrayArb(lua_State *L)
{
    LL_FUNC("ConvertRGBToGrayArb");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rc = ll_check_l_float32_default(_fun, L, 2, 0.3f);
    l_float32 gc = ll_check_l_float32_default(_fun, L, 2, 0.5f);
    l_float32 bc = ll_check_l_float32_default(_fun, L, 2, 0.2f);
    Pix *pix = pixConvertRGBToGrayArb(pixs, rc, gc, bc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix); fast version.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToGrayFast(lua_State *L)
{
    LL_FUNC("ConvertRGBToGrayFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertRGBToGrayFast(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix); min/max/diff version.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string defining the min/max type (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToGrayMinMax(lua_State *L)
{
    LL_FUNC("ConvertRGBToGrayMinMax");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_choose_min_max(_fun, L, 2, L_CHOOSE_MIN);
    Pix *pix = pixConvertRGBToGrayMinMax(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a gray 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (refval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToGraySatBoost(lua_State *L)
{
    LL_FUNC("ConvertRGBToGraySatBoost");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 refval = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixConvertRGBToGraySatBoost(pixs, refval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertRGBToHSV(lua_State *L)
{
    LL_FUNC("ConvertRGBToHSV");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixConvertRGBToHSV(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertRGBToHue(lua_State *L)
{
    LL_FUNC("ConvertRGBToHue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertRGBToHue(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa * on the Lua stack
 */
static int
ConvertRGBToLAB(lua_State *L)
{
    LL_FUNC("ConvertRGBToLAB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    FPixa *result = pixConvertRGBToLAB(pixs);
    return ll_push_FPixa(_fun, L, result);
}

/**
 * \brief Convert a 32bpp Pix* (%pixs) to a luminance 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertRGBToLuminance(lua_State *L)
{
    LL_FUNC("ConvertRGBToLuminance");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertRGBToLuminance(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertRGBToSaturation(lua_State *L)
{
    LL_FUNC("ConvertRGBToSaturation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertRGBToSaturation(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertRGBToValue(lua_State *L)
{
    LL_FUNC("ConvertRGBToValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixConvertRGBToValue(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa * on the Lua stack
 */
static int
ConvertRGBToXYZ(lua_State *L)
{
    LL_FUNC("ConvertRGBToXYZ");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    FPixa *result = pixConvertRGBToXYZ(pixs);
    return ll_push_FPixa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertRGBToYUV(lua_State *L)
{
    LL_FUNC("ConvertRGBToYUV");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixConvertRGBToYUV(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a Pix* (%pixs) to a 1 bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (threshold).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo1(lua_State *L)
{
    LL_FUNC("ConvertTo1");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 threshold = ll_check_l_int32_default(_fun, L, 4, 128);
    Pix *pix = pixConvertTo1(pixs, threshold);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a Pix* (%pixs) to a 16bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo16(lua_State *L)
{
    LL_FUNC("ConvertTo16");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvertTo16(pixs));
}

/**
 * \brief Convert a Pix* (%pixs) to a binary 1bpp Pix* (%pix) by sampling.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (threshold).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo1BySampling(lua_State *L)
{
    LL_FUNC("ConvertTo1BySampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 threshold = ll_check_l_int32(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvertTo1BySampling(pixs, factor, threshold));
}

/**
 * \brief Convert a Pix* (%pixs) to a 2bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo2(lua_State *L)
{
    LL_FUNC("ConvertTo2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvertTo2(pixs));
}

/**
 * \brief Convert a Pix* (%pixs) to a 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo32(lua_State *L)
{
    LL_FUNC("ConvertTo32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvertTo32(pixs));
}

/**
 * \brief Convert a Pix* (%pixs) to a 32bpp Pix* (%pix) by sampling.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo32BySampling(lua_State *L)
{
    LL_FUNC("ConvertTo32BySampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Pix(_fun, L, pixConvertTo32BySampling(pixs, factor));
}

/**
 * \brief Convert a Pix* (%pixs) to a 4bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo4(lua_State *L)
{
    LL_FUNC("ConvertTo4");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_Pix(_fun, L, pixConvertTo4(pixs));
}

/**
 * \brief Convert a Pix* (%pixs) to a 8bpp Pix* (%pix); colormapped or gray.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a boolean (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo8(lua_State *L)
{
    LL_FUNC("ConvertTo8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 cmapflag = ll_check_boolean(_fun, L, 2);
    return ll_push_Pix(_fun, L, pixConvertTo8(pixs, cmapflag));
}

/**
 * \brief Convert a Pix* (%pixs) to a binary 1bpp Pix* (%pix) by sampling.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a boolean (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo8BySampling(lua_State *L)
{
    LL_FUNC("ConvertTo8BySampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 cmapflag = ll_check_boolean(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvertTo8BySampling(pixs, factor, cmapflag));
}

/**
 * \brief Convert a Pix* (%pixs) to a colormapped 8bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a boolean (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo8Colormap(lua_State *L)
{
    LL_FUNC("ConvertTo8Colormap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ditherflag = ll_check_boolean(_fun, L, 2);
    return ll_push_Pix(_fun, L, pixConvertTo8Colormap(pixs, ditherflag));
}

/**
 * \brief Convert a Pix* (%pixs) to a 8bpp or 32bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is exptected to be a string describing the copy/clone mode (copyflag).
 * Arg #3 is exptected to be a boolean (wanrflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertTo8Or32(lua_State *L)
{
    LL_FUNC("ConvertTo8Or32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    l_int32 warnflag = ll_check_boolean(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixConvertTo8Or32(pixs, copyflag, warnflag));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ncomps).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DPix * on the Lua stack
 */
static int
ConvertToDPix(lua_State *L)
{
    LL_FUNC("ConvertToDPix");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ncomps = ll_check_l_int32(_fun, L, 2);
    DPix *dpix = pixConvertToDPix(pixs, ncomps);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ncomps).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix * on the Lua stack
 */
static int
ConvertToFPix(lua_State *L)
{
    LL_FUNC("ConvertToFPix");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ncomps = ll_check_l_int32(_fun, L, 2);
    FPix *fpix = pixConvertToFPix(pixs, ncomps);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a string (fileout).
 * Arg #5 is expected to be a l_int32 (x).
 * Arg #6 is expected to be a l_int32 (y).
 * Arg #7 is expected to be a l_int32 (res).
 * Arg #8 is expected to be a string (title).
 * Arg #9 is expected to be a string describing the position (position).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ConvertToPdf(lua_State *L)
{
    LL_FUNC("ConvertToPdf");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_compression(_fun, L, 2, IFF_DEFAULT);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    const char *fileout = ll_check_string(_fun, L, 4);
    l_int32 x = ll_check_l_int32(_fun, L, 5);
    l_int32 y = ll_check_l_int32(_fun, L, 6);
    l_int32 res = ll_check_l_int32(_fun, L, 7);
    const char *title = ll_check_string(_fun, L, 8);
    l_int32 position = ll_check_position(_fun, L, 9, 0);
    PdfData *lpd = nullptr;
    if (pixConvertToPdf(pix, type, quality, fileout, x, y, res, title, position ? &lpd : nullptr, position))
        return ll_push_nil(L);
    ll_push_PdfData(_fun, L, lpd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_int32 (res).
 * Arg #7 is expected to be a string (title).
 * Arg #8 is expected to be a string describing the position (position).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 lstring (%data, %nbytes) and PdfData* (%lpd) on the Lua stack
 */
static int
ConvertToPdfData(lua_State *L)
{
    LL_FUNC("ConvertToPdfData");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_int32 res = ll_check_l_int32(_fun, L, 6);
    const char *title = ll_check_string(_fun, L, 7);
    l_int32 position = ll_check_position(_fun, L, 8, 0);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    PdfData *lpd = nullptr;
    if (pixConvertToPdfData(pix, type, quality, &data, &nbytes, x, y, res, title, position ? &lpd : nullptr, position))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), nbytes);
    ll_free(data);
    ll_push_PdfData(_fun, L, lpd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a Boxa* (boxa).
 * Arg #6 is expected to be a l_int32 (quality).
 * Arg #7 is expected to be a l_float32 (scalefactor).
 * Arg #8 is expected to be a string (title).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ConvertToPdfDataSegmented(lua_State *L)
{
    LL_FUNC("ConvertToPdfDataSegmented");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    Boxa *boxa = ll_check_Boxa(_fun, L, 5);
    l_int32 quality = ll_check_l_int32(_fun, L, 6);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 7);
    const char *title = ll_check_string(_fun, L, 8);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (pixConvertToPdfDataSegmented(pixs, res, type, thresh, boxa, quality, scalefactor, title, &data, &nbytes))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), nbytes);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a Boxa* (boxa).
 * Arg #6 is expected to be a l_int32 (quality).
 * Arg #7 is expected to be a l_float32 (scalefactor).
 * Arg #8 is expected to be a string (title).
 * Arg #9 is expected to be a string (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ConvertToPdfSegmented(lua_State *L)
{
    LL_FUNC("ConvertToPdfSegmented");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    Boxa *boxa = ll_check_Boxa(_fun, L, 5);
    l_int32 quality = ll_check_l_int32(_fun, L, 6);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 7);
    const char *title = ll_check_string(_fun, L, 8);
    const char *fileout = ll_check_string(_fun, L, 9);
    return ll_push_boolean(_fun, L, 0 == pixConvertToPdfSegmented(pixs, res, type, thresh, boxa, quality, scalefactor, title, fileout));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * Arg #4 is expected to be a l_int32 (order).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertToSubpixelRGB(lua_State *L)
{
    LL_FUNC("ConvertToSubpixelRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    l_int32 order = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixConvertToSubpixelRGB(pixs, scalex, scaley, order);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvertYUVToRGB(lua_State *L)
{
    LL_FUNC("ConvertYUVToRGB");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixConvertYUVToRGB(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kel).
 * Arg #3 is expected to be a l_int32 (outdepth).
 * Arg #4 is expected to be a l_int32 (normflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Convolve(lua_State *L)
{
    LL_FUNC("Convolve");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kel = ll_check_Kernel(_fun, L, 2);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 3);
    l_int32 normflag = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixConvolve(pixs, kel, outdepth, normflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvolveRGB(lua_State *L)
{
    LL_FUNC("ConvolveRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kel = ll_check_Kernel(_fun, L, 2);
    Pix *pix = pixConvolveRGB(pixs, kel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kelx).
 * Arg #3 is expected to be a Kernel* (kely).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvolveRGBSep(lua_State *L)
{
    LL_FUNC("ConvolveRGBSep");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kelx = ll_check_Kernel(_fun, L, 2);
    Kernel *kely = ll_check_Kernel(_fun, L, 3);
    Pix *pix = pixConvolveRGBSep(pixs, kelx, kely);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kelx).
 * Arg #3 is expected to be a Kernel* (kely).
 * Arg #4 is expected to be a l_int32 (outdepth).
 * Arg #5 is expected to be a l_int32 (normflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvolveSep(lua_State *L)
{
    LL_FUNC("ConvolveSep");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kelx = ll_check_Kernel(_fun, L, 2);
    Kernel *kely = ll_check_Kernel(_fun, L, 3);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 4);
    l_int32 normflag = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixConvolveSep(pixs, kelx, kely, outdepth, normflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kel1).
 * Arg #3 is expected to be a Kernel* (kel2).
 * Arg #4 is expected to be a l_int32 (force8).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ConvolveWithBias(lua_State *L)
{
    LL_FUNC("ConvolveWithBias");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kel1 = ll_check_Kernel(_fun, L, 2);
    Kernel *kel2 = ll_check_Kernel(_fun, L, 3);
    l_int32 force8 = ll_check_l_int32(_fun, L, 4);
    l_int32 bias = 0;
    if (pixConvolveWithBias(pixs, kel1, kel2, force8, &bias))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, bias);
    return 1;
}

/**
 * \brief Copy a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixd = pixCopy(nullptr, pixs);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Copy border pixels from a Pix* (%pixs) to a new Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CopyBorder(lua_State *L)
{
    LL_FUNC("CopyBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pix = pixCopyBorder(nullptr, pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Copy the colormap of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyColormap(lua_State *L)
{
    LL_FUNC("CopyColormap");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopyColormap(pixd, pixs));
}

/**
 * \brief Copy dimensions from a Pix* (pixs) to self (pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyDimensions(lua_State *L)
{
    LL_FUNC("CopyDimensions");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopyDimensions(pixd, pixs));
}

/**
 * \brief Copy input format a Pix* (%pixs) to a Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyInputFormat(lua_State *L)
{
    LL_FUNC("CopyInputFormat");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopyInputFormat(pixd, pixs));
}

/**
 * \brief Copy one component in Pix* (%pixd) from Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a string with the component name (comp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyRGBComponent(lua_State *L)
{
    LL_FUNC("CopyRGBComponent");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 comp = ll_check_component(_fun, L, 3, L_ALPHA_CHANNEL);
    return ll_push_boolean(_fun, L, 0 == pixCopyRGBComponent(pixd, pixs, comp));
}

/**
 * \brief Copy resolution from a Pix* (%pixs) to a Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyResolution(lua_State *L)
{
    LL_FUNC("CopyResolution");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopyResolution(pixd, pixs));
}

/**
 * \brief Copy samples per pixel (%spp) from a Pix* (%pixs) to Pix* (%pixd).
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopySpp(lua_State *L)
{
    LL_FUNC("CopySpp");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopySpp(pixd, pixs));
}

/**
 * \brief Copy text from a Pix* (%pixs) to Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyText(lua_State *L)
{
    LL_FUNC("CopyText");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixCopyText(pixd, pixs));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CorrelationBinary(lua_State *L)
{
    LL_FUNC("CorrelationBinary");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_float32 val = 0;
    if (pixCorrelationBinary(pix1, pix2, &val))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, val);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (area1).
 * Arg #4 is expected to be a l_int32 (area2).
 * Arg #5 is expected to be a l_float32 (delx).
 * Arg #6 is expected to be a l_float32 (dely).
 * Arg #7 is expected to be a l_int32 (maxdiffw).
 * Arg #8 is expected to be a l_int32 (maxdiffh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CorrelationScore(lua_State *L)
{
    LL_FUNC("CorrelationScore");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 area1 = ll_check_l_int32(_fun, L, 3);
    l_int32 area2 = ll_check_l_int32(_fun, L, 4);
    l_float32 delx = ll_check_l_float32(_fun, L, 5);
    l_float32 dely = ll_check_l_float32(_fun, L, 6);
    l_int32 maxdiffw = ll_check_l_int32(_fun, L, 7);
    l_int32 maxdiffh = ll_check_l_int32(_fun, L, 8);
    l_int32 tab = 0;
    l_float32 score = 0;
    if (pixCorrelationScore(pix1, pix2, area1, area2, delx, dely, maxdiffw, maxdiffh, &tab, &score))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, score);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (area1).
 * Arg #4 is expected to be a l_int32 (area2).
 * Arg #5 is expected to be a l_int32 (delx).
 * Arg #6 is expected to be a l_int32 (dely).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CorrelationScoreShifted(lua_State *L)
{
    LL_FUNC("CorrelationScoreShifted");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 area1 = ll_check_l_int32(_fun, L, 3);
    l_int32 area2 = ll_check_l_int32(_fun, L, 4);
    l_int32 delx = ll_check_l_int32(_fun, L, 5);
    l_int32 dely = ll_check_l_int32(_fun, L, 6);
    l_int32 tab = 0;
    l_float32 score = 0;
    if (pixCorrelationScoreShifted(pix1, pix2, area1, area2, delx, dely, &tab, &score))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, score);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (area1).
 * Arg #4 is expected to be a l_int32 (area2).
 * Arg #5 is expected to be a l_float32 (delx).
 * Arg #6 is expected to be a l_float32 (dely).
 * Arg #7 is expected to be a l_int32 (maxdiffw).
 * Arg #8 is expected to be a l_int32 (maxdiffh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CorrelationScoreSimple(lua_State *L)
{
    LL_FUNC("CorrelationScoreSimple");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 area1 = ll_check_l_int32(_fun, L, 3);
    l_int32 area2 = ll_check_l_int32(_fun, L, 4);
    l_float32 delx = ll_check_l_float32(_fun, L, 5);
    l_float32 dely = ll_check_l_float32(_fun, L, 6);
    l_int32 maxdiffw = ll_check_l_int32(_fun, L, 7);
    l_int32 maxdiffh = ll_check_l_int32(_fun, L, 8);
    l_int32 tab = 0;
    l_float32 score = 0;
    if (pixCorrelationScoreSimple(pix1, pix2, area1, area2, delx, dely, maxdiffw, maxdiffh, &tab, &score))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, score);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (area1).
 * Arg #4 is expected to be a l_int32 (area2).
 * Arg #5 is expected to be a l_float32 (delx).
 * Arg #6 is expected to be a l_float32 (dely).
 * Arg #7 is expected to be a l_int32 (maxdiffw).
 * Arg #8 is expected to be a l_int32 (maxdiffh).
 * Arg #11 is expected to be a l_float32 (score_threshold).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CorrelationScoreThresholded(lua_State *L)
{
    LL_FUNC("CorrelationScoreThresholded");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 area1 = ll_check_l_int32(_fun, L, 3);
    l_int32 area2 = ll_check_l_int32(_fun, L, 4);
    l_float32 delx = ll_check_l_float32(_fun, L, 5);
    l_float32 dely = ll_check_l_float32(_fun, L, 6);
    l_int32 maxdiffw = ll_check_l_int32(_fun, L, 7);
    l_int32 maxdiffh = ll_check_l_int32(_fun, L, 8);
    l_int32 tab = 0;
    l_int32 downcount = 0;
    l_float32 score_threshold = ll_check_l_float32(_fun, L, 11);
    if (pixCorrelationScoreThresholded(pix1, pix2, area1, area2, delx, dely, maxdiffw, maxdiffh, &tab, &downcount, score_threshold))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_int32(_fun, L, downcount);
    return 2;
}

/**
 * \brief Count pixels of arbitrary value (%val) in Box* (%box) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (val).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountArbInRect(lua_State *L)
{
    LL_FUNC("CountArbInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Box *box = ll_check_Box_opt(_fun, L, 4);
    l_int32 count;
    if (pixCountArbInRect(pixs, box, val, factor, &count))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
CountByColumn(lua_State *L)
{
    LL_FUNC("CountByColumn");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Numa *result = pixCountByColumn(pix, box);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
CountByRow(lua_State *L)
{
    LL_FUNC("CountByRow");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Numa *result = pixCountByRow(pix, box);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CountConnComp(lua_State *L)
{
    LL_FUNC("CountConnComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    l_int32 count = 0;
    if (pixCountConnComp(pixs, connectivity, &count))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixels(lua_State *L)
{
    LL_FUNC("CountPixels");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 count = 0;
    if (pixCountPixels(pixs, &count, nullptr))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Count the number of pixels by column in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByColumn(lua_State *L)
{
    LL_FUNC("CountPixelsByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Numa *na = pixCountPixelsByColumn(pixs);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Count the number of pixels by row in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByRow(lua_State *L)
{
    LL_FUNC("CountPixelsByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Numa *na = pixCountPixelsByRow(pixs, nullptr);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs) inside a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixelsInRect(lua_State *L)
{
    LL_FUNC("CountPixelsInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRect(pixs, box, &count, nullptr))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Count the number of pixels in row (%row) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsInRow(lua_State *L)
{
    LL_FUNC("CountPixelsInRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRow(pixs, row, &count, nullptr))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Count the RGB colors in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountRGBColors(lua_State *L)
{
    LL_FUNC("CountRGBColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 count = pixCountRGBColors(pixs);
    return ll_push_l_int32(_fun, L, count);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (deltafract).
 * Arg #3 is expected to be a l_float32 (peakfract).
 * Arg #4 is expected to be a l_float32 (clipfract).
 * Arg #5 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CountTextColumns(lua_State *L)
{
    LL_FUNC("CountTextColumns");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 deltafract = ll_check_l_float32(_fun, L, 2);
    l_float32 peakfract = ll_check_l_float32(_fun, L, 3);
    l_float32 clipfract = ll_check_l_float32(_fun, L, 4);
    Pixa *pixadb = ll_check_Pixa(_fun, L, 5);
    l_int32 ncols = 0;
    if (pixCountTextColumns(pixs, deltafract, peakfract, clipfract, &ncols, pixadb))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncols);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixComp* (pixc).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CreateFromPixcomp(lua_State *L)
{
    LL_FUNC("CreateFromPixcomp");
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    Pix *pix = pixCreateFromPixcomp(pixc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is expected to be a l_int32 (depth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CreateHeader(lua_State *L)
{
    LL_FUNC("CreateHeader");
    l_int32 width = ll_check_l_int32(_fun, L, 1);
    l_int32 height = ll_check_l_int32(_fun, L, 2);
    l_int32 depth = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixCreateHeader(width, height, depth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Create a new Pix* but don't initialize it.
 * <pre>
 * Arg #1 is expected to be Pix*.
 * or
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1).
 * or
 * No Arg creates a 1x1 1bpp uninitialized Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CreateNoInit(lua_State *L)
{
    LL_FUNC("CreateNoInit");
    Pix *pixs = ll_check_Pix_opt(_fun, L, 1);
    Pix *pix = nullptr;
    if (pixs) {
        pix = pixCreateTemplateNoInit(pixs);
    } else if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32(_fun, L, 1);
        l_int32 height = ll_check_l_int32(_fun, L, 2);
        l_int32 depth = ll_check_l_int32_default(_fun, L, 3, 1);
        pix = pixCreateNoInit(width, height, depth);
    } else {
        pix = pixCreateNoInit(1, 1, 1);
    }
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Create a new Pix* (%pixd) from three layer Pix* (%pixr, %pixg, %pixb).
 * <pre>
 * Arg #1 is expected to be a Pix* (pixr).
 * Arg #2 is expected to be a Pix* (pixg).
 * Arg #3 is expected to be a Pix* (pixb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (pixd) on the Lua stack
 */
static int
CreateRGBImage(lua_State *L)
{
    LL_FUNC("CreateRGBImage");
    Pix *pixr = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    Pix *pixb = ll_check_Pix(_fun, L, 3);
    Pix *pixd = pixCreateRGBImage(pixr, pixg, pixb);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Create a new Pix* but don't initialize it.
 * <pre>
 * Arg #1 is expected to be Pix*.
 * or
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1).
 * or
 * No Arg creates a 1x1 1bpp uninitialized Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CreateTemplate(lua_State *L)
{
    LL_FUNC("CreateTemplate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixCreateTemplate(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Create a new Pix* but don't initialize it.
 * <pre>
 * Arg #1 is expected to be Pix*.
 * or
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1).
 * or
 * No Arg creates a 1x1 1bpp uninitialized Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CreateTemplateNoInit(lua_State *L)
{
    LL_FUNC("CreateTemplateNoInit");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixCreateTemplateNoInit(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CropAlignedToCentroid(lua_State *L)
{
    LL_FUNC("CropAlignedToCentroid");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Box *box1 = nullptr;
    Box *box2 = nullptr;
    if (pixCropAlignedToCentroid(pix1, pix2, factor, &box1, &box2))
        return ll_push_nil(L);
    ll_push_Box(_fun, L, box1);
    ll_push_Box(_fun, L, box2);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
CropToMatch(lua_State *L)
{
    LL_FUNC("CropToMatch");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    Pix *pixd1 = nullptr;
    Pix *pixd2 = nullptr;
    if (pixCropToMatch(pixs1, pixs2, &pixd1, &pixd2))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd1);
    ll_push_Pix(_fun, L, pixd2);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
CropToSize(lua_State *L)
{
    LL_FUNC("CropToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixCropToSize(pixs, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (thresh).
 * Arg #4 is expected to be a l_int32 (satlimit).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DarkenGray(lua_State *L)
{
    LL_FUNC("DarkenGray");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 thresh = ll_check_l_int32(_fun, L, 3);
    l_int32 satlimit = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDarkenGray(pixd, pixs, thresh, satlimit);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (nx).
 * Arg #4 is expected to be a l_int32 (ny).
 * Arg #5 is expected to be a l_float32 (thresh).
 * Arg #6 is expected to be a Pixa* (pixadebug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
DecideIfPhotoImage(lua_State *L)
{
    LL_FUNC("DecideIfPhotoImage");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 nx = ll_check_l_int32(_fun, L, 3);
    l_int32 ny = ll_check_l_int32(_fun, L, 4);
    l_float32 thresh = ll_check_l_float32(_fun, L, 5);
    Pixa *pixadebug = ll_check_Pixa_opt(_fun, L, 6);
    Numaa *naa = nullptr;
    if (pixDecideIfPhotoImage(pix, factor, nx, ny, thresh, &naa, pixadebug))
        return ll_push_nil(L);
    ll_push_Numaa(_fun, L, naa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (orient).
 * Arg #5 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
DecideIfTable(lua_State *L)
{
    LL_FUNC("DecideIfTable");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 orient = ll_check_l_int32(_fun, L, 3);
    l_int32 score = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 5);
    if (pixDecideIfTable(pixs, box, orient, &score, pixadb))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, score);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #4 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
DecideIfText(lua_State *L)
{
    LL_FUNC("DecideIfText");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 istext = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 4);
    if (pixDecideIfText(pixs, box, &istext, pixadb))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, istext);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a constl_uint32* (data).
 * Arg #2 is expected to be a size_t (nbytes).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DeserializeFromMemory(lua_State *L)
{
    LL_FUNC("DeserializeFromMemory");
    l_int32 len = 0;
    const l_uint32 *data = ll_unpack_Uarray(_fun, L, 1, &len);
    size_t nbytes = sizeof(l_uint32) * static_cast<size_t>(len);
    Pix *pix = pixDeserializeFromMemory(data, nbytes);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (redsearch).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Deskew(lua_State *L)
{
    LL_FUNC("Deskew");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixDeskew(pixs, redsearch);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixb).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (margin).
 * Arg #5 is expected to be a l_int32 (threshold).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DeskewBarcode(lua_State *L)
{
    LL_FUNC("DeskewBarcode");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixb = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 margin = ll_check_l_int32(_fun, L, 4);
    l_int32 threshold = ll_check_l_int32(_fun, L, 5);
    l_float32 angle = 0;
    l_float32 conf = 0;
    if (pixDeskewBarcode(pixs, pixb, box, margin, threshold, &angle, &conf))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (redsearch).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DeskewBoth(lua_State *L)
{
    LL_FUNC("DeskewBoth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixDeskewBoth(pixs, redsearch);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (redsweep).
 * Arg #3 is expected to be a l_float32 (sweeprange).
 * Arg #4 is expected to be a l_float32 (sweepdelta).
 * Arg #5 is expected to be a l_int32 (redsearch).
 * Arg #6 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DeskewGeneral(lua_State *L)
{
    LL_FUNC("DeskewGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 redsweep = ll_check_l_int32(_fun, L, 2);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 3);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 4);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 5);
    l_int32 thresh = ll_check_l_int32(_fun, L, 6);
    l_float32 angle = 0;
    l_float32 conf = 0;
    if (pixDeskewGeneral(pixs, redsweep, sweeprange, sweepdelta, redsearch, thresh, &angle, &conf))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nslices).
 * Arg #3 is expected to be a l_int32 (redsweep).
 * Arg #4 is expected to be a l_int32 (redsearch).
 * Arg #5 is expected to be a l_float32 (sweeprange).
 * Arg #6 is expected to be a l_float32 (sweepdelta).
 * Arg #7 is expected to be a l_float32 (minbsdelta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DeskewLocal(lua_State *L)
{
    LL_FUNC("DeskewLocal");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nslices = ll_check_l_int32(_fun, L, 2);
    l_int32 redsweep = ll_check_l_int32(_fun, L, 3);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 4);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 5);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 6);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 7);
    Pix *pix = pixDeskewLocal(pixs, nslices, redsweep, redsearch, sweeprange, sweepdelta, minbsdelta);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Destroy the colormap of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a PixColormap* (colormap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
DestroyColormap(lua_State *L)
{
    LL_FUNC("DestroyColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* colormap = ll_take_PixColormap(_fun, L, 2);
    (void)colormap;
    return ll_push_boolean(_fun, L, 0 == pixDestroyColormap(pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Dilate(lua_State *L)
{
    LL_FUNC("Dilate");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixDilate(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateBrick(lua_State *L)
{
    LL_FUNC("DilateBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDilateBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateBrickDwa(lua_State *L)
{
    LL_FUNC("DilateBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDilateBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateCompBrick(lua_State *L)
{
    LL_FUNC("DilateCompBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDilateCompBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateCompBrickDwa(lua_State *L)
{
    LL_FUNC("DilateCompBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDilateCompBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateCompBrickExtendDwa(lua_State *L)
{
    LL_FUNC("DilateCompBrickExtendDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDilateCompBrickExtendDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateGray(lua_State *L)
{
    LL_FUNC("DilateGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixDilateGray(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DilateGray3(lua_State *L)
{
    LL_FUNC("DilateGray3");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixDilateGray3(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Display(lua_State *L)
{
    LL_FUNC("Display");
    char title[256];
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    snprintf(title, sizeof(title), LL_PIX "*: %p", reinterpret_cast<void *>(pixs));
#if defined(HAVE_SDL2)
    return ll_push_boolean(_fun, L, DisplaySDL2(pixs, x, y, title));
#else
    return ll_push_boolean(_fun, L, 0 == pixDisplay(pixs, x, y));
#endif
}

/**
 * \brief Display a color array creating a Pix* (%pix).
 * <pre>
 * Arg #1 is expected to be a l_int32 (side).
 * Arg #2 is expected to be a l_int32 (ncols).
 * Arg #3 is expected to be a l_int32 (fontsize).
 * Arg #4 is expected to be a Lua array table (carray).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
DisplayColorArray(lua_State *L)
{
    LL_FUNC("DisplayColorArray");
    l_int32 side = ll_check_l_int32(_fun, L, 1);
    l_int32 ncols = ll_check_l_int32(_fun, L, 2);
    l_int32 fontsize = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 ncolors = 0;
    l_uint32 *carray = ll_unpack_Uarray(_fun, L, 4, &ncolors);
    Pix *pixd = pixDisplayColorArray(carray, ncolors, side, ncols, fontsize);
    LEPT_FREE(carray);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayDiffBinary(lua_State *L)
{
    LL_FUNC("DisplayDiffBinary");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixDisplayDiffBinary(pix1, pix2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Sel* (sel).
 * Arg #3 is expected to be a l_int32 (scalefactor).
 * Arg #4 is expected to be a l_uint32 (hitcolor).
 * Arg #5 is expected to be a l_uint32 (misscolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayHitMissSel(lua_State *L)
{
    LL_FUNC("DisplayHitMissSel");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Sel *sel = ll_check_Sel(_fun, L, 2);
    l_int32 scalefactor = ll_check_l_int32(_fun, L, 3);
    l_uint32 hitcolor = ll_check_l_uint32(_fun, L, 4);
    l_uint32 misscolor = ll_check_l_uint32(_fun, L, 5);
    Pix *pix = pixDisplayHitMissSel(pixs, sel, scalefactor, hitcolor, misscolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Display the layers of a Pix* (pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (val).
 * Arg #3 is expected to be a l_int32 (maxw).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (pixd) on the Lua stack
 */
static int
DisplayLayersRGBA(lua_State *L)
{
    LL_FUNC("DisplayLayersRGBA");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    l_int32 maxw = ll_check_l_int32(_fun, L, 3);
    Pix *pixd = pixDisplayLayersRGBA(pixs, val, maxw);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixp).
 * Arg #3 is expected to be a Pix* (pixe).
 * Arg #4 is expected to be a l_int32 (x0).
 * Arg #5 is expected to be a l_int32 (y0).
 * Arg #6 is expected to be a l_uint32 (color).
 * Arg #7 is expected to be a l_float32 (scale).
 * Arg #8 is expected to be a l_int32 (nlevels).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayMatchedPattern(lua_State *L)
{
    LL_FUNC("DisplayMatchedPattern");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixp = ll_check_Pix(_fun, L, 2);
    Pix *pixe = ll_check_Pix(_fun, L, 3);
    l_int32 x0 = ll_check_l_int32(_fun, L, 4);
    l_int32 y0 = ll_check_l_int32(_fun, L, 5);
    l_uint32 color = ll_check_l_uint32(_fun, L, 6);
    l_float32 scale = ll_check_l_float32(_fun, L, 7);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 8);
    Pix *pix = pixDisplayMatchedPattern(pixs, pixp, pixe, x0, y0, color, scale, nlevels);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pta* (pta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayPta(lua_State *L)
{
    LL_FUNC("DisplayPta");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pta *pta = ll_check_Pta(_fun, L, 3);
    Pix *pix = pixDisplayPta(pixd, pixs, pta);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pta* (pta).
 * Arg #4 is expected to be a Pix* (pixp).
 * Arg #5 is expected to be a l_int32 (cx).
 * Arg #6 is expected to be a l_int32 (cy).
 * Arg #7 is expected to be a l_uint32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayPtaPattern(lua_State *L)
{
    LL_FUNC("DisplayPtaPattern");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pta *pta = ll_check_Pta(_fun, L, 3);
    Pix *pixp = ll_check_Pix(_fun, L, 4);
    l_int32 cx = ll_check_l_int32(_fun, L, 5);
    l_int32 cy = ll_check_l_int32(_fun, L, 6);
    l_uint32 color = ll_check_l_uint32(_fun, L, 7);
    Pix *pix = pixDisplayPtaPattern(pixd, pixs, pta, pixp, cx, cy, color);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayPtaa(lua_State *L)
{
    LL_FUNC("DisplayPtaa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    Pix *pix = pixDisplayPtaa(pixs, ptaa);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Ptaa* (ptaa).
 * Arg #4 is expected to be a Pix* (pixp).
 * Arg #5 is expected to be a l_int32 (cx).
 * Arg #6 is expected to be a l_int32 (cy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayPtaaPattern(lua_State *L)
{
    LL_FUNC("DisplayPtaaPattern");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 3);
    Pix *pixp = ll_check_Pix(_fun, L, 4);
    l_int32 cx = ll_check_l_int32(_fun, L, 5);
    l_int32 cy = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixDisplayPtaaPattern(pixd, pixs, ptaa, pixp, cx, cy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a string (title).
 * Arg #5 is expected to be a l_int32 (dispflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
DisplayWithTitle(lua_State *L)
{
    LL_FUNC("DisplayWithTitle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    const char *title = ll_check_string(_fun, L, 4);
    l_int32 dispflag = ll_check_boolean(_fun, L, 5);
#if defined(HAVE_SDL2)
    UNUSED(dispflag);
    return ll_push_boolean(_fun, L, DisplaySDL2(pixs, x, y, title));
#else
    return ll_push_boolean(_fun, L, 0 == pixDisplayWithTitle(pixs, x, y, title, dispflag));
#endif
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (reduction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
DisplayWrite(lua_State *L)
{
    LL_FUNC("DisplayWrite");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 reduction = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixDisplayWrite(pixs, reduction));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * Arg #3 is expected to be a l_int32 (outdepth).
 * Arg #4 is expected to be a l_int32 (boundcond).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DistanceFunction(lua_State *L)
{
    LL_FUNC("DistanceFunction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 3);
    l_int32 boundcond = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDistanceFunction(pixs, connectivity, outdepth, boundcond);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DitherTo2bpp(lua_State *L)
{
    LL_FUNC("DitherTo2bpp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixDitherTo2bpp(pixs, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (lowerclip).
 * Arg #3 is expected to be a l_int32 (upperclip).
 * Arg #4 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DitherTo2bppSpec(lua_State *L)
{
    LL_FUNC("DitherTo2bppSpec");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 lowerclip = ll_check_l_int32(_fun, L, 2);
    l_int32 upperclip = ll_check_l_int32(_fun, L, 3);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixDitherTo2bppSpec(pixs, lowerclip, upperclip, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DitherToBinary(lua_State *L)
{
    LL_FUNC("DitherToBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixDitherToBinary(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (lowerclip).
 * Arg #3 is expected to be a l_int32 (upperclip).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DitherToBinarySpec(lua_State *L)
{
    LL_FUNC("DitherToBinarySpec");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 lowerclip = ll_check_l_int32(_fun, L, 2);
    l_int32 upperclip = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixDitherToBinarySpec(pixs, lowerclip, upperclip);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DrawBoxa(lua_State *L)
{
    LL_FUNC("DrawBoxa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixDrawBoxa(pixs, boxa, width, val);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (width).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DrawBoxaRandom(lua_State *L)
{
    LL_FUNC("DrawBoxaRandom");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixDrawBoxaRandom(pixs, boxa, width);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (height).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
EmbedForRotation(lua_State *L)
{
    LL_FUNC("EmbedForRotation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 height = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixEmbedForRotation(pixs, angle, incolor, width, height);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
EndianByteSwap(lua_State *L)
{
    LL_FUNC("EndianByteSwap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixEndianByteSwap(pixs));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
EndianByteSwapNew(lua_State *L)
{
    LL_FUNC("EndianByteSwapNew");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixEndianByteSwapNew(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
EndianTwoByteSwap(lua_State *L)
{
    LL_FUNC("EndianTwoByteSwap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixEndianTwoByteSwap(pixs));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
EndianTwoByteSwapNew(lua_State *L)
{
    LL_FUNC("EndianTwoByteSwapNew");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixEndianTwoByteSwapNew(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Equal(lua_State *L)
{
    LL_FUNC("Equal");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 same = 0;
    if (pixEqual(pix1, pix2, &same))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (use_alpha).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
EqualWithAlpha(lua_State *L)
{
    LL_FUNC("EqualWithAlpha");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 use_alpha = ll_check_l_int32(_fun, L, 3);
    l_int32 same = 0;
    if (pixEqualWithAlpha(pix1, pix2, use_alpha, &same))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
EqualWithCmap(lua_State *L)
{
    LL_FUNC("EqualWithCmap");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 same = 0;
    if (pixEqualWithCmap(pix1, pix2, &same))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, same);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
EqualizeTRC(lua_State *L)
{
    LL_FUNC("EqualizeTRC");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 factor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixEqualizeTRC(pixd, pixs, fract, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Erode(lua_State *L)
{
    LL_FUNC("Erode");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixErode(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeBrick(lua_State *L)
{
    LL_FUNC("ErodeBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixErodeBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeBrickDwa(lua_State *L)
{
    LL_FUNC("ErodeBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixErodeBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeCompBrick(lua_State *L)
{
    LL_FUNC("ErodeCompBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixErodeCompBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeCompBrickDwa(lua_State *L)
{
    LL_FUNC("ErodeCompBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixErodeCompBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeCompBrickExtendDwa(lua_State *L)
{
    LL_FUNC("ErodeCompBrickExtendDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixErodeCompBrickExtendDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeGray(lua_State *L)
{
    LL_FUNC("ErodeGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixErodeGray(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ErodeGray3(lua_State *L)
{
    LL_FUNC("ErodeGray3");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixErodeGray3(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (darkthresh).
 * Arg #3 is expected to be a l_float32 (edgecrop).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
EstimateBackground(lua_State *L)
{
    LL_FUNC("EstimateBackground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 2);
    l_float32 edgecrop = ll_check_l_float32(_fun, L, 3);
    l_int32 bg = 0;
    if (pixEstimateBackground(pixs, darkthresh, edgecrop, &bg))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, bg);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExpandBinaryPower2(lua_State *L)
{
    LL_FUNC("ExpandBinaryPower2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixExpandBinaryPower2(pixs, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xfact).
 * Arg #3 is expected to be a l_int32 (yfact).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExpandBinaryReplicate(lua_State *L)
{
    LL_FUNC("ExpandBinaryReplicate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xfact = ll_check_l_int32(_fun, L, 2);
    l_int32 yfact = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixExpandBinaryReplicate(pixs, xfact, yfact);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExpandReplicate(lua_State *L)
{
    LL_FUNC("ExpandReplicate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixExpandReplicate(pixs, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (addw).
 * Arg #3 is expected to be a l_int32 (addh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExtendByReplication(lua_State *L)
{
    LL_FUNC("ExtendByReplication");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 addw = ll_check_l_int32(_fun, L, 2);
    l_int32 addh = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixExtendByReplication(pixs, addw, addh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ExtractBarcodeCrossings(lua_State *L)
{
    LL_FUNC("ExtractBarcodeCrossings");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 3);
    Numa *result = pixExtractBarcodeCrossings(pixs, thresh, debugflag);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_float32 (binfract).
 * Arg #6 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ExtractBarcodeWidths1(lua_State *L)
{
    LL_FUNC("ExtractBarcodeWidths1");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_float32 binfract = ll_check_l_float32(_fun, L, 3);
    Numa *naehist = nullptr;
    Numa *naohist = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 6);
    if (pixExtractBarcodeWidths1(pixs, thresh, binfract, &naehist, &naohist, debugflag))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, naehist);
    ll_push_Numa(_fun, L, naohist);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ExtractBarcodeWidths2(lua_State *L)
{
    LL_FUNC("ExtractBarcodeWidths2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_float32 width = 0;
    Numa *nac = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    if (pixExtractBarcodeWidths2(pixs, thresh, &width, &nac, debugflag))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, width);
    ll_push_Numa(_fun, L, nac);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa * on the Lua stack
 */
static int
ExtractBarcodes(lua_State *L)
{
    LL_FUNC("ExtractBarcodes");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 2);
    Pixa *result = pixExtractBarcodes(pixs, debugflag);
    return ll_push_Pixa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExtractBorderConnComps(lua_State *L)
{
    LL_FUNC("ExtractBorderConnComps");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixExtractBorderConnComps(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ExtractBoundary(lua_State *L)
{
    LL_FUNC("ExtractBoundary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixExtractBoundary(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_uint32 * on the Lua stack
 */
static int
ExtractData(lua_State *L)
{
    LL_FUNC("ExtractData");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 *data = pixExtractData(pixs);
    l_int32 wpl = pixGetWpl(pixs);
    l_int32 h = pixGetHeight(pixs);
    return ll_push_Uarray_2d(_fun, L, data, wpl, h);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ExtractOnLine(lua_State *L)
{
    LL_FUNC("ExtractOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    Numa *na = pixExtractOnLine(pixs, x1, y1, x2, y2, factor);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxw).
 * Arg #3 is expected to be a l_int32 (maxh).
 * Arg #4 is expected to be a l_int32 (adjw).
 * Arg #5 is expected to be a l_int32 (adjh).
 * Arg #6 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa * on the Lua stack
 */
static int
ExtractRawTextlines(lua_State *L)
{
    LL_FUNC("ExtractRawTextlines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxw = ll_check_l_int32(_fun, L, 2);
    l_int32 maxh = ll_check_l_int32(_fun, L, 3);
    l_int32 adjw = ll_check_l_int32(_fun, L, 4);
    l_int32 adjh = ll_check_l_int32(_fun, L, 5);
    Pixa *pixadb = ll_check_Pixa(_fun, L, 6);
    Pixa *result = pixExtractRawTextlines(pixs, maxw, maxh, adjw, adjh, pixadb);
    return ll_push_Pixa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxw).
 * Arg #3 is expected to be a l_int32 (maxh).
 * Arg #4 is expected to be a l_int32 (minw).
 * Arg #5 is expected to be a l_int32 (minh).
 * Arg #6 is expected to be a l_int32 (adjw).
 * Arg #7 is expected to be a l_int32 (adjh).
 * Arg #8 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa * on the Lua stack
 */
static int
ExtractTextlines(lua_State *L)
{
    LL_FUNC("ExtractTextlines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxw = ll_check_l_int32(_fun, L, 2);
    l_int32 maxh = ll_check_l_int32(_fun, L, 3);
    l_int32 minw = ll_check_l_int32(_fun, L, 4);
    l_int32 minh = ll_check_l_int32(_fun, L, 5);
    l_int32 adjw = ll_check_l_int32(_fun, L, 6);
    l_int32 adjh = ll_check_l_int32(_fun, L, 7);
    Pixa *pixadb = ll_check_Pixa(_fun, L, 8);
    Pixa *result = pixExtractTextlines(pixs, maxw, maxh, minw, minh, adjw, adjh, pixadb);
    return ll_push_Pixa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixb).
 * Arg #3 is expected to be a l_float32 (factor).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FadeWithGray(lua_State *L)
{
    LL_FUNC("FadeWithGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixb = ll_check_Pix(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixFadeWithGray(pixs, pixb, factor, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xsize).
 * Arg #3 is expected to be a l_int32 (ysize).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FastTophat(lua_State *L)
{
    LL_FUNC("FastTophat");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xsize = ll_check_l_int32(_fun, L, 2);
    l_int32 ysize = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixFastTophat(pixs, xsize, ysize, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ncolor).
 * Arg #3 is expected to be a l_int32 (ngray).
 * Arg #4 is expected to be a l_int32 (maxncolors).
 * Arg #5 is expected to be a l_int32 (darkthresh).
 * Arg #6 is expected to be a l_int32 (lightthresh).
 * Arg #7 is expected to be a l_int32 (diffthresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FewColorsMedianCutQuantMixed(lua_State *L)
{
    LL_FUNC("FewColorsMedianCutQuantMixed");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ncolor = ll_check_l_int32(_fun, L, 2);
    l_int32 ngray = ll_check_l_int32(_fun, L, 3);
    l_int32 maxncolors = ll_check_l_int32(_fun, L, 4);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 5);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 6);
    l_int32 diffthresh = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixFewColorsMedianCutQuantMixed(pixs, ncolor, ngray, maxncolors, darkthresh, lightthresh, diffthresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FewColorsOctcubeQuant1(lua_State *L)
{
    LL_FUNC("FewColorsOctcubeQuant1");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixFewColorsOctcubeQuant1(pixs, level);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * Arg #3 is expected to be a Numa* (na).
 * Arg #4 is expected to be a l_int32 (ncolors).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FewColorsOctcubeQuant2(lua_State *L)
{
    LL_FUNC("FewColorsOctcubeQuant2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    Numa *na = ll_check_Numa(_fun, L, 3);
    l_int32 ncolors = ll_check_l_int32(_fun, L, 4);
    l_int32 nerrors = 0;
    if (pixFewColorsOctcubeQuant2(pixs, level, na, ncolors, &nerrors))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nerrors);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * Arg #3 is expected to be a l_int32 (darkthresh).
 * Arg #4 is expected to be a l_int32 (lightthresh).
 * Arg #5 is expected to be a l_int32 (diffthresh).
 * Arg #6 is expected to be a l_float32 (minfract).
 * Arg #7 is expected to be a l_int32 (maxspan).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FewColorsOctcubeQuantMixed(lua_State *L)
{
    LL_FUNC("FewColorsOctcubeQuantMixed");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 3);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 diffthresh = ll_check_l_int32(_fun, L, 5);
    l_float32 minfract = ll_check_l_float32(_fun, L, 6);
    l_int32 maxspan = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixFewColorsOctcubeQuantMixed(pixs, level, darkthresh, lightthresh, diffthresh, minfract, maxspan);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FillBgFromBorder(lua_State *L)
{
    LL_FUNC("FillBgFromBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixFillBgFromBorder(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FillClosedBorders(lua_State *L)
{
    LL_FUNC("FillClosedBorders");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixFillClosedBorders(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minsize).
 * Arg #3 is expected to be a l_float32 (maxhfract).
 * Arg #4 is expected to be a l_float32 (minfgfract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FillHolesToBoundingRect(lua_State *L)
{
    LL_FUNC("FillHolesToBoundingRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minsize = ll_check_l_int32(_fun, L, 2);
    l_float32 maxhfract = ll_check_l_float32(_fun, L, 3);
    l_float32 minfgfract = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixFillHolesToBoundingRect(pixs, minsize, maxhfract, minfgfract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (nx).
 * Arg #3 is expected to be a l_int32 (ny).
 * Arg #4 is expected to be a l_int32 (filltype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FillMapHoles(lua_State *L)
{
    LL_FUNC("FillMapHoles");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 nx = ll_check_l_int32(_fun, L, 2);
    l_int32 ny = ll_check_l_int32(_fun, L, 3);
    l_int32 filltype = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixFillMapHoles(pix, nx, ny, filltype));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (pta).
 * Arg #3 is expected to be a l_int32 (xmin).
 * Arg #4 is expected to be a l_int32 (ymin).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FillPolygon(lua_State *L)
{
    LL_FUNC("FillPolygon");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_int32 xmin = ll_check_l_int32(_fun, L, 3);
    l_int32 ymin = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixFillPolygon(pixs, pta, xmin, ymin);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (offset).
 * Arg #3 is expected to be a l_int32 (depth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FinalAccumulate(lua_State *L)
{
    LL_FUNC("FinalAccumulate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 offset = ll_check_l_uint32(_fun, L, 2);
    l_int32 depth = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixFinalAccumulate(pixs, offset, depth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (offset).
 * Arg #3 is expected to be a l_uint32 (threshold).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FinalAccumulateThreshold(lua_State *L)
{
    LL_FUNC("FinalAccumulateThreshold");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 offset = ll_check_l_uint32(_fun, L, 2);
    l_uint32 threshold = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixFinalAccumulateThreshold(pixs, offset, threshold);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindAreaFraction(lua_State *L)
{
    LL_FUNC("FindAreaFraction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 tab = 0;
    l_float32 fract = 0;
    if (pixFindAreaFraction(pixs, &tab, &fract))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, fract);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a Pix* (pixm).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindAreaFractionMasked(lua_State *L)
{
    LL_FUNC("FindAreaFractionMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 tab = 0;
    l_float32 fract = 0;
    if (pixFindAreaFractionMasked(pixs, box, pixm, &tab, &fract))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, fract);
    return 2;
}

/**
 * \brief Find the area to perimeter ratio in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
FindAreaPerimRatio(lua_State *L)
{
    LL_FUNC("FindAreaPerimRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0.0f;
    if (pixFindAreaPerimRatio(pixs, tab8, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
FindBaselines(lua_State *L)
{
    LL_FUNC("FindBaselines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *pta = nullptr;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 3);
    if (pixFindBaselines(pixs, &pta, pixadb))
        return ll_push_nil(L);
    ll_push_Pta(_fun, L, pta);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_int32 (lightthresh).
 * Arg #5 is expected to be a l_int32 (darkthresh).
 * Arg #6 is expected to be a l_int32 (mindiff).
 * Arg #7 is expected to be a l_int32 (colordiff).
 * Arg #8 is expected to be a l_float32 (edgefract).
 * Arg #12 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindColorRegions(lua_State *L)
{
    LL_FUNC("FindColorRegions");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 5);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 6);
    l_int32 colordiff = ll_check_l_int32(_fun, L, 7);
    l_float32 edgefract = ll_check_l_float32(_fun, L, 8);
    l_float32 colorfract = 0;
    Pix *colormask1 = nullptr;
    Pix *colormask2 = nullptr;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 12);
    if (pixFindColorRegions(pixs, pixm, factor,
                            lightthresh, darkthresh, mindiff, colordiff, edgefract,
                            &colorfract, &colormask1, &colormask2, pixadb))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, colorfract);
    ll_push_Pix(_fun, L, colormask1);
    ll_push_Pix(_fun, L, colormask2);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
FindCornerPixels(lua_State *L)
{
    LL_FUNC("FindCornerPixels");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *result = pixFindCornerPixels(pixs);
    return ll_push_Pta(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindDifferentialSquareSum(lua_State *L)
{
    LL_FUNC("FindDifferentialSquareSum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 sum = 0;
    if (pixFindDifferentialSquareSum(pixs, &sum))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, sum);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FindEqualValues(lua_State *L)
{
    LL_FUNC("FindEqualValues");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixFindEqualValues(pixs1, pixs2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_int32 (height).
 * Arg #5 is expected to be a l_int32 (npeaks).
 * Arg #6 is expected to be a l_float32 (erasefactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindHistoPeaksHSV(lua_State *L)
{
    LL_FUNC("FindHistoPeaksHSV");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_int32 height = ll_check_l_int32(_fun, L, 4);
    l_int32 npeaks = ll_check_l_int32(_fun, L, 5);
    l_float32 erasefactor = ll_check_l_float32(_fun, L, 6);
    Pta *pta = nullptr;
    Numa *natot = nullptr;
    Pixa *pixa = nullptr;
    if (pixFindHistoPeaksHSV(pixs, type, width, height, npeaks, erasefactor, &pta, &natot, &pixa))
        return ll_push_nil(L);
    ll_push_Pta(_fun, L, pta);
    ll_push_Numa(_fun, L, natot);
    ll_push_Pixa(_fun, L, pixa);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindHorizontalRuns(lua_State *L)
{
    LL_FUNC("FindHorizontalRuns");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 y = ll_check_l_int32(_fun, L, 2);
    l_int32 xstart = 0;
    l_int32 xend = 0;
    l_int32 n = 0;
    if (pixFindHorizontalRuns(pix, y, &xstart, &xend, &n))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xstart);
    ll_push_l_int32(_fun, L, xend);
    ll_push_l_int32(_fun, L, n);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (polarity).
 * Arg #3 is expected to be a l_int32 (nrect).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindLargeRectangles(lua_State *L)
{
    LL_FUNC("FindLargeRectangles");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 polarity = ll_check_l_int32(_fun, L, 2);
    l_int32 nrect = ll_check_l_int32(_fun, L, 3);
    Boxa *boxa = nullptr;
    Pix *pixdb = nullptr;
    if (pixFindLargeRectangles(pixs, polarity, nrect, &boxa, &pixdb))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    ll_push_Pix(_fun, L, pixdb);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (polarity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindLargestRectangle(lua_State *L)
{
    LL_FUNC("FindLargestRectangle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 polarity = ll_check_l_int32(_fun, L, 2);
    Box *box = nullptr;
    Pix *pixdb = nullptr;
    if (pixFindLargestRectangle(pixs, polarity, &box, &pixdb))
        return ll_push_nil(L);
    ll_push_Box(_fun, L, box);
    ll_push_Pix(_fun, L, pixdb);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindMaxHorizontalRunOnLine(lua_State *L)
{
    LL_FUNC("FindMaxHorizontalRunOnLine");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 y = ll_check_l_int32(_fun, L, 2);
    l_int32 xstart = 0;
    l_int32 size = 0;
    if (pixFindMaxHorizontalRunOnLine(pix, y, &xstart, &size))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xstart);
    ll_push_l_int32(_fun, L, size);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
FindMaxRuns(lua_State *L)
{
    LL_FUNC("FindMaxRuns");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 direction = ll_check_l_int32(_fun, L, 2);
    Numa *nastart = nullptr;
    if (pixFindMaxRuns(pix, direction, &nastart))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, nastart);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (x).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindMaxVerticalRunOnLine(lua_State *L)
{
    LL_FUNC("FindMaxVerticalRunOnLine");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 ystart = 0;
    l_int32 size = 0;
    if (pixFindMaxVerticalRunOnLine(pix, x, &ystart, &size))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ystart);
    ll_push_l_int32(_fun, L, size);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindNormalizedSquareSum(lua_State *L)
{
    LL_FUNC("FindNormalizedSquareSum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 hratio = 0;
    l_float32 vratio = 0;
    l_float32 fract = 0;
    if (pixFindNormalizedSquareSum(pixs, &hratio, &vratio, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, hratio);
    ll_push_l_float32(_fun, L, vratio);
    ll_push_l_float32(_fun, L, fract);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a l_int32 (x2).
 * Arg #4 is expected to be a l_int32 (y2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindOverlapFraction(lua_State *L)
{
    LL_FUNC("FindOverlapFraction");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    l_int32 x2 = ll_check_l_int32(_fun, L, 3);
    l_int32 y2 = ll_check_l_int32(_fun, L, 4);
    l_int32 tab = 0;
    l_float32 ratio = 0;
    l_int32 noverlap = 0;
    if (pixFindOverlapFraction(pixs1, pixs2, x2, y2, &tab, &ratio, &noverlap))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, ratio);
    ll_push_l_int32(_fun, L, noverlap);
    return 3;
}

/**
 * \brief Find the page foreground in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (threshold).
 * Arg #3 is expected to be a l_int32 (mindist).
 * Arg #4 is expected to be a l_int32 (erasedist).
 * Arg #5 is expected to be a boolean (showmorph).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
FindPageForeground(lua_State *L)
{
    LL_FUNC("FindPageForeground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 threshold = ll_check_l_int32_default(_fun, L, 2, 128);
    l_int32 mindist = ll_check_l_int32_default(_fun, L, 3, 50);
    l_int32 erasedist = ll_check_l_int32_default(_fun, L, 4, 30);
    l_int32 showmorph = ll_check_boolean_default(_fun, L, 5, FALSE);
    Box *box = pixFindPageForeground(pixs, threshold, mindist, erasedist, showmorph, nullptr);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindPerimSizeRatio(lua_State *L)
{
    LL_FUNC("FindPerimSizeRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 tab = 0;
    l_float32 ratio = 0;
    if (pixFindPerimSizeRatio(pixs, &tab, &ratio))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab);
    ll_push_l_float32(_fun, L, ratio);
    return 2;
}

/**
 * \brief Find the perimeter to area ratio in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
FindPerimToAreaRatio(lua_State *L)
{
    LL_FUNC("FindPerimToAreaRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0.0f;
    if (pixFindPerimToAreaRatio(pixs, tab8, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dist).
 * Arg #3 is expected to be a l_int32 (minw).
 * Arg #4 is expected to be a l_int32 (minh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
FindRectangleComps(lua_State *L)
{
    LL_FUNC("FindRectangleComps");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dist = ll_check_l_int32(_fun, L, 2);
    l_int32 minw = ll_check_l_int32(_fun, L, 3);
    l_int32 minh = ll_check_l_int32(_fun, L, 4);
    Boxa *result = pixFindRectangleComps(pixs, dist, minw, minh);
    return ll_push_Boxa(_fun, L, result);
}

/**
 * \brief Look for one or two square tiles with conforming median.
 *        intensity and low variance outside but near the input %box.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a string describing the search direction (searchdir).
 * Arg #4 is expected to be a l_int32 (mindist).
 * Arg #5 is expected to be a l_int32 (tsize).
 * Arg #6 is expected to be a l_int32 (ntiles).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
FindRepCloseTile(lua_State *L)
{
    LL_FUNC("FindRepCloseTile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 searchdir = ll_check_direction(_fun, L, 3, L_HORIZ);
    l_int32 mindist = ll_check_l_int32(_fun, L, 4);
    l_int32 tsize = ll_check_l_int32(_fun, L, 5);
    l_int32 ntiles = ll_check_l_int32(_fun, L, 6);
    Box *boxtile = nullptr;
    if (pixFindRepCloseTile(pixs, box, searchdir, mindist, tsize, ntiles, &boxtile, 0))
        return ll_push_nil(L);
    return ll_push_Box(_fun, L, boxtile);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkew(lua_State *L)
{
    LL_FUNC("FindSkew");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_float32 conf = 0;
    if (pixFindSkew(pixs, &angle, &conf))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (redsearch).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FindSkewAndDeskew(lua_State *L)
{
    LL_FUNC("FindSkewAndDeskew");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 2);
    l_float32 angle = 0;
    l_float32 conf = 0;
    Pix *pix = pixFindSkewAndDeskew(pixs, redsearch, &angle, &conf);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    ll_push_Pix(_fun, L, pix);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a l_int32 (redsweep).
 * Arg #5 is expected to be a l_int32 (redsearch).
 * Arg #6 is expected to be a l_float32 (sweeprange).
 * Arg #7 is expected to be a l_float32 (sweepdelta).
 * Arg #8 is expected to be a l_float32 (minbsdelta).
 * Arg #9 is expected to be a l_float32 (confprior).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkewOrthogonalRange(lua_State *L)
{
    LL_FUNC("FindSkewOrthogonalRange");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_float32 conf = 0;
    l_int32 redsweep = ll_check_l_int32(_fun, L, 4);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 5);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 6);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 7);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 8);
    l_float32 confprior = ll_check_l_float32(_fun, L, 9);
    if (pixFindSkewOrthogonalRange(pixs, &angle, &conf, redsweep, redsearch, sweeprange, sweepdelta, minbsdelta, confprior))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_float32 (sweeprange).
 * Arg #5 is expected to be a l_float32 (sweepdelta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkewSweep(lua_State *L)
{
    LL_FUNC("FindSkewSweep");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 4);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 5);
    if (pixFindSkewSweep(pixs, &angle, reduction, sweeprange, sweepdelta))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a l_int32 (redsweep).
 * Arg #5 is expected to be a l_int32 (redsearch).
 * Arg #6 is expected to be a l_float32 (sweeprange).
 * Arg #7 is expected to be a l_float32 (sweepdelta).
 * Arg #8 is expected to be a l_float32 (minbsdelta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkewSweepAndSearch(lua_State *L)
{
    LL_FUNC("FindSkewSweepAndSearch");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_float32 conf = 0;
    l_int32 redsweep = ll_check_l_int32(_fun, L, 4);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 5);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 6);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 7);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 8);
    if (pixFindSkewSweepAndSearch(pixs, &angle, &conf, redsweep, redsearch, sweeprange, sweepdelta, minbsdelta))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #5 is expected to be a l_int32 (redsweep).
 * Arg #6 is expected to be a l_int32 (redsearch).
 * Arg #7 is expected to be a l_float32 (sweepcenter).
 * Arg #8 is expected to be a l_float32 (sweeprange).
 * Arg #9 is expected to be a l_float32 (sweepdelta).
 * Arg #10 is expected to be a l_float32 (minbsdelta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkewSweepAndSearchScore(lua_State *L)
{
    LL_FUNC("FindSkewSweepAndSearchScore");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_float32 conf = 0;
    l_float32 endscore = 0;
    l_int32 redsweep = ll_check_l_int32(_fun, L, 5);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 6);
    l_float32 sweepcenter = ll_check_l_float32(_fun, L, 7);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 8);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 9);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 10);
    if (pixFindSkewSweepAndSearchScore(pixs, &angle, &conf, &endscore, redsweep, redsearch, sweepcenter, sweeprange, sweepdelta, minbsdelta))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    ll_push_l_float32(_fun, L, endscore);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #5 is expected to be a l_int32 (redsweep).
 * Arg #6 is expected to be a l_int32 (redsearch).
 * Arg #7 is expected to be a l_float32 (sweepcenter).
 * Arg #8 is expected to be a l_float32 (sweeprange).
 * Arg #9 is expected to be a l_float32 (sweepdelta).
 * Arg #10 is expected to be a l_float32 (minbsdelta).
 * Arg #11 is expected to be a l_int32 (pivot).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindSkewSweepAndSearchScorePivot(lua_State *L)
{
    LL_FUNC("FindSkewSweepAndSearchScorePivot");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = 0;
    l_float32 conf = 0;
    l_float32 endscore = 0;
    l_int32 redsweep = ll_check_l_int32(_fun, L, 5);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 6);
    l_float32 sweepcenter = ll_check_l_float32(_fun, L, 7);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 8);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 9);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 10);
    l_int32 pivot = ll_check_l_int32(_fun, L, 11);
    if (pixFindSkewSweepAndSearchScorePivot(pixs, &angle, &conf, &endscore, redsweep, redsearch, sweepcenter, sweeprange, sweepdelta, minbsdelta, pivot))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, angle);
    ll_push_l_float32(_fun, L, conf);
    ll_push_l_float32(_fun, L, endscore);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindStrokeLength(lua_State *L)
{
    LL_FUNC("FindStrokeLength");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 tab8 = 0;
    l_int32 length = 0;
    if (pixFindStrokeLength(pixs, &tab8, &length))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab8);
    ll_push_l_int32(_fun, L, length);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindStrokeWidth(lua_State *L)
{
    LL_FUNC("FindStrokeWidth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 tab8 = 0;
    l_float32 width = 0;
    Numa *nahisto = nullptr;
    if (pixFindStrokeWidth(pixs, thresh, &tab8, &width, &nahisto))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab8);
    ll_push_l_float32(_fun, L, width);
    ll_push_Numa(_fun, L, nahisto);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindThreshFgExtent(lua_State *L)
{
    LL_FUNC("FindThreshFgExtent");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    l_int32 top = 0;
    l_int32 bot = 0;
    if (pixFindThreshFgExtent(pixs, thresh, &top, &bot))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, top);
    ll_push_l_int32(_fun, L, bot);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (x).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FindVerticalRuns(lua_State *L)
{
    LL_FUNC("FindVerticalRuns");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 ystart = 0;
    l_int32 yend = 0;
    l_int32 n = 0;
    if (pixFindVerticalRuns(pix, x, &ystart, &yend, &n))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ystart);
    ll_push_l_int32(_fun, L, yend);
    ll_push_l_int32(_fun, L, n);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FixedOctcubeQuant256(lua_State *L)
{
    LL_FUNC("FixedOctcubeQuant256");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixFixedOctcubeQuant256(pixs, ditherflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FixedOctcubeQuantGenRGB(lua_State *L)
{
    LL_FUNC("FixedOctcubeQuantGenRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixFixedOctcubeQuantGenRGB(pixs, level);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FlipFHMTGen(lua_State *L)
{
    LL_FUNC("FlipFHMTGen");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    const char *name = ll_check_string(_fun, L, 3);
    /* XXX: deconstify */
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Pix *pix = pixFlipFHMTGen(pixd, pixs, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FlipLR(lua_State *L)
{
    LL_FUNC("FlipLR");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixFlipLR(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Flip a pixel value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlipPixel(lua_State *L)
{
    LL_FUNC("FlipPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixFlipPixel(pix, x, y));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FlipTB(lua_State *L)
{
    LL_FUNC("FlipTB");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixFlipTB(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Calculate the fraction of foreground in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
ForegroundFraction(lua_State *L)
{
    LL_FUNC("ForegroundFraction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0;
    if (pixForegroundFraction(pixs, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
FractionFgInMask(lua_State *L)
{
    LL_FUNC("FractionFgInMask");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_float32 fract = 0;
    if (pixFractionFgInMask(pix1, pix2, &fract))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FreeData(lua_State *L)
{
    LL_FUNC("FreeData");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixFreeData(pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (gamma).
 * Arg #4 is expected to be a l_int32 (minval).
 * Arg #5 is expected to be a l_int32 (maxval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GammaTRC(lua_State *L)
{
    LL_FUNC("GammaTRC");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    l_int32 minval = ll_check_l_int32(_fun, L, 4);
    l_int32 maxval = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixGammaTRC(pixd, pixs, gamma, minval, maxval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_float32 (gamma).
 * Arg #5 is expected to be a l_int32 (minval).
 * Arg #6 is expected to be a l_int32 (maxval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GammaTRCMasked(lua_State *L)
{
    LL_FUNC("GammaTRCMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_float32 gamma = ll_check_l_float32(_fun, L, 4);
    l_int32 minval = ll_check_l_int32(_fun, L, 5);
    l_int32 maxval = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixGammaTRCMasked(pixd, pixs, pixm, gamma, minval, maxval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (gamma).
 * Arg #4 is expected to be a l_int32 (minval).
 * Arg #5 is expected to be a l_int32 (maxval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GammaTRCWithAlpha(lua_State *L)
{
    LL_FUNC("GammaTRCWithAlpha");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    l_int32 minval = ll_check_l_int32(_fun, L, 4);
    l_int32 maxval = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixGammaTRCWithAlpha(pixd, pixs, gamma, minval, maxval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (%pixtext) and l_int32 (%htfound) on the Lua stack
 */
static int
GenHalftoneMask(lua_State *L)
{
    LL_FUNC("GenHalftoneMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 debug = ll_check_boolean_default(_fun, L, 2, FALSE);
    Pix *pixtext = nullptr;
    l_int32 htfound = 0;
    if (pixGenHalftoneMask(pixs, &pixtext, &htfound, debug))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixtext);
    ll_push_l_int32(_fun, L, htfound);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_float32 (thresh).
 * Arg #5 is expected to be a l_int32 (nx).
 * Arg #6 is expected to be a l_int32 (ny).
 * Arg #7 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GenPhotoHistos(lua_State *L)
{
    LL_FUNC("GenPhotoHistos");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_float32 thresh = ll_check_l_float32(_fun, L, 4);
    l_int32 nx = ll_check_l_int32(_fun, L, 5);
    l_int32 ny = ll_check_l_int32(_fun, L, 6);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 7);
    Numaa *naa = nullptr;
    l_int32 w = 0;
    l_int32 h = 0;
    if (pixGenPhotoHistos(pixs, box, factor, thresh, nx, ny, &naa, &w, &h, debugflag))
        return ll_push_nil(L);
    ll_push_Numaa(_fun, L, naa);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixvws).
 * Arg #3 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenTextblockMask(lua_State *L)
{
    LL_FUNC("GenTextblockMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixvws = ll_check_Pix(_fun, L, 2);
    Pixa *pixadb = ll_check_Pixa(_fun, L, 3);
    Pix *pix = pixGenTextblockMask(pixs, pixvws, pixadb);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenTextlineMask(lua_State *L)
{
    LL_FUNC("GenTextlineMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixvws = nullptr;
    l_int32 tlfound = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 4);
    if (pixGenTextlineMask(pixs, &pixvws, &tlfound, pixadb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixvws);
    ll_push_l_int32(_fun, L, tlfound);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GenerateCIData(lua_State *L)
{
    LL_FUNC("GenerateCIData");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 ascii85 = ll_check_l_int32(_fun, L, 4);
    CompData *cid = nullptr;
    if (pixGenerateCIData(pixs, type, quality, ascii85, &cid))
        return ll_push_nil(L);
    ll_push_CompData(_fun, L, cid);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* (pta).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateFromPta(lua_State *L)
{
    LL_FUNC("GenerateFromPta");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixGenerateFromPta(pta, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateHalftoneMask(lua_State *L)
{
    LL_FUNC("GenerateHalftoneMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixtext = nullptr;
    l_int32 htfound = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 4);
    if (pixGenerateHalftoneMask(pixs, &pixtext, &htfound, pixadb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixtext);
    ll_push_l_int32(_fun, L, htfound);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (lower).
 * Arg #3 is expected to be a l_int32 (upper).
 * Arg #4 is expected to be a l_int32 (inband).
 * Arg #5 is expected to be a l_int32 (usecmap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateMaskByBand(lua_State *L)
{
    LL_FUNC("GenerateMaskByBand");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 lower = ll_check_l_int32(_fun, L, 2);
    l_int32 upper = ll_check_l_int32(_fun, L, 3);
    l_int32 inband = ll_check_l_int32(_fun, L, 4);
    l_int32 usecmap = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixGenerateMaskByBand(pixs, lower, upper, inband, usecmap);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (refval).
 * Arg #3 is expected to be a l_int32 (delm).
 * Arg #4 is expected to be a l_int32 (delp).
 * Arg #5 is expected to be a l_float32 (fractm).
 * Arg #6 is expected to be a l_float32 (fractp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateMaskByBand32(lua_State *L)
{
    LL_FUNC("GenerateMaskByBand32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 refval = ll_check_l_uint32(_fun, L, 2);
    l_int32 delm = ll_check_l_int32(_fun, L, 3);
    l_int32 delp = ll_check_l_int32(_fun, L, 4);
    l_float32 fractm = ll_check_l_float32(_fun, L, 5);
    l_float32 fractp = ll_check_l_float32(_fun, L, 6);
    Pix *pix = pixGenerateMaskByBand32(pixs, refval, delm, delp, fractm, fractp);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (refval1).
 * Arg #3 is expected to be a l_uint32 (refval2).
 * Arg #4 is expected to be a l_int32 (distflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateMaskByDiscr32(lua_State *L)
{
    LL_FUNC("GenerateMaskByDiscr32");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 refval1 = ll_check_l_uint32(_fun, L, 2);
    l_uint32 refval2 = ll_check_l_uint32(_fun, L, 3);
    l_int32 distflag = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixGenerateMaskByDiscr32(pixs, refval1, refval2, distflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (val).
 * Arg #3 is expected to be a l_int32 (usecmap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GenerateMaskByValue(lua_State *L)
{
    LL_FUNC("GenerateMaskByValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    l_int32 usecmap = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixGenerateMaskByValue(pixs, val, usecmap);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (width).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
GeneratePtaBoundary(lua_State *L)
{
    LL_FUNC("GeneratePtaBoundary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    Pta *result = pixGeneratePtaBoundary(pixs, width);
    return ll_push_Pta(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hitdist).
 * Arg #3 is expected to be a l_int32 (missdist).
 * Arg #4 is expected to be a l_int32 (hitskip).
 * Arg #5 is expected to be a l_int32 (missskip).
 * Arg #6 is expected to be a l_int32 (topflag).
 * Arg #7 is expected to be a l_int32 (botflag).
 * Arg #8 is expected to be a l_int32 (leftflag).
 * Arg #9 is expected to be a l_int32 (rightflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
GenerateSelBoundary(lua_State *L)
{
    LL_FUNC("GenerateSelBoundary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hitdist = ll_check_l_int32(_fun, L, 2);
    l_int32 missdist = ll_check_l_int32(_fun, L, 3);
    l_int32 hitskip = ll_check_l_int32(_fun, L, 4);
    l_int32 missskip = ll_check_l_int32(_fun, L, 5);
    l_int32 topflag = ll_check_l_int32(_fun, L, 6);
    l_int32 botflag = ll_check_l_int32(_fun, L, 7);
    l_int32 leftflag = ll_check_l_int32(_fun, L, 8);
    l_int32 rightflag = ll_check_l_int32(_fun, L, 9);
    Pix *pixe = nullptr;
    if (pixGenerateSelBoundary(pixs, hitdist, missdist, hitskip, missskip, topflag, botflag, leftflag, rightflag, &pixe))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixe);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (hitfract).
 * Arg #3 is expected to be a l_float32 (missfract).
 * Arg #4 is expected to be a l_int32 (distance).
 * Arg #5 is expected to be a l_int32 (toppix).
 * Arg #6 is expected to be a l_int32 (botpix).
 * Arg #7 is expected to be a l_int32 (leftpix).
 * Arg #8 is expected to be a l_int32 (rightpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
GenerateSelRandom(lua_State *L)
{
    LL_FUNC("GenerateSelRandom");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 hitfract = ll_check_l_float32(_fun, L, 2);
    l_float32 missfract = ll_check_l_float32(_fun, L, 3);
    l_int32 distance = ll_check_l_int32(_fun, L, 4);
    l_int32 toppix = ll_check_l_int32(_fun, L, 5);
    l_int32 botpix = ll_check_l_int32(_fun, L, 6);
    l_int32 leftpix = ll_check_l_int32(_fun, L, 7);
    l_int32 rightpix = ll_check_l_int32(_fun, L, 8);
    Pix *pixe = nullptr;
    if (pixGenerateSelRandom(pixs, hitfract, missfract, distance, toppix, botpix, leftpix, rightpix, &pixe))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixe);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nhlines).
 * Arg #3 is expected to be a l_int32 (nvlines).
 * Arg #4 is expected to be a l_int32 (distance).
 * Arg #5 is expected to be a l_int32 (minlength).
 * Arg #6 is expected to be a l_int32 (toppix).
 * Arg #7 is expected to be a l_int32 (botpix).
 * Arg #8 is expected to be a l_int32 (leftpix).
 * Arg #9 is expected to be a l_int32 (rightpix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
GenerateSelWithRuns(lua_State *L)
{
    LL_FUNC("GenerateSelWithRuns");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nhlines = ll_check_l_int32(_fun, L, 2);
    l_int32 nvlines = ll_check_l_int32(_fun, L, 3);
    l_int32 distance = ll_check_l_int32(_fun, L, 4);
    l_int32 minlength = ll_check_l_int32(_fun, L, 5);
    l_int32 toppix = ll_check_l_int32(_fun, L, 6);
    l_int32 botpix = ll_check_l_int32(_fun, L, 7);
    l_int32 leftpix = ll_check_l_int32(_fun, L, 8);
    l_int32 rightpix = ll_check_l_int32(_fun, L, 9);
    Pix *pixe = nullptr;
    if (pixGenerateSelWithRuns(pixs, nhlines, nvlines, distance, minlength, toppix, botpix, leftpix, rightpix, &pixe))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixe);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBorda * on the Lua stack
 */
static int
GetAllCCBorders(lua_State *L)
{
    LL_FUNC("GetAllCCBorders");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    CCBorda *result = pixGetAllCCBorders(pixs);
    return ll_push_CCBorda(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetAutoFormat(lua_State *L)
{
    LL_FUNC("GetAutoFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 format = 0;
    if (pixGetAutoFormat(pix, &format))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, format);
    return 1;
}

/**
 * \brief Get the pixel average value for Pix* (%pixs) optionally masked with Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the type of stats (type).
 * Arg #3 is optional and, if given, expected to be a Pix* (pixm).
 * Arg #4 is optional and, if given, expected to be a l_int32 (x).
 * Arg #5 is optional and, if given, expected to be a l_int32 (y).
 * Arg #6 is optional and, if given, expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageMasked(lua_State *L)
{
    LL_FUNC("GetAverageMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 3);
    l_int32 x = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 6, 1);
    l_float32 value = 0.0;
    if (pixGetAverageMasked(pixs, pixm, x, y, factor, type, &value))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, value);
    return 3;
}

/**
 * \brief Get the pixel average RGB values for Pix* (%pixs) optionally masked with Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the type of stats (type).
 * Arg #3 is optional and, if given, expected to be a Pix* (pixm).
 * Arg #4 is optional and, if given, expected to be a l_int32 (x).
 * Arg #5 is optional and, if given, expected to be a l_int32 (y).
 * Arg #6 is optional and, if given, expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageMaskedRGB(lua_State *L)
{
    LL_FUNC("GetAverageMaskedRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 3);
    l_int32 x = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 6, 1);
    l_float32 rval = 0.0, gval = 0.0, bval = 0.0;
    if (pixGetAverageMaskedRGB(pixs, pixm, x, y, factor, type, &rval, &gval, &bval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, rval);
    ll_push_l_float32(_fun, L, gval);
    ll_push_l_float32(_fun, L, bval);
    return 3;
}

/**
 * \brief Get the average value for Pix* (%pixs) as tiled Pix* (%pixv).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the type of stats (type).
 * Arg #4 is expected to be a l_int32 (sx).
 * Arg #5 is expected to be a l_int32 (sy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageTiled(lua_State *L)
{
    LL_FUNC("GetAverageTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(_fun, L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(_fun, L, 4, 2);
    Pix *pixv = pixGetAverageTiled(pixs, sx, sy, type);
    return ll_push_Pix(_fun, L, pixv);
}

/**
 * \brief Get the average RGB values for Pix* (%pixs) as three Pix* (%pixr, %pixg, %pixb).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the type of stats (type).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageTiledRGB(lua_State *L)
{
    LL_FUNC("GetAverageTiledRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(_fun, L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(_fun, L, 4, 2);
    Pix *pixr = nullptr;
    Pix *pixg = nullptr;
    Pix *pixb = nullptr;
    if (pixGetAverageTiledRGB(pixs, sx, sy, type, &pixr, &pixg, &pixb))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixr) + ll_push_Pix(_fun, L, pixg) + ll_push_Pix(_fun, L, pixb);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a l_int32 (thresh).
 * Arg #6 is expected to be a l_int32 (mincount).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetBackgroundGrayMap(lua_State *L)
{
    LL_FUNC("GetBackgroundGrayMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    l_int32 thresh = ll_check_l_int32(_fun, L, 5);
    l_int32 mincount = ll_check_l_int32(_fun, L, 6);
    Pix *pixd = nullptr;
    if (pixGetBackgroundGrayMap(pixs, pixim, sx, sy, thresh, mincount, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetBackgroundGrayMapMorph(lua_State *L)
{
    LL_FUNC("GetBackgroundGrayMapMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_int32 size = ll_check_l_int32(_fun, L, 4);
    Pix *pixm = nullptr;
    if (pixGetBackgroundGrayMapMorph(pixs, pixim, reduction, size, &pixm))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixm);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_int32 (sx).
 * Arg #5 is expected to be a l_int32 (sy).
 * Arg #6 is expected to be a l_int32 (thresh).
 * Arg #7 is expected to be a l_int32 (mincount).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetBackgroundRGBMap(lua_State *L)
{
    LL_FUNC("GetBackgroundRGBMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_int32 sx = ll_check_l_int32(_fun, L, 4);
    l_int32 sy = ll_check_l_int32(_fun, L, 5);
    l_int32 thresh = ll_check_l_int32(_fun, L, 6);
    l_int32 mincount = ll_check_l_int32(_fun, L, 7);
    Pix *pixmr = nullptr;
    Pix *pixmg = nullptr;
    Pix *pixmb = nullptr;
    if (pixGetBackgroundRGBMap(pixs, pixim, pixg, sx, sy, thresh, mincount, &pixmr, &pixmg, &pixmb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixmr);
    ll_push_Pix(_fun, L, pixmg);
    ll_push_Pix(_fun, L, pixmb);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetBackgroundRGBMapMorph(lua_State *L)
{
    LL_FUNC("GetBackgroundRGBMapMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_int32 size = ll_check_l_int32(_fun, L, 4);
    Pix *pixmr = nullptr;
    Pix *pixmg = nullptr;
    Pix *pixmb = nullptr;
    if (pixGetBackgroundRGBMapMorph(pixs, pixim, reduction, size, &pixmr, &pixmg, &pixmb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixmr);
    ll_push_Pix(_fun, L, pixmg);
    ll_push_Pix(_fun, L, pixmb);
    return 3;
}

/**
 * \brief Get a binned color for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixg).
 * Arg #3 is expected to be a l_int32 (nbins).
 * Arg #4 is expected to be a l_int32 (factor).
 * Arg #5 is expected to be a Numa* (alut).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack (carray)
 */
static int
GetBinnedColor(lua_State *L)
{
    LL_FUNC("GetBinnedColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 4, 1);
    Numa *alut = ll_check_Numa(_fun, L, 5);
    l_uint32 *carray = nullptr;
    l_int32 res;

    if (pixGetBinnedColor(pixs, pixg, factor, nbins, alut, &carray, 0))
        return ll_push_nil(L);
    res = ll_push_Uarray(_fun, L, carray, nbins);
    LEPT_FREE(carray);
    return res;
}

/**
 * \brief Get a binned component range for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nbins).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a string defining the selected color (color).
 * Arg #5 is optional and, if given, expected to be a l_int32 (fontsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers and 1 table on the Lua stack (minval, maxval, carray)
 */
static int
GetBinnedComponentRange(lua_State *L)
{
    LL_FUNC("GetBinnedComponentRange");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 2, 2);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 color = ll_check_select_color(_fun, L, 4, L_SELECT_RED);
    l_int32 fontsize = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    l_uint32 *carray = nullptr;
    l_int32 res;

    if (pixGetBinnedComponentRange(pixs, nbins, factor, color, &minval, &maxval, &carray, fontsize))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, minval);
    ll_push_l_int32(_fun, L, maxval);
    res = ll_push_Uarray(_fun, L, carray, nbins);
    LEPT_FREE(carray);
    return 2 + res;
}

/**
 * \brief Get black or white value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a string describing the operation (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackOrWhiteVal(lua_State *L)
{
    LL_FUNC("GetBlackOrWhiteVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 op = ll_check_getval(_fun, L, 2, L_GET_BLACK_VAL);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, op, &val))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, val);
}

/**
 * \brief Get black value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackVal(lua_State *L)
{
    LL_FUNC("GetBlackVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_BLACK_VAL, &val))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, val);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBord * on the Lua stack
 */
static int
GetCCBorders(lua_State *L)
{
    LL_FUNC("GetCCBorders");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    CCBord *ccb = pixGetCCBorders(pixs, box);
    return ll_push_CCBord(_fun, L, ccb);
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogram(lua_State *L)
{
    LL_FUNC("GetCmapHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Numa(_fun, L, pixGetCmapHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) inside Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogramInRect(lua_State *L)
{
    LL_FUNC("GetCmapHistogramInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    return ll_push_Numa(_fun, L, pixGetCmapHistogramInRect(pixs, box, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) masked with another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm) (1bpp).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogramMasked(lua_State *L)
{
    LL_FUNC("GetCmapHistogramMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    return ll_push_Numa(_fun, L, pixGetCmapHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get a histogram AMAP* for the colors in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorAmapHistogram(lua_State *L)
{
    LL_FUNC("GetColorAmapHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Amap(_fun, L, pixGetColorAmapHistogram(pixs, factor));
}

/**
 * \brief Get the RGB histograms of the Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 Numa* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogram(lua_State *L)
{
    LL_FUNC("GetColorHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Numa *nar = nullptr;
    Numa *nag = nullptr;
    Numa *nab = nullptr;
    if (pixGetColorHistogram(pixs, factor, &nar, &nag, &nab))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, nar) + ll_push_Numa(_fun, L, nag) + ll_push_Numa(_fun, L, nab);
}

/**
 * \brief Get the RGB histograms of the Pix* (%pixs) masked with another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm) (1bpp).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 Numa* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogramMasked(lua_State *L)
{
    LL_FUNC("GetColorHistogramMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    Numa *nar = nullptr;
    Numa *nag = nullptr;
    Numa *nab = nullptr;
    if (pixGetColorHistogramMasked(pixs, pixm, x, y, factor, &nar, &nag, &nab))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, nar) + ll_push_Numa(_fun, L, nag) + ll_push_Numa(_fun, L, nab);
}

/**
 * \brief Get the color near the mask boundary from Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (dist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorNearMaskBoundary(lua_State *L)
{
    LL_FUNC("GetColorNearMaskBoundary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_uint32 val = 0;
    l_int32 dist = ll_check_l_int32(_fun, L, 4);
    if (pixGetColorNearMaskBoundary(pixs, pixm, box, dist, &val, 0))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, val);
}

/**
 * \brief Get the colormap of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
GetColormap(lua_State *L)
{
    LL_FUNC("GetColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* cmap = pixcmapCopy(pixGetColormap(pix));
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Get column stats for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetColumnStats(lua_State *L)
{
    LL_FUNC("GetColumnStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_select_color(_fun, L, 2, L_SELECT_RED);
    l_int32 nbins = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    l_float32 *rowvect = reinterpret_cast<l_float32 *>(LEPT_CALLOC(nbins, sizeof(l_float32)));
    if (!rowvect) {
        lua_pushfstring(L, "%s: could not allocate rowvect (%d)",
                        _fun, static_cast<size_t>(nbins) * sizeof(*rowvect));
        lua_error(L);
        return 0;
    }
    if (pixGetColumnStats(pixs, type, nbins, thresh, rowvect)) {
        LEPT_FREE(rowvect);
        return ll_push_nil(L);
    }
    ll_push_Farray(_fun, L, rowvect, nbins);
    LEPT_FREE(rowvect);
    return 1;
}

/**
 * \brief Get the data of a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 array table (h) of array tables (wpl) on the Lua stack
 */
static int
GetData(lua_State *L)
{
    LL_FUNC("GetData");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);
    return ll_push_Uarray_2d(_fun, L, data, wpl, h);
}

/**
 * \brief Get the Pix* depth.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDepth(lua_State *L)
{
    LL_FUNC("GetDepth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 depth = pixGetDepth(pix);
    return ll_push_l_int32(_fun, L, depth);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetDifferenceHistogram(lua_State *L)
{
    LL_FUNC("GetDifferenceHistogram");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Numa *result = pixGetDifferenceHistogram(pix1, pix2, factor);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_int32 (mindiff).
 * Arg #7 is expected to be a l_int32 (details).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetDifferenceStats(lua_State *L)
{
    LL_FUNC("GetDifferenceStats");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 4);
    l_float32 fractdiff = 0;
    l_float32 avediff = 0;
    l_int32 details = ll_check_l_int32(_fun, L, 7);
    if (pixGetDifferenceStats(pix1, pix2, factor, mindiff, &fractdiff, &avediff, details))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fractdiff);
    ll_push_l_float32(_fun, L, avediff);
    return 2;
}

/**
 * \brief Get the Pix* dimensions.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers (width, height, depth) on the Lua stack
 */
static int
GetDimensions(lua_State *L)
{
    LL_FUNC("GetDimensions");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width, height, depth;
    if (pixGetDimensions(pix, &width, &height, &depth))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, width) + ll_push_l_int32(_fun, L, height) + ll_push_l_int32(_fun, L, depth);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (side).
 * Arg #3 is expected to be a string (debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetEdgeProfile(lua_State *L)
{
    LL_FUNC("GetEdgeProfile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 side = ll_check_l_int32(_fun, L, 2);
    const char *debugfile = ll_check_string(_fun, L, 3);
    Numa *result = pixGetEdgeProfile(pixs, side, debugfile);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Get an extreme value for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a string describing the type (type; min or max).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (rval, gval, bval, grayval)
 */
static int
GetExtremeValue(lua_State *L)
{
    LL_FUNC("GetExtremeValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 type = ll_check_select_min_max(_fun, L, 3, L_SELECT_MIN);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 grayval = 0;
    if (pixGetExtremeValue(pixs, factor, type, &rval, &gval, &bval, &grayval))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, rval) +
        ll_push_l_int32(_fun, L, gval) +
        ll_push_l_int32(_fun, L, bval) +
        ll_push_l_int32(_fun, L, grayval);
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogram(lua_State *L)
{
    LL_FUNC("GetGrayHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Numa(_fun, L, pixGetGrayHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) inside Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogramInRect(lua_State *L)
{
    LL_FUNC("GetGrayHistogramInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    return ll_push_Numa(_fun, L, pixGetGrayHistogramInRect(pixs, box, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) masked with another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm) (1bpp).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogramMasked(lua_State *L)
{
    LL_FUNC("GetGrayHistogramMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    return ll_push_Numa(_fun, L, pixGetGrayHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get an array of histograms of the grayscale Pix* (%pixs) for %nx by %ny tiles.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (nx).
 * Arg #4 is expected to be a l_int32 (ny).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
static int
GetGrayHistogramTiled(lua_State *L)
{
    LL_FUNC("GetGrayHistogramTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 nx = ll_check_l_int32(_fun, L, 3);
    l_int32 ny = ll_check_l_int32(_fun, L, 4);
    return ll_push_Numaa(_fun, L, pixGetGrayHistogramTiled(pixs, factor, nx, ny));
}

/**
 * \brief Get the Pix* height.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetHeight(lua_State *L)
{
    LL_FUNC("GetHeight");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 height = pixGetHeight(pix);
    ll_push_l_int32(_fun, L, height);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBord* (ccb).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (xs).
 * Arg #5 is expected to be a l_int32 (ys).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetHoleBorder(lua_State *L)
{
    LL_FUNC("GetHoleBorder");
    CCBord *ccb = ll_check_CCBord(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 xs = ll_check_l_int32(_fun, L, 4);
    l_int32 ys = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixGetHoleBorder(ccb, pixs, box, xs, ys));
}

/**
 * \brief Get the input format of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetInputFormat(lua_State *L)
{
    LL_FUNC("GetInputFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushstring(L, ll_string_input_format(pixGetInputFormat(pix)));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (bgval).
 * Arg #3 is expected to be a l_int32 (smoothx).
 * Arg #4 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GetInvBackgroundMap(lua_State *L)
{
    LL_FUNC("GetInvBackgroundMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 bgval = ll_check_l_int32(_fun, L, 2);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 3);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixGetInvBackgroundMap(pixs, bgval, smoothx, smoothy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetLastOffPixelInRun(lua_State *L)
{
    LL_FUNC("GetLastOffPixelInRun");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 direction = ll_check_l_int32(_fun, L, 4);
    l_int32 loc = 0;
    if (pixGetLastOffPixelInRun(pixs, x, y, direction, &loc))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, loc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetLastOnPixelInRun(lua_State *L)
{
    LL_FUNC("GetLastOnPixelInRun");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 direction = ll_check_l_int32(_fun, L, 4);
    l_int32 loc = 0;
    if (pixGetLastOnPixelInRun(pixs, x, y, direction, &loc))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, loc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void ** on the Lua stack
 */
static int
GetLinePtrs(lua_State *L)
{
    LL_FUNC("GetLinePtrs");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 size = 0;
    if (pixGetLinePtrs(pix, &size))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, size);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nslices).
 * Arg #3 is expected to be a l_int32 (redsweep).
 * Arg #4 is expected to be a l_int32 (redsearch).
 * Arg #5 is expected to be a l_float32 (sweeprange).
 * Arg #6 is expected to be a l_float32 (sweepdelta).
 * Arg #7 is expected to be a l_float32 (minbsdelta).
 * Arg #8 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetLocalSkewAngles(lua_State *L)
{
    LL_FUNC("GetLocalSkewAngles");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nslices = ll_check_l_int32(_fun, L, 2);
    l_int32 redsweep = ll_check_l_int32(_fun, L, 3);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 4);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 5);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 6);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 7);
    l_int32 debug = ll_check_boolean_default(_fun, L, 8, FALSE);
    l_float32 a = 0;
    l_float32 b = 0;
    if (pixGetLocalSkewAngles(pixs, nslices, redsweep, redsearch, sweeprange, sweepdelta, minbsdelta, &a, &b, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, a);
    ll_push_l_float32(_fun, L, b);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nslices).
 * Arg #3 is expected to be a l_int32 (redsweep).
 * Arg #4 is expected to be a l_int32 (redsearch).
 * Arg #5 is expected to be a l_float32 (sweeprange).
 * Arg #6 is expected to be a l_float32 (sweepdelta).
 * Arg #7 is expected to be a l_float32 (minbsdelta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetLocalSkewTransform(lua_State *L)
{
    LL_FUNC("GetLocalSkewTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nslices = ll_check_l_int32(_fun, L, 2);
    l_int32 redsweep = ll_check_l_int32(_fun, L, 3);
    l_int32 redsearch = ll_check_l_int32(_fun, L, 4);
    l_float32 sweeprange = ll_check_l_float32(_fun, L, 5);
    l_float32 sweepdelta = ll_check_l_float32(_fun, L, 6);
    l_float32 minbsdelta = ll_check_l_float32(_fun, L, 7);
    Pta *ptas = nullptr;
    Pta *ptad = nullptr;
    if (pixGetLocalSkewTransform(pixs, nslices, redsweep, redsearch, sweeprange, sweepdelta, minbsdelta, &ptas, &ptad))
        return ll_push_nil(L);
    return ll_push_Pta(_fun, L, ptas) +
            ll_push_Pta(_fun, L, ptad);
}

/**
 * \brief Get the maximum value for Pix* (%pixs) optionally in rect Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (maxval, xmax, ymax)
 */
static int
GetMaxValueInRect(lua_State *L)
{
    LL_FUNC("GetMaxValueInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_uint32 maxval = 0;
    l_int32 xmax = 0;
    l_int32 ymax = 0;
    if (pixGetMaxValueInRect(pixs, box, &maxval, &xmax, &ymax))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, maxval) +
            ll_push_l_int32(_fun, L, xmax) +
            ll_push_l_int32(_fun, L, ymax);
}

/**
 * \brief Get the moment of order (%order) by column in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (order; 1 or 2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetMomentByColumn(lua_State *L)
{
    LL_FUNC("GetMomentByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 order = ll_check_l_int32(_fun, L, 2);
    Numa *na = pixGetMomentByColumn(pixs, order);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sigbits).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_int32 (ncolors).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetMostPopulatedColors(lua_State *L)
{
    LL_FUNC("GetMostPopulatedColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sigbits = ll_check_l_int32(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 ncolors = ll_check_l_int32(_fun, L, 4);
    l_uint32 *array = nullptr;
    PixColormap *cmap = nullptr;
    if (pixGetMostPopulatedColors(pixs, sigbits, factor, ncolors, &array, &cmap))
        return ll_push_nil(L);
    ll_push_Uarray(_fun, L, array, ncolors);
    ll_push_PixColormap(_fun, L, cmap);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBord* (ccb).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetOuterBorder(lua_State *L)
{
    LL_FUNC("GetOuterBorder");
    CCBord *ccb = ll_check_CCBord(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 result = pixGetOuterBorder(ccb, pixs, box);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
GetOuterBorderPta(lua_State *L)
{
    LL_FUNC("GetOuterBorderPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Pta *pta = pixGetOuterBorderPta(pixs, box);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa * on the Lua stack
 */
static int
GetOuterBordersPtaa(lua_State *L)
{
    LL_FUNC("GetOuterBordersPtaa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaa = pixGetOuterBordersPtaa(pixs);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_float32 on the Lua stack
 */
static int
GetPSNR(lua_State *L)
{
    LL_FUNC("GetPSNR");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_float32 psnr = 0;
    if (pixGetPSNR(pix1, pix2, factor, &psnr))
        return ll_push_nil(L);
    return ll_push_l_float32(_fun, L, psnr);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a l_int32 (sampling).
 * Arg #4 is expected to be a l_int32 (dilation).
 * Arg #5 is expected to be a l_int32 (mindiff).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetPerceptualDiff(lua_State *L)
{
    LL_FUNC("GetPerceptualDiff");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    l_int32 sampling = ll_check_l_int32(_fun, L, 3);
    l_int32 dilation = ll_check_l_int32(_fun, L, 4);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 5);
    l_float32 fract = 0;
    Pix *pixdiff1 = nullptr;
    Pix *pixdiff2 = nullptr;
    if (pixGetPerceptualDiff(pixs1, pixs2, sampling, dilation, mindiff, &fract, &pixdiff1, &pixdiff2))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, fract);
    ll_push_Pix(_fun, L, pixdiff1);
    ll_push_Pix(_fun, L, pixdiff2);
    return 3;
}

/**
 * \brief Get a pixel value from Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
GetPixel(lua_State *L)
{
    LL_FUNC("GetPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_uint32 val = 0;
    if (pixGetPixel(pix, x, y, &val))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, val);
}

/**
 * \brief Get the pixel average for Pix* (%pixs) optionally masked with Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Pix* (pixm).
 * Arg #3 is optional and, if given, expected to be a l_int32 (x).
 * Arg #4 is optional and, if given, expected to be a l_int32 (y).
 * Arg #5 is optional and, if given, expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPixelAverage(lua_State *L)
{
    LL_FUNC("GetPixelAverage");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 2);
    l_int32 x = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 5, 1);
    l_uint32 value = 0;
    if (pixGetPixelAverage(pixs, pixm, x,y, factor, &value))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, value);
}

/**
 * \brief Get the pixel stats for Pix* (%pixs) optionally masked with Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the type of stats (type).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPixelStats(lua_State *L)
{
    LL_FUNC("GetPixelStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_uint32 value = 0;
    if (pixGetPixelStats(pixs, factor, type, &value))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, value);
}

/**
 * \brief Create a new Pix* (%pixd) from one component (%comp) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string with the component name (comp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (pixd) on the Lua stack
 */
static int
GetRGBComponent(lua_State *L)
{
    LL_FUNC("GetRGBComponent");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 comp = ll_check_component(_fun, L, 2, L_ALPHA_CHANNEL);
    Pix *pixd = pixGetRGBComponent(pixs, comp);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (comp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GetRGBComponentCmap(lua_State *L)
{
    LL_FUNC("GetRGBComponentCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 comp = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixGetRGBComponentCmap(pixs, comp);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sigbits).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetRGBHistogram(lua_State *L)
{
    LL_FUNC("GetRGBHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sigbits = ll_check_l_int32(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Numa *result = pixGetRGBHistogram(pixs, sigbits, factor);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Extract red, green and blue components from Pix* (pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 lstrings on the Lua stack (%bufr, %bufg, %bufb)
 */
static int
GetRGBLine(lua_State *L)
{
    LL_FUNC("GetRGBLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    size_t width = static_cast<size_t>(pixGetWidth(pixs));
    l_uint8 *bufr = ll_calloc<l_uint8>(_fun, L, width);
    l_uint8 *bufg = ll_calloc<l_uint8>(_fun, L, width);
    l_uint8 *bufb = ll_calloc<l_uint8>(_fun, L, width);
    if (pixGetRGBLine(pixs, row, bufr, bufg, bufb)) {
        ll_free(bufr);
        ll_free(bufg);
        ll_free(bufb);
        return ll_push_nil(L);
    }
    lua_pushlstring(L, reinterpret_cast<const char *>(bufr), width);
    lua_pushlstring(L, reinterpret_cast<const char *>(bufg), width);
    lua_pushlstring(L, reinterpret_cast<const char *>(bufb), width);
    ll_free(bufr);
    ll_free(bufg);
    ll_free(bufb);
    return 3;
}
/**
 * \brief Get a pixel's RGB values from Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack
 */
static int
GetRGBPixel(lua_State *L)
{
    LL_FUNC("GetRGBPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    if (pixGetRGBPixel(pix, x, y, &rval, &gval, &bval))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, rval) +
        ll_push_l_int32(_fun, L, gval) +
        ll_push_l_int32(_fun, L, bval);
}

/**
 * \brief Get a random pixel's value from Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack (val, x, y)
 */
static int
GetRandomPixel(lua_State *L)
{
    LL_FUNC("GetRandomPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = 0;
    l_int32 y = 0;
    l_uint32 val = 0;
    if (pixGetRandomPixel(pix, &val, &x, &y))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, val) +
        ll_push_l_int32(_fun, L, x) +
        ll_push_l_int32(_fun, L, y);
}

/**
 * \brief Get the range values for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack (minval, maxval)
 */
static int
GetRangeValues(lua_State *L)
{
    LL_FUNC("GetRangeValues");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 color = ll_check_select_color(_fun, L, 3, L_SELECT_RED);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    if (pixGetRangeValues(pixs, factor, color, &minval, &maxval))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, minval) +
            ll_push_l_int32(_fun, L, maxval);
}

/**
 * \brief Get a rank color array for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nbins).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a string defining the selected color (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack (carray)
 */
static int
GetRankColorArray(lua_State *L)
{
    LL_FUNC("GetRankColorArray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 type = ll_check_select_color(_fun, L, 4, L_SELECT_AVERAGE);
    l_uint32 *carray = nullptr;
    l_int32 res;

    if (pixGetRankColorArray(pixs, nbins, type, factor, &carray, 0, 0))
        return ll_push_nil(L);
    res = ll_push_Uarray(_fun, L, carray, nbins);
    LEPT_FREE(carray);
    return res;
}

/**
 * \brief Get rank value for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetRankValue(lua_State *L)
{
    LL_FUNC("GetRankValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_float32 rank = ll_check_l_float32(_fun, L, 3);
    l_uint32 value = 0;
    if (pixGetRankValue(pixs, factor, rank, &value))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, value);
}

/**
 * \brief Get rank value for Pix* (%pixs) masked with another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (factor).
 * Arg #6 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number (value) and a Numa* (histogram) on the Lua stack
 */
static int
GetRankValueMasked(lua_State *L)
{
    LL_FUNC("GetRankValueMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    l_float32 rank = ll_check_l_int32(_fun, L, 6);
    l_float32 value = 0.0f;
    Numa *na = nullptr;
    if (pixGetRankValueMasked(pixs, pixm, x, y, factor, rank, &value, &na))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, value);
    ll_push_Numa(_fun, L, na);
    return 2;
}

/**
 * \brief Get rank values for RGB for Pix* (%pixs) masked with another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (factor).
 * Arg #6 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetRankValueMaskedRGB(lua_State *L)
{
    LL_FUNC("GetRankValueMaskedRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    l_float32 rank = ll_check_l_int32(_fun, L, 6);
    l_float32 rval = 0.0f, gval = 0.0f, bval = 0.0f;
    if (pixGetRankValueMaskedRGB(pixs, pixm, x, y, factor, rank, &rval, &gval, &bval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, rval);
    ll_push_l_float32(_fun, L, gval);
    ll_push_l_float32(_fun, L, bval);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetRasterData(lua_State *L)
{
    LL_FUNC("GetRasterData");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (pixGetRasterData(pixs, &data, &nbytes))
        return ll_push_nil(L);
    return ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), nbytes);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 refcount = pixGetRefcount(pix);
    return ll_push_l_int32(_fun, L, refcount);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #5 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetRegionsBinary(lua_State *L)
{
    LL_FUNC("GetRegionsBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixhm = nullptr;
    Pix *pixtm = nullptr;
    Pix *pixtb = nullptr;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 5);
    if (pixGetRegionsBinary(pixs, &pixhm, &pixtm, &pixtb, pixadb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixhm);
    ll_push_Pix(_fun, L, pixtm);
    ll_push_Pix(_fun, L, pixtb);
    return 3;
}

/**
 * \brief Get the PIX resolution (x, y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for two integer on the Lua stack
 */
static int
GetResolution(lua_State *L)
{
    LL_FUNC("GetResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres, yres;
    if (pixGetResolution(pix, &xres, &yres))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, xres) +
            ll_push_l_int32(_fun, L, yres);
}

/**
 * \brief Get row stats for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetRowStats(lua_State *L)
{
    LL_FUNC("GetRowStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_select_color(_fun, L, 2, L_SELECT_RED);
    l_int32 nbins = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    l_float32 *colvect = ll_calloc<l_float32>(_fun, L, nbins);
    if (pixGetRowStats(pixs, type, nbins, thresh, colvect)) {
        LEPT_FREE(colvect);
        return ll_push_nil(L);
    }
    ll_push_Farray(_fun, L, colvect, nbins);
    LEPT_FREE(colvect);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_int32 (minlength).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetRunCentersOnLine(lua_State *L)
{
    LL_FUNC("GetRunCentersOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 minlength = ll_check_l_int32(_fun, L, 4);
    Numa *na = pixGetRunCentersOnLine(pixs, x, y, minlength);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
GetRunsOnLine(lua_State *L)
{
    LL_FUNC("GetRunsOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    Numa *na = pixGetRunsOnLine(pixs, x1, y1, x2, y2);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_int32 (conn).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetSortedNeighborValues(lua_State *L)
{
    LL_FUNC("GetSortedNeighborValues");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 conn = ll_check_l_int32(_fun, L, 4);
    l_int32 *neigh = 0;
    l_int32 nvals = 0;
    if (pixGetSortedNeighborValues(pixs, x, y, conn, &neigh, &nvals))
        return ll_push_nil(L);
    ll_push_Iarray(_fun, L, neigh, nvals);
    ll_free(neigh);
    return 1;
}

/**
 * \brief Get the Pix* (%pix) samples per pixel (%spp).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetSpp(lua_State *L)
{
    LL_FUNC("GetSpp");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 spp = pixGetSpp(pix);
    return ll_push_l_int32(_fun, L, spp);
}

/**
 * \brief Get the text of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetText(lua_State *L)
{
    LL_FUNC("GetText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = pixGetText(pix);
    lua_pushstring(L, text);
    return 1;
}

/**
 * \brief Get white value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWhiteVal(lua_State *L)
{
    LL_FUNC("GetWhiteVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_WHITE_VAL, &val))
        return ll_push_nil(L);
    return ll_push_l_uint32(_fun, L, val);
}

/**
 * \brief Get the Pix* width.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWidth(lua_State *L)
{
    LL_FUNC("GetWidth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = pixGetWidth(pix);
    return ll_push_l_int32(_fun, L, width);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minwidth).
 * Arg #3 is expected to be a l_int32 (minheight).
 * Arg #4 is expected to be a l_int32 (maxwidth).
 * Arg #5 is expected to be a l_int32 (maxheight).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetWordBoxesInTextlines(lua_State *L)
{
    LL_FUNC("GetWordBoxesInTextlines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minwidth = ll_check_l_int32(_fun, L, 2);
    l_int32 minheight = ll_check_l_int32(_fun, L, 3);
    l_int32 maxwidth = ll_check_l_int32(_fun, L, 4);
    l_int32 maxheight = ll_check_l_int32(_fun, L, 5);
    Boxa *boxad = nullptr;
    Numa *nai = nullptr;
    if (pixGetWordBoxesInTextlines(pixs, minwidth, minheight, maxwidth, maxheight, &boxad, &nai))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxad);
    ll_push_Numa(_fun, L, nai);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minwidth).
 * Arg #3 is expected to be a l_int32 (minheight).
 * Arg #4 is expected to be a l_int32 (maxwidth).
 * Arg #5 is expected to be a l_int32 (maxheight).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetWordsInTextlines(lua_State *L)
{
    LL_FUNC("GetWordsInTextlines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minwidth = ll_check_l_int32(_fun, L, 2);
    l_int32 minheight = ll_check_l_int32(_fun, L, 3);
    l_int32 maxwidth = ll_check_l_int32(_fun, L, 4);
    l_int32 maxheight = ll_check_l_int32(_fun, L, 5);
    Boxa *boxad = nullptr;
    Pixa *pixad = nullptr;
    Numa *nai = nullptr;
    if (pixGetWordsInTextlines(pixs, minwidth, minheight, maxwidth, maxheight, &boxad, &pixad, &nai))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxad);
    ll_push_Pixa(_fun, L, pixad);
    ll_push_Numa(_fun, L, nai);
    return 3;
}

/**
 * \brief Get the Pix* (%pix) words per line (%wpl).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWpl(lua_State *L)
{
    LL_FUNC("GetWpl");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = pixGetWpl(pix);
    return ll_push_l_int32(_fun, L, wpl);
}

/**
 * \brief Get the Pix* X resolution.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetXRes(lua_State *L)
{
    LL_FUNC("GetXRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = pixGetXRes(pix);
    return ll_push_l_int32(_fun, L, xres);
}

/**
 * \brief Get the Pix* Y resolution.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetYRes(lua_State *L)
{
    LL_FUNC("GetYRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 yres = pixGetYRes(pix);
    return ll_push_l_int32(_fun, L, yres);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (rval).
 * Arg #4 is expected to be a l_int32 (gval).
 * Arg #5 is expected to be a l_int32 (bval).
 * Arg #6 is expected to be a l_int32 (factor).
 * Arg #7 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GlobalNormNoSatRGB(lua_State *L)
{
    LL_FUNC("GlobalNormNoSatRGB");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 rval = ll_check_l_int32(_fun, L, 3);
    l_int32 gval = ll_check_l_int32(_fun, L, 4);
    l_int32 bval = ll_check_l_int32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    l_float32 rank = ll_check_l_float32(_fun, L, 7);
    Pix *pix = pixGlobalNormNoSatRGB(pixd, pixs, rval, gval, bval, factor, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (rval).
 * Arg #4 is expected to be a l_int32 (gval).
 * Arg #5 is expected to be a l_int32 (bval).
 * Arg #6 is expected to be a l_int32 (mapval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GlobalNormRGB(lua_State *L)
{
    LL_FUNC("GlobalNormRGB");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 rval = ll_check_l_int32(_fun, L, 3);
    l_int32 gval = ll_check_l_int32(_fun, L, 4);
    l_int32 bval = ll_check_l_int32(_fun, L, 5);
    l_int32 mapval = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixGlobalNormRGB(pixd, pixs, rval, gval, bval, mapval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * Arg #4 is expected to be a l_int32 (dispy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GrayMorphSequence(lua_State *L)
{
    LL_FUNC("GrayMorphSequence");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    l_int32 dispy = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixGrayMorphSequence(pixs, sequence, dispsep, dispy);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a PixColormap* (cmap).
 * Arg #3 is expected to be a l_int32 (mindepth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GrayQuantFromCmap(lua_State *L)
{
    LL_FUNC("GrayQuantFromCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 2);
    l_int32 mindepth = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixGrayQuantFromCmap(pixs, cmap, mindepth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_float32 (minfract).
 * Arg #5 is expected to be a l_int32 (maxsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
GrayQuantFromHisto(lua_State *L)
{
    LL_FUNC("GrayQuantFromHisto");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_float32 minfract = ll_check_l_float32(_fun, L, 4);
    l_int32 maxsize = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixGrayQuantFromHisto(pixd, pixs, pixm, minfract, maxsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (height).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HDome(lua_State *L)
{
    LL_FUNC("HDome");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 height = ll_check_l_int32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixHDome(pixs, height, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HMT(lua_State *L)
{
    LL_FUNC("HMT");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixHMT(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (yloc).
 * Arg #4 is expected to be a l_float32 (radang).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HShear(lua_State *L)
{
    LL_FUNC("HShear");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 yloc = ll_check_l_int32(_fun, L, 3);
    l_float32 radang = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixHShear(pixd, pixs, yloc, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HShearCenter(lua_State *L)
{
    LL_FUNC("HShearCenter");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixHShearCenter(pixd, pixs, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HShearCorner(lua_State *L)
{
    LL_FUNC("HShearCorner");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixHShearCorner(pixd, pixs, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (yloc).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
HShearIP(lua_State *L)
{
    LL_FUNC("HShearIP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 yloc = ll_check_l_int32(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixHShearIP(pixs, yloc, radang, incolor));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (yloc).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
HShearLI(lua_State *L)
{
    LL_FUNC("HShearLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 yloc = ll_check_l_int32(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixHShearLI(pixs, yloc, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sm1h).
 * Arg #3 is expected to be a l_int32 (sm1v).
 * Arg #4 is expected to be a l_int32 (sm2h).
 * Arg #5 is expected to be a l_int32 (sm2v).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HalfEdgeByBandpass(lua_State *L)
{
    LL_FUNC("HalfEdgeByBandpass");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sm1h = ll_check_l_int32(_fun, L, 2);
    l_int32 sm1v = ll_check_l_int32(_fun, L, 3);
    l_int32 sm2h = ll_check_l_int32(_fun, L, 4);
    l_int32 sm2v = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixHalfEdgeByBandpass(pixs, sm1h, sm1v, sm2h, sm2v);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a l_float32 (fthresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
HasHighlightRed(lua_State *L)
{
    LL_FUNC("HasHighlightRed");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_float32 fthresh = ll_check_l_float32(_fun, L, 4);
    l_int32 hasred = 0;
    l_float32 ratio = 0;
    Pix *pixdb = nullptr;
    if (pixHasHighlightRed(pixs, factor, fract, fthresh, &hasred, &ratio, &pixdb))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, hasred);
    ll_push_l_float32(_fun, L, ratio);
    ll_push_Pix(_fun, L, pixdb);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a Pix* (pix3).
 * Arg #4 is expected to be a Pix* (pix4).
 * Arg #5 is expected to be a l_float32 (delx).
 * Arg #6 is expected to be a l_float32 (dely).
 * Arg #7 is expected to be a l_int32 (maxdiffw).
 * Arg #8 is expected to be a l_int32 (maxdiffh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Haustest(lua_State *L)
{
    LL_FUNC("Haustest");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    Pix *pix3 = ll_check_Pix(_fun, L, 3);
    Pix *pix4 = ll_check_Pix(_fun, L, 4);
    l_float32 delx = ll_check_l_float32(_fun, L, 5);
    l_float32 dely = ll_check_l_float32(_fun, L, 6);
    l_int32 maxdiffw = ll_check_l_int32(_fun, L, 7);
    l_int32 maxdiffh = ll_check_l_int32(_fun, L, 8);
    return ll_push_boolean(_fun, L, 0 == pixHaustest(pix1, pix2, pix3, pix4, delx, dely, maxdiffw, maxdiffh));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HolesByFilling(lua_State *L)
{
    LL_FUNC("HolesByFilling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixHolesByFilling(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (w).
 * Arg #2 is expected to be a l_int32 (h).
 * Arg #3 is expected to be a l_uint32 (offset).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
InitAccumulate(lua_State *L)
{
    LL_FUNC("InitAccumulate");
    l_int32 w = ll_check_l_int32(_fun, L, 1);
    l_int32 h = ll_check_l_int32(_fun, L, 2);
    l_uint32 offset = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixInitAccumulate(w, h, offset);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Sela* (sela).
 * Arg #3 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
IntersectionOfMorphOps(lua_State *L)
{
    LL_FUNC("IntersectionOfMorphOps");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Sela *sela = ll_check_Sela(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixIntersectionOfMorphOps(pixs, sela, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxaw).
 * Arg #3 is expected to be a Pix* (pixw).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxa) on the Lua stack
 */
static int
ItalicWords(lua_State *L)
{
    LL_FUNC("ItalicWords");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxaw = ll_check_Boxa(_fun, L, 2);
    Pix *pixw = ll_check_Pix(_fun, L, 3);
    Boxa *boxa = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    if (pixItalicWords(pixs, boxaw, pixw, &boxa, debugflag))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (fadeto).
 * Arg #4 is expected to be a l_float32 (distfract).
 * Arg #5 is expected to be a l_float32 (maxfade).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
LinearEdgeFade(lua_State *L)
{
    LL_FUNC("LinearEdgeFade");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 fadeto = ll_check_l_int32(_fun, L, 3);
    l_float32 distfract = ll_check_l_float32(_fun, L, 4);
    l_float32 maxfade = ll_check_l_float32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixLinearEdgeFade(pixs, dir, fadeto, distfract, maxfade));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (srcval).
 * Arg #4 is expected to be a l_uint32 (dstval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
LinearMapToTargetColor(lua_State *L)
{
    LL_FUNC("LinearMapToTargetColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_uint32 srcval = ll_check_l_uint32(_fun, L, 3);
    l_uint32 dstval = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixLinearMapToTargetColor(pixd, pixs, srcval, dstval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a Pix* (pixmin).
 * Arg #6 is expected to be a Pix* (pixmax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
LinearTRCTiled(lua_State *L)
{
    LL_FUNC("LinearTRCTiled");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    Pix *pixmin = ll_check_Pix(_fun, L, 5);
    Pix *pixmax = ll_check_Pix(_fun, L, 6);
    Pix *pix = pixLinearTRCTiled(pixd, pixs, sx, sy, pixmin, pixmax);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
LocToColorTransform(lua_State *L)
{
    LL_FUNC("LocToColorTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixLocToColorTransform(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxmin).
 * Arg #3 is expected to be a l_int32 (minmax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
LocalExtrema(lua_State *L)
{
    LL_FUNC("LocalExtrema");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxmin = ll_check_l_int32(_fun, L, 2);
    l_int32 minmax = ll_check_l_int32(_fun, L, 3);
    Pix *pixmin = nullptr;
    Pix *pixmax = nullptr;
    if (pixLocalExtrema(pixs, maxmin, minmax, &pixmin, &pixmax))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixmin);
    ll_push_Pix(_fun, L, pixmax);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
LocateBarcodes(lua_State *L)
{
    LL_FUNC("LocateBarcodes");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    Pix *pixb = nullptr;
    Pix *pixm = nullptr;
    if (pixLocateBarcodes(pixs, thresh, &pixb, &pixm))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixb);
    ll_push_Pix(_fun, L, pixm);
    return 2;
}

/**
 * \brief Create a new alpha mask Pix* (%pixd) from Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dist).
 * Arg #3 is optional and, if given, expected to be a boolean (getbox).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack or 2 Pix* and Box* on the Lua stack
 */
static int
MakeAlphaFromMask(lua_State *L)
{
    LL_FUNC("MakeAlphaFromMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dist = ll_check_l_int32(_fun, L, 2);
    int getbox = ll_check_boolean_default(_fun, L, 3, FALSE);
    Box* box = nullptr;
    Pix* pixd = pixMakeAlphaFromMask(pixs, dist, getbox ? &box : nullptr);
    if (!pixd)
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    return 1 + (box ? ll_push_Box(_fun, L, box) : 0);
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using arithmetic factors (%rc, %gc, %bc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rc).
 * Arg #3 is expected to be a l_float32 (gc).
 * Arg #4 is expected to be a l_float32 (bc).
 * Arg #5 is expected to be a l_float32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeArbMaskFromRGB(lua_State *L)
{
    LL_FUNC("MakeArbMaskFromRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rc = ll_check_l_float32(_fun, L, 2);
    l_float32 gc = ll_check_l_float32(_fun, L, 3);
    l_float32 bc = ll_check_l_float32(_fun, L, 4);
    l_float32 thresh = ll_check_l_float32(_fun, L, 5);
    ll_push_Pix(_fun, L, pixMakeArbMaskFromRGB(pixs, rc, gc, bc, thresh));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (w).
 * Arg #2 is expected to be a l_int32 (h).
 * Arg #3 is expected to be a l_float32 (hf1).
 * Arg #4 is expected to be a l_float32 (hf2).
 * Arg #5 is expected to be a l_float32 (vf1).
 * Arg #6 is expected to be a l_float32 (vf2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeFrameMask(lua_State *L)
{
    LL_FUNC("MakeFrameMask");
    l_int32 w = ll_check_l_int32(_fun, L, 1);
    l_int32 h = ll_check_l_int32(_fun, L, 2);
    l_float32 hf1 = ll_check_l_float32(_fun, L, 3);
    l_float32 hf2 = ll_check_l_float32(_fun, L, 4);
    l_float32 vf1 = ll_check_l_float32(_fun, L, 5);
    l_float32 vf2 = ll_check_l_float32(_fun, L, 6);
    Pix *pix = pixMakeFrameMask(w, h, hf1, hf2, vf1, vf2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeHistoHS(lua_State *L)
{
    LL_FUNC("MakeHistoHS");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Numa *nahue = nullptr;
    Numa *nasat = nullptr;
    if (pixMakeHistoHS(pixs, factor, &nahue, &nasat))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, nahue);
    ll_push_Numa(_fun, L, nasat);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeHistoHV(lua_State *L)
{
    LL_FUNC("MakeHistoHV");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Numa *nahue = nullptr;
    Numa *naval = nullptr;
    if (pixMakeHistoHV(pixs, factor, &nahue, &naval))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, nahue);
    ll_push_Numa(_fun, L, naval);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeHistoSV(lua_State *L)
{
    LL_FUNC("MakeHistoSV");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Numa *nasat = nullptr;
    Numa *naval = nullptr;
    if (pixMakeHistoSV(pixs, factor, &nasat, &naval))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, nasat);
    ll_push_Numa(_fun, L, naval);
    return 2;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a 2^depth entry lookup-table (%lut).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string of max. 256 bytes (lut).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromLUT(lua_State *L)
{
    LL_FUNC("MakeMaskFromLUT");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    size_t len = 0;
    const char* lut = ll_check_lstring(_fun, L, 2, &len);
    l_int32* tab = tab = ll_calloc<l_int32>(_fun, L, 256);
    size_t i;
    /* expand lookup-table (lut) to array of l_int32 (tab) */
    for (i = 0; i < 256 && i < len; i++)
        tab[i] = lut[i];
    ll_push_Pix(_fun, L, pixMakeMaskFromLUT(pixs, tab));
    LEPT_FREE(tab);
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a mask value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromVal(lua_State *L)
{
    LL_FUNC("MakeMaskFromVal");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    ll_push_Pix(_fun, L, pixMakeMaskFromVal(pixs, val));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (huecenter).
 * Arg #3 is expected to be a l_int32 (huehw).
 * Arg #4 is expected to be a l_int32 (satcenter).
 * Arg #5 is expected to be a l_int32 (sathw).
 * Arg #6 is expected to be a l_int32 (regionflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeRangeMaskHS(lua_State *L)
{
    LL_FUNC("MakeRangeMaskHS");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 huecenter = ll_check_l_int32(_fun, L, 2);
    l_int32 huehw = ll_check_l_int32(_fun, L, 3);
    l_int32 satcenter = ll_check_l_int32(_fun, L, 4);
    l_int32 sathw = ll_check_l_int32(_fun, L, 5);
    l_int32 regionflag = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixMakeRangeMaskHS(pixs, huecenter, huehw, satcenter, sathw, regionflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (huecenter).
 * Arg #3 is expected to be a l_int32 (huehw).
 * Arg #4 is expected to be a l_int32 (valcenter).
 * Arg #5 is expected to be a l_int32 (valhw).
 * Arg #6 is expected to be a l_int32 (regionflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeRangeMaskHV(lua_State *L)
{
    LL_FUNC("MakeRangeMaskHV");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 huecenter = ll_check_l_int32(_fun, L, 2);
    l_int32 huehw = ll_check_l_int32(_fun, L, 3);
    l_int32 valcenter = ll_check_l_int32(_fun, L, 4);
    l_int32 valhw = ll_check_l_int32(_fun, L, 5);
    l_int32 regionflag = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixMakeRangeMaskHV(pixs, huecenter, huehw, valcenter, valhw, regionflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (satcenter).
 * Arg #3 is expected to be a l_int32 (sathw).
 * Arg #4 is expected to be a l_int32 (valcenter).
 * Arg #5 is expected to be a l_int32 (valhw).
 * Arg #6 is expected to be a l_int32 (regionflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MakeRangeMaskSV(lua_State *L)
{
    LL_FUNC("MakeRangeMaskSV");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 satcenter = ll_check_l_int32(_fun, L, 2);
    l_int32 sathw = ll_check_l_int32(_fun, L, 3);
    l_int32 valcenter = ll_check_l_int32(_fun, L, 4);
    l_int32 valhw = ll_check_l_int32(_fun, L, 5);
    l_int32 regionflag = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixMakeRangeMaskSV(pixs, satcenter, sathw, valcenter, valhw, regionflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Boxa* (boxa).
 * Arg #4 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaskBoxa(lua_State *L)
{
    LL_FUNC("MaskBoxa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Boxa *boxa = ll_check_Boxa(_fun, L, 3);
    l_int32 op = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixMaskBoxa(pixd, pixs, boxa, op);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaskConnComp(lua_State *L)
{
    LL_FUNC("MaskConnComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Boxa *boxa = nullptr;
    if (pixMaskConnComp(pixs, connectivity, &boxa))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (threshdiff).
 * Arg #3 is expected to be a l_int32 (mindist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaskOverColorPixels(lua_State *L)
{
    LL_FUNC("MaskOverColorPixels");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 threshdiff = ll_check_l_int32(_fun, L, 2);
    l_int32 mindist = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMaskOverColorPixels(pixs, threshdiff, mindist);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (rmin).
 * Arg #3 is expected to be a l_int32 (rmax).
 * Arg #4 is expected to be a l_int32 (gmin).
 * Arg #5 is expected to be a l_int32 (gmax).
 * Arg #6 is expected to be a l_int32 (bmin).
 * Arg #7 is expected to be a l_int32 (bmax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaskOverColorRange(lua_State *L)
{
    LL_FUNC("MaskOverColorRange");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 rmin = ll_check_l_int32(_fun, L, 2);
    l_int32 rmax = ll_check_l_int32(_fun, L, 3);
    l_int32 gmin = ll_check_l_int32(_fun, L, 4);
    l_int32 gmax = ll_check_l_int32(_fun, L, 5);
    l_int32 bmin = ll_check_l_int32(_fun, L, 6);
    l_int32 bmax = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixMaskOverColorRange(pixs, rmin, rmax, gmin, gmax, bmin, bmax);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #6 is expected to be a l_int32 (mincount).
 * Arg #7 is expected to be a l_int32 (smoothx).
 * Arg #8 is expected to be a l_int32 (smoothy).
 * Arg #9 is expected to be a l_float32 (scorefract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaskedThreshOnBackgroundNorm(lua_State *L)
{
    LL_FUNC("MaskedThreshOnBackgroundNorm");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    l_int32 thresh = ll_check_l_int32(_fun, L, 5);
    l_int32 mincount = ll_check_l_int32(_fun, L, 6);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 7);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 8);
    l_float32 scorefract = ll_check_l_float32(_fun, L, 9);
    l_int32 othresh = 0;
    Pix* pix = pixMaskedThreshOnBackgroundNorm(pixs, pixim, sx, sy, thresh, mincount, smoothx, smoothy, scorefract, &othresh);
    ll_push_Pix(_fun, L, pix);
    ll_push_l_int32(_fun, L, othresh);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaxDynamicRange(lua_State *L)
{
    LL_FUNC("MaxDynamicRange");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixMaxDynamicRange(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MaxDynamicRangeRGB(lua_State *L)
{
    LL_FUNC("MaxDynamicRangeRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixMaxDynamicRangeRGB(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a Pix* (pixma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MeanInRectangle(lua_State *L)
{
    LL_FUNC("MeanInRectangle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Pix *pixma = ll_check_Pix(_fun, L, 3);
    l_float32 val = 0;
    if (pixMeanInRectangle(pixs, box, pixma, &val))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, val);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 DPix * on the Lua stack
 */
static int
MeanSquareAccum(lua_State *L)
{
    LL_FUNC("MeanSquareAccum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    DPix *dpix = pixMeanSquareAccum(pixs);
    return ll_push_DPix(_fun, L, dpix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (side).
 * Arg #3 is expected to be a l_int32 (minjump).
 * Arg #4 is expected to be a l_int32 (minreversal).
 * Arg #8 is expected to be a string (debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MeasureEdgeSmoothness(lua_State *L)
{
    LL_FUNC("MeasureEdgeSmoothness");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 side = ll_check_l_int32(_fun, L, 2);
    l_int32 minjump = ll_check_l_int32(_fun, L, 3);
    l_int32 minreversal = ll_check_l_int32(_fun, L, 4);
    l_float32 jpl = 0;
    l_float32 jspl = 0;
    l_float32 rpl = 0;
    const char *debugfile = ll_check_string(_fun, L, 8);
    if (pixMeasureEdgeSmoothness(pixs, side, minjump, minreversal, &jpl, &jspl, &rpl, debugfile))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, jpl);
    ll_push_l_float32(_fun, L, jspl);
    ll_push_l_float32(_fun, L, rpl);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MeasureSaturation(lua_State *L)
{
    LL_FUNC("MeasureSaturation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_float32 sat = 0;
    if (pixMeasureSaturation(pixs, factor, &sat))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, sat);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sigbits).
 * Arg #3 is expected to be a l_int32 (subsample).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 * on the Lua stack
 */
static int
MedianCutHisto(lua_State *L)
{
    LL_FUNC("MedianCutHisto");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sigbits = ll_check_l_int32(_fun, L, 2);
    l_int32 subsample = ll_check_l_int32(_fun, L, 3);
    l_int32 size = 1 << (3 * sigbits);
    l_int32 *result = pixMedianCutHisto(pixs, sigbits, subsample);
    return ll_push_Iarray(_fun, L, result, size);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MedianCutQuant(lua_State *L)
{
    LL_FUNC("MedianCutQuant");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixMedianCutQuant(pixs, ditherflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ditherflag).
 * Arg #3 is expected to be a l_int32 (outdepth).
 * Arg #4 is expected to be a l_int32 (maxcolors).
 * Arg #5 is expected to be a l_int32 (sigbits).
 * Arg #6 is expected to be a l_int32 (maxsub).
 * Arg #7 is expected to be a l_int32 (checkbw).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MedianCutQuantGeneral(lua_State *L)
{
    LL_FUNC("MedianCutQuantGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 2);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 3);
    l_int32 maxcolors = ll_check_l_int32(_fun, L, 4);
    l_int32 sigbits = ll_check_l_int32(_fun, L, 5);
    l_int32 maxsub = ll_check_l_int32(_fun, L, 6);
    l_int32 checkbw = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixMedianCutQuantGeneral(pixs, ditherflag, outdepth, maxcolors, sigbits, maxsub, checkbw);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (ncolor).
 * Arg #3 is expected to be a l_int32 (ngray).
 * Arg #4 is expected to be a l_int32 (darkthresh).
 * Arg #5 is expected to be a l_int32 (lightthresh).
 * Arg #6 is expected to be a l_int32 (diffthresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MedianCutQuantMixed(lua_State *L)
{
    LL_FUNC("MedianCutQuantMixed");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 ncolor = ll_check_l_int32(_fun, L, 2);
    l_int32 ngray = ll_check_l_int32(_fun, L, 3);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 5);
    l_int32 diffthresh = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixMedianCutQuantMixed(pixs, ncolor, ngray, darkthresh, lightthresh, diffthresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wf).
 * Arg #3 is expected to be a l_int32 (hf).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MedianFilter(lua_State *L)
{
    LL_FUNC("MedianFilter");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wf = ll_check_l_int32(_fun, L, 2);
    l_int32 hf = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMedianFilter(pixs, wf, hf);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (dist).
 * Arg #7 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MinMaxNearLine(lua_State *L)
{
    LL_FUNC("MinMaxNearLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 dist = ll_check_l_int32(_fun, L, 6);
    l_int32 direction = ll_check_l_int32(_fun, L, 7);
    Numa *namin = nullptr;
    Numa *namax = nullptr;
    l_float32 minave = 0;
    l_float32 maxave = 0;
    if (pixMinMaxNearLine(pixs, x1, y1, x2, y2, dist, direction, &namin, &namax, &minave, &maxave))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, namin);
    ll_push_Numa(_fun, L, namax);
    ll_push_l_float32(_fun, L, minave);
    ll_push_l_float32(_fun, L, maxave);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sx).
 * Arg #3 is expected to be a l_int32 (sy).
 * Arg #4 is expected to be a l_int32 (mindiff).
 * Arg #5 is expected to be a l_int32 (smoothx).
 * Arg #6 is expected to be a l_int32 (smoothy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MinMaxTiles(lua_State *L)
{
    LL_FUNC("MinMaxTiles");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sx = ll_check_l_int32(_fun, L, 2);
    l_int32 sy = ll_check_l_int32(_fun, L, 3);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 4);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 5);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 6);
    Pix *pixmin = nullptr;
    Pix *pixmax = nullptr;
    if (pixMinMaxTiles(pixs, sx, sy, mindiff, smoothx, smoothy, &pixmin, &pixmax))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixmin);
    ll_push_Pix(_fun, L, pixmax);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MinOrMax(lua_State *L)
{
    LL_FUNC("MinOrMax");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixMinOrMax(pixd, pixs1, pixs2, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MirrorDetect(lua_State *L)
{
    LL_FUNC("MirrorDetect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 conf = 0;
    if (pixMirrorDetect(pixs, &conf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MirrorDetectDwa(lua_State *L)
{
    LL_FUNC("MirrorDetectDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 conf = 0;
    if (pixMirrorDetectDwa(pixs, &conf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Create a mirrored tiling of the Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
MirroredTiling(lua_State *L)
{
    LL_FUNC("MirroredTiling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMirroredTiling(pixs, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ModifyBrightness(lua_State *L)
{
    LL_FUNC("ModifyBrightness");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixModifyBrightness(pixd, pixs, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ModifyHue(lua_State *L)
{
    LL_FUNC("ModifyHue");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixModifyHue(pixd, pixs, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ModifySaturation(lua_State *L)
{
    LL_FUNC("ModifySaturation");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixModifySaturation(pixd, pixs, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (width).
 * Arg #3 is expected to be a l_float32 (targetw).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ModifyStrokeWidth(lua_State *L)
{
    LL_FUNC("ModifyStrokeWidth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 width = ll_check_l_float32(_fun, L, 2);
    l_float32 targetw = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixModifyStrokeWidth(pixs, width, targetw);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphCompSequence(lua_State *L)
{
    LL_FUNC("MorphCompSequence");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMorphCompSequence(pixs, sequence, dispsep);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphCompSequenceDwa(lua_State *L)
{
    LL_FUNC("MorphCompSequenceDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMorphCompSequenceDwa(pixs, sequence, dispsep);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * Arg #4 is expected to be a l_int32 (smoothing).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphGradient(lua_State *L)
{
    LL_FUNC("MorphGradient");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    l_int32 smoothing = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixMorphGradient(pixs, hsize, vsize, smoothing);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphSequence(lua_State *L)
{
    LL_FUNC("MorphSequence");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMorphSequence(pixs, sequence, dispsep);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (minw).
 * Arg #5 is expected to be a l_int32 (minh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphSequenceByComponent(lua_State *L)
{
    LL_FUNC("MorphSequenceByComponent");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 minw = ll_check_l_int32(_fun, L, 4);
    l_int32 minh = ll_check_l_int32(_fun, L, 5);
    Boxa *boxa = nullptr;
    if (pixMorphSequenceByComponent(pixs, sequence, connectivity, minw, minh, &boxa))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a string (sequence).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * Arg #5 is expected to be a l_int32 (minw).
 * Arg #6 is expected to be a l_int32 (minh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphSequenceByRegion(lua_State *L)
{
    LL_FUNC("MorphSequenceByRegion");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    const char *sequence = ll_check_string(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    l_int32 minw = ll_check_l_int32(_fun, L, 5);
    l_int32 minh = ll_check_l_int32(_fun, L, 6);
    Boxa *boxa = nullptr;
    if (pixMorphSequenceByRegion(pixs, pixm, sequence, connectivity, minw, minh, &boxa))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (sequence).
 * Arg #3 is expected to be a l_int32 (dispsep).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphSequenceDwa(lua_State *L)
{
    LL_FUNC("MorphSequenceDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *sequence = ll_check_string(_fun, L, 2);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixMorphSequenceDwa(pixs, sequence, dispsep);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a string (sequence).
 * Arg #4 is expected to be a l_int32 (dispsep).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphSequenceMasked(lua_State *L)
{
    LL_FUNC("MorphSequenceMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    const char *sequence = ll_check_string(_fun, L, 3);
    l_int32 dispsep = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixMorphSequenceMasked(pixs, pixm, sequence, dispsep);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (roff).
 * Arg #3 is expected to be a l_float32 (goff).
 * Arg #4 is expected to be a l_float32 (boff).
 * Arg #5 is expected to be a l_float32 (delta).
 * Arg #6 is expected to be a l_int32 (nincr).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MosaicColorShiftRGB(lua_State *L)
{
    LL_FUNC("MosaicColorShiftRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 roff = ll_check_l_float32(_fun, L, 2);
    l_float32 goff = ll_check_l_float32(_fun, L, 3);
    l_float32 boff = ll_check_l_float32(_fun, L, 4);
    l_float32 delta = ll_check_l_float32(_fun, L, 5);
    l_int32 nincr = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixMosaicColorShiftRGB(pixs, roff, goff, boff, delta, nincr);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (factor).
 * Arg #3 is expected to be a l_uint32 (offset).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MultConstAccumulate(lua_State *L)
{
    LL_FUNC("MultConstAccumulate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 factor = ll_check_l_float32(_fun, L, 2);
    l_uint32 offset = ll_check_l_uint32(_fun, L, 3);
    l_int32 result = pixMultConstAccumulate(pixs, factor, offset);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rfact).
 * Arg #3 is expected to be a l_float32 (gfact).
 * Arg #4 is expected to be a l_float32 (bfact).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MultConstantColor(lua_State *L)
{
    LL_FUNC("MultConstantColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rfact = ll_check_l_float32(_fun, L, 2);
    l_float32 gfact = ll_check_l_float32(_fun, L, 3);
    l_float32 bfact = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixMultConstantColor(pixs, rfact, gfact, bfact);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
MultConstantGray(lua_State *L)
{
    LL_FUNC("MultConstantGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 val = ll_check_l_float32(_fun, L, 2);
    l_int32 result = pixMultConstantGray(pixs, val);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Kernel* (kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MultMatrixColor(lua_State *L)
{
    LL_FUNC("MultMatrixColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Kernel *kel = ll_check_Kernel(_fun, L, 2);
    Pix *pix = pixMultMatrixColor(pixs, kel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_uint32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MultiplyByColor(lua_State *L)
{
    LL_FUNC("MultiplyByColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_uint32 color = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixMultiplyByColor(pixd, pixs, box, color);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
NumColors(lua_State *L)
{
    LL_FUNC("NumColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 ncolors = 0;
    if (pixNumColors(pixs, factor, &ncolors))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncolors);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (darkthresh).
 * Arg #3 is expected to be a l_int32 (lightthresh).
 * Arg #4 is expected to be a l_float32 (minfract).
 * Arg #5 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
NumSignificantGrayColors(lua_State *L)
{
    LL_FUNC("NumSignificantGrayColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 darkthresh = ll_check_l_int32(_fun, L, 2);
    l_int32 lightthresh = ll_check_l_int32(_fun, L, 3);
    l_float32 minfract = ll_check_l_float32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    l_int32 ncolors = 0;
    if (pixNumSignificantGrayColors(pixs, darkthresh, lightthresh, minfract, factor, &ncolors))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncolors);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (level).
 * Arg #3 is expected to be a l_int32 (mincount).
 * Arg #4 is expected to be a l_float32 (minfract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
NumberOccupiedOctcubes(lua_State *L)
{
    LL_FUNC("NumberOccupiedOctcubes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    l_int32 mincount = ll_check_l_int32(_fun, L, 3);
    l_float32 minfract = ll_check_l_float32(_fun, L, 4);
    l_int32 ncolors = 0;
    if (pixNumberOccupiedOctcubes(pix, level, mincount, minfract, &ncolors))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncolors);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
OctcubeHistogram(lua_State *L)
{
    LL_FUNC("OctcubeHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    l_int32 ncolors = 0;
    if (pixOctcubeHistogram(pixs, level, &ncolors))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, ncolors);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a PixColormap* (cmap).
 * Arg #3 is expected to be a l_int32 (mindepth).
 * Arg #4 is expected to be a l_int32 (level).
 * Arg #5 is expected to be a l_int32 (metric).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctcubeQuantFromCmap(lua_State *L)
{
    LL_FUNC("OctcubeQuantFromCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 2);
    l_int32 mindepth = ll_check_l_int32(_fun, L, 3);
    l_int32 level = ll_check_l_int32(_fun, L, 4);
    l_int32 metric = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixOctcubeQuantFromCmap(pixs, cmap, mindepth, level, metric);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (depth).
 * Arg #3 is expected to be a l_int32 (graylevels).
 * Arg #4 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctcubeQuantMixedWithGray(lua_State *L)
{
    LL_FUNC("OctcubeQuantMixedWithGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 depth = ll_check_l_int32(_fun, L, 2);
    l_int32 graylevels = ll_check_l_int32(_fun, L, 3);
    l_int32 delta = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOctcubeQuantMixedWithGray(pixs, depth, graylevels, delta);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (colors).
 * Arg #3 is expected to be a l_int32 (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctreeColorQuant(lua_State *L)
{
    LL_FUNC("OctreeColorQuant");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 colors = ll_check_l_int32(_fun, L, 2);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixOctreeColorQuant(pixs, colors, ditherflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (colors).
 * Arg #3 is expected to be a l_int32 (ditherflag).
 * Arg #4 is expected to be a l_float32 (validthresh).
 * Arg #5 is expected to be a l_float32 (colorthresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctreeColorQuantGeneral(lua_State *L)
{
    LL_FUNC("OctreeColorQuantGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 colors = ll_check_l_int32(_fun, L, 2);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 3);
    l_float32 validthresh = ll_check_l_float32(_fun, L, 4);
    l_float32 colorthresh = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixOctreeColorQuantGeneral(pixs, colors, ditherflag, validthresh, colorthresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * Arg #3 is expected to be a l_int32 (ditherflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctreeQuantByPopulation(lua_State *L)
{
    LL_FUNC("OctreeQuantByPopulation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    l_int32 ditherflag = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixOctreeQuantByPopulation(pixs, level, ditherflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (maxcolors).
 * Arg #3 is expected to be a l_int32 (subsample).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OctreeQuantNumColors(lua_State *L)
{
    LL_FUNC("OctreeQuantNumColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxcolors = ll_check_l_int32(_fun, L, 2);
    l_int32 subsample = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixOctreeQuantNumColors(pixs, maxcolors, subsample);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Open(lua_State *L)
{
    LL_FUNC("Open");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixOpen(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenBrick(lua_State *L)
{
    LL_FUNC("OpenBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOpenBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenBrickDwa(lua_State *L)
{
    LL_FUNC("OpenBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOpenBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenCompBrick(lua_State *L)
{
    LL_FUNC("OpenCompBrick");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOpenCompBrick(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenCompBrickDwa(lua_State *L)
{
    LL_FUNC("OpenCompBrickDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOpenCompBrickDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hsize).
 * Arg #4 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenCompBrickExtendDwa(lua_State *L)
{
    LL_FUNC("OpenCompBrickExtendDwa");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hsize = ll_check_l_int32(_fun, L, 3);
    l_int32 vsize = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixOpenCompBrickExtendDwa(pixd, pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenGeneralized(lua_State *L)
{
    LL_FUNC("OpenGeneralized");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Sel *sel = ll_check_Sel(_fun, L, 3);
    Pix *pix = pixOpenGeneralized(pixd, pixs, sel);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenGray(lua_State *L)
{
    LL_FUNC("OpenGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixOpenGray(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OpenGray3(lua_State *L)
{
    LL_FUNC("OpenGray3");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixOpenGray3(pixs, hsize, vsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (minupconf).
 * Arg #3 is expected to be a l_float32 (minratio).
 * Arg #4 is expected to be a l_int32 (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 l_float32 (%upconf, %leftconf) and l_int32 (%rotation) on the Lua stack
 */
static int
OrientCorrect(lua_State *L)
{
    LL_FUNC("OrientCorrect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 minupconf = ll_check_l_float32(_fun, L, 2);
    l_float32 minratio = ll_check_l_float32(_fun, L, 3);
    l_int32 debug = ll_check_boolean_default(_fun, L, 4, FALSE);
    l_float32 upconf = 0;
    l_float32 leftconf = 0;
    l_int32 rotation = 0;
    if (pixOrientCorrect(pixs, minupconf, minratio, &upconf, &leftconf, &rotation, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, upconf);
    ll_push_l_float32(_fun, L, leftconf);
    ll_push_l_int32(_fun, L, rotation);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
OrientDetect(lua_State *L)
{
    LL_FUNC("OrientDetect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 upconf = 0;
    l_float32 leftconf = 0;
    if (pixOrientDetect(pixs, &upconf, &leftconf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, upconf);
    ll_push_l_float32(_fun, L, leftconf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
OrientDetectDwa(lua_State *L)
{
    LL_FUNC("OrientDetectDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 upconf = 0;
    l_float32 leftconf = 0;
    if (pixOrientDetectDwa(pixs, &upconf, &leftconf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, upconf);
    ll_push_l_float32(_fun, L, leftconf);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sx).
 * Arg #3 is expected to be a l_int32 (sy).
 * Arg #4 is expected to be a l_int32 (smoothx).
 * Arg #5 is expected to be a l_int32 (smoothy).
 * Arg #6 is expected to be a l_float32 (scorefract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
OtsuAdaptiveThreshold(lua_State *L)
{
    LL_FUNC("OtsuAdaptiveThreshold");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sx = ll_check_l_int32(_fun, L, 2);
    l_int32 sy = ll_check_l_int32(_fun, L, 3);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 4);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 5);
    l_float32 scorefract = ll_check_l_float32(_fun, L, 6);
    Pix *pixth = nullptr;
    Pix *pixd = nullptr;
    if (pixOtsuAdaptiveThreshold(pixs, sx, sy, smoothx, smoothy, scorefract, &pixth, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixth);
    ll_push_Pix(_fun, L, pixd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixim).
 * Arg #3 is expected to be a l_int32 (sx).
 * Arg #4 is expected to be a l_int32 (sy).
 * Arg #5 is expected to be a l_int32 (thresh).
 * Arg #6 is expected to be a l_int32 (mincount).
 * Arg #7 is expected to be a l_int32 (bgval).
 * Arg #8 is expected to be a l_int32 (smoothx).
 * Arg #9 is expected to be a l_int32 (smoothy).
 * Arg #10 is expected to be a l_float32 (scorefract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
OtsuThreshOnBackgroundNorm(lua_State *L)
{
    LL_FUNC("OtsuThreshOnBackgroundNorm");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixim = ll_check_Pix(_fun, L, 2);
    l_int32 sx = ll_check_l_int32(_fun, L, 3);
    l_int32 sy = ll_check_l_int32(_fun, L, 4);
    l_int32 thresh = ll_check_l_int32(_fun, L, 5);
    l_int32 mincount = ll_check_l_int32(_fun, L, 6);
    l_int32 bgval = ll_check_l_int32(_fun, L, 7);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 8);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 9);
    l_float32 scorefract = ll_check_l_float32(_fun, L, 10);
    l_int32 othresh = 0;
    Pix *pix = pixOtsuThreshOnBackgroundNorm(pixs, pixim, sx, sy, thresh, mincount, bgval, smoothx, smoothy, scorefract, &othresh);
    ll_push_Pix(_fun, L, pix);
    ll_push_l_int32(_fun, L, othresh);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
PadToCenterCentroid(lua_State *L)
{
    LL_FUNC("PadToCenterCentroid");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixPadToCenterCentroid(pixs, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
PaintBoxa(lua_State *L)
{
    LL_FUNC("PaintBoxa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixPaintBoxa(pixs, boxa, val);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
PaintBoxaRandom(lua_State *L)
{
    LL_FUNC("PaintBoxaRandom");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    Pix *pix = pixPaintBoxaRandom(pixs, boxa);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Paint %val through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset %x and %y.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* with 1 bit/pixel (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a string with a search direction name (searchdir).
 * Arg #6 is expected to be a l_int32 (mindist).
 * Arg #7 is expected to be a l_int32 (tilesize).
 * Arg #8 is expected to be a l_int32 (ntiles).
 * Arg #9 is optional and, if given, expected to be a l_int32 (distblend).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
PaintSelfThroughMask(lua_State *L)
{
    LL_FUNC("PaintSelfThroughMask");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 searchdir = ll_check_searchdir(_fun, L, 5, L_BOTH_DIRECTIONS);
    l_int32 mindist = ll_check_l_int32(_fun, L, 6);
    l_int32 tilesize = ll_check_l_int32(_fun, L, 7);
    l_int32 ntiles = ll_check_l_int32(_fun, L, 8);
    l_int32 distblend = ll_check_l_int32_default(_fun, L, 9, 0);
    return ll_push_boolean(_fun, L, 0 == pixPaintSelfThroughMask(pixd, pixm, x, y, searchdir, mindist, tilesize, ntiles, distblend));
}

/**
 * \brief Paint value (%val) through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset (%x, %y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* with (pixm) 1 bit/pixel.
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
PaintThroughMask(lua_State *L)
{
    LL_FUNC("PaintThroughMask");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_uint32 val = ll_check_l_uint32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixPaintThroughMask(pixd, pixm, x, y, val));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (pta).
 * Arg #3 is expected to be a l_int32 (outformat).
 * Arg #4 is expected to be a string (title).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
PlotAlongPta(lua_State *L)
{
    LL_FUNC("PlotAlongPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_int32 outformat = ll_check_l_int32(_fun, L, 3);
    const char *title = ll_check_string(_fun, L, 4);
    l_int32 result = pixPlotAlongPta(pixs, pta, outformat, title);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_float32 (cropfract).
 * Arg #4 is expected to be a l_int32 (outres).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Prepare1bpp(lua_State *L)
{
    LL_FUNC("Prepare1bpp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_float32 cropfract = ll_check_l_float32(_fun, L, 3);
    l_int32 outres = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixPrepare1bpp(pixs, box, cropfract, outres);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Print info about a Pix* (%pix) to a Lua stream (%stream).
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream io handle (stream).
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
PrintStreamInfo(lua_State *L)
{
    LL_FUNC("PrintStreamInfo");
    static char str[64];
    Pix *pix= ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    snprintf(str, sizeof(str), "%p\n", reinterpret_cast<void *>(pix));
    return ll_push_boolean(_fun, L, 0 == pixPrintStreamInfo(stream->f, pix, str));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (format).
 * Arg #3 is expected to be a l_int32 (method).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sarray * on the Lua stack
 */
static int
ProcessBarcodes(lua_State *L)
{
    LL_FUNC("ProcessBarcodes");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 format = ll_check_l_int32(_fun, L, 2);
    l_int32 method = ll_check_l_int32(_fun, L, 3);
    Sarray *saw = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    if (pixProcessBarcodes(pixs, format, method, &saw, debugflag))
        return ll_push_nil(L);
    ll_push_Sarray(_fun, L, saw);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Projective(lua_State *L)
{
    LL_FUNC("Projective");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixProjective(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectiveColor(lua_State *L)
{
    LL_FUNC("ProjectiveColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 3);
    if (pixProjectiveColor(pixs, &vc, colorval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectiveGray(lua_State *L)
{
    LL_FUNC("ProjectiveGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 3);
    if (pixProjectiveGray(pixs, &vc, grayval))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectivePta(lua_State *L)
{
    LL_FUNC("ProjectivePta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixProjectivePta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectivePtaColor(lua_State *L)
{
    LL_FUNC("ProjectivePtaColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixProjectivePtaColor(pixs, ptad, ptas, colorval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectivePtaGray(lua_State *L)
{
    LL_FUNC("ProjectivePtaGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 4);
    Pix *pix = pixProjectivePtaGray(pixs, ptad, ptas, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a Pix* (pixg).
 * Arg #5 is expected to be a l_float32 (fract).
 * Arg #6 is expected to be a l_int32 (border).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectivePtaWithAlpha(lua_State *L)
{
    LL_FUNC("ProjectivePtaWithAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    Pix *pixg = ll_check_Pix(_fun, L, 4);
    l_float32 fract = ll_check_l_float32(_fun, L, 5);
    l_int32 border = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixProjectivePtaWithAlpha(pixs, ptad, ptas, pixg, fract, border);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectiveSampled(lua_State *L)
{
    LL_FUNC("ProjectiveSampled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 vc = 0;
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    if (pixProjectiveSampled(pixs, &vc, incolor))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, vc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pta* (ptad).
 * Arg #3 is expected to be a Pta* (ptas).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ProjectiveSampledPta(lua_State *L)
{
    LL_FUNC("ProjectiveSampledPta");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pta *ptad = ll_check_Pta(_fun, L, 2);
    Pta *ptas = ll_check_Pta(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixProjectiveSampledPta(pixs, ptad, ptas, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (vmaxt).
 * Arg #4 is expected to be a l_int32 (vmaxb).
 * Arg #5 is expected to be a l_int32 (operation).
 * Arg #6 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
QuadraticVShear(lua_State *L)
{
    LL_FUNC("QuadraticVShear");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 vmaxt = ll_check_l_int32(_fun, L, 3);
    l_int32 vmaxb = ll_check_l_int32(_fun, L, 4);
    l_int32 operation = ll_check_l_int32(_fun, L, 5);
    l_int32 incolor = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixQuadraticVShear(pixs, dir, vmaxt, vmaxb, operation, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (vmaxt).
 * Arg #4 is expected to be a l_int32 (vmaxb).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
QuadraticVShearLI(lua_State *L)
{
    LL_FUNC("QuadraticVShearLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 vmaxt = ll_check_l_int32(_fun, L, 3);
    l_int32 vmaxb = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixQuadraticVShearLI(pixs, dir, vmaxt, vmaxb, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (vmaxt).
 * Arg #4 is expected to be a l_int32 (vmaxb).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
QuadraticVShearSampled(lua_State *L)
{
    LL_FUNC("QuadraticVShearSampled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 vmaxt = ll_check_l_int32(_fun, L, 3);
    l_int32 vmaxb = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixQuadraticVShearSampled(pixs, dir, vmaxt, vmaxb, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nlevels).
 * Arg #3 is expected to be a Pix* (pix_ma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
QuadtreeMean(lua_State *L)
{
    LL_FUNC("QuadtreeMean");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 2);
    Pix *pix_ma = ll_check_Pix(_fun, L, 3);
    FPixa *fpixa = nullptr;
    if (pixQuadtreeMean(pixs, nlevels, pix_ma, &fpixa))
        return ll_push_nil(L);
    ll_push_FPixa(_fun, L, fpixa);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nlevels).
 * Arg #3 is expected to be a Pix* (pix_ma).
 * Arg #4 is expected to be a DPix* (dpix_msa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
QuadtreeVariance(lua_State *L)
{
    LL_FUNC("QuadtreeVariance");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 2);
    Pix *pix_ma = ll_check_Pix(_fun, L, 3);
    DPix *dpix_msa = ll_check_DPix(_fun, L, 4);
    FPixa *fpixa_v = nullptr;
    FPixa *fpixa_rv = nullptr;
    if (pixQuadtreeVariance(pixs, nlevels, pix_ma, dpix_msa, &fpixa_v, &fpixa_rv))
        return ll_push_nil(L);
    ll_push_FPixa(_fun, L, fpixa_v);
    ll_push_FPixa(_fun, L, fpixa_rv);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a PixColormap* (cmap).
 * Arg #3 is expected to be a l_int32 (mindepth).
 * Arg #4 is expected to be a l_int32 (level).
 * Arg #5 is expected to be a l_int32 (metric).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
QuantFromCmap(lua_State *L)
{
    LL_FUNC("QuantFromCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    PixColormap *cmap = ll_check_PixColormap(_fun, L, 2);
    l_int32 mindepth = ll_check_l_int32(_fun, L, 3);
    l_int32 level = ll_check_l_int32(_fun, L, 4);
    l_int32 metric = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixQuantFromCmap(pixs, cmap, mindepth, level, metric);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a colormapped Pix* (%pixs) to a binary 1bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
QuantizeIfFewColors(lua_State *L)
{
    LL_FUNC("QuantizeIfFewColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 maxcolors = ll_check_l_int32_default(_fun, L, 2, 256);
    l_int32 mingraycolors = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 octlevel = ll_check_l_int32_default(_fun, L, 3, 3);
    Pix *pixd = nullptr;
    if (pixQuantizeIfFewColors(pixs, maxcolors, mingraycolors, octlevel, &pixd))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (xmag).
 * Arg #3 is expected to be a l_float32 (ymag).
 * Arg #4 is expected to be a l_float32 (xfreq).
 * Arg #5 is expected to be a l_float32 (yfreq).
 * Arg #6 is expected to be a l_int32 (nx).
 * Arg #7 is expected to be a l_int32 (ny).
 * Arg #8 is expected to be a l_uint32 (seed).
 * Arg #9 is expected to be a l_int32 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RandomHarmonicWarp(lua_State *L)
{
    LL_FUNC("RandomHarmonicWarp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 xmag = ll_check_l_float32(_fun, L, 2);
    l_float32 ymag = ll_check_l_float32(_fun, L, 3);
    l_float32 xfreq = ll_check_l_float32(_fun, L, 4);
    l_float32 yfreq = ll_check_l_float32(_fun, L, 5);
    l_int32 nx = ll_check_l_int32(_fun, L, 6);
    l_int32 ny = ll_check_l_int32(_fun, L, 7);
    l_uint32 seed = ll_check_l_uint32(_fun, L, 8);
    l_int32 grayval = ll_check_l_int32(_fun, L, 9);
    Pix *pix = pixRandomHarmonicWarp(pixs, xmag, ymag, xfreq, yfreq, nx, ny, seed, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get a binned color for Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string describing the select color type (type).
 * Arg #3 is expected to be a string describing the direction (direction).
 * Arg #4 is expected to be a l_int32 (size).
 * Arg #5 is expected to be a l_int32 (nbins).
 * </pre>
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
RankBinByStrip(lua_State *L)
{
    LL_FUNC("RankBinByStrip");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_select_color(_fun, L, 2, L_SELECT_RED);
    l_int32 direction = ll_check_direction(_fun, L, 3, L_SCAN_HORIZONTAL);
    l_int32 size = ll_check_l_int32_default(_fun, L, 4, 200);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 5, 2);
    Pix *pixd = pixRankBinByStrip(pixs, direction, size, nbins, type);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankColumnTransform(lua_State *L)
{
    LL_FUNC("RankColumnTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixRankColumnTransform(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wf).
 * Arg #3 is expected to be a l_int32 (hf).
 * Arg #4 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankFilter(lua_State *L)
{
    LL_FUNC("RankFilter");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wf = ll_check_l_int32(_fun, L, 2);
    l_int32 hf = ll_check_l_int32(_fun, L, 3);
    l_float32 rank = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixRankFilter(pixs, wf, hf, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wf).
 * Arg #3 is expected to be a l_int32 (hf).
 * Arg #4 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankFilterGray(lua_State *L)
{
    LL_FUNC("RankFilterGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wf = ll_check_l_int32(_fun, L, 2);
    l_int32 hf = ll_check_l_int32(_fun, L, 3);
    l_float32 rank = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixRankFilterGray(pixs, wf, hf, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wf).
 * Arg #3 is expected to be a l_int32 (hf).
 * Arg #4 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankFilterRGB(lua_State *L)
{
    LL_FUNC("RankFilterRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wf = ll_check_l_int32(_fun, L, 2);
    l_int32 hf = ll_check_l_int32(_fun, L, 3);
    l_float32 rank = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixRankFilterRGB(pixs, wf, hf, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wf).
 * Arg #3 is expected to be a l_int32 (hf).
 * Arg #4 is expected to be a l_float32 (rank).
 * Arg #5 is expected to be a l_float32 (scalefactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankFilterWithScaling(lua_State *L)
{
    LL_FUNC("RankFilterWithScaling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wf = ll_check_l_int32(_fun, L, 2);
    l_int32 hf = ll_check_l_int32(_fun, L, 3);
    l_float32 rank = ll_check_l_float32(_fun, L, 4);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixRankFilterWithScaling(pixs, wf, hf, rank, scalefactor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a Pix* (pix3).
 * Arg #4 is expected to be a Pix* (pix4).
 * Arg #5 is expected to be a l_float32 (delx).
 * Arg #6 is expected to be a l_float32 (dely).
 * Arg #7 is expected to be a l_int32 (maxdiffw).
 * Arg #8 is expected to be a l_int32 (maxdiffh).
 * Arg #9 is expected to be a l_int32 (area1).
 * Arg #10 is expected to be a l_int32 (area3).
 * Arg #11 is expected to be a l_float32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RankHaustest(lua_State *L)
{
    LL_FUNC("RankHaustest");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    Pix *pix3 = ll_check_Pix(_fun, L, 3);
    Pix *pix4 = ll_check_Pix(_fun, L, 4);
    l_float32 delx = ll_check_l_float32(_fun, L, 5);
    l_float32 dely = ll_check_l_float32(_fun, L, 6);
    l_int32 maxdiffw = ll_check_l_int32(_fun, L, 7);
    l_int32 maxdiffh = ll_check_l_int32(_fun, L, 8);
    l_int32 area1 = ll_check_l_int32(_fun, L, 9);
    l_int32 area3 = ll_check_l_int32(_fun, L, 10);
    l_float32 rank = ll_check_l_float32(_fun, L, 11);
    l_int32 tab8 = 0;
    if (pixRankHaustest(pix1, pix2, pix3, pix4, delx, dely, maxdiffw, maxdiffh, area1, area3, rank, &tab8))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, tab8);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RankRowTransform(lua_State *L)
{
    LL_FUNC("RankRowTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixRankRowTransform(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (dx).
 * Arg #3 is expected to be a l_int32 (dy).
 * Arg #4 is expected to be a l_int32 (dw).
 * Arg #5 is expected to be a l_int32 (dh).
 * Arg #6 is expected to be a l_int32 (op).
 * Arg #7 is expected to be a Pix* (pixs).
 * Arg #8 is expected to be a l_int32 (sx).
 * Arg #9 is expected to be a l_int32 (sy).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Rasterop(lua_State *L)
{
    LL_FUNC("Rasterop");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 dx = ll_check_l_int32(_fun, L, 2);
    l_int32 dy = ll_check_l_int32(_fun, L, 3);
    l_int32 dw = ll_check_l_int32(_fun, L, 4);
    l_int32 dh = ll_check_l_int32(_fun, L, 5);
    l_int32 op = ll_check_l_int32(_fun, L, 6);
    Pix *pixs = ll_check_Pix(_fun, L, 7);
    l_int32 sx = ll_check_l_int32(_fun, L, 8);
    l_int32 sy = ll_check_l_int32(_fun, L, 9);
    l_int32 result = pixRasterop(pixd, dx, dy, dw, dh, op, pixs, sx, sy);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RasteropFullImage(lua_State *L)
{
    LL_FUNC("RasteropFullImage");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 op = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixRasteropFullImage(pixd, pixs, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (by).
 * Arg #3 is expected to be a l_int32 (bh).
 * Arg #4 is expected to be a l_int32 (hshift).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RasteropHip(lua_State *L)
{
    LL_FUNC("RasteropHip");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 by = ll_check_l_int32(_fun, L, 2);
    l_int32 bh = ll_check_l_int32(_fun, L, 3);
    l_int32 hshift = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    l_int32 result = pixRasteropHip(pixd, by, bh, hshift, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (hshift).
 * Arg #3 is expected to be a l_int32 (vshift).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RasteropIP(lua_State *L)
{
    LL_FUNC("RasteropIP");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 hshift = ll_check_l_int32(_fun, L, 2);
    l_int32 vshift = ll_check_l_int32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixRasteropIP(pixd, hshift, vshift, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (bx).
 * Arg #3 is expected to be a l_int32 (bw).
 * Arg #4 is expected to be a l_int32 (vshift).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RasteropVip(lua_State *L)
{
    LL_FUNC("RasteropVip");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 bx = ll_check_l_int32(_fun, L, 2);
    l_int32 bw = ll_check_l_int32(_fun, L, 3);
    l_int32 vshift = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    l_int32 result = pixRasteropVip(pixd, bx, bw, vshift, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Read Pix* from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char* filename = ll_check_string(_fun, L, 1);
    Pix *pix = pixRead(filename);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (method).
 * Arg #3 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ReadBarcodeWidths(lua_State *L)
{
    LL_FUNC("ReadBarcodeWidths");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 method = ll_check_l_int32(_fun, L, 2);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 3);
    Numa *result = pixReadBarcodeWidths(pixs, method, debugflag);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (format).
 * Arg #3 is expected to be a l_int32 (method).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sarray * on the Lua stack
 */
static int
ReadBarcodes(lua_State *L)
{
    LL_FUNC("ReadBarcodes");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 format = ll_check_l_int32(_fun, L, 2);
    l_int32 method = ll_check_l_int32(_fun, L, 3);
    Sarray *saw = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    if (pixReadBarcodes(pixa, format, method, &saw, debugflag))
        return ll_push_nil(L);
    ll_push_Sarray(_fun, L, saw);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadFromMultipageTiff(lua_State *L)
{
    LL_FUNC("ReadFromMultipageTiff");
    const char *fname = ll_check_string(_fun, L, 1);
    size_t offset = 0;
    if (pixReadFromMultipageTiff(fname, &offset))
        return ll_push_nil(L);
    ll_push_size_t(_fun, L, offset);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ReadHeader(lua_State *L)
{
    LL_FUNC("ReadHeader");
    const char *filename = ll_check_string(_fun, L, 1);
    l_int32 format = 0;
    l_int32 w = 0;
    l_int32 h = 0;
    l_int32 bps = 0;
    l_int32 spp = 0;
    l_int32 iscmap = 0;
    if (pixReadHeader(filename, &format, &w, &h, &bps, &spp, &iscmap))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, format);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    ll_push_l_int32(_fun, L, bps);
    ll_push_l_int32(_fun, L, spp);
    ll_push_l_int32(_fun, L, iscmap);
    return 6;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a constl_uint8* (data).
 * Arg #2 is expected to be a size_t (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ReadHeaderMem(lua_State *L)
{
    LL_FUNC("ReadHeaderMem");
    size_t len = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &len);
    const l_uint8 *data = reinterpret_cast<const l_uint8 *>(str);
    l_int32 format = 0;
    l_int32 w = 0;
    l_int32 h = 0;
    l_int32 bps = 0;
    l_int32 spp = 0;
    l_int32 iscmap = 0;
    if (pixReadHeaderMem(data, len, &format, &w, &h, &bps, &spp, &iscmap))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, format);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    ll_push_l_int32(_fun, L, bps);
    ll_push_l_int32(_fun, L, spp);
    ll_push_l_int32(_fun, L, iscmap);
    return 6;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (index).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ReadIndexed(lua_State *L)
{
    LL_FUNC("ReadIndexed");
    l_int32 n = 0;
    Sarray *sa = ll_unpack_Sarray(_fun, L, 1, &n);
    l_int32 index = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixReadIndexed(sa, index);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * Arg #2 is expected to be a l_uint32 (reduction).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (hint).
 * Arg #5 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadJp2k(lua_State *L)
{
    LL_FUNC("ReadJp2k");
    const char *filename = ll_check_string(_fun, L, 1);
    l_uint32 reduction = ll_check_l_uint32(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 hint = ll_check_l_int32(_fun, L, 4);
    l_int32 debug = ll_check_boolean_default(_fun, L, 5, FALSE);
    Pix *pix = pixReadJp2k(filename, reduction, box, hint, debug);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * Arg #2 is expected to be a l_int32 (cmapflag).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #5 is expected to be a l_int32 (hint).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadJpeg(lua_State *L)
{
    LL_FUNC("ReadJpeg");
    const char *filename = ll_check_string(_fun, L, 1);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_int32 nwarn = 0;
    l_int32 hint = ll_check_l_int32(_fun, L, 5);
    if (pixReadJpeg(filename, cmapflag, reduction, &nwarn, hint))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nwarn);
    return 1;
}

/**
 * \brief Read a Pix* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pix *pix = pixReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemBmp(lua_State *L)
{
    LL_FUNC("ReadMemBmp");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* cdata = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemBmp(cdata, size);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemFromMultipageTiff(lua_State *L)
{
    LL_FUNC("ReadMemFromMultipageTiff");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* cdata = reinterpret_cast<const l_uint8 *>(str);
    size_t offset = 0;
    if (pixReadMemFromMultipageTiff(cdata, size, &offset))
        return ll_push_nil(L);
    ll_push_size_t(_fun, L, offset);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemGif(lua_State *L)
{
    LL_FUNC("ReadMemGif");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* cdata = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemGif(cdata, size);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * Arg #2 is expected to be a l_uint32 (reduction).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (hint).
 * Arg #5 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemJp2k(lua_State *L)
{
    LL_FUNC("ReadMemJp2k");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    l_uint32 reduction = ll_check_l_uint32(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 hint = ll_check_l_int32(_fun, L, 4);
    l_int32 debug = ll_check_boolean_default(_fun, L, 5, FALSE);
    Pix *pix = pixReadMemJp2k(data, size, reduction, box, hint, debug);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * Arg #2 is expected to be a l_int32 (cmflag).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #4 is expected to be a l_int32 (hint).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemJpeg(lua_State *L)
{
    LL_FUNC("ReadMemJpeg");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    l_int32 cmflag = ll_check_l_int32(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_int32 hint = ll_check_l_int32(_fun, L, 4);
    l_int32 nwarn = 0;
    if (pixReadMemJpeg(data, size, cmflag, reduction, &nwarn, hint))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nwarn);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemPng(lua_State *L)
{
    LL_FUNC("ReadMemPng");
    size_t filesize;
    const char *str = ll_check_lstring(_fun, L, 1, &filesize);
    const l_uint8* filedata = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemPng(filedata, filesize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemPnm(lua_State *L)
{
    LL_FUNC("ReadMemPnm");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemPnm(data, size);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemSpix(lua_State *L)
{
    LL_FUNC("ReadMemSpix");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemSpix(data, size);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a constl_uint8* (cdata).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemTiff(lua_State *L)
{
    LL_FUNC("ReadMemTiff");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* cdata = reinterpret_cast<const l_uint8 *>(str);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixReadMemTiff(cdata, size, n);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a constl_uint8* (filedata).
 * Arg #2 is expected to be a size_t (filesize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadMemWebP(lua_State *L)
{
    LL_FUNC("ReadMemWebP");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    const l_uint8* data = reinterpret_cast<const l_uint8 *>(str);
    Pix *pix = pixReadMemWebP(data, size);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Read Pix* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 hint = ll_check_hint(_fun, L, 2, 0);
    Pix *pix = pixReadStream(stream->f, hint);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamBmp(lua_State *L)
{
    LL_FUNC("ReadStreamBmp");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamBmp(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamGif(lua_State *L)
{
    LL_FUNC("ReadStreamGif");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamGif(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * Arg #2 is expected to be a l_uint32 (reduction).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (hint).
 * Arg #5 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamJp2k(lua_State *L)
{
    LL_FUNC("ReadStreamJp2k");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_uint32 reduction = ll_check_l_uint32(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 hint = ll_check_l_int32(_fun, L, 4);
    l_int32 debug = ll_check_boolean_default(_fun, L, 5, FALSE);
    Pix *pix = pixReadStreamJp2k(stream->f, reduction, box, hint, debug);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * Arg #2 is expected to be a l_int32 (cmapflag).
 * Arg #3 is expected to be a l_int32 (reduction).
 * Arg #5 is expected to be a l_int32 (hint).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamJpeg(lua_State *L)
{
    LL_FUNC("ReadStreamJpeg");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 2);
    l_int32 reduction = ll_check_l_int32(_fun, L, 3);
    l_int32 nwarn = 0;
    l_int32 hint = ll_check_l_int32(_fun, L, 5);
    if (pixReadStreamJpeg(stream->f, cmapflag, reduction, &nwarn, hint))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nwarn);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamPng(lua_State *L)
{
    LL_FUNC("ReadStreamPng");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamPng(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamPnm(lua_State *L)
{
    LL_FUNC("ReadStreamPnm");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamPnm(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamSpix(lua_State *L)
{
    LL_FUNC("ReadStreamSpix");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamSpix(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamTiff(lua_State *L)
{
    LL_FUNC("ReadStreamTiff");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixReadStreamTiff(stream->f, n);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadStreamWebP(lua_State *L)
{
    LL_FUNC("ReadStreamWebP");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pix *pix = pixReadStreamWebP(stream->f);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadTiff(lua_State *L)
{
    LL_FUNC("ReadTiff");
    const char *filename = ll_check_string(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixReadTiff(filename, n);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * Arg #2 is expected to be a l_int32 (hint).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReadWithHint(lua_State *L)
{
    LL_FUNC("ReadWithHint");
    const char *filename = ll_check_string(_fun, L, 1);
    l_int32 hint = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixReadWithHint(filename, hint);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReduceBinary2(lua_State *L)
{
    LL_FUNC("ReduceBinary2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint8 intab = 0;
    Pix *pix = pixReduceBinary2(pixs, &intab);
    ll_push_Pix(_fun, L, pix);
    ll_push_l_uint8(_fun, L, intab);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReduceRankBinary2(lua_State *L)
{
    LL_FUNC("ReduceRankBinary2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level = ll_check_l_int32(_fun, L, 2);
    l_uint8 intab = 0;
    Pix *pix = pixReduceRankBinary2(pixs, level, &intab);
    ll_push_Pix(_fun, L, pix);
    ll_push_l_uint8(_fun, L, intab);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level1).
 * Arg #3 is expected to be a l_int32 (level2).
 * Arg #4 is expected to be a l_int32 (level3).
 * Arg #5 is expected to be a l_int32 (level4).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ReduceRankBinaryCascade(lua_State *L)
{
    LL_FUNC("ReduceRankBinaryCascade");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level1 = ll_check_l_int32(_fun, L, 2);
    l_int32 level2 = ll_check_l_int32(_fun, L, 3);
    l_int32 level3 = ll_check_l_int32(_fun, L, 4);
    l_int32 level4 = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixReduceRankBinaryCascade(pixs, level1, level2, level3, level4);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RemoveAlpha(lua_State *L)
{
    LL_FUNC("RemoveAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixRemoveAlpha(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Remove border pixels (%npix) from a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (npix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
RemoveBorder(lua_State *L)
{
    LL_FUNC("RemoveBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 npix = ll_check_l_int32(_fun, L, 2);
    Pix* pixd = pixRemoveBorder(pixs, npix);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RemoveBorderConnComps(lua_State *L)
{
    LL_FUNC("RemoveBorderConnComps");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixRemoveBorderConnComps(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Remove border pixels in a Pix* to a new Pix* (general case).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (pixd) on the Lua stack
 */
static int
RemoveBorderGeneral(lua_State *L)
{
    LL_FUNC("RemoveBorderGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    Pix* pixd = pixRemoveBorderGeneral(pixs, left, right, top, bottom);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Remove border pixels resizing a Pix* (%pixs) to a new Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a l_int32 (height).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (pixd) on the Lua stack
 */
static int
RemoveBorderToSize(lua_State *L)
{
    LL_FUNC("RemoveBorderToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 height = ll_check_l_int32(_fun, L, 3);
    Pix* pix = pixRemoveBorderToSize(pixs, width, height);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RemoveColormap(lua_State *L)
{
    LL_FUNC("RemoveColormap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixRemoveColormap(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (ifnocmap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RemoveColormapGeneral(lua_State *L)
{
    LL_FUNC("RemoveColormapGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 ifnocmap = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixRemoveColormapGeneral(pixs, type, ifnocmap);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixp).
 * Arg #3 is expected to be a Pix* (pixe).
 * Arg #4 is expected to be a l_int32 (x0).
 * Arg #5 is expected to be a l_int32 (y0).
 * Arg #6 is expected to be a l_int32 (dsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RemoveMatchedPattern(lua_State *L)
{
    LL_FUNC("RemoveMatchedPattern");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixp = ll_check_Pix(_fun, L, 2);
    Pix *pixe = ll_check_Pix(_fun, L, 3);
    l_int32 x0 = ll_check_l_int32(_fun, L, 4);
    l_int32 y0 = ll_check_l_int32(_fun, L, 5);
    l_int32 dsize = ll_check_l_int32(_fun, L, 6);
    l_int32 result = pixRemoveMatchedPattern(pixs, pixp, pixe, x0, y0, dsize);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * Arg #5 is expected to be a l_int32 (bordersize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RemoveSeededComponents(lua_State *L)
{
    LL_FUNC("RemoveSeededComponents");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    l_int32 bordersize = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRemoveSeededComponents(pixd, pixs, pixm, connectivity, bordersize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RemoveUnusedColors(lua_State *L)
{
    LL_FUNC("RemoveUnusedColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 result = pixRemoveUnusedColors(pixs);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pixa* (pixa).
 * Arg #3 is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RemoveWithIndicator(lua_State *L)
{
    LL_FUNC("RemoveWithIndicator");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    Numa *na = ll_check_Numa(_fun, L, 3);
    l_int32 result = pixRemoveWithIndicator(pixs, pixa, na);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBox(lua_State *L)
{
    LL_FUNC("RenderBox");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_int32 op = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixRenderBox(pix, box, width, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBoxArb(lua_State *L)
{
    LL_FUNC("RenderBoxArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_int32 result = pixRenderBoxArb(pix, box, width, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * Arg #7 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBoxBlend(lua_State *L)
{
    LL_FUNC("RenderBoxBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_float32 fract = ll_check_l_float32(_fun, L, 7);
    l_int32 result = pixRenderBoxBlend(pix, box, width, rval, gval, bval, fract);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBoxa(lua_State *L)
{
    LL_FUNC("RenderBoxa");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_int32 op = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixRenderBoxa(pix, boxa, width, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBoxaArb(lua_State *L)
{
    LL_FUNC("RenderBoxaArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_int32 result = pixRenderBoxaArb(pix, boxa, width, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * Arg #7 is expected to be a l_float32 (fract).
 * Arg #8 is expected to be a l_int32 (removedups).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderBoxaBlend(lua_State *L)
{
    LL_FUNC("RenderBoxaBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_float32 fract = ll_check_l_float32(_fun, L, 7);
    l_int32 removedups = ll_check_l_int32(_fun, L, 8);
    l_int32 result = pixRenderBoxaBlend(pix, boxa, width, rval, gval, bval, fract, removedups);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (startval).
 * Arg #3 is expected to be a l_int32 (incr).
 * Arg #4 is expected to be a l_int32 (outdepth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RenderContours(lua_State *L)
{
    LL_FUNC("RenderContours");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 startval = ll_check_l_int32(_fun, L, 2);
    l_int32 incr = ll_check_l_int32(_fun, L, 3);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixRenderContours(pixs, startval, incr, outdepth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (nx).
 * Arg #3 is expected to be a l_int32 (ny).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_uint8 (rval).
 * Arg #6 is expected to be a l_uint8 (gval).
 * Arg #7 is expected to be a l_uint8 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderGridArb(lua_State *L)
{
    LL_FUNC("RenderGridArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 nx = ll_check_l_int32(_fun, L, 2);
    l_int32 ny = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 6);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 7);
    l_int32 result = pixRenderGridArb(pix, nx, ny, width, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBox(lua_State *L)
{
    LL_FUNC("RenderHashBox");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 op = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixRenderHashBox(pix, box, spacing, width, orient, outline, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (rval).
 * Arg #8 is expected to be a l_int32 (gval).
 * Arg #9 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBoxArb(lua_State *L)
{
    LL_FUNC("RenderHashBoxArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 rval = ll_check_l_int32(_fun, L, 7);
    l_int32 gval = ll_check_l_int32(_fun, L, 8);
    l_int32 bval = ll_check_l_int32(_fun, L, 9);
    l_int32 result = pixRenderHashBoxArb(pix, box, spacing, width, orient, outline, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (rval).
 * Arg #8 is expected to be a l_int32 (gval).
 * Arg #9 is expected to be a l_int32 (bval).
 * Arg #10 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBoxBlend(lua_State *L)
{
    LL_FUNC("RenderHashBoxBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 rval = ll_check_l_int32(_fun, L, 7);
    l_int32 gval = ll_check_l_int32(_fun, L, 8);
    l_int32 bval = ll_check_l_int32(_fun, L, 9);
    l_float32 fract = ll_check_l_float32(_fun, L, 10);
    l_int32 result = pixRenderHashBoxBlend(pix, box, spacing, width, orient, outline, rval, gval, bval, fract);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBoxa(lua_State *L)
{
    LL_FUNC("RenderHashBoxa");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 op = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixRenderHashBoxa(pix, boxa, spacing, width, orient, outline, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (rval).
 * Arg #8 is expected to be a l_int32 (gval).
 * Arg #9 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBoxaArb(lua_State *L)
{
    LL_FUNC("RenderHashBoxaArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 rval = ll_check_l_int32(_fun, L, 7);
    l_int32 gval = ll_check_l_int32(_fun, L, 8);
    l_int32 bval = ll_check_l_int32(_fun, L, 9);
    l_int32 result = pixRenderHashBoxaArb(pix, boxa, spacing, width, orient, outline, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (spacing).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (orient).
 * Arg #6 is expected to be a l_int32 (outline).
 * Arg #7 is expected to be a l_int32 (rval).
 * Arg #8 is expected to be a l_int32 (gval).
 * Arg #9 is expected to be a l_int32 (bval).
 * Arg #10 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashBoxaBlend(lua_State *L)
{
    LL_FUNC("RenderHashBoxaBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 spacing = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 orient = ll_check_l_int32(_fun, L, 5);
    l_int32 outline = ll_check_l_int32(_fun, L, 6);
    l_int32 rval = ll_check_l_int32(_fun, L, 7);
    l_int32 gval = ll_check_l_int32(_fun, L, 8);
    l_int32 bval = ll_check_l_int32(_fun, L, 9);
    l_float32 fract = ll_check_l_float32(_fun, L, 10);
    l_int32 result = pixRenderHashBoxaBlend(pix, boxa, spacing, width, orient, outline, rval, gval, bval, fract);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (spacing).
 * Arg #6 is expected to be a l_int32 (width).
 * Arg #7 is expected to be a l_int32 (orient).
 * Arg #8 is expected to be a l_int32 (outline).
 * Arg #9 is expected to be a l_int32 (rval).
 * Arg #10 is expected to be a l_int32 (gval).
 * Arg #11 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderHashMaskArb(lua_State *L)
{
    LL_FUNC("RenderHashMaskArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 spacing = ll_check_l_int32(_fun, L, 5);
    l_int32 width = ll_check_l_int32(_fun, L, 6);
    l_int32 orient = ll_check_l_int32(_fun, L, 7);
    l_int32 outline = ll_check_l_int32(_fun, L, 8);
    l_int32 rval = ll_check_l_int32(_fun, L, 9);
    l_int32 gval = ll_check_l_int32(_fun, L, 10);
    l_int32 bval = ll_check_l_int32(_fun, L, 11);
    l_int32 result = pixRenderHashMaskArb(pix, pixm, x, y, spacing, width, orient, outline, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (width).
 * Arg #7 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderLine(lua_State *L)
{
    LL_FUNC("RenderLine");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 width = ll_check_l_int32(_fun, L, 6);
    l_int32 op = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixRenderLine(pix, x1, y1, x2, y2, width, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (width).
 * Arg #7 is expected to be a l_uint8 (rval).
 * Arg #8 is expected to be a l_uint8 (gval).
 * Arg #9 is expected to be a l_uint8 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderLineArb(lua_State *L)
{
    LL_FUNC("RenderLineArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 width = ll_check_l_int32(_fun, L, 6);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 7);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 8);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 9);
    l_int32 result = pixRenderLineArb(pix, x1, y1, x2, y2, width, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (x1).
 * Arg #3 is expected to be a l_int32 (y1).
 * Arg #4 is expected to be a l_int32 (x2).
 * Arg #5 is expected to be a l_int32 (y2).
 * Arg #6 is expected to be a l_int32 (width).
 * Arg #7 is expected to be a l_uint8 (rval).
 * Arg #8 is expected to be a l_uint8 (gval).
 * Arg #9 is expected to be a l_uint8 (bval).
 * Arg #10 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderLineBlend(lua_State *L)
{
    LL_FUNC("RenderLineBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_int32 width = ll_check_l_int32(_fun, L, 6);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 7);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 8);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 9);
    l_float32 fract = ll_check_l_float32(_fun, L, 10);
    l_int32 result = pixRenderLineBlend(pix, x1, y1, x2, y2, width, rval, gval, bval, fract);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #2 is expected to be a Numa* (na).
 * Arg #3 is expected to be a l_int32 (plotloc).
 * Arg #4 is expected to be a l_int32 (linewidth).
 * Arg #5 is expected to be a l_int32 (max).
 * Arg #6 is expected to be a l_uint32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPlotFromNuma(lua_State *L)
{
    LL_FUNC("RenderPlotFromNuma");
    Pix *pix = nullptr;
    Numa *na = ll_check_Numa(_fun, L, 2);
    l_int32 plotloc = ll_check_l_int32(_fun, L, 3);
    l_int32 linewidth = ll_check_l_int32(_fun, L, 4);
    l_int32 max = ll_check_l_int32(_fun, L, 5);
    l_uint32 color = ll_check_l_uint32(_fun, L, 6);
    if (pixRenderPlotFromNuma(&pix, na, plotloc, linewidth, max, color))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #2 is expected to be a Numa* (na).
 * Arg #3 is expected to be a l_int32 (orient).
 * Arg #4 is expected to be a l_int32 (linewidth).
 * Arg #5 is expected to be a l_int32 (refpos).
 * Arg #6 is expected to be a l_int32 (max).
 * Arg #7 is expected to be a l_int32 (drawref).
 * Arg #8 is expected to be a l_uint32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPlotFromNumaGen(lua_State *L)
{
    LL_FUNC("RenderPlotFromNumaGen");
    Pix *pix = nullptr;
    Numa *na = ll_check_Numa(_fun, L, 2);
    l_int32 orient = ll_check_l_int32(_fun, L, 3);
    l_int32 linewidth = ll_check_l_int32(_fun, L, 4);
    l_int32 refpos = ll_check_l_int32(_fun, L, 5);
    l_int32 max = ll_check_l_int32(_fun, L, 6);
    l_int32 drawref = ll_check_l_int32(_fun, L, 7);
    l_uint32 color = ll_check_l_uint32(_fun, L, 8);
    if (pixRenderPlotFromNumaGen(&pix, na, orient, linewidth, refpos, max, drawref, color))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* (ptas).
 * Arg #2 is expected to be a l_int32 (width).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RenderPolygon(lua_State *L)
{
    LL_FUNC("RenderPolygon");
    Pta *ptas = ll_check_Pta(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 xmin = 0;
    l_int32 ymin = 0;
    if (pixRenderPolygon(ptas, width, &xmin, &ymin))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xmin);
    ll_push_l_int32(_fun, L, ymin);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (ptas).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_int32 (op).
 * Arg #5 is expected to be a l_int32 (closeflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPolyline(lua_State *L)
{
    LL_FUNC("RenderPolyline");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *ptas = ll_check_Pta(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_int32 op = ll_check_l_int32(_fun, L, 4);
    l_int32 closeflag = ll_check_l_int32(_fun, L, 5);
    l_int32 result = pixRenderPolyline(pix, ptas, width, op, closeflag);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (ptas).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * Arg #7 is expected to be a l_int32 (closeflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPolylineArb(lua_State *L)
{
    LL_FUNC("RenderPolylineArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *ptas = ll_check_Pta(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_int32 closeflag = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixRenderPolylineArb(pix, ptas, width, rval, gval, bval, closeflag);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (ptas).
 * Arg #3 is expected to be a l_int32 (width).
 * Arg #4 is expected to be a l_uint8 (rval).
 * Arg #5 is expected to be a l_uint8 (gval).
 * Arg #6 is expected to be a l_uint8 (bval).
 * Arg #7 is expected to be a l_float32 (fract).
 * Arg #8 is expected to be a l_int32 (closeflag).
 * Arg #9 is expected to be a l_int32 (removedups).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPolylineBlend(lua_State *L)
{
    LL_FUNC("RenderPolylineBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *ptas = ll_check_Pta(_fun, L, 2);
    l_int32 width = ll_check_l_int32(_fun, L, 3);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 5);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 6);
    l_float32 fract = ll_check_l_float32(_fun, L, 7);
    l_int32 closeflag = ll_check_l_int32(_fun, L, 8);
    l_int32 removedups = ll_check_l_int32(_fun, L, 9);
    l_int32 result = pixRenderPolylineBlend(pix, ptas, width, rval, gval, bval, fract, closeflag, removedups);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (pta).
 * Arg #3 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPta(lua_State *L)
{
    LL_FUNC("RenderPta");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_int32 op = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixRenderPta(pix, pta, op);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (pta).
 * Arg #3 is expected to be a l_uint8 (rval).
 * Arg #4 is expected to be a l_uint8 (gval).
 * Arg #5 is expected to be a l_uint8 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPtaArb(lua_State *L)
{
    LL_FUNC("RenderPtaArb");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 3);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 5);
    l_int32 result = pixRenderPtaArb(pix, pta, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Pta* (pta).
 * Arg #3 is expected to be a l_uint8 (rval).
 * Arg #4 is expected to be a l_uint8 (gval).
 * Arg #5 is expected to be a l_uint8 (bval).
 * Arg #6 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RenderPtaBlend(lua_State *L)
{
    LL_FUNC("RenderPtaBlend");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Pta *pta = ll_check_Pta(_fun, L, 2);
    l_uint8 rval = ll_check_l_uint8(_fun, L, 3);
    l_uint8 gval = ll_check_l_uint8(_fun, L, 4);
    l_uint8 bval = ll_check_l_uint8(_fun, L, 5);
    l_float32 fract = ll_check_l_float32(_fun, L, 6);
    l_int32 result = pixRenderPtaBlend(pix, pta, rval, gval, bval, fract);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * Arg #3 is expected to be a l_int32 (polyflag).
 * Arg #4 is expected to be a l_int32 (width).
 * Arg #5 is expected to be a l_int32 (closeflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RenderRandomCmapPtaa(lua_State *L)
{
    LL_FUNC("RenderRandomCmapPtaa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    l_int32 polyflag = ll_check_l_int32(_fun, L, 3);
    l_int32 width = ll_check_l_int32(_fun, L, 4);
    l_int32 closeflag = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRenderRandomCmapPtaa(pixs, ptaa, polyflag, width, closeflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Resize a Pix* image data.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ResizeImageData(lua_State *L)
{
    LL_FUNC("ResizeImageData");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixResizeImageData(pixd, pixs));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixt).
 * Arg #3 is expected to be a l_int32 (w).
 * Arg #4 is expected to be a l_int32 (h).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ResizeToMatch(lua_State *L)
{
    LL_FUNC("ResizeToMatch");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixt = ll_check_Pix(_fun, L, 2);
    l_int32 w = ll_check_l_int32(_fun, L, 3);
    l_int32 h = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixResizeToMatch(pixs, pixt, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (fract).
 * Arg #3 is expected to be a l_int32 (dir).
 * Arg #4 is expected to be a l_int32 (first).
 * Arg #5 is expected to be a l_int32 (last).
 * Arg #6 is expected to be a l_int32 (minreversal).
 * Arg #7 is expected to be a l_int32 (factor1).
 * Arg #8 is expected to be a l_int32 (factor2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
ReversalProfile(lua_State *L)
{
    LL_FUNC("ReversalProfile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = ll_check_l_float32(_fun, L, 2);
    l_int32 dir = ll_check_l_int32(_fun, L, 3);
    l_int32 first = ll_check_l_int32(_fun, L, 4);
    l_int32 last = ll_check_l_int32(_fun, L, 5);
    l_int32 minreversal = ll_check_l_int32(_fun, L, 6);
    l_int32 factor1 = ll_check_l_int32(_fun, L, 7);
    l_int32 factor2 = ll_check_l_int32(_fun, L, 8);
    Numa *result = pixReversalProfile(pixs, fract, dir, first, last, minreversal, factor1, factor2);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (incolor).
 * Arg #5 is expected to be a l_int32 (width).
 * Arg #6 is expected to be a l_int32 (height).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Rotate(lua_State *L)
{
    LL_FUNC("Rotate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    l_int32 width = ll_check_l_int32(_fun, L, 5);
    l_int32 height = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixRotate(pixs, angle, type, incolor, width, height);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Rotate180(lua_State *L)
{
    LL_FUNC("Rotate180");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixRotate180(pixd, pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xcen).
 * Arg #3 is expected to be a l_int32 (ycen).
 * Arg #4 is expected to be a l_float32 (angle).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Rotate2Shear(lua_State *L)
{
    LL_FUNC("Rotate2Shear");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xcen = ll_check_l_int32(_fun, L, 2);
    l_int32 ycen = ll_check_l_int32(_fun, L, 3);
    l_float32 angle = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRotate2Shear(pixs, xcen, ycen, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xcen).
 * Arg #3 is expected to be a l_int32 (ycen).
 * Arg #4 is expected to be a l_float32 (angle).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Rotate3Shear(lua_State *L)
{
    LL_FUNC("Rotate3Shear");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xcen = ll_check_l_int32(_fun, L, 2);
    l_int32 ycen = ll_check_l_int32(_fun, L, 3);
    l_float32 angle = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRotate3Shear(pixs, xcen, ycen, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Rotate90(lua_State *L)
{
    LL_FUNC("Rotate90");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 direction = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixRotate90(pixs, direction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAM(lua_State *L)
{
    LL_FUNC("RotateAM");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixRotateAM(pixs, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMColor(lua_State *L)
{
    LL_FUNC("RotateAMColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixRotateAMColor(pixs, angle, colorval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_uint32 (fillval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMColorCorner(lua_State *L)
{
    LL_FUNC("RotateAMColorCorner");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_uint32 fillval = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixRotateAMColorCorner(pixs, angle, fillval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_uint32 (colorval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMColorFast(lua_State *L)
{
    LL_FUNC("RotateAMColorFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_uint32 colorval = ll_check_l_uint32(_fun, L, 3);
    Pix *pix = pixRotateAMColorFast(pixs, angle, colorval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMCorner(lua_State *L)
{
    LL_FUNC("RotateAMCorner");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixRotateAMCorner(pixs, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMGray(lua_State *L)
{
    LL_FUNC("RotateAMGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 3);
    Pix *pix = pixRotateAMGray(pixs, angle, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_uint8 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateAMGrayCorner(lua_State *L)
{
    LL_FUNC("RotateAMGrayCorner");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_uint8 grayval = ll_check_l_uint8(_fun, L, 3);
    Pix *pix = pixRotateAMGrayCorner(pixs, angle, grayval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateBinaryNice(lua_State *L)
{
    LL_FUNC("RotateBinaryNice");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixRotateBinaryNice(pixs, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xcen).
 * Arg #3 is expected to be a l_int32 (ycen).
 * Arg #4 is expected to be a l_float32 (angle).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateBySampling(lua_State *L)
{
    LL_FUNC("RotateBySampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xcen = ll_check_l_int32(_fun, L, 2);
    l_int32 ycen = ll_check_l_int32(_fun, L, 3);
    l_float32 angle = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRotateBySampling(pixs, xcen, ycen, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (quads).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 quads = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixRotateOrth(pixs, quads);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xcen).
 * Arg #3 is expected to be a l_int32 (ycen).
 * Arg #4 is expected to be a l_float32 (angle).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateShear(lua_State *L)
{
    LL_FUNC("RotateShear");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xcen = ll_check_l_int32(_fun, L, 2);
    l_int32 ycen = ll_check_l_int32(_fun, L, 3);
    l_float32 angle = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixRotateShear(pixs, xcen, ycen, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateShearCenter(lua_State *L)
{
    LL_FUNC("RotateShearCenter");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixRotateShearCenter(pixs, angle, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RotateShearCenterIP(lua_State *L)
{
    LL_FUNC("RotateShearCenterIP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    l_int32 incolor = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixRotateShearCenterIP(pixs, angle, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xcen).
 * Arg #3 is expected to be a l_int32 (ycen).
 * Arg #4 is expected to be a l_float32 (angle).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
RotateShearIP(lua_State *L)
{
    LL_FUNC("RotateShearIP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xcen = ll_check_l_int32(_fun, L, 2);
    l_int32 ycen = ll_check_l_int32(_fun, L, 3);
    l_float32 angle = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    l_int32 result = pixRotateShearIP(pixs, xcen, ycen, angle, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (angle).
 * Arg #3 is expected to be a Pix* (pixg).
 * Arg #4 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RotateWithAlpha(lua_State *L)
{
    LL_FUNC("RotateWithAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 angle = ll_check_l_float32(_fun, L, 2);
    Pix *pixg = ll_check_Pix(_fun, L, 3);
    l_float32 fract = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixRotateWithAlpha(pixs, angle, pixg, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get the row stats for Pix* (%pixs) as six Numa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 Numa* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
RowStats(lua_State *L)
{
    LL_FUNC("RowStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa *mean = nullptr;
    Numa *median = nullptr;
    Numa *mode = nullptr;
    Numa *modecount = nullptr;
    Numa *var = nullptr;
    Numa *rootvar = nullptr;
    if (pixRowStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return ll_push_nil(L);
    return ll_push_Numa(_fun, L, mean) +
            ll_push_Numa(_fun, L, median) +
            ll_push_Numa(_fun, L, mode) +
            ll_push_Numa(_fun, L, modecount) +
            ll_push_Numa(_fun, L, var) +
            ll_push_Numa(_fun, L, rootvar);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (runtype).
 * Arg #3 is expected to be a l_int32 (direction).
 * Arg #4 is expected to be a l_int32 (maxsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa * on the Lua stack
 */
static int
RunHistogramMorph(lua_State *L)
{
    LL_FUNC("RunHistogramMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 runtype = ll_check_l_int32(_fun, L, 2);
    l_int32 direction = ll_check_l_int32(_fun, L, 3);
    l_int32 maxsize = ll_check_l_int32(_fun, L, 4);
    Numa *result = pixRunHistogramMorph(pixs, runtype, direction, maxsize);
    return ll_push_Numa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (color).
 * Arg #3 is expected to be a l_int32 (direction).
 * Arg #4 is expected to be a l_int32 (depth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
RunlengthTransform(lua_State *L)
{
    LL_FUNC("RunlengthTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 color = ll_check_l_int32(_fun, L, 2);
    l_int32 direction = ll_check_l_int32(_fun, L, 3);
    l_int32 depth = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixRunlengthTransform(pixs, color, direction, depth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (whsize).
 * Arg #3 is expected to be a l_float32 (factor).
 * Arg #4 is expected to be a l_int32 (addborder).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SauvolaBinarize(lua_State *L)
{
    LL_FUNC("SauvolaBinarize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 whsize = ll_check_l_int32(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    l_int32 addborder = ll_check_l_int32(_fun, L, 4);
    Pix *pixm = nullptr;
    Pix *pixsd = nullptr;
    Pix *pixth = nullptr;
    Pix *pixd = nullptr;
    if (pixSauvolaBinarize(pixs, whsize, factor, addborder, &pixm, &pixsd, &pixth, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixm);
    ll_push_Pix(_fun, L, pixsd);
    ll_push_Pix(_fun, L, pixth);
    ll_push_Pix(_fun, L, pixd);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (whsize).
 * Arg #3 is expected to be a l_float32 (factor).
 * Arg #4 is expected to be a l_int32 (nx).
 * Arg #5 is expected to be a l_int32 (ny).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SauvolaBinarizeTiled(lua_State *L)
{
    LL_FUNC("SauvolaBinarizeTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 whsize = ll_check_l_int32(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    l_int32 nx = ll_check_l_int32(_fun, L, 4);
    l_int32 ny = ll_check_l_int32(_fun, L, 5);
    Pix *pixth = nullptr;
    Pix *pixd = nullptr;
    if (pixSauvolaBinarizeTiled(pixs, whsize, factor, nx, ny, &pixth, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixth);
    ll_push_Pix(_fun, L, pixd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixm).
 * Arg #2 is expected to be a Pix* (pixms).
 * Arg #3 is expected to be a l_float32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SauvolaGetThreshold(lua_State *L)
{
    LL_FUNC("SauvolaGetThreshold");
    Pix *pixm = ll_check_Pix(_fun, L, 1);
    Pix *pixms = ll_check_Pix(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    Pix *pixsd = nullptr;
    if (pixSauvolaGetThreshold(pixm, pixms, factor, &pixsd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixsd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pixa* (pixa).
 * Arg #3 is expected to be a l_float32 (scalefactor).
 * Arg #4 is expected to be a l_int32 (newrow).
 * Arg #5 is expected to be a l_int32 (space).
 * Arg #6 is expected to be a l_int32 (dp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SaveTiled(lua_State *L)
{
    LL_FUNC("SaveTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 3);
    l_int32 newrow = ll_check_l_int32(_fun, L, 4);
    l_int32 space = ll_check_l_int32(_fun, L, 5);
    l_int32 dp = ll_check_l_int32(_fun, L, 6);
    l_int32 result = pixSaveTiled(pixs, pixa, scalefactor, newrow, space, dp);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pixa* (pixa).
 * Arg #3 is expected to be a l_float32 (scalefactor).
 * Arg #4 is expected to be a l_int32 (newrow).
 * Arg #5 is expected to be a l_int32 (space).
 * Arg #6 is expected to be a l_int32 (linewidth).
 * Arg #7 is expected to be a l_int32 (dp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SaveTiledOutline(lua_State *L)
{
    LL_FUNC("SaveTiledOutline");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 3);
    l_int32 newrow = ll_check_l_int32(_fun, L, 4);
    l_int32 space = ll_check_l_int32(_fun, L, 5);
    l_int32 linewidth = ll_check_l_int32(_fun, L, 6);
    l_int32 dp = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixSaveTiledOutline(pixs, pixa, scalefactor, newrow, space, linewidth, dp);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pixa* (pixa).
 * Arg #3 is expected to be a l_int32 (outwidth).
 * Arg #4 is expected to be a l_int32 (newrow).
 * Arg #5 is expected to be a l_int32 (space).
 * Arg #6 is expected to be a l_int32 (linewidth).
 * Arg #7 is expected to be a Bmf* (bmf).
 * Arg #8 is expected to be a string (textstr).
 * Arg #9 is expected to be a l_uint32 (val).
 * Arg #10 is expected to be a l_int32 (location).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SaveTiledWithText(lua_State *L)
{
    LL_FUNC("SaveTiledWithText");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    l_int32 outwidth = ll_check_l_int32(_fun, L, 3);
    l_int32 newrow = ll_check_l_int32(_fun, L, 4);
    l_int32 space = ll_check_l_int32(_fun, L, 5);
    l_int32 linewidth = ll_check_l_int32(_fun, L, 6);
    Bmf *bmf = ll_check_Bmf(_fun, L, 7);
    const char *textstr = ll_check_string(_fun, L, 8);
    l_uint32 val = ll_check_l_uint32(_fun, L, 9);
    l_int32 location = ll_check_l_int32(_fun, L, 10);
    l_int32 result = pixSaveTiledWithText(pixs, pixa, outwidth, newrow, space, linewidth, bmf, textstr, val, location);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Scale(lua_State *L)
{
    LL_FUNC("Scale");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScale(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (scalex).
 * Arg #4 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ScaleAndTransferAlpha(lua_State *L)
{
    LL_FUNC("ScaleAndTransferAlpha");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 scalex = ll_check_l_float32(_fun, L, 3);
    l_float32 scaley = ll_check_l_float32(_fun, L, 4);
    l_int32 result = pixScaleAndTransferAlpha(pixd, pixs, scalex, scaley);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleAreaMap(lua_State *L)
{
    LL_FUNC("ScaleAreaMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleAreaMap(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleAreaMap2(lua_State *L)
{
    LL_FUNC("ScaleAreaMap2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleAreaMap2(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wd).
 * Arg #3 is expected to be a l_int32 (hd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleAreaMapToSize(lua_State *L)
{
    LL_FUNC("ScaleAreaMapToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wd = ll_check_l_int32(_fun, L, 2);
    l_int32 hd = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleAreaMapToSize(pixs, wd, hd);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleBinary(lua_State *L)
{
    LL_FUNC("ScaleBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleBinary(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleByIntSampling(lua_State *L)
{
    LL_FUNC("ScaleByIntSampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixScaleByIntSampling(pixs, factor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleBySampling(lua_State *L)
{
    LL_FUNC("ScaleBySampling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleBySampling(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wd).
 * Arg #3 is expected to be a l_int32 (hd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleBySamplingToSize(lua_State *L)
{
    LL_FUNC("ScaleBySamplingToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wd = ll_check_l_int32(_fun, L, 2);
    l_int32 hd = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleBySamplingToSize(pixs, wd, hd);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleColor2xLI(lua_State *L)
{
    LL_FUNC("ScaleColor2xLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleColor2xLI(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleColor4xLI(lua_State *L)
{
    LL_FUNC("ScaleColor4xLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleColor4xLI(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleColorLI(lua_State *L)
{
    LL_FUNC("ScaleColorLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleColorLI(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * Arg #4 is expected to be a l_float32 (sharpfract).
 * Arg #5 is expected to be a l_int32 (sharpwidth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGeneral(lua_State *L)
{
    LL_FUNC("ScaleGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    l_float32 sharpfract = ll_check_l_float32(_fun, L, 4);
    l_int32 sharpwidth = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixScaleGeneral(pixs, scalex, scaley, sharpfract, sharpwidth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray2xLI(lua_State *L)
{
    LL_FUNC("ScaleGray2xLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleGray2xLI(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray2xLIDither(lua_State *L)
{
    LL_FUNC("ScaleGray2xLIDither");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleGray2xLIDither(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray2xLIThresh(lua_State *L)
{
    LL_FUNC("ScaleGray2xLIThresh");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixScaleGray2xLIThresh(pixs, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray4xLI(lua_State *L)
{
    LL_FUNC("ScaleGray4xLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleGray4xLI(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray4xLIDither(lua_State *L)
{
    LL_FUNC("ScaleGray4xLIDither");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleGray4xLIDither(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGray4xLIThresh(lua_State *L)
{
    LL_FUNC("ScaleGray4xLIThresh");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixScaleGray4xLIThresh(pixs, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayLI(lua_State *L)
{
    LL_FUNC("ScaleGrayLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleGrayLI(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xfact).
 * Arg #3 is expected to be a l_int32 (yfact).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayMinMax(lua_State *L)
{
    LL_FUNC("ScaleGrayMinMax");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xfact = ll_check_l_int32(_fun, L, 2);
    l_int32 yfact = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixScaleGrayMinMax(pixs, xfact, yfact, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayMinMax2(lua_State *L)
{
    LL_FUNC("ScaleGrayMinMax2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixScaleGrayMinMax2(pixs, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (rank).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayRank2(lua_State *L)
{
    LL_FUNC("ScaleGrayRank2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 rank = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixScaleGrayRank2(pixs, rank);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (level1).
 * Arg #3 is expected to be a l_int32 (level2).
 * Arg #4 is expected to be a l_int32 (level3).
 * Arg #5 is expected to be a l_int32 (level4).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayRankCascade(lua_State *L)
{
    LL_FUNC("ScaleGrayRankCascade");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 level1 = ll_check_l_int32(_fun, L, 2);
    l_int32 level2 = ll_check_l_int32(_fun, L, 3);
    l_int32 level3 = ll_check_l_int32(_fun, L, 4);
    l_int32 level4 = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixScaleGrayRankCascade(pixs, level1, level2, level3, level4);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleGrayToBinaryFast(lua_State *L)
{
    LL_FUNC("ScaleGrayToBinaryFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 thresh = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleGrayToBinaryFast(pixs, factor, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleLI(lua_State *L)
{
    LL_FUNC("ScaleLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleLI(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a l_float32 (scale).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleMipmap(lua_State *L)
{
    LL_FUNC("ScaleMipmap");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    l_float32 scale = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleMipmap(pixs1, pixs2, scale);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleRGBToBinaryFast(lua_State *L)
{
    LL_FUNC("ScaleRGBToBinaryFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 thresh = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleRGBToBinaryFast(pixs, factor, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (rwt).
 * Arg #3 is expected to be a l_float32 (gwt).
 * Arg #4 is expected to be a l_float32 (bwt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleRGBToGray2(lua_State *L)
{
    LL_FUNC("ScaleRGBToGray2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rwt = ll_check_l_float32(_fun, L, 2);
    l_float32 gwt = ll_check_l_float32(_fun, L, 3);
    l_float32 bwt = ll_check_l_float32(_fun, L, 4);
    Pix *pix = pixScaleRGBToGray2(pixs, rwt, gwt, bwt);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (color).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleRGBToGrayFast(lua_State *L)
{
    LL_FUNC("ScaleRGBToGrayFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 color = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleRGBToGrayFast(pixs, factor, color);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Scale resolution of a Pix* (%pix) by a factor (%xscale, %yscale).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_float32 (xscale).
 * Arg #3 is expected to be a l_float32 (yscale).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ScaleResolution(lua_State *L)
{
    LL_FUNC("ScaleResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_float32 xscale = ll_check_l_float32(_fun, L, 2);
    l_float32 yscale = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixScaleResolution(pix, xscale, yscale));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleSmooth(lua_State *L)
{
    LL_FUNC("ScaleSmooth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixScaleSmooth(pixs, scalex, scaley);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wd).
 * Arg #3 is expected to be a l_int32 (hd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleSmoothToSize(lua_State *L)
{
    LL_FUNC("ScaleSmoothToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wd = ll_check_l_int32(_fun, L, 2);
    l_int32 hd = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleSmoothToSize(pixs, wd, hd);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalefactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray(lua_State *L)
{
    LL_FUNC("ScaleToGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 2);
    Pix *pix = pixScaleToGray(pixs, scalefactor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray16(lua_State *L)
{
    LL_FUNC("ScaleToGray16");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray16(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray2(lua_State *L)
{
    LL_FUNC("ScaleToGray2");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray2(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray3(lua_State *L)
{
    LL_FUNC("ScaleToGray3");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray3(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray4(lua_State *L)
{
    LL_FUNC("ScaleToGray4");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray4(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray6(lua_State *L)
{
    LL_FUNC("ScaleToGray6");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray6(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGray8(lua_State *L)
{
    LL_FUNC("ScaleToGray8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixScaleToGray8(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalefactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGrayFast(lua_State *L)
{
    LL_FUNC("ScaleToGrayFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 2);
    Pix *pix = pixScaleToGrayFast(pixs, scalefactor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalefactor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToGrayMipmap(lua_State *L)
{
    LL_FUNC("ScaleToGrayMipmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 2);
    Pix *pix = pixScaleToGrayMipmap(pixs, scalefactor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wd).
 * Arg #3 is expected to be a l_int32 (hd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToSize(lua_State *L)
{
    LL_FUNC("ScaleToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wd = ll_check_l_int32(_fun, L, 2);
    l_int32 hd = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleToSize(pixs, wd, hd);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (delw).
 * Arg #3 is expected to be a l_int32 (delh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleToSizeRel(lua_State *L)
{
    LL_FUNC("ScaleToSizeRel");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 delw = ll_check_l_int32(_fun, L, 2);
    l_int32 delh = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixScaleToSizeRel(pixs, delw, delh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scalex).
 * Arg #3 is expected to be a l_float32 (scaley).
 * Arg #4 is expected to be a Pix* (pixg).
 * Arg #5 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ScaleWithAlpha(lua_State *L)
{
    LL_FUNC("ScaleWithAlpha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scalex = ll_check_l_float32(_fun, L, 2);
    l_float32 scaley = ll_check_l_float32(_fun, L, 3);
    Pix *pixg = ll_check_Pix(_fun, L, 4);
    l_float32 fract = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixScaleWithAlpha(pixs, scalex, scaley, pixg, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (lowthresh).
 * Arg #4 is expected to be a l_int32 (highthresh).
 * Arg #5 is expected to be a l_int32 (maxwidth).
 * Arg #6 is expected to be a l_int32 (factor).
 * Arg #7 is expected to be a l_int32 (scanflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ScanForEdge(lua_State *L)
{
    LL_FUNC("ScanForEdge");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 lowthresh = ll_check_l_int32(_fun, L, 3);
    l_int32 highthresh = ll_check_l_int32(_fun, L, 4);
    l_int32 maxwidth = ll_check_l_int32(_fun, L, 5);
    l_int32 factor = ll_check_l_int32(_fun, L, 6);
    l_int32 scanflag = ll_check_l_int32(_fun, L, 7);
    l_int32 loc = 0;
    if (pixScanForEdge(pixs, box, lowthresh, highthresh, maxwidth, factor, scanflag, &loc))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, loc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (scanflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ScanForForeground(lua_State *L)
{
    LL_FUNC("ScanForForeground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 scanflag = ll_check_l_int32(_fun, L, 3);
    l_int32 loc = 0;
    if (pixScanForForeground(pixs, box, scanflag, &loc))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, loc);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xi).
 * Arg #3 is expected to be a l_int32 (yi).
 * Arg #4 is expected to be a l_int32 (xf).
 * Arg #5 is expected to be a l_int32 (yf).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
SearchBinaryMaze(lua_State *L)
{
    LL_FUNC("SearchBinaryMaze");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xi = ll_check_l_int32(_fun, L, 2);
    l_int32 yi = ll_check_l_int32(_fun, L, 3);
    l_int32 xf = ll_check_l_int32(_fun, L, 4);
    l_int32 yf = ll_check_l_int32(_fun, L, 5);
    Pix *pixd = nullptr;
    if (pixSearchBinaryMaze(pixs, xi, yi, xf, yf, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xi).
 * Arg #3 is expected to be a l_int32 (yi).
 * Arg #4 is expected to be a l_int32 (xf).
 * Arg #5 is expected to be a l_int32 (yf).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
SearchGrayMaze(lua_State *L)
{
    LL_FUNC("SearchGrayMaze");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xi = ll_check_l_int32(_fun, L, 2);
    l_int32 yi = ll_check_l_int32(_fun, L, 3);
    l_int32 xf = ll_check_l_int32(_fun, L, 4);
    l_int32 yf = ll_check_l_int32(_fun, L, 5);
    Pix *pixd = nullptr;
    if (pixSearchGrayMaze(pixs, xi, yi, xf, yf, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Seedfill(lua_State *L)
{
    LL_FUNC("Seedfill");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 5);
    l_int32 result = pixSeedfill(pixs, stack, x, y, connectivity);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Seedfill4(lua_State *L)
{
    LL_FUNC("Seedfill4");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixSeedfill4(pixs, stack, x, y);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box * on the Lua stack
 */
static int
Seedfill4BB(lua_State *L)
{
    LL_FUNC("Seedfill4BB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    Box *result = pixSeedfill4BB(pixs, stack, x, y);
    return ll_push_Box(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Seedfill8(lua_State *L)
{
    LL_FUNC("Seedfill8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixSeedfill8(pixs, stack, x, y);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box * on the Lua stack
 */
static int
Seedfill8BB(lua_State *L)
{
    LL_FUNC("Seedfill8BB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    Box *result = pixSeedfill8BB(pixs, stack, x, y);
    return ll_push_Box(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Stack* (stack).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box * on the Lua stack
 */
static int
SeedfillBB(lua_State *L)
{
    LL_FUNC("SeedfillBB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Stack *stack = ll_check_Stack(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 5);
    Box *result = pixSeedfillBB(pixs, stack, x, y, connectivity);
    return ll_push_Box(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SeedfillBinary(lua_State *L)
{
    LL_FUNC("SeedfillBinary");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSeedfillBinary(pixd, pixs, pixm, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Pix* (pixm).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * Arg #5 is expected to be a l_int32 (xmax).
 * Arg #6 is expected to be a l_int32 (ymax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SeedfillBinaryRestricted(lua_State *L)
{
    LL_FUNC("SeedfillBinaryRestricted");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    l_int32 xmax = ll_check_l_int32(_fun, L, 5);
    l_int32 ymax = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixSeedfillBinaryRestricted(pixd, pixs, pixm, connectivity, xmax, ymax);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SeedfillGray(lua_State *L)
{
    LL_FUNC("SeedfillGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSeedfillGray(pixs, pixm, connectivity);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixb).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (delta).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SeedfillGrayBasin(lua_State *L)
{
    LL_FUNC("SeedfillGrayBasin");
    Pix *pixb = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 delta = ll_check_l_int32(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSeedfillGrayBasin(pixb, pixm, delta, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SeedfillGrayInv(lua_State *L)
{
    LL_FUNC("SeedfillGrayInv");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSeedfillGrayInv(pixs, pixm, connectivity);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SeedfillGrayInvSimple(lua_State *L)
{
    LL_FUNC("SeedfillGrayInvSimple");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSeedfillGrayInvSimple(pixs, pixm, connectivity);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SeedfillGraySimple(lua_State *L)
{
    LL_FUNC("SeedfillGraySimple");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSeedfillGraySimple(pixs, pixm, connectivity);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (maxiters).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SeedfillMorph(lua_State *L)
{
    LL_FUNC("SeedfillMorph");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 maxiters = ll_check_l_int32(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSeedfillMorph(pixs, pixm, maxiters, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Seedspread(lua_State *L)
{
    LL_FUNC("Seedspread");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixSeedspread(pixs, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectByAreaFraction(lua_State *L)
{
    LL_FUNC("SelectByAreaFraction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    l_int32 changed = 0;
    if (pixSelectByAreaFraction(pixs, thresh, connectivity, type, &changed))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, changed);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectByPerimSizeRatio(lua_State *L)
{
    LL_FUNC("SelectByPerimSizeRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    l_int32 changed = 0;
    if (pixSelectByPerimSizeRatio(pixs, thresh, connectivity, type, &changed))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, changed);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectByPerimToAreaRatio(lua_State *L)
{
    LL_FUNC("SelectByPerimToAreaRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    l_int32 changed = 0;
    if (pixSelectByPerimToAreaRatio(pixs, thresh, connectivity, type, &changed))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, changed);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a l_int32 (height).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * Arg #5 is expected to be a l_int32 (type).
 * Arg #6 is expected to be a l_int32 (relation).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectBySize(lua_State *L)
{
    LL_FUNC("SelectBySize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 height = ll_check_l_int32(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    l_int32 type = ll_check_l_int32(_fun, L, 5);
    l_int32 relation = ll_check_l_int32(_fun, L, 6);
    l_int32 changed = 0;
    if (pixSelectBySize(pixs, width, height, connectivity, type, relation, &changed))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, changed);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (thresh).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectByWidthHeightRatio(lua_State *L)
{
    LL_FUNC("SelectByWidthHeightRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 thresh = ll_check_l_float32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    l_int32 changed = 0;
    if (pixSelectByWidthHeightRatio(pixs, thresh, connectivity, type, &changed))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, changed);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SelectDefaultPdfEncoding(lua_State *L)
{
    LL_FUNC("SelectDefaultPdfEncoding");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 type = L_G4_ENCODE;
    if (selectDefaultPdfEncoding(pix, &type))
        return ll_push_nil(L);
    lua_pushstring(L, ll_string_encoding(type));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (areaslop).
 * Arg #3 is expected to be a l_int32 (yslop).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box * on the Lua stack
 */
static int
SelectLargeULComp(lua_State *L)
{
    LL_FUNC("SelectLargeULComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 areaslop = ll_check_l_float32(_fun, L, 2);
    l_int32 yslop = ll_check_l_int32(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    Box *result = pixSelectLargeULComp(pixs, areaslop, yslop, connectivity);
    return ll_push_Box(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SelectMinInConnComp(lua_State *L)
{
    LL_FUNC("SelectMinInConnComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    Pta *pta = nullptr;
    Numa *nav = nullptr;
    if (pixSelectMinInConnComp(pixs, pixm, &pta, &nav))
        return ll_push_nil(L);
    ll_push_Pta(_fun, L, pta);
    ll_push_Numa(_fun, L, nav);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mindist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SelectedLocalExtrema(lua_State *L)
{
    LL_FUNC("SelectedLocalExtrema");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mindist = ll_check_l_int32(_fun, L, 2);
    Pix *pixmin = nullptr;
    Pix *pixmax = nullptr;
    if (pixSelectedLocalExtrema(pixs, mindist, &pixmin, &pixmax))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixmin);
    ll_push_Pix(_fun, L, pixmax);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (connectivity).
 * Arg #3 is expected to be a l_int32 (minw).
 * Arg #4 is expected to be a l_int32 (minh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SelectiveConnCompFill(lua_State *L)
{
    LL_FUNC("SelectiveConnCompFill");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 2);
    l_int32 minw = ll_check_l_int32(_fun, L, 3);
    l_int32 minh = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSelectiveConnCompFill(pixs, connectivity, minw, minh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SerializeToMemory(lua_State *L)
{
    LL_FUNC("SerializeToMemory");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 *data = nullptr;
    size_t nbytes = 0;
    if (pixSerializeToMemory(pixs, &data, &nbytes))
        return ll_push_nil(L);
    return ll_push_Uarray(_fun, L, data, static_cast<l_int32>(nbytes / sizeof(l_uint32)));
}

/**
 * \brief Set all pixels in a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolane on the Lua stack
 */
static int
SetAll(lua_State *L)
{
    LL_FUNC("SetAll");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixSetAll(pix));
}

/**
 * \brief Set all pixels in a Pix* to an arbitrary value %val.
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_uint32 (val).
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAllArbitrary(lua_State *L)
{
    LL_FUNC("SetAllArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetAllArbitrary(pix, val));
}

/**
 * \brief Set all pixels in a Pix* to a gray value (%grayval).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (grayval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetAllGray(lua_State *L)
{
    LL_FUNC("SetAllGray");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 grayval = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetAllGray(pix, grayval));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SetAlphaOverWhite(lua_State *L)
{
    LL_FUNC("SetAlphaOverWhite");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pix = pixSetAlphaOverWhite(pixs);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Set all pixels in a Pix* to black.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlack(lua_State *L)
{
    LL_FUNC("SetBlack");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixSetBlackOrWhite(pix, L_SET_BLACK));
}

/**
 * \brief Set all pixels in a Pix* to black or white.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a string describing an operation (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlackOrWhite(lua_State *L)
{
    LL_FUNC("SetBlackOrWhite");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 op = ll_check_blackwhite(_fun, L, 2, L_SET_BLACK);
    return ll_push_boolean(_fun, L, 0 == pixSetBlackOrWhite(pix, op));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Boxa* (boxa).
 * Arg #3 is expected to be a l_int32 (op).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SetBlackOrWhiteBoxa(lua_State *L)
{
    LL_FUNC("SetBlackOrWhiteBoxa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 op = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixSetBlackOrWhiteBoxa(pixs, boxa, op);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Set border ring pixels in a Pix* (%pix) to value (%val).
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (dist).
 * Arg #3 is expected to be a l_uint32 (val).
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBorderRingVal(lua_State *L)
{
    LL_FUNC("SetBorderRingVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 dist = ll_check_l_int32(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixSetBorderRingVal(pix, dist, val));
}

/**
 * \brief Set border pixels in a Pix* (%pix) to value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * Arg #6 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBorderVal(lua_State *L)
{
    LL_FUNC("SetBorderVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_uint32 val = ll_check_l_uint32(_fun, L, 6);
    return ll_push_boolean(_fun, L, 0 == pixSetBorderVal(pix, left, right, top, bottom, val));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (sampling).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetChromaSampling(lua_State *L)
{
    LL_FUNC("SetChromaSampling");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 sampling = ll_check_l_int32(_fun, L, 2);
    l_int32 result = pixSetChromaSampling(pix, sampling);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Set the colormap of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a PixColormap* (colormap).
 *
 * Note:
 * The specified PixColormap* is empty afterwards.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
SetColormap(lua_State *L)
{
    LL_FUNC("SetColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* colormap = ll_take_PixColormap(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetColormap(pix, colormap));
}

/**
 * \brief Set all pixel components (%comp) in a Pix* (%pix) to a value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a string with the component name (comp).
 * Arg #3 is expected to be a l_int32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetComponentArbitrary(lua_State *L)
{
    LL_FUNC("SetComponentArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 comp = ll_check_component(_fun, L, 2, 0);
    l_int32 val = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixSetComponentArbitrary(pix, comp, val));
}

/**
 * \brief Set the data of a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Lua array table (h) of array tables (wpl).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetData(lua_State *L)
{
    LL_FUNC("SetData");
    Pix* pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);
    size_t size = static_cast<size_t>(wpl) * static_cast<size_t>(h) * sizeof(l_uint32);
    l_uint32 *data = ll_malloc<l_uint32>(_fun, L, size);
    /* copy the previous Pix->data in case only a part of data[][] is written */
    memcpy(data, pixGetData(pix), size);
    data = ll_unpack_Uarray_2d(_fun, L, 2, data, wpl, h);
    /* Do not free(data); it is owned by the Pix* now */
    return ll_push_boolean(_fun, L, 0 == pixSetData(pix, data));
}

/**
 * \brief Set the Pix* depth.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetDepth(lua_State *L)
{
    LL_FUNC("SetDepth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 depth = ll_check_l_int32_default(_fun, L, 2, pixGetDepth(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetDepth(pix, depth));
}

/**
 * \brief Set the Pix* dimensions.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * Arg #2 is expected to be a lua_Integer (width).
 * Arg #3 is expected to be a lua_Integer (height).
 * Arg #4 is expected to be a lua_Integer (depth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean result true or false
 */
static int
SetDimensions(lua_State *L)
{
    LL_FUNC("SetDimensions");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 height = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 depth = ll_check_l_int32_default(_fun, L, 4, 1);
    return ll_push_boolean(_fun, L, 0 == pixSetDimensions(pix, width, height, depth));
}

/**
 * \brief Set the Pix* height.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetHeight(lua_State *L)
{
    LL_FUNC("SetHeight");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 height = ll_check_l_int32_default(_fun, L, 2, pixGetHeight(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetHeight(pix, height));
}

/**
 * \brief Set all pixels inside a Box* (%box) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRect(lua_State *L)
{
    LL_FUNC("SetInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetInRect(pix, box));
}

/**
 * \brief Set all pixels inside a Box* (%box) in a Pix* (%pix) to a value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRectArbitrary(lua_State *L)
{
    LL_FUNC("SetInRectArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixSetInRectArbitrary(pix, box, val));
}

/**
 * \brief Set the input format of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string with the input format name (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInputFormat(lua_State *L)
{
    LL_FUNC("SetInputFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 format = ll_check_input_format(_fun, L, 2, IFF_UNKNOWN);
    return ll_push_boolean(_fun, L, 0 == pixSetInputFormat(pix, format));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs1).
 * Arg #2 is expected to be a Pix* (pixs2).
 * Arg #3 is expected to be a l_int32 (mindiff).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetLowContrast(lua_State *L)
{
    LL_FUNC("SetLowContrast");
    Pix *pixs1 = ll_check_Pix(_fun, L, 1);
    Pix *pixs2 = ll_check_Pix(_fun, L, 2);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSetLowContrast(pixs1, pixs2, mindiff);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixm) with 1 bit/pixel.
 * Arg #3 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMasked(lua_State *L)
{
    LL_FUNC("SetMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixSetMasked(pixd, pixm, val));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (rval).
 * Arg #6 is expected to be a l_int32 (gval).
 * Arg #7 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetMaskedCmap(lua_State *L)
{
    LL_FUNC("SetMaskedCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 rval = ll_check_l_int32(_fun, L, 5);
    l_int32 gval = ll_check_l_int32(_fun, L, 6);
    l_int32 bval = ll_check_l_int32(_fun, L, 7);
    l_int32 result = pixSetMaskedCmap(pixs, pixm, x, y, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Set the mask value (%val) for a Pix* (%pixd) at offset (%x,%y) from another Pix* (%pixm).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixm) with 1 bit/pixel.
 * Arg #3 is expected to be a l_uint32 (val).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMaskedGeneral(lua_State *L)
{
    LL_FUNC("SetMaskedGeneral");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixSetMaskedGeneral(pixd, pixm, val, x, y));
}

/**
 * \brief Set mirrored border pixels in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMirroredBorder(lua_State *L)
{
    LL_FUNC("SetMirroredBorder");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixSetMirroredBorder(pix, left, right, top, bottom));
}

/**
 * \brief Set or clear border pixels in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (left).
 * Arg #3 is expected to be a l_int32 (right).
 * Arg #4 is expected to be a l_int32 (top).
 * Arg #5 is expected to be a l_int32 (bottom).
 * Arg #6 is expected to be a operation (op = PIX_SET or PIX_CLR).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetOrClearBorder(lua_State *L)
{
    LL_FUNC("SetOrClearBorder");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_int32 op = ll_check_rasterop(_fun, L, 6, PIX_CLR);
    return ll_push_boolean(_fun, L, 0 == pixSetOrClearBorder(pix, left, right, top, bottom, op));
}

/**
 * \brief Set pad bits in a Pix* (%pix) to value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPadBits(lua_State *L)
{
    LL_FUNC("SetPadBits");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetPadBits(pix, val));
}

/**
 * \brief Set pad bits in a Pix* (%pix) band (%by, %bh) to value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (by).
 * Arg #3 is expected to be a l_int32 (bh).
 * Arg #4 is expected to be a l_int32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPadBitsBand(lua_State *L)
{
    LL_FUNC("SetPadBitsBand");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 by = ll_check_l_int32(_fun, L, 2);
    l_int32 bh = ll_check_l_int32(_fun, L, 3);
    l_int32 val = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixSetPadBitsBand(pix, by, bh, val));
}

/**
 * \brief Set a pixel value in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPixel(lua_State *L)
{
    LL_FUNC("SetPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4) - 1;
    return ll_push_boolean(_fun, L, 0 == pixSetPixel(pix, x, y, val));
}

/**
 * \brief Set a pixel column in Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (col).
 * Arg #3 is expected to be an array table of lua_Numbers for each row of pixd (tblvect).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack (result)
 */
static int
SetPixelColumn(lua_State *L)
{
    LL_FUNC("SetPixelColumn");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 col = ll_check_l_int32(_fun, L, 2);
    l_int32 rows = pixGetHeight(pixd);
    l_int32 n;
    l_float32 *tblvect = ll_unpack_Farray(_fun, L, 3, &n);
    l_float32 *colvect = reinterpret_cast<l_float32 *>(LEPT_CALLOC(rows, sizeof(l_float32)));
    l_int32 i;
    l_int32 result = FALSE;
    if (!colvect) {
        lua_pushfstring(L, "%s: could not allocate colvect (%d)",
                        _fun, static_cast<size_t>(rows) * sizeof(*colvect));
        lua_error(L);
        return 0;
    }
    for (i = 0; i < rows && i < n; i++)
        colvect[i] = tblvect[i];
    LEPT_FREE(tblvect);
    result = pixSetPixelColumn(pixd, col, colvect);
    LEPT_FREE(colvect);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief Set one component in Pix* (pixd 32bpp) from Pix* (pixs 8bpp).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a string with the component name (comp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetRGBComponent(lua_State *L)
{
    LL_FUNC("SetRGBComponent");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 comp = ll_check_component(_fun, L, 3, L_ALPHA_CHANNEL);
    return ll_push_boolean(_fun, L, 0 == pixSetRGBComponent(pixd, pixs, comp));
}

/**
 * \brief Set a pixel's RGB values in Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (x).
 * Arg #3 is expected to be a l_int32 (y).
 * Arg #4 is expected to be a l_int32 (rval).
 * Arg #5 is expected to be a l_int32 (gval).
 * Arg #6 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetRGBPixel(lua_State *L)
{
    LL_FUNC("SetRGBPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    return ll_push_boolean(_fun, L, 0 == pixSetRGBPixel(pix, x, y, rval, gval, bval));
}

/**
 * \brief Set the PIX side resolution (x, y).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * Arg #2 is expected to be a lua_Integer (xres).
 * Arg #3 is expected to be a lua_Integer (yres).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean result true or false
 */
static int
SetResolution(lua_State *L)
{
    LL_FUNC("SetResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32_default(_fun, L, 2, 300);
    l_int32 yres = ll_check_l_int32_default(_fun, L, 3, xres);
    return ll_push_boolean(_fun, L, 0 == pixSetResolution(pix, xres, yres));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (sindex).
 * Arg #4 is expected to be a l_int32 (rval).
 * Arg #5 is expected to be a l_int32 (gval).
 * Arg #6 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetSelectCmap(lua_State *L)
{
    LL_FUNC("SetSelectCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 sindex = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    l_int32 result = pixSetSelectCmap(pixs, box, sindex, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_int32 (sindex).
 * Arg #6 is expected to be a l_int32 (rval).
 * Arg #7 is expected to be a l_int32 (gval).
 * Arg #8 is expected to be a l_int32 (bval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetSelectMaskedCmap(lua_State *L)
{
    LL_FUNC("SetSelectMaskedCmap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 sindex = ll_check_l_int32(_fun, L, 5);
    l_int32 rval = ll_check_l_int32(_fun, L, 6);
    l_int32 gval = ll_check_l_int32(_fun, L, 7);
    l_int32 bval = ll_check_l_int32(_fun, L, 8);
    l_int32 result = pixSetSelectMaskedCmap(pixs, pixm, x, y, sindex, rval, gval, bval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Set the special value (%special) of a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (special).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetSpecial(lua_State *L)
{
    LL_FUNC("SetSpecial");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 special = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSetSpecial(pix, special));
}

/**
 * \brief Set the Pix* SPP (samples per pixel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetSpp(lua_State *L)
{
    LL_FUNC("SetSpp");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 spp = ll_check_l_int32_default(_fun, L, 2, pixGetSpp(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetSpp(pix, spp));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a l_int32 (thinfirst).
 * Arg #4 is expected to be a l_int32 (connectivity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SetStrokeWidth(lua_State *L)
{
    LL_FUNC("SetStrokeWidth");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 thinfirst = ll_check_l_int32(_fun, L, 3);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSetStrokeWidth(pixs, width, thinfirst, connectivity);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Set the text of a Pix*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (text).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetText(lua_State *L)
{
    LL_FUNC("SetText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, pixSetText(pix, text));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a string (textstr).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a l_int32 (x0).
 * Arg #6 is expected to be a l_int32 (y0).
 * Arg #7 is expected to be a l_int32 (wtext).
 * Arg #8 is expected to be a l_int32 (firstindent).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetTextblock(lua_State *L)
{
    LL_FUNC("SetTextblock");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Bmf *bmf = ll_check_Bmf(_fun, L, 2);
    const char *textstr = ll_check_string(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4);
    l_int32 x0 = ll_check_l_int32(_fun, L, 5);
    l_int32 y0 = ll_check_l_int32(_fun, L, 6);
    l_int32 wtext = ll_check_l_int32(_fun, L, 7);
    l_int32 firstindent = ll_check_l_int32(_fun, L, 8);
    l_int32 overflow = 0;
    if (pixSetTextblock(pixs, bmf, textstr, val, x0, y0, wtext, firstindent, &overflow))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, overflow);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Bmf* (bmf).
 * Arg #3 is expected to be a string (textstr).
 * Arg #4 is expected to be a l_uint32 (val).
 * Arg #5 is expected to be a l_int32 (x0).
 * Arg #6 is expected to be a l_int32 (y0).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetTextline(lua_State *L)
{
    LL_FUNC("SetTextline");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Bmf *bmf = ll_check_Bmf(_fun, L, 2);
    const char *textstr = ll_check_string(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4);
    l_int32 x0 = ll_check_l_int32(_fun, L, 5);
    l_int32 y0 = ll_check_l_int32(_fun, L, 6);
    l_int32 width = 0;
    l_int32 overflow = 0;
    if (pixSetTextline(pixs, bmf, textstr, val, x0, y0, &width, &overflow))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, width);
    ll_push_l_int32(_fun, L, overflow);
    return 2;
}

/**
 * \brief Create a new Pix* (%pixd) from Pix* (%pixs) seting alpha == 0 pixels to a value (%val).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_uint32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
SetUnderTransparency(lua_State *L)
{
    LL_FUNC("SetUnderTransparency");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    ll_push_Pix(_fun, L, pixSetUnderTransparency(pixs, val, 0));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to white.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetWhite(lua_State *L)
{
    LL_FUNC("SetWhite");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixSetBlackOrWhite(pix, L_SET_WHITE));
}

/**
 * \brief Set the Pix* width.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWidth(lua_State *L)
{
    LL_FUNC("SetWidth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32_default(_fun, L, 2, pixGetWidth(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetWidth(pix, width));
}

/**
 * \brief Set the Pix* (%pix) words per line (%wpl).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWpl(lua_State *L)
{
    LL_FUNC("SetWpl");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = ll_check_l_int32_default(_fun, L, 2, pixGetWpl(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetWpl(pix, wpl));
}

/**
 * \brief Set the Pix* X resolution.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetXRes(lua_State *L)
{
    LL_FUNC("SetXRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32_default(_fun, L, 2, pixGetXRes(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetXRes(pix, xres));
}

/**
 * \brief Set the Pix* Y resolution.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetYRes(lua_State *L)
{
    LL_FUNC("SetYRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 yres = ll_check_l_int32_default(_fun, L, 2, pixGetYRes(pix));
    return ll_push_boolean(_fun, L, 0 == pixSetYRes(pix, yres));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (compval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetZlibCompression(lua_State *L)
{
    LL_FUNC("SetZlibCompression");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 compval = ll_check_l_int32(_fun, L, 2);
    l_int32 result = pixSetZlibCompression(pix, compval);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_uint8 ** on the Lua stack
 */
static int
SetupByteProcessing(lua_State *L)
{
    LL_FUNC("SetupByteProcessing");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (pixSetupByteProcessing(pix, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Shift and transfer alpha channel from a Pix* (%pixs) to a Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (shiftx).
 * Arg #3 is expected to be a l_int32 (shifty).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ShiftAndTransferAlpha(lua_State *L)
{
    LL_FUNC("ShiftAndTransferAlpha");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 shiftx = ll_check_l_int32(_fun, L, 3);
    l_int32 shifty = ll_check_l_int32(_fun, L, 4);
    lua_pushboolean(L, pixShiftAndTransferAlpha(pixd, pixs, shiftx, shifty));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (srcval).
 * Arg #4 is expected to be a l_uint32 (dstval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ShiftByComponent(lua_State *L)
{
    LL_FUNC("ShiftByComponent");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_uint32 srcval = ll_check_l_uint32(_fun, L, 3);
    l_uint32 dstval = ll_check_l_uint32(_fun, L, 4);
    Pix *pix = pixShiftByComponent(pixd, pixs, srcval, dstval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (border).
 * Arg #3 is expected to be a l_int32 (nterms).
 * Arg #4 is expected to be a l_uint32 (seed).
 * Arg #5 is expected to be a l_uint32 (color).
 * Arg #6 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SimpleCaptcha(lua_State *L)
{
    LL_FUNC("SimpleCaptcha");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 border = ll_check_l_int32(_fun, L, 2);
    l_int32 nterms = ll_check_l_int32(_fun, L, 3);
    l_uint32 seed = ll_check_l_uint32(_fun, L, 4);
    l_uint32 color = ll_check_l_uint32(_fun, L, 5);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixSimpleCaptcha(pixs, border, nterms, seed, color, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (sigbits).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_int32 (ncolors).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SimpleColorQuantize(lua_State *L)
{
    LL_FUNC("SimpleColorQuantize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 sigbits = ll_check_l_int32(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 ncolors = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixSimpleColorQuantize(pixs, sigbits, factor, ncolors);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check if a Pix* is valid.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SizesEqual(lua_State *L)
{
    LL_FUNC("SizesEqual");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixSizesEqual(pix1, pix2));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SmoothConnectedRegions(lua_State *L)
{
    LL_FUNC("SmoothConnectedRegions");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 result = pixSmoothConnectedRegions(pixs, pixm, factor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (srcval).
 * Arg #4 is expected to be a l_uint32 (dstval).
 * Arg #5 is expected to be a l_int32 (diff).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SnapColor(lua_State *L)
{
    LL_FUNC("SnapColor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_uint32 srcval = ll_check_l_uint32(_fun, L, 3);
    l_uint32 dstval = ll_check_l_uint32(_fun, L, 4);
    l_int32 diff = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixSnapColor(pixd, pixs, srcval, dstval, diff);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_uint32 (srcval).
 * Arg #4 is expected to be a l_uint32 (dstval).
 * Arg #5 is expected to be a l_int32 (diff).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SnapColorCmap(lua_State *L)
{
    LL_FUNC("SnapColorCmap");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_uint32 srcval = ll_check_l_uint32(_fun, L, 3);
    l_uint32 dstval = ll_check_l_uint32(_fun, L, 4);
    l_int32 diff = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixSnapColorCmap(pixd, pixs, srcval, dstval, diff);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (orientflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SobelEdgeFilter(lua_State *L)
{
    LL_FUNC("SobelEdgeFilter");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 orientflag = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixSobelEdgeFilter(pixs, orientflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (minsum).
 * Arg #4 is expected to be a l_int32 (skipdist).
 * Arg #5 is expected to be a l_int32 (delta).
 * Arg #6 is expected to be a l_int32 (maxbg).
 * Arg #7 is expected to be a l_int32 (maxcomps).
 * Arg #8 is expected to be a l_int32 (remainder).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
SplitComponentIntoBoxa(lua_State *L)
{
    LL_FUNC("SplitComponentIntoBoxa");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 minsum = ll_check_l_int32(_fun, L, 3);
    l_int32 skipdist = ll_check_l_int32(_fun, L, 4);
    l_int32 delta = ll_check_l_int32(_fun, L, 5);
    l_int32 maxbg = ll_check_l_int32(_fun, L, 6);
    l_int32 maxcomps = ll_check_l_int32(_fun, L, 7);
    l_int32 remainder = ll_check_l_int32(_fun, L, 8);
    Boxa *result = pixSplitComponentIntoBoxa(pix, box, minsum, skipdist, delta, maxbg, maxcomps, remainder);
    return ll_push_Boxa(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (delta).
 * Arg #3 is expected to be a l_int32 (mindel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
SplitComponentWithProfile(lua_State *L)
{
    LL_FUNC("SplitComponentWithProfile");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    l_int32 mindel = ll_check_l_int32(_fun, L, 3);
    Pix *pixdebug = nullptr;
    if (pixSplitComponentWithProfile(pixs, delta, mindel, &pixdebug))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixdebug);
    return 1;
}

/**
 * \brief Split the distribution for bg/fg in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_float32 (scorefract).
 * Arg #3 is expected to be a l_int32 (factor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
SplitDistributionFgBg(lua_State *L)
{
    LL_FUNC("SplitDistributionFgBg");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scorefract = ll_check_l_float32_default(_fun, L, 2, 0.0f);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 thresh;
    l_int32 fgval;
    l_int32 bgval;
    if (pixSplitDistributionFgBg(pixs, scorefract, factor, &thresh, &fgval, &bgval, nullptr))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, thresh) +
            ll_push_l_int32(_fun, L, fgval) +
            ll_push_l_int32(_fun, L, bgval);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minsum).
 * Arg #3 is expected to be a l_int32 (skipdist).
 * Arg #4 is expected to be a l_int32 (delta).
 * Arg #5 is expected to be a l_int32 (maxbg).
 * Arg #6 is expected to be a l_int32 (maxcomps).
 * Arg #7 is expected to be a l_int32 (remainder).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa * on the Lua stack
 */
static int
SplitIntoBoxa(lua_State *L)
{
    LL_FUNC("SplitIntoBoxa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minsum = ll_check_l_int32(_fun, L, 2);
    l_int32 skipdist = ll_check_l_int32(_fun, L, 3);
    l_int32 delta = ll_check_l_int32(_fun, L, 4);
    l_int32 maxbg = ll_check_l_int32(_fun, L, 5);
    l_int32 maxcomps = ll_check_l_int32(_fun, L, 6);
    l_int32 remainder = ll_check_l_int32(_fun, L, 7);
    Boxa *boxa = pixSplitIntoBoxa(pixs, minsum, skipdist, delta, maxbg, maxcomps, remainder);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minw).
 * Arg #3 is expected to be a l_int32 (minh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SplitIntoCharacters(lua_State *L)
{
    LL_FUNC("SplitIntoCharacters");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minw = ll_check_l_int32(_fun, L, 2);
    l_int32 minh = ll_check_l_int32(_fun, L, 3);
    Boxa *boxa = nullptr;
    Pixa *pixa = nullptr;
    Pix *pixdebug = nullptr;
    if (pixSplitIntoCharacters(pixs, minw, minh, &boxa, &pixa, &pixdebug))
        return ll_push_nil(L);
    return ll_push_Boxa(_fun, L, boxa) +
            ll_push_Pixa(_fun, L, pixa) +
            ll_push_Pix(_fun, L, pixdebug);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_float32 (rwt).
 * Arg #4 is expected to be a l_float32 (gwt).
 * Arg #5 is expected to be a l_float32 (bwt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
StereoFromPair(lua_State *L)
{
    LL_FUNC("StereoFromPair");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_float32 rwt = ll_check_l_float32(_fun, L, 3);
    l_float32 gwt = ll_check_l_float32(_fun, L, 4);
    l_float32 bwt = ll_check_l_float32(_fun, L, 5);
    Pix *pix = pixStereoFromPair(pix1, pix2, rwt, gwt, bwt);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (hmax).
 * Arg #5 is expected to be a l_int32 (operation).
 * Arg #6 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
StretchHorizontal(lua_State *L)
{
    LL_FUNC("StretchHorizontal");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 hmax = ll_check_l_int32(_fun, L, 4);
    l_int32 operation = ll_check_l_int32(_fun, L, 5);
    l_int32 incolor = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixStretchHorizontal(pixs, dir, type, hmax, operation, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (hmax).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
StretchHorizontalLI(lua_State *L)
{
    LL_FUNC("StretchHorizontalLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 hmax = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixStretchHorizontalLI(pixs, dir, type, hmax, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (hmax).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
StretchHorizontalSampled(lua_State *L)
{
    LL_FUNC("StretchHorizontalSampled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    l_int32 hmax = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixStretchHorizontalSampled(pixs, dir, type, hmax, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (color).
 * Arg #3 is expected to be a l_int32 (depth).
 * Arg #4 is expected to be a l_int32 (nangles).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
StrokeWidthTransform(lua_State *L)
{
    LL_FUNC("StrokeWidthTransform");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 color = ll_check_l_int32(_fun, L, 2);
    l_int32 depth = ll_check_l_int32(_fun, L, 3);
    l_int32 nangles = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixStrokeWidthTransform(pixs, color, depth, nangles);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (skip).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pta * on the Lua stack
 */
static int
SubsampleBoundaryPixels(lua_State *L)
{
    LL_FUNC("SubsampleBoundaryPixels");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 skip = ll_check_l_int32(_fun, L, 2);
    Pta *result = pixSubsampleBoundaryPixels(pixs, skip);
    return ll_push_Pta(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs1).
 * Arg #3 is expected to be a Pix* (pixs2).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
SubtractGray(lua_State *L)
{
    LL_FUNC("SubtractGray");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs1 = ll_check_Pix(_fun, L, 2);
    Pix *pixs2 = ll_check_Pix(_fun, L, 3);
    Pix *pix = pixSubtractGray(pixd, pixs1, pixs2);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Swap an destroy a Pix* (pixd) with another Pix* (pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SwapAndDestroy(lua_State *L)
{
    LL_FUNC("SwapAndDestroy");
    Pix **ppixd = ll_check_udata<Pix>(_fun, L, 1, LL_PIX);
    Pix **ppixs = ll_check_udata<Pix>(_fun, L, 2, LL_PIX);
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSwapAndDestroy(&pixd, &pixs));
    *ppixd = pixd;
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TRCMap(lua_State *L)
{
    LL_FUNC("TRCMap");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    Numa *na = ll_check_Numa(_fun, L, 3);
    l_int32 result = pixTRCMap(pixs, pixm, na);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TestClipToForeground(lua_State *L)
{
    LL_FUNC("TestClipToForeground");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 canclip = 0;
    if (pixTestClipToForeground(pixs, &canclip))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, canclip);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1).
 * Arg #2 is expected to be a Pix* (pix2).
 * Arg #3 is expected to be a l_int32 (factor).
 * Arg #4 is expected to be a l_int32 (mindiff).
 * Arg #5 is expected to be a l_float32 (maxfract).
 * Arg #6 is expected to be a l_float32 (maxave).
 * Arg #8 is expected to be a l_int32 (details).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TestForSimilarity(lua_State *L)
{
    LL_FUNC("TestForSimilarity");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    l_int32 mindiff = ll_check_l_int32(_fun, L, 4);
    l_float32 maxfract = ll_check_l_float32(_fun, L, 5);
    l_float32 maxave = ll_check_l_float32(_fun, L, 6);
    l_int32 similar = 0;
    l_int32 details = ll_check_l_int32(_fun, L, 8);
    if (pixTestForSimilarity(pix1, pix2, factor, mindiff, maxfract, maxave, &similar, details))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, similar);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (connectivity).
 * Arg #4 is expected to be a l_int32 (maxiters).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThinConnected(lua_State *L)
{
    LL_FUNC("ThinConnected");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 connectivity = ll_check_l_int32(_fun, L, 3);
    l_int32 maxiters = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixThinConnected(pixs, type, connectivity, maxiters);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a Sela* (sela).
 * Arg #4 is expected to be a l_int32 (maxiters).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThinConnectedBySet(lua_State *L)
{
    LL_FUNC("ThinConnectedBySet");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    Sela *sela = ll_check_Sela(_fun, L, 3);
    l_int32 maxiters = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixThinConnectedBySet(pixs, type, sela, maxiters);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (d).
 * Arg #3 is expected to be a l_int32 (nlevels).
 * Arg #4 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Threshold8(lua_State *L)
{
    LL_FUNC("Threshold8");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 d = ll_check_l_int32(_fun, L, 2);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 3);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixThreshold8(pixs, d, nlevels, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_int32 (start).
 * Arg #4 is expected to be a l_int32 (end).
 * Arg #5 is expected to be a l_int32 (incr).
 * Arg #6 is expected to be a l_float32 (thresh48).
 * Arg #7 is expected to be a l_float32 (threshdiff).
 * Arg #10 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ThresholdByConnComp(lua_State *L)
{
    LL_FUNC("ThresholdByConnComp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 start = ll_check_l_int32(_fun, L, 3);
    l_int32 end = ll_check_l_int32(_fun, L, 4);
    l_int32 incr = ll_check_l_int32(_fun, L, 5);
    l_float32 thresh48 = ll_check_l_float32(_fun, L, 6);
    l_float32 threshdiff = ll_check_l_float32(_fun, L, 7);
    l_int32 globthresh = 0;
    Pix *pixd = nullptr;
    l_int32 debugflag = ll_check_l_int32(_fun, L, 10);
    if (pixThresholdByConnComp(pixs, pixm, start, end, incr, thresh48, threshdiff, &globthresh, &pixd, debugflag))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, globthresh);
    ll_push_Pix(_fun, L, pixd);
    return 2;
}

/**
 * \brief Get the thresholds for bg/fg in Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 factor.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack (result)
 */
static int
ThresholdForFgBg(lua_State *L)
{
    LL_FUNC("ThresholdForFgBg");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 fgval;
    l_int32 bgval;
    if (pixThresholdForFgBg(pixs, factor, thresh, &fgval, &bgval))
        return ll_push_nil(L);
    return ll_push_l_int32(_fun, L, fgval) +
            ll_push_l_int32(_fun, L, bgval);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (edgevals).
 * Arg #3 is expected to be a l_int32 (outdepth).
 * Arg #4 is expected to be a l_int32 (use_average).
 * Arg #5 is expected to be a l_int32 (setblack).
 * Arg #6 is expected to be a l_int32 (setwhite).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdGrayArb(lua_State *L)
{
    LL_FUNC("ThresholdGrayArb");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *edgevals = ll_check_string(_fun, L, 2);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 3);
    l_int32 use_average = ll_check_l_int32(_fun, L, 4);
    l_int32 setblack = ll_check_l_int32(_fun, L, 5);
    l_int32 setwhite = ll_check_l_int32(_fun, L, 6);
    Pix *pix = pixThresholdGrayArb(pixs, edgevals, outdepth, use_average, setblack, setwhite);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nlevels).
 * Arg #3 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdOn8bpp(lua_State *L)
{
    LL_FUNC("ThresholdOn8bpp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 2);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixThresholdOn8bpp(pixs, nlevels, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check if the pixel sum of Pix* (%pixs) is above threshold (%thresh).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ThresholdPixelSum(lua_State *L)
{
    LL_FUNC("ThresholdPixelSum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    l_int32 above = 0;
    if (pixThresholdPixelSum(pixs, thresh, &above, nullptr))
        return ll_push_nil(L);
    lua_pushboolean(L, above);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (filtertype).
 * Arg #3 is expected to be a l_int32 (edgethresh).
 * Arg #4 is expected to be a l_int32 (smoothx).
 * Arg #5 is expected to be a l_int32 (smoothy).
 * Arg #6 is expected to be a l_float32 (gamma).
 * Arg #7 is expected to be a l_int32 (minval).
 * Arg #8 is expected to be a l_int32 (maxval).
 * Arg #9 is expected to be a l_int32 (targetthresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ThresholdSpreadNorm(lua_State *L)
{
    LL_FUNC("ThresholdSpreadNorm");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 filtertype = ll_check_l_int32(_fun, L, 2);
    l_int32 edgethresh = ll_check_l_int32(_fun, L, 3);
    l_int32 smoothx = ll_check_l_int32(_fun, L, 4);
    l_int32 smoothy = ll_check_l_int32(_fun, L, 5);
    l_float32 gamma = ll_check_l_float32(_fun, L, 6);
    l_int32 minval = ll_check_l_int32(_fun, L, 7);
    l_int32 maxval = ll_check_l_int32(_fun, L, 8);
    l_int32 targetthresh = ll_check_l_int32(_fun, L, 9);
    Pix *pixth = nullptr;
    Pix *pixb = nullptr;
    Pix *pixd = nullptr;
    if (pixThresholdSpreadNorm(pixs, filtertype, edgethresh, smoothx, smoothy, gamma, minval, maxval, targetthresh, &pixth, &pixb, &pixd))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixth);
    ll_push_Pix(_fun, L, pixb);
    ll_push_Pix(_fun, L, pixd);
    return 3;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nlevels).
 * Arg #3 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdTo2bpp(lua_State *L)
{
    LL_FUNC("ThresholdTo2bpp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 2);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixThresholdTo2bpp(pixs, nlevels, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nlevels).
 * Arg #3 is expected to be a l_int32 (cmapflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdTo4bpp(lua_State *L)
{
    LL_FUNC("ThresholdTo4bpp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 2);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixThresholdTo4bpp(pixs, nlevels, cmapflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (thresh).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdToBinary(lua_State *L)
{
    LL_FUNC("ThresholdToBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixThresholdToBinary(pixs, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (threshval).
 * Arg #4 is expected to be a l_int32 (setval).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
ThresholdToValue(lua_State *L)
{
    LL_FUNC("ThresholdToValue");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 threshval = ll_check_l_int32(_fun, L, 3);
    l_int32 setval = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixThresholdToValue(pixd, pixs, threshval, setval);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (nx).
 * Arg #3 is expected to be a l_int32 (ny).
 * Arg #4 is expected to be a l_int32 (w).
 * Arg #5 is expected to be a l_int32 (h).
 * Arg #6 is expected to be a l_int32 (xoverlap).
 * Arg #7 is expected to be a l_int32 (yoverlap).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixTiling * on the Lua stack
 */
static int
TilingCreate(lua_State *L)
{
    LL_FUNC("TilingCreate");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nx = ll_check_l_int32(_fun, L, 2);
    l_int32 ny = ll_check_l_int32(_fun, L, 3);
    l_int32 w = ll_check_l_int32(_fun, L, 4);
    l_int32 h = ll_check_l_int32(_fun, L, 5);
    l_int32 xoverlap = ll_check_l_int32(_fun, L, 6);
    l_int32 yoverlap = ll_check_l_int32(_fun, L, 7);
    PixTiling *result = pixTilingCreate(pixs, nx, ny, w, h, xoverlap, yoverlap);
    return ll_push_PixTiling(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void on the Lua stack
 */
static int
TilingDestroy(lua_State *L)
{
    LL_FUNC("TilingDestroy");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    pixTilingDestroy(&pt);
    return 0;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TilingGetCount(lua_State *L)
{
    LL_FUNC("TilingGetCount");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 nx = 0;
    l_int32 ny = 0;
    if (pixTilingGetCount(pt, &nx, &ny))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, nx);
    ll_push_l_int32(_fun, L, ny);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TilingGetSize(lua_State *L)
{
    LL_FUNC("TilingGetSize");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    if (pixTilingGetSize(pt, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
TilingGetTile(lua_State *L)
{
    LL_FUNC("TilingGetTile");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 i = ll_check_l_int32(_fun, L, 2);
    l_int32 j = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixTilingGetTile(pt, i, j);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixTiling* (pt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TilingNoStripOnPaint(lua_State *L)
{
    LL_FUNC("TilingNoStripOnPaint");
    PixTiling *pt = ll_check_PixTiling(_fun, L, 1);
    l_int32 result = pixTilingNoStripOnPaint(pt);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a l_int32 (i).
 * Arg #3 is expected to be a l_int32 (j).
 * Arg #4 is expected to be a Pix* (pixs).
 * Arg #5 is expected to be a PixTiling* (pt).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
TilingPaintTile(lua_State *L)
{
    LL_FUNC("TilingPaintTile");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 i = ll_check_l_int32(_fun, L, 2);
    l_int32 j = ll_check_l_int32(_fun, L, 3);
    Pix *pixs = ll_check_Pix(_fun, L, 4);
    PixTiling *pt = ll_check_PixTiling(_fun, L, 5);
    l_int32 result = pixTilingPaintTile(pixd, i, j, pixs, pt);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * Arg #4 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Tophat(lua_State *L)
{
    LL_FUNC("Tophat");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixTophat(pixs, hsize, vsize, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Transfer all data from a Pix* (pixs) to self (pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be another Pix* (pixs).
 * Arg #3 is optional and expected to be a boolean (copytext).
 * Arg #4 is optional and expected to be a boolean (copyformat).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
TransferAllData(lua_State *L)
{
    LL_FUNC("TransferAllData");
    Pix **ppixs = ll_check_udata<Pix>(_fun, L, 2, LL_PIX);
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    int copytext = ll_check_boolean_default(_fun, L, 3, TRUE);
    int copyformat = ll_check_boolean_default(_fun, L, 4, TRUE);
    lua_pushboolean(L, 0 == pixTransferAllData(pixd, &pixs, copytext, copyformat));
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (hshift).
 * Arg #4 is expected to be a l_int32 (vshift).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
Translate(lua_State *L)
{
    LL_FUNC("Translate");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 hshift = ll_check_l_int32(_fun, L, 3);
    l_int32 vshift = ll_check_l_int32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixTranslate(pixd, pixs, hshift, vshift, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (orientflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
TwoSidedEdgeFilter(lua_State *L)
{
    LL_FUNC("TwoSidedEdgeFilter");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 orientflag = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixTwoSidedEdgeFilter(pixs, orientflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Sela* (sela).
 * Arg #3 is expected to be a l_int32 (type).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnionOfMorphOps(lua_State *L)
{
    LL_FUNC("UnionOfMorphOps");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Sela *sela = ll_check_Sela(_fun, L, 2);
    l_int32 type = ll_check_l_int32(_fun, L, 3);
    Pix *pix = pixUnionOfMorphOps(pixs, sela, type);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Convert a binary 1bpp Pix* (%pixs) to a depth bpp Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (depth).
 * Arg #2 is expected to be a boolean (invert).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
UnpackBinary(lua_State *L)
{
    LL_FUNC("UnpackBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 depth = ll_check_l_int32_default(_fun, L, 2, 8);
    l_int32 invert = ll_check_boolean(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixUnpackBinary(pixs, depth, invert));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMasking(lua_State *L)
{
    LL_FUNC("UnsharpMasking");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixUnsharpMasking(pixs, halfwidth, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMaskingFast(lua_State *L)
{
    LL_FUNC("UnsharpMaskingFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 direction = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixUnsharpMaskingFast(pixs, halfwidth, fract, direction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMaskingGray(lua_State *L)
{
    LL_FUNC("UnsharpMaskingGray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixUnsharpMaskingGray(pixs, halfwidth, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMaskingGray1D(lua_State *L)
{
    LL_FUNC("UnsharpMaskingGray1D");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 direction = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixUnsharpMaskingGray1D(pixs, halfwidth, fract, direction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMaskingGray2D(lua_State *L)
{
    LL_FUNC("UnsharpMaskingGray2D");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    Pix *pix = pixUnsharpMaskingGray2D(pixs, halfwidth, fract);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (halfwidth).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
UnsharpMaskingGrayFast(lua_State *L)
{
    LL_FUNC("UnsharpMaskingGrayFast");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 halfwidth = ll_check_l_int32(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 direction = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixUnsharpMaskingGrayFast(pixs, halfwidth, fract, direction);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
UpDownDetect(lua_State *L)
{
    LL_FUNC("UpDownDetect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 conf = 0;
    if (pixUpDownDetect(pixs, &conf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (mincount).
 * Arg #4 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
UpDownDetectDwa(lua_State *L)
{
    LL_FUNC("UpDownDetectDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 debug = ll_check_boolean_default(_fun, L, 3, FALSE);
    l_float32 conf = 0;
    if (pixUpDownDetectDwa(pixs, &conf, mincount, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a l_int32 (npixels).
 * Arg #4 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
UpDownDetectGeneral(lua_State *L)
{
    LL_FUNC("UpDownDetectGeneral");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 npixels = ll_check_l_int32(_fun, L, 3);
    l_int32 debug = ll_check_boolean_default(_fun, L, 4, FALSE);
    l_float32 conf = 0;
    if (pixUpDownDetectGeneral(pixs, &conf, mincount, npixels, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (mincount).
 * Arg #3 is expected to be a l_int32 (npixels).
 * Arg #4 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_float32 (%conf) on the Lua stack
 */
static int
UpDownDetectGeneralDwa(lua_State *L)
{
    LL_FUNC("UpDownDetectGeneralDwa");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 mincount = ll_check_l_int32(_fun, L, 2);
    l_int32 npixels = ll_check_l_int32(_fun, L, 3);
    l_int32 debug = ll_check_boolean_default(_fun, L, 4, FALSE);
    l_float32 conf = 0;
    if (pixUpDownDetectGeneralDwa(pixs, &conf, mincount, npixels, debug))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, conf);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
UsesCmapColor(lua_State *L)
{
    LL_FUNC("UsesCmapColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 color = 0;
    if (pixUsesCmapColor(pixs, &color))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, color);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (xloc).
 * Arg #4 is expected to be a l_float32 (radang).
 * Arg #5 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
VShear(lua_State *L)
{
    LL_FUNC("VShear");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 xloc = ll_check_l_int32(_fun, L, 3);
    l_float32 radang = ll_check_l_float32(_fun, L, 4);
    l_int32 incolor = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixVShear(pixd, pixs, xloc, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
VShearCenter(lua_State *L)
{
    LL_FUNC("VShearCenter");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixVShearCenter(pixd, pixs, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
VShearCorner(lua_State *L)
{
    LL_FUNC("VShearCorner");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixVShearCorner(pixd, pixs, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xloc).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
VShearIP(lua_State *L)
{
    LL_FUNC("VShearIP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xloc = ll_check_l_int32(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    l_int32 result = pixVShearIP(pixs, xloc, radang, incolor);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (xloc).
 * Arg #3 is expected to be a l_float32 (radang).
 * Arg #4 is expected to be a l_int32 (incolor).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
VShearLI(lua_State *L)
{
    LL_FUNC("VShearLI");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 xloc = ll_check_l_int32(_fun, L, 2);
    l_float32 radang = ll_check_l_float32(_fun, L, 3);
    l_int32 incolor = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixVShearLI(pixs, xloc, radang, incolor);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixg).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
VarThresholdToBinary(lua_State *L)
{
    LL_FUNC("VarThresholdToBinary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixVarThresholdToBinary(pixs, pixg);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Build the variance by column of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceByColumn(lua_State *L)
{
    LL_FUNC("VarianceByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixVarianceByColumn(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the variance by row of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceByRow(lua_State *L)
{
    LL_FUNC("VarianceByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixVarianceByRow(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the square root of the variance inside a Box* (%box) of Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is optional and, if given, expected to be a Box* (box).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceInRect(lua_State *L)
{
    LL_FUNC("VarianceInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_float32 sqrvar = 0.0f;
    if (pixVarianceInRect(pixs, box, &sqrvar))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, sqrvar);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a Pix* (pix_ma).
 * Arg #4 is expected to be a DPix* (dpix_msa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
VarianceInRectangle(lua_State *L)
{
    LL_FUNC("VarianceInRectangle");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    Pix *pix_ma = ll_check_Pix(_fun, L, 3);
    DPix *dpix_msa = ll_check_DPix(_fun, L, 4);
    l_float32 var = 0;
    l_float32 rvar = 0;
    if (pixVarianceInRectangle(pixs, box, pix_ma, dpix_msa, &var, &rvar))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, var);
    ll_push_l_float32(_fun, L, rvar);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (zbend).
 * Arg #3 is expected to be a l_int32 (zshiftt).
 * Arg #4 is expected to be a l_int32 (zshiftb).
 * Arg #5 is expected to be a l_int32 (ybendt).
 * Arg #6 is expected to be a l_int32 (ybendb).
 * Arg #7 is expected to be a l_int32 (redleft).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
WarpStereoscopic(lua_State *L)
{
    LL_FUNC("WarpStereoscopic");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 zbend = ll_check_l_int32(_fun, L, 2);
    l_int32 zshiftt = ll_check_l_int32(_fun, L, 3);
    l_int32 zshiftb = ll_check_l_int32(_fun, L, 4);
    l_int32 ybendt = ll_check_l_int32(_fun, L, 5);
    l_int32 ybendb = ll_check_l_int32(_fun, L, 6);
    l_int32 redleft = ll_check_l_int32(_fun, L, 7);
    Pix *pix = pixWarpStereoscopic(pixs, zbend, zshiftt, zshiftb, ybendt, ybendb, redleft);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * Arg #4 is expected to be a l_int32 (hasborder).
 * Arg #5 is expected to be a l_int32 (normflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
WindowedMean(lua_State *L)
{
    LL_FUNC("WindowedMean");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    l_int32 hasborder = ll_check_l_int32(_fun, L, 4);
    l_int32 normflag = ll_check_l_int32(_fun, L, 5);
    Pix *pix = pixWindowedMean(pixs, wc, hc, hasborder, normflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * Arg #4 is expected to be a l_int32 (hasborder).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
WindowedMeanSquare(lua_State *L)
{
    LL_FUNC("WindowedMeanSquare");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    l_int32 hasborder = ll_check_l_int32(_fun, L, 4);
    Pix *pix = pixWindowedMeanSquare(pixs, wc, hc, hasborder);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (wc).
 * Arg #3 is expected to be a l_int32 (hc).
 * Arg #4 is expected to be a l_int32 (hasborder).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WindowedStats(lua_State *L)
{
    LL_FUNC("WindowedStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 wc = ll_check_l_int32(_fun, L, 2);
    l_int32 hc = ll_check_l_int32(_fun, L, 3);
    l_int32 hasborder = ll_check_l_int32(_fun, L, 4);
    Pix *pixm = nullptr;
    Pix *pixms = nullptr;
    FPix *fpixv = nullptr;
    FPix *fpixrv = nullptr;
    if (pixWindowedStats(pixs, wc, hc, hasborder, &pixm, &pixms, &fpixv, &fpixrv))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixm);
    ll_push_Pix(_fun, L, pixms);
    ll_push_FPix(_fun, L, fpixv);
    ll_push_FPix(_fun, L, fpixrv);
    return 4;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixm).
 * Arg #2 is expected to be a Pix* (pixms).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WindowedVariance(lua_State *L)
{
    LL_FUNC("WindowedVariance");
    Pix *pixm = ll_check_Pix(_fun, L, 1);
    Pix *pixms = ll_check_Pix(_fun, L, 2);
    FPix *fpixv = nullptr;
    FPix *fpixrv = nullptr;
    if (pixWindowedVariance(pixm, pixms, &fpixv, &fpixrv))
        return ll_push_nil(L);
    ll_push_FPix(_fun, L, fpixv);
    ll_push_FPix(_fun, L, fpixrv);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (dir).
 * Arg #3 is expected to be a l_int32 (loc).
 * Arg #4 is expected to be a l_int32 (c1).
 * Arg #5 is expected to be a l_int32 (c2).
 * Arg #6 is expected to be a l_int32 (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WindowedVarianceOnLine(lua_State *L)
{
    LL_FUNC("WindowedVarianceOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dir = ll_check_l_int32(_fun, L, 2);
    l_int32 loc = ll_check_l_int32(_fun, L, 3);
    l_int32 c1 = ll_check_l_int32(_fun, L, 4);
    l_int32 c2 = ll_check_l_int32(_fun, L, 5);
    l_int32 size = ll_check_l_int32(_fun, L, 6);
    Numa *nad = nullptr;
    if (pixWindowedVarianceOnLine(pixs, dir, loc, c1, c2, size, &nad))
        return ll_push_nil(L);
    ll_push_Numa(_fun, L, nad);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (minwidth).
 * Arg #3 is expected to be a l_int32 (minheight).
 * Arg #4 is expected to be a l_int32 (maxwidth).
 * Arg #5 is expected to be a l_int32 (maxheight).
 * Arg #8 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WordBoxesByDilation(lua_State *L)
{
    LL_FUNC("WordBoxesByDilation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 minwidth = ll_check_l_int32(_fun, L, 2);
    l_int32 minheight = ll_check_l_int32(_fun, L, 3);
    l_int32 maxwidth = ll_check_l_int32(_fun, L, 4);
    l_int32 maxheight = ll_check_l_int32(_fun, L, 5);
    Boxa *boxa = nullptr;
    l_int32 size = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 8);
    if (pixWordBoxesByDilation(pixs, minwidth, minheight, maxwidth, maxheight, &boxa, &size, pixadb))
        return ll_push_nil(L);
    ll_push_Boxa(_fun, L, boxa);
    ll_push_l_int32(_fun, L, size);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a Pixa* (pixadb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WordMaskByDilation(lua_State *L)
{
    LL_FUNC("WordMaskByDilation");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = nullptr;
    l_int32 size = 0;
    Pixa *pixadb = ll_check_Pixa(_fun, L, 4);
    if (pixWordMaskByDilation(pixs, &pixm, &size, pixadb))
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixm);
    ll_push_l_int32(_fun, L, size);
    return 2;
}

/**
 * \brief Write the Pix* (%pix) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a string with the input format name (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 format = ll_check_input_format(_fun, L, 3, IFF_DEFAULT);
    return ll_push_boolean(_fun, L, 0 == pixWrite(filename, pix, format));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * Arg #2 is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteAutoFormat(lua_State *L)
{
    LL_FUNC("WriteAutoFormat");
    const char *filename = ll_check_string(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteAutoFormat(filename, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (fname).
 * Arg #3 is expected to be a l_int32 (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteDebug(lua_State *L)
{
    LL_FUNC("WriteDebug");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *fname = ll_check_string(_fun, L, 2);
    l_int32 format = ll_check_input_format(_fun, L, 3, IFF_DEFAULT);
    return ll_push_boolean(_fun, L, 0 == pixWriteDebug(fname, pix, format));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (progressive).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteImpliedFormat(lua_State *L)
{
    LL_FUNC("WriteImpliedFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 progressive = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteImpliedFormat(filename, pix, quality, progressive));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (nlevels).
 * Arg #5 is expected to be a l_int32 (hint).
 * Arg #6 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteJp2k(lua_State *L)
{
    LL_FUNC("WriteJp2k");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 4);
    l_int32 hint = ll_check_l_int32(_fun, L, 5);
    l_int32 debug = ll_check_boolean_default(_fun, L, 6, FALSE);
    return ll_push_boolean(_fun, L, 0 == pixWriteJp2k(filename, pix, quality, nlevels, hint, debug));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (progressive).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean the Lua stack
 */
static int
WriteJpeg(lua_State *L)
{
    LL_FUNC("WriteJpeg");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 progressive = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteJpeg(filename, pix, quality, progressive));
}

/**
 * \brief Write the Pix* (%pix) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be a string with the input format name (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 format = ll_check_input_format(_fun, L, 2, IFF_DEFAULT);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMem(&data, &size, pix, format))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%fdata, %fsize) on the Lua stack
 */
static int
WriteMemBmp(lua_State *L)
{
    LL_FUNC("WriteMemBmp");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint8 *fdata = nullptr;
    size_t fsize = 0;
    if (pixWriteMemBmp(&fdata, &fsize, pixs))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(fdata), fsize);
    ll_free(fdata);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemGif(lua_State *L)
{
    LL_FUNC("WriteMemGif");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemGif(&data, &size, pix))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (quality).
 * Arg #3 is expected to be a l_int32 (nlevels).
 * Arg #4 is expected to be a l_int32 (hint).
 * Arg #5 is expected to be a l_int32 (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemJp2k(lua_State *L)
{
    LL_FUNC("WriteMemJp2k");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 quality = ll_check_l_int32(_fun, L, 2);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 3);
    l_int32 hint = ll_check_l_int32(_fun, L, 4);
    l_int32 debug = ll_check_boolean_default(_fun, L, 5, FALSE);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemJp2k(&data, &size, pix, quality, nlevels, hint, debug))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #3 is expected to be a Pix* (pix).
 * Arg #4 is expected to be a l_int32 (quality).
 * Arg #5 is expected to be a l_int32 (progressive).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemJpeg(lua_State *L)
{
    LL_FUNC("WriteMemJpeg");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 quality = ll_check_l_int32(_fun, L, 2);
    l_int32 progressive = ll_check_l_int32(_fun, L, 3);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemJpeg(&data, &size, pix, quality, progressive))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (res).
 * Arg #4 is expected to be a l_float32 (scale).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemPS(lua_State *L)
{
    LL_FUNC("WriteMemPS");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 res = ll_check_l_int32(_fun, L, 3);
    l_float32 scale = ll_check_l_float32(_fun, L, 4);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemPS(&data, &size, pix, box, res, scale))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemPam(lua_State *L)
{
    LL_FUNC("WriteMemPam");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemPam(&data, &size, pix))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a string (title).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %nbytes) on the Lua stack
 */
static int
WriteMemPdf(lua_State *L)
{
    LL_FUNC("WriteMemPdf");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    const char *title = ll_check_string(_fun, L, 3);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (pixWriteMemPdf(&data, &nbytes, pix, res, title))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), nbytes);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_float32 (gamma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%filedata, %filesize) on the Lua stack
 */
static int
WriteMemPng(lua_State *L)
{
    LL_FUNC("WriteMemPng");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_float32 gamma = ll_check_l_float32(_fun, L, 2);
    l_uint8 *filedata = nullptr;
    size_t filesize = 0;
    if (pixWriteMemPng(&filedata, &filesize, pix, gamma))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(filedata), filesize);
    ll_free(filedata);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemPnm(lua_State *L)
{
    LL_FUNC("WriteMemPnm");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemPnm(&data, &size, pix))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemSpix(lua_State *L)
{
    LL_FUNC("WriteMemSpix");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemSpix(&data, &size, pix))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (comptype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemTiff(lua_State *L)
{
    LL_FUNC("WriteMemTiff");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 comptype = ll_check_l_int32(_fun, L, 2);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemTiff(&data, &size, pix, comptype))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (comptype).
 * Arg #3 is expected to be a Numa* (natags).
 * Arg #4 is expected to be a Sarray* (savals).
 * Arg #5 is expected to be a Sarray* (satypes).
 * Arg #6 is expected to be a Numa* (nasizes).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%data, %size) on the Lua stack
 */
static int
WriteMemTiffCustom(lua_State *L)
{
    LL_FUNC("WriteMemTiffCustom");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 comptype = ll_check_l_int32(_fun, L, 2);
    Numa *natags = ll_check_Numa(_fun, L, 3);
    Sarray *savals = ll_unpack_Sarray(_fun, L, 4, nullptr);
    Sarray *satypes = ll_unpack_Sarray(_fun, L, 5, nullptr);
    Numa *nasizes = ll_check_Numa(_fun, L, 6);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixWriteMemTiffCustom(&data, &size, pix, comptype, natags, savals, satypes, nasizes))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (quality).
 * Arg #3 is expected to be a l_int32 (lossless).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring (%encdata, %encsize) on the Lua stack
 */
static int
WriteMemWebP(lua_State *L)
{
    LL_FUNC("WriteMemWebP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 quality = ll_check_l_int32(_fun, L, 2);
    l_int32 lossless = ll_check_l_int32(_fun, L, 3);
    l_uint8 *encdata = nullptr;
    size_t encsize = 0;
    if (pixWriteMemWebP(&encdata, &encsize, pixs, quality, lossless))
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(encdata), encsize);
    ll_free(encdata);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixb).
 * Arg #2 is expected to be a Pix* (pixc).
 * Arg #3 is expected to be a l_float32 (scale).
 * Arg #4 is expected to be a l_int32 (pageno).
 * Arg #5 is expected to be a string (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMixedToPS(lua_State *L)
{
    LL_FUNC("WriteMixedToPS");
    Pix *pixb = ll_check_Pix(_fun, L, 1);
    Pix *pixc = ll_check_Pix(_fun, L, 2);
    l_float32 scale = ll_check_l_float32(_fun, L, 3);
    l_int32 pageno = ll_check_l_int32(_fun, L, 4);
    const char *fileout = ll_check_string(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixWriteMixedToPS(pixb, pixc, scale, pageno, fileout));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filein).
 * Arg #2 is expected to be a string (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WritePSEmbed(lua_State *L)
{
    LL_FUNC("WritePSEmbed");
    const char *filein = ll_check_string(_fun, L, 1);
    const char *fileout = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWritePSEmbed(filein, fileout));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_float32 (gamma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WritePng(lua_State *L)
{
    LL_FUNC("WritePng");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixWritePng(filename, pix, gamma));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Pix* (pixm).
 * Arg #3 is expected to be a l_float32 (textscale).
 * Arg #4 is expected to be a l_float32 (imagescale).
 * Arg #5 is expected to be a l_int32 (threshold).
 * Arg #6 is expected to be a l_int32 (pageno).
 * Arg #7 is expected to be a string (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteSegmentedPageToPS(lua_State *L)
{
    LL_FUNC("WriteSegmentedPageToPS");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_float32 textscale = ll_check_l_float32(_fun, L, 3);
    l_float32 imagescale = ll_check_l_float32(_fun, L, 4);
    l_int32 threshold = ll_check_l_int32(_fun, L, 5);
    l_int32 pageno = ll_check_l_int32(_fun, L, 6);
    const char *fileout = ll_check_string(_fun, L, 7);
    return ll_push_boolean(_fun, L, 0 == pixWriteSegmentedPageToPS(pixs, pixm, textscale, imagescale, threshold, pageno, fileout));
}

/**
 * \brief Write the Pix* (%pix) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a string with the input format name (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 format = ll_check_input_format(_fun, L, 3, IFF_DEFAULT);
    return ll_push_boolean(_fun, L, 0 == pixWriteStream(stream->f, pix, format));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamAsciiPnm(lua_State *L)
{
    LL_FUNC("WriteStreamAsciiPnm");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamAsciiPnm(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamBmp(lua_State *L)
{
    LL_FUNC("WriteStreamBmp");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamBmp(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamGif(lua_State *L)
{
    LL_FUNC("WriteStreamGif");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamGif(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (nlevels).
 * Arg #5 is expected to be a l_int32 (hint).
 * Arg #6 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamJp2k(lua_State *L)
{
    LL_FUNC("WriteStreamJp2k");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 nlevels = ll_check_l_int32(_fun, L, 4);
    l_int32 hint = ll_check_l_int32(_fun, L, 5);
    l_int32 debug = ll_check_boolean_default(_fun, L, 6, FALSE);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamJp2k(stream->f, pix, quality, nlevels, hint, debug));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (progressive).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamJpeg(lua_State *L)
{
    LL_FUNC("WriteStreamJpeg");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 progressive = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamJpeg(stream->f, pixs, quality, progressive));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a Box* (box).
 * Arg #4 is expected to be a l_int32 (res).
 * Arg #5 is expected to be a l_float32 (scale).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamPS(lua_State *L)
{
    LL_FUNC("WriteStreamPS");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_int32 res = ll_check_l_int32(_fun, L, 4);
    l_float32 scale = ll_check_l_float32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamPS(stream->f, pix, box, res, scale));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamPam(lua_State *L)
{
    LL_FUNC("WriteStreamPam");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamPam(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (res).
 * Arg #4 is expected to be a string (title).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamPdf(lua_State *L)
{
    LL_FUNC("WriteStreamPdf");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 res = ll_check_l_int32(_fun, L, 3);
    const char *title = ll_check_string(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamPdf(stream->f, pix, res, title));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_float32 (gamma).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamPng(lua_State *L)
{
    LL_FUNC("WriteStreamPng");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_float32 gamma = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamPng(stream->f, pix, gamma));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamPnm(lua_State *L)
{
    LL_FUNC("WriteStreamPnm");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamPnm(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamSpix(lua_State *L)
{
    LL_FUNC("WriteStreamSpix");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamSpix(stream->f, pix));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (comptype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamTiff(lua_State *L)
{
    LL_FUNC("WriteStreamTiff");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamTiff(stream->f, pix, comptype));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a string (modestr).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamTiffWA(lua_State *L)
{
    LL_FUNC("WriteStreamTiffWA");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    const char *modestr = ll_check_string(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamTiffWA(stream->f, pix, comptype, modestr));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (lossless).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStreamWebP(lua_State *L)
{
    LL_FUNC("WriteStreamWebP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 lossless = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteStreamWebP(stream->f, pixs, quality, lossless));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (res).
 * Arg #4 is expected to be a l_float32 (scale).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string (%str) on the Lua stack
 */
static int
WriteStringPS(lua_State *L)
{
    LL_FUNC("WriteStringPS");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 res = ll_check_l_int32(_fun, L, 3);
    l_float32 scale = ll_check_l_float32(_fun, L, 4);
    char *str = pixWriteStringPS(pixs, box, res, scale);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a Box* (box).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a string (modestr).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteTiff(lua_State *L)
{
    LL_FUNC("WriteTiff");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    const char *modestr = ll_check_string(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteTiff(filename, pix, comptype, modestr));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_int32 (comptype).
 * Arg #4 is expected to be a string (modestr).
 * Arg #5 is expected to be a Numa* (natags).
 * Arg #6 is expected to be a Sarray* (savals).
 * Arg #7 is expected to be a Sarray* (satypes).
 * Arg #8 is expected to be a Numa* (nasizes).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteTiffCustom(lua_State *L)
{
    LL_FUNC("WriteTiffCustom");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 comptype = ll_check_l_int32(_fun, L, 3);
    const char *modestr = ll_check_string(_fun, L, 4);
    Numa *natags = ll_check_Numa(_fun, L, 5);
    Sarray *savals = ll_unpack_Sarray(_fun, L, 6, nullptr);
    Sarray *satypes = ll_unpack_Sarray(_fun, L, 7, nullptr);
    Numa *nasizes = ll_check_Numa(_fun, L, 8);
    return ll_push_boolean(_fun, L, 0 == pixWriteTiffCustom(filename, pix, comptype, modestr, natags, savals, satypes, nasizes));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a string (filename).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (lossless).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteWebP(lua_State *L)
{
    LL_FUNC("WriteWebP");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 lossless = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == pixWriteWebP(filename, pixs, quality, lossless));
}

/**
 * \brief Check if all pixels in Pix* (%pixs) are 0.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Zero(lua_State *L)
{
    LL_FUNC("Zero");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 empty = 0;
    if (pixZero(pixs, &empty))
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, empty);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a string (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FHMTGen_1(lua_State *L)
{
    LL_FUNC("FHMTGen_1");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    const char *selname = ll_check_string(_fun, L, 3);
    Pix *pix = pixFHMTGen_1(pixd, pixs, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (operation).
 * Arg #4 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FMorphopGen_1(lua_State *L)
{
    LL_FUNC("FMorphopGen_1");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 operation = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    /* XXX: deconstify */
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Pix *pix = pixFMorphopGen_1(pixd, pixs, operation, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (operation).
 * Arg #4 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
FMorphopGen_2(lua_State *L)
{
    LL_FUNC("FMorphopGen_2");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 operation = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Pix *pix = pixFMorphopGen_2(pixd, pixs, operation, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a string (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
HMTDwa_1(lua_State *L)
{
    LL_FUNC("HMTDwa_1");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    const char *selname = ll_check_string(_fun, L, 3);
    Pix *pix = pixHMTDwa_1(pixd, pixs, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (operation).
 * Arg #4 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphDwa_1(lua_State *L)
{
    LL_FUNC("MorphDwa_1");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 operation = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    /* XXX: deconstify */
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Pix *pix = pixMorphDwa_1(pixd, pixs, operation, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a l_int32 (operation).
 * Arg #4 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
MorphDwa_2(lua_State *L)
{
    LL_FUNC("MorphDwa_2");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 operation = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    /* XXX: deconstify */
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Pix *pix = pixMorphDwa_2(pixd, pixs, operation, selname);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PIX.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pix* contained in the user data
 */
Pix *
ll_check_Pix(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pix>(_fun, L, arg, LL_PIX);
}

/**
 * \brief Optionally expect a LL_PIX at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pix* contained in the user data
 */
Pix *
ll_check_Pix_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Pix(_fun, L, arg);
}
/**
 * \brief Push PIX to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param pix pointer to the PIX
 * \return 1 Pix* on the Lua stack
 */
int
ll_push_Pix(const char *_fun, lua_State *L, Pix *pix)
{
    if (!pix)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PIX, pix);
}
/**
 * \brief Create and push a new Pix*.
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
int
ll_new_Pix(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                            Destroy},           /* garbage collector */
        {"__new",                           Create},            /* new Pix */
        {"__sub",                           Subtract},          /* Pix* = Pix* - Pix* */
        {"__bnot",                          Invert},            /* Pix* = not Pix* */
        {"__band",                          And},               /* Pix* = Pix* and Pix */
        {"__bor",                           Or},                /* Pix* = Pix* or Pix */
        {"__bxor",                          Xor},               /* Pix* = Pix* xor Pix */
        {"__tostring",                      toString},
        {"AbsDiffByColumn",                 AbsDiffByColumn},
        {"AbsDiffByRow",                    AbsDiffByRow},
        {"AbsDiffInRect",                   AbsDiffInRect},
        {"AbsDiffOnLine",                   AbsDiffOnLine},
        {"AbsDifference",                   AbsDifference},
        {"Accumulate",                      Accumulate},
        {"AccumulateSamples",               AccumulateSamples},
        {"AdaptThresholdToBinary",          AdaptThresholdToBinary},
        {"AdaptThresholdToBinaryGen",       AdaptThresholdToBinaryGen},
        {"AddAlphaTo1bpp",                  AddAlphaTo1bpp},
        {"AddAlphaToBlend",                 AddAlphaToBlend},
        {"AddBlackOrWhiteBorder",           AddBlackOrWhiteBorder},
        {"AddBorder",                       AddBorder},
        {"AddBorderGeneral",                AddBorderGeneral},
        {"AddConstantGray",                 AddConstantGray},
        {"AddContinuedBorder",              AddContinuedBorder},
        {"AddGaussianNoise",                AddGaussianNoise},
        {"AddGray",                         AddGray},
        {"AddGrayColormap8",                AddGrayColormap8},
        {"AddMinimalGrayColormap8",         AddMinimalGrayColormap8},
        {"AddMirroredBorder",               AddMirroredBorder},
        {"AddMixedBorder",                  AddMixedBorder},
        {"AddRGB",                          AddRGB},
        {"AddRepeatedBorder",               AddRepeatedBorder},
        {"AddSingleTextblock",              AddSingleTextblock},
        {"AddText",                         AddText},
        {"AddTextlines",                    AddTextlines},
        {"AddWithIndicator",                AddWithIndicator},
        {"Affine",                          Affine},
        {"AffineColor",                     AffineColor},
        {"AffineGray",                      AffineGray},
        {"AffinePta",                       AffinePta},
        {"AffinePtaColor",                  AffinePtaColor},
        {"AffinePtaGray",                   AffinePtaGray},
        {"AffinePtaWithAlpha",              AffinePtaWithAlpha},
        {"AffineSampled",                   AffineSampled},
        {"AffineSampledPta",                AffineSampledPta},
        {"AffineSequential",                AffineSequential},
        {"AlphaBlendUniform",               AlphaBlendUniform},
        {"AlphaIsOpaque",                   AlphaIsOpaque},
        {"And",                             And},
        {"ApplyInvBackgroundGrayMap",       ApplyInvBackgroundGrayMap},
        {"ApplyInvBackgroundRGBMap",        ApplyInvBackgroundRGBMap},
        {"ApplyLocalThreshold",             ApplyLocalThreshold},
        {"ApplyVariableGrayMap",            ApplyVariableGrayMap},
        {"AssignToNearestColor",            AssignToNearestColor},
        {"AverageByColumn",                 AverageByColumn},
        {"AverageByRow",                    AverageByRow},
        {"AverageInRect",                   AverageInRect},
        {"AverageIntensityProfile",         AverageIntensityProfile},
        {"AverageOnLine",                   AverageOnLine},
        {"BackgroundNorm",                  BackgroundNorm},
        {"BackgroundNormFlex",              BackgroundNormFlex},
        {"BackgroundNormGrayArray",         BackgroundNormGrayArray},
        {"BackgroundNormGrayArrayMorph",    BackgroundNormGrayArrayMorph},
        {"BackgroundNormMorph",             BackgroundNormMorph},
        {"BackgroundNormRGBArrays",         BackgroundNormRGBArrays},
        {"BackgroundNormRGBArraysMorph",    BackgroundNormRGBArraysMorph},
        {"BackgroundNormSimple",            BackgroundNormSimple},
        {"BestCorrelation",                 BestCorrelation},
        {"Bilateral",                       Bilateral},
        {"BilateralExact",                  BilateralExact},
        {"BilateralGray",                   BilateralGray},
        {"BilateralGrayExact",              BilateralGrayExact},
        {"Bilinear",                        Bilinear},
        {"BilinearColor",                   BilinearColor},
        {"BilinearGray",                    BilinearGray},
        {"BilinearPta",                     BilinearPta},
        {"BilinearPtaColor",                BilinearPtaColor},
        {"BilinearPtaGray",                 BilinearPtaGray},
        {"BilinearPtaWithAlpha",            BilinearPtaWithAlpha},
        {"BilinearSampled",                 BilinearSampled},
        {"BilinearSampledPta",              BilinearSampledPta},
        {"Blend",                           Blend},
        {"BlendBackgroundToColor",          BlendBackgroundToColor},
        {"BlendBoxaRandom",                 BlendBoxaRandom},
        {"BlendCmap",                       BlendCmap},
        {"BlendColor",                      BlendColor},
        {"BlendColorByChannel",             BlendColorByChannel},
        {"BlendGray",                       BlendGray},
        {"BlendGrayAdapt",                  BlendGrayAdapt},
        {"BlendGrayInverse",                BlendGrayInverse},
        {"BlendHardLight",                  BlendHardLight},
        {"BlendInRect",                     BlendInRect},
        {"BlendMask",                       BlendMask},
        {"BlendWithGrayMask",               BlendWithGrayMask},
        {"BlockBilateralExact",             BlockBilateralExact},
        {"Blockconv",                       Blockconv},
        {"BlockconvAccum",                  BlockconvAccum},
        {"BlockconvGray",                   BlockconvGray},
        {"BlockconvGrayTile",               BlockconvGrayTile},
        {"BlockconvGrayUnnormalized",       BlockconvGrayUnnormalized},
        {"BlockconvTiled",                  BlockconvTiled},
        {"Blockrank",                       Blockrank},
        {"Blocksum",                        Blocksum},
        {"CensusTransform",                 CensusTransform},
        {"Centroid",                        Centroid},
        {"Centroid8",                       Centroid8},
        {"ChangeRefcount",                  ChangeRefcount},
        {"ChooseOutputFormat",              ChooseOutputFormat},
        {"CleanBackgroundToWhite",          CleanBackgroundToWhite},
        {"CleanupByteProcessing",           CleanupByteProcessing},
        {"ClearAll",                        ClearAll},
        {"ClearInRect",                     ClearInRect},
        {"ClearPixel",                      ClearPixel},
        {"ClipBoxToEdges",                  ClipBoxToEdges},
        {"ClipBoxToForeground",             ClipBoxToForeground},
        {"ClipMasked",                      ClipMasked},
        {"ClipRectangle",                   ClipRectangle},
        {"ClipRectangles",                  ClipRectangles},
        {"ClipToForeground",                ClipToForeground},
        {"Clone",                           Clone},
        {"Close",                           Close},
        {"CloseBrick",                      CloseBrick},
        {"CloseBrickDwa",                   CloseBrickDwa},
        {"CloseCompBrick",                  CloseCompBrick},
        {"CloseCompBrickDwa",               CloseCompBrickDwa},
        {"CloseCompBrickExtendDwa",         CloseCompBrickExtendDwa},
        {"CloseGeneralized",                CloseGeneralized},
        {"CloseGray",                       CloseGray},
        {"CloseGray3",                      CloseGray3},
        {"CloseSafe",                       CloseSafe},
        {"CloseSafeBrick",                  CloseSafeBrick},
        {"CloseSafeCompBrick",              CloseSafeCompBrick},
        {"ColorContent",                    ColorContent},
        {"ColorFraction",                   ColorFraction},
        {"ColorGray",                       ColorGray},
        {"ColorGrayCmap",                   ColorGrayCmap},
        {"ColorGrayMasked",                 ColorGrayMasked},
        {"ColorGrayMaskedCmap",             ColorGrayMaskedCmap},
        {"ColorGrayRegions",                ColorGrayRegions},
        {"ColorGrayRegionsCmap",            ColorGrayRegionsCmap},
        {"ColorMagnitude",                  ColorMagnitude},
        {"ColorMorph",                      ColorMorph},
        {"ColorMorphSequence",              ColorMorphSequence},
        {"ColorSegment",                    ColorSegment},
        {"ColorSegmentClean",               ColorSegmentClean},
        {"ColorSegmentCluster",             ColorSegmentCluster},
        {"ColorSegmentRemoveColors",        ColorSegmentRemoveColors},
        {"ColorShiftRGB",                   ColorShiftRGB},
        {"ColorizeGray",                    ColorizeGray},
        {"ColorsForQuantization",           ColorsForQuantization},
        {"ColumnStats",                     ColumnStats},
        {"CombineMasked",                   CombineMasked},
        {"CombineMaskedGeneral",            CombineMaskedGeneral},
        {"CompareBinary",                   CompareBinary},
        {"CompareGray",                     CompareGray},
        {"CompareGrayByHisto",              CompareGrayByHisto},
        {"CompareGrayOrRGB",                CompareGrayOrRGB},
        {"ComparePhotoRegionsByHisto",      ComparePhotoRegionsByHisto},
        {"CompareRGB",                      CompareRGB},
        {"CompareRankDifference",           CompareRankDifference},
        {"CompareTiled",                    CompareTiled},
        {"CompareWithTranslation",          CompareWithTranslation},
        {"ComponentFunction",               ComponentFunction},
        {"ConformsToRectangle",             ConformsToRectangle},
        {"ConnComp",                        ConnComp},
        {"ConnCompAreaTransform",           ConnCompAreaTransform},
        {"ConnCompBB",                      ConnCompBB},
        {"ConnCompIncrAdd",                 ConnCompIncrAdd},
        {"ConnCompIncrInit",                ConnCompIncrInit},
        {"ConnCompPixa",                    ConnCompPixa},
        {"ConnCompTransform",               ConnCompTransform},
        {"ContrastNorm",                    ContrastNorm},
        {"ContrastTRC",                     ContrastTRC},
        {"ContrastTRCMasked",               ContrastTRCMasked},
        {"Convert16To8",                    Convert16To8},
        {"Convert1To16",                    Convert1To16},
        {"Convert1To2",                     Convert1To2},
        {"Convert1To2Cmap",                 Convert1To2Cmap},
        {"Convert1To32",                    Convert1To32},
        {"Convert1To4",                     Convert1To4},
        {"Convert1To4Cmap",                 Convert1To4Cmap},
        {"Convert1To8",                     Convert1To8},
        {"Convert1To8Cmap",                 Convert1To8Cmap},
        {"Convert24To32",                   Convert24To32},
        {"Convert2To8",                     Convert2To8},
        {"Convert32To16",                   Convert32To16},
        {"Convert32To24",                   Convert32To24},
        {"Convert32To8",                    Convert32To8},
        {"Convert4To8",                     Convert4To8},
        {"Convert8To16",                    Convert8To16},
        {"Convert8To2",                     Convert8To2},
        {"Convert8To32",                    Convert8To32},
        {"Convert8To4",                     Convert8To4},
        {"ConvertCmapTo1",                  ConvertCmapTo1},
        {"ConvertColorToSubpixelRGB",       ConvertColorToSubpixelRGB},
        {"ConvertForPSWrap",                ConvertForPSWrap},
        {"ConvertGrayToColormap",           ConvertGrayToColormap},
        {"ConvertGrayToColormap8",          ConvertGrayToColormap8},
        {"ConvertGrayToFalseColor",         ConvertGrayToFalseColor},
        {"ConvertGrayToSubpixelRGB",        ConvertGrayToSubpixelRGB},
        {"ConvertHSVToRGB",                 ConvertHSVToRGB},
        {"ConvertLossless",                 ConvertLossless},
        {"ConvertRGBToBinaryArb",           ConvertRGBToBinaryArb},
        {"ConvertRGBToColormap",            ConvertRGBToColormap},
        {"ConvertRGBToGray",                ConvertRGBToGray},
        {"ConvertRGBToGrayArb",             ConvertRGBToGrayArb},
        {"ConvertRGBToGrayFast",            ConvertRGBToGrayFast},
        {"ConvertRGBToGrayMinMax",          ConvertRGBToGrayMinMax},
        {"ConvertRGBToGraySatBoost",        ConvertRGBToGraySatBoost},
        {"ConvertRGBToHSV",                 ConvertRGBToHSV},
        {"ConvertRGBToHue",                 ConvertRGBToHue},
        {"ConvertRGBToLAB",                 ConvertRGBToLAB},
        {"ConvertRGBToLuminance",           ConvertRGBToLuminance},
        {"ConvertRGBToSaturation",          ConvertRGBToSaturation},
        {"ConvertRGBToValue",               ConvertRGBToValue},
        {"ConvertRGBToXYZ",                 ConvertRGBToXYZ},
        {"ConvertRGBToYUV",                 ConvertRGBToYUV},
        {"ConvertTo1",                      ConvertTo1},
        {"ConvertTo16",                     ConvertTo16},
        {"ConvertTo1BySampling",            ConvertTo1BySampling},
        {"ConvertTo2",                      ConvertTo2},
        {"ConvertTo32",                     ConvertTo32},
        {"ConvertTo32BySampling",           ConvertTo32BySampling},
        {"ConvertTo4",                      ConvertTo4},
        {"ConvertTo8",                      ConvertTo8},
        {"ConvertTo8BySampling",            ConvertTo8BySampling},
        {"ConvertTo8Colormap",              ConvertTo8Colormap},
        {"ConvertTo8Or32",                  ConvertTo8Or32},
        {"ConvertToDPix",                   ConvertToDPix},
        {"ConvertToFPix",                   ConvertToFPix},
        {"ConvertToPdf",                    ConvertToPdf},
        {"ConvertToPdfData",                ConvertToPdfData},
        {"ConvertToPdfDataSegmented",       ConvertToPdfDataSegmented},
        {"ConvertToPdfSegmented",           ConvertToPdfSegmented},
        {"ConvertToSubpixelRGB",            ConvertToSubpixelRGB},
        {"ConvertYUVToRGB",                 ConvertYUVToRGB},
        {"Convolve",                        Convolve},
        {"ConvolveRGB",                     ConvolveRGB},
        {"ConvolveRGBSep",                  ConvolveRGBSep},
        {"ConvolveSep",                     ConvolveSep},
        {"ConvolveWithBias",                ConvolveWithBias},
        {"Copy",                            Copy},
        {"CopyBorder",                      CopyBorder},
        {"CopyColormap",                    CopyColormap},
        {"CopyDimensions",                  CopyDimensions},
        {"CopyInputFormat",                 CopyInputFormat},
        {"CopyRGBComponent",                CopyRGBComponent},
        {"CopyResolution",                  CopyResolution},
        {"CopySpp",                         CopySpp},
        {"CopyText",                        CopyText},
        {"CorrelationBinary",               CorrelationBinary},
        {"CorrelationScore",                CorrelationScore},
        {"CorrelationScoreShifted",         CorrelationScoreShifted},
        {"CorrelationScoreSimple",          CorrelationScoreSimple},
        {"CorrelationScoreThresholded",     CorrelationScoreThresholded},
        {"CountArbInRect",                  CountArbInRect},
        {"CountByColumn",                   CountByColumn},
        {"CountByRow",                      CountByRow},
        {"CountConnComp",                   CountConnComp},
        {"CountPixels",                     CountPixels},
        {"CountPixelsByColumn",             CountPixelsByColumn},
        {"CountPixelsByRow",                CountPixelsByRow},
        {"CountPixelsInRect",               CountPixelsInRect},
        {"CountPixelsInRow",                CountPixelsInRow},
        {"CountRGBColors",                  CountRGBColors},
        {"CountTextColumns",                CountTextColumns},
        {"Create",                          Create},
        {"CreateFromPixcomp",               CreateFromPixcomp},
        {"CreateHeader",                    CreateHeader},
        {"CreateNoInit",                    CreateNoInit},
        {"CreateRGBImage",                  CreateRGBImage},
        {"CreateTemplate",                  CreateTemplate},
        {"CreateTemplateNoInit",            CreateTemplateNoInit},
        {"CropAlignedToCentroid",           CropAlignedToCentroid},
        {"CropToMatch",                     CropToMatch},
        {"CropToSize",                      CropToSize},
        {"DarkenGray",                      DarkenGray},
        {"DecideIfPhotoImage",              DecideIfPhotoImage},
        {"DecideIfTable",                   DecideIfTable},
        {"DecideIfText",                    DecideIfText},
        {"DeserializeFromMemory",           DeserializeFromMemory},
        {"Deskew",                          Deskew},
        {"DeskewBarcode",                   DeskewBarcode},
        {"DeskewBoth",                      DeskewBoth},
        {"DeskewGeneral",                   DeskewGeneral},
        {"DeskewLocal",                     DeskewLocal},
        {"Destroy",                         Destroy},
        {"DestroyColormap",                 DestroyColormap},
        {"Dilate",                          Dilate},
        {"DilateBrick",                     DilateBrick},
        {"DilateBrickDwa",                  DilateBrickDwa},
        {"DilateCompBrick",                 DilateCompBrick},
        {"DilateCompBrickDwa",              DilateCompBrickDwa},
        {"DilateCompBrickExtendDwa",        DilateCompBrickExtendDwa},
        {"DilateGray",                      DilateGray},
        {"DilateGray3",                     DilateGray3},
        {"Display",                         Display},
        {"DisplayColorArray",               DisplayColorArray},
        {"DisplayDiffBinary",               DisplayDiffBinary},
        {"DisplayHitMissSel",               DisplayHitMissSel},
        {"DisplayLayersRGBA",               DisplayLayersRGBA},
        {"DisplayMatchedPattern",           DisplayMatchedPattern},
        {"DisplayPta",                      DisplayPta},
        {"DisplayPtaPattern",               DisplayPtaPattern},
        {"DisplayPtaa",                     DisplayPtaa},
        {"DisplayPtaaPattern",              DisplayPtaaPattern},
        {"DisplayWithTitle",                DisplayWithTitle},
        {"DisplayWrite",                    DisplayWrite},
        {"DistanceFunction",                DistanceFunction},
        {"DitherTo2bpp",                    DitherTo2bpp},
        {"DitherTo2bppSpec",                DitherTo2bppSpec},
        {"DitherToBinary",                  DitherToBinary},
        {"DitherToBinarySpec",              DitherToBinarySpec},
        {"DrawBoxa",                        DrawBoxa},
        {"DrawBoxaRandom",                  DrawBoxaRandom},
        {"EmbedForRotation",                EmbedForRotation},
        {"EndianByteSwap",                  EndianByteSwap},
        {"EndianByteSwapNew",               EndianByteSwapNew},
        {"EndianTwoByteSwap",               EndianTwoByteSwap},
        {"EndianTwoByteSwapNew",            EndianTwoByteSwapNew},
        {"Equal",                           Equal},
        {"EqualWithAlpha",                  EqualWithAlpha},
        {"EqualWithCmap",                   EqualWithCmap},
        {"EqualizeTRC",                     EqualizeTRC},
        {"Erode",                           Erode},
        {"ErodeBrick",                      ErodeBrick},
        {"ErodeBrickDwa",                   ErodeBrickDwa},
        {"ErodeCompBrick",                  ErodeCompBrick},
        {"ErodeCompBrickDwa",               ErodeCompBrickDwa},
        {"ErodeCompBrickExtendDwa",         ErodeCompBrickExtendDwa},
        {"ErodeGray",                       ErodeGray},
        {"ErodeGray3",                      ErodeGray3},
        {"EstimateBackground",              EstimateBackground},
        {"ExpandBinaryPower2",              ExpandBinaryPower2},
        {"ExpandBinaryReplicate",           ExpandBinaryReplicate},
        {"ExpandReplicate",                 ExpandReplicate},
        {"ExtendByReplication",             ExtendByReplication},
        {"ExtractBarcodeCrossings",         ExtractBarcodeCrossings},
        {"ExtractBarcodeWidths1",           ExtractBarcodeWidths1},
        {"ExtractBarcodeWidths2",           ExtractBarcodeWidths2},
        {"ExtractBarcodes",                 ExtractBarcodes},
        {"ExtractBorderConnComps",          ExtractBorderConnComps},
        {"ExtractBoundary",                 ExtractBoundary},
        {"ExtractData",                     ExtractData},
        {"ExtractOnLine",                   ExtractOnLine},
        {"ExtractRawTextlines",             ExtractRawTextlines},
        {"ExtractTextlines",                ExtractTextlines},
        {"FHMTGen_1",                       FHMTGen_1},
        {"FMorphopGen_1",                   FMorphopGen_1},
        {"FMorphopGen_2",                   FMorphopGen_2},
        {"FadeWithGray",                    FadeWithGray},
        {"FastTophat",                      FastTophat},
        {"FewColorsMedianCutQuantMixed",    FewColorsMedianCutQuantMixed},
        {"FewColorsOctcubeQuant1",          FewColorsOctcubeQuant1},
        {"FewColorsOctcubeQuant2",          FewColorsOctcubeQuant2},
        {"FewColorsOctcubeQuantMixed",      FewColorsOctcubeQuantMixed},
        {"FillBgFromBorder",                FillBgFromBorder},
        {"FillClosedBorders",               FillClosedBorders},
        {"FillHolesToBoundingRect",         FillHolesToBoundingRect},
        {"FillMapHoles",                    FillMapHoles},
        {"FillPolygon",                     FillPolygon},
        {"FinalAccumulate",                 FinalAccumulate},
        {"FinalAccumulateThreshold",        FinalAccumulateThreshold},
        {"FindAreaFraction",                FindAreaFraction},
        {"FindAreaFractionMasked",          FindAreaFractionMasked},
        {"FindAreaPerimRatio",              FindAreaPerimRatio},
        {"FindBaselines",                   FindBaselines},
        {"FindColorRegions",                FindColorRegions},
        {"FindCornerPixels",                FindCornerPixels},
        {"FindDifferentialSquareSum",       FindDifferentialSquareSum},
        {"FindEqualValues",                 FindEqualValues},
        {"FindHistoPeaksHSV",               FindHistoPeaksHSV},
        {"FindHorizontalRuns",              FindHorizontalRuns},
        {"FindLargeRectangles",             FindLargeRectangles},
        {"FindLargestRectangle",            FindLargestRectangle},
        {"FindMaxHorizontalRunOnLine",      FindMaxHorizontalRunOnLine},
        {"FindMaxRuns",                     FindMaxRuns},
        {"FindMaxVerticalRunOnLine",        FindMaxVerticalRunOnLine},
        {"FindNormalizedSquareSum",         FindNormalizedSquareSum},
        {"FindOverlapFraction",             FindOverlapFraction},
        {"FindPageForeground",              FindPageForeground},
        {"FindPerimSizeRatio",              FindPerimSizeRatio},
        {"FindPerimToAreaRatio",            FindPerimToAreaRatio},
        {"FindRectangleComps",              FindRectangleComps},
        {"FindRepCloseTile",                FindRepCloseTile},
        {"FindSkew",                        FindSkew},
        {"FindSkewAndDeskew",               FindSkewAndDeskew},
        {"FindSkewOrthogonalRange",         FindSkewOrthogonalRange},
        {"FindSkewSweep",                   FindSkewSweep},
        {"FindSkewSweepAndSearch",          FindSkewSweepAndSearch},
        {"FindSkewSweepAndSearchScore",     FindSkewSweepAndSearchScore},
        {"FindSkewSweepAndSearchScorePivot",FindSkewSweepAndSearchScorePivot},
        {"FindStrokeLength",                FindStrokeLength},
        {"FindStrokeWidth",                 FindStrokeWidth},
        {"FindThreshFgExtent",              FindThreshFgExtent},
        {"FindVerticalRuns",                FindVerticalRuns},
        {"FixedOctcubeQuant256",            FixedOctcubeQuant256},
        {"FixedOctcubeQuantGenRGB",         FixedOctcubeQuantGenRGB},
        {"FlipFHMTGen",                     FlipFHMTGen},
        {"FlipLR",                          FlipLR},
        {"FlipPixel",                       FlipPixel},
        {"FlipTB",                          FlipTB},
        {"ForegroundFraction",              ForegroundFraction},
        {"FractionFgInMask",                FractionFgInMask},
        {"FreeData",                        FreeData},
        {"GammaTRC",                        GammaTRC},
        {"GammaTRCMasked",                  GammaTRCMasked},
        {"GammaTRCWithAlpha",               GammaTRCWithAlpha},
        {"GenHalftoneMask",                 GenHalftoneMask},
        {"GenPhotoHistos",                  GenPhotoHistos},
        {"GenTextblockMask",                GenTextblockMask},
        {"GenTextlineMask",                 GenTextlineMask},
        {"GenerateCIData",                  GenerateCIData},
        {"GenerateFromPta",                 GenerateFromPta},
        {"GenerateHalftoneMask",            GenerateHalftoneMask},
        {"GenerateMaskByBand",              GenerateMaskByBand},
        {"GenerateMaskByBand32",            GenerateMaskByBand32},
        {"GenerateMaskByDiscr32",           GenerateMaskByDiscr32},
        {"GenerateMaskByValue",             GenerateMaskByValue},
        {"GeneratePtaBoundary",             GeneratePtaBoundary},
        {"GenerateSelBoundary",             GenerateSelBoundary},
        {"GenerateSelRandom",               GenerateSelRandom},
        {"GenerateSelWithRuns",             GenerateSelWithRuns},
        {"GetAllCCBorders",                 GetAllCCBorders},
        {"GetAutoFormat",                   GetAutoFormat},
        {"GetAverageMasked",                GetAverageMasked},
        {"GetAverageMaskedRGB",             GetAverageMaskedRGB},
        {"GetAverageTiled",                 GetAverageTiled},
        {"GetAverageTiledRGB",              GetAverageTiledRGB},
        {"GetBackgroundGrayMap",            GetBackgroundGrayMap},
        {"GetBackgroundGrayMapMorph",       GetBackgroundGrayMapMorph},
        {"GetBackgroundRGBMap",             GetBackgroundRGBMap},
        {"GetBackgroundRGBMapMorph",        GetBackgroundRGBMapMorph},
        {"GetBinnedColor",                  GetBinnedColor},
        {"GetBinnedComponentRange",         GetBinnedComponentRange},
        {"GetBlackOrWhiteVal",              GetBlackOrWhiteVal},
        {"GetBlackVal",                     GetBlackVal},
        {"GetCCBorders",                    GetCCBorders},
        {"GetCmapHistogram",                GetCmapHistogram},
        {"GetCmapHistogramInRect",          GetCmapHistogramInRect},
        {"GetCmapHistogramMasked",          GetCmapHistogramMasked},
        {"GetColorAmapHistogram",           GetColorAmapHistogram},
        {"GetColorHistogram",               GetColorHistogram},
        {"GetColorHistogramMasked",         GetColorHistogramMasked},
        {"GetColorNearMaskBoundary",        GetColorNearMaskBoundary},
        {"GetColormap",                     GetColormap},
        {"GetColumnStats",                  GetColumnStats},
        {"GetData",                         GetData},
        {"GetDepth",                        GetDepth},
        {"GetDifferenceHistogram",          GetDifferenceHistogram},
        {"GetDifferenceStats",              GetDifferenceStats},
        {"GetDimensions",                   GetDimensions},
        {"GetEdgeProfile",                  GetEdgeProfile},
        {"GetExtremeValue",                 GetExtremeValue},
        {"GetGrayHistogram",                GetGrayHistogram},
        {"GetGrayHistogramInRect",          GetGrayHistogramInRect},
        {"GetGrayHistogramMasked",          GetGrayHistogramMasked},
        {"GetGrayHistogramTiled",           GetGrayHistogramTiled},
        {"GetHeight",                       GetHeight},
        {"GetHoleBorder",                   GetHoleBorder},
        {"GetInputFormat",                  GetInputFormat},
        {"GetInvBackgroundMap",             GetInvBackgroundMap},
        {"GetLastOffPixelInRun",            GetLastOffPixelInRun},
        {"GetLastOnPixelInRun",             GetLastOnPixelInRun},
        {"GetLinePtrs",                     GetLinePtrs},
        {"GetLocalSkewAngles",              GetLocalSkewAngles},
        {"GetLocalSkewTransform",           GetLocalSkewTransform},
        {"GetMaxValueInRect",               GetMaxValueInRect},
        {"GetMomentByColumn",               GetMomentByColumn},
        {"GetMostPopulatedColors",          GetMostPopulatedColors},
        {"GetOuterBorder",                  GetOuterBorder},
        {"GetOuterBorderPta",               GetOuterBorderPta},
        {"GetOuterBordersPtaa",             GetOuterBordersPtaa},
        {"GetPSNR",                         GetPSNR},
        {"GetPerceptualDiff",               GetPerceptualDiff},
        {"GetPixel",                        GetPixel},
        {"GetPixelAverage",                 GetPixelAverage},
        {"GetPixelStats",                   GetPixelStats},
        {"GetRGBComponent",                 GetRGBComponent},
        {"GetRGBComponentCmap",             GetRGBComponentCmap},
        {"GetRGBHistogram",                 GetRGBHistogram},
        {"GetRGBLine",                      GetRGBLine},
        {"GetRGBPixel",                     GetRGBPixel},
        {"GetRandomPixel",                  GetRandomPixel},
        {"GetRangeValues",                  GetRangeValues},
        {"GetRankColorArray",               GetRankColorArray},
        {"GetRankValue",                    GetRankValue},
        {"GetRankValueMasked",              GetRankValueMasked},
        {"GetRankValueMaskedRGB",           GetRankValueMaskedRGB},
        {"GetRasterData",                   GetRasterData},
        {"GetRefcount",                     GetRefcount},
        {"GetRegionsBinary",                GetRegionsBinary},
        {"GetResolution",                   GetResolution},
        {"GetRowStats",                     GetRowStats},
        {"GetRunCentersOnLine",             GetRunCentersOnLine},
        {"GetRunsOnLine",                   GetRunsOnLine},
        {"GetSortedNeighborValues",         GetSortedNeighborValues},
        {"GetSpp",                          GetSpp},
        {"GetText",                         GetText},
        {"GetWhiteVal",                     GetWhiteVal},
        {"GetWidth",                        GetWidth},
        {"GetWordBoxesInTextlines",         GetWordBoxesInTextlines},
        {"GetWordsInTextlines",             GetWordsInTextlines},
        {"GetWpl",                          GetWpl},
        {"GetXRes",                         GetXRes},
        {"GetYRes",                         GetYRes},
        {"GlobalNormNoSatRGB",              GlobalNormNoSatRGB},
        {"GlobalNormRGB",                   GlobalNormRGB},
        {"GrayMorphSequence",               GrayMorphSequence},
        {"GrayQuantFromCmap",               GrayQuantFromCmap},
        {"GrayQuantFromHisto",              GrayQuantFromHisto},
        {"HDome",                           HDome},
        {"HMT",                             HMT},
        {"HMTDwa_1",                        HMTDwa_1},
        {"HShear",                          HShear},
        {"HShearCenter",                    HShearCenter},
        {"HShearCorner",                    HShearCorner},
        {"HShearIP",                        HShearIP},
        {"HShearLI",                        HShearLI},
        {"HalfEdgeByBandpass",              HalfEdgeByBandpass},
        {"HasHighlightRed",                 HasHighlightRed},
        {"Haustest",                        Haustest},
        {"HolesByFilling",                  HolesByFilling},
        {"InitAccumulate",                  InitAccumulate},
        {"IntersectionOfMorphOps",          IntersectionOfMorphOps},
        {"Invert",                          Invert},
        {"ItalicWords",                     ItalicWords},
        {"LinearEdgeFade",                  LinearEdgeFade},
        {"LinearMapToTargetColor",          LinearMapToTargetColor},
        {"LinearTRCTiled",                  LinearTRCTiled},
        {"LocToColorTransform",             LocToColorTransform},
        {"LocalExtrema",                    LocalExtrema},
        {"LocateBarcodes",                  LocateBarcodes},
        {"MakeAlphaFromMask",               MakeAlphaFromMask},
        {"MakeArbMaskFromRGB",              MakeArbMaskFromRGB},
        {"MakeFrameMask",                   MakeFrameMask},
        {"MakeHistoHS",                     MakeHistoHS},
        {"MakeHistoHV",                     MakeHistoHV},
        {"MakeHistoSV",                     MakeHistoSV},
        {"MakeMaskFromLUT",                 MakeMaskFromLUT},
        {"MakeMaskFromVal",                 MakeMaskFromVal},
        {"MakeRangeMaskHS",                 MakeRangeMaskHS},
        {"MakeRangeMaskHV",                 MakeRangeMaskHV},
        {"MakeRangeMaskSV",                 MakeRangeMaskSV},
        {"MaskBoxa",                        MaskBoxa},
        {"MaskConnComp",                    MaskConnComp},
        {"MaskOverColorPixels",             MaskOverColorPixels},
        {"MaskOverColorRange",              MaskOverColorRange},
        {"MaskedThreshOnBackgroundNorm",    MaskedThreshOnBackgroundNorm},
        {"MaxDynamicRange",                 MaxDynamicRange},
        {"MaxDynamicRangeRGB",              MaxDynamicRangeRGB},
        {"MeanInRectangle",                 MeanInRectangle},
        {"MeanSquareAccum",                 MeanSquareAccum},
        {"MeasureEdgeSmoothness",           MeasureEdgeSmoothness},
        {"MeasureSaturation",               MeasureSaturation},
        {"MedianCutHisto",                  MedianCutHisto},
        {"MedianCutQuant",                  MedianCutQuant},
        {"MedianCutQuantGeneral",           MedianCutQuantGeneral},
        {"MedianCutQuantMixed",             MedianCutQuantMixed},
        {"MedianFilter",                    MedianFilter},
        {"MinMaxNearLine",                  MinMaxNearLine},
        {"MinMaxTiles",                     MinMaxTiles},
        {"MinOrMax",                        MinOrMax},
        {"MirrorDetect",                    MirrorDetect},
        {"MirrorDetectDwa",                 MirrorDetectDwa},
        {"MirroredTiling",                  MirroredTiling},
        {"ModifyBrightness",                ModifyBrightness},
        {"ModifyHue",                       ModifyHue},
        {"ModifySaturation",                ModifySaturation},
        {"ModifyStrokeWidth",               ModifyStrokeWidth},
        {"MorphCompSequence",               MorphCompSequence},
        {"MorphCompSequenceDwa",            MorphCompSequenceDwa},
        {"MorphDwa_1",                      MorphDwa_1},
        {"MorphDwa_2",                      MorphDwa_2},
        {"MorphGradient",                   MorphGradient},
        {"MorphSequence",                   MorphSequence},
        {"MorphSequenceByComponent",        MorphSequenceByComponent},
        {"MorphSequenceByRegion",           MorphSequenceByRegion},
        {"MorphSequenceDwa",                MorphSequenceDwa},
        {"MorphSequenceMasked",             MorphSequenceMasked},
        {"MosaicColorShiftRGB",             MosaicColorShiftRGB},
        {"MultConstAccumulate",             MultConstAccumulate},
        {"MultConstantColor",               MultConstantColor},
        {"MultConstantGray",                MultConstantGray},
        {"MultMatrixColor",                 MultMatrixColor},
        {"MultiplyByColor",                 MultiplyByColor},
        {"NumColors",                       NumColors},
        {"NumSignificantGrayColors",        NumSignificantGrayColors},
        {"NumberOccupiedOctcubes",          NumberOccupiedOctcubes},
        {"OctcubeHistogram",                OctcubeHistogram},
        {"OctcubeQuantFromCmap",            OctcubeQuantFromCmap},
        {"OctcubeQuantMixedWithGray",       OctcubeQuantMixedWithGray},
        {"OctreeColorQuant",                OctreeColorQuant},
        {"OctreeColorQuantGeneral",         OctreeColorQuantGeneral},
        {"OctreeQuantByPopulation",         OctreeQuantByPopulation},
        {"OctreeQuantNumColors",            OctreeQuantNumColors},
        {"Open",                            Open},
        {"OpenBrick",                       OpenBrick},
        {"OpenBrickDwa",                    OpenBrickDwa},
        {"OpenCompBrick",                   OpenCompBrick},
        {"OpenCompBrickDwa",                OpenCompBrickDwa},
        {"OpenCompBrickExtendDwa",          OpenCompBrickExtendDwa},
        {"OpenGeneralized",                 OpenGeneralized},
        {"OpenGray",                        OpenGray},
        {"OpenGray3",                       OpenGray3},
        {"Or",                              Or},
        {"OrientCorrect",                   OrientCorrect},
        {"OrientDetect",                    OrientDetect},
        {"OrientDetectDwa",                 OrientDetectDwa},
        {"OtsuAdaptiveThreshold",           OtsuAdaptiveThreshold},
        {"OtsuThreshOnBackgroundNorm",      OtsuThreshOnBackgroundNorm},
        {"PadToCenterCentroid",             PadToCenterCentroid},
        {"PaintBoxa",                       PaintBoxa},
        {"PaintBoxaRandom",                 PaintBoxaRandom},
        {"PaintSelfThroughMask",            PaintSelfThroughMask},
        {"PaintThroughMask",                PaintThroughMask},
        {"PlotAlongPta",                    PlotAlongPta},
        {"Prepare1bpp",                     Prepare1bpp},
        {"PrintStreamInfo",                 PrintStreamInfo},
        {"ProcessBarcodes",                 ProcessBarcodes},
        {"Projective",                      Projective},
        {"ProjectiveColor",                 ProjectiveColor},
        {"ProjectiveGray",                  ProjectiveGray},
        {"ProjectivePta",                   ProjectivePta},
        {"ProjectivePtaColor",              ProjectivePtaColor},
        {"ProjectivePtaGray",               ProjectivePtaGray},
        {"ProjectivePtaWithAlpha",          ProjectivePtaWithAlpha},
        {"ProjectiveSampled",               ProjectiveSampled},
        {"ProjectiveSampledPta",            ProjectiveSampledPta},
        {"QuadraticVShear",                 QuadraticVShear},
        {"QuadraticVShearLI",               QuadraticVShearLI},
        {"QuadraticVShearSampled",          QuadraticVShearSampled},
        {"QuadtreeMean",                    QuadtreeMean},
        {"QuadtreeVariance",                QuadtreeVariance},
        {"QuantFromCmap",                   QuantFromCmap},
        {"QuantizeIfFewColors",             QuantizeIfFewColors},
        {"RandomHarmonicWarp",              RandomHarmonicWarp},
        {"RankBinByStrip",                  RankBinByStrip},
        {"RankColumnTransform",             RankColumnTransform},
        {"RankFilter",                      RankFilter},
        {"RankFilterGray",                  RankFilterGray},
        {"RankFilterRGB",                   RankFilterRGB},
        {"RankFilterWithScaling",           RankFilterWithScaling},
        {"RankHaustest",                    RankHaustest},
        {"RankRowTransform",                RankRowTransform},
        {"Rasterop",                        Rasterop},
        {"RasteropFullImage",               RasteropFullImage},
        {"RasteropHip",                     RasteropHip},
        {"RasteropIP",                      RasteropIP},
        {"RasteropVip",                     RasteropVip},
        {"Read",                            Read},
        {"ReadBarcodeWidths",               ReadBarcodeWidths},
        {"ReadBarcodes",                    ReadBarcodes},
        {"ReadFromMultipageTiff",           ReadFromMultipageTiff},
        {"ReadHeader",                      ReadHeader},
        {"ReadHeaderMem",                   ReadHeaderMem},
        {"ReadIndexed",                     ReadIndexed},
        {"ReadJp2k",                        ReadJp2k},
        {"ReadJpeg",                        ReadJpeg},
        {"ReadMem",                         ReadMem},
        {"ReadMemBmp",                      ReadMemBmp},
        {"ReadMemFromMultipageTiff",        ReadMemFromMultipageTiff},
        {"ReadMemGif",                      ReadMemGif},
        {"ReadMemJp2k",                     ReadMemJp2k},
        {"ReadMemJpeg",                     ReadMemJpeg},
        {"ReadMemPng",                      ReadMemPng},
        {"ReadMemPnm",                      ReadMemPnm},
        {"ReadMemSpix",                     ReadMemSpix},
        {"ReadMemTiff",                     ReadMemTiff},
        {"ReadMemWebP",                     ReadMemWebP},
        {"ReadStream",                      ReadStream},
        {"ReadStreamBmp",                   ReadStreamBmp},
        {"ReadStreamGif",                   ReadStreamGif},
        {"ReadStreamJp2k",                  ReadStreamJp2k},
        {"ReadStreamJpeg",                  ReadStreamJpeg},
        {"ReadStreamPng",                   ReadStreamPng},
        {"ReadStreamPnm",                   ReadStreamPnm},
        {"ReadStreamSpix",                  ReadStreamSpix},
        {"ReadStreamTiff",                  ReadStreamTiff},
        {"ReadStreamWebP",                  ReadStreamWebP},
        {"ReadTiff",                        ReadTiff},
        {"ReadWithHint",                    ReadWithHint},
        {"ReduceBinary2",                   ReduceBinary2},
        {"ReduceRankBinary2",               ReduceRankBinary2},
        {"ReduceRankBinaryCascade",         ReduceRankBinaryCascade},
        {"RemoveAlpha",                     RemoveAlpha},
        {"RemoveBorder",                    RemoveBorder},
        {"RemoveBorderConnComps",           RemoveBorderConnComps},
        {"RemoveBorderGeneral",             RemoveBorderGeneral},
        {"RemoveBorderToSize",              RemoveBorderToSize},
        {"RemoveColormap",                  RemoveColormap},
        {"RemoveColormapGeneral",           RemoveColormapGeneral},
        {"RemoveMatchedPattern",            RemoveMatchedPattern},
        {"RemoveSeededComponents",          RemoveSeededComponents},
        {"RemoveUnusedColors",              RemoveUnusedColors},
        {"RemoveWithIndicator",             RemoveWithIndicator},
        {"RenderBox",                       RenderBox},
        {"RenderBoxArb",                    RenderBoxArb},
        {"RenderBoxBlend",                  RenderBoxBlend},
        {"RenderBoxa",                      RenderBoxa},
        {"RenderBoxaArb",                   RenderBoxaArb},
        {"RenderBoxaBlend",                 RenderBoxaBlend},
        {"RenderContours",                  RenderContours},
        {"RenderGridArb",                   RenderGridArb},
        {"RenderHashBox",                   RenderHashBox},
        {"RenderHashBoxArb",                RenderHashBoxArb},
        {"RenderHashBoxBlend",              RenderHashBoxBlend},
        {"RenderHashBoxa",                  RenderHashBoxa},
        {"RenderHashBoxaArb",               RenderHashBoxaArb},
        {"RenderHashBoxaBlend",             RenderHashBoxaBlend},
        {"RenderHashMaskArb",               RenderHashMaskArb},
        {"RenderLine",                      RenderLine},
        {"RenderLineArb",                   RenderLineArb},
        {"RenderLineBlend",                 RenderLineBlend},
        {"RenderPlotFromNuma",              RenderPlotFromNuma},
        {"RenderPlotFromNumaGen",           RenderPlotFromNumaGen},
        {"RenderPolygon",                   RenderPolygon},
        {"RenderPolyline",                  RenderPolyline},
        {"RenderPolylineArb",               RenderPolylineArb},
        {"RenderPolylineBlend",             RenderPolylineBlend},
        {"RenderPta",                       RenderPta},
        {"RenderPtaArb",                    RenderPtaArb},
        {"RenderPtaBlend",                  RenderPtaBlend},
        {"RenderRandomCmapPtaa",            RenderRandomCmapPtaa},
        {"ResizeImageData",                 ResizeImageData},
        {"ResizeToMatch",                   ResizeToMatch},
        {"ReversalProfile",                 ReversalProfile},
        {"Rotate",                          Rotate},
        {"Rotate180",                       Rotate180},
        {"Rotate2Shear",                    Rotate2Shear},
        {"Rotate3Shear",                    Rotate3Shear},
        {"Rotate90",                        Rotate90},
        {"RotateAM",                        RotateAM},
        {"RotateAMColor",                   RotateAMColor},
        {"RotateAMColorCorner",             RotateAMColorCorner},
        {"RotateAMColorFast",               RotateAMColorFast},
        {"RotateAMCorner",                  RotateAMCorner},
        {"RotateAMGray",                    RotateAMGray},
        {"RotateAMGrayCorner",              RotateAMGrayCorner},
        {"RotateBinaryNice",                RotateBinaryNice},
        {"RotateBySampling",                RotateBySampling},
        {"RotateOrth",                      RotateOrth},
        {"RotateShear",                     RotateShear},
        {"RotateShearCenter",               RotateShearCenter},
        {"RotateShearCenterIP",             RotateShearCenterIP},
        {"RotateShearIP",                   RotateShearIP},
        {"RotateWithAlpha",                 RotateWithAlpha},
        {"RowStats",                        RowStats},
        {"RunHistogramMorph",               RunHistogramMorph},
        {"RunlengthTransform",              RunlengthTransform},
        {"SauvolaBinarize",                 SauvolaBinarize},
        {"SauvolaBinarizeTiled",            SauvolaBinarizeTiled},
        {"SauvolaGetThreshold",             SauvolaGetThreshold},
        {"SaveTiled",                       SaveTiled},
        {"SaveTiledOutline",                SaveTiledOutline},
        {"SaveTiledWithText",               SaveTiledWithText},
        {"Scale",                           Scale},
        {"ScaleAndTransferAlpha",           ScaleAndTransferAlpha},
        {"ScaleAreaMap",                    ScaleAreaMap},
        {"ScaleAreaMap2",                   ScaleAreaMap2},
        {"ScaleAreaMapToSize",              ScaleAreaMapToSize},
        {"ScaleBinary",                     ScaleBinary},
        {"ScaleByIntSampling",              ScaleByIntSampling},
        {"ScaleBySampling",                 ScaleBySampling},
        {"ScaleBySamplingToSize",           ScaleBySamplingToSize},
        {"ScaleColor2xLI",                  ScaleColor2xLI},
        {"ScaleColor4xLI",                  ScaleColor4xLI},
        {"ScaleColorLI",                    ScaleColorLI},
        {"ScaleGeneral",                    ScaleGeneral},
        {"ScaleGray2xLI",                   ScaleGray2xLI},
        {"ScaleGray2xLIDither",             ScaleGray2xLIDither},
        {"ScaleGray2xLIThresh",             ScaleGray2xLIThresh},
        {"ScaleGray4xLI",                   ScaleGray4xLI},
        {"ScaleGray4xLIDither",             ScaleGray4xLIDither},
        {"ScaleGray4xLIThresh",             ScaleGray4xLIThresh},
        {"ScaleGrayLI",                     ScaleGrayLI},
        {"ScaleGrayMinMax",                 ScaleGrayMinMax},
        {"ScaleGrayMinMax2",                ScaleGrayMinMax2},
        {"ScaleGrayRank2",                  ScaleGrayRank2},
        {"ScaleGrayRankCascade",            ScaleGrayRankCascade},
        {"ScaleGrayToBinaryFast",           ScaleGrayToBinaryFast},
        {"ScaleLI",                         ScaleLI},
        {"ScaleMipmap",                     ScaleMipmap},
        {"ScaleRGBToBinaryFast",            ScaleRGBToBinaryFast},
        {"ScaleRGBToGray2",                 ScaleRGBToGray2},
        {"ScaleRGBToGrayFast",              ScaleRGBToGrayFast},
        {"ScaleResolution",                 ScaleResolution},
        {"ScaleSmooth",                     ScaleSmooth},
        {"ScaleSmoothToSize",               ScaleSmoothToSize},
        {"ScaleToGray",                     ScaleToGray},
        {"ScaleToGray16",                   ScaleToGray16},
        {"ScaleToGray2",                    ScaleToGray2},
        {"ScaleToGray3",                    ScaleToGray3},
        {"ScaleToGray4",                    ScaleToGray4},
        {"ScaleToGray6",                    ScaleToGray6},
        {"ScaleToGray8",                    ScaleToGray8},
        {"ScaleToGrayFast",                 ScaleToGrayFast},
        {"ScaleToGrayMipmap",               ScaleToGrayMipmap},
        {"ScaleToSize",                     ScaleToSize},
        {"ScaleToSizeRel",                  ScaleToSizeRel},
        {"ScaleWithAlpha",                  ScaleWithAlpha},
        {"ScanForEdge",                     ScanForEdge},
        {"ScanForForeground",               ScanForForeground},
        {"SearchBinaryMaze",                SearchBinaryMaze},
        {"SearchGrayMaze",                  SearchGrayMaze},
        {"Seedfill",                        Seedfill},
        {"Seedfill4",                       Seedfill4},
        {"Seedfill4BB",                     Seedfill4BB},
        {"Seedfill8",                       Seedfill8},
        {"Seedfill8BB",                     Seedfill8BB},
        {"SeedfillBB",                      SeedfillBB},
        {"SeedfillBinary",                  SeedfillBinary},
        {"SeedfillBinaryRestricted",        SeedfillBinaryRestricted},
        {"SeedfillGray",                    SeedfillGray},
        {"SeedfillGrayBasin",               SeedfillGrayBasin},
        {"SeedfillGrayInv",                 SeedfillGrayInv},
        {"SeedfillGrayInvSimple",           SeedfillGrayInvSimple},
        {"SeedfillGraySimple",              SeedfillGraySimple},
        {"SeedfillMorph",                   SeedfillMorph},
        {"Seedspread",                      Seedspread},
        {"SelectByAreaFraction",            SelectByAreaFraction},
        {"SelectByPerimSizeRatio",          SelectByPerimSizeRatio},
        {"SelectByPerimToAreaRatio",        SelectByPerimToAreaRatio},
        {"SelectBySize",                    SelectBySize},
        {"SelectByWidthHeightRatio",        SelectByWidthHeightRatio},
        {"SelectDefaultPdfEncoding",        SelectDefaultPdfEncoding},
        {"SelectLargeULComp",               SelectLargeULComp},
        {"SelectMinInConnComp",             SelectMinInConnComp},
        {"SelectedLocalExtrema",            SelectedLocalExtrema},
        {"SelectiveConnCompFill",           SelectiveConnCompFill},
        {"SerializeToMemory",               SerializeToMemory},
        {"SetAll",                          SetAll},
        {"SetAllArbitrary",                 SetAllArbitrary},
        {"SetAllGray",                      SetAllGray},
        {"SetAlphaOverWhite",               SetAlphaOverWhite},
        {"SetBlack",                        SetBlack},
        {"SetBlackOrWhite",                 SetBlackOrWhite},
        {"SetBlackOrWhiteBoxa",             SetBlackOrWhiteBoxa},
        {"SetBorderRingVal",                SetBorderRingVal},
        {"SetBorderVal",                    SetBorderVal},
        {"SetChromaSampling",               SetChromaSampling},
        {"SetColormap",                     SetColormap},
        {"SetComponentArbitrary",           SetComponentArbitrary},
        {"SetData",                         SetData},
        {"SetDepth",                        SetDepth},
        {"SetDimensions",                   SetDimensions},
        {"SetHeight",                       SetHeight},
        {"SetInRect",                       SetInRect},
        {"SetInRectArbitrary",              SetInRectArbitrary},
        {"SetInputFormat",                  SetInputFormat},
        {"SetLowContrast",                  SetLowContrast},
        {"SetMasked",                       SetMasked},
        {"SetMaskedCmap",                   SetMaskedCmap},
        {"SetMaskedGeneral",                SetMaskedGeneral},
        {"SetMirroredBorder",               SetMirroredBorder},
        {"SetOrClearBorder",                SetOrClearBorder},
        {"SetPadBits",                      SetPadBits},
        {"SetPadBitsBand",                  SetPadBitsBand},
        {"SetPixel",                        SetPixel},
        {"SetPixelColumn",                  SetPixelColumn},
        {"SetRGBComponent",                 SetRGBComponent},
        {"SetRGBPixel",                     SetRGBPixel},
        {"SetResolution",                   SetResolution},
        {"SetSelectCmap",                   SetSelectCmap},
        {"SetSelectMaskedCmap",             SetSelectMaskedCmap},
        {"SetSpecial",                      SetSpecial},
        {"SetSpp",                          SetSpp},
        {"SetStrokeWidth",                  SetStrokeWidth},
        {"SetText",                         SetText},
        {"SetTextblock",                    SetTextblock},
        {"SetTextline",                     SetTextline},
        {"SetUnderTransparency",            SetUnderTransparency},
        {"SetWhite",                        SetWhite},
        {"SetWidth",                        SetWidth},
        {"SetWpl",                          SetWpl},
        {"SetXRes",                         SetXRes},
        {"SetYRes",                         SetYRes},
        {"SetZlibCompression",              SetZlibCompression},
        {"SetupByteProcessing",             SetupByteProcessing},
        {"ShiftAndTransferAlpha",           ShiftAndTransferAlpha},
        {"ShiftByComponent",                ShiftByComponent},
        {"SimpleCaptcha",                   SimpleCaptcha},
        {"SimpleColorQuantize",             SimpleColorQuantize},
        {"SizesEqual",                      SizesEqual},
        {"SmoothConnectedRegions",          SmoothConnectedRegions},
        {"SnapColor",                       SnapColor},
        {"SnapColorCmap",                   SnapColorCmap},
        {"SobelEdgeFilter",                 SobelEdgeFilter},
        {"SplitComponentIntoBoxa",          SplitComponentIntoBoxa},
        {"SplitComponentWithProfile",       SplitComponentWithProfile},
        {"SplitDistributionFgBg",           SplitDistributionFgBg},
        {"SplitIntoBoxa",                   SplitIntoBoxa},
        {"SplitIntoCharacters",             SplitIntoCharacters},
        {"StereoFromPair",                  StereoFromPair},
        {"StretchHorizontal",               StretchHorizontal},
        {"StretchHorizontalLI",             StretchHorizontalLI},
        {"StretchHorizontalSampled",        StretchHorizontalSampled},
        {"StrokeWidthTransform",            StrokeWidthTransform},
        {"SubsampleBoundaryPixels",         SubsampleBoundaryPixels},
        {"Subtract",                        Subtract},
        {"SubtractGray",                    SubtractGray},
        {"SwapAndDestroy",                  SwapAndDestroy},
        {"TRCMap",                          TRCMap},
        {"TestClipToForeground",            TestClipToForeground},
        {"TestForSimilarity",               TestForSimilarity},
        {"ThinConnected",                   ThinConnected},
        {"ThinConnectedBySet",              ThinConnectedBySet},
        {"Threshold8",                      Threshold8},
        {"ThresholdByConnComp",             ThresholdByConnComp},
        {"ThresholdForFgBg",                ThresholdForFgBg},
        {"ThresholdGrayArb",                ThresholdGrayArb},
        {"ThresholdOn8bpp",                 ThresholdOn8bpp},
        {"ThresholdPixelSum",               ThresholdPixelSum},
        {"ThresholdSpreadNorm",             ThresholdSpreadNorm},
        {"ThresholdTo2bpp",                 ThresholdTo2bpp},
        {"ThresholdTo4bpp",                 ThresholdTo4bpp},
        {"ThresholdToBinary",               ThresholdToBinary},
        {"ThresholdToValue",                ThresholdToValue},
        {"TilingCreate",                    TilingCreate},
        {"TilingDestroy",                   TilingDestroy},
        {"TilingGetCount",                  TilingGetCount},
        {"TilingGetSize",                   TilingGetSize},
        {"TilingGetTile",                   TilingGetTile},
        {"TilingNoStripOnPaint",            TilingNoStripOnPaint},
        {"TilingPaintTile",                 TilingPaintTile},
        {"Tophat",                          Tophat},
        {"TransferAllData",                 TransferAllData},
        {"Translate",                       Translate},
        {"TwoSidedEdgeFilter",              TwoSidedEdgeFilter},
        {"UnionOfMorphOps",                 UnionOfMorphOps},
        {"UnpackBinary",                    UnpackBinary},
        {"UnsharpMasking",                  UnsharpMasking},
        {"UnsharpMaskingFast",              UnsharpMaskingFast},
        {"UnsharpMaskingGray",              UnsharpMaskingGray},
        {"UnsharpMaskingGray1D",            UnsharpMaskingGray1D},
        {"UnsharpMaskingGray2D",            UnsharpMaskingGray2D},
        {"UnsharpMaskingGrayFast",          UnsharpMaskingGrayFast},
        {"UpDownDetect",                    UpDownDetect},
        {"UpDownDetectDwa",                 UpDownDetectDwa},
        {"UpDownDetectGeneral",             UpDownDetectGeneral},
        {"UpDownDetectGeneralDwa",          UpDownDetectGeneralDwa},
        {"UsesCmapColor",                   UsesCmapColor},
        {"VShear",                          VShear},
        {"VShearCenter",                    VShearCenter},
        {"VShearCorner",                    VShearCorner},
        {"VShearIP",                        VShearIP},
        {"VShearLI",                        VShearLI},
        {"VarThresholdToBinary",            VarThresholdToBinary},
        {"VarianceByColumn",                VarianceByColumn},
        {"VarianceByRow",                   VarianceByRow},
        {"VarianceInRect",                  VarianceInRect},
        {"VarianceInRectangle",             VarianceInRectangle},
        {"WarpStereoscopic",                WarpStereoscopic},
        {"WindowedMean",                    WindowedMean},
        {"WindowedMeanSquare",              WindowedMeanSquare},
        {"WindowedStats",                   WindowedStats},
        {"WindowedVariance",                WindowedVariance},
        {"WindowedVarianceOnLine",          WindowedVarianceOnLine},
        {"WordBoxesByDilation",             WordBoxesByDilation},
        {"WordMaskByDilation",              WordMaskByDilation},
        {"Write",                           Write},
        {"WriteAutoFormat",                 WriteAutoFormat},
        {"WriteDebug",                      WriteDebug},
        {"WriteImpliedFormat",              WriteImpliedFormat},
        {"WriteJp2k",                       WriteJp2k},
        {"WriteJpeg",                       WriteJpeg},
        {"WriteMem",                        WriteMem},
        {"WriteMemBmp",                     WriteMemBmp},
        {"WriteMemGif",                     WriteMemGif},
        {"WriteMemJp2k",                    WriteMemJp2k},
        {"WriteMemJpeg",                    WriteMemJpeg},
        {"WriteMemPS",                      WriteMemPS},
        {"WriteMemPam",                     WriteMemPam},
        {"WriteMemPdf",                     WriteMemPdf},
        {"WriteMemPng",                     WriteMemPng},
        {"WriteMemPnm",                     WriteMemPnm},
        {"WriteMemSpix",                    WriteMemSpix},
        {"WriteMemTiff",                    WriteMemTiff},
        {"WriteMemTiffCustom",              WriteMemTiffCustom},
        {"WriteMemWebP",                    WriteMemWebP},
        {"WriteMixedToPS",                  WriteMixedToPS},
        {"WritePSEmbed",                    WritePSEmbed},
        {"WritePng",                        WritePng},
        {"WriteSegmentedPageToPS",          WriteSegmentedPageToPS},
        {"WriteStream",                     WriteStream},
        {"WriteStreamAsciiPnm",             WriteStreamAsciiPnm},
        {"WriteStreamBmp",                  WriteStreamBmp},
        {"WriteStreamGif",                  WriteStreamGif},
        {"WriteStreamJp2k",                 WriteStreamJp2k},
        {"WriteStreamJpeg",                 WriteStreamJpeg},
        {"WriteStreamPS",                   WriteStreamPS},
        {"WriteStreamPam",                  WriteStreamPam},
        {"WriteStreamPdf",                  WriteStreamPdf},
        {"WriteStreamPng",                  WriteStreamPng},
        {"WriteStreamPnm",                  WriteStreamPnm},
        {"WriteStreamSpix",                 WriteStreamSpix},
        {"WriteStreamTiff",                 WriteStreamTiff},
        {"WriteStreamTiffWA",               WriteStreamTiffWA},
        {"WriteStreamWebP",                 WriteStreamWebP},
        {"WriteStringPS",                   WriteStringPS},
        {"WriteTiff",                       WriteTiff},
        {"WriteTiffCustom",                 WriteTiffCustom},
        {"WriteWebP",                       WriteWebP},
        {"Xor",                             Xor},
        {"Zero",                            Zero},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    for (int i = 0; i < 256; i++) {
        tab8[i] = ((i >> 7) & 1) +
                  ((i >> 6) & 1) +
                  ((i >> 5) & 1) +
                  ((i >> 4) & 1) +
                  ((i >> 3) & 1) +
                  ((i >> 2) & 1) +
                  ((i >> 1) & 1) +
                  ((i >> 0) & 1);
    }

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_PIX);
    return ll_register_class(L, LL_PIX, methods, functions);
}
