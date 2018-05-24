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

/**
 * \brief toString
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    FUNC(LL_PIX ".toString");
    static char str[256];
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
 * \brief Create a new Pix*
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
    FUNC(LL_PIX ".Create");
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
 * \brief Create a new Pix* but don't initialize it
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
    FUNC(LL_PIX ".CreateNoInit");
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
 * \brief Destroy a Pix*
 * <pre>
 * Arg #1 is expected to be Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_PIX ".Destroy");
    Pix **ppix = reinterpret_cast<Pix **>(ll_check_udata(_fun, L, 1, LL_PIX));
    Pix *pix = *ppix;
    DBG(LOG_DESTROY, "%s: '%s' ppix=%p pix=%p refcount=%d\n",
        _fun, LL_PIX, ppix, pix, pixGetRefcount(pix));
    pixDestroy(&pix);
    *ppix = nullptr;
    return 0;
}

/**
 * \brief Copy a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_PIX ".Copy");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixd = pixCopy(nullptr, pixs);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Clone a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    FUNC(LL_PIX ".Clone");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixd = pixClone(pixs);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Resize a Pix* image data
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
    FUNC(LL_PIX ".ResizeImageData");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixResizeImageData(pixd, pixs));
    return 1;
}

/**
 * \brief Copy the colormap of a Pix*
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
    FUNC(LL_PIX ".CopyColormap");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopyColormap(pixd, pixs));
    return 1;
}

/**
 * \brief Check if a Pix* is valid
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SizesEqual(lua_State *L)
{
    FUNC(LL_PIX ".SizesEqual");
    Pix *pix1 = ll_check_Pix(_fun, L, 1);
    Pix *pix2 = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSizesEqual(pix1, pix2));
    return 1;
}

/**
 * \brief Transfer all data from a Pix* (pixs) to self (pixd)
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
    FUNC(LL_PIX ".TransferAllData");
    void **ppixs = ll_check_udata(_fun, L, 2, LL_PIX);
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    int copytext = ll_check_boolean_default(_fun, L, 3, TRUE);
    int copyformat = ll_check_boolean_default(_fun, L, 4, TRUE);
    lua_pushboolean(L, 0 == pixTransferAllData(pixd, &pixs, copytext, copyformat));
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Swap an destroy a Pix* (pixd) with another Pix* (pixs)
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
    FUNC(LL_PIX ".SwapAndDestroy");
    void **ppixd = ll_check_udata(_fun, L, 1, LL_PIX);
    void **ppixs = ll_check_udata(_fun, L, 2, LL_PIX);
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSwapAndDestroy(&pixd, &pixs));
    *ppixd = pixd;
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Get the Pix* width
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWidth(lua_State *L)
{
    FUNC(LL_PIX ".GetWidth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = pixGetWidth(pix);
    lua_pushinteger(L, width);
    return 1;
}

/**
 * \brief Set the Pix* width
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWidth(lua_State *L)
{
    FUNC(LL_PIX ".SetWidth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32_default(_fun, L, 2, pixGetWidth(pix));
    lua_pushboolean(L, 0 == pixSetWidth(pix, width));
    return 1;
}

/**
 * \brief Get the Pix* height
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetHeight(lua_State *L)
{
    FUNC(LL_PIX ".GetHeight");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 height = pixGetHeight(pix);
    lua_pushinteger(L, height);
    return 1;
}

/**
 * \brief Set the Pix* height
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetHeight(lua_State *L)
{
    FUNC(LL_PIX ".SetHeight");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 height = ll_check_l_int32_default(_fun, L, 2, pixGetHeight(pix));
    lua_pushboolean(L, 0 == pixSetHeight(pix, height));
    return 1;
}

/**
 * \brief Get the Pix* depth
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetDepth(lua_State *L)
{
    FUNC(LL_PIX ".GetDepth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 depth = pixGetDepth(pix);
    lua_pushinteger(L, depth);
    return 1;
}

/**
 * \brief Set the Pix* depth
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetDepth(lua_State *L)
{
    FUNC(LL_PIX ".SetDepth");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 depth = ll_check_l_int32_default(_fun, L, 2, pixGetDepth(pix));
    lua_pushboolean(L, 0 == pixSetDepth(pix, depth));
    return 1;
}

/**
 * \brief Get the Pix* dimensions
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers (width, height, depth) on the Lua stack
 */
static int
GetDimensions(lua_State *L)
{
    FUNC(LL_PIX ".GetDimensions");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width, height, depth;
    if (pixGetDimensions(pix, &width, &height, &depth))
        return ll_push_nil(L);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    lua_pushinteger(L, depth);
    return 3;
}

/**
 * \brief Set the Pix* dimensions
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
    FUNC(LL_PIX ".SetDimensions");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32_default(_fun, L, 2, 0);
    l_int32 height = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 depth = ll_check_l_int32_default(_fun, L, 4, 1);
    lua_pushboolean(L, 0 == pixSetDimensions(pix, width, height, depth));
    return 1;
}

/**
 * \brief Copy dimensions from a Pix* (pixs) to self (pixd)
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
    FUNC(LL_PIX ".CopyDimensions");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopyDimensions(pixd, pixs));
    return 1;
}

/**
 * \brief Get the Pix* (%pix) samples per pixel (%spp)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetSpp(lua_State *L)
{
    FUNC(LL_PIX ".GetSpp");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 spp = pixGetSpp(pix);
    lua_pushinteger(L, spp);
    return 1;
}

/**
 * \brief Set the Pix* SPP (samples per pixel)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetSpp(lua_State *L)
{
    FUNC(LL_PIX ".SetSpp");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 spp = ll_check_l_int32_default(_fun, L, 2, pixGetSpp(pix));
    lua_pushboolean(L, 0 == pixSetSpp(pix, spp));
    return 1;
}

/**
 * \brief Copy samples per pixel (%spp) from a Pix* (%pixs) to Pix* (%pixd)
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
    FUNC(LL_PIX ".CopySpp");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopySpp(pixd, pixs));
    return 1;
}

/**
 * \brief Get the Pix* (%pix) words per line (%wpl)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWpl(lua_State *L)
{
    FUNC(LL_PIX ".GetWpl");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = pixGetWpl(pix);
    lua_pushinteger(L, wpl);
    return 1;
}

/**
 * \brief Set the Pix* (%pix) words per line (%wpl)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWpl(lua_State *L)
{
    FUNC(LL_PIX ".SetWpl");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = ll_check_l_int32_default(_fun, L, 2, pixGetWpl(pix));
    lua_pushboolean(L, 0 == pixSetWpl(pix, wpl));
    return 1;
}

/**
 * \brief Get the Pix* X resolution
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetXRes(lua_State *L)
{
    FUNC(LL_PIX ".GetXRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = pixGetXRes(pix);
    lua_pushinteger(L, xres);
    return 1;
}

/**
 * \brief Set the Pix* X resolution
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetXRes(lua_State *L)
{
    FUNC(LL_PIX ".SetXRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32_default(_fun, L, 2, pixGetXRes(pix));
    lua_pushboolean(L, 0 == pixSetXRes(pix, xres));
    return 1;
}

/**
 * \brief Get the Pix* Y resolution
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetYRes(lua_State *L)
{
    FUNC(LL_PIX ".GetYRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 yres = pixGetYRes(pix);
    lua_pushinteger(L, yres);
    return 1;
}

/**
 * \brief Set the Pix* Y resolution
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetYRes(lua_State *L)
{
    FUNC(LL_PIX ".SetYRes");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 yres = ll_check_l_int32_default(_fun, L, 2, pixGetYRes(pix));
    lua_pushboolean(L, 0 == pixSetYRes(pix, yres));
    return 1;
}

/**
 * \brief Get the PIX resolution (x, y)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 for two integer on the Lua stack
 */
static int
GetResolution(lua_State *L)
{
    FUNC(LL_PIX ".GetResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres, yres;
    if (pixGetResolution(pix, &xres, &yres))
        return ll_push_nil(L);
    lua_pushinteger(L, xres);
    lua_pushinteger(L, yres);
    return 2;
}

/**
 * \brief Set the PIX side resolution (x, y)
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
    FUNC(LL_PIX ".SetResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 xres = ll_check_l_int32_default(_fun, L, 2, 300);
    l_int32 yres = ll_check_l_int32_default(_fun, L, 3, xres);
    lua_pushboolean(L, 0 == pixSetResolution(pix, xres, yres));
    return 1;
}

/**
 * \brief Copy resolution from a Pix* (%pixs) to a Pix* (%pixd)
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
    FUNC(LL_PIX ".CopyResolution");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopyResolution(pixd, pixs));
    return 1;
}

/**
 * \brief Scale resolution of a Pix* (%pix) by a factor (%xscale, %yscale)
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
    FUNC(LL_PIX ".ScaleResolution");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_float32 xscale = ll_check_l_float32(_fun, L, 2);
    l_float32 yscale = ll_check_l_float32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixScaleResolution(pix, xscale, yscale));
    return 1;
}

/**
 * \brief Get the input format of a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetInputFormat(lua_State *L)
{
    FUNC(LL_PIX ".GetInputFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushstring(L, ll_string_input_format(pixGetInputFormat(pix)));
    return 1;
}

/**
 * \brief Set the input format of a Pix*
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
    FUNC(LL_PIX ".SetInputFormat");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 format = ll_check_input_format(_fun, L, 2, IFF_UNKNOWN);
    lua_pushboolean(L, 0 == pixSetInputFormat(pix, format));
    return 1;
}

/**
 * \brief Copy input format a Pix* (%pixs) to a Pix* (%pixd)
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
    FUNC(LL_PIX ".CopyInputFormat");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopyInputFormat(pixd, pixs));
    return 1;
}

/**
 * \brief Set the special value (%special) of a Pix* (%pix)
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
    FUNC(LL_PIX ".SetSpecial");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 special = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSetSpecial(pix, special));
    return 1;
}

/**
 * \brief Get the text of a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetText(lua_State *L)
{
    FUNC(LL_PIX ".GetText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = pixGetText(pix);
    lua_pushstring(L, text);
    return 1;
}

/**
 * \brief Set the text of a Pix*
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
    FUNC(LL_PIX ".SetText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, pixSetText(pix, text));
    return 1;
}

/**
 * \brief Add to the text of a Pix*
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
    FUNC(LL_PIX ".AddText");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char* text = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, pixAddText(pix, text));
    return 1;
}

/**
 * \brief Copy text from a Pix* (%pixs) to Pix* (%pixd)
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
    FUNC(LL_PIX ".CopyText");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    lua_pushboolean(L, 0 == pixCopyText(pixd, pixs));
    return 1;
}

/**
 * \brief Get the colormap of a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
GetColormap(lua_State *L)
{
    FUNC(LL_PIX ".GetColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* cmap = pixcmapCopy(pixGetColormap(pix));
    return ll_push_PixColormap(_fun, L, cmap);
}

/**
 * \brief Set the colormap of a Pix*
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
    FUNC(LL_PIX ".SetColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* colormap = ll_take_PixColormap(L, 2);
    lua_pushboolean(L, 0 == pixSetColormap(pix, colormap));
    return 1;
}

/**
 * \brief Destroy the colormap of a Pix*
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
    FUNC(LL_PIX ".DestroyColormap");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    PixColormap* colormap = ll_take_PixColormap(L, 2);
    lua_pushboolean(L, 0 == pixDestroyColormap(pix));
    (void)colormap;
    return 1;
}

/**
 * \brief Get the data of a Pix* (%pix)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 array table (h) of array tables (wpl) on the Lua stack
 */
static int
GetData(lua_State *L)
{
    FUNC(LL_PIX ".GetData");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);
    return ll_push_uarray_2d(L, data, wpl, h);
}

/**
 * \brief Set the data of a Pix* (%pix)
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
    FUNC(LL_PIX ".SetData");
    Pix* pix = ll_check_Pix(_fun, L, 1);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);
    size_t size = static_cast<size_t>(wpl) * static_cast<size_t>(h) * sizeof(l_uint32);
    l_uint32 *data = ll_malloc<l_uint32>(_fun, L, size);
    /* copy the previous Pix->data in case only a part of data[][] is written */
    memcpy(data, pixGetData(pix), size);
    data = ll_unpack_uarray_2d(_fun, L, 2, data, wpl, h);
    lua_pushboolean(L, 0 == pixSetData(pix, data));
    /* Do not free(data); it is owned by the Pix* now */
    return 1;
}

/**
 * \brief Get a pixel value from Pix*
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
    FUNC(LL_PIX ".GetPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_uint32 val = 0;
    if (pixGetPixel(pix, x, y, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Set a pixel value in Pix*
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
    FUNC(LL_PIX ".SetPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_uint32 val = ll_check_l_uint32(_fun, L, 4) - 1;
    lua_pushboolean(L, 0 == pixSetPixel(pix, x, y, val));
    return 1;
}

/**
 * \brief Get a pixel's RGB values from Pix*
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
    FUNC(LL_PIX ".GetRGBPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    if (pixGetRGBPixel(pix, x, y, &rval, &gval, &bval))
        return ll_push_nil(L);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    return 3;
}

/**
 * \brief Set a pixel's RGB values in Pix*
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
    FUNC(LL_PIX ".SetRGBPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    l_int32 rval = ll_check_l_int32(_fun, L, 4);
    l_int32 gval = ll_check_l_int32(_fun, L, 5);
    l_int32 bval = ll_check_l_int32(_fun, L, 6);
    lua_pushboolean(L, 0 == pixSetRGBPixel(pix, x, y, rval, gval, bval));
    return 1;
}

/**
 * \brief Get a random pixel's value from Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack (val, x, y)
 */
static int
GetRandomPixel(lua_State *L)
{
    FUNC(LL_PIX ".GetRandomPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = 0;
    l_int32 y = 0;
    l_uint32 val = 0;
    if (pixGetRandomPixel(pix, &val, &x, &y))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 3;
}

/**
 * \brief Clear a pixel value in Pix*
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
    FUNC(LL_PIX ".ClearPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixClearPixel(pix, x, y));
    return 1;
}

/**
 * \brief Flip a pixel value in Pix*
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
    FUNC(LL_PIX ".FlipPixel");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 x = ll_check_l_int32(_fun, L, 2);
    l_int32 y = ll_check_l_int32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixFlipPixel(pix, x, y));
    return 1;
}

/**
 * \brief Get black or white value in Pix*
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
    FUNC(LL_PIX ".GetBlackOrWhiteVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 op = ll_check_getval(_fun, L, 2, L_GET_BLACK_VAL);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, op, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get black value in Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackVal(lua_State *L)
{
    FUNC(LL_PIX ".GetBlackVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_BLACK_VAL, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get white value in Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWhiteVal(lua_State *L)
{
    FUNC(LL_PIX ".GetWhiteVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_WHITE_VAL, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Clear all pixels in a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClearAll(lua_State *L)
{
    FUNC(LL_PIX ".ClearAll");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushboolean(L, 0 == pixClearAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a Pix*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolane on the Lua stack
 */
static int
SetAll(lua_State *L)
{
    FUNC(LL_PIX ".SetAll");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushboolean(L, 0 == pixSetAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to a gray value (%grayval)
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
    FUNC(LL_PIX ".SetAllGray");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 grayval = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSetAllGray(pix, grayval));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to an arbitrary value %val
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
    FUNC(LL_PIX ".SetAllArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSetAllArbitrary(pix, val));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to black or white
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
    FUNC(LL_PIX ".SetBlackOrWhite");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 op = ll_check_blackwhite(_fun, L, 2, L_SET_BLACK);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, op));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to black
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlack(lua_State *L)
{
    FUNC(LL_PIX ".SetBlack");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_BLACK));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to white
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetWhite(lua_State *L)
{
    FUNC(LL_PIX ".SetWhite");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_WHITE));
    return 1;
}

/**
 * \brief Set all pixel components (%comp) in a Pix* (%pix) to a value (%val)
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
    FUNC(LL_PIX ".SetComponentArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 comp = ll_check_component(_fun, L, 2, 0);
    l_int32 val = ll_check_l_int32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixSetComponentArbitrary(pix, comp, val));
    return 1;
}

/**
 * \brief Clear all pixels inside a Box* (%box) in a Pix* (%pix)
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
    FUNC(LL_PIX ".ClearInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    lua_pushboolean(L, 0 == pixClearInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a Box* (%box) in a Pix* (%pix)
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
    FUNC(LL_PIX ".SetInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSetInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a Box* (%box) in a Pix* (%pix) to a value (%val)
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
    FUNC(LL_PIX ".SetInRectArbitrary");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixSetInRectArbitrary(pix, box, val));
    return 1;
}

/**
 * \brief Blend all pixels inside a Box* (%box) in a Pix* (%pix) with value fraction (%val, %fract)
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
    FUNC(LL_PIX ".BlendInRect");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    l_float32 fract = ll_check_l_float32(_fun, L, 4);
    lua_pushboolean(L, 0 == pixBlendInRect(pix, box, val, fract));
    return 1;
}

/**
 * \brief Set pad bits in a Pix* (%pix) to value (%val)
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
    FUNC(LL_PIX ".SetPadBits");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    lua_pushboolean(L, 0 == pixSetPadBits(pix, val));
    return 1;
}

/**
 * \brief Set pad bits in a Pix* (%pix) band (%by, %bh) to value (%val)
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
    FUNC(LL_PIX ".SetPadBitsBand");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 by = ll_check_l_int32(_fun, L, 2);
    l_int32 bh = ll_check_l_int32(_fun, L, 3);
    l_int32 val = ll_check_l_int32(_fun, L, 4);
    lua_pushboolean(L, 0 == pixSetPadBitsBand(pix, by, bh, val));
    return 1;
}

/**
 * \brief Set or clear border pixels in a Pix* (%pix)
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
    FUNC(LL_PIX ".SetOrClearBorder");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_int32 op = ll_check_rasterop(_fun, L, 6, PIX_CLR);
    lua_pushboolean(L, 0 == pixSetOrClearBorder(pix, left, right, top, bottom, op));
    return 1;
}

/**
 * \brief Set border pixels in a Pix* (%pix) to value (%val)
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
    FUNC(LL_PIX ".SetBorderVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    l_uint32 val = ll_check_l_uint32(_fun, L, 6);
    lua_pushboolean(L, 0 == pixSetBorderVal(pix, left, right, top, bottom, val));
    return 1;
}

/**
 * \brief Set border ring pixels in a Pix* (%pix) to value (%val)
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
    FUNC(LL_PIX ".SetBorderRingVal");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 dist = ll_check_l_int32(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixSetBorderRingVal(pix, dist, val));
    return 1;
}

/**
 * \brief Set mirrored border pixels in a Pix* (%pix)
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
    FUNC(LL_PIX ".SetMirroredBorder");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 left = ll_check_l_int32(_fun, L, 2);
    l_int32 right = ll_check_l_int32(_fun, L, 3);
    l_int32 top = ll_check_l_int32(_fun, L, 4);
    l_int32 bottom = ll_check_l_int32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixSetMirroredBorder(pix, left, right, top, bottom));
    return 1;
}

/**
 * \brief Copy border pixels from a Pix* (%pixs) to a new Pix* (%pix)
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
    FUNC(LL_PIX ".CopyBorder");
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
 * \brief Add border pixels in a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".AddBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 npix = ll_check_l_int32(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    Pix* pixd = pixAddBorder(pixs, npix, val);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Add black or white border pixels in a Pix* to a new Pix*
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
    FUNC(LL_PIX ".AddBlackOrWhiteBorder");
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
 * \brief Add border pixels general (%val) in a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".AddBorderGeneral");
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
 * \brief Remove border pixels (%npix) from a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".RemoveBorder");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 npix = ll_check_l_int32(_fun, L, 2);
    Pix* pixd = pixRemoveBorder(pixs, npix);
    ll_push_Pix(_fun, L, pixd);
    return 1;
}

/**
 * \brief Remove border pixels in a Pix* to a new Pix* (general case)
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
    FUNC(LL_PIX ".RemoveBorderGeneral");
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
 * \brief Remove border pixels resizing a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".RemoveBorderToSize");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    l_int32 height = ll_check_l_int32(_fun, L, 3);
    Pix* pix = pixRemoveBorderToSize(pixs, width, height);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Add mirrored border pixels in a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".AddMirroredBorder");
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
 * \brief Add repeated border pixels in a Pix* to a new Pix*
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
    FUNC(LL_PIX ".AddRepeatedBorder");
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
 * \brief Add mixed border pixels in a Pix* (%pixs) to a new Pix* (%pixd)
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
    FUNC(LL_PIX ".AddMixedBorder");
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
 * \brief Add continued border pixels in a Pix* to a new Pix*
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
    FUNC(LL_PIX ".AddContinuedBorder");
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
 * \brief Shift and transfer alpha channel from a Pix* (%pixs) to a Pix* (%pixd)
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
    FUNC(LL_PIX ".ShiftAndTransferAlpha");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 shiftx = ll_check_l_int32(_fun, L, 3);
    l_int32 shifty = ll_check_l_int32(_fun, L, 4);
    lua_pushboolean(L, pixShiftAndTransferAlpha(pixd, pixs, shiftx, shifty));
    return 1;
}

/**
 * \brief Display the layers of a Pix* (pixd)
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
    FUNC(LL_PIX ".DisplayLayersRGBA");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    l_int32 maxw = ll_check_l_int32(_fun, L, 3);
    Pix *pixd = pixDisplayLayersRGBA(pixs, val, maxw);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Create a new Pix* (%pixd) from three layer Pix* (%pixr, %pixg, %pixb)
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
    FUNC(LL_PIX ".CreateRGBImage");
    Pix *pixr = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    Pix *pixb = ll_check_Pix(_fun, L, 3);
    Pix *pixd = pixCreateRGBImage(pixr, pixg, pixb);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Create a new Pix* (%pixd) from one component (%comp) of Pix* (%pixs)
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
    FUNC(LL_PIX ".GetRGBComponent");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 comp = ll_check_component(_fun, L, 2, L_ALPHA_CHANNEL);
    Pix *pixd = pixGetRGBComponent(pixs, comp);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Set one component in Pix* (pixd 32bpp) from Pix* (pixs 8bpp)
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
    FUNC(LL_PIX ".SetRGBComponent");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 comp = ll_check_component(_fun, L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixSetRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Copy one component in Pix* (%pixd) from Pix* (%pixs)
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
    FUNC(LL_PIX ".CopyRGBComponent");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    l_int32 comp = ll_check_component(_fun, L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixCopyRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Extract red, green and blue components from Pix* (pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetRGBLine(lua_State *L)
{
    FUNC(LL_PIX ".GetRGBLine");
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
 * \brief Check alpha layer of a Pix* for opaqueness
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AlphaIsOpaque(lua_State *L)
{
    FUNC(LL_PIX ".AlphaIsOpaque");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 opaque = 0;
    if (pixAlphaIsOpaque(pix, &opaque))
        return ll_push_nil(L);
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm)
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
    FUNC(LL_PIX ".SetMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    lua_pushboolean(L, 0 == pixSetMasked(pixd, pixm, val));
    return 1;
}

/**
 * \brief Set the mask value (%val) for a Pix* (%pixd) at offset (%x,%y) from another Pix* (%pixm)
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
    FUNC(LL_PIX ".SetMaskedGeneral");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_uint32 val = ll_check_l_uint32(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixSetMaskedGeneral(pixd, pixm, val, x, y));
    return 1;
}

/**
 * \brief Combine the Pix* (%pixs) with Pix* (%pixd) using another Pix* (%pixm) as mask
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
    FUNC(LL_PIX ".CombineMasked");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    lua_pushboolean(L, 0 == pixCombineMasked(pixd, pixs, pixm));
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp) at offset %x and %y
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
    FUNC(LL_PIX ".CombineMaskedGeneral");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pixm = ll_check_Pix(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixCombineMaskedGeneral(pixd, pixs, pixm, x, y));
    return 1;
}

/**
 * \brief Paint value (%val) through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset (%x, %y)
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
    FUNC(LL_PIX ".PaintThroughMask");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_uint32 val = ll_check_l_uint32(_fun, L, 5);
    lua_pushboolean(L, 0 == pixPaintThroughMask(pixd, pixm, x, y, val));
    return 1;
}

/**
 * \brief Paint %val through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset %x and %y
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
    FUNC(LL_PIX ".PaintSelfThroughMask");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 searchdir = ll_check_searchdir(_fun, L, 5, L_BOTH_DIRECTIONS);
    l_int32 mindist = ll_check_l_int32(_fun, L, 6);
    l_int32 tilesize = ll_check_l_int32(_fun, L, 7);
    l_int32 ntiles = ll_check_l_int32(_fun, L, 8);
    l_int32 distblend = ll_check_l_int32_default(_fun, L, 9, 0);
    lua_pushboolean(L, 0 == pixPaintSelfThroughMask(pixd, pixm, x, y, searchdir, mindist, tilesize, ntiles, distblend));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a mask value (%val)
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
    FUNC(LL_PIX ".MakeMaskFromVal");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    ll_push_Pix(_fun, L, pixMakeMaskFromVal(pixs, val));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a 2^depth entry lookup-table (%lut)
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
    FUNC(LL_PIX ".MakeMaskFromLUT");
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
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using arithmetic factors (%rc, %gc, %bc)
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
    FUNC(LL_PIX ".MakeArbMaskFromRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 rc = ll_check_l_float32(_fun, L, 2);
    l_float32 gc = ll_check_l_float32(_fun, L, 3);
    l_float32 bc = ll_check_l_float32(_fun, L, 4);
    l_float32 thresh = ll_check_l_float32(_fun, L, 5);
    ll_push_Pix(_fun, L, pixMakeArbMaskFromRGB(pixs, rc, gc, bc, thresh));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from Pix* (%pixs) seting alpha == 0 pixels to a value (%val)
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
    FUNC(LL_PIX ".SetUnderTransparency");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_uint32 val = ll_check_l_uint32(_fun, L, 2);
    ll_push_Pix(_fun, L, pixSetUnderTransparency(pixs, val, 0));
    return 1;
}

/**
 * \brief Create a new alpha mask Pix* (%pixd) from Pix* (%pixs)
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
    FUNC(LL_PIX ".MakeAlphaFromMask");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 dist = ll_check_l_int32(_fun, L, 2);
    int getbox = ll_check_boolean_default(_fun, L, 3, FALSE);
    Box* box = nullptr;
    Pix* pixd = pixMakeAlphaFromMask(pixs, dist, getbox ? &box : nullptr);
    if (!pixd)
        return ll_push_nil(L);
    ll_push_Pix(_fun, L, pixd);
    return 1 + box ? ll_push_Box(_fun, L, box) : 0;
}

/**
 * \brief Get the color near the mask boundary from Pix* (%pixs)
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
    FUNC(LL_PIX ".GetColorNearMaskBoundary");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    l_uint32 val = 0;
    l_int32 dist = ll_check_l_int32(_fun, L, 4);
    if (pixGetColorNearMaskBoundary(pixs, pixm, box, dist, &val, 0))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Invert the Pix* (%pixs)
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
    FUNC(LL_PIX ".Invert");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix_opt(_fun, L, 2);
    Pix *pix = pixInvert(pixs ? nullptr : pixd, pixs ? pixs : pixd);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief And the Pix* (%pix1) and Pix* (%pix2)
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
    FUNC(LL_PIX ".And");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixAnd(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Or the Pix* (%pix1) and Pix* (%pix2)
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
    FUNC(LL_PIX ".Or");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixOr(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Xor the Pix* (%pix1) and Pix* (%pix2)
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
    FUNC(LL_PIX ".Xor");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixXor(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Subtract the Pix* (%pix2) from Pix* (%pix1)
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
    FUNC(LL_PIX ".Subtract");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Pix *pix = pixSubtract(pixd, pixd, pixs);
    ll_push_Pix(_fun, L, pix);
    return 1;
}

/**
 * \brief Check if all pixels in Pix* (%pixs) are 0
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Zero(lua_State *L)
{
    FUNC(LL_PIX ".Zero");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 empty = 0;
    if (pixZero(pixs, &empty))
        return ll_push_nil(L);
    lua_pushboolean(L, empty);
    return 1;
}

/**
 * \brief Calculate the fraction of foreground in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
ForegroundFraction(lua_State *L)
{
    FUNC(LL_PIX ".ForegroundFraction");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0;
    if (pixForegroundFraction(pixs, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(fract));
    return 1;
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixels(lua_State *L)
{
    FUNC(LL_PIX ".CountPixels");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 count = 0;
    if (pixCountPixels(pixs, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs) inside a Box* (%box)
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
    FUNC(LL_PIX ".CountPixelsInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRect(pixs, box, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of pixels by row in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByRow(lua_State *L)
{
    FUNC(LL_PIX ".CountPixelsByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Numa *na = pixCountPixelsByRow(pixs, nullptr);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Count the number of pixels by column in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByColumn(lua_State *L)
{
    FUNC(LL_PIX ".CountPixelsByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Numa *na = pixCountPixelsByColumn(pixs);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Count the number of pixels in row (%row) of Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsInRow(lua_State *L)
{
    FUNC(LL_PIX ".CountPixelsInRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRow(pixs, row, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the moment of order (%order) by column in Pix* (%pixs)
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
    FUNC(LL_PIX ".GetMomentByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 order = ll_check_l_int32(_fun, L, 2);
    Numa *na = pixGetMomentByColumn(pixs, order);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Check if the pixel sum of Pix* (%pixs) is above threshold (%thresh)
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
    FUNC(LL_PIX ".ThresholdPixelSum");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 thresh = ll_check_l_int32(_fun, L, 2);
    l_int32 above = 0;
    if (pixThresholdPixelSum(pixs, thresh, &above, nullptr))
        return ll_push_nil(L);
    lua_pushboolean(L, above);
    return 1;
}

/**
 * \brief Build the average by row of Pix* (%pixs)
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
    FUNC(LL_PIX ".AverageByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 type = ll_check_what_is_max(_fun, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByRow(pixs, box, type);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the average by column of Pix* (%pixs)
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
    FUNC(LL_PIX ".AverageByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 type = ll_check_what_is_max(_fun, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByColumn(pixs, box, type);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the average inside a Box* (%box) of Pix* (%pixs)
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
    FUNC(LL_PIX ".AverageInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_float32 ave = 0.0f;
    if (pixAverageInRect(pixs, box, &ave))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(ave));
    return 1;
}

/**
 * \brief Build the variance by row of Pix* (%pixs)
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
    FUNC(LL_PIX ".VarianceByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixVarianceByRow(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the variance by column of Pix* (%pixs)
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
    FUNC(LL_PIX ".VarianceByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixVarianceByColumn(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the square root of the variance inside a Box* (%box) of Pix* (%pixs)
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
    FUNC(LL_PIX ".VarianceInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_float32 sqrvar = 0.0f;
    if (pixVarianceInRect(pixs, box, &sqrvar))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(sqrvar));
    return 1;
}

/**
 * \brief Build the absolute difference by row of Pix* (%pixs)
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
    FUNC(LL_PIX ".AbsDiffByRow");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixAbsDiffByRow(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the absolute difference by column of Pix* (%pixs)
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
    FUNC(LL_PIX ".AbsDiffByColumn");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    Numa* na = pixAbsDiffByColumn(pixs, box);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Build the absolute difference inside a Box* (%box) of Pix* (%pixs)
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
    FUNC(LL_PIX ".AbsDiffInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_int32 dir = ll_check_direction(_fun, L, 3, L_HORIZONTAL_LINE);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffInRect(pixs, box, dir, &absdiff))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(absdiff));
    return 1;
}

/**
 * \brief Build absolute difference on a line (%x1,%y1 to %x2,%y2) of Pix* (%pixs)
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
    FUNC(LL_PIX ".AbsDiffOnLine");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 x1 = ll_check_l_int32(_fun, L, 2);
    l_int32 y1 = ll_check_l_int32(_fun, L, 3);
    l_int32 x2 = ll_check_l_int32(_fun, L, 4);
    l_int32 y2 = ll_check_l_int32(_fun, L, 5);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffOnLine(pixs, x1, y1, x2, y2, &absdiff))
        return ll_push_nil(L);
    lua_pushnumber(L, static_cast<lua_Number>(absdiff));
    return 1;
}

/**
 * \brief Count pixels of arbitrary value (%val) in Box* (%box) of Pix* (%pixs)
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
    FUNC(LL_PIX ".CountArbInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 val = ll_check_l_int32(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    Box *box = ll_check_Box_opt(_fun, L, 4);
    l_int32 count;
    if (pixCountArbInRect(pixs, box, val, factor, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Create a mirrored tiling of the Pix* (%pixs)
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
    FUNC(LL_PIX ".MirroredTiling");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 h = ll_check_l_int32(_fun, L, 3);
    return ll_push_Pix(_fun, L, pixMirroredTiling(pixs, w, h));
}

/**
 * \brief Look for one or two square tiles with conforming median
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
    FUNC(LL_PIX ".FindRepCloseTile");
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
 * \brief Get the histogram of the grayscale Pix* (%pixs)
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
    FUNC(LL_PIX ".GetGrayHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Numa(_fun, L, pixGetGrayHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) masked with another Pix* (%pixm)
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
    FUNC(LL_PIX ".GetGrayHistogramMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    return ll_push_Numa(_fun, L, pixGetGrayHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) inside Box* (%box)
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
    FUNC(LL_PIX ".GetGrayHistogramInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    return ll_push_Numa(_fun, L, pixGetGrayHistogramInRect(pixs, box, factor));
}

/**
 * \brief Get an array of histograms of the grayscale Pix* (%pixs) for %nx by %ny tiles
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
    FUNC(LL_PIX ".GetGrayHistogramTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 nx = ll_check_l_int32(_fun, L, 3);
    l_int32 ny = ll_check_l_int32(_fun, L, 4);
    return ll_push_Numaa(_fun, L, pixGetGrayHistogramTiled(pixs, factor, nx, ny));
}

/**
 * \brief Get the RGB histograms of the Pix* (%pixs)
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
    FUNC(LL_PIX ".GetColorHistogram");
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
 * \brief Get the RGB histograms of the Pix* (%pixs) masked with another Pix* (%pixm)
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
    FUNC(LL_PIX ".GetColorHistogramMasked");
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
 * \brief Get the histogram of the color mapped Pix* (%pixs)
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
    FUNC(LL_PIX ".GetCmapHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Numa(_fun, L, pixGetCmapHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) masked with another Pix* (%pixm)
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
    FUNC(LL_PIX ".GetCmapHistogramMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    return ll_push_Numa(_fun, L, pixGetCmapHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) inside Box* (%box)
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
    FUNC(LL_PIX ".GetCmapHistogramInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 factor = ll_check_l_int32(_fun, L, 3);
    return ll_push_Numa(_fun, L, pixGetCmapHistogramInRect(pixs, box, factor));
}

/**
 * \brief Count the RGB colors in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountRGBColors(lua_State *L)
{
    FUNC(LL_PIX ".CountRGBColors");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 count = pixCountRGBColors(pixs);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get a histogram AMAP* for the colors in Pix* (%pixs)
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
    FUNC(LL_PIX ".GetColorAmapHistogram");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    return ll_push_Amap(_fun, L, pixGetColorAmapHistogram(pixs, factor));
}

/**
 * \brief Get rank value for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetRankValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_float32 rank = ll_check_l_float32(_fun, L, 3);
    l_uint32 value = 0;
    if (pixGetRankValue(pixs, factor, rank, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get rank values for RGB for Pix* (%pixs) masked with another Pix* (%pixm)
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
    FUNC(LL_PIX ".GetRankValueMaskedRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_int32 factor = ll_check_l_int32(_fun, L, 5);
    l_float32 rank = ll_check_l_int32(_fun, L, 6);
    l_float32 rval = 0.0f, gval = 0.0f, bval = 0.0f;
    if (pixGetRankValueMaskedRGB(pixs, pixm, x, y, factor, rank, &rval, &gval, &bval))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)rval);
    lua_pushnumber(L, (lua_Number)gval);
    lua_pushnumber(L, (lua_Number)bval);
    return 3;
}

/**
 * \brief Get rank value for Pix* (%pixs) masked with another Pix* (%pixm)
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
    FUNC(LL_PIX ".GetRankValueMasked");
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
    lua_pushnumber(L, (lua_Number)value);
    ll_push_Numa(_fun, L, na);
    return 1;
}

/**
 * \brief Get the pixel average for Pix* (%pixs) optionally masked with Pix* (%pixm)
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
    FUNC(LL_PIX ".GetPixelAverage");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 2);
    l_int32 x = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 5, 1);
    l_uint32 value = 0;
    if (pixGetPixelAverage(pixs, pixm, x,y, factor, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel stats for Pix* (%pixs) optionally masked with Pix* (%pixm)
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
    FUNC(LL_PIX ".GetPixelStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_uint32 value = 0;
    if (pixGetPixelStats(pixs, factor, type, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel average RGB values for Pix* (%pixs) optionally masked with Pix* (%pixm)
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
    FUNC(LL_PIX ".GetAverageMaskedRGB");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 3);
    l_int32 x = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 6, 1);
    l_float32 rval = 0.0, gval = 0.0, bval = 0.0;
    if (pixGetAverageMaskedRGB(pixs, pixm, x, y, factor, type, &rval, &gval, &bval))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)rval);
    lua_pushnumber(L, (lua_Number)gval);
    lua_pushnumber(L, (lua_Number)bval);
    return 3;
}

/**
 * \brief Get the pixel average value for Pix* (%pixs) optionally masked with Pix* (%pixm)
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
    FUNC(LL_PIX ".GetAverageMasked");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = ll_check_Pix_opt(_fun, L, 3);
    l_int32 x = ll_check_l_int32_default(_fun, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(_fun, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 6, 1);
    l_float32 value = 0.0;
    if (pixGetAverageMasked(pixs, pixm, x, y, factor, type, &value))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)value);
    return 3;
}

/**
 * \brief Get the average RGB values for Pix* (%pixs) as three Pix* (%pixr, %pixg, %pixb)
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
    FUNC(LL_PIX ".GetAverageTiledRGB");
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
 * \brief Get the average value for Pix* (%pixs) as tiled Pix* (%pixv)
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
    FUNC(LL_PIX ".GetAverageTiled");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(_fun, L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(_fun, L, 4, 2);
    Pix *pixv = pixGetAverageTiled(pixs, sx, sy, type);
    return ll_push_Pix(_fun, L, pixv);
}

/**
 * \brief Get the row stats for Pix* (%pixs) as six Numa*
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
    FUNC(LL_PIX ".RowStats");
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
 * \brief Get the column stats for Pix* (%pixs) as six Numa*
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
    FUNC(LL_PIX ".ColumnStats");
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
 * \brief Get the range values for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetRangeValues");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 color = ll_check_select_color(_fun, L, 3, L_SELECT_RED);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    if (pixGetRangeValues(pixs, factor, color, &minval, &maxval))
        return ll_push_nil(L);
    lua_pushinteger(L, minval);
    lua_pushinteger(L, maxval);
    return 2;
}

/**
 * \brief Get an extreme value for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetExtremeValue");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 type = ll_check_select_min_max(_fun, L, 3, L_SELECT_MIN);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 grayval = 0;
    if (pixGetExtremeValue(pixs, factor, type, &rval, &gval, &bval, &grayval))
        return ll_push_nil(L);
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    lua_pushinteger(L, grayval);
    return 4;
}

/**
 * \brief Get the maximum value for Pix* (%pixs) optionally in rect Box* (%box)
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
    FUNC(LL_PIX ".GetMaxValueInRect");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Box *box = ll_check_Box_opt(_fun, L, 2);
    l_uint32 maxval = 0;
    l_int32 xmax = 0;
    l_int32 ymax = 0;
    if (pixGetMaxValueInRect(pixs, box, &maxval, &xmax, &ymax))
        return ll_push_nil(L);
    lua_pushinteger(L, maxval);
    lua_pushinteger(L, xmax);
    lua_pushinteger(L, ymax);
    return 3;
}

/**
 * \brief Get a binned component range for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetBinnedComponentRange");
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
    lua_pushinteger(L, minval);
    lua_pushinteger(L, maxval);
    res = ll_push_uarray(L, carray, nbins);
    LEPT_FREE(carray);
    return 2 + res;
}

/**
 * \brief Get a rank color array for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetRankColorArray");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 type = ll_check_select_color(_fun, L, 4, L_SELECT_AVERAGE);
    l_uint32 *carray = nullptr;
    l_int32 res;

    if (pixGetRankColorArray(pixs, nbins, type, factor, &carray, 0, 0))
        return ll_push_nil(L);
    res = ll_push_uarray(L, carray, nbins);
    LEPT_FREE(carray);
    return res;
}

/**
 * \brief Get a binned color for Pix* (%pixs)
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
    FUNC(LL_PIX ".GetBinnedColor");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Pix *pixg = ll_check_Pix(_fun, L, 2);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 4, 1);
    Numa *alut = ll_check_Numa(_fun, L, 5);
    l_uint32 *carray = nullptr;
    l_int32 res;

    if (pixGetBinnedColor(pixs, pixg, factor, nbins, alut, &carray, 0))
        return ll_push_nil(L);
    res = ll_push_uarray(L, carray, nbins);
    LEPT_FREE(carray);
    return res;
}

/**
 * \brief Display a color array creating a Pix* (%pix)
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
    FUNC(LL_PIX ".DisplayColorArray");
    l_int32 side = ll_check_l_int32(_fun, L, 1);
    l_int32 ncols = ll_check_l_int32(_fun, L, 2);
    l_int32 fontsize = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 ncolors = 0;
    l_uint32 *carray = ll_unpack_uarray(_fun, L, 4, &ncolors);
    Pix *pixd = pixDisplayColorArray(carray, ncolors, side, ncols, fontsize);
    LEPT_FREE(carray);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Get a binned color for Pix* (%pixs)
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
    FUNC(LL_PIX ".RankBinByStrip");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_select_color(_fun, L, 2, L_SELECT_RED);
    l_int32 direction = ll_check_direction(_fun, L, 3, L_SCAN_HORIZONTAL);
    l_int32 size = ll_check_l_int32_default(_fun, L, 4, 200);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 5, 2);
    Pix *pixd = pixRankBinByStrip(pixs, direction, size, nbins, type);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Get row stats for Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetRowStats(lua_State *L)
{
    FUNC(LL_PIX ".GetRowStats");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 type = ll_check_select_color(_fun, L, 2, L_SELECT_RED);
    l_int32 nbins = ll_check_l_int32(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    l_float32 *colvect = ll_calloc<l_float32>(_fun, L, nbins);
    if (!colvect) {
        lua_pushfstring(L, "%s: could not allocate colvect (%d)",
                        _fun, static_cast<size_t>(nbins) * sizeof(*colvect));
        lua_error(L);
        return 0;
    }
    if (pixGetRowStats(pixs, type, nbins, thresh, colvect)) {
        LEPT_FREE(colvect);
        return ll_push_nil(L);
    }
    ll_push_farray(L, colvect, nbins);
    LEPT_FREE(colvect);
    return 1;
}

/**
 * \brief Get column stats for Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetColumnStats(lua_State *L)
{
    FUNC(LL_PIX ".GetColumnStats");
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
    ll_push_farray(L, rowvect, nbins);
    LEPT_FREE(rowvect);
    return 1;
}

/**
 * \brief Set a pixel column in Pix* (%pixd)
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
    FUNC(LL_PIX ".SetPixelColumn");
    Pix *pixd = ll_check_Pix(_fun, L, 1);
    l_int32 col = ll_check_l_int32(_fun, L, 2);
    l_int32 rows = pixGetHeight(pixd);
    l_int32 n;
    l_float32 *tblvect = ll_unpack_farray(_fun, L, 3, &n);
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
    lua_pushboolean(L, 0 == result);
    return 1;
}

/**
 * \brief Get the thresholds for bg/fg in Pix* (%pixs)
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
    FUNC(LL_PIX ".ThresholdForFgBg");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 3, 0);
    l_int32 fgval;
    l_int32 bgval;
    if (pixThresholdForFgBg(pixs, factor, thresh, &fgval, &bgval))
        return ll_push_nil(L);
    lua_pushinteger(L, fgval);
    lua_pushinteger(L, bgval);
    return 2;
}

/**
 * \brief Split the distribution for bg/fg in Pix* (%pixs)
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
    FUNC(LL_PIX ".SplitDistributionFgBg");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 scorefract = ll_check_l_float32_default(_fun, L, 2, 0.0f);
    l_int32 factor = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 thresh;
    l_int32 fgval;
    l_int32 bgval;
    if (pixSplitDistributionFgBg(pixs, scorefract, factor, &thresh, &fgval, &bgval, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, thresh);
    lua_pushinteger(L, fgval);
    lua_pushinteger(L, bgval);
    return 3;
}

/**
 * \brief Find the area to perimeter ratio in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
FindAreaPerimRatio(lua_State *L)
{
    FUNC(LL_PIX ".FindAreaPerimRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0.0f;
    if (pixFindAreaPerimRatio(pixs, tab8, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)fract);
    return 1;
}

/**
 * \brief Find the perimeter to area ratio in Pix* (%pixs)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (thresh, fgbal, bgval)
 */
static int
FindPerimToAreaRatio(lua_State *L)
{
    FUNC(LL_PIX ".FindPerimToAreaRatio");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_float32 fract = 0.0f;
    if (pixFindPerimToAreaRatio(pixs, tab8, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)fract);
    return 1;
}

/*
*
*    This file has these operations:
*
*      (1) Measurement of 1 bpp image properties
*      (2) Extract rectangular regions
*      (3) Clip to foreground
*      (4) Extract pixel averages, reversals and variance along lines
*      (5) Rank row and column transforms
*
*    Measurement of properties
*           l_int32     pixaFindDimensions()
*           l_int32     pixFindAreaPerimRatio()
*           Numa       *pixaFindPerimToAreaRatio()
*           l_int32     pixFindPerimToAreaRatio()
*           Numa       *pixaFindPerimSizeRatio()
*           l_int32     pixFindPerimSizeRatio()
*           Numa       *pixaFindAreaFraction()
*           l_int32     pixFindAreaFraction()
*           Numa       *pixaFindAreaFractionMasked()
*           l_int32     pixFindAreaFractionMasked()
*           Numa       *pixaFindWidthHeightRatio()
*           Numa       *pixaFindWidthHeightProduct()
*           l_int32     pixFindOverlapFraction()
*           Boxa       *pixFindRectangleComps()
*           l_int32     pixConformsToRectangle()
*
*    Extract rectangular region
*           Pixa       *pixClipRectangles()
*           Pix        *pixClipRectangle()
*           Pix        *pixClipMasked()
*           l_int32     pixCropToMatch()
*           Pix        *pixCropToSize()
*           Pix        *pixResizeToMatch()
*
*    Make a frame mask
*           Pix        *pixMakeFrameMask()
*
*    Fraction of Fg pixels under a mask
*           l_int32     pixFractionFgInMask()
*
*    Clip to foreground
*           Pix        *pixClipToForeground()
*           l_int32     pixTestClipToForeground()
*           l_int32     pixClipBoxToForeground()
*           l_int32     pixScanForForeground()
*           l_int32     pixClipBoxToEdges()
*           l_int32     pixScanForEdge()
*
*    Extract pixel averages and reversals along lines
*           Numa       *pixExtractOnLine()
*           l_float32   pixAverageOnLine()
*           Numa       *pixAverageIntensityProfile()
*           Numa       *pixReversalProfile()
*
*    Extract windowed variance along a line
*           Numa       *pixWindowedVarianceOnLine()
*
*    Extract min/max of pixel values near lines
*           l_int32     pixMinMaxNearLine()
*
*    Rank row and column transforms
*           Pix        *pixRankRowTransform()
*           Pix        *pixRankColumnTransform()
*/

/**
 * \brief Read Pix* from an external file (%filename)
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Read(lua_State *L)
{
    FUNC(LL_PIX ".Read");
    const char* filename = ll_check_string(_fun, L, 1);
    Pix *pix = pixRead(filename);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Read Pix* from a Lua io stream (%stream)
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    FUNC(LL_PIX ".ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 hint = ll_check_hint(_fun, L, 2, 0);
    Pix *pix = pixReadStream(stream->f, hint);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Read a Pix* from a Lua string (%data)
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    FUNC(LL_PIX ".ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pix *pix = pixReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Write the Pix* (%pix) to an external file (%filename)
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
    FUNC(LL_PIX ".Write");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 format = ll_check_input_format(_fun, L, 3, IFF_DEFAULT);
    lua_pushboolean(L, 0 == pixWrite(filename, pix, format));
    return 1;
}

/**
 * \brief Write the Pix* (%pix) to a Lua io stream (%stream)
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
    FUNC(LL_PIX ".WriteStream");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    l_int32 format = ll_check_input_format(_fun, L, 3, IFF_DEFAULT);
    lua_pushboolean(L, 0 == pixWriteStream(stream->f, pix, format));
    return 1;
}

/**
 * \brief Write the Pix* (%pix) to memory and return it as a Lua string
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be a string with the input format name (format).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    FUNC(LL_PIX ".WriteMem");
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
 * \brief Print info about a Pix* (%pix) to a Lua stream (%stream)
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
    FUNC(LL_PIX ".PrintStreamInfo");
    static char str[64];
    Pix *pix= ll_check_Pix(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    snprintf(str, sizeof(str), "%p\n", reinterpret_cast<void *>(pix));
    lua_pushboolean(L, 0 == pixPrintStreamInfo(stream->f, pix, str));
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PIX
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pix* contained in the user data
 */
Pix *
ll_check_Pix(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Pix **>(ll_check_udata(_fun, L, arg, LL_PIX)));
}

/**
 * \brief Optionally expect a LL_PIX at index %arg on the Lua stack
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
 * \brief Push PIX to the Lua stack and set its meta table
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
 * \brief Create and push a new Pix*
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
int
ll_new_Pix(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pix(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},           /* garbage collector */
        {"__new",                   Create},            /* new Pix */
        {"__sub",                   Subtract},
        {"__bnot",                  Invert},
        {"__band",                  And},
        {"__bor",                   Or},
        {"__bxor",                  Xor},
        {"__tostring",              toString},
        {"Copy",                    Copy},
        {"Clone",                   Clone},
        {"ResizeImageData",         ResizeImageData},
        {"CopyColormap",            CopyColormap},
        {"SizesEqual",              SizesEqual},
        {"TransferAllData",         TransferAllData},
        {"SwapAndDestroy",          SwapAndDestroy},
        {"GetWidth",                GetWidth},
        {"SetWidth",                SetWidth},
        {"GetHeight",               GetHeight},
        {"SetHeight",               SetHeight},
        {"GetDepth",                GetDepth},
        {"SetDepth",                SetDepth},
        {"GetDimensions",           GetDimensions},
        {"SetDimensions",           SetDimensions},
        {"CopyDimensions",          CopyDimensions},
        {"GetSpp",                  GetSpp},
        {"SetSpp",                  SetSpp},
        {"CopySpp",                 CopySpp},
        {"GetWpl",                  GetWpl},
        {"SetWpl",                  SetWpl},
        {"GetXRes",                 GetXRes},
        {"SetXRes",                 SetXRes},
        {"GetYRes",                 GetYRes},
        {"SetYRes",                 SetYRes},
        {"GetResolution",           GetResolution},
        {"SetResolution",           SetResolution},
        {"CopyResolution",          CopyResolution},
        {"ScaleResolution",         ScaleResolution},
        {"GetInputFormat",          GetInputFormat},
        {"SetInputFormat",          SetInputFormat},
        {"CopyInputFormat",         CopyInputFormat},
        {"SetSpecial",              SetSpecial},
        {"GetText",                 GetText},
        {"SetText",                 SetText},
        {"AddText",                 AddText},
        {"CopyText",                CopyText},
        {"GetColormap",             GetColormap},
        {"SetColormap",             SetColormap},
        {"DestroyColormap",         DestroyColormap},
        {"GetData",                 GetData},
        {"SetData",                 SetData},
        {"GetPixel",                GetPixel},
        {"SetPixel",                SetPixel},
        {"GetRGBPixel",             GetRGBPixel},
        {"SetRGBPixel",             SetRGBPixel},
        {"GetRandomPixel",          GetRandomPixel},
        {"ClearPixel",              ClearPixel},
        {"FlipPixel",               FlipPixel},
        {"GetBlackOrWhiteVal",      GetBlackOrWhiteVal},
        {"GetBlackVal",             GetBlackVal},       /* alias without 2nd parameter */
        {"GetWhiteVal",             GetWhiteVal},       /* alias without 2nd parameter */
        {"ClearAll",                ClearAll},
        {"SetAll",                  SetAll},
        {"SetAllGray",              SetAllGray},
        {"SetAllArbitrary",         SetAllArbitrary},
        {"SetBlackOrWhite",         SetBlackOrWhite},
        {"SetBlack",                SetBlack},
        {"SetWhite",                SetWhite},
        {"SetComponentArbitrary",   SetComponentArbitrary},
        {"ClearInRect",             ClearInRect},
        {"SetInRect",               SetInRect},
        {"SetInRectArbitrary",      SetInRectArbitrary},
        {"BlendInRect",             BlendInRect},
        {"SetPadBits",              SetPadBits},
        {"SetPadBitsBand",          SetPadBitsBand},
        {"SetOrClearBorder",        SetOrClearBorder},
        {"SetBorderVal",            SetBorderVal},
        {"SetBorderRingVal",        SetBorderRingVal},
        {"SetMirroredBorder",       SetMirroredBorder},
        {"CopyBorder",              CopyBorder},
        {"AddBorder",               AddBorder},
        {"AddBlackOrWhiteBorder",   AddBlackOrWhiteBorder},
        {"AddBorderGeneral",        AddBorderGeneral},
        {"RemoveBorder",            RemoveBorder},
        {"RemoveBorderGeneral",     RemoveBorderGeneral},
        {"RemoveBorderToSize",      RemoveBorderToSize},
        {"AddMirroredBorder",       AddMirroredBorder},
        {"AddRepeatedBorder",       AddRepeatedBorder},
        {"AddMixedBorder",          AddMixedBorder},
        {"AddContinuedBorder",      AddContinuedBorder},
        {"ShiftAndTransferAlpha",   ShiftAndTransferAlpha},
        {"DisplayLayersRGBA",       DisplayLayersRGBA},
        {"CreateRGBImage",          CreateRGBImage},
        {"GetRGBComponent",         GetRGBComponent},
        {"SetRGBComponent",         SetRGBComponent},
        {"CopyRGBComponent",        CopyRGBComponent},
        {"GetRGBLine",              GetRGBLine},
        {"AlphaIsOpaque",           AlphaIsOpaque},
        {"SetMasked",               SetMasked},
        {"SetMaskedGeneral",        SetMaskedGeneral},
        {"CombineMasked",           CombineMasked},
        {"CombineMaskedGeneral",    CombineMaskedGeneral},
        {"PaintThroughMask",        PaintThroughMask},
        {"PaintSelfThroughMask",    PaintSelfThroughMask},
        {"MakeMaskFromVal",         MakeMaskFromVal},
        {"MakeMaskFromLUT",         MakeMaskFromLUT},
        {"MakeArbMaskFromRGB",      MakeArbMaskFromRGB},
        {"SetUnderTransparency",    SetUnderTransparency},
        {"MakeAlphaFromMask",       MakeAlphaFromMask},
        {"GetColorNearMaskBoundary",GetColorNearMaskBoundary},
        {"Invert",                  Invert},
        {"Or",                      Or},
        {"And",                     And},
        {"Xor",                     Xor},
        {"Subtract",                Subtract},
        {"Zero",                    Zero},
        {"ForegroundFraction",      ForegroundFraction},
        {"CountPixels",             CountPixels},
        {"CountPixelsInRect",       CountPixelsInRect},
        {"CountPixelsByRow",        CountPixelsByRow},
        {"CountPixelsByColumn",     CountPixelsByColumn},
        {"CountPixelsInRow",        CountPixelsInRow},
        {"GetMomentByColumn",       GetMomentByColumn},
        {"ThresholdPixelSum",       ThresholdPixelSum},
        {"AverageByRow",            AverageByRow},
        {"AverageByColumn",         AverageByColumn},
        {"AverageInRect",           AverageInRect},
        {"VarianceByRow",           VarianceByRow},
        {"VarianceByColumn",        VarianceByColumn},
        {"VarianceInRect",          VarianceInRect},
        {"AbsDiffByRow",            AbsDiffByRow},
        {"AbsDiffByColumn",         AbsDiffByColumn},
        {"AbsDiffInRect",           AbsDiffInRect},
        {"AbsDiffOnLine",           AbsDiffOnLine},
        {"CountArbInRect",          CountArbInRect},
        {"MirroredTiling",          MirroredTiling},
        {"FindRepCloseTile",        FindRepCloseTile},
        {"GetGrayHistogram",        GetGrayHistogram},
        {"GetGrayHistogramMasked",  GetGrayHistogramMasked},
        {"GetGrayHistogramInRect",  GetGrayHistogramInRect},
        {"GetGrayHistogramTiled",   GetGrayHistogramTiled},
        {"GetColorHistogram",       GetColorHistogram},
        {"GetColorHistogramMasked", GetColorHistogramMasked},
        {"GetCmapHistogram",        GetCmapHistogram},
        {"GetCmapHistogramMasked",  GetCmapHistogramMasked},
        {"GetCmapHistogramInRect",  GetCmapHistogramInRect},
        {"CountRGBColors",          CountRGBColors},
        {"GetColorAmapHistogram",   GetColorAmapHistogram},
        {"GetRankValue",            GetRankValue},
        {"GetRankValueMaskedRGB",   GetRankValueMaskedRGB},
        {"GetRankValueMasked",      GetRankValueMasked},
        {"GetPixelAverage",         GetPixelAverage},
        {"GetPixelStats",           GetPixelStats},
        {"GetAverageMaskedRGB",     GetAverageMaskedRGB},
        {"GetAverageMasked",        GetAverageMasked},
        {"GetAverageTiledRGB",      GetAverageTiledRGB},
        {"GetAverageTiled",         GetAverageTiled},
        {"RowStats",                RowStats},
        {"ColumnStats",             ColumnStats},
        {"GetExtremeValue",         GetExtremeValue},
        {"GetMaxValueInRect",       GetMaxValueInRect},
        {"GetBinnedComponentRange", GetBinnedComponentRange},
        {"GetRankColorArray",       GetRankColorArray},
        {"GetBinnedColor",          GetBinnedColor},
        {"DisplayColorArray",       DisplayColorArray},
        {"RankBinByStrip",          RankBinByStrip},
        {"GetRowStats",             GetRowStats},
        {"GetColumnStats",          GetColumnStats},
        {"SetPixelColumn",          SetPixelColumn},
        {"ThresholdForFgBg",        ThresholdForFgBg},
        {"SplitDistributionFgBg",   SplitDistributionFgBg},
        {"FindAreaPerimRatio",      FindAreaPerimRatio},
        {"FindPerimToAreaRatio",    FindPerimToAreaRatio},
        {"Read",                    Read},
        {"ReadStream",              ReadStream},
        {"ReadMem",                 ReadMem},
        {"Write",                   Write},
        {"WriteStream",             WriteStream},
        {"WriteMem",                WriteMem},
        {"PrintStreamInfo",         PrintStreamInfo},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",                  Create},
        {"CreateNoInit",            CreateNoInit},
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
