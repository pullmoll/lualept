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
 * \file llpixcomp.cpp
 * \class PixComp
 *
 * A class to handle compressed Pix.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXCOMP

/** Define a function's name (_fun) with prefix PixComp */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a PixComp*.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PixComp *pixc = ll_take_udata<PixComp>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "pixc", reinterpret_cast<void *>(pixc));
    pixcompDestroy(&pixc);
    return 0;
}

/**
 * \brief Printable string for a PixComp*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixColormap* (cmaps).
 * </pre>
 * \param L Lua state
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    luaL_Buffer B;
    l_int32 w, h, d;
    l_int32 xres, yres, comptype, cmapflag;

    luaL_buffinit(L, &B);

    if (!pixc) {
        luaL_addstring(&B, "nil");
    } else if (pixcompGetDimensions(pixc, &w, &h, &d)) {
        luaL_addstring(&B, "invalid");
    } else {
        pixcompGetParameters(pixc, &xres, &yres, &comptype, &cmapflag);
        snprintf(str, LL_STRBUFF,
                 TNAME ": %p",
                 reinterpret_cast<void *>(pixc));
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    width = %d, height = %d, depth = %d\n",
                 w, h, d);
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    compression = %s\n",
                 ll_string_compression(comptype));
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    data = %p, size = %#" PRIx64 "\n",
                 reinterpret_cast<void *>(pixc->data), pixc->size);
        luaL_addstring(&B, str);

        snprintf(str, LL_STRBUFF,
                 "    %scolormap\n", cmapflag ? "" : "no ");
        luaL_addstring(&B, str);

        if (pixc->text) {
            snprintf(str, LL_STRBUFF,
                     "    text: %s", pixc->text);
        } else {
            snprintf(str, LL_STRBUFF,
                     "    no text");
        }
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Copy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixComp* (pixcs).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    PixComp *pixcs = ll_check_PixComp(_fun, L, 1);
    PixComp *pixc = pixcompCopy(pixcs);
    return ll_push_PixComp(_fun, L, pixc);
}

/**
 * \brief CreateFromFile() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (filename).
 * Arg #2 is expected to be a l_int32 (comptype).
 *
 * Leptonica's Notes:
 *      (1) Use %comptype == IFF_DEFAULT to have the compression
 *          type automatically determined.
 *      (2) If the comptype is invalid for this file, the default will
 *          be substituted.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CreateFromFile(lua_State *L)
{
    LL_FUNC("CreateFromFile");
    const char *filename = ll_check_string(_fun, L, 1);
    l_int32 comptype = ll_check_compression(_fun, L, 2);
    PixComp *pixc = pixcompCreateFromFile(filename, comptype);
    return ll_push_PixComp(_fun, L, pixc);
}

/**
 * \brief Create a new PixComp*.
 *
 * Arg #1 is expected to be a l_int32 (width).
 * Arg #2 is expected to be a l_int32 (height).
 *
 * \param L Lua state
 * \return 1 PixComp* on the Lua stack
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix* pix = ll_check_Pix(_fun, L, 1);
    l_int32 comptype = ll_check_compression(_fun, L, 2, 1);
    PixComp *pixcomp = pixcompCreateFromPix(pix, comptype);
    return ll_push_PixComp(_fun, L, pixcomp);
}

/**
 * \brief CreateFromString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (data, size).
 *
 * Leptonica's Notes:
 *      (1) This works when the compressed string is png, jpeg or tiffg4.
 *      (2) The copyflag determines if the data in the new Pixcomp is
 *          a copy of the input data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CreateFromString(lua_State *L)
{
    LL_FUNC("CreateFromString");
    size_t size = 0;
    const l_uint8 *cdata = ll_check_lbytes(_fun, L, 1, &size);
    l_int32 copyflag = L_INSERT;
    l_uint8 *data = ll_malloc<l_uint8>(_fun, L, size);
    PixComp *pixc = nullptr;
    memcpy(data, cdata, size);
    pixc = pixcompCreateFromString(data, size, copyflag);
    return ll_push_PixComp(_fun, L, pixc);
}

/**
 * \brief DetermineFormat() brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (comptype).
 * Arg #2 is expected to be a l_int32 (d).
 * Arg #3 is expected to be a l_int32 (cmapflag).
 *
 * Leptonica's Notes:
 *      (1) This determines the best format for a pix, given both
 *          the request (%comptype) and the image characteristics.
 *      (2) If %comptype == IFF_DEFAULT, this does not necessarily result
 *          in png encoding.  Instead, it returns one of the three formats
 *          that is both valid and most likely to give best compression.
 *      (3) If the pix cannot be compressed by the input value of
 *          %comptype, this selects IFF_PNG, which can compress all pix.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
DetermineFormat(lua_State *L)
{
    LL_FUNC("DetermineFormat");
    l_int32 comptype = ll_check_l_int32(_fun, L, 1);
    l_int32 d = ll_check_l_int32(_fun, L, 2);
    l_int32 cmapflag = ll_check_l_int32(_fun, L, 3);
    l_int32 format = 0;
    if (pixcompDetermineFormat(comptype, d, cmapflag, &format))
        return ll_push_nil(L);
    ll_push_string(_fun, L, ll_string_input_format(format));
    return 1;
}

/**
 * \brief GetDimensions() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixComp* (pixc).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 on the Lua stack
 */
static int
GetDimensions(lua_State *L)
{
    LL_FUNC("GetDimensions");
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    l_int32 w = 0;
    l_int32 h = 0;
    l_int32 d = 0;
    if (pixcompGetDimensions(pixc, &w, &h, &d))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    ll_push_l_int32(_fun, L, d);
    return 3;
}

/**
 * \brief GetParameters() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixComp* (pixc).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 on the Lua stack
 */
static int
GetParameters(lua_State *L)
{
    LL_FUNC("GetParameters");
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    l_int32 xres = 0;
    l_int32 yres = 0;
    l_int32 comptype = 0;
    l_int32 cmapflag = 0;
    if (pixcompGetParameters(pixc, &xres, &yres, &comptype, &cmapflag))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xres);
    ll_push_l_int32(_fun, L, yres);
    ll_push_l_int32(_fun, L, comptype);
    ll_push_l_int32(_fun, L, cmapflag);
    return 4;
}

/**
 * \brief WriteFile() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixc).
 * Arg #2 is expected to be a const char* (rootname).
 *
 * Leptonica's Notes:
 *      (1) The compressed data is written to file, and the filename is
 *          generated by appending the format extension to %rootname.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
WriteFile(lua_State *L)
{
    LL_FUNC("WriteFile");
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    const char *rootname = ll_check_string(_fun, L, 2);
    l_ok ok = pixcompWriteFile(rootname, pixc);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief WriteStreamInfo() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixc).
 * Arg #2 is expected to be a luaL_Stream* (stream->f).
 * Arg #3 is expected to be a const char* (text).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
WriteStreamInfo(lua_State *L)
{
    LL_FUNC("WriteStreamInfo");
    PixComp *pixc = ll_check_PixComp(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    const char *text = ll_opt_string(_fun, L, 3, "");
    l_ok ok = pixcompWriteStreamInfo(stream->f, pixc, text);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class PixComp*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixComp* contained in the user data
 */
PixComp *
ll_check_PixComp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PixComp>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a PixComp* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PixComp* contained in the user data
 */
PixComp *
ll_opt_PixComp(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_PixComp(_fun, L, arg);
}

/**
 * \brief Push PixComp* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param pixcomp pointer to the PixComp
 * \return 1 PixComp* on the Lua stack
 */
int
ll_push_PixComp(const char *_fun, lua_State *L, PixComp *pixcomp)
{
    if (!pixcomp)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, pixcomp);
}

/**
 * \brief Create and push a new PixComp*.
 * \param L Lua state
 * \return 1 PixComp* on the Lua stack
 */
int
ll_new_PixComp(lua_State *L)
{
    FUNC("ll_new_PixComp");
    PixComp *pixcomp = nullptr;
    Pix* pix = nullptr;
    const char *filename = nullptr;
    const l_uint8 *data = nullptr;
    size_t size = 0;
    l_int32 comptype = IFF_DEFAULT;
    l_int32 copyflag = L_COPY;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        pix = ll_opt_Pix(_fun, L, 1);
        comptype = ll_check_compression(_fun, L, 2);
        pixcomp = pixcompCreateFromPix(pix, comptype);
    }

    if (!pixcomp && ll_isstring(_fun, L, 1)) {
        filename = ll_opt_string(_fun, L, 1);
        comptype = ll_check_compression(_fun, L, 2);
        pixcomp = pixcompCreateFromFile(filename, comptype);
    }

    if (!pixcomp && ll_isstring(_fun, L, 1)) {
        data = ll_check_lbytes(_fun, L, 1, &size);
        /* XXX: deconstify */
        l_uint8 *data2 = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(data));
        copyflag = ll_check_access_storage(_fun, L, 2, copyflag);
        pixcomp = pixcompCreateFromString(data2, size, copyflag);
    }

    return ll_push_PixComp(_fun, L, pixcomp);
}

/**
 * \brief Register the PixComp methods and functions in the PixComp meta table.
 * \param L Lua state
 * \return 1 table on the Lua stack
 */
int
ll_open_PixComp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_PixComp},
        {"__tostring",          toString},
        {"Copy",                Copy},
        {"CreateFromFile",      CreateFromFile},
        {"CreateFromPix",       CreateFromPix},
        {"CreateFromString",    CreateFromString},
        {"Destroy",             Destroy},
        {"DetermineFormat",     DetermineFormat},
        {"GetDimensions",       GetDimensions},
        {"GetParameters",       GetParameters},
        {"WriteFile",           WriteFile},
        {"WriteStreamInfo",     WriteStreamInfo},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_PixComp);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
