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

#include "llept.h"
#include <lauxlib.h>
#include <lualib.h>

/*====================================================================*
 *
 *  Lua class PIX
 *
 *====================================================================*/

/**
 * @brief Check Lua stack at index %arg for udata of class LL_PIX
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIX contained in the user data
 */
PIX *
ll_check_PIX(lua_State *L, int arg)
{
    return *(PIX **)ll_check_udata(L, arg, LL_PIX);
}

/**
 * \brief Push PIX user data to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param pix pointer to the PIX
 * \return 1 PIX* on the Lua stack
 */
int
ll_push_PIX(lua_State *L, PIX *pix)
{
    if (NULL == pix)
        return 0;
    return ll_push_udata(L, LL_PIX, pix);
}

/**
 * \brief Create and push a new PIX*
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be a string (filename)
 * or
 * Arg #1 is expected to be PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
int
ll_new_PIX(lua_State *L)
{
    PIX *pix = NULL;
    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32_default(L, 1, 1);
        l_int32 height = ll_check_l_int32_default(L, 2, 1);
        l_int32 depth = ll_check_l_int32_default(L, 3, 1);
        pix = pixCreate(width, height, depth);
    } else if (lua_isuserdata(L, 1)) {
        PIX *pixs = ll_check_PIX(L, 1);
        pix = pixCreateTemplate(pixs);
    } else {
        const char* filename = lua_tostring(L, 1);
        pix = pixRead(filename);
    }
    return ll_push_PIX(L, pix);
}

static int
toString(lua_State* L)
{
    static char str[256];
    PIX *pix = ll_check_PIX(L, 1);
    luaL_Buffer B;
    char *dst = str;
    const char* format = NULL;
    l_int32 w, h, d, spp, wpl, ccnt, ctot;
    l_uint64 size;
    PIXCMAP *cmap = NULL;

    luaL_buffinit(L, &B);
    if (NULL == pix) {
        luaL_addstring(&B, "nil");
    } else {
        if (pixGetDimensions(pix, &w, &h, &d)) {
            dst += snprintf(dst, sizeof(str), "invalid");
        } else {
            spp = pixGetSpp(pix);
            wpl = pixGetWpl(pix);
            size = (l_uint64)wpl * (l_uint64)h * sizeof(l_uint32);
            format = ll_string_input_format(pixGetInputFormat(pix));
            dst += snprintf(dst, sizeof(str), "[%s] %dx%d %dbpp; %dspp; %dwpl; %llu bytes",
                     format, w, h, d, spp, wpl, size);
        }
        cmap = pixGetColormap(pix);
        if (NULL != cmap) {
            ccnt = pixcmapGetCount(cmap);
            ctot = pixcmapGetFreeCount(cmap) + ccnt;
            dst += snprintf(dst, sizeof(str) - (size_t)(dst - str),
                            "; %d[%d] colors", ccnt, ctot);
        }
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Create a new PIX*
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be a string (filename)
 * or
 * Arg #1 is expected to be PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_PIX(L);
}

/**
 * \brief Create a new PIX* but don't initialize it
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
CreateNoInit(lua_State *L)
{
    PIX *pix = NULL;
    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32(L, 1);
        l_int32 height = ll_check_l_int32(L, 2);
        l_int32 depth = ll_check_l_int32_default(L, 3, 1);
        pix = pixCreateNoInit(width, height, depth);
    } else if (lua_isuserdata(L, 1)) {
        PIX *pixs = ll_check_PIX(L, 1);
        pix = pixCreateTemplateNoInit(pixs);
    } else {
        /* FIXME: what is this? */
    }
    return ll_push_PIX(L, pix);
}

/**
 * \brief Destroy a PIX*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    void **ppix = ll_check_udata(L, 1, LL_PIX);
    DBG(LOG_DESTROY, "%s: '%s' ppix=%p pix=%p refcount=%d\n", __func__,
        LL_PIX, (void *)ppix, *ppix, pixGetRefcount(*(PIX **)ppix));
    pixDestroy((PIX **)ppix);
    *ppix = NULL;
    return 0;
}

/**
 * \brief Copy a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixd = pixCopy(NULL, pixs);
    return ll_push_PIX(L, pixd);
}

/**
 * \brief Clone a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixd = pixClone(pixs);
    return ll_push_PIX(L, pixd);
}

/**
 * \brief Resize a PIX* image data
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ResizeImageData(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixResizeImageData(pixd, pixs));
    return 1;
}

/**
 * \brief Copy the colormap of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyColormap(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopyColormap(pixd, pixs));
    return 1;
}

/**
 * \brief Check if a PIX* is valid
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SizesEqual(lua_State *L)
{
    PIX *pix1 = ll_check_PIX(L, 1);
    PIX *pix2 = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixSizesEqual(pix1, pix2));
    return 1;
}

/**
 * \brief Transfer all data from a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 * Arg #3 is optional and expected to be a boolean (copytext)
 * Arg #4 is optional and expected to be a boolean (copyformat)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
TransferAllData(lua_State *L)
{
    void **ppixs = ll_check_udata(L, 2, LL_PIX);
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    int copytext = lua_isboolean(L, 3) ? lua_toboolean(L, 3) : TRUE;
    int copyformat = lua_isboolean(L, 4) ? lua_toboolean(L, 4) : TRUE;
    lua_pushboolean(L, 0 == pixTransferAllData(pixd, &pixs, copytext, copyformat));
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Swap an destroy a PIX* (pixd) with another PIX* (pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SwapAndDestroy(lua_State *L)
{
    void **ppixd = ll_check_udata(L, 1, LL_PIX);
    void **ppixs = ll_check_udata(L, 2, LL_PIX);
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixSwapAndDestroy(&pixd, &pixs));
    *ppixd = pixd;
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Get the PIX* width
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetWidth(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 width = pixGetWidth(pix);
    lua_pushinteger(L, width);
    return 1;
}

/**
 * \brief Set the PIX* width
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWidth(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 width = ll_check_l_int32_default(L, 2, pixGetWidth(pix));
    lua_pushboolean(L, 0 == pixSetWidth(pix, width));
    return 1;
}

/**
 * \brief Get the PIX* height
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetHeight(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 height = pixGetHeight(pix);
    lua_pushinteger(L, height);
    return 1;
}

/**
 * \brief Set the PIX* height
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetHeight(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 height = ll_check_l_int32_default(L, 2, pixGetHeight(pix));
    lua_pushboolean(L, 0 == pixSetHeight(pix, height));
    return 1;
}

/**
 * \brief Get the PIX* depth
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetDepth(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 depth = pixGetDepth(pix);
    lua_pushinteger(L, depth);
    return 1;
}

/**
 * \brief Set the PIX* depth
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetDepth(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 depth = ll_check_l_int32_default(L, 2, pixGetDepth(pix));
    lua_pushboolean(L, 0 == pixSetDepth(pix, depth));
    return 1;
}

/**
 * \brief Get the PIX* dimensions
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 3 for four integers (or nil on error) on the stack
 */
static int
GetDimensions(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 width, height, depth;
    if (pixGetDimensions(pix, &width, &height, &depth))
        return 0;
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    lua_pushinteger(L, depth);
    return 3;
}

/**
 * \brief Set the PIX* dimensions
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 * Arg #2 is expected to be a lua_Integer (width)
 * Arg #3 is expected to be a lua_Integer (height)
 * Arg #4 is expected to be a lua_Integer (depth)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean result true or false
 */
static int
SetDimensions(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 width = ll_check_l_int32_default(L, 2, 0);
    l_int32 height = ll_check_l_int32_default(L, 3, 0);
    l_int32 depth = ll_check_l_int32_default(L, 4, 1);
    lua_pushboolean(L, 0 == pixSetDimensions(pix, width, height, depth));
    return 1;
}

/**
 * \brief Copy dimensions from a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyDimensions(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopyDimensions(pixd, pixs));
    return 1;
}

/**
 * \brief Get the PIX* SPP (samples per pixel)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetSpp(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 spp = pixGetSpp(pix);
    lua_pushinteger(L, spp);
    return 1;
}

/**
 * \brief Set the PIX* SPP (samples per pixel)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetSpp(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 spp = ll_check_l_int32_default(L, 2, pixGetSpp(pix));
    lua_pushboolean(L, 0 == pixSetSpp(pix, spp));
    return 1;
}

/**
 * \brief Copy SPP (samples per pixel) from a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopySpp(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopySpp(pixd, pixs));
    return 1;
}

/**
 * \brief Get the PIX* WPL (words per line)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetWpl(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 wpl = pixGetWpl(pix);
    lua_pushinteger(L, wpl);
    return 1;
}

/**
 * \brief Set the PIX* WPL (words per line)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWpl(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 wpl = ll_check_l_int32_default(L, 2, pixGetWpl(pix));
    lua_pushboolean(L, 0 == pixSetWpl(pix, wpl));
    return 1;
}

/**
 * \brief Get the PIX* X resolution
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetXRes(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 xres = pixGetXRes(pix);
    lua_pushinteger(L, xres);
    return 1;
}

/**
 * \brief Set the PIX* X resolution
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetXRes(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 xres = ll_check_l_int32_default(L, 2, pixGetXRes(pix));
    lua_pushboolean(L, 0 == pixSetXRes(pix, xres));
    return 1;
}

/**
 * \brief Get the PIX* Y resolution
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetYRes(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 yres = pixGetYRes(pix);
    lua_pushinteger(L, yres);
    return 1;
}

/**
 * \brief Set the PIX* Y resolution
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetYRes(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 yres = ll_check_l_int32_default(L, 2, pixGetYRes(pix));
    lua_pushboolean(L, 0 == pixSetYRes(pix, yres));
    return 1;
}

/**
 * \brief Get the PIX resolution (x, y)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 *
 * \param L pointer to the lua_State
 * \return 2 for two integers (or nil on error) on the stack
 */
static int
GetResolution(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 xres, yres;
    if (pixGetResolution(pix, &xres, &yres))
        return 0;
    lua_pushinteger(L, xres);
    lua_pushinteger(L, yres);
    return 2;
}

/**
 * \brief Set the PIX side resolution (x, y)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data
 * Arg #2 is expected to be a lua_Integer (xres)
 * Arg #3 is expected to be a lua_Integer (yres)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean result true or false
 */
static int
SetResolution(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 xres = ll_check_l_int32_default(L, 2, 300);
    l_int32 yres = ll_check_l_int32_default(L, 3, xres);
    lua_pushboolean(L, 0 == pixSetResolution(pix, xres, yres));
    return 1;
}

/**
 * \brief Copy resolution from a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyResolution(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopyResolution(pixd, pixs));
    return 1;
}

/**
 * \brief Scale resolution of a PIX* by a factor
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (xscale)
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (yscale)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
ScaleResolution(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_float32 xscale = ll_check_l_float32(L, 2);
    l_float32 yscale = ll_check_l_float32(L, 3);
    lua_pushboolean(L, 0 == pixScaleResolution(pix, xscale, yscale));
    return 1;
}

/**
 * \brief Get the input format of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetInputFormat(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    lua_pushstring(L, ll_string_input_format(pixGetInputFormat(pix)));
    return 1;
}

/**
 * \brief Set the input format of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a string with the input format name
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetInputFormat(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 format = ll_check_input_format(L, 2, IFF_UNKNOWN);
    lua_pushboolean(L, 0 == pixSetInputFormat(pix, format));
    return 1;
}

/**
 * \brief Copy input format a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyInputFormat(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopyInputFormat(pixd, pixs));
    return 1;
}

/**
 * \brief Set the special value of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetSpecial(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 special = ll_check_l_int32(L, 2);
    lua_pushboolean(L, 0 == pixSetSpecial(pix, special));
    return 1;
}

/**
 * \brief Get the text of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetText(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    const char* text = pixGetText(pix);
    lua_pushstring(L, text);
    return 1;
}

/**
 * \brief Set the text of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a string
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetText(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    const char* text = lua_tostring(L, 2);
    lua_pushboolean(L, pixSetText(pix, text));
    return 1;
}

/**
 * \brief Add to the text of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a string
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddText(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    const char* text = lua_tostring(L, 2);
    lua_pushboolean(L, pixAddText(pix, text));
    return 1;
}

/**
 * \brief Copy text from a PIX* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be another PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyText(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    lua_pushboolean(L, 0 == pixCopyText(pixd, pixs));
    return 1;
}

/**
 * \brief Get the colormap of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
GetColormap(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    PIXCMAP* colormap = pixGetColormap(pix);
    return ll_push_PIXCMAP(L, colormap);
}

/**
 * \brief Set the colormap of a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a PIXCMAP* user data (colormap)
 *
 * \param L pointer to the lua_State
 * \return 1 PIXCMAP* on the Lua stack
 */
static int
SetColormap(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    PIXCMAP* colormap = ll_take_PIXCMAP(L, 2);
    lua_pushboolean(L, 0 == pixSetColormap(pix, colormap));
    return 1;
}

/**
 * \brief Get a pixel value from PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
GetPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    l_uint32 val = 0;
    if (pixGetPixel(pix, x, y, &val))
        return 0;
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Set a pixel value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 * Arg #4 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    l_uint32 val = ll_check_l_uint32(L, 4) - 1;
    lua_pushboolean(L, 0 == pixSetPixel(pix, x, y, val));
    return 1;
}

/**
 * \brief Get a pixel's RGB values from PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack
 */
static int
GetRGBPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    if (pixGetRGBPixel(pix, x, y, &rval, &gval, &bval))
        return 0;
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    return 3;
}

/**
 * \brief Set a pixel's RGB values in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 * Arg #4 is expected to be a l_int32 (rval)
 * Arg #5 is expected to be a l_int32 (gval)
 * Arg #6 is expected to be a l_int32 (bval)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetRGBPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    l_int32 rval = ll_check_l_int32(L, 4);
    l_int32 gval = ll_check_l_int32(L, 5);
    l_int32 bval = ll_check_l_int32(L, 6);
    lua_pushboolean(L, 0 == pixSetRGBPixel(pix, x, y, rval, gval, bval));
    return 1;
}

/**
 * \brief Get a random pixel's value from PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack (val, x, y)
 */
static int
GetRandomPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = 0;
    l_int32 y = 0;
    l_uint32 val = 0;
    if (pixGetRandomPixel(pix, &val, &x, &y))
        return 0;
    lua_pushinteger(L, val);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 3;
}

/**
 * \brief Clear a pixel value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClearPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    lua_pushboolean(L, 0 == pixClearPixel(pix, x, y));
    return 1;
}

/**
 * \brief Flip a pixel value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlipPixel(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 x = ll_check_l_int32(L, 2);
    l_int32 y = ll_check_l_int32(L, 3);
    lua_pushboolean(L, 0 == pixFlipPixel(pix, x, y));
    return 1;
}

/**
 * \brief Get black or white value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (op = L_GET_BLACK_VAL or L_GET_WHITE_VAL)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackOrWhiteVal(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 op = ll_check_getval(L, 2, L_GET_BLACK_VAL);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, op, &val))
        return 0;
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get black value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackVal(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_BLACK_VAL, &val))
        return 0;
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get white value in PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWhiteVal(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_WHITE_VAL, &val))
        return 0;
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Clear all pixels in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ClearAll(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    lua_pushboolean(L, 0 == pixClearAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAll(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    lua_pushboolean(L, 0 == pixSetAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a PIX* to a gray value %grayval
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_int32 (grayval)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAllGray(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 grayval = ll_check_l_int32(L, 2);
    lua_pushboolean(L, 0 == pixSetAllGray(pix, grayval));
    return 1;
}

/**
 * \brief Set all pixels in a PIX* to an arbitrary value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAllArbitrary(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_uint32 val = ll_check_l_uint32(L, 2);
    lua_pushboolean(L, 0 == pixSetAllArbitrary(pix, val));
    return 1;
}

/**
 * \brief Set all pixels in a PIX* to black or white
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a string describing a setval (op = black, white)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlackOrWhite(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 op = ll_check_blackwhite(L, 2, L_SET_BLACK);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, op));
    return 1;
}

/**
 * \brief Set all pixels in a PIX* to black
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlack(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_BLACK));
    return 1;
}

/**
 * \brief Set all pixels in a PIX* to white
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetWhite(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_WHITE));
    return 1;
}

/**
 * \brief Set all pixel components %comp in a PIX* to a value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a string with the component name (comp)
 * Arg #3 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetComponentArbitrary(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 comp = ll_check_component(L, 2, 0);
    l_int32 val = ll_check_l_int32(L, 3);
    lua_pushboolean(L, 0 == pixSetComponentArbitrary(pix, comp, val));
    return 1;
}

/**
 * \brief Clear all pixels inside a BOX* in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ClearInRect(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    lua_pushboolean(L, 0 == pixClearInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a BOX* in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRect(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    lua_pushboolean(L, 0 == pixSetInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a BOX* in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRectArbitrary(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    lua_pushboolean(L, 0 == pixSetInRectArbitrary(pix, box, val));
    return 1;
}

/**
 * \brief Set all pixels inside a BOX* in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is expected to be a l_uint32 (val)
 * Arg #4 is expected to be a l_float32 (fract)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
BlendInRect(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    l_float32 fract = ll_check_l_float32(L, 4);
    lua_pushboolean(L, 0 == pixBlendInRect(pix, box, val, fract));
    return 1;
}

/**
 * \brief Set pad bits in a PIX* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPadBits(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 val = ll_check_l_int32(L, 2);
    lua_pushboolean(L, 0 == pixSetPadBits(pix, val));
    return 1;
}

/**
 * \brief Set pad bits in a PIX* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (by)
 * Arg #3 is expected to be a l_int32 (bh)
 * Arg #4 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPadBitsBand(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 by = ll_check_l_int32(L, 2);
    l_int32 bh = ll_check_l_int32(L, 3);
    l_int32 val = ll_check_l_int32(L, 4);
    lua_pushboolean(L, 0 == pixSetPadBitsBand(pix, by, bh, val));
    return 1;
}

/**
 * \brief Set or clear border pixels in a PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a operation (op = PIX_SET or PIX_CLR)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetOrClearBorder(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    l_int32 op = ll_check_rasterop(L, 6, PIX_CLR);
    lua_pushboolean(L, 0 == pixSetOrClearBorder(pix, left, right, top, bottom, op));
    return 1;
}

/**
 * \brief Set border pixels in a PIX* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBorderVal(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    l_uint32 val = ll_check_l_uint32(L, 6);
    lua_pushboolean(L, 0 == pixSetBorderVal(pix, left, right, top, bottom, val));
    return 1;
}

/**
 * \brief Set border pixels in a PIX* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (dist)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBorderRingVal(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 dist = ll_check_l_int32(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    lua_pushboolean(L, 0 == pixSetBorderRingVal(pix, dist, val));
    return 1;
}

/**
 * \brief Set mirrored border pixels in a PIX* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMirroredBorder(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    lua_pushboolean(L, 0 == pixSetMirroredBorder(pix, left, right, top, bottom));
    return 1;
}

/**
 * \brief Copy border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
CopyBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixCopyBorder(NULL, pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (npix)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 npix = ll_check_l_int32(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    PIX* pix = pixAddBorder(pixs, npix, val);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add black or white border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a operation (op = L_GET_BLACK_VAL or L_GET_WHITE_VAL)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddBlackOrWhiteBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    l_int32 op = ll_check_getval(L, 6, L_GET_BLACK_VAL);
    PIX* pix = pixAddBlackOrWhiteBorder(pixs, left, right, top, bottom, op);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddBorderGeneral(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    l_uint32 val = ll_check_l_uint32(L, 6);
    PIX* pix = pixAddBorderGeneral(pixs, left, right, top, bottom, val);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels in a PIX* to a new PIX* (simple case %npix)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (npix)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
RemoveBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 npix = ll_check_l_int32(L, 2);
    PIX* pix = pixRemoveBorder(pixs, npix);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels in a PIX* to a new PIX* (general case)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
RemoveBorderGeneral(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixRemoveBorderGeneral(pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels resizing a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (width)
 * Arg #3 is expected to be a l_int32 (height)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
RemoveBorderToSize(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 width = ll_check_l_int32(L, 2);
    l_int32 height = ll_check_l_int32(L, 3);
    PIX* pix = pixRemoveBorderToSize(pixs, width, height);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add mirrored border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddMirroredBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixAddMirroredBorder(pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add repeated border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddRepeatedBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixAddRepeatedBorder(pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add mixed border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddMixedBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixAddMixedBorder(pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Add continued border pixels in a PIX* to a new PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
AddContinuedBorder(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 left = ll_check_l_int32(L, 2);
    l_int32 right = ll_check_l_int32(L, 3);
    l_int32 top = ll_check_l_int32(L, 4);
    l_int32 bottom = ll_check_l_int32(L, 5);
    PIX* pix = pixAddContinuedBorder(pixs, left, right, top, bottom);
    ll_push_PIX(L, pix);
    return 1;
}

/**
 * \brief Shift and transfer alpha channel from a PIX* (pixs) to this PIX* (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ShiftAndTransferAlpha(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    l_int32 shiftx = ll_check_l_int32(L, 3);
    l_int32 shifty = ll_check_l_int32(L, 4);
    lua_pushboolean(L, pixShiftAndTransferAlpha(pixd, pixs, shiftx, shifty));
    return 1;
}

/**
 * \brief Display the layers of a PIX* (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
DisplayLayersRGBA(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_uint32 val = ll_check_l_uint32(L, 2);
    l_int32 maxw = ll_check_l_int32(L, 3);
    ll_push_PIX(L, pixDisplayLayersRGBA(pixs, val, maxw));
    return 1;
}

/**
 * \brief Create a new PIX* from three PIX* layers (%pixr, %pixg, %pixb)
 *
 * Arg #1 is expected to be a PIX* user data (pixr)
 * Arg #2 is expected to be a PIX* user data (pixg)
 * Arg #3 is expected to be a PIX* user data (pixb)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CreateRGBImage(lua_State *L)
{
    PIX *pixr = ll_check_PIX(L, 1);
    PIX *pixg = ll_check_PIX(L, 2);
    PIX *pixb = ll_check_PIX(L, 3);
    ll_push_PIX(L, pixCreateRGBImage(pixr, pixg, pixb));
    return 1;
}

/**
 * \brief Create a new PIX* from one component of PIX*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetRGBComponent(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 comp = ll_check_component(L, 2, L_ALPHA_CHANNEL);
    ll_push_PIX(L, pixGetRGBComponent(pixs, comp));
    return 1;
}

/**
 * \brief Set one component in PIX* (pixd 32bpp) from PIX* (pixs 8bpp)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data (pixs)
 * Arg #3 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetRGBComponent(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    l_int32 comp = ll_check_component(L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixSetRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Copy one component in PIX* (pixd 32bpp) from PIX* (pixs 32bpp)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data (pixs)
 * Arg #3 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyRGBComponent(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    l_int32 comp = ll_check_component(L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixCopyRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Extract red, green and blue components from PIX* (pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetRGBLine(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 row = ll_check_l_int32(L, 2);
    size_t width = (size_t) pixGetWidth(pixs);
    l_uint8 *bufr = (l_uint8 *) calloc(width, sizeof(l_uint8));
    l_uint8 *bufg = (l_uint8 *) calloc(width, sizeof(l_uint8));
    l_uint8 *bufb = (l_uint8 *) calloc(width, sizeof(l_uint8));
    if (NULL == bufr || NULL == bufg || NULL == bufb) {
        free(bufr);
        free(bufg);
        free(bufb);
        lua_pushliteral(L, "Could not alloc all of bufr, bufg, and bufb");
        lua_error(L);
        return 0;
    }
    if (pixGetRGBLine(pixs, row, bufr, bufg, bufb)) {
        free(bufr);
        free(bufg);
        free(bufb);
        return 0;
    }
    lua_pushlstring(L, (const char *)bufr, width);
    lua_pushlstring(L, (const char *)bufg, width);
    lua_pushlstring(L, (const char *)bufb, width);
    free(bufr);
    free(bufg);
    free(bufb);
    return 3;
}

/**
 * \brief Check alpha layer of a PIX* for opaqueness
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AlphaIsOpaque(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    l_int32 opaque = 0;
    if (pixAlphaIsOpaque(pix, &opaque))
        return 0;
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Set the mask for a PIX* (%pixd) from another PIX* (%pixm 1bpp)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMasked(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    lua_pushboolean(L, 0 == pixSetMasked(pixd, pixm, val));
    return 1;
}

/**
 * \brief Set the mask for a PIX* (%pixd) from another PIX* (%pixm 1bpp) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 * Arg #3 is expected to be a l_uint32 (val)
 * Arg #4 is expected to be a l_int32 (x)
 * Arg #5 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMaskedGeneral(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_uint32 val = ll_check_l_uint32(L, 3);
    l_int32 x = ll_check_l_int32(L, 4);
    l_int32 y = ll_check_l_int32(L, 5);
    lua_pushboolean(L, 0 == pixSetMaskedGeneral(pixd, pixm, val, x, y));
    return 1;
}

/**
 * \brief Set the mask for a PIX* (%pixd) from another PIX* (%pixm 1bpp)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data (pixs)
 * Arg #3 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMasked(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    PIX *pixm = ll_check_PIX(L, 3);
    lua_pushboolean(L, 0 == pixCombineMasked(pixd, pixs, pixm));
    return 1;
}

/**
 * \brief Set the mask for a PIX* (%pixd) from another PIX* (%pixm 1bpp) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data (pixs)
 * Arg #3 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 * Arg #4 is expected to be a l_int32 (x)
 * Arg #5 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMaskedGeneral(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixs = ll_check_PIX(L, 2);
    PIX *pixm = ll_check_PIX(L, 3);
    l_int32 x = ll_check_l_int32(L, 4);
    l_int32 y = ll_check_l_int32(L, 5);
    lua_pushboolean(L, 0 == pixCombineMaskedGeneral(pixd, pixs, pixm, x, y));
    return 1;
}

/**
 * \brief Paint %val through a mask PIX* (%pixm) onto a PIX* (%pixd) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
PaintThroughMask(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_uint32 val = ll_check_l_uint32(L, 5);
    lua_pushboolean(L, 0 == pixPaintThroughMask(pixd, pixm, x, y, val));
    return 1;
}

/**
 * \brief Paint %val through a mask PIX* (%pixm) onto a PIX* (%pixd) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixd)
 * Arg #2 is expected to be a PIX* user data with 1 bit/pixel (pixm)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a string with a search direction name (searchdir)
 * Arg #6 is expected to be a l_int32 (mindist)
 * Arg #7 is expected to be a l_int32 (tilesize)
 * Arg #8 is expected to be a l_int32 (ntiles)
 * Arg #9 is optional and, if given, expected to be a l_int32 (distblend)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
PaintSelfThroughMask(lua_State *L)
{
    PIX *pixd = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 searchdir = ll_check_searchdir(L, 5, L_BOTH_DIRECTIONS);
    l_int32 mindist = ll_check_l_int32(L, 6);
    l_int32 tilesize = ll_check_l_int32(L, 7);
    l_int32 ntiles = ll_check_l_int32(L, 8);
    l_int32 distblend = ll_check_l_int32_default(L, 9, 0);
    lua_pushboolean(L, 0 == pixPaintSelfThroughMask(pixd, pixm, x, y, searchdir, mindist, tilesize, ntiles, distblend));
    return 1;
}

/**
 * \brief Create a new PIX* (%pixd) from a source PIX* (%pixs) using a mask value (%val)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromVal(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 val = ll_check_l_int32(L, 2);
    ll_push_PIX(L, pixMakeMaskFromVal(pixs, val));
    return 1;
}

/**
 * \brief Create a new PIX* (%pixd) from a source PIX* (%pixs) using a 2^depth entry lookup-table (%lut)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string of max. 256 bytes (lut)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromLUT(lua_State *L)
{
    size_t len = 0, i;
    PIX *pixs = ll_check_PIX(L, 1);
    const char* lut = lua_tolstring(L, 2, &len);
    l_int32* tab = NULL;
    tab = (l_int32 *) calloc(256, sizeof(l_int32));
    if (NULL == tab) {
        lua_pushliteral(L, "Could not alloc table");
        lua_error(L);
        return 0;
    }
    /* expand lookup-table (lut) to array of l_int32 (tab) */
    for (i = 0; i < 256 && i < len; i++)
        tab[i] = lut[i];
    ll_push_PIX(L, pixMakeMaskFromLUT(pixs, tab));
    free(tab);
    return 1;
}

/**
 * \brief Create a new PIX* (%pixd) from a source PIX* (%pixs) using arithmetic factors (%rc, %gc, %bc)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_float32 (rc)
 * Arg #3 is expected to be a l_float32 (gc)
 * Arg #4 is expected to be a l_float32 (bc)
 * Arg #5 is expected to be a l_float32 (thresh)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeArbMaskFromRGB(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_float32 rc = ll_check_l_float32(L, 2);
    l_float32 gc = ll_check_l_float32(L, 3);
    l_float32 bc = ll_check_l_float32(L, 4);
    l_float32 thresh = ll_check_l_float32(L, 5);
    ll_push_PIX(L, pixMakeArbMaskFromRGB(pixs, rc, gc, bc, thresh));
    return 1;
}

/**
 * \brief Create a new PIX* (%pixd) from PIX* (%pixs) seting alpha == 0 pixels to a value (%val)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
SetUnderTransparency(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_uint32 val = ll_check_l_uint32(L, 2);
    ll_push_PIX(L, pixSetUnderTransparency(pixs, val, 0));
    return 1;
}

/**
 * \brief Create a new alpha mask PIX* (%pixd) from PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (dist)
 * Arg #3 is optional and, if given, expected to be a boolean (getbox)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack or 2 PIX* and BOX* on the Lua stack
 */
static int
MakeAlphaFromMask(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 dist = ll_check_l_int32(L, 2);
    int getbox = lua_isboolean(L, 3) ?  lua_toboolean(L, 3) : FALSE;
    BOX* box = NULL;
    PIX* pixd = pixMakeAlphaFromMask(pixs, dist, getbox ? &box : NULL);
    ll_push_PIX(L, pixd);
    if (NULL != pixd && NULL != box) {
        ll_push_BOX(L, box);
        return 2;
    }
    return 1;
}

/**
 * \brief Get the color near the mask boundary from PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm)
 * Arg #3 is expected to be a BOX* user data (box)
 * Arg #4 is expected to be a l_int32 (dist)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorNearMaskBoundary(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    BOX *box = ll_check_BOX(L, 3);
    l_uint32 val = 0;
    l_int32 dist = ll_check_l_int32(L, 4);
    if (pixGetColorNearMaskBoundary(pixs, pixm, box, dist, &val, 0))
        return 0;
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Invert the PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Invert(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixd = pixInvert(NULL, pixs);
    ll_push_PIX(L, pixd);
    return 1;
}

/**
 * \brief And the PIX* (%pix1) and PIX* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix1)
 * Arg #2 is expected to be another PIX* user data (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
And(lua_State *L)
{
    PIX *pix1 = ll_check_PIX(L, 1);
    PIX *pix2 = ll_check_PIX(L, 2);
    PIX *pixd = pixAnd(NULL, pix1, pix2);
    ll_push_PIX(L, pixd);
    return 1;
}

/**
 * \brief Or the PIX* (%pix1) and PIX* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix1)
 * Arg #2 is expected to be another PIX* user data (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Or(lua_State *L)
{
    PIX *pix1 = ll_check_PIX(L, 1);
    PIX *pix2 = ll_check_PIX(L, 2);
    PIX *pixd = pixOr(NULL, pix1, pix2);
    ll_push_PIX(L, pixd);
    return 1;
}

/**
 * \brief Xor the PIX* (%pix1) and PIX* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix1)
 * Arg #2 is expected to be another PIX* user data (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Xor(lua_State *L)
{
    PIX *pix1 = ll_check_PIX(L, 1);
    PIX *pix2 = ll_check_PIX(L, 2);
    PIX *pixd = pixXor(NULL, pix1, pix2);
    ll_push_PIX(L, pixd);
    return 1;
}

/**
 * \brief Subtract the PIX* (%pix2) from PIX* (%pix1)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix1)
 * Arg #2 is expected to be another PIX* user data (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Subtract(lua_State *L)
{
    PIX *pix1 = ll_check_PIX(L, 1);
    PIX *pix2 = ll_check_PIX(L, 2);
    PIX *pixd = pixSubtract(NULL, pix1, pix2);
    ll_push_PIX(L, pixd);
    return 1;
}

/**
 * \brief Check if all pixels in PIX* (%pixs) are 0
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Zero(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 empty = 0;
    if (pixZero(pixs, &empty))
        return 0;
    lua_pushboolean(L, empty);
    return 1;
}

/**
 * \brief Calculate the fraction of foreground in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
ForegroundFraction(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_float32 fract = 0;
    if (pixForegroundFraction(pixs, &fract))
        return 0;
    lua_pushnumber(L, (lua_Number) fract);
    return 1;
}

/**
 * \brief Count the number of foreground pixels in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixels(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 count = 0;
    if (pixCountPixels(pixs, &count, NULL))
        return 0;
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of foreground pixels in PIX* (%pixs) in a BOX* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixelsInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRect(pixs, box, &count, NULL))
        return 0;
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of pixels by row in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
CountPixelsByRow(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    NUMA *na = pixCountPixelsByRow(pixs, NULL);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Count the number of pixels by column in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
CountPixelsByColumn(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    NUMA *na = pixCountPixelsByColumn(pixs);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Count the number of pixels in row %row of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
CountPixelsInRow(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 row = ll_check_l_int32(L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRow(pixs, row, &count, NULL))
        return 0;
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the moment of order %order by column in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (order; 1 or 2)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetMomentByColumn(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 order = ll_check_l_int32(L, 2);
    NUMA *na = pixGetMomentByColumn(pixs, order);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Check if the pixel sum of PIX* (%pixs) is above threshold %thresh
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (thresh)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ThresholdPixelSum(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 thresh = ll_check_l_int32(L, 2);
    l_int32 above = 0;
    if (pixThresholdPixelSum(pixs, thresh, &above, NULL))
        return 0;
    lua_pushboolean(L, above);
    return 1;
}

/**
 * \brief Build the average by row of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 * Arg #3 is optional and, if specified, expected to be a string (type: white-is-max or black-is-max)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AverageByRow(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_int32 type = ll_check_what_is_max(L, 3, L_WHITE_IS_MAX);
    NUMA* na = pixAverageByRow(pixs, box, type);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the average by column of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 * Arg #3 is optional and, if specified, expected to be a string (type: white-is-max or black-is-max)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AverageByColumn(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_int32 type = ll_check_what_is_max(L, 3, L_WHITE_IS_MAX);
    NUMA* na = pixAverageByColumn(pixs, box, type);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the average inside a BOX* (%box) of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AverageInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_float32 ave = 0.0f;
    if (pixAverageInRect(pixs, box, &ave))
        return 0;
    lua_pushnumber(L, (lua_Number) ave);
    return 1;
}

/**
 * \brief Build the variance by row of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
VarianceByRow(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA* na = pixVarianceByRow(pixs, box);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the variance by column of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
VarianceByColumn(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA* na = pixVarianceByColumn(pixs, box);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the square root of the variance inside a BOX* (%box) of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
VarianceInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_float32 sqrvar = 0.0f;
    if (pixVarianceInRect(pixs, box, &sqrvar))
        return 0;
    lua_pushnumber(L, (lua_Number) sqrvar);
    return 1;
}

/**
 * \brief Build the absolute difference by row of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AbsDiffByRow(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA* na = pixAbsDiffByRow(pixs, box);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the absolute difference by column of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AbsDiffByColumn(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA* na = pixAbsDiffByColumn(pixs, box);
    return ll_push_NUMA(L, na);
}

/**
 * \brief Build the absolute difference inside a BOX* (%box) of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 * Arg #2 is optional and, if specified, expected to be a string (dir: horizontal-line or vertical-line)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AbsDiffInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_int32 dir = ll_check_direction(L, 3, L_HORIZONTAL_LINE);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffInRect(pixs, box, dir, &absdiff))
        return 0;
    lua_pushnumber(L, (lua_Number) absdiff);
    return 1;
}

/**
 * \brief Build absolute difference on a line (%x1,%y1 to %x2,%y2) of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
AbsDiffOnLine(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 x1 = ll_check_l_int32(L, 2);
    l_int32 y1 = ll_check_l_int32(L, 3);
    l_int32 x2 = ll_check_l_int32(L, 4);
    l_int32 y2 = ll_check_l_int32(L, 5);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffOnLine(pixs, x1, y1, x2, y2, &absdiff))
        return 0;
    lua_pushnumber(L, (lua_Number) absdiff);
    return 1;
}

/**
 * \brief Count pixels of arbitrary value %val in BOX* (%box) of PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (val)
 * Arg #3 is expected to be a l_int32 (factor)
 * Arg #4 is optional and, if specified, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
CountArbInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 val = ll_check_l_int32(L, 2);
    l_int32 factor = ll_check_l_int32(L, 3);
    BOX *box = lua_isuserdata(L, 4) ? ll_check_BOX(L, 4) : NULL;
    l_int32 count;
    if (pixCountArbInRect(pixs, box, val, factor, &count))
        return 0;
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Create a mirrored tiling of the PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (w)
 * Arg #3 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
MirroredTiling(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 w = ll_check_l_int32(L, 2);
    l_int32 h = ll_check_l_int32(L, 3);
    return ll_push_PIX(L, pixMirroredTiling(pixs, w, h));
}

/**
 * \brief Look for one or two square tiles with conforming median
 *        intensity and low variance outside but near the input %box.
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is expected to be a string describing the search direction (searchdir)
 * Arg #4 is expected to be a l_int32 (mindist)
 * Arg #5 is expected to be a l_int32 (tsize)
 * Arg #6 is expected to be a l_int32 (ntiles)
 *
 * \param L pointer to the lua_State
 * \return 1 BOX* on the Lua stack
 */
static int
FindRepCloseTile(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 searchdir = ll_check_direction(L, 3, L_HORIZ);
    l_int32 mindist = ll_check_l_int32(L, 4);
    l_int32 tsize = ll_check_l_int32(L, 5);
    l_int32 ntiles = ll_check_l_int32(L, 6);
    BOX *boxtile = NULL;
    if (pixFindRepCloseTile(pixs, box, searchdir, mindist, tsize, ntiles, &boxtile, 0))
        return 0;
    return ll_push_BOX(L, boxtile);
}

/**
 * \brief Get the histogram of the grayscale PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetGrayHistogram(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    return ll_push_NUMA(L, pixGetGrayHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the grayscale PIX* (%pixs) masked with another PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetGrayHistogramMasked(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 factor = ll_check_l_int32(L, 5);
    return ll_push_NUMA(L, pixGetGrayHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the grayscale PIX* (%pixs) inside BOX* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetGrayHistogramInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 factor = ll_check_l_int32(L, 3);
    return ll_push_NUMA(L, pixGetGrayHistogramInRect(pixs, box, factor));
}

/**
 * \brief Get an array of histograms of the grayscale PIX* (%pixs) for %nx by %ny tiles
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_int32 (nx)
 * Arg #4 is expected to be a l_int32 (ny)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMAA* on the Lua stack
 */
static int
GetGrayHistogramTiled(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    l_int32 nx = ll_check_l_int32(L, 3);
    l_int32 ny = ll_check_l_int32(L, 4);
    return ll_push_NUMAA(L, pixGetGrayHistogramTiled(pixs, factor, nx, ny));
}

/**
 * \brief Get the RGB histograms of the PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 NUMA* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogram(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    NUMA *nar = NULL;
    NUMA *nag = NULL;
    NUMA *nab = NULL;
    if (pixGetColorHistogram(pixs, factor, &nar, &nag, &nab))
        return 0;
    return ll_push_NUMA(L, nar) + ll_push_NUMA(L, nag) + ll_push_NUMA(L, nab);
}

/**
 * \brief Get the RGB histograms of the PIX* (%pixs) masked with another PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 NUMA* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogramMasked(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 factor = ll_check_l_int32(L, 5);
    NUMA *nar = NULL;
    NUMA *nag = NULL;
    NUMA *nab = NULL;
    if (pixGetColorHistogramMasked(pixs, pixm, x, y, factor, &nar, &nag, &nab))
        return 0;
    return ll_push_NUMA(L, nar) + ll_push_NUMA(L, nag) + ll_push_NUMA(L, nab);
}

/**
 * \brief Get the histogram of the color mapped PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetCmapHistogram(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    return ll_push_NUMA(L, pixGetCmapHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the color mapped PIX* (%pixs) masked with another PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetCmapHistogramMasked(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 factor = ll_check_l_int32(L, 5);
    return ll_push_NUMA(L, pixGetCmapHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the color mapped PIX* (%pixs) inside BOX* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a BOX* user data (box)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 NUMA* on the Lua stack
 */
static int
GetCmapHistogramInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = ll_check_BOX(L, 2);
    l_int32 factor = ll_check_l_int32(L, 3);
    return ll_push_NUMA(L, pixGetCmapHistogramInRect(pixs, box, factor));
}

/**
 * \brief Count the RGB colors in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountRGBColors(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 count = pixCountRGBColors(pixs);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get a histogram AMAP* for the colors in PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorAmapHistogram(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    return ll_push_AMAP(L, pixGetColorAmapHistogram(pixs, factor));
}

/**
 * \brief Get rank value for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_float32 (rank)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetRankValue(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32(L, 2);
    l_float32 rank = ll_check_l_float32(L, 3);
    l_uint32 value = 0;
    if (pixGetRankValue(pixs, factor, rank, &value))
        return 0;
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get rank values for RGB for PIX* (%pixs) masked with another PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 * Arg #6 is expected to be a l_float32 (rank)
 *
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetRankValueMaskedRGB(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 factor = ll_check_l_int32(L, 5);
    l_float32 rank = ll_check_l_int32(L, 6);
    l_float32 rval = 0.0f, gval = 0.0f, bval = 0.0f;
    if (pixGetRankValueMaskedRGB(pixs, pixm, x, y, factor, rank, &rval, &gval, &bval))
        return 0;
    lua_pushnumber(L, (lua_Number)rval);
    lua_pushnumber(L, (lua_Number)gval);
    lua_pushnumber(L, (lua_Number)bval);
    return 3;
}

/**
 * \brief Get rank value for PIX* (%pixs) masked with another PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixm)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 * Arg #6 is expected to be a l_float32 (rank)
 *
 * \param L pointer to the lua_State
 * \return 1 number (value) and a NUMA* (histogram) on the Lua stack
 */
static int
GetRankValueMasked(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = ll_check_PIX(L, 2);
    l_int32 x = ll_check_l_int32(L, 3);
    l_int32 y = ll_check_l_int32(L, 4);
    l_int32 factor = ll_check_l_int32(L, 5);
    l_float32 rank = ll_check_l_int32(L, 6);
    l_float32 value = 0.0f;
    NUMA *na = NULL;
    if (pixGetRankValueMasked(pixs, pixm, x, y, factor, rank, &value, &na))
        return 0;
    lua_pushnumber(L, (lua_Number)value);
    ll_push_NUMA(L, na);
    return 1;
}

/**
 * \brief Get the pixel average for PIX* (%pixs) optionally masked with PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if given, expected to be a PIX* user data (pixm)
 * Arg #3 is optional and, if given, expected to be a l_int32 (x)
 * Arg #4 is optional and, if given, expected to be a l_int32 (y)
 * Arg #5 is optional and, if given, expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPixelAverage(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixm = lua_isuserdata(L, 2) ? ll_check_PIX(L, 2) : NULL;
    l_int32 x = ll_check_l_int32_default(L, 3, 0);
    l_int32 y = ll_check_l_int32_default(L, 4, 0);
    l_int32 factor = ll_check_l_int32_default(L, 5, 1);
    l_uint32 value = 0;
    if (pixGetPixelAverage(pixs, pixm, x,y, factor, &value))
        return 0;
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel stats for PIX* (%pixs) optionally masked with PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPixelStats(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    l_int32 factor = ll_check_l_int32_default(L, 3, 1);
    l_uint32 value = 0;
    if (pixGetPixelStats(pixs, factor, type, &value))
        return 0;
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel average RGB values for PIX* (%pixs) optionally masked with PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is optional and, if given, expected to be a PIX* user data (pixm)
 * Arg #4 is optional and, if given, expected to be a l_int32 (x)
 * Arg #5 is optional and, if given, expected to be a l_int32 (y)
 * Arg #6 is optional and, if given, expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageMaskedRGB(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    PIX *pixm = lua_isuserdata(L, 3) ? ll_check_PIX(L, 3) : NULL;
    l_int32 x = ll_check_l_int32_default(L, 4, 0);
    l_int32 y = ll_check_l_int32_default(L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(L, 6, 1);
    l_float32 rval = 0.0, gval = 0.0, bval = 0.0;
    if (pixGetAverageMaskedRGB(pixs, pixm, x, y, factor, type, &rval, &gval, &bval))
        return 0;
    lua_pushnumber(L, (lua_Number)rval);
    lua_pushnumber(L, (lua_Number)gval);
    lua_pushnumber(L, (lua_Number)bval);
    return 3;
}

/**
 * \brief Get the pixel average value for PIX* (%pixs) optionally masked with PIX* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is optional and, if given, expected to be a PIX* user data (pixm)
 * Arg #4 is optional and, if given, expected to be a l_int32 (x)
 * Arg #5 is optional and, if given, expected to be a l_int32 (y)
 * Arg #6 is optional and, if given, expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageMasked(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    PIX *pixm = lua_isuserdata(L, 3) ? ll_check_PIX(L, 3) : NULL;
    l_int32 x = ll_check_l_int32_default(L, 4, 0);
    l_int32 y = ll_check_l_int32_default(L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(L, 6, 1);
    l_float32 value = 0.0;
    if (pixGetAverageMasked(pixs, pixm, x, y, factor, type, &value))
        return 0;
    lua_pushnumber(L, (lua_Number)value);
    return 3;
}

/**
 * \brief Get the average RGB values for PIX* (%pixs) as three PIX* (%pixr, %pixg, %pixb)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is expected to be a l_int32 (sx)
 * Arg #4 is expected to be a l_int32 (sy)
 *
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageTiledRGB(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(L, 4, 2);
    PIX *pixr = NULL;
    PIX *pixg = NULL;
    PIX *pixb = NULL;
    if (pixGetAverageTiledRGB(pixs, sx, sy, type, &pixr, &pixg, &pixb))
        return 0;
    return ll_push_PIX(L, pixr) + ll_push_PIX(L, pixg) + ll_push_PIX(L, pixb);
}

/**
 * \brief Get the average value for PIX* (%pixs) as PIX* (%pixv)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #4 is expected to be a l_int32 (sx)
 * Arg #5 is expected to be a l_int32 (sy)
 *
 * \param L pointer to the lua_State
 * \return 3 numbers on the Lua stack (rval, gval, bval)
 */
static int
GetAverageTiled(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 type = ll_check_stats_type(L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(L, 4, 2);
    PIX *pixv = pixGetAverageTiled(pixs, sx, sy, type);
    return ll_push_PIX(L, pixv);
}

/**
 * \brief Get the row stats for PIX* (%pixs) as six NUMA*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if given, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 6 NUMA* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
RowStats(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA *mean = NULL;
    NUMA *median = NULL;
    NUMA *mode = NULL;
    NUMA *modecount = NULL;
    NUMA *var = NULL;
    NUMA *rootvar = NULL;
    if (pixRowStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return 0;
    return ll_push_NUMA(L, mean) +
            ll_push_NUMA(L, median) +
            ll_push_NUMA(L, mode) +
            ll_push_NUMA(L, modecount) +
            ll_push_NUMA(L, var) +
            ll_push_NUMA(L, rootvar);
}

/**
 * \brief Get the column stats for PIX* (%pixs) as six NUMA*
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional and, if given, expected to be a BOX* user data (box)
 *
 * \param L pointer to the lua_State
 * \return 6 NUMA* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
ColumnStats(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    NUMA *mean = NULL;
    NUMA *median = NULL;
    NUMA *mode = NULL;
    NUMA *modecount = NULL;
    NUMA *var = NULL;
    NUMA *rootvar = NULL;
    if (pixColumnStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return 0;
    return ll_push_NUMA(L, mean) +
            ll_push_NUMA(L, median) +
            ll_push_NUMA(L, mode) +
            ll_push_NUMA(L, modecount) +
            ll_push_NUMA(L, var) +
            ll_push_NUMA(L, rootvar);
}

/**
 * \brief Get the range values for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_int32 (color)
 *
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack (minval, maxval)
 */
static int
GetRangeValues(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32_default(L, 2, 1);
    l_int32 color = ll_check_select_color(L, 3, L_SELECT_RED);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    if (pixGetRangeValues(pixs, factor, color, &minval, &maxval))
        return 0;
    lua_pushinteger(L, minval);
    lua_pushinteger(L, maxval);
    return 2;
}

/**
 * \brief Get an extreme value for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a string describing the type (type; min or max)
 *
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (rval, gval, bval, grayval)
 */
static int
GetExtremeValue(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 factor = ll_check_l_int32_default(L, 2, 1);
    l_int32 type = ll_check_select_min_max(L, 3, L_SELECT_MIN);
    l_int32 rval = 0;
    l_int32 gval = 0;
    l_int32 bval = 0;
    l_int32 grayval = 0;
    if (pixGetExtremeValue(pixs, factor, type, &rval, &gval, &bval, &grayval))
        return 0;
    lua_pushinteger(L, rval);
    lua_pushinteger(L, gval);
    lua_pushinteger(L, bval);
    lua_pushinteger(L, grayval);
    return 4;
}

/**
 * \brief Get the maximum value for PIX* (%pixs) optionally in rect BOX* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is optional an, if given, expected to be a BOX* user data
 *
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (maxval, xmax, ymax)
 */
static int
GetMaxValueInRect(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    BOX *box = lua_isuserdata(L, 2) ? ll_check_BOX(L, 2) : NULL;
    l_uint32 maxval = 0;
    l_int32 xmax = 0;
    l_int32 ymax = 0;
    if (pixGetMaxValueInRect(pixs, box, &maxval, &xmax, &ymax))
        return 0;
    lua_pushinteger(L, maxval);
    lua_pushinteger(L, xmax);
    lua_pushinteger(L, ymax);
    return 3;
}

/**
 * \brief Get a binned component range for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (nbins)
 * Arg #3 is expected to be a l_int32 (factor)
 * Arg #4 is expected to be a string defining the selected color (color)
 * Arg #5 is optional and, if given, expected to be a l_int32 (fontsize)
 *
 * \param L pointer to the lua_State
 * \return 2+nbins integers on the Lua stack (minval, maxval, carray[])
 */
static int
GetBinnedComponentRange(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 nbins = ll_check_l_int32_default(L, 2, 2);
    l_int32 factor = ll_check_l_int32_default(L, 3, 1);
    l_int32 color = ll_check_select_color(L, 4, L_SELECT_RED);
    l_int32 fontsize = ll_check_l_int32_default(L, 5, 0);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    l_uint32 *carray = NULL;
    l_int32 i;

    if (pixGetBinnedComponentRange(pixs, nbins, factor, color, &minval, &maxval, &carray, fontsize))
        return 0;
    lua_pushinteger(L, minval);
    lua_pushinteger(L, maxval);
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    free(carray);
    return 2 + nbins;
}

/**
 * \brief Get a rank color array for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a l_int32 (nbins)
 * Arg #3 is expected to be a l_int32 (factor)
 * Arg #4 is expected to be a string defining the selected color (color)
 *
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
GetRankColorArray(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    l_int32 nbins = ll_check_l_int32_default(L, 2, 1);
    l_int32 factor = ll_check_l_int32_default(L, 3, 1);
    l_int32 type = ll_check_select_color(L, 4, L_SELECT_AVERAGE);
    l_uint32 *carray = NULL;
    l_int32 i;

    if (pixGetRankColorArray(pixs, nbins, type, factor, &carray, 0, 0))
        return 0;
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    free(carray);
    return nbins;
}

/**
 * \brief Get a binned color for PIX* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pixs)
 * Arg #2 is expected to be a PIX* user data (pixg)
 * Arg #3 is expected to be a l_int32 (nbins)
 * Arg #4 is expected to be a l_int32 (factor)
 * Arg #5 is expected to be a NUMA* user data (alut)
 *
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
GetBinnedColor(lua_State *L)
{
    PIX *pixs = ll_check_PIX(L, 1);
    PIX *pixg = ll_check_PIX(L, 2);
    l_int32 nbins = ll_check_l_int32_default(L, 3, 1);
    l_int32 factor = ll_check_l_int32_default(L, 4, 1);
    NUMA *alut = ll_check_NUMA(L, 5);
    l_uint32 *carray = NULL;
    l_int32 i;

    if (pixGetBinnedColor(pixs, pixg, factor, nbins, alut, &carray, 0))
        return 0;
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    return nbins;
}


/*
*    Pixel histogram, rank val, averaging and min/max
*           l_int32     pixGetAverageTiledRGB()
*           PIX        *pixGetAverageTiled()
*           NUMA       *pixRowStats()
*           NUMA       *pixColumnStats()
*           l_int32     pixGetRangeValues()
*           l_int32     pixGetExtremeValue()
*           l_int32     pixGetMaxValueInRect()
*           l_int32     pixGetBinnedComponentRange()
*           l_int32     pixGetRankColorArray()
*           l_int32     pixGetBinnedColor()
*           PIX        *pixDisplayColorArray()
*           PIX        *pixRankBinByStrip()
*
*    Pixelwise aligned statistics
*           PIX        *pixaGetAlignedStats()
*           l_int32     pixaExtractColumnFromEachPix()
*           l_int32     pixGetRowStats()
*           l_int32     pixGetColumnStats()
*           l_int32     pixSetPixelColumn()
*
*    Foreground/background estimation
*           l_int32     pixThresholdForFgBg()
*           l_int32     pixSplitDistributionFgBg()
*/

/**
 * \brief Read new PIX*
 *
 * Arg #1 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 PIX* on the Lua stack
 */
static int
Read(lua_State *L)
{
    const char* filename = lua_tostring(L, 1);
    PIX *pix = pixRead(filename);
    return ll_push_PIX(L, pix);
}

/**
 * \brief Write the PIX* to a file
 *
 * Arg #1 (i.e. self) is expected to be a PIX* user data (pix)
 * Arg #2 is expected to be a string (filename)
 * Arg #3 is expected to be a string with the input format name (format)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    PIX *pix = ll_check_PIX(L, 1);
    const char *filename = lua_tostring(L, 2);
    l_int32 format = ll_check_input_format(L, 3, IFF_DEFAULT);
    lua_pushboolean(L, 0 == pixWrite(filename, pix, format));
    return 1;
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PIX(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},           /* garbage collector */
        {"__new",                   Create},            /* new PIX */
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
        {"Write",                   Write},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",                  Create},
        {"CreateNoInit",            CreateNoInit},
        {"Read",                    Read},
        LUA_SENTINEL
    };

    int res = ll_register_class(L, LL_PIX, methods, functions);
    lua_setglobal(L, LL_PIX);
    return res;
}
