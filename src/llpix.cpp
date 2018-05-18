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

#undef  _
#define _(x) (((x)>>7)&1)+(((x)>>6)&1)+(((x)>>5)&1)+(((x)>>4)&1)+(((x)>>3)&1)+(((x)>>2)&1)+(((x)>>0)&1)
/** Table of bit counts in a byte */
static l_int32 tab8[256] = {
    _(0x00),_(0x01),_(0x02),_(0x03),_(0x04),_(0x05),_(0x06),_(0x07),_(0x08),_(0x09),_(0x0a),_(0x0b),_(0x0c),_(0x0d),_(0x0e),_(0x0f),
    _(0x10),_(0x11),_(0x12),_(0x13),_(0x14),_(0x15),_(0x16),_(0x17),_(0x18),_(0x19),_(0x1a),_(0x1b),_(0x1c),_(0x1d),_(0x1e),_(0x1f),
    _(0x20),_(0x21),_(0x22),_(0x23),_(0x24),_(0x25),_(0x26),_(0x27),_(0x28),_(0x29),_(0x2a),_(0x2b),_(0x2c),_(0x2d),_(0x2e),_(0x2f),
    _(0x30),_(0x31),_(0x32),_(0x33),_(0x34),_(0x35),_(0x36),_(0x37),_(0x38),_(0x39),_(0x3a),_(0x3b),_(0x3c),_(0x3d),_(0x3e),_(0x3f),
    _(0x40),_(0x41),_(0x42),_(0x43),_(0x44),_(0x45),_(0x46),_(0x47),_(0x48),_(0x49),_(0x4a),_(0x4b),_(0x4c),_(0x4d),_(0x4e),_(0x4f),
    _(0x50),_(0x51),_(0x52),_(0x53),_(0x54),_(0x55),_(0x56),_(0x57),_(0x58),_(0x59),_(0x5a),_(0x5b),_(0x5c),_(0x5d),_(0x5e),_(0x5f),
    _(0x60),_(0x61),_(0x62),_(0x63),_(0x64),_(0x65),_(0x66),_(0x67),_(0x68),_(0x69),_(0x6a),_(0x6b),_(0x6c),_(0x6d),_(0x6e),_(0x6f),
    _(0x70),_(0x71),_(0x72),_(0x73),_(0x74),_(0x75),_(0x76),_(0x77),_(0x78),_(0x79),_(0x7a),_(0x7b),_(0x7c),_(0x7d),_(0x7e),_(0x7f),
    _(0x80),_(0x81),_(0x82),_(0x83),_(0x84),_(0x85),_(0x86),_(0x87),_(0x88),_(0x89),_(0x8a),_(0x8b),_(0x8c),_(0x8d),_(0x8e),_(0x8f),
    _(0x90),_(0x91),_(0x92),_(0x93),_(0x94),_(0x95),_(0x96),_(0x97),_(0x98),_(0x99),_(0x9a),_(0x9b),_(0x9c),_(0x9d),_(0x9e),_(0x9f),
    _(0xa0),_(0xa1),_(0xa2),_(0xa3),_(0xa4),_(0xa5),_(0xa6),_(0xa7),_(0xa8),_(0xa9),_(0xaa),_(0xab),_(0xac),_(0xad),_(0xae),_(0xaf),
    _(0xb0),_(0xb1),_(0xb2),_(0xb3),_(0xb4),_(0xb5),_(0xb6),_(0xb7),_(0xb8),_(0xb9),_(0xba),_(0xbb),_(0xbc),_(0xbd),_(0xbe),_(0xbf),
    _(0xc0),_(0xc1),_(0xc2),_(0xc3),_(0xc4),_(0xc5),_(0xc6),_(0xc7),_(0xc8),_(0xc9),_(0xca),_(0xcb),_(0xcc),_(0xcd),_(0xce),_(0xcf),
    _(0xd0),_(0xd1),_(0xd2),_(0xd3),_(0xd4),_(0xd5),_(0xd6),_(0xd7),_(0xd8),_(0xd9),_(0xda),_(0xdb),_(0xdc),_(0xdd),_(0xde),_(0xdf),
    _(0xe0),_(0xe1),_(0xe2),_(0xe3),_(0xe4),_(0xe5),_(0xe6),_(0xe7),_(0xe8),_(0xe9),_(0xea),_(0xeb),_(0xec),_(0xed),_(0xee),_(0xef),
    _(0xf0),_(0xf1),_(0xf2),_(0xf3),_(0xf4),_(0xf5),_(0xf6),_(0xf7),_(0xf8),_(0xf9),_(0xfa),_(0xfb),_(0xfc),_(0xfd),_(0xfe),_(0xff)
};
#undef  _

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
Pix *
ll_check_Pix(lua_State *L, int arg)
{
    return *(reinterpret_cast<Pix **>(ll_check_udata(L, arg, LL_PIX)));
}

/**
 * \brief Push PIX to the Lua stack and set its meta table
 * \param L pointer to the lua_State
 * \param pix pointer to the PIX
 * \return 1 Pix* on the Lua stack
 */
int
ll_push_Pix(lua_State *L, Pix *pix)
{
    if (!pix)
        return ll_push_nil(L);
    return ll_push_udata(L, LL_PIX, pix);
}

/**
 * \brief Create and push a new Pix*
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be a string (filename)
 * or
 * Arg #1 is expected to be Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
int
ll_new_Pix(lua_State *L)
{
    static char buff[32 + PATH_MAX];
    Pix *pix = nullptr;
    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32_default(__func__, L, 1, 1);
        l_int32 height = ll_check_l_int32_default(__func__, L, 2, 1);
        l_int32 depth = ll_check_l_int32_default(__func__, L, 3, 1);
        pix = pixCreate(width, height, depth);
    } else if (lua_isuserdata(L, 1)) {
        Pix *pixs = ll_check_Pix(L, 1);
        pix = pixCreateTemplate(pixs);
    } else {
        const char* filename = lua_tostring(L, 1);
        pix = pixRead(filename);
        snprintf(buff, sizeof(buff), "filename: %s", filename);
        pixAddText(pix, buff);
    }
    return ll_push_Pix(L, pix);
}

/**
 * @brief toString
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    static char str[256];
    Pix *pix = ll_check_Pix(L, 1);
    luaL_Buffer B;
    char *dst = str;
    const char* format = nullptr;
    l_int32 w, h, d, spp, wpl, ccnt, ctot;
    l_uint64 size;
    PixColormap *cmap = nullptr;

    luaL_buffinit(L, &B);
    if (!pix) {
        luaL_addstring(&B, "nil");
    } else {
        if (pixGetDimensions(pix, &w, &h, &d)) {
            dst += snprintf(dst, sizeof(str), "invalid");
        } else {
            spp = pixGetSpp(pix);
            wpl = pixGetWpl(pix);
            size = static_cast<l_uint64>(wpl) * static_cast<l_uint64>(h) * sizeof(l_uint32);
            format = ll_string_input_format(pixGetInputFormat(pix));
            dst += snprintf(dst, sizeof(str), "[%s] %dx%d %dbpp; %dspp; %dwpl; %llu bytes",
                     format, w, h, d, spp, wpl, size);
        }
        cmap = pixGetColormap(pix);
        if (nullptr != cmap) {
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
 * \brief Create a new Pix*
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be a string (filename)
 * or
 * Arg #1 is expected to be Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Create(lua_State *L)
{
    return ll_new_Pix(L);
}

/**
 * \brief Create a new Pix* but don't initialize it
 *
 * Arg #1 is expected to be a l_int32 (width)
 * Arg #2 is expected to be a l_int32 (height)
 * Arg #3 is optional and expected to be a l_int32 (depth; default = 1)
 * or
 * Arg #1 is expected to be Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CreateNoInit(lua_State *L)
{
    Pix *pix = nullptr;
    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        l_int32 width = ll_check_l_int32(__func__, L, 1);
        l_int32 height = ll_check_l_int32(__func__, L, 2);
        l_int32 depth = ll_check_l_int32_default(__func__, L, 3, 1);
        pix = pixCreateNoInit(width, height, depth);
    } else if (lua_isuserdata(L, 1)) {
        Pix *pixs = ll_check_Pix(L, 1);
        pix = pixCreateTemplateNoInit(pixs);
    } else {
        /* FIXME: what is this? */
    }
    return ll_push_Pix(L, pix);
}

/**
 * \brief Destroy a Pix*
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    Pix **ppix = reinterpret_cast<Pix **>(ll_check_udata(L, 1, LL_PIX));
    DBG(LOG_DESTROY, "%s: '%s' ppix=%p pix=%p refcount=%d\n",
        __func__, LL_PIX, ppix, *ppix, pixGetRefcount(*ppix));
    pixDestroy(reinterpret_cast<Pix **>(ppix));
    *ppix = nullptr;
    return 0;
}

/**
 * \brief Copy a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixd = pixCopy(nullptr, pixs);
    return ll_push_Pix(L, pixd);
}

/**
 * \brief Clone a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixd = pixClone(pixs);
    return ll_push_Pix(L, pixd);
}

/**
 * \brief Resize a Pix* image data
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ResizeImageData(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixResizeImageData(pixd, pixs));
    return 1;
}

/**
 * \brief Copy the colormap of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyColormap(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopyColormap(pixd, pixs));
    return 1;
}

/**
 * \brief Check if a Pix* is valid
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SizesEqual(lua_State *L)
{
    Pix *pix1 = ll_check_Pix(L, 1);
    Pix *pix2 = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixSizesEqual(pix1, pix2));
    return 1;
}

/**
 * \brief Transfer all data from a Pix* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
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
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    int copytext = lua_isboolean(L, 3) ? lua_toboolean(L, 3) : TRUE;
    int copyformat = lua_isboolean(L, 4) ? lua_toboolean(L, 4) : TRUE;
    lua_pushboolean(L, 0 == pixTransferAllData(pixd, &pixs, copytext, copyformat));
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Swap an destroy a Pix* (pixd) with another Pix* (pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SwapAndDestroy(lua_State *L)
{
    void **ppixd = ll_check_udata(L, 1, LL_PIX);
    void **ppixs = ll_check_udata(L, 2, LL_PIX);
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixSwapAndDestroy(&pixd, &pixs));
    *ppixd = pixd;
    *ppixs = pixs;
    return 1;
}

/**
 * \brief Get the Pix* width
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetWidth(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 width = pixGetWidth(pix);
    lua_pushinteger(L, width);
    return 1;
}

/**
 * \brief Set the Pix* width
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWidth(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 width = ll_check_l_int32_default(__func__, L, 2, pixGetWidth(pix));
    lua_pushboolean(L, 0 == pixSetWidth(pix, width));
    return 1;
}

/**
 * \brief Get the Pix* height
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetHeight(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 height = pixGetHeight(pix);
    lua_pushinteger(L, height);
    return 1;
}

/**
 * \brief Set the Pix* height
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetHeight(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 height = ll_check_l_int32_default(__func__, L, 2, pixGetHeight(pix));
    lua_pushboolean(L, 0 == pixSetHeight(pix, height));
    return 1;
}

/**
 * \brief Get the Pix* depth
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetDepth(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 depth = pixGetDepth(pix);
    lua_pushinteger(L, depth);
    return 1;
}

/**
 * \brief Set the Pix* depth
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetDepth(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 depth = ll_check_l_int32_default(__func__, L, 2, pixGetDepth(pix));
    lua_pushboolean(L, 0 == pixSetDepth(pix, depth));
    return 1;
}

/**
 * \brief Get the Pix* dimensions
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 3 for four integers (or nil on error) on the stack
 */
static int
GetDimensions(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 width = ll_check_l_int32_default(__func__, L, 2, 0);
    l_int32 height = ll_check_l_int32_default(__func__, L, 3, 0);
    l_int32 depth = ll_check_l_int32_default(__func__, L, 4, 1);
    lua_pushboolean(L, 0 == pixSetDimensions(pix, width, height, depth));
    return 1;
}

/**
 * \brief Copy dimensions from a Pix* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyDimensions(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopyDimensions(pixd, pixs));
    return 1;
}

/**
 * \brief Get the Pix* SPP (samples per pixel)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetSpp(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 spp = pixGetSpp(pix);
    lua_pushinteger(L, spp);
    return 1;
}

/**
 * \brief Set the Pix* SPP (samples per pixel)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetSpp(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 spp = ll_check_l_int32_default(__func__, L, 2, pixGetSpp(pix));
    lua_pushboolean(L, 0 == pixSetSpp(pix, spp));
    return 1;
}

/**
 * \brief Copy SPP (samples per pixel) from a Pix* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopySpp(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopySpp(pixd, pixs));
    return 1;
}

/**
 * \brief Get the Pix* WPL (words per line)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetWpl(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 wpl = pixGetWpl(pix);
    lua_pushinteger(L, wpl);
    return 1;
}

/**
 * \brief Set the Pix* WPL (words per line)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetWpl(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 wpl = ll_check_l_int32_default(__func__, L, 2, pixGetWpl(pix));
    lua_pushboolean(L, 0 == pixSetWpl(pix, wpl));
    return 1;
}

/**
 * \brief Get the Pix* X resolution
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetXRes(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 xres = pixGetXRes(pix);
    lua_pushinteger(L, xres);
    return 1;
}

/**
 * \brief Set the Pix* X resolution
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetXRes(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 xres = ll_check_l_int32_default(__func__, L, 2, pixGetXRes(pix));
    lua_pushboolean(L, 0 == pixSetXRes(pix, xres));
    return 1;
}

/**
 * \brief Get the Pix* Y resolution
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 integers (or nil on error) on the stack
 */
static int
GetYRes(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 yres = pixGetYRes(pix);
    lua_pushinteger(L, yres);
    return 1;
}

/**
 * \brief Set the Pix* Y resolution
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetYRes(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 yres = ll_check_l_int32_default(__func__, L, 2, pixGetYRes(pix));
    lua_pushboolean(L, 0 == pixSetYRes(pix, yres));
    return 1;
}

/**
 * \brief Get the PIX resolution (x, y)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 *
 * \param L pointer to the lua_State
 * \return 2 for two integers (or nil on error) on the stack
 */
static int
GetResolution(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 xres, yres;
    if (pixGetResolution(pix, &xres, &yres))
        return ll_push_nil(L);
    lua_pushinteger(L, xres);
    lua_pushinteger(L, yres);
    return 2;
}

/**
 * \brief Set the PIX side resolution (x, y)
 *
 * Arg #1 (i.e. self) is expected to be a Pix*
 * Arg #2 is expected to be a lua_Integer (xres)
 * Arg #3 is expected to be a lua_Integer (yres)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean result true or false
 */
static int
SetResolution(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 xres = ll_check_l_int32_default(__func__, L, 2, 300);
    l_int32 yres = ll_check_l_int32_default(__func__, L, 3, xres);
    lua_pushboolean(L, 0 == pixSetResolution(pix, xres, yres));
    return 1;
}

/**
 * \brief Copy resolution from a Pix* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyResolution(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopyResolution(pixd, pixs));
    return 1;
}

/**
 * \brief Scale resolution of a Pix* by a factor
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (xscale)
 * Arg #2 is expected to be a lua_Number in the range of l_float32 (yscale)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
ScaleResolution(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_float32 xscale = ll_check_l_float32(__func__, L, 2);
    l_float32 yscale = ll_check_l_float32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixScaleResolution(pix, xscale, yscale));
    return 1;
}

/**
 * \brief Get the input format of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetInputFormat(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    lua_pushstring(L, ll_string_input_format(pixGetInputFormat(pix)));
    return 1;
}

/**
 * \brief Set the input format of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a string with the input format name
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetInputFormat(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 format = ll_check_input_format(__func__, L, 2, IFF_UNKNOWN);
    lua_pushboolean(L, 0 == pixSetInputFormat(pix, format));
    return 1;
}

/**
 * \brief Copy input format a Pix* (pixs) to self (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyInputFormat(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopyInputFormat(pixd, pixs));
    return 1;
}

/**
 * \brief Set the special value of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
SetSpecial(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 special = ll_check_l_int32(__func__, L, 2);
    lua_pushboolean(L, 0 == pixSetSpecial(pix, special));
    return 1;
}

/**
 * \brief Get the text of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetText(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    const char* text = pixGetText(pix);
    lua_pushstring(L, text);
    return 1;
}

/**
 * \brief Set the text of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a string
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetText(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    const char* text = lua_tostring(L, 2);
    lua_pushboolean(L, pixSetText(pix, text));
    return 1;
}

/**
 * \brief Add to the text of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a string
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddText(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    const char* text = lua_tostring(L, 2);
    lua_pushboolean(L, pixAddText(pix, text));
    return 1;
}

/**
 * \brief Copy text from a Pix* (%pixs) to self (%pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be another Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 for boolean on the Lua stack
 */
static int
CopyText(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    lua_pushboolean(L, 0 == pixCopyText(pixd, pixs));
    return 1;
}

/**
 * \brief Get the colormap of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
GetColormap(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    PixColormap* colormap = pixGetColormap(pix);
    return ll_push_PixColormap(L, colormap);
}

/**
 * \brief Set the colormap of a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a PixColormap* (colormap)
 *
 * \param L pointer to the lua_State
 * \return 1 PixColormap* on the Lua stack
 */
static int
SetColormap(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    PixColormap* colormap = ll_take_PixColormap(L, 2);
    lua_pushboolean(L, 0 == pixSetColormap(pix, colormap));
    return 1;
}

/**
 * \brief Get a pixel value from Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
GetPixel(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
    l_uint32 val = 0;
    if (pixGetPixel(pix, x, y, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Set a pixel value in Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
    l_uint32 val = ll_check_l_uint32(__func__, L, 4) - 1;
    lua_pushboolean(L, 0 == pixSetPixel(pix, x, y, val));
    return 1;
}

/**
 * \brief Get a pixel's RGB values from Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack
 */
static int
GetRGBPixel(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
    l_int32 rval = ll_check_l_int32(__func__, L, 4);
    l_int32 gval = ll_check_l_int32(__func__, L, 5);
    l_int32 bval = ll_check_l_int32(__func__, L, 6);
    lua_pushboolean(L, 0 == pixSetRGBPixel(pix, x, y, rval, gval, bval));
    return 1;
}

/**
 * \brief Get a random pixel's value from Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 3 l_int32 on the Lua stack (val, x, y)
 */
static int
GetRandomPixel(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ClearPixel(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixClearPixel(pix, x, y));
    return 1;
}

/**
 * \brief Flip a pixel value in Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (x)
 * Arg #3 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlipPixel(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 x = ll_check_l_int32(__func__, L, 2);
    l_int32 y = ll_check_l_int32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixFlipPixel(pix, x, y));
    return 1;
}

/**
 * \brief Get black or white value in Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (op = L_GET_BLACK_VAL or L_GET_WHITE_VAL)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackOrWhiteVal(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 op = ll_check_getval(__func__, L, 2, L_GET_BLACK_VAL);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, op, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get black value in Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBlackVal(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_BLACK_VAL, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Get white value in Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetWhiteVal(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_uint32 val = 0;
    if (pixGetBlackOrWhiteVal(pix, L_GET_WHITE_VAL, &val))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Clear all pixels in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ClearAll(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    lua_pushboolean(L, 0 == pixClearAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAll(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    lua_pushboolean(L, 0 == pixSetAll(pix));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to a gray value %grayval
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (grayval)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAllGray(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 grayval = ll_check_l_int32(__func__, L, 2);
    lua_pushboolean(L, 0 == pixSetAllGray(pix, grayval));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to an arbitrary value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetAllArbitrary(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_uint32 val = ll_check_l_uint32(__func__, L, 2);
    lua_pushboolean(L, 0 == pixSetAllArbitrary(pix, val));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to black or white
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a string describing a setval (op = black, white)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlackOrWhite(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 op = ll_check_blackwhite(__func__, L, 2, L_SET_BLACK);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, op));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to black
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetBlack(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_BLACK));
    return 1;
}

/**
 * \brief Set all pixels in a Pix* to white
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetWhite(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    lua_pushboolean(L, 0 == pixSetBlackOrWhite(pix, L_SET_WHITE));
    return 1;
}

/**
 * \brief Set all pixel components %comp in a Pix* to a value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a string with the component name (comp)
 * Arg #3 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
SetComponentArbitrary(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 comp = ll_check_component(__func__, L, 2, 0);
    l_int32 val = ll_check_l_int32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixSetComponentArbitrary(pix, comp, val));
    return 1;
}

/**
 * \brief Clear all pixels inside a Box* in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
ClearInRect(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    lua_pushboolean(L, 0 == pixClearInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a Box* in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRect(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    lua_pushboolean(L, 0 == pixSetInRect(pix, box));
    return 1;
}

/**
 * \brief Set all pixels inside a Box* in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a Box* (box)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetInRectArbitrary(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixSetInRectArbitrary(pix, box, val));
    return 1;
}

/**
 * \brief Set all pixels inside a Box* in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a Box* (box)
 * Arg #3 is expected to be a l_uint32 (val)
 * Arg #4 is expected to be a l_float32 (fract)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
BlendInRect(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    l_float32 fract = ll_check_l_float32(__func__, L, 4);
    lua_pushboolean(L, 0 == pixBlendInRect(pix, box, val, fract));
    return 1;
}

/**
 * \brief Set pad bits in a Pix* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPadBits(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 val = ll_check_l_int32(__func__, L, 2);
    lua_pushboolean(L, 0 == pixSetPadBits(pix, val));
    return 1;
}

/**
 * \brief Set pad bits in a Pix* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 by = ll_check_l_int32(__func__, L, 2);
    l_int32 bh = ll_check_l_int32(__func__, L, 3);
    l_int32 val = ll_check_l_int32(__func__, L, 4);
    lua_pushboolean(L, 0 == pixSetPadBitsBand(pix, by, bh, val));
    return 1;
}

/**
 * \brief Set or clear border pixels in a Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    l_int32 op = ll_check_rasterop(__func__, L, 6, PIX_CLR);
    lua_pushboolean(L, 0 == pixSetOrClearBorder(pix, left, right, top, bottom, op));
    return 1;
}

/**
 * \brief Set border pixels in a Pix* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    l_uint32 val = ll_check_l_uint32(__func__, L, 6);
    lua_pushboolean(L, 0 == pixSetBorderVal(pix, left, right, top, bottom, val));
    return 1;
}

/**
 * \brief Set border pixels in a Pix* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (dist)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetBorderRingVal(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 dist = ll_check_l_int32(__func__, L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixSetBorderRingVal(pix, dist, val));
    return 1;
}

/**
 * \brief Set mirrored border pixels in a Pix* to value %val
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    lua_pushboolean(L, 0 == pixSetMirroredBorder(pix, left, right, top, bottom));
    return 1;
}

/**
 * \brief Copy border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
CopyBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixCopyBorder(nullptr, pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (npix)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 npix = ll_check_l_int32(__func__, L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    Pix* pix = pixAddBorder(pixs, npix, val);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add black or white border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a operation (op = L_GET_BLACK_VAL or L_GET_WHITE_VAL)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBlackOrWhiteBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    l_int32 op = ll_check_getval(__func__, L, 6, L_GET_BLACK_VAL);
    Pix* pix = pixAddBlackOrWhiteBorder(pixs, left, right, top, bottom, op);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 * Arg #6 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddBorderGeneral(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    l_uint32 val = ll_check_l_uint32(__func__, L, 6);
    Pix* pix = pixAddBorderGeneral(pixs, left, right, top, bottom, val);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels in a Pix* to a new Pix* (simple case %npix)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (npix)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
RemoveBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 npix = ll_check_l_int32(__func__, L, 2);
    Pix* pix = pixRemoveBorder(pixs, npix);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels in a Pix* to a new Pix* (general case)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
RemoveBorderGeneral(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixRemoveBorderGeneral(pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Remove border pixels resizing a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (width)
 * Arg #3 is expected to be a l_int32 (height)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
RemoveBorderToSize(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 width = ll_check_l_int32(__func__, L, 2);
    l_int32 height = ll_check_l_int32(__func__, L, 3);
    Pix* pix = pixRemoveBorderToSize(pixs, width, height);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add mirrored border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddMirroredBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixAddMirroredBorder(pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add repeated border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddRepeatedBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixAddRepeatedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add mixed border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddMixedBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixAddMixedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Add continued border pixels in a Pix* to a new Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a l_int32 (left)
 * Arg #3 is expected to be a l_int32 (right)
 * Arg #4 is expected to be a l_int32 (top)
 * Arg #5 is expected to be a l_int32 (bottom)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
AddContinuedBorder(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 left = ll_check_l_int32(__func__, L, 2);
    l_int32 right = ll_check_l_int32(__func__, L, 3);
    l_int32 top = ll_check_l_int32(__func__, L, 4);
    l_int32 bottom = ll_check_l_int32(__func__, L, 5);
    Pix* pix = pixAddContinuedBorder(pixs, left, right, top, bottom);
    ll_push_Pix(L, pix);
    return 1;
}

/**
 * \brief Shift and transfer alpha channel from a Pix* (pixs) to this Pix* (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    l_int32 shiftx = ll_check_l_int32(__func__, L, 3);
    l_int32 shifty = ll_check_l_int32(__func__, L, 4);
    lua_pushboolean(L, pixShiftAndTransferAlpha(pixd, pixs, shiftx, shifty));
    return 1;
}

/**
 * \brief Display the layers of a Pix* (pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_uint32 val = ll_check_l_uint32(__func__, L, 2);
    l_int32 maxw = ll_check_l_int32(__func__, L, 3);
    ll_push_Pix(L, pixDisplayLayersRGBA(pixs, val, maxw));
    return 1;
}

/**
 * \brief Create a new Pix* from three Pix* layers (%pixr, %pixg, %pixb)
 *
 * Arg #1 is expected to be a Pix* (pixr)
 * Arg #2 is expected to be a Pix* (pixg)
 * Arg #3 is expected to be a Pix* (pixb)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CreateRGBImage(lua_State *L)
{
    Pix *pixr = ll_check_Pix(L, 1);
    Pix *pixg = ll_check_Pix(L, 2);
    Pix *pixb = ll_check_Pix(L, 3);
    ll_push_Pix(L, pixCreateRGBImage(pixr, pixg, pixb));
    return 1;
}

/**
 * \brief Create a new Pix* from one component of Pix*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetRGBComponent(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 comp = ll_check_component(__func__, L, 2, L_ALPHA_CHANNEL);
    ll_push_Pix(L, pixGetRGBComponent(pixs, comp));
    return 1;
}

/**
 * \brief Set one component in Pix* (pixd 32bpp) from Pix* (pixs 8bpp)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* (pixs)
 * Arg #3 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetRGBComponent(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    l_int32 comp = ll_check_component(__func__, L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixSetRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Copy one component in Pix* (pixd 32bpp) from Pix* (pixs 32bpp)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* (pixs)
 * Arg #3 is expected to be a string with the component name (comp)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyRGBComponent(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    l_int32 comp = ll_check_component(__func__, L, 3, L_ALPHA_CHANNEL);
    lua_pushboolean(L, 0 == pixCopyRGBComponent(pixd, pixs, comp));
    return 1;
}

/**
 * \brief Extract red, green and blue components from Pix* (pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetRGBLine(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 row = ll_check_l_int32(__func__, L, 2);
    size_t width = (size_t) pixGetWidth(pixs);
    l_uint8 *bufr = (l_uint8 *) LEPT_CALLOC(width, sizeof(l_uint8));
    l_uint8 *bufg = (l_uint8 *) LEPT_CALLOC(width, sizeof(l_uint8));
    l_uint8 *bufb = (l_uint8 *) LEPT_CALLOC(width, sizeof(l_uint8));
    if (pixGetRGBLine(pixs, row, bufr, bufg, bufb)) {
        LEPT_FREE(bufr);
        LEPT_FREE(bufg);
        LEPT_FREE(bufb);
        return ll_push_nil(L);
    }
    lua_pushlstring(L, (const char *)bufr, width);
    lua_pushlstring(L, (const char *)bufg, width);
    lua_pushlstring(L, (const char *)bufb, width);
    LEPT_FREE(bufr);
    LEPT_FREE(bufg);
    LEPT_FREE(bufb);
    return 3;
}

/**
 * \brief Check alpha layer of a Pix* for opaqueness
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AlphaIsOpaque(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    l_int32 opaque = 0;
    if (pixAlphaIsOpaque(pix, &opaque))
        return ll_push_nil(L);
    lua_pushboolean(L, opaque);
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* with 1 bit/pixel (pixm)
 * Arg #3 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMasked(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    lua_pushboolean(L, 0 == pixSetMasked(pixd, pixm, val));
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* with 1 bit/pixel (pixm)
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
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_uint32 val = ll_check_l_uint32(__func__, L, 3);
    l_int32 x = ll_check_l_int32(__func__, L, 4);
    l_int32 y = ll_check_l_int32(__func__, L, 5);
    lua_pushboolean(L, 0 == pixSetMaskedGeneral(pixd, pixm, val, x, y));
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* (pixs)
 * Arg #3 is expected to be a Pix* with 1 bit/pixel (pixm)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMasked(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    Pix *pixm = ll_check_Pix(L, 3);
    lua_pushboolean(L, 0 == pixCombineMasked(pixd, pixs, pixm));
    return 1;
}

/**
 * \brief Set the mask for a Pix* (%pixd) from another Pix* (%pixm 1bpp) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* (pixs)
 * Arg #3 is expected to be a Pix* with 1 bit/pixel (pixm)
 * Arg #4 is expected to be a l_int32 (x)
 * Arg #5 is expected to be a l_int32 (y)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CombineMaskedGeneral(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixs = ll_check_Pix(L, 2);
    Pix *pixm = ll_check_Pix(L, 3);
    l_int32 x = ll_check_l_int32(__func__, L, 4);
    l_int32 y = ll_check_l_int32(__func__, L, 5);
    lua_pushboolean(L, 0 == pixCombineMaskedGeneral(pixd, pixs, pixm, x, y));
    return 1;
}

/**
 * \brief Paint %val through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* with 1 bit/pixel (pixm)
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
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_uint32 val = ll_check_l_uint32(__func__, L, 5);
    lua_pushboolean(L, 0 == pixPaintThroughMask(pixd, pixm, x, y, val));
    return 1;
}

/**
 * \brief Paint %val through a mask Pix* (%pixm) onto a Pix* (%pixd) at offset %x and %y
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a Pix* with 1 bit/pixel (pixm)
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
    Pix *pixd = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 searchdir = ll_check_searchdir(__func__, L, 5, L_BOTH_DIRECTIONS);
    l_int32 mindist = ll_check_l_int32(__func__, L, 6);
    l_int32 tilesize = ll_check_l_int32(__func__, L, 7);
    l_int32 ntiles = ll_check_l_int32(__func__, L, 8);
    l_int32 distblend = ll_check_l_int32_default(__func__, L, 9, 0);
    lua_pushboolean(L, 0 == pixPaintSelfThroughMask(pixd, pixm, x, y, searchdir, mindist, tilesize, ntiles, distblend));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a mask value (%val)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromVal(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 val = ll_check_l_int32(__func__, L, 2);
    ll_push_Pix(L, pixMakeMaskFromVal(pixs, val));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using a 2^depth entry lookup-table (%lut)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string of max. 256 bytes (lut)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
MakeMaskFromLUT(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    size_t len = 0;
    const char* lut = lua_tolstring(L, 2, &len);
    l_int32* tab = nullptr;
    size_t i;

    tab = (l_int32 *) LEPT_CALLOC(256, sizeof(l_int32));
    /* expand lookup-table (lut) to array of l_int32 (tab) */
    for (i = 0; i < 256 && i < len; i++)
        tab[i] = lut[i];
    ll_push_Pix(L, pixMakeMaskFromLUT(pixs, tab));
    LEPT_FREE(tab);
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from a source Pix* (%pixs) using arithmetic factors (%rc, %gc, %bc)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_float32 rc = ll_check_l_float32(__func__, L, 2);
    l_float32 gc = ll_check_l_float32(__func__, L, 3);
    l_float32 bc = ll_check_l_float32(__func__, L, 4);
    l_float32 thresh = ll_check_l_float32(__func__, L, 5);
    ll_push_Pix(L, pixMakeArbMaskFromRGB(pixs, rc, gc, bc, thresh));
    return 1;
}

/**
 * \brief Create a new Pix* (%pixd) from Pix* (%pixs) seting alpha == 0 pixels to a value (%val)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_uint32 (val)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
SetUnderTransparency(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_uint32 val = ll_check_l_uint32(__func__, L, 2);
    ll_push_Pix(L, pixSetUnderTransparency(pixs, val, 0));
    return 1;
}

/**
 * \brief Create a new alpha mask Pix* (%pixd) from Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (dist)
 * Arg #3 is optional and, if given, expected to be a boolean (getbox)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack or 2 Pix* and Box* on the Lua stack
 */
static int
MakeAlphaFromMask(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 dist = ll_check_l_int32(__func__, L, 2);
    int getbox = lua_isboolean(L, 3) ?  lua_toboolean(L, 3) : FALSE;
    Box* box = nullptr;
    Pix* pixd = pixMakeAlphaFromMask(pixs, dist, getbox ? &box : nullptr);
    ll_push_Pix(L, pixd);
    if (nullptr != pixd && nullptr != box) {
        ll_push_Box(L, box);
        return 2;
    }
    return 1;
}

/**
 * \brief Get the color near the mask boundary from Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm)
 * Arg #3 is expected to be a Box* (box)
 * Arg #4 is expected to be a l_int32 (dist)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorNearMaskBoundary(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    Box *box = ll_check_Box(L, 3);
    l_uint32 val = 0;
    l_int32 dist = ll_check_l_int32(__func__, L, 4);
    if (pixGetColorNearMaskBoundary(pixs, pixm, box, dist, &val, 0))
        return ll_push_nil(L);
    lua_pushinteger(L, val);
    return 1;
}

/**
 * \brief Invert the Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Invert(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixd = pixInvert(nullptr, pixs);
    ll_push_Pix(L, pixd);
    return 1;
}

/**
 * \brief And the Pix* (%pix1) and Pix* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1)
 * Arg #2 is expected to be another Pix* (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
And(lua_State *L)
{
    Pix *pix1 = ll_check_Pix(L, 1);
    Pix *pix2 = ll_check_Pix(L, 2);
    Pix *pixd = pixAnd(nullptr, pix1, pix2);
    ll_push_Pix(L, pixd);
    return 1;
}

/**
 * \brief Or the Pix* (%pix1) and Pix* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1)
 * Arg #2 is expected to be another Pix* (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Or(lua_State *L)
{
    Pix *pix1 = ll_check_Pix(L, 1);
    Pix *pix2 = ll_check_Pix(L, 2);
    Pix *pixd = pixOr(nullptr, pix1, pix2);
    ll_push_Pix(L, pixd);
    return 1;
}

/**
 * \brief Xor the Pix* (%pix1) and Pix* (%pix2)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1)
 * Arg #2 is expected to be another Pix* (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Xor(lua_State *L)
{
    Pix *pix1 = ll_check_Pix(L, 1);
    Pix *pix2 = ll_check_Pix(L, 2);
    Pix *pixd = pixXor(nullptr, pix1, pix2);
    ll_push_Pix(L, pixd);
    return 1;
}

/**
 * \brief Subtract the Pix* (%pix2) from Pix* (%pix1)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix1)
 * Arg #2 is expected to be another Pix* (pix2)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Subtract(lua_State *L)
{
    Pix *pix1 = ll_check_Pix(L, 1);
    Pix *pix2 = ll_check_Pix(L, 2);
    Pix *pixd = pixSubtract(nullptr, pix1, pix2);
    ll_push_Pix(L, pixd);
    return 1;
}

/**
 * \brief Check if all pixels in Pix* (%pixs) are 0
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Zero(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 empty = 0;
    if (pixZero(pixs, &empty))
        return ll_push_nil(L);
    lua_pushboolean(L, empty);
    return 1;
}

/**
 * \brief Calculate the fraction of foreground in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
ForegroundFraction(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_float32 fract = 0;
    if (pixForegroundFraction(pixs, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number) fract);
    return 1;
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixels(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 count = 0;
    if (pixCountPixels(pixs, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of foreground pixels in Pix* (%pixs) in a Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountPixelsInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRect(pixs, box, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Count the number of pixels by row in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByRow(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Numa *na = pixCountPixelsByRow(pixs, nullptr);
    return ll_push_Numa(L, na);
}

/**
 * \brief Count the number of pixels by column in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsByColumn(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Numa *na = pixCountPixelsByColumn(pixs);
    return ll_push_Numa(L, na);
}

/**
 * \brief Count the number of pixels in row %row of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountPixelsInRow(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 row = ll_check_l_int32(__func__, L, 2);
    l_int32 count = 0;
    if (pixCountPixelsInRow(pixs, row, &count, nullptr))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get the moment of order %order by column in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (order; 1 or 2)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetMomentByColumn(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 order = ll_check_l_int32(__func__, L, 2);
    Numa *na = pixGetMomentByColumn(pixs, order);
    return ll_push_Numa(L, na);
}

/**
 * \brief Check if the pixel sum of Pix* (%pixs) is above threshold %thresh
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (thresh)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ThresholdPixelSum(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 thresh = ll_check_l_int32(__func__, L, 2);
    l_int32 above = 0;
    if (pixThresholdPixelSum(pixs, thresh, &above, nullptr))
        return ll_push_nil(L);
    lua_pushboolean(L, above);
    return 1;
}

/**
 * \brief Build the average by row of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 * Arg #3 is optional and, if specified, expected to be a string (type: white-is-max or black-is-max)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageByRow(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    l_int32 type = ll_check_what_is_max(__func__, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByRow(pixs, box, type);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the average by column of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 * Arg #3 is optional and, if specified, expected to be a string (type: white-is-max or black-is-max)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageByColumn(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    l_int32 type = ll_check_what_is_max(__func__, L, 3, L_WHITE_IS_MAX);
    Numa* na = pixAverageByColumn(pixs, box, type);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the average inside a Box* (%box) of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AverageInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    l_float32 ave = 0.0f;
    if (pixAverageInRect(pixs, box, &ave))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number) ave);
    return 1;
}

/**
 * \brief Build the variance by row of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceByRow(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa* na = pixVarianceByRow(pixs, box);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the variance by column of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceByColumn(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa* na = pixVarianceByColumn(pixs, box);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the square root of the variance inside a Box* (%box) of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
VarianceInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    l_float32 sqrvar = 0.0f;
    if (pixVarianceInRect(pixs, box, &sqrvar))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number) sqrvar);
    return 1;
}

/**
 * \brief Build the absolute difference by row of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffByRow(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa* na = pixAbsDiffByRow(pixs, box);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the absolute difference by column of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffByColumn(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa* na = pixAbsDiffByColumn(pixs, box);
    return ll_push_Numa(L, na);
}

/**
 * \brief Build the absolute difference inside a Box* (%box) of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 * Arg #2 is optional and, if specified, expected to be a string (dir: horizontal-line or vertical-line)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    l_int32 dir = ll_check_direction(__func__, L, 3, L_HORIZONTAL_LINE);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffInRect(pixs, box, dir, &absdiff))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number) absdiff);
    return 1;
}

/**
 * \brief Build absolute difference on a line (%x1,%y1 to %x2,%y2) of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
AbsDiffOnLine(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 x1 = ll_check_l_int32(__func__, L, 2);
    l_int32 y1 = ll_check_l_int32(__func__, L, 3);
    l_int32 x2 = ll_check_l_int32(__func__, L, 4);
    l_int32 y2 = ll_check_l_int32(__func__, L, 5);
    l_float32 absdiff = 0.0f;
    if (pixAbsDiffOnLine(pixs, x1, y1, x2, y2, &absdiff))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number) absdiff);
    return 1;
}

/**
 * \brief Count pixels of arbitrary value %val in Box* (%box) of Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (val)
 * Arg #3 is expected to be a l_int32 (factor)
 * Arg #4 is optional and, if specified, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
CountArbInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 val = ll_check_l_int32(__func__, L, 2);
    l_int32 factor = ll_check_l_int32(__func__, L, 3);
    Box *box = lua_isuserdata(L, 4) ? ll_check_Box(L, 4) : nullptr;
    l_int32 count;
    if (pixCountArbInRect(pixs, box, val, factor, &count))
        return ll_push_nil(L);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Create a mirrored tiling of the Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (w)
 * Arg #3 is expected to be a l_int32 (h)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
MirroredTiling(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 w = ll_check_l_int32(__func__, L, 2);
    l_int32 h = ll_check_l_int32(__func__, L, 3);
    return ll_push_Pix(L, pixMirroredTiling(pixs, w, h));
}

/**
 * \brief Look for one or two square tiles with conforming median
 *        intensity and low variance outside but near the input %box.
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Box* (box)
 * Arg #3 is expected to be a string describing the search direction (searchdir)
 * Arg #4 is expected to be a l_int32 (mindist)
 * Arg #5 is expected to be a l_int32 (tsize)
 * Arg #6 is expected to be a l_int32 (ntiles)
 *
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
FindRepCloseTile(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_int32 searchdir = ll_check_direction(__func__, L, 3, L_HORIZ);
    l_int32 mindist = ll_check_l_int32(__func__, L, 4);
    l_int32 tsize = ll_check_l_int32(__func__, L, 5);
    l_int32 ntiles = ll_check_l_int32(__func__, L, 6);
    Box *boxtile = nullptr;
    if (pixFindRepCloseTile(pixs, box, searchdir, mindist, tsize, ntiles, &boxtile, 0))
        return ll_push_nil(L);
    return ll_push_Box(L, boxtile);
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogram(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    return ll_push_Numa(L, pixGetGrayHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) masked with another Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogramMasked(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 factor = ll_check_l_int32(__func__, L, 5);
    return ll_push_Numa(L, pixGetGrayHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the grayscale Pix* (%pixs) inside Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Box* (box)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetGrayHistogramInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_int32 factor = ll_check_l_int32(__func__, L, 3);
    return ll_push_Numa(L, pixGetGrayHistogramInRect(pixs, box, factor));
}

/**
 * \brief Get an array of histograms of the grayscale Pix* (%pixs) for %nx by %ny tiles
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_int32 (nx)
 * Arg #4 is expected to be a l_int32 (ny)
 *
 * \param L pointer to the lua_State
 * \return 1 Numaa* on the Lua stack
 */
static int
GetGrayHistogramTiled(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    l_int32 nx = ll_check_l_int32(__func__, L, 3);
    l_int32 ny = ll_check_l_int32(__func__, L, 4);
    return ll_push_Numaa(L, pixGetGrayHistogramTiled(pixs, factor, nx, ny));
}

/**
 * \brief Get the RGB histograms of the Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 Numa* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogram(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    Numa *nar = nullptr;
    Numa *nag = nullptr;
    Numa *nab = nullptr;
    if (pixGetColorHistogram(pixs, factor, &nar, &nag, &nab))
        return ll_push_nil(L);
    return ll_push_Numa(L, nar) + ll_push_Numa(L, nag) + ll_push_Numa(L, nab);
}

/**
 * \brief Get the RGB histograms of the Pix* (%pixs) masked with another Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 3 Numa* on the Lua stack (red, green, blue)
 */
static int
GetColorHistogramMasked(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 factor = ll_check_l_int32(__func__, L, 5);
    Numa *nar = nullptr;
    Numa *nag = nullptr;
    Numa *nab = nullptr;
    if (pixGetColorHistogramMasked(pixs, pixm, x, y, factor, &nar, &nag, &nab))
        return ll_push_nil(L);
    return ll_push_Numa(L, nar) + ll_push_Numa(L, nag) + ll_push_Numa(L, nab);
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogram(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    return ll_push_Numa(L, pixGetCmapHistogram(pixs, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) masked with another Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm) (1bpp)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogramMasked(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 factor = ll_check_l_int32(__func__, L, 5);
    return ll_push_Numa(L, pixGetCmapHistogramMasked(pixs, pixm, x, y, factor));
}

/**
 * \brief Get the histogram of the color mapped Pix* (%pixs) inside Box* (%box)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Box* (box)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetCmapHistogramInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = ll_check_Box(L, 2);
    l_int32 factor = ll_check_l_int32(__func__, L, 3);
    return ll_push_Numa(L, pixGetCmapHistogramInRect(pixs, box, factor));
}

/**
 * \brief Count the RGB colors in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
CountRGBColors(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 count = pixCountRGBColors(pixs);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * \brief Get a histogram AMAP* for the colors in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetColorAmapHistogram(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    return ll_push_Amap(L, pixGetColorAmapHistogram(pixs, factor));
}

/**
 * \brief Get rank value for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_float32 (rank)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetRankValue(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32(__func__, L, 2);
    l_float32 rank = ll_check_l_float32(__func__, L, 3);
    l_uint32 value = 0;
    if (pixGetRankValue(pixs, factor, rank, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get rank values for RGB for Pix* (%pixs) masked with another Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm)
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
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 factor = ll_check_l_int32(__func__, L, 5);
    l_float32 rank = ll_check_l_int32(__func__, L, 6);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixm)
 * Arg #3 is expected to be a l_int32 (x)
 * Arg #4 is expected to be a l_int32 (y)
 * Arg #5 is expected to be a l_int32 (factor)
 * Arg #6 is expected to be a l_float32 (rank)
 *
 * \param L pointer to the lua_State
 * \return 1 number (value) and a Numa* (histogram) on the Lua stack
 */
static int
GetRankValueMasked(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = ll_check_Pix(L, 2);
    l_int32 x = ll_check_l_int32(__func__, L, 3);
    l_int32 y = ll_check_l_int32(__func__, L, 4);
    l_int32 factor = ll_check_l_int32(__func__, L, 5);
    l_float32 rank = ll_check_l_int32(__func__, L, 6);
    l_float32 value = 0.0f;
    Numa *na = nullptr;
    if (pixGetRankValueMasked(pixs, pixm, x, y, factor, rank, &value, &na))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)value);
    ll_push_Numa(L, na);
    return 1;
}

/**
 * \brief Get the pixel average for Pix* (%pixs) optionally masked with Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if given, expected to be a Pix* (pixm)
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
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixm = lua_isuserdata(L, 2) ? ll_check_Pix(L, 2) : nullptr;
    l_int32 x = ll_check_l_int32_default(__func__, L, 3, 0);
    l_int32 y = ll_check_l_int32_default(__func__, L, 4, 0);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 5, 1);
    l_uint32 value = 0;
    if (pixGetPixelAverage(pixs, pixm, x,y, factor, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel stats for Pix* (%pixs) optionally masked with Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is expected to be a l_int32 (factor)
 *
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetPixelStats(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 3, 1);
    l_uint32 value = 0;
    if (pixGetPixelStats(pixs, factor, type, &value))
        return ll_push_nil(L);
    lua_pushinteger(L, value);
    return 1;
}

/**
 * \brief Get the pixel average RGB values for Pix* (%pixs) optionally masked with Pix* (%pixm)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is optional and, if given, expected to be a Pix* (pixm)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = lua_isuserdata(L, 3) ? ll_check_Pix(L, 3) : nullptr;
    l_int32 x = ll_check_l_int32_default(__func__, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(__func__, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 6, 1);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string describing the type of stats (type)
 * Arg #3 is optional and, if given, expected to be a Pix* (pixm)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    Pix *pixm = lua_isuserdata(L, 3) ? ll_check_Pix(L, 3) : nullptr;
    l_int32 x = ll_check_l_int32_default(__func__, L, 4, 0);
    l_int32 y = ll_check_l_int32_default(__func__, L, 5, 0);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 6, 1);
    l_float32 value = 0.0;
    if (pixGetAverageMasked(pixs, pixm, x, y, factor, type, &value))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)value);
    return 3;
}

/**
 * \brief Get the average RGB values for Pix* (%pixs) as three Pix* (%pixr, %pixg, %pixb)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(__func__, L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(__func__, L, 4, 2);
    Pix *pixr = nullptr;
    Pix *pixg = nullptr;
    Pix *pixb = nullptr;
    if (pixGetAverageTiledRGB(pixs, sx, sy, type, &pixr, &pixg, &pixb))
        return ll_push_nil(L);
    return ll_push_Pix(L, pixr) + ll_push_Pix(L, pixg) + ll_push_Pix(L, pixb);
}

/**
 * \brief Get the average value for Pix* (%pixs) as Pix* (%pixv)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_stats_type(__func__, L, 2, L_MEAN_ABSVAL);
    l_int32 sx = ll_check_l_int32_default(__func__, L, 3, 2);
    l_int32 sy = ll_check_l_int32_default(__func__, L, 4, 2);
    Pix *pixv = pixGetAverageTiled(pixs, sx, sy, type);
    return ll_push_Pix(L, pixv);
}

/**
 * \brief Get the row stats for Pix* (%pixs) as six Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if given, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 6 Numa* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
RowStats(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa *mean = nullptr;
    Numa *median = nullptr;
    Numa *mode = nullptr;
    Numa *modecount = nullptr;
    Numa *var = nullptr;
    Numa *rootvar = nullptr;
    if (pixRowStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return ll_push_nil(L);
    return ll_push_Numa(L, mean) +
            ll_push_Numa(L, median) +
            ll_push_Numa(L, mode) +
            ll_push_Numa(L, modecount) +
            ll_push_Numa(L, var) +
            ll_push_Numa(L, rootvar);
}

/**
 * \brief Get the column stats for Pix* (%pixs) as six Numa*
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional and, if given, expected to be a Box* (box)
 *
 * \param L pointer to the lua_State
 * \return 6 Numa* on the Lua stack (mean, median, mode, modecount, var, rootvar)
 */
static int
ColumnStats(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
    Numa *mean = nullptr;
    Numa *median = nullptr;
    Numa *mode = nullptr;
    Numa *modecount = nullptr;
    Numa *var = nullptr;
    Numa *rootvar = nullptr;
    if (pixColumnStats(pixs, box, &mean, &median, &mode, &modecount, &var, &rootvar))
        return ll_push_nil(L);
    return ll_push_Numa(L, mean) +
            ll_push_Numa(L, median) +
            ll_push_Numa(L, mode) +
            ll_push_Numa(L, modecount) +
            ll_push_Numa(L, var) +
            ll_push_Numa(L, rootvar);
}

/**
 * \brief Get the range values for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a l_int32 (color)
 *
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack (minval, maxval)
 */
static int
GetRangeValues(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 2, 1);
    l_int32 color = ll_check_select_color(__func__, L, 3, L_SELECT_RED);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 (factor)
 * Arg #3 is expected to be a string describing the type (type; min or max)
 *
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack (rval, gval, bval, grayval)
 */
static int
GetExtremeValue(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 2, 1);
    l_int32 type = ll_check_select_min_max(__func__, L, 3, L_SELECT_MIN);
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is optional an, if given, expected to be a Box*
 *
 * \param L pointer to the lua_State
 * \return 3 integers on the Lua stack (maxval, xmax, ymax)
 */
static int
GetMaxValueInRect(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Box *box = lua_isuserdata(L, 2) ? ll_check_Box(L, 2) : nullptr;
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
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 2, 2);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 color = ll_check_select_color(__func__, L, 4, L_SELECT_RED);
    l_int32 fontsize = ll_check_l_int32_default(__func__, L, 5, 0);
    l_int32 minval = 0;
    l_int32 maxval = 0;
    l_uint32 *carray = nullptr;
    l_int32 i;

    if (pixGetBinnedComponentRange(pixs, nbins, factor, color, &minval, &maxval, &carray, fontsize))
        return ll_push_nil(L);
    lua_pushinteger(L, minval);
    lua_pushinteger(L, maxval);
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    free(carray);
    return 2 + nbins;
}

/**
 * \brief Get a rank color array for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
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
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 2, 1);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 type = ll_check_select_color(__func__, L, 4, L_SELECT_AVERAGE);
    l_uint32 *carray = nullptr;
    l_int32 i;

    if (pixGetRankColorArray(pixs, nbins, type, factor, &carray, 0, 0))
        return ll_push_nil(L);
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    free(carray);
    return nbins;
}

/**
 * \brief Get a binned color for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a Pix* (pixg)
 * Arg #3 is expected to be a l_int32 (nbins)
 * Arg #4 is expected to be a l_int32 (factor)
 * Arg #5 is expected to be a Numa* (alut)
 *
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
GetBinnedColor(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    Pix *pixg = ll_check_Pix(L, 2);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 3, 1);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 4, 1);
    Numa *alut = ll_check_Numa(L, 5);
    l_uint32 *carray = nullptr;
    l_int32 i;

    if (pixGetBinnedColor(pixs, pixg, factor, nbins, alut, &carray, 0))
        return ll_push_nil(L);
    for (i = 0; i < nbins; i++)
        lua_pushinteger(L, carray[i]);
    return nbins;
}

/**
 * \brief Get a binned color for Pix* (%pixs)
 *
 * Arg #1 is expected to be a l_int32 (side)
 * Arg #2 is expected to be a l_int32 (ncols)
 * Arg #3 is expected to be a l_int32 (fontsize)
 * Arg #4 .. n is expected to be l_uint32 (carray)
 *
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
DisplayColorArray(lua_State *L)
{
    l_uint32 *carray = nullptr;
    l_int32 ncolors = 0;
    l_int32 side = ll_check_l_int32(__func__, L, 1);
    l_int32 ncols = ll_check_l_int32(__func__, L, 2);
    l_int32 fontsize = ll_check_l_int32_default(__func__, L, 3, 0);
    int i = 3;
    Pix *pixd = nullptr;

    while (!lua_isnil(L, ncolors + 4))
        ncolors++;
    carray = (l_uint32 *) LEPT_CALLOC(ncolors, sizeof(l_uint32));
    for (i = 0; i < ncolors; i++)
        carray[i] = ll_check_l_uint32(__func__, L, i + 4);
    pixd = pixDisplayColorArray(carray, ncolors, side, ncols, fontsize);
    return ll_push_Pix(L, pixd);
}

/**
 * \brief Get a binned color for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a string describing the select color type (type)
 * Arg #3 is expected to be a string describing the direction (direction)
 * Arg #4 is expected to be a l_int32 (size)
 * Arg #5 is expected to be a l_int32 (nbins)
 *
 * \param L pointer to the lua_State
 * \return nbins integers on the Lua stack (carray[])
 */
static int
RankBinByStrip(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_select_color(__func__, L, 2, L_SELECT_RED);
    l_int32 direction = ll_check_direction(__func__, L, 3, L_SCAN_HORIZONTAL);
    l_int32 size = ll_check_l_int32_default(__func__, L, 4, 200);
    l_int32 nbins = ll_check_l_int32_default(__func__, L, 5, 2);
    Pix *pixd = pixRankBinByStrip(pixs, direction, size, nbins, type);
    return ll_push_Pix(L, pixd);
}

/**
 * \brief Get row stats for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetRowStats(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_select_color(__func__, L, 2, L_SELECT_RED);
    l_int32 nbins = ll_check_l_int32(__func__, L, 3);
    l_int32 thresh = ll_check_l_int32_default(__func__, L, 4, 0);
    l_float32 *colvect = (l_float32 *)LEPT_CALLOC(nbins, sizeof(l_float32));
    l_int32 i;
    if (pixGetRowStats(pixs, type, nbins, thresh, colvect))
        return ll_push_nil(L);
    for (i = 0; i < nbins; i++)
        lua_pushnumber(L, (lua_Number) colvect[i]);
    return nbins;
}

/**
 * \brief Get row stats for Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 *
 * \param L pointer to the lua_State
 * \return nbins numbers on the Lua stack (colvect[])
 */
static int
GetColumnStats(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 type = ll_check_select_color(__func__, L, 2, L_SELECT_RED);
    l_int32 nbins = ll_check_l_int32(__func__, L, 3);
    l_int32 thresh = ll_check_l_int32_default(__func__, L, 4, 0);
    l_float32 *rowvect = (l_float32 *)LEPT_CALLOC(nbins, sizeof(l_float32));
    l_int32 i;
    if (pixGetColumnStats(pixs, type, nbins, thresh, rowvect))
        return ll_push_nil(L);
    for (i = 0; i < nbins; i++)
        lua_pushnumber(L, (lua_Number) rowvect[i]);
    return nbins;
}

/**
 * \brief Set a pixel column in Pix* (%pixd)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixd)
 * Arg #2 is expected to be a l_int32 (col)
 * Arg #3 .. n is expected to be lua_Numbers / l_float32 for each row of pixd (colvect)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack (result)
 */
static int
SetPixelColumn(lua_State *L)
{
    Pix *pixd = ll_check_Pix(L, 1);
    l_int32 col = ll_check_l_int32(__func__, L, 2);
    l_int32 rows = pixGetHeight(pixd);
    l_float32 *colvect = (l_float32 *)LEPT_CALLOC(rows, sizeof(l_float32));
    l_int32 i;
    int result = FALSE;
    for (i = 0; i < rows; i++)
        colvect[i] = (l_float32) lua_tonumber(L, i+3);
    result = pixSetPixelColumn(pixd, col, colvect);
    LEPT_FREE(colvect);
    lua_pushboolean(L, 0 == result);
    return 1;
}

/**
 * \brief Get the thresholds for bg/fg in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 factor
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack (result)
 */
static int
ThresholdForFgBg(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 2, 1);
    l_int32 thresh = ll_check_l_int32_default(__func__, L, 3, 0);
    l_int32 fgval;
    l_int32 bgval;
    if (pixThresholdForFgBg(pixs, factor, thresh, &fgval, &bgval))
        return ll_push_nil(L);
    lua_pushinteger(L, fgval);
    lua_pushinteger(L, bgval);
    return 2;
}

/**
 * \brief Get the thresholds for bg/fg in Pix* (%pixs)
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs)
 * Arg #2 is expected to be a l_int32 factor
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack (result)
 */
static int
SplitDistributionFgBg(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_float32 scorefract = ll_check_l_float32_default(__func__, L, 2, 0.0f);
    l_int32 factor = ll_check_l_int32_default(__func__, L, 3, 1);
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

static int
FindAreaPerimRatio(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
    l_float32 fract = 0.0f;
    if (pixFindAreaPerimRatio(pixs, tab8, &fract))
        return ll_push_nil(L);
    lua_pushnumber(L, (lua_Number)fract);
    return 1;
}

static int
FindPerimToAreaRatio(lua_State *L)
{
    Pix *pixs = ll_check_Pix(L, 1);
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
 * \brief Read new Pix*
 *
 * Arg #1 is expected to be a string (filename)
 *
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
Read(lua_State *L)
{
    const char* filename = lua_tostring(L, 1);
    Pix *pix = pixRead(filename);
    return ll_push_Pix(L, pix);
}

/**
 * \brief Write the Pix* to a file
 *
 * Arg #1 (i.e. self) is expected to be a Pix* (pix)
 * Arg #2 is expected to be a string (filename)
 * Arg #3 is expected to be a string with the input format name (format)
 *
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    Pix *pix = ll_check_Pix(L, 1);
    const char *filename = lua_tostring(L, 2);
    l_int32 format = ll_check_input_format(__func__, L, 3, IFF_DEFAULT);
    lua_pushboolean(L, 0 == pixWrite(filename, pix, format));
    return 1;
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
